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

// This tests the C hashing macros and C++ hashing functions.

#include "pw_tokenizer/hash.h"

#include <cstddef>
#include <cstdint>

#include "gtest/gtest.h"
#include "pw_preprocessor/util.h"
#include "pw_tokenizer/internal/pw_tokenizer_65599_fixed_length_128_hash_macro.h"
#include "pw_tokenizer/internal/pw_tokenizer_65599_fixed_length_256_hash_macro.h"
#include "pw_tokenizer/internal/pw_tokenizer_65599_fixed_length_80_hash_macro.h"
#include "pw_tokenizer/internal/pw_tokenizer_65599_fixed_length_96_hash_macro.h"
#include "pw_tokenizer_private/generated_hash_test_cases.h"

namespace pw::tokenizer {
namespace {

// Make sure the array test cases file isn't empty.
static_assert(PW_ARRAY_SIZE(kHashTests) > 10,
              "The test cases array is suspiciously small");

constexpr bool CheckGeneratedCases() {
  for (const auto [string, hash_length, python_hash, macro_hash] : kHashTests) {
    const uint32_t calculated_hash =
        PwTokenizer65599FixedLengthHash(string, hash_length);
    if (calculated_hash != python_hash || calculated_hash != macro_hash) {
      return false;
    }
  }

  return true;
}

static_assert(CheckGeneratedCases(),
              "Hashes in the generated test cases must match");
TEST(Hashing, GeneratedCasesAtRuntime) {
  for (const auto [string, hash_length, python_hash, macro_hash] : kHashTests) {
    const uint32_t calculated_hash =
        PwTokenizer65599FixedLengthHash(string, hash_length);
    EXPECT_EQ(calculated_hash, python_hash);
    EXPECT_EQ(calculated_hash, macro_hash);
  }
}

// Gets the size of the string, excluding the null terminator. A uint32_t is
// used instead of a size_t since the hash calculation requires a uint32_t.
template <uint32_t kSizeWithNull>
constexpr uint32_t StringLength(const char (&)[kSizeWithNull]) {
  static_assert(kSizeWithNull > 0u);
  return kSizeWithNull - 1;  // subtract the null terminator
}

TEST(Hashing, Runtime) PW_NO_SANITIZE("unsigned-integer-overflow") {
  // Coefficients for the hash terms; k1 is 1.
  static constexpr uint32_t k2 = k65599HashConstant;
  static constexpr uint32_t k3 = k65599HashConstant * k2;
  static constexpr uint32_t k4 = k65599HashConstant * k3;
  static constexpr uint32_t k5 = k65599HashConstant * k4;

  // Hash a few things at hash length 4
  // The coefficient calculation is done modulo 0x100000000, so the unsigned
  // integer overflows of the hash terms are intentional.
  EXPECT_EQ(PwTokenizer65599FixedLengthHash("", 4), StringLength(""));
  EXPECT_EQ(PwTokenizer65599FixedLengthHash("1", 4),
            StringLength("1") + k2 * '1');
  EXPECT_EQ(PwTokenizer65599FixedLengthHash("12", 4),
            StringLength("12") + k2 * '1' + k3 * '2');
  EXPECT_EQ(PwTokenizer65599FixedLengthHash("123", 4),
            StringLength("123") + k2 * '1' + k3 * '2' + k4 * '3');
  EXPECT_EQ(PwTokenizer65599FixedLengthHash("1234", 4),
            StringLength("1234") + k2 * '1' + k3 * '2' + k4 * '3' + k5 * '4');
  EXPECT_EQ(PwTokenizer65599FixedLengthHash("12345", 4),
            StringLength("12345") + k2 * '1' + k3 * '2' + k4 * '3' + k5 * '4');
  EXPECT_EQ(PwTokenizer65599FixedLengthHash("123456", 4),
            StringLength("123456") + k2 * '1' + k3 * '2' + k4 * '3' + k5 * '4');
}

#define _CHECK_HASH_LENGTH(string, length)                                   \
  static_assert(PwTokenizer65599FixedLengthHash(                             \
                    std::string_view(string, sizeof(string) - 1), length) == \
                    PW_TOKENIZER_65599_FIXED_LENGTH_##length##_HASH(string), \
                #length "-byte hash mismatch!")

// Use std::string_view so that \0 can appear in strings.
#define TEST_SUPPORTED_HASHES(string_literal)                                 \
  _CHECK_HASH_LENGTH(string_literal, 80);                                     \
  _CHECK_HASH_LENGTH(string_literal, 96);                                     \
  _CHECK_HASH_LENGTH(string_literal, 128);                                    \
  _CHECK_HASH_LENGTH(string_literal, 256);                                    \
  static_assert(                                                              \
      PwTokenizer65599FixedLengthHash(                                        \
          std::string_view(string_literal, sizeof(string_literal) - 1),       \
          sizeof(string_literal) - 1) == Hash(string_literal),                \
      "Hash function mismatch!");                                             \
  EXPECT_EQ(PwTokenizer65599FixedLengthHash(                                  \
                std::string_view(string_literal, sizeof(string_literal) - 1), \
                sizeof(string_literal) - 1),                                  \
            pw_tokenizer_65599FixedLengthHash(string_literal,                 \
                                              sizeof(string_literal) - 1,     \
                                              sizeof(string_literal) - 1))

TEST(HashMacro, Empty) { TEST_SUPPORTED_HASHES(""); }

TEST(HashMacro, NullTerminatorsAreIncludedInHash) {
  using namespace std::literals;  // use an sv literal to include the \0

  TEST_SUPPORTED_HASHES("hello\0there");
  TEST_SUPPORTED_HASHES("\0");
  TEST_SUPPORTED_HASHES("\0\0");
  TEST_SUPPORTED_HASHES("\0\0\0");

  static_assert(
      PwTokenizer65599FixedLengthHash(""sv, 80) !=
          PwTokenizer65599FixedLengthHash("\0"sv, 80),
      "Hashes of \\0 strings should differ since the lengths are included");
  static_assert(
      PwTokenizer65599FixedLengthHash("\0"sv, 80) !=
          PwTokenizer65599FixedLengthHash("\0\0"sv, 80),
      "Hashes of \\0 strings should differ since the lengths are included");
  static_assert(
      PwTokenizer65599FixedLengthHash("\0\0"sv, 80) !=
          PwTokenizer65599FixedLengthHash("\0\0\0"sv, 80),
      "Hashes of \\0 strings should differ since the lengths are included");
}

TEST(HashMacro, OneChar) {
  TEST_SUPPORTED_HASHES("a");
  TEST_SUPPORTED_HASHES("c");
  TEST_SUPPORTED_HASHES("A");
  TEST_SUPPORTED_HASHES("B");
  TEST_SUPPORTED_HASHES("C");
  TEST_SUPPORTED_HASHES("$");
  TEST_SUPPORTED_HASHES("%");
  TEST_SUPPORTED_HASHES("^");
  TEST_SUPPORTED_HASHES("\xa1");
  TEST_SUPPORTED_HASHES("\xff");
}

TEST(HashMacro, Phrases) {
  TEST_SUPPORTED_HASHES("WASD");
  TEST_SUPPORTED_HASHES("hello, world");
  TEST_SUPPORTED_HASHES("Let the wookie win.");
  TEST_SUPPORTED_HASHES("\x01 more test, just for \xffun");
}

TEST(HashMacro, HashesChange) {
  static_assert(PW_TOKENIZER_65599_FIXED_LENGTH_80_HASH("") !=
                PW_TOKENIZER_65599_FIXED_LENGTH_80_HASH("\0"));
  static_assert(PW_TOKENIZER_65599_FIXED_LENGTH_80_HASH("a") !=
                PW_TOKENIZER_65599_FIXED_LENGTH_80_HASH("b"));
  static_assert(PW_TOKENIZER_65599_FIXED_LENGTH_80_HASH("z") !=
                PW_TOKENIZER_65599_FIXED_LENGTH_80_HASH("aa"));
  static_assert(
      PW_TOKENIZER_65599_FIXED_LENGTH_80_HASH("make sure hashes change") !=
      PW_TOKENIZER_65599_FIXED_LENGTH_80_HASH("MAKE SURE HASHES CHANGE"));
}

// Define string literals to test boundary conditions.
#define LITERAL_79   \
  "0123456789ABCDEF" \
  "0123456789ABCDEF" \
  "0123456789ABCDEF" \
  "0123456789ABCDEF" \
  "0123456789ABCDE"
#define LITERAL_80 LITERAL_79 "F"

#define LITERAL_96 LITERAL_80 "0123456789ABCDEF"

#define LITERAL_128  \
  LITERAL_96         \
  "0123456789ABCDEF" \
  "0123456789ABCDEF"

static_assert(sizeof(LITERAL_79) - 1 == 79);
static_assert(sizeof(LITERAL_80) - 1 == 80);
static_assert(sizeof(LITERAL_96) - 1 == 96);
static_assert(sizeof(LITERAL_128) - 1 == 128);

TEST(HashMacro, HashNearFixedHashLength_80) {
  // 79, 80, 81, 82 bytes
  TEST_SUPPORTED_HASHES(LITERAL_79);
  TEST_SUPPORTED_HASHES(LITERAL_80);
  TEST_SUPPORTED_HASHES(LITERAL_80 "!");
  TEST_SUPPORTED_HASHES(LITERAL_80 "!!");
}

TEST(HashMacro, HashNearFixedHashLength_96) {
  // 95, 96, 97, 98 bytes
  TEST_SUPPORTED_HASHES(LITERAL_80 "0123456789ABCDE");
  TEST_SUPPORTED_HASHES(LITERAL_96);
  TEST_SUPPORTED_HASHES(LITERAL_96 "!");
  TEST_SUPPORTED_HASHES(LITERAL_96 "XY");
}

TEST(HashMacro, HashNearFixedHashLength_128) {
  // 127, 128, 129, 130 bytes
  TEST_SUPPORTED_HASHES(LITERAL_96
                        "0123456789ABCDEF"
                        "0123456789ABCDE");
  TEST_SUPPORTED_HASHES(LITERAL_128);
  TEST_SUPPORTED_HASHES(LITERAL_128 "!");
  TEST_SUPPORTED_HASHES(LITERAL_128 "AB");
}

TEST(HashMacro, HashVeryLongStrings) {
  TEST_SUPPORTED_HASHES(
      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
  TEST_SUPPORTED_HASHES(LITERAL_128 LITERAL_128 LITERAL_128);
}

TEST(HashMacro, ExtraCharactersAreIgnored) {
  static_assert(PW_TOKENIZER_65599_FIXED_LENGTH_80_HASH(LITERAL_80 "?") ==
                PW_TOKENIZER_65599_FIXED_LENGTH_80_HASH(LITERAL_80 "."));

  static_assert(PW_TOKENIZER_65599_FIXED_LENGTH_80_HASH(LITERAL_80 "ABCD") ==
                PW_TOKENIZER_65599_FIXED_LENGTH_80_HASH(LITERAL_80 "zyxw"));

  static_assert(PW_TOKENIZER_65599_FIXED_LENGTH_96_HASH(LITERAL_96 "?") ==
                PW_TOKENIZER_65599_FIXED_LENGTH_96_HASH(LITERAL_96 "."));

  static_assert(PW_TOKENIZER_65599_FIXED_LENGTH_96_HASH(LITERAL_96 "ABCD") ==
                PW_TOKENIZER_65599_FIXED_LENGTH_96_HASH(LITERAL_96 "zyxw"));

  static_assert(PW_TOKENIZER_65599_FIXED_LENGTH_128_HASH(LITERAL_128 "?") ==
                PW_TOKENIZER_65599_FIXED_LENGTH_128_HASH(LITERAL_128 "."));

  static_assert(PW_TOKENIZER_65599_FIXED_LENGTH_128_HASH(LITERAL_128 "ABCD") ==
                PW_TOKENIZER_65599_FIXED_LENGTH_128_HASH(LITERAL_128 "zyxw"));
}

}  // namespace
}  // namespace pw::tokenizer
