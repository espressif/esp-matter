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

// This test directly verifies the facade logic, by leveraging a fake backend
// that captures arguments and returns rather than aborting execution.

#include "pw_assert_test/fake_backend.h"

// This directly includes the assert facade implementation header rather than
// going through the backend header indirection mechanism, to prevent the real
// assert backend from triggering.
//
// clang-format off
#include "pw_assert/internal/check_impl.h"
// clang-format on

#include "gtest/gtest.h"
#include "pw_compilation_testing/negative_compilation.h"
#include "pw_status/status.h"

namespace {

#define EXPECT_MESSAGE(expected_assert_message)                        \
  do {                                                                 \
    EXPECT_STREQ(pw_captured_assert.message, expected_assert_message); \
  } while (0)

class AssertFailTest : public ::testing::Test {
 protected:
  void SetUp() override { pw_captured_assert.triggered = 0; }
  void TearDown() override { EXPECT_EQ(pw_captured_assert.triggered, 1); }
};

class AssertPassTest : public ::testing::Test {
 protected:
  void SetUp() override { pw_captured_assert.triggered = 0; }
  void TearDown() override { EXPECT_EQ(pw_captured_assert.triggered, 0); }
};

// PW_CRASH(...)
TEST_F(AssertFailTest, CrashMessageNoArguments) {
  PW_CRASH("Goodbye");
  EXPECT_MESSAGE("Goodbye");
}
TEST_F(AssertFailTest, CrashMessageWithArguments) {
  PW_CRASH("Goodbye cruel %s", "world");
  EXPECT_MESSAGE("Goodbye cruel world");
}

// PW_CHECK(...) - No message
TEST_F(AssertPassTest, CheckNoMessage) { PW_CHECK(true); }
TEST_F(AssertFailTest, CheckNoMessage) {
  PW_CHECK(false);
  EXPECT_MESSAGE("Check failed: false. ");
}
TEST_F(AssertPassTest, CheckNoMessageComplexExpression) { PW_CHECK(2 == 2); }
TEST_F(AssertFailTest, CheckNoMessageComplexExpression) {
  PW_CHECK(1 == 2);
  EXPECT_MESSAGE("Check failed: 1 == 2. ");
}

// PW_CHECK(..., msg) - With message; with and without arguments.
TEST_F(AssertPassTest, CheckMessageNoArguments) { PW_CHECK(true, "Hello"); }
TEST_F(AssertFailTest, CheckMessageNoArguments) {
  PW_CHECK(false, "Hello");
  EXPECT_MESSAGE("Check failed: false. Hello");
}
TEST_F(AssertPassTest, CheckMessageWithArguments) {
  PW_CHECK(true, "Hello %d", 5);
}
TEST_F(AssertFailTest, CheckMessageWithArguments) {
  PW_CHECK(false, "Hello %d", 5);
  EXPECT_MESSAGE("Check failed: false. Hello 5");
}

// PW_CHECK_INT_*(...)
// Binary checks with ints, comparisons: <, <=, =, !=, >, >=.

// Test message formatting separate from the triggering.
// Only test formatting for the type once.
TEST_F(AssertFailTest, IntLessThanNoMessageNoArguments) {
  PW_CHECK_INT_LT(5, -2);
  EXPECT_MESSAGE("Check failed: 5 (=5) < -2 (=-2). ");
}
TEST_F(AssertFailTest, IntLessThanMessageNoArguments) {
  PW_CHECK_INT_LT(5, -2, "msg");
  EXPECT_MESSAGE("Check failed: 5 (=5) < -2 (=-2). msg");
}
TEST_F(AssertFailTest, IntLessThanMessageArguments) {
  PW_CHECK_INT_LT(5, -2, "msg: %d", 6);
  EXPECT_MESSAGE("Check failed: 5 (=5) < -2 (=-2). msg: 6");
}

// Test comparison boundaries.

// INT <
TEST_F(AssertPassTest, IntLt1) { PW_CHECK_INT_LT(-1, 2); }
TEST_F(AssertPassTest, IntLt2) { PW_CHECK_INT_LT(1, 2); }
TEST_F(AssertFailTest, IntLt3) { PW_CHECK_INT_LT(-1, -2); }
TEST_F(AssertFailTest, IntLt4) { PW_CHECK_INT_LT(1, 1); }

// INT <=
TEST_F(AssertPassTest, IntLe1) { PW_CHECK_INT_LE(-1, 2); }
TEST_F(AssertPassTest, IntLe2) { PW_CHECK_INT_LE(1, 2); }
TEST_F(AssertFailTest, IntLe3) { PW_CHECK_INT_LE(-1, -2); }
TEST_F(AssertPassTest, IntLe4) { PW_CHECK_INT_LE(1, 1); }

// INT ==
TEST_F(AssertFailTest, IntEq1) { PW_CHECK_INT_EQ(-1, 2); }
TEST_F(AssertFailTest, IntEq2) { PW_CHECK_INT_EQ(1, 2); }
TEST_F(AssertFailTest, IntEq3) { PW_CHECK_INT_EQ(-1, -2); }
TEST_F(AssertPassTest, IntEq4) { PW_CHECK_INT_EQ(1, 1); }

// INT !=
TEST_F(AssertPassTest, IntNe1) { PW_CHECK_INT_NE(-1, 2); }
TEST_F(AssertPassTest, IntNe2) { PW_CHECK_INT_NE(1, 2); }
TEST_F(AssertPassTest, IntNe3) { PW_CHECK_INT_NE(-1, -2); }
TEST_F(AssertFailTest, IntNe4) { PW_CHECK_INT_NE(1, 1); }

// INT >
TEST_F(AssertFailTest, IntGt1) { PW_CHECK_INT_GT(-1, 2); }
TEST_F(AssertFailTest, IntGt2) { PW_CHECK_INT_GT(1, 2); }
TEST_F(AssertPassTest, IntGt3) { PW_CHECK_INT_GT(-1, -2); }
TEST_F(AssertFailTest, IntGt4) { PW_CHECK_INT_GT(1, 1); }

// INT >=
TEST_F(AssertFailTest, IntGe1) { PW_CHECK_INT_GE(-1, 2); }
TEST_F(AssertFailTest, IntGe2) { PW_CHECK_INT_GE(1, 2); }
TEST_F(AssertPassTest, IntGe3) { PW_CHECK_INT_GE(-1, -2); }
TEST_F(AssertPassTest, IntGe4) { PW_CHECK_INT_GE(1, 1); }

// PW_CHECK_UINT_*(...)
// Binary checks with uints, comparisons: <, <=, =, !=, >, >=.

// Test message formatting separate from the triggering.
// Only test formatting for the type once.
TEST_F(AssertFailTest, UintLessThanNoMessageNoArguments) {
  PW_CHECK_UINT_LT(5, 2);
  EXPECT_MESSAGE("Check failed: 5 (=5) < 2 (=2). ");
}
TEST_F(AssertFailTest, UintLessThanMessageNoArguments) {
  PW_CHECK_UINT_LT(5, 2, "msg");
  EXPECT_MESSAGE("Check failed: 5 (=5) < 2 (=2). msg");
}
TEST_F(AssertFailTest, UintLessThanMessageArguments) {
  PW_CHECK_UINT_LT(5, 2, "msg: %d", 6);
  EXPECT_MESSAGE("Check failed: 5 (=5) < 2 (=2). msg: 6");
}

// Test comparison boundaries.

// UINT <
TEST_F(AssertPassTest, UintLt1) { PW_CHECK_UINT_LT(1, 2); }
TEST_F(AssertFailTest, UintLt2) { PW_CHECK_UINT_LT(2, 2); }
TEST_F(AssertFailTest, UintLt3) { PW_CHECK_UINT_LT(2, 1); }

// UINT <=
TEST_F(AssertPassTest, UintLe1) { PW_CHECK_UINT_LE(1, 2); }
TEST_F(AssertPassTest, UintLe2) { PW_CHECK_UINT_LE(2, 2); }
TEST_F(AssertFailTest, UintLe3) { PW_CHECK_UINT_LE(2, 1); }

// UINT ==
TEST_F(AssertFailTest, UintEq1) { PW_CHECK_UINT_EQ(1, 2); }
TEST_F(AssertPassTest, UintEq2) { PW_CHECK_UINT_EQ(2, 2); }
TEST_F(AssertFailTest, UintEq3) { PW_CHECK_UINT_EQ(2, 1); }

// UINT !=
TEST_F(AssertPassTest, UintNe1) { PW_CHECK_UINT_NE(1, 2); }
TEST_F(AssertFailTest, UintNe2) { PW_CHECK_UINT_NE(2, 2); }
TEST_F(AssertPassTest, UintNe3) { PW_CHECK_UINT_NE(2, 1); }

// UINT >
TEST_F(AssertFailTest, UintGt1) { PW_CHECK_UINT_GT(1, 2); }
TEST_F(AssertFailTest, UintGt2) { PW_CHECK_UINT_GT(2, 2); }
TEST_F(AssertPassTest, UintGt3) { PW_CHECK_UINT_GT(2, 1); }

// UINT >=
TEST_F(AssertFailTest, UintGe1) { PW_CHECK_UINT_GE(1, 2); }
TEST_F(AssertPassTest, UintGe2) { PW_CHECK_UINT_GE(2, 2); }
TEST_F(AssertPassTest, UintGe3) { PW_CHECK_UINT_GE(2, 1); }

// PW_CHECK_PTR_*(...)
// Binary checks with uints, comparisons: <, <=, =, !=, >, >=.
// Note: The format checks are skipped since they're not portable.

// Test comparison boundaries.

// PTR <
TEST_F(AssertPassTest, PtrLt1) {
  PW_CHECK_PTR_LT(reinterpret_cast<void*>(0xa), reinterpret_cast<void*>(0xb));
}
TEST_F(AssertFailTest, PtrLt2) {
  PW_CHECK_PTR_LT(reinterpret_cast<void*>(0xb), reinterpret_cast<void*>(0xb));
}
TEST_F(AssertFailTest, PtrLt3) {
  PW_CHECK_PTR_LT(reinterpret_cast<void*>(0xb), reinterpret_cast<void*>(0xa));
}

// PTR <=
TEST_F(AssertPassTest, PtrLe1) {
  PW_CHECK_PTR_LE(reinterpret_cast<void*>(0xa), reinterpret_cast<void*>(0xb));
}
TEST_F(AssertPassTest, PtrLe2) {
  PW_CHECK_PTR_LE(reinterpret_cast<void*>(0xb), reinterpret_cast<void*>(0xb));
}
TEST_F(AssertFailTest, PtrLe3) {
  PW_CHECK_PTR_LE(reinterpret_cast<void*>(0xb), reinterpret_cast<void*>(0xa));
}

// PTR ==
TEST_F(AssertFailTest, PtrEq1) {
  PW_CHECK_PTR_EQ(reinterpret_cast<void*>(0xa), reinterpret_cast<void*>(0xb));
}
TEST_F(AssertPassTest, PtrEq2) {
  PW_CHECK_PTR_EQ(reinterpret_cast<void*>(0xb), reinterpret_cast<void*>(0xb));
}
TEST_F(AssertFailTest, PtrEq3) {
  PW_CHECK_PTR_EQ(reinterpret_cast<void*>(0xb), reinterpret_cast<void*>(0xa));
}

// PTR !=
TEST_F(AssertPassTest, PtrNe1) {
  PW_CHECK_PTR_NE(reinterpret_cast<void*>(0xa), reinterpret_cast<void*>(0xb));
}
TEST_F(AssertFailTest, PtrNe2) {
  PW_CHECK_PTR_NE(reinterpret_cast<void*>(0xb), reinterpret_cast<void*>(0xb));
}
TEST_F(AssertPassTest, PtrNe3) {
  PW_CHECK_PTR_NE(reinterpret_cast<void*>(0xb), reinterpret_cast<void*>(0xa));
}

// PTR >
TEST_F(AssertFailTest, PtrGt1) {
  PW_CHECK_PTR_GT(reinterpret_cast<void*>(0xa), reinterpret_cast<void*>(0xb));
}
TEST_F(AssertFailTest, PtrGt2) {
  PW_CHECK_PTR_GT(reinterpret_cast<void*>(0xb), reinterpret_cast<void*>(0xb));
}
TEST_F(AssertPassTest, PtrGt3) {
  PW_CHECK_PTR_GT(reinterpret_cast<void*>(0xb), reinterpret_cast<void*>(0xa));
}

// PTR >=
TEST_F(AssertFailTest, PtrGe1) {
  PW_CHECK_PTR_GE(reinterpret_cast<void*>(0xa), reinterpret_cast<void*>(0xb));
}
TEST_F(AssertPassTest, PtrGe2) {
  PW_CHECK_PTR_GE(reinterpret_cast<void*>(0xb), reinterpret_cast<void*>(0xb));
}
TEST_F(AssertPassTest, PtrGe3) {
  PW_CHECK_PTR_GE(reinterpret_cast<void*>(0xb), reinterpret_cast<void*>(0xa));
}

// NOTNULL
TEST_F(AssertPassTest, PtrNotNull) {
  PW_CHECK_NOTNULL(reinterpret_cast<void*>(0xa));
}
TEST_F(AssertFailTest, PtrNotNull) {
  PW_CHECK_NOTNULL(reinterpret_cast<void*>(0x0));
}

[[maybe_unused]] void Function1() {}
[[maybe_unused]] bool Function2(int) { return false; }

// NOTNULL for function poionters
TEST_F(AssertPassTest, FunctionPtrNotNull) {
  PW_CHECK_NOTNULL(&Function1);
  PW_CHECK_NOTNULL(&Function2);
}
TEST_F(AssertFailTest, FunctionPtrNotNull) {
  void (*const function)() = nullptr;
  PW_CHECK_NOTNULL(function);
}

[[maybe_unused]] void CompareIntWithString() {
#if PW_NC_TEST(CompareIntWithString)
  PW_NC_EXPECT("cannot initialize|invalid conversion");

  PW_CHECK_INT_EQ(123l, "This check message is accidentally compared to 123!");
#endif  // PW_NC_TEST
}

// Note: Due to platform inconsistencies, the below test for the NOTNULL
// message doesn't work. Some platforms print NULL formatted as %p as "(nil)",
// others "0x0". Leaving this here for reference.
//
//   TEST_F(AssertFailTest, PtrNotNullDescription) {
//     intptr_t intptr = 0;
//     PW_CHECK_NOTNULL(intptr);
//     EXPECT_MESSAGE("Check failed: intptr (=0x0) != nullptr (=0x0). ");
//   }

// PW_CHECK_FLOAT_*(...)
// Binary checks with floats, comparisons: EXACT_LT, EXACT_LE, NEAR, EXACT_EQ,
// EXACT_NE, EXACT_GE, EXACT_GT.

// Test message formatting separate from the triggering.
// Only test formatting for the type once.
TEST_F(AssertFailTest, FloatLessThanNoMessageNoArguments) {
  PW_CHECK_FLOAT_EXACT_LT(5.2, 2.3);
  EXPECT_MESSAGE("Check failed: 5.2 (=5.200000) < 2.3 (=2.300000). ");
}
TEST_F(AssertFailTest, FloatLessThanMessageNoArguments) {
  PW_CHECK_FLOAT_EXACT_LT(5.2, 2.3, "msg");
  EXPECT_MESSAGE("Check failed: 5.2 (=5.200000) < 2.3 (=2.300000). msg");
}
TEST_F(AssertFailTest, FloatLessThanMessageArguments) {
  PW_CHECK_FLOAT_EXACT_LT(5.2, 2.3, "msg: %d", 6);
  EXPECT_MESSAGE("Check failed: 5.2 (=5.200000) < 2.3 (=2.300000). msg: 6");
}
// Check float NEAR both above and below the permitted range.
TEST_F(AssertFailTest, FloatNearAboveNoMessageNoArguments) {
  PW_CHECK_FLOAT_NEAR(5.2, 2.3, 0.1);
  EXPECT_MESSAGE(
      "Check failed: 5.2 (=5.200000) <= 2.3 + abs_tolerance (=2.400000). ");
}
TEST_F(AssertFailTest, FloatNearAboveMessageNoArguments) {
  PW_CHECK_FLOAT_NEAR(5.2, 2.3, 0.1, "msg");
  EXPECT_MESSAGE(
      "Check failed: 5.2 (=5.200000) <= 2.3 + abs_tolerance (=2.400000). msg");
}
TEST_F(AssertFailTest, FloatNearAboveMessageArguments) {
  PW_CHECK_FLOAT_NEAR(5.2, 2.3, 0.1, "msg: %d", 6);
  EXPECT_MESSAGE(
      "Check failed: 5.2 (=5.200000) <= 2.3 + abs_tolerance (=2.400000). msg: "
      "6");
}
TEST_F(AssertFailTest, FloatNearBelowNoMessageNoArguments) {
  PW_CHECK_FLOAT_NEAR(1.2, 2.3, 0.1);
  EXPECT_MESSAGE(
      "Check failed: 1.2 (=1.200000) >= 2.3 - abs_tolerance (=2.200000). ");
}
TEST_F(AssertFailTest, FloatNearBelowMessageNoArguments) {
  PW_CHECK_FLOAT_NEAR(1.2, 2.3, 0.1, "msg");
  EXPECT_MESSAGE(
      "Check failed: 1.2 (=1.200000) >= 2.3 - abs_tolerance (=2.200000). msg");
}
TEST_F(AssertFailTest, FloatNearBelowMessageArguments) {
  PW_CHECK_FLOAT_NEAR(1.2, 2.3, 0.1, "msg: %d", 6);
  EXPECT_MESSAGE(
      "Check failed: 1.2 (=1.200000) >= 2.3 - abs_tolerance (=2.200000). msg: "
      "6");
}
// Test comparison boundaries.
// Note: The below example numbers all round to integer 1, to detect accidental
// integer conversions in the asserts.

// FLOAT <
TEST_F(AssertPassTest, FloatLt1) { PW_CHECK_FLOAT_EXACT_LT(1.1, 1.2); }
TEST_F(AssertFailTest, FloatLt2) { PW_CHECK_FLOAT_EXACT_LT(1.2, 1.2); }
TEST_F(AssertFailTest, FloatLt3) { PW_CHECK_FLOAT_EXACT_LT(1.2, 1.1); }

// FLOAT <=
TEST_F(AssertPassTest, FloatLe1) { PW_CHECK_FLOAT_EXACT_LE(1.1, 1.2); }
TEST_F(AssertPassTest, FloatLe2) { PW_CHECK_FLOAT_EXACT_LE(1.2, 1.2); }
TEST_F(AssertFailTest, FloatLe3) { PW_CHECK_FLOAT_EXACT_LE(1.2, 1.1); }

// FLOAT ~= based on absolute error.
TEST_F(AssertFailTest, FloatNearAbs1) { PW_CHECK_FLOAT_NEAR(1.09, 1.2, 0.1); }
TEST_F(AssertPassTest, FloatNearAbs2) { PW_CHECK_FLOAT_NEAR(1.1, 1.2, 0.1); }
TEST_F(AssertPassTest, FloatNearAbs3) { PW_CHECK_FLOAT_NEAR(1.2, 1.2, 0.1); }
TEST_F(AssertPassTest, FloatNearAbs4) { PW_CHECK_FLOAT_NEAR(1.2, 1.1, 0.1); }
TEST_F(AssertFailTest, FloatNearAbs5) { PW_CHECK_FLOAT_NEAR(1.21, 1.1, 0.1); }
// Make sure the abs_tolerance is asserted to be >= 0.
TEST_F(AssertFailTest, FloatNearAbs6) { PW_CHECK_FLOAT_NEAR(1.2, 1.2, -0.1); }
TEST_F(AssertPassTest, FloatNearAbs7) { PW_CHECK_FLOAT_NEAR(1.2, 1.2, 0.0); }

// FLOAT ==
TEST_F(AssertFailTest, FloatEq1) { PW_CHECK_FLOAT_EXACT_EQ(1.1, 1.2); }
TEST_F(AssertPassTest, FloatEq2) { PW_CHECK_FLOAT_EXACT_EQ(1.2, 1.2); }
TEST_F(AssertFailTest, FloatEq3) { PW_CHECK_FLOAT_EXACT_EQ(1.2, 1.1); }

// FLOAT !=
TEST_F(AssertPassTest, FloatNe1) { PW_CHECK_FLOAT_EXACT_NE(1.1, 1.2); }
TEST_F(AssertFailTest, FloatNe2) { PW_CHECK_FLOAT_EXACT_NE(1.2, 1.2); }
TEST_F(AssertPassTest, FloatNe3) { PW_CHECK_FLOAT_EXACT_NE(1.2, 1.1); }

// FLOAT >
TEST_F(AssertFailTest, FloatGt1) { PW_CHECK_FLOAT_EXACT_GT(1.1, 1.2); }
TEST_F(AssertFailTest, FloatGt2) { PW_CHECK_FLOAT_EXACT_GT(1.2, 1.2); }
TEST_F(AssertPassTest, FloatGt3) { PW_CHECK_FLOAT_EXACT_GT(1.2, 1.1); }

// FLOAT >=
TEST_F(AssertFailTest, FloatGe1) { PW_CHECK_FLOAT_EXACT_GE(1.1, 1.2); }
TEST_F(AssertPassTest, FloatGe2) { PW_CHECK_FLOAT_EXACT_GE(1.2, 1.2); }
TEST_F(AssertPassTest, FloatGe3) { PW_CHECK_FLOAT_EXACT_GE(1.2, 1.1); }

// Nested comma handling.
static int Add3(int a, int b, int c) { return a + b + c; }

TEST_F(AssertFailTest, CommaHandlingLeftSide) {
  PW_CHECK_INT_EQ(Add3(1, 2, 3), 4);
  EXPECT_MESSAGE("Check failed: Add3(1, 2, 3) (=6) == 4 (=4). ");
}
TEST_F(AssertFailTest, CommaHandlingRightSide) {
  PW_CHECK_INT_EQ(4, Add3(1, 2, 3));
  EXPECT_MESSAGE("Check failed: 4 (=4) == Add3(1, 2, 3) (=6). ");
}

// Verify that the CHECK_*(x,y) macros only evaluate their arguments once.
struct MultiEvaluateTestContext {
  int IncrementAndReturnZero() {
    counter += 1;
    return 0;
  }
  int counter = 0;
};

TEST(AssertPass, CheckSingleSideEffectingCall) {
  MultiEvaluateTestContext ctx;
  PW_CHECK(ctx.IncrementAndReturnZero() == 0);
  EXPECT_EQ(ctx.counter, 1);
}
TEST(AssertFail, CheckSingleSideEffectingCall) {
  MultiEvaluateTestContext ctx;
  PW_CHECK(ctx.IncrementAndReturnZero() == 1);
  EXPECT_EQ(ctx.counter, 1);
}
TEST(AssertPass, BinaryOpSingleSideEffectingCall) {
  MultiEvaluateTestContext ctx;
  PW_CHECK_INT_EQ(0, ctx.IncrementAndReturnZero());
  EXPECT_EQ(ctx.counter, 1);
}
TEST(AssertPass, BinaryOpTwoSideEffectingCalls) {
  MultiEvaluateTestContext ctx;
  PW_CHECK_INT_EQ(ctx.IncrementAndReturnZero(), ctx.IncrementAndReturnZero());
  EXPECT_EQ(ctx.counter, 2);
}
TEST(AssertFail, BinaryOpSingleSideEffectingCall) {
  MultiEvaluateTestContext ctx;
  PW_CHECK_INT_EQ(12314, ctx.IncrementAndReturnZero());
  EXPECT_EQ(ctx.counter, 1);
}
TEST(AssertFail, BinaryOpTwoSideEffectingCalls) {
  MultiEvaluateTestContext ctx;
  PW_CHECK_INT_EQ(ctx.IncrementAndReturnZero() + 10,
                  ctx.IncrementAndReturnZero());
  EXPECT_EQ(ctx.counter, 2);
}
TEST(AssertPass, CheckOkSingleSideEffectingCall) {
  MultiEvaluateTestContext ctx;
  PW_CHECK_OK(ctx.IncrementAndReturnZero() ? pw::OkStatus() : pw::OkStatus());
  EXPECT_EQ(ctx.counter, 1);
}
TEST(AssertFail, CheckOkSingleSideEffectingCall) {
  MultiEvaluateTestContext ctx;
  PW_CHECK_OK(ctx.IncrementAndReturnZero() ? pw::Status::NotFound()
                                           : pw::Status::NotFound());
  EXPECT_EQ(ctx.counter, 1);
}

// Verify side effects of debug checks work as expected.
// Only check a couple of cases, since the logic is all the same.

// When DCHECKs are enabled, they behave the same as normal checks.
// When DCHECKs are disabled, they should not trip, and their arguments
// shouldn't be evaluated.
constexpr int kExpectedSideEffects = PW_ASSERT_ENABLE_DEBUG;

TEST(AssertPass, DCheckEnabledSingleSideEffectingCall) {
  MultiEvaluateTestContext ctx;
  PW_DCHECK(ctx.IncrementAndReturnZero() == 0);
  EXPECT_EQ(ctx.counter, kExpectedSideEffects);
}
TEST(AssertFail, DCheckEnabledSingleSideEffectingCall) {
  MultiEvaluateTestContext ctx;
  PW_DCHECK(ctx.IncrementAndReturnZero() == 1);
  EXPECT_EQ(ctx.counter, kExpectedSideEffects);
}
TEST(AssertPass, DCheckEnabledBinaryOpSingleSideEffectingCall) {
  MultiEvaluateTestContext ctx;
  PW_DCHECK_INT_EQ(0, ctx.IncrementAndReturnZero());
  EXPECT_EQ(ctx.counter, kExpectedSideEffects);
}
TEST(AssertPass, DCheckEnabledBinaryOpTwoSideEffectingCalls) {
  MultiEvaluateTestContext ctx;
  PW_DCHECK_INT_EQ(ctx.IncrementAndReturnZero(), ctx.IncrementAndReturnZero());
  EXPECT_EQ(ctx.counter, 2 * kExpectedSideEffects);
}
TEST(AssertFail, DCheckEnabledBinaryOpSingleSideEffectingCall) {
  MultiEvaluateTestContext ctx;
  PW_DCHECK_INT_EQ(12314, ctx.IncrementAndReturnZero());
  EXPECT_EQ(ctx.counter, kExpectedSideEffects);
}
TEST(AssertFail, DCheckEnabledBinaryOpTwoSideEffectingCalls) {
  MultiEvaluateTestContext ctx;
  PW_DCHECK_INT_EQ(ctx.IncrementAndReturnZero() + 10,
                   ctx.IncrementAndReturnZero());
  EXPECT_EQ(ctx.counter, 2 * kExpectedSideEffects);
}
TEST(AssertPass, DCheckOkSingleSideEffectingCall) {
  MultiEvaluateTestContext ctx;
  PW_DCHECK_OK(ctx.IncrementAndReturnZero() ? pw::OkStatus() : pw::OkStatus());
  EXPECT_EQ(ctx.counter, kExpectedSideEffects);
}
TEST(AssertFail, DCheckOkSingleSideEffectingCall) {
  MultiEvaluateTestContext ctx;
  PW_DCHECK_OK(ctx.IncrementAndReturnZero() ? pw::Status::NotFound()
                                            : pw::Status::NotFound());
  EXPECT_EQ(ctx.counter, kExpectedSideEffects);
}

// Verify PW_CHECK_OK, including message handling.
TEST_F(AssertFailTest, StatusNotOK) {
  pw::Status status = pw::Status::Unknown();
  PW_CHECK_OK(status);
  EXPECT_MESSAGE("Check failed: status (=UNKNOWN) == OkStatus() (=OK). ");
}

TEST_F(AssertFailTest, StatusNotOKMessageNoArguments) {
  pw::Status status = pw::Status::Unknown();
  PW_CHECK_OK(status, "msg");
  EXPECT_MESSAGE("Check failed: status (=UNKNOWN) == OkStatus() (=OK). msg");
}

TEST_F(AssertFailTest, StatusNotOKMessageArguments) {
  pw::Status status = pw::Status::Unknown();
  PW_CHECK_OK(status, "msg: %d", 5);
  EXPECT_MESSAGE("Check failed: status (=UNKNOWN) == OkStatus() (=OK). msg: 5");
}

// Example expression for the test below.
pw::Status DoTheThing() { return pw::Status::ResourceExhausted(); }

TEST_F(AssertFailTest, NonTrivialExpression) {
  PW_CHECK_OK(DoTheThing());
  EXPECT_MESSAGE(
      "Check failed: DoTheThing() (=RESOURCE_EXHAUSTED) == OkStatus() (=OK). ");
}

// Note: This function seems pointless but it is not, since pw::Status::FOO
// constants are not actually status objects, but code objects. This way we can
// ensure the macros work with both real status objects and literals.
TEST_F(AssertPassTest, Function) { PW_CHECK_OK(pw::OkStatus()); }
TEST_F(AssertPassTest, Enum) { PW_CHECK_OK(PW_STATUS_OK); }
TEST_F(AssertFailTest, Function) { PW_CHECK_OK(pw::Status::Unknown()); }
TEST_F(AssertFailTest, Enum) { PW_CHECK_OK(PW_STATUS_UNKNOWN); }

#if PW_ASSERT_ENABLE_DEBUG

// In debug mode, the asserts should check their arguments.
TEST_F(AssertPassTest, DCheckFunction) { PW_DCHECK_OK(pw::OkStatus()); }
TEST_F(AssertPassTest, DCheckEnum) { PW_DCHECK_OK(PW_STATUS_OK); }
TEST_F(AssertFailTest, DCheckFunction) { PW_DCHECK_OK(pw::Status::Unknown()); }
TEST_F(AssertFailTest, DCheckEnum) { PW_DCHECK_OK(PW_STATUS_UNKNOWN); }
#else   // PW_ASSERT_ENABLE_DEBUG

// In release mode, all the asserts should pass.
TEST_F(AssertPassTest, DCheckFunction_Ok) { PW_DCHECK_OK(pw::OkStatus()); }
TEST_F(AssertPassTest, DCheckEnum_Ok) { PW_DCHECK_OK(PW_STATUS_OK); }
TEST_F(AssertPassTest, DCheckFunction_Err) {
  PW_DCHECK_OK(pw::Status::Unknown());
}
TEST_F(AssertPassTest, DCheckEnum_Err) { PW_DCHECK_OK(PW_STATUS_UNKNOWN); }
#endif  // PW_ASSERT_ENABLE_DEBUG

// TODO(keir): Figure out how to run some of these tests is C.

}  // namespace
