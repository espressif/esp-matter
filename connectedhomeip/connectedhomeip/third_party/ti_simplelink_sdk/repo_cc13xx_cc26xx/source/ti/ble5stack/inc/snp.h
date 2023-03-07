/******************************************************************************

 @file  snp.h

 @brief main header file for the serial network processor

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2015-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/**
 *  @defgroup SNP SNP
 *  @brief This module describes the Simple Network Processor API
 *  @{
 *  @file  snp.h
 *  @brief      Simple Network Processor API
 */

#ifndef SNP_H
#define SNP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

#include "hal_types.h"

/*********************************************************************
*  EXTERNAL VARIABLES
*/

/** @defgroup SNP_Constants SNP Constants
 * Defines used in SNP
 * @{
 */

/*********************************************************************
 * CONSTANTS
 */

//! \brief Location in memory where the SNP image is located
#define SNP_IMAGE_START           0x00000000

//! \brief list of command type (include SNP subsystem)
/** @defgroup SNP_NPI_TYPE NPI frame type for SNP (NPI cmd 0)
 * @{
 */
//! (0x55) Asynchronous Command type for the BLE simple network processor (cmd0 field of npi frame)
#define SNP_NPI_ASYNC_CMD_TYPE    (RPC_SYS_BLE_SNP+(NPI_MSG_TYPE_ASYNC<<5))
//! (0x35) Synchronous request type for the BLE simple network processor (cmd0 field of npi frame)
#define SNP_NPI_SYNC_REQ_TYPE     (RPC_SYS_BLE_SNP+(NPI_MSG_TYPE_SYNCREQ<<5))
//! (0x75) Synchronous response type for the BLE simple network processor (cmd0 field of npi frame)
#define SNP_NPI_SYNC_RSP_TYPE     (RPC_SYS_BLE_SNP+(NPI_MSG_TYPE_SYNCRSP<<5))
/**
  * @}
  */

//! \brief list of subgroup
// Fixed size, must not exceed 2 bits!
// the upper two bits of NPI frame cmd1 field is reserved for subgroup value.
enum SNP_CMD1_HDR_TYPE {
    SNP_DEVICE_GRP = 0,
    SNP_GAP_GRP,
    SNP_GATT_GRP,
    SNP_RFU_GRP
};

/**
 * @defgroup SNP_DEVICE_CMD_LIST List of commands for device sub group
 * @{
 */
#define SNP_MASK_EVT_REQ                       0x02  //!< SNP Mask Event Request
#define SNP_GET_REVISION_REQ                   0x03  //!< SNP Get Revision Request
#define SNP_HCI_CMD_REQ                        0x04  //!< SNP HCI Command Request
#define SNP_GET_STATUS_REQ                     0x06  //!< SNP Get Status Request
#define SNP_GET_RAND_REQ                       0x07  //!< SNP Get Rand Request
#define SNP_TEST_REQ                           0x10  //!< SNP Test Request
/** @} End SNP_DEVICE_CMD_LIST */

/**
 * @defgroup SNP_DEVICE_RSP_IND_LIST List of indication/response for device sub group
 * @{
 */
#define SNP_POWER_UP_IND                       0x01  //!< SNP Power Up Indication
#define SNP_MASK_EVENT_RSP                     0x02  //!< SNP Mask Event Response
#define SNP_GET_REVISION_RSP                   0x03  //!< SNP Get Revision Response
#define SNP_HCI_CMD_RSP                        0x04  //!< SNP HCI Command Response
#define SNP_EVENT_IND                          0x05  //!< SNP Event Indication
#define SNP_GET_STATUS_RSP                     0x06  //!< SNP Get Status Response
#define SNP_SYNC_ERROR_CMD_IND                 0x07  //!< SNP Sync Error Cmd Indication
#define SNP_GET_RAND_RSP                       0x08  //!< SNP Get Rand Response
#define SNP_TEST_RSP                           0x10  //!< SNP Test Response
/** @} End SNP_RSP_IND_LIST */

/**
 * @defgroup SNP_GAP_CMD_LIST List of GAP commands for device sub group
 * @{
 */
#define SNP_INIT_DEVICE_REQ                    0x41  //!< SNP GAP Init Device Request
#define SNP_START_ADV_REQ                      0x42  //!< SNP GAP Start Adv Request
#define SNP_SET_ADV_DATA_REQ                   0x43  //!< SNP GAP Set Adv Data Request
#define SNP_STOP_ADV_REQ                       0x44  //!< SNP GAP Stop Adv Request
#define SNP_UPDATE_CONN_PARAM_REQ              0x45  //!< SNP GAP Update Connection Param Request
#define SNP_TERMINATE_CONN_REQ                 0x46  //!< SNP GAP Terminate Connection Request
#define SNP_SET_GAP_PARAM_REQ                  0x48  //!< SNP GAP Set Conn Parameters Request
#define SNP_GET_GAP_PARAM_REQ                  0x49  //!< SNP GAP Get Conn Parameters Request
#define SNP_SET_SECURITY_PARAM_REQ             0x4A  //!< SNP GAP Set Security Parameters Request
#define SNP_SEND_SECURITY_REQUEST_REQ          0x4B  //!< SNP GAP Send Security Request
#define SNP_SET_AUTHENTICATION_DATA_REQ        0x4C  //!< SNP GAP Set Auth Data Request
#define SNP_SET_WHITE_LIST_POLICY_REQ          0x4D  //!< SNP GAP White List Policy Request
/** @} End SNP_GAP_CMD_LIST */

/**
 * @defgroup SNP_GAP_RSP_IND_LIST List of GAP responses/indications for device sub group
 * @{
 */
#define SNP_INIT_DEVICE_CNF                    0x41  //!< SNP Init Device Config
#define SNP_SET_ADV_DATA_CNF                   0x43  //!< SNP Set Advertising Data Config
#define SNP_UPDATE_CONN_PARAM_CNF              0x45  //!< SNP Update Connection Param Config
#define SNP_SET_GAP_PARAM_RSP                  0x48  //!< SNP GAP Set Parameter Response
#define SNP_GET_GAP_PARAM_RSP                  0x49  //!< SNP GAP Get Parameter Response
#define SNP_SET_SECURITY_PARAM_RSP             0x4A  //!< SNP Set Security Parameter Response
#define SNP_SEND_AUTHENTICATION_DATA_RSP       0x4C  //!< SNP Send Auth Data Response
#define SNP_SET_WHITE_LIST_POLICY_RSP          0x4D  //!< SNP Set White List Policy Response
/** @} End SNP_GAP_RSP_IND_LIST */

/**
 * @defgroup SNP_GATT_CMD_LIST List of GATT commands for device sub group
 * @{
 */
#define SNP_ADD_SERVICE_REQ                    0x81  //!< SNP Add Service request
#define SNP_ADD_CHAR_VAL_DECL_REQ              0x82  //!< SNP Add Char Value Declaration request
#define SNP_ADD_CHAR_DESC_DECL_REQ             0x83  //!< SNP Add Char Description Declaration request
#define SNP_REGISTER_SERVICE_REQ               0x84  //!< SNP Register Service with GATT request
#define SNP_GET_ATTR_VALUE_REQ                 0x85  //!< SNP Get attribute value request
#define SNP_SET_ATTR_VALUE_REQ                 0x86  //!< SNP Set attribute value request
#define SNP_CHAR_READ_CNF                      0x87  //!< SNP Read Char Configuration request
#define SNP_CHAR_WRITE_CNF                     0x88  //!< SNP Write Char Configuration
#define SNP_SEND_NOTIF_IND_REQ                 0x89  //!< SNP Send Notification/Indication request
#define SNP_CCCD_UPDATED_CNF                   0x8B  //!< SNP CCCD Updated Config request
#define SNP_SET_GATT_PARAM_REQ                 0x8C  //!< SNP Set GATT parameter request
#define SNP_GET_GATT_PARAM_REQ                 0x8D  //!< SNP Get GATT parameter request
#define SNP_REG_PREDEF_SRV_REQ                 0x8E  //!< SNP Register Redefined service request
/** @} End SNP_GATT_CMD_LIST */

/**
 * @defgroup SNP_GATT_RSP_IND_LIST List of GATT responses/indications for device sub group
 * @{
 */
#define SNP_ADD_SERVICE_RSP                    0x81  //!< SNP Add Service response
#define SNP_ADD_CHAR_VAL_DECL_RSP              0x82  //!< SNP Add Char Value Declaration response
#define SNP_ADD_CHAR_DESC_DECL_RSP             0x83  //!< SNP Add Char Description Declaration response
#define SNP_REGISTER_SERVICE_RSP               0x84  //!< SNP Register Service with GATT response
#define SNP_GET_ATTR_VALUE_RSP                 0x85  //!< SNP Get attribute value response
#define SNP_SET_ATTR_VALUE_RSP                 0x86  //!< SNP Set attribute value response
#define SNP_CHAR_READ_IND                      0x87  //!< SNP Read Char Configuration response
#define SNP_CHAR_WRITE_IND                     0x88  //!< SNP Write Char Configuration
#define SNP_SEND_NOTIF_IND_CNF                 0x89  //!< SNP Send Notification/Indication response
#define SNP_CCCD_UPDATED_IND                   0x8B  //!< SNP CCCD Updated Config response
#define SNP_SET_GATT_PARAM_RSP                 0x8C  //!< SNP Set GATT parameter response
#define SNP_GET_GATT_PARAM_RSP                 0x8D  //!< SNP Get GATT parameter response
#define SNP_REG_PREDEF_SRV_RSP                 0x8E  //!< SNP Register Redefined service response
/** @} End SNP_GATT_RSP_IND_LIST */

