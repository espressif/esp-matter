/******************************************************************************

 @file  rom_init.c

 @brief This file contains the externs for BLE Controller and OSAL ROM
        initialization.

 Group: WCS, BTS
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

/*******************************************************************************
 * INCLUDES
 */

#include "bcomdef.h"
#include "hal_types.h"
#include <inc/hw_types.h>
#include "rom_jt.h"
//
#include "bcomdef.h"
#include "onboard.h"
#include "osal.h"
#include "osal_memory.h"
#include "osal_pwrmgr.h"
#include "osal_bufmgr.h"
#include "osal_cbtimer.h"
#include <driverlib/vims.h>
#include <driverlib/interrupt.h>
#include "hal_assert.h"
#include "hci_tl.h"
#include "hal_trng_wrapper.h"
#include "mb.h"
#include "rf_hal.h"
#include "ll_config.h"
#include <inc/hw_sysctl.h>
#include <inc/hw_ioc.h>
#include "ll_common.h"
#include "icall.h"
#include "ecc_rom.h"
#include "linkdb.h"
#include "l2cap.h"
#include "att.h"
#include "gatt.h"
#include "gattservapp.h"
#include "gatt_uuid.h"
#include "gap.h"
#include "sm.h"
#include "gap_internal.h"
#include "linkdb_internal.h"

#include <ti/drivers/rf/RF.h>

/*******************************************************************************
 * EXTERNS
 */

// RAM address containing a pointer to the R2F flash jump table base address.
// Note: This linker imported symbol is treated as a variable by the compiler.

// ROM base address where the ROM's C runtime routine is expected.
extern uint32 ROM_BASE_ADDR;

// RAM base address of a table a pointers that are used by ROM and which are
// initialized by ROM_Init.
extern uint32 RAM_BASE_ADDR;

// Function pointers used by the Flash software (provided by RTOS).
extern ICall_Dispatcher ICall_dispatcher;
extern ICall_EnterCS    ICall_enterCriticalSection;
extern ICall_LeaveCS    ICall_leaveCriticalSection;

#if defined( ENABLE_ROM_CHECKSUM_CHECK )
// Checksum start/end address and value for ROM and Common ROM.
extern const uint32 __checksum_begin;
extern const uint32 __checksum_end;

// calculate checksum routine
extern uint16 slow_crc16( uint16 crc, uint8 *pAddr, uint32 len );
extern uint8  validChecksum( const uint32 *beginAddr, const uint32 *endAddr );
#endif // ENABLE_ROM_CHECKSUM_CHECK

// callback for radio driver events
extern void rfCallback( RF_Handle, RF_CmdHandle, RF_EventMask );
extern void rfPUpCallback( RF_Handle, RF_CmdHandle, RF_EventMask );

// Jump Table Function Externs: Needed to access internal system functions.
extern void linkDB_reportStatusChange( uint16 connectionHandle, uint8 changeType );
extern void L2CAP_SendDataPkt(void);
extern void l2capAllocChannel(void);
extern void l2capAllocConnChannel(void);
extern void l2capBuildInfoReq(void);
extern void l2capBuildParamUpdateReq(void);
extern void l2capBuildSignalHdr(void);
extern void l2capDisconnectAllChannels(void);
extern void l2capEncapSendData(void);
extern void l2capFindLocalId(void);
extern void l2capFreeChannel(void);
extern void l2capFreePendingPkt(void);
extern void l2capHandleTimerCB(void);
extern void l2capHandleRxError(void);
extern void l2capNotifyData(void);
extern void l2capNotifyEvent(void);
extern void l2capNotifySignal(void);
extern void l2capParseCmdReject(void);
extern void l2capParsePacket(void);
extern void l2capParseParamUpdateRsp(void);
extern void l2capParseSignalHdr(void);
extern void l2capProcessOSALMsg(void);
extern void l2capProcessReq(void);
extern void l2capProcessRsp(void);
extern void l2capProcessRxData(void);
extern void l2capProcessSignal(void);
extern void l2capSendCmd(void);
extern void l2capSendFCPkt(void);
extern void l2capSendPkt(void);
extern void l2capSendReq(void);
extern void l2capStartTimer(void);
extern void l2capStopTimer(void);
extern void l2capStoreFCPkt(void);
extern void attSendMsg(void);
extern void attSendRspMsg(void);
extern void gattFindServerInfo(void);
extern void gattFindService(void);
extern void gattGetPayload(void);
extern void gattGetServerStatus(void);
extern void gattNotifyEvent(void);
extern void gattParseReq(void);
extern void gattProcessExchangeMTUReq(void);
extern void gattProcessExecuteWriteReq(void);
extern void gattProcessFindByTypeValueReq(void);
extern void gattProcessFindInfoReq(void);
extern void gattProcessReadByGrpTypeReq(void);
extern void gattProcessReadByTypeReq(void);
extern void gattProcessReadReq(void);
extern void gattProcessReq(void);
extern void gattProcessRxData(void);
extern void gattProcessOSALMsg(void);
extern void gattProcessWriteReq(void);
extern void gattProcessReadMultiReq(void);
extern void gattRegisterServer(void);
extern void gattResetServerInfo(void);
extern void gattSendFlowCtrlEvt(void);
extern void gattServerHandleConnStatusCB(void);
extern void gattServerHandleTimerCB(void);
extern void gattServerNotifyTxCB(void);
extern void gattServerProcessMsgCB(void);
extern void gattServerStartTimer(void);
extern void gattServiceLastHandle(void);
extern void gattStartTimer(void);
extern void gattStopTimer(void);
extern void gattStoreServerInfo(void);
extern void gattServApp_FindServiceCBs(void);
extern void gattServApp_ProcessExchangeMTUReq(void);
extern void gattServApp_ProcessExecuteWriteReq(void);
extern void gattServApp_ProcessFindByTypeValueReq(void);
extern void gattServApp_ProcessPrepareWriteReq(void);
extern void gattServApp_ProcessReadBlobReq(void);
extern void gattServApp_ProcessReadByTypeReq(void);
extern void gattServApp_ProcessReadByGrpTypeReq(void);
extern void gattServApp_ProcessReadMultiReq(void);
extern void gattServApp_ProcessReadReq(void);
extern void gattServApp_ProcessWriteReq(void);
extern void smProcessHCIBLEEventCode(void);
extern void smProcessHCIBLEMetaEventCode(void);
extern void smProcessOSALMsg(void);
extern void generate_subkey(void);
extern void leftshift_onebit(void);
extern void padding(void);
extern void smAuthReqToUint8(void);
extern void smEncrypt(void);
extern void smEncryptLocal(void);
extern void smGenerateRandBuf(void);
extern void smStartRspTimer(void);
extern void smStopRspTimer(void);
extern void smUint8ToAuthReq(void);
extern void sm_c1new(void);
extern void sm_CMAC(void);
extern void sm_f4(void);
extern void sm_f5(void);
extern void sm_f6(void);
extern void sm_g2(void);
extern void sm_s1(void);
extern void sm_xor(void);
extern void xor_128(void);
extern void smDetermineIOCaps(void);
extern void smDetermineKeySize(void);
extern void smEndPairing(void);
extern void smFreePairingParams(void);
extern void smF5Wrapper(void);
extern void smGenerateAddrInput(void);
extern void smGenerateConfirm(void);
extern void smGenerateDHKeyCheck(void);
extern void smGeneratePairingReqRsp(void);
extern void smGenerateRandMsg(void);
extern void smGetECCKeys(void);
extern void smIncrementEccKeyRecycleCount(void);
extern void smLinkCheck(void);
extern void smNextPairingState(void);
extern void smOobSCAuthentication(void);
extern void smPairingSendEncInfo(void);
extern void smPairingSendIdentityAddrInfo(void);
extern void smPairingSendIdentityInfo(void);
extern void smPairingSendMasterID(void);
extern void smPairingSendSigningInfo(void);
extern void smProcessDataMsg(void);
extern void smProcessEncryptChange(void);
extern void smProcessPairingReq(void);
extern void smRegisterResponder(void);
extern void smSavePairInfo(void);
extern void smSaveRemotePublicKeys(void);
extern void smSendDHKeyCheck(void);
extern void smSendFailAndEnd(void);
extern void smSendFailureEvt(void);
extern void smSetPairingReqRsp(void);
extern void smSendPublicKeys(void);
extern void smStartEncryption(void);
extern void smTimedOut(void);
extern void sm_allocateSCParameters(void);
extern void sm_computeDHKey(void);
extern void sm_c1(void);
extern void smpProcessIncoming(void);
extern void smFinishPublicKeyExchange(void);
extern void smResponderAuthStageTwo(void);
extern void smpResponderProcessEncryptionInformation(void);
extern void smpResponderProcessIdentityAddrInfo(void);
extern void smpResponderProcessIdentityInfo(void);
extern void smpResponderProcessMasterID(void);
extern void smpResponderProcessPairingConfirm(void);
extern void smpResponderProcessPairingDHKeyCheck(void);
extern void smpResponderProcessPairingPublicKey(void);
extern void smpResponderProcessPairingRandom(void);
extern void smpResponderProcessPairingReq(void);
extern void smpResponderProcessSigningInfo(void);
extern void smpBuildEncInfo(void);
extern void smpBuildIdentityAddrInfo(void);
extern void smpBuildIdentityInfo(void);
extern void smpBuildMasterID(void);
extern void smpBuildPairingConfirm(void);
extern void smpBuildPairingDHKeyCheck(void);
extern void smpBuildPairingFailed(void);
extern void smpBuildPairingPublicKey(void);
extern void smpBuildPairingRandom(void);
extern void smpBuildPairingReq(void);
extern void smpBuildPairingReqRsp(void);
extern void smpBuildPairingRsp(void);
extern void smpBuildSecurityReq(void);
extern void smpBuildSigningInfo(void);
extern void smpParseEncInfo(void);
extern void smpParseIdentityAddrInfo(void);
extern void smpParseIdentityInfo(void);
extern void smpParseKeypressNoti(void);
extern void smpParseMasterID(void);
extern void smpParsePairingConfirm(void);
extern void smpParsePairingDHKeyCheck(void);
extern void smpParsePairingFailed(void);
extern void smpParsePairingPublicKey(void);
extern void smpParsePairingRandom(void);
extern void smpParsePairingReq(void);
extern void smpParseSecurityReq(void);
extern void smpParseSigningInfo(void);
extern void smSendSMMsg(void);
extern void LL_ENC_Init(void);

