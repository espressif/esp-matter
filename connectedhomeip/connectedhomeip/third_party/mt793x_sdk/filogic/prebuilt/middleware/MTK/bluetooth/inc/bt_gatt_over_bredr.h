/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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
#ifndef _BT_GATT_OVER_BREDR_CONNECT_H
#define _BT_GATT_OVER_BREDR_CONNECT_H
/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothGATT GATT
 * @{
 * @addtogroup BluetoothGATT_OVER_BR_EDR GATT over BR/EDR
 * @{
 * This section manages BT GATT over BR/EDR.
 *
 * @section Master how does GATT over BR/EDR connect work
 * - Step 1. (Mandatory) Implement the function #bt_app_event_callback() to handle the GATT connect events, such as connect, disconnect.
 *  - Sample Code:
 *     @code
 *             void bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *             {
 *                  switch (msg) {
 *                     case BT_GATT_OVER_BREDR_CONNECT_CNF:
 *                     {
 *                           bt_gatt_over_bredr_disconnect_ind_t *cnf = (bt_gatt_over_bredr_connect_cnf_t *)buff;
 *                           break;
 *                     }
 *                     case BT_GATT_OVER_BREDR_DISCONNECT_IND:
 *                     {
 *                           bt_gatt_over_bredr_disconnect_ind_t *cnf = (bt_gatt_over_bredr_disconnect_ind_t *)buff;
 *                           break;
 *                     }
 *                     default:
 *                           break;
 *                  }
 *             }
 *     @endcode
 * - Step 2. BT GATT connect(master).
 *  - Sample Code:
 *     @code
 *              bt_bd_addr_t addr = {0x00,0x01,0x02,0x03,0x04,0x05};
 *              bt_gatt_over_bredr_connect(&addr);
 *
 *     @endcode
 * - Step 3. BT GATT disconnect.
 *  - Sample Code:
 *     @code
 *              bt_gatt_over_bredr_disconnect(connection_handle);
 *
 *     @endcode
 @section Slave how does GATT over BR/EDR connect
 * - Step 1. (Mandatory) Enable GATT over BR/EDR function.
 *  - Sample Code:
 *     @code
 *             bt_gatt_over_bredr_switch(true);
 *     @endcode
 * - Step 2. (Mandatory) Implement the function #bt_app_event_callback() to handle the GATT connect events, such as connect, disconnect.
 *  - Sample Code:
 *     @code
 *             void bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *             {
 *                  switch (msg) {
 *                     case BT_GATT_OVER_BREDR_CONNECT_CNF:
 *                     {
 *                           bt_gatt_over_bredr_connect_cnf_t *cnf = (bt_gatt_over_bredr_connect_cnf_t *)buff;
 *                           break;
 *                     }
 *                     case BT_GATT_OVER_BREDR_DISCONNECT_IND:
 *                     {
 *                           bt_gatt_over_bredr_disconnect_ind_t *cnf = (bt_gatt_over_bredr_disconnect_ind_t *)buff;
 *                           break;
 *                     }
 *                     default:
 *                           break;
 *                  }
 *             }
 *     @endcode
 * - Step 3. The API bt_gatt_over_bredr_get_service_handle implemented by the upper layer,access the GAP and GATT service handle scope.
 *  - Sample Code:
 *     @code
 *              bt_status_t bt_gatt_over_bredr_get_service_handle(bt_gatt_over_bredr_service_handle_t *service_handle)
 *              {
 *                 service_handle->gap_start_handle = 0x0001;
 *                 service_handle->gap_end_handle = 0x0009;
 *                 service_handle->gatt_start_handle = 0x001;
 *                 service_handle->gatt_end_handle = 0x0011;
 *                 return BT_STATUS_SUCCESS;
 *              }
 *
 *     @endcode
 * - Step 4. BT GATT over BR/EDR disconnect.
 *  - Sample Code:
 *     @code
 *              bt_gatt_over_bredr_disconnect(connection_handle);
 *
 *     @endcode
 */
#include "bt_type.h"

/**
 * @defgroup BT_GATT_OVER_BREDR_CONNECT_define Define
 * @{
 * This section defines the macros for the #bt_gatt_over_bredr_connect().
 */

/**
 * @brief BT_GATT_OVER_BREDR_CONNECT event.
 */
#define BT_GATT_OVER_BREDR_CONNECT_CNF                          (BT_MODULE_GATT | 0x00150000)    /**< The connection confirmation event is triggered with #bt_gatt_over_bredr_connect_cnf_t as the payload. */
#define BT_GATT_OVER_BREDR_DISCONNECT_IND                       (BT_MODULE_GATT | 0x00160000)    /**< The disconnection indication event is triggered with #bt_gatt_over_bredr_disconnect_ind_t as the payload. */

/**
 * @brief  Attribute value length of the GATT service class ID list.
 */
#define BT_GATT_SDP_ATTRIBUTE_UUID_16    BT_SDP_UUID_16BIT
#define BT_GATT_SDP_ATTRIBUTE_UUID_32    BT_SDP_UUID_32BIT
#define BT_GATT_SDP_ATTRIBUTE_UUID_128   BT_SDP_UUID_128BIT

/**
 * @brief  Attribute value of the GATT service class ID list.
 */
#define BT_GATT_SDP_ATTRIBUTE_UUID_LENGTH_16    BT_SDP_ATTRIBUTE_HEADER_8BIT(3)
#define BT_GATT_SDP_ATTRIBUTE_UUID_LENGTH_32    BT_SDP_ATTRIBUTE_HEADER_8BIT(5)
#define BT_GATT_SDP_ATTRIBUTE_UUID_LENGTH_128   BT_SDP_ATTRIBUTE_HEADER_8BIT(17)

/**
 * @brief Attribute value of the GATT protocol descriptor list.
 * @param[in] start_handle  is GATT service start handle.
 * @param[in] end_handle    is GATT service end handle.
 */
#define BT_GATT_SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR(start_handle,end_handle)  \
        BT_SDP_ATTRIBUTE_HEADER_8BIT(19),                                   \
        BT_SDP_ATTRIBUTE_HEADER_8BIT(6),                                    \
        BT_SDP_UUID_16BIT(BT_SDP_PROTOCOL_L2CAP),                           \
        BT_SDP_UINT_16BIT(0x001F),                                          \
        BT_SDP_ATTRIBUTE_HEADER_8BIT(9),                                    \
        BT_SDP_UUID_16BIT(BT_SDP_PROTOCOL_ATT),                             \
        BT_SDP_UINT_16BIT(start_handle),                                    \
        BT_SDP_UINT_16BIT(end_handle),

/**
 * @brief Attribute value of the GATT Public Browse Group. Any SDP attribute structure can include this
 *        attribute to add itself to the root level of the Public Browse Group.
 */
#define BT_GATT_SDP_ATTRIBUTE_PUBLIC_BROWSE_GROUP  \
        BT_SDP_ATTRIBUTE_HEADER_8BIT(3),                                 \
        BT_SDP_UUID_16BIT(BT_SDP_SERVICE_CLASS_PUBLIC_BROWSE_GROUP),

/**
 * @brief The GATT service class ID list attribute.
 * @param[in] uuid_array  is an array to hold the #BT_GATT_SDP_ATTRIBUTE_UUID.
 */
#define BT_GATT_SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST(uuid_array)  \
        BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_SERVICE_CLASS_ID_LIST, uuid_array)

/**
 * @brief The GATT protocol descriptor list attribute.
 * @param[in] protocol_descriptor_array   is an array to hold the #BT_GATT_SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR.
 */
#define BT_GATT_SDP_ATTRIBUTE_ID_PROTOCOL_DESC_LIST(protocol_descriptor_array)  \
        BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_PROTOCOL_DESC_LIST, protocol_descriptor_array)


