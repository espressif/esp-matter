/******************************************************************************

 @file  icall_api.c

 @brief BLE stack C interface implementation on top of dispatcher messaging
        interface.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2013-2022, Texas Instruments Incorporated
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

/*******************************************************************************
 * INCLUDES
 */
#include <string.h>

#include <icall.h>

#include "gap.h"
#include "sm.h"
#include "linkdb.h"
#include "gatt.h"
#include "l2cap.h"
#include "hci_tl.h"

#include "gattservapp.h"
#include "gapgattserver.h"
#include "gapbondmgr.h"

#include "osal_snv.h"

#include "hci.h"
#include "hci_ext.h"
#include "icall_apimsg.h"
#include "ble_dispatch.h"

/*********************************************************************
 * CONSTANTS
 */

#define HCI_NO_PARAM 0

/*********************************************************************
 * GLOBAL VARIABLES
 */

uint16 lastAppOpcodeSent = 0xFFFF;

// BM allocator and de-allocator functions
pfnBMAlloc_t pfnBMAlloc = NULL;
pfnBMFree_t  pfnBMFree  = NULL;

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t getStatusValueFromErrNo(ICall_Errno errno);

static void setICallCmdEvtHdr(ICall_HciExtCmd *pHdr, uint8_t subgrp,
                              uint8_t cmdId);
static void setDispatchCmdEvtHdr(ICall_HciExtCmd *pHdr, uint8_t subgrp,
                                 uint8_t cmdId);
static ICall_Errno waitMatchCS(ICall_MsgMatchFn matchCSFn, void **msg);
static bStatus_t sendWaitMatchCS(ICall_EntityID src, void *msg,
                                 ICall_MsgMatchFn matchCSFn);
static bStatus_t sendWaitMatchValueCS(ICall_EntityID src, void *msg,
                                      ICall_MsgMatchFn matchCSFn, uint8_t len,
                                      uint8_t *pValue);
static void registerTask(uint8 taskID, uint8_t subgrp, uint8_t cmdId);

static bStatus_t gattRequest(uint16 connHandle, attMsg_t *pReq,
                             uint8 taskId, uint8 opcode);
static bStatus_t gattWriteLong(uint16 connHandle, attPrepareWriteReq_t *pReq,
                               uint8 taskId, uint8 opcode);
static bStatus_t gattIndNoti(uint16 connHandle, attMsg_t *pIndNoti,
                             uint8 authenticated, uint8 taskId, uint8 opcode);
static bStatus_t profileAddService(uint32 services, uint8 profileId,
                                   uint8 serviceId);
static bStatus_t gattAddService(uint32 services, uint8 cmdId);
static bStatus_t profileSetParameter(uint8 param, uint8 len, void *pValue,
                                     uint8_t subgrp, uint8_t cmdId,
                                     ICall_MsgMatchFn matchCSFn);
static bStatus_t profileGetParameter(uint8 param, void *pValue, uint8_t subgrp,
                                     uint8_t cmdId, ICall_MsgMatchFn matchCSFn);

static bool matchProfileCS(ICall_ServiceEnum src, ICall_EntityID dest,
                           const void *msg, uint8 profileId, uint8 cmdId);
static bool matchProfileSetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg, uint8 profileId);
static bool matchProfileGetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg, uint8 profileId);
static bool matchGGSSetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg);
static bool matchGGSGetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg);
static bool matchGSASetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg);
static bool matchGSAGetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg);
static bool matchProfileAddServiceCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                     const void *msg);

static bStatus_t gapSendParamsCmd(uint16_t connHandle, uint8_t param,
                                  uint8_t cmdId, ICall_MsgMatchFn matchCSFn);
static bStatus_t gapSendPtrParamsCmd(uint8_t *pParam1, uint8_t *pParam2,
                                     uint8_t cmdId, ICall_MsgMatchFn matchCSFn);
static bStatus_t gapSendParamAndPtrCmd(uint8_t taskID, uint8_t *pParam,
                                       uint8_t cmdId, ICall_MsgMatchFn matchCSFn);
static bool matchGapSetGetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                  const void *msg, uint8 cmdId);
static bool matchGapDeviceInitCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                 const void *msg);
static bool matchGapConfigDeviceAddrCS(ICall_ServiceEnum src,
                                       ICall_EntityID dest, const void *msg);
static bool matchGapUpdateLinkParamReqCS(ICall_ServiceEnum src,
                                         ICall_EntityID dest, const void *msg);
static bool matchGapConnParamUpdateReplyCS(ICall_ServiceEnum src,
                                           ICall_EntityID dest, const void *msg);
static bool matchGapTerminateLinkReqCS(ICall_ServiceEnum src,
                                       ICall_EntityID dest, const void *msg);
static bool matchGapUpdateAdvDataCS(ICall_ServiceEnum src,
                                    ICall_EntityID dest, const void *msg);
static bool matchGapSetAdvTokenCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                  const void *msg);
static bool matchGapRemoveAdvTokenCS(ICall_ServiceEnum src,
                                     ICall_EntityID dest, const void *msg);
static bool matchGapUpdateAdvTokenCS(ICall_ServiceEnum src,
                                     ICall_EntityID dest, const void *msg);
static bool matchGapAuthenticateCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg);
static bool matchGapTerminateAuthCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                    const void *msg);
static bool matchGapBondCS(ICall_ServiceEnum src, ICall_EntityID dest,
                           const void *msg);
static bool matchGapSignableCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg);
static bool matchGapPasskeyUpdateCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                    const void *msg);
static bool matchGapSetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg);
static bool matchGapGetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg);
static bool matchGapResolvePrivateAddrCS(ICall_ServiceEnum src,
                                         ICall_EntityID dest, const void *msg);
static bool matchGapSlaveSecurityReqCS(ICall_ServiceEnum src,
                                       ICall_EntityID dest, const void *msg);

static bool matchLinkDBStateCS(ICall_ServiceEnum src,
                               ICall_EntityID dest, const void *msg);
static bool matchLinkDBNumConnsCS(ICall_ServiceEnum src,
                                  ICall_EntityID dest, const void *msg);
static bool matchLinkDBNumActiveCS(ICall_ServiceEnum src,
                                  ICall_EntityID dest, const void *msg);
static bool matchLinkDBGetInfoCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                 const void *msg);

static bool matchBondMgrSetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg);
static bool matchBondMgrGetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg);
static bool matchBondMgrServiceChangeIndCS(ICall_ServiceEnum src,
                                           ICall_EntityID dest, const void *msg);

static bool matchL2capCmdStatus(ICall_ServiceEnum src, ICall_EntityID dest,
                                const void *msg, uint8_t cmdId);
static bool matchL2capParamUpdateCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                    const void *msg);
static bool matchL2capRegisterPsmCS(ICall_ServiceEnum src,
                                    ICall_EntityID dest, const void *msg);
static bool matchL2capDeregisterPsmCS(ICall_ServiceEnum src,
                                      ICall_EntityID dest, const void *msg);
static bool matchL2capPsmInfoCS(ICall_ServiceEnum src,
                                ICall_EntityID dest, const void *msg);
static bool matchL2capPsmChannelsCS(ICall_ServiceEnum src,
                                    ICall_EntityID dest, const void *msg);
static bool matchL2capChannelInfoCS(ICall_ServiceEnum src,
                                    ICall_EntityID dest, const void *msg);
static bool matchL2capSendSDUCS(ICall_ServiceEnum src,
                                ICall_EntityID dest, const void *msg);
static bool matchL2capConnectReqCS(ICall_ServiceEnum src,
                                   ICall_EntityID dest, const void *msg);
static bool matchL2capConnectRspCS(ICall_ServiceEnum src,
                                   ICall_EntityID dest, const void *msg);
static bool matchL2capDisconnectReqCS(ICall_ServiceEnum src,
                                      ICall_EntityID dest, const void *msg);
static bool matchL2capFCCreditCS(ICall_ServiceEnum src,
                                 ICall_EntityID dest, const void *msg);

static bool matchGattRequestCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg);
static bool matchGattCmdStatus(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg, uint8_t cmdId);
static bool matchGattDiscAllPrimarySvcCS(ICall_ServiceEnum src,
                                         ICall_EntityID dest, const void *msg);
static bool matchGattDiscPrimartSvcCS(ICall_ServiceEnum src,
                                      ICall_EntityID dest, const void *msg);
static bool matchGattFindIncludedSvcCS(ICall_ServiceEnum src,
                                       ICall_EntityID dest, const void *msg);
static bool matchGattDiscAllCharsCS(ICall_ServiceEnum src,
                                    ICall_EntityID dest, const void *msg);
static bool matchGattReliableWritesCS(ICall_ServiceEnum src,
                                      ICall_EntityID dest, const void *msg);
static bool matchGattGetNexthandleCS(ICall_ServiceEnum src,
                                      ICall_EntityID dest, const void *msg);
static bool matchGattWriteLongCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                 const void *msg);
static bool matchGattIndNotiCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg);
static bool matchCccUpdatedEventCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg);
static bStatus_t attSendParamAndPtrCmd(uint8_t opCode, uint16_t connHandle,
                                       attMsg_t *pMsg,
                                       ICall_MsgMatchFn matchCSFn);
static bool matchAttCmdStatus(ICall_ServiceEnum src, ICall_EntityID dest,
                              const void *msg, uint8_t cmdId);
static bool matchAttErrorRspCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg);
static bool matchAttReadRspCS(ICall_ServiceEnum src, ICall_EntityID dest,
                              const void *msg);
static bool matchAttReadBlobRspCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                  const void *msg);
static bool matchAttWriteRspCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg);
static bool matchAttExecuteWriteRspCS(ICall_ServiceEnum src,
                                      ICall_EntityID dest, const void *msg);
static bool matchAttValueHandleCfmCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                     const void *msg);

static bool matchGapPasscodeCS(ICall_ServiceEnum src,
                               ICall_EntityID dest, const void *msg);
static bool matchGapResolveAddrCS(ICall_ServiceEnum src,
                                  ICall_EntityID dest, const void *msg);
static bool matchGapLinkEstCS(ICall_ServiceEnum src,
                              ICall_EntityID dest, const void *msg);
static bool matchGattInitClientCS(ICall_ServiceEnum src,
                                  ICall_EntityID dest, const void *msg);
static bool matchGattSendRspCS(ICall_ServiceEnum src,
                               ICall_EntityID dest, const void *msg);
static bool matchGSARegisterServiceCS(ICall_ServiceEnum src,
                                      ICall_EntityID dest, const void *msg);
static bool matchGSADeregisterServiceCS(ICall_ServiceEnum src,
                                        ICall_EntityID dest, const void *msg);
static bool matchGSAServiceChangedIndCS(ICall_ServiceEnum src,
                                        ICall_EntityID dest, const void *msg);
static bool matchGSAReadRspCS(ICall_ServiceEnum src,
                              ICall_EntityID dest, const void *msg);

static hciStatus_t hciSendCmd(uint16_t opCode, ICall_MsgMatchFn matchCSFn);
static hciStatus_t hciSendParamsCmd(uint16_t opCode,
                                    uint16_t param1,
                                    uint16_t param2,
                                    uint16_t param3,
                                    ICall_MsgMatchFn matchCSFn);
static hciStatus_t hciSendPtrParamsCmd(uint16_t opCode,
                                       uint8_t *pParam1,
                                       uint8_t *pParam2,
                                       uint8_t *pParam3,
                                       ICall_MsgMatchFn matchCSFn);
static hciStatus_t hciSendParamAndPtrCmd(uint16_t opCode,
                                         uint16_t param, uint8_t *pParam,
                                         ICall_MsgMatchFn matchCSFn);
static bool matchHciCS(ICall_ServiceEnum src, ICall_EntityID dest,
                       const void *msg, uint16 cmdId);
static bool matchHciReadRemoteVersionInfoCS(ICall_ServiceEnum src,
                                            ICall_EntityID dest,
                                            const void *msg);
static bool matchHciSetEventMaskCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg);
static bool matchHciSetEventMaskPage2CS(ICall_ServiceEnum src,
                                        ICall_EntityID dest,
                                        const void *msg);
static bool matchHciResetCS(ICall_ServiceEnum src, ICall_EntityID dest,
                            const void *msg);
static bool matchHciReadTxPwrLvlCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg);
static bool matchHciReadLocalVerInfoCS(ICall_ServiceEnum src,
                                       ICall_EntityID dest,
                                       const void *msg);
static bool matchHciReadLocalSupportedCommandsCS(ICall_ServiceEnum src,
                                                 ICall_EntityID dest,
                                                 const void *msg);
static bool matchHciReadLocalSupportedFeaturesCS(ICall_ServiceEnum src,
                                                 ICall_EntityID dest,
                                                 const void *msg);
static bool matchHciReadBdAddrCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                 const void *msg);
static bool matchHciReadRssiCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg);
static bool matchHciLeSetEventMaskCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                     const void *msg);
static bool matchHciLeReadLocalSupportedFeaturesCS(ICall_ServiceEnum src,
                                                   ICall_EntityID dest,
                                                   const void *msg);
static bool matchHciLeReadAdvChanTxPowerCS(ICall_ServiceEnum src,
                                           ICall_EntityID dest,
                                           const void *msg);
static bool matchHciLeReadWhiteListSizeCS(ICall_ServiceEnum src,
                                          ICall_EntityID dest,
                                          const void *msg);
static bool matchHciLeClearWhiteListCS(ICall_ServiceEnum src,
                                       ICall_EntityID dest,
                                       const void *msg);
static bool matchHciLeAddWhiteListCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                     const void *msg);
static bool matchHciLeRemoveWhiteListCS(ICall_ServiceEnum src,
                                        ICall_EntityID dest,
                                        const void *msg);
static bool matchHciLeSetHostChanClassificationCS(ICall_ServiceEnum src,
                                                  ICall_EntityID dest,
                                                  const void *msg);
static bool matchHciLeReadRemoteUsedFeaturesCS(ICall_ServiceEnum src,
                                               ICall_EntityID dest,
                                               const void *msg);
static bool matchHciLeReadChannelMapCS(ICall_ServiceEnum src,
                                       ICall_EntityID dest,
                                       const void *msg);
static bool matchHciLeEncryptCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                const void *msg);
static bool matchHciLeReadSupportedStatesCS(ICall_ServiceEnum src,
                                            ICall_EntityID dest,
                                            const void *msg);
static bool matchHciLeReadMaxDataLenCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                       const void *msg);
static bool matchHciLeReadSuggestedDefaultDataLenCS(ICall_ServiceEnum src,
                                                    ICall_EntityID dest,
                                                    const void *msg);
static bool matchHciLeWriteSuggestedDefaultDataLenCS(ICall_ServiceEnum src,
                                                     ICall_EntityID dest,
                                                     const void *msg);
static bool matchHciLeSetDataLenCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg);
static bool matchHciLeTxTestCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg);
static bool matchHciLeRxTestCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg);
static bool matchHciLeTestEndCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                const void *msg);
static bool matchHciExtSetTxPowerCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg);
static bool matchHciExtOnePktPerEvtCS(ICall_ServiceEnum src,
                                      ICall_EntityID dest, const void *msg);
static bool matchHciExtDecryptCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                 const void *msg);
static bool matchHciExtSetLocalFeatsCS(ICall_ServiceEnum src,
                                       ICall_EntityID dest,
                                       const void *msg);
static bool matchHciExtSetFastTxRspTimeCS(ICall_ServiceEnum src,
                                          ICall_EntityID dest,
                                          const void *msg);
static bool matchHciExtSetSLOverrideCS(ICall_ServiceEnum src,
                                       ICall_EntityID dest,
                                       const void *msg);
static bool matchHciExtModemTestTxCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                     const void *msg);
static bool matchHciExtModemHopTestTxCS(ICall_ServiceEnum src,
                                        ICall_EntityID dest,
                                        const void *msg);
static bool matchHciExtModemTestRxCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                     const void *msg);
static bool matchHciExtEndModemTestCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                      const void *msg);
static bool matchHciExtSetBdAddrCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg);
static bool matchHciExtSetScaCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                const void *msg);
static bool matchHciExtEnablePTMCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg);
static bool matchHciExtSetMaxDtmTxPwrCS(ICall_ServiceEnum src,
                                        ICall_EntityID dest, const void *msg);
static bool matchHciExtDisconnectImmedCS(ICall_ServiceEnum src,
                                         ICall_EntityID dest, const void *msg);
static bool matchHciExtPacketErrorRateCS(ICall_ServiceEnum src,
                                         ICall_EntityID dest, const void *msg);
static bool matchHciExtPERByChanCS(ICall_ServiceEnum src,
                                   ICall_EntityID dest, const void *msg);
static bool matchHciExtBuildRevCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                  const void *msg);
static bool matchHciExtDelaySleepCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                    const void *msg);
static bool matchHciExtResetSystemCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                     const void *msg);
static bool matchHciExtNumComplPktsLimitCS(ICall_ServiceEnum src,
                                           ICall_EntityID dest,
                                           const void *msg);
static bool matchHciExtGetConnInfoCS(ICall_ServiceEnum src,
                                     ICall_EntityID dest,
                                     const void *msg);

static bool matchUtilNvCS(ICall_ServiceEnum src, ICall_EntityID dest,
                          const void *msg, uint8 cmdId);
static bool matchUtilNvReadCS(ICall_ServiceEnum src, ICall_EntityID dest,
                              const void *msg);
static bool matchUtilNvWriteCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg);
static bool matchUtilBuildRevCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                const void *msg);
static bool matchUtilGetTRNGCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg);
static bool matchSMGetScConfirmCS(ICall_ServiceEnum src,
                              ICall_EntityID dest, const void *msg);
/*********************************************************************
 * API FUNCTIONS
 */

/******************************************************************************
 * @fn      getStatusValueFromErrNo
 *
 * @brief   Return status values based on ICall return values.
 *
*           Note: It should be used only within this file.
 *
 * @param   errno  ICall error number
 *
 * @return  SUCCESS: When successful.
 *          INVALIDPARAMETER: Invalid parameter.
 *          MSG_BUFFER_NOT_AVAIL: Memory allocation error occurred.
 *          FAILURE: Some other failure.
 */
static bStatus_t getStatusValueFromErrNo(ICall_Errno errno)
{
  /* The following code could be made into a generic return type conversion. */
  if (errno == ICALL_ERRNO_SUCCESS)
  {
    return SUCCESS;
  }

  if (errno == ICALL_ERRNO_NO_RESOURCE)
  {
    return MSG_BUFFER_NOT_AVAIL;
  }

  if (errno == ICALL_ERRNO_INVALID_PARAMETER)
  {
    return INVALIDPARAMETER;
  }

  return FAILURE;
}

/******************************************************************************
 * @fn      setICallCmdEvtHdr
 *
 * @brief   Set the header for ICALL_CMD_EVENT message.
 *
 * @param   pHdr    pointer to header to be set
 * @param   subgrp  subgroup
 * @param   cmdId   command id
 *
 * @return  none
 */
static void setICallCmdEvtHdr(ICall_HciExtCmd *pHdr, uint8_t subgrp,
                              uint8_t cmdId)
{
  pHdr->hdr.event = ICALL_CMD_EVENT;
  pHdr->pktType = 0;
  pHdr->opCode = (VENDOR_SPECIFIC_OGF << 10) | (subgrp << 7) | cmdId;
}

