/********************************************************************************************************
 * @file     battery_check.c 
 *
 * @brief    This is the source file for BLE SDK
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

#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"

#include "battery_check.h"


#if (BATT_CHECK_ENABLE)

_attribute_data_retention_	u8 		lowBattDet_enable = 1;
							u8      adc_hw_initialized = 0;   //note: can not be retention variable
_attribute_data_retention_  u16     batt_vol_mv;



void battery_set_detect_enable (int en)
{
	lowBattDet_enable = en;

	if(!en){
		adc_hw_initialized = 0;   //need initialized again
	}

}


int battery_get_detect_enable (void)
{
	return lowBattDet_enable;
}

extern unsigned short g_adc_vref;
extern unsigned char g_adc_pre_scale;
extern unsigned char g_adc_vbat_divider;

_attribute_ram_code_ void adc_bat_detect_init(void)
{

	/******power off sar adc********/
	adc_power_off();

	//reset whole digital adc module
	adc_reset();

	/******set adc sample clk as 4MHz******/
	adc_clk_en();//enable signal of 24M clock to sar adc
	adc_set_clk(5);//default adc_clk 4M = 24M/(1+div),

	//set misc channel vref 1.2V
	analog_write_reg8(areg_adc_vref, ADC_VREF_1P2V);
	analog_write_reg8(areg_ain_scale  , (analog_read_reg8( areg_ain_scale  )&(0xC0)) | 0x3d );
	g_adc_vref = 1175;

#if VBAT_CHANNEL_EN//vbat mode, vbat channel
	//set Analog input pre-scaling,ADC_PRESCALE_1F4
	analog_write_reg8( areg_ain_scale  , (analog_read_reg8( areg_ain_scale  )&(~FLD_SEL_AIN_SCALE)) | (ADC_PRESCALE_1<<6) );
	g_adc_pre_scale = 1<<(unsigned char)ADC_PRESCALE_1;
#else
	//set Analog input pre-scaling,ADC_PRESCALE_1F4
	analog_write_reg8( areg_ain_scale  , (analog_read_reg8( areg_ain_scale  )&(~FLD_SEL_AIN_SCALE)) | (ADC_PRESCALE_1F4<<6) );
	g_adc_pre_scale = 1<<(unsigned char)ADC_PRESCALE_1F4;
#endif

	//set sample frequency.96k
	adc_set_state_length(240, 10);

	//default adc_resolution set as 14bit ,BIT(13) is sign bit
	adc_set_resolution(ADC_RES14);

	//set misc t_sample 6 cycle of adc clock:  6 * 1/4M
	adc_set_tsample_cycle(ADC_SAMPLE_CYC_6);

	//enable adc channel.
	adc_set_m_chn_en();

#if VBAT_CHANNEL_EN//vbat mode, vbat channel
	//set vbat divider : ADC_VBAT_DIV_1F3
	analog_write_reg8(areg_adc_vref_vbat_div, (analog_read_reg8(areg_adc_vref_vbat_div)&(~FLD_ADC_VREF_VBAT_DIV)) | (ADC_VBAT_DIV_1F3<<2) );
	g_adc_vbat_divider = 5-ADC_VBAT_DIV_1F3;

	adc_set_diff_input(ADC_VBAT, GND);
#else//base mode, gpio channel
	//set vbat divider : ADC_VBAT_DIV_OFF
	analog_write_reg8(areg_adc_vref_vbat_div, (analog_read_reg8(areg_adc_vref_vbat_div)&(~FLD_ADC_VREF_VBAT_DIV)) | (ADC_VBAT_DIV_OFF<<2) );
	g_adc_vbat_divider = 1;

	adc_set_diff_input(ADC_INPUT_PIN_CHN>>12, GND);
#endif
	/******power on sar adc********/
	//note: this setting must be set after all other settings
	adc_power_on();

	//wait at least 2 sample cycle(f = 96K, T = 10.4us)
	sleep_us(25);
}

