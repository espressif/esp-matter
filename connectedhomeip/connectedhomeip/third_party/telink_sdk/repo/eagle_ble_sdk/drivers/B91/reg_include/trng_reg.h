/********************************************************************************************************
 * @file	trng_reg.h
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
#ifndef TRNG_REG_H_
#define TRNG_REG_H_

/*******************************      trng registers: 101800      ******************************/
#define REG_TRNG_BASE					0x101800


#define reg_trng_cr0					REG_ADDR8(REG_TRNG_BASE)
enum{
	FLD_TRNG_CR0_RBGEN		= 	BIT(0),
	FLD_TRNG_CR0_ROSEN0 	= 	BIT(1),
	FLD_TRNG_CR0_ROSEN1 	= 	BIT(2),
	FLD_TRNG_CR0_ROSEN2 	= 	BIT(3),
	FLD_TRNG_CR0_ROSEN3 	= 	BIT(4),
};

#define reg_trng_rtcr					REG_ADDR32(REG_TRNG_BASE+0x04)
enum{
	FLD_TRNG_RTCR_MSEL		= 	BIT(0),
};

#define reg_rbg_sr						REG_ADDR8(REG_TRNG_BASE+0x08)
enum{
	FLD_RBG_SR_DRDY			= 	BIT(0),
};

#define reg_rbg_dr						REG_ADDR32(REG_TRNG_BASE+0x0c)

#endif
