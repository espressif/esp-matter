/********************************************************************************************************
 * @file	s7816.h
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
#ifndef S7816_H_
#define S7816_H_

#include "gpio.h"
#include "uart.h"
#include "stimer.h"

#define s7816_en(uart_num)                uart_rtx_en(uart_num)
#define s7816_set_rtx_pin(s7816_trx_pin)  uart_set_rtx_pin(s7816_trx_pin)

/**
 *  @brief  Define 7816 TRX pin.
 */
typedef enum{
	S7816_UART0_RTX_A4=GPIO_PA4,
	S7816_UART0_RTX_B3=GPIO_PB3,
	S7816_UART0_RTX_D3=GPIO_PD3,

	S7816_UART1_RTX_C7=GPIO_PC7,
	S7816_UART1_RTX_D7=GPIO_PD7,
	S7816_UART1_RTX_E2=GPIO_PE2,
}s7816_rtx_pin_e;

/**
 *  @brief  Define 7816 clock.
 */
typedef enum{
	S7816_4MHZ=4,
	S7816_6MHZ=6,
	S7816_12MHZ=12,
}s7816_clock_e;

/**
 *  @brief  Define 7816 clk pin.
 */
typedef enum{
	S7817_CLK_PA0=GPIO_PA0,
}s7816_clk_pin_e;


/**
 * @brief      	This function is used to set the s7816 clock.
 * @param[in]  	div	- set the divider of clock of 7816 module.
 * @return     	none.
 * @note        the clk-source of s7816 is 24M-pad,the clk of clk-pin can be divided as follow.
 * 				div:        0x60-4Mhz     0x40-6Mhz   0x20-12Mhz
 * 				baudrate:   0x60-10752    0x40-16194  0x20-32388
 */
extern void s7816_set_clk(unsigned char div);


/**
 * @brief      	This function is used to initialize the s7816 module.
 * @param[in]  	uart_num     - UART0 or UART1.
 * @param[in]  	clock        - the clock of s7816.
 * @param[in]  	f            - the clock frequency conversion factor of s7816.
 * @param[in]  	d            - the bitrate regulator of s7816.
 * @return     	none.
 */
extern void s7816_init(uart_num_e uart_num,s7816_clock_e clock,int f,int d);

/**
 * @brief      	This function is used to active the IC card,set the trx pin and coldreset.
 * @param[in]  	none.
 * @return     	none.
 * @note        extra time is needed for initial-atr after the function.
 */
extern void s7816_coldreset();

/**
 * @brief      	This function is used to set all the pin of s7816 module.
 * @param[in]  	rst_pin     - the rst pin of s7816.
 * @param[in]  	vcc_pin     - the vcc pin of s7816.
 * @param[in]  	clk_pin     - the clk pin of s7816.
 * @param[in]  	trx_pin     - the trx pin of s7816.
 * @return     	none.
 */
extern void s7816_set_pin(gpio_pin_e rst_pin,gpio_pin_e vcc_pin,s7816_clk_pin_e clk_pin,s7816_rtx_pin_e trx_pin);

/**
 * @brief      	This function is used to release the trigger
 * @param[in]  	none.
 * @return     	none.
 */
extern void s7816_release_trig();

/**
 * @brief      	This function is used to set the RST pin of s7816.
 * @param[in]  	pin_7816_rst - the RST pin of s7816.
 * @return     	none.
 */
extern void s7816_set_rst_pin(gpio_pin_e pin_7816_rst);

/**
 * @brief      	This function is used to set the VCC pin of s7816.
 * @param[in]   pin_7816_vcc - the VCC pin of s7816.
 * @return     	none.
 */
extern void s7816_set_vcc_pin(gpio_pin_e pin_7816_vcc);

/**
 * @brief      	This function is used to warmreset.
 * @param[in]  	none.
 * @return     	none.
 * @note        the warmreset is required after the IC-CARD active,extra time is needed for initial-atr after the function.
 */
extern void s7816_warmreset();

/**
 * @brief      	This function is used to set the rst-wait time of the s7816 module.
 * @param[in]  	rst_time_us - set the s7816_rst_time.
 * @return     	none.
 */
extern void s7816_set_time(int rst_time_us);

/**
 * @brief      	This function is used to warmreset.
 * @param[in]  	uart_num - UART0 or UART1.
 * @param[in]   tx_data  - the data need to send.
 * return       none.
 */
extern void s7816_send_byte(uart_num_e uart_num, unsigned char tx_data);

#endif /* S7816_H_ */

