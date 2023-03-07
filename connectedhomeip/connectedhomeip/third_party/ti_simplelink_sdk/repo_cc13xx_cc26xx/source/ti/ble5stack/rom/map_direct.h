/******************************************************************************

 @file  map_direct.h

 @brief File defines a direct mapping for all "MAP_" prefixed functions.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2016-2022, Texas Instruments Incorporated

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

#ifndef MAP_DIRECT_H
#define MAP_DIRECT_H

// Link Layer
#define MAP_LL_AddDeviceToResolvingList                              LL_AddDeviceToResolvingList
#define MAP_LL_AddWhiteListDevice                                    LL_AddWhiteListDevice
#define MAP_LL_AdvReportCback                                        LL_AdvReportCback
#define MAP_LL_AuthPayloadTimeoutExpiredCback                        LL_AuthPayloadTimeoutExpiredCback
#define MAP_LL_ChanMapUpdate                                         LL_ChanMapUpdate
#define MAP_LL_ClearResolvingList                                    LL_ClearResolvingList
#define MAP_LL_ClearWhiteList                                        LL_ClearWhiteList
#define MAP_LL_ConnActive                                            LL_ConnActive
#define MAP_LL_ConnParamUpdateRejectCback                            LL_ConnParamUpdateRejectCback
#define MAP_LL_ConnParamUpdateCback                                  LL_ConnParamUpdateCback
#define MAP_LL_ConnUpdate                                            LL_ConnUpdate
#define MAP_LL_ConnectionCompleteCback                               LL_ConnectionCompleteCback
#define MAP_LL_CreateConn                                            LL_CreateConn
#define MAP_LL_CreateConnCancel                                      LL_CreateConnCancel
#define MAP_LL_CtrlToHostFlowControl                                 LL_CtrlToHostFlowControl
#define MAP_LL_DataLengthChangeEventCback                            LL_DataLengthChangeEventCback
#define MAP_LL_DirectTestEnd                                         LL_DirectTestEnd
#define MAP_LL_DirectTestEndDoneCback                                LL_DirectTestEndDoneCback
#define MAP_LL_DirectTestRxTest                                      LL_DirectTestRxTest
#define MAP_LL_DirectTestTxTest                                      LL_DirectTestTxTest
#define MAP_LL_Disconnect                                            LL_Disconnect
#define MAP_LL_DisconnectCback                                       LL_DisconnectCback
#define MAP_LL_ENC_Init                                              LL_ENC_Init
#define MAP_LL_ENC_AES128_Decrypt                                    LL_ENC_AES128_Decrypt
#define MAP_LL_ENC_AES128_Encrypt                                    LL_ENC_AES128_Encrypt
#define MAP_LL_ENC_Decrypt                                           LL_ENC_Decrypt
#define MAP_LL_ENC_DecryptMsg                                        LL_ENC_DecryptMsg
#define MAP_LL_ENC_Encrypt                                           LL_ENC_Encrypt
#define MAP_LL_ENC_EncryptMsg                                        LL_ENC_EncryptMsg
#define MAP_LL_ENC_GenDeviceIV                                       LL_ENC_GenDeviceIV
#define MAP_LL_ENC_GenDeviceSKD                                      LL_ENC_GenDeviceSKD
#define MAP_LL_ENC_GenerateNonce                                     LL_ENC_GenerateNonce
#define MAP_LL_ENC_GeneratePseudoRandNum                             LL_ENC_GeneratePseudoRandNum
#define MAP_LL_ENC_GenerateTrueRandNum                               LL_ENC_GenerateTrueRandNum
#define MAP_LL_ENC_LoadKey                                           LL_ENC_LoadKey
#define MAP_LL_ENC_ReverseBytes                                      LL_ENC_ReverseBytes
#define MAP_LL_EXT_BuildRevision                                     LL_EXT_BuildRevision
#define MAP_LL_EXT_ClkDivOnHalt                                      LL_EXT_ClkDivOnHalt
#define MAP_LL_EXT_DeclareNvUsage                                    LL_EXT_DeclareNvUsage
#define MAP_LL_EXT_Decrypt                                           LL_EXT_Decrypt
#define MAP_LL_EXT_DelaySleep                                        LL_EXT_DelaySleep
#define MAP_LL_EXT_DisconnectImmed                                   LL_EXT_DisconnectImmed
#define MAP_LL_EXT_EndModemTest                                      LL_EXT_EndModemTest
#define MAP_LL_EXT_EnhancedModemHopTestTx                            LL_EXT_EnhancedModemHopTestTx
#define MAP_LL_EXT_EnhancedModemTestRx                               LL_EXT_EnhancedModemTestRx
#define MAP_LL_EXT_EnhancedModemTestTx                               LL_EXT_EnhancedModemTestTx
#define MAP_LL_EXT_ExtendRfRange                                     LL_EXT_ExtendRfRange
#define MAP_LL_EXT_ExtendRfRangeCback                                LL_EXT_ExtendRfRangeCback
#define MAP_LL_EXT_GetConnInfo                                       LL_EXT_GetConnInfo
#define MAP_LL_EXT_GetActiveConnInfo                                 LL_EXT_GetActiveConnInfo
#define MAP_LL_EXT_SetExtScanChannels                                LL_EXT_SetExtScanChannels
#define MAP_LL_EXT_SetQOSParameters                                  LL_EXT_SetQOSParameters
#define MAP_LL_EXT_SetQOSDefaultParameters                           LL_EXT_SetQOSDefaultParameters
#define MAP_LL_EXT_HaltDuringRf                                      LL_EXT_HaltDuringRf
#define MAP_LL_EXT_MapPmIoPort                                       LL_EXT_MapPmIoPort
#define MAP_LL_EXT_ModemHopTestTx                                    LL_EXT_ModemHopTestTx
#define MAP_LL_EXT_ModemTestRx                                       LL_EXT_ModemTestRx
#define MAP_LL_EXT_ModemTestTx                                       LL_EXT_ModemTestTx
#define MAP_LL_EXT_NumComplPktsLimit                                 LL_EXT_NumComplPktsLimit
#define MAP_LL_EXT_OnePacketPerEvent                                 LL_EXT_OnePacketPerEvent
#define MAP_LL_EXT_OverlappedProcessing                              LL_EXT_OverlappedProcessing
#define MAP_LL_EXT_PERbyChan                                         LL_EXT_PERbyChan
#define MAP_LL_EXT_PacketErrorRate                                   LL_EXT_PacketErrorRate
#define MAP_LL_EXT_PacketErrorRateCback                              LL_EXT_PacketErrorRateCback
#define MAP_LL_EXT_ReadRandomAddress                                 LL_EXT_ReadRandomAddress
#define MAP_LL_EXT_ResetSystem                                       LL_EXT_ResetSystem
#define MAP_LL_EXT_SaveFreqTune                                      LL_EXT_SaveFreqTune
#define MAP_LL_EXT_ScanReqReportCback                                LL_EXT_ScanReqReportCback
#define MAP_LL_EXT_ChanMapUpdateCback                                LL_EXT_ChanMapUpdateCback
#define MAP_LL_EXT_SetBDADDR                                         LL_EXT_SetBDADDR
#define MAP_LL_EXT_SetVirtualAdvAddr                                 LL_EXT_SetVirtualAdvAddr
#define MAP_LL_EXT_SetDtmTxPktCnt                                    LL_EXT_SetDtmTxPktCnt
#define MAP_LL_EXT_SetFastTxResponseTime                             LL_EXT_SetFastTxResponseTime
#define MAP_LL_EXT_SetFreqTune                                       LL_EXT_SetFreqTune
#define MAP_LL_EXT_SetLocalSupportedFeatures                         LL_EXT_SetLocalSupportedFeatures
#define MAP_LL_EXT_SetMaxDataLen                                     LL_EXT_SetMaxDataLen
#define MAP_LL_EXT_SetMaxDtmTxPower                                  LL_EXT_SetMaxDtmTxPower
#define MAP_LL_EXT_SetRxGain                                         LL_EXT_SetRxGain
#define MAP_LL_EXT_SetRxGainCback                                    LL_EXT_SetRxGainCback
#define MAP_LL_EXT_SetSCA                                            LL_EXT_SetSCA
#define MAP_LL_EXT_SetSlaveLatencyOverride                           LL_EXT_SetSlaveLatencyOverride
#define MAP_LL_EXT_SetTxPower                                        LL_EXT_SetTxPower
#define MAP_LL_EXT_SetTxPowerCback                                   LL_EXT_SetTxPowerCback
#define MAP_LL_ChannelSelectionAlgorithmCback                        LL_ChannelSelectionAlgorithmCback
#define MAP_LL_EncChangeCback                                        LL_EncChangeCback
#define MAP_LL_EncKeyRefreshCback                                    LL_EncKeyRefreshCback
#define MAP_LL_EncLtkNegReply                                        LL_EncLtkNegReply
#define MAP_LL_EncLtkReply                                           LL_EncLtkReply
#define MAP_LL_EncLtkReqCback                                        LL_EncLtkReqCback
#define MAP_LL_Encrypt                                               LL_Encrypt
#define MAP_LL_EnhancedConnectionCompleteCback                       LL_EnhancedConnectionCompleteCback
#define MAP_LL_EnhancedRxTest                                        LL_EnhancedRxTest
#define MAP_LL_EnhancedTxTest                                        LL_EnhancedTxTest
#define MAP_LL_GenerateDHKeyCmd                                      LL_GenerateDHKeyCmd
#define MAP_LL_GenerateDHKeyCompleteEventCback                       LL_GenerateDHKeyCompleteEventCback
#define MAP_LL_GetNumActiveConns                                     LL_GetNumActiveConns
#define MAP_LL_Init                                                  LL_Init
#define MAP_LL_NumEmptyWlEntries                                     LL_NumEmptyWlEntries
#define MAP_LL_PRIV_AddExtWLEntry                                    LL_PRIV_AddExtWLEntry
#define MAP_LL_PRIV_Ah                                               LL_PRIV_Ah
#define MAP_LL_PRIV_CheckRLPeerId                                    LL_PRIV_CheckRLPeerId
#define MAP_LL_PRIV_CheckRLPeerIdEntry                               LL_PRIV_CheckRLPeerIdEntry
#define MAP_LL_PRIV_ClearAllPrivIgn                                  LL_PRIV_ClearAllPrivIgn
#define MAP_LL_PRIV_ClearExtWL                                       LL_PRIV_ClearExtWL
#define MAP_LL_PRIV_FindExtWLEntry                                   LL_PRIV_FindExtWLEntry
#define MAP_LL_PRIV_FindPeerInRL                                     LL_PRIV_FindPeerInRL
#define MAP_LL_PRIV_GenerateNRPA                                     LL_PRIV_GenerateNRPA
#define MAP_LL_PRIV_GenerateRPA                                      LL_PRIV_GenerateRPA
#define MAP_LL_PRIV_GenerateRSA                                      LL_PRIV_GenerateRSA
#define MAP_LL_PRIV_Init                                             LL_PRIV_Init
#define MAP_LL_PRIV_IsIDA                                            LL_PRIV_IsIDA
#define MAP_LL_PRIV_IsNRPA                                           LL_PRIV_IsNRPA
#define MAP_LL_PRIV_IsRPA                                            LL_PRIV_IsRPA
#define MAP_LL_PRIV_IsResolvable                                     LL_PRIV_IsResolvable
#define MAP_LL_PRIV_IsZeroIRK                                        LL_PRIV_IsZeroIRK
#define MAP_LL_PRIV_NumberPeerRLEntries                              LL_PRIV_NumberPeerRLEntries
#define MAP_LL_PRIV_ResolveRPA                                       LL_PRIV_ResolveRPA
#define MAP_LL_PRIV_SetWLSize                                        LL_PRIV_SetWLSize
#define MAP_LL_PRIV_SetupPrivacy                                     LL_PRIV_SetupPrivacy
#define MAP_LL_PRIV_TeardownPrivacy                                  LL_PRIV_TeardownPrivacy
#define MAP_LL_PRIV_UpdateExtWLEntry                                 LL_PRIV_UpdateExtWLEntry
#define MAP_LL_PRIV_UpdateRL                                         LL_PRIV_UpdateRL
#define MAP_LL_PhyUpdateCompleteEventCback                           LL_PhyUpdateCompleteEventCback
#define MAP_LL_ProcessEvent                                          LL_ProcessEvent
#define MAP_LL_PseudoRand                                            LL_PseudoRand
#define MAP_LL_RX_bm_alloc                                           LL_RX_bm_alloc
#define MAP_LL_Rand                                                  LL_Rand
#define MAP_LL_RandCback                                             LL_RandCback
#define MAP_LL_ReadAdvChanTxPower                                    LL_ReadAdvChanTxPower
#define MAP_LL_ReadAuthPayloadTimeout                                LL_ReadAuthPayloadTimeout
#define MAP_LL_ReadBDADDR                                            LL_ReadBDADDR
#define MAP_LL_ReadChanMap                                           LL_ReadChanMap
#define MAP_LL_ReadDefaultDataLen                                    LL_ReadDefaultDataLen
#define MAP_LL_ReadLocalP256PublicKeyCmd                             LL_ReadLocalP256PublicKeyCmd
#define MAP_LL_ReadLocalP256PublicKeyCompleteEventCback              LL_ReadLocalP256PublicKeyCompleteEventCback
#define MAP_LL_ReadLocalResolvableAddress                            LL_ReadLocalResolvableAddress
#define MAP_LL_ReadLocalSupportedFeatures                            LL_ReadLocalSupportedFeatures
#define MAP_LL_ReadLocalVersionInfo                                  LL_ReadLocalVersionInfo
#define MAP_LL_ReadMaxDataLen                                        LL_ReadMaxDataLen
#define MAP_LL_ReadPeerResolvableAddress                             LL_ReadPeerResolvableAddress
#define MAP_LL_ReadPhy                                               LL_ReadPhy
#define MAP_LL_ReadRemoteUsedFeatures                                LL_ReadRemoteUsedFeatures
#define MAP_LL_ReadRemoteUsedFeaturesCompleteCback                   LL_ReadRemoteUsedFeaturesCompleteCback
#define MAP_LL_ReadRemoteVersionInfo                                 LL_ReadRemoteVersionInfo
#define MAP_LL_ReadRemoteVersionInfoCback                            LL_ReadRemoteVersionInfoCback
#define MAP_LL_ReadResolvingListSize                                 LL_ReadResolvingListSize
#define MAP_LL_ReadRssi                                              LL_ReadRssi
#define MAP_LL_ReadSupportedStates                                   LL_ReadSupportedStates
#define MAP_LL_ReadTxPowerLevel                                      LL_ReadTxPowerLevel
#define MAP_LL_ReadWlSize                                            LL_ReadWlSize
#define MAP_LL_RemoteConnParamReqCback                               LL_RemoteConnParamReqCback
#define MAP_LL_RemoteConnParamReqNegReply                            LL_RemoteConnParamReqNegReply
#define MAP_LL_RemoteConnParamReqReply                               LL_RemoteConnParamReqReply
#define MAP_LL_RemoveDeviceFromResolvingList                         LL_RemoveDeviceFromResolvingList
#define MAP_LL_RemoveWhiteListDevice                                 LL_RemoveWhiteListDevice
#define MAP_LL_Reset                                                 LL_Reset
#define MAP_LL_RxDataCompleteCback                                   LL_RxDataCompleteCback
#define MAP_LL_SetAddressResolutionEnable                            LL_SetAddressResolutionEnable
#define MAP_LL_SetAdvControl                                         LL_SetAdvControl
#define MAP_LL_SetAdvData                                            LL_SetAdvData
#define MAP_LL_SetAdvParam                                           LL_SetAdvParam
#define MAP_LL_SetDataLen                                            LL_SetDataLen
#define MAP_LL_SetDefaultPhy                                         LL_SetDefaultPhy
#define MAP_LL_SetPrivacyMode                                        LL_SetPrivacyMode
#define MAP_LL_SetPhy                                                LL_SetPhy
#define MAP_LL_SetRandomAddress                                      LL_SetRandomAddress
#define MAP_LL_SetResolvablePrivateAddressTimeout                    LL_SetResolvablePrivateAddressTimeout
#define MAP_LL_SetScanControl                                        LL_SetScanControl
#define MAP_LL_SetScanParam                                          LL_SetScanParam
#define MAP_LL_SetScanRspData                                        LL_SetScanRspData
#define MAP_LL_StartEncrypt                                          LL_StartEncrypt
#define MAP_LL_TX_bm_alloc                                           LL_TX_bm_alloc
#define MAP_LL_TxData                                                LL_TxData
#define MAP_LL_WriteAuthPayloadTimeout                               LL_WriteAuthPayloadTimeout
#define MAP_LL_WriteDefaultDataLen                                   LL_WriteDefaultDataLen
#define MAP_WL_AddEntry                                              WL_AddEntry
#define MAP_WL_Clear                                                 WL_Clear
#define MAP_WL_ClearEntry                                            WL_ClearEntry
#define MAP_WL_ClearIgnoreList                                       WL_ClearIgnoreList
#define MAP_WL_FindEntry                                             WL_FindEntry
#define MAP_WL_GetNumFreeEntries                                     WL_GetNumFreeEntries
#define MAP_WL_GetSize                                               WL_GetSize
#define MAP_WL_Init                                                  WL_Init
#define MAP_WL_RemoveEntry                                           WL_RemoveEntry
#define MAP_WL_SetWlIgnore                                           WL_SetWlIgnore
#define MAP_hciInitEventMasks                                        hciInitEventMasks
#define MAP_llActiveTask                                             llActiveTask
#define MAP_llAdv_TaskAbort                                          llAdv_TaskAbort
#define MAP_llAdv_TaskConnect                                        llAdv_TaskConnect
#define MAP_llAdv_TaskEnd                                            llAdv_TaskEnd
#define MAP_llAlignToNextEvent                                       llAlignToNextEvent
#define MAP_llAllocConnId                                            llAllocConnId
#define MAP_llAllocTask                                              llAllocTask
#define MAP_llAtLeastTwoChans                                        llAtLeastTwoChans
#define MAP_llCBTimer_AptoExpiredCback                               llCBTimer_AptoExpiredCback
#define MAP_llCalcScaFactor                                          llCalcScaFactor
#define MAP_llCheckForLstoDuringSL                                   llCheckForLstoDuringSL
#define MAP_llCheckRxBuffers                                         llCheckRxBuffers
#define MAP_llCheckWhiteListUsage                                    llCheckWhiteListUsage
#define MAP_llClearRfInts                                            llClearRfInts
#define MAP_llCombinePDU                                             llCombinePDU
#define MAP_llConnCleanup                                            llConnCleanup
#define MAP_llConnExists                                             llConnExists
#define MAP_llConnTerminate                                          llConnTerminate
#define MAP_llConvertCtrlProcTimeoutToEvent                          llConvertCtrlProcTimeoutToEvent
#define MAP_llConvertLstoToEvent                                     llConvertLstoToEvent
#define MAP_llCreateRxBuffer                                         llCreateRxBuffer
#define MAP_llDataGetConnPtr                                         llDataGetConnPtr
#define MAP_llGetConExtPtr                                           llGetConExtPtr
#define MAP_llDequeueCtrlPkt                                         llDequeueCtrlPkt
#define MAP_llDirAdv_TaskEnd                                         llDirAdv_TaskEnd
#define MAP_llDisableRfInts                                          llDisableRfInts
#define MAP_llDynamicAlloc                                           llDynamicAlloc
#define MAP_llDynamicFree                                            llDynamicFree
#define MAP_llEnableRfInts                                           llEnableRfInts
#define MAP_llEnqueueCtrlPkt                                         llEnqueueCtrlPkt
#define MAP_llEqAlreadyValidAddr                                     llEqAlreadyValidAddr
#define MAP_llEqSynchWord                                            llEqSynchWord
#define MAP_llEqualBytes                                             llEqualBytes
#define MAP_llEventDelta                                             llEventDelta
#define MAP_llEventInRange                                           llEventInRange
#define MAP_llFindNextAdvSet                                         llFindNextAdvSet
#define MAP_llFindNextSecCmd                                         llFindNextSecCmd
#define MAP_llFindNextSecTask                                        llFindNextSecTask
#define MAP_llFindStartType                                          llFindStartType_hook
#define MAP_llFragmentPDU                                            llFragmentPDU
#define MAP_llFreeTask                                               llFreeTask
#define MAP_llGenerateCRC                                            llGenerateCRC
#define MAP_llGenerateValidAccessAddr                                llGenerateValidAccessAddr
#define MAP_llGetActiveTasks                                         llGetActiveTasks
#define MAP_llGetAdvChanPDU                                          llGetAdvChanPDU
#define MAP_llGetCurrentTask                                         llGetCurrentTask
#define MAP_llGetCurrentTime                                         llGetCurrentTime
#define MAP_llGetMinCI                                               llGetMinCI
#define MAP_llGetNextConn                                            llGetNextConn_hook
#define MAP_llGetNextDataChan                                        llGetNextDataChan
#define MAP_llGetNextDataChanAlgo1                                   llGetNextDataChanAlgo1
#define MAP_llGetNextDataChanAlgo2                                   llGetNextDataChanAlgo2
#define MAP_llGetNumTasks                                            llGetNumTasks
#define MAP_llGenPrnE                                                llGenPrnE
#define MAP_llGetRfOverrideRegs                                      llGetRfOverrideRegs
#define MAP_llGetSlowestPhy                                          llGetSlowestPhy
#define MAP_llGetTask                                                llGetTask
#define MAP_llGetTaskState                                           llGetTaskState
#define MAP_llGetTxPower                                             llGetTxPower
#define MAP_llGtElevenTransitionsInLsh                               llGtElevenTransitionsInLsh
#define MAP_llGtSixConsecZerosOrOnes                                 llGtSixConsecZerosOrOnes
#define MAP_llGtTwentyFourTransitions                                llGtTwentyFourTransitions
#define MAP_llHaltRadio                                              llHaltRadio
#define MAP_llHardwareError                                          llHardwareError
#define MAP_llInitRAT                                                llInitRAT
#define MAP_llInit_TaskConnect                                       llInit_TaskConnect
#define MAP_llInit_TaskEnd                                           llInit_TaskEnd
#define MAP_llLtThreeOnesInLsb                                       llLtThreeOnesInLsb
#define MAP_llLtTwoChangesInLastSixBits                              llLtTwoChangesInLastSixBits
#define MAP_llMemCopyDst                                             llMemCopyDst
#define MAP_llMemCopySrc                                             llMemCopySrc
#define MAP_llMoveTempTxDataEntries                                  llMoveTempTxDataEntries
#define MAP_llOctets2Time                                            llOctets2Time
#define MAP_llOneBitSynchWordDiffer                                  llOneBitSynchWordDiffer
#define MAP_llPatchCM0                                               llPatchCM0
#define MAP_llPendingUpdateParam                                     llPendingUpdateParam
#define MAP_llProcessChanMap                                         llProcessChanMap
#define MAP_llProcessMasterControlPacket                             llProcessMasterControlPacket
#define MAP_llProcessMasterControlProcedures                         llProcessMasterControlProcedures
#define MAP_llProcessPostRfOps                                       llProcessPostRfOps
#define MAP_llProcessScanRxFIFO                                      llProcessScanRxFIFO
#define MAP_llProcessSlaveControlPacket                              llProcessSlaveControlPacket
#define MAP_llProcessSlaveControlProcedures                          llProcessSlaveControlProcedures
#define MAP_llProcessTxData                                          llProcessTxData
#define MAP_llRatChanCBack_A                                         llRatChanCBack_A
#define MAP_llRatChanCBack_B                                         llRatChanCBack_B
#define MAP_llRatChanCBack_C                                         llRatChanCBack_C
#define MAP_llRatChanCBack_D                                         llRatChanCBack_D
#define MAP_llRealignConn                                            llRealignConn
#define MAP_llRegisterConnEvtCallback                                llRegisterConnEvtCallback
#define MAP_llReleaseAllConnId                                       llReleaseAllConnId
#define MAP_llReleaseConnId                                          llReleaseConnId
#define MAP_llReplaceCtrlPkt                                         llReplaceCtrlPkt
#define MAP_llReplaceRxBuffers                                       llReplaceRxBuffers
#define MAP_llResetRadio                                             llResetRadio
#define MAP_llReverseBits                                            llReverseBits
#define MAP_llRfInit                                                 llRfInit
#define MAP_llRfSetup                                                llRfSetup
#define MAP_llRfStartFS                                              llRfStartFS
#define MAP_llScan_TaskEnd                                           llScan_TaskEnd
#define MAP_llScheduleTask                                           llScheduleTask
#ifdef ONE_BLE_LIB_SIZE_OPTIMIZATION
#define MAP_llScheduler                                              llScheduler_hook
#else
#define MAP_llScheduler                                              llScheduler
#endif
#define MAP_llSchedulerInit                                          llSchedulerInit
#define MAP_llSendReject                                             llSendReject
#define MAP_llSetCodedMaxTxTime                                      llSetCodedMaxTxTime
#define MAP_llSetFreqTune                                            llSetFreqTune
#define MAP_llSetNextDataChan                                        llSetNextDataChan
#define MAP_llSetTxPower                                             llSetTxPower
#define MAP_llSetTxPwrLegacy                                         llSetTxPwrLegacy
#define MAP_llSetupAdv                                               llSetupAdv
#define MAP_llSetupAdvDataEntryQueue                                 llSetupAdvDataEntryQueue
#define MAP_llSetupConn                                              llSetupConn
#define MAP_llSetupConnParamReq                                      llSetupConnParamReq
#define MAP_llSetupConnParamRsp                                      llSetupConnParamRsp
#define MAP_llSetupConnRxDataEntryQueue                              llSetupConnRxDataEntryQueue
#define MAP_llAddTxDataEntry                                         llAddTxDataEntry
#define MAP_llSetupEncReq                                            llSetupEncReq
#define MAP_llSetupEncRsp                                            llSetupEncRsp
#define MAP_llSetupFeatureSetReq                                     llSetupFeatureSetReq
#define MAP_llSetupFeatureSetRsp                                     llSetupFeatureSetRsp
#define MAP_llSetupInit                                              llSetupInit
#define MAP_llSetupInitDataEntryQueue                                llSetupInitDataEntryQueue
#define MAP_llSetupLenCtrlPkt                                        llSetupLenCtrlPkt
#define MAP_llSetupNextMasterEvent                                   llSetupNextMasterEvent
#define MAP_llSetupNextSlaveEvent                                    llSetupNextSlaveEvent
#define MAP_llSetupPauseEncReq                                       llSetupPauseEncReq
#define MAP_llSetupPauseEncRsp                                       llSetupPauseEncRsp
#define MAP_llSetupPhyCtrlPkt                                        llSetupPhyCtrlPkt
#define MAP_llSetupPingReq                                           llSetupPingReq
#define MAP_llSetupPingRsp                                           llSetupPingRsp
#define MAP_llSetupRATChanCompare                                    llSetupRATChanCompare
#define MAP_llSetupRejectInd                                         llSetupRejectInd
#define MAP_llSetupRejectIndExt                                      llSetupRejectIndExt
#define MAP_llSetupRfHal                                             llSetupRfHal
#define MAP_llSetupScan                                              llSetupScan
#define MAP_llSetupScanDataEntryQueue                                llSetupScanDataEntryQueue
#define MAP_llSetupStartEncReq                                       llSetupStartEncReq
#define MAP_llSetupStartEncRsp                                       llSetupStartEncRsp
#define MAP_llSetupTermInd                                           llSetupTermInd
#define MAP_llSetupUnknownRsp                                        llSetupUnknownRsp
#define MAP_llSetupUpdateChanReq                                     llSetupUpdateChanReq
#define MAP_llSetupUpdateParamReq                                    llSetupUpdateParamReq
#define MAP_llSetupVersionIndReq                                     llSetupVersionIndReq
#define MAP_llShellSortActiveConns                                   llShellSortActiveConns
#define MAP_llSortActiveConns                                        llSortActiveConns
#define MAP_llTaskError                                              llTaskError
#define MAP_llTime2Octets                                            llTime2Octets
#define MAP_llTimeCompare                                            llTimeCompare
#define MAP_llTimeDelta                                              llTimeDelta
#define MAP_llValidAccessAddr                                        llValidAccessAddr
#define MAP_llVerifyCodedConnInterval                                llVerifyCodedConnInterval
#define MAP_llVerifyConnParamReqParams                               llVerifyConnParamReqParams
#define MAP_llValidateConnParams                                     llValidateConnParams
#define MAP_llWriteTxData                                            llWriteTxData
#define MAP_LL_PM_Init                                               LL_PM_Init
#define MAP_LL_PM_GetRfCoreState                                     LL_PM_GetRfCoreState
#define MAP_LL_PM_StartRfTask                                        LL_PM_StartRfTask
#define MAP_LL_PM_PowerOnReq                                         LL_PM_PowerOnReq
#define MAP_LL_PM_PowerCycleRadio                                    LL_PM_PowerCycleRadio
#define MAP_LL_PM_ForceSysBusThroughRF                               LL_PM_ForceSysBusThroughRF
#define MAP_LL_PM_Enter_AES                                          LL_PM_Enter_AES
#define MAP_LL_PM_Exit_AES                                           LL_PM_Exit_AES
#define MAP_LL_PM_PowerOnRfCore                                      LL_PM_PowerOnRfCore
#define MAP_LL_PM_PowerOffRfCore                                     LL_PM_PowerOffRfCore
#define MAP_LL_PM_StopCurTaskTimer                                   LL_PM_StopCurTaskTimer
#define MAP_LL_PM_RtcSynchToRAT                                      LL_PM_RtcSynchToRAT
#define MAP_LL_PM_RtcSynchFromRAT                                    LL_PM_RtcSynchFromRAT
#define MAP_LL_PM_ShutdownFS                                         LL_PM_ShutdownFS
#define MAP_LL_PM_EnablePMNotifyHandler                              LL_PM_EnablePMNotifyHandler
#define MAP_LL_PM_TimeToNextRfEvent                                  LL_PM_TimeToNextRfEvent
#define MAP_LL_PM_SetTimerForWakeup                                  LL_PM_SetTimerForWakeup
#define MAP_LL_PM_SetPowerMgrRequirements                            LL_PM_SetPowerMgrRequirements
#define MAP_LL_PM_ReleasePowerMgrRequirements                        LL_PM_ReleasePowerMgrRequirements
#define MAP_LL_PM_PowerOnRfCoreOptimize                              LL_PM_PowerOnRfCoreOptimize
#define MAP_LL_PM_PowerOnRfCoreOptimize                              LL_PM_PowerOnRfCoreOptimize
#define MAP_LL_PM_SendWakeUpCmd                                      LL_PM_SendWakeUpCmd
#define MAP_LL_PM_WakeupHandler                                      LL_PM_WakeupHandler
#define MAP_LL_PM_PowerOffRfCoreOptimize                             LL_PM_PowerOffRfCoreOptimize
#define MAP_LL_PM_RFStartupState_2                                   LL_PM_RFStartupState_2
#define MAP_LL_PM_RFStartupState_3                                   LL_PM_RFStartupState_3
#define MAP_ll_eccInit                                               ll_eccInit
#define MAP_ll_ReadLocalP256PublicKey                                ll_ReadLocalP256PublicKey
#define MAP_ll_GenerateDHKey                                         ll_GenerateDHKey
#define MAP_LL_DeInit                                                LL_DeInit
#define MAP_LL_ReInit                                                LL_ReInit
// V5.0
#define MAP_LL_ReadPhy                                               LL_ReadPhy
#define MAP_LL_SetDefaultPhy                                         LL_SetDefaultPhy
#define MAP_LL_SetPhy                                                LL_SetPhy
#define MAP_LL_PhyUpdateCompleteEventCback                           LL_PhyUpdateCompleteEventCback
#define MAP_LL_EnhancedRxTest                                        LL_EnhancedRxTest
#define MAP_LL_EnhancedTxTest                                        LL_EnhancedTxTest
#define MAP_llSetupPhyCtrlPkt                                        llSetupPhyCtrlPkt
#define MAP_llGetRfOverrideRegs                                      llGetRfOverrideRegs
#define MAP_llLtThreeOnesInLsb                                       llLtThreeOnesInLsb
#define MAP_llGtElevenTransitionsInLsh                               llGtElevenTransitionsInLsh
#define MAP_llRemoveFromFeatureSet                                   llRemoveFromFeatureSet

// HCI
#define MAP_HCI_CommandCompleteEvent                                 HCI_CommandCompleteEvent
#define MAP_HCI_CommandStatusEvent                                   HCI_CommandStatusEvent
#define MAP_HCI_DataBufferOverflowEvent                              HCI_DataBufferOverflowEvent
#define MAP_HCI_DisconnectCmd                                        HCI_DisconnectCmd
#define MAP_HCI_EXT_BuildRevisionCmd                                 HCI_EXT_BuildRevisionCmd
#define MAP_HCI_EXT_ClkDivOnHaltCmd                                  HCI_EXT_ClkDivOnHaltCmd
#define MAP_HCI_EXT_DeclareNvUsageCmd                                HCI_EXT_DeclareNvUsageCmd
#define MAP_HCI_EXT_DecryptCmd                                       HCI_EXT_DecryptCmd
#define MAP_HCI_EXT_DelaySleepCmd                                    HCI_EXT_DelaySleepCmd
#define MAP_HCI_EXT_DisconnectImmedCmd                               HCI_EXT_DisconnectImmedCmd
#define MAP_HCI_EXT_EnablePTMCmd                                     HCI_EXT_EnablePTMCmd
#define MAP_HCI_EXT_EndModemTestCmd                                  HCI_EXT_EndModemTestCmd
#define MAP_HCI_EXT_EnhancedModemHopTestTxCmd                        HCI_EXT_EnhancedModemHopTestTxCmd
#define MAP_HCI_EXT_EnhancedModemTestRxCmd                           HCI_EXT_EnhancedModemTestRxCmd
#define MAP_HCI_EXT_EnhancedModemTestTxCmd                           HCI_EXT_EnhancedModemTestTxCmd
#define MAP_HCI_EXT_ExtendRfRangeCmd                                 HCI_EXT_ExtendRfRangeCmd
#define MAP_HCI_EXT_GetConnInfoCmd                                   HCI_EXT_GetConnInfoCmd
#define MAP_HCI_EXT_HaltDuringRfCmd                                  HCI_EXT_HaltDuringRfCmd
#define MAP_HCI_EXT_MapPmIoPortCmd                                   HCI_EXT_MapPmIoPortCmd
#define MAP_HCI_EXT_ModemHopTestTxCmd                                HCI_EXT_ModemHopTestTxCmd
#define MAP_HCI_EXT_ModemTestRxCmd                                   HCI_EXT_ModemTestRxCmd
#define MAP_HCI_EXT_ModemTestTxCmd                                   HCI_EXT_ModemTestTxCmd
#define MAP_HCI_EXT_NumComplPktsLimitCmd                             HCI_EXT_NumComplPktsLimitCmd
#define MAP_HCI_EXT_OnePktPerEvtCmd                                  HCI_EXT_OnePktPerEvtCmd
#define MAP_HCI_EXT_OverlappedProcessingCmd                          HCI_EXT_OverlappedProcessingCmd
#define MAP_HCI_EXT_PERbyChanCmd                                     HCI_EXT_PERbyChanCmd
#define MAP_HCI_EXT_PacketErrorRateCmd                               HCI_EXT_PacketErrorRateCmd
#define MAP_HCI_EXT_ResetSystemCmd                                   HCI_EXT_ResetSystemCmd
#define MAP_HCI_EXT_SaveFreqTuneCmd                                  HCI_EXT_SaveFreqTuneCmd
#define MAP_HCI_EXT_SetBDADDRCmd                                     HCI_EXT_SetBDADDRCmd
#define MAP_HCI_EXT_SetDtmTxPktCntCmd                                HCI_EXT_SetDtmTxPktCntCmd
#define MAP_HCI_EXT_SetFastTxResponseTimeCmd                         HCI_EXT_SetFastTxResponseTimeCmd
#define MAP_HCI_EXT_SetFreqTuneCmd                                   HCI_EXT_SetFreqTuneCmd
#define MAP_HCI_EXT_SetLocalSupportedFeaturesCmd                     HCI_EXT_SetLocalSupportedFeaturesCmd
#define MAP_HCI_EXT_SetMaxDataLenCmd                                 HCI_EXT_SetMaxDataLenCmd
#define MAP_HCI_EXT_SetMaxDtmTxPowerCmd                              HCI_EXT_SetMaxDtmTxPowerCmd
#define MAP_HCI_EXT_SetRxGainCmd                                     HCI_EXT_SetRxGainCmd
#define MAP_HCI_EXT_SetSCACmd                                        HCI_EXT_SetSCACmd
#define MAP_HCI_EXT_SetSlaveLatencyOverrideCmd                       HCI_EXT_SetSlaveLatencyOverrideCmd
#define MAP_HCI_EXT_SetTxPowerCmd                                    HCI_EXT_SetTxPowerCmd
#define MAP_HCI_EXT_SetVirtualAdvAddrCmd                             HCI_EXT_SetVirtualAdvAddrCmd
#define MAP_HCI_EXT_ReadRandAddr                                     HCI_EXT_ReadRandAddr
#define MAP_HCI_EXT_SetExtScanChannels                               HCI_EXT_SetExtScanChannels
#define MAP_HCI_EXT_SetQOSParameters                                 HCI_EXT_SetQOSParameters
#define MAP_HCI_EXT_SetQOSDefaultParameters                          HCI_EXT_SetQOSDefaultParameters
#define MAP_HCI_EXT_SetHostDefChanClassificationCmd                  HCI_EXT_SetHostDefChanClassificationCmd
#define MAP_HCI_EXT_SetHostConnChanClassificationCmd                 HCI_EXT_SetHostConnChanClassificationCmd
#define MAP_HCI_HardwareErrorEvent                                   HCI_HardwareErrorEvent
#define MAP_HCI_HostBufferSizeCmd                                    HCI_HostBufferSizeCmd
#define MAP_HCI_HostNumCompletedPktCmd                               HCI_HostNumCompletedPktCmd
#define MAP_HCI_LE_AddDeviceToResolvingListCmd                       HCI_LE_AddDeviceToResolvingListCmd
#define MAP_HCI_LE_AddWhiteListCmd                                   HCI_LE_AddWhiteListCmd
#define MAP_HCI_LE_ClearResolvingListCmd                             HCI_LE_ClearResolvingListCmd
#define MAP_HCI_LE_ClearWhiteListCmd                                 HCI_LE_ClearWhiteListCmd
#define MAP_HCI_LE_ConnUpdateCmd                                     HCI_LE_ConnUpdateCmd
#define MAP_HCI_LE_CreateConnCancelCmd                               HCI_LE_CreateConnCancelCmd
#define MAP_HCI_LE_CreateConnCmd                                     HCI_LE_CreateConnCmd
#define MAP_HCI_LE_EncryptCmd                                        HCI_LE_EncryptCmd
#define MAP_HCI_LE_EnhancedRxTestCmd                                 HCI_LE_EnhancedRxTestCmd
#define MAP_HCI_LE_EnhancedTxTestCmd                                 HCI_LE_EnhancedTxTestCmd
#define MAP_HCI_LE_EnhancedCteRxTestCmd                              HCI_LE_EnhancedCteRxTestCmd
#define MAP_HCI_LE_EnhancedCteTxTestCmd                              HCI_LE_EnhancedCteTxTestCmd
#define MAP_HCI_LE_GenerateDHKeyCmd                                  HCI_LE_GenerateDHKeyCmd
#define MAP_HCI_LE_LtkReqNegReplyCmd                                 HCI_LE_LtkReqNegReplyCmd
#define MAP_HCI_LE_LtkReqReplyCmd                                    HCI_LE_LtkReqReplyCmd
#define MAP_HCI_LE_RandCmd                                           HCI_LE_RandCmd
#define MAP_HCI_LE_ReadAdvChanTxPowerCmd                             HCI_LE_ReadAdvChanTxPowerCmd
#define MAP_HCI_LE_ReadBufSizeCmd                                    HCI_LE_ReadBufSizeCmd
#define MAP_HCI_LE_ReadChannelMapCmd                                 HCI_LE_ReadChannelMapCmd
#define MAP_HCI_LE_ReadLocalP256PublicKeyCmd                         HCI_LE_ReadLocalP256PublicKeyCmd
#define MAP_HCI_LE_ReadLocalResolvableAddressCmd                     HCI_LE_ReadLocalResolvableAddressCmd
#define MAP_HCI_LE_ReadLocalSupportedFeaturesCmd                     HCI_LE_ReadLocalSupportedFeaturesCmd
#define MAP_HCI_LE_ReadMaxDataLenCmd                                 HCI_LE_ReadMaxDataLenCmd
#define MAP_HCI_LE_ReadPeerResolvableAddressCmd                      HCI_LE_ReadPeerResolvableAddressCmd
#define MAP_HCI_LE_ReadPhyCmd                                        HCI_LE_ReadPhyCmd
#define MAP_HCI_LE_ReadRemoteUsedFeaturesCmd                         HCI_LE_ReadRemoteUsedFeaturesCmd
#define MAP_HCI_LE_ReadResolvingListSizeCmd                          HCI_LE_ReadResolvingListSizeCmd
#define MAP_HCI_LE_ReadSuggestedDefaultDataLenCmd                    HCI_LE_ReadSuggestedDefaultDataLenCmd
#define MAP_HCI_LE_ReadSupportedStatesCmd                            HCI_LE_ReadSupportedStatesCmd
#define MAP_HCI_LE_ReadWhiteListSizeCmd                              HCI_LE_ReadWhiteListSizeCmd
#define MAP_HCI_LE_ReceiverTestCmd                                   HCI_LE_ReceiverTestCmd
#define MAP_HCI_LE_RemoteConnParamReqNegReplyCmd                     HCI_LE_RemoteConnParamReqNegReplyCmd
#define MAP_HCI_LE_RemoteConnParamReqReplyCmd                        HCI_LE_RemoteConnParamReqReplyCmd
#define MAP_HCI_LE_RemoveDeviceFromResolvingListCmd                  HCI_LE_RemoveDeviceFromResolvingListCmd
#define MAP_HCI_LE_RemoveWhiteListCmd                                HCI_LE_RemoveWhiteListCmd
#define MAP_HCI_LE_SetAddressResolutionEnableCmd                     HCI_LE_SetAddressResolutionEnableCmd
#define MAP_HCI_LE_SetAdvDataCmd                                     HCI_LE_SetAdvDataCmd
#define MAP_HCI_LE_SetAdvEnableCmd                                   HCI_LE_SetAdvEnableCmd
#define MAP_HCI_LE_SetAdvParamCmd                                    HCI_LE_SetAdvParamCmd
#define MAP_HCI_LE_SetDataLenCmd                                     HCI_LE_SetDataLenCmd
#define MAP_HCI_LE_SetDefaultPhyCmd                                  HCI_LE_SetDefaultPhyCmd
#define MAP_HCI_LE_SetEventMaskCmd                                   HCI_LE_SetEventMaskCmd
#define MAP_HCI_LE_SetHostChanClassificationCmd                      HCI_LE_SetHostChanClassificationCmd
#define MAP_HCI_LE_SetPrivacyModeCmd                                 HCI_LE_SetPrivacyModeCmd
#define MAP_HCI_LE_SetPhyCmd                                         HCI_LE_SetPhyCmd
#define MAP_HCI_LE_SetRandomAddressCmd                               HCI_LE_SetRandomAddressCmd
#define MAP_HCI_LE_SetResolvablePrivateAddressTimeoutCmd             HCI_LE_SetResolvablePrivateAddressTimeoutCmd
#define MAP_HCI_LE_SetScanEnableCmd                                  HCI_LE_SetScanEnableCmd
#define MAP_HCI_LE_SetScanParamCmd                                   HCI_LE_SetScanParamCmd
#define MAP_HCI_LE_SetScanRspDataCmd                                 HCI_LE_SetScanRspDataCmd
#define MAP_HCI_LE_StartEncyptCmd                                    HCI_LE_StartEncyptCmd
#define MAP_HCI_LE_TestEndCmd                                        HCI_LE_TestEndCmd
#define MAP_HCI_LE_TransmitterTestCmd                                HCI_LE_TransmitterTestCmd
#define MAP_HCI_LE_WriteSuggestedDefaultDataLenCmd                   HCI_LE_WriteSuggestedDefaultDataLenCmd
#define MAP_HCI_NumOfCompletedPacketsEvent                           HCI_NumOfCompletedPacketsEvent
#define MAP_HCI_ReadAuthPayloadTimeoutCmd                            HCI_ReadAuthPayloadTimeoutCmd
#define MAP_HCI_ReadBDADDRCmd                                        HCI_ReadBDADDRCmd
#define MAP_HCI_ReadLocalSupportedCommandsCmd                        HCI_ReadLocalSupportedCommandsCmd
#define MAP_HCI_ReadLocalSupportedFeaturesCmd                        HCI_ReadLocalSupportedFeaturesCmd
#define MAP_HCI_ReadLocalVersionInfoCmd                              HCI_ReadLocalVersionInfoCmd
#define MAP_HCI_ReadRemoteVersionInfoCmd                             HCI_ReadRemoteVersionInfoCmd
#define MAP_HCI_ReadRssiCmd                                          HCI_ReadRssiCmd
#define MAP_HCI_ReadTransmitPowerLevelCmd                            HCI_ReadTransmitPowerLevelCmd
#define MAP_HCI_ResetCmd                                             HCI_ResetCmd
#define MAP_HCI_ReverseBytes                                         HCI_ReverseBytes
#define MAP_HCI_SendCommandCompleteEvent                             HCI_SendCommandCompleteEvent
#define MAP_HCI_SendCommandStatusEvent                               HCI_SendCommandStatusEvent
#define MAP_HCI_SendControllerToHostEvent                            HCI_SendControllerToHostEvent
#define MAP_HCI_SendDataPkt                                          HCI_SendDataPkt
#define MAP_HCI_SetControllerToHostFlowCtrlCmd                       HCI_SetControllerToHostFlowCtrlCmd
#define MAP_HCI_SetEventMaskCmd                                      HCI_SetEventMaskCmd
#define MAP_HCI_SetEventMaskPage2Cmd                                 HCI_SetEventMaskPage2Cmd
#define MAP_HCI_ValidConnTimeParams                                  HCI_ValidConnTimeParams
#define MAP_HCI_VendorSpecifcCommandCompleteEvent                    HCI_VendorSpecifcCommandCompleteEvent
#define MAP_HCI_WriteAuthPayloadTimeoutCmd                           HCI_WriteAuthPayloadTimeoutCmd
#define MAP_HCI_bm_alloc                                             HCI_bm_alloc
#define MAP_HCI_L2CAPTaskRegister                                    HCI_L2CAPTaskRegister
#define MAP_HCI_GAPTaskRegister                                      HCI_GAPTaskRegister
#define MAP_HCI_SMPTaskRegister                                      HCI_SMPTaskRegister
// V5.0
#define MAP_HCI_LE_ReadPhyCmd                                        HCI_LE_ReadPhyCmd
#define MAP_HCI_LE_SetDefaultPhyCmd                                  HCI_LE_SetDefaultPhyCmd
#define MAP_HCI_LE_SetPhyCmd                                         HCI_LE_SetPhyCmd
#define MAP_HCI_LE_EnhancedRxTestCmd                                 HCI_LE_EnhancedRxTestCmd
#define MAP_HCI_LE_EnhancedTxTestCmd                                 HCI_LE_EnhancedTxTestCmd
#define MAP_HCI_LE_EnhancedCteRxTestCmd                              HCI_LE_EnhancedCteRxTestCmd
#define MAP_HCI_LE_EnhancedCteTxTestCmd                              HCI_LE_EnhancedCteTxTestCmd
#define MAP_HCI_LE_ReadTxPowerCmd                                    HCI_LE_ReadTxPowerCmd
#define MAP_HCI_LE_ReadRfPathCompCmd                                 HCI_LE_ReadRfPathCompCmd
#define MAP_HCI_LE_WriteRfPathCompCmd                                HCI_LE_WriteRfPathCompCmd
// V5.0 LE AE API
#define MAP_LE_SetAdvSetRandAddr                                     LE_SetAdvSetRandAddr
#define MAP_LE_SetExtAdvParams                                       LE_SetExtAdvParams
#define MAP_LE_SetExtAdvData                                         LE_SetExtAdvData_hook
#define MAP_LE_SetExtScanRspData                                     LE_SetExtScanRspData_hook
#define MAP_LE_SetExtAdvEnable                                       LE_SetExtAdvEnable_hook
#define MAP_LE_RemoveAdvSet                                          LE_RemoveAdvSet
#define MAP_LE_ClearAdvSets                                          LE_ClearAdvSets
#define MAP_LE_ReadMaxAdvDataLen                                     LE_ReadMaxAdvDataLen
#define MAP_LE_ReadNumSupportedAdvSets                               LE_ReadNumSupportedAdvSets
#define MAP_LE_AE_SetData                                            LE_AE_SetData
#define MAP_LE_SetExtScanParams                                      LE_SetExtScanParams
#define MAP_LE_SetExtScanEnable                                      LE_SetExtScanEnable
#define MAP_LE_ExtCreateConn                                         LE_ExtCreateConn
#define MAP_LE_ReadTxPowerCmd                                        LE_ReadTxPowerCmd
#define MAP_LE_ReadRfPathCompCmd                                     LE_ReadRfPathCompCmd
#define MAP_LE_WriteRfPathCompCmd                                    LE_WriteRfPathCompCmd
// V5.0 AE Callbacks
#define MAP_LE_ExtAdvRptEvt                                          LE_ExtAdvRptEvt
// V5.0 LL AE API
#define MAP_LL_AE_Init                                               LL_AE_Init
#define MAP_LL_AE_RegCBack                                           LL_AE_RegCBack
#define MAP_LL_GetAdvSet                                             LL_GetAdvSet
#define MAP_LL_CountAdvSets                                          LL_CountAdvSets
#define MAP_LL_DisableAdvSets                                        LL_DisableAdvSets
// V5.0 LL Internal API
#define MAP_llUpdateSortedAdvList                                    llUpdateSortedAdvList
#define MAP_llSetAETimeConsume                                       llSetAETimeConsume
#define MAP_llDetachNode                                             llDetachNode
#define MAP_llAddAdvSortedEntry                                      llAddAdvSortedEntry
#define MAP_llAllocRfMem                                             llAllocRfMem
#define MAP_llSetupExtAdvLegacy                                      llSetupExtAdvLegacy
#define MAP_llSetupExtAdv                                            llSetupExtAdv_hook
#define MAP_llSetupExtScan                                           llSetupExtScan_sPatch
#define MAP_llSetupExtInit                                           llSetupExtInit_sPatch
#define MAP_llSetupExtHdr                                            llSetupExtHdr
#define MAP_llSetupExtData                                           llSetupExtData
#define MAP_llGetExtHdrLen                                           llGetExtHdrLen
#define MAP_llNextChanIndex                                          llNextChanIndex_hook
#define MAP_llTxPwrPoutLU                                            llTxPwrPoutLU
#define MAP_llTxPwrLU                                                llTxPwrLU
#define MAP_llTxPwrSetRfGainIndex                                    llTxPwrSetRfGainIndex
#define MAP_llTxPwrSwitchPA                                          llTxPwrSwitchPA
#define MAP_llTxPwrSetRfCmdType                                      llTxPwrSetRfCmdType
#define MAP_llExtAdvCBack                                            llExtAdvCBack
#define MAP_llCheckCBack                                             llCheckCBack
#define MAP_llEndExtAdvTask                                          llEndExtAdvTask
#define MAP_llEndExtScanTask                                         llEndExtScanTask
#define MAP_llEndExtInitTask                                         llEndExtInitTask
// RF Post Processing
#define MAP_llExtAdv_PostProcess                                     llExtAdv_PostProcess
#define MAP_llExtScan_PostProcess                                    llExtScan_PostProcess
#define MAP_llExtInit_PostProcess                                    llExtInit_PostProcess
// RF Event Processing
#define MAP_llProcessExtScanRxFIFO                                   llProcessExtScanRxFIFO
#define MAP_llSendAdvSetTermEvent                                    llSendAdvSetTermEvent
#define MAP_llSendAdvSetEndEvent                                     llSendAdvSetEndEvent
#define MAP_llStartDurationTimer                                     llStartDurationTimer
// Other
#define MAP_llExtAdvSchedSetup                                       llExtAdvSchedSetup
#define MAP_llExtScanSchedSetup                                      llExtScanSchedSetup
#define MAP_llExtInitSchedSetup                                      llExtInitSchedSetup
#define MAP_llLinkSchedSetup                                         llLinkSchedSetup
#define MAP_llSetupRatCompare                                        llSetupRatCompare
#define MAP_llClearRatCompare                                        llClearRatCompare
// Link DB
#define MAP_linkDB_Add                                               linkDB_Add_sPatch
#define MAP_linkDB_Authen                                            linkDB_Authen
#define MAP_linkDB_Find                                              linkDB_Find
#define MAP_linkDB_Init                                              linkDB_Init
#define MAP_linkDB_MTU                                               linkDB_MTU
#define MAP_linkDB_NumActive                                         linkDB_NumActive
#define MAP_linkDB_NumConns                                          linkDB_NumConns
#define MAP_linkDB_Register                                          linkDB_Register
#define MAP_linkDB_Remove                                            linkDB_Remove
#define MAP_linkDB_Role                                              linkDB_Role
#define MAP_linkDB_State                                             linkDB_State
#define MAP_linkDB_Update                                            linkDB_Update
#define MAP_linkDB_UpdateMTU                                         linkDB_UpdateMTU
#define MAP_linkDB_reportStatusChange                                linkDB_reportStatusChange
#define MAP_linkDB_SecurityModeSCOnly                                linkDB_SecurityModeSCOnly
#define MAP_linkDB_updateConnParam                                   linkDB_updateConnParam

// L2CAP
#define MAP_L2CAP_BuildCmdReject                                     L2CAP_BuildCmdReject
#define MAP_L2CAP_BuildConnectReq                                    L2CAP_BuildConnectReq
#define MAP_L2CAP_BuildParamUpdateRsp                                L2CAP_BuildParamUpdateRsp
#define MAP_L2CAP_CmdReject                                          L2CAP_CmdReject
#define MAP_L2CAP_ConnParamUpdateReq                                 L2CAP_ConnParamUpdateReq
#define MAP_L2CAP_ConnParamUpdateRsp                                 L2CAP_ConnParamUpdateRsp
#define MAP_L2CAP_DisconnectReq                                      L2CAP_DisconnectReq
#define MAP_L2CAP_GetMTU                                             L2CAP_GetMTU
#define MAP_L2CAP_GetParamValue                                      L2CAP_GetParamValue
#define MAP_L2CAP_HostNumCompletedPkts                               L2CAP_HostNumCompletedPkts
#define MAP_L2CAP_ParseConnectReq                                    L2CAP_ParseConnectReq
#define MAP_L2CAP_ParseFlowCtrlCredit                                L2CAP_ParseFlowCtrlCredit
#define MAP_L2CAP_ParseParamUpdateReq                                L2CAP_ParseParamUpdateReq
#define MAP_L2CAP_RegisterApp                                        L2CAP_RegisterApp
#define MAP_L2CAP_SendData                                           L2CAP_SendData
#define MAP_L2CAP_SendDataPkt                                        L2CAP_SendDataPkt
#define MAP_L2CAP_SetBufSize                                         L2CAP_SetBufSize
#define MAP_L2CAP_SetControllerToHostFlowCtrl                        L2CAP_SetControllerToHostFlowCtrl
#define MAP_L2CAP_bm_alloc                                           L2CAP_bm_alloc
#define MAP_l2capAllocChannel                                        l2capAllocChannel
#define MAP_l2capAllocConnChannel                                    l2capAllocConnChannel
#define MAP_l2capAllocPsm                                            l2capAllocPsm
#define MAP_l2capBuildConnectRsp                                     l2capBuildConnectRsp
#define MAP_l2capBuildDisconnectReq                                  l2capBuildDisconnectReq
#define MAP_l2capBuildDisconnectRsp                                  l2capBuildDisconnectRsp
#define MAP_l2capBuildFlowCtrlCredit                                 l2capBuildFlowCtrlCredit
#define MAP_l2capBuildInfoReq                                        l2capBuildInfoReq
#define MAP_l2capBuildInfoRsp                                        l2capBuildInfoRsp
#define MAP_l2capBuildParamUpdateReq                                 l2capBuildParamUpdateReq
#define MAP_l2capBuildSignalHdr                                      l2capBuildSignalHdr
#define MAP_l2capDisconnectAllChannels                               l2capDisconnectAllChannels
#define MAP_l2capDisconnectChannel                                   l2capDisconnectChannel
#define MAP_l2capEncapSendData                                       l2capEncapSendData
#define MAP_l2capFindLocalCID                                        l2capFindLocalCID
#define MAP_l2capFindLocalId                                         l2capFindLocalId
#define MAP_l2capFindNextSegment                                     l2capFindNextSegment
#define MAP_l2capFindPsm                                             l2capFindPsm
#define MAP_l2capFindRemoteCID                                       l2capFindRemoteCID
#define MAP_l2capFindRemoteId                                        l2capFindRemoteId
#define MAP_l2capFlowCtrlCredit                                      l2capFlowCtrlCredit
#define MAP_l2capFreeChannel                                         l2capFreeChannel
#define MAP_l2capFreePendingPkt                                      l2capFreePendingPkt
#define MAP_l2capFreeTxSDU                                           l2capFreeTxSDU
#define MAP_l2capGetCoChannelInfo                                    l2capGetCoChannelInfo
#define MAP_l2capHandleTimerCB                                       l2capHandleTimerCB
#define MAP_l2capHandleRxError                                       l2capHandleRxError
#define MAP_l2capNotifyChannelEstEvt                                 l2capNotifyChannelEstEvt
#define MAP_l2capNotifyChannelTermEvt                                l2capNotifyChannelTermEvt
#define MAP_l2capNotifyCreditEvt                                     l2capNotifyCreditEvt
#define MAP_l2capNotifyData                                          l2capNotifyData
#define MAP_l2capNotifyEvent                                         l2capNotifyEvent
#define MAP_l2capNotifySendSduDoneEvt                                l2capNotifySendSduDoneEvt
#define MAP_l2capNotifySignal                                        l2capNotifySignal
#define MAP_l2capNumActiveChannnels                                  l2capNumActiveChannnels
#define MAP_l2capParseCmdReject                                      l2capParseCmdReject
#define MAP_l2capParseConnectRsp                                     l2capParseConnectRsp
#define MAP_l2capParseDisconnectReq                                  l2capParseDisconnectReq
#define MAP_l2capParseDisconnectRsp                                  l2capParseDisconnectRsp
#define MAP_l2capParsePacket                                         l2capParsePacket
#define MAP_l2capParseParamUpdateRsp                                 l2capParseParamUpdateRsp
#define MAP_l2capParseSignalHdr                                      l2capParseSignalHdr
#define MAP_l2capProcessConnectReq                                   l2capProcessConnectReq
#define MAP_l2capProcessOSALMsg                                      l2capProcessOSALMsg
#define MAP_l2capProcessReq                                          l2capProcessReq
#define MAP_l2capProcessRsp                                          l2capProcessRsp
#define MAP_l2capProcessRxData                                       l2capProcessRxData
#define MAP_l2capProcessSignal                                       l2capProcessSignal
#define MAP_l2capReassembleSegment                                   l2capReassembleSegment
#define MAP_l2capSendCmd                                             l2capSendCmd
#define MAP_l2capSendConnectRsp                                      l2capSendConnectRsp
#define MAP_l2capSendFCPkt                                           l2capSendFCPkt
#define MAP_l2capSendNextSegment                                     l2capSendNextSegment
#define MAP_l2capSendPkt                                             l2capSendPkt
#define MAP_l2capSendReq                                             l2capSendReq
#define MAP_l2capSendSegment                                         l2capSendSegment
#define MAP_l2capStartTimer                                          l2capStartTimer
#define MAP_l2capStopTimer                                           l2capStopTimer
#define MAP_l2capStoreFCPkt                                          l2capStoreFCPkt

// ATT
#define MAP_ATT_BuildErrorRsp                                        ATT_BuildErrorRsp
#define MAP_ATT_BuildExchangeMTURsp                                  ATT_BuildExchangeMTURsp
#define MAP_ATT_BuildFindByTypeValueRsp                              ATT_BuildFindByTypeValueRsp
#define MAP_ATT_BuildFindInfoRsp                                     ATT_BuildFindInfoRsp
#define MAP_ATT_BuildHandleValueInd                                  ATT_BuildHandleValueInd
#define MAP_ATT_BuildPrepareWriteRsp                                 ATT_BuildPrepareWriteRsp
#define MAP_ATT_BuildReadBlobRsp                                     ATT_BuildReadBlobRsp
#define MAP_ATT_BuildReadByGrpTypeRsp                                ATT_BuildReadByGrpTypeRsp
#define MAP_ATT_BuildReadByTypeRsp                                   ATT_BuildReadByTypeRsp
#define MAP_ATT_BuildReadMultiRsp                                    ATT_BuildReadMultiRsp
#define MAP_ATT_BuildReadRsp                                         ATT_BuildReadRsp
#define MAP_ATT_CompareUUID                                          ATT_CompareUUID
#define MAP_ATT_ConvertUUIDto128                                     ATT_ConvertUUIDto128
#define MAP_ATT_ConvertUUIDto16                                      ATT_ConvertUUIDto16
#define MAP_ATT_ErrorRsp                                             ATT_ErrorRsp
#define MAP_ATT_ExchangeMTUReq                                       ATT_ExchangeMTUReq
#define MAP_ATT_ExchangeMTURsp                                       ATT_ExchangeMTURsp
#define MAP_ATT_ExecuteWriteReq                                      ATT_ExecuteWriteReq
#define MAP_ATT_ExecuteWriteRsp                                      ATT_ExecuteWriteRsp
#define MAP_ATT_FindByTypeValueReq                                   ATT_FindByTypeValueReq
#define MAP_ATT_FindByTypeValueRsp                                   ATT_FindByTypeValueRsp
#define MAP_ATT_FindInfoReq                                          ATT_FindInfoReq
#define MAP_ATT_FindInfoRsp                                          ATT_FindInfoRsp
#define MAP_ATT_GetMTU                                               ATT_GetMTU
#define MAP_ATT_HandleValueInd                                       ATT_HandleValueInd
#define MAP_ATT_HandleValueNoti                                      ATT_HandleValueNoti
#define MAP_ATT_ParseErrorRsp                                        ATT_ParseErrorRsp
#define MAP_ATT_ParseExchangeMTUReq                                  ATT_ParseExchangeMTUReq
#define MAP_ATT_ParseExecuteWriteReq                                 ATT_ParseExecuteWriteReq
#define MAP_ATT_ParseFindInfoReq                                     ATT_ParseFindInfoReq
#define MAP_ATT_ParseFindByTypeValueReq                              ATT_ParseFindByTypeValueReq
#define MAP_ATT_ParseHandleValueInd                                  ATT_ParseHandleValueInd
#define MAP_ATT_ParsePacket                                          ATT_ParsePacket
#define MAP_ATT_ParsePrepareWriteReq                                 ATT_ParsePrepareWriteReq
#define MAP_ATT_ParseReadBlobReq                                     ATT_ParseReadBlobReq
#define MAP_ATT_ParseReadByTypeReq                                   ATT_ParseReadByTypeReq
#define MAP_ATT_ParseReadMultiReq                                    ATT_ParseReadMultiReq
#define MAP_ATT_ParseReadReq                                         ATT_ParseReadReq
#define MAP_ATT_ParseWriteReq                                        ATT_ParseWriteReq
#define MAP_ATT_PrepareWriteReq                                      ATT_PrepareWriteReq
#define MAP_ATT_PrepareWriteRsp                                      ATT_PrepareWriteRsp
#define MAP_ATT_ReadBlobReq                                          ATT_ReadBlobReq
#define MAP_ATT_ReadBlobRsp                                          ATT_ReadBlobRsp
#define MAP_ATT_ReadByGrpTypeReq                                     ATT_ReadByGrpTypeReq
#define MAP_ATT_ReadByGrpTypeRsp                                     ATT_ReadByGrpTypeRsp
#define MAP_ATT_ReadByTypeReq                                        ATT_ReadByTypeReq
#define MAP_ATT_ReadByTypeRsp                                        ATT_ReadByTypeRsp
#define MAP_ATT_ReadMultiReq                                         ATT_ReadMultiReq
#define MAP_ATT_ReadMultiRsp                                         ATT_ReadMultiRsp
#define MAP_ATT_ReadReq                                              ATT_ReadReq
#define MAP_ATT_ReadRsp                                              ATT_ReadRsp
#define MAP_ATT_RegisterClient                                       ATT_RegisterClient
#define MAP_ATT_RegisterServer                                       ATT_RegisterServer
#define MAP_ATT_UpdateMTU                                            ATT_UpdateMTU
#define MAP_ATT_WriteReq                                             ATT_WriteReq
#define MAP_ATT_WriteRsp                                             ATT_WriteRsp
#define MAP_attSendMsg                                               attSendMsg_sPatch
#define MAP_attSendRspMsg                                            attSendRspMsg

// GATT
#define MAP_GATT_AppCompletedMsg                                     GATT_AppCompletedMsg
#define MAP_GATT_bm_alloc                                            GATT_bm_alloc
#define MAP_GATT_bm_free                                             GATT_bm_free
#define MAP_GATT_FindHandle                                          GATT_FindHandle
#define MAP_GATT_FindHandleUUID                                      GATT_FindHandleUUID
#define MAP_GATT_FindNextAttr                                        GATT_FindNextAttr
#define MAP_GATT_Indication                                          GATT_Indication
#define MAP_GATT_InitServer                                          GATT_InitServer
#define MAP_GATT_NotifyEvent                                         GATT_NotifyEvent
#define MAP_GATT_ServiceEncKeySize                                   GATT_ServiceEncKeySize
#define MAP_GATT_ServiceNumAttrs                                     GATT_ServiceNumAttrs
#define MAP_GATT_UpdateMTU                                           GATT_UpdateMTU
#define MAP_GATT_userDefined16bitUUID                                GATT_userDefined16bitUUID
#define MAP_GATT_VerifyReadPermissions                               GATT_VerifyReadPermissions
#define MAP_GATT_VerifyWritePermissions                              GATT_VerifyWritePermissions
#define MAP_gattClientHandleConnStatusCB                             gattClientHandleConnStatusCB
#define MAP_gattClientHandleTimerCB                                  gattClientHandleTimerCB
#define MAP_gattClientNotifyTxCB                                     gattClientNotifyTxCB
#define MAP_gattClientProcessMsgCB                                   gattClientProcessMsgCB
#define MAP_gattClientStartTimer                                     gattClientStartTimer
#define MAP_gattFindByTypeValue                                      gattFindByTypeValue
#define MAP_gattFindClientInfo                                       gattFindClientInfo
#define MAP_gattFindInfo                                             gattFindInfo
#define MAP_gattFindServerInfo                                       gattFindServerInfo
#define MAP_gattFindService                                          gattFindService
#define MAP_gattGetClientStatus                                      gattGetClientStatus
#define MAP_gattGetPayload                                           gattGetPayload
#define MAP_gattGetServerStatus                                      gattGetServerStatus
#define MAP_gattNotifyEvent                                          gattNotifyEvent
#define MAP_gattParseReq                                             gattParseReq
#define MAP_gattPrepareWriteReq                                      gattPrepareWriteReq
#define MAP_gattProcessExchangeMTUReq                                gattProcessExchangeMTUReq
#define MAP_gattProcessExecuteWriteReq                               gattProcessExecuteWriteReq
#define MAP_gattProcessFindByTypeValue                               gattProcessFindByTypeValue
#define MAP_gattProcessFindByTypeValueReq                            gattProcessFindByTypeValueReq
#define MAP_gattProcessFindInfo                                      gattProcessFindInfo
#define MAP_gattProcessFindInfoReq                                   gattProcessFindInfoReq
#define MAP_gattProcessMultiReqs                                     gattProcessMultiReqs
#define MAP_gattProcessReadByGrpType                                 gattProcessReadByGrpType
#define MAP_gattProcessReadByGrpTypeReq                              gattProcessReadByGrpTypeReq
#define MAP_gattProcessReadByType                                    gattProcessReadByType
#define MAP_gattProcessReadByTypeReq                                 gattProcessReadByTypeReq
#define MAP_gattProcessReadLong                                      gattProcessReadLong
#define MAP_gattProcessReadReq                                       gattProcessReadReq
#define MAP_gattProcessReliableWrites                                gattProcessReliableWrites
#define MAP_gattProcessReq                                           gattProcessReq
#define MAP_gattProcessRxData                                        gattProcessRxData
#define MAP_gattProcessOSALMsg                                       gattProcessOSALMsg
#define MAP_gattProcessWriteLong                                     gattProcessWriteLong
#define MAP_gattProcessWriteReq                                      gattProcessWriteReq
#define MAP_gattReadByGrpType                                        gattReadByGrpType
#define MAP_gattRead                                                 gattRead
#define MAP_gattReadByType                                           gattReadByType
#define MAP_gattReadLong                                             gattReadLong
#define MAP_gattProcessReadMultiReq                                  gattProcessReadMultiReq
#define MAP_gattRegisterClient                                       gattRegisterClient
#define MAP_gattRegisterServer                                       gattRegisterServer
#define MAP_gattResetClientInfo                                      gattResetClientInfo
#define MAP_gattResetServerInfo                                      gattResetServerInfo
#define MAP_gattSendFlowCtrlEvt                                      gattSendFlowCtrlEvt
#define MAP_gattServerHandleConnStatusCB                             gattServerHandleConnStatusCB
#define MAP_gattServerHandleTimerCB                                  gattServerHandleTimerCB
#define MAP_gattServerNotifyTxCB                                     gattServerNotifyTxCB
#define MAP_gattServerProcessMsgCB                                   gattServerProcessMsgCB
#define MAP_gattServerStartTimer                                     gattServerStartTimer
#define MAP_gattServiceLastHandle                                    gattServiceLastHandle
#define MAP_gattStartTimer                                           gattStartTimer
#define MAP_gattStopTimer                                            gattStopTimer
#define MAP_gattStoreClientInfo                                      gattStoreClientInfo
#define MAP_gattStoreServerInfo                                      gattStoreServerInfo
#define MAP_gattWrite                                                gattWrite
#define MAP_gattWriteLong                                            gattWriteLong

// GATT Server
#define MAP_GATTServApp_ReadAttr                                     GATTServApp_ReadAttr
#define MAP_GATTServApp_ReadCharCfg                                  GATTServApp_ReadCharCfg
#define MAP_GATTServApp_SendCCCUpdatedEvent                          GATTServApp_SendCCCUpdatedEvent
#define MAP_GATTServApp_WriteAttr                                    GATTServApp_WriteAttr
#define MAP_gattServApp_buildReadByTypeRsp                           gattServApp_buildReadByTypeRsp
#define MAP_gattServApp_ClearPrepareWriteQ                           gattServApp_ClearPrepareWriteQ
#define MAP_gattServApp_EnqueuePrepareWriteReq                       gattServApp_EnqueuePrepareWriteReq
#define MAP_gattServApp_FindAuthorizeAttrCB                          gattServApp_FindAuthorizeAttrCB
#define MAP_gattServApp_FindPrepareWriteQ                            gattServApp_FindPrepareWriteQ
#define MAP_gattServApp_FindReadAttrCB                               gattServApp_FindReadAttrCB
#define MAP_gattServApp_FindServiceCBs                               gattServApp_FindServiceCBs
#define MAP_gattServApp_FindWriteAttrCB                              gattServApp_FindWriteAttrCB
#define MAP_gattServApp_IsWriteLong                                  gattServApp_IsWriteLong
#define MAP_gattServApp_PrepareWriteQInUse                           gattServApp_PrepareWriteQInUse
#define MAP_gattServApp_ProcessExchangeMTUReq                        gattServApp_ProcessExchangeMTUReq
#define MAP_gattServApp_ProcessExecuteWriteReq                       gattServApp_ProcessExecuteWriteReq
#define MAP_gattServApp_ProcessFindByTypeValueReq                    gattServApp_ProcessFindByTypeValueReq
#define MAP_gattServApp_ProcessPrepareWriteReq                       gattServApp_ProcessPrepareWriteReq
#define MAP_gattServApp_ProcessReadByGrpTypeReq                      gattServApp_ProcessReadByGrpTypeReq
#define MAP_gattServApp_ProcessReadByTypeReq                         gattServApp_ProcessReadByTypeReq
#define MAP_gattServApp_ProcessReadBlobReq                           gattServApp_ProcessReadBlobReq
#define MAP_gattServApp_ProcessReadMultiReq                          gattServApp_ProcessReadMultiReq
#define MAP_gattServApp_ProcessReadReq                               gattServApp_ProcessReadReq
#define MAP_gattServApp_ProcessWriteReq                              gattServApp_ProcessWriteReq
#define MAP_gattServApp_ProcessReliableWrites                        gattServApp_ProcessReliableWrites
#define MAP_gattServApp_ProcessWriteLong                             gattServApp_ProcessWriteLong
#define MAP_gattServApp_SetNumPrepareWrites                          gattServApp_SetNumPrepareWrites
#define MAP_gattServApp_EnqueueReTx                                  gattServApp_EnqueueReTx
#define MAP_gattServApp_DequeueReTx                                  gattServApp_DequeueReTx

// GAP API
#define MAP_GAP_Authenticate                                         GAP_Authenticate
#define MAP_GAP_Bond                                                 GAP_Bond
#define MAP_gap_CentConnRegister                                     gap_CentConnRegister
#define MAP_GAP_CentDevMgrInit                                       GAP_CentDevMgrInit
#ifdef GAP_BOND_MGR
#define MAP_GAP_DeviceInit                                           GAP_DeviceInit
#else
#define MAP_GAP_DeviceInit                                           GAP_DeviceInit_noGAPBondMgr
#endif
#define MAP_GAP_GetIRK                                               GAP_GetIRK
#define MAP_GAP_GetSRK                                               GAP_GetSRK
#define MAP_GAP_GetParamValue                                        GAP_GetParamValue
#define MAP_GAP_Init                                                 GAP_Init
#define MAP_GAP_NumActiveConnections                                 GAP_NumActiveConnections
#define MAP_gap_ParamsInit                                           gap_ParamsInit
#define MAP_GAP_PasscodeUpdate                                       GAP_PasscodeUpdate
#define MAP_GAP_PasskeyUpdate                                        GAP_PasskeyUpdate
#define MAP_gap_PeriConnRegister                                     gap_PeriConnRegister
#define MAP_gap_PrivacyInit                                          gap_PrivacyInit
#define MAP_GAP_ProcessEvent                                         GAP_ProcessEvent
#define MAP_GAP_RegisterBondMgrCBs                                   GAP_RegisterBondMgrCBs
#define MAP_GAP_RegisterForMsgs                                      GAP_RegisterForMsgs
#define MAP_gap_SecParamsInit                                        gap_SecParamsInit
#define MAP_GAP_SendSlaveSecurityRequest                             GAP_SendSlaveSecurityRequest
#define MAP_GAP_SetParamValue                                        GAP_SetParamValue
#define MAP_GAP_Signable                                             GAP_Signable
#define MAP_GAP_TerminateAuth                                        GAP_TerminateAuth
#define MAP_GAP_TerminateLinkReq                                     GAP_TerminateLinkReq
#define MAP_GAP_UpdateLinkParamReq                                   GAP_UpdateLinkParamReq
#define MAP_GAP_UpdateLinkParamReqReply                              GAP_UpdateLinkParamReqReply
#ifdef GAP_BOND_MGR
#define MAP_GAP_UpdateResolvingList                                  GAP_UpdateResolvingList
#else
#define MAP_GAP_UpdateResolvingList                                  GAP_UpdateResolvingList_noGAPBondMgr
#endif
#define MAP_GapConfig_SetParameter                                   GapConfig_SetParameter
#define MAP_GAP_GetDevAddress                                        GAP_GetDevAddress

// GAP Task
#define MAP_gapProcessBLEEvents                                      gapProcessBLEEvents
#define MAP_gapProcessCommandStatusEvt                               gapProcessCommandStatusEvt
#define MAP_gapProcessConnEvt                                        gapProcessConnEvt
#define MAP_gapProcessHCICmdCompleteEvt                              gapProcessHCICmdCompleteEvt
#define MAP_gapProcessOSALMsg                                        gapProcessOSALMsg

// GAP Link Mgr
#define MAP_disconnectNext                                           disconnectNext
#define MAP_gapFreeAuthLink                                          gapFreeAuthLink
#define MAP_gapPairingCompleteCB                                     gapPairingCompleteCB
#define MAP_gapPasskeyNeededCB                                       gapPasskeyNeededCB
#define MAP_gapProcessConnectionCompleteEvt                          gapProcessConnectionCompleteEvt
#define MAP_gapProcessDisconnectCompleteEvt                          gapProcessDisconnectCompleteEvt
#define MAP_gapProcessRemoteConnParamReqEvt                          gapProcessRemoteConnParamReqEvt
#define MAP_gapRegisterCentralConn                                   gapRegisterCentralConn
#define MAP_gapRegisterPeripheralConn                                gapRegisterPeripheralConn
#define MAP_gapSendBondCompleteEvent                                 gapSendBondCompleteEvent
#define MAP_gapSendLinkUpdateEvent                                   gapSendLinkUpdateEvent
#define MAP_gapSendLinkRejectEvent                                   gapSendLinkRejectEvent
#define MAP_gapSendPairingReqEvent                                   gapSendPairingReqEvent
#define MAP_gapSendSignUpdateEvent                                   gapSendSignUpdateEvent
#define MAP_gapSendSlaveSecurityReqEvent                             gapSendSlaveSecurityReqEvent_hook
#define MAP_gapUpdateConnSignCounter                                 gapUpdateConnSignCounter
#define MAP_sendAuthEvent                                            sendAuthEvent
#define MAP_sendEstLinkEvent                                         sendEstLinkEvent
#define MAP_sendTerminateEvent                                       sendTerminateEvent
#define MAP_gapConnEvtNoticeCB                                       gapConnEvtNoticeCB

// GAP Config Manager
#define MAP_gapGetDevAddressMode                                     gapGetDevAddressMode
#define MAP_gapGetSignCounter                                        gapGetSignCounter
#define MAP_gapGetState                                              gapGetState
#define MAP_gapGetSRK                                                gapGetSRK
#define MAP_gapHost2CtrlOwnAddrType                                  gapHost2CtrlOwnAddrType
#define MAP_gapIncSignCounter                                        gapIncSignCounter
#define MAP_gapReadBufSizeCmdStatus                                  gapReadBufSizeCmdStatus
#define MAP_gapSendDeviceInitDoneEvent                               gapSendDeviceInitDoneEvent

// GAP Dev Mgr
#define MAP_gapClrState                                              gapClrState
#define MAP_gapFindADType                                            gapFindADType
#define MAP_gapIsAdvertising                                         gapIsAdvertising
#define MAP_gapIsScanning                                            gapIsScanning
#define MAP_gapIsInitiating                                          gapIsInitiating
#define MAP_gapSetState                                              gapSetState
#define MAP_gapValidADType                                           gapValidADType

// GAP Peripheral Link Mgr
#define MAP_gapL2capConnParamUpdateReq                               gapL2capConnParamUpdateReq
#define MAP_gapPeriProcessConnEvt                                    gapPeriProcessConnEvt
#define MAP_gapPeriProcessConnUpdateCmdStatus                        gapPeriProcessConnUpdateCmdStatus
#define MAP_gapPeriProcessConnUpdateCompleteEvt                      gapPeriProcessConnUpdateCompleteEvt
#define MAP_gapPeriProcessSignalEvt                                  gapPeriProcessSignalEvt

// GAP Advertiser Module
#define MAP_gapAdv_init                                              gapAdv_init
#define MAP_GapAdv_create                                            GapAdv_create
#define MAP_GapAdv_destroy                                           GapAdv_destroy
#define MAP_GapAdv_enable                                            GapAdv_enable
#define MAP_GapAdv_disable                                           GapAdv_disable
#define MAP_GapAdv_setParam                                          GapAdv_setParam
#define MAP_GapAdv_getParam                                          GapAdv_getParam
#define MAP_GapAdv_getBuffer                                         GapAdv_getBuffer
#define MAP_GapAdv_loadByHandle                                      GapAdv_loadByHandle
#define MAP_GapAdv_loadByBuffer                                      GapAdv_loadByBuffer
#define MAP_GapAdv_prepareLoadByHandle                               GapAdv_prepareLoadByHandle
#define MAP_GapAdv_prepareLoadByBuffer                               GapAdv_prepareLoadByBuffer
#define MAP_GapAdv_abortLoad                                         GapAdv_abortLoad
#define MAP_GapAdv_setEventMask                                      GapAdv_setEventMask
#define MAP_gapAdv_searchForBufferUse                                gapAdv_searchForBufferUse
#define MAP_gapAdv_processRemoveSetEvt                               gapAdv_processRemoveSetEvt
#define MAP_GapAdv_setVirtualAdvAddr                                 GapAdv_setVirtualAdvAddr

// GAP Scanner Module
#define MAP_gapScan_init                                             gapScan_init
#define MAP_GapScan_registerCb                                       GapScan_registerCb
#define MAP_GapScan_setPhyParams                                     GapScan_setPhyParams
#define MAP_GapScan_getPhyParams                                     GapScan_getPhyParams
#define MAP_GapScan_setParam                                         GapScan_setParam
#define MAP_GapScan_getParam                                         GapScan_getParam
#define MAP_GapScan_setEventMask                                     GapScan_setEventMask
#define MAP_GapScan_enable                                           GapScan_enable
#define MAP_GapScan_getAdvReport                                     GapScan_getAdvReport
#define MAP_GapScan_discardAdvReportList                             GapScan_discardAdvReportList
#define MAP_GapScan_disable                                          GapScan_disable
#define MAP_gapScan_filterDiscMode                                   gapScan_filterDiscMode
#define MAP_gapScan_discardAdvRptSession                             gapScan_discardAdvRptSession
#define MAP_gapScan_defragAdvRpt                                     gapScan_defragAdvRpt
#define MAP_gapScan_saveRptAndNotify                                 gapScan_saveRptAndNotify
#define MAP_gapScan_sendSessionEndEvt                                gapScan_sendSessionEndEvt
#define MAP_gapScan_processAdvRptCb                                  gapScan_processAdvRptCb
#define MAP_gapScan_processStartEndCb                                gapScan_processStartEndCb
#define MAP_gapScan_processErrorCb                                   gapScan_processErrorCb
#define MAP_gapScan_processSessionEndEvt                             gapScan_processSessionEndEvt

// GAP Initiator Module
#define MAP_GapInit_setPhyParam                                      GapInit_setPhyParam
#define MAP_GapInit_getPhyParam                                      GapInit_getPhyParam
#define MAP_gapInit_connect_internal                                 gapInit_connect_internal
#define MAP_GapInit_connect                                          GapInit_connect
#define MAP_GapInit_connectWl                                        GapInit_connectWl
#define MAP_GapInit_cancelConnect                                    GapInit_cancelConnect
#define MAP_gapInit_sendConnCancelledEvt                             gapInit_sendConnCancelledEvt
#define MAP_gapInit_initiatingEnd                                    gapInit_initiatingEnd

// GAP Central Link Mgr
#define MAP_gapCentProcessConnEvt                                    gapCentProcessConnEvt
#define MAP_gapCentProcessConnUpdateCompleteEvt                      gapCentProcessConnUpdateCompleteEvt
#define MAP_gapCentProcessSignalEvt                                  gapCentProcessSignalEvt
#define MAP_gapProcessCreateLLConnCmdStatus                          gapProcessCreateLLConnCmdStatus
#define MAP_gapTerminateConnComplete                                 gapTerminateConnComplete

// SM API
#define MAP_SM_RegisterTask                                          SM_RegisterTask
#define MAP_SM_GetEccKeys                                            SM_GetEccKeys
#define MAP_SM_GetDHKey                                              SM_GetDHKey
#define MAP_SM_GetScConfirmOob                                       SM_GetScConfirmOob
#define MAP_SM_InitiatorInit                                         SM_InitiatorInit
#define MAP_SM_StartPairing                                          SM_StartPairing
#define MAP_SM_StartEncryption                                       SM_StartEncryption
#define MAP_SM_ResponderInit                                         SM_ResponderInit
#define MAP_SM_NewRandKey                                            SM_NewRandKey
#define MAP_SM_Encrypt                                               SM_Encrypt
#define MAP_SM_GenerateAuthenSig                                     SM_GenerateAuthenSig
#define MAP_SM_VerifyAuthenSig                                       SM_VerifyAuthenSig
#define MAP_SM_PasskeyUpdate                                         SM_PasskeyUpdate
#define MAP_SM_SetECCRegenerationCount                               SM_SetECCRegenerationCount
#define MAP_SM_Init                                                  SM_Init
#define MAP_SM_ProcessEvent                                          SM_ProcessEvent
#define MAP_SM_p256KeyCB                                             SM_p256KeyCB_hook
#define MAP_SM_dhKeyCB                                               SM_dhKeyCB_hook

// SM Task
#define MAP_smProcessHCIBLEEventCode                                 smProcessHCIBLEEventCode
#define MAP_smProcessHCIBLEMetaEventCode                             smProcessHCIBLEMetaEventCode
#define MAP_smProcessOSALMsg                                         smProcessOSALMsg

// SM Manager
#define MAP_generate_subkey                                          generate_subkey
#define MAP_leftshift_onebit                                         leftshift_onebit
#define MAP_padding                                                  padding
#define MAP_smAuthReqToUint8                                         smAuthReqToUint8
#define MAP_smEncrypt                                                smEncrypt
#define MAP_smEncryptLocal                                           smEncryptLocal
#define MAP_smGenerateRandBuf                                        smGenerateRandBuf
#define MAP_smStartRspTimer                                          smStartRspTimer
#define MAP_smStopRspTimer                                           smStopRspTimer
#define MAP_smUint8ToAuthReq                                         smUint8ToAuthReq
#define MAP_sm_c1new                                                 sm_c1new
#define MAP_sm_CMAC                                                  sm_CMAC
#define MAP_sm_d1                                                    sm_d1
#define MAP_sm_dm                                                    sm_dm
#define MAP_sm_f4                                                    sm_f4
#define MAP_sm_f5                                                    sm_f5
#define MAP_sm_f6                                                    sm_f6
#define MAP_sm_g2                                                    sm_g2
#define MAP_sm_s1                                                    sm_s1
#define MAP_sm_xor                                                   sm_xor
#define MAP_xor_128                                                  xor_128

// SM Pairing Manager
#define MAP_smDetermineIOCaps                                        smDetermineIOCaps
#define MAP_smDetermineKeySize                                       smDetermineKeySize
#define MAP_smEndPairing                                             smEndPairing
#define MAP_smFreePairingParams                                      smFreePairingParams
#define MAP_smF5Wrapper                                              smF5Wrapper
#define MAP_smGenerateAddrInput                                      smGenerateAddrInput_sPatch
#define MAP_smGenerateConfirm                                        smGenerateConfirm
#define MAP_smGenerateDHKeyCheck                                     smGenerateDHKeyCheck
#define MAP_smGeneratePairingReqRsp                                  smGeneratePairingReqRsp_hook
#define MAP_smGenerateRandMsg                                        smGenerateRandMsg
#define MAP_smGetECCKeys                                             smGetECCKeys
#define MAP_smIncrementEccKeyRecycleCount                            smIncrementEccKeyRecycleCount
#define MAP_smLinkCheck                                              smLinkCheck
#define MAP_smNextPairingState                                       smNextPairingState
#define MAP_smOobSCAuthentication                                    smOobSCAuthentication
#define MAP_smPairingSendEncInfo                                     smPairingSendEncInfo
#define MAP_smPairingSendIdentityAddrInfo                            smPairingSendIdentityAddrInfo
#define MAP_smPairingSendIdentityInfo                                smPairingSendIdentityInfo
#define MAP_smPairingSendMasterID                                    smPairingSendMasterID
#define MAP_smPairingSendSigningInfo                                 smPairingSendSigningInfo
#define MAP_smProcessDataMsg                                         smProcessDataMsg
#define MAP_smProcessEncryptChange                                   smProcessEncryptChange
#define MAP_smProcessPairingReq                                      smProcessPairingReq
#define MAP_smRegisterInitiator                                      smRegisterInitiator
#define MAP_smRegisterResponder                                      smRegisterResponder
#define MAP_smSavePairInfo                                           smSavePairInfo
#define MAP_smSaveRemotePublicKeys                                   smSaveRemotePublicKeys
#define MAP_smSendDHKeyCheck                                         smSendDHKeyCheck
#define MAP_smSendFailAndEnd                                         smSendFailAndEnd
#define MAP_smSendFailureEvt                                         smSendFailureEvt
#define MAP_smSetPairingReqRsp                                       smSetPairingReqRsp
#define MAP_smSendPublicKeys                                         smSendPublicKeys
#define MAP_smStartEncryption                                        smStartEncryption
#define MAP_smTimedOut                                               smTimedOut
#define MAP_sm_allocateSCParameters                                  sm_allocateSCParameters
#define MAP_sm_computeDHKey                                          sm_computeDHKey
#define MAP_sm_c1                                                    sm_c1
#define MAP_smpProcessIncoming                                       smpProcessIncoming

// SM Initiatior Pairing Manager
#define MAP_setupInitiatorKeys                                       setupInitiatorKeys
#define MAP_smEncLTK                                                 smEncLTK
#define MAP_smInitiatorAuthStageTwo                                  smInitiatorAuthStageTwo
#define MAP_smInitiatorSendNextKeyInfo                               smInitiatorSendNextKeyInfo
#define MAP_smpInitiatorProcessEncryptionInformation                 smpInitiatorProcessEncryptionInformation
#define MAP_smpInitiatorProcessIdentityAddrInfo                      smpInitiatorProcessIdentityAddrInfo
#define MAP_smpInitiatorProcessIdentityInfo                          smpInitiatorProcessIdentityInfo
#define MAP_smpInitiatorProcessIncoming                              smpInitiatorProcessIncoming
#define MAP_smpInitiatorProcessMasterID                              smpInitiatorProcessMasterID
#define MAP_smpInitiatorProcessPairingConfirm                        smpInitiatorProcessPairingConfirm
#define MAP_smpInitiatorProcessPairingDHKeyCheck                     smpInitiatorProcessPairingDHKeyCheck
#define MAP_smpInitiatorProcessPairingPubKey                         smpInitiatorProcessPairingPubKey_sPatch
#define MAP_smpInitiatorProcessPairingRandom                         smpInitiatorProcessPairingRandom_hook
#define MAP_smpInitiatorProcessPairingRsp                            smpInitiatorProcessPairingRsp_sPatch
#define MAP_smpInitiatorProcessSigningInfo                           smpInitiatorProcessSigningInfo

// SM Response Pairing Manager
#define MAP_smFinishPublicKeyExchange                                smFinishPublicKeyExchange
#define MAP_smResponderAuthStageTwo                                  smResponderAuthStageTwo
#define MAP_smResponderProcessLTKReq                                 smResponderProcessLTKReq_hook
#define MAP_smResponderSendNextKeyInfo                               smResponderSendNextKeyInfo
#define MAP_smpResponderProcessEncryptionInformation                 smpResponderProcessEncryptionInformation
#define MAP_smpResponderProcessIdentityAddrInfo                      smpResponderProcessIdentityAddrInfo
#define MAP_smpResponderProcessIdentityInfo                          smpResponderProcessIdentityInfo
#define MAP_smpResponderProcessIncoming                              smpResponderProcessIncoming_hook
#define MAP_smpResponderProcessMasterID                              smpResponderProcessMasterID
#define MAP_smpResponderProcessPairingConfirm                        smpResponderProcessPairingConfirm
#define MAP_smpResponderProcessPairingDHKeyCheck                     smpResponderProcessPairingDHKeyCheck
#define MAP_smpResponderProcessPairingPublicKey                      smpResponderProcessPairingPublicKey_hook
#define MAP_smpResponderProcessPairingRandom                         smpResponderProcessPairingRandom
#define MAP_smpResponderProcessPairingReq                            smpResponderProcessPairingReq_sPatch
#define MAP_smpResponderProcessSigningInfo                           smpResponderProcessSigningInfo
#define MAP_smpResponderSendPairRspEvent                             smpResponderSendPairRspEvent

// SM Protocol
#define MAP_smpBuildEncInfo                                          smpBuildEncInfo
#define MAP_smpBuildIdentityAddrInfo                                 smpBuildIdentityAddrInfo
#define MAP_smpBuildIdentityInfo                                     smpBuildIdentityInfo
#define MAP_smpBuildKeypressNoti                                     smpBuildKeypressNoti
#define MAP_smpBuildMasterID                                         smpBuildMasterID
#define MAP_smpBuildPairingConfirm                                   smpBuildPairingConfirm
#define MAP_smpBuildPairingDHKeyCheck                                smpBuildPairingDHKeyCheck
#define MAP_smpBuildPairingFailed                                    smpBuildPairingFailed
#define MAP_smpBuildPairingPublicKey                                 smpBuildPairingPublicKey
#define MAP_smpBuildPairingRandom                                    smpBuildPairingRandom
#define MAP_smpBuildPairingReq                                       smpBuildPairingReq
#define MAP_smpBuildPairingReqRsp                                    smpBuildPairingReqRsp
#define MAP_smpBuildPairingRsp                                       smpBuildPairingRsp
#define MAP_smpBuildSecurityReq                                      smpBuildSecurityReq
#define MAP_smpBuildSigningInfo                                      smpBuildSigningInfo
#define MAP_smpParseEncInfo                                          smpParseEncInfo
#define MAP_smpParseIdentityAddrInfo                                 smpParseIdentityAddrInfo
#define MAP_smpParseIdentityInfo                                     smpParseIdentityInfo
#define MAP_smpParseKeypressNoti                                     smpParseKeypressNoti
#define MAP_smpParseMasterID                                         smpParseMasterID
#define MAP_smpParsePairingConfirm                                   smpParsePairingConfirm
#define MAP_smpParsePairingDHKeyCheck                                smpParsePairingDHKeyCheck
#define MAP_smpParsePairingFailed                                    smpParsePairingFailed
#define MAP_smpParsePairingPublicKey                                 smpParsePairingPublicKey
#define MAP_smpParsePairingRandom                                    smpParsePairingRandom
#define MAP_smpParsePairingReq                                       smpParsePairingReq
#define MAP_smpParseSecurityReq                                      smpParseSecurityReq
#define MAP_smpParseSigningInfo                                      smpParseSigningInfo
#define MAP_smSendSMMsg                                              smSendSMMsg

// OSAL
#define MAP_osal_revmemcpy                                           osal_revmemcpy
#define MAP_osal_rand                                                osal_rand
#define MAP_osal_start_reload_timer                                  osal_start_reload_timer
#define MAP_osal_memdup                                              osal_memdup
#define MAP_osal_isbufset                                            osal_isbufset
#define MAP_osal_pwrmgr_task_state                                   osal_pwrmgr_task_state
#define MAP_osal_msg_allocate                                        osal_msg_allocate
#define MAP_osal_msg_deallocate                                      osal_msg_deallocate
#define MAP_osal_msg_send                                            osal_msg_send
#define MAP_osal_msg_receive                                         osal_msg_receive
#define MAP_osal_set_event                                           osal_set_event
#define MAP_osal_bm_alloc                                            osal_bm_alloc
#define MAP_osal_bm_adjust_header                                    osal_bm_adjust_header
#define MAP_osal_bm_free                                             osal_bm_free
#define MAP_osal_buffer_uint32                                       osal_buffer_uint32
#define MAP_osal_build_uint32                                        osal_build_uint32
#define MAP_osal_start_timerEx                                       osal_start_timerEx
#define MAP_osal_stop_timerEx                                        osal_stop_timerEx
#define MAP_osal_clear_event                                         osal_clear_event
#define MAP_osal_memcmp                                              osal_memcmp
#define MAP_osal_memcpy                                              osal_memcpy
#define MAP_osal_memdup                                              osal_memdup
#define MAP_osal_memset                                              osal_memset
#define MAP_osal_CbTimerStart                                        osal_CbTimerStart
#define MAP_osal_CbTimerStop                                         osal_CbTimerStop
#define MAP_osal_CbTimerUpdate                                       osal_CbTimerUpdate
#define MAP_osal_mem_alloc                                           osal_mem_alloc
#define MAP_osal_mem_free                                            osal_mem_free
#define MAP_osal_mem_allocLimited                                    osal_mem_allocLimited
#define MAP_osal_strlen                                              osal_strlen
#define MAP_osal_list_clearList                                      osal_list_clearList
#define MAP_osal_list_empty                                          osal_list_empty
#define MAP_osal_list_get                                            osal_list_get
#define MAP_osal_list_head                                           osal_list_head
#define MAP_osal_list_insert                                         osal_list_insert
#define MAP_osal_list_next                                           osal_list_next
#define MAP_osal_list_prev                                           osal_list_prev
#define MAP_osal_list_put                                            osal_list_put
#define MAP_osal_list_putHead                                        osal_list_putHead
#define MAP_osal_list_remove                                         osal_list_remove
#define MAP_osal_list_tail                                           osal_list_tail
#define MAP_osal_get_timeoutEx                                       osal_get_timeoutEx

// RF HAL
#define MAP_RFHAL_InitRfHal                                          RFHAL_InitRfHal
#define MAP_RFHAL_AllocRatChan                                       RFHAL_AllocRatChan
#define MAP_RFHAL_FreeRatChan                                        RFHAL_FreeRatChan
#define MAP_RFHAL_MapRatChansToInt                                   RFHAL_MapRatChansToInt
#define MAP_RFHAL_RegisterRatChanCallback                            RFHAL_RegisterRatChanCallback
#define MAP_RFHAL_AddTxDataEntry                                     RFHAL_AddTxDataEntry
#define MAP_RFHAL_InitDataQueue                                      RFHAL_InitDataQueue
#define MAP_RFHAL_FreeNextTxDataEntry                                RFHAL_FreeNextTxDataEntry
#define MAP_RFHAL_AllocDataEntryQueue                                RFHAL_AllocDataEntryQueue
#define MAP_RFHAL_FreeDataEntryQueue                                 RFHAL_FreeDataEntryQueue
#define MAP_RFHAL_GetNextDataEntry                                   RFHAL_GetNextDataEntry
#define MAP_RFHAL_GetTempDataEntry                                   RFHAL_GetTempDataEntry
#define MAP_RFHAL_NextDataEntryDone                                  RFHAL_NextDataEntryDone
#define MAP_RFHAL_AllocDataEntry                                     RFHAL_AllocDataEntry
#define MAP_RFHAL_FreeDataEntry                                      RFHAL_FreeDataEntry
#define MAP_RFHAL_BuildRingBuffer                                    RFHAL_BuildRingBuffer
#define MAP_RFHAL_BuildDataEntryRingBuffer                           RFHAL_BuildDataEntryRingBuffer
#define MAP_RFHAL_BuildLinkedBuffer                                  RFHAL_BuildLinkedBuffer

// Misc.
#define MAP_Onboard_soft_reset                                       Onboard_soft_reset
#define MAP_IntMasterEnable                                          IntMasterEnable
#define MAP_IntMasterDisable                                         IntMasterDisable
#define MAP_IntEnable                                                IntEnable
#define MAP_IntDisable                                               IntDisable
#define MAP_halAssertHandler                                         halAssertHandler
#define MAP_HalTRNG_InitTRNG                                         HalTRNG_InitTRNG
#define MAP_HalTRNG_GetTRNG                                          HalTRNG_GetTRNG

#ifdef ONE_BLE_LIB_SIZE_OPTIMIZATION
#define MAP_rfCallback                                               rfCallback_hook
#else
#define MAP_rfCallback                                               rfCallback
#endif

#define MAP_rfPUpCallback                                            rfPUpCallback
#define MAP_rfErrorCallback                                          rfErrorCallback

/*******************************************************************************
 * RTLS hooks
 */
