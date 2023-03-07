/*
 *
 * Copyright 2016-2018 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sm_timer.h>
#include <stdint.h>

#include "board.h"

#if defined(SSS_USE_FTR_FILE)
#include "fsl_sss_ftr.h"
#else
#include "fsl_sss_ftr_default.h"
#endif

#if defined(__GNUC__)
#pragma GCC push_options
#pragma GCC optimize("O0")
#endif


#if !defined(SDK_OS_FREE_RTOS) && !defined(FSL_RTOS_FREE_RTOS)

extern volatile uint32_t gtimer_kinetis_msticks; // counter for 1ms SysTicks

//__INLINE
static void systick_delay(const uint32_t delayTicks) {
    volatile uint32_t currentTicks;
    if(delayTicks >= 0x7FFFFFFFu) {
        return;
    }

    __disable_irq();

    if ((gtimer_kinetis_msticks) & 0x80000000u)
    {
        /* gtimer_kinetis_msticks has increased drastically (MSB is set),
         * So, reset gtimer_kinetis_msticks before it's too late to detect an
         * overflow. */
        gtimer_kinetis_msticks = 0;
    }

    currentTicks = gtimer_kinetis_msticks;   // read current tick counter

    __DSB();
    __enable_irq();

    // Now loop until required number of ticks passes
    while ((gtimer_kinetis_msticks - currentTicks) <= delayTicks) {
#if !defined(IMX_RT)
#ifdef __WFI
        __WFI();
#endif
#endif
    }
}

#ifndef WEAK
#define WEAK __attribute__ ((weak))
#endif
WEAK void SysTick_Handler_APP_CB(void);
WEAK void SysTick_Handler_APP_CB() {

}


/* interrupt handler for system ticks */
void SysTick_Handler(void) {
    gtimer_kinetis_msticks+=1;
    SysTick_Handler_APP_CB();
}


/* initializes the system tick counter
 * return 0 on succes, 1 on failure */
uint32_t sm_initSleep() {
    gtimer_kinetis_msticks = 0;
    SysTick_Config(SystemCoreClock / 1000);
    __enable_irq();
    return 0;
}

/**
 * Implement a blocking (for the calling thread) wait for a number of milliseconds.
 */
void sm_sleep(uint32_t msec) {
#if SSS_HAVE_NXPNFCRDLIB
    //NXPNFCRDLIB also uses systick for a different purpose hence this is done
    sm_initSleep();
#endif
    /* if struck here check whether sm_initSleep() is called */
    systick_delay(MS_TO_TICKS(msec));
}

#if defined(__GNUC__)
#pragma GCC pop_options
#endif

#endif /* !SDK_OS_FREE_RTOS && ! FSL_RTOS_FREE_RTOS */
