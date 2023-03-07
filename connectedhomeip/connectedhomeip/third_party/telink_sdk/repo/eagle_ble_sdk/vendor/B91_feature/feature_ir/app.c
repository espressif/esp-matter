/********************************************************************************************************
 * @file	app.c
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
#include "application/keyboard/keyboard.h"
#include "application/usbstd/usbkeycode.h"
#include "app_config.h"
#include "app.h"
#include "app_buffer.h"
#include "../default_att.h"

#include "rc_ir.h"
#include "rc_ir_learn.h"


#if (FEATURE_TEST_MODE == TEST_IR)


#define		MY_RF_POWER_INDEX					RF_POWER_INDEX_P2p79dBm

/**
 * @brief	Adv Packet data
 */
const u8	tbl_advData[] = {
	 0x08, 0x09, 'f', 'e', 'a', 't', 'u', 'r', 'e',
	 0x02, 0x01, 0x05, 							// BLE limited discoverable mode and BR/EDR not supported
	 0x03, 0x19, 0x80, 0x01, 					// 384, Generic Remote Control, Generic category
	 0x05, 0x02, 0x12, 0x18, 0x0F, 0x18,		// incomplete list of service class UUIDs (0x1812, 0x180F)
};

/**
 * @brief	Scan Response Packet data
 */
const u8	tbl_scanRsp [] = {
		 0x08, 0x09, 'f', 'e', 'a', 't', 'u', 'r', 'e',
	};


/**
 * @brief      callback function of LinkLayer Event "BLT_EV_FLAG_SUSPEND_ENTER"
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
_attribute_ram_code_ void  ble_remote_set_sleep_wakeup (u8 e, u8 *p, int n)
{
	if( blc_ll_getCurrentState() == BLS_LINK_STATE_CONN && ((u32)(bls_pm_getSystemWakeupTick() - clock_time())) > 80 * SYSTEM_TIMER_TICK_1MS){  //suspend time > 30ms.add gpio wakeup
		bls_pm_setWakeupSource(PM_WAKEUP_PAD);  //gpio pad wakeup suspend/deepsleep
	}
}

/**
 * @brief      callback function of LinkLayer Event "BLT_EV_FLAG_CONNECT"
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
void	task_connect (u8 e, u8 *p, int n)
{
	bls_l2cap_requestConnParamUpdate (8, 8, 99, 400);  // 1 S

#if (UI_LED_ENABLE)
	gpio_write(GPIO_LED_RED, LED_ON_LEVAL);  //red light on
#endif
}

/**
 * @brief      callback function of LinkLayer Event "BLT_EV_FLAG_TERMINATE"
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
void 	task_terminate(u8 e,u8 *p, int n) //*p is terminate reason
{

	if(*p == HCI_ERR_CONN_TIMEOUT){

	}
	else if(*p == HCI_ERR_REMOTE_USER_TERM_CONN){  //0x13

	}
	else if(*p == HCI_ERR_CONN_TERM_MIC_FAILURE){

	}
	else{

	}


#if (UI_LED_ENABLE)
	gpio_write(GPIO_LED_RED, !LED_ON_LEVAL);  //red light off
#endif

}

/**
 * @brief      callback function of LinkLayer Event "BLT_EV_FLAG_SUSPEND_EXIT"
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
_attribute_ram_code_ void	user_set_rf_power (u8 e, u8 *p, int n)
{
	rf_set_power_level_index (MY_RF_POWER_INDEX);
}

/**
 * @brief      power management code for application
 * @param	   none
 * @return     none
 */
