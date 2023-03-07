/******************************************************************************

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated

 All rights reserved not granted herein.
 Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free,
 non-exclusive license under copyrights and patents it now or hereafter
 owns or controls to make, have made, use, import, offer to sell and sell
 ("Utilize") this software subject to the terms herein. With respect to the
 foregoing patent license, such license is granted solely to the extent that
 any such patent is necessary to Utilize the software alone. The patent
 license shall not apply to any combinations which include this software,
 other than combinations with devices manufactured by or for TI ("TI
 Devices"). No hardware patent is licensed hereunder.

 Redistributions must preserve existing copyright notices and reproduce
 this license (including the above copyright notice and the disclaimer and
 (if applicable) source code license limitations below) in the documentation
 and/or other materials provided with the distribution.

 Redistribution and use in binary form, without modification, are permitted
 provided that the following conditions are met:

   * No reverse engineering, decompilation, or disassembly of this software
     is permitted with respect to any software provided in binary form.
   * Any redistribution and use are licensed by TI for use only with TI Devices.
   * Nothing shall obligate TI to provide you with source code for the software
     licensed and provided to you in object code.

 If software source code is provided to you, modification and redistribution
 of the source code are permitted provided that the following conditions are
 met:

   * Any redistribution and use of the source code, including any resulting
     derivative works, are licensed by TI for use only with TI Devices.
   * Any redistribution and use of any object code compiled from the source
     code and any resulting derivative works, are licensed by TI for use
     only with TI Devices.

 Neither the name of Texas Instruments Incorporated nor the names of its
 suppliers may be used to endorse or promote products derived from this
 software without specific prior written permission.

 DISCLAIMER.

 THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/**
 *  @defgroup HCI HCI
 *  @brief This module implements the Host Controller Interface.
 *  For a detailed usage section describing how to send these commands and receive events,
 *  see the HCI Section of the User's Guide.
 *  @{
 */

/** @defgroup HCI_Function_Maps HCI Function Maps
 *  HCI Function Mapping
 *  @{
 *
 * This section lists the mapping between stack APIs and function names, as described
 * in the BT core spec for commands that the application can call and receive a \
 * corresponding event.
 * no.
 *
 * <table>
 * <caption id="API_Function_Map">API Function Map</caption>
 * <tr>
 *   <th>Stack API</th>
 *   <th>BT Function</th>
 * </tr>
 * <tr>
 *   <td>@ref HCI_HostNumCompletedPktCmd</td>
 *   <td>Host Number Of Completed Packets Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_AddDeviceToResolvingListCmd</td>
 *   <td>LE Add Device to Resolving List Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_AddWhiteListCmd</td>
 *   <td>LE Add Device To White List Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_ClearWhiteListCmd</td>
 *   <td>LE Clear White List Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_EncryptCmd</td>
 *   <td>LE Encrypt Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_RandCmd</td>
 *   <td>LE Rand Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_ReadBufSizeCmd</td>
 *   <td>LE Read Buffer Size Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_ReadChannelMapCmd</td>
 *   <td>LE Read Channel Map Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_ReadLocalResolvableAddressCmd</td>
 *   <td>LE Read Local Resolvable Address Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_ReadLocalSupportedFeaturesCmd</td>
 *   <td>Read Local Supported Commands Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_ReadMaxDataLenCmd</td>
 *   <td>LE Read Maximum Data Length Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_ReadPeerResolvableAddressCmd</td>
 *   <td>LE Read Peer Resolvable Address Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_ReadResolvingListSizeCmd</td>
 *   <td>LE Read Resolving List Size Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_ReadSuggestedDefaultDataLenCmd</td>
 *   <td>LE Read Suggested Default Data Length Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_ReadSupportedStatesCmd</td>
 *   <td>LE Read Supported States Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_ReadWhiteListSizeCmd</td>
 *   <td>LE Read White List Size Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_RemoveDeviceFromResolvingListCmd</td>
 *   <td>LE Remove Device From Resolving List Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_RemoveWhiteListCmd</td>
 *   <td>LE Remove Device From White List Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_SetDataLenCmd</td>
 *   <td>LE Set Data Length Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_SetEventMaskCmd</td>
 *   <td>LE Set Event Mask Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_SetHostChanClassificationCmd</td>
 *   <td>LE Set Host Channel Classification Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_SetResolvablePrivateAddressTimeoutCmd</td>
 *   <td>LE Set Resolvable Private Address Timeout Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_SetPrivacyModeCmd</td>
 *   <td>LE Set Privacy Mode Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_TestEndCmd</td>
 *   <td>LE Test End Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_TransmitterTestCmd</td>
 *   <td>LE Transmitter Test Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_WriteSuggestedDefaultDataLenCmd</td>
 *   <td>LE Write Suggested Default Data Length Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_ReadAuthPayloadTimeoutCmd</td>
 *   <td>Read Authenticated Payload Timeout Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_ReadBDADDRCmd</td>
 *   <td>Read BD_ADDR Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_ReadLocalSupportedCommandsCmd</td>
 *   <td>Read Local Supported Commands Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_ReadLocalSupportedFeaturesCmd</td>
 *   <td>Read Local Supported Features Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_ReadLocalVersionInfoCmd</td>
 *   <td>Read Local Version Information Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_ReadRssiCmd</td>
 *   <td>Read RSSI Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_ReadTransmitPowerLevelCmd</td>
 *   <td>Read Transmit Power Level Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_ResetCmd</td>
 *   <td>Reset Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_SetEventMaskCmd</td>
 *   <td>Set Event Mask Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_SetEventMaskPage2Cmd</td>
 *   <td>Set Event Mask Page 2 Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_ReadTxPowerCmd</td>
 *   <td>LE Read Tx Power Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_ReadRfPathCompCmd</td>
 *   <td>LE Read Rf Path Compensation Command</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_LE_WriteRfPathCompCmd</td>
 *   <td>LE Write Rf Path Compensation Command</td>
 * </tr>
 * </table>
 *
 * This section lists the mapping between stack APIs and function names, as described
 * in the Vendor Specific API guide included with the installer.
 *
 * <table>
 * <caption id="Vendor_Specific_API_Function_Map">Vendor Specific API Function Map</caption>
 * <tr>
 *   <th>Stack API</th>
 *   <th>Vendor Specific Function</th>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_BuildRevisionCmd</td>
 *   <td>HCI Extension Build Revision</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_DecryptCmd</td>
 *   <td>HCI Extension Decrypt</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_DisconnectImmedCmd</td>
 *   <td>HCI Extension Disconnect Immediate</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_EnhancedModemHopTestTxCmd</td>
 *   <td>HCI Extension Enhanced Modem Hop Test Transmit</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_EnhancedModemTestRxCmd</td>
 *   <td>HCI Extension Enhanced Modem Test Receive</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_EnhancedModemTestTxCmd</td>
 *   <td>HCI Extension Enhanced Modem Test Transmit</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_EnablePTMCmd</td>
 *   <td>HCI Extension Enable PTM</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_EndModemTestCmd</td>
 *   <td>HCI Extension End Modem Test</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_ModemHopTestTxCmd</td>
 *   <td>HCI Extension Modem Hop Test Transmit</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_ModemTestRxCmd</td>
 *   <td>HCI Extension Modem Test Receive</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_ModemTestTxCmd</td>
 *   <td>HCI Extension Modem Test Transmit</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_NumComplPktsLimitCmd</td>
 *   <td>HCI Extension Number Completed Packets Limit</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_OnePktPerEvtCmd</td>
 *   <td>HCI Extension One Packet Per Event</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_PacketErrorRateCmd</td>
 *   <td>HCI Extension Packet Error Rate</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_PERbyChanCmd</td>
 *   <td>HCI Extension Packet Error Rate By Channel</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_SaveFreqTuneCmd</td>
 *   <td>HCI Extension Save Frequency Tuning</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_SetFastTxResponseTimeCmd</td>
 *   <td>HCI Extension Set Fast Transmit Response Time</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_SetFreqTuneCmd</td>
 *   <td>HCI Extension Set Frequency Tuning</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_SetLocalSupportedFeaturesCmd</td>
 *   <td>HCI Extension Set Local Supported Features</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_SetMaxDataLenCmd</td>
 *   <td>HCI Extension Set Max Data Length</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_SetMaxDtmTxPowerCmd</td>
 *   <td>HCI Extension Set Max DTM Transmitter Power</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_SetRxGainCmd</td>
 *   <td>HCI Extension Set Receiver Gain</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_SetSCACmd</td>
 *   <td>HCI Extension Set SCA</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_SetTxPowerCmd</td>
 *   <td>HCI Extension Set Transmitter Power</td>
 * </tr>
 * <tr>
 *   <td>@ref HCI_EXT_SetDtmTxPktCntCmd</td>
 *   <td>HCI Extension Set DTM Transmitter Packet Count</td>
 * </tr>
 * </table>
 *
 *  @{
 */

/** @} End HCI_Function_Maps */


/**
 *
 *  @file  hci.h
 *  @brief      HCI layer interface
 */

#ifndef HCI_H
#define HCI_H

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * INCLUDES
 */
#include "bcomdef.h"
#include "osal.h"
#include "ll_common.h"
#include "ll_ae.h"
#include "hal_assert.h"

/*
 * MACROS
 */

/*
 * CONSTANTS
 */

/**
 * @defgroup HCI_Constants HCI Constants
 * @{
 */

/**
 * @defgroup HCI_Error_Codes HCI Error Codes
 * @{
 * Per the Bluetooth Core Specification, V4.0.0, Vol. 2, Part D.
 */
#define HCI_SUCCESS                                                         0x00 //!< Success
#define HCI_ERROR_CODE_UNKNOWN_HCI_CMD                                      0x01 //!< Unknown HCI Command
#define HCI_ERROR_CODE_UNKNOWN_CONN_ID                                      0x02 //!< Unknown Connection ID
#define HCI_ERROR_CODE_HW_FAILURE                                           0x03 //!< Hardware Failure
#define HCI_ERROR_CODE_PAGE_TIMEOUT                                         0x04 //!< Page Timeout
#define HCI_ERROR_CODE_AUTH_FAILURE                                         0x05 //!< Authentication Fialure
#define HCI_ERROR_CODE_PIN_KEY_MISSING                                      0x06 //!< Pin Key Missing
#define HCI_ERROR_CODE_MEM_CAP_EXCEEDED                                     0x07 //!< Memory Capability Exceeded
#define HCI_ERROR_CODE_CONN_TIMEOUT                                         0x08 //!< Connection Timeout
#define HCI_ERROR_CODE_CONN_LIMIT_EXCEEDED                                  0x09 //!< Connection Limit Exceeded
#define HCI_ERROR_CODE_SYNCH_CONN_LIMIT_EXCEEDED                            0x0A //!< Synchonous Connection Limit Exceeded
#define HCI_ERROR_CODE_ACL_CONN_ALREADY_EXISTS                              0x0B //!< ACL Connection Already Exists
#define HCI_ERROR_CODE_CMD_DISALLOWED                                       0x0C //!< Command Disallowed
#define HCI_ERROR_CODE_CONN_REJ_LIMITED_RESOURCES                           0x0D //!< Connection Rejected due to Limited Resources
#define HCI_ERROR_CODE_CONN_REJECTED_SECURITY_REASONS                       0x0E //!< Connection Rejected for Security Reasons
#define HCI_ERROR_CODE_CONN_REJECTED_UNACCEPTABLE_BDADDR                    0x0F //!< Connection Rejected due to Unacceptable BDADDR
#define HCI_ERROR_CODE_CONN_ACCEPT_TIMEOUT_EXCEEDED                         0x10 //!< Connection Acceptance Timeout Exceeded
#define HCI_ERROR_CODE_UNSUPPORTED_FEATURE_PARAM_VALUE                      0x11 //!< Unsupported Paramter
#define HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS                               0x12 //!< Invalid HCI Command Parameters
#define HCI_ERROR_CODE_REMOTE_USER_TERM_CONN                                0x13 //!< Remote User Terminated Connection
#define HCI_ERROR_CODE_REMOTE_DEVICE_TERM_CONN_LOW_RESOURCES                0x14 //!< Remote Device Terminated Connection due to Low Resources
#define HCI_ERROR_CODE_REMOTE_DEVICE_TERM_CONN_POWER_OFF                    0x15 //!< Remote Device Terminated Connection due to Power Off
#define HCI_ERROR_CODE_CONN_TERM_BY_LOCAL_HOST                              0x16 //!< Connection Terminated by Local Host
#define HCI_ERROR_CODE_REPEATED_ATTEMPTS                                    0x17 //!< Repeated Attempts
#define HCI_ERROR_CODE_PAIRING_NOT_ALLOWED                                  0x18 //!< Pairing Not Allowed
#define HCI_ERROR_CODE_UNKNOWN_LMP_PDU                                      0x19 //!< Unknown LMP PDU
#define HCI_ERROR_CODE_UNSUPPORTED_REMOTE_FEATURE                           0x1A //!< Unsupported Remote Feature
#define HCI_ERROR_CODE_SCO_OFFSET_REJ                                       0x1B //!< SCO Offset Rejected
#define HCI_ERROR_CODE_SCO_INTERVAL_REJ                                     0x1C //!< SCO Interval Rejected
#define HCI_ERROR_CODE_SCO_AIR_MODE_REJ                                     0x1D //!< SCO Air Mode Rejected
#define HCI_ERROR_CODE_INVALID_LMP_PARAMS                                   0x1E //!< Invalid LMP Paramaters
#define HCI_ERROR_CODE_UNSPECIFIED_ERROR                                    0x1F //!< Unspecified Error
#define HCI_ERROR_CODE_UNSUPPORTED_LMP_PARAM_VAL                            0x20 //!< Unsupported LMP Parameter Value
#define HCI_ERROR_CODE_ROLE_CHANGE_NOT_ALLOWED                              0x21 //!< Role Change Not Allowed
#define HCI_ERROR_CODE_LMP_LL_RESP_TIMEOUT                                  0x22 //!< LMP LL Response Timeout
#define HCI_ERROR_CODE_LMP_ERR_TRANSACTION_COLLISION                        0x23 //!< LMP Error Transaction Collision
#define HCI_ERROR_CODE_LMP_PDU_NOT_ALLOWED                                  0x24 //!< LMP DU Not Allowed
#define HCI_ERROR_CODE_ENCRYPT_MODE_NOT_ACCEPTABLE                          0x25 //!< Encrypt Mode not Acceptable
#define HCI_ERROR_CODE_LINK_KEY_CAN_NOT_BE_CHANGED                          0x26 //!< Link Key Can Not Be Changed
#define HCI_ERROR_CODE_REQ_QOS_NOT_SUPPORTED                                0x27 //!< Request QOS Not Supported
#define HCI_ERROR_CODE_INSTANT_PASSED                                       0x28 //!< Instant Passed
#define HCI_ERROR_CODE_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED                  0x29 //!< Pairing with Unit Key Not Supported
#define HCI_ERROR_CODE_DIFFERENT_TRANSACTION_COLLISION                      0x2A //!< Different Transaction Collision
#define HCI_ERROR_CODE_RESERVED1                                            0x2B //!< Reserved
#define HCI_ERROR_CODE_QOS_UNACCEPTABLE_PARAM                               0x2C //!< QOS Unacceptable Parameter
#define HCI_ERROR_CODE_QOS_REJ                                              0x2D //!< QOS Reject
#define HCI_ERROR_CODE_CHAN_ASSESSMENT_NOT_SUPPORTED                        0x2E //!< Channel Assessment Not Supported
#define HCI_ERROR_CODE_INSUFFICIENT_SECURITY                                0x2F //!< Insufficient Security
#define HCI_ERROR_CODE_PARAM_OUT_OF_MANDATORY_RANGE                         0x30 //!< Parameter out of Mandatory Range
#define HCI_ERROR_CODE_RESERVED2                                            0x31 //!< Reserved
#define HCI_ERROR_CODE_ROLE_SWITCH_PENDING                                  0x32 //!< Role Switch Pending
#define HCI_ERROR_CODE_RESERVED3                                            0x33 //!< Reserved
#define HCI_ERROR_CODE_RESERVED_SLOT_VIOLATION                              0x34 //!< Reserved Slot Violation
#define HCI_ERROR_CODE_ROLE_SWITCH_FAILED                                   0x35 //!< Role Switch Failed
#define HCI_ERROR_CODE_EXTENDED_INQUIRY_RESP_TOO_LARGE                      0x36 //!< Extended Inquiry Response too Large
#define HCI_ERROR_CODE_SIMPLE_PAIRING_NOT_SUPPORTED_BY_HOST                 0x37 //!< Simple Pairing Not Supported by Host
#define HCI_ERROR_CODE_HOST_BUSY_PAIRING                                    0x38 //!< Host Busy Pairing
#define HCI_ERROR_CODE_CONN_REJ_NO_SUITABLE_CHAN_FOUND                      0x39 //!< Connection Rejected No Suitable Channel Found
#define HCI_ERROR_CODE_CONTROLLER_BUSY                                      0x3A //!< Controller Busy
#define HCI_ERROR_CODE_UNACCEPTABLE_CONN_PARAMETERS                         0x3B //!< Unacceptable Connection Parameters
#define HCI_ERROR_CODE_DIRECTED_ADV_TIMEOUT                                 0x3C //!< Directed Advertising Timeout
#define HCI_ERROR_CODE_CONN_TERM_MIC_FAILURE                                0x3D //!< Connection Terminated due to MIC Failure
#define HCI_ERROR_CODE_CONN_FAILED_TO_ESTABLISH                             0x3E //!< Connection Failed to Establish
#define HCI_ERROR_CODE_MAC_CONN_FAILED                                      0x3F //!< MAC Connection Failed
#define HCI_ERROR_CODE_COARSE_CLOCK_ADJUST_REJECTED                         0x40 //!< Coarse Clock Adjust Rejected
#define HCI_ERROR_CODE_TYPE0_SUBMAP_NOT_DEFINED                             0x41 //!< Type0 Submap Not Defined
#define HCI_ERROR_CODE_UNKNOWN_ADVERTISING_IDENTIFIER                       0x42 //!< Unknown Advertising Identifier
#define HCI_ERROR_CODE_LIMIT_REACHED                                        0x43 //!< Limit Reached
#define HCI_ERROR_CODE_OPERATION_CANCELLED_BY_HOST                          0x44 //!< Operation Canceled by Host
/** @} End HCI_Error_Codes */

/**
 * @defgroup HCI_Cmd_API_Params HCI Command API Parameters
 * @{
 */
#define HCI_MAX_NUM_CMD_BUFFERS                        LL_MAX_NUM_CMD_BUFFERS   //!< Max Num Command Buffer
#if defined(CC2540) || defined(CC2541) || defined(CC2541S)
#define HCI_MAX_NUM_DATA_BUFFERS                       LL_MAX_NUM_DATA_BUFFERS  //<! Max Num Data Buffers
#endif // CC2540 | CC2541 | CC2541S

/** @} End HCI_Cmd_API_Params */


// Send Data Packet Boundary Flags
#define FIRST_PKT_HOST_TO_CTRL                         LL_DATA_FIRST_PKT_HOST_TO_CTRL       //!< First Host to Controller Packet
#define CONTINUING_PKT                                 LL_DATA_CONTINUATION_PKT             //!< Continuing Packet
#define FIRST_PKT_CTRL_TO_HOST                         LL_DATA_FIRST_PKT_CTRL_TO_HOST       //!< First Packet Controller to Host


