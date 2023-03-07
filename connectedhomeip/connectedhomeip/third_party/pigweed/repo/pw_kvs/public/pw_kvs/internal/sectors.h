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

#include <climits>
#include <cstddef>
#include <cstdint>

#include "pw_containers/vector.h"
#include "pw_kvs/flash_memory.h"
#include "pw_span/span.h"

namespace pw {
namespace kvs {
namespace internal {

// Tracks the available and used space in each sector used by the KVS.
class SectorDescriptor {
 public:
  // The number of bytes available to be written in this sector. It the sector
  // is marked as corrupt, no bytes are available.
  size_t writable_bytes() const {
    return (tail_free_bytes_ == kCorruptSector) ? 0 : tail_free_bytes_;
  }

  void set_writable_bytes(uint16_t writable_bytes) {
    tail_free_bytes_ = writable_bytes;
  }

  void mark_corrupt() { tail_free_bytes_ = kCorruptSector; }
  bool corrupt() const { return tail_free_bytes_ == kCorruptSector; }

  // The number of bytes of valid data in this sector.
  size_t valid_bytes() const { return valid_bytes_; }

  // Adds valid bytes without updating the writable bytes.
  void AddValidBytes(uint16_t bytes) { valid_bytes_ += bytes; }

  // Removes valid bytes without updating the writable bytes.
  void RemoveValidBytes(uint16_t bytes) {
    if (bytes > valid_bytes()) {
      // TODO(hepler): use a DCHECK instead -- this is a programming error
      valid_bytes_ = 0;
    } else {
      valid_bytes_ -= bytes;
    }
  }

  // Removes writable bytes without updating the valid bytes.
  void RemoveWritableBytes(uint16_t bytes) {
    if (bytes > writable_bytes()) {
      // TODO(hepler): use a DCHECK instead -- this is a programming error
      tail_free_bytes_ = 0;
    } else {
      tail_free_bytes_ -= bytes;
    }
  }

  bool HasSpace(size_t required_space) const {
    return writable_bytes() >= required_space;
  }

  bool Empty(size_t sector_size_bytes) const {
    return writable_bytes() == sector_size_bytes;
  }

  // Returns the number of bytes that would be recovered if this sector is
  // garbage collected.
  size_t RecoverableBytes(size_t sector_size_bytes) const {
    return sector_size_bytes - valid_bytes_ - writable_bytes();
  }

  static constexpr size_t max_sector_size() { return kMaxSectorSize; }

 private:
  friend class Sectors;

  static constexpr uint16_t kCorruptSector = UINT16_MAX;
  static constexpr size_t kMaxSectorSize = UINT16_MAX - 1;

  explicit constexpr SectorDescriptor(uint16_t sector_size_bytes)
      : tail_free_bytes_(sector_size_bytes), valid_bytes_(0) {}

  uint16_t tail_free_bytes_;  // writable bytes at the end of the sector
  uint16_t valid_bytes_;      // sum of sizes of valid entries
};

// Represents a list of sectors usable by the KVS.
class Sectors {
 public:
  using Address = FlashPartition::Address;

  constexpr Sectors(Vector<SectorDescriptor>& sectors,
                    FlashPartition& partition,
                    const SectorDescriptor** temp_sectors_to_skip)
      : descriptors_(sectors),
        partition_(partition),
        last_new_(nullptr),
        temp_sectors_to_skip_(temp_sectors_to_skip) {}

  // Resets the Sectors list. Must be called before using the object.
  void Reset() {
    last_new_ = descriptors_.begin();
    descriptors_.assign(partition_.sector_count(),
                        SectorDescriptor(partition_.sector_size_bytes()));
  }

  // The last sector that was selected as the "new empty sector" to write to.
  // This last new sector is used as the starting point for the next "find a new
  // empty sector to write to" operation. By using the last new sector as the
  // start point we will cycle which empty sector is selected next, spreading
  // the wear across all the empty sectors, rather than putting more wear on the
  // lower number sectors.
  //
  // Use SectorDescriptor* for the persistent storage rather than sector index
  // because SectorDescriptor* is the standard way to identify a sector.
  SectorDescriptor* last_new() const { return last_new_; }

