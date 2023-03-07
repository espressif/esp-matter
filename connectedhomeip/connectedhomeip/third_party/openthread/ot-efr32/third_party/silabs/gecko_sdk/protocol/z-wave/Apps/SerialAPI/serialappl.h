/**
 * @file
 * Header file for Serial API implementation.
 *
 * Contains various application definitions and SerialAPI
 * functionality support definitions.
 *
 * @copyright 2019 Silicon Laboratories Inc.
 */
#ifndef _SERIALAPPL_H_
#define _SERIALAPPL_H_
#include "config_app.h"

#include <ZW_typedefs.h>

#ifndef UNIT_TEST
/* Z-Wave library functionality support definitions */
#ifdef ZW_SLAVE
#include <slave_supported_func.h>
#else
#include <controller_supported_func.h>
#endif
#endif /*#ifndef UNIT_TEST*/

#include <ZW_SerialAPI.h>

#include <ZW.h>

#ifdef ZW_SLAVE
#include <ZW_slave_api.h>
#endif
#ifdef ZW_SECURITY_PROTOCOL
#include <ZW_TransportSecProtocol.h>
#endif

#ifdef ZW_CONTROLLER
#include <ZW_controller_api.h>
#endif

/* Serial API application manufacturer_id */
#define SERIALAPI_MANUFACTURER_ID1           (uint8_t)((APP_MANUFACTURER_ID & 0xFF00) >> 8) /* MSB */
#define SERIALAPI_MANUFACTURER_ID2           (uint8_t)( APP_MANUFACTURER_ID & 0x00FF)       /* LSB */
/* Serial API application manufacturer product type */
#define SERIALAPI_MANUFACTURER_PRODUCT_TYPE1 (uint8_t)((PRODUCT_TYPE_ID & 0xFF00) >> 8)     /* MSB */
#define SERIALAPI_MANUFACTURER_PRODUCT_TYPE2 (uint8_t) (PRODUCT_TYPE_ID & 0x00FF)           /* LSB */
/* Serial API application manufacturer product id */
#define SERIALAPI_MANUFACTURER_PRODUCT_ID1   (uint8_t)((APP_PRODUCT_ID & 0xFF00) >> 8)      /* MSB */
#define SERIALAPI_MANUFACTURER_PRODUCT_ID2   (uint8_t) (APP_PRODUCT_ID & 0x00FF)            /* LSB */

/* Serial API version */
#define SERIAL_API_VER 9

/* Max number of times a frame will be transmitted to PC */
#define MAX_SERIAL_RETRY 3

/* Number of bytes in a homeID */
#define HOMEID_LENGTH 4

/* Max number of nodes in a multi cast (group) */
#define MAX_GROUP_NODES 64

/* Macro for accessing the byte in byte_array at the index indx */
#define BYTE_IN_AR(byte_array, indx) (*(byte_array + indx))

/* Macro for getting HIGH uint8_t in wVar uint16_t variable */
#define BYTE_GET_HIGH_BYTE_IN_WORD(wVar) *((uint8_t*)&wVar)

/* Macro for getting LOW uint8_t in wVar uint16_t variable */
#define BYTE_GET_LOW_BYTE_IN_WORD(wVar) *((uint8_t*)&wVar + 1)

/* Macro for setting HIGH uint8_t and LOW uint8_t in wVar uint16_t variable */
#define WORD_SET_HIGH_LOW_BYTES(wVar, bHIGHByte, bLOWByte) BYTE_GET_HIGH_BYTE_IN_WORD(wVar) = bHIGHByte; \
                                                           BYTE_GET_LOW_BYTE_IN_WORD(wVar)  = bLOWByte

/* States for ApplicationPoll function */
enum
{
  stateStartup,
  stateIdle,
  stateTxSerial,
  stateFrameParse,
  stateCallbackTxSerial,
  stateCommandTxSerial,
  stateAppSuspend
};

/* States for FUNC_ID_NVM_BACKUP_RESTORE operation */
typedef enum
{
  NVMBackupRestoreOperationOpen,
  NVMBackupRestoreOperationRead,
  NVMBackupRestoreOperationWrite,
  NVMBackupRestoreOperationClose
} eNVMBackupRestoreOperation;

/* Return values for FUNC_ID_NVM_BACKUP_RESTORE operation */
typedef enum
{
  NVMBackupRestoreReturnValueOK = false,                /* Everything is OK, so far */
  NVMBackupRestoreReturnValueError = true,              /* Non specific error */
  NVMBackupRestoreReturnValueOperationMismatch,         /* Error mixing read and write */
  NVMBackupRestoreReturnValueOperationDisturbed,        /* Error read operation disturbed by other write */
  NVMBackupRestoreReturnValueEOF = EOF                  /* Not really an error. Just an indication of EndOfFile */
} eNVMBackupRestoreReturnValue;

#ifndef ZW_SECURITY_PROTOCOL
/* params used by ApplicationNodeInformation */
#define APPL_NODEPARM_MAX       35
#endif
#define APPL_SLAVENODEPARM_MAX  APPL_NODEPARM_MAX


typedef enum _E_SERIALAPI_SET_LEARN_MODE_
{
  SERIALPI_SET_LEARN_MODE_ZW_SET_LEARN_MODE_DISABLE           = ZW_SET_LEARN_MODE_DISABLE,
  SERIALPI_SET_LEARN_MODE_ZW_SET_LEARN_MODE_CLASSIC           = ZW_SET_LEARN_MODE_CLASSIC,
  SERIALPI_SET_LEARN_MODE_ZW_SET_LEARN_MODE_NWI               = ZW_SET_LEARN_MODE_NWI,
  SERIALPI_SET_LEARN_MODE_ZW_SET_LEARN_MODE_NWE               = ZW_SET_LEARN_MODE_NWE,
  SERIALPI_SET_LEARN_MODE_ZW_SET_LEARN_MODE_MAX               = SERIALPI_SET_LEARN_MODE_ZW_SET_LEARN_MODE_NWE,

  /* slave_learn_plus/ctrl_learn extensions */
  SERIALPI_SET_LEARN_MODE_LEARN_PLUS_OFFSET                   = 0x80,

} E_SERIALAPI_SET_LEARN_MODE;

#ifdef ZW_SLAVE_ROUTING
/* SerialAPI only used state - used when ZW_RequestNodeInfo transmit fails */
/* It is then assumed that the destination node did not receive the request. */
#define UPDATE_STATE_NODE_INFO_REQ_FAILED   0x81
#endif

/* SerialAPI functionality support definitions */
#define SUPPORT_SEND_DATA_TIMING                        1
/* Definitions for SerialAPI startup */
typedef enum
{
  SERIALAPI_CONFIG_STARTUP_NOTIFICATION_ENABLED = 1,
  SERIALAPI_CONFIG_UNDEFINED = 0xFE
} SERIALAPI_CONFIG_T;

#if defined(NUNIT_TEST) && !defined(ZW_CONTROLLER_BRIDGE)
#define SUPPORT_ZW_NUNIT                                1
#else
#define SUPPORT_ZW_NUNIT                                0
#endif