#if (BAT_LEAKAGE_PROTECT_EN)
_attribute_ram_code_ void app_battery_power_check(u16 threshold_deep_vol_mv, u16 threshold_suspend_vol_mv)
{


	//when MCU powered up or wakeup from deep/deep with retention, adc need be initialized
	if(!adc_hw_initialized){
		adc_hw_initialized = 1;
		adc_bat_detect_init();
	}
	//Note:25us should be reserved between each reading(wait at least 2 sample cycle(f = 96K, T = 10.4us)).
	//The sdk is only sampled once, and the user can redesign the filtering algorithm according to the actual application.
	unsigned short adc_misc_data;
	u32 adc_result;
	u32 adc_average;
	u8 ana_read_f3 = analog_read_reg8(areg_adc_data_sample_control);
	analog_write_reg8(areg_adc_data_sample_control, ana_read_f3 | FLD_NOT_SAMPLE_ADC_DATA);
	adc_misc_data = analog_read_reg16(areg_adc_misc_l);
	analog_write_reg8(areg_adc_data_sample_control, ana_read_f3 & (~FLD_NOT_SAMPLE_ADC_DATA));


	if(adc_misc_data & BIT(13)){
		adc_misc_data=0;
		return;
	}

	adc_average = adc_misc_data;
	adc_result = adc_average;
////////////////// adc sample data convert to voltage(mv) ////////////////
	//(adc_result * g_adc_vref * g_adc_vbat_divider * g_adc_pre_scale) >> 13
#if VBAT_CHANNEL_EN
//	batt_vol_mv  = (adc_result * 1175 * 3)>>13;
	batt_vol_mv  = adc_calculate_voltage(adc_result);
#else
//	batt_vol_mv  = (adc_result * 1175)>>10;
	batt_vol_mv  = adc_calculate_voltage(adc_result);
#endif

	u8 analog_used_deep_reg = analog_read_reg8(USED_DEEP_ANA_REG);
	if(batt_vol_mv < threshold_deep_vol_mv){
		u32 pin[] = KB_DRIVE_PINS;
		for (int i=0; i<(sizeof (pin)/sizeof(*pin)); i++)
		{
			cpu_set_gpio_wakeup (pin[i], Level_High, 1);  //drive pin pad high wakeup deepsleep
		}

		if(batt_vol_mv <= threshold_suspend_vol_mv)//  bat_vol<1.8v  enter suspend
		{
			analog_write_reg8(USED_DEEP_ANA_REG,  analog_used_deep_reg | LOW_BATT_FLG | LOW_BATT_SUSPEND_FLG);  //mark
			cpu_sleep_wakeup(SUSPEND_MODE, PM_WAKEUP_PAD, 0);  //suspend
		}
		else  //1.8v<bat_vol<2.0v  enter deep sleep
		{
			analog_write_reg8(USED_DEEP_ANA_REG,  analog_used_deep_reg | (LOW_BATT_FLG & (~LOW_BATT_SUSPEND_FLG)));  //mark
			cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);  //deepsleep
		}
	}
	else
	{
		analog_write_reg8(USED_DEEP_ANA_REG,  analog_used_deep_reg & (~(LOW_BATT_FLG | LOW_BATT_SUSPEND_FLG)));  //mark
	}

}
#else
_attribute_ram_code_ void app_battery_power_check(u16 alram_vol_mv)
{
	u16 temp;
	int i,j;

	//when MCU powered up or wakeup from deep/deep with retention, adc need be initialized
	if(!adc_hw_initialized){
		adc_hw_initialized = 1;
		adc_bat_detect_init();
	}

	//Note:25us should be reserved between each reading(wait at least 2 sample cycle(f = 96K, T = 10.4us)).
	//The sdk is only sampled once, and the user can redesign the filtering algorithm according to the actual application.
	unsigned short adc_misc_data;
	u32 adc_result;
	u32 adc_average;
	u8 ana_read_f3 = analog_read_reg8(areg_adc_data_sample_control);
	analog_write_reg8(areg_adc_data_sample_control, ana_read_f3 | FLD_NOT_SAMPLE_ADC_DATA);
	adc_misc_data = analog_read_reg16(areg_adc_misc_l);
	analog_write_reg8(areg_adc_data_sample_control, ana_read_f3 & (~FLD_NOT_SAMPLE_ADC_DATA));


	if(adc_misc_data & BIT(13)){
		adc_misc_data=0;
		return;
	}

	adc_average = adc_misc_data;
	adc_result = adc_average;
////////////////// adc sample data convert to voltage(mv) ////////////////
	//(adc_result * g_adc_vref * g_adc_vbat_divider * g_adc_pre_scale) >> 13
#if VBAT_CHANNEL_EN
	batt_vol_mv  = (adc_result * 1175 * 3)>>13;
#else
	batt_vol_mv  = (adc_result * 1175)>>10;
#endif

	if(batt_vol_mv < alram_vol_mv){

		analog_write_reg8(USED_DEEP_ANA_REG,  analog_read_reg8(USED_DEEP_ANA_REG) | LOW_BATT_FLG);  //mark

		u32 pin[] = KB_DRIVE_PINS;
		for (int i=0; i<(sizeof (pin)/sizeof(*pin)); i++)
		{
			cpu_set_gpio_wakeup (pin[i], Level_High, 1);  //drive pin pad high wakeup deepsleep
		}

		cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);  //deepsleep
	}

}
#endif
#endif
