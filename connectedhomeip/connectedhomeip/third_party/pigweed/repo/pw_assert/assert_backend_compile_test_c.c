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

// This is a compile test that verifies that the assert macros compile in a C
// context. They are not correctness checks.
//
// Note: These tests cannot be run with a normal assert backend, since they
// will abort. However, the assert_basic backend supports non-aborting assert;
// see the note in assert_backend_compile_test.cc.

#include "pw_assert/check.h"

static void EnsureNullIsIncluded(void) {
  // This is a compile check to ensure NULL is defined. It comes before the
  // status.h include to ensure we don't accidentally get NULL from status.h.
  PW_CHECK_NOTNULL(0xa);
  PW_CHECK_NOTNULL(0x0);
}

#include <stdbool.h>

#include "pw_assert/assert.h"
#include "pw_assert/short.h"
#include "pw_status/status.h"

#ifdef __cplusplus
#error "This file must be compiled as plain C to verify C compilation works."
#endif  // __cplusplus

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

static int Add3(int a, int b, int c) { return a + b + c; }

void AssertBackendCompileTestsInC(void) {
  {  // TEST(Crash, WithAndWithoutMessageArguments)
    MAYBE_SKIP_TEST;
    PW_CRASH(FAIL_IF_HIDDEN);
    PW_CRASH(FAIL_IF_HIDDEN_ARGS, z);
  }

  {  // TEST(Check, NoMessage)
    MAYBE_SKIP_TEST;
    PW_CHECK(1);
    PW_CHECK(0);
  }

  {  // TEST(Check, WithMessageAndArgs)
    MAYBE_SKIP_TEST;
    PW_CHECK(1, FAIL_IF_DISPLAYED);
    PW_CHECK(1, FAIL_IF_DISPLAYED_ARGS, z);

    PW_CHECK(0, FAIL_IF_HIDDEN);
    PW_CHECK(0, FAIL_IF_HIDDEN_ARGS, z);
  }

  {  // TEST(Check, IntComparison)
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

  {  // TEST(Check, UintComparison)
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

  {  // TEST(Check, PtrComparison)
    MAYBE_SKIP_TEST;
    void* x_ptr = (void*)(50);
    void* y_ptr = (void*)(66);

    PW_CHECK_PTR_EQ(x_ptr, y_ptr);
    PW_CHECK_PTR_LE(x_ptr, y_ptr, "PTR: " FAIL_IF_DISPLAYED);
    PW_CHECK_PTR_LE(x_ptr, y_ptr, "PTR: " FAIL_IF_DISPLAYED_ARGS, z);

    PW_CHECK_PTR_GE(x_ptr, y_ptr);
    PW_CHECK_PTR_GE(x_ptr, y_ptr, "PTR: " FAIL_IF_HIDDEN);
    PW_CHECK_PTR_GE(x_ptr, y_ptr, "PTR: " FAIL_IF_HIDDEN_ARGS, z);

    PW_CHECK_NOTNULL(0xa);
    PW_CHECK_NOTNULL(0x0);
  }

  {  // TEST(Check, FloatComparison)
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
  {  // TEST(DCheck, Sampling)
    MAYBE_SKIP_TEST;
    PW_DCHECK(5 == 10);
    PW_DCHECK(5 == 10, "Message");
    PW_DCHECK(5 == 10, "Message: %d", 5);
    PW_DCHECK_INT_LE(5.4, 10.0);
    PW_DCHECK_FLOAT_EXACT_EQ(5.4, 10.0, "Message");
  }

  {  // TEST(Check, ComparisonArgumentsWithCommas)
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

  {  // TEST(Check, ShortNamesWork) {
    MAYBE_SKIP_TEST;

    // Crash
    CRASH(FAIL_IF_HIDDEN);
    CRASH(FAIL_IF_HIDDEN_ARGS, z);

    // Check
    CHECK(1, FAIL_IF_DISPLAYED);
    CHECK(1, FAIL_IF_DISPLAYED_ARGS, z);
    CHECK(0, FAIL_IF_HIDDEN);
    CHECK(0, FAIL_IF_HIDDEN_ARGS, z);

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

  {  // Compile tests for PW_CHECK_OK().
    PW_CHECK_OK(PW_STATUS_OK);
    PW_CHECK_OK(PW_STATUS_OK, "msg");
    PW_CHECK_OK(PW_STATUS_OK, "msg: %d", 5);
    PW_DCHECK_OK(PW_STATUS_OK);
    PW_DCHECK_OK(PW_STATUS_OK, "msg");
    PW_DCHECK_OK(PW_STATUS_OK, "msg: %d", 5);
  }

  {  // TEST(Assert, Basic)
    MAYBE_SKIP_TEST;
    PW_ASSERT(false);
    PW_ASSERT(123 == 456);
  }

  {  // Compile tests for PW_ASSERT().
    PW_ASSERT(true);
    PW_ASSERT(123 != 456);

    PW_DASSERT(true);
    PW_DASSERT(123 != 456);
  }

  EnsureNullIsIncluded();
}
