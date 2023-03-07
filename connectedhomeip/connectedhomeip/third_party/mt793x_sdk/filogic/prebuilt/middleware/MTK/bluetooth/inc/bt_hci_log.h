/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */
/* MediaTek restricted information */

#ifndef __BT_DRIVER_HCI_LOG_H__
#define __BT_DRIVER_HCI_LOG_H__

//#include "hci_log.h"
#include <stdint.h>
#define BT_HCI_LOG_CMD      0x01  /**< HCI log type is command. */
#define BT_HCI_LOG_EVENT    0x02  /**< HCI log type is event. */
#define BT_HCI_LOG_ACL_IN   0x04  /**< HCI log type is acl data from remote device. */
#define BT_HCI_LOG_ACL_OUT  0x08  /**< HCI log type is acl data sent to remote device. */
#define BT_HCI_LOG_SCO_IN   0x10  /**< HCI log type is sco data from remote device.. */
#define BT_HCI_LOG_SCO_OUT  0x20  /**< HCI log type is sco data sent to remote device. */
#define BT_HCI_LOG_ISO_IN   0x40  /**< HCI log type is iso data sent from remote device. */
#define BT_HCI_LOG_ISO_OUT  0x80  /**< HCI log type is iso data sent to remote device. */
#define BT_HCI_LOG_MSG      0xFE  /**< HCI log type is message. */

/**
 * @brief Log HCI packets to UART for debug.
 * @param[in] in       is packet type. 0 means out hci packet. 1 means in hci packet. 2 means message.
 * @param[in] data     is the pointer to H4 HCI packet.
 * @param[in] data_len is the length of H4 HCI packet.
 */
void bt_hci_log(const uint8_t in, const void *data, const uint32_t data_len);

#endif // __BT_DRIVER_HCI_LOG_H__

