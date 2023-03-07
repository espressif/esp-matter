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
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "pw_containers/vector.h"
#include "pw_kvs/checksum.h"
#include "pw_kvs/flash_memory.h"
#include "pw_kvs/format.h"
#include "pw_kvs/internal/entry.h"
#include "pw_kvs/internal/entry_cache.h"
#include "pw_kvs/internal/key_descriptor.h"
#include "pw_kvs/internal/sectors.h"
#include "pw_kvs/internal/span_traits.h"
#include "pw_kvs/key.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"

namespace pw {
namespace kvs {

enum class GargbageCollectOnWrite {
  // Disable all automatic garbage collection on write.
  kDisabled,

  // Allow up to a single sector, if needed, to be garbage collected on write.
  kOneSector,

  // Allow as many sectors as needed be garbage collected on write.
  kAsManySectorsNeeded,
};

enum class ErrorRecovery {
  // Immediately do full recovery of any errors that are detected.
  kImmediate,

  // Recover from errors, but delay time consuming recovery steps until later
  // as part of other time consuming operations. Such as waiting to garbage
  // collect sectors with corrupt entries until the next garbage collection.
  kLazy,

  // Only recover from errors when manually triggered as part of maintenance
  // operations. This is not recommended for normal use, only for test or
  // read-only use.
  kManual,
};

struct Options {
  // Perform garbage collection if necessary when writing. If not kDisabled,
  // garbage collection is attempted if space for an entry cannot be found. This
  // is a relatively lengthy operation. If kDisabled, Put calls that would
  // require garbage collection fail with RESOURCE_EXHAUSTED.
  GargbageCollectOnWrite gc_on_write =
      GargbageCollectOnWrite::kAsManySectorsNeeded;

  // When the KVS handles errors that are discovered, such as corrupt entries,
  // not enough redundant copys of an entry, etc.
  ErrorRecovery recovery = ErrorRecovery::kLazy;

  // Verify an entry's checksum after reading it from flash.
  bool verify_on_read = true;

  // Verify an in-flash entry's checksum after writing it.
  bool verify_on_write = true;
};

class KeyValueStore {
 public:
  // KeyValueStores are declared as instances of
  // KeyValueStoreBuffer<MAX_ENTRIES, MAX_SECTORS>, which allocates buffers for
  // tracking entries and flash sectors.

  // Initializes the key-value store. Must be called before calling other
  // functions.
  //
  // Return values:
  //
  //          OK: KVS successfully initialized.
  //   DATA_LOSS: KVS initialized and is usable, but contains corrupt data.
  //     UNKNOWN: Unknown error. KVS is not initialized.
  //
  Status Init();

  bool initialized() const {
    return initialized_ == InitializationState::kReady;
  }

  // Reads the value of an entry in the KVS. The value is read into the provided
  // buffer and the number of bytes read is returned. If desired, the read can
  // be started at an offset.
  //
  // If the output buffer is too small for the value, Get returns
  // RESOURCE_EXHAUSTED with the number of bytes read. The remainder of the
  // value can be read by calling get with an offset.
  //
  //                    OK: the entry was successfully read
  //             NOT_FOUND: the key is not present in the KVS
  //             DATA_LOSS: found the entry, but the data was corrupted
  //    RESOURCE_EXHAUSTED: the buffer could not fit the entire value, but as
  //                        many bytes as possible were written to it
  //   FAILED_PRECONDITION: the KVS is not initialized
  //      INVALID_ARGUMENT: key is empty or too long or value is too large
  //
  StatusWithSize Get(Key key,
                     span<std::byte> value,
                     size_t offset_bytes = 0) const;

  // This overload of Get accepts a pointer to a trivially copyable object.
  // If the value is an array, call Get with
  // as_writable_bytes(span(array)), or pass a pointer to the array
  // instead of the array itself.
  template <typename Pointer,
            typename = std::enable_if_t<std::is_pointer<Pointer>::value>>
  Status Get(const Key& key, const Pointer& pointer) const {
    using T = std::remove_reference_t<std::remove_pointer_t<Pointer>>;
    CheckThatObjectCanBePutOrGet<T>();
    return FixedSizeGet(key, pointer, sizeof(T));
  }

