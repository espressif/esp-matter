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

#include "app_config.h"
#include "app.h"



#if (FEATURE_TEST_MODE == TEST_EMI)


//-----------------------------------------[start] EMI Segment

/*
 * @brief 	this macro definition serve to open the setting to deal with problem of zigbee mode 2480Mhz
 * 			band edge can't pass the spec.only use it at the time of certification.
 * */
#define	FIX_ZIGBEE_BANDEAGE_EN	0
#define TX_PACKET_MODE_ADDR 		     0x00000005
#define RUN_STATUE_ADDR 			     0x00000006
#define TEST_COMMAND_ADDR			     0x00000007
#define POWER_ADDR 					     0x00000008
#define CHANNEL_ADDR				     0x00000009
#define RF_MODE_ADDR				     0x0000000a
#define CD_MODE_HOPPING_CHN			     0x0000000b
#define RSSI_ADDR                        0x00000004
#define RX_PACKET_NUM_ADDR               0x0000000c
#define CAP_VALUE					     0xfe000

#define MAX_RF_CHANNEL                   40

/**
 * @brief   The emi test command and function
 */
typedef struct {
	unsigned char  cmd_id; 										/**< emi command id */
	void	 (*func)(rf_mode_e, unsigned char , signed char);   /**< emi function */
}test_list_t;

/**
 * @brief   The emi test mode.
 */
typedef enum{
	ble2m = 0,
	ble1m = 1,
	zigbee250k = 2,
	ble125K = 3,
	ble500K = 4
}emi_rf_mode_e;

/**
 * @brief   Init the emi frequency hopping channel
 */
const unsigned char rf_chn_hipping[MAX_RF_CHANNEL] = {
	48,  4, 66, 62, 44,
	12, 38, 16 ,26, 20,
	22, 68, 18, 28,	42,
	32, 34, 36, 14,	40,
	30, 54, 46, 2,	50,
	52, 80, 56, 78,	74,
	8,  64, 6,  24,	70,
	72, 60, 76, 58, 10,
};

/**
 * @brief   Init the emi reference parameter
 */
unsigned char  g_power_level = 0;  // 0
unsigned char  g_chn = 2;          // 2
unsigned char  g_cmd_now = 1;      // 1
unsigned char  g_run = 1;          // 1
unsigned char  g_hop = 0;          // 0
unsigned char  g_tx_cnt = 0;       // 0
emi_rf_mode_e  g_mode = zigbee250k;// 2

void emi_init(void);
void emicarrieronly(rf_mode_e rf_mode,unsigned char pwr,signed char rf_chn);
void emi_con_prbs9(rf_mode_e rf_mode,unsigned char pwr,signed char rf_chn);
void emirx(rf_mode_e rf_mode,unsigned char pwr,signed char rf_chn);
void emitxprbs9(rf_mode_e rf_mode,unsigned char pwr,signed char rf_chn);
void emitx55(rf_mode_e rf_mode,unsigned char pwr,signed char rf_chn);
void emitx0f(rf_mode_e rf_mode,unsigned char pwr,signed char rf_chn);
void emi_con_tx55(rf_mode_e rf_mode,unsigned char pwr,signed char rf_chn);
void emi_con_tx0f(rf_mode_e rf_mode,unsigned char pwr,signed char rf_chn);

/**
 * @brief   Init the structure of the emi test command and function
 */
test_list_t  ate_list[] = {
		{0x01,emicarrieronly},
		{0x02,emi_con_prbs9},
		{0x03,emirx},
		{0x04,emitxprbs9},
		{0x05,emitx55},
		{0x06,emitx0f},
		{0x07,emi_con_tx55},
		{0x08,emi_con_tx0f}
};

/**
 * @brief		This function serves to EMI Init
 * @return 		none
 */

