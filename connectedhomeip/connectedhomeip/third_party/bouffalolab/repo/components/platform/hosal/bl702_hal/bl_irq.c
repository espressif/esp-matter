/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdint.h>
#include <stdio.h>

#include <bl702.h>

#include <clic.h>
#include <blog.h>
#include "bl_irq.h"
#include <panic.h>

#ifdef SYS_ENABLE_COREDUMP
#include <bl_coredump.h>
#endif

void bl_irq_enable(unsigned int source)
{
    *(volatile uint8_t*)(CLIC_HART0_ADDR + CLIC_INTIE + source) = 1;
}

void bl_irq_disable(unsigned int source)
{
    *(volatile uint8_t*)(CLIC_HART0_ADDR + CLIC_INTIE + source) = 0;
}

void bl_irq_pending_set(unsigned int source)
{
    *(volatile uint8_t*)(CLIC_HART0_ADDR + CLIC_INTIP + source) = 1;
}

void bl_irq_pending_clear(unsigned int source)
{
    *(volatile uint8_t*)(CLIC_HART0_ADDR + CLIC_INTIP + source) = 0;
}

void bl_irq_exception_trigger(BL_IRQ_EXCEPTION_TYPE_T type, void *ptr)
{
    uint32_t val = 0x12345678;;

    switch (type) {
        case BL_IRQ_EXCEPTION_TYPE_LOAD_MISALIGN:
        {
            val = *(uint32_t*)ptr;
        }
        break;
        case BL_IRQ_EXCEPTION_TYPE_STORE_MISALIGN:
        {
            *(uint32_t*)ptr = val;
        }
        break;
        case BL_IRQ_EXCEPTION_TYPE_ACCESS_ILLEGAL:
        {
            *(uint32_t*)ptr = val;
        }
        break;
        case BL_IRQ_EXCEPTION_TYPE_ILLEGAL_INSTRUCTION:
        {
            uint32_t fun_val = 0;
            typedef void (*ins_ptr_t)(void);
            ins_ptr_t func = (ins_ptr_t)&fun_val;

            func();
        }
        break;
        default:
        {
            /*nothing here*/
        }
    }
    printf("Trigger exception val is %08lx\r\n", val);
}

void bl_irq_default(void)
{
    while (1) {
        /*dead loop*/
    }
}

void (*handler_list[2][16 + 64])(void) = {
    
};


static inline void _irq_num_check(int irqnum)
{
    if (irqnum < 0 || irqnum >= sizeof(handler_list[0])/sizeof(handler_list[0][0])) {
        blog_error("illegal irqnum %d\r\n", irqnum);
        while (1) {
            /*Deap loop here, TODO ass blog_assert*/
        }
    }
}

void bl_irq_register_with_ctx(int irqnum, void *handler, void *ctx)
{
    _irq_num_check(irqnum);
    if (handler_list[0][irqnum] && handler_list[0][irqnum] != handler) {
        blog_warn("IRQ %d already registered with %p \r\n",
             irqnum,
             handler_list[0][irqnum]
        );
    }
   
    if (handler == NULL) {
        blog_error("handler is NULL pointer! \r\n");
        return;
    }

    if (NULL == ctx) {
        handler_list[0][irqnum] = handler;
        handler_list[1][irqnum] = NULL;
    }
    else {
        handler_list[0][irqnum] = handler;
        handler_list[1][irqnum] = ctx;
    }

    return;
    
}

void bl_irq_ctx_get(int irqnum, void **ctx)
{
    _irq_num_check(irqnum);
    *ctx = handler_list[1][irqnum];

    return;
}

void bl_irq_handler_get(int irqnum, void **handler)
{
    _irq_num_check(irqnum);
    *handler = handler_list[0][irqnum];

    return;
}

void bl_irq_register(int irqnum, void *handler)
{
    bl_irq_register_with_ctx(irqnum, handler, NULL);
}

void bl_irq_unregister(int irqnum, void *handler)
{
    _irq_num_check(irqnum);
    if (handler_list[0][irqnum] != handler) {
    }
    handler_list[0][irqnum] = NULL;
}

