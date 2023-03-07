/******************************************************************************

 @file  ble_dispatch_lite_idx.h

 @brief list the index of the ble API jump table.
        The BLE API jump table, and the indexes bellow are automatically
        generated with a script. Manual modifications can be done, but if
        permanent modification are sought, then the script and the template should
        be used in order to avoid mis-alignment between indexes and the jump
        table.
        if this file is changed, both stack and application project need to be
        rebuilt, stack first.

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

#ifndef BLE_DISPATCH_LITE_IDX_H
#define BLE_DISPATCH_LITE_IDX_H

#define JT_INDEX(index)                             (index)

// BLE APIs Jumpt Table
//
// <START TABLE - DO NOT REMOVE!>
 /* HCI API */
/***********/
#define IDX_HCI_ReadRssiCmd                              JT_INDEX(0)
#define IDX_HCI_SetEventMaskCmd                          JT_INDEX(1)
#define IDX_HCI_SetEventMaskPage2Cmd                     JT_INDEX(2)
#define IDX_HCI_ResetCmd                                 JT_INDEX(3)
#define IDX_HCI_ReadLocalVersionInfoCmd                  JT_INDEX(4)
#define IDX_HCI_ReadLocalSupportedCommandsCmd            JT_INDEX(5)
#define IDX_HCI_ReadLocalSupportedFeaturesCmd            JT_INDEX(6)
#define IDX_HCI_ReadBDADDRCmd                            JT_INDEX(7)
#define IDX_HCI_LE_ReceiverTestCmd                       JT_INDEX(8)
#define IDX_HCI_LE_TransmitterTestCmd                    JT_INDEX(9)
#define IDX_HCI_LE_TestEndCmd                            JT_INDEX(10)
#define IDX_HCI_LE_EncryptCmd                            JT_INDEX(11)
#define IDX_HCI_LE_RandCmd                               JT_INDEX(12)
#define IDX_HCI_LE_ReadSupportedStatesCmd                JT_INDEX(13)
#define IDX_HCI_LE_ReadWhiteListSizeCmd                  JT_INDEX(14)
#define IDX_HCI_LE_ClearWhiteListCmd                     JT_INDEX(15)
#define IDX_HCI_LE_AddWhiteListCmd                       JT_INDEX(16)
#define IDX_HCI_LE_RemoveWhiteListCmd                    JT_INDEX(17)
#define IDX_HCI_LE_SetEventMaskCmd                       JT_INDEX(18)
#define IDX_HCI_LE_ReadLocalSupportedFeaturesCmd         JT_INDEX(19)
#define IDX_HCI_LE_ReadBufSizeCmd                        JT_INDEX(20)
#define IDX_HCI_LE_SetRandomAddressCmd                   JT_INDEX(21)

// <<INSERT:#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG))>>
// <<BEGIN_COUNT>>
#define IDX_HCI_LE_ReadAdvChanTxPowerCmd                 JT_INDEX(22)
// <<END_COUNT>>
// <<INSERT:#else // !(defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG)))>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif //  (defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG)))>>

// <<INSERT:#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))>>
// <<BEGIN_COUNT>>
#define IDX_HCI_DisconnectCmd                            JT_INDEX(23)
#define IDX_HCI_LE_ReadChannelMapCmd                     JT_INDEX(24)
#define IDX_HCI_LE_ReadRemoteUsedFeaturesCmd             JT_INDEX(25)
#define IDX_HCI_ReadRemoteVersionInfoCmd                 JT_INDEX(26)
#define IDX_HCI_ReadTransmitPowerLevelCmd                JT_INDEX(27)
#define IDX_HCI_SetControllerToHostFlowCtrlCmd           JT_INDEX(28)
#define IDX_HCI_HostBufferSizeCmd                        JT_INDEX(29)
#define IDX_HCI_HostNumCompletedPktCmd                   JT_INDEX(30)
// <<END_COUNT>>
// <<INSERT:#else // !(defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG)))>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif //  (defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG)))>>

// <<INSERT:#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (INIT_CFG))>>
// <<BEGIN_COUNT>>
#define IDX_HCI_LE_SetHostChanClassificationCmd          JT_INDEX(31)
// <<END_COUNT>>
// <<INSERT:#else // !(defined(CTRL_CONFIG) && (CTRL_CONFIG & (INIT_CFG)))>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif //  (defined(CTRL_CONFIG) && (CTRL_CONFIG & (INIT_CFG)))>>

// <<INSERT:#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))>>
// <<BEGIN_COUNT>>
#define IDX_HCI_LE_ReadMaxDataLenCmd                     JT_INDEX(32)
#define IDX_HCI_LE_SetDataLenCmd                         JT_INDEX(33)
#define IDX_HCI_LE_WriteSuggestedDefaultDataLenCmd       JT_INDEX(34)
#define IDX_HCI_LE_ReadSuggestedDefaultDataLenCmd        JT_INDEX(35)
// <<END_COUNT>>
// <<INSERT:#else // !(...)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif //  (...)>>

// <<INSERT:#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))>>
// <<BEGIN_COUNT>>
#define IDX_HCI_ReadAuthPayloadTimeoutCmd                JT_INDEX(36)
#define IDX_HCI_WriteAuthPayloadTimeoutCmd               JT_INDEX(37)
#define IDX_HCI_LE_RemoteConnParamReqReplyCmd            JT_INDEX(38)
#define IDX_HCI_LE_RemoteConnParamReqNegReplyCmd         JT_INDEX(39)
// <<END_COUNT>>
// <<INSERT:#else // !(...)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // (...)>>