extern uint8 MAP_llGetCteInfo( uint8 id, void *ptr );
extern uint8 MAP_RTLSSrv_processHciEvent(uint16_t hciEvt, uint16_t hciEvtSz, uint8_t *pEvtData);
extern uint8 MAP_LL_EnhancedCteRxTest( uint8 rxChan,
                                            uint8 rxPhy,
                                            uint8 modIndex,
                                            uint8 expectedCteLength,
                                            uint8 expectedCteType,
                                            uint8 slotDurations,
                                            uint8 length,
                                            uint8 *pAntenna);

extern uint8 MAP_LL_EnhancedCteTxTest( uint8 txChan,
                                            uint8 payloadLen,
                                            uint8 payloadType,
                                            uint8 txPhy,
                                            uint8 cteLength,
                                            uint8 cteType,
                                            uint8 length,
                                            uint8 *pAntenna);

extern uint8 MAP_LL_DirectCteTestTxTest( uint8 txChan,
                                          uint8 payloadLen,
                                          uint8 payloadType,
                                          uint8 txPhy,
                                          uint8 cteLength,
                                          uint8 cteType,
                                          uint8 length,
                                          uint8 *pAntenna);

extern uint8 MAP_LL_DirectCteTestRxTest( uint8 rxChan,
                                          uint8 rxPhy,
                                          uint8 modIndex,
                                          uint8 expectedCteLength,
                                          uint8 expectedCteType,
                                          uint8 slotDurations,
                                          uint8 length,
                                          uint8 *pAntenna);

