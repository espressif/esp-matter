/******************************************************************************

 @file  icall_api_idx.h

 @brief implementation of API override for the case the stack is use as library.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2016-2022, Texas Instruments Incorporated
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

#ifndef ICALL_API_IDX_H
#define ICALL_API_IDX_H


#ifndef STACK_LIBRARY
#include "ble_dispatch_lite_idx.h"
#else
/* GAP-GATT service API */
/************************/
#define IDX_GGS_SetParameter                          GGS_SetParameter
#define IDX_GGS_AddService                            GGS_AddService
#define IDX_GGS_GetParameter                          GGS_GetParameter
#define IDX_GGS_RegisterAppCBs                        GGS_RegisterAppCBs
#define IDX_GGS_SetParamValue                         GGS_SetParamValue
#define IDX_GGS_GetParamValue                         GGS_GetParamValue

/* GAP Bond Manager API */
/************************/
#define IDX_GAPBondMgr_SetParameter                   GAPBondMgr_SetParameter
#define IDX_GAPBondMgr_GetParameter                   GAPBondMgr_GetParameter
#define IDX_GAPBondMgr_Pair                           GAPBondMgr_Pair
#define IDX_GAPBondMgr_ServiceChangeInd               GAPBondMgr_ServiceChangeInd
#define IDX_GAPBondMgr_Register                       GAPBondMgr_Register
#define IDX_GAPBondMgr_PasscodeRsp                    GAPBondMgr_PasscodeRsp
#define IDX_GAPBondMgr_FindAddr                       GAPBondMgr_FindAddr
#define IDX_GAPBondMgr_ProcessGAPMsg                  GAPBondMgr_ProcessGAPMsg
#define IDX_GAPBondMgr_SCSetRemoteOOBParameters       GAPBondMgr_SCSetRemoteOOBParameters
#define IDX_GAPBondMgr_SCGetLocalOOBParameters        GAPBondMgr_SCGetLocalOOBParameters
#define IDX_GAPBondMgr_GenerateEccKeys                GAPBondMgr_GenerateEccKeys

/* GAP API */
/***********/
#ifdef GAP_BOND_MGR
#define IDX_GAP_DeviceInit                            GAP_DeviceInit
#else
#define IDX_GAP_DeviceInit                            GAP_DeviceInit_noGAPBondMgr
#endif
#define IDX_GAP_DeInit                                GAP_DeInit
#define IDX_GAP_ReInit                                GAP_ReInit
#define IDX_GAP_RegisterForMsgs                       GAP_RegisterForMsgs
#define IDX_GAP_SetParamValue                         GAP_SetParamValue
#define IDX_GAP_GetParamValue                         GAP_GetParamValue
#define IDX_GAP_RegisterBondMgrCBs                    GAP_RegisterBondMgrCBs
#define IDX_GapConfig_SetParameter                    GapConfig_SetParameter
#define IDX_GAP_GetIRK                                GAP_GetIRK
#define IDX_GAP_GetSRK                                GAP_GetSRK
#define IDX_GapAdv_create                             GapAdv_create
#define IDX_GapAdv_destroy                            GapAdv_destroy
#define IDX_GapAdv_enable                             GapAdv_enable
#define IDX_GapAdv_disable                            GapAdv_disable
#define IDX_GapAdv_setParam                           GapAdv_setParam
#define IDX_GapAdv_getParam                           GapAdv_getParam
#define IDX_GapAdv_getBuffer                          GapAdv_getBuffer
#define IDX_GapAdv_loadByHandle                       GapAdv_loadByHandle
#define IDX_GapAdv_loadByBuffer                       GapAdv_loadByBuffer_hook
#define IDX_GapAdv_prepareLoadByHandle                GapAdv_prepareLoadByHandle
#define IDX_GapAdv_prepareLoadByBuffer                GapAdv_prepareLoadByBuffer
#define IDX_GapAdv_abortLoad                          GapAdv_abortLoad
#define IDX_GapAdv_setEventMask                       GapAdv_setEventMask
#define IDX_GapAdv_setVirtualAdvAddr                  GapAdv_setVirtualAdvAddr
#define IDX_GapScan_registerCb                        GapScan_registerCb
#define IDX_GapAdv_SetPeriodicAdvParams               GapAdv_SetPeriodicAdvParams
#define IDX_GapAdv_SetPeriodicAdvData                 GapAdv_SetPeriodicAdvData
#define IDX_GapAdv_SetPeriodicAdvEnable               GapAdv_SetPeriodicAdvEnable
#define IDX_GapScan_setPhyParams                      GapScan_setPhyParams
#define IDX_GapScan_getPhyParams                      GapScan_getPhyParams
#define IDX_GapScan_setParam                          GapScan_setParam
#define IDX_GapScan_getParam                          GapScan_getParam
#define IDX_GapScan_setEventMask                      GapScan_setEventMask
#define IDX_GapScan_enable                            GapScan_enable
#define IDX_GapScan_disable                           GapScan_disable
#define IDX_GapScan_getAdvReport                      GapScan_getAdvReport
#define IDX_GapScan_discardAdvReportList              GapScan_discardAdvReportList
#define IDX_GapScan_PeriodicAdvCreateSync             GapScan_PeriodicAdvCreateSync
#define IDX_GapScan_PeriodicAdvCreateSyncCancel       GapScan_PeriodicAdvCreateSyncCancel
#define IDX_GapScan_PeriodicAdvTerminateSync          GapScan_PeriodicAdvTerminateSync
#define IDX_GapScan_SetPeriodicAdvReceiveEnable       GapScan_SetPeriodicAdvReceiveEnable
#define IDX_GapScan_AddDeviceToPeriodicAdvList        GapScan_AddDeviceToPeriodicAdvList
#define IDX_GapScan_RemoveDeviceFromPeriodicAdvList   GapScan_RemoveDeviceFromPeriodicAdvList
#define IDX_GapScan_ReadPeriodicAdvListSize           GapScan_ReadPeriodicAdvListSize
#define IDX_GapScan_ClearPeriodicAdvList              GapScan_ClearPeriodicAdvList
#define IDX_GapInit_setPhyParam                       GapInit_setPhyParam
#define IDX_GapInit_getPhyParam                       GapInit_getPhyParam
#define IDX_GapInit_connect                           GapInit_connect
#define IDX_GapInit_connectWl                         GapInit_connectWl
#define IDX_GapInit_cancelConnect                     GapInit_cancelConnect

