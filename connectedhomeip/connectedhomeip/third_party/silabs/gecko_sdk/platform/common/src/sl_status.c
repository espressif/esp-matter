/***************************************************************************//**
 * @file
 * @brief Status
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stddef.h>
#include <stdio.h>
#include "sl_status.h"
#include "sl_string.h"
#include "sl_status_string_config.h"

/*******************************************************************************
 *********************   SL_STATUS STRING COMPONENTS ***************************
 ******************************************************************************/

#define  SL_MAX_STATUS_STRING  100u

// Status code prefix
static const char *sli_status_prefix = "SL_STATUS_";

static const char *sli_status_group_name[] =
{
  "",
#if SL_STATUS_STRING_ENABLE_PLATFORM == 1
  "PLATFORM_",
  "PLATFORM_",
#else
  "",
  "",
#endif
#if SL_STATUS_STRING_ENABLE_HARDWARE == 1
  "HARDWARE_",
#else
  "",
#endif
#if SL_STATUS_STRING_ENABLE_BLUETOOTH == 1
  "BT_",
  "BT_MESH_",
#else
  "",
  "",
#endif
#if SL_STATUS_STRING_ENABLE_CAN_CANOPEN == 1
  "CAN_CANOPEN_",
#else
  "",
#endif
#if SL_STATUS_STRING_ENABLE_CONNECT == 1
  "CONNECT_",
#else
  "",
#endif
#if SL_STATUS_STRING_ENABLE_NET_SUITE == 1
  "NET_SUITE_",
#else
  "",
#endif
#if SL_STATUS_STRING_ENABLE_THREAD == 1
  "THREAD_",
#else
  "",
#endif
#if SL_STATUS_STRING_ENABLE_USB == 1
  "USB_",
#else
  "",
#endif
#if SL_STATUS_STRING_ENABLE_WIFI == 1
  "WIFI_",
#else
  "",
#endif
#if SL_STATUS_STRING_ENABLE_ZIGBEE == 1
  "ZIGBEE_",
#else
  "",
#endif
#if SL_STATUS_STRING_ENABLE_Z_WAVE == 1
  "Z_WAVE_",
#else
  "",
#endif
#if SL_STATUS_STRING_ENABLE_GECKO_OS == 1
  "GECKO_OS_",
  "GECKO_OS_",
#else
  "",
  "",
#endif
#if SL_STATUS_STRING_ENABLE_BLUETOOTH == 1
  "BT_CTRL_",
  "BT_ATT_",
  "BT_SMP_",
  "BT_MESH_FOUNDATION_",
#else
  "",
  "",
  "",
  "",
#endif
};

// Status code names (generic group)
#if SL_STATUS_STRING_ENABLE_GENERIC == 1
static const char *sli_status_group_generic[] =
{
  "OK",
  "FAIL",
  "INVALID_STATE",
  "NOT_READY",
  "BUSY",
  "IN_PROGRESS",
  "ABORT",
  "TIMEOUT",
  "PERMISSION",
  "WOULD_BLOCK",
  "IDLE",
  "IS_WAITING",
  "NONE_WAITING",
  "SUSPENDED",
  "NOT_AVAILABLE",
  "NOT_SUPPORTED",
  "INITIALIZATION",
  "NOT_INITIALIZED",
  "ALREADY_INITIALIZED",
  "DELETED",
  "ISR",
  "NETWORK_UP",
  "NETWORK_DOWN",
  "NOT_JOINED",
  "NO_BEACONS",
  "ALLOCATION_FAILED",
  "NO_MORE_RESOURCE",
  "EMPTY",
  "FULL",
  "WOULD_OVERFLOW",
  "HAS_OVERFLOWED",
  "OWNERSHIP",
  "IS_OWNER",
  "INVALID_PARAMETER",
  "NULL_POINTER",
  "INVALID_CONFIGURATION",
  "INVALID_MODE",
  "INVALID_HANDLE",
  "INVALID_TYPE",
  "INVALID_INDEX",
  "INVALID_RANGE",
  "INVALID_KEY",
  "INVALID_CREDENTIALS",
  "INVALID_COUNT",
  "INVALID_SIGNATURE",
  "NOT_FOUND",
  "ALREADY_EXISTS",
  "IO",
  "IO_TIMEOUT",
  "TRANSMIT",
  "TRANSMIT_UNDERFLOW",
  "TRANSMIT_INCOMPLETE",
  "TRANSMIT_BUSY",
  "RECEIVE",
  "OBJECT_READ",
  "OBJECT_WRITE",
  "MESSAGE_TOO_LONG",
  "EEPROM_MFG_VERSION_MISMATCH",
  "EEPROM_STACK_VERSION_MISMATCH",
  "FLASH_WRITE_INHIBITED",
  "FLASH_VERIFY_FAILED",
  "FLASH_PROGRAM_FAILED",
  "FLASH_ERASE_FAILED",
  "MAC_NO_DATA",
  "MAC_NO_ACK_RECEIVED",
  "MAC_INDIRECT_TIMEOUT",
  "MAC_UNKNOWN_HEADER_TYPE",
  "MAC_ACK_HEADER_TYPE",
  "MAC_COMMAND_TRANSMIT_FAILURE",
  "CLI_STORAGE_NVM_OPEN_ERROR",
  "SECURITY_IMAGE_CHECKSUM_ERROR",
  "SECURITY_DECRYPT_ERROR",
  "COMMAND_IS_INVALID",
  "COMMAND_TOO_LONG",
  "COMMAND_INCOMPLETE",
  "BUS_ERROR",
  "CCA_FAILURE",
  "MAC_SCANNING",
  "MAC_INCORRECT_SCAN_TYPE",
  "INVALID_CHANNEL_MASK",
  "BAD_SCAN_DURATION",
};
#endif