/*******************************************************************************
 * PROTOTYPES
 */

void ROM_Spinlock( void );

/*******************************************************************************
 * MACROS
 */

// ICall function pointers and R2F/R2R flash JT pointers for ROM
#define pICallRomDispatch      (uint32 *)(&RAM_BASE_ADDR+0)
#define pICallRomEnterCS       (uint32 *)(&RAM_BASE_ADDR+1)
#define pICallRomLeaveCS       (uint32 *)(&RAM_BASE_ADDR+2)
#define pROM_JT                (uint32 *)(&RAM_BASE_ADDR+3)

// Runtime Init code for Common ROM
#define RT_Init_ROM ((RT_Init_fp)&ROM_BASE_ADDR)

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

typedef void (*RT_Init_fp)(void);

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

#include "ll.h"
#include "ll_enc.h"
#include "ll_wl.h"
#include "ll_timer_drift.h"
#include "ll_rat.h"
#include "ll_privacy.h"

// ROM Flash Jump Table

#if defined __TI_COMPILER_VERSION || defined __TI_COMPILER_VERSION__
#pragma DATA_ALIGN(ROM_Flash_JT, 4)
#elif defined(__GNUC__) || defined(__clang__)
__attribute__ ((aligned (4)))
#else
#pragma data_alignment=4
#endif
const uint32 ROM_Flash_JT[] =
{
  (uint32)&hciTaskID,                                        // ROM_JT_OFFSET[0]
  (uint32)&hciL2capTaskID,                                   // ROM_JT_OFFSET[1]
  (uint32)&hciGapTaskID,                                     // ROM_JT_OFFSET[2]
  (uint32)&hciSmpTaskID,                                     // ROM_JT_OFFSET[3]
  (uint32)&hciTestTaskID,                                    // ROM_JT_OFFSET[4]
  (uint32)&llConfigTable,                                    // ROM_JT_OFFSET[5]
  (uint32)&llTaskList,                                       // ROM_JT_OFFSET[6]
  (uint32)&sizeInfo,                                         // ROM_JT_OFFSET[7]
#ifdef HOST_CONFIG
  (uint32)primaryServiceUUID,                                // ROM_JT_OFFSET[8]
  (uint32)secondaryServiceUUID,                              // ROM_JT_OFFSET[9]
  (uint32)serviceChangedUUID,                                // ROM_JT_OFFSET[10]
  (uint32)characterUUID,                                     // ROM_JT_OFFSET[11]
#else // !HOST_CONFIG
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
#endif // HOST_CONFIG
  (uint32)osal_mem_alloc,                                    // ROM_JT_OFFSET[12]
  (uint32)osal_mem_free,                                     // ROM_JT_OFFSET[13]
  (uint32)osal_bm_free,                                      // ROM_JT_OFFSET[14]
  (uint32)osal_memcpy,                                       // ROM_JT_OFFSET[15]
  (uint32)osal_revmemcpy,                                    // ROM_JT_OFFSET[16]
  (uint32)osal_memset,                                       // ROM_JT_OFFSET[17]
  (uint32)osal_msg_allocate,                                 // ROM_JT_OFFSET[18]
  (uint32)osal_msg_deallocate,                               // ROM_JT_OFFSET[19]
  (uint32)osal_msg_send,                                     // ROM_JT_OFFSET[20]
  (uint32)osal_msg_receive,                                  // ROM_JT_OFFSET[21]
  (uint32)osal_set_event,                                    // ROM_JT_OFFSET[22]
  (uint32)osal_bm_alloc,                                     // ROM_JT_OFFSET[23]
  (uint32)osal_bm_adjust_header,                             // ROM_JT_OFFSET[24]
  (uint32)osal_buffer_uint32,                                // ROM_JT_OFFSET[25]
  (uint32)osal_build_uint32,                                 // ROM_JT_OFFSET[26]
  (uint32)osal_isbufset,                                     // ROM_JT_OFFSET[27]
  (uint32)osal_start_timerEx,                                // ROM_JT_OFFSET[28]
  (uint32)osal_stop_timerEx,                                 // ROM_JT_OFFSET[29]
  (uint32)osal_start_reload_timer,                           // ROM_JT_OFFSET[30]
  (uint32)osal_clear_event,                                  // ROM_JT_OFFSET[31]
  (uint32)osal_memdup,                                       // ROM_JT_OFFSET[32]
  (uint32)osal_memcmp,                                       // ROM_JT_OFFSET[33]
  (uint32)osal_rand,                                         // ROM_JT_OFFSET[34]
  (uint32)osal_CbTimerStart,                                 // ROM_JT_OFFSET[35]
  (uint32)osal_CbTimerStop,                                  // ROM_JT_OFFSET[36]
  (uint32)osal_CbTimerUpdate,                                // ROM_JT_OFFSET[37]
  (uint32)halAssertHandler,                                  // ROM_JT_OFFSET[38]
  (uint32)halAssertHandlerExt,                               // ROM_JT_OFFSET[39]
  (uint32)HCI_bm_alloc,                                      // ROM_JT_OFFSET[40]
  (uint32)HCI_CommandCompleteEvent,                          // ROM_JT_OFFSET[41]
  (uint32)HCI_CommandStatusEvent,                            // ROM_JT_OFFSET[42]
  (uint32)HCI_DataBufferOverflowEvent,                       // ROM_JT_OFFSET[43]
  (uint32)HCI_DisconnectCmd,                                 // ROM_JT_OFFSET[44]
  (uint32)HCI_GAPTaskRegister,                               // ROM_JT_OFFSET[45]
  (uint32)HCI_HardwareErrorEvent,                            // ROM_JT_OFFSET[46]
  (uint32)HCI_HostBufferSizeCmd,                             // ROM_JT_OFFSET[47]
  (uint32)HCI_HostNumCompletedPktCmd,                        // ROM_JT_OFFSET[48]
  (uint32)HCI_L2CAPTaskRegister,                             // ROM_JT_OFFSET[49]
  (uint32)HCI_NumOfCompletedPacketsEvent,                    // ROM_JT_OFFSET[50]
  (uint32)HCI_ReadBDADDRCmd,                                 // ROM_JT_OFFSET[51]
  (uint32)HCI_ResetCmd,                                      // ROM_JT_OFFSET[52]
  (uint32)HCI_ReverseBytes,                                  // ROM_JT_OFFSET[53]
  (uint32)HCI_SendCommandCompleteEvent,                      // ROM_JT_OFFSET[54]
  (uint32)HCI_SendCommandStatusEvent,                        // ROM_JT_OFFSET[55]
  (uint32)HCI_SendControllerToHostEvent,                     // ROM_JT_OFFSET[56]
  (uint32)HCI_SendDataPkt,                                   // ROM_JT_OFFSET[57]
  (uint32)HCI_SetControllerToHostFlowCtrlCmd,                // ROM_JT_OFFSET[58]
  (uint32)HCI_SMPTaskRegister,                               // ROM_JT_OFFSET[59]
  (uint32)HCI_ValidConnTimeParams,                           // ROM_JT_OFFSET[60]
  (uint32)HCI_VendorSpecifcCommandCompleteEvent,             // ROM_JT_OFFSET[61]
  (uint32)hciInitEventMasks,                                 // ROM_JT_OFFSET[62]
  (uint32)HCI_LE_ConnUpdateCmd,                              // ROM_JT_OFFSET[63]
  (uint32)HCI_LE_LtkReqNegReplyCmd,                          // ROM_JT_OFFSET[64]
  (uint32)HCI_LE_LtkReqReplyCmd,                             // ROM_JT_OFFSET[65]
  (uint32)HCI_LE_ReadBufSizeCmd,                             // ROM_JT_OFFSET[66]
  (uint32)HCI_LE_RemoteConnParamReqReplyCmd,                 // ROM_JT_OFFSET[67]
  (uint32)HCI_LE_RemoteConnParamReqNegReplyCmd,              // ROM_JT_OFFSET[68]
  (uint32)HCI_LE_SetAdvDataCmd,                              // ROM_JT_OFFSET[69]
  (uint32)HCI_LE_SetAdvEnableCmd,                            // ROM_JT_OFFSET[70]
  (uint32)HCI_LE_SetAdvParamCmd,                             // ROM_JT_OFFSET[71]
  (uint32)HCI_LE_SetRandomAddressCmd,                        // ROM_JT_OFFSET[72]
  (uint32)HCI_LE_SetScanRspDataCmd,                          // ROM_JT_OFFSET[73]
  (uint32)LL_AddWhiteListDevice,                             // ROM_JT_OFFSET[74]
  (uint32)LL_AuthPayloadTimeoutExpiredCback,                 // ROM_JT_OFFSET[75]
  (uint32)LL_ChanMapUpdate,                                  // ROM_JT_OFFSET[76]
  (uint32)LL_ClearWhiteList,                                 // ROM_JT_OFFSET[77]
  (uint32)LL_ConnActive,                                     // ROM_JT_OFFSET[78]
  (uint32)LL_ConnParamUpdateCback,                           // ROM_JT_OFFSET[79]
  (uint32)LL_ConnUpdate,                                     // ROM_JT_OFFSET[80]
#if !(defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG))
  (uint32)LL_CreateConn,                                     // ROM_JT_OFFSET[81]
#else // AE_CFG
  (uint32)ROM_Spinlock,
#endif // !AE_CFG
  (uint32)LL_CreateConnCancel,                               // ROM_JT_OFFSET[82]
  (uint32)LL_CtrlToHostFlowControl,                          // ROM_JT_OFFSET[83]
  (uint32)LL_DirectTestEnd,                                  // ROM_JT_OFFSET[84]
  (uint32)LL_DirectTestEndDoneCback,                         // ROM_JT_OFFSET[85]
  (uint32)LL_DirectTestRxTest,                               // ROM_JT_OFFSET[86]
  (uint32)LL_DirectTestTxTest,                               // ROM_JT_OFFSET[87]
  (uint32)LL_Disconnect,                                     // ROM_JT_OFFSET[88]
  (uint32)LL_DisconnectCback,                                // ROM_JT_OFFSET[89]
  (uint32)LL_EncChangeCback,                                 // ROM_JT_OFFSET[90]
  (uint32)LL_EncKeyRefreshCback,                             // ROM_JT_OFFSET[91]
  (uint32)LL_EncLtkNegReply,                                 // ROM_JT_OFFSET[92]
  (uint32)LL_EncLtkReply,                                    // ROM_JT_OFFSET[93]
  (uint32)LL_EncLtkReqCback,                                 // ROM_JT_OFFSET[94]
  (uint32)LL_Encrypt,                                        // ROM_JT_OFFSET[95]
  (uint32)LL_GetNumActiveConns,                              // ROM_JT_OFFSET[96]
  (uint32)LL_RX_bm_alloc,                                    // ROM_JT_OFFSET[97]
  (uint32)LL_Rand,                                           // ROM_JT_OFFSET[98]
  (uint32)LL_RandCback,                                      // ROM_JT_OFFSET[99]
  (uint32)LL_ReadAdvChanTxPower,                             // ROM_JT_OFFSET[100]
  (uint32)LL_ReadAuthPayloadTimeout,                         // ROM_JT_OFFSET[101]
  (uint32)LL_ReadBDADDR,                                     // ROM_JT_OFFSET[102]
  (uint32)LL_ReadChanMap,                                    // ROM_JT_OFFSET[103]
  (uint32)LL_ReadLocalSupportedFeatures,                     // ROM_JT_OFFSET[104]
  (uint32)LL_ReadLocalVersionInfo,                           // ROM_JT_OFFSET[105]
  (uint32)LL_ReadRemoteUsedFeatures,                         // ROM_JT_OFFSET[106]
  (uint32)LL_ReadRemoteUsedFeaturesCompleteCback,            // ROM_JT_OFFSET[107]
  (uint32)LL_ReadRemoteVersionInfo,                          // ROM_JT_OFFSET[108]
  (uint32)LL_ReadRemoteVersionInfoCback,                     // ROM_JT_OFFSET[109]
  (uint32)LL_ReadRssi,                                       // ROM_JT_OFFSET[110]
  (uint32)LL_ReadSupportedStates,                            // ROM_JT_OFFSET[111]
  (uint32)LL_ReadTxPowerLevel,                               // ROM_JT_OFFSET[112]
  (uint32)LL_ReadWlSize,                                     // ROM_JT_OFFSET[113]
  (uint32)LL_RemoteConnParamReqReply,                        // ROM_JT_OFFSET[114]
  (uint32)LL_RemoteConnParamReqNegReply,                     // ROM_JT_OFFSET[115]
  (uint32)LL_RemoteConnParamReqCback,                        // ROM_JT_OFFSET[116]
  (uint32)LL_RemoveWhiteListDevice,                          // ROM_JT_OFFSET[117]
  (uint32)LL_Reset,                                          // ROM_JT_OFFSET[118]
  (uint32)LL_RxDataCompleteCback,                            // ROM_JT_OFFSET[119]
#if !(defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG))
  (uint32)LL_SetAdvControl,                                  // ROM_JT_OFFSET[120]
  (uint32)LL_SetAdvData,                                     // ROM_JT_OFFSET[121]
  (uint32)LL_SetAdvParam,                                    // ROM_JT_OFFSET[122]
#else // AE_CFG
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
#endif // !AE_CFG
  (uint32)LL_SetRandomAddress,                               // ROM_JT_OFFSET[123]
#if !(defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG))
  (uint32)LL_SetScanControl,                                 // ROM_JT_OFFSET[124]
  (uint32)LL_SetScanParam,                                   // ROM_JT_OFFSET[125]
  (uint32)LL_SetScanRspData,                                 // ROM_JT_OFFSET[126]
#else // AE_CFG
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
#endif // !AE_CFG
  (uint32)LL_StartEncrypt,                                   // ROM_JT_OFFSET[127]
  (uint32)LL_TX_bm_alloc,                                    // ROM_JT_OFFSET[128]
  (uint32)LL_TxData,                                         // ROM_JT_OFFSET[129]
  (uint32)LL_WriteAuthPayloadTimeout,                        // ROM_JT_OFFSET[130]
  (uint32)LL_ENC_AES128_Decrypt,                             // ROM_JT_OFFSET[131]
  (uint32)LL_ENC_AES128_Encrypt,                             // ROM_JT_OFFSET[132]
  (uint32)LL_ENC_DecryptMsg,                                 // ROM_JT_OFFSET[133]
  (uint32)LL_ENC_Encrypt,                                    // ROM_JT_OFFSET[134]
  (uint32)LL_ENC_EncryptMsg,                                 // ROM_JT_OFFSET[135]
  (uint32)LL_ENC_GenDeviceIV,                                // ROM_JT_OFFSET[136]
  (uint32)LL_ENC_GenDeviceSKD,                               // ROM_JT_OFFSET[137]
  (uint32)LL_ENC_GenerateNonce,                              // ROM_JT_OFFSET[138]
  (uint32)LL_ENC_GeneratePseudoRandNum,                      // ROM_JT_OFFSET[139]
  (uint32)LL_ENC_GenerateTrueRandNum,                        // ROM_JT_OFFSET[140]
  (uint32)LL_ENC_LoadKey,                                    // ROM_JT_OFFSET[141]
  (uint32)LL_ENC_ReverseBytes,                               // ROM_JT_OFFSET[142]
  (uint32)LL_EXT_AdvEventNotice,                             // ROM_JT_OFFSET[143]
  (uint32)LL_EXT_BuildRevision,                              // ROM_JT_OFFSET[144]
  (uint32)LL_EXT_ClkDivOnHalt,                               // ROM_JT_OFFSET[145]
  (uint32)LL_EXT_ConnEventNotice,                            // ROM_JT_OFFSET[146]
  (uint32)LL_EXT_DeclareNvUsage,                             // ROM_JT_OFFSET[147]
  (uint32)LL_EXT_Decrypt,                                    // ROM_JT_OFFSET[148]
  (uint32)LL_EXT_DisconnectImmed,                            // ROM_JT_OFFSET[149]
#if !defined(CTRL_V50_CONFIG)
  (uint32)LL_EXT_EndModemTest,                               // ROM_JT_OFFSET[150]
#else // !CTRL_V50_CONFIG
  (uint32)ROM_Spinlock,
#endif // CTRL_V50_CONFIG
  (uint32)LL_EXT_ExtendRfRange,                              // ROM_JT_OFFSET[151]
  (uint32)LL_EXT_GetConnInfo,                                // ROM_JT_OFFSET[152]
  (uint32)LL_EXT_HaltDuringRf,                               // ROM_JT_OFFSET[153]
  (uint32)LL_EXT_MapPmIoPort,                                // ROM_JT_OFFSET[154]
#if !defined(CTRL_V50_CONFIG)
  (uint32)LL_EXT_ModemHopTestTx,                             // ROM_JT_OFFSET[155]
  (uint32)LL_EXT_ModemTestRx,                                // ROM_JT_OFFSET[156]
  (uint32)LL_EXT_ModemTestTx,                                // ROM_JT_OFFSET[157]
#else // !CTRL_V50_CONFIG
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
#endif // CTRL_V50_CONFIG
  (uint32)LL_EXT_NumComplPktsLimit,                          // ROM_JT_OFFSET[158]
  (uint32)LL_EXT_OnePacketPerEvent,                          // ROM_JT_OFFSET[159]
  (uint32)LL_EXT_OverlappedProcessing,                       // ROM_JT_OFFSET[160]
  (uint32)LL_EXT_PERbyChan,                                  // ROM_JT_OFFSET[161]
  (uint32)LL_EXT_PacketErrorRate,                            // ROM_JT_OFFSET[162]
  (uint32)LL_EXT_PacketErrorRateCback,                       // ROM_JT_OFFSET[163]
  (uint32)LL_EXT_ResetSystem,                                // ROM_JT_OFFSET[164]
  (uint32)LL_EXT_SaveFreqTune,                               // ROM_JT_OFFSET[165]
  (uint32)LL_EXT_SetBDADDR,                                  // ROM_JT_OFFSET[166]
  (uint32)LL_EXT_SetFastTxResponseTime,                      // ROM_JT_OFFSET[167]
  (uint32)LL_EXT_SetFreqTune,                                // ROM_JT_OFFSET[168]
  (uint32)LL_EXT_SetLocalSupportedFeatures,                  // ROM_JT_OFFSET[169]
  (uint32)LL_EXT_SetMaxDtmTxPower,                           // ROM_JT_OFFSET[170]
  (uint32)LL_EXT_SetRxGain,                                  // ROM_JT_OFFSET[171]
  (uint32)LL_EXT_SetSCA,                                     // ROM_JT_OFFSET[172]
  (uint32)LL_EXT_SetSlaveLatencyOverride,                    // ROM_JT_OFFSET[173]
  (uint32)LL_EXT_SetTxPower,                                 // ROM_JT_OFFSET[174]
  (uint32)WL_AddEntry,                                       // ROM_JT_OFFSET[175]
  (uint32)WL_Clear,                                          // ROM_JT_OFFSET[176]
  (uint32)WL_ClearEntry,                                     // ROM_JT_OFFSET[177]
  (uint32)WL_ClearIgnoreList,                                // ROM_JT_OFFSET[178]
  (uint32)WL_FindEntry,                                      // ROM_JT_OFFSET[179]
  (uint32)WL_GetNumFreeEntries,                              // ROM_JT_OFFSET[180]
  (uint32)WL_GetSize,                                        // ROM_JT_OFFSET[181]
  (uint32)WL_Init,                                           // ROM_JT_OFFSET[182]
  (uint32)WL_RemoveEntry,                                    // ROM_JT_OFFSET[183]
  (uint32)rfCallback,                                        // ROM_JT_OFFSET[184]
  (uint32)RFHAL_InitDataQueue,                               // ROM_JT_OFFSET[185]
  (uint32)RFHAL_FreeNextTxDataEntry,                         // ROM_JT_OFFSET[186]
  (uint32)RFHAL_GetNextDataEntry,                            // ROM_JT_OFFSET[187]
  (uint32)RFHAL_GetTempDataEntry,                            // ROM_JT_OFFSET[188]
  (uint32)LL_DataLengthChangeEventCback,                     // ROM_JT_OFFSET[189]
  (uint32)LL_ReadDefaultDataLen,                             // ROM_JT_OFFSET[190]
  (uint32)LL_ReadMaxDataLen,                                 // ROM_JT_OFFSET[191]
  (uint32)LL_SetDataLen,                                     // ROM_JT_OFFSET[192]
  (uint32)LL_WriteDefaultDataLen,                            // ROM_JT_OFFSET[193]
  (uint32)LL_EXT_SetMaxDataLen,                              // ROM_JT_OFFSET[194]
  (uint32)llCheckRxBuffers,                                  // ROM_JT_OFFSET[195]
  (uint32)llCreateRxBuffer,                                  // ROM_JT_OFFSET[196]
  (uint32)llOctets2Time,                                     // ROM_JT_OFFSET[197]
  (uint32)llReplaceRxBuffers,                                // ROM_JT_OFFSET[198]
  (uint32)llSetupLenCtrlPkt,                                 // ROM_JT_OFFSET[199]
  (uint32)llTime2Octets,                                     // ROM_JT_OFFSET[200]
  (uint32)HCI_LE_GenerateDHKeyCmd,                           // ROM_JT_OFFSET[201]
  (uint32)HCI_LE_ReadLocalP256PublicKeyCmd,                  // ROM_JT_OFFSET[202]
  (uint32)LL_GenerateDHKeyCmd,                               // ROM_JT_OFFSET[203]
  (uint32)LL_GenerateDHKeyCompleteEventCback,                // ROM_JT_OFFSET[204]
  (uint32)LL_ReadLocalP256PublicKeyCmd,                      // ROM_JT_OFFSET[205]
  (uint32)LL_ReadLocalP256PublicKeyCompleteEventCback,       // ROM_JT_OFFSET[206]
  (uint32)HCI_LE_AddDeviceToResolvingListCmd,                // ROM_JT_OFFSET[207]
  (uint32)HCI_LE_SetAddressResolutionEnableCmd,              // ROM_JT_OFFSET[208]
  (uint32)HCI_LE_SetResolvablePrivateAddressTimeoutCmd,      // ROM_JT_OFFSET[209]
  (uint32)LL_AddDeviceToResolvingList,                       // ROM_JT_OFFSET[210]
  (uint32)LL_RemoveDeviceFromResolvingList,                  // ROM_JT_OFFSET[211]
  (uint32)LL_ClearResolvingList,                             // ROM_JT_OFFSET[212]
  (uint32)LL_ReadResolvingListSize,                          // ROM_JT_OFFSET[213]
  (uint32)LL_ReadPeerResolvableAddress,                      // ROM_JT_OFFSET[214]
  (uint32)LL_ReadLocalResolvableAddress,                     // ROM_JT_OFFSET[215]
  (uint32)LL_SetAddressResolutionEnable,                     // ROM_JT_OFFSET[216]
  (uint32)LL_SetResolvablePrivateAddressTimeout,             // ROM_JT_OFFSET[217]
  (uint32)LL_PRIV_Ah,                                        // ROM_JT_OFFSET[218]
  (uint32)LL_PRIV_ClearExtWL,                                // ROM_JT_OFFSET[219]
  (uint32)LL_PRIV_FindPeerInRL,                              // ROM_JT_OFFSET[220]
  (uint32)LL_PRIV_GenerateRPA,                               // ROM_JT_OFFSET[221]
  (uint32)LL_PRIV_IsRPA,                                     // ROM_JT_OFFSET[222]
  (uint32)LL_PRIV_IsZeroIRK,                                 // ROM_JT_OFFSET[223]
  (uint32)LL_PRIV_ResolveRPA,                                // ROM_JT_OFFSET[224]
  (uint32)LL_PRIV_SetupPrivacy,                              // ROM_JT_OFFSET[225]
  (uint32)LL_PRIV_SetWLSize,                                 // ROM_JT_OFFSET[226]
  (uint32)LL_PRIV_TeardownPrivacy,                           // ROM_JT_OFFSET[227]
  (uint32)llActiveTask,                                      // ROM_JT_OFFSET[228]
  (uint32)llAllocTask,                                       // ROM_JT_OFFSET[229]
  (uint32)llAddTxDataEntry,                                  // ROM_JT_OFFSET[230]
  (uint32)llAlignToNextEvent,                                // ROM_JT_OFFSET[231]
  (uint32)llAllocConnId,                                     // ROM_JT_OFFSET[232]
  (uint32)llAtLeastTwoChans,                                 // ROM_JT_OFFSET[233]
  (uint32)llCalcScaFactor,                                   // ROM_JT_OFFSET[234]
  (uint32)llCBTimer_AptoExpiredCback,                        // ROM_JT_OFFSET[235]
  (uint32)llCheckForLstoDuringSL,                            // ROM_JT_OFFSET[236]
  (uint32)llCheckWhiteListUsage,                             // ROM_JT_OFFSET[237]
  (uint32)llConnCleanup,                                     // ROM_JT_OFFSET[238]
  (uint32)llConnExists,                                      // ROM_JT_OFFSET[239]
  (uint32)llConnTerminate,                                   // ROM_JT_OFFSET[240]
  (uint32)llConvertCtrlProcTimeoutToEvent,                   // ROM_JT_OFFSET[241]
  (uint32)llConvertLstoToEvent,                              // ROM_JT_OFFSET[242]
  (uint32)llDataGetConnPtr,                                  // ROM_JT_OFFSET[243]
  (uint32)llDequeueCtrlPkt,                                  // ROM_JT_OFFSET[244]
  (uint32)llEnqueueCtrlPkt,                                  // ROM_JT_OFFSET[245]
  (uint32)llEventDelta,                                      // ROM_JT_OFFSET[246]
  (uint32)llEventInRange,                                    // ROM_JT_OFFSET[247]
  (uint32)llFragmentPDU,                                     // ROM_JT_OFFSET[248]
  (uint32)llFreeTask,                                        // ROM_JT_OFFSET[249]
  (uint32)llGenerateCRC,                                     // ROM_JT_OFFSET[250]
  (uint32)llGetCurrentTask,                                  // ROM_JT_OFFSET[251]
  (uint32)llGetNumTasks,                                     // ROM_JT_OFFSET[252]
  (uint32)llGetTaskState,                                    // ROM_JT_OFFSET[253]
#if ( CTRL_CONFIG & INIT_CFG )
  (uint32)llGenerateValidAccessAddr,                         // ROM_JT_OFFSET[254]
#else // !INIT_CFG
  (uint32)ROM_Spinlock,
#endif // INIT_CFG
  (uint32)llGetCurrentTime,                                  // ROM_JT_OFFSET[255]
  (uint32)llGetNextDataChan,                                 // ROM_JT_OFFSET[256]
  (uint32)llGetTxPower,                                      // ROM_JT_OFFSET[257]
  (uint32)llHaltRadio,                                       // ROM_JT_OFFSET[258]
  (uint32)llHardwareError,                                   // ROM_JT_OFFSET[259]
  (uint32)llInitFeatureSet,                                  // ROM_JT_OFFSET[260]
  (uint32)llMemCopySrc,                                      // ROM_JT_OFFSET[261]
  (uint32)llMoveTempTxDataEntries,                           // ROM_JT_OFFSET[262]
  (uint32)llPendingUpdateParam,                              // ROM_JT_OFFSET[263]
  (uint32)llProcessChanMap,                                  // ROM_JT_OFFSET[264]
  (uint32)llProcessTxData,                                   // ROM_JT_OFFSET[265]
  (uint32)llProcessSlaveControlProcedures,                   // ROM_JT_OFFSET[266]
  (uint32)llReleaseConnId,                                   // ROM_JT_OFFSET[267]
  (uint32)llReplaceCtrlPkt,                                  // ROM_JT_OFFSET[268]
  (uint32)llRfInit,                                          // ROM_JT_OFFSET[269]
  (uint32)llRfSetup,                                         // ROM_JT_OFFSET[270]
  (uint32)llRfStartFS,                                       // ROM_JT_OFFSET[271]
  (uint32)llScheduler,                                       // ROM_JT_OFFSET[272]
  (uint32)llScheduleTask,                                    // ROM_JT_OFFSET[273]
  (uint32)llSendReject,                                      // ROM_JT_OFFSET[274]
  (uint32)llSetNextDataChan,                                 // ROM_JT_OFFSET[275]
  (uint32)llSetTxPower,                                      // ROM_JT_OFFSET[276]
#if !(defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG))
  (uint32)llSetupAdv,                                        // ROM_JT_OFFSET[277]
#else // AE_CFG
  (uint32)ROM_Spinlock,
#endif // !AE_CFG
  (uint32)llSetupAdvDataEntryQueue,                          // ROM_JT_OFFSET[278]
  (uint32)llSetupConn,                                       // ROM_JT_OFFSET[279]
  (uint32)llSetupConnParamReq,                               // ROM_JT_OFFSET[280]
  (uint32)llSetupConnParamRsp,                               // ROM_JT_OFFSET[281]
  (uint32)llSetupConnRxDataEntryQueue,                       // ROM_JT_OFFSET[282]
  (uint32)llSetupEncRsp,                                     // ROM_JT_OFFSET[283]
  (uint32)llSetupFeatureSetReq,                              // ROM_JT_OFFSET[284]
  (uint32)llSetupFeatureSetRsp,                              // ROM_JT_OFFSET[285]
#if !(defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG))
  (uint32)llSetupInit,                                       // ROM_JT_OFFSET[286]
#else // AE_CFG
  (uint32)ROM_Spinlock,
#endif // !AE_CFG
  (uint32)llSetupInitDataEntryQueue,                         // ROM_JT_OFFSET[287]
  (uint32)llSetupNextSlaveEvent,                             // ROM_JT_OFFSET[288]
  (uint32)llSetupPauseEncRsp,                                // ROM_JT_OFFSET[289]
  (uint32)llSetupPingReq,                                    // ROM_JT_OFFSET[290]
  (uint32)llSetupPingRsp,                                    // ROM_JT_OFFSET[291]
  (uint32)llSetupRejectInd,                                  // ROM_JT_OFFSET[292]
  (uint32)llSetupRejectIndExt,                               // ROM_JT_OFFSET[293]
#if !(defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG))
  (uint32)llSetupScan,                                       // ROM_JT_OFFSET[294]
#else // AE_CFG
  (uint32)ROM_Spinlock,
#endif // !AE_CFG
  (uint32)llSetupScanDataEntryQueue,                         // ROM_JT_OFFSET[295]
  (uint32)llSetupStartEncReq,                                // ROM_JT_OFFSET[296]
  (uint32)llSetupStartEncRsp,                                // ROM_JT_OFFSET[297]
  (uint32)llSetupTermInd,                                    // ROM_JT_OFFSET[298]
  (uint32)llSetupUnknownRsp,                                 // ROM_JT_OFFSET[299]
  (uint32)llSetupVersionIndReq,                              // ROM_JT_OFFSET[300]
  (uint32)llTimeCompare,                                     // ROM_JT_OFFSET[301]
  (uint32)llTimeDelta,                                       // ROM_JT_OFFSET[302]
  (uint32)llValidateConnParams,                              // ROM_JT_OFFSET[303]
  (uint32)llVerifyConnParamReqParams,                        // ROM_JT_OFFSET[304]
  (uint32)llWriteTxData,                                     // ROM_JT_OFFSET[305]
#ifdef HOST_CONFIG
  (uint32)linkDB_Add,                                        // ROM_JT_OFFSET[306]
  (uint32)linkDB_Authen,                                     // ROM_JT_OFFSET[307]
  (uint32)linkDB_Find,                                       // ROM_JT_OFFSET[308]
  (uint32)ROM_Spinlock,
  (uint32)linkDB_Init,                                       // ROM_JT_OFFSET[309]
  (uint32)linkDB_MTU,                                        // ROM_JT_OFFSET[310]
  (uint32)linkDB_NumActive,                                  // ROM_JT_OFFSET[311]
  (uint32)linkDB_NumConns,                                   // ROM_JT_OFFSET[312]
  (uint32)linkDB_Register,                                   // ROM_JT_OFFSET[313]
  (uint32)linkDB_Remove,                                     // ROM_JT_OFFSET[314]
  (uint32)linkDB_Role,                                       // ROM_JT_OFFSET[315]
  (uint32)linkDB_reportStatusChange,                         // ROM_JT_OFFSET[316]
  (uint32)linkDB_State,                                      // ROM_JT_OFFSET[317]
  (uint32)linkDB_UpdateMTU,                                  // ROM_JT_OFFSET[318]
  (uint32)linkDB_Update,                                     // ROM_JT_OFFSET[319]
  (uint32)L2CAP_bm_alloc,                                    // ROM_JT_OFFSET[320]
  (uint32)L2CAP_BuildCmdReject,                              // ROM_JT_OFFSET[321]
  (uint32)L2CAP_BuildParamUpdateRsp,                         // ROM_JT_OFFSET[322]
  (uint32)L2CAP_CmdReject,                                   // ROM_JT_OFFSET[323]
  (uint32)L2CAP_ConnParamUpdateReq,                          // ROM_JT_OFFSET[324]
  (uint32)L2CAP_GetMTU,                                      // ROM_JT_OFFSET[325]
  (uint32)L2CAP_HostNumCompletedPkts,                        // ROM_JT_OFFSET[326]
  (uint32)L2CAP_ParseParamUpdateReq,                         // ROM_JT_OFFSET[327]
  (uint32)L2CAP_RegisterApp,                                 // ROM_JT_OFFSET[328]
  (uint32)L2CAP_SendData,                                    // ROM_JT_OFFSET[329]
  (uint32)L2CAP_SendDataPkt,                                 // ROM_JT_OFFSET[330]
  (uint32)L2CAP_SetBufSize,                                  // ROM_JT_OFFSET[331]
  (uint32)L2CAP_SetControllerToHostFlowCtrl,                 // ROM_JT_OFFSET[332]
  (uint32)l2capAllocChannel,                                 // ROM_JT_OFFSET[333]
  (uint32)l2capAllocConnChannel,                             // ROM_JT_OFFSET[334]
  (uint32)l2capBuildInfoReq,                                 // ROM_JT_OFFSET[335]
  (uint32)l2capBuildParamUpdateReq,                          // ROM_JT_OFFSET[336]
  (uint32)l2capBuildSignalHdr,                               // ROM_JT_OFFSET[337]
  (uint32)l2capDisconnectAllChannels,                        // ROM_JT_OFFSET[338]
  (uint32)l2capEncapSendData,                                // ROM_JT_OFFSET[339]
  (uint32)l2capFindLocalId,                                  // ROM_JT_OFFSET[340]
  (uint32)l2capFreeChannel,                                  // ROM_JT_OFFSET[341]
  (uint32)l2capFreePendingPkt,                               // ROM_JT_OFFSET[342]
  (uint32)l2capHandleTimerCB,                                // ROM_JT_OFFSET[343]
  (uint32)l2capHandleRxError,                                // ROM_JT_OFFSET[344]
  (uint32)l2capNotifyData,                                   // ROM_JT_OFFSET[345]
  (uint32)l2capNotifyEvent,                                  // ROM_JT_OFFSET[346]
  (uint32)l2capNotifySignal,                                 // ROM_JT_OFFSET[347]
  (uint32)l2capParseCmdReject,                               // ROM_JT_OFFSET[348]
  (uint32)l2capParsePacket,                                  // ROM_JT_OFFSET[349]
  (uint32)l2capParseParamUpdateRsp,                          // ROM_JT_OFFSET[350]
  (uint32)l2capParseSignalHdr,                               // ROM_JT_OFFSET[351]
  (uint32)l2capProcessOSALMsg,                               // ROM_JT_OFFSET[352]
  (uint32)l2capProcessReq,                                   // ROM_JT_OFFSET[353]
  (uint32)l2capProcessRsp,                                   // ROM_JT_OFFSET[354]
  (uint32)l2capProcessRxData,                                // ROM_JT_OFFSET[355]
  (uint32)l2capProcessSignal,                                // ROM_JT_OFFSET[356]
  (uint32)l2capSendCmd,                                      // ROM_JT_OFFSET[357]
  (uint32)l2capSendFCPkt,                                    // ROM_JT_OFFSET[358]
  (uint32)l2capSendPkt,                                      // ROM_JT_OFFSET[359]
  (uint32)l2capSendReq,                                      // ROM_JT_OFFSET[360]
  (uint32)l2capStartTimer,                                   // ROM_JT_OFFSET[361]
  (uint32)l2capStopTimer,                                    // ROM_JT_OFFSET[362]
  (uint32)l2capStoreFCPkt,                                   // ROM_JT_OFFSET[363]
  (uint32)ATT_BuildErrorRsp,                                 // ROM_JT_OFFSET[364]
  (uint32)ATT_BuildExchangeMTURsp,                           // ROM_JT_OFFSET[365]
  (uint32)ATT_BuildFindByTypeValueRsp,                       // ROM_JT_OFFSET[366]
  (uint32)ATT_BuildFindInfoRsp,                              // ROM_JT_OFFSET[367]
  (uint32)ATT_BuildHandleValueInd,                           // ROM_JT_OFFSET[368]
  (uint32)ATT_BuildPrepareWriteRsp,                          // ROM_JT_OFFSET[369]
  (uint32)ATT_BuildReadBlobRsp,                              // ROM_JT_OFFSET[370]
  (uint32)ATT_BuildReadByGrpTypeRsp,                         // ROM_JT_OFFSET[371]
  (uint32)ATT_BuildReadByTypeRsp,                            // ROM_JT_OFFSET[372]
  (uint32)ATT_BuildReadMultiRsp,                             // ROM_JT_OFFSET[373]
  (uint32)ATT_BuildReadRsp,                                  // ROM_JT_OFFSET[374]
  (uint32)ATT_CompareUUID,                                   // ROM_JT_OFFSET[375]
  (uint32)ATT_ConvertUUIDto128,                              // ROM_JT_OFFSET[376]
  (uint32)ATT_ErrorRsp,                                      // ROM_JT_OFFSET[377]
  (uint32)ATT_ExchangeMTURsp,                                // ROM_JT_OFFSET[378]
  (uint32)ATT_ExecuteWriteRsp,                               // ROM_JT_OFFSET[379]
  (uint32)ATT_FindByTypeValueRsp,                            // ROM_JT_OFFSET[380]
  (uint32)ATT_FindInfoRsp,                                   // ROM_JT_OFFSET[381]
  (uint32)ATT_GetMTU,                                        // ROM_JT_OFFSET[382]
  (uint32)ATT_HandleValueInd,                                // ROM_JT_OFFSET[383]
  (uint32)ATT_HandleValueNoti,                               // ROM_JT_OFFSET[384]
  (uint32)ATT_ParseExchangeMTUReq,                           // ROM_JT_OFFSET[385]
  (uint32)ATT_ParseExecuteWriteReq,                          // ROM_JT_OFFSET[386]
  (uint32)ATT_ParseFindInfoReq,                              // ROM_JT_OFFSET[387]
  (uint32)ATT_ParseFindByTypeValueReq,                       // ROM_JT_OFFSET[388]
  (uint32)ATT_ParsePacket,                                   // ROM_JT_OFFSET[389]
  (uint32)ATT_ParsePrepareWriteReq,                          // ROM_JT_OFFSET[390]
  (uint32)ATT_ParseReadBlobReq,                              // ROM_JT_OFFSET[391]
  (uint32)ATT_ParseReadByTypeReq,                            // ROM_JT_OFFSET[392]
  (uint32)ATT_ParseReadMultiReq,                             // ROM_JT_OFFSET[393]
  (uint32)ATT_ParseReadReq,                                  // ROM_JT_OFFSET[394]
  (uint32)ATT_ParseWriteReq,                                 // ROM_JT_OFFSET[395]
  (uint32)ATT_PrepareWriteRsp,                               // ROM_JT_OFFSET[396]
  (uint32)ATT_ReadBlobRsp,                                   // ROM_JT_OFFSET[397]
  (uint32)ATT_ReadByGrpTypeRsp,                              // ROM_JT_OFFSET[398]
  (uint32)ATT_ReadByTypeRsp,                                 // ROM_JT_OFFSET[399]
  (uint32)ATT_ReadMultiRsp,                                  // ROM_JT_OFFSET[400]
  (uint32)ATT_ReadRsp,                                       // ROM_JT_OFFSET[401]
  (uint32)ATT_RegisterServer,                                // ROM_JT_OFFSET[402]
  (uint32)ATT_UpdateMTU,                                     // ROM_JT_OFFSET[403]
  (uint32)ATT_WriteRsp,                                      // ROM_JT_OFFSET[404]
  (uint32)attSendMsg,                                        // ROM_JT_OFFSET[405]
  (uint32)attSendRspMsg,                                     // ROM_JT_OFFSET[406]
  (uint32)GATT_AppCompletedMsg,                              // ROM_JT_OFFSET[407]
  (uint32)GATT_bm_alloc,                                     // ROM_JT_OFFSET[408]
  (uint32)GATT_bm_free,                                      // ROM_JT_OFFSET[409]
  (uint32)GATT_FindHandle,                                   // ROM_JT_OFFSET[410]
  (uint32)GATT_FindHandleUUID,                               // ROM_JT_OFFSET[411]
  (uint32)GATT_FindNextAttr,                                 // ROM_JT_OFFSET[412]
  (uint32)GATT_InitServer,                                   // ROM_JT_OFFSET[413]
  (uint32)GATT_Indication,                                   // ROM_JT_OFFSET[414]
  (uint32)GATT_NotifyEvent,                                  // ROM_JT_OFFSET[415]
  (uint32)GATT_ServiceEncKeySize,                            // ROM_JT_OFFSET[416]
  (uint32)GATT_ServiceNumAttrs,                              // ROM_JT_OFFSET[417]
  (uint32)GATT_UpdateMTU,                                    // ROM_JT_OFFSET[418]
  (uint32)GATT_VerifyReadPermissions,                        // ROM_JT_OFFSET[419]
  (uint32)GATT_VerifyWritePermissions,                       // ROM_JT_OFFSET[420]
  (uint32)gattFindServerInfo,                                // ROM_JT_OFFSET[421]
  (uint32)gattFindService,                                   // ROM_JT_OFFSET[422]
  (uint32)gattGetPayload,                                    // ROM_JT_OFFSET[423]
  (uint32)gattGetServerStatus,                               // ROM_JT_OFFSET[424]
  (uint32)gattNotifyEvent,                                   // ROM_JT_OFFSET[425]
  (uint32)gattParseReq,                                      // ROM_JT_OFFSET[426]
  (uint32)gattProcessExchangeMTUReq,                         // ROM_JT_OFFSET[427]
  (uint32)gattProcessExecuteWriteReq,                        // ROM_JT_OFFSET[428]
  (uint32)gattProcessFindByTypeValueReq,                     // ROM_JT_OFFSET[429]
  (uint32)gattProcessFindInfoReq,                            // ROM_JT_OFFSET[430]
  (uint32)gattProcessReadByGrpTypeReq,                       // ROM_JT_OFFSET[431]
  (uint32)gattProcessReadByTypeReq,                          // ROM_JT_OFFSET[432]
  (uint32)gattProcessReadReq,                                // ROM_JT_OFFSET[433]
  (uint32)gattProcessReq,                                    // ROM_JT_OFFSET[434]
  (uint32)gattProcessRxData,                                 // ROM_JT_OFFSET[435]
  (uint32)gattProcessOSALMsg,                                // ROM_JT_OFFSET[436]
  (uint32)gattProcessWriteReq,                               // ROM_JT_OFFSET[437]
  (uint32)gattProcessReadMultiReq,                           // ROM_JT_OFFSET[438]
  (uint32)gattRegisterServer,                                // ROM_JT_OFFSET[439]
  (uint32)gattResetServerInfo,                               // ROM_JT_OFFSET[440]
  (uint32)gattSendFlowCtrlEvt,                               // ROM_JT_OFFSET[441]
  (uint32)gattServerHandleConnStatusCB,                      // ROM_JT_OFFSET[442]
  (uint32)gattServerHandleTimerCB,                           // ROM_JT_OFFSET[443]
  (uint32)gattServerNotifyTxCB,                              // ROM_JT_OFFSET[444]
  (uint32)gattServerProcessMsgCB,                            // ROM_JT_OFFSET[445]
  (uint32)gattServerStartTimer,                              // ROM_JT_OFFSET[446]
  (uint32)gattServiceLastHandle,                             // ROM_JT_OFFSET[447]
  (uint32)gattStartTimer,                                    // ROM_JT_OFFSET[448]
  (uint32)gattStopTimer,                                     // ROM_JT_OFFSET[449]
  (uint32)gattStoreServerInfo,                               // ROM_JT_OFFSET[450]
  (uint32)gattServApp_buildReadByTypeRsp,                    // ROM_JT_OFFSET[451]
#if ( ( HOST_CONFIG & CENTRAL_CFG ) || ( HOST_CONFIG & PERIPHERAL_CFG) )
  (uint32)gattServApp_ClearPrepareWriteQ,                    // ROM_JT_OFFSET[452]
  (uint32)gattServApp_EnqueuePrepareWriteReq,                // ROM_JT_OFFSET[453]
#else //
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
#endif
  (uint32)gattServApp_FindAuthorizeAttrCB,                   // ROM_JT_OFFSET[454]
  (uint32)gattServApp_FindPrepareWriteQ,                     // ROM_JT_OFFSET[455]
  (uint32)gattServApp_FindServiceCBs,                        // ROM_JT_OFFSET[456]
  (uint32)gattServApp_IsWriteLong,                           // ROM_JT_OFFSET[457]
#if ( ( HOST_CONFIG & CENTRAL_CFG ) || ( HOST_CONFIG & PERIPHERAL_CFG) )
  (uint32)gattServApp_ProcessExchangeMTUReq,                 // ROM_JT_OFFSET[458]
#else //
  (uint32)ROM_Spinlock,
#endif
  (uint32)gattServApp_ProcessExecuteWriteReq,                // ROM_JT_OFFSET[459]
  (uint32)gattServApp_ProcessFindByTypeValueReq,             // ROM_JT_OFFSET[460]
  (uint32)gattServApp_ProcessPrepareWriteReq,                // ROM_JT_OFFSET[461]
#if ( ( HOST_CONFIG & CENTRAL_CFG ) || ( HOST_CONFIG & PERIPHERAL_CFG) )
  (uint32)GATTServApp_ReadAttr,                              // ROM_JT_OFFSET[462]
#else //
  (uint32)ROM_Spinlock,
#endif
  (uint32)gattServApp_ProcessReadBlobReq,                    // ROM_JT_OFFSET[463]
  (uint32)gattServApp_ProcessReadByTypeReq,                  // ROM_JT_OFFSET[464]
  (uint32)gattServApp_ProcessReadByGrpTypeReq,               // ROM_JT_OFFSET[465]
  (uint32)gattServApp_ProcessReadMultiReq,                   // ROM_JT_OFFSET[466]
  (uint32)gattServApp_ProcessReadReq,                        // ROM_JT_OFFSET[467]
#if ( ( HOST_CONFIG & CENTRAL_CFG ) || ( HOST_CONFIG & PERIPHERAL_CFG) )
  (uint32)gattServApp_ProcessReliableWrites,                 // ROM_JT_OFFSET[468]
  (uint32)GATTServApp_WriteAttr,                             // ROM_JT_OFFSET[469]
  (uint32)gattServApp_ProcessWriteLong,                      // ROM_JT_OFFSET[470]
#else //
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
#endif
  (uint32)gattServApp_ProcessWriteReq,                       // ROM_JT_OFFSET[471]
  (uint32)ROM_Spinlock,
  (uint32)GAP_GetIRK,                                        // ROM_JT_OFFSET[472]
  (uint32)GAP_GetParamValue,                                 // ROM_JT_OFFSET[473]
  (uint32)GAP_NumActiveConnections,                          // ROM_JT_OFFSET[474]
  (uint32)GAP_PasscodeUpdate,                                // ROM_JT_OFFSET[475]
  (uint32)gapProcessBLEEvents,                               // ROM_JT_OFFSET[476]
  (uint32)gapProcessCommandStatusEvt,                        // ROM_JT_OFFSET[477]
  (uint32)gapProcessConnEvt,                                 // ROM_JT_OFFSET[478]
  (uint32)gapProcessHCICmdCompleteEvt,                       // ROM_JT_OFFSET[479]
  (uint32)gapProcessOSALMsg,                                 // ROM_JT_OFFSET[480]
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)GAP_GetDevAddress,                                 // ROM_JT_OFFSET[481]
  (uint32)gapGetDevAddressMode,                              // ROM_JT_OFFSET[482]
  (uint32)gapGetSignCounter,                                 // ROM_JT_OFFSET[483]
  (uint32)gapGetSRK,                                         // ROM_JT_OFFSET[484]
  (uint32)gapHost2CtrlOwnAddrType,                           // ROM_JT_OFFSET[485]
  (uint32)gapIncSignCounter,                                 // ROM_JT_OFFSET[486]
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)gapReadBufSizeCmdStatus,                           // ROM_JT_OFFSET[487]
  (uint32)gapSendDeviceInitDoneEvent,                        // ROM_JT_OFFSET[488]
  (uint32)disconnectNext,                                    // ROM_JT_OFFSET[489]
  (uint32)gapFreeAuthLink,                                   // ROM_JT_OFFSET[490]
  (uint32)ROM_Spinlock,
  (uint32)gapPairingCompleteCB,                              // ROM_JT_OFFSET[491]
  (uint32)gapPasskeyNeededCB,                                // ROM_JT_OFFSET[492]
  (uint32)gapProcessConnectionCompleteEvt,                   // ROM_JT_OFFSET[493]
  (uint32)gapProcessDisconnectCompleteEvt,                   // ROM_JT_OFFSET[494]
  (uint32)gapProcessRemoteConnParamReqEvt,                   // ROM_JT_OFFSET[495]
  (uint32)gapRegisterPeripheralConn,                         // ROM_JT_OFFSET[496]
  (uint32)gapSendBondCompleteEvent,                          // ROM_JT_OFFSET[497]
  (uint32)gapSendLinkUpdateEvent,                            // ROM_JT_OFFSET[498]
  (uint32)gapSendPairingReqEvent,                            // ROM_JT_OFFSET[499]
  (uint32)gapSendSignUpdateEvent,                            // ROM_JT_OFFSET[500]
  (uint32)gapUpdateConnSignCounter,                          // ROM_JT_OFFSET[501]
  (uint32)sendAuthEvent,                                     // ROM_JT_OFFSET[502]
  (uint32)sendEstLinkEvent,                                  // ROM_JT_OFFSET[503]
  (uint32)sendTerminateEvent,                                // ROM_JT_OFFSET[504]
  (uint32)gapClrState,                                       // ROM_JT_OFFSET[505]
  (uint32)gapFindADType,                                     // ROM_JT_OFFSET[506]
  (uint32)gapIsAdvertising,                                  // ROM_JT_OFFSET[507]
  (uint32)gapIsScanning,                                     // ROM_JT_OFFSET[508]
  (uint32)gapSetState,                                       // ROM_JT_OFFSET[509]
  (uint32)gapValidADType,                                    // ROM_JT_OFFSET[510]
  (uint32)gapL2capConnParamUpdateReq,                        // ROM_JT_OFFSET[511]
  (uint32)gapPeriProcessConnUpdateCmdStatus,                 // ROM_JT_OFFSET[512]
  (uint32)gapPeriProcessConnUpdateCompleteEvt,               // ROM_JT_OFFSET[513]
  (uint32)gapPeriProcessSignalEvt,                           // ROM_JT_OFFSET[514]
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
#if ( ( HOST_CONFIG & CENTRAL_CFG ) || ( HOST_CONFIG & PERIPHERAL_CFG) )
  (uint32)SM_dhKeyCB,                                        // ROM_JT_OFFSET[515]
