/**
 * @file
 * @copyright 2019 Silicon Laboratories Inc.
 */
#ifndef _ZW_SERIALAPI_H_
#define _ZW_SERIALAPI_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/


/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

#define SOF 0x01  /* Start Of Frame */
#define ACK 0x06  /* Acknowledge successfull frame reception */
#define NAK 0x15  /* Not Acknowledge successfull frame reception - please retransmit... */
#define CAN 0x18  /* Frame received (from host) was dropped - waiting for ACK */

/* Frame types */
#define REQUEST                                         0x00
#define RESPONSE                                        0x01

/* Flags used in FUNC_ID_SERIAL_API_GET_INIT_DATA functionality */
#define GET_INIT_DATA_FLAG_SLAVE_API                    0x01
#define GET_INIT_DATA_FLAG_TIMER_SUPPORT                0x02
#define GET_INIT_DATA_FLAG_CONTROLLER_STATUS            0x04 /* Obsolete. USE next */
#define GET_INIT_DATA_FLAG_SECONDARY_CTRL               0x04
#define GET_INIT_DATA_FLAG_IS_SUC                       0x08

/* Firmware update functionality specifics */
/* Enum definitions for Firmware Update functionality selector; firmwareUpdateFunction */
typedef enum
{
  FIRMWARE_UPDATE_NVM_INIT = 0,
  FIRMWARE_UPDATE_NVM_SET_NEW_IMAGE = 1,
  FIRMWARE_UPDATE_NVM_GET_NEW_IMAGE = 2,
  FIRMWARE_UPDATE_NVM_UPDATE_CRC16 = 3,
  FIRMWARE_UPDATE_NVM_IS_VALID_CRC16 = 4,
  FIRMWARE_UPDATE_NVM_WRITE = 5,
  FIRMWARE_UPDATE_NVM_UNKNOWN = 0xFF
} FIRMWARE_UPDATE_NVM_T;

/* Function IDs */
#define FUNC_ID_SERIAL_API_GET_INIT_DATA                0x02
#define FUNC_ID_SERIAL_API_APPL_NODE_INFORMATION        0x03
#define FUNC_ID_APPLICATION_COMMAND_HANDLER             0x04
#define FUNC_ID_ZW_GET_CONTROLLER_CAPABILITIES          0x05

/* SERIAL API ver 4 added - START */
#define FUNC_ID_SERIAL_API_SET_TIMEOUTS                 0x06
#define FUNC_ID_SERIAL_API_GET_CAPABILITIES             0x07
#define FUNC_ID_SERIAL_API_SOFT_RESET                   0x08
/* SERIAL API ver 4 added - END */

#define FUNC_ID_ZW_GET_PROTOCOL_VERSION                 0x09

/* Function ID for startup message */
#define FUNC_ID_SERIAL_API_STARTED                      0x0A
#define FUNC_ID_SERIAL_API_SETUP                        0x0B

#define FUNC_ID_SERIAL_API_APPL_NODE_INFORMATION_CMD_CLASSES  0x0C

#define FUNC_ID_ZW_SEND_DATA_EX                         0x0E
#define FUNC_ID_ZW_SEND_DATA_MULTI_EX                   0x0F

#define FUNC_ID_ZW_SET_RF_RECEIVE_MODE                  0x10
#define FUNC_ID_ZW_SET_SLEEP_MODE                       0x11
#define FUNC_ID_ZW_SEND_NODE_INFORMATION                0x12
#define FUNC_ID_ZW_SEND_DATA                            0x13
#define FUNC_ID_ZW_SEND_DATA_MULTI                      0x14
#define FUNC_ID_ZW_GET_VERSION                          0x15

/* SERIAL API ver 4 added - START */
#define FUNC_ID_ZW_SEND_DATA_ABORT                      0x16
// FUNC_ID_ZW_RF_POWER_LEVEL_SET is no longer supported
#define FUNC_ID_ZW_RF_POWER_LEVEL_SET                   0x17
#define FUNC_ID_ZW_SEND_DATA_META                       0x18
/* SERIAL API ver 4 added - END */

