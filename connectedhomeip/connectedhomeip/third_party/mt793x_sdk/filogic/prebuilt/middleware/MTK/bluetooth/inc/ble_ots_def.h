/* Copyright Statement:
 *
 * (C) 2022  Airoha Technology Corp. All rights reserved.
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

#ifndef __BLE_OTS_DEF_H__
#define __BLE_OTS_DEF_H__
/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothOTS OTS
 * @{
 * This section introduces the Object Transfer Service (OTS) APIs including terms and acronyms, supported features, enums and structures.
 * For more information about OTS, please refer to the <a href="https://www.bluetooth.com/specifications/specs/?status=active&show_latest_version=0&show_latest_version=1&keyword=OTS&filter=">Object Transfer Service 1.0</a>.
 * @addtogroup OTS Bluetooth Low Energy
 * @{
 * This section defines the Low Energy (LE) OTS macros and structures.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b OTS                        | Object Transfer Service. This service provides management and control features supporting bulk data transfers which occur via a separate L2CAP connection oriented channel.|
 * |\b OACP                       | Object Action Control Point. The Object Action Control Point is used by a Client to control certain behaviors of the Server. With the exception of the Create procedure which creates a new object, the OACP procedures affect the Current Object only. |
 * |\b OLCP                       | Object List Control Point. The role of the OLCP is to provide a mechanism for the Client to find the desired object and to designate it as the Current Object.. |
 *
 *
 */
#include "bt_type.h"
/**
 * @defgroup Bluetoothhbif_ots_define Define
 * @{
 */

/**
 * @brief The OTS service UUID.
 */
#define BLE_UUID16_OBJECT_TRANSFER_SERVICE    (0x1825)        /**< Object transfer service. */

/**
 * @brief The max number of characteristics.
 */
#define BLE_OTS_MAX_CHARC_NUMBER                 (14)         /**< The number of OTS characteristics (There are 3 Object List Filter characteristic). */

/**
 * @brief The max number of characteristic types.
 */
#define BLE_OTS_MAX_CHARC_TYPE_NUMBER            (12)         /**< The number of OTS characteristics type. */

/**
 * @brief The length of object ID.
 */
#define BLE_OTS_OBJECT_ID_LENGTH                 (6)          /**< LUID (Locally Unique Identifier): UINT48 value in the range 0x000000000100 to 0xFFFFFFFFFFFF */

/**
 * @brief The number of List Filter characteristic.
 */
#define BLE_OTS_LIST_FILTER_CHARC_NUMBER         (3)          /**< The number of Object List Filter characteristic. */

/**
 * @brief The OTS service start handle for #BLE_OTS_SERVICE.
 */
#define BLE_OTS_START_HANDLE                                                (0xB001)    /**< OTS Start Handle. */
#define BLE_OTS_VALUE_HANDLE_OTS_FEATURE                                    (0xB003)    /**< Attribute Value Handle of OTS Feature Characteristic. */
#define BLE_OTS_VALUE_HANDLE_OBJECT_NAME                                    (0xB005)    /**< Attribute Value Handle of Object Name Characteristic. */
#define BLE_OTS_VALUE_HANDLE_OBJECT_TYPE                                    (0xB007)    /**< Attribute Value Handle of Object Type Characteristic. */
#define BLE_OTS_VALUE_HANDLE_OBJECT_SIZE                                    (0xB009)    /**< Attribute Value Handle of Object Size Characteristic. */
#define BLE_OTS_VALUE_HANDLE_OBJECT_ID                                      (0xB00B)    /**< Attribute Value Handle of Object Id Characteristic. */
#define BLE_OTS_VALUE_HANDLE_OBJECT_PROPERTIES                              (0xB00D)    /**< Attribute Value Handle of Object Properties Characteristic. */
#define BLE_OTS_VALUE_HANDLE_OBJECT_ACTION_CONTROL_POINT                    (0xB00F)    /**< Attribute Value Handle of Object Action Control Point Characteristic. */
#define BLE_OTS_VALUE_HANDLE_OBJECT_LIST_CONTROL_POINT                      (0xB012)    /**< Attribute Value Handle of Object List Control Point  Characteristic. */
#define BLE_OTS_END_HANDLE                                                  (0xB013)    /**< OTS End Handle. */

