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

#include "pw_checksum/crc32.h"

namespace pw::checksum {
namespace {

// Calculates the partial CRC32 of the low order kBits of value using
// the reversed polynomial kPolynomial. This is a building block for
// both implementing a tableless CRC32 implementation as well as generating
// look up tables for tables based implementations.
//
// Information on CRC32 can be found at:
//   https://en.wikipedia.org/wiki/Cyclic_redundancy_check
template <std::size_t kBits, uint32_t kPolynomial>
constexpr uint32_t Crc32ProcessDataChunk(uint32_t value) {
  for (uint32_t j = 0; j < kBits; j++) {
    value = (value >> 1u) ^
            (static_cast<uint32_t>(-static_cast<int32_t>(value & 1u)) &
             kPolynomial);
  }
  return value;
}

// Generates a lookup table for a table based CRC32 implementation.
// The table pre-computes the CRC for every value representable by
// kBits of data. kPolynomial is used as the reversed polynomial
// for the computation. The returned table will have 2^kBits entries.
template <std::size_t kBits, uint32_t kPolynomial>
constexpr std::array<uint32_t, (1 << kBits)> GenerateCrc32Table() {
  std::array<uint32_t, (1 << kBits)> table{};
  for (uint32_t i = 0; i < (1 << kBits); i++) {
    table[i] = Crc32ProcessDataChunk<kBits, kPolynomial>(i);
  }
  return table;
}

// Reversed polynomial for the commonly used CRC32 variant. See:
// https://en.wikipedia.org/wiki/Cyclic_redundancy_check#Polynomial_representations_of_cyclic_redundancy_checks
constexpr uint32_t kCrc32Polynomial = 0xEDB88320;

}  // namespace

extern "C" uint32_t _pw_checksum_InternalCrc32EightBit(const void* data,
                                                       size_t size_bytes,
                                                       uint32_t state) {
  static constexpr std::array<uint32_t, 256> kCrc32Table =
      GenerateCrc32Table<8, kCrc32Polynomial>();
  const uint8_t* data_bytes = static_cast<const uint8_t*>(data);

  for (size_t i = 0; i < size_bytes; ++i) {
    state = kCrc32Table[(state ^ data_bytes[i]) & 0xFFu] ^ (state >> 8);
  }

  return state;
}

extern "C" uint32_t _pw_checksum_InternalCrc32FourBit(const void* data,
                                                      size_t size_bytes,
                                                      uint32_t state) {
  static constexpr std::array<uint32_t, 16> kCrc32Table =
      GenerateCrc32Table<4, kCrc32Polynomial>();
  const uint8_t* data_bytes = static_cast<const uint8_t*>(data);

  for (size_t i = 0; i < size_bytes; ++i) {
    state ^= data_bytes[i];
    state = kCrc32Table[state & 0x0f] ^ (state >> 4);
    state = kCrc32Table[state & 0x0f] ^ (state >> 4);
  }

  return state;
}

extern "C" uint32_t _pw_checksum_InternalCrc32OneBit(const void* data,
                                                     size_t size_bytes,
                                                     uint32_t state) {
  const uint8_t* data_bytes = static_cast<const uint8_t*>(data);

  for (size_t i = 0; i < size_bytes; ++i) {
    state = Crc32ProcessDataChunk<8, kCrc32Polynomial>(state ^ data_bytes[i]);
  }

  return state;
}

}  // namespace pw::checksum
