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

// These tests call the pw_varint module API from C. The return values are
// checked in the main C++ tests.

#include <stddef.h>

#include "pw_varint/varint.h"

size_t pw_varint_CallEncode(uint64_t integer,
                            void* output,
                            size_t output_size) {
  return pw_varint_Encode(integer, output, output_size);
}

size_t pw_varint_CallZigZagEncode(int64_t integer,
                                  void* output,
                                  size_t output_size) {
  return pw_varint_ZigZagEncode(integer, output, output_size);
}

size_t pw_varint_CallDecode(void* input, size_t input_size, uint64_t* output) {
  return pw_varint_Decode(input, input_size, output);
}

size_t pw_varint_CallZigZagDecode(void* input,
                                  size_t input_size,
                                  int64_t* output) {
  return pw_varint_ZigZagDecode(input, input_size, output);
}
