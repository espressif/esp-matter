.. _module-pw_compilation_testing:

======================
pw_compilation_testing
======================
The pw_compilation_testing module provides for negative compilation (NC)
testing. Negative compilation tests ensure that code that should not compile
does not compile. Negative compilation testing is helpful in a variety of
scenarios, for example:

- Testing for compiler errors, such as ``[[nodiscard]]`` checks.
- Testing that a template cannot be instantiated with certain types.
- Testing that a ``static_assert`` statement is triggered as expected.
- For a ``constexpr`` function, testing that a ``PW_ASSERT`` is triggered as
  expected.

Negative compilation tests are only supported in GN currently. Negative
compilation tests are not currently supported in GN on Windows due to
`b/241565082 <https://issues.pigweed.dev/241565082>`_.

.. warning::

  This module is in an early, experimental state. Do not use it unless you have
  consulted with the Pigweed team.

---------------------------------
Negative compilation test example
---------------------------------
.. code-block:: cpp

  #include "gtest/gtest.h"
  #include "pw_compilation_testing/negative_compilation.h"

  template <int kValue>
  struct MyStruct {
    static_assert(kValue % 2 == 0, "wrong number!");

    constexpr int MultiplyOdd(int runtime_value) const {
      PW_ASSERT(runtime_value % 2 == 0);
      return kValue * runtime_value;
    }
  };

  [[maybe_unused]] MyStruct<16> this_one_works;

  // NC tests cannot be compiled, so they are created in preprocessor #if or
  // #elif blocks. These NC tests check that a static_assert statement fails if
  // the code is compiled.
  #if PW_NC_TEST(NegativeOddNumber)
  PW_NC_EXPECT("wrong number!");
  [[maybe_unused]] MyStruct<-1> illegal;
  #elif PW_NC_TEST(PositiveOddNumber)
  PW_NC_EXPECT("wrong number!");
  [[maybe_unused]] MyStruct<5> this_is_illegal;
  #endif  // PW_NC_TEST

  struct Foo {
    // Negative compilation tests can go anywhere in a source file.
  #if PW_NC_TEST(IllegalValueAsClassMember)
    PW_NC_EXPECT("wrong number!");
    MyStruct<12> also_illegal;
  #endif  // PW_NC_TEST
  };

  TEST(MyStruct, MultiplyOdd) {
    MyStruct<5> five;
    EXPECT_EQ(five.MultiplyOdd(3), 15);

    // This NC test checks that a specific PW_ASSERT() fails when expected.
    // This only works in an NC test if the PW_ASSERT() fails while the compiler
    // is executing constexpr code. The test code is used in a constexpr
    // statement to force compile-time evaluation.
    #if PW_NC_TEST(MyStruct_MultiplyOdd_AssertsOnOddNumber)
    [[maybe_unused]] constexpr auto fail = [] {
      PW_NC_EXPECT("PW_ASSERT\(runtime_value % 2 == 0\);");
      MyStruct<3> my_struct;
      return my_struct.MultiplyOdd(4);  // Even number, PW_ASSERT should fail.
    }();
    #endif  // PW_NC_TEST
  }

------------------------------------
Creating a negative compilation test
------------------------------------
- Declare a ``pw_cc_negative_compilation_test()`` GN target or set
  ``negative_compilation_test = true`` in a ``pw_test()`` target.
- Add the test to the build in a toolchain with negative compilation testing
  enabled (``pw_compilation_testing_NEGATIVE_COMPILATION_ENABLED = true``).
- In the test source files, add
  ``#include "pw_compilation_testing/negative_compilation.h"``.
- Use the ``PW_NC_TEST(TestName)`` macro in a ``#if`` statement.
- Immediately after the ``PW_NC_TEST(TestName)``, provide one or more
  Python-style regular expressions with the ``PW_NC_EXPECT()`` macro, one per
  line.
- Execute the tests by running the build.

To simplify parsing, all ``PW_NC_TEST()`` statements must fit on a single line.
``PW_NC_EXPECT()`` statements may span multiple lines, but must contain a single
regular expression as a string literal. The string may be comprised of multiple
implicitly concatenated string literals. The ``PW_NC_EXPECT()`` statement cannot
contain anything else except for ``//``-style comments.

