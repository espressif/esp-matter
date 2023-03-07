########################################
Secure Storage Service Integration Guide
########################################

************
Introduction
************
TF-M Secure Storage (SST) service implements PSA Protected Storage APIs.

The service is backed by hardware isolation of the flash access domain and, in
the current version, relies on hardware to isolate the flash area from
non-secure access. In absence of hardware level isolation, the secrecy and
integrity of data is still maintained.

The SST service implements an AES-GCM based AEAD encryption policy, as a
reference, to protect data integrity and authenticity.

SST reuses the non-hierarchical filesystem provided by the TF-M Internal Trusted
Storage service to store encrypted, authenticated objects on the external flash
device.

The design addresses the following high level requirements as well:

- **Confidentiality** - Resistance to unauthorised accesses through
  hardware/software attacks.

- **Access Authentication** - Mechanism to establish requester's identity (a
  non-secure entity, secure entity, or a remote server).

- **Integrity** - Resistant to tampering by either the normal users of a product,
  package, or system or others with physical access to it. If the content of the
  secure storage is changed maliciously, the service is able to detect it.

- **Reliability** - Resistant to power failure scenarios and incomplete write
  cycles.

- **Configurability** - High level configurability to scale up/down memory
  footprint to cater for a variety of devices with varying security
  requirements.

- **Performance** - Optimized to be used for resource constrained devices with
  very small silicon footprint, the PPA (power, performance, area) should be
  optimal.

*******************************
Current SST Service Limitations
*******************************
- **Fragmentation** - The current design does not support fragmentation, as an
  asset is stored in a contiguous space in a block. This means that the maximum
  asset size can only be up-to a block size. Detailed information about the
  maximum asset size can be found in the section `Maximum asset size` below.
  Each block can potentially store multiple assets.
  A delete operation implicitly moves all the assets towards the top of the block
  to avoid fragmentation within block. However, this may also result in
  unutilized space at the end of each block.

- **Asset size limitation** - An asset is stored in a contiguous space in a
  block/sector. Hence, the maximum asset size can be up-to the size of the
  data block/sector. Detailed information about the maximum asset size can be
  found in the section `Maximum asset size` below.

- **Non-hierarchical storage model** - The current design uses a
  non-hierarchical storage model, as a filesystem, where all the assets are
  managed by a linearly indexed list of metadata. This model locates the
  metadata in blocks which are always stored in the same flash location. That
  increases the number of writes in a specific flash location as every change in
  the storage area requires a metadata update.

- **PSA internal trusted storage API** - In the current design, the service does
  not use the PSA Internal Trusted Storage API to write the rollback protection
  values stored in the internal storage.

- **Protection against physical storage medium failure** - Complete handling of
  inherent failures of storage mediums (e.g. bad blocks in a NAND based device)
  is not supported by the current design.

- **Key diversification** - In a more robust design, each asset would be
  encrypted through a different key.

- **Lifecycle management** - Currently, it does not support any subscription
  based keys and certificates required in a secure lifecycle management. Hence,
  an asset's validity time-stamp can not be invalidated based on the system
  time.

- **Provisioning vs user/device data** - In the current design, all assets are
  treated in the same manner. In an alternative design, it may be required to
  create separate partitions for provisioning content and user/device generated
  content. This is to allow safe update of provisioning data during firmware
  updates without the need to wipe out the user/device generated data.

**************
Code Structure
**************
Secure storage service code is located in ``secure_fw/services/secure_storage/``
and is divided as follows:

    - Core files
    - Cryptographic interfaces
    - Non-volatile (NV) counters interfaces

The PSA PS interfaces for SST service are located in ``interface/include/psa``

PSA Protected Storage Interfaces
================================

The SST service exposes the following mandatory PSA PS interfaces, version 1.0:

.. code-block:: c

    psa_status_t psa_ps_set(psa_storage_uid_t uid, size_t data_length, const void *p_data, psa_storage_create_flags_t create_flags);
    psa_status_t psa_ps_get(psa_storage_uid_t uid, size_t data_offset, size_t data_size, void *p_data, size_t *p_data_length);
    psa_status_t psa_ps_get_info(psa_storage_uid_t uid, struct psa_storage_info_t *p_info);
    psa_status_t psa_ps_remove(psa_storage_uid_t uid);
    uint32_t psa_ps_get_support(void);

For the moment, it does not support the extended version of those APIs.

These PSA PS interfaces and SST TF-M types are defined and documented in
``interface/include/psa/protected_storage.h``,
``interface/include/psa/storage_common.h`` and
``interface/include/tfm_sst_defs.h``

Core Files
==========
- ``tfm_sst_req_mngr.c`` - Contains the SST request manager implementation which
  handles all requests which arrive to the service. This layer extracts the
  arguments from the input and output vectors, and it calls the protected
  storage layer with the provided parameters.

- ``tfm_protected_storage.c`` - Contains the TF-M protected storage API
  implementations which are the entry points to the SST service.

