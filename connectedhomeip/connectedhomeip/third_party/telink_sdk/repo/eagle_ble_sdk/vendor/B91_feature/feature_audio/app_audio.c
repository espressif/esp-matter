/********************************************************************************************************
 * @file	app_audio.c
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
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"

#include "app_config.h"
#include "app.h"
#include "app_buffer.h"
#include "application/keyboard/keyboard.h"
#include "application/usbstd/usbkeycode.h"
#include "app_att.h"


#include "application/audio/tl_audio.h"
#include "application/audio/audio_config.h"
#include "app_audio.h"

#if (FEATURE_TEST_MODE == TEST_AUDIO)

extern	int device_in_connection_state;

extern	u32	interval_update_tick;

extern	u32	latest_user_event_tick;

extern	u8	ui_mic_enable;

extern	u8 	key_voice_press;

extern	int ui_mtu_size_exchange_req;

extern	u32 key_voice_pressTick;

dma_chain_config_t rx_dma_list_config[2];
dma_chain_config_t tx_dma_list_config[2];
extern u8 audio_tx_dma_chn;
extern u8 audio_rx_dma_chn;
volatile u8 amic_enable;

#if(MICPHONE_SELECT == BLE_DMIC_SELECT)
/**
 * @brief      the func serve to init dmic
 * @param[in]  none
 * @return     none
 */
void audio_dmic_init()
{
	audio_set_codec_in_path_a_d_gain(CODEC_IN_D_GAIN_20_DB,CODEC_IN_A_GAIN_0_DB);
	audio_set_dmic_pin(DMIC_D4_DAT_D5_CLK);
	audio_init(DMIC_IN_TO_BUF ,AUDIO_16K,MONO_BIT_16);
	audio_rx_dma_chain_init(DMA2,(u16*)&buffer_mic,TL_MIC_BUFFER_SIZE);
}
#elif(MICPHONE_SELECT == BLE_AMIC_SELECT)

/**
 * @brief      the func serve to init amic
 * @param[in]  en   0:close the micphone  1:open the micphone
 * @return     none
 */
void audio_amic_init(void)
{
	audio_set_codec_in_path_a_d_gain(CODEC_IN_D_GAIN_20_DB,CODEC_IN_A_GAIN_0_DB);//recommend setting dgain:20db,again 0db
	audio_init(AMIC_IN_TO_BUF_TO_LINE_OUT ,AUDIO_16K,MONO_BIT_16);
	audio_rx_dma_chain_init(DMA2,(u16*)buffer_mic,TL_MIC_BUFFER_SIZE);
}
#endif

/**
 * @brief      power off the codec,close dma and clk
 * @param[in]  none
 * @return     none
 */
void audio_mic_off()//
{
	audio_clk_en(0,0);
	audio_rx_dma_dis();
	audio_codec_adc_power_down();
}


#if (TL_AUDIO_MODE == TL_AUDIO_RCU_ADPCM_GATT_TLEINK)					//GATT Telink

	u32 audio_stick = 0;
	_attribute_data_retention_	u8  audio_start = 0;

	/**
	 * @brief      for open the audio and mtu size exchange
	 * @param[in]  en   0:close the micphone  1:open the micphone
	 * @return     none
	 */
	void ui_enable_mic (int en)
	{
		ui_mic_enable = en;

		#if (BLT_APP_LED_ENABLE)
			device_led_setup(led_cfg[en ? LED_AUDIO_ON : LED_AUDIO_OFF]);
		#endif
		gpio_write(GPIO_LED_BLUE,en);
		if(en){  //audio on
			///////////////////// AUDIO initialization///////////////////
			#if (MICPHONE_SELECT == BLE_DMIC_SELECT)  //Dmic config
				audio_dmic_init();
			#else  //Amic config
				audio_amic_init();
			#endif
		}
		else{  //audio off
			#if (MICPHONE_SELECT == BLE_DMIC_SELECT)  //Dmic config
				audio_mic_off();
			#else  //audio off
				audio_mic_off();
			#endif
		}

		#if (BATT_CHECK_ENABLE)
			battery_set_detect_enable(!en);
		#endif
	}

	/**
	 * @brief      for open the audio and mtu size exchange
	 * @param[in]  none
	 * @return     none
	 */
	void voice_press_proc(void)
	{
		key_voice_press = 0;
		ui_enable_mic (1);
		audio_stick = clock_time()|1;
	}

	/**
	 * @brief      audio task in loop for encode and transmit encode data
	 * @param[in]  none
	 * @return     none
	 */
	void task_audio (void)
	{
		static u32 audioProcTick = 0;
		if(clock_time_exceed(audioProcTick, 500)){
			audioProcTick = clock_time();
		}
		else{
			return;
		}
		///////////////////////////////////////////////////////////////
		proc_mic_encoder ();

		if (blc_ll_getTxFifoNumber() < 9)
		{
			int *p = mic_encoder_data_buffer ();
			if (p)					//around 3.2 ms @16MHz clock
			{
				if( BLE_SUCCESS == blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, AUDIO_MIC_INPUT_DP_H, (u8*)p, ADPCM_PACKET_LEN) ){
					mic_encoder_data_read_ok();
//					DBG_C HN5_TOGGLE;
				}
			}
		}
