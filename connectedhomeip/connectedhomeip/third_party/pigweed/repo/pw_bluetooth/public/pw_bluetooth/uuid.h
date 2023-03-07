// Copyright 2022 The Pigweed Authors
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
#include <climits>
#include <cstdint>

#include "pw_assert/assert.h"
#include "pw_bluetooth/internal/hex.h"
#include "pw_span/span.h"
#include "pw_string/string.h"

namespace pw::bluetooth {

// A 128-bit Universally Unique Identifier (UUID).
// See Core Spec v5.3 Volume 3, Part B, Section 2.5.1.
//
// Bluetooth defines 16-bit, 32-bit and 128-bit UUID representations for a
// 128-bit UUID, all of which are used in the protocol. 16-bit UUIDs values
// define only the "YYYY" portion in the following UUID pattern (with XXXX set
// as 0), while 32-bit UUID value define the "XXXXYYYY" portion. When using
// these short UUIDs, the remaining bits are set by the Bluetooth_Base_UUID as
// follows:
//   XXXXYYYY-0000-1000-8000-00805f9b34fb
//
// This class always stores UUID in their 128-bit representation in little
// endian format.
class Uuid {
 public:
  // String size of a hexadecimal representation of a UUID, not including the
  // null terminator.
  static constexpr size_t kHexStringSize = 36;

  // Create a UUID from a span of 128-bit data. UUIDs are represented as
  // little endian bytes.
  explicit constexpr Uuid(const span<const uint8_t, 16> uuid_span) : uuid_() {
    for (size_t i = 0; i < sizeof(uuid_); i++) {
      uuid_[i] = uuid_span[i];
    }
  }

  // Create a UUID from its string representation. This is parsed manually here
  // so it can be parsed at compile time with constexpr. A valid UUID is a hex
  // string with hyphen separators at the 9th, 14th, 19th, and 24th
  // positions, for example:
  //   "0000180a-0000-1000-8000-00805f9b34fb"
  // The `str` parameter is byte longer than kHexStringSize so it can be
  // initialized with literal strings including the null terminator, as in
  // BluetoothBase().
  constexpr Uuid(const char (&str)[kHexStringSize + 1]) : uuid_() {
    size_t out_hex_index = 2 * sizeof(uuid_);  // UUID is stored little-endian.
    for (size_t i = 0; i < kHexStringSize; i++) {
      // Indices at which we expect to find a hyphen ('-') in a UUID string.
      if (i == 8 || i == 13 || i == 18 || i == 23) {
        PW_ASSERT(str[i] == '-');
        continue;
      }
      PW_ASSERT(str[i] != 0);
      out_hex_index--;
      uint16_t value = internal::HexToNibble(str[i]);
      PW_ASSERT(value <= 0xf);
      if (out_hex_index % 2 == 0) {
        uuid_[out_hex_index / 2] |= value;
      } else {
        uuid_[out_hex_index / 2] = value << 4;
      }
    }
  }

  // The Bluetooth_Base_UUID defined by the specification. This is the base for
  // all 16-bit and 32-bit short UUIDs.
  static constexpr const Uuid& BluetoothBase();

  // Create a UUID combining 96-bits from a base UUID with a 16-bit or 32-bit
  // value. 16-bit values will be extended to 32-bit ones, meaning the that the
  // 16 most significant bits will be set to 0 regardless of the value on the
  // base UUID.
  constexpr Uuid(uint32_t short_uuid, const Uuid& base_uuid)
      : uuid_(base_uuid.uuid_) {
    uuid_[kBaseOffset] = short_uuid & 0xff;
    uuid_[kBaseOffset + 1] = (short_uuid >> CHAR_BIT) & 0xff;
    uuid_[kBaseOffset + 2] = (short_uuid >> CHAR_BIT * 2) & 0xff;
    uuid_[kBaseOffset + 3] = (short_uuid >> CHAR_BIT * 3) & 0xff;
  }

  // Create a short UUID (32-bit or 16-bit) using the standard Bluetooth base
  // UUID.
  explicit constexpr Uuid(uint32_t short_uuid)
      : Uuid(short_uuid, BluetoothBase()) {}

