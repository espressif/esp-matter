/********************************************************************************************************
 * @file	tl_audio.c
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

#include 	"tl_common.h"
#include 	"drivers.h"
#include	"tl_audio.h"
#include	"audio_config.h"
#include	"adpcm.h"
#include    "sbc.h"




#if (TL_AUDIO_MODE & RCU_PROJECT)				//RCU

#if (TL_AUDIO_MODE & (TL_AUDIO_MASK_SBC_MODE | TL_AUDIO_MASK_MSBC_MODE))

	u8	buffer_mic_enc[(ADPCM_PACKET_LEN+3)*TL_MIC_PACKET_BUFFER_NUM];

#else
	#define	BUFFER_PACKET_SIZE		((ADPCM_PACKET_LEN >> 2) * TL_MIC_PACKET_BUFFER_NUM)

	#if BUFFER_PACKET_SIZE
	int		buffer_mic_enc[BUFFER_PACKET_SIZE];
	#endif
#endif

u8		buffer_mic_pkt_wptr;
u8		buffer_mic_pkt_rptr;


#if  TL_NOISE_SUPRESSION_ENABLE

	int md_long =0;
	int md_short =0;
	int md_im =0;
	int md_noise = 0;
	int md_gain = 256;

#endif

#if (IIR_FILTER_ENABLE)

//inner band EQ default parameter. user need to set according to actual situation.
//int filter_1[10] = {16630, -22724, 15507, 22724, -15753};
//int filter_2[10] = {15380, 0, 14450, 0, -13446};
//int filter_3[10] = {14961, 14869, 0, -13446, 0};

int filter_1[10] = {995*4, 1990*4, 995*4, 849*4, 734*4};
int filter_2[10] = {3691*4, -5564*4, 2915*4, 5564*4, -2510*4};
int filter_3[10] = {2534*4, -1482*4, 955*4,  3956*4, -1866*4};

u8  filter1_shift = 0;
u8  filter2_shift = 0;
u8  filter3_shift = 0;

//used for OOB processing. i.e LPF. user need to set according to actual situation
int LPF_FILTER_1[10] = {739,87,739,2419,-1401};
int LPF_FILTER_2[10] = {4301,5262,4299,889,-3601};

u8  lpf_filter1_shift = 0;
u8  lpf_filter2_shift = 0;


//voice data out of band need to be processed using 12 bits.
_attribute_ram_code_ void voice_iir_OOB(signed short * ps, signed short *pd, int* coef, int nsample,u8 shift)
{
	int i = 0;
	long int s = 0;
	for (i=0; i<nsample; i++)
	{
		 //s = (*ps * coef[0])>>shift;
		s = (*ps * coef[0])>>0;          		//input 16-bit
		s += coef[5] * coef[1];
		s += coef[6] * coef[2];       		//coef 0,1,2: 12-bit
		s += coef[7] * coef[3];
		s += coef[8] * coef[4];      		//coef 4 & 5: 10-bit; coef 7 & 8: 18-bit
		//s = s >> 10;                        //18-bit
		//s = s >> 12;                        //18-bit

		s = ((s + (1 << 11)) >> 12); /////this line code indicate that process sample data with 12 bits

#if 0
		if (s >= (1<<18))
			  s = (1<<18) - 1;
		else if (s < -(1<<18))
			  s = - (1<<18);
#endif
		coef[6] = coef[5];                  //16-bit
		coef[5] = *ps++;              		//16-bit
		coef[8] = coef[7];                  //18-bit
		coef[7] = s;
		//*pd++ = s >> 3;
		//*pd++ = s >> 1;

		//limit
		if(s > 32767){
			s = 32767;
		}
		else if(s < -32767){
			s = -32767;
		}

		*pd++ = s >> shift;
	}
}


_attribute_ram_code_ static inline void audio_getHalfsample_func(s16*ps, u16 len)
{
	for(u16 i = 0;i < (len >> 1);i++)
	{
		ps[i] = ps[2 * i];
	}
}

//voice data inner band need to be processed using 14 bits.
_attribute_ram_code_ void voice_iir(signed short * ps, signed short *pd, int* coef, int nsample,u8 shift)
{
	int i = 0;
	long int s = 0;
	for (i=0; i<nsample; i++)
	{
		 //s = (*ps * coef[0])>>shift;
		s = (*ps * coef[0])>>0;          		//input 16-bit
		s += coef[5] * coef[1];
		s += coef[6] * coef[2];       		//coef 0,1,2: 12-bit
		s += coef[7] * coef[3];
		s += coef[8] * coef[4];      		//coef 4 & 5: 10-bit; coef 7 & 8: 18-bit
		//s = s >> 10;                        //18-bit
		//s = s >> 12;                        //18-bit

		s = ((s + (1 << 13)) >> 14);   /////this line code indicate that process sample data with 14 bits
//		s = ((s + (1 << 11)) >> 12);
#if 0
		if (s >= (1<<18))
			  s = (1<<18) - 1;
		else if (s < -(1<<18))
			  s = - (1<<18);
#endif
		coef[6] = coef[5];                  //16-bit
		coef[5] = *ps++;              		//16-bit
		coef[8] = coef[7];                  //18-bit
		coef[7] = s;
		//*pd++ = s >> 3;
		//*pd++ = s >> 1;

		//limit
		if(s > 32767){
			s = 32767;
		}
		else if(s < -32767){
			s = -32767;
		}

		*pd++ = s >> shift;
	}
}

void Audio_VolumeSet(unsigned char input_output_select,unsigned char volume_set_value)
{
#if (BLE_DMIC_ENABLE)
	reg_mic_ctrl =    MASK_VAL( FLD_AUD_MIC_VOL_CONTROL,      	(volume_set_value & 0x3f),\
			                    FLD_AUD_MIC_MONO_EN, 	        1, \
			                    FLD_AUD_AMIC_DMIC_SELECT,    	1 );
#else
	reg_mic_ctrl =    MASK_VAL( FLD_AUD_MIC_VOL_CONTROL,      	(volume_set_value & 0x3f),\
			                    FLD_AUD_MIC_MONO_EN, 	        1, \
			                    FLD_AUD_AMIC_DMIC_SELECT,    	0 );
#endif

}
#define  IIR_FILTER_ADR  0x71000
#define  CBUFFER_SIZE    20
u8 filter_step_enable = 0;
u8 vol_gain_tmp       = 0xff;
u8 pga_post_gain_tmp  = 0xff;
u8 filter_cmp[20];
/*
 FLASH 0x71000
 +----------------0x71000
  | c1(20B) first inner band EQ
 +----------------0x71013
  | c2(20B) second inner band EQ
 +----------------0x71027
  | c3(20B) three inner band EQ
 +----------------0x7103B
  | c4(20B) first out of band EQ
 +----------------0x7104F
  | c5(20B) second out of band EQ
 +----------------0x71063
  | f1_sft(1B) | f2_sft(1B) |f3_sft(1B) |f4_sft(1B) | f5_sft(1B)	five shift
 +----------------0x7109F

 +----------------0x710A0
  | vol_gain(1B)volume gain setting
 +----------------0x710A1
  | PGA_POST_GAIN(1B)(0~49): 0x710A1
 +----------------0x710A2
*/
#define MANUAL_VOLUMN_SETTINGS			0x1C
void filter_setting()
{
	//get the configuration data in the flash
	u32 *pfilter     = (u32*)IIR_FILTER_ADR;
	u8  *p_start_iir = (u8 *)(pfilter);

	memset(filter_cmp, 0xff,sizeof(filter_cmp));

	if(memcmp(p_start_iir,filter_cmp,sizeof(filter_cmp)))//step 1 disaptch
	{
		memcpy((u8 *)filter_1,p_start_iir,CBUFFER_SIZE);
		filter_step_enable |= BIT(1);
		memset(filter_cmp, 0xff,sizeof(filter_cmp));
	}
	if(memcmp(p_start_iir+CBUFFER_SIZE,filter_cmp,sizeof(filter_cmp)))//step 2 disaptch
	{
		memcpy((u8 *)filter_2,p_start_iir+CBUFFER_SIZE,CBUFFER_SIZE);
		filter_step_enable |= BIT(2);
		memset(filter_cmp, 0xff,sizeof(filter_cmp));
	}
	if(memcmp(p_start_iir+(CBUFFER_SIZE*2),filter_cmp,sizeof(filter_cmp)))//step 3 dispatch
	{
		memcpy((u8 *)filter_3,p_start_iir+(CBUFFER_SIZE*2),CBUFFER_SIZE);
		filter_step_enable |= BIT(3);
		memset(filter_cmp, 0xff,sizeof(filter_cmp));
	}

	if(memcmp(p_start_iir+(CBUFFER_SIZE*3),filter_cmp,sizeof(filter_cmp)))//step 4 dispatch
	{
		memcpy((u8 *)LPF_FILTER_1,p_start_iir+(CBUFFER_SIZE*3),CBUFFER_SIZE);
		filter_step_enable |= BIT(4);
		memset(filter_cmp, 0xff,sizeof(filter_cmp));
	}
	if(memcmp(p_start_iir+(CBUFFER_SIZE*4),filter_cmp,sizeof(filter_cmp)))//step 5 dispatch
	{
		memcpy((u8 *)LPF_FILTER_2,p_start_iir+(CBUFFER_SIZE*4),CBUFFER_SIZE);
		filter_step_enable |= BIT(5);
		memset(filter_cmp, 0xff,sizeof(filter_cmp));
	}

	int i;
	i = CBUFFER_SIZE*5;

	filter1_shift = (p_start_iir[i] == 0xff)	?	0:p_start_iir[i];
    filter2_shift = (p_start_iir[i+1] == 0xff)	?	0:p_start_iir[i+1];
	filter3_shift = (p_start_iir[i+2] == 0xff)	?  	0:p_start_iir[i+2];

	lpf_filter1_shift = (p_start_iir[i+3] == 0xff)?	0:p_start_iir[i+3];
	lpf_filter2_shift = (p_start_iir[i+4] == 0xff)?	0:p_start_iir[i+4];

	//volume gain setting .position 0x710A0
	vol_gain_tmp = p_start_iir[0xA0];
	if(vol_gain_tmp!=0xff){
		if(vol_gain_tmp&0x80){
			if(MANUAL_VOLUMN_SETTINGS<(vol_gain_tmp&0x7f)){
				return;
			}
			Audio_VolumeSet(1,MANUAL_VOLUMN_SETTINGS-(vol_gain_tmp&0x7f));
		}else{
			if(MANUAL_VOLUMN_SETTINGS+vol_gain_tmp>0x3f){
				return;
			}
			Audio_VolumeSet(1,MANUAL_VOLUMN_SETTINGS+vol_gain_tmp);
		}
	}else{
		Audio_VolumeSet(1,MANUAL_VOLUMN_SETTINGS);
	}

	//PGA_POST_GAIN setting .position 0x710A1
#if (!BLE_DMIC_ENABLE)
	pga_post_gain_tmp = p_start_iir[0xA1];
	if(pga_post_gain_tmp != 0xff){
		if(pga_post_gain_tmp > 0x0f){//0x0f: PGA_GAIN_VOL_0_0DB
			return;
		}
		analog_write(codec_ana_cfg4,(analog_read(codec_ana_cfg4) & 0x00) | pga_post_gain_tmp);
	}
#endif
	return ;
}

