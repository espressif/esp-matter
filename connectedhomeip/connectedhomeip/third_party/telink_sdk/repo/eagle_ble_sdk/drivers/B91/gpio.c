/********************************************************************************************************
 * @file	gpio.c
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
#include "gpio.h"

/**********************************************************************************************************************
 *                                			  local constants                                                       *
 *********************************************************************************************************************/


/**********************************************************************************************************************
 *                                           	local macro                                                        *
 *********************************************************************************************************************/


/**********************************************************************************************************************
 *                                             local data type                                                     *
 *********************************************************************************************************************/


/**********************************************************************************************************************
 *                                              global variable                                                       *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *                                              local variable                                                     *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *                                          local function prototype                                               *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *                                         global function implementation                                             *
 *********************************************************************************************************************/




/**
 * @brief      This function enable the input function of a pin.
 * @param[in]  pin - the pin needs to set the input function.
 * @return     none.
 */
void gpio_input_en(gpio_pin_e pin)
{
	unsigned char	bit = pin & 0xff;
	unsigned short group = pin & 0xf00;

	if(group == GPIO_GROUPA || group == GPIO_GROUPB || group == GPIO_GROUPE)
	{
		BM_SET(reg_gpio_ie(pin), bit);
	}

    else if(group == GPIO_GROUPC)
    {
    	analog_write_reg8(areg_gpio_pc_ie, analog_read_reg8(areg_gpio_pc_ie)|bit);
    }

    else if(group == GPIO_GROUPD)
    {
    	analog_write_reg8(areg_gpio_pd_ie, analog_read_reg8(areg_gpio_pd_ie)|bit);
    }
}

/**
 * @brief      This function disable the input function of a pin.
 * @param[in]  pin - the pin needs to set the input function.
 * @return     none.
 */
void gpio_input_dis(gpio_pin_e pin)
{
	unsigned char	bit = pin & 0xff;
	unsigned short group = pin & 0xf00;

	if(group == GPIO_GROUPA || group == GPIO_GROUPB || group == GPIO_GROUPE)
	{
		BM_CLR(reg_gpio_ie(pin), bit);
	}

    else if(group == GPIO_GROUPC)
    {
    	analog_write_reg8(areg_gpio_pc_ie, analog_read_reg8(areg_gpio_pc_ie)&(~bit));
    }

    else if(group == GPIO_GROUPD)
    {
    	analog_write_reg8(areg_gpio_pd_ie, analog_read_reg8(areg_gpio_pd_ie)&(~bit));
    }
}

/**
 * @brief      This function set the input function of a pin.
 * @param[in]  pin - the pin needs to set the input function
 * @param[in]  value - enable or disable the pin's input function(1: enable,0: disable )
 * @return     none
 */
void gpio_set_input(gpio_pin_e pin, unsigned char value)
{
	if(value)
	{
		gpio_input_en(pin);
	}
	else
	{
		gpio_input_dis(pin);
	}
}

/**
 * @brief      This function set the pin's driving strength at strong.
 * @param[in]  pin - the pin needs to set the driving strength
 * @return     none
 */
 void gpio_ds_en(gpio_pin_e pin)
{
	unsigned char	bit = pin & 0xff;
	unsigned short group = pin & 0xf00;
	if(group == GPIO_GROUPC)
	{analog_write_reg8(areg_gpio_pc_ds, analog_read_reg8(areg_gpio_pc_ds)|bit);}
	else if(group == GPIO_GROUPD)
	{analog_write_reg8(areg_gpio_pd_ds, analog_read_reg8(areg_gpio_pd_ds)|bit);}
	else
	{BM_SET(reg_gpio_ds(pin), bit);}
}


 /**
  * @brief      This function set the pin's driving strength.
  * @param[in]  pin - the pin needs to set the driving strength at poor.
  * @return     none
  */
  void gpio_ds_dis(gpio_pin_e pin)
 {
 	unsigned char	bit = pin & 0xff;
 	unsigned short group = pin & 0xf00;
 	if(group == GPIO_GROUPC)
 	{analog_write_reg8(areg_gpio_pc_ds, analog_read_reg8(areg_gpio_pc_ds)&(~bit));}
 	else if(group == GPIO_GROUPD)
 	{analog_write_reg8(areg_gpio_pd_ds, analog_read_reg8(areg_gpio_pd_ds)&(~bit));}
 	else
 	{BM_CLR(reg_gpio_ds(pin), bit);}
 }


/**
 * @brief      This function servers to set the specified GPIO as high resistor.
 * @param[in]  pin  - select the specified GPIO
 * @return     none.
 */
