/********************************************************************************************************
 * @file	pke_reg.h
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
#pragma once

#include "../sys.h"



/*******************************      pke registers: 0x110000      ******************************/

#define REG_PKE_BASE			0x110000

#define reg_pke_ctrl			REG_ADDR32(REG_PKE_BASE+0x00)
enum{
	FLD_PKE_CTRL_START			= BIT(0),
	FLD_PKE_CTRL_STOP			= BIT(16),
};

#define reg_pke_conf			REG_ADDR32(REG_PKE_BASE+0x04)
typedef enum{
	FLD_PKE_CONF_IRQ_EN			= BIT(8),
	FLD_PKE_CONF_PARTIAL_RADIX	= BIT_RNG(16,23),
	FLD_PKE_CONF_BASE_RADIX		= BIT_RNG(24,26),
}pke_conf_e;

#define reg_pke_mc_ptr			REG_ADDR32(REG_PKE_BASE+0x10)

#define reg_pke_stat			REG_ADDR32(REG_PKE_BASE+0x20)
typedef enum{
	FLD_PKE_STAT_DONE			= BIT(0),
}pke_status_e;

#define reg_pke_rt_code			REG_ADDR32(REG_PKE_BASE+0x24)
enum{
	FLD_PKE_RT_CODE_STOP_LOG	= BIT_RNG(0,3),
};

#define reg_pke_exe_conf		REG_ADDR32(REG_PKE_BASE+0x50)
enum{
	FLD_PKE_EXE_CONF_IAFF_R0	= BIT(0),
	FLD_PKE_EXE_CONF_IMON_R0	= BIT(1),
	FLD_PKE_EXE_CONF_IAFF_R1	= BIT(2),
	FLD_PKE_EXE_CONF_IMON_R1	= BIT(3),
	FLD_PKE_EXE_CONF_OAFF		= BIT(4),
	FLD_PKE_EXE_CONF_OMON		= BIT(5),
	FLD_PKE_EXE_CONF_ME_SCA_EN	= BIT_RNG(8,9),
};




