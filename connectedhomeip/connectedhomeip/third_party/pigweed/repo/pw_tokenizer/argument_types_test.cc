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

#include "pw_tokenizer/internal/argument_types.h"

#include <cstddef>

#include "gtest/gtest.h"
#include "pw_preprocessor/concat.h"
#include "pw_tokenizer_private/argument_types_test.h"

namespace pw::tokenizer {
namespace {

struct FakeType {};

// Check each relevant type mapping.
#define CHECK_TYPE(c_type, enum_type)                     \
  static_assert(_PW_VARARGS_TYPE((c_type)0) == enum_type, \
                #c_type " should map to " #enum_type)

// integral
// clang-format off
CHECK_TYPE(bool,               PW_TOKENIZER_ARG_TYPE_INT);
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
CHECK_TYPE(FakeType*,         _PW_TOKENIZER_SELECT_INT_TYPE(void*));

// nullptr
CHECK_TYPE(std::nullptr_t,    _PW_TOKENIZER_SELECT_INT_TYPE(void*));
static_assert(_PW_VARARGS_TYPE(nullptr) ==
              _PW_TOKENIZER_SELECT_INT_TYPE(void*));

// clang-format on

// Define a macro that generates expected values for tests. This works with
// either 4-bit or 6-bit argument counts (for encoding types in a uint32_t or
// uint64_t).
#define PACKED_TYPES(...)                                                 \
  ((PW_CONCAT(0b, __VA_ARGS__, u) << PW_TOKENIZER_TYPE_COUNT_SIZE_BITS) | \
   PW_MACRO_ARG_COUNT(__VA_ARGS__))

// Test this test macro for both uint32_t and uint64_t.
#if PW_TOKENIZER_CFG_ARG_TYPES_SIZE_BYTES == 4

static_assert(PACKED_TYPES(00) == 0b00'0001u);
static_assert(PACKED_TYPES(11) == 0b11'0001u);
static_assert(PACKED_TYPES(01, 10) == 0b0110'0010u);
static_assert(PACKED_TYPES(11, 01, 10) == 0b110110'0011u);
static_assert(PACKED_TYPES(11, 10, 01, 00) == 0b11'10'01'00'0100u);

#elif PW_TOKENIZER_CFG_ARG_TYPES_SIZE_BYTES == 8

static_assert(PACKED_TYPES(00) == 0b00'000001u);
static_assert(PACKED_TYPES(11) == 0b11'000001u);
static_assert(PACKED_TYPES(01, 10) == 0b0110'000010u);
static_assert(PACKED_TYPES(11, 01, 10) == 0b110110'000011u);
static_assert(PACKED_TYPES(11, 10, 01, 00) == 0b11'10'01'00'000100u);

#else

#error "Unsupported value for PW_TOKENIZER_CFG_ARG_TYPES_SIZE_BYTES"

#endif  // PW_TOKENIZER_CFG_ARG_TYPES_SIZE_BYTES

#define SOME_OTHER_MACRO(...) PW_TOKENIZER_ARG_TYPES(__VA_ARGS__)

TEST(ArgumentTypes, Empty) {
  static_assert(PW_TOKENIZER_ARG_TYPES() == 0u);
  static_assert(PW_TOKENIZER_ARG_TYPES(/* nothing here */) == 0u);
  static_assert(SOME_OTHER_MACRO() == 0u);
  static_assert(SOME_OTHER_MACRO(/* nothing here */) == 0u);
}

TEST(ArgumentTypes, Int32) {
  static_assert(PW_TOKENIZER_ARG_TYPES(0) == PACKED_TYPES(00));
  static_assert(PW_TOKENIZER_ARG_TYPES('a') == PACKED_TYPES(00));
  static_assert(PW_TOKENIZER_ARG_TYPES(u'X') == PACKED_TYPES(00));
  static_assert(PW_TOKENIZER_ARG_TYPES(static_cast<uint16_t>(1)) ==
                PACKED_TYPES(00));
}

TEST(ArgumentTypes, Int64) {
  static_assert(PW_TOKENIZER_ARG_TYPES(-123ll) == PACKED_TYPES(01));
  static_assert(PW_TOKENIZER_ARG_TYPES(123ull) == PACKED_TYPES(01));
  static_assert(PW_TOKENIZER_ARG_TYPES(static_cast<uint64_t>(1)) ==
                PACKED_TYPES(01));
}

TEST(ArgumentTypes, Double) {
  static_assert(PW_TOKENIZER_ARG_TYPES(1.0) == PACKED_TYPES(10));
  static_assert(PW_TOKENIZER_ARG_TYPES(5.0f) == PACKED_TYPES(10));
  float number = 9;
  static_assert(PW_TOKENIZER_ARG_TYPES(number) == PACKED_TYPES(10));
}

TEST(ArgumentTypes, String) {
  static_assert(PW_TOKENIZER_ARG_TYPES("string") == PACKED_TYPES(11));
  const char buffer[2] = {'a', 'b'};
  static_assert(PW_TOKENIZER_ARG_TYPES(buffer) == PACKED_TYPES(11));
  char mutable_buffer[8] = {};
  static_assert(PW_TOKENIZER_ARG_TYPES(mutable_buffer) == PACKED_TYPES(11));
  char character = 'a';
  static_assert(PW_TOKENIZER_ARG_TYPES(&character) == PACKED_TYPES(11));
}

TEST(ArgumentTypes, Pointer) {
  static const bool some_data[8] = {};

  // For 32-bit systems, non-string pointers show up as int32.
  static_assert(sizeof(void*) != sizeof(int32_t) ||
                PACKED_TYPES(00) == PW_TOKENIZER_ARG_TYPES(nullptr));
  static_assert(sizeof(void*) != sizeof(int32_t) ||
                PACKED_TYPES(00) ==
                    PW_TOKENIZER_ARG_TYPES(static_cast<void*>(nullptr)));
  static_assert(sizeof(void*) != sizeof(int32_t) ||
                PACKED_TYPES(00) == PW_TOKENIZER_ARG_TYPES(some_data));

  // For 64-bit systems, non-string pointers show up as int64.
  static_assert(sizeof(void*) != sizeof(int64_t) ||
                PACKED_TYPES(01) == PW_TOKENIZER_ARG_TYPES(nullptr));
  static_assert(sizeof(void*) != sizeof(int64_t) ||
                PACKED_TYPES(01) ==
                    PW_TOKENIZER_ARG_TYPES(static_cast<void*>(nullptr)));
  static_assert(sizeof(void*) != sizeof(int64_t) ||
                PACKED_TYPES(01) == PW_TOKENIZER_ARG_TYPES(some_data));
}

TEST(ArgumentTypes, TwoArgs) {
  static_assert(PW_TOKENIZER_ARG_TYPES(-100, 1000) == PACKED_TYPES(00, 00));
  static_assert(PW_TOKENIZER_ARG_TYPES(-100, 10ll) == PACKED_TYPES(01, 00));
  static_assert(PW_TOKENIZER_ARG_TYPES(-100, 1.0f) == PACKED_TYPES(10, 00));
  static_assert(PW_TOKENIZER_ARG_TYPES(-100, "hi") == PACKED_TYPES(11, 00));

  static_assert(PW_TOKENIZER_ARG_TYPES(1ull, 1000) == PACKED_TYPES(00, 01));
  static_assert(PW_TOKENIZER_ARG_TYPES(1ull, 10ll) == PACKED_TYPES(01, 01));
  static_assert(PW_TOKENIZER_ARG_TYPES(1ull, 1.0f) == PACKED_TYPES(10, 01));
  static_assert(PW_TOKENIZER_ARG_TYPES(1ull, "hi") == PACKED_TYPES(11, 01));

  static_assert(PW_TOKENIZER_ARG_TYPES(9.0f, 1000) == PACKED_TYPES(00, 10));
  static_assert(PW_TOKENIZER_ARG_TYPES(9.0f, 10ll) == PACKED_TYPES(01, 10));
  static_assert(PW_TOKENIZER_ARG_TYPES(9.0f, 1.0f) == PACKED_TYPES(10, 10));
  static_assert(PW_TOKENIZER_ARG_TYPES(9.0f, "hi") == PACKED_TYPES(11, 10));

  static_assert(PW_TOKENIZER_ARG_TYPES("!!", 1000) == PACKED_TYPES(00, 11));
  static_assert(PW_TOKENIZER_ARG_TYPES("!!", 10ll) == PACKED_TYPES(01, 11));
  static_assert(PW_TOKENIZER_ARG_TYPES("!!", 1.0f) == PACKED_TYPES(10, 11));
  static_assert(PW_TOKENIZER_ARG_TYPES("!!", "hi") == PACKED_TYPES(11, 11));
}

TEST(ArgumentTypes, MultipleArgs) {
  // clang-format off
  static_assert(PW_TOKENIZER_ARG_TYPES(1) == 1);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2) == 2);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3) == 3);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4) == 4);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5) == 5);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6) == 6);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7) == 7);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8) == 8);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9) == 9);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9, 10) == 10);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11) == 11);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12) == 12);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13) == 13);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14) == 14);