- ``sst_object_system.c`` - Contains the object system implementation to manage
  all objects in SST area.

- ``sst_object_table.c`` - Contains the object system table implementation which
  complements the object system to manage all object in the SST area.
  The object table has an entry for each object stored in the object system
  and keeps track of its version and owner.

- ``sst_encrypted_object.c`` - Contains an implementation to manipulate
  encrypted objects in the SST object system.

- ``sst_utils.c`` - Contains common and basic functionalities used across the
  SST service code.

Flash Filesystem and Flash Interfaces
=====================================
The SST service reuses the non-hierarchical filesystem and flash interfaces
provided by the TF-M Internal Trusted Storage service. It stores encrypted,
authenticated objects on the external flash device by making service calls to
the ITS service. When the ITS service receives requests from the SST partition,
it handles the request by using a separate filesystem context initialised to use
the external flash device.

The ITS filesystem and flash interfaces and their implementation can be found in
``secure_fw/services/internal_trusted_storage/flash_fs`` and
``secure_fw/services/internal_trusted_storage/flash`` respectively. More
information about the filesystem and flash interfaces can be found in the
:doc:`ITS integration guide
</docs/user_guides/services/tfm_its_integration_guide>`.

The structure containing info about the external flash device, used by the ITS
service to handle requests from the SST partition, is defined in
``secure_fw/services/internal_trusted_storage/flash/its_flash_info_external.c``,
which depends on target-specific definitions from
``platform/ext/target/<TARGET_NAME>/partition/flash_layout.h``. Please see the
`Secure Storage Service Definitions` section for details.

Cryptographic Interface
=======================
- ``crypto/sst_crypto_interface.h`` - Abstracts the cryptographic operations for
  the secure storage service.

- ``crypto/sst_crypto_interface.c`` - Implements the SST service cryptographic
  operations with calls to the TF-M Crypto service.

Non-volatile (NV) Counters Interface
====================================
The current SST service provides rollback protection based on NV
counters.
SST defines and implements the following NV counters functionalities:

- ``nv_counters/sst_nv_counters.h`` - Abstracts SST non-volatile
  counters operations. This API detaches the use of NV counters from the TF-M NV
  counters implementation, provided by the platform, and provides a mechanism to
  compile in a different API implementation for test purposes. A SST test suite
  **may** provide its own custom implementation to be able to test different SST
  service use cases.

- ``nv_counters/sst_nv_counters.c`` - Implements the SST NV counters interfaces
  based on TF-M NV counters implementation provided by the platform.

*****************************
SST Service Integration Guide
*****************************
This section describes mandatory (i.e. **must** implement) or optional
(i.e. **may** implement) interfaces which the system integrator have to take
in to account in order to integrate the secure storage service in a new
platform.

Maximum Asset Size
==================
An asset is stored in a contiguous space in a block/sector. The maximum
size of an asset can be up-to the size of the data block/sector minus the object
header size (``SST_OBJECT_HEADER_SIZE``) which is defined in
``sst_object_defs.h``. The ``SST_OBJECT_HEADER_SIZE`` changes based on the
``SST_ENCRYPTION`` flag status.

Secure Storage Service Definitions
==================================
The SST service requires the following platform definitions:

- ``SST_FLASH_AREA_ADDR`` - Defines the flash address where the secure storage
  area starts.
- ``SST_FLASH_AREA_SIZE`` - Defines the size of the dedicated flash area
  for secure storage in bytes.
- ``SST_SECTOR_SIZE`` - Defines the size of the flash sectors (the smallest
  erasable unit) in bytes.
- ``SST_SECTORS_PER_BLOCK`` - Defines the number of contiguous SST_SECTOR_SIZE
  to form a logical block in the filesystem.
- ``SST_FLASH_DEV_NAME`` - Specifies the flash device used by SST to store the
  data.
- ``SST_FLASH_PROGRAM_UNIT`` - Defines the smallest flash programmable unit in
  bytes. Valid values are powers of two between 1 and ``SST_SECTOR_SIZE``
  inclusive.
- ``SST_MAX_ASSET_SIZE`` - Defines the maximum asset size to be stored in the
  SST area. This size is used to define the temporary buffers used by SST to
  read/write the asset content from/to flash. The memory used by the temporary
  buffers is allocated statically as SST does not use dynamic memory allocation.
- ``SST_NUM_ASSETS`` - Defines the maximum number of assets to be stored in the
  SST area. This number is used to dimension statically the object table size in
  RAM (fast access) and flash (persistent storage). The memory used by the
  object table is allocated statically as SST does not use dynamic memory
  allocation.

The sectors reserved to be used as secure storage **must** be contiguous sectors
starting at ``SST_FLASH_AREA_ADDR``.

The design requires either 2 blocks, or any number of blocks greater than or
equal to 4. Total number of blocks can not be 0, 1 or 3. This is a design choice
limitation to provide power failure safe update operations.

Target must provide a header file, called ``flash_layout.h``, which defines the
information explained above. The defines must be named as they are specified
above.

