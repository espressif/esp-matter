/********************************************************************************************************
 * @file	swire_reg.h
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
#ifndef SWIRE_REG_H
#define SWIRE_REG_H
#include "../sys.h"




#define    SWIRE_BASE_ADDR			        0x100c00


#define   reg_swire_data      		    REG_ADDR8(SWIRE_BASE_ADDR)

#define   reg_swire_ctl				    REG_ADDR8(SWIRE_BASE_ADDR+1)
enum{
	FLD_SWIRE_WR	        	= BIT(0),
	FLD_SWIRE_RD 	     	    = BIT(1),
	FLD_SWIRE_CMD 		        = BIT(2),
	FLD_SWIRE_ERR_FLAG			= BIT(3),
	// default :FLD_SWIRE_EOP = 1: 1byte format--- 1bit cmd + 8bit data +1 clock end(0)
	// FLD_SWIRE_EOP = 0: 1byte format--- 1bit cmd + 8bit data +1bit(1) + 1 clock end(0)
	FLD_SWIRE_EOP     			= BIT(4),
	FLD_SWIRE_USB_DET    	  	= BIT(6),
	FLD_SWIRE_USB_EN      		= BIT(7),
};
#define   reg_swire_ctl2				 REG_ADDR8(SWIRE_BASE_ADDR+2)
enum{
	FLD_SWIRE_CLK_DIV	        = BIT_RNG(0,6),
};

#define   reg_swire_id				    REG_ADDR8(SWIRE_BASE_ADDR+3)
enum{
	FLD_SWIRE_ID_VALID	        = BIT_RNG(0,4),
	FLD_SWIRE_FIFO_MODE	        = BIT(7),
};
#endif
