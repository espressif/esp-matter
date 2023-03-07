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

// This file defines classes for managing the in-flash format for KVS entires.
#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

#include "pw_kvs/alignment.h"
#include "pw_kvs/checksum.h"
#include "pw_kvs/flash_memory.h"
#include "pw_kvs/format.h"
#include "pw_kvs/internal/hash.h"
#include "pw_kvs/internal/key_descriptor.h"
#include "pw_kvs/key.h"
#include "pw_span/span.h"

namespace pw {
namespace kvs {
namespace internal {

// Entry represents a key-value entry in a flash partition.
class Entry {
 public:
  static constexpr size_t kMinAlignmentBytes = sizeof(EntryHeader);
  static constexpr size_t kMaxKeyLength = 0b111111;

  using Address = FlashPartition::Address;

  // Buffer capable of holding any valid key (without a null terminator);
  using KeyBuffer = std::array<char, kMaxKeyLength>;

  // Returns flash partition Read error codes, or one of the following:
  //
  //          OK: successfully read the header and initialized the Entry
  //   NOT_FOUND: read the header, but the data appears to be erased
  //   DATA_LOSS: read the header, but it contained invalid data
  //
  static Status Read(FlashPartition& partition,
                     Address address,
                     const internal::EntryFormats& formats,
                     Entry* entry);

  // Reads a key into a buffer, which must be at least key_length bytes.
  static Status ReadKey(FlashPartition& partition,
                        Address address,
                        size_t key_length,
                        char* key);

  // Creates a new Entry for a valid (non-deleted) entry.
  static Entry Valid(FlashPartition& partition,
                     Address address,
                     const EntryFormat& format,
                     Key key,
                     span<const std::byte> value,
                     uint32_t transaction_id) {
    return Entry(
        partition, address, format, key, value, value.size(), transaction_id);
  }

  // Creates a new Entry for a tombstone entry, which marks a deleted key.
  static Entry Tombstone(FlashPartition& partition,
                         Address address,
                         const EntryFormat& format,
                         Key key,
                         uint32_t transaction_id) {
    return Entry(partition,
                 address,
                 format,
                 key,
                 {},
                 kDeletedValueLength,
                 transaction_id);
  }

  Entry() = default;

  KeyDescriptor descriptor(Key key) const { return descriptor(Hash(key)); }

  KeyDescriptor descriptor(uint32_t key_hash) const {
    return KeyDescriptor{key_hash,
                         transaction_id(),
                         deleted() ? EntryState::kDeleted : EntryState::kValid};
  }

  StatusWithSize Write(Key key, span<const std::byte> value) const;

  // Changes the format and transcation ID for this entry. In order to calculate
  // the new checksum, the entire entry is read into a small stack-allocated
  // buffer. The updated entry may be written to flash using the Copy function.
  Status Update(const EntryFormat& new_format, uint32_t new_transaction_id);

  // Writes this entry at a new address. The key and value are read from the
  // entry's current address. The Entry object's header, which may be newer than
  // what is in flash, is used.
  StatusWithSize Copy(Address new_address) const;

  // Reads a key into a buffer, which must be large enough for a max-length key.
  // If successful, the size is returned in the StatusWithSize. The key is not
  // null terminated.
  template <size_t kSize>
  StatusWithSize ReadKey(std::array<char, kSize>& key) const {
    static_assert(kSize >= kMaxKeyLength);
    return StatusWithSize(
        ReadKey(partition(), address_, key_length(), key.data()), key_length());
  }

  StatusWithSize ReadValue(span<std::byte> buffer,
                           size_t offset_bytes = 0) const;

  Status ValueMatches(span<const std::byte> value) const;

  Status VerifyChecksum(Key key, span<const std::byte> value) const;

  Status VerifyChecksumInFlash() const;

  // Calculates the total size of an entry, including padding.
  static size_t size(const FlashPartition& partition,
                     Key key,
                     span<const std::byte> value) {
    return AlignUp(sizeof(EntryHeader) + key.size() + value.size(),
                   std::max(partition.alignment_bytes(), kMinAlignmentBytes));
  }

  // Byte size of overhead (not-key, not-value) in an entry. Does not include
  // any paddding used to get proper size alignment.
  static constexpr size_t entry_overhead() { return sizeof(EntryHeader); }

  Address address() const { return address_; }

  void set_address(Address address) { address_ = address; }

  // The address at which the next possible entry could be located.
  Address next_address() const { return address() + size(); }

  // Total size of this entry, including padding.
  size_t size() const { return AlignUp(content_size(), alignment_bytes()); }

  // The length of the key in bytes. Keys are not null terminated.
  size_t key_length() const { return header_.key_length_bytes; }

  // The size of the value, without padding. The size is 0 if this is a
  // tombstone entry.
  size_t value_size() const {
    return deleted() ? 0u : header_.value_size_bytes;
  }

  uint32_t magic() const { return header_.magic; }

  uint32_t transaction_id() const { return header_.transaction_id; }

  // True if this is a tombstone entry.
  bool deleted() const {
    return header_.value_size_bytes == kDeletedValueLength;
  }

  void DebugLog() const;

 private:
  static constexpr uint16_t kDeletedValueLength = 0xFFFF;

  Entry(FlashPartition& partition,
        Address address,
        const EntryFormat& format,
        Key key,
        span<const std::byte> value,
        uint16_t value_size_bytes,
        uint32_t transaction_id);

  constexpr Entry(FlashPartition* partition,
                  Address address,
                  const EntryFormat& format,
                  EntryHeader header)
      : partition_(partition),
        address_(address),
        checksum_algo_(format.checksum),
        header_(header) {}

  FlashPartition& partition() const { return *partition_; }

  size_t alignment_bytes() const { return (header_.alignment_units + 1) * 16; }

  // The total size of the entry, excluding padding.
  size_t content_size() const {
    return sizeof(EntryHeader) + key_length() + value_size();
  }

  span<const std::byte> checksum_bytes() const {
    return as_bytes(span<const uint32_t>(&header_.checksum, 1));
  }

  span<const std::byte> CalculateChecksum(Key key,
                                          span<const std::byte> value) const;

  Status CalculateChecksumFromFlash();

  // Update the checksum with 0s to pad the entry to its alignment boundary.
  void AddPaddingBytesToChecksum() const;

  static constexpr uint8_t alignment_bytes_to_units(size_t alignment_bytes) {
    return (alignment_bytes + 15) / 16 - 1;  // An alignment of 0 is invalid.
  }

  FlashPartition* partition_;
  Address address_;
  ChecksumAlgorithm* checksum_algo_;
  EntryHeader header_;
};

}  // namespace internal
}  // namespace kvs
}  // namespace pw
