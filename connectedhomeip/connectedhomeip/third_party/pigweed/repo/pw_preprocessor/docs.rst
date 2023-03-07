.. _module-pw_preprocessor:

---------------
pw_preprocessor
---------------
The preprocessor module provides various helpful preprocessor macros.

Compatibility
=============
C and C++

Headers
=======
The preprocessor module provides several headers.

pw_preprocessor/arguments.h
---------------------------------
Defines macros for handling variadic arguments to function-like macros. Macros
include the following:

.. c:macro:: PW_DELEGATE_BY_ARG_COUNT(name, ...)

  Selects and invokes a macro based on the number of arguments provided. Expands
  to ``<name><arg_count>(...)``. For example,
  ``PW_DELEGATE_BY_ARG_COUNT(foo_, 1, 2, 3)`` expands to ``foo_3(1, 2, 3)``.

  This example shows how ``PW_DELEGATE_BY_ARG_COUNT`` could be used to log a
  customized message based on the number of arguments provided.

  .. code-block:: cpp

      #define ARG_PRINT(...)  PW_DELEGATE_BY_ARG_COUNT(_ARG_PRINT, __VA_ARGS__)
      #define _ARG_PRINT0(a)        LOG_INFO("nothing!")
      #define _ARG_PRINT1(a)        LOG_INFO("1 arg: %s", a)
      #define _ARG_PRINT2(a, b)     LOG_INFO("2 args: %s, %s", a, b)
      #define _ARG_PRINT3(a, b, c)  LOG_INFO("3 args: %s, %s, %s", a, b, c)

  When used, ``ARG_PRINT`` expands to the ``_ARG_PRINT#`` macro corresponding
  to the number of arguments.

  .. code-block:: cpp

      ARG_PRINT();               // Outputs: nothing!
      ARG_PRINT("a");            // Outputs: 1 arg: a
      ARG_PRINT("a", "b");       // Outputs: 2 args: a, b
      ARG_PRINT("a", "b", "c");  // Outputs: 3 args: a, b, c

.. c:macro:: PW_COMMA_ARGS(...)

  Expands to a comma followed by the arguments if any arguments are provided.
  Otherwise, expands to nothing. If the final argument is empty, it is omitted.
  This is useful when passing ``__VA_ARGS__`` to a variadic function or template
  parameter list, since it removes the extra comma when no arguments are
  provided. ``PW_COMMA_ARGS`` must NOT be used when invoking a macro from
  another macro.

  For example. ``PW_COMMA_ARGS(1, 2, 3)``, expands to ``, 1, 2, 3``, while
  ``PW_COMMA_ARGS()`` expands to nothing. ``PW_COMMA_ARGS(1, 2, )`` expands to
  ``, 1, 2``.

pw_preprocessor/boolean.h
-------------------------
Defines macros for boolean logic on literal 1s and 0s. This is useful for
situations when a literal is needed to build the name of a function or macro.

pw_preprocessor/compiler.h
--------------------------
Macros for compiler-specific features, such as attributes or builtins.

.. c:macro:: PW_PACKED(declaration)

  Marks a struct or class as packed.

.. c:macro:: PW_USED

  Marks a function or object as used, ensuring code for it is generated.

.. c:macro:: PW_NO_PROLOGUE

  Prevents generation of a prologue or epilogue for a function. This is
  helpful when implementing the function in assembly.

.. c:macro:: PW_PRINTF_FORMAT(format_index, parameter_index)

  Marks that a function declaration takes a printf-style format string and
  variadic arguments. This allows the compiler to perform check the validity of
  the format string and arguments. This macro must only be on the function
  declaration, not the definition.

  The format_index is index of the format string parameter and parameter_index
  is the starting index of the variadic arguments. Indices start at 1. For C++
  class member functions, add one to the index to account for the implicit this
  parameter.

  This example shows a function where the format string is argument 2 and the
  varargs start at argument 3.

  .. code-block:: cpp

    int PrintfStyleFunction(char* buffer,
                            const char* fmt, ...) PW_PRINTF_FORMAT(2,3);

    int PrintfStyleFunction(char* buffer, const char* fmt, ...) {
      ... implementation here ...
    }

.. c:macro:: PW_PLACE_IN_SECTION(name)

  Places a variable in the specified linker section.

.. c:macro:: PW_KEEP_IN_SECTION(name)

  Places a variable in the specified linker section and directs the compiler
  to keep the variable, even if it is not used. Depending on the linker
  options, the linker may still remove this section if it is not declared in
  the linker script and marked KEEP.

.. c:macro:: PW_NO_RETURN

  Indicate to the compiler that the annotated function won't return. Example:

  .. code-block:: cpp

    PW_NO_RETURN void HandleAssertFailure(ErrorCode error_code);


.. c:macro:: PW_NO_INLINE

  Prevents the compiler from inlining a fuction.

.. c:macro:: PW_UNREACHABLE

  Indicate to the compiler that the given section of code will not be reached.
  Example:

  .. code-block:: cpp

    int main() {
      InitializeBoard();
      vendor_StartScheduler();  // Note: vendor forgot noreturn attribute.
      PW_UNREACHABLE;
    }