#if MCU_STALL_ENABLE
		if(!blc_ll_isBrxBusy())
		{
			asm("WFI");
		}
#endif
	}

	/**
	 * @brief      This function serves to Request ConnParamUpdate
	 * @param[in]  none
	 * @return     none
	 */
	void blc_checkConnParamUpdate(void)
	{
		extern u32 interval_update_tick;

		if(	 interval_update_tick && clock_time_exceed(interval_update_tick,5*1000*1000) && \
			 blc_ll_getCurrentState() == BLS_LINK_STATE_CONN &&  bls_ll_getConnectionInterval()!= 8 )
		{
			interval_update_tick = clock_time() | 1;
			bls_l2cap_requestConnParamUpdate (8, 8, 99, 400);
		}
	}

	/**
	 * @brief      audio proc in main loop
	 * @param[in]  none
	 * @return     none
	 */
	void proc_audio(void){
		//blc_checkConnParamUpdate();
		if(ui_mic_enable){
			if(audio_start || (audio_stick && clock_time_exceed(audio_stick, 380*1000))){
				audio_start = 1;
				task_audio();
			}
		}
		else{
			audio_start = 0;
		}
	}
#elif (TL_AUDIO_MODE == TL_AUDIO_RCU_ADPCM_GATT_GOOGLE) 				// GATT GOOGLE
#include "application/audio/gl_audio.h"
extern u8		buffer_mic_pkt_wptr;
extern u8		buffer_mic_pkt_rptr;
extern u16		app_audio_sync_serial;
extern u32		app_audio_timer;


extern u16		app_audio_sync_serial;
extern u32		app_audio_timer;

_attribute_data_retention_ u8    audio_send_index = 0;
_attribute_data_retention_ u32   audio_stick = 0;
_attribute_data_retention_ u8  audio_start = 0;

/**
 * @brief      for open the audio and mtu size exchange
 * @param[in]  en   0:close the micphone  1:open the micphone
 * @return     none
 */
void ui_enable_mic (int en)
{
	ui_mic_enable = en;

	#if (BLT_APP_LED_ENABLE)
		device_led_setup(led_cfg[en ? LED_AUDIO_ON : LED_AUDIO_OFF]);
	#endif
	gpio_write(GPIO_LED_BLUE,en);
	if(en){  //audio on
		app_audio_sync_serial = 0;
		bls_pm_setManualLatency(0);
		bls_pm_setSuspendMask(SUSPEND_DISABLE);
		app_audio_timer = clock_time() | 1;
		audio_stick = clock_time() | 1;
		extern u16	adpcm_serial_num;
		adpcm_serial_num = 0;
		audio_send_index = 0;
		///////////////////// AUDIO initialization///////////////////
		#if (MICPHONE_SELECT == BLE_DMIC_SELECT)  //Dmic config
			audio_dmic_init();
		#else  //Amic config
			audio_amic_init();
		#endif
	}
	else{  //audio off
		audio_stick = 0;
		app_audio_timer = 0;
		audio_start = 0;
		#if (MICPHONE_SELECT == BLE_DMIC_SELECT)  //Dmic config
			audio_mic_off();
		#else  //audio off
			audio_mic_off();
		#endif
	}

	#if (BATT_CHECK_ENABLE)
		battery_set_detect_enable(!en);
	#endif
}