#endif


#if ((TL_AUDIO_MODE == TL_AUDIO_RCU_ADPCM_HID) || (TL_AUDIO_MODE == TL_AUDIO_RCU_ADPCM_HID_DONGLE_TO_STB))//RCU,HID Service,ADPCM
extern int predict;
extern int predict_idx;

void 	audio_mic_param_init(void)
{
	predict = 0;
	predict_idx = 0;

	buffer_mic_pkt_wptr = 0;
	buffer_mic_pkt_rptr = 0;
}

_attribute_ram_code_ void	proc_mic_encoder (void)
{
	static u16	buffer_mic_rptr;
	u16 mic_wptr = (audio_get_rx_dma_wptr (DMA2) - (u32)buffer_mic) >> 1;
	u16 l = (mic_wptr >= buffer_mic_rptr) ? (mic_wptr - buffer_mic_rptr) : 0xffff;

	if ((l >=(TL_MIC_BUFFER_SIZE>>2)) && (((u8)(buffer_mic_pkt_wptr - buffer_mic_pkt_rptr) & (TL_MIC_PACKET_BUFFER_NUM * 2 - 1)) < TL_MIC_PACKET_BUFFER_NUM)) {

		s16 *ps = buffer_mic + buffer_mic_rptr;
#if 	TL_NOISE_SUPRESSION_ENABLE
        // for FIR adc sample data, only half part data are effective
		for (int i=0; i<TL_MIC_ADPCM_UNIT_SIZE*2; i++) {
			ps[i] = noise_supression (ps[i]);
        }
#endif

#if (IIR_FILTER_ENABLE)
		#if 0
			//step1: out of band voice 12bits EQ filter process
			voice_iir_OOB(ps, ps, LPF_FILTER_1, (TL_MIC_BUFFER_SIZE>>2), lpf_filter1_shift);//12 bits
			voice_iir_OOB(ps, ps, LPF_FILTER_2, (TL_MIC_BUFFER_SIZE>>2), lpf_filter2_shift);//12 bits

			//step2: 32K->16K
			audio_getHalfsample_func(ps, TL_MIC_BUFFER_SIZE>>2); //496B/2=> 248B

			//step3: inner band voice 14bits EQ filter process
			voice_iir(ps,ps,filter_1,(TL_MIC_BUFFER_SIZE>>3),filter1_shift);//14 bits
			voice_iir(ps,ps,filter_2,(TL_MIC_BUFFER_SIZE>>3),filter2_shift);//14 bits
			voice_iir(ps,ps,filter_3,(TL_MIC_BUFFER_SIZE>>3),filter3_shift);//14 bits
		#endif

		#if 0
			//step1: out of band voice 12bits EQ filter process
			if((filter_step_enable & 0x30) != 0)
			{
				if(filter_step_enable & BIT(4))
				{//DBG_C HN1_TOGGLE;
					voice_iir_OOB(ps, ps, LPF_FILTER_1, (TL_MIC_BUFFER_SIZE>>2), lpf_filter1_shift);//12 bits
				}
				if(filter_step_enable & BIT(5))
				{//DBG_C HN1_TOGGLE;
					voice_iir_OOB(ps, ps, LPF_FILTER_2, (TL_MIC_BUFFER_SIZE>>2), lpf_filter2_shift);//12 bits
				}
			}
			//step2: 32K->16K
			audio_getHalfsample_func(ps, TL_MIC_BUFFER_SIZE>>2); //496B/2=> 248B
		#endif

		//step3: inner band voice 14bits EQ filter process
		if((filter_step_enable & 0x0e) != 0)
		{
			if(filter_step_enable & BIT(1))
			{
				voice_iir(ps,ps,filter_1,(TL_MIC_BUFFER_SIZE>>2),filter1_shift);//14 bits
			}
			if(filter_step_enable & BIT(2))
			{
				voice_iir(ps,ps,filter_2,(TL_MIC_BUFFER_SIZE>>2),filter2_shift);//14 bits
			}
			if(filter_step_enable & BIT(3))
			{
				voice_iir(ps,ps,filter_3,(TL_MIC_BUFFER_SIZE>>2),filter3_shift);//14 bits
			}
		}

		// step4: Soft HPF, NONE need
#endif
		mic_to_adpcm_split (	ps,	TL_MIC_ADPCM_UNIT_SIZE,
						(s16 *)(buffer_mic_enc + (ADPCM_PACKET_LEN>>2) *
						(buffer_mic_pkt_wptr & (TL_MIC_PACKET_BUFFER_NUM - 1))), 1);

		buffer_mic_rptr = buffer_mic_rptr ? 0 : (TL_MIC_BUFFER_SIZE>>2);
		buffer_mic_pkt_wptr++;
	}
}

int	*	mic_encoder_data_buffer ()
{
	if (buffer_mic_pkt_rptr == buffer_mic_pkt_wptr) {
			return 0;
	}

	int *ps = buffer_mic_enc + (ADPCM_PACKET_LEN>>2) *
			(buffer_mic_pkt_rptr & (TL_MIC_PACKET_BUFFER_NUM - 1));


	return ps;
}

void mic_encoder_data_read_ok (void)
{
	buffer_mic_pkt_rptr++;
}