#else //
  (uint32)ROM_Spinlock,
#endif
  (uint32)SM_GenerateAuthenSig,                              // ROM_JT_OFFSET[516]
#if ( ( HOST_CONFIG & CENTRAL_CFG ) || ( HOST_CONFIG & PERIPHERAL_CFG) )
  (uint32)SM_p256KeyCB,                                      // ROM_JT_OFFSET[517]
#else //
  (uint32)ROM_Spinlock,
#endif
  (uint32)SM_PasskeyUpdate,                                  // ROM_JT_OFFSET[518]
  (uint32)SM_StartPairing,                                   // ROM_JT_OFFSET[519]
  (uint32)SM_VerifyAuthenSig,                                // ROM_JT_OFFSET[520]
  (uint32)smProcessHCIBLEEventCode,                          // ROM_JT_OFFSET[521]
  (uint32)smProcessHCIBLEMetaEventCode,                      // ROM_JT_OFFSET[522]
  (uint32)smProcessOSALMsg,                                  // ROM_JT_OFFSET[523]
  (uint32)generate_subkey,                                   // ROM_JT_OFFSET[524]
  (uint32)leftshift_onebit,                                  // ROM_JT_OFFSET[525]
  (uint32)padding,                                           // ROM_JT_OFFSET[526]
  (uint32)smAuthReqToUint8,                                  // ROM_JT_OFFSET[527]
  (uint32)smEncrypt,                                         // ROM_JT_OFFSET[528]
  (uint32)smEncryptLocal,                                    // ROM_JT_OFFSET[529]
  (uint32)smGenerateRandBuf,                                 // ROM_JT_OFFSET[530]
  (uint32)smStartRspTimer,                                   // ROM_JT_OFFSET[531]
  (uint32)smStopRspTimer,                                    // ROM_JT_OFFSET[532]
  (uint32)smUint8ToAuthReq,                                  // ROM_JT_OFFSET[533]
  (uint32)sm_c1new,                                          // ROM_JT_OFFSET[534]
  (uint32)sm_CMAC,                                           // ROM_JT_OFFSET[535]
  (uint32)sm_f4,                                             // ROM_JT_OFFSET[536]
  (uint32)sm_f5,                                             // ROM_JT_OFFSET[537]
  (uint32)sm_f6,                                             // ROM_JT_OFFSET[538]
  (uint32)sm_g2,                                             // ROM_JT_OFFSET[539]
  (uint32)sm_s1,                                             // ROM_JT_OFFSET[540]
  (uint32)sm_xor,                                            // ROM_JT_OFFSET[541]
  (uint32)xor_128,                                           // ROM_JT_OFFSET[542]
  (uint32)smDetermineIOCaps,                                 // ROM_JT_OFFSET[543]
  (uint32)smDetermineKeySize,                                // ROM_JT_OFFSET[544]
  (uint32)smEndPairing,                                      // ROM_JT_OFFSET[545]
  (uint32)smFreePairingParams,                               // ROM_JT_OFFSET[546]
  (uint32)smF5Wrapper,                                       // ROM_JT_OFFSET[547]
  (uint32)smGenerateAddrInput,                               // ROM_JT_OFFSET[548]
  (uint32)smGenerateConfirm,                                 // ROM_JT_OFFSET[549]
  (uint32)smGenerateDHKeyCheck,                              // ROM_JT_OFFSET[550]
  (uint32)smGeneratePairingReqRsp,                           // ROM_JT_OFFSET[551]
  (uint32)smGenerateRandMsg,                                 // ROM_JT_OFFSET[552]
  (uint32)smGetECCKeys,                                      // ROM_JT_OFFSET[553]
  (uint32)smIncrementEccKeyRecycleCount,                     // ROM_JT_OFFSET[554]
  (uint32)smLinkCheck,                                       // ROM_JT_OFFSET[555]
  (uint32)smNextPairingState,                                // ROM_JT_OFFSET[556]
  (uint32)smOobSCAuthentication,                             // ROM_JT_OFFSET[557]
  (uint32)smPairingSendEncInfo,                              // ROM_JT_OFFSET[558]
  (uint32)smPairingSendIdentityAddrInfo,                     // ROM_JT_OFFSET[559]
  (uint32)smPairingSendIdentityInfo,                         // ROM_JT_OFFSET[560]
  (uint32)smPairingSendMasterID,                             // ROM_JT_OFFSET[561]
  (uint32)smPairingSendSigningInfo,                          // ROM_JT_OFFSET[562]
  (uint32)smProcessDataMsg,                                  // ROM_JT_OFFSET[563]
  (uint32)smProcessEncryptChange,                            // ROM_JT_OFFSET[564]
  (uint32)smProcessPairingReq,                               // ROM_JT_OFFSET[565]
  (uint32)smRegisterResponder,                               // ROM_JT_OFFSET[566]
  (uint32)smSavePairInfo,                                    // ROM_JT_OFFSET[567]
  (uint32)smSaveRemotePublicKeys,                            // ROM_JT_OFFSET[568]
  (uint32)smSendDHKeyCheck,                                  // ROM_JT_OFFSET[569]
  (uint32)smSendFailAndEnd,                                  // ROM_JT_OFFSET[570]
  (uint32)smSendFailureEvt,                                  // ROM_JT_OFFSET[571]
  (uint32)smSetPairingReqRsp,                                // ROM_JT_OFFSET[572]
  (uint32)smSendPublicKeys,                                  // ROM_JT_OFFSET[573]
  (uint32)smStartEncryption,                                 // ROM_JT_OFFSET[574]
  (uint32)smTimedOut,                                        // ROM_JT_OFFSET[575]
  (uint32)sm_allocateSCParameters,                           // ROM_JT_OFFSET[576]
  (uint32)sm_computeDHKey,                                   // ROM_JT_OFFSET[577]
  (uint32)sm_c1,                                             // ROM_JT_OFFSET[578]
  (uint32)smpProcessIncoming,                                // ROM_JT_OFFSET[579]
  (uint32)smFinishPublicKeyExchange,                         // ROM_JT_OFFSET[580]
  (uint32)smResponderAuthStageTwo,                           // ROM_JT_OFFSET[581]
  (uint32)smpResponderProcessEncryptionInformation,          // ROM_JT_OFFSET[582]
  (uint32)smpResponderProcessIdentityAddrInfo,               // ROM_JT_OFFSET[583]
  (uint32)smpResponderProcessIdentityInfo,                   // ROM_JT_OFFSET[584]
  (uint32)smpResponderProcessMasterID,                       // ROM_JT_OFFSET[585]
  (uint32)smpResponderProcessPairingConfirm,                 // ROM_JT_OFFSET[586]
  (uint32)smpResponderProcessPairingDHKeyCheck,              // ROM_JT_OFFSET[587]
  (uint32)smpResponderProcessPairingPublicKey,               // ROM_JT_OFFSET[588]
  (uint32)smpResponderProcessPairingRandom,                  // ROM_JT_OFFSET[589]
  (uint32)smpResponderProcessPairingReq,                     // ROM_JT_OFFSET[590]
  (uint32)smpResponderProcessSigningInfo,                    // ROM_JT_OFFSET[591]
  (uint32)smpBuildEncInfo,                                   // ROM_JT_OFFSET[592]
  (uint32)smpBuildIdentityAddrInfo,                          // ROM_JT_OFFSET[593]
  (uint32)smpBuildIdentityInfo,                              // ROM_JT_OFFSET[594]
  (uint32)smpBuildMasterID,                                  // ROM_JT_OFFSET[595]
  (uint32)smpBuildPairingConfirm,                            // ROM_JT_OFFSET[596]
  (uint32)smpBuildPairingDHKeyCheck,                         // ROM_JT_OFFSET[597]
  (uint32)smpBuildPairingFailed,                             // ROM_JT_OFFSET[598]
  (uint32)smpBuildPairingPublicKey,                          // ROM_JT_OFFSET[599]
  (uint32)smpBuildPairingRandom,                             // ROM_JT_OFFSET[600]
  (uint32)smpBuildPairingReq,                                // ROM_JT_OFFSET[601]
  (uint32)smpBuildPairingReqRsp,                             // ROM_JT_OFFSET[602]
  (uint32)smpBuildPairingRsp,                                // ROM_JT_OFFSET[603]
  (uint32)smpBuildSecurityReq,                               // ROM_JT_OFFSET[604]
  (uint32)smpBuildSigningInfo,                               // ROM_JT_OFFSET[605]
  (uint32)smpParseEncInfo,                                   // ROM_JT_OFFSET[606]
  (uint32)smpParseIdentityAddrInfo,                          // ROM_JT_OFFSET[607]
  (uint32)smpParseIdentityInfo,                              // ROM_JT_OFFSET[608]
  (uint32)smpParseKeypressNoti,                              // ROM_JT_OFFSET[609]
  (uint32)smpParseMasterID,                                  // ROM_JT_OFFSET[610]
  (uint32)smpParsePairingConfirm,                            // ROM_JT_OFFSET[611]
  (uint32)smpParsePairingDHKeyCheck,                         // ROM_JT_OFFSET[612]
  (uint32)smpParsePairingFailed,                             // ROM_JT_OFFSET[613]
  (uint32)smpParsePairingPublicKey,                          // ROM_JT_OFFSET[614]
  (uint32)smpParsePairingRandom,                             // ROM_JT_OFFSET[615]
  (uint32)smpParsePairingReq,                                // ROM_JT_OFFSET[616]
  (uint32)smpParseSecurityReq,                               // ROM_JT_OFFSET[617]
  (uint32)smpParseSigningInfo,                               // ROM_JT_OFFSET[618]
  (uint32)smSendSMMsg,                                       // ROM_JT_OFFSET[619]