extern uint8 MAP_LL_SetConnectionCteReceiveParams( uint16 connHandle, uint8 samplingEnable,
                                                   uint8 slotDurations, uint8 length, uint8 *pAntenna );
extern uint8 MAP_LL_SetConnectionCteTransmitParams( uint16 connHandle, uint8  types,
                                                    uint8 length, uint8 *pAntenna );
extern uint8 MAP_LL_SetConnectionCteRequestEnable( uint16 connHandle, uint8 enable,
                                                   uint16 interval, uint8 length, uint8 type );
extern uint8 MAP_LL_SetConnectionCteResponseEnable( uint16 connHandle, uint8 enable );
extern uint8 MAP_LL_ReadAntennaInformation( uint8 *sampleRates, uint8 *maxNumOfAntennas,
                                            uint8 *maxSwitchPatternLen, uint8 *maxCteLen);
extern void MAP_llUpdateCteState( void *connPtr);
extern uint8 MAP_llSetupCte( void *connPtr, uint8 req);
extern uint8 MAP_llFreeCteSamplesEntryQueue( void );
extern uint8 MAP_LL_EXT_SetLocationingAccuracy( uint16 handle, uint8  sampleRate1M, uint8  sampleSize1M,
                                                uint8  sampleRate2M, uint8  sampleSize2M, uint8  sampleCtrl);