/// @brief Invalid RSSI
#define HCI_RSSI_NOT_AVAILABLE                         LL_RSSI_NOT_AVAILABLE

/**
 * @defgroup Disconnect_Reasons Disconnect Reasons
 * @{
 */
#define HCI_DISCONNECT_AUTH_FAILURE                    HCI_ERROR_CODE_AUTH_FAILURE                              //!< Authentication Failure
#define HCI_DISCONNECT_REMOTE_USER_TERM                HCI_ERROR_CODE_REMOTE_USER_TERM_CONN                     //!< Remote User Terminated
#define HCI_DISCONNECT_REMOTE_DEV_LOW_RESOURCES        HCI_ERROR_CODE_REMOTE_DEVICE_TERM_CONN_LOW_RESOURCES     //!< Remote Device Low Resources
#define HCI_DISCONNECT_REMOTE_DEV_POWER_OFF            HCI_ERROR_CODE_REMOTE_DEVICE_TERM_CONN_POWER_OFF         //!< Remote Device Power Off
#define HCI_DISCONNECT_UNSUPPORTED_REMOTE_FEATURE      HCI_ERROR_CODE_UNSUPPORTED_REMOTE_FEATURE                //!< Unsupported Remote Feature
#define HCI_DISCONNECT_KEY_PAIRING_NOT_SUPPORTED       HCI_ERROR_CODE_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED       //!< Key Paring Not Supported
#define HCI_DISCONNECT_UNACCEPTABLE_CONN_INTERVAL      HCI_ERROR_CODE_UNACCEPTABLE_CONN_INTERVAL                //!< Unnaceptable Connection Interval
/** @} End Disconnect_Reasons */

/**
 * @defgroup TX_Power_Mode Read TX Power Mode
 * @{
 */
// Tx Power Types
#define HCI_READ_CURRENT_TX_POWER_LEVEL                LL_READ_CURRENT_TX_POWER_LEVEL           //!< Read Current TX Power Level
#define HCI_READ_MAX_TX_POWER_LEVEL                    LL_READ_MAX_TX_POWER_LEVEL               //!< Read Max TX Power Level
/** @} End TX_Power_Mode */

/**
 * @defgroup Flow_Control Host Flow Control Flow Control Options
 * @{
 */
#define HCI_CTRL_TO_HOST_FLOW_CTRL_OFF                 0  //!< Controller to Host Flow Controll Off
#define HCI_CTRL_TO_HOST_FLOW_CTRL_ACL_ON_SYNCH_OFF    1  //!< ACL On, Synchronous Off
#define HCI_CTRL_TO_HOST_FLOW_CTRL_ACL_OFF_SYNCH_ON    2  //!< ACL Off, Synchronous On
#define HCI_CTRL_TO_HOST_FLOW_CTRL_ACL_ON_SYNCH_ON     3  //!< ACL On, Synchronous On
/** @} End Flow_control */

/**
 * @defgroup Addr_type Device Address Type
 * @{
 */
#define HCI_PUBLIC_DEVICE_ADDRESS                      LL_DEV_ADDR_TYPE_PUBLIC      //!< Public Device Address
#define HCI_RANDOM_DEVICE_ADDRESS                      LL_DEV_ADDR_TYPE_RANDOM      //!< Random Device Address
/** @} End Addr_type */

/**
 * @defgroup Adv_type Advertiser Type
 * @{
 */
#define HCI_CONNECTABLE_UNDIRECTED_ADV                 LL_ADV_CONNECTABLE_UNDIRECTED_EVT        //!< Connectable Undirected Advertising
#define HCI_CONNECTABLE_DIRECTED_HDC_ADV               LL_ADV_CONNECTABLE_DIRECTED_HDC_EVT      //!< Connectable Directed High Duty Cycle Advertising
#define HCI_SCANNABLE_UNDIRECTED                       LL_ADV_SCANNABLE_UNDIRECTED_EVT          //!< Scannable Undirected Advertising
#define HCI_NONCONNECTABLE_UNDIRECTED_ADV              LL_ADV_NONCONNECTABLE_UNDIRECTED_EVT     //!< Nonconnectable Undirected Advertising
#define HCI_CONNECTABLE_DIRECTED_LDC_ADV               LL_ADV_CONNECTABLE_DIRECTED_LDC_EVT      //!< Connectable Directed Low Duty Cycle Advertising
/** @} End Adv_type */

/**
 * @defgroup Adv_channels Advertiser Channels
 * @{
 */
#define HCI_ADV_CHAN_37                                LL_ADV_CHAN_37       //!< Channel 37
#define HCI_ADV_CHAN_38                                LL_ADV_CHAN_38       //!< Channel 38
#define HCI_ADV_CHAN_39                                LL_ADV_CHAN_39       //!< Channel 39
#define HCI_ADV_CHAN_ALL                               (LL_ADV_CHAN_37 | LL_ADV_CHAN_38 | LL_ADV_CHAN_39)   //!< All Channels
/** @} End Adv_channels */

/**
 * @defgroup Adv_whitelist Advertiser White List Policy
 * @{
 */
#define HCI_ADV_WL_POLICY_ANY_REQ                      LL_ADV_WL_POLICY_ANY_REQ             //!< Any Request
#define HCI_ADV_WL_POLICY_WL_SCAN_REQ                  LL_ADV_WL_POLICY_WL_SCAN_REQ         //!< Scan Requests
#define HCI_ADV_WL_POLICY_WL_CONNECT_IND               LL_ADV_WL_POLICY_WL_CONNECT_IND      //!< Connection Inidication
#define HCI_ADV_WL_POLICY_WL_ALL_REQ                   LL_ADV_WL_POLICY_WL_ALL_REQ          //!< All Requests
/** @} End Adv_whitelist */

/**
 * @defgroup Adv_commands Advertiser Commands
 * @{
 */
#define HCI_ENABLE_ADV                                 LL_ADV_MODE_ON       //!< Advertising Enable
#define HCI_DISABLE_ADV                                LL_ADV_MODE_OFF      //!< Advertising Disable
/** @} End Adv_commands */

/**
 * @defgroup Scan_types Scan Types
 * @{
 */
#define HCI_SCAN_PASSIVE                               LL_SCAN_PASSIVE      //!< Passive Scan
#define HCI_SCAN_ACTIVE                                LL_SCAN_ACTIVE       //!< Active Scan
/** @} End Scan_types */

/**
 * @defgroup Scan_whitelist Scan White List Policy
 * @{
 */
// Scan White List Policy
#define HCI_SCAN_WL_POLICY_ANY_ADV_PKTS                LL_SCAN_WL_POLICY_ANY_ADV_PKTS       //!< Any Advertising Packets
#define HCI_SCAN_WL_POLICY_USE_WHITE_LIST              LL_SCAN_WL_POLICY_USE_WHITE_LIST     //!< Use Whitelist
/** @} End Scan_whitelist */

/**
 * @defgroup Scan_filtering Scan Filtering
 * @{
 */
// Scan Filtering
#define HCI_FILTER_REPORTS_DISABLE                     LL_FILTER_REPORTS_DISABLE    //!< Disable
#define HCI_FILTER_REPORTS_ENABLE                      LL_FILTER_REPORTS_ENABLE     //!< Enable
/** @} End Scan_filtering */

/**
 * @defgroup Scan_commands Scan Commands
 * @{
 */
// Scan Commands
#define HCI_SCAN_STOP                                  LL_SCAN_STOP     //!< Scan Stop
#define HCI_SCAN_START                                 LL_SCAN_START            //!< Scan Start
/** @} End Scan_commands */

/**
 * @defgroup Init_whitelist Initiator White List Policy
 * @{
 */
#define HCI_INIT_WL_POLICY_USE_PEER_ADDR               LL_INIT_WL_POLICY_USE_PEER_ADDR      //!< Use Peer Address
#define HCI_INIT_WL_POLICY_USE_WHITE_LIST              LL_INIT_WL_POLICY_USE_WHITE_LIST     //!< Use Whitelist
/** @} End Init_whitelist */

/**
 * @defgroup Encryption_Related Encryption Related
 * @{
 */
#define HCI_ENCRYPTION_OFF                             LL_ENCRYPTION_OFF        //!< Off
#define HCI_ENCRYPTION_ON                              LL_ENCRYPTION_ON         //!< On
/** @} End Encryption_Related */

/**
 * @defgroup DTM_params Direct Test Mode
 * @{
 */
#define HCI_DIRECT_TEST_PAYLOAD_PRBS9                  LL_DIRECT_TEST_PAYLOAD_PRBS9     //!< PRBS9
#define HCI_DIRECT_TEST_PAYLOAD_0x0F                   LL_DIRECT_TEST_PAYLOAD_0x0F      //!< 0x0F
#define HCI_DIRECT_TEST_PAYLOAD_0x55                   LL_DIRECT_TEST_PAYLOAD_0x55      //!< 0x55
#define HCI_DIRECT_TEST_PAYLOAD_PRBS15                 LL_DIRECT_TEST_PAYLOAD_PRBS15    //!< PRBS15
#define HCI_DIRECT_TEST_PAYLOAD_0xFF                   LL_DIRECT_TEST_PAYLOAD_0xFF      //!< 0xFF
#define HCI_DIRECT_TEST_PAYLOAD_0x00                   LL_DIRECT_TEST_PAYLOAD_0x00      //!< 0x00
#define HCI_DIRECT_TEST_PAYLOAD_0xF0                   LL_DIRECT_TEST_PAYLOAD_0xF0      //!< 0xF0
#define HCI_DIRECT_TEST_PAYLOAD_0xAA                   LL_DIRECT_TEST_PAYLOAD_0xAA      //!< 0xAA
/** @} End DTM_params */

/**
 * @defgroup DTM_enhanced Direct Test Mode Enhanced
 * @{
 */
#define HCI_DTM_STANDARD_MODULATION_INDEX              LL_DTM_STANDARD_MODULATION_INDEX     //!< Standard Modulation
#define HCI_DTM_STABLE_MODULATION_INDEX                LL_DTM_STABLE_MODULATION_INDEX       //!< Stable Modulation
/** @} End DTM_enhanced */

/**
 * @defgroup DTM_TxPktCnt Direct Test Mode Options
 * @{
 */
#define HCI_EXT_DTM_TX_CONTINUOUS                      LL_EXT_DTM_TX_CONTINUOUS                //!< Tx Continuously
/** @} End DTM_TxPktCnt */

/**
 * @defgroup Priv_1_2 Privacy 1.2
 * @{
 */
#define HCI_DISABLE_ADDR_RESOLUTION                    LL_DISABLE_ADDR_RESOLUTION       //!< Disable
#define HCI_ENABLE_ADDR_RESOLUTION                     LL_ENABLE_ADDR_RESOLUTION        //!< Enable
#define HCI_NETWORK_PRIVACY_MODE                       LL_NETWORK_PRIVACY_MODE          //!< Network Privacy
#define HCI_DEVICE_PRIVACY_MODE                        LL_DEVICE_PRIVACY_MODE           //!< Device Privacy
/** @} End Priv_1_2 */

/**
 * @defgroup PHY_2_CODED 2 Mbps & Coded PHY
 * @{
 */
#define HCI_PHY_USE_PHY_PARAM                          LL_PHY_USE_PHY_PARAM     //!< Use Phy Param
#define HCI_PHY_USE_ANY_PHY                            LL_PHY_USE_ANY_PHY       //!< Use any Phy
#define HCI_PHY_1_MBPS                                 LL_PHY_1_MBPS            //!< 1 Mbps
#define HCI_PHY_2_MBPS                                 LL_PHY_2_MBPS            //!< 2 Mbps
#define HCI_PHY_CODED                                  LL_PHY_CODED             //!< Coded
/** @} End PHY_2_CODED */

/**
 * @defgroup PHY_UPDATE PHY Update Complete Event
 * @{
 */
#define PHY_UPDATE_COMPLETE_EVENT_1M                   1  //!< Complete event for 1 Mbps
#define PHY_UPDATE_COMPLETE_EVENT_2M                   2  //!< Complete event for 2 Mbps
#define PHY_UPDATE_COMPLETE_EVENT_CODED                3  //!< Complete event for coded phy
/** @} End PHY_UPDATE */

/**
 * @defgroup RX_Gain RX Gain
 * @{
 */
#define HCI_EXT_RX_GAIN_STD                            LL_EXT_RX_GAIN_STD       //!< Standard Gain
#define HCI_EXT_RX_GAIN_HIGH                           LL_EXT_RX_GAIN_HIGH      //!< High Gain
/** @} End RX_Gain */

/**
 * @defgroup TX_Power_Index TX Power Index
 * @{
 */
#if defined( CC26XX ) || defined( CC13XX )
#define HCI_EXT_TX_POWER_MINUS_20_DBM                   LL_EXT_TX_POWER_MINUS_20_DBM            //!< -20 dBm
#define HCI_EXT_TX_POWER_MINUS_18_DBM                   LL_EXT_TX_POWER_MINUS_18_DBM            //!< -18 dBm
#define HCI_EXT_TX_POWER_MINUS_15_DBM                   LL_EXT_TX_POWER_MINUS_15_DBM            //!< -15 dBm
#define HCI_EXT_TX_POWER_MINUS_12_DBM                   LL_EXT_TX_POWER_MINUS_12_DBM            //!< -12 dBm
#define HCI_EXT_TX_POWER_MINUS_10_DBM                   LL_EXT_TX_POWER_MINUS_10_DBM            //!< -10 dBm
#define HCI_EXT_TX_POWER_MINUS_9_DBM                    LL_EXT_TX_POWER_MINUS_9_DBM             //!< -9 dBm
#define HCI_EXT_TX_POWER_MINUS_6_DBM                    LL_EXT_TX_POWER_MINUS_6_DBM             //!< -6 dBm
#define HCI_EXT_TX_POWER_MINUS_5_DBM                    LL_EXT_TX_POWER_MINUS_5_DBM             //!< -5 dBm
#define HCI_EXT_TX_POWER_MINUS_3_DBM                    LL_EXT_TX_POWER_MINUS_3_DBM             //!< -3 dBm
#define HCI_EXT_TX_POWER_0_DBM                          LL_EXT_TX_POWER_0_DBM                   //!< 0 dBm
#define HCI_EXT_TX_POWER_1_DBM                          LL_EXT_TX_POWER_1_DBM                   //!< 1 dBm
#define HCI_EXT_TX_POWER_2_DBM                          LL_EXT_TX_POWER_2_DBM                   //!< 2 dBm
#define HCI_EXT_TX_POWER_3_DBM                          LL_EXT_TX_POWER_3_DBM                   //!< 3 dBm
#define HCI_EXT_TX_POWER_4_DBM                          LL_EXT_TX_POWER_4_DBM                   //!< 4 dBm
#define HCI_EXT_TX_POWER_5_DBM                          LL_EXT_TX_POWER_5_DBM                   //!< 5 dBm
#if defined( CC13X2P )
#define HCI_EXT_TX_POWER_P2_14_DBM_P4_6_DBM             LL_EXT_TX_POWER_P2_14_DBM_P4_6_DBM      //!< P2: 14 dBm P4: 6 dBm
#define HCI_EXT_TX_POWER_P2_15_DBM_P4_7_DBM             LL_EXT_TX_POWER_P2_15_DBM_P4_7_DBM      //!< P2: 15 dBm P4: 7 dBm
#define HCI_EXT_TX_POWER_P2_16_DBM_P4_8_DBM             LL_EXT_TX_POWER_P2_16_DBM_P4_8_DBM      //!< P2: 16 dBm P4: 8 dBm
#define HCI_EXT_TX_POWER_P2_17_DBM_P4_9_DBM             LL_EXT_TX_POWER_P2_17_DBM_P4_9_DBM      //!< P2: 17 dBm P4: 9 dBm
#define HCI_EXT_TX_POWER_P2_18_DBM_P4_10_DBM            LL_EXT_TX_POWER_P2_18_DBM_P4_10_DBM     //!< P2: 18 dBm P4: 10 dBm
#define HCI_EXT_TX_POWER_P2_19_DBM                      LL_EXT_TX_POWER_P2_19_DBM               //!< P2: 19 dBm
#define HCI_EXT_TX_POWER_P2_20_DBM                      LL_EXT_TX_POWER_P2_20_DBM               //!< P2: 20 dBm
#endif // CC13X2P
#else // CC254x
#define HCI_EXT_TX_POWER_MINUS_23_DBM                  LL_EXT_TX_POWER_MINUS_23_DBM //!< -23 dBm
#define HCI_EXT_TX_POWER_MINUS_6_DBM                   LL_EXT_TX_POWER_MINUS_6_DBM  //!< -6 dBm
#define HCI_EXT_TX_POWER_0_DBM                         LL_EXT_TX_POWER_0_DBM        //!< 0 dBm
#define HCI_EXT_TX_POWER_4_DBM                         LL_EXT_TX_POWER_4_DBM        //!< 4 dBm
#endif // CC26XX/CC13XX
/** @} End TX_Power_Index */

/**
 * @defgroup One_Pkt One Packet Per Event
 * @{
 */
#define HCI_EXT_ENABLE_ONE_PKT_PER_EVT                 LL_EXT_ENABLE_ONE_PKT_PER_EVT    //!< Enable
#define HCI_EXT_DISABLE_ONE_PKT_PER_EVT                LL_EXT_DISABLE_ONE_PKT_PER_EVT   //!< Disable
/** @} End One_Pkt */

/// @cond CC254X

/**
 * @defgroup Clk_Divide Clock Divide on Halt
 * @{
 */
#define HCI_EXT_ENABLE_CLK_DIVIDE_ON_HALT              LL_EXT_ENABLE_CLK_DIVIDE_ON_HALT //!< Enable
#define HCI_EXT_DISABLE_CLK_DIVIDE_ON_HALT             LL_EXT_DISABLE_CLK_DIVIDE_ON_HALT    //!< Disable
/** @} End Clk_Divide */

/// @endcond // CC254X

/**
 * @defgroup NV_Use NV in/not in use
 * @{
 */
#define HCI_EXT_NV_IN_USE                              LL_EXT_NV_IN_USE         //!< NV in Use
#define HCI_EXT_NV_NOT_IN_USE                          LL_EXT_NV_NOT_IN_USE     //!< NV Not in Use
/** @} End NV_Use */

/**
 * @defgroup Fast_TX Fast Tx Response Time
 * @{
 */
#define HCI_EXT_ENABLE_FAST_TX_RESP_TIME               LL_EXT_ENABLE_FAST_TX_RESP_TIME  //!< Enable
#define HCI_EXT_DISABLE_FAST_TX_RESP_TIME              LL_EXT_DISABLE_FAST_TX_RESP_TIME //!< Disable
/** @} End Fast_TX */

/**
 * @defgroup SL_Override Slave Latency Override
 * @{
 */
#define HCI_EXT_ENABLE_SL_OVERRIDE                     LL_EXT_ENABLE_SL_OVERRIDE    //!< Enable
#define HCI_EXT_DISABLE_SL_OVERRIDE                    LL_EXT_DISABLE_SL_OVERRIDE   //!< Disable
/** @} End SL_Override */