/**
 * @brief The GATT Public Browse Group attribute.
 * @param[in] browse_group_array  is an array to hold the #BT_GATT_SDP_ATTRIBUTE_PUBLIC_BROWSE_GROUP.
 */
#define BT_GATT_SDP_ATTRIBUTE_ID_BROWSE_GROUP_LIST(browse_group_array)  \
        BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_BROWSE_GROUP_LIST, browse_group_array)

/**
 * @}
 */


/**
 * @defgroup BT_GATT_OVER_BREDR_CONNECT_struct Struct
 * @{
 * This section defines the structures for connection and dis connection.
 */

/** @brief this structure define for #BT_GATT_OVER_BREDR_CONNECT_CNF,connect conplete. */
typedef struct {
    bt_handle_t connection_handle;                                      /**< The connection handle of the GATT over BR/EDR. */
    uint16_t remote_rx_mtu;                                             /**<rx mtu size of remote device. */
    bt_bd_addr_t *address;                                              /**< The address of a remote device to start a connection. */
} bt_gatt_over_bredr_connect_cnf_t;

/** @brief this structure define for #BT_GATT_OVER_BREDR_DISCONNECT_IND. */
typedef struct {
    bt_handle_t connection_handle;                                        /**< The connection handle of the GATT over BR/EDR. */
    bt_bd_addr_t *address;                                                /**< The address of a remote device to start a connection. */
} bt_gatt_over_bredr_disconnect_ind_t;

