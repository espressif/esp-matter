/***************************************************************************//**
 * @file app_se_manager_macro.h
 * @brief SE manager macros.
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
#ifndef APP_SE_MANAGER_MACRO_H
#define APP_SE_MANAGER_MACRO_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "em_cmu.h"
#include <inttypes.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#if (SE_MANAGER_PRINT == 1)
#define print_error_cycle(CODE, CTX)                                      \
  do {                                                                    \
    sl_status_t ret;                                                      \
    uint32_t cycles;                                                      \
                                                                          \
    if (CTX != NULL) {                                                    \
      sl_se_init_command_context(CTX);                                    \
    }                                                                     \
                                                                          \
    DWT->CYCCNT = 0;                                                      \
    ret = CODE;                                                           \
    cycles = DWT->CYCCNT;                                                 \
                                                                          \
    if (CTX != NULL) {                                                    \
      sl_se_deinit_command_context(CTX);                                  \
      sl_status_print(ret);                                               \
    } else {                                                              \
      if (ret == SL_STATUS_OK) {                                          \
        printf("SL_STATUS_OK");                                           \
      } else {                                                            \
        if ((ret >> 16) != 0xffff) {                                      \
          sl_status_print(ret);                                           \
        } else {                                                          \
          printf("Failed - 0x%08lX", ret);                                \
        }                                                                 \
      }                                                                   \
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

#define print_error_cycle(CODE, CTX)     \
  do {                                   \
    sl_status_t ret;                     \
                                         \
    if (CTX != NULL) {                   \
      sl_se_init_command_context(CTX);   \
    }                                    \
                                         \
    ret = CODE;                          \
    if (CTX != NULL) {                   \
      sl_se_deinit_command_context(CTX); \
    }                                    \
                                         \
    if (ret == SL_STATUS_OK) {           \
      printf("OK\n");                    \
    } else {                             \
      printf("Failed\n");                \
    }                                    \
    return ret;                          \
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
#if (SE_MANAGER_PRINT == 1)
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
#else
  (void)0;
#endif
}

#endif  // APP_SE_MANAGER_MACRO_H
