/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef __BT_DRIVER_H_
#define __BT_DRIVER_H_

#include "FreeRTOS.h"
#include "semphr.h"
#include <stdio.h>
#include "syslog.h"
#include "btif_mt7933.h"
#include "btif_util.h"

#define HCI_PKT_CMD 0x01
#define HCI_PKT_ACL 0x02
#define HCI_PKT_SCO 0x03
#define HCI_PKT_EVT 0x04
#define HCI_PKT_ISO 0x05


#define RX_PACKET_BUF_LEN (1024 * 3)

#define BT_DRV_LOGV BTIF_LOG_V
#define BT_DRV_LOGD BTIF_LOG_D
#define BT_DRV_LOGI BTIF_LOG_I
#define BT_DRV_LOGW BTIF_LOG_W
#define BT_DRV_LOGE BTIF_LOG_E

enum bt_driver_state_t {
	BT_DRIVER_STATE_UNKNOWN,
	BT_DRIVER_STATE_INIT,
	BT_DRIVER_STATE_SUSPENDING,
	BT_DRIVER_STATE_SUSPENDED,
	BT_DRIVER_STATE_RESUMING,
	BT_DRIVER_STATE_RESUMED,
};

#define BT_OFF 0
#define BT_ON  1

typedef void (*bt_rx_data_ready_cb)(void);
typedef void (*bt_state_change_cb)(int state);

struct bt_rx_packet_t {
	unsigned char buf[RX_PACKET_BUF_LEN];
	unsigned int valid_len;
	unsigned int exp_len;
	xSemaphoreHandle mtx;
};

int bt_driver_init(void);
void bt_driver_register_state_change_cb(bt_state_change_cb cb, int *curr_state);
int bt_driver_trigger_controller_codedump(void);
int bt_driver_func_on(void);
int bt_driver_func_off(void);
int bt_driver_tx_debug(unsigned char *packet, unsigned int length);
int bt_driver_rx_timeout(unsigned char *buf, unsigned int len);
void bt_driver_bgfsys_on(void);
int bt_driver_dlfw(void);
void bt_driver_print_version_info(void);
void bt_driver_set_own_ctrl(unsigned char ctrl);
uint8_t bt_driver_get_own_ctrl(void);
int bt_driver_set_own_type(unsigned char own_type);
void bt_driver_suspend(void *p_data);
void bt_driver_resume(void *p_data);
int bt_driver_do_resume(void);
uint8_t bt_driver_get_own_type(void);


/*****************************************************************************
 *  Driver Exported API Declarations for Upper Layer
 ****************************************************************************/
/**
 * @brief     This function is called to download firmware and power on BT radio.
 * @return    0, the operation completed successfully, otherwise it failed.
 */
#ifndef MTK_DRV_VND_LAYER
int bt_driver_power_on(void);
#else
int BTIF_driver_power_on(void);
#endif

/**
 * @brief     This function is called to power off BT radio.
 * @return    0, the operation completed successfully, otherwise it failed.
 */
#ifndef MTK_DRV_VND_LAYER
int bt_driver_power_off(void);
#else
int BTIF_driver_power_off(void);
#endif

/**
 * @brief     This function is called to send data to bluetooth chip.
 * @param[in] packet                     data to be sent
 * @param[in] length                     data length
 * @return    0, the operation completed successfully, otherwise it failed.
 */
#ifndef MTK_DRV_VND_LAYER
int bt_driver_tx(unsigned char *packet, unsigned int length);
#else
int BTIF_driver_tx(unsigned char *packet, unsigned int length);
#endif

/**
 * @brief     This function is called to do rx loop and send to HB
 * @return    N/A
 */
#ifndef MTK_DRV_VND_LAYER
void bt_driver_recv(void);
int bt_driver_rx(unsigned char *buf, unsigned int buf_len);
#else
void BTIF_driver_recv(void);
#endif

/**
 * @brief     This function is called to register callback so that upper layer
 *            can receive data from driver.
 * @param[in] cb                     callback function
 * @param[in] restore_cb             overwrite callback or not
 * @return    0, the operation completed successfully, otherwise it failed.
 */
int bt_driver_register_event_cb(bt_rx_data_ready_cb cb, int restore_cb);
#endif /*__BT_DRIVER_H_*/
