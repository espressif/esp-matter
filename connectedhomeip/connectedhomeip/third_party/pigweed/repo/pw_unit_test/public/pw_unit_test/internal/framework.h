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

// The Pigweed unit test framework requires C++17 to use its full functionality.
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>

#include "pw_polyfill/standard.h"
#include "pw_preprocessor/compiler.h"
#include "pw_preprocessor/util.h"
#include "pw_span/span.h"
#include "pw_unit_test/config.h"
#include "pw_unit_test/event_handler.h"

#if PW_CXX_STANDARD_IS_SUPPORTED(17)
#include <string_view>

#include "pw_string/string_builder.h"
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

#define GTEST_TEST(test_suite_name, test_name)                           \
  _PW_TEST_SUITE_NAMES_MUST_BE_UNIQUE(void /* TEST */, test_suite_name); \
  _PW_TEST(test_suite_name, test_name, ::pw::unit_test::internal::Test)

// TEST() is a pretty generic macro name which could conflict with other code.
// If GTEST_DONT_DEFINE_TEST is set, don't alias GTEST_TEST to TEST.
#if !(defined(GTEST_DONT_DEFINE_TEST) && GTEST_DONT_DEFINE_TEST)
#define TEST(test_suite_name, test_name) GTEST_TEST(test_suite_name, test_name)
#endif  // !GTEST_DONT_DEFINE_TEST

#define TEST_F(test_fixture, test_name)                                \
  _PW_TEST_SUITE_NAMES_MUST_BE_UNIQUE(int /* TEST_F */, test_fixture); \
  _PW_TEST(test_fixture, test_name, test_fixture)

#define EXPECT_TRUE(expr) static_cast<void>(_PW_TEST_BOOL(expr, true))
#define EXPECT_FALSE(expr) static_cast<void>(_PW_TEST_BOOL(expr, false))
#define EXPECT_EQ(lhs, rhs) static_cast<void>(_PW_TEST_OP(lhs, rhs, ==))
#define EXPECT_NE(lhs, rhs) static_cast<void>(_PW_TEST_OP(lhs, rhs, !=))
#define EXPECT_GT(lhs, rhs) static_cast<void>(_PW_TEST_OP(lhs, rhs, >))
#define EXPECT_GE(lhs, rhs) static_cast<void>(_PW_TEST_OP(lhs, rhs, >=))
#define EXPECT_LT(lhs, rhs) static_cast<void>(_PW_TEST_OP(lhs, rhs, <))
#define EXPECT_LE(lhs, rhs) static_cast<void>(_PW_TEST_OP(lhs, rhs, <=))
#define EXPECT_STREQ(lhs, rhs) static_cast<void>(_PW_TEST_C_STR(lhs, rhs, ==))
#define EXPECT_STRNE(lhs, rhs) static_cast<void>(_PW_TEST_C_STR(lhs, rhs, !=))

#define ASSERT_TRUE(expr) _PW_TEST_ASSERT(_PW_TEST_BOOL(expr, true))
#define ASSERT_FALSE(expr) _PW_TEST_ASSERT(_PW_TEST_BOOL(expr, false))
#define ASSERT_EQ(lhs, rhs) _PW_TEST_ASSERT(_PW_TEST_OP(lhs, rhs, ==))
#define ASSERT_NE(lhs, rhs) _PW_TEST_ASSERT(_PW_TEST_OP(lhs, rhs, !=))
#define ASSERT_GT(lhs, rhs) _PW_TEST_ASSERT(_PW_TEST_OP(lhs, rhs, >))
#define ASSERT_GE(lhs, rhs) _PW_TEST_ASSERT(_PW_TEST_OP(lhs, rhs, >=))
#define ASSERT_LT(lhs, rhs) _PW_TEST_ASSERT(_PW_TEST_OP(lhs, rhs, <))
#define ASSERT_LE(lhs, rhs) _PW_TEST_ASSERT(_PW_TEST_OP(lhs, rhs, <=))
#define ASSERT_STREQ(lhs, rhs) _PW_TEST_ASSERT(_PW_TEST_C_STR(lhs, rhs, ==))
#define ASSERT_STRNE(lhs, rhs) _PW_TEST_ASSERT(_PW_TEST_C_STR(lhs, rhs, !=))

// Generates a non-fatal failure with a generic message.
#define ADD_FAILURE()                                                  \
  ::pw::unit_test::internal::Framework::Get().CurrentTestExpectSimple( \
      "(line is not executed)", "(line was executed)", __LINE__, false)

