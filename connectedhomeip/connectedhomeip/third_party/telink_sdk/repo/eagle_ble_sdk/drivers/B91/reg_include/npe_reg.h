/********************************************************************************************************
 * @file	npe_reg.h
 *
 * @brief	This is the header file for B91
 *
 * @author	Driver Group
 * @date	2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#ifndef NPE_REG_H
#define NPE_REG_H
#include "../sys.h"
#define  NPE_BASE_ADDR			         0x112000
#define reg_npe_addr(addr)		((addr)-CPU_DLM_BASE+DLM_BASE)
#define reg_npe_0x40			REG_ADDR32(NPE_BASE_ADDR+0x40)
#define reg_npe_0x6c			REG_ADDR32(NPE_BASE_ADDR+0x6c)
#define reg_npe_0x84			REG_ADDR32(NPE_BASE_ADDR+0x84)
#define reg_npe_0x74			REG_ADDR32(NPE_BASE_ADDR+0x74)
#define reg_npe_0x80			REG_ADDR32(NPE_BASE_ADDR+0x80)
#define reg_npe_0x84			REG_ADDR32(NPE_BASE_ADDR+0x84)


#define reg_npe_irq_0		    REG_ADDR8(STIMER_BASE_ADDR+0x20)
enum{

	FLD_IRQ_NPE_BUS0 			    =   BIT(0),
	FLD_IRQ_NPE_BUS1 			    =   BIT(1),
	FLD_IRQ_NPE_BUS2 			    =   BIT(2),
	FLD_IRQ_NPE_BUS3 			    =   BIT(3),
	FLD_IRQ_NPE_BUS4 			    =   BIT(4),
	FLD_IRQ_NPE_BUS7 			    =   BIT(7),

};

#define reg_npe_irq_1		    REG_ADDR8(STIMER_BASE_ADDR+0x21)
enum{

	FLD_IRQ_NPE_BUS8 			    =   BIT(0),
	FLD_IRQ_NPE_BUS13 			    =   BIT(5),
	FLD_IRQ_NPE_BUS14 			    =   BIT(6),
	FLD_IRQ_NPE_BUS15 			    =   BIT(7),
};

#define reg_npe_irq_2		    REG_ADDR8(STIMER_BASE_ADDR+0x22)
enum{

	FLD_IRQ_NPE_BUS17 			    =   BIT(1),
	FLD_IRQ_NPE_BUS21 			    =   BIT(5),
	FLD_IRQ_NPE_BUS22 			    =   BIT(6),
	FLD_IRQ_NPE_BUS23			    =   BIT(7),
};

#define reg_npe_irq_3		    REG_ADDR8(STIMER_BASE_ADDR+0x23)
enum{

	FLD_IRQ_NPE_BUS24 			    =   BIT(0),
	FLD_IRQ_NPE_BUS25 			    =   BIT(1),
	FLD_IRQ_NPE_BUS26 			    =   BIT(2),
	FLD_IRQ_NPE_BUS27			    =   BIT(3),
	FLD_IRQ_NPE_BUS28 			    =   BIT(4),
	FLD_IRQ_NPE_BUS29 			    =   BIT(5),
	FLD_IRQ_NPE_BUS30 			    =   BIT(6),
	FLD_IRQ_NPE_BUS31			    =   BIT(7),
};
#endif
