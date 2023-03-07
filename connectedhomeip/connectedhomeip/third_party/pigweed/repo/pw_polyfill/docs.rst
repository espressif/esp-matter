.. _module-pw_polyfill:

===========
pw_polyfill
===========
The ``pw_polyfill`` module supports compiling code against different C++
standards. It also supports backporting a few C++17 features to C++14.

----------------------------------------------------
Adapt code to compile with different versions of C++
----------------------------------------------------

C++ standard macro
==================
``pw_polyfill/standard.h`` provides a macro for checking if a C++ standard is
supported.

.. c:macro:: PW_CXX_STANDARD_IS_SUPPORTED(standard)

   Evaluates true if the provided C++ standard (98, 11, 14, 17, 20) is supported
   by the compiler. This is a simpler, cleaner alternative to checking the value
   of the ``__cplusplus`` macro.

Language feature macros
=======================
``pw_polyfill/language_feature_macros.h`` provides macros for adapting code to
work with or without C++ language features.

.. list-table::
  :header-rows: 1

  * - Macro
    - Feature
    - Description
    - Feature test macro
  * - ``PW_INLINE_VARIABLE``
    - inline variables
    - inline if supported by the compiler
    - ``__cpp_inline_variables``
  * - ``PW_CONSTEXPR_CPP20``
    - ``constexpr``
    - ``constexpr`` if compiling for C++20
    - ``__cplusplus >= 202002L``
  * - ``PW_CONSTEVAL``
    - ``consteval``
    - ``consteval`` if supported by the compiler
    - ``__cpp_consteval``
  * - ``PW_CONSTINIT``
    - ``constinit``
    - ``constinit`` in clang and GCC 10+
    - ``__cpp_constinit``

In GN, Bazel, or CMake, depend on ``$dir_pw_polyfill``, ``//pw_polyfill``,
or ``pw_polyfill``, respectively, to access these features. In other build
systems, add ``pw_polyfill/public`` as an include path.

------------------------------------------------
Backport new C++ features to older C++ standards
------------------------------------------------
Pigweed backports a few C++ features to older C++ standards. These features
are provided in the ``pw`` namespace. If the features are provided by the
toolchain, the ``pw`` versions are aliases of the ``std`` versions.

``pw_polyfill`` also backports a few C++17 library features to C++14 by wrapping
the standard C++ and C headers. The wrapper headers include the original header
using `#include_next
<https://gcc.gnu.org/onlinedocs/cpp/Wrapper-Headers.html>`_, then add missing
features. The backported features are only defined if they aren't provided by
the standard header and can only be used when compiling with C++14 in GN.

Backported features
===================
.. list-table::
  :header-rows: 1

  * - Header
    - Feature
    - Feature test macro
    - Module
    - Polyfill header
    - Polyfill name
  * - ``<array>``
    - ``std::to_array``
    - ``__cpp_lib_to_array``
    - :ref:`module-pw_containers`
    - ``pw_containers/to_array.h``
    - ``pw::containers::to_array``
  * - ``<bit>``
    - ``std::endian``
    - ``__cpp_lib_endian``
    - :ref:`module-pw_bytes`
    - ``pw_bytes/bit.h``
    - ``pw::endian``
  * - ``<cstdlib>``
    - ``std::byte``
    - ``__cpp_lib_byte``
    - pw_polyfill
    - ``<cstdlib>``
    - ``std::byte``
  * - ``<iterator>``
    - ``std::data``, ``std::size``
    - ``__cpp_lib_nonmember_container_access``
    - pw_polyfill
    - ``<iterator>``
    - ``std::data``, ``std::size``
  * - ``<span>``
    - ``std::span``
    - ``__cpp_lib_span``
    - :ref:`module-pw_span`
    - ``pw_span/span.h``
    - ``pw::span``

-------------
Compatibility
-------------
C++14

Zephyr
======
To enable ``pw_polyfill`` for Zephyr add ``CONFIG_PIGWEED_POLYFILL=y`` to the
project's configuration.