/**
* @brief OTS characteristic UUID.
*/
#define BLE_SIG_UUID16_OTS_FEATURE                                   (0x2ABD)   /**< OTS feature UUID.*/
#define BLE_SIG_UUID16_OBJECT_NAME                                   (0x2ABE)   /**< Object Name UUID.*/
#define BLE_SIG_UUID16_OBJECT_TYPE                                   (0x2ABF)   /**< Object Type UUID.*/
#define BLE_SIG_UUID16_OBJECT_SIZE                                   (0x2AC0)   /**< Object Size UUID.*/
#define BLE_SIG_UUID16_OBJECT_FIRST_CREATED                          (0x2AC1)   /**< Object First Created UUID.*/
#define BLE_SIG_UUID16_OBJECT_LAST_MODIFIED                          (0x2AC2)   /**< Object Last Modified UUID.*/
#define BLE_SIG_UUID16_OBJECT_ID                                     (0x2AC3)   /**< Object Id UUID.*/
#define BLE_SIG_UUID16_OBJECT_PROPERTIES                             (0x2AC4)   /**< Object Properties UUID.*/
#define BLE_SIG_UUID16_OBJECT_ACTION_CONTROL_POINT                   (0x2AC5)   /**< Object Action Control Point UUID.*/
#define BLE_SIG_UUID16_OBJECT_LIST_CONTROL_POINT                     (0x2AC6)   /**< Object List Control Point UUID.*/
#define BLE_SIG_UUID16_OBJECT_LIST_FILTER                            (0x2AC7)   /**< Object List Filter UUID.*/
#define BLE_SIG_UUID16_OBJECT_CHANGED                                (0x2AC8)   /**< Object Changed UUID.*/
typedef uint8_t ble_ots_uuid_t;

#define BLE_OTS_CHARC_TYPE_OTS_FEATURE                    0     /**< Characteristic type of OTS feature.*/
#define BLE_OTS_CHARC_TYPE_OBJECT_NAME                    1     /**< Characteristic type of Object Name.*/
#define BLE_OTS_CHARC_TYPE_OBJECT_TYPE                    2     /**< Characteristic type of Object Type.*/
#define BLE_OTS_CHARC_TYPE_OBJECT_SIZE                    3     /**< Characteristic type of Object Size.*/
#define BLE_OTS_CHARC_TYPE_OBJECT_FIRST_CREATED           4     /**< Characteristic type of Object First Created.*/
#define BLE_OTS_CHARC_TYPE_OBJECT_LAST_MODIFIED           5     /**< Characteristic type of Object Last Modified.*/
#define BLE_OTS_CHARC_TYPE_OBJECT_ID                      6     /**< Characteristic type of Object Id.*/
#define BLE_OTS_CHARC_TYPE_OBJECT_PROPERTIES              7     /**< Characteristic type of Object Properties.*/
#define BLE_OTS_CHARC_TYPE_OBJECT_ACTION_CONTROL_POINT    8     /**< Characteristic type of Object Action Control Point.*/
#define BLE_OTS_CHARC_TYPE_OBJECT_LIST_CONTROL_POINT      9     /**< Characteristic type of Object List Control Point.*/
#define BLE_OTS_CHARC_TYPE_OBJECT_LIST_FILTER             10    /**< Characteristic type of Object List Filter.*/
#define BLE_OTS_CHARC_TYPE_OBJECT_CHANGED                 11    /**< Characteristic type of Object Changed.*/
#define BLE_OTS_CHARC_TYPE_OBJECT_TRANSFER_SERVICE        0xE0  /**< Characteristic type of Object Transfer Service.*/
#define BLE_OTS_CHARC_TYPE_INVALID                        0xFF  /**< Invalid Characteristic type.*/
typedef uint8_t ble_otp_charc_type_t;                   /**< The OTS characteristic type.*/