#elif ((TL_AUDIO_MODE == TL_AUDIO_RCU_SBC_HID) || (TL_AUDIO_MODE == TL_AUDIO_RCU_SBC_HID_DONGLE_TO_STB))//RCU,HID Service,SBC
void 	audio_mic_param_init(void)
{
	sbcenc_reset();
}
_attribute_ram_code_ void	proc_mic_encoder (void)
{
	static u16	buffer_mic_rptr;
	u16 mic_wptr = (audio_get_rx_dma_wptr (DMA2) - (u32)buffer_mic) >> 1;
	u16 l = (mic_wptr >= buffer_mic_rptr) ? (mic_wptr - buffer_mic_rptr) : 0xffff;

	if (l >= MIC_SHORT_DEC_SIZE) {
		u32 Temp_out_len = 0;
		s16 *ps = buffer_mic + buffer_mic_rptr;
		u8 *out = (u8 *)(buffer_mic_enc + (ADPCM_PACKET_LEN+3) * (buffer_mic_pkt_wptr & (TL_MIC_PACKET_BUFFER_NUM - 1)));

#if 	TL_NOISE_SUPRESSION_ENABLE
        // for FIR adc sample data, only half part data are effective
		for (int i=0; i<TL_MIC_ADPCM_UNIT_SIZE*2; i++) {
			ps[i] = noise_supression (ps[i]);
        }
#endif

#if (IIR_FILTER_ENABLE)
		#if 0
			//step1: out of band voice 12bits EQ filter process
			voice_iir_OOB(ps, ps, LPF_FILTER_1, (TL_MIC_BUFFER_SIZE>>2), lpf_filter1_shift);//12 bits
			voice_iir_OOB(ps, ps, LPF_FILTER_2, (TL_MIC_BUFFER_SIZE>>2), lpf_filter2_shift);//12 bits

			//step2: 32K->16K
			audio_getHalfsample_func(ps, TL_MIC_BUFFER_SIZE>>2); //496B/2=> 248B

			//step3: inner band voice 14bits EQ filter process
			voice_iir(ps,ps,filter_1,(TL_MIC_BUFFER_SIZE>>3),filter1_shift);//14 bits
			voice_iir(ps,ps,filter_2,(TL_MIC_BUFFER_SIZE>>3),filter2_shift);//14 bits
			voice_iir(ps,ps,filter_3,(TL_MIC_BUFFER_SIZE>>3),filter3_shift);//14 bits
		#endif

		#if 0
			//step1: out of band voice 12bits EQ filter process
			if((filter_step_enable & 0x30) != 0)
			{
				if(filter_step_enable & BIT(4))
				{//DBG_C HN1_TOGGLE;
					voice_iir_OOB(ps, ps, LPF_FILTER_1, (TL_MIC_BUFFER_SIZE>>2), lpf_filter1_shift);//12 bits
				}
				if(filter_step_enable & BIT(5))
				{//DBG_C HN1_TOGGLE;
					voice_iir_OOB(ps, ps, LPF_FILTER_2, (TL_MIC_BUFFER_SIZE>>2), lpf_filter2_shift);//12 bits
				}
			}
			//step2: 32K->16K
			audio_getHalfsample_func(ps, TL_MIC_BUFFER_SIZE>>2); //496B/2=> 248B
		#endif

		//step3: inner band voice 14bits EQ filter process
		if((filter_step_enable & 0x0e) != 0)
		{
			if(filter_step_enable & BIT(1))
			{
				voice_iir(ps,ps,filter_1,(TL_MIC_BUFFER_SIZE>>2),filter1_shift);//14 bits
			}
			if(filter_step_enable & BIT(2))
			{
				voice_iir(ps,ps,filter_2,(TL_MIC_BUFFER_SIZE>>2),filter2_shift);//14 bits
			}
			if(filter_step_enable & BIT(3))
			{
				voice_iir(ps,ps,filter_3,(TL_MIC_BUFFER_SIZE>>2),filter3_shift);//14 bits
			}
		}

		// step4: Soft HPF, NONE need
#endif
		sbc_enc((u8 *)ps, MIC_SHORT_DEC_SIZE << 1, out, (ADPCM_PACKET_LEN+3), &Temp_out_len);

		buffer_mic_rptr = buffer_mic_rptr ? 0 : (TL_MIC_BUFFER_SIZE>>2);
		buffer_mic_pkt_wptr ++;

		int pkts = (buffer_mic_pkt_wptr - buffer_mic_pkt_rptr) & ((TL_MIC_PACKET_BUFFER_NUM << 1) - 1);
		if (pkts > TL_MIC_PACKET_BUFFER_NUM) {  // overflow of encode
			buffer_mic_pkt_rptr++;
		}
	}
}

int	*	mic_encoder_data_buffer ()
{
	u8 tx_frame_cnt = 1;//TL_MIC_MSBC_TX_FRAME_CNT;
	int pkts = (buffer_mic_pkt_wptr - buffer_mic_pkt_rptr) & ((TL_MIC_PACKET_BUFFER_NUM << 1) - 1);

    if (pkts < tx_frame_cnt)
        return 0;

    int *ps = (int *)(buffer_mic_enc + (ADPCM_PACKET_LEN+3) *
                (buffer_mic_pkt_rptr & (TL_MIC_PACKET_BUFFER_NUM - 1)));


    return ps;
}
void mic_encoder_data_read_ok (void)
{
	buffer_mic_pkt_rptr++;
}

#elif (TL_AUDIO_MODE == TL_AUDIO_RCU_MSBC_HID)						//RCU,HID Service,MSBC

void 	audio_mic_param_init(void)
{
	sbcenc_reset();
}
void	proc_mic_encoder (void)
{
	static u16	buffer_mic_rptr;
	u16 mic_wptr = (audio_get_rx_dma_wptr (DMA2) - (u32)buffer_mic) >> 1;
	u16 l = (mic_wptr >= buffer_mic_rptr) ? (mic_wptr - buffer_mic_rptr) : 0xffff;

	if (l >= MIC_SHORT_DEC_SIZE) {
		u32 Temp_out_len = 0;
		s16 *ps = buffer_mic + buffer_mic_rptr;
		u8 *out = (u8 *)(buffer_mic_enc + (ADPCM_PACKET_LEN+3) * (buffer_mic_pkt_wptr & (TL_MIC_PACKET_BUFFER_NUM - 1)));

#if 	TL_NOISE_SUPRESSION_ENABLE
        // for FIR adc sample data, only half part data are effective
		for (int i=0; i<TL_MIC_ADPCM_UNIT_SIZE*2; i++) {
			ps[i] = noise_supression (ps[i]);
        }
#endif

#if (IIR_FILTER_ENABLE)
		#if 0
			//step1: out of band voice 12bits EQ filter process
			voice_iir_OOB(ps, ps, LPF_FILTER_1, (TL_MIC_BUFFER_SIZE>>2), lpf_filter1_shift);//12 bits
			voice_iir_OOB(ps, ps, LPF_FILTER_2, (TL_MIC_BUFFER_SIZE>>2), lpf_filter2_shift);//12 bits

			//step2: 32K->16K
			audio_getHalfsample_func(ps, TL_MIC_BUFFER_SIZE>>2); //496B/2=> 248B

			//step3: inner band voice 14bits EQ filter process
			voice_iir(ps,ps,filter_1,(TL_MIC_BUFFER_SIZE>>3),filter1_shift);//14 bits
			voice_iir(ps,ps,filter_2,(TL_MIC_BUFFER_SIZE>>3),filter2_shift);//14 bits
			voice_iir(ps,ps,filter_3,(TL_MIC_BUFFER_SIZE>>3),filter3_shift);//14 bits
		#endif

		#if 0
			//step1: out of band voice 12bits EQ filter process
			if((filter_step_enable & 0x30) != 0)
			{
				if(filter_step_enable & BIT(4))
				{//DBG_C HN1_TOGGLE;
					voice_iir_OOB(ps, ps, LPF_FILTER_1, (TL_MIC_BUFFER_SIZE>>2), lpf_filter1_shift);//12 bits
				}
				if(filter_step_enable & BIT(5))
				{//DBG_C HN1_TOGGLE;
					voice_iir_OOB(ps, ps, LPF_FILTER_2, (TL_MIC_BUFFER_SIZE>>2), lpf_filter2_shift);//12 bits
				}
			}
			//step2: 32K->16K
			audio_getHalfsample_func(ps, TL_MIC_BUFFER_SIZE>>2); //496B/2=> 248B
		#endif

		//step3: inner band voice 14bits EQ filter process
		if((filter_step_enable & 0x0e) != 0)
		{
			if(filter_step_enable & BIT(1))
			{
				voice_iir(ps,ps,filter_1,(TL_MIC_BUFFER_SIZE>>2),filter1_shift);//14 bits
			}
			if(filter_step_enable & BIT(2))
			{
				voice_iir(ps,ps,filter_2,(TL_MIC_BUFFER_SIZE>>2),filter2_shift);//14 bits
			}
			if(filter_step_enable & BIT(3))
			{
				voice_iir(ps,ps,filter_3,(TL_MIC_BUFFER_SIZE>>2),filter3_shift);//14 bits
			}
		}

		// step4: Soft HPF, NONE need
#endif
		sbc_enc((u8 *)ps, MIC_SHORT_DEC_SIZE << 1, out, ADPCM_PACKET_LEN, &Temp_out_len);

		buffer_mic_rptr = buffer_mic_rptr ? 0 : (TL_MIC_BUFFER_SIZE>>2);
		buffer_mic_pkt_wptr ++;

		int pkts = (buffer_mic_pkt_wptr - buffer_mic_pkt_rptr) & ((TL_MIC_PACKET_BUFFER_NUM << 1) - 1);
		if (pkts > TL_MIC_PACKET_BUFFER_NUM) {  // overflow of encode
			buffer_mic_pkt_rptr++;
		}
	}
}