void voice_key_press(void)
{
	if(!ui_mic_enable && blc_ll_getCurrentState() == BLS_LINK_STATE_CONN){

		if(app_audio_key_start(1) == APP_AUDIO_ENABLE){
			ui_enable_mic(1);
		}
	}
}

void voice_key_release(void)
{
	if(ui_mic_enable){
		if(app_audio_key_start(0) == APP_AUDIO_DISABLE){
			ui_enable_mic(0);
		}
	}
}

_attribute_ram_code_ void task_audio (void)
{
	static u32 audioProcTick = 0;
	if(clock_time_exceed(audioProcTick, 500)){
		audioProcTick = clock_time();
	}
	else{
		return;
	}

	///////////////////////////////////////////////////////////////
	if(app_audio_timeout_proc()){
		return;
	}
	proc_mic_encoder ();
	u8 audio_send_length;
	//////////////////////////////////////////////////////////////////
	if (blc_ll_getTxFifoNumber() < 7)
	{
		int *p = mic_encoder_data_buffer ();
		if(p)
		{
			for(u8 i=0; i<7; i++)
			{
				if(audio_send_index < 6)
				{
					audio_send_length = 20;

				}
				else if(audio_send_index == 6)
				{
					audio_send_length = 14;
				}
				else
				{
					audio_send_length = 0;
				}
				if(BLE_SUCCESS == blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, AUDIO_GOOGLE_RX_DP_H, (u8*)p+audio_send_index*20, audio_send_length))
				{
					audio_send_index++;
				}
				else
				{
					return ;
				}
				if(audio_send_index == 7)
				{
					audio_send_index = 0;
					mic_encoder_data_read_ok();
				}
			}

		}
	}
}

void proc_audio(void){

	if(ui_mic_enable){
		if(audio_start || (audio_stick && clock_time_exceed(audio_stick, 380*1000))){// for 8278
			audio_start = 1;
			task_audio();
		}
	}
	else{
		audio_start = 0;
	}

}