#define IDX_HCI_LE_AddDeviceToResolvingListCmd           JT_INDEX(40)
#define IDX_HCI_LE_RemoveDeviceFromResolvingListCmd      JT_INDEX(41)
#define IDX_HCI_LE_ClearResolvingListCmd                 JT_INDEX(42)
#define IDX_HCI_LE_ReadResolvingListSizeCmd              JT_INDEX(43)
#define IDX_HCI_LE_ReadPeerResolvableAddressCmd          JT_INDEX(44)
#define IDX_HCI_LE_ReadLocalResolvableAddressCmd         JT_INDEX(45)
#define IDX_HCI_LE_SetAddressResolutionEnableCmd         JT_INDEX(46)
#define IDX_HCI_LE_SetResolvablePrivateAddressTimeoutCmd JT_INDEX(47)
#define IDX_HCI_LE_SetPrivacyModeCmd                     JT_INDEX(48)
#define IDX_HCI_LE_ReadLocalP256PublicKeyCmd             JT_INDEX(49)
#define IDX_HCI_LE_GenerateDHKeyCmd                      JT_INDEX(50)

// <<INSERT:#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG)) &&  \>>
// <<INSERT:    defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))>>
// <<BEGIN_COUNT>>
#define IDX_HCI_LE_ReadPhyCmd                            JT_INDEX(51)
#define IDX_HCI_LE_SetDefaultPhyCmd                      JT_INDEX(52)
#define IDX_HCI_LE_SetPhyCmd                             JT_INDEX(53)
#define IDX_HCI_LE_EnhancedRxTestCmd                     JT_INDEX(54)
#define IDX_HCI_LE_EnhancedTxTestCmd                     JT_INDEX(55)
// <<END_COUNT>>
// <<INSERT:#else // !(...)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // (...)>>

#define IDX_HCI_LE_ReadTxPowerCmd                        JT_INDEX(56)
#define IDX_HCI_LE_ReadRfPathCompCmd                     JT_INDEX(57)
#define IDX_HCI_LE_WriteRfPathCompCmd                    JT_INDEX(58)

// <<INSERT:#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG) && \>>
// <<INSERT:    defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | ADV_NCONN_CFG))>>
// <<BEGIN_COUNT>>
#define IDX_LE_SetAdvSetRandAddr                         JT_INDEX(59)
#define IDX_LE_SetExtAdvParams                           JT_INDEX(60)
#define IDX_LE_SetExtAdvData                             JT_INDEX(61)
#define IDX_LE_SetExtScanRspData                         JT_INDEX(62)
#define IDX_LE_SetExtAdvEnable                           JT_INDEX(63)
#define IDX_LE_RemoveAdvSet                              JT_INDEX(64)
#define IDX_LE_ClearAdvSets                              JT_INDEX(65)
#define IDX_LE_ReadMaxAdvDataLen                         JT_INDEX(66)
#define IDX_LE_ReadNumSupportedAdvSets                   JT_INDEX(67)
// <<END_COUNT>>
// <<INSERT:#else // !(AE_CFG && (ADV_CONN_CFG | ADV_NCONN_CFG))>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // AE_CFG && ADV_CONN_CFG | ADV_NCONN_CFG>>

// <<INSERT:#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG) && \>>
// <<INSERT:    defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)>>
// <<BEGIN_COUNT>>
#define IDX_LE_SetExtScanParams                          JT_INDEX(68)
#define IDX_LE_SetExtScanEnable                          JT_INDEX(69)
// <<END_COUNT>>
// <<INSERT:#else // !(AE_CFG && SCAN_CFG)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // AE_CFG &&SCAN_CFG>>

// <<INSERT:#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG) && \>>
// <<INSERT:    defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)>>
#define IDX_LE_ExtCreateConn                             JT_INDEX(70)
// <<INSERT:#else // !(AE_CFG && INIT_CFG)>>
// <<INSERT:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // AE_CFG && INIT_CFG>>

// <<INSERT:#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG)>>
// <<BEGIN_COUNT>>
#define IDX_LL_AE_RegCBack                               JT_INDEX(71)
// <<END_COUNT>>
// <<INSERT:#else // !AE_CFG>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // AE_CFG>>

// <<INSERT:#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG) && \>>
// <<INSERT:    defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))>>
// <<BEGIN_COUNT>>
#define IDX_LL_GetAdvSet                                 JT_INDEX(72)
#define IDX_LL_CountAdvSets                              JT_INDEX(73)
#define IDX_llAllocRfMem                                 JT_INDEX(74)
#define IDX_llSetupExtAdv                                JT_INDEX(75)
#define IDX_llSetupExtAdvLegacy                          JT_INDEX(76)
#define IDX_llExtAdvCBack                                JT_INDEX(77)
#define IDX_llSetupExtHdr                                JT_INDEX(78)
#define IDX_llSetupExtData                               JT_INDEX(79)
#define IDX_llGetExtHdrLen                               JT_INDEX(80)
// <<END_COUNT>>
// <<INSERT:#else // !(AE_CFG && (ADV_CONN_CFG | ADV_NCONN_CFG)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // AE_CFG && ADV_CONN_CFG | ADV_NCONN_CFG>>

// <<INSERT:#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG) && \>>
// <<INSERT:    defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)>>
#define IDX_llSetupExtScan                               JT_INDEX(81)
// <<INSERT:#else // !(AE_CFG && SCAN_CFG)>>
// <<INSERT:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // AE_CFG && SCAN_CFG>>

// <<INSERT:#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG) && \>>
// <<INSERT:    defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)>>
// <<BEGIN_COUNT>>
#define IDX_llSetupExtInit                               JT_INDEX(82)
#define IDX_llNextChanIndex                              JT_INDEX(83)
// <<END_COUNT>>
// <<INSERT:#else // !(AE_CFG && INIT_CFG)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // AE_CFG && INIT_CFG>>