extern void MAP_llSetRfReportAodPackets( void );
extern void MAP_llMaster_TaskEnd(void);
extern void MAP_llSlave_TaskEnd(void);
extern void MAP_HCI_CteRequestFailedEvent( uint8  status, uint16 connHandle);

/*******************************************************************************
 * RF hooks
 */
extern void  MAP_rf_patch_cpe(void);
extern void  MAP_rf_patch_rfe(void);
extern void  MAP_rf_patch_mce(void);

/*******************************************************************************
 * DMM hooks
 */
extern void   MAP_llDmmSetThreshold(uint8 state, uint8 handle, uint8 reset);
extern uint32 MAP_llDmmGetActivityIndex(uint16 cmdNum);
extern uint8  MAP_llDmmSetAdvHandle(uint8 handle, uint8 clear);
extern void   MAP_llDmmDynamicFree(void);
extern uint8  MAP_llDmmDynamicAlloc(void);
extern uint32_t MAP_LL_AbortedCback( uint8 preempted );
extern uint8  MAP_llSetStarvationMode(uint16 connId, uint8 setOnOffValue);


/*******************************************************************************
 * Coex hooks
 */
extern void  *MAP_llCoexGetParams(uint16 cmdNum);

/*******************************************************************************
 * Periodic Adv hooks
 */
