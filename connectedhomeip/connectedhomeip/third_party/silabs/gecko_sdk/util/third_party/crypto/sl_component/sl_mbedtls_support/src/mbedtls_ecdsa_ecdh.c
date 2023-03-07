/***************************************************************************//**
 * @file
 * @brief mbed TLS elliptic curve operations accelerated by PSA crypto drivers
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

/**
 * This file includes an alternative implementation of high-level ECDSA and ECDH
 * functions from the mbed TLS API, using the relevant accelerators incorporated
 * in devices from Silicon Labs.
 *
 * For Series-1 devices with a CRYPTO peripheral, see crypto_ecp.c.
 *
 * This alternative implementation calls the PSA Crypto drivers provided
 * by Silicon Labs. For details on these drivers, see \ref sl_psa_drivers.
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ECP_C)

#if defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT)      \
  || defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT) \
  || defined(MBEDTLS_ECDSA_GENKEY_ALT)        \
  || defined(MBEDTLS_ECDSA_VERIFY_ALT)        \
  || defined(MBEDTLS_ECDSA_SIGN_ALT)

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT)
#include "sli_se_transparent_functions.h"
#define ECC_IMPLEMENTATION_PRESENT
#define ECC_KEYGEN_FCT      sli_se_transparent_generate_key
#define ECC_PUBKEY_FCT      sli_se_transparent_export_public_key
#define ECDSA_SIGN_FCT      sli_se_transparent_sign_hash
#define ECDSA_VERIFY_FCT    sli_se_transparent_verify_hash
#define ECDH_DERIVE_FCT     sli_se_transparent_key_agreement
#elif defined(CRYPTOACC_PRESENT)
#include "sli_cryptoacc_transparent_functions.h"
#define ECC_IMPLEMENTATION_PRESENT
#define ECC_KEYGEN_FCT      sli_cryptoacc_transparent_generate_key
#define ECC_PUBKEY_FCT      sli_cryptoacc_transparent_export_public_key
#define ECDSA_SIGN_FCT      sli_cryptoacc_transparent_sign_hash
#define ECDSA_VERIFY_FCT    sli_cryptoacc_transparent_verify_hash
#define ECDH_DERIVE_FCT     sli_cryptoacc_transparent_key_agreement
#endif

#include "mbedtls/ecdh.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/bignum.h"
#include "mbedtls/error.h"
#include "psa/crypto.h"

// Parameter validation macros based on platform_util.h
#define ECDH_VALIDATE_RET(cond) \
  MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_ECP_BAD_INPUT_DATA)
#define ECDH_VALIDATE(cond) \
  MBEDTLS_INTERNAL_VALIDATE(cond)

#define ECDSA_VALIDATE_RET(cond) \
  MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_ECP_BAD_INPUT_DATA)
#define ECDSA_VALIDATE(cond) \
  MBEDTLS_INTERNAL_VALIDATE(cond)

#if defined(ECC_IMPLEMENTATION_PRESENT)
static int psa_status_to_mbedtls(psa_status_t status)
{
  switch ( status ) {
    case PSA_SUCCESS:
      return 0;
    case PSA_ERROR_INVALID_SIGNATURE:
      return MBEDTLS_ERR_ECP_VERIFY_FAILED;
    case PSA_ERROR_HARDWARE_FAILURE:
      return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    case PSA_ERROR_NOT_SUPPORTED:
      return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
    default:
      return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
  }
}

static int mbedtls_grp_to_psa_attr(mbedtls_ecp_group_id id,
                                   psa_key_attributes_t *attr)
{
  switch (id) {
    case MBEDTLS_ECP_DP_SECP192R1:
      attr->MBEDTLS_PRIVATE(core).MBEDTLS_PRIVATE(type) = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1);
      psa_set_key_bits(attr, 192);
      break;
#if defined(CRYPTOACC_PRESENT)
    case MBEDTLS_ECP_DP_SECP224R1:
      attr->MBEDTLS_PRIVATE(core).MBEDTLS_PRIVATE(type) = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1);
      psa_set_key_bits(attr, 224);
      break;
    case MBEDTLS_ECP_DP_SECP256K1:
      attr->MBEDTLS_PRIVATE(core).MBEDTLS_PRIVATE(type) = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_K1);
      psa_set_key_bits(attr, 256);
      break;
#endif
    case MBEDTLS_ECP_DP_SECP256R1:
      attr->MBEDTLS_PRIVATE(core).MBEDTLS_PRIVATE(type) = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1);
      psa_set_key_bits(attr, 256);
      break;
    case MBEDTLS_ECP_DP_SECP384R1:
      attr->MBEDTLS_PRIVATE(core).MBEDTLS_PRIVATE(type) = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1);
      psa_set_key_bits(attr, 384);
      break;
    case MBEDTLS_ECP_DP_SECP521R1:
      attr->MBEDTLS_PRIVATE(core).MBEDTLS_PRIVATE(type) = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1);
      psa_set_key_bits(attr, 521);
      break;
    case MBEDTLS_ECP_DP_CURVE25519:
      attr->MBEDTLS_PRIVATE(core).MBEDTLS_PRIVATE(type) = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_MONTGOMERY);
      psa_set_key_bits(attr, 255);
      break;
    default:
      return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
  }
  return PSA_SUCCESS;
}
#if defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT) \
  || defined(MBEDTLS_ECDSA_GENKEY_ALT)
static int ecc_keygen(mbedtls_ecp_group *grp, mbedtls_mpi *d, mbedtls_ecp_point *Q)
{
  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
  uint8_t keybuf[((((MBEDTLS_ECP_MAX_BYTES) +3) / 4) * 4) * 2 + 1u] = { 0 };

  psa_status_t status = psa_status_to_mbedtls(
    mbedtls_grp_to_psa_attr(grp->id, &attr) );
  if ( status != PSA_SUCCESS ) {
    return status;
  }

  size_t keybytes;
  status = psa_status_to_mbedtls(
    ECC_KEYGEN_FCT(&attr,
                   keybuf,
                   sizeof(keybuf),
                   &keybytes) );

  if ( status != PSA_SUCCESS ) {
    return status;
  }

  if (PSA_KEY_TYPE_ECC_GET_FAMILY(psa_get_key_type(&attr)) == PSA_ECC_FAMILY_MONTGOMERY) {
    mbedtls_mpi_read_binary_le(d, keybuf, keybytes);
  } else {
    mbedtls_mpi_read_binary(d, keybuf, keybytes);
  }

  status = psa_status_to_mbedtls(
    ECC_PUBKEY_FCT(&attr,
                   keybuf,
                   keybytes,
                   keybuf,
                   sizeof(keybuf),
                   &keybytes) );

  if ( status != PSA_SUCCESS ) {
    return status;
  }

  if ( PSA_KEY_TYPE_ECC_GET_FAMILY(psa_get_key_type(&attr)) == PSA_ECC_FAMILY_MONTGOMERY ) {
    mbedtls_mpi_read_binary_le(&Q->MBEDTLS_PRIVATE(X), keybuf, keybytes);
  } else {
    // The first byte is used to store uncompressed representation byte.
    mbedtls_mpi_read_binary(&Q->MBEDTLS_PRIVATE(X), keybuf + 1u, keybytes / 2);
    mbedtls_mpi_read_binary(&Q->MBEDTLS_PRIVATE(Y), keybuf + keybytes / 2 + 1u, keybytes / 2);
    mbedtls_mpi_lset(&Q->MBEDTLS_PRIVATE(Z), 1);
  }

  return status;
}
#endif /* #if defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT)
       || defined(MBEDTLS_ECDSA_GENKEY_ALT) */

