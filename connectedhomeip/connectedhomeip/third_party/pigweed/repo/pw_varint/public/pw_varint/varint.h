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

#include <stddef.h>
#include <stdint.h>

#include "pw_preprocessor/compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

// Expose a subset of the varint API for use in C code.

typedef enum {
  PW_VARINT_ZERO_TERMINATED_LEAST_SIGNIFICANT = 0,
  PW_VARINT_ZERO_TERMINATED_MOST_SIGNIFICANT = 1,
  PW_VARINT_ONE_TERMINATED_LEAST_SIGNIFICANT = 2,
  PW_VARINT_ONE_TERMINATED_MOST_SIGNIFICANT = 3,
} pw_varint_Format;

size_t pw_varint_EncodeCustom(uint64_t integer,
                              void* output,
                              size_t output_size,
                              pw_varint_Format format);
size_t pw_varint_DecodeCustom(const void* input,
                              size_t input_size,
                              uint64_t* output,
                              pw_varint_Format format);

static inline size_t pw_varint_Encode(uint64_t integer,
                                      void* output,
                                      size_t output_size) {
  return pw_varint_EncodeCustom(
      integer, output, output_size, PW_VARINT_ZERO_TERMINATED_MOST_SIGNIFICANT);
}

size_t pw_varint_ZigZagEncode(int64_t integer,
                              void* output,
                              size_t output_size);

static inline size_t pw_varint_Decode(const void* input,
                                      size_t input_size,
                                      uint64_t* output) {
  return pw_varint_DecodeCustom(
      input, input_size, output, PW_VARINT_ZERO_TERMINATED_MOST_SIGNIFICANT);
}

size_t pw_varint_ZigZagDecode(const void* input,
                              size_t input_size,
                              int64_t* output);

// Returns the size of an when encoded as a varint.
size_t pw_varint_EncodedSize(uint64_t integer);
size_t pw_varint_ZigZagEncodedSize(int64_t integer);

#ifdef __cplusplus

}  // extern "C"

#include <limits>
#include <type_traits>

#include "pw_polyfill/language_feature_macros.h"
#include "pw_span/span.h"