void gpio_shutdown(gpio_pin_e pin)
{
	unsigned short group = pin & 0xf00;
	unsigned char bit = pin & 0xff;
	switch(group)
	{
		case GPIO_GROUPA:
			reg_gpio_pa_oen |= bit;//disable output
			reg_gpio_pa_out &= (~bit);//set low level
			reg_gpio_pa_ie &= (~bit);//disable input
			break;
		case GPIO_GROUPB:
			reg_gpio_pb_oen |= bit;
			reg_gpio_pb_out &= (~bit);
			reg_gpio_pb_ie &= (~bit);
			break;
		case GPIO_GROUPC:
			reg_gpio_pc_oen |= bit;
			reg_gpio_pc_out &= (~bit);
			analog_write_reg8(areg_gpio_pc_ie, analog_read_reg8(areg_gpio_pc_ie) & (~bit));
			break;
		case GPIO_GROUPD:
			reg_gpio_pd_oen |= bit;
			reg_gpio_pd_out &= (~bit);
			analog_write_reg8(areg_gpio_pd_ie, analog_read_reg8(areg_gpio_pd_ie) & (~bit));
			break;

		case GPIO_GROUPE:
			reg_gpio_pe_oen |= bit;
			reg_gpio_pe_out &= (~bit);
			reg_gpio_pe_ie &= (~bit);
			break;

		case GPIO_ALL:
		{
			//as gpio
			reg_gpio_pa_gpio = 0x7f;
			reg_gpio_pb_gpio = 0xff;
			reg_gpio_pc_gpio = 0xff;
			reg_gpio_pd_gpio = 0xff;
			reg_gpio_pe_gpio = 0xff;

			//output disable
			reg_gpio_pa_oen = 0xff;
			reg_gpio_pb_oen = 0xff;
			reg_gpio_pc_oen = 0xff;
			reg_gpio_pd_oen = 0xff;
			reg_gpio_pe_oen = 0xff;

			//set low level
			reg_gpio_pa_out = 0x00;
			reg_gpio_pb_out = 0x00;
			reg_gpio_pc_out = 0x00;
			reg_gpio_pd_out = 0x00;
			reg_gpio_pe_out = 0x00;

			//disable input
			reg_gpio_pa_ie = 0x80;					//SWS
			reg_gpio_pb_ie = 0x00;
			analog_write_reg8(areg_gpio_pc_ie, 0);
			analog_write_reg8(areg_gpio_pd_ie, 0);
			reg_gpio_pe_ie = 0x00;
		}
	}
}



/**
 * @brief     This function set a pin's IRQ.
 * @param[in] pin 			- the pin needs to enable its IRQ.
 * @param[in] trigger_type  - gpio interrupt type.
 * 							  0: rising edge.
 * 							  1: falling edge.
 * 							  2: high level.
 * 							  3: low level.
 * @return    none.
 */
void gpio_set_irq(gpio_pin_e pin, gpio_irq_trigger_type_e trigger_type)
{
	switch(trigger_type)
	{
	case INTR_RISING_EDGE:
		BM_CLR(reg_gpio_pol(pin), pin & 0xff);
		BM_CLR(reg_gpio_irq_risc_mask, FLD_GPIO_IRQ_LVL_GPIO);
	break;
	case INTR_FALLING_EDGE:
		BM_SET(reg_gpio_pol(pin), pin & 0xff);
		BM_CLR(reg_gpio_irq_risc_mask, FLD_GPIO_IRQ_LVL_GPIO);
	break;
	case INTR_HIGH_LEVEL:
		BM_CLR(reg_gpio_pol(pin), pin & 0xff);
		BM_SET(reg_gpio_irq_risc_mask, FLD_GPIO_IRQ_LVL_GPIO);
	break;
	case INTR_LOW_LEVEL:
		BM_SET(reg_gpio_pol(pin), pin & 0xff);
		BM_SET(reg_gpio_irq_risc_mask, FLD_GPIO_IRQ_LVL_GPIO);
	 break;
	}
	reg_gpio_irq_ctrl |= FLD_GPIO_CORE_INTERRUPT_EN;
	reg_gpio_irq_clr = FLD_GPIO_IRQ_CLR;//must clear cause to unexpected interrupt.
	BM_SET(reg_gpio_irq_risc_mask, FLD_GPIO_IRQ_MASK_GPIO);

}

/**
 * @brief     This function set a pin's IRQ_RISC0.
 * @param[in] pin 			- the pin needs to enable its IRQ.
 * @param[in] trigger_type  - gpio interrupt type 0  rising edge 1 falling edge 2 high level 3 low level.
 * @return    none.
 */