extern uint8 MAP_LE_SetPeriodicAdvParams( uint8 advHandle,
                                          uint16 periodicAdvIntervalMin,
                                          uint16 periodicAdvIntervalMax,
                                          uint16 periodicAdvProp );

extern uint8 MAP_LE_SetPeriodicAdvData( uint8 advHandle, uint8 operation,
                                        uint8 dataLength, uint8 *data );

extern uint8 MAP_LE_SetPeriodicAdvEnable( uint8 enable, uint8 advHandle );
extern uint8 MAP_LE_SetConnectionlessCteTransmitParams( uint8 advHandle, uint8 cteLen,
                                                        uint8 cteType, uint8 cteCount,
                                                        uint8 length, uint8 *pAntenna );
extern uint8 MAP_LE_SetConnectionlessCteTransmitEnable( uint8 advHandle, uint8 enable );
extern void *MAP_llGetPeriodicAdv( uint8 handle );
extern void MAP_llUpdatePeriodicAdvChainPacket( void );
extern void MAP_llSetPeriodicAdvChmapUpdate( uint8 set );
extern void MAP_llPeriodicAdv_PostProcess( void );
extern uint8 MAP_llTrigPeriodicAdv( void *pAdvSet, void *pPeriodicAdv );
extern uint8 MAP_llSetupPeriodicAdv( void *pAdvSet );
extern void MAP_llEndPeriodicAdvTask( void *pPeriodicAdv );
extern void *MAP_llFindNextPeriodicAdv( void );
extern void MAP_llSetPeriodicSyncInfo( void *pAdvSet, uint8 *pBuf );
extern void *MAP_llGetCurrentPeriodicAdv( void );
extern uint8 MAP_gapAdv_periodicAdvCmdCompleteCBs( void *pMsg );
extern void MAP_llClearPeriodicAdvSets( void );