/******************************************************************************
 * @fn      setDispatchCmdEvtHdr
 *
 * @brief   Set the header for DISPATCH_CMD_EVENT message.
 *
 * @param   pHdr    pointer to header to be set
 * @param   subgrp  subgroup
 * @param   cmdId   command id
 *
 * @return  none
 */
static void setDispatchCmdEvtHdr(ICall_HciExtCmd *pHdr, uint8_t subgrp,
                                 uint8_t cmdId)
{
  // Note that srctaskid shall be filled in via sendmsg()
  pHdr->hdr.event = DISPATCH_CMD_EVENT;
  pHdr->pktType = 0;
  pHdr->opCode = subgrp;
  pHdr->cmdId = cmdId;
}

/*********************************************************************
 * @fn      waitMatchCS
 *
 * @brief   Wait for a matching Command Status response.
 *
 * @param   matchCSFn  pointer to a function that would return TRUE when
 *                     the message matches its condition.
 * @param   msg  pointer to a pointer variable to store the starting
 *               address of the message body being retrieved. The pointer
 *               must not be NULL.
 *
 * @return  ICALL_ERRNO_SUCCESS when the operation was successful and a
 *          message was retrieved.
 *          ICALL_ERRNO_UNKNOWN_THREAD when this function is called from
 *          a thread which either has not registered an entity, or it has
 *          but it serves the BLE Service Class.
 */
static ICall_Errno waitMatchCS(ICall_MsgMatchFn matchCSFn, void **msg)
{
  if (ICall_threadServes(ICALL_SERVICE_CLASS_BLE))
  {
    /* Blocking ICall is not allowed for BLE Stack thread, and hence
     * it's disabled.
     */
    return ICALL_ERRNO_UNKNOWN_THREAD;
  }

  return ICall_waitMatch(ICALL_TIMEOUT_FOREVER, matchCSFn, NULL, NULL, msg);
}

/*********************************************************************
 * @fn      sendWaitMatchCS
 *
 * @brief   Send a message and wait for a matching Command Status response.
 *
 * @param   src  entity id of the sender of the message
 * @param   msg  pointer to the message body to send.
 * @param   matchCSFn  pointer to a function that would return TRUE when
 *                      the message matches its condition.
 *
 * @return  SUCCESS or FAILURE
 */
static bStatus_t sendWaitMatchCS(ICall_EntityID src, void *msg,
                                 ICall_MsgMatchFn matchCSFn)
{
  ICall_Errno errno;

    // check if this is a command packet, and if so, save the opcode
  if ( ((ICall_HciExtCmd *)msg)->pktType == HCI_CMD_PACKET )
  {
    lastAppOpcodeSent = ((ICall_HciExtCmd *)msg)->opCode;
  }

  /* Send the message */
  errno = ICall_sendServiceMsg(src, ICALL_SERVICE_CLASS_BLE,
                               ICALL_MSG_FORMAT_3RD_CHAR_TASK_ID, msg);

  if (errno == ICALL_ERRNO_SUCCESS)
  {
    ICall_GapCmdStatus *pCmdStatus = NULL;

    errno = waitMatchCS(matchCSFn, (void **)&pCmdStatus);
    if (errno == ICALL_ERRNO_SUCCESS)
    {
      uint8 status = pCmdStatus->hdr.hdr.status;

      // Free command status
      ICall_freeMsg(pCmdStatus);

      return status;
    }
  }

  return getStatusValueFromErrNo(errno);
}

/*********************************************************************
 * @fn      sendWaitMatchValueCS
 *
 * @brief   Send a message and wait for a matching value in Command
 *          Status response.
 *
 * @param   src  entity id of the sender of the message
 * @param   msg  pointer to the message body to send.
 * @param   matchCSFn  pointer to a function that would return TRUE when
 *                      the message matches its condition.
 * @param   len  expected length of value field
 * @param   pValue  pointer to value to be returned
 *
 * @return  SUCCESS or FAILURE
 */
static bStatus_t sendWaitMatchValueCS(ICall_EntityID src, void *msg,
                                      ICall_MsgMatchFn matchCSFn, uint8_t len,
                                      uint8_t *pValue)
{
  ICall_Errno errno;

  /* Send the message */
  errno = ICall_sendServiceMsg(src, ICALL_SERVICE_CLASS_BLE,
                               ICALL_MSG_FORMAT_3RD_CHAR_TASK_ID, msg);

  if (errno == ICALL_ERRNO_SUCCESS)
  {
    ICall_GapCmdStatus *pCmdStatus = NULL;

    errno = waitMatchCS(matchCSFn, (void **)&pCmdStatus);
    if (errno == ICALL_ERRNO_SUCCESS)
    {
      uint8 status = pCmdStatus->hdr.hdr.status;

      if ((status == SUCCESS) && (pCmdStatus->len == len))
      {
        // copy message body
        memcpy(pValue, pCmdStatus->pValue, len);
      }

      // Free command status
      ICall_freeMsg(pCmdStatus);

      return status;
    }
  }

  return getStatusValueFromErrNo(errno);
}

/******************************************************************************
 * @fn      registerTask
 *
 * @brief   Register a task to receive Host messages.
 *
 * @param   taskID  app task to register
 * @param   subgrp  subgroup
 * @param   cmdId   command id
 *
 * @return  none
 */
static void registerTask(uint8 taskID, uint8_t subgrp, uint8_t cmdId)
{
  // Allocate message buffer space
  ICall_RegisterTaskMsg *msg =
    (ICall_RegisterTaskMsg *)ICall_allocMsg(sizeof(ICall_RegisterTaskMsg));

  if (msg)
  {
    setDispatchCmdEvtHdr(&msg->hdr, subgrp, cmdId);

    // Set task id
    msg->taskID = ICall_getLocalMsgEntityId(ICALL_SERVICE_CLASS_BLE_MSG, taskID);

    // Send the message
    ICall_sendServiceMsg(ICall_getEntityId(), ICALL_SERVICE_CLASS_BLE,
                         ICALL_MSG_FORMAT_3RD_CHAR_TASK_ID, msg);
  }
}

/*********************************************************************
 * Register your task ID to receive extra (unprocessed) HCI status and
 * complete, and Host events.
 *
 * Public function defined in gap.h.
 */
void GAP_RegisterForMsgs(uint8 taskID)
{
  registerTask(taskID, DISPATCH_GAP_PROFILE, DISPATCH_GAP_REG_FOR_MSG);
}

/*******************************************************************************
 * @fn          NPI_RegisterTask
 *
 * @brief       This routine sends the taskID for the UART task to NPI
 *
 * input parameters
 *
 * @param       taskID - for the UART app task
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void NPI_RegisterTask(uint8_t taskId)
{
  registerTask(taskId, DISPATCH_GENERAL, DISPATCH_GENERAL_REG_NPI);
}

/*******************************************************************************
 * @fn          L2CAP_RegisterFlowCtrlTask
 *
 * @brief       This routine sends the taskID for the task which wishes to
 *              HCI NumCompletePkt
 *
 * input parameters
 *
 * @param       taskID - for the UART app task
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void L2CAP_RegisterFlowCtrlTask(uint8_t taskId)
{
  registerTask(taskId, DISPATCH_GENERAL, DISPATCH_GENERAL_REG_L2CAP_FC);
}

/*********************************************************************
 * @brief   Register callback functions with the bond manager.
 *
 * Public function defined in gapbondmgr.h.
 */
void GAPBondMgr_Register(gapBondCBs_t *pCB)
{
  // Allocate message buffer space
  ICall_BondMgrRegister *msg =
    (ICall_BondMgrRegister *)ICall_allocMsg(sizeof(ICall_BondMgrRegister));

  if (msg)
  {
    setDispatchCmdEvtHdr(&msg->hdr, DISPATCH_GAP_PROFILE,
                         DISPATCH_PROFILE_REG_CB);
    // set callback
    msg->pCB = pCB;

    // Send the message
    ICall_sendServiceMsg(ICall_getEntityId(), ICALL_SERVICE_CLASS_BLE,
                         ICALL_MSG_FORMAT_3RD_CHAR_TASK_ID, msg);
  }
}

/*********************************************************************
 * @brief   Respond to a passcode request.
 *
 * Public function defined in gapbondmgr.h.
 */