/**
* @brief The OACP features.
*/
typedef struct {
    uint32_t oacp_create_op_code_supported:1;                       /**< Create op code supported. */
    uint32_t oacp_delete_op_code_supported:1;                       /**< Delete op code supported. */
    uint32_t oacp_calculate_checksum_op_code_supported:1;           /**< Calculate checksum op code supported. */
    uint32_t oacp_execute_op_code_supported:1;                      /**< Execute op code supported. */
    uint32_t oacp_read_op_code_supported:1;                         /**< Read op code supported. */
    uint32_t oacp_write_op_code_supported:1;                        /**< Write op code supported. */
    uint32_t appending_additional_data_to_object_supported:1;       /**< Appending additional data to object supported. */
    uint32_t truncation_of_objects_supported:1;                     /**< Truncation of objects supported. */
    uint32_t patching_of_objects_supported:1;                       /**< Patching of objects supported. */
    uint32_t oacp_abort_op_code_supported:1;                        /**< Reserved */
    uint32_t reserved:22;
} ble_ots_oacp_features_t;

/**
* @brief The OLCP features.
*/
typedef struct {
    uint32_t olcp_go_to_op_code_supported:1;                         /**< Go to op code supported. */
    uint32_t olcp_order_op_code_supported:1;                         /**< Order op code supported. */
    uint32_t olcp_request_number_of_objects_op_code_supported:1;     /**< Request number of objects op code supported. */
    uint32_t olcp_clear_marking_op_code_supported:1;                 /**< Clear marking op code supported. */
    uint32_t reserved:28;                                            /**< Reserved */
} ble_ots_olcp_features_t;

/**
 *  @brief The Object properties.
 */
typedef struct {
    uint32_t delete:1;          /**< Delete. */
    uint32_t execute:1;         /**< Execute. */
    uint32_t read:1;            /**< Read. */
    uint32_t write:1;           /**< Write. */
    uint32_t append:1;          /**< Append. */
    uint32_t truncate:1;        /**< Truncate. */
    uint32_t patch:1;           /**< Patch. */
    uint32_t mark:1;            /**< Mark. */
    uint32_t reserved:24;       /**< Reserved. */
} ble_ots_object_properties_t;

/**
* @brief The OACP op code.
*/
#define BLE_OTS_OACP_OPCODE_CREATE                       0x01   /**< Create.*/
#define BLE_OTS_OACP_OPCODE_DELETE                       0x02   /**< Delete.*/
#define BLE_OTS_OACP_OPCODE_CALCULATE_CHECKSUM           0x03   /**< Calculate checksum.*/
#define BLE_OTS_OACP_OPCODE_EXECUTE                      0x04   /**< Execute.*/
#define BLE_OTS_OACP_OPCODE_READ                         0x05   /**< Read.*/
#define BLE_OTS_OACP_OPCODE_WRITE                        0x06   /**< Write.*/
#define BLE_OTS_OACP_OPCODE_ABORT                        0x07   /**< Abort.*/
#define BLE_OTS_OACP_OPCODE_RESPONSE_CODE                0x60   /**< Response code.*/
typedef uint8_t ble_ots_oacp_opcode_t;                  /**< The OTS OACP opcode.*/

