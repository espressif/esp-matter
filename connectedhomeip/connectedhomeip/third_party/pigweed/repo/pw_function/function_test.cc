// Copyright 2021 The Pigweed Authors
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

#include "pw_function/function.h"

#include "gtest/gtest.h"
#include "pw_compilation_testing/negative_compilation.h"
#include "pw_polyfill/language_feature_macros.h"

namespace pw {
namespace {

#if PW_NC_TEST(CannotInstantiateWithNonFunction)
PW_NC_EXPECT("must be instantiated with a function type");

[[maybe_unused]] Function<int> function_pointer;

#elif PW_NC_TEST(CannotInstantiateWithFunctionPointer1)
PW_NC_EXPECT("must be instantiated with a function type");

[[maybe_unused]] Function<void (*)()> function_pointer;

#elif PW_NC_TEST(CannotInstantiateWithFunctionPointer2)
PW_NC_EXPECT("must be instantiated with a function type");

[[maybe_unused]] void SomeFunction(int);

[[maybe_unused]] Function<decltype(&SomeFunction)> function_pointer;

#elif PW_NC_TEST(CannotInstantiateWithFunctionReference)
PW_NC_EXPECT("must be instantiated with a function type");

[[maybe_unused]] Function<void (&)()> function_pointer;

#endif  // PW_NC_TEST

// Ensure that Function can be constant initialized.
[[maybe_unused]] PW_CONSTINIT Function<void()> can_be_constant_initialized;

int Multiply(int a, int b) { return a * b; }

TEST(Function, OperatorCall) {
  Function<int(int, int)> multiply(Multiply);
  EXPECT_EQ(multiply(3, 7), 21);
}

void CallbackAdd(int a, int b, pw::Function<void(int sum)> callback) {
  callback(a + b);
}

void InlineCallbackAdd(int a,
                       int b,
                       pw::InlineFunction<void(int sum)> callback) {
  callback(a + b);
}

int add_result = -1;

void free_add_callback(int sum) { add_result = sum; }

TEST(Function, ConstructInPlace_FreeFunction) {
  add_result = -1;
  CallbackAdd(25, 17, free_add_callback);
  EXPECT_EQ(add_result, 42);
}

TEST(Function, ConstructInPlace_NonCapturingLambda) {
  add_result = -1;
  CallbackAdd(25, 18, [](int sum) { add_result = sum; });
  EXPECT_EQ(add_result, 43);
}

TEST(Function, ConstructInPlace_CapturingLambda) {
  int result = -1;
  CallbackAdd(25, 19, [&](int sum) { result = sum; });
  EXPECT_EQ(result, 44);
}

TEST(InlineFunction, ConstructInPlace_FreeFunction) {
  add_result = -1;
  InlineCallbackAdd(25, 17, free_add_callback);
  EXPECT_EQ(add_result, 42);
}

TEST(InlineFunction, ConstructInPlace_NonCapturingLambda) {
  add_result = -1;
  InlineCallbackAdd(25, 18, [](int sum) { add_result = sum; });
  EXPECT_EQ(add_result, 43);
}

TEST(InlineFunction, ConstructInPlace_CapturingLambda) {
  int result = -1;
  InlineCallbackAdd(25, 19, [&](int sum) { result = sum; });
  EXPECT_EQ(result, 44);
}

class CallableObject {
 public:
  CallableObject(int* result) : result_(result) {}

  CallableObject(CallableObject&& other) = default;
  CallableObject& operator=(CallableObject&& other) = default;

  void operator()(int sum) { *result_ = sum; }

 private:
  int* result_;
};

TEST(Function, ConstructInPlace_CallableObject) {
  int result = -1;
  CallbackAdd(25, 20, CallableObject(&result));
  EXPECT_EQ(result, 45);
}

TEST(InlineFunction, ConstructInPlace_CallableObject) {
  int result = -1;
  InlineCallbackAdd(25, 20, CallableObject(&result));
  EXPECT_EQ(result, 45);
}

class MemberFunctionTest : public ::testing::Test {
 protected:
  MemberFunctionTest() : result_(-1) {}

  void set_result(int result) { result_ = result; }

