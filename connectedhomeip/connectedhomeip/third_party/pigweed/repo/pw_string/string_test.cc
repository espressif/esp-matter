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

#include "pw_string/string.h"

#include <cstddef>
#include <iterator>
#include <type_traits>

#include "gtest/gtest.h"
#include "pw_compilation_testing/negative_compilation.h"
#include "pw_polyfill/standard.h"

namespace pw {

#if PW_CXX_STANDARD_IS_SUPPORTED(17)  // std::string_view is a C++17 feature

using namespace std::string_view_literals;

template <typename T>
class StringViewLike {
 public:
  constexpr StringViewLike(const T* data, size_t size) : value_(data, size) {}

  constexpr operator std::basic_string_view<T>() const { return value_; }

 private:
  std::basic_string_view<T> value_;
};

// The StringView overload ignores types that convert to const T* to avoid
// ambiguity with the existing const T* overload.
template <typename T>
class StringViewLikeButConvertsToPointer : public StringViewLike<T> {
 public:
  using StringViewLike<T>::StringViewLike;

  constexpr operator std::basic_string_view<T>() const { return value_; }
  constexpr operator const T*() const { return value_.data(); }

 private:
  std::basic_string_view<T> value_;
};

#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

template <typename T>
class EvenNumberIterator {
 public:
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using pointer = const T*;
  using reference = const T&;
  using iterator_category = std::input_iterator_tag;

  // Rounds down to nearest even.
  explicit constexpr EvenNumberIterator(value_type value)
      : value_(static_cast<value_type>(value & ~static_cast<value_type>(1))) {}

  constexpr EvenNumberIterator& operator++() {
    value_ += 2;
    return *this;
  }

  constexpr const T& operator*() const { return value_; }

  constexpr bool operator==(const EvenNumberIterator& rhs) const {
    return value_ == rhs.value_;
  }

  constexpr bool operator!=(const EvenNumberIterator& rhs) const {
    return value_ != rhs.value_;
  }

