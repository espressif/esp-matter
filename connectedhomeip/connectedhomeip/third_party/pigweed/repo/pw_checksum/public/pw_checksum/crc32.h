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

// CRC-32 (CRC32) implementation with initial value 0xFFFFFFFF. This provides C
// functions and a C++ class. Use of the C API is discouraged; use the Crc32
// class whevener possible.
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "pw_checksum/internal/config.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// Value of an empty CRC32. May be serve as the starting CRC32 value for
// pw_checksum_Crc32Append.
#define PW_CHECKSUM_EMPTY_CRC32 ~_PW_CHECKSUM_CRC32_INITIAL_STATE

// The initial state for internal CRC32 calculations. Do not use this value
// directly.
#define _PW_CHECKSUM_CRC32_INITIAL_STATE 0xFFFFFFFFu

// Internal implementation function for CRC32. Do not call it directly.
uint32_t _pw_checksum_InternalCrc32EightBit(const void* data,
                                            size_t size_bytes,
                                            uint32_t state);

uint32_t _pw_checksum_InternalCrc32FourBit(const void* data,
                                           size_t size_bytes,
                                           uint32_t state);
uint32_t _pw_checksum_InternalCrc32OneBit(const void* data,
                                          size_t size_bytes,
                                          uint32_t state);

#if PW_CHECKSUM_CRC32_DEFAULT_IMPL == PW_CHECKSUM_CRC32_8BITS
#define _pw_checksum_InternalCrc32 _pw_checksum_InternalCrc32EightBit
#elif PW_CHECKSUM_CRC32_DEFAULT_IMPL == PW_CHECKSUM_CRC32_4BITS
#define _pw_checksum_InternalCrc32 _pw_checksum_InternalCrc32FourBit
#elif PW_CHECKSUM_CRC32_DEFAULT_IMPL == PW_CHECKSUM_CRC32_1BITS
#define _pw_checksum_InternalCrc32 _pw_checksum_InternalCrc32OneBit
#endif

// Calculates the CRC32 for the provided data.
static inline uint32_t pw_checksum_Crc32(const void* data, size_t size_bytes) {
  return ~_pw_checksum_InternalCrc32(
      data, size_bytes, _PW_CHECKSUM_CRC32_INITIAL_STATE);
}

// Updates an existing CRC value. The previous_result must have been returned
// from a previous CRC32 call.
static inline uint32_t pw_checksum_Crc32Append(const void* data,
                                               size_t size_bytes,
                                               uint32_t previous_result) {
  // CRC32 values are finalized by inverting the bits. The finalization step
  // must be undone before appending to a prior CRC32 value, then redone so this
  // function returns a usable value after each call.
  return ~_pw_checksum_InternalCrc32(data, size_bytes, ~previous_result);
}

#ifdef __cplusplus
}  // extern "C"

#include "pw_span/span.h"

namespace pw::checksum {

// Calculates the CRC32 for all data passed to Update.
//
// This class is more efficient than the CRC32 C functions since it doesn't
// finalize the value each time it is appended to.
template <uint32_t (*kChecksumFunction)(const void*, size_t, uint32_t)>
class Crc32Impl {
 public:
  // Calculates the CRC32 for the provided data and returns it as a uint32_t.
  // To update a CRC in multiple pieces, use an instance of the Crc32 class.
  static uint32_t Calculate(span<const std::byte> data) {
    return ~kChecksumFunction(
        data.data(), data.size_bytes(), _PW_CHECKSUM_CRC32_INITIAL_STATE);
  }

  constexpr Crc32Impl() : state_(kInitialValue) {}

  void Update(span<const std::byte> data) {
    state_ = kChecksumFunction(data.data(), data.size(), state_);
  }

  void Update(std::byte data) { Update(span(&data, 1)); }

  // Returns the value of the CRC32 for all data passed to Update.
  uint32_t value() const { return ~state_; }

  // Resets the CRC to the initial value.
  void clear() { state_ = kInitialValue; }

 private:
  static constexpr uint32_t kInitialValue = _PW_CHECKSUM_CRC32_INITIAL_STATE;

  uint32_t state_;
};

using Crc32 = Crc32Impl<_pw_checksum_InternalCrc32>;
using Crc32EightBit = Crc32Impl<_pw_checksum_InternalCrc32EightBit>;
using Crc32FourBit = Crc32Impl<_pw_checksum_InternalCrc32FourBit>;
using Crc32OneBit = Crc32Impl<_pw_checksum_InternalCrc32OneBit>;

}  // namespace pw::checksum

#endif  // __cplusplus
