/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <inttypes.h>
#include "tfm_hal_device_header.h"
#include "secure_utilities.h"
#include "tfm_arch.h"
#include "tfm_secure_api.h"
#include "spm_api.h"
#include "core/tfm_core_svc.h"

#if !defined(__ARM_ARCH_8M_BASE__)
#error "Unsupported ARM Architecture."
#endif

#ifdef TFM_PSA_API
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
 * Stack status before calling pendsv_do_schedule():
 *
 *                       MSP->[ R4 - R11 ]
 *                            [ PSP      ]--->[ R0 - R3  ]
 *                            [ PSP Limit]    [ R12      ]
 *                            [ R2(dummy)]    [ LR_of_RA ]
 *                            [ LR       ]    [ RA       ]
 *                            [ ........ ]    [ XPSR     ]
 *                            [ ........ ]    [ ........ ]
 *                                            [ ........ ]
 *
 * tfm_pendsv_do_schedule() updates stacked context into current thread and
 * replace stacked context with context of next thread.
 *
 * Scheduler does not support handler mode thread so take PSP/PSP_LIMIT as
 * thread SP/SP_LIMIT. R2 holds dummy data due to stack operation is 8 bytes
 * aligned.
 */
#if defined(__ICCARM__)
#pragma required = tfm_pendsv_do_schedule
#endif

__attribute__((naked)) void PendSV_Handler(void)
{
    __ASM volatile(
        "mrs     r0, psp                    \n"
        "mrs     r1, psplim                 \n"
        "push    {r0, r1, r2, lr}           \n"
        "push    {r4-r7}                    \n"
        "mov     r4, r8                     \n"
        "mov     r5, r9                     \n"
        "mov     r6, r10                    \n"
        "mov     r7, r11                    \n"
        "push    {r4-r7}                    \n"
        "mov     r0, sp                     \n"
        "bl      tfm_pendsv_do_schedule     \n"
        "pop     {r4-r7}                    \n"
        "mov     r8, r4                     \n"
        "mov     r9, r5                     \n"
        "mov     r10, r6                    \n"
        "mov     r11, r7                    \n"
        "pop     {r4-r7}                    \n"
        "pop     {r0-r3}                    \n"
        "mov     lr, r3                     \n"
        "msr     psp, r0                    \n"
        "msr     psplim, r1                 \n"
        "bx      lr                         \n"
    );
}

void tfm_arch_init_actx(struct tfm_arch_ctx_t *p_actx,
                        uint32_t sp, uint32_t sp_limit)
{
    p_actx->sp = sp;
    p_actx->sp_limit = sp_limit;
    p_actx->lr = EXC_RETURN_THREAD_S_PSP;
}
#else
__attribute__((section("SFN"), naked))
int32_t tfm_core_sfn_request(const struct tfm_sfn_req_s *desc_ptr)
{
    __ASM volatile(
        "PUSH   {lr}                        \n"
        "PUSH   {r4-r7}                     \n"
        "MOV    r4, r8                      \n"
        "MOV    r5, r9                      \n"
        "MOV    r6, r10                     \n"
        "MOV    r7, r11                     \n"
        "PUSH   {r4-r7}                     \n"
        "MOV    r4, r12                     \n"
        "PUSH   {r4}                        \n"
        "SVC    %[SVC_REQ]                  \n"
        "MOVS   r4, #0                      \n"
        "MOV    r5, r4                      \n"
        "MOV    r6, r4                      \n"
        "MOV    r7, r4                      \n"
        "MOV    r8, r4                      \n"
        "MOV    r9, r4                      \n"
        "MOV    r10, r4                     \n"
        "MOV    r11, r4                     \n"
        "BLX    lr                          \n"
        "SVC    %[SVC_RET]                  \n"
        "POP    {r4}                        \n"
        "MOV    r12, r4                     \n"
        "POP    {r4-r7}                     \n"
        "MOV    r8, r4                      \n"
        "MOV    r9, r5                      \n"
        "MOV    r10, r6                     \n"
        "MOV    r11, r7                     \n"
        "POP    {r4-r7}                     \n"
        "POP    {pc}                        \n"
        : : [SVC_REQ] "I" (TFM_SVC_SFN_REQUEST),
            [SVC_RET] "I" (TFM_SVC_SFN_RETURN)
        );
}