More information about the ``flash_layout.h`` content, not SST related, is
available in :doc:`platform readme </platform/ext/readme>` along with other
platform information.

TF-M NV Counter Interface
=========================
To have a platform independent way to access the NV counters, TF-M defines a
platform NV counter interface. For API specification, please check:
``platform/include/tfm_plat_nv_counters.h``

The system integrators **may** implement this interface based on the target
capabilities and set the ``SST_ROLLBACK_PROTECTION`` flag to compile in
the rollback protection code.

Secret Platform Unique Key
==========================
The encryption policy relies on a secret hardware unique key (HUK) per device.
It is system integrator's responsibility to provide an implementation which
**must** be a non-mutable target implementation.
For API specification, please check:
``platform/include/tfm_plat_crypto_keys.h``

A stub implementation is provided in
``platform/ext/common/template/crypto_keys.c``

Non-Secure Identity Manager
===========================
TF-M core tracks the current client IDs running in the secure or non-secure
processing environment. It provides a dedicated API to retrieve the client ID
which performs the service request.

:doc:`NS client identification documentation </docs/user_guides/tfm_ns_client_identification>`
provides further details on how client identification works.

SST service uses that TF-M core API to retrieve the client ID and associate it
as the owner of an asset. Only the owner can read, write or delete that asset
based on the creation flags.

The :doc:`integration guide </docs/user_guides/tfm_integration_guide>` provides further
details of non-secure implementation requirements for TF-M.

Cryptographic Interface
=======================
The reference encryption policy is built on AES-GCM, and it **may** be replaced
by a vendor specific implementation.

The SST service abstracts all the cryptographic requirements and specifies the
required cryptographic interface in
``secure_fw/services/secure_storage/crypto/sst_crypto_interface.h``

The SST service cryptographic operations are implemented in
``secure_fw/services/secure_storage/crypto/sst_crypto_interface.c``, using calls
to the TF-M Crypto service.

SST Service Features Flags
==========================
SST service defines a set of flags that can be used to compile in/out certain
SST service features. The ``CommonConfig.cmake`` file sets the default values
of those flags. However, those flags values can be overwritten by setting them
in ``platform/ext/<TARGET_NAME>.cmake`` based on the target capabilities or
needs. The list of SST services flags are:

- ``SST_ENCRYPTION``- this flag allows to enable/disable encryption
  option to encrypt the secure storage data.
- ``SST_CREATE_FLASH_LAYOUT``- this flag indicates that it is required
  to create a SST flash layout. If this flag is set, SST service will
  generate an empty and valid SST flash layout to store assets. It will
  erase all data located in the assigned SST memory area before generating
  the SST layout.  This flag is required to be set if the SST memory area
  is located in a non-persistent memory.  This flag can be set if the SST
  memory area is located in a persistent memory without a valid SST flash
  layout in it. That is the case when it is the first time in the device
  life that the SST service is executed.
- ``SST_VALIDATE_METADATA_FROM_FLASH``- this flag allows to
  enable/disable the validation mechanism to check the metadata store in flash
  every time the flash data is read from flash. This validation is required
  if the flash is not hardware protected against malicious writes. In case
  the flash is protected against malicious writes (i.e embedded flash, etc),
  this validation can be disabled in order to reduce the validation overhead.
- ``SST_ROLLBACK_PROTECTION``- this flag allows to enable/disable
  rollback protection in secure storage service. This flag takes effect only
  if the target has non-volatile counters and ``SST_ENCRYPTION`` flag is on.
- ``SST_RAM_FS``- this flag allows to enable/disable the use of RAM
  instead of the flash to store the FS in secure storage service. This flag
  is set by default in the regression tests, if it is not defined by the
  platform.  The SST regression tests reduce the life of the flash memory
  as they write/erase multiple times in the memory.

  .. Note::
    If this flag is manually disabled when running the regression tests,
    then the storage flash area should also be erased before running the tests
    to ensure they run successfully. The type of storage flash area is platform
    specific (QSPI, eFlash, etc.) and it is described in corresponding
    flash_layout.h

- ``SST_TEST_NV_COUNTERS``- this flag enables the virtual
  implementation of the SST NV counters interface in
  ``test/suites/sst/secure/nv_counters``, which emulates NV counters in
  RAM, and disables the hardware implementation of NV counters provided by
  the secure service. This flag is enabled by default when building the
  regression tests and disabled by default otherwise.  This flag can be
  overridden to ``OFF`` when building the regression tests. In this case,
  the SST rollback protection test suite will not be built, as it relies
  on extra functionality provided by the virtual NV counters to simulate
  different rollback scenarios. The remainder of the SST test suites will
  run using the hardware NV counters. Please note that running the tests in
  this configuration will quickly increase the hardware NV counter values,
  which cannot be decreased again.
  Overriding this flag from its default value of ``OFF`` when not
  building the regression tests is not currently supported.

--------------

*Copyright (c) 2018-2020, Arm Limited. All rights reserved.*
