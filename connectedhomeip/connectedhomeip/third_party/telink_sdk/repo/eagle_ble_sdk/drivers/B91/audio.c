/********************************************************************************************************
 * @file	audio.c
 *
 * @brief	This is the source file for B91
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
#include "audio.h"
#include "clock.h"
#include "pwm.h"
#include "stimer.h"


unsigned char audio_rx_dma_chn;
unsigned char audio_tx_dma_chn;

dma_chain_config_t g_audio_tx_dma_list_cfg;
dma_chain_config_t g_audio_rx_dma_list_cfg;

aduio_i2s_codec_config_t audio_i2s_codec_config = {
    .audio_in_mode          = BIT_16_MONO,
    .audio_out_mode         = BIT_16_MONO_FIFO0,
    .i2s_data_select        = I2S_BIT_16_DATA,
    .codec_data_select      = CODEC_BIT_16_DATA,
    .i2s_codec_m_s_mode     = I2S_M_CODEC_S,
    .i2s_data_invert_select = I2S_DATA_INVERT_DIS, // L channel default
    .in_digital_gain        = CODEC_IN_D_GAIN_0_DB,
    .in_analog_gain         = CODEC_IN_A_GAIN_0_DB,
    .out_digital_gain       = CODEC_OUT_D_GAIN_0_DB,
    .out_analog_gain        = CODEC_OUT_A_GAIN_0_DB,
    .mic_input_mode_select  = 1, // 0 single-ended input, 1 differential input
    .adc_wnf_mode_select    = CODEC_ADC_WNF_INACTIVE,
};

dma_config_t audio_dma_rx_config = {
    .dst_req_sel    = 0,
    .src_req_sel    = DMA_REQ_AUDIO0_RX, // rx req
    .dst_addr_ctrl  = DMA_ADDR_INCREMENT,
    .src_addr_ctrl  = DMA_ADDR_FIX,
    .dstmode        = DMA_NORMAL_MODE,
    .srcmode        = DMA_HANDSHAKE_MODE,
    .dstwidth       = DMA_CTR_WORD_WIDTH, // must word
    .srcwidth       = DMA_CTR_WORD_WIDTH, // must word
    .src_burst_size = 0,                  // must 0
    .read_num_en    = 0,
    .priority       = 0,
    .write_num_en   = 0,
    .auto_en        = 0, // must 0
};

dma_config_t audio_dma_tx_config = {
    .dst_req_sel    = DMA_REQ_AUDIO0_TX, // tx req
    .src_req_sel    = 0,
    .dst_addr_ctrl  = DMA_ADDR_FIX,
    .src_addr_ctrl  = DMA_ADDR_INCREMENT, // increment
    .dstmode        = DMA_HANDSHAKE_MODE, // handshake
    .srcmode        = DMA_NORMAL_MODE,
    .dstwidth       = DMA_CTR_WORD_WIDTH, // must word
    .srcwidth       = DMA_CTR_WORD_WIDTH, // must word
    .src_burst_size = 0,                  // must 0
    .read_num_en    = 0,
    .priority       = 0,
    .write_num_en   = 0,
    .auto_en        = 0, // must 0
};



/**
 * @brief      This function serves to invert data between R channel and L channel.
 * @param[in]  en - I2S_DATA_INVERT_DIS: L channel (default); I2S_DATA_INVERT_EN: R channel.
 * @return     none
 */
void audio_set_mono_chn(audio_data_invert_e en)
{
    audio_i2s_codec_config.i2s_data_invert_select = en;
}

/**
 * @brief      This function serves to set mic input mode.
 * @param[in]  input_mode - 0 single-ended input, 1 differential input.
 * @return     none
 */
void audio_set_codec_mic_input_mode(unsigned char input_mode)
{
    audio_i2s_codec_config.mic_input_mode_select = input_mode;
}

/**
 * 	@brief      This function serves to set in path digital and analog gain  .
 * 	@param[in]  d_gain - digital gain value
 * 	@param[in]  a_gain - analog  gain value
 * 	@return     none
 */
void audio_set_codec_in_path_a_d_gain(codec_in_path_digital_gain_e d_gain, codec_in_path_analog_gain_e a_gain)
{
    audio_i2s_codec_config.in_digital_gain = d_gain;
    audio_i2s_codec_config.in_analog_gain  = a_gain;
}


/**
 * 	@brief      This function serves to set out path digital and analog gain  .
 * 	@param[in]  d_gain - digital gain value
 * 	@param[in]  a_gain - analog  gain value
 * 	@return     none
 */
void audio_set_codec_out_path_a_d_gain(codec_out_path_digital_gain_e d_gain, codec_out_path_analog_gain_e a_gain)
{
    audio_i2s_codec_config.out_digital_gain = d_gain;
    audio_i2s_codec_config.out_analog_gain  = a_gain;
}

/**
 * @brief      This function serves to choose which is master to provide clock.
 * @param[in]  m_s - I2S_S_CODEC_M: i2s as slave ,codec as master; I2S_M_CODEC_S: i2s as  master, codec  as slave.
 * @return     none
 */
void audio_set_i2s_codec_m_s(i2s_codec_m_s_mode_e m_s)
{
    audio_i2s_codec_config.i2s_codec_m_s_mode = m_s;
}

/**
 * @brief  This function serves to set wind noise filter(WNF),it is a programmable high pass filter feature enabling to reduce wind
noise.
 * @param[in] mode - the wind noise filter mode,the wind noise filter is a 1st order filter.
*                                              Mode1  -3dB   59Hz
*  Wind Noise Filter corner frequency          Mode2  -3dB   117Hz
                                                           Mode3  -3dB   235Hz
* @return    none
 */
void audio_set_codec_wnf(adc_wnf_mode_sel_e mode)
{
    audio_i2s_codec_config.adc_wnf_mode_select = mode;
}

/**
 * 	@brief      This function serves to choose which is master to provide clock.
 * 	@param[in]  chn_wl: select word  length and audio channel number
 * 	@return     none
 */
void aduio_set_chn_wl(audio_channel_wl_mode_e chn_wl)
{
    switch (chn_wl)
    {
    case MONO_BIT_16:
        audio_i2s_codec_config.audio_in_mode     = BIT_16_MONO;
        audio_i2s_codec_config.audio_out_mode    = BIT_16_MONO_FIFO0;
        audio_i2s_codec_config.i2s_data_select   = I2S_BIT_16_DATA;
        audio_i2s_codec_config.codec_data_select = CODEC_BIT_16_DATA;
        break;

    case MONO_BIT_20:
        audio_i2s_codec_config.audio_in_mode     = BIT_20_OR_24_MONO;
        audio_i2s_codec_config.audio_out_mode    = BIT_20_OR_24_MONO_FIFO0;
        audio_i2s_codec_config.i2s_data_select   = I2S_BIT_20_DATA;
        audio_i2s_codec_config.codec_data_select = CODEC_BIT_20_DATA;
        break;

    case MONO_BIT_24:
        audio_i2s_codec_config.audio_in_mode     = BIT_20_OR_24_MONO;
        audio_i2s_codec_config.audio_out_mode    = BIT_20_OR_24_MONO_FIFO0;
        audio_i2s_codec_config.i2s_data_select   = I2S_BIT_24_DATA;
        audio_i2s_codec_config.codec_data_select = CODEC_BIT_24_DATA;
        break;

    case STEREO_BIT_16:
        audio_i2s_codec_config.audio_in_mode     = BIT_16_STEREO;
        audio_i2s_codec_config.audio_out_mode    = BIT_16_STEREO_FIFO0;
        audio_i2s_codec_config.i2s_data_select   = I2S_BIT_16_DATA;
        audio_i2s_codec_config.codec_data_select = CODEC_BIT_16_DATA;
        break;

    case STEREO_BIT_20:
        audio_i2s_codec_config.audio_in_mode     = BIT_20_OR_24_STEREO;
        audio_i2s_codec_config.audio_out_mode    = BIT_20_OR_24_STEREO_FIFO0;
        audio_i2s_codec_config.i2s_data_select   = I2S_BIT_20_DATA;
        audio_i2s_codec_config.codec_data_select = CODEC_BIT_20_DATA;
        break;

    case STEREO_BIT_24:
        audio_i2s_codec_config.audio_in_mode     = BIT_20_OR_24_STEREO;
        audio_i2s_codec_config.audio_out_mode    = BIT_20_OR_24_STEREO_FIFO0;
        audio_i2s_codec_config.i2s_data_select   = I2S_BIT_24_DATA;
        audio_i2s_codec_config.codec_data_select = CODEC_BIT_24_DATA;
        break;
    }
}


