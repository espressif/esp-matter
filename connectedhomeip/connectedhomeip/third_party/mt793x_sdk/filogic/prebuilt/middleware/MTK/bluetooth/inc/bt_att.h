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

#ifndef __BT_ATT_SPEC_H__
#define __BT_ATT_SPEC_H__

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothGATT GATT
 * @{
 * @addtogroup BluetoothBLE_ATT Define and structure
 * @{
 * This section introduces the ATT operation codes, request and response structures, and API prototypes
 * and definitions of the GATT UUID and characteristics.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b MTU                        | Maximum Transmission Unit, specifies the maximum size of a packet to send. |
 *
 */

#include "bt_platform.h"
#include "bt_uuid.h"

BT_EXTERN_C_BEGIN

/**
 * @defgroup BluetoothBLE_ATT_GATT_define Define
 * @{
 * This section defines the ATT opcode and error codes.
 */
#define BT_ATT_DEFAULT_MTU  23  /**< It's defined in ATT spec. */
#define BT_ATT_HANDLE_INVALID  0  /**< It's defined in ATT spec. */


#define BT_ATT_OPCODE_ERROR_RESPONSE                0x01            /**< Error response. */
#define BT_ATT_OPCODE_EXCHANGE_MTU_REQUEST          0x02            /**< Exchange MTU request. */
#define BT_ATT_OPCODE_EXCHANGE_MTU_RESPONSE         0x03            /**< Exchange MTU response. */
#define BT_ATT_OPCODE_FIND_INFORMATION_REQUEST      0x04            /**< Find information request. */
#define BT_ATT_OPCODE_FIND_INFORMATION_RESPONSE     0x05            /**< Find information response. */
#define BT_ATT_OPCODE_FIND_BY_TYPE_VALUE_REQUEST    0x06            /**< Find by type value request. */
#define BT_ATT_OPCODE_FIND_BY_TYPE_VALUE_RESPONSE   0x07            /**< Find by type value response. */
#define BT_ATT_OPCODE_READ_BY_TYPE_REQUEST          0x08            /**< Read by type request. */
#define BT_ATT_OPCODE_READ_BY_TYPE_RESPONSE         0x09            /**< Read by type response. */
#define BT_ATT_OPCODE_READ_REQUEST                  0x0A            /**< Read request. */
#define BT_ATT_OPCODE_READ_RESPONSE                 0x0B            /**< Read response. */
#define BT_ATT_OPCODE_READ_BLOB_REQUEST             0x0C            /**< Read blob request. */
#define BT_ATT_OPCODE_READ_BLOB_RESPONSE            0x0D            /**< Read blob response. */
#define BT_ATT_OPCODE_READ_MULTIPLE_REQUEST         0x0E            /**< Read multiple request. */
#define BT_ATT_OPCODE_READ_MULTIPLE_RESPONSE        0x0F            /**< Read multiple response. */
#define BT_ATT_OPCODE_READ_BY_GROUP_TYPE_REQUEST    0x10            /**< Read by group type request. */
#define BT_ATT_OPCODE_READ_BY_GROUP_TYPE_RESPONSE   0x11            /**< Read by group type response. */
#define BT_ATT_OPCODE_WRITE_REQUEST                 0x12            /**< Write request. */
#define BT_ATT_OPCODE_WRITE_RESPONSE                0x13            /**< Write response. */
#define BT_ATT_OPCODE_PREPARE_WRITE_REQUEST         0x16            /**< Prepare write request. */
#define BT_ATT_OPCODE_PREPARE_WRITE_RESPONSE        0x17            /**< Prepare write response. */
#define BT_ATT_OPCODE_EXECUTE_WRITE_REQUEST         0x18            /**< Execute write request. */
#define BT_ATT_OPCODE_EXECUTE_WRITE_RESPONSE        0x19            /**< Execute write response. */
#define BT_ATT_OPCODE_HANDLE_VALUE_NOTIFICATION     0x1B            /**< Handle value notification. */
#define BT_ATT_OPCODE_HANDLE_VALUE_INDICATION       0x1D            /**< Handle value indication. */
#define BT_ATT_OPCODE_HANDLE_VALUE_CONFIRMATION     0x1E            /**< Handle value confirmation. */
#define BT_ATT_OPCODE_READ_MULTIPLE_VARIABLE_REQ    0x20            /**< Read multiple variable request. */
#define BT_ATT_OPCODE_READ_MULTIPLE_VARIABLE_RSP    0x21            /**< Read multiple variable response. */
#define BT_ATT_OPCODE_MULTIPLE_HANDLE_VALUE_NTF     0x23            /**< Handle Length Value Tuple List notification. */
#define BT_ATT_OPCODE_WRITE_COMMAND                 0x52            /**< Write command. */
#define BT_ATT_OPCODE_SIGNED_WRITE_COMMAND          0xD2            /**< Signed write command. */
#define BT_ATT_OPCODE_SIGNED_FLAG                   0x80            /**< Signed flag. */
#define BT_ATT_OPCODE_COMMAND_FLAG                  0x40            /**< Command flag. */
#define BT_ATT_OPCODE_METHOD_MASK                   0x3F            /**< Method mask. */

#define BT_ATT_ERRCODE_INVALID_HANDLE                   0x01        /**< The given attribute handle was invalid on this server. */
#define BT_ATT_ERRCODE_READ_NOT_PERMITTED               0x02        /**< The attribute cannot be read. */
#define BT_ATT_ERRCODE_WRITE_NOT_PERMITTED              0x03        /**< The attribute cannot be written. */
#define BT_ATT_ERRCODE_INVALID_PDU                      0x04        /**< The attribute PDU is invalid. */
#define BT_ATT_ERRCODE_INSUFFICIENT_AUTHENTICATION      0x05        /**< The attribute requires authentication before it can be read or written. */
#define BT_ATT_ERRCODE_REQUEST_NOT_SUPPORTED            0x06        /**< The request received from the client is not supported. */
#define BT_ATT_ERRCODE_INVALID_OFFSET                   0x07        /**< The offset specified was past the end of the attribute. */
#define BT_ATT_ERRCODE_INSUFFICIENT_AUTHORIZATION       0x08        /**< The attribute requires authorization before it can be read or written. */
#define BT_ATT_ERRCODE_PREPARE_QUEUE_FULL               0x09        /**< Too many prepare writes have been queued. */
#define BT_ATT_ERRCODE_ATTRIBUTE_NOT_FOUND              0x0A        /**< No attribute found within the given attribute handle range. */
#define BT_ATT_ERRCODE_ATTRIBUTE_NOT_LONG               0x0B        /**< The attribute cannot be read or written using the Read Blob Request. */
#define BT_ATT_ERRCODE_INSUFFICIENT_ENCRYPTION_KEY_SIZE 0x0C        /**< The Encryption Key Size used for encrypting this link is insufficient. */
#define BT_ATT_ERRCODE_INVALID_ATTRIBUTE_VALUE_LENGTH   0x0D        /**< The attribute value length is invalid for the operation. */
#define BT_ATT_ERRCODE_UNLIKELY_ERROR                   0x0E        /**< The attribute request has encountered an error that was unlikely, and therefore could not be completed as requested. */
#define BT_ATT_ERRCODE_INSUFFICIENT_ENCRYPTION          0x0F        /**< The attribute requires encryption before it can be read or written. */
#define BT_ATT_ERRCODE_UNSUPPORTED_GROUP_TYPE           0x10        /**< The attribute type is not a supported grouping attribute as defined by a higher layer specification. */
#define BT_ATT_ERRCODE_INSUFFICIENT_RESOURCES           0x11        /**< Insufficient resources to complete the request. */
#define BT_ATT_ERRCODE_DATABASE_OUT_OF_SYNC             0x12        /**< The server requests the client to rediscover the database. */
#define BT_ATT_ERRCODE_VALUE_NOT_ALLOWED                0x13        /**< The attribute parameter value was not allowed. */
#define BT_ATT_ERRCODE_APPLICATION_BASE                 0x80        /**< The base value for the application error code. */
#define BT_ATT_ERRCODE_CONTINUE                         0x81        /**< There are more data waiting to be retrieved. */
#define BT_ATT_ERRCODE_APPLICATION_MAX                  0x9F        /**< The maximum value for the application error code. */
#define BT_ATT_ERRCODE_COMMON_BASE                      0xE0        /**< The base value for the common profile and service error code. */
#define BT_ATT_ERRCODE_COMMON_MAX                       0xFE        /**< The maximum value for the common profile and service error code. */
#define BT_ATT_ERRCODE_OUT_OF_MEMORY                    0xFF        /**< Out of memory. */

#define BT_ATT_ERRCODE_COMMON_WRITE_REQ_REJECTED        0xFC        /**< Write Request Rejected. */
#define BT_ATT_ERRCODE_COMMON_CCCD_IMPROPERLY_CONFIG    0xFD        /**< Client Characteristic Configuration Descriptor Improperly Configured. */
#define BT_ATT_ERRCODE_COMMON_PROCEDURE_IN_PROGRESS     BT_ATT_ERRCODE_COMMON_MAX        /**< Procedure Already in Progress. */
#define BT_ATT_ERRCODE_COMMON_OUT_OF_RANGE              BT_ATT_ERRCODE_OUT_OF_MEMORY        /**< Out of Range. */


#define BT_ATT_EXECUTE_WRITE_CANCEL                     0x00        /**< Execute write cancel. */
#define BT_ATT_EXECUTE_WRITE_WRITE                      0x01        /**< Execute write. */

#define BT_ATT_CLIENT_CHARC_CONFIG_SIZE          2             /**< The size of the Client Characteristic Configuration descriptor value. */
#define BT_ATT_CLIENT_CHARC_CONFIG_DEFAULT       0x0000        /**< The default value for the Client Characteristic Configuration descriptor value. */
#define BT_ATT_CLIENT_CHARC_CONFIG_NOTIFICATION  0x0001        /**< The Characteristic Value shall be notified. This value can only be set if the notify bit is set for the characteristic's properties. */
#define BT_ATT_CLIENT_CHARC_CONFIG_INDICATION    0x0002        /**< The Characteristic Value shall be indicated. This value can only be set if the indicate bit is set for the characteristic's properties. */

/**
 * @}
 */

/**
 * @defgroup BluetoothBLE_ATT_GATT_struct Struct
 * @{
 * This section defines basic data structures for the ATT.
 */

/**
 *  @brief ATT error response structure.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;             /**< #BT_ATT_OPCODE_ERROR_RESPONSE. */
    uint8_t error_opcode;       /**< The request that generated this error response. */
    uint16_t error_handle;      /**< The attribute handle that generated this error response. */
    uint8_t error_code;         /**< The reason why the request has generated an error response. */
}) bt_att_error_rsp_t;

