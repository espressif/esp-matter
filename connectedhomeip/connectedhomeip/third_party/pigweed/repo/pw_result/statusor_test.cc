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

// These tests are a modified version of the tests for absl::StatusOr:
// inclusive-language: disable
// https://github.com/abseil/abseil-cpp/blob/master/absl/status/statusor_test.cc
// inclusive-language: enable

#include <any>
#include <array>
#include <initializer_list>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include "gtest/gtest.h"
#include "pw_result/result.h"

namespace {

#define EXPECT_OK(expression) EXPECT_EQ(::pw::OkStatus(), expression)
#define ASSERT_OK(expression) ASSERT_EQ(::pw::OkStatus(), expression)

struct CopyDetector {
  CopyDetector() = default;
  explicit CopyDetector(int xx) : x(xx) {}
  CopyDetector(CopyDetector&& d) noexcept
      : x(d.x), copied(false), moved(true) {}
  CopyDetector(const CopyDetector& d) : x(d.x), copied(true), moved(false) {}
  CopyDetector& operator=(const CopyDetector& c) {
    x = c.x;
    copied = true;
    moved = false;
    return *this;
  }
  CopyDetector& operator=(CopyDetector&& c) noexcept {
    x = c.x;
    copied = false;
    moved = true;
    return *this;
  }
  int x = 0;
  bool copied = false;
  bool moved = false;
};

// Define custom macros instead of the CopyDetectorHas matcher.
#define EXPECT_COPY_DETECTOR_HAS(                       \
    value, expected_x, expected_moved, expected_copied) \
  EXPECT_EQ(value.x, expected_x);                       \
  EXPECT_EQ(value.moved, expected_moved);               \
  EXPECT_EQ(value.copied, expected_copied)

#define EXPECT_OK_AND_COPY_DETECTOR_HAS(                                      \
    statusor_expr, expected_x, expected_moved, expected_copied)               \
  do {                                                                        \
    auto&& temp_status_or = statusor_expr;                                    \
    ASSERT_EQ(::pw::OkStatus(), temp_status_or.status());                     \
    EXPECT_COPY_DETECTOR_HAS(                                                 \
        temp_status_or.value(), expected_x, expected_moved, expected_copied); \
  } while (0)

#define EXPECT_OK_AND_ANY_WITH_COPY_DETECTOR_HAS(                     \
    statusor_expr, expected_x, expected_moved, expected_copied)       \
  do {                                                                \
    auto&& temp_status_or = statusor_expr;                            \
    ASSERT_EQ(::pw::OkStatus(), temp_status_or.status());             \
    const auto& temp_any_value =                                      \
        std::any_cast<const CopyDetector&>(temp_status_or.value());   \
    EXPECT_COPY_DETECTOR_HAS(                                         \
        temp_any_value, expected_x, expected_moved, expected_copied); \
  } while (0)

class Base1 {
 public:
  virtual ~Base1() {}
  int pad;
};

class Base2 {
 public:
  virtual ~Base2() {}
  int yetotherpad;
};

class Derived : public Base1, public Base2 {
 public:
  ~Derived() override {}
  int evenmorepad;
};

class CopyNoAssign {
 public:
  explicit CopyNoAssign(int value) : foo(value) {}
  CopyNoAssign(const CopyNoAssign& other) : foo(other.foo) {}
  const CopyNoAssign& operator=(const CopyNoAssign&) = delete;

  int foo;
};

pw::Result<std::unique_ptr<int>> ReturnUniquePtr() {
  // Uses implicit constructor from T&&
  return std::make_unique<int>(0);
}

TEST(Result, ElementType) {
  static_assert(std::is_same<pw::Result<int>::value_type, int>());
  static_assert(std::is_same<pw::Result<char>::value_type, char>());
}

TEST(Result, TestMoveOnlyInitialization) {
  pw::Result<std::unique_ptr<int>> thing(ReturnUniquePtr());
  ASSERT_TRUE(thing.ok());
  EXPECT_EQ(0, **thing);
  int* previous = thing->get();

  thing = ReturnUniquePtr();
  EXPECT_TRUE(thing.ok());
  EXPECT_EQ(0, **thing);
  EXPECT_NE(previous, thing->get());
}

TEST(Result, TestMoveOnlyValueExtraction) {
  pw::Result<std::unique_ptr<int>> thing(ReturnUniquePtr());
  ASSERT_TRUE(thing.ok());
  std::unique_ptr<int> ptr = *std::move(thing);
  EXPECT_EQ(0, *ptr);

  thing = std::move(ptr);
  ptr = std::move(*thing);
  EXPECT_EQ(0, *ptr);
}

TEST(Result, TestMoveOnlyInitializationFromTemporaryByValueOrDie) {
  std::unique_ptr<int> ptr(*ReturnUniquePtr());
  EXPECT_EQ(0, *ptr);
}

TEST(Result, TestValueOrDieOverloadForConstTemporary) {
  static_assert(
      std::is_same<const int&&,
                   decltype(std::declval<const pw::Result<int>&&>().value())>(),
      "value() for const temporaries should return const T&&");
}

TEST(Result, TestMoveOnlyConversion) {
  pw::Result<std::unique_ptr<const int>> const_thing(ReturnUniquePtr());
  EXPECT_TRUE(const_thing.ok());
  EXPECT_EQ(0, **const_thing);

  // Test rvalue converting assignment
  const int* const_previous = const_thing->get();
  const_thing = ReturnUniquePtr();
  EXPECT_TRUE(const_thing.ok());
  EXPECT_EQ(0, **const_thing);
  EXPECT_NE(const_previous, const_thing->get());
}

TEST(Result, TestMoveOnlyVector) {
  // Check that pw::Result<MoveOnly> works in vector.
  std::vector<pw::Result<std::unique_ptr<int>>> vec;
  vec.push_back(ReturnUniquePtr());
  vec.resize(2);
  auto another_vec = std::move(vec);
  EXPECT_EQ(0, **another_vec[0]);
  EXPECT_EQ(pw::Status::Unknown(), another_vec[1].status());
}

TEST(Result, TestDefaultCtor) {
  pw::Result<int> thing;
  EXPECT_FALSE(thing.ok());
  EXPECT_EQ(thing.status().code(), pw::Status::Unknown().code());
}

TEST(Result, StatusCtorForwards) {
  pw::Status status = pw::Status::Internal();

  EXPECT_EQ(pw::Result<int>(status).status(), pw::Status::Internal());

  EXPECT_EQ(pw::Result<int>(std::move(status)).status(),
            pw::Status::Internal());
}

#define EXPECT_DEATH_OR_THROW(statement, status) \
  EXPECT_DEATH_IF_SUPPORTED(statement, status.str());

TEST(ResultDeathTest, TestDefaultCtorValue) {
  pw::Result<int> thing;
  EXPECT_DEATH_OR_THROW(thing.value(), pw::Status::Unknown());
  const pw::Result<int> thing2;
  EXPECT_DEATH_OR_THROW(thing2.value(), pw::Status::Unknown());
}

TEST(ResultDeathTest, TestValueNotOk) {
  pw::Result<int> thing(pw::Status::Cancelled());
  EXPECT_DEATH_OR_THROW(thing.value(), pw::Status::Cancelled());
}

TEST(ResultDeathTest, TestValueNotOkConst) {
  const pw::Result<int> thing(pw::Status::Unknown());
  EXPECT_DEATH_OR_THROW(thing.value(), pw::Status::Unknown());
}

TEST(ResultDeathTest, TestPointerDefaultCtorValue) {
  pw::Result<int*> thing;
  EXPECT_DEATH_OR_THROW(thing.value(), pw::Status::Unknown());
}

TEST(ResultDeathTest, TestPointerValueNotOk) {
  pw::Result<int*> thing(pw::Status::Cancelled());
  EXPECT_DEATH_OR_THROW(thing.value(), pw::Status::Cancelled());
}

TEST(ResultDeathTest, TestPointerValueNotOkConst) {
  const pw::Result<int*> thing(pw::Status::Cancelled());
  EXPECT_DEATH_OR_THROW(thing.value(), pw::Status::Cancelled());
}

