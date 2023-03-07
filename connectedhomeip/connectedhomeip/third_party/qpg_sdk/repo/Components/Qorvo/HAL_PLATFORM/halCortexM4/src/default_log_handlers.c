/*
 * Copyright (c) 2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *   Default logging handlers for ARM-based devices.
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "hal.h"
#include "gpLog.h"
#include "core_cm4.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID     GP_COMPONENT_ID_HALCORTEXM4

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    External Function Declarations
 *****************************************************************************/
#if defined(__ICCARM__)
#if __CM4_CMSIS_VERSION <= 0x04001E
/* SCB System Handler Control and State Register Definitions */
#define SCB_SHCSR_USGFAULTENA_Pos          18U                                            /*!< SCB SHCSR: USGFAULTENA Position */
#define SCB_SHCSR_USGFAULTENA_Msk          (1UL << SCB_SHCSR_USGFAULTENA_Pos)             /*!< SCB SHCSR: USGFAULTENA Mask */

#define SCB_SHCSR_BUSFAULTENA_Pos          17U                                            /*!< SCB SHCSR: BUSFAULTENA Position */
#define SCB_SHCSR_BUSFAULTENA_Msk          (1UL << SCB_SHCSR_BUSFAULTENA_Pos)             /*!< SCB SHCSR: BUSFAULTENA Mask */

#define SCB_SHCSR_MEMFAULTENA_Pos          16U                                            /*!< SCB SHCSR: MEMFAULTENA Position */
#define SCB_SHCSR_MEMFAULTENA_Msk          (1UL << SCB_SHCSR_MEMFAULTENA_Pos)             /*!< SCB SHCSR: MEMFAULTENA Mask */

#define SCB_SHCSR_SVCALLPENDED_Pos         15U                                            /*!< SCB SHCSR: SVCALLPENDED Position */
#define SCB_SHCSR_SVCALLPENDED_Msk         (1UL << SCB_SHCSR_SVCALLPENDED_Pos)            /*!< SCB SHCSR: SVCALLPENDED Mask */

#define SCB_SHCSR_BUSFAULTPENDED_Pos       14U                                            /*!< SCB SHCSR: BUSFAULTPENDED Position */
#define SCB_SHCSR_BUSFAULTPENDED_Msk       (1UL << SCB_SHCSR_BUSFAULTPENDED_Pos)          /*!< SCB SHCSR: BUSFAULTPENDED Mask */

#define SCB_SHCSR_MEMFAULTPENDED_Pos       13U                                            /*!< SCB SHCSR: MEMFAULTPENDED Position */
#define SCB_SHCSR_MEMFAULTPENDED_Msk       (1UL << SCB_SHCSR_MEMFAULTPENDED_Pos)          /*!< SCB SHCSR: MEMFAULTPENDED Mask */

#define SCB_SHCSR_USGFAULTPENDED_Pos       12U                                            /*!< SCB SHCSR: USGFAULTPENDED Position */
#define SCB_SHCSR_USGFAULTPENDED_Msk       (1UL << SCB_SHCSR_USGFAULTPENDED_Pos)          /*!< SCB SHCSR: USGFAULTPENDED Mask */

#define SCB_SHCSR_SYSTICKACT_Pos           11U                                            /*!< SCB SHCSR: SYSTICKACT Position */
#define SCB_SHCSR_SYSTICKACT_Msk           (1UL << SCB_SHCSR_SYSTICKACT_Pos)              /*!< SCB SHCSR: SYSTICKACT Mask */

#define SCB_SHCSR_PENDSVACT_Pos            10U                                            /*!< SCB SHCSR: PENDSVACT Position */
#define SCB_SHCSR_PENDSVACT_Msk            (1UL << SCB_SHCSR_PENDSVACT_Pos)               /*!< SCB SHCSR: PENDSVACT Mask */