_attribute_ram_code_
void blt_pm_proc(void)
{

#if(BLE_APP_PM_ENABLE)
	#if (PM_DEEPSLEEP_RETENTION_ENABLE)
		bls_pm_setSuspendMask (SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
	#else
		bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
	#endif


	//do not care about keyScan/button_detect power here, if you care about this, please refer to "8258_ble_remote" demo
	#if (UI_KEYBOARD_ENABLE)
		if(scan_pin_need || key_not_released)
		{
			bls_pm_setSuspendMask (SUSPEND_DISABLE);
		}
	#endif
#endif  //end of BLE_APP_PM_ENABLE
}

#if (UI_KEYBOARD_ENABLE)



_attribute_data_retention_	int 	key_not_released;

#define CONSUMER_KEY   	   		1
#define KEYBOARD_KEY   	   		2
_attribute_data_retention_	u8 		key_type;





/**
 * @brief		this function is used to process keyboard matrix status change.
 * @param[in]	none
 * @return      none
 */
typedef enum{
	Ble_mode = 0,
	IR_mode = 1,
	IR_Learn_mode = 2
}switch_mode_e;

/**
 * @brief		this function is used to process keyboard matrix status change.
 * @param[in]	interval_led:   times_led:
 * @return      none
 */
typedef enum{
	led_off = 0,
	led_on = 1,
	led_turkle = 2
}led_state_e;

typedef struct{
	u8  tick_en;
	u8  num;
	u32 tick_time;
	u32 interval;
	u8  times;
	led_state_e stat;
	gpio_pin_e pin;
}led_typedef_t;

led_typedef_t  led_st;
led_typedef_t  *g_led_st;

void led_turkle_ctrl(led_typedef_t *led_st)
{
   if(led_st->num >= 2 * (led_st->times))
   {
	   gpio_write(led_st->pin,0);
	   led_st->stat = led_off;
   }
   else if(led_st->tick_en == 0)         // ok
   {
	   led_st->tick_time = clock_time();
	   led_st->tick_en = 1;
	   gpio_toggle(led_st->pin);	    // LED toggle
   }
   else if(clock_time_exceed(led_st->tick_time, led_st->interval))
   {
	   led_st->tick_en = 0;
	   led_st->num++;
   }
}
void led_ctrl(led_typedef_t *led_st)
{
	switch(led_st->stat)
	{
		 case led_off:
			 led_off();
			 led_st->num = 0;
			 led_st->tick_en = 0;
			 led_st->tick_time = 0;
			 g_led_st->interval = 0;
			 g_led_st->times = 0;
			 led_st->pin = GPIO_LED_BLUE;
		 break;

		 case led_on:
			 gpio_write(led_st->pin,1);
		 break;

		 case led_turkle:
			 led_turkle_ctrl(led_st);
		 break;

		 default:
			 led_off();
		 break;
	}
}
static u8 switch_key = 0;        // Ble_IR_mode:0 IR_Only_mode:1 IR_Learn_mode
ir_learn_send_t ir_learn_result;
void key_change_proc(void)
{
	u8 key0 = kb_event.keycode[0];
	u8 key_buf[8] = {0,0,0,0,0,0,0,0};

    key_not_released = 1;
	if (kb_event.cnt == 2)   //two key press, do  not process
	{

	}
	else if(kb_event.cnt == 1)
	{
		u16 consumer_key;
		led_off();
		switch(key0)
		{
			case CR_VOL_DN:

				if(switch_key == IR_mode)
				{
					g_led_st->stat = led_turkle;
					g_led_st->num = 0;
					g_led_st->tick_en = 0;
					g_led_st->tick_time = 0;
					g_led_st->interval = 500000;
					g_led_st->times = 1;
					g_led_st->pin = GPIO_LED_GREEN;
					ir_nec_send(0,1,0);
				}
				else if(switch_key == IR_Learn_mode)
				{
					if(get_ir_learn_state() == 0) // success
					{
						g_led_st->stat = led_on;
						g_led_st->num = 0;
						g_led_st->tick_en = 0;
						g_led_st->tick_time = 0;
						g_led_st->interval = 0;
						g_led_st->times = 0;
						g_led_st->pin = GPIO_LED_WHITE;

						ir_learn_copy_result(&ir_learn_result);
						ir_learn_send_init();
						ir_learn_send(&ir_learn_result);

					}
					else
					{
						g_led_st->stat = led_turkle;
						g_led_st->num = 0;
						g_led_st->tick_en = 0;
						g_led_st->tick_time = 0;
						g_led_st->interval = 500000;
						g_led_st->times = 1;
						g_led_st->pin = GPIO_LED_WHITE;
					}
				}
				else
				{
					g_led_st->stat = led_turkle;
					g_led_st->num = 0;
					g_led_st->tick_en = 0;
					g_led_st->tick_time = 0;
					g_led_st->interval = 500000;
					g_led_st->times = 1;
					g_led_st->pin = GPIO_LED_BLUE;
					key_type = CONSUMER_KEY;
					consumer_key = MKEY_VOL_UP;
					blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, HID_CONSUME_REPORT_INPUT_DP_H, (u8 *)&consumer_key, 2);
				}
				break;

			case CR_VOL_UP:

				if(switch_key == IR_mode)
				{
					g_led_st->stat = led_turkle;
					g_led_st->num = 0;
					g_led_st->tick_en = 0;
					g_led_st->tick_time = 0;
					g_led_st->interval = 500000;
					g_led_st->times = 1;
					g_led_st->pin = GPIO_LED_GREEN;
					ir_nec_send(0,1,1);
				}
				else if(switch_key == IR_Learn_mode)
				{
					if(get_ir_learn_state() == 0) // success
					{
						g_led_st->stat = led_on;
						g_led_st->num = 0;
						g_led_st->tick_en = 0;
						g_led_st->tick_time = 0;
						g_led_st->interval = 0;
						g_led_st->times = 0;
						g_led_st->pin = GPIO_LED_WHITE;

						ir_learn_copy_result(&ir_learn_result);
						ir_learn_send_init();
						ir_learn_send(&ir_learn_result);
					}
				}
				else
				{
					g_led_st->stat = led_turkle;
					g_led_st->num = 0;
					g_led_st->tick_en = 0;
					g_led_st->tick_time = 0;
					g_led_st->interval = 500000;
					g_led_st->times = 1;
					g_led_st->pin = GPIO_LED_BLUE;
					key_type = CONSUMER_KEY;
					consumer_key = MKEY_VOL_DN;
					blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, HID_CONSUME_REPORT_INPUT_DP_H, (u8 *)&consumer_key, 2);
				}
				break;

			case VK_1:
				if(switch_key == IR_mode)
				{
					g_led_st->stat = led_turkle;
					g_led_st->num = 0;
					g_led_st->tick_en = 0;
					g_led_st->tick_time = 0;
					g_led_st->interval = 500000;
					g_led_st->times = 1;
					g_led_st->pin = GPIO_LED_GREEN;
					ir_nec_send(0,1,2);

				}
				else if(switch_key == IR_Learn_mode)
				{
					g_led_st->stat = led_on;
					g_led_st->num = 0;
					g_led_st->tick_en = 0;
					g_led_st->tick_time = 0;
					g_led_st->interval = 0;
					g_led_st->times = 0;
					g_led_st->pin = GPIO_LED_WHITE;

					ir_learn_copy_result(&ir_learn_result);
					ir_learn_send_init();
					ir_learn_send(&ir_learn_result);
				}
				else
				{
					key_type = KEYBOARD_KEY;
					key_buf[2] = key0;
					blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8);
				}
				break;

			case VK_2:  // switch key

				key_type = KEYBOARD_KEY;
				key_buf[2] = key0;
				blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8);

				switch_key++;
				switch_key = (switch_key > IR_Learn_mode)? Ble_mode : switch_key;

				switch(switch_key)
				{
					case Ble_mode:
						g_led_st->stat = led_turkle;
						g_led_st->num = 0;
						g_led_st->tick_en = 0;
						g_led_st->tick_time = 0;
						g_led_st->interval = 500000;
						g_led_st->times = 3;
						g_led_st->pin = GPIO_LED_BLUE;
						rc_ir_init();
						ir_learn_stop();
						bls_ll_setAdvEnable(BLC_ADV_ENABLE);
					break;

					case IR_mode:
						g_led_st->stat = led_turkle;
						g_led_st->num = 0;
						g_led_st->tick_en = 0;
						g_led_st->tick_time = 0;
						g_led_st->interval = 500000;
						g_led_st->times = 3;
						g_led_st->pin = GPIO_LED_GREEN;
					break;

					case IR_Learn_mode:
						g_led_st->stat = led_turkle;
						g_led_st->num = 0;
						g_led_st->tick_en = 0;
						g_led_st->tick_time = 0;
						g_led_st->interval = 500000;
						g_led_st->times = 3;
						g_led_st->pin = GPIO_LED_WHITE;

						if( blc_ll_getCurrentState() == BLS_LINK_STATE_CONN )
						{
							bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN); // close connect
						}
						bls_ll_setAdvEnable(BLC_ADV_DISABLE);   // close adv

						ir_learn_init();
						ir_learn_start();

					break;
				}
				break;

			default:
				g_led_st->stat = led_turkle;
				g_led_st->num = 0;
				g_led_st->tick_en = 0;
				g_led_st->tick_time = 0;
				g_led_st->pin = GPIO_LED_BLUE;
				break;
	    }
	}
	else   //kb_event.cnt == 0,  key release
	{
		key_not_released = 0;
		if(key_type == CONSUMER_KEY)
		{
			u16 consumer_key = 0;

			blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, HID_CONSUME_REPORT_INPUT_DP_H, (u8 *)&consumer_key, 2);
		}
		else if(key_type == KEYBOARD_KEY)
		{
			key_buf[2] = 0;

			blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8); //release
		}
	}


}



