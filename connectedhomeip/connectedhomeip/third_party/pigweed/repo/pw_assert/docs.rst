.. _module-pw_assert:

=========
pw_assert
=========

--------
Overview
--------
Pigweed's assert module enables applications to check preconditions, triggering
a crash if the condition is not met. Consistent use of asserts is one aspect of
defensive programming that can lead to more reliable and less buggy code.

The assert API facilitates flexible crash handling through Pigweed's facade
mechanism. The API is designed to enable features like:

- Optional ancillary printf-style messages along assertions
- Capturing actual values of binary operator assertions like ``a < b``
- Compatibility with pw_tokenizer for reduced binary code size

The ``pw_assert`` API provides three classes of macros:

- **PW_CRASH(format, ...)** - Trigger a crash with a message.
- **PW_CHECK(condition[, format, ...])** - Assert a condition, optionally with
  a message.
- **PW_CHECK_<type>_<cmp>(a, b[, fmt, ...])** - Assert that the expression ``a
  <cmp> b`` is true, optionally with a message.
- **PW_ASSERT(condition)** - Header- and constexpr-safe assert.

.. tip::

  All of the ``CHECK`` macros optionally support a message with additional
  arguments, to assist in debugging when an assert triggers:

  .. code-block:: cpp

    PW_CHECK_INT_LE(ItemCount(), 100);
    PW_CHECK_INT_LE(ItemCount(), 100, "System state: %s", GetStateStr());

  To ensure compatibility with :ref:`module-pw_assert_log` and
  :ref:`module-pw_log_tokenized`, the message must be a string literal.

Example
=======

.. code-block:: cpp

  #include "pw_assert/check.h"

  int main() {
    bool sensor_running = StartSensor(&msg);
    PW_CHECK(sensor_running, "Sensor failed to start; code: %s", msg);

    int temperature_c = ReadSensorCelcius();
    PW_CHECK_INT_LE(temperature_c, 100,
                    "System is way out of heat spec; state=%s",
                    ReadSensorStateString());
  }

.. tip::

  All macros have both a ``CHECK`` and ``DCHECK`` variant. The ``CHECK``
  variant is always enabled, even in production. Generally, we advise making
  most asserts ``CHECK`` rather than ``DCHECK``, unless there is a critical
  performance or code size reason to use ``DCHECK``.

  .. code-block:: cpp

    // This assert is always enabled, even in production.
    PW_CHECK_INT_LE(ItemCount(), 100);

    // This assert is enabled based on ``PW_ASSERT_ENABLE_DEBUG``.
    // The functions ItemCount() and GetStateStr() are never called.
    PW_DCHECK_INT_LE(ItemCount(), 100, "System state: %s", GetStateStr());

.. tip::

  Use ``PW_ASSERT`` from ``pw_assert/assert.h`` for asserts in headers or
  asserting in ``constexpr`` contexts.

Structure of Assert Modules
===========================
The module is split into two components:

1. The **facade** (this module) which is only a macro interface layer, and
   performs the actual checks for the conditions.
2. The **backend**, provided elsewhere, that handles the consequences of an
   assert failing. Example backends include ``pw_assert_basic``, which prints a
   useful message and either quits the application (on host) or hangs in a
   while loop (on device). In the future, there will be a tokenized assert
   backend. This is also where application or product specific crash handling
   would go.

.. mermaid::

  graph LR
    facade --> backend

See the Backend API section below for more details.

----------
Facade API
----------
The below functions describe the assert API functions that applications should
invoke to assert. These macros are found in the ``pw_assert/check.h`` header.

.. cpp:function:: PW_CRASH(format, ...)

  Trigger a crash with a message. Replaces LOG_FATAL() in other systems. Can
  include a message with format arguments; for example:

  .. code-block:: cpp

    PW_CRASH("Unexpected: frobnitz in state: %s", frobnitz_state);

  Note: ``PW_CRASH`` is the equivalent of ``LOG_FATAL`` in other systems, where
  a device crash is triggered with a message. In Pigweed, logging and
  crashing/asserting are separated. There is a ``LOG_CRITICAL`` level in the
  logging module, but it does not have side effects; for ``LOG_FATAL``, instead
  use this macro (``PW_CRASH``).

