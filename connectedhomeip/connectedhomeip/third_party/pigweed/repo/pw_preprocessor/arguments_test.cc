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

// Many of these tests are static asserts. If these compile, they pass. The TEST
// functions are used for organization only.

#include "pw_preprocessor/arguments.h"

#include <tuple>

#include "gtest/gtest.h"

namespace pw {
namespace {

#define EMPTY_ARG

TEST(HasArgs, WithoutArguments) {
  static_assert(PW_HAS_ARGS() == 0);
  static_assert(PW_HAS_ARGS(/**/) == 0);
  static_assert(PW_HAS_ARGS(/* uhm, hi */) == 0);
  static_assert(PW_HAS_ARGS(EMPTY_ARG) == 0);

  // Test how the macro handles whitespace and comments.
  // clang-format off
  static_assert(PW_HAS_ARGS(     ) == 0);
  static_assert(PW_HAS_ARGS(
      ) == 0);
  static_assert(PW_HAS_ARGS(
      // wow
      // This is a comment.
      ) == 0);
  // clang-format on

  static_assert(PW_EMPTY_ARGS() == 1);
  static_assert(PW_EMPTY_ARGS(/* hello */) == 1);
  static_assert(PW_EMPTY_ARGS(
                    // hello
                    /* goodbye */) == 1);
}

TEST(HasArgs, WithArguments) {
  static_assert(PW_HAS_ARGS(()) == 1);
  static_assert(PW_HAS_ARGS(0) == 1);
  static_assert(PW_HAS_ARGS(, ) == 1);
  static_assert(PW_HAS_ARGS(a, b, c) == 1);
  static_assert(PW_HAS_ARGS(PW_HAS_ARGS) == 1);
  static_assert(PW_HAS_ARGS(PW_HAS_ARGS()) == 1);

  static_assert(PW_EMPTY_ARGS(0) == 0);
  static_assert(PW_EMPTY_ARGS(, ) == 0);
  static_assert(PW_EMPTY_ARGS(a, b, c) == 0);
  static_assert(PW_EMPTY_ARGS(PW_HAS_ARGS) == 0);
  static_assert(PW_EMPTY_ARGS(PW_HAS_ARGS()) == 0);
}

constexpr int TestFunc(int arg, ...) { return arg; }

#define CALL_FUNCTION(arg, ...) TestFunc(arg PW_COMMA_ARGS(__VA_ARGS__))

template <typename T, typename... Args>
constexpr T TemplateArgCount() {
  return sizeof...(Args);
}

#define COUNT_ARGS_TEMPLATE(...) \
  TemplateArgCount<int PW_COMMA_ARGS(__VA_ARGS__)>()

TEST(CommaVarargs, NoArguments) {
  static_assert(TestFunc(0 PW_COMMA_ARGS()) == 0);
  static_assert(TestFunc(1 /* whoa */ PW_COMMA_ARGS(
                    /* this macro */) /* is cool! */) == 1);

  static_assert(TemplateArgCount<int PW_COMMA_ARGS()>() == 0);
  static_assert(TemplateArgCount<int PW_COMMA_ARGS(/* nothing */)>() == 0);

  static_assert(CALL_FUNCTION(2) == 2);
  static_assert(CALL_FUNCTION(3, ) == 3);
  static_assert(CALL_FUNCTION(4, /* nothing */) == 4);

  static_assert(COUNT_ARGS_TEMPLATE() == 0);
  static_assert(COUNT_ARGS_TEMPLATE(/* nothing */) == 0);
}

TEST(CommaVarargs, WithArguments) {
  static_assert(TestFunc(0 PW_COMMA_ARGS(1)) == 0);
  static_assert(TestFunc(1 PW_COMMA_ARGS(1, 2)) == 1);
  static_assert(TestFunc(2 PW_COMMA_ARGS(1, 2, "three")) == 2);

  static_assert(TemplateArgCount<int PW_COMMA_ARGS(bool)>() == 1);
  static_assert(TemplateArgCount<int PW_COMMA_ARGS(char, const char*)>() == 2);
  static_assert(TemplateArgCount<int PW_COMMA_ARGS(int, char, const char*)>() ==
                3);

  static_assert(CALL_FUNCTION(3) == 3);
  static_assert(CALL_FUNCTION(4, ) == 4);
  static_assert(CALL_FUNCTION(5, /* nothing */) == 5);

  static_assert(COUNT_ARGS_TEMPLATE(int) == 1);
  static_assert(COUNT_ARGS_TEMPLATE(int, int) == 2);
  static_assert(COUNT_ARGS_TEMPLATE(int, int, int) == 3);
}

TEST(CommaVarargs, EmptyFinalArgument) {
  static_assert(COUNT_ARGS_TEMPLATE(EMPTY_ARG) == 0);
  static_assert(COUNT_ARGS_TEMPLATE(int, ) == 1);
  static_assert(COUNT_ARGS_TEMPLATE(int, EMPTY_ARG) == 1);
  static_assert(COUNT_ARGS_TEMPLATE(int, /* EMPTY_ARG */) == 1);
  static_assert(COUNT_ARGS_TEMPLATE(int, int, ) == 2);
  static_assert(COUNT_ARGS_TEMPLATE(int, int, int, ) == 3);
  static_assert(COUNT_ARGS_TEMPLATE(int, int, int, EMPTY_ARG) == 3);
}

// This test demonstrates that PW_COMMA_ARGS behaves unexpectedly when it is
// used when invoking another macro. DO NOT use PW_COMMA_ARGS when invoking
// another macro!
#define BAD_DEMO(fmt, ...) _BAD_DEMO_ADD_123(fmt PW_COMMA_ARGS(__VA_ARGS__))

#define _BAD_DEMO_ADD_123(fmt, ...) \
  _BAD_DEMO_CAPTURE_ARGS("%d: " fmt, 123 PW_COMMA_ARGS(__VA_ARGS__))

#define _BAD_DEMO_CAPTURE_ARGS(...) std::make_tuple(__VA_ARGS__)

TEST(CommaVarargs, MisbehavesWithMacroToMacroUse_NoArgs_ArgsAreOkay) {
  auto [a1, a2] = BAD_DEMO("Hello world");
  EXPECT_STREQ(a1, "%d: Hello world");
  EXPECT_EQ(a2, 123);
}

TEST(CommaVarargs, MisbehavesWithMacroToMacroUse_WithArgs_ArgsOutOfOrder) {
  // If there is an additional argument, the order is incorrect! The 123
  // argument should go before the "world?" argument, but it is inserted after.
  // This would be a compilation error if these arguments were passed to printf.
  // What's worse is that this can silently fail if the arguments happen to be
  // compatible types.
  const auto [a1, a2, a3] = BAD_DEMO("Hello %s", "world?");
  EXPECT_STREQ(a1, "%d: Hello %s");
  EXPECT_STREQ(a2, "world?");
  EXPECT_EQ(a3, 123);
}

TEST(CountArgs, Zero) {
  static_assert(PW_MACRO_ARG_COUNT() == 0);
  static_assert(PW_MACRO_ARG_COUNT(/**/) == 0);
  static_assert(PW_MACRO_ARG_COUNT(/* uhm, hi */) == 0);

  // clang-format off
  static_assert(PW_MACRO_ARG_COUNT(     ) == 0);
  static_assert(PW_MACRO_ARG_COUNT(
      ) == 0);
  static_assert(PW_MACRO_ARG_COUNT(
      // wow
      // This is a comment.
      ) == 0);
  // clang-format on
}

TEST(CountArgs, Commas) {
  // clang-format off
  static_assert(PW_MACRO_ARG_COUNT(,) == 2);
  static_assert(PW_MACRO_ARG_COUNT(,,) == 3);
  static_assert(PW_MACRO_ARG_COUNT(,,,) == 4);
  // clang-format on
  static_assert(PW_MACRO_ARG_COUNT(, ) == 2);
  static_assert(PW_MACRO_ARG_COUNT(, , ) == 3);
  static_assert(PW_MACRO_ARG_COUNT(, , , ) == 4);
}

TEST(CountArgs, Parentheses) {
  static_assert(PW_MACRO_ARG_COUNT(()) == 1);
  static_assert(PW_MACRO_ARG_COUNT((1, 2, 3, 4)) == 1);
  static_assert(PW_MACRO_ARG_COUNT((1, 2, 3), (1, 2, 3, 4)) == 2);
  static_assert(PW_MACRO_ARG_COUNT((), ()) == 2);
  static_assert(PW_MACRO_ARG_COUNT((-), (o)) == 2);
  static_assert(PW_MACRO_ARG_COUNT((, , (, , ), ), (123, 4)) == 2);
  static_assert(PW_MACRO_ARG_COUNT(1, (2, 3, 4), (<5, 6>)) == 3);
}

template <typename... Args>
constexpr size_t FunctionArgCount(Args...) {
  return sizeof...(Args);
}

static_assert(FunctionArgCount() == 0);
static_assert(FunctionArgCount(1) == 1);
static_assert(FunctionArgCount(1, 2) == 2);

TEST(CountFunctionArgs, NonEmptyLastArg) {
  static_assert(PW_FUNCTION_ARG_COUNT(a) == 1);
  static_assert(PW_FUNCTION_ARG_COUNT(1, 2) == 2);
  static_assert(PW_FUNCTION_ARG_COUNT(1, 2, 3) == 3);
}

TEST(CountFunctionArgs, EmptyLastArg) {
  static_assert(PW_FUNCTION_ARG_COUNT() == 0);
  static_assert(PW_FUNCTION_ARG_COUNT(a, ) == 1);
  static_assert(PW_FUNCTION_ARG_COUNT(1, 2, ) == 2);
  static_assert(PW_FUNCTION_ARG_COUNT(1, 2, 3, ) == 3);

  static_assert(PW_FUNCTION_ARG_COUNT(a, EMPTY_ARG) == 1);
  static_assert(PW_FUNCTION_ARG_COUNT(1, 2, EMPTY_ARG) == 2);
  static_assert(PW_FUNCTION_ARG_COUNT(1, 2, 3, EMPTY_ARG) == 3);
}

constexpr const char* Value(const char* str = nullptr) { return str; }

TEST(LastArg, NonEmptyLastArg) {
  constexpr const char* last = "last!";
  static_assert(Value(PW_LAST_ARG(last)) == last);
  static_assert(Value(PW_LAST_ARG(1, last)) == last);
  static_assert(Value(PW_LAST_ARG(1, 2, last)) == last);
}

TEST(LastArg, EmptyLastArg) {
  static_assert(Value(PW_LAST_ARG()) == nullptr);
  static_assert(Value(PW_LAST_ARG(1, )) == nullptr);
  static_assert(Value(PW_LAST_ARG(1, 2, )) == nullptr);
  static_assert(Value(PW_LAST_ARG(1, 2, 3, )) == nullptr);
}

TEST(DropLastArg, NonEmptyLastArg) {
  static_assert(FunctionArgCount(PW_DROP_LAST_ARG(1)) == 0);
  static_assert(FunctionArgCount(PW_DROP_LAST_ARG(1, 2)) == 1);
  static_assert(FunctionArgCount(PW_DROP_LAST_ARG(1, 2, 3)) == 2);
}

TEST(DropLastArg, EmptyLastArg) {
  static_assert(FunctionArgCount(PW_DROP_LAST_ARG()) == 0);
  static_assert(FunctionArgCount(PW_DROP_LAST_ARG(1, )) == 1);
  static_assert(FunctionArgCount(PW_DROP_LAST_ARG(1, 2, )) == 2);
  static_assert(FunctionArgCount(PW_DROP_LAST_ARG(1, 2, 3, )) == 3);
}

TEST(DropLastArgIfEmpty, NonEmptyLastArg) {
  static_assert(FunctionArgCount(PW_DROP_LAST_ARG_IF_EMPTY(1)) == 1);
  static_assert(FunctionArgCount(PW_DROP_LAST_ARG_IF_EMPTY(1, 2)) == 2);
  static_assert(FunctionArgCount(PW_DROP_LAST_ARG_IF_EMPTY(1, 2, 3)) == 3);
}

TEST(DropLastArgIfEmpty, EmptyLastArg) {
  static_assert(FunctionArgCount(PW_DROP_LAST_ARG_IF_EMPTY()) == 0);
  static_assert(FunctionArgCount(PW_DROP_LAST_ARG_IF_EMPTY(1, )) == 1);
  static_assert(FunctionArgCount(PW_DROP_LAST_ARG_IF_EMPTY(1, 2, )) == 2);
  static_assert(FunctionArgCount(PW_DROP_LAST_ARG_IF_EMPTY(1, 2, 3, )) == 3);
}

#define SOME_VARIADIC_MACRO(...) PW_MACRO_ARG_COUNT(__VA_ARGS__)

#define ANOTHER_VARIADIC_MACRO(arg, ...) SOME_VARIADIC_MACRO(__VA_ARGS__)

#define ALWAYS_ONE_ARG(...) SOME_VARIADIC_MACRO((__VA_ARGS__))

TEST(CountArgs, NestedMacros) {
  static_assert(SOME_VARIADIC_MACRO() == 0);
  static_assert(SOME_VARIADIC_MACRO(X1) == 1);
  static_assert(SOME_VARIADIC_MACRO(X1, X2) == 2);
  static_assert(SOME_VARIADIC_MACRO(X1, X2, X3) == 3);
  static_assert(SOME_VARIADIC_MACRO(X1, X2, X3, X4) == 4);
  static_assert(SOME_VARIADIC_MACRO(X1, X2, X3, X4, X5) == 5);

  static_assert(ANOTHER_VARIADIC_MACRO() == 0);
  static_assert(ANOTHER_VARIADIC_MACRO(X0) == 0);
  static_assert(ANOTHER_VARIADIC_MACRO(X0, X1) == 1);
  static_assert(ANOTHER_VARIADIC_MACRO(X0, X1, X2) == 2);
  static_assert(ANOTHER_VARIADIC_MACRO(X0, X1, X2, X3) == 3);
  static_assert(ANOTHER_VARIADIC_MACRO(X0, X1, X2, X3, X4) == 4);
  static_assert(ANOTHER_VARIADIC_MACRO(X0, X1, X2, X3, X4, X5) == 5);

  static_assert(ALWAYS_ONE_ARG() == 1);
  static_assert(ALWAYS_ONE_ARG(X0) == 1);
  static_assert(ALWAYS_ONE_ARG(X0, X1) == 1);
  static_assert(ALWAYS_ONE_ARG(X0, X1, X2) == 1);
  static_assert(ALWAYS_ONE_ARG(X0, X1, X2, X3) == 1);
  static_assert(ALWAYS_ONE_ARG(X0, X1, X2, X3, X4) == 1);
  static_assert(ALWAYS_ONE_ARG(X0, X1, X2, X3, X4, X5) == 1);
}

/* Tests all supported arg counts. This test was generated by the following
   Python 3 code:
for i in range(64 + 1):
  args = [f'X{x}' for x in range(1, i + 1)]
  print(f'  static_assert(PW_MACRO_ARG_COUNT({", ".join(args)}) == {i})  ')
*/
TEST(CountArgs, AllSupported) {
  // clang-format off
  static_assert(PW_MACRO_ARG_COUNT() == 0);
  static_assert(PW_MACRO_ARG_COUNT(X1) == 1);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2) == 2);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3) == 3);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4) == 4);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5) == 5);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6) == 6);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7) == 7);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8) == 8);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9) == 9);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10) == 10);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11) == 11);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12) == 12);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13) == 13);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14) == 14);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15) == 15);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16) == 16);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17) == 17);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18) == 18);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19) == 19);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20) == 20);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21) == 21);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22) == 22);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23) == 23);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24) == 24);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25) == 25);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26) == 26);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27) == 27);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28) == 28);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29) == 29);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30) == 30);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31) == 31);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32) == 32);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33) == 33);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34) == 34);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35) == 35);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36) == 36);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37) == 37);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38) == 38);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39) == 39);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40) == 40);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41) == 41);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42) == 42);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43) == 43);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43, X44) == 44);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43, X44, X45) == 45);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43, X44, X45, X46) == 46);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43, X44, X45, X46, X47) == 47);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43, X44, X45, X46, X47, X48) == 48);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43, X44, X45, X46, X47, X48, X49) == 49);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43, X44, X45, X46, X47, X48, X49, X50) == 50);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43, X44, X45, X46, X47, X48, X49, X50, X51) == 51);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43, X44, X45, X46, X47, X48, X49, X50, X51, X52) == 52);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43, X44, X45, X46, X47, X48, X49, X50, X51, X52, X53) == 53);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43, X44, X45, X46, X47, X48, X49, X50, X51, X52, X53, X54) == 54);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43, X44, X45, X46, X47, X48, X49, X50, X51, X52, X53, X54, X55) == 55);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43, X44, X45, X46, X47, X48, X49, X50, X51, X52, X53, X54, X55, X56) == 56);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43, X44, X45, X46, X47, X48, X49, X50, X51, X52, X53, X54, X55, X56, X57) == 57);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43, X44, X45, X46, X47, X48, X49, X50, X51, X52, X53, X54, X55, X56, X57, X58) == 58);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43, X44, X45, X46, X47, X48, X49, X50, X51, X52, X53, X54, X55, X56, X57, X58, X59) == 59);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43, X44, X45, X46, X47, X48, X49, X50, X51, X52, X53, X54, X55, X56, X57, X58, X59, X60) == 60);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43, X44, X45, X46, X47, X48, X49, X50, X51, X52, X53, X54, X55, X56, X57, X58, X59, X60, X61) == 61);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43, X44, X45, X46, X47, X48, X49, X50, X51, X52, X53, X54, X55, X56, X57, X58, X59, X60, X61, X62) == 62);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43, X44, X45, X46, X47, X48, X49, X50, X51, X52, X53, X54, X55, X56, X57, X58, X59, X60, X61, X62, X63) == 63);
  static_assert(PW_MACRO_ARG_COUNT(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, X33, X34, X35, X36, X37, X38, X39, X40, X41, X42, X43, X44, X45, X46, X47, X48, X49, X50, X51, X52, X53, X54, X55, X56, X57, X58, X59, X60, X61, X62, X63, X64) == 64);
  // clang-format on
}

TEST(DelegateByArgCount, WithoutAndWithoutArguments) {
#define TEST_SUM0() (0)
#define TEST_SUM1(a) (a)
#define TEST_SUM2(a, b) ((a) + (b))
#define TEST_SUM3(a, b, c) ((a) + (b) + (c))

  static_assert(PW_DELEGATE_BY_ARG_COUNT(TEST_SUM) == 0);
  static_assert(PW_DELEGATE_BY_ARG_COUNT(TEST_SUM, 5) == 5);
  static_assert(PW_DELEGATE_BY_ARG_COUNT(TEST_SUM, 1, 2) == 3);
  static_assert(PW_DELEGATE_BY_ARG_COUNT(TEST_SUM, 1, 2, 3) == 6);
}

}  // namespace
}  // namespace pw