#ifdef PORT_STATUS
#define SUPPORT_ZW_PORT_STATUS                          1
#else
#define SUPPORT_ZW_PORT_STATUS                          0
#endif
#define SUPPORT_ZW_SET_SECURITY_S0_NETWORK_KEY          0  /*deprecated*/
/* Enable support for SerialAPI Startup Notification */
#define SUPPORT_SERIAL_API_STARTUP_NOTIFICATION         1

/* Security in Protocol SerialAPI functionality support definitions */
#ifdef ZW_SECURITY_PROTOCOL
#define SUPPORT_APPLICATION_SECURITY_EVENT              0
#define SUPPORT_SERIAL_API_APPL_NODE_INFORMATION_CMD_CLASSES  1
#if (SUPPORT_ZW_GET_SECURITY_KEYS | \
     SUPPORT_ZW_SET_SECURITY_S0_NETWORK_KEY | \
     SUPPORT_ZW_GET_SECURITY_S2_PUBLIC_DSK | \
     SUPPORT_ZW_SET_SECURITY_S2_CRITICAL_NODE_ID)
#define SUPPORT_ZW_SECURITY_SETUP                       1
#else
#define SUPPORT_ZW_SECURITY_SETUP                       0
#endif
#else  /*#ifdef ZW_SECURITY_PROTOCOL*/
#define SUPPORT_SERIAL_API_APPL_NODE_INFORMATION_CMD_CLASSES  0
#define SUPPORT_ZW_SECURITY_SETUP                       0
#define SUPPORT_APPLICATION_SECURITY_EVENT              0
#endif

/* Common SerialAPI functionality support definitions */
#define SUPPORT_SERIAL_API_APPL_NODE_INFORMATION        1

#define SUPPORT_SERIAL_API_GET_CAPABILITIES             1
#define SUPPORT_SERIAL_API_SOFT_RESET                   1

#define SUPPORT_SERIAL_API_POWER_MANAGEMENT             0
#define SUPPORT_SERIAL_API_READY                        0

#define SUPPORT_SERIAL_API_EXT                          1
#ifdef ZW_SECURITY_PROTOCOL
/* Only libraries with SECURITY buildin should supports this (slave_enhanced_232) */
#define SUPPORT_SERIAL_API_APPL_NODE_INFORMATION_CMD_CLASSES  1
#else
#define SUPPORT_SERIAL_API_APPL_NODE_INFORMATION_CMD_CLASSES  0
#endif

#ifdef ZW_ENABLE_RTC
#define SUPPORT_CLOCK_SET                               1
#define SUPPORT_CLOCK_GET                               1
#define SUPPORT_CLOCK_CMP                               1
#define SUPPORT_RTC_TIMER_CREATE                        1
#define SUPPORT_RTC_TIMER_READ                          1
#define SUPPORT_RTC_TIMER_DELETE                        1
#define SUPPORT_RTC_TIMER_CALL                          1
#else
#define SUPPORT_CLOCK_SET                               0
#define SUPPORT_CLOCK_GET                               0
#define SUPPORT_CLOCK_CMP                               0
#define SUPPORT_RTC_TIMER_CREATE                        0
#define SUPPORT_RTC_TIMER_READ                          0
#define SUPPORT_RTC_TIMER_DELETE                        0
#define SUPPORT_RTC_TIMER_CALL                          0
#endif

#define SUPPORT_ZW_AUTO_PROGRAMMING                     1

#ifdef TIMER_SUPPORT
#define SUPPORT_TIMER_START                             1
#define SUPPORT_TIMER_RESTART                           1
#define SUPPORT_TIMER_CANCEL                            1
#define SUPPORT_TIMER_CALL                              1
#else
#define SUPPORT_TIMER_START                             0
#define SUPPORT_TIMER_RESTART                           0
#define SUPPORT_TIMER_CANCEL                            0
#define SUPPORT_TIMER_CALL                              0
#endif

#if defined(NUNIT_TEST) && !defined(ZW_CONTROLLER_BRIDGE)
#define SUPPORT_ZW_NUNIT                                1
#else
#define SUPPORT_ZW_NUNIT                                0
#endif

#ifdef PORT_STATUS
#define SUPPORT_ZW_PORT_STATUS                          1
#else
#define SUPPORT_ZW_PORT_STATUS                          0
#endif
/* ZW_EnableSUC() no longer exists in the library */

/* */
#define SUPPORT_SERIAL_API_GET_APPL_HOST_MEMORY_OFFSET  0


