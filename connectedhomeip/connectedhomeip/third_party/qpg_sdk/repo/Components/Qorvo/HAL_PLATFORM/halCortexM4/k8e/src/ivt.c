/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * Interrupt vector table for Cortex M4
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
#include "gpAssert.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_HALCORTEXM4

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

#if defined(__GNUC__)
#define IVT_GET_STACK_POINTER()     (&_estack)
#endif
#if defined(__IAR_SYSTEMS_ICC__)
#define IVT_GET_STACK_POINTER()     (__sfe( "CSTACK" ))
#endif

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

#if defined(__GNUC__)
extern unsigned long _estack;
#endif

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static void unexpected_int(void);

// This should be done after declaration of the aliased functions
#include "dev_alias.h"

/*****************************************************************************
 *                    External Function Prototypes
 *****************************************************************************/

// Declaration of ISR handlers
#include "dev_handlers.h"

/*****************************************************************************
 *                    Interrupt Vector Table
 *****************************************************************************/

// segment declaration needed in case of IAR
#if defined(__IAR_SYSTEMS_ICC__)
#pragma language=extended
#pragma segment="CSTACK"
#endif

// Define function NAME_handler() which takes care of the interrupt prologue
// and epilogue and calls function NAME_handler_impl() to do the actual
// interrupt handling.
//
// Functions NAME_handler_impl() are weak symbols which may be replaced
// in components or applications.

// When using IAR compiler, we can add the #pragma call_graph_root to get
// stack usage analysis for interrupts
#if defined(__IAR_SYSTEMS_ICC__)
#define CALL_GRAPH_ROOT _Pragma ("call_graph_root=\"interrupt_handlers\"")
#else
#define CALL_GRAPH_ROOT
#endif

#define HAL_DEFINE_INTERRUPT_WRAPPER(rawfunc, implfunc)             \
CALL_GRAPH_ROOT                                                     \
INTERRUPT_H void rawfunc (void)                                     \
{                                                                   \
    hal_IntHandlerPrologue();                                       \
    implfunc ();                                                    \
    hal_IntHandlerEpilogue();                                       \
}

HAL_DEFINE_INTERRUPT_WRAPPER(pendsv_handler,    pendsv_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(systick_handler,   systick_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(adcif_handler,     adcif_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(asp_handler,       asp_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(bbpll_handler,     bbpll_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(dmas_handler,      dmas_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(es_handler,        es_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(gpio_handler,      gpio_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(gpmicro_handler,   gpmicro_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(i2cm_handler,      i2cm_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(i2csl_handler,     i2csl_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(i2sm_handler,      i2sm_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(ipcext2x_handler,  ipcext2x_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(ipcgpm2x_handler,  ipcgpm2x_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(ipcx2int_handler,  ipcx2int_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(ir_handler,        ir_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(keypad_handler,    keypad_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(mri_handler,       mri_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(parble_handler,    parble_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(parfcs_handler,    parfcs_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(phy_handler,       phy_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(pwms_handler,      pwms_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(qta_handler,       qta_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(rci_handler,       rci_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(rpa_handler,       rpa_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(secproc_handler,   secproc_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(spim_handler,      spim_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(spisl_handler,     spisl_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(ssp_handler,       ssp_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(stbc_handler,      stbc_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(timer_handler,     timer_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(trc_handler,       trc_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(uart0_handler,     uart0_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(uart1_handler,     uart1_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(uart2_handler,     uart2_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(watchdog_handler,  watchdog_handler_impl)


/*****************************************************************************
 *                    Additional sections
 *****************************************************************************/

/*
 * Default alias functions for weak and undefined isr functions.
 */
#ifdef GP_DIVERSITY_LOG
extern void Log_Unexpected_Interrupt(void);
extern void Log_Unexpected_Fault(UInt32 sp, UInt32 regp);
#endif //GP_DIVERSITY_LOG

void unexpected_fault_validsp(UInt32 sp, UInt32 regp)
{
#ifdef GP_DIVERSITY_LOG
    Log_Unexpected_Fault(sp, regp);
#endif
    /* never ignore - perhaps move to explicit SW reset? */
    GP_ASSERT_SYSTEM(false);
}

void unexpected_int(void)
{
#ifdef GP_DIVERSITY_LOG
    Log_Unexpected_Interrupt();
#endif
    /* ignore */
    GP_ASSERT_DEV_INT(false);
}