#elif ((TL_AUDIO_MODE == TL_AUDIO_RCU_ADPCM_HID) || (TL_AUDIO_MODE == TL_AUDIO_RCU_ADPCM_HID_DONGLE_TO_STB))	//HID Service,ADPCM
	u8		audio_start_status = 0;
	u8		audio_end_status = 0;
	u32 	audio_stick = 0;
	_attribute_data_retention_	u8  audio_start = 0;
	_attribute_data_retention_	u8  audio_bt_status = 0;
	u8 audio_send_idx = 0;
	u32 audio_end_tick = 0;

	extern u8		buffer_mic_pkt_wptr;
	extern u8		buffer_mic_pkt_rptr;

	/**
	 * @brief      for open the audio and mtu size exchange
	 * @param[in]  en   0:close the micphone  1:open the micphone
	 * @return     none
	 */
	void ui_enable_mic (int en)
	{
		ui_mic_enable = en;

		#if (BLT_APP_LED_ENABLE)
			device_led_setup(led_cfg[en ? LED_AUDIO_ON : LED_AUDIO_OFF]);
		#endif
		gpio_write(GPIO_LED_BLUE,en);
		if(en){  //audio on
			audio_mic_param_init();
			audio_send_idx = 0;
			audio_end_tick = 0;
			buffer_mic_pkt_rptr = buffer_mic_pkt_wptr = 0;
			///////////////////// AUDIO initialization///////////////////
			#if (MICPHONE_SELECT == BLE_DMIC_SELECT)  //Dmic config
				audio_dmic_init();
			#else  //Amic config
				audio_amic_init();
			#endif
		}
		else{  //audio off
			buffer_mic_pkt_rptr = buffer_mic_pkt_wptr = 0;
			audio_start_status = 0;
			audio_end_status = 0;
			audio_stick = 0;
			audio_end_tick = 0;
			audio_bt_status = APP_AUDIO_BT_CLOSE;
			#if (MICPHONE_SELECT == BLE_DMIC_SELECT)  //Dmic config
				audio_mic_off();
			#else  //audio off
				audio_mic_off();
			#endif
		}

		#if (BATT_CHECK_ENABLE)
			battery_set_detect_enable(!en);
		#endif
	}

	void voice_key_press(void)
	{

		if(!ui_mic_enable && blc_ll_getCurrentState() == BLS_LINK_STATE_CONN){
			if(audio_start_status == 0){

				u8 value[20]={0x99, 0x99, 0x99, 0x21, };// AC_SEARCH
				if(blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, HID_CONSUME_REPORT_INPUT_DP_H, value, 20)){
					audio_start_status = 2; //push notify fail
				}
				else{
					audio_start_status = 1; //push notify success
				}
			}
		}

	}

	void voice_key_release(void)
	{
		if(ui_mic_enable && (audio_end_status == 0)){
			u8 value[20]={0x99, 0x99, 0x99, 0x24, };// AC_SEARCH
			if (blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, HID_CONSUME_REPORT_INPUT_DP_H, value, 20)){
				audio_end_status = 2;	//push notify fail
			}
			else{
				audio_end_status = 1;	//push notify success
			}
			audio_end_tick = clock_time() | 1;
		}
	}

	void audio_state_check(void)
	{
		if (audio_start_status == 2){//press
			u8 value[20]={0x99, 0x99, 0x99, 0x21, };// AC_SEARCH
			if(!blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, HID_CONSUME_REPORT_INPUT_DP_H, value, 20)){
				audio_start_status = 2; //push notify success
			}
		}
		if(audio_end_status == 2){//release
			u8 value[20]={0x99, 0x99, 0x99, 0x24, };// AC_SEARCH
			if (!blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, HID_CONSUME_REPORT_INPUT_DP_H, value, 20)){
				audio_end_status = 0;	//push notify success
			}
		}
	}

	int server2client_auido_proc(u16 connHandle,void* p)
	{
		rf_packet_att_data_t *pw = (rf_packet_att_data_t *)p;

		if (pw->dat[0] == APP_AUDIO_BT_OPEN){  //audio start
			if(!ui_mic_enable){
				audio_bt_status = APP_AUDIO_BT_OPEN;
			}

		}
		else if(pw->dat[0] == APP_AUDIO_BT_CLOSE){
			audio_bt_status = APP_AUDIO_BT_CLOSE;
		}
//		else if(pw->dat[0] == APP_AUDIO_BT_CONFIG){
//
//		}
//		else{
//			return 1;
//		}
		return 0;
	}


	_attribute_ram_code_ void task_audio (void)
	{
		static u32 audioProcTick = 0;
		if(clock_time_exceed(audioProcTick, 500)){
			audioProcTick = clock_time();
		}
		else{
			return;
		}



		proc_mic_encoder ();

		//////////////////////////////////////////////////////////////////
		if (blc_ll_getTxFifoNumber() < 8 + audio_send_idx)//8 + audio_send_idx
		{
			int *p = mic_encoder_data_buffer ();
			if (p)					//around 3.2 ms @16MHz clock
			{
				u8 i =0;
				u8 handle;
				u8 *buff;
				for(i=0;i<12;i++){
					handle = HID_AUDIO_REPORT_INPUT_FIRST_DP_H + (audio_send_idx%3)*4;//+ (audio_send_idx%3)*4
					buff = ((u8*)(p))+20*audio_send_idx;
					if(BLE_SUCCESS == blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, handle, buff, 20)){
						audio_send_idx++;
					}
					if(audio_send_idx == 6){
						audio_send_idx = 0;
						buffer_mic_pkt_rptr++;
						return;
					}
				}
			}
		}
	}

	void proc_audio(void){
		if(!ui_mic_enable && key_voice_pressTick && clock_time_exceed(key_voice_pressTick,3*1000*1000)){
			audio_start_status = 0;
			audio_end_status = 0;
			return;
		}
		if(ui_mic_enable && audio_stick && clock_time_exceed(audio_stick, 100*1000*1000)){
			ui_enable_mic (0);
			return;
		}
		if(!key_voice_press && ui_mic_enable && audio_end_tick && clock_time_exceed(audio_end_tick, 200*1000)){
			ui_enable_mic (0);
			return;
		}
		if((!ui_mic_enable) && audio_bt_status == APP_AUDIO_BT_OPEN){
			audio_stick = clock_time()|1;
			ui_enable_mic (1);
		}
		if(ui_mic_enable){
			if(audio_start || (audio_stick && clock_time_exceed(audio_stick, 380*1000))){
				audio_start = 1;
				task_audio();
			}
		}
		else{
			audio_start = 0;
		}

	}