 private:
  value_type value_;
};

#ifdef __cpp_deduction_guides

TEST(InlineString, DeduceBasicString_Char) {
  InlineBasicString string_10("1234567890");
  static_assert(std::is_same_v<decltype(string_10), InlineString<10>>);

  InlineBasicString string_3 = "abc";
  static_assert(std::is_same_v<decltype(string_3), InlineString<3>>);

  string_10.resize(6);
  EXPECT_STREQ(
      string_10.append(string_3).append(InlineBasicString("?")).c_str(),
      "123456abc?");
}

TEST(InlineString, DeduceBasicString_Int) {
  constexpr long kLongArray[4] = {0, 1, 2, 0};
  InlineBasicString string_3 = kLongArray;
  static_assert(std::is_same_v<decltype(string_3), InlineBasicString<long, 3>>);

  EXPECT_EQ(string_3, InlineBasicString(kLongArray));
}

// Test CTAD on the InlineString alias, if supported.
#if __cpp_deduction_guides >= 201907L

TEST(InlineString, DeduceString) {
  InlineString string("123456789");
  static_assert(std::is_same_v<decltype(string), InlineString<10>>);

  EXPECT_STREQ("123456789", string.c_str());
}

#endif  // __cpp_deduction_guides >= 201907L
#endif  // __cpp_deduction_guides

template <typename T, size_t kExpectedSize>
constexpr bool TestEqual(const T* string, const T (&expected)[kExpectedSize]) {
  for (size_t i = 0; i < kExpectedSize; ++i) {
    if (string[i] != expected[i]) {
      return false;
    }
  }
  return true;
}

// Compares a pw::InlineBasicString to a null-terminated array of characters.
#define EXPECT_PW_STRING(pw_string, array)                               \
  ASSERT_EQ(pw_string.size(), sizeof(array) / sizeof(array[0]) - 1);     \
  ASSERT_EQ(pw_string.c_str()[pw_string.size()], decltype(array[0]){0}); \
  EXPECT_TRUE(TestEqual(pw_string.c_str(), array))

#define EXPECT_CONSTEXPR_PW_STRING(string, array)                       \
  static_assert(string.size() == sizeof(array) / sizeof(array[0]) - 1,  \
                #string ".size() == sizeof(" #array ") - 1 FAILED");    \
  static_assert(string.c_str()[string.size()] == decltype(array[0]){0}, \
                #string " must be null terminated");                    \
  static_assert(TestEqual(string.c_str(), array),                       \
                #string " == " #array " FAILED")

// This macro performs operations on a string and checks the result.
//
//   1. Declare a string variable named fixed_str using the provided
//      initialization statement. fixed_str can be used in tests that
//      specifically want a known-capacity string.
//   2. Declare a generic-capacity generic_str reference for use in tests that
//      specifically want to use a generic-capacity string.
//   3. Declare a str reference for use in tests. It is fixed-capacity in
//      constexpr tests and known-capacity in runtime tests.
//   4. Execute the provided statements.
//   5. Check that str equals the provided string literal.
//
// The test is executed twice:
//   - At compile time (with constexpr and static_assert) on a known-length
//     string (InlineString<kLength>).
//   - At runtime a generic reference (InlineString<>&) at runtime.
#if __cpp_constexpr >= 201603L  // constexpr lambdas are required
#define TEST_CONSTEXPR_STRING(create_string, statements, expected) \
  do {                                                             \
    constexpr auto constexpr_str = [] {                            \
      [[maybe_unused]] auto fixed_str = create_string;             \
      [[maybe_unused]] auto& str = fixed_str;                      \
      [[maybe_unused]] auto& generic_str = Generic(fixed_str);     \
      statements;                                                  \
      return str;                                                  \
    }();                                                           \
    EXPECT_CONSTEXPR_PW_STRING(constexpr_str, expected);           \
  } while (0)
#else  // Skip constexpr tests in C++14.
#define TEST_CONSTEXPR_STRING(create_string, statements, expected) \
  do {                                                             \
    constexpr auto str = create_string;                            \
    EXPECT_NE(str.data(), nullptr);                                \
  } while (0)
#endif  //__cpp_constexpr >= 201603L

#define TEST_RUNTIME_STRING(create_string, statements, expected) \
  do {                                                           \
    [[maybe_unused]] auto fixed_str = create_string;             \
    [[maybe_unused]] auto& generic_str = Generic(fixed_str);     \
    [[maybe_unused]] auto& str = generic_str;                    \
    statements;                                                  \
    EXPECT_PW_STRING(str, expected);                             \
  } while (0)

#define TEST_STRING(create_string, statements, expected)      \
  TEST_CONSTEXPR_STRING(create_string, statements, expected); \
  TEST_RUNTIME_STRING(create_string, statements, expected)

// Casts any pw::InlineString to a generic (runtime-capacity) reference.
template <typename T>
constexpr const InlineBasicString<T>& Generic(const InlineBasicString<T>& str) {
  return str;
}

template <typename T>
constexpr InlineBasicString<T>& Generic(InlineBasicString<T>& str) {
  return str;
}

constexpr InlineString<0> kEmptyCapacity0;
constexpr InlineString<10> kEmptyCapacity10;

constexpr InlineString<10> kSize5Capacity10 = "12345";
constexpr InlineString<10> kSize10Capacity10("1234567890", 10);

constexpr const char* kPointer0 = "";
constexpr const char* kPointer10 = "9876543210";

constexpr const char kArrayNull1[1] = {'\0'};
constexpr const char kArray5[5] = {'1', '2', '3', '4', '\0'};

// Invalid, non-terminated arrays used in negative compilation tests.
[[maybe_unused]] constexpr const char kArrayNonNull1[1] = {'?'};
[[maybe_unused]] constexpr const char kArrayNonNull5[5] = {
    '1', '2', '3', '4', '5'};

constexpr EvenNumberIterator<char> kEvenNumbers0(0);
constexpr EvenNumberIterator<char> kEvenNumbers2(2);
constexpr EvenNumberIterator<char> kEvenNumbers8(8);

#if PW_CXX_STANDARD_IS_SUPPORTED(17)  // std::string_view is a C++17 feature

constexpr std::string_view kView0;
constexpr std::string_view kView5 = "12345"sv;
constexpr std::string_view kView10 = "1234567890"sv;

constexpr StringViewLike<char> kStringViewLike10("0123456789", 10);

#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

//
// Construction and assignment
//

// Constructor

TEST(InlineString, Construct_Default) {
  constexpr InlineString<0> kEmpty0;
  static_assert(kEmpty0.empty(), "Must be empty");
  static_assert(kEmpty0.c_str()[0] == '\0', "Must be null terminated");

  constexpr InlineString<10> kEmpty10;
  static_assert(kEmpty10.empty(), "Must be empty");
  static_assert(kEmpty10.c_str()[0] == '\0', "Must be null terminated");
}

TEST(InlineString, Construct_Characters) {
  TEST_STRING(InlineString<0>(0, 'a'), , "");
  TEST_STRING(InlineString<10>(0, 'a'), , "");

  TEST_STRING(InlineString<1>(1, 'a'), , "a");
  TEST_STRING(InlineString<10>(1, 'a'), , "a");

  TEST_STRING(InlineString<10>(10, 'a'), , "aaaaaaaaaa");

  TEST_STRING(InlineString<10>(0, '\0'), , "");
  TEST_STRING(InlineString<10>(1, '\0'), , "\0");
  TEST_STRING(InlineString<10>(5, '\0'), , "\0\0\0\0\0");

#if PW_NC_TEST(Construct_Char_TooMany_0)
  PW_NC_EXPECT("PW_ASSERT\(new_size <= max_size\(\)\)");
  [[maybe_unused]] constexpr InlineString<0> too_large(1, 'A');
#elif PW_NC_TEST(Construct_Char_TooMany_10)
  PW_NC_EXPECT("PW_ASSERT\(new_size <= max_size\(\)\)");
  [[maybe_unused]] constexpr InlineString<10> too_large(11, 'A');
#endif  // PW_NC_TEST
}
// NOLINTNEXTLINE(google-readability-function-size)
TEST(InlineString, Construct_Substr) {
  TEST_STRING(InlineString<10>(kEmptyCapacity0, 0), , "");
  TEST_STRING(InlineString<10>(kEmptyCapacity0, 0, 0), , "");
  TEST_STRING(InlineString<10>(Generic(kEmptyCapacity0), 0), , "");
  TEST_STRING(InlineString<10>(Generic(kEmptyCapacity0), 0, 0), , "");

  TEST_STRING(InlineString<0>(kEmptyCapacity10, 0), , "");
  TEST_STRING(InlineString<0>(kEmptyCapacity10, 0, 0), , "");
  TEST_RUNTIME_STRING(InlineString<0>(Generic(kEmptyCapacity10), 0), , "");
  TEST_RUNTIME_STRING(InlineString<0>(Generic(kEmptyCapacity10), 0, 0), , "");

  TEST_STRING(InlineString<10>(kSize5Capacity10, 0), , "12345");
  TEST_RUNTIME_STRING(
      InlineString<10>(Generic(kSize5Capacity10), 0), , "12345");
  TEST_STRING(InlineString<10>(kSize5Capacity10, 1), , "2345");
  TEST_RUNTIME_STRING(InlineString<10>(Generic(kSize5Capacity10), 1), , "2345");
  TEST_STRING(InlineString<10>(kSize5Capacity10, 4), , "5");
  TEST_RUNTIME_STRING(InlineString<10>(Generic(kSize5Capacity10), 4), , "5");
  TEST_STRING(InlineString<10>(kSize5Capacity10, 5), , "");
  TEST_RUNTIME_STRING(InlineString<10>(Generic(kSize5Capacity10), 5), , "");

  TEST_STRING(InlineString<10>(kSize5Capacity10, 0, 0), , "");
  TEST_RUNTIME_STRING(InlineString<10>(Generic(kSize5Capacity10), 0, 0), , "");
  TEST_STRING(InlineString<10>(kSize5Capacity10, 0, 1), , "1");
  TEST_RUNTIME_STRING(InlineString<10>(Generic(kSize5Capacity10), 0, 1), , "1");
  TEST_STRING(InlineString<10>(kSize5Capacity10, 1, 0), , "");
  TEST_RUNTIME_STRING(InlineString<10>(Generic(kSize5Capacity10), 1, 0), , "");
  TEST_STRING(InlineString<10>(kSize5Capacity10, 1, 1), , "2");
  TEST_RUNTIME_STRING(InlineString<10>(Generic(kSize5Capacity10), 1, 1), , "2");
  TEST_STRING(InlineString<10>(kSize5Capacity10, 1, 4), , "2345");
  TEST_RUNTIME_STRING(
      InlineString<10>(Generic(kSize5Capacity10), 1, 4), , "2345");
  TEST_STRING(InlineString<10>(kSize5Capacity10, 1, 5), , "2345");
  TEST_RUNTIME_STRING(
      InlineString<10>(Generic(kSize5Capacity10), 1, 4), , "2345");
  TEST_STRING(InlineString<10>(kSize5Capacity10, 1, 9000), , "2345");
  TEST_RUNTIME_STRING(
      InlineString<10>(Generic(kSize5Capacity10), 1, 9000), , "2345");

  TEST_STRING(InlineString<10>(kSize5Capacity10, 4, 9000), , "5");
  TEST_RUNTIME_STRING(
      InlineString<10>(Generic(kSize5Capacity10), 4, 9000), , "5");

  TEST_STRING(InlineString<10>(kSize5Capacity10, 5, 0), , "");
  TEST_RUNTIME_STRING(InlineString<10>(Generic(kSize5Capacity10), 5, 0), , "");
  TEST_STRING(InlineString<10>(kSize5Capacity10, 5, 1), , "");
  TEST_RUNTIME_STRING(InlineString<10>(Generic(kSize5Capacity10), 5, 1), , "");

#if PW_NC_TEST(Construct_Substr_IndexPastEnd)
  PW_NC_EXPECT("PW_ASSERT\(index <= source_size\)");
  [[maybe_unused]] constexpr InlineString<10> bad_string(kSize5Capacity10, 6);
#endif  // PW_NC_TEST
}

TEST(InlineString, Construct_PointerLength) {
  TEST_STRING(InlineString<0>(static_cast<const char*>(nullptr), 0), , "");
  TEST_STRING(InlineString<10>(static_cast<const char*>(nullptr), 0), , "");

  TEST_STRING(InlineString<0>(kPointer0, 0), , "");
  TEST_STRING(InlineString<0>(kPointer10, 0), , "");
  TEST_STRING(InlineString<10>(kPointer10, 0), , "");

  TEST_STRING(InlineString<1>(kPointer10, 1), , "9");
  TEST_STRING(InlineString<5>(kPointer10, 1), , "9");

  TEST_STRING(InlineString<5>(kPointer10, 4), , "9876");
  TEST_STRING(InlineString<5>(kPointer10 + 1, 4), , "8765");

  TEST_STRING(InlineString<5>(kPointer10, 5), , "98765");
  TEST_STRING(InlineString<5>(kPointer10 + 1, 5), , "87654");

#if PW_NC_TEST(Construct_PointerLength_LengthLargerThanCapacity)
  PW_NC_EXPECT("PW_ASSERT\(new_size <= max_size\(\)\)");
  [[maybe_unused]] constexpr InlineString<5> bad_string(kPointer10, 6);
#elif PW_NC_TEST(Construct_PointerLength_LengthLargerThanInputString)
  PW_NC_EXPECT_CLANG(
      "constexpr variable 'bad_string' must be initialized by a constant "
      "expression");
  PW_NC_EXPECT_GCC("outside the bounds of array type");
  [[maybe_unused]] constexpr InlineString<10> bad_string(kPointer10 + 6, 7);
#endif  // PW_NC_TEST
}

TEST(InlineString, Construct_Pointer) {
  TEST_STRING(InlineString<10>(static_cast<const char*>("")), , "");
  TEST_STRING(InlineString<10>(kPointer10), , "9876543210");
  TEST_STRING(InlineString<10>(kPointer10 + 5), , "43210");
  TEST_STRING(InlineString<10>(kPointer10 + 10), , "");

  TEST_STRING(InlineString<10>(static_cast<const char*>("ab\0cde")), , "ab");
  TEST_STRING(InlineString<2>(static_cast<const char*>("ab\0cde")), , "ab");

#if PW_NC_TEST(Construct_Pointer_LargerThanCapacity)
  PW_NC_EXPECT("PW_ASSERT\(new_size <= max_size\(\)\)");
  [[maybe_unused]] constexpr InlineString<5> bad_string(kPointer10);
#endif  // PW_NC_TEST
}

TEST(InlineString, Construct_Array) {
  TEST_STRING(InlineString<0>(""), , "");

  TEST_STRING(InlineString<1>(""), , "");
  TEST_STRING(InlineString<10>(""), , "");

  TEST_STRING(InlineString<2>("A"), , "A");
  TEST_STRING(InlineString<10>("A"), , "A");
  TEST_STRING(InlineString<10>("123456789"), , "123456789");

  TEST_STRING(InlineString<2>("\0"), , "");
  TEST_STRING(InlineString<10>(""), , "");
  TEST_STRING(InlineString<10>("12\000456789"), , "12");

  TEST_STRING(InlineString<1>(kArrayNull1), , "");
  TEST_STRING(InlineString<5>(kArray5), , "1234");
  TEST_STRING(InlineString<10>(kArray5), , "1234");

#if PW_NC_TEST(Construct_Array_NullTerminationIsRequiredFillsCapacity)
  PW_NC_EXPECT("PW_ASSERT\(.*The array is not null terminated");
  [[maybe_unused]] constexpr InlineString<1> bad_string(kArrayNonNull1);
#elif PW_NC_TEST(Construct_Array_NullTerminationIsRequiredExtraCapacity)
  PW_NC_EXPECT("PW_ASSERT\(.*The array is not null terminated");
  [[maybe_unused]] constexpr InlineString<10> bad_string(kArrayNonNull5);
#elif PW_NC_TEST(Construct_Array_NonTerminatedArrayDoesNotFit)
  PW_NC_EXPECT(
      "InlineString's capacity is too small to hold the assigned string");
  [[maybe_unused]] constexpr InlineString<3> bad_string(kArrayNonNull5);
#elif PW_NC_TEST(Construct_Array_SingleCharLiteralRequiresCapacityOfAtLeast1)
  PW_NC_EXPECT(
      "InlineString's capacity is too small to hold the assigned string");
  [[maybe_unused]] constexpr InlineString<0> bad_string("A");
#elif PW_NC_TEST(Construct_Array_5CharLiteralRequiresCapacityOfAtLeast5)
  PW_NC_EXPECT(
      "InlineString's capacity is too small to hold the assigned string");
  [[maybe_unused]] constexpr InlineString<4> bad_string("ACDEF");
#elif PW_NC_TEST(Construct_Array_TooManyNulls)
  PW_NC_EXPECT(
      "InlineString's capacity is too small to hold the assigned string");
  [[maybe_unused]] constexpr InlineString<3> bad_string(kArray5);
#endif  // PW_NC_TEST
}

TEST(InlineString, Construct_Iterator) {
#if PW_CXX_STANDARD_IS_SUPPORTED(17)  // std::string_view is a C++17 feature
  TEST_STRING(InlineString<0>(kView0.begin(), kView0.end()), , "");
  TEST_STRING(InlineString<0>(kView5.end(), kView5.end()), , "");
  TEST_STRING(InlineString<5>(kView0.begin(), kView0.end()), , "");
  TEST_STRING(InlineString<5>(kView5.end(), kView5.end()), , "");

  TEST_STRING(InlineString<5>(kView5.begin(), kView5.end()), , "12345");
  TEST_STRING(InlineString<10>(kView5.begin(), kView5.end()), , "12345");
  TEST_STRING(InlineString<10>(kView10.begin(), kView10.end()), , "1234567890");
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

  TEST_STRING(InlineString<0>(kEvenNumbers0, kEvenNumbers0), , "");
  TEST_STRING(InlineString<10>(kEvenNumbers2, kEvenNumbers2), , "");

  TEST_STRING(InlineString<4>(kEvenNumbers0, kEvenNumbers2), , "\0");
  TEST_STRING(InlineString<4>(kEvenNumbers0, kEvenNumbers8), , "\0\2\4\6");
  TEST_STRING(InlineString<10>(kEvenNumbers0, kEvenNumbers8), , "\0\2\4\6");

#if PW_NC_TEST(Construct_Iterator_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(current_position != string_end\)");
  [[maybe_unused]] constexpr InlineString<3> str(kEvenNumbers0, kEvenNumbers8);
#endif  // PW_NC_TEST
}

TEST(InlineString, Construct_CopySameCapacity) {
  static_assert(std::is_trivially_copyable<InlineString<0>>(), "Copy");
  static_assert(std::is_trivially_copyable<InlineString<10>>(), "Copy");
  static_assert(std::is_trivially_copyable<InlineBasicString<int, 10>>(),
                "Copy");

  TEST_STRING(InlineString<0>(kEmptyCapacity0), , "");
  TEST_STRING(InlineString<10>(kEmptyCapacity10), , "");
  TEST_STRING(InlineString<10>(kSize5Capacity10), , "12345");
  TEST_STRING(InlineString<10>(kSize10Capacity10), , "1234567890");
}

TEST(InlineString, Construct_CopyDifferentCapacity) {
  TEST_STRING(InlineString<1>(kEmptyCapacity0), , "");
  TEST_STRING(InlineString<5>(kEmptyCapacity0), , "");
  TEST_STRING(InlineString<11>(kEmptyCapacity10), , "");
  TEST_STRING(InlineString<11>(kSize5Capacity10), , "12345");
  TEST_STRING(InlineString<11>(kSize10Capacity10), , "1234567890");
  TEST_STRING(InlineString<30>(kSize10Capacity10), , "1234567890");

#if PW_NC_TEST(Construct_CopyDifferentCapacity_DoesNotFit)
  PW_NC_EXPECT(
      "pw::InlineString must be at least as large as the source string");
  [[maybe_unused]] InlineString<5> bad_string(kEmptyCapacity10);
#elif PW_NC_TEST(Construct_CopyDifferentCapacity_DoesNotFitConstexpr)
  PW_NC_EXPECT(
      "pw::InlineString must be at least as large as the source string");
  constexpr [[maybe_unused]] InlineString<5> bad_string(kEmptyCapacity10);
#endif  // PW_NC_TEST
}

TEST(InlineString, Construct_CopyGenericCapacity) {
  TEST_STRING(InlineString<10>(Generic(kEmptyCapacity0)), , "");
  TEST_RUNTIME_STRING(InlineString<10>(Generic(kEmptyCapacity10)), , "");
  TEST_RUNTIME_STRING(InlineString<10>(Generic(kSize5Capacity10)), , "12345");
  TEST_RUNTIME_STRING(
      InlineString<10>(Generic(kSize10Capacity10)), , "1234567890");
  TEST_RUNTIME_STRING(
      InlineString<20>(Generic(kSize10Capacity10)), , "1234567890");
}

TEST(InlineString, Construct_InitializerList) {
  TEST_STRING(InlineString<0>({}), , "");
  TEST_STRING(InlineString<1>({}), , "");
  TEST_STRING(InlineString<10>({}), , "");

  TEST_STRING(InlineString<1>({'\0'}), , "\0");
  TEST_STRING(InlineString<1>({'?'}), , "?");

  TEST_STRING(InlineString<5>({'A'}), , "A");
  TEST_STRING(InlineString<5>({'\0', '\0', '\0'}), , "\0\0\0");
  TEST_STRING(InlineString<5>({'5', '4', '3', '2', '1'}), , "54321");

#if PW_NC_TEST(Construct_InitializerList_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(new_size <= max_size\(\)\)");
  [[maybe_unused]] constexpr InlineString<3> bad_string({'1', '2', '3', '\0'});
#endif  // PW_NC_TEST
}

#if PW_CXX_STANDARD_IS_SUPPORTED(17)

constexpr InlineString<16> TakesInlineString(const InlineString<16>& str) {
  return str;
}

struct HoldsString {
  InlineString<16> value;
};

TEST(InlineString, Construct_StringView) {
  TEST_STRING(InlineString<0>(""sv), , "");
  TEST_STRING(InlineString<10>(""sv), , "");
  TEST_STRING(InlineString<10>("01234"sv), , "01234");
  TEST_STRING(InlineString<10>("0123456789"sv), , "0123456789");
  TEST_STRING(InlineString<20>("0123456789"sv), , "0123456789");

  TEST_STRING(InlineString<10>(StringViewLike<char>("01234", 5)), , "01234");
  TEST_STRING(InlineString<10>(kStringViewLike10), , "0123456789");

  // pw::InlineString supports implicit conversion from std::string_view.
  constexpr InlineString<16> implicit_call = TakesInlineString("1234"sv);
  EXPECT_CONSTEXPR_PW_STRING(implicit_call, "1234");

  constexpr HoldsString implicit_initialize_1{.value = "1234"sv};
  EXPECT_CONSTEXPR_PW_STRING(implicit_initialize_1.value, "1234");

  constexpr HoldsString implicit_initialize_2{.value{"1234"sv}};
  EXPECT_CONSTEXPR_PW_STRING(implicit_initialize_2.value, "1234");

  constexpr HoldsString implicit_initialize_3{"1234"sv};
  EXPECT_CONSTEXPR_PW_STRING(implicit_initialize_3.value, "1234");

#if PW_NC_TEST(Construct_StringView_DoesNotFit)
  PW_NC_EXPECT(
      "pw::InlineString must be at least as large as the source string");
  [[maybe_unused]] InlineString<5> bad_string(kEmptyCapacity10);
#elif PW_NC_TEST(Construct_StringView_DoesNotFitConstexpr)
  PW_NC_EXPECT(
      "pw::InlineString must be at least as large as the source string");
  constexpr [[maybe_unused]] InlineString<5> bad_string(kEmptyCapacity10);
#elif PW_NC_TEST(Construct_StringView_NoConversionFromAmbiguousClass)
  PW_NC_EXPECT_CLANG("no matching constructor");
  PW_NC_EXPECT_GCC("no matching function for call to");
  [[maybe_unused]] InlineString<10> fail(
      StringViewLikeButConvertsToPointer<char>("1", 1));
#elif PW_NC_TEST(Construct_StringView_NoImplicitConversionFromStringViewLike)
  PW_NC_EXPECT_CLANG("no matching function for call to 'TakesInlineString'");
  PW_NC_EXPECT_GCC(
      "invalid initialization of reference of type .* from expression of type "
      "'const pw::StringViewLike<char>'");
  TakesInlineString(kStringViewLike10);
#elif PW_NC_TEST(Construct_StringView_NoImplicitConvFromStringViewLikeInInit1)
  PW_NC_EXPECT_GCC("could not convert 'pw::kStringViewLike10'");
  PW_NC_EXPECT_CLANG("no viable conversion from 'const StringViewLike<char>'");
  (void)HoldsString{.value = kStringViewLike10};
#elif PW_NC_TEST(Construct_StringView_NoImplicitConvFromStringViewLikeInInit2)
  PW_NC_EXPECT_GCC("could not convert 'pw::kStringViewLike10'");
  PW_NC_EXPECT_CLANG("no viable conversion from 'const StringViewLike<char>'");
  (void)HoldsString{kStringViewLike10};
#endif  // PW_NC_TEST
}

TEST(InlineString, Construct_StringViewSubstr) {
  TEST_STRING(InlineString<0>(""sv, 0, 0), , "");
  TEST_STRING(InlineString<5>(""sv, 0, 0), , "");

  TEST_STRING(InlineString<5>("0123456789"sv, 5, 0), , "");
  TEST_STRING(InlineString<5>("0123456789"sv, 10, 0), , "");

  TEST_STRING(InlineString<5>("0123456789"sv, 0, 5), , "01234");
  TEST_STRING(InlineString<5>("0123456789"sv, 1, 5), , "12345");
  TEST_STRING(InlineString<5>("0123456789"sv, 8, 2), , "89");
  TEST_STRING(InlineString<5>("0123456789"sv, 8, 10), , "89");
  TEST_STRING(InlineString<5>("0123456789"sv, 10, 100), , "");

  TEST_STRING(InlineString<10>("0123456789"sv, 0, 10), , "0123456789");
  TEST_STRING(InlineString<10>("0123456789"sv, 0, 100), , "0123456789");

  TEST_STRING(InlineString<10>(kStringViewLike10, 0, 100), , "0123456789");

#if PW_NC_TEST(Construct_StringViewSubstr_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(new_size <= max_size\(\)\)");
  [[maybe_unused]] constexpr InlineString<10> bad_string(
      "0123456789?"sv, 0, 11);
#elif PW_NC_TEST(Construct_StringViewSubstr_IndexTooFar)
  PW_NC_EXPECT_CLANG("must be initialized by a constant expression");
  PW_NC_EXPECT_GCC("call to non-'constexpr' function");
  [[maybe_unused]] constexpr InlineString<10> bad_string("12345"sv, 6, 0);
#endif  // PW_NC_TEST
}
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

TEST(InlineString, Construct_Nullptr) {
#if PW_NC_TEST(Construct_Nullptr)
  PW_NC_EXPECT("Cannot construct from nullptr");
  [[maybe_unused]] constexpr InlineString<0> bad_string(nullptr);
#endif  // PW_NC_TEST
}

// operator=

TEST(InlineString, AssignOperator_Copy) {
  TEST_STRING(InlineString<0>(), fixed_str = InlineString<0>(), "");
  TEST_STRING(InlineString<10>("something"),
              fixed_str = InlineString<9>("el\0se"),
              "el");
  TEST_STRING(InlineString<10>("0_o"), fixed_str = InlineString<10>(), "");

#if PW_NC_TEST(AssignOperator_Copy_DoesNotFit)
  PW_NC_EXPECT(
      "pw::InlineString must be at least as large as the source string");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<5> str;
    return str = InlineString<6>("2big");
  }();
#elif PW_NC_TEST(AssignOperator_Copy_NotSupportedByGeneric)
  PW_NC_EXPECT("operator=.*protected");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<5> str;
    return Generic(str) = InlineString<0>();
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, AssignOperator_Array) {
  TEST_STRING(InlineString<1>({'a'}), fixed_str = "", "");
  TEST_STRING(InlineString<10>("hey"), fixed_str = "wow", "wow");
  TEST_STRING(InlineString<10>("hey"), fixed_str = "123456789", "123456789");

#if PW_NC_TEST(AssignOperator_Array_DoesNotFit)
  PW_NC_EXPECT(
      "InlineString's capacity is too small to hold the assigned string");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<4> str("abc");
    return str = "12345";
  }();
#elif PW_NC_TEST(AssignOperator_Array_NotSupportedByGeneric)
  PW_NC_EXPECT("operator=");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<5> str("abc");
    return Generic(str) = "";
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, AssignOperator_Pointer) {
  TEST_STRING(InlineString<1>({'a'}), fixed_str = kPointer0, "");
  TEST_STRING(InlineString<10>("hey"),
              fixed_str = static_cast<const char*>("wow"),
              "wow");
  TEST_STRING(InlineString<10>("hey"), fixed_str = kPointer10, "9876543210");

#if PW_NC_TEST(AssignOperator_Pointer_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(new_size <= max_size\(\)\)");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<5> str("abc");
    return str = static_cast<const char*>("123456");
  }();
#elif PW_NC_TEST(AssignPointer_Pointer_NotSupportedByGeneric)
  PW_NC_EXPECT("operator=");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<5> str("abc");
    return Generic(str) = kPointer0;
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, AssignOperator_Character) {
  TEST_STRING(InlineString<1>(), fixed_str = '\0', "\0");
  TEST_STRING(InlineString<1>({'a'}), fixed_str = '\0', "\0");
  TEST_STRING(InlineString<10>("hey"), fixed_str = '?', "?");

#if PW_NC_TEST(AssignPointer_Character_DoesNotFit)
  PW_NC_EXPECT("Cannot assign a character to pw::InlineString<0>");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<0> str;
    return str = 'a';
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, AssignOperator_InitializerList) {
  TEST_STRING(InlineString<1>(), fixed_str = {'\0'}, "\0");
  TEST_STRING(InlineString<1>({'a'}), fixed_str = {'\0'}, "\0");
  TEST_STRING(
      InlineString<10>("hey"), (fixed_str = {'W', 'h', 'y', '?'}), "Why?");

#if PW_NC_TEST(AssignPointer_InitializerList_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(new_size <= max_size\(\)\)");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<2> str;
    return str = {'1', '2', '3'};
  }();
#endif  // PW_NC_TEST
}

