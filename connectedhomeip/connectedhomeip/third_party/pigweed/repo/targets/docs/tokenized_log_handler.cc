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

#include "pw_bytes/span.h"
#include "pw_sys_io/sys_io.h"
#include "pw_tokenizer/tokenize_to_global_handler_with_payload.h"

extern "C" void pw_tokenizer_HandleEncodedMessageWithPayload(
    pw_tokenizer_Payload, const uint8_t encoded_message[], size_t size_bytes) {
  pw::sys_io::WriteBytes(pw::ConstByteSpan(
      reinterpret_cast<const std::byte*>(encoded_message), size_bytes));
}
