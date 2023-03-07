/*
 *
 * Copyright 2016-2018,2020 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sm_timer.h>
#include <stdint.h>

#include "board.h"

#if defined(__GNUC__)
#pragma GCC push_options
#pragma GCC optimize("O0")
#endif

volatile uint32_t gtimer_kinetis_msticks; // counter for 1ms SysTicks

volatile int gusleep_delay;

#define CORR_FRDM_K64F_ARMCC   (1000/100)
#define CORR_FRDM_K64F_ICCARM  (1000/108)
#define CORR_FRDM_K64F_GCC     (1000/100)

#define CORR_LPC_K32W_ARMCC   (1000/100)
#define CORR_LPC_K32W_ICCARM  (1000/108)
#define CORR_LPC_K32W_GCC     (1000/100)

#define CORR_MIMXRT1060_EVK_ARMCC   (1000/125)
#define CORR_MIMXRT1060_EVK_ICCARM  (1000/125)
#define CORR_MIMXRT1060_EVK_GCC     (1000/125)

#define CORR_FRDM_K82F_ARMCC   (1000/76)
#define CORR_FRDM_K82F_ICCARM  (1000/83)
#define CORR_FRDM_K82F_GCC     (1000/76)

#define CORR_FRDM_KW41Z_ARMCC  (1000/81)
#define CORR_FRDM_KW41Z_ICCARM (1000/83)
#define CORR_FRDM_KW41Z_GCC    (1000/251)  //for consistency sake 4 is specified in same format

#define CORR_LPC54018_ARMCC  (1000/125)
#define CORR_LPC54018_ICCARM (1000/112)
#define CORR_LPC54018_GCC    (1000/125)  //for consistency sake 4 is specified in same format


#if defined(FRDM_KW41Z)
#    if defined(__ARMCC_VERSION)
#       define CORRECTION_TOLERENCE CORR_FRDM_KW41Z_ARMCC
#    elif defined(__ICCARM__)
#       define CORRECTION_TOLERENCE CORR_FRDM_KW41Z_ICCARM
#    else
#       define CORRECTION_TOLERENCE CORR_FRDM_KW41Z_GCC
#    endif
#elif defined(FRDM_K82F)
#    if defined(__ARMCC_VERSION)
#       define CORRECTION_TOLERENCE CORR_FRDM_K82F_ARMCC
#    elif defined(__ICCARM__)
#       define CORRECTION_TOLERENCE CORR_FRDM_K82F_ICCARM
#    else
#       define CORRECTION_TOLERENCE CORR_FRDM_K82F_GCC
#    endif

#elif defined(IMX_RT)
#    if defined(__ARMCC_VERSION)
#       define CORRECTION_TOLERENCE CORR_MIMXRT1060_EVK_ARMCC
#    elif defined(__ICCARM__)
#       define CORRECTION_TOLERENCE CORR_MIMXRT1060_EVK_ICCARM
#    else
#       define CORRECTION_TOLERENCE CORR_MIMXRT1060_EVK_GCC
#    endif
#elif defined(CPU_LPC54018)
#    if defined(__ARMCC_VERSION)
#       define CORRECTION_TOLERENCE CORR_LPC54018_ARMCC
#    elif defined(__ICCARM__)
#       define CORRECTION_TOLERENCE CORR_LPC54018_ICCARM
#    else
#       define CORRECTION_TOLERENCE CORR_LPC54018_GCC
#    endif

#elif defined(LPC_K32W)
#    if defined(__ARMCC_VERSION)
#       define CORRECTION_TOLERENCE CORR_LPC_K32W_ARMCC
#    elif defined(__ICCARM__)
#       define CORRECTION_TOLERENCE CORR_LPC_K32W_ICCARM
#    else
#       define CORRECTION_TOLERENCE CORR_LPC_K32W_GCC
#    endif
#else //defined(FRDM_K64F)
#    if defined(__ARMCC_VERSION)
#       define CORRECTION_TOLERENCE CORR_FRDM_K64F_ARMCC
#    elif defined(__ICCARM__)
#       define CORRECTION_TOLERENCE CORR_FRDM_K64F_ICCARM
#    else
#       define CORRECTION_TOLERENCE CORR_FRDM_K64F_GCC
#    endif
#endif


//for ARM6 taken care at file level
// #ifdef __ARMCC_VERSION
// #pragma O0
// #endif

void sm_usleep(uint32_t microsec) {
    gusleep_delay = microsec * CORRECTION_TOLERENCE;
    while (gusleep_delay-- ) {
        __NOP();
    }
}

#if defined(__GNUC__)
#pragma GCC pop_options
#endif