/**
* @brief The OACP result code.
*/
#define BLE_OTS_OACP_RESULT_CODE_SUCCESS                     0x01   /**< Success.*/
#define BLE_OTS_OACP_RESULT_CODE_OPCODE_NOT_SUPPORTED        0x02   /**< Not supported.*/
#define BLE_OTS_OACP_RESULT_CODE_INVALID_PARAMETER           0x03   /**< Invalid parameter.*/
#define BLE_OTS_OACP_RESULT_CODE_INSUFFICIENT_RESOURCE       0x04   /**< Insufficient resource.*/
#define BLE_OTS_OACP_RESULT_CODE_INVALID_OBJECT              0x05   /**< Invalid object.*/
#define BLE_OTS_OACP_RESULT_CODE_CHANNEL_UNAVAILABLE         0x06   /**< Channel unavailable.*/
#define BLE_OTS_OACP_RESULT_CODE_UNSUPPORTED_TYPE            0x07   /**< Unsupported type.*/
#define BLE_OTS_OACP_RESULT_CODE_PROCEDURE_NOT_PERMITTED     0x08   /**< Procedure not permitted.*/
#define BLE_OTS_OACP_RESULT_CODE_OBJECT_LOCKED               0x09   /**< Object locked.*/
#define BLE_OTS_OACP_RESULT_CODE_OPERATION_FAILED            0x0A   /**< Operation failed.*/
typedef uint8_t ble_ots_oacp_result_code_t;             /**< The OTS OACP result code.*/

/**
* @brief The OLCP op code.
*/
#define BLE_OTS_OLCP_OPCODE_FIRST                        0x01   /**< First.*/
#define BLE_OTS_OLCP_OPCODE_LAST                         0x02   /**< Last.*/
#define BLE_OTS_OLCP_OPCODE_PREVIOUS                     0x03   /**< Previous.*/
#define BLE_OTS_OLCP_OPCODE_NEXT                         0x04   /**< Next.*/
#define BLE_OTS_OLCP_OPCODE_GO_TO                        0x05   /**< Go to.*/
#define BLE_OTS_OLCP_OPCODE_ORDER                        0x06   /**< Order.*/
#define BLE_OTS_OLCP_OPCODE_REQUEST_NUMBER_OF_OBJECTS    0x07   /**< Request number of objects.*/
#define BLE_OTS_OLCP_OPCODE_CLEAR_MARKING                0x08   /**< Clear marking.*/
#define BLE_OTS_OLCP_OPCODE_RESPONSE_CODE                0x70   /**< Response code.*/
typedef uint8_t ble_ots_olcp_opcode_t;                  /**< The OTS OLCP opcode.*/

/**
* @brief The OLCP result code.
*/
#define BLE_OTS_OLCP_RESULT_CODE_SUCCESS                     0x01   /**< Success.*/
#define BLE_OTS_OLCP_RESULT_CODE_OPCODE_NOT_SUPPORTED        0x02   /**< Not supported.*/
#define BLE_OTS_OLCP_RESULT_CODE_INVALID_PARAMETER           0x03   /**< Invalid parameter.*/
#define BLE_OTS_OLCP_RESULT_CODE_OPERATION_FAILED            0x04   /**< Operation failed.*/
#define BLE_OTS_OLCP_RESULT_CODE_OUT_OF_BOUNDS               0x05   /**< Out of bounds.*/
#define BLE_OTS_OLCP_RESULT_CODE_TWO_MANY_OBJECTS            0x06   /**< Two many objects.*/
#define BLE_OTS_OLCP_RESULT_CODE_NO_OBJECT                   0x07   /**< No object.*/
#define BLE_OTS_OLCP_RESULT_CODE_OBJECT_ID_NOT_FOUND         0x08   /**< Object id not found.*/
typedef uint8_t ble_ots_olcp_result_code_t;             /**< The OTS OLCP result code.*/