bStatus_t GAPBondMgr_PasscodeRsp(uint16 connectionHandle, uint8 status,
                                 uint32 passcode)
{
  size_t allocSize = sizeof(ICall_BondMgrPasscodeRsp);

  /* Allocate message buffer space */
  ICall_BondMgrPasscodeRsp *msg =
    (ICall_BondMgrPasscodeRsp *)ICall_allocMsg(allocSize);

  if (msg)
  {
    setDispatchCmdEvtHdr(&msg->hdr, DISPATCH_GAP_PROFILE,
                         DISPATCH_GAP_BOND_PASSCODE_RSP);

    // Set service change parameters
    msg->connHandle = connectionHandle;
    msg->status = status;
    msg->passcode = passcode;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchGapPasscodeCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * @brief   Notify the Bond Manager that a Slave Security Request is received.
 *
 * Public function defined in gapbondmgr.h.
 */
uint8 GAPBondMgr_FindAddr(uint8 addrType, uint8 *pDevAddr,
                             uint8 *pResolvedAddr)
{
  uint8_t bondIdx = GAP_BONDINGS_MAX;

  // Allocate message buffer space
  ICall_BondMgrFindAddr *msg =
    (ICall_BondMgrFindAddr *)ICall_allocMsg(sizeof(ICall_BondMgrFindAddr));

  if (msg)
  {
    setDispatchCmdEvtHdr(&msg->hdr, DISPATCH_GAP_PROFILE,
                         DISPATCH_GAP_BOND_FIND_ADDR);

    // Set resolve addr parameters
    msg->addrType = addrType;
    msg->pDevAddr = pDevAddr;
    msg->pResolvedAddr = pResolvedAddr;

    // Send the message
    sendWaitMatchValueCS(ICall_getEntityId(), msg, matchGapResolveAddrCS,
                         sizeof(bondIdx), (uint8_t *)&bondIdx);
  }

  return bondIdx;
}

/******************************************************************************
 * Add function for the GATT Service.
 *
 * Public function defined in gattservapp.h.
 */
bStatus_t GATTServApp_AddService(uint32 services)
{
  return gattAddService(services, DISPATCH_PROFILE_ADD_SERVICE);
}

/******************************************************************************
 * Register a service's attribute list and callback functions with the GATT
 * Server Application.
 *
 * Public function defined in gattservapp.h.
 */
bStatus_t GATTServApp_RegisterService(gattAttribute_t *pAttrs,
                                      uint16 numAttrs, uint8 encKeySize,
                                      CONST gattServiceCBs_t *pServiceCBs)
{
  // Allocate message buffer space
  ICall_GSA_RegService *msg =
    (ICall_GSA_RegService *)ICall_allocMsg(sizeof(ICall_GSA_RegService));

  if (msg)
  {
    setDispatchCmdEvtHdr(&msg->hdr, DISPATCH_GATT_SERV_APP,
                         DISPATCH_PROFILE_REG_SERVICE);

    // copy pAttrs, numAttrs, encKeySize, and pServiceCBs over
    msg->pAttrs = pAttrs;
    msg->numAttrs = numAttrs;
    msg->encKeySize = encKeySize;
    msg->pServiceCBs = pServiceCBs;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchGSARegisterServiceCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/******************************************************************************
 * Deregister a service's attribute list and callback functions from
 * the GATT Server Application.
 *
 * Public function defined in gattservapp.h.
 */
bStatus_t GATTServApp_DeregisterService(uint16 handle, gattAttribute_t **p2pAttrs)
{
  // Allocate message buffer space
  ICall_GSA_DeregService *msg =
    (ICall_GSA_DeregService *)ICall_allocMsg(sizeof(ICall_GSA_DeregService));

  if (msg)
  {
    setDispatchCmdEvtHdr(&msg->hdr, DISPATCH_GATT_SERV_APP,
                         DISPATCH_PROFILE_DEREG_SERVICE);

    // set connection handle and p2pAttrs
    msg->handle = handle;
    msg->p2pAttrs = p2pAttrs;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchGSADeregisterServiceCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/******************************************************************************
 * Set a GATT Server parameter.
 *
 * Public function defined in gattservapp.h.
 */
bStatus_t GATTServApp_SetParameter(uint8 param, uint8 len, void *pValue)
{
  return profileSetParameter(param, len, pValue, DISPATCH_GATT_SERV_APP,
                             DISPATCH_PROFILE_SET_PARAM, matchGSASetParamCS);
}

/******************************************************************************
 * Get a GATT Server parameter.
 *
 * Public function defined in gattservapp.h.
 */
bStatus_t GATTServApp_GetParameter(uint8 param, void *pValue)
{
  return profileGetParameter(param, pValue, DISPATCH_GATT_SERV_APP,
                             DISPATCH_PROFILE_GET_PARAM, matchGSAGetParamCS);
}

/******************************************************************************
 * Send out a Service Changed Indication.
 *
 * Public function defined in gattservapp.h.
 */
bStatus_t GATTServApp_SendServiceChangedInd(uint16 connHandle, uint8 taskId)
{
  size_t allocSize = sizeof(ICall_GSA_ServiceChangeInd);

  ICall_GSA_ServiceChangeInd *msg =
    (ICall_GSA_ServiceChangeInd *)ICall_allocMsg(allocSize);

  if (msg)
  {
    setDispatchCmdEvtHdr(&msg->hdr, DISPATCH_GATT_SERV_APP,
                         DISPATCH_GSA_SERVICE_CHANGE_IND);

    msg->connHandle = connHandle;
    msg->taskId = ICall_getLocalMsgEntityId(ICALL_SERVICE_CLASS_BLE_MSG, taskId);

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchGSAServiceChangedIndCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/******************************************************************************
 * Return a Delayed Read Response.
 *
 * Public function defined in gattservapp.h.
 */
bStatus_t GATTServApp_ReadRsp( uint16 connHandle, uint8 *pAttrValue,
                               uint16 attrLen, uint16 attrHandle )
{
  // Allocate message buffer space
  ICall_GSA_ReadRsp *msg =
    (ICall_GSA_ReadRsp *)ICall_allocMsg(sizeof(ICall_GSA_ReadRsp));

  if (msg)
  {
    setDispatchCmdEvtHdr(&msg->hdr, DISPATCH_GATT_SERV_APP,
                         DISPATCH_PROFILE_DELAYED_RSP);

    // Copy Params
    msg->connHandle = connHandle;
    msg->pAttrValue = pAttrValue;
    msg->attrLen = attrLen;
    msg->attrHandle = attrHandle;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchGSAReadRspCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

#if defined ( GATT_QUAL )
/*******************************************************************************
 * Add function for the GATT Qualification Services.
 *
 * Public function defined in gatttest.h.
 */
bStatus_t GATTQual_AddService( uint32 services )
{
  return gattAddService(services, DISPATCH_GSA_ADD_QUAL_SERVICE);
}
#endif // GATT_QUAL

#if defined ( GATT_TEST )
/*******************************************************************************
 * Add function for the GATT Test Services.
 *
 * Public function defined in gatttest.h.
 */
bStatus_t GATTTest_AddService( uint32 services )
{
  return gattAddService(services, DISPATCH_GSA_ADD_TEST_SERVICE);
}
#endif // GATT_TEST

/******************************************************************************
 * Register for GATT local events and ATT Responses pending for transmission.
 *
 * Public function defined in gatt.h.
 */
void GATT_RegisterForMsgs(uint8 taskID)
{
  registerTask(taskID, DISPATCH_GATT_PROFILE, DISPATCH_GATT_REG_FOR_MSG);
}

/******************************************************************************
 * Initialize the Generic Attribute Profile Client.
 *
 * Public function defined in gatt.h.
 */
bStatus_t GATT_InitClient(void)
{
  // Allocate message buffer space
  ICall_GattInitClient *msg =
    (ICall_GattInitClient *)ICall_allocMsg(sizeof(ICall_GattInitClient));

  if (msg)
  {
    setDispatchCmdEvtHdr(&msg->hdr, DISPATCH_GATT_PROFILE,
                         DISPATCH_GATT_INIT_CLIENT);

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchGattInitClientCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/******************************************************************************
 * Register to receive incoming ATT Indications or Notifications
 * of attribute values.
 *
 * Public function declared in gatt.h
 */
void GATT_RegisterForInd(uint8 taskId)
{
  ICall_GattRegisterForInd *msg =
    (ICall_GattRegisterForInd *)ICall_allocMsg(sizeof(ICall_GattRegisterForInd));

  if (msg)
  {
    setDispatchCmdEvtHdr(&msg->hdr, DISPATCH_GATT_PROFILE,
                         DISPATCH_GATT_REG_4_IND);
    // Set task id
    msg->taskId = ICall_getLocalMsgEntityId(ICALL_SERVICE_CLASS_BLE_MSG, taskId);

    // Send the message
    ICall_sendServiceMsg(ICall_getEntityId(), ICALL_SERVICE_CLASS_BLE,
                         ICALL_MSG_FORMAT_3RD_CHAR_TASK_ID, msg);
  }
}

/*********************************************************************
 * GATT Set Host To App Flow Control.
 *
 * Public function defined in gatt.h.
 *
 * @return  none
 */
void GATT_SetHostToAppFlowCtrl(uint16 heapSize, uint8 flowCtrlMode)
{
  ICall_GattHtaFlowCtrl *msg =
    (ICall_GattHtaFlowCtrl *)ICall_allocMsg(sizeof(ICall_GattHtaFlowCtrl));

  if (msg)
  {
    setDispatchCmdEvtHdr(&msg->hdr, DISPATCH_GATT_PROFILE,
                         DISPATCH_GATT_HTA_FLOW_CTRL);

    // Set heap size and flow control mode
    msg->heapSize = heapSize;
    msg->flowCtrlMode = flowCtrlMode;

    // Send the message
    ICall_sendServiceMsg(ICall_getEntityId(), ICALL_SERVICE_CLASS_BLE,
                         ICALL_MSG_FORMAT_3RD_CHAR_TASK_ID, msg);
  }
}

/*********************************************************************
 * GATT App Completed Message.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
void GATT_AppCompletedMsg(gattMsgEvent_t *pMsg)
{
  ICall_GattAppComplMsg *msg =
    (ICall_GattAppComplMsg *)ICall_allocMsg(sizeof(ICall_GattAppComplMsg));

  if (msg)
  {
    setDispatchCmdEvtHdr(&msg->hdr, DISPATCH_GATT_PROFILE,
                         DISPATCH_GATT_APP_COMPL_MSG);

    // Set pointer to message
    msg->pMsg = pMsg;

    // Send the message
    ICall_sendServiceMsg(ICall_getEntityId(), ICALL_SERVICE_CLASS_BLE,
                         ICALL_MSG_FORMAT_3RD_CHAR_TASK_ID, msg);
  }
}

/*********************************************************************
 * Send an ATT Response message out.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_SendRsp(uint16 connHandle, uint8 method, gattMsg_t *pRsp)
{
  ICall_GattSendRsp *msg =
    (ICall_GattSendRsp *)ICall_allocMsg(sizeof(ICall_GattSendRsp));

  if (msg)
  {
    setDispatchCmdEvtHdr(&msg->hdr, DISPATCH_GATT_PROFILE,
                         DISPATCH_GATT_SEND_RSP);

    // Set connection handle, method and response pointer
    msg->connHandle = connHandle;
    msg->method = method;
    msg->pRsp = pRsp;

    /* Send the message. */
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchGattSendRspCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * @brief   Add function for the GAP GATT Service.
 *
 * @param   services  services to add. This is a bit map and can
 *                     contain more than one service.
 *
 *
 * @return      SUCCESS / FAILURE
 */
bStatus_t GGS_AddService(uint32 services)
{
  return profileAddService(services, DISPATCH_GAP_GATT_SERV,
                           DISPATCH_PROFILE_ADD_SERVICE);
}

/*********************************************************************
 * Set GGS Parameter
 *
 * Public function defined in peripheral.h
 */
bStatus_t GGS_SetParameter(uint8 param, uint8 len, void *value)
{
  return profileSetParameter(param, len, value, DISPATCH_GAP_GATT_SERV,
                             DISPATCH_PROFILE_SET_PARAM, matchGGSSetParamCS);
}

/*********************************************************************
 * Get GGS Parameter
 *
 * Public function defined in peripheral.h
 */
bStatus_t GGS_GetParameter(uint8 param, void *value)
{
  return profileGetParameter(param, value, DISPATCH_GAP_GATT_SERV,
                             DISPATCH_PROFILE_GET_PARAM, matchGGSGetParamCS);
}

/*********************************************************************
 * @brief   Register callback functions with the bond manager.
 *
 * Public function defined in gapbondmgr.h.
 */
void GGS_RegisterAppCBs( ggsAppCBs_t *appCallbacks )
{
  // Allocate message buffer space
  ICall_GGSRegisterAppCBs *msg =
    (ICall_GGSRegisterAppCBs *)ICall_allocMsg(sizeof(ICall_GGSRegisterAppCBs));

  if (msg)
  {
    setDispatchCmdEvtHdr(&msg->hdr, DISPATCH_GAP_GATT_SERV,
                         DISPATCH_PROFILE_REG_CB);
    // set callback
    msg->pCB = appCallbacks;

    // Send the message
    ICall_sendServiceMsg(ICall_getEntityId(), ICALL_SERVICE_CLASS_BLE,
                         ICALL_MSG_FORMAT_3RD_CHAR_TASK_ID, msg);
  }
}

/*********************************************************************
 * Compare a received Profile Command Status message for a match
 *
 * @param src        originator of the message as a service enumeration
 * @param dest       destination entity id of the message
 * @param msg        pointer to the message body
 * @param subGroup   subgroup id
 * @param profileId  command id
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchProfileCS(ICall_ServiceEnum src, ICall_EntityID dest,
                           const void *msg, uint8 subGroup, uint8 cmdId)
{
  ICall_GapCmdStatus *pMsg = (ICall_GapCmdStatus *)msg;

  if ((pMsg->hdr.hdr.event == ICALL_EVENT_EVENT)              &&
      (pMsg->hdr.eventOpcode == HCI_EXT_GAP_CMD_STATUS_EVENT) &&
      (pMsg->opCode == subGroup)                              &&
      (pMsg->cmdId == cmdId))
  {
    return true;
  }

  return false;
}

/*********************************************************************
 * Compare a received Profile Set Param Value Command Status message for a match
 *
 * @param src       originator of the message as a service enumeration
 * @param dest      destination entity id of the message
 * @param msg       pointer to the message body
 * @param profileId profile id
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchProfileSetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg, uint8 profileId)
{
  return matchProfileCS(src, dest, msg, profileId, DISPATCH_PROFILE_SET_PARAM);
}

/*********************************************************************
 * Compare a received Profile Get Param Value Command Status message for a match
 *
 * @param src        originator of the message as a service enumeration
 * @param dest       destination entity id of the message
 * @param msg        pointer to the message body
 * @param profileId  profile id
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchProfileGetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg, uint8 profileId)
{
  return matchProfileCS(src, dest, msg, profileId, DISPATCH_PROFILE_GET_PARAM);
}

/*********************************************************************
 * Compare a received GGS Get Param Value Command Status message for a match
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGGSSetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg)
{
  return matchProfileSetParamCS(src, dest, msg, DISPATCH_GAP_GATT_SERV);
}

/*********************************************************************
 * Compare a received GGS Get Param Value Command Status message for a match
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGGSGetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg)
{
  return matchProfileGetParamCS(src, dest, msg, DISPATCH_GAP_GATT_SERV);
}

/*********************************************************************
 * Compare a received GSA Get Param Value Command Status message for a match
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGSASetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg)
{
  return matchProfileSetParamCS(src, dest, msg, DISPATCH_GATT_SERV_APP);
}

/*********************************************************************
 * Compare a received GSA Get Param Value Command Status message for a match
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGSAGetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg)
{
  return matchProfileGetParamCS(src, dest, msg, DISPATCH_GATT_SERV_APP);
}

/*********************************************************************
 * Compare a received Profile Add Service Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 * @param cmdId command id
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchProfileAddServiceCS(ICall_ServiceEnum src,
                                     ICall_EntityID dest, const void *msg)
{
  ICall_GapCmdStatus *pMsg = (ICall_GapCmdStatus *)msg;

  if ((pMsg->hdr.hdr.event == ICALL_EVENT_EVENT)              &&
      (pMsg->hdr.eventOpcode == HCI_EXT_GAP_CMD_STATUS_EVENT) &&
      ((pMsg->opCode == DISPATCH_GAP_GATT_SERV)               ||
       (pMsg->opCode == DISPATCH_GATT_SERV_APP))              &&
      ((pMsg->cmdId == DISPATCH_PROFILE_ADD_SERVICE)          ||
       (pMsg->cmdId == DISPATCH_GSA_ADD_QUAL_SERVICE)         ||
       (pMsg->cmdId == DISPATCH_GSA_ADD_TEST_SERVICE)))
  {
    return true;
  }

  return false;
}

/******************************************************************************
 * @fn      profileAddService
 *
 * @brief   Add function for a Service.
 *
 * @param   services   services to add. This is a bit map and can
 *                     contain more than one service.
 * @param   profileId  profile id.
 * @param   serviceId  service to add.
 *
 * @return  SUCCESS: When successful.
 *          INVALIDPARAMETER: Invalid parameter.
 *          MSG_BUFFER_NOT_AVAIL: Memory allocation error occurred.
 *          FAILURE: Some other failure.
 */
static bStatus_t profileAddService(uint32 services, uint8 profileId,
                                   uint8 serviceId)
{
  ICall_ProfileAddService *msg =
    (ICall_ProfileAddService *)ICall_allocMsg(sizeof(ICall_ProfileAddService));

  if (msg)
  {
    setDispatchCmdEvtHdr(&msg->hdr, profileId, serviceId);

    // set services
    msg->services = services;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchProfileAddServiceCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/******************************************************************************
 * @fn      gattAddService
 *
 * @brief   Add function for a GATT Service.
 *
 * @param   services   services to add. This is a bit map and can
 *                     contain more than one service.
 * @param   serviceId  service to add.
 *
 * @return  SUCCESS: When successful.
 *          INVALIDPARAMETER: Invalid parameter.
 *          MSG_BUFFER_NOT_AVAIL: Memory allocation error occurred.
 *          FAILURE: Some other failure.
 */
static bStatus_t gattAddService(uint32 services, uint8 serviceId)
{
  return profileAddService(services, DISPATCH_GATT_SERV_APP, serviceId);
}

/******************************************************************************
 * @fn      profileSetParameter
 *
 * @brief   Set Parameter function for a profile.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to right
 * @param   pValue - pointer to data to write.  This is dependent on the
 *                   the parameter ID and WILL be cast to the appropriate
 *                   data type (example: data type of uint16 will be cast
 *                   to uint16 pointer).
 * @param   subgrp - subgroup ID
 * @param   cmdId - command ID
 * @param   matchCSFn  pointer to a function that would return TRUE when
 *                     the message matches its condition
 *
 * @return  SUCCESS: When successful.
 *          INVALIDPARAMETER: Invalid parameter.
 *          MSG_BUFFER_NOT_AVAIL: Memory allocation error occurred.
 *          FAILURE: Some other failure.
 */
static bStatus_t profileSetParameter(uint8 param, uint8 len, void *pValue,
                                     uint8_t subgrp, uint8_t cmdId,
                                     ICall_MsgMatchFn matchCSFn)
{
  // Allocate message buffer space
  ICall_ProfileSetParam *msg =
    (ICall_ProfileSetParam *)ICall_allocMsg(sizeof(ICall_ProfileSetParam));

  if (msg)
  {
    setDispatchCmdEvtHdr(&msg->hdr, subgrp, cmdId);

    // copy param ID, len, value
    msg->paramIdLenVal.paramId = param;
    msg->paramIdLenVal.len = len;
    msg->paramIdLenVal.pValue = pValue;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchCSFn);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/******************************************************************************
 * @fn      profileGetParameter
 *
 * @brief   Get Parameter function for a profile.
 *
 * @param   param - Profile parameter ID
 * @param   pValue - pointer to data to put. This is dependent on the
 *                   parameter ID and WILL be cast to the appropriate
 *                   data type (example: data type of uint16 will be
 *                   cast to uint16 pointer).
 * @param   subgrp - subgroup ID
 * @param   cmdId - command ID
 * @param   matchCSFn  pointer to a function that would return TRUE when
 *                     the message matches its condition
 *
 * @return  SUCCESS: When successful.
 *          INVALIDPARAMETER: Invalid parameter.
 *          MSG_BUFFER_NOT_AVAIL: Memory allocation error occurred.
 *          FAILURE: Some other failure.
 */
static bStatus_t profileGetParameter(uint8 param, void *pValue, uint8_t subgrp,
                                     uint8_t cmdId, ICall_MsgMatchFn matchCSFn)
{
  // Allocate message buffer space
  ICall_ProfileGetParam *msg =
    (ICall_ProfileGetParam *)ICall_allocMsg(sizeof(ICall_ProfileGetParam));

  if (msg)
  {
    setDispatchCmdEvtHdr(&msg->hdr, subgrp, cmdId);

    // copy param ID, value
    msg->paramIdVal.paramId = param;
    msg->paramIdVal.pValue  = pValue;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchCSFn);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * BLE HOST API FUNCTIONS
 */

/*********************************************************************
 * GATT Exchange MTU.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_ExchangeMTU(uint16 connHandle, attExchangeMTUReq_t *pReq,
                           uint8 taskId)
{
  return gattRequest(connHandle, (attMsg_t *)pReq, taskId,
                     ATT_EXCHANGE_MTU_REQ);
}

/*********************************************************************
 * GATT Discover All Primary Services.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_DiscAllPrimaryServices(uint16 connHandle, uint8 taskId)
{
  size_t allocSize = sizeof(ICall_GattDiscAllPrimaryServ);

  ICall_GattDiscAllPrimaryServ *msg =
    (ICall_GattDiscAllPrimaryServ *)ICall_allocMsg(allocSize);

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GATT_SUBGRP, ATT_READ_BY_GRP_TYPE_REQ);

    msg->connHandle = connHandle;
    msg->taskId = ICall_getLocalMsgEntityId(ICALL_SERVICE_CLASS_BLE_MSG, taskId);

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchGattDiscAllPrimarySvcCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * GATT Discover Primary Service By UUID.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_DiscPrimaryServiceByUUID(uint16 connHandle, uint8 *pValue,
                                        uint8 len, uint8 taskId)
{
  size_t allocSize = sizeof(ICall_GattDiscPrimServByUUID);

  ICall_GattDiscPrimServByUUID *msg =
    (ICall_GattDiscPrimServByUUID *)ICall_allocMsg(allocSize);

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GATT_SUBGRP,
                      ATT_FIND_BY_TYPE_VALUE_REQ);
    // Set parameters
    msg->connHandle = connHandle;
    msg->len = len;
    msg->pValue = pValue;
    msg->taskId = ICall_getLocalMsgEntityId(ICALL_SERVICE_CLASS_BLE_MSG, taskId);

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchGattDiscPrimartSvcCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * GATT Find Included Services.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_FindIncludedServices(uint16 connHandle, uint16 startHandle,
                                    uint16 endHandle, uint8 taskId)
{
  ICall_GattDiscAllChars *msg =
    (ICall_GattDiscAllChars *)ICall_allocMsg(sizeof(ICall_GattDiscAllChars));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GATT_SUBGRP,
                      GATT_FIND_INCLUDED_SERVICES);
    // Set handles
    msg->connHandle = connHandle;
    msg->startHandle = startHandle;
    msg->endHandle = endHandle;
    msg->taskId = ICall_getLocalMsgEntityId(ICALL_SERVICE_CLASS_BLE_MSG, taskId);

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchGattFindIncludedSvcCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * GATT Discover All Characteristics
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_DiscAllChars(uint16 connHandle, uint16 startHandle,
                            uint16 endHandle, uint8 taskId)
{
  ICall_GattDiscAllChars *msg =
    (ICall_GattDiscAllChars *)ICall_allocMsg(sizeof(ICall_GattDiscAllChars));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GATT_SUBGRP, GATT_DISC_ALL_CHARS);

    // Set handles
    msg->connHandle = connHandle;
    msg->startHandle = startHandle;
    msg->endHandle = endHandle;
    msg->taskId = ICall_getLocalMsgEntityId(ICALL_SERVICE_CLASS_BLE_MSG, taskId);

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchGattDiscAllCharsCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * GATT Discover Characteristics by UUID.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_DiscCharsByUUID(uint16 connHandle, attReadByTypeReq_t *pReq,
                               uint8 taskId)
{
  return gattRequest(connHandle, (attMsg_t *)pReq, taskId,
                     ATT_READ_BY_TYPE_REQ);
}

/*********************************************************************
 * GATT Discover All Characteristic Descriptors.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_DiscAllCharDescs(uint16 connHandle, uint16 startHandle,
                                 uint16 endHandle, uint8 taskId)
{
  attFindInfoReq_t req;

  req.startHandle = startHandle;
  req.endHandle = endHandle;

  return gattRequest(connHandle, (attMsg_t *)&req, taskId, ATT_FIND_INFO_REQ);
}

/*********************************************************************
 * GATT Read Characteristic Value.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_ReadCharValue(uint16 connHandle, attReadReq_t *pReq,
                             uint8 taskId)
{
  return gattRequest(connHandle, (attMsg_t *)pReq, taskId, ATT_READ_REQ);
}

/*********************************************************************
 * GATT Read Characteristic Value using the Characteristic UUID.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_ReadUsingCharUUID(uint16 connHandle, attReadByTypeReq_t *pReq,
                                 uint8 taskId)
{
  return gattRequest(connHandle, (attMsg_t *)pReq, taskId,
                     GATT_READ_USING_CHAR_UUID);
}

/*********************************************************************
 * GATT Read Long Characteristic Value.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_ReadLongCharValue(uint16 connHandle, attReadBlobReq_t *pReq,
                                 uint8 taskId)
{
  return gattRequest(connHandle, (attMsg_t *)pReq, taskId,
                     ATT_READ_BLOB_REQ);
}

/*********************************************************************
 * GATT Read Multiple Characteristic Values.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_ReadMultiCharValues(uint16 connHandle, attReadMultiReq_t *pReq,
                                   uint8 taskId)
{
  return gattRequest(connHandle, (attMsg_t *)pReq, taskId,
                     ATT_READ_MULTI_REQ);
}

/*********************************************************************
 * GATT Write Without Response.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_WriteNoRsp(uint16 connHandle, attWriteReq_t *pReq)
{
  return gattRequest(connHandle, (attMsg_t *)pReq, ICall_getEntityId(),
                     GATT_WRITE_NO_RSP);
}

/*********************************************************************
 * GATT Signed Write Without Response.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_SignedWriteNoRsp(uint16 connHandle, attWriteReq_t *pReq)
{
  return gattRequest(connHandle, (attMsg_t *)pReq, ICall_getEntityId(),
                     GATT_WRITE_NO_RSP);
}

/*********************************************************************
 * GATT Write Characteristic Value.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_WriteCharValue(uint16 connHandle, attWriteReq_t *pReq,
                              uint8 taskId)
{
  return gattRequest(connHandle, (attMsg_t *)pReq, taskId, ATT_WRITE_REQ);
}

/*********************************************************************
 * GATT Write Long Characteristic Value.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_WriteLongCharValue(uint16 connHandle, attPrepareWriteReq_t *pReq,
                                  uint8 taskId)
{
  return gattWriteLong(connHandle, pReq, taskId, ATT_PREPARE_WRITE_REQ);
}

/*********************************************************************
 * GATT Write Long Characteristic Descriptor.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_WriteLongCharDesc(uint16 connHandle, attPrepareWriteReq_t *pReq,
                                 uint8 taskId)
{
  return gattWriteLong(connHandle, pReq, taskId, GATT_WRITE_LONG_CHAR_DESC);
}

/*********************************************************************
 * Send GATT Reliable Writes.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_ReliableWrites(uint16 connHandle, attPrepareWriteReq_t *pReqs,
                              uint8 numReqs, uint8 flags, uint8 taskId)
{
  ICall_GattReliableWrite *msg =
    (ICall_GattReliableWrite *)ICall_allocMsg(sizeof(ICall_GattReliableWrite));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GATT_SUBGRP, GATT_RELIABLE_WRITES);

    // Set GATT write parameters
    msg->connHandle = connHandle;
    msg->flags = flags;
    msg->numReqs = numReqs;
    msg->pReqs = pReqs;
    msg->taskId = ICall_getLocalMsgEntityId(ICALL_SERVICE_CLASS_BLE_MSG, taskId);

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchGattReliableWritesCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * GATT Read Characteristic Descriptor.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_ReadCharDesc(uint16 connHandle, attReadReq_t *pReq,
                            uint8 taskId)
{
  return gattRequest(connHandle, (attMsg_t *)pReq, taskId,
                     GATT_READ_CHAR_DESC);
}

/*********************************************************************
 * // GATT Read Long Characteristic Descriptor.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_ReadLongCharDesc(uint16 connHandle, attReadBlobReq_t *pReq,
                                uint8 taskId)
{
  return gattRequest(connHandle, (attMsg_t *)pReq, taskId,
                     GATT_READ_LONG_CHAR_DESC);
}

/*********************************************************************
 * GATT Write Characteristic Descriptor.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_WriteCharDesc(uint16 connHandle, attWriteReq_t *pReq,
                             uint8 taskId)
{
  return gattRequest(connHandle, (attMsg_t *)pReq, taskId,
                     GATT_WRITE_CHAR_DESC);
}

/*********************************************************************
 * GATT Handle Value Notification.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_Notification(uint16 connHandle, attHandleValueNoti_t *pNoti,
                            uint8 authenticated)
{
  return gattIndNoti(connHandle, (attMsg_t *)pNoti, authenticated,
                     ICall_getEntityId(), ATT_HANDLE_VALUE_NOTI);
}

/*********************************************************************
 * GATT Handle Value Notification.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
uint16_t GATT_GetNextHandle( void )
{
  uint16_t paramValue = 0;

  ICall_GattGetNextHandle *msg =
    (ICall_GattGetNextHandle *)ICall_allocMsg(sizeof(ICall_GattGetNextHandle));


  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_UTIL_SUBGRP, UTIL_EXT_GATT_GET_NEXT_HANDLE);

    // Send the message
    sendWaitMatchValueCS(ICall_getEntityId(), msg, matchGattGetNexthandleCS,
                         sizeof(paramValue), (uint8_t *)&paramValue);
  }

  return paramValue;
}

/*********************************************************************
 * GATT Handle Value Indication.
 *
 * Public function defined in gatt.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GATT_Indication(uint16 connHandle, attHandleValueInd_t *pInd,
                          uint8 authenticated, uint8 taskId)
{
  return gattIndNoti(connHandle, (attMsg_t *)pInd, authenticated,
                     taskId, ATT_HANDLE_VALUE_IND);
}

/*********************************************************************
 * Send ATT Error Response.
 *
 * Public function defined in att.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t ATT_ErrorRsp(uint16 connHandle, attErrorRsp_t *pRsp)
{
  return attSendParamAndPtrCmd(ATT_ERROR_RSP, connHandle, (attMsg_t *)pRsp,
                               matchAttErrorRspCS);
}

/*********************************************************************
 * Send ATT Read Response.
 *
 * Public function defined in att.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t ATT_ReadRsp(uint16 connHandle, attReadRsp_t *pRsp)
{
  return attSendParamAndPtrCmd(ATT_READ_RSP, connHandle, (attMsg_t *)pRsp,
                               matchAttReadRspCS);
}

/*********************************************************************
 * Send ATT Read Blob Response.
 *
 * Public function defined in att.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t ATT_ReadBlobRsp(uint16 connHandle, attReadBlobRsp_t *pRsp)
{
  return attSendParamAndPtrCmd(ATT_READ_BLOB_RSP, connHandle, (attMsg_t *)pRsp,
                               matchAttReadBlobRspCS);
}

/*********************************************************************
 * Send ATT Write Response.
 *
 * Public function defined in att.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t ATT_WriteRsp(uint16 connHandle)
{
  return attSendParamAndPtrCmd(ATT_WRITE_RSP, connHandle, NULL,
                               matchAttWriteRspCS);
}

/*********************************************************************
 * Send ATT Execute Write Response.
 *
 * Public function defined in att.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t ATT_ExecuteWriteRsp(uint16 connHandle)
{
  return attSendParamAndPtrCmd(ATT_EXECUTE_WRITE_RSP, connHandle, NULL,
                               matchAttExecuteWriteRspCS);
}

/*********************************************************************
 * Send ATT Handle Value Confirmation.
 *
 * Public function defined in att.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t ATT_HandleValueCfm(uint16 connHandle)
{
  return attSendParamAndPtrCmd(ATT_HANDLE_VALUE_CFM, connHandle, NULL,
                               matchAttValueHandleCfmCS);
}

/*******************************************************************************
 * Send an ATT message and wait for a matching Command Status response.
 *
 * @param   opCode     opcode of the message
 * @param   connHandle connection handle
 * @param   pMsg       pointer to the message
 * @param   matchCSFn  pointer to a function that would return TRUE when
 *                     the message matches its condition
 *
 * @return  SUCCESS or FAILURE
 */
static bStatus_t attSendParamAndPtrCmd(uint8_t opCode, uint16_t connHandle,
                                       attMsg_t *pMsg,
                                       ICall_MsgMatchFn matchCSFn)
{
  // Allocate message buffer space
  ICall_AttParamAndPtr *msg =
    (ICall_AttParamAndPtr *)ICall_allocMsg(sizeof(ICall_AttParamAndPtr));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_ATT_SUBGRP, opCode);

    // Set connection handle and set request
    msg->connHandle = connHandle;
    msg->pMsg = pMsg;

    /* Send the message. */
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchCSFn);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * Use this function to initialize GAP Device parameters
 *
 * Public function defined in gap.h.
 */
bStatus_t GAP_DeviceInit(uint8 profileRole, uint8 taskID,
                         GAP_Addr_Modes_t addrMode, uint8_t* pRandomAddr)
{
  // Allocate message buffer space
  ICall_GapDeviceInit *msg =
    (ICall_GapDeviceInit *)ICall_allocMsg(sizeof(ICall_GapDeviceInit));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GAP_SUBGRP, HCI_EXT_GAP_DEVICE_INIT);

    // Set GAP Device parameters
    msg->profileRole = profileRole;
    msg->taskID = ICall_getLocalMsgEntityId(ICALL_SERVICE_CLASS_BLE_MSG, taskID);
    msg->addrType = addrMode;
    msg->pRandomAddr = pRandomAddr;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchGapDeviceInitCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * Update the link parameters to a slave device.
 *
 * Public function defined in gap.h.
 */
bStatus_t GAP_UpdateLinkParamReq(gapUpdateLinkParamReq_t *pParams)
{
  return gapSendPtrParamsCmd((uint8_t *)pParams, NULL,
                             HCI_EXT_GAP_UPDATE_LINK_PARAM_REQ,
                             matchGapUpdateLinkParamReqCS);
}

/*********************************************************************
 * Send desired updated connection parameters when sent a Connection
 * Parameter Update Request from a remote device is received.
 *
 * Public function defined in gap.h.
 */
bStatus_t GAP_UpdateLinkParamReqReply(gapUpdateLinkParamReqReply_t *pParams)
{
  return gapSendPtrParamsCmd((uint8_t *)pParams, NULL,
                             HCI_EXT_GAP_UPDATE_LINK_PARAM_REQ_REPLY,
                             matchGapConnParamUpdateReplyCS);
}

/*********************************************************************
 * Terminate a link connection.
 *
 * Public function defined in gap.h.
 */
bStatus_t GAP_TerminateLinkReq(uint16 connHandle, uint8 reason)
{
  // Allocate message buffer space
  ICall_GapTerminateLink *msg =
    (ICall_GapTerminateLink *)ICall_allocMsg(sizeof(ICall_GapTerminateLink));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GAP_SUBGRP, HCI_EXT_GAP_TERMINATE_LINK);

    // Set connection handle and reason for termination
    msg->connHandle = connHandle;
    msg->reason = reason;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg,
                           matchGapTerminateLinkReqCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * GAP Authenticate
 *
 * Public function defined in gap.h.
 */
bStatus_t GAP_Authenticate(gapAuthParams_t *pParams,
                           gapPairingReq_t *pPairReq)
{
  return gapSendPtrParamsCmd((uint8_t *)pParams, (uint8_t *)pPairReq,
                             HCI_EXT_GAP_AUTHENTICATE,
                             matchGapAuthenticateCS);
}

/*********************************************************************
 * Terminate an authentication/pairing process.
 *
 * Public function defined in gap.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GAP_TerminateAuth(uint16 connectionHandle, uint8 reason)
{
  return gapSendParamsCmd(connectionHandle, reason, HCI_EXT_GAP_TERMINATE_AUTH,
                          matchGapTerminateAuthCS);
}

/*********************************************************************
 * Set up the connection's bound paramaters.
 *
 * NOTE:        This function is called after the link is established.
 *
 * Public function defined in gap.h.
 */
bStatus_t GAP_Bond(uint16 connectionHandle, uint8 authenticated,
                   uint8 secureConnections, smSecurityInfo_t *pParams,
                   uint8 startEncryption)
{
  /* Allocate message buffer space */
  ICall_GapBond *msg =
    (ICall_GapBond *)ICall_allocMsg(sizeof(ICall_GapBond));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GAP_SUBGRP, HCI_EXT_GAP_BOND);

    // Set connHandle and other params for bonding
    msg->connHandle = connectionHandle;
    msg->authenticated = authenticated;
    msg->secureConnections = secureConnections;
    msg->pParams = pParams;
    msg->startEncryption = startEncryption;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchGapBondCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * Set up the connection to accept signed data.
 *
 * Public function defined in gap.h.
 */
bStatus_t GAP_Signable(uint16 connectionHandle, uint8 authenticated,
                       smSigningInfo_t *pParams)
{
  ICall_GapSignable *msg =
    (ICall_GapSignable *)ICall_allocMsg(sizeof(ICall_GapSignable));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GAP_SUBGRP, HCI_EXT_GAP_SIGNABLE);

    // set connection handle and other params
    msg->connHandle = connectionHandle;
    msg->authenticated = authenticated;
    msg->pParams = pParams;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchGapSignableCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * GAP Update the passkey.
 *
 * Public function defined in gap.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t GAP_PasskeyUpdate(uint8 *pPasskey, uint16 connectionHandle)
{
  size_t allocSize = sizeof(ICall_GapPassKeyUpdateParam);

  ICall_GapPassKeyUpdateParam *msg =
    (ICall_GapPassKeyUpdateParam *)ICall_allocMsg(allocSize);

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GAP_SUBGRP, HCI_EXT_GAP_PASSKEY_UPDATE);

    // Set connection handle and passkey
    msg->connHandle = connectionHandle;
    msg->pPasskey = pPasskey;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchGapPasskeyUpdateCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * Set a GAP Parameter value.  Use this function to change the default
 * GAP parameter values.
 *
 * Public function defined in gap.h.
 */
bStatus_t GAP_SetParamValue(gapParamIDs_t paramID, uint16 paramValue)
{
  /* Allocate message buffer space */
  ICall_GapSetParam *msg =
    (ICall_GapSetParam *)ICall_allocMsg(sizeof(ICall_GapSetParam));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GAP_SUBGRP, HCI_EXT_GAP_SET_PARAM);

    // Set param ID and value
    msg->paramID = paramID;
    msg->paramValue = paramValue;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchGapSetParamCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * Get a GAP Parameter value.  Use this function to get GAP parameter values.
 *
 * Public function defined in gap.h.
 */
uint16 GAP_GetParamValue(gapParamIDs_t paramID)
{
  uint16_t paramValue = 0;

  // Allocate message buffer space
  ICall_GapGetParam *msg =
    (ICall_GapGetParam *)ICall_allocMsg(sizeof(ICall_GapGetParam));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GAP_SUBGRP, HCI_EXT_GAP_GET_PARAM);

    msg->paramID = paramID;

    // Send the message
    sendWaitMatchValueCS(ICall_getEntityId(), msg, matchGapGetParamCS,
                         sizeof(paramValue), (uint8_t *)&paramValue);
  }

  return paramValue;
}

/*********************************************************************
 * Send Slave Security Request
 *
 * Public function defined in gap.h.
 */
bStatus_t GAP_SendSlaveSecurityRequest(uint16 connHandle, uint8 authReq)
{
  return gapSendParamsCmd(connHandle, authReq,
                          HCI_EXT_GAP_SLAVE_SECURITY_REQ_UPDATE,
                          matchGapSlaveSecurityReqCS);
}

/*********************************************************************
 * @brief   Check to see if a physical link is in a specific state.
 *
 * Public function defined in linkdb.h.
 */
uint8 linkDB_State(uint16 connectionHandle, uint8 state)
{
  uint8_t linkState = FALSE;

  /* Allocate message buffer space */
  ICall_LinkDBState *msg =
    (ICall_LinkDBState *)ICall_allocMsg(sizeof(ICall_LinkDBState));

  if (msg)
  {
    setDispatchCmdEvtHdr(&msg->hdr, DISPATCH_GAP_PROFILE,
                         DISPATCH_GAP_LINKDB_STATE);

    /* create message header */
    msg->connHandle = connectionHandle;
    msg->state = state;

    /* Send the message. */
    sendWaitMatchValueCS(ICall_getEntityId(), msg, matchLinkDBStateCS,
                         sizeof(linkState), &linkState);
  }

  return linkState;
}

/*********************************************************************
 * @brief   Return the maximum number of connections supported.
 *
 * Public function defined in linkdb.h.
 */
uint8 linkDB_NumConns( void )
{
  uint8_t numConns = 0;

  /* Allocate message buffer space */
  ICall_LinkDBNumConns *msg =
    (ICall_LinkDBNumConns *)ICall_allocMsg(sizeof(ICall_LinkDBNumConns));

  if (msg)
  {
    setDispatchCmdEvtHdr(&msg->hdr, DISPATCH_GAP_PROFILE,
                         DISPATCH_GAP_LINKDB_NUM_CONNS);

    /* Send the message. */
    sendWaitMatchValueCS(ICall_getEntityId(), msg, matchLinkDBNumConnsCS,
                         sizeof(numConns), &numConns);
  }

  return numConns;
}

/*********************************************************************
 * @brief   Return the current number of connections.
 *
 * Public function defined in linkdb.h.
 */
uint8 linkDB_NumActive(void)
{
  uint8_t numActive = 0;

  /* Allocate message buffer space */
  ICall_LinkDBNumActive *msg =
    (ICall_LinkDBNumActive *)ICall_allocMsg(sizeof(ICall_LinkDBNumActive));

  if (msg)
  {
    setDispatchCmdEvtHdr(&msg->hdr, DISPATCH_GAP_PROFILE,
                         DISPATCH_GAP_LINKDB_NUM_ACTIVE);

    /* Send the message. */
    sendWaitMatchValueCS(ICall_getEntityId(), msg, matchLinkDBNumActiveCS,
                         sizeof(numActive), &numActive);
  }

  return numActive;
}

/*********************************************************************
 * @brief   Return the link information of a given connection handle.
 *
 * Public function defined in linkdb.h.
 */
uint8 linkDB_GetInfo(uint16 connHandle, linkDBInfo_t * pInfo)
{
  /* Allocate message buffer space */
  ICall_LinkDBGetInfo *msg =
    (ICall_LinkDBGetInfo *)ICall_allocMsg(sizeof(ICall_LinkDBGetInfo));

  if (msg)
  {
    setDispatchCmdEvtHdr(&msg->hdr, DISPATCH_GAP_PROFILE,
                         DISPATCH_GAP_LINKDB_GET_INFO);

    msg->connHandle = connHandle;
    msg->pInfo = pInfo;

    /* Send the message. */
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchLinkDBGetInfoCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * @brief   Set a GAP Bond Manager parameter.
 *
 * Public function defined in gapbondmgr.h.
 */
bStatus_t GAPBondMgr_SetParameter(uint16 param, uint8 len, void *pValue)
{
  /* Allocate message buffer space */
  ICall_ProfileSetParam *msg =
    (ICall_ProfileSetParam *)ICall_allocMsg(sizeof(ICall_ProfileSetParam));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GAP_SUBGRP, HCI_EXT_GAP_BOND_SET_PARAM);

    /* create message header */
    msg->paramIdLenVal.paramId = param;
    msg->paramIdLenVal.len = len;
    msg->paramIdLenVal.pValue = pValue;

    /* Send the message. */
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchBondMgrSetParamCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * @brief   Get a GAP Bond Manager parameter.
 *
 * Public function defined in gapbondmgr.h.
 */
bStatus_t GAPBondMgr_GetParameter(uint16 param, void *pValue)
{
  /* Allocate message buffer space */
  ICall_GapGetParam *msg =
    (ICall_GapGetParam *)ICall_allocMsg(sizeof(ICall_GapGetParam));

  if (msg)
  {
    ICall_Errno errno;

    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GAP_SUBGRP, HCI_EXT_GAP_BOND_GET_PARAM);

    // Set paramID
    msg->paramID = param;

    // Send the message
    errno = ICall_sendServiceMsg(ICall_getEntityId(), ICALL_SERVICE_CLASS_BLE,
                                 ICALL_MSG_FORMAT_3RD_CHAR_TASK_ID, msg);

    if (errno == ICALL_ERRNO_SUCCESS)
    {
      ICall_GapCmdStatus *pCmdStatus = NULL;

      errno = waitMatchCS(matchBondMgrGetParamCS, (void **)&pCmdStatus);
      if (errno == ICALL_ERRNO_SUCCESS)
      {
        uint8 status = pCmdStatus->hdr.hdr.status;

        if (status == SUCCESS)
        {
          // copy message body
          memcpy(pValue, pCmdStatus->pValue, pCmdStatus->len);
        }

        ICall_freeMsg(pCmdStatus);

        return status;
      }
    }

    return getStatusValueFromErrNo(errno);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
* @brief  Service change indication for Bond Manager
*
* Public function defined in gapbondmgr.h.
*/
bStatus_t GAPBondMgr_ServiceChangeInd(uint16 connHandle, uint8 setParam)
{
  return gapSendParamsCmd(connHandle, setParam, HCI_EXT_GAP_BOND_SERVICE_CHANGE,
                          matchBondMgrServiceChangeIndCS);
}

/*********************************************************************
 * Send Connection Parameter Update request.
 *
 * Public function defined in l2cap.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t L2CAP_ConnParamUpdateReq(uint16 connHandle,
                                   l2capParamUpdateReq_t *pUpdateReq,
                                   uint8 taskId)
{
  size_t allocSize = sizeof(ICall_L2capParamUpdateReq);

  // Allocate message buffer space
  ICall_L2capParamUpdateReq *msg =
    (ICall_L2capParamUpdateReq *)ICall_allocMsg(allocSize);

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_L2CAP_SUBGRP, L2CAP_PARAM_UPDATE_REQ);

    // Set connection handle and copy the request
    msg->connHandle = connHandle;
    msg->pUpdateReq = pUpdateReq;
    msg->taskId = ICall_getLocalMsgEntityId(ICALL_SERVICE_CLASS_BLE_MSG, taskId);

    /* Send the message. */
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchL2capParamUpdateCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * Register a Protocol/Service Multiplexer with L2CAP.
 *
 * Public function defined in l2cap.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t L2CAP_RegisterPsm( l2capPsm_t *pPsm )
{
  size_t allocSize = sizeof(ICall_L2capRegisterPsm);

  // Allocate message buffer space
  ICall_L2capRegisterPsm *msg =
    (ICall_L2capRegisterPsm *)ICall_allocMsg(allocSize);

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_L2CAP_SUBGRP,
                      HCI_EXT_L2CAP_REGISTER_PSM);

    // Set pointer to PSM structure
    msg->pPsm = pPsm;

    /* Send the message. */
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchL2capRegisterPsmCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * Deregister a Protocol/Service Multiplexer with L2CAP.
 *
 * Public function defined in l2cap.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t L2CAP_DeregisterPsm( uint8 taskId, uint16 psm )
{
  size_t allocSize = sizeof(ICall_L2capDeregisterPsm);

  // Allocate message buffer space
  ICall_L2capDeregisterPsm *msg =
    (ICall_L2capDeregisterPsm *)ICall_allocMsg(allocSize);

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_L2CAP_SUBGRP,
                      HCI_EXT_L2CAP_DEREGISTER_PSM);

    // Set task PSM belongs to and PSM to deregister
    msg->taskId = ICall_getLocalMsgEntityId(ICALL_SERVICE_CLASS_BLE_MSG, taskId);
    msg->psm = psm;

    /* Send the message. */
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchL2capDeregisterPsmCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * Get information about a given registered PSM.
 *
 * Public function defined in l2cap.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t L2CAP_PsmInfo( uint16 psm, l2capPsmInfo_t *pInfo )
{
  // Allocate message buffer space
  ICall_L2capPsmInfo *msg =
    (ICall_L2capPsmInfo *)ICall_allocMsg(sizeof(ICall_L2capPsmInfo));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_L2CAP_SUBGRP,
                      HCI_EXT_L2CAP_PSM_INFO);

    // Set psm and pointer to PSM info structure
    msg->psm = psm;
    msg->pInfo = pInfo;

    /* Send the message. */
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchL2capPsmInfoCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * Get all active channels for a given registered PSM.
 *
 * Public function defined in l2cap.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t L2CAP_PsmChannels( uint16 psm, uint8 numCIDs, uint16 *pCIDs )
{
  // Allocate message buffer space
  ICall_L2capPsmChannels *msg =
    (ICall_L2capPsmChannels *)ICall_allocMsg(sizeof(ICall_L2capPsmChannels));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_L2CAP_SUBGRP,
                      HCI_EXT_L2CAP_PSM_CHANNELS);

    // Set psm, number of CIDs and structure to copy CIDs into
    msg->psm = psm;
    msg->numCIDs = numCIDs;
    msg->pCIDs = pCIDs;

    /* Send the message. */
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchL2capPsmChannelsCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * Get information about a given active Connection Oriented Channnel.
 *
 * Public function defined in l2cap.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t L2CAP_ChannelInfo( uint16 CID, l2capChannelInfo_t *pInfo )
{
  // Allocate message buffer space
  ICall_L2capChannelInfo *msg =
    (ICall_L2capChannelInfo *)ICall_allocMsg(sizeof(ICall_L2capChannelInfo));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_L2CAP_SUBGRP,
                      HCI_EXT_L2CAP_CHANNEL_INFO);

    // Set local channel id and structure to copy channel info into
    msg->CID = CID;
    msg->pInfo = pInfo;

    /* Send the message. */
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchL2capChannelInfoCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * Send data packet over an L2CAP connection oriented channel established
 * over a physical connection.
 *
 * Public function defined in l2cap.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t L2CAP_SendSDU( l2capPacket_t *pPkt )
{
  // Allocate message buffer space
  ICall_L2capSendSDU *msg =
    (ICall_L2capSendSDU *)ICall_allocMsg(sizeof(ICall_L2capSendSDU));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_L2CAP_SUBGRP,
                      HCI_EXT_L2CAP_DATA);

    // Set pointer rto packet to be sent
    msg->pPkt = pPkt;

    /* Send the message. */
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchL2capSendSDUCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * Send Connection Request.
 *
 * Public function defined in l2cap.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t L2CAP_ConnectReq( uint16 connHandle, uint16 psm, uint16 peerPsm )
{
  // Allocate message buffer space
  ICall_L2capConnectReq *msg =
    (ICall_L2capConnectReq *)ICall_allocMsg(sizeof(ICall_L2capConnectReq));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_L2CAP_SUBGRP,
                      L2CAP_CONNECT_REQ);

    // Set connection to create channel on, local PSM and peer PSM
    msg->connHandle = connHandle;
    msg->psm = psm;
    msg->peerPsm = peerPsm;

    /* Send the message. */
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchL2capConnectReqCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * Send Connection Response.
 *
 * Public function defined in l2cap.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t L2CAP_ConnectRsp( uint16 connHandle, uint8 id, uint16 result )
{
  // Allocate message buffer space
  ICall_Ll2capConnectRsp *msg =
    (ICall_Ll2capConnectRsp *)ICall_allocMsg(sizeof(ICall_Ll2capConnectRsp));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_L2CAP_SUBGRP,
                      L2CAP_CONNECT_RSP);

    // Set connection handle, id received in connection request and result
    msg->connHandle = connHandle;
    msg->id = id;
    msg->result = result;

    /* Send the message. */
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchL2capConnectRspCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * Send Disconnection Request.
 *
 * Public function defined in l2cap.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t L2CAP_DisconnectReq( uint16 CID )
{
  size_t allocSize = sizeof(ICall_L2capDisconnectReq);

  // Allocate message buffer space
  ICall_L2capDisconnectReq *msg =
    (ICall_L2capDisconnectReq *)ICall_allocMsg(allocSize);

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_L2CAP_SUBGRP,
                      L2CAP_DISCONNECT_REQ);

    // Set local CID to disconnect
    msg->CID = CID;

    /* Send the message. */
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchL2capDisconnectReqCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * Send Flow Control Credit.
 *
 * Public function defined in l2cap.h.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t L2CAP_FlowCtrlCredit( uint16 CID, uint16 peerCredits )
{
  size_t allocSize = sizeof(ICall_L2capFlowCtrlCredit);

  // Allocate message buffer space
  ICall_L2capFlowCtrlCredit *msg =
    (ICall_L2capFlowCtrlCredit *)ICall_allocMsg(allocSize);

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_L2CAP_SUBGRP,
                      L2CAP_FLOW_CTRL_CREDIT);

    // Set local CID and number of credits to give to peer devic
    msg->CID = CID;
    msg->peerCredits = peerCredits;

    /* Send the message. */
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchL2capFCCreditCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * @fn      gattRequest
 *
 * @brief   Send GATT Generic Request.
 *
 * @param   connHandle  connection to use
 * @param   pReq        pointer to request to be sent
 * @param   taskId      task to be notified of response
 * @param   opcode      ATT message opcode
 *
 * @return  SUCCESS or FAILURE
 */
static bStatus_t gattRequest(uint16 connHandle, attMsg_t *pReq, uint8 taskId,
                             uint8 opcode)
{
  ICall_GattReq *msg = (ICall_GattReq *)ICall_allocMsg(sizeof(ICall_GattReq));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GATT_SUBGRP, opcode);

    // Set connection handle and copy the GATT request
    msg->connHandle = connHandle;
    msg->pReq = pReq;
    msg->taskId = ICall_getLocalMsgEntityId(ICALL_SERVICE_CLASS_BLE_MSG, taskId);

    /* Send the message. */
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchGattRequestCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * @fn      gattWriteLong
 *
 * @brief   GATT Write Long Characteristic Value or Descriptor.
 *
 * @param   connHandle connection to use
 * @param   pReq       pointer to request to be sent
 * @param   taskId     task to be notified of response
 * @param   ATT_PREPARE_WRITE_REQ or GATT_WRITE_LONG_CHAR_DESC
 *
 * @return  SUCCESS or FAILURE
 */
static bStatus_t gattWriteLong(uint16 connHandle, attPrepareWriteReq_t *pReq,
                               uint8 taskId, uint8 opcode)
{
  ICall_GattWriteLong *msg =
    (ICall_GattWriteLong *)ICall_allocMsg(sizeof(ICall_GattWriteLong));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GATT_SUBGRP, opcode);

    // Set connection handle and copy the GATT request
    msg->connHandle = connHandle;
    msg->pReq = pReq;
    msg->taskId = ICall_getLocalMsgEntityId(ICALL_SERVICE_CLASS_BLE_MSG, taskId);

    /* Send the message. */
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchGattWriteLongCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * @fn      gattIndNoti
 *
 * @brief   GATT Handle Value Indication or Notification.
 *
 * @param   connHandle     connection to use
 * @param   pIndNoti       pointer to indication/notification to be sent
 * @param   authenticated  whether an authenticated link is required
 * @param   taskId         task to be notified of confirmation
 * @param   ATT_HANDLE_VALUE_IND or ATT_HANDLE_VALUE_NOTI
 *
 * @return  SUCCESS or FAILURE
 */
static bStatus_t gattIndNoti(uint16 connHandle, attMsg_t *pIndNoti,
                             uint8 authenticated, uint8 taskId, uint8 opcode)
{
  ICall_GattInd *msg = (ICall_GattInd *)ICall_allocMsg(sizeof(ICall_GattInd));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GATT_SUBGRP, opcode);

    // Set connection handle and set request
    msg->connHandle = connHandle;
    msg->authenticated = authenticated;
    msg->pIndNoti = pIndNoti;
    msg->taskId = ICall_getLocalMsgEntityId(ICALL_SERVICE_CLASS_BLE_MSG, taskId);

    /* Send the message. */
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchGattIndNotiCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * @fn      GATTServApp_SendCCCUpdatedEvent
 *
 * @brief   Build and send the GATT_CLIENT_CHAR_CFG_UPDATED_EVENT to
 *          the app.
 *
 * @param   connHandle - connection handle
 * @param   attrHandle - attribute handle
 * @param   value - attribute new value
 *
 * @return  none
 */
void GATTServApp_SendCCCUpdatedEvent(uint16 connHandle, uint16 attrHandle,
                                     uint16 value)
{
   ICall_cccUpdateMsg *msg = (ICall_cccUpdateMsg *)ICall_allocMsg(sizeof(ICall_cccUpdateMsg));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GATT_SUBGRP, GATT_CCC_UPDATE);

    // Set connection handle and message data
    msg->connHandle = connHandle;
    msg->attrHandle = attrHandle;
    msg->value = value;

    /* Send the message. */
    sendWaitMatchCS(ICall_getEntityId(), msg, matchCccUpdatedEventCS);
  }
}

/*********************************************************************
 * @fn      GATT_bm_alloc
 *
 * @brief   GATT implementation of the allocator functionality.
 *
 *          Note: This function should only be called by GATT and
 *                the upper layer protocol/application.
 *
 * @param   connHandle - connection that message is to be sent on.
 * @param   opcode - opcode of message that buffer to be allocated for.
 * @param   size - number of bytes to allocate from the heap.
 * @param   pSizeAlloc - number of bytes allocated for the caller from the heap.
 *
 * @return  pointer to the heap allocation; NULL if error or failure.
 */
void *GATT_bm_alloc(uint16 connHandle, uint8 opcode, uint16 size, uint16 *pSizeAlloc)
{
  if (pfnBMAlloc != NULL)
  {
    return (*pfnBMAlloc)(BM_MSG_GATT, size, connHandle, opcode, pSizeAlloc);
  }

  return ((void *)NULL);
}

/*********************************************************************
 * @fn      L2CAP_bm_alloc
 *
 * @brief   L2CAP implementation of the allocator functionality.
 *
 *          Note: This function should only be called by L2CAP and
 *                the upper layer protocol/application.
 *
 * @param   size - number of bytes to allocate from the heap.
 *
 * @return  pointer to the heap allocation; NULL if error or failure.
 */
void *L2CAP_bm_alloc( uint16 size )
{
  if (pfnBMAlloc != NULL)
  {
    return (*pfnBMAlloc)(BM_MSG_L2CAP, size, 0, 0, NULL);
  }

  return ((void *)NULL);
}

/*********************************************************************
 * @fn      GATT_bm_free
 *
 * @brief   GATT implementation of the de-allocator functionality.
 *
 * @param   pMsg - pointer to the message containing the memory to free.
 * @param   opcode - opcode of the message.
 *
 * @return  none
 */
void GATT_bm_free(gattMsg_t *pMsg, uint8 opcode)
{
  if (pfnBMFree != NULL)
  {
    (*pfnBMFree)(BM_MSG_GATT, pMsg, opcode);
  }
}

/*********************************************************************
 * @fn      BM_free
 *
 * @brief   Implementation of the de-allocator functionality.
 *
 * @param   payload_ptr - pointer to the memory to free.
 *
 * @return  none
 */
void BM_free(void *payload_ptr)
{
  if (pfnBMFree != NULL)
  {
    (*pfnBMFree)(BM_MSG_GENERIC, payload_ptr, 0);
  }
}

/*******************************************************************************
 * Send a GAP message and wait for a matching Command Status response.
 *
 * @param   opCode     opcode of the message
 * @param   connHandle connection handle
 * @param   param      parameter of the message
 * @param   matchCSFn  pointer to a function that would return TRUE when
 *                     the message matches its condition
 *
 * @return  SUCCESS or FAILURE
 */
static bStatus_t gapSendParamsCmd(uint16_t connHandle, uint8_t param,
                                  uint8_t cmdId, ICall_MsgMatchFn matchCSFn)
{
  // Allocate message buffer space
  ICall_GapParams *msg =
    (ICall_GapParams *)ICall_allocMsg(sizeof(ICall_GapParams));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GAP_SUBGRP, cmdId);

    // Set pointer to link parameters
    msg->connHandle = connHandle;
    msg->param = param;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchCSFn);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*******************************************************************************
 * Send a GAP message and wait for a matching Command Status response.
 *
 * @param   opCode     opcode of the message
 * @param   pParam1    pointer to the first parameter of the message
 * @param   pParam2    pointer to the second parameter of the message
 * @param   matchCSFn  pointer to a function that would return TRUE when
 *                     the message matches its condition
 *
 * @return  SUCCESS or FAILURE
 */
static bStatus_t gapSendPtrParamsCmd(uint8_t *pParam1, uint8_t *pParam2,
                                     uint8_t cmdId, ICall_MsgMatchFn matchCSFn)
{
  // Allocate message buffer space
  ICall_GapPtrParams *msg =
    (ICall_GapPtrParams *)ICall_allocMsg(sizeof(ICall_GapPtrParams));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GAP_SUBGRP, cmdId);

    // Set pointer to link parameters
    msg->pParam1 = pParam1;
    msg->pParam2 = pParam2;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchCSFn);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*******************************************************************************
 * Send a GAP message and wait for a matching Command Status response.
 *
 * @param   opCode     opcode of the message
 * @param   taskID     task ID
 * @param   pParam     pointer to the parameter of the message
 * @param   matchCSFn  pointer to a function that would return TRUE when
 *                     the message matches its condition
 *
 * @return  SUCCESS or FAILURE
 */
static bStatus_t gapSendParamAndPtrCmd(uint8_t taskID, uint8_t *pParam,
                                       uint8_t cmdId, ICall_MsgMatchFn matchCSFn)
{
  // Allocate message buffer space
  ICall_GapParamAndPtr *msg =
    (ICall_GapParamAndPtr *)ICall_allocMsg(sizeof(ICall_GapParamAndPtr));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GAP_SUBGRP, cmdId);

    // Set pointer to link parameters
    msg->taskID = ICall_getLocalMsgEntityId(ICALL_SERVICE_CLASS_BLE_MSG, taskID);
    msg->pParam = pParam;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchCSFn);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * Compare a received GAP Profile Set/Get Param Value Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 * @param cmdId set or get command id
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGapSetGetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                  const void *msg, uint8 cmdId)
{
  ICall_GapCmdStatus *pMsg = (ICall_GapCmdStatus *)msg;

  if ((pMsg->hdr.hdr.event == ICALL_EVENT_EVENT)              &&
      (pMsg->hdr.eventOpcode == HCI_EXT_GAP_CMD_STATUS_EVENT) &&
      ((pMsg->opCode >> 10) == VENDOR_SPECIFIC_OGF)           &&
      (((pMsg->opCode >> 7) & 0x07) == HCI_EXT_GAP_SUBGRP)    &&
      ((pMsg->opCode & 0x007F) == cmdId))
  {
    return true;
  }

  return false;
}

/*********************************************************************
 * Compare a received GAP Device Init Command Status message for a match
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGapDeviceInitCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                 const void *msg)
{
  return matchGapSetGetParamCS(src, dest, msg, HCI_EXT_GAP_DEVICE_INIT);
}

/*********************************************************************
 * Compare a received GAP Configure Device Address Command Status message
 * for a match
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGapConfigDeviceAddrCS(ICall_ServiceEnum src,
                                       ICall_EntityID dest, const void *msg)
{
  return matchGapSetGetParamCS(src, dest, msg,
                               HCI_EXT_GAP_CONFIG_DEVICE_ADDR);
}

/*********************************************************************
 * Compare a received GAP Update Link Parameters Request Command Status
 * message for a match
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGapUpdateLinkParamReqCS(ICall_ServiceEnum src,
                                         ICall_EntityID dest, const void *msg)
{
  return matchGapSetGetParamCS(src, dest, msg,
                               HCI_EXT_GAP_UPDATE_LINK_PARAM_REQ);
}

/*********************************************************************
 * Compare a received GAP Send Connection Parameters Update Reply Command Status
 * message for a match
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGapConnParamUpdateReplyCS(ICall_ServiceEnum src,
                                           ICall_EntityID dest, const void *msg)
{
  return matchGapSetGetParamCS(src, dest, msg,
                               HCI_EXT_GAP_UPDATE_LINK_PARAM_REQ_REPLY);
}

/*********************************************************************
 * Compare a received GAP Terminate Link Request Command Status message
 * for a match
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGapTerminateLinkReqCS(ICall_ServiceEnum src,
                                       ICall_EntityID dest, const void *msg)
{
  return matchGapSetGetParamCS(src, dest, msg, HCI_EXT_GAP_TERMINATE_LINK);
}

/*********************************************************************
 * Compare a received GAP Update Advertising Data Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGapUpdateAdvDataCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                    const void *msg)
{
  return matchGapSetGetParamCS(src, dest, msg, HCI_EXT_GAP_UPDATE_ADV_DATA);
}

/*********************************************************************
 * Compare a received GAP Set Advertising Token Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGapSetAdvTokenCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                  const void *msg)
{
  return matchGapSetGetParamCS(src, dest, msg, HCI_EXT_GAP_SET_ADV_TOKEN);
}

/*********************************************************************
 * Compare a received GAP Remove Adv Token Command Status message for a match
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGapRemoveAdvTokenCS(ICall_ServiceEnum src,
                                     ICall_EntityID dest, const void *msg)
{
  return matchGapSetGetParamCS(src, dest, msg, HCI_EXT_GAP_REMOVE_ADV_TOKEN);
}

/*********************************************************************
 * Compare a received GAP Update Advertising Token Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGapUpdateAdvTokenCS(ICall_ServiceEnum src,
                                     ICall_EntityID dest, const void *msg)
{
  return matchGapSetGetParamCS(src, dest, msg, HCI_EXT_GAP_UPDATE_ADV_TOKENS);
}

/*********************************************************************
 * Compare a received GAP Authenticate Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGapAuthenticateCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg)
{
  return matchGapSetGetParamCS(src, dest, msg, HCI_EXT_GAP_AUTHENTICATE);
}

/*********************************************************************
 * Compare a received GAP Terminate Authentication Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGapTerminateAuthCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                    const void *msg)
{
  return matchGapSetGetParamCS(src, dest, msg, HCI_EXT_GAP_TERMINATE_AUTH);
}

/*********************************************************************
 * Compare a received GAP Bond Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGapBondCS(ICall_ServiceEnum src, ICall_EntityID dest,
                           const void *msg)
{
  return matchGapSetGetParamCS(src, dest, msg, HCI_EXT_GAP_BOND);
}

/*********************************************************************
 * Compare a received GAP Signable Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGapSignableCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg)
{
  return matchGapSetGetParamCS(src, dest, msg, HCI_EXT_GAP_SIGNABLE);
}

/*********************************************************************
 * Compare a received GAP Update Passkey Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGapPasskeyUpdateCS(ICall_ServiceEnum src,
                                    ICall_EntityID dest, const void *msg)
{
  return matchGapSetGetParamCS(src, dest, msg, HCI_EXT_GAP_PASSKEY_UPDATE);
}

/*********************************************************************
 * Compare a received GAP Set Param Value Command Status message for a match
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGapSetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg)
{
  return matchGapSetGetParamCS(src, dest, msg, HCI_EXT_GAP_SET_PARAM);
}

/*********************************************************************
 * Compare a received GAP Get Param Value Command Status message for a match
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGapGetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg)
{
  return matchGapSetGetParamCS(src, dest, msg, HCI_EXT_GAP_GET_PARAM);
}

/*********************************************************************
 * Compare a received GAP Slave Security Request Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGapSlaveSecurityReqCS(ICall_ServiceEnum src,
                                       ICall_EntityID dest, const void *msg)
{
  return matchGapSetGetParamCS(src, dest, msg,
                               HCI_EXT_GAP_SLAVE_SECURITY_REQ_UPDATE);
}

/*********************************************************************
 * Compare a received GAP Slave Security Request Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchLinkDBStateCS(ICall_ServiceEnum src,
                               ICall_EntityID dest, const void *msg)
{
  return matchProfileCS(src, dest, msg, DISPATCH_GAP_PROFILE,
                        DISPATCH_GAP_LINKDB_STATE);
}

/*********************************************************************
 * Compare a received Link DB Number of Connections Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchLinkDBNumConnsCS(ICall_ServiceEnum src,
                                  ICall_EntityID dest, const void *msg)
{
  return matchProfileCS(src, dest, msg, DISPATCH_GAP_PROFILE,
                        DISPATCH_GAP_LINKDB_NUM_CONNS);
}

/*********************************************************************
 * Compare a received Link DB Number of Active Connections Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchLinkDBNumActiveCS(ICall_ServiceEnum src,
                                   ICall_EntityID dest, const void *msg)
{
  return matchProfileCS(src, dest, msg, DISPATCH_GAP_PROFILE,
                        DISPATCH_GAP_LINKDB_NUM_ACTIVE);
}

/*********************************************************************
 * Compare a received Link DB Get Info Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchLinkDBGetInfoCS(ICall_ServiceEnum src,
                              ICall_EntityID dest, const void *msg)
{
  return matchProfileCS(src, dest, msg, DISPATCH_GAP_PROFILE,
                        DISPATCH_GAP_LINKDB_GET_INFO);
}

/*********************************************************************
 * Compare a received Bond Manager Set Param Value Command Status message
 * for a match
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchBondMgrSetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg)
{
  return matchGapSetGetParamCS(src, dest, msg, HCI_EXT_GAP_BOND_SET_PARAM);
}

/*********************************************************************
 * Compare a received Bond Manager Get Param Value Command Status message
 * for a match
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchBondMgrGetParamCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg)
{
  return matchGapSetGetParamCS(src, dest, msg, HCI_EXT_GAP_BOND_GET_PARAM);
}

/*********************************************************************
 * Compare a received Bond Manager Service Change Indication Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchBondMgrServiceChangeIndCS(ICall_ServiceEnum src,
                                           ICall_EntityID dest, const void *msg)
{
  return matchGapSetGetParamCS(src, dest, msg,
                               HCI_EXT_GAP_BOND_SERVICE_CHANGE);
}

/*********************************************************************
 * Compare a received L2CAPCommand Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 * @param cmdId l2cap command id
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchL2capCmdStatus(ICall_ServiceEnum src, ICall_EntityID dest,
                                const void *msg, uint8_t cmdId)
{
  ICall_GapCmdStatus *pMsg = (ICall_GapCmdStatus *)msg;

  if ((pMsg->hdr.hdr.event == ICALL_EVENT_EVENT)              &&
      (pMsg->hdr.eventOpcode == HCI_EXT_GAP_CMD_STATUS_EVENT) &&
      ((pMsg->opCode >> 10) == VENDOR_SPECIFIC_OGF)           &&
      (((pMsg->opCode >> 7) & 0x07) == HCI_EXT_L2CAP_SUBGRP)  &&
      ((pMsg->opCode & 0x007F) == cmdId))
  {
    return true;
  }

  return false;
}

/*********************************************************************
 * Compare a received L2CAP Connection Param Update Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchL2capParamUpdateCS(ICall_ServiceEnum src,
                                    ICall_EntityID dest, const void *msg)
{
  return matchL2capCmdStatus(src, dest, msg, L2CAP_PARAM_UPDATE_REQ);
}

/*********************************************************************
 * Compare a received L2CAP Register PSM Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchL2capRegisterPsmCS(ICall_ServiceEnum src,
                                    ICall_EntityID dest, const void *msg)
{
  return matchL2capCmdStatus(src, dest, msg, HCI_EXT_L2CAP_REGISTER_PSM);
}

/*********************************************************************
 * Compare a received L2CAP Deregister PSM Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchL2capDeregisterPsmCS(ICall_ServiceEnum src,
                                      ICall_EntityID dest, const void *msg)
{
  return matchL2capCmdStatus(src, dest, msg, HCI_EXT_L2CAP_DEREGISTER_PSM);
}

/*********************************************************************
 * Compare a received L2CAP PSM Info Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchL2capPsmInfoCS(ICall_ServiceEnum src,
                                ICall_EntityID dest, const void *msg)
{
  return matchL2capCmdStatus(src, dest, msg, HCI_EXT_L2CAP_PSM_INFO);
}

/*********************************************************************
 * Compare a received L2CAP PSM Channels Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchL2capPsmChannelsCS(ICall_ServiceEnum src,
                                    ICall_EntityID dest, const void *msg)
{
  return matchL2capCmdStatus(src, dest, msg, HCI_EXT_L2CAP_PSM_CHANNELS);
}

/*********************************************************************
 * Compare a received L2CAP Channel Info Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchL2capChannelInfoCS(ICall_ServiceEnum src,
                                    ICall_EntityID dest, const void *msg)
{
  return matchL2capCmdStatus(src, dest, msg, HCI_EXT_L2CAP_CHANNEL_INFO);
}

/*********************************************************************
 * Compare a received L2CAP Send SDU Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchL2capSendSDUCS(ICall_ServiceEnum src,
                                ICall_EntityID dest, const void *msg)
{
  return matchL2capCmdStatus(src, dest, msg, HCI_EXT_L2CAP_DATA);
}

/*********************************************************************
 * Compare a received L2CAP Connection Request Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchL2capConnectReqCS(ICall_ServiceEnum src,
                                   ICall_EntityID dest, const void *msg)
{
  return matchL2capCmdStatus(src, dest, msg, L2CAP_CONNECT_REQ);
}

/*********************************************************************
 * Compare a received L2CAP Connection Response Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchL2capConnectRspCS(ICall_ServiceEnum src,
                                   ICall_EntityID dest, const void *msg)
{
  return matchL2capCmdStatus(src, dest, msg, L2CAP_CONNECT_RSP);
}

/*********************************************************************
 * Compare a received L2CAP Disconnection Request Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchL2capDisconnectReqCS(ICall_ServiceEnum src,
                                      ICall_EntityID dest, const void *msg)
{
  return matchL2capCmdStatus(src, dest, msg, L2CAP_DISCONNECT_REQ);
}

/*********************************************************************
 * Compare a received L2CAP Flow Control Credit Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchL2capFCCreditCS(ICall_ServiceEnum src,
                                 ICall_EntityID dest, const void *msg)
{
  return matchL2capCmdStatus(src, dest, msg, L2CAP_FLOW_CTRL_CREDIT);
}

/*********************************************************************
 * Compare a received GATT Request Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGattRequestCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg)
{
  ICall_GapCmdStatus *pMsg = (ICall_GapCmdStatus *)msg;

  if ((pMsg->hdr.hdr.event == ICALL_EVENT_EVENT)              &&
      (pMsg->hdr.eventOpcode == HCI_EXT_GAP_CMD_STATUS_EVENT) &&
      ((pMsg->opCode >> 10) == VENDOR_SPECIFIC_OGF)           &&
      (((pMsg->opCode >> 7) & 0x07) == HCI_EXT_GATT_SUBGRP)   &&
      ((pMsg->opCode & 0x007F) > 0))
  {
    return true;
  }

  return false;
}

/*********************************************************************
 * Compare a received GATT Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 * @param cmdId command id
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGattCmdStatus(ICall_ServiceEnum src, ICall_EntityID dest,
                                const void *msg, uint8_t cmdId)
{
  ICall_GapCmdStatus *pMsg = (ICall_GapCmdStatus *)msg;

  if ((pMsg->hdr.hdr.event == ICALL_EVENT_EVENT)              &&
      (pMsg->hdr.eventOpcode == HCI_EXT_GAP_CMD_STATUS_EVENT) &&
      ((pMsg->opCode >> 10) == VENDOR_SPECIFIC_OGF)           &&
      (((pMsg->opCode >> 7) & 0x07) == HCI_EXT_GATT_SUBGRP)   &&
      ((pMsg->opCode & 0x007F) == cmdId))
  {
    return true;
  }

  return false;
}

/*********************************************************************
 * Compare a received GATT Discover All Primary Services Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 * @param cmdId command id
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGattDiscAllPrimarySvcCS(ICall_ServiceEnum src,
                                         ICall_EntityID dest, const void *msg)
{
  return matchGattCmdStatus(src, dest, msg, ATT_READ_BY_GRP_TYPE_REQ);
}

/*********************************************************************
 * Compare a received GATT Discover Primary Service By UUID Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 * @param cmdId command id
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGattDiscPrimartSvcCS(ICall_ServiceEnum src,
                                      ICall_EntityID dest, const void *msg)
{
  return matchGattCmdStatus(src, dest, msg, ATT_FIND_BY_TYPE_VALUE_REQ);
}

/*********************************************************************
 * Compare a received GATT Find Included Services Command Status message
 *  for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 * @param cmdId command id
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGattFindIncludedSvcCS(ICall_ServiceEnum src,
                                       ICall_EntityID dest, const void *msg)
{
  return matchGattCmdStatus(src, dest, msg, GATT_FIND_INCLUDED_SERVICES);
}

/*********************************************************************
 * Compare a received GATT Discover All Characteristics Command Status
 *  message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 * @param cmdId command id
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGattDiscAllCharsCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                    const void *msg)
{
  return matchGattCmdStatus(src, dest, msg, GATT_DISC_ALL_CHARS);
}

/*********************************************************************
 * Compare a received GATT Reliable Writes Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 * @param cmdId command id
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGattReliableWritesCS(ICall_ServiceEnum src,
                                      ICall_EntityID dest, const void *msg)
{
  return matchGattCmdStatus(src, dest, msg, GATT_RELIABLE_WRITES);
}

/*********************************************************************
 * Compare a received GATT get next handle Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 * @param cmdId command id
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGattGetNexthandleCS(ICall_ServiceEnum src,
                                      ICall_EntityID dest, const void *msg)
{
  return matchUtilNvCS(src, dest, msg, UTIL_EXT_GATT_GET_NEXT_HANDLE);
}


/*********************************************************************
 * Compare a received GATT Write Long Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 * @param cmdId command id
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGattWriteLongCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                 const void *msg)
{
  return (matchGattCmdStatus(src, dest, msg, ATT_PREPARE_WRITE_REQ)   ||
          matchGattCmdStatus(src, dest, msg, GATT_WRITE_LONG_CHAR_DESC));
}

/*********************************************************************
 * Compare a received GATT Indication or Notification Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 * @param cmdId command id
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGattIndNotiCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg)
{
  return (matchGattCmdStatus(src, dest, msg, ATT_HANDLE_VALUE_IND) ||
          matchGattCmdStatus(src, dest, msg, ATT_HANDLE_VALUE_NOTI));
}

/*********************************************************************
 * Compare a received CCC Updated Event Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 * @param cmdId command id
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchCccUpdatedEventCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg)
{
  return (matchGattCmdStatus(src, dest, msg, GATT_CCC_UPDATE));
}

/*********************************************************************
 * Compare a received ATT Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 * @param cmdId command id
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchAttCmdStatus(ICall_ServiceEnum src, ICall_EntityID dest,
                              const void *msg, uint8_t cmdId)
{
  ICall_GapCmdStatus *pMsg = (ICall_GapCmdStatus *)msg;

  if ((pMsg->hdr.hdr.event == ICALL_EVENT_EVENT)              &&
      (pMsg->hdr.eventOpcode == HCI_EXT_GAP_CMD_STATUS_EVENT) &&
      ((pMsg->opCode >> 10) == VENDOR_SPECIFIC_OGF)           &&
      (((pMsg->opCode >> 7) & 0x07) == HCI_EXT_ATT_SUBGRP)    &&
      ((pMsg->opCode & 0x007F) == cmdId))
  {
    return true;
  }

  return false;
}

/*********************************************************************
 * Compare a received ATT Error Response Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchAttErrorRspCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg)
{
  return matchAttCmdStatus(src, dest, msg, ATT_ERROR_RSP);
}

/*********************************************************************
 * Compare a received ATT Read Response Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchAttReadRspCS(ICall_ServiceEnum src, ICall_EntityID dest,
                              const void *msg)
{
  return matchAttCmdStatus(src, dest, msg, ATT_READ_RSP);
}

/*********************************************************************
 * Compare a received ATT Read Blob Response Command Status message for
 * a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchAttReadBlobRspCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                  const void *msg)
{
  return matchAttCmdStatus(src, dest, msg, ATT_READ_BLOB_RSP);
}

/*********************************************************************
 * Compare a received ATT Write Response Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchAttWriteRspCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg)
{
  return matchAttCmdStatus(src, dest, msg, ATT_WRITE_RSP);
}

/*********************************************************************
 * Compare a received ATT Execute Write Response Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchAttExecuteWriteRspCS(ICall_ServiceEnum src,
                                      ICall_EntityID dest, const void *msg)
{
  return matchAttCmdStatus(src, dest, msg, ATT_EXECUTE_WRITE_RSP);
}

/*********************************************************************
 * Compare a received ATT Handle Value Confirmation Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchAttValueHandleCfmCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                     const void *msg)
{
  return matchAttCmdStatus(src, dest, msg, ATT_HANDLE_VALUE_CFM);
}

/*********************************************************************
 * Compare a received GAP Bond Manager Passcode Response Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGapPasscodeCS(ICall_ServiceEnum src,
                               ICall_EntityID dest, const void *msg)
{
  return matchProfileCS(src, dest, msg, DISPATCH_GAP_PROFILE,
                        DISPATCH_GAP_BOND_PASSCODE_RSP);
}

/*********************************************************************
 * Compare a received GAP Bond Manager Resolve Address Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGapResolveAddrCS(ICall_ServiceEnum src,
                                  ICall_EntityID dest, const void *msg)
{
  return matchProfileCS(src, dest, msg, DISPATCH_GAP_PROFILE,
                        DISPATCH_GAP_BOND_FIND_ADDR);
}

/*********************************************************************
 * Compare a received GAP Bond Manager Link Established Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGapLinkEstCS(ICall_ServiceEnum src,
                              ICall_EntityID dest, const void *msg)
{
  return matchProfileCS(src, dest, msg, DISPATCH_GAP_PROFILE,
                        DISPATCH_GAP_BOND_LINK_EST);
}

/*********************************************************************
 * Compare a received GATT Client Init Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGattInitClientCS(ICall_ServiceEnum src,
                                  ICall_EntityID dest, const void *msg)
{
  return matchProfileCS(src, dest, msg, DISPATCH_GATT_PROFILE,
                        DISPATCH_GATT_INIT_CLIENT);
}

/*********************************************************************
 * Compare a received GATT Send Response Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGattSendRspCS(ICall_ServiceEnum src,
                               ICall_EntityID dest, const void *msg)
{
  return matchProfileCS(src, dest, msg, DISPATCH_GATT_PROFILE,
                        DISPATCH_GATT_SEND_RSP);
}

/*********************************************************************
 * Compare a received GATT Server App Register Service Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGSARegisterServiceCS(ICall_ServiceEnum src,
                                      ICall_EntityID dest, const void *msg)
{
  return matchProfileCS(src, dest, msg, DISPATCH_GATT_SERV_APP,
                        DISPATCH_PROFILE_REG_SERVICE);
}

/*********************************************************************
 * Compare a received GATT Server App Deregister Service Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGSADeregisterServiceCS(ICall_ServiceEnum src,
                                        ICall_EntityID dest, const void *msg)
{
  return matchProfileCS(src, dest, msg, DISPATCH_GATT_SERV_APP,
                        DISPATCH_PROFILE_DEREG_SERVICE);
}

/*********************************************************************
 * Compare a received GATT Server App Service Changed Indication Command
 * Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGSAServiceChangedIndCS(ICall_ServiceEnum src,
                                        ICall_EntityID dest, const void *msg)
{
  return matchProfileCS(src, dest, msg, DISPATCH_GATT_SERV_APP,
                        DISPATCH_GSA_SERVICE_CHANGE_IND);
}

/*********************************************************************
 * Compare a received GATT Server App Read Response Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchGSAReadRspCS(ICall_ServiceEnum src,
                              ICall_EntityID dest, const void *msg)
{
  return matchProfileCS(src, dest, msg, DISPATCH_GATT_SERV_APP,
                        DISPATCH_PROFILE_DELAYED_RSP);
}

/*********************************************************************
 * @brief   Generate a Secure Connections F4 confirmation value (OOB only)
 *
 * Public function defined in sm.h.
 */
bStatus_t SM_GetScConfirmOob(uint8 *publicKey, uint8 *oob, uint8 *pOut)
{
  size_t allocSize = sizeof(ICall_SMGetScConfirmOob);

  /* Allocate message buffer space */
  ICall_SMGetScConfirmOob *msg =
    (ICall_SMGetScConfirmOob *)ICall_allocMsg(allocSize);

  if (msg)
  {
    ICall_Errno errno;

    //setDispatchCmdEvtHdr(&msg->hdr, DISPATCH_SM,
    //                     DISPATCH_SM_GET_SC_CONFIRM_OOB);
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_GAP_SUBGRP, HCI_EXT_SM_GET_CONFIRM_OOB);

    // Set service change parameters
    msg->publicKey = publicKey;
    msg->oob = oob;

    // Send the message
    errno = ICall_sendServiceMsg(ICall_getEntityId(), ICALL_SERVICE_CLASS_BLE,
                                 ICALL_MSG_FORMAT_3RD_CHAR_TASK_ID, msg);

    // Send the message
    //return sendWaitMatchCS(ICall_getEntityId(), msg, matchSMGetScConfirmCS);

    if (errno == ICALL_ERRNO_SUCCESS)
    {
      ICall_GapCmdStatus *pCmdStatus = NULL;

      errno = waitMatchCS(matchSMGetScConfirmCS, (void **)&pCmdStatus);

      if (errno == ICALL_ERRNO_SUCCESS)
      {
        uint8 status = pCmdStatus->hdr.hdr.status;

        if (status == SUCCESS)
        {
          // copy message body
          memcpy(pOut, pCmdStatus->pValue, pCmdStatus->len);
        }

        ICall_freeMsg(pCmdStatus);

        return status;
      }
    }

    return getStatusValueFromErrNo(errno);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * Compare a received SM Get DH Key Command Status message for a match
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchSMGetScConfirmCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                  const void *msg)
{
  //return matchProfileCS(src, dest, msg, DISPATCH_SM,
  //                      DISPATCH_SM_GET_SC_CONFIRM_OOB);
  return matchGapSetGetParamCS(src, dest, msg, HCI_EXT_SM_GET_CONFIRM_OOB);

}

/*********************************************************************
 * BT/LE HCI Commands: Link Layer
 */

/*******************************************************************************
 * This BT API is used to request version information from the the remote
 * device in a connection.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_ReadRemoteVersionInfoCmd(uint16 connHandle)
{
  return hciSendParamsCmd(HCI_READ_REMOTE_VERSION_INFO, connHandle,
                          HCI_NO_PARAM, HCI_NO_PARAM, matchHciReadRemoteVersionInfoCS);
}

/*******************************************************************************
 * This BT API is used to set the HCI event mask, which is used to determine
 * which events are supported.
 *
 * Note: The global pHciEvtMask is used for BT events. A different global is
 *       used for LE events: bleEvtMask.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_SetEventMaskCmd(uint8 *pMask)
{
  return hciSendPtrParamsCmd(HCI_SET_EVENT_MASK, pMask,
                             NULL, NULL, matchHciSetEventMaskCS);
}

/*******************************************************************************
 * This BT API is used to set the HCI event mask page 2, which is used to
 * determine which events are supported.
 *
 * Note: The global pHciEvtMask2 is used for BT events. A different global is
 *       used for LE events: bleEvtMask.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_SetEventMaskPage2Cmd(uint8 *pMask)
{
  return hciSendPtrParamsCmd(HCI_SET_EVENT_MASK_PAGE_2, pMask,
                             NULL, NULL, matchHciSetEventMaskPage2CS);
}

/*******************************************************************************
 *
 * This BT API is used to reset the Link Layer.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_ResetCmd( void )
{
  return hciSendCmd(HCI_RESET, matchHciResetCS);
}

/*******************************************************************************
 *
 * This BT API is used to read the transmit power level.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_ReadTransmitPowerLevelCmd(uint16 connHandle, uint8 txPwrType)
{
  return hciSendParamsCmd(HCI_READ_TRANSMIT_POWER, connHandle, txPwrType,
                          HCI_NO_PARAM, matchHciReadTxPwrLvlCS);
}

/*******************************************************************************
 * This BT API is used to read the local version information.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_ReadLocalVersionInfoCmd(void)
{
  return hciSendCmd(HCI_READ_LOCAL_VERSION_INFO, matchHciReadLocalVerInfoCS);
}

/*******************************************************************************
 * This BT API is used to read the locally supported commands.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_ReadLocalSupportedCommandsCmd(void)
{
  return hciSendCmd(HCI_READ_LOCAL_SUPPORTED_COMMANDS,
                    matchHciReadLocalSupportedCommandsCS);
}

/*******************************************************************************
 * This BT API is used to read the locally supported features.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_ReadLocalSupportedFeaturesCmd(void)
{
  return hciSendCmd(HCI_READ_LOCAL_SUPPORTED_FEATURES,
                    matchHciReadLocalSupportedFeaturesCS);
}

/*******************************************************************************
 * This API is used to read this device's BLE address (BDADDR).
 *
 * Note: This command is only allowed when the device's state is Standby.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_ReadBDADDRCmd(void)
{
  return hciSendCmd(HCI_READ_BDADDR, matchHciReadBdAddrCS);
}

/*******************************************************************************
 * This BT API is used to read the RSSI.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_ReadRssiCmd(uint16 connHandle)
{
  return hciSendParamsCmd(HCI_READ_RSSI, connHandle, HCI_NO_PARAM,
                          HCI_NO_PARAM, matchHciReadRssiCS);
}

/*******************************************************************************
 * This LE API is used to set the HCI LE event mask, which is used to determine
 * which LE events are supported.
 *
 * Note: The global bleEvtMask is used for LE events. A different global is used
 *       for BT events: pHciEvtMask.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetEventMaskCmd(uint8 *pEventMask)
{
  return hciSendPtrParamsCmd(HCI_LE_SET_EVENT_MASK, pEventMask,
                             NULL, NULL, matchHciLeSetEventMaskCS);
}

/*******************************************************************************
 * This LE API is used to read the LE locally supported features.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadLocalSupportedFeaturesCmd(void)
{
  return hciSendCmd(HCI_LE_READ_LOCAL_SUPPORTED_FEATURES,
                    matchHciLeReadLocalSupportedFeaturesCS);
}

/*******************************************************************************
 * This LE API is used to read transmit power when Advertising.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadAdvChanTxPowerCmd(void)
{
  return hciSendCmd(HCI_LE_READ_ADV_CHANNEL_TX_POWER,
                    matchHciLeReadAdvChanTxPowerCS);
}

/*******************************************************************************
 * This LE API is used to read the total number of white list entries that can
 * be stored in the Controller.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadWhiteListSizeCmd(void)
{
  return hciSendCmd(HCI_LE_READ_WHITE_LIST_SIZE, matchHciLeReadWhiteListSizeCS);
}

/*******************************************************************************
 * This LE API is used to clear the white list.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ClearWhiteListCmd(void)
{
  return hciSendCmd(HCI_LE_CLEAR_WHITE_LIST, matchHciLeClearWhiteListCS);
}

/*******************************************************************************
 * This LE API is used to add a white list entry.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_AddWhiteListCmd(uint8 addrType, uint8 *devAddr)
{
  return hciSendParamAndPtrCmd(HCI_LE_ADD_WHITE_LIST, addrType,
                               devAddr, matchHciLeAddWhiteListCS);
}

/*******************************************************************************
 * This LE API is used to remove a white list entry.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_RemoveWhiteListCmd(uint8 addrType, uint8 *devAddr)
{
  return hciSendParamAndPtrCmd(HCI_LE_REMOVE_WHITE_LIST, addrType,
                               devAddr, matchHciLeRemoveWhiteListCS);
}

/*******************************************************************************
 * This LE API is used to update the current data channel map.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetHostChanClassificationCmd(uint8 *chanMap)
{
  return hciSendPtrParamsCmd(HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION, chanMap,
                             NULL, NULL, matchHciLeSetHostChanClassificationCS);
}

/*******************************************************************************
 * This LE API is used to read a connection's data channel map.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadChannelMapCmd(uint16 connHandle)
{
  return hciSendParamsCmd(HCI_LE_READ_CHANNEL_MAP, connHandle,
                          HCI_NO_PARAM, HCI_NO_PARAM,
                          matchHciLeReadChannelMapCS);
}

/*******************************************************************************
 * This LE API is used to read the remote device's used features.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadRemoteUsedFeaturesCmd(uint16 connHandle)
{
  return hciSendParamsCmd(HCI_LE_READ_REMOTE_USED_FEATURES, connHandle,
                          HCI_NO_PARAM, HCI_NO_PARAM,
                          matchHciLeReadRemoteUsedFeaturesCS);
}

/*******************************************************************************
 * This LE API is used to perform an encryption using AES128.
 *
 * Note: Input parameters are ordered MSB..LSB.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_EncryptCmd(uint8 *key, uint8 *plainText)
{
  return hciSendPtrParamsCmd(HCI_LE_ENCRYPT, key,
                             plainText, NULL, matchHciLeEncryptCS);
}

/*******************************************************************************
 * This LE API is used to read the Controller's supported states.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadSupportedStatesCmd(void)
{
  return hciSendCmd(HCI_LE_READ_SUPPORTED_STATES,
                    matchHciLeReadSupportedStatesCS);
}

/*******************************************************************************
 * This LE API is used to read the maximum supported transmit and
 * receive payload octets and packet duration times.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadMaxDataLenCmd(void)
{
  return hciSendCmd(HCI_LE_READ_MAX_DATA_LENGTH,
                    matchHciLeReadMaxDataLenCS);
}

/*******************************************************************************
 * Compare a received HCI LE Read Max Data Length Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciLeReadMaxDataLenCS(ICall_ServiceEnum src,
                                       ICall_EntityID dest,
                                       const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_LE_READ_MAX_DATA_LENGTH);
}

/*******************************************************************************
 * This LE API is used to read the default maximum trasmit packet
 * size and the default maximum packet transmit time to be used
 * for new connections.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadSuggestedDefaultDataLenCmd(void)
{
  return hciSendCmd(HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH,
                    matchHciLeReadSuggestedDefaultDataLenCS);
}

/*******************************************************************************
 * Compare a received HCI LE Read Suggested Default Data Length Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciLeReadSuggestedDefaultDataLenCS(ICall_ServiceEnum src,
                                                    ICall_EntityID dest,
                                                    const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH);
}

/*******************************************************************************
 * This LE API is used to write the default maximum trasmit packet
 * size and the default maximum packet transmit time to be used
 * for new connections.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_WriteSuggestedDefaultDataLenCmd(uint16 txOctets,
                                                   uint16 txTime)
{
  return hciSendParamsCmd(HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH,
                          txOctets, txTime, HCI_NO_PARAM,
                          matchHciLeWriteSuggestedDefaultDataLenCS);
}

/*******************************************************************************
 * Compare a received HCI LE Write Suggested Default Data Length Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciLeWriteSuggestedDefaultDataLenCS(ICall_ServiceEnum src,
                                                    ICall_EntityID dest,
                                                    const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH);
}

/*******************************************************************************
 * This LE API is used to set the maximum transmission packet size
 * and the maximum packet transmission time for the connection.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetDataLenCmd(uint16 connHandle,
                                 uint16 txOctets,
                                 uint16 txTime)
{
  return hciSendParamsCmd(HCI_LE_SET_DATA_LENGTH, connHandle,
                          txOctets, txTime, matchHciLeSetDataLenCS);
}

/*******************************************************************************
 * Compare a received HCI LE Set Data Length Command Status message for
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciLeSetDataLenCS(ICall_ServiceEnum src,
                                   ICall_EntityID dest,
                                   const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_LE_SET_DATA_LENGTH);
}

/*******************************************************************************
 * This LE API is used to start the transmit Direct Test Mode test.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_TransmitterTestCmd(uint8 txChan,
                                      uint8 dataLen,
                                      uint8 payloadType)
{
  ICall_HciLe_TxTest *msg =
    (ICall_HciLe_TxTest *)ICall_allocMsg(sizeof(ICall_HciLe_TxTest));

  // Note: All HCI Ext Command definitions include OGF (63) and CSG (0): 0xFC00
  if (msg)
  {
    // Note that srctaskid shall be filled in via sendmsg()
    msg->hdr.hdr.event = ICALL_CMD_EVENT;
    msg->hdr.pktType   = HCI_CMD_PACKET;
    msg->hdr.opCode    = HCI_LE_TRANSMITTER_TEST;

    // Set message body
    msg->txChan      = txChan;
    msg->dataLen     = dataLen;
    msg->payloadType = payloadType;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchHciLeTxTestCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*******************************************************************************
 * This LE API is used to start the receiver Direct Test Mode test.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReceiverTestCmd(uint8 rxFreq)
{
  return hciSendParamsCmd(HCI_LE_RECEIVER_TEST, rxFreq, HCI_NO_PARAM,
                          HCI_NO_PARAM, matchHciLeRxTestCS);
}

/*******************************************************************************
 * This LE API is used to end the Direct Test Mode test.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_TestEndCmd(void)
{
  return hciSendCmd(HCI_LE_TEST_END, matchHciLeTestEndCS);
}

/*******************************************************************************
 * Send an HCI message and wait for a matching Command Status response.
 *
 * @param   opCode     opcode of the message
 * @param   matchCSFn  pointer to a function that would return TRUE when
 *                     the message matches its condition
 *
 * @return  SUCCESS or FAILURE
 */
static hciStatus_t hciSendCmd(uint16_t opCode, ICall_MsgMatchFn matchCSFn)
{
  ICall_HciExtCmd *msg =
    (ICall_HciExtCmd *)ICall_allocMsg(sizeof(ICall_HciExtCmd));

  // Note: All HCI Ext Command definitions include OGF (63) and CSG (0): 0xFC00
  if (msg)
  {
    // Note that srctaskid shall be filled in via sendmsg()
    msg->hdr.event = ICALL_CMD_EVENT;
    msg->pktType   = HCI_CMD_PACKET;
    msg->opCode    = opCode;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchCSFn);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*******************************************************************************
 * Send an HCI message and wait for a matching Command Status response.
 *
 * @param   opCode     opcode of the message
 * @param   param1     first parameter of the message
 * @param   param2     second parameter of the message
 * @param   param3     third parameter of the message
 * @param   matchCSFn  pointer to a function that would return TRUE when
 *                     the message matches its condition
 *
 * @return  SUCCESS or FAILURE
 */
static hciStatus_t hciSendParamsCmd(uint16_t opCode,
                                    uint16_t param1,
                                    uint16_t param2,
                                    uint16_t param3,
                                    ICall_MsgMatchFn matchCSFn)
{
  ICall_Hci_Params *msg =
    (ICall_Hci_Params *)ICall_allocMsg(sizeof(ICall_Hci_Params));

  // Note: All HCI Ext Command definitions include OGF (63) and CSG (0): 0xFC00
  if (msg)
  {
    // Note that srctaskid shall be filled in via sendmsg()
    msg->hdr.hdr.event = ICALL_CMD_EVENT;
    msg->hdr.pktType   = HCI_CMD_PACKET;
    msg->hdr.opCode    = opCode;

    // Set message parameters
    msg->param1 = param1;
    msg->param2 = param2;
    msg->param3 = param3;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchCSFn);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*******************************************************************************
 * Send an HCI message and wait for a matching Command Status response.
 *
 * @param   opCode     opcode of the message
 * @param   pParam1    pointer to the first parameter of the message
 * @param   pParam2    pointer to the second parameter of the message
 * @param   pParam3    pointer to the third parameter of the message
 * @param   matchCSFn  pointer to a function that would return TRUE when
 *                     the message matches its condition
 *
 * @return  SUCCESS or FAILURE
 */
static hciStatus_t hciSendPtrParamsCmd( uint16_t opCode,
                                        uint8_t *pParam1,
                                        uint8_t *pParam2,
                                        uint8_t *pParam3,
                                        ICall_MsgMatchFn matchCSFn )
{
  ICall_Hci_PtrParams *msg =
    (ICall_Hci_PtrParams *)ICall_allocMsg(sizeof(ICall_Hci_PtrParams));

  // Note: All HCI Ext Command definitions include OGF (63) and CSG (0): 0xFC00
  if (msg)
  {
    // Note that srctaskid shall be filled in via sendmsg()
    msg->hdr.hdr.event = ICALL_CMD_EVENT;
    msg->hdr.pktType   = HCI_CMD_PACKET;
    msg->hdr.opCode    = opCode;

    // Set message pointer parameters
    msg->pParam1 = pParam1;
    msg->pParam2 = pParam2;
    msg->pParam3 = pParam3;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchCSFn);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*******************************************************************************
 * Send an HCI message and wait for a matching Command Status response.
 *
 * @param   opCode     opcode of the message
 * @param   param      parameter of the message
 * @param   pParam     pointer to parameter of the message
 * @param   matchCSFn  pointer to a function that would return TRUE when
 *                     the message matches its condition
 *
 * @return  SUCCESS or FAILURE
 */
static hciStatus_t hciSendParamAndPtrCmd(uint16_t opCode,
                                         uint16_t param, uint8_t *pParam,
                                         ICall_MsgMatchFn matchCSFn)
{
  ICall_Hci_ParamAndPtr *msg =
    (ICall_Hci_ParamAndPtr *)ICall_allocMsg(sizeof(ICall_Hci_ParamAndPtr));

  // Note: All HCI Ext Command definitions include OGF (63) and CSG (0): 0xFC00
  if (msg)
  {
    // Note that srctaskid shall be filled in via sendmsg()
    msg->hdr.hdr.event = ICALL_CMD_EVENT;
    msg->hdr.pktType   = HCI_CMD_PACKET;
    msg->hdr.opCode    = opCode;

    // Set message parameter and pointer
    msg->param = param;
    msg->pParam = pParam;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchCSFn);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * Compare a received HCI Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 * @param cmdId command id
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciCS(ICall_ServiceEnum src, ICall_EntityID dest,
                       const void *msg, uint16 cmdId)
{
  ICall_GapCmdStatus *pMsg = (ICall_GapCmdStatus *)msg;

  if ((pMsg->hdr.hdr.event == ICALL_EVENT_EVENT)               &&
       (pMsg->hdr.eventOpcode == HCI_EXT_GAP_CMD_STATUS_EVENT) &&
       (pMsg->opCode == cmdId))
  {
    return true;
  }

  return false;
}

/*********************************************************************
 * Compare a received HCI Read Remote Version Info Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciReadRemoteVersionInfoCS(ICall_ServiceEnum src,
                                            ICall_EntityID dest,
                                            const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_READ_REMOTE_VERSION_INFO);
}

/*********************************************************************
 * Compare a received HCI Set Event Mask Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciSetEventMaskCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_SET_EVENT_MASK);
}

/*********************************************************************
 * Compare a received HCI Set Event Mask Page 2 Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciSetEventMaskPage2CS(ICall_ServiceEnum src,
                                        ICall_EntityID dest,
                                        const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_SET_EVENT_MASK_PAGE_2);
}

/*********************************************************************
 * Compare a received HCI Reset Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciResetCS(ICall_ServiceEnum src, ICall_EntityID dest,
                            const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_RESET);
}

/*********************************************************************
 * Compare a received HCI Ext Read Tx Power Level Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciReadTxPwrLvlCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_READ_TRANSMIT_POWER);
}

/*********************************************************************
 * Compare a received HCI Ext Read Tx Power Level Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciReadLocalVerInfoCS(ICall_ServiceEnum src,
                                       ICall_EntityID dest,
                                       const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_READ_LOCAL_VERSION_INFO);
}

/*********************************************************************
 * Compare a received HCI Ext Read Local Supported Commands Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciReadLocalSupportedCommandsCS(ICall_ServiceEnum src,
                                                 ICall_EntityID dest,
                                                 const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_READ_LOCAL_SUPPORTED_COMMANDS);
}

/*********************************************************************
 * Compare a received HCI Ext Read Local Supported Features Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciReadLocalSupportedFeaturesCS(ICall_ServiceEnum src,
                                                 ICall_EntityID dest,
                                                 const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_READ_LOCAL_SUPPORTED_FEATURES);
}

/*********************************************************************
 * Compare a received HCI Read BD Address Command Status message for
 * a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciReadBdAddrCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                 const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_READ_BDADDR);
}

/*********************************************************************
 * Compare a received HCI Read RSSI Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciReadRssiCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_READ_RSSI);
}

/*********************************************************************
 * Compare a received HCI LE Set Event Mask Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciLeSetEventMaskCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                     const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_LE_SET_EVENT_MASK);
}

/*********************************************************************
 * Compare a received HCI LE Read Local Supported Features Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciLeReadLocalSupportedFeaturesCS(ICall_ServiceEnum src,
                                                   ICall_EntityID dest,
                                                   const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_LE_READ_LOCAL_SUPPORTED_FEATURES);
}

/*********************************************************************
 * Compare a received HCI LE Read Adv Chan Tx Power Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciLeReadAdvChanTxPowerCS(ICall_ServiceEnum src,
                                           ICall_EntityID dest,
                                           const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_LE_READ_ADV_CHANNEL_TX_POWER);
}

/*********************************************************************
 * Compare a received HCI LE Read White List Size Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciLeReadWhiteListSizeCS(ICall_ServiceEnum src,
                                          ICall_EntityID dest,
                                          const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_LE_READ_WHITE_LIST_SIZE);
}

/*********************************************************************
 * Compare a received HCI LE Clear White List Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciLeClearWhiteListCS(ICall_ServiceEnum src,
                                       ICall_EntityID dest,
                                       const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_LE_CLEAR_WHITE_LIST);
}

/*********************************************************************
 * Compare a received HCI LE Add White List Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciLeAddWhiteListCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                     const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_LE_ADD_WHITE_LIST);
}

/*********************************************************************
 * Compare a received HCI LE Add Remove List Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciLeRemoveWhiteListCS(ICall_ServiceEnum src,
                                        ICall_EntityID dest,
                                        const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_LE_REMOVE_WHITE_LIST);
}

/*********************************************************************
 * Compare a received HCI LE Set Host Channel Classification Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciLeSetHostChanClassificationCS(ICall_ServiceEnum src,
                                                  ICall_EntityID dest,
                                                  const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION);
}

/*********************************************************************
 * Compare a received HCI LE Read Remote Used Features Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciLeReadRemoteUsedFeaturesCS(ICall_ServiceEnum src,
                                               ICall_EntityID dest,
                                               const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_LE_READ_REMOTE_USED_FEATURES);
}

/*********************************************************************
 * Compare a received HCI LE Read Channel Map Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciLeReadChannelMapCS(ICall_ServiceEnum src,
                                       ICall_EntityID dest,
                                       const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_LE_READ_CHANNEL_MAP);
}

/*********************************************************************
 * Compare a received HCI LE Encrypt Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciLeEncryptCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_LE_ENCRYPT);
}

/*********************************************************************
 * Compare a received HCI LE Read Supported States Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciLeReadSupportedStatesCS(ICall_ServiceEnum src,
                                            ICall_EntityID dest,
                                            const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_LE_READ_SUPPORTED_STATES);
}

/*********************************************************************
 * Compare a received HCI LE Start the Transmit Direct Test Mode test Command
 * Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciLeTxTestCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_LE_TRANSMITTER_TEST);
}

/*********************************************************************
 * Compare a received HCI LE Start the Receiver Direct Test Mode test Command
 * Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciLeRxTestCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_LE_RECEIVER_TEST);
}

/*********************************************************************
 * Compare a received HCI LE End the Direct Test Mode test Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciLeTestEndCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_LE_TEST_END);
}

/*********************************************************************
 * HCI Vendor Specific Commands: Link Layer Extensions
 */

/*******************************************************************************
 * This API is used to set this device's TX Power
 *
 * Note: This command is only allowed when the device's state is Standby.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetTxPowerCmd(uint8 txPower)
{
  return hciSendParamsCmd(HCI_EXT_SET_TX_POWER, txPower,
                          HCI_NO_PARAM, HCI_NO_PARAM,
                          matchHciExtSetTxPowerCS);
}

/*******************************************************************************
 * This HCI Extension API is used to set whether a connection will be limited
 * to one packet per event.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_OnePktPerEvtCmd(uint8 control)
{
  return hciSendParamsCmd(HCI_EXT_ONE_PKT_PER_EVT, control,
                          HCI_NO_PARAM, HCI_NO_PARAM,
                          matchHciExtOnePktPerEvtCS);
}

/*******************************************************************************
 * This HCI Extension API is used to decrypt encrypted data using AES128.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_DecryptCmd(uint8 *key, uint8 *encText)
{
  return hciSendPtrParamsCmd(HCI_EXT_DECRYPT, key,
                             encText, NULL, matchHciExtDecryptCS);
}

/*******************************************************************************
 * This HCI Extension API is used to write this devie's supported features.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetLocalSupportedFeaturesCmd(uint8 *localFeatures)
{
  return hciSendPtrParamsCmd(HCI_EXT_SET_LOCAL_SUPPORTED_FEATURES,
                             localFeatures, NULL, NULL, matchHciExtSetLocalFeatsCS);
}

/*******************************************************************************
 * This HCI Extension API is used to set whether transmit data is sent as soon
 * as possible even when slave latency is used.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetFastTxResponseTimeCmd(uint8 control)
{
  return hciSendParamsCmd(HCI_EXT_SET_FAST_TX_RESP_TIME, control,
                          HCI_NO_PARAM, HCI_NO_PARAM,
                          matchHciExtSetFastTxRspTimeCS);
}

/*******************************************************************************
 * This HCI Extension API is used to enable or disable suspending slave
 * latency.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetSlaveLatencyOverrideCmd(uint8 control)
{
  return hciSendParamsCmd(HCI_EXT_OVERRIDE_SL, control,
                          HCI_NO_PARAM, HCI_NO_PARAM, matchHciExtSetSLOverrideCS);
}

/*******************************************************************************
 * This API is used start a continuous transmitter modem test, using either
 * a modulated or unmodulated carrier wave tone, at the frequency that
 * corresponds to the specified RF channel. Use HCI_EXT_EndModemTestCmd command
 * to end the test.
 *
 * Note: A Controller reset will be issued by the HCI_EXT_EndModemTestCmd command!
 * Note: The BLE device will transmit at maximum power.
 * Note: This API can be used to verify this device meets Japan's TELEC
 *       regulations.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_ModemTestTxCmd(uint8 cwMode, uint8 txFreq)
{
  return hciSendParamsCmd(HCI_EXT_MODEM_TEST_TX, cwMode,
                          txFreq, HCI_NO_PARAM, matchHciExtModemTestTxCS);
}

/*******************************************************************************
 * This API is used to start a continuous transmitter direct test mode test
 * using a modulated carrier wave and transmitting a 37 byte packet of
 * Pseudo-Random 9-bit data. A packet is transmitted on a different frequency
 * (linearly stepping through all RF channels 0..39) every 625us. Use
 * HCI_EXT_EndModemTestCmd to end the test.
 *
 * Note: A Controller reset will be issued by the HCI_EXT_EndModemTestCmd command!
 * Note: The BLE device will transmit at maximum power.
 * Note: This API can be used to verify this device meets Japan's TELEC
 *       regulations.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_ModemHopTestTxCmd(void)
{
  return hciSendCmd(HCI_EXT_MODEM_HOP_TEST_TX, matchHciExtModemHopTestTxCS);
}

/*******************************************************************************
 * This API is used to start a continuous receiver modem test using a modulated
 * carrier wave tone, at the frequency that corresponds to the specific RF
 * channel. Any received data is discarded. Receiver gain may be adjusted using
 * the HCI_EXT_SetRxGainCmd command. RSSI may be read during this test by using the
 * HCI_ReadRssi command. Use the HCI_EXT_EndModemTestCmd command to end the test.
 *
 * Note: A Controller reset will be issued by LL_EXT_EndModemTest!
 * Note: The BLE device will transmit at maximum power.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_ModemTestRxCmd(uint8 rxFreq)
{
  return hciSendParamsCmd(HCI_EXT_MODEM_TEST_RX, rxFreq,
                          HCI_NO_PARAM, HCI_NO_PARAM,
                          matchHciExtModemTestRxCS);
}

/*******************************************************************************
 * This API is used to shutdown a modem test. A complete Controller reset will
 * take place.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_EndModemTestCmd(void)
{
  return hciSendCmd(HCI_EXT_END_MODEM_TEST, matchHciExtEndModemTestCS);
}

/*******************************************************************************
 * This API is used to set this device's BLE address (BDADDR).
 *
 * Note: This command is only allowed when the device's state is Standby.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetBDADDRCmd(uint8 *bdAddr)
{
  return hciSendPtrParamsCmd(HCI_EXT_SET_BDADDR, bdAddr,
                             NULL, NULL, matchHciExtSetBdAddrCS);
}

/*******************************************************************************
 * This API is used to set this device's Sleep Clock Accuracy value.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetSCACmd(uint16 scaInPPM)
{
  return hciSendParamsCmd(HCI_EXT_SET_SCA, scaInPPM, HCI_NO_PARAM,
                          HCI_NO_PARAM, matchHciExtSetScaCS);
}

/*******************************************************************************
 * This HCI Extension API is used to enable Production Test Mode.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_EnablePTMCmd(void)
{
  return hciSendCmd(HCI_EXT_ENABLE_PTM, matchHciExtEnablePTMCS);
}

/*******************************************************************************
 * This HCI Extension API is used to set the max TX power for Direct Test Mode.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetMaxDtmTxPowerCmd(uint8 txPower)
{
  return hciSendParamsCmd(HCI_EXT_SET_MAX_DTM_TX_POWER, txPower,
                          HCI_NO_PARAM, HCI_NO_PARAM,
                          matchHciExtSetMaxDtmTxPwrCS);
}

/*******************************************************************************
 * This HCI Extension API is used to terminate a connection immediately without
 * following normal BLE disconnect control procedure.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_DisconnectImmedCmd(uint16 connHandle)
{
  return hciSendParamsCmd(HCI_EXT_DISCONNECT_IMMED, connHandle,
                          HCI_NO_PARAM, HCI_NO_PARAM,
                          matchHciExtDisconnectImmedCS);
}

/*******************************************************************************
 * This HCI Extension API is used to Reset or Read the Packet Error Rate data
 * for a connection.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_PacketErrorRateCmd(uint16 connHandle, uint8 command)
{
  return hciSendParamsCmd(HCI_EXT_PER, connHandle,
                          command, HCI_NO_PARAM, matchHciExtPacketErrorRateCS);
}

/*******************************************************************************
 * This HCI Extension API is used to start or end Packet Error Rate by Frequency
 * counter accumulation for a connection.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_PERbyChanCmd(uint16 connHandle, perByChan_t *perByChan)
{
  return hciSendParamAndPtrCmd(HCI_EXT_PER_BY_CHAN, connHandle,
                               (uint8_t *)perByChan, matchHciExtPERByChanCS);
}

/*******************************************************************************
 * This HCI Extension API is used to set a user revision number or read
 * the build revision number (combined user/system build number).
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_BuildRevisionCmd(uint8 mode, uint16 userRevNum)
{
  return hciSendParamsCmd(HCI_EXT_BUILD_REVISION, mode,
                          userRevNum, HCI_NO_PARAM, matchHciExtBuildRevCS);
}

/*******************************************************************************
 * This HCI Extension API is used to set the sleep delay.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_DelaySleepCmd(uint16 delay)
{
  return hciSendParamsCmd(HCI_EXT_DELAY_SLEEP, delay,
                          HCI_NO_PARAM, HCI_NO_PARAM, matchHciExtDelaySleepCS);
}

/*******************************************************************************
 * This HCI Extension API is used to issue a soft or hard system reset.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_ResetSystemCmd(uint8 mode)
{
  return hciSendParamsCmd(HCI_EXT_RESET_SYSTEM, mode,
                          HCI_NO_PARAM, HCI_NO_PARAM, matchHciExtResetSystemCS);
}

/*******************************************************************************
 * This HCI Extension API is used to set the minimum number of completed packets
 * which must be met before a Number of Completed Packets event is returned. If
 * the limit is not reach by the end of the connection event, then a Number of
 * Completed Packets event will be returned (if non-zero) based on the
 * flushOnEvt flag.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_NumComplPktsLimitCmd(uint8 limit, uint8 flushOnEvt)
{
  return hciSendParamsCmd(HCI_EXT_NUM_COMPLETED_PKTS_LIMIT, limit,
                          flushOnEvt, HCI_NO_PARAM,
                          matchHciExtNumComplPktsLimitCS);
}

/*******************************************************************************
 * This HCI Extension API is used to get the number of connections, and the
 * number of active connections.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_GetConnInfoCmd( uint8         *numAllocConns,
                                    uint8         *numActiveConns,
                                    hciConnInfo_t *activeConnInfo )
{
  return hciSendPtrParamsCmd( HCI_EXT_GET_CONNECTION_INFO,
                               numAllocConns,
                               numActiveConns,
                               (uint8_t *)activeConnInfo,
                               matchHciExtGetConnInfoCS );
}
/*********************************************************************
 * Compare a received HCI Ext Set Tx Power Command Status message for
 * a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtSetTxPowerCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_SET_TX_POWER);
}

/*********************************************************************
 * Compare a received HCI Ext One Packet Per Event Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtOnePktPerEvtCS(ICall_ServiceEnum src,
                                      ICall_EntityID dest, const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_ONE_PKT_PER_EVT);
}

/*********************************************************************
 * Compare a received HCI Ext Decrypt Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtDecryptCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                 const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_DECRYPT);
}

/*********************************************************************
 * Compare a received HCI Ext Set Local Supported Features Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtSetLocalFeatsCS(ICall_ServiceEnum src,
                                       ICall_EntityID dest,
                                       const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_SET_LOCAL_SUPPORTED_FEATURES);
}

/*********************************************************************
 * Compare a received HCI Ext Set Fast Tx Response Time Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtSetFastTxRspTimeCS(ICall_ServiceEnum src,
                                          ICall_EntityID dest,
                                          const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_SET_FAST_TX_RESP_TIME);
}

/*********************************************************************
 * Compare a received HCI Ext Set Slave Latency Override Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtSetSLOverrideCS(ICall_ServiceEnum src,
                                       ICall_EntityID dest,
                                       const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_OVERRIDE_SL);
}

/*********************************************************************
 * Compare a received HCI Ext Start a Continuous Transmitter Modem Test Command
 * Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtModemTestTxCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                     const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_MODEM_TEST_TX);
}

/*********************************************************************
 * Compare a received HCI Ext Start a Continuous Transmitter Modem Test Command
 * Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtModemHopTestTxCS(ICall_ServiceEnum src,
                                        ICall_EntityID dest,
                                        const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_MODEM_HOP_TEST_TX);
}

/*********************************************************************
 * Compare a received HCI Ext Start a Continuous Receiver Modem Test Command
 * Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtModemTestRxCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                     const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_MODEM_TEST_RX);
}

/*********************************************************************
 * Compare a received HCI Ext Shutdown a Modem Test Command Status message
 * for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtEndModemTestCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                      const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_END_MODEM_TEST);
}

/*********************************************************************
 * Compare a received HCI Ext Set BD Address Command Status message for
 * a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtSetBdAddrCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_SET_BDADDR);
}

/*********************************************************************
 * Compare a received HCI Ext Set SCA Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtSetScaCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_SET_SCA);
}

/*********************************************************************
 * Compare a received HCI Ext Enable Production Test Mode Command Status
 * message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtEnablePTMCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                   const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_ENABLE_PTM);
}

/*********************************************************************
 * Compare a received HCI Ext Set the Max TX Power for Direct Test Mode test
 * Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtSetMaxDtmTxPwrCS(ICall_ServiceEnum src,
                                        ICall_EntityID dest, const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_SET_MAX_DTM_TX_POWER);
}

/*********************************************************************
 * Compare a received HCI Ext Set the Max TX Power for Direct Test Mode test
 * Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtDisconnectImmedCS(ICall_ServiceEnum src,
                                         ICall_EntityID dest, const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_DISCONNECT_IMMED);
}

/*********************************************************************
 * Compare a received HCI Ext Set the Max TX Power for Direct Test Mode test
 * Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtPacketErrorRateCS(ICall_ServiceEnum src,
                                         ICall_EntityID dest, const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_PER);
}

/*********************************************************************
 * Compare a received HCI Ext Set the Max TX Power for Direct Test Mode test
 * Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtPERByChanCS(ICall_ServiceEnum src,
                                   ICall_EntityID dest, const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_PER_BY_CHAN);
}

/*********************************************************************
 * Compare a received HCI Ext Set or Read a User Revision Number Command
 * Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtBuildRevCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                  const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_BUILD_REVISION);
}

/*********************************************************************
 * Compare a received HCI Ext Set or Read a User Revision Number Command
 * Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtDelaySleepCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                    const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_DELAY_SLEEP);
}

/*********************************************************************
 * Compare a received HCI Ext Set or Read a User Revision Number Command
 * Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtResetSystemCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                     const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_RESET_SYSTEM);
}

/*********************************************************************
 * Compare a received HCI Ext Set or Read a User Revision Number Command
 * Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtNumComplPktsLimitCS(ICall_ServiceEnum src,
                                           ICall_EntityID dest,
                                           const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_NUM_COMPLETED_PKTS_LIMIT);
}

/*********************************************************************
 * Compare a received HCI Ext Set or Read a User Revision Number Command
 * Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchHciExtGetConnInfoCS(ICall_ServiceEnum src,
                                     ICall_EntityID dest,
                                     const void *msg)
{
  return matchHciCS(src, dest, msg, HCI_EXT_GET_CONNECTION_INFO);
}

/*********************************************************************
 * Simple NV API FUNCTIONS
 */

/*********************************************************************
 * Read data from NV.
 *
 * Public function defined in osal_snv.h.
 */
uint8 osal_snv_read(osalSnvId_t id, osalSnvLen_t len, void *pBuf)
{
  ICall_UtilNvRead *msg =
    (ICall_UtilNvRead *)ICall_allocMsg(sizeof(ICall_UtilNvRead));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_UTIL_SUBGRP, HCI_EXT_UTIL_NV_READ);

    // Set BD Address
    msg->id = id;
    msg->len = len;
    msg->pBuf = pBuf;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchUtilNvReadCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * Write a data item to NV.
 *
 * Public function defined in osal_snv.h.
 */
uint8 osal_snv_write(osalSnvId_t id, osalSnvLen_t len, void *pBuf)
{
  ICall_UtilNvWrite *msg =
    (ICall_UtilNvWrite *)ICall_allocMsg(sizeof(ICall_UtilNvWrite));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_UTIL_SUBGRP, HCI_EXT_UTIL_NV_WRITE);

    // Set write info
    msg->id = id;
    msg->len = len;
    msg->pBuf = pBuf;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchUtilNvWriteCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*********************************************************************
 * Compare a received Util NV Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 * @param cmdId command id
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchUtilNvCS(ICall_ServiceEnum src, ICall_EntityID dest,
                          const void *msg, uint8 cmdId)
{
  ICall_GapCmdStatus *pMsg = (ICall_GapCmdStatus *)msg;

  if ((pMsg->hdr.hdr.event == ICALL_EVENT_EVENT)              &&
      (pMsg->hdr.eventOpcode == HCI_EXT_GAP_CMD_STATUS_EVENT) &&
      ((pMsg->opCode >> 10) == VENDOR_SPECIFIC_OGF)           &&
      (((pMsg->opCode >> 7) & 0x07) == HCI_EXT_UTIL_SUBGRP)   &&
      ((pMsg->opCode & 0x007F) == cmdId))
  {
    return true;
  }

  return false;
}

/*********************************************************************
 * Compare a received Util NV Read Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchUtilNvReadCS(ICall_ServiceEnum src, ICall_EntityID dest,
                              const void *msg)
{
  return matchUtilNvCS(src, dest, msg, HCI_EXT_UTIL_NV_READ);
}

/*********************************************************************
 * Compare a received Util NV Write Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchUtilNvWriteCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg)
{
  return matchUtilNvCS(src, dest, msg, HCI_EXT_UTIL_NV_WRITE);
}

/*********************************************************************
 * UTILITY API FUNCTIONS
 */

/*********************************************************************
 * Read the Build Configuration used to build the BLE stack.
 *
 * Public function defined in ICallBleAPIMSG.h.
 */
bStatus_t Util_buildRevision(ICall_BuildRevision *pBuildRev)
{
  ICall_UtilBuildRev *msg =
    (ICall_UtilBuildRev *)ICall_allocMsg(sizeof(ICall_UtilBuildRev));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_UTIL_SUBGRP, HCI_EXT_UTIL_BUILD_REV);

    // Set read info
    msg->pBuildRev = pBuildRev;

    // Send the message
    return sendWaitMatchCS(ICall_getEntityId(), msg, matchUtilBuildRevCS);
  }

  return MSG_BUFFER_NOT_AVAIL;
}

/*******************************************************************************
 * @fn          Util_GetTRNG
 *
 * @brief       This routine returns a 32 bit TRNG number.
 *
 * @param       None.
 *
 * @return      A 32 bit TRNG number.
 */
uint32_t Util_GetTRNG(void)
{
  uint32_t trngVal = 0;

  /* Allocate message buffer space */
  ICall_UtilGetTRNG *msg =
    (ICall_UtilGetTRNG *)ICall_allocMsg(sizeof(ICall_UtilGetTRNG));

  if (msg)
  {
    setICallCmdEvtHdr(&msg->hdr, HCI_EXT_UTIL_SUBGRP, HCI_EXT_UTIL_GET_TRNG);

    /* Send the message. */
    sendWaitMatchValueCS(ICall_getEntityId(), msg, matchUtilGetTRNGCS,
                         sizeof(trngVal), (uint8_t *)&trngVal);
  }

  return trngVal;
}

/*********************************************************************
 * Compare a received Util Build Revision Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchUtilBuildRevCS(ICall_ServiceEnum src, ICall_EntityID dest,
                                const void *msg)
{
  return matchUtilNvCS(src, dest, msg, HCI_EXT_UTIL_BUILD_REV);
}

/*********************************************************************
 * Compare a received Util Get TRNG Command Status message for a match.
 *
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
static bool matchUtilGetTRNGCS(ICall_ServiceEnum src, ICall_EntityID dest,
                               const void *msg)
{
  return matchUtilNvCS(src, dest, msg, HCI_EXT_UTIL_GET_TRNG);
}


/*********************************************************************
*********************************************************************/
