/********************************************************************************************************
 * @file	core.h
 *
 * @brief	This is the header file for B91
 *
 * @author	Driver Group
 * @date	2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/
#ifndef CORE_H
#define CORE_H
#include "sys.h"

// #define NDS_MXSTATUS            0x0 //TBD
#define NDS_MEPC                0x341
#define NDS_MIE                 0x304
#define NDS_MILMB               0x7C0
#define NDS_MDLMB               0x7C1
#define NDS_MSTATUS             0x300

#define read_csr(var, csr)      __asm__ volatile ("csrr %0, %1" : "=r" (var) : "i" (csr))
#define write_csr(csr, val)     __asm__ volatile ("csrw %0, %1" :: "i" (csr), "r" (val))
#define set_csr(csr, bit)       __asm__ volatile ("csrs %0, %1" :: "i" (csr), "r" (bit))
#define clear_csr(csr, bit)     __asm__ volatile ("csrc %0, %1" :: "i" (csr), "r" (bit))

/*
 * Inline nested interrupt entry/exit macros
 */
/* Svae/Restore macro */
#define save_csr(r)             long __##r;          \
                                read_csr(__##r,r);
#define restore_csr(r)          write_csr(r, __##r);
/* Support PowerBrake (Performance Throttling) feature */


#define save_mxstatus()         save_csr(NDS_MXSTATUS)
#define restore_mxstatus()      restore_csr(NDS_MXSTATUS)

 /* Nested IRQ entry macro : Save CSRs and enable global interrupt. */
#define core_save_nested_context()                              \
	 save_csr(NDS_MEPC)                              \
	 save_csr(NDS_MSTATUS)                           \
	 save_mxstatus()                                 \
	 set_csr(NDS_MSTATUS, 1<<3);

/* Nested IRQ exit macro : Restore CSRs */
#define core_restore_nested_context()                               \
	 clear_csr(NDS_MSTATUS, 1<<3);            \
	 restore_csr(NDS_MSTATUS)                        \
	 restore_csr(NDS_MEPC)                           \
	 restore_mxstatus()

typedef enum{
	FLD_FEATURE_PREEMPT_PRIORITY_INT_EN = BIT(0),
	FLD_FEATURE_VECTOR_MODE_EN 			= BIT(1),
}
feature_e;


/**
 * @brief Disable interrupts globally in the system.external, timer and software interrupts.
 * @return  r - the value of machine interrupt enable(MIE) register.
 * @note  this function must be used when the system wants to disable all the interrupt.
 */
static inline unsigned int core_interrupt_disable(void){

	unsigned int r;
	read_csr (r, NDS_MIE);
	clear_csr(NDS_MIE, BIT(3)| BIT(7)| BIT(11));
	return r;
}


/**
 * @brief restore interrupts globally in the system. external,timer and software interrupts.
 * @param[in]  en - the value of machine interrupt enable(MIE) register before disable.
 * @return     0
 * @note this function must be used when the system wants to restore all the interrupt.
 */
static inline unsigned int core_restore_interrupt(unsigned int en){

	set_csr(NDS_MIE, en);
	return 0;
}

/**
 * @brief enable interrupts globally in the system.
 * @return  none
 */
static inline void core_interrupt_enable(void)
{
	set_csr(NDS_MSTATUS,1<<3);
	set_csr(NDS_MIE,1<<11 | 1 << 7 | 1 << 3);

}
#endif