#define FUNC_ID_ZW_RESERVED_SD                          0x19
#define FUNC_ID_ZW_RESERVED_SDM                         0x1A
#define FUNC_ID_ZW_RESERVED_SRI                         0x1B

#define FUNC_ID_ZW_SET_ROUTING_INFO                     0x1B

#define FUNC_ID_ZW_GET_RANDOM                           0x1C
#define FUNC_ID_ZW_RANDOM                               0x1D
#define FUNC_ID_ZW_RF_POWER_LEVEL_REDISCOVERY_SET       0x1E

#define FUNC_ID_MEMORY_GET_ID                           0x20
#define FUNC_ID_MEMORY_GET_BYTE                         0x21
#define FUNC_ID_MEMORY_PUT_BYTE                         0x22
#define FUNC_ID_MEMORY_GET_BUFFER                       0x23
#define FUNC_ID_MEMORY_PUT_BUFFER                       0x24
/* Unimplemented - START */
#define FUNC_ID_SERIAL_API_GET_APPL_HOST_MEMORY_OFFSET  0x25
#define FUNC_ID_DEBUG_OUTPUT                            0x26
/* Unimplemented - END */

#define FUNC_ID_AUTO_PROGRAMMING                        0x27

#define FUNC_ID_NVR_GET_VALUE                           0x28

#define FUNC_ID_NVM_GET_ID                              0x29
#define FUNC_ID_NVM_EXT_READ_LONG_BUFFER                0x2A
#define FUNC_ID_NVM_EXT_WRITE_LONG_BUFFER               0x2B
#define FUNC_ID_NVM_EXT_READ_LONG_BYTE                  0x2C
#define FUNC_ID_NVM_EXT_WRITE_LONG_BYTE                 0x2D
#define FUNC_ID_NVM_BACKUP_RESTORE                      0x2E

#define FUNC_ID_ZW_NVR_GET_APP_VALUE                    0x2F

#define FUNC_ID_CLOCK_SET                               0x30
#define FUNC_ID_CLOCK_GET                               0x31
#define FUNC_ID_CLOCK_CMP                               0x32
#define FUNC_ID_RTC_TIMER_CREATE                        0x33
#define FUNC_ID_RTC_TIMER_READ                          0x34
#define FUNC_ID_RTC_TIMER_DELETE                        0x35
#define FUNC_ID_RTC_TIMER_CALL                          0x36

#define FUNC_ID_CLEAR_TX_TIMERS                         0x37
#define FUNC_ID_GET_TX_TIMERS                           0x38

#define FUNC_ID_ZW_CLEAR_NETWORK_STATS                  0x39
#define FUNC_ID_ZW_GET_NETWORK_STATS                    0x3A
#define FUNC_ID_ZW_GET_BACKGROUND_RSSI                  0x3B
#define FUNC_ID_ZW_SET_LISTEN_BEFORE_TALK_THRESHOLD     0x3C
#define FUNC_ID_ZW_REMOVE_NODE_ID_FROM_NETWORK          0x3F

#define FUNC_ID_ZW_SET_LEARN_NODE_STATE                 0x40
#define FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO               0x41
#define FUNC_ID_ZW_SET_DEFAULT                          0x42
#define FUNC_ID_ZW_NEW_CONTROLLER                       0x43
// <Obsoleted>
#define FUNC_ID_ZW_REPLICATION_COMMAND_COMPLETE         0x44
#define FUNC_ID_ZW_REPLICATION_SEND_DATA                0x45
// </Obsoleted>
#define FUNC_ID_ZW_ASSIGN_RETURN_ROUTE                  0x46
#define FUNC_ID_ZW_DELETE_RETURN_ROUTE                  0x47
#define FUNC_ID_ZW_REQUEST_NODE_NEIGHBOR_UPDATE         0x48
#define FUNC_ID_ZW_APPLICATION_UPDATE                   0x49

/*Obsolete use ZW_APPLICATION_UPDATE */
#define FUNC_ID_ZW_APPLICATION_CONTROLLER_UPDATE        0x49

