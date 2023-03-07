/********************************************************************************************************
 * @file	adc_reg.h
 *
 * @brief	This is the header file for B91
 *
 * @author	Driver Group
 * @date	2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#ifndef ADC_REG_H
#define ADC_REG_H
#include "../sys.h"

#define areg_adc_clk_setting 0x82
enum
{
	FLD_CLK_24M_TO_SAR_EN = BIT(6),
};

#define areg_adc_vref					0xea
enum{
	FLD_ADC_VREF_CHN_M = 		       BIT_RNG(0,1),
};
#define areg_adc_ain_chn_misc			0xeb
enum{
	FLD_ADC_AIN_NEGATIVE = 		        BIT_RNG(0,3),
	FLD_ADC_AIN_POSITIVE = 		        BIT_RNG(4,7),
};
#define areg_adc_res_m					0xec
enum{
	FLD_ADC_RES_M         =             BIT_RNG(0,1),
	FLD_ADC_EN_DIFF_CHN_M =          	BIT(6),
};
#define areg_adc_tsmaple_m				0xee
enum{
	FLD_ADC_TSAMPLE_CYCLE_CHN_M = 		BIT_RNG(0,3),
};
#define areg_r_max_mc					0xef
enum{
	FLD_R_MAX_MC0	= 			BIT_RNG(0,7),//0xef<7:0>  r_max_mc[7:0]
};
#define	areg_r_max_s					0xf1
enum{
	FLD_R_MAX_S		=			BIT_RNG(0,3),//0xf1<3:0>  r_max_s
	FLD_R_MAX_MC1	= 			BIT_RNG(6,7),//0xf1<7:6>  r_max_mc[9:8]
};
#define areg_adc_chn_en					0xf2
enum{
	FLD_ADC_CHN_EN_M	= 		BIT(2),
	FLD_ADC_MAX_SCNT	=		BIT_RNG(4,5),
};
#define areg_adc_data_sample_control	0xf3
enum{
	FLD_NOT_SAMPLE_ADC_DATA 		= BIT(0),
};
#define areg_adc_sample_clk_div		0xf4
enum{
	FLD_ADC_SAMPLE_CLK_DIV  = 	       BIT_RNG(0,2),
};
#define areg_adc_misc_l					0xf7
#define areg_adc_misc_h					0xf8
#define areg_adc_vref_vbat_div			0xf9
enum{
	FLD_ADC_VREF_VBAT_DIV   = 	        BIT_RNG(2,3),
};
#define areg_ain_scale					0xfa
enum{
	FLD_ADC_ITRIM_PREAMP 	= 	BIT_RNG(0,1),
	FLD_ADC_ITRIM_VREFBUF	= 	BIT_RNG(2,3),
	FLD_ADC_ITRIM_VCMBUF	= 	BIT_RNG(4,5),
	FLD_SEL_AIN_SCALE 		= 	BIT_RNG(6,7),
};
#define areg_adc_pga_ctrl				0xfc
enum{
	FLD_SAR_ADC_POWER_DOWN 		= BIT(5),
};
#define areg_temp_sensor_ctrl 0x00
enum{
	FLD_TEMP_SENSOR_POWER_DOWN = BIT(4),
};
#endif
