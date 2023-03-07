/******************************************************************************

 @file  rom_init.c

 @brief This file contains the externs for BLE Controller and OSAL ROM
        initialization.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated

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
#include "gap_internal.h"
#include "sm.h"
#include "osal_list.h"
#include "linkdb_internal.h"
#include "gap_advertiser_internal.h"
#include "gap_advertiser.h"
#include "gap_scanner_internal.h"
#include "gap_scanner.h"
#include "gap_initiator.h"
#include "sm_internal.h"
#include <ti/drivers/rf/RF.h>
#ifdef SYSCFG
#include "ti_ble_config.h"
#endif

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
extern void rfCallback_hook( RF_Handle rfHandle, RF_CmdHandle cmdHandle, RF_EventMask events );
extern void rfCallback( RF_Handle, RF_CmdHandle, RF_EventMask );

#if defined(CTRL_V42_CONFIG) && (CTRL_V42_CONFIG & EXT_DATA_LEN_CFG)
extern void rfPUpCallback( RF_Handle, RF_CmdHandle, RF_EventMask );
#endif // EXT_DATA_LEN_CFG

// Jump Table Function Externs: Needed to access internal system functions.
extern void ll_eccInit(void);
extern void ll_GenerateDHKey(void);
extern void ll_ReadLocalP256PublicKey(void);
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
extern void l2capFindPsm(void);
extern void l2capAllocPsm(void);
extern void l2capNumActiveChannnels(void);
extern void l2capFindLocalCID(void);
extern void l2capGetCoChannelInfo(void);
extern void l2capFindRemoteId(void);
extern void l2capSendConnectRsp(void);
extern void l2capBuildDisconnectReq(void);
extern void l2capFlowCtrlCredit(void);
extern void l2capReassembleSegment(void);
extern void l2capParseConnectRsp(void);
extern void l2capNotifyChannelEstEvt(void);
extern void l2capParseDisconnectRsp(void);
extern void l2capNotifyChannelTermEvt(void);
extern void l2capProcessConnectReq(void);
extern void l2capParseDisconnectReq(void);
extern void l2capBuildDisconnectRsp(void);
extern void l2capFindRemoteCID(void);
extern void l2capDisconnectChannel(void);
extern void l2capSendNextSegment(void);
extern void l2capFindNextSegment(void);
extern void l2capSendSegment(void);
extern void l2capFreeTxSDU(void);
extern void l2capNotifyCreditEvt(void);
extern void l2capBuildConnectRsp(void);
extern void l2capNotifySendSduDoneEvt(void);
extern void l2capBuildFlowCtrlCredit(void);
extern void attSendMsg(void);
extern void attSendMsg_sPatch(void);
extern void attSendRspMsg(void);
extern void gattClientHandleConnStatusCB(void);
extern void gattClientHandleTimerCB(void);
extern void gattClientNotifyTxCB(void);
extern void gattClientProcessMsgCB(void);
extern void gattFindClientInfo(void);
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
extern void gattRegisterClient(void);
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
extern void gattClientStartTimer(void);
extern void gattProcessMultiReqs(void);
extern void gattResetClientInfo(void);
extern void gattProcessFindInfo(void);
extern void gattProcessFindByTypeValue(void);
extern void gattProcessReadByType(void);
extern void gattProcessReadLong(void);
extern void gattProcessReadByGrpType(void);
extern void gattProcessReliableWrites(void);
extern void gattProcessWriteLong(void);
extern void gattWrite(void);
extern void gattWriteLong(void);
extern void gattPrepareWriteReq(void);
extern void gattStoreClientInfo(void);
extern void gattReadByGrpType(void);
extern void gattFindByTypeValue(void);
extern void gattReadByType(void);
extern void gattFindInfo(void);
extern void gattRead(void);
extern void gattReadLong(void);
extern void gattGetClientStatus(void);
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
extern void gapScan_filterDiscMode(void);
extern void gapScan_discardAdvRptSession(void);
extern void gapScan_sendSessionEndEvt(void);
extern void gapScan_defragAdvRpt(void);
extern void gapScan_saveRptAndNotify(void);
extern void gapScan_processAdvRptCb(void);
extern void gapScan_processStartEndCb(void);
extern void gapScan_processErrorCb(void);
extern void gapScan_processSessionEndEvt(void);
extern void gapInit_connect_internal(void);
extern void gapInit_sendConnCancelledEvt(void);
extern void gapInit_initiatingEnd(void);

/*******************************************************************************
 * PROTOTYPES
 */

void ROM_Spinlock( void );
uint32 FLASH_EmptyFunc( void ) { return 0; }

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

#if defined (MAX_NUM_BLE_CONNS)
  #if (MAX_NUM_BLE_CONNS == 1)
    #define ONE_CONN
  #endif
#endif

// ROM Flash Jump Table
#if defined ( FLASH_ROM_BUILD )
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
  (uint32)&wlSize,                                           // ROM_JT_OFFSET[8]
  (uint32)&rlSize,                                           // ROM_JT_OFFSET[9]
#ifdef HOST_CONFIG
  (uint32)&gapAppTaskID,                                     // ROM_JT_OFFSET[10]
  (uint32)&gapEndAppTaskID,                                  // ROM_JT_OFFSET[11]
  (uint32)primaryServiceUUID,                                // ROM_JT_OFFSET[12]
  (uint32)secondaryServiceUUID,                              // ROM_JT_OFFSET[13]
  (uint32)serviceChangedUUID,                                // ROM_JT_OFFSET[14]
  (uint32)characterUUID,                                     // ROM_JT_OFFSET[15]
  (uint32)includeUUID,                                       // ROM_JT_OFFSET[16]
#else // !HOST_CONFIG
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
#endif // HOST_CONFIG
  (uint32)osal_list_empty,                                   // ROM_JT_OFFSET[17]
  (uint32)osal_list_put,                                     // ROM_JT_OFFSET[18]
  (uint32)osal_list_head,                                    // ROM_JT_OFFSET[19]
  (uint32)osal_list_remove,                                  // ROM_JT_OFFSET[20]
  (uint32)osal_mem_alloc,                                    // ROM_JT_OFFSET[21]
  (uint32)osal_mem_free,                                     // ROM_JT_OFFSET[22]
  (uint32)osal_bm_free,                                      // ROM_JT_OFFSET[23]
  (uint32)osal_memcpy,                                       // ROM_JT_OFFSET[24]
  (uint32)osal_revmemcpy,                                    // ROM_JT_OFFSET[25]
  (uint32)osal_memset,                                       // ROM_JT_OFFSET[26]
  (uint32)osal_msg_allocate,                                 // ROM_JT_OFFSET[27]
  (uint32)osal_msg_deallocate,                               // ROM_JT_OFFSET[28]
  (uint32)osal_msg_send,                                     // ROM_JT_OFFSET[29]
  (uint32)osal_msg_receive,                                  // ROM_JT_OFFSET[30]
  (uint32)osal_set_event,                                    // ROM_JT_OFFSET[31]
  (uint32)osal_bm_alloc,                                     // ROM_JT_OFFSET[32]
  (uint32)osal_bm_adjust_header,                             // ROM_JT_OFFSET[33]
  (uint32)osal_buffer_uint32,                                // ROM_JT_OFFSET[34]
  (uint32)osal_build_uint32,                                 // ROM_JT_OFFSET[35]
  (uint32)osal_isbufset,                                     // ROM_JT_OFFSET[36]
  (uint32)osal_start_timerEx,                                // ROM_JT_OFFSET[37]
  (uint32)osal_stop_timerEx,                                 // ROM_JT_OFFSET[38]
  (uint32)osal_start_reload_timer,                           // ROM_JT_OFFSET[39]
  (uint32)osal_clear_event,                                  // ROM_JT_OFFSET[40]
  (uint32)osal_memdup,                                       // ROM_JT_OFFSET[41]
  (uint32)osal_memcmp,                                       // ROM_JT_OFFSET[42]
  (uint32)osal_rand,                                         // ROM_JT_OFFSET[43]
  (uint32)osal_CbTimerStart,                                 // ROM_JT_OFFSET[44]
  (uint32)osal_CbTimerStop,                                  // ROM_JT_OFFSET[45]
  (uint32)osal_CbTimerUpdate,                                // ROM_JT_OFFSET[46]
  (uint32)osal_mem_allocLimited,                             // ROM_JT_OFFSET[47]
  (uint32)osal_get_timeoutEx,                                // ROM_JT_OFFSET[48]
  (uint32)halAssertHandler,                                  // ROM_JT_OFFSET[49]
  (uint32)halAssertHandlerExt,                               // ROM_JT_OFFSET[50]
  (uint32)HCI_bm_alloc,                                      // ROM_JT_OFFSET[51]
  (uint32)HCI_CommandCompleteEvent,                          // ROM_JT_OFFSET[52]
  (uint32)HCI_CommandStatusEvent,                            // ROM_JT_OFFSET[53]
  (uint32)HCI_DataBufferOverflowEvent,                       // ROM_JT_OFFSET[54]
  (uint32)HCI_DisconnectCmd,                                 // ROM_JT_OFFSET[55]
  (uint32)HCI_GAPTaskRegister,                               // ROM_JT_OFFSET[56]
  (uint32)HCI_HardwareErrorEvent,                            // ROM_JT_OFFSET[57]
  (uint32)HCI_HostBufferSizeCmd,                             // ROM_JT_OFFSET[58]
  (uint32)HCI_HostNumCompletedPktCmd,                        // ROM_JT_OFFSET[59]
  (uint32)HCI_L2CAPTaskRegister,                             // ROM_JT_OFFSET[60]
  (uint32)HCI_NumOfCompletedPacketsEvent,                    // ROM_JT_OFFSET[61]
  (uint32)HCI_ReadBDADDRCmd,                                 // ROM_JT_OFFSET[62]
  (uint32)HCI_ResetCmd,                                      // ROM_JT_OFFSET[63]
  (uint32)HCI_ReverseBytes,                                  // ROM_JT_OFFSET[64]
  (uint32)HCI_SendCommandCompleteEvent,                      // ROM_JT_OFFSET[65]
  (uint32)HCI_SendCommandStatusEvent,                        // ROM_JT_OFFSET[66]
  (uint32)HCI_SendControllerToHostEvent,                     // ROM_JT_OFFSET[67]
  (uint32)HCI_SendDataPkt,                                   // ROM_JT_OFFSET[68]
  (uint32)HCI_SetControllerToHostFlowCtrlCmd,                // ROM_JT_OFFSET[69]
  (uint32)HCI_SMPTaskRegister,                               // ROM_JT_OFFSET[70]
  (uint32)HCI_ValidConnTimeParams,                           // ROM_JT_OFFSET[71]
  (uint32)HCI_VendorSpecifcCommandCompleteEvent,             // ROM_JT_OFFSET[72]
  (uint32)hciInitEventMasks,                                 // ROM_JT_OFFSET[73]
  (uint32)HCI_LE_ConnUpdateCmd,                              // ROM_JT_OFFSET[74]
  (uint32)HCI_LE_CreateConnCancelCmd,                        // ROM_JT_OFFSET[75]
  (uint32)HCI_LE_LtkReqNegReplyCmd,                          // ROM_JT_OFFSET[76]
  (uint32)HCI_LE_LtkReqReplyCmd,                             // ROM_JT_OFFSET[77]
  (uint32)HCI_LE_ReadBufSizeCmd,                             // ROM_JT_OFFSET[78]
  (uint32)HCI_LE_RemoteConnParamReqReplyCmd,                 // ROM_JT_OFFSET[79]
  (uint32)HCI_LE_RemoteConnParamReqNegReplyCmd,              // ROM_JT_OFFSET[80]
  (uint32)HCI_LE_SetAdvDataCmd,                              // ROM_JT_OFFSET[81]
  (uint32)HCI_LE_SetAdvEnableCmd,                            // ROM_JT_OFFSET[82]
  (uint32)HCI_LE_SetAdvParamCmd,                             // ROM_JT_OFFSET[83]
  (uint32)HCI_LE_SetRandomAddressCmd,                        // ROM_JT_OFFSET[84]
  (uint32)HCI_LE_SetScanRspDataCmd,                          // ROM_JT_OFFSET[85]
  (uint32)HCI_LE_GenerateDHKeyCmd,                           // ROM_JT_OFFSET[86]
  (uint32)HCI_LE_ReadLocalP256PublicKeyCmd,                  // ROM_JT_OFFSET[87]
  (uint32)HCI_LE_SetAddressResolutionEnableCmd,              // ROM_JT_OFFSET[88]
  (uint32)HCI_LE_SetPrivacyModeCmd,                          // ROM_JT_OFFSET[89]
  (uint32)HCI_LE_SetResolvablePrivateAddressTimeoutCmd,      // ROM_JT_OFFSET[90]
  (uint32)HCI_LE_StartEncyptCmd,                             // ROM_JT_OFFSET[91]
  (uint32)LE_AE_SetData,                                     // ROM_JT_OFFSET[92]
  (uint32)LE_ClearAdvSets,                                   // ROM_JT_OFFSET[93]
#if ( CTRL_CONFIG & ( SCAN_CFG | INIT_CFG ) )
  (uint32)LE_ExtCreateConn,                                  // ROM_JT_OFFSET[94]
#else // !( CTRL_CONFIG & (SCAN_CFG | INIT_CFG) )
  (uint32)ROM_Spinlock,
#endif // ( CTRL_CONFIG & (SCAN_CFG | INIT_CFG) )
  (uint32)LE_ReadNumSupportedAdvSets,                        // ROM_JT_OFFSET[95]
  (uint32)LE_ReadRfPathCompCmd,                              // ROM_JT_OFFSET[96]
  (uint32)LE_ReadTxPowerCmd,                                 // ROM_JT_OFFSET[97]
  (uint32)LE_RemoveAdvSet,                                   // ROM_JT_OFFSET[98]
#if ( CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG) )
  (uint32)LE_SetExtAdvData_hook,                             // ROM_JT_OFFSET[99]
  (uint32)LE_SetExtAdvEnable_hook,                           // ROM_JT_OFFSET[100]
  (uint32)LE_SetExtAdvParams,                                // ROM_JT_OFFSET[101]
#else // !( CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG) )
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
#endif // ( CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG) )
#if ( CTRL_CONFIG & ( SCAN_CFG | INIT_CFG ) )
  (uint32)LE_SetExtScanEnable,                               // ROM_JT_OFFSET[102]
  (uint32)LE_SetExtScanParams,                               // ROM_JT_OFFSET[103]
#else // !( CTRL_CONFIG & (SCAN_CFG | INIT_CFG) )
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
#endif // ( CTRL_CONFIG & (SCAN_CFG | INIT_CFG) )
  (uint32)LE_SetExtScanRspData_hook,                         // ROM_JT_OFFSET[104]
  (uint32)LE_WriteRfPathCompCmd,                             // ROM_JT_OFFSET[105]
  (uint32)LL_AE_RegCBack,                                    // ROM_JT_OFFSET[106]
  (uint32)LL_AddWhiteListDevice,                             // ROM_JT_OFFSET[107]
  (uint32)LL_AuthPayloadTimeoutExpiredCback,                 // ROM_JT_OFFSET[108]
  (uint32)LL_ChanMapUpdate,                                  // ROM_JT_OFFSET[109]
  (uint32)LL_ClearWhiteList,                                 // ROM_JT_OFFSET[110]
  (uint32)LL_ConnActive,                                     // ROM_JT_OFFSET[111]
  (uint32)LL_ConnParamUpdateCback,                           // ROM_JT_OFFSET[112]
  (uint32)LL_ConnUpdate,                                     // ROM_JT_OFFSET[113]
  (uint32)LL_CountAdvSets,                                   // ROM_JT_OFFSET[114]