/**
 * @brief     This function selects  pin  for i2s.
 * @param[in]  pin - the selected pin.
 * @return    none
 */
void audio_i2s_set_pin_mux(i2s_pin_e pin)
{

    unsigned char val       = 0;
    unsigned char start_bit = (BIT_LOW_BIT(pin & 0xff) % 4) << 1;
    unsigned char mask      = (unsigned char) ~BIT_RNG(start_bit, start_bit + 1);
    if (pin == I2S_BCK_PC3)
    {
        val = 0; // function 0
    }
    else if ((pin == I2S_ADC_LR_PC4) || (pin == I2S_ADC_DAT_PC5) || (pin == I2S_DAC_LR_PC6) || (pin == I2S_DAC_DAT_PC7))
    {
        val = 1 << (start_bit); // function 1
    }
    reg_gpio_func_mux(pin) = (reg_gpio_func_mux(pin) & mask) | val;
    gpio_function_dis((gpio_pin_e) pin);
}

/**
 * @brief     This function configures i2s pin.
 * @param[in] none
 * @return    none
 */
void audio_i2s_set_pin(void)
{
    audio_i2s_set_pin_mux(I2S_BCK_PC3);
    audio_i2s_set_pin_mux(I2S_ADC_LR_PC4);
    audio_i2s_set_pin_mux(I2S_ADC_DAT_PC5);
    audio_i2s_set_pin_mux(I2S_DAC_LR_PC6);
    audio_i2s_set_pin_mux(I2S_DAC_DAT_PC7);
}

/**
 * 	@brief      This function serves to set codec supply voltage
 * 	@param[in]  volt - the voltage of codec supply.A1 2.8V default,A0 1.8V default.
 * 	@return     none
 *
 */
void audio_set_codec_supply(codec_volt_supply_e volt)
{

    if (0xff == g_chip_version) // A0 1.8v default ( BIT(7) - 1: 2.8v 0: 1.8v )
    {
        if (CODEC_2P8V == volt)
        {
            analog_write_reg8(0x02, analog_read_reg8(0x02) | BIT(7));
        }

        else if (CODEC_1P8V == volt)
        {
            analog_write_reg8(0x02, analog_read_reg8(0x02) & (~BIT(7)));
        }
    }

    else // A1 2.8v default ( BIT(7) - 1: 1.8v 0: 2.8v )
    {
        if (CODEC_1P8V == volt)
        {
            analog_write_reg8(0x02, analog_read_reg8(0x02) | BIT(7));
        }

        else if (CODEC_2P8V == volt)
        {
            analog_write_reg8(0x02, analog_read_reg8(0x02) & (~BIT(7)));
        }
    }
}

/**
 * @brief     This function configures dmic pin.
 * @param[in] pin_gp - the group of dmic pin
 * @return    none
 */
void audio_set_dmic_pin(dmic_pin_group_e pin_gp)
{
    if (pin_gp == DMIC_GROUPB_B2_DAT_B3_B4_CLK)
    {
        reg_gpio_pad_mul_sel = BIT(2);
        reg_gpio_pb_fuc_h    = reg_gpio_pb_fuc_h & (~BIT_RNG(0, 1));
        reg_gpio_pb_fuc_l    = (reg_gpio_pb_fuc_l & (~BIT_RNG(4, 7)));
        gpio_function_dis(GPIO_PB2 | GPIO_PB3 | GPIO_PB4);
    }
    else if (pin_gp == DMIC_GROUPC_C1_DAT_C2_C3_CLK)
    {
        reg_gpio_pad_mul_sel = BIT(0);
        reg_gpio_pc_fuc_l    = (reg_gpio_pc_fuc_l & (~BIT_RNG(2, 7))) | ((2 << 2) | (2 << 4) | (2 << 6));
        gpio_function_dis(GPIO_PC1 | GPIO_PC2 | GPIO_PC3);
    }
    else if (pin_gp == DMIC_GROUPD_D4_DAT_D5_D6_CLK) // can not use in A0
    {
        reg_gpio_pd_fuc_h = (reg_gpio_pd_fuc_h & (~BIT_RNG(0, 5))) | ((1 << 0) | (1 << 2) | (1 << 4));
        gpio_function_dis(GPIO_PD4 | GPIO_PD5 | GPIO_PD6);
    }
    else if (pin_gp == DMIC_B2_DAT_B3_CLK)
    {
        reg_gpio_pad_mul_sel = BIT(2);
        reg_gpio_pb_fuc_l    = (reg_gpio_pb_fuc_l & (~BIT_RNG(4, 7)));
        gpio_function_dis(GPIO_PB2 | GPIO_PB3);
    }
    else if (pin_gp == DMIC_C1_DAT_C2_CLK)
    {
        reg_gpio_pad_mul_sel = BIT(0);
        reg_gpio_pc_fuc_l    = (reg_gpio_pc_fuc_l & (~BIT_RNG(2, 5))) | ((2 << 2) | (2 << 4));
        gpio_function_dis(GPIO_PC1 | GPIO_PC2);
    }
    else if (pin_gp == DMIC_D4_DAT_D5_CLK) // can not use in A0
    {
        reg_gpio_pd_fuc_h = (reg_gpio_pd_fuc_h & (~BIT_RNG(0, 3))) | ((1 << 0) | (1 << 2));
        gpio_function_dis(GPIO_PD4 | GPIO_PD5);
    }
}


/**
 * @brief     This function serves to enable rx_dma channel.
 * @return    none
 */
void audio_rx_dma_en(void)
{
    dma_chn_en(audio_rx_dma_chn);
}

/**
 * @brief     This function serves to disable rx_dma channel.
 * @return    none
 */
void audio_rx_dma_dis(void)
{
    dma_chn_dis(audio_rx_dma_chn);
}

/**
 * @brief     This function serves to enable tx_dma channel.
 * @return    none
 */
void audio_tx_dma_en(void)
{
    dma_chn_en(audio_tx_dma_chn);
}

/**
 * @brief     This function serves to disable dis_dma channel.
 * @return    none
 */
void audio_tx_dma_dis(void)
{
    dma_chn_dis(audio_tx_dma_chn);
}

/**
 * @brief     This function serves to config  rx_dma channel.
 * @param[in] chn          - dma channel
 * @param[in] dst_addr     - the dma address of destination
 * @param[in] data_len     - the length of dma rx size by byte
 * @param[in] head_of_list - the head address of dma llp.
 * @return    none
 */
