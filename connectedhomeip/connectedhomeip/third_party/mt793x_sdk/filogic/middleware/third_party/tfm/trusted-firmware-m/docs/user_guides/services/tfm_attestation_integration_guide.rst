#############################################
Initial Attestation Service Integration Guide
#############################################

************
Introduction
************
TF-M Initial Attestation Service allows the application to prove the device
identity during an authentication process to a verification entity. The initial
attestation service can create a token on request, which contains a fix set of
device specific data. Device must contain an attestation key pair, which is
unique per device. The token is signed with the private part of attestation key
pair. The public part of the key pair is known by the verification entity. The
public key is used to verify the token authenticity. The data items in the token
used to verify the device integrity and assess its trustworthiness. Attestation
key provisioning is out of scope for the attestation service and is expected to
take part during manufacturing of the device.

***************************************
Claims in the initial attestation token
***************************************
The initial attestation token is formed of claims. A claim is a data item,
which is represented in a key - value structure. The following fixed set of
claims are included in the token:

    - **Auth challenge**: Input object from caller. Can be a single nonce from
      server or hash of nonce and attested data. It is intended to provide
      freshness to report and the caller has responsibility to arrange
      this. Allowed length: 32, 48, 64 bytes. The claim is modeled to be
      eventually represented by the EAT standard claim nonce. Until such a
      time as that standard exists, the claim will be represented by a custom
      claim. Value is encoded as byte string.

    - **Instance ID**: It represents the unique identifier of the instance. In
      the PSA definition it is a hash of the public attestation key of the
      instance. The claim is modeled to be eventually represented by the EAT
      standard claim UEID of type GUID. Until such a time as that standard
      exists, the claim will be represented by a custom claim  Value is encoded
      as byte string.

    - **Verification service indicator**: Optional, recommended claim. It
      is used by a Relying Party to locate a validation service for the
      token. The value is a text string that can be used to locate the service
      or a URL specifying the address of the service. The claim is modelled to
      be eventually represented by the EAT standard claim origination. Until
      such a time as that standard exists, the claim will be represented by
      a custom claim. Value is encoded as text string.

    - **Profile definition**: Optional, recommended claim. It contains the
      name of a document that describes the 'profile' of the token, being
      a full description of the claims, their usage, verification and token
      signing. The document name may include versioning. Custom claim with a
      value encoded as text string.

    - **Implementation ID**: Uniquely identifies the underlying immutable PSA
      RoT. A verification service can use this claim to locate the details of
      the verification process. Such details include the implementation’s origin
      and associated certification state. Custom claim with a value encoded as
      byte string.

    - **Client ID**: The partition ID of that secure partition or non-secure
      thread who called the initial attestation API. Custom claim with a value
      encoded as a `signed` integer. Negative number represents non-secure
      caller, positive numbers represents secure callers, zero is invalid.

    - **Security lifecycle**: It represents the current lifecycle state of
      the instance. Custom claim with a value encoded as an integer.

    - **Hardware version**: Optional claim. Globally unique number in EAN-13
      format identifying the GDSII that went to fabrication, HW and ROM. It can
      be used to reference the security level of the PSA-ROT via a certification
      website. Custom claim with a value is encoded as text string.

    - **Boot seed**: It represents a random value created at system boot
      time that will allow differentiation of reports from different system
      sessions. The size is 32 bytes. Custom claim with a value is encoded as
      byte string.

    - **Software components**: Optional, but required in order to be compliant
      with the PSA-SM. It represents the software state of the system. The value
      of the claim is an array of CBOR map entries, with one entry per software
      component within the device. Each map contains multiple claims that
      describe evidence about the details of the software component.

    - **No software measurements**: Optional, but required if no software
      component claims are made. In the event that the implementation does not
      contain any software measurements then it is mandatory to include this
      claim to indicate this is a deliberate state. Custom claim with a value
      encoded as an unsigned integer set to 1.