// Status code names (bluetooth group)
#if SL_STATUS_STRING_ENABLE_BLUETOOTH == 1
static const char *sli_status_group_bluetooth[] =
{
  "",
  "",
  "OUT_OF_BONDS",
  "UNSPECIFIED",
  "HARDWARE",
  "",
  "NO_BONDING",
  "CRYPTO",
  "DATA_CORRUPTED",
  "",
  "INVALID_SYNC_HANDLE",
  "INVALID_MODULE_ACTION",
  "RADIO",
  "L2CAP_REMOTE_DISCONNECTED",
  "L2CAP_LOCAL_DISCONNECTED",
  "L2CAP_CID_NOT_EXIST",
  "L2CAP_LE_DISCONNECTED",
  "",
  "L2CAP_FLOW_CONTROL_VIOLATED",
  "L2CAP_FLOW_CONTROL_CREDIT_OVERFLOWED",
  "L2CAP_NO_FLOW_CONTROL_CREDIT",
  "L2CAP_CONNECTION_REQUEST_TIMEOUT",
  "L2CAP_INVALID_CID",
  "L2CAP_WRONG_STATE",
  "",
  "",
  "",
  "PS_STORE_FULL",
  "PS_KEY_NOT_FOUND",
  "APPLICATION_MISMATCHED_OR_INSUFFICIENT_SECURITY",
  "APPLICATION_ENCRYPTION_DECRYPTION_ERROR"
};
#endif

