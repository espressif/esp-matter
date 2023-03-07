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

#include <cstdint>
#include <cstring>
#include <memory>

#include "pw_assert/config.h"
#include "pw_assert_tokenized/handler.h"
#include "pw_base64/base64.h"
#include "pw_bytes/endian.h"
#include "pw_log/log.h"
#include "pw_log_tokenized/config.h"
#include "pw_log_tokenized/log_tokenized.h"
#include "pw_log_tokenized/metadata.h"
#include "pw_span/span.h"

extern "C" void pw_assert_tokenized_HandleAssertFailure(
    uint32_t tokenized_file_name, int line_number) {
  // Buffer size for binary->base64 conversion with a null terminator.
  constexpr size_t kBufferSize =
      pw::base64::EncodedSize(sizeof(tokenized_file_name)) + 1;
  std::byte* hash_buffer = reinterpret_cast<std::byte*>(&tokenized_file_name);
  char base64_buffer[kBufferSize];

  size_t len =
      pw::base64::Encode(pw::span(hash_buffer, sizeof(tokenized_file_name)),
                         pw::span(base64_buffer));
  base64_buffer[len] = '\0';
#if PW_ASSERT_ENABLE_DEBUG
  PW_LOG(PW_LOG_LEVEL_FATAL,
         PW_LOG_FLAGS,
         "PW_ASSERT() or PW_DASSERT() failure at $%s:%d",
         base64_buffer,
         line_number);
#else
  PW_LOG(PW_LOG_LEVEL_FATAL,
         PW_LOG_FLAGS,
         "PW_ASSERT() failure. Note: PW_DASSERT disabled $%s:%d",
         base64_buffer,
         line_number);
#endif  // PW_ASSERT_ENABLE_DEBUG
  PW_UNREACHABLE;
}

extern "C" void pw_assert_tokenized_HandleCheckFailure(
    uint32_t tokenized_message, int line_number) {
  // If line_number is too large to fit in the packed payload, the Metadata
  // class will properly set it to 0, which is the expected value for line
  // number values that would cause the bit field to overflow.
  // See https://pigweed.dev/pw_log_tokenized/#c.PW_LOG_TOKENIZED_LINE_BITS for
  // more info.
  const pw_tokenizer_Payload payload =
      pw::log_tokenized::Metadata(
          PW_LOG_LEVEL_FATAL, 0, PW_LOG_FLAGS, line_number)
          .value();
  std::array<std::byte, sizeof(tokenized_message)> token_buffer =
      pw::bytes::CopyInOrder(pw::endian::little, tokenized_message);

  pw_tokenizer_HandleEncodedMessageWithPayload(
      payload,
      reinterpret_cast<const uint8_t*>(token_buffer.data()),
      token_buffer.size());
  PW_UNREACHABLE;
}
