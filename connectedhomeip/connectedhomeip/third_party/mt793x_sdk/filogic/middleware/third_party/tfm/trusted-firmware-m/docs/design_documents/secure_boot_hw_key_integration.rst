HW crypto key integration in TF-M secure boot
=============================================

:Author: Tamas Ban
:Organization: Arm Limited
:Contact: Tamas Ban <tamas.ban@arm.com>
:Status: Accepted

Abstract
--------

`PSA Trusted Boot and Firmware Update <https://pages.arm.com/psa-resources-tbfu.html>`__
specification requires the support of at least one immutable root of trust
public key (ROTPK) for firmware verification. This can be stored using a locked
on-chip flash memory, a secure-element or on-chip OTP memory. It also beneficial
to be able to provision these keys during the factory life-cycle of the device
independently from any software components. The current key handling solution
in TF-M secure boot does not supports this key provisioning process. MCUBoot
requires compile time built-in public key(s) for image verification. This
limitation does not fit well with a scenario with multiple vendors where
multiple MCU software components might be deployed by different vendors in
different points in the life-cycle of the device and they do not want to share
the keys in-advance for embedding in the bootloader code. The goal of this
document to propose a solution to decouple MCUBoot from public key(s) and
enable the independent deployment of ROTPK.

Existing key handling solution
------------------------------

MCUBoot code contains a compile-time built-in key array which can store any
number of key(s) for firmware verification: ``bl2/ext/mcuboot/keys.c``. These
public key(s) must be available when MCUBoot image is built. There is a script
``bl2/ext/mcuboot/scipt/imgtool.py`` which can generate a new key pair, and
extract the public key part in the expected ASN.1 format and encode it as C
structure. The script is also capable of signing the image with the private key.
In order to identify and validate the corresponding public key during image
verification the hash of the public key is appended to the image manifest area
(TLV encoded metadata). During image verification the bootloader retrieves the
hash of the public key from the manifest area and compare against the on-the-fly
calculated hash value of the built-in public keys. An exact match identifies and
validates the public key which must be used for image verification.

Current memory layout::

    |----------------------|
    |                      |
    |     MCUBoot code     |
    |                      |
    |   Full public key    |
    |                      |
    |----------------------|
    |                      |
    |       Image          |
    |                      |
    |----------------------|
    |  Image Manifest(TLV) |
    |                      |
    |  Hash of public key  |
    |----------------------|
    |                      |
    |   Rest of memory     |
    |                      |

Requirements
------------

- Multiple independent vendor scenario must be supported, when more than one
  ROTPK is provisioned to the device.
- The corresponding public key for image verification must be identifiable and
  verifiable.
- Key identity must be immutable and anchored to the device itself.
- Key(s) can be provisioned independently from bootloader.

Design proposal
---------------
HW key(s) might stored in OTP memory which is an expensive resource, so
storing a large key (such as RSA) is inefficient. Therefore, it is assumed that
only the hash of the ROTPK will be stored in the HW. If only the hash of the
public key is stored in the HW then the whole public key must be transfered to
the device, because it must be available during image verification. This
transfer can be done in the same way as how the hash of the key is transfered
to the device with the current solution. A new TLV type (TLV_KEY) can be
introduced to carry the whole public key. The corresponding public key will be
appended to the image itself in the manifest area. It has the drawback that the
image will be bigger, but it is assumed that the additional cost of the bigger
image (extra flash area + power for download) is less than the cost of the OTP
area.

The verification flow:

 - Look up the TLV_KEY field to get the public key.
 - Calculates its hash(SHA256) value.
 - Get the hash of ROTPK from the platform layer (stored in HW)
 - Compare the two hash values, if they match then the key can be used to
   validate the image. In case of failure consider the images as unauthenticated.

Proposed memory layout::

    |----------------------|
    |                      |
    |     MCUBoot code     |
    |                      |
    |    NO PUBLIC KEY     |
    |                      |
    |----------------------|
    |                      |
    |       Image          |
    |                      |
    |----------------------|
    |  Image Manifest(TLV) |
    |                      |
    |   Full public key    |
    |----------------------|
    |                      |
    |                      |
    |   Rest of memory     |
    |                      |
    |                      |
    |----------------------|
    |   Immutable memory   |
    |                      |
    |  Hash of public key  |
    |----------------------|
    |                      |

Platform support
----------------

A new platform API used by the bootloader must be introduced to retrieve the
image corresponding hash of ROTPK:

``enum tfm_plat_err_t tfm_plat_get_rotpk_hash(uint8_t image_id,
uint8_t *rotpk_hash, uint32_t *rotpk_hash_size);``

The mapping between image identity and public key can be hard-code in the
platform layer. This simplifies the validation of the public key, because no
look-up would be needed. It is assumed that the memory area of the ROTPK hash is
not directly accessible, therefore a buffer is allocated by the caller to store
the hash there.

Compile time configurability
----------------------------

The solution must be compile time configurable in order to be able to switch
between built-in key(s) and HW key(s) support, and also due to backwards
compatibility.

Tooling
-------

``bl2/ext/mcuboot/scipt/imgtool.py`` will be modified to include the whole
public key to the TLV area (instead of the hash).

Table to compare the current and proposed solution::

    |---------|-----------------------|-------------------|--------------------|
    |         |Key format in manifest |Key in MCUBoot code|     Key in HW      |
    |---------|-----------------------|-------------------|--------------------|
    |Proposed |    Full public key    |  No key embedded  | Hash of public key |
    |---------|-----------------------|-------------------|--------------------|
    |Current  |   Hash of public key  |  Full public key  |   No key in HW     |
    |---------|-----------------------|-------------------|--------------------|

--------------

*Copyright (c) 2019, Arm Limited. All rights reserved.*
