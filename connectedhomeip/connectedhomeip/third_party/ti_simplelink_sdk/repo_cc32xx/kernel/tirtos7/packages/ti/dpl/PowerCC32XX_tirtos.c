/*
 * Copyright (c) 2015-2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== PowerCC32XX_tirtos.c ========
 */
#include <stdint.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>

/* driverlib header files */
#include <ti/devices/cc32xx/inc/hw_types.h>
#include <ti/devices/cc32xx/driverlib/prcm.h>
#include <ti/devices/cc32xx/driverlib/cpu.h>
#include <ti/devices/cc32xx/driverlib/rom.h>
#include <ti/devices/cc32xx/driverlib/rom_map.h>

#include <ti/drivers/ITM.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC32XX.h>

/* bitmask of constraints that disallow LPDS */
#define LPDS_DISALLOWED (1 << PowerCC32XX_DISALLOW_LPDS)

/*
 * macro to resolve RTC count when reading from 40MHz domain (need to read 3
 * times and compare values to ensure proper sync with RTC running at 32786Hz)
 */
#define COUNT_WITHIN_TRESHOLD(a, b, c, th) \
        ((((b) - (a)) <= (th)) ? (b) : (c))

/* macros for enabling RTC */
#define HWREG(x) (*((volatile unsigned long *)(x)))
#define HIB3P3_BASE                        0x4402F800
#define HIB3P3_O_MEM_HIB_RTC_TIMER_ENABLE  0x00000004

static uint64_t getCountsRTC(void);

/*
 *  ======== PowerCC32XX_sleepPolicy ========
 */
void PowerCC32XX_sleepPolicy(void)
{
    uint32_t constraintMask;
    uint32_t deltaTicksPerRTC;
    uint32_t beforeTick;
    uint64_t beforeRTC;
    uint64_t wakeRTC;
    uint64_t deltaTime;
    uint64_t remain;
    uint64_t temp64;
    uint64_t latency;
    uint32_t deltaTick;
    uint32_t workTick;
    uint32_t newTick;
    uint32_t newDelta;
    uint32_t taskKey;
    uint32_t swiKey;
    uint32_t i;
    bool slept = false;

    /* disable servicing of interrupts */
    MAP_CPUcpsid();

    /* disable Swi scheduling */
    swiKey = Hwi_swiDisable();

    /* disable Task scheduling */
    taskKey = Task_disable();

    /* query the declared constraints */
    constraintMask = Power_getConstraintMask();

    /* check if there is a constraint to disallow LPDS */
    if ((constraintMask & LPDS_DISALLOWED) == 0) {

        /* query Clock for the next tick that has a scheduled timeout */
        deltaTick = Clock_getTicksUntilTimeout();

        /* convert next tick to units of microseconds in the future */
        deltaTime = (uint64_t)deltaTick * (uint64_t)Clock_tickPeriod;

        /* check if there is enough time to transition to/from LPDS */
        latency = Power_getTransitionLatency(PowerCC32XX_LPDS, Power_TOTAL);
        if (deltaTime > latency) {

            /* decision is now made, going to transition to LPDS ... */

            /* get current Clock tick count */
            beforeTick = Clock_getTicks();

            /* sample the RTC count before sleep */
            beforeRTC = getCountsRTC();

            /* set tick count for next scheduled work */
            workTick = beforeTick + deltaTick;

            /* stop SYSTICK */
            Clock_tickStop();

            /* compute the time interval for the LPDS wake timer */
            deltaTime -= latency;

            /* convert the interval in usec to units of RTC counts */
            remain = (deltaTime * 32768) / 1000000;

            /* if necessary clip the interval to a max 32-bit value */
            if (remain > 0xFFFFFFFF) {
                remain = 0xFFFFFFFF;
            }

            /* set the interval for the LPDS wake timer */
            MAP_PRCMLPDSIntervalSet(remain);

            /* enable the RTC as an LPDS wake source */
            MAP_PRCMLPDSWakeupSourceEnable(PRCM_LPDS_TIMER);

            /* Flush any remaining log messages in the ITM */
            ITM_flush();

            /* now go to LPDS ... */
            Power_sleep(PowerCC32XX_LPDS);

            /* Restore ITM settings */
            ITM_restore();

            /* get the RTC count after wakeup */
            wakeRTC = getCountsRTC();

            /* compute delta Clock ticks spent in LPDS */
            temp64 = (((wakeRTC - beforeRTC) * 1000000) /
                (32768 * Clock_tickPeriod));
            deltaTicksPerRTC = (uint32_t) temp64;

            /* compute resulting 'new' tick count */
            newTick = beforeTick + deltaTicksPerRTC;

            /*
             * now update Clock's internal tick count to reflect elapsed
             * time per the RTC, accomodating eventual rollover of the
             * 32-bit Clock tick count
             *
             * 'workDelta' is 'deltaTick', the number of ticks from before
             *  go to LPDS to when work is scheduled with Clock
             *
             * 'newDelta' is the number of ticks from before go to LPDS to
             * the newly computed newTick
             */
            newDelta = newTick - beforeTick;

            /* first, handle normal case of early wake (before workTick) ... */
            if (newDelta < deltaTick) {
                /* just set count to newTick */
                Clock_setTicks(newTick);
            }
            /* else, if woke on workTick or later */
            else {
                /*
                 * update tick count, trigger Clock Swi to run ASAP
                 *  1. set tick count to workTick - 1
                 *  2. call Clock_doTick() as many times as necessary to
                 *     account for the number of missed ticks [each call will
                 *     increment tick count and post the Clock Swi, which will
                 *     run Clock_workFunc(), which uses the Swi post count to
                 *     catch up for ticks that weren't serviced on time]
                 */
                Clock_setTicks(workTick - 1);
                for (i = 0; i < (newDelta - deltaTick + 1); i++) {
                    ti_sysbios_knl_Clock_doTick(0);
                }
            }

            /* reconfigure and restart the SYSTICK timer */
            Clock_tickReconfig();
            Clock_tickStart();

            /* set a flag to indicate LPDS was invoked */
            slept = TRUE;
        }
    }

    /* sleep, but only if did not invoke a sleep state above */
    if (!(slept)) {
        /* Flush any remaining log messages in the ITM */
        ITM_flush();
        MAP_PRCMSleepEnter();
        /* Restore ITM settings */
        ITM_restore();
    }

    /* re-enable interrupts */
    MAP_CPUcpsie();

    /* restore Swi scheduling */
    Hwi_swiRestore(swiKey);

    /* restore Task scheduling */
    Task_restore(taskKey);
}