  // Adds a key-value entry to the KVS. If the key was already present, its
  // value is overwritten.
  //
  // The value may be a span of bytes or a trivially copyable object.
  //
  // In the current implementation, all keys in the KVS must have a unique hash.
  // If Put is called with a key whose hash matches an existing key, nothing
  // is added and ALREADY_EXISTS is returned.
  //
  //                    OK: the entry was successfully added or updated
  //             DATA_LOSS: checksum validation failed after writing the data
  //    RESOURCE_EXHAUSTED: there is not enough space to add the entry
  //        ALREADY_EXISTS: the entry could not be added because a different key
  //                        with the same hash is already in the KVS
  //   FAILED_PRECONDITION: the KVS is not initialized
  //      INVALID_ARGUMENT: key is empty or too long or value is too large
  //
  template <typename T,
            typename std::enable_if_t<ConvertsToSpan<T>::value>* = nullptr>
  Status Put(const Key& key, const T& value) {
    return PutBytes(key, as_bytes(internal::make_span(value)));
  }

  template <typename T,
            typename std::enable_if_t<!ConvertsToSpan<T>::value>* = nullptr>
  Status Put(const Key& key, const T& value) {
    CheckThatObjectCanBePutOrGet<T>();
    return PutBytes(key, as_bytes(span<const T>(&value, 1)));
  }

  // Removes a key-value entry from the KVS.
  //
  //                    OK: the entry was successfully added or updated
  //             NOT_FOUND: the key is not present in the KVS
  //             DATA_LOSS: checksum validation failed after recording the erase
  //    RESOURCE_EXHAUSTED: insufficient space to mark the entry as deleted
  //   FAILED_PRECONDITION: the KVS is not initialized
  //      INVALID_ARGUMENT: key is empty or too long
  //
  Status Delete(Key key);

  // Returns the size of the value corresponding to the key.
  //
  //                    OK: the size was returned successfully
  //             NOT_FOUND: the key is not present in the KVS
  //             DATA_LOSS: checksum validation failed after reading the entry
  //   FAILED_PRECONDITION: the KVS is not initialized
  //      INVALID_ARGUMENT: key is empty or too long
  //
  StatusWithSize ValueSize(Key key) const;

  // Perform all maintenance possible, including all neeeded repairing of
  // corruption and garbage collection of reclaimable space in the KVS. When
  // configured for manual recovery, this (along with FullMaintenance) is the
  // only way KVS repair is triggered.
  //
  // - Heavy garbage collection of all reclaimable space, regardless of valid
  //   data in the sector.
  Status HeavyMaintenance() {
    return FullMaintenanceHelper(MaintenanceType::kHeavy);
  }

  // Perform all maintenance possible, including all neeeded repairing of
  // corruption and garbage collection of reclaimable space in the KVS. When
  // configured for manual recovery, this (along with HeavyMaintenance) is the
  // only way KVS repair is triggered.
  //
  // - Regular will not garbage collect sectors with valid data unless the KVS
  //   is mostly full.
  Status FullMaintenance() {
    return FullMaintenanceHelper(MaintenanceType::kRegular);
  }

  // Perform a portion of KVS maintenance. If configured for at least lazy
  // recovery, will do any needed repairing of corruption. Does garbage
  // collection of part of the KVS, typically a single sector or similar unit
  // that makes sense for the KVS implementation.
  Status PartialMaintenance();

  void LogDebugInfo() const;

  // Classes and functions to support STL-style iteration.
  class iterator;

  class Item {
   public:
    // The key as a null-terminated string.
    const char* key() const { return key_buffer_.data(); }

    // Gets the value referred to by this iterator. Equivalent to
    // KeyValueStore::Get.
    StatusWithSize Get(span<std::byte> value_buffer,
                       size_t offset_bytes = 0) const {
      return kvs_.Get(key(), *iterator_, value_buffer, offset_bytes);
    }

    template <typename Pointer,
              typename = std::enable_if_t<std::is_pointer<Pointer>::value>>
    Status Get(const Pointer& pointer) const {
      using T = std::remove_reference_t<std::remove_pointer_t<Pointer>>;
      CheckThatObjectCanBePutOrGet<T>();
      return kvs_.FixedSizeGet(key(), *iterator_, pointer, sizeof(T));
    }

    // Reads the size of the value referred to by this iterator. Equivalent to
    // KeyValueStore::ValueSize.
    StatusWithSize ValueSize() const { return kvs_.ValueSize(*iterator_); }

   private:
    friend class iterator;

    constexpr Item(const KeyValueStore& kvs,
                   const internal::EntryCache::const_iterator& item_iterator)
        : kvs_(kvs), iterator_(item_iterator), key_buffer_{} {}

    void ReadKey();

    const KeyValueStore& kvs_;
    internal::EntryCache::const_iterator iterator_;