#else // HOST_CONFIG
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
#endif // HOST_CONFIG
 // ROM WORKAROUND - REMOVE FOR NEXT ROM FREEZE
#if !(defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG)) && \
      defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
  (uint32)llGetAdvChanPDU,                                   // ROM_JT_OFFSET[620]
  (uint32)llExtScanSchedSetup,                               // ROM_JT_OFFSET[621]
  (uint32)llExtInitSchedSetup,                               // ROM_JT_OFFSET[622]
#else // AE_CFG || !SCAN_CFG
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
#endif // !AE_CFG && SCAN_CFG
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG) && \
    defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
  (uint32)llExtAdvSchedSetup,                                // ROM_JT_OFFSET[623]
  (uint32)llSetupRatCompare,                                 // ROM_JT_OFFSET[624]
  (uint32)LL_DisableAdvSets,                                 // ROM_JT_OFFSET[625]
#else  // !AE_CFG | !(ADV_NCONN_CFG | ADV_CONN_CFG)
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
#endif // AE_CFG & (ADV_NCONN_CFG | ADV_CONN_CFG)
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG) && \
    defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG)
  (uint32)llLinkSchedSetup,                                  // ROM_JT_OFFSET[626]
#else  // !AE_CFG | !ADV_CONN_CFG
  (uint32)ROM_Spinlock,
