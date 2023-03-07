/********************************************************************************************************
 * @file	audio_common.h
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
#ifndef AUDIO_COMMON_H_
#define AUDIO_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#define		TL_AUDIO_MASK_SBC_MODE							(0x00000001)
#define		TL_AUDIO_MASK_MSBC_MODE							(0x00000002)
#define		TL_AUDIO_MASK_ADPCM_MODE						(0x00000004)
#define		TL_AUDIO_MASK_OPUS_MODE							(0x00000008)

#define		TL_AUDIO_MASK_HID_SERVICE_CHANNEL				(0x00000100)
#define		TL_AUDIO_MASK_GATT_SERVICE_TELINK_CHANNEL		(0x00000200)
#define		TL_AUDIO_MASK_GATT_SERVICE_GOOGLE_CHANNEL		(0x00000400)
#define		TL_AUDIO_MASK_GATT_SERVICE_AMAZON_CHANNEL		(0x00000800)

#define		TL_AUDIO_MASK_DONGLE_TO_STB						(0x00010000)

#define		RCU_PROJECT										(0x01000000)
#define		DONGLE_PROJECT									(0x02000000)

#define		AUDIO_DISABLE									0

//RCU Audio Mode

#define		TL_AUDIO_RCU_ADPCM_GATT_TLEINK					(RCU_PROJECT | TL_AUDIO_MASK_ADPCM_MODE | TL_AUDIO_MASK_GATT_SERVICE_TELINK_CHANNEL)

#define		TL_AUDIO_RCU_ADPCM_GATT_GOOGLE					(RCU_PROJECT | TL_AUDIO_MASK_ADPCM_MODE | TL_AUDIO_MASK_GATT_SERVICE_GOOGLE_CHANNEL)

#define		TL_AUDIO_RCU_ADPCM_HID							(RCU_PROJECT | TL_AUDIO_MASK_ADPCM_MODE | TL_AUDIO_MASK_HID_SERVICE_CHANNEL)

#define		TL_AUDIO_RCU_SBC_HID							(RCU_PROJECT | TL_AUDIO_MASK_SBC_MODE | TL_AUDIO_MASK_HID_SERVICE_CHANNEL)

#define		TL_AUDIO_RCU_ADPCM_HID_DONGLE_TO_STB			(RCU_PROJECT | TL_AUDIO_MASK_ADPCM_MODE | TL_AUDIO_MASK_HID_SERVICE_CHANNEL | TL_AUDIO_MASK_DONGLE_TO_STB)

#define		TL_AUDIO_RCU_SBC_HID_DONGLE_TO_STB				(RCU_PROJECT | TL_AUDIO_MASK_SBC_MODE | TL_AUDIO_MASK_HID_SERVICE_CHANNEL | TL_AUDIO_MASK_DONGLE_TO_STB)

#define		TL_AUDIO_RCU_MSBC_HID							(RCU_PROJECT | TL_AUDIO_MASK_MSBC_MODE | TL_AUDIO_MASK_HID_SERVICE_CHANNEL)

#define		TL_AUDIO_RCU_OPUS_GATT_AMAZON					(RCU_PROJECT | TL_AUDIO_MASK_OPUS_MODE | TL_AUDIO_MASK_GATT_SERVICE_AMAZON_CHANNEL)

//Dongle Audio Mode

#define		TL_AUDIO_DONGLE_ADPCM_GATT_TELINK				(DONGLE_PROJECT | TL_AUDIO_MASK_ADPCM_MODE | TL_AUDIO_MASK_GATT_SERVICE_TELINK_CHANNEL)

#define		TL_AUDIO_DONGLE_ADPCM_GATT_GOOGLE				(DONGLE_PROJECT | TL_AUDIO_MASK_ADPCM_MODE | TL_AUDIO_MASK_GATT_SERVICE_GOOGLE_CHANNEL)

#define		TL_AUDIO_DONGLE_ADPCM_HID						(DONGLE_PROJECT | TL_AUDIO_MASK_ADPCM_MODE | TL_AUDIO_MASK_HID_SERVICE_CHANNEL)

#define		TL_AUDIO_DONGLE_SBC_HID							(DONGLE_PROJECT | TL_AUDIO_MASK_SBC_MODE | TL_AUDIO_MASK_HID_SERVICE_CHANNEL)

#define		TL_AUDIO_DONGLE_ADPCM_HID_DONGLE_TO_STB			(DONGLE_PROJECT | TL_AUDIO_MASK_ADPCM_MODE | TL_AUDIO_MASK_HID_SERVICE_CHANNEL | TL_AUDIO_MASK_DONGLE_TO_STB)

#define		TL_AUDIO_DONGLE_SBC_HID_DONGLE_TO_STB			(DONGLE_PROJECT | TL_AUDIO_MASK_SBC_MODE | TL_AUDIO_MASK_HID_SERVICE_CHANNEL | TL_AUDIO_MASK_DONGLE_TO_STB)

#define		TL_AUDIO_DONGLE_MSBC_HID						(DONGLE_PROJECT | TL_AUDIO_MASK_MSBC_MODE | TL_AUDIO_MASK_HID_SERVICE_CHANNEL)

#define		TL_AUDIO_DONGLE_OPUS_GATT_AMAZON				(DONGLE_PROJECT | TL_AUDIO_MASK_OPUS_MODE | TL_AUDIO_MASK_GATT_SERVICE_AMAZON_CHANNEL)

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_COMMON_H_ */
