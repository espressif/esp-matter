/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
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


#ifndef __BT_L2CAP_H__
#define __BT_L2CAP_H__

#include "bt_type.h"
#include "bt_platform.h"

BT_EXTERN_C_BEGIN

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothL2CAP L2CAP
 * @{
 * @addtogroup L2CAP_EDR Classic Bluetooth
 * @{
 * This section describes the logical link control and adaptation protocol (L2CAP) APIs for Bluetooth with Enhanced Data Rate (EDR).
 * The L2CAP provides connection oriented data services to upper layer protocols with protocol multiplexing capability.
 * The L2CAP layer provides logical channels, named L2CAP channels, which are multiplexed over one or more logical links.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b L2CAP                      | Logical link control and adaptation protocol. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#Logical_link_control_and_adaptation_protocol_(L2CAP)">Wikipedia</a>. |
 * |\b PSM                        | Protocol/Service Multiplexer. It is used to connect the l2cap dynamic channel with the remote device. |
 * |\b MTU                        | Maximum transmission unit. It is the maximum SDU size of the capable of sending or receiving on the l2cap channel. |
 * |\b SDU                        | L2CAP service data units. |
 *
 * @section bt_l2cap_api_usage How to use this module
 * - The L2CAP user profile must first define its PSM value, register configuration, and l2cap event callback of the profile.
 *   Then, the L2CAP APIs can be used to connect or diconnect profile and send or receive data. The corresponding L2CAP events will also be reported in the L2CAP event callback.
 *
 *  - Sample code:
 *   - 1. Configure the L2CAP profiles and implement the L2CAP event callback, and then add these configuration to the L2CAP stack.
 *      @code
 *         typedef struct {
 *             uint32_t l2cap_handle;
 *             bt_l2cap_demo_profile_status_t status;
 *             uint16_t out_mtu;
 *             uint16_t in_mtu;
 *             uint16_t psm;
 *             bt_bd_addr_t addr;
 *         } bt_demo_profile_channel_context_t;
 *         typedef enum {
 *             BT_L2CAP_DEMO_PROFILE_IDLE = 0,
 *             BT_L2CAP_DEMO_PROFILE_CONNECTING,
 *             BT_L2CAP_DEMO_PROFILE_CONNECTED,
 *             BT_L2CAP_DEMO_PROFILE_DISCONNECTING,
 *             BT_L2CAP_DEMO_PROFILE_DISCONNECTED,
 *         } bt_l2cap_demo_profile_status_t;
 *
 *         #define BT_L2CAP_PSM_DEMO_PROFILE 0x1005
 *         uint8_t tx_data_buffer[512] = {0};
 *         uint8_t rx_data_buffer[512] = {0};
 *         bt_bd_addr_t bt_ldp_bt_addr = {0x11,0x22,0x33,0x44,0x55,0x66};
 *         bt_demo_profile_channel_context_t profile_context = {0};
 *
 *         uint32_t bt_l2cap_demo_profile_util_ascii_to_hex(const uint8_t *a, uint8_t len)
 *         {
 *             uint32_t i = 0;
 *             while (len) {
 *                 if ((*a >= '0') && (*a <= '9')) {
 *                     i = (i * 16) + (*a - '0');
 *                 } else if ((*a >= 'a') && (*a <= 'z')) {
 *                     i = (i * 16) + (*a - 'a') + 0xA;
 *                 } else if ((*a >= 'A') && (*a <= 'Z')) {
 *                     i = (i * 16) + (*a - 'A') + 0xA;
 *                 }
 *                 --len;
 *                 a++;
 *             };
 *             return i;
 *         }
 *
 *         bt_status_t bt_demo_profile_l2cap_psm_callback(uint32_t handle, bt_l2cap_dynamic_psm_callback_param_t *info)
 *         {
 *             bt_status_t status = BT_STATUS_SUCCESS;
 *             switch (info->event) {
 *                 case BT_L2CAP_EVENT_CONNECT_CNF : {
 *                     ...
 *                     break;
 *                 }
 *                 case BT_L2CAP_EVENT_DISCONNECTED : {
 *                     ...
 *                     break;
 *                 }
 *                 case BT_L2CAP_EVENT_CONNECT_IND : {
 *                     ...
 *                     break;
 *                 }
 *                 case BT_L2CAP_EVENT_DATA_IND : {
 *                     ...
 *                     break;
 *                 }
 *                 default:
 *                     break;
 *             }
 *             return status;
 *         }
 *
 *         const bt_l2cap_customer_config_info_t l2cap_customized_profile_config[] = {
 *             {
 *                 BT_L2CAP_MODE_BASIC,
 *                 BT_L2CAP_PSM_DEMO_PROFILE,
 *                 bt_demo_profile_l2cap_psm_callback,
 *                 BT_L2CAP_DEFAULT_MTU,
 *                 BT_L2CAP_MINIMUM_MTU,
 *                 BT_GAP_SECURITY_LEVEL_2,
 *             },
 *         };
 *
 *         uint8_t bt_l2cap_get_customer_profile_config(bt_l2cap_customer_config_info_t ***config_table)
 *         {
 *             *config_table = &l2cap_customized_profile_config;
 *             return sizeof(l2cap_customized_profile_config) / sizeof(bt_l2cap_customer_config_info_t);
 *         }
 *      @endcode
 *
 *   - 2. The local device connects the L2CAP channel to the remote device. If the SDP search is neccessary, the procedure of the SDP search should be executed before calling this API.
 *      @code
 *         bt_status_t bt_l2cap_demo_profile_connect(void)
 *         {
 *             if (bt_l2cap_connect(&(profile_context.l2cap_handle), &bt_ldp_bt_addr, profile_context.psm) == BT_STATUS_SUCCESS) {
 *                 profile_context.status = BT_L2CAP_DEMO_PROFILE_CONNECTING;
 *                 memcpy(&(profile_context.addr), &bt_ldp_bt_addr, sizeof(bt_bd_addr_t));
 *                 return BT_STATUS_SUCCESS;
 *             } else {
 *                 return BT_STATUS_FAIL;
 *             }
 *         }
 *
 *         bt_status_t bt_demo_profile_l2cap_psm_callback(uint32_t handle, bt_l2cap_dynamic_psm_callback_param_t *info)
 *         {
 *             bt_status_t status = BT_STATUS_SUCCESS;
= *             switch (info->event) {
 *                 case BT_L2CAP_EVENT_CONNECT_CNF : {
 *                     if (BT_STATUS_SUCCESS == info->status) {
 *                         if (profile_context.l2cap_handle == handle) {
 *                             profile_context.status = BT_L2CAP_DEMO_PROFILE_CONNECTED;
 *                             profile_context.out_mtu = info->remote_rx_mtu;
 *                         }
 *                     }
 *                     break;
 *                 }
 *                 default:
 *                     break;
 *             }
 *             return status;
 *         }
 *      @endcode
 *
 *   - 3. The local device receives a L2CAP connection request from the remote device and sends a corresponding response to the remote device.
 *      @code
 *         bt_status_t bt_demo_profile_l2cap_psm_callback(uint32_t handle, bt_l2cap_dynamic_psm_callback_param_t *info)
 *         {
 *             bt_status_t status = BT_STATUS_SUCCESS;
 *             switch (info->event) {
 *                 case BT_L2CAP_EVENT_CONNECT_IND : {
 *                     profile_context.status = BT_L2CAP_DEMO_PROFILE_CONNECTING;
 *                     profile_context.l2cap_handle = handle;
 *                     break;
 *                 }
 *                 default:
 *                     break;
 *             }
 *             return status;
 *         }
 *
 *         bt_status_t bt_l2cap_demo_profile_handle_connect_rsp(void)
 *         {
 *             return bt_l2cap_connect_rsp(profile_context.l2cap_handle, BT_L2CAP_CONN_ACCEPTED);
 *         }
 *
 *         bt_status_t bt_demo_profile_l2cap_psm_callback(uint32_t handle, bt_l2cap_dynamic_psm_callback_param_t *info)
 *         {
 *             bt_status_t status = BT_STATUS_SUCCESS;
 *             switch (info->event) {
 *                 case BT_L2CAP_EVENT_CONNECT_CNF : {
 *                     if (BT_STATUS_SUCCESS == info->status) {
 *                         if (profile_context.l2cap_handle == handle) {
 *                             profile_context.status = BT_L2CAP_DEMO_PROFILE_CONNECTED;
 *                             profile_context.out_mtu = info->remote_rx_mtu;
 *                         }
 *                     }
 *                     break;
 *                 }
 *                 default:
 *                     break;
 *             }
 *             return status;
 *         }
 *      @endcode
 *
 *   - 4. Send and receive data on the L2CAP channel.
 *      @code
 *         bt_status_t bt_l2cap_demo_profile_handle_send(void)
 *         {
 *             uint16_t real_len = sizeof(tx_data_buffer);
 *             if (profile_context.status == BT_L2CAP_DEMO_PROFILE_CONNECTED && real_len <= profile_context.out_mtu) {
 *                 return bt_l2cap_send(profile_context.l2cap_handle, &tx_data_buffer, real_len);
 *             } else {
 *                 return BT_STATUS_FAIL;
 *             }
 *         }
 *
 *         bt_status_t bt_demo_profile_l2cap_psm_callback(uint32_t handle, bt_l2cap_dynamic_psm_callback_param_t *info)
 *         {
 *             bt_status_t status = BT_STATUS_SUCCESS;
 *             switch (info->event) {
 *                 case BT_L2CAP_EVENT_DATA_IND : {
 *                     uint16_t copy_len = sizeof(rx_data_buffer);
 *                     bt_l2cap_demo_profile_dump_log(info->packet.data, info->packet.length);
 *                     break;
 *                 }
 *                 default:
 *                     break;
 *             }
 *             return status;
 *         }
 *      @endcode
 *
 *   - 5. Disconnect the L2CAP channel.
 *      @code
 *         bt_status_t bt_l2cap_demo_profile_handle_disconnect(void)
 *         {
 *             if (bt_l2cap_disconnect(profile_context.l2cap_handle) == BT_STATUS_SUCCESS) {
 *                 profile_context.status = BT_L2CAP_DEMO_PROFILE_DISCONNECTING;
 *                 return BT_STATUS_SUCCESS;
 *             } else {
 *                 return BT_STATUS_FAIL;
 *             }
 *         }
 *
 *         bt_status_t bt_demo_profile_l2cap_psm_callback(uint32_t handle, bt_l2cap_dynamic_psm_callback_param_t *info)
 *         {
 *             bt_status_t status = BT_STATUS_SUCCESS;
 *             switch (info->event) {
 *                 case BT_L2CAP_EVENT_CONNECT_IND : {
 *                     profile_context.status = BT_L2CAP_DEMO_PROFILE_CONNECTING;
 *                     profile_context.l2cap_handle = handle;
 *                     break;
 *                 }
 *                 default:
 *                     break;
 *             }
 *             return status;
 *         }
 *      @endcode
 *
 */


/**
 * @defgroup Bluetoothbt_l2cap_define Define
 * @{
 * Define Bluetooth L2CAP data types and values.
 */

/**
 *  @brief Define for L2CAP dynamic PSM type.
 */
#define BT_L2CAP_PSM_DYNAMIC_BEGIN    0x1000          /**< The value indicates the beginning of the dynamic PSM value that the user profile should be greater than.*/
typedef uint16_t bt_l2cap_dynamic_psm_value_t;    /**< The type of the dynamic PSM value. */

/**
 *  @brief Define for L2CAP event type. These events will be reported by the callback #bt_l2cap_dynamic_psm_callback_t that are registered by each profile.
 */
#define BT_L2CAP_EVENT_CONNECT_IND      1    /**< This event indicates the l2cap channel had received a connection indication from the remote device, and the API #bt_l2cap_connect_rsp must be called for connecting the l2cap channel.*/
#define BT_L2CAP_EVENT_CONNECT_CNF      2    /**< This event indicates the l2cap channel had connected with the filed "remote_rx_mtu" of the parameter #bt_l2cap_dynamic_psm_callback_param_t.*/
#define BT_L2CAP_EVENT_DISCONNECTED     3    /**< This event indicates the l2cap channel had disconnected.*/
#define BT_L2CAP_EVENT_DATA_IND         4    /**< This event indicates the data packet had been received with the filed "packet" of the parameter #bt_l2cap_dynamic_psm_callback_param_t.*/
typedef uint8_t bt_l2cap_event_type_t;    /**< The type of the L2CAP event type.*/

/**
 *  @brief Define for the result type of the response to connect the l2cap channel. It is used as the parameter of #bt_l2cap_connect_rsp .
 */
#define BT_L2CAP_CONNECT_ACCEPTED                     0x0000      /**< The result indicates the local side accepted the connection request, and the event #BT_L2CAP_EVENT_CONNECT_CNF will be received later.*/
#define BT_L2CAP_CONNECT_REJECT_NO_RESOURCES          0x0001      /**< The result indicates the local side rejected the connection request because there are not enough resources, and the event #BT_L2CAP_EVENT_DISCONNECTED will be received later.*/
typedef uint16_t bt_l2cap_connect_result_t;                   /**< The type of the result for connecting the l2cap channel.*/

/**
 *  @brief Define the default MTU size of the l2cap channel if the profile does not have a explicit MTU size.
 */
#define BT_L2CAP_DEFAULT_MTU           0x02A0

/**
 *  @brief Define the minimum MTU size of the l2cap channel that the profile must support.
 */
#define BT_L2CAP_MINIMUM_MTU           0x0030

/**
 *  @brief Define the maximum MTU size of the l2cap channel.
 */
#define BT_L2CAP_MAXIMUM_MTU           0x0400

/**
 *  @brief Define for L2CAP channel mode type. Only the basic mode is supported now.
 */
#define BT_L2CAP_MODE_BASIC             0x00     /**< The basic mode of the L2CAP channel.*/
#define BT_L2CAP_MODE_INVAILID          0xFF     /**< The invalid mode of the L2CAP channel.*/
typedef uint8_t bt_l2cap_channel_mode_t;         /**< The type of the L2CAP channel mode.*/
/**
 * @}
 */

/**
 * @defgroup Bluetoothbt_l2cap_struct Struct
 * @{
 * Define Classic Bluetooth L2CAP structures for L2CAP APIs and events.
 */

/**
 * @brief This structure defines the #bt_l2cap_dynamic_psm_callback_t parameter type.
 */
typedef struct {
    bt_l2cap_event_type_t event;                     /**<The L2CAP event.*/
    bt_status_t status;                              /**<The status of the L2CAP event. It also indicates the reason for the event #BT_L2CAP_EVENT_DISCONNECTED or #BT_L2CAP_EVENT_CONNECT_CNF.*/
    union {
        bt_bd_addr_t *address;                       /**<The Bluetooth address of a remote device which is valid for the event #BT_L2CAP_EVENT_CONNECT_IND. */
        uint16_t remote_rx_mtu;                      /**<The MTU size of the RX capability of the remote L2CAP which is valid for the event #BT_L2CAP_EVENT_CONNECT_CNF*/
        struct {
            void *data;                              /**<The buffer of the received data which is valid for the event #BT_L2CAP_EVENT_DATA_IND*/
            uint16_t length;                         /**<The buffer size of the received data which is valid for the event #BT_L2CAP_EVENT_DATA_IND*/
        } packet;
    };
} bt_l2cap_dynamic_psm_callback_param_t;

/**
 * @brief Define for the callback type of the L2CAP channel. The profile need to implement it for receiving the L2CAP events that defined in #bt_l2cap_event_type_t.
 */
typedef bt_status_t (*bt_l2cap_dynamic_psm_callback_t)(uint32_t handle, bt_l2cap_dynamic_psm_callback_param_t *info);

/**
 * @brief This structure defines the #bt_l2cap_get_customer_profile_config parameter type.
 */
typedef struct {
    bt_l2cap_channel_mode_t l2cap_mode;             /**<The L2CAP mode.*/
    bt_l2cap_dynamic_psm_value_t psm;               /**<The L2CAP dynamic PSM value.*/
    bt_l2cap_dynamic_psm_callback_t callback;       /**<The callback of the L2CAP channel for receiving events.*/
    uint16_t max_rx_mtu;                            /**<The maximum RX MTU size for the L2CAP channel. It is the maximum size that the remote L2CAP can receive.*/
    uint16_t min_tx_mtu;                            /**<The minimum TX MTU size for the L2CAP channel. It is the minimum size that the local L2CAP can support to send all TX packets of the profile. It will also be no less than #BT_L2CAP_MINIMUM_MTU.*/
    uint16_t security_level;                        /**<The security level of the l2cap channel defined in #bt_gap_security_level_t.*/
} bt_l2cap_customer_config_info_t;
/**
 * @}
 */

/**
 * @brief     This function connects a L2CAP channel with a remote device by Bluetooth address and PSM. The event #BT_L2CAP_EVENT_CONNECT_CNF should then also be received.
 * @param[out] handle                 is the handle of the l2cap channel.
 * @param[in]  address                is the Bluetooth address of a remote device.
 * @param[in]  psm                    is the PSM value of the profile.
 * @return                            #BT_STATUS_SUCCESS means the connection request has been sent. A confirmation of the connection will be received.
 *                                    #BT_STATUS_FAIL means sending the connection request failed.
 *                                    #BT_STATUS_L2CAP_CHANNEL_EXISTED means the l2cap channel already existed.
 *                                    #BT_STATUS_LINK_IS_DISCONNECTING means the acl link is disconnecting and that cannot connect to an l2cap channel.
 *                                    #BT_STATUS_L2CAP_LOCAL_REJECTED_NO_RESOURCES means the l2cap connection buffer is not enough.
 */
bt_status_t bt_l2cap_connect(uint32_t *handle, const bt_bd_addr_t *address, bt_l2cap_dynamic_psm_value_t psm);

/**
 * @brief     This function sends the connection response to connect an l2cap channel when the #BT_L2CAP_EVENT_CONNECT_IND event is received.
 * @param[in] handle                  is the handle of the l2cap channel.
 * @param[in] result                  is the result of the connection response.
 * @return                            #BT_STATUS_SUCCESS means the connection response has been sent out and the connection confirmation will be received.
 *                                    #BT_STATUS_FAIL means the handle, the data or the length is invalid.
 *                                    #BT_STATUS_OUT_OF_MEMORY means the TX memory of the Bluetooth stack is not enough.
 */
bt_status_t bt_l2cap_connect_rsp(uint32_t handle, bt_l2cap_connect_result_t result);

/**
 * @brief     This function sends the data to remote device after the l2cap channel is connected.
 * @param[in] handle                  is the handle of the l2cap channel.
 * @param[in] data                    is the data to send on the l2cap channel.
 * @param[in] length                  is the length of the data to send on the l2cap channel.
 * @return                            #BT_STATUS_SUCCESS means the data has been sent out successfully.
 *                                    #BT_STATUS_FAIL means the handle, the data or the length is invalid.
 *                                    #BT_STATUS_OUT_OF_MEMORY means the TX memory the of Bluetooth stack is not enough.
 */
bt_status_t bt_l2cap_send(uint32_t handle, void *data, uint32_t length);

/**
 * @brief     This function disconnects the l2cap channel after it is connected.
 * @param[in] handle                  is the handle of the l2cap channel.
 * @return                            #BT_STATUS_SUCCESS means the disconnection request has been sent. Wait for the disconnection confirmation.
 *                                    #BT_STATUS_FAIL means the handle is invalid.
 */
bt_status_t bt_l2cap_disconnect(uint32_t handle);

/**
 * @brief     This function gets the handle of the l2cap channel by Bluetooth address and PSM.
 * @param[in]  addr                   is the Bluetooth address of a remote device.
 * @param[in]  psm                    is the PSM value of the profile.
 * @return                            is the handle of the l2cap channel.
 */
uint32_t bt_l2cap_get_channel_handle(const bt_bd_addr_t *addr, bt_l2cap_dynamic_psm_value_t psm);

/**
 * @brief     This is a user defined callback and called by the L2CAP module. It should be implemented by the application to provide the customer config and profile number.
 * @param[in] config_table   is a pointer to the config table. If there is no customized profile, the config_table should be NULL and return 0.
 * @return    the number of profiles.
 */
uint8_t bt_l2cap_get_customer_profile_config(const bt_l2cap_customer_config_info_t ***config_table);

BT_EXTERN_C_END
/**
 * @}
 * @}
 * @}
 */
#endif /*__BT_L2CAP_H__*/