Each software component claim can include the following properties. Any property
that is not optional must be included:

    - **Measurement type**: Optional claim. It represents the role of the
      software component. Value is encoded as short(!) text string.

    - **Measurement value**: It represents a hash of the invariant software
      component in memory at start-up time. The value must be a cryptographic
      hash of 256 bits or stronger. Value is encoded as byte string.

    - **Version**: Optional claim. It represents the issued software
      version. Value is encoded as text string.

    - **Signer ID**: Optional claim, but required in order to be compliant with
      the PSA-SM. It represents the hash of a signing authority public key.
      Value is encoded as byte string.

    - **Measurement description**: Optional claim. It represents the way in
      which the measurement value of the software component is computed. Value
      is encoded as text string containing an abbreviated description (name)
      of the measurement method.

*********************************************
Initial attestation token (IAT) data encoding
*********************************************
The initial attestation token is planned to be aligned with future version of
`Entity Attestation Token <https://tools.ietf.org/html/draft-mandyam-eat-01>`__
format. The token is encoded according to the
`CBOR <https://tools.ietf.org/html/rfc7049>`__ format and signed according to
`COSE <https://tools.ietf.org/html/rfc8152>`__ standard.

**************
Code structure
**************
The PSA interface for the Initial Attestation Service is located in
``interface/include``. The only header to be included by applications that want
to use functions from the PSA API is ``psa/initial_attestation.h``.

The TF-M Initial Attestation Service source files are located in
``secure_fw/services/initial_attestation``.
The CBOR library is located in ``lib/ext/qcbor`` folder.

Service source files
====================
- CBOR library
    - ``lib/ext/qcbor`` This library is used to create a proper CBOR token.
      It can be used on 32-bit and 64-bit machines.  It was designed to suite
      constrained devices with low memory usage and without dynamic memory
      allocation.
      It is a fork of this external `QCBOR library <https://github.com/laurencelundblade/QCBOR>`__.
    - ``lib/ext/qcbor/inc/qcbor.h``: Public API documentation of CBOR
      library.

- COSE library:
    - ``lib/t_cose``: This library is used to sign a CBOR token and create
      the COSE header and signature around the initial attestation token. Only
      a subset of the `COSE <https://tools.ietf.org/html/rfc8152>`__ standard
      is implemented. Only the cose_sign1 signature schema is supported.
    - ``lib/t_cose/src/t_cose_crypto.h``: Expose an API to bind ``t_cose``
      library with available crypto library in the device.
    - ``lib/t_cose/src/t_cose_psa_crypto.c``: Implements the exposed API
      and ports ``t_cose`` to the PSA Crypto API.
- Initial Attestation Service:
    - ``attestation_core.c`` : Implements core functionalities such as
      implementation of APIs, retrieval of claims and token creation.
    - ``attest_token.c``: Implements the token creation function such as
      start and finish token creation and adding claims to the token.
    - ``attestation_key.c``: Get the attestation key from platform layer
      and register it to the TF-M Crypto service for further usage.
    - ``tfm_attestation.c``: Implements the SPM abstraction layer, and bind
      the attestation service to the SPM implementation in TF-M project.
    - ``tfm_attestation_secure_api.c``: Implements the secure API layer to
      allow other services in the secure domain to request functionalities
      from the attestation service using the PSA API interface.
    - ``tfm_attestation_req_mngr.c``: Includes the initialization entry of
      attestation service and handles attestation service requests in IPC
      model.

Service interface definitions
=============================
- **Boot loader interface**: The attestation service might include data
  in the token about the distinct software components in the device. This data
  is provided by the boot loader and must be encoded in the TLV format,
  definition is described below in the boot loader interface paragraph. Possible
  claims in the boot status are describe above in the software components
  paragraph.
- **Hardware abstraction layer**:
    - Headers are located in ``platform/include`` folder.
    - ``tfm_attest_hal.h``: Expose an API to get the following claims:
      security lifecycle, verification service indicator, profile definition.
    - ``tfm_plat_boot_seed.h``: Expose an API to get the boot seed claim.
    - ``tfm_plat_device_id.h``: Expose an API to get the following claims:
      implementation ID, hardware version, instance ID.
- **SPM interface**:
    - ``attestation.h``: Expose an API to bind attestation service to an SPM
      implementation.
- **PSA interface**:
    - ``psa/initial_attestation.h``: Public API definition of initial
      attestation service.
- **Crypto interface**:
    - ``t_cose_crypto.h``: Expose an API to bind the ``t_cose`` implementation
      to any cryptographic library.
    - ``tfm_plat_crypto_keys.h``: Expose an API to get the attestation key from
      platform layer.