Test assertions
===============
Negative compilation tests must have at least one assertion about the
compilation output. The assertion macros must be placed immediately after the
line with the ``PW_NC_TEST()`` or the test will fail.

.. c:macro:: PW_NC_EXPECT(regex_string_literal)

  When negative compilation tests are run, checks the compilation output for the
  provided regular expression. The argument to the ``PW_NC_EXPECT()`` statement
  must be a string literal. The literal is interpreted character-for-character
  as a Python raw string literal and compiled as a Python `re
  <https://docs.python.org/3/library/re.html>`_ regular expression.

  For example, ``PW_NC_EXPECT("something (went|has gone) wrong!")`` searches the
  failed compilation output with the Python regular expression
  ``re.compile("something (went|has gone) wrong!")``.

.. c:macro:: PW_NC_EXPECT_GCC(regex_string_literal)

   Same as :c:macro:`PW_NC_EXPECT`, but only applies when compiling with GCC.

.. c:macro:: PW_NC_EXPECT_CLANG(regex_string_literal)

   Same as :c:macro:`PW_NC_EXPECT`, but only applies when compiling with Clang.

.. admonition:: Test expectation tips
   :class: tip

   Be as specific as possible, but avoid compiler-specific error text. Try
   matching against the following:

   - ``static_assert`` messages.
   - Contents of specific failing lines of source code:
     ``PW_NC_EXPECT("PW_ASSERT\(!empty\(\));")``.
   - Comments on affected lines: ``PW_NC_EXPECT("// Cannot construct from
     nullptr")``.
   - Function names: ``PW_NC_EXPECT("SomeFunction\(\).*private")``.

   Do not match against the following:

   - Source file paths.
   - Source line numbers.
   - Compiler-specific wording of error messages, except when necessary.

------
Design
------
The basic flow for negative compilation testing is as follows.

- The user defines negative compilation tests in preprocessor ``#if`` blocks
  using the ``PW_NC_TEST()`` and :c:macro:`PW_NC_EXPECT` macros.
- The build invokes the ``pw_compilation_testing.generator`` script. The
  generator script:

  - finds ``PW_NC_TEST()`` statements and extracts a list of test cases,
  - finds all associated :c:macro:`PW_NC_EXPECT` statements, and
  - generates build targets for each negative compilation tests,
    passing the test information and expectations to the targets.

- The build compiles the test source file with all tests disabled.
- The build invokes the negative compilation test targets, which run the
  ``pw_compilation_testing.runner`` script. The test runner script:

  - invokes the compiler, setting a preprocessor macro that enables the ``#if``
    block for the test.
  - captures the compilation output, and
  - checks the compilation output for the :c:macro:`PW_NC_EXPECT` expressions.

- If compilation failed, and the output matches the test case's
  :c:macro:`PW_NC_EXPECT` expressions, the test passes.
- If compilation succeeded or the :c:macro:`PW_NC_EXPECT` expressions did not
  match the output, the test fails.

Existing frameworks
===================
Pigweed's negative compilation tests were inspired by Chromium's `no-compile
tests <https://www.chromium.org/developers/testing/no-compile-tests/>`_
tests and a similar framework used internally at Google. Pigweed's negative
compilation testing framework improves on these systems in a few respects:

- Trivial integration with unit tests. Negative compilation tests can easily be
  placed alongside other unit tests instead of in separate files.
- Safer, more natural macro-based API for test declarations. Other systems use
  ``#ifdef`` macro checks to define test cases, which fail silently when there
  are typos. Pigweed's framework uses function-like macros, which provide a
  clean and natural API, catch typos, and ensure the test is integrated with the
  NC test framework.
- More readable, flexible test assertions. Other frameworks place assertions in
  comments after test names, while Pigweed's framework uses function-like
  macros. Pigweed also supports compiler-specific assertions.
- Assertions are required. This helps ensure that compilation fails for the
  expected reason and not for an accidental typo or unrelated issue.