#define IDX_GAP_TerminateLinkReq                      GAP_TerminateLinkReq
#define IDX_GAP_UpdateLinkParamReq                    GAP_UpdateLinkParamReq
#define IDX_GAP_UpdateLinkParamReqReply               GAP_UpdateLinkParamReqReply
#define IDX_Gap_RegisterConnEventCb                   Gap_RegisterConnEventCb
#define IDX_GAP_Authenticate                          GAP_Authenticate
#define IDX_GAP_TerminateAuth                         GAP_TerminateAuth
#define IDX_GAP_PasskeyUpdate                         GAP_PasskeyUpdate
#define IDX_GAP_SendSlaveSecurityRequest              GAP_SendSlaveSecurityRequest
#define IDX_GAP_Signable                              GAP_Signable
#define IDX_GAP_Bond                                  GAP_Bond

/* RTLS Services API */
/*********************/
#define IDX_RTLSSrv_init                               RTLSSrv_init
#define IDX_RTLSSrv_register                           RTLSSrv_register
#define IDX_RTLSSrv_setConnCteReceiveParams            RTLSSrv_setConnCteReceiveParams
#define IDX_RTLSSrv_setConnCteTransmitParams           RTLSSrv_setConnCteTransmitParams
#define IDX_RTLSSrv_setConnCteRequestEnableCmd         RTLSSrv_setConnCteRequestEnableCmd
#define IDX_RTLSSrv_setConnCteResponseEnableCmd        RTLSSrv_setConnCteResponseEnableCmd
#define IDX_RTLSSrv_readAntennaInformationCmd          RTLSSrv_readAntennaInformationCmd
#define IDX_RTLSSrv_setCteSampleAccuracy               RTLSSrv_setCteSampleAccuracy
#define IDX_RTLSSrv_setPinOutput                       RTLSSrv_setPinOutput
#define IDX_RTLSSrv_SetCLCteTransmitParams             RTLSSrv_SetCLCteTransmitParams
#define IDX_RTLSSrv_CLCteTransmitEnable                RTLSSrv_CLCteTransmitEnable
#define IDX_RTLSSrv_setCLCteSamplingEnableCmd          RTLSSrv_setCLCteSamplingEnableCmd

