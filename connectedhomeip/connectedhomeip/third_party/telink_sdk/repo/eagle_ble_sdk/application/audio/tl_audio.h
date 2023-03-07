/********************************************************************************************************
 * @file	tl_audio.h
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
#ifndef TL_AUDIO_H_
#define TL_AUDIO_H_

#include "tl_common.h"
#include "audio_config.h"

#if (TL_AUDIO_MODE & RCU_PROJECT)								//RCU
#ifndef		TL_NOISE_SUPRESSION_ENABLE
#define		TL_NOISE_SUPRESSION_ENABLE			0
#endif

#ifndef		IIR_FILTER_ENABLE
#define		IIR_FILTER_ENABLE			0
#endif

#ifndef		TL_MIC_PACKET_BUFFER_NUM
#define		TL_MIC_PACKET_BUFFER_NUM	4
#endif

#ifndef		MIC_SHORT_DEC_SIZE
#define		MIC_SHORT_DEC_SIZE			0
#endif

#ifndef 	ADPCM_PACKET_LEN
#define 	ADPCM_PACKET_LEN			0
#endif

#ifndef 	TL_MIC_ADPCM_UNIT_SIZE
#define 	TL_MIC_ADPCM_UNIT_SIZE		0
#endif

#ifndef		TL_MIC_BUFFER_SIZE
#define		TL_MIC_BUFFER_SIZE			0
#endif

#if TL_MIC_BUFFER_SIZE
s16		buffer_mic[TL_MIC_BUFFER_SIZE>>1] __attribute__((aligned(4)));
#endif

#if TL_NOISE_SUPRESSION_ENABLE
extern int md_long;
extern int md_short;
extern int md_im;
extern int md_noise;
extern int md_gain;

static inline int noise_supression (s16 md) {

	static int md_th =384;
	//static int md_long =0;
	//static int md_short =0;
	//static int md_im =0;
	//static int md_noise = 0;
	//static int md_gain = 256;
        md_long = ((md_long * 1023) + abs (md)) >> 10;
        md_short = ((md_short * 127) + abs (md)) >> 7;
        md_im = ((md_im * 15) + abs (md)) >> 4;

        //md_long = ((md_long * 127) + abs (md)) >> 7;
        //md_short = ((md_short * 31) + abs (md)) >> 5;
        //md_im = ((md_im * 3) + abs (md)) >> 2;

        if ((md_noise && (md_short > md_th)) || (md_im > (md_th<<1))) {
            md_noise = 0;
            md_long = md_short << 2;
        }
        else if (!md_noise && md_long < md_th) {
            md_noise = 1;
        }
        if (md_noise) {
            if (md_gain) { md_gain --; }
        }
        else {
            if (md_gain < 256) { md_gain ++;}
        }

        return (md * md_gain + 128) >> 8;
}
#endif


#ifdef __cplusplus
extern "C" {
#endif

void 	voice_iir_OOB(signed short * ps, signed short *pd, int* coef, int nsample,u8 shift);
void 	voice_iir(signed short * ps, signed short *pd, int* coef, int nsample,u8 shift);
void 	Audio_VolumeSet(unsigned char input_output_select,unsigned char volume_set_value);
void 	filter_setting();

void 	audio_mic_param_init(void);
void	proc_mic_encoder (void);
int	*	mic_encoder_data_buffer ();
void 	mic_encoder_data_read_ok (void);

#elif (TL_AUDIO_MODE & DONGLE_PROJECT)								//Dongle

void abuf_init ();
void abuf_mic_add (u32 *p);
void abuf_mic_dec ();
void abuf_dec_usb ();

#endif

#ifdef __cplusplus
}
#endif

#endif /* TL_AUDIO_H_ */
