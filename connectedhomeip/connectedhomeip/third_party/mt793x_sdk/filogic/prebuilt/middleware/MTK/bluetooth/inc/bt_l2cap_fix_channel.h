/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */
/* Airoha restricted information */
#ifndef _BT_L2CAP_FIX_CHANNEL_H_
#define _BT_L2CAP_FIX_CHANNEL_H_
BT_EXTERN_C_BEGIN
#include "bt_gap.h"
/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothL2CAP L2CAP
 * @{
 * @addtogroup BluetoothL2CAP_FIX_CHANNEL L2CAP_FIX_CHANNEL
 * @{
 * This section describes l2cap fix channel usage and API prototypes
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b CID                        |Channel Identifier.                                                      |
 *
 * @section bt_l2cap_fix_channel_api_usage How to use this module
 * - Step 1: (Mandatory) Each L2CAP fix channel use must register a fix channel id and callback.
 *  - Sample code:
 *     @code
 *               bt_l2cap_fix_cid_t cid = 0x001F;
 *               static bt_status_t bt_app_l2cap_callback(bt_l2cap_fix_channel_event_t event_id, bt_status_t status, void *buffer)
 *               {
 *                    switch(event_id) {
 *                       case BT_L2CAP_FIX_CHANNEL_CONNECT_IND:
 *                       {
 *                          bt_l2cap_fix_channel_connect_ind_t *ind = (bt_l2cap_fix_channel_connect_ind_t *)buffer;
 *                          break;
 *                       }
 *                       case BT_L2CAP_FIX_CHANNEL_DISCONNECT_IND:
 *                       {
 *                          bt_l2cap_fix_channel_disconnect_ind_t *ind = (bt_l2cap_fix_channel_disconnect_ind_t *)buffer;
 *                          break;
 *                       }
 *                       case BT_L2CAP_FIX_CHANNEL_DATA_IND:
 *                       {
 *                          bt_l2cap_fix_channel_packet_ind_t *ind = (bt_l2cap_fix_channel_packet_ind_t *)buffer;
 *                          break;
 *                       }
 *                       default:
 *                          break;
 *                  }
 *                  return BT_STATUS_SUCCESS;
 *              }
 *             bt_l2cap_fix_channel_register(BT_L2CAP_FIX_CHANNEL_BLE, cid, bt_app_l2cap_callback);
 *     @endcode
 *- Step 2: To establish BR/EDR fix channel ACL connection(only BR/EDR connect).
 *  - Sample code:
 *     @code
 *               uint8_t addr[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
 *               bt_l2cap_fix_channel_conenct(addr, BT_GAP_SECURITY_LEVEL_1);
 *     @endcode
 *- Step 3: L2CAP fix channel send data.
 *  - Sample code:
 *     @code:
 *               uint8_t buf[3] = {0x00, 0x01, 0x02};
 *               bt_l2cap_fix_channel_tx_data_t data;
 *               data.type = BT_L2CAP_FIX_CHANNEL_BLE;
 *               data.connection_handle = handle;
 *               data.cid = 0x001F;
 *               memcpy(data.data, buf, 3);
 *               data.length = 3;
 *               bt_l2cap_fix_channel_send(&data);
 *     @endcode
 */

/**
 * @addtogroup BluetoothBLE_L2CAP_FIX_CHANNEL_define Define
 * @{
 * This section defines macros for the L2CAP FIX CHANNEL.
 */

/**
*  @brief This bt_l2cap_fix_channel_event_t defines the L2CAP fix channel event.
*/
#define BT_L2CAP_FIX_CHANNEL_CONNECT_IND                   (0x0001)            /**< This is the connection complete, with #bt_l2cap_fix_channel_connect_ind_t as the payload. */
#define BT_L2CAP_FIX_CHANNEL_DISCONNECT_IND                (0x0002)            /**< This is the disconnection complete, with #bt_l2cap_fix_channel_disconnect_ind_t as the payload. */
#define BT_L2CAP_FIX_CHANNEL_DATA_IND                      (0x0003)            /**< This is the data indication, with #bt_l2cap_fix_channel_data_ind_t as the payload. */
typedef uint8_t bt_l2cap_fix_channel_event_t;

/**
*  @brief This bt_l2cap_fix_cid_t defines the L2CAP le and br/edr fix channel id.
*/
#define BT_L2CAP_FIX_CID_LE_INTERNAL_BEGIN           (0x0100)         /**< The le fix id scope that the internal can use begins to be defined, the customer cannot use cid for this range. */
#define BT_L2CAP_FIX_CID_LE_INTERNAL_END             (0x0131)         /**< The le fix id scope that the internal can use ends to be defined, the customer cannot use cid for this range. */
#define BT_L2CAP_FIX_CID_LE_CUSTOMER_BEGIN           (0x0132)         /**< The le fix id scope that the customer can use begins to be defined. */
#define BT_L2CAP_FIX_CID_LE_CUSTOMER_END             (0x0164)         /**< The le fix id scope that the customer can use ends to be defined. */
#define BT_L2CAP_FIX_CID_BREDR_INTERNAL_BEGIN        (0x0010)         /**< The BR/EDR fix id scope that the internal can use begins to be defined, the customer cannot use cid for this range. */
#define BT_L2CAP_FIX_CID_BREDR_INTERNAL_END          (0x0024)         /**< The BR/EDR fix id scope that the internal can use begins to be defined, the customer cannot use cid for this range. */
#define BT_L2CAP_FIX_CID_BREDR_CUSTOMER_BEGIN        (0x0025)         /**< The BR/EDR fix id scope that the customer can use begins to be defined. */
#define BT_L2CAP_FIX_CID_BREDR_CUSTOMER_END          (0x003E)         /**< The BR/EDR fix id scope that the customer can use ends to be defined. */
typedef uint16_t bt_l2cap_fix_cid_t;                                  /**< The type of fix channel id. */


/**
*  @brief This bt_l2cap_fix_channel_send function send maximum data length.
*/
#define BT_L2CAP_FIX_CHANNEL_BREDR_SEND_MAX_LENGTH           (1017)          /**< This macro definition is the maximum data length that BR/EDR can send. */
#define BT_L2CAP_FIX_CHANNEL_LE_SEND_MAX_LENGTH              (512)           /**< This macro definition is the maximum data length that BLE can send. */

/**
 * @}
 */

/**
 * @defgroup BT_L2CAP_FIX_CHANNEL struct Struct
 * @{
 * This section defines the structures for the connect/disconnect/data.
 */

/** @brief This structure define for fix channel id type. */
typedef enum {
    BT_L2CAP_FIX_CHANNEL_BLE = 0,                  /**< The LE fix channel type. */
    BT_L2CAP_FIX_CHANNEL_BREDR                     /**< The BR/EDR fix channel type. */
} bt_l2cap_fix_channel_t;

/** @brief This structure define for #BT_L2CAP_FIX_CHANNEL_CONNECT_IND, connect complete. */
typedef struct {
    bt_handle_t connection_handle;                /**< The connection handle of the ACL connect. */
    bt_bd_addr_ptr_t addr;                        /**< Remote device address. */
} bt_l2cap_fix_channel_connect_ind_t;

/** @brief This structure define for #BT_L2CAP_FIX_CHANNEL_DISCONNECT_IND, disconnect complete. */
typedef struct {
    bt_handle_t connection_handle;               /**< The connection handle of the ACL connect. */
    bt_bd_addr_ptr_t addr;                       /**< Remote device address. */
} bt_l2cap_fix_channel_disconnect_ind_t;

/** @brief This structure define for #BT_L2CAP_FIX_CHANNEL_DATA_IND. */
typedef struct {
    bt_handle_t connection_handle;            /**< The connection handle of the ACL connect. */
    bt_l2cap_fix_cid_t cid;                   /**< The registered fix channel id. */
    uint8_t *packet;                          /**< Data packet. */
    uint16_t packet_length;                   /**< Data packet length. */
} bt_l2cap_fix_channel_data_ind_t;

/** @brief This structure define for send data to use the fix channel. */
typedef struct {
    bt_l2cap_fix_channel_t type;              /**< Define in the #bt_l2cap_fix_channel_event_t. */
    bt_handle_t connection_handle;            /**< The connection handle of the ACL connect. */
    bt_l2cap_fix_cid_t cid;                   /**< The registered fix channel id.*/
    uint8_t *data;                            /**< Send data.*/
    uint16_t length;                          /**< Send data length, LE send max length is 512, BR/EDR send max length is 1017. */
} bt_l2cap_fix_channel_tx_data_t;

/**
 * @}
 */

/**
 * @brief The function is L2CAP fix channel callback, which receives the data and connect/disconnect the callback to the user based on the fix channel id.
 * @param[in] event_id              is the event type, define in the #bt_l2cap_fix_channel_event_t.
 * @param[in] status                is the event status #BT_STATUS_SUCCESS is successfully, otherwise failed.
 * @param[in] buffer                is the event structure.
 * @return    For #BT_STATUS_SUCCESS, the callback function was called successful, otherwise failed.
 */
typedef bt_status_t(* bt_l2cap_fix_channel_callback_t)(bt_l2cap_fix_channel_event_t event_id, bt_status_t status, void *buffer);

/**
 * @brief   Function for register fix channel id.
 * @param[in] type        define in the #bt_l2cap_fix_channel_event_t.
 * @param[in] cid         channel id, 0x0000 is invalid, le:the scope of 0x0100-0x0164,br:the scope of 0x0008-0x003e.
 * @param[in] callback    callback, is the fix channel event callback, defined by #bt_l2cap_fix_channel_callback_t.
 * @return                #BT_STATUS_SUCCESS, the register fix channel successfully.
 *                        #BT_STATUS_OUT_OF_MEMORY, not enough memory allocated for the register.
 *                        #BT_STATUS_FAIL, the operation has failed.
 *                       #BT_STATUS_L2CAP_FIX_CID_IN_USE, the fix channel id has been used.
 *                        #BT_STATUS_L2CAP_FIX_CID_INVAILD, the fix channel id is invalid.
 */

bt_status_t bt_l2cap_fix_channel_register(bt_l2cap_fix_channel_t type, bt_l2cap_fix_cid_t cid, bt_l2cap_fix_channel_callback_t callback);
/**
 * @brief   Function for unregister fix channel id.
 * @param[in] type        define in the #bt_l2cap_fix_channel_event_t.
 * @param[in] cid         channel id, 0x0000 is invalid, le:the scope of 0x0100-0x0164,br:the scope of 0x0008-0x003e.
 * @return                #BT_STATUS_SUCCESS, the unregister fix channel successfully.
 *                        #BT_STATUS_FAIL, the operation has failed.
 */

bt_status_t bt_l2cap_fix_channel_unregister(bt_l2cap_fix_channel_t type, bt_l2cap_fix_cid_t cid);
/**
 * @brief   This function connects to the specified remote device. Please note that, this function is only used to connect BR/EDR
 *                The event #BT_L2CAP_FIX_CHANNEL_CONNECT_IND is sent to the upper layer with the connection request result.
 * @param[in] addr                  is a pointer to the remote device's address.
 * @param[in] security_level        the link security level types.
 * @return                          #BT_STATUS_SUCCESS, the connection request started successfully.
 *                                  #BT_STATUS_FAIL, the operation has failed.
 */

bt_status_t bt_l2cap_fix_channel_bredr_connect(const bt_bd_addr_t *addr, bt_gap_security_level_t security_level);
/**
 * @brief   This function disconnects to the specified remote device. The event #BT_L2CAP_FIX_CHANNEL_DISCONNECT_IND is sent to the upper layer with the disconnection request reslult.
 * @param[in] connection_handle       The disconnection handle of the acl connect.
 * @return                            #BT_STATUS_SUCCESS, the disconnection request started successfully.
 *                                    #BT_STATUS_FAIL, the operation has failed.
 */

bt_status_t bt_l2cap_fix_channel_bredr_disconnect(bt_handle_t connection_handle);
/**
 * @brief   This function sends data to a remote device.
 * @param[in] data           define in the #bt_l2cap_fix_channel_data_t.
 * @return                   #BT_STATUS_SUCCESS, the operation completed successfully.
 *                           #BT_STATUS_OUT_OF_MEMORY, not enough memory allocated for send data.
 *                           #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_l2cap_fix_channel_send(bt_l2cap_fix_channel_tx_data_t *data);

/**
 * @}
 * @}
 * @}
 */
BT_EXTERN_C_END
#endif