/** @defgroup SNP_ERRORS list of status generated by the SNP.
 * @{
 */
#define SNP_SUCCESS                            0x00  //!< SNP Success Status
#define SNP_FAILURE                            0x83  //!< SNP Failure Status
#define SNP_INVALID_PARAMS                     0x84  //!< SNP Invalid Parameters Status
#define SNP_CMD_ALREADY_IN_PROGRESS            0x85  //!< SNP Command Already in Progress Status
#define SNP_CMD_REJECTED                       0x86  //!< SNP Command Rejected Status
#define SNP_OUT_OF_RESOURCES                   0x87  //!< SNP Out of Resources Status
#define SNP_UNKNOWN_ATTRIBUTE                  0x88  //!< SNP Unknown Attribute Status
#define SNP_UNKNOWN_SERVICE                    0x89  //!< SNP Unknown Service Status
#define SNP_ALREADY_ADVERTISING                0x8A  //!< SNP Already Advertising Status
#define SNP_NOT_ADVERTISING                    0x8B  //!< SNP Not Advertising Status
#define SNP_HCI_RSP_COLLISION_RSP              0x8C  //!< SNP HCI Response Collision Status
#define SNP_HCI_CMD_UNKNOWN                    0x8D  //!< SNP HCI Command Unknown Status
#define SNP_GATT_COLLISION                     0x8E  //!< SNP GATT Collision Status
#define SNP_NOTIF_IND_NOT_ENABLE_BY_CLIENT     0x8F  //!< SNP Notifications/Indications Not Enabled
#define SNP_NOTIF_IND_NOT_ALLOWED              0x90  //!< SNP Notifications/Indications Not Allowed
#define SNP_NOTIF_IND_NO_CCCD_ATTRIBUTE        0x91  //!< SNP Notifications/Indications No CCCD Attribute
#define SNP_NOT_CONNECTED                      0x92  //!< SNP Not Connected Status
#define SNP_FAILURE_NO_ATT_RSP                 0x93  //!< SNP No Attribute Response Failure Status
/**
 * @}
 */

/** @defgroup SNP_EVENT list of asynchronous events generated by the SNP.
 * @{
 */
#define  SNP_CONN_EST_EVT                      0x0001  //!< SNP Connection Established Event
#define  SNP_CONN_TERM_EVT                     0x0002  //!< SNP Connection Terminated Event
#define  SNP_CONN_PARAM_UPDATED_EVT            0x0004  //!< SNP Connection Parameters Updated Event
#define  SNP_ADV_STARTED_EVT                   0x0008  //!< SNP Advertising Started Event
#define  SNP_ADV_ENDED_EVT                     0x0010  //!< SNP Advertising Ended Event
#define  SNP_ATT_MTU_EVT                       0x0020  //!< SNP Attribute MTU Changed Event
#define  SNP_SECURITY_EVT                      0x0040  //!< SNP Security Event
#define  SNP_AUTHENTICATION_EVT                0x0080  //!< SNP Authentication Event
#define  SNP_ERROR_EVT                         0x8000  //!< SNP Error Event
/**
 * @}
 */
//! \brief this mask check that only meaningful event can be masked/send.
#define SNP_DEV_ALL_EVENT_MASK (SNP_CONN_EST_EVT | SNP_CONN_TERM_EVT | \
SNP_CONN_PARAM_UPDATED_EVT | SNP_ADV_STARTED_EVT | SNP_ADV_ENDED_EVT | \
  SNP_ATT_MTU_EVT | SNP_ERROR_EVT)

/* HCI Opcodes */
/** @defgroup SNP_ALLOWED_HCI list of all HCI commands allowed by the SNP.
 * @{
 */
#define SNP_HCI_OPCODE_EXT_SET_TX_POWER                 0xFC01 //!< HCI Extension Set Tx Power
/// @cond NODOC
#define SNP_HCI_OPCODE_EXT_MODEM_TEST_TX                0xFC08 //!< HCI Extension Modem Test Tx
#define SNP_HCI_OPCODE_EXT_MODEM_HOP_TEST_TX            0xFC09 //!< HCI Extension Modem Hop Test Tx
#define SNP_HCI_OPCODE_EXT_MODEM_TEST_RX                0xFC0A //!< HCI Extension Test Rx
#define SNP_HCI_OPCODE_EXT_END_MODEM_TEST               0xFC0B //!< HCI Extension End Modem Test
/// @endcond // NODOC
#define SNP_HCI_OPCODE_EXT_SET_BDADDR                   0xFC0C //!< HCI Extension Set BD Address
#define SNP_HCI_OPCODE_EXT_SET_SCA                      0xFC0D //!< HCI Extension Set SCA
#define SNP_HCI_OPCODE_EXT_ENABLE_PTM                   0xFC0E //!< HCI Extension Enable PTM
#define SNP_HCI_OPCODE_EXT_SET_MAX_DTM_TX_POWER         0xFC11 //!< HCI Extension Set Max DTM Tx Power
#define SNP_HCI_OPCODE_EXT_RESET_SYSTEM                 0xFC1D //!< HCI Extension Reset System
#define SNP_HCI_OPCODE_READ_BDADDR                      0x1009 //!< HCI Extension Read BD Address
#define SNP_HCI_OPCODE_READ_RSSI                        0x1405 //!< HCI Extension Read RSSI
#define SNP_HCI_OPCODE_LE_RECEIVER_TEST                 0x201D //!< HCI Extension LE Receiver Test
#define SNP_HCI_OPCODE_LE_TRANSMITTER_TEST              0x201E //!< HCI Extension LE Transmitter Test
#define SNP_HCI_OPCODE_LE_TEST_END                      0x201F //!< HCI Extension LE Test End Command
#define SNP_HCI_OPCODE_LE_READ_PHY                      0x2030 //!< HCI LE Read Phy
#define SNP_HCI_OPCODE_LE_SET_DEFAULT_PHY               0x2031 //!< HCI LE Set Default Phy Command
#define SNP_HCI_OPCODE_LE_SET_PHY                       0x2032 //!< HCI LE Set Phy
#define SNP_HCI_OPCODE_EXT_PER                          0xFC14 //!< HCI Extension PER Command
#define SNP_HCI_OPCODE_EXT_DECRYPT                      0xFC05 //!< HCI Extension decrypt encrypted data using AES128
#define SNP_HCI_OPCODE_LE_ENCRYPT                       0x2017 //!< HCI Extension encrypt data using AES128
#define SNP_HCI_OPCODE_EXT_OVERRIDE_SL                  0xFC1A //!< HCI Extension enable or disable suspending slave latency
#define SNP_HCI_OPCODE_EXT_SET_FAST_TX_RESP_TIME        0xFC07 //!< HCI Extension set whether transmit data is sent as soon as possible even when slave latency is used
#define SNP_HCI_OPCODE_EXT_ONE_PKT_PER_EVT              0xFC02 //!< HCI Extension set whether a connection will be limited to one packet per event
#define SNP_HCI_OPCODE_EXT_GET_CONNECTION_INFO          0xFC20 //!< HCI Extension get connection related information
/**
 * @}
 */

//! \brief Number of bytes an HCI OPCODE should be
#define SNP_HCI_OPCODE_SIZE              0x02


/* Advertising */
/** @defgroup SNP_AdvType Advertisement Type
 * @{
 */
#define SNP_ADV_TYPE_CONN                0x00 //!< Connectable undirected advertisement
#define SNP_ADV_TYPE_SCANABLE            0x02 //!< Scannable undirected advertisement
#define SNP_ADV_TYPE_NONCONN             0x03 //!< Non-Connectable undirected advertisement
 /**
 * @}
 */

/** @defgroup SNP_AdvFilterPolicyType Advertisement Filter Policy Type
 * @{
 */
#define SNP_FILTER_POLICY_ALL             0x00 //!< Allow Scan Request from Any, Allow Connect Request from Any (default).
#define SNP_FILTER_POLICY_WHITE           0x03 //!< Allow Scan Request and Connect from White List Only
/**
 * @}
 */

/** @defgroup SNP_AdvBehavior Adv. behavior when connection are established or terminated.
 * @{
 */
//! Advertising is disabled during connection and will not start after.
#define SNP_ADV_STOP_ON_CONN             0x00
//! @brief Advertising will continue with non-connectable advertising when
//! connection is established. period and interval of advertising are different when the device is in a connection. see @ref SNP_startAdv
#define SNP_ADV_RESTART_ON_CONN_EST      0x01
//! @brief Advertising will start with connectable advertising when a
//! connection is terminated.
#define SNP_ADV_RESTART_ON_CONN_TERM     0x02
 /**
 * @}
 */