#if !(defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG))
  (uint32)LL_CreateConn,                                     // ROM_JT_OFFSET[115]
#else // AE_CFG
  (uint32)ROM_Spinlock,
#endif // !AE_CFG
  (uint32)LL_CreateConnCancel,                               // ROM_JT_OFFSET[116]
  (uint32)LL_CtrlToHostFlowControl,                          // ROM_JT_OFFSET[117]
  (uint32)LL_DirectTestEnd,                                  // ROM_JT_OFFSET[118]
  (uint32)LL_DirectTestEndDoneCback,                         // ROM_JT_OFFSET[119]
#if (defined(HCI_TL_FULL) || defined(PTM_MODE))
  (uint32)LL_DirectTestRxTest,                               // ROM_JT_OFFSET[120]
  (uint32)LL_DirectTestTxTest,                               // ROM_JT_OFFSET[121]
#else // (defined(HCI_TL_FULL) || defined(PTM_MODE))
  (uint32)ROM_Spinlock,                                      // ROM_JT_OFFSET[120]
  (uint32)ROM_Spinlock,                                      // ROM_JT_OFFSET[121]
#endif // (defined(HCI_TL_FULL) || defined(PTM_MODE))
  (uint32)LL_DisableAdvSets,                                 // ROM_JT_OFFSET[122]
  (uint32)LL_Disconnect,                                     // ROM_JT_OFFSET[123]
  (uint32)LL_DisconnectCback,                                // ROM_JT_OFFSET[124]
  (uint32)LL_EncChangeCback,                                 // ROM_JT_OFFSET[125]
  (uint32)LL_EncKeyRefreshCback,                             // ROM_JT_OFFSET[126]
  (uint32)LL_EncLtkNegReply,                                 // ROM_JT_OFFSET[127]
  (uint32)LL_EncLtkReply,                                    // ROM_JT_OFFSET[128]
  (uint32)LL_EncLtkReqCback,                                 // ROM_JT_OFFSET[129]
  (uint32)LL_Encrypt,                                        // ROM_JT_OFFSET[130]
#if (defined(HCI_TL_FULL) || defined(PTM_MODE))
  (uint32)LL_EnhancedRxTest,                                 // ROM_JT_OFFSET[131]
  (uint32)LL_EnhancedTxTest,                                 // ROM_JT_OFFSET[132]
#else // (defined(HCI_TL_FULL) || defined(PTM_MODE))
  (uint32)ROM_Spinlock,                                      // ROM_JT_OFFSET[131]
  (uint32)ROM_Spinlock,                                      // ROM_JT_OFFSET[132]
#endif // (defined(HCI_TL_FULL) || defined(PTM_MODE))
  (uint32)LL_GetAdvSet,                                      // ROM_JT_OFFSET[133]
  (uint32)LL_GetNumActiveConns,                              // ROM_JT_OFFSET[134]
  (uint32)LL_PhyUpdateCompleteEventCback,                    // ROM_JT_OFFSET[135]
  (uint32)LL_RX_bm_alloc,                                    // ROM_JT_OFFSET[136]
  (uint32)LL_Rand,                                           // ROM_JT_OFFSET[137]
  (uint32)LL_RandCback,                                      // ROM_JT_OFFSET[138]
  (uint32)LL_ReadAdvChanTxPower,                             // ROM_JT_OFFSET[139]
  (uint32)LL_ReadAuthPayloadTimeout,                         // ROM_JT_OFFSET[140]
  (uint32)LL_ReadBDADDR,                                     // ROM_JT_OFFSET[141]
  (uint32)LL_ReadChanMap,                                    // ROM_JT_OFFSET[142]
  (uint32)LL_ReadLocalSupportedFeatures,                     // ROM_JT_OFFSET[143]
  (uint32)LL_ReadLocalVersionInfo,                           // ROM_JT_OFFSET[144]
  (uint32)LL_ReadPhy,                                        // ROM_JT_OFFSET[145]
  (uint32)LL_ReadRemoteUsedFeatures,                         // ROM_JT_OFFSET[146]
  (uint32)LL_ReadRemoteUsedFeaturesCompleteCback,            // ROM_JT_OFFSET[147]
  (uint32)LL_ReadRemoteVersionInfo,                          // ROM_JT_OFFSET[148]
  (uint32)LL_ReadRemoteVersionInfoCback,                     // ROM_JT_OFFSET[149]
  (uint32)LL_ReadRssi,                                       // ROM_JT_OFFSET[150]
  (uint32)LL_ReadSupportedStates,                            // ROM_JT_OFFSET[151]
  (uint32)LL_ReadTxPowerLevel,                               // ROM_JT_OFFSET[152]
  (uint32)LL_ReadWlSize,                                     // ROM_JT_OFFSET[153]
  (uint32)LL_RemoteConnParamReqReply,                        // ROM_JT_OFFSET[154]
  (uint32)LL_RemoteConnParamReqNegReply,                     // ROM_JT_OFFSET[155]
  (uint32)LL_RemoteConnParamReqCback,                        // ROM_JT_OFFSET[156]
  (uint32)LL_RemoveWhiteListDevice,                          // ROM_JT_OFFSET[157]
  (uint32)LL_Reset,                                          // ROM_JT_OFFSET[158]
#if (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  (uint32)LL_RxDataCompleteCback,                            // ROM_JT_OFFSET[159]
#else // !(ADV_CONN_CFG | INIT_CFG)
  (uint32)ROM_Spinlock,
#endif // (ADV_CONN_CFG | INIT_CFG)
#if !(defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG))
  (uint32)LL_SetAdvControl,                                  // ROM_JT_OFFSET[160]
  (uint32)LL_SetAdvData,                                     // ROM_JT_OFFSET[161]
  (uint32)LL_SetAdvParam,                                    // ROM_JT_OFFSET[162]
#else // AE_CFG
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
#endif // !AE_CFG
  (uint32)LL_SetDefaultPhy,                                  // ROM_JT_OFFSET[163]
  (uint32)LL_SetPhy,                                         // ROM_JT_OFFSET[164]
  (uint32)LL_SetPrivacyMode,                                 // ROM_JT_OFFSET[165]
  (uint32)LL_SetRandomAddress,                               // ROM_JT_OFFSET[166]
#if !(defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG))
  (uint32)LL_SetScanControl,                                 // ROM_JT_OFFSET[167]
  (uint32)LL_SetScanParam,                                   // ROM_JT_OFFSET[168]
  (uint32)LL_SetScanRspData,                                 // ROM_JT_OFFSET[169]
#else // AE_CFG
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
#endif // !AE_CFG
  (uint32)LL_StartEncrypt,                                   // ROM_JT_OFFSET[170]
  (uint32)LL_TX_bm_alloc,                                    // ROM_JT_OFFSET[171]
  (uint32)LL_TxData,                                         // ROM_JT_OFFSET[172]
  (uint32)LL_WriteAuthPayloadTimeout,                        // ROM_JT_OFFSET[173]
  (uint32)LL_ENC_AES128_Decrypt,                             // ROM_JT_OFFSET[174]
  (uint32)LL_ENC_AES128_Encrypt,                             // ROM_JT_OFFSET[175]
  (uint32)LL_ENC_DecryptMsg,                                 // ROM_JT_OFFSET[176]
  (uint32)LL_ENC_Encrypt,                                    // ROM_JT_OFFSET[177]
  (uint32)LL_ENC_EncryptMsg,                                 // ROM_JT_OFFSET[178]
  (uint32)LL_ENC_GenDeviceIV,                                // ROM_JT_OFFSET[179]
  (uint32)LL_ENC_GenDeviceSKD,                               // ROM_JT_OFFSET[180]
  (uint32)LL_ENC_GenerateNonce,                              // ROM_JT_OFFSET[181]
  (uint32)LL_ENC_GeneratePseudoRandNum,                      // ROM_JT_OFFSET[182]
  (uint32)LL_ENC_GenerateTrueRandNum,                        // ROM_JT_OFFSET[183]
  (uint32)LL_ENC_LoadKey,                                    // ROM_JT_OFFSET[184]
  (uint32)LL_ENC_ReverseBytes,                               // ROM_JT_OFFSET[185]
  (uint32)LL_EXT_BuildRevision,                              // ROM_JT_OFFSET[186]
  (uint32)LL_EXT_ClkDivOnHalt,                               // ROM_JT_OFFSET[187]
  (uint32)LL_EXT_DeclareNvUsage,                             // ROM_JT_OFFSET[188]
  (uint32)LL_EXT_Decrypt,                                    // ROM_JT_OFFSET[189]
  (uint32)LL_EXT_DisconnectImmed,                            // ROM_JT_OFFSET[190]
  (uint32)LL_EXT_EndModemTest,                               // ROM_JT_OFFSET[191]
#if (defined(HCI_TL_FULL) || defined(PTM_MODE))
  (uint32)LL_EXT_EnhancedModemHopTestTx,                     // ROM_JT_OFFSET[192]
  (uint32)LL_EXT_EnhancedModemTestRx,                        // ROM_JT_OFFSET[193]
  (uint32)LL_EXT_EnhancedModemTestTx,                        // ROM_JT_OFFSET[194]
#else // (defined(HCI_TL_FULL) || defined(PTM_MODE))
  (uint32)ROM_Spinlock,                                      // ROM_JT_OFFSET[192]
  (uint32)ROM_Spinlock,                                      // ROM_JT_OFFSET[193]
  (uint32)ROM_Spinlock,                                      // ROM_JT_OFFSET[194]
#endif // (defined(HCI_TL_FULL) || defined(PTM_MODE))
  (uint32)LL_EXT_ExtendRfRange,                              // ROM_JT_OFFSET[195]
  (uint32)LL_EXT_GetConnInfo,                                // ROM_JT_OFFSET[196]
  (uint32)LL_EXT_HaltDuringRf,                               // ROM_JT_OFFSET[197]
  (uint32)LL_EXT_MapPmIoPort,                                // ROM_JT_OFFSET[198]
#if (defined(HCI_TL_FULL) || defined(PTM_MODE))
  (uint32)LL_EXT_ModemHopTestTx,                             // ROM_JT_OFFSET[199]
  (uint32)LL_EXT_ModemTestRx,                                // ROM_JT_OFFSET[200]
  (uint32)LL_EXT_ModemTestTx,                                // ROM_JT_OFFSET[201]
#else // (defined(HCI_TL_FULL) || defined(PTM_MODE))
  (uint32)ROM_Spinlock,                                      // ROM_JT_OFFSET[199]
  (uint32)ROM_Spinlock,                                      // ROM_JT_OFFSET[200]
  (uint32)ROM_Spinlock,                                      // ROM_JT_OFFSET[201]
#endif // (defined(HCI_TL_FULL) || defined(PTM_MODE))
  (uint32)LL_EXT_NumComplPktsLimit,                          // ROM_JT_OFFSET[202]
  (uint32)LL_EXT_OnePacketPerEvent,                          // ROM_JT_OFFSET[203]
  (uint32)LL_EXT_OverlappedProcessing,                       // ROM_JT_OFFSET[204]
  (uint32)LL_EXT_PERbyChan,                                  // ROM_JT_OFFSET[205]
  (uint32)LL_EXT_PacketErrorRate,                            // ROM_JT_OFFSET[206]
  (uint32)LL_EXT_PacketErrorRateCback,                       // ROM_JT_OFFSET[207]
  (uint32)LL_EXT_ReadRandomAddress,                          // ROM_JT_OFFSET[208]
  (uint32)LL_EXT_ResetSystem,                                // ROM_JT_OFFSET[209]
  (uint32)LL_EXT_SaveFreqTune,                               // ROM_JT_OFFSET[210]
  (uint32)LL_EXT_SetBDADDR,                                  // ROM_JT_OFFSET[211]
  (uint32)LL_EXT_SetDtmTxPktCnt,                             // ROM_JT_OFFSET[212]
  (uint32)LL_EXT_SetFastTxResponseTime,                      // ROM_JT_OFFSET[213]
  (uint32)LL_EXT_SetFreqTune,                                // ROM_JT_OFFSET[214]
  (uint32)LL_EXT_SetLocalSupportedFeatures,                  // ROM_JT_OFFSET[215]
  (uint32)LL_EXT_SetMaxDtmTxPower,                           // ROM_JT_OFFSET[216]
  (uint32)LL_EXT_SetRxGain,                                  // ROM_JT_OFFSET[217]
  (uint32)LL_EXT_SetSCA,                                     // ROM_JT_OFFSET[218]
  (uint32)LL_EXT_SetSlaveLatencyOverride,                    // ROM_JT_OFFSET[219]
  (uint32)LL_EXT_SetTxPower,                                 // ROM_JT_OFFSET[220]
  (uint32)WL_AddEntry,                                       // ROM_JT_OFFSET[221]
  (uint32)WL_Clear,                                          // ROM_JT_OFFSET[222]
  (uint32)WL_ClearEntry,                                     // ROM_JT_OFFSET[223]
  (uint32)WL_ClearIgnoreList,                                // ROM_JT_OFFSET[224]
  (uint32)WL_FindEntry,                                      // ROM_JT_OFFSET[225]
  (uint32)WL_GetNumFreeEntries,                              // ROM_JT_OFFSET[226]
  (uint32)WL_GetSize,                                        // ROM_JT_OFFSET[227]
  (uint32)WL_Init,                                           // ROM_JT_OFFSET[228]
  (uint32)WL_RemoveEntry,                                    // ROM_JT_OFFSET[229]
  (uint32)WL_SetWlIgnore,                                    // ROM_JT_OFFSET[230]
#ifdef ONE_BLE_LIB_SIZE_OPTIMIZATION
  (uint32)rfCallback_hook,                                   // ROM_JT_OFFSET[231]
#else  // ONE_BLE_LIB_SIZE_OPTIMIZATION
  (uint32)rfCallback,                                        // ROM_JT_OFFSET[231]
