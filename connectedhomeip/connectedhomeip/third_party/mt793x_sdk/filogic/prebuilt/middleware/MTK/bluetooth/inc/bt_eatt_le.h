/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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


#ifndef __BT_EATT_LE_H__
#define __BT_EATT_LE_H__

#include "bt_platform.h"
#include "bt_type.h"
#include "bt_gattc.h"
#include "bt_gatts.h"


typedef uint8_t bt_eatt_le_conn_req_failed_reason;
#define BT_EATT_LE_CONN_REQ_FAILED_DUE_TO_INVALID_RESPONSE_FROM_REMOTE          0x00    /**<  failed to connect EATT since remote device's response is invalid*/
#define BT_EATT_LE_CONN_REQ_REFUSED_BY_REMOTE_PSM_NOT_SUPPORT                   0x02    /**<  remote device refuse to connect EATT since it does not support this PSM*/
#define BT_EATT_LE_CONN_REQ_REFUSED_BY_REMOTE_INSUFFICIENT_AUTHENTICAIOTN       0x05    /**<  remote device refuse to connect EATT since authentication is not sufficient*/
#define BT_EATT_LE_CONN_REQ_REFUSED_BY_REMOTE_INSUFFICIENT_AUTHORIZATION        0x06    /**<  remote device refuse to connect EATT since authorization is not sufficient*/
#define BT_EATT_LE_CONN_REQ_REFUSED_BY_REMOTE_INSUFFICIENT_ENC_KEY_SIZE         0x07    /**<  remote device refuse to connect EATT since encryption key size is not long enough*/
#define BT_EATT_LE_CONN_REQ_REFUSED_BY_REMOTE_INSUFFICIENT_ENC                  0x08    /**<  remote device refuse to connect EATT since connection is not encrypted*/
#define BT_EATT_LE_CONN_REQ_REFUSED_BY_REMOTE_UNACCEPTABLE_PARAMETERS           0x0B    /**<  remote device refuse to connect EATT since our parameter is not accepted*/


typedef uint8_t bt_eatt_le_disconnect_reason;
#define BT_EATT_LE_CHANNEL_DISCONNECTED_DUE_TO_LOCAL_REQUEST                    0x00    /**<  EATT disconnected since request by local host*/
#define BT_EATT_LE_CHANNEL_DISCONNECTED_DUE_TO_DUPLICATED_CID                   0x01    /**<  EATT disconnected since duplicated CID allocated by remote device*/
#define BT_EATT_LE_CHANNEL_DISCONNECTED_DUE_TO_CREDIT_OVERFLOW                  0x02    /**<  EATT disconnected since remote peer returned too much credit*/
#define BT_EATT_LE_CHANNEL_DISCONNECTED_DUE_TO_CREDIT_UNDERFLOW                 0x03    /**<  EATT disconnected since remote peer send packet while credit is zero*/
#define BT_EATT_LE_CHANNEL_DISCONNECTED_DUE_TO_INVALID_MTU_CONFIGURATION        0x04    /**<  EATT disconnected since remote peer reconfigured MTU incorrectly*/
#define BT_EATT_LE_CHANNEL_DISCONNECTED_DUE_TO_REMOTE_REQUEST                   0x05    /**<  EATT disconnected since remote peer requested to do so*/
#define BT_EATT_LE_CHANNEL_DISCONNECTED_DUE_TO_INVALID_FRAME_RECEIVED           0x06    /**<  EATT disconnected since remote peer sent invalid packet*/



typedef uint8_t le_eatt_event_type;
#define    BT_EATT_EVENT_CHANNEL_CONNECTED              0   /**<parameter format: #bt_eatt_evt_channel_connected_evt_t        */
#define    BT_EATT_EVENT_CONNECTION_REQUEST_FAILED      1   /**<parameter format: #bt_eatt_evt_conn_req_failed_evt_t          */
#define    BT_EATT_EVENT_CHANNEL_DISCONNECTED           2   /**<parameter format: #bt_eatt_evt_channel_disconnected_evt_t     */
#define    BT_EATT_EVENT_CHANNEL_RECONFIGURED           3   /**<parameter format: #bt_eatt_evt_channel_reconfigured_evt_t     */

#define    EATT_CONNECTED_EVT_MAX_CID_NUM               5   /**< maximum number of channels that can be connected within one connection event  */



/**
 * @brief EATT connected event type.
 */
typedef struct {
    uint16_t            remote_cid[EATT_CONNECTED_EVT_MAX_CID_NUM];             /**<  remote channel ID. Used to identify remote channels while calling EATT APIs*/
    uint16_t            mtu;                                                    /**<  remote channel MTU size*/
    uint8_t             num_of_channels;                                        /**<  indicates how many channels is connected in this event*/
} bt_eatt_evt_channel_connected_evt_t;

/**
 * @brief EATT connection request fail event type.
 */
typedef struct {
    uint8_t             reason;    /**<  refer #bt_eatt_le_conn_req_failed_reason */
} bt_eatt_evt_conn_req_failed_evt_t;

/**
 * @brief EATT disconnected event type.
 */
typedef struct {
    uint16_t            remote_cid;             /**<   remote channel ID which reconfigured its MTU size*/
    uint16_t            mtu;                    /**<   new MTU size reconfigured by remote channel*/
} bt_eatt_evt_channel_reconfigured_evt_t;

typedef struct {
    uint16_t            remote_cid;             /**<   remote channel ID which just disconnected*/
    uint8_t             reason;                 /**<   disconnection reason  refer #bt_eatt_le_disconnect_reason*/
} bt_eatt_evt_channel_disconnected_evt_t;