#elif ((TL_AUDIO_MODE == TL_AUDIO_RCU_SBC_HID) || (TL_AUDIO_MODE == TL_AUDIO_RCU_SBC_HID_DONGLE_TO_STB))	//HID Service, SBC
	u8		audio_start_status = 0;
	u8		audio_end_status = 0;
	u32 	audio_stick = 0;
	_attribute_data_retention_	u8  audio_start = 0;
	_attribute_data_retention_	u8  audio_bt_status = 0;
	u8 audio_send_idx = 0;
	u32 audio_end_tick = 0;

	extern u8		buffer_mic_pkt_wptr;
	extern u8		buffer_mic_pkt_rptr;

	/**
	 * @brief      for open the audio and mtu size exchange
	 * @param[in]  en   0:close the micphone  1:open the micphone
	 * @return     none
	 */
	void ui_enable_mic (int en)
	{
		ui_mic_enable = en;

		#if (BLT_APP_LED_ENABLE)
			device_led_setup(led_cfg[en ? LED_AUDIO_ON : LED_AUDIO_OFF]);
		#endif
		gpio_write(GPIO_LED_BLUE,en);
		if(en){  //audio on
			audio_mic_param_init();
			audio_send_idx = 0;
			audio_end_tick = 0;
			///////////////////// AUDIO initialization///////////////////
			#if (MICPHONE_SELECT == BLE_DMIC_SELECT)  //Dmic config
				audio_dmic_init();
			#else  //Amic config
				audio_amic_init();
			#endif
		}
		else{  //audio off
			buffer_mic_pkt_rptr = buffer_mic_pkt_wptr = 0;
			audio_start_status = 0;
			audio_end_status = 0;
			audio_stick = 0;
			audio_end_tick = 0;
			audio_bt_status = APP_AUDIO_BT_CLOSE;
			#if (MICPHONE_SELECT == BLE_DMIC_SELECT)  //Dmic config
				audio_mic_off();
			#else  //audio off
				audio_mic_off();
			#endif
		}

		#if (BATT_CHECK_ENABLE)
			battery_set_detect_enable(!en);
		#endif
	}

	void voice_key_press(void)
	{

		if(!ui_mic_enable && blc_ll_getCurrentState() == BLS_LINK_STATE_CONN){
			if(audio_start_status == 0){
				u8 value[20]={0x99, 0x99, 0x99, 0x31, };// AC_SEARCH
				if(blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, HID_CONSUME_REPORT_INPUT_DP_H, value, 20)){
					audio_start_status = 2; //push notify fail
				}
				else{
					audio_start_status = 1; //push notify success
				}
			}
		}
	}

	void voice_key_release(void)
	{
		if(ui_mic_enable && (audio_end_status == 0)){
			u8 value[20]={0x99, 0x99, 0x99, 0x34, };// AC_SEARCH
			if (blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, HID_CONSUME_REPORT_INPUT_DP_H, value, 20)){
				audio_end_status = 2;	//push notify fail
			}
			else{
				audio_end_status = 1;	//push notify success
			}
			audio_end_tick = clock_time() | 1;
		}
	}

	void audio_state_check(void)
	{
		if (audio_start_status == 2){//press
			u8 value[20]={0x99, 0x99, 0x99, 0x31, };// AC_SEARCH
			if(blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE,HID_CONSUME_REPORT_INPUT_DP_H, value, 20)==BLE_SUCCESS){
				audio_start_status = 2; //push notify success
			}
		}
		if(audio_end_status == 2){//release
			u8 value[20]={0x99, 0x99, 0x99, 0x34, };// AC_SEARCH
			if (blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE,HID_CONSUME_REPORT_INPUT_DP_H, value, 20)==BLE_SUCCESS){
				audio_end_status = 0;	//push notify success
			}
		}
	}

	int server2client_auido_proc(u16 connHandle, void* p)
	{
		rf_packet_att_data_t *pw = (rf_packet_att_data_t *)p;

		if (pw->dat[0] == APP_AUDIO_BT_OPEN){  //audio start
			if(!ui_mic_enable){
				audio_bt_status = APP_AUDIO_BT_OPEN;
			}

		}
		else if(pw->dat[0] == APP_AUDIO_BT_CLOSE){
			audio_bt_status = APP_AUDIO_BT_CLOSE;
		}
		return 0;
	}


	_attribute_ram_code_ void task_audio (void)
	{
		static u32 audioProcTick = 0;
		if(clock_time_exceed(audioProcTick, 500)){
			audioProcTick = clock_time();
		}
		else{
			return;
		}

		///////////////////////////////////////////////////////////////


		proc_mic_encoder ();

		//////////////////////////////////////////////////////////////////
		if (blc_ll_getTxFifoNumber() < 12)//8 + audio_send_idx
		{
			int *p = mic_encoder_data_buffer ();
			if (p)					//around 3.2 ms @16MHz clock
			{
				u8 i =0;
				u8 handle;
				for(i=0;i<3;i++){
					handle = HID_AUDIO_REPORT_INPUT_FIRST_DP_H + (audio_send_idx%3)*4;//+ (audio_send_idx%3)*4
					if(BLE_SUCCESS == blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, handle, (u8*)p +3, 20)){
						audio_send_idx++;
						buffer_mic_pkt_rptr++;
						if(audio_send_idx == 3){
							audio_send_idx = 0;
						}
						return;
					}
				}
			}
		}
	}

	void proc_audio(void){
		if(!ui_mic_enable && key_voice_pressTick && clock_time_exceed(key_voice_pressTick,3*1000*1000)){
			key_voice_pressTick = 0;
			audio_start_status = 0;
			audio_end_status = 0;
			return;
		}
		if(ui_mic_enable && audio_stick && clock_time_exceed(audio_stick, 100*1000*1000)){
			ui_enable_mic (0);
			return;
		}
		if(!key_voice_press && ui_mic_enable && audio_end_tick && clock_time_exceed(audio_end_tick, 200*1000)){
			ui_enable_mic (0);
			return;
		}
		if((!ui_mic_enable) && audio_bt_status == APP_AUDIO_BT_OPEN){
			audio_stick = clock_time()|1;
			ui_enable_mic (1);
		}
		if(ui_mic_enable){
			if(audio_start || (audio_stick && clock_time_exceed(audio_stick, 380*1000))){
				audio_start = 1;
				task_audio();
			}
		}
		else{
			audio_start = 0;
		}

	}
