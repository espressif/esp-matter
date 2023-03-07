/*
 *  Copyright (c) 2016,2017 MediaTek Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */

#ifndef __BTMTK_BUFFER_MODE_H__
#define __BTMTK_BUFFER_MODE_H__

#include "bt_driver.h"

#define BUFFER_MODE_SWITCH_FILE "wifi"
#define BUFFER_MODE_SWITCH_FIELD "EfuseBufferModeCal"
#define EFUSE_MODE    0
#define BIN_FILE_MODE 1
#define AUTO_MODE     2

#define EFUSE_CHIP_ID_ADDR 0x0
#define EFUSE_VERSION      0x2

#define READ_EFUSE_CMD_LEN          18
#define READ_EFUSE_EVT_HDR_LEN      9
#define READ_EFUSE_EVT_COMPLETE_LEN 33
#define READ_EFUSE_CMD_BLOCK_OFFSET 10

#define EEPROM_CHIP_ID_OFFSET 0
#define EEPROM_CHIP_ID_LEN    2

#define SET_ADDRESS_CMD_LEN 10
#define SET_ADDRESS_EVT_LEN 7
#define SET_ADDRESS_CMD_PAYLOAD_OFFSET 4

#define SET_RADIO_CMD_LEN             12
#define SET_RADIO_EVT_LEN             7
#define SET_RADIO_CMD_EDR_DEF_OFFSET  4
#define SET_RADIO_CMD_BLE_OFFSET      8
#define SET_RADIO_CMD_EDR_MAX_OFFSET  9
#define SET_RADIO_CMD_EDR_MODE_OFFSET 11

#define SET_GRP_CMD_LEN 13
#define SET_GRP_EVT_LEN 7
#define SET_GRP_CMD_PAYLOAD_OFFSET 8

#define SET_PWR_OFFSET_CMD_LEN 14
#define SET_PWR_OFFSET_EVT_LEN 7
#define SET_PWR_OFFSET_CMD_PAYLOAD_OFFSET 8

#define SET_ANT_TO_PIN_LOSS_CMD_LEN 8
#define SET_ANT_TO_PIN_LOSS_EVT_LEN 7
#define SET_ANT_TO_PIN_LOSS_CMD_PAYLOAD_OFFSET 7

#define SET_TX_POWER_CAL_CMD_LEN 20
#define SET_TX_POWER_CAL_EVT_LEN 7
#define SET_TX_POWER_CAL_CMD_PAYLOAD_OFFSET 4

#define BUFFER_LEN_ADDR_RADIO 10

#define BUFFER_MODE_MAC_LENGTH 6
#define BT_MAC_OFFSET          0x203

#define BUFFER_MODE_RADIO_LENGTH 4
#define BT_RADIO_OFFSET          0x209

#define BUFFER_MODE_POWER_CAL_LENGTH 16
#define BT_POWER_CAL_OFFSET          0x26B

#define BUFFER_LEN_OFFSET_CAL 65

#define BUFFER_MODE_GROUP_LENGTH 5
#define BT_GROUP_ANT0_OFFSET     0x4F6
#define BT_GROUP_ANT1_OFFSET     0x513

#define BUFFER_MODE_CAL_LENGTH 6
#define BT_CAL_ANT0_OFFSET     0x4DE
#define BT_CAL_ANT1_OFFSET     0x4FB

#define BUFFER_MODE_LOSS_LENGHT 1
#define BT_LOSS_ANT0_OFFSET     0x4D8
#define BT_LOSS_ANT1_OFFSET     0x4D9

typedef uint8_t bt_drv_addr_t[BUFFER_MODE_MAC_LENGTH];

struct btmtk_buf_mode_radio_t {
	uint8_t radio_0; /* bit 0-5:edr_init_pwr, 6-7:edr_pwr_mode */
	uint8_t radio_1; /* bit 0-5:edr_max_pwr, 6-7:reserved */
	uint8_t radio_2; /* bit 0-5:ble_default_pwr, 6-7:reserved */
	uint8_t radio_3; /* reserved */
};

struct btmtk_buf_mode_t {
	uint8_t efuse_mode;
	uint8_t ready; /* Flag for mark that eeprom content has checked */
	uint8_t bt_mac[BUFFER_MODE_MAC_LENGTH];
	struct btmtk_buf_mode_radio_t bt_radio;
	uint8_t bt_power_cal[BUFFER_MODE_POWER_CAL_LENGTH];
	uint8_t bt_ant0_grp_boundary[BUFFER_MODE_GROUP_LENGTH];
	uint8_t bt_ant1_grp_boundary[BUFFER_MODE_GROUP_LENGTH];
	uint8_t bt_ant0_pwr_offset[BUFFER_MODE_CAL_LENGTH];
	uint8_t bt_ant1_pwr_offset[BUFFER_MODE_CAL_LENGTH];
	uint8_t bt_ant0_loss[BUFFER_MODE_LOSS_LENGHT];
	uint8_t bt_ant1_loss[BUFFER_MODE_LOSS_LENGHT];
};

bt_drv_addr_t *btmtk_get_eeprom_bt_addr(void);
int btmtk_buffer_mode_send(void);
int btmtk_buffer_mode_initialize(void);

#endif /* __BTMTK_BUFFER_MODE_H__ */
