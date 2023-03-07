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
#include <cstdint>

#include "pw_assert/assert.h"
#include "pw_bluetooth/internal/hex.h"
#include "pw_span/span.h"
#include "pw_string/string.h"

namespace pw::bluetooth {

// A 48-bit bluetooth device address (BD_ADDR) in little endian format.
// See Core Spec v5.3 Volume 2, Part B, Section 1.2.
class Address {
 public:
  // String size of a hexadecimal representation of an Address, not including
  // the null terminator.
  static constexpr size_t kHexStringSize = 17;

  // Create an Address from its binary representation.
  // The first byte in the span is the last one in the hex representation, thus
  // the BD_ADDR 00:11:22:33:44:55 should be created from the span with bytes:
  // {0x55, 0x44, 0x33, 0x22, 0x11, 0x00}.
  constexpr Address(const span<const uint8_t, 6> addr_span) : addr_() {
    static_assert(addr_span.size() == sizeof(addr_));
    for (size_t i = 0; i < sizeof(addr_); i++) {
      addr_[i] = addr_span[i];
    }
  }

  // Create an address from the hex format "00:11:22:33:44:55". The passed
  // string must have a length of 17 and a ":" character on the 3rd, 6th, 9th,
  // 12th and 15th positions. The hexadecimal representation is such that the
  // first byte in the string is the last byte in the binary representation.
  constexpr Address(const char (&str_addr)[kHexStringSize + 1]) : addr_() {
    PW_ASSERT((str_addr[2] == ':') && (str_addr[5] == ':') &&
              (str_addr[8] == ':') && (str_addr[11] == ':') &&
              (str_addr[14] == ':'));
    for (size_t i = 0; i < sizeof(addr_); i++) {
      uint16_t value = (internal::HexToNibble(str_addr[3 * i]) << 4u) |
                       internal::HexToNibble(str_addr[3 * i + 1]);
      addr_[sizeof(addr_) - 1 - i] = value;
      PW_ASSERT(value <= 0xff);
    }
  }

  // Return the bluetooth address a the 6-byte binary representation.
  constexpr span<const uint8_t, 6> AsSpan() const {
    return span<const uint8_t, 6>{addr_.data(), addr_.size()};
  }

  // Return an inline pw_string representation of the Address in hexadecimal
  // using ":" characters as byte separator.
  constexpr InlineString<kHexStringSize> ToString() const {
    InlineString<kHexStringSize> ret;
    for (size_t i = addr_.size(); i-- != 0;) {
      ret += internal::NibbleToHex(addr_[i] >> 4);
      ret += internal::NibbleToHex(addr_[i] & 0xf);
      if (i) {
        ret += ':';
      }
    }
    return ret;
  }

 private:
  std::array<uint8_t, 6> addr_;
};

// Address comparators:
constexpr bool operator==(const Address& a, const Address& b) {
  const auto a_span = a.AsSpan();
  const auto b_span = b.AsSpan();
  for (size_t i = 0; i < a_span.size(); i++) {
    if (a_span[i] != b_span[i]) {
      return false;
    }
  }
  return true;
}

constexpr bool operator!=(const Address& a, const Address& b) {
  return !(a == b);
}

}  // namespace pw::bluetooth
