.. _module-pw_sys_io:

---------
pw_sys_io
---------
This module defines a simple and unoptimized interface for byte-by-byte
input/output. This can be done over a logging system, stdio, UART, via a
photodiode and modulated kazoo, or basically any way to get data in and out
of an application.

This facade doesn't dictate any policies on input and output data encoding,
format, or transmission protocol. It only requires that backends return a
``OkStatus()`` if the operation succeeds. Backends may provide useful error
Status types, but depending on the implementation-specific Status values is
NOT recommended. Since this facade provides a very vague I/O interface, it
does NOT provide tests. Backends are expected to provide their own testing to
validate correctness.

The intent of this module for simplifying bringup or otherwise getting data
in/out of a CPU in a way that is platform-agnostic. The interface is designed
to be easy to understand. There's no initialization as part of this
interface, there's no configuration, and the interface is no-frills WYSIWYG
byte-by-byte i/o.

**It is strongly advised NOT to build projects on top of this interface.** There
are many drawbacks to using this interface, so it's not generally suited for use
in production.

Setup
=====
This module requires relatively minimal setup:

  1. Choose a ``pw_sys_io`` backend, or write one yourself.
  2. If using GN build, Specify the ``pw_sys_io_BACKEND`` GN build arg to point
     the library that provides a ``pw_sys_io`` backend.

Module usage
============
See backend docs for how to interact with the underlying system I/O
implementation.

Dependencies
============
  * pw_sys_io_backend
  * pw_span
  * pw_status