#if PW_CXX_STANDARD_IS_SUPPORTED(17)
TEST(InlineString, AssignOperator_StringView) {
  TEST_STRING(InlineString<1>(), fixed_str = "\0"sv, "\0");
  TEST_STRING(InlineString<1>({'a'}), fixed_str = "\0"sv, "\0");
  TEST_STRING(InlineString<10>("hey"), fixed_str = "Why?"sv, "Why?");

#if PW_NC_TEST(AssignPointer_StringView_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(new_size <= max_size\(\)\)");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<2> str;
    return str = "123"sv;
  }();
#endif  // PW_NC_TEST
}
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

// assign

TEST(InlineString, Assign_Characters) {
  TEST_STRING(InlineString<0>(), str.assign(0, 'a'), "");
  TEST_STRING(InlineString<10>(), str.assign(0, 'a'), "");
  TEST_STRING(InlineString<10>("hey"), str.assign(0, 'a'), "");

  TEST_STRING(InlineString<1>(), str.assign(1, 'a'), "a");
  TEST_STRING(InlineString<10>(), str.assign(10, 'a'), "aaaaaaaaaa");

  TEST_STRING(InlineString<1>({'?'}), str.assign(1, 'a'), "a");
  TEST_STRING(InlineString<10>("1"), str.assign(1, 'a'), "a");
  TEST_STRING(InlineString<10>("123456789"), str.assign(1, 'a'), "a");
  TEST_STRING(InlineString<10>("?"), str.assign(10, 'a'), "aaaaaaaaaa");

  TEST_STRING(InlineString<5>("?"), str.assign(0, '\0'), "");
  TEST_STRING(InlineString<5>("?"), str.assign(1, '\0'), "\0");
  TEST_STRING(InlineString<5>("?"), str.assign(5, '\0'), "\0\0\0\0\0");
  TEST_STRING(InlineString<10>("???"), str.assign(5, '\0'), "\0\0\0\0\0");

#if PW_NC_TEST(Assign_Characters_TooMany)
  PW_NC_EXPECT("PW_ASSERT\(new_size <= max_size\(\)\)");
  [[maybe_unused]] constexpr auto value = [] {
    InlineString<6> str;
    return str.assign(7, '?');
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, Assign_CopySameCapacity) {
  TEST_STRING(InlineString<0>(), str.assign(kEmptyCapacity0), "");
  TEST_STRING(InlineString<10>(), str.assign(kEmptyCapacity10), "");
  TEST_STRING(InlineString<10>(), str.assign(kSize5Capacity10), "12345");
  TEST_STRING(InlineString<10>(), str.assign(kSize10Capacity10), "1234567890");
}

TEST(InlineString, Assign_CopyDifferentCapacity) {
  TEST_STRING(InlineString<1>(), str.assign(kEmptyCapacity0), "");
  TEST_STRING(InlineString<5>(), str.assign(kEmptyCapacity0), "");
  TEST_STRING(InlineString<11>(), str.assign(kEmptyCapacity10), "");
  TEST_STRING(InlineString<11>(), str.assign(kSize5Capacity10), "12345");
  TEST_STRING(InlineString<11>(), str.assign(kSize10Capacity10), "1234567890");
  TEST_STRING(InlineString<30>(), str.assign(kSize10Capacity10), "1234567890");

#if PW_NC_TEST(Assign_CopyDifferentCapacity_DoesNotFit)
  PW_NC_EXPECT(
      "pw::InlineString must be at least as large as the source string");
  [[maybe_unused]] InlineString<5> bad_string;
  bad_string.assign(kEmptyCapacity10);
#elif PW_NC_TEST(Assign_CopyDifferentCapacity_DoesNotFitConstexpr)
  PW_NC_EXPECT(
      "pw::InlineString must be at least as large as the source string");
  [[maybe_unused]] InlineString<5> bad_string;
  bad_string.assign(kEmptyCapacity10);
#endif  // PW_NC_TEST
}

TEST(InlineString, Assign_CopyGenericCapacity) {
  TEST_STRING(InlineString<10>(), str.assign(Generic(kEmptyCapacity0)), "");
  TEST_RUNTIME_STRING(
      InlineString<10>(), str.assign(Generic(kEmptyCapacity10)), "");
  TEST_RUNTIME_STRING(
      InlineString<10>(), str.assign(Generic(kSize5Capacity10)), "12345");
  TEST_RUNTIME_STRING(
      InlineString<10>(), str.assign(Generic(kSize10Capacity10)), "1234567890");
  TEST_RUNTIME_STRING(
      InlineString<20>(), str.assign(Generic(kSize10Capacity10)), "1234567890");
}

TEST(InlineString, Assign_Substr) {  // NOLINT(google-readability-function-size)
  TEST_STRING(InlineString<10>(), str.assign(kEmptyCapacity0, 0), "");
  TEST_STRING(InlineString<10>(), str.assign(kEmptyCapacity0, 0, 0), "");
  TEST_STRING(InlineString<10>(), str.assign(Generic(kEmptyCapacity0), 0), "");
  TEST_STRING(
      InlineString<10>(), str.assign(Generic(kEmptyCapacity0), 0, 0), "");

  TEST_STRING(InlineString<0>(), str.assign(kEmptyCapacity10, 0), "");
  TEST_STRING(InlineString<0>(), str.assign(kEmptyCapacity10, 0, 0), "");
  TEST_RUNTIME_STRING(
      InlineString<0>(), str.assign(Generic(kEmptyCapacity10), 0), "");
  TEST_RUNTIME_STRING(
      InlineString<0>(), str.assign(Generic(kEmptyCapacity10), 0, 0), "");

  TEST_STRING(InlineString<10>(), str.assign(kSize5Capacity10, 0), "12345");
  TEST_RUNTIME_STRING(
      InlineString<10>(), str.assign(Generic(kSize5Capacity10), 0), "12345");
  TEST_STRING(InlineString<10>(), str.assign(kSize5Capacity10, 1), "2345");
  TEST_RUNTIME_STRING(
      InlineString<10>(), str.assign(Generic(kSize5Capacity10), 1), "2345");
  TEST_STRING(InlineString<10>(), str.assign(kSize5Capacity10, 4), "5");
  TEST_RUNTIME_STRING(
      InlineString<10>(), str.assign(Generic(kSize5Capacity10), 4), "5");
  TEST_STRING(InlineString<10>(), str.assign(kSize5Capacity10, 5), "");
  TEST_RUNTIME_STRING(
      InlineString<10>(), str.assign(Generic(kSize5Capacity10), 5), "");

  TEST_STRING(InlineString<10>(), str.assign(kSize5Capacity10, 0, 0), "");
  TEST_RUNTIME_STRING(
      InlineString<10>(), str.assign(Generic(kSize5Capacity10), 0, 0), "");
  TEST_STRING(InlineString<10>(), str.assign(kSize5Capacity10, 0, 1), "1");
  TEST_RUNTIME_STRING(
      InlineString<10>(), str.assign(Generic(kSize5Capacity10), 0, 1), "1");
  TEST_STRING(InlineString<10>(), str.assign(kSize5Capacity10, 1, 0), "");
  TEST_RUNTIME_STRING(
      InlineString<10>(), str.assign(Generic(kSize5Capacity10), 1, 0), "");
  TEST_STRING(InlineString<10>(), str.assign(kSize5Capacity10, 1, 1), "2");
  TEST_RUNTIME_STRING(
      InlineString<10>(), str.assign(Generic(kSize5Capacity10), 1, 1), "2");
  TEST_STRING(InlineString<10>(), str.assign(kSize5Capacity10, 1, 4), "2345");
  TEST_RUNTIME_STRING(
      InlineString<10>(), str.assign(Generic(kSize5Capacity10), 1, 4), "2345");

  TEST_STRING(InlineString<10>(), str.assign(kSize5Capacity10, 4, 9000), "5");
  TEST_RUNTIME_STRING(
      InlineString<10>(), str.assign(Generic(kSize5Capacity10), 4, 9000), "5");

  TEST_STRING(InlineString<10>(), str.assign(kSize5Capacity10, 5, 0), "");
  TEST_RUNTIME_STRING(
      InlineString<10>(), str.assign(Generic(kSize5Capacity10), 5, 0), "");
  TEST_STRING(InlineString<10>(), str.assign(kSize5Capacity10, 5, 1), "");
  TEST_RUNTIME_STRING(
      InlineString<10>(), str.assign(Generic(kSize5Capacity10), 5, 1), "");

#if PW_NC_TEST(Assign_Substr_IndexPastEnd)
  PW_NC_EXPECT("PW_ASSERT\(index <= source_size\)");
  [[maybe_unused]] constexpr auto bad_string = [] {
    InlineString<10> str;
    return str.assign(kSize5Capacity10, 6);
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, Assign_PointerLength) {
  TEST_STRING(InlineString<0>(), str.assign(nullptr, 0), "");
  TEST_STRING(InlineString<10>(), str.assign(nullptr, 0), "");

  TEST_STRING(InlineString<0>(), str.assign(kPointer0, 0), "");
  TEST_STRING(InlineString<0>(), str.assign(kPointer10, 0), "");
  TEST_STRING(InlineString<10>("abc"), str.assign(kPointer10, 0), "");

  TEST_STRING(InlineString<1>(), str.assign(kPointer10, 1), "9");
  TEST_STRING(InlineString<5>("?"), str.assign(kPointer10, 1), "9");

  TEST_STRING(InlineString<5>("?"), str.assign(kPointer10, 4), "9876");
  TEST_STRING(InlineString<5>("?"), str.assign(kPointer10 + 1, 4), "8765");

  TEST_STRING(InlineString<5>("?"), str.assign(kPointer10, 5), "98765");
  TEST_STRING(InlineString<5>("?"), str.assign(kPointer10 + 1, 5), "87654");

#if PW_NC_TEST(Assign_PointerLength_LengthLargerThanCapacity)
  PW_NC_EXPECT("PW_ASSERT\(new_size <= max_size\(\)\)");
  [[maybe_unused]] constexpr auto bad_string = [] {
    InlineString<5> str;
    return str.assign(kPointer10, 6);
  }();
#elif PW_NC_TEST(Assign_PointerLength_LengthLargerThanInputString)
  PW_NC_EXPECT_CLANG(
      "constexpr variable 'bad_string' must be initialized by a constant "
      "expression");
  PW_NC_EXPECT_GCC("outside the bounds of array type");
  [[maybe_unused]] constexpr auto bad_string = [] {
    InlineString<10> str;
    return str.assign(kPointer10 + 6, 7);
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, Assign_Pointer) {
  TEST_STRING(
      InlineString<10>("\0"), str.assign(static_cast<const char*>("")), "");
  TEST_STRING(InlineString<10>("abc"), str.assign(kPointer10), "9876543210");
  TEST_STRING(InlineString<10>("abc"), str.assign(kPointer10 + 5), "43210");
  TEST_STRING(InlineString<10>("abc"), str.assign(kPointer10 + 10), "");

  TEST_STRING(InlineString<10>(),
              str.assign(static_cast<const char*>("ab\0cde")),
              "ab");
  TEST_STRING(
      InlineString<2>(), str.assign(static_cast<const char*>("ab\0cde")), "ab");

#if PW_NC_TEST(Assign_Pointer_LargerThanCapacity)
  PW_NC_EXPECT("PW_ASSERT\(new_size <= max_size\(\)\)");
  [[maybe_unused]] constexpr auto bad_string = [] {
    InlineString<5> str;
    return str.assign(kPointer10);
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, Assign_Array) {
  TEST_STRING(InlineString<0>(), str.assign(""), "");
  TEST_STRING(InlineString<1>(), str.assign(""), "");
  TEST_STRING(InlineString<10>("a"), str.assign(""), "");

  TEST_STRING(InlineString<1>(), str.assign("A"), "A");
  TEST_STRING(InlineString<10>(), str.assign("A"), "A");
  TEST_STRING(InlineString<10>(), str.assign("123456789"), "123456789");

  TEST_STRING(InlineString<1>(), str.assign("\0"), "");
  TEST_STRING(InlineString<10>(), str.assign("\0"), "");
  TEST_STRING(InlineString<10>(), str.assign("12\000456789"), "12");

  TEST_STRING(InlineString<1>(""), str.assign(kArrayNull1), "");
  TEST_STRING(InlineString<5>(), str.assign(kArray5), "1234");
  TEST_STRING(InlineString<10>(), str.assign(kArray5), "1234");

  TEST_RUNTIME_STRING(InlineString<1>(), Generic(str).assign("?"), "?");
  TEST_RUNTIME_STRING(
      InlineString<5>("abcd"), Generic(str).assign("12345"), "12345");

#if 0   // Triggers PW_ASSERT
  [[maybe_unused]] InlineString<5> too_small("abcd");
  Generic(too_small).assign("123456");
#endif  // 0

#if PW_NC_TEST(Assign_Array_NullTerminationIsRequiredFillsCapacity)
  PW_NC_EXPECT("PW_ASSERT\(.*The array is not null terminated");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<1> bad_string;
    return bad_string.assign(kArrayNonNull1);
  }();
#elif PW_NC_TEST(Assign_Array_NullTerminationIsRequiredExtraCapacity)
  PW_NC_EXPECT("PW_ASSERT\(.*The array is not null terminated");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<10> bad_string;
    return bad_string.assign(kArrayNonNull5);
  }();
#elif PW_NC_TEST(Assign_Array_NonTerminatedArrayDoesNotFit)
  PW_NC_EXPECT(
      "InlineString's capacity is too small to hold the assigned string");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<3> bad_string;
    return bad_string.assign(kArrayNonNull5);
  }();
#elif PW_NC_TEST(Assign_Array_SingleCharLiteralRequiresCapacityOfAtLeast1)
  PW_NC_EXPECT(
      "InlineString's capacity is too small to hold the assigned string");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<0> str;
    return str.assign("?");
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, Assign_Iterator) {
#if PW_CXX_STANDARD_IS_SUPPORTED(17)
  TEST_STRING(InlineString<0>(), str.assign(kView0.begin(), kView0.end()), "");
  TEST_STRING(InlineString<0>(), str.assign(kView5.end(), kView5.end()), "");
  TEST_STRING(
      InlineString<5>("abc"), str.assign(kView0.begin(), kView0.end()), "");
  TEST_STRING(
      InlineString<5>("abc"), str.assign(kView5.end(), kView5.end()), "");

  TEST_STRING(
      InlineString<5>(), str.assign(kView5.begin(), kView5.end()), "12345");
  TEST_STRING(InlineString<10>("abc"),
              str.assign(kView5.begin(), kView5.end()),
              "12345");
  TEST_STRING(InlineString<10>("abc"),
              str.assign(kView10.begin(), kView10.end()),
              "1234567890");
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

  TEST_STRING(InlineString<0>(), str.assign(kEvenNumbers0, kEvenNumbers0), "");
  TEST_STRING(InlineString<10>(), str.assign(kEvenNumbers2, kEvenNumbers2), "");

  TEST_STRING(
      InlineString<4>("abc"), str.assign(kEvenNumbers0, kEvenNumbers2), "\0");
  TEST_STRING(InlineString<4>("abc"),
              str.assign(kEvenNumbers0, kEvenNumbers8),
              "\0\2\4\6");
  TEST_STRING(InlineString<10>("abc"),
              str.assign(kEvenNumbers0, kEvenNumbers8),
              "\0\2\4\6");

#if PW_NC_TEST(Assign_Iterator_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(current_position != string_end\)");
  [[maybe_unused]] constexpr auto bad_string = [] {
    InlineString<3> str;
    return str.assign(kEvenNumbers0, kEvenNumbers8);
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, Assign_InitializerList) {
  TEST_STRING(InlineString<0>(), str.assign({}), "");
  TEST_STRING(InlineString<1>(), str.assign({}), "");
  TEST_STRING(InlineString<10>("abc"), str.assign({}), "");

  TEST_STRING(InlineString<1>(), str.assign({'\0'}), "\0");
  TEST_STRING(InlineString<1>(), str.assign({'?'}), "?");

  TEST_STRING(InlineString<5>("abc"), str.assign({'A'}), "A");
  TEST_STRING(InlineString<5>("abc"), str.assign({'\0', '\0', '\0'}), "\0\0\0");
  TEST_STRING(
      InlineString<5>("abc"), str.assign({'5', '4', '3', '2', '1'}), "54321");

#if PW_NC_TEST(Assign_InitializerList_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(new_size <= max_size\(\)\)");
  [[maybe_unused]] constexpr auto bad_string = [] {
    InlineString<3> str;
    return str.assign({'1', '2', '3', '\0'});
  }();
#endif  // PW_NC_TEST
}

#if PW_CXX_STANDARD_IS_SUPPORTED(17)
TEST(InlineString, Assign_StringView) {
  TEST_STRING(InlineString<0>(), str.assign(""sv), "");
  TEST_STRING(InlineString<10>("abc"), str.assign(""sv), "");
  TEST_STRING(InlineString<10>("abc"), str.assign("01234"sv), "01234");
  TEST_STRING(
      InlineString<10>("abc"), str.assign("0123456789"sv), "0123456789");
  TEST_STRING(InlineString<20>(), str.assign("0123456789"sv), "0123456789");

  TEST_STRING(InlineString<10>("abc"),
              str.assign(StringViewLike<char>("01234", 5)),
              "01234");
  TEST_STRING(InlineString<10>(), str.assign(kStringViewLike10), "0123456789");

#if PW_NC_TEST(Assign_StringView_DoesNotFit)
  PW_NC_EXPECT(
      "pw::InlineString must be at least as large as the source string");
  [[maybe_unused]] InlineString<5> bad_string;
  bad_string.assign(kEmptyCapacity10);
#elif PW_NC_TEST(Assign_StringView_DoesNotFitConstexpr)
  PW_NC_EXPECT(
      "pw::InlineString must be at least as large as the source string");
  [[maybe_unused]] constexpr auto bad_string = [] {
    InlineString<5> str;
    str.assign(kEmptyCapacity10);
    return str;
  }();
#elif PW_NC_TEST(Assign_StringView_NoAssignmentFromAmbiguousClass)
  PW_NC_EXPECT_CLANG("no matching member function for call to");
  PW_NC_EXPECT_GCC("no matching function for call to");
  [[maybe_unused]] InlineString<10> fail;
  fail.assign(StringViewLikeButConvertsToPointer<char>("1", 1));
#endif  // PW_NC_TEST
}

// NOLINTNEXTLINE(google-readability-function-size)
TEST(InlineString, Assign_StringViewSubstr) {
  TEST_STRING(InlineString<0>(), str.assign(""sv, 0, 0), "");
  TEST_STRING(InlineString<5>(), str.assign(""sv, 0, 0), "");

  TEST_STRING(InlineString<5>(), str.assign("0123456789"sv, 5, 0), "");
  TEST_STRING(InlineString<5>(), str.assign("0123456789"sv, 10, 0), "");

  TEST_STRING(InlineString<5>(), str.assign("0123456789"sv, 0, 5), "01234");
  TEST_STRING(InlineString<5>(), str.assign("0123456789"sv, 1, 5), "12345");
  TEST_STRING(InlineString<5>(), str.assign("0123456789"sv, 8, 2), "89");
  TEST_STRING(InlineString<5>(), str.assign("0123456789"sv, 8, 10), "89");
  TEST_STRING(InlineString<5>(), str.assign("0123456789"sv, 10, 10), "");
  TEST_STRING(InlineString<5>(), str.assign("0123456789"sv, 10, 100), "");

  TEST_STRING(
      InlineString<10>(), str.assign("0123456789"sv, 0, 10), "0123456789");
  TEST_STRING(
      InlineString<10>(), str.assign("0123456789"sv, 0, 100), "0123456789");

  TEST_STRING(
      InlineString<10>(), str.assign(kStringViewLike10, 0, 100), "0123456789");

#if PW_NC_TEST(Assign_StringViewSubstr_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(new_size <= max_size\(\)\)");
  [[maybe_unused]] constexpr auto bad_string = [] {
    InlineString<10> str;
    return str.assign("0123456789?"sv, 0, 11);
  }();
#elif PW_NC_TEST(Assign_StringViewSubstr_IndexTooFar)
  PW_NC_EXPECT_CLANG("must be initialized by a constant expression");
  PW_NC_EXPECT_GCC("call to non-'constexpr' function");
  [[maybe_unused]] constexpr auto bad_string = [] {
    InlineString<10> str;
    return str.assign("12345"sv, 6, 0);
  }();
#endif  // PW_NC_TEST
}
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

//
// Element access
//

TEST(InlineString, At) {
  static_assert(kSize5Capacity10.at(0) == '1', "1");
  static_assert(kSize5Capacity10.at(1) == '2', "2");
  static_assert(kSize5Capacity10.at(2) == '3', "3");
  static_assert(kSize5Capacity10.at(3) == '4', "4");
  static_assert(kSize5Capacity10.at(4) == '5', "5");

  static_assert(kSize10Capacity10.at(9) == '0', "null");

  EXPECT_EQ(Generic(kSize5Capacity10).at(0), '1');
  EXPECT_EQ(Generic(kSize5Capacity10).at(1), '2');
  EXPECT_EQ(Generic(kSize5Capacity10).at(2), '3');
  EXPECT_EQ(Generic(kSize5Capacity10).at(3), '4');
  EXPECT_EQ(Generic(kSize5Capacity10).at(4), '5');

#if PW_NC_TEST(At_OutOfBounds)
  PW_NC_EXPECT("PW_ASSERT\(index < length\(\)\);");
  [[maybe_unused]] constexpr char out_of_bounds = kSize5Capacity10.at(5);
#endif  // PW_NC_TEST
}

TEST(InlineString, SubscriptOperator) {
  static_assert(kSize5Capacity10[0] == '1', "1");
  static_assert(kSize5Capacity10[1] == '2', "2");
  static_assert(kSize5Capacity10[2] == '3', "3");
  static_assert(kSize5Capacity10[3] == '4', "4");
  static_assert(kSize5Capacity10[4] == '5', "5");

  static_assert(kSize10Capacity10[9] == '0', "null");

  EXPECT_EQ(Generic(kSize5Capacity10)[0], '1');
  EXPECT_EQ(Generic(kSize5Capacity10)[1], '2');
  EXPECT_EQ(Generic(kSize5Capacity10)[2], '3');
  EXPECT_EQ(Generic(kSize5Capacity10)[3], '4');
  EXPECT_EQ(Generic(kSize5Capacity10)[4], '5');

  static_assert(kSize5Capacity10[5] == '\0', "No range checking");
  static_assert(kSize5Capacity10[6] == '\0', "No range checking");
}

TEST(InlineString, FrontBack) {
  static_assert(kSize10Capacity10.front() == '1', "1");
  static_assert(kSize10Capacity10.back() == '0', "0");
  EXPECT_EQ(Generic(kSize10Capacity10).front(), '1');
  EXPECT_EQ(Generic(kSize10Capacity10).back(), '0');
}

TEST(InlineString, DataCStr) {
  static_assert(kSize10Capacity10.data() == kSize10Capacity10.c_str(),
                "data() and c_str()");
  EXPECT_EQ(Generic(kSize10Capacity10).data(),
            Generic(kSize10Capacity10).c_str());

  EXPECT_STREQ(kSize10Capacity10.data(), "1234567890");
  EXPECT_STREQ(kSize10Capacity10.c_str(), "1234567890");
}

#if PW_CXX_STANDARD_IS_SUPPORTED(17)
TEST(InlineString, ConvertsToStringView) {
  static_assert(std::string_view(kSize5Capacity10) == "12345"sv);
  EXPECT_EQ(std::string_view(Generic(kSize5Capacity10)), "12345"sv);
}

//
// Iterators
//

TEST(InlineString, Iterators) {
  static_assert(kEmptyCapacity10.begin() == kEmptyCapacity10.end());
  static_assert(kSize5Capacity10.end() - kSize5Capacity10.begin() == 5u);
  static_assert(kSize5Capacity10.begin() + 5 == kSize5Capacity10.end());

  static_assert(*kSize5Capacity10.begin() == '1');
  static_assert(*(kSize5Capacity10.begin() + 1) == '2');

  static_assert(kEmptyCapacity10.rbegin() == kEmptyCapacity10.rend());
  static_assert(kSize5Capacity10.rend() - kSize5Capacity10.rbegin() == 5u);
  static_assert(kSize5Capacity10.rbegin() + 5 == kSize5Capacity10.rend());

  static_assert(*kSize5Capacity10.rbegin() == '5');
  static_assert(*(kSize5Capacity10.rbegin() + 1) == '4');

  static_assert(kSize5Capacity10.begin() == kSize5Capacity10.cbegin());
  static_assert(kSize5Capacity10.end() == kSize5Capacity10.cend());
  static_assert(kSize5Capacity10.rbegin() == kSize5Capacity10.crbegin());
  static_assert(kSize5Capacity10.rend() == kSize5Capacity10.crend());

  static_assert([] {
    char expected = '1';
    for (char ch : kSize5Capacity10) {
      if (ch != expected) {
        return false;
      }
      expected += 1;
    }
    return true;
  }());
}
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

//
// Capacity
//

TEST(InlineString, Size) {
  static_assert(kEmptyCapacity0.empty(), "empty");
  static_assert(kEmptyCapacity10.empty(), "empty");

  static_assert(kEmptyCapacity10.size() == 0u, "0");  // NOLINT
  static_assert(kSize5Capacity10.size() == 5u, "5");
  static_assert(kEmptyCapacity10.length() == 0u, "0");
  static_assert(kSize5Capacity10.length() == 5u, "5");
}

TEST(InlineString, MaxSize) {
  static_assert(InlineString<0>().max_size() == 0u, "0");
  static_assert(InlineString<1>().max_size() == 1u, "1");
  static_assert(InlineString<10>().max_size() == 10u, "10");
  static_assert(InlineString<10>("123").max_size() == 10u, "10");
  static_assert(Generic(InlineString<10>("123")).max_size() == 10u, "10");

  static_assert(InlineString<0>().capacity() == 0u, "0");
  static_assert(InlineString<10>().capacity() == 10u, "10");
}

//
// Operations
//

// clear

TEST(InlineString, Clear) {
  TEST_STRING(InlineString<0>(), str.clear(), "");
  TEST_STRING(InlineString<8>(), str.clear(), "");
  TEST_STRING(InlineString<8>("stuff"), str.clear(), "");
  TEST_RUNTIME_STRING(InlineString<8>("stuff"), generic_str.clear(), "");
  TEST_STRING(InlineString<8>("!!"), str.clear(); str.assign("?"), "?");
}

// TODO(b/239996007): Test insert.

// TODO(b/239996007): Test erase.

TEST(InlineString, PushBack) {
  TEST_STRING(InlineString<1>(), str.push_back('#'), "#");
  TEST_STRING(InlineString<5>("abc"), str.push_back('d');
              str.push_back('e'), "abcde");

#if PW_NC_TEST(PushBack_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(size\(\) < max_size\(\)\)");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<1> str("?", 1);
    str.push_back('a');
    return str;
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, PopBack) {
  TEST_STRING(InlineString<1>("?", 1), str.pop_back(), "");
  TEST_STRING(InlineString<1>(), str.push_back('?'); str.pop_back(), "");

  TEST_STRING(InlineString<5>("abc"), str.pop_back(), "ab");
  TEST_STRING(InlineString<5>("abcde", 5), str.pop_back(), "abcd");

#if PW_NC_TEST(PopBack_Empty)
  PW_NC_EXPECT("PW_ASSERT\(!empty\(\)\)");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<0> str;
    str.pop_back();
    return str;
  }();
#endif  // PW_NC_TEST
}

// append

TEST(InlineString, Append_BasicString) {
  TEST_STRING(InlineString<0>(), str.append(kEmptyCapacity0), "");
  TEST_STRING(InlineString<10>(), str.append(kEmptyCapacity10), "");
  TEST_STRING(InlineString<10>(), str.append(kSize5Capacity10), "12345");
  TEST_STRING(InlineString<10>(), str.append(kSize10Capacity10), "1234567890");

  TEST_STRING(InlineString<1>({'a'}), str.append(kEmptyCapacity0), "a");
  TEST_STRING(InlineString<11>("a"), str.append(kEmptyCapacity10), "a");
  TEST_STRING(InlineString<11>("a"), str.append(kSize5Capacity10), "a12345");
  TEST_STRING(
      InlineString<11>("a"), str.append(kSize10Capacity10), "a1234567890");

#if PW_NC_TEST(Append_BasicString_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(count <= max_size\(\) - size\(\)\)");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<3> str({0, 1});
    return str.append(kSize5Capacity10);
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, Append_Characters) {
  TEST_STRING(InlineString<1>(), str.append(0, '1'), "");
  TEST_STRING(InlineString<1>(), str.append(1, '1'), "1");
  TEST_STRING(InlineString<10>(), str.append(2, '1'), "11");
  TEST_STRING(InlineString<10>(), str.append(10, '1'), "1111111111");

  TEST_STRING(InlineString<4>("Hi"), str.append(0, '!'), "Hi");
  TEST_STRING(InlineString<4>("Hi"), str.append(1, '!'), "Hi!");
  TEST_STRING(InlineString<6>("Hi"), str.append(2, '!'), "Hi!!");
  TEST_STRING(InlineString<6>("Hi"), str.append(4, '!'), "Hi!!!!");

#if PW_NC_TEST(Append_Characters_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(count <= max_size\(\) - size\(\)\)");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<3> str({0, 1});
    return str.append(2, '?');
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, Append_PointerSize) {
  TEST_STRING(InlineString<0>(), str.append("", 0), "");
  TEST_STRING(InlineString<10>(), str.append("", 0), "");
  TEST_STRING(InlineString<1>(), str.append("?", 1), "?");
  TEST_STRING(InlineString<10>("abc"), str.append("", 0), "abc");
  TEST_STRING(InlineString<10>(), str.append("1234567", 1), "1");
  TEST_STRING(InlineString<10>("abc"), str.append("1234567", 3), "abc123");

#if PW_NC_TEST(Append_PointerSize_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(count <= max_size\(\) - size\(\)\)");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<3> str({0, 1});
    return str.append("23", 2);
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, Append_Array) {
  TEST_STRING(InlineString<1>(), fixed_str.append(""), "");
  TEST_STRING(InlineString<2>(), fixed_str.append("a"), "a");
  TEST_STRING(InlineString<6>(), fixed_str.append("12345"), "12345");

  TEST_STRING(InlineString<1>({'a'}), fixed_str.append(""), "a");
  TEST_STRING(InlineString<2>("a"), fixed_str.append("a"), "aa");
  TEST_STRING(InlineString<6>("a"), fixed_str.append("12345"), "a12345");

#if PW_NC_TEST(Append_Array_DoesNotFit)
  PW_NC_EXPECT(
      "InlineString's capacity is too small to hold the assigned string");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<2> str;
    return str.append("123");
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, Append_Pointer) {
  TEST_STRING(InlineString<0>(), str.append(kPointer0), "");
  TEST_STRING(InlineString<10>(), str.append(kPointer10), "9876543210");
  TEST_STRING(InlineString<10>("abc"), str.append(kPointer10 + 5), "abc43210");
  TEST_STRING(InlineString<13>("abc"), str.append(kPointer10), "abc9876543210");

#if PW_NC_TEST(Append_Pointer_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(count <= max_size\(\) - size\(\)\)");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<3> str({0, 1});
    return str.append(kPointer10 + 8);
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, Append_Iterator) {
  TEST_STRING(InlineString<0>(), str.append(kEvenNumbers0, kEvenNumbers0), "");
  TEST_STRING(InlineString<10>(), str.append(kEvenNumbers0, kEvenNumbers0), "");
  TEST_STRING(InlineString<10>(), str.append(kEvenNumbers0, kEvenNumbers0), "");
  TEST_STRING(
      InlineString<10>(), str.append(kEvenNumbers0, kEvenNumbers8), "\0\2\4\6");
  TEST_STRING(InlineString<10>("a"),
              str.append(kEvenNumbers0, kEvenNumbers8),
              "a\0\2\4\6");

#if PW_NC_TEST(Append_Iterator_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(current_position != string_end\)");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<3> str;
    return str.append(kEvenNumbers0, kEvenNumbers8);
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, Append_InitializerList) {
  TEST_STRING(InlineString<0>(), str.append({}), "");
  TEST_STRING(InlineString<10>(), str.append({1, 2, 3}), "\1\2\3");
  TEST_STRING(InlineString<10>("abc"), str.append({1, 2, 3}), "abc\1\2\3");
  TEST_STRING(InlineString<5>("abc"), str.append({'4', '5'}), "abc45");

#if PW_NC_TEST(Append_InitializerList_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(count <= max_size\(\) - size\(\)\)");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<3> str({0, 1, 2});
    return str.append({3});
  }();
#endif  // PW_NC_TEST
}

