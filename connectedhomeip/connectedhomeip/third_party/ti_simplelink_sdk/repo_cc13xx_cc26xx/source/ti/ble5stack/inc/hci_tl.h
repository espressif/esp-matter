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
 *  @addtogroup HCI
 *  @{
 *  @file  hci_tl.h
 * @brief Types, constants, external functions etc. for
        the BLE HCI Transport Layer.
 */

#ifndef HCI_TL_H
#define HCI_TL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */

#include "hci.h"
#include "osal.h"
#include "hci_data.h"
#include "hci_event.h"

/// @cond NODOC
extern uint8 hciTaskID;
extern uint8 hciTestTaskID;
extern uint8 hciGapTaskID;
extern uint8 hciL2capTaskID;
extern uint8 hciSmpTaskID;
/// @endcond //NODOC

/*******************************************************************************
 * MACROS
 */

/// @cond NODOC
/// @brief HCI Assert
#define HCI_ASSERT(condition)  HAL_ASSERT(condition)
/// @endcond // NODOC

/** @} */ // end of HCI

/*******************************************************************************
 * CONSTANTS
 */

/**
 *  @addtogroup HCI_Constants
 *  @{
*/

/// @cond NODOC

// OSAL Task Events
#define HCI_TX_PROCESS_EVENT                              0x0001        //!< TX Process Event
#define HCI_TEST_UART_SEND_EVENT                          0x0002        //!< Test UART Send Event
#define HCI_BDADDR_UPDATED_EVENT                          0x4000        //!< BD Address Updated Event

// OSAL Message Header Events
#define HCI_CTRL_TO_HOST_EVENT                            0x01          //!< Controller to Host Event
#define HCI_HOST_TO_CTRL_CMD_EVENT                        0x02          //!< Host to Controller Command
#define HCI_HOST_TO_CTRL_DATA_EVENT                       0x03          //!< Host to Controller Data

#define HCI_BDADDR_LEN                                    6             //!< BD Address Length

// Max Allowed HCI Packet
#define HCI_MAX_CMD_PKT_SIZE                              0xFF          //!< Max Command Packet Size
#define HCI_MAX_DATA_PKT_SIZE                             0xFFFF        //!< Max Data Pakcet Size

// Max Data Length in Packet
#define HCI_DATA_MAX_DATA_LENGTH                          27            //!< Max Data Length

/**
 * @brief Minimum length for command packet
 *
 * Minimum length for CMD packet is 1+2+1
 * | Packet Type (1) | OPCode(2) | Length(1) |
 */
#define HCI_CMD_MIN_LENGTH                                4

/**
 * @brief Minimum length for event packet
 *
 * Minimum length for EVENT packet is 1+1+1
 * | Packet Type (1) | Event Code(1) | Length(1) |
 */
#define HCI_EVENT_MIN_LENGTH                              3

/**
 * @brief Minimum length for event packet
 *
 * Minimum length for HCI_EXT_GAP_ADV_SCAN_EVENT packet is 2+1+4+1+1
 * | Event Type (2) | Status(1) | Event Code(4) | Handle (1) | Data len (1) |
 */
#define HCI_EXT_GAP_ADV_EVENT_MIN_LENGTH                  9

/**
 * @brief Minimum length for data packet
 *
 * Minimum length for DATA packet is 1+2+2
 * | Packet Type (1) | Handler(2) | Length(2) |
 */
#define HCI_DATA_MIN_LENGTH                               5

#define HCI_MAX_NUM_CONNECTIONS                           0x03        //!< Max Number of Connections

#define HCI_TX_DATA_ANY_CONNECTION                        0xFF        //!< TX Data Any Connection

// HCI Packet Types
#define HCI_CMD_PACKET                                    0x01        //!< Command Packet
#define HCI_ACL_DATA_PACKET                               0x02        //!< ACL Data Packet
#define HCI_SCO_DATA_PACKET                               0x03        //!< SCO Data Packet
#define HCI_EVENT_PACKET                                  0x04        //!< Event Packet
#define HCI_EXTENDED_EVENT_PACKET                         0x05        //!< Extended Event Packet   (2 bytes length)
#define HCI_EXTENDED_CMD_PACKET                           0x09        //!< Extended Command Packet (2 bytes length)


// States for Command and Data packet parser
#define HCI_PARSER_STATE_PKT_TYPE                         0           //!< Packet Type Parser State
#define HCI_CMD_PARSER_STATE_OPCODE                       1           //!< Command Opcode Parser State
#define HCI_CMD_PARSER_STATE_LENGTH                       2           //!< Command Length Parser State
#define HCI_CMD_PARSER_STATE_DATA                         3           //!< Command Data Parser State
#define HCI_DATA_PARSER_STATE_HANDLE                      4           //!< Data Handle Parser State
#define HCI_DATA_PARSER_STATE_LENGTH                      5           //!< Data Parser State
#define HCI_DATA_PARSER_STATE_DATA                        6           //!< Data Data Parser State

// HCI Command Subgroup
#define HCI_OPCODE_CSG_LINK_LAYER                         0           //!< Link Layer Command Subgroup
#define HCI_OPCODE_CSG_CSG_L2CAP                          1           //!< L2CAP Command Subgroup
#define HCI_OPCODE_CSG_CSG_ATT                            2           //!< ATT Command Subgroup
#define HCI_OPCODE_CSG_CSG_GATT                           3           //!< GATT Command Subgroup
#define HCI_OPCODE_CSG_CSG_GAP                            4           //!< GAP Command Subgroup
#define HCI_OPCODE_CSG_CSG_SM                             5           //!< SM Command Subgroup
#define HCI_OPCODE_CSG_CSG_Reserved                       6           //!< Reserved
#define HCI_OPCODE_CSG_CSG_USER_PROFILE                   7           //!< User Profile Command Subgroup

// Vendor Specific OGF
#define VENDOR_SPECIFIC_OGF                               0x3F        //!< Vendor Specific

/// @endcond // NODOC

/*
** HCI Command Opcodes
*/

// Link Control Commands
/// @cond NODOC
#define HCI_DISCONNECT                                    0x0406    //!< opcode of @ref HCI_DisconnectCmd
/// @endcond //NODOC
#define HCI_READ_REMOTE_VERSION_INFO                      0x041D    //!< opcode of @ref HCI_ReadRemoteVersionInfoCmd

// Controller and Baseband Commands
#define HCI_SET_EVENT_MASK                                0x0C01    //!< opcode of @ref HCI_SetEventMaskCmd
#define HCI_RESET                                         0x0C03    //!< opcode of@ref HCI_ResetCmd
#define HCI_READ_TRANSMIT_POWER                           0x0C2D    //!< opcode of@ref HCI_ReadTransmitPowerLevelCmd
/// @cond NODOC
#define HCI_SET_CONTROLLER_TO_HOST_FLOW_CONTROL           0x0C31    //!< opcode of @ref HCI_SetControllerToHostFlowCtrlCmd
#define HCI_HOST_BUFFER_SIZE                              0x0C33    //!< opcode of @ref HCI_HostBufferSizeCmd
/// @endcond //NODOC
#define HCI_HOST_NUM_COMPLETED_PACKETS                    0x0C35    //!< opcode of @ref HCI_HostNumCompletedPktCmd
#define HCI_SET_EVENT_MASK_PAGE_2                         0x0C63    //!< opcode of @ref HCI_SetEventMaskPage2Cmd
#define HCI_READ_AUTH_PAYLOAD_TIMEOUT                     0x0C7B    //!< opcode of @ref HCI_ReadAuthPayloadTimeoutCmd
#define HCI_WRITE_AUTH_PAYLOAD_TIMEOUT                    0x0C7C    //!< opcode of @ref HCI_WriteAuthPayloadTimeoutCmd

// Information Parameters
#define HCI_READ_LOCAL_VERSION_INFO                       0x1001    //!< opcode of @ref HCI_ReadLocalVersionInfoCmd
#define HCI_READ_LOCAL_SUPPORTED_COMMANDS                 0x1002    //!< opcode of @ref HCI_ReadLocalSupportedCommandsCmd
#define HCI_READ_LOCAL_SUPPORTED_FEATURES                 0x1003    //!< opcode of @ref HCI_ReadLocalSupportedFeaturesCmd
#define HCI_READ_BDADDR                                   0x1009    //!< opcode of @ref HCI_ReadBDADDRCmd

// Status Parameters
#define HCI_READ_RSSI                                     0x1405    //!< opcode of @ref HCI_ReadRssiCmd

// LE Commands
// V4.0
#define HCI_LE_SET_EVENT_MASK                             0x2001    //!< opcode of @ref HCI_LE_SetEventMaskCmd
#define HCI_LE_READ_BUFFER_SIZE                           0x2002    //!< opcode of @ref HCI_LE_ReadBufSizeCmd
#define HCI_LE_READ_LOCAL_SUPPORTED_FEATURES              0x2003    //!< opcode of @ref HCI_LE_ReadLocalSupportedFeaturesCmd
/// @cond NODOC
#define HCI_LE_SET_RANDOM_ADDR                            0x2005    //!< opcode of @ref HCI_LE_SetRandomAddressCmd
#define HCI_LE_SET_ADV_PARAM                              0x2006    //!< opcode of @ref HCI_LE_SetAdvParamCmd
#define HCI_LE_READ_ADV_CHANNEL_TX_POWER                  0x2007    //!< opcode of @ref HCI_LE_ReadAdvChanTxPowerCmd
#define HCI_LE_SET_ADV_DATA                               0x2008    //!< opcode of @ref HCI_LE_SetAdvDataCmd
#define HCI_LE_SET_SCAN_RSP_DATA                          0x2009    //!< opcode of @ref HCI_LE_SetScanRspDataCmd
#define HCI_LE_SET_ADV_ENABLE                             0x200A    //!< opcode of @ref HCI_LE_SetAdvEnableCmd
#define HCI_LE_SET_SCAN_PARAM                             0x200B    //!< opcode of @ref HCI_LE_SetScanParamCmd
#define HCI_LE_SET_SCAN_ENABLE                            0x200C    //!< opcode of @ref HCI_LE_SetScanEnableCmd
#define HCI_LE_CREATE_CONNECTION                          0x200D    //!< opcode of @ref HCI_LE_CreateConnCmd
#define HCI_LE_CREATE_CONNECTION_CANCEL                   0x200E    //!< opcode of @ref HCI_LE_CreateConnCancelCmd
/// @endcond // NODOC
#define HCI_LE_READ_WHITE_LIST_SIZE                       0x200F    //!< opcode of @ref HCI_LE_ReadWhiteListSizeCmd
#define HCI_LE_CLEAR_WHITE_LIST                           0x2010    //!< opcode of @ref HCI_LE_ClearWhiteListCmd
#define HCI_LE_ADD_WHITE_LIST                             0x2011    //!< opcode of @ref HCI_LE_AddWhiteListCmd
#define HCI_LE_REMOVE_WHITE_LIST                          0x2012    //!< opcode of @ref HCI_LE_RemoveWhiteListCmd
/// @cond NODOC
#define HCI_LE_CONNECTION_UPDATE                          0x2013    //!< opcode of @ref HCI_LE_ConnUpdateCmd
/// @endcond // NODOC
#define HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION            0x2014    //!< opcode of @ref HCI_LE_SetHostChanClassificationCmd
#define HCI_LE_READ_CHANNEL_MAP                           0x2015    //!< opcode of @ref HCI_LE_ReadChannelMapCmd
#define HCI_LE_READ_REMOTE_USED_FEATURES                  0x2016    //!< opcode of @ref HCI_LE_ReadRemoteUsedFeaturesCmd
#define HCI_LE_ENCRYPT                                    0x2017    //!< opcode of @ref HCI_LE_EncryptCmd
#define HCI_LE_RAND                                       0x2018    //!< opcode of @ref HCI_LE_RandCmd
/// @cond NODOC
#define HCI_LE_START_ENCRYPTION                           0x2019    //!< opcode of @ref HCI_LE_StartEncyptCmd
#define HCI_LE_LTK_REQ_REPLY                              0x201A    //!< opcode of @ref HCI_LE_LtkReqReplyCmd
#define HCI_LE_LTK_REQ_NEG_REPLY                          0x201B    //!< opcode of @ref HCI_LE_LtkReqNegReplyCmd
/// @endcond // NODOC
#define HCI_LE_READ_SUPPORTED_STATES                      0x201C    //!< opcode of @ref HCI_LE_ReadSupportedStatesCmd
#define HCI_LE_RECEIVER_TEST                              0x201D    //!< opcode of @ref HCI_LE_ReceiverTestCmd
#define HCI_LE_TRANSMITTER_TEST                           0x201E    //!< opcode of @ref HCI_LE_TransmitterTestCmd
#define HCI_LE_TEST_END                                   0x201F    //!< opcode of @ref HCI_LE_TestEndCmd
// V4.1
/// @cond NODOC
#define HCI_LE_REMOTE_CONN_PARAM_REQ_REPLY                0x2020    //!< opcode of @ref HCI_LE_RemoteConnParamReqReplyCmd
#define HCI_LE_REMOTE_CONN_PARAM_REQ_NEG_REPLY            0x2021    //!< opcode of @ref HCI_LE_RemoteConnParamReqNegReplyCmd
/// @endcond // NODOC
// V4.2
#define HCI_LE_SET_DATA_LENGTH                            0x2022    //!< opcode of @ref HCI_LE_SetDataLenCmd
#define HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH         0x2023    //!< opcode of @ref HCI_LE_ReadSuggestedDefaultDataLenCmd
#define HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH        0x2024    //!< opcode of @ref HCI_LE_WriteSuggestedDefaultDataLenCmd
/// @cond NODOC
#define HCI_LE_READ_LOCAL_P256_PUBLIC_KEY                 0x2025    //!< opcode of @ref HCI_LE_ReadLocalP256PublicKeyCmd
#define HCI_LE_GENERATE_DHKEY                             0x2026    //!< opcode of @ref HCI_LE_GenerateDHKeyCmd
/// @endcond // NODOC
#define HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST               0x2027    //!< opcode of @ref HCI_LE_AddDeviceToResolvingListCmd
#define HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST          0x2028    //!< opcode of @ref HCI_LE_RemoveDeviceFromResolvingListCmd
#define HCI_LE_CLEAR_RESOLVING_LIST                       0x2029    //!< opcode of @ref HCI_LE_ClearResolvingListCmd
#define HCI_LE_READ_RESOLVING_LIST_SIZE                   0x202A    //!< opcode of @ref HCI_LE_ReadResolvingListSizeCmd
#define HCI_LE_READ_PEER_RESOLVABLE_ADDRESS               0x202B    //!< opcode of @ref HCI_LE_ReadPeerResolvableAddressCmd
#define HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS              0x202C    //!< opcode of @ref HCI_LE_ReadLocalResolvableAddressCmd
/// @cond NODOC
#define HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE              0x202D    //!< opcode of @ref HCI_LE_SetAddressResolutionEnableCmd
/// @endcond // NODOC
#define HCI_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT     0x202E    //!< opcode of @ref HCI_LE_SetResolvablePrivateAddressTimeoutCmd
#define HCI_LE_READ_MAX_DATA_LENGTH                       0x202F    //!< opcode of @ref HCI_LE_ReadMaxDataLenCmd
#define HCI_LE_SET_PRIVACY_MODE                           0x204E    //!< opcode of @ref HCI_LE_SetPrivacyModeCmd
// V5.0
#define HCI_LE_READ_PHY                                   0x2030    //!< opcode of @ref HCI_LE_ReadPhyCmd
#define HCI_LE_SET_DEFAULT_PHY                            0x2031    //!< opcode of @ref HCI_LE_SetDefaultPhyCmd
#define HCI_LE_SET_PHY                                    0x2032    //!< opcode of @ref HCI_LE_SetPhyCmd
#define HCI_LE_ENHANCED_RECEIVER_TEST                     0x2033    //!< opcode of @ref HCI_LE_EnhancedRxTestCmd
#define HCI_LE_ENHANCED_TRANSMITTER_TEST                  0x2034    //!< opcode of @ref HCI_LE_EnhancedTxTestCmd
#define HCI_LE_READ_TX_POWER                              0x204B    //!< opcode of @ref HCI_LE_ReadTxPowerCmd
#define HCI_LE_READ_RF_PATH_COMPENSATION                  0x204C    //!< opcode of @ref HCI_LE_ReadRfPathCompCmd
#define HCI_LE_WRITE_RF_PATH_COMPENSATION                 0x204D    //!< opcode of @ref HCI_LE_WriteRfPathCompCmd
// @cond NODOC
#define HCI_LE_SET_ADV_SET_RANDOM_ADDRESS                 0x2035    //!< opcode of @ref HCI_LE_SetAdvSetRandAddrCmd
#define HCI_LE_SET_EXT_ADV_PARAMETERS                     0x2036    //!< opcode of @ref HCI_LE_SetExtAdvParamsCmd
#define HCI_LE_SET_EXT_ADV_DATA                           0x2037    //!< opcode of @ref HCI_LE_SetExtAdvDataCmd
#define HCI_LE_SET_EXT_SCAN_RESPONSE_DATA                 0x2038    //!< opcode of @ref HCI_LE_SetExtScanRspDataCmd
#define HCI_LE_SET_EXT_ADV_ENABLE                         0x2039    //!< opcode of @ref HCI_LE_SetExtAdvEnableCmd
#define HCI_LE_READ_MAX_ADV_DATA_LENGTH                   0x203A    //!< opcode of @ref HCI_LE_ReadMaxAdvDataLenCmd
#define HCI_LE_READ_NUM_SUPPORTED_ADV_SETS                0x203B    //!< opcode of @ref HCI_LE_ReadNumSupportedAdvSetsCmd
#define HCI_LE_REMOVE_ADV_SET                             0x203C    //!< opcode of @ref HCI_LE_RemoveAdvSetCmd
#define HCI_LE_CLEAR_ADV_SETS                             0x203D    //!< opcode of @ref HCI_LE_ClearAdvSetsCmd
#define HCI_LE_SET_PERIODIC_ADV_PARAMETERS                0x203E    //!< opcode of @ref HCI_LE_SetPeriodicAdvParamsCmd
#define HCI_LE_SET_PERIODIC_ADV_DATA                      0x203F    //!< opcode of @ref HCI_LE_SetPeriodicAdvDataCmd
#define HCI_LE_SET_PERIODIC_ADV_ENABLE                    0x2040    //!< opcode of @ref HCI_LE_SetPeriodicAdvEnableCmd
#define HCI_LE_SET_EXT_SCAN_PARAMETERS                    0x2041    //!< opcode of @ref LE_SetExtScanParams
#define HCI_LE_SET_EXT_SCAN_ENABLE                        0x2042    //!< opcode of @ref LE_SetExtScanEnable
#define HCI_LE_EXT_CREATE_CONN                            0x2043    //!< opcode of @ref LE_ExtCreateConn
#define HCI_LE_PERIODIC_ADV_CREATE_SYNC                   0x2044    //!< opcode of @ref HCI_LE_PeriodicAdvCreateSyncCmd
#define HCI_LE_PERIODIC_ADV_CREATE_SYNC_CANCEL            0x2045    //!< opcode of @ref HCI_LE_PeriodicAdvCreateSyncCancelCmd
#define HCI_LE_PERIODIC_ADV_TERMINATE_SYNC                0x2046    //!< opcode of @ref HCI_LE_PeriodicAdvTerminateSyncCmd
#define HCI_LE_ADD_DEVICE_TO_PERIODIC_ADV_LIST            0x2047    //!< opcode of @ref HCI_LE_AddDeviceToPeriodicAdvListCmd
#define HCI_LE_REMOVE_DEVICE_FROM_PERIODIC_ADV_LIST       0x2048    //!< opcode of @ref HCI_LE_RemoveDeviceFromPeriodicAdvListCmd
#define HCI_LE_CLEAR_PERIODIC_ADV_LIST                    0x2049    //!< opcode of @ref HCI_LE_ClearPeriodicAdvListCmd
#define HCI_LE_READ_PERIODIC_ADV_LIST_SIZE                0x204A    //!< opcode of @ref HCI_LE_ReadPeriodicAdvListSizeCmd
// @endcond // NODOC

// V5.1
// @cond NODOC
#define HCI_LE_ENHANCED_CTE_RECEIVER_TEST                 0x204F    //!< opcode of @ref HCI_LE_EnhancedCteRxTestCmd
#define HCI_LE_ENHANCED_CTE_TRANSMITTER_TEST              0x2050    //!< opcode of @ref HCI_LE_EnhancedCteTxTestCmd
#define HCI_LE_SET_CONNECTIONLESS_CTE_TRANSMIT_PARAMS     0x2051    //!< opcode of @ref HCI_LE_SetConnectionlessCteTransmitParamsCmd
#define HCI_LE_SET_CONNECTIONLESS_CTE_TRANSMIT_ENABLE     0x2052    //!< opcode of @ref HCI_LE_SetConnectionlessCteTransmitEnableCmd
#define HCI_LE_SET_CONNECTIONLESS_IQ_SAMPLING_ENABLE      0x2053    //!< opcode of @ref HCI_LE_SetConnectionlessIqSamplingEnableCmd
#define HCI_LE_SET_CONNECTION_CTE_RECEIVE_PARAMS          0x2054    //!< opcode of @ref HCI_LE_SetConnectionCteReceiveParams
#define HCI_LE_SET_CONNECTION_CTE_TRANSMIT_PARAMS         0x2055    //!< opcode of @ref HCI_LE_SetConnectionCteTransmitParams
#define HCI_LE_SET_CONNECTION_CTE_REQUEST_ENABLE          0x2056    //!< opcode of @ref HCI_LE_SetConnectionCteRequestEnable
#define HCI_LE_SET_CONNECTION_CTE_RESPONSE_ENABLE         0x2057    //!< opcode of @ref HCI_LE_SetConnectionCteResponseEnable
#define HCI_LE_READ_ANTENNA_INFORMATION                   0x2058    //!< opcode of @ref HCI_LE_ReadAntennaInformation
#define HCI_LE_SET_PERIODIC_ADV_RECEIVE_ENABLE            0x2059    //!< opcode of @ref HCI_LE_SetPeriodicAdvReceiveEnableCmd
// @endcond // NODOC

/// @endcond //NODOC
// LE Vendor Specific LL Extension Commands
#define HCI_EXT_SET_RX_GAIN                                 0xFC00    //!< opcode of @ref HCI_EXT_SetRxGainCmd
#define HCI_EXT_SET_TX_POWER                                0xFC01    //!< opcode of @ref HCI_EXT_SetTxPowerCmd
#define HCI_EXT_ONE_PKT_PER_EVT                             0xFC02    //!< opcode of @ref HCI_EXT_OnePktPerEvtCmd
/// @cond CC254X
#define HCI_EXT_CLK_DIVIDE_ON_HALT                          0xFC03    //!< opcode of @ref HCI_EXT_ClkDivOnHaltCmd
#define HCI_EXT_DECLARE_NV_USAGE                            0xFC04    //!< opcode of @ref HCI_EXT_DeclareNvUsageCmd
/// @endcond // CC254X
#define HCI_EXT_DECRYPT                                     0xFC05    //!< opcode of @ref HCI_EXT_DecryptCmd
#define HCI_EXT_SET_LOCAL_SUPPORTED_FEATURES                0xFC06    //!< opcode of @ref HCI_EXT_SetLocalSupportedFeaturesCmd
#define HCI_EXT_SET_FAST_TX_RESP_TIME                       0xFC07    //!< opcode of @ref HCI_EXT_SetFastTxResponseTimeCmd
#define HCI_EXT_MODEM_TEST_TX                               0xFC08    //!< opcode of @ref HCI_EXT_ModemTestTxCmd
#define HCI_EXT_MODEM_HOP_TEST_TX                           0xFC09    //!< opcode of @ref HCI_EXT_ModemHopTestTxCmd
#define HCI_EXT_MODEM_TEST_RX                               0xFC0A    //!< opcode of @ref HCI_EXT_ModemTestRxCmd
#define HCI_EXT_END_MODEM_TEST                              0xFC0B    //!< opcode of @ref HCI_EXT_EndModemTestCmd
/// @cond NODOC
#define HCI_EXT_SET_BDADDR                                  0xFC0C    //!< opcode of @ref HCI_EXT_SetBDADDRCmd
/// @endcond //NODOC
#define HCI_EXT_SET_SCA                                     0xFC0D    //!< opcode of @ref HCI_EXT_SetSCACmd
#define HCI_EXT_ENABLE_PTM                                  0xFC0E    //!< opcode of @ref HCI_EXT_EnablePTMCmd
#define HCI_EXT_SET_FREQ_TUNE                               0xFC0F    //!< opcode of @ref HCI_EXT_SetFreqTuneCmd
#define HCI_EXT_SAVE_FREQ_TUNE                              0xFC10    //!< opcode of @ref HCI_EXT_SaveFreqTuneCmd
#define HCI_EXT_SET_MAX_DTM_TX_POWER                        0xFC11    //!< opcode of @ref HCI_EXT_SetMaxDtmTxPowerCmd
/// @cond CC254X
#define HCI_EXT_MAP_PM_IO_PORT                              0xFC12    //!< opcode of @ref HCI_EXT_MapPmIoPortCmd
/// @endcond //CC254X
#define HCI_EXT_DISCONNECT_IMMED                            0xFC13    //!< opcode of @ref HCI_EXT_DisconnectImmedCmd
#define HCI_EXT_PER                                         0xFC14    //!< opcode of @ref HCI_EXT_PacketErrorRateCmd
#define HCI_EXT_PER_BY_CHAN                                 0xFC15    //!< opcode of @ref HCI_EXT_PERbyChanCmd
/// @cond CC254X
#define HCI_EXT_EXTEND_RF_RANGE                             0xFC16    //!< opcode of  @ref HCI_EXT_ExtendRfRangeCmd
/// @endcond // CC254X
/// @cond CC254X
#define HCI_EXT_HALT_DURING_RF                              0xFC19    //!< opcode of @ref HCI_EXT_HaltDuringRfCmd
/// @endcond // CC254X
#define HCI_EXT_OVERRIDE_SL                                 0xFC1A    //!< opcode of @ref HCI_EXT_SetSlaveLatencyOverrideCmd
#define HCI_EXT_BUILD_REVISION                              0xFC1B    //!< opcode of @ref HCI_EXT_BuildRevisionCmd
/// @cond CC254X
#define HCI_EXT_DELAY_SLEEP                                 0xFC1C    //!< opcode of @ref HCI_EXT_DelaySleepCmd
/// @endcond //CC254X
#define HCI_EXT_RESET_SYSTEM                                0xFC1D    //!< opcode of @ref HCI_EXT_ResetSystemCmd
/// @cond CC254X
#define HCI_EXT_OVERLAPPED_PROCESSING                       0xFC1E    //!< opcode of @ref HCI_EXT_OverlappedProcessingCmd
/// @endcond //CC254X
#define HCI_EXT_NUM_COMPLETED_PKTS_LIMIT                    0xFC1F    //!< opcode of @ref HCI_EXT_NumComplPktsLimitCmd
/// @cond NODOC
#define HCI_EXT_GET_CONNECTION_INFO                         0xFC20    //!< opcode of @ref HCI_EXT_GetConnInfoCmd
/// @endcond // NODOC
#define HCI_EXT_SET_MAX_DATA_LENGTH                         0xFC21    //!< opcode of @ref HCI_EXT_SetMaxDataLenCmd
#define HCI_EXT_SET_DTM_TX_PKT_CNT                          0xFC24    //!< opcode of @ref HCI_EXT_SetDtmTxPktCntCmd
/// @cond NODOC
#define HCI_EXT_READ_RAND_ADDR                              0xFC25    //!< opcode of @ref HCI_ReadRandAddrCmd
/// @endcond //NODOC
#define HCI_EXT_ENHANCED_MODEM_TEST_TX                      0xFC27    //!< opcode of @ref HCI_EXT_EnhancedModemTestTxCmd
#define HCI_EXT_ENHANCED_MODEM_HOP_TEST_TX                  0xFC28    //!< opcode of @ref HCI_EXT_EnhancedModemHopTestTxCmd
#define HCI_EXT_ENHANCED_MODEM_TEST_RX                      0xFC29    //!< opcode of @ref HCI_EXT_EnhancedModemTestRxCmd
// @cond NODOC
#define HCI_EXT_SET_PIN_OUTPUT                              0xFC2A    //!< opcode of @ref HCI_SetPinOutputCmd
#define HCI_EXT_SET_LOCATIONING_ACCURACY                    0xFC2B    //!< opcode of @ref HCI_SetLocationingAccuracyCmd
#define HCI_EXT_GET_ACTIVE_CONNECTION_INFO                  0xFC2C    //!< opcode of @ref HCI_EXT_GetActiveConnInfoCmd
#define HCI_EXT_COEX_ENABLE                                 0xFC2E    //!< opcode of @ref HCI_EXT_CoexEnableCmd

#define HCI_EXT_LL_TEST_MODE                                0xFC70    //!< opcode of @ref HCI_EXT_LLTestModeCmd

#define HCI_EXT_LE_SET_EXT_ADV_DATA                         0xFC71    //!< opcode of @ref HCI_EXT_LE_SetExtAdvDataCmd
#define HCI_EXT_LE_SET_EXT_SCAN_RESPONSE_DATA               0xFC72    //!< opcode of @ref HCI_EXT_LE_SetExtScanRspDataCmd
#define HCI_EXT_LE_SET_EXT_VIRTUAL_ADV_ADDRESS              0xFC73    //!< opcode of @ref HCI_EXT_SetVirtualAdvAddrCmd
#define HCI_EXT_SET_SCAN_CHAN                               0xFC74    //!< opcode of @ref HCI_EXT_SetExtScanChannels
#define HCI_EXT_SET_QOS_PARAMETERS                          0xFC75    //!< opcode of @ref HCI_EXT_SetQOSParameters
#define HCI_EXT_SET_QOS_DEFAULT_PARAMETERS                  0xFC76    //!< opcode of @ref HCI_EXT_SetQOSDefaultParameters
#define HCI_LE_MAKE_DISCOVERABLE_DONE                       0xFC77    //!< opcode of @ref HCI_LE_SetAdvStatus
#define HCI_LE_END_DISCOVERABLE_DONE                        0xFC78    //!< opcode of @ref HCI_LE_SetAdvStatus
#define HCI_EXT_SET_HOST_DEFAULT_CHANNEL_CLASSIFICATION     0xFC79    //!< opcode of @ref HCI_EXT_SetHostDefChanClassificationCmd
#define HCI_EXT_SET_HOST_CONNECTION_CHANNEL_CLASSIFICATION  0xFC7A    //!< opcode of @ref HCI_EXT_SetHostConnChanClassificationCmd

// @endcond //NODOC

/*
** HCI Event Codes
*/

// BT Events
#define HCI_DISCONNECTION_COMPLETE_EVENT_CODE             0x05      //!< event of type @ref hciEvt_DisconnComplete_t
#define HCI_ENCRYPTION_CHANGE_EVENT_CODE                  0x08      //!< event of type @ref hciEvt_EncryptChange_t
#define HCI_READ_REMOTE_INFO_COMPLETE_EVENT_CODE          0x0C      //!< event of type @ref hciPacket_t
#define HCI_COMMAND_COMPLETE_EVENT_CODE                   0x0E      //!< event of type @ref hciEvt_CmdComplete_t
#define HCI_COMMAND_STATUS_EVENT_CODE                     0x0F      //!< event of type @ref hciEvt_CommandStatus_t
#define HCI_BLE_HARDWARE_ERROR_EVENT_CODE                 0x10      //!< event of type @ref hciEvt_HardwareError_t
#define HCI_NUM_OF_COMPLETED_PACKETS_EVENT_CODE           0x13      //!< event of type @ref hciEvt_NumCompletedPkt_t
#define HCI_DATA_BUFFER_OVERFLOW_EVENT                    0x1A      //!< event of type @ref hciEvt_BufferOverflow_t
#define HCI_KEY_REFRESH_COMPLETE_EVENT_CODE               0x30      //!< event of type @ref hciPacket_t
#define HCI_APTO_EXPIRED_EVENT_CODE                       0x57      //!< event of type @ref hciEvt_AptoExpired_t

// LE Event Code (for LE Meta Events)
#define HCI_LE_EVENT_CODE                                 0x3E      //!< LE Event

// LE Meta Event Codes
#define HCI_BLE_CONNECTION_COMPLETE_EVENT                 0x01      //!< event of type @ref hciEvt_BLEConnComplete_t
#define HCI_BLE_ADV_REPORT_EVENT                          0x02      //!< event of type @ref hciEvt_DevInfo_t
#define HCI_BLE_CONN_UPDATE_COMPLETE_EVENT                0x03      //!< event of type @ref hciEvt_BLEConnUpdateComplete_t
#define HCI_BLE_READ_REMOTE_FEATURE_COMPLETE_EVENT        0x04      //!< event of type @ref hciEvt_BLEReadRemoteFeatureComplete_t
#define HCI_BLE_LTK_REQUESTED_EVENT                       0x05      //!< event of type @ref hciEvt_BLELTKReq_t
// V4.1
#define HCI_BLE_REMOTE_CONN_PARAM_REQUEST_EVENT           0x06      //!< event of type @ref hciEvt_BLERemoteConnParamReq_t
// V4.2
#define HCI_BLE_DATA_LENGTH_CHANGE_EVENT                  0x07      //!< event of type @ref hciEvt_BLEDataLengthChange_t
#define HCI_BLE_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE_EVENT 0x08      //!< event of type @ref hciEvt_BLEReadP256PublicKeyComplete_t
#define HCI_BLE_GENERATE_DHKEY_COMPLETE_EVENT             0x09      //!< event of type @ref hciEvt_BLEGenDHKeyComplete_t
#define HCI_BLE_ENHANCED_CONNECTION_COMPLETE_EVENT        0x0A      //!< event of type @ref hciEvt_BLEEnhConnComplete_t
#define HCI_BLE_DIRECT_ADVERTISING_REPORT_EVENT           0x0B      //!< direct advertising report...not used
// V5.0
#define HCI_BLE_PHY_UPDATE_COMPLETE_EVENT                 0x0C      //!< event of type @ref hciEvt_BLEPhyUpdateComplete_t

#define HCI_BLE_EXTENDED_ADV_REPORT_EVENT                 0x0D      //!< Extended Adv Report
#define HCI_BLE_PERIODIC_ADV_SYNCH_ESTABLISHED_EVENT      0x0E      //!< Periodic Adv Synch Established
#define HCI_BLE_PERIODIC_ADV_REPORT_EVENT                 0x0F      //!< Periodic Adv Report
#define HCI_BLE_PERIODIC_ADV_SYNCH_LOST_EVENT             0x10      //!< Periodic Adv Synch Lost
#define HCI_BLE_SCAN_TIMEOUT_EVENT                        0x11      //!< Scan Timeout
#define HCI_BLE_ADV_SET_TERMINATED_EVENT                  0x12      //!< Adv Set Terminated
#define HCI_BLE_SCAN_REQUEST_RECEIVED_EVENT               0x13      //!< Scan Request Received
#define HCI_BLE_CHANNEL_SELECTION_ALGORITHM_EVENT         0x14      //!< Channel Selection Algorithm
#define HCI_BLE_CONNECTIONLESS_IQ_REPORT_EVENT            0x15      //!< CTE sample connectionless report
#define HCI_BLE_CONNECTION_IQ_REPORT_EVENT                0x16      //!< CTE sample connection report
#define HCI_BLE_CTE_REQUEST_FAILED_EVENT                  0x17      //!< CTE sample failed

#define HCI_BLE_BLE_LOG_STRINGS_MAX 0x17
extern char *BLEEventCode_BleLogStrings[];

// VS Meta Event Codes - Texas Instruments Inc specific!
#define HCI_BLE_SCAN_REQ_REPORT_EVENT                     0x80      //!< event of type @ref hciEvt_BLEScanReqReport_t
#define HCI_BLE_EXT_CONNECTION_IQ_REPORT_EVENT            0x81      //!< event of type @ref hciEvt_BLEExtCteConnectionIqReport_t
#define HCI_BLE_CHANNEL_MAP_UPDATE_EVENT                  0x82      //!< event of type @ref hciEvt_BLEChanMapUpdate_t
#define HCI_BLE_CONN_UPDATE_REJECT_EVENT                  0x83      //!< event of type @ref hciEvt_BLEConnUpdateComplete_t
#define HCI_BLE_EXT_CONNECTIONLESS_IQ_REPORT_EVENT        0x84      //!< event of type @ref hciEvt_BLEExtCteConnectionlessIqReport_t

#define HCI_TEST_EVENT_CODE                               0xFE      //!< test event code use for cte test
#define HCI_VE_EVENT_CODE                                 0xFF      //!< event of type @ref hciEvt_VSCmdComplete_t

// LE Vendor Specific LL Extension Events
#define HCI_EXT_SET_RX_GAIN_EVENT                         0x0400    //!< event from @ref HCI_EXT_SetRxGainCmd
#define HCI_EXT_SET_TX_POWER_EVENT                        0x0401    //!< event from @ref HCI_EXT_SetTxPowerCmd
#define HCI_EXT_ONE_PKT_PER_EVT_EVENT                     0x0402    //!< event from @ref HCI_EXT_OnePktPerEvtCmd
/// @cond CC254X
#define HCI_EXT_CLK_DIVIDE_ON_HALT_EVENT                  0x0403    //!< event from @ref HCI_EXT_ClkDivOnHaltCmd
#define HCI_EXT_DECLARE_NV_USAGE_EVENT                    0x0404    //!< event from @ref HCI_EXT_DeclareNvUsageCmd
/// @endcond //CC254X
#define HCI_EXT_DECRYPT_EVENT                             0x0405    //!< event from @ref HCI_EXT_DecryptCmd
#define HCI_EXT_SET_LOCAL_SUPPORTED_FEATURES_EVENT        0x0406    //!< event from @ref HCI_EXT_SetLocalSupportedFeaturesCmd
#define HCI_EXT_SET_FAST_TX_RESP_TIME_EVENT               0x0407    //!< event from @ref HCI_EXT_SetFastTxResponseTimeCmd
#define HCI_EXT_MODEM_TEST_TX_EVENT                       0x0408    //!< event from @ref HCI_EXT_ModemTestTxCmd
#define HCI_EXT_MODEM_HOP_TEST_TX_EVENT                   0x0409    //!< event from @ref HCI_EXT_ModemHopTestTxCmd
#define HCI_EXT_MODEM_TEST_RX_EVENT                       0x040A    //!< event from @ref HCI_EXT_ModemTestRxCmd
#define HCI_EXT_END_MODEM_TEST_EVENT                      0x040B    //!< event from @ref HCI_EXT_EndModemTestCmd
/// @cond NODOC
#define HCI_EXT_SET_BDADDR_EVENT                          0x040C    //!< event from @ref HCI_EXT_SetBDADDRCmd
/// @endcond //NODOC
#define HCI_EXT_SET_SCA_EVENT                             0x040D    //!< event from @ref HCI_EXT_SetSCACmd
#define HCI_EXT_ENABLE_PTM_EVENT                          0x040E    //!< event from @ref HCI_EXT_EnablePTMCmd
#define HCI_EXT_SET_FREQ_TUNE_EVENT                       0x040F    //!< event from @ref HCI_EXT_SetFreqTuneCmd
#define HCI_EXT_SAVE_FREQ_TUNE_EVENT                      0x0410    //!< event from @ref HCI_EXT_SaveFreqTuneCmd
#define HCI_EXT_SET_MAX_DTM_TX_POWER_EVENT                0x0411    //!< event from @ref HCI_EXT_SetMaxDtmTxPowerCmd
/// @cond CC254X
#define HCI_EXT_MAP_PM_IO_PORT_EVENT                      0x0412    //!< event from @ref HCI_EXT_MapPmIoPortCmd
/// @endcond //CC254X
#define HCI_EXT_DISCONNECT_IMMED_EVENT                    0x0413    //!< event from @ref HCI_EXT_DisconnectImmedCmd
#define HCI_EXT_PER_EVENT                                 0x0414    //!< event from @ref HCI_EXT_PacketErrorRateCmd
#define HCI_EXT_PER_BY_CHAN_EVENT                         0x0415    //!< event from @ref HCI_EXT_PERbyChanCmd
/// @cond CC254X
#define HCI_EXT_EXTEND_RF_RANGE_EVENT                     0x0416    //!< event from @ref HCI_EXT_ExtendRfRangeCmd
/// @endcond //CC254X
/// @cond CC254X
#define HCI_EXT_HALT_DURING_RF_EVENT                      0x0419    //!< event from @ref HCI_EXT_HaltDuringRfCmd
#define HCI_EXT_OVERRIDE_SL_EVENT                         0x041A    //!< event from @ref HCI_EXT_SetSlaveLatencyOverrideCmd
/// @endcond //CC254X
#define HCI_EXT_BUILD_REVISION_EVENT                      0x041B    //!< event from @ref HCI_EXT_BuildRevisionCmd
/// @cond CC254X
#define HCI_EXT_DELAY_SLEEP_EVENT                         0x041C    //!< event from @ref HCI_EXT_DelaySleepCmd
/// @endcond //CC254X
#define HCI_EXT_RESET_SYSTEM_EVENT                        0x041D    //!< event from @ref HCI_EXT_ResetSystemCmd
/// @cond CC254X
#define HCI_EXT_OVERLAPPED_PROCESSING_EVENT               0x041E    //!< event from @ref HCI_EXT_OverlappedProcessingCmd
/// @endcond //CC254X
#define HCI_EXT_NUM_COMPLETED_PKTS_LIMIT_EVENT            0x041F    //!< event from @ref HCI_EXT_NumComplPktsLimitCmd
/// @cond NODOC
#define HCI_EXT_GET_CONNECTION_INFO_EVENT                 0x0420    //!< event from @ref HCI_EXT_GetConnInfoCmd
/// @endcond // NODOC
#define HCI_EXT_SET_MAX_DATA_LENGTH_EVENT                 0x0421    //!< event from @ref HCI_EXT_SetMaxDataLenCmd
#define HCI_EXT_SET_DTM_TX_PKT_CNT_EVENT                  0x0424    //!< event from @ref HCI_EXT_SetDtmTxPktCntCmd
/// @cond NODOC
#define HCI_EXT_READ_RAND_ADDR_EVENT                      0x0425    //!< event from @ref HCI_EXT_ReadRandAddrCmd
/// @endcond //NODOC
#define HCI_EXT_LE_ADV_EVENT                              0x0425    //!< Extended advertisement event
#define HCI_EXT_LE_SCAN_EVENT                             0x0426    //!< Extended Scanner event

#define HCI_EXT_ENHANCED_MODEM_TEST_TX_EVENT              0x0427    //!< event from @ref HCI_EXT_EnhancedModemTestTxCmd
#define HCI_EXT_ENHANCED_MODEM_HOP_TEST_TX_EVENT          0x0428    //!< event from @ref HCI_EXT_EnhancedModemHopTestTxCmd
#define HCI_EXT_ENHANCED_MODEM_TEST_RX_EVENT              0x0429    //!< event from @ref HCI_EXT_EnhancedModemTestRxCmd
// @cond NODOC
#define HCI_EXT_SET_PIN_OUTPUT_EVENT                       0x042A    //!< event from @ref HCI_EXT_SetPinOutputCmd
#define HCI_EXT_SET_LOCATIONING_ACCURACY_EVENT             0x042B    //!< event from @ref HCI_EXT_SetLocationingAccuracyCmd
#define HCI_EXT_GET_ACTIVE_CONNECTION_INFO_EVENT           0x042C    //!< event from @ref HCI_EXT_GetActiveConnInfoCmd
#define HCI_EXT_SET_VIRTUAL_ADV_ADDRESS_EVENT              0x042D    //!< event from @ref HCI_EXT_SetVirtualAdvAddrCmd
#define HCI_EXT_COEX_ENABLE_EVENT                          0x042E    //!< event from @ref HCI_EXT_CoexEnableCmd
#define HCI_EXT_SET_HOST_DEF_CHANNEL_CLASSIFICATION_EVENT  0x042F    //!< event from @ref HCI_EXT_CoexEnableCmd
#define HCI_EXT_SET_HOST_CONN_CHANNEL_CLASSIFICATION_EVENT 0x0430    //!< event from @ref HCI_EXT_CoexEnableCmd



#define HCI_EXT_LL_TEST_MODE_EVENT                        0x0470    //!< LL Test Mode
// @endcond // NODOC

// Vendor Specific Event Mask - Texas Instruments Inc specific!
#define NO_FEATURES_ENABLED                               0x00      //!< No Features Enabled
#define PEER_PARAM_REJECT_ENABLED                         0x01      //!< Peer Device Conn Param Reject Mask Enabled

/** @} */ // end of HCI_Constants

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

extern uint8 hciVsEvtMask;

/*
** HCI OSAL API
*/

/// @cond NODOC

/**
 * @fn          HCI_Init
 *
 * @brief       This is the HCI OSAL task initialization routine.
 *
 * @param       taskID The HCI OSAL task identifier.
 */
extern void HCI_Init( uint8 taskID );


/**
 * @fn      HCI_ParamUpdateRjctEvtRegister
 *
 * @brief   Register to receive incoming HCI Events for rejection of peer device
 *          Connection parameter update request.
 *
 * @param   None.
 *
 * @return  void
 */
extern void HCI_ParamUpdateRjctEvtRegister();


/**
 * @fn          HCI_TL_getCmdResponderID
 *
 * @brief       Used by application to intercept and handle HCI TL events and
 *              messages
 *
 * @param       taskID The HCI OSAL task identifier.
 */
extern void HCI_TL_getCmdResponderID( uint8 taskID );


/**
 * @fn          HCI_ProcessEvent
 *
 * @brief       This is the HCI OSAL task process event handler.
 *
 * @param       taskID The HCI OSAL task identifier.
 * @param       events HCI OSAL task events.
 *
 * @return      Unprocessed events.
 */
extern uint16 HCI_ProcessEvent( uint8  task_id,
                                uint16 events );


/// @endcond //NODOC

#ifdef __cplusplus
}
#endif

#endif /* HCI_TL_H */