int	*	mic_encoder_data_buffer ()
{
	if (buffer_mic_pkt_rptr == buffer_mic_pkt_wptr) {
			return 0;
	}
    int *ps = (int *)(buffer_mic_enc + (ADPCM_PACKET_LEN+3) *
                (buffer_mic_pkt_rptr & (TL_MIC_PACKET_BUFFER_NUM - 1)));


    return ps;
}
void mic_encoder_data_read_ok (void)
{
	buffer_mic_pkt_rptr++;
}

#elif (TL_AUDIO_MODE == TL_AUDIO_RCU_ADPCM_GATT_TLEINK)						//RCU,GATT Telink
void	proc_mic_encoder (void)
{
	static u16	buffer_mic_rptr;
	u16 mic_wptr = (audio_get_rx_dma_wptr (DMA2) - (u32)buffer_mic) >> 1;
	u16 l = (mic_wptr >= buffer_mic_rptr) ? (mic_wptr - buffer_mic_rptr) : 0xffff;

	if (l >=(TL_MIC_BUFFER_SIZE>>2)) {

		s16 *ps = buffer_mic + buffer_mic_rptr;
#if 	TL_NOISE_SUPRESSION_ENABLE
        // for FIR adc sample data, only half part data are effective
		for (int i=0; i<TL_MIC_ADPCM_UNIT_SIZE*2; i++) {
			ps[i] = noise_supression (ps[i]);
        }
#endif

#if (IIR_FILTER_ENABLE)
		#if 0
			//step1: out of band voice 12bits EQ filter process
			voice_iir_OOB(ps, ps, LPF_FILTER_1, (TL_MIC_BUFFER_SIZE>>2), lpf_filter1_shift);//12 bits
			voice_iir_OOB(ps, ps, LPF_FILTER_2, (TL_MIC_BUFFER_SIZE>>2), lpf_filter2_shift);//12 bits

			//step2: 32K->16K
			audio_getHalfsample_func(ps, TL_MIC_BUFFER_SIZE>>2); //496B/2=> 248B

			//step3: inner band voice 14bits EQ filter process
			voice_iir(ps,ps,filter_1,(TL_MIC_BUFFER_SIZE>>3),filter1_shift);//14 bits
			voice_iir(ps,ps,filter_2,(TL_MIC_BUFFER_SIZE>>3),filter2_shift);//14 bits
			voice_iir(ps,ps,filter_3,(TL_MIC_BUFFER_SIZE>>3),filter3_shift);//14 bits
		#endif

		#if 0
			//step1: out of band voice 12bits EQ filter process
			if((filter_step_enable & 0x30) != 0)
			{
				if(filter_step_enable & BIT(4))
				{
					voice_iir_OOB(ps, ps, LPF_FILTER_1, (TL_MIC_BUFFER_SIZE>>2), lpf_filter1_shift);//12 bits
				}
				if(filter_step_enable & BIT(5))
				{
					voice_iir_OOB(ps, ps, LPF_FILTER_2, (TL_MIC_BUFFER_SIZE>>2), lpf_filter2_shift);//12 bits
				}
			}
			//step2: 32K->16K
			audio_getHalfsample_func(ps, TL_MIC_BUFFER_SIZE>>2); //496B/2=> 248B
		#endif

		//step3: inner band voice 14bits EQ filter process
		if((filter_step_enable & 0x0e) != 0)
		{
			if(filter_step_enable & BIT(1))
			{
				voice_iir(ps,ps,filter_1,(TL_MIC_BUFFER_SIZE>>2),filter1_shift);//14 bits
			}
			if(filter_step_enable & BIT(2))
			{
				voice_iir(ps,ps,filter_2,(TL_MIC_BUFFER_SIZE>>2),filter2_shift);//14 bits
			}
			if(filter_step_enable & BIT(3))
			{
				voice_iir(ps,ps,filter_3,(TL_MIC_BUFFER_SIZE>>2),filter3_shift);//14 bits
			}
		}

		// step4: Soft HPF, NONE need
#endif
		mic_to_adpcm_split (	ps,	TL_MIC_ADPCM_UNIT_SIZE,
						(s16 *)(buffer_mic_enc + (ADPCM_PACKET_LEN>>2) *
						(buffer_mic_pkt_wptr & (TL_MIC_PACKET_BUFFER_NUM - 1))), 1);

		buffer_mic_rptr = buffer_mic_rptr ? 0 : (TL_MIC_BUFFER_SIZE>>2);
		buffer_mic_pkt_wptr++;
		int pkts = (buffer_mic_pkt_wptr - buffer_mic_pkt_rptr) & (TL_MIC_PACKET_BUFFER_NUM*2-1);
		if (pkts > TL_MIC_PACKET_BUFFER_NUM) {
			buffer_mic_pkt_rptr++;
		}
	}
}

int	*	mic_encoder_data_buffer ()
{
	if (buffer_mic_pkt_rptr == buffer_mic_pkt_wptr) {
			return 0;
	}

	int *ps = buffer_mic_enc + (ADPCM_PACKET_LEN>>2) *
			(buffer_mic_pkt_rptr & (TL_MIC_PACKET_BUFFER_NUM - 1));


	return ps;
}

void mic_encoder_data_read_ok (void)
{
	buffer_mic_pkt_rptr++;
}


#elif (TL_AUDIO_MODE == TL_AUDIO_RCU_ADPCM_GATT_GOOGLE)						//RCU,GATT GOOGLE
void	proc_mic_encoder (void)
{
	static u16	buffer_mic_rptr;
	u16 mic_wptr = (audio_get_rx_dma_wptr (DMA2) - (u32)buffer_mic) >> 1;
	u16 l = (mic_wptr >= buffer_mic_rptr) ? (mic_wptr - buffer_mic_rptr) : 0xffff;

	if (l >=(TL_MIC_BUFFER_SIZE>>2)) {

		s16 *ps = buffer_mic + buffer_mic_rptr;
#if 	TL_NOISE_SUPRESSION_ENABLE
        // for FIR adc sample data, only half part data are effective
		for (int i=0; i<TL_MIC_ADPCM_UNIT_SIZE*2; i++) {
			ps[i] = noise_supression (ps[i]);
        }
#endif

#if (IIR_FILTER_ENABLE)
		#if 0
			//step1: out of band voice 12bits EQ filter process
			voice_iir_OOB(ps, ps, LPF_FILTER_1, (TL_MIC_BUFFER_SIZE>>2), lpf_filter1_shift);//12 bits
			voice_iir_OOB(ps, ps, LPF_FILTER_2, (TL_MIC_BUFFER_SIZE>>2), lpf_filter2_shift);//12 bits

			//step2: 32K->16K
			audio_getHalfsample_func(ps, TL_MIC_BUFFER_SIZE>>2); //496B/2=> 248B

			//step3: inner band voice 14bits EQ filter process
			voice_iir(ps,ps,filter_1,(TL_MIC_BUFFER_SIZE>>3),filter1_shift);//14 bits
			voice_iir(ps,ps,filter_2,(TL_MIC_BUFFER_SIZE>>3),filter2_shift);//14 bits
			voice_iir(ps,ps,filter_3,(TL_MIC_BUFFER_SIZE>>3),filter3_shift);//14 bits
		#endif

		#if 0
			//step1: out of band voice 12bits EQ filter process
			if((filter_step_enable & 0x30) != 0)
			{
				if(filter_step_enable & BIT(4))
				{//DBG_C HN1_TOGGLE;
					voice_iir_OOB(ps, ps, LPF_FILTER_1, (TL_MIC_BUFFER_SIZE>>2), lpf_filter1_shift);//12 bits
				}
				if(filter_step_enable & BIT(5))
				{//DBG_C HN1_TOGGLE;
					voice_iir_OOB(ps, ps, LPF_FILTER_2, (TL_MIC_BUFFER_SIZE>>2), lpf_filter2_shift);//12 bits
				}
			}
			//step2: 32K->16K
			audio_getHalfsample_func(ps, TL_MIC_BUFFER_SIZE>>2); //496B/2=> 248B
		#endif

		//step3: inner band voice 14bits EQ filter process
		if((filter_step_enable & 0x0e) != 0)
		{
			if(filter_step_enable & BIT(1))
			{
				voice_iir(ps,ps,filter_1,(TL_MIC_BUFFER_SIZE>>2),filter1_shift);//14 bits
			}
			if(filter_step_enable & BIT(2))
			{
				voice_iir(ps,ps,filter_2,(TL_MIC_BUFFER_SIZE>>2),filter2_shift);//14 bits
			}
			if(filter_step_enable & BIT(3))
			{
				voice_iir(ps,ps,filter_3,(TL_MIC_BUFFER_SIZE>>2),filter3_shift);//14 bits
			}
		}

		// step4: Soft HPF, NONE need
#endif
		mic_to_adpcm_split (	ps,	TL_MIC_ADPCM_UNIT_SIZE,
						(s16 *)(buffer_mic_enc + (ADPCM_PACKET_LEN>>2) *
						(buffer_mic_pkt_wptr & (TL_MIC_PACKET_BUFFER_NUM - 1))), 1);

		buffer_mic_rptr = buffer_mic_rptr ? 0 : (TL_MIC_BUFFER_SIZE>>2);
		buffer_mic_pkt_wptr++;
		int pkts = (buffer_mic_pkt_wptr - buffer_mic_pkt_rptr) & (TL_MIC_PACKET_BUFFER_NUM*2-1);
		if (pkts > TL_MIC_PACKET_BUFFER_NUM) {
			buffer_mic_pkt_rptr++;
		}

	}
}