/** @defgroup SNP_Advbuffer Adv. data buffer
 * @{
 */
//!  data buffer holding the scanning response
#define SNP_SCANRSP_DATA                 0x00 // Scan response data
//!  data buffer holding advertisement data that will be advertise if the device is NOT in a connection
#define SNP_NONCONN_ADV_DATA             0x01 // Non-connectable advertising data.
//!  data buffer holding advertisement data that will be advertise if the device is in a connection
#define SNP_CONN_ADV_DATA                0x02 // Connectable advertising data.
 /**
 * @}
 */

//!< This is the maximum size of a scan response or advertising data buffer.
#define SNP_MAX_ADVDATA_LENGTH           31

//! @brief Connection Terminated (CT)
/** @defgroup SNP_connTerm connection termination reason.
 * @{
 */
#define SNP_CT_SUPERVISOR_TO             0x08 //!< Supervisor timeout
#define SNP_CT_PEER_REQ                  0x13 //!< Peer Requested
#define SNP_CT_HOST_REQ                  0x16 //!< Host Requested
#define SNP_CT_CTRL_PKT_TO               0x22 //!< Control Packet Timeout
#define SNP_CT_CTRL_PKT_INST_PASS        0x28 //!< Control Packet Instance Passed
#define SNP_CT_LSTO_VIOLATION            0x3B //!< LSTO Violation
#define SNP_CT_MIC_FAILURE               0x3D //!< MIC Failure
 /**
 * @}
 */

/* Connection Parameters and Updates */
//! \brief Connection Interval Range (in 1.25ms units, ranging from 7.5ms to 4s)
#define SNP_CONN_INT_MIN                 0x0006  //!< minimal connection interval range: 7.5ms
#define SNP_CONN_INT_MAX                 0x0C80  //!< maximal connection interval range: 4a

//! \brief Slave Latency Ranges
#define SNP_CONN_SL_MIN                  0x0000 //!< minimal slave latency value
#define SNP_CONN_SL_MAX                  0x01F3 //!< maximum slave latency value

//! \brief Supervisor Timeout Range (in 10ms units, ranging from 100ms to 32 seconds)
#define SNP_CONN_SUPERVISOR_TIMEOUT_MIN  0x000A //!< minimum supervision timeout, multiple of 10ms
#define SNP_CONN_SUPERVISOR_TIMEOUT_MAX  0x0C80 //!< maximum supervision timeout, multiple of 10ms

/** @defgroup SNP_SERVICE_TYPE Service Type
 * @{
 */
//! Primary service
#define SNP_PRIMARY_SERVICE                   1
//! Secondary service
#define SNP_SECONDARY_SERVICE                 2
/** @} End SNP_SERVICE_TYPE  */

#define SNP_CHAR_MANAGED_BY_AP             0x00 //!< characteristic value management Management option (RFU)
#define SNP_CHAR_MANAGED_BY_NP             0x01 //!< characteristic value management Management option (RFU)

/** @defgroup UUID_SIZES SNP UUID sizes
 * @{
 */
#define SNP_16BIT_UUID_SIZE                   2 //!< 16 BIT UUID SIZE
#define SNP_128BIT_UUID_SIZE                 16 //!< 128 BIT UUID SIZE
/** @} End UUID_SIZES  */

/** @defgroup SNP_PERMIT_BITMAPS_DEFINES GATT Attribute Access Permissions Bit Fields
 * @{
 */
//! @brief For now, Only read and write permissions are allowed.
//! permission to read the attribute value
#define SNP_GATT_PERMIT_READ                0x01
//! permission to write the attribute value
#define SNP_GATT_PERMIT_WRITE               0x02
//! permission to authenticated read the attribute value
#define SNP_GATT_PERMIT_AUTHEN_READ         0x04
//! permission to authenticated write the attribute value
#define SNP_GATT_PERMIT_AUTHEN_WRITE        0x08
//! permission to encrypted read the attribute value
#define SNP_GATT_PERMIT_ENCRYPT_READ        0x40
//! permission to encrypted write the attribute value
#define SNP_GATT_PERMIT_ENCRYPT_WRITE       0x80

/** @} End SNP_PERMIT_BITMAPS_DEFINES */

//! \brief mask for clearing RFU bits on the attribute permissions field
#define SNP_GATT_PERM_RFU_MASK   (SNP_GATT_PERMIT_READ         | \
                                  SNP_GATT_PERMIT_WRITE        | \
                                  SNP_GATT_PERMIT_AUTHEN_READ  | \
                                  SNP_GATT_PERMIT_AUTHEN_WRITE | \
                                  SNP_GATT_PERMIT_ENCRYPT_READ | \
                                  SNP_GATT_PERMIT_ENCRYPT_WRITE)

/** @defgroup SNP_PROP_BITMAPS_DEFINES GATT Characteristic Properties Bit Fields
 * @{
 */
#define SNP_GATT_PROP_READ               0x0002 //!< Read
#define SNP_GATT_PROP_WRITE_NORSP        0x0004 //!< Write without response
#define SNP_GATT_PROP_WRITE              0x0008 //!< Writes
#define SNP_GATT_PROP_NOTIFICATION       0x0010 //!< Notifications
#define SNP_GATT_PROP_INDICATION         0x0020 //!< Indications
/** @} End SNP_PROP_BITMAPS_DEFINES */

//! \brief Maximum Length of the Characteristic Value Attribute
#define SNP_GATT_CHAR_MAX_LENGTH            512

/** @defgroup SNP_IND_TYPE SNP indication type
 * @{
 */
//! \brief Notification type
#define SNP_SEND_NOTIFICATION               0x01
//! \brief Indication type
#define SNP_SEND_INDICATION                 0x02
/** @} End SNP_IND_TYPE */


/** @defgroup SNP_PREDEF_SERVICE Predefined services
 * @{
 */
#define SNP_GGS_SERV_ID                     0x01 //!< Generic Access Service
#define SNP_DIS_SERV_ID                     0x02 //!< Device Info Service
/** @} End SNP_PREDEF_SERVICE */

/** @defgroup SNP_DEV_INFO_SERVICE Device Information Service Parameters
 * @{
 */
#define SNP_DEVINFO_SYSTEM_ID                  0 //!< RW  uint8_t[#SNP_DEVINFO_SYSTEM_ID_LEN]
#define SNP_DEVINFO_MODEL_NUMBER               1 //!< RW  uint8_t[#SNP_DEVINFO_STRING_LEN]
#define SNP_DEVINFO_SERIAL_NUMBER              2 //!< RW  uint8_t[#SNP_DEVINFO_STRING_LEN]
#define SNP_DEVINFO_FIRMWARE_REV               3 //!< RW  uint8_t[#SNP_DEVINFO_STRING_LEN]
#define SNP_DEVINFO_HARDWARE_REV               4 //!< RW  uint8_t[#SNP_DEVINFO_STRING_LEN]
#define SNP_DEVINFO_SOFTWARE_REV               5 //!< RW  uint8_t[#SNP_DEVINFO_STRING_LEN]
#define SNP_DEVINFO_MANUFACTURER_NAME          6 //!< RW  uint8_t[#SNP_DEVINFO_STRING_LEN]
#define SNP_DEVINFO_11073_CERT_DATA            7 //!< RW variable length
#define SNP_DEVINFO_PNP_ID                     8 //!< RW  uint8_t[#SNP_DEVINFO_PNP_ID_LEN]

//! \brief System ID length
#define SNP_DEVINFO_SYSTEM_ID_LEN              8

//! \brief PnP ID length
#define SNP_DEVINFO_PNP_ID_LEN                 7

//! \brief default string length, including the null terminated char.
// this is for information only, do not change it here.
#define SNP_DEVINFO_STRING_LEN                 21
/** @} End SNP_DEV_INFO_SERVICE */

/** @defgroup SNP_GAP_SERVICE GAP GATT Server Parameters
 * @{
 */
#define SNP_GGS_DEVICE_NAME_ATT                0   //!< RW  uint8_t[#SNP_GAP_DEVICE_NAME_LEN]
#define SNP_GGS_APPEARANCE_ATT                 1   //!< RW  uint16_t
#define SNP_GGS_PERI_CONN_PARAM_ATT            4   //!< RW  sizeof(#snpGapPeriConnectParams_t)
#define SNP_GGS_W_PERMIT_DEVICE_NAME_ATT       6   //!< SET ONLY (W   uint8_t)
#define SNP_GGS_W_PERMIT_APPEARANCE_ATT        7   //!< SET ONLY (W   uint8_t)

//! \brief GAP Device Name Length, excluding null-terminate character
#define SNP_GAP_DEVICE_NAME_LEN                     21

// Privacy Flag States
#define SNP_GAP_PRIVACY_DISABLED            0x00 //!< Privacy Flag State Disabled
#define SNP_GAP_PRIVACY_ENABLED             0x01 //!< Privacy Flag State Enabled

/** @} End SNP_GAP_SERVICE */

