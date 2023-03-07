.. _module-pw_sys_io_stdio:

---------------
pw_sys_io_stdio
---------------
The ``pw_sys_io_stdio`` backend implements the ``pw_sys_io`` facade using
stdio.

Why not just use stdio directly?
--------------------------------

The nice thing about using ``pw_sys_io`` is that it's rather easy to get a
board up and running with a target-specific backend. This means when drafting
out a quick application you can write it against ``pw_sys_io`` and, with some
care, the application will be able to run on both host and target devices.

While it's not recommended to use ``pw_sys_io`` for any production
applications, it can be rather helpful for early prototyping.

Setup
=====
This module requires relatively minimal setup:

  1. Write code against the ``pw_sys_io`` facade.
  2. Direct the ``pw_sys_io_BACKEND`` GN build arg to point to this backend.

Module usage
============
For the most part, applications built with this backend will behave similarly
to an application built directly against stdio.

Dependencies
============
  * ``pw_sys_io`` facade