#endif // AE_CFG & ADV_CONN_CFG        
  (uint32)LL_ENC_Init,                                       // ROM_JT_OFFSET[627] 
  (uint32)LL_ENC_AES128_Encrypt,                             // ROM_JT_OFFSET[628] 
  (uint32)LL_ENC_AES128_Decrypt,                             // ROM_JT_OFFSET[629] 
};


/*******************************************************************************
 * @fn          BLE ROM Spinlock
 *
 * @brief       This routine is used to trap indexing errors in R2R JT.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void ROM_Spinlock( void )
{
  volatile uint8 i = 1;

  while(i);
}


/*******************************************************************************
 * @fn          BLE ROM Initialization
 *
 * @brief       This routine initializes the BLE Controller ROM software. First,
 *              the image's CRC is verified. Next, its C runtime is initialized.
 *              Then the ICall function pointers for dispatch, and enter/leave
 *              critical section are initialized. Finally, the location of the
 *              R2F and R2R flash jump tables are initialized.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void ROM_Init( void )
{
#if defined( ENABLE_ROM_CHECKSUM_CHECK )
  volatile uint8 i;

  // verify the Controller ROM image
  i = validChecksum(&__checksum_begin, &__checksum_end);

  // trap a checksum failure - what now?
  while( !i );
#endif // ENABLE_ROM_CHECKSUM_CHECK

  /*
  ** Controller ROM
  */

  // execute the ROM C runtime initialization
  // Note: This is the ROM's C Runtime initialization, not the flash's, which
  //       has already taken place.
  RT_Init_ROM();

  // initialize ICall function pointers for ROM
  // Note: The address of these functions is determined by the Application, and
  //       is passed to the Stack image via startup_entry.
  *pICallRomDispatch = (uint32)ICall_dispatcher;
  *pICallRomEnterCS  = (uint32)ICall_enterCriticalSection;
  *pICallRomLeaveCS  = (uint32)ICall_leaveCriticalSection;

  // initialize RAM pointer to ROM Flash JT for ROM code
  *pROM_JT = (uint32)ROM_Flash_JT;

  return;
}

/*******************************************************************************
 */