#if GTEST_HAS_DEATH_TEST
TEST(ResultDeathTest, TestStatusCtorStatusOk) {
  EXPECT_DEBUG_DEATH(
      {
        // This will DCHECK
        pw::Result<int> thing(pw::OkStatus());
        // In optimized mode, we are actually going to get error::INTERNAL for
        // status here, rather than crashing, so check that.
        EXPECT_FALSE(thing.ok());
        EXPECT_EQ(thing.status().code(), pw::Status::Internal().code());
      },
      "An OK status is not a valid constructor argument");
}

TEST(ResultDeathTest, TestPointerStatusCtorStatusOk) {
  EXPECT_DEBUG_DEATH(
      {
        pw::Result<int*> thing(pw::OkStatus());
        // In optimized mode, we are actually going to get error::INTERNAL for
        // status here, rather than crashing, so check that.
        EXPECT_FALSE(thing.ok());
        EXPECT_EQ(thing.status().code(), pw::Status::Internal().code());
      },
      "An OK status is not a valid constructor argument");
}
#endif

TEST(Result, ValueAccessor) {
  const int kIntValue = 110;
  {
    pw::Result<int> status_or(kIntValue);
    EXPECT_EQ(kIntValue, status_or.value());
    EXPECT_EQ(kIntValue, std::move(status_or).value());
  }
  {
    pw::Result<CopyDetector> status_or(kIntValue);
    EXPECT_OK_AND_COPY_DETECTOR_HAS(status_or, kIntValue, false, false);
    CopyDetector copy_detector = status_or.value();
    EXPECT_COPY_DETECTOR_HAS(copy_detector, kIntValue, false, true);
    copy_detector = std::move(status_or).value();
    EXPECT_COPY_DETECTOR_HAS(copy_detector, kIntValue, true, false);
  }
}

TEST(Result, BadValueAccess) {
  const pw::Status kError = pw::Status::Cancelled();
  pw::Result<int> status_or(kError);
  EXPECT_DEATH_OR_THROW(status_or.value(), kError);
}

TEST(Result, TestStatusCtor) {
  pw::Result<int> thing(pw::Status::Cancelled());
  EXPECT_FALSE(thing.ok());
  EXPECT_EQ(thing.status().code(), pw::Status::Cancelled().code());
}

TEST(Result, TestValueCtor) {
  const int kI = 4;
  const pw::Result<int> thing(kI);
  EXPECT_TRUE(thing.ok());
  EXPECT_EQ(kI, *thing);
}

struct Foo {
  const int x;
  explicit Foo(int y) : x(y) {}
};

TEST(Result, InPlaceConstruction) {
  pw::Result<Foo> status_or(std::in_place, 10);
  ASSERT_TRUE(status_or.ok());
  EXPECT_EQ(status_or->x, 10);
}

struct InPlaceHelper {
  InPlaceHelper(std::initializer_list<int> xs, std::unique_ptr<int> yy)
      : x(xs), y(std::move(yy)) {}
  const std::vector<int> x;
  std::unique_ptr<int> y;
};

TEST(Result, InPlaceInitListConstruction) {
  pw::Result<InPlaceHelper> status_or(
      std::in_place, {10, 11, 12}, std::make_unique<int>(13));
  ASSERT_TRUE(status_or.ok());
  ASSERT_EQ(status_or->x.size(), 3u);
  EXPECT_EQ(status_or->x[0], 10);
  EXPECT_EQ(status_or->x[1], 11);
  EXPECT_EQ(status_or->x[2], 12);
  EXPECT_EQ(*(status_or->y), 13);
}

TEST(Result, Emplace) {
  pw::Result<Foo> status_or_foo(10);
  status_or_foo.emplace(20);

  ASSERT_TRUE(status_or_foo.ok());
  EXPECT_EQ(status_or_foo->x, 20);

  status_or_foo = pw::Status::InvalidArgument();
  EXPECT_FALSE(status_or_foo.ok());
  EXPECT_EQ(status_or_foo.status().code(),
            pw::Status::InvalidArgument().code());
  status_or_foo.emplace(20);
  ASSERT_TRUE(status_or_foo.ok());
  EXPECT_EQ(status_or_foo->x, 20);
}

TEST(Result, EmplaceInitializerList) {
  pw::Result<InPlaceHelper> status_or(
      std::in_place, {10, 11, 12}, std::make_unique<int>(13));
  status_or.emplace({1, 2, 3}, std::make_unique<int>(4));
  ASSERT_TRUE(status_or.ok());
  ASSERT_EQ(status_or->x.size(), 3u);
  EXPECT_EQ(status_or->x[0], 1);
  EXPECT_EQ(status_or->x[1], 2);
  EXPECT_EQ(status_or->x[2], 3);
  EXPECT_EQ(*(status_or->y), 4);

  status_or = pw::Status::InvalidArgument();
  EXPECT_FALSE(status_or.ok());
  EXPECT_EQ(status_or.status().code(), pw::Status::InvalidArgument().code());

  status_or.emplace({1, 2, 3}, std::make_unique<int>(4));
  ASSERT_TRUE(status_or.ok());
  ASSERT_EQ(status_or->x.size(), 3u);
  EXPECT_EQ(status_or->x[0], 1);
  EXPECT_EQ(status_or->x[1], 2);
  EXPECT_EQ(status_or->x[2], 3);
  EXPECT_EQ(*(status_or->y), 4);
}

TEST(Result, TestCopyCtorStatusOk) {
  const int kI = 4;
  const pw::Result<int> original(kI);
  const pw::Result<int> copy(original);
  EXPECT_OK(copy.status());
  EXPECT_EQ(*original, *copy);
}

TEST(Result, TestCopyCtorStatusNotOk) {
  pw::Result<int> original(pw::Status::Cancelled());
  pw::Result<int> copy(original);
  EXPECT_EQ(copy.status().code(), pw::Status::Cancelled().code());
}

TEST(Result, TestCopyCtorNonAssignable) {
  const int kI = 4;
  CopyNoAssign value(kI);
  pw::Result<CopyNoAssign> original(value);
  pw::Result<CopyNoAssign> copy(original);
  EXPECT_OK(copy.status());
  EXPECT_EQ(original->foo, copy->foo);
}

TEST(Result, TestCopyCtorStatusOKConverting) {
  const int kI = 4;
  pw::Result<int> original(kI);
  pw::Result<double> copy(original);
  EXPECT_OK(copy.status());
  EXPECT_EQ(*original, *copy);
}

TEST(Result, TestCopyCtorStatusNotOkConverting) {
  pw::Result<int> original(pw::Status::Cancelled());
  pw::Result<double> copy(original);
  EXPECT_EQ(copy.status(), original.status());
}

TEST(Result, TestAssignmentStatusOk) {
  // Copy assignmment
  {
    const auto p = std::make_shared<int>(17);
    pw::Result<std::shared_ptr<int>> source(p);

    pw::Result<std::shared_ptr<int>> target;
    target = source;

    ASSERT_TRUE(target.ok());
    EXPECT_OK(target.status());
    EXPECT_EQ(p, *target);

    ASSERT_TRUE(source.ok());
    EXPECT_OK(source.status());
    EXPECT_EQ(p, *source);
  }

  // Move asssignment
  {
    const auto p = std::make_shared<int>(17);
    pw::Result<std::shared_ptr<int>> source(p);

    pw::Result<std::shared_ptr<int>> target;
    target = std::move(source);

    ASSERT_TRUE(target.ok());
    EXPECT_OK(target.status());
    EXPECT_EQ(p, *target);

    ASSERT_TRUE(source.ok());  // NOLINT(bugprone-use-after-move)
    EXPECT_OK(source.status());
    EXPECT_EQ(nullptr, *source);
  }
}

TEST(Result, TestAssignmentStatusNotOk) {
  // Copy assignment
  {
    const pw::Status expected = pw::Status::Cancelled();
    pw::Result<int> source(expected);

    pw::Result<int> target;
    target = source;

    EXPECT_FALSE(target.ok());
    EXPECT_EQ(expected, target.status());

    EXPECT_FALSE(source.ok());
    EXPECT_EQ(expected, source.status());
  }

  // Move assignment
  {
    const pw::Status expected = pw::Status::Cancelled();
    pw::Result<int> source(expected);

    pw::Result<int> target;
    target = std::move(source);

    EXPECT_FALSE(target.ok());
    EXPECT_EQ(expected, target.status());

    EXPECT_FALSE(source.ok());  // NOLINT(bugprone-use-after-move)
    // absl::Status sets itself to INTERNAL when moved, but pw::Status does not.
    // EXPECT_EQ(source.status().code(), pw::Status::Internal().code());
  }
}

