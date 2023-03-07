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

// This file provides functions for working with the prefixed Base64 format for
// tokenized messages. This format is useful for transmitting tokenized messages
// as plain text.
//
// The format uses a prefix character ($), followed by the Base64 version of the
// tokenized message. For example, consider a tokenized message with token
// 0xfeb35a42 and encoded argument 0x13. This messsage would be encoded as
// follows:
//
//            Binary: 42 5a b3 fe 13  [5 bytes]
//
//   Prefixed Base64: $Qlqz/hM=       [9 bytes]
//
#pragma once

#include <stddef.h>

#include "pw_preprocessor/util.h"
#include "pw_tokenizer/config.h"

// This character is used to mark the start of a Base64-encoded tokenized
// message. For consistency, it is recommended to always use $ if possible.
// If required, a different non-Base64 character may be used as a prefix.
#define PW_TOKENIZER_BASE64_PREFIX '$'

PW_EXTERN_C_START

// Encodes a binary tokenized message as prefixed Base64 with a null terminator.
// Returns the encoded string length (excluding the null terminator). Returns 0
// if the buffer is too small. Always null terminates if the output buffer is
// not empty.
//
// Equivalent to pw::tokenizer::PrefixedBase64Encode.
size_t pw_tokenizer_PrefixedBase64Encode(const void* binary_message,
                                         size_t binary_size_bytes,
                                         void* output_buffer,
                                         size_t output_buffer_size_bytes);

// Decodes a prefixed Base64 tokenized message to binary. Returns the size of
// the decoded binary data. The resulting data is ready to be passed to
// pw::tokenizer::Detokenizer::Detokenize. Returns 0 if the buffer is too small,
// the expected prefix character is missing, or the Base64 data is corrupt.
//
// Equivalent to pw::tokenizer::PrefixedBase64Encode.
size_t pw_tokenizer_PrefixedBase64Decode(const void* base64_message,
                                         size_t base64_size_bytes,
                                         void* output_buffer,
                                         size_t output_buffer_size);

PW_EXTERN_C_END

#ifdef __cplusplus

#include <string_view>

#include "pw_base64/base64.h"
#include "pw_span/span.h"
#include "pw_tokenizer/config.h"
#include "pw_tokenizer/tokenize.h"

namespace pw::tokenizer {

inline constexpr char kBase64Prefix = PW_TOKENIZER_BASE64_PREFIX;

#undef PW_TOKENIZER_BASE64_PREFIX  // In C++, use the variable, not the macro.

// Returns the size of a tokenized message (token + arguments) when encoded as
// prefixed Base64. Includes room for the prefix character ($) and encoded
// message. This value is the capacity needed to encode to a pw::InlineString.
constexpr size_t Base64EncodedStringSize(size_t message_size) {
  return sizeof(kBase64Prefix) + base64::EncodedSize(message_size);
}

// Same as Base64EncodedStringSize(), but for sizing char buffers. Includes room
// for the prefix character ($), encoded message, and a null terminator.
constexpr size_t Base64EncodedBufferSize(size_t message_size) {
  return Base64EncodedStringSize(message_size) + sizeof('\0');
}

// The minimum buffer size that can hold a tokenized message that is
// PW_TOKENIZER_CFG_ENCODING_BUFFER_SIZE_BYTES long encoded as prefixed Base64.
inline constexpr size_t kDefaultBase64EncodedBufferSize =
    Base64EncodedBufferSize(PW_TOKENIZER_CFG_ENCODING_BUFFER_SIZE_BYTES);

// Encodes a binary tokenized message as prefixed Base64 with a null terminator.
// Returns the encoded string length (excluding the null terminator). Returns 0
// if the buffer is too small. Always null terminates if the output buffer is
// not empty.
inline size_t PrefixedBase64Encode(span<const std::byte> binary_message,
                                   span<char> output_buffer) {
  return pw_tokenizer_PrefixedBase64Encode(binary_message.data(),
                                           binary_message.size(),
                                           output_buffer.data(),
                                           output_buffer.size());
}

// Also accept a span<const uint8_t> for the binary message.
inline size_t PrefixedBase64Encode(span<const uint8_t> binary_message,
                                   span<char> output_buffer) {
  return PrefixedBase64Encode(as_bytes(binary_message), output_buffer);
}

// Encodes a binary tokenized message as prefixed Base64 to a pw::InlineString,
// appending to any existing contents. Asserts if the message does not fit in
// the string.
void PrefixedBase64Encode(span<const std::byte> binary_message,
                          InlineString<>& output);

inline void PrefixedBase64Encode(span<const uint8_t> binary_message,
                                 InlineString<>& output) {
  return PrefixedBase64Encode(as_bytes(binary_message), output);
}

// Encodes a binary tokenized message as prefixed Base64 to a pw::InlineString.
// The pw::InlineString is sized to fit messages up to
// kMaxBinaryMessageSizeBytes long. Asserts if the message is larger.
template <size_t kMaxBinaryMessageSizeBytes =
              PW_TOKENIZER_CFG_ENCODING_BUFFER_SIZE_BYTES>
auto PrefixedBase64Encode(span<const std::byte> binary_message) {
  static_assert(kMaxBinaryMessageSizeBytes >= 1, "Messages cannot be empty");
  InlineString<Base64EncodedStringSize(kMaxBinaryMessageSizeBytes)> string(
      1, kBase64Prefix);
  base64::Encode(binary_message, string);
  return string;
}

template <size_t kMaxBinaryMessageSizeBytes =
              PW_TOKENIZER_CFG_ENCODING_BUFFER_SIZE_BYTES>
auto PrefixedBase64Encode(span<const uint8_t> binary_message) {
  return PrefixedBase64Encode<kMaxBinaryMessageSizeBytes>(
      as_bytes(binary_message));
}

// Decodes a prefixed Base64 tokenized message to binary. Returns the size of
// the decoded binary data. The resulting data is ready to be passed to
// pw::tokenizer::Detokenizer::Detokenize.
inline size_t PrefixedBase64Decode(std::string_view base64_message,
                                   span<std::byte> output_buffer) {
  return pw_tokenizer_PrefixedBase64Decode(base64_message.data(),
                                           base64_message.size(),
                                           output_buffer.data(),
                                           output_buffer.size());
}

// Decodes a prefixed Base64 tokenized message to binary in place. Returns the
// size of the decoded binary data.
inline size_t PrefixedBase64DecodeInPlace(span<std::byte> buffer) {
  return pw_tokenizer_PrefixedBase64Decode(
      buffer.data(), buffer.size(), buffer.data(), buffer.size());
}

// Decodes a prefixed Base64 tokenized message to binary in place. Resizes the
// string to fit the decoded binary data.
template <typename CharT>
inline void PrefixedBase64DecodeInPlace(InlineBasicString<CharT>& string) {
  static_assert(sizeof(CharT) == sizeof(char));
  string.resize(pw_tokenizer_PrefixedBase64Decode(
      string.data(), string.size(), string.data(), string.size()));
}

}  // namespace pw::tokenizer

#endif  // __cplusplus