/*******************************************************************************
 * Periodic Scan hooks
 */
extern uint8 MAP_LE_PeriodicAdvCreateSync( uint8  options,
                                                uint8  advSID,
                                                uint8  advAddrType,
                                                uint8  *advAddress,
                                                uint16 skip,
                                                uint16 syncTimeout,
                                                uint8  syncCteType );
extern uint8 MAP_LE_PeriodicAdvCreateSyncCancel( void );
extern uint8 MAP_LE_PeriodicAdvTerminateSync( uint16 syncHandle );
extern uint8 MAP_LE_AddDeviceToPeriodicAdvList( uint8 advAddrType,
                                                     uint8 *advAddress,
                                                     uint8 advSID );
extern uint8 MAP_LE_RemoveDeviceFromPeriodicAdvList( uint8 advAddrType,
                                                          uint8 *advAddress,
                                                          uint8 advSID );
extern uint8 MAP_LE_ClearPeriodicAdvList( void );
extern uint8 MAP_LE_ReadPeriodicAdvListSize( uint8 *listSize );
extern uint8 MAP_LE_SetPeriodicAdvReceiveEnable( uint16 syncHandle, uint8  enable );
extern uint8 MAP_LE_SetConnectionlessIqSamplingEnable( uint16 syncHandle, uint8 samplingEnable,
                                                       uint8 slotDurations, uint8 maxSampledCtes,
                                                       uint8 length, uint8 *pAntenna );