/** @brief this structure defined to obtain the GAP and GATT service handle  */
typedef struct {
    uint16_t gap_start_handle;                                           /**< GAP service start handle value. */
    uint16_t gap_end_handle;                                             /**< GAP service end handle value. */
    uint16_t gatt_start_handle;                                          /**< GATT service start handle value. */
    uint16_t gatt_end_handle;                                            /**< GATT service end handle value. */
} bt_gatt_over_bredr_service_handle_t;

/**
 * @}
 */


/**
 * @brief   This function connects to the specified remote device.Please note that, this function is only called on the master side
 *                The event #BT_GATT_OVER_BREDR_CONNECT_CNF is sent to the upper layer with the connection request result.
 * @param[in] addr  is a pointer to the remote device's address.
 * @return    #BT_STATUS_SUCCESS, the connection request started successfully.
 *            #BT_STATUS_OUT_OF_MEMORY, not enough memory allocated for the gatt connect.
 *            #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_gatt_over_bredr_connect(const bt_bd_addr_t *addr);

/**
 * @brief   This function disconnects to the specified remote device. The event #BT_GATT_OVER_BREDR_DISCONNECT_IND is sent to the upper layer with the connection request reslult.
 * @param[in] connection_handle  is the connection handle of the specified remote device.
 * @return    #BT_STATUS_SUCCESS, the connection request started successfully.
 *            #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_gatt_over_bredr_disconnect(bt_handle_t connection_handle);
/**
* @brief  this user defined API, invoked by the GATT SDP record process, should be implemented by the application. It is the start and end handle for access to gap and gatt services.
* @param[in] service_handle    is the GAP and GATT service handle by the application.
* @return                      if application If the user has a service database, and it is successfully filled in, the returned value is #BT_STATUS_SUCCESS; otherwise the returned value is #BT_STATUS_FAIL.
*/
bt_status_t bt_gatt_over_bredr_get_service_handle(bt_gatt_over_bredr_service_handle_t *service_handle);
/**
* @brief this API use to enable/disable GATT over BR/EDR feature.
* @param[in] flag    true:enable GATT over BR/EDR,flase:disable GATT over BR/EDR.
* @return    none
*/
void bt_gatt_over_bredr_switch(bool flag);

/**
 * @}
 * @}
 * @}
 */
#endif /*__BT_GATT_OVER_BR/EDR_H__*/