/** @defgroup SNP_GATT_DESC_HEADER_LIST SNP GATT Descriptors Header
 * @{
 */
#define SNP_DESC_HEADER_GEN_SHORT_UUID      0x01 //!< Descriptor Header for Generic Short UUID
#define SNP_DESC_HEADER_GEN_LONG_UUID       0x02 //!< Descriptor Header for Generic Long UUID
#define SNP_DESC_HEADER_CCCD                0x04 //!< Descriptor Header for CCCD
#define SNP_DESC_HEADER_FORMAT              0x08 //!< Descriptor Header for Format
#define SNP_DESC_HEADER_USER_DESC           0x80 //!< Descriptor Header for User Description
#define SNP_DESC_HEADER_UNSUPPORTED_MASK    0x70 //!< Descriptor Header for Unsupported
/** @} End SNP_GATT_DESC_HEADER_LIST */

#define SNP_GATT_CLIENT_CFG_NOTIFY          0x0001 //!< The Characteristic Value shall be notified
#define SNP_GATT_CLIENT_CFG_INDICATE        0x0002 //!< The Characteristic Value shall be indicated

/** @defgroup SNP_GATT_RSP_OPTIONS Options for Response Needed Field
 * @{
 */
#define SNP_RESPONSE_NOT_NEEDED             0x00 //!< AP Does Not Need To Respond
#define SNP_RESPONSE_NEEDED                 0x01 //!< AP Response Needed
/** @} End SNP_GATT_RSP_OPTIONS */

#define SNP_INVALID_HANDLE                  0x0000 //!< Invalid Handle


/** @defgroup SNP_GAP_TERM_CONN_OPTION GAP GATT Terminate connection request option field
 * @{
 */
#define SNP_GAP_TERM_CONN_DEFAULT           0x00  //!< Default disconnection, send BLE packet to tell the remote side to disconnect.
#define SNP_GAP_TERM_CONN_IMMEDIATLY        0x01  //!< Abrupt disconnection, stop sending RF packets. The other side will timeout.
/** @} End SNP_GAP_TERM_CONN_OPTION */

/** @defgroup SNP_GAPBOND_PARAMS GAP Bond Manager parameters
 * @{
 */
#define SNP_GAPBOND_PAIRING_MODE            0x00  //!< Behavior on connections
#define SNP_GAPBOND_IO_CAPABILITIES         0x01  //!< IO Capabilities
#define SNP_GAPBOND_BONDING_ENABLED         0x02  //!< Toggle Bonding
#define SNP_GAPBOND_ERASE_ALLBONDS          0x03  //!< Erase all Bonds
#define SNP_GAPBOND_LRU_BOND_REPLACEMENT    0x04  //!< Overwrite Least Recently Used Bond when record is full.
/** @} End SNP_GAPBOND_PARAMS */

/** @defgroup SNP_SECURITY_STATE Security States
 * @{
 */
#define SNP_GAPBOND_PAIRING_STATE_COMPLETE   0x01  //!< Pairing complete
#define SNP_GAPBOND_PAIRING_STATE_BONDED     0x02  //!< Devices bonded
#define SNP_GAPBOND_PAIRING_STATE_BOND_SAVED 0x03  //!< Bonding record saved in NV
/** @} End SNP_SECURITY_STATE */

/** @} End SNP_Constants */

/*-------------------------------------------------------------------
* TYPEDEFS - Initialization and Configuration
*/
/*** Device Data Types ***/

/** @defgroup SNP_Structs SNP Structures
 * @{
 */

/**
 * @struct snpMaskEventReq_t
 *
 * @brief Parameter Structure for @ref SNP_maskEvt
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint16_t eventMask; //!< 2 Byte bitmask of Events to be masked
                      //!
                      //! list of events available are listed here: @ref SNP_EVENT
} snpMaskEventReq_t;

/**
 * @struct snpMaskEventRsp_t
 *
 * @brief Parameter Structure for @ref SNP_MASK_EVENT_RSP
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint16_t maskedEvent; //!< 2 Byte Events that has been masked.
                        //!
                        //! list of events available are listed here: @ref SNP_EVENT
} snpMaskEventRsp_t;

/**
 * @struct snpTestCmdRsp_t
 *
 * @brief Parameter Structure for @ref SNP_executeTestCmd
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint16_t  memAlo;  //!< 2 Byte HEAP allocated memory
  uint16_t  memMax;  //!< 2 Byte maximum HEAP ever allocated since startup
  uint16_t  memSize; //!< 2 Byte total HEAP size
} snpTestCmdRsp_t;

/**
 * @struct snpHciCmdReq_t
 *
 * @brief Parameter Structure for @ref SNP_executeHCIcmd
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint16_t  opcode; //!< 2 Byte HCI opcode to execute
                    //!
                    //! The list of available HCI opcode is listed here: @ref SNP_ALLOWED_HCI
  uint8_t   *pData; //!< Pointer Parameters of the HCI opcode.
                    //!
                    //! Parameter depends of the HCI command being used.
                    //! These parameter are identical to the ones define in TI HCI vendor guide
                    //! and by the Bluetooth Specification
} snpHciCmdReq_t;

/**
 * @struct snpHciCmdRsp_t
 *
 * @brief Parameter Structure for @ref SNP_HCI_CMD_RSP
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t  status; //!< 1 Byte status of the request (SUCCESS or Error, @ref SNP_ERRORS)
  uint16_t opcode; //!< 2 Byte HCI opcode that was executed in the request, @ref SNP_ALLOWED_HCI
  uint8_t  *pData; //!< Pointer Result parameters for this opcode
                   //!
                   //! The expected result is the same as described in the HCI vendor guide or
                   //! Bluetooth Specification.
                   //! The Event Code may not be the same, the opcode referenced the request
                   //! is sent back instead. (in the snpHciCmdRsp_t::opcode field)
} snpHciCmdRsp_t;

/**
 * @struct snpGetRevisionRsp_t
 *
 * @brief Parameter Structure for @ref SNP_getRev
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t  status;            //!< 1 Byte status of the request (SUCCESS or Error, @ref SNP_ERRORS)
  uint16_t snpVer;            //!< 2 Byte Version of the SNP (major, minor)
  uint8_t  stackBuildVer[10]; //!< 10 Byte Stack Revision see TI HCI vendor guide.
} snpGetRevisionRsp_t;

/**
 * @struct snpGetRandRsp_t
 *
 * @brief Parameter Structure for the response to @ref SNP_getRev
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint32_t  rand;            //!< 32-bit Random Number generated by TRNG
} snpGetRandRsp_t;

/**
 * @struct snpGetStatusCmdRsp_t
 * @brief Parameter Structure for the response to @ref SNP_getStatus
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t  gapRoleStatus;  //!< @brief 1 Byte Current state of the GAP Role ( @ref gaprole_States_t)
                           //!
                           //! Currently, Only Peripheral: \n
                           //! is the only possible GAP Peripheral Role State
  uint8_t  advStatus;      //!< @brief 1 Byte Current state of advertising :
                           //!
                           //! TRUE= advertisement enable,  \n
                           //! FALSE = advertisement disable \n
  uint8_t  ATTstatus;      //!< @brief 1 Byte Current state of the GATT server
                           //!
                           //! TRUE= operation ongoing, snpGetStatusCmdRsp_t::ATTmethod will indicate the currently running operation \n
                           //! FALSE = no operation \n
  uint8_t  ATTmethod;      //!< 1 Byte Current GATT operation in progress if snpGetStatusCmdRsp_t::ATTstatus is set to TRUE
} snpGetStatusCmdRsp_t;

/*** GAP Data Types ***/

