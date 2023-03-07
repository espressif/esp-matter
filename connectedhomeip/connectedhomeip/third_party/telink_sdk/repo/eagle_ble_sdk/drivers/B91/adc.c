/********************************************************************************************************
 * @file	adc.c
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
#include "adc.h"
#include "audio.h"
#include "compiler.h"

_attribute_data_retention_sec_ unsigned short g_adc_vref = 1175; //default ADC ref voltage (unit:mV)
volatile unsigned char g_adc_pre_scale;
volatile unsigned char g_adc_vbat_divider;

dma_chn_e adc_dma_chn;
dma_config_t adc_rx_dma_config=
{
	.dst_req_sel= 0,
	.src_req_sel=DMA_REQ_AUDIO1_RX,//adc use the audio1 interface
	.dst_addr_ctrl=DMA_ADDR_INCREMENT,
	.src_addr_ctrl=DMA_ADDR_FIX,
	.dstmode=DMA_NORMAL_MODE,
	.srcmode=DMA_HANDSHAKE_MODE,
	.dstwidth=DMA_CTR_WORD_WIDTH,//must word
	.srcwidth=DMA_CTR_WORD_WIDTH,//must word
	.src_burst_size=0,//must 0
	.read_num_en=0,
	.priority=0,
	.write_num_en=0,
	.auto_en=0,//must 0
};
/**
 * @brief     This function serves to config adc_dma_chn channel.
 * @param[in]  chn - the DMA channel
 * @return    none
 */
void adc_set_dma_config(dma_chn_e chn)
{
	adc_dma_chn = chn;
	dma_config(chn, &adc_rx_dma_config);
	dma_clr_irq_mask(adc_dma_chn,TC_MASK|ERR_MASK|ABT_MASK);
	dma_set_irq_mask(adc_dma_chn, TC_MASK);

	audio_data_fifo1_path_sel(SAR_ADC_DATA_IN_FIFO,OUT_NO_USE);//connect DMA and ADC by audio input fifo1.
}
/**
 * @brief     This function serves to start sample with adc DMA channel.
 * @param[in] adc_data_buf 	- the address of data buffer
 * @param[in] data_byte_len - the length of data size by byte
 * @return    none
 */
void adc_start_sample_dma(unsigned short *adc_data_buf,unsigned int data_byte_len)
{
	dma_set_address(adc_dma_chn,reg_fifo_buf_adr(1),(unsigned int)convert_ram_addr_cpu2bus(adc_data_buf));
	dma_set_size(adc_dma_chn,data_byte_len,DMA_WORD_WIDTH);
	dma_chn_en(adc_dma_chn);
	adc_fifo_enable();
}
/**
 * @brief     This function serves to get adc DMA sample status.
 * @return    	0: the sample is in progress.
 * 				!0: the sample is finished.
 */
unsigned char adc_get_sample_status_dma(void)
{
	return (dma_get_tc_irq_status(1<<adc_dma_chn));
}
/**
 * @brief     This function serves to clear adc DMA sample status.
 * @return    none
 */
void adc_clr_sample_status_dma(void)
{
	dma_chn_dis(adc_dma_chn);
	dma_clr_tc_irq_status(1<<adc_dma_chn);
}
/**
 * @brief     This function serves to stop adc DMA sample.
 * @return    none
 * @attention  function disable adc sample fifo, next sample should enable it.
 */
void adc_stop_sample_dma(void)
{
	adc_fifo_disable();
}
/**
 * @brief This function is used to set IO port for ADC supply or ADC IO port voltage sampling.
 * @param[in]  mode - ADC gpio pin sample mode
 * @param[in]  pin - adc_input_pin_def_e ADC input gpio pin
 * @return none
 */