/**
 *  @brief ATT exchange MTU request structure.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;             /**< #BT_ATT_OPCODE_EXCHANGE_MTU_REQUEST. */
    uint16_t client_rx_mtu;     /**< The MTU size of the client&apos;s receiver. */
}) bt_att_exchange_mtu_req_t;

/**
 *  @brief ATT exchange MTU response structure.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;             /**< #BT_ATT_OPCODE_EXCHANGE_MTU_RESPONSE. */
    uint16_t server_rx_mtu;     /**< The MTU size of the server&apos;s receiver. */
}) bt_att_exchange_mtu_rsp_t;

/**
 *  @brief ATT find information request structure.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;             /**< #BT_ATT_OPCODE_FIND_INFORMATION_REQUEST. */
    uint16_t starting_handle;   /**< The first requested handle&sbquo; an integer. */
    uint16_t ending_handle;     /**< The last requested handle&sbquo; an integer. */
}) bt_att_find_info_req_t;

/**
 *  @brief ATT find information response structure.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;            /**< #BT_ATT_OPCODE_FIND_INFORMATION_RESPONSE. */
    uint8_t format;            /**< The format of the information data. */
    uint8_t info_data[1];      /**< The information data with a format determined by the Format field. */
}) bt_att_find_info_rsp_t;

