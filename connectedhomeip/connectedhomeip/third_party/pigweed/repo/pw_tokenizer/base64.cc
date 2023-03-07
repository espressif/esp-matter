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

#include "pw_tokenizer/base64.h"

namespace pw::tokenizer {

extern "C" size_t pw_tokenizer_PrefixedBase64Encode(
    const void* binary_message,
    size_t binary_size_bytes,
    void* output_buffer,
    size_t output_buffer_size_bytes) {
  char* output = static_cast<char*>(output_buffer);
  const size_t encoded_size = Base64EncodedBufferSize(binary_size_bytes);

  if (output_buffer_size_bytes < encoded_size) {
    if (output_buffer_size_bytes > 0u) {
      output[0] = '\0';
    }

    return 0;
  }

  output[0] = kBase64Prefix;
  base64::Encode(
      span(static_cast<const std::byte*>(binary_message), binary_size_bytes),
      &output[1]);
  output[encoded_size - 1] = '\0';
  return encoded_size - sizeof('\0');  // exclude the null terminator
}

void PrefixedBase64Encode(span<const std::byte> binary_message,
                          InlineString<>& output) {
  output.push_back(kBase64Prefix);
  base64::Encode(binary_message, output);
}

extern "C" size_t pw_tokenizer_PrefixedBase64Decode(const void* base64_message,
                                                    size_t base64_size_bytes,
                                                    void* output_buffer,
                                                    size_t output_buffer_size) {
  const char* base64 = static_cast<const char*>(base64_message);

  if (base64_size_bytes == 0 || base64[0] != kBase64Prefix) {
    return 0;
  }

  return base64::Decode(
      std::string_view(&base64[1], base64_size_bytes - 1),
      span(static_cast<std::byte*>(output_buffer), output_buffer_size));
}

}  // namespace pw::tokenizer