_attribute_data_retention_		static u32 keyScanTick = 0;



/**
 * @brief      this function is used to detect if key pressed or released.
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
_attribute_ram_code_
void proc_keyboard (u8 e, u8 *p, int n)
{
	if(clock_time_exceed(keyScanTick, 8000)){
		keyScanTick = clock_time();
	}
	else{
		return;
	}

	kb_event.keycode[0] = 0;
	int det_key = kb_scan_key (0, 1);

	if (det_key){
		key_change_proc();
	}
	if(switch_key == IR_Learn_mode)
	{
		ir_learn_detect();
	}

}

#endif

_attribute_no_inline_ void user_init_normal(void)
{
	/* random number generator must be initiated here( in the beginning of user_init_nromal).
	 * When deepSleep retention wakeUp, no need initialize again */
	random_generator_init();  //this is must



//////////////////////////// BLE stack Initialization  Begin //////////////////////////////////
	/* for 1M   Flash, flash_sector_mac_address equals to 0xFF000
	 * for 2M   Flash, flash_sector_mac_address equals to 0x1FF000*/
	u8  mac_public[6];
	u8  mac_random_static[6];
	blc_initMacAddress(flash_sector_mac_address, mac_public, mac_random_static);


	//////////// Controller Initialization  Begin /////////////////////////
	blc_ll_initBasicMCU();                      //mandatory
	blc_ll_initStandby_module(mac_public);		//mandatory
	blc_ll_initAdvertising_module(); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initConnection_module();				//connection module  mandatory for BLE slave/master
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,

	blc_ll_setAclConnMaxOctetsNumber(ACL_CONN_MAX_RX_OCTETS, ACL_CONN_MAX_TX_OCTETS);

	blc_ll_initAclConnTxFifo(app_acl_txfifo, ACL_TX_FIFO_SIZE, ACL_TX_FIFO_NUM);
	blc_ll_initAclConnRxFifo(app_acl_rxfifo, ACL_RX_FIFO_SIZE, ACL_RX_FIFO_NUM);

	u8 check_status = blc_controller_check_appBufferInitialization();
	if(check_status != BLE_SUCCESS){
		/* here user should set some log to know which application buffer incorrect */
		write_log32(0x88880000 | check_status);
		while(1);
	}
	//////////// Controller Initialization  End /////////////////////////


	//////////// Host Initialization  Begin /////////////////////////
	/* Host Initialization */
	/* GAP initialization must be done before any other host feature initialization !!! */
	blc_gap_peripheral_init();    //gap initialization
	extern void my_att_init ();
	my_att_init (); //gatt initialization

	/* L2CAP Initialization */
	blc_l2cap_register_handler (blc_l2cap_packet_receive);

	/* SMP Initialization may involve flash write/erase(when one sector stores too much information,
	 *   is about to exceed the sector threshold, this sector must be erased, and all useful information
	 *   should re_stored) , so it must be done after battery check */
	blc_smp_peripheral_init();

	// Hid device on android7.0/7.1 or later version
	// New paring: send security_request immediately after connection complete
	// reConnect:  send security_request 1000mS after connection complete. If master start paring or encryption before 1000mS timeout, slave do not send security_request.
	blc_smp_configSecurityRequestSending(SecReq_IMM_SEND, SecReq_PEND_SEND, 1000); //if not set, default is:  send "security request" immediately after link layer connection established(regardless of new connection or reconnection)
	//////////// Host Initialization  End /////////////////////////