void adc_pin_config(adc_input_pin_mode_e mode ,adc_input_pin_def_e pin)
{
	unsigned short adc_input_pin = pin & 0xfff;
	switch(mode)
	{
		case ADC_GPIO_MODE :
			gpio_function_en(adc_input_pin);
			gpio_input_dis(adc_input_pin);
			gpio_output_dis(adc_input_pin);
			gpio_set_low_level(adc_input_pin);
			break;
		case ADC_VBAT_MODE :
			gpio_function_en(adc_input_pin);
			gpio_input_dis(adc_input_pin);
			gpio_output_en(adc_input_pin);
			gpio_set_high_level(adc_input_pin);
			break;
	}
}
/**
 * @brief This function is used to set two IO port configuration and set it as input channel of ADC difference IO port voltage sampling.
 * @param[in]  p_pin - enum variable of ADC analog positive input IO.
 * @param[in]  n_pin - enum variable of ADC analog negative input IO.
 * @return none
 */
void adc_set_diff_pin(adc_input_pin_def_e p_pin, adc_input_pin_def_e n_pin)
{
	adc_pin_config(ADC_GPIO_MODE, p_pin);
	adc_pin_config(ADC_GPIO_MODE, n_pin);
	adc_set_diff_input(p_pin >> 12, n_pin >> 12);
}
/**
 * @brief This function serves to set the channel reference voltage.
 * @param[in]  v_ref - enum variable of ADC reference voltage.
 * @return none
 */
void adc_set_ref_voltage(adc_ref_vol_e v_ref)
{
	analog_write_reg8(areg_adc_vref, v_ref);
	if(v_ref == ADC_VREF_1P2V)
	{
		//Vref buffer bias current trimming: 		150%
		//Comparator preamp bias current trimming:  100%
		analog_write_reg8(areg_ain_scale  , (analog_read_reg8( areg_ain_scale  )&(0xC0)) | 0x3d );
		g_adc_vref = 1175;
	}
	else if(v_ref == ADC_VREF_0P9V)
	{
		//Vref buffer bias current trimming: 		100%
		//Comparator preamp bias current trimming:  100%
		analog_write_reg8( areg_ain_scale  , (analog_read_reg8( areg_ain_scale  )&(0xC0)) | 0x15 );
		g_adc_vref=900;// v_ref = ADC_VREF_0P9V,
	}
}
/**
 * @brief This function serves to set the sample frequency.
 * @param[in]  sample_freq - enum variable of ADC sample frequency.
 * @return none
 */
void adc_set_sample_rate(adc_sample_freq_e sample_freq)
{
	switch(sample_freq)
	{
		case ADC_SAMPLE_FREQ_23K :
			adc_set_state_length(1023, 15);
	/**
	* 		The length of Tsample should match the sampling frequency.
	*		changed by chaofan,confirmed by haitao.20201230.
	**/
			adc_set_tsample_cycle(ADC_SAMPLE_CYC_24);//24 adc clocks for sample cycle
			break;
		case ADC_SAMPLE_FREQ_48K :
			adc_set_state_length(490, 10);
			adc_set_tsample_cycle(ADC_SAMPLE_CYC_12);//12 adc clocks for sample cycle
			break;
		case ADC_SAMPLE_FREQ_96K :
			adc_set_state_length(240, 10);
			adc_set_tsample_cycle(ADC_SAMPLE_CYC_6);//6 adc clocks for sample cycle
			break;
	}
}
/**
 * @brief This function serves to set pre_scaling factor.
 * @param[in]  pre_scale - enum variable of ADC pre_scaling factor.
 * @return none
 */
void adc_set_scale_factor(adc_pre_scale_e pre_scale)
{
	analog_write_reg8( areg_ain_scale  , (analog_read_reg8( areg_ain_scale  )&(~FLD_SEL_AIN_SCALE)) | (pre_scale<<6) );
	g_adc_pre_scale = 1<<(unsigned char)pre_scale;
}
/**
 * @brief      This function serves to select Vbat voltage division factor
 * @param[in]  vbat_div - enum variable of Vbat division factor.
 * @return     none
 */