// <<INSERT:#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))>>
// <<BEGIN_COUNT>>
#define IDX_HCI_LE_SetAdvParamCmd                        JT_INDEX(84)
#define IDX_HCI_LE_SetAdvDataCmd                         JT_INDEX(85)
#define IDX_HCI_LE_SetScanRspDataCmd                     JT_INDEX(86)
#define IDX_HCI_LE_SetAdvEnableCmd                       JT_INDEX(87)
// <<END_COUNT>>
// <<INSERT:#else // !(...)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // (...)>>

// <<INSERT:#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)>>
// <<BEGIN_COUNT>>
#define IDX_HCI_LE_SetScanParamCmd                       JT_INDEX(88)
#define IDX_HCI_LE_SetScanEnableCmd                      JT_INDEX(89)
// <<END_COUNT>>
// <<INSERT:#else // !(...)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // (...)>>

// <<INSERT:#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)>>
// <<BEGIN_COUNT>>
#define IDX_HCI_LE_CreateConnCmd                         JT_INDEX(90)
#define IDX_HCI_LE_CreateConnCancelCmd                   JT_INDEX(91)
#define IDX_HCI_LE_StartEncyptCmd                        JT_INDEX(92)
// <<END_COUNT>>
// <<INSERT:#else // !(...)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // (...)>>

// <<INSERT:#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))>>
// <<BEGIN_COUNT>>
#define IDX_HCI_LE_ConnUpdateCmd                         JT_INDEX(93)
// <<END_COUNT>>
// <<INSERT:#else // !(...)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // (...)>>

// <<INSERT:#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG)>>
// <<BEGIN_COUNT>>
#define IDX_HCI_LE_LtkReqReplyCmd                        JT_INDEX(94)
#define IDX_HCI_LE_LtkReqNegReplyCmd                     JT_INDEX(95)
// <<END_COUNT>>
// <<INSERT:#else // !(...)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // (...)>>

/* HCI Vendor Specific (Extended) API */
/**************************************/
#define IDX_HCI_EXT_SetTxPowerCmd                        JT_INDEX(96)
#define IDX_HCI_EXT_BuildRevisionCmd                     JT_INDEX(97)
#define IDX_HCI_EXT_DelaySleepCmd                        JT_INDEX(98)
#define IDX_HCI_EXT_DecryptCmd                           JT_INDEX(99)
#define IDX_HCI_EXT_EnablePTMCmd                         JT_INDEX(100)
#define IDX_HCI_EXT_ModemTestTxCmd                       JT_INDEX(101)
#define IDX_HCI_EXT_ModemHopTestTxCmd                    JT_INDEX(102)
#define IDX_HCI_EXT_ModemTestRxCmd                       JT_INDEX(103)
#define IDX_HCI_EXT_EnhancedModemTestTxCmd               JT_INDEX(104)
#define IDX_HCI_EXT_EnhancedModemHopTestTxCmd            JT_INDEX(105)
#define IDX_HCI_EXT_EnhancedModemTestRxCmd               JT_INDEX(106)
#define IDX_HCI_EXT_EndModemTestCmd                      JT_INDEX(107)
#define IDX_HCI_EXT_SetBDADDRCmd                         JT_INDEX(108)
#define IDX_HCI_EXT_ResetSystemCmd                       JT_INDEX(109)
#define IDX_HCI_EXT_SetLocalSupportedFeaturesCmd         JT_INDEX(110)
#define IDX_HCI_EXT_SetMaxDtmTxPowerCmd                  JT_INDEX(111)
#define IDX_HCI_EXT_SetRxGainCmd                         JT_INDEX(112)
#define IDX_HCI_EXT_ExtendRfRangeCmd                     JT_INDEX(113)
#define IDX_HCI_EXT_HaltDuringRfCmd                      JT_INDEX(114)
#define IDX_HCI_EXT_ClkDivOnHaltCmd                      JT_INDEX(115)
#define IDX_HCI_EXT_DeclareNvUsageCmd                    JT_INDEX(116)
#define IDX_HCI_EXT_MapPmIoPortCmd                       JT_INDEX(117)
#define IDX_HCI_EXT_SetFreqTuneCmd                       JT_INDEX(118)
#define IDX_HCI_EXT_SaveFreqTuneCmd                      JT_INDEX(119)

// <<INSERT:#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))>>
// <<BEGIN_COUNT>>
#define IDX_HCI_EXT_DisconnectImmedCmd                   JT_INDEX(120)
#define IDX_HCI_EXT_PacketErrorRateCmd                   JT_INDEX(121)
#define IDX_HCI_EXT_NumComplPktsLimitCmd                 JT_INDEX(122)
#define IDX_HCI_EXT_OnePktPerEvtCmd                      JT_INDEX(123)
#define IDX_HCI_EXT_SetSCACmd                            JT_INDEX(124)
#define IDX_HCI_EXT_GetConnInfoCmd                       JT_INDEX(125)
#define IDX_HCI_EXT_OverlappedProcessingCmd              JT_INDEX(126)
// <<END_COUNT>>
// <<INSERT:#else // !(defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG)))>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif //  (defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG)))>>

// <<INSERT:#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG)>>
// <<BEGIN_COUNT>>
#define IDX_HCI_EXT_SetFastTxResponseTimeCmd             JT_INDEX(127)
#define IDX_HCI_EXT_SetSlaveLatencyOverrideCmd           JT_INDEX(128)
// <<END_COUNT>>
// <<INSERT:#else // !INIT_CFG>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ADV_CONN_CFG>>

// <<INSERT:#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))>>
// <<BEGIN_COUNT>>
#define IDX_HCI_EXT_SetMaxDataLenCmd                     JT_INDEX(129)
// <<END_COUNT>>
// <<INSERT:#else // !(...)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // (...)>>