int	*	mic_encoder_data_buffer ()
{
	if (buffer_mic_pkt_rptr == buffer_mic_pkt_wptr) {
			return 0;
	}

	int *ps = buffer_mic_enc + (ADPCM_PACKET_LEN>>2) *
			(buffer_mic_pkt_rptr & (TL_MIC_PACKET_BUFFER_NUM - 1));


	return ps;
}

void mic_encoder_data_read_ok (void)
{
	buffer_mic_pkt_rptr++;
}
#else

#endif

#elif (TL_AUDIO_MODE & DONGLE_PROJECT)								//Dongle

#if (TL_AUDIO_MODE == TL_AUDIO_DONGLE_ADPCM_GATT_TELINK)			//Dongle, GATT Telink
u8		abuf_mic_wptr, abuf_dec_wptr;
u16		abuf_dec_rptr;

#define DEC_BUFFER_SIZE		(MIC_SHORT_DEC_SIZE<<2)
#define	PACK_POINTER		(abuf_dec_rptr/248) | (abuf_dec_wptr<<8) | (abuf_mic_wptr<<16)
#define	USB_ISO_IN_SIZE		(MIC_SAMPLE_RATE / 1000)

u8		abuf_mic[MIC_ADPCM_FRAME_SIZE * 4];
s16		abuf_dec[DEC_BUFFER_SIZE];

int		abuf_reset = 0;

void abuf_init ()
{
	abuf_mic_wptr = abuf_dec_wptr = 0;
	abuf_reset = 16;
}

void abuf_mic_add (u32 *p)
{
	u32 *pd = (u32 *) (abuf_mic + (abuf_mic_wptr & 3) * MIC_ADPCM_FRAME_SIZE);
	for (int i=0; i<(MIC_ADPCM_FRAME_SIZE>>2); i++)
	{
		*pd ++ = *p++;
	}
	abuf_mic_wptr ++;
}

void abuf_mic_dec ()
{
	static int start = 1;
	static int abuf_reset_no;
	if (abuf_reset)
	{
		abuf_dec_wptr = abuf_mic_wptr;
	}
	else
	{
		u8 num_mic = abuf_mic_wptr - abuf_dec_wptr;
		u8 num_dec = abuf_dec_wptr - (abuf_dec_rptr/MIC_SHORT_DEC_SIZE);

		if (num_mic > 4) 			// in case of overflow
		{
			log_data (TR_24_abuf_dec_rptr, PACK_POINTER);
			abuf_dec_wptr ++;
		}

		if (num_dec > 4)
		{
			//log_event (TR_T_abuf_overflow_dec);
			//log_data (TR_24_abuf_dec_wptr, (abuf_dec_rptr>>8) | (abuf_dec_wptr<<8) | (abuf_mic_wptr<<16));

			abuf_reset = 16;
			start = 1;
			abuf_reset_no++;
		}
		else if ( ((!start && num_mic>=1) || (start && num_mic>=2)) && (num_dec <= 3) )
		{
			log_data (TR_24_abuf_dec_wptr, PACK_POINTER);
			adpcm_to_pcm (
					(s16 *) (abuf_mic + (abuf_dec_wptr & 3) * MIC_ADPCM_FRAME_SIZE),
					abuf_dec + (abuf_dec_wptr & 3) * MIC_SHORT_DEC_SIZE,
					MIC_SHORT_DEC_SIZE );

			abuf_dec_wptr ++;			// 256-byte = 128-s16
			start = 0;
		}
	}
}


_attribute_ram_code_ void abuf_dec_usb ()
{
//	log_event (TR_T_abuf_usb);
//	log_data (TR_24_abuf_dec_rptr, (abuf_dec_rptr/248) | (abuf_dec_wptr<<8) | (abuf_mic_wptr<<16));
//	log_data (TR_24_abuf_reset, abuf_reset);
	static u32 tick_usb_iso_in;
	static u8  buffer_empty = 1;
	static u8  n_usb_iso = 0;

	n_usb_iso++;

	if (clock_time_exceed (tick_usb_iso_in, 4000))
	{
		abuf_reset = 16;
	}

	tick_usb_iso_in = clock_time ();
	if (abuf_reset)
	{
		abuf_dec_rptr = abuf_dec_wptr*MIC_SHORT_DEC_SIZE;
		abuf_reset--;
	}
	/////////////////// copy data to usb iso in buffer ///////////////
	reg_usb_ep7_ptr = 0;
	u8 num = abuf_dec_wptr - (abuf_dec_rptr/MIC_SHORT_DEC_SIZE);
	if (num)
	{
		if ( (buffer_empty && num >= 3) || (!buffer_empty && (num >= 1 || (n_usb_iso & 3))) )
		{
			buffer_empty = 0;

			u16 offset = abuf_dec_rptr%DEC_BUFFER_SIZE;
			s16 *ps = abuf_dec + offset;


			if(offset == DEC_BUFFER_SIZE - (USB_ISO_IN_SIZE/2)){
				for (int i=0; i<(USB_ISO_IN_SIZE/2); i++)
				{
					reg_usb_ep7_dat = *ps;
					reg_usb_ep7_dat = *ps++ >> 8;
				}
				ps = abuf_dec;
				for (int i=0; i<(USB_ISO_IN_SIZE/2); i++)
				{
					reg_usb_ep7_dat = *ps;
					reg_usb_ep7_dat = *ps++ >> 8;
				}
			}
			else{
				for (int i=0; i<USB_ISO_IN_SIZE; i++)
				{
					reg_usb_ep7_dat = *ps;
					reg_usb_ep7_dat = *ps++ >> 8;
				}
			}


			abuf_dec_rptr += USB_ISO_IN_SIZE;
			if(abuf_dec_rptr >= (MIC_SHORT_DEC_SIZE<<8) ){
				abuf_dec_rptr = 0;
			}
		}
		else
		{
			for (int i=0; i<USB_ISO_IN_SIZE * 2; i++)
			{
				reg_usb_ep7_dat = 0;
			}
		}
	}
	else
	{
		for (int i=0; i<USB_ISO_IN_SIZE * 2; i++)
		{
			reg_usb_ep7_dat = 0;
		}
		buffer_empty = 1;
	}
	reg_usb_ep7_ctrl = BIT(0);			//ACK iso in
}

#elif (TL_AUDIO_MODE == TL_AUDIO_DONGLE_ADPCM_GATT_GOOGLE)				//Dongle, GATT GOOGLE