void adc_set_vbat_divider(adc_vbat_div_e vbat_div)
{
	analog_write_reg8(areg_adc_vref_vbat_div, (analog_read_reg8(areg_adc_vref_vbat_div)&(~FLD_ADC_VREF_VBAT_DIV)) | (vbat_div<<2) );
	if(vbat_div)
	{
		g_adc_vbat_divider=5-vbat_div;
	}
	else
	{
		g_adc_vbat_divider=1;
	}
}
/**
 * @brief This function serves to ADC init.
 * @param[in]  v_ref - enum variable of ADC reference voltage.
 * @param[in]  pre_scale - enum variable of ADC pre_scaling factor.
 * @param[in]  sample_freq - enum variable of ADC sample frequency.
 * @return none
 * @attention Many features are configured in adc_init function. But some features
 * 		such as adc_clk, resolution, tsample_cycle, we think better to set as default value,
 * 		and user don't need to change them in most use cases.
 */
void adc_init(adc_ref_vol_e v_ref,adc_pre_scale_e pre_scale,adc_sample_freq_e sample_freq)
{
	adc_power_off();//power off sar adc
	adc_reset();//reset whole digital adc module
	adc_clk_en();//enable signal of 24M clock to sar adc
	adc_set_clk(5);//default adc_clk 4M = 24M/(1+div),
	adc_set_ref_voltage(v_ref);//set channel Vref
	adc_set_scale_factor(pre_scale);//set Analog input pre-scaling
	adc_set_sample_rate(sample_freq);//set sample frequency.
	adc_set_resolution(ADC_RES14);//default adc_resolution set as 14bit ,BIT(13) is sign bit
	/**
	* 		Move the Tsample set to function adc_set_sample_rate(),because of the length of Tsample should match the sampling frequency.
	*		changed by chaofan,confirmed by haitao.20201230.
	**/
	adc_set_m_chn_en();//enable adc channel.
}
/**
 * @brief This function serves to ADC gpio sample init.
 * @param[in]  pin - adc_input_pin_def_e ADC input gpio pin
 * @param[in]  v_ref - enum variable of ADC reference voltage.
 * @param[in]  pre_scale - enum variable of ADC pre_scaling factor.
 * @param[in]  sample_freq - enum variable of ADC sample frequency.
 * @return none
 * @attention gpio voltage sample suggested initial setting are Vref = 1.2V, pre_scale = 1/4. 
 *			changed by chaofan.20201230.
 */
void adc_gpio_sample_init(adc_input_pin_def_e pin,adc_ref_vol_e v_ref,adc_pre_scale_e pre_scale,adc_sample_freq_e sample_freq)
{
	adc_init(v_ref,pre_scale,sample_freq);
	adc_set_vbat_divider(ADC_VBAT_DIV_OFF);
	adc_pin_config(ADC_GPIO_MODE, pin);
	adc_set_diff_input(pin >> 12, GND);
}
/**
 * @brief This function servers to initialized ADC temperature sensor.When the reference voltage is set to 1.2V, and
 * at the same time, the division factor is set to 1 the most accurate.
 * @return     none.
 * @attention Temperature sensor suggested initial setting are Vref = 1.2V, pre_scale = 1.
 * 			The user don't need to change it.
 */
void adc_temperature_sample_init(void)
{
	adc_init(ADC_VREF_1P2V, ADC_PRESCALE_1, ADC_SAMPLE_FREQ_96K);
	adc_set_diff_input(ADC_TEMSENSORP_EE, ADC_TEMSENSORN_EE);
	adc_set_vbat_divider(ADC_VBAT_DIV_OFF);
	adc_temp_sensor_power_on();
}

/**
 * @brief This function servers to set ADC configuration with internal Vbat channel for ADC supply voltage sampling.
 * @return none
 * @attention Vbat channel battery voltage sample suggested initial setting are Vref = 1.2V, pre_scale = 1/4, vbat_div = off.
 * 			The Vbat channel battery voltage sample range is 1.8~3.5V and is low accuracy,
 * 			and must set sys_init with the mode for battery voltage less than 3.6V.
 * 			For accurate battery voltage sampling or battery voltage > 3.6V, should use gpio sampling with some external voltage divider.
 *			Recommended configuration parameters:
 *			--3/4 external resistor voltage divider(total resistance 400k, without any capacitance),
 *			--1.2V Vref,
 *			--1/4 Scale
 *			--Sampling frequence below 48K.
 *			changed by chaofan.20201230.
 */
