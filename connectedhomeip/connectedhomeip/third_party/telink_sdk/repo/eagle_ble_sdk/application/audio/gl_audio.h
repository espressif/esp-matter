/********************************************************************************************************
 * @file	gl_audio.h
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
#ifndef GL_AUDIO_H_
#define GL_AUDIO_H_

#include "audio_config.h"

#if (TL_AUDIO_MODE == TL_AUDIO_RCU_ADPCM_GATT_GOOGLE)

#define APP_AUDIO_ENABLE					1
#define APP_AUDIO_DISABLE					2
#define APP_AUDIO_PROCESS_ERR				0

#define APP_AUDIO_GOOGLE_TIMEOUT1   		3000000 // 3s
#define APP_AUDIO_GOOGLE_TIMEOUT2   		15000000 // 15s

#define AUDIO_GOOGLE_CMD_CAP     			0x0A
#define AUDIO_GOOGLE_CMD_OPEN    			0x0C
#define AUDIO_GOOGLE_CMD_CLOSE   			0x0D
#define AUDIO_GOOGLE_CMD_EXTEND  			0x0E


#define ATV_MIC_CHAR_RSP_CAP     			0x0B
#define ATV_MIC_CHAR_RSP_OPEN    			0x04
#define ATV_MIC_CHAR_RSP_CLOSE   			0x00
#define ATV_MIC_CHAR_RSP_SYNC    			0x0A
#define ATV_MIC_CHAR_RSP_MIC_OPEN_ERROR   	0x0C

#define APP_AUDIO_FLAG_NONE      			0x0000
#define APP_AUDIO_FLAG_T4H       			BIT(15) //bit[15]:0--google mode, 1--t4h mode
#define APP_AUDIO_FLAG_8K        			BIT(14) //bit[14]:0--16k sample, 1--8k sample
#define APP_AUDIO_FLAG_READY     			BIT(13) //bit[13]:0-google voice is not ready, 1-ready
#define APP_AUDIO_FLAG_WAIT      			BIT(12) //bit[12]:0-audio has no event to deal, 1-audio has event to deal
#define APP_AUDIO_FLAG_BLE       			BIT(11) //bit[11]:0-ble disconnect, 1-connect
#define APP_AUDIO_FLAG_PAIR      			BIT(10) //bit[10]:0-has no pair info, 1-has pair info
#define APP_AUDIO_FLAG_MULPKT    			BIT(1) //bit[1]:0-multi packet request disable, 1-enable
#define APP_AUDIO_FLAG_ENABLE    			BIT(0) //bit[0]:0-audio status(close), 1-open
#define APP_AUDIO_FLAG_SYNC      			BIT(4) //bit[4]:0-no need sync packet, 1-need
#define APP_AUDIO_FLAG_UPDATE0   			BIT(5) //bit[5]:0-no need update param, 1-need update param (7.5ms)
#define APP_AUDIO_FLAG_UPDATE1   			BIT(6) //bit[6]:0-no need update param, 1-need update param (10ms)


#define APP_AUDIO_KEY_FLAG_NONE           	0x00
#define APP_AUDIO_KEY_FLAG_PRESS          	0x80 //bit[8]:1-press, 0-release
#define APP_AUDIO_KEY_FLAG_REPORT_PRESS   	0x01
#define APP_AUDIO_KEY_FLAG_REPORT_RELEASE 	0x02
#define APP_AUDIO_KEY_FLAG_REPORT_START   	0x04
#define APP_AUDIO_KEY_FLAG_REPORT_STOP    	0x08
#define APP_AUDIO_KEY_FLAG_EVENT_MASK     	0x0F

// ref the google audio spec

typedef enum {
	 VERSION_10_HIGH = 0x01,
	 VERSION_10_LOW = 0x00,
	 VERSION_04_HIGH = 0x00,
	 VERSION_04_LOW = 0x04,
}VoiceVersion_TypeDef;

typedef enum {
	 CS_8K16B = 0x01,
	 CS_16K16B = 0x02,
	 CS_BOTH = 0x03,
}CodecsSupported_TypeDef;

typedef enum {
	 v4CS_BOTH_HIGH = 0x00,
	 v4CS_BOTH_LOW = 0x03,
}Version04CodecsSupported_TypeDef;

typedef enum {
	 v4BF_HIGH = 0x00,
	 v4BF_LOW = 0x86,
}Version04ByteFrame_TypeDef;

typedef enum {
	 ON_REQUEST = 0x00,
	 PTT = 0x01,
	 HTT = 0x03,
}AssistantInteractionModel_TypeDef;

typedef enum {
	 BYTE_20_HIGH = 0x00,
	 BYTE_20_LOW = 0x14,
	 BYTE_160_HIGH = 0x00,
	 BYTE_160_LOW = 0xA0,
}AudioFrameSize_TypeDef;

typedef enum {
	 EcSet = 0x01,
	 EcReserved = 0x00,
}ExtraConfiguration_TypeDef;


typedef enum {
	 AS_MICCLOSE = 0x00,
	 AS_RELEASE_BUTTON = 0x02,
	 AS_AUDIO_START_COME = 0x04,
	 AS_TIMEOUT = 0x08,
	 AS_DISABLE_CCC = 0x10,
	 AS_OTHERS = 0x80,

}AudioStop_TypeDef;

typedef enum {
	 REASON_MICOPEN = 0x00,
	 REASON_PTT = 0x01,
	 REASON_HTT = 0x03,
}AudioStartReason_TypeDef;



typedef enum {
	 MIC_OPEN_ERROR_HIGH = 0x0F,
	 MIC_ALREADY_OPEN_LOW = 0x01,
	 MIC_REMOTE_ERROR_LOW = 0x02,
	 MIC_DISABLE_CCC_LOW = 0x03,
	 MIC_ONGOING_LOW = 0x80,
	 MIC_INTERNAL_ERROR_LOW = 0xff,
}MicOpenError_TypeDef;

#ifdef __cplusplus
extern "C" {
#endif

unsigned char app_audio_key_start(unsigned char isPress);
int app_audio_timeout_proc(void);
int app_audio_timeout_proc(void);
int app_auido_google_callback(u16 connHandle,void* p);
void google_handle_init(u16 ctl_dp_h, u16 report_dp_h);

#ifdef __cplusplus
}
#endif

#endif

#endif /* GL_AUDIO_H_ */