  // Sets the last new sector from the provided address.
  void set_last_new_sector(Address address) {
    last_new_ = &FromAddress(address);
  }

  // Checks if an address is in the particular sector.
  bool AddressInSector(const SectorDescriptor& sector, Address address) const {
    const Address sector_base = BaseAddress(sector);
    const Address sector_end = sector_base + partition_.sector_size_bytes();

    return ((address >= sector_base) && (address < sector_end));
  }

  // Returns the first address in the provided sector.
  Address BaseAddress(const SectorDescriptor& sector) const {
    return Index(sector) * partition_.sector_size_bytes();
  }

  SectorDescriptor& FromAddress(Address address) const {
    // TODO(hepler): Add boundary checking once asserts are supported.
    // DCHECK_LT(index, sector_map_size_);`
    return descriptors_[address / partition_.sector_size_bytes()];
  }

  Address NextWritableAddress(const SectorDescriptor& sector) const {
    return BaseAddress(sector) + partition_.sector_size_bytes() -
           sector.writable_bytes();
  }

  // Finds either an existing sector with enough space that is not the sector to
  // skip, or an empty sector. Maintains the invariant that there is always at
  // least 1 empty sector. Addresses in reserved_addresses are avoided.
  Status FindSpace(SectorDescriptor** found_sector,
                   size_t size,
                   span<const Address> reserved_addresses) {
    return Find(kAppendEntry, found_sector, size, {}, reserved_addresses);
  }

  // Same as FindSpace, except that the 1 empty sector invariant is ignored.
  // Both addresses_to_skip and reserved_addresses are avoided.
  Status FindSpaceDuringGarbageCollection(
      SectorDescriptor** found_sector,
      size_t size,
      span<const Address> addresses_to_skip,
      span<const Address> reserved_addresses) {
    return Find(kGarbageCollect,
                found_sector,
                size,
                addresses_to_skip,
                reserved_addresses);
  }

  // Finds a sector that is ready to be garbage collected. Returns nullptr if no
  // sectors can / need to be garbage collected.
  SectorDescriptor* FindSectorToGarbageCollect(
      span<const Address> reserved_addresses) const;

  // The number of sectors in use.
  size_t size() const { return descriptors_.size(); }

  // The maximum number of sectors supported.
  size_t max_size() const { return descriptors_.max_size(); }

  // Returns the index of the provided sector. Used for logging.
  unsigned Index(const SectorDescriptor& sector) const {
    return &sector - descriptors_.begin();
  }
  unsigned Index(const SectorDescriptor* s) const { return Index(*s); }
  unsigned Index(Address address) const { return Index(FromAddress(address)); }

  // Iterators for iterating over all sectors.
  using iterator = Vector<SectorDescriptor>::iterator;
  using const_iterator = Vector<SectorDescriptor>::const_iterator;

  iterator begin() { return descriptors_.begin(); }
  const_iterator begin() const { return descriptors_.begin(); }
  iterator end() { return descriptors_.end(); }
  const_iterator end() const { return descriptors_.end(); }

 private:
  enum FindMode { kAppendEntry, kGarbageCollect };

  Status Find(FindMode find_mode,
              SectorDescriptor** found_sector,
              size_t size,
              span<const Address> addresses_to_skip,
              span<const Address> reserved_addresses);

  SectorDescriptor& WearLeveledSectorFromIndex(size_t idx) const;

  Vector<SectorDescriptor>& descriptors_;
  FlashPartition& partition_;

  SectorDescriptor* last_new_;

  // Temp buffer with space for redundancy * 2 - 1 sector pointers. This list is
  // used to track sectors that should be excluded from Find functions.
  const SectorDescriptor** const temp_sectors_to_skip_;
};

}  // namespace internal
}  // namespace kvs
}  // namespace pw