/**
* @brief The list sort order.
*/
#define BLE_OTS_LIST_SORT_ORDER_RESERVED                                     0      /**< Reserved.*/
#define BLE_OTS_LIST_SORT_ORDER_OBJECT_NAME_ASCENDING                        0x01   /**< Name ascending.*/
#define BLE_OTS_LIST_SORT_ORDER_OBJECT_TYPE_ASCENDING                        0x02   /**< Type ascending.*/
#define BLE_OTS_LIST_SORT_ORDER_OBJECT_CURRENT_SIZE_ASCENDING                0x03   /**< Current size ascending.*/
#define BLE_OTS_LIST_SORT_ORDER_OBJECT_FIRST_CREATED_TIMESTAMP_ASCENDING     0x04   /**< First created timestamp ascending.*/
#define BLE_OTS_LIST_SORT_ORDER_OBJECT_LAST_CREATED_TIMESTAMP_ASCENDING      0x05   /**< Last created timestamp ascending.*/
#define BLE_OTS_LIST_SORT_ORDER_OBJECT_NAME_DESCENDING                       0x11   /**< Name descending.*/
#define BLE_OTS_LIST_SORT_ORDER_OBJECT_TYPE_DESCENDING                       0x12   /**< Type filter descending.*/
#define BLE_OTS_LIST_SORT_ORDER_OBJECT_CURRENT_SIZE_DESCENDING               0x13   /**< Current filter descending.*/
#define BLE_OTS_LIST_SORT_ORDER_OBJECT_FIRST_CREATED_TIMESTAMP_DESCENDING    0x14   /**< First created timestamp descending.*/
#define BLE_OTS_LIST_SORT_ORDER_OBJECT_LAST_CREATED_TIMESTAMP_DESCENDING     0x15   /**< Last created timestamp descending.*/
typedef uint8_t ble_ots_list_sort_order_t;                              /**< The OTS list sort order.*/

/**
* @brief The filter value.
*/
#define BLE_OTS_FILTER_VALUE_NO_FILTER                                       0x00   /**< No filter.*/
#define BLE_OTS_FILTER_VALUE_NAME_STARTS_WITH                                0x01   /**< Name starts with.*/
#define BLE_OTS_FILTER_VALUE_NAME_ENDS_WITH                                  0x02   /**< Name ends with.*/
#define BLE_OTS_FILTER_VALUE_NAME_CONTAINS                                   0x03   /**< Name contains.*/
#define BLE_OTS_FILTER_VALUE_NAME_IS_EXACTLY                                 0x04   /**< Name is exatly.*/
#define BLE_OTS_FILTER_VALUE_OBJECT_TYPE                                     0x05   /**< Object type.*/
#define BLE_OTS_FILTER_VALUE_CREATED_BETWEEN                                 0x06   /**< Created between.*/
#define BLE_OTS_FILTER_VALUE_MODIFIED_BETWEEN                                0x07   /**< Modified between.*/
#define BLE_OTS_FILTER_VALUE_CURRENT_SIZE_BETWEEN                            0x08   /**< Current size between.*/
#define BLE_OTS_FILTER_VALUE_ALLOCATED_SIZE_BETWEEN                          0x09   /**< Allocated size between.*/
#define BLE_OTS_FILTER_VALUE_MARKED_OBJECTS                                  0x0A   /**< Marked objects.*/
typedef uint8_t ble_ots_filter_value_t;                                     /**< The OTS filter value.*/

/**
* @brief The object changed flags.
*/
typedef struct {
    uint8_t source_of_change:1;                             /**< Source of change.*/
    uint8_t change_occurred_to_the_object_contents:1;       /**< Change occurred to the object contents.*/
    uint8_t change_occurred_to_the_object_metadata:1;       /**< Change occurred to the object metadata.*/
    uint8_t object_creation:1;                              /**< Object creation.*/
    uint8_t object_deletion:1;                              /**< Object deletion.*/
    uint8_t reserved:3;                                     /**< Reserved.*/
} ble_ots_object_changed_flags_field_t;

/**
 *  @brief This structure defines the UUID type and attribute handle.
 */
typedef struct {
    ble_ots_uuid_t uuid_type;   /**<  UUID type. */
    uint16_t att_handle;        /**<  Attribute handle. */
} ble_ots_attribute_handle_t;   /**< The OTS attribute handle.*/
/**
 * @}
 */

/**
 * @}
 * @}
 * @}
 */
#endif

