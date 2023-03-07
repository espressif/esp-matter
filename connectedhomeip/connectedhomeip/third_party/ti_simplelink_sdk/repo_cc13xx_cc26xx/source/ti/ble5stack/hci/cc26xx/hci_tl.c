/******************************************************************************

 @file  hci_tl.c

 @brief This file includes implementation for HCI task, event handler,
        HCI Command, Data, and Event procoessing and sending, for the
        BLE Transport Layer.

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

/*******************************************************************************
 * INCLUDES
 */

#include "npi.h"
#include "hci_event.h"
#include "hci_tl.h"
#include "osal_bufmgr.h"

#if defined( CC26XX ) || defined( CC13XX )
#include "ll_common.h"
#endif // CC26XX/CC13XX

extern uint8 hciPTMenabled;

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

typedef hciStatus_t (*hciFunc_t)( uint8 *pBuf );

typedef struct
{
  uint16    opCode;
  hciFunc_t hciFunc;
} hciCmdFunc_t;

typedef const hciCmdFunc_t cmdPktTable_t;

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

uint8 hciTaskID;
uint8 hciTestTaskID;
//
uint8 hciGapTaskID;
uint8 hciL2capTaskID;
uint8 hciSmpTaskID;
//
uint8 hciVsEvtMask;

/*
** Controller Prototypes
*/

#ifndef ICALL_LITE

// Serial Port Related
void hciSerialPacketParser( uint8 port, uint8 event );
void hciProcessHostToCtrlCmd( hciPacket_t *pBuf );
void hciProcessHostToCtrlData( hciDataPacket_t *pMsg );
void hciProcessCtrlToHost( hciPacket_t *pBuf );

// Linker Control Commands
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
hciStatus_t hciDisconnect                          ( uint8 *pBuf );
hciStatus_t hciReadRemoteVersionInfo               ( uint8 *pBuf );
#endif // ADV_CONN_CFG | INIT_CFG

// Controller and Baseband Commands
hciStatus_t hciSetEventMask                        ( uint8 *pBuf );
hciStatus_t hciSetEventMaskPage2                   ( uint8 *pBuf );
hciStatus_t hciReset                               ( uint8 *pBuf );
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
hciStatus_t hciReadTransmitPowerLevel              ( uint8 *pBuf );
hciStatus_t hciSetControllerToHostFlowCtrl         ( uint8 *pBuf );
hciStatus_t hciHostBufferSize                      ( uint8 *pBuf );
hciStatus_t hciHostNumCompletedPkt                 ( uint8 *pBuf );
#endif // ADV_CONN_CFG | INIT_CFG

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
hciStatus_t hciReadAuthPayloadTimeout              ( uint8 *pBuf );
hciStatus_t hciWriteAuthPayloadTimeout             ( uint8 *pBuf );
#endif // ADV_CONN_CFG | INIT_CFG

// Information Parameters
hciStatus_t hciReadLocalVersionInfo                ( uint8 *pBuf );
hciStatus_t hciReadLocalSupportedCommands          ( uint8 *pBuf );
hciStatus_t hciReadLocalSupportedFeatures          ( uint8 *pBuf );
hciStatus_t hciReadBDADDR                          ( uint8 *pBuf );
hciStatus_t hciReadRssi                            ( uint8 *pBuf );

// LE Commands
hciStatus_t hciLESetEventMask                      ( uint8 *pBuf );
hciStatus_t hciLEReadBufSize                       ( uint8 *pBuf );
hciStatus_t hciLEReadLocalSupportedFeatures        ( uint8 *pBuf );
hciStatus_t hciLESetRandomAddr                     ( uint8 *pBuf );
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
hciStatus_t hciLESetAdvParam                       ( uint8 *pBuf );
hciStatus_t hciLESetAdvData                        ( uint8 *pBuf );
hciStatus_t hciLESetScanRspData                    ( uint8 *pBuf );
hciStatus_t hciLESetAdvEnab                        ( uint8 *pBuf );
hciStatus_t hciLEReadAdvChanTxPower                ( uint8 *pBuf );
#endif // ADV_NCONN_CFG | ADV_CONN_CFG
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
hciStatus_t hciLESetScanParam                      ( uint8 *pBuf );
hciStatus_t hciLESetScanEnable                     ( uint8 *pBuf );
#endif // SCAN_CFG
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
hciStatus_t hciLECreateConn                        ( uint8 *pBuf );
hciStatus_t hciLECreateConnCancel                  ( uint8 *pBuf );
#endif // INIT_CFG
hciStatus_t hciLEReadWhiteListSize                 ( uint8 *pBuf );
hciStatus_t hciLEClearWhiteList                    ( uint8 *pBuf );
hciStatus_t hciLEAddWhiteList                      ( uint8 *pBuf );
hciStatus_t hciLERemoveWhiteList                   ( uint8 *pBuf );
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
hciStatus_t hciLEConnUpdate                        ( uint8 *pBuf );
#endif // ADV_CONN_CFG | INIT_CFG
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
hciStatus_t hciLESetHostChanClass                  ( uint8 *pBuf );
hciStatus_t hciLEReadChanMap                       ( uint8 *pBuf );
hciStatus_t hciLEReadRemoteUsedFeatures            ( uint8 *pBuf );
#endif // INIT_CFG
hciStatus_t hciLEEncrypt                           ( uint8 *pBuf );
hciStatus_t hciLERand                              ( uint8 *pBuf );
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
hciStatus_t hciLEStartEncrypt                      ( uint8 *pBuf );
#endif // INIT_CFG
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG)
hciStatus_t hciLELtkReqReply                       ( uint8 *pBuf );
hciStatus_t hciLELtkReqNegReply                    ( uint8 *pBuf );
#endif // ADV_CONN_CFG
hciStatus_t hciLEReadSupportedStates               ( uint8 *pBuf );
hciStatus_t hciLEReceiverTest                      ( uint8 *pBuf );
hciStatus_t hciLETransmitterTest                   ( uint8 *pBuf );
hciStatus_t hciLETestEnd                           ( uint8 *pBuf );

// V4.1
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
hciStatus_t hciLERemoteConnParamReqReply           ( uint8 *pBuf );
hciStatus_t hciLERemoteConnParamReqNegReply        ( uint8 *pBuf );
#endif // (ADV_CONN_CFG | INIT_CFG)

// V4.2 - Extended Data Length
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
hciStatus_t hciLESetDataLen                        ( uint8 *pBuf );
hciStatus_t hciLEReadSuggestedDefaultDataLen       ( uint8 *pBuf );
hciStatus_t hciLEWriteSuggestedDefaultDataLen      ( uint8 *pBuf );
hciStatus_t hciLEReadMaxDataLen                    ( uint8 *pBuf );
hciStatus_t hciExtSetMaxDataLen                    ( uint8 *pBuf );
#endif // (ADV_CONN_CFG | INIT_CFG)

// V4.2 - Privacy 1.2
hciStatus_t hciLEAddDeviceToResolvingList          ( uint8 *pBuf );
hciStatus_t hciLERemoveDeviceFromResolvingList     ( uint8 *pBuf );
hciStatus_t hciLEClearResolvingList                ( uint8 *pBuf );
hciStatus_t hciLEReadResolvingListSize             ( uint8 *pBuf );
hciStatus_t hciLEReadPeerResolvableAddress         ( uint8 *pBuf );
hciStatus_t hciLEReadLocalResolvableAddress        ( uint8 *pBuf );
hciStatus_t hciLESetAddressResolutionEnable        ( uint8 *pBuf );
hciStatus_t hciLESetResolvablePrivateAddressTimeout ( uint8 *pBuf );
hciStatus_t hciLESetPrivacyMode                    ( uint8 *pBuf );

// 4.2 - Secure Connections
hciStatus_t hciLEReadLocalP256PubicKey             ( uint8 *pBuf );
hciStatus_t hciLEGenerateDHKey                     ( uint8 *pBuf );

// V5.0 - 2M and Coded PHY
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG)) &&  \
    defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
hciStatus_t hciLEReadPhy                           ( uint8 *pBuf );
hciStatus_t hciLESetDefaultPhy                     ( uint8 *pBuf );
hciStatus_t hciLESetPhy                            ( uint8 *pBuf );
hciStatus_t hciLEEnhancedRxTest                    ( uint8 *pBuf );
hciStatus_t hciLEEnhancedTxTest                    ( uint8 *pBuf );
#endif // (PHY_2MBPS_CFG | PHY_LR_CFG) & (ADV_CONN_CFG | INIT_CFG)

hciStatus_t HCI_LE_ReadTxPowerCmd                  ( uint8 *pBuf );
hciStatus_t HCI_LE_ReadRfPathCompCmd               ( uint8 *pBuf );
hciStatus_t HCI_LE_WriteRfPathCompCmd              ( uint8 *pBuf );

hciStatus_t hciLESetConnectionCteReceiveParams     ( uint8 *pBuf );
hciStatus_t hciLESetConnectionCteTransmitParams    ( uint8 *pBuf );
hciStatus_t hciLESetConnectionCteRequestEnable     ( uint8 *pBuf );
hciStatus_t hciLESetConnectionCteResponseEnable    ( uint8 *pBuf );
hciStatus_t hciLEReadAntennaInformation            ( uint8 *pBuf );

hciStatus_t hciLESetPeriodicAdvParams              ( uint8 *pBuf );
hciStatus_t hciLESetPeriodicAdvData                ( uint8 *pBuf );
hciStatus_t hciLESetPeriodicAdvEnable              ( uint8 *pBuf );
hciStatus_t hciLESetConnectionlessCteTransmitParams( uint8 *pBuf );
hciStatus_t hciLESetConnectionlessCteTransmitEnable( uint8 *pBuf );
hciStatus_t hciLEPeriodicAdvCreateSync             ( uint8 *pBuf );
hciStatus_t hciLEPeriodicAdvCreateSyncCancel       ( uint8 *pBuf );
hciStatus_t hciLEPeriodicAdvTerminateSync          ( uint8 *pBuf );
hciStatus_t hciLEAddDeviceToPeriodicAdvList        ( uint8 *pBuf );
hciStatus_t hciLERemoveDeviceFromPeriodicAdvList   ( uint8 *pBuf );
hciStatus_t hciLEClearPeriodicAdvList              ( uint8 *pBuf );
hciStatus_t hciLEReadPeriodicAdvListSize           ( uint8 *pBuf );
hciStatus_t hciLESetPeriodicAdvReceiveEnable       ( uint8 *pBuf );
hciStatus_t hciLESetConnectionlessIqSamplingEnable ( uint8 *pBuf );