#if PW_CXX_STANDARD_IS_SUPPORTED(17)
TEST(InlineString, Append_StringView) {
  TEST_STRING(InlineString<0>(), str.append(""sv), "");
  TEST_STRING(InlineString<10>("a"), str.append(""sv), "a");
  TEST_STRING(InlineString<10>("abc"), str.append("123"sv), "abc123");
  TEST_STRING(InlineString<5>("abc"), str.append("45"sv), "abc45");

#if PW_NC_TEST(Append_StringView_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(count <= max_size\(\) - size\(\)\)");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<3> str({0, 1, 2});
    return str.append("3"sv);
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, Append_StringViewSubstr) {
  TEST_STRING(InlineString<0>(), str.append(""sv, 0), "");
  TEST_STRING(InlineString<0>(), str.append(""sv, 0, 0), "");
  TEST_RUNTIME_STRING(InlineString<4>("a"), str.append("123"sv, 0), "a123");
  TEST_RUNTIME_STRING(InlineString<4>("a"), str.append("123"sv, 1, 0), "a");
  TEST_RUNTIME_STRING(InlineString<4>("a"), str.append("123"sv, 1, 1), "a2");
  TEST_RUNTIME_STRING(InlineString<4>("a"), str.append("123"sv, 1, 99), "a23");
  TEST_RUNTIME_STRING(InlineString<4>("a"), str.append("123"sv, 3, 99), "a");

#if PW_NC_TEST(Append_StringViewSubstr_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(count <= max_size\(\) - size\(\)\)");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<3> str({0, 1, 2});
    return str.append("34"sv, 1);
  }();