#endif // ONE_BLE_LIB_SIZE_OPTIMIZATION
  (uint32)RFHAL_InitDataQueue,                               // ROM_JT_OFFSET[232]
  (uint32)RFHAL_FreeNextTxDataEntry,                         // ROM_JT_OFFSET[233]
  (uint32)RFHAL_GetNextDataEntry,                            // ROM_JT_OFFSET[234]
  (uint32)RFHAL_GetTempDataEntry,                            // ROM_JT_OFFSET[235]
  (uint32)RFHAL_NextDataEntryDone,                           // ROM_JT_OFFSET[236]
  (uint32)LL_DataLengthChangeEventCback,                     // ROM_JT_OFFSET[237]
  (uint32)LL_ReadDefaultDataLen,                             // ROM_JT_OFFSET[238]
  (uint32)LL_ReadMaxDataLen,                                 // ROM_JT_OFFSET[239]
  (uint32)LL_SetDataLen,                                     // ROM_JT_OFFSET[240]
  (uint32)LL_WriteDefaultDataLen,                            // ROM_JT_OFFSET[241]
  (uint32)LL_EXT_SetMaxDataLen,                              // ROM_JT_OFFSET[242]
  (uint32)llCheckCBack,                                      // ROM_JT_OFFSET[243]
  (uint32)llCheckRxBuffers,                                  // ROM_JT_OFFSET[244]
  (uint32)llCreateRxBuffer,                                  // ROM_JT_OFFSET[245]
  (uint32)llOctets2Time,                                     // ROM_JT_OFFSET[246]
  (uint32)llReplaceRxBuffers,                                // ROM_JT_OFFSET[247]
  (uint32)llRegisterConnEvtCallback,                         // ROM_JT_OFFSET[248]
  (uint32)llSetupLenCtrlPkt,                                 // ROM_JT_OFFSET[249]
  (uint32)llTime2Octets,                                     // ROM_JT_OFFSET[250]
  (uint32)llTxPwrPoutLU,                                     // ROM_JT_OFFSET[251]
  (uint32)LL_GenerateDHKeyCmd,                               // ROM_JT_OFFSET[252]
  (uint32)LL_GenerateDHKeyCompleteEventCback,                // ROM_JT_OFFSET[253]
  (uint32)LL_ReadLocalP256PublicKeyCmd,                      // ROM_JT_OFFSET[254]
  (uint32)LL_ReadLocalP256PublicKeyCompleteEventCback,       // ROM_JT_OFFSET[255]
  (uint32)ll_eccInit,                                        // ROM_JT_OFFSET[256]
  (uint32)ll_GenerateDHKey,                                  // ROM_JT_OFFSET[257]
  (uint32)ll_ReadLocalP256PublicKey,                         // ROM_JT_OFFSET[258]
  (uint32)LL_AddDeviceToResolvingList,                       // ROM_JT_OFFSET[259]
  (uint32)LL_RemoveDeviceFromResolvingList,                  // ROM_JT_OFFSET[260]
  (uint32)LL_ClearResolvingList,                             // ROM_JT_OFFSET[261]
  (uint32)LL_ReadResolvingListSize,                          // ROM_JT_OFFSET[262]
  (uint32)LL_ReadPeerResolvableAddress,                      // ROM_JT_OFFSET[263]
  (uint32)LL_ReadLocalResolvableAddress,                     // ROM_JT_OFFSET[264]
  (uint32)LL_SetAddressResolutionEnable,                     // ROM_JT_OFFSET[265]
  (uint32)LL_SetResolvablePrivateAddressTimeout,             // ROM_JT_OFFSET[266]
  (uint32)LL_PRIV_AddExtWLEntry,                             // ROM_JT_OFFSET[267]
  (uint32)LL_PRIV_Ah,                                        // ROM_JT_OFFSET[268]
  (uint32)LL_PRIV_CheckRLPeerId,                             // ROM_JT_OFFSET[269]
  (uint32)LL_PRIV_CheckRLPeerIdEntry,                        // ROM_JT_OFFSET[270]
  (uint32)LL_PRIV_ClearAllPrivIgn,                           // ROM_JT_OFFSET[271]
  (uint32)LL_PRIV_ClearExtWL,                                // ROM_JT_OFFSET[272]
  (uint32)LL_PRIV_FindExtWLEntry,                            // ROM_JT_OFFSET[273]
  (uint32)LL_PRIV_FindPeerInRL,                              // ROM_JT_OFFSET[274]
  (uint32)LL_PRIV_GenerateRPA,                               // ROM_JT_OFFSET[275]
  (uint32)LL_PRIV_IsIDA,                                     // ROM_JT_OFFSET[276]
  (uint32)LL_PRIV_IsRPA,                                     // ROM_JT_OFFSET[277]
  (uint32)LL_PRIV_IsZeroIRK,                                 // ROM_JT_OFFSET[278]
  (uint32)LL_PRIV_IsResolvable,                              // ROM_JT_OFFSET[279]
  (uint32)LL_PRIV_ResolveRPA,                                // ROM_JT_OFFSET[280]
  (uint32)LL_PRIV_SetupPrivacy,                              // ROM_JT_OFFSET[281]
  (uint32)LL_PRIV_SetWLSize,                                 // ROM_JT_OFFSET[282]
  (uint32)LL_PRIV_TeardownPrivacy,                           // ROM_JT_OFFSET[283]
  (uint32)llActiveTask,                                      // ROM_JT_OFFSET[284]
  (uint32)llAllocTask,                                       // ROM_JT_OFFSET[285]
  (uint32)llAddTxDataEntry,                                  // ROM_JT_OFFSET[286]
  (uint32)llAlignToNextEvent,                                // ROM_JT_OFFSET[287]
  (uint32)llAllocConnId,                                     // ROM_JT_OFFSET[288]
#if ( CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG) )
  (uint32)llAllocRfMem,                                      // ROM_JT_OFFSET[289]
#else // !(ADV_NCONN_CFG | ADV_CONN_CFG)
  (uint32)ROM_Spinlock,
#endif // (ADV_NCONN_CFG | ADV_CONN_CFG)
  (uint32)llAtLeastTwoChans,                                 // ROM_JT_OFFSET[290]
  (uint32)llCalcScaFactor,                                   // ROM_JT_OFFSET[291]
  (uint32)llCBTimer_AptoExpiredCback,                        // ROM_JT_OFFSET[292]
  (uint32)llCheckForLstoDuringSL,                            // ROM_JT_OFFSET[293]
  (uint32)llCheckWhiteListUsage,                             // ROM_JT_OFFSET[294]
  (uint32)llClearRatCompare,                                 // ROM_JT_OFFSET[295]
  (uint32)llConnCleanup,                                     // ROM_JT_OFFSET[296]
  (uint32)llConnExists,                                      // ROM_JT_OFFSET[297]
  (uint32)llConnTerminate,                                   // ROM_JT_OFFSET[298]
  (uint32)llConvertCtrlProcTimeoutToEvent,                   // ROM_JT_OFFSET[299]
  (uint32)llConvertLstoToEvent,                              // ROM_JT_OFFSET[300]
  (uint32)llDataGetConnPtr,                                  // ROM_JT_OFFSET[301]
  (uint32)llDequeueCtrlPkt,                                  // ROM_JT_OFFSET[302]
#if ( CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG) )
  (uint32)llExtAdvSchedSetup,                                // ROM_JT_OFFSET[303]
#else // !(ADV_NCONN_CFG | ADV_CONN_CFG) 
  (uint32)ROM_Spinlock,
#endif // (ADV_NCONN_CFG | ADV_CONN_CFG) 
  (uint32)llEndExtAdvTask,                                   // ROM_JT_OFFSET[304]
  (uint32)llEndExtInitTask,                                  // ROM_JT_OFFSET[305]
  (uint32)llEndExtScanTask,                                  // ROM_JT_OFFSET[306]
  (uint32)llEnqueueCtrlPkt,                                  // ROM_JT_OFFSET[307]
  (uint32)llEqAlreadyValidAddr,                              // ROM_JT_OFFSET[308]
  (uint32)llEqSynchWord,                                     // ROM_JT_OFFSET[309]
  (uint32)llEqualBytes,                                      // ROM_JT_OFFSET[310]
  (uint32)llEventDelta,                                      // ROM_JT_OFFSET[311]
  (uint32)llEventInRange,                                    // ROM_JT_OFFSET[312]
  (uint32)llExtAdvCBack,                                     // ROM_JT_OFFSET[313]
  (uint32)llExtInit_PostProcess,                             // ROM_JT_OFFSET[314]
#if ( CTRL_CONFIG & INIT_CFG )
  (uint32)llExtInitSchedSetup,                               // ROM_JT_OFFSET[315]
#else // !INIT_CFG
  (uint32)ROM_Spinlock,
#endif // INIT_CFG
  (uint32)llExtScan_PostProcess,                             // ROM_JT_OFFSET[316]
#if ( CTRL_CONFIG & SCAN_CFG )
  (uint32)llExtScanSchedSetup,                               // ROM_JT_OFFSET[317]
#else // !SCAN_CFG
  (uint32)ROM_Spinlock,
#endif // SCAN_CFG
  (uint32)llFindNextAdvSet,                                  // ROM_JT_OFFSET[318]
  (uint32)llFindNextSecTask,                                 // ROM_JT_OFFSET[319]
  (uint32)llFindStartType_hook,                              // ROM_JT_OFFSET[320]
  (uint32)llFragmentPDU,                                     // ROM_JT_OFFSET[321]
  (uint32)llFreeTask,                                        // ROM_JT_OFFSET[322]
  (uint32)llGenerateCRC,                                     // ROM_JT_OFFSET[323]
  (uint32)llGetActiveTasks,                                  // ROM_JT_OFFSET[324]
  (uint32)llGetCurrentTask,                                  // ROM_JT_OFFSET[325]
  (uint32)llGetExtHdrLen,                                    // ROM_JT_OFFSET[326]
#if (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  (uint32)llGetNextConn_hook,                                // ROM_JT_OFFSET[327]
#else // !(ADV_CONN_CFG | INIT_CFG)
  (uint32)ROM_Spinlock,
#endif // (ADV_CONN_CFG | INIT_CFG)
  (uint32)llGetNumTasks,                                     // ROM_JT_OFFSET[328]
  (uint32)llGetSlowestPhy,                                   // ROM_JT_OFFSET[329]
  (uint32)llGetTaskState,                                    // ROM_JT_OFFSET[330]
#if ( CTRL_CONFIG & INIT_CFG )
  (uint32)llGenerateValidAccessAddr,                         // ROM_JT_OFFSET[331]
#else // !INIT_CFG
  (uint32)ROM_Spinlock,
#endif // INIT_CFG
  (uint32)llGetCurrentTime,                                  // ROM_JT_OFFSET[332]
  (uint32)llGetNextDataChan,                                 // ROM_JT_OFFSET[333]
  (uint32)llGetNextDataChanAlgo1,                            // ROM_JT_OFFSET[334]
  (uint32)llGetNextDataChanAlgo2,                            // ROM_JT_OFFSET[335]
  (uint32)llGetTxPower,                                      // ROM_JT_OFFSET[336]
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & PHY_LR_CFG)
  (uint32)llGtElevenTransitionsInLsh,                        // ROM_JT_OFFSET[337]
#else // AE_CFG
  (uint32)ROM_Spinlock,
#endif // !AE_CFG
  (uint32)llGtSixConsecZerosOrOnes,                          // ROM_JT_OFFSET[338]
  (uint32)llGtTwentyFourTransitions,                         // ROM_JT_OFFSET[339]
  (uint32)llHaltRadio,                                       // ROM_JT_OFFSET[340]
  (uint32)llHardwareError,                                   // ROM_JT_OFFSET[341]
  (uint32)llInitFeatureSet,                                  // ROM_JT_OFFSET[342]
#if (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  (uint32)llLinkSchedSetup,                                  // ROM_JT_OFFSET[343]
#else // !(ADV_CONN_CFG | INIT_CFG)
  (uint32)ROM_Spinlock,
#endif // (ADV_CONN_CFG | INIT_CFG)
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & PHY_LR_CFG)
  (uint32)llLtThreeOnesInLsb,                                // ROM_JT_OFFSET[344]
#else // AE_CFG
  (uint32)ROM_Spinlock,
#endif // !AE_CFG
  (uint32)llLtTwoChangesInLastSixBits,                       // ROM_JT_OFFSET[345]
  (uint32)llMemCopySrc,                                      // ROM_JT_OFFSET[346]
  (uint32)llMoveTempTxDataEntries,                           // ROM_JT_OFFSET[347]
#if ( CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG) )
  (uint32)llNextChanIndex_hook,                              // ROM_JT_OFFSET[348]
#else // !( CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG) )
  (uint32)ROM_Spinlock,
#endif // ( CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG) )
  (uint32)llOneBitSynchWordDiffer,                           // ROM_JT_OFFSET[349]
  (uint32)llPendingUpdateParam,                              // ROM_JT_OFFSET[350]
  (uint32)llProcessChanMap,                                  // ROM_JT_OFFSET[351]
  (uint32)llProcessMasterControlProcedures,                  // ROM_JT_OFFSET[352]
  (uint32)llProcessTxData,                                   // ROM_JT_OFFSET[353]
  (uint32)llProcessSlaveControlProcedures,                   // ROM_JT_OFFSET[354]
  (uint32)llReleaseConnId,                                   // ROM_JT_OFFSET[355]
  (uint32)llReplaceCtrlPkt,                                  // ROM_JT_OFFSET[356]
  (uint32)llReverseBits,                                     // ROM_JT_OFFSET[357]
  (uint32)llRfInit,                                          // ROM_JT_OFFSET[358]
  (uint32)llRfSetup,                                         // ROM_JT_OFFSET[359]
  (uint32)llRfStartFS,                                       // ROM_JT_OFFSET[360]
#ifdef ONE_BLE_LIB_SIZE_OPTIMIZATION
  (uint32)llScheduler_hook,                                  // ROM_JT_OFFSET[361]
#else
  (uint32)llScheduler,                                       // ROM_JT_OFFSET[361]
#endif
  (uint32)llScheduleTask,                                    // ROM_JT_OFFSET[362]
  (uint32)llSendAdvSetEndEvent,                              // ROM_JT_OFFSET[363]
  (uint32)llSendAdvSetTermEvent,                             // ROM_JT_OFFSET[364]
  (uint32)llSendReject,                                      // ROM_JT_OFFSET[365]
  (uint32)llSetCodedMaxTxTime,                               // ROM_JT_OFFSET[366]
  (uint32)llSetNextDataChan,                                 // ROM_JT_OFFSET[367]
  (uint32)llSetTxPower,                                      // ROM_JT_OFFSET[368]
#if !(defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG))
  (uint32)llSetupAdv,                                        // ROM_JT_OFFSET[369]
#else // AE_CFG
  (uint32)ROM_Spinlock,
#endif // !AE_CFG
  (uint32)llSetupAdvDataEntryQueue,                          // ROM_JT_OFFSET[370]
#if ( CTRL_CONFIG & ( SCAN_CFG | INIT_CFG ) )
  (uint32)llSetupConn,                                       // ROM_JT_OFFSET[371]
#else // !(SCAN_CFG | INIT_CFG)
  (uint32)ROM_Spinlock,
#endif // (SCAN_CFG | INIT_CFG)
  (uint32)llSetupConnParamReq,                               // ROM_JT_OFFSET[372]
  (uint32)llSetupConnParamRsp,                               // ROM_JT_OFFSET[373]
  (uint32)llSetupConnRxDataEntryQueue,                       // ROM_JT_OFFSET[374]
  (uint32)llSetupEncReq,                                     // ROM_JT_OFFSET[375]
  (uint32)llSetupEncRsp,                                     // ROM_JT_OFFSET[376]
#if ( CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG) )
  (uint32)llSetupExtAdv_hook,                                // ROM_JT_OFFSET[377]
  (uint32)llSetupExtAdvLegacy,                               // ROM_JT_OFFSET[378]
#else // !( CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG) )
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
#endif // ( CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG) )
  (uint32)llSetupExtData,                                    // ROM_JT_OFFSET[379]
  (uint32)llSetupExtHdr,                                     // ROM_JT_OFFSET[380]
  (uint32)llSetupExtInit,                                    // ROM_JT_OFFSET[381]
  (uint32)llSetupExtScan,                                    // ROM_JT_OFFSET[382]
  (uint32)llSetupFeatureSetReq,                              // ROM_JT_OFFSET[383]
  (uint32)llSetupFeatureSetRsp,                              // ROM_JT_OFFSET[384]
#if !(defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG))
  (uint32)llSetupInit,                                       // ROM_JT_OFFSET[385]
#else // AE_CFG
  (uint32)ROM_Spinlock,
#endif // !AE_CFG
  (uint32)llSetupInitDataEntryQueue,                         // ROM_JT_OFFSET[386]
#if ( CTRL_CONFIG & ( SCAN_CFG | INIT_CFG ) )
  (uint32)llSetupNextMasterEvent,                            // ROM_JT_OFFSET[387]
#else // !(SCAN_CFG | INIT_CFG)
  (uint32)ROM_Spinlock,
#endif // (SCAN_CFG | INIT_CFG)
#if ( CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG) )
  (uint32)llSetupNextSlaveEvent,                             // ROM_JT_OFFSET[388]
#else // !( CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG) )
  (uint32)ROM_Spinlock,