// Status code names (bluetooth ctrl group)
#if SL_STATUS_STRING_ENABLE_BLUETOOTH == 1
static const char *sli_status_group_bluetooth_ctrl[] =
{
  "",
  "",
  "UNKNOWN_CONNECTION_IDENTIFIER",
  "",
  "",
  "AUTHENTICATION_FAILURE",
  "PIN_OR_KEY_MISSING",
  "MEMORY_CAPACITY_EXCEEDED",
  "CONNECTION_TIMEOUT",
  "CONNECTION_LIMIT_EXCEEDED",
  "SYNCHRONOUS_CONNECTION_LIMIT_EXCEEDED",
  "ACL_CONNECTION_ALREADY_EXISTS",
  "COMMAND_DISALLOWED",
  "CONNECTION_REJECTED_DUE_TO_LIMITED_RESOURCES",
  "CONNECTION_REJECTED_DUE_TO_SECURITY_REASONS",
  "CONNECTION_REJECTED_DUE_TO_UNACCEPTABLE_BD_ADDR",
  "CONNECTION_ACCEPT_TIMEOUT_EXCEEDED",
  "UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE",
  "INVALID_COMMAND_PARAMETERS",
  "REMOTE_USER_TERMINATED",
  "REMOTE_DEVICE_TERMINATED_CONNECTION_DUE_TO_LOW_RESOURCES",
  "REMOTE_POWERING_OFF",
  "CONNECTION_TERMINATED_BY_LOCAL_HOST",
  "REPEATED_ATTEMPTS",
  "PAIRING_NOT_ALLOWED",
  "",
  "UNSUPPORTED_REMOTE_FEATURE",
  "",
  "",
  "",
  "",
  "UNSPECIFIED_ERROR",
  "",
  "",
  "LL_RESPONSE_TIMEOUT",
  "LL_PROCEDURE_COLLISION",
  "",
  "ENCRYPTION_MODE_NOT_ACCEPTABLE",
  "LINK_KEY_CANNOT_BE_CHANGED",
  "",
  "INSTANT_PASSED",
  "PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED",
  "DIFFERENT_TRANSACTION_COLLISION",
  "",
  "",
  "",
  "CHANNEL_ASSESSMENT_NOT_SUPPORTED",
  "INSUFFICIENT_SECURITY",
  "PARAMETER_OUT_OF_MANDATORY_RANGE",
  "",
  "",
  "",
  "",
  "",
  "",
  "SIMPLE_PAIRING_NOT_SUPPORTED_BY_HOST",
  "HOST_BUSY_PAIRING",
  "CONNECTION_REJECTED_DUE_TO_NO_SUITABLE_CHANNEL_FOUND",
  "CONTROLLER_BUSY",
  "UNACCEPTABLE_CONNECTION_INTERVAL",
  "ADVERTISING_TIMEOUT",
  "CONNECTION_TERMINATED_DUE_TO_MIC_FAILURE",
  "CONNECTION_FAILED_TO_BE_ESTABLISHED",
  "MAC_CONNECTION_FAILED",
  "COARSE_CLOCK_ADJUSTMENT_REJECTED_BUT_WILL_TRY_TO_ADJUST_USING_CLOCK_DRAGGING",
  "",
  "UNKNOWN_ADVERTISING_IDENTIFIER",
  "LIMIT_REACHED",
  "OPERATION_CANCELLED_BY_HOST",
  "PACKET_TOO_LONG"
};
#endif

// Status code names (bluetooth att group)
#if SL_STATUS_STRING_ENABLE_BLUETOOTH == 1
static const char *sli_status_group_bluetooth_att[] =
{
  "",
  "INVALID_HANDLE",
  "READ_NOT_PERMITTED",
  "WRITE_NOT_PERMITTED",
  "INVALID_PDU",
  "INSUFFICIENT_AUTHENTICATION",
  "REQUEST_NOT_SUPPORTED",
  "INVALID_OFFSET",
  "INSUFFICIENT_AUTHORIZATION",
  "PREPARE_QUEUE_FULL",
  "ATT_NOT_FOUND",
  "ATT_NOT_LONG",
  "INSUFFICIENT_ENC_KEY_SIZE",
  "INVALID_ATT_LENGTH",
  "UNLIKELY_ERROR",
  "INSUFFICIENT_ENCRYPTION",
  "UNSUPPORTED_GROUP_TYPE",
  "INSUFFICIENT_RESOURCES",
  "OUT_OF_SYNC",
  "VALUE_NOT_ALLOWED",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "APPLICATION",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "WRITE_REQUEST_REJECTED",
  "CLIENT_CHARACTERISTIC_CONFIGURATION_DESCRIPTOR_IMPROPERLY_CONFIGURED",
  "PROCEDURE_ALREADY_IN_PROGRESS",
  "OUT_OF_RANGE"
};
#endif

// Status code names (bluetooth smp group)
#if SL_STATUS_STRING_ENABLE_BLUETOOTH == 1
static const char *sli_status_group_bluetooth_smp[] =
{
  "",
  "PASSKEY_ENTRY_FAILED",
  "OOB_NOT_AVAILABLE",
  "AUTHENTICATION_REQUIREMENTS",
  "CONFIRM_VALUE_FAILED",
  "PAIRING_NOT_SUPPORTED",
  "ENCRYPTION_KEY_SIZE",
  "COMMAND_NOT_SUPPORTED",
  "UNSPECIFIED_REASON",
  "REPEATED_ATTEMPTS",
  "INVALID_PARAMETERS",
  "DHKEY_CHECK_FAILED",
  "NUMERIC_COMPARISON_FAILED",
  "BREDR_PAIRING_IN_PROGRESS",
  "CROSS_TRANSPORT_KEY_DERIVATION_GENERATION_NOT_ALLOWED",
  "KEY_REJECTED"
};
#endif

