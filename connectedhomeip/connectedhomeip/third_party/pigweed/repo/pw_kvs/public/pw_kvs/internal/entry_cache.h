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

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "pw_containers/vector.h"
#include "pw_kvs/flash_memory.h"
#include "pw_kvs/format.h"
#include "pw_kvs/internal/key_descriptor.h"
#include "pw_kvs/internal/sectors.h"
#include "pw_kvs/key.h"
#include "pw_span/span.h"

namespace pw {
namespace kvs {
namespace internal {

// Caches information about a key-value entry. Facilitates quickly finding
// entries without having to read flash.
class EntryMetadata {
 public:
  using Address = FlashPartition::Address;

  EntryMetadata() = default;

  uint32_t hash() const { return descriptor_->key_hash; }

  uint32_t transaction_id() const { return descriptor_->transaction_id; }

  EntryState state() const { return descriptor_->state; }

  // The first known address of this entry.
  uint32_t first_address() const { return addresses_[0]; }

  // All addresses for this entry, including redundant entries, if any.
  const span<Address>& addresses() const { return addresses_; }

  // True if the KeyDesctiptor's transaction ID is newer than the specified ID.
  bool IsNewerThan(uint32_t other_transaction_id) const {
    // TODO(hepler): Consider handling rollover.
    return transaction_id() > other_transaction_id;
  }

  // Adds a new address to the entry metadata. MUST NOT be called more times
  // than allowed by the redundancy.
  void AddNewAddress(Address address) {
    // Each descriptor is given sufficient space in an EntryCache's address
    // buffer to meet the redundancy requirements of an EntryCache. This object
    // isn't aware of required redundancy, so there's no strict checking that
    // this contract is respected.
    addresses_ = span<Address>(addresses_.begin(), addresses_.size() + 1);
    addresses_[addresses_.size() - 1] = address;
  }

  // Remove an address from the entry metadata.
  void RemoveAddress(Address address_to_remove);

  // Resets the KeyDescrtiptor and addresses to refer to the provided
  // KeyDescriptor and address.
  void Reset(const KeyDescriptor& descriptor, Address address);

 private:
  friend class EntryCache;

  constexpr EntryMetadata(KeyDescriptor& descriptor, span<Address> addresses)
      : descriptor_(&descriptor), addresses_(addresses) {}

  KeyDescriptor* descriptor_;
  span<Address> addresses_;
};

// Tracks entry metadata. Combines KeyDescriptors and with their associated
// addresses.
class EntryCache {
 private:
  enum Constness : bool { kMutable = false, kConst = true };

  // Iterates over the EntryCache as EntryMetadata objects.
  template <Constness kIsConst>
  class Iterator {
   public:
    using value_type =
        std::conditional_t<kIsConst, const EntryMetadata, EntryMetadata>;

    Iterator& operator++() {
      ++metadata_.descriptor_;
      return *this;
    }

    Iterator operator++(int) {
      Iterator original = *this;
      operator++();
      return original;
    }

    // Updates the internal EntryMetadata object.
    value_type& operator*() const {
      metadata_.addresses_ = entry_cache_->addresses(
          metadata_.descriptor_ - entry_cache_->descriptors_.begin());
      return metadata_;
    }
    value_type* operator->() const { return &operator*(); }

    constexpr bool operator==(const Iterator& rhs) const {
      return metadata_.descriptor_ == rhs.metadata_.descriptor_;
    }
    constexpr bool operator!=(const Iterator& rhs) const {
      return metadata_.descriptor_ != rhs.metadata_.descriptor_;
    }

    // Allow non-const to convert to const.
    operator Iterator<kConst>() const {
      return {entry_cache_, metadata_.descriptor_};
    }

   private:
    friend class EntryCache;

    constexpr Iterator(const EntryCache* entry_cache, KeyDescriptor* descriptor)
        : entry_cache_(entry_cache), metadata_(*descriptor, {}) {}

    const EntryCache* entry_cache_;