/**
 * @struct snpStartAdvReq_t
 *
 * @brief Parameter Structure for @ref SNP_startAdv
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t  type;                //!< 1 Byte Type of Advertisement, see @ref SNP_AdvType
  uint16_t timeout;             //!< @brief 2 Byte Timeout of the advertisement.
                                //!
                                //! unit is in 625us period. value of 0 mean infinite advertisement.
  uint16_t interval;            //!< @brief 2 Byte Interval between advertisement event in 625us period.
                                //!
                                //! unit is in 625us period.
  uint8_t  filterPolicy;        //!< @brief 1 Byte Advertising filter policy, Reserved for Future Use
  uint8_t  initiatorAddrType;   //!< @brief 1 Byte Initiator Address Type, Reserved for Future Use
  uint8_t  initiatorAddress[6]; //!< @brief 6 Byte Initiator Address, Reserved for Future Use
  uint8_t  behavior;            //!< 1 Byte Behavior of Advertisement on connection event, @ref SNP_AdvBehavior
} snpStartAdvReq_t;


/**
 * @struct snpSetAdvDataReq_t
 *
 * @brief Parameter Structure for @ref SNP_setAdvData
 *
 *  The maximum advertisement / scan response size is 31 Bytes.
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t type;         //!< 1 Byte Type of Advertisement data to update, see @ref SNP_Advbuffer
  uint8_t *pData;       //!< Pointer to the advertisement data buffer to update, up to 31 Bytes
} snpSetAdvDataReq_t;

/**
 * @struct snpTermConnReq_t
 *
 * @brief Connection Termination Request Structure
 *
 *  Only one connection is supported at the moment. Handle should be included anyway.
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint16_t connHandle; //!< 2 Byte Connection Handle to terminate
  uint8_t  option;     //!< 1 Byte Type of disconnection requested. see @ref SNP_GAP_TERM_CONN_OPTION.
} snpTermConnReq_t;

/**
 * @struct snpUpdateConnParamReq_t
 *
 * @brief Parameter Structure for @ref SNP_updateConnParam
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint16_t connHandle;  //!< 2 Byte handle of the connection to update parameters to.
  uint16_t intervalMin; //!< @brief 2 Byte Minimum value for the connection event interval.
                        //!
                        //!< This shall be less than or equal to intervalMax.\n
                        //! Range: 0x0006 to 0x0C80 \n
                        //! Time = intervalMin * 1.25 msec \n
                        //! Time Range: 7.5 msec to 4 seconds.
  uint16_t intervalMax; ///< @brief 2 Byte Maximum value for the connection event interval.
                        //!
                        /// This shall be greater than or equal to intervalMin.\n
                        //! Range: 0x0006 to 0x0C80 \n
                        //! Time = intervalMax * 1.25 msec \n
                        //! Time Range: 7.5 msec to 4 seconds.
  uint16_t slaveLatency; //!< @brief 2 Byte Slave latency for the connection in number of connection events.
                         //!
                         //! Range: 0x0000 to 0x01F3
  uint16_t supervisionTimeout; //!<  @brief 2 Byte Supervision timeout for the LE Link.
                               //!
                               //! Range: 0x000A to 0x0C80 \n
                               //! Time = connTimeout * 10 msec \n
                               //! Time Range: 100 msec to 32 seconds

} snpUpdateConnParamReq_t;

/**
 * @struct snpUpdateConnParamCnf_t
 *
 * @brief Parameter Structure for @ref SNP_UPDATE_CONN_PARAM_CNF
 *
 * This does not mean parameters have been changed, only that the NP has successfully sent the request.
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t  status;      //!< status of the request (SUCCESS or Error, @ref SNP_ERRORS)
  uint16_t connHandle;  //!< handle of the connection the parameter update was intended for
} snpUpdateConnParamCnf_t;

/**
 * @struct snpSetGapParamReq_t
 * @brief Parameter Structure for @ref SNP_setGapParam / @ref SNP_SET_GAP_PARAM_REQ
 *
 * This does not mean parameters have been changed, only that the NP has successfully sent the request.
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 *
 * @typedef snpGetGapParamReq_t
 * @brief Parameter Structure for @ref SNP_getGapParam / @ref SNP_GET_GAP_PARAM_REQ - Same Structure as @ref snpSetGapParamReq_t
 *
 * This does not mean parameters have been changed, only that the NP has successfully sent the request.
 *
 * Same Structure as @ref snpSetGapParamReq_t
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 *
 * @typedef snpSetSecParamReq_t
 * @brief Parameter Structure for @ref SNP_setSecurityParams / @ref SNP_SET_SECURITY_PARAM_REQ - Same Structure as @ref snpSetGapParamReq_t
 *
 *  2 Byte Structure Details:
 *
 *  (Byte 1) Parameter : Byte 2 Possible Values                            \n
 *  (0x00) Pairing Modes: @ref GAPBondMgr_Pairing_Modes                    \n
 *  (0x01) IO Capabilities of the device: @ref GAPBondMgr_IO_Caps          \n
 *  (0x02) Bonding: TRUE(enable) or FALSE(disable)                         \n
 *  (0x03) Erase All Bonds: (no parameter values)                          \n
 *  (0x04) Replace Least Recently Used Bond: TRUE(enable) or FALSE(disable)
 *
 * Same Structure as @ref snpSetGapParamReq_t
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint16_t paramId;  //!< 2 Byte GAP Parameter ID, @ref GAP_Params or @ref snpSetSecParamReq_t
  uint16_t value;    //!< 2 Byte Parameter Value set if for SET command
} snpSetGapParamReq_t, snpGetGapParamReq_t, snpSetSecParamReq_t;

/**
 * @struct snpGetGapParamRsp_t
 *
 * @brief Parameter Structure for the Get GAP Parameter Value Response
 *
 * Used for @ref SNP_GET_GAP_PARAM_RSP
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t  status;      //!< 1 Byte Status of the request (SUCCESS or Error, @ref SNP_ERRORS)
  uint16_t paramId;     //!< 2 Byte GAP Parameter ID, @ref GAP_Params
  uint16_t value;       //!< 2 Byte Parameter Value retrieved.
} snpGetGapParamRsp_t;

/**
 *  @struct snpSetAuthDataReq_t
 *
 *  @brief Parameter Structure for @ref SNP_setAuthenticationData / @ref SNP_SET_AUTHENTICATION_DATA_REQ
 *
 *      For Passcode: 000000 - 999999
 *
 *      For Numeric Comparison: TRUE(match) or FALSE(no match)
 *
 *  This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint32_t authData;    //!< 4 Byte Authentication Data. 000000 - 999999 or TRUE/FALSE
} snpSetAuthDataReq_t;

/*** GATT Data Types ***/

