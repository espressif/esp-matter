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
#pragma once

#if defined(PW_NEGATIVE_COMPILATION_TESTS_ENABLED) && \
    PW_NEGATIVE_COMPILATION_TESTS_ENABLED == 1

// Declares a compilation failure test. Must be used in a #if or #elif
// statement. The code in the section must not compile.
//
// Internally, this expands to a macro that enables or disables the code section
// for the compilation failure test. It would be possible to use a plain macro
// and defined(...) for this, but the function-like macro gives a cleaner
// interface and catches typos that would otherwise cause tests to silently be
// skipped.
#define PW_NC_TEST(test_case) PW_NC_TEST_EXECUTE_CASE_##test_case

#else

// If testing is disbaled, always evaluate false to disable the test case.
#define PW_NC_TEST(test_case) 0 && PW_NC_TEST_EXECUTE_CASE_##test_case

#endif  // PW_NEGATIVE_COMPILATION_TESTS_ENABLED

// Checks that the compilation output matches the provided regex in a negative
// compilation test. The regex must be a simple string literal. In Python, the
// string is taken directly from the C++ source, interpreted as a regular string
// literal, and compiled as a regular expression.
#define PW_NC_EXPECT(regex) \
  static_assert(PW_NEGATIVE_COMPILATION_TESTS_ENABLED == 1, "")

// Checks that the compilation output matches the regex in Clang compilers only.
#define PW_NC_EXPECT_CLANG(regex) \
  static_assert(PW_NEGATIVE_COMPILATION_TESTS_ENABLED == 1, "")

// Checks that the compilation output matches the regex in GCC compilers only.
#define PW_NC_EXPECT_GCC(regex) \
  static_assert(PW_NEGATIVE_COMPILATION_TESTS_ENABLED == 1, "")