#endif  // PW_NC_TEST
}
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

// operator+=

TEST(InlineString, AppendOperator_BasicString) {
  TEST_STRING(InlineString<1>(), str.append(0, '1'), "");
  TEST_STRING(InlineString<1>(), str.append(1, '1'), "1");
  TEST_STRING(InlineString<10>(), str.append(2, '1'), "11");
  TEST_STRING(InlineString<10>(), str.append(10, '1'), "1111111111");

  TEST_STRING(InlineString<4>("Hi"), str.append(0, '!'), "Hi");
  TEST_STRING(InlineString<4>("Hi"), str.append(1, '!'), "Hi!");
  TEST_STRING(InlineString<6>("Hi"), str.append(2, '!'), "Hi!!");
  TEST_STRING(InlineString<6>("Hi"), str.append(4, '!'), "Hi!!!!");

#if PW_NC_TEST(AppendOperator_BasicString_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(count <= max_size\(\) - size\(\)\)");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<3> str({0, 1});
    return str.append(kSize5Capacity10);
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, AppendOperator_Character) {
  TEST_STRING(InlineString<1>(), fixed_str += '1', "1");
  TEST_STRING(InlineString<10>(), fixed_str += '\0', "\0");

  TEST_STRING(InlineString<3>("Hi"), fixed_str += '!', "Hi!");
  TEST_STRING(InlineString<10>("Hi"), fixed_str += '!', "Hi!");

#if PW_NC_TEST(AppendOperator_Characters_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(size\(\) < max_size\(\)\);");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<3> str({0, 1, 2});
    return str += '?';
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, AppendOperator_Array) {
  TEST_STRING(InlineString<1>(), fixed_str += "", "");
  TEST_STRING(InlineString<2>(), fixed_str += "a", "a");
  TEST_STRING(InlineString<6>(), fixed_str += "12345", "12345");

  TEST_STRING(InlineString<1>({'a'}), fixed_str += "", "a");
  TEST_STRING(InlineString<2>("a"), fixed_str += "a", "aa");
  TEST_STRING(InlineString<6>("a"), fixed_str += "12345", "a12345");

#if PW_NC_TEST(AppendOperator_Array_DoesNotFit)
  PW_NC_EXPECT(
      "InlineString's capacity is too small to hold the assigned string");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<3> str;
    return str += "1234";
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, AppendOperator_Pointer) {
  TEST_STRING(InlineString<0>(), fixed_str += kPointer0, "");
  TEST_STRING(InlineString<10>(), fixed_str += kPointer10, "9876543210");
  TEST_STRING(InlineString<10>("abc"), fixed_str += kPointer10 + 5, "abc43210");
  TEST_STRING(
      InlineString<13>("abc"), fixed_str += kPointer10, "abc9876543210");

#if PW_NC_TEST(AppendOperator_Pointer_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(count <= max_size\(\) - size\(\)\)");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<3> str({0, 1});
    return str.append(kPointer10 + 8);
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, AppendOperator_InitializerList) {
  TEST_STRING(InlineString<0>(), fixed_str += {}, "");
  TEST_STRING(InlineString<10>(), (fixed_str += {1, 2, 3}), "\1\2\3");
  TEST_STRING(InlineString<10>("abc"), (fixed_str += {1, 2, 3}), "abc\1\2\3");
  TEST_STRING(InlineString<5>("abc"), (fixed_str += {'4', '5'}), "abc45");

#if PW_NC_TEST(AppendOperator_InitializerList_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(count <= max_size\(\) - size\(\)\)");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<3> str({0, 1, 2});
    return str.append({3});
  }();