// Vendor Specific Commands
hciStatus_t hciExtSetRxGain                        ( uint8 *pBuf );
hciStatus_t hciExtSetTxPower                       ( uint8 *pBuf );
hciStatus_t hciExtExtendRfRange                    ( uint8 *pBuf );
hciStatus_t hciExtHaltDuringRf                     ( uint8 *pBuf );
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
hciStatus_t hciExtOnePktPerEvt                     ( uint8 *pBuf );
#endif // ADV_CONN_CFG | INIT_CFG
hciStatus_t hciExtClkDivOnHalt                     ( uint8 *pBuf );
hciStatus_t hciExtDeclareNvUsage                   ( uint8 *pBuf );
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
hciStatus_t hciExtDelayPostProc                    ( uint8 *pBuf );
#endif // ADV_CONN_CFG | INIT_CFG
hciStatus_t hciExtDecrypt                          ( uint8 *pBuf );
hciStatus_t hciExtSetLocalSupportedFeatures        ( uint8 *pBuf );
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG)
hciStatus_t hciExtSetFastTxResponseTime            ( uint8 *pBuf );
hciStatus_t hciExtSetSlaveLatencyOverride          ( uint8 *pBuf );
#endif // ADV_CONN_CFG
#if !defined(CTRL_V50_CONFIG)
hciStatus_t hciExtModemTestTx                      ( uint8 *pBuf );
hciStatus_t hciExtModemHopTestTx                   ( uint8 *pBuf );
hciStatus_t hciExtModemtestRx                      ( uint8 *pBuf );
hciStatus_t hciExtEndModemTest                     ( uint8 *pBuf );
#endif // !CTRL_V50_CONFIG
hciStatus_t hciExtSetBDADDR                        ( uint8 *pBuf );
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
hciStatus_t hciExtSetSCA                           ( uint8 *pBuf );
#endif // ADV_CONN_CFG | INIT_CFG
hciStatus_t hciExtEnablePTM                        ( uint8 *pBuf );
hciStatus_t hciExtSetFreqTune                      ( uint8 *pBuf );
hciStatus_t hciExtSaveFreqTune                     ( uint8 *pBuf );
hciStatus_t hciExtSetMaxDtmTxPower                 ( uint8 *pBuf );
hciStatus_t hciExtMapPmIoPort                      ( uint8 *pBuf );
hciStatus_t hciExtBuildRevision                    ( uint8 *pBuf );
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_NCONN_CFG )
hciStatus_t hciExtSetVirtualAdvAddr                ( uint8 *pBuf );
#endif // ADV_NCONN_CFG
hciStatus_t hciExtDelaySleep                       ( uint8 *pBuf );
hciStatus_t hciExtResetSystem                      ( uint8 *pBuf );
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
hciStatus_t hciExtDisconnectImmed                  ( uint8 *pBuf );
hciStatus_t hciExtPER                              ( uint8 *pBuf );
hciStatus_t hciExtOverlappedProcessing             ( uint8 *pBuf );
hciStatus_t hciExtNumComplPktsLimit                ( uint8 *pBuf );
hciStatus_t hciExtGetConnInfo                      ( uint8 *pBuf );
#endif // ADV_CONN_CFG | INIT_CFG
#if defined(BLE_VS_FEATURES) && (BLE_VS_FEATURES & SCAN_REQ_RPT_CFG) &&        \
    defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
hciStatus_t hciExtScanReqRpt                       ( uint8 *pBuf );
#endif // SCAN_REQ_RPT_CFG & (ADV_NCONN_CFG | ADV_CONN_CFG)
#ifdef LL_TEST_MODE
hciStatus_t hciExtLLTestMode                       ( uint8 *pBuf );
#endif // LL_TEST_MODE

// handle how the transport layer is built for a source build
#if !defined(HCI_TL_FULL) && !defined(HCI_TL_NONE)
  #if defined(CC2540) || defined(CC2541) || defined(CC2541S)
    #if defined(HAL_UART) || defined(HAL_UART_SPI)
      #if (HAL_UART == TRUE) || (HAL_UART_SPI != 0)
        #define HCI_TL_FULL
      #else // HAL_UART==FALSE/HAL_UART_SPI==0 or no/other defined value
        #define HCI_TL_NONE
      #endif // HAL_UART==TRUE || HAL_UART_SPI!=0
    #endif // HAL_UART || HAL_UART_SPI
  #else // CC26XX
    #define HCI_TL_NONE
  #endif
#endif // !HCI_TL_FULL && !HCI_TL_NONE

#if defined(HCI_TL_FULL)
// HCI Packet Opcode Jump Table
cmdPktTable_t hciCmdTable[] =
{
  // Linker Control Commands
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  {HCI_DISCONNECT                           , hciDisconnect                    },
  {HCI_READ_REMOTE_VERSION_INFO             , hciReadRemoteVersionInfo         },
#endif // ADV_CONN_CFG | INIT_CFG

  // Controller and Baseband Commands
  {HCI_SET_EVENT_MASK                       , hciSetEventMask                  },
  {HCI_SET_EVENT_MASK_PAGE_2                , hciSetEventMaskPage2             },
  {HCI_RESET                                , hciReset                         },
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  {HCI_READ_TRANSMIT_POWER                  , hciReadTransmitPowerLevel        },
  {HCI_SET_CONTROLLER_TO_HOST_FLOW_CONTROL  , hciSetControllerToHostFlowCtrl   },
  {HCI_HOST_BUFFER_SIZE                     , hciHostBufferSize                },
  {HCI_HOST_NUM_COMPLETED_PACKETS           , hciHostNumCompletedPkt           },
#endif // ADV_CONN_CFG | INIT_CFG

// Informational Parameters
  {HCI_READ_LOCAL_VERSION_INFO              , hciReadLocalVersionInfo          },
  {HCI_READ_LOCAL_SUPPORTED_COMMANDS        , hciReadLocalSupportedCommands    },
  {HCI_READ_LOCAL_SUPPORTED_FEATURES        , hciReadLocalSupportedFeatures    },
  {HCI_READ_BDADDR                          , hciReadBDADDR                    },
  {HCI_READ_RSSI                            , hciReadRssi                      },

  // LE Commands
  {HCI_LE_SET_EVENT_MASK                    , hciLESetEventMask                },
  {HCI_LE_READ_BUFFER_SIZE                  , hciLEReadBufSize                 },
  {HCI_LE_READ_LOCAL_SUPPORTED_FEATURES     , hciLEReadLocalSupportedFeatures  },
  {HCI_LE_SET_RANDOM_ADDR                   , hciLESetRandomAddr               },
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
  {HCI_LE_SET_ADV_PARAM                     , hciLESetAdvParam                 },
  {HCI_LE_SET_ADV_DATA                      , hciLESetAdvData                  },
  {HCI_LE_SET_SCAN_RSP_DATA                 , hciLESetScanRspData              },
  {HCI_LE_SET_ADV_ENABLE                    , hciLESetAdvEnab                  },
  {HCI_LE_READ_ADV_CHANNEL_TX_POWER         , hciLEReadAdvChanTxPower          },
#endif // ADV_NCONN_CFG | ADV_CONN_CFG
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
  {HCI_LE_SET_SCAN_PARAM                    , hciLESetScanParam                },
  {HCI_LE_SET_SCAN_ENABLE                   , hciLESetScanEnable               },
#endif // SCAN_CFG
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
  {HCI_LE_CREATE_CONNECTION                 , hciLECreateConn                  },
  {HCI_LE_CREATE_CONNECTION_CANCEL          , hciLECreateConnCancel            },
#endif // INIT_CFG
  {HCI_LE_READ_WHITE_LIST_SIZE              , hciLEReadWhiteListSize           },
  {HCI_LE_CLEAR_WHITE_LIST                  , hciLEClearWhiteList              },
  {HCI_LE_ADD_WHITE_LIST                    , hciLEAddWhiteList                },
  {HCI_LE_REMOVE_WHITE_LIST                 , hciLERemoveWhiteList             },
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  {HCI_LE_CONNECTION_UPDATE                 , hciLEConnUpdate                  },
#endif // ADV_CONN_CFG | INIT_CFG
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
  {HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION   , hciLESetHostChanClass            },
  {HCI_LE_READ_CHANNEL_MAP                  , hciLEReadChanMap                 },
  {HCI_LE_READ_REMOTE_USED_FEATURES         , hciLEReadRemoteUsedFeatures      },
#endif // INIT_CFG
  {HCI_LE_ENCRYPT                           , hciLEEncrypt                     },
  {HCI_LE_RAND                              , hciLERand                        },
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
  {HCI_LE_START_ENCRYPTION                  , hciLEStartEncrypt                },
#endif // INIT_CFG
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG)
  {HCI_LE_LTK_REQ_REPLY                     , hciLELtkReqReply                 },
  {HCI_LE_LTK_REQ_NEG_REPLY                 , hciLELtkReqNegReply              },
#endif // ADV_CONN_CFG
  {HCI_LE_READ_SUPPORTED_STATES             , hciLEReadSupportedStates         },
  {HCI_LE_RECEIVER_TEST                     , hciLEReceiverTest                },
  {HCI_LE_TRANSMITTER_TEST                  , hciLETransmitterTest             },
  {HCI_LE_TEST_END                          , hciLETestEnd                     },

// V4.1

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  {HCI_READ_AUTH_PAYLOAD_TIMEOUT            , hciReadAuthPayloadTimeout        },
  {HCI_WRITE_AUTH_PAYLOAD_TIMEOUT           , hciWriteAuthPayloadTimeout       },
#endif // (ADV_CONN_CFG | INIT_CFG)

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  {HCI_LE_REMOTE_CONN_PARAM_REQ_REPLY       , hciLERemoteConnParamReqReply     },
  {HCI_LE_REMOTE_CONN_PARAM_REQ_NEG_REPLY   , hciLERemoteConnParamReqNegReply  },
#endif // (ADV_CONN_CFG | INIT_CFG)

// V4.2 - Extended Data Length
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  {HCI_LE_SET_DATA_LENGTH                   ,  hciLESetDataLen                 },
  {HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH,  hciLEReadSuggestedDefaultDataLen},
  {HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH, hciLEWriteSuggestedDefaultDataLen},
  {HCI_LE_READ_MAX_DATA_LENGTH              ,  hciLEReadMaxDataLen             },
