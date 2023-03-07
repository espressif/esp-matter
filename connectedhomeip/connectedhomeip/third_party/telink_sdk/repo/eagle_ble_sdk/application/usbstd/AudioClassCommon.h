/********************************************************************************************************
 * @file	AudioClassCommon.h
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
#include "stdDescriptors.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#define AUDIO_CHANNEL_LEFT_FRONT           BIT(0)
#define AUDIO_CHANNEL_RIGHT_FRONT          BIT(1)
#define AUDIO_CHANNEL_CENTER_FRONT         BIT(2)
#define AUDIO_CHANNEL_LOW_FREQ_ENHANCE     BIT(3)
#define AUDIO_CHANNEL_LEFT_SURROUND        BIT(4)
#define AUDIO_CHANNEL_RIGHT_SURROUND       BIT(5)
#define AUDIO_CHANNEL_LEFT_OF_CENTER       BIT(6)
#define AUDIO_CHANNEL_RIGHT_OF_CENTER      BIT(7)
#define AUDIO_CHANNEL_SURROUND             BIT(8)
#define AUDIO_CHANNEL_SIDE_LEFT            BIT(9)
#define AUDIO_CHANNEL_SIDE_RIGHT           BIT(10)
#define AUDIO_CHANNEL_TOP                  BIT(11)

#define AUDIO_FEATURE_MUTE                 BIT(0)
#define AUDIO_FEATURE_VOLUME               BIT(1)
#define AUDIO_FEATURE_BASS                 BIT(2)
#define AUDIO_FEATURE_MID                  BIT(3)
#define AUDIO_FEATURE_TREBLE               BIT(4)
#define AUDIO_FEATURE_GRAPHIC_EQUALIZER    BIT(5)
#define AUDIO_FEATURE_AUTOMATIC_GAIN       BIT(6)
#define AUDIO_FEATURE_DELAY                BIT(7)
#define AUDIO_FEATURE_BASS_BOOST           BIT(8)
#define AUDIO_FEATURE_BASS_LOUDNESS        BIT(9)

#define AUDIO_TERMINAL_UNDEFINED           (0x100)
#define AUDIO_TERMINAL_STREAMING           (0x101)
#define AUDIO_TERMINAL_VENDOR              (0x1FF)
#define AUDIO_TERMINAL_IN_UNDEFINED        (0x200)
#define AUDIO_TERMINAL_IN_MIC              (0x201)
#define AUDIO_TERMINAL_IN_DESKTOP_MIC      (0x202)
#define AUDIO_TERMINAL_IN_PERSONAL_MIC     (0x203)
#define AUDIO_TERMINAL_IN_OMNIDIR_MIC      (0x204)
#define AUDIO_TERMINAL_IN_MIC_ARRAY        (0x205)
#define AUDIO_TERMINAL_IN_PROCESSING_MIC   (0x206)
#define AUDIO_TERMINAL_IN_OUT_UNDEFINED    (0x300)
#define AUDIO_TERMINAL_OUT_SPEAKER         (0x301)
#define AUDIO_TERMINAL_OUT_HEADPHONES      (0x302)
#define AUDIO_TERMINAL_OUT_HEAD_MOUNTED    (0x303)
#define AUDIO_TERMINAL_OUT_DESKTOP         (0x304)
#define AUDIO_TERMINAL_OUT_ROOM            (0x305)
#define AUDIO_TERMINAL_OUT_COMMUNICATION   (0x306)
#define AUDIO_TERMINAL_OUT_LOWFREQ         (0x307)

#define AUDIO_SAMPLE_FREQ(freq)            { \
	                                           .Byte1 = ((u32)freq & 0xFF), \
										   	   .Byte2 = (((u32)freq >> 8) & 0xFF), \
										   	   .Byte3 = (((u32)freq >> 16) & 0xFF) \
	                                       }


#define AUDIO_EP_FULL_PACKETS_ONLY        BIT(7)
#define AUDIO_EP_ACCEPTS_SMALL_PACKETS    (0<<7)
#define AUDIO_EP_SAMPLE_FREQ_CONTROL      BIT(0)
#define AUDIO_EP_PITCH_CONTROL            BIT(1)


enum Audio_Descriptor_ClassSubclassProtocol_t
{
	AUDIO_CSCP_StreamingProtocol = 0,
	AUDIO_CSCP_ControlProtocol = 0,
	AUDIO_CSCP_AudioClass = 1,
	AUDIO_CSCP_ControlSubclass = 1,
	AUDIO_CSCP_AudioStreamingSubclass,
	AUDIO_CSCP_MIDIStreamingSubclass,
};

enum Audio_CSInterface_AC_SubTypes_t
{
	AUDIO_DSUBTYPE_CSInterface_Header = 1,
	AUDIO_DSUBTYPE_CSInterface_InputTerminal,
	AUDIO_DSUBTYPE_CSInterface_OutputTerminal,
	AUDIO_DSUBTYPE_CSInterface_Mixer,
	AUDIO_DSUBTYPE_CSInterface_Selector,
	AUDIO_DSUBTYPE_CSInterface_Feature,
	AUDIO_DSUBTYPE_CSInterface_Processing,
	AUDIO_DSUBTYPE_CSInterface_Extension,
};

enum Audio_CSInterface_AS_SubTypes_t
{
	AUDIO_DSUBTYPE_CSInterface_General = 1,
	AUDIO_DSUBTYPE_CSInterface_FormatType,
	AUDIO_DSUBTYPE_CSInterface_FormatSpecific,
};

enum Audio_CSEndpoint_SubTypes_t
{
	AUDIO_DSUBTYPE_CSEndpoint_General = 1,
};

enum Audio_ClassRequests_t
{
	AUDIO_REQ_SetCurrent = 1,
	AUDIO_REQ_SetMinimum,
	AUDIO_REQ_SetMaximum,
	AUDIO_REQ_SetResolution,
	AUDIO_REQ_SetMemory,
	AUDIO_REQ_GetCurrent = 0x81,
	AUDIO_REQ_GetMinimum,
	AUDIO_REQ_GetMaximum,
	AUDIO_REQ_GetResolution,
	AUDIO_REQ_GetMemory,
	AUDIO_REQ_GetStatus = 0xFF,
};

enum Audio_EndpointControls_t
{
	AUDIO_EPCONTROL_SamplingFreq = 1,
	AUDIO_EPCONTROL_Pitch,
};


typedef struct
{
	USB_Descriptor_Hdr_t Header;
	u8 Subtype;
	u8 TerminalID;
	u16 TerminalType;
	u8 AssociatedOutputTerminal;
	u8 TotalChannels;
	u16 ChannelConfig;
	u8 ChannelStrIndex;
	u8 TerminalStrIndex;
}USB_Audio_Descriptor_InputTerminal_t;

typedef struct
{
	u8 Length;
	u8 DescriptorType;
	u8 DescriptorSubtype;
	u8 TerminalID;
	u16 TerminalType;
	u8 AssocTerminal;
	u8 NrChannels;
	u16 ChannelConfig;
	u8 ChannelNames;
	u8 Terminal;
}USB_Audio_StdDescriptor_InputTerminal_t;

typedef struct
{
	USB_Descriptor_Hdr_t Header;
	u8 Subtype;
	u8 TerminalID;
	u16 TerminalType;
	u8 AssociatedInputTerminal;
	u8 SourceID;
	u8 TerminalStrIndex;
}USB_Audio_Descriptor_OutputTerminal_t;

typedef struct
{
	u8 Length;
	u8 DescriptorType;
	u8 DescriptorSubtype;
	u8 TerminalID;
	u16 TerminalType;
	u8 AssocTerminal;
	u8 SourceID;
	u8 Terminal;
}USB_Audio_StdDescriptor_OutputTerminal_t;

typedef struct
{
	USB_Descriptor_Hdr_t Header;
	u8 Subtype;
	u8 ACSpecification[2];
	u8 TotalLength[2];
	u8 InCollection;
	u8 InterfaceNumber;
}USB_Audio_Descriptor_Interface_AC_t;

typedef struct
{
	USB_Descriptor_Hdr_t Header;
	u8 Subtype;
	u8 ACSpecification[2];
	u8 TotalLength[2];
	u8 InCollection;
	u8 InterfaceNumber_speaker;
	u8 InterfaceNumber_mic;
}USB_Audio_Descriptor_Interface_AC_TL_t;

typedef struct
{
	u8 Length;
	u8 DescriptorType;
	u8 DescriptorSubtype;
	u8 BcdADC[2];
	u8 TotalLength[2];
	u8 InCollection;
	u8 InterfaceNumbers;
}USB_Audio_StdDescriptor_Interface_AC_t;

typedef struct
{
	USB_Descriptor_Hdr_t Header;
	u8 Subtype;
	u8 UnitID;
	u8 SourceID;
	u8 ControlSize;
	u8 ChannelControls[3];
	u8 FeatureUnitStrIndex;
}USB_Audio_Descriptor_FeatureUnit_t;

typedef struct
{
	USB_Descriptor_Hdr_t Header;
	u8 Subtype;
	u8 UnitID;
	u8 SourceID;
	u8 ControlSize;
	u8 ChannelControls[2];
	u8 FeatureUnitStrIndex;
}USB_Audio_Descriptor_FeatureUnit_Mic_t;

typedef struct
{
	u8 Length;
	u8 DescriptorType;
	u8 DescriptorSubtype;
	u8 UnitID;
	u8 SourceID;
	u8 ControlSize;
	u8 MAControls[3];
	u8 Feature;
}USB_Audio_StdDescriptor_FeatureUnit_t;

typedef struct
{
	USB_Descriptor_Hdr_t Header;
	u8 Subtype;
	u8 TerminalLink;
	u8 FrameDelay;
	u8 AudioFormat[2];
}USB_Audio_Descriptor_Interface_AS_t;

typedef struct
{
	u8 Length;
	u8 DescriptorType;
	u8 DescriptorSubtype;
	u8 TerminalLink;
	u8 Delay;
	u16 FormatTag;
}USB_Audio_StdDescriptor_Interface_AS_t;

typedef struct
{
	USB_Descriptor_Hdr_t Header;
	u8 Subtype;
	u8 FormatType;
	u8 Channels;
	u8 SubFrameSize;
	u8 BitResolution;
	u8 TotalDiscreteSampleRates;
}USB_Audio_Descriptor_Format_t;

typedef struct
{
	u8 Byte1;
	u8 Byte2;
	u8 Byte3;
}USB_Audio_SampleFreq_t;

typedef struct
{
	u8 Length;
	u8 DescriptorType;
	u8 DescriptorSubtype;
	u8 FormatType;
	u8 NrChannels;
	u8 SubFrameSize;
	u8 BitResolution;
	u8 SampleFrequencyType;
}USB_Audio_StdDescriptor_Format_t;

typedef struct
{
	USB_Descriptor_Endpoint_t Endpoint;
	u8 Refresh;
	u8 SyncEndpointNumber;
}USB_Audio_Descriptor_StreamEndpoint_Std_t;

typedef struct
{
	u8 Length;
	u8 DescriptorType;
	u8 EndpointAddress;
	u8 MAttributes;
	u16 MaxPacketSize;
	u8 Interval;
	u8 Refresh;
	u8 SynchAddress;
}USB_Audio_StdDescriptor_StreamEndpoint_Std_t;

typedef struct
{
	USB_Descriptor_Hdr_t Header;
	u8 Subtype;
	u8 Attributes;
	u8 LockDelayUnits;
	u8 LockDelay[2];
}USB_Audio_Descriptor_StreamEndpoint_Spc_t;

typedef struct
{
	u8 Length;
	u8 DescriptorType;
	u8 DescriptorSubtype;
	u8 MAttributes;
	u8 LockDelayUnits;
	u16 LockDelay;
}USB_Audio_StdDescriptor_StreamEndpoint_Spc_t;

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