#endif  // PW_NC_TEST
}

#if PW_CXX_STANDARD_IS_SUPPORTED(17)
TEST(InlineString, AppendOperator_StringView) {
  TEST_STRING(InlineString<0>(), fixed_str += ""sv, "");
  TEST_STRING(InlineString<10>("a"), fixed_str += ""sv, "a");
  TEST_STRING(InlineString<10>("abc"), fixed_str += "123"sv, "abc123");
  TEST_STRING(InlineString<5>("abc"), fixed_str += "45"sv, "abc45");

#if PW_NC_TEST(AppendOperator_StringView_DoesNotFit)
  PW_NC_EXPECT("PW_ASSERT\(count <= max_size\(\) - size\(\)\)");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<3> str({0, 1, 2});
    return str.append("3"sv);
  }();
#endif  // PW_NC_TEST
}
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

TEST(InlineString, Compare) {
  EXPECT_EQ(InlineString<10>("abb").compare(InlineString<5>("abb")), 0);
  EXPECT_LT(InlineString<10>("abb").compare(InlineString<5>("bbb")), 0);
  EXPECT_LT(InlineString<10>("bb").compare(InlineString<5>("bbb")), 0);

#if PW_CXX_STANDARD_IS_SUPPORTED(17)
  static_assert(InlineString<10>("bbb").compare(InlineString<5>("bbb")) == 0,
                "equal");
  static_assert(InlineString<10>("abb").compare(InlineString<5>("bbb")) < 0,
                "less");
  static_assert(InlineString<10>("bbb").compare(InlineString<5>("abb")) > 0,
                "greater");

  static_assert(InlineString<10>("bb").compare(InlineString<5>("bbb")) < 0,
                "less");
  static_assert(InlineString<10>("bbb").compare(InlineString<5>("bb")) > 0,
                "greater");

  static_assert(InlineString<10>("bb").compare(InlineString<5>("abb")) > 0,
                "less");
  static_assert(InlineString<10>("abb").compare(InlineString<5>("bb")) < 0,
                "greater");

  static_assert(InlineString<5>("").compare(InlineString<5>("")) == 0, "equal");
  static_assert(InlineString<5>("").compare(InlineString<5>("abc")) < 0,
                "less");
  static_assert(InlineString<5>("abc").compare(InlineString<5>("")) > 0,
                "greater");

  constexpr InlineBasicString<unsigned long long, 3> kUllString1(
      {0, std::numeric_limits<unsigned long long>::max(), 0});
  constexpr InlineBasicString<unsigned long long, 3> kUllString2(
      {std::numeric_limits<unsigned long long>::max(), 0});

  static_assert(kUllString1.compare(kUllString1) == 0, "equal");
  static_assert(kUllString1.compare(kUllString2) < 0, "less");
  static_assert(kUllString2.compare(kUllString1) > 0, "greater");
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)
}