u8		abuf_mic_wptr, abuf_dec_wptr;
u16		abuf_dec_rptr;

#define DEC_BUFFER_SIZE			(MIC_SHORT_DEC_SIZE<<2)
#define	PACK_POINTER			(abuf_dec_rptr/248) | (abuf_dec_wptr<<8) | (abuf_mic_wptr<<16)
#define	USB_ISO_IN_SIZE			(MIC_SAMPLE_RATE / 1000)
#define MIC_ADPCM_FRAME_NUM		4

u8		abuf_mic[MIC_ADPCM_FRAME_SIZE * 4];
s16		abuf_dec[DEC_BUFFER_SIZE];

int		abuf_reset = 0;

void abuf_init ()
{
	abuf_mic_wptr = abuf_dec_wptr = 0;
	abuf_reset = 16;
}

void abuf_mic_add (u32 *p)
{
	u8 *src;
	src = (u8*)p;
	u8 *pd = (u8 *) (abuf_mic + (abuf_mic_wptr & (MIC_ADPCM_FRAME_NUM - 1)) * MIC_ADPCM_FRAME_SIZE);
	for (int i=0; i<(MIC_ADPCM_FRAME_SIZE); i++)
	{
		*pd ++ = *src++;
	}
	abuf_mic_wptr ++;
}

void abuf_mic_dec ()
{
	static int start = 1;
	static int abuf_reset_no;
	static u32 smbc_decode_len = 0;
	if (abuf_reset)
	{
		abuf_dec_wptr = abuf_mic_wptr;
	}
	else
	{
		u8 num_mic = abuf_mic_wptr - abuf_dec_wptr;
		u8 num_dec = abuf_dec_wptr - (abuf_dec_rptr/MIC_SHORT_DEC_SIZE);

		if (num_mic > MIC_ADPCM_FRAME_NUM) 			// in case of overflow
		{
			log_data (TR_24_abuf_dec_rptr, PACK_POINTER);
			abuf_dec_wptr ++;
		}

		if (num_dec > MIC_ADPCM_FRAME_NUM)
		{
			//log_event (TR_T_abuf_overflow_dec);
			//log_data (TR_24_abuf_dec_wptr, (abuf_dec_rptr>>8) | (abuf_dec_wptr<<8) | (abuf_mic_wptr<<16));

			abuf_reset = 16;
			start = 1;
			abuf_reset_no++;
		}
		else if ( ((!start && num_mic>=1) || (start && num_mic>=2)) && (num_dec <= (MIC_ADPCM_FRAME_NUM - 1)) )
		{
			log_data (TR_24_abuf_dec_wptr, PACK_POINTER);
			adpcm_to_pcm (
					(s16 *) (abuf_mic + (abuf_dec_wptr & 3) * MIC_ADPCM_FRAME_SIZE),
					abuf_dec + (abuf_dec_wptr & 3) * MIC_SHORT_DEC_SIZE,
					MIC_SHORT_DEC_SIZE );

			abuf_dec_wptr ++;			// 256-byte = 128-s16
			start = 0;
		}
	}
}


_attribute_ram_code_ void abuf_dec_usb ()
{
//	log_event (TR_T_abuf_usb);
//	log_data (TR_24_abuf_dec_rptr, (abuf_dec_rptr/248) | (abuf_dec_wptr<<8) | (abuf_mic_wptr<<16));
//	log_data (TR_24_abuf_reset, abuf_reset);
	static u32 tick_usb_iso_in;
	static u8  buffer_empty = 1;
	static u8  n_usb_iso = 0;

	n_usb_iso++;

	if (clock_time_exceed (tick_usb_iso_in, 4000))
	{
		abuf_reset = 16;
	}

	tick_usb_iso_in = clock_time ();
	if (abuf_reset)
	{
		abuf_dec_rptr = abuf_dec_wptr*MIC_SHORT_DEC_SIZE;
		abuf_reset--;
	}
	/////////////////// copy data to usb iso in buffer ///////////////
	reg_usb_ep7_ptr = 0;
	u8 num = abuf_dec_wptr - (abuf_dec_rptr/MIC_SHORT_DEC_SIZE);
	if (num)
	{
		if ( (buffer_empty && num >= 3) || (!buffer_empty && (num >= 1 || (n_usb_iso & 3))) )
		{
			buffer_empty = 0;

			u16 offset = abuf_dec_rptr%DEC_BUFFER_SIZE;
			s16 *ps = abuf_dec + offset;


			if(offset == DEC_BUFFER_SIZE - (USB_ISO_IN_SIZE/2)){
				for (int i=0; i<(USB_ISO_IN_SIZE/2); i++)
				{
					reg_usb_ep7_dat = *ps;
					reg_usb_ep7_dat = *ps++ >> 8;
				}
				ps = abuf_dec;
				for (int i=0; i<(USB_ISO_IN_SIZE/2); i++)
				{
					reg_usb_ep7_dat = *ps;
					reg_usb_ep7_dat = *ps++ >> 8;
				}
			}
			else{
				for (int i=0; i<USB_ISO_IN_SIZE; i++)
				{
					reg_usb_ep7_dat = *ps;
					reg_usb_ep7_dat = *ps++ >> 8;
				}
			}


			abuf_dec_rptr += USB_ISO_IN_SIZE;
			if(abuf_dec_rptr >= (MIC_SHORT_DEC_SIZE<<8) ){
				abuf_dec_rptr = 0;
			}
		}
		else
		{
//			for (int i=0; i<USB_ISO_IN_SIZE * 2; i++)
//			{
//				reg_usb_ep7_dat = 0;
//			}
		}
	}
	else
	{
//		for (int i=0; i<USB_ISO_IN_SIZE * 2; i++)
//		{
//			reg_usb_ep7_dat = 0;
//		}
		buffer_empty = 1;
	}
	reg_usb_ep7_ctrl = BIT(0);			//ACK iso in
}



#elif (TL_AUDIO_MODE == TL_AUDIO_DONGLE_ADPCM_HID_DONGLE_TO_STB)			//Dongle, HID Service,ADPCM,DONGLE TO STB, STB decode

#elif (TL_AUDIO_MODE == TL_AUDIO_DONGLE_ADPCM_HID)							//Dongle, HID Service,ADPCM,DONGLE decode
u8		abuf_mic_wptr, abuf_dec_wptr;
u16		abuf_dec_rptr;

#define DEC_BUFFER_SIZE		(MIC_SHORT_DEC_SIZE<<2)
#define	PACK_POINTER		(abuf_dec_rptr/248) | (abuf_dec_wptr<<8) | (abuf_mic_wptr<<16)
#define	USB_ISO_IN_SIZE		(MIC_SAMPLE_RATE / 1000)

u8		abuf_mic[MIC_ADPCM_FRAME_SIZE * 8];
s16		abuf_dec[DEC_BUFFER_SIZE];

int		abuf_reset = 0;

extern int predict;
extern int predict_idx;

u8 mic_cnt = 0;
void abuf_init ()
{
	abuf_mic_wptr = abuf_dec_wptr = 0;
	abuf_reset = 16;

	mic_cnt = 0;
	predict  = 0;
	predict_idx = 0;

}
void abuf_mic_add (u32 *p)
{
	u32 *pd = (u32 *) (abuf_mic + (abuf_mic_wptr & 0x07) * MIC_ADPCM_FRAME_SIZE);
	for (int i=0; i<(MIC_ADPCM_FRAME_SIZE>>2); i++)
	{
		*pd ++ = *p++;
	}
	abuf_mic_wptr ++;
}

void abuf_mic_dec ()
{
	static int start = 1;
	static int abuf_reset_no;

		u8 num_mic = (u8)(abuf_mic_wptr - abuf_dec_wptr);
		u8 num_dec = (u8)(abuf_dec_wptr - (abuf_dec_rptr/MIC_SHORT_DEC_SIZE));

		if (num_mic > 8) 			// in case of overflow
		{
			log_data (TR_24_abuf_dec_rptr, PACK_POINTER);
//			abuf_dec_wptr ++;
		}

		if (num_dec > 4)
		{
			//log_event (TR_T_abuf_overflow_dec);
			//log_data (TR_24_abuf_dec_wptr, (abuf_dec_rptr>>8) | (abuf_dec_wptr<<8) | (abuf_mic_wptr<<16));

			abuf_dec_rptr = abuf_dec_wptr*MIC_SHORT_DEC_SIZE;
			num_dec = 0;
//			abuf_reset = 16;
//			start = 1;
			abuf_reset_no++;
		}

		if ( ((!start && num_mic>=1) || (start && num_mic>=2)) )
		{
			log_data (TR_24_abuf_dec_wptr, PACK_POINTER);
			adpcm_to_pcm (
					(s16 *) (abuf_mic + (abuf_dec_wptr & 7) * MIC_ADPCM_FRAME_SIZE),
					abuf_dec + (abuf_dec_wptr & 3) * MIC_SHORT_DEC_SIZE,
					MIC_SHORT_DEC_SIZE );

			abuf_dec_wptr ++;			// 256-byte = 128-s16
			start = 0;
		}

}