#endif // (ADV_CONN_CFG | INIT_CFG)

// V4.2 - Secure Connections
  {HCI_LE_READ_LOCAL_P256_PUBLIC_KEY        ,  hciLEReadLocalP256PubicKey      },
  {HCI_LE_GENERATE_DHKEY                    ,  hciLEGenerateDHKey              },

// V4.2 - Privacy 1.2
  {HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST      ,  hciLEAddDeviceToResolvingList   },
  {HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST ,  hciLERemoveDeviceFromResolvingList},
  {HCI_LE_CLEAR_RESOLVING_LIST              ,  hciLEClearResolvingList         },
  {HCI_LE_READ_RESOLVING_LIST_SIZE          ,  hciLEReadResolvingListSize      },
  {HCI_LE_READ_PEER_RESOLVABLE_ADDRESS      ,  hciLEReadPeerResolvableAddress  },
  {HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS     ,  hciLEReadLocalResolvableAddress },
  {HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE     ,  hciLESetAddressResolutionEnable },
  {HCI_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT,  hciLESetResolvablePrivateAddressTimeout},
  {HCI_LE_SET_PRIVACY_MODE                  ,  hciLESetPrivacyMode             },

// V5.0 - 2M and Coded PHY
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG)) &&  \
    defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  {HCI_LE_READ_PHY                          , hciLEReadPhy                     },
  {HCI_LE_SET_DEFAULT_PHY                   , hciLESetDefaultPhy               },
  {HCI_LE_SET_PHY                           , hciLESetPhy                      },
  {HCI_LE_ENHANCED_RECEIVER_TEST            , hciLEEnhancedRxTest              },
  {HCI_LE_ENHANCED_TRANSMITTER_TEST         , hciLEEnhancedTxTest              },
#endif // (PHY_2MBPS_CFG | PHY_LR_CFG) & (ADV_CONN_CFG | INIT_CFG)

  {HCI_LE_READ_TX_POWER                     , hciLEReadTxPower                 },
  {HCI_LE_READ_RF_PATH_COMPENSATION         , hciLEReadRfPathCompCmd           },
  {HCI_LE_WRITE_RF_PATH_COMPENSATION        , hciLEWriteRfPathCompCmd          },

  (HCI_LE_SET_CONNECTION_CTE_RECEIVE_PARAMS , hciLESetConnectionCteReceiveParams},
  (HCI_LE_SET_CONNECTION_CTE_TRANSMIT_PARAMS, hciLESetConnectionCteTransmitParams},
  (HCI_LE_SET_CONNECTION_CTE_REQUEST_ENABLE , hciLESetConnectionCteRequestEnable},
  (HCI_LE_SET_CONNECTION_CTE_RESPONSE_ENABLE, hciLESetConnectionCteResponseEnable},
  (HCI_LE_READ_ANTENNA_INFORMATION,           hciLEReadAntennaInformation},

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
  (HCI_LE_SET_PERIODIC_ADV_PARAMETERS        ,hciLESetPeriodicAdvParams},
  (HCI_LE_SET_PERIODIC_ADV_DATA              ,hciLESetPeriodicAdvData},
  (HCI_LE_SET_PERIODIC_ADV_ENABLE            ,hciLESetPeriodicAdvEnable},
  (HCI_LE_SET_CONNECTIONLESS_CTE_TRANSMIT_PARAMS,hciLESetConnectionlessCteTransmitParams},
  (HCI_LE_SET_CONNECTIONLESS_CTE_TRANSMIT_ENABLE,hciLESetConnectionlessCteTransmitEnable},
#endif // ADV_NCONN_CFG | ADV_CONN_CFG
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
  (HCI_LE_PERIODIC_ADV_CREATE_SYNC            ,hciLEPeriodicAdvCreateSync          },
  (HCI_LE_PERIODIC_ADV_CREATE_SYNC_CANCEL     ,hciLEPeriodicAdvCreateSyncCancel    },
  (HCI_LE_PERIODIC_ADV_TERMINATE_SYNC         ,hciLEPeriodicAdvTerminateSync       },
  (HCI_LE_ADD_DEVICE_TO_PERIODIC_ADV_LIST     ,hciLEAddDeviceToPeriodicAdvList     },
  (HCI_LE_REMOVE_DEVICE_FROM_PERIODIC_ADV_LIST,hciLERemoveDeviceFromPeriodicAdvList},
  (HCI_LE_CLEAR_PERIODIC_ADV_LIST             ,hciLEClearPeriodicAdvList           },
  (HCI_LE_READ_PERIODIC_ADV_LIST_SIZE         ,hciLEReadPeriodicAdvListSize        },
  (HCI_LE_SET_PERIODIC_ADV_RECEIVE_ENABLE     ,hciLESetPeriodicAdvReceiveEnable    },
  (HCI_LE_SET_CONNECTIONLESS_IQ_SAMPLING_ENABLE,hciLESetConnectionlessIqSamplingEnable},
#endif

  // Vendor Specific Commands
  {HCI_EXT_SET_RX_GAIN                      , hciExtSetRxGain                  },
  {HCI_EXT_SET_TX_POWER                     , hciExtSetTxPower                 },
  {HCI_EXT_EXTEND_RF_RANGE                  , hciExtExtendRfRange              },
  {HCI_EXT_HALT_DURING_RF                   , hciExtHaltDuringRf               },
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  {HCI_EXT_ONE_PKT_PER_EVT                  , hciExtOnePktPerEvt               },
#endif // ADV_CONN_CFG | INIT_CFG
  {HCI_EXT_CLK_DIVIDE_ON_HALT               , hciExtClkDivOnHalt               },
  {HCI_EXT_DECLARE_NV_USAGE                 , hciExtDeclareNvUsage             },
  {HCI_EXT_DECRYPT                          , hciExtDecrypt                    },
  {HCI_EXT_SET_LOCAL_SUPPORTED_FEATURES     , hciExtSetLocalSupportedFeatures  },
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG)
  {HCI_EXT_SET_FAST_TX_RESP_TIME            , hciExtSetFastTxResponseTime      },
  {HCI_EXT_OVERRIDE_SL                      , hciExtSetSlaveLatencyOverride    },
#endif // ADV_CONN_CFG
#if !defined(CTRL_V50_CONFIG)
  {HCI_EXT_MODEM_TEST_TX                    , hciExtModemTestTx                },
  {HCI_EXT_MODEM_HOP_TEST_TX                , hciExtModemHopTestTx             },
  {HCI_EXT_MODEM_TEST_RX                    , hciExtModemtestRx                },
  {HCI_EXT_END_MODEM_TEST                   , hciExtEndModemTest               },
#endif // !CTRL_V50_CONFIG
  {HCI_EXT_SET_BDADDR                       , hciExtSetBDADDR                  },
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  {HCI_EXT_SET_SCA                          , hciExtSetSCA                     },
#endif // ADV_CONN_CFG | INIT_CFG
  {HCI_EXT_SET_MAX_DTM_TX_POWER             , hciExtSetMaxDtmTxPower           },
  {HCI_EXT_MAP_PM_IO_PORT                   , hciExtMapPmIoPort                },
  {HCI_EXT_SET_FREQ_TUNE                    , hciExtSetFreqTune                },
  {HCI_EXT_SAVE_FREQ_TUNE                   , hciExtSaveFreqTune               },
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  {HCI_EXT_DISCONNECT_IMMED                 , hciExtDisconnectImmed            },
  {HCI_EXT_PER                              , hciExtPER                        },
  {HCI_EXT_OVERLAPPED_PROCESSING            , hciExtOverlappedProcessing       },
  {HCI_EXT_NUM_COMPLETED_PKTS_LIMIT         , hciExtNumComplPktsLimit          },
  {HCI_EXT_GET_CONNECTION_INFO              , hciExtGetConnInfo                },
#endif // ADV_CONN_CFG | INIT_CFG
  {HCI_EXT_BUILD_REVISION                   , hciExtBuildRevision              },
  {HCI_EXT_DELAY_SLEEP                      , hciExtDelaySleep                 },
  // TEMP: OVERLAPPED PROCESSING HOLDER
  {HCI_EXT_RESET_SYSTEM                     , hciExtResetSystem                },
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_NCONN_CFG )
  {HCI_EXT_LE_SET_EXT_VIRTUAL_ADV_ADDRESS   , hciExtSetVirtualAdvAddr          },
#endif // ADV_NCONN_CFG
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  {HCI_EXT_SET_MAX_DATA_LENGTH              , hciExtSetMaxDataLen             },
#endif // (ADV_CONN_CFG | INIT_CFG)
#ifdef LL_TEST_MODE
  {HCI_EXT_LL_TEST_MODE                     , hciExtLLTestMode                 },
#endif // LL_TEST_MODE
  {HCI_EXT_READ_RAND_ADDR                   , hciExtReadRandAddr               },

  //Channel map Classification API
  {HCI_EXT_SET_HOST_DEFAULT_CHANNEL_CLASSIFICATION    , hciExtSetHostDefChanClass },
  {HCI_EXT_SET_HOST_CONNECTION_CHANNEL_CLASSIFICATION , hciExtSetHostConnChanClass},

  // Last Table Entry Delimiter
  {0xFFFF                                   , NULL                             }
};

#else // either HCI_TL_NONE or nothing is defined
// No Transport Layer
cmdPktTable_t hciCmdTable[] =
{
  // Last Table Entry Delimiter
  {0xFFFF                                   , NULL                             }
};

#endif // HCI_TL_FULL

#endif //ICALL_LITE

/*
** HCI OSAL API
*/

/*******************************************************************************
 * This is the HCI OSAL task initialization routine.
 *
 * Public function defined in hci.h.
 */
void HCI_Init( uint8 taskID )
{
  // initialize the task for HCI-Controller
  hciTaskID      = taskID;
  hciTestTaskID  = 0;
  hciGapTaskID   = 0;
  hciL2capTaskID = 0;
  hciSmpTaskID   = 0;

  // reset the Bluetooth and the BLE event mask bits
  hciInitEventMasks();

  // set again here to avoid patch. Updated LE_EVT_MASK_BYTE2 to include CTE related events.
  pBleEvtMask[2] = LE_EVT_MASK_BYTE2;

  // disable PTM runtime flag
  hciPTMenabled = FALSE;

  // disable HCI Vendor Specific Event Mask
  hciVsEvtMask = NO_FEATURES_ENABLED;

  return;
}