#define FID_BM_OFFS(FUNCID) ((FUNCID - 1) / 8)
#define FID_BM(FUNCID) (1 << ((FUNCID - 1) & 0x07))
#define CAP_FID(OFFSET, FUNCID) (((FUNCID != 0) && (FID_BM_OFFS(FUNCID) == OFFSET)) ? FID_BM(FUNCID) : 0)
#ifdef ZW_CONTROLLER_SINGLE
#define CAP_BM(OFFSET) (CAP_FID(OFFSET, (SUPPORT_SERIAL_API_POWER_MANAGEMENT == 0) ? 0 : FUNC_ID_SERIAL_API_POWER_MANAGEMENT) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_READY == 0) ? 0 : FUNC_ID_SERIAL_API_READY) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_GET_INIT_DATA == 0) ? 0 : FUNC_ID_SERIAL_API_GET_INIT_DATA) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_APPL_NODE_INFORMATION == 0) ? 0 : FUNC_ID_SERIAL_API_APPL_NODE_INFORMATION) | \
                        CAP_FID(OFFSET, (SUPPORT_APPLICATION_COMMAND_HANDLER_BRIDGE == 0) ? 0 : FUNC_ID_APPLICATION_COMMAND_HANDLER_BRIDGE) | \
                        CAP_FID(OFFSET, (SUPPORT_APPLICATION_COMMAND_HANDLER == 0) ? 0 : FUNC_ID_APPLICATION_COMMAND_HANDLER) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_CONTROLLER_CAPABILITIES == 0) ? 0 : FUNC_ID_ZW_GET_CONTROLLER_CAPABILITIES) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_SET_TIMEOUTS == 0) ? 0 : FUNC_ID_SERIAL_API_SET_TIMEOUTS) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_GET_CAPABILITIES == 0) ? 0 : FUNC_ID_SERIAL_API_GET_CAPABILITIES) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_SOFT_RESET == 0) ? 0 : FUNC_ID_SERIAL_API_SOFT_RESET) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_STARTUP_NOTIFICATION == 0) ? 0 : FUNC_ID_SERIAL_API_STARTED) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_SETUP == 0) ? 0 : FUNC_ID_SERIAL_API_SETUP) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_APPL_NODE_INFORMATION_CMD_CLASSES == 0) ? 0 : FUNC_ID_SERIAL_API_APPL_NODE_INFORMATION_CMD_CLASSES) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_PROTOCOL_VERSION == 0) ? 0 : FUNC_ID_ZW_GET_PROTOCOL_VERSION) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_RF_RECEIVE_MODE == 0) ? 0 : FUNC_ID_ZW_SET_RF_RECEIVE_MODE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_SLEEP_MODE == 0) ? 0 : FUNC_ID_ZW_SET_SLEEP_MODE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_NODE_INFORMATION == 0) ? 0 : FUNC_ID_ZW_SEND_NODE_INFORMATION) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_DATA == 0) ? 0 : FUNC_ID_ZW_SEND_DATA) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_DATA_BRIDGE == 0) ? 0 : FUNC_ID_ZW_SEND_DATA_BRIDGE)|\
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_DATA_EX == 0) ? 0 : FUNC_ID_ZW_SEND_DATA_EX) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_DATA_META_BRIDGE == 0) ? 0 : FUNC_ID_ZW_SEND_DATA_META_BRIDGE)|\
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_DATA_MULTI == 0) ? 0 : FUNC_ID_ZW_SEND_DATA_MULTI) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_DATA_MULTI_BRIDGE == 0) ? 0 : FUNC_ID_ZW_SEND_DATA_MULTI_BRIDGE)|\
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_DATA_MULTI_EX == 0) ? 0 : FUNC_ID_ZW_SEND_DATA_MULTI_EX) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SECURITY_SETUP == 0) ? 0 : FUNC_ID_ZW_SECURITY_SETUP) | \
                        CAP_FID(OFFSET, (SUPPORT_APPLICATION_SECURITY_EVENT == 0) ? 0 : FUNC_ID_APPLICATION_SECURITY_EVENT) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_VERSION == 0) ? 0 : FUNC_ID_ZW_GET_VERSION) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_DATA_ABORT == 0) ? 0 : FUNC_ID_ZW_SEND_DATA_ABORT) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_RF_POWER_LEVEL_SET == 0) ? 0 : FUNC_ID_ZW_RF_POWER_LEVEL_SET) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_RF_POWER_LEVEL_GET == 0) ? 0 : FUNC_ID_ZW_RF_POWER_LEVEL_GET) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_TYPE_LIBRARY == 0) ? 0 : FUNC_ID_ZW_TYPE_LIBRARY) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_PROTOCOL_STATUS == 0) ? 0 : FUNC_ID_ZW_GET_PROTOCOL_STATUS) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_WATCHDOG_ENABLE == 0) ? 0 : FUNC_ID_ZW_WATCHDOG_ENABLE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_WATCHDOG_DISABLE == 0) ? 0 : FUNC_ID_ZW_WATCHDOG_DISABLE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_WATCHDOG_KICK == 0) ? 0 : FUNC_ID_ZW_WATCHDOG_KICK) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_WATCHDOG_START == 0) ? 0 : FUNC_ID_ZW_WATCHDOG_START)|\
                        CAP_FID(OFFSET, (SUPPORT_ZW_WATCHDOG_STOP == 0) ? 0 : FUNC_ID_ZW_WATCHDOG_STOP)|\
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_WUT_TIMEOUT == 0) ? 0 : FUNC_ID_ZW_SET_WUT_TIMEOUT) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_IS_WUT_KICKED == 0) ? 0 : FUNC_ID_ZW_IS_WUT_KICKED) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_EXT_INT_LEVEL == 0) ? 0 : FUNC_ID_ZW_SET_EXT_INT_LEVEL) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_DATA_META == 0) ? 0 : FUNC_ID_ZW_SEND_DATA_META) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_TEST_FRAME == 0) ? 0 : FUNC_ID_ZW_SEND_TEST_FRAME) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_RANDOM == 0) ? 0 : FUNC_ID_ZW_GET_RANDOM) | \
                        CAP_FID(OFFSET, (SUPPORT_MEMORY_GET_ID == 0) ? 0 : FUNC_ID_MEMORY_GET_ID) | \
                        CAP_FID(OFFSET, (SUPPORT_MEMORY_GET_BYTE == 0) ? 0 : FUNC_ID_MEMORY_GET_BYTE) | \
                        CAP_FID(OFFSET, (SUPPORT_MEMORY_PUT_BYTE == 0) ? 0 : FUNC_ID_MEMORY_PUT_BYTE) | \
                        CAP_FID(OFFSET, (SUPPORT_MEMORY_GET_BUFFER == 0) ? 0 : FUNC_ID_MEMORY_GET_BUFFER) | \
                        CAP_FID(OFFSET, (SUPPORT_MEMORY_PUT_BUFFER == 0) ? 0 : FUNC_ID_MEMORY_PUT_BUFFER) | \
                        CAP_FID(OFFSET, (SUPPORT_NVM_BACKUP_RESTORE == 0) ? 0 : FUNC_ID_NVM_BACKUP_RESTORE) | \
                        CAP_FID(OFFSET, (SUPPORT_NVM_GET_ID == 0) ? 0 : FUNC_ID_NVM_GET_ID) | \
                        CAP_FID(OFFSET, (SUPPORT_NVM_EXT_READ_LONG_BYTE == 0) ? 0 : FUNC_ID_NVM_EXT_READ_LONG_BYTE) | \
                        CAP_FID(OFFSET, (SUPPORT_NVM_EXT_WRITE_LONG_BYTE == 0) ? 0 : FUNC_ID_NVM_EXT_WRITE_LONG_BYTE) | \
                        CAP_FID(OFFSET, (SUPPORT_NVM_EXT_READ_LONG_BUFFER == 0) ? 0 : FUNC_ID_NVM_EXT_READ_LONG_BUFFER) | \
                        CAP_FID(OFFSET, (SUPPORT_NVM_EXT_WRITE_LONG_BUFFER == 0) ? 0 : FUNC_ID_NVM_EXT_WRITE_LONG_BUFFER) | \
                        CAP_FID(OFFSET, (SUPPORT_CLOCK_SET == 0) ? 0 : FUNC_ID_CLOCK_SET) | \
                        CAP_FID(OFFSET, (SUPPORT_CLOCK_GET == 0) ? 0 : FUNC_ID_CLOCK_GET) | \
                        CAP_FID(OFFSET, (SUPPORT_CLOCK_CMP == 0) ? 0 : FUNC_ID_CLOCK_CMP) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_NVR_GET_VALUE == 0) ? 0 : FUNC_ID_NVR_GET_VALUE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_AUTO_PROGRAMMING == 0) ? 0 : FUNC_ID_AUTO_PROGRAMMING) | \
                        CAP_FID(OFFSET, (SUPPORT_PWR_SETSTOPMODE == 0) ? 0 : FUNC_ID_PWR_SETSTOPMODE) | \
                        CAP_FID(OFFSET, (SUPPORT_PWR_CLK_PD == 0) ? 0 : FUNC_ID_PWR_CLK_PD) | \
                        CAP_FID(OFFSET, (SUPPORT_PWR_CLK_PUP == 0) ? 0 : FUNC_ID_PWR_CLK_PUP) | \
                        CAP_FID(OFFSET, (SUPPORT_PWR_SELECT_CLK == 0) ? 0 : FUNC_ID_PWR_SELECT_CLK) | \
                        CAP_FID(OFFSET, (SUPPORT_RTC_TIMER_CREATE == 0) ? 0 : FUNC_ID_RTC_TIMER_CREATE) | \
                        CAP_FID(OFFSET, (SUPPORT_RTC_TIMER_READ == 0) ? 0 : FUNC_ID_RTC_TIMER_READ) | \
                        CAP_FID(OFFSET, (SUPPORT_RTC_TIMER_DELETE == 0) ? 0 : FUNC_ID_RTC_TIMER_DELETE) | \
                        CAP_FID(OFFSET, (SUPPORT_RTC_TIMER_CALL == 0) ? 0 : FUNC_ID_RTC_TIMER_CALL) | \
                        CAP_FID(OFFSET, (SUPPORT_FUNC_ID_CLEAR_TX_TIMERS == 0) ? 0 : FUNC_ID_CLEAR_TX_TIMERS) | \
                        CAP_FID(OFFSET, (SUPPORT_FUNC_ID_GET_TX_TIMERS == 0) ? 0 : FUNC_ID_GET_TX_TIMERS) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_CLEAR_NETWORK_STATS == 0) ? 0 : FUNC_ID_ZW_CLEAR_NETWORK_STATS) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_NETWORK_STATS == 0) ? 0 : FUNC_ID_ZW_GET_NETWORK_STATS) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_BACKGROUND_RSSI == 0) ? 0 : FUNC_ID_ZW_GET_BACKGROUND_RSSI) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_REMOVE_NODE_ID_FROM_NETWORK == 0) ? 0 : FUNC_ID_ZW_REMOVE_NODE_ID_FROM_NETWORK) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_LEARN_NODE_STATE == 0) ? 0 : FUNC_ID_ZW_SET_LEARN_NODE_STATE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_NODE_PROTOCOL_INFO == 0) ? 0 : FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_DEFAULT == 0) ? 0 : FUNC_ID_ZW_SET_DEFAULT) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_NEW_CONTROLLER == 0) ? 0 : FUNC_ID_ZW_NEW_CONTROLLER) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_REPLICATION_COMMAND_COMPLETE == 0) ? 0 : FUNC_ID_ZW_REPLICATION_COMMAND_COMPLETE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_REPLICATION_SEND_DATA == 0) ? 0 : FUNC_ID_ZW_REPLICATION_SEND_DATA) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_ASSIGN_RETURN_ROUTE == 0) ? 0 : FUNC_ID_ZW_ASSIGN_RETURN_ROUTE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_DELETE_RETURN_ROUTE == 0) ? 0 : FUNC_ID_ZW_DELETE_RETURN_ROUTE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_REQUEST_NODE_NEIGHBOR_UPDATE == 0) ? 0 : FUNC_ID_ZW_REQUEST_NODE_NEIGHBOR_UPDATE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_NEIGHBOR_COUNT == 0) ? 0 : FUNC_ID_ZW_GET_NEIGHBOR_COUNT) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_ARE_NODES_NEIGHBOURS == 0) ? 0 : FUNC_ID_ZW_ARE_NODES_NEIGHBOURS) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_APPLICATION_CONTROLLER_UPDATE == 0) ? 0 : FUNC_ID_ZW_APPLICATION_UPDATE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_ADD_NODE_TO_NETWORK == 0) ? 0 : FUNC_ID_ZW_ADD_NODE_TO_NETWORK) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_REMOVE_NODE_FROM_NETWORK == 0) ? 0 : FUNC_ID_ZW_REMOVE_NODE_FROM_NETWORK) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_CREATE_NEW_PRIMARY == 0) ? 0 : FUNC_ID_ZW_CREATE_NEW_PRIMARY) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_CONTROLLER_CHANGE == 0) ? 0 : FUNC_ID_ZW_CONTROLLER_CHANGE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_REQUEST_NODE_INFO == 0) ? 0 : FUNC_ID_ZW_REQUEST_NODE_INFO) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_REMOVE_FAILED_NODE_ID == 0) ? 0 : FUNC_ID_ZW_REMOVE_FAILED_NODE_ID) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_IS_FAILED_NODE_ID == 0) ? 0 : FUNC_ID_ZW_IS_FAILED_NODE_ID) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_REPLACE_FAILED_NODE == 0) ? 0 : FUNC_ID_ZW_REPLACE_FAILED_NODE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_ROUTING_MAX == 0) ? 0 : FUNC_ID_ZW_GET_ROUTING_MAX) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_ROUTING_MAX == 0) ? 0 : FUNC_ID_ZW_SET_ROUTING_MAX) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_IS_PRIMARY_CTRL == 0) ? 0 : FUNC_ID_ZW_IS_PRIMARY_CTRL) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_AES_ECB == 0) ? 0 : FUNC_ID_ZW_AES_ECB) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_APPLICATION_UPDATE == 0) ? 0 : FUNC_ID_ZW_APPLICATION_UPDATE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_LEARN_MODE == 0) ? 0 : FUNC_ID_ZW_SET_LEARN_MODE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_EXPLORE_REQUEST_INCLUSION == 0) ? 0 : FUNC_ID_ZW_EXPLORE_REQUEST_INCLUSION) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_EXPLORE_REQUEST_EXCLUSION == 0) ? 0 : FUNC_ID_ZW_EXPLORE_REQUEST_EXCLUSION) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_ASSIGN_SUC_RETURN_ROUTE == 0) ? 0 : FUNC_ID_ZW_ASSIGN_SUC_RETURN_ROUTE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_ENABLE_SUC == 0) ? 0 : FUNC_ID_ZW_ENABLE_SUC) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_REQUEST_NETWORK_UPDATE == 0) ? 0 : FUNC_ID_ZW_REQUEST_NETWORK_UPDATE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_SUC_NODE_ID == 0) ? 0 : FUNC_ID_ZW_SET_SUC_NODE_ID) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_DELETE_SUC_RETURN_ROUTE == 0) ? 0 : FUNC_ID_ZW_DELETE_SUC_RETURN_ROUTE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_SUC_NODE_ID == 0) ? 0 : FUNC_ID_ZW_GET_SUC_NODE_ID) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_SUC_ID == 0) ? 0 : FUNC_ID_ZW_SEND_SUC_ID) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_ASSIGN_PRIORITY_SUC_RETURN_ROUTE == 0) ? 0 : FUNC_ID_ZW_ASSIGN_PRIORITY_SUC_RETURN_ROUTE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_ASSIGN_PRIORITY_RETURN_ROUTE == 0) ? 0 : FUNC_ID_ZW_ASSIGN_PRIORITY_RETURN_ROUTE) | \
                        CAP_FID(OFFSET, (SUPPORT_TIMER_START == 0) ? 0 : FUNC_ID_TIMER_START) | \
                        CAP_FID(OFFSET, (SUPPORT_TIMER_RESTART == 0) ? 0 : FUNC_ID_TIMER_RESTART) | \
                        CAP_FID(OFFSET, (SUPPORT_TIMER_CANCEL == 0) ? 0 : FUNC_ID_TIMER_CANCEL) | \
                        CAP_FID(OFFSET, (SUPPORT_TIMER_CALL == 0) ? 0 : FUNC_ID_TIMER_CALL) | \
                        CAP_FID(OFFSET, (SUPPORT_GET_ROUTING_TABLE_LINE == 0) ? 0 : FUNC_ID_GET_ROUTING_TABLE_LINE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_ROUTING_INFO == 0) ? 0 : FUNC_ID_ZW_SET_ROUTING_INFO) | \
                        CAP_FID(OFFSET, (SUPPORT_GET_TX_COUNTER == 0) ? 0 : FUNC_ID_GET_TX_COUNTER) | \
                        CAP_FID(OFFSET, (SUPPORT_RESET_TX_COUNTER == 0) ? 0 : FUNC_ID_RESET_TX_COUNTER) | \
                        CAP_FID(OFFSET, (SUPPORT_STORE_NODEINFO == 0) ? 0 : FUNC_ID_STORE_NODEINFO) | \
                        CAP_FID(OFFSET, (SUPPORT_STORE_HOMEID == 0) ? 0 : FUNC_ID_STORE_HOMEID) | \
                        CAP_FID(OFFSET, (SUPPORT_LOCK_ROUTE_RESPONSE == 0) ? 0 : FUNC_ID_LOCK_ROUTE_RESPONSE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_PRIORITY_ROUTE == 0) ? 0 : FUNC_ID_ZW_GET_PRIORITY_ROUTE)|\
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_PRIORITY_ROUTE == 0) ? 0 : FUNC_ID_ZW_SET_PRIORITY_ROUTE)|\
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_APPL_SLAVE_NODE_INFORMATION == 0) ? 0 : FUNC_ID_SERIAL_API_APPL_SLAVE_NODE_INFORMATION) | \
                        CAP_FID(OFFSET, (SUPPORT_APPLICATION_SLAVE_COMMAND_HANDLER == 0) ? 0 : FUNC_ID_APPLICATION_SLAVE_COMMAND_HANDLER) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_SLAVE_NODE_INFORMATION == 0) ? 0 : FUNC_ID_ZW_SEND_SLAVE_NODE_INFORMATION) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_SLAVE_DATA == 0) ? 0 : FUNC_ID_ZW_SEND_SLAVE_DATA) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_SLAVE_LEARN_MODE == 0) ? 0 : FUNC_ID_ZW_SET_SLAVE_LEARN_MODE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_VIRTUAL_NODES == 0) ? 0 : FUNC_ID_ZW_GET_VIRTUAL_NODES) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_IS_VIRTUAL_NODE == 0) ? 0 : FUNC_ID_ZW_IS_VIRTUAL_NODE) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_TEST == 0) ? 0 : FUNC_ID_SERIAL_API_TEST) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_EXT == 0) ? 0 : FUNC_ID_SERIAL_API_EXT) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_GET_APPL_HOST_MEMORY_OFFSET == 0) ? 0 : FUNC_ID_SERIAL_API_GET_APPL_HOST_MEMORY_OFFSET) |\
                        CAP_FID(OFFSET, (SUPPORT_ZW_NUNIT == 0) ? 0 : FUNC_ID_ZW_NUNIT_CMD) |\
                        CAP_FID(OFFSET, (SUPPORT_ZW_NUNIT == 0) ? 0 : FUNC_ID_ZW_NUNIT_INIT) |\
                        CAP_FID(OFFSET, (SUPPORT_ZW_NUNIT == 0) ? 0 : FUNC_ID_ZW_NUNIT_LIST) |\
                        CAP_FID(OFFSET, (SUPPORT_ZW_NUNIT == 0) ? 0 : FUNC_ID_ZW_NUNIT_RUN) |\
                        CAP_FID(OFFSET, (SUPPORT_ZW_NUNIT == 0) ? 0 : FUNC_ID_ZW_NUNIT_END) |\
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_LISTEN_BEFORE_TALK_THRESHOLD == 0) ? 0 : FUNC_ID_ZW_SET_LISTEN_BEFORE_TALK_THRESHOLD)| \
                        CAP_FID(OFFSET, (SUPPORT_ZW_NETWORK_MANAGEMENT_SET_MAX_INCLUSION_REQUEST_INTERVALS == 0) ? 0 : FUNC_ID_ZW_NETWORK_MANAGEMENT_SET_MAX_INCLUSION_REQUEST_INTERVALS) \
                       )
