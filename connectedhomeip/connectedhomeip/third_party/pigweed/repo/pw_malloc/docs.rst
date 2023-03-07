.. _module-pw_malloc:

---------
pw_malloc
---------

This module defines an interface for replacing the standard libc dynamic memory
operations.

This facade doesn't implement any heap structure or dynamic memory methods. It
only requires that backends implements a ``void pw_MallocInit();`` method.
This function is called before static intialization, and is responsible for
initializing global data structures required by the malloc implementation.

The intent of this module is to provide an interface for user-provided dynamic
memory operations that is compatible with different implementations.

Setup
=====
This module requires the following setup:

  1. Chose a ``pw_malloc`` backend, or write one yourself.
  2. If using GN build, Specify the ``pw_malloc_BACKEND`` GN build arg to point
     the library that provides a ``pw_malloc`` backend.

Module usage
============
See backend docs for how to interact with the underlying dynamic memory
operations implementation.