    // Buffer large enough for a null-terminated version of any valid key.
    std::array<char, internal::Entry::kMaxKeyLength + 1> key_buffer_;
  };

  class iterator {
   public:
    iterator& operator++();

    iterator operator++(int) {
      const iterator original(item_.kvs_, item_.iterator_);
      operator++();
      return original;
    }

    // Reads the entry's key from flash.
    const Item& operator*() {
      item_.ReadKey();
      return item_;
    }

    const Item* operator->() {
      return &operator*();  // Read the key into the Item object.
    }

    constexpr bool operator==(const iterator& rhs) const {
      return item_.iterator_ == rhs.item_.iterator_;
    }

    constexpr bool operator!=(const iterator& rhs) const {
      return item_.iterator_ != rhs.item_.iterator_;
    }

   private:
    friend class KeyValueStore;

    constexpr iterator(
        const KeyValueStore& kvs,
        const internal::EntryCache::const_iterator& item_iterator)
        : item_(kvs, item_iterator) {}

    Item item_;
  };

  using const_iterator = iterator;  // Standard alias for iterable types.

  iterator begin() const;
  iterator end() const { return iterator(*this, entry_cache_.end()); }

  // Returns the number of valid entries in the KeyValueStore.
  size_t size() const { return entry_cache_.present_entries(); }

  // Returns the number of valid entries and deleted entries yet to be collected
  size_t total_entries_with_deleted() const {
    return entry_cache_.total_entries();
  }

  size_t max_size() const { return entry_cache_.max_entries(); }

  size_t empty() const { return size() == 0u; }

  // Returns the number of transactions that have occurred since the KVS was
  // first used. This value is retained across initializations, but is reset if
  // the underlying flash is erased.
  uint32_t transaction_count() const { return last_transaction_id_; }

  struct StorageStats {
    size_t writable_bytes;
    size_t in_use_bytes;
    size_t reclaimable_bytes;
    size_t sector_erase_count;
    size_t corrupt_sectors_recovered;
    size_t missing_redundant_entries_recovered;
  };

  StorageStats GetStorageStats() const;

  // Level of redundancy to use for writing entries.
  size_t redundancy() const { return entry_cache_.redundancy(); }

  bool error_detected() const { return error_detected_; }

  // Maximum number of bytes allowed for a key-value combination.
  size_t max_key_value_size_bytes() const {
    return max_key_value_size_bytes(partition_.sector_size_bytes());
  }

  // Maximum number of bytes allowed for a given sector size for a key-value
  // combination.
  static constexpr size_t max_key_value_size_bytes(
      size_t partition_sector_size_bytes) {
    return partition_sector_size_bytes - Entry::entry_overhead();
  }

  // Check KVS for any error conditions. Primarily intended for test and
  // internal use.
  bool CheckForErrors();

 protected:
  using Address = FlashPartition::Address;
  using Entry = internal::Entry;
  using KeyDescriptor = internal::KeyDescriptor;
  using SectorDescriptor = internal::SectorDescriptor;

  // In the future, will be able to provide additional EntryFormats for
  // backwards compatibility.
  KeyValueStore(FlashPartition* partition,
                span<const EntryFormat> formats,
                const Options& options,
                size_t redundancy,
                Vector<SectorDescriptor>& sector_descriptor_list,
                const SectorDescriptor** temp_sectors_to_skip,
                Vector<KeyDescriptor>& key_descriptor_list,
                Address* addresses);

 private:
  using EntryMetadata = internal::EntryMetadata;
  using EntryState = internal::EntryState;

  template <typename T>
  static constexpr void CheckThatObjectCanBePutOrGet() {
    static_assert(
        std::is_trivially_copyable<T>::value && !std::is_pointer<T>::value,
        "Only trivially copyable, non-pointer objects may be Put and Get by "
        "value. Any value may be stored by converting it to a byte span "
        "with as_bytes(span(&value, 1)) or "
        "as_writable_bytes(span(&value, 1)).");
  }

  Status InitializeMetadata();
  Status LoadEntry(Address entry_address, Address* next_entry_address);
  Status ScanForEntry(const SectorDescriptor& sector,
                      Address start_address,
                      Address* next_entry_address);

  // Remove deleted keys from the entry cache, including freeing sector bytes
  // used by those keys. This must only be done directly after a full garbage
  // collection, otherwise the current deleted entry could be garbage
  // collected before the older stale entry producing a window for an
  // invalid/corrupted KVS state if there was a power-fault, crash or other
  // interruption.
  Status RemoveDeletedKeyEntries();