  int result_;
};

TEST_F(MemberFunctionTest, ConstructInPlace_Lambda) {
  CallbackAdd(25, 21, [this](int sum) { set_result(sum); });
  EXPECT_EQ(result_, 46);
}

TEST(Function, Null_OperatorBool) {
  Closure implicit_null;
  Closure explicit_null(nullptr);
  Closure assigned_null = nullptr;
  Closure not_null([]() {});

  EXPECT_FALSE(bool(implicit_null));
  EXPECT_FALSE(bool(explicit_null));
  EXPECT_FALSE(bool(assigned_null));
  EXPECT_TRUE(bool(not_null));

  EXPECT_TRUE(!implicit_null);
  EXPECT_TRUE(!explicit_null);
  EXPECT_TRUE(!assigned_null);
  EXPECT_FALSE(!not_null);
}

TEST(Function, Null_OperatorEquals) {
  Closure implicit_null;
  Closure explicit_null(nullptr);
  Closure assigned_null = nullptr;
  Closure not_null([]() {});

  EXPECT_TRUE(implicit_null == nullptr);
  EXPECT_TRUE(explicit_null == nullptr);
  EXPECT_TRUE(assigned_null == nullptr);
  EXPECT_TRUE(not_null != nullptr);

  EXPECT_FALSE(implicit_null != nullptr);
  EXPECT_FALSE(explicit_null != nullptr);
  EXPECT_FALSE(assigned_null != nullptr);
  EXPECT_FALSE(not_null == nullptr);
}

TEST(Function, Null_Set) {
  Closure function = []() {};
  EXPECT_NE(function, nullptr);
  function = nullptr;
  EXPECT_EQ(function, nullptr);
}

void DoNothing() {}

TEST(Function, Null_FunctionPointer) {
  void (*ptr)() = DoNothing;
  Closure not_null(ptr);
  EXPECT_NE(not_null, nullptr);
  ptr = nullptr;
  Closure is_null(ptr);
  EXPECT_EQ(is_null, nullptr);
}

TEST(Function, Move_Null) {
  Closure moved;
  EXPECT_EQ(moved, nullptr);
  Closure function(std::move(moved));
  EXPECT_EQ(function, nullptr);

// Ignore use-after-move.
#ifndef __clang_analyzer__
  EXPECT_EQ(moved, nullptr);
#endif  // __clang_analyzer__
}

TEST(Function, MoveAssign_Null) {
  Closure moved;
  EXPECT_EQ(moved, nullptr);
  Closure function = std::move(moved);
  EXPECT_EQ(function, nullptr);

// Ignore use-after-move.
#ifndef __clang_analyzer__
  EXPECT_EQ(moved, nullptr);
#endif  // __clang_analyzer__
}

TEST(Function, Move_Inline) {
  Function<int(int, int)> moved(Multiply);
  EXPECT_NE(moved, nullptr);
  Function<int(int, int)> multiply(std::move(moved));
  EXPECT_EQ(multiply(3, 3), 9);

// Ignore use-after-move.
#ifndef __clang_analyzer__
  EXPECT_EQ(moved, nullptr);
#endif  // __clang_analyzer__
}

TEST(InlineFunction, Move_InlineFunctionToFunction) {
  InlineFunction<int(int, int)> moved(Multiply);
  EXPECT_NE(moved, nullptr);
  Function<int(int, int)> multiply(std::move(moved));
  EXPECT_EQ(multiply(3, 3), 9);

// Ignore use-after-move.
#ifndef __clang_analyzer__
  EXPECT_EQ(moved, nullptr);
#endif  // __clang_analyzer__
}

TEST(Function, MoveAssign_Inline) {
  Function<int(int, int)> moved(Multiply);
  EXPECT_NE(moved, nullptr);
  Function<int(int, int)> multiply = std::move(moved);
  EXPECT_EQ(multiply(3, 3), 9);

// Ignore use-after-move.
#ifndef __clang_analyzer__
  EXPECT_EQ(moved, nullptr);
#endif  // __clang_analyzer__
}

TEST(InlineFunction, MoveAssign_InlineFunctionToFunction) {
  InlineFunction<int(int, int)> moved(Multiply);
  EXPECT_NE(moved, nullptr);
  Function<int(int, int)> multiply = std::move(moved);
  EXPECT_EQ(multiply(3, 3), 9);

// Ignore use-after-move.
#ifndef __clang_analyzer__
  EXPECT_EQ(moved, nullptr);
#endif  // __clang_analyzer__
}

TEST(Function, MoveAssign_Callable) {
  Function<int(int, int)> operation = Multiply;
  EXPECT_EQ(operation(3, 3), 9);
  operation = [](int a, int b) -> int { return a + b; };
  EXPECT_EQ(operation(3, 3), 6);
}

TEST(InlineFunction, MoveAssign_Callable) {
  InlineFunction<int(int, int)> operation = Multiply;
  EXPECT_EQ(operation(3, 3), 9);
  operation = [](int a, int b) -> int { return a + b; };
  EXPECT_EQ(operation(3, 3), 6);
}

class MoveTracker {
 public:
  MoveTracker() : move_count_(0) {}

