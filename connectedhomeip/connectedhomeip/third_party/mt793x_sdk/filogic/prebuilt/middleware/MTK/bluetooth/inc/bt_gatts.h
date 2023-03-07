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

#ifndef __BT_GATTS_H__
#define __BT_GATTS_H__

#include "bt_debug.h"
#include "bt_uuid.h"
#include "bt_gattc.h"
#include "bt_gatt.h"

BT_EXTERN_C_BEGIN
/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothGATT GATT
 * @{
 * @addtogroup BluetoothBLE_GATTS GATT Server
 * @{
 * This section introduces the GATT server's database including terms and acronyms, supported features, details on how to use
 * this driver, GATT server's function groups, enums, structures and functions.
 * GATT server's database is divided into three levels, from the top to the bottom, the server, the services and the records.
 * A server can have more than one service.
 * A service can have more than one record with a type of #bt_gatts_service_rec_t.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b GATTS                      | GATT Server, is the device that accepts incoming commands and requests from the GATT client. |
 * |\b ACL                        | Asynchronous Connectionless Link. For more information, please refer to B. A. Forouzan, Data Communications and Networking, Fourth Edition. Wireless LANs, Chapter 14, p. 439, McGraw-Hill Professional, 2007. |
 * |\b MTU                        | Maximum Transmission Unit, specifies the maximum size of a packet to send. |
 *
 * @section bt_gatts_api_usage How to use this module
 *  GATTS is composed of #bt_gatts_service_rec_t.
 *
 * In this example, a server is created with two services bt_if_gap_service and bt_if_gatt_service.
 *  - bt_if_gap_service has 4 records.
 *  - bt_if_gatt_service has 5 records.
 * |server|service|handle|record|
 * |:---:|:------:|:------:|:--------|
 * |bt_if_gatt_server|bt_if_gap_service|0x0001|bt_if_gap_primary_service|
 * |||0x0002|bt_if_gap_included_manufacturer_service|
 * |||0x0003|bt_if_gap_char4_dev_name|
 * |||0x0004|bt_if_gap_dev_name|
 * ||bt_if_gatt_service|0x0011|bt_if_gatt_primary_service|
 * |||0x0012|bt_if_gatt_char4_service_changed|
 * |||0x0013|bt_if_gatt_service_changed|
 * |||0x0014|bt_if_gatt_client_config|
 * |||0x0015|bt_if_gatt_server_config|
 *
 * ## Sample code
 * - Step 1. Create a GAP service with the bt_if_gap_service and related records.
 *      + Each record is placed in an array of bt_if_gap_service_rec.
 *      + The value of the starting_handle is assigned to the first record, bt_if_gap_primary_service.
 *      + The value of the ending_handle is assigned to the last record, bt_if_gap_dev_name.
 *      + The handles of the records between the first and the last are arranged sequentially.
 *      + (ending_handle - starting_handle + 1) should be equal to the number of records.
 *      + required_encryption_key_size specifies the encryption key size required by this service. If the record's permission was set with #BT_GATTS_REC_PERM_READABLE_ENCRYPTION or #BT_GATTS_REC_PERM_WRITABLE_ENCRYPTION, GATTS will check if the channel is encrypted and the encryption key size is greater than or equal to required_encryption_key_size.
 * @code
 * // Create a record.
 * BT_GATTS_NEW_PRIMARY_SERVICE_16(bt_if_gap_primary_service, BT_GATT_UUID16_GAP_SERVICE);
 * BT_GATTS_NEW_INCLUDED_SERVICE_128(bt_if_gap_included_manufacturer_service, 0x0500, 0x0522);
 * BT_GATTS_NEW_CHARC_16_WRITABLE(bt_if_gap_char4_dev_name, BT_GATT_CHARC_PROP_READ, 0x0004, BT_SIG_UUID16_DEVICE_NAME);
 * BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_gap_dev_name, BT_SIG_UUID_DEVICE_NAME,
 *              BT_GATTS_REC_PERM_READABLE|BT_GATTS_REC_PERM_WRITABLE, bt_if_gap_dev_name_callback);
 * static const bt_gatts_service_rec_t *bt_if_gap_service_rec[] = { //Collect all records
 *  (const bt_gatts_service_rec_t*) &bt_if_gap_primary_service,
 *  (const bt_gatts_service_rec_t*) &bt_if_gap_included_manufacturer_service,
 *  (const bt_gatts_service_rec_t*) &bt_if_gap_char4_dev_name,
 *  (const bt_gatts_service_rec_t*) &bt_if_gap_dev_name
 *  };
 * static const bt_gatts_service_t bt_if_gap_service = {
 *  .starting_handle = 0x0001, // Starting handle.
 *  .ending_handle = 0x0004,  // Ending handle.
 *  .required_encryption_key_size = 7, // If the record needs a channel encryption, the encryption key size should be greater than or equal to 7.
 *  .records = bt_if_gap_service_rec
 *  };
 *
 * @endcode
 *
 * - Step 2. Create a GATT service with the bt_if_gatt_service and related records.
 * @code
 * // Create each record.
 * BT_GATTS_NEW_PRIMARY_SERVICE_16(bt_if_gatt_primary_service, BT_GATT_UUID16_GATT_SERVICE);
 * BT_GATTS_NEW_CHARC_16(bt_if_gatt_char4_service_changed, BT_GATT_CHARC_PROP_READ|BT_GATT_CHARC_PROP_NOTIFY|BT_GATT_CHARC_PROP_INDICATE,
                0x0013, BT_SIG_UUID16_SERVICE_CHANGED);
 * BT_GATTS_NEW_CHARC_VALUE_UINT32_WRITABLE(bt_if_gatt_service_changed, BT_SIG_UUID_SERVICE_CHANGED,
                             0x2, 0x0001050F);
 * BT_GATTS_NEW_CLIENT_CHARC_CONFIG(bt_if_gatt_client_config,
 *       BT_GATTS_REC_PERM_READABLE|BT_GATTS_REC_PERM_WRITABLE,
 *       bt_if_gatt_client_config_callback);
 * BT_GATTS_NEW_SERVER_CHARC_CONFIG(bt_if_gatt_server_config,
 *       BT_GATTS_REC_PERM_READABLE|BT_GATTS_REC_PERM_WRITABLE,
 *       bt_if_gatt_server_config_callback);
 *
 * static const bt_gatts_service_rec_t *bt_if_gatt_service_rec[] = { // Collect all records.
 *   (const bt_gatts_service_rec_t*) &bt_if_gatt_primary_service,
 *   (const bt_gatts_service_rec_t*) &bt_if_gatt_char4_service_changed,
 *   (const bt_gatts_service_rec_t*) &bt_if_gatt_service_changed,
 *   (const bt_gatts_service_rec_t*) &bt_if_gatt_client_config,
 *   (const bt_gatts_service_rec_t*) &bt_if_gatt_server_config
 *   };
 *
 * static const bt_gatts_service_t bt_if_gatt_service = {
 *   .starting_handle = 0x0011, // Starting handle.
 *   .ending_handle = 0x0015,  // Ending handle.
 *   .required_encryption_key_size = 9, // If the record needs channel encryption, the size of encryption key should be greater than or equal to 9.
 *   .records = bt_if_gatt_service_rec
 *   };
 *
 * @endcode
 *
 * - Step 3. Create a bt_if_gatt_server and collect all services.
 *      + The server provides all services to GATTS using #bt_gatts_get_server_by_handle().
 * @code
 * // The server collects all services.
 * const bt_gatts_service_t * bt_if_gatt_server[] = {
 *  &bt_if_gap_service,
 *  &bt_if_gatt_service_ro,
 *  NULL
 *  };
 *
 * @endcode
 *
 */

/**
 * @addtogroup BluetoothBLE_GATTS_define Define
 * @{
 * This section defines the macros for the GATTS.
 */

#define BT_GATTS_EXCHANGE_MTU_INDICATION                (BT_MODULE_GATT | 0x00000100) /**< This is the indication event, generated when there is mtu exchange request from fgatt client, with #bt_gatts_exchange_mtu_ind_t as the payload. */

/**
 * Note, when creating a record, the application should synchronize the record's permission and characteristic properties to be read out by the client. When the client requests to read or write the record, the Bluetooth stack checks this permission.
 */
#define BT_GATTS_REC_PERM_READ_SHIFT(mask)               ((mask)<<4) /**< Read the permission mask shift. */
#define BT_GATTS_REC_PERM_WRITE_SHIFT(mask)              ((mask)<<0) /**< Write the permission mask shift. */
#define BT_GATTS_REC_PERM_READ_MASK                      0xf0 /**< Read the permission mask. */
#define BT_GATTS_REC_PERM_READABLE                       0x80 /**< The record is readable without any security requirement. */
#define BT_GATTS_REC_PERM_READABLE_ENCRYPTION            (BT_GATTS_REC_PERM_READABLE | (BT_GATTS_REC_PERM_READ_SHIFT(BT_GAP_LE_SECURITY_ENCRYPTION_MASK)))/**< The record is readable when the connection is encrypted. */
#define BT_GATTS_REC_PERM_READABLE_AUTHENTICATION        (BT_GATTS_REC_PERM_READABLE | (BT_GATTS_REC_PERM_READ_SHIFT(BT_GAP_LE_SECURITY_AUTHENTICATION_MASK)))/**< The record is readable only for the authenticated connection. */
#define BT_GATTS_REC_PERM_READABLE_AUTHORIZATION         (BT_GATTS_REC_PERM_READABLE | (BT_GATTS_REC_PERM_READ_SHIFT(BT_GAP_LE_SECURITY_AUTHORIZATION_MASK)))/**< The record is readable only after the application is granted a permission. */
#define BT_GATTS_IS_READABLE(perm)                       (((perm) & BT_GATTS_REC_PERM_READ_MASK)> 0) /**< Checks if the record is readable. */
#define BT_GATTS_REC_PERM_WRITE_MASK                     0x0f /**< Write the permission mask. */
#define BT_GATTS_REC_PERM_WRITABLE                       0x08/**< The record is writable without any security requirement. */
#define BT_GATTS_REC_PERM_WRITABLE_ENCRYPTION            (BT_GATTS_REC_PERM_WRITABLE | (BT_GATTS_REC_PERM_WRITE_SHIFT(BT_GAP_LE_SECURITY_ENCRYPTION_MASK)))/**< The record is writable when the connection is encrypted. */
#define BT_GATTS_REC_PERM_WRITABLE_AUTHENTICATION        (BT_GATTS_REC_PERM_WRITABLE | (BT_GATTS_REC_PERM_WRITE_SHIFT(BT_GAP_LE_SECURITY_AUTHENTICATION_MASK)))/**< The record is writable only for the authenticated connection. */
#define BT_GATTS_REC_PERM_WRITABLE_AUTHORIZATION         (BT_GATTS_REC_PERM_WRITABLE | (BT_GATTS_REC_PERM_WRITE_SHIFT(BT_GAP_LE_SECURITY_AUTHORIZATION_MASK)))/**< The record is writable only after the application is granted a permission. */
#define BT_GATTS_IS_WRITABLE(perm)                       (((perm) & BT_GATTS_REC_PERM_WRITE_MASK)> 0) /**< Checks if the record is writable. */
/* #bt_atts_rec_perm_t format:
 * 7  6             5                 4                3  2             1                 0
 * [R][R_Encryption][R_Authentication][R_Authorization][W][W_Encryption][W_Authentication][W_Authorization] */
#define BT_GATTS_CALLBACK_READ                          0 /**< It is used in the record's callback #bt_gatts_rec_callback_t() to notify a read request. */
#define BT_GATTS_CALLBACK_WRITE                         1 /**< It is used in the record's callback #bt_gatts_rec_callback_t() to notify a write request. */
#define BT_GATTS_CALLBACK_PREPARE_WRITE                 2 /**< It is used in the record's callback #bt_gatts_rec_callback_t() to notify a prepare write request. */
/* Please don't change sequence of #BT_GATTS_CALLBACK_EXECUTE_WRITE_CANCEL & #BT_GATTS_CALLBACK_EXECUTE_WRITE_WRITE */
#define BT_GATTS_CALLBACK_EXECUTE_WRITE_CANCEL          3 /**< It is used in the record's callback #bt_gatts_rec_callback_t() to notify an execute write with action cancel request. */
#define BT_GATTS_CALLBACK_EXECUTE_WRITE_WRITE           4 /**< It is used in the record's callback #bt_gatts_rec_callback_t() to notify an execute write with action write request. */
/*
*  HB_BUG_FIX start
*  [Issue Description]
*      Original design does not differentiate the write request (with response) and write command (without response) in the callback function.
*      It may be an issue if application would check the operation validation based on the attribute property by itself and reject the write command.
*      Add BT_GATTS_CALLBACK_WRITE_COMMAND to differentiate write command from write request
*/
#define BT_GATTS_CALLBACK_WRITE_COMMAND                 5 /**< It is used in the record's callback #bt_gatts_rec_callback_t() to notify a write command without response. Only available when HB open __BT_GATTS_EH__*/
/* HB_BUG_FIX end */

/* -------------Execute write status------------- */
/* The status should be one of BT_ATT_ERRCODE */
#define BT_GATTS_EXECUTE_WRITE_RLT_SET(handle, status)    (((handle)<<8)|(status)) /**< It is used to create a response to the execute write operation. */
#define BT_GATTS_EXECUTE_WRITE_RLT_GET_HANDLE(s)          (((s)>>8)&0xFFFF) /**< It is used to get handle of the execute write response. */
#define BT_GATTS_EXECUTE_WRITE_RLT_GET_STATUS(s)          ((s)&0xFF) /**< It is used to get status of the execute write response. */

#define BT_GATTS_ASYNC_RESPONSE  (0xFFFFFFFF) /**<It is used to indicate an asynchronous response to the write operation .*/
/**
 * @}
 */
typedef uint8_t bt_atts_rec_perm_t; /**< Bt_atts_rec_perm_t is a member of #bt_gatts_service_rec_t. It is used to store the record's read or write security permission. */

/* --------------Extern UUID --------------------- */
extern const bt_uuid_t BT_GATT_UUID_PRIMARY_SERVICE;            /**< UUID for GATT primary service. */
extern const bt_uuid_t BT_GATT_UUID_SECONDARY_SERVICE;          /**< UUID for GATT secondary primary service. */
extern const bt_uuid_t BT_GATT_UUID_INCLUDED_SERVICE;           /**< UUID for GATT included service. */
extern const bt_uuid_t BT_GATT_UUID_CHARC;                      /**< UUID for GATT characteristic. */
extern const bt_uuid_t BT_GATT_UUID_CHARC_EXTENDED_PROPERTIES;  /**< UUID for GATT characteristic extended properties. */
extern const bt_uuid_t BT_GATT_UUID_CHARC_USER_DESCRIPTION;     /**< UUID for GATT characteristic user description. */
extern const bt_uuid_t BT_GATT_UUID_CLIENT_CHARC_CONFIG;        /**< UUID for GATT client characteristic configuration. */
extern const bt_uuid_t BT_GATT_UUID_SERVER_CHARC_CONFIG;        /**< UUID for GATT server characteristic configuration. */
extern const bt_uuid_t BT_GATT_UUID_CHARC_FORMAT;               /**< UUID for GATT characteristic presentation format. */
extern const bt_uuid_t BT_GATT_UUID_CHARC_AGGREGATE_FORMAT;     /**< UUID for GATT characteristic aggregate format. */

/* -------- Callback format ------ */
/**
 * @brief This function is the record's callback called by the GATTS to read or write the record's attribute value.
 *        If GATTS calls with #BT_GATTS_CALLBACK_READ and with the size of 0, it is a request to query the length of the attribute value.
 * @param[in] rw     is #BT_GATTS_CALLBACK_READ, #BT_GATTS_CALLBACK_WRITE, #BT_GATTS_CALLBACK_PREPARE_WRITE, #BT_GATTS_CALLBACK_EXECUTE_WRITE_CANCEL or #BT_GATTS_CALLBACK_EXECUTE_WRITE_WRITE.
 * @param[in] handle is the ACL connection handle.
 * @param[in] data   is the data buffer.
 * @param[in] size   is the number of bytes in the data buffer.
 * @param[in] offset is the offset of the attribute value. The read or write starts from the start address of attribute value plus the offset.
 * @return
 *      * For #BT_GATTS_CALLBACK_READ, #BT_GATTS_CALLBACK_WRITE and #BT_GATTS_CALLBACK_PREPARE_WRITE operation, the callback returns the length of the data read or written in the operation.
 *      * For #BT_GATTS_CALLBACK_EXECUTE_WRITE_CANCEL and #BT_GATTS_CALLBACK_EXECUTE_WRITE_WRITE operation, the callback returns the status of the execute write.
 */
typedef uint32_t (*bt_gatts_rec_callback_t)(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);
/**
 * @addtogroup BluetoothBLE_GATTS_struct Struct
 * @{
 * This section defines the basic data structures for the GATTS.
 */

/**
*  @brief This structure defines the data type of the callback parameter for the event (#BT_GATTS_EXCHANGE_MTU_INDICATION) which indicates a exchange MTU request from gatt client.
*/
typedef struct {
    bt_handle_t connection_handle; /**< The connection handle. */
    uint16_t server_mtu;           /**< The negotiated server MTU size. */
} bt_gatts_exchange_mtu_ind_t;

/**
 *  @brief Record structure of the GATTS.
 */
typedef struct {
    const bt_uuid_t *uuid_ptr; /**< A pointer to the UUID of attribute type. */
    bt_atts_rec_perm_t perm;   /**< The read or write permission and security requirement of the attribute, #BT_GATTS_REC_PERM_READABLE, #BT_GATTS_REC_PERM_WRITABLE. */
    uint8_t value_len;         /**< The length of the attribute value. If the value_len is 0, the record has a callback #bt_gatts_rec_callback_t(), followed by this structure. GATTS will call the record's callback to read or write the attribute value.*/
} bt_gatts_service_rec_t;

/* -------- Service ---------------------------- */
/**
 *  @brief Service structure of the GATTS.
 */
typedef struct {
    uint16_t starting_handle;                /**< The service's starting handle. */
    uint16_t ending_handle;                  /**< The service's ending handle. */
    uint8_t  required_encryption_key_size;   /**< The required encryption key size. */
    const bt_gatts_service_rec_t **records;  /**< A pointer to the records in the service. */
} bt_gatts_service_t;

/**
 *  @brief Request the GATTS to do the execute write.
 */
typedef struct {
    uint16_t handle;  /**< The connection handle. */
    uint8_t flag;     /**< The flag indicating whether to execute write operation. */
} bt_gatts_execute_write_req_t;

/**
 *  @brief Request the GATTS to do an authorization check.
 */
BT_PACKED(
typedef struct {
    uint8_t  read_write;        /**< #BT_GATTS_CALLBACK_READ or #BT_GATTS_CALLBACK_WRITE. */
    uint16_t attribute_handle;  /**< The attribute handle. */
    uint16_t connection_handle; /**< The connection handle. */
}) bt_gatts_authorization_check_req_t;

/**
*  @brief Included service value structure of the GATTS.
*/
BT_PACKED(
typedef struct {
    uint16_t service_handle;    /**< The starting handle of the included service. */
    uint16_t end_group_handle;  /**< The ending handle of the included service. */
    uint16_t uuid16;            /**< If the UUID16 is 0x0000, it is the vendor's private UUID. */
}) bt_gatts_included_service_value_t;

/**
*  @brief Characteristic UUID16 value structure of the GATTS.
*/
BT_PACKED(
typedef struct {
    uint8_t properties;     /**< The property of the characteristic value. */
    uint16_t handle;        /**< The handle of the characteristic value. */
    uint16_t uuid16;        /**< The UUID16 value of the characteristic. */
}) bt_gatts_characteristic_uuid16_value_t;

/**
*  @brief Characteristic UUID128 value structure of the GATTS.
*/
BT_PACKED(
typedef struct {
    uint8_t properties;     /**< The property of the characteristic value. */
    uint16_t handle;        /**< The handle of the characteristic value. */
    bt_uuid_t uuid128;      /**< The UUID128 value of the characteristic. */
}) bt_gatts_characteristic_uuid128_value_t;

/**
*  @brief Characteristic value structure of the GATTS.
*/
BT_PACKED(
typedef struct {
    BT_PACKED(
    union{
        /* Other type will use callback instead. */
        uint8_t  value_uint_8;              /**< The uint8 value of the characteristic.*/
        uint16_t value_uint_16;             /**< The uint16 value of the characteristic.*/
        uint32_t value_uint_32;             /**< The uint32 value of the characteristic.*/
        int8_t  value_int_8;                /**< The int8 value of the characteristic.*/
        int16_t value_int_16;               /**< The int16 value of the characteristic.*/
        int32_t value_int_32;               /**< The int32 value of the characteristic.*/
        bt_gatts_rec_callback_t callback;   /**< If the length rec_hdr.value_len is 0, the callback should handle the read and write requests. */
    });
}) bt_gatts_characteristic_value_t;

/**
*  @brief Characteristic format value structure of the GATTS.
*/
BT_PACKED(
typedef struct {
    uint8_t format;         /**< The format of the characteristic value.*/
    uint8_t exponent;       /**< The exponent to determine how the value of the characteristic is further formatted.*/
    uint16_t unit;          /**< The unit of the  characteristic.*/
    uint8_t name_space;     /**< The namespace of the characteristic.*/
    uint16_t description;   /**< The description of the characteristic.*/
}) bt_gatts_characteristic_format_value_t;

/**
*  @brief 16-bit primary service structure of the GATTS.
*/
typedef struct {
    // Permission: #BT_GATTS_REC_PERM_READABLE
    bt_gatts_service_rec_t rec_hdr;     /**< The record, which is the basic element of a service or characteristic. */
    uint16_t uuid16;                    /**< The 16-bit primary service UUID. */
} bt_gatts_primary_service_16_t;

/**
*  @brief 16-bit secondary service structure of the GATTS.
*/
typedef bt_gatts_primary_service_16_t bt_gatts_secondary_service_16_t;

/**
*  @brief 128-bit primary service structure of the GATTS.
*/
typedef struct {
    // Permission: #BT_GATTS_REC_PERM_READABLE
    bt_gatts_service_rec_t rec_hdr;     /**< The record, which is the basic element of a service or characteristic. */
    bt_uuid_t uuid128;                  /**< The 128-bit primary service UUID. */
} bt_gatts_primary_service_128_t;

/**
*  @brief 128-bit secondary service structure of the GATTS.
*/
typedef bt_gatts_primary_service_128_t bt_gatts_secondary_service_128_t;

/**
*  @brief Included service structure of the GATTS.
*/
typedef struct {
    // Permission: #BT_GATTS_REC_PERM_READABLE
    bt_gatts_service_rec_t rec_hdr;     /**< The record, which is the basic element of a service or characteristic. */
    bt_gatts_included_service_value_t value;     /**< The value of the included service. */
} bt_gatts_included_service_t;

/**
*  @brief 16-bit UUID characteristic structure of the GATTS.
*/
typedef struct {
    // Permission: #BT_GATTS_REC_PERM_READABLE
    bt_gatts_service_rec_t rec_hdr;     /**< The record, which is the basic element of a service or characteristic. */
    bt_gatts_characteristic_uuid16_value_t value;   /**< The value of the characteristic. */
} bt_gatts_characteristic_16_t;

/**
*  @brief 128-bit UUID characteristic structure of the GATTS.
*/
typedef struct {
    // Permission: #BT_GATTS_REC_PERM_READABLE
    bt_gatts_service_rec_t rec_hdr;     /**< The record, which is the basic element of a service or characteristic. */
    bt_gatts_characteristic_uuid128_value_t value;  /**< The value of the characteristic. */
} bt_gatts_characteristic_128_t;

/**
*  @brief Characteristic structure of the GATTS.
*/
typedef struct {
    // Permission: Application define
    bt_gatts_service_rec_t rec_hdr;     /**< The record, which is the basic element of a service or characteristic. */
    bt_gatts_characteristic_value_t value;  /**< The value of the characteristic. */
} bt_gatts_characteristic_t;

/**
*  @brief utf-16 characteristic structure of the GATTS.
*/
typedef struct {
    // Permission: Application define
    bt_gatts_service_rec_t rec_hdr;     /**< The record, which is the basic element of a service or characteristic. */
    uint8_t str[16];                    /**< The value of the characteristic. */
} bt_gatts_characteristic_str16_t;

/**
*  @brief Half word characteristic structure of the GATTS.
*/
typedef struct {
    // Permission: Application define
    bt_gatts_service_rec_t rec_hdr;     /**< The record, which is the basic element of a service or characteristic. */
    uint16_t halfw[8];                  /**< The value of the characteristic. */
} bt_gatts_characteristic_halfw8_t;

/**
*  @brief Characteristic extended properties descriptor structure of the GATTS.
*/
typedef struct {
    // Permission: #BT_GATTS_REC_PERM_READABLE
    bt_gatts_service_rec_t rec_hdr;     /**< The record, which is the basic element of a service or characteristic. */
    uint16_t extended_properties;       /**< The extended properties of the characteristic descriptor. */
} bt_gatts_characteristic_extended_properties_t;

/**
*  @brief Characteristic user description descriptor structure of the GATTS.
*/
typedef struct {
    // Permission: Application define
    bt_gatts_service_rec_t rec_hdr;     /**< The record, which is the basic element of a service or characteristic. */
    bt_gatts_rec_callback_t callback;   /**< When the rec_hdr.value_len is 0, the callback should handle the read and write request. */
} bt_gatts_characteristic_user_description_t;

/**
*  @brief Characteristic user description str16 descriptor structure of the GATTS.
*/
typedef bt_gatts_characteristic_str16_t bt_gatts_characteristic_user_description_str16_t;

/**
*  @brief Client characteristic configuration descriptor structure of the GATTS.
*/
typedef struct {
    // Permission: #BT_GATTS_REC_PERM_READABLE | Application define write.
    bt_gatts_service_rec_t rec_hdr;     /**< The record, which is the basic element of a service or characteristic. */
    bt_gatts_rec_callback_t callback;   /**< If the rec_hdr.value_len is 0, the callback should handle the read and write request. */
} bt_gatts_client_characteristic_config_t;

/**
*  @brief Server characteristic configuration descriptor structure of the GATTS.
*/
typedef bt_gatts_client_characteristic_config_t bt_gatts_server_characteristic_config_t;

/**
*  @brief Characteristic presentation format descriptor structure of the GATTS.
*/
typedef struct {
    // Permission: #BT_GATTS_REC_PERM_READABLE
    bt_gatts_service_rec_t rec_hdr;     /**< The record, which is the basic element of a service or characteristic. */
    bt_gatts_characteristic_format_value_t value;   /**< The format value of the descriptor. */
} bt_gatts_characteristic_format_t;

/**
*  @brief Characteristic aggregate format descriptor structure of the GATTS.
*/
typedef struct {
    // Permission: #BT_GATTS_REC_PERM_READABLE
    bt_gatts_service_rec_t rec_hdr;     /**< The record, which is the basic element of a service or characteristic. */
    uint16_t handle_list[8];            /**< The handle list of the characteristic aggregate format descriptor. */
} bt_gatts_characteristic_aggregate_format_t;

/**
*  @brief This structure defines the data type of the callback parameter for the events (#BT_GATTS_CALLBACK_WRITE and #BT_GATTS_CALLBACK_READ) which call #bt_gatts_send_response() to send a response.
*/
typedef struct {
    uint16_t attribute_handle;              /**< Attribute handle. */
} bt_gatts_error_rsp_t;

/**
*  @brief This structure defines the data type of the callback parameter for the event (#BT_GATTS_CALLBACK_READ) which call #bt_gatts_send_response() to send a response.
*/
typedef struct {
    uint8_t  *value;                        /**< The value of read response. */
    uint16_t length;                        /**< The value length of read response. */
    //uint16_t offset;                        /**< The value offset of read response. */
} bt_gatts_read_rsp_t;

#ifdef __BT_GATTS_EH__
/**
*  @brief For adaption layer, define possible structure union of the GATTS.
*/
BT_PACKED(
typedef struct {
    BT_PACKED(
    union{
        bt_gatts_primary_service_16_t                  primary_value_16;
        bt_gatts_primary_service_128_t                 primary_value_128;
        bt_gatts_included_service_t                    included_service;
        bt_gatts_characteristic_16_t                   char_value_16;
        bt_gatts_characteristic_128_t                  char_value_128;
        bt_gatts_characteristic_t                      char_value;
        bt_gatts_characteristic_extended_properties_t  desc_ext_prop;
        bt_gatts_characteristic_user_description_t     desc_user_desc;
        bt_gatts_client_characteristic_config_t        desc_cli_conf;
        bt_gatts_server_characteristic_config_t        desc_ser_conf;
        bt_gatts_characteristic_format_t               desc_present_fmt;
        bt_gatts_characteristic_aggregate_format_t     desc_aggr_fmt;
    });
}) bt_gatts_attr_definition_t;
#endif

/**
* @}
*/

/**
* @addtogroup BluetoothBLE_GATTS_define Define
* @{
* This section describes the macros defined for the GATTS.
*/

/**
* @brief This macro creates a 16-bit primary service.
* @param[in] name               is the name of the record.
* @param[in] uuid16_value       is the 16-bit primary service UUID.
*/
#define BT_GATTS_NEW_PRIMARY_SERVICE_16(name, uuid16_value)          \
    static const bt_gatts_primary_service_16_t name = {     \
    .rec_hdr.uuid_ptr = &BT_GATT_UUID_PRIMARY_SERVICE,      \
    .rec_hdr.perm = BT_GATTS_REC_PERM_READABLE,              \
    .rec_hdr.value_len = 2,                                 \
    .uuid16 = uuid16_value                                   \
    }

/**
* @brief This macro creates a 128-bit primary service.
* @param[in] name               is the name of the record.
* @param[in] uuid128_value      is the 128-bit primary service UUID.
*/
#define BT_GATTS_NEW_PRIMARY_SERVICE_128(name, uuid128_value)        \
    static const bt_gatts_primary_service_128_t name = {     \
    .rec_hdr.uuid_ptr = &BT_GATT_UUID_PRIMARY_SERVICE,      \
    .rec_hdr.perm = BT_GATTS_REC_PERM_READABLE,      \
    .rec_hdr.value_len = 16,                                \
    .uuid128 = uuid128_value                                 \
    }

/**
* @brief This macro creates a 16-bit secondary service.
* @param[in] name               is the name of the record.
* @param[in] uuid16_value       is the 16-bit secondary service UUID.
*/
#define BT_GATTS_NEW_SECONDARY_SERVICE_16(name, uuid16_value)        \
    static const bt_gatts_secondary_service_16_t name = {   \
    .rec_hdr.uuid_ptr = &BT_GATT_UUID_SECONDARY_SERVICE,    \
    .rec_hdr.perm = BT_GATTS_REC_PERM_READABLE,      \
    .rec_hdr.value_len = 2,                                 \
    .uuid16 = uuid16_value                                   \
    }

/**
* @brief This macro creates a 128-bit secondary service.
* @param[in] name               is the name of the record.
* @param[in] uuid128_value      is the 128-bit secondary service UUID.
*/
#define BT_GATTS_NEW_SECONDARY_SERVICE_128(name, uuid128_value)      \
    static const bt_gatts_secondary_service_128_t name = {   \
    .rec_hdr.uuid_ptr = &BT_GATT_UUID_SECONDARY_SERVICE,    \
    .rec_hdr.perm = BT_GATTS_REC_PERM_READABLE,      \
    .rec_hdr.value_len = 16,                                \
    .uuid128 = uuid128_value                                \
    }

/**
* @brief This macro creates a 16-bit included service.
* @param[in] name               is the name of the record.
* @param[in] s_handle           is the starting handle of the included service.
* @param[in] e_handle           is the ending handle of the included service.
* @param[in] uuid               is the 16-bit included service UUID.
*/
#define BT_GATTS_NEW_INCLUDED_SERVICE_16(name, s_handle, e_handle, uuid)  \
    static const bt_gatts_included_service_t name = {                \
    .rec_hdr.uuid_ptr = &BT_GATT_UUID_INCLUDED_SERVICE,     \
    .rec_hdr.perm = BT_GATTS_REC_PERM_READABLE,             \
    .rec_hdr.value_len = 6,                                 \
    .value.service_handle = s_handle,                       \
    .value.end_group_handle = e_handle,                     \
    .value.uuid16 = uuid                                    \
    }

/**
* @brief This macro creates a 128-bit included service.
* @param[in] name               is the name of the record.
* @param[in] s_handle           is the starting handle of the included service.
* @param[in] e_handle           is the ending handle of the included service.
*/
#define BT_GATTS_NEW_INCLUDED_SERVICE_128(name, s_handle, e_handle)       \
    static const bt_gatts_included_service_t name = {                \
    .rec_hdr.uuid_ptr = &BT_GATT_UUID_INCLUDED_SERVICE,     \
    .rec_hdr.perm = BT_GATTS_REC_PERM_READABLE,             \
    .rec_hdr.value_len = 4,                                 \
    .value.service_handle = s_handle,                       \
    .value.end_group_handle = e_handle,                     \
    .value.uuid16 =0x0000                                   \
    }

/**
* @brief This macro creates a 16-bit readable characteristic.
* @param[in] name              is the name of the record.
* @param[in] prop              specifies the properties of the characteristic.
* @param[in] _handle           is the handle of the characteristic value.
* @param[in] uuid              is the 16-bit characteristic UUID.
*/
#define BT_GATTS_NEW_CHARC_16(name, prop, _handle, uuid)     \
    static const bt_gatts_characteristic_16_t name = {       \
    .rec_hdr.uuid_ptr = &BT_GATT_UUID_CHARC,                 \
    .rec_hdr.perm = BT_GATTS_REC_PERM_READABLE,              \
    .rec_hdr.value_len = 5,                                  \
    .value.properties = prop,                                \
    .value.handle = _handle,                                 \
    .value.uuid16 = uuid                                     \
    }

/**
* @brief This macro creates a 16-bit writable characteristic.
* @param[in] name              is the name of the record.
* @param[in] prop              specifies the properties of the characteristic.
* @param[in] _handle           is the handle of the characteristic value.
* @param[in] uuid              is the 16-bit characteristic UUID.
*/
#define BT_GATTS_NEW_CHARC_16_WRITABLE(name, prop, _handle, uuid)     \
    static bt_gatts_characteristic_16_t name = {            \
    .rec_hdr.uuid_ptr = &BT_GATT_UUID_CHARC,                \
    .rec_hdr.perm = BT_GATTS_REC_PERM_READABLE,             \
    .rec_hdr.value_len = 5,                                 \
    .value.properties = prop,                               \
    .value.handle = _handle,                                \
    .value.uuid16 = uuid                                    \
    }

/**
* @brief This macro creates a 128-bit readable characteristic.
* @param[in] name              is the name of the record.
* @param[in] prop              specifies the properties of the characteristic.
* @param[in] _handle           is the handle of the characteristic value.
* @param[in] uuid              is the 128-bit characteristic UUID.
*/
#define BT_GATTS_NEW_CHARC_128(name, prop, _handle, uuid)    \
    static const bt_gatts_characteristic_128_t name = {     \
    .rec_hdr.uuid_ptr = &BT_GATT_UUID_CHARC,       \
    .rec_hdr.perm = BT_GATTS_REC_PERM_READABLE,      \
    .rec_hdr.value_len = 19,                                \
    .value.properties = prop,                               \
    .value.handle = _handle,                                 \
    .value.uuid128 = uuid                                   \
    }

/**
* @brief This macro creates a const uint8 characteristic value.
* @param[in] name              is the name of the record.
* @param[in] uuid              is the UUID of the characteristic value.
* @param[in] _perm             is the permission of the characteristic value.
* @param[in] _value            is uint8 characteristic value.
*/
#define BT_GATTS_NEW_CHARC_VALUE_UINT8(name, uuid, _perm, _value)     \
    static const bt_gatts_characteristic_t name = {                 \
    .rec_hdr.uuid_ptr = &uuid,                                      \
    .rec_hdr.perm = _perm,                                           \
    .rec_hdr.value_len = 1,                                         \
    .value.value_uint_8 = _value                                     \
    }

/**
* @brief This macro creates a uint8 characteristic value.
* @param[in] name              is the name of the record.
* @param[in] uuid              is the UUID of the characteristic value.
* @param[in] _perm             is the permission of the characteristic value.
* @param[in] _value            is uint8 characteristic value.
*/
#define BT_GATTS_NEW_CHARC_VALUE_UINT8_WRITABLE(name, uuid, _perm, _value)     \
    static bt_gatts_characteristic_t name = {                 \
    .rec_hdr.uuid_ptr = &uuid,                                      \
    .rec_hdr.perm = _perm,                                           \
    .rec_hdr.value_len = 1,                                         \
    .value.value_uint_8 = _value                                     \
    }

/**
* @brief This macro creates a const uint16 characteristic value.
* @param[in] name              is the name of the record.
* @param[in] uuid              is the UUID of the characteristic value.
* @param[in] _perm             is the permission of the characteristic value.
* @param[in] _value            is uint16 characteristic value.
*/
#define BT_GATTS_NEW_CHARC_VALUE_UINT16(name, uuid, _perm, _value)    \
    static const bt_gatts_characteristic_t name = {                 \
    .rec_hdr.uuid_ptr = &uuid,                                      \
    .rec_hdr.perm = _perm,                                           \
    .rec_hdr.value_len = 2,                                         \
    .value.value_uint_16 = _value                                    \
    }

/**
* @brief This macro creates a const uint32 characteristic value.
* @param[in] name              is the name of the record.
* @param[in] uuid              is the UUID of the characteristic value.
* @param[in] _perm             is the permission of the characteristic value.
* @param[in] _value            is uint32 characteristic value.
*/
#define BT_GATTS_NEW_CHARC_VALUE_UINT32(name, uuid, _perm, _value)    \
    static const bt_gatts_characteristic_t name = {                 \
    .rec_hdr.uuid_ptr = &uuid,                                      \
    .rec_hdr.perm = _perm,                                           \
    .rec_hdr.value_len = 4,                                         \
    .value.value_uint_32 = _value                                    \
    }

/**
* @brief This macro creates a uint32 characteristic value.
* @param[in] name              is the name of the record.
* @param[in] uuid              is the UUID of the characteristic value.
* @param[in] _perm             is the permission of the characteristic value.
* @param[in] _value            is uint32 characteristic value.
*/
#define BT_GATTS_NEW_CHARC_VALUE_UINT32_WRITABLE(name, uuid, _perm, _value)    \
    static bt_gatts_characteristic_t name = {                 \
    .rec_hdr.uuid_ptr = &uuid,                                      \
    .rec_hdr.perm = _perm,                                           \
    .rec_hdr.value_len = 4,                                         \
    .value.value_uint_32 = _value                                    \
    }

/**
* @brief This macro creates a const int8 characteristic value.
* @param[in] name              is the name of the record.
* @param[in] uuid              is the UUID of the characteristic value.
* @param[in] _perm             is the permission of the characteristic value.
* @param[in] _value            is int8 characteristic value.
*/
#define BT_GATTS_NEW_CHARC_VALUE_INT8(name, uuid, _perm, _value)      \
    static const bt_gatts_characteristic_t name = {                 \
    .rec_hdr.uuid_ptr = &uuid,                                      \
    .rec_hdr.perm = _perm,                                           \
    .rec_hdr.value_len = 1,                                         \
    .value.value_int_8 = _value                                      \
    }

/**
* @brief This macro creates a const int16 characteristic value.
* @param[in] name              is the name of the record.
* @param[in] uuid              is the UUID of the characteristic value.
* @param[in] _perm             is the permission of the characteristic value.
* @param[in] _value            is int16 characteristic value.
*/
#define BT_GATTS_NEW_CHARC_VALUE_INT16(name, uuid, _perm, _value)     \
    static const bt_gatts_characteristic_t name = {                 \
    .rec_hdr.uuid_ptr = &uuid,                                      \
    .rec_hdr.perm = _perm,                                           \
    .rec_hdr.value_len = 2,                                         \
    .value.value_int_16 = _value                                     \
    }

/**
* @brief This macro creates a const int32 characteristic value.
* @param[in] name              is the name of the record.
* @param[in] uuid              is the UUID of the characteristic value.
* @param[in] _perm             is the permission of the characteristic value.
* @param[in] _value            is int32 characteristic value.
*/
#define BT_GATTS_NEW_CHARC_VALUE_INT32(name, uuid, _perm, _value)     \
    static const bt_gatts_characteristic_t name = {                 \
    .rec_hdr.uuid_ptr = &uuid,                                      \
    .rec_hdr.perm = _perm,                                           \
    .rec_hdr.value_len = 4,                                         \
    .value.value_int_32 = _value                                     \
    }

/**
* @brief This macro creates a characteristic value with callback.
* @param[in] name              is the name of the record.
* @param[in] uuid              is the UUID of the characteristic value.
* @param[in] _perm             is the permission of the characteristic value.
* @param[in] call              is the callback to handle the characteristic value read and write request.
*/
#define BT_GATTS_NEW_CHARC_VALUE_CALLBACK(name, uuid, _perm, call)   \
    static const bt_gatts_characteristic_t name = {                 \
    .rec_hdr.uuid_ptr = &uuid,                                      \
    .rec_hdr.perm = _perm,                                           \
    .rec_hdr.value_len = 0,                                         \
    .value.callback = call                                          \
    }

/**
* @brief This macro creates a characteristic value with callback.
* @param[in] name              is the name of the record.
* @param[in] uuid              is the UUID of the characteristic value.
* @param[in] _perm             is the permission of the characteristic value.
* @param[in] call              is the callback to handle the characteristic value read and write request.
*/
#define BT_GATTS_NEW_CHARC_VALUE_CALLBACK_WRITABLE(name, uuid, _perm, call)   \
    static bt_gatts_characteristic_t name = {                 \
    .rec_hdr.uuid_ptr = &uuid,                                      \
    .rec_hdr.perm = _perm,                                           \
    .rec_hdr.value_len = 0,                                         \
    .value.callback = call                                          \
    }

/**
* @brief This macro creates a str16 characteristic value.
*  IMPORTANT: The length(val_len) of val_name should be less than or equal to 16.
* @param[in] name              is the name of the record.
* @param[in] uuid              is the UUID of the characteristic value.
* @param[in] _perm             is the permission of the characteristic value.
* @param[in] val_len           is the length of the characteristic value.
* @param[in] val_name          is the name of the buffer to hold the characteristic value.
*/
#define BT_GATTS_NEW_CHARC_VALUE_STR16(name, uuid, _perm, val_len, val_name)      \
    static const bt_gatts_characteristic_str16_t name = {                 \
    .rec_hdr.uuid_ptr = &uuid,                                      \
    .rec_hdr.perm = _perm,                                           \
    .rec_hdr.value_len = val_len,                                        \
    .str = val_name                                                     \
    }

/**
* @brief This macro creates a half word characteristic value.
* @param[in] name              is the name of the record.
* @param[in] uuid              is the UUID of the characteristic value.
* @param[in] _perm             is the permission of the characteristic value.
* @param[in] val_len           is the length of the characteristic value.
*/
#define BT_GATTS_NEW_CHARC_VALUE_HALFW8_WRITABLE(name, uuid, _perm, val_len,...)      \
    static bt_gatts_characteristic_halfw8_t name = {                 \
    .rec_hdr.uuid_ptr = &uuid,                                      \
    .rec_hdr.perm = _perm,                                           \
    .rec_hdr.value_len = val_len,                                        \
    .halfw =  {__VA_ARGS__}                                                     \
    }

/**
* @brief This macro creates a characteristic extended properties descriptor.
* @param[in] name              is the name of the record.
* @param[in] ext_prop          specifies the characteristic extended properties.
*/
#define BT_GATTS_NEW_CHARC_EXTENDED_PROPERTIES(name, ext_prop)          \
    static const bt_gatts_characteristic_extended_properties_t name = { \
    .rec_hdr.uuid_ptr = &BT_GATT_UUID_CHARC_EXTENDED_PROPERTIES,          \
    .rec_hdr.perm = BT_GATTS_REC_PERM_READABLE,                  \
    .rec_hdr.value_len = 2,                                             \
    .extended_properties = ext_prop                                     \
    }

/**
* @brief This macro creates a characteristic str16 user description descriptor.
*  IMPORTANT: The length(val_len) of val_name should be less than or equal to 16.
* @param[in] name              is the name of the record.
* @param[in] _perm             is the permission of the record.
* @param[in] val_len           is the length of the record.
* @param[in] val_name          is the name of the buffer to hold the record value.
*/
#define BT_GATTS_NEW_CHARC_USER_DESCRIPTION_STR16(name, _perm, val_len, val_name)      \
    static const bt_gatts_characteristic_user_description_str16_t name = {                 \
    .rec_hdr.uuid_ptr = &BT_GATT_UUID_CHARC_USER_DESCRIPTION,   \
    .rec_hdr.perm = _perm,                                           \
    .rec_hdr.value_len = val_len,                                        \
    .str = val_name                                                     \
    }

/**
* @brief This macro creates a characteristic user description descriptor.
* @param[in] name              is the name of the record.
* @param[in] _perm             is the permission of the record.
* @param[in] _callback         is the callback to handle the record read and write request.
*/
#define BT_GATTS_NEW_CHARC_USER_DESCRIPTION(name, _perm, _callback)          \
    static const bt_gatts_characteristic_user_description_t name = {       \
    .rec_hdr.uuid_ptr = &BT_GATT_UUID_CHARC_USER_DESCRIPTION,     \
    .rec_hdr.perm = _perm,                                               \
    .rec_hdr.value_len = 0,                                             \
    .callback = _callback                                                \
    }

/**
* @brief This macro creates a client characteristic configuration descriptor.
* @param[in] name              is the name of the record.
* @param[in] _perm             is the permission of the record.
* @param[in] _callback         is the callback to handle the record read and write request.
*/
#define BT_GATTS_NEW_CLIENT_CHARC_CONFIG(name, _perm, _callback)          \
    static const bt_gatts_client_characteristic_config_t name = {       \
    .rec_hdr.uuid_ptr = &BT_GATT_UUID_CLIENT_CHARC_CONFIG,     \
    .rec_hdr.perm = _perm,                                               \
    .rec_hdr.value_len = 0,                                             \
    .callback = _callback                                                \
    }

/**
* @brief This macro creates a server characteristic configuration descriptor.
* @param[in] name              is the name of the record.
* @param[in] _perm             is the permission of the record.
* @param[in] _callback         is the callback to handle the record read and write request.
*/
#define BT_GATTS_NEW_SERVER_CHARC_CONFIG(name, _perm, _callback)          \
    static const bt_gatts_server_characteristic_config_t name = {       \
    .rec_hdr.uuid_ptr = &BT_GATT_UUID_SERVER_CHARC_CONFIG,     \
    .rec_hdr.perm = _perm,                                               \
    .rec_hdr.value_len = 0,                                             \
    .callback = _callback                                                \
    }

/**
* @brief This macro creates a characteristic presentation format descriptor.
* @param[in] name                is the name of the record.
* @param[in] _format             is the format of the value of this characteristic.
* @param[in] _exponent           is the exponent field to determine how the value is further formatted.
* @param[in] _unit               is the unit of the characteristic.
* @param[in] _name_space         is the namespace of the characteristic.
* @param[in] _description        is the description of the characteristic.
*/
#define BT_GATTS_NEW_CHARC_FORMAT(name, _format, _exponent, _unit, _name_space, _description)    \
    static const bt_gatts_characteristic_format_t name = {                                  \
    .rec_hdr.uuid_ptr = &BT_GATT_UUID_CHARC_FORMAT,                                \
    .rec_hdr.perm = BT_GATTS_REC_PERM_READABLE,                                      \
    .rec_hdr.value_len = 7,                                                                 \
    .value.format = _format,                                                                 \
    .value.exponent = _exponent,                                                             \
    .value.unit = _unit,                                                                     \
    .value.name_space = _name_space,                                                         \
    .value.description = _description                                                        \
    }

/**
* @brief This macro creates a characteristic aggregate format descriptor.
*  IMPORTANT: The length(val_len) is the number of bytes, and the maximum number of handles is 8.
* @param[in] name              is the name of the record.
* @param[in] _perm             is the permission of the record.
* @param[in] val_len           is the length of the handle list in bytes.
*/
#define BT_GATTS_NEW_CHARC_AGGREGATE_FORMAT_H8(name, _perm, val_len,...)      \
    static const bt_gatts_characteristic_aggregate_format_t name = {                 \
    .rec_hdr.uuid_ptr = &BT_GATT_UUID_CHARC_AGGREGATE_FORMAT,                                      \
    .rec_hdr.perm = _perm,                                           \
    .rec_hdr.value_len = val_len,                                        \
    .handle_list = {__VA_ARGS__}                                                     \
    }

/**
 * @}
 */

/** (__MTK_COMMON__: Keep this for backward compatible)
 * @deprecated  The function is phase out, delete it,please use the bt_gatts_get_server_by_handle()
 * @brief This function is a callback API called by the GATTS to get the application's GATTS database.
 * @return       The application's GATTS database or null, if application does not provide GATTS database.
 */
const bt_gatts_service_t **bt_get_gatt_server(void);

/**
 * @brief This function is a callback API called by the GATTS to get the application's GATTS database according to a different link.
 * @param[in] connection_handle     is the connection handle.
 * @return                          The application's GATTS database or null if the application does not provide GATTS database.
 */
const bt_gatts_service_t **bt_gatts_get_server_by_handle(bt_handle_t connection_handle);

/**
 * @brief This function sends a characteristic value notification or indication.
 * @param[in] connection_handle     is the connection handle.
 * @param[in] req                   is the GATT characteristic value notification or indication structure.
 * @return                          #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                  #BT_STATUS_FAIL, the operation has failed.
 *                                  #BT_STATUS_OUT_OF_MEMORY, out of memory.
 *                                  #BT_STATUS_CONNECTION_IN_USE, an ongoing request.
 */
bt_status_t bt_gatts_send_charc_value_notification_indication(
    bt_handle_t connection_handle,
    bt_gattc_charc_value_notification_indication_t *req);

#ifdef __BT_GATTS_EH__
/**
 * @brief This function is used to find attribute handle in service
 * @param[in] serv  is address of service
 * @param[in] rec   is address of record
 * @return               the attribute handle of record in service
 */
int bt_gatts_get_service_handle(const bt_gatts_service_t *serv, const bt_gatts_service_rec_t *rec);

/**
 * @brief This function is invoked by application. It sends a read response with characteristic value set by application.
 * @param[in] conn_handle     is the connection handle.
 * @param[in] attribute_handle  is the GATT characteristic handle.
 * @param[in] data      is the data set by application. The memory is allocated and freed by application.
 * @param[in] size      is the size of the data set by application
 * @param[in] offset    is used for Read Blob response
 * @param[in] status    is the return status from application.
 * @return                          #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                  #BT_STATUS_FAIL, the operation has failed.
 *                                  #BT_STATUS_OUT_OF_MEMORY, out of memory.
 *                                  #BT_STATUS_CONNECTION_IN_USE, an ongoing request.
 */
bt_status_t bt_gatts_send_read_response(bt_handle_t conn_handle, uint16_t attribute_handle, uint8_t *data,
                uint16_t size, uint16_t offset, bt_status_t status);

/**
 * @brief This function is invoked by application. It sends a write response with characteristic value set by application.
 * @param[in] conn_handle     is the connection handle.
 * @param[in] attribute_handle  is the GATT characteristic handle.
 * @param[in] status    is the return status from application.
 * @return                          #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                  #BT_STATUS_FAIL, the operation has failed.
 *                                  #BT_STATUS_OUT_OF_MEMORY, out of memory.
 *                                  #BT_STATUS_CONNECTION_IN_USE, an ongoing request.
 */
bt_status_t bt_gatts_send_write_response(bt_handle_t conn_handle, uint16_t attribute_handle, bt_status_t status);

/**
 * @brief This function is invoked by application. It sends a prepare write response with characteristic value set by application.
 * @param[in] conn_handle     is the connection handle.
 * @param[in] attribute_handle  is the GATT characteristic handle.
 * @param[in] data      is the data set by application. The memory is allocated and freed by application.
 * @param[in] size      is the size of the data set by application
 * @param[in] offset    is used for Read Blob response
 * @param[in] status    is the return status from application.
 * @return                          #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                  #BT_STATUS_FAIL, the operation has failed.
 *                                  #BT_STATUS_OUT_OF_MEMORY, out of memory.
 *                                  #BT_STATUS_CONNECTION_IN_USE, an ongoing request.
 */
bt_status_t bt_gatts_send_prepare_write_response(bt_handle_t conn_handle, uint16_t attribute_handle, uint8_t *data,
                uint16_t size, uint16_t offset, bt_status_t status);
#endif

/**
 * @brief This function sets the maximum MTU supported by the application. This value will be used in GATTS MTU exchange response and GATTC MTU exchange request or other BT_STATUS_XXX.
 * @param[in] max_mtu               is the maximum MTU size in the range from 23 to 512 bytes.
 * @return                          If the value is accepted, the returned value is #BT_STATUS_SUCCESS; otherwise the returned value is #BT_STATUS_FAIL.
 */
bt_status_t bt_gatts_set_max_mtu(uint16_t max_mtu);

/**
* @brief  This user defined API, invoked by the GATTS process, should be implemented by the application. It is used to ask user whether to give authorization for specified attribute handle.
* @param[in] req               is the GATT authorization request to be checked by the application.
* @return                      If application accepts peer access to this attribute, the returned value is #BT_STATUS_SUCCESS; otherwise the returned value is #BT_STATUS_UNSUPPORTED.
*/
bt_status_t bt_gatts_get_authorization_check_result(bt_gatts_authorization_check_req_t *req);

/**
* @brief  This user defined API, invoked by the GATTS process, should be implemented by the application. It is used to execute the write request and check whether the execution is successful.
* @param[in] req               is the request to the GATTS to execute the write request.
* @return                      If the write request was executed successfully, the returned value is #BT_STATUS_SUCCESS; otherwise the returned value is BT_ATT_ERRCODE_XXXX.
*/
bt_status_t bt_gatts_get_execute_write_result(bt_gatts_execute_write_req_t *req);

/**
* @brief  This function sends a error response or write response for GATT write operation.
* @param[in] connection_handle  is the ACL connection handle.
 * @param[in] result  Error code.
 * @param[in] rw     is only #BT_GATTS_CALLBACK_WRITE.
 * @param[in] data   is the data buffer.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                  #BT_STATUS_FAIL, the operation has failed.
 *                                  #BT_STATUS_OUT_OF_MEMORY, out of memory.
 *                                  #BT_STATUS_CONNECTION_NOT_FOUND, wrong connection handle.
 */
bt_status_t bt_gatts_send_response(bt_handle_t connection_handle, uint8_t result, const uint8_t rw, void *data);


/** @}
* @}
* @}
*/
BT_EXTERN_C_END
#endif /*__BT_GATTS_H__*/