/**
 * @defgroup TX_Modulation TX Modulation
 * @{
 */
#define HCI_EXT_TX_MODULATED_CARRIER                   LL_EXT_TX_MODULATED_CARRIER  //!< Modulated
#define HCI_EXT_TX_UNMODULATED_CARRIER                 LL_EXT_TX_UNMODULATED_CARRIER //!< Unmodulated
/** @} End TX_Modulation */

/**
 * @defgroup Freq_Tune Frequency Tuning
 * @{
 */
#define HCI_PTM_SET_FREQ_TUNE_DOWN                     LL_EXT_SET_FREQ_TUNE_DOWN    //!< Tune Down
#define HCI_PTM_SET_FREQ_TUNE_UP                       LL_EXT_SET_FREQ_TUNE_UP      //!< Tune Up
/** @} End Freq_Tune */

/// @cond CC254X

/**
 * @defgroup Map_PM Map PM to I/O Port
 * @{
 */
#define HCI_EXT_PM_IO_PORT_P0                          LL_EXT_PM_IO_PORT_P0
#define HCI_EXT_PM_IO_PORT_P1                          LL_EXT_PM_IO_PORT_P1
#define HCI_EXT_PM_IO_PORT_P2                          LL_EXT_PM_IO_PORT_P2
#define HCI_EXT_PM_IO_PORT_NONE                        LL_EXT_PM_IO_PORT_NONE

#define HCI_EXT_PM_IO_PORT_PIN0                        LL_EXT_PM_IO_PORT_PIN0
#define HCI_EXT_PM_IO_PORT_PIN1                        LL_EXT_PM_IO_PORT_PIN1
#define HCI_EXT_PM_IO_PORT_PIN2                        LL_EXT_PM_IO_PORT_PIN2
#define HCI_EXT_PM_IO_PORT_PIN3                        LL_EXT_PM_IO_PORT_PIN3
#define HCI_EXT_PM_IO_PORT_PIN4                        LL_EXT_PM_IO_PORT_PIN4
#define HCI_EXT_PM_IO_PORT_PIN5                        LL_EXT_PM_IO_PORT_PIN5
#define HCI_EXT_PM_IO_PORT_PIN6                        LL_EXT_PM_IO_PORT_PIN6
#define HCI_EXT_PM_IO_PORT_PIN7                        LL_EXT_PM_IO_PORT_PIN7
/** @} End Map_PM */

/// @endcond // CC254X

/**
 * @defgroup PER_Cmd Packet Error Rate Command
 * @{
 */
#define HCI_EXT_PER_RESET                              LL_EXT_PER_RESET     //!< Reset
#define HCI_EXT_PER_READ                               LL_EXT_PER_READ      //!< Read
/** @} End PER_Cmd */

/// @cond CC254X

/**
 * @defgroup Halt_RF Halt During RF
 * @{
 */
#define HCI_EXT_HALT_DURING_RF_DISABLE                 LL_EXT_HALT_DURING_RF_DISABLE    //!< Disable
#define HCI_EXT_HALT_DURING_RF_ENABLE                  LL_EXT_HALT_DURING_RF_ENABLE     //!< Enable
/** @} End Halt_RF */

/// @endcond // CC254X

/**
 * @defgroup User_Rev Set User Revision Command
 * @{
 */
#define HCI_EXT_SET_USER_REVISION                      LL_EXT_SET_USER_REVISION     //!< Set
#define HCI_EXT_READ_BUILD_REVISION                    LL_EXT_READ_BUILD_REVISION   //!< Read
/** @} End User_Rev */

/**
 * @defgroup Reset_Type Reset Type
 * @{
 */
#define HCI_EXT_RESET_SYSTEM_HARD                      LL_EXT_RESET_SYSTEM_HARD     //!< Hard Reset
#define HCI_EXT_RESET_SYSTEM_SOFT                      LL_EXT_RESET_SYSTEM_SOFT     //!< Soft Reset
/** @} End Reset_Type */

/// @cond CC254X

/**
 * @defgroup Overlapped Overlapped Processing
 * @{
 */
#define HCI_EXT_DISABLE_OVERLAPPED_PROCESSING          LL_EXT_DISABLE_OVERLAPPED_PROCESSING //!< Disable
#define HCI_EXT_ENABLE_OVERLAPPED_PROCESSING           LL_EXT_ENABLE_OVERLAPPED_PROCESSING  //!< Enable
/** @} End Overlapped */

/// @endcond // CC254X

/**
 * @defgroup Compl_Pkts Number of Completed Packets
 * @{
 */
#define HCI_EXT_DISABLE_NUM_COMPL_PKTS_ON_EVENT        LL_EXT_DISABLE_NUM_COMPL_PKTS_ON_EVENT   //!< Disable
#define HCI_EXT_ENABLE_NUM_COMPL_PKTS_ON_EVENT         LL_EXT_ENABLE_NUM_COMPL_PKTS_ON_EVENT    //!< Enable
/** @} End Compl_Pkts */

/**
 * @defgroup Scan_req Scan Request Report
 * @{
 */
#define HCI_EXT_DISABLE_SCAN_REQUEST_REPORT            LL_EXT_DISABLE_SCAN_REQUEST_REPORT   //!< Disable
#define HCI_EXT_ENABLE_SCAN_REQUEST_REPORT             LL_EXT_ENABLE_SCAN_REQUEST_REPORT    //!< Enable
/** @} End Scan_req */

/** @} End HCI_Cmd_API_Params */

/**
 * @defgroup HCI_Event_API_Params HCI Event API Parameters
 *
 * HCI Link Types for Buffer Overflow
 * @{
 */
#define HCI_LINK_TYPE_SCO_BUFFER_OVERFLOW             0    //!< SCO Buffer Overflow
#define HCI_LINK_TYPE_ACL_BUFFER_OVERFLOW             1    //!< ACL Buffer Overflow
/** @} End HCI_Event_API_Params */

/**
 * @defgroup RF_Setup_Phy_Params RF Setup PHY Parameters
 *
 * Enhanced Modem Test BLE5 PHY's
 * @{
 */
#define HCI_EXT_RF_SETUP_1M_PHY                       LL_EXT_RF_SETUP_1M_PHY       //!< 1M
#define HCI_EXT_RF_SETUP_2M_PHY                       LL_EXT_RF_SETUP_2M_PHY       //!< 2M
#define HCI_EXT_RF_SETUP_CODED_S8_PHY                 LL_EXT_RF_SETUP_CODED_S8_PHY //!< Coded-S8
#define HCI_EXT_RF_SETUP_CODED_S2_PHY                 LL_EXT_RF_SETUP_CODED_S2_PHY  //!< Coded-S2
/** @} End RF_Setup_Phy_Params */

/** @} End HCI_Constants */

/*
 * TYPEDEFS
 */

/// @cond NODOC
typedef uint8 hciStatus_t;
/// @endcond // NODOC

/**
 * @defgroup HCI_Events HCI Events
 * @{
 */

/// @brief LE Connection Complete Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8  BLEEventCode;            //!< BLE Event Code
  uint8  status;                  //!< status of event
  uint16 connectionHandle;        //!< connection handle
  uint8  role;                    //!< role of connection
  uint8  peerAddrType;            //!< peer address type
  uint8  peerAddr[B_ADDR_LEN];    //!< peer address
  uint16 connInterval;            //!< connection interval
  uint16 connLatency;             //!< connection latency
  uint16 connTimeout;             //!< connection supervision timeout
  uint8  clockAccuracy;           //!< connection clock accuracy
} hciEvt_BLEConnComplete_t;

/// @brief LE Enhanced Connection Complete Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8  BLEEventCode;            //!< BLE Event Code
  uint8  status;                  //!< status of event
  uint16 connectionHandle;        //!< connection handle
  uint8  role;                    //!< role of connection
  uint8  peerAddrType;            //!< peer address type
  uint8  peerAddr[B_ADDR_LEN];    //!< peer address
  uint16 connInterval;            //!< connection interval
  uint16 connLatency;             //!< connection latency
  uint16 connTimeout;             //!< connection supervision timeout
  uint8  clockAccuracy;           //!< connection clock accuracy
  // Note: These fields normally follow peerAddr. Put here for Host optimization.
  uint8  localRPA[B_ADDR_LEN];    //!< local resolvable private address
  uint8  peerRPA[B_ADDR_LEN];     //!< peer resolvable private address
} hciEvt_BLEEnhConnComplete_t;

/// @brief LE Connection Complete Event
typedef union _hciEvt_BLEConnComplete_u_
{
  hciEvt_BLEConnComplete_t      legEvt;   //!< connection complete event
  hciEvt_BLEEnhConnComplete_t   enhEvt;   //!< enhanced connection complete event
} hciEvt_BLEConnComplete_u;

/// @brief LE Advertising Report Event
typedef struct
{
  uint8  eventType;                       //!< advertisement or scan response event type
  uint8  addrType;                        //!< public or random address type
  uint8  addr[B_ADDR_LEN];                //!< device address
  uint8  dataLen;                         //!< length of report data
  uint8  rspData[B_MAX_ADV_LEN];          //!< report data given by dataLen
  int8   rssi;                            //!< report RSSI
} hciEvt_DevInfo_t;

/// @brief LE Advertising Packet Report Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8  BLEEventCode;            //!< BLE Event Code
  uint8  numDevices;              //!< number of discovered devices
  hciEvt_DevInfo_t* devInfo;      //!< pointer to the array of devInfo
} hciEvt_BLEAdvPktReport_t;

/// @brief VS LE Scan Request Report Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8  BLEEventCode;            //!< BLE Event Code
  uint8  eventType;               //!< scan request event type
  uint8  peerAddrType;            //!< peer address type
  uint8  peerAddr[B_ADDR_LEN];    //!< peer address
  uint8  bleChan;                 //!< BLE channel
  int8   rssi;                    //!< report RSSI
} hciEvt_BLEScanReqReport_t;

/// @brief VS LE Channel Map Update Event
typedef struct
{
  osal_event_hdr_t  hdr;                          //!< osal event header
  uint8   BLEEventCode;                           //!< BLE Event Code
  uint16  connHandle;                             //!< connection handle
  uint8   nextDataChan;                           //!< next channel
  uint8   newChanMap[LL_NUM_BYTES_FOR_CHAN_MAP];  //!< bitmap of used BLE channels
} hciEvt_BLEChanMapUpdate_t;

/// @brief LE Connection Update Complete Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8  BLEEventCode;            //!< BLE Event Code
  uint8  status;                  //!< status of event
  uint16 connectionHandle;        //!< connection handle
  uint16 connInterval;            //!< connection interval
  uint16 connLatency;             //!< connection latency
  uint16 connTimeout;             //!< connection supervision timeout
} hciEvt_BLEConnUpdateComplete_t;

/// @brief LE Read Remote Used Features Complete Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8  BLEEventCode;            //!< BLE Event Code
  uint8  status;                  //!< status of event
  uint16 connectionHandle;        //!< connection handle
  uint8  features[8];             //!< features
} hciEvt_BLEReadRemoteFeatureComplete_t;

/// @brief LE Encryption Change Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8  BLEEventCode;            //!< BLE Event Code
  uint16 connHandle;              //!< connection handle
  uint8  reason;                  //!< reason
  uint8  encEnable;               //!< encryption enabled or not
} hciEvt_EncryptChange_t;

/// @brief LE Long Term Key Requested Event
typedef struct
{
  osal_event_hdr_t  hdr;            //!< osal event header
  uint8  BLEEventCode;              //!< BLE Event Code
  uint16 connHandle;                //!< connection handle
  uint8  random[B_RANDOM_NUM_SIZE]; //!< random
  uint16 encryptedDiversifier;      //!< encrypted diversifier
} hciEvt_BLELTKReq_t;

/// @brief Number of Completed Packets Event
typedef struct
{
  osal_event_hdr_t  hdr;            //!< osal event header
  uint8  numHandles;                //!< number of handles
  uint16 *pConnectionHandle;        //!< pointer to the connection handle array
  uint16 *pNumCompletedPackets;     //!< pointer to the number of completed packets array
} hciEvt_NumCompletedPkt_t;

/// @brief Command Complete Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8  numHciCmdPkt;            //!< number of HCI Command Packet
  uint16 cmdOpcode;               //!< command opcode
  uint8  *pReturnParam;           //!< pointer to the return parameter
} hciEvt_CmdComplete_t;

/// @brief Vendor Specific Command Complete Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8   length;                 //!< length of parametric data, in bytes
  uint16  cmdOpcode;              //!< command opcode
  uint8  *pEventParam;            //!< event parameters
} hciEvt_VSCmdComplete_t;

/// @brief Command Status Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8  cmdStatus;               //!< command status
  uint8  numHciCmdPkt;            //!< number of HCI command packets
  uint16 cmdOpcode;               //!< command opcode
} hciEvt_CommandStatus_t;

/// @brief Hardware Error Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8 hardwareCode;             //!< hardware error code
} hciEvt_HardwareError_t;

/// @brief Disconnection Complete Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8  status;                  //!< status of event
  uint16 connHandle;              //!< connection handle
  uint8  reason;                  //!< reason
} hciEvt_DisconnComplete_t;

/// @brief Data Buffer Overflow Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8 linkType;                 //!< synchronous or asynchronous buffer overflow
} hciEvt_BufferOverflow_t;

/// @brief Authenticated Payload Timeout Expired Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint16 connHandle;              //!< connection handle
} hciEvt_AptoExpired_t;

/// @brief LE Remote Connection Parameter Request Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8  BLEEventCode;            //!< BLE Event Code
  uint8  status;                  //!< status of event
  uint16 connHandle;              //!< connection handle
  uint16 Interval_Min;            //!< minimum connection interval
  uint16 Interval_Max;            //!< maximum connection interval
  uint16 Latency;                 //!< slave latency
  uint16 Timeout;                 //!< supervision timeout
} hciEvt_BLERemoteConnParamReq_t;

/// @brief LE Phy Update Complete Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8  BLEEventCode;            //!< BLE Event Code
  uint8  status;                  //!< status of event
  uint16 connHandle;              //!< connection handle
  uint8  txPhy;                   //!< TX PHY
  uint8  rxPhy;                   //!< RX PHY
} hciEvt_BLEPhyUpdateComplete_t;

/// @brief LE Data Length Change Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8  BLEEventCode;            //!< BLE Event Code
  uint16 connHandle;              //!< connection handle
  uint16 maxTxOctets;             //!< maximum transmission bytes
  uint16 maxTxTime;               //!< maximum transmission time
  uint16 maxRxOctets;             //!< maximum receive bytes
  uint16 maxRxTime;               //!< maximum receive time
} hciEvt_BLEDataLengthChange_t;

/// @brief LE Read P256 Public Key Complete Event
typedef struct
{
  osal_event_hdr_t  hdr;             //!< osal event header
  uint8 BLEEventCode;                //!< BLE Event code
  uint8 status;                      //!< status
  uint8 p256Key[LL_SC_P256_KEY_LEN]; //!< p256 key
} hciEvt_BLEReadP256PublicKeyComplete_t;

/// @brief LE Generate DHKey Complete Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8 BLEEventCode;             //!< BLE Event Code
  uint8 status;                   //!< status
  uint8 dhKey[LL_SC_DHKEY_LEN];   //!< DH Key
} hciEvt_BLEGenDHKeyComplete_t;

/// @brief LE CTE Connection IQ Report Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8  BLEEventCode;            //!< BLE Event Code
  uint16 connHandle;              //!< connection handle
  uint8  phy;                     //!< current phy
  uint8  dataChIndex;             //!< index of data channel
  uint16 rssi;                    //!< current rssi
  uint8  rssiAntenna;             //!< antenna ID
  uint8  cteType;                 //!< cte type
  uint8  slotDuration;            //!< sampling slot 1us or 2us
  uint8  status;                  //!< packet status (success or CRC error)
  uint16 connEvent;               //!< connection event
  uint8  sampleCount;             //!< number of samples
  int8   *iqSamples;              //!< list of interleaved I/Q samples
} hciEvt_BLECteConnectionIqReport_t;

/// @brief LE CTE Request Failed Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8 BLEEventCode;             //!< BLE Event Code
  uint8 status;                   //!< status
  uint16 connHandle;              //!< connection handle
} hciEvt_BLECteRequestFailed_t;

/// @brief LE Extended CTE Connection IQ Report Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8  BLEEventCode;            //!< BLE Event Code
  uint16 totalDataLen;            //!< samples data length includes in all next events
  uint8  eventIndex;              //!< event number
  uint16 connHandle;              //!< connection handle
  uint8  phy;                     //!< current phy
  uint8  dataChIndex;             //!< index of data channel
  uint16 rssi;                    //!< current rssi
  uint8  rssiAntenna;             //!< antenna ID
  uint8  cteType;                 //!< cte type
  uint8  slotDuration;            //!< sampling slot 1us or 2us
  uint8  status;                  //!< packet status (success or CRC error)
  uint16 connEvent;               //!< connection event
  uint8  dataLen;                 //!< samples data length
  uint8  sampleRate;              //!< sample rate (1/2/3/4 MHz)
  uint8  sampleSize;              //!< sample size (8 or 16 bit)
  uint8  sampleCtrl;              //!< sample control - default filtering or RAW_RF
  void   *iqSamples;              //!< list of interleaved I/Q samples (list size is dataLen*2)
} hciEvt_BLEExtCteConnectionIqReport_t;

/// @brief LE CTE Connectionless IQ Report Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8  BLEEventCode;            //!< BLE Event Code
  uint16 syncHandle;              //!< sync handle in periodic adv
  uint8  channelIndex;            //!< index of channel
  uint16 rssi;                    //!< current rssi
  uint8  rssiAntenna;             //!< antenna ID
  uint8  cteType;                 //!< cte type
  uint8  slotDuration;            //!< sampling slot 1us or 2us
  uint8  status;                  //!< packet status (success or CRC error)
  uint16 eventCounter;            //!< periodic adv event counter
  uint8  sampleCount;             //!< number of samples
  int8   *iqSamples;              //!< list of interleaved I/Q samples
} hciEvt_BLECteConnectionlessIqReport_t;

/// @brief LE Extended CTE Connectionless IQ Report Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8  BLEEventCode;            //!< BLE Event Code
  uint16 totalDataLen;            //!< samples data length includes in all next events
  uint8  eventIndex;              //!< event number
  uint16 syncHandle;              //!< sync handle in periodic adv
  uint8  channelIndex;            //!< index of channel
  uint16 rssi;                    //!< current rssi
  uint8  rssiAntenna;             //!< antenna ID
  uint8  cteType;                 //!< cte type
  uint8  slotDuration;            //!< sampling slot 1us or 2us
  uint8  status;                  //!< packet status (success or CRC error)
  uint16 eventCounter;            //!< periodic adv event counter
  uint8  dataLen;                 //!< samples data length
  uint8  sampleRate;              //!< sample rate (1/2/3/4 MHz)
  uint8  sampleSize;              //!< sample size (8 or 16 bit)
  uint8  sampleCtrl;              //!< sample control - default filtering or RAW_RF
  void   *iqSamples;              //!< list of interleaved I/Q samples (list size is dataLen*2)
} hciEvt_BLEExtCteConnectionlessIqReport_t;