#define SCB_SHCSR_MONITORACT_Pos            8U                                            /*!< SCB SHCSR: MONITORACT Position */
#define SCB_SHCSR_MONITORACT_Msk           (1UL << SCB_SHCSR_MONITORACT_Pos)              /*!< SCB SHCSR: MONITORACT Mask */

#define SCB_SHCSR_SVCALLACT_Pos             7U                                            /*!< SCB SHCSR: SVCALLACT Position */
#define SCB_SHCSR_SVCALLACT_Msk            (1UL << SCB_SHCSR_SVCALLACT_Pos)               /*!< SCB SHCSR: SVCALLACT Mask */

#define SCB_SHCSR_USGFAULTACT_Pos           3U                                            /*!< SCB SHCSR: USGFAULTACT Position */
#define SCB_SHCSR_USGFAULTACT_Msk          (1UL << SCB_SHCSR_USGFAULTACT_Pos)             /*!< SCB SHCSR: USGFAULTACT Mask */

#define SCB_SHCSR_BUSFAULTACT_Pos           1U                                            /*!< SCB SHCSR: BUSFAULTACT Position */
#define SCB_SHCSR_BUSFAULTACT_Msk          (1UL << SCB_SHCSR_BUSFAULTACT_Pos)             /*!< SCB SHCSR: BUSFAULTACT Mask */

#define SCB_SHCSR_MEMFAULTACT_Pos           0U                                            /*!< SCB SHCSR: MEMFAULTACT Position */
#define SCB_SHCSR_MEMFAULTACT_Msk          (1UL /*<< SCB_SHCSR_MEMFAULTACT_Pos*/)         /*!< SCB SHCSR: MEMFAULTACT Mask */

/* SCB Configurable Fault Status Register Definitions */
#define SCB_CFSR_USGFAULTSR_Pos            16U                                            /*!< SCB CFSR: Usage Fault Status Register Position */
#define SCB_CFSR_USGFAULTSR_Msk            (0xFFFFUL << SCB_CFSR_USGFAULTSR_Pos)          /*!< SCB CFSR: Usage Fault Status Register Mask */

#define SCB_CFSR_BUSFAULTSR_Pos             8U                                            /*!< SCB CFSR: Bus Fault Status Register Position */
#define SCB_CFSR_BUSFAULTSR_Msk            (0xFFUL << SCB_CFSR_BUSFAULTSR_Pos)            /*!< SCB CFSR: Bus Fault Status Register Mask */

#define SCB_CFSR_MEMFAULTSR_Pos             0U                                            /*!< SCB CFSR: Memory Manage Fault Status Register Position */
#define SCB_CFSR_MEMFAULTSR_Msk            (0xFFUL /*<< SCB_CFSR_MEMFAULTSR_Pos*/)        /*!< SCB CFSR: Memory Manage Fault Status Register Mask */

/* MemManage Fault Status Register (part of SCB Configurable Fault Status Register) */
#define SCB_CFSR_MMARVALID_Pos             (SCB_SHCSR_MEMFAULTACT_Pos + 7U)               /*!< SCB CFSR (MMFSR): MMARVALID Position */
#define SCB_CFSR_MMARVALID_Msk             (1UL << SCB_CFSR_MMARVALID_Pos)                /*!< SCB CFSR (MMFSR): MMARVALID Mask */

#define SCB_CFSR_MLSPERR_Pos               (SCB_SHCSR_MEMFAULTACT_Pos + 5U)               /*!< SCB CFSR (MMFSR): MLSPERR Position */
#define SCB_CFSR_MLSPERR_Msk               (1UL << SCB_CFSR_MLSPERR_Pos)                  /*!< SCB CFSR (MMFSR): MLSPERR Mask */

#define SCB_CFSR_MSTKERR_Pos               (SCB_SHCSR_MEMFAULTACT_Pos + 4U)               /*!< SCB CFSR (MMFSR): MSTKERR Position */
#define SCB_CFSR_MSTKERR_Msk               (1UL << SCB_CFSR_MSTKERR_Pos)                  /*!< SCB CFSR (MMFSR): MSTKERR Mask */