/* HCI API */
/***********/
#define IDX_HCI_ReadRemoteVersionInfoCmd              HCI_ReadRemoteVersionInfoCmd
#define IDX_HCI_SetEventMaskCmd                       HCI_SetEventMaskCmd
#define IDX_HCI_SetEventMaskPage2Cmd                  HCI_SetEventMaskPage2Cmd
#define IDX_HCI_ResetCmd                              HCI_ResetCmd
#define IDX_HCI_ReadTransmitPowerLevelCmd             HCI_ReadTransmitPowerLevelCmd
#define IDX_HCI_ReadLocalVersionInfoCmd               HCI_ReadLocalVersionInfoCmd
#define IDX_HCI_ReadLocalSupportedCommandsCmd         HCI_ReadLocalSupportedCommandsCmd
#define IDX_HCI_ReadLocalSupportedFeaturesCmd         HCI_ReadLocalSupportedFeaturesCmd
#define IDX_HCI_ReadBDADDRCmd                         HCI_ReadBDADDRCmd
#define IDX_HCI_ReadRssiCmd                           HCI_ReadRssiCmd
#define IDX_HCI_ValidConnTimeParams                   HCI_ValidConnTimeParams
#define IDX_HCI_LE_SetEventMaskCmd                    HCI_LE_SetEventMaskCmd
#define IDX_HCI_LE_ReadLocalSupportedFeaturesCmd      HCI_LE_ReadLocalSupportedFeaturesCmd
#define IDX_HCI_LE_ReadAdvChanTxPowerCmd              HCI_LE_ReadAdvChanTxPowerCmd
#define IDX_HCI_LE_ReadWhiteListSizeCmd               HCI_LE_ReadWhiteListSizeCmd
#define IDX_HCI_LE_ClearWhiteListCmd                  HCI_LE_ClearWhiteListCmd
#define IDX_HCI_LE_AddWhiteListCmd                    HCI_LE_AddWhiteListCmd
#define IDX_HCI_LE_RemoveWhiteListCmd                 HCI_LE_RemoveWhiteListCmd
#define IDX_HCI_LE_SetHostChanClassificationCmd       HCI_LE_SetHostChanClassificationCmd
#define IDX_HCI_LE_ReadChannelMapCmd                  HCI_LE_ReadChannelMapCmd
#define IDX_HCI_LE_ReadRemoteUsedFeaturesCmd          HCI_LE_ReadRemoteUsedFeaturesCmd
#define IDX_HCI_LE_EncryptCmd                         HCI_LE_EncryptCmd
#define IDX_HCI_LE_ReadSupportedStatesCmd             HCI_LE_ReadSupportedStatesCmd
#define IDX_HCI_LE_ReceiverTestCmd                    HCI_LE_ReceiverTestCmd
#define IDX_HCI_LE_TransmitterTestCmd                 HCI_LE_TransmitterTestCmd
#define IDX_HCI_LE_TestEndCmd                         HCI_LE_TestEndCmd
#define IDX_HCI_LE_ReadMaxDataLenCmd                  HCI_LE_ReadMaxDataLenCmd
#define IDX_HCI_LE_ReadSuggestedDefaultDataLenCmd     HCI_LE_ReadSuggestedDefaultDataLenCmd
#define IDX_HCI_LE_WriteSuggestedDefaultDataLenCmd    HCI_LE_WriteSuggestedDefaultDataLenCmd
#define IDX_HCI_LE_SetDataLenCmd                      HCI_LE_SetDataLenCmd
#define IDX_HCI_LE_ReadTxPowerCmd                     HCI_LE_ReadTxPowerCmd
#define IDX_HCI_LE_ReadRfPathCompCmd                  HCI_LE_ReadRfPathCompCmd
#define IDX_HCI_LE_WriteRfPathCompCmd                 HCI_LE_WriteRfPathCompCmd
#define IDX_HCI_LE_SetConnectionCteReceiveParamsCmd   HCI_LE_SetConnectionCteReceiveParamsCmd
#define IDX_HCI_LE_SetConnectionCteTransmitParamsCmd  HCI_LE_SetConnectionCteTransmitParamsCmd
#define IDX_HCI_LE_SetConnectionCteRequestEnableCmd   HCI_LE_SetConnectionCteRequestEnableCmd
#define IDX_HCI_LE_SetConnectionCteResponseEnableCmd  HCI_LE_SetConnectionCteResponseEnableCmd
#define IDX_HCI_LE_ReadAntennaInformationCmd          HCI_LE_ReadAntennaInformationCmd
#define IDX_HCI_LE_SetPeriodicAdvParamsCmd            HCI_LE_SetPeriodicAdvParamsCmd
#define IDX_HCI_LE_SetPeriodicAdvDataCmd              HCI_LE_SetPeriodicAdvDataCmd
#define IDX_HCI_LE_SetPeriodicAdvEnableCmd            HCI_LE_SetPeriodicAdvEnableCmd
#define IDX_HCI_LE_PeriodicAdvCreateSyncCmd           HCI_LE_PeriodicAdvCreateSyncCmd
#define IDX_HCI_LE_PeriodicAdvCreateSyncCancelCmd     HCI_LE_PeriodicAdvCreateSyncCancelCmd
#define IDX_HCI_LE_PeriodicAdvTerminateSyncCmd        HCI_LE_PeriodicAdvTerminateSyncCmd
#define IDX_HCI_LE_AddDeviceToPeriodicAdvListCmd      HCI_LE_AddDeviceToPeriodicAdvListCmd
#define IDX_HCI_LE_RemoveDeviceFromPeriodicAdvListCmd HCI_LE_RemoveDeviceFromPeriodicAdvListCmd
#define IDX_HCI_LE_ClearPeriodicAdvListCmd            HCI_LE_ClearPeriodicAdvListCmd
#define IDX_HCI_LE_ReadPeriodicAdvListSizeCmd         HCI_LE_ReadPeriodicAdvListSizeCmd
#define IDX_HCI_LE_SetPeriodicAdvReceiveEnableCmd     HCI_LE_SetPeriodicAdvReceiveEnableCmd
#define IDX_HCI_LE_SetConnectionlessCteTransmitParamsCmd HCI_LE_SetConnectionlessCteTransmitParamsCmd
#define IDX_HCI_LE_SetConnectionlessCteTransmitEnableCmd HCI_LE_SetConnectionlessCteTransmitEnableCmd
#define IDX_HCI_LE_SetConnectionlessIqSamplingEnableCmd HCI_LE_SetConnectionlessIqSamplingEnableCmd
#define IDX_HCI_LE_SetExtScanRspData                    HCI_LE_SetExtScanRspData
#define IDX_HCI_LE_SetExtAdvData                        HCI_LE_SetExtAdvData
#define IDX_HCI_LE_SetAdvStatus                         HCI_LE_SetAdvStatus

