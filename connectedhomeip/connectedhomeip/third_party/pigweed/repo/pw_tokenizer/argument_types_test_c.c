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

// This C source file tests that the tokenizer argument type encoding works
// correctly in C. These functions are called from the main C++ test file
// argument_types_test.cc.
#include <assert.h>
#include <stddef.h>

#include "pw_tokenizer_private/argument_types_test.h"

#ifdef __cplusplus
#error "This is a test of C code and must be compiled as C, not C++."
#endif  // __cplusplus

// Stand-in type for pointer argument type test
struct FakeType {
  char unused;
};

// Check each relevant type mapping using static_asserts.
#define CHECK_TYPE(c_type, enum_type)                     \
  static_assert(_PW_VARARGS_TYPE((c_type)1) == enum_type, \
                #c_type " should map to " #enum_type)

// integral
// clang-format off
CHECK_TYPE(_Bool,              PW_TOKENIZER_ARG_TYPE_INT);
CHECK_TYPE(char,               PW_TOKENIZER_ARG_TYPE_INT);
CHECK_TYPE(signed char,        PW_TOKENIZER_ARG_TYPE_INT);
CHECK_TYPE(unsigned char,      PW_TOKENIZER_ARG_TYPE_INT);
CHECK_TYPE(short,              PW_TOKENIZER_ARG_TYPE_INT);
CHECK_TYPE(unsigned short,     PW_TOKENIZER_ARG_TYPE_INT);
CHECK_TYPE(int,                PW_TOKENIZER_ARG_TYPE_INT);
CHECK_TYPE(unsigned int,       PW_TOKENIZER_ARG_TYPE_INT);
CHECK_TYPE(long,              _PW_TOKENIZER_SELECT_INT_TYPE(long));
CHECK_TYPE(unsigned long,     _PW_TOKENIZER_SELECT_INT_TYPE(unsigned long));
CHECK_TYPE(long long,          PW_TOKENIZER_ARG_TYPE_INT64);
CHECK_TYPE(unsigned long long, PW_TOKENIZER_ARG_TYPE_INT64);

// floating point
CHECK_TYPE(float,              PW_TOKENIZER_ARG_TYPE_DOUBLE);
CHECK_TYPE(double,             PW_TOKENIZER_ARG_TYPE_DOUBLE);
CHECK_TYPE(long double,        PW_TOKENIZER_ARG_TYPE_DOUBLE);

// strings
CHECK_TYPE(char*,              PW_TOKENIZER_ARG_TYPE_STRING);
CHECK_TYPE(const char*,        PW_TOKENIZER_ARG_TYPE_STRING);

// pointers (which should map to the appropriate sized integer)
CHECK_TYPE(void*,             _PW_TOKENIZER_SELECT_INT_TYPE(void*));
CHECK_TYPE(const void*,       _PW_TOKENIZER_SELECT_INT_TYPE(void*));
CHECK_TYPE(signed char*,      _PW_TOKENIZER_SELECT_INT_TYPE(void*));
CHECK_TYPE(unsigned char*,    _PW_TOKENIZER_SELECT_INT_TYPE(void*));
CHECK_TYPE(int*,              _PW_TOKENIZER_SELECT_INT_TYPE(void*));
CHECK_TYPE(long long*,        _PW_TOKENIZER_SELECT_INT_TYPE(void*));
CHECK_TYPE(struct FakeType*,  _PW_TOKENIZER_SELECT_INT_TYPE(void*));
// clang-format on

// null
static_assert(_PW_VARARGS_TYPE(NULL) == _PW_TOKENIZER_SELECT_INT_TYPE(void*),
              "");

static char char_array[16];

// Define the test functions that are called by the C++ unit test.
#define DEFINE_TEST_FUNCTION(name, ...)                \
  pw_tokenizer_ArgTypes pw_TestTokenizer##name(void) { \
    (void)char_array;                                  \
    return PW_TOKENIZER_ARG_TYPES(__VA_ARGS__);        \
  }                                                    \
  static_assert(1, "Macros must be terminated with a semicolon")

DEFINE_TEST_FUNCTION(NoArgs);

DEFINE_TEST_FUNCTION(Char, 'a');
DEFINE_TEST_FUNCTION(Uint8, ((uint8_t)23));
DEFINE_TEST_FUNCTION(Uint16, ((int16_t)100));
DEFINE_TEST_FUNCTION(Int32, ((int32_t)1));
DEFINE_TEST_FUNCTION(Int64, ((int64_t)0));
DEFINE_TEST_FUNCTION(Uint64, ((uint64_t)1));
DEFINE_TEST_FUNCTION(Float, 1e10f);
DEFINE_TEST_FUNCTION(Double, -2.5e-50);
DEFINE_TEST_FUNCTION(String, "const char*");
DEFINE_TEST_FUNCTION(MutableString, ((char*)NULL));

DEFINE_TEST_FUNCTION(IntFloat, 54321, ((float)0));
DEFINE_TEST_FUNCTION(Uint64Char, ((uint64_t)0ull), ((unsigned char)'x'));
DEFINE_TEST_FUNCTION(StringString, char_array, ((const char*)NULL));
DEFINE_TEST_FUNCTION(Uint16Int, ((uint16_t)100), ((int)0));
DEFINE_TEST_FUNCTION(FloatString, 100.0f, "string");

DEFINE_TEST_FUNCTION(Null, NULL);
DEFINE_TEST_FUNCTION(Pointer, ((void*)NULL));
DEFINE_TEST_FUNCTION(PointerPointer, (int*)char_array, (void*)0);