void audio_rx_dma_config(dma_chn_e chn, unsigned short * dst_addr, unsigned int data_len, dma_chain_config_t * head_of_list)
{
    audio_rx_dma_chn = chn;
    audio_set_rx_buff_len(data_len);
    dma_config(audio_rx_dma_chn, &audio_dma_rx_config);
    dma_set_address(chn, REG_AUDIO_AHB_BASE, (unsigned int) convert_ram_addr_cpu2bus(dst_addr));
    dma_set_size(chn, data_len, DMA_WORD_WIDTH);
    reg_dma_llp(chn) = (unsigned int) convert_ram_addr_cpu2bus(head_of_list);
}

/**
 * @brief     This function serves to set rx dma chain transfer
 * @param[in] rx_config - the head of list of llp_pointer.
 * @param[in] llpointer - the next element of llp_pointer.
 * @param[in] dst_addr  -the dma address of destination.
 * @param[in] data_len  -the length of dma size by byte.
 * @return    none
 */
void audio_rx_dma_add_list_element(dma_chain_config_t * config_addr, dma_chain_config_t * llpointer, unsigned short * dst_addr,
                                   unsigned int data_len)
{
    config_addr->dma_chain_ctl      = reg_dma_ctrl(audio_rx_dma_chn) | BIT(0);
    config_addr->dma_chain_src_addr = REG_AUDIO_AHB_BASE;
    config_addr->dma_chain_dst_addr = (unsigned int) convert_ram_addr_cpu2bus(dst_addr);
    config_addr->dma_chain_data_len = dma_cal_size(data_len, 4);
    config_addr->dma_chain_llp_ptr  = (unsigned int) convert_ram_addr_cpu2bus(llpointer);
}


/**
 * @brief     This function serves to config  tx_dma channel.
 * @param[in] chn          - dma channel
 * @param[in] src_addr     - the address of source
 * @param[in] data_len     - the length of dma rx size by byte
 * @param[in] head_of_list - the head address of dma llp.
 * @return    none
 */
void audio_tx_dma_config(dma_chn_e chn, unsigned short * src_addr, unsigned int data_len, dma_chain_config_t * head_of_list)
{
    audio_tx_dma_chn = chn;
    audio_set_tx_buff_len(data_len);
    dma_config(audio_tx_dma_chn, &audio_dma_tx_config);
    dma_set_address(chn, (unsigned int) convert_ram_addr_cpu2bus(src_addr), REG_AUDIO_AHB_BASE);
    dma_set_size(chn, data_len, DMA_WORD_WIDTH);
    reg_dma_llp(chn) = (unsigned int) convert_ram_addr_cpu2bus(head_of_list);
}

/**
 * @brief     This function serves to set tx dma chain transfer
 * @param[in] config_addr - the head of list of llp_pointer.
 * @param[in] llpointer   - the next element of llp_pointer.
 * @param[in] src_addr    - the address of source
 * @param[in] data_len    - the length of dma size by byte.
 * @return    none
 */
void audio_tx_dma_add_list_element(dma_chain_config_t * config_addr, dma_chain_config_t * llpointer, unsigned short * src_addr,
                                   unsigned int data_len)
{
    config_addr->dma_chain_ctl      = reg_dma_ctrl(audio_tx_dma_chn) | BIT(0);
    config_addr->dma_chain_src_addr = (unsigned int) convert_ram_addr_cpu2bus(src_addr);
    config_addr->dma_chain_dst_addr = REG_AUDIO_AHB_BASE;
    config_addr->dma_chain_data_len = dma_cal_size(data_len, 4);
    config_addr->dma_chain_llp_ptr  = (unsigned int) convert_ram_addr_cpu2bus(llpointer);
}


/**
 * @brief     This function serves to  initialize audio by mc
 * @param[in] flow_mode  - select input out flow mode
 * @param[in] rate       - audio sampling rate.
 * @param[in] channel_wl - word length and channel number.
 * @return    none
 */
void audio_init(audio_flow_mode_e flow_mode, audio_sample_rate_e rate, audio_channel_wl_mode_e channel_wl)
{
    aduio_set_chn_wl(channel_wl);
    audio_set_codec_clk(1, 16); // from ppl 192/16=12M
    audio_mux_config(CODEC_I2S, audio_i2s_codec_config.audio_in_mode, audio_i2s_codec_config.audio_in_mode,
                     audio_i2s_codec_config.audio_out_mode);
    audio_i2s_config(I2S_I2S_MODE, audio_i2s_codec_config.i2s_data_select, audio_i2s_codec_config.i2s_codec_m_s_mode,
                     audio_i2s_codec_config.i2s_data_invert_select);
    audio_set_i2s_clock(rate, AUDIO_RATE_EQUAL, 0);
    audio_clk_en(1, 1);
    reg_audio_codec_vic_ctr = FLD_AUDIO_CODEC_SLEEP_ANALOG; // active analog sleep mode
    while (!(reg_audio_codec_stat_ctr & FLD_AUDIO_CODEC_PON_ACK))
        ; // wait codec can be configed
    if (flow_mode < BUF_TO_LINE_OUT)
    {
        audio_codec_adc_config(audio_i2s_codec_config.i2s_codec_m_s_mode, (flow_mode % 3), rate,
                               audio_i2s_codec_config.codec_data_select, MCU_WREG);
    }

    if (flow_mode > LINE_IN_TO_BUF)
    {
        audio_codec_dac_config(audio_i2s_codec_config.i2s_codec_m_s_mode, rate, audio_i2s_codec_config.codec_data_select, MCU_WREG);
    }
    while (!(reg_audio_codec_stat_ctr == (FLD_AUDIO_CODEC_ADC12_LOCKED | FLD_AUDIO_CODEC_DAC_LOCKED | FLD_AUDIO_CODEC_PON_ACK)))
        ; // wait codec adc/dac locked

    audio_data_fifo0_path_sel(I2S_DATA_IN_FIFO, I2S_OUT);
}

/**
 * @brief     This function serves to read data from codec register.
 * @param[in] addr: the address of codec register
 * @return    none
 */
unsigned char audio_i2c_codec_read(unsigned char addr)
{
    reg_i2c_data_buf(0) = addr << 1;
    reg_i2c_len         = 0x01; // rx_len
    reg_i2c_sct1        = FLD_I2C_LS_ID | FLD_I2C_LS_ADDR | FLD_I2C_LS_START;

    while (i2c_master_busy())
        ; // wait busy=0
    while (reg_i2c_mst & FLD_I2C_ACK_IN)
        ; // wait ack=0

    reg_i2c_sct1 = FLD_I2C_LS_ID | FLD_I2C_LS_DATAR | FLD_I2C_LS_START | FLD_I2C_LS_ID_R | FLD_I2C_LS_ACK;
    while (i2c_master_busy())
        ; // wait busy=0
    unsigned char rdat8 = reg_i2c_data_buf(0);
    reg_i2c_sct1        = FLD_I2C_LS_STOP | FLD_I2C_LS_ID_R;
    while (i2c_master_busy())
        ; // wait busy=0
    return rdat8;
}

/**
 * @brief     This function serves to write data to  codec register.
 * @param[in] addr: the address of codec register
 * @return    none
 */
void audio_i2c_codec_write(unsigned char addr, unsigned char wdat)
{

    reg_i2c_data_buf(0) = addr << 1;
    reg_i2c_data_buf(1) = wdat;
    reg_i2c_len         = 2; // tx_len
    reg_i2c_sct1        = FLD_I2C_LS_ID | FLD_I2C_LS_DATAW | FLD_I2C_LS_START | FLD_I2C_LS_STOP;
    while (i2c_master_busy())
        ; // wait busy=0
    while (reg_i2c_mst & FLD_I2C_ACK_IN)
        ; // wait ack=0
}