void emi_init(void)
{
	rf_access_code_comm(EMI_ACCESS_CODE);             // access code

    write_sram8(TX_PACKET_MODE_ADDR,g_tx_cnt);        // tx_cnt
    write_sram8(RUN_STATUE_ADDR,g_run);               // run
    write_sram8(TEST_COMMAND_ADDR,g_cmd_now);         // cmd
    write_sram8(POWER_ADDR,g_power_level);            // power
    write_sram8(CHANNEL_ADDR,g_chn);                  // chn
    write_sram8(RF_MODE_ADDR,g_mode);                 // mode
    write_sram8(CD_MODE_HOPPING_CHN,g_hop);           // hop
    write_sram8(RSSI_ADDR,0);                         // rssi
    write_sram32(RX_PACKET_NUM_ADDR,0);               // rx_packet_num

    blc_app_loadCustomizedParameters();               // calibration internal cap value

}

/**
 * @brief		This function serves to EMI CarryOnly
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr     - power level of RF.
 * @param[in]   rf_chn  - channel of RF.
 * @return 		none
 */

void emicarrieronly(rf_mode_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	(void)(rf_mode);
	rf_mode_e  power = rf_power_Level_list[pwr];
	rf_emi_tx_single_tone(power,rf_chn);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode ));
	rf_emi_stop();
}

/**
 * @brief		This function serves to EMI ConPrbs9
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr 	- power level of RF.
 * @param[in]   rf_chn 	- channel of RF.
 * @return 		none
 */
void emi_con_prbs9(rf_mode_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	unsigned int t0 = reg_system_tick,chnidx = 1;
	rf_power_level_e power = rf_power_Level_list[pwr];
	g_hop = read_sram8(CD_MODE_HOPPING_CHN);
	rf_emi_tx_continue_update_data(rf_mode,power,rf_chn,0);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode ))
	{
#if FIX_ZIGBEE_BANDEAGE_EN
		if(rf_mode == RF_MODE_ZIGBEE_250K)
		{
			if (rf_chn == 80)
			{
				write_reg8(0x140c23,0x00);
			}
			else
			{
				write_reg8(0x140c23,0x80);
			}
		}
#endif
		rf_continue_mode_run();
		if(g_hop)
		{
			while(!clock_time_exceed(t0,10000))    // run 10ms
				rf_continue_mode_run();

			while(!clock_time_exceed(t0,20000));   // stop 20ms
			t0 = reg_system_tick;
			rf_emi_tx_continue_update_data(rf_mode,power,rf_chn_hipping[chnidx - 1],0);
			(chnidx >= MAX_RF_CHANNEL)?(chnidx = 1):(chnidx++);
		}
	}
	rf_emi_stop();

	rf_set_tx_rx_off();
	dma_reset();
	rf_emi_reset_baseband();
}

/**
 * @brief		This function serves to EMI Rx
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr	    - power level of RF.
 * @param[in]   rf_chn  - channel of RF.
 * @return 		none
 */
void emirx(rf_mode_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	(void)(pwr);
	rf_emi_rx_setup(rf_mode,rf_chn);
	write_sram8(RSSI_ADDR,0);
	write_sram32(RX_PACKET_NUM_ADDR,0);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode ))
	{
		rf_emi_rx_loop();
		if(rf_emi_get_rxpkt_cnt() != read_sram32(RX_PACKET_NUM_ADDR))
		{
		    write_sram8(RSSI_ADDR,rf_emi_get_rssi_avg());
		    write_sram32(RX_PACKET_NUM_ADDR,rf_emi_get_rxpkt_cnt());
		}
	}
	rf_emi_stop();
}

/**
 * @brief		This function serves to EMI TxPrbs
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr     - power level of RF.
 * @param[in]   rf_chn  - channel of RF.
 * @return 		none
 */

void emitxprbs9(rf_mode_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	unsigned int tx_num = 0;
	rf_power_level_e  power = rf_power_Level_list[pwr];
	rf_emi_tx_burst_setup(rf_mode,power,rf_chn,1);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode ))
	{
		rf_emi_tx_burst_loop(rf_mode,0);
		if(g_tx_cnt)
		{
			tx_num++;
			if(tx_num >= 1000)
				break;
		}
	}
	rf_emi_stop();
}

/**
 * @brief		This function serves to EMI Tx55
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr     - power level of RF.
 * @param[in]   rf_chn  - channel of RF.
 * @return 		none
 */

