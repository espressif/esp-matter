/* Copyright Statement:
 *
 * (C) 2005-2017 MediaTek Inc. All rights reserved.
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
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
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

#ifndef __BT_AIRUPDATE_H__
#define __BT_AIRUPDATE_H__

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothAIRUPDATE AIRUPDATE
 * @{
 * The Airupdate Profile can be used to generate a fixed L2CAP channel to transfer special data  between two peer devices.
 * It can only wait  for another device to take initiative to connect.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b L2CAP                     | Logical Link Control and Adaptation Protocol. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#Logical_link_control_and_adaptation_protocol_(L2CAP)">Wikipedia</a>. |
 *
 * @section bt_airupdate_api_usage How to use this module
 * This section presents the Airupdadte connection handle and a method to inframtion packet.
 *  - 1. Mandatory, implement #bt_app_event_callback() to handle the Airupdate events, such as connect, RX indication and more.
 *   - Sample code:
 *    @code
 *       bt_airupdate_data_received_ind_t saved_data = {0};
 *       void bt_app_event_callback(bt_msg_type_t msg_type, bt_status_t status, void *data)
 *       {
 *          switch (msg_type)
 *          {
 *              case BT_AIRUPDATE_CONNECTED_IND:
 *              {
 *                  bt_airupdate_connected_ind_t *connected = (bt_airupdate_connected_ind_t *)data;
 *                  // Airupdate link connected.
 *                  // Records the connected information(handle, remote address).
 *                  break;
 *              }
 *              case BT_AIRUPDATE_DISCONNECTED_IND:
 *              {
 *                  bt_airupdate_disconnected_ind_t *connected = (bt_airupdate_disconnected_ind_t *)data;
 *                  // Airupdate link disconnected.
 *                  // Records the disconnected information(handle, reason).
 *                  break;
 *              }
 *              case BT_AIRUPDATE_DATA_RECIEVED_IND:
 *              {
 *                   bt_airupdate_data_received_ind_t* data_ind_p = (bt_airupdate_data_received_ind_t*) buff;
 *                   bt_airupdate_hold_data(data_ind_p->packet);
  *                  saved_data.packet = data_ind_p->packet;
 *                   saved_data.packet_length = data_ind_p->packet_length;
 *              }
 *              default:
 *              {
 *                  break;
 *              }
 *           }
 *       }
 *    @endcode
 */


#include "bt_type.h"

/**
* @defgroup Bluetoothbt_airupdate_define Define
* @{
*/

/**
 * @brief An invalid value for the AIRUPDATE handle.
 */
#define BT_AIRUPDATE_INVALID_HANDLE               0x00000000

/**
 * @brief The default MTU value for the AIRUPDATE profile.
 */
#define BT_AIRUDPATE_DEFAULT_MTU                  (0x02A0)

/**
 * @brief The event reported to the user.
 */
#define BT_AIRUPDATE_CONNECTED_IND                 (BT_MODULE_AIRUPDATE | 0x0001)    /**< An attempt to connect from a remote device with the #bt_airupdate_connected_ind_t payload. */
#define BT_AIRUPDATE_DISCONNECTED_IND              (BT_MODULE_AIRUPDATE | 0x0002)    /**< A disconnect attempt is initiated by a local or remote device or a link loss occurred with the #bt_airupdate_disconnected_ind_t payload. */
#define BT_AIRUPDATE_DATA_RECIEVED_IND             (BT_MODULE_AIRUPDATE | 0x0003)    /**< Data was received from the remote device. The "packet" and "pakcet_length" fields describe the received data. */
#if 0
#define BT_AIRUPDATE_READY_TO_SEND_IND             (BT_MODULE_AIRUPDATE | 0x0004)    /**< The Bluetooth TX buffer has free space for sending data. Call the function #bt_airupdate_send_data() to send the application data. */
#endif
/**
 * @}
 */

/**
* @defgroup Bluetoothbt_airupdate_struct Struct
* @{
*/

/**
 *  @brief This structure defines the result of the #BT_AIRUPDATE_CONNECTED_IND event.
 */
typedef struct {
    uint32_t handle;            /**< AIRUPDATE handle. */
    uint16_t max_packet_length; /**< The maximum length of a TX/RX packet after an Airupdate connection is established. */
    bt_bd_addr_t *address;      /**< Bluetooth address of a remote device. */
} bt_airupdate_connected_ind_t;


/**
 *  @brief This structure defines the #BT_AIRUPDATE_DISCONNECTED_IND result.
 */
typedef struct {
    uint32_t handle;            /**< AIRUPDATE handle. */
    uint32_t reason;            /**< The disconnection reason. */
} bt_airupdate_disconnected_ind_t;


/**
 *  @brief This structure defines the #BT_AIRUPDATE_DATA_RECIEVED_IND result.
 */
typedef struct {
    uint32_t handle;            /**< The Airupdate handle of the current connection.*/
    uint16_t packet_length;     /**< The length of the received packet.*/
    uint8_t *packet;            /**< The packet is received from a remote device.*/
} bt_airupdate_data_recieved_ind_t;

#if 0
/** @brief This structure defines #BT_AIRUPDATE_READY_TO_SEND_IND. */
typedef struct {
    uint32_t handle;            /**< The Airupdate handle of the current connection.*/
} bt_airupdate_ready_to_send_ind_t;
#endif

/**
 * @}
 */

BT_EXTERN_C_BEGIN


/**
 * @brief               This function send data to the remote device.
 * @param[in] handle    is the connection handle for the specified link.
 * @param[in] data      defines the send information from upper layer.
 * @param[in] length    defines the length of the information packet should be sent out instantly.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the operation has failed.
 *                      #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_airupdate_send_data(uint32_t handle, uint8_t *data, uint16_t length);

/**
 * @brief           This function holds the Airupdate packet until it is fully consumed.
 * @param[in] data  is the initialization parameter. The value is provided once it is called by the Airupdate.
 * @return          None.
 */
void bt_airupdate_hold_data(uint8_t *data);

/**
* @brief           This function releases the Airupdate packet after it is consumed.
* @param[in] data  is the initialization parameter. The value is provided once it is called by the Airupdate.
* @return          None.
*/
void bt_airupdate_release_data(uint8_t *data);

BT_EXTERN_C_END
/**
 * @}
 * @}
 */
#endif /*__BT_AIRUPDATE_H__*/

