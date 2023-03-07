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

// This function tests the C implementation of tokenization API. These functions
// are called from the main C++ test file.

#include "pw_tokenizer/tokenize_to_global_handler.h"
#include "pw_tokenizer/tokenize_to_global_handler_with_payload.h"
#include "pw_tokenizer_private/tokenize_test.h"

#ifdef __cplusplus
#error "This is a test of C code and must be compiled as C, not C++."
#endif  // __cplusplus

// This test invokes the tokenization API with a variety of types. To simplify
// validating the encoded data, numbers that are sequential when zig-zag encoded
// are used as arguments.
void pw_tokenizer_ToGlobalHandlerTest_SequentialZigZag(void) {
  PW_TOKENIZE_TO_GLOBAL_HANDLER(TEST_FORMAT_SEQUENTIAL_ZIG_ZAG,
                                0u,
                                -1,
                                1u,
                                (unsigned)-2,
                                (unsigned short)2u,
                                (signed char)-3,
                                3,
                                -4l,
                                4ul,
                                -5ll,
                                5ull,
                                (signed char)-6,
                                (char)6,
                                (signed char)-7);
}

void pw_tokenizer_ToGlobalHandlerWithPayloadTest_SequentialZigZag(void) {
  PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD((pw_tokenizer_Payload)600613,
                                             TEST_FORMAT_SEQUENTIAL_ZIG_ZAG,
                                             0u,
                                             -1,
                                             1u,
                                             (unsigned)-2,
                                             (unsigned short)2u,
                                             (signed char)-3,
                                             3,
                                             -4l,
                                             4ul,
                                             -5ll,
                                             5ull,
                                             (signed char)-6,
                                             (char)6,
                                             (signed char)-7);
}