// Status code names (bluetooth mesh group)
#if SL_STATUS_STRING_ENABLE_BLUETOOTH == 1
static const char *sli_status_group_bluetooth_mesh[] =
{
  "",
  "ALREADY_EXISTS",
  "DOES_NOT_EXIST",
  "LIMIT_REACHED",
  "INVALID_ADDRESS",
  "MALFORMED_DATA",
  "ALREADY_INITIALIZED",
  "NOT_INITIALIZED",
  "NO_FRIEND_OFFER",
  "PROV_LINK_CLOSED",
  "PROV_INVALID_PDU",
  "PROV_INVALID_PDU_FORMAT",
  "PROV_UNEXPECTED_PDU",
  "PROV_CONFIRMATION_FAILED",
  "PROV_OUT_OF_RESOURCES",
  "PROV_DECRYPTION_FAILED",
  "PROV_UNEXPECTED_ERROR",
  "PROV_CANNOT_ASSIGN_ADDR",
  "ADDRESS_TEMPORARILY_UNAVAILABLE",
  "ADDRESS_ALREADY_USED",
  "PUBLISH_NOT_CONFIGURED",
  "APP_KEY_NOT_BOUND"
};
#endif

// Status code names (bluetooth mesh foundation group)
#if SL_STATUS_STRING_ENABLE_BLUETOOTH == 1
static const char *sli_status_group_bluetooth_mesh_foundation[] =
{
  "",
  "INVALID_ADDRESS",
  "INVALID_MODEL",
  "INVALID_APP_KEY",
  "INVALID_NET_KEY",
  "INSUFFICIENT_RESOURCES",
  "KEY_INDEX_EXISTS",
  "INVALID_PUBLISH_PARAMS",
  "NOT_SUBSCRIBE_MODEL",
  "STORAGE_FAILURE",
  "NOT_SUPPORTED",
  "CANNOT_UPDATE",
  "CANNOT_REMOVE",
  "CANNOT_BIND",
  "TEMPORARILY_UNABLE",
  "CANNOT_SET",
  "UNSPECIFIED",
  "INVALID_BINDING"
};
#endif

// Status code names (wifi group)
#if SL_STATUS_STRING_ENABLE_WIFI == 1
static const char *sli_status_group_wifi[] =
{
  "",
  "INVALID_KEY",
  "FIRMWARE_DOWNLOAD_TIMEOUT",
  "UNSUPPORTED_MESSAGE_ID",
  "WARNING",
  "NO_PACKET_TO_RECEIVE",
  "",
  "",
  "SLEEP_GRANTED",
  "SLEEP_NOT_GRANTED",
  "",
  "",
  "",
  "",
  "",
  "",
  "SECURE_LINK_MAC_KEY_ERROR",
  "SECURE_LINK_MAC_KEY_ALREADY_BURNED",
  "SECURE_LINK_RAM_MODE_NOT_ALLOWED",
  "SECURE_LINK_FAILED_UNKNOWN_MODE",
  "SECURE_LINK_EXCHANGE_FAILED",
  "",
  "",
  "",
  "WRONG_STATE",
  "CHANNEL_NOT_ALLOWED",
  "NO_MATCHING_AP",
  "CONNECTION_ABORTED",
  "CONNECTION_TIMEOUT",
  "CONNECTION_REJECTED_BY_AP",
  "CONNECTION_AUTH_FAILURE",
  "RETRY_EXCEEDED",
  "TX_LIFETIME_EXCEEDED"
};
#endif

// Table of pointers to groups status codes
static const char **sli_status_group[] =
{
#if SL_STATUS_STRING_ENABLE_GENERIC == 1
  sli_status_group_generic,
#else
  NULL,
#endif
  NULL,
  NULL,
  NULL,
#if SL_STATUS_STRING_ENABLE_BLUETOOTH == 1
  sli_status_group_bluetooth,
  sli_status_group_bluetooth_mesh,
#else
  NULL,
  NULL,
#endif
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
#if SL_STATUS_STRING_ENABLE_WIFI == 1
  sli_status_group_wifi,
#else
  NULL,
#endif
  NULL,
  NULL,
  NULL,
  NULL,
#if SL_STATUS_STRING_ENABLE_BLUETOOTH == 1
  sli_status_group_bluetooth_ctrl,
  sli_status_group_bluetooth_att,
  sli_status_group_bluetooth_smp,
  sli_status_group_bluetooth_mesh_foundation,
#else
  NULL,
  NULL,
  NULL,
  NULL
#endif
};

