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

// These tests call the pw_base64 module API from C. The return values are
// checked in the main C++ tests.
//
// The encoded / decoded size macros are tested in the main C++ tests.

#include <stddef.h>

#include "pw_base64/base64.h"

void pw_Base64CallEncode(const void* binary_data,
                         const size_t binary_size_bytes,
                         char* output) {
  pw_Base64Encode(binary_data, binary_size_bytes, output);
}

size_t pw_Base64CallDecode(const char* base64,
                           size_t base64_size_bytes,
                           void* output) {
  return pw_Base64Decode(base64, base64_size_bytes, output);
}

bool pw_Base64CallIsValid(const char* base64_data, size_t base64_size) {
  return pw_Base64IsValid(base64_data, base64_size);
}