PSA interface
=============
The TF-M Initial Attestation Service exposes the following PSA
interface:

.. code-block:: c

    psa_status_t
    psa_initial_attest_get_token(const uint8_t *auth_challenge,
                                 size_t         challenge_size,
                                 uint8_t       *token_buf,
                                 size_t         token_buf_size,
                                 size_t        *token_size);

    psa_status_t
    psa_initial_attest_get_token_size(size_t challenge_size,
                                      size_t *token_size);

    psa_status_t
    tfm_initial_attest_get_public_key(uint8_t         *public_key,
                                      size_t           public_key_buf_size,
                                      size_t          *public_key_len,
                                      psa_ecc_curve_t *elliptic_curve_type);

The caller must allocate a large enough buffer, where the token is going to be
created by Initial Attestation Service. The size of the created token is highly
dependent on the number of software components in the system and the provided
attributes of these. The ``psa_initial_attest_get_token_size()`` function can be
called to get the exact size of the created token.

System integrators might need to port these interfaces to a custom secure
partition manager implementation (SPM). Implementations in TF-M project can be
found here:

-  ``interface/src/tfm_initial_attestation_func_api.c``: non-secure interface
   implementation for library model
-  ``interface/src/tfm_initial_attestation_ipc_api.c``: non-secure interface
   implementation for IPC model
-  ``secure_fw/services/initial_attestation/tfm_attestation_secure_api.c``:
   secure interface implementation

Secure Partition Manager (SPM) interface
========================================
The Initial Attestation Service defines the following interface towards the
secure partition manager (SPM). System integrators **must** port this interface
according to their SPM implementation.

.. code:: c

    enum psa_attest_err_t
    attest_get_boot_data(uint8_t major_type, void *ptr, uint32_t len);

    enum psa_attest_err_t
    attest_get_caller_client_id(int32_t *caller_id);

- ``attest_get_boot_data()``: Service can retrieve the relevant data from shared
  memory area between boot loader and runtime software. It might be the case
  that only SPM has direct access to the shared memory area, therefore this
  function can be used to copy the service related data from shared memory to
  a local memory buffer. In TF-M implementation this function must be called
  during service initialization phase, because the shared memory region is
  deliberately overlapping with secure main stack to spare some memory and reuse
  this area during execution. If boot loader is not available in the system to
  provide attributes of software components then this function must be
  implemented in a way that just initialize service's memory buffer to:

  .. code-block:: c

      struct shared_data_tlv_header *tlv_header = (struct shared_data_tlv_header *)ptr;
      tlv_header->tlv_magic   = 2016;
      tlv_header->tlv_tot_len = sizeof(struct shared_data_tlv_header *tlv_header);

- ``attest_get_caller_client_id()``: Retrieves the ID of the caller thread.
- ``tfm_client.h``: Service relies on the following external definitions, which
  must be present or included in this header file:

  .. code-block:: c

      typedef struct psa_invec {
          const void *base;
          size_t len;
      } psa_invec;

      typedef struct psa_outvec {
          void *base;
          size_t len;
      } psa_outvec;

Hardware abstraction layer
==========================
The following API definitions are intended to retrieve the platform specific
claims. System integrators **must** implement these interface according to their
SoC and software design. Detailed definition of the claims are above
in the claims in the initial attestation token paragraph.

- ``tfm_attest_hal_get_security_lifecycle()``: Get the security lifecycle of the
  device.
- ``tfm_attest_hal_get_verification_service()``: Get the verification
  service indicator for initial attestation.
- ``tfm_attest_hal_get_profile_definition()``: Get the name of the profile
  definition document for initial attestation.
- ``tfm_plat_get_boot_seed()``: Get the boot seed, which is a constant random
  number during a boot cycle.
- ``tfm_plat_get_implementation_id``: Get the implementation ID of the
  device.
- ``tfm_plat_get_hw_version``: Get the hardware version of the device.

Boot loader interface
=====================
It is **recommended** to have a secure boot loader in the boot chain, which is
capable of measuring the runtime firmware components (calculates the hash value
of firmware images) and provide other attributes of these (version, type, etc).
If the used boot loader is not capable of sharing these information with the
runtime software then the ``BOOT_DATA_AVAILABLE`` compiler flag **must** be
set to false (see `Related compile time options`_).