#define SCB_CFSR_MUNSTKERR_Pos             (SCB_SHCSR_MEMFAULTACT_Pos + 3U)               /*!< SCB CFSR (MMFSR): MUNSTKERR Position */
#define SCB_CFSR_MUNSTKERR_Msk             (1UL << SCB_CFSR_MUNSTKERR_Pos)                /*!< SCB CFSR (MMFSR): MUNSTKERR Mask */

#define SCB_CFSR_DACCVIOL_Pos              (SCB_SHCSR_MEMFAULTACT_Pos + 1U)               /*!< SCB CFSR (MMFSR): DACCVIOL Position */
#define SCB_CFSR_DACCVIOL_Msk              (1UL << SCB_CFSR_DACCVIOL_Pos)                 /*!< SCB CFSR (MMFSR): DACCVIOL Mask */

#define SCB_CFSR_IACCVIOL_Pos              (SCB_SHCSR_MEMFAULTACT_Pos + 0U)               /*!< SCB CFSR (MMFSR): IACCVIOL Position */
#define SCB_CFSR_IACCVIOL_Msk              (1UL /*<< SCB_CFSR_IACCVIOL_Pos*/)             /*!< SCB CFSR (MMFSR): IACCVIOL Mask */

/* BusFault Status Register (part of SCB Configurable Fault Status Register) */
#define SCB_CFSR_BFARVALID_Pos            (SCB_CFSR_BUSFAULTSR_Pos + 7U)                  /*!< SCB CFSR (BFSR): BFARVALID Position */
#define SCB_CFSR_BFARVALID_Msk            (1UL << SCB_CFSR_BFARVALID_Pos)                 /*!< SCB CFSR (BFSR): BFARVALID Mask */

#define SCB_CFSR_LSPERR_Pos               (SCB_CFSR_BUSFAULTSR_Pos + 5U)                  /*!< SCB CFSR (BFSR): LSPERR Position */
#define SCB_CFSR_LSPERR_Msk               (1UL << SCB_CFSR_LSPERR_Pos)                    /*!< SCB CFSR (BFSR): LSPERR Mask */

#define SCB_CFSR_STKERR_Pos               (SCB_CFSR_BUSFAULTSR_Pos + 4U)                  /*!< SCB CFSR (BFSR): STKERR Position */
#define SCB_CFSR_STKERR_Msk               (1UL << SCB_CFSR_STKERR_Pos)                    /*!< SCB CFSR (BFSR): STKERR Mask */

#define SCB_CFSR_UNSTKERR_Pos             (SCB_CFSR_BUSFAULTSR_Pos + 3U)                  /*!< SCB CFSR (BFSR): UNSTKERR Position */
#define SCB_CFSR_UNSTKERR_Msk             (1UL << SCB_CFSR_UNSTKERR_Pos)                  /*!< SCB CFSR (BFSR): UNSTKERR Mask */

#define SCB_CFSR_IMPRECISERR_Pos          (SCB_CFSR_BUSFAULTSR_Pos + 2U)                  /*!< SCB CFSR (BFSR): IMPRECISERR Position */
#define SCB_CFSR_IMPRECISERR_Msk          (1UL << SCB_CFSR_IMPRECISERR_Pos)               /*!< SCB CFSR (BFSR): IMPRECISERR Mask */

#define SCB_CFSR_PRECISERR_Pos            (SCB_CFSR_BUSFAULTSR_Pos + 1U)                  /*!< SCB CFSR (BFSR): PRECISERR Position */
#define SCB_CFSR_PRECISERR_Msk            (1UL << SCB_CFSR_PRECISERR_Pos)                 /*!< SCB CFSR (BFSR): PRECISERR Mask */