.. cpp:function:: PW_CHECK(condition)
.. cpp:function:: PW_CHECK(condition, format, ...)
.. cpp:function:: PW_DCHECK(condition)
.. cpp:function:: PW_DCHECK(condition, format, ...)

  Assert that a condition is true, optionally including a message with
  arguments to report if the codition is false.

  The ``DCHECK`` variants only run if ``PW_ASSERT_ENABLE_DEBUG`` is enabled;
  otherwise, the entire statement is removed (and the expression not evaluated).

  Example:

  .. code-block:: cpp

    PW_CHECK(StartTurbines());
    PW_CHECK(StartWarpDrive(), "Oddly warp drive couldn't start; ruh-roh!");
    PW_CHECK(RunSelfTest(), "Failure in self test; try %d", TestAttempts());

  .. attention::

    Don't use use ``PW_CHECK`` for binary comparisons or status checks!

    Instead, use the ``PW_CHECK_<TYPE>_<OP>`` macros. These macros enable
    capturing the value of the operands, and also tokenizing them if using a
    tokenizing assert backend. For example, if ``x`` and ``b`` are integers,
    use instead ``PW_CHECK_INT_LT(x, b)``.

    Additionally, use ``PW_CHECK_OK(status)`` when checking for an OK status,
    since it enables showing a human-readable status string rather than an
    integer (e.g. ``status == RESOURCE_EXHAUSTED`` instead of ``status == 5``.

    +------------------------------------+-------------------------------------+
    | **Do NOT do this**                 | **Do this instead**                 |
    +------------------------------------+-------------------------------------+
    | ``PW_CHECK(a_int < b_int)``        | ``PW_CHECK_INT_LT(a_int, b_int)``   |
    +------------------------------------+-------------------------------------+
    | ``PW_CHECK(a_ptr <= b_ptr)``       | ``PW_CHECK_PTR_LE(a_ptr, b_ptr)``   |
    +------------------------------------+-------------------------------------+
    | ``PW_CHECK(Temp() <= 10.0)``       | ``PW_CHECK_FLOAT_EXACT_LE(``        |
    |                                    | ``    Temp(), 10.0)``               |
    +------------------------------------+-------------------------------------+
    | ``PW_CHECK(Foo() == OkStatus())``  | ``PW_CHECK_OK(Foo())``              |
    +------------------------------------+-------------------------------------+

.. cpp:function:: PW_CHECK_NOTNULL(ptr)
.. cpp:function:: PW_CHECK_NOTNULL(ptr, format, ...)
.. cpp:function:: PW_DCHECK_NOTNULL(ptr)
.. cpp:function:: PW_DCHECK_NOTNULL(ptr, format, ...)

  Assert that the given pointer is not ``NULL``, optionally including a message
  with arguments to report if the pointer is ``NULL``.

  The ``DCHECK`` variants only run if ``PW_ASSERT_ENABLE_DEBUG`` is enabled;
  otherwise, the entire statement is removed (and the expression not evaluated).

  .. code-block:: cpp

    Foo* foo = GetTheFoo()
    PW_CHECK_NOTNULL(foo);

    Bar* bar = GetSomeBar();
    PW_CHECK_NOTNULL(bar, "Weirdly got NULL bar; state: %d", MyState());

.. cpp:function:: PW_CHECK_TYPE_OP(a, b)
.. cpp:function:: PW_CHECK_TYPE_OP(a, b, format, ...)
.. cpp:function:: PW_DCHECK_TYPE_OP(a, b)
.. cpp:function:: PW_DCHECK_TYPE_OP(a, b, format, ...)

  Asserts that ``a OP b`` is true, where ``a`` and ``b`` are converted to
  ``TYPE``; with ``OP`` and ``TYPE`` described below.

  If present, the optional format message is reported on failure. Depending on
  the backend, values of ``a`` and ``b`` will also be reported.

  The ``DCHECK`` variants only run if ``PW_ASSERT_ENABLE_DEBUG`` is enabled;
  otherwise, the entire statement is removed (and the expression not evaluated).

  Example, with no message:

  .. code-block:: cpp

    PW_CHECK_INT_LE(CurrentTemperature(), 100);
    PW_CHECK_INT_LE(ItemCount(), 100);

  Example, with an included message and arguments:

  .. code-block:: cpp

    PW_CHECK_FLOAT_EXACT_GE(BatteryVoltage(), 3.2,
                            "System state=%s", SysState());

  Below is the full list of binary comparison assert macros, along with the
  type specifier. The specifier is irrelevant to application authors but is
  needed for backend implementers.

  +-------------------------+--------------+-----------+-----------------------+
  | Macro                   | a, b type    | condition | a, b format specifier |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_INT_LE         | int          | a <= b    | %d                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_INT_LT         | int          | a <  b    | %d                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_INT_GE         | int          | a >= b    | %d                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_INT_GT         | int          | a >  b    | %d                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_INT_EQ         | int          | a == b    | %d                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_INT_NE         | int          | a != b    | %d                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_UINT_LE        | unsigned int | a <= b    | %u                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_UINT_LT        | unsigned int | a <  b    | %u                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_UINT_GE        | unsigned int | a >= b    | %u                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_UINT_GT        | unsigned int | a >  b    | %u                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_UINT_EQ        | unsigned int | a == b    | %u                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_UINT_NE        | unsigned int | a != b    | %u                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_PTR_LE         | void*        | a <= b    | %p                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_PTR_LT         | void*        | a <  b    | %p                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_PTR_GE         | void*        | a >= b    | %p                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_PTR_GT         | void*        | a >  b    | %p                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_PTR_EQ         | void*        | a == b    | %p                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_PTR_NE         | void*        | a != b    | %p                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_FLOAT_EXACT_LE | float        | a <= b    | %f                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_FLOAT_EXACT_LT | float        | a <  b    | %f                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_FLOAT_EXACT_GE | float        | a >= b    | %f                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_FLOAT_EXACT_GT | float        | a >  b    | %f                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_FLOAT_EXACT_EQ | float        | a == b    | %f                    |
  +-------------------------+--------------+-----------+-----------------------+
  | PW_CHECK_FLOAT_EXACT_NE | float        | a != b    | %f                    |
  +-------------------------+--------------+-----------+-----------------------+

  The above ``CHECK_*_*()`` are also available in DCHECK variants, which will
  only evaluate their arguments and trigger if the ``PW_ASSERT_ENABLE_DEBUG``
  macro is enabled.

  +--------------------------+--------------+-----------+----------------------+
  | Macro                    | a, b type    | condition | a, b format          |
  |                          |              |           | specifier            |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_INT_LE         | int          | a <= b    | %d                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_INT_LT         | int          | a <  b    | %d                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_INT_GE         | int          | a >= b    | %d                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_INT_GT         | int          | a >  b    | %d                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_INT_EQ         | int          | a == b    | %d                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_INT_NE         | int          | a != b    | %d                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_UINT_LE        | unsigned int | a <= b    | %u                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_UINT_LT        | unsigned int | a <  b    | %u                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_UINT_GE        | unsigned int | a >= b    | %u                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_UINT_GT        | unsigned int | a >  b    | %u                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_UINT_EQ        | unsigned int | a == b    | %u                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_UINT_NE        | unsigned int | a != b    | %u                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_PTR_LE         | void*        | a <= b    | %p                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_PTR_LT         | void*        | a <  b    | %p                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_PTR_GE         | void*        | a >= b    | %p                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_PTR_GT         | void*        | a >  b    | %p                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_PTR_EQ         | void*        | a == b    | %p                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_PTR_NE         | void*        | a != b    | %p                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_FLOAT_EXACT_LE | float        | a <= b    | %f                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_FLOAT_EXACT_LT | float        | a <  b    | %f                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_FLOAT_EXACT_GE | float        | a >= b    | %f                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_FLOAT_EXACT_GT | float        | a >  b    | %f                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_FLOAT_EXACT_EQ | float        | a == b    | %f                   |
  +--------------------------+--------------+-----------+----------------------+
  | PW_DCHECK_FLOAT_EXACT_NE | float        | a != b    | %f                   |
  +--------------------------+--------------+-----------+----------------------+

.. attention::

  For float, proper comparator checks which take floating point
  precision and ergo error accumulation into account are not provided on
  purpose as this comes with some complexity and requires application
  specific tolerances in terms of Units of Least Precision (ULP). Instead,
  we recommend developers carefully consider how floating point precision and
  error impact the data they are bounding and whether checks are appropriate.

.. cpp:function:: PW_CHECK_FLOAT_NEAR(a, b, abs_tolerance)
.. cpp:function:: PW_CHECK_FLOAT_NEAR(a, b, abs_tolerance, format, ...)
.. cpp:function:: PW_DCHECK_FLOAT_NEAR(a, b, abs_tolerance)
.. cpp:function:: PW_DCHECK_FLOAT_NEAR(a, b, abs_tolerance, format, ...)

  Asserts that ``(a >= b - abs_tolerance) && (a <= b + abs_tolerance)`` is true,
  where ``a``, ``b``, and ``abs_tolerance`` are converted to ``float``.

  .. note::
    This also asserts that ``abs_tolerance >= 0``.

  The ``DCHECK`` variants only run if ``PW_ASSERT_ENABLE_DEBUG`` is enabled;
  otherwise, the entire statement is removed (and the expression not evaluated).

  Example, with no message:

  .. code-block:: cpp

    PW_CHECK_FLOAT_NEAR(cos(0.0f), 1, 0.001);

  Example, with an included message and arguments:

  .. code-block:: cpp

    PW_CHECK_FLOAT_NEAR(FirstOperation(), RedundantOperation(), 0.1,
                        "System state=%s", SysState());

.. cpp:function:: PW_CHECK_OK(status)
.. cpp:function:: PW_CHECK_OK(status, format, ...)
.. cpp:function:: PW_DCHECK_OK(status)
.. cpp:function:: PW_DCHECK_OK(status, format, ...)

  Assert that ``status`` evaluates to ``pw::OkStatus()`` (in C++) or
  ``PW_STATUS_OK`` (in C). Optionally include a message with arguments to
  report.

  The ``DCHECK`` variants only run if ``PW_ASSERT_ENABLE_DEBUG`` is defined;
  otherwise, the entire statement is removed (and the expression not evaluated).

  .. code-block:: cpp

    pw::Status operation_status = DoSomeOperation();
    PW_CHECK_OK(operation_status);

    // Any expression that evaluates to a pw::Status or pw_Status works.
    PW_CHECK_OK(DoTheThing(), "System state: %s", SystemState());

    // C works too.
    pw_Status c_status = DoMoreThings();
    PW_CHECK_OK(c_status, "System state: %s", SystemState());

  .. note::

    Using ``PW_CHECK_OK(status)`` instead of ``PW_CHECK(status == OkStatus())``
    enables displaying an error message with a string version of the error
    code; for example ``status == RESOURCE_EXHAUSTED`` instead of ``status ==
    5``.

.. _module-pw_assert-assert-api:

----------
Assert API
----------
The normal ``PW_CHECK_*`` and ``PW_DCHECK_*`` family of macros are intended to
provide rich debug information, like the file, line number, value of operands
in boolean comparisons, and more. However, this comes at a cost: these macros
depend directly on the backend headers, and may perform complicated call-site
transformations like tokenization.

There are several issues with the normal ``PW_CHECK_*`` suite of macros:

1. ``PW_CHECK_*`` in headers can cause ODR violations in the case of tokenized
   asserts, due to differing module choices.
2. ``PW_CHECK_*`` is not constexpr-safe.
3. ``PW_CHECK_*`` can cause code bloat with some backends; this is the tradeoff
   to get rich assert information.
4. ``PW_CHECK_*`` can trigger circular dependencies when asserts are used from
   low-level contexts, like in ``<span>``.

**PW_ASSERT** solves all of the above problems: No risk of ODR violations, are
constexpr safe, and have a tiny call site footprint; and there is no header
dependency on the backend preventing circular include issues.  However, there
are **no format messages, no captured line number, no captured file, no captured
expression, or anything other than a binary indication of failure**.

Example
=======

.. code-block:: cpp

  // This example demonstrates asserting in a header.

  #include "pw_assert/assert.h"

  class InlinedSubsystem {
   public:
    void DoSomething() {
      // GOOD: No problem; PW_ASSERT is fine to inline and place in a header.
      PW_ASSERT(IsEnabled());
    }
    void DoSomethingElse() {
      // BAD: Generally avoid using PW_DCHECK() or PW_CHECK in headers. If you
      // want rich asserts or logs, move the function into the .cc file, and
      // then use PW_CHECK there.
      PW_DCHECK(IsEnabled());  // DON'T DO THIS
    }
  };

PW_ASSERT API Reference
=======================
.. cpp:function:: PW_ASSERT(condition)

  A header- and constexpr-safe version of ``PW_CHECK()``.

  If the given condition is false, crash the system. Otherwise, do nothing.
  The condition is guaranteed to be evaluated. This assert implementation is
  guaranteed to be constexpr-safe.

.. cpp:function:: PW_DASSERT(condition)

  A header- and constexpr-safe version of ``PW_DCHECK()``.

  Same as ``PW_ASSERT()``, except that if ``PW_ASSERT_ENABLE_DEBUG == 0``, the
  assert is disabled and condition is not evaluated.

.. cpp:function:: PW_ASSERT_OK(expression)

  A header- and constexpr-safe version of ``PW_CHECK_OK()``.

  If the given expression is not `OK`, crash the system. Otherwise, do nothing.
  The condition is guarenteed to be evaluated.

.. attention::

  Unlike the ``PW_CHECK_*()`` suite of macros, ``PW_ASSERT()`` and
  ``PW_DASSERT()`` capture no rich information like line numbers, the file,
  expression arguments, or the stringified expression. Use these macros **only
  when absolutely necessary**---in headers, constexpr contexts, or in rare cases
  where the call site overhead of a full PW_CHECK must be avoided.

  Use ``PW_CHECK_*()`` whenever possible.

PW_ASSERT API Backend
=====================
The ``PW_ASSERT`` API ultimately calls the C function
``pw_assert_HandleFailure()``, which must be provided by the ``pw_assert``
backend. The ``pw_assert_HandleFailure()`` function must not return.

.. _module-pw_assert-circular-deps:

Avoiding Circular Dependencies With ``PW_ASSERT``
=================================================
Because asserts are so widely used, including in low-level libraries, it is
common for the ``pw_assert`` backend to cause circular dependencies. Because of
this, assert backends may avoid declaring explicit dependencies, instead relying
on include paths to access header files.

In GN, the ``pw_assert`` backend's full implementation with true dependencies is
made available through the ``$dir_pw_assert:impl`` group. When
``pw_assert_BACKEND`` is set, ``$dir_pw_assert:impl`` must be listed in the
``pw_build_LINK_DEPS`` variable. See :ref:`module-pw_build-link-deps`.

In the ``pw_assert``, the backend's full implementation is placed in the
``$pw_assert_BACKEND.impl`` target. ``$dir_pw_assert:impl`` depends on this
backend target. The ``$pw_assert_BACKEND.impl`` target may be an empty group if
the backend target can use its dependencies directly without causing circular
dependencies.

In order to break dependency cycles, the ``pw_assert_BACKEND`` target may need
to directly provide dependencies through include paths only, rather than GN
``public_deps``. In this case, GN header checking can be disabled with
``check_includes = false``.

.. _module-pw_assert-backend_api:

-----------
Backend API
-----------
The backend controls what to do in the case of an assertion failure. In the
most basic cases, the backend could display the assertion failure on something
like sys_io and halt in a while loop waiting for a debugger. In other cases,
the backend could store crash details like the current thread's stack to flash.

This facade module (``pw_assert``) does not provide a backend. See
:ref:`module-pw_assert_basic` for a basic implementation.

.. attention::

  The facade macros (``PW_CRASH`` and related) are expected to behave like they
  have the ``[[noreturn]]`` attribute set. This implies that the backend handler
  functions, ``PW_HANDLE_*`` defined by the backend, must not return.

  In other words, the device must reboot.

The backend must provide the header

``pw_assert_backend/check_backend.h``

and that header must define the following macros:

.. cpp:function:: PW_HANDLE_CRASH(message, ...)

  Trigger a system crash or halt, and if possible, deliver the specified
  message and arguments to the user or developer.

.. cpp:function:: PW_HANDLE_ASSERT_FAILURE(condition_str, message, ...)

  Trigger a system crash or halt, and if possible, deliver the condition string
  (indicating what expression was false) and the message with format arguments,
  to the user or developer.

  This macro is invoked from the ``PW_CHECK`` facade macro if condition is
  false.

.. cpp:function:: PW_HANDLE_ASSERT_BINARY_COMPARE_FAILURE( \
    a_str, a_val, op_str, b_str, b_val, type_fmt, message, ...)

  Trigger a system crash or halt for a failed binary comparison assert (e.g.
  any of the ``PW_CHECK_<type>_<op>`` macros). The handler should combine the
  assert components into a useful message for the user; though in some cases
  this may not be possible.

  Consider the following example:

  .. code-block:: cpp

    int temp = 16;
    int max_temp = 15;
    PW_CHECK_INT_LE(temp, MAX_TEMP, "Got too hot; state: %s", GetSystemState());

  In this block, the assert will trigger, which will cause the facade to invoke
  the handler macro. Below is the meaning of the arguments, referencing to the
  example:

  - ``a_str`` - Stringified first operand. In the example: ``"temp"``.
  - ``a_val`` - The value of the first operand. In the example: ``16``.
  - ``op_str`` - The string version of the operator. In the example: "<=".
  - ``b_str`` - Stringified second operand. In the example: ``"max_temp"``.
  - ``b_val`` - The value of the second operand. In the example: ``15``.
  - ``type_fmt`` - The format code for the type. In the example: ``"%d"``.
  - ``message, ...`` - A formatted message to go with the assert. In the
    example: ``"Got too hot; state: %s", "ON_FIRE"``.

  .. tip::

    See :ref:`module-pw_assert_basic` for one way to combine these arguments
    into a meaningful error message.

Additionally, the backend must provide a link-time function for the
``PW_ASSERT`` assert handler. This does not need to appear in the backend
header, but instead is in a ``.cc`` file.

.. cpp:function:: pw_assert_HandleFailure()

  Handle a low-level crash. This crash entry happens through
  ``pw_assert/assert.h``. In this crash handler, there is no access to line,
  file, expression, or other rich assert information. Backends should do
  something reasonable in this case; typically, capturing the stack is useful.

Backend Build Targets
=====================
In GN, the backend must provide a ``pw_assert.impl`` build target in the same
directory as the backend target. If the main backend target's dependencies would
cause dependency cycles, the actual backend implementation with its full
dependencies is placed in the ``pw_assert.impl`` target. If this is not
necessary, ``pw_assert.impl`` can be an empty group. Circular dependencies are a
common problem with ``pw_assert`` because it is so widely used. See
:ref:`module-pw_assert-circular-deps`.

Macro-based PW_ASSERT()/PW_DASSERT() backend
============================================
The pw_assert API is being re-assessed to provide more helpful information in
contexts where ``PW_CHECK_*()`` macros cannot be used. A first step towards this
is providing a macro-based backend API for the ``PW_ASSERT()`` and
``PW_DASSERT()`` macros.

.. warning::
  This part of ``pw_assert``'s API is transitional, and any project-specific
  reliance on any of the API mentioned here will likely experience breakages.
  In particular, ``PW_ASSERT_HANDLE_FAILURE`` and ``PW_HANDLE_ASSERT_FAILURE``
  are extremely confusingly similar and are NOT interchangeable.

A macro-based backend for the ``PW_ASSERT()`` macros must provide the following
macro in a header at ``pw_assert_backend/assert_backend.h``.

.. cpp:function:: PW_ASSERT_HANDLE_FAILURE(expression)

  Handle a low-level crash. This crash entry happens through
  ``pw_assert/assert.h``. Backends must ensure their implementation is safe for
  usage in headers, constexpr contexts, and templates. This macro should expand
  to an expression that does not return.

Similar to the ``PW_CHECK_*()`` facade, the header backend that provides an
expansion for the ``PW_ASSERT_HANDLE_FAILURE()`` macro can be controlled in the
GN build using the ``pw_assert_LITE_BACKEND`` build argument. In addition to
the header-based target at ``${pw_assert_LITE_BACKEND}``, a source set at
``${pw_assert_LITE_BACKEND}.impl`` is also required as a way to reduce the
impact of :ref:`circular dependencies <module-pw_assert-circular-deps>`.

--------------------------
Frequently Asked Questions
--------------------------

When should DCHECK_* be used instead of CHECK_* and vice versa?
===============================================================
There is no hard and fast rule for when to use one or the other.

In theory, ``DCHECK_*`` macros should never be used and all the asserts should
remain active in production. In practice, **assert statements come at a binary
size and runtime cost**, even when using extensions like a tokenized assert
backend that strips the stringified assert expression from the binary. Each
assert is **at least a branch with a function call**; depending on the assert
backend, that function call may take several arguments (like the message, the
file line number, the module, etc). These function calls can take 10-20 bytes
or more of ROM each. Thus, there is a balance to be struct between ``DCHECK_*``
and ``CHECK_*``.

Pigweed uses these conventions to decide between ``CHECK_*`` and ``DCHECK_*``:

- **Prefer to use CHECK_* at public API boundaries** of modules, where an
  invalid value is a clear programmer bug. In certain cases use ``DCHECK_*`` to
  keep binary size small when in production; for example, in modules with a
  large public API surface, or modules with many inlined functions in headers.
- **Avoid using CHECK_* macros in headers.** It is still OK to use ``CHECK_*``
  macros in headers, but carefully consider the cost, since inlined use of the
  ``CHECK_*`` macros in headers will expand to the full assert cost for every
  translation unit that includes the header and calls the function with the
  ``CHECK_*`` instance. ``DCHECK_*`` macros are are better, but even they come
  at a cost, since it is preferable to be able to compile a binary in debug
  mode for as long as possible on the road to production.
- **Prefer to use DCHECK_* variants for internal asserts** that attempt to
  catch module-author level programming errors. For example, use DCHECKs to
  verify internal function preconditions, or other invariants that should
  always be true but will likely never fire in production. In some cases using
  ``CHECK_*`` macros for internal consistency checking can make sense, if the
  runtime cost is low and there are only a couple of instances.

.. tip::

  **Do not return error status codes for obvious API misuse**

  Returning an error code may **mask the earliest sign of a bug** because
  notifying the developer of the problem depends on correct propagation of the
  error to upper levels of the system. Instead, prefer to use the ``CHECK_*``
  or ``DCHECK_*`` macros to ensure a prompt termination and warning to the
  developer.

  **Error status codes should be reserved for system misbehaviour or expected
  exceptional cases**, like a sensor is not yet ready, or a storage subsystem
  is full when writing. Doing ``CHECK_*`` assertions in those cases would be a
  mistake; so use error codes in those cases instead.

How should objects be asserted against or compared?
===================================================
Unfortunately, there is no native mechanism for this, and instead the way to
assert object states or comparisons is with the normal ``PW_CHECK_*`` macros
that operate on booleans, ints, and floats.

This is due to the requirement of supporting C and also tokenization. It may be
possible support rich object comparisons by defining a convention for
stringifying objects; however, this hasn't been added yet. Additionally, such a
mechanism would not work well with tokenization. In particular, it would
require runtime stringifying arguments and rendering them with ``%s``, which
leads to binary bloat even with tokenization. So it is likely that a rich
object assert API won't be added.

Why was the assert facade designed this way?
============================================
The Pigweed assert API was designed taking into account the needs of several
past projects the team members were involved with. Based on those experiences,
the following were key requirements for the API:

1. **C compatibility** - Since asserts are typically invoked from arbitrary
   contexts, including from vendor or third party code, the assert system must
   have a C-compatible API. Some API functions working only in C++ is
   acceptable, as long as the key functions work in C.
2. **Capturing both expressions and values** - Since asserts can trigger in
   ways that are not repeatable, it is important to capture rich diagnostic
   information to help identifying the root cause of the fault. For asserts,
   this means including the failing expression text, and optionally also
   capturing failing expression values. For example, instead of capturing an
   error with the expression (``x < y``), capturing an error with the
   expression and values(``x < y, with x = 10, y = 0``).
3. **Tokenization compatible** - It's important that the assert expressions
   support tokenization; both the expression itself (e.g. ``a < b``) and the
   message attached to the expression. For example: ``PW_CHECK(ItWorks(), "Ruh
   roh: %d", some_int)``.
4. **Customizable assert handling** - Most products need to support custom
   handling of asserts. In some cases, an assert might trigger printing out
   details to a UART; in other cases, it might trigger saving a log entry to
   flash. The assert system must support this customization.

The combination of #1, #2, and #3 led to the structure of the API. In
particular, the need to support tokenized asserts and the need to support
capturing values led to the choice of having ``PW_CHECK_INT_LE(a, b)`` instead
of ``PW_CHECK(a <= b)``. Needing to support tokenization is what drove the
facade & backend arrangement, since the backend must provide the raw macros for
asserting in that case, rather than terminating at a C-style API.

Why isn't there a ``PW_CHECK_LE``? Why is the type (e.g. ``INT``) needed?
=========================================================================
The problem with asserts like ``PW_CHECK_LE(a, b)`` instead of
``PW_CHECK_INT_LE(a, b)`` or ``PW_CHECK_FLOAT_EXACT_LE(a, b)`` is that to
capture the arguments with the tokenizer, we need to know the types. Using the
preprocessor, it is impossible to dispatch based on the types of ``a`` and
``b``, so unfortunately having a separate macro for each of the types commonly
asserted on is necessary.

----------------------------
Module Configuration Options
----------------------------
The following configurations can be adjusted via compile-time configuration of
this module, see the
:ref:`module documentation <module-structure-compile-time-configuration>` for
more details.

.. c:macro:: PW_ASSERT_ENABLE_DEBUG

  Controls whether ``DCHECK`` and ``DASSERT`` are enabled.

  This defaults to being disabled if ``NDEBUG`` is defined, else it is enabled
  by default.

.. c:macro:: PW_ASSERT_CAPTURE_VALUES

  Controls whether the evaluated values of a CHECK statement are captured as
  arguments to the final string. Disabling this will reduce code size at CHECK
  callsites, but slightly reduces debugability.

  This defaults to enabled.

-------------
Compatibility
-------------
The facade is compatible with both C and C++.

----------------
Roadmap & Status
----------------
The Pigweed assert subsystem consiststs of several modules that work in
coordination. This module is the facade (API), then a number of backends are
available to handle assert failures. Products can also define their own
backends. In some cases, the backends will have backends (like
``pw_log_tokenized``).

Below is a brief summary of what modules are ready for use:

Available Assert Backends
=========================
- ``pw_assert`` - **Stable** - The assert facade (this module). This module is
  stable, and in production use. The documentation is comprehensive and covers
  the functionality. There are (a) tests for the facade macro processing logic,
  using a fake assert backend; and (b) compile tests to verify that the
  selected backend compiles with all supported assert constructions and types.
- ``pw_assert:print_and_abort_backend`` - **Stable** - Uses the ``printf`` and
  ``abort`` standard library functions to implement the assert facade. Prints
  the assert expression, evaluated arguments if any, file/line, function name,
  and user message, then aborts. Only suitable for targets that support these
  standard library functions. Compatible with C++14.
- ``pw_assert_basic`` - **Stable** - The assert basic module is a simple assert
  handler that displays the failed assert line and the values of captured
  arguments. Output is directed to ``pw_sys_io``. This module is a great
  ready-to-roll module when bringing up a system, but is likely not the best
  choice for production.
- ``pw_assert_log`` - **Stable** - This assert backend redirects to logging,
  but with a logging flag set that indicates an assert failure. This is our
  advised approach to get **tokenized asserts**--by using tokenized logging,
  then using the ``pw_assert_log`` backend.

Note: If one desires a null assert module (where asserts are removed), use
``pw_assert_log`` in combination with ``pw_log_null``. This will direct asserts
to logs, then the logs are removed due to the null backend.

Missing Functionality
=====================
- **Stack traces** - Pigweed doesn't have a reliable stack walker, which makes
  displaying a stack trace on crash harder. We plan to add this eventually.
- **Snapshot integration** - Pigweed doesn't yet have a rich system state
  capture system that can capture state like number of tasks, available memory,
  and so on. Snapshot facilities are the obvious ones to run inside an assert
  handler. It'll happen someday.