    // Mark this mutable so it can be updated in the const operator*() method.
    // This allows lazy updating of the EntryMetadata.
    mutable EntryMetadata metadata_;
  };

 public:
  using iterator = Iterator<kMutable>;
  using const_iterator = Iterator<kConst>;

  using Address = FlashPartition::Address;

  // The type to use for an address list with the specified number of entries
  // and redundancy. kRedundancy extra entries are added to make room for a
  // temporary list of entry addresses.
  template <size_t kMaxEntries, size_t kRedundancy>
  using AddressList = Address[kMaxEntries * kRedundancy + kRedundancy];

  constexpr EntryCache(Vector<KeyDescriptor>& descriptors,
                       Address* addresses,
                       size_t redundancy)
      : descriptors_(descriptors),
        addresses_(addresses),
        redundancy_(redundancy) {}

  // Clears all KeyDescriptors.
  void Reset() const { descriptors_.clear(); }

  // Finds the metadata for an entry matching a particular key. Searches for a
  // KeyDescriptor that matches this key and sets *metadata to point to it if
  // one is found.
  //
  //             OK: there is a matching descriptor and *metadata is set
  //      NOT_FOUND: there is no descriptor that matches this key, but this key
  //                 has a unique hash (and could potentially be added to the
  //                 KVS)
  // ALREADY_EXISTS: there is no descriptor that matches this key, but the
  //                 key's hash collides with the hash for an existing
  //                 descriptor
  //
  StatusWithSize Find(FlashPartition& partition,
                      const Sectors& sectors,
                      const EntryFormats& formats,
                      Key key,
                      EntryMetadata* metadata) const;

  // Adds a new descriptor to the descriptor list. The entry MUST be unique and
  // the EntryCache must NOT be full!
  EntryMetadata AddNew(const KeyDescriptor& descriptor, Address address) const;

  // Adds a new descriptor, overwrites an existing one, or adds an additional
  // redundant address to one. The sector size is included for checking that
  // redundant entries are in different sectors.
  Status AddNewOrUpdateExisting(const KeyDescriptor& descriptor,
                                Address address,
                                size_t sector_size_bytes) const;

  // Removes an existing entry from the cache. Returns an iterator to the
  // next entry so that iteration can continue.
  iterator RemoveEntry(iterator& entry_it);

  // Returns a pointer to an array of redundancy() addresses for temporary use.
  // This is used by the KeyValueStore to track reserved addresses when finding
  // space for a new entry.
  Address* TempReservedAddressesForWrite() const {
    return &addresses_[descriptors_.max_size() * redundancy_];
  }

  // The number of copies of each entry.
  size_t redundancy() const { return redundancy_; }

  // True if no more entries can be added to the cache.
  bool full() const { return descriptors_.full(); }

  // The total number of entries, including tombstone entries.
  size_t total_entries() const { return descriptors_.size(); }

  // The total number of present (non-tombstone) entries.
  size_t present_entries() const;

  // The maximum number of entries supported by this EntryCache.
  size_t max_entries() const { return descriptors_.max_size(); }

  iterator begin() const { return {this, descriptors_.begin()}; }
  const_iterator cbegin() const { return {this, descriptors_.begin()}; }

  iterator end() const { return {this, descriptors_.end()}; }
  const_iterator cend() const { return {this, descriptors_.end()}; }

 private:
  int FindIndex(uint32_t key_hash) const;

  // Adds the address to the descriptor at the specified index if there is an
  // address slot available.
  void AddAddressIfRoom(size_t descriptor_index, Address address) const;

  // Returns a span of the valid addresses for the descriptor.
  span<Address> addresses(size_t descriptor_index) const;

  Address* first_address(size_t descriptor_index) const {
    return &addresses_[descriptor_index * redundancy_];
  }

  Address* ResetAddresses(size_t descriptor_index, Address address) const;

  Vector<KeyDescriptor>& descriptors_;
  FlashPartition::Address* const addresses_;
  const size_t redundancy_;
};

}  // namespace internal
}  // namespace kvs
}  // namespace pw