  Status PutBytes(Key key, span<const std::byte> value);

  StatusWithSize ValueSize(const EntryMetadata& metadata) const;

  Status ReadEntry(const EntryMetadata& metadata, Entry& entry) const;

  // Finds the metadata for an entry matching a particular key. Searches for a
  // KeyDescriptor that matches this key and sets *metadata_out to point to it
  // if one is found.
  //
  //             OK: there is a matching descriptor and *metadata is set
  //      NOT_FOUND: there is no descriptor that matches this key, but this key
  //                 has a unique hash (and could potentially be added to the
  //                 KVS)
  // ALREADY_EXISTS: there is no descriptor that matches this key, but the
  //                 key's hash collides with the hash for an existing
  //                 descriptor
  //
  Status FindEntry(Key key, EntryMetadata* metadata_out) const;

  // Searches for a KeyDescriptor that matches this key and sets *metadata_out
  // to point to it if one is found.
  //
  //          OK: there is a matching descriptor and *metadata_out is set
  //   NOT_FOUND: there is no descriptor that matches this key
  //
  Status FindExisting(Key key, EntryMetadata* metadata_out) const;

  StatusWithSize Get(Key key,
                     const EntryMetadata& metadata,
                     span<std::byte> value_buffer,
                     size_t offset_bytes) const;

  Status FixedSizeGet(Key key, void* value, size_t size_bytes) const;

  Status FixedSizeGet(Key key,
                      const EntryMetadata& metadata,
                      void* value,
                      size_t size_bytes) const;

  Status CheckWriteOperation(Key key) const;
  Status CheckReadOperation(Key key) const;

  Status WriteEntryForExistingKey(EntryMetadata& metadata,
                                  EntryState new_state,
                                  Key key,
                                  span<const std::byte> value);

  Status WriteEntryForNewKey(Key key, span<const std::byte> value);

  Status WriteEntry(Key key,
                    span<const std::byte> value,
                    EntryState new_state,
                    EntryMetadata* prior_metadata = nullptr,
                    const internal::Entry* prior_entry = nullptr);

  EntryMetadata CreateOrUpdateKeyDescriptor(const Entry& new_entry,
                                            Key key,
                                            EntryMetadata* prior_metadata,
                                            size_t prior_size);

  EntryMetadata UpdateKeyDescriptor(const Entry& entry,
                                    Address new_address,
                                    EntryMetadata* prior_metadata,
                                    size_t prior_size);

  Status GetAddressesForWrite(Address* write_addresses, size_t write_size);

  Status GetSectorForWrite(SectorDescriptor** sector,
                           size_t entry_size,
                           span<const Address> reserved_addresses);

  Status MarkSectorCorruptIfNotOk(Status status, SectorDescriptor* sector);

  Status AppendEntry(const Entry& entry, Key key, span<const std::byte> value);

  StatusWithSize CopyEntryToSector(Entry& entry,
                                   SectorDescriptor* new_sector,
                                   Address new_address);

  Status RelocateEntry(const EntryMetadata& metadata,
                       KeyValueStore::Address& address,
                       span<const Address> reserved_addresses);

  // Perform all maintenance possible, including all neeeded repairing of
  // corruption and garbage collection of reclaimable space in the KVS. When
  // configured for manual recovery, this is the only way KVS repair is
  // triggered.
  //
  // - Regular will not garbage collect sectors with valid data unless the KVS
  //   is mostly full.
  // - Heavy will garbage collect all reclaimable space regardless of valid data
  //   in the sector.
  enum class MaintenanceType {
    kRegular,
    kHeavy,
  };
  Status FullMaintenanceHelper(MaintenanceType maintenance_type);

  // Find and garbage collect a singe sector that does not include a reserved
  // address.
  Status GarbageCollect(span<const Address> reserved_addresses);

  Status RelocateKeyAddressesInSector(SectorDescriptor& sector_to_gc,
                                      const EntryMetadata& metadata,
                                      span<const Address> reserved_addresses);

  Status GarbageCollectSector(SectorDescriptor& sector_to_gc,
                              span<const Address> reserved_addresses);

  // Ensure that all entries are on the primary (first) format. Entries that are
  // not on the primary format are rewritten.
  //
  // Return: status + number of entries updated.
  StatusWithSize UpdateEntriesToPrimaryFormat();

  Status AddRedundantEntries(EntryMetadata& metadata);

  Status RepairCorruptSectors();

  Status EnsureFreeSectorExists();

