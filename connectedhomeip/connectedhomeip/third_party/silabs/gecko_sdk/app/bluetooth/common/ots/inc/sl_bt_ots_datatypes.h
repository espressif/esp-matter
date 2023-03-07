/***************************************************************************//**
 * @file
 * @brief Object Transfer Service Datatypes and Definitions
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef SL_BT_OTS_DATATYPES_H
#define SL_BT_OTS_DATATYPES_H

#include <stdbool.h>
#include <stdint.h>
#include "sl_bt_api.h"
#include "sl_slist.h"
#include "sl_status.h"
#include "sl_enum.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Global configuration declarations

// Attribute error codes
#define ATT_STATUS_MASK                        0xFF
#define ATT_ERR_SUCCESS                        0x00
#define ATT_ERR_INVALID_HANDLE                 (SL_STATUS_BT_ATT_INVALID_HANDLE & ATT_STATUS_MASK)
#define ATT_ERR_READ_NOT_PERMITTED             (SL_STATUS_BT_ATT_READ_NOT_PERMITTED & ATT_STATUS_MASK)
#define ATT_ERR_WRITE_NOT_PERMITTED            (SL_STATUS_BT_ATT_WRITE_NOT_PERMITTED & ATT_STATUS_MASK)
#define ATT_ERR_INVALID_PDU                    (SL_STATUS_BT_ATT_INVALID_PDU & ATT_STATUS_MASK)
#define ATT_ERR_AUTHENTICATION                 (SL_STATUS_BT_ATT_INSUFFICIENT_AUTHENTICATION & ATT_STATUS_MASK)
#define ATT_ERR_NOT_SUPPORTED                  (SL_STATUS_BT_ATT_REQUEST_NOT_SUPPORTED & ATT_STATUS_MASK)
#define ATT_ERR_INVALID_OFFSET                 (SL_STATUS_BT_ATT_INVALID_OFFSET & ATT_STATUS_MASK)
#define ATT_ERR_AUTHORIZATION                  (SL_STATUS_BT_ATT_INSUFFICIENT_AUTHORIZATION & ATT_STATUS_MASK)
#define ATT_ERR_PREPARE_QUEUE_FULL             (SL_STATUS_BT_ATT_PREPARE_QUEUE_FULL & ATT_STATUS_MASK)
#define ATT_ERR_ATTRIBUTE_NOT_FOUND            (SL_STATUS_BT_ATT_ATT_NOT_FOUND & ATT_STATUS_MASK)
#define ATT_ERR_ATTRIBUTE_NOT_LONG             (SL_STATUS_BT_ATT_ATT_NOT_LONG & ATT_STATUS_MASK)
#define ATT_ERR_ENCRYPTION_KEY_SIZE            (SL_STATUS_BT_ATT_INSUFFICIENT_ENC_KEY_SIZE & ATT_STATUS_MASK)
#define ATT_ERR_INVALID_ATTRIBUTE_LEN          (SL_STATUS_BT_ATT_INVALID_ATT_LENGTH & ATT_STATUS_MASK)
#define ATT_ERR_UNLIKELY                       (SL_STATUS_BT_ATT_UNLIKELY_ERROR & ATT_STATUS_MASK)
#define ATT_ERR_INSUFFICIENT_ENCRYPTION        (SL_STATUS_BT_ATT_INSUFFICIENT_ENCRYPTION & ATT_STATUS_MASK)
#define ATT_ERR_UNSUPPORTED_GROUP_TYPE         (SL_STATUS_BT_ATT_UNSUPPORTED_GROUP_TYPE & ATT_STATUS_MASK)
#define ATT_ERR_INSUFFICIENT_RESOURCES         (SL_STATUS_BT_ATT_INSUFFICIENT_RESOURCES & ATT_STATUS_MASK)
#define ATT_ERR_DB_OUT_OF_SYNC                 (SL_STATUS_BT_ATT_OUT_OF_SYNC & ATT_STATUS_MASK)
#define ATT_ERR_VALUE_NOT_ALLOWED              (SL_STATUS_BT_ATT_VALUE_NOT_ALLOWED & ATT_STATUS_MASK)
#define ATT_ERR_IMPORER_CCCD                   (SL_STATUS_BT_ATT_CLIENT_CHARACTERISTIC_CONFIGURATION_DESCRIPTOR_IMPROPERLY_CONFIGURED & ATT_STATUS_MASK)

// Specific error codes
#define ATT_ERR_WRITE_REQUEST_REJECTED         0x80
#define ATT_ERR_OBJECT_NOT_SELECTED            0x81
#define ATT_ERR_CONCURRENCY_LIMIT_EXCEEDED     0x82
#define ATT_ERR_OBJECT_NAME_ALREADY_EXISTS     0x83

// -----------------------------------------------------------------------------
// Flag declarations

// OACP Feature definitions
#define SL_BT_OTS_OACP_FEATURE_CREATE_OP_CODE_SUPPORTED_BIT_POS                         0
#define SL_BT_OTS_OACP_FEATURE_CREATE_OP_CODE_SUPPORTED_MASK                            (1 << SL_BT_OTS_OACP_FEATURE_CREATE_OP_CODE_SUPPORTED_BIT_POS)
#define SL_BT_OTS_OACP_FEATURE_DELETE_OP_CODE_SUPPORTED_BIT_POS                         1
#define SL_BT_OTS_OACP_FEATURE_DELETE_OP_CODE_SUPPORTED_MASK                            (1 << SL_BT_OTS_OACP_FEATURE_DELETE_OP_CODE_SUPPORTED_BIT_POS)
#define SL_BT_OTS_OACP_FEATURE_CALCULATE_CHECKSUM_OP_CODE_SUPPORTED_BIT_POS             2
#define SL_BT_OTS_OACP_FEATURE_CALCULATE_CHECKSUM_OP_CODE_SUPPORTED_MASK                (1 << SL_BT_OTS_OACP_FEATURE_CALCULATE_CHECKSUM_OP_CODE_SUPPORTED_BIT_POS)
#define SL_BT_OTS_OACP_FEATURE_EXECUTE_OP_CODE_SUPPORTED_BIT_POS                        3
#define SL_BT_OTS_OACP_FEATURE_EXECUTE_OP_CODE_SUPPORTED_MASK                           (1 << SL_BT_OTS_OACP_FEATURE_EXECUTE_OP_CODE_SUPPORTED_BIT_POS)
#define SL_BT_OTS_OACP_FEATURE_READ_OP_CODE_SUPPORTED_BIT_POS                           4
#define SL_BT_OTS_OACP_FEATURE_READ_OP_CODE_SUPPORTED_MASK                              (1 << SL_BT_OTS_OACP_FEATURE_READ_OP_CODE_SUPPORTED_BIT_POS)
#define SL_BT_OTS_OACP_FEATURE_WRITE_OP_CODE_SUPPORTED_BIT_POS                          5
#define SL_BT_OTS_OACP_FEATURE_WRITE_OP_CODE_SUPPORTED_MASK                             (1 << SL_BT_OTS_OACP_FEATURE_WRITE_OP_CODE_SUPPORTED_BIT_POS)
#define SL_BT_OTS_OACP_FEATURE_APPEND_ADDITIONAL_DATA_SUPPORTED_BIT_POS                 6
#define SL_BT_OTS_OACP_FEATURE_APPEND_ADDITIONAL_DATA_SUPPORTED_MASK                    (1 << SL_BT_OTS_OACP_FEATURE_APPEND_ADDITIONAL_DATA_SUPPORTED_BIT_POS)
#define SL_BT_OTS_OACP_FEATURE_TRUNCATION_OF_OBJECTS_SUPPORTED_BIT_POS                  7
#define SL_BT_OTS_OACP_FEATURE_TRUNCATION_OF_OBJECTS_SUPPORTED_MASK                     (1 << SL_BT_OTS_OACP_FEATURE_TRUNCATION_OF_OBJECTS_SUPPORTED_BIT_POS)
#define SL_BT_OTS_OACP_FEATURE_PATCHING_OF_OBJECTS_SUPPORTED_BIT_POS                    8
#define SL_BT_OTS_OACP_FEATURE_PATCHING_OF_OBJECTS_SUPPORTED_MASK                       (1 << SL_BT_OTS_OACP_FEATURE_PATCHING_OF_OBJECTS_SUPPORTED_BIT_POS)
#define SL_BT_OTS_OACP_FEATURE_ABORT_OP_CODE_SUPPORTED_BIT_POS                          9
#define SL_BT_OTS_OACP_FEATURE_ABORT_OP_CODE_SUPPORTED_MASK                             (1 << SL_BT_OTS_OACP_FEATURE_ABORT_OP_CODE_SUPPORTED_BIT_POS)

// OLCP Feature definitions
#define SL_BT_OTS_OLCP_FEATURE_GO_TO_OP_CODE_SUPPORTED_BIT_POS                          0
#define SL_BT_OTS_OLCP_FEATURE_GO_TO_OP_CODE_SUPPORTED_MASK                             (1 << SL_BT_OTS_OLCP_FEATURE_GO_TO_OP_CODE_SUPPORTED_BIT_POS)
#define SL_BT_OTS_OLCP_FEATURE_ORDER_OP_CODE_SUPPORTED_BIT_POS                          1
#define SL_BT_OTS_OLCP_FEATURE_ORDER_OP_CODE_SUPPORTED_MASK                             (1 << SL_BT_OTS_OLCP_FEATURE_ORDER_OP_CODE_SUPPORTED_BIT_POS)
#define SL_BT_OTS_OLCP_FEATURE_REQUEST_NUMBER_OF_OBJECTS_OP_CODE_SUPPORTED_BIT_POS      2
#define SL_BT_OTS_OLCP_FEATURE_REQUEST_NUMBER_OF_OBJECTS_OP_CODE_SUPPORTED_MASK         (1 << SL_BT_OTS_OLCP_FEATURE_REQUEST_NUMBER_OF_OBJECTS_OP_CODE_SUPPORTED_BIT_POS)
#define SL_BT_OTS_OLCP_FEATURE_CLEAR_MARKING_OP_CODE_SUPPORTED_BIT_POS                  3
#define SL_BT_OTS_OLCP_FEATURE_CLEAR_MARKING_OP_CODE_SUPPORTED_MASK                     (1 << SL_BT_OTS_OLCP_FEATURE_CLEAR_MARKING_OP_CODE_SUPPORTED_BIT_POS)

// Object properties definitions
#define SL_BT_OTS_OBJECT_PROPERTY_DELETE_BIT_POS                                        0
#define SL_BT_OTS_OBJECT_PROPERTY_DELETE_MASK                                           (1 << SL_BT_OTS_OBJECT_PROPERTY_DELETE_BIT_POS)
#define SL_BT_OTS_OBJECT_PROPERTY_EXECUTE_BIT_POS                                       1
#define SL_BT_OTS_OBJECT_PROPERTY_EXECUTE_MASK                                          (1 << SL_BT_OTS_OBJECT_PROPERTY_EXECUTE_BIT_POS)
#define SL_BT_OTS_OBJECT_PROPERTY_READ_BIT_POS                                          2
#define SL_BT_OTS_OBJECT_PROPERTY_READ_MASK                                             (1 << SL_BT_OTS_OBJECT_PROPERTY_READ_BIT_POS)
#define SL_BT_OTS_OBJECT_PROPERTY_WRITE_BIT_POS                                         3
#define SL_BT_OTS_OBJECT_PROPERTY_WRITE_MASK                                            (1 << SL_BT_OTS_OBJECT_PROPERTY_WRITE_BIT_POS)
#define SL_BT_OTS_OBJECT_PROPERTY_APPEND_BIT_POS                                        4
#define SL_BT_OTS_OBJECT_PROPERTY_APPEND_MASK                                           (1 << SL_BT_OTS_OBJECT_PROPERTY_APPEND_BIT_POS)
#define SL_BT_OTS_OBJECT_PROPERTY_TRUNCATE_BIT_POS                                      5
#define SL_BT_OTS_OBJECT_PROPERTY_TRUNCATE_MASK                                         (1 << SL_BT_OTS_OBJECT_PROPERTY_TRUNCATE_BIT_POS)
#define SL_BT_OTS_OBJECT_PROPERTY_PATCH_BIT_POS                                         6
#define SL_BT_OTS_OBJECT_PROPERTY_PATCH_MASK                                            (1 << SL_BT_OTS_OBJECT_PROPERTY_PATCH_BIT_POS)
#define SL_BT_OTS_OBJECT_PROPERTY_MARK_BIT_POS                                          7
#define SL_BT_OTS_OBJECT_PROPERTY_MARK_MASK                                             (1 << SL_BT_OTS_OBJECT_PROPERTY_MARK_BIT_POS)

// Write mode definitions
#define SL_BT_OTS_WRITE_MODE_NONE                                                       0
#define SL_BT_OTS_WRITE_MODE_TRUNCATE_BIT_POS                                           1
#define SL_BT_OTS_WRITE_MODE_TRUNCATE_MASK                                              (1 << SL_BT_OTS_WRITE_MODE_TRUNCATE_BIT_POS)

// Object change flags
#define SL_BT_OTS_OBJECT_CHANGE_SOURCE_BIT_POS                                          0
#define SL_BT_OTS_OBJECT_CHANGE_SOURCE_MASK                                             (1 << SL_BT_OTS_OBJECT_CHANGE_SOURCE_BIT_POS)
#define SL_BT_OTS_OBJECT_CHANGE_CONTENTS_BIT_POS                                        1
#define SL_BT_OTS_OBJECT_CHANGE_CONTENTS_MASK                                           (1 << SL_BT_OTS_OBJECT_CHANGE_CONTENTS_BIT_POS)
#define SL_BT_OTS_OBJECT_CHANGE_METADATA_BIT_POS                                        2
#define SL_BT_OTS_OBJECT_CHANGE_METADATA_MASK                                           (1 << SL_BT_OTS_OBJECT_CHANGE_METADATA_BIT_POS)
#define SL_BT_OTS_OBJECT_CHANGE_CREATION_BIT_POS                                        3
#define SL_BT_OTS_OBJECT_CHANGE_CREATION_MASK                                           (1 << SL_BT_OTS_OBJECT_CHANGE_CREATION_BIT_POS)
#define SL_BT_OTS_OBJECT_CHANGE_DELETION_BIT_POS                                        4
#define SL_BT_OTS_OBJECT_CHANGE_DELETION_MASK                                           (1 << SL_BT_OTS_OBJECT_CHANGE_DELETION_BIT_POS)

// Directory Listing Object
#define SL_BT_OTS_DLO_OBJECT_UUID_SIZE_BIT_POS                                          0
#define SL_BT_OTS_DLO_OBJECT_UUID_SIZE_MASK                                             (1 << SL_BT_OTS_DLO_OBJECT_UUID_SIZE_BIT_POS)
#define SL_BT_OTS_DLO_OBJECT_FIRST_CREATED_PRESENT_BIT_POS                              1
#define SL_BT_OTS_DLO_OBJECT_FIRST_CREATED_PRESENT_MASK                                 (1 << SL_BT_OTS_DLO_OBJECT_FIRST_CREATED_PRESENT_BIT_POS)
#define SL_BT_OTS_DLO_OBJECT_LAST_MODIFIED_PRESENT_BIT_POS                              2
#define SL_BT_OTS_DLO_OBJECT_LAST_MODIFIED_PRESENT_MASK                                 (1 << SL_BT_OTS_DLO_OBJECT_LAST_MODIFIED_PRESENT_BIT_POS)
#define SL_BT_OTS_DLO_OBJECT_PROPERTIES_PRESENT_BIT_POS                                 3
#define SL_BT_OTS_DLO_OBJECT_PROPERTIES_PRESENT_MASK                                    (1 << SL_BT_OTS_DLO_OBJECT_PROPERTIES_PRESENT_BIT_POS)
#define SL_BT_OTS_DLO_EXTENDED_FLAGS_PRESENT_BIT_POS                                    7
#define SL_BT_OTS_DLO_EXTENDED_FLAGS_PRESENT_MASK                                       (1 << SL_BT_OTS_DLO_EXTENDED_FLAGS_PRESENT_BIT_POS)

// Metadata field
#define METADATA_FIELD(X) (1 << X)
#define SL_BT_OTS_METADATA_FIELD_ALL           0xFF
#define SL_BT_OTS_METADATA_FIELD_NAME          METADATA_FIELD(SL_BT_OTS_OBJECT_METADATA_READ_OBJECT_NAME)
#define SL_BT_OTS_METADATA_FIELD_TYPE          METADATA_FIELD(SL_BT_OTS_OBJECT_METADATA_READ_OBJECT_TYPE)
#define SL_BT_OTS_METADATA_FIELD_SIZE          METADATA_FIELD(SL_BT_OTS_OBJECT_METADATA_READ_OBJECT_SIZE)
#define SL_BT_OTS_METADATA_FIELD_FIRST_CREATED METADATA_FIELD(SL_BT_OTS_OBJECT_METADATA_READ_OBJECT_FIRST_CREATED)
#define SL_BT_OTS_METADATA_FIELD_LAST_MODIFIED METADATA_FIELD(SL_BT_OTS_OBJECT_METADATA_READ_OBJECT_LAST_MODIFIED)
#define SL_BT_OTS_METADATA_FIELD_ID            METADATA_FIELD(SL_BT_OTS_OBJECT_METADATA_READ_OBJECT_ID)
#define SL_BT_OTS_METADATA_FIELD_PROPERTIES    METADATA_FIELD(SL_BT_OTS_OBJECT_METADATA_READ_OBJECT_PROPERTIES)
#define SL_BT_OTS_METADATA_FIELD_NONE          0x00

// -----------------------------------------------------------------------------
// Object ID declarations

#define SL_BT_OTS_OBJECT_ID_SIZE                6
#define SL_BT_OTS_OBJECT_ID_USABLE_SIZE         5
#define SL_BT_OTS_OBJECT_ID_RFU_MASK            0x0000000000FF

#define SL_BT_OTS_INVALID_OBJECT_RFU            0xFF
#define SL_BT_OTS_DIRECTORY_LIST_OBJECT_ID      0x000000000000
#define SL_BT_OTS_TIME_SIZE                     7

#define SL_BT_OTS_UUID_SIZE_16                  2
#define SL_BT_OTS_UUID_SIZE_128                 16

// -----------------------------------------------------------------------------
// OTS Misc declarations

/// OTS Subscription status for no subscription
#define SL_BT_OTS_SUBSCRIPTION_STATUS_NONE                 0

/// OTS Characteristics
#define SL_BT_OTS_CHARACTERISTIC_UUID_OTS_FEATURE          0x2ABD
#define SL_BT_OTS_CHARACTERISTIC_UUID_OBJECT_NAME          0x2ABE
#define SL_BT_OTS_CHARACTERISTIC_UUID_OBJECT_TYPE          0x2ABF
#define SL_BT_OTS_CHARACTERISTIC_UUID_OBJECT_SIZE          0x2AC0
#define SL_BT_OTS_CHARACTERISTIC_UUID_OBJECT_FIRST_CREATED 0x2AC1
#define SL_BT_OTS_CHARACTERISTIC_UUID_OBJECT_LAST_MODIFIED 0x2AC2
#define SL_BT_OTS_CHARACTERISTIC_UUID_OBJECT_ID            0x2AC3
#define SL_BT_OTS_CHARACTERISTIC_UUID_OBJECT_PROPERTIES    0x2AC4
#define SL_BT_OTS_CHARACTERISTIC_UUID_OACP                 0x2AC5
#define SL_BT_OTS_CHARACTERISTIC_UUID_OLCP                 0x2AC6
#define SL_BT_OTS_CHARACTERISTIC_UUID_OBJECT_LIST_FILTER   0x2AC7
#define SL_BT_OTS_CHARACTERISTIC_UUID_OBJECT_CHANGED       0x2AC8

#define SL_BT_OTS_CHARACTERISTIC_UUID_COUNT                      12

#define SL_BT_OTS_INDICATION_OVERHEAD                            4
#define SL_BT_OTS_INDICATION_HEADER                              2
#define SL_BT_OTS_INDICATION_SIZE_MAX                            7

/// Array of OTS Characteristic UUIDs
extern const uint16_t sl_bt_ots_characteristic_uuids[SL_BT_OTS_CHARACTERISTIC_UUID_COUNT];

// -----------------------------------------------------------------------------
// General type definitions

/// OTS Characteristic UUID indices
SL_ENUM(sl_bt_ots_characteristic_uuid_index) {
  SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OTS_FEATURE          = 0,
  SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_NAME          = 1,
  SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_TYPE          = 2,
  SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_SIZE          = 3,
  SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_FIRST_CREATED = 4,
  SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_LAST_MODIFIED = 5,
  SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_ID            = 6,
  SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_PROPERTIES    = 7,
  SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OACP                 = 8,
  SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OLCP                 = 9,
  SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_LIST_FILTER   = 10,
  SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_CHANGED       = 11,
  SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_INVALID              = 0xFF
};

/// Object Action Control Point Features
typedef uint32_t sl_bt_ots_oacp_features_t;

/// Object List Control Point Features
typedef uint32_t sl_bt_ots_olcp_features_t;

/// Object Transfer Service features
typedef PACKSTRUCT (struct {
  sl_bt_ots_oacp_features_t oacp_features; ///< OACP feature flags
  sl_bt_ots_olcp_features_t olcp_features; ///< OLCP feature flags
}) sl_bt_ots_features_t;

// Metadata datatypes
/// Object name
typedef PACKSTRUCT (struct {
  uint16_t length; ///< Length of the object name field
  char     *name;  ///< Object name string.
}) sl_bt_ots_object_name_t;

/// Object type variants
typedef union {
  sl_bt_uuid_16_t *sig;    ///< 16 bit UUID object type
  uuid_128        *custom; ///< 128 bit UUID object type
} sl_bt_object_type_variant_t;

/// Object type
typedef struct {
  bool                        uuid_is_sig; ///< Field that indicating if the
                                           ///  type has 16 or 128 bit length.
  uint8_t                     *uuid_data;  ///< Type data
} sl_bt_ots_object_type_t;

/// Object size
typedef PACKSTRUCT (struct {
  uint32_t current_size;   ///< Indicates the actual size of the object
  uint32_t allocated_size; ///< Contains the allocated size of the object
}) sl_bt_ots_object_size_t;

/// Object times
typedef union {
  PACKSTRUCT(struct {
    uint16_t year;                   ///< Year field of time
    uint8_t  month;                  ///< Month field of time
    uint8_t  day;                    ///< Day field of time
    uint8_t  hours;                  ///< Hour field of time
    uint8_t  minutes;                ///< Minute field of time
    uint8_t  seconds;                ///< Second field of time
  }) time;
  uint8_t data[SL_BT_OTS_TIME_SIZE]; ///< Alternative byte based access to time.
} sl_bt_ots_time_t;

/// Object first created
typedef sl_bt_ots_time_t sl_bt_ots_object_first_created_t;

/// Object last modified
typedef sl_bt_ots_time_t sl_bt_ots_object_last_modified_t;

/// Complete Object ID
typedef PACKSTRUCT (struct {
  uint8_t rfu;                                     ///< First byte is Reserved for Future Use
  uint8_t usable[SL_BT_OTS_OBJECT_ID_USABLE_SIZE]; ///< Usable data for Object ID
}) sl_bt_ots_object_id_complete_t;

/// Object ID
typedef union {
  uint8_t                        data[SL_BT_OTS_OBJECT_ID_SIZE]; ///< Data for Object ID
  sl_bt_ots_object_id_complete_t id;                             ///< Structured access of Object ID.
} sl_bt_ots_object_id_t;

/// Object properties
typedef uint32_t sl_bt_ots_object_properties_t;

/// Object metadata write event type
SL_ENUM(sl_bt_ots_object_metadata_write_event_type_t) {
  SL_BT_OTS_OBJECT_METADATA_WRITE_OBJECT_NAME          = SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_NAME,          ///< Write object name event type
  SL_BT_OTS_OBJECT_METADATA_WRITE_OBJECT_FIRST_CREATED = SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_FIRST_CREATED, ///< Write Object First Created event type
  SL_BT_OTS_OBJECT_METADATA_WRITE_OBJECT_LAST_MODIFIED = SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_LAST_MODIFIED, ///< Write Object Last Modified Created event type
  SL_BT_OTS_OBJECT_METADATA_WRITE_OBJECT_PROPERTIES    = SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_PROPERTIES     ///< Write Object Properties event type
};

/// Object metadata write event parameters
typedef union {
  sl_bt_ots_object_name_t          object_name;          ///< Name of the object
  sl_bt_ots_object_first_created_t object_first_created; ///< Time of object First Created
  sl_bt_ots_object_last_modified_t object_last_modified; ///< Time of object Last Modified
  sl_bt_ots_object_properties_t    object_properties;    ///< Object properties
} sl_bt_ots_object_metadata_write_parameters_t;

/// Object metadata write response codes
SL_ENUM(sl_bt_ots_object_metadata_write_response_code_t) {
  SL_BT_OTS_OBJECT_METADATA_WRITE_RESPONSE_CODE_SUCCESS                    = ATT_ERR_SUCCESS,                    ///< Successful write
  SL_BT_OTS_OBJECT_METADATA_WRITE_RESPONSE_CODE_WRITE_REQUEST_REJECTED     = ATT_ERR_WRITE_REQUEST_REJECTED,     ///< Write rejected
  SL_BT_OTS_OBJECT_METADATA_WRITE_RESPONSE_CODE_OBJECT_NOT_SELECTED        = ATT_ERR_OBJECT_NOT_SELECTED,        ///< No object selected
  SL_BT_OTS_OBJECT_METADATA_WRITE_RESPONSE_CODE_CONCURRENCY_LIMIT_EXCEEDED = ATT_ERR_CONCURRENCY_LIMIT_EXCEEDED, ///< Concurrency limit exceeded
  SL_BT_OTS_OBJECT_METADATA_WRITE_RESPONSE_CODE_OBJECT_NAME_ALREADY_EXISTS = ATT_ERR_OBJECT_NAME_ALREADY_EXISTS, ///< Object name already exists
  SL_BT_OTS_OBJECT_METADATA_WRITE_RESPONSE_CODE_NOT_SUPPORTED              = ATT_ERR_NOT_SUPPORTED               ///< Not supported operation
};

/// Object metadata read event type
SL_ENUM(sl_bt_ots_object_metadata_read_event_type_t) {
  SL_BT_OTS_OBJECT_METADATA_READ_OBJECT_NAME          = SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_NAME,          ///< Read Object Name event type
  SL_BT_OTS_OBJECT_METADATA_READ_OBJECT_TYPE          = SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_TYPE,          ///< Read Object Type event type
  SL_BT_OTS_OBJECT_METADATA_READ_OBJECT_SIZE          = SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_SIZE,          ///< Read Object Size event type
  SL_BT_OTS_OBJECT_METADATA_READ_OBJECT_FIRST_CREATED = SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_FIRST_CREATED, ///< Read Object First Created event type
  SL_BT_OTS_OBJECT_METADATA_READ_OBJECT_LAST_MODIFIED = SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_LAST_MODIFIED, ///< Read Object Last Modified event type
  SL_BT_OTS_OBJECT_METADATA_READ_OBJECT_ID            = SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_ID,            ///< Read Object ID event type
  SL_BT_OTS_OBJECT_METADATA_READ_OBJECT_PROPERTIES    = SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_PROPERTIES     ///< Read Object Properties event type
};

/// Metadata fields type
typedef uint8_t sl_bt_ots_metadata_fields_t;

/// Object metadata read event parameters
typedef union {
  sl_bt_ots_object_name_t          object_name;          ///< Name of the object
  sl_bt_ots_object_type_t          object_type;          ///< Type of the object
  sl_bt_ots_object_size_t          object_size;          ///< Size of the object
  sl_bt_ots_object_first_created_t object_first_created; ///< Time of object First Created
  sl_bt_ots_object_last_modified_t object_last_modified; ///< Time of object Last Modified
  sl_bt_ots_object_id_t            object_id;            ///< Object ID
  sl_bt_ots_object_properties_t    object_properties;    ///< Object properties
} sl_bt_ots_object_metadata_read_parameters_t;

/// Object metadata read response codes
SL_ENUM(sl_bt_ots_object_metadata_read_response_code_t) {
  SL_BT_OTS_OBJECT_METADATA_READ_RESPONSE_CODE_SUCCESS                    = 0x00,                               ///< Successful read
  SL_BT_OTS_OBJECT_METADATA_READ_RESPONSE_CODE_OBJECT_NOT_SELECTED        = ATT_ERR_OBJECT_NOT_SELECTED,        ///< Object is not selected
  SL_BT_OTS_OBJECT_METADATA_READ_RESPONSE_CODE_CONCURRENCY_LIMIT_EXCEEDED = ATT_ERR_CONCURRENCY_LIMIT_EXCEEDED, ///< Concurrency limit exceeded
  SL_BT_OTS_OBJECT_METADATA_READ_RESPONSE_CODE_NOT_SUPPORTED              = ATT_ERR_NOT_SUPPORTED               ///< Not supported operation
};

/// OACP opcode
SL_ENUM(sl_bt_ots_oacp_opcode_t) {
  SL_BT_OTS_OACP_OPCODE_CREATE             = 0x01, ///< OACP Create Object opcode. Client can create a new object by issuing
                                                   ///  this opcode with size and type parameters. After a successful creation,
                                                   ///  the object metadata should be set by the client.
  SL_BT_OTS_OACP_OPCODE_DELETE             = 0x02, ///< OACP Delete Object opcode. Client can delete Current Object using
                                                   ///  this opcode.
  SL_BT_OTS_OACP_OPCODE_CALCULATE_CHECKSUM = 0x03, ///< OACP Calculate Checksum opcode. Client can start checksum calculation
                                                   ///  of the Current Object on the server by issuing this opcode with
                                                   ///  offset and length parameters. The result is sent in the server response.
  SL_BT_OTS_OACP_OPCODE_EXECUTE            = 0x04, ///< OACP Execute object opcode. Client can execute the Current Object
                                                   ///  by issuing this opcode with optional user-defined parameters
  SL_BT_OTS_OACP_OPCODE_READ               = 0x05, ///< OACP Read Object opcode. Client can read the Current Object
                                                   ///  by issuing this opcode with offset and length parameters.
  SL_BT_OTS_OACP_OPCODE_WRITE              = 0x06, ///< OACP Write Object opcode. Client can write the Current object by
                                                   ///  issuing this opcode with offset, length and mode parameters.
  SL_BT_OTS_OACP_OPCODE_ABORT              = 0x07, ///< OACP Abort opcode. Client can abort read operations using this opcode.
  SL_BT_OTS_OACP_OPCODE_RESPONSE_CODE      = 0x60  ///< OACP Response opcode. It can contain checksum or execute object
                                                   ///  Parameters.
};

typedef sl_bt_ots_oacp_opcode_t sl_bt_ots_oacp_event_t;

/// OACP response code
SL_ENUM(sl_bt_ots_oacp_response_code_t) {
  SL_BT_OTS_OACP_RESPONSE_CODE_SUCCESS                 = 0x01, ///< Success.
  SL_BT_OTS_OACP_RESPONSE_CODE_OP_CODE_NOT_SUPPORTED   = 0x02, ///< Opcode is not supported by the Server.
  SL_BT_OTS_OACP_RESPONSE_CODE_INVALID_PARAMETER       = 0x03, ///< Invalid parameter sent by the Client.
  SL_BT_OTS_OACP_RESPONSE_CODE_INSUFFICIENT_RESOURCES  = 0x04, ///< Insufficient resources.
  SL_BT_OTS_OACP_RESPONSE_CODE_INVALID_OBJECT          = 0x05, ///< Current object is invalid.
  SL_BT_OTS_OACP_RESPONSE_CODE_CHANNEL_UNAVAILABLE     = 0x06, ///< L2CAP channel is not available for use.
  SL_BT_OTS_OACP_RESPONSE_CODE_UNSUPPORTED_TYPE        = 0x07, ///< Object Type is not supported by the server.
  SL_BT_OTS_OACP_RESPONSE_CODE_PROCEDURE_NOT_PERMITTED = 0x08, ///< Procedure is not permitted on the Current Object.
  SL_BT_OTS_OACP_RESPONSE_CODE_OBJECT_LOCKED           = 0x09, ///< Object is locked by the Server.
  SL_BT_OTS_OACP_RESPONSE_CODE_OPERATION_FAILED        = 0x0A, ///< Operation failed due to other reason.
};

/// OACP Create opcode parameters
typedef PACKSTRUCT (struct {
  uint32_t size;                          ///< Required size of the object
  uint8_t  type[SL_BT_OTS_UUID_SIZE_128]; ///< Required type of the object
}) sl_bt_ots_oacp_create_parameters_t;

/// OACP Calculate Checksum opcode parameters
typedef PACKSTRUCT (struct {
  uint32_t offset; ///< Offset to calculate checksum from.
  uint32_t length; ///< Length of the data from the offset.
}) sl_bt_ots_oacp_calculate_checksum_parameters_t;

/// OACP Execute opcode parameters
typedef uint8array sl_bt_ots_oacp_execute_parameters_t;

/// OACP Read opcode parameters
typedef PACKSTRUCT (struct {
  uint32_t offset; ///< Offset to read data from.
  uint32_t length; ///< Length of the data to be read.
}) sl_bt_ots_oacp_read_parameters_t;

/// Object write mode flags
typedef uint8_t sl_bt_ots_oacp_write_mode_t;

/// OACP Write opcode parameters
typedef PACKSTRUCT (struct {
  uint32_t                      offset; ///< Offset of the data to be written.
  uint32_t                      length; ///< Length of the data to be written.
  sl_bt_ots_oacp_write_mode_t   mode;   ///< Mode of write: truncation can be selected.
}) sl_bt_ots_oacp_write_parameters_t;

/// OACP parameters
typedef union {
  sl_bt_ots_oacp_create_parameters_t             create;             ///< Parameters for object creation
  sl_bt_ots_oacp_calculate_checksum_parameters_t calculate_checksum; ///< Parameters for checksum calculation
  sl_bt_ots_oacp_execute_parameters_t            execute;            ///< Parameters for object execution
  sl_bt_ots_oacp_read_parameters_t               read;               ///< Parameters for object read
  sl_bt_ots_oacp_write_parameters_t              write;              ///< Parameters for object write
} sl_bt_ots_oacp_parameters_t;

/// Data response credits
typedef uint16_t sl_bt_ots_l2cap_credit_t;

SL_ENUM(sl_bt_ots_transfer_result_t) {
  SL_BT_OTS_TRANSFER_FINISHED_RESPONSE_CODE_SUCCESS         = 0x00, ///< Successful data transfer.
  SL_BT_OTS_TRANSFER_FINISHED_RESPONSE_CODE_CHANNEL_ERROR   = 0x01, ///< Channel error occurred.
  SL_BT_OTS_TRANSFER_FINISHED_RESPONSE_CODE_CONNECTION_LOST = 0x02, ///< Disconnected during transfer.
  SL_BT_OTS_TRANSFER_FINISHED_RESPONSE_CODE_TIMEOUT         = 0x03  ///< Timeout occurred.
};

/// OACP Message
typedef PACKSTRUCT (struct {
  sl_bt_ots_oacp_opcode_t     opcode;     ///< OACP opcode
  uint8_t                     data[];     ///< Response data
}) sl_bt_ots_oacp_message_t;

/// OACP response data
typedef union {
  uint32_t checksum; ///< Checksum data for Calculate Checksum opcode response.
  PACKSTRUCT(struct {
    uint8_t len;
    uint8_t *data;
  }) execute;         ///< Execution response data for Execute Object opcode.
} sl_bt_ots_oacp_response_data_t;

/// OACP response
typedef PACKSTRUCT (struct {
  sl_bt_ots_oacp_opcode_t         response_opcode; ///< OACP response opcode
  sl_bt_ots_oacp_opcode_t         opcode;          ///< OACP opcode
  sl_bt_ots_oacp_response_code_t  response;        ///< Response parameters
  uint8_t                         data[];          ///< Response data
}) sl_bt_ots_oacp_response_message_t;

/// OACP opcode
SL_ENUM(sl_bt_ots_olcp_opcode_t) {
  SL_BT_OTS_OLCP_OPCODE_FIRST                     = 0x01, ///< OACP First opcode. Mandatory opcode.
                                                          ///  Server selects the first object in the list as Current Object.
                                                          ///  Required opcode.
                                                          ///  Server can respond with:
                                                          ///  - Success,
                                                          ///  - Operation Failed,
                                                          ///  - Too Many Objects,
                                                          ///  - No Object
  SL_BT_OTS_OLCP_OPCODE_LAST                      = 0x02, ///< OACP Last opcode. Mandatory opcode.
                                                          ///  Server selects the last object in the list as Current Object.
                                                          ///  Required opcode.
                                                          ///  Server can respond with:
                                                          ///  - Success,
                                                          ///  - Operation Failed,
                                                          ///  - Too Many Objects,
                                                          ///  - No Object
  SL_BT_OTS_OLCP_OPCODE_PREVIOUS                  = 0x03, ///< OACP Previous opcode. Mandatory opcode.
                                                          ///  Server selects the previous object in the list as Current Object.
                                                          ///  Server can respond with:
                                                          ///  - Success,
                                                          ///  - Out Of Bounds,
                                                          ///  - Operation Failed,
                                                          ///  - Too Many Objects,
                                                          ///  - No Object
  SL_BT_OTS_OLCP_OPCODE_NEXT                      = 0x04, ///< OACP Next opcode. Mandatory opcode.
                                                          ///  Server selects the next object in the list as Current Object.
                                                          ///  Server can respond with:
                                                          ///  - Success,
                                                          ///  - Out Of Bounds,
                                                          ///  - Operation Failed,
                                                          ///  - Too Many Objects,
                                                          ///  - No Object
  SL_BT_OTS_OLCP_OPCODE_GO_TO                     = 0x05, ///< OACP Go To opcode. Optional opcode.
                                                          ///  Server selects object specified by the object ID parameter
                                                          ///  as Current Object.
                                                          ///  Server can respond with:
                                                          ///  - Success,
                                                          ///  - Invalid Parameter,
                                                          ///  - Object ID Not Found,
                                                          ///  - Op Code Not Supported,
                                                          ///  - Operation Failed,
                                                          ///  - Too Many Objects,
                                                          ///  - No Object
  SL_BT_OTS_OLCP_OPCODE_ORDER                     = 0x06, ///< OACP Order opcode. Optional opcode.
                                                          ///  Server shall arrange the list of objects in order according
                                                          ///  to the value of the List Sort Order parameter.
                                                          ///  Server can respond with:
                                                          ///  - Success,
                                                          ///  - Invalid Parameter,
                                                          ///  - Op Code Not Supported,
                                                          ///  - Operation Failed,
                                                          ///  - Too Many Objects,
                                                          ///  - No Object
  SL_BT_OTS_OLCP_OPCODE_REQUEST_NUMBER_OF_OBJECTS = 0x07, ///< OACP Request Number of Objects opcode. Optional opcode.
                                                          ///  The Server shall report the total number of objects found in
                                                          ///  the list of objects.
                                                          ///  Server can respond with:
                                                          ///  - Success(extended with the number of objects as a parameter),
                                                          ///  - Op Code Not Supported,
                                                          ///  - Operation Failed,
                                                          ///  - Too Many Objects,
                                                          ///  - No Object
  SL_BT_OTS_OLCP_OPCODE_CLEAR_MARKING             = 0x08, ///< OACP Clear Marking opcode. Optional opcode.
                                                          ///  Server shall update the marking of objects so that all objects
                                                          ///  in the list of objects become unmarked.
                                                          ///  Server can respond with:
                                                          ///  - Success,
                                                          ///  - Op Code Not Supported,
                                                          ///  - Operation Failed,
                                                          ///  - Too Many Objects,
                                                          ///  - No Object
  SL_BT_OTS_OLCP_OPCODE_RESPONSE_CODE             = 0x70, ///< OACP Response code opcode. Mandatory opcode.
                                                          ///  The Server indicates the response using this opcode.
                                                          ///  The response can contain parameters, like the Number of Objects.
};

typedef sl_bt_ots_olcp_opcode_t sl_bt_ots_olcp_event_t;

/// OLCP response code
SL_ENUM(sl_bt_ots_olcp_response_code_t) {
  SL_BT_OTS_OLCP_RESPONSE_CODE_SUCCESS               = 0x01, ///< Operation was successful.
  SL_BT_OTS_OLCP_RESPONSE_CODE_OP_CODE_NOT_SUPPORTED = 0x02, ///< Opcode is not supported.
  SL_BT_OTS_OLCP_RESPONSE_CODE_INVALID_PARAMETER     = 0x03, ///< Invalid parameter.
  SL_BT_OTS_OLCP_RESPONSE_CODE_OPEATION_FAILED       = 0x04, ///< Operation failed.
  SL_BT_OTS_OLCP_RESPONSE_CODE_OUT_OF_BOUNDS         = 0x05, ///< Out of bounds (next or previous).
  SL_BT_OTS_OLCP_RESPONSE_CODE_TOO_MANY_OBJECTS      = 0x06, ///< Too many objects in the list (resource limitation).
  SL_BT_OTS_OLCP_RESPONSE_CODE_NO_OBJECT             = 0x07, ///< Zero objects in the current list.
  SL_BT_OTS_OLCP_RESPONSE_CODE_OBJECT_ID_NOT_FOUND   = 0x08, ///< No object found with the requested Object ID.
};

/// List Sort Order
SL_ENUM(sl_bt_ots_list_sort_order_t) {
  SL_BT_OTS_LIST_SORT_ORDER_NAME_ASC           = 0x01, ///< Sort the list in ascending order by Object Name.
  SL_BT_OTS_LIST_SORT_ORDER_TYPE_ASC           = 0x02, ///< Sort the list in ascending order by Object Type.
  SL_BT_OTS_LIST_SORT_ORDER_CURRENT_SIZE_ASC   = 0x03, ///< Sort the list in ascending order by Current Size.
  SL_BT_OTS_LIST_SORT_ORDER_FIRST_CREATED_ASC  = 0x04, ///< Sort the list in ascending order by First Created.
  SL_BT_OTS_LIST_SORT_ORDER_LAST_MODIFIED_ASC  = 0x05, ///< Sort the list in ascending order by Last Modified.
  SL_BT_OTS_LIST_SORT_ORDER_NAME_DESC          = 0x11, ///< Sort the list in descending order by Object Name.
  SL_BT_OTS_LIST_SORT_ORDER_TYPE_DESC          = 0x12, ///< Sort the list in descending order by Object Type.
  SL_BT_OTS_LIST_SORT_ORDER_CURRENT_SIZE_DESC  = 0x13, ///< Sort the list in descending order by Current Size.
  SL_BT_OTS_LIST_SORT_ORDER_FIRST_CREATED_DESC = 0x14, ///< Sort the list in descending order by First Created.
  SL_BT_OTS_LIST_SORT_ORDER_LAST_MODIFIED_DESC = 0x15, ///< Sort the list in descending order by Last Modified.
};

/// OLCP Go To opcode parameters
typedef PACKSTRUCT (struct {
  sl_bt_ots_object_id_t id;  ///< ID parameter
}) sl_bt_ots_olcp_go_to_parameters_t;

/// OLCP Order opcode parameters
typedef PACKSTRUCT (struct {
  sl_bt_ots_list_sort_order_t list_sort_order;  ///< List Sort Order parameter
}) sl_bt_ots_olcp_order_parameters_t;

/// OACP opcode parameters
typedef union {
  sl_bt_ots_olcp_go_to_parameters_t                  go_to;                  ///< Go To opcode parameters
  sl_bt_ots_olcp_order_parameters_t                  order;                  ///< Order opcode parameters
} sl_bt_ots_olcp_parameters_t;

/// OLCP message
typedef PACKSTRUCT (struct {
  sl_bt_ots_olcp_opcode_t     opcode;     ///< OLCP opcode
  uint8_t                     data[];     ///< OLCP opcode parameters
}) sl_bt_ots_olcp_message_t;

/// OLCP response message
typedef PACKSTRUCT (struct {
  sl_bt_ots_olcp_opcode_t              response_opcode;   ///< OLCP opcode
  sl_bt_ots_olcp_opcode_t              opcode;            ///< OLCP opcode
  sl_bt_ots_olcp_response_code_t       response;          ///< OLCP response
  uint32_t                             number_of_objects; ///< OLCP Number of Objects in response (optional)
}) sl_bt_ots_olcp_response_message_t;

/// Object List Filter
SL_ENUM(sl_bt_ots_object_list_filter_type_t) {
  SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NO_FILTER              = 0x00, ///< No filter
  SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NAME_STARTS_WITH       = 0x01, ///< Name starts with the string specified in the parameter
  SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NAME_ENDS_WITH         = 0x02, ///< Name ends with the string specified in the parameter
  SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NAME_CONTAINS          = 0x03, ///< Name contains the string specified in the parameter
  SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NAME_IS_EXACTLY        = 0x04, ///< Name is exactly the string specified in the parameter
  SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_OBJECT_TYPE            = 0x05, ///< The object type (UUID) is specified in the parameter
  SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_CREATED_BETWEEN        = 0x06, ///< The object is created between the time values specified with the parameters (inclusive)
  SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_MODIFIED_BETWEEN       = 0x07, ///< The object is modified between the time values specified with the parameters (inclusive)
  SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_CURRENT_SIZE_BETWEEN   = 0x08, ///< The current size is between the size values specified with the parameters (inclusive)
  SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_ALLOCATED_SIZE_BETWEEN = 0x09, ///< The allocated size is between the size values specified with the parameters (inclusive)
  SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_MARKED_OBJECTS         = 0x0A  ///< List only marked objects
};

/// Name filter parameters
typedef sl_bt_ots_object_name_t sl_bt_ots_object_list_filter_name_parameters_t;

/// Time filter parameters
typedef PACKSTRUCT (struct {
  sl_bt_ots_time_t from; ///< Inclusive time to contain objects from
  sl_bt_ots_time_t to;   ///< Inclusive time to contain objects to
}) sl_bt_ots_object_list_filter_time_parameters_t;

/// Size filter parameters
typedef PACKSTRUCT (struct {
  uint32_t min; ///< Inclusive size to contain objects from
  uint32_t max; ///< Inclusive size to contain objects to
}) sl_bt_ots_object_list_filter_size_parameters_t;

typedef sl_bt_ots_object_type_t sl_bt_ots_object_list_filter_type_parameters_t;

/// Object List Filter content
typedef PACKSTRUCT (struct {
  sl_bt_ots_object_list_filter_type_t filter_type;       ///< Filter type
  union {
    sl_bt_ots_object_list_filter_name_parameters_t name; ///< Name based filter parameter
    sl_bt_ots_object_list_filter_time_parameters_t time; ///< Time based filter parameter
    sl_bt_ots_object_list_filter_size_parameters_t size; ///< Size filter parameter
    sl_bt_ots_object_list_filter_type_parameters_t type; ///< Size filter parameter
  } parameters;                                          ///< Filter parameters
}) sl_bt_ots_object_list_filter_content_t;

/// Object change flags
typedef uint8_t sl_bt_ots_object_changed_flags_t;

/// Object Changed content
typedef PACKSTRUCT (struct {
  sl_bt_ots_object_changed_flags_t flags;  ///< Flags to indicate the type of change
  sl_bt_ots_object_id_t            object; ///< Object ID
}) sl_bt_ots_object_changed_content_t;

/// Directory Listing Object
typedef struct {
  uint16_t                         dls_length;    ///< DLO entry length
  sl_bt_ots_object_id_t            id;            ///< Object ID
  sl_bt_ots_object_name_t          name;          ///< Object Name
  uint8_t                          dls_flags;     ///< Flags indicating the content of the DLO entry
  sl_bt_ots_object_type_t          type;          ///< Object type
  // Optionals
  sl_bt_ots_object_size_t          size;          ///< Object size: current and allocated size (optional)
  sl_bt_ots_object_first_created_t first_created; ///< Object First Created (optional)
  sl_bt_ots_object_last_modified_t last_modified; ///< Object Last Modified (optional)
  sl_bt_ots_object_properties_t    properties;    ///< Object properties (optional)
} sl_bt_ots_object_t;

/// OTS GATT database handles
typedef PACKSTRUCT (struct {
  uint32_t service;                         ///< GATT database handle for Object Transfer Service
  union {
    uint16_t array[12];
    PACKSTRUCT(struct {
      uint16_t ots_feature;                 ///< GATT database handle for OTS Features characteristic
      uint16_t object_name;                 ///< GATT database handle for Object Name characteristic
      uint16_t object_type;                 ///< GATT database handle for Object Type characteristic
      uint16_t object_size;                 ///< GATT database handle for Object Size characteristic
      uint16_t object_first_created;        ///< GATT database handle for Object Frist Created characteristic
      uint16_t object_last_modified;        ///< GATT database handle for Object Last Modified characteristic
      uint16_t object_id;                   ///< GATT database handle for Object ID characteristic
      uint16_t object_properties;           ///< GATT database handle for Object Properties characteristic
      uint16_t object_action_control_point; ///< GATT database handle for OACP characteristic
      uint16_t object_list_control_point;   ///< GATT database handle for OLCP characteristic
      uint16_t object_list_filter;          ///< GATT database handle for Object List Filter characteristic
      uint16_t object_changed;              ///< GATT database handle for Object Changed characteristic
    }) handles;
  } characteristics;
}) sl_bt_ots_gattdb_handles_t;

/// OTS characteristic indication subscription status type

typedef union {
  PACKSTRUCT(struct {
    uint8_t oacp: 1;            ///< Indication status for OACP characteristic
    uint8_t olcp: 1;            ///< Indication status for OLCP characteristic
    uint8_t object_changed: 1;  ///< Indication status for Object Changed characteristic
  }) subscribed;
  uint8_t data;
} sl_bt_ots_subscription_status_t;

/***************************************************************************//**
 * Function prototype to compare keys (for list ordering)
 * @param[in]  key_1     Pointer to key 1
 * @param[in]  key_2     Pointer to key 2
 * @return 0 if key_1 is equal to key_2,
 *         less than 0 if key_2 < key_1 and
 *         more than 0 otherwise.
 ******************************************************************************/
typedef int (*sl_bt_ots_compare_t)(uint8_t *key_1,
                                   uint8_t *key_2);

/// Datatype for object list item
typedef PACKSTRUCT (struct {
  sl_slist_node_t node;   ///< List node field (pointer to next)
  uint8_t         *key;   ///< Pointer to list key (order key)
  uint8_t         *value; ///< Pointer to value (the Object)
}) sl_bt_ots_list_item_t;

/// Datatype for object list
typedef PACKSTRUCT (struct {
  sl_bt_ots_compare_t   compare; ///< Comparator function
  sl_bt_ots_list_item_t *list;   ///< List header
}) sl_bt_ots_list_t;

#ifdef __cplusplus
};
#endif

#endif // SL_BT_OTS_DATATYPES_H