/// @brief LE Periodic Advertising Sync Established event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8  BLEEventCode;            //!< BLE Event Code
  uint8  status;                  //!< status of event
  uint16 syncHandle;              //!< sync handle
  uint8  sid;                     //!< Peer SID
  uint8  addrType;                //!< Peer address type
  uint8  address[B_ADDR_LEN];     //!< Peer address
  uint8  phy;                     //!< Peer PHY
  uint16 periodicInterval;        //!< Periodic interval
  uint8  clockAccuracy;           //!< Peer Clock Accuracy
} hciEvt_BLEPeriodicAdvSyncEstablished_t;

/// @brief LE Periodic Advertising Report event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8  BLEEventCode;            //!< BLE Event Code
  uint16 syncHandle;              //!< sync handle
  int8   txPower;                 //!< Tx Power information
  int8   rssi;                    //!< RSSI of the received packet
  uint8  cteType;                 //!< CTE type received
  uint8  dataStatus;              //!< Periodic data status
  uint8  dataLen;                 //!< Periodic data length
  uint8  *data;                   //!< Periodic data received from peer
} hciEvt_BLEPeriodicAdvReport_t;

/// @brief Periodic Advertising Sync Lost Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8  BLEEventCode;            //!< BLE Event Code
  uint16 syncHandle;              //!< sync handle
} hciEvt_BLEPeriodicAdvSyncLost_t;


/// @brief Data structure for HCI Command Complete Event Return Parameter
typedef struct
{
  uint8  status;                  //!< status of event
  uint16 dataPktLen;              //!< data packet length
  uint8  numDataPkts;             //!< number of data packets
} hciRetParam_LeReadBufSize_t;

/// @brief LE Channel Selection Algorithm Event
typedef struct
{
  osal_event_hdr_t  hdr;          //!< osal event header
  uint8  BLEEventCode;            //!< BLE Event Code
  uint16 connHandle;              //!< connection handle
  uint8  chSelAlgo;               //!< channel selection algorithm
} hciEvt_BLEChanSelAlgo_t;

/// @brief HCI Packet event
typedef struct
{
  osal_event_hdr_t hdr;         //!< OSAL event header
  uint8            *pData;      //!< data
} hciPacket_t;

/// @brief HCI Data Packet
typedef struct
{
  osal_event_hdr_t hdr;           //!< OSAL Event header
  uint8  pktType;                 //!< packet type
  uint16 connHandle;              //!< connection handle
  uint8  pbFlag;                  //!< flag
  uint16 pktLen;                  //!< packet length
  uint8  *pData;                  //!< data
} hciDataPacket_t;

/// @cond NODOC

// OSAL HCI_DATA_EVENT message format. This message is used to forward incoming
// data messages up to an application
typedef struct
{
  osal_event_hdr_t hdr;                   //!< OSAL event header
  uint16 connHandle;                      //!< connection handle
  uint8  pbFlag;                          //!< data packet boundary flag
  uint16 len;                             //!< length of data packet
  uint8  *pData;                          //!< data packet given by len
} hciDataEvent_t;

/// @endcond // NODOC

#if defined( CC26XX ) || defined( CC13XX )
PACKED_TYPEDEF_STRUCT
#else // CC254x
typedef struct
#endif // CC26XX/CC13XX
{
  uint8 connId;                           //!< device connection handle
  uint8 role;                             //!< device connection role
  uint8 addr[LL_DEVICE_ADDR_LEN];         //!< peer device address
  uint8 addrType;                         //!< peer device address type
} hciConnInfo_t; //!< Connection Info

#if defined( CC26XX ) || defined( CC13XX )
PACKED_TYPEDEF_STRUCT
#else // CC254x
typedef struct
#endif // CC26XX/CC13XX
{
  uint32_t  accessAddr;                          //! return error code if failed to get conn info
  uint16_t  connInterval;                        //! connection interval time, range (7.5ms, 4s), 625us increments
  uint8_t   hopValue;                            //! Hop value for conn alg 1, integer range (5,16), can also be used to tell if using alg 2 by sending special code not in normal hop range i.e. 0xff or 0x00.
  uint16_t  mSCA;                                //! master sleep clock accuracy code mapped based on BLE spec
  uint8_t   nextChan;                            //! next data channel
  uint8_t   chanMap[LL_NUM_BYTES_FOR_CHAN_MAP];  //! bitmap of used BLE channels
  uint8_t   crcInit[LL_PKT_CRC_LEN];             //! connection CRC initialization value (24 bits)
}hciActiveConnInfo_t;

#if defined( CC26XX ) || defined( CC13XX )
PACKED_TYPEDEF_STRUCT
#else // CC254x
typedef struct
#endif // CC26XX/CC13XX
{
  uint16 numPkts;                         //!< total number of rx packets
  uint16 numCrcErr;                       //!< number rx packets with CRC error
  uint16 numEvents;                       //!< number of connection events
  uint16 numMMissedEvents;                //!< number missed connection events
} hciPER_t; //!<Packet Error Rate

/** @} End HCI_Events */

/*
 * LOCAL VARIABLES
 */

/*
 * GLOBAL VARIABLES
 */

/// @cond NODOC

/**
 * Allocate memory using buffer management.
 *
 * @note This function should never be called by the application. It is only
 * used by HCI and L2CAP_bm_alloc.
 *
 * @param size Number of bytes to allocate from the heap.
 *
 * @return Pointer to buffer, or NULL.
 */
extern void *HCI_bm_alloc( uint16 size );

/**
 * Checks that the connection time parameter ranges are valid
 *
 * Also checks if the min/max CI range is valid.
 *
 * @param connIntervalMin Minimum connection interval.
 * @param connIntervalMax Maximum connection interval.
 * @param connLatency Connection slave latency.
 * @param connTimeout Connection supervision timeout.
 *
 * @return TRUE:  Connection time parameter check is valid.
 * @return FALSE: Connection time parameter check is invalid.
 */
extern uint8 HCI_ValidConnTimeParams( uint16 connIntervalMin,
                                      uint16 connIntervalMax,
                                      uint16 connLatency,
                                      uint16 connTimeout );

/**
 * HCI vendor specific registration for HCI Test Application.
 *
 * @param taskID The HCI Test Application OSAL task identifier.
 */
extern void HCI_TestAppTaskRegister( uint8 taskID );

/**
 * HCI vendor specific registration for Host GAP.
 *
 * @param taskID The Host GAP OSAL task identifier.
 */
extern void HCI_GAPTaskRegister( uint8 taskID );

/**
 * HCI vendor specific registration for Host L2CAP.
 *
 * @param taskID The Host L2CAP OSAL task identifier.
 */
extern void HCI_L2CAPTaskRegister( uint8 taskID );

/**
 * HCI vendor specific registration for Host SMP.
 *
 * @param taskID The Host SMP OSAL task identifier.
 */
extern void HCI_SMPTaskRegister( uint8 taskID );

/**
 * HCI vendor specific registration for Host extended commands.
 *
 * @param taskID The Host Extended Command OSAL task identifier.
 */
extern void HCI_ExtTaskRegister( uint8 taskID );

/**
 * Send an ACL data packet over a connection.
 *
 * @note Empty packets are not sent.
 *
 * @warning L2CAP is affected by this routine's status values as it must remap
 * them to Host status values. If any additional status values are added
 * and/or changed in this routine, a TI stack engineer must be notified!
 *
 * @par Corresponding Events
 * @ref hciEvt_NumCompletedPkt_t
 *
 * @param connHandle Connection ID (handle).
 * @param pbFlag Packet Boundary Flag.
 * @param pktLen Number of bytes of data to transmit.
 * @param *pData Pointer to data buffer to transmit.
 *
 * @return @ref HCI_ERROR_CODE_CONTROLLER_BUSY
 * @return @ref HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS
 * @return @ref HCI_SUCCESS : the data was transmitted and freed, or it is
 *         still in use (i.e. queued).
 * @return other value: data transmission failed
 */

extern hciStatus_t HCI_SendDataPkt( uint16 connHandle,
                                    uint8  pbFlag,
                                    uint16 pktLen,
                                    uint8  *pData );

/**
 * Terminate a connection.
 *
 * @par Corresponding Events
 * @ref hciEvt_CommandStatus_t <br>
 * @ref hciEvt_DisconnComplete_t
 *
 * @param connHandle Connection handle.
 * @param reason @ref Disconnect_Reasons
 *
 * @par Corresponding Event
 * @ref hciEvt_CommandStatus_t with cmdOpcode @ref HCI_DISCONNECT
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_DisconnectCmd( uint16 connHandle,
                                      uint8  reason );

/// @endcond // NODOC

/**
 * Request version information from the remote device in a connection.
 *
 * @par Corresponding Events
 * @ref hciEvt_CommandStatus_t with cmdOpcode @ref HCI_READ_REMOTE_VERSION_INFO <br>
 * @ref hciPacket_t with @ref HCI_READ_REMOTE_INFO_COMPLETE_EVENT_CODE
 *      (if HCI_SUCCESS is returned) <br>
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_READ_REMOTE_VERSION_INFO
 *      (if HCI_SUCCESS is not returned) <br>
 *
 * @param connHandle Connection handle.
 *
 * @return @ref HCI_SUCCESS
 * @return @ref HCI_ERROR_CODE_UNKNOWN_CONN_ID : no active connection
 * @return @ref HCI_ERROR_CODE_CONTROLLER_BUSY : version request already sent
 *         and is pending
 */
extern hciStatus_t HCI_ReadRemoteVersionInfoCmd( uint16 connHandle );


/**
 * Set the HCI event mask
 *
 * The event mask is used to determine which events are supported.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_SET_EVENT_MASK
 *
 * @param pMask Pointer to an eight byte event mask.
 *
 * @return @ref HCI_SUCCESS
 * @return @ref HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS : bad parameters
 */
extern hciStatus_t HCI_SetEventMaskCmd( uint8 *pMask );


/**
 * Set the HCI event mask page 2
 *
 * The HCI Event mask page 2 is used to determine which events are supported.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_SET_EVENT_MASK_PAGE_2
 *
 * @param pMask Pointer to an eight byte event mask.
 *
 * @return @ref HCI_SUCCESS
 * @return @ref HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS : bad parameters
 */
extern hciStatus_t HCI_SetEventMaskPage2Cmd( uint8 *pMask );

/**
 * Reset the Link Layer.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_RESET
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_ResetCmd( void );

/**
 * Read the transmit power level.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_READ_TRANSMIT_POWER
 *
 * @param connHandle Connection handle.
 * @param txPwrType ref TX_Power_Mode
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_ReadTransmitPowerLevelCmd( uint16 connHandle,
                                                  uint8  txPwrType );

/// @cond CC254X

/**
 * Turn flow control on or off
 *
 * Turn flow control on or off for data sent from the Controller to Host.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode
 *      @ref HCI_SET_CONTROLLER_TO_HOST_FLOW_CONTROL
 *
 * @param @ref Flow_Control
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_SetControllerToHostFlowCtrlCmd( uint8 flowControlEnable );

/**
 * Set maximum ACL buffer size
 *
 * Used by the host to notify the Controller of the maximum size ACL buffer size
 * the Controller can send to the Host.
 *
 * @note This command is currently ignored by the Controller. It is assumed that
 * the Host can always handle the maximum BLE data packet size.
 *
 * @par Corresponding Events:
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_HOST_BUFFER_SIZE
 *
 * @param hostAclPktLen        Host ACL data packet length.
 * @param hostSyncPktLen       Host SCO data packet length .
 * @param hostTotalNumAclPkts  Host total number of ACL data packets.
 * @param hostTotalNumSyncPkts Host total number of SCO data packets.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_HostBufferSizeCmd( uint16 hostAclPktLen,
                                          uint8  hostSyncPktLen,
                                          uint16 hostTotalNumAclPkts,
                                          uint16 hostTotalNumSyncPkts );

/// @endcond // CC254X

/**
 * Set the number of HCI data packets completed per connection handle
 *
 * Used by the Host to notify the Controller of the number of HCI data packets
 * that have been completed for each connection handle since this command was
 * previously sent to the controller.
 *
 * The Host_Number_Of_Conpleted_Packets command is a special command. No event
 * is normally generated after the command has completed. The command should
 * only be issued by the Host if flow control in the direction from controller
 * to the host is on and there is at least one connection, or if the controller
 * is in local loop-back mode.
 *
 * @note It is assumed that there will be at most only one handle. Even if more
 * than one handle is provided, the Controller does not track Host buffers as a
 * function of connection handles (and isn't required to do so).
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_HOST_NUM_COMPLETED_PACKETS
 *      (if HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS is returned)
 *
 * @param numHandles  Number of connection handles.
 * @param connHandles Array of connection handles.
 * @param numCompletedPkts Array of number of completed packets.
 *
 * @return @ref HCI_SUCCESS
 * @return @ref HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS
 */
extern hciStatus_t HCI_HostNumCompletedPktCmd( uint8   numHandles,
                                               uint16 *connHandles,
                                               uint16 *numCompletedPkts );

/**
 * Read the connection's Authenticated Payload Timeout value.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_READ_AUTH_PAYLOAD_TIMEOUT
 *
 * @param connHandle The LL connection ID to read the APTO value from.
 * @param aptoValue Pointer to current APTO value, in units of 10 ms.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_ReadAuthPayloadTimeoutCmd( uint16  connHandle,
                                                  uint16 *aptoValue );

/**
 * Used to write the connection's Authenticated Payload Timeout value.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_WRITE_AUTH_PAYLOAD_TIMEOUT
 *
 * @param connHandle The LL connection ID to write the APTO value to.
 * @param aptoValue  The APTO value, in units of 10 ms.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_WriteAuthPayloadTimeoutCmd( uint16 connHandle,
                                                   uint16 aptoValue );

/**
 * Read the local version information.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_READ_LOCAL_VERSION_INFO
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_ReadLocalVersionInfoCmd( void );

/**
 * Read the locally supported commands.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_READ_LOCAL_SUPPORTED_COMMANDS
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_ReadLocalSupportedCommandsCmd( void );

/**
 * Read the locally supported features.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_READ_LOCAL_SUPPORTED_FEATURES
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_ReadLocalSupportedFeaturesCmd( void );

/**
 * Read this device's BLE address (BDADDR).
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_READ_BDADDR
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_ReadBDADDRCmd( void );

/**
 * Read RSSI of last packet
 *
 *  Read the RSSI of the last packet received on a connection given by the
 *  connection handle. If the Receiver Modem test is running
 *  (@ref HCI_EXT_ModemTestRxCmd ), then the RF RSSI for the last received data
 *  will be returned. If there is no RSSI value, then @ref HCI_RSSI_NOT_AVAILABLE
 *  will be returned.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_READ_RSSI
 *
 * @param connHandle Connection handle.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_ReadRssiCmd( uint16 connHandle );

/**
 * Set the HCI LE event mask
 *
 * The event mask to determine which LE events are supported.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_SET_EVENT_MASK
 *
 * @param pEventMask Pointer to LE event mask of 8 bytes.
 *
 * @return @ref HCI_SUCCESS
 * @return @ref HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS
 */
extern hciStatus_t HCI_LE_SetEventMaskCmd( uint8 *pEventMask );

/**
 * Determine the maximum ACL data packet size allowed by the Controller.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_READ_BUFFER_SIZE
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_ReadBufSizeCmd( void );

/**
 * Read the LE locally supported features.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode
 *      @ref HCI_LE_READ_LOCAL_SUPPORTED_FEATURES
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_ReadLocalSupportedFeaturesCmd( void );

/**
 * Set this device's Random address.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_SET_RANDOM_ADDR
 *
 * @param pRandAddr Pointer to random address.
 *
 * @return @ref HCI_SUCCESS
 * @return @ref HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS
 */
extern hciStatus_t HCI_LE_SetRandomAddressCmd( uint8 *pRandAddr );

/**
 * Set the Advertising parameters.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_SET_ADV_PARAM
 *
 * @param advIntervalMin Minimum allowed advertising interval.
 * @param advIntervalMax Maximum allowed advertising interval.
 * @param advType @ref Adv_type
 * @param ownAddrType@ref Addr_type
 * @param directAddrType @ref Addr_type
 * @param directAddr Pointer to address of device when using directed advertising.
 * @param advChannelMap @ref Adv_channels
 * @param advFilterPolicy @ref Adv_whitelist
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_SetAdvParamCmd( uint16 advIntervalMin,
                                          uint16 advIntervalMax,
                                          uint8  advType,
                                          uint8  ownAddrType,
                                          uint8  directAddrType,
                                          uint8  *directAddr,
                                          uint8  advChannelMap,
                                          uint8  advFilterPolicy );

/**
 * Set the Advertising data.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_SET_ADV_DATA
 *
 * @param dataLen Length of Advertising data.
 * @param pData Pointer to Advertising data.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_SetAdvDataCmd( uint8 dataLen,
                                         uint8 *pData );
/**
 * Set the Advertising data.
 * This function will be called only when BLE3_CMD compilation flag
 * is set
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_SET_ADV_DATA
 *
 * @param pCmdParams - Pointer to input parameters
 *
 * @return @ref HCI_SUCCESS
 */
hciStatus_t HCI_LE_SetExtAdvData( aeSetDataCmd_t *pCmdParams );

/**
 * Set the Advertising Scan Response data.
 * This function will be called only when BLE3_CMD compilation flag
 * is set
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_SET_SCAN_RSP_DATA
 *
 * @param pCmdParams - Pointer to input parameters
 *
 * @return @ref HCI_SUCCESS
 */
hciStatus_t HCI_LE_SetExtScanRspData( aeSetDataCmd_t *pCmdParams );

/**
 * Set the Advertising status.
 * This function will be called only when BLE3_CMD compilation flag
 * is set
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_MAKE_DISCOVERABLE_DONE or
 * @HCI_LE_END_DISCOVERABLE_DONE
 *
 * @param pCmdParams - Pointer to input parameters
 *
 * @return @ref HCI_SUCCESS
 */
hciStatus_t HCI_LE_SetAdvStatus( aeEnableCmd_t *pCmdParams );

