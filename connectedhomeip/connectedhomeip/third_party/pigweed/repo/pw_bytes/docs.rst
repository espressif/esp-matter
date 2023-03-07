.. _module-pw_bytes:

=========
pw_bytes
=========
pw_bytes is a collection of utilities for manipulating binary data.

-------------
Compatibility
-------------
C++17

------------
Dependencies
------------
* ``pw_preprocessor``
* ``pw_status``
* ``pw_span``

--------
Features
--------

pw_bytes/array.h
================
Functions for working with byte arrays, primarily for building fixed-size byte
arrays at compile time.

pw_bytes/byte_builder.h
=======================
.. cpp:class:: ByteBuilder

  ``ByteBuilder`` is a class that facilitates building or reading arrays of
  bytes in a fixed-size buffer. ByteBuilder handles reading and writing integers
  with varying endianness.

.. cpp:class:: template <size_t kMaxSize> ByteBuffer

  ``ByteBuilder`` with an internally allocated buffer.

Size report: using ByteBuffer
-----------------------------
.. include:: byte_builder_size_report

pw_bytes/bit.h
================
Implementation of features provided by C++20's ``<bit>`` header. Supported
features:

* ``pw::endian`` -- Implementation of the ``std::endian`` enum. If
  ``std::endian`` is available, ``pw::endian`` is an alias of it.

pw_bytes/endian.h
=================
Functions for converting the endianness of integral values.

pw_bytes/units.h
================
Constants, functions and user-defined literals for specifying a number of bytes
in powers of two, as defined by IEC 60027-2 A.2 and ISO/IEC 80000:13-2008.

The supported suffixes include:

* ``_B``   for bytes     (1024\ :sup:`0`)
* ``_KiB`` for kibibytes (1024\ :sup:`1`)
* ``_MiB`` for mebibytes (1024\ :sup:`2`)
* ``_GiB`` for gibibytes (1024\ :sup:`3`)
* ``_TiB`` for tebibytes (1024\ :sup:`4`)
* ``_PiB`` for pebibytes (1024\ :sup:`5`)
* ``_EiB`` for exbibytes (1024\ :sup:`6`)

In order to use these you must use a using namespace directive, for example:

.. code-block:: cpp

  #include "pw_bytes/units.h"

  using namespace pw::bytes::unit_literals;

  constexpr size_t kRandomBufferSizeBytes = 1_MiB + 42_KiB;

In some cases, the use of user-defined literals is not permitted because of the
required using namespace directive. One example of this is in header files,
where it is undesirable to pollute the namespace. For this situation, there are
also similar functions:

.. code-block:: cpp

  #include "pw_bytes/units.h"

  constexpr size_t kBufferSizeBytes = pw::bytes::MiB(1) + pw::bytes::KiB(42);

------
Zephyr
------
To enable ``pw_bytes`` for Zephyr add ``CONFIG_PIGWEED_BYTES=y`` to the
project's configuration.
