======================================
Internal Trusted Storage (ITS) Service
======================================

:Author: Jamie Fox
:Organization: Arm Limited
:Contact: Jamie Fox <jamie.fox@arm.com>
:Status: Accepted

PSA Internal Trusted Storage
============================
PSA Internal Trusted Storage (ITS) is a PSA RoT Service for storing the most
security-critical device data (e.g. cryptographic keys) in internal storage,
which is trusted to provide data confidentiality and authenticity. This
contrasts with PSA Protected Storage, which is an Application RoT service that
allows larger data sets to be stored securely in external flash, with the option
for encryption, authentication and rollback protection to protect the
data-at-rest.

Current TF-M Secure Storage
===========================
Currently, the TF-M Secure Storage service implements PSA Protected Storage
version 1.0-beta2. There is not yet an implementation of PSA Internal Trusted
Storage in TF-M.

New TF-M service
================
The proposal is to implement the *PSA Internal Trusted Storage API* with the
*TF-M Internal Trusted Storage service*. It can be abbreviated to *TF-M ITS
service* in general and to ``its`` in code. This name has the advantage of
making clear the correspondence between the service and the API it implements.

If this name is adopted, then it may make sense to rename the *Secure Storage
service* to the *Protected Storage service* in the future to match. Then "secure
storage" could refer to the two services as a collective.

The TF-M ITS service will implement PSA ITS version 1.0. It will be provided by
a separate partition to Protected Storage, for a couple of reasons:

- To permit isolation between the services.

  - ITS is a PSA RoT Service, while Protected Storage is an Application RoT
    Service.

- To avoid circular dependencies.

  - The PSA Firmware Framework does not permit circular dependencies between
    partitions, which would occur if Protected Storage and ITS were provided by
    the same partition. Protected Storage depends on Crypto, which in turn
    depends on ITS.

The existing SST filesystem will be reused to provide the backend of the
service, with the flash layer modified to direct storage to internal flash,
rather than external.

Compared to Protected Storage, encryption, authentication and rollback
protection are not required, so the SST encrypted object layer and the crypto
and NV counter interfaces are not required. The rollback protection feature of
the object table is also not required.

Code structure
==============
The code structure of the service will be as follows:

``interface/``

- ``include/psa/internal_trusted_storage.h`` - PSA ITS API
- ``src/tfm_its_api.c`` - PSA ITS API implementation for NSPE

``secure_fw/ns_callable/tfm_veneers.c`` - ITS veneers (auto-generated from
manifest)

``secure_fw/services/internal_trusted_storage/``

- ``tfm_internal_trusted_storage.yaml`` - Partition manifest
- ``tfm_its_secure_api.c`` - PSA ITS API implementation for SPE
- ``tfm_its_req_mngr.c`` - Uniform secure functions and IPC request handlers
- ``tfm_internal_trusted_storage.h`` - TF-M ITS API (with client_id parameter)
- ``tfm_internal_trusted_storage.c`` - TF-M ITS implementation, using the
  flash_fs as a backend
- ``flash_fs/`` - Filesystem
- ``flash/`` - Flash interface

``test/suites/its/``

- ``non_secure/psa_its_ns_interface_testsuite.c`` - Non-secure interface tests
- ``secure/psa_its_s_interface_testsuite.c`` - Secure interface tests

TF-M ITS implementation
-----------------------
The following APIs will be exposed by ``tfm_internal_trusted_storage.h``::

    psa_status_t tfm_its_init(void);

    psa_status_t tfm_its_set(int32_t client_id,
                             psa_storage_uid_t uid,
                             size_t data_length,
                             const void *p_data,
                             psa_storage_create_flags_t create_flags);

    psa_status_t tfm_its_get(int32_t client_id,
                             psa_storage_uid_t uid,
                             size_t data_offset,
                             size_t data_size,
                             void *p_data,
                             size_t *p_data_length);

    psa_status_t tfm_its_get_info(int32_t client_id,
                                  psa_storage_uid_t uid,
                                  struct psa_storage_info_t *p_info);

    psa_status_t tfm_its_remove(int32_t client_id,
                                psa_storage_uid_t uid);

That is, the TF-M ITS APIs will have the same prototypes as the PSA ITS APIs,
but with the addition of a ``client_id`` parameter, which will be passed from
the ITS request manager. A ``tfm_its_init`` function will also be present, which
will be called at initialisation time and not exposed through a veneer or SID.

The implementation in ``tfm_internal_trusted_storage.c`` must validate the
parameters (excepting memory references, which are validated by the SPM),
translate the UID and client ID into a file ID and then make appropriate calls
to the filesystem layer. It must also take care ensure that any PSA Storage
flags associated with the UID are honoured.

Filesystem
----------
The ITS filesystem will be copied and modified from the SST filesystem. The
modifications required will be to rename symbols from ``sst`` to ``its`` and to
update the implementation to be aligned with the latest version of the PSA
Storage spec (which consists mainly of moving to the ``psa_status_t`` error type
and using common error codes from ``psa/error.h``).

The filesystem will also be modified to align the size of each file stored to
the alignment requirement exposed by the flash interface, by adding appropriate
padding.

The filesystem code will be de-duplicated again once the ITS service is
implemented (see below).

Flash layer
-----------
The flash layer will be copied from SST, and modified to direct writes to the
internal flash device. It too needs to be updated to use ``psa_status_t`` error
types.