#else
#define CAP_BM(OFFSET) (CAP_FID(OFFSET, (SUPPORT_SERIAL_API_POWER_MANAGEMENT == 0) ? 0 : FUNC_ID_SERIAL_API_POWER_MANAGEMENT) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_READY == 0) ? 0 : FUNC_ID_SERIAL_API_READY) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_GET_INIT_DATA == 0) ? 0 : FUNC_ID_SERIAL_API_GET_INIT_DATA) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_APPL_NODE_INFORMATION == 0) ? 0 : FUNC_ID_SERIAL_API_APPL_NODE_INFORMATION) | \
                        CAP_FID(OFFSET, (SUPPORT_APPLICATION_COMMAND_HANDLER_BRIDGE == 0) ? 0 : FUNC_ID_APPLICATION_COMMAND_HANDLER_BRIDGE) | \
                        CAP_FID(OFFSET, (SUPPORT_APPLICATION_COMMAND_HANDLER == 0) ? 0 : FUNC_ID_APPLICATION_COMMAND_HANDLER) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_CONTROLLER_CAPABILITIES == 0) ? 0 : FUNC_ID_ZW_GET_CONTROLLER_CAPABILITIES) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_SET_TIMEOUTS == 0) ? 0 : FUNC_ID_SERIAL_API_SET_TIMEOUTS) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_GET_CAPABILITIES == 0) ? 0 : FUNC_ID_SERIAL_API_GET_CAPABILITIES) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_SOFT_RESET == 0) ? 0 : FUNC_ID_SERIAL_API_SOFT_RESET) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_STARTUP_NOTIFICATION == 0) ? 0 : FUNC_ID_SERIAL_API_STARTED) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_SETUP == 0) ? 0 : FUNC_ID_SERIAL_API_SETUP) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_APPL_NODE_INFORMATION_CMD_CLASSES == 0) ? 0 : FUNC_ID_SERIAL_API_APPL_NODE_INFORMATION_CMD_CLASSES) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_PROTOCOL_VERSION == 0) ? 0 : FUNC_ID_ZW_GET_PROTOCOL_VERSION) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_RF_RECEIVE_MODE == 0) ? 0 : FUNC_ID_ZW_SET_RF_RECEIVE_MODE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_SLEEP_MODE == 0) ? 0 : FUNC_ID_ZW_SET_SLEEP_MODE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_NODE_INFORMATION == 0) ? 0 : FUNC_ID_ZW_SEND_NODE_INFORMATION) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_DATA == 0) ? 0 : FUNC_ID_ZW_SEND_DATA) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_DATA_BRIDGE == 0) ? 0 : FUNC_ID_ZW_SEND_DATA_BRIDGE)|\
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_DATA_EX == 0) ? 0 : FUNC_ID_ZW_SEND_DATA_EX) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_DATA_META_BRIDGE == 0) ? 0 : FUNC_ID_ZW_SEND_DATA_META_BRIDGE)|\
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_DATA_MULTI == 0) ? 0 : FUNC_ID_ZW_SEND_DATA_MULTI) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_DATA_MULTI_BRIDGE == 0) ? 0 : FUNC_ID_ZW_SEND_DATA_MULTI_BRIDGE)|\
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_DATA_MULTI_EX == 0) ? 0 : FUNC_ID_ZW_SEND_DATA_MULTI_EX) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SECURITY_SETUP == 0) ? 0 : FUNC_ID_ZW_SECURITY_SETUP) | \
                        CAP_FID(OFFSET, (SUPPORT_APPLICATION_SECURITY_EVENT == 0) ? 0 : FUNC_ID_APPLICATION_SECURITY_EVENT) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_VERSION == 0) ? 0 : FUNC_ID_ZW_GET_VERSION) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_DATA_ABORT == 0) ? 0 : FUNC_ID_ZW_SEND_DATA_ABORT) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_RF_POWER_LEVEL_SET == 0) ? 0 : FUNC_ID_ZW_RF_POWER_LEVEL_SET) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_RF_POWER_LEVEL_GET == 0) ? 0 : FUNC_ID_ZW_RF_POWER_LEVEL_GET) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_TYPE_LIBRARY == 0) ? 0 : FUNC_ID_ZW_TYPE_LIBRARY) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_PROTOCOL_STATUS == 0) ? 0 : FUNC_ID_ZW_GET_PROTOCOL_STATUS) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_WATCHDOG_ENABLE == 0) ? 0 : FUNC_ID_ZW_WATCHDOG_ENABLE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_WATCHDOG_DISABLE == 0) ? 0 : FUNC_ID_ZW_WATCHDOG_DISABLE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_WATCHDOG_KICK == 0) ? 0 : FUNC_ID_ZW_WATCHDOG_KICK) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_WATCHDOG_START == 0) ? 0 : FUNC_ID_ZW_WATCHDOG_START)|\
                        CAP_FID(OFFSET, (SUPPORT_ZW_WATCHDOG_STOP == 0) ? 0 : FUNC_ID_ZW_WATCHDOG_STOP)|\
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_WUT_TIMEOUT == 0) ? 0 : FUNC_ID_ZW_SET_WUT_TIMEOUT) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_IS_WUT_KICKED == 0) ? 0 : FUNC_ID_ZW_IS_WUT_KICKED) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_EXT_INT_LEVEL == 0) ? 0 : FUNC_ID_ZW_SET_EXT_INT_LEVEL) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_DATA_META == 0) ? 0 : FUNC_ID_ZW_SEND_DATA_META) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_TEST_FRAME == 0) ? 0 : FUNC_ID_ZW_SEND_TEST_FRAME) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_RANDOM == 0) ? 0 : FUNC_ID_ZW_GET_RANDOM) | \
                        CAP_FID(OFFSET, (SUPPORT_MEMORY_GET_ID == 0) ? 0 : FUNC_ID_MEMORY_GET_ID) | \
                        CAP_FID(OFFSET, (SUPPORT_MEMORY_GET_BYTE == 0) ? 0 : FUNC_ID_MEMORY_GET_BYTE) | \
                        CAP_FID(OFFSET, (SUPPORT_MEMORY_PUT_BYTE == 0) ? 0 : FUNC_ID_MEMORY_PUT_BYTE) | \
                        CAP_FID(OFFSET, (SUPPORT_MEMORY_GET_BUFFER == 0) ? 0 : FUNC_ID_MEMORY_GET_BUFFER) | \
                        CAP_FID(OFFSET, (SUPPORT_MEMORY_PUT_BUFFER == 0) ? 0 : FUNC_ID_MEMORY_PUT_BUFFER) | \
                        CAP_FID(OFFSET, (SUPPORT_NVM_BACKUP_RESTORE == 0) ? 0 : FUNC_ID_NVM_BACKUP_RESTORE) | \
                        CAP_FID(OFFSET, (SUPPORT_NVM_GET_ID == 0) ? 0 : FUNC_ID_NVM_GET_ID) | \
                        CAP_FID(OFFSET, (SUPPORT_NVM_EXT_READ_LONG_BYTE == 0) ? 0 : FUNC_ID_NVM_EXT_READ_LONG_BYTE) | \
                        CAP_FID(OFFSET, (SUPPORT_NVM_EXT_WRITE_LONG_BYTE == 0) ? 0 : FUNC_ID_NVM_EXT_WRITE_LONG_BYTE) | \
                        CAP_FID(OFFSET, (SUPPORT_NVM_EXT_READ_LONG_BUFFER == 0) ? 0 : FUNC_ID_NVM_EXT_READ_LONG_BUFFER) | \
                        CAP_FID(OFFSET, (SUPPORT_NVM_EXT_WRITE_LONG_BUFFER == 0) ? 0 : FUNC_ID_NVM_EXT_WRITE_LONG_BUFFER) | \
                        CAP_FID(OFFSET, (SUPPORT_CLOCK_SET == 0) ? 0 : FUNC_ID_CLOCK_SET) | \
                        CAP_FID(OFFSET, (SUPPORT_CLOCK_GET == 0) ? 0 : FUNC_ID_CLOCK_GET) | \
                        CAP_FID(OFFSET, (SUPPORT_CLOCK_CMP == 0) ? 0 : FUNC_ID_CLOCK_CMP) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_NVR_GET_VALUE == 0) ? 0 : FUNC_ID_NVR_GET_VALUE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_AUTO_PROGRAMMING == 0) ? 0 : FUNC_ID_AUTO_PROGRAMMING) | \
                        CAP_FID(OFFSET, (SUPPORT_PWR_SETSTOPMODE == 0) ? 0 : FUNC_ID_PWR_SETSTOPMODE) | \
                        CAP_FID(OFFSET, (SUPPORT_PWR_CLK_PD == 0) ? 0 : FUNC_ID_PWR_CLK_PD) | \
                        CAP_FID(OFFSET, (SUPPORT_PWR_CLK_PUP == 0) ? 0 : FUNC_ID_PWR_CLK_PUP) | \
                        CAP_FID(OFFSET, (SUPPORT_PWR_SELECT_CLK == 0) ? 0 : FUNC_ID_PWR_SELECT_CLK) | \
                        CAP_FID(OFFSET, (SUPPORT_RTC_TIMER_CREATE == 0) ? 0 : FUNC_ID_RTC_TIMER_CREATE) | \
                        CAP_FID(OFFSET, (SUPPORT_RTC_TIMER_READ == 0) ? 0 : FUNC_ID_RTC_TIMER_READ) | \
                        CAP_FID(OFFSET, (SUPPORT_RTC_TIMER_DELETE == 0) ? 0 : FUNC_ID_RTC_TIMER_DELETE) | \
                        CAP_FID(OFFSET, (SUPPORT_RTC_TIMER_CALL == 0) ? 0 : FUNC_ID_RTC_TIMER_CALL) | \
                        CAP_FID(OFFSET, (SUPPORT_FUNC_ID_CLEAR_TX_TIMERS == 0) ? 0 : FUNC_ID_CLEAR_TX_TIMERS) | \
                        CAP_FID(OFFSET, (SUPPORT_FUNC_ID_GET_TX_TIMERS == 0) ? 0 : FUNC_ID_GET_TX_TIMERS) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_CLEAR_NETWORK_STATS == 0) ? 0 : FUNC_ID_ZW_CLEAR_NETWORK_STATS) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_NETWORK_STATS == 0) ? 0 : FUNC_ID_ZW_GET_NETWORK_STATS) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_BACKGROUND_RSSI == 0) ? 0 : FUNC_ID_ZW_GET_BACKGROUND_RSSI) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_REMOVE_NODE_ID_FROM_NETWORK == 0) ? 0 : FUNC_ID_ZW_REMOVE_NODE_ID_FROM_NETWORK) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_LEARN_NODE_STATE == 0) ? 0 : FUNC_ID_ZW_SET_LEARN_NODE_STATE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_NODE_PROTOCOL_INFO == 0) ? 0 : FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_DEFAULT == 0) ? 0 : FUNC_ID_ZW_SET_DEFAULT) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_NEW_CONTROLLER == 0) ? 0 : FUNC_ID_ZW_NEW_CONTROLLER) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_REPLICATION_COMMAND_COMPLETE == 0) ? 0 : FUNC_ID_ZW_REPLICATION_COMMAND_COMPLETE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_REPLICATION_SEND_DATA == 0) ? 0 : FUNC_ID_ZW_REPLICATION_SEND_DATA) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_ASSIGN_RETURN_ROUTE == 0) ? 0 : FUNC_ID_ZW_ASSIGN_RETURN_ROUTE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_DELETE_RETURN_ROUTE == 0) ? 0 : FUNC_ID_ZW_DELETE_RETURN_ROUTE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_REQUEST_NODE_NEIGHBOR_UPDATE == 0) ? 0 : FUNC_ID_ZW_REQUEST_NODE_NEIGHBOR_UPDATE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_APPLICATION_CONTROLLER_UPDATE == 0) ? 0 : FUNC_ID_ZW_APPLICATION_UPDATE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_ADD_NODE_TO_NETWORK == 0) ? 0 : FUNC_ID_ZW_ADD_NODE_TO_NETWORK) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_REMOVE_NODE_FROM_NETWORK == 0) ? 0 : FUNC_ID_ZW_REMOVE_NODE_FROM_NETWORK) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_CREATE_NEW_PRIMARY == 0) ? 0 : FUNC_ID_ZW_CREATE_NEW_PRIMARY) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_CONTROLLER_CHANGE == 0) ? 0 : FUNC_ID_ZW_CONTROLLER_CHANGE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_REQUEST_NODE_INFO == 0) ? 0 : FUNC_ID_ZW_REQUEST_NODE_INFO) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_REMOVE_FAILED_NODE_ID == 0) ? 0 : FUNC_ID_ZW_REMOVE_FAILED_NODE_ID) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_IS_FAILED_NODE_ID == 0) ? 0 : FUNC_ID_ZW_IS_FAILED_NODE_ID) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_REPLACE_FAILED_NODE == 0) ? 0 : FUNC_ID_ZW_REPLACE_FAILED_NODE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_ROUTING_MAX == 0) ? 0 : FUNC_ID_ZW_GET_ROUTING_MAX) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_ROUTING_MAX == 0) ? 0 : FUNC_ID_ZW_SET_ROUTING_MAX) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_IS_PRIMARY_CTRL == 0) ? 0 : FUNC_ID_ZW_IS_PRIMARY_CTRL) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_AES_ECB == 0) ? 0 : FUNC_ID_ZW_AES_ECB) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_APPLICATION_UPDATE == 0) ? 0 : FUNC_ID_ZW_APPLICATION_UPDATE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_LEARN_MODE == 0) ? 0 : FUNC_ID_ZW_SET_LEARN_MODE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_EXPLORE_REQUEST_INCLUSION == 0) ? 0 : FUNC_ID_ZW_EXPLORE_REQUEST_INCLUSION) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_EXPLORE_REQUEST_EXCLUSION == 0) ? 0 : FUNC_ID_ZW_EXPLORE_REQUEST_EXCLUSION) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_ASSIGN_SUC_RETURN_ROUTE == 0) ? 0 : FUNC_ID_ZW_ASSIGN_SUC_RETURN_ROUTE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_ENABLE_SUC == 0) ? 0 : FUNC_ID_ZW_ENABLE_SUC) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_REQUEST_NETWORK_UPDATE == 0) ? 0 : FUNC_ID_ZW_REQUEST_NETWORK_UPDATE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_SUC_NODE_ID == 0) ? 0 : FUNC_ID_ZW_SET_SUC_NODE_ID) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_DELETE_SUC_RETURN_ROUTE == 0) ? 0 : FUNC_ID_ZW_DELETE_SUC_RETURN_ROUTE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_SUC_NODE_ID == 0) ? 0 : FUNC_ID_ZW_GET_SUC_NODE_ID) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_SUC_ID == 0) ? 0 : FUNC_ID_ZW_SEND_SUC_ID) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_ASSIGN_PRIORITY_SUC_RETURN_ROUTE == 0) ? 0 : FUNC_ID_ZW_ASSIGN_PRIORITY_SUC_RETURN_ROUTE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_ASSIGN_PRIORITY_RETURN_ROUTE == 0) ? 0 : FUNC_ID_ZW_ASSIGN_PRIORITY_RETURN_ROUTE) | \
                        CAP_FID(OFFSET, (SUPPORT_TIMER_START == 0) ? 0 : FUNC_ID_TIMER_START) | \
                        CAP_FID(OFFSET, (SUPPORT_TIMER_RESTART == 0) ? 0 : FUNC_ID_TIMER_RESTART) | \
                        CAP_FID(OFFSET, (SUPPORT_TIMER_CANCEL == 0) ? 0 : FUNC_ID_TIMER_CANCEL) | \
                        CAP_FID(OFFSET, (SUPPORT_TIMER_CALL == 0) ? 0 : FUNC_ID_TIMER_CALL) | \
                        CAP_FID(OFFSET, (SUPPORT_GET_ROUTING_TABLE_LINE == 0) ? 0 : FUNC_ID_GET_ROUTING_TABLE_LINE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_ROUTING_INFO == 0) ? 0 : FUNC_ID_ZW_SET_ROUTING_INFO) | \
                        CAP_FID(OFFSET, (SUPPORT_GET_TX_COUNTER == 0) ? 0 : FUNC_ID_GET_TX_COUNTER) | \
                        CAP_FID(OFFSET, (SUPPORT_RESET_TX_COUNTER == 0) ? 0 : FUNC_ID_RESET_TX_COUNTER) | \
                        CAP_FID(OFFSET, (SUPPORT_STORE_NODEINFO == 0) ? 0 : FUNC_ID_STORE_NODEINFO) | \
                        CAP_FID(OFFSET, (SUPPORT_STORE_HOMEID == 0) ? 0 : FUNC_ID_STORE_HOMEID) | \
                        CAP_FID(OFFSET, (SUPPORT_LOCK_ROUTE_RESPONSE == 0) ? 0 : FUNC_ID_LOCK_ROUTE_RESPONSE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_PRIORITY_ROUTE == 0) ? 0 : FUNC_ID_ZW_GET_PRIORITY_ROUTE)|\
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_PRIORITY_ROUTE == 0) ? 0 : FUNC_ID_ZW_SET_PRIORITY_ROUTE)|\
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_APPL_SLAVE_NODE_INFORMATION == 0) ? 0 : FUNC_ID_SERIAL_API_APPL_SLAVE_NODE_INFORMATION) | \
                        CAP_FID(OFFSET, (SUPPORT_APPLICATION_SLAVE_COMMAND_HANDLER == 0) ? 0 : FUNC_ID_APPLICATION_SLAVE_COMMAND_HANDLER) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_SLAVE_NODE_INFORMATION == 0) ? 0 : FUNC_ID_ZW_SEND_SLAVE_NODE_INFORMATION) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SEND_SLAVE_DATA == 0) ? 0 : FUNC_ID_ZW_SEND_SLAVE_DATA) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_SLAVE_LEARN_MODE == 0) ? 0 : FUNC_ID_ZW_SET_SLAVE_LEARN_MODE) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_GET_VIRTUAL_NODES == 0) ? 0 : FUNC_ID_ZW_GET_VIRTUAL_NODES) | \
                        CAP_FID(OFFSET, (SUPPORT_ZW_IS_VIRTUAL_NODE == 0) ? 0 : FUNC_ID_ZW_IS_VIRTUAL_NODE) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_EXT == 0) ? 0 : FUNC_ID_SERIAL_API_EXT) | \
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_GET_APPL_HOST_MEMORY_OFFSET == 0) ? 0 : FUNC_ID_SERIAL_API_GET_APPL_HOST_MEMORY_OFFSET) |\
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_PROMISCUOUS_MODE == 0) ? 0 : FUNC_ID_ZW_SET_PROMISCUOUS_MODE) |\
                        CAP_FID(OFFSET, (SUPPORT_ZW_NUNIT == 0) ? 0 : FUNC_ID_ZW_NUNIT_CMD) |\
                        CAP_FID(OFFSET, (SUPPORT_ZW_NUNIT == 0) ? 0 : FUNC_ID_ZW_NUNIT_INIT) |\
                        CAP_FID(OFFSET, (SUPPORT_ZW_NUNIT == 0) ? 0 : FUNC_ID_ZW_NUNIT_LIST) |\
                        CAP_FID(OFFSET, (SUPPORT_ZW_NUNIT == 0) ? 0 : FUNC_ID_ZW_NUNIT_RUN) |\
                        CAP_FID(OFFSET, (SUPPORT_ZW_NUNIT == 0) ? 0 : FUNC_ID_ZW_NUNIT_END) |\
                        CAP_FID(OFFSET, (SUPPORT_ZW_SET_LISTEN_BEFORE_TALK_THRESHOLD == 0) ? 0 : FUNC_ID_ZW_SET_LISTEN_BEFORE_TALK_THRESHOLD)| \
                        CAP_FID(OFFSET, (SUPPORT_ZW_NETWORK_MANAGEMENT_SET_MAX_INCLUSION_REQUEST_INTERVALS == 0) ? 0 : FUNC_ID_ZW_NETWORK_MANAGEMENT_SET_MAX_INCLUSION_REQUEST_INTERVALS)| \
                        CAP_FID(OFFSET, (SUPPORT_ZW_INITIATE_SHUTDOWN == 0) ? 0 : FUNC_ID_ZW_INITIATE_SHUTDOWN) |\
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_API_GET_LR_NODES == 0) ? 0 : FUNC_ID_SERIAL_API_GET_LR_NODES) |\
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_GET_LR_CHANNEL == 0) ? 0 : FUNC_ID_GET_LR_CHANNEL) |\
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_SET_LR_CHANNEL == 0) ? 0 : FUNC_ID_SET_LR_CHANNEL) |\
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_SET_LR_VIRTUAL_IDS == 0) ? 0 : FUNC_ID_ZW_SET_LR_VIRTUAL_IDS) |\
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_ENABLE_RADIO_PTI == 0) ? 0 : FUNC_ID_ENABLE_RADIO_PTI) |\
                        CAP_FID(OFFSET, (SUPPORT_SERIAL_ENABLE_RADIO_PTI == 0) ? 0 : FUNC_ID_GET_RADIO_PTI) \
                       )