/* HCI Extended API */
/********************/
#define IDX_HCI_EXT_SetTxPowerCmd                        HCI_EXT_SetTxPowerCmd
#define IDX_HCI_EXT_OnePktPerEvtCmd                      HCI_EXT_OnePktPerEvtCmd
#define IDX_HCI_EXT_DecryptCmd                           HCI_EXT_DecryptCmd
#define IDX_HCI_EXT_SetLocalSupportedFeaturesCmd         HCI_EXT_SetLocalSupportedFeaturesCmd
#define IDX_HCI_EXT_SetFastTxResponseTimeCmd             HCI_EXT_SetFastTxResponseTimeCmd
#define IDX_HCI_EXT_SetSlaveLatencyOverrideCmd           HCI_EXT_SetSlaveLatencyOverrideCmd
#define IDX_HCI_EXT_ModemTestTxCmd                       HCI_EXT_ModemTestTxCmd
#define IDX_HCI_EXT_ModemHopTestTxCmd                    HCI_EXT_ModemHopTestTxCmd
#define IDX_HCI_EXT_ModemTestRxCmd                       HCI_EXT_ModemTestRxCmd
#define IDX_HCI_EXT_EnhancedModemTestTxCmd               HCI_EXT_EnhancedModemTestTxCmd
#define IDX_HCI_EXT_EnhancedModemHopTestTxCmd            HCI_EXT_EnhancedModemHopTestTxCmd
#define IDX_HCI_EXT_EnhancedModemTestRxCmd               HCI_EXT_EnhancedModemTestRxCmd
#define IDX_HCI_EXT_EndModemTestCmd                      HCI_EXT_EndModemTestCmd
#define IDX_HCI_EXT_SetBDADDRCmd                         HCI_EXT_SetBDADDRCmd
#define IDX_HCI_EXT_SetSCACmd                            HCI_EXT_SetSCACmd
#define IDX_HCI_EXT_EnablePTMCmd                         HCI_EXT_EnablePTMCmd
#define IDX_HCI_EXT_SetMaxDtmTxPowerCmd                  HCI_EXT_SetMaxDtmTxPowerCmd
#define IDX_HCI_EXT_DisconnectImmedCmd                   HCI_EXT_DisconnectImmedCmd
#define IDX_HCI_EXT_PacketErrorRateCmd                   HCI_EXT_PacketErrorRateCmd
#define IDX_HCI_EXT_PERbyChanCmd                         HCI_EXT_PERbyChanCmd
#define IDX_HCI_EXT_BuildRevisionCmd                     HCI_EXT_BuildRevisionCmd
#define IDX_HCI_EXT_DelaySleepCmd                        HCI_EXT_DelaySleepCmd
#define IDX_HCI_EXT_ResetSystemCmd                       HCI_EXT_ResetSystemCmd
#define IDX_HCI_EXT_NumComplPktsLimitCmd                 HCI_EXT_NumComplPktsLimitCmd
#define IDX_HCI_EXT_GetConnInfoCmd                       HCI_EXT_GetConnInfoCmd
#define IDX_HCI_EXT_GetActiveConnInfoCmd                 HCI_EXT_GetActiveConnInfoCmd
#define IDX_HCI_EXT_SetDtmTxPktCntCmd                    HCI_EXT_SetDtmTxPktCntCmd
#define IDX_HCI_EXT_SetHostDefChanClassificationCmd      HCI_EXT_SetHostDefChanClassificationCmd
#define IDX_HCI_EXT_SetHostConnChanClassificationCmd     HCI_EXT_SetHostConnChanClassificationCmd
#define IDX_HCI_TL_getCmdResponderID                     HCI_TL_getCmdResponderID
#define IDX_HCI_LE_RandCmd                               HCI_LE_RandCmd
#define IDX_HCI_LE_ReadBufSizeCmd                        HCI_LE_ReadBufSizeCmd
#define IDX_HCI_LE_SetRandomAddressCmd                   HCI_LE_SetRandomAddressCmd
#define IDX_HCI_DisconnectCmd                            HCI_DisconnectCmd
#define IDX_HCI_SetControllerToHostFlowCtrlCmd           HCI_SetControllerToHostFlowCtrlCmd
#define IDX_HCI_HostBufferSizeCmd                        HCI_HostBufferSizeCmd
#define IDX_HCI_HostNumCompletedPktCmd                   HCI_HostNumCompletedPktCmd
#define IDX_HCI_ReadAuthPayloadTimeoutCmd                HCI_ReadAuthPayloadTimeoutCmd
#define IDX_HCI_WriteAuthPayloadTimeoutCmd               HCI_WriteAuthPayloadTimeoutCmd
#define IDX_HCI_LE_RemoteConnParamReqReplyCmd            HCI_LE_RemoteConnParamReqReplyCmd
#define IDX_HCI_LE_RemoteConnParamReqNegReplyCmd         HCI_LE_RemoteConnParamReqNegReplyCmd
#define IDX_HCI_LE_AddDeviceToResolvingListCmd           HCI_LE_AddDeviceToResolvingListCmd
#define IDX_HCI_LE_RemoveDeviceFromResolvingListCmd      HCI_LE_RemoveDeviceFromResolvingListCmd
#define IDX_HCI_LE_ClearResolvingListCmd                 HCI_LE_ClearResolvingListCmd
#define IDX_HCI_LE_ReadResolvingListSizeCmd              HCI_LE_ReadResolvingListSizeCmd
#define IDX_HCI_LE_ReadPeerResolvableAddressCmd          HCI_LE_ReadPeerResolvableAddressCmd
#define IDX_HCI_LE_ReadLocalResolvableAddressCmd         HCI_LE_ReadLocalResolvableAddressCmd
#define IDX_HCI_LE_SetAddressResolutionEnableCmd         HCI_LE_SetAddressResolutionEnableCmd
#define IDX_HCI_LE_SetResolvablePrivateAddressTimeoutCmd HCI_LE_SetResolvablePrivateAddressTimeoutCmd
#define IDX_HCI_LE_SetPrivacyModeCmd                     HCI_LE_SetPrivacyModeCmd
#define IDX_HCI_LE_ReadLocalP256PublicKeyCmd             HCI_LE_ReadLocalP256PublicKeyCmd
#define IDX_HCI_LE_GenerateDHKeyCmd                      HCI_LE_GenerateDHKeyCmd
#define IDX_HCI_LE_SetAdvParamCmd                        HCI_LE_SetAdvParamCmd
#define IDX_HCI_LE_SetAdvDataCmd                         HCI_LE_SetAdvDataCmd
#define IDX_HCI_LE_SetScanRspDataCmd                     HCI_LE_SetScanRspDataCmd
#define IDX_HCI_LE_SetAdvEnableCmd                       HCI_LE_SetAdvEnableCmd
#define IDX_HCI_LE_SetScanParamCmd                       HCI_LE_SetScanParamCmd
#define IDX_HCI_LE_SetScanEnableCmd                      HCI_LE_SetScanEnableCmd
#define IDX_HCI_LE_CreateConnCmd                         HCI_LE_CreateConnCmd
#define IDX_HCI_LE_CreateConnCancelCmd                   HCI_LE_CreateConnCancelCmd
#define IDX_HCI_LE_StartEncyptCmd                        HCI_LE_StartEncyptCmd
#define IDX_HCI_LE_ConnUpdateCmd                         HCI_LE_ConnUpdateCmd
#define IDX_HCI_LE_LtkReqReplyCmd                        HCI_LE_LtkReqReplyCmd
#define IDX_HCI_LE_LtkReqNegReplyCmd                     HCI_LE_LtkReqNegReplyCmd
#define IDX_HCI_EXT_SetRxGainCmd                         HCI_EXT_SetRxGainCmd
#define IDX_HCI_EXT_ExtendRfRangeCmd                     HCI_EXT_ExtendRfRangeCmd
#define IDX_HCI_EXT_HaltDuringRfCmd                      HCI_EXT_HaltDuringRfCmd
#define IDX_HCI_EXT_ClkDivOnHaltCmd                      HCI_EXT_ClkDivOnHaltCmd
#define IDX_HCI_EXT_DeclareNvUsageCmd                    HCI_EXT_DeclareNvUsageCmd
#define IDX_HCI_EXT_MapPmIoPortCmd                       HCI_EXT_MapPmIoPortCmd
#define IDX_HCI_EXT_SetFreqTuneCmd                       HCI_EXT_SetFreqTuneCmd
#define IDX_HCI_EXT_SaveFreqTuneCmd                      HCI_EXT_SaveFreqTuneCmd
#define IDX_HCI_EXT_OverlappedProcessingCmd              HCI_EXT_OverlappedProcessingCmd
#define IDX_HCI_EXT_SetMaxDataLenCmd                     HCI_EXT_SetMaxDataLenCmd
#define IDX_HCI_EXT_LLTestModeCmd                        HCI_EXT_LLTestModeCmd
#define IDX_HCI_SendDataPkt                              HCI_SendDataPkt
#define IDX_HCI_CommandStatusEvent                       HCI_CommandStatusEvent
#define IDX_HCI_CommandCompleteEvent                     HCI_CommandCompleteEvent
#define IDX_HCI_bm_alloc                                 HCI_bm_alloc
#define IDX_HCI_LE_ReadPhyCmd                            HCI_LE_ReadPhyCmd
#define IDX_HCI_LE_SetDefaultPhyCmd                      HCI_LE_SetDefaultPhyCmd
#define IDX_HCI_LE_SetPhyCmd                             HCI_LE_SetPhyCmd
#define IDX_HCI_LE_EnhancedRxTestCmd                     HCI_LE_EnhancedRxTestCmd
#define IDX_HCI_LE_EnhancedTxTestCmd                     HCI_LE_EnhancedTxTestCmd
#define IDX_HCI_LE_EnhancedCteRxTestCmd                  HCI_LE_EnhancedCteRxTestCmd
#define IDX_HCI_LE_EnhancedCteTxTestCmd                  HCI_LE_EnhancedCteTxTestCmd
#define IDX_LE_SetAdvSetRandAddr                         LE_SetAdvSetRandAddr
#define IDX_LE_SetExtAdvParams                           LE_SetExtAdvParams
#define IDX_LE_SetExtAdvData                             LE_SetExtAdvData
#define IDX_LE_SetExtScanRspData                         LE_SetExtScanRspData
#define IDX_LE_SetExtAdvEnable                           LE_SetExtAdvEnable
#define IDX_LE_RemoveAdvSet                              LE_RemoveAdvSet
#define IDX_LE_ClearAdvSets                              LE_ClearAdvSets
#define IDX_LE_SetExtScanParams                          LE_SetExtScanParams
#define IDX_LE_SetExtScanEnable                          LE_SetExtScanEnable
#define IDX_LE_ReadMaxAdvDataLen                         LE_ReadMaxAdvDataLen
#define IDX_LE_ReadNumSupportedAdvSets                   LE_ReadNumSupportedAdvSets
#define IDX_LL_AE_RegCBack                               LL_AE_RegCBack
#define IDX_LE_ExtCreateConn                             LE_ExtCreateConn
#define IDX_HCI_EXT_SetPinOutputCmd                      HCI_EXT_SetPinOutputCmd
#define IDX_HCI_EXT_SetLocationingAccuracyCmd            HCI_EXT_SetLocationingAccuracyCmd
#define IDX_HCI_EXT_SetVirtualAdvAddrCmd                 HCI_EXT_SetVirtualAdvAddrCmd
#define IDX_HCI_EXT_CoexEnableCmd                        HCI_EXT_CoexEnableCmd
#define IDX_HCI_EXT_SetExtScanChannels                   HCI_EXT_SetExtScanChannels
#define IDX_HCI_EXT_SetQOSParameters                     HCI_EXT_SetQOSParameters
#define IDX_HCI_EXT_SetQOSDefaultParameters              HCI_EXT_SetQOSDefaultParameters

