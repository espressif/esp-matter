/********************************************************************************************************
 * @file	blt_led.c
 *
 * @brief	This is the source file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	2020.06
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#include "tl_common.h"
#include "drivers.h"
#include "blt_led.h"



_attribute_data_retention_ device_led_t device_led;

/**
 * @brief		This function is used to control device led on or off
 * @param[in]	on - the status of led
 * @return      none
 */
void device_led_on_off(u8 on)
{
	gpio_write( device_led.gpio_led, on^device_led.polar );
	gpio_set_output_en(device_led.gpio_led, on);
	device_led.isOn = on;
}


/**
 * @brief		This function is used to initialize device led setting
 * @param[in]	gpio - the GPIO corresponding to device led
 * @param[in]	polarity - 1 for high led on, 0 for low led on
 * @return      none
 */
void device_led_init(u32 gpio,u8 polarity){  //polarity: 1 for high led on, 0 for low led on

#if (BLT_APP_LED_ENABLE)
	device_led.gpio_led = gpio;
	device_led.polar = !polarity;
	gpio_write( gpio, !polarity );
#endif
}

/**
 * @brief		This function is used to create new led task
 * @param[in]	led_cfg - Configure the parameters for led event
 * @return      0 - new led event priority not higher than the not ongoing one
 * 				1 - new led event created successfully
 */
int device_led_setup(led_cfg_t led_cfg)
{
#if (BLT_APP_LED_ENABLE)
	if( device_led.repeatCount &&  device_led.priority >= led_cfg.priority){
		return 0; //new led event priority not higher than the not ongoing one
	}
	else{
		device_led.onTime_ms = led_cfg.onTime_ms;
		device_led.offTime_ms = led_cfg.offTime_ms;
		device_led.repeatCount = led_cfg.repeatCount;
		device_led.priority = led_cfg.priority;

        if(led_cfg.repeatCount == 0xff){ //for long on/long off
        	device_led.repeatCount = 0;
        }
        else{ //process one of on/off Time is zero situation
        	if(!device_led.onTime_ms){  //onTime is zero
        		device_led.offTime_ms *= device_led.repeatCount;
        		device_led.repeatCount = 1;
        	}
        	else if(!device_led.offTime_ms){
        		device_led.onTime_ms *= device_led.repeatCount;
        	    device_led.repeatCount = 1;
        	}
        }

        device_led.startTick = clock_time();
        device_led_on_off(device_led.onTime_ms ? 1 : 0);

		return 1;
	}
#else
	return 0;
#endif
}

/**
 * @brief		This function is used to manage led tasks
 * @param[in]	none
 * @return      none
 */
void led_proc(void)
{
#if (BLT_APP_LED_ENABLE)
	if(device_led.isOn){
		if(clock_time_exceed(device_led.startTick,device_led.onTime_ms*1000)){
			device_led_on_off(0);
			if(device_led.offTime_ms){ //offTime not zero
				device_led.startTick += device_led.onTime_ms*SYSTEM_TIMER_TICK_1MS;
			}
			else{
				device_led.repeatCount = 0;
			}
		}
	}
	else{
		if(clock_time_exceed(device_led.startTick,device_led.offTime_ms*1000)){
			if(--device_led.repeatCount){
				device_led_on_off(1);
				device_led.startTick += device_led.offTime_ms*SYSTEM_TIMER_TICK_1MS;
			}
		}
	}
#endif
}

