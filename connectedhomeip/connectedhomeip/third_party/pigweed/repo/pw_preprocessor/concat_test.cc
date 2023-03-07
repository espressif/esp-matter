// Copyright 2019 The Pigweed Authors
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

#include "pw_preprocessor/concat.h"

#include "gtest/gtest.h"
#include "pw_preprocessor/util.h"

namespace pw {
namespace {

TEST(Concat, WithoutMacroExpansions) {
  static_assert(PW_CONCAT() 9000 PW_CONCAT() == 9000);
  static_assert(PW_CONCAT(1, 2) == 12);
  static_assert(PW_CONCAT(1, 2, 3, 4) == 1234);
  static_assert(PW_CONCAT(1, 2, 3, 4, 5) == 12345);
  static_assert(PW_CONCAT(1, 2, 3, 4, 5, 6) == 123456);
  static_assert(PW_CONCAT(1, 2, 3, 4, 5, 6, 7) == 1234567);
  static_assert(PW_CONCAT(1, 2, 3, 4, 5, 6, 7, 8) == 12345678);

  static_assert(PW_CONCAT(0x, 3, 4, 5, 6, 7, 8, llu) == 0x345678llu);
  static_assert(PW_CONCAT(0x, 3, 4, 5, 6, 7, 8, 9, llu) == 0x3456789llu);
  static_assert(PW_CONCAT(0x, 3, 4, 5, 6, 7, 8, 9, A, llu) == 0x3456789Allu);
  static_assert(PW_CONCAT(0x, 3, 4, 5, 6, 7, 8, 9, A, B, llu) ==
                0x3456789ABllu);
  static_assert(PW_CONCAT(0x, 3, 4, 5, 6, 7, 8, 9, A, B, C, llu) ==
                0x3456789ABCllu);
  static_assert(PW_CONCAT(0x, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, llu) ==
                0x3456789ABCDllu);
  static_assert(PW_CONCAT(0x, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, llu) ==
                0x3456789ABCDEllu);
  static_assert(PW_CONCAT(0x, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F, llu) ==
                0x3456789ABCDEFllu);
  static_assert(PW_CONCAT(0x, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F, F, llu) ==
                0x3456789ABCDEFFllu);
}

// Macros which expand out to test that PW_CONCAT properly expands them.
#define _PW_TEST_SECTION(x) "section: " x
#define _PW_TEST_NUMBER 123
#define _PW_OUTER_MACRO(section_name) \
  _PW_TEST_SECTION(PW_STRINGIFY(PW_CONCAT(section_name, _PW_TEST_NUMBER)))

#define _PW_WHOSE() my
#define _PW_WHAT var

TEST(Concat, WithMacroExpansions) {
  static_assert(PW_CONCAT(_PW_TEST_NUMBER, 5) == 1235);

  int my_var;
  EXPECT_EQ(&PW_CONCAT(_PW_WHOSE(), _, _PW_WHAT), &my_var);

  EXPECT_STREQ(_PW_OUTER_MACRO(what is up), "section: what is up123");
}

// Test concatenation up to the maximum supported length.
TEST(Concat, MaximumConcatenationLength) {
  constexpr int value31_234567890123456789012345678901 = 1337;
  constexpr int value32_2345678901234567890123456789012 = 101010;

  // clang-format off
  static_assert(PW_CONCAT(value31_, 2, 3, 4, 5, 6, 7, 8, 9, 0,
                                 1, 2, 3, 4, 5, 6, 7, 8, 9, 0,
                                 1, 2, 3, 4, 5, 6, 7, 8, 9, 0,
                                 1) == 1337);

  static_assert(PW_CONCAT(value32_, 2, 3, 4, 5, 6, 7, 8, 9, 0,
                                 1, 2, 3, 4, 5, 6, 7, 8, 9, 0,
                                 1, 2, 3, 4, 5, 6, 7, 8, 9, 0,
                                 1, 2) == 101010);
  // clang-format on
}

}  // namespace
}  // namespace pw