/**
 * @brief      This function serves to enable i2c master for codec i2c slave .
 * @param[in]  none.
 * @return     none.
 */
void audio_i2c_init(codec_type_e codec_type, i2c_sda_pin_e sda_pin, i2c_scl_pin_e scl_pin)
{
    i2c_master_init();
    i2c_set_master_clk((unsigned char) (sys_clk.pclk * 1000 * 1000 / (4 * 20000))); // set i2c frequency 400K.
    // reg_i2c_sp=0x1e;//200k sys_clk.pclk=24M
    if (codec_type == INNER_CODEC)
    {
        reg_audio_i2c_mode = 0x05;        // codec config by i2c
        reg_i2c_id         = (0x34 << 1); // set i2c id
        reg_audio_i2c_addr = 0x34;
    }
    else if (codec_type == EXT_CODEC)
    {
        reg_i2c_id = 0x34;
        i2c_set_pin(sda_pin, scl_pin);
    }
}

/**
 * @brief     This function serves to  initialize audio by i2c
 * @param[in] flow_mode  - select input out flow mode
 * @param[in] rate       - audio sampling rate.
 * @param[in] channel_wl - word length and channel number.
 * @return    none
 */
void audio_init_i2c(audio_flow_mode_e flow_mode, audio_sample_rate_e rate, audio_channel_wl_mode_e channel_wl)
{
    aduio_set_chn_wl(channel_wl);
    audio_set_codec_clk(1, 16); ////from ppl 192/16=12M
    audio_mux_config(CODEC_I2S, audio_i2s_codec_config.audio_in_mode, audio_i2s_codec_config.audio_in_mode,
                     audio_i2s_codec_config.audio_out_mode);
    audio_i2s_config(I2S_I2S_MODE, audio_i2s_codec_config.i2s_data_select, audio_i2s_codec_config.i2s_codec_m_s_mode,
                     audio_i2s_codec_config.i2s_data_invert_select);
    audio_set_i2s_clock(rate, AUDIO_RATE_EQUAL, 0);
    audio_clk_en(1, 1);
    audio_i2c_init(INNER_CODEC, 0, 0);
    audio_i2c_codec_write(addr_audio_codec_vic_ctr, FLD_AUDIO_CODEC_SLEEP_ANALOG); // active analog sleep mode
    while (!(audio_i2c_codec_read(addr_audio_codec_stat_ctr) & FLD_AUDIO_CODEC_PON_ACK))
        ; // wait codec can be configed
    if (flow_mode < BUF_TO_LINE_OUT)
    {
        audio_codec_adc_config(audio_i2s_codec_config.i2s_codec_m_s_mode, (flow_mode % 3), rate,
                               audio_i2s_codec_config.codec_data_select, I2C_WREG);
    }
    if (flow_mode > LINE_IN_TO_BUF)
    {
        audio_codec_dac_config(audio_i2s_codec_config.i2s_codec_m_s_mode, rate, audio_i2s_codec_config.codec_data_select, I2C_WREG);
    }
    while (!(audio_i2c_codec_read(addr_audio_codec_stat_ctr) ==
             (FLD_AUDIO_CODEC_ADC12_LOCKED | FLD_AUDIO_CODEC_DAC_LOCKED | FLD_AUDIO_CODEC_PON_ACK)))
        ; // wait codec adc/dac locked
    audio_data_fifo0_path_sel(I2S_DATA_IN_FIFO, I2S_OUT);
}

/**
 * @brief     This function serves to config codec for dac.
 * @param[in] mode        - select i2s as master or slave
 * @param[in] rate		  - audio sampling rate
 * @param[in] data_select - codec dac word length
 * @param[in] wreg_mode   - mcu or i2c config codec
 * @return    none
 */
void audio_codec_dac_config(i2s_codec_m_s_mode_e mode, audio_sample_rate_e rate, codec_data_select_e data_select,
                            codec_wreg_mode_e wreg_mode)
{

    if (wreg_mode == MCU_WREG)
    {
        BM_SET(reg_audio_codec_dac_ctr, FLD_AUDIO_CODEC_DAC_SOFT_MUTE); // dac mute
        if ((audio_i2s_codec_config.audio_out_mode == BIT_16_MONO_FIFO0) ||
            ((audio_i2s_codec_config.audio_out_mode == BIT_20_OR_24_MONO_FIFO0)))
        {
            BM_CLR(reg_audio_codec_dac_ctr, FLD_AUDIO_CODEC_DAC_SB);        // active DAC power
            BM_SET(reg_audio_codec_dac_ctr, FLD_AUDIO_CODEC_DAC_LEFT_ONLY); // active left channel only
        }
        else
        {
            BM_CLR(reg_audio_codec_dac_ctr,
                   FLD_AUDIO_CODEC_DAC_SB | FLD_AUDIO_CODEC_DAC_LEFT_ONLY); // active DAC power,active left and right channel
        }

        BM_CLR(reg_audio_codec_vic_ctr,
               FLD_AUDIO_CODEC_SB | FLD_AUDIO_CODEC_SB_ANALOG |
                   FLD_AUDIO_CODEC_SLEEP_ANALOG); // disable sleep mode ,disable sb_analog,disable global standby

        /* data word length ,interface master/slave selection, audio interface protocol selection ,active dac audio interface*/
        reg_audio_codec_dac_itf_ctr = MASK_VAL(FLD_AUDIO_CODEC_FORMAT, CODEC_I2S_MODE, FLD_AUDIO_CODEC_DAC_ITF_SB, CODEC_ITF_AC,
                                               FLD_AUDIO_CODEC_SLAVE, mode, FLD_AUDIO_CODEC_WL, data_select);

        /*disable DAC digital gain coupling, Left channel DAC digital gain*/
        reg_audio_codec_dacl_gain =
            MASK_VAL(FLD_AUDIO_CODEC_DAC_LRGOD, 0, FLD_AUDIO_CODEC_DAC_GODL, audio_i2s_codec_config.out_digital_gain);

        reg_audio_codec_dacr_gain =
            MASK_VAL(FLD_AUDIO_CODEC_DAC_GODR, audio_i2s_codec_config.out_digital_gain); /*Right channel DAC digital gain*/

        /*disable Headphone gain coupling, set Left channel HP amplifier gain*/
        reg_audio_codec_hpl_gain =
            MASK_VAL(FLD_AUDIO_CODEC_HPL_LRGO, 0, FLD_AUDIO_CODEC_HPL_GOL, audio_i2s_codec_config.out_analog_gain);

        reg_audio_codec_hpr_gain = MASK_VAL(
            FLD_AUDIO_CODEC_HPR_GOR, audio_i2s_codec_config.out_analog_gain); /* Right channel HP amplifier gain programming*/

        reg_audio_codec_dac_freq_ctr = (FLD_AUDIO_CODEC_DAC_FREQ & (rate == AUDIO_ADC_16K_DAC_48K ? AUDIO_48K : rate));

        BM_CLR(reg_audio_codec_dac_ctr, FLD_AUDIO_CODEC_DAC_SOFT_MUTE); /*dac mute*/
    }

    else if (wreg_mode == I2C_WREG)
    {
        /*active DAC power,active left and right channel,dac mute*/
        audio_i2c_codec_write(
            addr_audio_codec_dac_ctr,
            MASK_VAL(FLD_AUDIO_CODEC_DAC_SB, 0, FLD_AUDIO_CODEC_DAC_LEFT_ONLY, 0, FLD_AUDIO_CODEC_DAC_SOFT_MUTE, 1));

        /*disable sleep mode ,disable sb_analog,disable global standby*/
        audio_i2c_codec_write(addr_audio_codec_vic_ctr,
                              MASK_VAL(FLD_AUDIO_CODEC_SB, 0, FLD_AUDIO_CODEC_SB_ANALOG, 0, FLD_AUDIO_CODEC_SLEEP_ANALOG, 0));

        /*data word length ,interface master/slave selection, audio interface protocol selection ,active dac audio interface */
        audio_i2c_codec_write(addr_audio_codec_dac_itf_ctr,
                              MASK_VAL(FLD_AUDIO_CODEC_FORMAT, CODEC_I2S_MODE, FLD_AUDIO_CODEC_DAC_ITF_SB, CODEC_ITF_AC,
                                       FLD_AUDIO_CODEC_SLAVE, mode, FLD_AUDIO_CODEC_WL, data_select));

        /* disable DAC digital gain coupling, Left channel DAC digital gain */
        audio_i2c_codec_write(
            addr_audio_codec_dacl_gain,
            MASK_VAL(FLD_AUDIO_CODEC_DAC_LRGOD, 0, FLD_AUDIO_CODEC_DAC_GODL, audio_i2s_codec_config.out_digital_gain));

        audio_i2c_codec_write(
            addr_audio_codec_dacr_gain,
            MASK_VAL(FLD_AUDIO_CODEC_DAC_GODR, audio_i2s_codec_config.out_digital_gain)); /* Right channel DAC digital gain */

        /* disable Headphone gain coupling, set Left channel HP amplifier gain */
        audio_i2c_codec_write(
            addr_audio_codec_hpl_gain,
            MASK_VAL(FLD_AUDIO_CODEC_HPL_LRGO, 0, FLD_AUDIO_CODEC_HPL_GOL, audio_i2s_codec_config.out_analog_gain));

        audio_i2c_codec_write(addr_audio_codec_hpr_gain,
                              MASK_VAL(FLD_AUDIO_CODEC_HPR_GOR,
                                       audio_i2s_codec_config.out_analog_gain)); /*  Right channel HP amplifier gain programming*/

        audio_i2c_codec_write(addr_audio_codec_dac_freq_ctr, (FLD_AUDIO_CODEC_DAC_FREQ & rate));

        /*dac mute*/
        audio_i2c_codec_write(
            addr_audio_codec_dac_ctr,
            MASK_VAL(FLD_AUDIO_CODEC_DAC_SB, 0, FLD_AUDIO_CODEC_DAC_LEFT_ONLY, 0, FLD_AUDIO_CODEC_DAC_SOFT_MUTE, 0));
    }
}