  Status EnsureEntryRedundancy();

  Status FixErrors();

  Status Repair();

  internal::Entry CreateEntry(Address address,
                              Key key,
                              span<const std::byte> value,
                              EntryState state);

  void LogSectors() const;
  void LogKeyDescriptor() const;

  FlashPartition& partition_;
  const internal::EntryFormats formats_;

  // List of sectors used by this KVS.
  internal::Sectors sectors_;

  // Unordered list of KeyDescriptors. Finding a key requires scanning and
  // verifying a match by reading the actual entry.
  internal::EntryCache entry_cache_;

  Options options_;

  // Threshold value for when to garbage collect all stale data. Above the
  // threshold, GC all reclaimable bytes regardless of if valid data is in
  // sector. Below the threshold, only GC sectors with reclaimable bytes and no
  // valid bytes. The threshold is based on the portion of KVS capacity used by
  // valid bytes.
  static constexpr size_t kGcUsageThresholdPercentage = 70;

  enum class InitializationState {
    // KVS Init() has not been called and KVS is not usable.
    kNotInitialized,

    // KVS Init() run but not all the needed invariants are met for an operating
    // KVS. KVS is not writable, but read operaions are possible.
    kNeedsMaintenance,

    // KVS is fully ready for use.
    kReady,
  };
  InitializationState initialized_;

  // error_detected_ needs to be set from const KVS methods (such as Get), so
  // make it mutable.
  mutable bool error_detected_;

  struct InternalStats {
    size_t sector_erase_count;
    size_t corrupt_sectors_recovered;
    size_t missing_redundant_entries_recovered;
  };
  InternalStats internal_stats_;

  uint32_t last_transaction_id_;
};

template <size_t kMaxEntries,
          size_t kMaxUsableSectors,
          size_t kRedundancy = 1,
          size_t kEntryFormats = 1>
class KeyValueStoreBuffer : public KeyValueStore {
 public:
  // Constructs a KeyValueStore on the partition, with support for one
  // EntryFormat (kEntryFormats must be 1).
  KeyValueStoreBuffer(FlashPartition* partition,
                      const EntryFormat& format,
                      const Options& options = {})
      : KeyValueStoreBuffer(
            partition,
            span<const EntryFormat, kEntryFormats>(
                reinterpret_cast<const EntryFormat (&)[1]>(format)),
            options) {
    static_assert(kEntryFormats == 1,
                  "kEntryFormats EntryFormats must be specified");
  }

  // Constructs a KeyValueStore on the partition. Supports multiple entry
  // formats. The first EntryFormat is used for new entries.
  KeyValueStoreBuffer(FlashPartition* partition,
                      span<const EntryFormat, kEntryFormats> formats,
                      const Options& options = {})
      : KeyValueStore(partition,
                      formats_,
                      options,
                      kRedundancy,
                      sectors_,
                      temp_sectors_to_skip_,
                      key_descriptors_,
                      addresses_),
        sectors_(),
        key_descriptors_(),
        formats_() {
    std::copy(formats.begin(), formats.end(), formats_.begin());
  }

 private:
  static_assert(kMaxEntries > 0u);
  static_assert(kMaxUsableSectors > 0u);
  static_assert(kRedundancy > 0u);
  static_assert(kEntryFormats > 0u);

  Vector<SectorDescriptor, kMaxUsableSectors> sectors_;

  // Allocate space for an list of SectorDescriptors to avoid while searching
  // for space to write entries. This is used to avoid changing sectors that are
  // reserved for a new entry or marked as having other redundant copies of an
  // entry. Up to kRedundancy sectors are reserved for a new entry, and up to
  // kRedundancy - 1 sectors can have redundant copies of an entry, giving a
  // maximum of 2 * kRedundancy - 1 sectors to avoid.
  const SectorDescriptor* temp_sectors_to_skip_[2 * kRedundancy - 1];

  // KeyDescriptors for use by the KVS's EntryCache.
  Vector<KeyDescriptor, kMaxEntries> key_descriptors_;

  // An array of addresses associated with the KeyDescriptors for use with the
  // EntryCache. To support having KeyValueStores with different redundancies,
  // the addresses are stored in a parallel array instead of inside
  // KeyDescriptors.
  internal::EntryCache::AddressList<kRedundancy, kMaxEntries> addresses_;

  // EntryFormats that can be read by this KeyValueStore.
  std::array<EntryFormat, kEntryFormats> formats_;
};

}  // namespace kvs
}  // namespace pw
