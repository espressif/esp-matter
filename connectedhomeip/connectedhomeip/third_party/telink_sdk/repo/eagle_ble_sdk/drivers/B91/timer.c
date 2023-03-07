/********************************************************************************************************
 * @file	timer.c
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
#include "timer.h"
/**********************************************************************************************************************
 *                                         global function implementation                                             *
 *********************************************************************************************************************/

/**
 * @brief     the specifed timer start working.
 * @param[in] type - select the timer to start.
 * @return    none
 */
void timer_start(timer_type_e type)
{
 	switch(type)
 	{
 		case TIMER0:
 			reg_tmr_ctrl0 |= FLD_TMR0_EN;
 			break;
 		case TIMER1:
 			reg_tmr_ctrl0 |= FLD_TMR1_EN;
 			break;
 		default:
 			break;
 	}
}

/**
 * @brief     the specifed timer stop working.
 * @param[in] type - select the timer to stop.
 * @return    none
 */
void timer_stop(timer_type_e type)
{
	switch(type)
	{
		case TIMER0:
			reg_tmr_ctrl0 &= (~FLD_TMR0_EN);
			break;
		case TIMER1:
			reg_tmr_ctrl0 &= (~FLD_TMR1_EN);
			break;
		default:
			break;
	}
}



/**
 * @brief     set mode, initial tick and capture of timer.
 * @param[in] type - select the timer to start.
 * @param[in] mode - select mode for timer.
 * @param[in] init_tick - initial tick.
 * @param[in] cap_tick  - tick of capture.
 * @return    none
 */
void timer_set_mode(timer_type_e type, timer_mode_e mode)
{
	switch(type)
 	{
 		case TIMER0:
 			reg_tmr_sta = FLD_TMR_STA_TMR0; //clear irq status
 		 	reg_tmr_ctrl0 &= (~FLD_TMR0_MODE);
 		 	reg_tmr_ctrl0 |= mode;
 			break;
 		case TIMER1:
 			reg_tmr_sta = FLD_TMR_STA_TMR1; //clear irq status
 			reg_tmr_ctrl0 &= (~FLD_TMR1_MODE);
 			reg_tmr_ctrl0 |= (mode<<4);
 			break;
 		default:
 			break;
 	}

}

/**
 * @brief     initiate GPIO for gpio trigger and gpio width mode of timer.
 * @param[in] type - select the timer to start.
 * @param[in] pin - select pin for timer.
 * @param[in] pol - select polarity for gpio trigger and gpio width
 * @return    none
 */
void timer_gpio_init(timer_type_e type, gpio_pin_e pin, gpio_pol_e pol )
{
	gpio_function_en(pin);
	gpio_output_dis(pin); 	//disable output
	gpio_input_en(pin);		//enable input
 	switch(type)
 	{
 		case TIMER0:
 		 	if(pol==POL_FALLING)
 		 	{
 		 		gpio_set_up_down_res(pin,GPIO_PIN_PULLUP_10K);
 		 		gpio_set_gpio2risc0_irq(pin,INTR_LOW_LEVEL);
 		 		gpio_gpio2risc0_irq_en(pin);
 		 	}
 		 	else if(pol==POL_RISING)
 		 	{
 		 		gpio_set_up_down_res(pin,GPIO_PIN_PULLDOWN_100K);
 		 		gpio_set_gpio2risc0_irq(pin,INTR_HIGH_LEVEL);
 		 		gpio_gpio2risc0_irq_en(pin);
 		 	}
 			break;

 		case TIMER1:
 		 	if(pol==POL_FALLING)
 		 	{
 		 		gpio_set_up_down_res(pin,GPIO_PIN_PULLUP_10K);
 		 		gpio_set_gpio2risc1_irq(pin,INTR_LOW_LEVEL);
 		 		gpio_gpio2risc1_irq_en(pin);
 		 	}
 		 	else if(pol==POL_RISING)
 		 	{
 		 		gpio_set_up_down_res(pin,GPIO_PIN_PULLDOWN_100K);
 		 		gpio_set_gpio2risc1_irq(pin,INTR_HIGH_LEVEL);
 		 		gpio_gpio2risc1_irq_en(pin);

 		 	}
 			break;

 		default:
 			break;
 	}

}


