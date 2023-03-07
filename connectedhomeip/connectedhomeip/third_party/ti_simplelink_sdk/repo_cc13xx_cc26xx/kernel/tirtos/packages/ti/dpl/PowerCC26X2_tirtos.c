/*
 * Copyright (c) 2017-2020, Texas Instruments Incorporated
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
 * EXEMPLARY, OR CONSEQueueNTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== PowerCC26X2_tirtos.c ========
 */

#include <stdbool.h>
#include <ti/drivers/ITM.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26X2.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/hal/Hwi.h>

/* driverlib header files */
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(driverlib/prcm.h)
#include DeviceFamily_constructPath(driverlib/osc.h)
#include DeviceFamily_constructPath(driverlib/cpu.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include DeviceFamily_constructPath(driverlib/vims.h)

extern PowerCC26X2_ModuleState PowerCC26X2_module;

UInt PowerCC26XX_taskKey;
UInt PowerCC26XX_swiKey;

/*
 *  ======== PowerCC26XX_standbyPolicy ========
 */
void PowerCC26XX_standbyPolicy(void)
{
    bool justIdle = TRUE;
    uint32_t constraints;
    uint32_t ticks, time;

    /* disable interrupts */
    CPUcpsid();

    /* check operating conditions, optimally choose DCDC versus GLDO */
    SysCtrl_DCDC_VoltageConditionalControl();

    /* query the declared constraints */
    constraints = Power_getConstraintMask();

    /* do quick check to see if only WFI allowed; if yes, do it now */
    if ((constraints &
        ((1 << PowerCC26XX_DISALLOW_STANDBY) | (1 << PowerCC26XX_DISALLOW_IDLE))) ==
        ((1 << PowerCC26XX_DISALLOW_STANDBY) | (1 << PowerCC26XX_DISALLOW_IDLE))) {

        /* Flush any remaining log messages in the ITM */
        ITM_flush();
        PRCMSleep();
        /* Restore ITM settings */
        ITM_restore();
    }
    /*
     *  check if any sleep modes are allowed for automatic activation
     */
    else {
        /* check if we are allowed to go to standby */
        if ((constraints & (1 << PowerCC26XX_DISALLOW_STANDBY)) == 0) {
            /*
             * Check how many ticks until the next scheduled wakeup.  A value of
             * zero indicates a wakeup will occur as the current Clock tick
             * period expires; a very large value indicates a very large number
             * of Clock tick periods will occur before the next scheduled
             * wakeup.
             */
            ticks = Clock_getTicksUntilInterrupt();

            /* convert ticks to usec */
            time = ticks * Clock_tickPeriod;

            /* check if can go to STANDBY */
            if (time > Power_getTransitionLatency(PowerCC26XX_STANDBY,
                Power_TOTAL)) {

                /* schedule the wakeup event */
                ticks -= PowerCC26X2_WAKEDELAYSTANDBY / Clock_tickPeriod;
                Clock_setTimeout(Clock_handle((Clock_Struct *)&PowerCC26X2_module.clockObj), ticks);
                Clock_start(Clock_handle((Clock_Struct *)&PowerCC26X2_module.clockObj));

                /* Flush any remaining log messages in the ITM */
                ITM_flush();

                /* go to standby mode */
                Power_sleep(PowerCC26XX_STANDBY);

                /* Restore ITM settings */
                ITM_restore();

                Clock_stop(Clock_handle((Clock_Struct *)&PowerCC26X2_module.clockObj));
                justIdle = FALSE;
            }
        }

        /* idle if allowed */
        if (justIdle) {

            /* Flush any remaining log messages in the ITM */
            ITM_flush();

            /*
             * Power off the CPU domain; VIMS will power down if SYSBUS is
             * powered down, and SYSBUS will power down if there are no
             * dependencies
             * NOTE: if radio driver is active it must force SYSBUS enable to
             * allow access to the bus and SRAM
             */
            if ((constraints & (1 << PowerCC26XX_DISALLOW_IDLE)) == 0) {
                uint32_t modeVIMS;
                /* 1. Get the current VIMS mode */
                do {
                    modeVIMS = VIMSModeGet(VIMS_BASE);
                } while (modeVIMS == VIMS_MODE_CHANGING);

                /* 2. Configure flash to remain on in IDLE or not and keep
                 *    VIMS powered on if it is configured as GPRAM
                 * 3. Always keep cache retention ON in IDLE
                 * 4. Turn off the CPU power domain
                 * 5. Ensure any possible outstanding AON writes complete
                 * 6. Enter IDLE
                 */
                if ((constraints & (1 << PowerCC26XX_NEED_FLASH_IN_IDLE)) ||
                    (modeVIMS == VIMS_MODE_DISABLED)) {
                    SysCtrlIdle(VIMS_ON_BUS_ON_MODE);
                }
                else {
                    SysCtrlIdle(VIMS_ON_CPU_ON_MODE);
                }

                /* 7. Make sure MCU and AON are in sync after wakeup */
                SysCtrlAonUpdate();
            }
            else {
                PRCMSleep();
            }

            /* Restore ITM settings */
            ITM_restore();
        }
    }

    /* re-enable interrupts */
    CPUcpsie();
}