void gpio_set_gpio2risc0_irq(gpio_pin_e pin, gpio_irq_trigger_type_e trigger_type)
{

	switch(trigger_type)
	{
	case INTR_RISING_EDGE:
		BM_CLR(reg_gpio_pol(pin), pin & 0xff);
		BM_CLR(reg_gpio_irq_risc_mask, FLD_GPIO_IRQ_LVL_GPIO2RISC0);
	break;
	case INTR_FALLING_EDGE:
		BM_SET(reg_gpio_pol(pin), pin & 0xff);
		BM_CLR(reg_gpio_irq_risc_mask, FLD_GPIO_IRQ_LVL_GPIO2RISC0);
	break;
	case INTR_HIGH_LEVEL:
		BM_CLR(reg_gpio_pol(pin), pin & 0xff);
		BM_SET(reg_gpio_irq_risc_mask, FLD_GPIO_IRQ_LVL_GPIO2RISC0);
		break;
	case INTR_LOW_LEVEL:
		BM_SET(reg_gpio_pol(pin), pin & 0xff);
		BM_SET(reg_gpio_irq_risc_mask, FLD_GPIO_IRQ_LVL_GPIO2RISC0);
	   break;
	}
	reg_gpio_irq_clr = FLD_GPIO_IRQ_GPIO2RISC0_CLR;//must clear cause to unexpected interrupt.
	BM_SET(reg_gpio_irq_risc_mask, FLD_GPIO_IRQ_MASK_GPIO2RISC0);

}

/**
 * @brief     This function set a pin's IRQ_RISC1.
 * @param[in] pin 			- the pin needs to enable its IRQ.
 * @param[in] trigger_type  - gpio interrupt type 0  rising edge 1 falling edge 2 high level 3 low level
 * @return    none.
 */
void gpio_set_gpio2risc1_irq(gpio_pin_e pin, gpio_irq_trigger_type_e trigger_type)
{
	switch(trigger_type)
	{
	case INTR_RISING_EDGE:
		BM_CLR(reg_gpio_pol(pin), pin & 0xff);
		BM_CLR(reg_gpio_irq_risc_mask, FLD_GPIO_IRQ_LVL_GPIO2RISC1);
	break;
	case INTR_FALLING_EDGE:
		BM_SET(reg_gpio_pol(pin), pin & 0xff);
		BM_CLR(reg_gpio_irq_risc_mask, FLD_GPIO_IRQ_LVL_GPIO2RISC1);
	break;
	case INTR_HIGH_LEVEL:
		BM_CLR(reg_gpio_pol(pin), pin & 0xff);
		BM_SET(reg_gpio_irq_risc_mask, FLD_GPIO_IRQ_LVL_GPIO2RISC1);
		break;
	case INTR_LOW_LEVEL:
		BM_SET(reg_gpio_pol(pin), pin & 0xff);
		BM_SET(reg_gpio_irq_risc_mask, FLD_GPIO_IRQ_LVL_GPIO2RISC1);
	   break;
	}
	reg_gpio_irq_clr =FLD_GPIO_IRQ_GPIO2RISC1_CLR;//must clear cause to unexpected interrupt.
	BM_SET(reg_gpio_irq_risc_mask, FLD_GPIO_IRQ_MASK_GPIO2RISC1);

}

/**
 * @brief     This function set a pin's pull-up/down resistor.
 * @param[in] pin - the pin needs to set its pull-up/down resistor.
 * @param[in] up_down_res - the type of the pull-up/down resistor.
 * @return    none.
 */
void gpio_set_up_down_res(gpio_pin_e pin, gpio_pull_type_e up_down_res)
{
	unsigned char r_val = up_down_res & 0x03;

	unsigned char base_ana_reg = 0x0e + ((pin >> 8) << 1) + ( (pin & 0xf0) ? 1 : 0 );  //group = gpio>>8;
	unsigned char shift_num, mask_not;

	if(pin & 0x11){
			shift_num = 0;
			mask_not = 0xfc;
		}
		else if(pin & 0x22){
			shift_num = 2;
			mask_not = 0xf3;
		}
		else if(pin & 0x44){
			shift_num = 4;
			mask_not = 0xcf;
		}
		else if(pin & 0x88){
			shift_num = 6;
			mask_not = 0x3f;
		}
		else{
			return;
		}
	analog_write_reg8(base_ana_reg, (analog_read_reg8(base_ana_reg) & mask_not) | (r_val << shift_num));
}

/**
 * @brief     This function set pin's 30k pull-up registor.
 * @param[in] pin - the pin needs to set its pull-up registor.
 * @return    none.
  * @attention  This function sets the digital pull-up, it will not work after entering low power consumption.
 */
void gpio_set_pullup_res_30k(gpio_pin_e pin)
{
	unsigned char	bit = pin & 0xff;
	unsigned short group = pin & 0xf00;

	if(group==GPIO_GROUPC)
	{
		analog_write_reg8(areg_gpio_pc_pe, analog_read_reg8(areg_gpio_pc_pe) | bit);
	}
	else if(group==GPIO_GROUPD)
	{
		analog_write_reg8(areg_gpio_pd_pe, analog_read_reg8(areg_gpio_pd_pe) | bit);
	}
	else
	{
		BM_SET(reg_gpio_oen(pin),bit);
		BM_SET(reg_gpio_out(pin),bit);
	}
}


/**********************************************************************************************************************
  *                    						local function implementation                                             *
  *********************************************************************************************************************/