#if defined(MBEDTLS_ECDSA_GENKEY_ALT)
/*
 * Generate key pair
 */
int mbedtls_ecdsa_genkey(mbedtls_ecdsa_context *ctx, mbedtls_ecp_group_id gid,
                         int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
  /* PSA uses internal entropy */
  (void)f_rng;
  (void)p_rng;

  ECDSA_VALIDATE_RET(ctx   != NULL);
  ECDSA_VALIDATE_RET(f_rng != NULL);

  mbedtls_ecp_group_load(&ctx->MBEDTLS_PRIVATE(grp), gid);

  return ecc_keygen(&ctx->MBEDTLS_PRIVATE(grp), &ctx->MBEDTLS_PRIVATE(d), &ctx->MBEDTLS_PRIVATE(Q));
}
#endif /* MBEDTLS_ECDSA_GENKEY_ALT */

#if defined(MBEDTLS_ECDSA_SIGN_ALT)
int mbedtls_ecdsa_sign(mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                       const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                       int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
  /* PSA uses internal entropy */
  (void)f_rng;
  (void)p_rng;

  ECDSA_VALIDATE_RET(grp   != NULL);
  ECDSA_VALIDATE_RET(r     != NULL);
  ECDSA_VALIDATE_RET(s     != NULL);
  ECDSA_VALIDATE_RET(d     != NULL);
  ECDSA_VALIDATE_RET(f_rng != NULL);
  ECDSA_VALIDATE_RET(buf   != NULL || blen == 0);

  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
  uint8_t key_signature_buf[((((MBEDTLS_ECP_MAX_BYTES) +3) / 4) * 4) * 2] = { 0 };

  psa_status_t status = psa_status_to_mbedtls(
    mbedtls_grp_to_psa_attr(grp->id, &attr));
  if ( status != PSA_SUCCESS ) {
    return status;
  }
  psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_SIGN_HASH);

  if (PSA_KEY_TYPE_ECC_GET_FAMILY(psa_get_key_type(&attr)) == PSA_ECC_FAMILY_MONTGOMERY) {
    return PSA_ERROR_NOT_SUPPORTED;
  }

  size_t keybytes = PSA_BITS_TO_BYTES(psa_get_key_bits(&attr));

  // Make sure d is in range 1..n-1
  if ((mbedtls_mpi_cmp_int(d, 1) < 0) || (mbedtls_mpi_cmp_mpi(d, &grp->N) >= 0)) {
    return MBEDTLS_ERR_ECP_INVALID_KEY;
  }

  mbedtls_mpi_write_binary(d, key_signature_buf, keybytes);

  status = psa_status_to_mbedtls(
    ECDSA_SIGN_FCT(&attr,
                   key_signature_buf,
                   keybytes,
                   PSA_ALG_ECDSA_ANY,
                   buf,
                   blen,
                   key_signature_buf,
                   sizeof(key_signature_buf),
                   &keybytes) );

  if ( status != PSA_SUCCESS ) {
    return status;
  }

  mbedtls_mpi_read_binary(r, key_signature_buf, keybytes / 2);
  mbedtls_mpi_read_binary(s, key_signature_buf + (keybytes / 2), keybytes / 2);

  return status;
}
#endif /* MBEDTLS_ECDSA_SIGN_ALT */