/**
 *  @brief ATT find by type value request structure.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;                 /**< #BT_ATT_OPCODE_FIND_BY_TYPE_VALUE_REQUEST. */
    uint16_t starting_handle;       /**< The first requested handle&sbquo; an integer. */
    uint16_t ending_handle;         /**< The last requested handle&sbquo; an integer. */
    uint16_t uuid;                  /**< Two octet UUID to find. */
    uint8_t attribute_value[1];     /**< The attribute value to find. */
}) bt_att_find_by_type_value_req_t;

/**
 *  @brief ATT find by type value response structure.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;                    /**< #BT_ATT_OPCODE_FIND_BY_TYPE_VALUE_RESPONSE. */
    uint8_t handles_info_list[1];      /**< A list of one or more Handle Information. */
}) bt_att_find_by_type_value_rsp_t;

/**
 *  @brief ATT read by type request structure.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;             /**< #BT_ATT_OPCODE_READ_BY_TYPE_REQUEST. */
    uint16_t starting_handle;   /**< The first requested handle&sbquo; an integer. */
    uint16_t ending_handle;     /**< The last requested handle&sbquo; an integer. */
    bt_uuid_t type;             /**< 2 or 16 octet UUID. */
}) bt_att_read_by_type_req_t;

/**
 *  @brief ATT read by type response structure.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;                    /**< #BT_ATT_OPCODE_READ_BY_TYPE_RESPONSE. */
    uint8_t length;                    /**< The size of each attribute handle-value pair. */
    uint8_t attribute_data_list[1];    /**< A list of Attribute Data. */
}) bt_att_read_by_type_rsp_t;