/**
 * @struct snpAddServiceReq_t
 *
 * @brief Add Service Request Structure for @ref SNP_addService / @ref SNP_ADD_SERVICE_REQ
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t type;         //!< 1 Byte Type of the service to add. see @ref SNP_SERVICE_TYPE
  uint8_t UUID[16];     //!< 16 Byte UUID of the service, up to 16 Bytes
                        //! @ref UUID_desc to know more about how to use UUID field.\n
                        //! UUID can be 2 bytes only. \n
                        //! at reception of this request, the length of the UUID can be deduce from the NPI frame length.
                        //! at transmission of this request, the length of UUID will need to be indicated outside of this structure.
} snpAddServiceReq_t;

/**
 * @struct snpAddCharValueDeclReq_t
 *
 * @brief Parameter Structure used for @ref SNP_addCharValueDecl / @ref SNP_ADD_CHAR_VAL_DECL_REQ
 *
 * Characteristic Value is managed by the AP.
 *
 *  snpAddCharValueDeclReq_t::charValPerms are defined by a profile or are implementation specific.
 *  TI BLE-Stack has define its own permissions: @ref SNP_PERMIT_BITMAPS_DEFINES. \n
 *  Basic filtering is done using those values For example, a read request on a characteristic value which does not
 *  permit reading will be rejected by the stack without AP interaction.\n
 *
 *  snpAddCharValueDeclReq_t::charValProps are defined by the BLE specification, @ref SNP_PROP_BITMAPS_DEFINES.
 *  TI BLE-Stack expect the properties to be align logically with the permission. For example, a read permission
 *  on an attribute should also have a read property.
 *
 *  snpAddCharValueDeclReq_t::UUID refer to @ref UUID_desc
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t  charValPerms;  //!< 1 Byte permissions of the value attribute, see @ref SNP_PERMIT_BITMAPS_DEFINES
  uint16_t charValProps;  //!< 2 Byte property of the value attribute, see @ref SNP_PROP_BITMAPS_DEFINES
  uint8_t  mgmtOption;    //!< 1 Byte Reserved for future use
  uint16_t charValMaxLen; //!< 1 Byte Reserved for future use Range: 0-512
                          //!
                          //!  Since for now the characteristic value is managed only by the AP, this length field is not useful. Reserve for future use. \n
  uint8_t  UUID[16];      //!< 16 Bytes UUID of the characteristic. see @ref UUID_desc
                          //!
                          //! UUID can be 2 bytes only.
                          //! at reception of this request, the length of the UUID can be deduce from the NPI frame length.
                          //! at transmission of this request, the length of UUID will need to be indicated outside of this structure.
} snpAddCharValueDeclReq_t;

/**
 * @struct snpAddCharValueDeclRsp_t
 *
 * @brief Parameter Structure for Response to @ref SNP_addCharValueDecl / @ref SNP_ADD_CHAR_VAL_DECL_RSP
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t  status;          //!< 1 Byte status of the request (SUCCESS or Error, @ref SNP_ERRORS)
  uint16_t attrHandle;      //!< 2 Byte attribute handle of the characteristic value.
} snpAddCharValueDeclRsp_t;

/**
 * @struct snpAddAttrCccd_t
 *
 * @brief Parameter Structure used to add a Client Characteristic Configuration Description Attribute
 *
 *  Used with @ref snpAddCharDescDeclReq_t
 *
 *  snpAddAttrCccd_t::perms are defined by a profile or are implementation specific.
 *  TI BLE-Stack has define its own permissions: @ref SNP_PERMIT_BITMAPS_DEFINES. \n
 *  Basic filtering is done using those values For example, a read request on a characteristic value which does not
 *  permit reading will be rejected by the stack without AP interaction.\n
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t perms;        //!< 1 Byte permissions of the attribute, @ref SNP_PERMIT_BITMAPS_DEFINES
} snpAddAttrCccd_t;

/**
 * @struct snpAddAttrFormat_t
 *
 * @brief Parameter Structure used to add a Format attribute
 *
 *  Used with @ref snpAddCharDescDeclReq_t
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t  format;      //!<  1 Byte Format, as described by BLE Spec, Vol 3, Part G, Chapter 3.3
  uint8_t  exponent;    //!<  1 Byte Exponent, as described by BLE Spec, Vol 3, Part G, Chapter 3.3
  uint16_t unit;        //!<  2 Byte Unit, as described by BLE Spec, Vol 3, Part G, Chapter 3.3
  uint8_t  namespace;   //!<  1 Byte Namespace, as described by BLE Spec, Vol 3, Part G, Chapter 3.3
  uint16_t desc;        //!<  2 Byte Desc, as described by BLE Spec, Vol 3, Part G, Chapter 3.3
} snpAddAttrFormat_t;

/**
 * @struct snpAddAttrUserDesc_t
 *
 * @brief Parameter Structure used to add a User Descriptor attribute
 *
 *  Used with @ref snpAddCharDescDeclReq_t
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t  perms;       //!<  1 Byte Permissions of the attribute, @ref SNP_PERMIT_BITMAPS_DEFINES, force to Read only
  uint16_t maxLen;      //!<  2 Byte Maximum possible length of the string, plus one byte for null terminator (range from 1 to 512)
  uint16_t initLen;     //!<  2 Byte initial length of the string, plus one byte for null terminator (must be <= snpAddAttrUserDesc_t::maxLen)
  uint8_t  *pDesc;      //!<  Pointer to initial string (must be null terminated)
} snpAddAttrUserDesc_t;

/**
 * @struct snpAddAttrGenShortUUID_t
 *
 * @brief Parameter Structure used to add a generic attribute descriptor (short UUID)
 *
 *  Used with @ref snpAddCharDescDeclReq_t
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t  perms;       //!< 1 Byte Permissions of the attribute, see @ref SNP_PERMIT_BITMAPS_DEFINES, force to Read only
  uint16_t maxLen;      //!< 2 Byte Reserved for future use Range: 0-512
                        //!
                        //!  Since the characteristic value is managed only by the AP, this length field is not useful. Reserved for future use. \n
  uint8_t  UUID[2];     //!< 2 Byte UUID of the attribute. see @ref UUID_desc
} snpAddAttrGenShortUUID_t;

/**
 * @struct snpAddAttrGenLongUUID_t
 *
 * @brief Parameter Structure used to add a generic attribute descriptor (long UUID)
 *
 *  Used with @ref snpAddCharDescDeclReq_t
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t  perms;       //!< 1 Byte Permissions of the attribute, see @ref SNP_PERMIT_BITMAPS_DEFINES, force to Read only
  uint16_t maxLen;      //!< 2 Byte Reserved for future use Range: 0-512
                        //!
                        //!  Since the characteristic value is managed only by the AP, this length field is not useful. Reserved for future use. \n
  uint8_t  UUID[16];    //!< 16 Byte UUID of the attribute. see @ref UUID_desc
} snpAddAttrGenLongUUID_t;

/**
 * @struct snpAddCharDescDeclReq_t
 *
 * @brief Parameter Structure for @ref SNP_addDescriptionValue
 *
 *  snpAddCharDescDeclReq_t::header can add multiple attribute types to the
 *  characteristic. When a bit is set corresponding to the attribute type,
 *  the corresponding set of Parameter Structures must be present to be added.
 *
 *  Parameters Structures must appear in the same order as the bits appear in
 *  snpAddCharDescDeclReq_t::header (from least-significant bit to most-significant bit).
 *
 *  @par    Example: Adding CCCD and User Description Attributes
 *
 *  The first byte sent will be snpAddCharDescDeclReq_t::header will be (0x84),
 *  followed the CCCD Parameter Structure, @ref snpAddAttrCccd_t (1 Byte), and
 *  finally followed by User Descriptor Parameter Structure @ref snpAddAttrUserDesc_t (n Bytes)
 *
 * This packet structure is not used to create a NPI frame
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t                       header;      //!< 1 Byte Header Field, @ref SNP_GATT_DESC_HEADER_LIST
  snpAddAttrGenShortUUID_t      *pShortUUID; //!< Pointer to Parameter Structure, @ref snpAddAttrGenShortUUID_t
  snpAddAttrGenLongUUID_t       *pLongUUID;  //!< Pointer to Parameter Structure, @ref snpAddAttrGenLongUUID_t
  snpAddAttrCccd_t              *pCCCD;      //!< Pointer to Parameter Structure, @ref snpAddAttrCccd_t
  snpAddAttrFormat_t            *pFormat;    //!< Pointer to Parameter Structure, @ref snpAddAttrFormat_t
  snpAddAttrUserDesc_t          *pUserDesc;  //!< Pointer to Parameter Structure, @ref snpAddAttrUserDesc_t
} snpAddCharDescDeclReq_t;

/**
 * @struct snpAddCharDescDeclRsp_t
 *
 * @brief Parameter Structure to add new Characteristic Description Response @ref SNP_ADD_CHAR_DESC_DECL_RSP
 *
 * snpAddCharDescDeclRsp_t::handles will be populated with attributes added
 * based on the header, starting from LSB to MSB.
 *
 * This packet structure is not used to create a NPI frame
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t  status;       //!< 1 Byte Command Status @ref SNP_ERRORS
  uint8_t  header;       //!< 1 Byte Header bit field of request.
                         //!
                         //!  Defines which attributes are added. \n
                         //!  @ref SNP_GATT_DESC_HEADER_LIST
  uint16_t handles[6];   //!< Maximum handles returned is 6. One for each possible attribute added. Header will determine the number of actual handles returned
} snpAddCharDescDeclRsp_t;

/**
 * @struct snpRegisterServiceRsp_t
 *
 * @brief Parameter Structure for @ref SNP_registerService / @ref SNP_REGISTER_SERVICE_RSP
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t  status;      //!< 1 Byte Status of the request (SUCCESS or Error, @ref SNP_ERRORS)
  uint16_t startHandle; //!< 2 Byte First attribute handle of the registered service
  uint16_t endHandle;   //!< 2 Byte Last attribute handle of the registered service
} snpRegisterServiceRsp_t;

/**
 * @struct snpCharReadInd_t
 *
 * @brief Parameter Structure for @ref SNP_CHAR_READ_IND / @ ref SNP_charRead
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint16_t connHandle;  //!< 2 Byte Handle of the connection
  uint16_t attrHandle;  //!< 2 Byte Handle of the characteristic value attribute being read
  uint16_t offset;      //!< 2 Byte Offset of the characteristic to start reading from
  uint16_t maxSize;     //!< 2 Byte Max Size of the data to read
} snpCharReadInd_t;

/**
 * @struct snpCharReadCnf_t
 *
 * @brief Parameter Structure for @ref SNP_CHAR_READ_CNF / @ref snpCharReadCnf_t
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t  status;      //!< 1 Byte Status of the request (SUCCESS or Error, @ref SNP_ERRORS)
  uint16_t connHandle;  //!< 2 Byte Handle of the connection
  uint16_t attrHandle;  //!< 2 Byte Handle of the characteristic value attribute being read
  uint16_t offset;      //!< 2 Byte Offset of the characteristic to start reading from
  uint8_t *pData;       //!< Pointer of Characteristic value data \n
                        //!  From snpCharReadCnf_t::offset to offset + snpCharReadInd_t::maxSize -1.
} snpCharReadCnf_t;

/**
 * @struct snpCharWriteInd_t
 *
 * @brief Parameter Structure for the Characteristic Write Request Indication data
 *
 * Used for @ref SNP_CHAR_WRITE_IND
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint16_t connHandle; //!< 2 Byte Handle of the connection
  uint16_t attrHandle; //!< 2 Byte Handle of the characteristic value attribute being written
  uint8_t  rspNeeded;  //!< 1 Byte Indicate if the AP must answer this indication or not, @ref SNP_GATT_RSP_OPTIONS
  uint16_t offset;     //!< 2 Byte Offset of the characteristic to start writing to
  uint8_t *pData;      //!< Pointer to Data to write to the characteristic value,
                       //!  from snpCharWriteInd_t::offset to size of data (deduced from NPI frame length)
} snpCharWriteInd_t;

/**
 * @struct snpCharWriteCnf_t
 *
 * @brief Parameter Structure for @ref SNP_writeCharCnf / @ref SNP_CHAR_WRITE_CNF
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 *
 * @typedef snpCharCfgUpdatedRsp_t
 *
 * @brief Parameter Structure for @ref SNP_processCharConfigUpdatedCnf / @ref SNP_CCCD_UPDATED_CNF - Same Structure as @ref snpCharWriteCnf_t
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 *
 * @typedef snpNotifIndCnf_t
 *
 * @brief Parameter Structure for SNP_sendNotifcnf_rsp - Same Structure as @ref snpCharWriteCnf_t
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t  status;      //!< 1 Byte Status of the request (SUCCESS or Error, @ref SNP_ERRORS)
  uint16_t connHandle;  //!< 2 Byte Handle of the connection
} snpCharWriteCnf_t, snpCharCfgUpdatedRsp_t,  snpNotifIndCnf_t ;

/**
 * @struct snpNotifIndReq_t
 * @brief Parameter Structure for the Characteristic Notification and Indication Request data
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint16_t connHandle;  //!< 2 Byte Handle of the connection
  uint16_t attrHandle;  //!< 2 Byte Handle of the characteristic value attribute to notify/indicate
  uint8_t authenticate; //!< 1 Byte Reserved for future use
  uint8_t type;         //!< 1 Byte Type of the request, see @ref SNP_IND_TYPE
  uint8_t *pData;       //!< Pointer to
} snpNotifIndReq_t;

/**
 * @struct snpSetGattParamRsp_t
 *
 * @brief   1 Byte Command Status Response
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 *
 * @typedef snpSetAdvDataCnf_t
 *
 * @brief   1 Byte Command Status Response
 *
 * Used by @ref SNP_SET_ADV_DATA_CNF event
 *
 * Same Structure as @ref snpSetGattParamRsp_t
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 *
 * @typedef snpSetGapParamRsp_t
 *
 * @brief   1 Byte Command Status Response
 *
 * Used for @ref SNP_SET_GAP_PARAM_RSP
 *
 * Same Structure as @ref snpSetGattParamRsp_t
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 *
 * @typedef snpAddServiceRsp_t
 *
 * @brief   1 Byte Command Status Response
 *
 * Used for @ref SNP_ADD_SERVICE_RSP
 *
 * Same Structure as @ref snpSetGattParamRsp_t
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 *
 * @typedef snpSetSecParamRsp_t
 *
 * @brief   1 Byte Command Status Response
 *
 * Used for @ref SNP_SET_SECURITY_PARAM_RSP
 *
 * Same Structure as @ref snpSetGattParamRsp_t
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 *
 * @typedef snpSetAuthDataRsp_t
 *
 * @brief   1 Byte Command Status Response
 *
 * Used  for @ref SNP_SEND_AUTHENTICATION_DATA_RSP
 *
 * Same Structure as @ref snpSetGattParamRsp_t
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 *
 * @typedef snpSetWhiteListRsp_t
 *
 * @brief   1 Byte Command Status Response
 *
 * Used for @ref SNP_SET_WHITE_LIST_POLICY_RSP
 *
 * Same Structure as @ref snpSetGattParamRsp_t
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 *
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t status;       //!< 1 Byte Command Status (SUCCESS or Error, @ref SNP_ERRORS)
}snpSetGattParamRsp_t, snpSetAdvDataCnf_t, snpSetGapParamRsp_t,
snpAddServiceRsp_t, snpSetSecParamRsp_t, snpSetAuthDataRsp_t,
snpSetWhiteListRsp_t;

/**
 * @struct snpSetGattParamReq_t
 *
 * @brief Parameter Structure for @ref SNP_setGATTParam / @ref SNP_SET_GATT_PARAM_RSP
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 *
 * @typedef snpGetGattParamRsp_t
 *
 * @brief Parameter Structure for @ref SNP_getGATTParam / @ref SNP_GET_GATT_PARAM_RSP
 *
 * Same Structure as @ref snpSetGattParamReq_t
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t serviceID;    //!< 1 Byte ID of the service containing the parameter
                        //!
                        //! Existing list of service can be found here: see  @ref SNP_PREDEF_SERVICE
  uint8_t paramID;      //!< 1 Byte ID of the parameter of the service to update/retrieved
  uint8_t *pData;       //!< Pointer of Data to write/retrieved.
} snpSetGattParamReq_t, snpGetGattParamRsp_t;

/**
 * @struct snpSetAttrValueReq_t
 *
 * @brief Parameter Structure for @ref SNP_setAttrValue / @ref SNP_SET_ATTR_VALUE_REQ
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint16_t attrHandle;  //!< 2 Byte Attribute handle of the characteristic value.
  uint8_t *pData;       //!< Pointer of data value to set.
} snpSetAttrValueReq_t;

/**
 * @struct snpSetAttrValueRsp_t
 *
 * @brief Parameter Structure for response to @ref SNP_setAttrValue / @ref SNP_SET_ATTR_VALUE_RSP
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t  status;       //!< 1 Byte Status of the request (SUCCESS or Error, @ref SNP_ERRORS)
  uint16_t attrHandle;   //!< 2 Byte Attribute handle of the characteristic value.
} snpSetAttrValueRsp_t;

/**
 * @struct snpGetAttrValueReq_t
 *
 * @brief Parameter Structure for @ref SNP_getAttrValue / @ref SNP_SET_ATTR_VALUE_REQ
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint16_t attrHandle;   //!< 2 Byte Attribute handle of the characteristic value.
} snpGetAttrValueReq_t;

/**
 * @struct snpGetAttrValueRsp_t
 *
 * @brief Parameter Structure for response to @ref SNP_getAttrValue / @ref SNP_SET_ATTR_VALUE_RSP
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t  status;       //!< 1 Byte Status of the request (SUCCESS or Error, @ref SNP_ERRORS)
  uint16_t attrHandle;   //!< 2 Byte Attribute handle of the characteristic value.
  uint8_t  *pData;       //!< Pointer to data of the value to set.
} snpGetAttrValueRsp_t;

/**
 * @struct snpGetGattParamReq_t
 *
 * @brief Parameter Structure for @ref SNP_getGATTParam / @ref SNP_GET_GATT_PARAM_REQ
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t serviceID;    //!< 1 Byte ID of the service containing the parameter to define
  uint8_t paramID;      //!< 1 Byte ID of the parameter to retrieved.
} snpGetGattParamReq_t;

/**
 * @struct snpCharCfgUpdatedInd_t
 *
 * @brief Parameter Structure for the Characteristic Configuration Update Indication.
 *
 * The @ref SNP_CCCD_UPDATED_IND event generates this.
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint16_t  connHandle;   //!< 2 Byte Handle of the connection
  uint16_t  cccdHandle;   //!< 2 Byte Handle of the characteristic value attribute to notify/indicate
  uint8_t   rspNeeded;    //!< 1 Byte Indicate if the AP must answer this indication or not, @ref SNP_GATT_RSP_OPTIONS
  uint16_t  value;        //!< 2 Byte Value to write to the CCCD
} snpCharCfgUpdatedInd_t;

/**
 * @struct snpSetWhiteListReq_t
 *
 * @brief Parameter Structure for @ref SNP_setWhiteListFilterPolicy / @ref SNP_SET_WHITE_LIST_POLICY_REQ
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t useWhiteList; //!< 1 Byte White List Filter Policy, @ref SNP_AdvFilterPolicyType
} snpSetWhiteListReq_t;

/**
 * @union snp_msg_t
 * @brief The SNP message.  A union of all SNP message types
 */
