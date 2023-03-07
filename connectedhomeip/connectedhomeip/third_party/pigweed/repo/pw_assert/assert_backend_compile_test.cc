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

// This series of "tests" is more a compile test to verify that the assert
// backend is able to compile the constructs promised by the assert facade.
// Truly testing the backend in a general way from the facade is impossible
// since the device will go down when an assert triggers, and so that must be
// handled inside the individual backends.
//
// NOTE: While these tests are not intended to run, it *is* possible to run
// them with the assert_basic backend, in a special mode where the assert
// statements fall through instead of aborting.
//
// To run these "tests" for pw_assert_basic, you must modify two things:
//
//   (1) Set DISABLE_ASSERT_TEST_EXECUTION 0 in assert_backend_compile_test.cc
//   (2) Set DISABLE_ASSERT_TEST_EXECUTION 0 in assert_backend_compile_test.c
//   (3) Set PW_ASSERT_BASIC_DISABLE_NORETURN 1 in assert_basic.h
//   (4) Compile and run the resulting binary, paying attention to the
//       displayed error messages. If "FAIL IF DISPLAYED" is printed, the
//       test has failed. If any "FAIL_IF_HIDDEN" asserts are not displayed,
//       the test has failed. Obviously manually verifying these is a pain
//       and so this is not a suitable test for production.
//
// TODO(b/235289499): Add verification of the actually recorded asserts
// statements.

#include "gtest/gtest.h"
#include "pw_assert/short.h"
#include "pw_status/status.h"

// This is a global constant to feed into the formatter for tests.
// Intended to pair with FAIL_IF_DISPLAYED_ARGS or FAIL_IF_HIDDEN_ARGS.
static const int z = 10;

// At some point in the future when there is a proper test system in place for
// crashing, the below strings can help indicate pass/fail for a check.

#define FAIL_IF_DISPLAYED "FAIL IF DISPLAYED"
#define FAIL_IF_DISPLAYED_ARGS "FAIL IF DISPLAYED: %d"

#define FAIL_IF_HIDDEN "FAIL IF HIDDEN"
#define FAIL_IF_HIDDEN_ARGS "FAIL IF HIDDEN: %d"

// This switch exists to support compiling and/or running the tests.
#define DISABLE_ASSERT_TEST_EXECUTION 1
#if DISABLE_ASSERT_TEST_EXECUTION
#define MAYBE_SKIP_TEST return
#else
#define MAYBE_SKIP_TEST ;
#endif