/* L2CAP API */
/*************/
#define IDX_L2CAP_DeregisterPsm                       L2CAP_DeregisterPsm
#define IDX_L2CAP_ConnParamUpdateReq                  L2CAP_ConnParamUpdateReq
#define IDX_L2CAP_ParseParamUpdateReq                 L2CAP_ParseParamUpdateReq
#define IDX_L2CAP_ParseInfoReq                        L2CAP_ParseInfoReq
#define IDX_L2CAP_RegisterPsm                         L2CAP_RegisterPsm
#define IDX_L2CAP_PsmInfo                             L2CAP_PsmInfo
#define IDX_L2CAP_PsmChannels                         L2CAP_PsmChannels
#define IDX_L2CAP_ChannelInfo                         L2CAP_ChannelInfo
#define IDX_L2CAP_ConnectReq                          L2CAP_ConnectReq
#define IDX_L2CAP_ConnectRsp                          L2CAP_ConnectRsp
#define IDX_L2CAP_DisconnectReq                       L2CAP_DisconnectReq
#define IDX_L2CAP_FlowCtrlCredit                      L2CAP_FlowCtrlCredit
#define IDX_L2CAP_SendSDU                             L2CAP_SendSDU
#define IDX_L2CAP_SetParamValue                       L2CAP_SetParamValue
#define IDX_L2CAP_GetParamValue                       L2CAP_GetParamValue
#define IDX_L2CAP_RegisterFlowCtrlTask                L2CAP_RegisterFlowCtrlTask
#define IDX_L2CAP_InfoReq                             L2CAP_InfoReq
#define IDX_L2CAP_RegisterFlowCtrlTask                L2CAP_RegisterFlowCtrlTask