/**
 * Set the Advertising Scan Response data.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_SET_SCAN_RSP_DATA
 *
 * @param dataLen Length of Scan Response data.
 * @param pData Pointer to Scan Response data.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_SetScanRspDataCmd( uint8 dataLen,
                                             uint8 *pData );

/**
 * Turn Advertising on or off.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_SET_ADV_ENABLE
 *
 * @param advEnable @ref Adv_commands
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_SetAdvEnableCmd( uint8 advEnable );

/**
 * Read transmit power when Advertising.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_READ_ADV_CHANNEL_TX_POWER
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_ReadAdvChanTxPowerCmd( void );

/**
 * Set the Scan parameters.
 *
 * @note When the scanWindow equals the scanInterval then scanning is continuous.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_SET_SCAN_PARAM
 *
 * @param scanType@ref Scan_types
 * @param scanInterval Time between scan events.
 * @param scanWindow Time of scan before scan event ends.
 * @param ownAddrType This device's address.
 * @param filterPolicy @ref Scan_types
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_SetScanParamCmd( uint8  scanType,
                                           uint16 scanInterval,
                                           uint16 scanWindow,
                                           uint8  ownAddrType,
                                           uint8  filterPolicy );

/**
 * Turn Scanning on or off.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_SET_SCAN_ENABLE
 * @ref hciEvt_DevInfo_t
 *
 * @param scanEnable @ref Scan_commands
 * @param filterDuplicates @ref Scan_filtering
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_SetScanEnableCmd( uint8 scanEnable,
                                            uint8 filterDuplicates );

/**
 * Create a connection.
 *
 * @note When the scanWindow equals the scanInterval then scanning is continuous.
 *
 * @par Corresponding Events
 * @ref  hciEvt_CommandStatus_t with cmdOpcode @ref HCI_LE_CREATE_CONNECTION<br>
 * @ref  hciEvt_BLEConnComplete_u
 *
 * @param scanInterval Time between Init scan events.
 * @param scanWindow Time of scan before Init scan event ends.
 * @param initFilterPolicy @ref Init_whitelist
 * @param addrTypePeer @ref Addr_type
 * @param peerAddr Pointer to peer device's address.
 * @param ownAddrType @ref Addr_type
 * @param connIntervalMin Minimum allowed connection interval.
 * @param connIntervalMax Maximum allowed connection interval.
 * @param connLatency Number of skipped events (slave latency).
 * @param connTimeout Connection supervision timeout.
 * @param minLen Info parameter about min length of conn.
 * @param maxLen Info parameter about max length of conn.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_CreateConnCmd( uint16 scanInterval,
                                         uint16 scanWindow,
                                         uint8  initFilterPolicy,
                                         uint8  addrTypePeer,
                                         uint8  *peerAddr,
                                         uint8  ownAddrType,
                                         uint16 connIntervalMin,
                                         uint16 connIntervalMax,
                                         uint16 connLatency,
                                         uint16 connTimeout,
                                         uint16 minLen,
                                         uint16 maxLen );

/**
 * Cancel connection.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_CREATE_CONNECTION_CANCEL
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_CreateConnCancelCmd( void );

/**
 * Read the white list.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_READ_WHITE_LIST_SIZE
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_ReadWhiteListSizeCmd( void );


/**
 * Clear the white list.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_CLEAR_WHITE_LIST
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_ClearWhiteListCmd( void );


/**
 * Add a white list entry.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_ADD_WHITE_LIST
 *
 * @param addrType @ref Addr_type
 * @param devAddr Pointer to address of device to put in white list.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_AddWhiteListCmd( uint8 addrType,
                                           uint8 *devAddr );


/**
 * Remove a white list entry.
 *
 * @par Corresponding Events
   @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_REMOVE_WHITE_LIST
 *
 * @param addrType @ref Addr_type
 * @param devAddr Pointer to address of device to remove from the white list.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_RemoveWhiteListCmd( uint8 addrType,
                                              uint8 *devAddr );

/**
 * Update the connection parameters.
 *
 * @par Corresponding Events
 * @ref  hciEvt_CommandStatus_t with cmdOpcode @ref HCI_LE_CONNECTION_UPDATE<br>
 * @ref  hciEvt_BLEConnUpdateComplete_t
 *
 * @param connHandle Connection handle.
 * @param connIntervalMin Minimum allowed connection interval.
 * @param connIntervalMax Maximum allowed connection interval.
 * @param connLatency Number of skipped events (slave latency).
 * @param connTimeout Connection supervision timeout.
 * @param minLen Info parameter about min length of conn.
 * @param maxLen Info parameter about max length of conn.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_ConnUpdateCmd( uint16 connHandle,
                                         uint16 connIntervalMin,
                                         uint16 connIntervalMax,
                                         uint16 connLatency,
                                         uint16 connTimeout,
                                         uint16 minLen,
                                         uint16 maxLen );

/**
 * Update the current data channel map.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION
 *
 * @param chanMap Pointer to the new channel map.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_SetHostChanClassificationCmd( uint8 *chanMap );

/**
 * Update the default channel map.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_EXT_SET_HOST_DEFAULT_CHANNEL_CLASSIFICATION
 *
 * @param chanMap Pointer to the new channel map.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_SetHostDefChanClassificationCmd( uint8 *chanMap );

/**
 * Update the channel map of specific connection.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_EXT_SET_HOST_CONNECTION_CHANNEL_CLASSIFICATION
 *
 * @param chanMap Pointer to the new channel map.
 * @param connID  connection handle
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_SetHostConnChanClassificationCmd( uint8 *chanMap , uint16 connID );

/**
 * Read a connection's data channel map.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION
 *
 * @param connHandle Connection handle.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_ReadChannelMapCmd( uint16 connHandle );

/**
 * Read the remote device's used features.
 *
 * @par Corresponding Events
 * @ref  hciEvt_CommandStatus_t with cmdOpcode
 *       @ref HCI_LE_READ_REMOTE_USED_FEATURES <br>
 * @ref  hciEvt_BLEReadRemoteFeatureComplete_t
 *
 * @param connHandle Connection handle.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_ReadRemoteUsedFeaturesCmd( uint16 connHandle );

/**
 * Perform an encryption using AES128.
 *
 * @note Input parameters are ordered MSB..LSB.
 *
 * @note If a task has registered with HCI to
 * receive security events, the @ref hciEvt_CmdComplete_t events resulting from
 * this operation will be sent to it instead of the task that registered with
 * @ref GAP_RegisterForMsgs.  This is the case for all of the sample
 * applications.  In this case, the event will then be forwarded to the
 * application as a @ref HCI_SMP_EVENT_EVENT stack message.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_ENCRYPT
 *
 * @param key Pointer to 16 byte encryption key.
 * @param plainText Pointer to 16 byte plain-text data.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_EncryptCmd( uint8 *key,
                                      uint8 *plainText );

/**
 * Generate a random number.
 *
 * The random number generation will not complete until any current radio
 * operations end. If this is the case, the "status" of the pReturnParam of the
 * initial @ref hciEvt_CmdComplete_t will be set to
 * LL_STATUS_ERROR_DUE_TO_DELAYED_RESOURCES (0x12). Another
 * @ref hciEvt_CmdComplete_t will be sent after the radio operations end that
 * contains the random number.
 *
 * @note If a task has registered with HCI to
 * receive security events, the @ref hciEvt_CmdComplete_t events resulting from
 * this operation will be sent to it instead of the task that registered with
 * @ref GAP_RegisterForMsgs.  This is the case for all of the sample
 * applications.  In this case, the event will then be forwarded to the
 * application as a @ref HCI_SMP_EVENT_EVENT stack message.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_RAND
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_RandCmd( void );

/**
 * Start encryption in a connection.
 *
 * @par Corresponding Events
 * @ref  hciEvt_CommandStatus_t with cmdOpcode @ref HCI_LE_START_ENCRYPTION <br>
 * @ref  hciEvt_EncryptChange_t <br>
 * @ref  hciEvt_EncryptChange_t
 *
 * @param connHandle Connection handle.
 * @param random Pointer to eight byte Random number.
 * @param encDiv Pointer to two byte Encrypted Diversifier.
 * @param ltk Pointer to 16 byte Long Term Key.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_StartEncyptCmd( uint16 connHandle,
                                          uint8  *random,
                                          uint8  *encDiv,
                                          uint8  *ltk );

/**
 * Send a positive LTK reply to the controller
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_LTK_REQ_REPLY
 *
 * @param connHandle Connection handle.
 * @param ltk Pointer to 16 byte Long Term Key.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_LtkReqReplyCmd( uint16 connHandle,
                                          uint8  *ltk );


/**
 * Send a negative LTK reply to the Controller
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_LTK_REQ_NEG_REPLY
 *
 * @param connHandle Connection handle.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_LtkReqNegReplyCmd( uint16 connHandle );

/**
 * Read the Controller's supported states.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_READ_SUPPORTED_STATES
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_ReadSupportedStatesCmd( void );

/**
 * Start the receiver Direct Test Mode test.
 *
 * @note An HCI reset should be issued when done using DTM
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_RECEIVER_TEST
 *
 * @param rxChan Rx Channel k=0..39, where F=2402+(k*2MHz).
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_ReceiverTestCmd( uint8 rxChan );

/**
 * This LE API is used to start the transmit Direct Test Mode test.
 *
 * The Controller shall transmit at maximum power.
 *
 * @warning An HCI reset should be issued when done using DTM.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_TRANSMITTER_TEST
 *
 * @param txChan Tx RF frequency k=0..39, where F=2402+(k*2MHz).
 * @param dataLen Test data length: 0..37 bytes
 * @param pktPayload @ref DTM_params
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_TransmitterTestCmd( uint8 txChan,
                                              uint8 dataLen,
                                              uint8 pktPayload );

/**
 * End the Direct Test Mode test.
 *
 * @warning An HCI reset should be issued when done using DTM.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_TEST_END
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_TestEndCmd( void );

// V4.1

/**
 * Positively reply to the Connection Parameter Update Request
 *
 * This LE API is used to positively reply to the HCI LE Remote Connection
 * Parameter Request event from the Controller. This command indicates that the
 * Host has accepted the remote device's request to change connection parameters.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode
 *      @ref HCI_LE_REMOTE_CONN_PARAM_REQ_REPLY
 *
 * @param connHandle Connection handle.
 * @param connIntervalMin Minimum allowed connection interval.
 * @param connIntervalMax Maximum allowed connection interval.
 * @param connLatency Number of skipped events (slave latency).
 * @param connTimeout Connection supervision timeout.
 * @param minLen Info parameter about min length of conn.
 * @param maxLen Info parameter about max length of conn.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_RemoteConnParamReqReplyCmd( uint16 connHandle,
                                                      uint16 connIntervalMin,
                                                      uint16 connIntervalMax,
                                                      uint16 connLatency,
                                                      uint16 connTimeout,
                                                      uint16 minLen,
                                                      uint16 maxLen );

/**
 * Negatively reply to the Connection Parameter Update Request
 *
 * This LE API is used to negatively reply to the HCI LE Remote Connection
 * Parameter Request event from the Controller. This command indicates that the
 * Host has denied the remote device's request to change connection parameters.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode
 * @ref HCI_LE_REMOTE_CONN_PARAM_REQ_NEG_REPLY
 *
 * @param connHandle Connection handle.
 * @param reason Reason connection parameter request was rejected.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_RemoteConnParamReqNegReplyCmd( uint16 connHandle,
                                                         uint8  reason );

// V4.2 Extended Data Length

/**
 * Set Data Length
 *
 * Set the maximum transmission packet size and the maximum packet transmission
 * time for the connection.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_SET_DATA_LENGTH
 *
 * @param connHandle Connection handle.
 * @param txOctets Maximum transmit payload size (in bytes).
 * @param txTime Maximum transmit time (in us).
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_SetDataLenCmd( uint16 connHandle,
                                         uint16 txOctets,
                                         uint16 txTime );

/**
 * Read Suggested Default Data Length
 *
 * Read the default maximum transmit packet size and the default maximum packet
 * transmit time to be used for new connections.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_ReadSuggestedDefaultDataLenCmd( void );

/**
 * Write Suggested Default Data Length
 *
 * Set the default maximum transmission packet size and the default maximum
 * packet transmission time for the connection.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode
 *      @ref HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH
 *
 * @param txOctets Maximum transmit payload size (in bytes).
 * @param txTime Maximum transmit time (in us).
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_WriteSuggestedDefaultDataLenCmd( uint16 txOctets,
                                                           uint16 txTime );

/**
 * Read max data length
 *
 * Read the maximum supported transmit and receive payload octets and packet
 * duration times.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_READ_MAX_DATA_LENGTH
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_ReadMaxDataLenCmd( void );

// V4.2 Privacy 1.2

/**
 * Add a device to the resolving list
 *
 * Add one device to the list of address translations used to resolve Resolvable
 * Private Addresses in the Controller.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST
 *
 * @param peerIdAddrType @ref Addr_type
 * @param peerIdAddr Peer device Identity Address.
 * @param peerIRK IRK of peer device.
 * @param localIRK IRK for own device.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_AddDeviceToResolvingListCmd( uint8  peerIdAddrType,
                                                       uint8 *peerIdAddr,
                                                       uint8 *peerIRK,
                                                       uint8 *localIRK );

/**
 * Remove a device from the whitelist
 *
 * Remove one device from the list of address translations used to resolve
 * Resolvable Private Addresses in the Controller.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST
 *
 * @param peerIdAddrType @ref Addr_type
 * @param peerIdAddr Peer device Identity Address.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_RemoveDeviceFromResolvingListCmd( uint8  peerIdAddrType,
                                                            uint8 *peerIdAddr );

/**
 * Remove all devices from the whitelist
 *
 * Remove all devices from the list of address translations used to resolve
 * Resolvable Private addresses in the Controller.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_CLEAR_RESOLVING_LIST
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_ClearResolvingListCmd( void );

/**
 * Read Resolving List Size
 *
 * Read the total number of address translation entries in the resolving list
 * that can be stored in the Controller.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_READ_RESOLVING_LIST_SIZE
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_ReadResolvingListSizeCmd( void );

/**
 * Read the Peer Resolvable Address
 *
 * Get the current peer Resolvable Private Address being used for the
 * corresponding peer Public or Random (Static) Identity Address.
 *
 * @warning The peer's Resolvable Private Address being used may change after
 * this command is called.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_READ_PEER_RESOLVABLE_ADDRESS
 *
 * @param peerIdAddrType @ref Addr_type
 * @param peerIdAddr Peer device Identity Address.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_ReadPeerResolvableAddressCmd( uint8  peerIdAddrType,
                                                        uint8 *peerIdAddr );

/**
 * Read Local Resolvable Private Address
 *
 * Get the current local Resolvable Private Address being used for the
 * corresponding local Public or Random (Static) Identity Address.
 *
 * @warning The local Resolvable Private Address being used may change after
 * this command is called.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode
 *      @ref HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS
 *
 * @param localIdAddrType @ref Addr_type
 * @param localIdAddr     Local device Identity Address.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_ReadLocalResolvableAddressCmd( uint8  localIdAddrType,
                                                         uint8 *localIdAddr );

/**
 * Disable / Enable Address Resolution
 *
 * Enable resolution of Resolvable Private Addresses in the Controller. This
 * causes the Controller to use the resolving list whenever the Controller
 * receives a local or peer Resolvable Private Address.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE
 *
 * @param addrResolutionEnable @ref Priv_1_2
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_SetAddressResolutionEnableCmd( uint8 addrResolutionEnable );

/**
 * Set Resolvable Private Address Timeout
 *
 * Set the length of time the Controller  uses a Resolvable Private Address
 * before a new Resolvable Private Address is generated and starts being used.
 *
 * @note This timeout applies to all addresses generated by the Controller..
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT
 *
 * @param rpaTimeout RPA timeout (in secs).
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_SetResolvablePrivateAddressTimeoutCmd( uint16 rpaTimeout );

/**
 * Set Privacy Mode
 *
 * Set the Privacy Mode to either Network Privacy Mode or Device Privacy Mode.
 * The Privacy Mode can be set for any peer in the Resolving List.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_SET_PRIVACY_MODE
 *
 * @param peerIdAddrType @ref Addr_type
 * @param peerIdAddr     Peer device Identity Address.
 * @param privacyMode    @ref HCI_NETWORK_PRIVACY_MODE
 *             @ref HCI_DEVICE_PRIVACY_MODE
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_SetPrivacyModeCmd( uint8  peerIdAddrType,
                                             uint8 *peerIdAddr,
                                             uint8  privacyMode );

// V4.2 Secure Connections

/**
 * Read local P-256 Public Key
 *
 * Read the local P-256 public key from the Controller. The Controller shall
 * generate a new P-256 public / private key pair upon receipt of this command.
 *
 * @warning This routine will tie up the LL for about 160 ms
 *
 * @par Corresponding Events
 * @ref hciEvt_CommandStatus_t with cmdOpcode
 *      @ref HCI_LE_READ_LOCAL_P256_PUBLIC_KEY <br>
 * @ref hciEvt_BLEReadP256PublicKeyComplete_t
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_ReadLocalP256PublicKeyCmd( void );

/**
 * Generate Diffie-Hellman Key
 *
 * This LE API is used to initiate the generation of a Diffie-Hellman key in the
 * Controller for use over the LE transport. This command takes the remote P-256
 * public key as input. The Diffie-Hellman key generation uses the private key
 * generated by LE_Read_Local_P256_Public_Key command.
 *
 * @warning This routine will tie up the LL for about 160 ms
 *
 * @par Corresponding Events
 * @ref hciEvt_CommandStatus_t with cmdOpcode @ref HCI_LE_GENERATE_DHKEY <br>
 * @ref hciEvt_BLEGenDHKeyComplete_t
 *
 * @param publicKey: The remote P-256 public key (X-Y format).
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_GenerateDHKeyCmd( uint8 *publicKey );

// V5.0 2M and Coded PHY

/**
 * Read the current transmitter and receiver PHY.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_READ_PHY
 *      and Return Parameters of -  0: Status
 *                                  1: Connection Handle LSB
 *                                  2: Connection Handle MSB
 *                                  3: Tx PHY
 *                                  4: Rx PHY
 *
 * @param connHandle Connection handle.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_ReadPhyCmd( uint16 connHandle );

/**
 * Set Default PHY
 *
 * Allows the Host to specify its preferred values for the transmitter and
 * receiver PHY to be used for all subsequent connections.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_SET_DEFAULT_PHY
 *
 * @param allPhys Host preference on how to handle txPhy and rxPhy.
 * @param txPhy   Bit field of Host preferred Tx PHY. See @ref PHY_2_CODED
 * @param rxPhy   Bit field of Host preferred Rx PHY. See @ref PHY_2_CODED
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_SetDefaultPhyCmd( uint8 allPhys,
                                            uint8 txPhy,
                                            uint8 rxPhy );


/**
 * Request a change to the transmitter and receiver PHY for a connection.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_SET_PHY
 *
 * @param connHandle Connection handle.
 * @param allPhys    Host preference on how to handle txPhy and rxPhy.
 * @param txPhy      Bit field of Host preferred Tx PHY.
 * @param rxPhy      Bit field of Host preferred Rx PHY.
 * @param phyOpts    Bit field of Host preferred PHY options.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_SetPhyCmd( uint16 connHandle,
                                     uint8  allPhys,
                                     uint8  txPhy,
                                     uint8  rxPhy,
                                     uint16 phyOpts );


/**
 * Start Enhanced RX Test
 *
 * Start a test where the DUT receives reference packets at a fixed interval.
 * The tester generates the test reference packets.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_ENHANCED_RECEIVER_TEST
 *
 * @param rxChan Rx Channel k=0..39, where F=2402+(k*2MHz).
 * @param rxPhy Rx PHY to use. See @ref PHY_2_CODED
 * @param modIndex @ref DTM_enhanced
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_EnhancedRxTestCmd( uint8 rxChan,
                                             uint8 rxPhy,
                                             uint8 modIndex );


/**
 * Start Enhanced TX Test
 *
 * Start a test where the DUT generates test reference packets at a fixed
 * interval. The Controller shall transmit at maximum power.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_ENHANCED_TRANSMITTER_TEST
 *
 * @param txChan Tx RF channel k=0..39, where F=2402+(k*2MHz).
 * @param payloadLen Byte length (0..37) in payload for each packet.
 * @param payloadType @ref DTM_params
 * @param txPhy Tx PHY to use. See @ref PHY_2_CODED
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_EnhancedTxTestCmd( uint8 txChan,
                                             uint8 payloadLen,
                                             uint8 payloadType,
                                             uint8 txPhy );
/**
 * Start Enhanced CTE RX Test
 *
 * Start a test where the DUT receives reference packets at a fixed interval.
 * The tester generates the test reference packets.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode HCI_LE_ENHANCED_CTE_RECEIVER_TEST
 *
 * @param rxChan Rx Channel k=0..39, where F=2402+(k*2MHz).
 * @param rxPhy Rx PHY to use. See @ref PHY_2_CODED
 * @param modIndex @ref DTM_enhanced
 * @param expectedCteLength - Expected CTE length in 8 &mu;s units.
 * @param expectedCteType - Expected CTE type as bitmask (bit 0 - Allow AoA CTE Response).
 * @param slotDurations - Switching and sampling slots in 1 us or 2 us each (1 or 2).
 * @param length - The number of Antenna IDs in the pattern (2 to 75).
 * @param pAntenna - List of Antenna IDs in the pattern.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_EnhancedCteRxTestCmd( uint8 rxChan,
                                                uint8 rxPhy,
                                                uint8 modIndex,
                                                uint8 expectedCteLength,
                                                uint8 expectedCteType,
                                                uint8 slotDurations,
                                                uint8 length,
                                                uint8 *pAntenna);

/**
 * Start Enhanced CTE TX Test
 *
 * Start a test where the DUT generates test reference packets at a fixed
 * interval. The Controller shall transmit at maximum power.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode HCI_LE_ENHANCED_CTE_TRANSMITTER_TEST
 *
 * @param txChan Tx RF channel k=0..39, where F=2402+(k*2MHz).
 * @param payloadLen Byte length (0..37) in payload for each packet.
 * @param payloadType @ref DTM_params
 * @param txPhy Tx PHY to use. See @ref PHY_2_CODED
 * @param cteLength - CTE length in 8 &mu;s units.
 * @param cteType - CTE type as bitmask (bit 0 - Allow AoA CTE Response).
 * @param length - The number of Antenna IDs in the pattern (2 to 75).
 * @param pAntenna - List of Antenna IDs in the pattern.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_EnhancedCteTxTestCmd( uint8 txChan,
                                                uint8 payloadLen,
                                                uint8 payloadType,
                                                uint8 txPhy,
                                                uint8 cteLength,
                                                uint8 cteType,
                                                uint8 length,
                                                uint8 *pAntenna);
/**
 * Read the minimum and maximum supported Tx Power.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_READ_TX_POWER
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_ReadTxPowerCmd( void );

/**
 * Read the Tx and Rx RF Path Compensation values.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_READ_RF_PATH_COMPENSATION
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_ReadRfPathCompCmd( void );

/**
 * Write the Tx and Rx RF Path Compensation values.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_LE_WRITE_RF_PATH_COMPENSATION
 *
 * @param txPathParam Tx path compensation parameter (in 0.1 dBm).
 * @param rxPathParam Rx path compensation parameter (in 0.1 dBm).
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_WriteRfPathCompCmd( int16 txPathParam,
                                              int16 rxPathParam );

/**
 * Used to enable or disable sampling received Constant Tone Extension fields on a
 * connection and to set the antenna switching pattern and switching and sampling slot 
 * durations to be used.
 *
 * @design /ref did_202754181
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode HCI_LE_SET_CONNECTION_CTE_RECEIVE_PARAMS
 *
 * @param connHandle - Connection handle.
 * @param samplingEnable - Sample CTE on a connection and report the samples to the Host (0 or 1).
 * @param slotDurations - Switching and sampling slots in 1 us or 2 us each (1 or 2).
 * @param length - The number of Antenna IDs in the pattern (2 to 75).
 * @param pAntenna - List of Antenna IDs in the pattern.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_SetConnectionCteReceiveParamsCmd( uint16 connHandle,
                                                            uint8 samplingEnable,
                                                            uint8 slotDurations,
                                                            uint8 length,
                                                            uint8 *pAntenna);


/**
 * Set the antenna switching pattern and permitted Constant Tone Extension types used 
 * for transmitting Constant Tone Extensions requested by the peer device on a connection.
 *
 * @design /ref did_202754181
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode HCI_LE_SET_CONNECTION_CTE_TRANSMIT_PARAMS
 *
 * @param connHandle - Connection handle.
 * @param types - CTE types as bitmask (bit 0 - Allow AoA CTE Response).
 * @param length - The number of Antenna IDs in the pattern (2 to 75).
 * @param pAntenna - List of Antenna IDs in the pattern.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_SetConnectionCteTransmitParamsCmd( uint16 connHandle,
                                                             uint8 types,
                                                             uint8 length,
                                                             uint8 *pAntenna);

/**
 * Start or stop initiating the CTE Request procedure on a connection.
 *
 * @design /ref did_202754181
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode HCI_LE_SET_CONNECTION_CTE_REQUEST_ENABLE
 *
 * @param connHandle - Connection handle.
 * @param enable - Enable or disable CTE Request for a connection (1 or 0).
 * @param interval - Requested interval for initiating the CTE Request procedure in number of connection events (1 to 0xFFFF)
 * @param length - Min length of the CTE being requested in 8 us units (2 to 20).
 * @param type - Requested CTE type (0 - AoA, 1 - AoD with 1us slots, 2 - AoD with 2us slots).
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_SetConnectionCteRequestEnableCmd( uint16 connHandle,
                                                            uint8 enable,
                                                            uint16 interval,
                                                            uint8 length,
                                                            uint8 type);

/**
 * Set a respond to LL_CTE_REQ PDUs with LL_CTE_RSP PDUs on a connection.
 *
 * @design /ref did_202754181
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode HCI_LE_SET_CONNECTION_CTE_RESPONSE_ENABLE
 *
 * @param connHandle - Connection handle.
 * @param enable - Enable or disable CTE Response for a connection (1 or 0).
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_SetConnectionCteResponseEnableCmd( uint16 connHandle,
                                                             uint8 enable);

/**
 * Read the CTE antenna information.
 *
 * @design /ref did_202754181
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode HCI_LE_READ_ANTENNA_INFORMATION
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_LE_ReadAntennaInformationCmd( void );

/**
 * Used by the Host to set the advertiser parameters for periodic advertising
 *
 * @design /ref did_286039104
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode HCI_LE_SET_PERIODIC_ADV_PARAMETERS
 *
 * @param advHandle – Used to identify a periodic advertising train
 *                  Created by LE Set Extended Advertising Parameters command
 * @param periodicAdvIntervalMin – Minimum advertising interval for periodic advertising
 *                  Range: 0x0006 to 0xFFFF Time = N * 1.25 ms Time Range: 7.5ms to 81.91875 s
 * @param periodicAdvIntervalMax – Maximum advertising interval for periodic advertising
 *                  Range: 0x0006 to 0xFFFF Time = N * 1.25 ms Time Range: 7.5ms to 81.91875 s
 * @param periodicAdvProp – Periodic advertising properties
 *                  Set bit 6 for include TxPower in the advertising PDU
 *
 * @return  HCI status
 */