/******************************************************************************
 * @fn      HCI_ParamUpdateRjctEvtRegister
 *
 * @brief   Register to receive incoming HCI Events for rejection of peer device
 *          Connection parameter update request.
 *
 * @param   None.
 *
 * @return  void
 */
void HCI_ParamUpdateRjctEvtRegister()
{
  hciVsEvtMask |= PEER_PARAM_REJECT_ENABLED;
}

/*******************************************************************************
 * This is the application's registration as the controller of the HCI TL.
 *
 * Public function defined in hci.h.
 */
void HCI_TL_getCmdResponderID( uint8 taskID )
{
  // Override the hciTaskID.
  hciTaskID = taskID;
}

/*******************************************************************************
 * This is the HCI OSAL task process event handler.
 *
 * Public function defined in hci.h.
 */
uint16 HCI_ProcessEvent( uint8 task_id, uint16 events )
{
  osal_event_hdr_t *pMsg;

  // check for system messages
  if ( events & SYS_EVENT_MSG )
  {
    if (( pMsg = (osal_event_hdr_t *)osal_msg_receive(hciTaskID) ))
    {
#if !defined(HCI_TL_NONE) && !defined(ICALL_LITE)
      switch( pMsg->event )
      {
      case HCI_HOST_TO_CTRL_DATA_EVENT:
        // process HCI data packet
        hciProcessHostToCtrlData( (hciDataPacket_t *)pMsg );
        break;

      case HCI_HOST_TO_CTRL_CMD_EVENT:
        // process HCI command packet
        hciProcessHostToCtrlCmd( (hciPacket_t *)pMsg );
        break;

      case HCI_CTRL_TO_HOST_EVENT:
        hciProcessCtrlToHost( (hciPacket_t *)pMsg );
        break;

      default:
        break;
      }
#else // HCI_TL_NONE && defined(ICALL_LITE)
      if ( (pMsg->event == HCI_HOST_TO_CTRL_DATA_EVENT) ||
           (pMsg->event == HCI_CTRL_TO_HOST_EVENT) )
      {
        // deallocate data
        osal_bm_free( ((hciDataPacket_t *)pMsg)->pData );
      }

      // deallocate the message
      (void)osal_msg_deallocate( (uint8 *)pMsg );
#endif // !HCI_TL_NONE && !ICALL_LITE
    }

    // clear unproccessed events
    events ^= SYS_EVENT_MSG;
  }
  
  // return unprocessed events
  return( events );
}

/*
** HCI Vendor Specific Handlers for Host
*/

/*******************************************************************************
 * Register GAP task ID with HCI.
 *
 * Public function defined in hci.h.
 */
void HCI_TestAppTaskRegister( uint8 taskID )
{
  hciTestTaskID = taskID;
}


/*******************************************************************************
 * Register GAP task ID with HCI.
 *
 * Public function defined in hci.h.
 */
void HCI_GAPTaskRegister( uint8 taskID )
{
  hciGapTaskID = taskID;
}


/*******************************************************************************
 * Register L2CAP task ID with HCI.
 *
 * Public function defined in hci.h.
 */
void HCI_L2CAPTaskRegister( uint8 taskID )
{
  hciL2capTaskID = taskID;
}


/*******************************************************************************
 * Register SMP task ID with HCI.
 *
 * Public function defined in hci.h.
 */
void HCI_SMPTaskRegister( uint8 taskID )
{
  hciSmpTaskID = taskID;
}


#ifndef ICALL_LITE
#if !defined(HCI_TL_NONE)

/*
** Controller Serial Related API
*/

/*******************************************************************************
 * @fn          hciProcessHostToCtrlCmd
 *
 * @brief       This routine handles HCI controller commands received from Host.
 *
 * input parameters
 *
 * @param       pMsg - Pointer to HCI command packet.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void hciProcessHostToCtrlCmd( hciPacket_t *pMsg )
{
  uint16 cmdOpCode;
  uint8  status;
  uint8  i = 0;

  // retrieve opcode
  cmdOpCode = BUILD_UINT16 (pMsg->pData[1], pMsg->pData[2]);

  // lookup corresponding function
  while ((hciCmdTable[i].opCode != 0xFFFF) && (hciCmdTable[i].hciFunc != NULL))
  {
    // there's a valid entry at this index, but check if it's the one we want
    if (hciCmdTable[i].opCode == cmdOpCode)
    {
      // it is, so jump to this function
      (void)(hciCmdTable[i].hciFunc)(&pMsg->pData[4]);

      // done
      break;
    }

    // next...
    i++;
  }

  // check if a matching opcode was found
  if ((hciCmdTable[i].opCode == 0xFFFF) && (hciCmdTable[i].hciFunc == NULL))
  {
    // none found, so return error
    status = HCI_ERROR_CODE_UNKNOWN_HCI_CMD;

    HCI_CommandCompleteEvent ( cmdOpCode, 1, &status);
  }

  // deallocate the message
  (void)osal_msg_deallocate( (uint8 *)pMsg );

  return;
}


/*******************************************************************************
 * @fn          hciProcessHostToCtrlData
 *
 * @brief       This routine handles HCI controller data received from Host.
 *
 *              HCI Data Packet frame format, and size in bytes:
 *              | Packet Type (1) | Handle (2) | Length (2) | Data (N) |
 *
 * input parameters
 *
 * @param       pMsg - Pointer to HCI data packet.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void hciProcessHostToCtrlData( hciDataPacket_t *pMsg )
{
  // two types of data possible
  switch( pMsg->pktType )
  {
#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
    case HCI_ACL_DATA_PACKET:
      // check for a problem sending data
      // Note: Success either means the packet was sent and the buffer was
      //       freed, or the packet was queued for a later transmission.
      if ( HCI_SendDataPkt( pMsg->connHandle,
                            pMsg->pbFlag,
                            pMsg->pktLen,
                            pMsg->pData ) != HCI_SUCCESS )
      {
        // packet wasn't sent or queued, so free the user's data
        osal_bm_free( (void *)pMsg->pData );
      }
      break;

#else // CTRL_CONFIG=(!ADV_CONN_CFG & !INIT_CFG)
    case HCI_ACL_DATA_PACKET:
        // data not supported for this configuration
        osal_bm_free( (void *)pMsg->pData );
      break;

#endif // CTRL_CONFIG=(ADV_CONN_CFG | INIT_CFG)

    case HCI_SCO_DATA_PACKET:
      // ASSERT
      // DROP THROUGH
      //lint -fallthrough

    default:
      break;
  }

  // deallocate message
  (void)osal_msg_deallocate( (uint8 *)pMsg );

  return;
}


/*******************************************************************************
 * @fn          hciProcessCtrlToHost
 *
 * @brief       This routine handles sending an HCI packet out the serial
 *              interface.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to the HCI packet.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void hciProcessCtrlToHost( hciPacket_t *pBuf )
{
  switch (pBuf->pData[0])
  {
    case HCI_ACL_DATA_PACKET:
    case HCI_SCO_DATA_PACKET:

      // send through UART - dual solution
      if ( NPI_WriteTransport( pBuf->pData,
                               HCI_DATA_MIN_LENGTH + BUILD_UINT16(pBuf->pData[3],
                                                                  pBuf->pData[4]) ) == 0 )
      {
        // the data was not written. Restore it in the mesage queue.
        (void)osal_msg_push_front( hciTaskID, (uint8 *)pBuf );

        return; // we're done here!
      }

      // free the packet buffer
      osal_bm_free( pBuf->pData );

      break;

    case HCI_EVENT_PACKET:

      // send event through UART - dual solution
      if ( NPI_WriteTransport( &pBuf->pData[0],
                               HCI_EVENT_MIN_LENGTH + pBuf->pData[2] ) == 0 )
      {
#if defined( CC26XX ) || defined( CC13XX )
        // check if we're no longer booting
        if ( SysBootMsg == INVALID_SYSBOOTMSG )
        {
          // the data was not written, so restore it in the mesage queue
          (void)osal_msg_push_front( hciTaskID, (uint8 *)pBuf );

          return; // we're done here!
        }
        // check if we are still booting
        else if ( SysBootMsg == NULL )
        {
          // save the message until we are able to send this packet after boot
          SysBootMsg = (uint8 *)pBuf;

          return; // we're done here!
        }
        //else toss the message since, at most, only one is saved
#else // !CC26XX/!CC13XX
        // the data was not written, so restore it in the mesage queue
        (void)osal_msg_push_front( hciTaskID, (uint8 *)pBuf );

        return; // we're done here!
#endif // CC26XX
      }
      break;

    default:
      break;
  }

  // deallocate the message if controller only
  (void)osal_msg_deallocate( (uint8 *)pBuf );

  return;
}


/*
** Serial Packet Translation Functions for HCI APIs
*/