#endif // ( CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG) )
  (uint32)llSetupPauseEncReq,                                // ROM_JT_OFFSET[389]
  (uint32)llSetupPauseEncRsp,                                // ROM_JT_OFFSET[390]
  (uint32)llSetupPhyCtrlPkt,                                 // ROM_JT_OFFSET[391]
  (uint32)llSetupPingReq,                                    // ROM_JT_OFFSET[392]
  (uint32)llSetupPingRsp,                                    // ROM_JT_OFFSET[393]
  (uint32)llSetupRatCompare,                                 // ROM_JT_OFFSET[394]
  (uint32)llSetupRejectInd,                                  // ROM_JT_OFFSET[395]
  (uint32)llSetupRejectIndExt,                               // ROM_JT_OFFSET[396]
#if !(defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG))
  (uint32)llSetupScan,                                       // ROM_JT_OFFSET[397]
#else // AE_CFG
  (uint32)ROM_Spinlock,
#endif // !AE_CFG
  (uint32)llSetupScanDataEntryQueue,                         // ROM_JT_OFFSET[398]
  (uint32)llSetupStartEncReq,                                // ROM_JT_OFFSET[399]
  (uint32)llSetupStartEncRsp,                                // ROM_JT_OFFSET[400]
  (uint32)llSetupTermInd,                                    // ROM_JT_OFFSET[401]
  (uint32)llSetupUnknownRsp,                                 // ROM_JT_OFFSET[402]
  (uint32)llSetupUpdateChanReq,                              // ROM_JT_OFFSET[403]
  (uint32)llSetupUpdateParamReq,                             // ROM_JT_OFFSET[404]
  (uint32)llSetupVersionIndReq,                              // ROM_JT_OFFSET[405]
  (uint32)llStartDurationTimer,                              // ROM_JT_OFFSET[406]
  (uint32)llTimeCompare,                                     // ROM_JT_OFFSET[407]
  (uint32)llTimeDelta,                                       // ROM_JT_OFFSET[408]
  (uint32)llValidAccessAddr,                                 // ROM_JT_OFFSET[409]
  (uint32)llValidateConnParams,                              // ROM_JT_OFFSET[410]
  (uint32)llVerifyCodedConnInterval,                         // ROM_JT_OFFSET[411]
  (uint32)llVerifyConnParamReqParams,                        // ROM_JT_OFFSET[412]
  (uint32)llWriteTxData,                                     // ROM_JT_OFFSET[413]
#ifdef HOST_CONFIG
  (uint32)linkDB_Add_sPatch,                                 // ROM_JT_OFFSET[414]
  (uint32)linkDB_Authen,                                     // ROM_JT_OFFSET[415]
  (uint32)linkDB_Find,                                       // ROM_JT_OFFSET[416]
  (uint32)linkDB_Init,                                       // ROM_JT_OFFSET[417]
  (uint32)linkDB_MTU,                                        // ROM_JT_OFFSET[418]
  (uint32)linkDB_Register,                                   // ROM_JT_OFFSET[419]
  (uint32)linkDB_Remove,                                     // ROM_JT_OFFSET[420]
  (uint32)linkDB_Role,                                       // ROM_JT_OFFSET[421]
  (uint32)linkDB_reportStatusChange,                         // ROM_JT_OFFSET[422]
  (uint32)linkDB_UpdateMTU,                                  // ROM_JT_OFFSET[423]
  (uint32)linkDB_Update,                                     // ROM_JT_OFFSET[424]
  (uint32)linkDB_updateConnParam,                            // ROM_JT_OFFSET[425]
  (uint32)linkDB_NumActive,                                  // ROM_JT_OFFSET[426]
  (uint32)linkDB_NumConns,                                   // ROM_JT_OFFSET[427]
  (uint32)linkDB_State,                                      // ROM_JT_OFFSET[428]
  (uint32)L2CAP_bm_alloc,                                    // ROM_JT_OFFSET[429]
  (uint32)L2CAP_BuildCmdReject,                              // ROM_JT_OFFSET[430]
  (uint32)L2CAP_BuildConnectReq,                             // ROM_JT_OFFSET[431]
  (uint32)L2CAP_BuildParamUpdateRsp,                         // ROM_JT_OFFSET[432]
  (uint32)L2CAP_CmdReject,                                   // ROM_JT_OFFSET[433]
  (uint32)L2CAP_ConnParamUpdateReq,                          // ROM_JT_OFFSET[434]
  (uint32)L2CAP_ConnParamUpdateRsp,                          // ROM_JT_OFFSET[435]
  (uint32)L2CAP_DisconnectReq,                               // ROM_JT_OFFSET[436]
  (uint32)L2CAP_GetMTU,                                      // ROM_JT_OFFSET[437]
  (uint32)L2CAP_HostNumCompletedPkts,                        // ROM_JT_OFFSET[438]
  (uint32)L2CAP_ParseConnectReq,                             // ROM_JT_OFFSET[439]
  (uint32)L2CAP_ParseFlowCtrlCredit,                         // ROM_JT_OFFSET[440]
  (uint32)L2CAP_ParseParamUpdateReq,                         // ROM_JT_OFFSET[441]
  (uint32)L2CAP_RegisterApp,                                 // ROM_JT_OFFSET[442]
  (uint32)L2CAP_SendData,                                    // ROM_JT_OFFSET[443]
  (uint32)L2CAP_SendDataPkt,                                 // ROM_JT_OFFSET[444]
  (uint32)L2CAP_SetBufSize,                                  // ROM_JT_OFFSET[445]
  (uint32)L2CAP_SetControllerToHostFlowCtrl,                 // ROM_JT_OFFSET[446]
  (uint32)l2capAllocChannel,                                 // ROM_JT_OFFSET[447]
  (uint32)l2capAllocConnChannel,                             // ROM_JT_OFFSET[448]
  (uint32)l2capBuildInfoReq,                                 // ROM_JT_OFFSET[449]
  (uint32)l2capBuildParamUpdateReq,                          // ROM_JT_OFFSET[450]
  (uint32)l2capBuildSignalHdr,                               // ROM_JT_OFFSET[451]
  (uint32)l2capDisconnectAllChannels,                        // ROM_JT_OFFSET[452]
  (uint32)l2capEncapSendData,                                // ROM_JT_OFFSET[453]
  (uint32)l2capFindLocalId,                                  // ROM_JT_OFFSET[454]
  (uint32)l2capFreeChannel,                                  // ROM_JT_OFFSET[455]
  (uint32)l2capFreePendingPkt,                               // ROM_JT_OFFSET[456]
  (uint32)l2capHandleTimerCB,                                // ROM_JT_OFFSET[457]
  (uint32)l2capHandleRxError,                                // ROM_JT_OFFSET[458]
  (uint32)l2capNotifyData,                                   // ROM_JT_OFFSET[459]
  (uint32)l2capNotifyEvent,                                  // ROM_JT_OFFSET[460]
  (uint32)l2capNotifySignal,                                 // ROM_JT_OFFSET[461]
  (uint32)l2capParseCmdReject,                               // ROM_JT_OFFSET[462]
  (uint32)l2capParsePacket,                                  // ROM_JT_OFFSET[463]
  (uint32)l2capParseParamUpdateRsp,                          // ROM_JT_OFFSET[464]
  (uint32)l2capParseSignalHdr,                               // ROM_JT_OFFSET[465]
  (uint32)l2capProcessOSALMsg,                               // ROM_JT_OFFSET[466]
  (uint32)l2capProcessReq,                                   // ROM_JT_OFFSET[467]
  (uint32)l2capProcessRsp,                                   // ROM_JT_OFFSET[468]
  (uint32)l2capProcessRxData,                                // ROM_JT_OFFSET[469]
  (uint32)l2capProcessSignal,                                // ROM_JT_OFFSET[470]
  (uint32)l2capSendCmd,                                      // ROM_JT_OFFSET[471]
  (uint32)l2capSendFCPkt,                                    // ROM_JT_OFFSET[472]
  (uint32)l2capSendPkt,                                      // ROM_JT_OFFSET[473]
  (uint32)l2capSendReq,                                      // ROM_JT_OFFSET[474]
  (uint32)l2capStartTimer,                                   // ROM_JT_OFFSET[475]
  (uint32)l2capStopTimer,                                    // ROM_JT_OFFSET[476]
  (uint32)l2capStoreFCPkt,                                   // ROM_JT_OFFSET[477]
  (uint32)l2capFindPsm,                                      // ROM_JT_OFFSET[478]
  (uint32)l2capAllocPsm,                                     // ROM_JT_OFFSET[479]
  (uint32)l2capNumActiveChannnels,                           // ROM_JT_OFFSET[480]
  (uint32)l2capFindLocalCID,                                 // ROM_JT_OFFSET[481]
  (uint32)l2capGetCoChannelInfo,                             // ROM_JT_OFFSET[482]
  (uint32)l2capFindRemoteId,                                 // ROM_JT_OFFSET[483]
  (uint32)l2capSendConnectRsp,                               // ROM_JT_OFFSET[484]
  (uint32)l2capBuildDisconnectReq,                           // ROM_JT_OFFSET[485]
  (uint32)l2capFlowCtrlCredit,                               // ROM_JT_OFFSET[486]
  (uint32)l2capReassembleSegment,                            // ROM_JT_OFFSET[487]
  (uint32)l2capParseConnectRsp,                              // ROM_JT_OFFSET[488]
  (uint32)l2capNotifyChannelEstEvt,                          // ROM_JT_OFFSET[489]
  (uint32)l2capParseDisconnectRsp,                           // ROM_JT_OFFSET[490]
  (uint32)l2capNotifyChannelTermEvt,                         // ROM_JT_OFFSET[491]
  (uint32)l2capProcessConnectReq,                            // ROM_JT_OFFSET[492]
  (uint32)l2capParseDisconnectReq,                           // ROM_JT_OFFSET[493]
  (uint32)l2capBuildDisconnectRsp,                           // ROM_JT_OFFSET[494]
  (uint32)l2capFindRemoteCID,                                // ROM_JT_OFFSET[495]
  (uint32)l2capDisconnectChannel,                            // ROM_JT_OFFSET[496]
  (uint32)l2capSendNextSegment,                              // ROM_JT_OFFSET[497]
  (uint32)l2capFindNextSegment,                              // ROM_JT_OFFSET[498]
  (uint32)l2capSendSegment,                                  // ROM_JT_OFFSET[499]
  (uint32)l2capFreeTxSDU,                                    // ROM_JT_OFFSET[500]
  (uint32)l2capNotifyCreditEvt,                              // ROM_JT_OFFSET[501]
  (uint32)l2capBuildConnectRsp,                              // ROM_JT_OFFSET[502]
  (uint32)l2capNotifySendSduDoneEvt,                         // ROM_JT_OFFSET[503]
  (uint32)l2capBuildFlowCtrlCredit,                          // ROM_JT_OFFSET[504]
  (uint32)ATT_BuildErrorRsp,                                 // ROM_JT_OFFSET[505]
  (uint32)ATT_BuildExchangeMTURsp,                           // ROM_JT_OFFSET[506]
  (uint32)ATT_BuildFindByTypeValueRsp,                       // ROM_JT_OFFSET[507]
  (uint32)ATT_BuildFindInfoRsp,                              // ROM_JT_OFFSET[508]
  (uint32)ATT_BuildHandleValueInd,                           // ROM_JT_OFFSET[509]
  (uint32)ATT_BuildPrepareWriteRsp,                          // ROM_JT_OFFSET[510]
  (uint32)ATT_BuildReadBlobRsp,                              // ROM_JT_OFFSET[511]
  (uint32)ATT_BuildReadByGrpTypeRsp,                         // ROM_JT_OFFSET[512]
  (uint32)ATT_BuildReadByTypeRsp,                            // ROM_JT_OFFSET[513]
  (uint32)ATT_BuildReadMultiRsp,                             // ROM_JT_OFFSET[514]
  (uint32)ATT_BuildReadRsp,                                  // ROM_JT_OFFSET[515]
  (uint32)ATT_CompareUUID,                                   // ROM_JT_OFFSET[516]
  (uint32)ATT_ConvertUUIDto128,                              // ROM_JT_OFFSET[517]
  (uint32)ATT_ErrorRsp,                                      // ROM_JT_OFFSET[518]
  (uint32)ATT_ExchangeMTUReq,                                // ROM_JT_OFFSET[519]
  (uint32)ATT_ExchangeMTURsp,                                // ROM_JT_OFFSET[520]
  (uint32)ATT_ExecuteWriteReq,                               // ROM_JT_OFFSET[521]
  (uint32)ATT_ExecuteWriteRsp,                               // ROM_JT_OFFSET[522]
  (uint32)ATT_FindByTypeValueReq,                            // ROM_JT_OFFSET[523]
  (uint32)ATT_FindByTypeValueRsp,                            // ROM_JT_OFFSET[524]
  (uint32)ATT_FindInfoReq,                                   // ROM_JT_OFFSET[525]
  (uint32)ATT_FindInfoRsp,                                   // ROM_JT_OFFSET[526]
  (uint32)ATT_GetMTU,                                        // ROM_JT_OFFSET[527]
  (uint32)ATT_HandleValueInd,                                // ROM_JT_OFFSET[528]
  (uint32)ATT_HandleValueNoti,                               // ROM_JT_OFFSET[529]
  (uint32)ATT_ParseErrorRsp,                                 // ROM_JT_OFFSET[530]
  (uint32)ATT_ParseExchangeMTUReq,                           // ROM_JT_OFFSET[531]
  (uint32)ATT_ParseExecuteWriteReq,                          // ROM_JT_OFFSET[532]
  (uint32)ATT_ParseFindInfoReq,                              // ROM_JT_OFFSET[533]
  (uint32)ATT_ParseFindByTypeValueReq,                       // ROM_JT_OFFSET[534]
  (uint32)ATT_ParseHandleValueInd,                           // ROM_JT_OFFSET[535]
  (uint32)ATT_ParsePacket,                                   // ROM_JT_OFFSET[536]
  (uint32)ATT_ParsePrepareWriteReq,                          // ROM_JT_OFFSET[537]
  (uint32)ATT_ParseReadBlobReq,                              // ROM_JT_OFFSET[538]
  (uint32)ATT_ParseReadByTypeReq,                            // ROM_JT_OFFSET[539]
  (uint32)ATT_ParseReadMultiReq,                             // ROM_JT_OFFSET[540]
  (uint32)ATT_ParseReadReq,                                  // ROM_JT_OFFSET[541]
  (uint32)ATT_ParseWriteReq,                                 // ROM_JT_OFFSET[542]
  (uint32)ATT_PrepareWriteReq,                               // ROM_JT_OFFSET[543]
  (uint32)ATT_PrepareWriteRsp,                               // ROM_JT_OFFSET[544]
  (uint32)ATT_ReadBlobReq,                                   // ROM_JT_OFFSET[545]
  (uint32)ATT_ReadBlobRsp,                                   // ROM_JT_OFFSET[546]
  (uint32)ATT_ReadByGrpTypeReq,                              // ROM_JT_OFFSET[547]
  (uint32)ATT_ReadByGrpTypeRsp,                              // ROM_JT_OFFSET[548]
  (uint32)ATT_ReadByTypeReq,                                 // ROM_JT_OFFSET[549]
  (uint32)ATT_ReadByTypeRsp,                                 // ROM_JT_OFFSET[550]
  (uint32)ATT_ReadMultiReq,                                  // ROM_JT_OFFSET[551]
  (uint32)ATT_ReadMultiRsp,                                  // ROM_JT_OFFSET[552]
  (uint32)ATT_ReadReq,                                       // ROM_JT_OFFSET[553]
  (uint32)ATT_ReadRsp,                                       // ROM_JT_OFFSET[554]
  (uint32)ATT_RegisterServer,                                // ROM_JT_OFFSET[555]
  (uint32)ATT_RegisterClient,                                // ROM_JT_OFFSET[556]
  (uint32)ATT_UpdateMTU,                                     // ROM_JT_OFFSET[557]
  (uint32)ATT_WriteReq,                                      // ROM_JT_OFFSET[558]
  (uint32)ATT_WriteRsp,                                      // ROM_JT_OFFSET[559]
  (uint32)attSendMsg_sPatch,                                 // ROM_JT_OFFSET[560]
  (uint32)attSendRspMsg,                                     // ROM_JT_OFFSET[561]
  (uint32)GATT_AppCompletedMsg,                              // ROM_JT_OFFSET[562]
  (uint32)GATT_bm_alloc,                                     // ROM_JT_OFFSET[563]
  (uint32)GATT_bm_free,                                      // ROM_JT_OFFSET[564]
  (uint32)GATT_FindHandle,                                   // ROM_JT_OFFSET[565]
  (uint32)GATT_FindHandleUUID,                               // ROM_JT_OFFSET[566]
  (uint32)GATT_FindNextAttr,                                 // ROM_JT_OFFSET[567]
  (uint32)GATT_InitServer,                                   // ROM_JT_OFFSET[568]
  (uint32)GATT_Indication,                                   // ROM_JT_OFFSET[569]
  (uint32)GATT_NotifyEvent,                                  // ROM_JT_OFFSET[570]
  (uint32)GATT_ServiceEncKeySize,                            // ROM_JT_OFFSET[571]
  (uint32)GATT_ServiceNumAttrs,                              // ROM_JT_OFFSET[572]
  (uint32)GATT_UpdateMTU,                                    // ROM_JT_OFFSET[573]
  (uint32)GATT_VerifyReadPermissions,                        // ROM_JT_OFFSET[574]
  (uint32)GATT_VerifyWritePermissions,                       // ROM_JT_OFFSET[575]
  (uint32)gattClientHandleConnStatusCB,                      // ROM_JT_OFFSET[576]
  (uint32)gattClientHandleTimerCB,                           // ROM_JT_OFFSET[577]
  (uint32)gattClientNotifyTxCB,                              // ROM_JT_OFFSET[578]
  (uint32)gattClientProcessMsgCB,                            // ROM_JT_OFFSET[579]
  (uint32)gattFindClientInfo,                                // ROM_JT_OFFSET[580]
  (uint32)gattFindServerInfo,                                // ROM_JT_OFFSET[581]
  (uint32)gattFindService,                                   // ROM_JT_OFFSET[582]
  (uint32)gattGetPayload,                                    // ROM_JT_OFFSET[583]
  (uint32)gattGetServerStatus,                               // ROM_JT_OFFSET[584]
  (uint32)gattNotifyEvent,                                   // ROM_JT_OFFSET[585]
  (uint32)gattParseReq,                                      // ROM_JT_OFFSET[586]
  (uint32)gattProcessExchangeMTUReq,                         // ROM_JT_OFFSET[587]
  (uint32)gattProcessExecuteWriteReq,                        // ROM_JT_OFFSET[588]
  (uint32)gattProcessFindByTypeValueReq,                     // ROM_JT_OFFSET[589]
  (uint32)gattProcessFindInfoReq,                            // ROM_JT_OFFSET[590]
  (uint32)gattProcessReadByGrpTypeReq,                       // ROM_JT_OFFSET[591]
  (uint32)gattProcessReadByTypeReq,                          // ROM_JT_OFFSET[592]
  (uint32)gattProcessReadReq,                                // ROM_JT_OFFSET[593]
  (uint32)gattProcessReq,                                    // ROM_JT_OFFSET[594]
  (uint32)gattProcessRxData,                                 // ROM_JT_OFFSET[595]
  (uint32)gattProcessOSALMsg,                                // ROM_JT_OFFSET[596]
  (uint32)gattProcessWriteReq,                               // ROM_JT_OFFSET[597]
  (uint32)gattProcessReadMultiReq,                           // ROM_JT_OFFSET[598]
  (uint32)gattRegisterClient,                                // ROM_JT_OFFSET[599]
  (uint32)gattRegisterServer,                                // ROM_JT_OFFSET[600]
  (uint32)gattResetServerInfo,                               // ROM_JT_OFFSET[601]
  (uint32)gattSendFlowCtrlEvt,                               // ROM_JT_OFFSET[602]
  (uint32)gattServerHandleConnStatusCB,                      // ROM_JT_OFFSET[603]
  (uint32)gattServerHandleTimerCB,                           // ROM_JT_OFFSET[604]
  (uint32)gattServerNotifyTxCB,                              // ROM_JT_OFFSET[605]
  (uint32)gattServerProcessMsgCB,                            // ROM_JT_OFFSET[606]
  (uint32)gattServerStartTimer,                              // ROM_JT_OFFSET[607]
  (uint32)gattServiceLastHandle,                             // ROM_JT_OFFSET[608]
  (uint32)gattStartTimer,                                    // ROM_JT_OFFSET[609]
  (uint32)gattStopTimer,                                     // ROM_JT_OFFSET[610]
  (uint32)gattStoreServerInfo,                               // ROM_JT_OFFSET[611]
  (uint32)gattClientStartTimer,                              // ROM_JT_OFFSET[612]
  (uint32)gattProcessMultiReqs,                              // ROM_JT_OFFSET[613]
  (uint32)gattResetClientInfo,                               // ROM_JT_OFFSET[614]
  (uint32)gattProcessFindInfo,                               // ROM_JT_OFFSET[615]
  (uint32)gattProcessFindByTypeValue,                        // ROM_JT_OFFSET[616]
  (uint32)gattProcessReadByType,                             // ROM_JT_OFFSET[617]
  (uint32)gattProcessReadLong,                               // ROM_JT_OFFSET[618]
  (uint32)gattProcessReadByGrpType,                          // ROM_JT_OFFSET[619]
  (uint32)gattProcessReliableWrites,                         // ROM_JT_OFFSET[620]
  (uint32)gattProcessWriteLong,                              // ROM_JT_OFFSET[621]
  (uint32)gattWrite,                                         // ROM_JT_OFFSET[622]
  (uint32)gattWriteLong,                                     // ROM_JT_OFFSET[623]
  (uint32)gattPrepareWriteReq,                               // ROM_JT_OFFSET[624]
  (uint32)gattStoreClientInfo,                               // ROM_JT_OFFSET[625]
  (uint32)gattReadByGrpType,                                 // ROM_JT_OFFSET[626]
  (uint32)gattFindByTypeValue,                               // ROM_JT_OFFSET[627]
  (uint32)gattReadByType,                                    // ROM_JT_OFFSET[628]
  (uint32)gattFindInfo,                                      // ROM_JT_OFFSET[629]
  (uint32)gattRead,                                          // ROM_JT_OFFSET[630]
  (uint32)gattReadLong,                                      // ROM_JT_OFFSET[631]
  (uint32)gattGetClientStatus,                               // ROM_JT_OFFSET[632]
  (uint32)gattServApp_buildReadByTypeRsp,                    // ROM_JT_OFFSET[633]