// <<INSERT:#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_NCONN_CFG)>>
// <<BEGIN_COUNT>>
#define IDX_HCI_EXT_SetVirtualAdvAddrCmd                 JT_INDEX(130)
// <<END_COUNT>>
// <<INSERT:#else // !(...)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // (...)>>

// <<INSERT:#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)>>
// <<BEGIN_COUNT>>
#define IDX_HCI_EXT_SetExtScanChannels                   JT_INDEX(131)
// <<END_COUNT>>
// <<INSERT:#else // !(...)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // (...)>>

#define IDX_HCI_EXT_SetQOSParameters	                 JT_INDEX(132)
#define IDX_HCI_EXT_SetQOSDefaultParameters	             JT_INDEX(133)

// <<INSERT:#ifdef LL_TEST_MODE>>
// <<BEGIN_COUNT>>
#define IDX_HCI_EXT_LLTestModeCmd                        JT_INDEX(134)
// <<END_COUNT>>
// <<INSERT:#else // !(...)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // (...)>>

#define IDX_HCI_EXT_SetDtmTxPktCntCmd                    JT_INDEX(135)

// <<INSERT:#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))>>
#define IDX_HCI_SendDataPkt                              JT_INDEX(136)
// <<INSERT:#endif // ADV_CONN_CFG | INIT_CFG>>


// <<INSERT:#if defined(HCI_TL_FULL)>>
// <<BEGIN_COUNT>>
#define IDX_HCI_CommandStatusEvent                       JT_INDEX(137)
#define IDX_HCI_CommandCompleteEvent                     JT_INDEX(138)
#define IDX_HCI_bm_alloc                                 JT_INDEX(139)
// <<END_COUNT>>
// <<INSERT:#else // !defined(HCI_TL_FULL)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // defined(HCI_TL_FULL)>>

// <<INSERT:#if defined(HCI_TL_FULL)>>
// <<BEGIN_COUNT>>
#define IDX_HCI_TL_getCmdResponderID                     JT_INDEX(140)
// <<END_COUNT>>
// <<INSERT:#else // !defined(HCI_TL_FULL)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // defined(HCI_TL_FULL)>>


/* GAP-GATT service API */
/************************/
// <<INSERT:#if defined(HOST_CONFIG)>>
// <<BEGIN_COUNT>>
#define IDX_GGS_SetParameter                             JT_INDEX(141)
#define IDX_GGS_AddService                               JT_INDEX(142)
#define IDX_GGS_GetParameter                             JT_INDEX(143)
// <<END_COUNT>>
// <<INSERT:#else // !defined(HOST_CONFIG)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // defined(HOST_CONFIG)>>

// <<INSERT:#if ( HOST_CONFIG & PERIPHERAL_CFG ) & defined ( GAP_PRIVACY_RECONNECT )>>
// <<BEGIN_COUNT>>
#define IDX_GGS_RegisterAppCBs                           JT_INDEX(144)
// <<END_COUNT>>
// <<INSERT:#else // !( HOST_CONFIG & PERIPHERAL_CFG ) & defined ( GAP_PRIVACY_RECONNECT )>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( HOST_CONFIG & PERIPHERAL_CFG ) & defined ( GAP_PRIVACY_RECONNECT )>>

// <<INSERT:#if ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES) )))>>
// <<BEGIN_COUNT>>
#define IDX_GGS_SetParamValue                            JT_INDEX(145)
#define IDX_GGS_GetParamValue                            JT_INDEX(146)
// <<END_COUNT>>
// <<INSERT:#else // !( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES))  ))>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES))  ))>>


/* GAP Bond Manager API */
/************************/
// <<INSERT:#if defined(GAP_BOND_MGR)>>
// <<BEGIN_COUNT>>
#define IDX_GAPBondMgr_SetParameter                      JT_INDEX(147)
#define IDX_GAPBondMgr_GetParameter                      JT_INDEX(148)
#define IDX_GAPBondMgr_Pair                              JT_INDEX(149)
#define IDX_GAPBondMgr_Register                          JT_INDEX(150)
#define IDX_GAPBondMgr_PasscodeRsp                       JT_INDEX(151)
#define IDX_GAPBondMgr_FindAddr                          JT_INDEX(152)
#define IDX_GAPBondMgr_SCGetLocalOOBParameters           JT_INDEX(153)
#define IDX_GAPBondMgr_SCSetRemoteOOBParameters          JT_INDEX(154)
#define IDX_GAPBondMgr_GenerateEccKeys                   JT_INDEX(155)
// <<END_COUNT>>
// <<INSERT:#else // !( GAP_BOND_MGR )>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( GAP_BOND_MGR )>>

// <<INSERT:#if defined(GAP_BOND_MGR) && !defined(GATT_NO_SERVICE_CHANGED)>>
// <<BEGIN_COUNT>>
#define IDX_GAPBondMgr_ServiceChangeInd                  JT_INDEX(156)
// <<END_COUNT>>
// <<INSERT:#else // !( defined(GAP_BOND_MGR) && !defined(GATT_NO_SERVICE_CHANGED) )>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( defined(GAP_BOND_MGR) && !defined(GATT_NO_SERVICE_CHANGED) )>>

/* GAP API */
/***********/
// <<INSERT:#if defined(HOST_CONFIG)>>
// <<BEGIN_COUNT>>
#define IDX_GAP_DeviceInit                               JT_INDEX(157)
#define IDX_GAP_RegisterForMsgs                          JT_INDEX(158)
#define IDX_GAP_SetParamValue                            JT_INDEX(159)
#define IDX_GAP_GetParamValue                            JT_INDEX(160)
#define IDX_GapConfig_SetParameter                       JT_INDEX(161)
#define IDX_GAP_GetIRK                                   JT_INDEX(162)
#define IDX_GAP_GetSRK                                   JT_INDEX(163)
#define IDX_GAP_DeInit                                   JT_INDEX(164)
#define IDX_GAP_ReInit                                   JT_INDEX(165)
// <<END_COUNT>>
// <<INSERT:#else // !defined(HOST_CONFIG)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // defined(HOST_CONFIG)>>