typedef union
{
  //Mask events
  snpMaskEventReq_t        maskEventReq;        //!< event mask request.
  snpMaskEventRsp_t        maskEventCnf;        //!< event mask confirmation.

  //Revision
  snpGetRevisionRsp_t      revisionRsp;         //!< Get revision Response.

  //Rand
  snpGetRandRsp_t          randRsp;             //!< Get Random Number Response

  //HCI
  snpHciCmdReq_t           hciCmdReq;           //!< HCI command request
  snpHciCmdRsp_t           hciCmdRsp;           //!< HCI command response (event)

  //Status
  snpGetStatusCmdRsp_t     getStatusRsp;        //!< Get status Response

  //Test
  snpTestCmdRsp_t          testCmdRsp;          //!< Test command response

  //Advertising
  snpStartAdvReq_t         startAdv;            //!< Start advertising request.
  snpSetAdvDataReq_t       setAdvDataReq;       //!< Set advertising data Request.
  snpSetAdvDataCnf_t       setAdvDataCnf;       //!< Set advertising data Response.

  //Connection
  snpTermConnReq_t         connTermReq;         //!< Connection terminated Request.
  snpUpdateConnParamReq_t  updateConnParamsReq; //!< Update connection parameters indication.
  snpUpdateConnParamCnf_t  updateConnParamsCnf; //!< Update connection parameters, request.

  //GAP Parameters
  snpSetGapParamReq_t      setGapParamReq;      //!< Set GAP parameter Request.
  snpSetGapParamRsp_t      setGapParamRsp;      //!< Set GAP parameter Response.
  snpGetGapParamReq_t      getGapParamReq;      //!< Get GAP parameter Request.
  snpGetGapParamRsp_t      getGapParamRsp;      //!< Get GAP parameter Response.

  //Security
  snpSetSecParamReq_t      setSecParamReq;      //!< Set the Security Requirements request.
  snpSetSecParamRsp_t      setSecParamRsp;      //!< Set Security Requirements response.
  snpSetAuthDataReq_t      setAuthDataReq;      //!< Set Authentication Data request.
  snpSetAuthDataRsp_t      setAuthDataRsp;      //!< Set Authentication Data response.

  //White List
  snpSetWhiteListReq_t     setWhiteListReq;    //!< Set the White List Filter Policy request.
  snpSetWhiteListRsp_t     setWhiteListRsp;    //!< Set the White List Filter Policy response.

  //Services
  snpAddServiceReq_t       addServiceReq;       //!< Add service request.
  snpAddServiceRsp_t       addServiceRsp;       //!< Add service response.
  snpAddCharValueDeclReq_t addCharValueDecReq;  //!< Add characteristic value Request.
  snpAddCharValueDeclRsp_t addCharValueDecRsp;  //!< Add characteristic value Response.
  snpAddCharDescDeclReq_t  addCharDescDeclReq;  //!< Add characteristic descriptor Request.
  snpAddCharDescDeclRsp_t  addCharDescDeclRsp;  //!< Add characteristic descriptor Response.
  snpRegisterServiceRsp_t  registerService;     //!< Register service Response.

  //Incoming GATT Requests
  snpCharReadInd_t         readInd;             //!< Characteristic read request.
  snpCharReadCnf_t         readCnf;             //!< Characteristic read confirmation.
  snpCharWriteInd_t        charWriteInd;        //!< Characteristic write request.
  snpCharWriteCnf_t        charWriteCnf;        //!< Characteristic write confirmation.
  snpNotifIndReq_t         notifIndReq;         //!< Notification or indication request
  snpNotifIndCnf_t         notifIndCnf;         //!< Indication Confirmation.

  //NP GATT Parameters of predefined services
  snpSetGattParamReq_t     setGattParamReq;     //!< Set GATT parameter request.
  snpSetGattParamRsp_t     setGattParamRsp;     //!< Set GATT parameter response.
  snpGetGattParamReq_t     getGattParamReq;     //!< Get GATT parameter response.
  snpGetGattParamRsp_t     getGattParamRsp;     //!< Get GATT parameter request.

  //CCCD Updates
  snpCharCfgUpdatedInd_t   charCfgUpdatedReq;   //!< Characteristic configuration updated indication.
  snpCharCfgUpdatedRsp_t   charcfgUpdatedRsp;   //!< Characteristic configuration update response.
} snp_msg_t;