.. c:macro:: PW_NO_SANITIZE(check)

  Indicate to a sanitizer compiler runtime to skip the named check in the
  associated function.
  Example:

  .. code-block:: cpp

    uint32_t djb2(const void* buf, size_t len)
        PW_NO_SANITIZE("unsigned-integer-overflow"){
      uint32_t hash = 5381;
      const uint8_t* u8 = static_cast<const uint8_t*>(buf);
      for (size_t i = 0; i < len; ++i) {
        hash = (hash * 33) + u8[i]; // hash * 33 + c
      }
      return hash;
    }

.. c:macro:: PW_HAVE_ATTRIBUTE(x)

  Wrapper around `__has_attribute`, which is defined by GCC 5+ and Clang and
  evaluates to a non zero constant integer if the attribute is supported or 0
  if not.

.. c:macro:: PW_HAVE_CPP_ATTRIBUTE(x)

  Wrapper around `__has_cpp_attribute`, which was introduced in the C++20
  standard. It is supported by compilers even if C++20 is not in use. Evaluates
  to a non zero constant integer if the C++ attribute is supported or 0 if not.

.. c:macro:: PW_PRAGMA(contents)

  Expands to a _Pragma with the contents as a string. _Pragma must take a
  single string literal; this can be used to construct a _Pragma argument.

.. c:macro:: PW_WEAK

  Marks a function or object as weak, allowing the definition to be overriden.

  This can be useful when supporting third-party SDKs which may conditionally
  compile in code, for example:

  .. code-block:: cpp

    PW_WEAK void SysTick_Handler(void) {
      // Default interrupt handler that might be overriden.
    }

.. c:macro:: PW_ALIAS(aliased_to)

  Marks a weak function as an alias to another, allowing the definition to
  be given a default and overriden.

  This can be useful when supporting third-party SDKs which may conditionally
  compile in code, for example:

  .. code-block:: cpp

    // Driver handler replaced with default unless overridden.
    void USART_DriverHandler(void) PW_ALIAS(DefaultDriverHandler);

.. c:macro:: PW_ATTRIBUTE_LIFETIME_BOUND

  PW_ATTRIBUTE_LIFETIME_BOUND indicates that a resource owned by a function
  parameter or implicit object parameter is retained by the return value of the
  annotated function (or, for a parameter of a constructor, in the value of the
  constructed object). This attribute causes warnings to be produced if a
  temporary object does not live long enough.

  When applied to a reference parameter, the referenced object is assumed to be
  retained by the return value of the function. When applied to a non-reference
  parameter (for example, a pointer or a class type), all temporaries
  referenced by the parameter are assumed to be retained by the return value of
  the function.

  See also the upstream documentation:
  https://clang.llvm.org/docs/AttributeReference.html#lifetimebound

  This is a copy of ABSL_ATTRIBUTE_LIFETIME_BOUND.

Modifying compiler diagnostics
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
``pw_preprocessor/compiler.h`` provides macros for enabling or disabling
compiler diagnostics (warnings or errors).

.. c:macro:: PW_MODIFY_DIAGNOSTICS_PUSH()

  Starts a new group of :c:macro:`PW_MODIFY_DIAGNOSTIC` statements. A
  :c:macro:`PW_MODIFY_DIAGNOSTICS_POP` statement must follow.

.. c:macro:: PW_MODIFY_DIAGNOSTICS_POP()

  :c:macro:`PW_MODIFY_DIAGNOSTIC` statements since the most recent
  :c:macro:`PW_MODIFY_DIAGNOSTICS_PUSH` no longer apply after this statement.

.. c:macro:: PW_MODIFY_DIAGNOSTIC(kind, option)

  Changes how a diagnostic (warning or error) is handled. Most commonly used to
  disable warnings. ``PW_MODIFY_DIAGNOSTIC`` should be used between
  :c:macro:`PW_MODIFY_DIAGNOSTICS_PUSH` and :c:macro:`PW_MODIFY_DIAGNOSTICS_POP`
  statements to avoid applying the modifications too broadly.

  ``kind`` may be ``warning``, ``error``, or ``ignored``.

These macros can be used to disable warnings for precise sections of code, even
a single line if necessary.

.. code-block:: c

  PW_MODIFY_DIAGNOSTICS_PUSH();
  PW_MODIFY_DIAGNOSTIC(ignored, "-Wunused-variable");

  static int this_variable_is_never_used;

  PW_MODIFY_DIAGNOSTICS_POP();

.. tip::

  :c:macro:`PW_MODIFY_DIAGNOSTIC` and related macros should rarely be used.
  Whenever possible, fix the underlying issues about which the compiler is
  warning, rather than silencing the diagnostics.

pw_preprocessor/concat.h
------------------------
Defines the ``PW_CONCAT(...)`` macro, which expands its arguments if they are
macros and token pastes the results. This can be used for building names of
classes, variables, macros, etc.

pw_preprocessor/util.h
----------------------
General purpose, useful macros.

* ``PW_ARRAY_SIZE(array)`` -- calculates the size of a C array
* ``PW_STRINGIFY(...)`` -- expands its arguments as macros and converts them to
  a string literal
* ``PW_EXTERN_C`` -- declares a name to be ``extern "C"`` in C++; expands to
  nothing in C
* ``PW_EXTERN_C_START`` / ``PW_EXTERN_C_END`` -- declares an ``extern "C" { }``
  block in C++; expands to nothing in C

Zephyr
======
To enable ``pw_preprocessor`` for Zephyr add ``CONFIG_PIGWEED_PREPROCESSOR=y``
to the project's configuration.
