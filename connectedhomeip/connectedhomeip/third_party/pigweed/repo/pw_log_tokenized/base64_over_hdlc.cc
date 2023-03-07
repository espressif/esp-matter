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

// This function serves as a backend for pw_tokenizer / pw_log_tokenized that
// encodes tokenized logs as Base64 and writes them using HDLC.

#include "pw_log_tokenized/base64_over_hdlc.h"

#include "pw_hdlc/encoder.h"
#include "pw_span/span.h"
#include "pw_stream/sys_io_stream.h"
#include "pw_tokenizer/base64.h"
#include "pw_tokenizer/tokenize_to_global_handler_with_payload.h"

namespace pw::log_tokenized {
namespace {

stream::SysIoWriter writer;

}  // namespace

// Base64-encodes tokenized logs and writes them to pw::sys_io as HDLC frames.
extern "C" void pw_tokenizer_HandleEncodedMessageWithPayload(
    pw_tokenizer_Payload,  // TODO(hepler): Use the metadata for filtering.
    const uint8_t log_buffer[],
    size_t size_bytes) {
  // Encode the tokenized message as Base64.
  char base64_buffer[tokenizer::kDefaultBase64EncodedBufferSize];
  const size_t base64_bytes = tokenizer::PrefixedBase64Encode(
      span(log_buffer, size_bytes), base64_buffer);
  base64_buffer[base64_bytes] = '\0';

  // HDLC-encode the Base64 string via a SysIoWriter.
  hdlc::WriteUIFrame(PW_LOG_TOKENIZED_BASE64_LOG_HDLC_ADDRESS,
                     as_bytes(span(base64_buffer, base64_bytes)),
                     writer);
}

}  // namespace pw::log_tokenized
