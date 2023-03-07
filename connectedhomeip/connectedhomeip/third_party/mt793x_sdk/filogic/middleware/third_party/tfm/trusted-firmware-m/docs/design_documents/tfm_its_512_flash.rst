###############################################################
Add support for block-aligned flash in Internal Trusted Storage
###############################################################

:Author: Minos Galanakis
:Organization: Arm Limited
:Contact: Minos Galanakis <minos.galanakis@arm.com>
:Status: Accepted

Abstract
========

The proposal is describing a mechanism to enable the use of larger flash
devices, imposing a requirement for word-aligned full-block program operations,
in Trusted Firmare-M.


Requirements
============

- Allow page-aligned writes for up to 512 Bytes per page.
- Guarantee data integrity and power-failure reliability.
- Do not alter existing supported platform behaviour.

Current implementation
======================

In the current ITS filesystem design, each filesystem create or write operation
requires two flash blocks to be updated: first the data block and then the
metadata block. Buffering is avoided as much as possible to reduce
RAM requirements.

However, if the ITS_FLASH_PROGRAM_UNIT is 512 Bytes then the data will have to
stored in a temporary memory location in order to be able to write
that much data in one-shot.

Proposed implementation overview
================================

1. A new block-sized static buffer should be added to its_flash.c when
   ``ITS_FLASH_PROGRAM_UNIT`` is larger than currently supported.
2. Methods calling the flash API such as ``its_flash_write()`` or
   ``its_flash_block_to_block_move()`` will populate the buffer instead of
   directly programming the flash.
3. A new method ``its_flash_flush()``, should be provided in order to flush
   the block buffer to the device.
4. ``its_flash_flush()`` should be called twice: Once after a data block
   update and once more after the metadata block update is completed.
5. The proposed design should require that the data block update is always
   completed before the metadata block update starts
6. Writes to the block buffer should be atomic, and guarded against corruption
   by data from different blocks.

Considerations
==============

- The proposed implementation will increase the RAM usage of ITS by the size
  of a block, only for platforms which require block-aligned writes.
- Currently power-failure is detected by software by incrementing an 8-bit
  metadata header field (``swap_count``), as the last written byte. When the
  proposed block-buffer is used, the block is programmed in one-shot and the
  order the bytes are written on the physical device, is hardware dependent.
- A set of guarantees are required by the supported flash ECC devices.
  The device's flash APIs should provide a mechanism to capture and raise
  incomplete program operations, as well as write bytes in a sequential order.

For example, if a board powers down through a 512 page program operation, the
next read operation should return an error rather than read invalid data.

Functional flow diagram
=======================

The logic of the proposal is described in the following diagram

.. code-block::

        |----------------------|
        |   data write()       |
        |----------------------|
        |                      |        |------------------------------|
    |-> |  its_flash_write  |     --->  | data[] -> its_block_buffer[] |
    |   |                      |        |------------------------------|
    |   |----------------------|
    |   |                      |        |------------------------------------|
    |   |   its_flash_flush    |  --->  | its_block_buffer[] -> flash dev IO |
    |   |                      |        |------------------------------------|
    |   |----------------------|
    |             |
    |             ------------------------------------
    |                                                |
    |                                                V
    |   |----------------------|        |--------------------------|
    |   | data write() complete|        | metadata write() complete|
    |   |----------------------|        |--------------------------|
    | <-|  Metadata write()    |                     |
        |----------------------|                     |
                                                     V
                                        |--------------------------|
                                        |    Operation Complete    |
                                        |--------------------------|

--------------

*Copyright (c) 2019-2020, Arm Limited. All rights reserved.*