/*
 *  ======== PowerCC26XX_schedulerDisable ========
 */
void PowerCC26XX_schedulerDisable()
{
    PowerCC26XX_taskKey = Task_disable();
    PowerCC26XX_swiKey = Swi_disable();
}

/*
 *  ======== PowerCC26XX_schedulerRestore ========
 */
void PowerCC26XX_schedulerRestore()
{
    Swi_restore(PowerCC26XX_swiKey);
    Task_restore(PowerCC26XX_taskKey);
}

/*
 *  ======== Timer_disableCC26xx ========
 *  This is a support function for the ti.sysbios.family.arm.lm4.Timer module.
 *  It allows the Timer module in the kernel to cooperate with the Power
 *  framework in ti/drivers.  This function has a strong definition, and will
 *  override the default support function the Timer module provides (with a
 *  weak definition), which is used when power management is not used in
 *  an application.
 */
void ti_sysbios_family_arm_lm4_Timer_disableCC26xx__I(int32_t id)
{
    uint32_t key;

    key = Hwi_disable();

    switch (id) {
       case 0: Power_releaseDependency(PowerCC26XX_PERIPH_GPT0);
                break;

        case 1: Power_releaseDependency(PowerCC26XX_PERIPH_GPT1);
                break;

        case 2: Power_releaseDependency(PowerCC26XX_PERIPH_GPT2);
                break;

        case 3: Power_releaseDependency(PowerCC26XX_PERIPH_GPT3);
                break;

        default:
                break;
    }

    /* release the disallow standby constraint when the GP timer is disabled */
    Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);

    Hwi_restore(key);
}

/*
 *  ======== Timer_enableCC26xx ========
 *  This is a support function for the ti.sysbios.family.arm.lm4.Timer module.
 *  It allows the Timer module in the kernel to cooperate with the Power
 *  framework in ti/drivers.  This function has a strong definition, and will
 *  override the default support function the Timer module provides (with a
 *  weak definition), which is used when power management is not used in
 *  an application.
 */
void ti_sysbios_family_arm_lm4_Timer_enableCC26xx__I(int32_t id)
{
    uint32_t key;

    key = Hwi_disable();

    switch (id) {
        case 0: Power_setDependency(PowerCC26XX_PERIPH_GPT0);
                break;

        case 1: Power_setDependency(PowerCC26XX_PERIPH_GPT1);
                break;

        case 2: Power_setDependency(PowerCC26XX_PERIPH_GPT2);
                break;

        case 3: Power_setDependency(PowerCC26XX_PERIPH_GPT3);
                break;

        default:
                break;
    }

    /* declare the disallow standby constraint while GP timer is in use */
    Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);

    Hwi_restore(key);
}
