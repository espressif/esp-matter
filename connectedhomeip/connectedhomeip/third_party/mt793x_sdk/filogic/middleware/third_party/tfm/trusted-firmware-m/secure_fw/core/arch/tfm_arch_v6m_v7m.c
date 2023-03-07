/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <inttypes.h>
#include "tfm_hal_device_header.h"
#include "tfm_arch.h"

#if !defined(__ARM_ARCH_6M__) && !defined(__ARM_ARCH_7M__) && \
    !defined(__ARM_ARCH_7EM__)
#error "Unsupported ARM Architecture."
#endif

extern uint32_t SVCHandler_main(uint32_t *svc_args, uint32_t lr);

/*
 * Stack status at PendSV entry:
 *
 *                                            [ R0 - R3  ]<- PSP
 *                                            [ R12      ]
 *                                            [ LR_of_RA ]
 *                       MSP->[ ........ ]    [ RA       ]
 *                            [ ........ ]    [ XPSR     ]
 *                                            [ ........ ]
 *                                            [ ........ ]
 *
 * Stack status before calling tfm_pendsv_do_schedule():
 *
 *                       MSP->[ R8 - R9  ]
 *                            [ R4 - R7  ]
 *                            [ PSP      ]--->[ R0 - R3  ]
 *                            [ LR       ]    [ R12      ]
 *                            [ ........ ]    [ LR_of_RA ]
 *                            [ ........ ]    [ RA       ]
 *                                            [ XPSR     ]
 *                                            [ ........ ]
 *                                            [ ........ ]
 *
 * tfm_pendsv_do_schedule() updates stacked context into current thread and
 * replace stacked context with context of next thread.
 *
 * Scheduler does not support handler mode thread so take PSP as thread SP.
 */
#if defined(__ICCARM__)
extern void tfm_pendsv_do_schedule(void);
#pragma required = tfm_pendsv_do_schedule
#endif

__attribute__((naked)) void PendSV_Handler(void)
{
    __ASM volatile(
        "MRS     r0, psp                    \n"
        "PUSH    {r0, lr}                   \n"
        "PUSH    {r4-r7}                    \n"
        "MOV     r4, r8                     \n"
        "MOV     r5, r9                     \n"
        "MOV     r6, r10                    \n"
        "MOV     r7, r11                    \n"
        "PUSH    {r4-r7}                    \n"
        "MOV     r0, sp                     \n"
        "BL      tfm_pendsv_do_schedule     \n"
        "POP     {r4-r7}                    \n"
        "MOV     r8, r4                     \n"
        "MOV     r9, r5                     \n"
        "MOV     r10, r6                    \n"
        "MOV     r11, r7                    \n"
        "POP     {r4-r7}                    \n"
        "POP     {r0, r1}                   \n"
        "MOV     lr, r1                     \n"
        "MSR     psp, r0                    \n"
        "BX      lr                         \n"
    );
}

void tfm_arch_init_actx(struct tfm_arch_ctx_t *p_actx,
                        uint32_t sp, uint32_t sp_limit)
{
    (void)sp_limit;

    p_actx->sp = sp;
    p_actx->lr = EXC_RETURN_THREAD_S_PSP;
}

#if defined(__ICCARM__)
uint32_t tfm_core_svc_handler(uint32_t *svc_args, uint32_t exc_return);
#pragma required = tfm_core_svc_handler
#endif

__attribute__((naked)) void SVC_Handler(void)
{
    __ASM volatile(
    "MOVS    r0, #4                \n" /* Check store SP in thread mode to r0 */
    "MOV     r1, lr                \n"
    "TST     r0, r1                \n"
    "BEQ     handler               \n"
    "MRS     r0, PSP               \n" /* Coming from thread mode */
    "B       sp_stored             \n"
    "handler:                      \n"
    "BX      lr                    \n" /* Coming from handler mode */
    "sp_stored:                    \n"
    "MOV     r1, lr                \n"
    "BL      tfm_core_svc_handler  \n"
    "BX      r0                    \n"
    );
}

/**
 * \brief Overwrites default Hard fault handler.
 */
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
void HardFault_Handler(void)
{
    /* HFSR can be read to provide further information of cause of HardFault */
     __ASM volatile("b    .");
}
#elif defined(__ARM_ARCH_6M__)
void HardFault_Handler(void)
{
    /* In a baseline implementation there is no way, to find out whether this is
     * a hard fault triggered directly, or another fault that has been
     * escalated.
     */
     __ASM volatile("b    .");
}
#endif

/* Reserved for future usage */
__attribute__((naked)) void MemManage_Handler(void)
{
    __ASM volatile("b    .");
}

__attribute__((naked)) void BusFault_Handler(void)
{
    __ASM volatile("b    .");
}

__attribute__((naked)) void UsageFault_Handler(void)
{
    __ASM volatile("b    .");
}

void tfm_arch_prioritize_secure_exception(void)
{
}

/* There is no FPCA in v6m */
#ifndef __ARM_ARCH_6M__
__attribute__((naked, noinline)) void tfm_arch_clear_fp_status(void)
{
    __ASM volatile(
                   ".syntax unified          \n"
                   "mrs  r0, control         \n"
                   "bics r0, r0, #4          \n"
                   "msr  control, r0         \n"
                   "isb                      \n"
                   "bx   lr                  \n"
                  );
}
#else
void tfm_arch_clear_fp_status(void)
{
}
#endif
