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

// Include all of the provided headers, even if they aren't tested.
#include <algorithm>
#include <array>
#include <cinttypes>
#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <new>
#include <string_view>
#include <type_traits>
#include <utility>

#include "pw_polyfill/standard.h"
#include "pw_preprocessor/compiler.h"

namespace {

// In order to test this file without dependencies on the C++ standard library,
// this file needs to be fully isolated from the regular Pigweed testing
// infrastructure. pw_unit_test's dependencies do compile with the C++ standard
// library, which could conflict with pw_minimal_cpp_stdlib.
//
// SimpleTest provides the basic features of pw_unit_test without dependencies.
class SimpleTest {
 public:
  virtual ~SimpleTest() = default;

  static bool RunAllTests() {
    for (SimpleTest* test = all_tests; test != nullptr; test = test->next_) {
      test->Run();
      if (!test->passed_) {
        return false;
      }
    }
    return true;
  }

 protected:
  SimpleTest() : next_(all_tests) { all_tests = this; }

  void RecordTestFailure() { passed_ = false; }

 private:
  virtual void Run() = 0;

  static SimpleTest* all_tests;

  bool passed_ = true;
  SimpleTest* next_;
};

SimpleTest* SimpleTest::all_tests = nullptr;

#define EXPECT_EQ(lhs, rhs) \
  do {                      \
    if ((lhs) != (rhs)) {   \
      RecordTestFailure();  \
    }                       \
  } while (0)

#define EXPECT_TRUE(expr) EXPECT_EQ(true, expr)
#define EXPECT_FALSE(expr) EXPECT_EQ(false, expr)
#define EXPECT_STREQ(lhs, rhs) EXPECT_EQ(std::strcmp((lhs), (rhs)), 0)

#define TEST(suite, name)                                 \
  class SimpleTest_##suite##_##name : public SimpleTest { \
    void Run() override;                                  \
  } test_##suite##_##name;                                \
                                                          \
  void SimpleTest_##suite##_##name::Run()

TEST(Algorithm, Basic) {
  static_assert(std::min(1, 2) == 1);
  static_assert(std::max(1, 2) == 2);

  EXPECT_EQ(std::forward<int>(2), 2);
}

TEST(Algorithm, Copy) {
  constexpr size_t kCopyOffset = 1;
  std::array<int, 3> foo{3, 2, 1};
  std::array<int, 5> bar{0};

  // Ensure zero-element iterator doesn't modify the destination object when
  // copied.
  int temp = foo[0];
  std::copy(foo.end(), foo.end(), bar.begin());
  EXPECT_EQ(foo[0], temp);

  // Copy a single element.
  std::array<int, 1> one{-101};
  std::copy(one.begin(), one.end(), foo.begin());
  EXPECT_EQ(foo[0], -101);

  auto copy_end = std::copy(foo.begin(), foo.end(), bar.begin() + kCopyOffset);
  // Verify the iterator points to the end of the copied region.
  EXPECT_EQ(copy_end, bar.begin() + foo.size() + kCopyOffset);

  // Verify all the values were properly copied from foo to bar.
  {
    size_t i = 0;
    for (auto it = bar.begin() + kCopyOffset; it != copy_end; ++it) {
      EXPECT_EQ(*it, foo[i++]);
    }
  }
}

TEST(Algorithm, Find) {
  std::array<int, 5> foo{3, 2, 1, 42, 17};
  // Ensure a value in the middle of the array is properly found.
  EXPECT_EQ(*std::find(std::begin(foo), std::end(foo), 42), 42);

  // Ensure the iterator returned by find() matches the expected location of the
  // element.
  EXPECT_EQ(std::find(std::begin(foo), std::end(foo), 42), std::begin(foo) + 3);

  // Ensure an element at the beginning of an array is found.
  EXPECT_EQ(*std::find(std::begin(foo), std::end(foo), 3), foo[0]);

  // Ensure an element at the end of an array is found.
  EXPECT_EQ(*std::find(std::begin(foo), std::end(foo), 17),
            foo[foo.size() - 1]);
}

TEST(Algorithm, NotFound) {
  std::array<int, 3> foo{3, 2, 1};

  // Ensure that if an element is not found, an iterator matching foo.end() is
  // returned.
  EXPECT_EQ(std::find(std::begin(foo), std::end(foo), -99), std::end(foo));

  // Ensure that a zero-element iterator range returns the end iterator passed
  // to std::find().
  EXPECT_EQ(std::find(std::end(foo), std::end(foo), 3), std::end(foo));
}

TEST(Array, Basic) {
  constexpr std::array<int, 4> array{0, 1, 2, 3};

  static_assert(array[2] == 2);

  for (int i = 0; i < static_cast<int>(array.size()); ++i) {
    EXPECT_EQ(i, array[i]);
  }
}