/**
 * @brief     This function serves to config codec for adc.
 * @param[in] mode        - select i2s as master or slave
 * @param[in] in_mode     - line_in/amic/dmic input mode select
 * @param[in] rate        - audio sampling rate
 * @param[in] data_select - codec adc word length
 * @param[in] wreg_mode   - mcu or i2c config codec
 * @return    none
 */
void audio_codec_adc_config(i2s_codec_m_s_mode_e mode, audio_input_mode_e in_mode, audio_sample_rate_e rate,
                            codec_data_select_e data_select, codec_wreg_mode_e wreg_mode)
{

    if (wreg_mode == MCU_WREG)
    {
        BM_SET(reg_audio_codec_adc12_ctr, FLD_AUDIO_CODEC_ADC12_SOFT_MUTE); /*adc mute*/
        if ((audio_i2s_codec_config.audio_in_mode == BIT_16_MONO) || ((audio_i2s_codec_config.audio_in_mode == BIT_20_OR_24_MONO)))
        {
            BM_CLR(reg_audio_codec_adc12_ctr, FLD_AUDIO_CODEC_ADC1_SB); /*active anc0 channel,mono .*/
        }
        else
        {
            BM_CLR(reg_audio_codec_adc12_ctr, FLD_AUDIO_CODEC_ADC1_SB | FLD_AUDIO_CODEC_ADC2_SB); /*active adc0 and adc1  channel*/
        }
        BM_CLR(reg_audio_codec_vic_ctr,
               FLD_AUDIO_CODEC_SB | FLD_AUDIO_CODEC_SB_ANALOG |
                   FLD_AUDIO_CODEC_SLEEP_ANALOG); /*disable sleep mode ,disable sb_analog,disable global standby*/

        if (in_mode == AMIC_INPUT)
        {
            /*Microphone 1 input selection ,Microphone biasing active,Single-ended input,MICBIAS1 output=2.08V,*/
            reg_audio_codec_mic1_ctr =
                MASK_VAL(FLD_AUDIO_CODEC_MIC1_SEL, 0, FLD_AUDIO_CODEC_MICBIAS1_SB, 0, FLD_AUDIO_CODEC_MIC_DIFF1,
                         audio_i2s_codec_config.mic_input_mode_select, FLD_AUDIO_CODEC_MICBIAS1_V, 0);
            /*Microphone 2 input selection,Single-ended input*/
            reg_audio_codec_mic2_ctr =
                MASK_VAL(FLD_AUDIO_CODEC_MIC2_SEL, 0, FLD_AUDIO_CODEC_MIC_DIFF2, audio_i2s_codec_config.mic_input_mode_select);

            /*set wind noise filter */
            reg_audio_codec_adc_wnf_ctr = audio_i2s_codec_config.adc_wnf_mode_select;

            /*analog 0/4/8/12/16/20 dB boost gain*/
            reg_audio_codec_mic_l_R_gain = MASK_VAL(FLD_AUDIO_CODEC_AMIC_L_GAIN, audio_i2s_codec_config.in_analog_gain,
                                                    FLD_AUDIO_CODEC_AMIC_R_GAIN, audio_i2s_codec_config.in_analog_gain);
        }
        else if (in_mode == DMIC_INPUT)
        {
            reg_audio_dmic_12 = MASK_VAL(FLD_AUDIO_CODEC_ADC_DMIC_SEL2, 1, FLD_AUDIO_CODEC_ADC_DMIC_SEL1, 1,
                                         FLD_AUDIO_CODEC_DMIC2_SB, CODEC_ITF_AC, FLD_AUDIO_CODEC_DMIC1_SB, CODEC_ITF_AC);
        }

        else if (in_mode == LINE_INPUT)
        {
            reg_audio_codec_mic1_ctr = MASK_VAL(FLD_AUDIO_CODEC_MIC_DIFF1, audio_i2s_codec_config.mic_input_mode_select);

            reg_audio_codec_mic2_ctr = MASK_VAL(FLD_AUDIO_CODEC_MIC_DIFF2, audio_i2s_codec_config.mic_input_mode_select);

            /*analog 0/4/8/12/16/20 dB boost gain*/
            reg_audio_codec_mic_l_R_gain = MASK_VAL(FLD_AUDIO_CODEC_AMIC_L_GAIN, audio_i2s_codec_config.in_analog_gain,
                                                    FLD_AUDIO_CODEC_AMIC_R_GAIN, audio_i2s_codec_config.in_analog_gain);
        }

        /*0db~43db  1db step ,digital programmable gain*/
        reg_audio_adc1_gain =
            MASK_VAL(FLD_AUDIO_CODEC_ADC_LRGID, 1, FLD_AUDIO_CODEC_ADC_GID1, audio_i2s_codec_config.in_digital_gain);
        /*data word length ,interface master/slave selection, audio interface protocol selection  */
        reg_audio_codec_adc_itf_ctr =
            MASK_VAL(FLD_AUDIO_CODEC_FORMAT, CODEC_I2S_MODE, FLD_AUDIO_CODEC_SLAVE, mode, FLD_AUDIO_CODEC_WL, data_select);
        /*audio adc interface active*/
        BM_CLR(reg_audio_codec_adc2_ctr, FLD_AUDIO_CODEC_ADC12_SB);

        /*  adc high pass filter active, set adc sample rate   */
        reg_audio_codec_adc_freq_ctr =
            MASK_VAL(FLD_AUDIO_CODEC_ADC12_HPF_EN, 1, FLD_AUDIO_CODEC_ADC_FREQ, rate == AUDIO_ADC_16K_DAC_48K ? AUDIO_16K : rate);

        BM_CLR(reg_audio_codec_adc12_ctr, FLD_AUDIO_CODEC_ADC12_SOFT_MUTE); /*adc unmute*/
    }

    else if (wreg_mode == I2C_WREG)
    {

        /*active adc0 and adc1  channel, if mono only active adc1,adc mute*/
        audio_i2c_codec_write(addr_audio_codec_adc12_ctr,
                              MASK_VAL(FLD_AUDIO_CODEC_ADC1_SB, 0, FLD_AUDIO_CODEC_ADC2_SB, 0, FLD_AUDIO_CODEC_ADC12_SOFT_MUTE, 1));

        /*disable sleep mode ,disable sb_analog,disable global standby*/
        audio_i2c_codec_write(addr_audio_codec_vic_ctr,
                              MASK_VAL(FLD_AUDIO_CODEC_SB, 0, FLD_AUDIO_CODEC_SB_ANALOG, 0, FLD_AUDIO_CODEC_SLEEP_ANALOG, 0));

        if (in_mode == AMIC_INPUT)
        {
            /*Microphone 1 input selection ,Microphone biasing active,Single-ended input,MICBIAS1 output=2.08V,*/
            audio_i2c_codec_write(addr_audio_codec_mic1_ctr,
                                  MASK_VAL(FLD_AUDIO_CODEC_MIC1_SEL, 0, FLD_AUDIO_CODEC_MICBIAS1_SB, 0, FLD_AUDIO_CODEC_MIC_DIFF1,
                                           0, FLD_AUDIO_CODEC_MICBIAS1_V, 0));

            /*Microphone 2 input selection,Single-ended input*/
            audio_i2c_codec_write(addr_audio_codec_mic2_ctr, MASK_VAL(FLD_AUDIO_CODEC_MIC2_SEL, 0, FLD_AUDIO_CODEC_MIC_DIFF2, 0));

            /*analog 0/4/8/12/16/20 dB boost gain*/
            audio_i2c_codec_write(addr_audio_codec_mic_l_R_gain,
                                  MASK_VAL(FLD_AUDIO_CODEC_AMIC_L_GAIN, audio_i2s_codec_config.in_analog_gain,
                                           FLD_AUDIO_CODEC_AMIC_R_GAIN, audio_i2s_codec_config.in_analog_gain));
        }
        else if (in_mode == DMIC_INPUT)
        {
            audio_i2c_codec_write(addr_audio_dmic_12,
                                  MASK_VAL(FLD_AUDIO_CODEC_ADC_DMIC_SEL2, 1, FLD_AUDIO_CODEC_ADC_DMIC_SEL1, 1,
                                           FLD_AUDIO_CODEC_DMIC2_SB, CODEC_ITF_AC, FLD_AUDIO_CODEC_DMIC1_SB, CODEC_ITF_AC));
        }

        else if (in_mode == LINE_INPUT)
        {
            /*analog 0/4/8/12/16/20 dB boost gain*/
            audio_i2c_codec_write(addr_audio_codec_mic_l_R_gain,
                                  MASK_VAL(FLD_AUDIO_CODEC_AMIC_L_GAIN, audio_i2s_codec_config.in_analog_gain,
                                           FLD_AUDIO_CODEC_AMIC_R_GAIN, audio_i2s_codec_config.in_analog_gain));
        }

        /*0db~43db  1db step ,digital programmable gain*/
        audio_i2c_codec_write(
            addr_audio_adc1_gain,
            MASK_VAL(FLD_AUDIO_CODEC_ADC_LRGID, 1, FLD_AUDIO_CODEC_ADC_GID1, audio_i2s_codec_config.in_digital_gain));

        audio_i2c_codec_write(
            addr_audio_codec_adc_itf_ctr,
            MASK_VAL(FLD_AUDIO_CODEC_FORMAT, CODEC_I2S_MODE, FLD_AUDIO_CODEC_SLAVE, mode, FLD_AUDIO_CODEC_WL, data_select));

        audio_i2c_codec_write(addr_audio_codec_adc2_ctr, ~FLD_AUDIO_CODEC_ADC12_SB); /*audio adc interface active*/

        /*  adc high pass filter active, set adc sample rate   */
        audio_i2c_codec_write(addr_audio_codec_adc_freq_ctr,
                              MASK_VAL(FLD_AUDIO_CODEC_ADC12_HPF_EN, 1, FLD_AUDIO_CODEC_ADC_FREQ, rate));

        /*dac mute*/
        audio_i2c_codec_write(addr_audio_codec_adc12_ctr,
                              MASK_VAL(FLD_AUDIO_CODEC_ADC1_SB, 0, FLD_AUDIO_CODEC_ADC2_SB, 0, FLD_AUDIO_CODEC_ADC12_SOFT_MUTE, 0));
    }
}


