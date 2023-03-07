/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*!

  @mainpage CryptoCell-312 runtime-software API overview

  This documentation describes the runtime APIs provided by Arm CryptoCell-312.
  It provides the programmer with all information necessary for integrating
  and using the runtime APIs in the target environment.\n
  The API layer enables using the CryptoCell cryptographic algorithms, for example,
  AES, hash, RSA and ECC.\n
  Cryptographic algorithms can be divided into two main categories:
  <ul><li>Symmetric algorithms are mostly used for message confidentiality.\n
  The symmetric encryption algorithms are accessible via the generic cipher layer.
  For more information, see mbedtls_cipher_setup().</li>
  <li>Asymmetric algorithms are mostly used for key exchange and message integrity.\n
  The asymmetric encryption algorithms are accessible via the generic public
  key layer.</li></ul>
  The following algorithms are provided:
  <ul><li>Symmetric:<ul>
    <li>AES. \ref cc_aes_hw_limit.</li></ul></li>
    <li>Asymmetric:<ul>
    <li>Diffie-Hellman-Merkle. See ::mbedtls_dhm_read_public(), ::mbedtls_dhm_make_public()
      and ::mbedtls_dhm_calc_secret().</li>
    <li>RSA. See ::mbedtls_rsa_public() and ::mbedtls_rsa_private().</li>
    <li>Elliptic Curves over GF(p). See ::mbedtls_ecp_point_init().</li>
    <li>Elliptic Curve Digital Signature Algorithm (ECDSA). See ::mbedtls_ecdsa_init().</li>
    <li>Elliptic Curve Diffie Hellman (ECDH). See ::mbedtls_ecdh_init().</li></ul></li></ul>
  The documentation is automatically generated from the source code using Doxygen.\n
  For more information on Doxygen, see http://www.stack.nl/~dimitri/doxygen/.\n
  The <b>Modules</b> section introduces the high-level module concepts used
  throughout this documentation.
 */