#if PW_TOKENIZER_CFG_ARG_TYPES_SIZE_BYTES >= 8
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14) == 14);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15) == 15);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16) == 16);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17) == 17);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18) == 18);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19) == 19);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20) == 20);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21) == 21);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22) == 22);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23) == 23);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24) == 24);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25) == 25);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26) == 26);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27) == 27);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28) == 28);
  static_assert(PW_TOKENIZER_ARG_TYPES(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29) == 29);
#endif  // PW_TOKENIZER_CFG_ARG_TYPES_SIZE_BYTES
  // clang-format on
}

// The ArgumentTypesFromC test suite tests the arguments types macro in C. The
// pw_Test* functions are defined in argument_types_c_test.c
TEST(ArgumentTypesFromC, NoArgs) {
  EXPECT_EQ(0b0000u, pw_TestTokenizerNoArgs());
}

TEST(ArgumentTypesFromC, OneArg_Int32) {
  EXPECT_EQ(PACKED_TYPES(00), pw_TestTokenizerChar());
  EXPECT_EQ(PACKED_TYPES(00), pw_TestTokenizerUint8());
  EXPECT_EQ(PACKED_TYPES(00), pw_TestTokenizerUint16());
  EXPECT_EQ(PACKED_TYPES(00), pw_TestTokenizerInt32());
}