#define SCB_CFSR_IBUSERR_Pos              (SCB_CFSR_BUSFAULTSR_Pos + 0U)                  /*!< SCB CFSR (BFSR): IBUSERR Position */
#define SCB_CFSR_IBUSERR_Msk              (1UL << SCB_CFSR_IBUSERR_Pos)                   /*!< SCB CFSR (BFSR): IBUSERR Mask */

/* UsageFault Status Register (part of SCB Configurable Fault Status Register) */
#define SCB_CFSR_DIVBYZERO_Pos            (SCB_CFSR_USGFAULTSR_Pos + 9U)                  /*!< SCB CFSR (UFSR): DIVBYZERO Position */
#define SCB_CFSR_DIVBYZERO_Msk            (1UL << SCB_CFSR_DIVBYZERO_Pos)                 /*!< SCB CFSR (UFSR): DIVBYZERO Mask */

#define SCB_CFSR_UNALIGNED_Pos            (SCB_CFSR_USGFAULTSR_Pos + 8U)                  /*!< SCB CFSR (UFSR): UNALIGNED Position */
#define SCB_CFSR_UNALIGNED_Msk            (1UL << SCB_CFSR_UNALIGNED_Pos)                 /*!< SCB CFSR (UFSR): UNALIGNED Mask */

#define SCB_CFSR_NOCP_Pos                 (SCB_CFSR_USGFAULTSR_Pos + 3U)                  /*!< SCB CFSR (UFSR): NOCP Position */
#define SCB_CFSR_NOCP_Msk                 (1UL << SCB_CFSR_NOCP_Pos)                      /*!< SCB CFSR (UFSR): NOCP Mask */

#define SCB_CFSR_INVPC_Pos                (SCB_CFSR_USGFAULTSR_Pos + 2U)                  /*!< SCB CFSR (UFSR): INVPC Position */
#define SCB_CFSR_INVPC_Msk                (1UL << SCB_CFSR_INVPC_Pos)                     /*!< SCB CFSR (UFSR): INVPC Mask */

#define SCB_CFSR_INVSTATE_Pos             (SCB_CFSR_USGFAULTSR_Pos + 1U)                  /*!< SCB CFSR (UFSR): INVSTATE Position */
#define SCB_CFSR_INVSTATE_Msk             (1UL << SCB_CFSR_INVSTATE_Pos)                  /*!< SCB CFSR (UFSR): INVSTATE Mask */

#define SCB_CFSR_UNDEFINSTR_Pos           (SCB_CFSR_USGFAULTSR_Pos + 0U)                  /*!< SCB CFSR (UFSR): UNDEFINSTR Position */
#define SCB_CFSR_UNDEFINSTR_Msk           (1UL << SCB_CFSR_UNDEFINSTR_Pos)                /*!< SCB CFSR (UFSR): UNDEFINSTR Mask */

#define xPSR_ISR_Pos                        0U                                            /*!< xPSR: ISR Position */
#define xPSR_ISR_Msk                       (0x1FFUL /*<< xPSR_ISR_Pos*/)                  /*!< xPSR: ISR Mask */
#endif
#endif

/*****************************************************************************
 *                    Init
 *****************************************************************************/
static void BusFault_Dump(UInt32 cfsr)
{
    if (cfsr & SCB_CFSR_IBUSERR_Msk)
    {
        GP_LOG_SYSTEM_PRINTF("instruction bus error.",0);
    }
    if (cfsr & SCB_CFSR_PRECISERR_Msk)
    {
        GP_LOG_SYSTEM_PRINTF("precise error, see stacked PC.",0);
    }
    if (cfsr & SCB_CFSR_IMPRECISERR_Msk)
    {
        GP_LOG_SYSTEM_PRINTF("imprecise error.",0);
    }
    if (cfsr & SCB_CFSR_UNSTKERR_Msk)
    {
        GP_LOG_SYSTEM_PRINTF("unstacking error.",0);
    }
    if (cfsr & SCB_CFSR_STKERR_Msk)
    {
        GP_LOG_SYSTEM_PRINTF("stacking error.",0);
    }
    if (cfsr & SCB_CFSR_BFARVALID_Msk)
    {
        GP_LOG_SYSTEM_PRINTF("address BF: %lx",0, (unsigned long int)SCB->BFAR);
    }
}