// TODO(b/239996007): Test other pw::InlineString functions:
//
//   - starts_with
//   - ends_with
//   - contains
//   - replace
//   - substr
//   - copy

TEST(InlineString, Resize) {
  TEST_STRING(InlineString<10>(), str.resize(0), "");
  TEST_STRING(InlineString<10>(), str.resize(5), "\0\0\0\0\0");
  TEST_STRING(InlineString<10>(), str.resize(10), "\0\0\0\0\0\0\0\0\0\0");
  TEST_STRING(InlineString<10>(), str.resize(0, 'a'), "");
  TEST_STRING(InlineString<10>(), str.resize(5, 'a'), "aaaaa");
  TEST_STRING(InlineString<10>(), str.resize(10, 'a'), "aaaaaaaaaa");

  TEST_STRING(InlineString<10>("ABCDE"), str.resize(0), "");
  TEST_STRING(InlineString<10>("ABCDE"), str.resize(4), "ABCD");
  TEST_STRING(InlineString<10>("ABCDE"), str.resize(5), "ABCDE");
  TEST_STRING(InlineString<10>("ABCDE"), str.resize(10), "ABCDE\0\0\0\0\0");
  TEST_STRING(InlineString<10>("ABCDE"), str.resize(0, 'a'), "");
  TEST_STRING(InlineString<10>("ABCDE"), str.resize(3, 'a'), "ABC");
  TEST_STRING(InlineString<10>("ABCDE"), str.resize(5, 'a'), "ABCDE");
  TEST_STRING(InlineString<10>("ABCDE"), str.resize(10, 'a'), "ABCDEaaaaa");

#if PW_NC_TEST(Resize_LargerThanCapacity)
  PW_NC_EXPECT("PW_ASSERT\(new_size <= max_size\(\)\)");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<4> str("123");
    str.resize(5);
    return str;
  }();
#endif  // PW_NC_TEST
}

TEST(InlineString, ResizeAndOverwrite) {
  TEST_STRING(InlineString<2>(),
              str.resize_and_overwrite([](char* out, size_t) {
                out[0] = '\0';
                out[1] = '?';
                return 2;
              }),
              "\0?");
  TEST_STRING(InlineString<10>("ABCDE"),
              str.resize_and_overwrite([](char* out, size_t size) {
                out[1] = '?';
                for (size_t i = 5; i < size; ++i) {
                  out[i] = static_cast<char>('0' + i);
                }
                return size - 1;  // chop off the last character
              }),
              "A?CDE5678");

#if PW_NC_TEST(ResizeAndOverwrite_LargerThanCapacity)
  PW_NC_EXPECT("PW_ASSERT\(static_cast<size_t>\(new_size\) <= max_size\(\)\)");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<4> str("123");
    str.resize_and_overwrite([](char*, size_t) { return 5; });
    return str;
  }();
#elif PW_NC_TEST(ResizeAndOverwrite_NegativeSize)
  PW_NC_EXPECT("PW_ASSERT\(static_cast<size_t>\(new_size\) <= max_size\(\)\)");
  [[maybe_unused]] constexpr auto fail = [] {
    InlineString<4> str("123");
    str.resize_and_overwrite([](char*, size_t) { return -1; });
    return str;
  }();
#endif  // PW_NC_TEST
}

// TODO(b/239996007): Test other pw::InlineString functions:
//   - swap

//
// Search
//

// TODO(b/239996007): Test search functions.

// TODO(b/239996007): Test operator+.

