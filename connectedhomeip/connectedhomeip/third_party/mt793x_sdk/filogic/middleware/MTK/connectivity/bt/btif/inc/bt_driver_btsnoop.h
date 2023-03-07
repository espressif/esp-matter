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

#ifndef __BT_DRIVER_BTSNOOP_H_
#define __BT_DRIVER_BTSNOOP_H_

#ifdef __cplusplus
extern "C"
{
#endif /* #ifdef __cplusplus */

// HCI log type
#define BT_HCI_LOG_CMD     0x01    // command
#define BT_HCI_LOG_EVENT   0x02    // event
#define BT_HCI_LOG_ACL_IN  0x04    // acl received
#define BT_HCI_LOG_ACL_OUT 0x08    // acl sent
#define BT_HCI_LOG_SCO_IN  0x10    // sco received
#define BT_HCI_LOG_SCO_OUT 0x20    // sco sent
#define BT_HCI_LOG_ISO_IN  0x40    // iso received
#define BT_HCI_LOG_ISO_OUT 0x80    // iso sent
#define BT_HCI_LOG_MSG     0xFE    // message

struct btsnoop_packet_t {
	unsigned char b_valid;
	unsigned char type;
	unsigned int len; // length of packet

	// header + hci packet, dynamically alloacted and freed
	unsigned char *packet;
};

int bt_driver_btsnoop_push_packet(unsigned char type, unsigned char *buffer, unsigned int length);
int bt_driver_btsnoop_init(void);
void bt_driver_btsnoop_ctrl(unsigned char enable);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /*__BT_DRIVER_BTSNOOP_H_*/