#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
/*******************************************************************************
 * @fn          hciDisconnect
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciDisconnect( uint8 *pBuf )
{
  return HCI_DisconnectCmd ( BUILD_UINT16(pBuf[0],
                                          pBuf[1]),
                                          pBuf[2] );
}
#endif // CTRL_CONFIG=(ADV_CONN_CFG | INIT_CFG)


#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
/*******************************************************************************
 * @fn          hciReadRemoteVersionInfo
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciReadRemoteVersionInfo( uint8 *pBuf )
{
  return HCI_ReadRemoteVersionInfoCmd( BUILD_UINT16(pBuf[0],
                                                    pBuf[1]) );
}
#endif // CTRL_CONFIG=(ADV_CONN_CFG | INIT_CFG)


/*******************************************************************************
 * @fn          hciSetEventMask
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciSetEventMask( uint8 *pBuf )
{
  return HCI_SetEventMaskCmd( pBuf );
}


/*******************************************************************************
 * @fn          hciSetEventMaskPage2
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciSetEventMaskPage2( uint8 *pBuf )
{
  return HCI_SetEventMaskPage2Cmd( pBuf );
}


/*******************************************************************************
 * @fn          hciReset
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciReset( uint8 *pBuf )
{
  // unused input parameter; PC-Lint error 715.
  (void)pBuf;

  return HCI_ResetCmd();
}


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          hciReadTransmitPowerLevel
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciReadTransmitPowerLevel( uint8 *pBuf )
{
  return HCI_ReadTransmitPowerLevelCmd ( BUILD_UINT16(pBuf[0],
                                                      pBuf[1]),
                                         pBuf[2] );
}
#endif // ADV_CONN_CFG | INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          hciSetControllerToHostFlowCtrl
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciSetControllerToHostFlowCtrl( uint8 *pBuf )
{
  return HCI_SetControllerToHostFlowCtrlCmd( pBuf[0] );
}
#endif // ADV_CONN_CFG | INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          hciHostBufferSize
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       hostAclPktLen        - Host ACL data packet length.
 * @param       hostSyncPktLen       - Host SCO data packet length .
 * @param       hostTotalNumAclPkts  - Host total number of ACL data packets.
 * @param       hostTotalNumSyncPkts - Host total number of SCO data packets.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciHostBufferSize( uint8 *pBuf )
{
  return HCI_HostBufferSizeCmd( BUILD_UINT16(pBuf[0], pBuf[1]),
                                pBuf[2],
                                BUILD_UINT16(pBuf[3], pBuf[4]),
                                BUILD_UINT16(pBuf[5], pBuf[6]) );
}
#endif // ADV_CONN_CFG | INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          hciHostNumCompletedPkt
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciHostNumCompletedPkt( uint8 *pBuf )
{
  return HCI_HostNumCompletedPktCmd( pBuf[0],
                                     (uint16 *)&pBuf[1],
                                     (uint16 *)&pBuf[3] );
}
#endif // ADV_CONN_CFG | INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          hciReadAuthPayloadTimeout
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciReadAuthPayloadTimeout( uint8 *pBuf )
{
  return HCI_ReadAuthPayloadTimeoutCmd( BUILD_UINT16(pBuf[0], pBuf[1]),
                                        (uint16 *)&pBuf[2] );
}
#endif // ADV_CONN_CFG | INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          hciWriteAuthPayloadTimeout
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciWriteAuthPayloadTimeout( uint8 *pBuf )
{
  return HCI_WriteAuthPayloadTimeoutCmd( BUILD_UINT16(pBuf[0], pBuf[1]),
                                         BUILD_UINT16(pBuf[2], pBuf[3]) );
}
#endif // ADV_CONN_CFG | INIT_CFG


/*******************************************************************************
 * @fn          hciReadLocalVersionInfo
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciReadLocalVersionInfo( uint8 *pBuf )
{
  // unused input parameter; PC-Lint error 715.
  (void)pBuf;

  return HCI_ReadLocalVersionInfoCmd();
}


/*******************************************************************************
 * @fn          hciReadLocalSupportedCommands
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciReadLocalSupportedCommands( uint8 *pBuf )
{
  // unused input parameter; PC-Lint error 715.
  (void)pBuf;

  return HCI_ReadLocalSupportedCommandsCmd();
}


/*******************************************************************************
 * @fn          hciReadLocalSupportedFeatures
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciReadLocalSupportedFeatures( uint8 *pBuf )
{
  // unused input parameter; PC-Lint error 715.
  (void)pBuf;

  return HCI_ReadLocalSupportedFeaturesCmd();
}


/*******************************************************************************
 * @fn          hciReadRssi
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciReadRssi( uint8 *pBuf )
{
  return HCI_ReadRssiCmd( BUILD_UINT16(pBuf[0], pBuf[1]) );
}


/*******************************************************************************
 * @fn          hciLESetEventMask
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetEventMask( uint8 *pBuf )
{
  return HCI_LE_SetEventMaskCmd( pBuf );
}


/*******************************************************************************
 * @fn          hciLEReadBufSize
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadBufSize( uint8 *pBuf )
{
  // unused input parameter; PC-Lint error 715.
  (void)pBuf;

  return HCI_LE_ReadBufSizeCmd();
}


/*******************************************************************************
 * @fn          hciLEReadLocalSupportedFeatures
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadLocalSupportedFeatures( uint8 *pBuf )
{
  // unused input parameter; PC-Lint error 715.
  (void)pBuf;

  return HCI_LE_ReadLocalSupportedFeaturesCmd();
}


/*******************************************************************************
 * @fn          hciLESetRandomAddr
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetRandomAddr( uint8 *pBuf )
{
  // unused input parameter; PC-Lint error 715.
  (void)pBuf;

  return HCI_LE_SetRandomAddressCmd( pBuf );
}


#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG))
/*******************************************************************************
 * @fn          hciLESetAdvParam
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetAdvParam( uint8 *pBuf )
{
  return HCI_LE_SetAdvParamCmd( BUILD_UINT16(pBuf[0], pBuf[1]),
                                BUILD_UINT16(pBuf[2], pBuf[3]),
                                pBuf[4],
                                pBuf[5],
                                pBuf[6],
                                &pBuf[7],
                                pBuf[13],
                                pBuf[14] );
}
#endif // CTRL_CONFIG=(ADV_NCONN_CFG | ADV_CONN_CFG)


#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG))
/*******************************************************************************
 * @fn          hciLESetAdvData
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetAdvData( uint8 *pBuf )
{
  return HCI_LE_SetAdvDataCmd( pBuf[0],
                               &pBuf[1] );
}
#endif // CTRL_CONFIG=(ADV_NCONN_CFG | ADV_CONN_CFG)


#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG))
/*******************************************************************************
 * @fn          hciLESetScanRspData
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetScanRspData( uint8 *pBuf )
{
  return HCI_LE_SetScanRspDataCmd( pBuf[0],
                                   &pBuf[1] );
}
#endif // CTRL_CONFIG=(ADV_NCONN_CFG | ADV_CONN_CFG)


#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG))
/*******************************************************************************
 * @fn          hciLESetAdvEnab
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetAdvEnab( uint8 *pBuf )
{
  return HCI_LE_SetAdvEnableCmd( pBuf[0] );
}
#endif // CTRL_CONFIG=(ADV_NCONN_CFG | ADV_CONN_CFG)


#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG))
/*******************************************************************************
 * @fn          hciLEReadAdvChanTxPower
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadAdvChanTxPower( uint8 *pBuf )
{
  // unused input parameter; PC-Lint error 715.
  (void)pBuf;

  return HCI_LE_ReadAdvChanTxPowerCmd();
}
#endif // CTRL_CONFIG=(ADV_NCONN_CFG | ADV_CONN_CFG)

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_NCONN_CFG )
hciStatus_t hciExtSetVirtualAdvAddr( uint8 *pBuf)
{
  return HCI_EXT_SetVirtualAdvAddrCmd( pBuf[0],
                                       pBuf[1] );
}
#endif //CTRL_CONFIG=(ADV_NCONN_CFG)

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
/*******************************************************************************
 * @fn          hciLESetScanParam
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetScanParam( uint8 *pBuf )
{
  return HCI_LE_SetScanParamCmd( pBuf[0],
                                 BUILD_UINT16(pBuf[1], pBuf[2]),
                                 BUILD_UINT16(pBuf[3], pBuf[4]),
                                 pBuf[5],
                                 pBuf[6] );
}
#endif // CTRL_CONFIG=SCAN_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
/*******************************************************************************
 * @fn          hciLESetScanEnable
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetScanEnable( uint8 *pBuf )
{
  return HCI_LE_SetScanEnableCmd( pBuf[0],
                                  pBuf[1] );
}
#endif // CTRL_CONFIG=SCAN_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
/*******************************************************************************
 * @fn          hciLECreateConn
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLECreateConn( uint8 *pBuf )
{
  return HCI_LE_CreateConnCmd( BUILD_UINT16(pBuf[0], pBuf[1]),
                               BUILD_UINT16(pBuf[2], pBuf[3]),
                               pBuf[4],
                               pBuf[5],
                               &pBuf[6],
                               pBuf[12],
                               BUILD_UINT16(pBuf[13], pBuf[14]),
                               BUILD_UINT16(pBuf[15], pBuf[16]),
                               BUILD_UINT16(pBuf[17], pBuf[18]),
                               BUILD_UINT16(pBuf[19], pBuf[20]),
                               BUILD_UINT16(pBuf[21], pBuf[22]),
                               BUILD_UINT16(pBuf[23], pBuf[24]) );
}
#endif // CTRL_CONFIG=INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
/*******************************************************************************
 * @fn          hciLECreateConnCancel
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLECreateConnCancel( uint8 *pBuf )
{
  // unused input parameter; PC-Lint error 715.
  (void)pBuf;

  return HCI_LE_CreateConnCancelCmd();
}
#endif // CTRL_CONFIG=INIT_CFG


/*******************************************************************************
 * @fn          hciLEReadWhiteListSize
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadWhiteListSize( uint8 *pBuf )
{
  // unused input parameter; PC-Lint error 715.
  (void)pBuf;

  return HCI_LE_ReadWhiteListSizeCmd();
}


/*******************************************************************************
 * @fn          hciLEClearWhiteList
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEClearWhiteList( uint8 *pBuf )
{
  // unused input parameter; PC-Lint error 715.
  (void)pBuf;

  return HCI_LE_ClearWhiteListCmd();
}


/*******************************************************************************
 * @fn          hciLEAddWhiteList
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEAddWhiteList( uint8 *pBuf )
{
  return HCI_LE_AddWhiteListCmd( pBuf[0],
                                 &pBuf[1] );
}


/*******************************************************************************
 * @fn          hciLERemoveWhiteList
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLERemoveWhiteList( uint8 *pBuf )
{
  return HCI_LE_RemoveWhiteListCmd( pBuf[0],
                                    &pBuf[1] );
}


/*******************************************************************************
 * @fn          hciLEConnUpdate
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
hciStatus_t hciLEConnUpdate( uint8 *pBuf )
{
  return HCI_LE_ConnUpdateCmd( BUILD_UINT16(pBuf[0], pBuf[1]),
                               BUILD_UINT16(pBuf[2], pBuf[3]),
                               BUILD_UINT16(pBuf[4], pBuf[5]),
                               BUILD_UINT16(pBuf[6], pBuf[7]),
                               BUILD_UINT16(pBuf[8], pBuf[9]),
                               BUILD_UINT16(pBuf[10], pBuf[11]),
                               BUILD_UINT16(pBuf[12], pBuf[13]) );
}
#endif // CTRL_CONFIG=ADV_CONN_CFG | INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
/*******************************************************************************
 * @fn          hciLESetHostChanClass
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetHostChanClass( uint8 *pBuf )
{
  return HCI_LE_SetHostChanClassificationCmd( pBuf );
}
#endif // CTRL_CONFIG=INIT_CFG

/*******************************************************************************
 * @fn          hciExtSetHostDefChanClass
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetHostDefChanClass( uint8 *pBuf )
{
  return HCI_EXT_SetHostDefChanClassificationCmd( pBuf );
}

/*******************************************************************************
 * @fn          hciExtSetHostConnChanClass
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetHostConnChanClass( uint8 *pBuf )
{
  return HCI_EXT_SetHostConnChanClassificationCmd( pBuf , BUILD_UINT16(pBuf[5], pBuf[6]) );
}

#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
/*******************************************************************************
 * @fn          hciLEReadChanMap
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadChanMap( uint8 *pBuf )
{
  return HCI_LE_ReadChannelMapCmd( BUILD_UINT16(pBuf[0], pBuf[1]) );
}
#endif // CTRL_CONFIG=(ADV_CONN_CFG | INIT_CFG)


#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
/*******************************************************************************
 * @fn          hciLEReadRemoteUsedFeatures
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadRemoteUsedFeatures( uint8 *pBuf )
{
  return HCI_LE_ReadRemoteUsedFeaturesCmd( BUILD_UINT16(pBuf[0], pBuf[1]) );
}
#endif // CTRL_CONFIG=ADV_CONN_CFG | INIT_CFG


/*******************************************************************************
 * @fn          hciLEEncrypt
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEEncrypt( uint8 *pBuf )
{
  // reverse byte order of key (MSB..LSB required)
  HCI_ReverseBytes( &pBuf[0], KEYLEN );

  // reverse byte order of plaintext (MSB..LSB required)
  HCI_ReverseBytes( &pBuf[KEYLEN], KEYLEN );

  return HCI_LE_EncryptCmd( &pBuf[0],
                            &pBuf[KEYLEN] );
}


/*******************************************************************************
 * @fn          hciLERand
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLERand( uint8 *pBuf )
{
  (void)pBuf;

  return HCI_LE_RandCmd();
}


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
/*******************************************************************************
 * @fn          hciLEStartEncrypt
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEStartEncrypt( uint8 *pBuf )
{
  return HCI_LE_StartEncyptCmd( BUILD_UINT16(pBuf[0], pBuf[1]),
                                &pBuf[2],
                                &pBuf[10],
                                &pBuf[12] );
}
#endif // CTRL_CONFIG=INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG)
/*******************************************************************************
 * @fn          hciLELtkReqReply
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLELtkReqReply( uint8 *pBuf )
{
  return HCI_LE_LtkReqReplyCmd( BUILD_UINT16(pBuf[0], pBuf[1]),
                                &pBuf[2] );
}
#endif // CTRL_CONFIG=ADV_CONN_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG)
/*******************************************************************************
 * @fn          hciLELtkReqNegReply
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLELtkReqNegReply( uint8 *pBuf )
{
  return HCI_LE_LtkReqNegReplyCmd( BUILD_UINT16(pBuf[0], pBuf[1]) );
}
#endif // CTRL_CONFIG=ADV_CONN_CFG


/*******************************************************************************
 * @fn          hciLEReadSupportedStates
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadSupportedStates( uint8 *pBuf )
{
  // unused input parameter; PC-Lint error 715.
  (void)pBuf;

  return HCI_LE_ReadSupportedStatesCmd();
}


/*******************************************************************************
 * @fn          hciLEReceiverTest
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReceiverTest( uint8 *pBuf )
{
  return HCI_LE_ReceiverTestCmd( pBuf[0] );
}


/*******************************************************************************
 * @fn          hciLETransmitterTest
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLETransmitterTest( uint8 *pBuf )
{
  return HCI_LE_TransmitterTestCmd( pBuf[0],
                                    pBuf[1],
                                    pBuf[2] );
}


/*******************************************************************************
 * @fn          hciLETestEnd
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLETestEnd( uint8 *pBuf )
{
  // unused input parameter; PC-Lint error 715.
  (void)pBuf;

  return HCI_LE_TestEndCmd();
}


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          hciLERemoteConnParamReqReply
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLERemoteConnParamReqReply( uint8 *pBuf )
{
  return HCI_LE_RemoteConnParamReqReplyCmd( BUILD_UINT16(pBuf[0], pBuf[1]),
                                            BUILD_UINT16(pBuf[2], pBuf[3]),
                                            BUILD_UINT16(pBuf[4], pBuf[5]),
                                            BUILD_UINT16(pBuf[6], pBuf[7]),
                                            BUILD_UINT16(pBuf[8], pBuf[9]),
                                            BUILD_UINT16(pBuf[10], pBuf[11]),
                                            BUILD_UINT16(pBuf[12], pBuf[13]) );
}
#endif // (ADV_CONN_CFG | INIT_CFG)


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          hciLERemoteConnParamReqNegReply
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLERemoteConnParamReqNegReply( uint8 *pBuf )
{
  return HCI_LE_RemoteConnParamReqNegReplyCmd( BUILD_UINT16(pBuf[0], pBuf[1]),
                                               pBuf[2] );
}
#endif // (ADV_CONN_CFG | INIT_CFG)


// V4.2 - Extended Data Length

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          hciLESetDataLen
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetDataLen( uint8 *pBuf )
{
  return HCI_LE_SetDataLenCmd( BUILD_UINT16(pBuf[0], pBuf[1]),
                               BUILD_UINT16(pBuf[2], pBuf[3]),
                               BUILD_UINT16(pBuf[4], pBuf[5]) );
}
#endif // (ADV_CONN_CFG | INIT_CFG)


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          hciLEReadSuggestedDefaultDataLen
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadSuggestedDefaultDataLen( uint8 *pBuf )
{
  return HCI_LE_ReadSuggestedDefaultDataLenCmd();
}
#endif // (ADV_CONN_CFG | INIT_CFG)


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          hciLEWriteSuggestedDefaultDataLen
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEWriteSuggestedDefaultDataLen( uint8 *pBuf )
{
  return HCI_LE_WriteSuggestedDefaultDataLenCmd( BUILD_UINT16(pBuf[0], pBuf[1]),
                                                 BUILD_UINT16(pBuf[2], pBuf[3]) );

}
#endif // (ADV_CONN_CFG | INIT_CFG)


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          hciLEReadMaxDataLen
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadMaxDataLen( uint8 *pBuf )
{
  return HCI_LE_ReadMaxDataLenCmd();
}
#endif // (ADV_CONN_CFG | INIT_CFG)

// V4.2 - Secure Connections

/*******************************************************************************
 * @fn          hciLEReadLocalP256PubicKey
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadLocalP256PubicKey( uint8 *pBuf )
{
  return HCI_LE_ReadLocalP256PublicKeyCmd();
}


/*******************************************************************************
 * @fn          hciLEGenerateDHKey
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEGenerateDHKey( uint8 *pBuf )
{
  return HCI_LE_GenerateDHKeyCmd( pBuf );
}

// V4.2 - Privacy 1.2

/*******************************************************************************
 * @fn          hciLEAddDeviceToResolvingList
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEAddDeviceToResolvingList( uint8 *pBuf )
{
  return HCI_LE_AddDeviceToResolvingListCmd(  pBuf[0],
                                             &pBuf[1],
                                             &pBuf[7],
                                             &pBuf[23] );
}


/*******************************************************************************
 * @fn          hciLERemoveDeviceFromResolvingList
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLERemoveDeviceFromResolvingList( uint8 *pBuf )
{
  return HCI_LE_RemoveDeviceFromResolvingListCmd(  pBuf[0],
                                                  &pBuf[1] );
}


/*******************************************************************************
 * @fn          hciLEClearResolvingList
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEClearResolvingList( uint8 *pBuf )
{
  return HCI_LE_ClearResolvingListCmd();
}


/*******************************************************************************
 * @fn          hciLEReadResolvingListSize
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadResolvingListSize( uint8 *pBuf )
{
  return HCI_LE_ReadResolvingListSizeCmd();
}


/*******************************************************************************
 * @fn          hciLEReadPeerResolvableAddress
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadPeerResolvableAddress( uint8 *pBuf )
{
  return HCI_LE_ReadPeerResolvableAddressCmd(  pBuf[0],
                                              &pBuf[1] );
}


/*******************************************************************************
 * @fn          hciLEReadLocalResolvableAddress
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadLocalResolvableAddress( uint8 *pBuf )
{
  return HCI_LE_ReadLocalResolvableAddressCmd(  pBuf[0],
                                               &pBuf[1] );
}


/*******************************************************************************
 * @fn          hciLESetAddressResolutionEnable
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetAddressResolutionEnable( uint8 *pBuf )
{
  return HCI_LE_SetAddressResolutionEnableCmd( pBuf[0] );
}


/*******************************************************************************
 * @fn          hciLESetResolvablePrivateAddressTimeout
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetResolvablePrivateAddressTimeout( uint8 *pBuf )
{
  return HCI_LE_SetResolvablePrivateAddressTimeoutCmd( BUILD_UINT16(pBuf[0], pBuf[1]) );
}


/*******************************************************************************
 * @fn          hciLESetPrivacyMode
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetPrivacyMode( uint8 *pBuf )
{
  return HCI_LE_SetPrivacyModeCmd(  pBuf[0],
                                   &pBuf[1],
                                    pBuf[5] );
}


// V5.0 - 2M and Coded PHY

#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG)) &&  \
    defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          hciLEReadPhy
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadPhy( uint8 *pBuf )
{
  return HCI_LE_ReadPhyCmd( BUILD_UINT16(pBuf[0], pBuf[1]) );
}
#endif // (PHY_2MBPS_CFG | PHY_LR_CFG) & (DV_CONN_CFG | INIT_CFG)


#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG))
/*******************************************************************************
 * @fn          hciLESetDefaultPhy
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetDefaultPhy( uint8 *pBuf )
{
  return HCI_LE_SetDefaultPhyCmd( pBuf[0],
                                  pBuf[1],
                                  pBuf[2] );
}
#endif // PHY_2MBPS_CFG | PHY_LR_CFG


#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG)) &&  \
    defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          hciLESetPhy
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetPhy( uint8 *pBuf )
{
  return HCI_LE_SetPhyCmd( BUILD_UINT16(pBuf[0], pBuf[1]),
                           pBuf[2],
                           pBuf[3],
                           pBuf[4],
                           BUILD_UINT16(pBuf[5], pBuf[6]) );
}
#endif // (PHY_2MBPS_CFG | PHY_LR_CFG) & (ADV_CONN_CFG | INIT_CFG)


#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG))
/*******************************************************************************
 * @fn          hciLEEnhancedRxTest
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEEnhancedRxTest( uint8 *pBuf )
{
  return HCI_LE_EnhancedRxTestCmd( pBuf[0],
                                   pBuf[1],
                                   pBuf[2] );
}
#endif // PHY_2MBPS_CFG | PHY_LR_CFG


#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG))
/*******************************************************************************
 * @fn          hciLEEnhancedTxTest
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEEnhancedTxTest( uint8 *pBuf )
{
  return HCI_LE_EnhancedTxTestCmd( pBuf[0],
                                   pBuf[1],
                                   pBuf[2],
                                   pBuf[3] );
}
#endif // PHY_2MBPS_CFG | PHY_LR_CFG

/*******************************************************************************
 * @fn          hciLEEnhancedTxTest
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEEnhancedTxTest( uint8 *pBuf )
{
  return HCI_LE_EnhancedTxTestCmd( pBuf[0],
                                   pBuf[1],
                                   pBuf[2],
                                   pBuf[3] );
}

#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG))
/*******************************************************************************
 * @fn          hciLEEnhancedCteRxTest
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEEnhancedCteRxTest( uint8 *pBuf )
{
  return HCI_LE_EnhancedCteRxTestCmd( pBuf[0],
                                      pBuf[1],
                                      pBuf[2],
                                      pBuf[3],
                                      pBuf[4],
                                      pBuf[5],
                                      pBuf[6],
                                      &pBuf[7] );
}
#endif // PHY_2MBPS_CFG | PHY_LR_CFG


#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG))
/*******************************************************************************
 * @fn          hciLEEnhancedCteTxTest
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEEnhancedCteTxTest( uint8 *pBuf )
{
  return HCI_LE_EnhancedCteTxTestCmd( pBuf[0],
                                      pBuf[1],
                                      pBuf[2],
                                      pBuf[3],
                                      pBuf[4],
                                      pBuf[5],
                                      pBuf[6],
                                      &pBuf[7] );
}
#endif // PHY_2MBPS_CFG | PHY_LR_CFG

/*******************************************************************************
 * @fn          hciLESetConnectionCteReceiveParams
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetConnectionCteReceiveParams( uint8 *pBuf )
{
  return HCI_LE_SetConnectionCteReceiveParamsCmd( BUILD_UINT16(pBuf[0], pBuf[1])
                                                  pBuf[2], 
                                                  pBuf[3], 
                                                  pBuf[4],
                                                  &pBuf[5]);
}

/*******************************************************************************
 * @fn          hciLESetConnectionCteTransmitParams
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetConnectionCteTransmitParams( uint8 *pBuf )
{
  return HCI_LE_SetConnectionCteTransmitParamsCmd( BUILD_UINT16(pBuf[0], pBuf[1]),
                                                  pBuf[2], 
                                                  pBuf[3], 
                                                  &pBuf[4]);
}

/*******************************************************************************
 * @fn          hciLESetConnectionCteRequestEnable
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetConnectionCteRequestEnable( uint8 *pBuf )
{
  return HCI_LE_SetConnectionCteRequestEnableCmd( BUILD_UINT16(pBuf[0], pBuf[1]) ,
                                                  pBuf[2], 
                                                  BUILD_UINT16(pBuf[3], pBuf[4]), 
                                                  pBuf[5],
                                                  pBuf[6]);
}

/*******************************************************************************
 * @fn          hciLESetConnectionCteResponseEnable
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetConnectionCteResponseEnable( uint8 *pBuf )
{
  return HCI_LE_SetConnectionCteResponseEnableCmd( BUILD_UINT16(pBuf[0], pBuf[1]),
                                                   pBuf[2] );
}


/*******************************************************************************
 * @fn          hciLEReadAntennaInformation
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadAntennaInformation( uint8 *pBuf )
{
  return HCI_LE_ReadAntennaInformationCmd(void);
}

#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG))
/*******************************************************************************
 * @fn          hciLESetPeriodicAdvParams
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetPeriodicAdvParams( uint8 *pBuf )
{
  return HCI_LE_SetPeriodicAdvParamsCmd( pBuf[0],
                                         BUILD_UINT16(pBuf[1], pBuf[2]),
                                         BUILD_UINT16(pBuf[3], pBuf[4]),
                                         BUILD_UINT16(pBuf[5], pBuf[6]));
}

/*******************************************************************************
 * @fn          hciLESetPeriodicAdvData
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetPeriodicAdvData( uint8 *pBuf )
{
  return HCI_LE_SetPeriodicAdvDataCmd( pBuf[0],
                                       pBuf[1],
                                       pBuf[2]),
                                       &pBuf[3]);
}

/*******************************************************************************
 * @fn          hciLESetPeriodicAdvEnable
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetPeriodicAdvEnable( uint8 *pBuf )
{
  return HCI_LE_SetPeriodicAdvEnableCmd( pBuf[0],
                                         pBuf[1]);
}

/*******************************************************************************
 * @fn          hciLESetConnectionlessCteTransmitParams
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetConnectionlessCteTransmitParams( uint8 *pBuf )
{
  return HCI_LE_SetConnectionlessCteTransmitParamsCmd( pBuf[0],
                                                       pBuf[1],
                                                       pBuf[2],
                                                       pBuf[3],
                                                       pBuf[4],
                                                       &pBuf[5]);
}

/*******************************************************************************
 * @fn          hciLESetConnectionlessCteTransmitEnable
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetConnectionlessCteTransmitEnable( uint8 *pBuf )
{
  return HCI_LE_SetConnectionlessCteTransmitEnableCmd( pBuf[0],
                                                       pBuf[1]);
}
#endif // CTRL_CONFIG=(ADV_NCONN_CFG | ADV_CONN_CFG)

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
/*******************************************************************************
 * @fn          hciLEPeriodicAdvCreateSync
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEPeriodicAdvCreateSync( uint8 *pBuf )
{
  return HCI_LE_PeriodicAdvCreateSyncCmd( pBuf[0],
                                          pBuf[1],
                                          pBuf[2],
                                          &pBuf[3],
                                          BUILD_UINT16(pBuf[9], pBuf[10]),
                                          BUILD_UINT16(pBuf[11], pBuf[12]),
                                          pBuf[13]);
}

/*******************************************************************************
 * @fn          hciLEPeriodicAdvCreateSyncCancel
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEPeriodicAdvCreateSyncCancel( uint8 *pBuf )
{
  return HCI_LE_PeriodicAdvCreateSyncCancelCmd();
}

/*******************************************************************************
 * @fn          hciLEPeriodicAdvTerminateSync
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEPeriodicAdvTerminateSync( uint8 *pBuf )
{
  return HCI_LE_PeriodicAdvTerminateSyncCmd(BUILD_UINT16(pBuf[0], pBuf[1]));
}

/*******************************************************************************
 * @fn          hciLEAddDeviceToPeriodicAdvList
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEAddDeviceToPeriodicAdvList( uint8 *pBuf )
{
  return HCI_LE_AddDeviceToPeriodicAdvListCmd(pBuf[0],
                                              &pBuf[1],
                                              pBuf[7]);
}

/*******************************************************************************
 * @fn          hciLERemoveDeviceFromPeriodicAdvList
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLERemoveDeviceFromPeriodicAdvList( uint8 *pBuf )
{
  return HCI_LE_RemoveDeviceFromPeriodicAdvListCmd(pBuf[0],
                                                   &pBuf[1],
                                                   pBuf[7]);
}

/*******************************************************************************
 * @fn          hciLEClearPeriodicAdvList
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEClearPeriodicAdvList( uint8 *pBuf )
{
  return HCI_LE_ClearPeriodicAdvListCmd();
}

/*******************************************************************************
 * @fn          hciLEReadPeriodicAdvListSize
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadPeriodicAdvListSize( uint8 *pBuf )
{
  return HCI_LE_ReadPeriodicAdvListSizeCmd();
}

/*******************************************************************************
 * @fn          hciLESetPeriodicAdvReceiveEnable
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetPeriodicAdvReceiveEnable( uint8 *pBuf )
{
  return HCI_LE_SetPeriodicAdvReceiveEnableCmd(BUILD_UINT16(pBuf[0], pBuf[1]),
                                               pBuf[2]);
}

/*******************************************************************************
 * @fn          hciLESetConnectionlessIqSamplingEnable
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetConnectionlessIqSamplingEnable( uint8 *pBuf )
{
  return HCI_LE_SetConnectionlessIqSamplingEnableCmd( BUILD_UINT16(pBuf[0], pBuf[1]),
                                                      pBuf[2],
                                                      pBuf[3],
                                                      pBuf[4],
                                                      pBuf[5],
                                                      &pBuf[6]);
#endif

/*
** Vendor Specific Commands
*/

