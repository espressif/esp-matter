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

#ifndef __BT_PBAPC_H__
#define __BT_PBAPC_H__

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothPBAP PBAP
 * @{
 * The Phonebook Access Profile (PBAP) defines the protocols and procedures to exchange phonebook objects between two peer devices.
 * PBAP client (PBAPC) is implemented for the client side connection according to the PBAP specification. It can initiate a connection to a remote device.
 * The API provides the following functionalities, such as get the number of phonebook objects, get the phonebook objects, get the caller's name by the phone number and more.
 * Note: It only supports access to phonebook objects storaged in Phone, not support in SIM.
 * @section bt_pbapc_api_usage How to use this module
 * - PBAPC connection with the server in open mode, as shown in the figure titled as "PBAPC connection establishment without authentication message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *  - Step1: Mandatory, call the function #bt_pbapc_connect() to initiate a connection to a remote server.
 *   - Sample code:
 *    @code
 *       ret = bt_pbapc_connect(&handle, &address);
 *    @endcode
 *  - Step2: #bt_app_event_callback() is triggered by #BT_PBAPC_CONNECT_CNF event, if the connection is established.
 *    - Sample code:
 *     @code
 *       bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *       {
 *           switch (msg)
 *           {
 *               case BT_PBAPC_CONNECT_CNF:
 *               {
 *                   bt_pbapc_connect_cnf_t *data = (bt_pbapc_connect_cnf_t*)buff;
 *                   // Check the connection result and save conn_id if the connection is successful.
 *                   break;
 *               }
 *           }
 *       }
 *     @endcode
 * - Get the number of phonebook objects, as shown in the figure titled as "Get the number of phonebook objects message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *  - Step1: Mandatory, call the function #bt_pbapc_get_num_of_phonebook_objects() to get the number of phonebook objects of type is Missed Calls History or Main Phonebook from a remote device.
 *   - Sample code:
 *    @code
 *       ret = bt_pbapc_get_num_of_phonebook_objects(handle, type);
 *    @endcode
 *  - Step2: #bt_app_event_callback() is triggered by the #BT_PBAPC_GET_NUMBER_OF_PHOENBOOK_OBJECTS_IND event, if there is data received from a remote device.
 *  - Step3: #bt_app_event_callback() is triggered by the #BT_PBAPC_GET_NUMBER_OF_PHOENBOOK_OBJECTS_CNF event, as a result of a call to the function #bt_pbapc_get_num_of_phonebook_objects().
 *   - Sample code:
 *     @code
 *        bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *        {
 *            switch (event_id)
 *            {
 *                 case BT_PBAPC_GET_NUMBER_OF_PHOENBOOK_OBJECTS_IND:
 *                 {
 *                     bt_pbapc_get_num_of_phonebook_objects_ind_t *ind = (bt_pbapc_get_num_of_phonebook_objects_ind_t *)buff;
 *                      // Show the number of phonebook objects.
 *                     break;
 *                 }
 *                 case BT_PBAPC_GET_NUMBER_OF_PHOENBOOK_OBJECTS_CNF:
 *                 {
 *                     bt_pbapc_get_num_of_phonebook_objects_cnf_t *cnf = (bt_pbapc_get_num_of_phonebook_objects_cnf_t *)buff;
 *                     // Check the response.
 *                     break;
 *                 }
 *            }
 *        }
 *     @endcode
 * - Get data. Provides two methods, download and browsing.
 *  -  Download. Get the data using PullPhonebook function, as shown in the figure titled as "PBAPC Get phonebook object message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Step1: Optional, call #bt_pbapc_get_phonebook_objects() to retrieve the phonebook objects of type Missed Calls History or Main Phonebook from a remote device.
 *    - Sample code:
 *     @code
 *        ret = bt_pbapc_get_phonebook_objects(handle, offset,  type);
 *     @endcode
 *   - Step2: #bt_app_event_callback() is triggered by the #BT_PBAPC_GET_PHOENBOOK_OBJECTS_IND event, if there is data received from a remote device.
 *   - Step3: #bt_app_event_callback() is triggered by the #BT_PBAPC_GET_PHONEBOOK_OBJECTS_CNF event, as a result of a call to the function #bt_pbapc_get_phonebook_objects().
 *    - Sample code:
 *     @code
 *        bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *        {
 *            switch (event_id)
 *            {
 *                 case BT_PBAPC_GET_PHOENBOOK_OBJECTS_IND:
 *                 {
 *                     bt_pbapc_get_phonebook_objects_ind_t *ind = (bt_pbapc_get_phonebook_objects_ind_t *)buff;
 *                      // Show the name and number of phonebook objects.
 *                     break;
 *                 }
 *                 case BT_PBAPC_GET_PHONEBOOK_OBJECTS_CNF:
 *                 {
 *                     bt_pbapc_get_phonebook_objects_cnf_t *cnf = (bt_pbapc_get_phonebook_objects_cnf_t *)buff;
 *                     // Check the response.
 *                     break;
 *                 }
 *            }
 *        }
 *     @endcode
 *  - Browsing. Get the data using PullvCardlisting function, as shown in the figure titled as "PBAPC get caller name by number message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Step1: Optional, call #bt_pbapc_get_caller_name_by_number() to retrieve contact name of phonebook objects stored in the phone from a remote device.
 *    - Sample code:
 *     @code
 *        ret = bt_pbapc_get_caller_name_by_number(handle, number);
 *     @endcode
 *   - Step2: #bt_app_event_callback() is triggered by the #BT_PBAPC_GET_CALLER_NAME_IND event, if there is data received from a remote device.
 *   - Step3: #bt_app_event_callback() is triggered by the #BT_PBAPC_GET_CALLER_NAME_CNF event, as a result of a call to the function #bt_pbapc_get_caller_name_by_number().
 *    - Sample code:
 *     @code
 *        bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *        {
 *            switch (event_id)
 *            {
 *                case BT_PBAPC_GET_CALLER_NAME_IND:
 *                {
 *                    bt_pbapc_get_caller_name_ind_t *ind = (bt_pbapc_get_caller_name_ind_t *)buff;
 *                    // Show  the contact name of phonebook objects.
 *                    break;
 *                }
 *                case BT_PBAPC_GET_CALLER_NAME_CNF:
 *                {
 *                    bt_pbapc_get_caller_name_cnf_t *cnf = (bt_pbapc_get_caller_name_cnf_t *)buff;
 *                    // Check the response.
 *                    break;
 *                }
 *            }
 *        }
 *     @endcode
 * - Pull the specific vCard, as shown in the figure titled as "PBAPC PullvCardEnty message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *  - Step1: Optional, call #bt_pbapc_pull_vcard_entry() to retrieve the specific phonebook objects stored in the phone from a remote device.
 *   - Sample code:
 *    @code
 *       ret = bt_pbapc_pull_vcard_entry(handle,  index);
 *    @endcode
 *  - Step2: #bt_app_event_callback() is triggered by the #BT_PBAPC_GET_PHOENBOOK_OBJECTS_IND event, if there is data received from a remote device.
 *  - Step3: #bt_app_event_callback() is triggered by the #BT_PBAPC_GET_PHONEBOOK_OBJECTS_CNF event, as a result of a call to the function #bt_pbapc_pull_vcard_entry().
 *   - Sample code:
 *    @code
 *       bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *       {
 *           switch (event_id)
 *           {
 *                case BT_PBAPC_GET_PHOENBOOK_OBJECTS_IND:
 *                {
 *                     bt_pbapc_get_phonebook_objects_ind_t *ind = (bt_pbapc_get_phonebook_objects_ind_t *)buff;
 *                      // Show the name and number of phonebook objects.
 *                     break;
 *                }
 *                case BT_PBAPC_GET_PHONEBOOK_OBJECTS_CNF:
 *                {
 *                    bt_pbapc_get_phonebook_objects_cnf_t *cnf = (bt_pbapc_get_phonebook_objects_cnf_t *)buff;
 *                    // Check the response.
 *                    break;
 *                }
 *           }
 *       }
 *    @endcode
 * -  Disconnect PBAPC connection. Close the PBAPC connection, as shown in the figure titled as "Disconnect message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *  - Step1: Call #bt_pbapc_disconnect() to disconnect the PBAPC connection from a remote device.
 *   - Sample code:
 *    @code
 *       ret = bt_pbapc_disconnect(handle);
 *    @endcode
 *  - Step2: #bt_app_event_callback() is triggered by the #BT_PBAPC_DISCONNECT_IND event to indicate the result of the disconnection.
 *   - Sample code:
 *    @code
 *       bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *       {
 *           switch (msg)
 *           {
 *               case BT_PBAPC_DISCONNECT_IND:
 *               {
 *                   bt_pbapc_disconnect_ind_t *ind = (bt_pbapc_disconnect_ind_t *)buff;
 *                   if (status == BT_STATUS_SUCCESS) {
 *                       // The PBAPC has disconnected.
 *                   }
 *                   break;
 *               }
 *           }
 *       }
 *    @endcode
 */
#include "bt_type.h"
#include "bt_system.h"
#include "bt_debug.h"
#include "bt_platform.h"

/**
 * @defgroup Bluetoothbt_pbapc_define Define
 * @{
 */
/**
 * @brief The following macros define the PBAPC event types.
 */

#define BT_PBAPC_CONNECT_CNF                          (BT_MODULE_PBAPC|0x01) /**< The connection confirmation event, a result of the connection establishment initiated by the local device, with #bt_pbapc_connect_cnf_t as the payload in the callback function.*/
#define BT_PBAPC_GET_NUMBER_OF_PHOENBOOK_OBJECTS_IND  (BT_MODULE_PBAPC|0x02) /**< The indication event of getting the number of missed call objects from the remote device, with #bt_pbapc_get_number_of_phonebook_objects_ind_t as the payload in the callback function.*/
#define BT_PBAPC_GET_NUMBER_OF_PHOENBOOK_OBJECTS_CNF  (BT_MODULE_PBAPC|0x03) /**< The confirmation event as a result of the completion of getting the number of missed call objects from the remote device, with #bt_pbapc_get_number_of_phonebook_objects_cnf_t as the payload in the callback function.*/
#define BT_PBAPC_GET_PHOENBOOK_OBJECTS_IND            (BT_MODULE_PBAPC|0x04) /**< The indication event of getting a missed call object from the remote device, with #bt_pbapc_get_phonebook_objects_ind_t as the payload in the callback function.*/
#define BT_PBAPC_GET_PHONEBOOK_OBJECTS_CNF            (BT_MODULE_PBAPC|0x05) /**< The confirmation event as a result of the completion of getting the missed call object from the remote device, with #bt_pbapc_get_phonebook_objects_cnf_t as the payload in the callback function.*/
#define BT_PBAPC_GET_CALLER_NAME_IND                  (BT_MODULE_PBAPC|0x06) /**< The indication event of getting the caller name object from the remote device, with #bt_pbapc_get_caller_name_ind_t pas the payload in the callback function.*/
#define BT_PBAPC_GET_CALLER_NAME_CNF                  (BT_MODULE_PBAPC|0x07) /**< The confirmation event as a result of the completion of getting the caller name object from the remote device, with #bt_pbapc_get_caller_name_cnf_t as the payload in the callback function.*/
#define BT_PBAPC_DISCONNECT_IND                       (BT_MODULE_PBAPC|0x08) /**< The disconnection indication event, initiated by a local or a remote device, with #bt_pbapc_disconnect_ind_t as the payload in the callback function. */
/**
 * @brief The following macros define the type of phonebook objects.
 */
typedef uint8_t bt_pbapc_phonebook_type_t;
#define BT_PBAPC_MISSED_CALL_TYPE 0x00 /**< The missed call type. */
#define BT_PBAPC_POHONEBOOK_TYPE 0x01 /**< The phonebook type. */
/**
 * @}
 */

/**
 * @defgroup Bluetoothbt_pbapc_struct Struct
 * @{
 */

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_PBAPC_CONNECT_CNF event.
 */
typedef struct {
    uint32_t handle;                         /**< PBAPC handle. */
} bt_pbapc_connect_cnf_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_PBAPC_DISCONNECT_IND event.
 */
typedef struct {
    uint32_t handle;                         /**< PBAPC handle. */
} bt_pbapc_disconnect_ind_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_PBAPC_GET_NUMBER_OF_PHOENBOOK_OBJECTS_CNF event.
 */
typedef struct {
    uint32_t handle;                         /**< PBAPC handle. */
} bt_pbapc_get_number_of_phonebook_objects_cnf_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_PBAPC_GET_NUMBER_OF_PHOENBOOK_OBJECTS_IND event.
 */
typedef struct {
    uint32_t handle;                         /**< PBAPC handle. */
    uint16_t number;                         /**< The number of Phonebook objects*/
    uint8_t new_missed_call;                 /**< Unread missed call number*/
} bt_pbapc_get_number_of_phonebook_objects_ind_t;
/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_PBAPC_GET_PHONEBOOK_OBJECTS_CNF event.
 */
typedef struct {
    uint32_t handle;                         /**< PBAPC handle. */
} bt_pbapc_get_phonebook_objects_cnf_t;

/**
 *  @brief This structure defines the name and number of a missed call.
 */
typedef struct {
    uint8_t number_length;                   /**< The length of the number. */
    uint8_t name_length;                     /**< The length of the name. */
    uint8_t *number;                         /**< The content of the number. */
    uint8_t *name;                           /**< The content of the name. */
} bt_pbapc_phonebook_objects_t;

/**
 *  @brief This structure defines the data type of the parameter in the callback for #BT_PBAPC_GET_PHOENBOOK_OBJECTS_IND event.
 */
typedef struct {
    uint32_t handle;                         /**< PBAPC handle. */
    bt_pbapc_phonebook_objects_t *object;    /**< The information of phonebook objects. */
} bt_pbapc_get_phonebook_objects_ind_t;

/**
 * @brief This structure defines the contact name.
 */
typedef struct {
    uint8_t length;                          /**< The length of the contact name. */
    uint8_t *name;                           /**< The the contact name. */
} bt_pbapc_contact_t;

/**
 *  @brief This structure defines the data type of the parameter in the callback for #BT_PBAPC_GET_CALLER_NAME_CNF event.
 */
typedef struct {
    uint32_t handle;                         /**< PBAPC handle. */
} bt_pbapc_get_caller_name_cnf_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_PBAPC_GET_CALLER_NAME_IND event.
 */
typedef struct {
    uint32_t handle;                         /**< PBAPC handle. */
    bt_pbapc_contact_t *caller;              /**< The contact information of phonebook objects. */
} bt_pbapc_get_caller_name_ind_t;

/**
* @}
*/


BT_EXTERN_C_BEGIN
/**
 * @brief               This function connects to the specified remote device.
 *                      #BT_PBAPC_CONNECT_CNF event is sent to the upper layer with the connection result.
 * @param[out] handle   is the identifier of the remote device.
 * @param[in]  address  is the Bluetooth address of the remote device.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                          #BT_STATUS_FAIL, the operation has failed.
 *                          #BT_STATUS_OUT_OF_MEMORY, out of memory.

 */
bt_status_t bt_pbapc_connect(uint32_t *handle, const bt_bd_addr_t *address);

/**
 * @brief               This function gets the number of phonebook objects of type Missed Calls History or Main Phonebook of the specified remote device.
 *                      The #BT_PBAPC_GET_NUMBER_OF_PHOENBOOK_OBJECTS_IND & #BT_PBAPC_GET_NUMBER_OF_PHOENBOOK_OBJECTS_CNF event is sent to the upper layer with the connection result.
 * @param[in] handle is the identifier of the remote device.
 * @param[in] type   is the type of phonebook objects.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                          #BT_STATUS_FAIL, the operation has failed.
 *                          #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_pbapc_get_num_of_phonebook_objects(uint32_t handle, bt_pbapc_phonebook_type_t type);

/**
 * @brief               This function gets the phonebook objects of type Missed Calls History or Main Phone Book of the specified remote device.
 *                      #BT_PBAPC_GET_PHOENBOOK_OBJECTS_IND and #BT_PBAPC_GET_PHONEBOOK_OBJECTS_CNF events are sent to the upper layer with the connection result.
 * @param[in]  handle   is the identifier of the remote device.
 * @param[in]  offset    is the start offset.
 * @param[in]  type      is the type of phonebook objects.
 * @return               #BT_STATUS_SUCCESS, the operation completed successfully.
 *                           #BT_STATUS_FAIL, the operation has failed.
 *                           #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_pbapc_get_phonebook_objects(uint32_t handle, uint16_t offset, bt_pbapc_phonebook_type_t type);

/**
 * @brief               This function gets the contact name of a phonebook object stored in the phonebook of the remote device.
 *                      The #BT_PBAPC_GET_CALLER_NAME_IND and #BT_PBAPC_GET_CALLER_NAME_CNF events are sent to the upper layer with the connection result.
 * @param[in] handle   is the identifier of the remote device.
 * @param[in] number  is the telephone number, if it is null, the result is the number of phonebook objects stored in the phone.
 * @return                   #BT_STATUS_SUCCESS, the operation completed successfully.
 *                               #BT_STATUS_FAIL, the operation has failed.
 *                               #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_pbapc_get_caller_name_by_number(uint32_t handle, uint8_t *number);

/**
 * @brief               This function pulls a specific vCard object from the specified remote device.
 *                      The #BT_PBAPC_GET_PHOENBOOK_OBJECTS_IND and #BT_PBAPC_GET_PHONEBOOK_OBJECTS_CNF events are sent to the upper layer with the connection result.
 * @param[in] handle   is the identifier of the remote device.
 * @param[in] index     is the index of phonebook objects.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                          #BT_STATUS_FAIL, the operation has failed.
 *                          #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_pbapc_pull_vcard_entry(uint32_t handle, uint8_t index);

/**
 * @brief               This function disconnects from the specified remote device.
 *                      #BT_PBAPC_DISCONNECT_IND event is sent to the upper layer with the result of the disconnection.
 * @param[in]  handle   is the identifier of the remote device.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                          #BT_STATUS_FAIL, the operation has failed.
 *                          #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_pbapc_disconnect(uint32_t handle);

/**
 * @brief               This function connects with authentication to the specified remote device.
 *                      #BT_PBAPC_CONNECT_CNF event is sent to the upper layer with the connection result.
 * @param[in] handle   is the identifier of the remote device.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the operation has failed.
 *                      #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_pbapc_connect_with_auth(uint32_t handle);

BT_EXTERN_C_END
/**
 * @}
 * @}
 */

#endif

