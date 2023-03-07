###################################
Details for the platform/ext folder
###################################
This folder has code that has been imported from other projects. This means the
files in this folder and subfolders have Apache 2.0 license which is different
to BSD 3.0 license applied to the parent TF-M project.

.. Note::
    This folder is strictly Apache 2.0 with the exception of cmake files.
    Maintainers should be consulted if this needs to be revisited.

***********
Sub-folders
***********

cmsis
=====
This folder contains core and compiler specific header files imported from the
``CMSIS_5`` project.

common
======

armclang and gcc
----------------
These contain the linker scripts used to configure the memory regions in TF-M
regions.

cc312
-----
This folder contains cmake and code files to interact with the CC312
cryptographic accelerator. Integrating a platform with the CC312 requires some
configuration, of which an example can be seen in the
``platform/ext/musca_b1.cmake`` file.

To configure the CC312 at build time, a few cmake arguments can be specified.

- ``CRYPTO_HW_ACCELERATOR``
   - ``ON`` All possible mbedtls cryptographic operations will be offloaded to
     the CC312 accelerator.
   - ``OFF`` The cryptographic accelerator will be ignored and software
     cryptography will be used.

- ``CRYPTO_HW_ACCELERATOR_OTP_STATE``
   - ``DISABLED`` The HW accelerator will not use any data from its onboard OTP
     (One Time Programmable) memory.
   - ``PROVISIONING`` This special mode is used to program cryptographic
     material into the OTP memory. When the flag is set TF-M will not boot, but
     will instead program the hardware unique key, the root of trust private key
     and the attestation private key into the OTP memory. This mode is not
     compatible with
     ``CRYPTO_HW_ACCELERATOR=ON``.
   - ``ENABLED`` The HW accelerator will use the previously programmed data as
     the hardware unique key, the root of trust private key and the attestation
     private key. This mode requires ``CRYPTO_HW_ACCELERATOR=ON``.


.. Warning::

   Provisioning **can not** be reversed, and data in the OTP memory **can not**
   be changed once set.

template
--------
This directory contains platform-independent dummy implementations of the
interfaces in ``platform/include``. These implementations can be built directly
for initial testing of a platform port, or used as a basic template for a real
implementation for a particular target. They **must not** be used in production
systems.

other
-----
This folder also contains stdout redirection to UART and
``tfm_mbedcrypto_config.h`` for all the targets.

drivers
=======
This folder contains the headers with CMSIS compliant driver definitions that
that TF-M project expects a target to provide.

target_cfg.h
------------
This file is expected to define the following macros respectively.

- ``TFM_DRIVER_STDIO`` - This macro should expand to a structure of type
  ``ARM_DRIVER_USART``. TFM redirects its standard input and output to this
  instance of USART.
- ``NS_DRIVER_STDIO`` - This macro should expand to a structure of type
  ``ARM_DRIVER_USART``. Non-Secure application redirects its standard input and
  output to this instance of USART.

target
======
This folder contains the files for individual target.

Flash layout header file
------------------------
Target must provide a header file, called ``flash_layout.h``, which defines the
information explained in the follow subsections. The defines must be named
as they are in the subsections.

BL2 bootloader
^^^^^^^^^^^^^^
The BL2 bootloader requires the following definitions:

- ``FLASH_BASE_ADDRESS`` - Defines the first valid address in the flash.
- ``FLASH_AREA_BL2_OFFSET`` - Defines the offset from the flash base address
  where the BL2 - MCUBOOT area starts.
- ``FLASH_AREA_BL2_SIZE`` - Defines the size of the BL2 area.
- ``FLASH_AREA_SCRATCH_OFFSET`` - Defines the offset from the flash base
  address where the scratch area starts, which is used during image swapping.
- ``FLASH_AREA_SCRATCH_SIZE`` - Defines the size of the scratch area. The
  minimal size must be as the biggest sector size in the flash.
- ``FLASH_DEV_NAME`` - Specifies the flash device used by BL2.

The BL2 requires further definitions depending on the number of images, the
meaning of these macros are also slightly different:

- Required definitions in case of 1 image (S and NS images are concatenated
  and handled together as one binary blob):

    - ``FLASH_AREA_0_OFFSET`` - Defines the offset from the flash base address
      where the primary image area starts, which hosts the active firmware
      image.
    - ``FLASH_AREA_0_SIZE`` - Defines the size of the primary image area.
    - ``FLASH_AREA_2_OFFSET`` - Defines the offset from the flash base address
      where the secondary image area starts, which is a placeholder for new
      firmware images.
    - ``FLASH_AREA_2_SIZE`` - Defines the size of the secondary image area.

- Required definitions in case of 2 images (S and NS images are handled and
  updated separately):

    - ``FLASH_AREA_0_OFFSET`` - Defines the offset from the flash base address
      where the primary image areas start, which host the active firmware
      images. It is also the offset of the primary (active) secure image area.
    - ``FLASH_AREA_0_SIZE`` - Defines the size of the primary secure image area.
    - ``FLASH_AREA_1_OFFSET`` - Defines the offset from the flash base address
      where the primary (active) non-secure image area starts.
    - ``FLASH_AREA_1_SIZE`` - Defines the size of the primary non-secure image
      area.
    - ``FLASH_AREA_2_OFFSET`` - Defines the offset from the flash base address
      where the secondary image areas start, which are placeholders for new
      firmware images. It is also the offset of the secondary secure image area.
    - ``FLASH_AREA_2_SIZE`` - Defines the size of the secondary secure image
      area.
    - ``FLASH_AREA_3_OFFSET`` - Defines the offset from the flash base address
      where the secondary non-secure image area starts.
    - ``FLASH_AREA_3_SIZE`` - Defines the size of the secondary non-secure image
      area.