#define FUNC_ID_ZW_ADD_NODE_TO_NETWORK                  0x4A
#define FUNC_ID_ZW_REMOVE_NODE_FROM_NETWORK             0x4B
#define FUNC_ID_ZW_CREATE_NEW_PRIMARY                   0x4C
#define FUNC_ID_ZW_CONTROLLER_CHANGE                    0x4D

#define FUNC_ID_ZW_RESERVED_FN                          0x4E

/* Devkit 6.6x */
#define FUNC_ID_ZW_ASSIGN_PRIORITY_RETURN_ROUTE         0x4F

/* Slave only */
#define FUNC_ID_ZW_SET_LEARN_MODE                       0x50
/* Slave only end */

#define FUNC_ID_ZW_ASSIGN_SUC_RETURN_ROUTE              0x51
#define FUNC_ID_ZW_ENABLE_SUC                           0x52
#define FUNC_ID_ZW_REQUEST_NETWORK_UPDATE               0x53
#define FUNC_ID_ZW_SET_SUC_NODE_ID                      0x54
#define FUNC_ID_ZW_DELETE_SUC_RETURN_ROUTE              0x55
#define FUNC_ID_ZW_GET_SUC_NODE_ID                      0x56
#define FUNC_ID_ZW_SEND_SUC_ID                          0x57

/* Devkit 6.6x */
#define FUNC_ID_ZW_ASSIGN_PRIORITY_SUC_RETURN_ROUTE     0x58

/* Obsolete - Not available in devkit 6.6x+ */
#define FUNC_ID_ZW_REDISCOVERY_NEEDED                   0x59

#define FUNC_ID_ZW_REQUEST_NODE_NEIGHBOR_UPDATE_OPTION  0x5A

/* Slave only */
#define FUNC_ID_ZW_SUPPORT9600_ONLY                     0x5B
/* Slave only end */

/* Enhanced/Routing Slave only */
#define FUNC_ID_ZW_REQUEST_NEW_ROUTE_DESTINATIONS       0x5C
#define FUNC_ID_ZW_IS_NODE_WITHIN_DIRECT_RANGE          0x5D
/* Enhanced/Routing Slave only end */

#define FUNC_ID_ZW_EXPLORE_REQUEST_INCLUSION            0x5E
#define FUNC_ID_ZW_EXPLORE_REQUEST_EXCLUSION            0x5F

#define FUNC_ID_ZW_REQUEST_NODE_INFO                    0x60
#define FUNC_ID_ZW_REMOVE_FAILED_NODE_ID                0x61
#define FUNC_ID_ZW_IS_FAILED_NODE_ID                    0x62
#define FUNC_ID_ZW_REPLACE_FAILED_NODE                  0x63

/* In 6.0x the function id was wrong so we need to support this wrong function id as well in the future */
#define FUNC_ID_ZW_SET_ROUTING_MAX_6_00                 0x65
/* */

#define FUNC_ID_ZW_IS_PRIMARY_CTRL                      0x66

#define FUNC_ID_ZW_AES_ECB                              0x67

#define FUNC_ID_TIMER_START                             0x70
#define FUNC_ID_TIMER_RESTART                           0x71
#define FUNC_ID_TIMER_CANCEL                            0x72
#define FUNC_ID_TIMER_CALL                              0x73

/* Firmware Update API */
#define FUNC_ID_ZW_FIRMWARE_UPDATE_NVM                  0x78

/* Installer API */
#define FUNC_ID_GET_ROUTING_TABLE_LINE                  0x80
#define FUNC_ID_GET_TX_COUNTER                          0x81
#define FUNC_ID_RESET_TX_COUNTER                        0x82
#define FUNC_ID_STORE_NODEINFO                          0x83
#define FUNC_ID_STORE_HOMEID                            0x84
/* Installer API only end */

#define FUNC_ID_LOCK_ROUTE_RESPONSE                     0x90
#ifdef ZW_ROUTING_DEMO
/* Max hops in route */
#define TRANSMIT_ROUTED_ATTEMPT                         0x08
#define FUNC_ID_ZW_SEND_DATA_ROUTE_DEMO	                0x91
#endif