/* GATT API */
/************/
#define IDX_GATT_RegisterForInd                       GATT_RegisterForInd
#define IDX_GATT_RegisterForReq                       GATT_RegisterForReq
#define IDX_GATT_PrepareWriteReq                      GATT_PrepareWriteReq
#define IDX_GATT_ExecuteWriteReq                      GATT_ExecuteWriteReq
#define IDX_GATT_InitClient                           GATT_InitClient
#define IDX_GATT_InitServer                           GATT_InitServer
#define IDX_GATT_SendRsp                              GATT_SendRsp
#define IDX_GATT_GetNextHandle                        GATT_GetNextHandle
#define IDX_GATT_ServiceNumAttrs                      GATT_ServiceNumAttrs
#define IDX_GATT_FindHandle                           GATT_FindHandle
#define IDX_GATT_PrepareWriteReq                      GATT_PrepareWriteReq
#define IDX_GATT_ExecuteWriteReq                      GATT_ExecuteWriteReq
#define IDX_GATT_FindUUIDRec                          GATT_FindUUIDRec
#define IDX_GATT_RegisterService                      GATT_RegisterService
#define IDX_GATT_DeregisterService                    GATT_DeregisterService
#define IDX_GATT_Indication                           GATT_Indication
#define IDX_GATT_ExchangeMTU                          GATT_ExchangeMTU
#define IDX_GATT_DiscAllPrimaryServices               GATT_DiscAllPrimaryServices
#define IDX_GATT_DiscPrimaryServiceByUUID             GATT_DiscPrimaryServiceByUUID
#define IDX_GATT_FindIncludedServices                 GATT_FindIncludedServices
#define IDX_GATT_DiscAllChars                         GATT_DiscAllChars
#define IDX_GATT_DiscCharsByUUID                      GATT_DiscCharsByUUID
#define IDX_GATT_DiscAllCharDescs                     GATT_DiscAllCharDescs
#define IDX_GATT_ReadCharValue                        GATT_ReadCharValue
#define IDX_GATT_ReadUsingCharUUID                    GATT_ReadUsingCharUUID
#define IDX_GATT_ReadLongCharValue                    GATT_ReadLongCharValue
#define IDX_GATT_ReadMultiCharValues                  GATT_ReadMultiCharValues
#define IDX_GATT_WriteCharValue                       GATT_WriteCharValue
#define IDX_GATT_WriteLongCharValue                   GATT_WriteLongCharValue
#define IDX_GATT_ReliableWrites                       GATT_ReliableWrites
#define IDX_GATT_ReadCharDesc                         GATT_ReadCharDesc
#define IDX_GATT_ReadLongCharDesc                     GATT_ReadLongCharDesc
#define IDX_GATT_WriteCharDesc                        GATT_WriteCharDesc
#define IDX_GATT_WriteLongCharDesc                    GATT_WriteLongCharDesc
#define IDX_GATT_Notification                         GATT_Notification
#define IDX_GATT_WriteNoRsp                           GATT_WriteNoRsp
#define IDX_GATT_SignedWriteNoRsp                     GATT_SignedWriteNoRsp
#define IDX_GATT_RegisterForMsgs                      GATT_RegisterForMsgs
#define IDX_GATT_UpdateMTU                            GATT_UpdateMTU
#define IDX_GATT_SetHostToAppFlowCtrl                 GATT_SetHostToAppFlowCtrl
#define IDX_GATT_AppCompletedMsg                      GATT_AppCompletedMsg