_attribute_ram_code_ void abuf_dec_usb ()
{
//	log_event (TR_T_abuf_usb);
//	log_data (TR_24_abuf_dec_rptr, (abuf_dec_rptr/248) | (abuf_dec_wptr<<8) | (abuf_mic_wptr<<16));
//	log_data (TR_24_abuf_reset, abuf_reset);
	static u32 tick_usb_iso_in;
	static u8  buffer_empty = 1;
	static u8  n_usb_iso = 0;

	n_usb_iso++;

	if (clock_time_exceed (tick_usb_iso_in, 4000))
	{
		abuf_reset = 16;
	}

	tick_usb_iso_in = clock_time ();
	if (abuf_reset)
	{
		abuf_dec_rptr = abuf_dec_wptr*MIC_SHORT_DEC_SIZE;
		abuf_reset--;
	}
	/////////////////// copy data to usb iso in buffer ///////////////
	reg_usb_ep7_ptr = 0;
	u8 num = abuf_dec_wptr - (abuf_dec_rptr/MIC_SHORT_DEC_SIZE);
	if (num)
	{
		if ( (buffer_empty && num >= 3) || (!buffer_empty && (num >= 1 || (n_usb_iso & 3))) )
		{
			buffer_empty = 0;

			u16 offset = abuf_dec_rptr%DEC_BUFFER_SIZE;
			s16 *ps = abuf_dec + offset;


			if(offset == DEC_BUFFER_SIZE - (USB_ISO_IN_SIZE/2)){
				for (int i=0; i<(USB_ISO_IN_SIZE/2); i++)
				{
					reg_usb_ep7_dat = *ps;
					reg_usb_ep7_dat = *ps++ >> 8;
				}
				ps = abuf_dec;
				for (int i=0; i<(USB_ISO_IN_SIZE/2); i++)
				{
					reg_usb_ep7_dat = *ps;
					reg_usb_ep7_dat = *ps++ >> 8;
				}
			}
			else{
				for (int i=0; i<USB_ISO_IN_SIZE; i++)
				{
					reg_usb_ep7_dat = *ps;
					reg_usb_ep7_dat = *ps++ >> 8;
				}
			}


			abuf_dec_rptr += USB_ISO_IN_SIZE;
			if(abuf_dec_rptr >= (MIC_SHORT_DEC_SIZE<<8) ){
				abuf_dec_rptr = 0;
			}
		}
		else
		{
//			for (int i=0; i<USB_ISO_IN_SIZE * 2; i++)
//			{
//				reg_usb_ep7_dat = 0;
//			}
		}
	}
	else
	{
//		for (int i=0; i<USB_ISO_IN_SIZE * 2; i++)
//		{
//			reg_usb_ep7_dat = 0;
//		}
		buffer_empty = 1;
	}
	reg_usb_ep7_ctrl = BIT(0);			//ACK iso in
}
#elif (TL_AUDIO_MODE == TL_AUDIO_DONGLE_SBC_HID_DONGLE_TO_STB)					//Dongle,HID Service,SBC,DONGLE TO STB, STB decode


#elif (TL_AUDIO_MODE == TL_AUDIO_DONGLE_SBC_HID)								//Dongle,HID Service,SBC,DONGLE decode

u8		abuf_mic_wptr, abuf_dec_wptr;
u16		abuf_dec_rptr;
#define	MIC_ADPCM_FRAME_NUM		8
#define DEC_BUFFER_SIZE		(MIC_SHORT_DEC_SIZE* MIC_ADPCM_FRAME_NUM)
#define	PACK_POINTER		(abuf_dec_rptr/248) | (abuf_dec_wptr<<8) | (abuf_mic_wptr<<16)
#define	USB_ISO_IN_SIZE		(MIC_SAMPLE_RATE / 1000)

u8		abuf_mic[MIC_ADPCM_FRAME_SIZE * MIC_ADPCM_FRAME_NUM];
s16		abuf_dec[DEC_BUFFER_SIZE];

int		abuf_reset = 0;

void abuf_init ()
{
	abuf_mic_wptr = abuf_dec_wptr = 0;
	abuf_reset = 16;
}
void abuf_mic_add (u32 *p)
{
	u8 *src;
	src = (u8*)p;
	u8 *pd = (u8 *) (abuf_mic + (abuf_mic_wptr & (MIC_ADPCM_FRAME_NUM-1)) * MIC_ADPCM_FRAME_SIZE);
	for (int i=0; i<(MIC_ADPCM_FRAME_SIZE); i++)
	{
		*pd ++ = *src++;
	}
//	*pd = 0;
	abuf_mic_wptr ++;
}

_attribute_ram_code_ void abuf_mic_dec ()
{
	static int start = 1;
	static int abuf_reset_no;
	static u32 smbc_decode_len = 0;
	if (abuf_reset)
	{
		abuf_dec_wptr = abuf_mic_wptr;
	}
	else
	{
		u8 num_mic = (u8)(abuf_mic_wptr - abuf_dec_wptr);
//		u8 num_dec = (u8)(abuf_dec_wptr - (abuf_dec_rptr/MIC_SHORT_DEC_SIZE));

		if (num_mic > MIC_ADPCM_FRAME_NUM) 			// in case of overflow
		{
			log_data (TR_24_abuf_dec_rptr, PACK_POINTER);
			abuf_dec_wptr ++;
		}

//		if (num_dec > MIC_ADPCM_FRAME_NUM)
//		{
//			//log_event (TR_T_abuf_overflow_dec);
//			//log_data (TR_24_abuf_dec_wptr, (abuf_dec_rptr>>8) | (abuf_dec_wptr<<8) | (abuf_mic_wptr<<16));
//
//			log_event (TR_T_abuf_overflow_dec);
//			abuf_dec_rptr = abuf_dec_wptr*MIC_SHORT_DEC_SIZE;
//			abuf_reset = 16;
//			start = 1;
//			abuf_reset_no++;
//		}

//		if ( ((!start && num_mic>=1) || (start && num_mic>=2))&& (num_dec <= (MIC_ADPCM_FRAME_NUM-1)))
		if ( ((!start && num_mic>=1) || (start && num_mic>=2)))
		{
			log_data (TR_24_abuf_dec_wptr, PACK_POINTER);
			sbc_decode(abuf_mic + (abuf_dec_wptr & (MIC_ADPCM_FRAME_NUM - 1)) * MIC_ADPCM_FRAME_SIZE, MIC_ADPCM_FRAME_SIZE,
								abuf_dec + (abuf_dec_wptr & (MIC_ADPCM_FRAME_NUM - 1)) * MIC_SHORT_DEC_SIZE, MIC_SHORT_DEC_SIZE * 2, &smbc_decode_len);

			abuf_dec_wptr ++;			// 256-byte = 128-s16
			start = 0;
		}

	}
}

