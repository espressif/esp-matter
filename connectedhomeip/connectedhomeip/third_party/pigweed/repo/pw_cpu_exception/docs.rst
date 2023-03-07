.. _module-pw_cpu_exception:

================
pw_cpu_exception
================
Pigweed's exception module provides a consistent interface for entering an
application's CPU exception handler. While the actual exception handling
behavior is left to an application to implement, this module deals with any
architecture-specific actions required before calling the application exception
handler. More specifically, the exception module collects CPU state that may
otherwise be clobbered by an application's exception handler.

-----
Setup
-----
This module has three facades, each of whose backends are set with a
different GN variable.

``pw_cpu_exception_ENTRY_BACKEND``
==================================
This is the library that handles early exception entry and prepares any CPU
state that must be available to the exception handler via the
pw_cpu_exception_State object. The backend for this facade is
architecture-specific.

An application using this module **must** connect ``pw_cpu_exception_Entry()`` to
the platform's CPU exception handler interrupt so ``pw_cpu_exception_Entry()`` is
called immediately upon a CPU exception. For specifics on how this may be done,
see the backend documentation for your architecture.

``pw_cpu_exception_HANDLER_BACKEND``
====================================
This facade is backed by an application-specific handler that determines what to
do when an exception is encountered. This may be capturing a crash report before
resetting the device, or in some cases handling the exception to allow execution
to continue.

Applications must also provide an implementation for
``pw_cpu_exception_DefaultHandler()``. The behavior of this functions is entirely
up to the application/project, but some examples are provided below:

  * Enter an infinite loop so the device can be debugged by JTAG.
  * Reset the device.
  * Attempt to handle the exception so execution can continue.
  * Capture and record additional device state and save to flash for a crash
    report.
  * A combination of the above, using logic that fits the needs of your project.

``pw_cpu_exception_SUPPORT_BACKEND``
====================================
This facade provides architecture-independent functions that may be helpful for
dumping CPU state in various forms. This allows an application to create an
application-specific handler that is portable across multiple architectures.

Avoiding circular dependencies with ``pw_cpu_exception_ENTRY_BACKEND``
======================================================================
The entry facade is hard tied to the definition of the
``pw_cpu_exception_State``, so spliting them into separate facades would require
extra configurations along with extra compatibility checks to ensure they are
never mismatched.

In GN, this circular dependency is avoided by collecting the backend's full
implementation including the entry method through the
``pw_cpu_exception:entry_impl`` group. When ``pw_cpu_exception_ENTRY_BACKEND``
is set, ``$dir_pw_cpu_exception:entry_impl`` must listed in the
``pw_build_LINK_DEPS`` variable. See :ref:`module-pw_build-link-deps`.

Entry backends must provide their own ``*.impl`` target that collects their
entry implementation.

------------
Module Usage
------------
Basic usage of this module entails applications supplying a definition for
``pw_cpu_exception_DefaultHandler()``. ``pw_cpu_exception_DefaultHandler()`` should
contain any logic to determine if a exception can be recovered from, as well as
necessary actions to properly recover. If the device cannot recover from the
exception, the function should **not** return.

``pw_cpu_exception_DefaultHandler()`` is called indirectly, and may be overridden
at runtime via ``pw_cpu_exception_SetHandler()``. The handler can also be reset to
point to ``pw_cpu_exception_DefaultHandler()`` by calling
``pw_cpu_exception_RestoreDefaultHandler()``.

When writing an exception handler, prefer to use the functions provided by this
interface rather than relying on the backend implementation of
``pw_cpu_exception_State``. This allows better code portability as it helps
prevent an application fault handler from being tied to a single backend.

For example; when logging or dumping CPU state, prefer ``ToString()`` or
``RawFaultingCpuState()`` over directly accessing members of a
``pw_cpu_exception_State`` object.

Some exception handling behavior may require architecture-specific CPU state to
attempt to correct a fault. In this situation, the application's exception
handler will be tied to the backend implementation of the CPU exception module.

--------------------
Backend Expectations
--------------------
CPU exception backends do not provide an exception handler, but instead provide
mechanisms to capture CPU state for use by an application's exception handler,
and allow recovery from CPU exceptions when possible.

  * The entry backend should provide a definition for the
    ``pw_cpu_exception_State`` object through
    ``pw_cpu_exception_backend/state.h``.
  * In GN, the entry backend should also provide a ``.impl`` suffixed form of
    the entry backend target which collects the actual entry implementation to
    avoid circular dependencies due to the state definition in the entry backend
    target.
  * The entry backend should implement the ``pw_cpu_exception_Entry()`` function
    that will call ``pw_cpu_exception_HandleException()`` after performing any
    necessary actions prior to handing control to the application's exception
    handler (e.g. capturing necessary CPU state).
  * If an application's exception handler backend modifies the captured CPU
    state, the state should be treated as though it were the original state of
    the CPU when the exception occurred. The backend may need to manually
    restore some of the modified state to ensure this on exception handler
    return.

-------------
Compatibility
-------------
Most of the pw_cpu_exception module is C-compatible. The exception to this is
the "support" facade and library, which requires C++.

------------
Dependencies
------------
  * ``pw_span``
  * ``pw_preprocessor``
