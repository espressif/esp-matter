/*
 * Copyright (c) 2015, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
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

#ifndef _DEFAULT_HANDLERS_H_
#define _DEFAULT_HANDLERS_H_

// Functions XXX_handler() are directly called via the interrupt vector table.
// These functions must be declared INTERRUPT_H and are responsible for calling
// hal_IntHandlerPrologue() / hal_IntHandlerEpilogue() from ther implementation.
// See JIRA ticket SDB000-6 for the importance of hal_IntHandlerEpilogue().
//
// Functions XXX_handler_impl() are called via a wrapper function which takes
// care of the interrupt context, prologue and epilogue. The implementation of
// XXX_handler_impl() should not call hal_IntHandlerPrologue / hal_IntHandlerEpilogue.

// Declarations for the standard Cortex handlers
extern WEAK INTERRUPT_H void reset_handler(void);
extern WEAK INTERRUPT_H void backup_handler(void);
extern WEAK INTERRUPT_H void hardfault_handler(void);
extern WEAK INTERRUPT_H void memmanage_handler(void);
extern WEAK INTERRUPT_H void busfault_handler(void);
extern WEAK INTERRUPT_H void usagefault_handler(void);
extern WEAK INTERRUPT_H void svcall_handler(void);
extern WEAK INTERRUPT_H void debugmon_handler(void);
extern WEAK void pendsv_handler_impl(void);
extern WEAK void systick_handler_impl(void);

// Include the chip specific handlers
#include "dev_handlers.h"

#endif //_DEFAULT_HANDLERS_H_