// <<INSERT:#if ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ))>>
// <<BEGIN_COUNT>>
#define IDX_GAP_UpdateLinkParamReq                       JT_INDEX(166)
#define IDX_GAP_TerminateLinkReq                         JT_INDEX(167)
#define IDX_GAP_UpdateLinkParamReqReply                  JT_INDEX(168)
#define IDX_GAP_Gap_RegisterConnEventCb                  JT_INDEX(169)
// <<END_COUNT>>
// <<INSERT:#else // !( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )>>

// <<INSERT:#if ( HOST_CONFIG & ( PERIPHERAL_CFG | BROADCASTER_CFG ))>>
// <<BEGIN_COUNT>>
#define IDX_GapAdv_create                                JT_INDEX(170)
#define IDX_GapAdv_destroy                               JT_INDEX(171)
#define IDX_GapAdv_enable                                JT_INDEX(172)
#define IDX_GapAdv_disable                               JT_INDEX(173)
#define IDX_GapAdv_setParam                              JT_INDEX(174)
#define IDX_GapAdv_getParam                              JT_INDEX(175)
#define IDX_GapAdv_getBuffer                             JT_INDEX(176)
#define IDX_GapAdv_loadByHandle                          JT_INDEX(177)
#define IDX_GapAdv_loadByBuffer                          JT_INDEX(178)
#define IDX_GapAdv_prepareLoadByHandle                   JT_INDEX(179)
#define IDX_GapAdv_prepareLoadByBuffer                   JT_INDEX(180)
#define IDX_GapAdv_setEventMask                          JT_INDEX(181)
#define IDX_GapAdv_abortLoad                             JT_INDEX(182)
#define IDX_GapAdv_setVirtualAdvAddr                     JT_INDEX(183)
#define IDX_GapAdv_SetPeriodicAdvParams                  JT_INDEX(184)
#define IDX_GapAdv_SetPeriodicAdvData                    JT_INDEX(185)
#define IDX_GapAdv_SetPeriodicAdvEnable                  JT_INDEX(186)
// <<END_COUNT>>
// <<INSERT:#else // !( HOST_CONFIG & ( PERIPHERAL_CFG | BROADCASTER_CFG ) )>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( HOST_CONFIG & ( PERIPHERAL_CFG | BROADCASTER_CFG ) )>>

// <<INSERT:#if ( HOST_CONFIG & ( CENTRAL_CFG | OBSERVER_CFG ))>>
// <<BEGIN_COUNT>>
#define IDX_GapScan_registerCb                           JT_INDEX(187)
#define IDX_GapScan_setPhyParams                         JT_INDEX(188)
#define IDX_GapScan_getPhyParams                         JT_INDEX(189)
#define IDX_GapScan_setParam                             JT_INDEX(190)
#define IDX_GapScan_getParam                             JT_INDEX(191)
#define IDX_GapScan_setEventMask                         JT_INDEX(192)
#define IDX_GapScan_enable                               JT_INDEX(193)
#define IDX_GapScan_disable                              JT_INDEX(194)
#define IDX_GapScan_getAdvReport                         JT_INDEX(195)
#define IDX_GapScan_discardAdvReportList                 JT_INDEX(196)
#define IDX_GapScan_PeriodicAdvCreateSync                JT_INDEX(197)
#define IDX_GapScan_PeriodicAdvCreateSyncCancel          JT_INDEX(198)
#define IDX_GapScan_PeriodicAdvTerminateSync             JT_INDEX(199)
#define IDX_GapScan_SetPeriodicAdvReceiveEnable          JT_INDEX(200)
#define IDX_GapScan_AddDeviceToPeriodicAdvList           JT_INDEX(201)
#define IDX_GapScan_RemoveDeviceFromPeriodicAdvList      JT_INDEX(202)
#define IDX_GapScan_ReadPeriodicAdvListSize              JT_INDEX(203)
#define IDX_GapScan_ClearPeriodicAdvList                 JT_INDEX(204)
// <<END_COUNT>>
// <<INSERT:#else // !( HOST_CONFIG & ( CENTRAL_CFG | OBSERVER_CFG ) )>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( HOST_CONFIG & ( CENTRAL_CFG | OBSERVER_CFG ) )>>

// <<INSERT:#if ( HOST_CONFIG & CENTRAL_CFG )>>
// <<BEGIN_COUNT>>
#define IDX_GapInit_setPhyParam                          JT_INDEX(205)
#define IDX_GapInit_getPhyParam                          JT_INDEX(206)
#define IDX_GapInit_connect                              JT_INDEX(207)
#define IDX_GapInit_connectWl                            JT_INDEX(208)
#define IDX_GapInit_cancelConnect                        JT_INDEX(209)
// <<END_COUNT>>
// <<INSERT:#else // !( HOST_CONFIG & CENTRAL_CFG )>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( HOST_CONFIG & CENTRAL_CFG )>>

// <<INSERT:#if defined(HOST_CONFIG)>>
// <<BEGIN_COUNT>>
#define IDX_GAP_TerminateAuth                            JT_INDEX(210)
#define IDX_GAP_Authenticate                             JT_INDEX(211)
#define IDX_GAP_PasskeyUpdate                            JT_INDEX(212)
#define IDX_GAP_Signable                                 JT_INDEX(213)
#define IDX_GAP_Bond                                     JT_INDEX(214)
#define IDX_GAP_SetPrivacyMode                           JT_INDEX(215)
// <<END_COUNT>>
// <<INSERT:#else // !(defined(HOST_CONFIG))>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // defined(HOST_CONFIG)>>