#if ( ( HOST_CONFIG & CENTRAL_CFG ) || ( HOST_CONFIG & PERIPHERAL_CFG) )
  (uint32)gattServApp_ClearPrepareWriteQ,                    // ROM_JT_OFFSET[634]
  (uint32)gattServApp_EnqueuePrepareWriteReq,                // ROM_JT_OFFSET[635]
#else //
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
#endif
  (uint32)gattServApp_EnqueueReTx,                           // ROM_JT_OFFSET[636]
  (uint32)gattServApp_FindAuthorizeAttrCB,                   // ROM_JT_OFFSET[637]
  (uint32)gattServApp_FindPrepareWriteQ,                     // ROM_JT_OFFSET[638]
  (uint32)gattServApp_FindServiceCBs,                        // ROM_JT_OFFSET[639]
  (uint32)gattServApp_IsWriteLong,                           // ROM_JT_OFFSET[640]
#if ( ( HOST_CONFIG & CENTRAL_CFG ) || ( HOST_CONFIG & PERIPHERAL_CFG) )
  (uint32)gattServApp_ProcessExchangeMTUReq,                 // ROM_JT_OFFSET[641]
#else //
  (uint32)ROM_Spinlock,
#endif
  (uint32)gattServApp_ProcessExecuteWriteReq,                // ROM_JT_OFFSET[642]
  (uint32)gattServApp_ProcessFindByTypeValueReq,             // ROM_JT_OFFSET[643]
  (uint32)gattServApp_ProcessPrepareWriteReq,                // ROM_JT_OFFSET[644]
#if ( ( HOST_CONFIG & CENTRAL_CFG ) || ( HOST_CONFIG & PERIPHERAL_CFG) )
  (uint32)GATTServApp_ReadAttr,                              // ROM_JT_OFFSET[645]
#else //
  (uint32)ROM_Spinlock,
#endif
  (uint32)gattServApp_ProcessReadBlobReq,                    // ROM_JT_OFFSET[646]
  (uint32)gattServApp_ProcessReadByTypeReq,                  // ROM_JT_OFFSET[647]
  (uint32)gattServApp_ProcessReadByGrpTypeReq,               // ROM_JT_OFFSET[648]
  (uint32)gattServApp_ProcessReadMultiReq,                   // ROM_JT_OFFSET[649]
  (uint32)gattServApp_ProcessReadReq,                        // ROM_JT_OFFSET[650]
#if ( ( HOST_CONFIG & CENTRAL_CFG ) || ( HOST_CONFIG & PERIPHERAL_CFG) )
  (uint32)gattServApp_ProcessReliableWrites,                 // ROM_JT_OFFSET[651]
  (uint32)GATTServApp_WriteAttr,                             // ROM_JT_OFFSET[652]
  (uint32)gattServApp_ProcessWriteLong,                      // ROM_JT_OFFSET[653]
#else //
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
#endif
  (uint32)gattServApp_ProcessWriteReq,                       // ROM_JT_OFFSET[654]
  (uint32)gap_CentConnRegister,                              // ROM_JT_OFFSET[655]
#ifdef GAP_BOND_MGR
  (uint32)GAP_DeviceInit,                                    // ROM_JT_OFFSET[656]
#else
  (uint32)GAP_DeviceInit_noGAPBondMgr,                       // ROM_JT_OFFSET[656]
#endif
  (uint32)GAP_GetDevAddress,                                 // ROM_JT_OFFSET[657]
  (uint32)GAP_GetIRK,                                        // ROM_JT_OFFSET[658]
  (uint32)GAP_GetParamValue,                                 // ROM_JT_OFFSET[659]
  (uint32)GapConfig_SetParameter,                            // ROM_JT_OFFSET[660]
  (uint32)gapConnEvtNoticeCB,                                // ROM_JT_OFFSET[661]
  (uint32)GAP_NumActiveConnections,                          // ROM_JT_OFFSET[662]
  (uint32)gap_ParamsInit,                                    // ROM_JT_OFFSET[663]
  (uint32)GAP_PasscodeUpdate,                                // ROM_JT_OFFSET[664]
  (uint32)gap_PeriConnRegister,                              // ROM_JT_OFFSET[665]
  (uint32)gap_PrivacyInit,                                   // ROM_JT_OFFSET[666]
  (uint32)gap_SecParamsInit,                                 // ROM_JT_OFFSET[667]
  (uint32)GAP_UpdateLinkParamReqReply,                       // ROM_JT_OFFSET[668]
#ifdef GAP_BOND_MGR
  (uint32)GAP_UpdateResolvingList,                           // ROM_JT_OFFSET[669]
#else
  (uint32)GAP_UpdateResolvingList_noGAPBondMgr,              // ROM_JT_OFFSET[656]
#endif
  (uint32)gapProcessBLEEvents,                               // ROM_JT_OFFSET[670]
  (uint32)gapProcessCommandStatusEvt,                        // ROM_JT_OFFSET[671]
  (uint32)gapProcessConnEvt,                                 // ROM_JT_OFFSET[672]
  (uint32)gapProcessHCICmdCompleteEvt,                       // ROM_JT_OFFSET[673]
  (uint32)gapProcessOSALMsg,                                 // ROM_JT_OFFSET[674]
  (uint32)gapGetDevAddressMode,                              // ROM_JT_OFFSET[675]
  (uint32)gapGetSignCounter,                                 // ROM_JT_OFFSET[676]
  (uint32)gapGetState,                                       // ROM_JT_OFFSET[677]
  (uint32)gapGetSRK,                                         // ROM_JT_OFFSET[678]
  (uint32)gapHost2CtrlOwnAddrType,                           // ROM_JT_OFFSET[679]
  (uint32)gapIncSignCounter,                                 // ROM_JT_OFFSET[680]
  (uint32)gapReadBufSizeCmdStatus,                           // ROM_JT_OFFSET[681]
  (uint32)gapSendDeviceInitDoneEvent,                        // ROM_JT_OFFSET[682]
  (uint32)gapCentProcessConnEvt,                             // ROM_JT_OFFSET[683]
  (uint32)gapCentProcessConnUpdateCompleteEvt,               // ROM_JT_OFFSET[684]
  (uint32)gapCentProcessSignalEvt,                           // ROM_JT_OFFSET[685]
  (uint32)disconnectNext,                                    // ROM_JT_OFFSET[686]
  (uint32)gapFreeAuthLink,                                   // ROM_JT_OFFSET[687]
  (uint32)gapPairingCompleteCB,                              // ROM_JT_OFFSET[688]
  (uint32)gapPasskeyNeededCB,                                // ROM_JT_OFFSET[689]
  (uint32)gapProcessConnectionCompleteEvt,                   // ROM_JT_OFFSET[690]
  (uint32)gapProcessDisconnectCompleteEvt,                   // ROM_JT_OFFSET[691]
  (uint32)gapProcessRemoteConnParamReqEvt,                   // ROM_JT_OFFSET[692]
  (uint32)gapRegisterCentralConn,                            // ROM_JT_OFFSET[693]
  (uint32)gapRegisterPeripheralConn,                         // ROM_JT_OFFSET[694]
  (uint32)gapSendBondCompleteEvent,                          // ROM_JT_OFFSET[695]
  (uint32)gapSendLinkUpdateEvent,                            // ROM_JT_OFFSET[696]
  (uint32)gapSendPairingReqEvent,                            // ROM_JT_OFFSET[697]
  (uint32)gapSendSignUpdateEvent,                            // ROM_JT_OFFSET[698]
  (uint32)gapSendSlaveSecurityReqEvent_hook,                 // ROM_JT_OFFSET[699]
  (uint32)gapUpdateConnSignCounter,                          // ROM_JT_OFFSET[700]
  (uint32)sendAuthEvent,                                     // ROM_JT_OFFSET[701]
  (uint32)sendEstLinkEvent,                                  // ROM_JT_OFFSET[702]
  (uint32)sendTerminateEvent,                                // ROM_JT_OFFSET[703]
  (uint32)gapClrState,                                       // ROM_JT_OFFSET[704]
  (uint32)gapFindADType,                                     // ROM_JT_OFFSET[705]
  (uint32)gapIsAdvertising,                                  // ROM_JT_OFFSET[706]
  (uint32)gapIsInitiating,                                   // ROM_JT_OFFSET[707]
  (uint32)gapIsScanning,                                     // ROM_JT_OFFSET[708]
  (uint32)gapSetState,                                       // ROM_JT_OFFSET[709]
  (uint32)gapValidADType,                                    // ROM_JT_OFFSET[710]
  (uint32)gapL2capConnParamUpdateReq,                        // ROM_JT_OFFSET[711]
  (uint32)gapPeriProcessConnUpdateCmdStatus,                 // ROM_JT_OFFSET[712]
  (uint32)gapPeriProcessConnUpdateCompleteEvt,               // ROM_JT_OFFSET[713]
  (uint32)gapPeriProcessSignalEvt,                           // ROM_JT_OFFSET[714]
  (uint32)gapPeriProcessConnEvt,                             // ROM_JT_OFFSET[715]
