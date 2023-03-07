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
#include <limits>

#include "pw_assert/assert.h"

namespace pw::protobuf {

// Per the protobuf specification, valid field numbers range between 1 and
// 2**29 - 1, inclusive. The numbers 19000-19999 are reserved for internal
// use.
constexpr static uint32_t kMaxFieldNumber = (1u << 29) - 1;
constexpr static uint32_t kFirstReservedNumber = 19000;
constexpr static uint32_t kLastReservedNumber = 19999;

constexpr bool ValidFieldNumber(uint32_t field_number) {
  return field_number != 0 && field_number <= kMaxFieldNumber &&
         !(field_number >= kFirstReservedNumber &&
           field_number <= kLastReservedNumber);
}

constexpr bool ValidFieldNumber(uint64_t field_number) {
  if (field_number > std::numeric_limits<uint32_t>::max()) {
    return false;
  }
  return ValidFieldNumber(static_cast<uint32_t>(field_number));
}

enum class WireType {
  kVarint = 0,
  kFixed64 = 1,
  kDelimited = 2,
  // Wire types 3 and 4 are deprecated per the protobuf specification.
  kFixed32 = 5,
};

// Represents a protobuf field key, storing a field number and wire type.
class FieldKey {
 public:
  // Checks if the given encoded protobuf key is valid. Must be called before
  // instantiating a FieldKey object with it.
  static constexpr bool IsValidKey(uint64_t key) {
    uint64_t field_number = key >> kFieldNumberShift;
    uint32_t wire_type = key & kWireTypeMask;

    return ValidFieldNumber(field_number) && (wire_type <= 2 || wire_type == 5);
  }

  // Creates a field key with the given field number and type.
  //
  // Precondition: The field number is valid.
  constexpr FieldKey(uint32_t field_number, WireType wire_type)
      : key_(field_number << kFieldNumberShift |
             static_cast<uint32_t>(wire_type)) {
    PW_DASSERT(ValidFieldNumber(field_number));
  }

  // Parses a field key from its encoded representation.
  //
  // Precondition: The field number is valid. Call IsValidKey(key) first.
  constexpr FieldKey(uint32_t key) : key_(key) {
    PW_DASSERT(ValidFieldNumber(field_number()));
  }

  constexpr operator uint32_t() { return key_; }

  constexpr uint32_t field_number() const { return key_ >> kFieldNumberShift; }
  constexpr WireType wire_type() const {
    return static_cast<WireType>(key_ & kWireTypeMask);
  }

 private:
  static constexpr unsigned int kFieldNumberShift = 3u;
  static constexpr unsigned int kWireTypeMask = (1u << kFieldNumberShift) - 1u;

  uint32_t key_;
};

[[deprecated("Use the FieldKey class")]] constexpr uint32_t MakeKey(
    uint32_t field_number, WireType wire_type) {
  return FieldKey(field_number, wire_type);
}

}  // namespace pw::protobuf