extern uint8 MAP_llProcessExtScanRxFIFO_hook(void);
extern void MAP_llProcessPeriodicScanSyncInfo( uint8 *pPkt, void *advEvent, uint32 timeStamp, uint8 phy );
extern void MAP_llEndPeriodicScanTask( void *pPeriodicScan );
extern void MAP_llPeriodicScan_PostProcess( void );
extern void MAP_llProcessPeriodicScanRxFIFO( void );
extern void *MAP_llFindNextPeriodicScan( void );
extern void MAP_llTerminatePeriodicScan( void );
extern void *MAP_llGetPeriodicScan( uint16 handle );
extern void *MAP_llGetCurrentPeriodicScan( uint8 state );
extern uint8 MAP_llGetPeriodicScanCteTasks( void );
extern uint8_t MAP_gapScan_periodicAdvCmdCompleteCBs( void *pMsg );
extern uint8_t MAP_gapScan_periodicAdvCmdStatusCBs( void *pMsg );
extern uint8_t MAP_gapScan_processBLEPeriodicAdvCBs( void *pMsg );
extern void MAP_llClearPeriodicScanSets( void );
extern void MAP_llUpdateExtScanAcceptSyncInfo( void );

extern uint8_t LE_SetExtAdvData_hook( void *pMsg );
extern uint8_t LE_SetExtScanRspData_hook( void *pMsg );
extern uint8_t LE_SetExtAdvEnable_hook(void *pMsg );
extern uint8 MAP_gapAdv_handleAdvHciCmdComplete( void *pMsg );