#if ( ( HOST_CONFIG & CENTRAL_CFG ) || ( HOST_CONFIG & PERIPHERAL_CFG) )
  (uint32)SM_dhKeyCB_hook,                                   // ROM_JT_OFFSET[716]
#else //
  (uint32)ROM_Spinlock,
#endif
  (uint32)SM_GenerateAuthenSig,                              // ROM_JT_OFFSET[717]
#if ( ( HOST_CONFIG & CENTRAL_CFG ) || ( HOST_CONFIG & PERIPHERAL_CFG) )
  (uint32)SM_p256KeyCB_hook,                                 // ROM_JT_OFFSET[718]
#else //
  (uint32)ROM_Spinlock,
#endif
  (uint32)SM_PasskeyUpdate,                                  // ROM_JT_OFFSET[719]
  (uint32)SM_StartPairing,                                   // ROM_JT_OFFSET[720]
  (uint32)SM_VerifyAuthenSig,                                // ROM_JT_OFFSET[721]
  (uint32)SM_ResponderInit,                                  // ROM_JT_OFFSET[722]
  (uint32)SM_InitiatorInit,                                  // ROM_JT_OFFSET[723]
  (uint32)SM_StartEncryption,                                // ROM_JT_OFFSET[724]
  (uint32)smProcessHCIBLEEventCode,                          // ROM_JT_OFFSET[725]
  (uint32)smProcessHCIBLEMetaEventCode,                      // ROM_JT_OFFSET[726]
  (uint32)smProcessOSALMsg,                                  // ROM_JT_OFFSET[727]
  (uint32)generate_subkey,                                   // ROM_JT_OFFSET[728]
  (uint32)leftshift_onebit,                                  // ROM_JT_OFFSET[729]
  (uint32)padding,                                           // ROM_JT_OFFSET[730]
  (uint32)smAuthReqToUint8,                                  // ROM_JT_OFFSET[731]
  (uint32)smEncrypt,                                         // ROM_JT_OFFSET[732]
  (uint32)smEncryptLocal,                                    // ROM_JT_OFFSET[733]
  (uint32)smGenerateRandBuf,                                 // ROM_JT_OFFSET[734]
  (uint32)smStartRspTimer,                                   // ROM_JT_OFFSET[735]
  (uint32)smUint8ToAuthReq,                                  // ROM_JT_OFFSET[736]
  (uint32)sm_c1new,                                          // ROM_JT_OFFSET[737]
  (uint32)sm_CMAC,                                           // ROM_JT_OFFSET[738]
  (uint32)sm_f4,                                             // ROM_JT_OFFSET[739]
  (uint32)sm_f5,                                             // ROM_JT_OFFSET[740]
  (uint32)sm_f6,                                             // ROM_JT_OFFSET[741]
  (uint32)sm_g2,                                             // ROM_JT_OFFSET[742]
  (uint32)sm_s1,                                             // ROM_JT_OFFSET[743]
  (uint32)sm_xor,                                            // ROM_JT_OFFSET[744]
  (uint32)xor_128,                                           // ROM_JT_OFFSET[745]
  (uint32)smDetermineIOCaps,                                 // ROM_JT_OFFSET[746]
  (uint32)smDetermineKeySize,                                // ROM_JT_OFFSET[747]
  (uint32)smEndPairing,                                      // ROM_JT_OFFSET[748]
  (uint32)smFreePairingParams,                               // ROM_JT_OFFSET[749]
  (uint32)smF5Wrapper,                                       // ROM_JT_OFFSET[750]
  (uint32)smGenerateAddrInput_sPatch,                        // ROM_JT_OFFSET[751]
  (uint32)smGenerateConfirm,                                 // ROM_JT_OFFSET[752]
  (uint32)smGenerateDHKeyCheck,                              // ROM_JT_OFFSET[753]
  (uint32)smGeneratePairingReqRsp_hook,                      // ROM_JT_OFFSET[754]
  (uint32)smGenerateRandMsg,                                 // ROM_JT_OFFSET[755]
  (uint32)smGetECCKeys,                                      // ROM_JT_OFFSET[756]
  (uint32)smIncrementEccKeyRecycleCount,                     // ROM_JT_OFFSET[757]
  (uint32)smLinkCheck,                                       // ROM_JT_OFFSET[758]
  (uint32)smNextPairingState,                                // ROM_JT_OFFSET[759]
  (uint32)smOobSCAuthentication,                             // ROM_JT_OFFSET[760]
  (uint32)smPairingSendEncInfo,                              // ROM_JT_OFFSET[761]
  (uint32)smPairingSendIdentityAddrInfo,                     // ROM_JT_OFFSET[762]
  (uint32)smPairingSendIdentityInfo,                         // ROM_JT_OFFSET[763]
  (uint32)smPairingSendMasterID,                             // ROM_JT_OFFSET[764]
  (uint32)smPairingSendSigningInfo,                          // ROM_JT_OFFSET[765]
  (uint32)smProcessDataMsg,                                  // ROM_JT_OFFSET[766]
  (uint32)smProcessEncryptChange,                            // ROM_JT_OFFSET[767]
  (uint32)smProcessPairingReq,                               // ROM_JT_OFFSET[768]
  (uint32)smRegisterResponder,                               // ROM_JT_OFFSET[769]
  (uint32)smSavePairInfo,                                    // ROM_JT_OFFSET[770]
  (uint32)smSaveRemotePublicKeys,                            // ROM_JT_OFFSET[771]
  (uint32)smSendDHKeyCheck,                                  // ROM_JT_OFFSET[772]
  (uint32)smSendFailAndEnd,                                  // ROM_JT_OFFSET[773]
  (uint32)smSendFailureEvt,                                  // ROM_JT_OFFSET[774]
  (uint32)smSetPairingReqRsp,                                // ROM_JT_OFFSET[775]
  (uint32)smSendPublicKeys,                                  // ROM_JT_OFFSET[776]
  (uint32)smStartEncryption,                                 // ROM_JT_OFFSET[777]
  (uint32)smTimedOut,                                        // ROM_JT_OFFSET[778]
  (uint32)sm_allocateSCParameters,                           // ROM_JT_OFFSET[779]
  (uint32)sm_computeDHKey,                                   // ROM_JT_OFFSET[780]
  (uint32)sm_c1,                                             // ROM_JT_OFFSET[781]
  (uint32)smpProcessIncoming,                                // ROM_JT_OFFSET[782]
  (uint32)smFinishPublicKeyExchange,                         // ROM_JT_OFFSET[783]
  (uint32)smResponderAuthStageTwo,                           // ROM_JT_OFFSET[784]
  (uint32)smpResponderProcessEncryptionInformation,          // ROM_JT_OFFSET[785]
  (uint32)smpResponderProcessIdentityAddrInfo,               // ROM_JT_OFFSET[786]
  (uint32)smpResponderProcessIdentityInfo,                   // ROM_JT_OFFSET[787]
  (uint32)smpResponderProcessMasterID,                       // ROM_JT_OFFSET[788]
  (uint32)smpResponderProcessPairingConfirm,                 // ROM_JT_OFFSET[789]
  (uint32)smpResponderProcessPairingDHKeyCheck,              // ROM_JT_OFFSET[790]
  (uint32)smpResponderProcessPairingPublicKey_hook,          // ROM_JT_OFFSET[791]
  (uint32)smpResponderProcessPairingRandom,                  // ROM_JT_OFFSET[792]
  (uint32)smpResponderProcessPairingReq_sPatch,              // ROM_JT_OFFSET[793]
  (uint32)smpResponderProcessSigningInfo,                    // ROM_JT_OFFSET[794]
  (uint32)smpBuildEncInfo,                                   // ROM_JT_OFFSET[795]
  (uint32)smpBuildIdentityAddrInfo,                          // ROM_JT_OFFSET[796]
  (uint32)smpBuildIdentityInfo,                              // ROM_JT_OFFSET[797]
  (uint32)smpBuildMasterID,                                  // ROM_JT_OFFSET[798]
  (uint32)smpBuildPairingConfirm,                            // ROM_JT_OFFSET[799]
  (uint32)smpBuildPairingDHKeyCheck,                         // ROM_JT_OFFSET[800]
  (uint32)smpBuildPairingFailed,                             // ROM_JT_OFFSET[801]
  (uint32)smpBuildPairingPublicKey,                          // ROM_JT_OFFSET[802]
  (uint32)smpBuildPairingRandom,                             // ROM_JT_OFFSET[803]
  (uint32)smpBuildPairingReq,                                // ROM_JT_OFFSET[804]
  (uint32)smpBuildPairingReqRsp,                             // ROM_JT_OFFSET[805]
  (uint32)smpBuildPairingRsp,                                // ROM_JT_OFFSET[806]
  (uint32)smpBuildSecurityReq,                               // ROM_JT_OFFSET[807]
  (uint32)smpBuildSigningInfo,                               // ROM_JT_OFFSET[808]
  (uint32)smpParseEncInfo,                                   // ROM_JT_OFFSET[809]
  (uint32)smpParseIdentityAddrInfo,                          // ROM_JT_OFFSET[810]
  (uint32)smpParseIdentityInfo,                              // ROM_JT_OFFSET[811]
  (uint32)smpParseKeypressNoti,                              // ROM_JT_OFFSET[812]
  (uint32)smpParseMasterID,                                  // ROM_JT_OFFSET[813]
  (uint32)smpParsePairingConfirm,                            // ROM_JT_OFFSET[814]
  (uint32)smpParsePairingDHKeyCheck,                         // ROM_JT_OFFSET[815]
  (uint32)smpParsePairingFailed,                             // ROM_JT_OFFSET[816]
  (uint32)smpParsePairingPublicKey,                          // ROM_JT_OFFSET[817]
  (uint32)smStopRspTimer,                                    // ROM_JT_OFFSET[818]
  (uint32)smpParsePairingRandom,                             // ROM_JT_OFFSET[819]
  (uint32)smpParsePairingReq,                                // ROM_JT_OFFSET[820]
  (uint32)smpParseSecurityReq,                               // ROM_JT_OFFSET[821]
  (uint32)smpParseSigningInfo,                               // ROM_JT_OFFSET[822]
  (uint32)smSendSMMsg,                                       // ROM_JT_OFFSET[823]
  (uint32)smpInitiatorProcessPairingRsp_sPatch,              // ROM_JT_OFFSET[824]
  (uint32)smpInitiatorProcessPairingPubKey_sPatch,           // ROM_JT_OFFSET[825]
  (uint32)smpInitiatorProcessPairingDHKeyCheck,              // ROM_JT_OFFSET[826]
  (uint32)smpInitiatorProcessPairingConfirm,                 // ROM_JT_OFFSET[827]
#if ( HOST_CONFIG & CENTRAL_CFG )
  (uint32)smpInitiatorProcessPairingRandom_hook,             // ROM_JT_OFFSET[828]
#else //
  (uint32)ROM_Spinlock,
#endif
  (uint32)smpInitiatorProcessEncryptionInformation,          // ROM_JT_OFFSET[829]
  (uint32)smpInitiatorProcessMasterID,                       // ROM_JT_OFFSET[830]
  (uint32)smpInitiatorProcessIdentityInfo,                   // ROM_JT_OFFSET[831]
  (uint32)smpInitiatorProcessIdentityAddrInfo,               // ROM_JT_OFFSET[832]
  (uint32)smpInitiatorProcessSigningInfo,                    // ROM_JT_OFFSET[833]
  (uint32)smInitiatorAuthStageTwo,                           // ROM_JT_OFFSET[834]
  (uint32)setupInitiatorKeys,                                // ROM_JT_OFFSET[835]
  (uint32)smInitiatorSendNextKeyInfo,                        // ROM_JT_OFFSET[836]
  (uint32)smpResponderProcessIncoming_hook,                  // ROM_JT_OFFSET[837]
  (uint32)smResponderSendNextKeyInfo,                        // ROM_JT_OFFSET[838]
  (uint32)smpResponderSendPairRspEvent,                      // ROM_JT_OFFSET[839]
#if ( HOST_CONFIG & PERIPHERAL_CFG )
  (uint32)smResponderProcessLTKReq_hook,                     // ROM_JT_OFFSET[840]
#else //
  (uint32)ROM_Spinlock,
#endif
  (uint32)smRegisterInitiator,                               // ROM_JT_OFFSET[841]
  (uint32)smEncLTK,                                          // ROM_JT_OFFSET[842]
  (uint32)smpInitiatorProcessIncoming,                       // ROM_JT_OFFSET[843]
  (uint32)gapScan_init,                                      // ROM_JT_OFFSET[844]
  (uint32)gapScan_filterDiscMode,                            // ROM_JT_OFFSET[845]
  (uint32)gapScan_discardAdvRptSession,                      // ROM_JT_OFFSET[846]
  (uint32)gapScan_sendSessionEndEvt,                         // ROM_JT_OFFSET[847]
  (uint32)GapScan_discardAdvReportList,                      // ROM_JT_OFFSET[848]
  (uint32)gapScan_defragAdvRpt,                              // ROM_JT_OFFSET[849]
  (uint32)gapScan_saveRptAndNotify,                          // ROM_JT_OFFSET[850]
  (uint32)gapScan_processAdvRptCb,                           // ROM_JT_OFFSET[851]
  (uint32)gapScan_processStartEndCb,                         // ROM_JT_OFFSET[852]
  (uint32)gapScan_processErrorCb,                            // ROM_JT_OFFSET[853]
  (uint32)gapScan_processSessionEndEvt,                      // ROM_JT_OFFSET[854]
  (uint32)GapInit_cancelConnect,                             // ROM_JT_OFFSET[855]
  (uint32)gapInit_connect_internal,                          // ROM_JT_OFFSET[856]
  (uint32)gapInit_sendConnCancelledEvt,                      // ROM_JT_OFFSET[857]
  (uint32)gapInit_initiatingEnd,                             // ROM_JT_OFFSET[858]
  (uint32)gapAdv_init,                                       // ROM_JT_OFFSET[859]
  (uint32)gapAdv_searchForBufferUse,                         // ROM_JT_OFFSET[860]
  (uint32)GapAdv_disable,                                    // ROM_JT_OFFSET[861]
  (uint32)gapAdv_processRemoveSetEvt,                        // ROM_JT_OFFSET[862]
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
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
  (uint32)ROM_Spinlock,
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
};
#endif //FLASH_ROM_BUILD

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

#if defined(__GNUC__) && !defined(__clang__)
__attribute__((optimize("O0")))
#endif
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
#if defined ( FLASH_ROM_BUILD )
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
#endif // FLASH_ROM_BUILD

#ifdef ONE_BLE_LIB_SIZE_OPTIMIZATION

extern void rfCallback_all( RF_Handle rfHandle, RF_CmdHandle cmdHandle, RF_EventMask events );
extern void rfCallback_ADV_NCONN_and_ADV_CONN( RF_Handle rfHandle, RF_CmdHandle cmdHandle, RF_EventMask events );
extern void rfCallback_ADV_NCONN( RF_Handle rfHandle, RF_CmdHandle cmdHandle, RF_EventMask events );
extern void rfCallback_SCAN_CFG_and_INIT_CFG( RF_Handle rfHandle, RF_CmdHandle cmdHandle, RF_EventMask events );

void rfCallback_hook( RF_Handle rfHandle, RF_CmdHandle cmdHandle, RF_EventMask events )
{
#if ( CTRL_CONFIG == ADV_NCONN_CFG )
  rfCallback_ADV_NCONN(rfHandle, cmdHandle, events);
#elif ( CTRL_CONFIG == (ADV_NCONN_CFG | ADV_CONN_CFG) )
  rfCallback_ADV_NCONN_and_ADV_CONN(rfHandle, cmdHandle, events);
#elif ( CTRL_CONFIG == (SCAN_CFG | INIT_CFG) )
  rfCallback_SCAN_CFG_and_INIT_CFG(rfHandle, cmdHandle, events);
#else
  rfCallback_all(rfHandle, cmdHandle, events);
#endif
}