namespace pw {
namespace varint {

// The maximum number of bytes occupied by an encoded varint.
PW_INLINE_VARIABLE constexpr size_t kMaxVarint32SizeBytes = 5;
PW_INLINE_VARIABLE constexpr size_t kMaxVarint64SizeBytes = 10;

// ZigZag encodes a signed integer. This maps small negative numbers to small,
// unsigned positive numbers, which improves their density for LEB128 encoding.
//
// ZigZag encoding works by moving the sign bit from the most-significant bit to
// the least-significant bit. For the signed k-bit integer n, the formula is
//
//   (n << 1) ^ (n >> (k - 1))
//
// See the following for a description of ZigZag encoding:
//   https://developers.google.com/protocol-buffers/docs/encoding#types
template <typename T>
constexpr std::make_unsigned_t<T> ZigZagEncode(T n) {
  static_assert(std::is_signed<T>(), "Zig-zag encoding is for signed integers");
  using U = std::make_unsigned_t<T>;
  return (static_cast<U>(n) << 1) ^ static_cast<U>(n >> (sizeof(T) * 8 - 1));
}

// ZigZag decodes a signed integer.
// The calculation is done modulo std::numeric_limits<T>::max()+1, so the
// unsigned integer overflows are intentional.
template <typename T>
constexpr std::make_signed_t<T> ZigZagDecode(T n)
    PW_NO_SANITIZE("unsigned-integer-overflow") {
  static_assert(std::is_unsigned<T>(),
                "Zig-zag decoding is for unsigned integers");
  return static_cast<std::make_signed_t<T>>((n >> 1) ^ (~(n & 1) + 1));
}

// Encodes a uint64_t with Little-Endian Base 128 (LEB128) encoding.
inline size_t EncodeLittleEndianBase128(uint64_t integer,
                                        const span<std::byte>& output) {
  return pw_varint_Encode(integer, output.data(), output.size());
}

// Encodes the provided integer using a variable-length encoding and returns the
// number of bytes written.
//
// The encoding is the same as used in protocol buffers. Signed integers are
// ZigZag encoded to remove leading 1s from small negative numbers, then the
// resulting number is encoded as Little Endian Base 128 (LEB128). Unsigned
// integers are encoded directly as LEB128.
//
// Returns the number of bytes written or 0 if the result didn't fit in the
// encoding buffer.
template <typename T>
size_t Encode(T integer, const span<std::byte>& output) {
  if (std::is_signed<T>()) {
    return pw_varint_ZigZagEncode(integer, output.data(), output.size());
  } else {
    return pw_varint_Encode(integer, output.data(), output.size());
  }
}

// Decodes a varint-encoded value. If reading into a signed integer, the value
// is ZigZag decoded.
//
// Returns the number of bytes read from the input if successful. Returns zero
// if the result does not fit in a int64_t / uint64_t or if the input is
// exhausted before the number terminates. Reads a maximum of 10 bytes.
//
// The following example decodes multiple varints from a buffer:
//
//   while (!data.empty()) {
//     int64_t value;
//     size_t bytes = Decode(data, &value);
//
//     if (bytes == 0u) {
//       return Status::DataLoss();
//     }
//     results.push_back(value);
//     data = data.subspan(bytes)
//   }
//
inline size_t Decode(const span<const std::byte>& input, int64_t* value) {
  return pw_varint_ZigZagDecode(input.data(), input.size(), value);
}

inline size_t Decode(const span<const std::byte>& input, uint64_t* value) {
  return pw_varint_Decode(input.data(), input.size(), value);
}

enum class Format {
  kZeroTerminatedLeastSignificant = PW_VARINT_ZERO_TERMINATED_LEAST_SIGNIFICANT,
  kZeroTerminatedMostSignificant = PW_VARINT_ZERO_TERMINATED_MOST_SIGNIFICANT,
  kOneTerminatedLeastSignificant = PW_VARINT_ONE_TERMINATED_LEAST_SIGNIFICANT,
  kOneTerminatedMostSignificant = PW_VARINT_ONE_TERMINATED_MOST_SIGNIFICANT,
};

// Encodes a varint in a custom format.
inline size_t Encode(uint64_t value, span<std::byte> output, Format format) {
  return pw_varint_EncodeCustom(value,
                                output.data(),
                                output.size(),
                                static_cast<pw_varint_Format>(format));
}

// Decodes a varint from a custom format.
inline size_t Decode(span<const std::byte> input,
                     uint64_t* value,
                     Format format) {
  return pw_varint_DecodeCustom(
      input.data(), input.size(), value, static_cast<pw_varint_Format>(format));
}

// Returns a size of an integer when encoded as a varint.
constexpr size_t EncodedSize(uint64_t integer) {
  return integer == 0 ? 1 : (64 - __builtin_clzll(integer) + 6) / 7;
}

// Returns a size of an signed integer when ZigZag encoded as a varint.
constexpr size_t ZigZagEncodedSize(int64_t integer) {
  return EncodedSize(ZigZagEncode(integer));
}

// Returns the maximum integer value that can be encoded in a varint of the
// specified number of bytes.
//
// These values are also listed in the table below. Zigzag encoding cuts these
// in half, as positive and negative integers are alternated.
//
//   Bytes          Max value
//     1                          127
//     2                       16,383
//     3                    2,097,151
//     4                  268,435,455
//     5               34,359,738,367 -- needed for max uint32 value
//     6            4,398,046,511,103
//     7          562,949,953,421,311
//     8       72,057,594,037,927,935
//     9    9,223,372,036,854,775,807
//     10            uint64 max value
//
constexpr uint64_t MaxValueInBytes(size_t bytes) {
  return bytes >= kMaxVarint64SizeBytes ? std::numeric_limits<uint64_t>::max()
                                        : (uint64_t(1) << (7 * bytes)) - 1;
}

}  // namespace varint
}  // namespace pw

#endif  // __cplusplus