  constexpr Uuid(const Uuid&) = default;
  constexpr Uuid& operator=(const Uuid&) = default;

  // Return a 2-byte span containing the 16-bit little endian representation of
  // the UUID. This is useful when Same112BitBase(BluetoothBase()) is true.
  constexpr span<const uint8_t, 2> As16BitSpan() const {
    return span<const uint8_t, 2>{uuid_.data() + kBaseOffset, 2u};
  }

  // Return a 4-byte span containing the 32-bit little endian representation of
  // the UUID. This is useful when Same96BitBase(BluetoothBase()) is true.
  constexpr span<const uint8_t, 4> As32BitSpan() const {
    return span<const uint8_t, 4>{uuid_.data() + kBaseOffset, 4u};
  }

  // Return the 128-bit (16-byte) little endian representation of the UUID.
  constexpr span<const uint8_t, 16> As128BitSpan() const {
    return span<const uint8_t, 16>{uuid_.data(), 16u};
  }

  // Return whether the UUID shares the same 112-bit base with another UUID.
  // Sharing the same 112-bit base with BluetoothBase() means that this UUID
  // can be resented as a 16-bit UUID.
  constexpr bool Same112BitBase(const Uuid& other) const {
    return Same96BitBase(other) && uuid_[14] == other.uuid_[14] &&
           uuid_[15] == other.uuid_[15];
  }

  // Return whether the UUID shares the same 96-bit base with another UUID.
  // Sharing the same 96-bit base with BluetoothBase() means that this UUID
  // can be resented as a 32-bit UUID.
  constexpr bool Same96BitBase(const Uuid& other) const {
    for (size_t i = 0; i < 12; i++) {
      if (uuid_[i] != other.uuid_[i])
        return false;
    }
    return true;
  }

  // Return whether the UUID is a 16-bit UUID represented as 128-bit using the
  // BluetoothBase() as the base.
  constexpr bool Is16BitUuid() const { return Same112BitBase(BluetoothBase()); }

  // Return whether the UUID is a 32-bit UUID represented as 128-bit using the
  // BluetoothBase() as the base.
  constexpr bool Is32BitUuid() const { return Same96BitBase(BluetoothBase()); }

  // Return an inline pw_string representation of the UUID in hexadecimal.
  constexpr InlineString<kHexStringSize> ToString() const {
    InlineString<kHexStringSize> ret;
    for (size_t i = uuid_.size(); i-- != 0;) {
      ret += internal::NibbleToHex(uuid_[i] >> 4);
      ret += internal::NibbleToHex(uuid_[i] & 0xf);
      if ((i == 12) || (i == 10) || (i == 8) || (i == 6)) {
        ret += '-';
      }
    }
    return ret;
  }

 private:
  // Offset at which the short 16-bit and 32-bit UUID little-endian data starts
  // in the uuid_ array.
  static constexpr size_t kBaseOffset = 12;

  std::array<uint8_t, 16> uuid_;
};

namespace internal {
// When BluetoothBase() is used in constexpr expressions it would normally be
// evaluated to a final different Uuid, such as when used in Uuid(uint32_t),
// however if a reference to the return value of BluetoothBase() is needed this
// variable would be the only global symbol that provides it even if it is used
// from multiple translation units.
constexpr Uuid kBluetoothBaseUuid{"00000000-0000-1000-8000-00805F9B34FB"};
}  // namespace internal

inline constexpr const Uuid& Uuid::BluetoothBase() {
  return internal::kBluetoothBaseUuid;
}

// Uuid comparators:
constexpr bool operator==(const Uuid& a, const Uuid& b) {
  const auto a_span = a.As128BitSpan();
  const auto b_span = b.As128BitSpan();
  for (size_t i = 0; i < a_span.size(); i++) {
    if (a_span[i] != b_span[i]) {
      return false;
    }
  }
  return true;
}

constexpr bool operator!=(const Uuid& a, const Uuid& b) { return !(a == b); }

}  // namespace pw::bluetooth