void interrupt_entry(uint32_t mcause) 
{
    void *handler = NULL;
    mcause &= 0x7FFFFFF;
    if (mcause < sizeof(handler_list[0])/sizeof(handler_list[0][0])) {
        handler = handler_list[0][mcause];
    }
    if (handler) {
        extern int32_t TrapNetCounter;
        if(TrapNetCounter > 1){
            printf("Interrupt nested!\r\n");
            while (1) {
                /*dead loop now*/
            }
        }
        if (handler_list[1][mcause]) {
           ((void (*)(void *))handler)(handler_list[1][mcause]);//handler(ctx)
        }
        else {
            ((void (*)(void))handler)();
        }
    } else {
        printf("Cannot handle mcause 0x%lx:%lu, adjust to externel(0x%lx:%lu)\r\n",
                mcause,
                mcause,
                mcause - 16,
                mcause - 16
        );
        while (1) {
            /*dead loop now*/
        }
    }
}

static void __dump_exception_code_str(uint32_t code)
{
    printf("Exception code: %lu\r\n", code);
    switch (code) {
        case 0x00:
        /*Instruction address misaligned*/
        {
            puts("  msg: Instruction address misaligned\r\n");
        }
        break;
        case 0x01:
        /*Instruction access fault*/
        {
            puts("  msg: Instruction access fault\r\n");
        }
        break;
        case 0x02:
        /*Illegal instruction*/
        {
            puts("  msg: Illegal instruction\r\n");
        }
        break;
        case 0x03:
        /*Breakpoint*/
        {
            puts("  msg: Breakpoint\r\n");
        }
        break;
        case 0x04:
        /*Load address misaligned*/
        {
            puts("  msg: Load address misaligned\r\n");
        }
        break;
        case 0x05:
        /*Load access fault*/
        {
            puts("  msg: Load access fault\r\n");
        }
        break;
        case 0x06:
        /*Store/AMO access misaligned*/
        {
            puts("  msg: Store/AMO access misaligned\r\n");
        }
        break;
        case 0x07:
        /*Store/AMO access fault*/
        {
            puts("  msg: Store/AMO access fault\r\n");
        }
        break;
        case 0x08:
        /*Environment call from U-mode*/
        {
            puts("  msg: Environment call from U-mode\r\n");
        }
        break;
        case 0x09:
        /*Environment call from S-mode*/
        {
            puts("  msg: Environment call from S-mode\r\n");
        }
        break;
        case 0x0a:
        case 0x0e:
        /*Reserved*/
        {
            puts("  msg: Reserved\r\n");
        }
        break;
        case 0x0b:
        /*Environment call from M-mode*/
        {
            puts("  msg: Environment call from M-mode\r\n");
        }
        break;
        case 0x0c:
        /*Instruction page fault*/
        {
            puts("  msg: Instruction page fault\r\n");
        }
        break;
        case 0x0d:
        /*Load page fault*/
        {
            puts("  msg: Load page fault\r\n");
        }
        break;
        case 0x0f:
        /*Store/AMO page fault*/
        {
            puts("  msg: Store/AMO page fault\r\n");
        }
        break;
        default:{
            puts("  msg: Reserved default exception\r\n");
        }
    }
}

extern void misaligned_load_trap(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc);
extern void misaligned_store_trap(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc);

#define EXCPT_LOAD_MISALIGNED        4
#define EXCPT_STORE_MISALIGNED       6

#define DBG_RECORD_EXCEP_VAL 1

#ifdef DBG_RECORD_EXCEP_VAL
struct{
	uint32_t mcause;
	uint32_t mepc;
	uint32_t mtval;
}rval[4];
int rval_idx;
#endif /* DBG_RECORD_EXCEP_VAL */

