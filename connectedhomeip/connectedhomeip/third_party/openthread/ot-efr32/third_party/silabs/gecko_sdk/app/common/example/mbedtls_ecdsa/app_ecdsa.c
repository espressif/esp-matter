/***************************************************************************//**
 * @file
 * @brief mbedTLS ECDSA examples functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "app_ecdsa.h"

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"

#include "sl_sleeptimer.h"

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/private_access.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/
#define mbedtls_printf     printf

/*******************************************************************************
 ***************************   Local functions  ********************************
 ******************************************************************************/
static void dump_buf(const char *title, unsigned char *buf, size_t len)
{
  size_t i;

  mbedtls_printf("%s", title);
  for ( i = 0; i < len; i++ ) {
    mbedtls_printf("%c%c", "0123456789ABCDEF"[buf[i] / 16],
                   "0123456789ABCDEF"[buf[i] % 16]);
  }
  mbedtls_printf("\n");
}

static void dump_pubkey(const char *title, mbedtls_ecdsa_context *key)
{
  unsigned char buf[300];
  size_t len;

  if ( mbedtls_ecp_point_write_binary(&key->MBEDTLS_PRIVATE(grp), &key->MBEDTLS_PRIVATE(Q),
                                      MBEDTLS_ECP_PF_UNCOMPRESSED, &len, buf, sizeof buf) != 0 ) {
    mbedtls_printf("internal error\n");
    return;
  }

  dump_buf(title, buf, len);
}

/*******************************************************************************
 ***************************   Global functions  *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize example.
 ******************************************************************************/
void app_ecdsa_init(void)
{
  return;
}
/***************************************************************************//**
 * Take input from user to set curve.
 ******************************************************************************/
mbedtls_ecp_group_id input_curve(void)
{
  int response = 1;
  int loop_control = 0;

  mbedtls_printf("\nWelcome to the ECDSA example application. \n");
  mbedtls_printf("You can choose between the follwing curves: \n");
  mbedtls_printf("1. MBEDTLS_ECP_DP_SECP256R1 \n");
  mbedtls_printf("2. MBEDTLS_ECP_DP_SECP192R1 \n");

  while (0 == loop_control) {
    if (response > 0) {
      mbedtls_printf("Please type in the preferred option, '1' or '2': ");
    }
    response = getchar();
    if ('1' == response) {
      mbedtls_printf("\n Thanks. You chose 1. MBEDTLS_ECP_DP_SECP256R1. \n");
      return MBEDTLS_ECP_DP_SECP256R1;
    }
    if ('2' == response) {
      mbedtls_printf("\n Thanks. You chose 2. MBEDTLS_ECP_DP_SECP192R1. \n");
      return MBEDTLS_ECP_DP_SECP192R1;
    }
  }

  mbedtls_printf("\n Using as default: MBEDTLS_ECP_DP_SECP256R1. \n");
  return MBEDTLS_ECP_DP_SECP256R1;
}

/***************************************************************************//**
 * Example function for ECDSA digital signature.
 ******************************************************************************/