typedef void (* eatt_evt_cb)(le_eatt_event_type type, void *parameter);
typedef bt_status_t (*gatt_procedure_t)(bt_handle_t connection_handle,const void *req);


/*      EATT API       */
/**
 * @brief     This function initialize the channel number of EATT.
 * @param[in] channel_num  is the number of EATT channel.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully.
 *            #BT_STATUS_OUT_OF_MEMORY, the memory is not enough for requested channel number.
 */
bt_status_t bt_le_eatt_init(uint32_t channel_num);

/**
 * @brief     This function tries to create EATT connections with specified peer device.
 *
 * @param[in]  connection_handle  specifies the device to connect with.
 * @param[in]  num_of_channels    is the number of channels to connect. Should not be greater than #EATT_CONNECTED_EVT_MAX_CID_NUM
 * @param[in]  evt_cb             is the callback function to handle EATT events.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_le_eatt_connect(
    bt_handle_t connection_handle,
    uint8_t num_of_channels,
    eatt_evt_cb  evt_cb);

/**
 * @brief     This function returns the current remote cid of EATT bearer while called during GATT client module event
 *
 * @param[in] connection_handle   is the connection handle of current connection
 * @return    remote cid
 */
uint16_t bt_le_eatt_get_current_channel_id(uint16_t connection_handle);

/**
 * @brief     This function is an general enhanced GATT API which sends protocol via connected EATT bearer or ATT bearer.
 *            Multiple ATT/EATT transactions can be processed on different channels simultaneously.
 *
 * @param[in] remote_cid   is a pointer to the remote channel id of connected EATT bearer.
 *            Should be obtained from #bt_le_eatt_get_current_channel_id() or #bt_eatt_evt_channel_connected_evt_t.
 *            if remote_cid is 0,the function will selecting a ATT/EATT bearer to excute gatt_procedure and return the remote_cid.
 *            if remote_cid is not 0,the function will finding the ATT/EATT bearer with the remote channel id is remote_cid to excute gatt_procedure.
 * @param[in] gatt_procedure   is the gatt procedure API of client or server which is used to send protocol.
 *            refer #bt_gattc_read_charc or #bt_gatts_send_charc_value_notification_indication and so on for more details of parameters required by this API.
 * @param[out] remote_cid   is a pointer to the remote channel id of which ATT/EATT bearer is selected to send protocol.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_le_eatt_procedure(
    bt_handle_t connection_handle,
    const void *req,
    uint16_t *remote_cid,
    gatt_procedure_t gatt_procedure);

/**
 * @brief     This function is an enhanced GATT API which sends protocol via connected EATT bearer or ATT bearer.
 *            Multiple ATT/EATT transactions can be processed on different channels simultaneously.
 *            refer #bt_gatts_send_response for more details of parameters required by this API.
 *
 * @param[in] remote_cid   is a pointer to the remote channel id of connected EATT bearer.
 *            Should be obtained from #bt_le_eatt_get_current_channel_id() or #bt_eatt_evt_channel_connected_evt_t.
 *            if remote_cid is 0,the function will selecting a ATT/EATT bearer to send protocol and return the remote_cid.
 *            if remote_cid is not 0,the function will finding the ATT/EATT bearer with the remote channel id is remote_cid to send protocol.
 * @param[out] remote_cid   is a pointer to the remote channel id of which ATT/EATT bearer is selected to send protocol.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_le_eatt_procedure_gatts_send_response(
    bt_handle_t connection_handle,
    uint8_t result,
    const uint8_t rw,
    void *data,
    uint16_t *remote_cid);

/**
 * @brief     This function is an enhanced GATT API which sends protocol via connected EATT bearer or ATT bearer.
 *            Multiple ATT/EATT transactions can be processed on different channels simultaneously.
 *            refer #bt_gattc_write_without_rsp for more details of parameters required by this API.
 *
 * @param[in] remote_cid   is a pointer to the remote channel id of connected EATT bearer.
 *            Should be obtained from #bt_le_eatt_get_current_channel_id() or #bt_eatt_evt_channel_connected_evt_t.
 *            if remote_cid is 0,the function will selecting a ATT/EATT bearer to send protocol and return the remote_cid.
 *            if remote_cid is not 0,the function will finding the ATT/EATT bearer with the remote channel id is remote_cid to send protocol.
 * @param[out] remote_cid   is a pointer to the remote channel id of which ATT/EATT bearer is selected to send protocol.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_le_eatt_procedure_gattc_write_without_rsp(
    bt_handle_t connection_handle,
    const bt_gattc_write_without_rsp_req_t *req,
    uint16_t *remote_cid);

/**
 * @brief     This function is an enhanced GATT API which sends protocol via connected EATT bearer or ATT bearer.
 *            Multiple ATT/EATT transactions can be processed on different channels simultaneously.
 *            refer #bt_gattc_prepare_write_charc for more details of parameters required by this API.
 *
 * @param[in] remote_cid   is a pointer to the remote channel id of connected EATT bearer.
 *            Should be obtained from #bt_le_eatt_get_current_channel_id() or #bt_eatt_evt_channel_connected_evt_t.
 *            if remote_cid is 0,the function will selecting a ATT/EATT bearer to send protocol and return the remote_cid.
 *            if remote_cid is not 0,the function will finding the ATT/EATT bearer with the remote channel id is remote_cid to send protocol.
 * @param[out] remote_cid   is a pointer to the remote channel id of which ATT/EATT bearer is selected to send protocol.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
    bt_status_t bt_le_eatt_procedure_gattc_prepare_write_charc(
        bt_handle_t connection_handle,
        uint8_t is_reliable,
        uint16_t offset,
        const bt_gattc_prepare_write_charc_req_t *req,
        uint16_t *remote_cid);

#endif //__BT_EATT_LE_H__
