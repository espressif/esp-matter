=====================================
Secure Storage service key management
=====================================

:Author: Jamie Fox
:Organization: Arm Limited
:Contact: Jamie Fox <jamie.fox@arm.com>
:Status: Accepted

Background
==========
The PSA Protected Storage API requires confidentiality for external storage to
be provided by:

    **cryptographic ciphers using device-bound keys**, a tamper resistant
    enclosure, or an inaccessible deployment location, depending on the threat
    model of the deployed system.

A TBSA-M-compliant device must embed a Hardware Unique Key (HUK), which provides
the root of trust (RoT) for confidentiality in the system. It must have at least
128 bits of entropy (and a 128 bit data size), and be accessible only to Trusted
code or Trusted hardware that acts on behalf of Trusted code. [TBSA-M]_

In the current implementation, the Secure Storage (SST) service reads the HUK
directly and imports it into the Crypto partition for further use. This has
multiple drawbacks:

- If there were a flaw in SST that allowed an attacker to obtain its key, then
  the HUK would be exposed, and so the attacker would be able to decrypt not
  just secure storage but also anything else encrypted with the HUK or a key
  derived from the HUK.
- Using the same key for two or more different cryptographic algorithms may
  reduce the security provided by one or more of them.
- It is not possible to re-key if the HUK is used directly, for example in the
  case of a lost key.
- It is incompatible with devices where the HUK is in an enclave and cannot be
  read directly.

Proposal
========
Each time the system boots, SST will request that the Crypto service uses a key
derivation function (KDF) to derive a storage key from the HUK. The storage key
could be kept in on-chip volatile memory private to the Crypto partition, or it
could remain inside a secure element. Either way it will not be returned to SST.

For each call to the PSA Protected Storage APIs, SST will make requests to the
Crypto service to perform AEAD encryption and/or decryption operations using the
storage key (providing a fresh nonce for each encryption).

At no point will SST access the key material itself, only referring to the HUK
and storage key by their handles in the Crypto service.

Key derivation
==============
SST will make key derivation requests to the Crypto service with calls to the
PSA Crypto APIs. In order to derive the storage key, the following calls will be
made::

    /* Open a handle to the HUK */
    psa_open_key(PSA_KEY_LIFETIME_PERSISTENT,
                 TFM_CRYPTO_KEY_ID_HUK,
                 &huk_key_handle)

    /* Set up a key derivation operation with the HUK as the input key */
    psa_key_derivation(&sst_key_generator,
                       huk_key_handle,
                       TFM_CRYPTO_ALG_HUK_DERIVATION,
                       SST_KEY_SALT, SST_KEY_SALT_LEN_BYTES,
                       SST_KEY_LABEL, SST_KEY_LABEL_LEN_BYTES,
                       SST_KEY_LEN_BYTES)

    /* Create the storage key from the key generator */
    psa_generator_import_key(sst_key_handle,
                             SST_KEY_TYPE,
                             PSA_BYTES_TO_BITS(SST_KEY_LEN_BYTES),
                             &sst_key_generator)

.. note:: ``TFM_CRYPTO_KEY_ID_HUK`` is a PSA Crypto key ID that is assumed in
          this design to identify the hardware unique key.

          ``sst_key_handle`` is a PSA Crypto key handle to a volatile key, set
          up in the normal way. After the call to ``psa_generator_import_key``,
          it contains the storage key.

          ``SST_KEY_SALT`` can be ``NULL``, as it is only used in the 'extract'
          step of HKDF, which is redundant when the input key material is a
          cryptographically strong key. [RFC5869]_ It must be constant so that
          the same key can be derived each boot, to decrypt previously-stored
          data.

          ``SST_KEY_LABEL`` can be any string that is independent of the input
          key material and different to the label used in any other derivation
          from the same input key. It prevents two different contexts from
          deriving the same output key from the same input key.

In the call to ``psa_key_derivation()``, ``TFM_CRYPTO_ALG_HUK_DERIVATION`` is
supplied as the key derivation algorithm argument. This indicates that the key
derivation should be done from the HUK, and allows it to be implemented in a
platform-defined way (e.g. using a crypto accelerator). The system integrator
should choose the most optimal algorithm for the platform, or fall back to the
software implementation if none is available.

When implemented in software, the key derivation function used by the crypto
service to derive the storage key will be HKDF, with SHA-256 as the underlying
hash function. HKDF is suitable because:

- It is simple and efficient, requiring only two HMAC operations when the length
  of the output key material is less than or equal to the hash length (as is the
  case here).
- The trade-off is that HKDF is only suitable when the input key material has at
  least as much entropy as required for the output key material. But this is the
  case here, as the HUK has 128 bits of entropy, the same as required by SST.
- HKDF is standardised in RFC 5869 [RFC5869]_ and its security has been formally
  analysed. [HKDF]_
- It is supported by the TF-M Crypto service.

The choice of underlying hash function is fairly straightforward: it needs to be
a modern standardised algorithm, considered to be secure and supported by TF-M
Crypto. This narrows it down to just the SHA-2 family. Of the hash functions in
the family, SHA-256 is the simplest and provides more than enough output length.

Keeping the storage key private to SST
--------------------------------------
The salt and label fields are not generally secret, so an Application RoT
service could request the Crypto service to derive the same storage key from the
HUK, which violates isolation between Application RoT partitions to some extent.
This could be fixed in a number of ways:

- Only PSA RoT partitions can request Crypto to derive keys from the HUK.

  - But then either SST has to be in the PSA RoT or request a service in the PSA
    RoT to do the derivation on its behalf.

- SST has a secret (pseudo)random salt, accessible only to it, that it uses to
  derive the storage key.

  - Where would this salt be stored? It cannot be generated fresh each boot
    because the storage key must stay the same across reboots.

- The Crypto service appends the partition ID to the label, so that no two
  partitions can derive the same key.

  - Still need to make sure only PSA RoT partitions can directly access the HUK
    or Secure Enclave. The label is not secret, so any actor that can access the
    HUK could simply perform the derivation itself, rather than making a request
    to the Crypto service.

The third option would solve the issue with the fewest drawbacks, so this option
is the one that is proposed.

Key use
=======
To encrypt and decrypt data, SST will call the PSA Crypto AEAD APIs in the same
way as the current implementation, but ``sst_key_handle`` will refer to the
storage key, rather than the imported HUK. For each encryption operation, the
following call is made (and analogously for decryption)::

    psa_aead_encrypt(sst_key_handle, SST_CRYPTO_ALG,
                     crypto->ref.iv, SST_IV_LEN_BYTES,
                     add, add_len,
                     in, in_len,
                     out, out_size, out_len)

Future changes
==============
In the future, the client's partition ID and the asset's UID could be used to
derive a key that is unique to that asset, each time the Protected Storage APIs
are called (*key diversification*). To achieve this, the key derivation must use
a ``label`` parameter that is unique to each client ID, UID pair.

References
==========
.. [TBSA-M] Arm Platform Security Architecture Trusted Base System Architecture
   for Armv6-M, Armv7-M and Armv8-M, version 1.0
.. [HKDF] Hugo Krawczyk. 2010. Cryptographic extraction and key derivation: the
   HKDF scheme. In Proceedings of the 30th annual conference on Advances in
   cryptology (CRYPTO'10)
.. [RFC5869] IETF RFC 5869: HMAC-based Extract-and-Expand Key Derivation
   Function (HKDF)

--------------

*Copyright (c) 2019-2020, Arm Limited. All rights reserved.*