TEST(Result, TestAssignmentStatusOKConverting) {
  // Copy assignment
  {
    const int kI = 4;
    pw::Result<int> source(kI);

    pw::Result<double> target;
    target = source;

    ASSERT_TRUE(target.ok());
    EXPECT_OK(target.status());
    EXPECT_EQ(kI, *target);

    ASSERT_TRUE(source.ok());
    EXPECT_OK(source.status());
    EXPECT_EQ(kI, *source);
  }

  // Move assignment
  {
    const auto p = new int(17);
    pw::Result<std::unique_ptr<int>> source(p);

    pw::Result<std::shared_ptr<int>> target;
    target = std::move(source);

    ASSERT_TRUE(target.ok());
    EXPECT_OK(target.status());
    EXPECT_EQ(p, target->get());

    ASSERT_TRUE(source.ok());  // NOLINT(bugprone-use-after-move)
    EXPECT_OK(source.status());
    EXPECT_EQ(nullptr, source->get());
  }
}

// implicit_cast
template <class T>
struct type_identity {
  using type = T;
};

template <typename To>
constexpr To implicit_cast(typename type_identity<To>::type to) {
  return to;
}

struct A {
  int x;
};

struct ImplicitConstructibleFromA {
  int x;
  bool moved;
  ImplicitConstructibleFromA(const A& a)  // NOLINT
      : x(a.x), moved(false) {}
  ImplicitConstructibleFromA(A&& a)  // NOLINT
      : x(a.x), moved(true) {}
};

TEST(Result, ImplicitConvertingConstructor) {
  auto status_or = implicit_cast<pw::Result<ImplicitConstructibleFromA>>(
      pw::Result<A>(A{11}));
  ASSERT_OK(status_or.status());
  EXPECT_EQ(status_or->x, 11);
  EXPECT_TRUE(status_or->moved);

  pw::Result<A> a(A{12});
  auto status_or_2 = implicit_cast<pw::Result<ImplicitConstructibleFromA>>(a);
  ASSERT_OK(status_or_2.status());
  EXPECT_EQ(status_or_2->x, 12);
  EXPECT_FALSE(status_or_2->moved);
}

struct ExplicitConstructibleFromA {
  int x;
  bool moved;
  explicit ExplicitConstructibleFromA(const A& a) : x(a.x), moved(false) {}
  explicit ExplicitConstructibleFromA(A&& a) : x(a.x), moved(true) {}
};

TEST(Result, ExplicitConvertingConstructor) {
  EXPECT_FALSE(
      (std::is_convertible<const pw::Result<A>&,
                           pw::Result<ExplicitConstructibleFromA>>::value));
  EXPECT_FALSE(
      (std::is_convertible<pw::Result<A>&&,
                           pw::Result<ExplicitConstructibleFromA>>::value));
  auto a1 = pw::Result<ExplicitConstructibleFromA>(pw::Result<A>(A{11}));
  ASSERT_OK(a1.status());
  EXPECT_EQ(a1->x, 11);
  EXPECT_TRUE(a1->moved);

  pw::Result<A> a(A{12});
  auto a2 = pw::Result<ExplicitConstructibleFromA>(a);
  ASSERT_OK(a2.status());
  EXPECT_EQ(a2->x, 12);
  EXPECT_FALSE(a2->moved);
}

struct ImplicitConstructibleFromBool {
  ImplicitConstructibleFromBool(bool y) : x(y) {}  // NOLINT
  bool x = false;
};

struct ConvertibleToBool {
  explicit ConvertibleToBool(bool y) : x(y) {}
  operator bool() const { return x; }  // NOLINT
  bool x = false;
};

TEST(Result, ImplicitBooleanConstructionWithImplicitCasts) {
  auto a = pw::Result<bool>(pw::Result<ConvertibleToBool>(true));
  ASSERT_OK(a.status());
  EXPECT_TRUE(*a);

  auto b = pw::Result<bool>(pw::Result<ConvertibleToBool>(false));
  ASSERT_OK(b.status());
  EXPECT_FALSE(*b);

  auto c = pw::Result<ImplicitConstructibleFromBool>(pw::Result<bool>(false));
  ASSERT_OK(c.status());
  EXPECT_EQ(c->x, false);
  EXPECT_FALSE(
      (std::is_convertible<pw::Result<ConvertibleToBool>,
                           pw::Result<ImplicitConstructibleFromBool>>::value));
}

TEST(Result, BooleanConstructionWithImplicitCasts) {
  auto a = pw::Result<bool>(pw::Result<ConvertibleToBool>(true));
  ASSERT_OK(a.status());
  EXPECT_TRUE(*a);

  auto b = pw::Result<bool>(pw::Result<ConvertibleToBool>(false));
  ASSERT_OK(b.status());
  EXPECT_FALSE(*b);

  auto c = pw::Result<ImplicitConstructibleFromBool>{pw::Result<bool>(false)};
  ASSERT_OK(c.status());
  EXPECT_FALSE(c->x);

  auto d = pw::Result<ImplicitConstructibleFromBool>{
      pw::Result<bool>(pw::Status::InvalidArgument())};
  EXPECT_FALSE(d.ok());

  auto e = pw::Result<ImplicitConstructibleFromBool>{
      pw::Result<ConvertibleToBool>(ConvertibleToBool{false})};
  ASSERT_OK(e.status());
  EXPECT_FALSE(e->x);

  auto f = pw::Result<ImplicitConstructibleFromBool>{
      pw::Result<ConvertibleToBool>(pw::Status::InvalidArgument())};
  EXPECT_FALSE(f.ok());
}

TEST(Result, ConstImplicitCast) {
  auto a = implicit_cast<pw::Result<bool>>(pw::Result<const bool>(true));
  ASSERT_OK(a.status());
  EXPECT_TRUE(*a);
  auto b = implicit_cast<pw::Result<bool>>(pw::Result<const bool>(false));
  ASSERT_OK(b.status());
  EXPECT_FALSE(*b);
  auto c = implicit_cast<pw::Result<const bool>>(pw::Result<bool>(true));
  ASSERT_OK(c.status());
  EXPECT_TRUE(*c);
  auto d = implicit_cast<pw::Result<const bool>>(pw::Result<bool>(false));
  ASSERT_OK(d.status());
  EXPECT_FALSE(*d);
  auto e = implicit_cast<pw::Result<const std::string>>(
      pw::Result<std::string>("foo"));
  ASSERT_OK(e.status());
  EXPECT_EQ(*e, "foo");
  auto f = implicit_cast<pw::Result<std::string>>(
      pw::Result<const std::string>("foo"));
  ASSERT_OK(f.status());
  EXPECT_EQ(*f, "foo");
  auto g = implicit_cast<pw::Result<std::shared_ptr<const std::string>>>(
      pw::Result<std::shared_ptr<std::string>>(
          std::make_shared<std::string>("foo")));
  ASSERT_OK(g.status());
  EXPECT_EQ(*(*g), "foo");
}

TEST(Result, ConstExplicitConstruction) {
  auto a = pw::Result<bool>(pw::Result<const bool>(true));
  ASSERT_OK(a.status());
  EXPECT_TRUE(*a);
  auto b = pw::Result<bool>(pw::Result<const bool>(false));
  ASSERT_OK(b.status());
  EXPECT_FALSE(*b);
  auto c = pw::Result<const bool>(pw::Result<bool>(true));
  ASSERT_OK(c.status());
  EXPECT_TRUE(*c);
  auto d = pw::Result<const bool>(pw::Result<bool>(false));
  ASSERT_OK(d.status());
  EXPECT_FALSE(*d);
}

struct ExplicitConstructibleFromInt {
  int x;
  explicit ExplicitConstructibleFromInt(int y) : x(y) {}
};

TEST(Result, ExplicitConstruction) {
  auto a = pw::Result<ExplicitConstructibleFromInt>(10);
  ASSERT_OK(a.status());
  EXPECT_EQ(a->x, 10);
}