/* GATT SERVER APPLICATION API */
/*******************************/
#define IDX_GATTServApp_SendServiceChangedInd         GATTServApp_SendServiceChangedInd
#define IDX_GATTServApp_RegisterService               GATTServApp_RegisterService
#define IDX_GATTServApp_AddService                    GATTServApp_AddService
#define IDX_GATTServApp_AddService                    GATTServApp_AddService
#define IDX_GATTServApp_DeregisterService             GATTServApp_DeregisterService
#define IDX_GATTServApp_SetParameter                  GATTServApp_SetParameter
#define IDX_GATTServApp_GetParameter                  GATTServApp_GetParameter
#define IDX_GATTServApp_SendCCCUpdatedEvent           GATTServApp_SendCCCUpdatedEvent
#define IDX_GATTServApp_ReadRsp                       GATTServApp_ReadRsp
#define IDX_GATTQual_AddService                       GATTQual_AddService
#define IDX_GATTTest_AddService                       GATTTest_AddService
#define IDX_GATTServApp_GetParamValue                 GATTServApp_GetParamValue
#define IDX_GATTServApp_SetParamValue                 GATTServApp_SetParamValue
#define IDX_GATTServApp_RegisterForMsg                GATTServApp_RegisterForMsg

/* LINK DB API */
/***************/
#define IDX_linkDB_NumActive                          linkDB_NumActive
#define IDX_linkDB_GetInfo                            linkDB_GetInfo
#define IDX_linkDB_State                              linkDB_State
#define IDX_linkDB_NumConns                           linkDB_NumConns

