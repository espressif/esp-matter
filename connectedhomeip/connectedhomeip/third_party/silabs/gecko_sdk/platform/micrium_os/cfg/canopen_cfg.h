/***************************************************************************//**
 * @file
 * @brief CANopen Configuration - Configuration Template File
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef _CANOPEN_CFG_H_
#define _CANOPEN_CFG_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                      CANOPEN OBJECT DIRECTORY CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

// <q CANOPEN_OBJ_PARAM_EN> Loading/saving of parameters from/into non-volatile memory
// <i> If enabled, the parameter callback functions are used when accessing the standard object directory entries at index 0x1010 (Store) and 0x1011 (Restore default parameters).
// <i> The CANopen stack will perform some plausibility checks on the accessed entries. In case of a configuration error, the corresponding error code will be set within the node error.
// <i> Default: 1
#define CANOPEN_OBJ_PARAM_EN         1

// <q CANOPEN_OBJ_STRING_EN> String object management
// <i> Enables or disables the string object management inside the object dictionary.
// <i> Default: 1
#define CANOPEN_OBJ_STRING_EN        1

// <q CANOPEN_OBJ_DOMAIN_EN> Domain object management
// <i> Enables or disables the domain object management inside the object dictionary.
// <i> Default: 1
#define CANOPEN_OBJ_DOMAIN_EN        1

/********************************************************************************************************
 ********************************************************************************************************
 *                                      CANOPEN SDO SERVER CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

// <o CANOPEN_SDO_MAX_SERVER_QTY> Number of SDO servers for a given node <1-128>
// <i> Default: 2
#define CANOPEN_SDO_MAX_SERVER_QTY   2

// <q CANOPEN_SDO_DYN_ID_EN> Allow changing an SDO server COB-ID at runtime
// <i> It is possible to dynamically assign an SDO server COB-ID if certain SDO server parameters (located in the indexes range 0x1200-0x127F) have been configured to allow it.
// <i> Default: 1
#define CANOPEN_SDO_DYN_ID_EN        1

// <q CANOPEN_SDO_SEG_EN> SDO segmented transfer
// <i> The segmented transfer, also referred as a non-expedited transfer, allows you to transfer data of any size during the segment phase (upload/download SDO segment).
// <i> If disabled, user data is limited to four bytes during the init phase of an SDO transfer (initiate SDO upload/download).
// <i> Default: 1
#define CANOPEN_SDO_SEG_EN           1

/********************************************************************************************************
 ********************************************************************************************************
 *                                      CANOPEN EMERGENCY CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

// <o CANOPEN_EMCY_MAX_ERR_QTY> Number of emergency codes for a given node <1-255>
// <i> If the emergency module is enabled, the object entries 0x1001 (Error register) and 0x1014 (COB-ID EMCY) shall be configured correctly.
// <i> When this configuration is set to 0, all the Emergency service support is disabled within the stack.
// <i> Default: 6
#define CANOPEN_EMCY_MAX_ERR_QTY     6

// <q CANOPEN_EMCY_REG_CLASS_EN> Emergency error classes support
// <i> Emergency error classes such as current, voltage, and temperature within the error register object entry (index 0x1001) considered as optional by the CiA 301 specification.
// <i> Default: 0
#define CANOPEN_EMCY_REG_CLASS_EN    0

// <q CANOPEN_EMCY_EMCY_MAN_EN> Manufacturer-specific information field in the emergency messages
// <i> Enables or disables the manufacturer-specific information field in the emergency messages. The manufacturer-specific field corresponds to 5 bytes in each 8-byte emergency message.
// <i> Default: 0
#define CANOPEN_EMCY_EMCY_MAN_EN     0

// <q CANOPEN_EMCY_HIST_EN> Emergency error history
// <i> If enabled, the standard object directory entry 0x1003 (pre-defined error field) shall be configured according to the CANopen standard.
// <i> Default: 0
#define CANOPEN_EMCY_HIST_EN         0

// <q CANOPEN_EMCY_HIST_MAN_EN> Manufacturer-specific information field within the EMCY history entries
// <i> Entries are located in the standard object "pre-defined error field" at index 0x1003.
// <i> Default: 0
#define CANOPEN_EMCY_HIST_MAN_EN     0

/********************************************************************************************************
 ********************************************************************************************************
 *                                      CANOPEN TIMER MANAGEMENT CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

// <o CANOPEN_TMR_MAX_QTY> Timer action queue length <1-255>
// <i> The timer is used by some communication objects such as NMT heartbeat and by PDOs.
// <i> Default: 5
#define CANOPEN_TMR_MAX_QTY          5

/********************************************************************************************************
 ********************************************************************************************************
 *                                      CANOPEN PDO TRANSFER CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

// <o CANOPEN_RPDO_MAX_QTY> number of active Receive PDOs (RPDO) for a given node <1-512>
// <i> A Receive PDO is described in the node's object dictionary by the RPDO communication parameter (starting at index 0x1400) and mapping parameter (starting at index 0x1600).
// <i> Default: 4
#define CANOPEN_RPDO_MAX_QTY         4

// <o CANOPEN_RPDO_MAX_MAP_QTY> Number of Receive PDO mapping objects <1-64>
// <i> Default: 2
#define CANOPEN_RPDO_MAX_MAP_QTY     2

// <q CANOPEN_RPDO_DYN_COM_EN> Support of dynamic communication profiles within the standard TPDO object entrie
// <i> Standard RPDO object entries are at index 0x1400 (RPDO communication parameter).
// <i> Default: 1
#define CANOPEN_RPDO_DYN_COM_EN      1

// <q CANOPEN_RPDO_DYN_MAP_EN> Support of dynamic mapping profile within the standard RPDO object entries
// <i> Enabling this requires you to also enable support for dynamic communication profiles, constant CANOPEN_RPDO_DYN_COM_EN.
// <i> Default: 1
#define CANOPEN_RPDO_DYN_MAP_EN      1

// <o CANOPEN_TPDO_MAX_QTY> Number of active Transmit PDOs (TPDO) for a given node <1-512>
// <i> A Transmit PDO is described in the node's object dictionary by the TPDO communication parameter (starting at index 0x1800) and mapping parameter (starting at index 0x1A00).
// <i> Default: 4
#define CANOPEN_TPDO_MAX_QTY         4

// <o CANOPEN_TPDO_MAX_MAP_QTY> Number of Transmit PDO mapping objects <1-64>
// <i> Default: 2
#define CANOPEN_TPDO_MAX_MAP_QTY     2

// <q CANOPEN_TPDO_DYN_COM_EN> Support of dynamic communication profile within the standard TPDO object entries
// <i> Standard TPDO object entries are at index 0x1800 (TPDO communication parameter).
// <i> Default: 1
#define CANOPEN_TPDO_DYN_COM_EN      1

// <q CANOPEN_TPDO_DYN_MAP_EN> Support of dynamic mapping profile within the standard TPDO object entries
// <i> Default: 1
#define CANOPEN_TPDO_DYN_MAP_EN      1

/********************************************************************************************************
 ********************************************************************************************************
 *                                      CANOPEN SYNC CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

// <q CANOPEN_SYNC_EN> Synchronous PDOs
// <i> Enables the use of synchronous PDOs.
// <i> Default: 0
#define CANOPEN_SYNC_EN              0

/********************************************************************************************************
 ********************************************************************************************************
 *                                      CANOPEN DEBUG CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

// <q CANOPEN_DBG_CTR_ERR_EN> Error counters
// <i> When enabled, if errors occur during the CAN frames decoding and processing by the CANopen Service task, these errors will be recorded into internal counters. The error counters are per node instance.
// <i> Default: 0
#define CANOPEN_DBG_CTR_ERR_EN       0

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _CANOPEN_CFG_H_