The shared data between boot loader and runtime software is TLV encoded. The
definition of TLV structure is described in ``bl2/include/tfm_boot_status.h``.
The shared data is stored in a well known location in secure internal memory
and this is a contract between boot loader and runtime SW.

The structure of shared data must be the following:

-  At the beginning there must be a header: ``struct shared_data_tlv_header``
   This contains a magic number and a size field which covers the entire size
   of the shared data area including this header.

   .. code-block:: c

       struct shared_data_tlv_header {
           uint16_t tlv_magic;
           uint16_t tlv_tot_len;
       };

-  After the header there come the entries which are composed from an
   entry header structure: ``struct shared_data_tlv_entry`` and the data. In
   the entry header is a type field ``tlv_type`` which identify the consumer of
   the entry in the runtime software and specify the subtype of that data item.
   There is a size field ``tlv_len`` which covers the size of the entry header
   and the data. After this structure comes the actual data.

   .. code-block:: c

       struct shared_data_tlv_entry {
           uint16_t tlv_type;
           uint16_t tlv_len;
       };

-  Arbitrary number and size of data entry can be in the shared memory
   area.

The figure below gives of overview about the ``tlv_type`` field in the entry
header. The ``tlv_type`` always composed from a major and minorbnumber. Major
number identifies the addressee in runtime software, which the databentry is
sent to. Minor number used to encode more info about the data entry. The actual
definition of minor number could change per major number. In case of boot
status data, which is going to be processed by initial attestation service
the minor number is split further to two part: ``sw_module`` and ``claim``. The
``sw_module`` identifies the SW component in the system which the data item
belongs to and the ``claim`` part identifies the exact type of the data.

``tlv_type`` description::

    |------------------------------------------------ |
    |                  tlv_type (16 bits)             |
    |-------------------------------------------------|
    |   tlv_major(4 bits)   |   tlv_minor(12 bits)    |
    |-------------------------------------------------|
    | MAJOR_IAS   | sw_module(6 bits) | claim(6 bits) |
    |-------------------------------------------------|
    | MAJOR_CORE  |          TBD                      |
    |-------------------------------------------------|

Overall structure of shared data::

    ---------------------------------------------------------------
    | Magic number(uint16_t) | Shared data total length(uint16_t) |
    ---------------------------------------------------------------
    | Major_type(4 bits) | Minor_type(12 bits) | Length(uint16_t) |
    ---------------------------------------------------------------
    |                         Raw data                            |
    ---------------------------------------------------------------
    |                              .                              |
    |                              .                              |
    |                              .                              |
    ---------------------------------------------------------------
    | Major_type(4 bits) | Minor_type(12 bits) | Length(uint16_t) |
    ---------------------------------------------------------------
    |                         Raw data                            |
    ---------------------------------------------------------------

Crypto interface
================
Device **must** contain an asymmetric key pair. The private part of it is used
to sign the initial attestation token. Current implementation supports only the
ECDSA P256 signature over SHA256. The public part of the key pair is used to
create the key identifier (kid) in the unprotected part of the COSE header. The
kid is used by verification entity to look up the corresponding public key to
verify the signature in the token. The `t_cose` part of the initial attestation
service implements the signature generation and kid creation. But the actual
calculation of token's hash and signature is done by the Crypto service in the
device. System integrators might need to re-implement the following functions
if they want to use initial attestation service with a different cryptographic
library than Crypto service:

- ``t_cose_crypto_pub_key_sign()``: Calculates the signature over a hash value.
- ``t_cose_crypto_get_ec_pub_key()``: Get the public key to create the key
  identifier.
- ``t_cose_crypto_hash_start()``: Start a multipart hash operation.
- ``t_cose_crypto_hash_update()``: Add a message fragment to a multipart hash
  operation.
- ``t_cose_crypto_hash_finish()``:Finish the calculation of the hash of a
  message.

Interface needed by verification code:

-  ``t_cose_crypto_pub_key_verify()``: Verify the signature over a hash value.

Key handling
------------
The provisioning of the initial attestation key is out of scope of the service
and this document. It is assumed that device maker provisions the unique
asymmetric key pair during the manufacturing process. The following API is
defined to retrieve the attestation key pair from platform layer. Software
integrators **must** port this interface according to their SoC design and make
sure that key pair is available by Crypto service:

- ``tfm_plat_get_initial_attest_key()``: Retrieve the initial attestation key
  pair from platform layer.

In TF-M project the attestation key is retrieved by initial attestation service.
The key is registered and unregistered to the Crypto service by attestation
service with ``psa_import_key()`` and ``psa_destroy_key()`` API calls for
further usage. See in ``attestation_key.c``. In other implementation if the
attestation key is directly retrieved by the Crypto service then this key
handling is not necessary.

Initial Attestation Service compile time options
================================================
There is a defined set of flags that can be used to compile in/out certain
service features. The ``CommonConfig.cmake`` file sets the default values of
those flags. The list of flags are:

- ``ATTEST_INCLUDE_OPTIONAL_CLAIMS``: Include also the optional claims to the
  attestation token. Default value: True.
- ``ATTEST_INCLUDE_TEST_CODE``: Test code is removed from COSE library and from
  attestation test suite if it is False. Its default value depends on the build
  type. It is True if build type is ``Debug``, otherwise False (different kinds
  of ``Release`` builds).
- ``ATTEST_INCLUDE_COSE_KEY_ID``: COSE key-id is an optional field in the COSE
  unprotected header. Key-id is calculated and added to the COSE header based
  on the value of this flag. Default value: False.

Related compile time options
----------------------------
- ``BOOT_DATA_AVAILABLE``: The boot data is expected to be present in the shared
  data area between the boot loader and the runtime firmware when it's true.
  Otherwise, when it's false does not check the content of the shared data area.
  Also assume that the TLV header is present and valid (the magic number is
  correct) and there are no other data entries. Its default value depends on
  the BL2 flag.

************
Verification
************
The initial attestation token is verified by the attestation test suite in
``test/suites/attestation``. The test suite is responsible for verifying the
token signature and parsing the token to verify its encoding and the presence of
the mandatory claims. This test suite can be executed on the device. It is part
of the regression test suite. When the user builds TF-M with any of the
``ConfigRegression*.cmake`` configurations then this test is executed
automatically. The test suite is configurable in the
``test/suites/attestation/attest_token_test_values.h`` header file. In this file
there are two attributes for each claim which are configurable (more details
in the header file):

 - Requirements of presence: optional or mandatory
 - Expected value: Value check can be disabled or expected value can be provided
   here.

There is another possibility to verify the attestation token. This addresses
the off-device testing when the token is already retrieved from the device and
verification is done on the requester side. There is a Python script for this
purpose in ``tools/iat-verifier``. It does the same checking as the
attestation test suite. The following steps describe how to simulate an
off-device token verification on a host computer. It is described how to
retrieve an initial attestation token when TF-M code is executed on FVP
and how to use the iat_verifier script to check the token. This example assumes
that user has license for DS-5 and FVP models:

 - Build TF-M with any of the ``ConfigRegression*.cmake`` build configurations
   for MPS2 AN521 platform. More info in
   :doc:`tfm_build_instruction </docs/user_guides/tfm_build_instruction>`.
 - Lunch FVP model in DS-5. More info in
   :doc:`tfm_user_guide </docs/user_guides/tfm_user_guide>`.
 - Set a breakpoint in ``test/suites/attestation/attest_token_test.c``
   in ``decode_test_internal(..)`` after the ``token_main_alt(..)`` returned,
   i.e. on line 859. Execute the code in the model until the breakpoint hits
   second time. At this point the console prints the following message:
   ``ECDSA signature test of attest token``.
 - At this point the token resides in the model memory and can be dumped to host
   computer.
 - The ADDRESS and SIZE attributes of the initial attestation token is stored in
   the ``completed_token`` local variable. Their value can be extracted in the
   ``(x)=Variables`` debug window.
 - Apply this command in the ``Commands`` debug window to dump the token in
   binary format to the host computer:
   ``dump memory <PATH>/iat_01.cbor <ADDRESS> +<SIZE>``
 - Execute this command on the host computer to verify the token:
   ``check_iat -p -K -k platform/ext/common/template/tfm_initial_attestation_key.pem <PATH>/iat_01.cbor``
 - Documentation of the iat-verifier can be found
   :doc:`here </tools/iat-verifier/README>`.

--------------

*Copyright (c) 2018-2020, Arm Limited. All rights reserved.*
