/***************************************************************************//**
 * @file
 * @brief mbedTLS ECDH examples functions
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

#include "app_ecdh.h"

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/private_access.h"

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"

#include "sl_sleeptimer.h"

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

static void dump_mpi(const char *title, mbedtls_mpi *mpi)
{
  unsigned char buf[32];

  if ( mbedtls_mpi_write_binary(mpi, buf, sizeof buf) != 0 ) {
    mbedtls_printf("internal error\n");
    return;
  }

  dump_buf(title, buf, sizeof buf);
}

/*******************************************************************************
 ***************************   Global functions  ********************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize example.
 ******************************************************************************/
void app_ecdh_init(void)
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

  mbedtls_printf("\nWelcome to the mbedTLS ECDH example application. \n");
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
 * Example function for ECDH key generation.
 ******************************************************************************/
void app_ecdh_process_action(void)
{
  int ret;
  mbedtls_ecdh_context ctx_cli, ctx_srv;
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctr_drbg;
  unsigned char cli_x_to_srv[32], cli_y_to_srv[32],
                srv_x_to_cli[32], srv_y_to_cli[32];
  const char pers[] = "ecdh";

  mbedtls_ecp_group_id mbedtls_ecc_id;

  uint32_t time_tick;
  uint32_t time_diff_tick;
  uint32_t time_diff_ms;

  mbedtls_ecdh_init(&ctx_cli);
  mbedtls_ecdh_init(&ctx_srv);
  mbedtls_ctr_drbg_init(&ctr_drbg);

  // Take curve as input from the user
  mbedtls_ecc_id = input_curve();

  // Initialize random number generation
  mbedtls_printf("  . Seeding the random number generator...");

  mbedtls_entropy_init(&entropy);
  if ( (ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                    (const unsigned char *) pers,
                                    sizeof pers)) != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
    goto exit;
  }

  mbedtls_printf(" ok\n");

  // Client: inialize context and generate keypair
  mbedtls_printf("  . Setting up client context...");

  ret = mbedtls_ecp_group_load(&ctx_cli.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(grp), mbedtls_ecc_id);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ecp_group_load returned %d\n", ret);
    goto exit;
  }

  time_tick = sl_sleeptimer_get_tick_count();

  ret = mbedtls_ecdh_gen_public(&ctx_cli.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(grp), &ctx_cli.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(d), &ctx_cli.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(Q),
                                mbedtls_ctr_drbg_random, &ctr_drbg);

  time_diff_tick = sl_sleeptimer_get_tick_count() - time_tick;
  time_diff_ms = sl_sleeptimer_tick_to_ms(time_diff_tick);

  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ecdh_gen_public returned %d\n", ret);
    goto exit;
  }

  ret = mbedtls_mpi_write_binary(&ctx_cli.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(X), cli_x_to_srv, 32);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_write_binary returned %d\n", ret);
    goto exit;
  }

  ret = mbedtls_mpi_write_binary(&ctx_cli.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(Y), cli_y_to_srv, 32);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_write_binary returned %d\n", ret);
    goto exit;
  }

  mbedtls_printf(" ok  (key size: %d bits, clock cycles: %ld time: %" PRIu32 " ms)\n",
                 (int) ctx_cli.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(grp).pbits, time_diff_tick, time_diff_ms);

  // Server: initialize context and generate keypair
  mbedtls_printf("  . Setting up server context...");

  ret = mbedtls_ecp_group_load(&ctx_srv.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(grp), mbedtls_ecc_id);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ecp_group_load returned %d\n", ret);
    goto exit;
  }

  time_tick = sl_sleeptimer_get_tick_count();
  ret = mbedtls_ecdh_gen_public(&ctx_srv.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(grp), &ctx_srv.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(d), &ctx_srv.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(Q),
                                mbedtls_ctr_drbg_random, &ctr_drbg);

  time_diff_tick = sl_sleeptimer_get_tick_count() - time_tick;
  time_diff_ms = sl_sleeptimer_tick_to_ms(time_diff_tick);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ecdh_gen_public returned %d\n", ret);
    goto exit;
  }

  ret = mbedtls_mpi_write_binary(&ctx_srv.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(X), srv_x_to_cli, 32);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_write_binary returned %d\n", ret);
    goto exit;
  }

  ret = mbedtls_mpi_write_binary(&ctx_srv.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(Y), srv_y_to_cli, 32);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_write_binary returned %d\n", ret);
    goto exit;
  }

  mbedtls_printf(" ok  (key size: %d bits, clock cycles: %ld time: %" PRIu32 " ms)\n",
                 (int) ctx_srv.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(grp).pbits, time_diff_tick, time_diff_ms);

  // Server: read peer's key and generate shared secret
  mbedtls_printf("  . Server reading client key and computing secret...");

  ret = mbedtls_mpi_lset(&ctx_srv.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(Qp).MBEDTLS_PRIVATE(Z), 1);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_lset returned %d\n", ret);
    goto exit;
  }

  ret = mbedtls_mpi_read_binary(&ctx_srv.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(Qp).MBEDTLS_PRIVATE(X), cli_x_to_srv, 32);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_read_binary returned %d\n", ret);
    goto exit;
  }

  ret = mbedtls_mpi_read_binary(&ctx_srv.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(Qp).MBEDTLS_PRIVATE(Y), cli_y_to_srv, 32);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_read_binary returned %d\n", ret);
    goto exit;
  }

  time_tick = sl_sleeptimer_get_tick_count();

  #if defined(_SILICON_LABS_32B_SERIES_2)
  ret = mbedtls_ecdh_compute_shared(&ctx_srv.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(grp), &ctx_srv.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(z),
                                    &ctx_srv.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(Qp), &ctx_srv.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(d),
                                    NULL, NULL);
  #else
  ret = mbedtls_ecdh_compute_shared(&ctx_srv.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(grp), &ctx_srv.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(z),
                                    &ctx_srv.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(Qp), &ctx_srv.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(d),
                                    mbedtls_ctr_drbg_random, &ctr_drbg);
  #endif

  time_diff_tick = sl_sleeptimer_get_tick_count() - time_tick;
  time_diff_ms = sl_sleeptimer_tick_to_ms(time_diff_tick);

  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ecdh_compute_shared returned 0x%x\n", ret);
    goto exit;
  }

  mbedtls_printf(" ok  clock cycles: %ld time: %" PRIu32 " ms)\n",
                 time_diff_tick, time_diff_ms);

  // Client: read peer's key and generate shared secret
  mbedtls_printf("  . Client reading server key and computing secret...");

  ret = mbedtls_mpi_lset(&ctx_cli.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(Qp).MBEDTLS_PRIVATE(Z), 1);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_lset returned %d\n", ret);
    goto exit;
  }

  ret = mbedtls_mpi_read_binary(&ctx_cli.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(Qp).MBEDTLS_PRIVATE(X), srv_x_to_cli, 32);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_read_binary returned %d\n", ret);
    goto exit;
  }

  ret = mbedtls_mpi_read_binary(&ctx_cli.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(Qp).MBEDTLS_PRIVATE(Y), srv_y_to_cli, 32);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_read_binary returned %d\n", ret);
    goto exit;
  }

  time_tick = sl_sleeptimer_get_tick_count();

  #if defined(_SILICON_LABS_32B_SERIES_2)
  ret = mbedtls_ecdh_compute_shared(&ctx_cli.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(grp), &ctx_cli.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(z),
                                    &ctx_cli.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(Qp), &ctx_cli.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(d),
                                    NULL, NULL);
  #else
  ret = mbedtls_ecdh_compute_shared(&ctx_cli.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(grp), &ctx_cli.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(z),
                                    &ctx_cli.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(Qp), &ctx_cli.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(d),
                                    mbedtls_ctr_drbg_random, &ctr_drbg);
  #endif

  time_diff_tick = sl_sleeptimer_get_tick_count() - time_tick;
  time_diff_ms = sl_sleeptimer_tick_to_ms(time_diff_tick);

  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ecdh_compute_shared returned %d\n", ret);
    goto exit;
  }

  mbedtls_printf(" ok  (clock cycles: %ld time: %" PRIu32 " ms)\n",
                 time_diff_tick, time_diff_ms);

  // Verification: are the computed secret equal?
  mbedtls_printf("  . Checking if both computed secrets are equal...");

  ret = mbedtls_mpi_cmp_mpi(&ctx_cli.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(z), &ctx_srv.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(z));
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ecdh_compute_shared returned %d\n", ret);
    goto exit;
  }

  mbedtls_printf(" ok\n");

  dump_mpi("  + Shared Secret: ", &ctx_cli.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(z));

  mbedtls_printf("\n");

  exit:

  mbedtls_ecdh_free(&ctx_srv);
  mbedtls_ecdh_free(&ctx_cli);
  mbedtls_ctr_drbg_free(&ctr_drbg);
  mbedtls_entropy_free(&entropy);
}