#endif  /* ZW_CONTROLLER_SINGLE */


#if SUPPORT_SERIAL_API_READY
enum
{
  /* SERIAL_LINK_IDLE = Ready for incomming Serial communication, but */
  /* do not transmit anything via the serial link even if application */
  /* frames is received on the RF, which normally should be transmitted */
  /* to the HOST. */
  SERIAL_LINK_DETACHED = 0,
  /* SERIAL_LINK_CONNECTED = There exists a HOST so transmit on serial */
  /* link if needed. */
  SERIAL_LINK_CONNECTED = 1
};

extern uint8_t serialLinkState;
#endif /* SUPPORT_SERIAL_API_READY */

extern void DoRespond_workbuf(
  uint8_t cnt);

extern void set_state_and_notify(
  uint8_t st
);

extern void set_state(
  uint8_t st
);

extern bool Request(
  uint8_t cmd,             /*IN   Command                  */
  uint8_t *pData,         /*IN   pointer to data          */
  uint8_t len              /*IN   Length of data           */
);

extern bool RequestUnsolicited(
  uint8_t cmd,             /*IN   Command                  */
  uint8_t *pData,         /*IN   pointer to data          */
  uint8_t len              /*IN   Length of data           */
);

extern void Respond(
  uint8_t cmd,             /*IN   Command                  */
  uint8_t const * pData,         /*IN   pointer to data          */
  uint8_t len              /*IN   Length of data           */
);
extern void DoRespond(uint8_t retVal);