/*
 *  ======== PowerCC32XX_initPolicy ========
 */
void PowerCC32XX_initPolicy(void)
{
    /* enable the RTC; if it is already enabled this has no affect */
    HWREG(HIB3P3_BASE + HIB3P3_O_MEM_HIB_RTC_TIMER_ENABLE) = 0x1;

    /*
     * check that the Clock module configuration is compatible with this
     * policy; if it is not, declare a constraint to prohibit LPDS
     */
    if ((Clock_tickSource != Clock_TickSource_TIMER) ||
        (Clock_tickMode != Clock_TickMode_PERIODIC)) {
        Power_setConstraint(PowerCC32XX_DISALLOW_LPDS);
    }
}

/*
 *  ======== Timer_disableCC32XX ========
 *  This is a support function for the ti.sysbios.family.arm.lm4.Timer module.
 *  It allows the Timer module in the kernel to cooperate with the Power
 *  framework in ti/drivers.  This function has a strong definition, and will
 *  override the default support function the Timer module provides (with a
 *  weak definition), which is used when power management is not used in
 *  an application.
 */
void ti_sysbios_family_arm_lm4_Timer_disableCC32XX__I(int32_t id)
{
    uint32_t key;

    key = Hwi_disable();

    switch (id) {
       case 0: Power_releaseDependency(PowerCC32XX_PERIPH_TIMERA0);
                break;

        case 1: Power_releaseDependency(PowerCC32XX_PERIPH_TIMERA1);
                break;

        case 2: Power_releaseDependency(PowerCC32XX_PERIPH_TIMERA2);
                break;

        case 3: Power_releaseDependency(PowerCC32XX_PERIPH_TIMERA3);
                break;

        default:
                break;
    }

    /* release the disallow LPDS constraint when the GP timer is disabled */
    Power_releaseConstraint(PowerCC32XX_DISALLOW_LPDS);

    Hwi_restore(key);
}

/*
 *  ======== Timer_enableCC32XX ========
 *  This is a support function for the ti.sysbios.family.arm.lm4.Timer module.
 *  It allows the Timer module in the kernel to cooperate with the Power
 *  framework in ti/drivers.  This function has a strong definition, and will
 *  override the default support function the Timer module provides (with a
 *  weak definition), which is used when power management is not used in
 *  an application.
 */
void ti_sysbios_family_arm_lm4_Timer_enableCC32XX__I(int32_t id)
{
    uint32_t key;

    key = Hwi_disable();

    switch (id) {
        case 0: Power_setDependency(PowerCC32XX_PERIPH_TIMERA0);
                break;

        case 1: Power_setDependency(PowerCC32XX_PERIPH_TIMERA1);
                break;

        case 2: Power_setDependency(PowerCC32XX_PERIPH_TIMERA2);
                break;

        case 3: Power_setDependency(PowerCC32XX_PERIPH_TIMERA3);
                break;

        default:
                break;
    }

    /* declare the disallow LPDS constraint while GP timer is in use */
    Power_setConstraint(PowerCC32XX_DISALLOW_LPDS);

    Hwi_restore(key);
}

/*
 *  ======== getCountsRTC ========
 */
static uint64_t getCountsRTC(void)
{
    uint64_t count[3];
    uint64_t curr;
    uint32_t i;

    /*
     *  get the current RTC count, using the fast interface; to use the
     *  fast interface the count must be read three times, and then
     *  the value that matches on at least two of the reads is chosen
     */
    for (i = 0; i < 3; i++) {
        count[i] = MAP_PRCMSlowClkCtrFastGet();
    }
    curr = COUNT_WITHIN_TRESHOLD(count[0], count[1], count[2], 1);

    return (curr);
}