void adc_battery_voltage_sample_init(void)
{
	adc_init(ADC_VREF_1P2V, ADC_PRESCALE_1F4, ADC_SAMPLE_FREQ_96K);
	adc_set_vbat_divider(ADC_VBAT_DIV_OFF);
	adc_set_diff_input(ADC_VBAT, GND);
}
/**
 * @brief This function serves to start adc sample and get raw adc sample code.
 * @param[in]   sample_buffer 		- pointer to the buffer adc sample code need to store.
 * @param[in]   sample_num 			- the number of adc sample code.
 * @return 		none
 */
void adc_get_code_dma(unsigned short *sample_buffer, unsigned short sample_num)
{
	/******start adc sample********/
	adc_start_sample_dma((unsigned short *)sample_buffer, sample_num<<1);
	/******wait for adc sample finish********/
	while(!adc_get_sample_status_dma());
	/******stop dma smaple********/
	adc_stop_sample_dma();
	/******clear adc sample finished status********/
	adc_clr_sample_status_dma();//must
	/******get adc sample data and sort these data ********/
	for(int i=0;i<sample_num;i++)
	{
		if(sample_buffer[i] & BIT(13))
		{  //14 bit resolution, BIT(13) is sign bit, 1 means negative voltage in differential_mode
			sample_buffer[i] = 0;
		}
		else
		{
			sample_buffer[i] = (sample_buffer[i] & 0x1fff);  //BIT(12..0) is valid adc code
		}
	}
}
/**
 * @brief This function serves to directly get an adc sample code from analog registers.
 * 		If you want to get the sampling results twice in succession,
 *       Must ensure that the sampling interval is more than 2 times the sampling period.
 * @return 	adc_code 	- the adc sample code.
 */
unsigned short adc_get_code(void)
{
	unsigned short adc_code;
	/******Lock ADC code in analog register ********/
	analog_write_reg8(areg_adc_data_sample_control,analog_read_reg8(areg_adc_data_sample_control) | FLD_NOT_SAMPLE_ADC_DATA);
	adc_code = analog_read_reg16(areg_adc_misc_l);
	analog_write_reg8(areg_adc_data_sample_control,analog_read_reg8(areg_adc_data_sample_control) & (~FLD_NOT_SAMPLE_ADC_DATA));

	if(adc_code & BIT(13)){
		adc_code=0;
	}
	else{
		adc_code &= 0x1FFF;
	}
	return adc_code;
}
/**
 * @brief This function serves to calculate voltage from adc sample code.
 * @param[in]   adc_code	- the adc sample code.
 * @return 		adc_vol_mv 	- the average value of adc voltage value.
 */
unsigned short adc_calculate_voltage(unsigned short adc_code)
{
	//////////////// adc sample data convert to voltage(mv) ////////////////
	//                          (Vref, adc_pre_scale)   (BIT<12~0> valid data)
	//			 =  adc_code * Vref * adc_pre_scale / 0x2000
	//           =  adc_code * Vref * adc_pre_scale >>13
	return ((adc_code * g_adc_vbat_divider * g_adc_pre_scale * g_adc_vref)>>13);
}
/**
 * @brief This function serves to calculate temperature from temperature sensor adc sample code.
 * @param[in]   adc_code	 		- the temperature sensor adc sample code.
 * @return 		adc_temp_value	 	- the of temperature value.
 * attention   Temperature and adc_code are linearly related. We test four chips between -40~130 (Celsius) and got an average relationship:
 * 			Temp =  564 - ((adc_code * 819)>>13),when Vref = 1.2V, pre_scale = 1.
 */
unsigned short adc_calculate_temperature(unsigned short adc_code)
{
	//////////////// adc sample data convert to temperature(Celsius) ////////////////
	//adc_temp_value = 564 - ((adc_code * 819)>>13)
	return 564 - ((adc_code * 819)>>13);
}


