/********************************************************************************************************
 * @file	StdRequestType.h
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
/* Includes: */
#include "tl_common.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	extern "C" {
#endif


#define CONTROL_REQTYPE_TYPE       		0x60
#define CONTROL_REQTYPE_DIRECTION  		0x80
#define CONTROL_REQTYPE_RECIPIENT  		0x1F
#define REQDIR_HOSTTODEVICE        		(0<<7)
#define REQDIR_DEVICETOHOST        		(1<<7)
#define REQTYPE_STANDARD           		(0<<5)
#define REQTYPE_CLASS              		(1<<5)
#define REQTYPE_VENDOR             		(2<<5)
#define REQREC_DEVICE              		(0<<0)
#define REQREC_INTERFACE           		(1<<0)
#define REQREC_ENDPOINT            		(2<<0)
#define REQREC_OTHER               		(3<<0)
#define FEATURE_SELFPOWERED_ENABLED     (1<<0)
#define FEATURE_REMOTE_WAKEUP_ENABLED   (1<<1)

typedef struct
{
	u8  RequestType;
	u8  Request;
	u16 Value;
	u16 Index;
	u16 Length;
}USB_Request_Hdr_t;

enum USB_Control_Request_t
{
	REQ_GetStatus,
	REQ_ClearFeature,
	REQ_SetFeature = 3,
	REQ_SetAddress = 5,
	REQ_GetDescriptor,
	REQ_SetDescriptor,
	REQ_GetConfiguration,
	REQ_SetConfiguration,
	REQ_GetInterface,
	REQ_SetInterface,
	REQ_SynchFrame,
};


enum USB_Feature_Selectors_t
{
	FEATURE_SEL_EndpointHalt,
	FEATURE_SEL_DeviceRemoteWakeup,
	FEATURE_SEL_TestMode,
};

#if defined(__cplusplus)
	}
#endif