/**
 * @brief     This function serves to set data path.
 * @param[in] audio_flow    - audio flow  select
 * @param[in] ain0_mode     - fifo0 input mode select
 * @param[in] ain1_mode     - fifo1 input mode select
 * @param[in] i2s_aout_mode - fifo output source select
 * @return    none
 */
void audio_mux_config(audio_flow_e audio_flow, audio_in_mode_e ain0_mode, audio_in_mode_e ain1_mode, audio_out_mode_e i2s_aout_mode)
{
    reg_audio_ctrl |= audio_flow;
    reg_audio_tune = MASK_VAL(FLD_AUDIO_I2S_I2S_AIN0_COME, ain0_mode, FLD_AUDIO_I2S_I2S_AIN1_COME, ain1_mode,
                              FLD_AUDIO_I2S_I2S_AOUT_COME, i2s_aout_mode);
}


/**
 * @brief     This function serves to config interface, word length, and m/s .
 * @param[in] i2s_format - interface protocol
 * @param[in] wl   		 - audio data word length
 * @param[in] m_s        - select i2s as master or slave
 * @param[in] en         - 1 enable audio data invert , 0 disable audio data invert .for example in mono mode switch R and L channel
 * data to fifo0
 * @return    none
 */
void audio_i2s_config(i2s_mode_select_e i2s_format, i2s_data_select_e wl, i2s_codec_m_s_mode_e m_s, audio_data_invert_e en)
{

    reg_i2s_cfg = MASK_VAL(FLD_AUDIO_I2S_FORMAT, i2s_format, FLD_AUDIO_I2S_WL, wl, FLD_AUDIO_I2S_LRP, 0, FLD_AUDIO_I2S_LRSWAP, en,
                           FLD_AUDIO_I2S_ADC_DCI_MS, m_s, FLD_AUDIO_I2S_DAC_DCI_MS, m_s);
}

/**
 * @brief     This function serves to set i2s clock and audio sampling rate when i2s as master.
 * @param[in] audio_rate - audio sampling rate
 * @param[in] match		 - the match of audio rate.
 * @param[in] match_en   - initial must 0, then change rate must 1
 * @return    none
 * @attention i2s clock  divider from pll,sampling rate calculation is based on pll=192M,so pll must be 192M
 */