TEST(Result, ImplicitConstruction) {
  // Check implicit casting works.
  auto status_or =
      implicit_cast<pw::Result<std::variant<int, std::string>>>(10);
  ASSERT_OK(status_or.status());
  EXPECT_EQ(std::get<int>(*status_or), 10);
}

TEST(Result, ImplicitConstructionFromInitliazerList) {
  // Note: dropping the explicit std::initializer_list<int> is not supported
  // by pw::Result or std::optional.
  auto status_or = implicit_cast<pw::Result<std::vector<int>>>({{10, 20, 30}});
  ASSERT_OK(status_or.status());
  ASSERT_EQ(status_or->size(), 3u);
  EXPECT_EQ((*status_or)[0], 10);
  EXPECT_EQ((*status_or)[1], 20);
  EXPECT_EQ((*status_or)[2], 30);
}

TEST(Result, UniquePtrImplicitConstruction) {
  auto status_or = implicit_cast<pw::Result<std::unique_ptr<Base1>>>(
      std::make_unique<Derived>());
  ASSERT_OK(status_or.status());
  EXPECT_NE(status_or->get(), nullptr);
}

TEST(Result, NestedResultCopyAndMoveConstructorTests) {
  pw::Result<pw::Result<CopyDetector>> status_or = CopyDetector(10);
  pw::Result<pw::Result<CopyDetector>> status_error =
      pw::Status::InvalidArgument();
  ASSERT_OK(status_or.status());
  EXPECT_OK_AND_COPY_DETECTOR_HAS(*status_or, 10, true, false);
  pw::Result<pw::Result<CopyDetector>> a = status_or;
  EXPECT_OK_AND_COPY_DETECTOR_HAS(*a, 10, false, true);
  pw::Result<pw::Result<CopyDetector>> a_err = status_error;
  EXPECT_FALSE(a_err.ok());

  const pw::Result<pw::Result<CopyDetector>>& cref = status_or;
  pw::Result<pw::Result<CopyDetector>> b = cref;  // NOLINT
  ASSERT_OK(b.status());
  EXPECT_OK_AND_COPY_DETECTOR_HAS(*b, 10, false, true);
  const pw::Result<pw::Result<CopyDetector>>& cref_err = status_error;
  pw::Result<pw::Result<CopyDetector>> b_err = cref_err;  // NOLINT
  EXPECT_FALSE(b_err.ok());

  pw::Result<pw::Result<CopyDetector>> c = std::move(status_or);
  ASSERT_OK(c.status());
  EXPECT_OK_AND_COPY_DETECTOR_HAS(*c, 10, true, false);
  pw::Result<pw::Result<CopyDetector>> c_err = std::move(status_error);
  EXPECT_FALSE(c_err.ok());
}

TEST(Result, NestedResultCopyAndMoveAssignment) {
  pw::Result<pw::Result<CopyDetector>> status_or = CopyDetector(10);
  pw::Result<pw::Result<CopyDetector>> status_error =
      pw::Status::InvalidArgument();
  pw::Result<pw::Result<CopyDetector>> a;
  a = status_or;
  ASSERT_TRUE(a.ok());
  EXPECT_OK_AND_COPY_DETECTOR_HAS(*a, 10, false, true);
  a = status_error;
  EXPECT_FALSE(a.ok());

  const pw::Result<pw::Result<CopyDetector>>& cref = status_or;
  a = cref;
  ASSERT_TRUE(a.ok());
  EXPECT_OK_AND_COPY_DETECTOR_HAS(*a, 10, false, true);
  const pw::Result<pw::Result<CopyDetector>>& cref_err = status_error;
  a = cref_err;
  EXPECT_FALSE(a.ok());
  a = std::move(status_or);
  ASSERT_TRUE(a.ok());
  EXPECT_OK_AND_COPY_DETECTOR_HAS(*a, 10, true, false);
  a = std::move(status_error);
  EXPECT_FALSE(a.ok());
}

struct Copyable {
  Copyable() {}
  Copyable(const Copyable&) {}
  Copyable& operator=(const Copyable&) { return *this; }
};

struct MoveOnly {
  MoveOnly() {}
  MoveOnly(MoveOnly&&) {}
  MoveOnly& operator=(MoveOnly&&) { return *this; }
};

struct NonMovable {
  NonMovable() {}
  NonMovable(const NonMovable&) = delete;
  NonMovable(NonMovable&&) = delete;
  NonMovable& operator=(const NonMovable&) = delete;
  NonMovable& operator=(NonMovable&&) = delete;
};

TEST(Result, CopyAndMoveAbility) {
  EXPECT_TRUE(std::is_copy_constructible<Copyable>::value);
  EXPECT_TRUE(std::is_copy_assignable<Copyable>::value);
  EXPECT_TRUE(std::is_move_constructible<Copyable>::value);
  EXPECT_TRUE(std::is_move_assignable<Copyable>::value);
  EXPECT_FALSE(std::is_copy_constructible<MoveOnly>::value);
  EXPECT_FALSE(std::is_copy_assignable<MoveOnly>::value);
  EXPECT_TRUE(std::is_move_constructible<MoveOnly>::value);
  EXPECT_TRUE(std::is_move_assignable<MoveOnly>::value);
  EXPECT_FALSE(std::is_copy_constructible<NonMovable>::value);
  EXPECT_FALSE(std::is_copy_assignable<NonMovable>::value);
  EXPECT_FALSE(std::is_move_constructible<NonMovable>::value);
  EXPECT_FALSE(std::is_move_assignable<NonMovable>::value);
}

TEST(Result, ResultAnyCopyAndMoveConstructorTests) {
  pw::Result<std::any> status_or = CopyDetector(10);
  pw::Result<std::any> status_error = pw::Status::InvalidArgument();
  EXPECT_OK_AND_ANY_WITH_COPY_DETECTOR_HAS(status_or, 10, true, false);
  pw::Result<std::any> a = status_or;
  EXPECT_OK_AND_ANY_WITH_COPY_DETECTOR_HAS(a, 10, false, true);
  pw::Result<std::any> a_err = status_error;
  EXPECT_FALSE(a_err.ok());

  const pw::Result<std::any>& cref = status_or;
  // No lint for no-change copy.
  pw::Result<std::any> b = cref;  // NOLINT
  EXPECT_OK_AND_ANY_WITH_COPY_DETECTOR_HAS(b, 10, false, true);
  const pw::Result<std::any>& cref_err = status_error;
  // No lint for no-change copy.
  pw::Result<std::any> b_err = cref_err;  // NOLINT
  EXPECT_FALSE(b_err.ok());

  pw::Result<std::any> c = std::move(status_or);
  EXPECT_OK_AND_ANY_WITH_COPY_DETECTOR_HAS(c, 10, true, false);
  pw::Result<std::any> c_err = std::move(status_error);
  EXPECT_FALSE(c_err.ok());
}

TEST(Result, ResultAnyCopyAndMoveAssignment) {
  pw::Result<std::any> status_or = CopyDetector(10);
  pw::Result<std::any> status_error = pw::Status::InvalidArgument();
  pw::Result<std::any> a;
  a = status_or;
  EXPECT_OK_AND_ANY_WITH_COPY_DETECTOR_HAS(a, 10, false, true);
  a = status_error;
  EXPECT_FALSE(a.ok());

  const pw::Result<std::any>& cref = status_or;
  a = cref;
  EXPECT_OK_AND_ANY_WITH_COPY_DETECTOR_HAS(a, 10, false, true);
  const pw::Result<std::any>& cref_err = status_error;
  a = cref_err;
  EXPECT_FALSE(a.ok());
  a = std::move(status_or);
  EXPECT_OK_AND_ANY_WITH_COPY_DETECTOR_HAS(a, 10, true, false);
  a = std::move(status_error);
  EXPECT_FALSE(a.ok());
}

