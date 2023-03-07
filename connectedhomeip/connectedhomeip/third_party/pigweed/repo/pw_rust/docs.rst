.. _module-pw_rust:

=======
pw_rust
=======
Rust support in pigweed is ***highly*** experimental.  Currently only building
a single host binary using the standard libraries is supported.  Only GN builds
are supported and building on Windows is currently unsupported.

--------
Building
--------
To build the sample rust targets, you need to enable
``pw_rust_ENABLE_EXPERIMENTAL_BUILD``:

.. code:: bash

   $ gn gen out --args="pw_rust_ENABLE_EXPERIMENTAL_BUILD=true"

Once that is set, you can build and run the ``hello`` example:

.. code:: bash

   $ ninja -C out host_clang_debug/obj/pw_rust/example/bin/hello
   $ ./out/host_clang_debug/obj/pw_rust/example/bin/hello
   Hello, Pigweed!