_attribute_ram_code_sec_noinline_ void audio_set_i2s_clock(audio_sample_rate_e audio_rate, audio_rate_match_e match,
                                                           unsigned char match_en)
{
    reg_tx_wptr = 0xffff; // enable tx_rptr
    unsigned short tx_rptr_old;
    switch (audio_rate)
    {
    case AUDIO_8K:
        audio_set_i2s_clk(1, 8);   // set i2s clk 24M
        audio_set_i2s_bclk(12);    // 24/(2*12) = 1M bclk
        audio_set_lrclk(125, 125); // bclk/125=8k
        break;

    case AUDIO_16K:
        audio_set_i2s_clk(1, 8);   // set i2s clk 24M
        audio_set_i2s_bclk(6);     // 24/(2*6) = 2M bclk
        audio_set_lrclk(125, 125); // bclk/125=16k
        break;

    case AUDIO_32K:
        audio_set_i2s_clk(1, 8);   // set i2s clk 24M
        audio_set_i2s_bclk(3);     // 24/(2*3) = 4M bclk
        audio_set_lrclk(125, 125); // bclk/125=32k
        break;

    case AUDIO_ADC_16K_DAC_48K:
        audio_set_i2s_clk(2, 125); // i2s clk 3.072 M
        audio_set_i2s_bclk(0);     // 3.072/1 = 3.072M bclk
        audio_set_lrclk(192, 64);  // adc_lrclk=3.072/192=16K,dac_lrclk=3.072/64=48K
        break;

    case AUDIO_48K:
        if (match_en)
        {
            tx_rptr_old = reg_tx_rptr;
            while (tx_rptr_old == reg_tx_rptr)
                ;
        }
        if (match == AUDIO_RATE_EQUAL) // 48000
        {
            audio_set_i2s_clk(2, 125); // i2s clk 3.072 M
            audio_set_i2s_bclk(0);     // 3.072/1 = 3.072M bclk
            audio_set_lrclk(64, 64);   // bclk/64=48k
        }

        else if (match == AUDIO_RATE_GT_L0) // 48004
        {
            audio_set_i2s_clk(3, 169);
            audio_set_i2s_bclk(0);
            audio_set_lrclk(71, 71);
        }

        else if (match == AUDIO_RATE_GT_L1) // 48012.0
        {
            audio_set_i2s_clk(4, 129);
            audio_set_i2s_bclk(0);
            audio_set_lrclk(124, 124);
        }
        else if (match == AUDIO_RATE_LT_L0)
        {
            audio_set_i2s_clk(2, 63); // 47994.0
            audio_set_i2s_bclk(0);
            audio_set_lrclk(127, 127);
        }

        else if (match == AUDIO_RATE_LT_L1)
        {
            audio_set_i2s_clk(4, 165); // 47985.0
            audio_set_i2s_bclk(0);
            audio_set_lrclk(97, 97);
        }
        break;

    case AUDIO_44EP1K:
        if (match_en)
        {
            tx_rptr_old = reg_tx_rptr;
            while (tx_rptr_old == reg_tx_rptr)
                ;
        }

        if (match == AUDIO_RATE_EQUAL) // 44099.9
        {
            audio_set_i2s_clk(8, 215);
            audio_set_i2s_bclk(0);
            audio_set_lrclk(162, 162);
        }
        else if (match == AUDIO_RATE_GT_L0) // 44110.2
        {
            audio_set_i2s_clk(11, 228);
            audio_set_i2s_bclk(0);
            audio_set_lrclk(210, 210);
        }

        else if (match == AUDIO_RATE_GT_L1) // 44117.6
        {
            audio_set_i2s_clk(5, 170);
            audio_set_i2s_bclk(0);
            audio_set_lrclk(128, 128);
        }

        else if (match == AUDIO_RATE_LT_L0) // 44094.4
        {
            audio_set_i2s_clk(7, 254);
            audio_set_i2s_bclk(0);
            audio_set_lrclk(120, 120);
        }

        else if (match == AUDIO_RATE_LT_L1) // 44081.6
        {
            audio_set_i2s_clk(9, 245);
            audio_set_i2s_bclk(0);
            audio_set_lrclk(160, 160);
        }
        break;
    }
}


/**
 * @brief  This function serves to set audio rx dma chain transfer.
 * @param[in] chn       -  dma channel
 * @param[in] in_buff   - the pointer of rx_buff.
 * @param[in] buff_size - the size of rx_buff.
 * @return    none
 */
void audio_rx_dma_chain_init(dma_chn_e chn, unsigned short * in_buff, unsigned int buff_size)
{
    audio_rx_dma_config(chn, (unsigned short *) in_buff, buff_size, &g_audio_rx_dma_list_cfg);
    audio_rx_dma_add_list_element(&g_audio_rx_dma_list_cfg, &g_audio_rx_dma_list_cfg, (unsigned short *) in_buff, buff_size);
    audio_rx_dma_en();
}

/**
 * @brief  This function serves to initialize audio tx dma chain transfer.
 * @param[in] chn       - dma channel
 * @param[in] out_buff  - the pointer of tx_buff.
 * @param[in] buff_size - the size of tx_buff.
 * @return    none
 */
void audio_tx_dma_chain_init(dma_chn_e chn, unsigned short * out_buff, unsigned int buff_size)
{
    audio_tx_dma_config(chn, (unsigned short *) out_buff, buff_size, &g_audio_tx_dma_list_cfg);
    audio_tx_dma_add_list_element(&g_audio_tx_dma_list_cfg, &g_audio_tx_dma_list_cfg, (unsigned short *) out_buff, buff_size);
    audio_tx_dma_en();
}

#define WM8731_ANA_AUDIO_PATH_CTRL 0x08        // Analogue Audio Path Control
#define WM8731_DIG_AUDIO_PATH_CTRL 0x0a        // Digital Audio Path Control
#define WM8731_POWER_DOWN_CTRL 0x0c            // Power Down Control
#define WM8731_ST_LINE_VOL 0x00                // Set linmute volume
#define WM8731_ST_RINE_VOL 0x02                // Set rinmute volume
#define WM8731_DIG_AUDIO_INTERFACE_FORMAT 0x0e // Digital Audio Interface Format
#define WM8731_SAMPLING_CTRL 0x10              // Sampling Control
#define WM8731_ACTIVE_CTRL 0x12                // Active Control
#define WM8731_RESET_CTRL 0x1e                 // Reset Register

unsigned char LineIn_To_I2S_CMD_TAB[9][2] = {
    { WM8731_RESET_CTRL, 0x00 },
    { WM8731_ST_LINE_VOL, 0x10 },
    { WM8731_ST_RINE_VOL, 0x10 },
    { WM8731_ANA_AUDIO_PATH_CTRL, 0x13 },
    { WM8731_DIG_AUDIO_PATH_CTRL, 0x00 },
    { WM8731_POWER_DOWN_CTRL, 0x22 },
    { WM8731_DIG_AUDIO_INTERFACE_FORMAT, 0x02 },
    { WM8731_SAMPLING_CTRL, 0x19 },
    { WM8731_ACTIVE_CTRL, 0x01 },

};

/**
 * @brief     This function serves to  set external  codec by i2c
 * @return    none
 */
void audio_set_ext_codec(void)
{
    for (unsigned char i = 0; i < 9; i++)
    {
        audio_i2c_codec_write(LineIn_To_I2S_CMD_TAB[i][0] >> 1, LineIn_To_I2S_CMD_TAB[i][1]);
    }
}


/**
 * @brief     This function serves to  set pwm0 as external codec mclk
 * @param[in] pin - the pin of pwm0
 * @return    none
 */