extern hciStatus_t HCI_LE_SetPeriodicAdvParamsCmd( uint8 advHandle,
                                                   uint16 periodicAdvIntervalMin,
                                                   uint16 periodicAdvIntervalMax,
                                                   uint16 periodicAdvProp );


/**
 *
 * @brief   Used to set the advertiser data used in periodic advertising PDUs.
 *          This command may be issued at any time after the advertising set identified by
 *          the Advertising_Handle parameter has been configured for periodic advertising
 *          using the HCI_LE_Set_Periodic_Advertising_Parameters command
 *
 * @design /ref did_286039104
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode HCI_LE_SET_PERIODIC_ADV_DATA
 *
 * @param   advHandle  – Used to identify a periodic advertising train
 * @param   operation  – 0x00 - Intermediate fragment of fragmented periodic advertising data
 *                       0x01 - First fragment of fragmented periodic advertising data
 *                       0x02 - Last fragment of fragmented periodic advertising data
 *                       0x03 - Complete periodic advertising data
 * @param   dataLength – The number of bytes in the Advertising Data parameter
 * @param   data       – Periodic advertising data
 *
 * @return  HCI status
 */
extern hciStatus_t HCI_LE_SetPeriodicAdvDataCmd( uint8 advHandle,
                                                 uint8 operation,
                                                 uint8 dataLength,
                                                 uint8 *data );

/**
 * @brief   Used to request the advertiser to enable or disable
 *          the periodic advertising for the advertising set
 *
 * @design /ref did_286039104
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode HCI_LE_SET_PERIODIC_ADV_ENABLE
 *
 * @param   enable    – 0x00 - Periodic advertising is disabled (default)
 *                      0x01 - Periodic advertising is enabled
 * @param   advHandle – Used to identify a periodic advertising train
 *
 * @return  HCI status
 */
extern hciStatus_t HCI_LE_SetPeriodicAdvEnableCmd( uint8 enable,
                                                   uint8 advHandle );

/**
 * @brief   Used by the Host to set the type, length, and antenna switching pattern
 *          for the transmission of Constant Tone Extensions in any periodic advertising.
 *
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode HCI_LE_SET_CONNECTIONLESS_CTE_TRANSMIT_PARAMS
 *
 * @param   advHandle – Used to identify a periodic advertising train
 * @param   cteLen    – CTE length (0x02 - 0x14) 16 usec - 160 usec
 * @param   cteType   – CTE type (0 - AoA, 1 - AoD 1usec, 2 - AoD 2usec)
 * @param   cteCount  – Number of CTE's to transmit in the same periodic event
 * @param   length    – Number of items in Antenna array (relevant to AoD only)
 * @param   pAntenna  – Pointer to Antenna array (relevant to AoD only)
 *
 * @return  HCI status
 */
extern hciStatus_t HCI_LE_SetConnectionlessCteTransmitParamsCmd( uint8 advHandle,
                                                                 uint8 cteLen,
                                                                 uint8 cteType,
                                                                 uint8 cteCount,
                                                                 uint8 length,
                                                                 uint8 *pAntenna);

/**
 * @brief   Used by the Host to request that the Controller enables or disables
 *          the use of Constant Tone Extensions in any periodic advertising.
 *
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode HCI_LE_SET_CONNECTIONLESS_CTE_TRANSMIT_ENABLE
 *
 * @param   advHandle – Used to identify a periodic advertising train
 * @param   enable    – 0x00 - Advertising with CTE is disabled (default)
 *                      0x01 - Advertising with CTE is enabled
 *
 * @return  HCI status
 */
extern hciStatus_t HCI_LE_SetConnectionlessCteTransmitEnableCmd( uint8 advHandle,
                                                                 uint8 enable );

/**
 * HCI_LE_PeriodicAdvCreateSyncCmd
 *
 * Used a scanner to synchronize with a periodic advertising train from
 * an advertiser and begin receiving periodic advertising packets.
 *
 * @design /ref did_286039104
 *
 * @param   options     – Clear Bit 0 - Use the advSID, advAddrType, and advAddress
 *                                      parameters to determine which advertiser to listen to.
 *                        Set Bit 0   - Use the Periodic Advertiser List to determine which
 *                                      advertiser to listen to.
 *                        Clear Bit 1 - Reporting initially enabled.
 *                        Set Bit 1   - Reporting initially disabled.
 * @param   advSID      – Advertising SID subfield in the ADI field used to identify
 *                        the Periodic Advertising (Range: 0x00 to 0x0F)
 * @param   advAddrType – Advertiser address type - 0x00 - public ; 0x01 - random
 * @param   advAddress  – Advertiser address (6 bytes)
 * @param   skip        – The maximum number of periodic advertising events that can be
 *                        skipped after a successful receive (Range: 0x0000 to 0x01F3)
 * @param   syncTimeout – Synchronization timeout for the periodic advertising train
 *                           Range: 0x000A to 0x4000 Time = N*10 ms Time Range: 100 ms to 163.84 s
 * @param   syncCteType – Set Bit 0 - Do not sync to packets with an AoA CTE
 *                        Set Bit 1 - Do not sync to packets with an AoD CTE with 1 us slots
 *                        Set Bit 2 - Do not sync to packets with an AoD CTE with 2 us slots
 *                        Set Bit 4 - Do not sync to packets without a CTE
 *
 * @return  HCI_Success
 */
extern hciStatus_t HCI_LE_PeriodicAdvCreateSyncCmd( uint8  options,
                                                    uint8  advSID,
                                                    uint8  advAddrType,
                                                    uint8  *advAddress,
                                                    uint16 skip,
                                                    uint16 syncTimeout,
                                                    uint8  syncCteType );

/**
 * HCI_LE_PeriodicAdvCreateSyncCancelCmd
 *
 * Used a scanner to cancel the HCI_LE_Periodic_Advertising_Create_Sync
 * command while it is pending.
 *
 * @design  /ref did_286039104
 *
 * @return  HCI status
 */
extern hciStatus_t HCI_LE_PeriodicAdvCreateSyncCancelCmd( void );

/**
 * HCI_LE_PeriodicAdvTerminateSyncCmd
 *
 * Used a scanner to stop reception of the periodic advertising
 * train identified by the syncHandle parameter.
 *
 * @design  /ref did_286039104
 *
 * @param   syncHandle - Handle identifying the periodic advertising train
 *                       (Range: 0x0000 to 0x0EFF)
 *                       The handle was assigned by the Controller while generating
 *                       the LE Periodic Advertising Sync Established event
 *
 * @return  HCI status
 */
extern hciStatus_t HCI_LE_PeriodicAdvTerminateSyncCmd( uint16 syncHandle );

/**
 * HCI_LE_AddDeviceToPeriodicAdvertiserListCmd
 *
 * Used a scanner to add an entry, consisting of a single device address
 * and SID, to the Periodic Advertiser list stored in the Controller.
 *
 * @design  /ref did_286039104
 *
 * @param   advAddrType – Advertiser address type - 0x00 - Public or Public Identity Address
 *                                                  0x01 - Random or Random (static) Identity Address
 * @param   advAddress  – Advertiser address (6 bytes)
 * @param   advSID      – Advertising SID subfield in the ADI field used to identify
 *                        the Periodic Advertising (Range: 0x00 to 0x0F)
 *
 * @return  HCI status
 */
extern hciStatus_t HCI_LE_AddDeviceToPeriodicAdvListCmd( uint8 advAddrType,
                                                         uint8 *advAddress,
                                                         uint8 advSID );

/**
 * HCI_LE_RemoveDeviceFromPeriodicAdvListCmd
 *
 * Used a scanner to remove one entry from the list of Periodic Advertisers
 * stored in the Controller.
 *
 * @design  /ref did_286039104
 *
 * @param   advAddrType – Advertiser address type -
 *                        0x00 - Public or Public Identity Address
 *                        0x01 - Random or Random (static) Identity Address
 * @param   advAddress  – Advertiser address (6 bytes)
 * @param   advSID      – Advertising SID subfield in the ADI field used to identify
 *                        the Periodic Advertising (Range: 0x00 to 0x0F)
 *
 * @return  HCI status
 */
extern hciStatus_t HCI_LE_RemoveDeviceFromPeriodicAdvListCmd( uint8 advAddrType,
                                                              uint8 *advAddress,
                                                              uint8 advSID );

/**
 * HCI_LE_ClearPeriodicAdvListCmd
 *
 * Used a scanner to remove all entries from the list of Periodic
 * Advertisers in the Controller.
 *
 * @design  /ref did_286039104
 *
 * @return  HCI status
 */
extern hciStatus_t HCI_LE_ClearPeriodicAdvListCmd( void );

/**
 * HCI_LE_ReadPeriodicAdvListSizeCmd
 *
 * Used a scanner to read the total number of Periodic Advertiser
 * list entries that can be stored in the Controller.
 *
 * @design  /ref did_286039104
 *
 * @return  HCI status
 *          Periodic Advertiser List Size (Range: 0x01 to 0xFF)
 */
extern hciStatus_t HCI_LE_ReadPeriodicAdvListSizeCmd( void );

/**
 * HCI_LE_SetPeriodicAdvReceiveEnableCmd
 *
 * Used a scanner to enable or disable reports for the periodic
 * advertising train identified by the syncHandle parameter.
 *
 * @design  /ref did_286039104
 *
 * @param   syncHandle - Handle identifying the periodic advertising train
 *                       (Range: 0x0000 to 0x0EFF)
 *                       The handle was assigned by the Controller while generating
 *                       the LE Periodic Advertising Sync Established event
 * @param   enable     - 0x00 - Reporting disable
 *                       0x01 - Reporting enable
 *
 * @return  HCI status
 */
extern hciStatus_t HCI_LE_SetPeriodicAdvReceiveEnableCmd( uint16 syncHandle,
                                                          uint8  enable );

/**
 * HCI_LE_SetConnectionlessIqSamplingEnableCmd
 *
 * Used by the Host to request that the Controller enables or disables capturing
 * IQ samples from the CTE of periodic advertising packets in the periodic
 * advertising train identified by the syncHandle parameter.
 *
 * @param   syncHandle - Handle identifying the periodic advertising train (Range: 0x0000 to 0x0EFF)
 * @param   samplingEnable - Sample CTE on a received periodic advertising and report the samples to the Host.
 * @param   slotDurations - Switching and sampling slots in 1 us or 2 us each (1 or 2).
 * @param   maxSampledCtes – 0 - Sample and report all available CTEs
 *                           1 to 16 - Max number of CTEs to sample and report in each periodic event
 * @param   length    – Number of items in Antenna array (relevant to AoA only)
 * @param   pAntenna  – Pointer to Antenna array (relevant to AoA only)
 *
 * @return  HCI status
 */
extern hciStatus_t HCI_LE_SetConnectionlessIqSamplingEnableCmd( uint16 syncHandle,
                                                                uint8 samplingEnable,
                                                                uint8 slotDurations,
                                                                uint8 maxSampledCtes,
                                                                uint8 length,
                                                                uint8 *pAntenna);

/*
** HCI Vendor Specific Commands: Link Layer Extensions
*/

