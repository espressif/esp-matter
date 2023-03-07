/*
 * Filename: ti3254.h
 *
 * Description: Macro for TI3254 registers
 *
 * Copyright (C) 2019 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
//*****************************************************************************

#ifndef __TI3254_H__
#define __TI3254_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#define CODEC_I2C_SLAVE_ADDR      ((0x30 >> 1))
#define CRYSTAL_FREQ               40000000
#define SYS_CLK                    CRYSTAL_FREQ

#define TI3254_PAGE_0					0x00
#define TI3254_PAGE_1					0x01
#define TI3254_PAGE_8					0x08
#define TI3254_PAGE_44					0x2C

#define PAGE_CTRL_REG					0x00

// Page 0
#define TI3254_PAGE_SEL_REG				0x00
#define TI3254_SW_RESET_REG				0x01
#define TI3254_CLK_MUX_REG				0x04
#define TI3254_CLK_PLL_P_R_REG			0x05
#define TI3254_CLK_PLL_J_REG			0x06
#define TI3254_CLK_PLL_D_MSB_REG		0x07
#define TI3254_CLK_PLL_D_LSB_REG		0x08
#define TI3254_CLK_NDAC_REG				0x0B
#define TI3254_CLK_MDAC_REG				0x0C
#define TI3254_DAC_OSR_MSB_REG			0x0D
#define TI3254_DAC_OSR_LSB_REG			0x0E
#define TI3254_DSP_D_CTRL_1_REG			0x0F
#define TI3254_DSP_D_CTRL_2_REG			0x10
#define TI3254_DSP_D_INTERPOL_REG		0x11
#define TI3254_CLK_NADC_REG				0x12
#define TI3254_CLK_MADC_REG				0x13
#define TI3254_ADC_OSR_REG				0x14
#define TI3254_DSP_A_CTRL_1_REG			0x15
#define TI3254_DSP_A_CTRL_2_REG			0x16
#define TI3254_DSP_A_DEC_FACT_REG		0x17
#define TI3254_AUDIO_IF_1_REG			0x1B

#define TI3254_DAC_SIG_P_BLK_CTRL_REG	0x3C
#define TI3254_ADC_SIG_P_BLK_CTRL_REG	0x3D

#define TI3254_DAC_CHANNEL_SETUP_1_REG	0x3F
#define TI3254_DAC_CHANNEL_SETUP_2_REG	0x40

#define TI3254_LEFT_DAC_VOL_CTRL_REG	0x41
#define TI3254_RIGHT_DAC_VOL_CTRL_REG	0x42

#define TI3254_ADC_CHANNEL_SETUP_REG	0x51
#define TI3254_ADC_FINE_GAIN_ADJ_REG	0x52

#define TI3254_LEFT_ADC_VOL_CTRL_REG	0x53
#define TI3254_RIGHT_ADC_VOL_CTRL_REG	0x54



//Page 1

#define TI3254_PWR_CTRL_REG				0x01
#define TI3254_LDO_CTRL_REG				0x02
#define TI3254_OP_DRV_PWR_CTRL_REG		0x09
#define TI3254_HPL_ROUTING_SEL_REG		0x0C
#define TI3254_HPR_ROUTING_SEL_REG		0x0D
#define TI3254_LOL_ROUTING_SEL_REG		0x0E
#define TI3254_LOR_ROUTING_SEL_REG		0x0F
#define TI3254_HPL_DRV_GAIN_CTRL_REG	0x10
#define TI3254_HPR_DRV_GAIN_CTRL_REG	0x11
#define TI3254_LOL_DRV_GAIN_CTRL_REG	0x12
#define TI3254_LOR_DRV_GAIN_CTRL_REG	0x13
#define TI3254_HP_DRV_START_UP_CTRL_REG	0x14

#define TI3254_MICBIAS_CTRL_REG			0x33
#define TI3254_LEFT_MICPGA_P_CTRL_REG	0x34
#define TI3254_LEFT_MICPGA_N_CTRL_REG	0x36
#define TI3254_RIGHT_MICPGA_P_CTRL_REG	0x37
#define TI3254_RIGHT_MICPGA_N_CTRL_REG	0x39
#define TI3254_FLOAT_IP_CTRL_REG		0x3a
#define TI3254_LEFT_MICPGA_VOL_CTRL_REG	0x3B
#define TI3254_RIGHT_MICPGA_VOL_CTRL_REG 0x3C

#define TI3254_ANALOG_IP_QCHRG_CTRL_REG	0x47
#define TI3254_REF_PWR_UP_CTRL_REG		0x7B


// Page 8
#define TI3254_ADC_ADP_FILTER_CTRL_REG	0x01

// Page 44
#define TI3254_DAC_ADP_FILTER_CTRL_REG	0x01


#ifdef __cplusplus
}
#endif


#endif /* __TI3254_H__ */