The table below shows a fraction of the flash layout in case of 2 and 1
updatable images with the related flash areas that hold the firmware images:

+-----------------------+--------------------+-----------------------+-----------------------------+
| Image number: 2                            | Image number: 1                                     |
+=======================+====================+=======================+=============================+
| **Flash area**        | **Content**        | **Flash area**        | **Content**                 |
+-----------------------+--------------------+-----------------------+-----------------------------+
| FLASH_AREA_0          | | Secure image     | FLASH_AREA_0          | | Secure + Non-secure image |
|                       | | primary slot     |                       | | primary slot              |
+-----------------------+--------------------+-----------------------+                             +
| FLASH_AREA_1          | | Non-secure image |                       |                             |
|                       | | primary slot     |                       |                             |
+-----------------------+--------------------+-----------------------+-----------------------------+
| FLASH_AREA_2          | | Secure image     | FLASH_AREA_2          | | Secure + Non-secure image |
|                       | | secondary slot   |                       | | secondary slot            |
+-----------------------+--------------------+-----------------------+                             +
| FLASH_AREA_3          | | Non-secure image |                       |                             |
|                       | | secondary slot   |                       |                             |
+-----------------------+--------------------+-----------------------+-----------------------------+
| FLASH_AREA_SCRATCH    | Scratch area       | FLASH_AREA_SCRATCH    | Scratch area                |
+-----------------------+--------------------+-----------------------+-----------------------------+

- ``IMAGE_EXECUTABLE_RAM_START`` - Defines the start of the region to which
  images are allowed to be loaded. Only used if ``MCUBOOT_UPGRADE_STRATEGY`` is
  configured to be ``RAM_LOADING``.

- ``IMAGE_EXECUTABLE_RAM_SIZE`` - Defines the size of the region to which images
  are allowed to be loaded. Only used if ``MCUBOOT_UPGRADE_STRATEGY`` is
  configured to be ``RAM_LOADING``.

Assemble tool
^^^^^^^^^^^^^
The ``assemble.py`` tool is used to concatenate secure and non-secure binary
to a single binary blob. It requires the following definitions:

- ``SECURE_IMAGE_OFFSET`` - Defines the offset from the single binary blob base
  address, where the secure image starts.
- ``SECURE_IMAGE_MAX_SIZE`` - Defines the maximum size of the secure image area.
- ``NON_SECURE_IMAGE_OFFSET`` - Defines the offset from the single binary blob
  base address,   where the non-secure image starts.
- ``NON_SECURE_IMAGE_MAX_SIZE`` - Defines the maximum size of the non-secure
  image area.

Image tool
^^^^^^^^^^^^^
The ``imgtool.py`` tool is used to handle the tasks related to signing the
binary. It requires the following definition:

- ``IMAGE_LOAD_ADDRESS`` - Defines the address to where the image is loaded and
  is executed from. Only used if ``MCUBOOT_UPGRADE_STRATEGY`` is configured to
  be ``RAM_LOADING``.

Secure Storage (SST) Service definitions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The SST service requires the following definitions:

- ``SST_FLASH_AREA_ADDR`` - Defines the flash address where the secure storage
  area starts.
- ``SST_FLASH_AREA_SIZE`` - Defines the size of the dedicated flash area
  for secure storage in bytes.
- ``SST_SECTOR_SIZE`` - Defines the size of the external flash sectors (the
  smallest erasable unit) in bytes.
- ``SST_SECTORS_PER_BLOCK`` - Defines the number of contiguous SST_SECTOR_SIZE
  to form a logical block in the filesystem.
- ``SST_FLASH_DEV_NAME`` - Specifies the flash device used by SST to store the
  data.
- ``SST_FLASH_PROGRAM_UNIT`` - Defines the smallest flash programmable unit in
  bytes.
- ``SST_MAX_ASSET_SIZE`` - Defines the maximum asset size to be stored in the
  SST area.
- ``SST_NUM_ASSETS`` - Defines the maximum number of assets to be stored in the
  SST area.

.. Note::

    The sectors must be consecutive.

Internal Trusted Storage (ITS) Service definitions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The ITS service requires the following definitions:

- ``ITS_FLASH_AREA_ADDR`` - Defines the flash address where the internal trusted
  storage area starts.
- ``ITS_FLASH_AREA_SIZE`` - Defines the size of the dedicated flash area for
  internal trusted storage in bytes.
- ``ITS_SECTOR_SIZE`` - Defines the size of the internal flash sectors (the
  smallest erasable unit) in bytes.
- ``ITS_SECTORS_PER_BLOCK`` - Defines the number of contiguous ITS_SECTOR_SIZE
  to form a logical block in the filesystem.
- ``ITS_FLASH_DEV_NAME`` - Specifies the internal flash device used by ITS to
  store the data.
- ``ITS_FLASH_PROGRAM_UNIT`` - Defines the smallest flash programmable unit in
  bytes.
- ``ITS_MAX_ASSET_SIZE`` - Defines the maximum asset size to be stored in the
  ITS area.
- ``ITS_NUM_ASSETS`` - Defines the maximum number of assets to be stored in the
  ITS area.

.. Note::

    The sectors must be consecutive.

***************************************
Expose target support for HW components
***************************************
Services may require HW components to be supported by the target to enable some
features (e.g. SST service with rollback protection, etc). The following
definitions need to be set in the .cmake file if the target has the following
HW components:

- ``TARGET_NV_COUNTERS_ENABLE`` - Specifies that the target has non-volatile
  (NV) counters.

--------------

*Copyright (c) 2017-2020, Arm Limited. All rights reserved.*