TEST(InlineString, ComparisonOperators_InlineString) {
  EXPECT_EQ(InlineString<10>("a"), InlineString<10>("a"));
  EXPECT_NE(InlineString<10>("a"), InlineString<10>("b"));
  EXPECT_LT(InlineString<10>("a"), InlineString<10>("b"));
  EXPECT_LE(InlineString<10>("a"), InlineString<10>("b"));
  EXPECT_GT(InlineString<10>("b"), InlineString<10>("a"));
  EXPECT_GE(InlineString<10>("b"), InlineString<10>("a"));

#if PW_CXX_STANDARD_IS_SUPPORTED(17)
  static_assert(InlineString<10>() == InlineString<10>(), "equal");  // NOLINT
  static_assert(InlineString<10>("abc") == InlineString<5>("abc"), "equal");
  static_assert(InlineString<1>({'a'}) == InlineString<10>("a"), "equal");
  static_assert(!(InlineString<10>("?") == InlineString<10>()),  // NOLINT
                "equal");

  static_assert(InlineString<10>() != InlineString<10>("a"),  // NOLINT
                "not equal");
  static_assert(InlineString<10>("") != InlineString<5>("abc"), "not equal");
  static_assert(InlineString<1>({'\0'}) != InlineString<10>(""), "not equal");
  static_assert(!(InlineString<1>({'\0'}) != InlineString<10>("\0"sv)),
                "not equal");

  static_assert(InlineString<10>() < InlineString<10>("a"), "less");
  static_assert(InlineString<10>("ab") < InlineString<5>("abc"), "less");
  static_assert(InlineString<1>({'\0'}) < InlineString<10>("\1\0"), "less");
  static_assert(!(InlineString<1>({'\2'}) < InlineString<10>("\1\0")), "less");

  static_assert(InlineString<10>() <= InlineString<10>("a"), "less equal");
  static_assert(InlineString<10>("a") <= InlineString<10>("a"), "less equal");
  static_assert(InlineString<10>("ab") <= InlineString<5>("abc"), "less equal");
  static_assert(InlineString<10>("abc") <= InlineString<5>("abc"),
                "less equal");
  static_assert(InlineString<1>({'\0'}) <= InlineString<10>("\1\0"),
                "less equal");
  static_assert(InlineString<2>({'\1', '\0'}) <= InlineString<10>("\1\0"sv),
                "less equal");
  static_assert(!(InlineString<2>({'\2', '\0'}) <= InlineString<10>("\1\0"sv)),
                "less equal");

  static_assert(InlineString<10>("?") > InlineString<10>(""), "greater");
  static_assert(InlineString<10>("abc") > InlineString<5>("ab"), "greater");
  static_assert(InlineString<2>({'\1', '\0'}) > InlineString<10>("\1"),
                "greater");
  static_assert(!(InlineString<2>({'\1', '\0'}) > InlineString<10>("\2")),
                "greater");

  static_assert(InlineString<10>("?") >= InlineString<10>(""), "greater equal");
  static_assert(InlineString<10>("?") >= InlineString<10>("?"),
                "greater equal");
  static_assert(InlineString<10>("abc") >= InlineString<5>("ab"),
                "greater equal");
  static_assert(InlineString<10>("abc") >= InlineString<5>("abc"),
                "greater equal");
  static_assert(InlineString<2>({'\1', '\0'}) >= InlineString<10>("\1"),
                "greater equal");
  static_assert(InlineString<2>({'\1', '\0'}) >= InlineString<10>("\1\0"),
                "greater equal");
  static_assert(!(InlineString<3>("\0\0") >= InlineString<10>("\1\0")),
                "greater equal");
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)
}

TEST(InlineString, ComparisonOperators_NullTerminatedString) {
  EXPECT_EQ(InlineString<10>("a"), "a");
  EXPECT_EQ("a", InlineString<10>("a"));

  EXPECT_NE(InlineString<10>("a"), "b");
  EXPECT_NE("a", InlineString<10>("b"));

  EXPECT_LT(InlineString<10>("a"), "b");
  EXPECT_LT("a", InlineString<10>("b"));

  EXPECT_LE(InlineString<10>("a"), "b");
  EXPECT_LE("a", InlineString<10>("b"));
  EXPECT_LE(InlineString<10>("a"), "a");
  EXPECT_LE("a", InlineString<10>("a"));

  EXPECT_GT(InlineString<10>("b"), "a");
  EXPECT_GT("b", InlineString<10>("a"));

  EXPECT_GE(InlineString<10>("b"), "a");
  EXPECT_GE("b", InlineString<10>("a"));
  EXPECT_GE(InlineString<10>("a"), "a");
  EXPECT_GE("a", InlineString<10>("a"));

#if PW_CXX_STANDARD_IS_SUPPORTED(17)
  static_assert(InlineString<10>() == "", "equal");  // NOLINT
  static_assert("" == InlineString<10>(), "equal");  // NOLINT
  static_assert(InlineString<10>("abc") == "abc", "equal");
  static_assert("abc" == InlineString<5>("abc"), "equal");

  static_assert("" != InlineString<10>("a"), "not equal");    // NOLINT
  static_assert(InlineString<10>("a") != "", "not equal");    // NOLINT
  static_assert(InlineString<10>("") != "abc", "not equal");  // NOLINT
  static_assert("" != InlineString<5>("abc"), "not equal");   // NOLINT

  static_assert(InlineString<10>() < "a", "less");
  static_assert("" < InlineString<10>("a"), "less");
  static_assert(InlineString<10>("ab") < "abc", "less");
  static_assert("ab" < InlineString<5>("abc"), "less");

  static_assert(InlineString<10>() <= "a", "less equal");
  static_assert("" <= InlineString<10>("a"), "less equal");
  static_assert(InlineString<10>("a") <= "a", "less equal");
  static_assert("a" <= InlineString<10>("a"), "less equal");

  static_assert(InlineString<10>("ab") <= "abc", "less equal");
  static_assert("ab" <= InlineString<5>("abc"), "less equal");
  static_assert(InlineString<10>("abc") <= "abc", "less equal");
  static_assert("abc" <= InlineString<5>("abc"), "less equal");

  static_assert(InlineString<10>("?") > "", "greater");
  static_assert("?" > InlineString<10>(""), "greater");
  static_assert(InlineString<10>("abc") > "ab", "greater");
  static_assert("abc" > InlineString<5>("ab"), "greater");

  static_assert(InlineString<10>("?") >= "", "greater equal");
  static_assert("?" >= InlineString<10>(""), "greater equal");
  static_assert(InlineString<10>("abc") >= "ab", "greater equal");
  static_assert("abc" >= InlineString<5>("ab"), "greater equal");
  static_assert(InlineString<10>("abc") >= "abc", "greater equal");
  static_assert("abc" >= InlineString<5>("abc"), "greater equal");
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)
}

// Test instantiating an InlineBasicString with different character types.

#if __cpp_constexpr >= 201603L  // constexpr lambdas are required
#define PW_STRING_WRAP_TEST_EXPANSION(expr) \
  do {                                      \
    expr;                                   \
  } while (0)
#else
#define PW_STRING_WRAP_TEST_EXPANSION(expr)
#endif  // __cpp_constexpr >= 201603L

#define TEST_FOR_TYPES(test_macro, ...)                                  \
  PW_STRING_WRAP_TEST_EXPANSION(test_macro(char, __VA_ARGS__));          \
  PW_STRING_WRAP_TEST_EXPANSION(test_macro(unsigned char, __VA_ARGS__)); \
  PW_STRING_WRAP_TEST_EXPANSION(test_macro(signed char, __VA_ARGS__));   \
  PW_STRING_WRAP_TEST_EXPANSION(test_macro(short, __VA_ARGS__));         \
  PW_STRING_WRAP_TEST_EXPANSION(test_macro(int, __VA_ARGS__));           \
  PW_STRING_WRAP_TEST_EXPANSION(test_macro(unsigned, __VA_ARGS__));      \
  PW_STRING_WRAP_TEST_EXPANSION(test_macro(long, __VA_ARGS__));          \
  PW_STRING_WRAP_TEST_EXPANSION(test_macro(long long, __VA_ARGS__))

TEST(BasicStrings, Empty) {
#define BASIC_STRINGS_EMPTY(type, capacity)                         \
  constexpr InlineBasicString<type, capacity> string;               \
  static_assert(string.empty(), "empty");                           \
  static_assert(string.size() == 0u, "size 0"); /* NOLINT */        \
  static_assert(string.c_str()[0] == static_cast<type>(0), "null"); \
  static_assert(std::basic_string_view<type>(string).empty())

  TEST_FOR_TYPES(BASIC_STRINGS_EMPTY, 0);
  TEST_FOR_TYPES(BASIC_STRINGS_EMPTY, 1);
  TEST_FOR_TYPES(BASIC_STRINGS_EMPTY, 50);

#undef BASIC_STRINGS_EMPTY
}

TEST(BasicStrings, InitializerList) {
#define BASIC_STRINGS_INITIALIZER_LIST(type, capacity)                 \
  constexpr InlineBasicString<type, capacity> string({0, 1, 2, 3, 4}); \
  static_assert(string.size() == 5u, "size 5");                        \
  static_assert(string[0] == static_cast<type>(0), "0");               \
  static_assert(string[1] == static_cast<type>(1), "1");               \
  static_assert(string[2] == static_cast<type>(2), "2");               \
  static_assert(string[3] == static_cast<type>(3), "3");               \
  static_assert(string[4] == static_cast<type>(4), "4");               \
  static_assert(string.c_str()[0] == static_cast<type>(0), "null");    \
  static_assert(std::basic_string_view<type>(string).size() == 5)

  TEST_FOR_TYPES(BASIC_STRINGS_INITIALIZER_LIST, 5);
  TEST_FOR_TYPES(BASIC_STRINGS_INITIALIZER_LIST, 10);
  TEST_FOR_TYPES(BASIC_STRINGS_INITIALIZER_LIST, 50);

#undef BASIC_STRINGS_INITIALIZER_LIST
}

TEST(BasicStrings, VariousOperations) {
#define BASIC_STRINGS_VARIOUS_OPERATIONS(type, capacity) \
  static constexpr type kOne[2] = {1, 0};                \
  constexpr auto string = [] {                           \
    InlineBasicString<type, capacity> str({0});          \
    str.append(kOne);                                    \
    str.append({2, 10, 99});                             \
    str.resize(3);                                       \
    str.push_back(static_cast<int>(3));                  \
    str.append(InlineBasicString<type, 2>({4}));         \
    return str;                                          \
  }();                                                   \
  static_assert(string.size() == 5);                     \
  static_assert(string[0] == static_cast<type>(0), "0"); \
  static_assert(string[1] == static_cast<type>(1), "1"); \
  static_assert(string[2] == static_cast<type>(2), "2"); \
  static_assert(string[3] == static_cast<type>(3), "3"); \
  static_assert(string[4] == static_cast<type>(4), "4"); \
  static_assert(string.c_str()[0] == static_cast<type>(0), "null")

  TEST_FOR_TYPES(BASIC_STRINGS_VARIOUS_OPERATIONS, 5);
  TEST_FOR_TYPES(BASIC_STRINGS_VARIOUS_OPERATIONS, 10);
  TEST_FOR_TYPES(BASIC_STRINGS_VARIOUS_OPERATIONS, 50);

#undef BASIC_STRINGS_VARIOUS_OPERATIONS
}

}  // namespace pw
