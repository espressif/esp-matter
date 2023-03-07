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




//_attribute_data_retention_	u8		adc_first_flg = 1;
_attribute_data_retention_	u8 		lowBattDet_enable = 1;
							u8      adc_hw_initialized = 0;   //note: can not be retention variable
_attribute_data_retention_  u16     batt_vol_mv;




/**
 * @brief		callback function of adjust whether allow enter to pm or not
 * @param[in]	none
 * @return      0 orbidden enter cpu_sleep_wakeup, 1 allow enter cpu_sleep_wakeup
 */
int app_suspend_enter_low_battery (void)
{
	if (!gpio_read(GPIO_WAKEUP_MODULE)) //gpio low level
	{
		analog_write(USED_DEEP_ANA_REG,  analog_read(USED_DEEP_ANA_REG)|LOW_BATT_FLG);  //mark
		return 1;//allow enter cpu_sleep_wakeup
	}

	analog_write(USED_DEEP_ANA_REG,  analog_read(USED_DEEP_ANA_REG)&(~LOW_BATT_FLG));  //clr
	return 0; //forbidden enter cpu_sleep_wakeup
}



/**
 * @brief		get the value of lowBattDet_enable
 * @param[in]	none
 * @return      the value of lowBattDet_enable
 */
int battery_get_detect_enable (void)
{
	return lowBattDet_enable;
}


/**
 * @brief		This is battery check function
 * @param[in]	alram_vol_mv - input battery calue
 * @return      0 fail 1 success
 */
_attribute_ram_code_ int app_battery_power_check(u16 alram_vol_mv)
{


	//when MCU powered up or wakeup from deep/deep with retention, adc need be initialized
	if(!adc_hw_initialized){

		adc_hw_initialized = 1;
#if VBAT_CHANNEL_EN
		adc_battery_voltage_sample_init();
#else
		adc_gpio_sample_init(ADC_INPUT_PIN_CHN,ADC_VREF_1P2V,ADC_PRESCALE_1F4,ADC_SAMPLE_FREQ_96K);
#endif
		adc_power_on();
	}


	u32 t0 = clock_time();

	while(!clock_time_exceed(t0, 25));  //wait at least 2 sample cycle(f = 96K, T = 10.4us)

	u32 adc_result;
	u32 adc_average;
	adc_average = adc_get_code();

	if(adc_average & BIT(13)){
		adc_average=0;
		return 1;
	}
#if 1
	adc_result = adc_average;
#else  	//history data filter
	if(adc_first_flg){
		adc_result = adc_average;
		adc_first_flg = 0;
	}
	else{
		adc_result = ( (adc_result*3) + adc_average + 2 )>>2;  //filter
	}
#endif
//////////////// adc sample data convert to voltage(mv) ////////////////
	batt_vol_mv  = adc_calculate_voltage(adc_result);

	if(batt_vol_mv < alram_vol_mv){

		GPIO_WAKEUP_MODULE_LOW;
		bls_pm_registerFuncBeforeSuspend( &app_suspend_enter_low_battery );
//		bls_pm_registerFuncBeforeSuspend( NULL );

		cpu_set_gpio_wakeup (GPIO_WAKEUP_MODULE, Level_High, 1);  //drive pin pad high wakeup deepsleep

		cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);  //deepsleep
//		cpu_sleep_wakeup(DEEPSLEEP_MODE, 0, 0);  //deepsleep
		return 1;
	}
	else{ // batt level > alarm level
		analog_write(USED_DEEP_ANA_REG,  analog_read(USED_DEEP_ANA_REG)&(~LOW_BATT_FLG));  //clr
		return 0;
	}
}

#endif