/* ZW_SetPriorityRoute/ZW_GetPriorityRoute replaces ZW_SetLastWorkingRoute/ZW_GetLastWorkingRoute */
#define FUNC_ID_ZW_GET_PRIORITY_ROUTE                   0x92
#define FUNC_ID_ZW_SET_PRIORITY_ROUTE                   0x93
/* NOTE: Obsoleted - BEGIN */
#define FUNC_ID_ZW_GET_LAST_WORKING_ROUTE               0x92
#define FUNC_ID_ZW_SET_LAST_WORKING_ROUTE               0x93
/* NOTE: Obsoleted - END */

#define FUNC_ID_SERIAL_API_TEST                         0x95

#define FUNC_ID_SERIAL_API_EXT                          0x98

/* DevKit 6.7x introduced - slave_enhanced_232 and slave_routing only */
#define FUNC_ID_ZW_SECURITY_SETUP                       0x9C
#define FUNC_ID_APPLICATION_SECURITY_EVENT              0x9D

/* ZW_CONTROLLER_BRIDGE only START */
#define FUNC_ID_SERIAL_API_APPL_SLAVE_NODE_INFORMATION  0xA0
/* OBSOLETE: In DevKit 4.5x/6.0x Controller Bridge applications, this is obsoleted */
/* by the FUNC_ID_APPLICATION_COMMAND_HANDLER_BRIDGE */
#define FUNC_ID_APPLICATION_SLAVE_COMMAND_HANDLER       0xA1
#define FUNC_ID_ZW_SEND_SLAVE_NODE_INFORMATION          0xA2
#define FUNC_ID_ZW_SEND_SLAVE_DATA                      0xA3
#define FUNC_ID_ZW_SET_SLAVE_LEARN_MODE                 0xA4
#define FUNC_ID_ZW_GET_VIRTUAL_NODES                    0xA5
#define FUNC_ID_ZW_IS_VIRTUAL_NODE                      0xA6
#define FUNC_ID_ZW_RESERVED_SSD                         0xA7
/* DevKit 4.5x/6.0x added - obsoletes FUNC_ID_APPLICATION_SLAVE_COMMAND_HANDLER and */
/* FUNC_ID_APPLICATION_COMMAND_HANDLER for the Controller Bridge applications as */
/* this handles both cases - only for 4.5x/6.0x based Controller Bridge applications */
#define FUNC_ID_APPLICATION_COMMAND_HANDLER_BRIDGE      0xA8
/* DevKit 4.5x/6.0x added - Adds sourceNodeID to the parameter list */
#define FUNC_ID_ZW_SEND_DATA_BRIDGE                     0xA9
/* Obsolete */
#define FUNC_ID_ZW_SEND_DATA_META_BRIDGE                0xAA
#define FUNC_ID_ZW_SEND_DATA_MULTI_BRIDGE               0xAB
/* ZW_CONTROLLER_BRIDGE only END */

#define FUNC_ID_PWR_SETSTOPMODE                         0xB0    // ZW102 only
#define FUNC_ID_PWR_CLK_PD                              0xB1    // ZW102 only
#define FUNC_ID_PWR_CLK_PUP                             0xB2    // ZW102 only
#define FUNC_ID_PWR_SELECT_CLK                          0xB3    // ZW102 only
#define FUNC_ID_ZW_SET_WUT_TIMEOUT                      0xB4
#define FUNC_ID_ZW_IS_WUT_KICKED                        0xB5    // ZW201 only

#define FUNC_ID_ZW_WATCHDOG_ENABLE                      0xB6
#define FUNC_ID_ZW_WATCHDOG_DISABLE                     0xB7
#define FUNC_ID_ZW_WATCHDOG_KICK                        0xB8
/* Obsolete use FUNC_ID_ZW_INT_EXT_LEVEL_SET */
#define FUNC_ID_ZW_SET_EXT_INT_LEVEL                    0xB9    // ZW201 only
#define FUNC_ID_ZW_INT_EXT_LEVEL_SET                    0xB9

// FUNC_ID_ZW_RF_POWER_LEVEL_GET is no longer supported
#define FUNC_ID_ZW_RF_POWER_LEVEL_GET                   0xBA
#define FUNC_ID_ZW_GET_NEIGHBOR_COUNT                   0xBB
#define FUNC_ID_ZW_ARE_NODES_NEIGHBOURS                 0xBC