extern void PopCallBackQueue(void);

extern void PopCommandQueue(void);

extern uint8_t GetCallbackCnt(void);

extern void ZW_GetMfgTokenDataCountryFreq(void *data);

#ifdef ZW_CONTROLLER_SINGLE
#if SUPPORT_SERIAL_API_TEST

extern void
SendTestReport(
  uint8_t txStatus);

extern void
TestStartRound(void);

extern void
SendTestRoundReport(
  uint8_t txStatus);

extern void
ZCB_TestDelayNextSendTimeout(void);

extern void
ZCB_TestDelayTimeout(void);

extern bool
TestFindNextNode(void);

extern void
ZCB_TestSendComplete(
  uint8_t bStatus,
  TX_STATUS_TYPE *txStatusReport);

extern void
TestSend(void);

#endif /* SUPPORT_SERIAL_API_TEST */
#endif /* ZW_CONTROLLER_SINGLE */

#if SUPPORT_SERIAL_API_POWER_MANAGEMENT
extern void
ZCB_PowerManagementWakeUpOnExternalActive(void);

extern void
ZCB_PowerManagementWakeUpOnTimerHandler(void);

extern void
ZCB_powerManagementPoweredUpPinActive(void);

extern void
PowerManagementSetPowerDown(void);

extern void
PowerManagementSetPowerUp(void);

