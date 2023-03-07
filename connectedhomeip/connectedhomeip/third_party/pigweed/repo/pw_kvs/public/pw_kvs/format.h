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

#include <cstdint>

#include "pw_kvs/checksum.h"
#include "pw_span/span.h"

namespace pw {
namespace kvs {

// The EntryFormat defines properties of KVS entries that use a particular magic
// number.
struct EntryFormat {
  // Magic is a unique constant identifier for entries.
  //
  // Upon reading from an address in flash, the magic number facilitiates
  // quickly differentiating between:
  //
  // - Reading erased data - typically 0xFF - from flash.
  // - Reading corrupted data
  // - Reading a valid entry
  //
  // When selecting a magic for your particular KVS, pick a random 32 bit
  // integer rather than a human readable 4 bytes. This decreases the
  // probability of a collision with a real string when scanning in the case of
  // corruption. To generate such a number:
  /*
       $ python3 -c 'import random; print(hex(random.randint(0,2**32)))'
       0xaf741757
  */
  uint32_t magic;

  // The checksum algorithm is used to calculate checksums for KVS entries. If
  // it is null, no checksum is used.
  ChecksumAlgorithm* checksum;
};

namespace internal {

// Disk format of the header used for each key-value entry.
struct EntryHeader {
  // For KVS magic value always use a random 32 bit integer rather than a
  // human readable 4 bytes. See pw_kvs/format.h::EntryFormat for more
  // information.
  uint32_t magic;

  // The checksum of the entire entry, including the header, key, value, and
  // zero-value padding bytes. The checksum is calculated as if the checksum
  // field value was zero.
  uint32_t checksum;

  // Stores the alignment in 16-byte units, starting from 16. To calculate the
  // number of bytes, add one to this number and multiply by 16.
  uint8_t alignment_units;

  // The length of the key in bytes. The key is not null terminated.
  //  6 bits, 0:5 - key length - maximum 64 characters
  //  2 bits, 6:7 - reserved
  uint8_t key_length_bytes;

  // Byte length of the value; maximum of 65534. The max uint16_t value (65535
  // or 0xFFFF) is reserved to indicate this is a tombstone (deleted) entry.
  uint16_t value_size_bytes;

  // The transaction ID for this key. Monotonically increasing.
  uint32_t transaction_id;
};

static_assert(sizeof(EntryHeader) == 16, "EntryHeader must not have padding");

// This class wraps EntryFormat instances to support having multiple
// simultaneously supported formats.
class EntryFormats {
 public:
  explicit constexpr EntryFormats(span<const EntryFormat> formats)
      : formats_(formats) {}

  explicit constexpr EntryFormats(const EntryFormat& format)
      : formats_(&format, 1) {}

  const EntryFormat& primary() const { return formats_.front(); }

  bool KnownMagic(uint32_t magic) const { return Find(magic) != nullptr; }

  const EntryFormat* Find(uint32_t magic) const;

 private:
  const span<const EntryFormat> formats_;
};

}  // namespace internal
}  // namespace kvs
}  // namespace pw