#define FUNC_ID_ZW_TYPE_LIBRARY                         0xBD
#define FUNC_ID_ZW_SEND_TEST_FRAME                      0xBE
#define FUNC_ID_ZW_GET_PROTOCOL_STATUS                  0xBF

#define FUNC_ID_ZW_SET_PROMISCUOUS_MODE                 0xD0
/* SERIAL API ver 5 added - START */
#define FUNC_ID_PROMISCUOUS_APPLICATION_COMMAND_HANDLER 0xD1
/* SERIAL API ver 5 added - END */

#define FUNC_ID_ZW_WATCHDOG_START                       0xD2
#define FUNC_ID_ZW_WATCHDOG_STOP                        0xD3

#define FUNC_ID_ZW_SET_ROUTING_MAX                      0xD4
/* Unimplemented - START */
/* Obsoleted */
#define FUNC_ID_ZW_GET_ROUTING_MAX                      0xD5

#define FUNC_ID_PM_STAY_AWAKE                           0xD7
#define FUNC_ID_PM_CANCEL                               0xD8
/* Unimplemented - END */

/* Allocated for setting the maximum number of 128sec ticks inbetween SmartStart inclusion requests. */
#define FUNC_ID_ZW_NETWORK_MANAGEMENT_SET_MAX_INCLUSION_REQUEST_INTERVALS 0xD6

#define FUNC_ID_ZW_INITIATE_SHUTDOWN                    0xD9

#define FUNC_ID_SERIAL_API_GET_LR_NODES                 0xDA

#define FUNC_ID_GET_LR_CHANNEL                          0xDB
#define FUNC_ID_SET_LR_CHANNEL                          0xDC

#define FUNC_ID_ZW_SET_LR_VIRTUAL_IDS                   0xDD

#define FUNC_ID_GET_DCDC_CONFIG                         0xDE
#define FUNC_ID_SET_DCDC_CONFIG                         0xDF

/* Allocated for NUNIT test */
#define FUNC_ID_ZW_NUNIT_CMD                            0xE0 // DEPRECATED
#define FUNC_ID_ZW_NUNIT_INIT                           0xE1 // DEPRECATED
#define FUNC_ID_ZW_NUNIT_LIST                           0xE2 // DEPRECATED
#define FUNC_ID_ZW_NUNIT_RUN                            0xE3 // DEPRECATED
#define FUNC_ID_ZW_NUNIT_END                            0xE4 // DEPRECATED

#define FUNC_ID_ENABLE_RADIO_PTI                        0xE7
#define FUNC_ID_GET_RADIO_PTI                           0xE8

#define FUNC_ID_SEND_NOP                                0xE9 //Reserved. Command to be implemented in future.

/* Allocated for Power Management */
#define FUNC_ID_SERIAL_API_POWER_MANAGEMENT             0xEE
#define FUNC_ID_SERIAL_API_READY                        0xEF

/* Allocated for proprietary serial API commands */
#define FUNC_ID_PROPRIETARY_0                           0xF0
#define FUNC_ID_PROPRIETARY_1                           0xF1
#define FUNC_ID_PROPRIETARY_2                           0xF2
#define FUNC_ID_PROPRIETARY_3                           0xF3
#define FUNC_ID_PROPRIETARY_4                           0xF4
#define FUNC_ID_PROPRIETARY_5                           0xF5
#define FUNC_ID_PROPRIETARY_6                           0xF6
#define FUNC_ID_PROPRIETARY_7                           0xF7
#define FUNC_ID_PROPRIETARY_8                           0xF8
#define FUNC_ID_PROPRIETARY_9                           0xF9
#define FUNC_ID_PROPRIETARY_A                           0xFA
#define FUNC_ID_PROPRIETARY_B                           0xFB
#define FUNC_ID_PROPRIETARY_C                           0xFC
#define FUNC_ID_PROPRIETARY_D                           0xFD
#define FUNC_ID_PROPRIETARY_E                           0xFE


/* Illegal function ID */
#define FUNC_ID_UNKNOWN                                 0xFF

#endif /*_ZW_SERIALAPI_H_*/