// <<INSERT:#if (HOST_CONFIG & PERIPHERAL_CFG)>>
// <<BEGIN_COUNT>>
#define IDX_GAP_SendSlaveSecurityRequest                 JT_INDEX(216)
// <<END_COUNT>>
// <<INSERT:#else // !(HOST_CONFIG & PERIPHERAL_CFG)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // (HOST_CONFIG & PERIPHERAL_CFG)>>

/* RTLS Services API */
#define IDX_RTLSSrv_init                                JT_INDEX(217)
#define IDX_RTLSSrv_register                            JT_INDEX(218)
#define IDX_RTLSSrv_setConnCteReceiveParams             JT_INDEX(219)
#define IDX_RTLSSrv_setConnCteTransmitParams            JT_INDEX(220)
#define IDX_RTLSSrv_setConnCteRequestEnableCmd          JT_INDEX(221)
#define IDX_RTLSSrv_setConnCteResponseEnableCmd         JT_INDEX(222)
#define IDX_RTLSSrv_readAntennaInformationCmd           JT_INDEX(223)
#define IDX_RTLSSrv_setCteSampleAccuracy                JT_INDEX(224)
#define IDX_RTLSSrv_initAntArray                        JT_INDEX(225)
#define IDX_RTLSSrv_SetCLCteTransmitParams              JT_INDEX(226)
#define IDX_RTLSSrv_CLCteTransmitEnable                 JT_INDEX(227)
#define IDX_RTLSSrv_setCLCteSamplingEnableCmd           JT_INDEX(228)

/* L2CAP API */
/*************/
// <<INSERT:#if defined(HOST_CONFIG)>>
// <<BEGIN_COUNT>>
#define IDX_L2CAP_ConnParamUpdateReq                     JT_INDEX(229)
#define IDX_L2CAP_ParseParamUpdateReq                    JT_INDEX(230)
#define IDX_L2CAP_ParseInfoReq                           JT_INDEX(231)
// <<END_COUNT>>
// <<INSERT:#else // !defined(HOST_CONFIG)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // defined(HOST_CONFIG)>>

// <<INSERT:#if defined(BLE_V41_FEATURES) && (BLE_V41_FEATURES & L2CAP_COC_CFG)>>
// <<BEGIN_COUNT>>
#define IDX_L2CAP_RegisterPsm                            JT_INDEX(232)
#define IDX_L2CAP_DeregisterPsm                          JT_INDEX(233)
#define IDX_L2CAP_PsmInfo                                JT_INDEX(234)
#define IDX_L2CAP_PsmChannels                            JT_INDEX(235)
#define IDX_L2CAP_ChannelInfo                            JT_INDEX(236)
#define IDX_L2CAP_SendSDU                                JT_INDEX(237)
#define IDX_L2CAP_ConnectReq                             JT_INDEX(238)
#define IDX_L2CAP_ConnectRsp                             JT_INDEX(239)
#define IDX_L2CAP_DisconnectReq                          JT_INDEX(240)
#define IDX_L2CAP_FlowCtrlCredit                         JT_INDEX(241)
// <<END_COUNT>>
// <<INSERT:#else // !( (BLE_V41_FEATURES) && (BLE_V41_FEATURES & L2CAP_COC_CFG) )>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( (BLE_V41_FEATURES) && (BLE_V41_FEATURES & L2CAP_COC_CFG) )>>

// <<INSERT:#if ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ))>>
// <<BEGIN_COUNT>>
#define IDX_L2CAP_RegisterFlowCtrlTask                   JT_INDEX(242)
// <<END_COUNT>>
// <<INSERT:#else // !( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )>>

// <<INSERT:#if ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES) )))>>
// <<BEGIN_COUNT>>
#define IDX_L2CAP_SetParamValue                          JT_INDEX(243)
#define IDX_L2CAP_GetParamValue                          JT_INDEX(244)
// <<END_COUNT>>
// <<INSERT:#else // !( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES))  ))>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES))  ))>>

// <<INSERT:#if ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL)  ))>>
// <<BEGIN_COUNT>>
#define IDX_L2CAP_InfoReq                                JT_INDEX(245)
// <<END_COUNT>>
// <<INSERT:#else // !( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL)  ))>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL)  ))>>

/* GATT API */
/************/
// <<INSERT:#if defined(HOST_CONFIG)>>
// <<BEGIN_COUNT>>
#define IDX_GATT_Indication                              JT_INDEX(246)
#define IDX_GATT_Notification                            JT_INDEX(247)
// <<END_COUNT>>
// <<INSERT:#else // !defined(HOST_CONFIG)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // defined(HOST_CONFIG)>>

// <<INSERT:#if ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ))>>
// <<BEGIN_COUNT>>
#define IDX_GATT_RegisterForMsgs                         JT_INDEX(248)
#define IDX_GATT_UpdateMTU                               JT_INDEX(249)
#define IDX_GATT_SendRsp                                 JT_INDEX(250)
#define IDX_GATT_SetHostToAppFlowCtrl                    JT_INDEX(251)
#define IDX_GATT_AppCompletedMsg                         JT_INDEX(252)
// <<END_COUNT>>
// <<INSERT:#else // !( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )>>

