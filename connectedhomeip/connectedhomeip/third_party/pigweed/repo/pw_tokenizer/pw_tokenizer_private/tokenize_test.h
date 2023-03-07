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

// Functions to test that the tokenization macro works correctly in C code.
// These are defined in tokenize_test.c and global_handlers_test.c.
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "pw_preprocessor/util.h"

PW_EXTERN_C_START

#define TEST_FORMAT_STRING_SHORT_FLOAT "Hello %s! %hd %e"

void pw_tokenizer_ToBufferTest_StringShortFloat(void* buffer,
                                                size_t* buffer_size);

#define TEST_FORMAT_SEQUENTIAL_ZIG_ZAG "%u%d%02x%X%hu%hhd%d%ld%lu%lld%llu%c%c%c"

void pw_tokenizer_ToBufferTest_SequentialZigZag(void* buffer,
                                                size_t* buffer_size);

void pw_tokenizer_ToCallbackTest_SequentialZigZag(
    void (*callback)(const uint8_t* buffer, size_t size));

#define TEST_FORMAT_REQUIRES_8 "Won't fit : %s%d"

void pw_tokenizer_ToBufferTest_Requires8(void* buffer, size_t* buffer_size);

void pw_tokenizer_ToGlobalHandlerTest_SequentialZigZag(void);

void pw_tokenizer_ToGlobalHandlerWithPayloadTest_SequentialZigZag(void);

PW_EXTERN_C_END
