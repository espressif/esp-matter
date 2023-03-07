/********************************************************************************************************
 * @file	ota_stack.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	06,2020
 *
 * @par		Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd.
 *			All rights reserved.
 *
 *          The information contained herein is confidential property of Telink
 *          Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *          of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *          Co., Ltd. and the licensee or the terms described here-in. This heading
 *          MUST NOT be removed from this file.
 *
 *          Licensee shall not delete, modify or alter (or permit any third party to delete, modify, or
 *          alter) any information contained herein in whole or in part except as expressly authorized
 *          by Telink semiconductor (shanghai) Co., Ltd. Otherwise, licensee shall be solely responsible
 *          for any claim to the extent arising out of or relating to such deletion(s), modification(s)
 *          or alteration(s).
 *
 *          Licensees are granted free, non-transferable use of the information in this
 *          file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
#ifndef STACK_BLE_SERVICE_OTA_STACK_H_
#define STACK_BLE_SERVICE_OTA_STACK_H_

#include "stack/ble/ble_common.h"


#ifndef 		DBG_OTA_FLOW
#define			DBG_OTA_FLOW										0
#endif

#ifndef 		OTA_FIRMWARE_RESUME_ENABLE
#define			OTA_FIRMWARE_RESUME_ENABLE							0
#endif


#ifndef 		BLE_OTA_FW_CHECK_EN
#define 		BLE_OTA_FW_CHECK_EN									1
#endif


#define 		FW_MIN_SIZE											0x04000	 //16K

#if (MCU_CORE_TYPE == MCU_CORE_9518)
#define 		FW_MAX_SIZE											0x3F000	 //256K(0x40000) - 4K = 252K
#endif

#define 		OTA_FLOW_VERSION									BIT(0)
#define 		OTA_FLOW_START										BIT(1)
#define 		OTA_FLOW_DATA_COME									BIT(2)
#define 		OTA_FLOW_VALID_DATA									BIT(3)
#define 		OTA_FLOW_GET_SIZE									BIT(5)
#define 		OTA_FLOW_END										BIT(6)



#define 		OTA_STEP_IDLE										0
#define 		OTA_STEP_START										BIT(0)
#define 		OTA_STEP_DATA										BIT(1)
#define 		OTA_STEP_FEEDBACK									BIT(2)  //feedback OTA result to peer device
#define 		OTA_STEP_FINISH										BIT(3)



#define 		DATA_PENDING_VERSION_RSP							1
#define 		DATA_PENDING_OTA_RESULT								2
#define 		DATA_PENDING_TERMINATE_CMD							3


/**
 *  @brief
 */
typedef struct {
	u16  	ota_cmd;
	u8		data[1];
} ota_cmd_t;





typedef struct{
	u8	ota_step;
	u8	otaResult;
	u8	version_accept;
	u8	resume_mode;   //1: resume_mode enable; 0: resume_mode disable

	u8	ota_busy;
	u8 	fw_check_en;
	u8  fw_check_match;
	u8	flow_mask;

	u8	pdu_len; 	    //OTA valid data length
	u8	last_pdu_crc_offset;
	u8	last_actual_pdu_len;
	u8	last_valid_pdu_len;  //maximum value 240

	u8	data_pending_type;  //mark, and also data length
	u8	otaInit;
	u8	newFwArea_clear;
	u8	u8_rsvd;

	u16 local_version_num;  //default value:0; use use API to set version
	u16 ota_attHandle;
	u16 ota_connHandle;
	u16 last_adr_index;

	u32 fw_crc_default;
	u32 fw_crc_init;
	u32 fw_crc_addr;

	u32 firmware_size;
	int flash_addr_mark;  //must be "s32", have special useage with "< 0"
	int cur_adr_index; //must be "s32"

	u32 feedback_begin_tick;  //add a OTA feedback timeout control, prevent some extreme case which lead to OTA flow blocked
	u32 ota_start_tick;
	u32 data_packet_tick;
	u32 process_timeout_us;
	u32 packet_timeout_us;

}ota_server_t;

extern ota_server_t blotaSvr;

#if (MCU_CORE_TYPE == MCU_CORE_825x || MCU_CORE_TYPE == MCU_CORE_827x)
extern int 	ota_firmware_size_k;
#endif
extern int	ota_program_bootAddr;
extern u32	ota_program_offset;


#ifdef __cplusplus
extern "C" {
#endif

/* hidden API, if user do not use firmware check, call  this API to disable it. */
void 		blc_ota_setOtaFirmwareCheckEnable(int en);

/* hidden API, if user do not want share same firmware check algorithm with all other customers,
 * he can call this API change crc32 init_value, pay attention that TestBench need also corresponding process. */
void 		blc_ota_setFirmwareCheckCrcInitValue(u32 crc_init_value);




int 		blt_ota_server_main_loop(void);
int 		blt_ota_server_terminate(u16 connHandle);

void 		blt_ota_setResult(int next_step, int result);
void 		blt_ota_reset(void);

ble_sts_t 	blt_ota_pushVersionRsp(void);

unsigned short crc16 (unsigned char *pD, int len);

#ifdef __cplusplus
}
#endif

#endif /* STACK_BLE_SERVICE_OTA_STACK_H_ */