/*******************************************************************************
 * @fn          hciExtSetRxGain
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetRxGain( uint8 *pBuf )
{
  return HCI_EXT_SetRxGainCmd( pBuf[0] );
}


/*******************************************************************************
 * @fn          hciExtSetTxPower
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetTxPower( uint8 *pBuf )
{
  return HCI_EXT_SetTxPowerCmd( pBuf[0] );
}


/*******************************************************************************
 * @fn          hciExtExtendRfRange
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtExtendRfRange( uint8 *pBuf )
{
  // unused input parameter; PC-Lint error 715.
  (void)pBuf;

  return HCI_EXT_ExtendRfRangeCmd();
}


/*******************************************************************************
 * @fn          hciExtHaltDuringRf
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtHaltDuringRf( uint8 *pBuf )
{
  return HCI_EXT_HaltDuringRfCmd( pBuf[0] );
}


/*******************************************************************************
 * @fn          hciExtSetMaxDtmTxPower
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetMaxDtmTxPower( uint8 *pBuf )
{
  return HCI_EXT_SetMaxDtmTxPowerCmd( pBuf[0] );
}


/*******************************************************************************
 * @fn          hciExtMapPmIoPort
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtMapPmIoPort( uint8 *pBuf )
{
  return HCI_EXT_MapPmIoPortCmd( pBuf[0],
                                 pBuf[1] );
}


#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
/*******************************************************************************
 * @fn          hciExtDisconnectImmed
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtDisconnectImmed( uint8 *pBuf )
{
  return HCI_EXT_DisconnectImmedCmd ( BUILD_UINT16(pBuf[0],
                                                   pBuf[1]) );
}
#endif // CTRL_CONFIG=(ADV_CONN_CFG | INIT_CFG)


#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
/*******************************************************************************
 * @fn          hciExtPER
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtPER( uint8 *pBuf )
{
  return HCI_EXT_PacketErrorRateCmd ( BUILD_UINT16(pBuf[0],
                                                   pBuf[1]),
                                      pBuf[2] );
}
#endif // CTRL_CONFIG=(ADV_CONN_CFG | INIT_CFG)


#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
/*******************************************************************************
 * @fn          hciExtOverlappedProcessing
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtOverlappedProcessing( uint8 *pBuf )
{
  return HCI_EXT_OverlappedProcessingCmd ( pBuf[0] );
}
#endif // CTRL_CONFIG=(ADV_CONN_CFG | INIT_CFG)


#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
/*******************************************************************************
 * @fn          hciExtNumComplPktsLimit
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtNumComplPktsLimit( uint8 *pBuf )
{
  return HCI_EXT_NumComplPktsLimitCmd( pBuf[0],
                                       pBuf[1] );
}
#endif // CTRL_CONFIG=(ADV_CONN_CFG | INIT_CFG)


#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
/*******************************************************************************
 * @fn          hciExtGetConnInfo
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtGetConnInfo( uint8 *pBuf )
{
  return HCI_EXT_GetConnInfoCmd( NULL,
                                 NULL,
                                 NULL );
}
#endif // CTRL_CONFIG=(ADV_CONN_CFG | INIT_CFG)


#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
/*******************************************************************************
 * @fn          hciExtOnePktPerEvt
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtOnePktPerEvt( uint8 *pBuf )
{
  return HCI_EXT_OnePktPerEvtCmd( pBuf[0] );
}
#endif // CTRL_CONFIG=(ADV_CONN_CFG | INIT_CFG)


/*******************************************************************************
 * @fn          hciExtClkDivOnHalt
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtClkDivOnHalt( uint8 *pBuf )
{
  return HCI_EXT_ClkDivOnHaltCmd( pBuf[0] );
}


/*******************************************************************************
 * @fn          hciExtDeclareNvUsage
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtDeclareNvUsage( uint8 *pBuf )
{
  return HCI_EXT_DeclareNvUsageCmd( pBuf[0] );
}


/*******************************************************************************
 * @fn          hciExtDecrypt
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtDecrypt( uint8 *pBuf )
{
  // reverse byte order of key (MSB..LSB required)
  HCI_ReverseBytes( &pBuf[0], KEYLEN );

  // reverse byte order of encText (MSB..LSB required)
  HCI_ReverseBytes( &pBuf[KEYLEN], KEYLEN );

  return HCI_EXT_DecryptCmd( &pBuf[0],
                             &pBuf[KEYLEN] );
}


/*******************************************************************************
 * @fn          hciExtSetLocalSupportedFeatures
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetLocalSupportedFeatures( uint8 *pBuf )
{
  return HCI_EXT_SetLocalSupportedFeaturesCmd( pBuf );
}


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG)
/*******************************************************************************
 * @fn          hciExtSetFastTxResponseTime
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetFastTxResponseTime( uint8 *pBuf )
{
  return HCI_EXT_SetFastTxResponseTimeCmd( pBuf[0] );
}
#endif // ADV_CONN_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG)
/*******************************************************************************
 * @fn          hciExtSetSlaveLatencyOverride
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetSlaveLatencyOverride( uint8 *pBuf )
{
  return HCI_EXT_SetSlaveLatencyOverrideCmd( pBuf[0] );
}
#endif // ADV_CONN_CFG


#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
/*******************************************************************************
 * @fn          hciExtSetSCA
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetSCA( uint8 *pBuf )
{
  return HCI_EXT_SetSCACmd( BUILD_UINT16(pBuf[0], pBuf[1]) );
}
#endif // ADV_CONN_CFG | INIT_CFG


/*******************************************************************************
 * @fn          hciExtBuildRevision
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtBuildRevision( uint8 *pBuf )
{
  // unused input parameter; PC-Lint error 715.
  (void)pBuf;

  return HCI_EXT_BuildRevisionCmd( pBuf[0], BUILD_UINT16( pBuf[1],
                                                          pBuf[2]) );
}


/*******************************************************************************
 * @fn          hciExtDelaySleep
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtDelaySleep( uint8 *pBuf )
{
  return HCI_EXT_DelaySleepCmd( BUILD_UINT16(pBuf[0], pBuf[1]) );
}


/*******************************************************************************
 * @fn          hciExtResetSystem
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtResetSystem( uint8 *pBuf )
{
  return HCI_EXT_ResetSystemCmd( pBuf[0] );
}


#ifdef LL_TEST_MODE
/*******************************************************************************
 * @fn          hciExtLLTestMode
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtLLTestMode( uint8 *pBuf )
{
  return HCI_EXT_LLTestModeCmd( pBuf[0] );
}
#endif // LL_TEST_MODE


/*
** Allowed PTM Commands
*/