void app_ecdsa_process_action(void)
{
  int ret;
  mbedtls_ecdsa_context ctx_sign, ctx_verify;
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_ecp_group_id mbedtls_ecc_id;
  mbedtls_mpi r;
  mbedtls_mpi s;

  unsigned char hash[] = "This should be the hash of a message.";
  unsigned char sig[MBEDTLS_ECDSA_MAX_LEN];
  size_t sig_len;
  const char *pers = "ecdsa";

  uint32_t time_tick;
  uint32_t time_diff_tick;
  uint32_t time_diff_ms;

  mbedtls_mpi_init(&r);
  mbedtls_mpi_init(&s);
  mbedtls_ecdsa_init(&ctx_sign);
  mbedtls_ecdsa_init(&ctx_verify);
  mbedtls_ctr_drbg_init(&ctr_drbg);

  memset(sig, 0, sizeof(sig));
  ret = 1;

  // Take curve as input from the user
  mbedtls_ecc_id = input_curve();

  // Generate a key pair for signing
  mbedtls_printf("\n  . Seeding the random number generator...");

  mbedtls_entropy_init(&entropy);
  if ( (ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                    (const unsigned char *) pers,
                                    strlen(pers) ) ) != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
    goto exit;
  }

  mbedtls_printf(" ok\n  . Generating key pair...");

  time_tick = sl_sleeptimer_get_tick_count();
  if ( (ret = mbedtls_ecdsa_genkey(&ctx_sign, mbedtls_ecc_id,
                                   mbedtls_ctr_drbg_random, &ctr_drbg) ) != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ecdsa_genkey returned %d\n", ret);
    goto exit;
  }

  time_diff_tick = sl_sleeptimer_get_tick_count() - time_tick;
  time_diff_ms = sl_sleeptimer_tick_to_ms(time_diff_tick);

  mbedtls_printf(" ok (key size: %d bits, clock cycles: %ld, time: %" PRIu32 " ms)\n",
                 (int) ctx_sign.MBEDTLS_PRIVATE(grp).pbits, time_diff_tick, time_diff_ms);

  dump_pubkey("  + Public key: ", &ctx_sign);

  // Sign some message hash
  sig_len = ctx_sign.MBEDTLS_PRIVATE(grp).pbits / 8u * 2u;
  mbedtls_printf("  . Signing message...");

  time_tick = sl_sleeptimer_get_tick_count();
  if ( (ret = mbedtls_ecdsa_sign(&(ctx_sign.MBEDTLS_PRIVATE(grp)), &r, &s, &(ctx_sign.MBEDTLS_PRIVATE(d)),
                                 hash, sizeof(hash),
                                 mbedtls_ctr_drbg_random, &ctr_drbg) ) != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ecdsa_genkey returned %d\n", ret);
    goto exit;
  }
  time_diff_tick = sl_sleeptimer_get_tick_count() - time_tick;
  time_diff_ms = sl_sleeptimer_tick_to_ms(time_diff_tick);

  mbedtls_mpi_write_binary(&r, ((uint8_t*)sig), sig_len / 2u);
  mbedtls_mpi_write_binary(&s, ((uint8_t*)&sig[sig_len / 2u]), sig_len / 2u);

  mbedtls_printf(" ok (signature length = %u bytes, clock cycles: %ld, time: %" PRIu32 " ms)\n",
                 (unsigned int) sig_len, time_diff_tick, time_diff_ms);

  dump_buf("  + Hash: ", hash, sizeof hash);
  dump_buf("  + Signature: ", sig, sig_len);

  // Transfer public information to verifying context

  // We could use the same context for verification and signatures, but we
  // chose to use a new one in order to make it clear that the verifying
  // context only needs the public key (Q), and not the private key (d).
  mbedtls_printf("  . Preparing verification context...");

  if ( (ret = mbedtls_ecp_group_copy(&ctx_verify.MBEDTLS_PRIVATE(grp), &ctx_sign.MBEDTLS_PRIVATE(grp)) ) != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ecp_group_copy returned %d\n", ret);
    goto exit;
  }

  if ( (ret = mbedtls_ecp_copy(&ctx_verify.MBEDTLS_PRIVATE(Q), &ctx_sign.MBEDTLS_PRIVATE(Q)) ) != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ecp_copy returned %d\n", ret);
    goto exit;
  }

  ret = 0;

  // Verify signature
  mbedtls_printf(" ok\n  . Verifying signature...");

  time_tick = sl_sleeptimer_get_tick_count();

  if ( (ret = mbedtls_ecdsa_verify(&(ctx_verify.MBEDTLS_PRIVATE(grp)),
                                   hash, sizeof(hash),
                                   &(ctx_verify.MBEDTLS_PRIVATE(Q)), &r, &s) ) != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ecdsa_read_signature returned %d\n", ret);
    goto exit;
  }
  time_diff_tick = sl_sleeptimer_get_tick_count() - time_tick;
  time_diff_ms = sl_sleeptimer_tick_to_ms(time_diff_tick);

  mbedtls_printf(" ok  (clock cycles: %ld, time: %" PRIu32 " ms)\n", time_diff_tick, time_diff_ms);
  mbedtls_printf("\n");
  exit:

  mbedtls_mpi_free(&r);
  mbedtls_mpi_free(&s);
  mbedtls_ecdsa_free(&ctx_verify);
  mbedtls_ecdsa_free(&ctx_sign);
  mbedtls_ctr_drbg_free(&ctr_drbg);
  mbedtls_entropy_free(&entropy);
}
