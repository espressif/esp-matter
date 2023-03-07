/********************************************************************************************************
 * @file	blt_led.h
 *
 * @brief	This is the header file for BLE SDK
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

#ifndef BLT_LED_H_
#define BLT_LED_H_

#include "tl_common.h"



#ifndef BLT_APP_LED_ENABLE
#define BLT_APP_LED_ENABLE				0
#endif


//led management
/**
 * @brief	Configure the parameters for led event
 */
typedef struct{
	unsigned short onTime_ms;
	unsigned short offTime_ms;

	unsigned char  repeatCount;  //0xff special for long on(offTime_ms=0)/long off(onTime_ms=0)
	unsigned char  priority;     //0x00 < 0x01 < 0x02 < 0x04 < 0x08 < 0x10 < 0x20 < 0x40 < 0x80
} led_cfg_t;

/**
 * @brief	the status of led event
 */
typedef struct {
	unsigned char  isOn;
	unsigned char  polar;
	unsigned char  repeatCount;
	unsigned char  priority;


	unsigned short onTime_ms;
	unsigned short offTime_ms;

	unsigned int gpio_led;
	unsigned int startTick;
}device_led_t;

extern device_led_t device_led;

#define  DEVICE_LED_BUSY	(device_led.repeatCount)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief		This function is used to manage led tasks
 * @param[in]	none
 * @return      none
 */
extern void led_proc(void);

/**
 * @brief		This function is used to initialize device led setting
 * @param[in]	gpio - the GPIO corresponding to device led
 * @param[in]	polarity - 1 for high led on, 0 for low led on
 * @return      none
 */
extern void device_led_init(u32 gpio,u8 polarity);

/**
 * @brief		This function is used to create new led task
 * @param[in]	led_cfg - Configure the parameters for led event
 * @return      0 - new led event priority not higher than the not ongoing one
 * 				1 - new led event created successfully
 */
int device_led_setup(led_cfg_t led_cfg);

/**
 * @brief		This function is used to manage led tasks
 * @param[in]	none
 * @return      none
 */
static inline void device_led_process(void)
{
#if (BLT_APP_LED_ENABLE)
	if(DEVICE_LED_BUSY){
		led_proc();
	}
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* BLT_LED_H_ */