/*** SNP EVENTS ***/

/**
 * @struct snpConnEstEvt_t
 *
 * @brief Parameter Structure for the Connection Established Event
 *
 * See @ref SNP_Event_ind for more information.
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint16_t connHandle;          //!< 2 Byte Handle of the connection
  uint16_t connInterval;        //!< 2 Byte @see snpUpdateConnParamReq_t::connInterval
  uint16_t slaveLatency;        //!< 2 Byte @see snpUpdateConnParamReq_t::slaveLatency
  uint16_t supervisionTimeout;  //!< 2 Byte @see snpUpdateConnParamReq_t::supervisionTimeout
  uint8_t  addressType;         //!< 1 Byte Type of initiator address
  uint8_t  pAddr[6];            //!< 6 Byte Address of the initiator
} snpConnEstEvt_t;

/**
 * @struct snpConnTermEvt_t
 *
 * @brief Parameter Structure for the Connection terminated Event
 *
 * See @ref SNP_Event_ind for more information.
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint16_t connHandle;          //!< 2 Byte Handle of the connection
  uint8_t  reason;              //!< 1 Byte Reason of the termination @ref SNP_connTerm
} snpConnTermEvt_t;

/**
 * @struct snpUpdateConnParamEvt_t
 *
 * @brief Parameter Structure for the Update Connection Parameter Event
 *
 * See @ref SNP_Event_ind for more information.
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint16_t connHandle;          //!< 2 Byte Handle of the connection
  uint16_t connInterval;        //!< 2 Byte @see snpUpdateConnParamReq_t::connInterval
  uint16_t slaveLatency;        //!< 2 Byte @see snpUpdateConnParamReq_t::slaveLatency
  uint16_t supervisionTimeout;  //!< 2 Byte @see snpUpdateConnParamReq_t::supervisionTimeout
} snpUpdateConnParamEvt_t;

/**
 * @struct snpAdvStatusEvt_t
 *
 * @brief Parameter Structure for the Advertising state change Event
 *
 * See @ref SNP_Event_ind for more information.
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t status;           //!< 1 Byte Status @ref SNP_ERRORS
} snpAdvStatusEvt_t;

/**
 * @struct snpSecurityEvt_t
 *
 * @brief Parameter Structure for the Security Event
 *
 * See @ref SNP_Event_ind for more information.
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t state;  //!< 1 Byte New Security State @ref SNP_SECURITY_STATE
  uint8_t status; //!< 1 Byte Status upon entering this state @ref SNP_ERRORS
} snpSecurityEvt_t;

/**
 * @struct snpErrorEvt_t
 *
 * @brief Parameter Structure for the Error Event @ref SNP_ERROR_EVT
 *
 * See @ref SNP_Event_ind for more information.
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint16_t opcode;       //!< 2 Byte SNP opcode that triggered the error
  uint8_t  status;       //!< 1 Byte Status of the request @ref SNP_ERRORS
} snpErrorEvt_t;

/**
 * @struct snpATTMTUSizeEvt_t
 *
 * @brief Parameter Structure for the ATT MTU size updated Event data
 *
 * See @ref SNP_Event_ind for more information.
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint16_t connHandle;  //!< 2 Byte Handle of the connection
  uint16_t attMtuSize;  //!< 2 Byte New ATT MTU Size negotiated between GATT client and GATT server.
} snpATTMTUSizeEvt_t;

/**
 * @struct snpAuthenticationEvt_t
 *
 * @brief Parameter Structure for MITM Authentication data to be displayed
 *
 * See @ref SNP_Event_ind for more information.
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint8_t display; //!< 1 Byte TRUE if device must display passcode.
  uint8_t input;   //!< 1 Byte TRUE if device must input passcode.
  uint32_t numCmp; //!< 32 bit Numeric Comparison value to display, 0 if not used.
} snpAuthenticationEvt_t;

/**
 * @union snpEventParam_t
 * @brief SNP Event Parameters
 */
typedef union
{
  snpConnEstEvt_t         connEstEvt;         //!< Parameter Structure @ref snpConnEstEvt_t
  snpConnTermEvt_t        connTermEvt;        //!< Parameter Structure @ref snpConnTermEvt_t
  snpUpdateConnParamEvt_t updateConnParamEvt; //!< Parameter Structure @ref snpUpdateConnParamEvt_t
  snpAdvStatusEvt_t       advStatusEvt;       //!< Parameter Structure @ref snpAdvStatusEvt_t
  snpATTMTUSizeEvt_t      attMTUSizeEvt;      //!< Parameter Structure @ref snpATTMTUSizeEvt_t
  snpErrorEvt_t           advErrorEvt;        //!< Parameter Structure @ref snpErrorEvt_t
  snpSecurityEvt_t        securityEvt;        //!< Parameter Structure @ref snpSecurityEvt_t
  snpAuthenticationEvt_t  authenticationEvt;  //!< Parameter Structure @ref snpAuthenticationEvt_t
} snpEventParam_t;

/**
 * @struct snpEvt_t
 *
 * @brief SNP Event Structure
 *
 * This is a packed structure. see @ref TL_Parameter for more information.
 */
PACKED_TYPEDEF_STRUCT
{
  uint16_t        event;        //!< 2 Byte Event, @ref SNP_EVENT
  snpEventParam_t *pEvtParams;  //!< @ref snpEventParam_t
} snpEvt_t;

/**
 * @struct snpGapPeriConnectParams_t
 *
 * @brief Connection parameters for the peripheral device.
 *
 * These numbers are used to compare against connection events and
 * request connection parameter updates with the master.
 * this structure is used to update the parameter #SNP_GGS_PERI_CONN_PARAM_ATT of the GAP/GATT service.
 */
typedef struct
{
  uint16_t intervalMin; //!< 2 Byte @see snpUpdateConnParamReq_t::connInterval
  uint16_t intervalMax; //!< 2 Byte @see snpUpdateConnParamReq_t::connInterval
  uint16_t latency;     //!< 2 Byte @see snpUpdateConnParamReq_t::slaveLatency
  uint16_t timeout;     //!< 2 Byte @see snpUpdateConnParamReq_t::supervisionTimeout
} snpGapPeriConnectParams_t;

/** @} End SNP_Structs */

/*********************************************************************
 * MACROS
 */

/** @defgroup SNP_MACROS SNP Macros
 * @{
 */

//! \brief CMD1 Header Mask Constant
#define SNP_CMD1_HDR_MASK               0xC0

//! \brief CMD1 Header Offset Constant
#define SNP_CMD1_HDR_POS                0x6

//! \brief Macro to retrieve the HDR bitfield from CMD1
#define SNP_GET_OPCODE_HDR_CMD1(cmd)    (((cmd) & SNP_CMD1_HDR_MASK) >>  SNP_CMD1_HDR_POS)

//! \brief CMD1 ID Mask Constant
#define SNP_CMD1_ID_MASK                0x3F

//! \brief Macro to retrieve the ID bitfield from CMD1
#define SNP_GET_OPCODE_ID_CMD1(cmd)     ((cmd) & SNP_CMD1_ID_MASK)

/** @} End SNP_MACROS */

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SNP_H */

/** @} End SNP */
