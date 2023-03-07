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

// pw::Result is derived from absl::StatusOr, but has some small differences.
// This test covers basic pw::Result functionality and as well as the features
// supported by pw::Result that are not supported by absl::StatusOr (constexpr
// use in particular).
//
// The complete, thorough pw::Result tests are in statusor_test.cc, which is
// derived from Abseil's tests for absl::StatusOr.

#include "pw_result/result.h"

#include "gtest/gtest.h"
#include "pw_status/status.h"
#include "pw_status/try.h"

namespace pw {
namespace {

TEST(Result, CreateOk) {
  Result<const char*> res("hello");
  EXPECT_TRUE(res.ok());
  EXPECT_EQ(res.status(), OkStatus());
  EXPECT_EQ(res.value(), "hello");
}

TEST(Result, CreateNotOk) {
  Result<int> res(Status::DataLoss());
  EXPECT_FALSE(res.ok());
  EXPECT_EQ(res.status(), Status::DataLoss());
}

TEST(Result, ValueOr) {
  Result<int> good(3);
  Result<int> bad(Status::DataLoss());
  EXPECT_EQ(good.value_or(42), 3);
  EXPECT_EQ(bad.value_or(42), 42);
}

TEST(Result, Deref) {
  struct Tester {
    constexpr bool True() { return true; }
    constexpr bool False() { return false; }
  };

  auto tester = Result<Tester>(Tester());
  EXPECT_TRUE(tester.ok());
  EXPECT_TRUE(tester->True());
  EXPECT_FALSE(tester->False());
  EXPECT_TRUE((*tester).True());
  EXPECT_FALSE((*tester).False());
  EXPECT_EQ(tester.value().True(), tester->True());
  EXPECT_EQ(tester.value().False(), tester->False());
}

TEST(Result, ConstDeref) {
  struct Tester {
    constexpr bool True() const { return true; }
    constexpr bool False() const { return false; }
  };

  const auto tester = Result<Tester>(Tester());
  EXPECT_TRUE(tester.ok());
  EXPECT_TRUE(tester->True());
  EXPECT_FALSE(tester->False());
  EXPECT_TRUE((*tester).True());
  EXPECT_FALSE((*tester).False());
  EXPECT_EQ(tester.value().True(), tester->True());
  EXPECT_EQ(tester.value().False(), tester->False());
}

TEST(Result, ConstructType) {
  struct Point {
    Point(int a, int b) : x(a), y(b) {}

    int x;
    int y;
  };

  Result<Point> origin{std::in_place, 0, 0};
  ASSERT_TRUE(origin.ok());
  ASSERT_EQ(origin.value().x, 0);
  ASSERT_EQ(origin.value().y, 0);
}

Result<float> Divide(float a, float b) {
  if (b == 0) {
    return Status::InvalidArgument();
  }
  return a / b;
}

TEST(Divide, ReturnOk) {
  Result<float> res = Divide(10, 5);
  ASSERT_TRUE(res.ok());
  EXPECT_EQ(res.value(), 2.0f);
}

TEST(Divide, ReturnNotOk) {
  Result<float> res = Divide(10, 0);
  EXPECT_FALSE(res.ok());
  EXPECT_EQ(res.status(), Status::InvalidArgument());
}

Result<bool> ReturnResult(Result<bool> result) { return result; }

Status TryResultAssign(Result<bool> result) {
  PW_TRY_ASSIGN(const bool value, ReturnResult(result));

  // Any status other than OK should have already returned.
  EXPECT_EQ(result.status(), OkStatus());
  EXPECT_EQ(value, result.value());
  return result.status();
}

TEST(Result, TryAssign) {
  EXPECT_EQ(TryResultAssign(Status::Cancelled()), Status::Cancelled());
  EXPECT_EQ(TryResultAssign(Status::DataLoss()), Status::DataLoss());
  EXPECT_EQ(TryResultAssign(Status::Unimplemented()), Status::Unimplemented());
  EXPECT_EQ(TryResultAssign(false), OkStatus());
  EXPECT_EQ(TryResultAssign(true), OkStatus());
}

struct Value {
  int number;
};

TEST(Result, ConstexprOk) {
  static constexpr pw::Result<Value> kResult(Value{123});

  static_assert(kResult.status() == pw::OkStatus());
  static_assert(kResult.ok());

  static_assert((*kResult).number == 123);
  static_assert((*std::move(kResult)).number == 123);

  static_assert(kResult->number == 123);
  static_assert(std::move(kResult)->number == 123);

  static_assert(kResult.value().number == 123);
  static_assert(std::move(kResult).value().number == 123);

  static_assert(kResult.value_or(Value{99}).number == 123);
  static_assert(std::move(kResult).value_or(Value{99}).number == 123);
}

TEST(Result, ConstexprNotOk) {
  static constexpr pw::Result<Value> kResult(pw::Status::NotFound());

  static_assert(kResult.status() == pw::Status::NotFound());
  static_assert(!kResult.ok());

  static_assert(kResult.value_or(Value{99}).number == 99);
  static_assert(std::move(kResult).value_or(Value{99}).number == 99);
}

TEST(Result, ConstexprNotOkCopy) {
  static constexpr pw::Result<Value> kResult(pw::Status::NotFound());
  constexpr pw::Result<Value> kResultCopy(kResult);

  static_assert(kResultCopy.status() == pw::Status::NotFound());
  static_assert(!kResultCopy.ok());

  static_assert(kResultCopy.value_or(Value{99}).number == 99);
  static_assert(std::move(kResultCopy).value_or(Value{99}).number == 99);
}

auto multiply = [](int x) -> Result<int> { return x * 2; };
auto add_two = [](int x) -> Result<int> { return x + 2; };
auto fail_unknown = [](int) -> Result<int> { return Status::Unknown(); };

TEST(Result, AndThenNonConstLValueRefInvokeSuccess) {
  Result<int> r = 32;
  auto ret = r.and_then(multiply);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(*ret, 64);
}

TEST(Result, AndThenNonConstLValueRefInvokeFail) {
  Result<int> r = 32;
  auto ret = r.and_then(fail_unknown);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::Unknown());
}

TEST(Result, AndThenNonConstLValueRefSkips) {
  Result<int> r = Status::NotFound();
  auto ret = r.and_then(multiply);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::NotFound());
}