/**
 * Set the receiver gain.
 *
 * The default system value for this feature is @ref HCI_EXT_RX_GAIN_STD
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_SET_RX_GAIN
 *
 * @param rxGain @ref RX_Gain
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_SetRxGainCmd( uint8 rxGain );


/**
 * Set the transmit power.
 *
 * The 'txPower' input parameter should be an index from @ref TX_Power_Index.
 *
 * The default system value for this feature is @ref HCI_EXT_TX_POWER_0_DBM
 *
 * @note For the CC254x platform, a setting of 4dBm is only allowed for the
 * CC2540.
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_SET_TX_POWER
 *
 * @param txPower @ref TX_Power_Index
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_SetTxPowerCmd( uint8 txPower );


/**
 * Set whether a connection will be limited to one packet per event.
 *
 * Note: When set, all connections will be restricted to one packet per event.
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_ONE_PKT_PER_EVT
 *
 * @param control @ref One_Pkt
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_OnePktPerEvtCmd( uint8 control );

/// @cond CC254X

/**
 * Set whether the system clock will be divided when the MCU is halted.
 *
 * The default system value for this feature is
 * @ref HCI_EXT_DISABLE_ONE_PKT_PER_EVT. This command can be used to trade-off
 * throughput and power consumption during a connection. When enabled, power can
 * be conserved during a connection by limiting the number of packets per
 * connection event to one, at the expense of more limited throughput. When
 * disabled, the number of packets transferred during a connection event is
 * not limited, at the expense of higher power consumption.
 *
 * @par Corresponding Events:
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_CLK_DIVIDE_ON_HALT
 *
 * @param control @ref Clk_Divide
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_ClkDivOnHaltCmd( uint8 control );

/**
 * Declare NV Usage
 *
 * Indicate to the Controller whether or not the Host will be using the NV
 * memory during BLE operations. The default system value for this feature is
 * @ref HCI_EXT_NV_IN_USE. When the NV is not in use during BLE operations, the
 * Controller is able to bypass internal checks that reduce overhead processing,
 * thereby reducing average power consumption.
 *
 * @note This command is only allowed when the BLE Controller is idle.
 *
 * @warning Using NV when declaring it is not in use may result in a hung
 * BLE Connection.
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_DECLARE_NV_USAGE
 *
 * @param mode @ref NV_Use
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_DeclareNvUsageCmd( uint8 mode );

/// @endcond //CC254X

/**
 * Decrypt encrypted data using AES128.
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_DECRYPT
 *
 * @param key Pointer to 16 byte encryption key. The most significant octet of
 *        the data corresponds to key[0] using the notation specified in FIPS 197.
 * @param encText Pointer to 16 byte encrypted data. The most significant octet of
 *        the key corresponds to key[0] using the notation specified in FIPS 197.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_DecryptCmd( uint8 *key,
                                       uint8 *encText );

/**
 * Write this device's supported features.
 *
 * This command is used to set the Controller's Local Supported Features. For a
 * complete list of supported LE features, see the BT Spec.
 *
 * @note This command can be issued either before or after one or more
 * connections are formed. However, the local features set in this manner are
 * only effective if performed before a Feature Exchange Procedure has been
 * initiated by the Master. Once this control procedure has been completed for a
 * particular connection, only the exchanged feature set for that connection
 * will be used. Since the Link Layer may initiate the feature exchange
 * procedure autonomously, it is best to use this command before the connection
 * is formed.
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_SET_LOCAL_SUPPORTED_FEATURES
 *
 * @param localFeatures Pointer to eight bytes of local features.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_SetLocalSupportedFeaturesCmd( uint8 *localFeatures );

/**
 * Set Fast TX Response Time
 *
 * This command is used to configure the Link Layer fast transmit response time
 * feature. The default system value for this feature is
 * @ref HCI_EXT_ENABLE_FAST_TX_RESP_TIME. When the Host transmits data, the
 * controller (by default) ensures the packet is sent over the LL connection
 * with as little delay as possible, even when the connection is configured to
 * use slave latency. That is, the transmit response time will tend to be no
 * longer than the connection interval. This results in lower power savings
 * since the LL may need to wake to transmit during connection events that would
 * normally have been skipped. If saving power is more critical than fast
 * transmit response time, then this feature can be disabled using this command.
 * When disabled, the transmit response time will be no longer than slave
 * latency + 1 times the connection interval.
 *
 * @note This command is only valid for a Slave controller.
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_SET_FAST_TX_RESP_TIME
 *
 * @param control @ref Fast_TX
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_SetFastTxResponseTimeCmd( uint8 control );


/**
 * Enable or disable suspending slave latency.
 *
 * This command is used to enable or disable the Slave Latency Override,
 * allowing the user to ensure that Slave Latency is not applied even though it
 * is active. The default value is @ref HCI_EXT_DISABLE_SL_OVERRIDE
 *
 * @note This command will be disallowed if there are no active connection in
 * the slave role.
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_OVERRIDE_SL
 *
 * @param control @ref SL_Override
 *
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_SetSlaveLatencyOverrideCmd( uint8 control );


/**
 * Start Modem TX Test
 *
 * Start a continuous transmitter modem test, using either a modulated or
 * unmodulated carrier wave tone, at the frequency that corresponds to the
 * specified RF channel. Use @ref HCI_EXT_EndModemTestCmd command to end the
 * test.
 *
 * The BLE device will transmit at maximum power.
 *
 * This modem test can be used to satisfy in part radio regulation
 * requirements as specific in standards such as ARIB STD-T66.
 *
 * @note A Controller reset will be issued by HCI_EXT_EndModemTestCmd.
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_MODEM_TEST_TX
 *
 * @param cwMode @ref TX_Modulation
 * @param txChan Transmit RF channel k=0..39, where BLE F=2402+(k*2MHz).
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_ModemTestTxCmd( uint8 cwMode,
                                           uint8 txChan );

/**
 * Start Modem Hop Tx Test
 *
 * Start a continuous transmitter direct test mode test using a modulated carrier
 * wave and transmitting a 37 byte packet of Pseudo-Random 9-bit data. A packet
 * is transmitted on a different frequency (linearly stepping through all RF
 * channels 0..39) every 625 us. Use @ref HCI_EXT_EndModemTestCmd command to end
 * the test.
 *
 * The BLE device will transmit at maximum power.
 *
 * This modem test can be used to satisfy in part radio regulation
 * requirements as specific in standards such as ARIB STD-T66.
 *
 * @note A Controller reset will be issued by HCI_EXT_EndModemTestCmd.
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_MODEM_HOP_TEST_TX
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_ModemHopTestTxCmd( void );

/**
 * Start Modem Hop RX Test
 *
 * Start a continuous receiver modem test using a modulated carrier wave tone,
 * at the frequency that corresponds to the specific RF channel. Any received
 * data is discarded. Receiver gain may be adjusted using the
 * @ref HCI_EXT_SetRxGainCmd. RSSI may be read during this test by using the
 * @ref HCI_ReadRssiCmd. Use @ref HCI_EXT_EndModemTestCmd command to end the
 * test.
 *
 * This modem test can be used to satisfy in part radio regulation
 * requirements as specific in standards such as ARIB STD-T66.
 *
 * @note A Controller reset will be issued by @ref HCI_EXT_EndModemTestCmd.
 *
 * @param rxChan Receiver RF channel k=0..39, where BLE F=2402+(k*2MHz).
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_MODEM_TEST_RX
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_ModemTestRxCmd( uint8 rxChan );

/**
 * Start Enhanced Modem TX Test
 *
 * Start an enhanced BLE5 continuous transmitter modem test, using either a
 * modulated or unmodulated carrier wave tone, at the frequency that corresponds
 * to the specified RF channel for a given PHY (1M, 2M, Coded S2, or Coded S8).
 * Use @ref HCI_EXT_EndModemTestCmd command to end the test.
 *
 * The BLE device will transmit at maximum power.
 *
 * This modem test can be used to satisfy in part radio regulation
 * requirements as specific in standards such as ARIB STD-T66.
 *
 * @note A Controller reset will be issued by HCI_EXT_EndModemTestCmd.
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_ENHANCED_MODEM_TEST_TX
 *
 * @param cwMode @ref TX_Modulation
 * @param rfPhy  @ref RF_Setup_Phy_Params
 * @param rfChan Transmit RF channel k=0..39, where BLE F=2402+(k*2MHz).
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_EnhancedModemTestTxCmd( uint8 cwMode,
                                                   uint8 rfPhy,
                                                   uint8 rfChan );

/**
 * Start Enhanced Modem Hop Tx Test
 *
 * Start an enhanced BLE5 continuous transmitter direct test mode test using a
 * modulated carrier wave. A test reference data packet is transmitted on a
 * different frequency (linearly stepping through all RF channels 0..39), for a
 * given PHY (1M, 2M, Coded S2, Coded S8), every period (depending on the
 * payload length, as given Vol. 6, Part F, section 4.1.6). Use
 * @ref HCI_EXT_EndModemTestCmd command to end the test.
 *
 * The BLE device will transmit at maximum power.
 *
 * This modem test can be used to satisfy in part radio regulation
 * requirements as specific in standards such as ARIB STD-T66.
 *
 * @note A Controller reset will be issued by HCI_EXT_EndModemTestCmd.
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode
 *      @ref HCI_EXT_ENHANCED_MODEM_HOP_TEST_TX
 *
 * @param payloadLen  Number bytes (0..255)in payload for each packet.
 * @param payloadType @ref DTM_params
 * @param rfPhy  @ref RF_Setup_Phy_Params
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_EnhancedModemHopTestTxCmd( uint8 payloadLen,
                                                      uint8 payloadType,
                                                      uint8 rfPhy );

/**
 * Start Enhanced Modem Hop RX Test
 *
 * Start an enhanced BLE5 continuous receiver modem test using a modulated
 * carrier wave tone, at the frequency that corresponds to the specific RF
 * channel, for a given PHY (1M, 2M, Coded S2, or Coded S8). Any received data
 * is discarded. RSSI may be read during this test by using the
 * @ref HCI_ReadRssiCmd. Use @ref HCI_EXT_EndModemTestCmd command to end the
 * test.
 *
 * This modem test can be used to satisfy in part radio regulation
 * requirements as specific in standards such as ARIB STD-T66.
 *
 * @note A Controller reset will be issued by @ref HCI_EXT_EndModemTestCmd.
 *
 * @param rfPhy  @ref RF_Setup_Phy_Params
 * @param rfChan Receiver RF channel k=0..39, where BLE F=2402+(k*2MHz).
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_ENHANCED_MODEM_TEST_RX
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_EnhancedModemTestRxCmd( uint8 rfPhy,
                                                   uint8 rfChan );

/**
 * Shutdown a modem test.
 *
 * @note A complete Controller reset will take place.
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_END_MODEM_TEST
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_EndModemTestCmd( void );

/**
 * Set this device's BLE address (BDADDR).
 *
 * This address will override the device's address determined when the device is
 * reset (i.e. a hardware reset, not an HCI Controller Reset). To restore the
 * device's initialized address, issue this command with an invalid address.
 *
 * @note This command is only allowed when the device's state is Standby.
 *
 * @warning This command is intended to only be used during initialization. Changing the
 * device's BDADDR after various BLE operations have already taken place may cause
 * unexpected problems.
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_SET_BDADDR
 *
 * @param bdAddr  A pointer to a buffer to hold this device's address.
 *        An invalid address (i.e. all FF's) will restore this device's address
 *        to the address set at initialization.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_SetBDADDRCmd( uint8 *bdAddr );

/**
 * Set this device's Sleep Clock Accuracy.
 *
 * For a slave device, this value is directly used, but only if power management
 * is enabled. For a master device, this value is converted into one of eight
 * ordinal values representing a SCA range, as specified in the BT Spec. For a
 * Slave device, the value is directly used. The system default value for a
 * Master and Slave device is 50ppm and 40ppm, respectively.
 *
 * @note This command is only allowed when the device is not in a connection.
 * The device's SCA value remains unaffected by a @ref HCI_ResetCmd
 *
 * @note For AE, each defined Advertisement Set, and any subsequent Advertisement
 * Set created by calling @ref GapAdv_create, will use the same SCA value. As
 * such, once a connection if formed, the value for the SCA could be changed
 * using this API, thereby allowing subsequent connections to form with different
 * SCA values, if so desired.
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_SET_SCA
 *
 * @param scaInPPM A SCA value in PPM from 0..500.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_SetSCACmd( uint16 scaInPPM );

/**
 * Enable Production Test Mode.
 *
 * This command is used to enable Production Test Mode (PTM). This mode is used
 * by the customer during assembly of their product to allow limited access to
 * the BLE Controller for testing and configuration. This command is only
 * available when the BLE Controller is built without external access to the
 * Controller (i.e. when no transport interface such as RS232 is permitted).
 * This mode will remain enabled until the device is reset. Please see the
 * related [application note]
 * (http://processors.wiki.ti.com/index.php/PTM_cc2640 "Application Note") for
 * additional details.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_EnablePTMCmd( void );

/**
 * Set Frequency Tuning
 *
 * Set the frequency tuning up or down.When the current setting is already at
 * its max value, then stepping up will have no effect. When the current setting
 * is already at its min value, then stepping down will have no effect. This
 * setting will only remain in effect until the device is reset unless
 * @ref HCI_EXT_SaveFreqTuneCmd is used to save it in non-volatile memory.
 *
 * @note This is a Production Test Mode only command. See
 * @ref HCI_EXT_EnablePTMCmd.
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_SET_FREQ_TUNE
 *
 * @param step @ref Freq_Tune
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_SetFreqTuneCmd( uint8 step );

/**
 * Save the frequency tuning value to non-volatile memory.
 *
 * This setting will be used by the BLE Controller upon reset, and when waking
 * from Sleep.
 *
 * @note This is a Production Test Mode only command. See
 * @ref HCI_EXT_EnablePTMCmd
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_SET_FREQ_TUNE
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_SaveFreqTuneCmd( void );

/**
 * Set the maximum transmit output power for DTM.
 *
 * This command is used to override the RF transmitter output power used by the
 * Direct Test Mode (DTM). Normally, the maximum transmitter output power
 * setting used by DTM is the maximum transmitter output power setting for the
 * device (i.e. 4 dBm for the CC2540; 0 dBm for the CC2541; 5 dBm for the
 * CC264x). This command will change the value used by DTM.
 *
 * @note When DTM is ended by a call to @ref HCI_LE_TestEndCmd, or a
 * @ref HCI_ResetCmd is used, the transmitter output power setting is restored
 * to the default value of @ref HCI_EXT_TX_POWER_0_DBM
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_SET_MAX_DTM_TX_POWER
 *
 * @param txPower @ref TX_Power_Index
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_SetMaxDtmTxPowerCmd( uint8 txPower );

/// @cond CC254X

/**
 * Map PM to I/O Port
 *
 * Configure and map a CC254x I/O
 * Port as a General Purpose I/O (GPIO) output signal that reflects
 * the Power Management (PM) state of the CC254x device. The GPIO
 * output will be High on Wake, and Low upon entering Sleep. This
 * feature can be disabled by specifying HCI_EXT_PM_IO_PORT_NONE
 * for the ioPort (ioPin is then ignored). The system default value
 * upon hardware reset is disabled. This command can be used to
 * control an external DC-DC Converter (its actual intent) such has
 * the TI TPS62730 (or any similar converter that works the same
 * way). This command should be used with extreme care as it will
 * override how the Port/Pin was previously configured! This
 * includes the mapping of Port 0 pins to 32kHz clock output,
 * Analog I/O, UART, Timers; Port 1 pins to Observables, Digital
 * Regulator status, UART, Timers; Port 2 pins to an external 32kHz
 * XOSC. The selected Port/Pin will be configured as an output GPIO
 * with interrupts masked. Careless use can result in a
 * reconfiguration that could disrupt the system. It is therefore
 * the user's responsibility to ensure the selected Port/Pin does
 * not cause any conflicts in the system.
 *
 * @note
 * Only Pins 0, 3 and 4 are valid for Port 2 since Pins 1
 * and 2 are mapped to debugger signals DD and DC. <br>
 * Port/Pin signal change will only occur when Power Savings
 * is enabled.
 *
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_MAP_PM_IO_PORT
 *
 * @param ioPort @ref Map_PM
 *
 * @param ioPin  @ref Map_PM
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_MapPmIoPortCmd( uint8 ioPort, uint8 ioPin );

/// @endcond // CC254X

/**
 * Disconnect the connection immediately.
 *
 * This command is used to disconnect a connection immediately. This command can
 * be useful for when a connection needs to be ended without the latency
 * associated with the normal BLE Controller Terminate control procedure.
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_DISCONNECT_IMMED <br>
 * @ref hciEvt_DisconnComplete_t
 *
 * @param connHandle Connection handle.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_DisconnectImmedCmd( uint16 connHandle );

/**
 * Packet Error Rate Command
 *
 * Reset or Read the Packet Error Rate counters for a connection.
 *
 * @note The counters are only 16 bits. At the shortest connection interval,
 * this provides a bit over 8 minutes of data.
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_PER , followed by
 *      another <br>
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_PER
 *
 * @param connHandle The LL connection ID on which to send this data.
 * @param command @ref PER_Cmd
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_PacketErrorRateCmd( uint16 connHandle, uint8 command );

/**
 * Packet Error Rate by Channel Command
 *
 * This command is used to start or end Packet Error Rate by Channel counter
 * accumulation for a connection, and can be used by an application to make
 * Coexistence assessments. Based on the results, an application can perform an
 * Update Channel Classification command to limit channel interference from
 * other wireless standards.
 *
 * If perByChan is NULL, counter accumulation will be discontinued. If perByChan
 * is not NULL, then it is assumed that there is sufficient memory for the PER
 * data of type perByChan_t (defined in ll.h)
 *
 * @code{.c}
 * // Packet Error Rate Information By Channel
 * typedef struct
 * {
 *   uint16 numPkts[ LL_MAX_NUM_DATA_CHAN ];
 *   uint16 numCrcErr[ LL_MAX_NUM_DATA_CHAN ];
 * } perByChan_t;
 * @endcode
 *
 * @note
 * This command is only allowed as a direct function call, and is only intended
 * to be used by an embedded application. <br>
 * As indicated, the counters are 16 bits. At the shortest connection interval,
 * this provides a bit over 8 minutes of data. <br>
 * This command can be used in combination with @ref HCI_EXT_PacketErrorRateCmd.
 *
 * @warning It is the user's responsibility to ensure there is sufficient memory
 * allocated. The user is also responsible for maintaining the counters,
 * clearing them if required before starting accumulation.
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_PER_BY_CHAN
 *
 * @param connHandle The LL connection ID on which to send this data.
 * @param perByChan Pointer to PER by Channel data, or NULL.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_PERbyChanCmd( uint16 connHandle, perByChan_t *perByChan );

/// @cond CC254X

/**
 * Extend Rf Range using the TI CC2590 2.4 GHz RF Front End device.
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_EXTEND_RF_RANGE
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_ExtendRfRangeCmd( void );

/**
 * Halt During RF Command
 *
 * Enable or disable halting the CPU during RF. The system defaults to enabled.
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_HALT_DURING_RF
 *
 * @param mode @ref Halt_RF
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_HaltDuringRfCmd( uint8 mode );

/// @endcond // CC254X

/// @cond NODOC

/**
 * Advertising Event Notice Command
 *
 * Enable or disable a notification to the specified task using the specified
 * task event whenever an advertising event ends. A non-zero taskEvent value is
 * taken to be "enable", while a zero valued taskEvent is taken to be "disable".
 *
 * @warning This command does not check if the taskID and/or taskEvent are valid.
 *
 * @note This command does not return any HCI events but has a meaningful return
 * status.
 *
 * If configured successfully, it will result in BLE stack events sent as shown
 * in the following example
 *
 *
 * @par         Usage
 * 1. Define the event in the application
 * @code{.c}
 * // Application specific event ID for HCI Advertisement End Events
 * #define SBP_HCI_ADV_EVT_END_EVT              0x0001
 * @endcode
 * 2. Configure the Bluetooth low energy protocol stack to return the event
 * @code{.c}
 * HCI_EXT_AdvEventNoticeCmd( selfEntity, SBP_HCI_ADV_EVT_END_EVT);
 * @endcode
 * 3. Check for and receive these events in the application
 * @code{.c}
 * if (ICall_fetchServiceMsg(&src, &dest,
 *                           (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
 * {
 *   uint8 safeToDealloc = TRUE;
 *
 *   if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))
 *   {
 *     ICall_Stack_Event *pEvt = (ICall_Stack_Event *)pMsg;
 *
 *     // Check for BLE stack events first
 *     if (pEvt->signature == 0xffff)
 *     {
 *       if (pEvt->event_flag & SBP_HCI_ADV_EVT_END_EVT)
 *       {
 *         //Advertisement ended. Process as desired.
 *       }
 * @endcode
 *
 * @param taskID User's task ID.
 * @param taskEvent User's task event.
 *
 * @return @ref HCI_SUCCESS
 * @return @ref HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS : More than 1 bit was set
 *         in taskEvent
 */