TEST(Result, ResultCopyAndMoveTestsConstructor) {
  pw::Result<CopyDetector> status_or(10);
  EXPECT_OK_AND_COPY_DETECTOR_HAS(status_or, 10, false, false);
  pw::Result<CopyDetector> a(status_or);
  EXPECT_OK_AND_COPY_DETECTOR_HAS(a, 10, false, true);
  const pw::Result<CopyDetector>& cref = status_or;
  pw::Result<CopyDetector> b(cref);  // NOLINT
  EXPECT_OK_AND_COPY_DETECTOR_HAS(b, 10, false, true);
  pw::Result<CopyDetector> c(std::move(status_or));
  EXPECT_OK_AND_COPY_DETECTOR_HAS(c, 10, true, false);
}

TEST(Result, ResultCopyAndMoveTestsAssignment) {
  pw::Result<CopyDetector> status_or(10);
  EXPECT_OK_AND_COPY_DETECTOR_HAS(status_or, 10, false, false);
  pw::Result<CopyDetector> a;
  a = status_or;
  EXPECT_OK_AND_COPY_DETECTOR_HAS(a, 10, false, true);
  const pw::Result<CopyDetector>& cref = status_or;
  pw::Result<CopyDetector> b;
  b = cref;
  EXPECT_OK_AND_COPY_DETECTOR_HAS(b, 10, false, true);
  pw::Result<CopyDetector> c;
  c = std::move(status_or);
  EXPECT_OK_AND_COPY_DETECTOR_HAS(c, 10, true, false);
}

TEST(Result, StdAnyAssignment) {
  EXPECT_FALSE(
      (std::is_assignable<pw::Result<std::any>, pw::Result<int>>::value));
  pw::Result<std::any> status_or;
  status_or = pw::Status::InvalidArgument();
  EXPECT_FALSE(status_or.ok());
}

TEST(Result, ImplicitAssignment) {
  pw::Result<std::variant<int, std::string>> status_or;
  status_or = 10;
  ASSERT_OK(status_or.status());
  EXPECT_EQ(std::get<int>(*status_or), 10);
}

TEST(Result, SelfDirectInitAssignment) {
  pw::Result<std::vector<int>> status_or = {{10, 20, 30}};
  status_or = *status_or;
  ASSERT_OK(status_or.status());
  ASSERT_EQ(status_or->size(), 3u);
  EXPECT_EQ((*status_or)[0], 10);
  EXPECT_EQ((*status_or)[1], 20);
  EXPECT_EQ((*status_or)[2], 30);
}

TEST(Result, ImplicitCastFromInitializerList) {
  pw::Result<std::vector<int>> status_or = {{10, 20, 30}};
  ASSERT_OK(status_or.status());
  ASSERT_EQ(status_or->size(), 3u);
  EXPECT_EQ((*status_or)[0], 10);
  EXPECT_EQ((*status_or)[1], 20);
  EXPECT_EQ((*status_or)[2], 30);
}

TEST(Result, UniquePtrImplicitAssignment) {
  pw::Result<std::unique_ptr<Base1>> status_or;
  status_or = std::make_unique<Derived>();
  ASSERT_OK(status_or.status());
  EXPECT_NE(status_or->get(), nullptr);
}

TEST(Result, Pointer) {
  struct Base {};
  struct B : public Base {};
  struct C : private Base {};

  EXPECT_TRUE((std::is_constructible<pw::Result<Base*>, B*>::value));
  EXPECT_TRUE((std::is_convertible<B*, pw::Result<Base*>>::value));
  EXPECT_FALSE((std::is_constructible<pw::Result<Base*>, C*>::value));
  EXPECT_FALSE((std::is_convertible<C*, pw::Result<Base*>>::value));
}

TEST(Result, TestAssignmentStatusNotOkConverting) {
  // Copy assignment
  {
    const pw::Status expected = pw::Status::Cancelled();
    pw::Result<int> source(expected);

    pw::Result<double> target;
    target = source;

    EXPECT_FALSE(target.ok());
    EXPECT_EQ(expected, target.status());

    EXPECT_FALSE(source.ok());
    EXPECT_EQ(expected, source.status());
  }

  // Move assignment
  {
    const pw::Status expected = pw::Status::Cancelled();
    pw::Result<int> source(expected);

    pw::Result<double> target;
    target = std::move(source);

    EXPECT_FALSE(target.ok());
    EXPECT_EQ(expected, target.status());

    EXPECT_FALSE(source.ok());  // NOLINT(bugprone-use-after-move)

    // absl::Status sets itself to INTERNAL when moved, but pw::Status does not.
    // EXPECT_EQ(source.status().code(), pw::Status::Internal().code());
  }
}

TEST(Result, SelfAssignment) {
  // Copy-assignment, status OK
  {
    // A string long enough that it's likely to defeat any inline representation
    // optimization.
    const std::string long_str(128, 'a');

    pw::Result<std::string> so = long_str;
    so = *&so;

    ASSERT_TRUE(so.ok());
    EXPECT_OK(so.status());
    EXPECT_EQ(long_str, *so);
  }

  // Copy-assignment, error status
  {
    pw::Result<int> so = pw::Status::NotFound();
    so = *&so;

    EXPECT_FALSE(so.ok());
    EXPECT_EQ(so.status().code(), pw::Status::NotFound().code());
  }

  // Move-assignment with copyable type, status OK
  {
    pw::Result<int> so = 17;

    // Fool the compiler, which otherwise complains.
    auto& same = so;
    so = std::move(same);

    ASSERT_TRUE(so.ok());
    EXPECT_OK(so.status());
    EXPECT_EQ(17, *so);
  }

  // Move-assignment with copyable type, error status
  {
    pw::Result<int> so = pw::Status::NotFound();

    // Fool the compiler, which otherwise complains.
    auto& same = so;
    so = std::move(same);

    EXPECT_FALSE(so.ok());
    EXPECT_EQ(so.status().code(), pw::Status::NotFound().code());
  }

  // Move-assignment with non-copyable type, status OK
  {
    const auto raw = new int(17);
    pw::Result<std::unique_ptr<int>> so = std::unique_ptr<int>(raw);

    // Fool the compiler, which otherwise complains.
    auto& same = so;
    so = std::move(same);

    ASSERT_TRUE(so.ok());
    EXPECT_OK(so.status());
    EXPECT_EQ(raw, so->get());
  }

  // Move-assignment with non-copyable type, error status
  {
    pw::Result<std::unique_ptr<int>> so = pw::Status::NotFound();

    // Fool the compiler, which otherwise complains.
    auto& same = so;
    so = std::move(same);

    EXPECT_FALSE(so.ok());
    EXPECT_EQ(so.status().code(), pw::Status::NotFound().code());
  }
}

// These types form the overload sets of the constructors and the assignment
// operators of `MockValue`. They distinguish construction from assignment,
// lvalue from rvalue.
struct FromConstructibleAssignableLvalue {};
struct FromConstructibleAssignableRvalue {};
struct FromImplicitConstructibleOnly {};
struct FromAssignableOnly {};

// This class is for testing the forwarding value assignments of `Result`.
// `from_rvalue` indicates whether the constructor or the assignment taking
// rvalue reference is called. `from_assignment` indicates whether any
// assignment is called.
struct MockValue {
  // Constructs `MockValue` from `FromConstructibleAssignableLvalue`.
  MockValue(const FromConstructibleAssignableLvalue&)  // NOLINT
      : from_rvalue(false), assigned(false) {}
  // Constructs `MockValue` from `FromConstructibleAssignableRvalue`.
  MockValue(FromConstructibleAssignableRvalue&&)  // NOLINT
      : from_rvalue(true), assigned(false) {}
  // Constructs `MockValue` from `FromImplicitConstructibleOnly`.
  // `MockValue` is not assignable from `FromImplicitConstructibleOnly`.
  MockValue(const FromImplicitConstructibleOnly&)  // NOLINT
      : from_rvalue(false), assigned(false) {}
  // Assigns `FromConstructibleAssignableLvalue`.
  MockValue& operator=(const FromConstructibleAssignableLvalue&) {
    from_rvalue = false;
    assigned = true;
    return *this;
  }
  // Assigns `FromConstructibleAssignableRvalue` (rvalue only).
  MockValue& operator=(FromConstructibleAssignableRvalue&&) {
    from_rvalue = true;
    assigned = true;
    return *this;
  }
  // Assigns `FromAssignableOnly`, but not constructible from
  // `FromAssignableOnly`.
  MockValue& operator=(const FromAssignableOnly&) {
    from_rvalue = false;
    assigned = true;
    return *this;
  }
  bool from_rvalue;
  bool assigned;
};