TEST(Result, AndThenNonConstRvalueRefInvokeSuccess) {
  Result<int> r = 32;
  auto ret = std::move(r).and_then(multiply);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(*ret, 64);
}

TEST(Result, AndThenNonConstRvalueRefInvokeFails) {
  Result<int> r = 64;
  auto ret = std::move(r).and_then(fail_unknown);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::Unknown());
}

TEST(Result, AndThenNonConstRvalueRefSkips) {
  Result<int> r = Status::NotFound();
  auto ret = std::move(r).and_then(multiply);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::NotFound());
}

TEST(Result, AndThenConstLValueRefInvokeSuccess) {
  const Result<int> r = 32;
  auto ret = r.and_then(multiply);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(*ret, 64);
}

TEST(Result, AndThenConstLValueRefInvokeFail) {
  const Result<int> r = 32;
  auto ret = r.and_then(fail_unknown);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::Unknown());
}

TEST(Result, AndThenConstLValueRefSkips) {
  const Result<int> r = Status::NotFound();
  auto ret = r.and_then(multiply);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::NotFound());
}

TEST(Result, AndThenConstRValueRefInvokeSuccess) {
  const Result<int> r = 32;
  auto ret = std::move(r).and_then(multiply);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(*ret, 64);
}

TEST(Result, AndThenConstRValueRefInvokeFail) {
  const Result<int> r = 32;
  auto ret = std::move(r).and_then(fail_unknown);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::Unknown());
}

TEST(Result, AndThenConstRValueRefSkips) {
  const Result<int> r = Status::NotFound();
  auto ret = std::move(r).and_then(multiply);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::NotFound());
}