#if defined(MBEDTLS_ECDSA_VERIFY_ALT)
int mbedtls_ecdsa_verify(mbedtls_ecp_group *grp,
                         const unsigned char *buf, size_t blen,
                         const mbedtls_ecp_point *Q, const mbedtls_mpi *r, const mbedtls_mpi *s)
{
  ECDSA_VALIDATE_RET(grp != NULL);
  ECDSA_VALIDATE_RET(Q   != NULL);
  ECDSA_VALIDATE_RET(r   != NULL);
  ECDSA_VALIDATE_RET(s   != NULL);
  ECDSA_VALIDATE_RET(buf != NULL || blen == 0);

  uint8_t pub[((((MBEDTLS_ECP_MAX_BYTES) +3) / 4) * 4) * 2 + 1] = { 0 };
  uint8_t signature[((((MBEDTLS_ECP_MAX_BYTES) +3) / 4) * 4) * 2] = { 0 };
  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;

  psa_status_t status = psa_status_to_mbedtls(
    mbedtls_grp_to_psa_attr(grp->id, &attr) );
  if ( status != PSA_SUCCESS ) {
    return status;
  }
  psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_VERIFY_HASH);

  if (PSA_KEY_TYPE_ECC_GET_FAMILY(psa_get_key_type(&attr)) == PSA_ECC_FAMILY_MONTGOMERY) {
    return PSA_ERROR_NOT_SUPPORTED;
  }

  attr.MBEDTLS_PRIVATE(core).MBEDTLS_PRIVATE(type) =
    PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_KEY_TYPE_ECC_GET_FAMILY(psa_get_key_type(&attr)));

  size_t keybytes = PSA_BITS_TO_BYTES(psa_get_key_bits(&attr));

  /* pull out signature info from mbedtls structures */
  mbedtls_mpi_write_binary(r, signature, keybytes);
  mbedtls_mpi_write_binary(s, &signature[keybytes], keybytes);

  pub[0] = 0x04; // Uncompressed public key
  mbedtls_mpi_write_binary(&Q->MBEDTLS_PRIVATE(X), &pub[1u], keybytes);
  mbedtls_mpi_write_binary(&Q->MBEDTLS_PRIVATE(Y), &pub[keybytes + 1u], keybytes);

  return psa_status_to_mbedtls(
    ECDSA_VERIFY_FCT(&attr,
                     pub,
                     keybytes * 2 + 1u,
                     PSA_ALG_ECDSA_ANY,
                     buf,
                     blen,
                     signature,
                     keybytes * 2) );
}
#endif /* MBEDTLS_ECDSA_VERIFY_ALT */