/**
 *  @brief ATT read request structure.
 */
BT_PACKED(
typedef struct{
    uint8_t opcode;                 /**< #BT_ATT_OPCODE_READ_REQUEST. */
    uint16_t attribute_handle;      /**< The handle of the attribute to be read. */
}) bt_att_read_req_t;

/**
 *  @brief ATT read response structure.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;                 /**< #BT_ATT_OPCODE_READ_RESPONSE. */
    uint8_t attribute_value[1];     /**< The value of the attribute of the given handle. */
}) bt_att_read_rsp_t;

/**
 *  @brief ATT read blob request structure.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;                 /**< #BT_ATT_OPCODE_READ_BLOB_REQUEST. */
    uint16_t attribute_handle;      /**< The handle of the attribute to be read. */
    uint16_t value_offset;          /**< The offset of the first octet to be read. */
}) bt_att_read_blob_req_t;

/**
 *  @brief ATT read blob response structure.
 */
typedef bt_att_read_rsp_t bt_att_read_blob_rsp_t;

/**
 *  @brief ATT read multiple request structure.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;                 /**< #BT_ATT_OPCODE_READ_MULTIPLE_REQUEST. */
    uint16_t set_of_handles[1];     /**< A set of two or more attribute handles. */
}) bt_att_read_multiple_req_t;

/**
 *  @brief ATT read multiple response structure.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;                 /**< #BT_ATT_OPCODE_READ_MULTIPLE_RESPONSE. */
    uint8_t set_of_values[1];       /**< A set of two or more values. */
}) bt_att_read_multiple_rsp_t;