static void UsageFault_Dump(UInt32 cfsr)
{
    if (cfsr & SCB_CFSR_DIVBYZERO_Msk)
    {
        GP_LOG_SYSTEM_PRINTF("divide by zero.",0);
    }
    if (cfsr & SCB_CFSR_UNALIGNED_Msk)
    {
        GP_LOG_SYSTEM_PRINTF("unaligned access",0);
    }
    if (cfsr & SCB_CFSR_NOCP_Msk)
    {
        GP_LOG_SYSTEM_PRINTF("no coprocessor",0);
    }
    if (cfsr & SCB_CFSR_INVPC_Msk)
    {
        GP_LOG_SYSTEM_PRINTF("invalid pc/return address",0);
    }
    if (cfsr & SCB_CFSR_INVSTATE_Msk)
    {
        GP_LOG_SYSTEM_PRINTF("invalid state",0);
    }
    if (cfsr & SCB_CFSR_UNDEFINSTR_Msk)
    {
        GP_LOG_SYSTEM_PRINTF("undefined instruction",0);
    }
}

void Log_Unexpected_Fault(UInt32 msp, UInt32 regp)
{
    UInt32* sp   = (UInt32*) msp;
#if defined (GP_DIVERSITY_LOG)
    UInt32* regs = (UInt32*) regp;
#endif
    UInt32 xPsr  = __get_xPSR();
    UInt32 cfsr  = SCB->CFSR;
    UInt32 shcsr = SCB->SHCSR;
    UInt32 ccr   = SCB->CCR;
    UInt32 exceptionNr = xPsr & xPSR_ISR_Msk;
    UIntLoop i;
#if defined(__GNUC__) && !defined(__SEGGER_LINKER)
    extern UInt32 _estack;
    UInt32* estack = &_estack;
#elif defined(__IAR_SYSTEMS_ICC__) || defined(__SEGGER_LINKER)
    /*
     *  estack should be initialized with a linker defined value.
     *  Currently IAR and SEGGER do not export such a symbol, so use fixe value for now.
     */
    UInt32* estack = (UInt32*)(GP_MM_UCRAM_END);
#endif


    GP_LOG_SYSTEM_PRINTF("======= Unexpected fault =======",0);
    GP_LOG_SYSTEM_PRINTF("Exception Number = 0x%lx", 0, exceptionNr);
    GP_LOG_SYSTEM_PRINTF("SHCSR: 0x%lx", 0, (unsigned long int)shcsr);
    GP_LOG_SYSTEM_PRINTF("CCR  : 0x%lx", 0, (unsigned long int)ccr);
    gpLog_Flush();

    if (exceptionNr == 3)
    {
        // Hard fault handler
        UInt32 hfsr = SCB->HFSR;

        GP_LOG_SYSTEM_PRINTF("Hard fault handler: HFSR = 0x%lx",0,(unsigned long int)hfsr);
        if (hfsr & SCB_HFSR_FORCED_Msk)
        {
            // Forced hard fault (by absence of fault handler, or priority too low)
            GP_LOG_SYSTEM_PRINTF("Forced hard fault. CFSR = 0x%lx", 0, (unsigned long int)cfsr);

            if (cfsr & SCB_CFSR_MEMFAULTSR_Msk)
            {
                GP_LOG_SYSTEM_PRINTF("MemManage Fault",0);

                if (cfsr & SCB_CFSR_MMARVALID_Msk)
                {
                    GP_LOG_SYSTEM_PRINTF("address MM: %lx",0, (unsigned long int)SCB->MMFAR);
                }
            }
            else if (cfsr & SCB_CFSR_BUSFAULTSR_Msk)
            {
                GP_LOG_SYSTEM_PRINTF("Bus Fault",0);
                BusFault_Dump(cfsr);
            }
            else if (cfsr & SCB_CFSR_USGFAULTSR_Msk)
            {
                GP_LOG_SYSTEM_PRINTF("Usage Fault",0);
                UsageFault_Dump(cfsr);
            }
            else
            {
                GP_LOG_SYSTEM_PRINTF("Unknown fault!",0);
            }
            gpLog_Flush();
        }
    }
    else if (exceptionNr == 4)
    {
        // Add code for memmanage faul
        GP_LOG_SYSTEM_PRINTF("MemManage Fault",0);
    }
    else if(exceptionNr == 5)
    {
        BusFault_Dump(cfsr);
    }
    else if(exceptionNr == 6)
    {
        UsageFault_Dump(cfsr);
    }

    GP_LOG_SYSTEM_PRINTF("Stack pointer: 0x%lx", 0, (unsigned long int)msp);
    GP_LOG_SYSTEM_PRINTF("Registers:",0);
    for (i = 0; i < 4; ++i)
        GP_LOG_SYSTEM_PRINTF("R%i: 0x%lx", 0, i, (unsigned long int) sp[i]);
    gpLog_Flush();
    for (i = 0; i < 8; ++i)
        GP_LOG_SYSTEM_PRINTF("R%i: 0x%lx", 0, i + 4, (unsigned long int) regs[i]);
    gpLog_Flush();
    GP_LOG_SYSTEM_PRINTF("R12: 0x%lx", 0, (unsigned long int)sp[4]);
    GP_LOG_SYSTEM_PRINTF("LR: 0x%lx",  0, (unsigned long int)sp[5]);
    GP_LOG_SYSTEM_PRINTF("PC: 0x%lx",  0, (unsigned long int)sp[6]);
    GP_LOG_SYSTEM_PRINTF("PSR: 0x%lx", 0, (unsigned long int)sp[7]);
    GP_LOG_SYSTEM_PRINTF("======= End =======",0);
    gpLog_Flush();

#if defined(__GNUC__) || defined(__IAR_SYSTEMS_ICC__)
    GP_LOG_SYSTEM_PRINTF("Stack content (assuming full descending stack orientation)", 0);
    GP_LOG_SYSTEM_PRINTF("sp: 0x%lx, estack: 0x%lx", 0, (unsigned long int)sp, (unsigned long int)estack);
    for (UInt32* sf = sp; sf < estack;  sf++)
    {
        GP_LOG_SYSTEM_PRINTF("Address: 0x%lx, Value: 0x%lx", 0, (unsigned long int)sf, *sf);
        gpLog_Flush();
    }
#endif

    GP_LOG_SYSTEM_PRINTF("Interrupt status",0);
    for (i = 0; i <= 30; i++)
    {
        GP_LOG_SYSTEM_PRINTF("Int: %i en: %x, act: %lx pen: %lx pri: %lx", 0,
                i,
                (NVIC->ISER[0] & (1 << i)) > 0,
                (unsigned long int)NVIC_GetActive((IRQn_Type) i),
                (unsigned long int)NVIC_GetPendingIRQ((IRQn_Type) i),
                (unsigned long int)NVIC_GetPriority((IRQn_Type) i)
            );
        gpLog_Flush();
    }
}

void Log_Unexpected_Interrupt(void)
{
    UInt32 psr=__get_xPSR();
#if defined (GP_DIVERSITY_LOG)
    UInt16 exceptionNr = psr & xPSR_ISR_Msk;
#endif
    // Device specific interrupts start at 16
    GP_LOG_SYSTEM_PRINTF("Unexpected GP int: %i", 0, exceptionNr - 16);
    gpLog_Flush();
}