// Number of groups for validation
static const uint8_t sli_status_group_number = sizeof(sli_status_group) / sizeof(char **);

// Number of strings per group for validation
// Note: (1) The error code range goes from 0x00 - 0xFF, for a total of 256 possible values.
//           To be able to store the length value on 8 bits, we actually store (length - 1).
//           This amounts to store the maximum error code value possible for the given group.
//       (2) When the number of string in the group is 1, the value 0u will be store,
//           indicating the only error code possible is 0x00. But 0u value is also used when
//           the group is disabled. Therefore, we should not rely on this table to know if a group
//           is enabled or not, but instead validate with the 'sli_status_group' array.
static const uint8_t sli_status_group_table_length[] =
{
#if SL_STATUS_STRING_ENABLE_GENERIC == 1
  (sizeof(sli_status_group_generic) / sizeof(char *) - 1),
#else
  0u,
#endif
  0u,
  0u,
  0u,
#if SL_STATUS_STRING_ENABLE_BLUETOOTH == 1
  (sizeof(sli_status_group_bluetooth) / sizeof(char *) - 1),
  (sizeof(sli_status_group_bluetooth_mesh) / sizeof(char *) - 1),
#else
  0u,
  0u,
#endif
  0u,
  0u,
  0u,
  0u,
  0u,
#if SL_STATUS_STRING_ENABLE_WIFI == 1
  (sizeof(sli_status_group_wifi) / sizeof(char *) - 1),
#else
  0u,
#endif
  0u,
  0u,
  0u,
  0u,
#if SL_STATUS_STRING_ENABLE_BLUETOOTH == 1
  (sizeof(sli_status_group_bluetooth_ctrl) / sizeof(char *) - 1),
  (sizeof(sli_status_group_bluetooth_att) / sizeof(char *) - 1),
  (sizeof(sli_status_group_bluetooth_smp) / sizeof(char *) - 1),
  (sizeof(sli_status_group_bluetooth_mesh_foundation) / sizeof(char *) - 1)
#else
  0u,
  0u,
  0u,
  0u
#endif
};

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Gets string associated to error code.
 ******************************************************************************/
int32_t sl_status_get_string_n(sl_status_t status, char *buffer, uint32_t buffer_length)
{
  int32_t cnt = 0;
  uint8_t group_number = (status & SL_STATUS_SPACE_MASK) >> 8u;
  uint8_t error_number = status & (sl_status_t)0xFF;
  bool error_status;

  if (group_number >= sli_status_group_number) {
    return cnt;
  }

  if (sli_status_group[group_number] == NULL) {
    uint32_t status_hex = status & (SL_STATUS_SPACE_MASK | (sl_status_t)0xFF);
    cnt = snprintf(buffer, buffer_length, "%s0x%04lX", sli_status_prefix, status_hex);
  } else {
    if (error_number > sli_status_group_table_length[group_number]) {
      return cnt;
    }

    error_status = sl_str_is_empty(sli_status_group[group_number][error_number]);

    if (!error_status) {
      cnt = snprintf(buffer, buffer_length, "%s%s%s", sli_status_prefix,
                     sli_status_group_name[group_number],
                     sli_status_group[group_number][error_number]);
    }
  }

  return cnt;
}

/***************************************************************************//**
 * Prints string associated to error code
 ******************************************************************************/
void sl_status_print(sl_status_t status)
{
  uint8_t group_number = (status & SL_STATUS_SPACE_MASK) >> 8u;
  uint8_t error_number = status & (sl_status_t)0xFF;
  bool error_status;

  if (group_number >= sli_status_group_number) {
    return;
  }

  if (sli_status_group[group_number] == NULL) {
    uint32_t status_hex = status & (SL_STATUS_SPACE_MASK | (sl_status_t)0xFF);
    printf("%s0x%04lX", sli_status_prefix, status_hex);
  } else {
    if (error_number > sli_status_group_table_length[group_number]) {
      return;
    }

    error_status = sl_str_is_empty(sli_status_group[group_number][error_number]);

    if (!error_status) {
      printf("%s%s%s", sli_status_prefix,
             sli_status_group_name[group_number],
             sli_status_group[group_number][error_number]);
    }
  }
}