  MoveTracker(MoveTracker&& other) : move_count_(other.move_count_ + 1) {}
  MoveTracker& operator=(MoveTracker&& other) = default;

  int operator()() const { return move_count_; }

 private:
  int move_count_;
};

TEST(Function, Move_CustomObject) {
  Function<int()> moved((MoveTracker()));
  EXPECT_EQ(moved(), 1);
  Function<int()> tracker(std::move(moved));
  EXPECT_EQ(tracker(), 2);

// Ignore use-after-move.
#ifndef __clang_analyzer__
  EXPECT_EQ(moved, nullptr);
#endif  // __clang_analyzer__
}

TEST(Function, MoveAssign_CustomObject) {
  Function<int()> moved((MoveTracker()));
  EXPECT_EQ(moved(), 1);
  Function<int()> tracker = std::move(moved);
  EXPECT_EQ(tracker(), 2);

// Ignore use-after-move.
#ifndef __clang_analyzer__
  EXPECT_EQ(moved, nullptr);
#endif  // __clang_analyzer__
}

TEST(Function, MoveOnlyType) {
  class MoveOnlyType {
   public:
    MoveOnlyType() = default;

    MoveOnlyType(const MoveOnlyType& other) = delete;
    MoveOnlyType& operator=(const MoveOnlyType& other) = delete;

    MoveOnlyType(MoveOnlyType&&) = default;
    MoveOnlyType& operator=(MoveOnlyType&&) = default;

    bool ItsWorking() const { return true; }
  };

  pw::Function<bool(MoveOnlyType)> function = [](MoveOnlyType value) {
    return value.ItsWorking();
  };

  MoveOnlyType move_only;
  EXPECT_TRUE(function(std::move(move_only)));
}

TEST(Function, CallbackCanOnlyBeCalledOnce) {
  Callback<void()> cb([]() {});
  cb();
  EXPECT_FALSE(cb);
  EXPECT_EQ(cb, nullptr);
}

TEST(Function, CallbackDestroysTargetAfterBeingCalled) {
  class MoveOnlyDestructionCounter {
   public:
    MoveOnlyDestructionCounter(int* destroyed_count)
        : destroyed_(destroyed_count) {}

    MoveOnlyDestructionCounter(const MoveOnlyDestructionCounter& other) =
        delete;
    MoveOnlyDestructionCounter& operator=(
        const MoveOnlyDestructionCounter& other) = delete;

    MoveOnlyDestructionCounter(MoveOnlyDestructionCounter&& t) {
      *this = std::move(t);
    }
    MoveOnlyDestructionCounter& operator=(MoveOnlyDestructionCounter&& t) {
      destroyed_ = t.destroyed_;
      t.destroyed_ = nullptr;
      return *this;
    }

    ~MoveOnlyDestructionCounter() {
      if (destroyed_) {
        (*destroyed_)++;
      }
    }

   private:
    int* destroyed_;
  };

  int destroyed_count = 0;
  MoveOnlyDestructionCounter destruction_counter(&destroyed_count);
  Callback<void()> cb = [destruction_counter =
                             std::move(destruction_counter)]() {};
  EXPECT_EQ(destroyed_count, 0);
  cb();
  EXPECT_EQ(destroyed_count, 1);
}

}  // namespace
}  // namespace pw

namespace obscure_different_namespace_which_should_never_collide {
namespace {

TEST(Function, Null_OperatorEquals_DifferentNamespace) {
  pw::Closure implicit_null;
  pw::Closure explicit_null(nullptr);
  pw::Closure assigned_null = nullptr;
  pw::Closure not_null([]() {});

  EXPECT_TRUE(implicit_null == nullptr);
  EXPECT_TRUE(explicit_null == nullptr);
  EXPECT_TRUE(assigned_null == nullptr);
  EXPECT_TRUE(not_null != nullptr);

  EXPECT_FALSE(implicit_null != nullptr);
  EXPECT_FALSE(explicit_null != nullptr);
  EXPECT_FALSE(assigned_null != nullptr);
  EXPECT_FALSE(not_null == nullptr);
}

}  // namespace
}  // namespace obscure_different_namespace_which_should_never_collide
