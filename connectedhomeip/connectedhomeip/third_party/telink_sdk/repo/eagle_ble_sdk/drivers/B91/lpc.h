/********************************************************************************************************
 * @file	lpc.h
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

#include "analog.h"

/**
 * define input IO.
 */
typedef enum{
	LPC_INPUT_PB1 = 1,
	LPC_INPUT_PB2 = 2,
	LPC_INPUT_PB3 = 3,
	LPC_INPUT_PB4 = 4,
	LPC_INPUT_PB5 = 5,
	LPC_INPUT_PB6 = 6,
	LPC_INPUT_PB7 = 7,
}lpc_input_channel_e;

/**
 * define work mode.
 */
typedef enum{
	LPC_NORMAL = 0,
	LPC_LOWPOWER,
}lpc_mode_e;

/**
 * define Reference voltage.
 */
typedef enum{
	LPC_REF_974MV  = 1,
	LPC_REF_923MV  = 2,
	LPC_REF_872MV  = 3,
	LPC_REF_820MV  = 4,
	LPC_REF_PB0    = 5,
	LPC_REF_PB3    = 6,
}lpc_reference_e;

/**
 * define scale.
 */
typedef enum{
	LPC_SCALING_PER25  = 0,
	LPC_SCALING_PER50  = 1,
	LPC_SCALING_PER75  = 2,
	LPC_SCALING_PER100 = 3,
}lpc_scaling_e;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief		This function servers to powers down low power comparator.
 * @return		none.
 */
static inline void lpc_power_down(void)
{
	analog_write_reg8(0x07,(analog_read_reg8(0x07))|0x08);
}

/**
 * @brief		This function servers to power on low power comparator.
 * @return		none.
 */
static inline void lpc_power_on(void)
{
	analog_write_reg8(0x06,analog_read_reg8(0x06) & 0xfd);
}

/**
 * @brief		This function selects input channel for low power comparator.
 * @param[in]	pin		- selected input channel.Input derived from external PortB(PB<1>~PB<7>).
 * @return		none.
 */
static inline void lpc_set_input_chn(lpc_input_channel_e pin)
{
	analog_write_reg8(0x0d,(analog_read_reg8(0x0d) & 0xf8) | pin);
}

/**
 * @brief		This function serves to set scaling_coefficient for low power comparator.
 * @param[in]	divider	- selected scaling coefficient.(%25,%50,%75,%100)
 * @return		none.
 */
static inline void lpc_set_scaling_coeff(lpc_scaling_e divider)
{
	analog_write_reg8(0x0b,(analog_read_reg8(0x0b)&0xcf)|(divider<<4));
}

/**
 * @brief		This function serves to get the comparison results.if Vin>Vref 0x88[6]=0,else 0x88[6]=1.
 * @return		comparison results.
 */
static inline unsigned char lpc_get_result(void)
{
	return ((analog_read_reg8(0x88)&0x40)>>6);
}

/**
 * @brief		This function selects input reference voltage for low power comparator.
 * @param[in]	mode	- lower power comparator working mode includes normal mode and low power mode.
 * @param[in]	ref		- selected input reference voltage.
 * @return		none.
 */
void lpc_set_input_ref(lpc_mode_e mode, lpc_reference_e ref);

#ifdef __cplusplus
}
#endif