/*******************************************************************************
 * @fn          hciReadBDADDR
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciReadBDADDR( uint8 *pBuf )
{
  // unused input parameter; PC-Lint error 715.
  (void)pBuf;

  return HCI_ReadBDADDRCmd();
}


/*******************************************************************************
 * @fn          hciExtModemTestTx
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtModemTestTx( uint8 *pBuf )
{
  return HCI_EXT_ModemTestTxCmd( pBuf[0], pBuf[1] );
}


/*******************************************************************************
 * @fn          hciExtModemHopTestTxCmd
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtModemHopTestTx( uint8 *pBuf )
{
  return HCI_EXT_ModemHopTestTxCmd();
}


/*******************************************************************************
 * @fn          hciExtModemtestRxCmd
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtModemtestRx( uint8 *pBuf )
{
  return HCI_EXT_ModemTestRxCmd( pBuf[0] );
}


/*******************************************************************************
 * @fn          hciExtEnhancedModemTestTx
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtEnhancedModemTestTx( uint8 *pBuf )
{
  return HCI_EXT_EnhancedModemTestTxCmd( pBuf[0], pBuf[1] );
}


/*******************************************************************************
 * @fn          hciExtEnhancedModemHopTestTxCmd
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtEnhancedModemHopTestTx( uint8 *pBuf )
{
  return HCI_EXT_EnhancedModemHopTestTxCmd();
}


/*******************************************************************************
 * @fn          hciExtEnhancedModemtestRxCmd
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtEnhancedModemtestRx( uint8 *pBuf )
{
  return HCI_EXT_EnhancedModemTestRxCmd( pBuf[0] );
}


/*******************************************************************************
 * @fn          hciExtEndModemTest
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtEndModemTest( uint8 *pBuf )
{
  return HCI_EXT_EndModemTestCmd();
}


/*******************************************************************************
 * @fn          hciExtSetBDADDR
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetBDADDR( uint8 *pBuf )
{
  return HCI_EXT_SetBDADDRCmd( pBuf );
}


/*******************************************************************************
 * @fn          hciExtEnablePTM
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtEnablePTM( uint8 *pBuf )
{
  return HCI_EXT_EnablePTMCmd();
}


/*******************************************************************************
 * @fn          hciExtSetFreqTune
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetFreqTune( uint8 *pBuf )
{
  return HCI_EXT_SetFreqTuneCmd( pBuf[0] );
}


/*******************************************************************************
 * @fn          hciExtSaveFreqTune
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSaveFreqTune( uint8 *pBuf )
{
  return HCI_EXT_SaveFreqTuneCmd();
}


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          hciExtSetMaxDataLen
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetMaxDataLen( uint8 *pBuf )
{
  return HCI_EXT_SetMaxDataLenCmd( BUILD_UINT16(pBuf[0], pBuf[1]),
                                   BUILD_UINT16(pBuf[2], pBuf[3]),
                                   BUILD_UINT16(pBuf[4], pBuf[5]),
                                   BUILD_UINT16(pBuf[6], pBuf[7]) );
}
#endif // (ADV_CONN_CFG | INIT_CFG)


/*******************************************************************************
 * @fn          hciExtSetDtmTxPktCnt
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetDtmTxPktCnt( uint8 *pBuf )
{
  return HCI_EXT_SetDtmTxPktCntCmd( BUILD_UINT16(pBuf[0], pBuf[1]) );
}

/*******************************************************************************
 * @fn          hciExtReadRandAddr
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtReadRandAddr( uint8 *pBuf )
{
  // unused input parameter; PC-Lint error 715.
  (void)pBuf;

  return HCI_EXT_ReadRandAddrCmd();
}

/*******************************************************************************
 * @fn          hciExtSetPinOutput
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetPinOutput( uint8 *pBuf )
{
  return HCI_EXT_SetPinOutputCmd(pBuf[0], pBuf[1]);
}

/*******************************************************************************
 * @fn          hciExtSetLocationingAccuracy
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetLocationingAccuracy( uint8 *pBuf )
{
  return HCI_EXT_SetLocationingAccuracyCmd(BUILD_UINT16(pBuf[0], pBuf[1]),
                                           pBuf[2],
                                           pBuf[3],
                                           pBuf[4],
                                           pBuf[5],
                                           pBuf[6]);
}

 #if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
/*******************************************************************************
 * @fn          hciExtSetExtScanChannels
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetExtScanChannels( uint8 *pBuf )
{
  return HCI_EXT_SetExtScanChannels( pBuf[0] );
}
#endif // CTRL_CONFIG & SCAN_CFG

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          hciExtSetQOSParameters
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetQOSParameters( uint8 *pBuf )
{
  return HCI_EXT_SetExtSetQOSParameters( pBuf[0],
                                         pBuf[1],
                                         BUILD_UINT32(pBuf[2], pBuf[3], pBuf[4],pBuf[5]),
                                         BUILD_UINT16(pBuf[6], pBuf[7]));
}

/*******************************************************************************
 * @fn          hciExtSetQOSDefaultParameters
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetQOSDefaultParameters( uint8 *pBuf )
{
    return HCI_EXT_SetQOSDefaultParameters( pBuf[0]);
}
#endif // ADV_CONN_CFG & INIT_CFG

/*******************************************************************************
 * @fn          hciExtCoexEnable
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtCoexEnable( uint8 *pBuf )
{
  return HCI_EXT_CoexEnableCmd(pBuf[0]);
}

#endif // !HCI_TL_NONE

#endif // ICALL_LITE
/*******************************************************************************
 */