// <<INSERT:#if ( (HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG )) && (!defined(GATT_NO_CLIENT)))>>
// <<BEGIN_COUNT>>
#define IDX_GATT_InitClient                              JT_INDEX(253)
#define IDX_GATT_RegisterForInd                          JT_INDEX(254)
// <<END_COUNT>>
// <<INSERT:#else // !( (HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG )) && (!defined(GATT_NO_CLIENT)) )>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ((HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG )) && (!defined(GATT_NO_CLIENT)) )>>

// <<INSERT:#if defined(HOST_CONFIG) && !defined(GATT_NO_CLIENT)>>
// <<BEGIN_COUNT>>
#define IDX_GATT_ExchangeMTU                             JT_INDEX(255)
#define IDX_GATT_DiscAllPrimaryServices                  JT_INDEX(256)
#define IDX_GATT_DiscPrimaryServiceByUUID                JT_INDEX(257)
#define IDX_GATT_DiscAllChars                            JT_INDEX(258)
#define IDX_GATT_DiscCharsByUUID                         JT_INDEX(259)
#define IDX_GATT_DiscAllCharDescs                        JT_INDEX(260)
#define IDX_GATT_ReadCharValue                           JT_INDEX(261)
#define IDX_GATT_ReadUsingCharUUID                       JT_INDEX(262)
#define IDX_GATT_ReadLongCharValue                       JT_INDEX(263)
#define IDX_GATT_ReadMultiCharValues                     JT_INDEX(264)
#define IDX_GATT_WriteNoRsp                              JT_INDEX(265)
#define IDX_GATT_SignedWriteNoRsp                        JT_INDEX(266)
#define IDX_GATT_WriteCharValue                          JT_INDEX(267)
#define IDX_GATT_WriteLongCharValue                      JT_INDEX(268)
#define IDX_GATT_WriteLongCharDesc                       JT_INDEX(269)
#define IDX_GATT_ReliableWrites                          JT_INDEX(270)
#define IDX_GATT_ReadCharDesc                            JT_INDEX(271)
#define IDX_GATT_ReadLongCharDesc                        JT_INDEX(272)
#define IDX_GATT_WriteCharDesc                           JT_INDEX(273)
// <<END_COUNT>>
// <<INSERT:#else // !(defined(HOST_CONFIG) !defined(GATT_NO_CLIENT) )>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // (defined(HOST_CONFIG) !defined(GATT_NO_CLIENT) )>>

// <<INSERT:#if ( HOST_CONFIG & ( PERIPHERAL_CFG ))>>
// <<BEGIN_COUNT>>
#define IDX_GATT_GetNextHandle                           JT_INDEX(274)
#define IDX_GATT_ServiceNumAttrs                         JT_INDEX(275)
#define IDX_GATT_FindHandle                              JT_INDEX(276)
// <<END_COUNT>>
// <<INSERT:#else // !( HOST_CONFIG & ( PERIPHERAL_CFG ) )>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( HOST_CONFIG & ( PERIPHERAL_CFG ) )>>

// <<INSERT:#if ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES) )))>>
// <<BEGIN_COUNT>>
#define IDX_GATT_PrepareWriteReq                         JT_INDEX(277)
#define IDX_GATT_ExecuteWriteReq                         JT_INDEX(278)
#define IDX_GATT_FindUUIDRec                             JT_INDEX(279)
#define IDX_GATT_RegisterService                         JT_INDEX(280)
#define IDX_GATT_DeregisterService                       JT_INDEX(281)
#define IDX_GATT_FindIncludedServices                    JT_INDEX(282)
// <<END_COUNT>>
// <<INSERT:#else // !( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES))  ))>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES))  ))>>

// <<INSERT:#if ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) ))>>
// <<BEGIN_COUNT>>
#define IDX_GATT_RegisterForReq                          JT_INDEX(283)
// <<END_COUNT>>
// <<INSERT:#else // !( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) ))>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) ))>>

/* GATT SERVER APPLICATION API */
/*******************************/
// <<INSERT:#if defined(HOST_CONFIG)>>
// <<BEGIN_COUNT>>
#define IDX_GATTServApp_RegisterService                  JT_INDEX(284)
#define IDX_GATTServApp_AddService                       JT_INDEX(285)
#define IDX_GATTServApp_DeregisterService                JT_INDEX(286)
#define IDX_GATTServApp_SetParameter                     JT_INDEX(287)
#define IDX_GATTServApp_GetParameter                     JT_INDEX(288)
#define IDX_GATTServApp_SendCCCUpdatedEvent              JT_INDEX(289)
// <<END_COUNT>>
// <<INSERT:#else // !defined(HOST_CONFIG)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // defined(HOST_CONFIG)>>

// <<INSERT:#if !defined(GATT_NO_SERVICE_CHANGED)>>
// <<BEGIN_COUNT>>
#define IDX_GATTServApp_SendServiceChangedInd            JT_INDEX(290)
// <<END_COUNT>>
// <<INSERT:#else // !( !defined(GATT_NO_SERVICE_CHANGED) )>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( !defined(GATT_NO_SERVICE_CHANGED) )>>

// <<INSERT:#ifdef ATT_DELAYED_REQ>>
// <<BEGIN_COUNT>>
#define IDX_GATTServApp_ReadRsp                          JT_INDEX(291)
// <<END_COUNT>>
// <<INSERT:#else // !( ATT_DELAYED_REQ )>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( ATT_DELAYED_REQ )>>

// <<INSERT:#ifdef GATT_QUAL>>
// <<BEGIN_COUNT>>
#define IDX_GATTQual_AddService                          JT_INDEX(292)
// <<END_COUNT>>
// <<INSERT:#else // !( GATT_QUAL )>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( GATT_QUAL )>>

// <<INSERT:#ifdef GATT_TEST>>
// <<BEGIN_COUNT>>
#define IDX_GATTTest_AddService                          JT_INDEX(293)
// <<END_COUNT>>
// <<INSERT:#else // !( GATT_TEST )>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( GATT_TEST )>>