extern void LL_ProcessEvent_LL_EVT_EXT_SCAN_TIMEOUT_SCAN_CFG( void );
extern void LL_ProcessEvent_LL_EVT_EXT_SCAN_TIMEOUT_no_SCAN_CFG( void );

void LL_ProcessEvent_LL_EVT_EXT_SCAN_TIMEOUT_hook( void )
{
#if ( CTRL_CONFIG & SCAN_CFG )
  LL_ProcessEvent_LL_EVT_EXT_SCAN_TIMEOUT_SCAN_CFG();
#else
  LL_ProcessEvent_LL_EVT_EXT_SCAN_TIMEOUT_no_SCAN_CFG();
#endif
}

extern void LL_ProcessEvent_LL_EVT_MASTER_CONN_CREATED_INIT_CFG( void );
extern void LL_ProcessEvent_LL_EVT_MASTER_CONN_CREATED_no_INIT_CFG( void );

void LL_ProcessEvent_LL_EVT_MASTER_CONN_CREATED_hook( void )
{
#if ( CTRL_CONFIG & INIT_CFG )
  LL_ProcessEvent_LL_EVT_MASTER_CONN_CREATED_INIT_CFG();
#else
  LL_ProcessEvent_LL_EVT_MASTER_CONN_CREATED_no_INIT_CFG();
#endif
}

extern uint8 HOOK_llProcessMasterControlPacket( llConnState_t *connPtr, uint8 *pBuf );

uint8 HOOK_llProcessMasterControlPacket_hook( void *connPtr, uint8 *pBuf )
{
#if ( CTRL_CONFIG & INIT_CFG )
  return HOOK_llProcessMasterControlPacket(connPtr, pBuf);
#else
  return TRUE;
#endif
}

extern void        llScheduler( void );
extern void        llScheduler_no_INIT_CFG_no_SCAN_CFG( void );
extern void        llScheduler_no_INIT_CFG( void );

void llScheduler_hook( void )
{
#if ( CTRL_CONFIG & INIT_CFG )
  llScheduler();
#elif ( CTRL_CONFIG & SCAN_CFG )
  llScheduler_no_INIT_CFG();
#else
  llScheduler_no_INIT_CFG_no_SCAN_CFG();
#endif
}

#endif //ONE_BLE_LIB_SIZE_OPTIMIZATION


/*******************************************************************************
 * RTLS hooks
 */
extern uint8 llGetCteInfo( uint8 id, void *ptr );
extern uint8 RTLSSrv_processHciEvent(uint16_t hciEvt, uint16_t hciEvtSz, uint8_t *pEvtData);
extern uint8 RTLSSrv_processPeriodicAdvEvent(void *pMsg);
extern uint8 LL_EnhancedCteRxTest( uint8 rxChan,
                                        uint8 rxPhy,
                                        uint8 modIndex,
                                        uint8 expectedCteLength,
                                        uint8 expectedCteType,
                                        uint8 slotDurations,
                                        uint8 length,
                                        uint8 *pAntenna);

extern uint8 LL_EnhancedCteTxTest( uint8 txChan,
                                        uint8 payloadLen,
                                        uint8 payloadType,
                                        uint8 txPhy,
                                        uint8 cteLength,
                                        uint8 cteType,
                                        uint8 length,
                                        uint8 *pAntenna);

extern uint8 LL_DirectCteTestTxTest( uint8 txChan,
                                          uint8 payloadLen,
                                          uint8 payloadType,
                                          uint8 txPhy,
                                          uint8 cteLength,
                                          uint8 cteType,
                                          uint8 length,
                                          uint8 *pAntenna);

extern uint8 LL_DirectCteTestRxTest( uint8 rxChan,
                                          uint8 rxPhy,
                                          uint8 modIndex,
                                          uint8 expectedCteLength,
                                          uint8 expectedCteType,
                                          uint8 slotDurations,
                                          uint8 length,
                                          uint8 *pAntenna);

uint8 MAP_LL_DirectCteTestTxTest( uint8 txChan,
                                       uint8 payloadLen,
                                       uint8 payloadType,
                                       uint8 txPhy,
                                       uint8 cteLength,
                                       uint8 cteType,
                                       uint8 length,
                                       uint8 *pAntenna)
{
#ifdef RTLS_CTE_TEST
  return LL_DirectCteTestTxTest( txChan,
                                 payloadLen,
                                 payloadType,
                                 txPhy,
                                 cteLength,
                                 cteType,
                                 length,
                                 pAntenna);
#else
  return (LL_STATUS_SUCCESS);
#endif

}




uint8 MAP_LL_DirectCteTestRxTest( uint8 rxChan,
                                       uint8 rxPhy,
                                       uint8 modIndex,
                                       uint8 expectedCteLength,
                                       uint8 expectedCteType,
                                       uint8 slotDurations,
                                       uint8 length,
                                       uint8 *pAntenna)
{
#ifdef RTLS_CTE_TEST
  return LL_DirectCteTestRxTest( rxChan,
                                 rxPhy,
                                 modIndex,
                                 expectedCteLength,
                                 expectedCteType,
                                 slotDurations,
                                 length,
                                 pAntenna);
#else
  return (LL_STATUS_SUCCESS);
#endif
}


uint8 MAP_LL_EnhancedCteTxTest( uint8 txChan,
                                     uint8 payloadLen,
                                     uint8 payloadType,
                                     uint8 txPhy,
                                     uint8 cteLength,
                                     uint8 cteType,
                                     uint8 length,
                                     uint8 *pAntenna)
{
#ifdef RTLS_CTE_TEST
  return LL_EnhancedCteTxTest (txChan,
                               payloadLen,
                               payloadType,
                               txPhy,
                               cteLength,
                               cteType,
                               length,
                               pAntenna);
#else
  return (LL_STATUS_ERROR_COMMAND_DISALLOWED);
#endif
}

uint8 MAP_LL_EnhancedCteRxTest( uint8 rxChan,
                                     uint8 rxPhy,
                                     uint8 modIndex,
                                     uint8 expectedCteLength,
                                     uint8 expectedCteType,
                                     uint8 slotDurations,
                                     uint8 length,
                                     uint8 *pAntenna)
{
#ifdef RTLS_CTE_TEST
  return LL_EnhancedCteRxTest (rxChan,
                               rxPhy,
                               modIndex,
                               expectedCteLength,
                               expectedCteType,
                               slotDurations,
                               length,
                               pAntenna);
#else
  return (LL_STATUS_ERROR_COMMAND_DISALLOWED);
#endif
}

void MAP_llSetRfReportAodPackets( void )
{
#ifdef RTLS_CTE_TEST
  llSetRfReportAodPackets();
#endif
}

uint8 MAP_llGetCteInfo( uint8 id, void *ptr )
{
#ifdef RTLS_CTE
  return llGetCteInfo(id, ptr);
#else
  return 1;
#endif
}

uint8 MAP_RTLSSrv_processHciEvent(uint16_t hciEvt, uint16_t hciEvtSz, uint8_t *pEvtData)
{
#ifdef RTLS_CTE
  return RTLSSrv_processHciEvent(hciEvt, hciEvtSz, pEvtData);
#else
  return 1; //safeToDealloc
#endif
}

uint8 MAP_LL_SetConnectionCteReceiveParams( uint16 connHandle, uint8 samplingEnable,
                                            uint8 slotDurations, uint8 length, uint8 *pAntenna )
{
#ifdef RTLS_CTE
  return LL_SetConnectionCteReceiveParams( connHandle, samplingEnable, slotDurations, length, pAntenna );
#else
  return 1;
#endif
}

uint8 MAP_LL_SetConnectionCteTransmitParams( uint16 connHandle, uint8  types,
                                             uint8 length, uint8 *pAntenna )
{
#ifdef RTLS_CTE
  return LL_SetConnectionCteTransmitParams( connHandle, types, length, pAntenna );
#else
  return 1;
#endif
}

uint8 MAP_LL_SetConnectionCteRequestEnable( uint16 connHandle, uint8 enable,
                                             uint16 interval, uint8 length, uint8 type )
{
#ifdef RTLS_CTE
  return LL_SetConnectionCteRequestEnable( connHandle, enable, interval, length, type );
#else
  return 1;
#endif
}

uint8 MAP_LL_SetConnectionCteResponseEnable( uint16 connHandle, uint8 enable )
{
#ifdef RTLS_CTE
  return LL_SetConnectionCteResponseEnable( connHandle, enable );
#else
  return 1;
#endif
}

uint8 MAP_LL_ReadAntennaInformation( uint8 *sampleRates, uint8 *maxNumOfAntennas,
                                     uint8 *maxSwitchPatternLen, uint8 *maxCteLen)
{
#ifdef RTLS_CTE
  return LL_ReadAntennaInformation( sampleRates, maxNumOfAntennas, maxSwitchPatternLen, maxCteLen );
#else
  return 1;
#endif
}

void MAP_llUpdateCteState( void *connPtr )
{
#ifdef RTLS_CTE
  llUpdateCteState(connPtr);
#endif
}

uint8 MAP_llSetupCte( void *connPtr, uint8 req)
{
#ifdef RTLS_CTE
  return llSetupCte( connPtr, req );
#else
  return 0;
#endif
}

uint8 MAP_llFreeCteSamplesEntryQueue( void )
{
#ifdef RTLS_CTE
  return llFreeCteSamplesEntryQueue();
#else
  return 0;
#endif
}

uint8 MAP_LL_EXT_SetLocationingAccuracy( uint16 handle, uint8  sampleRate1M, uint8  sampleSize1M,
                                         uint8  sampleRate2M, uint8  sampleSize2M, uint8  sampleCtrl)
{
#ifdef RTLS_CTE
  return LL_EXT_SetLocationingAccuracy( handle, sampleRate1M, sampleSize1M,
                                        sampleRate2M, sampleSize2M, sampleCtrl);
#else
  return 1;
#endif
}

void MAP_HCI_CteRequestFailedEvent( uint8  status, uint16 connHandle)
{
#ifdef RTLS_CTE
  HCI_CteRequestFailedEvent(status,connHandle);
#endif
}

/*******************************************************************************
 * DMM hooks
 */
extern void   llDmmSetThreshold(uint8 state, uint8 handle, uint8 reset);
extern uint32 llDmmGetActivityIndex(uint16 cmdNum);
extern uint8  llDmmSetAdvHandle(uint8 handle, uint8 clear);
extern void   llDmmDynamicFree(void);
extern uint8  llDmmDynamicAlloc(void);
extern uint32_t LL_AbortedCback( uint8 preempted );
extern uint32_t LL_AbortedCback_all( uint8 preempted );
extern uint32_t LL_AbortedCback_peripheral( uint8 preempted );
extern uint32_t LL_AbortedCback_broadcaster( uint8 preempted );
extern uint32_t LL_AbortedCback_central( uint8 preempted );

void MAP_llDmmSetThreshold( uint8 state, uint8 handle, uint8 reset )
{
#ifdef USE_DMM
  llDmmSetThreshold(state, handle, reset);
#endif
}

uint32 MAP_llDmmGetActivityIndex( uint16 cmdNum )
{
#ifdef USE_DMM
  return llDmmGetActivityIndex(cmdNum);
#else
  return 0;
#endif
}

uint8 MAP_llDmmSetAdvHandle( uint8 handle, uint8 clear )
{
#ifdef USE_DMM
  return llDmmSetAdvHandle(handle, clear);
#else
  return 0;
#endif
}

void MAP_llDmmDynamicFree( void )
{
#ifdef USE_DMM
  llDmmDynamicFree();
#endif
}

uint8 MAP_llDmmDynamicAlloc( void )
{
#ifdef USE_DMM
  return llDmmDynamicAlloc();
#else
  return 0;
#endif
}

uint32_t MAP_LL_AbortedCback( uint8 preempted )
{
#if ( CTRL_CONFIG == ADV_NCONN_CFG )
  return (LL_AbortedCback_broadcaster(preempted));
#elif ( CTRL_CONFIG == (ADV_NCONN_CFG | ADV_CONN_CFG) )
  return (LL_AbortedCback_peripheral(preempted));
#elif ( CTRL_CONFIG == (SCAN_CFG | INIT_CFG) )
  return (LL_AbortedCback_central(preempted));
#else
  return (LL_AbortedCback_all(preempted));
#endif
}

uint8 MAP_llSetStarvationMode(uint16 connId, uint8 setOnOffValue)
{
#if defined ( USE_DMM ) || defined ( NO_QOS )
  return LL_INACTIVE_CONNECTIONS;
#else
  return llSetStarvationMode(connId, setOnOffValue);
#endif
}

void MAP_llMaster_TaskEnd(void)
{
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
  llMaster_TaskEnd();
#endif
  return;
}

void MAP_llSlave_TaskEnd(void)
{
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG)
  llSlave_TaskEnd();
#endif
  return;
}

void MAP_llInitFeatureSet( void )
{
  llInitFeatureSet();
#ifdef EXCLUDE_AE
  MAP_llRemoveFromFeatureSet(LL_FEATURE_EXTENDED_ADVERTISING);
#endif
}

/*******************************************************************************
 * Coex hooks
 */
extern llCoexParams_t *llCoexGetParams(uint16 cmdNum);

void *MAP_llCoexGetParams(uint16 cmdNum)
{
#ifdef USE_COEX
  return llCoexGetParams(cmdNum);
#else
  return NULL;
#endif
}

/*******************************************************************************
 * Periodic Adv hooks
 */
uint8 MAP_LE_SetPeriodicAdvParams( uint8 advHandle,
                                   uint16 periodicAdvIntervalMin,
                                   uint16 periodicAdvIntervalMax,
                                   uint16 periodicAdvProp )
{
#ifdef USE_PERIODIC_ADV
  return LE_SetPeriodicAdvParams(advHandle,
                                 periodicAdvIntervalMin,
                                 periodicAdvIntervalMax,
                                 periodicAdvProp);
#else
  return 1;
#endif
}

uint8 MAP_LE_SetPeriodicAdvData( uint8 advHandle, uint8 operation,
                                 uint8 dataLength, uint8 *data )
{
#ifdef USE_PERIODIC_ADV
  return LE_SetPeriodicAdvData( advHandle,operation,dataLength,data );
#else
  return 1;
#endif
}

uint8 MAP_LE_SetPeriodicAdvEnable( uint8 enable,uint8 advHandle )
{
#ifdef USE_PERIODIC_ADV
  return LE_SetPeriodicAdvEnable( enable, advHandle);
#else
  return 1;
#endif
}

uint8 MAP_LE_SetConnectionlessCteTransmitParams( uint8 advHandle, uint8 cteLen, uint8 cteType,
                                                 uint8 cteCount, uint8 length, uint8 *pAntenna )
{
#if defined ( USE_PERIODIC_ADV ) && defined ( RTLS_CTE )
  return LE_SetConnectionlessCteTransmitParams( advHandle, cteLen, cteType, cteCount, length, pAntenna );
#else
  return 1;
#endif
}

uint8 MAP_LE_SetConnectionlessCteTransmitEnable( uint8 advHandle, uint8 enable )
{
#if defined ( USE_PERIODIC_ADV ) && defined ( RTLS_CTE )
  return LE_SetConnectionlessCteTransmitEnable( advHandle, enable);
#else
  return 1;
#endif
}

void *MAP_llGetPeriodicAdv( uint8 handle )
{
#ifdef USE_PERIODIC_ADV
  return llGetPeriodicAdv( handle );
#else
  return NULL;
#endif
}