// Generates a fatal failure with a generic message.
#define GTEST_FAIL() return ADD_FAILURE()

// Skips test at runtime, which is neither successful nor failed. Skip aborts
// current function.
#define GTEST_SKIP() \
  return ::pw::unit_test::internal::Framework::Get().CurrentTestSkip(__LINE__)

// Define either macro to 1 to omit the definition of FAIL(), which is a
// generic name and clashes with some other libraries.
#if !(defined(GTEST_DONT_DEFINE_FAIL) && GTEST_DONT_DEFINE_FAIL)
#define FAIL() GTEST_FAIL()
#endif  // !GTEST_DONT_DEFINE_FAIL

// Generates a success with a generic message.
#define GTEST_SUCCEED()                                                \
  ::pw::unit_test::internal::Framework::Get().CurrentTestExpectSimple( \
      "(success)", "(success)", __LINE__, true)

// Define either macro to 1 to omit the definition of SUCCEED(), which
// is a generic name and clashes with some other libraries.
#if !(defined(GTEST_DONT_DEFINE_SUCCEED) && GTEST_DONT_DEFINE_SUCCEED)
#define SUCCEED() GTEST_SUCCEED()
#endif  // !GTEST_DONT_DEFINE_SUCCEED

// pw_unit_test framework entry point. Runs every registered test case and
// dispatches the results through the event handler. Returns a status of zero
// if all tests passed, or nonzero if there were any failures.
// This is compatible with GoogleTest.
//
// In order to receive test output, an event handler must be registered before
// this is called:
//
//   int main() {
//     MyEventHandler handler;
//     pw::unit_test::RegisterEventHandler(&handler);
//     return RUN_ALL_TESTS();
//   }
//
#define RUN_ALL_TESTS() \
  ::pw::unit_test::internal::Framework::Get().RunAllTests()

// Death tests are not supported. The *_DEATH_IF_SUPPORTED macros do nothing.
#define GTEST_HAS_DEATH_TEST 0

#define EXPECT_DEATH_IF_SUPPORTED(statement, regex) \
  if (0) {                                          \
    static_cast<void>(statement);                   \
    static_cast<void>(regex);                       \
  }                                                 \
  static_assert(true, "Macros must be terminated with a semicolon")

#define ASSERT_DEATH_IF_SUPPORTED(statement, regex) \
  EXPECT_DEATH_IF_SUPPORTED(statement, regex)

namespace pw {

#if PW_CXX_STANDARD_IS_SUPPORTED(17)

namespace string {

// This function is used to print unknown types that are used in EXPECT or
// ASSERT statements in tests.
//
// You can add support for displaying custom types by defining a ToString
// template specialization. For example:
//
//   namespace pw {
//
//   template <>
//   StatusWithSize ToString<MyType>(const MyType& value,
//                                   span<char> buffer) {
//     return string::Format("<MyType|%d>", value.id);
//   }
//
//   }  // namespace pw
//
// See the documentation in pw_string/string_builder.h for more information.
template <typename T>
StatusWithSize UnknownTypeToString(const T& value, span<char> buffer) {
  StringBuilder sb(buffer);
  sb << '<' << sizeof(value) << "-byte object at 0x" << &value << '>';
  return sb.status_with_size();
}

}  // namespace string

#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

namespace unit_test {

// Sets the event handler for a test run. Must be called before RUN_ALL_TESTS()
// to receive test output.
void RegisterEventHandler(EventHandler* event_handler);

namespace internal {

class Test;
class TestInfo;

// Used to tag arguments to EXPECT_STREQ/EXPECT_STRNE so they are treated like C
// strings rather than pointers.
struct CStringArg {
  const char* const c_str;
};

// Singleton test framework class responsible for managing and running test
// cases. This implementation is internal to Pigweed test; free functions
// wrapping its functionality are exposed as the public interface.
class Framework {
 public:
  constexpr Framework()
      : current_test_(nullptr),
        current_result_(TestResult::kSuccess),
        run_tests_summary_{.passed_tests = 0,
                           .failed_tests = 0,
                           .skipped_tests = 0,
                           .disabled_tests = 0},
        exit_status_(0),
        event_handler_(nullptr),
        memory_pool_() {}

  static Framework& Get() { return framework_; }

  // Registers a single test case with the framework. The framework owns the
  // registered unit test. Called during static initialization.
  void RegisterTest(TestInfo* test) const;

