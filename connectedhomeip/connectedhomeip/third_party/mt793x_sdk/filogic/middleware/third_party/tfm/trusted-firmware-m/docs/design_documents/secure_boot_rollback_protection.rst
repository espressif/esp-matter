#######################################
Rollback protection in TF-M secure boot
#######################################

:Author: Tamas Ban
:Organization: Arm Limited
:Contact: Tamas Ban <tamas.ban@arm.com>
:Status: Accepted

Anti-rollback protection
========================
The goal of anti-rollback protection is to prevent downgrading of the device to
an older version of its software, which has been deprecated due to security
concerns.

Elements of software upgrade
============================
During a software upgrade the following entities are involved:

 - Boot loader
 - Manifest data: Metadata of the software image: size, version, hash,
   signature, etc.
 - Software image: binary data, elf, etc.

Validation of new image
=======================
Boot loader is responsible to authenticate the new image according to the
required policies and decide whether the new image is fulfilling all the
requirements. Boot loader verifies the image integrity (hash calculation) and
the origination (signature validation), and might verify other requirements as
well. If the new image is successfully authenticated then the boot loader is in
charge to do the necessary steps (load to execute address, etc.) to enable the
new image to be executed. During the validation process the image and the
manifest data is checked.

Manifest format in MCUBoot
==========================
MCUBoot has a custom manifest format, which is composed of two parts:

 - Image header: Prepended to the beginning of the image.
   It is integrity protected:
 - TLV section: Appended to the end of the image. It is not integrity protected:

  - Hash of public key, hash of image, signature of image

.. code-block:: c

    struct image_header {
        uint32_t ih_magic;
        uint32_t ih_load_addr;
        uint16_t ih_hdr_size;
        uint16_t _pad1;
        uint32_t ih_img_size;
        uint32_t ih_flags;
        struct image_version ih_ver;
        uint32_t _pad2;
    };

Security counter
================
The aim of a security counter is to have an independent (from the image version)
counter in the image manifest to ensure anti-rollback protection. During
software release the value of this counter must be increased if a security flaw
was fixed in the current version. Later when this image is installed on the
device then it is not allowed to go back to earlier versions. It is beneficial
to handle this counter independently from image main version number:

 - It does not need to increase with each software release
 - It makes it possible to do software downgrade to some extent: if the security
   counter has the same value in the older image then it is accepted.
 - If the boot loader verifies multiple images then these can be handled
   independently.

However, as an alternative solution the image version number also could serve
as the security counter of the image. Even the version number itself could be
checked during the anti-rollback verification or the value of the security
counter could be derived from the image main version. It is the responsibility
of the software issuer to define which policy to apply.

Implementation of security counter
==================================
The value of the security counter is a security critical data. Any change in
its value has a security implication. Therefore it must be in the integrity
protected part of the image manifest. Because the image header is almost fully
utilised (few unused fields) and the change of image header structure could
lead to compatibility issues between boot loader and runtime software, it is
proposed to extend the integrity protection to some part of the TLV section.
One of the unused fields in the image header can be used to store the size of
the integrity protected area of the TLV section. This is necessary to know how
to calculate properly the image hash and signature. With this extension of the
integrity protected area other attributes that require integrity protection
can easily be added to the image manifest.

Trusted non-volatile (NV) counters
==================================
The Trusted Base System Architecture (TBSA-M) defines non-volatile (NV) counters
or version counters as a counter with the following properties:

 - It must only be possible to increment a version counter through a Trusted
   access.
 - It must only be possible to increment a version counter. It must not be
   possible to decrement it.
 - When a version counter reaches its maximum value, it must not roll over,
   and no further changes must be possible.
 - A version counter must be non-volatile, and the stored value must survive
   a power down period up to the lifetime of the device.

Trusted non-volatile counters can be used to store the value of security
counters per updatable software image. Ideally all independently updatable
software images should have a separate security counter. In current TF-M
implementation the BL2 is not updatable and the secure and non-secure images
are updated together as a single binary. Therefore, one counter is enough to
implement rollback protection. In future the secure and non-secure binaries
will be handled independently; at that time the introduction of a second
counter will be necessary.

Currently the NV counters can be manipulated through the interface described
in ``tfm_plat_nv_counters.h``.

NV counters and anti-rollback protection
========================================
Trusted non-volatile counters might not be supported by a hardware platform.
In this case anti-rollback protection might still be feasible.

The device threat model needs to consider the following aspects:

 - What is the trust level of the boot loader towards the active software

  - If the boot loader cannot protect the anti-rollback mechanism from the
    secure image then the following threat is unmitigated: The content of the
    memory area which holds the active image could be replaced with a valid but
    an older version of the software with software only attack, i.e.: remote
    code execution.
  - If the boot loader does not trust the loaded image at all then security
    counter must have a copy in NV counter area.

 - Another aspect to consider is where the active image is stored

  - Trusted memory: i.e. on-chip flash (eFlash). The threat that a malicious
    actor can modify the content of trusted memory with HW attack is out of
    scope for the current implementation. It is assumed that if an active image
    and related manifest data is stored in trusted memory then the included
    security counter cannot be compromised.
  - Untrusted memory: i.e. external (off-chip) storage, where malicious actor
    can physically access it so it is possible to modify its content, therefore
    the value of included security counter is unreliable.

If the active image is stored in untrusted storage and it is feasible to modify
its content (i.e. replace the whole image to an older version including
corresponding manifest) then the value of security counter must be copied to
the NV counter area. During software validation the boot loader must compare
the new image's security counters against the security counter stored in
non-volatile counters.

If the active image is stored in trusted memory and boot loader trusts in the
active software then it is not mandatory to store the security counter to
non-volatile counter area. During software validation the boot loader is
allowed to compare the new image's security counters against active image's
security counter.

The evaluation of trusted and untrusted memory must be done per target platform
during threat modelling.

For the most robust implementation the following principles should be applied:

 - Always use NV counters for storing security counter value if supported by
   the hardware.
 - Each software stage must not be able to decrease their corresponding NV
   counter.

Boot flow with anti-rollback protection
=======================================
During software upgrade as part of the image validation process the new and
active image security counters must be compared. The new image can be accepted
if its security counter has a greater or equal value than the active image
security counter. From where to extract the active image security counter it
can be platform dependent. It might read out directly from active image
manifest data (only if it is in trusted memory) or the corresponding
non-volatile counter is read.

If non-volatile counters are used to save security counters then their value
must be updated:

 - If the boot loader does not support to revert previous images (just
   overwrites the previously active image with the new one) in case of faulty
   update then the non-volatile counter can be updated to be equal with the
   new image security counter after successful authentication of the new image.
 - If revert is supported then non-volatile counter can be updated just after
   a test run of the new software when its health check is done. Just in case
   of successful health check can the counter updated to avoid the prevention
   of the revert. This might require a secondary restart of the device.

Tool support
============
There is a Python script, ``imgtool.py`` which is used to prepare the new image
for upgrade (add header, sign the image, append TLV section). This script must
be modified to get an additional command line argument which serves as security
counter. The security counter must be included in the integrity protected part
of the TLV section.

--------------

*Copyright (c) 2019, Arm Limited. All rights reserved.*
