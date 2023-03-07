// Copyright 2020 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#define PW_LOG_MODULE_NAME "KVS"
#define PW_LOG_LEVEL PW_KVS_LOG_LEVEL

#include "pw_kvs/key_value_store.h"

#include <algorithm>
#include <cinttypes>
#include <cstring>
#include <type_traits>

#include "pw_assert/check.h"
#include "pw_kvs_private/config.h"
#include "pw_log/shorter.h"
#include "pw_status/try.h"

namespace pw::kvs {
namespace {

using std::byte;

constexpr bool InvalidKey(Key key) {
  return key.empty() || (key.size() > internal::Entry::kMaxKeyLength);
}

}  // namespace

KeyValueStore::KeyValueStore(FlashPartition* partition,
                             span<const EntryFormat> formats,
                             const Options& options,
                             size_t redundancy,
                             Vector<SectorDescriptor>& sector_descriptor_list,
                             const SectorDescriptor** temp_sectors_to_skip,
                             Vector<KeyDescriptor>& key_descriptor_list,
                             Address* addresses)
    : partition_(*partition),
      formats_(formats),
      sectors_(sector_descriptor_list, *partition, temp_sectors_to_skip),
      entry_cache_(key_descriptor_list, addresses, redundancy),
      options_(options),
      initialized_(InitializationState::kNotInitialized),
      error_detected_(false),
      internal_stats_({}),
      last_transaction_id_(0) {}

Status KeyValueStore::Init() {
  initialized_ = InitializationState::kNotInitialized;
  error_detected_ = false;
  last_transaction_id_ = 0;

  INF("Initializing key value store");
  if (partition_.sector_count() > sectors_.max_size()) {
    ERR("KVS init failed: kMaxUsableSectors (=%u) must be at least as "
        "large as the number of sectors in the flash partition (=%u)",
        unsigned(sectors_.max_size()),
        unsigned(partition_.sector_count()));
    return Status::FailedPrecondition();
  }

  if (partition_.sector_count() < 2) {
    ERR("KVS init failed: FlashParition sector count (=%u) must be at 2. KVS "
        "requires at least 1 working sector + 1 free/reserved sector",
        unsigned(partition_.sector_count()));
    return Status::FailedPrecondition();
  }

  const size_t sector_size_bytes = partition_.sector_size_bytes();

  // TODO(davidrogers): investigate doing this as a static assert/compile-time
  // check.
  if (sector_size_bytes > SectorDescriptor::max_sector_size()) {
    ERR("KVS init failed: sector_size_bytes (=%u) is greater than maximum "
        "allowed sector size (=%u)",
        unsigned(sector_size_bytes),
        unsigned(SectorDescriptor::max_sector_size()));
    return Status::FailedPrecondition();
  }

  Status metadata_result = InitializeMetadata();

  if (!error_detected_) {
    initialized_ = InitializationState::kReady;
  } else {
    initialized_ = InitializationState::kNeedsMaintenance;

    if (options_.recovery != ErrorRecovery::kManual) {
      size_t pre_fix_redundancy_errors =
          internal_stats_.missing_redundant_entries_recovered;
      Status recovery_status = FixErrors();

      if (recovery_status.ok()) {
        if (metadata_result.IsOutOfRange()) {
          internal_stats_.missing_redundant_entries_recovered =
              pre_fix_redundancy_errors;
          INF("KVS init: Redundancy level successfully updated");
        } else {
          WRN("KVS init: Corruption detected and fully repaired");
        }
        initialized_ = InitializationState::kReady;
      } else if (recovery_status.IsResourceExhausted()) {
        WRN("KVS init: Unable to maintain required free sector");
      } else {
        WRN("KVS init: Corruption detected and unable repair");
      }
    } else {
      WRN("KVS init: Corruption detected, no repair attempted due to options");
    }
  }

  INF("KeyValueStore init complete: active keys %u, deleted keys %u, sectors "
      "%u, logical sector size %u bytes",
      unsigned(size()),
      unsigned(entry_cache_.total_entries() - size()),
      unsigned(sectors_.size()),
      unsigned(partition_.sector_size_bytes()));

  // Report any corruption was not repaired.
  if (error_detected_) {
    WRN("KVS init: Corruption found but not repaired, KVS unavailable until "
        "successful maintenance.");
    return Status::DataLoss();
  }

  return OkStatus();
}

Status KeyValueStore::InitializeMetadata() {
  const size_t sector_size_bytes = partition_.sector_size_bytes();

  sectors_.Reset();
  entry_cache_.Reset();

  DBG("First pass: Read all entries from all sectors");
  Address sector_address = 0;

  size_t total_corrupt_bytes = 0;
  size_t corrupt_entries = 0;
  bool empty_sector_found = false;
  size_t entry_copies_missing = 0;

  for (SectorDescriptor& sector : sectors_) {
    Address entry_address = sector_address;

    size_t sector_corrupt_bytes = 0;

    for (int num_entries_in_sector = 0; true; num_entries_in_sector++) {
      DBG("Load entry: sector=%u, entry#=%d, address=%u",
          unsigned(sector_address),
          num_entries_in_sector,
          unsigned(entry_address));

      if (!sectors_.AddressInSector(sector, entry_address)) {
        DBG("Fell off end of sector; moving to the next sector");
        break;
      }

      Address next_entry_address;
      Status status = LoadEntry(entry_address, &next_entry_address);
      if (status.IsNotFound()) {
        DBG("Hit un-written data in sector; moving to the next sector");
        break;
      } else if (!status.ok()) {
        // The entry could not be read, indicating likely data corruption within
        // the sector. Try to scan the remainder of the sector for other
        // entries.

        error_detected_ = true;
        corrupt_entries++;

        status = ScanForEntry(sector,
                              entry_address + Entry::kMinAlignmentBytes,
                              &next_entry_address);
        if (!status.ok()) {
          // No further entries in this sector. Mark the remaining bytes in the
          // sector as corrupt (since we can't reliably know the size of the
          // corrupt entry).
          sector_corrupt_bytes +=
              sector_size_bytes - (entry_address - sector_address);
          break;
        }

        sector_corrupt_bytes += next_entry_address - entry_address;
      }

      // Entry loaded successfully; so get ready to load the next one.
      entry_address = next_entry_address;

      // Update of the number of writable bytes in this sector.
      sector.set_writable_bytes(sector_size_bytes -
                                (entry_address - sector_address));
    }

    if (sector_corrupt_bytes > 0) {
      // If the sector contains corrupt data, prevent any further entries from
      // being written to it by indicating that it has no space. This should
      // also make it a decent GC candidate. Valid keys in the sector are still
      // readable as normal.
      sector.mark_corrupt();
      error_detected_ = true;

      WRN("Sector %u contains %uB of corrupt data",
          sectors_.Index(sector),
          unsigned(sector_corrupt_bytes));
    }

    if (sector.Empty(sector_size_bytes)) {
      empty_sector_found = true;
    }
    sector_address += sector_size_bytes;
    total_corrupt_bytes += sector_corrupt_bytes;
  }

  DBG("Second pass: Count valid bytes in each sector");
  Address newest_key = 0;

  // For every valid entry, for each address, count the valid bytes in that
  // sector. If the address fails to read, remove the address and mark the
  // sector as corrupt. Track which entry has the newest transaction ID for
  // initializing last_new_sector_.
  for (EntryMetadata& metadata : entry_cache_) {
    if (metadata.addresses().size() < redundancy()) {
      DBG("Key 0x%08x missing copies, has %u, needs %u",
          unsigned(metadata.hash()),
          unsigned(metadata.addresses().size()),
          unsigned(redundancy()));
      entry_copies_missing++;
    }
    size_t index = 0;
    while (index < metadata.addresses().size()) {
      Address address = metadata.addresses()[index];
      Entry entry;

      Status read_result = Entry::Read(partition_, address, formats_, &entry);

      SectorDescriptor& sector = sectors_.FromAddress(address);

      if (read_result.ok()) {
        sector.AddValidBytes(entry.size());
        index++;
      } else {
        corrupt_entries++;
        total_corrupt_bytes += sector.writable_bytes();
        error_detected_ = true;
        sector.mark_corrupt();

        // Remove the bad address and stay at this index. The removal
        // replaces out the removed address with the back address so
        // this index needs to be rechecked with the new address.
        metadata.RemoveAddress(address);
      }
    }

    if (metadata.IsNewerThan(last_transaction_id_)) {
      last_transaction_id_ = metadata.transaction_id();
      newest_key = metadata.addresses().back();
    }
  }

  sectors_.set_last_new_sector(newest_key);

  if (!empty_sector_found) {
    DBG("No empty sector found");
    error_detected_ = true;
  }

  if (entry_copies_missing > 0) {
    bool other_errors = error_detected_;
    error_detected_ = true;

    if (!other_errors && entry_copies_missing == entry_cache_.total_entries()) {
      INF("KVS configuration changed to redundancy of %u total copies per key",
          unsigned(redundancy()));
      return Status::OutOfRange();
    }
  }

  if (error_detected_) {
    WRN("Corruption detected. Found %u corrupt bytes, %u corrupt entries, "
        "and %u keys missing redundant copies.",
        unsigned(total_corrupt_bytes),
        unsigned(corrupt_entries),
        unsigned(entry_copies_missing));
    return Status::FailedPrecondition();
  }
  return OkStatus();
}

KeyValueStore::StorageStats KeyValueStore::GetStorageStats() const {
  StorageStats stats{};
  const size_t sector_size = partition_.sector_size_bytes();
  bool found_empty_sector = false;
  stats.sector_erase_count = internal_stats_.sector_erase_count;
  stats.corrupt_sectors_recovered = internal_stats_.corrupt_sectors_recovered;
  stats.missing_redundant_entries_recovered =
      internal_stats_.missing_redundant_entries_recovered;

  for (const SectorDescriptor& sector : sectors_) {
    stats.in_use_bytes += sector.valid_bytes();
    stats.reclaimable_bytes += sector.RecoverableBytes(sector_size);

    if (!found_empty_sector && sector.Empty(sector_size)) {
      // The KVS tries to always keep an empty sector for GC, so don't count
      // the first empty sector seen as writable space. However, a free sector
      // cannot always be assumed to exist; if a GC operation fails, all sectors
      // may be partially written, in which case the space reported might be
      // inaccurate.
      found_empty_sector = true;
      continue;
    }

    stats.writable_bytes += sector.writable_bytes();
  }

  return stats;
}

// Check KVS for any error conditions. Primarily intended for test and
// internal use.
bool KeyValueStore::CheckForErrors() {
  // Check for corrupted sectors
  for (SectorDescriptor& sector : sectors_) {
    if (sector.corrupt()) {
      error_detected_ = true;
      return error_detected();
    }
  }

  // Check for missing redundancy.
  if (redundancy() > 1) {
    for (const EntryMetadata& metadata : entry_cache_) {
      if (metadata.addresses().size() < redundancy()) {
        error_detected_ = true;
        return error_detected();
      }
    }
  }

  return error_detected();
}

Status KeyValueStore::LoadEntry(Address entry_address,
                                Address* next_entry_address) {
  Entry entry;
  PW_TRY(Entry::Read(partition_, entry_address, formats_, &entry));

  // Read the key from flash & validate the entry (which reads the value).
  Entry::KeyBuffer key_buffer;
  PW_TRY_ASSIGN(size_t key_length, entry.ReadKey(key_buffer));
  const Key key(key_buffer.data(), key_length);

  PW_TRY(entry.VerifyChecksumInFlash());

  // A valid entry was found, so update the next entry address before doing any
  // of the checks that happen in AddNewOrUpdateExisting.
  *next_entry_address = entry.next_address();
  return entry_cache_.AddNewOrUpdateExisting(
      entry.descriptor(key), entry.address(), partition_.sector_size_bytes());
}

// Scans flash memory within a sector to find a KVS entry magic.
Status KeyValueStore::ScanForEntry(const SectorDescriptor& sector,
                                   Address start_address,
                                   Address* next_entry_address) {
  DBG("Scanning sector %u for entries starting from address %u",
      sectors_.Index(sector),
      unsigned(start_address));

  // Entries must start at addresses which are aligned on a multiple of
  // Entry::kMinAlignmentBytes. However, that multiple can vary between entries.
  // When scanning, we don't have an entry to tell us what the current alignment
  // is, so the minimum alignment is used to be exhaustive.
  for (Address address = AlignUp(start_address, Entry::kMinAlignmentBytes);
       sectors_.AddressInSector(sector, address);
       address += Entry::kMinAlignmentBytes) {
    uint32_t magic;
    StatusWithSize read_result =
        partition_.Read(address, as_writable_bytes(span(&magic, 1)));
    if (!read_result.ok()) {
      continue;
    }
    if (formats_.KnownMagic(magic)) {
      DBG("Found entry magic at address %u", unsigned(address));
      *next_entry_address = address;
      return OkStatus();
    }
  }

  return Status::NotFound();
}

#if PW_KVS_REMOVE_DELETED_KEYS_IN_HEAVY_MAINTENANCE

Status KeyValueStore::RemoveDeletedKeyEntries() {
  for (internal::EntryCache::iterator it = entry_cache_.begin();
       it != entry_cache_.end();
       ++it) {
    EntryMetadata& entry_metadata = *it;

    // The iterator we are given back from RemoveEntry could also be deleted,
    // so loop until we find one that isn't deleted.
    while (entry_metadata.state() == EntryState::kDeleted) {
      // Read the original entry to get the size for sector accounting purposes.
      Entry entry;
      PW_TRY(ReadEntry(entry_metadata, entry));

      for (Address address : entry_metadata.addresses()) {
        sectors_.FromAddress(address).RemoveValidBytes(entry.size());
      }

      it = entry_cache_.RemoveEntry(it);

      if (it == entry_cache_.end()) {
        return OkStatus();  // new iterator is the end, bail
      }

      entry_metadata = *it;  // set entry_metadata to check for deletion again
    }
  }

  return OkStatus();
}

#endif  // PW_KVS_REMOVE_DELETED_KEYS_IN_HEAVY_MAINTENANCE

StatusWithSize KeyValueStore::Get(Key key,
                                  span<byte> value_buffer,
                                  size_t offset_bytes) const {
  PW_TRY_WITH_SIZE(CheckReadOperation(key));

  EntryMetadata metadata;
  PW_TRY_WITH_SIZE(FindExisting(key, &metadata));

  return Get(key, metadata, value_buffer, offset_bytes);
}

Status KeyValueStore::PutBytes(Key key, span<const byte> value) {
  PW_TRY(CheckWriteOperation(key));
  DBG("Writing key/value; key length=%u, value length=%u",
      unsigned(key.size()),
      unsigned(value.size()));

  if (Entry::size(partition_, key, value) > partition_.sector_size_bytes()) {
    DBG("%u B value with %u B key cannot fit in one sector",
        unsigned(value.size()),
        unsigned(key.size()));
    return Status::InvalidArgument();
  }

  EntryMetadata metadata;
  Status status = FindEntry(key, &metadata);

  if (status.ok()) {
    // TODO(davidrogers): figure out logging how to support multiple addresses.
    DBG("Overwriting entry for key 0x%08x in %u sectors including %u",
        unsigned(metadata.hash()),
        unsigned(metadata.addresses().size()),
        sectors_.Index(metadata.first_address()));
    return WriteEntryForExistingKey(metadata, EntryState::kValid, key, value);
  }

  if (status.IsNotFound()) {
    return WriteEntryForNewKey(key, value);
  }

  return status;
}

Status KeyValueStore::Delete(Key key) {
  PW_TRY(CheckWriteOperation(key));

  EntryMetadata metadata;
  PW_TRY(FindExisting(key, &metadata));

  // TODO(davidrogers): figure out logging how to support multiple addresses.
  DBG("Writing tombstone for key 0x%08x in %u sectors including %u",
      unsigned(metadata.hash()),
      unsigned(metadata.addresses().size()),
      sectors_.Index(metadata.first_address()));
  return WriteEntryForExistingKey(metadata, EntryState::kDeleted, key, {});
}

void KeyValueStore::Item::ReadKey() {
  key_buffer_.fill('\0');

  Entry entry;
  if (kvs_.ReadEntry(*iterator_, entry).ok()) {
    entry.ReadKey(key_buffer_)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
  }
}

KeyValueStore::iterator& KeyValueStore::iterator::operator++() {
  // Skip to the next entry that is valid (not deleted).
  while (++item_.iterator_ != item_.kvs_.entry_cache_.end() &&
         item_.iterator_->state() != EntryState::kValid) {
  }
  return *this;
}

KeyValueStore::iterator KeyValueStore::begin() const {
  internal::EntryCache::const_iterator cache_iterator = entry_cache_.begin();
  // Skip over any deleted entries at the start of the descriptor list.
  while (cache_iterator != entry_cache_.end() &&
         cache_iterator->state() != EntryState::kValid) {
    ++cache_iterator;
  }
  return iterator(*this, cache_iterator);
}

StatusWithSize KeyValueStore::ValueSize(Key key) const {
  PW_TRY_WITH_SIZE(CheckReadOperation(key));

  EntryMetadata metadata;
  PW_TRY_WITH_SIZE(FindExisting(key, &metadata));

  return ValueSize(metadata);
}

Status KeyValueStore::ReadEntry(const EntryMetadata& metadata,
                                Entry& entry) const {
  // Try to read an entry
  Status read_result = Status::DataLoss();
  for (Address address : metadata.addresses()) {
    read_result = Entry::Read(partition_, address, formats_, &entry);
    if (read_result.ok()) {
      return read_result;
    }

    // Found a bad address. Set the sector as corrupt.
    error_detected_ = true;
    sectors_.FromAddress(address).mark_corrupt();
  }

  ERR("No valid entries for key. Data has been lost!");
  return read_result;
}

Status KeyValueStore::FindEntry(Key key, EntryMetadata* metadata_out) const {
  StatusWithSize find_result =
      entry_cache_.Find(partition_, sectors_, formats_, key, metadata_out);

  if (find_result.size() > 0u) {
    error_detected_ = true;
  }
  return find_result.status();
}

Status KeyValueStore::FindExisting(Key key, EntryMetadata* metadata_out) const {
  Status status = FindEntry(key, metadata_out);

  // If the key's hash collides with an existing key or if the key is deleted,
  // treat it as if it is not in the KVS.
  if (status.IsAlreadyExists() ||
      (status.ok() && metadata_out->state() == EntryState::kDeleted)) {
    return Status::NotFound();
  }
  return status;
}

StatusWithSize KeyValueStore::Get(Key key,
                                  const EntryMetadata& metadata,
                                  span<std::byte> value_buffer,
                                  size_t offset_bytes) const {
  Entry entry;

  PW_TRY_WITH_SIZE(ReadEntry(metadata, entry));

  StatusWithSize result = entry.ReadValue(value_buffer, offset_bytes);
  if (result.ok() && options_.verify_on_read && offset_bytes == 0u) {
    Status verify_result =
        entry.VerifyChecksum(key, value_buffer.first(result.size()));
    if (!verify_result.ok()) {
      std::memset(value_buffer.data(), 0, result.size());
      return StatusWithSize(verify_result, 0);
    }

    return StatusWithSize(verify_result, result.size());
  }
  return result;
}

Status KeyValueStore::FixedSizeGet(Key key,
                                   void* value,
                                   size_t size_bytes) const {
  PW_TRY(CheckWriteOperation(key));

  EntryMetadata metadata;
  PW_TRY(FindExisting(key, &metadata));

  return FixedSizeGet(key, metadata, value, size_bytes);
}

Status KeyValueStore::FixedSizeGet(Key key,
                                   const EntryMetadata& metadata,
                                   void* value,
                                   size_t size_bytes) const {
  // Ensure that the size of the stored value matches the size of the type.
  // Otherwise, report error. This check avoids potential memory corruption.
  PW_TRY_ASSIGN(const size_t actual_size, ValueSize(metadata));

  if (actual_size != size_bytes) {
    DBG("Requested %u B read, but value is %u B",
        unsigned(size_bytes),
        unsigned(actual_size));
    return Status::InvalidArgument();
  }

  StatusWithSize result =
      Get(key, metadata, span(static_cast<byte*>(value), size_bytes), 0);

  return result.status();
}

StatusWithSize KeyValueStore::ValueSize(const EntryMetadata& metadata) const {
  Entry entry;
  PW_TRY_WITH_SIZE(ReadEntry(metadata, entry));

  return StatusWithSize(entry.value_size());
}

Status KeyValueStore::CheckWriteOperation(Key key) const {
  if (InvalidKey(key)) {
    return Status::InvalidArgument();
  }

  // For normal write operation the KVS must be fully ready.
  if (!initialized()) {
    return Status::FailedPrecondition();
  }
  return OkStatus();
}

Status KeyValueStore::CheckReadOperation(Key key) const {
  if (InvalidKey(key)) {
    return Status::InvalidArgument();
  }

  // Operations that are explicitly read-only can be done after init() has been
  // called but not fully ready (when needing maintenance).
  if (initialized_ == InitializationState::kNotInitialized) {
    return Status::FailedPrecondition();
  }
  return OkStatus();
}

Status KeyValueStore::WriteEntryForExistingKey(EntryMetadata& metadata,
                                               EntryState new_state,
                                               Key key,
                                               span<const byte> value) {
  // Read the original entry to get the size for sector accounting purposes.
  Entry entry;
  PW_TRY(ReadEntry(metadata, entry));

  return WriteEntry(key, value, new_state, &metadata, &entry);
}

Status KeyValueStore::WriteEntryForNewKey(Key key, span<const byte> value) {
  // If there is no room in the cache for a new entry, it is possible some cache
  // entries could be freed by removing deleted keys. If deleted key removal is
  // enabled and the KVS is configured to make all possible writes succeed,
  // attempt heavy maintenance now.
#if PW_KVS_REMOVE_DELETED_KEYS_IN_HEAVY_MAINTENANCE
  if (options_.gc_on_write == GargbageCollectOnWrite::kAsManySectorsNeeded &&
      entry_cache_.full()) {
    Status maintenance_status = HeavyMaintenance();
    if (!maintenance_status.ok()) {
      WRN("KVS Maintenance failed for write: %s", maintenance_status.str());
      return maintenance_status;
    }
  }
#endif  // PW_KVS_REMOVE_DELETED_KEYS_IN_HEAVY_MAINTENANCE

  if (entry_cache_.full()) {
    WRN("KVS full: trying to store a new entry, but can't. Have %u entries",
        unsigned(entry_cache_.total_entries()));
    return Status::ResourceExhausted();
  }

  return WriteEntry(key, value, EntryState::kValid);
}

Status KeyValueStore::WriteEntry(Key key,
                                 span<const byte> value,
                                 EntryState new_state,
                                 EntryMetadata* prior_metadata,
                                 const Entry* prior_entry) {
  // If new entry and prior entry have matching value size, state, and checksum,
  // check if the values match. Directly compare the prior and new values
  // because the checksum can not be depended on to establish equality, it can
  // only be depended on to establish inequality.
  if (prior_entry != nullptr && prior_entry->value_size() == value.size() &&
      prior_metadata->state() == new_state &&
      prior_entry->ValueMatches(value).ok()) {
    // The new value matches the prior value, don't need to write anything. Just
    // keep the existing entry.
    DBG("Write for key 0x%08x with matching value skipped",
        unsigned(prior_metadata->hash()));
    return OkStatus();
  }

  // List of addresses for sectors with space for this entry.
  Address* reserved_addresses = entry_cache_.TempReservedAddressesForWrite();

  // Find addresses to write the entry to. This may involve garbage collecting
  // one or more sectors.
  const size_t entry_size = Entry::size(partition_, key, value);
  PW_TRY(GetAddressesForWrite(reserved_addresses, entry_size));

  // Write the entry at the first address that was found.
  Entry entry = CreateEntry(reserved_addresses[0], key, value, new_state);
  PW_TRY(AppendEntry(entry, key, value));

  // After writing the first entry successfully, update the key descriptors.
  // Once a single new the entry is written, the old entries are invalidated.
  size_t prior_size = prior_entry != nullptr ? prior_entry->size() : 0;
  EntryMetadata new_metadata =
      CreateOrUpdateKeyDescriptor(entry, key, prior_metadata, prior_size);

  // Write the additional copies of the entry, if redundancy is greater than 1.
  for (size_t i = 1; i < redundancy(); ++i) {
    entry.set_address(reserved_addresses[i]);
    PW_TRY(AppendEntry(entry, key, value));
    new_metadata.AddNewAddress(reserved_addresses[i]);
  }
  return OkStatus();
}

KeyValueStore::EntryMetadata KeyValueStore::CreateOrUpdateKeyDescriptor(
    const Entry& entry,
    Key key,
    EntryMetadata* prior_metadata,
    size_t prior_size) {
  // If there is no prior descriptor, create a new one.
  if (prior_metadata == nullptr) {
    return entry_cache_.AddNew(entry.descriptor(key), entry.address());
  }

  return UpdateKeyDescriptor(
      entry, entry.address(), prior_metadata, prior_size);
}

KeyValueStore::EntryMetadata KeyValueStore::UpdateKeyDescriptor(
    const Entry& entry,
    Address new_address,
    EntryMetadata* prior_metadata,
    size_t prior_size) {
  // Remove valid bytes for the old entry and its copies, which are now stale.
  for (Address address : prior_metadata->addresses()) {
    sectors_.FromAddress(address).RemoveValidBytes(prior_size);
  }

  prior_metadata->Reset(entry.descriptor(prior_metadata->hash()), new_address);
  return *prior_metadata;
}

Status KeyValueStore::GetAddressesForWrite(Address* write_addresses,
                                           size_t write_size) {
  for (size_t i = 0; i < redundancy(); i++) {
    SectorDescriptor* sector;
    PW_TRY(GetSectorForWrite(&sector, write_size, span(write_addresses, i)));
    write_addresses[i] = sectors_.NextWritableAddress(*sector);

    DBG("Found space for entry in sector %u at address %u",
        sectors_.Index(sector),
        unsigned(write_addresses[i]));
  }

  return OkStatus();
}

// Finds a sector to use for writing a new entry to. Does automatic garbage
// collection if needed and allowed.
//
//                 OK: Sector found with needed space.
// RESOURCE_EXHAUSTED: No sector available with the needed space.
Status KeyValueStore::GetSectorForWrite(
    SectorDescriptor** sector,
    size_t entry_size,
    span<const Address> reserved_addresses) {
  Status result = sectors_.FindSpace(sector, entry_size, reserved_addresses);

  size_t gc_sector_count = 0;
  bool do_auto_gc = options_.gc_on_write != GargbageCollectOnWrite::kDisabled;

  // Do garbage collection as needed, so long as policy allows.
  while (result.IsResourceExhausted() && do_auto_gc) {
    if (options_.gc_on_write == GargbageCollectOnWrite::kOneSector) {
      // If GC config option is kOneSector clear the flag to not do any more
      // GC after this try.
      do_auto_gc = false;
    }
    // Garbage collect and then try again to find the best sector.
    Status gc_status = GarbageCollect(reserved_addresses);
    if (!gc_status.ok()) {
      if (gc_status.IsNotFound()) {
        // Not enough space, and no reclaimable bytes, this KVS is full!
        return Status::ResourceExhausted();
      }
      return gc_status;
    }

    result = sectors_.FindSpace(sector, entry_size, reserved_addresses);

    gc_sector_count++;
    // Allow total sectors + 2 number of GC cycles so that once reclaimable
    // bytes in all the sectors have been reclaimed can try and free up space by
    // moving entries for keys other than the one being worked on in to sectors
    // that have copies of the key trying to be written.
    if (gc_sector_count > (partition_.sector_count() + 2)) {
      ERR("Did more GC sectors than total sectors!!!!");
      return Status::ResourceExhausted();
    }
  }

  if (!result.ok()) {
    WRN("Unable to find sector to write %u B", unsigned(entry_size));
  }
  return result;
}

Status KeyValueStore::MarkSectorCorruptIfNotOk(Status status,
                                               SectorDescriptor* sector) {
  if (!status.ok()) {
    DBG("  Sector %u corrupt", sectors_.Index(sector));
    sector->mark_corrupt();
    error_detected_ = true;
  }
  return status;
}

Status KeyValueStore::AppendEntry(const Entry& entry,
                                  Key key,
                                  span<const byte> value) {
  const StatusWithSize result = entry.Write(key, value);

  SectorDescriptor& sector = sectors_.FromAddress(entry.address());

  if (!result.ok()) {
    ERR("Failed to write %u bytes at %#x. %u actually written",
        unsigned(entry.size()),
        unsigned(entry.address()),
        unsigned(result.size()));
    PW_TRY(MarkSectorCorruptIfNotOk(result.status(), &sector));
  }

  if (options_.verify_on_write) {
    PW_TRY(MarkSectorCorruptIfNotOk(entry.VerifyChecksumInFlash(), &sector));
  }

  sector.RemoveWritableBytes(result.size());
  sector.AddValidBytes(result.size());
  return OkStatus();
}

StatusWithSize KeyValueStore::CopyEntryToSector(Entry& entry,
                                                SectorDescriptor* new_sector,
                                                Address new_address) {
  const StatusWithSize result = entry.Copy(new_address);

  PW_TRY_WITH_SIZE(MarkSectorCorruptIfNotOk(result.status(), new_sector));

  if (options_.verify_on_write) {
    Entry new_entry;
    PW_TRY_WITH_SIZE(MarkSectorCorruptIfNotOk(
        Entry::Read(partition_, new_address, formats_, &new_entry),
        new_sector));
    // TODO(davidrogers): add test that catches doing the verify on the old
    // entry.
    PW_TRY_WITH_SIZE(MarkSectorCorruptIfNotOk(new_entry.VerifyChecksumInFlash(),
                                              new_sector));
  }
  // Entry was written successfully; update descriptor's address and the sector
  // descriptors to reflect the new entry.
  new_sector->RemoveWritableBytes(result.size());
  new_sector->AddValidBytes(result.size());

  return result;
}

Status KeyValueStore::RelocateEntry(const EntryMetadata& metadata,
                                    KeyValueStore::Address& address,
                                    span<const Address> reserved_addresses) {
  Entry entry;
  PW_TRY(ReadEntry(metadata, entry));

  // Find a new sector for the entry and write it to the new location. For
  // relocation the find should not not be a sector already containing the key
  // but can be the always empty sector, since this is part of the GC process
  // that will result in a new empty sector. Also find a sector that does not
  // have reclaimable space (mostly for the full GC, where that would result in
  // an immediate extra relocation).
  SectorDescriptor* new_sector;

  PW_TRY(sectors_.FindSpaceDuringGarbageCollection(
      &new_sector, entry.size(), metadata.addresses(), reserved_addresses));

  Address new_address = sectors_.NextWritableAddress(*new_sector);
  PW_TRY_ASSIGN(const size_t result_size,
                CopyEntryToSector(entry, new_sector, new_address));
  sectors_.FromAddress(address).RemoveValidBytes(result_size);
  address = new_address;

  return OkStatus();
}

Status KeyValueStore::FullMaintenanceHelper(MaintenanceType maintenance_type) {
  if (initialized_ == InitializationState::kNotInitialized) {
    return Status::FailedPrecondition();
  }

  // Full maintenance can be a potentially heavy operation, and should be
  // relatively infrequent, so log start/end at INFO level.
  INF("Beginning full maintenance");
  CheckForErrors();

  // Step 1: Repair errors
  if (error_detected_) {
    PW_TRY(Repair());
  }

  // Step 2: Make sure all the entries are on the primary format.
  StatusWithSize update_status = UpdateEntriesToPrimaryFormat();
  Status overall_status = update_status.status();

  if (!overall_status.ok()) {
    ERR("Failed to update all entries to the primary format");
  }

  SectorDescriptor* sector = sectors_.last_new();

  // Calculate number of bytes for the threshold.
  size_t threshold_bytes =
      (partition_.size_bytes() * kGcUsageThresholdPercentage) / 100;

  // Is bytes in use over the threshold.
  StorageStats stats = GetStorageStats();
  bool over_usage_threshold = stats.in_use_bytes > threshold_bytes;
  bool heavy = (maintenance_type == MaintenanceType::kHeavy);
  bool force_gc = heavy || over_usage_threshold || (update_status.size() > 0);

  auto do_garbage_collect_pass = [&]() {
    // TODO(drempel): look in to making an iterator method for cycling through
    // sectors starting from last_new_sector_.
    Status gc_status;
    for (size_t j = 0; j < sectors_.size(); j++) {
      sector += 1;
      if (sector == sectors_.end()) {
        sector = sectors_.begin();
      }

      if (sector->RecoverableBytes(partition_.sector_size_bytes()) > 0 &&
          (force_gc || sector->valid_bytes() == 0)) {
        gc_status = GarbageCollectSector(*sector, {});
        if (!gc_status.ok()) {
          ERR("Failed to garbage collect all sectors");
          break;
        }
      }
    }
    if (overall_status.ok()) {
      overall_status = gc_status;
    }
  };

  // Step 3: Do full garbage collect pass for all sectors. This will erase all
  // old/state entries from flash and leave only current/valid entries.
  do_garbage_collect_pass();

#if PW_KVS_REMOVE_DELETED_KEYS_IN_HEAVY_MAINTENANCE
  // Step 4: (if heavy maintenance) garbage collect all the deleted keys.
  if (heavy) {
    // If enabled, remove deleted keys from the entry cache, including freeing
    // sector bytes used by those keys. This must only be done directly after a
    // full garbage collection, otherwise the current deleted entry could be
    // garbage collected before the older stale entry producing a window for an
    // invalid/corrupted KVS state if there was a power-fault, crash or other
    // interruption.
    overall_status.Update(RemoveDeletedKeyEntries());

    // Do another garbage collect pass that will fully remove the deleted keys
    // from flash. Garbage collect will only touch sectors that have something
    // to garbage collect, which in this case is only sectors containing deleted
    // keys.
    do_garbage_collect_pass();
  }
#endif  // PW_KVS_REMOVE_DELETED_KEYS_IN_HEAVY_MAINTENANCE

  if (overall_status.ok()) {
    INF("Full maintenance complete");
  } else {
    ERR("Full maintenance finished with some errors");
  }
  return overall_status;
}

Status KeyValueStore::PartialMaintenance() {
  if (initialized_ == InitializationState::kNotInitialized) {
    return Status::FailedPrecondition();
  }

  CheckForErrors();
  // Do automatic repair, if KVS options allow for it.
  if (error_detected_ && options_.recovery != ErrorRecovery::kManual) {
    PW_TRY(Repair());
  }
  return GarbageCollect(span<const Address>());
}

Status KeyValueStore::GarbageCollect(span<const Address> reserved_addresses) {
  DBG("Garbage Collect a single sector");
  for ([[maybe_unused]] Address address : reserved_addresses) {
    DBG("   Avoid address %u", unsigned(address));
  }

  // Step 1: Find the sector to garbage collect
  SectorDescriptor* sector_to_gc =
      sectors_.FindSectorToGarbageCollect(reserved_addresses);

  if (sector_to_gc == nullptr) {
    // Nothing to GC.
    return Status::NotFound();
  }

  // Step 2: Garbage collect the selected sector.
  return GarbageCollectSector(*sector_to_gc, reserved_addresses);
}

Status KeyValueStore::RelocateKeyAddressesInSector(
    SectorDescriptor& sector_to_gc,
    const EntryMetadata& metadata,
    span<const Address> reserved_addresses) {
  for (FlashPartition::Address& address : metadata.addresses()) {
    if (sectors_.AddressInSector(sector_to_gc, address)) {
      DBG("  Relocate entry for Key 0x%08" PRIx32 ", sector %u",
          metadata.hash(),
          sectors_.Index(sectors_.FromAddress(address)));
      PW_TRY(RelocateEntry(metadata, address, reserved_addresses));
    }
  }

  return OkStatus();
}

Status KeyValueStore::GarbageCollectSector(
    SectorDescriptor& sector_to_gc, span<const Address> reserved_addresses) {
  DBG("  Garbage Collect sector %u", sectors_.Index(sector_to_gc));

  // Step 1: Move any valid entries in the GC sector to other sectors
  if (sector_to_gc.valid_bytes() != 0) {
    for (EntryMetadata& metadata : entry_cache_) {
      PW_TRY(RelocateKeyAddressesInSector(
          sector_to_gc, metadata, reserved_addresses));
    }
  }

  if (sector_to_gc.valid_bytes() != 0) {
    ERR("  Failed to relocate valid entries from sector being garbage "
        "collected, %u valid bytes remain",
        unsigned(sector_to_gc.valid_bytes()));
    return Status::Internal();
  }

  // Step 2: Reinitialize the sector
  if (!sector_to_gc.Empty(partition_.sector_size_bytes())) {
    sector_to_gc.mark_corrupt();
    internal_stats_.sector_erase_count++;
    PW_TRY(partition_.Erase(sectors_.BaseAddress(sector_to_gc), 1));
    sector_to_gc.set_writable_bytes(partition_.sector_size_bytes());
  }

  DBG("  Garbage Collect sector %u complete", sectors_.Index(sector_to_gc));
  return OkStatus();
}

StatusWithSize KeyValueStore::UpdateEntriesToPrimaryFormat() {
  size_t entries_updated = 0;
  for (EntryMetadata& prior_metadata : entry_cache_) {
    Entry entry;
    PW_TRY_WITH_SIZE(ReadEntry(prior_metadata, entry));
    if (formats_.primary().magic == entry.magic()) {
      // Ignore entries that are already on the primary format.
      continue;
    }

    DBG("Updating entry 0x%08x from old format [0x%08x] to new format "
        "[0x%08x]",
        unsigned(prior_metadata.hash()),
        unsigned(entry.magic()),
        unsigned(formats_.primary().magic));

    entries_updated++;

    last_transaction_id_ += 1;
    PW_TRY_WITH_SIZE(entry.Update(formats_.primary(), last_transaction_id_));

    // List of addresses for sectors with space for this entry.
    Address* reserved_addresses = entry_cache_.TempReservedAddressesForWrite();

    // Find addresses to write the entry to. This may involve garbage collecting
    // one or more sectors.
    PW_TRY_WITH_SIZE(GetAddressesForWrite(reserved_addresses, entry.size()));

    PW_TRY_WITH_SIZE(
        CopyEntryToSector(entry,
                          &sectors_.FromAddress(reserved_addresses[0]),
                          reserved_addresses[0]));

    // After writing the first entry successfully, update the key descriptors.
    // Once a single new the entry is written, the old entries are invalidated.
    EntryMetadata new_metadata = UpdateKeyDescriptor(
        entry, reserved_addresses[0], &prior_metadata, entry.size());

    // Write the additional copies of the entry, if redundancy is greater
    // than 1.
    for (size_t i = 1; i < redundancy(); ++i) {
      PW_TRY_WITH_SIZE(
          CopyEntryToSector(entry,
                            &sectors_.FromAddress(reserved_addresses[i]),
                            reserved_addresses[i]));
      new_metadata.AddNewAddress(reserved_addresses[i]);
    }
  }

  return StatusWithSize(entries_updated);
}

// Add any missing redundant entries/copies for a key.
Status KeyValueStore::AddRedundantEntries(EntryMetadata& metadata) {
  Entry entry;
  PW_TRY(ReadEntry(metadata, entry));
  PW_TRY(entry.VerifyChecksumInFlash());

  while (metadata.addresses().size() < redundancy()) {
    SectorDescriptor* new_sector;
    PW_TRY(GetSectorForWrite(&new_sector, entry.size(), metadata.addresses()));

    Address new_address = sectors_.NextWritableAddress(*new_sector);
    PW_TRY(CopyEntryToSector(entry, new_sector, new_address));

    metadata.AddNewAddress(new_address);
  }
  return OkStatus();
}

Status KeyValueStore::RepairCorruptSectors() {
  // Try to GC each corrupt sector, even if previous sectors fail. If GC of a
  // sector failed on the first pass, then do a second pass, since a later
  // sector might have cleared up space or otherwise unblocked the earlier
  // failed sector.
  Status repair_status = OkStatus();

  size_t loop_count = 0;
  do {
    loop_count++;
    // Error of RESOURCE_EXHAUSTED indicates no space found for relocation.
    // Reset back to OK for the next pass.
    if (repair_status.IsResourceExhausted()) {
      repair_status = OkStatus();
    }

    DBG("   Pass %u", unsigned(loop_count));
    for (SectorDescriptor& sector : sectors_) {
      if (sector.corrupt()) {
        DBG("   Found sector %u with corruption", sectors_.Index(sector));
        Status sector_status = GarbageCollectSector(sector, {});
        if (sector_status.ok()) {
          internal_stats_.corrupt_sectors_recovered += 1;
        } else if (repair_status.ok() || repair_status.IsResourceExhausted()) {
          repair_status = sector_status;
        }
      }
    }
    DBG("   Pass %u complete", unsigned(loop_count));
  } while (!repair_status.ok() && loop_count < 2);

  return repair_status;
}

Status KeyValueStore::EnsureFreeSectorExists() {
  Status repair_status = OkStatus();
  bool empty_sector_found = false;

  DBG("   Find empty sector");
  for (SectorDescriptor& sector : sectors_) {
    if (sector.Empty(partition_.sector_size_bytes())) {
      empty_sector_found = true;
      DBG("   Empty sector found");
      break;
    }
  }
  if (empty_sector_found == false) {
    DBG("   No empty sector found, attempting to GC a free sector");
    Status sector_status = GarbageCollect(span<const Address, 0>());
    if (repair_status.ok() && !sector_status.ok()) {
      DBG("   Unable to free an empty sector");
      repair_status = sector_status;
    }
  }

  return repair_status;
}

Status KeyValueStore::EnsureEntryRedundancy() {
  Status repair_status = OkStatus();

  if (redundancy() == 1) {
    DBG("   Redundancy not in use, nothting to check");
    return OkStatus();
  }

  DBG("   Write any needed additional duplicate copies of keys to fulfill %u"
      " redundancy",
      unsigned(redundancy()));
  for (EntryMetadata& metadata : entry_cache_) {
    if (metadata.addresses().size() >= redundancy()) {
      continue;
    }

    DBG("   Key with %u of %u copies found, adding missing copies",
        unsigned(metadata.addresses().size()),
        unsigned(redundancy()));
    Status fill_status = AddRedundantEntries(metadata);
    if (fill_status.ok()) {
      internal_stats_.missing_redundant_entries_recovered += 1;
      DBG("   Key missing copies added");
    } else {
      DBG("   Failed to add key missing copies");
      if (repair_status.ok()) {
        repair_status = fill_status;
      }
    }
  }

  return repair_status;
}

Status KeyValueStore::FixErrors() {
  DBG("Fixing KVS errors");

  // Step 1: Garbage collect any sectors marked as corrupt.
  Status overall_status = RepairCorruptSectors();

  // Step 2: Make sure there is at least 1 empty sector. This needs to be a
  // seperate check of sectors from step 1, because a found empty sector might
  // get written to by a later GC that fails and does not result in a free
  // sector.
  Status repair_status = EnsureFreeSectorExists();
  if (overall_status.ok()) {
    overall_status = repair_status;
  }

  // Step 3: Make sure each stored key has the full number of redundant
  // entries.
  repair_status = EnsureEntryRedundancy();
  if (overall_status.ok()) {
    overall_status = repair_status;
  }

  if (overall_status.ok()) {
    error_detected_ = false;
    initialized_ = InitializationState::kReady;
  }
  return overall_status;
}

Status KeyValueStore::Repair() {
  // If errors have been detected, just reinit the KVS metadata. This does a
  // full deep error check and any needed repairs. Then repair any errors.
  INF("Starting KVS repair");

  DBG("Reinitialize KVS metadata");
  InitializeMetadata()
      .IgnoreError();  // TODO(b/242598609): Handle Status properly

  return FixErrors();
}

KeyValueStore::Entry KeyValueStore::CreateEntry(Address address,
                                                Key key,
                                                span<const byte> value,
                                                EntryState state) {
  // Always bump the transaction ID when creating a new entry.
  //
  // Burning transaction IDs prevents inconsistencies between flash and memory
  // that which could happen if a write succeeds, but for some reason the read
  // and verify step fails. Here's how this would happen:
  //
  //   1. The entry is written but for some reason the flash reports failure OR
  //      The write succeeds, but the read / verify operation fails.
  //   2. The transaction ID is NOT incremented, because of the failure
  //   3. (later) A new entry is written, re-using the transaction ID (oops)
  //
  // By always burning transaction IDs, the above problem can't happen.
  last_transaction_id_ += 1;

  if (state == EntryState::kDeleted) {
    return Entry::Tombstone(
        partition_, address, formats_.primary(), key, last_transaction_id_);
  }
  return Entry::Valid(partition_,
                      address,
                      formats_.primary(),
                      key,
                      value,
                      last_transaction_id_);
}

void KeyValueStore::LogDebugInfo() const {
  const size_t sector_size_bytes = partition_.sector_size_bytes();
  DBG("====================== KEY VALUE STORE DUMP =========================");
  DBG(" ");
  DBG("Flash partition:");
  DBG("  Sector count     = %u", unsigned(partition_.sector_count()));
  DBG("  Sector max count = %u", unsigned(sectors_.max_size()));
  DBG("  Sectors in use   = %u", unsigned(sectors_.size()));
  DBG("  Sector size      = %u", unsigned(sector_size_bytes));
  DBG("  Total size       = %u", unsigned(partition_.size_bytes()));
  DBG("  Alignment        = %u", unsigned(partition_.alignment_bytes()));
  DBG(" ");
  DBG("Key descriptors:");
  DBG("  Entry count     = %u", unsigned(entry_cache_.total_entries()));
  DBG("  Max entry count = %u", unsigned(entry_cache_.max_entries()));
  DBG(" ");
  DBG("      #     hash        version    address   address (hex)");
  size_t count = 0;
  for (const EntryMetadata& metadata : entry_cache_) {
    DBG("   |%3zu: | %8zx  |%8zu  | %8zu | %8zx",
        count++,
        size_t(metadata.hash()),
        size_t(metadata.transaction_id()),
        size_t(metadata.first_address()),
        size_t(metadata.first_address()));
  }
  DBG(" ");

  DBG("Sector descriptors:");
  DBG("      #     tail free  valid    has_space");
  for (const SectorDescriptor& sd : sectors_) {
    DBG("   |%3u: | %8zu  |%8zu  | %s",
        sectors_.Index(sd),
        size_t(sd.writable_bytes()),
        sd.valid_bytes(),
        sd.writable_bytes() ? "YES" : "");
  }
  DBG(" ");

  // TODO(keir): This should stop logging after some threshold.
  // size_t dumped_bytes = 0;
  DBG("Sector raw data:");
  for (size_t sector_id = 0; sector_id < sectors_.size(); ++sector_id) {
    // Read sector data. Yes, this will blow the stack on embedded.
    std::array<byte, 500> raw_sector_data;  // TODO!!!
    [[maybe_unused]] StatusWithSize sws =
        partition_.Read(sector_id * sector_size_bytes, raw_sector_data);
    DBG("Read: %u bytes", unsigned(sws.size()));

    DBG("  base    addr  offs   0  1  2  3  4  5  6  7");
    for (size_t i = 0; i < sector_size_bytes; i += 8) {
      DBG("  %3zu %8zx %5zu | %02x %02x %02x %02x %02x %02x %02x %02x",
          sector_id,
          (sector_id * sector_size_bytes) + i,
          i,
          static_cast<unsigned int>(raw_sector_data[i + 0]),
          static_cast<unsigned int>(raw_sector_data[i + 1]),
          static_cast<unsigned int>(raw_sector_data[i + 2]),
          static_cast<unsigned int>(raw_sector_data[i + 3]),
          static_cast<unsigned int>(raw_sector_data[i + 4]),
          static_cast<unsigned int>(raw_sector_data[i + 5]),
          static_cast<unsigned int>(raw_sector_data[i + 6]),
          static_cast<unsigned int>(raw_sector_data[i + 7]));

      // TODO(keir): Fix exit condition.
      if (i > 128) {
        break;
      }
    }
    DBG(" ");
  }

  DBG("////////////////////// KEY VALUE STORE DUMP END /////////////////////");
}

void KeyValueStore::LogSectors() const {
  DBG("Sector descriptors: count %u", unsigned(sectors_.size()));
  for (auto& sector : sectors_) {
    DBG("  - Sector %u: valid %u, recoverable %u, free %u",
        sectors_.Index(sector),
        unsigned(sector.valid_bytes()),
        unsigned(sector.RecoverableBytes(partition_.sector_size_bytes())),
        unsigned(sector.writable_bytes()));
  }
}

void KeyValueStore::LogKeyDescriptor() const {
  DBG("Key descriptors: count %u", unsigned(entry_cache_.total_entries()));
  for (const EntryMetadata& metadata : entry_cache_) {
    DBG("  - Key: %s, hash %#x, transaction ID %u, first address %#x",
        metadata.state() == EntryState::kDeleted ? "Deleted" : "Valid",
        unsigned(metadata.hash()),
        unsigned(metadata.transaction_id()),
        unsigned(metadata.first_address()));
  }
}

}  // namespace pw::kvs