TEST(Result, AndThenMultipleChained) {
  Result<int> r = 32;
  auto ret = r.and_then(multiply).and_then(add_two).and_then(multiply);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(*ret, 132);
}

auto return_status = [](Status) { return Status::Unknown(); };
auto return_result = [](Status) { return Result<int>(Status::Internal()); };

TEST(Result, OrElseNonConstLValueRefSkips) {
  Result<int> r = 32;
  auto ret = r.or_else(return_status);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(*ret, 32);
}

TEST(Result, OrElseNonConstLValueRefStatusInvokes) {
  Result<int> r = Status::NotFound();
  auto ret = r.or_else(return_status);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::Unknown());
}

TEST(Result, OrElseNonConstLValueRefResultInvokes) {
  Result<int> r = Status::NotFound();
  auto ret = r.or_else(return_result);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::Internal());
}

TEST(Result, OrElseNonConstLValueRefVoidSkips) {
  Result<int> r = 32;
  bool invoked = false;
  auto ret = r.or_else([&invoked](Status) { invoked = true; });
  EXPECT_FALSE(invoked);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(*ret, 32);
}

TEST(Result, OrElseNonConstLValueRefVoidInvokes) {
  Result<int> r = Status::NotFound();
  bool invoked = false;
  auto ret = r.or_else([&invoked](Status) { invoked = true; });
  EXPECT_TRUE(invoked);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::NotFound());
}

TEST(Result, OrElseNonConstRValueRefSkips) {
  Result<int> r = 32;
  auto ret = std::move(r).or_else(return_status);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(*ret, 32);
}

TEST(Result, OrElseNonConstRValueRefStatusInvokes) {
  Result<int> r = Status::NotFound();
  auto ret = std::move(r).or_else(return_status);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::Unknown());
}

TEST(Result, OrElseNonConstRValueRefResultInvokes) {
  Result<int> r = Status::NotFound();
  auto ret = std::move(r).or_else(return_result);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::Internal());
}

TEST(Result, OrElseNonConstRValueRefVoidSkips) {
  Result<int> r = 32;
  bool invoked = false;
  auto ret = std::move(r).or_else([&invoked](Status) { invoked = true; });
  EXPECT_FALSE(invoked);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(*ret, 32);
}

TEST(Result, OrElseNonConstRValueRefVoidInvokes) {
  Result<int> r = Status::NotFound();
  bool invoked = false;
  auto ret = std::move(r).or_else([&invoked](Status) { invoked = true; });
  EXPECT_TRUE(invoked);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::NotFound());
}

TEST(Result, OrElseConstLValueRefSkips) {
  const Result<int> r = 32;
  auto ret = r.or_else(return_status);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(*ret, 32);
}

TEST(Result, OrElseConstLValueRefStatusInvokes) {
  const Result<int> r = Status::NotFound();
  auto ret = r.or_else(return_status);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::Unknown());
}

TEST(Result, OrElseConstLValueRefResultInvokes) {
  const Result<int> r = Status::NotFound();
  auto ret = r.or_else(return_result);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::Internal());
}

TEST(Result, OrElseConstLValueRefVoidSkips) {
  const Result<int> r = 32;
  bool invoked = false;
  auto ret = r.or_else([&invoked](Status) { invoked = true; });
  EXPECT_FALSE(invoked);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(*ret, 32);
}

TEST(Result, OrElseConstLValueRefVoidInvokes) {
  const Result<int> r = Status::NotFound();
  bool invoked = false;
  auto ret = r.or_else([&invoked](Status) { invoked = true; });
  EXPECT_TRUE(invoked);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::NotFound());
}

TEST(Result, OrElseConstRValueRefSkips) {
  const Result<int> r = 32;
  auto ret = std::move(r).or_else(return_status);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(*ret, 32);
}

