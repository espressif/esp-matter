.. _docs-embedded-cpp:

==================
Embedded C++ Guide
==================

This page contains recommendations for using C++ for embedded software. For
Pigweed code, these should be considered as requirements. For external
projects, these recommendations can serve as a resource for efficiently using
C++ in embedded projects.

These recommendations are subject to change as the C++ standard and compilers
evolve, and as the authors continue to gain more knowledge and experience in
this area. If you disagree with recommendations, please discuss them with the
Pigweed team, as we're always looking to improve the guide or correct any
inaccuracies.

Constexpr functions
===================
Constexpr functions are functions that may be called from a constant
expression, such as a template parameter, constexpr variable initialization, or
``static_assert`` statement. Labeling a function ``constexpr`` does not
guarantee that it is executed at compile time; if called from regular code, it
will be compiled as a regular function and executed at run time.

Constexpr functions are implicitly inline, which means they are suitable to be
defined in header files. Like any function in a header, the compiler is more
likely to inline it than other functions. Marking non-trivial functions as
``constexpr`` could increase code size, so check the compilation results if this
is a concern.

Simple constructors should be marked ``constexpr`` whenever possible. GCC
produces smaller code in some situations when the ``constexpr`` specifier is
present. Do not avoid important initialization in order to make the class
constexpr-constructible unless it actually needs to be used in a constant
expression.

Constexpr variables
===================
Constants should be marked ``constexpr`` whenever possible. Constexpr variables
can be used in any constant expression, such as a non-type template argument,
``static_assert`` statement, or another constexpr variable initialization.
Constexpr variables can be initialized at compile time with values calculated by
constexpr functions.

``constexpr`` implies ``const`` for variables, so there is no need to include
the ``const`` qualifier when declaring a constexpr variable.

Unlike constexpr functions, constexpr variables are **not** implicitly inline.
Constexpr variables in headers must be declared with the ``inline`` specifier.

.. code-block:: cpp

  namespace pw {

  inline constexpr const char* kStringConstant = "O_o";

  inline constexpr float kFloatConstant1 = CalculateFloatConstant(1);
  inline constexpr float kFloatConstant2 = CalculateFloatConstant(2);

  }  // namespace pw

Function templates
==================
Function templates facilitate writing code that works with different types. For
example, the following clamps a value within a minimum and maximum:

.. code-block:: cpp

  template <typename T>
  T Clamp(T min, T max, T value) {
    if (value < min) {
      return min;
    }
    if (value > max) {
      return max;
    }
    return value;
  }

The above code works seamlessly with values of any type -- float, int, or even a
custom type that supports the < and > operators.

The compiler implements templates by generating a separate version of the
function for each set of types it is instantiated with. This can increase code
size significantly.

.. tip::

  Be careful when instantiating non-trivial template functions with multiple
  types.

Virtual functions
=================
Virtual functions provide for runtime polymorphism. Unless runtime polymorphism
is required, virtual functions should be avoided. Virtual functions require a
virtual table, which increases RAM usage and requires extra instructions at each
call site. Virtual functions can also inhibit compiler optimizations, since the
compiler may not be able to tell which functions will actually be invoked. This
can prevent linker garbage collection, resulting in unused functions being
linked into a binary.

When runtime polymorphism is required, virtual functions should be considered.
C alternatives, such as a struct of function pointers, could be used instead,
but these approaches may offer no performance advantage while sacrificing
flexibility and ease of use.

.. tip::

  Only use virtual functions when runtime polymorphism is needed.

Compiler warnings
=================
Bugs in embedded systems can be difficult to track down. Compiler warnings are
one tool to help identify and fix bugs early in development.

Pigweed compiles with a strict set of warnings. The warnings include the
following:

  * ``-Wall`` and ``-Wextra`` -- Standard sets of compilation warnings, which
    are recommended for all projects.
  * ``-Wimplicit-fallthrough`` -- Requires explicit ``[[fallthrough]]``
    annotations for fallthrough between switch cases. Prevents unintentional
    fallthroughs if a ``break`` or ``return`` is forgotten.
  * ``-Wundef`` -- Requires macros to be defined before using them. This
    disables the standard, problematic behavior that replaces undefined (or
    misspelled) macros with ``0``.

Unused variable and function warnings
-------------------------------------
The ``-Wall`` and ``-Wextra`` flags enable warnings about unused variables or
functions. Usually, the best way to address these warnings is to remove the
unused items. In some circumstances, these cannot be removed, so the warning
must be silenced. This is done in one of the following ways:

  1. When possible, delete unused variables, functions, or class definitions.
  2. If an unused entity must remain in the code, avoid giving it a name. A
     common situation that triggers unused parameter warnings is implementing a
     virtual function or callback. In C++, function parameters may be unnamed.
     If desired, the variable name can remain in the code as a comment.

     .. code-block:: cpp

       class BaseCalculator {
        public:
         virtual int DoMath(int number_1, int number_2, int number_3) = 0;
       };

       class Calculator : public BaseCalculator {
         int DoMath(int number_1, int /* number_2 */, int) override {
           return number_1 * 100;
         }
       };

  3. In C++, annotate unused entities with `[[maybe_unused]]
     <https://en.cppreference.com/w/cpp/language/attributes/maybe_unused>`_ to
     silence warnings.

     .. code-block:: cpp

       // This variable is unused in certain circumstances.
       [[maybe_unused]] int expected_size = size * 4;
       #if OPTION_1
       DoThing1(expected_size);
       #elif OPTION_2
       DoThing2(expected_size);
       #endif

  4. As a final option, cast unused variables to ``void`` to silence these
     warnings. Use ``static_cast<void>(unused_var)`` in C++ or
     ``(void)unused_var`` in C.

     In C, silencing warnings on unused functions may require compiler-specific
     attributes (``__attribute__((unused))``). Avoid this by removing the
     functions or compiling with C++ and using ``[[maybe_unused]]``.

Dealing with ``nodiscard`` return values
----------------------------------------
There are rare circumstances where a ``nodiscard`` return value from a function
call needs to be discarded.  For ``pw::Status`` value ``.IgnoreError()`` can be
appended to the the function call.  For other instances, ``std::ignore`` can be
used.

.. code-block:: cpp

  // <tuple> defines std::ignore.
  #include <tuple>

  DoThingWithStatus().IgnoreError();
  std::ignore = DoThingWithReturnValue();