namespace {

TEST(Crash, WithAndWithoutMessageArguments) {
  MAYBE_SKIP_TEST;
  PW_CRASH(FAIL_IF_HIDDEN);
  PW_CRASH(FAIL_IF_HIDDEN_ARGS, z);
}

TEST(Check, NoMessage) {
  MAYBE_SKIP_TEST;
  PW_CHECK(true);
  PW_CHECK(false);
}

TEST(Check, WithMessageAndArgs) {
  MAYBE_SKIP_TEST;
  PW_CHECK(true, FAIL_IF_DISPLAYED);
  PW_CHECK(true, FAIL_IF_DISPLAYED_ARGS, z);

  PW_CHECK(false, FAIL_IF_HIDDEN);
  PW_CHECK(false, FAIL_IF_HIDDEN_ARGS, z);
}

TEST(Check, IntComparison) {
  MAYBE_SKIP_TEST;
  int x_int = 50;
  int y_int = 66;

  PW_CHECK_INT_LE(x_int, y_int);
  PW_CHECK_INT_LE(x_int, y_int, "INT: " FAIL_IF_DISPLAYED);
  PW_CHECK_INT_LE(x_int, y_int, "INT: " FAIL_IF_DISPLAYED_ARGS, z);

  PW_CHECK_INT_GE(x_int, y_int);
  PW_CHECK_INT_GE(x_int, y_int, "INT: " FAIL_IF_HIDDEN);
  PW_CHECK_INT_GE(x_int, y_int, "INT: " FAIL_IF_HIDDEN_ARGS, z);
}

TEST(Check, UintComparison) {
  MAYBE_SKIP_TEST;
  unsigned int x_uint = 50;
  unsigned int y_uint = 66;

  PW_CHECK_UINT_LE(x_uint, y_uint);
  PW_CHECK_UINT_LE(x_uint, y_uint, "UINT: " FAIL_IF_DISPLAYED);
  PW_CHECK_UINT_LE(x_uint, y_uint, "UINT: " FAIL_IF_DISPLAYED_ARGS, z);

  PW_CHECK_UINT_GE(x_uint, y_uint);
  PW_CHECK_UINT_GE(x_uint, y_uint, "UINT: " FAIL_IF_HIDDEN);
  PW_CHECK_UINT_GE(x_uint, y_uint, "UINT: " FAIL_IF_HIDDEN_ARGS, z);
}

TEST(Check, PtrComparison) {
  MAYBE_SKIP_TEST;
  void* x_ptr = reinterpret_cast<void*>(50);
  void* y_ptr = reinterpret_cast<void*>(66);

  PW_CHECK_PTR_EQ(x_ptr, y_ptr);
  PW_CHECK_PTR_LE(x_ptr, y_ptr, "PTR: " FAIL_IF_DISPLAYED);
  PW_CHECK_PTR_LE(x_ptr, y_ptr, "PTR: " FAIL_IF_DISPLAYED_ARGS, z);

  PW_CHECK_PTR_GE(x_ptr, y_ptr);
  PW_CHECK_PTR_GE(x_ptr, y_ptr, "PTR: " FAIL_IF_HIDDEN);
  PW_CHECK_PTR_GE(x_ptr, y_ptr, "PTR: " FAIL_IF_HIDDEN_ARGS, z);
}

TEST(Check, FloatComparison) {
  MAYBE_SKIP_TEST;
  float x_float = 50.5;
  float y_float = 66.5;

  PW_CHECK_FLOAT_EXACT_LE(x_float, y_float);
  PW_CHECK_FLOAT_EXACT_LE(x_float, y_float, "FLOAT: " FAIL_IF_DISPLAYED);
  PW_CHECK_FLOAT_EXACT_LE(
      x_float, y_float, "FLOAT: " FAIL_IF_DISPLAYED_ARGS, z);

  PW_CHECK_FLOAT_EXACT_GE(x_float, y_float);
  PW_CHECK_FLOAT_EXACT_GE(x_float, y_float, "FLOAT: " FAIL_IF_HIDDEN);
  PW_CHECK_FLOAT_EXACT_GE(x_float, y_float, "FLOAT: " FAIL_IF_HIDDEN_ARGS, z);
}

// Don't exhaustively test the DCHECKs but have a sampling of them.
TEST(DCheck, Sampling) {
  MAYBE_SKIP_TEST;
  PW_DCHECK(5 == 10);
  PW_DCHECK(5 == 10, "Message");
  PW_DCHECK(5 == 10, "Message: %d", 5);
  PW_DCHECK_INT_LE(5.4, 10.0);
  PW_DCHECK_FLOAT_EXACT_EQ(5.4, 10.0, "Message");
}

static int Add3(int a, int b, int c) { return a + b + c; }

TEST(Check, ComparisonArgumentsWithCommas) {
  MAYBE_SKIP_TEST;
  int x_int = 50;
  int y_int = 66;

  PW_CHECK_INT_LE(Add3(1, 2, 3), y_int);
  PW_CHECK_INT_LE(x_int, Add3(1, 2, 3));

  PW_CHECK_INT_LE(Add3(1, 2, 3), y_int, FAIL_IF_DISPLAYED);
  PW_CHECK_INT_LE(x_int, Add3(1, 2, 3), FAIL_IF_DISPLAYED_ARGS, z);

  PW_CHECK_INT_LE(Add3(1, 2, 3), Add3(1, 2, 3), "INT: " FAIL_IF_DISPLAYED);
  PW_CHECK_INT_LE(x_int, y_int, "INT: " FAIL_IF_DISPLAYED_ARGS, z);
}

TEST(Check, ShortNamesWork) {
  MAYBE_SKIP_TEST;

  // Crash
  CRASH(FAIL_IF_HIDDEN);
  CRASH(FAIL_IF_HIDDEN_ARGS, z);

  // Check
  CHECK(true, FAIL_IF_DISPLAYED);
  CHECK(true, FAIL_IF_DISPLAYED_ARGS, z);
  CHECK(false, FAIL_IF_HIDDEN);
  CHECK(false, FAIL_IF_HIDDEN_ARGS, z);

  // Check with binary comparison
  int x_int = 50;
  int y_int = 66;

  CHECK_INT_LE(Add3(1, 2, 3), y_int);
  CHECK_INT_LE(x_int, Add3(1, 2, 3));

  CHECK_INT_LE(Add3(1, 2, 3), y_int, FAIL_IF_DISPLAYED);
  CHECK_INT_LE(x_int, Add3(1, 2, 3), FAIL_IF_DISPLAYED_ARGS, z);

  CHECK_INT_LE(Add3(1, 2, 3), Add3(1, 2, 3), "INT: " FAIL_IF_DISPLAYED);
  CHECK_INT_LE(x_int, y_int, "INT: " FAIL_IF_DISPLAYED_ARGS, z);
}

pw::Status MakeStatus(pw::Status status) { return status; }

TEST(Check, CheckOkMacrosCompile) {
  MAYBE_SKIP_TEST;
  pw::Status status = pw::Status::Unknown();

  // Typical case with long names.
  PW_CHECK_OK(status);
  PW_CHECK_OK(status, "msg");
  PW_CHECK_OK(status, "msg: %d", 5);

  // Short names.
  CHECK_OK(status);
  CHECK_OK(status, "msg");
  CHECK_OK(status, "msg: %d", 5);

  // Status from a literal.
  PW_CHECK_OK(pw::OkStatus());

  // Status from a function.
  PW_CHECK_OK(MakeStatus(pw::OkStatus()));

  // Status from C enums.
  PW_CHECK_OK(PW_STATUS_OK);
}

// These are defined in assert_test.c, to test C compatibility.
extern "C" void AssertBackendCompileTestsInC();

TEST(Check, AssertBackendCompileTestsInC) {
  MAYBE_SKIP_TEST;
  AssertBackendCompileTestsInC();
}

}  // namespace