// <<INSERT:#if ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES) )))>>
// <<BEGIN_COUNT>>
#define IDX_GATTServApp_GetParamValue                    JT_INDEX(294)
#define IDX_GATTServApp_SetParamValue                    JT_INDEX(295)
// <<END_COUNT>>
// <<INSERT:#else // !( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES))  ))>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES))  ))>>

// <<INSERT:#if ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) ))>>
// <<BEGIN_COUNT>>
#define IDX_GATTServApp_RegisterForMsg                   JT_INDEX(296)
// <<END_COUNT>>
// <<INSERT:#else // !( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL)  ))>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL)   ))>>


/* LINK DB API */
/***************/
// <<INSERT:#if defined(HOST_CONFIG)>>
// <<BEGIN_COUNT>>
#define IDX_linkDB_NumActive                             JT_INDEX(297)
#define IDX_linkDB_GetInfo                               JT_INDEX(298)
#define IDX_linkDB_State                                 JT_INDEX(299)
#define IDX_linkDB_NumConns                              JT_INDEX(300)
// <<END_COUNT>>
// <<INSERT:#else // !defined(HOST_CONFIG)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // defined(HOST_CONFIG)>>

/* Security Manager API  */
/*************************/
// <<INSERT:#if defined(HOST_CONFIG)>>
// <<BEGIN_COUNT>>
#define IDX_SM_GetScConfirmOob                           JT_INDEX(301)
#define IDX_SM_GenerateRandBuf                           JT_INDEX(302)
#define IDX_SM_AuthenticatedPairingOnlyMode              JT_INDEX(303)
// <<END_COUNT>>
// <<INSERT:#else // !defined(HOST_CONFIG)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // defined(HOST_CONFIG)>>

// <<INSERT:#if defined(HOST_CONFIG)>>
// <<BEGIN_COUNT>>
#define IDX_SM_GetEccKeys                                JT_INDEX(304)
#define IDX_SM_GetDHKey                                  JT_INDEX(305)
#define IDX_SM_RegisterTask                              JT_INDEX(306)
// <<END_COUNT>>
// <<INSERT:#else // !(defined(HOST_CONFIG))>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // #if defined(HOST_CONFIG)>>

/* ATT API */
/***********/
// <<INSERT:#if defined(HOST_CONFIG)>>
// <<BEGIN_COUNT>>
#define IDX_ATT_ErrorRsp                                 JT_INDEX(307)
#define IDX_ATT_ReadBlobRsp                              JT_INDEX(308)
#define IDX_ATT_ExecuteWriteRsp                          JT_INDEX(309)
#define IDX_ATT_WriteRsp                                 JT_INDEX(310)
#define IDX_ATT_ReadRsp                                  JT_INDEX(311)
// <<END_COUNT>>
// <<INSERT:#else // !defined(HOST_CONFIG)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // defined(HOST_CONFIG)>>

// <<INSERT:#if ( defined(HOST_CONFIG) && defined(HCI_TL_FULL))>>
// <<BEGIN_COUNT>>
#define IDX_ATT_ParseExchangeMTUReq                      JT_INDEX(312)
#define IDX_ATT_ExchangeMTURsp                           JT_INDEX(313)
#define IDX_ATT_FindInfoRsp                              JT_INDEX(314)
#define IDX_ATT_FindByTypeValueRsp                       JT_INDEX(315)
#define IDX_ATT_ReadByTypeRsp                            JT_INDEX(316)
#define IDX_ATT_ReadMultiRsp                             JT_INDEX(317)
#define IDX_ATT_ReadByGrpTypeRsp                         JT_INDEX(318)
#define IDX_ATT_PrepareWriteRsp                          JT_INDEX(319)
// <<END_COUNT>>
// <<INSERT:#else // !defined(HOST_CONFIG)>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // defined(HOST_CONFIG)>>

// <<INSERT:#if defined(HOST_CONFIG) && !defined(GATT_NO_CLIENT)>>
// <<BEGIN_COUNT>>
#define IDX_ATT_HandleValueCfm                           JT_INDEX(320)
// <<END_COUNT>>
// <<INSERT:#else // !(defined(HOST_CONFIG) && !defined(GATT_NO_CLIENT) )>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // (defined(HOST_CONFIG) && !defined(GATT_NO_CLIENT) )>>

// <<INSERT:#if ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES) )))>>
// <<BEGIN_COUNT>>
#define IDX_ATT_SetParamValue                            JT_INDEX(321)
#define IDX_ATT_GetParamValue                            JT_INDEX(322)
// <<END_COUNT>>
// <<INSERT:#else // !( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES))  ))>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES))  ))>>


/* SNV API */
/***********/
#define IDX_osal_snv_read                                JT_INDEX(323)
#define IDX_osal_snv_write                               JT_INDEX(324)

/* UTIL API */
/************/
// <<INSERT:#if !defined(HCI_TL_NONE)>>
// <<BEGIN_COUNT>>
#define IDX_NPI_RegisterTask                             JT_INDEX(325)
// <<END_COUNT>>
// <<INSERT:#else // !( !defined(HCI_TL_NONE) )>>
// <<INSERT_LOOP:(uint32)icall_liteErrorFunction,>>
// <<INSERT:#endif // ( !defined(HCI_TL_NONE) )>>

// <<INSERT:/* this should remain last, so that for any configuration, >>
// <<INSERT:the revision needs to be read. this enable quick detection of bad alignement >>
// <<INSERT:in the table */>>
#define IDX_buildRevision                                JT_INDEX(326)
// <END TABLE - DO NOT REMOVE!>

#endif /* BLE_DISPATCH_LITE_IDX_H */