  // Sets the handler to which the framework dispatches test events. During a
  // test run, the framework owns the event handler.
  void RegisterEventHandler(EventHandler* event_handler) {
    event_handler_ = event_handler;
  }

  // Runs all registered test cases, returning a status of 0 if all succeeded or
  // nonzero if there were any failures. Test events that occur during the run
  // are sent to the registered event handler, if any.
  int RunAllTests();

#if PW_CXX_STANDARD_IS_SUPPORTED(17)
  // Only run test suites whose names are included in the provided list during
  // the next test run. This is C++17 only; older versions of C++ will run all
  // non-disabled tests.
  void SetTestSuitesToRun(span<std::string_view> test_suites) {
    test_suites_to_run_ = test_suites;
  }
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

  bool ShouldRunTest(const TestInfo& test_info) const;

  // Whether the current test is skipped.
  bool IsSkipped() const { return current_result_ == TestResult::kSkipped; }

  // Constructs an instance of a unit test class and runs the test.
  //
  // Tests are constructed within a static memory pool at run time instead of
  // being statically allocated to avoid blowing up the size of the test binary
  // in cases where users have large test fixtures (e.g. containing buffers)
  // reused many times. Instead, only a small, fixed-size TestInfo struct is
  // statically allocated per test case, with a run() function that references
  // this method instantiated for its test class.
  template <typename TestInstance>
  static void CreateAndRunTest(const TestInfo& test_info) {
    static_assert(
        sizeof(TestInstance) <= sizeof(memory_pool_),
        "The test memory pool is too small for this test. Either increase "
        "PW_UNIT_TEST_CONFIG_MEMORY_POOL_SIZE or decrease the size of your "
        "test fixture.");

    Framework& framework = Get();
    framework.StartTest(test_info);

    // Reset the memory pool to a marker value to help detect use of
    // uninitialized memory.
    std::memset(&framework.memory_pool_, 0xa5, sizeof(framework.memory_pool_));

    // Construct the test object within the static memory pool. The StartTest
    // function has already been called by the TestInfo at this point.
    TestInstance* test_instance = new (&framework.memory_pool_) TestInstance;
    test_instance->PigweedTestRun();

    // Manually call the destructor as it is not called automatically for
    // objects constructed using placement new.
    test_instance->~TestInstance();

    framework.EndCurrentTest();
  }

  // Runs an expectation function for the currently active test case.
  template <typename Expectation, typename Lhs, typename Rhs>
  bool CurrentTestExpect(Expectation expectation,
                         const Lhs& lhs,
                         const Rhs& rhs,
                         [[maybe_unused]] const char* expectation_string,
                         const char* expression,
                         int line) {
    // Size of the buffer into which to write the string with the evaluated
    // version of the arguments. This buffer is allocated on the unit test's
    // stack, so it shouldn't be too large.
    // TODO(hepler): Make this configurable.
    [[maybe_unused]] constexpr size_t kExpectationBufferSizeBytes = 128;

    const bool success = expectation(lhs, rhs);
    CurrentTestExpectSimple(
        expression,
#if PW_CXX_STANDARD_IS_SUPPORTED(17)
        MakeString<kExpectationBufferSizeBytes>(ConvertForPrint(lhs),
                                                ' ',
                                                expectation_string,
                                                ' ',
                                                ConvertForPrint(rhs))
            .c_str(),
#else
        "(evaluation requires C++17)",
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)
        line,
        success);
    return success;
  }

  // Skips the current test and dispatches an event for it.
  void CurrentTestSkip(int line);

  // Dispatches an event indicating the result of an expectation.
  void CurrentTestExpectSimple(const char* expression,
                               const char* evaluated_expression,
                               int line,
                               bool success);

 private:
  // Convert char* to void* so that they are printed as pointers instead of
  // strings in EXPECT_EQ and other macros. EXPECT_STREQ wraps its pointers in a
  // CStringArg so its pointers are treated like C strings.
  static constexpr const void* ConvertForPrint(const char* str) { return str; }

  static constexpr const void* ConvertForPrint(char* str) { return str; }

  static constexpr const char* ConvertForPrint(CStringArg value) {
    return value.c_str;
  }

  template <typename T>
  static constexpr T ConvertForPrint(T&& value) {
    return std::forward<T>(value);
  }

  // Sets current_test_ and dispatches an event indicating that a test started.
  void StartTest(const TestInfo& test);

  // Dispatches event indicating that a test finished and clears current_test_.
  void EndCurrentTest();