// operator=(U&&)
TEST(Result, PerfectForwardingAssignment) {
  // U == T
  constexpr int kValue1 = 10, kValue2 = 20;
  pw::Result<CopyDetector> status_or;
  CopyDetector lvalue(kValue1);
  status_or = lvalue;
  EXPECT_OK_AND_COPY_DETECTOR_HAS(status_or, kValue1, false, true);
  status_or = CopyDetector(kValue2);
  EXPECT_OK_AND_COPY_DETECTOR_HAS(status_or, kValue2, true, false);

  // U != T
  EXPECT_TRUE(
      (std::is_assignable<pw::Result<MockValue>&,
                          const FromConstructibleAssignableLvalue&>::value));
  EXPECT_TRUE((std::is_assignable<pw::Result<MockValue>&,
                                  FromConstructibleAssignableLvalue&&>::value));
  EXPECT_FALSE(
      (std::is_assignable<pw::Result<MockValue>&,
                          const FromConstructibleAssignableRvalue&>::value));
  EXPECT_TRUE((std::is_assignable<pw::Result<MockValue>&,
                                  FromConstructibleAssignableRvalue&&>::value));
  EXPECT_TRUE(
      (std::is_assignable<pw::Result<MockValue>&,
                          const FromImplicitConstructibleOnly&>::value));
  EXPECT_FALSE((std::is_assignable<pw::Result<MockValue>&,
                                   const FromAssignableOnly&>::value));

  pw::Result<MockValue> from_lvalue(FromConstructibleAssignableLvalue{});
  EXPECT_FALSE(from_lvalue->from_rvalue);
  EXPECT_FALSE(from_lvalue->assigned);
  from_lvalue = FromConstructibleAssignableLvalue{};
  EXPECT_FALSE(from_lvalue->from_rvalue);
  EXPECT_TRUE(from_lvalue->assigned);

  pw::Result<MockValue> from_rvalue(FromConstructibleAssignableRvalue{});
  EXPECT_TRUE(from_rvalue->from_rvalue);
  EXPECT_FALSE(from_rvalue->assigned);
  from_rvalue = FromConstructibleAssignableRvalue{};
  EXPECT_TRUE(from_rvalue->from_rvalue);
  EXPECT_TRUE(from_rvalue->assigned);

  pw::Result<MockValue> from_implicit_constructible(
      FromImplicitConstructibleOnly{});
  EXPECT_FALSE(from_implicit_constructible->from_rvalue);
  EXPECT_FALSE(from_implicit_constructible->assigned);
  // construct a temporary `Result` object and invoke the `Result` move
  // assignment operator.
  from_implicit_constructible = FromImplicitConstructibleOnly{};
  EXPECT_FALSE(from_implicit_constructible->from_rvalue);
  EXPECT_FALSE(from_implicit_constructible->assigned);
}

TEST(Result, TestStatus) {
  pw::Result<int> good(4);
  EXPECT_TRUE(good.ok());
  pw::Result<int> bad(pw::Status::Cancelled());
  EXPECT_FALSE(bad.ok());
  EXPECT_EQ(bad.status().code(), pw::Status::Cancelled().code());
}

TEST(Result, OperatorStarRefQualifiers) {
  static_assert(
      std::is_same<const int&,
                   decltype(*std::declval<const pw::Result<int>&>())>(),
      "Unexpected ref-qualifiers");
  static_assert(
      std::is_same<int&, decltype(*std::declval<pw::Result<int>&>())>(),
      "Unexpected ref-qualifiers");
  static_assert(
      std::is_same<const int&&,
                   decltype(*std::declval<const pw::Result<int>&&>())>(),
      "Unexpected ref-qualifiers");
  static_assert(
      std::is_same<int&&, decltype(*std::declval<pw::Result<int>&&>())>(),
      "Unexpected ref-qualifiers");
}

TEST(Result, OperatorStar) {
  const pw::Result<std::string> const_lvalue("hello");
  EXPECT_EQ("hello", *const_lvalue);

  pw::Result<std::string> lvalue("hello");
  EXPECT_EQ("hello", *lvalue);

  // Note: Recall that std::move() is equivalent to a static_cast to an rvalue
  // reference type.
  const pw::Result<std::string> const_rvalue("hello");
  EXPECT_EQ("hello", *std::move(const_rvalue));  // NOLINT

  pw::Result<std::string> rvalue("hello");
  EXPECT_EQ("hello", *std::move(rvalue));
}

TEST(Result, OperatorArrowQualifiers) {
  static_assert(
      std::is_same<
          const int*,
          decltype(std::declval<const pw::Result<int>&>().operator->())>(),
      "Unexpected qualifiers");
  static_assert(
      std::is_same<int*,
                   decltype(std::declval<pw::Result<int>&>().operator->())>(),
      "Unexpected qualifiers");
  static_assert(
      std::is_same<
          const int*,
          decltype(std::declval<const pw::Result<int>&&>().operator->())>(),
      "Unexpected qualifiers");
  static_assert(
      std::is_same<int*,
                   decltype(std::declval<pw::Result<int>&&>().operator->())>(),
      "Unexpected qualifiers");
}

TEST(Result, OperatorArrow) {
  const pw::Result<std::string> const_lvalue("hello");
  EXPECT_EQ(std::string("hello"), const_lvalue->c_str());

  pw::Result<std::string> lvalue("hello");
  EXPECT_EQ(std::string("hello"), lvalue->c_str());
}

TEST(Result, RValueStatus) {
  pw::Result<int> so(pw::Status::NotFound());
  const pw::Status s = std::move(so).status();

  EXPECT_EQ(s.code(), pw::Status::NotFound().code());

  // Check that !ok() still implies !status().ok(), even after moving out of the
  // object. See the note on the rvalue ref-qualified status method.
  EXPECT_FALSE(so.ok());  // NOLINT
  EXPECT_FALSE(so.status().ok());

  // absl::Status sets itself to INTERNAL when moved, but pw::Status does not.
  // EXPECT_EQ(so.status().code(), pw::Status::Internal().code());
}

TEST(Result, TestValue) {
  const int kI = 4;
  pw::Result<int> thing(kI);
  EXPECT_EQ(kI, *thing);
}

TEST(Result, TestValueConst) {
  const int kI = 4;
  const pw::Result<int> thing(kI);
  EXPECT_EQ(kI, *thing);
}

TEST(Result, TestPointerDefaultCtor) {
  pw::Result<int*> thing;
  EXPECT_FALSE(thing.ok());
  EXPECT_EQ(thing.status().code(), pw::Status::Unknown().code());
}

TEST(Result, TestPointerStatusCtor) {
  pw::Result<int*> thing(pw::Status::Cancelled());
  EXPECT_FALSE(thing.ok());
  EXPECT_EQ(thing.status().code(), pw::Status::Cancelled().code());
}

TEST(Result, TestPointerValueCtor) {
  const int kI = 4;

  // Construction from a non-null pointer
  {
    pw::Result<const int*> so(&kI);
    EXPECT_TRUE(so.ok());
    EXPECT_OK(so.status());
    EXPECT_EQ(&kI, *so);
  }

  // Construction from a null pointer constant
  {
    pw::Result<const int*> so(nullptr);
    EXPECT_TRUE(so.ok());
    EXPECT_OK(so.status());
    EXPECT_EQ(nullptr, *so);
  }

  // Construction from a non-literal null pointer
  {
    const int* const p = nullptr;

    pw::Result<const int*> so(p);
    EXPECT_TRUE(so.ok());
    EXPECT_OK(so.status());
    EXPECT_EQ(nullptr, *so);
  }
}

TEST(Result, TestPointerCopyCtorStatusOk) {
  const int kI = 0;
  pw::Result<const int*> original(&kI);
  pw::Result<const int*> copy(original);
  EXPECT_OK(copy.status());
  EXPECT_EQ(*original, *copy);
}

TEST(Result, TestPointerCopyCtorStatusNotOk) {
  pw::Result<int*> original(pw::Status::Cancelled());
  pw::Result<int*> copy(original);
  EXPECT_EQ(copy.status().code(), pw::Status::Cancelled().code());
}