/**
 *  @brief ATT read multiple variable request structure.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;                 /**< #BT_ATT_OPCODE_READ_MULTIPLE_VARIABLE_REQ. */
    uint16_t set_of_handles[1];     /**< A set of two or more attribute handles. */
}) bt_att_read_multiple_variable_req_t;

/**
 *  @brief ATT read multiple variable response structure.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;                     /**< #BT_ATT_OPCODE_READ_MULTIPLE_VARIABLE_RSP. */
    uint8_t length_value_tuple_list[1]; /**< A list of Length Value Tuples. */
}) bt_att_read_multiple_variable_rsp_t;


/**
 *  @brief ATT read by group type request structure.
 */
typedef bt_att_read_by_type_req_t bt_att_read_by_group_type_req_t;

/**
 *  @brief ATT read by group type response structure.
 */
typedef bt_att_read_by_type_rsp_t bt_att_read_by_group_type_rsp_t;

/**
 *  @brief ATT write request structure.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;                    /**< #BT_ATT_OPCODE_WRITE_REQUEST. */
    uint16_t attribute_handle;         /**< The handle of the attribute to be written. */
    uint8_t attribute_value[1];        /**< The value of the attribute to be written. */
}) bt_att_write_req_t;

/**
 *  @brief ATT write response structure.
 */
typedef struct {
    uint8_t opcode;     /**< #BT_ATT_OPCODE_WRITE_RESPONSE. */
} bt_att_write_rsp_t;

/**
 *  @brief ATT write command structure.
 */
typedef bt_att_write_req_t bt_att_write_command_t;

/**
 *  @brief ATT prepare write request structure.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;                     /**< #BT_ATT_OPCODE_PREPARE_WRITE_REQUEST. */
    uint16_t attribute_handle;          /**< The handle of the attribute to be written. */
    uint16_t value_offset;              /**< The offset of the first octet to be written. */
    uint8_t *part_attribute_value;      /**< The address of the attribute value to be written to. */
}) bt_att_prepare_write_req_t;

/**
 *  @brief ATT prepare write response structure.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;                    /**< #BT_ATT_OPCODE_PREPARE_WRITE_RESPONSE. */
    uint16_t attribute_handle;         /**< The handle of the attribute to be written. */
    uint16_t value_offset;             /**< The offset of the first octet to be written. */
    uint8_t part_attribute_value[1];   /**< The value of the attribute to be written. */
}) bt_att_prepare_write_rsp_t;

/**
 *  @brief ATT execute write request structure.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;             /**< #BT_ATT_OPCODE_EXECUTE_WRITE_REQUEST. */
    uint8_t flags;              /**< 0x00: Cancel all prepared writes. 0x01: Immediately write all pending prepared values. */
}) bt_att_execute_write_req_t;

/**
 *  @brief ATT execute write response structure.
 */
typedef bt_att_write_rsp_t bt_att_execute_write_rsp_t;

/**
 *  @brief ATT handle value notification structure.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;                     /**< #BT_ATT_OPCODE_HANDLE_VALUE_NOTIFICATION. */
    uint16_t handle;                    /**< The handle of the attribute. */
    uint8_t attribute_value[1];         /**< The current value of the attribute. */
}) bt_att_handle_value_notification_t;

/**
 *  @brief ATT handle value indication structure.
 */
typedef bt_att_handle_value_notification_t bt_att_handle_value_indication_t;

/**
 *  @brief ATT handle value confirmation structure.
 */
typedef bt_att_write_rsp_t bt_att_handle_value_confirmation_t;

/**
 *  @brief ATT multiple handle value notification structure.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;                                 /**< #BT_ATT_OPCODE_MULTIPLE_HANDLE_VALUE_NTF. */
    uint8_t handle_length_value_tuple_list[1];      /**< A list of Handle Length Value Tuples. */
}) bt_att_multiple_handle_value_notification_t;


/**
 * @}
 */

BT_EXTERN_C_END
/**
 * @}
 * @}
 * @}
 */

#endif /* __BT_ATT_SPEC_H__ */