/******************************************************************************
* Single Connection
*/
extern uint8 llGetNextConn_hook( void );
extern uint32 MAP_llCalcConnMaxTimeLength(uint16 startConnId, uint16 bestSelectedConnIdAfterStart);
extern uint8 MAP_llFindStartType( void *secTask, void *primTask );
extern void MAP_llPeriodicAdv_Config( void *pAdvSet );

/******************************************************************************
* Legacy Adv Only
*/
extern uint8 llSetupExtAdv_hook( void *pAdvSet );
extern uint8 llNextChanIndex_hook( uint16 eventCounter );
extern void  MAP_llUpdateExtAdvTx( void *pAdvSet );
extern void  MAP_llPrepareNextExtAdv( void *pAdvSet );
extern void  MAP_llInitFeatureSet( void );
/*******************************************************************************
 * Secure Manager hooks
 */
extern void  SM_dhKeyCB_hook( void *pDhKey );
extern void  SM_p256KeyCB_hook( void *pK, uint8 *privateKey );
extern uint8 smpInitiatorProcessPairingRandom_hook( void *pParsedMsg, uint16_t connHandle);
extern uint8 smpResponderProcessPairingPublicKey_hook( void *pParsedMsg );
extern uint8 smResponderProcessLTKReq_hook( uint16 connectionHandle, uint8 *pRandom, uint16 encDiv );
extern uint8 smGeneratePairingReqRsp_hook( void );
extern void  gapSendSlaveSecurityReqEvent_hook( uint8 taskID, uint16 connHandle, uint8 *pDevAddr, uint8 authReq );
#endif // MAP_DIRECT_H