  // Singleton instance of the framework class.
  static Framework framework_;

  // Linked list of all registered test cases. This is static as it tests are
  // registered using static initialization.
  static TestInfo* tests_;

  // The current test case which is running.
  const TestInfo* current_test_;

  // Overall result of the current test case (pass/fail/skip).
  TestResult current_result_;

  // Overall result of the ongoing test run, which covers multiple tests.
  RunTestsSummary run_tests_summary_;

  // Program exit status returned by RunAllTests for GoogleTest compatibility.
  int exit_status_;

  // Handler to which to dispatch test events.
  EventHandler* event_handler_;

#if PW_CXX_STANDARD_IS_SUPPORTED(17)
  span<std::string_view> test_suites_to_run_;
#else
  span<const char*> test_suites_to_run_;  // Always empty in C++14.
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

  std::aligned_storage_t<config::kMemoryPoolSize, alignof(std::max_align_t)>
      memory_pool_;
};

// Information about a single test case, including a pointer to a function which
// constructs and runs the test class. These are statically allocated instead of
// the test classes, as test classes can be very large.
class TestInfo {
 public:
  TestInfo(const char* const test_suite_name,
           const char* const test_name,
           const char* const file_name,
           void (*run_func)(const TestInfo&))
      : test_case_{
        .suite_name = test_suite_name,
        .test_name = test_name,
        .file_name = file_name,
       }, run_(run_func) {
    Framework::Get().RegisterTest(this);
  }

  // The name of the suite to which the test case belongs, the name of the test
  // case itself, and the path to the file in which the test case is located.
  const TestCase& test_case() const { return test_case_; }

  bool enabled() const;

  void run() const { run_(*this); }

  TestInfo* next() const { return next_; }
  void set_next(TestInfo* next) { next_ = next; }

 private:
  TestCase test_case_;

  // Function which runs the test case. Refers to Framework::CreateAndRunTest
  // instantiated for the test case's class.
  void (*run_)(const TestInfo&);

  // TestInfo structs are registered with the test framework and stored as a
  // linked list.
  TestInfo* next_ = nullptr;
};

// Base class for all test cases or custom test fixtures.
// Every unit test created using the TEST or TEST_F macro defines a class that
// inherits from this (or a subclass of this).
//
// For example, given the following test definition:
//
//   TEST(MyTest, SaysHello) {
//     ASSERT_STREQ(SayHello(), "Hello, world!");
//   }
//
// A new class is defined for the test, e.g. MyTest_SaysHello_Test. This class
// inherits from the Test class and implements its PigweedTestBody function with
// the block provided to the TEST macro.
class Test {
 public:
  Test(const Test&) = delete;
  Test& operator=(const Test&) = delete;

  virtual ~Test() = default;

  // Runs the unit test.
  void PigweedTestRun() {
    SetUp();
    // TODO(deymo): Skip the test body if there's a fatal error in SetUp().
    if (!Framework::Get().IsSkipped()) {
      PigweedTestBody();
    }
    TearDown();
  }

 protected:
  Test() = default;

  // Called immediately before executing the test body.
  //
  // Setup and cleanup can typically be done in the test fixture's constructor
  // and destructor, but there are cases where SetUp/TearDown must be used
  // instead. See the Google Test documentation for more information.
  virtual void SetUp() {}

  // Called immediately after executing the test body.
  virtual void TearDown() {}

 private:
  friend class internal::Framework;

  // The user-provided body of the test case. Populated by the TEST macro.
  virtual void PigweedTestBody() = 0;
};

// Checks that a test suite name is valid.
constexpr bool HasNoUnderscores(const char* suite) {
  const char* disabled_prefix = "DISABLED_";

  for (; *suite != '\0'; ++suite) {
    if (*suite == *disabled_prefix) {
      disabled_prefix += 1;
    } else {
      disabled_prefix = "";
      if (*suite == '_') {
        return false;
      }
    }
  }
  return true;
}

}  // namespace internal

#if PW_CXX_STANDARD_IS_SUPPORTED(17)
inline void SetTestSuitesToRun(span<std::string_view> test_suites) {
  internal::Framework::Get().SetTestSuitesToRun(test_suites);
}
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

}  // namespace unit_test
}  // namespace pw

