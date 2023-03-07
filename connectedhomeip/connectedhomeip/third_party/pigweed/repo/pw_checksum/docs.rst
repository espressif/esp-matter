.. _module-pw_checksum:

-----------
pw_checksum
-----------
The ``pw_checksum`` module provides functions for calculating checksums.

pw_checksum/crc16_ccitt.h
=========================

.. cpp:namespace:: pw::checksum

.. cpp:var:: constexpr uint16_t kCcittCrc16DefaultInitialValue = 0xFFFF

  The default initial value for the CRC16.

.. cpp:function:: uint16_t CcittCrc16(span<const std::byte> data, uint16_t initial_value = kCcittCrc16DefaultInitialValue)

  Calculates the CRC16 of the provided data using polynomial 0x1021, with a
  default initial value of :cpp:expr:`0xFFFF`.

  To incrementally calculate a CRC16, use the previous value as the initial
  value.

  .. code-block:: cpp

    uint16_t crc = CcittCrc16(my_data);

    crc  = CcittCrc16(more_data, crc);

pw_checksum/crc32.h
===================

.. cpp:var:: constexpr uint32_t kCrc32InitialValue = 0xFFFFFFFF

  The initial value for the CRC32.

.. cpp:function:: uint32_t Crc32(span<const std::byte> data)

  Calculates the initial / one-time CRC32 of the provided data using polynomial
  0x4C11DB7, with an initial value of :cpp:expr:`0xFFFFFFFF`.

  .. code-block:: cpp

    uint32_t crc = Crc32(my_data);

.. cpp:function:: uint32_t Crc32(span<const std::byte> data, uint32_t previous_result)

  Incrementally append calculation of a CRC32, need to pass in the previous
  result.

  .. code-block:: cpp

    uint32_t crc = Crc32(my_data);
    crc = Crc32(more_data, crc);

.. _CRC32 Implementations:

Implementations
---------------
Pigweed provides 3 different CRC32 implementations with different size and
runtime tradeoffs.  The below table summarizes the variants.  For more detailed
size information see the :ref:`pw_checksum-size-report` below.  Instructions
counts were calculated by hand by analyzing the
`assembly <https://godbolt.org/z/nY1bbb5Pb>`_. Clock Cycle counts were measured
using :ref:`module-pw_perf_test` on a STM32F429I-DISC1 development board.


.. list-table::
   :header-rows: 1

   * - Variant
     - Relative size (see Size Report below)
     - Speed
     - Lookup table size (entries)
     - Instructions/byte (M33/-Os)
     - Clock Cycles (123 char string)
     - Clock Cycles (9 bytes)
   * - 8 bits per iteration (default)
     - large
     - fastest
     - 256
     - 8
     - 1538
     - 170
   * - 4 bits per iteration
     - small
     - fast
     - 16
     - 13
     - 2153
     - 215
   * - 1 bit per iteration
     - smallest
     - slow
     - 0
     - 43
     - 7690
     - 622

The default implementation provided by the APIs above can be selected through
:ref:`Module Configuration Options`.  Additionally ``pw_checksum`` provides
variants of the C++ API to explicitly use each of the implementations.  These
classes provide the same API as ``Crc32``:

* ``Crc32EightBit``
* ``Crc32FourBit``
* ``Crc32OneBit``

.. _pw_checksum-size-report:

Size report
===========
The CRC module currently optimizes for speed instead of binary size, by using
pre-computed 256-entry tables to reduce the CPU cycles per byte CRC
calculation.

.. include:: size_report

Compatibility
=============
* C
* C++17

Dependencies
============
* ``pw_span``

.. _Module Configuration Options:

Module Configuration Options
============================
The following configurations can be adjusted via compile-time configuration of
this module, see the
:ref:`module documentation <module-structure-compile-time-configuration>` for
more details.

.. c:macro:: PW_CHECKSUM_CRC32_DEFAULT_IMPL

  Selects which of the :ref:`CRC32 Implementations` the default CRC32 APIs
  use.  Set to one of the following values:

  * ``PW_CHECKSUM_CRC32_8BITS``
  * ``PW_CHECKSUM_CRC32_4BITS``
  * ``PW_CHECKSUM_CRC32_1BITS``

Zephyr
======
To enable ``pw_checksum`` for Zephyr add ``CONFIG_PIGWEED_CHECKSUM=y`` to the
project's configuration.