TEST(Result, TestPointerCopyCtorStatusOKConverting) {
  Derived derived;
  pw::Result<Derived*> original(&derived);
  pw::Result<Base2*> copy(original);
  EXPECT_OK(copy.status());
  EXPECT_EQ(static_cast<const Base2*>(*original), *copy);
}

TEST(Result, TestPointerCopyCtorStatusNotOkConverting) {
  pw::Result<Derived*> original(pw::Status::Cancelled());
  pw::Result<Base2*> copy(original);
  EXPECT_EQ(copy.status().code(), pw::Status::Cancelled().code());
}

TEST(Result, TestPointerAssignmentStatusOk) {
  const int kI = 0;
  pw::Result<const int*> source(&kI);
  pw::Result<const int*> target;
  target = source;
  EXPECT_OK(target.status());
  EXPECT_EQ(*source, *target);
}

TEST(Result, TestPointerAssignmentStatusNotOk) {
  pw::Result<int*> source(pw::Status::Cancelled());
  pw::Result<int*> target;
  target = source;
  EXPECT_EQ(target.status().code(), pw::Status::Cancelled().code());
}

TEST(Result, TestPointerAssignmentStatusOKConverting) {
  Derived derived;
  pw::Result<Derived*> source(&derived);
  pw::Result<Base2*> target;
  target = source;
  EXPECT_OK(target.status());
  EXPECT_EQ(static_cast<const Base2*>(*source), *target);
}

TEST(Result, TestPointerAssignmentStatusNotOkConverting) {
  pw::Result<Derived*> source(pw::Status::Cancelled());
  pw::Result<Base2*> target;
  target = source;
  EXPECT_EQ(target.status(), source.status());
}

TEST(Result, TestPointerStatus) {
  const int kI = 0;
  pw::Result<const int*> good(&kI);
  EXPECT_TRUE(good.ok());
  pw::Result<const int*> bad(pw::Status::Cancelled());
  EXPECT_EQ(bad.status().code(), pw::Status::Cancelled().code());
}

TEST(Result, TestPointerValue) {
  const int kI = 0;
  pw::Result<const int*> thing(&kI);
  EXPECT_EQ(&kI, *thing);
}

TEST(Result, TestPointerValueConst) {
  const int kI = 0;
  const pw::Result<const int*> thing(&kI);
  EXPECT_EQ(&kI, *thing);
}

TEST(Result, ResultVectorOfUniquePointerCanReserveAndResize) {
  using EvilType = std::vector<std::unique_ptr<int>>;
  static_assert(std::is_copy_constructible<EvilType>::value);
  std::vector<::pw::Result<EvilType>> v(5);
  v.reserve(v.capacity() + 10);
  v.resize(v.capacity() + 10);
}

TEST(Result, ConstPayload) {
  // A reduced version of a problematic type found in the wild. All of the
  // operations below should compile.
  pw::Result<const int> a;

  // Copy-construction
  pw::Result<const int> b(a);

  // Copy-assignment
  EXPECT_FALSE(std::is_copy_assignable<pw::Result<const int>>::value);

  // Move-construction
  pw::Result<const int> c(std::move(a));

  // Move-assignment
  EXPECT_FALSE(std::is_move_assignable<pw::Result<const int>>::value);
}

TEST(Result, MapToResultUniquePtr) {
  // A reduced version of a problematic type found in the wild. All of the
  // operations below should compile.
  using MapType = std::map<std::string, pw::Result<std::unique_ptr<int>>>;

  MapType a;

  // Move-construction
  MapType b(std::move(a));

  // Move-assignment
  a = std::move(b);
}

TEST(Result, ValueOrOk) {
  const pw::Result<int> status_or = 0;
  EXPECT_EQ(status_or.value_or(-1), 0);
}

TEST(Result, ValueOrDefault) {
  const pw::Result<int> status_or = pw::Status::Cancelled();
  EXPECT_EQ(status_or.value_or(-1), -1);
}

TEST(Result, MoveOnlyValueOrOk) {
  pw::Result<std::unique_ptr<int>> status_or = std::make_unique<int>(0);
  ASSERT_TRUE(status_or.ok());
  auto value = std::move(status_or).value_or(std::make_unique<int>(-1));
  EXPECT_EQ(*value, 0);
}

TEST(Result, MoveOnlyValueOrDefault) {
  pw::Result<std::unique_ptr<int>> status_or(pw::Status::Cancelled());
  ASSERT_FALSE(status_or.ok());
  auto value = std::move(status_or).value_or(std::make_unique<int>(-1));
  EXPECT_EQ(*value, -1);
}

static pw::Result<int> MakeStatus() { return 100; }

TEST(Result, TestIgnoreError) { MakeStatus().IgnoreError(); }

TEST(Result, EqualityOperator) {
  constexpr int kNumCases = 4;
  std::array<pw::Result<int>, kNumCases> group1 = {
      pw::Result<int>(1),
      pw::Result<int>(2),
      pw::Result<int>(pw::Status::InvalidArgument()),
      pw::Result<int>(pw::Status::Internal())};
  std::array<pw::Result<int>, kNumCases> group2 = {
      pw::Result<int>(1),
      pw::Result<int>(2),
      pw::Result<int>(pw::Status::InvalidArgument()),
      pw::Result<int>(pw::Status::Internal())};
  for (int i = 0; i < kNumCases; ++i) {
    for (int j = 0; j < kNumCases; ++j) {
      if (i == j) {
        EXPECT_TRUE(group1[i] == group2[j]);
        EXPECT_FALSE(group1[i] != group2[j]);
      } else {
        EXPECT_FALSE(group1[i] == group2[j]);
        EXPECT_TRUE(group1[i] != group2[j]);
      }
    }
  }
}

struct MyType {
  bool operator==(const MyType&) const { return true; }
};

enum class ConvTraits { kNone = 0, kImplicit = 1, kExplicit = 2 };

// This class has conversion operator to `Result<T>` based on value of
// `conv_traits`.
template <typename T, ConvTraits conv_traits = ConvTraits::kNone>
struct ResultConversionBase {};

template <typename T>
struct ResultConversionBase<T, ConvTraits::kImplicit> {
  operator pw::Result<T>() const& {  // NOLINT
    return pw::Status::InvalidArgument();
  }
  operator pw::Result<T>() && {  // NOLINT
    return pw::Status::InvalidArgument();
  }
};

template <typename T>
struct ResultConversionBase<T, ConvTraits::kExplicit> {
  explicit operator pw::Result<T>() const& {
    return pw::Status::InvalidArgument();
  }
  explicit operator pw::Result<T>() && { return pw::Status::InvalidArgument(); }
};

// This class has conversion operator to `T` based on the value of
// `conv_traits`.
template <typename T, ConvTraits conv_traits = ConvTraits::kNone>
struct ConversionBase {};

template <typename T>
struct ConversionBase<T, ConvTraits::kImplicit> {
  operator T() const& { return t; }         // NOLINT
  operator T() && { return std::move(t); }  // NOLINT
  T t;
};

template <typename T>
struct ConversionBase<T, ConvTraits::kExplicit> {
  explicit operator T() const& { return t; }
  explicit operator T() && { return std::move(t); }
  T t;
};

// This class has conversion operator to `pw::Status` based on the value of
// `conv_traits`.
template <ConvTraits conv_traits = ConvTraits::kNone>
struct StatusConversionBase {};

template <>
struct StatusConversionBase<ConvTraits::kImplicit> {
  operator pw::Status() const& {  // NOLINT
    return pw::Status::Internal();
  }
  operator pw::Status() && {  // NOLINT
    return pw::Status::Internal();
  }
};

template <>
struct StatusConversionBase<ConvTraits::kExplicit> {
  explicit operator pw::Status() const& {  // NOLINT
    return pw::Status::Internal();
  }
  explicit operator pw::Status() && {  // NOLINT
    return pw::Status::Internal();
  }
};

static constexpr int kConvToStatus = 1;
static constexpr int kConvToResult = 2;
static constexpr int kConvToT = 4;
static constexpr int kConvExplicit = 8;