#define _PW_TEST(test_suite_name, test_name, parent_class)                     \
  static_assert(sizeof(#test_suite_name) > 1,                                  \
                "The test suite name must not be empty");                      \
  static_assert(::pw::unit_test::internal::HasNoUnderscores(#test_suite_name), \
                "The test suite name (" #test_suite_name                       \
                ") cannot contain underscores");                               \
  static_assert(sizeof(#test_name) > 1, "The test name must not be empty");    \
                                                                               \
  _PW_TEST_CLASS(test_suite_name,                                              \
                 test_name,                                                    \
                 test_suite_name##_##test_name##_Test,                         \
                 parent_class)

#define _PW_TEST_CLASS(suite, name, class_name, parent_class)               \
  class class_name final : public parent_class {                            \
   private:                                                                 \
    void PigweedTestBody() override;                                        \
  };                                                                        \
                                                                            \
  extern "C" {                                                              \
                                                                            \
  /* Declare the TestInfo as non-const since const variables do not work */ \
  /* with the PW_UNIT_TEST_LINK_FILE_CONTAINING_TEST macro. */              \
  /* NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables) */  \
  ::pw::unit_test::internal::TestInfo _pw_unit_test_Info_##suite##_##name(  \
      #suite,                                                               \
      #name,                                                                \
      __FILE__,                                                             \
      ::pw::unit_test::internal::Framework::CreateAndRunTest<class_name>);  \
                                                                            \
  } /* extern "C" */                                                        \
                                                                            \
  void class_name::PigweedTestBody()

#define _PW_TEST_ASSERT(expectation)                                           \
  do {                                                                         \
    if (!(expectation)) {                                                      \
      return static_cast<void>(0); /* Prevent using ASSERT in constructors. */ \
    }                                                                          \
  } while (0)

#define _PW_TEST_BOOL(expr, value)                               \
  ::pw::unit_test::internal::Framework::Get().CurrentTestExpect( \
      [](bool lhs, bool rhs) { return lhs == rhs; },             \
      static_cast<bool>(expr),                                   \
      value,                                                     \
      "is",                                                      \
      #expr " is " #value,                                       \
      __LINE__)

#define _PW_TEST_OP(lhs, rhs, op)                                \
  ::pw::unit_test::internal::Framework::Get().CurrentTestExpect( \
      [](const auto& _pw_lhs, const auto& _pw_rhs) {             \
        return _pw_lhs op _pw_rhs;                               \
      },                                                         \
      (lhs),                                                     \
      (rhs),                                                     \
      #op,                                                       \
      #lhs " " #op " " #rhs,                                     \
      __LINE__)

#define _PW_TEST_C_STR(lhs, rhs, op)                             \
  ::pw::unit_test::internal::Framework::Get().CurrentTestExpect( \
      [](const auto& _pw_lhs, const auto& _pw_rhs) {             \
        return std::strcmp(_pw_lhs.c_str, _pw_rhs.c_str) op 0;   \
      },                                                         \
      ::pw::unit_test::internal::CStringArg{lhs},                \
      ::pw::unit_test::internal::CStringArg{rhs},                \
      #op,                                                       \
      #lhs " " #op " " #rhs,                                     \
      __LINE__)

// Checks that test suite names between TEST and TEST_F declarations are unique.
// This works by declaring a function named for the test suite. The function
// takes no arguments but is declared with different return types in the TEST
// and TEST_F macros. If a TEST and TEST_F use the same test suite name, the
// function declarations conflict, resulting in a compilation error.
//
// This catches most conflicts, but a runtime check is ultimately needed since
// tests may be declared in different translation units.
#if !defined(__clang__) && !defined(__GNUC___) && __GNUC__ <= 8
// For some reason GCC8 is unable to ignore -Wredundant-decls here.
#define _PW_TEST_SUITE_NAMES_MUST_BE_UNIQUE(return_type, test_suite)
#else  // All other compilers.
#define _PW_TEST_SUITE_NAMES_MUST_BE_UNIQUE(return_type, test_suite)           \
  PW_MODIFY_DIAGNOSTICS_PUSH();                                                \
  PW_MODIFY_DIAGNOSTIC(ignored, "-Wredundant-decls");                          \
  extern "C" return_type /* use extern "C" to escape namespacing */            \
      PwUnitTestSuiteNamesMustBeUniqueBetweenTESTandTEST_F_##test_suite(void); \
  PW_MODIFY_DIAGNOSTICS_POP()
#endif  // GCC8 or older.

// Alias Test as ::testing::Test for GoogleTest compatibility.
namespace testing {

using Test = ::pw::unit_test::internal::Test;

}  // namespace testing