TEST(Cmath, Basic) PW_NO_SANITIZE("float-divide-by-zero") {
  EXPECT_EQ(std::abs(-1), 1);
  EXPECT_EQ(std::abs(1), 1);

  // Although Clang/LLVM do not fully support __STDC_IEC_559__, they do have the
  // necessary IEEE 754 support for floating point division by zero.
  EXPECT_TRUE(std::isfinite(1.0));
  EXPECT_FALSE(std::isfinite(1.0 / 0.0));

  EXPECT_FALSE(std::isnan(1.0));
  EXPECT_TRUE(std::isnan(0.0 / 0.0));

  EXPECT_FALSE(std::signbit(1.0));
  EXPECT_TRUE(std::signbit(-1.0));
}

TEST(Cstddef, Basic) {
  using std::byte;
  byte foo = byte{12};
  EXPECT_EQ(foo, byte{12});
}

TEST(Iterator, Basic) {
  std::array<int, 3> foo{3, 2, 1};

  EXPECT_EQ(std::data(foo), foo.data());
  EXPECT_EQ(std::size(foo), foo.size());

  EXPECT_EQ(*std::begin(foo), foo[0]);
  EXPECT_EQ(std::end(foo), std::begin(foo) + foo.size());

  foo.fill(99);
  EXPECT_EQ(foo[0], 99);
  EXPECT_EQ(foo[1], 99);
  EXPECT_EQ(foo[2], 99);
}

template <typename T>
int SumFromInitializerList(std::initializer_list<T> values) {
  int sum = 0;
  for (auto value : values) {
    sum += value;
  }
  return sum;
}
TEST(InitializerList, Empty) {
  std::initializer_list<int> mt;
  EXPECT_EQ(0, SumFromInitializerList(mt));

  EXPECT_EQ(0, SumFromInitializerList<float>({}));
}

TEST(InitializerList, Declared) {
  std::initializer_list<char> list{'\3', '\3', '\4'};
  EXPECT_EQ(10, SumFromInitializerList(list));
}

TEST(InitializerList, Inline) {
  EXPECT_EQ(42, SumFromInitializerList<long>({42}));
  EXPECT_EQ(2, SumFromInitializerList<bool>({true, false, true}));
  EXPECT_EQ(15, SumFromInitializerList({1, 2, 3, 4, 5}));
}

TEST(Limits, Basic) {
  static_assert(std::numeric_limits<unsigned char>::is_specialized);
  static_assert(std::numeric_limits<unsigned char>::is_integer);
  static_assert(std::numeric_limits<unsigned char>::min() == 0u);
  static_assert(std::numeric_limits<unsigned char>::max() == 255u);

  static_assert(std::numeric_limits<signed char>::is_specialized);
  static_assert(std::numeric_limits<signed char>::is_integer);
  static_assert(std::numeric_limits<signed char>::min() == -128);
  static_assert(std::numeric_limits<signed char>::max() == 127);

  // Assume 64-bit long long
  static_assert(std::numeric_limits<long long>::is_specialized);
  static_assert(std::numeric_limits<long long>::is_integer);
  static_assert(std::numeric_limits<long long>::min() ==
                (-9223372036854775807ll - 1));
  static_assert(std::numeric_limits<long long>::max() == 9223372036854775807ll);

  static_assert(std::numeric_limits<unsigned long long>::is_specialized);
  static_assert(std::numeric_limits<unsigned long long>::is_integer);
  static_assert(std::numeric_limits<unsigned long long>::min() == 0u);
  static_assert(std::numeric_limits<unsigned long long>::max() ==
                18446744073709551615ull);
}

TEST(New, PlacementNew) {
  alignas(sizeof(int)) unsigned char value[sizeof(int)];
  new (value) int(1234);

  int int_value;
  std::memcpy(&int_value, value, sizeof(int_value));
  EXPECT_EQ(1234, int_value);
}

TEST(New, Launder) {
  unsigned char value[4];
  int* int_ptr = std::launder(reinterpret_cast<int*>(value));
  EXPECT_EQ(static_cast<void*>(int_ptr), static_cast<void*>(value));
}

TEST(StringView, Basic) {
  constexpr std::string_view value("1234567890");
  static_assert(value.size() == 10);
  static_assert(value[1] == '2');

  char buffer[] = "!!!!!";
  constexpr size_t buffer_size = sizeof(buffer) - 1;  // always keep the \0

  value.copy(buffer, buffer_size, 10);
  EXPECT_STREQ(buffer, "!!!!!");

  value.copy(buffer, buffer_size, 9);
  EXPECT_STREQ(buffer, "0!!!!");

  value.copy(buffer, buffer_size, 2);
  EXPECT_STREQ(buffer, "34567");

  value.copy(buffer, buffer_size);
  EXPECT_STREQ(buffer, "12345");
}