Platform layer
--------------
The TF-M platform layer must be be updated to distinguish between the external
flash device used for Protected Storage and internal flash device used for ITS.
A flash region for the relevant storage service needs to be allocated in each.

On test platforms these may just be two distinct regions of the same flash
device, but in general they will separate devices with their own drivers.

Detailed design considerations
==============================

Mapping UID onto file ID
------------------------
The ITS APIs identify assets with 64-bit UIDs, to which the ITS service must
append the 32-bit client ID of the calling partition for access control. The
existing filesystem uses 32-bit file IDs to identify files, so some mapping
would be required to convert between the identifiers.

SST uses the object table to do the mapping from client ID, UID pairs to file
IDs, which means making an extra filesystem read/write for each get/set
operation. This mapping has minimal overhead for SST though, because object
table lookups are already required for rollback protection.

For ITS, no rollback protection feature is required, so there are two options:

- Keep a simplified version of the SST object table that just maps from
  (client ID, UID) to file ID

- Modify the filesystem to take (at least) 96-bit file IDs, in the form of a
  fixed-length char buffer.

The advantage of the former is that it would require no extra modification to
the existing filesystem code, and the existing SST object table could be cut
down for ITS. However, it would mean that every ITS request would invoke twice
the number of filesystem operations, increasing latency and flash wear. The code
size of the ITS partition would be increased, as would RAM usage as the table
would need to be read into RAM.

The latter option would make the filesystem slightly more complex: the size of a
metadata entry would be increased by 64-bits and the 96-bit fids would need to
be copied and compared with ``memcpy`` and ``memcmp`` calls. On the other hand,
mapping onto file IDs would incur only the cost of copying the UID and client ID
values into the file ID buffer.

A third, even more general, solution would be to use arbitrary-length
null-terminated strings as the file IDs. This is the standard solution in
full-featured filesystems, but we do not currently require this level of
complexity in secure storage.

With this in mind, the proposed option is the second.

Storing create flags
--------------------
The ITS APIs provide a 32-bit ``create_flags`` parameter, which contains bit
flags that determine the properties of the stored data. Only one flag is
currently defined for ITS: ``PSA_STORAGE_FLAG_WRITE_ONCE``, which prevents a UID
from being modified or deleted after it is set for the first time.

There are two places that these flags could be stored: in the file data or as
part of the file metadata.

For the first option, the ITS implementation would need to copy to the flags
into the buffer containing the data, and adjust the size accordingly, for each
set operation, and the reverse for each get. Every get_info operation would need
to read some of the file data, rather than just the metadata, implying a second
flash read. A potential downside is that many of the cryptographic assets stored
in ITS will be aligned to power-of-two sizes; adding an extra 32-bits would
misalign the size, which may reduce flash performance or necessitate adding
padding to align to the flash page size.

To implement the second option, a 32-bit ``flag`` field would be added to the
filesystem's metadata structure, whose interpretation is defined by the user.
This field would clearly be catered towards the PSA Storage APIs, even if
nominally generic, and alternative filesystems may not have any such field.
However, it is a more intuitive solution and would simplify both flash alignment
and get_info operations.

Overall, it seems more beneficial to store the flags in the metadata, so this is
the proposed solution.

Code sharing between Protected Storage and ITS
----------------------------------------------
To de-duplicate the filesystem code used by both Protected Storage and ITS, it
is proposed that Protected Storage calls ITS APIs as its backend filesystem.

Protected Storage essentially becomes an encryption, authentication and rollback
protection layer on top of ITS. It makes IPC requests or secure function calls
to the ITS service to do filesystem operations on its behalf.

This has a couple of advantages:

- It shrinks Protected Storage's stack size, because the filesystem and flash
  layer stack is only in ITS.

- It automatically solves the problem of ensuring mutual exclusion in the
  filesystem and flash layers when Protected Storage and ITS are called
  concurrently. The second request to ITS will just be made to wait by the SPM.

The disadvantage of this approach is that it will increase the latency of
Protected Storage requests, due to the extra overhead associated with making a
second IPC request or secure function call. It also limits Protected Storage to
using only the ITS APIs, unless extra veneers are added solely for Protected
Storage to use. This, for example, prevents Protected Storage from doing partial
writes to file without reading and re-writing the whole file.

ITS will need to be modified to direct calls from Protected Storage to a
different flash device. It can use the client ID to detect when the caller is
Protected Storage, and pass down the identity of the flash device to use to the
flash layer, which then calls the appropriate driver.

An open question is what to do if Protected Storage itself wants to store
something in internal storage in the future (e.g. rollback counters, hash
tree/table or top hash). A couple of possible solutions would be:

- Divide up the UIDs, so certain UIDs from Protected Storage refer to assets in
  internal storage, and others to ones in external storage.

- Use the ``type`` field of ``psa_call`` in IPC model and extra veneers in
  library model to distinguish between internal and external storage requests.

The other option for code sharing would be for Protected Storage and ITS to
directly share filesystem code, which would be placed in a shared code region.
With this approach, mutual exclusion to the flash device would need to be
implemented separately, as would some way of isolating static memory belonging
to each partition but not the code. Because of these complications, this option
has not been considered further at this time.

--------------

*Copyright (c) 2019, Arm Limited. All rights reserved.*