void MAP_llUpdatePeriodicAdvChainPacket( void )
{
#ifdef USE_PERIODIC_ADV
  llUpdatePeriodicAdvChainPacket();
#endif
}

void MAP_llSetPeriodicAdvChmapUpdate( uint8 set )
{
#ifdef USE_PERIODIC_ADV
  llSetPeriodicAdvChmapUpdate( set );
#endif
}

void MAP_llPeriodicAdv_PostProcess( void )
{
#ifdef USE_PERIODIC_ADV
  llPeriodicAdv_PostProcess();
#endif
}

uint8 MAP_llTrigPeriodicAdv( void *pAdvSet, void *pPeriodicAdv )
{
#ifdef USE_PERIODIC_ADV
  return llTrigPeriodicAdv( pAdvSet, pPeriodicAdv);
#else
  return 0;
#endif
}

uint8 MAP_llSetupPeriodicAdv( void *pAdvSet )
{
#ifdef USE_PERIODIC_ADV
  return llSetupPeriodicAdv( pAdvSet );
#else
  return 0;
#endif
}

void MAP_llEndPeriodicAdvTask( void *pPeriodicAdv )
{
#ifdef USE_PERIODIC_ADV
  llEndPeriodicAdvTask( pPeriodicAdv );
#endif
}

void *MAP_llFindNextPeriodicAdv( void )
{
#ifdef USE_PERIODIC_ADV
  return llFindNextPeriodicAdv();
#else
  return NULL;
#endif
}

void MAP_llSetPeriodicSyncInfo( void *pAdvSet, uint8 *pBuf )
{
#ifdef USE_PERIODIC_ADV
  llSetPeriodicSyncInfo(pAdvSet,pBuf);
#endif
}

void *MAP_llGetCurrentPeriodicAdv( void )
{
#ifdef USE_PERIODIC_ADV
  return llGetCurrentPeriodicAdv();
#else
  return NULL;
#endif
}

void MAP_llPeriodicAdv_Config( void *pAdvSet )
{
#ifdef USE_PERIODIC_ADV
  llPeriodicAdv_Config( pAdvSet );
#endif
}

void MAP_llClearPeriodicAdvSets( void )
{
#ifdef USE_PERIODIC_ADV
  llClearPeriodicAdvSets();
#endif
}

uint8 MAP_gapAdv_periodicAdvCmdCompleteCBs( void *pMsg )
{
#ifdef USE_PERIODIC_ADV
  return gapAdv_periodicAdvCmdCompleteCBs(pMsg);
#else
  return TRUE;
#endif
}

/*******************************************************************************
 * Periodic Scan hooks
 */
extern uint8 llProcessExtScanRxFIFO_hook(void);
extern void llProcessPeriodicScanSyncInfo( uint8 *pPkt, aeExtAdvRptEvt_t *advEvent, uint32 timeStamp, uint8 phy );
extern ble5OpCmd_t *llFindNextPeriodicScan( void );
extern void llUpdateExtScanAcceptSyncInfo( void );

uint8 MAP_LE_PeriodicAdvCreateSync( uint8  options, uint8  advSID, uint8  advAddrType, uint8  *advAddress,
                                    uint16 skip, uint16 syncTimeout, uint8  syncCteType )
{
#ifdef USE_PERIODIC_SCAN
  return LE_PeriodicAdvCreateSync( options, advSID, advAddrType, advAddress, skip,syncTimeout,syncCteType);
#else
  return 1;
#endif
}

uint8 MAP_LE_PeriodicAdvCreateSyncCancel( void )
{
#ifdef USE_PERIODIC_SCAN
  return LE_PeriodicAdvCreateSyncCancel();
#else
  return 1;
#endif
}

uint8 MAP_LE_PeriodicAdvTerminateSync( uint16 syncHandle )
{
#ifdef USE_PERIODIC_SCAN
  return LE_PeriodicAdvTerminateSync( syncHandle );
#else
  return 1;
#endif
}

uint8 MAP_LE_AddDeviceToPeriodicAdvList( uint8 advAddrType, uint8 *advAddress, uint8 advSID )
{
#ifdef USE_PERIODIC_SCAN
  return LE_AddDeviceToPeriodicAdvList( advAddrType, advAddress, advSID );
#else
  return 1;
#endif
}

uint8 MAP_LE_RemoveDeviceFromPeriodicAdvList( uint8 advAddrType, uint8 *advAddress, uint8 advSID )
{
#ifdef USE_PERIODIC_SCAN
  return LE_RemoveDeviceFromPeriodicAdvList( advAddrType, advAddress, advSID);
#else
  return 1;
#endif
}

uint8 MAP_LE_ClearPeriodicAdvList( void )
{
#ifdef USE_PERIODIC_SCAN
  return LE_ClearPeriodicAdvList();
#else
  return 1;
#endif
}

uint8 MAP_LE_ReadPeriodicAdvListSize( uint8 *listSize )
{
#ifdef USE_PERIODIC_SCAN
  return LE_ReadPeriodicAdvListSize( listSize );
#else
  return 1;
#endif
}

uint8 MAP_LE_SetPeriodicAdvReceiveEnable( uint16 syncHandle, uint8  enable )
{
#ifdef USE_PERIODIC_SCAN
  return LE_SetPeriodicAdvReceiveEnable( syncHandle, enable);
#else
  return 1;
#endif
}

uint8 MAP_LE_SetConnectionlessIqSamplingEnable( uint16 syncHandle, uint8 samplingEnable,
                                                uint8 slotDurations, uint8 maxSampledCtes,
                                                uint8 length, uint8 *pAntenna )
{
#if defined ( USE_PERIODIC_SCAN ) && defined ( RTLS_CTE )
  return LE_SetConnectionlessIqSamplingEnable( syncHandle, samplingEnable, slotDurations, maxSampledCtes, length, pAntenna );
#else
  return 1;
#endif
}


uint8 MAP_llProcessExtScanRxFIFO_hook(void)
{
#ifdef USE_PERIODIC_SCAN
  return llProcessExtScanRxFIFO_hook();
#else
  return 0;
#endif
}

void MAP_llProcessPeriodicScanSyncInfo( uint8 *pPkt, void *advEvent, uint32 timeStamp, uint8 phy )
{
#ifdef USE_PERIODIC_SCAN
  llProcessPeriodicScanSyncInfo( pPkt, advEvent, timeStamp, phy );
#endif
}

void MAP_llEndPeriodicScanTask( void *pPeriodicScan )
{
#ifdef USE_PERIODIC_SCAN
  llEndPeriodicScanTask( pPeriodicScan );
#endif
}

void MAP_llPeriodicScan_PostProcess( void )
{
#ifdef USE_PERIODIC_SCAN
  llPeriodicScan_PostProcess();
#endif
}

void MAP_llProcessPeriodicScanRxFIFO( void )
{
#ifdef USE_PERIODIC_SCAN
  llProcessPeriodicScanRxFIFO();
#endif
}

void *MAP_llFindNextPeriodicScan( void )
{
#ifdef USE_PERIODIC_SCAN
  return llFindNextPeriodicScan();
#else
  return NULL;
#endif
}

void MAP_llTerminatePeriodicScan( void )
{
#ifdef USE_PERIODIC_SCAN
  llTerminatePeriodicScan();
#endif
}

void *MAP_llGetPeriodicScan( uint16 handle )
{
#ifdef USE_PERIODIC_SCAN
  return llGetPeriodicScan(handle);
#else
  return NULL;
#endif
}

void *MAP_llGetCurrentPeriodicScan( uint8 state )
{
#ifdef USE_PERIODIC_SCAN
  return llGetCurrentPeriodicScan(state);
#else
  return NULL;
#endif
}

uint8 MAP_llGetPeriodicScanCteTasks( void )
{
#ifdef USE_PERIODIC_SCAN
  return llGetPeriodicScanCteTasks();
#else
  return 0;
#endif
}

uint8_t MAP_gapScan_periodicAdvCmdCompleteCBs( void *pMsg )
{
#ifdef USE_PERIODIC_SCAN
  #ifdef USE_PERIODIC_RTLS
    hciEvt_CmdComplete_t *pEvt = (hciEvt_CmdComplete_t *)pMsg;
    return RTLSSrv_processHciEvent(pEvt->cmdOpcode, sizeof(pEvt->pReturnParam), pEvt->pReturnParam);
  #else
    return gapScan_periodicAdvCmdCompleteCBs(pMsg);
  #endif
#else
  return TRUE;
#endif
}

uint8_t MAP_gapScan_periodicAdvCmdStatusCBs( void *pMsg )
{
#ifdef USE_PERIODIC_SCAN
  #ifdef USE_PERIODIC_RTLS
	hciEvt_CommandStatus_t *pEvt = (hciEvt_CommandStatus_t *)pMsg;
	return RTLSSrv_processHciEvent(pEvt->cmdOpcode, sizeof(pEvt->cmdStatus), &pEvt->cmdStatus);
 #else
    return gapScan_periodicAdvCmdStatusCBs(pMsg);
 #endif
#else
  return TRUE;
#endif
}

uint8_t MAP_gapScan_processBLEPeriodicAdvCBs( void *pMsg )
{
#ifdef USE_PERIODIC_SCAN
  #ifdef USE_PERIODIC_RTLS
    return RTLSSrv_processPeriodicAdvEvent(pMsg);
  #else
    return gapScan_processBLEPeriodicAdvCBs(pMsg);
  #endif
#else
  return TRUE;
#endif
}

void MAP_llClearPeriodicScanSets( void )
{
#ifdef USE_PERIODIC_SCAN
  llClearPeriodicScanSets();
#endif
}

void MAP_llUpdateExtScanAcceptSyncInfo( void )
{
#ifdef USE_PERIODIC_SCAN
  llUpdateExtScanAcceptSyncInfo();
#endif
}

/**
* These hooks created to change the call to the relevant HCI command
* instead of calling the controller directly.
* This is needed to support the relevant command complete events
* that are passed to the application when using BLE3_CMD
* compilation flag
*/
uint8_t LE_SetExtAdvData_hook( void * pMsg )
{
#ifdef BLE3_CMD
  return HCI_LE_SetExtAdvData(pMsg);
#else
  return LE_SetExtAdvData(pMsg);
#endif
}
uint8_t LE_SetExtScanRspData_hook( void * pMsg)
{
#ifdef BLE3_CMD
  return HCI_LE_SetExtScanRspData(pMsg);
#else
  return LE_SetExtScanRspData(pMsg);
#endif
}

uint8_t LE_SetExtAdvEnable_hook( void * pMsg)
{
#ifdef BLE3_CMD
  return HCI_LE_SetAdvStatus(pMsg);
#else
  return LE_SetExtAdvEnable(pMsg);
#endif
}

uint8 MAP_gapAdv_handleAdvHciCmdComplete( void *pMsg )
{
#ifdef BLE3_CMD
  return gapAdv_handleAdvHciCmdComplete(pMsg);
#else
  return TRUE;
#endif
}

/*******************************************************************************
 * Single Connection
 */
extern uint8 llGetSingleConn( void );
extern uint32 llCalcSingleConnMaxTimeLength(uint16 startConnId, uint16 bestSelectedConnIdAfterStart);
extern uint32 llCalcConnMaxTimeLength(uint16 startConnId, uint16 bestSelectedConnIdAfterStart);

uint8 llGetNextConn_hook( void )
{
#if defined ONE_CONN
  return llGetSingleConn();
#else
  return llGetNextConn();
#endif
}

uint32 MAP_llCalcConnMaxTimeLength(uint16 startConnId, uint16 bestSelectedConnIdAfterStart)
{
#if defined ONE_CONN
  return llCalcSingleConnMaxTimeLength(startConnId, bestSelectedConnIdAfterStart);
#else
  return llCalcConnMaxTimeLength(startConnId, bestSelectedConnIdAfterStart);
#endif
}

uint8 llFindStartType_hook( void *secTask, void *primTask )
{
#if defined NO_QOS
  return llFindStartTypeNoQos( secTask, primTask );
#else
  return llFindStartType( secTask, primTask );
#endif
}

/******************************************************************************
* Legacy Adv Only
*/
extern void llUpdateExtAdvTx( advSet_t *pAdvSet );
extern void llPrepareNextExtAdv( advSet_t *pAdvSet );

uint8 llSetupExtAdv_hook( void *pAdvSet )
{
#ifndef EXCLUDE_AE
  return llSetupExtAdv( pAdvSet );
#else
  return LL_STATUS_ERROR_FEATURE_NOT_SUPPORTED;
#endif
}

uint8 llNextChanIndex_hook( uint16 eventCounter )
{
#ifndef EXCLUDE_AE
  return llNextChanIndex( eventCounter );
#else
  return 0xFF;
#endif
}

void MAP_llUpdateExtAdvTx( void *pAdvSet )
{
#ifndef EXCLUDE_AE
  llUpdateExtAdvTx( pAdvSet );
#endif
}

void MAP_llPrepareNextExtAdv( void *pAdvSet )
{
#ifndef EXCLUDE_AE
  llPrepareNextExtAdv( pAdvSet );
#endif
}

/*******************************************************************************
 * Secure Manager hooks
 */
extern uint8 smpResponderProcessIncoming_hook( linkDBItem_t *pLinkItem, uint8 cmdID, smpMsgs_t *pParsedMsg );

void SM_dhKeyCB_hook( void *pDhKey )
{
#ifndef EXCLUDE_SM
  SM_dhKeyCB( pDhKey );
#endif
}

void SM_p256KeyCB_hook( void *pK, uint8 *privateKey )
{
#ifndef EXCLUDE_SM
  SM_p256KeyCB( pK, privateKey );
#endif
}

uint8 smpInitiatorProcessPairingRandom_hook( void *pParsedMsg, uint16_t connHandle)
{
#ifndef EXCLUDE_SM
  return smpInitiatorProcessPairingRandom( pParsedMsg, connHandle);
#else
  return SMP_PAIRING_FAILED_CMD_NOT_SUPPORTED;
#endif
}

uint8 smpResponderProcessPairingPublicKey_hook( void *pParsedMsg )
{
#ifndef EXCLUDE_SM
  return smpResponderProcessPairingPublicKey( pParsedMsg );
#else
  return SMP_PAIRING_FAILED_CMD_NOT_SUPPORTED;
#endif
}

uint8 smResponderProcessLTKReq_hook( uint16 connectionHandle, uint8 *pRandom, uint16 encDiv )
{
#ifndef EXCLUDE_SM
  return smResponderProcessLTKReq( connectionHandle, pRandom, encDiv );
#else
  return TRUE;
#endif
}

uint8 smpResponderProcessIncoming_hook( linkDBItem_t *pLinkItem, uint8 cmdID, smpMsgs_t *pParsedMsg )
{
#ifndef EXCLUDE_SM
  return smpResponderProcessIncoming_sPatch( pLinkItem, cmdID, pParsedMsg );
#else
  // This will cause the responder to send pairing fail response
  return SMP_PAIRING_FAILED_NOT_SUPPORTED;
#endif
}

uint8 smGeneratePairingReqRsp_hook( void )
{
#ifndef EXCLUDE_SM
  return smGeneratePairingReqRsp();
#else
  return bleIncorrectMode;
#endif
}

void gapSendSlaveSecurityReqEvent_hook( uint8 taskID, uint16 connHandle, uint8 *pDevAddr, uint8 authReq )
{
#ifndef EXCLUDE_SM
  gapSendSlaveSecurityReqEvent(taskID, connHandle, pDevAddr, authReq);
#else
  // The initiator shall respnd with pairing failed command
  MAP_GAP_TerminateAuth(connHandle, SMP_PAIRING_FAILED_NOT_SUPPORTED);
#endif
}
/*******************************************************************************
 */
