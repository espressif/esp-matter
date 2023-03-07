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

#include <array>
#include <cstddef>
#include <string_view>
#include <vector>

#include "gtest/gtest.h"
#include "pw_kvs/internal/span_traits.h"
#include "pw_span/span.h"

namespace pw::kvs {
namespace {

using internal::make_span;

using std::byte;

// Test that the ConvertsToSpan trait correctly idenitifies types that convert
// to span.

// Basic types should not convert to span.
struct Foo {};

static_assert(!ConvertsToSpan<Foo>());
static_assert(!ConvertsToSpan<int>());
static_assert(!ConvertsToSpan<void>());
static_assert(!ConvertsToSpan<byte>());
static_assert(!ConvertsToSpan<byte*>());

// Arrays without an extent are just pointers -- these should not convert.
static_assert(!ConvertsToSpan<bool[]>());
static_assert(!ConvertsToSpan<const int[]>());
static_assert(!ConvertsToSpan<bool (&)[]>());
static_assert(!ConvertsToSpan<const int (&)[]>());
static_assert(!ConvertsToSpan<bool (&&)[]>());
static_assert(!ConvertsToSpan<const int (&&)[]>());

// C arrays convert to span.
static_assert(ConvertsToSpan<std::array<int, 5>>());
static_assert(ConvertsToSpan<decltype("Hello!")>());

static_assert(ConvertsToSpan<bool[1]>());
static_assert(ConvertsToSpan<char[35]>());
static_assert(ConvertsToSpan<const int[35]>());

static_assert(ConvertsToSpan<bool (&)[1]>());
static_assert(ConvertsToSpan<char (&)[35]>());
static_assert(ConvertsToSpan<const int (&)[35]>());

static_assert(ConvertsToSpan<bool (&&)[1]>());
static_assert(ConvertsToSpan<bool (&&)[1]>());
static_assert(ConvertsToSpan<char (&&)[35]>());
static_assert(ConvertsToSpan<const int (&&)[35]>());

// Container types convert to span.
struct FakeContainer {
  const char* data() const { return nullptr; }
  size_t size() const { return 0; }