/* ATT API */
/***********/
#define IDX_ATT_HandleValueCfm                        ATT_HandleValueCfm
#define IDX_ATT_ErrorRsp                              ATT_ErrorRsp
#define IDX_ATT_ReadBlobRsp                           ATT_ReadBlobRsp
#define IDX_ATT_ExecuteWriteRsp                       ATT_ExecuteWriteRsp
#define IDX_ATT_WriteRsp                              ATT_WriteRsp
#define IDX_ATT_ReadRsp                               ATT_ReadRsp
#define IDX_ATT_ParseExchangeMTUReq                   ATT_ParseExchangeMTUReq
#define IDX_ATT_ExchangeMTURsp                        ATT_ExchangeMTURsp
#define IDX_ATT_FindInfoRsp                           ATT_FindInfoRsp
#define IDX_ATT_FindByTypeValueRsp                    ATT_FindByTypeValueRsp
#define IDX_ATT_ReadByTypeRsp                         ATT_ReadByTypeRsp
#define IDX_ATT_ReadMultiRsp                          ATT_ReadMultiRsp
#define IDX_ATT_ReadByGrpTypeRsp                      ATT_ReadByGrpTypeRsp
#define IDX_ATT_PrepareWriteRsp                       ATT_PrepareWriteRsp
#define IDX_ATT_SetParamValue                         ATT_SetParamValue
#define IDX_ATT_GetParamValue                         ATT_GetParamValue

/* Security Manager API */
/***********/
#define IDX_SM_GetScConfirmOob                        SM_GetScConfirmOob
#define IDX_SM_GetEccKeys                             SM_GetEccKeys
#define IDX_SM_GetDHKey                               SM_GetDHKey
#define IDX_SM_RegisterTask                           SM_RegisterTask
#define IDX_SM_GenerateRandBuf                        SM_GenerateRandBuf
#define IDX_SM_AuthenticatedPairingOnlyMode           SM_AuthenticatedPairingOnlyMode

/* SNV API */
/***********/
#define IDX_osal_snv_read                             osal_snv_read
#define IDX_osal_snv_write                            osal_snv_write

/* UTIL API */
/************/
#define IDX_NPI_RegisterTask                          NPI_RegisterTask
#define IDX_buildRevision                             buildRevision

#endif /* !STACK_LIBRARY */

#endif /* ICALL_API_IDX_H */
