.. _module-pw_third_party_boringssl:

=========
BoringSSL
=========

The ``$dir_pw_third_party/boringssl`` module provides the build files to
compile and use BoringSSL. The source code of BoringSSL needs to be provided by
the user. It is recommended to download it via Git submodules.

-------------
Build support
-------------

This module provides support to compile BoringSSL with GN. This is required when
compiling backends modules that use BoringSSL, such as some facades in
:ref:`module-pw_crypto`

Submodule
=========

The recommended way to include BoringSSL source code is to add it as a
submodule:

.. code-block:: sh

  git submodule add https://boringssl.googlesource.com/boringssl/ \
      third_party/boringssl/src

GN
==
The GN build file depends on a generated file called ``BUILD.generated.gni``
with the list of the different types of source files for the selected BoringSSL
version.

.. code-block:: sh

  cd third_party/boringssl
  python src/util/generate_build_files.py gn

The GN variables needed are defined in
``$dir_pw_third_party/boringssl/boringssl.gni``:

#. Set the GN ``dir_pw_third_party_boringssl`` to the path of the BoringSSL
   installation.

   - If using the submodule path from above, add the following to the
     ``default_args`` in the project's ``.gn``:

     .. code-block::

       dir_pw_third_party_boringssl = "//third_party/boringssl/src"

#. Having a non-empty ``dir_pw_third_party_boringssl`` variable causes GN to
   attempt to include the ``BUILD.generated.gni`` file from the sources even
   during the bootstrap process before the source package is installed by the
   bootstrap process. To avoid this problem, set this variable to the empty
   string during bootstrap by adding it to the ``virtualenv.gn_args`` setting in
   the ``env_setup.json`` file:

   .. code-block:: json

     {
       "virtualenv": {
         "gn_args": [
           "dir_pw_third_party_boringssl=\"\""
         ]
       }
     }


After this is done a ``pw_source_set`` for the BoringSSL library is created at
``$dir_pw_third_party/borignssl``.
