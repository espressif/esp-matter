/********************************************************************************************************
 * @file	usbaud.h
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

#include <application/usbstd/HIDClassCommon.h>
#include <application/usbstd/HIDReportData.h>
#include "tl_common.h"

// telink usb report ctrl command. used mixed with USB_REPORT_NO_EVENT
enum {
	USB_AUD_PLAY_PAUSE		= 0,
	USB_AUD_NEXT_TRACK		= 1,
	USB_AUD_PREV_TRACK		= 2,
	USB_AUD_STOP			= 3,
	USB_AUD_PLAY			= 4,
	USB_AUD_PAUSE			= 5,
	USB_AUD_FAST_FWD		= 6,
	USB_AUD_REWIND			= 7,
	USB_AUD_VOL_INC			= 0x10,
	USB_AUD_VOL_DEC			= 0x11,
	USB_AUD_VOL_MUTE 		= 0x12,
};

#define AUDIO_FEATURE_ID_SPEAKER	0x02
#define AUDIO_FEATURE_ID_MIC		0x05

typedef struct{
	s16 speaker_vol;
	s16 mic_vol;
	s8 speaker_mute;
	s8 mic_mute;
	s8 change;
}usb_audio_status_t;


typedef struct {
	u16 vol_cur;
	u16	vol_step;
	u8 	mute;
}speaker_setting_t;


typedef struct {
	u16 vol_cur;
	u16	vol_step;
	u8 	mute;
}mic_setting_t;

#define AUDIO_VOLUME_STEP_MAX  11

#define MIC_VOL_MIN			((s16) 0x0000)     /* Volume Minimum Value */
#define MIC_VOL_MAX			((s16) 0x1e00)     /* Volume Maximum Value */
#define	MIC_VOL_RES			0x0180     /* Volume Resolution */
#define MIC_VOL_DEF			0x1800     /* Volume default */
#define	MIC_MAX_STEP		(MIC_VOL_MAX / MIC_VOL_RES)

#define	SPEAKER_VOL_MIN		((s16) 0xa000)     /* Volume Minimum Value */
#define	SPEAKER_VOL_MAX		((s16) 0x0300)     /* Volume Maximum Value */
#define SPEAKER_VOL_RES		0x0180     /* Volume Resolution */
#define SPEAKER_VOL_DEF		0x8000     /* Volume default */
#define SPEAKER_VOL_STEP	400

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

int usbaud_handle_set_speaker_cmd(int type);
int usbaud_handle_set_mic_cmd(int type);
int usbaud_handle_get_speaker_cmd(int req, int type);
int usbaud_handle_get_mic_cmd(int req, int type);
void usbaud_init(void);
u8 usbaud_speaker_vol_get(void);
u8 usbaud_mic_vol_get(void);
void usbaud_mic_en(int en);

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