extern void
PowerManagementCheck(void);

extern void
PurgeCallbackQueue(void);

extern void
PurgeCommandQueue(void);
#endif /* SUPPORT_SERIAL_API_POWER_MANAGEMENT */

// Prioritized events that can wakeup protocol thread.
typedef enum EApplicationEvent
{
  EAPPLICATIONEVENT_ZWRX = 0,
  EAPPLICATIONEVENT_ZWCOMMANDSTATUS,
  EAPPLICATIONEVENT_STATECHANGE,
  EAPPLICATIONEVENT_SERIALDATARX,
  EAPPLICATIONEVENT_SERIALTIMEOUT,  
  EAPPLICATIONEVENT_TIMER
} EApplicationEvent;

/* FUNC_ID_SERIAL_API_STARTED Capabilities bit field definitions */
typedef enum
{
  SERIAL_API_STARTED_CAPABILITIES_L0NG_RANGE = 1<<0 // Controller is Z-Wave Long Range capable
  /* Can be extended with future capability bits here */
} eSerialAPIStartedCapabilities;

extern void ApplicationNodeUpdate(uint8_t bStatus, uint16_t nodeID, uint8_t *pCmd, uint8_t bLen);

/* Should be enough */
#define BUF_SIZE_RX 168
#define BUF_SIZE_TX 168

extern uint8_t compl_workbuf[BUF_SIZE_TX];

#endif /* _SERIALAPPL_H_ */