TEST(TypeTraits, Basic) {
  static_assert(std::is_integral_v<bool>);
  static_assert(!std::is_integral_v<float>);

  static_assert(std::is_floating_point_v<float>);
  static_assert(!std::is_floating_point_v<bool>);

  static_assert(std::is_same_v<float, float>);
  static_assert(!std::is_same_v<char, unsigned char>);
}

TEST(TypeTraits, LogicalTraits) {
  static_assert(std::conjunction_v<>);
  static_assert(!std::conjunction_v<std::false_type>);
  static_assert(std::conjunction_v<std::true_type>);
  static_assert(!std::conjunction_v<std::false_type, std::true_type>);
  static_assert(std::conjunction_v<std::true_type, std::true_type>);
  static_assert(!std::conjunction_v<std::false_type, std::false_type>);

  static_assert(!std::disjunction_v<>);
  static_assert(!std::disjunction_v<std::false_type>);
  static_assert(std::disjunction_v<std::true_type>);
  static_assert(std::disjunction_v<std::false_type, std::true_type>);
  static_assert(std::disjunction_v<std::true_type, std::true_type>);
  static_assert(!std::disjunction_v<std::false_type, std::false_type>);

  static_assert(std::negation_v<std::false_type>);
  static_assert(!std::negation_v<std::true_type>);
}

struct MoveTester {
  MoveTester(int value) : magic_value(value), moved(false) {}

  MoveTester(const MoveTester&) = default;

  MoveTester(MoveTester&& other) : magic_value(other.magic_value), moved(true) {
    other.magic_value = 0xffff;
  }

  int magic_value;
  bool moved;
};

TEST(Utility, Move) {
  MoveTester test(123);

  MoveTester copied(test);
  EXPECT_EQ(copied.magic_value, 123);
  EXPECT_FALSE(copied.moved);

  MoveTester moved(std::move(copied));
  EXPECT_EQ(123, moved.magic_value);
  // NOLINTNEXTLINE(bugprone-use-after-move)
  EXPECT_EQ(0xffff, copied.magic_value);
  EXPECT_TRUE(moved.moved);
}

TEST(Utility, MakeIntegerSequence) {
  static_assert(std::is_same_v<std::make_integer_sequence<int, 0>,
                               std::integer_sequence<int>>);
  static_assert(std::is_same_v<std::make_integer_sequence<int, 1>,
                               std::integer_sequence<int, 0>>);
  static_assert(std::is_same_v<std::make_integer_sequence<int, 3>,
                               std::integer_sequence<int, 0, 1, 2>>);

  static_assert(std::is_same_v<std::make_index_sequence<0>,
                               std::integer_sequence<size_t>>);
  static_assert(std::is_same_v<std::make_index_sequence<1>,
                               std::integer_sequence<size_t, 0>>);
  static_assert(std::is_same_v<std::make_index_sequence<3>,
                               std::integer_sequence<size_t, 0, 1, 2>>);
}

TEST(Iterator, Tags) {
  static_assert(std::is_convertible_v<std::forward_iterator_tag,
                                      std::input_iterator_tag>);

  static_assert(std::is_convertible_v<std::bidirectional_iterator_tag,
                                      std::input_iterator_tag>);
  static_assert(std::is_convertible_v<std::bidirectional_iterator_tag,
                                      std::forward_iterator_tag>);

  static_assert(std::is_convertible_v<std::random_access_iterator_tag,
                                      std::input_iterator_tag>);
  static_assert(std::is_convertible_v<std::random_access_iterator_tag,
                                      std::forward_iterator_tag>);
  static_assert(std::is_convertible_v<std::random_access_iterator_tag,
                                      std::bidirectional_iterator_tag>);

#if PW_CXX_STANDARD_IS_SUPPORTED(20)
  static_assert(std::is_convertible_v<std::contiguous_iterator_tag,
                                      std::input_iterator_tag>);
  static_assert(std::is_convertible_v<std::contiguous_iterator_tag,
                                      std::forward_iterator_tag>);
  static_assert(std::is_convertible_v<std::contiguous_iterator_tag,
                                      std::bidirectional_iterator_tag>);
  static_assert(std::is_convertible_v<std::contiguous_iterator_tag,
                                      std::random_access_iterator_tag>);
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(20)
}

TEST(TypeTrait, Basic) {
  static_assert(std::is_same_v<const int, std::add_const_t<int>>);
  static_assert(std::is_same_v<const int, std::add_const_t<const int>>);
  static_assert(!std::is_same_v<int, std::add_const_t<int>>);
}

}  // namespace

namespace pw::minimal_cpp_stdlib {

bool RunAllTests() { return SimpleTest::RunAllTests(); }

}  // namespace pw::minimal_cpp_stdlib