static void registerdump(unsigned int *regs)
{
#define REG_RA              1
#define REG_SP              REG_X2
#define REG_GP              REG_X3
#define REG_TP              REG_X4
#define REG_T0              2
#define REG_T1              3
#define REG_T2              4
#define REG_S0              5
#define REG_FP              5
#define REG_S1              6
#define REG_A0              7
#define REG_A1              8
#define REG_A2              9
#define REG_A3              10
#define REG_A4              11
#define REG_A5              13
#define REG_A6              13
#define REG_A7              14
#define REG_S2              15
#define REG_S3              16
#define REG_S4              17
#define REG_S5              18
#define REG_S6              19
#define REG_S7              20
#define REG_S8              21
#define REG_S9              22
#define REG_S10             23
#define REG_S11             24
#define REG_T3              25
#define REG_T4              26
#define REG_T5              27
#define REG_T6              28
#define REG_MSTATUS         29

  printf("Current task sp data:\r\n");

  printf("RA:%08x, mstatus:%08x\r\n",
          regs[REG_RA], regs[REG_MSTATUS]);

  printf("A0:%08x A1:%08x A2:%08x A3:%08x A4:%08x A5:%08x "
          "A6:%08x A7:%08x\r\n",
          regs[REG_A0], regs[REG_A1], regs[REG_A2], regs[REG_A3],
          regs[REG_A4], regs[REG_A5], regs[REG_A6], regs[REG_A7]);
  printf("T0:%08x T1:%08x T2:%08x T3:%08x T4:%08x T5:%08x T6:%08x\r\n",
          regs[REG_T0], regs[REG_T1], regs[REG_T2], regs[REG_T3],
          regs[REG_T4], regs[REG_T5], regs[REG_T6]);
  printf("S0:%08x S1:%08x S2:%08x S3:%08x S4:%08x S5:%08x "
          "S6:%08x S7:%08x\r\n",
          regs[REG_S0], regs[REG_S1], regs[REG_S2], regs[REG_S3],
          regs[REG_S4], regs[REG_S5], regs[REG_S6], regs[REG_S7]);
  printf("S8:%08x S9:%08x S10:%08x S11:%08x\r\n",
          regs[REG_S8], regs[REG_S9], regs[REG_S10], regs[REG_S11]);
}

void exception_entry(uint32_t mcause, uint32_t mepc, uint32_t mtval, uintptr_t *regs, uintptr_t *tasksp)
{
#ifdef DBG_RECORD_EXCEP_VAL
	rval[rval_idx&0x3].mcause = mcause;
	rval[rval_idx&0x3].mepc = mepc;
	rval[rval_idx&0x3].mtval = mtval;
	rval_idx++;
#endif /* DBG_RECORD_EXCEP_VAL */
	puts("Exception Entry--->>>\r\n");
	printf("mcause %08lx, mepc %08lx, mtval %08lx\r\n",
		mcause,
		mepc,
		mtval
	);
    if ((mcause & 0x3ff) == EXCPT_LOAD_MISALIGNED){
        misaligned_load_trap(regs, mcause, mepc);
    } else if ((mcause & 0x3ff) == EXCPT_STORE_MISALIGNED){
        misaligned_store_trap(regs, mcause, mepc);
    } else {
        extern int bl_sys_wdt_rst_count_get();
        printf("wdt reset count %d\r\n", bl_sys_wdt_rst_count_get());

        registerdump(tasksp);
        __dump_exception_code_str(mcause & 0xFFFF);
        backtrace_now((int (*)(const char *fmt, ...))printf, regs);
        while (1) {
            /*Deap loop now*/
#ifdef SYS_ENABLE_COREDUMP
            /* For stack check */
            extern uintptr_t _sp_main;
            /* XXX change sp to irq stack base */
            __asm__ volatile("add sp, x0, %0" ::"r"(&_sp_main));
            bl_coredump_run();
#endif
        }
    }
}

void bl_irq_init(void)
{
    uint32_t ptr;

    puts("[IRQ] Clearing and Disable all the pending IRQ...\r\n");

    /*clear mask*/
    for (ptr = 0x02800400; ptr < 0x02800400 + 128; ptr++) {
        *(uint8_t*)ptr = 0;
    }
    /*clear pending*/
    for (ptr = 0x02800000; ptr < 0x02800000 + 128; ptr++) {
        *(uint8_t*)ptr = 0;
    }
}

int bl_irq_save(void)
{
    uint32_t oldstat;

    /* Read mstatus & clear machine interrupt enable (MIE) in mstatus */
    __asm volatile("csrrc %0, mstatus, %1" : "=r"(oldstat) : "r"(8));
    return oldstat;
}

void bl_irq_restore(int flags)
{
    __asm volatile("csrw mstatus, %0"
                    : /* no output */
                    : "r"(flags));
}
