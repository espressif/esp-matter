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

// Functions for encoding and decoding data in Base64 as specified by RFC 3548
// and RFC 4648. See https://tools.ietf.org/html/rfc4648
#pragma once

#include <stdbool.h>
#include <stddef.h>

// C-compatible versions of a subset of the pw_base64 module.
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// Returns the size of the given number of bytes when encoded as Base64. Base64
//
// Equivalent to pw::base64::EncodedSize().
#define PW_BASE64_ENCODED_SIZE(binary_size_bytes) \
  (((size_t)binary_size_bytes + 2) / 3 * 4)  // +2 to round up to a 3-byte group

// Encodes the provided data in Base64 and writes the result to the buffer.
// Exactly PW_BASE64_ENCODED_SIZE(binary_size_bytes) bytes will be written. The
// output buffer *MUST* be large enough for the encoded output!
//
// Equivalent to pw::base64::Encode().
void pw_Base64Encode(const void* binary_data,
                     const size_t binary_size_bytes,
                     char* output);

// Evaluates to the maximum size of decoded Base64 data in bytes.
//
// Equivalent to pw::base64::MaxDecodedSize().
#define PW_BASE64_MAX_DECODED_SIZE(base64_size_bytes) \
  (((size_t)base64_size_bytes) / 4 * 3)

// Decodes the provided Base64 data into raw binary. The output buffer *MUST* be
// at least PW_BASE64_MAX_DECODED_SIZE bytes large.
//
// Equivalent to pw::base64::Decode().
size_t pw_Base64Decode(const char* base64,
                       size_t base64_size_bytes,
                       void* output);

// Returns true if the provided string is valid Base64 encoded data. Accepts
// either the standard (+/) or URL-safe (-_) alphabets.
//
// Equivalent to pw::base64::IsValid().
bool pw_Base64IsValid(const char* base64_data, size_t base64_size);

// C++ API, which uses the C functions internally.
#ifdef __cplusplus
}  // extern "C"

#include <string_view>
#include <type_traits>

#include "pw_span/span.h"
#include "pw_string/string.h"

namespace pw::base64 {

// Returns the size of the given number of bytes when encoded as Base64. Base64
// encodes 3-byte groups into 4-character strings. The final group is padded to
// be 3-bytes if it only has 1 or 2.
constexpr size_t EncodedSize(size_t binary_size_bytes) {
  return PW_BASE64_ENCODED_SIZE(binary_size_bytes);
}

// Encodes the provided data in Base64 and writes the result to the buffer.
// Encodes to the standard alphabet with + and / for characters 62 and 63.
// Exactly EncodedSize(binary_size_bytes) bytes will be written. The
// output buffer *MUST* be large enough for the encoded output! The input and
// output buffers MUST NOT be the same; encoding cannot occur in place.
//
// The resulting string in the output is NOT null-terminated!
inline void Encode(span<const std::byte> binary, char* output) {
  pw_Base64Encode(binary.data(), binary.size_bytes(), output);
}

// Encodes the provided data in Base64 if the result fits in the provided
// buffer. Returns the number of bytes written, which will be 0 if the output
// buffer is too small.
size_t Encode(span<const std::byte> binary, span<char> output_buffer);

// Appends Base64 encoded binary data to the provided pw::InlineString. If the
// data does not fit in the string, an assertion fails.
void Encode(span<const std::byte> binary, InlineString<>& output);

// Creates a pw::InlineString<> large enough to hold kMaxBinaryDataSizeBytes of
// binary data when encoded as Base64 and encodes the provided span into it.
// If the data is larger than kMaxBinaryDataSizeBytes, an assertion fails.
template <size_t kMaxBinaryDataSizeBytes>
inline InlineString<EncodedSize(kMaxBinaryDataSizeBytes)> Encode(
    span<const std::byte> binary) {
  InlineString<EncodedSize(kMaxBinaryDataSizeBytes)> output;
  Encode(binary, output);
  return output;
}

// Returns the maximum size of decoded Base64 data in bytes. base64_size_bytes
// must be a multiple of 4, since Base64 encodes 3-byte groups into 4-character
// strings. If the last 3-byte group has padding, the actual decoded size would
// be 1 or 2 bytes less than MaxDecodedSize.
constexpr size_t MaxDecodedSize(size_t base64_size_bytes) {
  return PW_BASE64_MAX_DECODED_SIZE(base64_size_bytes);
}

// Decodes the provided Base64 data into raw binary. The output buffer *MUST* be
// at least MaxDecodedSize bytes large. The output buffer may be the same as the
// input buffer; decoding can occur in place. Returns the number of bytes that
// were decoded.
//
// Decodes either standard (+/) or URL-safe (-_) alphabets. The data must be
// padded to 4-character blocks with =. This function does NOT check that the
// input is valid! Use IsValid or the four-argument overload to check the
// input formatting.
inline size_t Decode(std::string_view base64, void* output) {
  return pw_Base64Decode(base64.data(), base64.size(), output);
}

// Decodes the provided Base64 data, if the data is valid and fits in the output
// buffer. Returns the number of bytes written, which will be 0 if the data is
// invalid or doesn't fit.
size_t Decode(std::string_view base64, span<std::byte> output_buffer);

template <typename T>
inline void DecodeInPlace(InlineBasicString<T>& buffer) {
  static_assert(sizeof(T) == sizeof(char));
  buffer.resize(Decode(buffer, buffer.data()));
}

// Returns true if the provided string is valid Base64 encoded data. Accepts
// either the standard (+/) or URL-safe (-_) alphabets.
inline bool IsValid(std::string_view base64) {
  return pw_Base64IsValid(base64.data(), base64.size());
}

}  // namespace pw::base64

#endif  // __cplusplus