#if defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT)
int mbedtls_ecdh_gen_public(mbedtls_ecp_group *grp, mbedtls_mpi *d, mbedtls_ecp_point *Q,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng)
{
  /* PSA uses internal entropy */
  (void)f_rng;
  (void)p_rng;

  ECDH_VALIDATE_RET(grp != NULL);
  ECDH_VALIDATE_RET(d != NULL);
  ECDH_VALIDATE_RET(Q != NULL);
  ECDH_VALIDATE_RET(f_rng != NULL);

  return ecc_keygen(grp, d, Q);
}
#endif /* #if defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT) */

#if defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT)
int mbedtls_ecdh_compute_shared(mbedtls_ecp_group *grp, mbedtls_mpi *z,
                                const mbedtls_ecp_point *Q, const mbedtls_mpi *d,
                                int (*f_rng)(void *, unsigned char *, size_t),
                                void *p_rng)
{
  /* PSA uses internal entropy */
  (void)f_rng;
  (void)p_rng;

  ECDH_VALIDATE_RET(grp != NULL);
  ECDH_VALIDATE_RET(Q != NULL);
  ECDH_VALIDATE_RET(d != NULL);
  ECDH_VALIDATE_RET(z != NULL);

  uint8_t pub[((((MBEDTLS_ECP_MAX_BYTES) +3) / 4) * 4) * 2 + 1u] = { 0 };
  uint8_t priv[((((MBEDTLS_ECP_MAX_BYTES) +3) / 4) * 4) * 2] = { 0 };
  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;

  psa_status_t status = psa_status_to_mbedtls(
    mbedtls_grp_to_psa_attr(grp->id, &attr) );
  if ( status != PSA_SUCCESS ) {
    return status;
  }
  psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_DERIVE);

  size_t keylen = PSA_BITS_TO_BYTES(psa_get_key_bits(&attr));
  size_t publen;

  /* pull out key info from mbedtls structures */
  if (PSA_KEY_TYPE_ECC_GET_FAMILY(psa_get_key_type(&attr)) == PSA_ECC_FAMILY_MONTGOMERY) {
    publen = keylen;
    mbedtls_mpi_write_binary_le(d, priv, keylen);
    mbedtls_mpi_write_binary_le(&Q->MBEDTLS_PRIVATE(X), pub, keylen);
  } else {
    publen = 2 * keylen + 1u;
    mbedtls_mpi_write_binary(d, priv, keylen);
    pub[0] = 0x04; // uncompressed public key
    mbedtls_mpi_write_binary(&Q->MBEDTLS_PRIVATE(X), pub + 1u, keylen);
    mbedtls_mpi_write_binary(&Q->MBEDTLS_PRIVATE(Y), pub + keylen + 1u, keylen);
  }

  status = psa_status_to_mbedtls(
    ECDH_DERIVE_FCT(PSA_ALG_ECDH,
                    &attr,
                    priv,
                    keylen,
                    pub,
                    publen,
                    pub,
                    sizeof(pub),
                    &publen) );

  if ( status != PSA_SUCCESS ) {
    return status;
  }

  if (PSA_KEY_TYPE_ECC_GET_FAMILY(psa_get_key_type(&attr)) == PSA_ECC_FAMILY_MONTGOMERY) {
    mbedtls_mpi_read_binary_le(z, pub, publen);
  } else {
    mbedtls_mpi_read_binary(z, pub, publen);
  }
  return status;
}
#endif /* #if defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT) */

#endif /* ECC_IMPLEMENTATION_PRESENT */

#endif /* #if defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT) || defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT) */

#endif /* #if defined(MBEDTLS_ECP_C) */