void pwm_set(pwm_pin_e pin)
{
    reg_pwm_enable  = 0;
    reg_pwm0_enable = 0; // off pwm0
    pwm_set_pin(pin);
    pwm_set_clk((unsigned char) (sys_clk.pclk * 1000 * 1000 / 24000000 - 1));
    // reg_pwm_clkdiv = 0;//set pwm clk equal pclk 24M
    pwm_set_pwm0_mode(PWM_NORMAL_MODE);
    pwm_set_tcmp(PWM0_ID, 1);
    pwm_set_tmax(PWM0_ID, 2); // 24M/2=12M pwm  mclk to  ext codec clk
    pwm_start(PWM0_ID);

}


/**
 * @brief This function serves to initialize audio(external codec WM8731) by i2c.
 * @param[in]  pwm0_pin - the pin of pwm0
 * @param[in]  sda_pin  - the pin port selected as I2C sda pin port.
 * @param[in]  scl_pin  - the pin port selected as I2C scl pin port.
 * @return    none
 */
void audio_i2s_init(pwm_pin_e pwm0_pin, i2c_sda_pin_e sda_pin, i2c_scl_pin_e scl_pin)
{
    pwm_set(pwm0_pin);
    audio_i2s_set_pin();
    audio_i2c_init(EXT_CODEC, sda_pin, scl_pin);
    aduio_set_chn_wl(MONO_BIT_16);
    audio_mux_config(IO_I2S, BIT_16_MONO, BIT_16_MONO, BIT_16_MONO_FIFO0);
    audio_i2s_config(I2S_I2S_MODE, I2S_BIT_16_DATA, I2S_M_CODEC_S, I2S_DATA_INVERT_DIS);
    audio_set_i2s_clock(AUDIO_32K, AUDIO_RATE_EQUAL, 0);
    audio_clk_en(1, 1);
    audio_set_ext_codec();
    audio_data_fifo0_path_sel(I2S_DATA_IN_FIFO, I2S_OUT);
}

/**
 * @brief    This function serves to active soft mute dac and disable dma .
 * @return    none
 */
void audio_pause_out_path(void)
{
    BM_SET(reg_audio_codec_dac_ctr, FLD_AUDIO_CODEC_DAC_SOFT_MUTE); // dac mute
    audio_tx_dma_dis();
}

/**
 * @brief    This function serves to inactive soft mute dac and enable dma after change_sample_rate.
 * @return    none
 */
void audio_resume_out_path(void)
{
    BM_CLR(reg_audio_codec_dac_ctr, FLD_AUDIO_CODEC_DAC_SOFT_MUTE); // dac unmute
    audio_tx_dma_en();
}
/**
 * @brief     This function serves to change sample rate for dac.
 * @param[in] rate     -  the sample rate of dac
 * @return    none
 */
_attribute_ram_code_sec_ void audio_change_sample_rate(audio_sample_rate_e rate)
{
    audio_set_i2s_clock(rate, AUDIO_RATE_EQUAL, 1);
    reg_audio_codec_dac_freq_ctr = (FLD_AUDIO_CODEC_DAC_FREQ & rate);
}



/**
 * @brief     This function serves to power down codec_dac.
 * @return    none
 */
void audio_codec_dac_power_down(void)
{
    BM_SET(reg_audio_codec_dac_ctr, FLD_AUDIO_CODEC_DAC_SOFT_MUTE);
    delay_ms(10);
    BM_SET(reg_audio_codec_dac_itf_ctr, FLD_AUDIO_CODEC_DAC_ITF_SB);
    BM_SET(reg_audio_codec_dac_ctr, FLD_AUDIO_CODEC_DAC_SB);
    delay_ms(1);
    BM_SET(reg_audio_codec_vic_ctr, FLD_AUDIO_CODEC_SLEEP_ANALOG);
    reg_audio_codec_vic_ctr = MASK_VAL(FLD_AUDIO_CODEC_SB, CODEC_ITF_PD, FLD_AUDIO_CODEC_SB_ANALOG, CODEC_ITF_PD,
                                       FLD_AUDIO_CODEC_SLEEP_ANALOG, CODEC_ITF_PD);
    audio_tx_dma_dis();
    audio_clk_en(0, 0);
}

/**
 * @brief     This function serves to power on codec_dac.
 * @return    none
 */
void audio_codec_dac_power_on(void)
{
    audio_clk_en(1, 1);
    BM_SET(reg_audio_codec_vic_ctr, FLD_AUDIO_CODEC_SLEEP_ANALOG);
    delay_ms(1);
    BM_CLR(reg_audio_codec_dac_ctr, FLD_AUDIO_CODEC_DAC_SB);
    BM_CLR(reg_audio_codec_dac_itf_ctr, FLD_AUDIO_CODEC_DAC_ITF_SB);
    reg_audio_codec_vic_ctr = MASK_VAL(FLD_AUDIO_CODEC_SB, CODEC_ITF_AC, FLD_AUDIO_CODEC_SB_ANALOG, CODEC_ITF_AC,
                                       FLD_AUDIO_CODEC_SLEEP_ANALOG, CODEC_ITF_AC);
    BM_CLR(reg_audio_codec_dac_ctr, FLD_AUDIO_CODEC_DAC_SOFT_MUTE); // un mute
    audio_tx_dma_en();
}

/**
 * @brief     This function serves to power down codec_adc.
 * @return    none
 */
void audio_codec_adc_power_down(void)
{
    BM_SET(reg_audio_codec_adc12_ctr, FLD_AUDIO_CODEC_ADC12_SOFT_MUTE);
    delay_ms(10);
    BM_SET(reg_audio_codec_adc2_ctr, FLD_AUDIO_CODEC_ADC12_SB);
    BM_SET(reg_audio_codec_adc12_ctr, FLD_AUDIO_CODEC_ADC1_SB | FLD_AUDIO_CODEC_ADC2_SB);

    BM_SET(reg_audio_codec_vic_ctr, FLD_AUDIO_CODEC_SLEEP_ANALOG);
    reg_audio_codec_vic_ctr = MASK_VAL(FLD_AUDIO_CODEC_SB, CODEC_ITF_PD, FLD_AUDIO_CODEC_SB_ANALOG, CODEC_ITF_PD,
                                       FLD_AUDIO_CODEC_SLEEP_ANALOG, CODEC_ITF_PD);
    audio_rx_dma_dis();
    audio_clk_en(0, 0);
}


/**
 * @brief     This function serves to power on codec_adc.
 * @return    none
 */
void audio_codec_adc_power_on(void)
{
    audio_clk_en(1, 1);
    BM_SET(reg_audio_codec_vic_ctr, FLD_AUDIO_CODEC_SLEEP_ANALOG);
    delay_ms(1);
    BM_CLR(reg_audio_codec_adc12_ctr, FLD_AUDIO_CODEC_ADC1_SB | FLD_AUDIO_CODEC_ADC2_SB);
    BM_CLR(reg_audio_codec_adc2_ctr, FLD_AUDIO_CODEC_ADC12_SB);
    reg_audio_codec_vic_ctr = MASK_VAL(FLD_AUDIO_CODEC_SB, CODEC_ITF_AC, FLD_AUDIO_CODEC_SB_ANALOG, CODEC_ITF_AC,
                                       FLD_AUDIO_CODEC_SLEEP_ANALOG, CODEC_ITF_AC);
    BM_CLR(reg_audio_codec_adc12_ctr, FLD_AUDIO_CODEC_ADC12_SOFT_MUTE);

    audio_rx_dma_en();
}