extern hciStatus_t HCI_EXT_AdvEventNoticeCmd( uint8 taskID, uint16 taskEvent );

/// @endcond // NODOC

/**
 * Connection Event Notice Command
 *
 * For a given connection handle, enable or disable a notification to the
 * specified task using the specified task event whenever a Connection event
 * ends. A non-zero taskEvent value is taken to be "enable", while a zero valued
 * taskEvent taken to be "disable".
 *
 * @warning This command does not check if the taskID and/or taskEvent are valid
 *
 * @note This command does not return any HCI events but has a meaningful
 * return status.
 *
 * If configured successfully, it will result in BLE stack events sent as shown
 * in the following example.
 *
 * @par Usage
 * 1. Define the event in the application
 * @code{.c}
 * // Application specific event ID for HCI Connection Event End Events
 * #define SBP_HCI_CONN_EVT_END_EVT              0x0001
 * @endcode
 * 2. Configure the Bluetooth low energy protocol stack to return the event. This
 * must be done after the connection has been formed.
 * @code{.c}
 * HCI_EXT_ConnEventNoticeCmd(pMsg->connHandle, selfEntity, SBP_HCI_CONN_EVT_END_EVT)
 * @endcode
 * 3. Check for and receive these events in the application
 * @code{.c}
 * if (ICall_fetchServiceMsg(&src, &dest,
 *                           (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
 * {
 *   uint8 safeToDealloc = TRUE;
 *
 *   if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))
 *   {
 *     ICall_Stack_Event *pEvt = (ICall_Stack_Event *)pMsg;
 *
 *     // Check for BLE stack events first
 *     if (pEvt->signature == 0xffff)
 *     {
 *       if (pEvt->event_flag & SBP_HCI_CONN_EVT_END_EVT)
 *       {
 *         //Connection event ended. Process as desired.
 *       }
 * @endcode
 *
 * @param connHandle The HCI connection ID for connection event notice.
 * @param taskID User's task ID.
 * @param taskEvent User's task event.
 *
 * @return @ref HCI_SUCCESS
 * @return @ref HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS : >1 bit was set in taskEvent
 */
extern hciStatus_t HCI_EXT_ConnEventNoticeCmd( uint16 connHandle, uint8 taskID, uint16 taskEvent );

/**
 * Set a user revision number or read the build revision number.
 *
 * This command allows the embedded user code to set their own 16-bit revision
 * number or read the build revision number of the Bluetooth low energy stack
 * library software. The default value of the revision number is zero. When you
 * update a Bluetooth low energy project by adding your own code, use this API
 * to set your own revision number. When called with mode set to
 * @ref HCI_EXT_SET_USER_REVISION, the stack saves this value. No event is
 * returned from this API when used this way.
 *
 * @par Corresponding Events
 * @ref hciEvt_VSCmdComplete_t with cmdOpcode @ref HCI_EXT_BUILD_REVISION
 * (only when mode == @ref HCI_EXT_READ_BUILD_REVISION)
 *
 * @param mode @ref User_Rev
 *
 * @param userRevNum user defined revision number
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_BuildRevisionCmd( uint8 mode, uint16 userRevNum );

/// @cond CC254X

/**
 * Set the sleep delay.
 *
 * This command is used to set the delay before sleep occurs after Reset or upon
 * waking from sleep to allow the external 32kHz crystal to stabilize. If this
 * command is never used, the default delay is 400 ms on the CC254x. This
 * command is no longer supported on the CC264x platform. If the customer's
 * hardware requires a different delay or does not require this delay at all,
 * it can be changed by calling this command during their OSAL task
 * initialization. A zero delay value will eliminate the delay after Reset and
 * (unless changed again) all subsequent wakes from sleep; a non-zero delay
 * value will change the delay after Reset and (unless changed again) all
 * subsequent wakes from sleep. If this command is used any time after system
 * initialization, then the new delay value will be applied the next time the
 * delay is used.
 *
 * @note This delay only applies to Reset and Sleep. If a periodic timer is
 * used, or a BLE operation is active, then only Sleep is used, and this delay
 * will only occur after Reset.
 *
 * @note There is no distinction made between a hard and soft reset. The delay
 * (if non-zero) will be applied the same way in either case.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_EXT_DELAY_SLEEP
 *
 * @param delay 0..1000, in milliseconds.
 *
 * @return @ref HCI_SUCCESS : successfully set or read has started
 * @return @ref HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS : invalid parameter
 */
extern hciStatus_t HCI_EXT_DelaySleepCmd( uint16 delay );

/// @endcond // CC254X

/**
 * Issue a soft or hard system reset.
 *
 * This command is used to issue a hard or soft system reset. On the CC254x, a
 * hard reset is caused by a watchdog timer timeout, while a soft reset is
 * caused by resetting the PC to zero. On the CC264x, a hard reset is caused by
 * setting the SYSRESET bit in the System Controller Reset Control register.
 *
 * @note
 * The soft reset is currently not supported on the CC264x.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_EXT_RESET_SYSTEM
 *
 * @param mode @ref Reset_Type
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_ResetSystemCmd( uint8 mode );

// @cond NODOC

/**
 * Send a LL Test Mode test case.
 *
 * @param testCase See list of defines in ll_common.h.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_EXT_LL_TEST_MODE
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_LLTestModeCmd( uint8 testCase );


/**
 * Enable or disable overlapped processing.
 *
 * @param mode @ref Overlapped
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_EXT_OVERLAPPED_PROCESSING
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_OverlappedProcessingCmd( uint8 mode );

/// @endcond // NODOC

/**
 * Set the Number of Completed Packets Limit
 *
 * This command is used to set the limit on the minimum number of complete
 * packets before a Number of Completed Packets event is returned by the
 * Controller. If the limit is not reached by the end of a connection event,
 * then the @ref hciEvt_NumCompletedPkt_t event will be returned (if non-zero)
 * based on the flushOnEvt flag. The limit can be set from one to
 * @ref HCI_MAX_NUM_CMD_BUFFERS (see  @ref HCI_LE_ReadBufSizeCmd). The default
 * limit is 1; the default flushOnEvt flag is FALSE.
 *
 * @par Corresponding Events:
 * @ref hciEvt_NumCompletedPkt_t <br>
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_EXT_NUM_COMPLETED_PKTS_LIMIT
 *
 * @param limit From 1 to @ref HCI_MAX_NUM_CMD_BUFFERS.
 * @param flushOnEvt @ref Compl_Pkts
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_NumComplPktsLimitCmd( uint8 limit,
                                                 uint8 flushOnEvt );

/**
 * Get Connection Info
 *
 * This API is used to get connection related information, which includes the
 * number of allocated connections, the number of active connections, and for
 * each active connection, the connection ID, the connection role (Master or
 * Slave), the peer address and peer address type. The number of allocated
 * connections is based on a default build value that can be changed using
 * MAX_NUM_BLE_CONNS. The number of active connections refers to active BLE connections.
 *
 * @note If all the parameters are NULL, then the command is assumed to have
 * originated from the transport layer. Otherwise, they are assumed to have
 * originated from a direct call by the Application and any non-NULL pointer
 * will be directly used.
 *
 * @warning If either parameter is non-NULL, the pointer will be used to store
 * the result. It is the user's responsibility to ensure there is sufficient
 * memory allocated.
 *
 * @par Corresponding Events:
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_EXT_GET_CONNECTION_INFO
 *
 * @param numAllocConns  Pointer for number of build time connections.
 * @param numActiveConns Pointer for number of active BLE connections.
 * @param activeConnInfo Pointer for active connection information.
 *
 * @param numAllocConns  Number of build time connections allowed.
 * @param numActiveConns Number of active BLE connections.
 * @param activeConnInfo Active connection information.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_GetConnInfoCmd( uint8         *numAllocConns,
                                           uint8         *numActiveConns,
                                           hciConnInfo_t *activeConnInfo );

/**
 * Set the maximum Rx and Tx Octets (in bytes) and Time (in us).
 *
 * @note Any parameter not within a valid range will leave the corresponding
 * supportedMax value unchanged.
 *
 * @par Corresponding Events:
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_EXT_SET_MAX_DATA_LENGTH
 *
 * @param txOctets Value of maximum supported Tx octets (in bytes).
 * @param txTime   Value of maximum supported Tx time (in us).
 * @param rxOctets Value of maximum supported Rx octets (in bytes).
 * @param rxTime   Value of maximum supported Rx time (in us).
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_SetMaxDataLenCmd( uint16 txOctets,
                                             uint16 txTime,
                                             uint16 rxOctets,
                                             uint16 rxTime );

/**
 * Set the DTM transmit packet count.
 *
 * The default system value for this feature is @ref DTM_TxPktCnt.
 *
 * @par Corresponding Events
 * @ref  hciEvt_VSCmdComplete_t
 *
 * @param txPktCnt  Number of DTM packets to transmit.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_SetDtmTxPktCntCmd( uint16 txPktCnt );


/**
 * Read this device's BLE Random Address.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_EXT_READ_RAND_ADDR
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_ReadRandAddrCmd( void );


/**
 * Set pin as output and initialize it.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_EXT_SET_PIN_OUTPUT
 *
 * @param dio - pin number.
 * @param value - initialize pin value (0 or 1). 0xFF will return the pin to input.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_SetPinOutputCmd( uint8 dio,
                                            uint8 value );

/**
 * Set CTE accuracy for 1M and 2M PHY
 *
 * @design  /ref did_202754181
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_EXT_SET_LOCATIONING_ACCURACY
 *
 * @param handle - Connection handle (0x0XXX) or periodic advertising train handle (0x1XXX).
 * @param sampleRate1M sample rate for PHY 1M
 *        range : 1 - least accuracy (as in 5.1 spec) to 4 - most accuracy
 * @param sampleSize1M sample size for PHY 1M
 *        range : 1 - 8 bits (as in 5.1 spec) or 2 - 16 bits (more accurate)
 * @param sampleRate2M sample rate for PHY 2M
 *        range : 1 - least accuracy (as in 5.1 spec) to 4 - most accuracy
 * @param sampleSize2M sample size for PHY 2M
 *        range : 1 - 8 bits (as in 5.1 spec) or 2 - 16 bits (more accurate)
 * @param sampleCtrl - sample control flags
 *        range : range : bit0=0 - Default filtering, bit0=1 - RAW_RF(no filtering), , bit1..7=0 - spare
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_SetLocationingAccuracyCmd( uint16 handle,
                                                      uint8  sampleRate1M,
                                                      uint8  sampleSize1M,
                                                      uint8  sampleRate2M,
                                                      uint8  sampleSize2M,
                                                      uint8  sampleCtrl);

/**
 * @brief       This API is used to get connection related information required
 *              to follow the target connection with a BLE conneciton monitor.
 *
 * input parameters
 *
 * @param       connId         - Connection id of an active BLE connection.
 * @param       activeConnInfo - Pointer for active connection information that
 *                               is needed to track BLE connection. Note: If not
 *                               NULL it is the users responsibility to allocate
 *                               memory for this pointer based on
 *                               sizeof(hciActiveConnInfo_t) and also to free it
 *                               when done.
 *
 * output parameters
 *
 * @param       activeConnInfo - Pointer for active connection information that
 *                               is needed to track BLE connection.
 *
 * @return      LL_STATUS_SUCCESS
 */
extern hciStatus_t HCI_EXT_GetActiveConnInfoCmd( uint8 connId, hciActiveConnInfo_t *activeConnInfo );


/**
 *
 * @brief       This API is used to set the advertiser's virtual public address.
 *
 *              Note: This command is only allowed when the advertise set is not active,
 *                    and its PDU type is Legacy Non-Connectable and Non-Scanable.
 *
 * @design /ref 239346186
 *
 * input parameters
 *
 * @param       handle - advertising handle
 *
 * @param       bdAddr - the address which will be assigned to the handle
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      LL_STATUS_SUCCESS
 */
extern hciStatus_t HCI_EXT_SetVirtualAdvAddrCmd( uint8 advHandle,  uint8 *bdAddr );

/**
 * @brief       This API is used to set the scan channels mapping
 *
 * input parameters
 *
 * @param       extScanChannelsMap - channel scan definitions.
 * Note:        The parameter can receive the values -
 *              LL_SCN_ADV_MAP_CHAN_37,
 *              LL_SCN_ADV_MAP_CHAN_38,
 *              LL_SCN_ADV_MAP_CHAN_39,
 *              LL_SCN_ADV_MAP_CHAN_37_38,
 *              LL_SCN_ADV_MAP_CHAN_37_39,
 *              LL_SCN_ADV_MAP_CHAN_38_39,
 *              LL_SCN_ADV_MAP_CHAN_ALL.
 *
 *              @ref GAP_scanChannels_t.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_SetExtScanChannels( uint8 extScanChannelsMap );

/**
 * @brief       This API is used to set the QOS Parameters
 *              according to the entered parameter type.
 *
 * @design      /ref did_361975877
 * @design      /ref did_408769671
 *
 * input parameters
 *
 * @param       taskType  - The type of task.
 *                          For Connections task type:        LL_QOS_CONN_TASK_TYPE.
 *                          For Advertise task type:          LL_QOS_ADV_TASK_TYPE.
 *                          For Scan task type:               LL_QOS_SCN_TASK_TYPE.
 *                          For Initiator task type:          LL_QOS_INIT_TASK_TYPE.
 *                          For Periodic Advertise task type: LL_QOS_PERIODIC_ADV_TASK_TYPE.
 *                          For Periodic Scan task type:      LL_QOS_PERIODIC_SCN_TASK_TYPE.
 *
 * @param       paramType  - The type of parameter.
 *                           General: LL_QOS_TYPE_PRIORITY.
 *                           For connections: LL_QOS_TYPE_CONN_MIN_LENGTH /
 *                                            LL_QOS_TYPE_CONN_MAX_LENGTH.
 *
 * @param       paramVal   - The value of the parameter.
 *                           General:
 *                           LL_QOS_TYPE_PRIORITY optional values: LL_QOS_LOW_PRIORITY,
 *                                                                 LL_QOS_MEDIUM_PRIORITY,
 *                                                                 LL_QOS_HIGH_PRIORITY.
 *                                                                 Range [0-2].
 *                           For connections only:
 *                           LL_QOS_TYPE_CONN_MIN_LENGTH optional values: Time in [us].
 *                                                                        for coded connection   Range [LL_MIN_LINK_DATA_TIME_CODED (2704 us) - LL_MAX_LINK_DATA_TIME_CODED (17040 us)].
 *                                                                        for uncoded connection Range [LL_MIN_LINK_DATA_TIME (328 us) - LL_MAX_LINK_DATA_TIME_UNCODED (2120 us)].
 *                           LL_QOS_TYPE_CONN_MAX_LENGTH optional values: Time in [us].
 *                                                                        for coded connection   Range [LL_MIN_LINK_DATA_TIME_CODED (2704 us) - Connection's Interval ]
 *                                                                        for uncoded connection Range [LL_MIN_LINK_DATA_TIME (328 us) - Connection's Interval ]
 *
 *                          Note: For the LL_QOS_TYPE_CONN_MIN_LENGTH value a margin time is added of LL_TOTAL_MARGIN_TIME_FOR_MIN_CONN_RAT_TICKS.
 *                          Note: LL_QOS_TYPE_CONN_MAX_LENGTH must be larger than (LL_QOS_TYPE_CONN_MIN_LENGTH + LL_TOTAL_MARGIN_TIME_FOR_MIN_CONN_RAT_TICKS) Value.
 *
 *
 * @param       taskHandle  - The Task Handle of which we want to update it's parameters.
 *                            This variable is not relevent for LL_QOS_SCN_TASK_TYPE / LL_QOS_INIT_TASK_TYPE because
 *                            There is only 1 set for scanner or initiator.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_SetQOSParameters( uint8  taskType,
                                             uint8  paramType,
                                             uint32 ParamVal,
                                             uint16 taskHandle);

/**
 * @brief       This API is used to set the Default QOS Parameters Values.
 *
 * @design      /ref did_361975877
 * @design      /ref did_408769671
 *
 * input parameters
 *
 * @param       paramDefaultVal   - The default value of this qos type.
 *                                  For LL_QOS_TYPE_PRIORITY optional values: LL_QOS_LOW_PRIORITY,
 *                                                                            LL_QOS_MEDIUM_PRIORITY,
 *                                                                            LL_QOS_HIGH_PRIORITY.
 *                                                                            Range [0-2].
 *
 * @param       paramType         - The type of parameter.
 *                                  General: LL_QOS_TYPE_PRIORITY.
 *                                  For connections: LL_QOS_TYPE_CONN_MIN_LENGTH /
 *                                                   LL_QOS_TYPE_CONN_MAX_LENGTH.
 *
 * @param       taskType          - The type of task we would like to change it's default value.
 *                                  For Connections task type:        LL_QOS_CONN_TASK_TYPE.
 *                                  For Advertise task type:          LL_QOS_ADV_TASK_TYPE.
 *                                  For Scan task type:               LL_QOS_SCN_TASK_TYPE.
 *                                  For Initiator task type:          LL_QOS_INIT_TASK_TYPE.
 *                                  For Periodic Advertise task type: LL_QOS_PERIODIC_ADV_TASK_TYPE.
 *                                  For Periodic Scan task type:      LL_QOS_PERIODIC_SCN_TASK_TYPE.
 * output parameters
 *
 * @param       None.
 *
 * @return      @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_SetQOSDefaultParameters(uint32 paramDefaultVal,
                                                   uint8  paramType,
                                                   uint8  taskType);

/**
 * Enable or disable the Coex feature.
 *
 * @par Corresponding Events
 * @ref hciEvt_CmdComplete_t with cmdOpcode @ref HCI_EXT_COEX_ENABLE
 *
 * @param enable - 1 to enable and 0 to disable.
 *
 * @return @ref HCI_SUCCESS
 */
extern hciStatus_t HCI_EXT_CoexEnableCmd( uint8 enable );

#ifdef __cplusplus
}
#endif

#endif /* HCI_H */

/** @} End HCI */