//////////////////////////// BLE stack Initialization  End //////////////////////////////////


	u8 status = bls_ll_setAdvParam(  ADV_INTERVAL_30MS, ADV_INTERVAL_35MS,
									 ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC,
									 0,  NULL,
									 BLT_ENABLE_ADV_ALL,
									 ADV_FP_NONE);
	if(status != BLE_SUCCESS) { while(1); }  //debug: adv setting err



	bls_ll_setAdvData( (u8 *)tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData( (u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));



	bls_ll_setAdvEnable(BLC_ADV_ENABLE);  //adv enable



	//set rf power index, user must set it after every suspend wakeup, cause relative setting will be reset in suspend
	user_set_rf_power(0, 0, 0);


	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &task_terminate);
	bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_EXIT, &user_set_rf_power);

#if REMOTE_IR_ENABLE
	rc_ir_init();
	g_led_st = &led_st;
	memset(&led_st,0,sizeof(led_typedef_t));
#endif
	///////////////////// Power Management initialization///////////////////
#if(BLE_APP_PM_ENABLE)
	blc_ll_initPowerManagement_module();

	#if (PM_DEEPSLEEP_RETENTION_ENABLE)
		bls_pm_setSuspendMask (SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
		blc_pm_setDeepsleepRetentionThreshold(95, 95);
		blc_pm_setDeepsleepRetentionEarlyWakeupTiming(360);
		//blc_pm_setDeepsleepRetentionType(DEEPSLEEP_MODE_RET_SRAM_LOW64K); //default use 32k deep retention
	#else
		bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
	#endif

	bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_ENTER, &ble_remote_set_sleep_wakeup);