__attribute__((section("SFN"), naked))
void priv_irq_handler_main(uint32_t partition_id,
                                                  uint32_t unpriv_handler,
                                                  uint32_t irq_signal,
                                                  uint32_t irq_line)
{
    __ASM(
          /* Save the callee saved registers*/
          "PUSH   {r4-r7, lr}               \n"
          "MOV    r4, r8                    \n"
          "MOV    r5, r9                    \n"
          "MOV    r6, r10                   \n"
          "MOV    r7, r11                   \n"
          "PUSH   {r4-r7}                   \n"
          "MOV    r4, r12                   \n"
          "PUSH   {r4}                      \n"
          /* Request SVC to configure environment for the unpriv IRQ handler */
          "SVC    %[SVC_REQ]                \n"
          /* clear the callee saved registers to prevent information leak */
          "MOVS   r4, #0                    \n"
          "MOV    r5, r4                    \n"
          "MOV    r6, r4                    \n"
          "MOV    r7, r4                    \n"
          "MOV    r8, r4                    \n"
          "MOV    r9, r4                    \n"
          "MOV    r10, r4                   \n"
          "MOV    r11, r4                   \n"
          /* Branch to the unprivileged handler */
          "BLX    lr                        \n"
          /* Request SVC to reconfigure the environment of the interrupted
           * partition
           */
          "SVC    %[SVC_RET]                \n"
          /* restore callee saved registers and return */
          "POP    {r4}                      \n"
          "MOV    r12, r4                   \n"
          "POP    {r4-r7}                   \n"
          "MOV    r8, r4                    \n"
          "MOV    r9, r5                    \n"
          "MOV    r10, r6                   \n"
          "MOV    r11, r7                   \n"
          "POP   {r4-r7, pc}                \n"
          : : [SVC_REQ] "I" (TFM_SVC_DEPRIV_REQ)
          , [SVC_RET] "I" (TFM_SVC_DEPRIV_RET)
          );
}
#endif

/**
 * \brief Overwrites default Hard fault handler.
 *
 * In case of a baseline implementation fault conditions that would generate a
 * SecureFault in a mainline implementation instead generate a Secure HardFault.
 */
void HardFault_Handler(void)
{
    /* In a baseline implementation there is no way, to find out whether this is
     * a hard fault triggered directly, or another fault that has been
     * escalated.
     */
    while (1) {
        ;
    }
}

#if defined(__ICCARM__)
uint32_t tfm_core_svc_handler(uint32_t *svc_args, uint32_t exc_return);
#pragma required = tfm_core_svc_handler
#endif

__attribute__((naked)) void SVC_Handler(void)
{
    __ASM volatile(
    "MRS     r2, MSP                        \n"
    "MOVS    r1, #4                         \n"
    "MOV     r3, lr                         \n"
    "MOV     r0, r2                         \n"
    "TST     r1, r3                         \n"
    "BEQ     handler                        \n"
    /* If SVC was made from thread mode, overwrite r0 with PSP */
    "MRS     r0, PSP                        \n"
    "handler:                               \n"
    "MOV     r1, lr                         \n"
    "BL      tfm_core_svc_handler           \n"
    "BX      r0                             \n"
    );
}

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
    uint32_t VECTKEY;
    SCB_Type *scb = SCB;
    uint32_t AIRCR;

    /* Set PRIS flag in AIRCR */
    AIRCR = scb->AIRCR;
    VECTKEY = (~AIRCR & SCB_AIRCR_VECTKEYSTAT_Msk);
    scb->AIRCR = SCB_AIRCR_PRIS_Msk |
                 VECTKEY |
                 (AIRCR & ~SCB_AIRCR_VECTKEY_Msk);
}

/* There is no FPCA in baseline. */
void tfm_arch_clear_fp_status(void)
{
}
