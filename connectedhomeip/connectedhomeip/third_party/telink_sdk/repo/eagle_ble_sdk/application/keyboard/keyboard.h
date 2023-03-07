/********************************************************************************************************
 * @file	keyboard.h
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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "drivers.h"

#define KB_RETURN_KEY_MAX	6

#define	KB_NUMLOCK_STATUS_INVALID			BIT(7)
#define	KB_NUMLOCK_STATUS_POWERON			BIT(15)

#define DEEPBACK_KEY_IDLE					0
#define DEEPBACK_KEY_CACHE					1   //there is deepback key cached in kb_event_cache
#define DEEPBACK_KEY_WAIT_RELEASE   		2


#ifndef		KB_REPEAT_KEY_ENABLE
#define		KB_REPEAT_KEY_ENABLE			0
#endif

#define KEY_NONE	  	0
#define KEY_CHANGE  	1
#define KEY_SAME		2
typedef struct{
	unsigned char key_change_flg;
	unsigned char key_repeat_flg;
	unsigned char keycode0;
	unsigned char keycode1;
	unsigned int key_change_tick;
}repeatKey_t;

extern repeatKey_t repeat_key;


typedef struct{
	unsigned char cnt;
	unsigned char ctrl_key;
	unsigned char keycode[KB_RETURN_KEY_MAX];
	//unsigned char padding[2];	//  for  32 bit padding,  if KB_RETURN_KEY_MAX change,  this should be changed
}kb_data_t;

extern kb_data_t	kb_event;
extern kb_data_t	kb_event_cache;
extern unsigned char deepback_key_state;
extern unsigned int deepback_key_tick;


#ifndef		LONG_PRESS_KEY_POWER_OPTIMIZE
#define		LONG_PRESS_KEY_POWER_OPTIMIZE		0
#endif


#ifndef		STUCK_KEY_PROCESS_ENABLE
#define		STUCK_KEY_PROCESS_ENABLE			0
#endif





int kb_is_data_same(kb_data_t *a, kb_data_t *b);

static inline int kb_is_key_valid(kb_data_t *p){
	return (p->cnt || p->ctrl_key);
}
static inline void kb_set_key_invalid(kb_data_t *p){
	p->cnt = p->ctrl_key = 0;
}


extern unsigned int kb_key_pressed(unsigned char * gpio);
extern unsigned int kb_scan_key_value (int numlock_status, int read_key,unsigned char * gpio);

extern unsigned int	scan_pin_need;


static inline unsigned int kb_scan_key (int numlock_status, int read_key) {
	unsigned char gpio[8];


	scan_pin_need = kb_key_pressed (gpio);
	if(scan_pin_need){
		return  kb_scan_key_value(numlock_status,read_key,gpio);
	}
	else{
#if (KB_REPEAT_KEY_ENABLE)
		repeat_key.key_change_flg = KEY_NONE;
#endif
		return 0;
	}
}

#ifdef __cplusplus
}
#endif
