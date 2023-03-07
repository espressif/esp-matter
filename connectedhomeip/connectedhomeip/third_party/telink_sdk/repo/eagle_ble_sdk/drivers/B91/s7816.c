/********************************************************************************************************
 * @file	s7816.c
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
#include "s7816.h"
#include "dma.h"
#include "plic.h"

volatile unsigned int s7816_rst_pin;
volatile unsigned int s7816_vcc_pin;
volatile unsigned int s7816_rtx_pin;
volatile unsigned char s7816_clock;
volatile int s7816_rst_time; // us
/**
 * @brief      	This function is used to set the s7816 clock.
 * @param[in]  	div	- set the divider of clock of 7816 module.
 * @return     	none.
 * @note        the clk-source of s7816 is 24M-pad,the clk of clk-pin can be divided as follow.
 * 				div:        0x60-4Mhz     0x40-6Mhz   0x20-12Mhz
 * 				baudrate:   0x60-10752    0x40-16194  0x20-32388
 */
void s7816_set_clk(unsigned char div)
{
    reg_7816_clk_div &= 0x0f;
    reg_7816_clk_div |= (unsigned char) div;
}

/**
 * @brief      	This function is used to set the rst-wait time of the s7816 module.
 * @param[in]  	rst_time_us - set the s7816_rst_time.
 * @return     	none.
 */
void s7816_set_time(int rst_time_us)
{
    s7816_rst_time = rst_time_us;
}
/**
 * @brief      	This function is used to set the RST pin of s7816.
 * @param[in]  	pin_7816_rst - the RST pin of s7816.
 * @return     	none.
 */
void s7816_set_rst_pin(gpio_pin_e pin_7816_rst)
{
    s7816_rst_pin = pin_7816_rst;
    gpio_function_en(pin_7816_rst);
    gpio_output_en(pin_7816_rst);
    gpio_input_dis(pin_7816_rst);
    gpio_set_low_level(pin_7816_rst);
}

/**
 * @brief      	This function is used to set the VCC pin of s7816.
 * @param[in]   pin_7816_vcc - the VCC pin of s7816.
 * @return     	none.
 */
void s7816_set_vcc_pin(gpio_pin_e pin_7816_vcc)
{
    s7816_vcc_pin = pin_7816_vcc;
    gpio_function_en(pin_7816_vcc);
    gpio_output_en(pin_7816_vcc);
    gpio_input_dis(pin_7816_vcc);
    gpio_set_low_level(pin_7816_vcc);
}

/**
 * @brief      	This function is used to initialize the s7816 module.
 * @param[in]  	uart_num     - UART0 or UART1.
 * @param[in]  	clock        - the clock of s7816.
 * @param[in]  	f            - the clock frequency regulator of s7816,372 by default.
 * @param[in]  	d            - the bitrate regulator of s7816,1 by default.
 * @return     	none.
 */
void s7816_init(uart_num_e uart_num, s7816_clock_e clock, int f, int d)
{
    unsigned short div;
    unsigned char bwpc;
    s7816_clock    = clock;
    s7816_rst_time = 40000 / clock; // us

    int baud = clock * 1000000 * d / f;
    if (clock == S7816_4MHZ)
    {
        s7816_set_clk(0x60);
    }
    else if (clock == S7816_6MHZ)
    {
        s7816_set_clk(0x40);
    }
    else if (clock == S7816_12MHZ)
    {
        s7816_set_clk(0x20);
    }
    uart_reset(uart_num);
    uart_cal_div_and_bwpc(baud, 24 * 1000 * 1000, &div, &bwpc);
    uart_init(uart_num, div, bwpc, UART_PARITY_EVEN, UART_STOP_BIT_ONE); // 7816 protocol stipulate the parity bit should be even.
}

/**
 * @brief      	This function is used to set all the pin of s7816 module.
 * @param[in]  	rst_pin     - the rst pin of s7816.
 * @param[in]  	vcc_pin     - the vcc pin of s7816.
 * @param[in]  	clk_pin     - the clk pin of s7816.
 * @param[in]  	trx_pin     - the trx pin of s7816.
 * @return     	none.
 */
void s7816_set_pin(gpio_pin_e rst_pin, gpio_pin_e vcc_pin, s7816_clk_pin_e clk_pin, s7816_rtx_pin_e rtx_pin)
{
    s7816_set_rst_pin(rst_pin);
    s7816_rst_pin = rst_pin;

    s7816_set_vcc_pin(vcc_pin);
    s7816_vcc_pin = vcc_pin;

    reg_gpio_func_mux(clk_pin) = (reg_gpio_func_mux(clk_pin) & (~BIT_RNG(0, 1))) | BIT(0);
    gpio_function_dis((gpio_pin_e) clk_pin);

    s7816_rtx_pin =
        rtx_pin; // if the trx function set to early,it may trigger interrupt by accident.so we set the function in coldreset.
}

/**
 * @brief      	This function is used to active the IC card,set the trx pin and coldreset.
 * @param[in]  	none.
 * @return     	none.
 * @note        extra time is needed for initial-atr after the function.
 */
void s7816_coldreset()
{
    gpio_set_high_level(s7816_vcc_pin);
    delay_us(20);               // wait for the vcc  stable.
    reg_7816_clk_div |= BIT(7); // enable the 7816 clk,the pin is A0.
    delay_us(s7816_rst_time);
    s7816_set_rtx_pin(s7816_rtx_pin);   // uart tx/rx pin set,if the trx pin set before this place,it may
    gpio_set_high_level(s7816_rst_pin); // the IC card will return the initial ATR.
}

/**
 * @brief      	This function is used to release the trigger.
 * @param[in]  	none.
 * @return     	none.
 */
void s7816_release_trig()
{
    gpio_set_low_level(s7816_rst_pin);
    reg_7816_clk_div &= (BIT(7) - 1);
    gpio_set_low_level(s7816_vcc_pin);
}

/**
 * @brief      	This function is used to warmreset.
 * @param[in]  	none.
 * @return     	none.
 * @note        the warmreset is required after the IC-CARD active,extra time is needed for initial-atr after the function.
 */
void s7816_warmreset()
{
    gpio_set_low_level(s7816_rst_pin);
    delay_us(s7816_rst_time);
    gpio_set_high_level(s7816_rst_pin); // The IC card will return the initial ATR.
}

/**
 * @brief      	This function is used to warmreset.
 * @param[in]  	uart_num - UART0 or UART1.
 * @param[in]   tx_data  - the data need to send.
 * return       none.
 */
void s7816_send_byte(uart_num_e uart_num, unsigned char tx_data)
{
    uart_send_byte(uart_num, tx_data);
    uart_rtx_pin_tx_trig(uart_num);
}
