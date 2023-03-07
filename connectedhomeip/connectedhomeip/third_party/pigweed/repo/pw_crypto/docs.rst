.. _module-pw_crypto:

pw_crypto
=========
A set of safe (read: easy to use, hard to misuse) crypto APIs.

The following crypto services are provided by this module.

1. Hashing a message with `SHA256`_.
2. Verifying a digital signature signed with `ECDSA`_ over the NIST P256 curve.
3. Many more to come ...

SHA256
------

1. Obtaining a oneshot digest.

.. code-block:: cpp

  #include "pw_crypto/sha256.h"

  std::byte digest[32];
  if (!pw::crypto::sha256::Hash(message, digest).ok()) {
    // Handle errors.
  }

  // The content can also come from a pw::stream::Reader.
  if (!pw::crypto::sha256::Hash(reader, digest).ok()) {
    // Handle errors.
  }

2. Hashing a long, potentially non-contiguous message.

.. code-block:: cpp

  #include "pw_crypto/sha256.h"

  std::byte digest[32];

  if (!pw::crypto::sha256::Sha256()
      .Update(chunk1).Update(chunk2).Update(chunk...)
      .Final().ok()) {
    // Handle errors.
  }

ECDSA
-----

1. Verifying a digital signature signed with ECDSA over the NIST P256 curve.

.. code-block:: cpp

  #include "pw_crypto/sha256.h"

  std::byte digest[32];
  if (!pw::crypto::sha256::Hash(message, digest).ok()) {
    // handle errors.
  }

  if (!pw::crypto::ecdsa::VerifyP256Signature(public_key, digest,
                                              signature).ok()) {
    // handle errors.
  }

2. Verifying a digital signature signed with ECDSA over the NIST P256 curve,
   with a long and/or non-contiguous message.

.. code-block:: cpp

  #include "pw_crypto/sha256.h"

  std::byte digest[32];

  if (!pw::crypto::sha256::Sha256()
      .Update(chunk1).Update(chunk2).Update(chunkN)
      .Final(digest).ok()) {
      // Handle errors.
  }

  if (!pw::crypto::ecdsa::VerifyP256Signature(public_key, digest,
                                              signature).ok()) {
      // Handle errors.
  }

Configuration
-------------

The crypto services offered by pw_crypto can be backed by different backend
crypto libraries.

Mbed TLS
^^^^^^^^

To select the Mbed TLS backend, the MbedTLS library needs to be installed and
configured.

.. code-block:: sh

  # Install and configure MbedTLS
  pw package install mbedtls
  gn gen out --args='
      dir_pw_third_party_mbedtls=pw_env_setup_PACKAGE_ROOT + "/mbedtls"
      pw_crypto_SHA256_BACKEND="//pw_crypto:sha256_mbedtls"
      pw_crypto_ECDSA_BACKEND="//pw_crypto:ecdsa_mbedtls"
  '

  ninja -C out

For optimal code size and/or performance, the Mbed TLS library can be configured
per product. Mbed TLS configuration is achieved by turning on and off MBEDTLS_*
options in a config.h file. See //third_party/mbedtls for how this is done.

``pw::crypto::sha256`` does not need any special configuration as it uses the
mbedtls_sha256_* APIs directly. However you can optionally turn on
``MBEDTLS_SHA256_SMALLER`` to further reduce the code size to from 3KiB to
~1.8KiB at a ~30% slowdown cost (Cortex-M4).

.. code-block:: c

   #define MBEDTLS_SHA256_SMALLER

``pw::crypto::ecdsa`` requires the following minimum configurations which yields
a code size of ~12KiB.

.. code-block:: c

   #define MBEDTLS_BIGNUM_C
   #define MBEDTLS_ECP_C
   #define MBEDTLS_ECDSA_C
   // The ASN1 options are needed only because mbedtls considers and verifies
   // them (in check_config.h) as dependencies of MBEDTLS_ECDSA_C.
   #define MBEDTLS_ASN1_WRITE_C
   #define MBEDTLS_ASN1_PARSE_C
   #define MBEDTLS_ECP_NO_INTERNAL_RNG
   #define MBEDTLS_ECP_DP_SECP256R1_ENABLED

BoringSSL
^^^^^^^^^

To select the BoringSSL backend, the BoringSSL library needs to be installed and
configured.

.. code-block:: sh

  # Install and configure BoringSSL
  pw package install boringssl
  gn gen out --args='
      dir_pw_third_party_boringssl=pw_env_setup_PACKAGE_ROOT + "/boringssl"
      pw_crypto_SHA256_BACKEND="//pw_crypto:sha256_boringssl"
      pw_crypto_ECDSA_BACKEND="//pw_crypto:ecdsa_boringssl"
  '

  ninja -C out

BoringSSL does not provide a public configuration interface to reduce the code
size.

Micro ECC
^^^^^^^^^

To select Micro ECC, the library needs to be installed and configured.

.. code-block:: sh

  # Install and configure Micro ECC
  pw package install micro-ecc
  gn gen out --args='
      dir_pw_third_party_micro_ecc=pw_env_setup_PACKAGE_ROOT + "//micro-ecc"
      pw_crypto_ECDSA_BACKEND="//pw_crypto:ecdsa_uecc"
  '

Note Micro-ECC does not implement any hashing functions, so you will need to use other backends for SHA256 functionality if needed.

Size Reports
------------

Below are size reports for each crypto service. These vary across
configurations.

.. include:: size_report