void emitx55(rf_mode_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	unsigned int tx_num = 0;
	rf_power_level_e  power = rf_power_Level_list[pwr];
	rf_emi_tx_burst_setup(rf_mode,power,rf_chn,2);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode ))
	{
		rf_emi_tx_burst_loop(rf_mode,2);
		if(g_tx_cnt)
		{
			tx_num++;
			if(tx_num >= 1000)
				break;
		}
	}
	rf_emi_stop();
}

/**
 * @brief		This function serves to EMI Tx0f
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr     - power level of RF.
 * @param[in]   rf_chn  - channel of RF.
 * @return 		none
 */
void emitx0f(rf_mode_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	unsigned int tx_num = 0;
	rf_power_level_e  power = rf_power_Level_list[pwr];
	rf_emi_tx_burst_setup(rf_mode,power,rf_chn,1);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode ))
	{
		rf_emi_tx_burst_loop(rf_mode,1);
		if(g_tx_cnt)
		{
			tx_num++;
			if(tx_num >= 1000)
				break;
		}
	}
	rf_emi_stop();
}

/**
 * @brief		This function serves to EMI_CON_TX55
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr     - power level of RF.
 * @param[in]   rf_chn  - channel of RF.
 * @return 		none
 */
void emi_con_tx55(rf_mode_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	rf_power_level_e power = rf_power_Level_list[pwr];
	rf_emi_tx_continue_update_data(rf_mode,power,rf_chn,2);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode ))
	{
		rf_continue_mode_run();
	}
	rf_emi_stop();
}

/**
 * @brief		This function serves to EMI_CON_TX0f
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr     - power level of RF.
 * @param[in]   rf_chn  - channel of RF.
 * @return 		none
 */
void emi_con_tx0f(rf_mode_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	rf_power_level_e power = rf_power_Level_list[pwr];
	rf_emi_tx_continue_update_data(rf_mode,power,rf_chn,1);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode ))
	{
		rf_continue_mode_run();
	}
	rf_emi_stop();
}

/**
 * @brief	  This function serves to EMI ServiceLoop
 * @return 	  none
 */
void emi_serviceloop(void)
{
   unsigned char i = 0;

   while(1)
   {
	   g_run = read_sram8(RUN_STATUE_ADDR);  // get the run state!
	   if(g_run != 0)
	   {
		   g_power_level = read_sram8(POWER_ADDR);
		   g_chn = read_sram8(CHANNEL_ADDR);
		   g_mode = read_sram8(RF_MODE_ADDR);
		   g_cmd_now = read_sram8(TEST_COMMAND_ADDR);  // get the command!
		   g_tx_cnt = read_sram8(TX_PACKET_MODE_ADDR);
		   g_hop = read_sram8(CD_MODE_HOPPING_CHN);

		   for (i = 0;i < sizeof(ate_list)/sizeof(test_list_t);i++)
		   {
				if(g_cmd_now == ate_list[i].cmd_id)
				{
					switch(g_mode)
					{
					  case ble2m:
						  ate_list[i].func(RF_MODE_BLE_2M,g_power_level,g_chn);
						  break;

					  case ble1m:
						  ate_list[i].func(RF_MODE_BLE_1M_NO_PN,g_power_level,g_chn);
						  break;

					  case zigbee250k:
						  ate_list[i].func(RF_MODE_ZIGBEE_250K,g_power_level,g_chn);
						  break;

					  case ble125K:
						  ate_list[i].func(RF_MODE_LR_S8_125K,g_power_level,g_chn);
						  break;

					  case ble500K:
						  ate_list[i].func(RF_MODE_LR_S2_500K,g_power_level,g_chn);
						  break;

					  default:break;
					}
				}
		   }
		   g_run = 0;
		   write_sram8(RUN_STATUE_ADDR,g_run);
	   }
   }
}


/**
 * @brief		user initialization when MCU wake_up from deepSleep_retention mode
 * @param[in]	none
 * @return      none
 */
_attribute_ram_code_ void user_init_deepRetn(void)
{
}

/**
 * @brief		This function serves to User Init
 * @return 		none
 */

_attribute_no_inline_ void user_init_normal(void)
{
	emi_init();
}

/**
 * @brief		This function serves to main
 * @return 		none
 */
_attribute_no_inline_ void main_loop(void)
{
	emi_serviceloop();
}





#endif  //end of (FEATURE_TEST_MODE == ...)
