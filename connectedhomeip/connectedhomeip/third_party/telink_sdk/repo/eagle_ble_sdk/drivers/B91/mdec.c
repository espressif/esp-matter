/********************************************************************************************************
 * @file	mdec.c
 *
 * @brief	This is the source file for B91
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
#include"mdec.h"
#include "compiler.h"
#include "reg_include/mdec_reg.h"
#include "analog.h"
#include "clock.h"

/**
 * @brief		This function is used to initialize the MDEC module,include clock setting and input IO select.
 * @param[in]	pin	- mdec pin.
 * 					  In order to distinguish which pin the data is input from,only one input pin can be selected one time.
 * @return		none.
 */
void mdec_init(mdec_pin_e pin)
{
	analog_write_reg8(mdec_rst_addr,(analog_read_reg8(mdec_rst_addr) & (~FLD_CLS_MDEC)) | pin);//A0/B7/C4/D0/E0
}

/**
 * @brief		This function is used to read the receive data of MDEC module's IO.
 * @param[out]	dat		- The array to store date.
 * @return		1 decode success,  0 decode failure.
 */
unsigned char mdec_read_dat(unsigned char *dat)
{
	unsigned char m0,m1,m2,data_crc;

	dat[0]=analog_read_reg8(0x6a);
	dat[1]=analog_read_reg8(0x6b);
	dat[2]=analog_read_reg8(0x6c);
	dat[3]=analog_read_reg8(0x6d);
	dat[4]=analog_read_reg8(0x6e);

	m0= ((dat[0]>>5)<<4);
	m1= dat[0]&0x07;
	m2= m0+m1;
	data_crc=(((m2+dat[1])^dat[2])+dat[3])^0xa5;

	if(data_crc==dat[4]){
		return 1;
	}
	return 0;
}