TEST(ArgumentTypesFromC, OneArg_Int64) {
  EXPECT_EQ(PACKED_TYPES(01), pw_TestTokenizerInt64());
  EXPECT_EQ(PACKED_TYPES(01), pw_TestTokenizerUint64());
}

TEST(ArgumentTypesFromC, OneArg_Float) {
  EXPECT_EQ(PACKED_TYPES(10), pw_TestTokenizerFloat());
  EXPECT_EQ(PACKED_TYPES(10), pw_TestTokenizerDouble());
}

TEST(ArgumentTypesFromC, OneArg_String) {
  EXPECT_EQ(PACKED_TYPES(11), pw_TestTokenizerString());
  EXPECT_EQ(PACKED_TYPES(11), pw_TestTokenizerMutableString());
}

TEST(ArgumentTypesFromC, MultipleArgs) {
  EXPECT_EQ(PACKED_TYPES(10, 00), pw_TestTokenizerIntFloat());
  EXPECT_EQ(PACKED_TYPES(00, 01), pw_TestTokenizerUint64Char());
  EXPECT_EQ(PACKED_TYPES(11, 11), pw_TestTokenizerStringString());
  EXPECT_EQ(PACKED_TYPES(00, 00), pw_TestTokenizerUint16Int());
  EXPECT_EQ(PACKED_TYPES(11, 10), pw_TestTokenizerFloatString());
}

TEST(ArgumentTypesFromC, Pointers) {
  if constexpr (sizeof(void*) == sizeof(int32_t)) {
    EXPECT_EQ(PACKED_TYPES(00), pw_TestTokenizerNull());
    EXPECT_EQ(PACKED_TYPES(00), pw_TestTokenizerPointer());
    EXPECT_EQ(PACKED_TYPES(00, 00), pw_TestTokenizerPointerPointer());
  } else {  // 64-bit system
    EXPECT_EQ(PACKED_TYPES(01), pw_TestTokenizerNull());
    EXPECT_EQ(PACKED_TYPES(01), pw_TestTokenizerPointer());
    EXPECT_EQ(PACKED_TYPES(01, 01), pw_TestTokenizerPointerPointer());
  }
}

}  // namespace
}  // namespace pw::tokenizer