constexpr ConvTraits GetConvTraits(int bit, int config) {
  return (config & bit) == 0
             ? ConvTraits::kNone
             : ((config & kConvExplicit) == 0 ? ConvTraits::kImplicit
                                              : ConvTraits::kExplicit);
}

// This class conditionally has conversion operator to `pw::Status`, `T`,
// `Result<T>`, based on values of the template parameters.
template <typename T, int config>
struct CustomType
    : ResultConversionBase<T, GetConvTraits(kConvToResult, config)>,
      ConversionBase<T, GetConvTraits(kConvToT, config)>,
      StatusConversionBase<GetConvTraits(kConvToStatus, config)> {};

struct ConvertibleToAnyResult {
  template <typename T>
  operator pw::Result<T>() const {  // NOLINT
    return pw::Status::InvalidArgument();
  }
};

// Test the rank of overload resolution for `Result<T>` constructor and
// assignment, from highest to lowest:
// 1. T/Status
// 2. U that has conversion operator to pw::Result<T>
// 3. U that is convertible to Status
// 4. U that is convertible to T
TEST(Result, ConstructionFromT) {
  // Construct pw::Result<T> from T when T is convertible to
  // pw::Result<T>
  {
    ConvertibleToAnyResult v;
    pw::Result<ConvertibleToAnyResult> statusor(v);
    EXPECT_TRUE(statusor.ok());
  }
  {
    ConvertibleToAnyResult v;
    pw::Result<ConvertibleToAnyResult> statusor = v;
    EXPECT_TRUE(statusor.ok());
  }
  // Construct pw::Result<T> from T when T is explicitly convertible to
  // Status
  {
    CustomType<MyType, kConvToStatus | kConvExplicit> v;
    pw::Result<CustomType<MyType, kConvToStatus | kConvExplicit>> statusor(v);
    EXPECT_TRUE(statusor.ok());
  }
  {
    CustomType<MyType, kConvToStatus | kConvExplicit> v;
    pw::Result<CustomType<MyType, kConvToStatus | kConvExplicit>> statusor = v;
    EXPECT_TRUE(statusor.ok());
  }
}

// Construct pw::Result<T> from U when U is explicitly convertible to T
TEST(Result, ConstructionFromTypeConvertibleToT) {
  {
    CustomType<MyType, kConvToT | kConvExplicit> v;
    pw::Result<MyType> statusor(v);
    EXPECT_TRUE(statusor.ok());
  }
  {
    CustomType<MyType, kConvToT> v;
    pw::Result<MyType> statusor = v;
    EXPECT_TRUE(statusor.ok());
  }
}

// Construct pw::Result<T> from U when U has explicit conversion operator to
// pw::Result<T>
TEST(Result, ConstructionFromTypeWithConversionOperatorToResultT) {
  {
    CustomType<MyType, kConvToResult | kConvExplicit> v;
    pw::Result<MyType> statusor(v);
    EXPECT_EQ(statusor, v.operator pw::Result<MyType>());
  }
  {
    CustomType<MyType, kConvToT | kConvToResult | kConvExplicit> v;
    pw::Result<MyType> statusor(v);
    EXPECT_EQ(statusor, v.operator pw::Result<MyType>());
  }
  {
    CustomType<MyType, kConvToResult | kConvToStatus | kConvExplicit> v;
    pw::Result<MyType> statusor(v);
    EXPECT_EQ(statusor, v.operator pw::Result<MyType>());
  }
  {
    CustomType<MyType, kConvToT | kConvToResult | kConvToStatus | kConvExplicit>
        v;
    pw::Result<MyType> statusor(v);
    EXPECT_EQ(statusor, v.operator pw::Result<MyType>());
  }
  {
    CustomType<MyType, kConvToResult> v;
    pw::Result<MyType> statusor = v;
    EXPECT_EQ(statusor, v.operator pw::Result<MyType>());
  }
  {
    CustomType<MyType, kConvToT | kConvToResult> v;
    pw::Result<MyType> statusor = v;
    EXPECT_EQ(statusor, v.operator pw::Result<MyType>());
  }
  {
    CustomType<MyType, kConvToResult | kConvToStatus> v;
    pw::Result<MyType> statusor = v;
    EXPECT_EQ(statusor, v.operator pw::Result<MyType>());
  }
  {
    CustomType<MyType, kConvToT | kConvToResult | kConvToStatus> v;
    pw::Result<MyType> statusor = v;
    EXPECT_EQ(statusor, v.operator pw::Result<MyType>());
  }
}

TEST(Result, ConstructionFromTypeConvertibleToStatus) {
  // Construction fails because conversion to `Status` is explicit.
  {
    CustomType<MyType, kConvToStatus | kConvExplicit> v;
    pw::Result<MyType> statusor(v);
    EXPECT_FALSE(statusor.ok());
    EXPECT_EQ(statusor.status(), static_cast<pw::Status>(v));
  }
  {
    CustomType<MyType, kConvToT | kConvToStatus | kConvExplicit> v;
    pw::Result<MyType> statusor(v);
    EXPECT_FALSE(statusor.ok());
    EXPECT_EQ(statusor.status(), static_cast<pw::Status>(v));
  }
  {
    CustomType<MyType, kConvToStatus> v;
    pw::Result<MyType> statusor = v;
    EXPECT_FALSE(statusor.ok());
    EXPECT_EQ(statusor.status(), static_cast<pw::Status>(v));
  }
  {
    CustomType<MyType, kConvToT | kConvToStatus> v;
    pw::Result<MyType> statusor = v;
    EXPECT_FALSE(statusor.ok());
    EXPECT_EQ(statusor.status(), static_cast<pw::Status>(v));
  }
}

TEST(Result, AssignmentFromT) {
  // Assign to pw::Result<T> from T when T is convertible to
  // pw::Result<T>
  {
    ConvertibleToAnyResult v;
    pw::Result<ConvertibleToAnyResult> statusor;
    statusor = v;
    EXPECT_TRUE(statusor.ok());
  }
  // Assign to pw::Result<T> from T when T is convertible to Status
  {
    CustomType<MyType, kConvToStatus> v;
    pw::Result<CustomType<MyType, kConvToStatus>> statusor;
    statusor = v;
    EXPECT_TRUE(statusor.ok());
  }
}

TEST(Result, AssignmentFromTypeConvertibleToT) {
  // Assign to pw::Result<T> from U when U is convertible to T
  {
    CustomType<MyType, kConvToT> v;
    pw::Result<MyType> statusor;
    statusor = v;
    EXPECT_TRUE(statusor.ok());
  }
}

TEST(Result, AssignmentFromTypeWithConversionOperatortoResultT) {
  // Assign to pw::Result<T> from U when U has conversion operator to
  // pw::Result<T>
  {
    CustomType<MyType, kConvToResult> v;
    pw::Result<MyType> statusor;
    statusor = v;
    EXPECT_EQ(statusor, v.operator pw::Result<MyType>());
  }
  {
    CustomType<MyType, kConvToT | kConvToResult> v;
    pw::Result<MyType> statusor;
    statusor = v;
    EXPECT_EQ(statusor, v.operator pw::Result<MyType>());
  }
  {
    CustomType<MyType, kConvToResult | kConvToStatus> v;
    pw::Result<MyType> statusor;
    statusor = v;
    EXPECT_EQ(statusor, v.operator pw::Result<MyType>());
  }
  {
    CustomType<MyType, kConvToT | kConvToResult | kConvToStatus> v;
    pw::Result<MyType> statusor;
    statusor = v;
    EXPECT_EQ(statusor, v.operator pw::Result<MyType>());
  }
}

TEST(Result, AssignmentFromTypeConvertibleToStatus) {
  // Assign to pw::Result<T> from U when U is convertible to Status
  {
    CustomType<MyType, kConvToStatus> v;
    pw::Result<MyType> statusor;
    statusor = v;
    EXPECT_FALSE(statusor.ok());
    EXPECT_EQ(statusor.status(), static_cast<pw::Status>(v));
  }
  {
    CustomType<MyType, kConvToT | kConvToStatus> v;
    pw::Result<MyType> statusor;
    statusor = v;
    EXPECT_FALSE(statusor.ok());
    EXPECT_EQ(statusor.status(), static_cast<pw::Status>(v));
  }
}

}  // namespace
