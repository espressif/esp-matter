/***************************************************************************//**
 * @file app_psa_crypto_macro.h
 * @brief PSA Crypto macros.
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
#ifndef APP_PSA_CRYPTO_MACRO_H
#define APP_PSA_CRYPTO_MACRO_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "em_cmu.h"
#include "psa/crypto.h"
#include <inttypes.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#if defined(DWT_CTRL_CYCCNTENA_Msk) && (PSA_CRYPTO_PRINT == 1)
#define print_error_cycle(CODE)                                           \
  do {                                                                    \
    psa_status_t ret;                                                     \
    uint32_t cycles;                                                      \
                                                                          \
    DWT->CYCCNT = 0;                                                      \
    ret = CODE;                                                           \
    cycles = DWT->CYCCNT;                                                 \
                                                                          \
    if (ret == PSA_SUCCESS) {                                             \
      printf("PSA_SUCCESS");                                              \
    } else {                                                              \
      printf("PSA_ERROR: %ld", ret);                                      \
    }                                                                     \
                                                                          \
    if (cycles < (CMU_ClockFreqGet(cmuClock_CORE) / 10)) {                \
      printf(" (cycles: %" PRIu32 " time: %" PRIu32 " us)\n",             \
             cycles,                                                      \
             (cycles * 10) / (CMU_ClockFreqGet(cmuClock_CORE) / 100000)); \
    }                                                                     \
    else {                                                                \
      printf(" (cycles: %" PRIu32 " time: %" PRIu32 " ms)\n",             \
             cycles,                                                      \
             cycles / (CMU_ClockFreqGet(cmuClock_CORE) / 1000));          \
    }                                                                     \
    return ret;                                                           \
  } while (0)

#else

#define print_error_cycle(CODE)     \
  do {                              \
    psa_status_t ret;               \
                                    \
    ret = CODE;                     \
    if (ret == PSA_SUCCESS) {       \
      printf("OK\n");               \
    } else {                        \
      printf("Failed: %ld\n", ret); \
    }                               \
    return ret;                     \
  } while (0)

#endif

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Enable cycle counter in DWT.
 ******************************************************************************/
__STATIC_INLINE void enable_cycle_count(void)
{
#if defined(DWT_CTRL_CYCCNTENA_Msk) && (PSA_CRYPTO_PRINT == 1)
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
#else
  (void)0;
#endif
}

#endif  // APP_PSA_CRYPTO_MACRO_H