_attribute_ram_code_ void abuf_dec_usb ()
{
//	log_event (TR_T_abuf_usb);
//	log_data (TR_24_abuf_dec_rptr, (abuf_dec_rptr/248) | (abuf_dec_wptr<<8) | (abuf_mic_wptr<<16));
//	log_data (TR_24_abuf_reset, abuf_reset);
	static u32 tick_usb_iso_in;
	static u8  buffer_empty = 1;
	static u8  n_usb_iso = 0;

	n_usb_iso++;

	if (clock_time_exceed (tick_usb_iso_in, 4000))
	{
		abuf_reset = 16;
	}

	tick_usb_iso_in = clock_time ();
	if (abuf_reset)
	{
		abuf_dec_rptr = abuf_dec_wptr*MIC_SHORT_DEC_SIZE;
		abuf_reset--;
	}
	/////////////////// copy data to usb iso in buffer ///////////////
	reg_usb_ep7_ptr = 0;
	u8 num = abuf_dec_wptr - (abuf_dec_rptr/MIC_SHORT_DEC_SIZE);
	if (num)
	{
		if ( (buffer_empty && num >= (MIC_ADPCM_FRAME_NUM - 1)) || (!buffer_empty && (num >= 1 || (n_usb_iso & (MIC_ADPCM_FRAME_NUM - 1)))) )
		{
			buffer_empty = 0;

			u16 offset = abuf_dec_rptr%DEC_BUFFER_SIZE;
			s16 *ps = abuf_dec + offset;


			if(offset == DEC_BUFFER_SIZE - (USB_ISO_IN_SIZE/2)){
				for (int i=0; i<(USB_ISO_IN_SIZE/2); i++)
				{
					reg_usb_ep7_dat = *ps;
					reg_usb_ep7_dat = *ps++ >> 8;
				}
				ps = abuf_dec;
				for (int i=0; i<(USB_ISO_IN_SIZE/2); i++)
				{
					reg_usb_ep7_dat = *ps;
					reg_usb_ep7_dat = *ps++ >> 8;
				}
			}
			else{
				for (int i=0; i<USB_ISO_IN_SIZE; i++)
				{
					reg_usb_ep7_dat = *ps;
					reg_usb_ep7_dat = *ps++ >> 8;
				}
			}


			abuf_dec_rptr += USB_ISO_IN_SIZE;
			if(abuf_dec_rptr >= (MIC_SHORT_DEC_SIZE<<8) ){
				abuf_dec_rptr = 0;
			}
		}
		else
		{
//			for (int i=0; i<USB_ISO_IN_SIZE * 2; i++)
//			{
//				reg_usb_ep7_dat = 0;
//			}
		}
	}
	else
	{
//		for (int i=0; i<USB_ISO_IN_SIZE * 2; i++)
//		{
//			reg_usb_ep7_dat = 0;
//		}
		buffer_empty = 1;
	}
	reg_usb_ep7_ctrl = BIT(0);			//ACK iso in
}

#elif (TL_AUDIO_MODE == TL_AUDIO_DONGLE_MSBC_HID)						//Dongle,HID Service,MSBC,DONGLE decode

u8		abuf_mic_wptr, abuf_dec_wptr;
u16		abuf_dec_rptr;
#define	MIC_ADPCM_FRAME_NUM		8
#define DEC_BUFFER_SIZE		(MIC_SHORT_DEC_SIZE* MIC_ADPCM_FRAME_NUM)
#define	PACK_POINTER		(abuf_dec_rptr/248) | (abuf_dec_wptr<<8) | (abuf_mic_wptr<<16)
#define	USB_ISO_IN_SIZE		(MIC_SAMPLE_RATE / 1000)

u8		abuf_mic[MIC_ADPCM_FRAME_SIZE * MIC_ADPCM_FRAME_NUM];
s16		abuf_dec[DEC_BUFFER_SIZE];

int		abuf_reset = 0;

void abuf_init ()
{
	abuf_mic_wptr = abuf_dec_wptr = 0;
	abuf_reset = 16;
}
void abuf_mic_add (u32 *p)
{
	u8 *src;
	src = (u8*)p;
	u8 *pd = (u8 *) (abuf_mic + (abuf_mic_wptr & (MIC_ADPCM_FRAME_NUM-1)) * MIC_ADPCM_FRAME_SIZE);
	for (int i=0; i<(MIC_ADPCM_FRAME_SIZE); i++)
	{
		*pd ++ = *src++;
	}
//	*pd = 0;
	abuf_mic_wptr ++;
}

_attribute_ram_code_ void abuf_mic_dec ()
{
	static int start = 1;
	static int abuf_reset_no;
	static u32 smbc_decode_len = 0;
	if (abuf_reset)
	{
		abuf_dec_wptr = abuf_mic_wptr;
	}
	else
	{
		u8 num_mic = (u8)(abuf_mic_wptr - abuf_dec_wptr);
//		u8 num_dec = (u8)(abuf_dec_wptr - (abuf_dec_rptr/MIC_SHORT_DEC_SIZE));

		if (num_mic > MIC_ADPCM_FRAME_NUM) 			// in case of overflow
		{
			log_data (TR_24_abuf_dec_rptr, PACK_POINTER);
			abuf_dec_wptr ++;
		}

//		if (num_dec > MIC_ADPCM_FRAME_NUM)
//		{
//			//log_event (TR_T_abuf_overflow_dec);
//			//log_data (TR_24_abuf_dec_wptr, (abuf_dec_rptr>>8) | (abuf_dec_wptr<<8) | (abuf_mic_wptr<<16));
//
//			log_event (TR_T_abuf_overflow_dec);
//			abuf_dec_rptr = abuf_dec_wptr*MIC_SHORT_DEC_SIZE;
//			abuf_reset = 16;
//			start = 1;
//			abuf_reset_no++;
//		}

//		if ( ((!start && num_mic>=1) || (start && num_mic>=2))&& (num_dec <= (MIC_ADPCM_FRAME_NUM-1)))
		if ( ((!start && num_mic>=1) || (start && num_mic>=2)))
		{
			sbc_decode(abuf_mic + (abuf_dec_wptr & (MIC_ADPCM_FRAME_NUM - 1)) * MIC_ADPCM_FRAME_SIZE, MIC_ADPCM_FRAME_SIZE,
								abuf_dec + (abuf_dec_wptr & (MIC_ADPCM_FRAME_NUM - 1)) * MIC_SHORT_DEC_SIZE, MIC_SHORT_DEC_SIZE * 2, &smbc_decode_len);

			abuf_dec_wptr ++;			// 256-byte = 128-s16
			start = 0;
		}

	}
}

_attribute_ram_code_ void abuf_dec_usb ()
{
//	log_event (TR_T_abuf_usb);
//	log_data (TR_24_abuf_dec_rptr, (abuf_dec_rptr/248) | (abuf_dec_wptr<<8) | (abuf_mic_wptr<<16));
//	log_data (TR_24_abuf_reset, abuf_reset);
	static u32 tick_usb_iso_in;
	static u8  buffer_empty = 1;
	static u8  n_usb_iso = 0;

	n_usb_iso++;

	if (clock_time_exceed (tick_usb_iso_in, 4000))
	{
		abuf_reset = 16;
	}

	tick_usb_iso_in = clock_time ();
	if (abuf_reset)
	{
		abuf_dec_rptr = abuf_dec_wptr*MIC_SHORT_DEC_SIZE;
		abuf_reset--;
	}
	/////////////////// copy data to usb iso in buffer ///////////////
	reg_usb_ep7_ptr = 0;
	u8 num = abuf_dec_wptr - (abuf_dec_rptr/MIC_SHORT_DEC_SIZE);
	if (num)
	{
		if ( (buffer_empty && num >= (MIC_ADPCM_FRAME_NUM - 1)) || (!buffer_empty && (num >= 1 || (n_usb_iso & (MIC_ADPCM_FRAME_NUM - 1)))) )
		{
			buffer_empty = 0;

			u16 offset = abuf_dec_rptr%DEC_BUFFER_SIZE;
			s16 *ps = abuf_dec + offset;


			if(offset == DEC_BUFFER_SIZE - (USB_ISO_IN_SIZE/2)){
				for (int i=0; i<(USB_ISO_IN_SIZE/2); i++)
				{
					reg_usb_ep7_dat = *ps;
					reg_usb_ep7_dat = *ps++ >> 8;
				}
				ps = abuf_dec;
				for (int i=0; i<(USB_ISO_IN_SIZE/2); i++)
				{
					reg_usb_ep7_dat = *ps;
					reg_usb_ep7_dat = *ps++ >> 8;
				}
			}
			else{
				for (int i=0; i<USB_ISO_IN_SIZE; i++)
				{
					reg_usb_ep7_dat = *ps;
					reg_usb_ep7_dat = *ps++ >> 8;
				}
			}


			abuf_dec_rptr += USB_ISO_IN_SIZE;
			if(abuf_dec_rptr >= (MIC_SHORT_DEC_SIZE<<8) ){
				abuf_dec_rptr = 0;
			}
		}
		else
		{
//			for (int i=0; i<USB_ISO_IN_SIZE * 2; i++)
//			{
//				reg_usb_ep7_dat = 0;
//			}
		}
	}
	else
	{
//		for (int i=0; i<USB_ISO_IN_SIZE * 2; i++)
//		{
//			reg_usb_ep7_dat = 0;
//		}
		buffer_empty = 1;
	}
	reg_usb_ep7_ctrl = BIT(0);			//ACK iso in
}
#else

#endif

#endif
