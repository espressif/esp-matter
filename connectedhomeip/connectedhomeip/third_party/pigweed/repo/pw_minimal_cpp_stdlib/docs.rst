.. _module-pw_minimal_cpp_stdlib:

=====================
pw_minimal_cpp_stdlib
=====================
The ``pw_minimal_cpp_stdlib`` module provides an extremely limited
implementation of the C++ Standard Library. This module falls far, far short of
providing a complete C++ Standard Library and should only be used for testing
and development when compiling with C++17 or newer without a C++ Standard
Library. Production code should use a real C++ Standard Library implementation,
such as `libc++ <https://libcxx.llvm.org/>`_ or
`libstdc++ <https://gcc.gnu.org/onlinedocs/libstdc++/>`_.

.. warning::

  ``pw_minimal_cpp_stdlib`` was created for a very specific purpose. It is NOT a
  general purpose C++ Standard Library implementation and should not be used as
  one. Many features are missing, some features non-functioning stubs, and some
  features may not match the C++ standard.

-----------
Code layout
-----------
The C++ Standard Library headers (e.g. ``<cstdint>`` and ``<type_traits>``) are
defined in ``public/``. These files are symlinks to their implementations in
``public/internal/``.

.. tip::

  You can automatically recreate the symlinks in ``public/`` by executing the
  following Bash code from ``pw_minimal_cpp_stdlib/public/``.

  .. code-block:: bash

    for f in $(ls internal/); do ln -s internal/$f ${f%.h}; done

The top-level ``build_with_minimal_cpp_stdlib`` GN group builds a few supported
modules with ``pw_minimal_cpp_stdlib`` swapped in for the C++ library at the
toolchain level. Notably, ``pw_minimal_cpp_stdlib`` does not support
``pw_unit_test``, so this group does not run any tests.

------------
Requirements
------------
- C++17
- gcc or clang
- The C Standard Library