  const char* begin() const { return nullptr; }
  const char* end() const { return nullptr; }
};

static_assert(ConvertsToSpan<FakeContainer>());

static_assert(ConvertsToSpan<FakeContainer&>());
static_assert(ConvertsToSpan<FakeContainer&&>());
static_assert(ConvertsToSpan<const FakeContainer>());
static_assert(ConvertsToSpan<const FakeContainer&>());
static_assert(ConvertsToSpan<const FakeContainer&&>());

static_assert(ConvertsToSpan<std::string_view>());
static_assert(ConvertsToSpan<std::string_view&>());
static_assert(ConvertsToSpan<std::string_view&&>());

static_assert(ConvertsToSpan<const std::string_view>());
static_assert(ConvertsToSpan<const std::string_view&>());
static_assert(ConvertsToSpan<const std::string_view&&>());

// Spans should also convert to span.
static_assert(ConvertsToSpan<span<int>>());
static_assert(ConvertsToSpan<span<byte>>());
static_assert(ConvertsToSpan<span<const int*>>());
static_assert(ConvertsToSpan<span<bool>&&>());
static_assert(ConvertsToSpan<const span<bool>&>());
static_assert(ConvertsToSpan<span<bool>&&>());

// These tests for the make_span function were copied from Chromium:
// https://chromium.googlesource.com/chromium/src/+/main/base/containers/span_unittest.cc

TEST(SpanTest, MakeSpanFromDataAndSize) {
  int* nullint = nullptr;
  auto empty_span = make_span(nullint, 0);
  EXPECT_TRUE(empty_span.empty());
  EXPECT_EQ(nullptr, empty_span.data());
  std::vector<int> vector = {1, 1, 2, 3, 5, 8};
  span<int> expected_span(vector.data(), vector.size());
  auto made_span = make_span(vector.data(), vector.size());
  EXPECT_EQ(expected_span.data(), made_span.data());
  EXPECT_EQ(expected_span.size(), made_span.size());
  static_assert(decltype(made_span)::extent == dynamic_extent);
  static_assert(
      std::is_same<decltype(expected_span), decltype(made_span)>::value,
      "the type of made_span differs from expected_span!");
}

TEST(SpanTest, MakeSpanFromPointerPair) {
  int* nullint = nullptr;
  auto empty_span = make_span(nullint, nullint);
  EXPECT_TRUE(empty_span.empty());
  EXPECT_EQ(nullptr, empty_span.data());
  std::vector<int> vector = {1, 1, 2, 3, 5, 8};
  span<int> expected_span(vector.data(), vector.size());
  auto made_span = make_span(vector.data(), vector.data() + vector.size());
  EXPECT_EQ(expected_span.data(), made_span.data());
  EXPECT_EQ(expected_span.size(), made_span.size());
  static_assert(decltype(made_span)::extent == dynamic_extent);
  static_assert(
      std::is_same<decltype(expected_span), decltype(made_span)>::value,
      "the type of made_span differs from expected_span!");
}

TEST(SpanTest, MakeSpanFromConstexprArray) {
  static constexpr int kArray[] = {1, 2, 3, 4, 5};
  constexpr span<const int, 5> expected_span(kArray);
  constexpr auto made_span = make_span(kArray);
  EXPECT_EQ(expected_span.data(), made_span.data());
  EXPECT_EQ(expected_span.size(), made_span.size());
  static_assert(decltype(made_span)::extent == 5);
  static_assert(
      std::is_same<decltype(expected_span), decltype(made_span)>::value,
      "the type of made_span differs from expected_span!");
}

TEST(SpanTest, MakeSpanFromStdArray) {
  const std::array<int, 5> kArray = {{1, 2, 3, 4, 5}};
  span<const int, 5> expected_span(kArray);
  auto made_span = make_span(kArray);
  EXPECT_EQ(expected_span.data(), made_span.data());
  EXPECT_EQ(expected_span.size(), made_span.size());
  static_assert(decltype(made_span)::extent == 5);
  static_assert(
      std::is_same<decltype(expected_span), decltype(made_span)>::value,
      "the type of made_span differs from expected_span!");
}

TEST(SpanTest, MakeSpanFromConstContainer) {
  const std::vector<int> vector = {-1, -2, -3, -4, -5};
  span<const int> expected_span(vector);
  auto made_span = make_span(vector);
  EXPECT_EQ(expected_span.data(), made_span.data());
  EXPECT_EQ(expected_span.size(), made_span.size());
  static_assert(decltype(made_span)::extent == dynamic_extent);
  static_assert(
      std::is_same<decltype(expected_span), decltype(made_span)>::value,
      "the type of made_span differs from expected_span!");
}

#if 0  // Not currently working with fixed extent spans.

TEST(SpanTest, MakeStaticSpanFromConstContainer) {
  const std::vector<int> vector = {-1, -2, -3, -4, -5};
  span<const int, 5> expected_span(vector.data(), vector.size());
  auto made_span = make_span<5>(vector);
  EXPECT_EQ(expected_span.data(), made_span.data());
  EXPECT_EQ(expected_span.size(), made_span.size());
  static_assert(decltype(made_span)::extent == 5);
  static_assert(
      std::is_same<decltype(expected_span), decltype(made_span)>::value,
      "the type of made_span differs from expected_span!");
}

#endif  // 0

TEST(SpanTest, MakeSpanFromContainer) {
  std::vector<int> vector = {-1, -2, -3, -4, -5};
  span<int> expected_span(vector);
  auto made_span = make_span(vector);
  EXPECT_EQ(expected_span.data(), made_span.data());
  EXPECT_EQ(expected_span.size(), made_span.size());
  static_assert(decltype(made_span)::extent == dynamic_extent);
  static_assert(
      std::is_same<decltype(expected_span), decltype(made_span)>::value,
      "the type of made_span differs from expected_span!");
}

#if 0  // Not currently working with fixed extent spans.

TEST(SpanTest, MakeStaticSpanFromContainer) {
  std::vector<int> vector = {-1, -2, -3, -4, -5};
  span<int, 5> expected_span(vector.data(), vector.size());
  auto made_span = make_span<5>(vector);
  EXPECT_EQ(expected_span.data(), make_span<5>(vector).data());
  EXPECT_EQ(expected_span.size(), make_span<5>(vector).size());
  static_assert(decltype(make_span<5>(vector))::extent == 5);
  static_assert(
      std::is_same<decltype(expected_span), decltype(made_span)>::value,
      "the type of made_span differs from expected_span!");
}

TEST(SpanTest, MakeStaticSpanFromConstexprContainer) {
  constexpr StringPiece str = "Hello, World";
  constexpr auto made_span = make_span<12>(str);
  static_assert(str.data() == made_span.data(), "Error: data() does not match");
  static_assert(str.size() == made_span.size(), "Error: size() does not match");
  static_assert(std::is_same<decltype(str)::value_type,
                             decltype(made_span)::value_type>::value,
                "Error: value_type does not match");
  static_assert(str.size() == decltype(made_span)::extent,
                "Error: extent does not match");
}

#endif  // 0

TEST(SpanTest, MakeSpanFromRValueContainer) {
  std::vector<int> vector = {-1, -2, -3, -4, -5};
  span<const int> expected_span(vector);
  // Note: While static_cast<T&&>(foo) is effectively just a fancy spelling of
  // std::move(foo), make_span does not actually take ownership of the passed in
  // container. Writing it this way makes it more obvious that we simply care
  // about the right behavour when passing rvalues.
  auto made_span = make_span(static_cast<std::vector<int>&&>(vector));
  EXPECT_EQ(expected_span.data(), made_span.data());
  EXPECT_EQ(expected_span.size(), made_span.size());
  static_assert(decltype(made_span)::extent == dynamic_extent);
  static_assert(
      std::is_same<decltype(expected_span), decltype(made_span)>::value,
      "the type of made_span differs from expected_span!");
}

#if 0  // Not currently working with fixed extent spans.

TEST(SpanTest, MakeStaticSpanFromRValueContainer) {
  std::vector<int> vector = {-1, -2, -3, -4, -5};
  span<const int, 5> expected_span(vector.data(), vector.size());
  // Note: While static_cast<T&&>(foo) is effectively just a fancy spelling of
  // std::move(foo), make_span does not actually take ownership of the passed in
  // container. Writing it this way makes it more obvious that we simply care
  // about the right behavour when passing rvalues.
  auto made_span = make_span<5>(static_cast<std::vector<int>&&>(vector));
  EXPECT_EQ(expected_span.data(), made_span.data());
  EXPECT_EQ(expected_span.size(), made_span.size());
  static_assert(decltype(made_span)::extent == 5);
  static_assert(
      std::is_same<decltype(expected_span), decltype(made_span)>::value,
      "the type of made_span differs from expected_span!");
}

#endif  // 0

TEST(SpanTest, MakeSpanFromDynamicSpan) {
  static constexpr int kArray[] = {1, 2, 3, 4, 5};
  constexpr span<const int> expected_span(kArray);
  constexpr auto made_span = make_span(expected_span);
  static_assert(std::is_same<decltype(expected_span)::element_type,
                             decltype(made_span)::element_type>::value,
                "make_span(span) should have the same element_type as span");
  static_assert(expected_span.data() == made_span.data(),
                "make_span(span) should have the same data() as span");
  static_assert(expected_span.size() == made_span.size(),
                "make_span(span) should have the same size() as span");
  static_assert(decltype(made_span)::extent == decltype(expected_span)::extent,
                "make_span(span) should have the same extent as span");
  static_assert(
      std::is_same<decltype(expected_span), decltype(made_span)>::value,
      "the type of made_span differs from expected_span!");
}

TEST(SpanTest, MakeSpanFromStaticSpan) {
  static constexpr int kArray[] = {1, 2, 3, 4, 5};
  constexpr span<const int, 5> expected_span(kArray);
  constexpr auto made_span = make_span(expected_span);
  static_assert(std::is_same<decltype(expected_span)::element_type,
                             decltype(made_span)::element_type>::value,
                "make_span(span) should have the same element_type as span");
  static_assert(expected_span.data() == made_span.data(),
                "make_span(span) should have the same data() as span");
  static_assert(expected_span.size() == made_span.size(),
                "make_span(span) should have the same size() as span");
  static_assert(decltype(made_span)::extent == decltype(expected_span)::extent,
                "make_span(span) should have the same extent as span");
  static_assert(
      std::is_same<decltype(expected_span), decltype(made_span)>::value,
      "the type of made_span differs from expected_span!");
}

}  // namespace
}  // namespace pw::kvs