TEST(Result, OrElseConstRValueRefStatusInvokes) {
  const Result<int> r = Status::NotFound();
  auto ret = std::move(r).or_else(return_status);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::Unknown());
}

TEST(Result, OrElseConstRValueRefResultInvokes) {
  const Result<int> r = Status::NotFound();
  auto ret = std::move(r).or_else(return_result);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::Internal());
}

TEST(Result, OrElseConstRValueRefVoidSkips) {
  const Result<int> r = 32;
  bool invoked = false;
  auto ret = std::move(r).or_else([&invoked](Status) { invoked = true; });
  EXPECT_FALSE(invoked);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(*ret, 32);
}

TEST(Result, OrElseConstRValueRefVoidInvokes) {
  const Result<int> r = Status::NotFound();
  bool invoked = false;
  auto ret = std::move(r).or_else([&invoked](Status) { invoked = true; });
  EXPECT_TRUE(invoked);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::NotFound());
}

TEST(Result, OrElseMultipleChained) {
  Result<int> r = Status::NotFound();
  bool invoked = false;
  auto ret =
      r.or_else(return_result).or_else([&invoked](Status) { invoked = true; });
  EXPECT_TRUE(invoked);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::Internal());
}

auto multiply_int = [](int x) { return x * 2; };
auto add_two_int = [](int x) { return x + 2; };
auto make_value = [](int x) { return Value{.number = x}; };

TEST(Result, TransformNonConstLValueRefInvokeSuccess) {
  Result<int> r = 32;
  auto ret = r.transform(multiply_int);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(*ret, 64);
}

TEST(Result, TransformNonConstLValueRefInvokeDifferentType) {
  Result<int> r = 32;
  auto ret = r.transform(make_value);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(ret->number, 32);
}

TEST(Result, TransformNonConstLValueRefSkips) {
  Result<int> r = Status::NotFound();
  auto ret = r.transform(multiply_int);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::NotFound());
}

TEST(Result, TransformNonConstRValueRefInvokeSuccess) {
  Result<int> r = 32;
  auto ret = std::move(r).transform(multiply_int);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(*ret, 64);
}

TEST(Result, TransformNonConstRValueRefInvokeDifferentType) {
  Result<int> r = 32;
  auto ret = std::move(r).transform(make_value);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(ret->number, 32);
}

TEST(Result, TransformNonConstRValueRefSkips) {
  Result<int> r = Status::NotFound();
  auto ret = std::move(r).transform(multiply_int);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::NotFound());
}

TEST(Result, TransformConstLValueRefInvokeSuccess) {
  const Result<int> r = 32;
  auto ret = r.transform(multiply_int);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(*ret, 64);
}

TEST(Result, TransformConstLValueRefInvokeDifferentType) {
  const Result<int> r = 32;
  auto ret = r.transform(make_value);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(ret->number, 32);
}

TEST(Result, TransformConstLValueRefSkips) {
  const Result<int> r = Status::NotFound();
  auto ret = r.transform(multiply_int);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::NotFound());
}

TEST(Result, TransformConstRValueRefInvokeSuccess) {
  const Result<int> r = 32;
  auto ret = std::move(r).transform(multiply_int);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(*ret, 64);
}

TEST(Result, TransformConstRValueRefInvokeDifferentType) {
  const Result<int> r = 32;
  auto ret = std::move(r).transform(make_value);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(ret->number, 32);
}

TEST(Result, TransformConstRValueRefSkips) {
  const Result<int> r = Status::NotFound();
  auto ret = std::move(r).transform(multiply_int);
  ASSERT_FALSE(ret.ok());
  EXPECT_EQ(ret.status(), Status::NotFound());
}

TEST(Result, TransformMultipleChained) {
  Result<int> r = 32;
  auto ret = r.transform(multiply_int)
                 .transform(add_two_int)
                 .transform(multiply_int)
                 .transform(make_value);
  ASSERT_TRUE(ret.ok());
  EXPECT_EQ(ret->number, 132);
}

}  // namespace
}  // namespace pw
