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

#include <cstdarg>
#include <cstddef>
#include <cstring>

#include "pw_span/span.h"
#include "pw_tokenizer/config.h"
#include "pw_tokenizer/internal/argument_types.h"
#include "pw_tokenizer/tokenize.h"

namespace pw {
namespace tokenizer {

// Encodes a tokenized string's arguments to a buffer. The
// pw_tokenizer_ArgTypes parameter specifies the argument types, in place of a
// format string.
//
// Most tokenization implementations may use the EncodedMessage class below.
size_t EncodeArgs(pw_tokenizer_ArgTypes types,
                  va_list args,
                  span<std::byte> output);

// Encodes a tokenized message to a fixed size buffer. The size of the buffer is
// determined by the PW_TOKENIZER_CFG_ENCODING_BUFFER_SIZE_BYTES config macro.
//
// This class is used to encode tokenized messages passed in from the
// tokenization macros. The macros provided by pw_tokenizer use this class, and
// projects that elect to define their own versions of the tokenization macros
// should use it when possible.
//
// To use the pw::Tokenizer::EncodedMessage, construct it with the token,
// argument types, and va_list from the variadic arguments:
//
//   void SendLogMessage(span<std::byte> log_data);
//
//   extern "C" void TokenizeToSendLogMessage(pw_tokenizer_Token token,
//                                            pw_tokenizer_ArgTypes types,
//                                            ...) {
//     va_list args;
//     va_start(args, types);
//     EncodedMessage encoded_message(token, types, args);
//     va_end(args);
//
//     SendLogMessage(encoded_message);  // EncodedMessage converts to span
//   }
//
class EncodedMessage {
 public:
  // Encodes a tokenized message to an internal buffer.
  EncodedMessage(pw_tokenizer_Token token,
                 pw_tokenizer_ArgTypes types,
                 va_list args) {
    std::memcpy(data_, &token, sizeof(token));
    args_size_ =
        EncodeArgs(types, args, span<std::byte>(data_).subspan(sizeof(token)));
  }

  // The binary-encoded tokenized message.
  const std::byte* data() const { return data_; }

  // Returns the data() as a pointer to uint8_t instead of std::byte.
  const uint8_t* data_as_uint8() const {
    return reinterpret_cast<const uint8_t*>(data());
  }

  // The size of the encoded tokenized message in bytes.
  size_t size() const { return sizeof(pw_tokenizer_Token) + args_size_; }

 private:
  std::byte data_[PW_TOKENIZER_CFG_ENCODING_BUFFER_SIZE_BYTES];
  size_t args_size_;
};

static_assert(PW_TOKENIZER_CFG_ENCODING_BUFFER_SIZE_BYTES >=
                  sizeof(pw_tokenizer_Token),
              "PW_TOKENIZER_CFG_ENCODING_BUFFER_SIZE_BYTES must be at least "
              "large enough for a token (4 bytes)");

}  // namespace tokenizer
}  // namespace pw