#elif (TL_AUDIO_MODE == TL_AUDIO_RCU_MSBC_HID)					//HID Service, MSBC

		u8		audio_start_status = 0;
		u8		audio_end_status = 0;
		u32 	audio_stick = 0;
		_attribute_data_retention_	u8  audio_start = 0;
		_attribute_data_retention_	u8  audio_bt_status = 0;
		u8 audio_send_idx = 0;
		u32 audio_end_tick = 0;

		extern u8		buffer_mic_pkt_wptr;
		extern u8		buffer_mic_pkt_rptr;

		/**
		 * @brief      for open the audio and mtu size exchange
		 * @param[in]  en   0:close the micphone  1:open the micphone
		 * @return     none
		 */
		void ui_enable_mic (int en)
		{
			ui_mic_enable = en;

			#if (BLT_APP_LED_ENABLE)
				device_led_setup(led_cfg[en ? LED_AUDIO_ON : LED_AUDIO_OFF]);
			#endif
			gpio_write(GPIO_LED_BLUE,en);
			if(en){  //audio on
				audio_mic_param_init();
				audio_send_idx = 0;
				audio_end_tick = 0;
				///////////////////// AUDIO initialization///////////////////
				#if (MICPHONE_SELECT == BLE_DMIC_SELECT)  //Dmic config
					audio_dmic_init();
				#else  //Amic config
					audio_amic_init();
				#endif
			}
			else{  //audio off
				buffer_mic_pkt_rptr = buffer_mic_pkt_wptr = 0;
				audio_start_status = 0;
				audio_end_status = 0;
				audio_stick = 0;
				audio_end_tick = 0;
				audio_bt_status = APP_AUDIO_BT_CLOSE;
				#if (MICPHONE_SELECT == BLE_DMIC_SELECT)  //Dmic config
					audio_mic_off();
				#else  //audio off
					audio_mic_off();
				#endif
			}

			#if (BATT_CHECK_ENABLE)
				battery_set_detect_enable(!en);
			#endif
		}

		void voice_key_press(void)
		{

			if(!ui_mic_enable && blc_ll_getCurrentState() == BLS_LINK_STATE_CONN){
				if(audio_start_status == 0){

					u8 value[20]={0x99, 0x99, 0x99, 0x31, };// AC_SEARCH
					if(blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, HID_CONSUME_REPORT_INPUT_DP_H, value, 20)){
						audio_start_status = 2; //push notify fail
					}
					else{
						audio_start_status = 1; //push notify success
					}
				}
			}

		}

		void voice_key_release(void)
		{
			if(ui_mic_enable && (audio_end_status == 0)){
				u8 value[20]={0x99, 0x99, 0x99, 0x34, };// AC_SEARCH
				if (blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, HID_CONSUME_REPORT_INPUT_DP_H, value, 20)){
					audio_end_status = 2;	//push notify fail
				}
				else{
					audio_end_status = 1;	//push notify success
				}
				audio_end_tick = clock_time() | 1;
			}
		}

		void audio_state_check(void)
		{
			if (audio_start_status == 2){//press
				u8 value[20]={0x99, 0x99, 0x99, 0x31, };// AC_SEARCH
				if(!blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, HID_CONSUME_REPORT_INPUT_DP_H, value, 20)){
					audio_start_status = 2; //push notify success
				}
			}
			if(audio_end_status == 2){//release
				u8 value[20]={0x99, 0x99, 0x99, 0x34, };// AC_SEARCH
				if (!blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, HID_CONSUME_REPORT_INPUT_DP_H, value, 20)){
					audio_end_status = 0;	//push notify success
				}
			}
		}

		int server2client_auido_proc(u16 connHandle,void* p)
		{
			rf_packet_att_data_t *pw = (rf_packet_att_data_t *)p;

			if (pw->dat[0] == APP_AUDIO_BT_OPEN){  //audio start
				if(!ui_mic_enable){
					audio_bt_status = APP_AUDIO_BT_OPEN;
				}

			}
			else if(pw->dat[0] == APP_AUDIO_BT_CLOSE){
				audio_bt_status = APP_AUDIO_BT_CLOSE;
			}
			return 0;
		}


		_attribute_ram_code_ void task_audio (void)
		{

			///////////////////////////////////////////////////////////////
			proc_mic_encoder ();

			//////////////////////////////////////////////////////////////////
			if (blc_ll_getTxFifoNumber() < 12)//8 + audio_send_idx
			{
				int *p = mic_encoder_data_buffer ();
				if (p)					//around 3.2 ms @16MHz clock
				{
					u8 i =0;
					u8 handle;
					for(i=0;i<3;i++){
						handle = HID_AUDIO_REPORT_INPUT_FIRST_DP_H + (audio_send_idx%3)*4;//+ (audio_send_idx%3)*4
						if(BLE_SUCCESS == blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, handle, (u8*)p, ADPCM_PACKET_LEN)){
							audio_send_idx++;
							buffer_mic_pkt_rptr++;
							if(audio_send_idx == 3){
								audio_send_idx = 0;
							}
							return;
						}
					}
				}
			}
		}

		void proc_audio(void){
			if(!ui_mic_enable && key_voice_pressTick && clock_time_exceed(key_voice_pressTick,3*1000*1000)){
				audio_start_status = 0;
				audio_end_status = 0;
				return;
			}
			if(ui_mic_enable && audio_stick && clock_time_exceed(audio_stick, 100*1000*1000)){
				ui_enable_mic (0);
				return;
			}
			if(!key_voice_press && ui_mic_enable && audio_end_tick && clock_time_exceed(audio_end_tick, 200*1000)){
				ui_enable_mic (0);
				return;
			}
			if((!ui_mic_enable) && audio_bt_status == APP_AUDIO_BT_OPEN){
				audio_stick = clock_time()|1;
				ui_enable_mic (1);
			}
			if(ui_mic_enable){

				if(audio_start || (audio_stick && clock_time_exceed(audio_stick, 380*1000))){// for 8278
					audio_start = 1;
					task_audio();
				}
			}
			else{
				audio_start = 0;
			}

		}
#else

#endif

#endif
