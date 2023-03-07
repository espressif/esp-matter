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

// Provides an implementation of the CRC-16-CCITT or CRC-CCITT checksum, which
// uses the polynomial 0x1021:
//
//   x^16 + x^12 + x^5 + 1
//
// with initial value 0xFFFF. See https://www.zlib.net/crc_v3.txt.
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// C API for calculating the CRC-16-CCITT of an array of data.
uint16_t pw_checksum_Crc16Ccitt(const void* data,
                                size_t size_bytes,
                                uint16_t initial_value);

#ifdef __cplusplus
}  // extern "C"

#include "pw_bytes/span.h"
#include "pw_span/span.h"

namespace pw::checksum {

// Calculates the CRC-16-CCITT for all data passed to Update.
class Crc16Ccitt {
 public:
  static constexpr uint16_t kInitialValue = 0xFFFF;

  // Calculates the CRC-16-CCITT for the provided data and returns it as a
  // uint16_t. To update a CRC in multiple calls, use an instance of the
  // Crc16Ccitt class or pass the previous value as the initial_value argument.
  static uint16_t Calculate(span<const std::byte> data,
                            uint16_t initial_value = kInitialValue) {
    return pw_checksum_Crc16Ccitt(
        data.data(), data.size_bytes(), initial_value);
  }

  static uint16_t Calculate(std::byte data,
                            uint16_t initial_value = kInitialValue) {
    return Calculate(ConstByteSpan(&data, 1), initial_value);
  }

  constexpr Crc16Ccitt() : value_(kInitialValue) {}

  void Update(span<const std::byte> data) { value_ = Calculate(data, value_); }

  void Update(std::byte data) { Update(ByteSpan(&data, 1)); }

  // Returns the value of the CRC-16-CCITT for all data passed to Update.
  uint16_t value() const { return value_; }

  // Resets the CRC to the initial value.
  void clear() { value_ = kInitialValue; }

 private:
  uint16_t value_;
};

}  // namespace pw::checksum

#endif  // __cplusplus