#else
	bls_pm_setSuspendMask (SUSPEND_DISABLE);
#endif



#if (UI_KEYBOARD_ENABLE)
	/////////// keyboard gpio wakeup init ////////
	u32 pin[] = KB_DRIVE_PINS;
	for (int i=0; i<(sizeof (pin)/sizeof(*pin)); i++)
	{
		cpu_set_gpio_wakeup (pin[i], Level_High,1);  //drive pin pad high wakeup deepsleep
	}

	bls_app_registerEventCallback (BLT_EV_FLAG_GPIO_EARLY_WAKEUP, &proc_keyboard);
#endif

}

/**
 * @brief		user initialization when MCU wake_up from deepSleep_retention mode
 * @param[in]	none
 * @return      none
 */
_attribute_ram_code_ void user_init_deepRetn(void)
{
#if (PM_DEEPSLEEP_RETENTION_ENABLE)

	blc_ll_initBasicMCU();   //mandatory
	rf_set_power_level_index (MY_RF_POWER_INDEX);
	blc_ll_recoverDeepRetention();

	DBG_CHN0_HIGH;    //debug
	irq_enable();
	#if (UI_KEYBOARD_ENABLE)
		/////////// keyboard gpio wakeup init ////////
		u32 pin[] = KB_DRIVE_PINS;
		for (int i=0; i<(sizeof (pin)/sizeof(*pin)); i++)
		{
			cpu_set_gpio_wakeup (pin[i], Level_High,1);  //drive pin pad high wakeup deepsleep
		}
	#endif

#endif
}

/**
 * @brief		This function serves to main
 * @return 		none
 */
void main_loop(void)
{
	////////////////////////////////////// BLE entry /////////////////////////////////
	blt_sdk_main_loop();

	////////////////////////////////////// UI entry /////////////////////////////////
	proc_keyboard (0, 0, 0);

	////////////////////////////////////// PM Process /////////////////////////////////
	blt_pm_proc();

	led_ctrl(g_led_st);
}


#endif  //end of (FEATURE_TEST_MODE == ...)
