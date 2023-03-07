/******************************************************************************

 @file  icall_ble_apimsg.h

 @brief ICall BLE stack messaging API

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
#ifndef ICALLBLEAPIMSG_H
#define ICALLBLEAPIMSG_H

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------
 * INCLUDES
 */

#include "bcomdef.h"
#include "hci_ext.h"
#include "hci.h"
#include "hci_tl.h"
#include "gapgattserver.h"
#include "linkdb.h"
#include "linkdb_internal.h"
#include "gapbondmgr.h"
#include "l2cap.h"
#include "gatt_uuid.h"
#include "gap.h"
#include "gap_advertiser.h"
#include "gap_scanner.h"
#include "gap_initiator.h"
#include "sm.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gatt_profile_uuid.h"
#include "gattservapp.h"
#include "osal_snv.h"
#include "ll_common.h"
#include "ll_ae.h"
#include "rtls_srv_api.h"

#if defined (GATT_TEST) || defined (GATT_QUAL)
  #include "gatttest.h"
#endif /* defined (GATT_TEST) || defined (GATT_QUAL) */

#if defined(HOST_CONFIG) && defined(STACK_LIBRARY)
#include "osal_bufmgr.h"
#endif  /* defined(HOST_CONFIG) && !defined(STACK_LIBRARY)*/

#ifdef HOST_CONFIG 
#include <../rom/rom_jt.h>
#endif /* HOST_CONFIG */

/*-------------------------------------------------------------------
 * TYPEDEFS
 */

/**
 * Event message header.
 * This is how it is defined in legacy BLE HCI_EXT_CMD_EVENT interface. It's
 * replicated here to provide a sample without actual BLE code.
 */
typedef struct _ICall_Hdr_
{
  uint8_t event;  //!< event
  uint8_t status; //!< status
} ICall_Hdr;

/**
 * Event message.
 * This is the structure used by BLE stack to set event flags for a task.
 */
typedef struct _ICall_Stack_Event_
{
  uint16_t signature;  //!< signature (0xffff)
  uint16_t event_flag; //!< event bit(s)
} ICall_Stack_Event;

/**
 * BLE HCI_EXT_CMD_EVENT event message common header.
 * This is how it is defined in legacy BLE HCI_EXT_CMD_EVENT interface. It's
 * replicated here to provide a sample without actual BLE code.
 */
typedef struct _ICall_HciExtCmd_
{
  ICall_Hdr hdr;     //!< header event field must be set as ICALL_CMD_EVENT
  uint8_t srctaskid; //!< Source task ID internally used by stack
  uint8_t pktType;   //!< Currently not used
  uint16_t opCode;   //!< Opcode - subgroup id and command id/profile id
  uint8_t cmdId;     //!< command id (applicable only to User Profile subgroup)
} ICall_HciExtCmd;

/**
 * BLE GAP DEVICE INIT message header and body.
 * This is an example of how legacy C function API is translated into a
 * messaging interface.
 * @see GAP_DeviceInit()
 */
typedef struct _ICall_GapDeviceInit_
{
  /**
   * Message header.
   * Note that the event field must be set to
   * @ref ICALL_CMD_EVENT in order to identify the message as distinct from
   * HCI_EXT_CMD_EVENT.
   */
  ICall_HciExtCmd hdr;        //!< hdr event field must be set as ICALL_CMD_EVENT
  uint8_t profileRole;        //!< profileRole
  uint8_t taskID;             //!< task ID
  GAP_Addr_Modes_t addrMode ; //!< own address mode
  uint8_t *pRandomAddr;       //!< random static address
} ICall_GapDeviceInit ;

/**
 * ICall message containing parameter type, length and value
 */
typedef struct _ICall_paramIdLenVal_
{
  uint16_t paramId;         //!< param Id
  uint16_t len;             //!< param length
  void *pValue;             //!< param value
}ICall_paramIdLenVal;

/**
 * ICall message  containing parameter type and value
 */
typedef struct _ICall_paramIdVal_
{
  uint16_t paramId;        //!< param Id
  void *pValue;            //!< param value
}ICall_paramIdVal;

/**
 * ICall message containing parameter ID and value for GAP Set Param
 * @see GAP_SetParamValue()
 */
typedef struct _ICall_GapSetParam_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t paramID;    //!< paramID
  uint16_t paramValue; //!< paramValue
} ICall_GapSetParam;

/**
 * ICall message containing parameter ID for GAP Get Param
 * @see GAP_GetParamValue()
 */
typedef struct _ICall_GapGetParam_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t paramID;    //!< paramID
} ICall_GapGetParam;

/**
 * ICall message containing HciExtCmd hdr and services to be added to profile
 *
 * @see profileAddService
 */
typedef struct _ICall_ProfileAddService_
{
  ICall_HciExtCmd hdr; //!< header event field must be set as ICALL_CMD_EVENT
  uint32_t services;   //!< services
} ICall_ProfileAddService;

/**
 * ICall message containing HciExtCmd hdr and parameter - type,length and value
 * to set profile parameter - used for GGS, simpleProfile etc.
 */
typedef struct _ICall_ProfileSetParam_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  ICall_paramIdLenVal paramIdLenVal;//!< param ID, length and value
} ICall_ProfileSetParam;

/**
 * ICall message containing HciExtCmd hdr and parameter - type and value
 * to get profile parameter - used for GGS, simpleProfile etc.
 */
typedef struct _ICall_ProfileGetParam_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  ICall_paramIdVal paramIdVal; //!< param ID and value
} ICall_ProfileGetParam;

/**
 * ICall message containing connection handle and state for Link DB State
 * @see linkDB_State()
 */
typedef struct _ICall_LinkDBState_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t connHandle; //!< connection handle
  uint8_t state;       //!< state
} ICall_LinkDBState;

/**
 * ICall message containing header for Link DB Number of Connections
 * @see linkDB_NumConns()
 */
typedef struct _ICall_LinkDBNumConns_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
} ICall_LinkDBNumConns;

/**
 * ICall message containing header for Link DB Number of Connections
 * @see linkDB_NumActive()
 */
typedef struct _ICall_LinkDBNumActive_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
} ICall_LinkDBNumActive;

/**
 * ICall message containing header for Link DB Find
 * @see linkDB_Find()
 */
typedef struct _ICall_LinkDBGetInfo_
{
  ICall_HciExtCmd hdr;  //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t connHandle;  //!< connection handle
  linkDBInfo_t * pInfo; //!< Address of Link DB info item to be copied into
} ICall_LinkDBGetInfo;

/**
 * ICall message containing HciExtCmd hdr and pointer to callback for
 * Security Manager callback register
 *
 * @see SM_RegisterCb
 */
typedef struct _ICall_SMRegisterTask_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint8_t taskID;      //!< task ID
} ICall_SMRegisterTask;

/**
 * ICall message containing HciExtCmd hdr
 *
 * @see SM_GetEccKeys
 */
typedef struct _ICall_SMGetEccKeys_
{
  ICall_HciExtCmd hdr; //!< header event field must be set as ICALL_CMD_EVENT
} ICall_SMGetEccKeys;

/**
 * ICall message containing HciExtCmd hdr and pointer to arguements for
 * ECDH key.
 *
 * @see SM_GetDHKey
 */
typedef struct _ICall_SMGetDHKey_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint8 *sk;           //!< pointer to secret (private) key
  uint8 *pk1;          //!< pointer to first public key
  uint8 *pk2;          //!< pointer to second public key
} ICall_SMGetDHKey;

/**
 * ICall message containing HciExtCmd hdr and pointer to arguements for
 * Secure Connections f4 confirmation value generation (OOB only).
 *
 * @see SM_GetScConfirmOob
 */
typedef struct _ICall_SMGetScConfirmOob_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint8 *publicKey;    //!< pointer to public key
  uint8 *oob;          //!< pointer to random data (OOB data)
} ICall_SMGetScConfirmOob;

/**
 * ICall message containing HciExtCmd hdr and pointer to callback for
 * gap gatt server register
 *
 * @see GGS_RegisterAppCBs
 */
typedef struct _ICall_GGSRegisterAppCBs_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  ggsAppCBs_t *pCB;    //!< pointer to callback function
} ICall_GGSRegisterAppCBs;

/**
 * ICall message containing HciExtCmd hdr and pointer to callback for
 * bond manager register
 *
 * @see GAPBondMgr_Register
 */
typedef struct _ICall_BondMgrRegister_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  gapBondCBs_t *pCB;   //!< pointer to callback function
} ICall_BondMgrRegister;

/**
 * ICall message containing HciExtCmd hdr, connection handle and passcode
 *
 * @see GAPBondMgr_PasscodeRsp
 */
typedef struct _ICall_BondMgrPasscodeRsp_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t connHandle; //!< connection handle
  uint8_t status;      //!< status
  uint32_t passcode;   //!< passcode
} ICall_BondMgrPasscodeRsp;


/**
 * ICall message containing HciExtCmd hdr and connection handle
 *
 * @see GAPBondMgr_FindAddr
 */
typedef struct _ICall_BondMgrResolveAddr_
{
  ICall_HciExtCmd hdr;    //!< hdr event field must be set as ICALL_CMD_EVENT
  uint8_t addrType;       //!< device address type
  uint8_t *pDevAddr;      //!< device address
  uint8_t *pResolvedAddr; //!< pointer to buffer to put the resolved address
} ICall_BondMgrFindAddr;

/**
 * ICall message containing HciExtCmd hdr, connection handle and other security
 * parameters
 *
 * @see GAP_Bond
 */
typedef struct _ICall_GapBond_
{
  ICall_HciExtCmd hdr;       //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t connHandle;       //!< connection handle
  uint8_t authenticated;     //!< field for authentication
  uint8_t secureConnections; //!< field for Secure Connections.
  smSecurityInfo_t *pParams; //!< security parameters
  uint8_t startEncryption;   //!< start Encryption
} ICall_GapBond;

/**
 * ICall message containing HciExtCmd hdr and GAP parameters
 *
 */
typedef struct _ICall_GapParams_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t connHandle; //!< connection handle
  uint8_t  param;      //!< parameter
} ICall_GapParams;

/**
 * ICall message containing HciExtCmd hdr and pointer to GAP parameters
 *
 */
typedef struct _ICall_GapPtrParams_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint8_t *pParam1;    //!< pointer to first parameter
  uint8_t *pParam2;    //!< pointer to second parameter
} ICall_GapPtrParams;

/**
 * ICall message containing HciExtCmd hdr and GAP parameter/pointer to parameter
 *
 */
typedef struct _ICall_GapParamAndPtr_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint8_t taskID;      //!< task id
  uint8_t *pParam;     //!< pointer to parameter
} ICall_GapParamAndPtr;

/**
 * ICall message containing HciExtCmd hdr, connection handle and reason
 * parameters for GAP Terminate Link request
 *
 * @see GAP_TerminateLinkReq
 */
typedef struct _ICall_GapTerminateLink_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t connHandle; //!< connection handle
  uint8_t reason;      //!< reason
} ICall_GapTerminateLink;

/**
 * ICall message containing HciExtCmd hdr, task ID, advertising data type,
 * length and value
 *
 * @see GapAdv_create
 */
typedef struct _ICall_GapAdvCreateParams_
{
  ICall_HciExtCmd hdr;          //!< header event field must be set as ICALL_CMD_EVENT
  pfnGapCB_t *cb;            //!< callback function
  uint8 *pHandle;     //!< handle to be assigned by GapAdv module
} ICall_GapAdvCreateParams;

/**
 * ICall message containing HciExtCmd hdr, connection handle and other signing
 * parameters
 *
 * @see GAP_Signable
 */
typedef struct _ICall_GapSignable_
{
  ICall_HciExtCmd hdr;      //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t connHandle;      //!< connection handle
  uint8_t authenticated;    //!< authenticated
  smSigningInfo_t *pParams; //!< signing parameters
} ICall_GapSignable;

/**
 * ICall message containing HciExtCmd hdr, connection handle and passkey
 *
 * @see GAP_PasskeyUpdate
 */
typedef struct _ICall_GapPassKeyUpdateParam_
{
  ICall_HciExtCmd hdr; //!< header event field must be set as ICALL_CMD_EVENT
  uint16_t connHandle; //!< connection Handle
  uint8_t *pPasskey;   //!< passKey
} ICall_GapPassKeyUpdateParam;

/**
 * ICall message containing HciExtCmd hdr, connection handle and ATT message
 * request
 * @see gattRequest
 */
typedef struct _ICall_GattReq_
{
  ICall_HciExtCmd hdr; //!< header event field must be set as ICALL_CMD_EVENT
  uint16_t connHandle; //!< connection Handle
  attMsg_t *pReq;      //!< ATT message request
  uint8_t taskId;      //!< task Id
} ICall_GattReq;

/**
 * ICall message containing HciExtCmd hdr, connection handle
 *
 * @see GATT_DiscAllPrimaryServices
 */
typedef struct _ICall_GattDiscAllPrimaryServ_
{
  ICall_HciExtCmd hdr; //!< header event field must be set as ICALL_CMD_EVENT
  uint16_t connHandle; //!< connection Handle
  uint8_t taskId;      //!< task Id
} ICall_GattDiscAllPrimaryServ;

/**
 * ICall message containing HciExtCmd hdr, connection handle, UUID length and
 * value
 * @see GATT_DiscPrimaryServiceByUUID
 */
typedef struct _ICall_GattDiscPrimServByUUID_
{
  ICall_HciExtCmd hdr; //!< header event field must be set as ICALL_CMD_EVENT
  uint16_t connHandle; //!< connection Handle
  uint8_t len;         //!< length
  uint8_t *pValue;     //!< pValue
  uint8_t taskId;      //!< task Id
} ICall_GattDiscPrimServByUUID;

/**
 * ICall message containing HciExtCmd hdr, connection handle, start Handle and
 * end Handle
 * @see GATT_FindIncludedServices/ GATT_DiscAllChars
 */
typedef struct _ICall_GattDiscAllChars_
{
  ICall_HciExtCmd hdr; //!< header event field must be set as ICALL_CMD_EVENT
  uint16_t connHandle; //!< connection Handle
  uint16_t startHandle;//!< start Handle
  uint16_t endHandle;  //!< end Handle
  uint8_t taskId;      //!< task Id
} ICall_GattDiscAllChars;

/**
 * ICall message containing HciExtCmd hdr, connection handle and
 * gattPrepareWriteReq
 *
 * @see gattWriteLong
 */
typedef struct _ICall_GattWriteLong_
{
  ICall_HciExtCmd hdr;        //!< hdr field must be set as ICALL_CMD_EVENT
  uint16_t connHandle;        //!< connection Handle
  attPrepareWriteReq_t *pReq; //!< pReq
  uint8_t taskId;             //!< task Id
} ICall_GattWriteLong;

/**
 * ICall message containing HciExtCmd hdr, connection handle, flags and
 * attPrepareWriteRequests. The numReqs refers to number of requests
 *
 * @see GATT_ReliableWrites
 */
typedef struct _ICall_GattReliableWrite_
{
  ICall_HciExtCmd hdr;         //!< hdr field must be set as ICALL_CMD_EVENT
  uint16_t connHandle;         //!< connection Handle
  uint8_t flags;               //!< flags
  uint8_t numReqs;             //!< number of requests
  attPrepareWriteReq_t *pReqs; //!< att prepare write request
  uint8_t taskId;              //!< task Id
} ICall_GattReliableWrite;

/**
 * ICall message containing HciExtCmd hdr, connection handle, attMsg request
 * and field to indicate authenticated/not
 *
 * @see gattIndNoti
 */
typedef struct _ICall_GattInd_
{
  ICall_HciExtCmd hdr;   //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t connHandle;   //!< connection Handle
  attMsg_t *pIndNoti;    //!< pointer to notification/indication
  uint8_t authenticated; //!< authenticated
  uint8_t taskId;        //!< task Id
} ICall_GattInd;

/**
 * ICall message containing HciExtCmd hdr
 *
 * @see ICall_GattInitClient
 */
typedef struct _ICall_GattInitClient_
{
  ICall_HciExtCmd hdr; //!< header event field must be set as ICALL_CMD_EVENT
} ICall_GattInitClient;

typedef struct _ICall_GattGetNextHandle_
{
  ICall_HciExtCmd hdr; //!< header event field must be set as ICALL_CMD_EVENT
} ICall_GattGetNextHandle;

/**
 * ICall message containing HciExtCmd hdr
 *
 * @see GATT_RegisterForInd
 */
typedef struct _ICall_GattRegisterForInd_
{
  ICall_HciExtCmd hdr; //!< header event field must be set as ICALL_CMD_EVENT
  uint8_t taskId;      //!< taskId
} ICall_GattRegisterForInd;

/**
 * ICall message containing HciExtCmd hdr, heap size and flow control mode
 *
 * @see GATT_SetHostToAppFlowCtrl
 */
typedef struct _ICall_GattHtaFlowCtrl_
{
  ICall_HciExtCmd hdr;  //!< header event field must be set as ICALL_CMD_EVENT
  uint16_t heapSize;    //!< internal heap size
  uint8_t flowCtrlMode; //!< flow control mode: TRUE or FALSE
} ICall_GattHtaFlowCtrl;

/**
 * ICall message containing HciExtCmd hdr and processed GATT message
 *
 * @see GATT_AppCompletedMsg
 */
typedef struct _ICall_GattAppComplMsg_
{
  ICall_HciExtCmd hdr;  //!< header event field must be set as ICALL_CMD_EVENT
  gattMsgEvent_t *pMsg; //!< pointer to processed GATT message
} ICall_GattAppComplMsg;

/**
 * ICall message containing HciExtCmd hdr, connection handle, method and
 * ATT response
 *
 * @see GATT_SendRsp
 */
typedef struct _ICall_GattSendRsp_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t connHandle; //!< connection handle
  uint8_t method;      //!< type of response message
  gattMsg_t *pRsp;     //!< pointer to ATT response
} ICall_GattSendRsp;

/**
 * ICall message containing HciExtCmd hdr, connection handle and ATT message
 *
 * @see ATT_HandleValueCfm
 */
typedef struct _ICall_AttParamAndPtr_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t connHandle; //!< connection handle
  attMsg_t *pMsg;      //!< pointer to ATT message
} ICall_AttParamAndPtr;

/**
 * ICall message containing HciExtCmd hdr, connection handle and ATT message
 *
 * @see ATT_HandleValueCfm
 */
typedef struct _ICall_cccUpdateMsg_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t connHandle; //!< connection handle
  uint16_t attrHandle; //!< attribute handle
  uint16_t value;      //!< new attribute value
} ICall_cccUpdateMsg;

/**
 * ICall message containing HciExtCmd hdr, connection handle and
 * L2CAP Connection Parameter Update Request
 *
 * @see L2CAP_ConnParamUpdateReq
 */
typedef struct _ICall_L2capParamUpdateReq_
{
  ICall_HciExtCmd hdr;       //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t connHandle;       //!< connection Handle
  l2capParamUpdateReq_t *pUpdateReq; //!< update request
  uint8 taskId;              //!< task Id
} ICall_L2capParamUpdateReq;

/**
 * ICall message containing HciExtCmd hdr, CID, SDU and SUD length
 *
 * @see L2CAP_SendSDU
 */
typedef struct _ICall_L2capSendSDU_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  l2capPacket_t *pPkt; //< pointer to packet to be sent
} ICall_L2capSendSDU;

/**
 * ICall message containing HciExtCmd hdr and pointer to PSM structure
 *
 * @see L2CAP_RegisterPsm
 */
typedef struct _ICall_L2capRegisterPsm_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  l2capPsm_t *pPsm;    //!< pointer to PSM structure
} ICall_L2capRegisterPsm;

/**
 * ICall message containing HciExtCmd hdr, task id and local PSM
 *
 * @see L2CAP_DeregisterPsm
 */
typedef struct _ICall_L2capDeregisterPsm_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint8_t taskId;      //!< task PSM belongs to
  uint16_t psm;        //!< PSM to deregister
} ICall_L2capDeregisterPsm;

/**
 * ICall message containing HciExtCmd hdr, local PSM and pointer to structure
 * to copy PSM info into
 *
 * @see L2CAP_PsmInfo
 */
typedef struct _ICall_L2capPsmInfo_
{
  ICall_HciExtCmd hdr;   //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t psm;          //!< PSM Id
  l2capPsmInfo_t *pInfo; //!< structure to copy PSM info into
} ICall_L2capPsmInfo;

/**
 * ICall message containing HciExtCmd hdr, local PSM, number of channels and
 * pointer to structure to copy CIDs into
 *
 * @see L2CAP_PsmChannels
 */
typedef struct _ICall_L2capPsmChannels_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t psm;        //!< PSM Id
  uint8_t numCIDs;     //!< number of CIDs can be copied
  uint16_t *pCIDs;     //!< structure to copy CIDs into
} ICall_L2capPsmChannels;

/**
 * ICall message containing HciExtCmd hdr, CID and structure to copy channel
 * info into
 *
 * @see L2CAP_ChannelInfo
 */
typedef struct _ICall_L2capChannelInfo_
{
  ICall_HciExtCmd hdr;       //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t CID;              //!< local channel id
  l2capChannelInfo_t *pInfo; //!< structure to copy channel info into
} ICall_L2capChannelInfo;

/**
 * ICall message containing HciExtCmd hdr, connection handle, local PSM and
 * peer PSM
 *
 * @see L2CAP_ConnectReq
 */
typedef struct _ICall_L2capConnectReq_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t connHandle; //!< connection to create channel on
  uint16_t psm;        //!< local PSM
  uint16_t peerPsm;    //!< peer PSM
} ICall_L2capConnectReq;

/**
 * ICall message containing HciExtCmd hdr, connection handle, id and result
 *
 * @see L2CAP_ConnectRsp
 */
typedef struct _ICall_Ll2capConnectRsp_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t connHandle; //!< connection handle
  uint8_t id;          //!< identifier received in connection request
  uint16_t result;     //!< outcome of connection request
} ICall_Ll2capConnectRsp;

/**
 * ICall message containing HciExtCmd hdr and CID
 *
 * @see L2CAP_DisconnectReq
 */
typedef struct _ICall_L2capDisconnectReq_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t CID;        //!< local CID to disconnect
} ICall_L2capDisconnectReq;

/**
 * ICall message containing HciExtCmd hdr, CID and peer credits
 *
 * @see L2CAP_FlowCtrlCredit
 */
typedef struct _ICall_L2capFlowCtrlCredit_
{
  ICall_HciExtCmd hdr;  //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t CID;         //!< local CID
  uint16_t peerCredits; //!< number of credits to give to peer device
} ICall_L2capFlowCtrlCredit;

/**
 * ICall message containing HciExtCmd hdr, attribute table, number of
 * atributes in the table, and service CBs
 *
 * @see GATTServApp_RegisterService
 */
typedef struct _ICall_GSA_RegService_
{
  ICall_HciExtCmd hdr;     //!< hdr event field must be set as ICALL_CMD_EVENT
  gattAttribute_t *pAttrs; //!< Array of attribute records to be registered
  uint16_t numAttrs;       //!< Number of attributes in array
  uint8_t encKeySize;      //!< Minimum encryption key size required by service
  CONST gattServiceCBs_t *pServiceCBs; //!< Service callback function pointers
} ICall_GSA_RegService;

/**
 * ICall message containing HciExtCmd hdr, attribute table, number of
 * atributes in the table, and service CBs
 *
 * @see GATTServApp_DeregisterService
 */
typedef struct _ICall_GSA_DeregService_
{
  ICall_HciExtCmd hdr;       //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t handle;           //!< handle of service to be deregistered
  gattAttribute_t **p2pAttrs;//!< ptr to array of attribute records (returned)
} ICall_GSA_DeregService;

/**
 * ICall message containing HciExtCmd hdr, connection handle and task id
 *
 * @see GATTServApp_SendServiceChangedInd
 */
typedef struct _ICall_GSA_ServiceChangeInd_
{
  ICall_HciExtCmd hdr; //!< header event field must be set as ICALL_CMD_EVENT
  uint16_t connHandle; //!< connection Handle
  uint8_t taskId;      //!< task Id
} ICall_GSA_ServiceChangeInd;

/**
 * ICall message containing HciExtCmd hdr, connHandle, attribute value pointer,
 * attribute length, and attribute handle
 *
 * @see GATTServApp_GATTServApp_ReadRsp()
 */
typedef struct _ICall_GSA_ReadRsp_
{
  ICall_HciExtCmd hdr; //!< header event field must be set as ICALL_CMD_EVENT
  uint16_t connHandle; //!< Connection Handle
  uint8_t *pAttrValue; //!< Pointer to Attribute Value
  uint16_t attrLen;    //!< Length of Attribute Read
  uint16_t attrHandle;  //!< Attribute Handle
} ICall_GSA_ReadRsp;

/*
 * HCI BT/LE/Extension Commands: Link Layer
 */

/**
 * ICall message containing HciExtCmd hdr and parameters
 */
typedef struct _ICall_Hci_Params_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t param1;     //!< first parameter
  uint16_t param2;     //!< second parameter
  uint16_t param3;     //!< third parameter
} ICall_Hci_Params;

/**
 * ICall message containing HciExtCmd hdr and pointer to parameters
 */
typedef struct _ICall_Hci_PtrParams_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint8_t *pParam1;    //!< pointer to first parameter
  uint8_t *pParam2;    //!< pointer to second parameter
  uint8_t *pParam3;    //!< pointer to third parameter
} ICall_Hci_PtrParams;

/**
 * ICall message containing HciExtCmd hdr, parameter and pointer
 */
typedef struct _ICall_Hci_ParamAndPtr_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint16_t param;      //!< first parameter
  uint8_t *pParam;     //!< pointer to second parameter
} ICall_Hci_ParamAndPtr;

/**
 * ICall message containing HciExtCmd hdr and the transmit DTM info
 *
 * @see HCI_LE_TransmitterTestCmd
 */
typedef struct _ICall_HciLe_TxTest_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
  uint8_t txChan;      //!< Tx RF channel
  uint8_t dataLen;     //!< test data length in bytes
  uint8_t payloadType; //!< type of packet payload
} ICall_HciLe_TxTest;

/**
 * ICall message containing HciExtCmd hdr and Util NV Read parameters
 *
 * @see osal_snv_read
 */
typedef struct _ICall_UtilNvRead_
{
  ICall_HciExtCmd hdr; //!< header event field must be set as ICALL_CMD_EVENT
  osalSnvId_t id;      //!< valid NV item Id
  osalSnvLen_t len;    //!< length of data to read
  void *pBuf;          //!< data is read into this buffer
} ICall_UtilNvRead;

/**
 * ICall message containing HciExtCmd hdr and Util NV Write parameters
 *
 * @see osal_snv_write
 */
typedef struct _ICall_UtilNvWrite_
{
  ICall_HciExtCmd hdr; //!< header event field must be set as ICALL_CMD_EVENT
  osalSnvId_t id;      //!< valid NV item Id
  osalSnvLen_t len;    //!< length of data to write
  void *pBuf;          //!< data to write
} ICall_UtilNvWrite;

/**
 * ICall structure containing Build Revision parameters
 *
 * @see Util_buildRevision
 */
typedef struct _ICall_BuildRevision_
{
  uint32_t stackVersion; //!< stack revision
  uint16_t buildVersion; //!< build revision
  uint8_t  stackInfo;    //!< stack info
  uint16_t ctrlInfo;     //!< controller info
  uint16_t hostInfo;     //!< host info
} ICall_BuildRevision;

/**
 * ICall message containing HciExtCmd hdr and Util Build Revision parameter
 *
 * @see ICall_BuildRevision
 */
typedef struct _ICall_UtilBuildRev_
{
  ICall_HciExtCmd hdr; //!< header event field must be set as ICALL_CMD_EVENT
  ICall_BuildRevision *pBuildRev; //!< ptr to struct to copy build revision into
} ICall_UtilBuildRev;

/**
 * ICall message containing header for Get TRNG Number
 * @see Util_GetTRNG()
 */
typedef struct _ICall_UtilGetTRNG_
{
  ICall_HciExtCmd hdr; //!< hdr event field must be set as ICALL_CMD_EVENT
} ICall_UtilGetTRNG;

/**
 * ICall message containing HciExtCmd hdr and task ID
 *
 * @see GAP_RegisterForMsgs
 */
typedef struct _ICall_RegisterTaskMsg_
{
  ICall_HciExtCmd hdr; //!< header event field must be set as ICALL_CMD_EVENT
  uint8_t taskID;      //!< task Id
} ICall_RegisterTaskMsg;

/**
 * A union for application to be able to access a received BLE stack
 * command message through, in order not to violate strict aliasing rule.
 */
typedef union _ICall_CmdMsg_
{
  ICall_Hdr                  hdr;                //!< message header
  ICall_HciExtCmd            hciExtCmd;          //!< HCI EXT CMD event msg

  /*
   * HCI BT/LE/Extension Commands: Link Layer
   */
  ICall_Hci_Params           hciParams;          //!< HCI parameters
  ICall_Hci_PtrParams        hciPtrParams;       //!< HCI pointer parameters
  ICall_Hci_ParamAndPtr      hciParamAndPtr;     //!< HCI parameter and pointer
  ICall_HciLe_TxTest         hciLeTxTest;        //!< HCI LE Tx Test Cmd

  /*
   * Host Vendor Specific Commands: GAP/SM/GATT/L2CAP Extensions
   */
  ICall_GapParams            gapParams;          //!< GAP parameters
  ICall_GapPtrParams         gapPtrParams;       //!< GAP pointer parameters
  ICall_GapParamAndPtr       gapParamAndPtr;     //!< GAP parameter and pointer
  ICall_RegisterTaskMsg      registerTaskMsg;    //!< Register task message
  ICall_GapDeviceInit        gapDeviceInit;      //!< GAP Device Init message
  ICall_GapSetParam          gapSetParam;        //!< GAP Set Parameter
  ICall_GapGetParam          gapGetParam;        //!< GAP Get Parameter
  ICall_ProfileAddService    profileAddService;  //!< Profile Add Service
  ICall_ProfileSetParam      profileSetParam;    //!< Profile Set Parameter
  ICall_ProfileGetParam      profileGetParam;    //!< Profile Get Parameter
  ICall_GapAdvCreateParams   gapAdvCreateParams; //!< GAP Update Advertising
  ICall_GapBond              gapBondParams;      //!< GAP Bond
  ICall_GapTerminateLink     gapTerminateLink;   //!< GAP Terminate Link request
  ICall_GapSignable          gapEnableSignedData;//!< GAP Enabled signed data
  ICall_GattReq              gattReq;            //!< GATT Generic Request
  ICall_GattDiscAllChars     gattDiscAllChars;   //!< GATT Discover all Chars
  ICall_GattWriteLong        gattWriteLong;      //!< GATT Write Long Char Value
  ICall_GattReliableWrite    gattReliableWrite;  //!< GATT Reliable Write
  ICall_GattInd              gattInd;            //!< GATT Indication
  ICall_GattInitClient       gattInitClient;     //!< GATT Initialize Client
  ICall_GattRegisterForInd   gattRegisterForInd; //!< GATT Reg for Indication
  ICall_GattHtaFlowCtrl      gattHtaFlowCtrl;    //!< GATT Host To App Flow Ctrl
  ICall_GattAppComplMsg      gattAppComplMsg;    //!< GATT App Completed Message
  ICall_GattSendRsp          gattSendRsp;        //!< GATT Send Response
  ICall_AttParamAndPtr       attParamAndPtr;     //!< ATT parameter and pointer
  ICall_cccUpdateMsg         cccUpdateMsg;       //!< GATT CCC Update Event
  ICall_L2capParamUpdateReq  l2capParamUpdateReq;//!< L2CAP Param Update req
  ICall_L2capSendSDU         l2capSendSDU;       //!< L2CAP Send SDU
  ICall_L2capRegisterPsm     l2capRegisterPsm;   //!< L2CAP Register PSM
  ICall_L2capDeregisterPsm   l2capDeregisterPsm; //!< L2CAP Deregister PSM
  ICall_L2capPsmInfo         l2capPsmInfo;       //!< L2CAP PSM Info
  ICall_L2capPsmChannels     l2capPsmChannels;   //!< L2CAP PSM Channels
  ICall_L2capChannelInfo     l2capChannelInfo;   //!< L2CAP Channel Info
  ICall_L2capConnectReq      l2capConnectReq;    //!< L2CAP Connect Request
  ICall_Ll2capConnectRsp     l2capConnectRsp;    //!< L2CAP Connect Response
  ICall_L2capDisconnectReq   l2capDisconnectReq; //!< L2CAP Disconnect Request
  ICall_L2capFlowCtrlCredit  l2capFlowCtrlCredit;//!< L2CAP Flow Ctrl Credit
  ICall_GSA_RegService       gsaRegService;      //!< GSA Register Service
  ICall_GSA_DeregService     gsaDeregService;    //!< GSA Deregister Service
  ICall_GSA_ServiceChangeInd gsaServiceChangeInd;//!< GSA Service Changed Ind
  ICall_GSA_ReadRsp          gsaReadRsp;         //!< GSA Read Response
  ICall_GGSRegisterAppCBs    ggsRegister;        //!< GGS Register App Callbacks
  ICall_UtilNvRead           utilNvRead;         //!< Util NV Read
  ICall_UtilNvWrite          utilNvWrite;        //!< Util NV Write
  ICall_UtilBuildRev         utilBuildRev;       //!< Util Build Revision
  ICall_UtilGetTRNG          utilGetTRNG;        //!< Util Get TRNG Number
  ICall_BondMgrRegister      gapBondMgrRegister;   //!< GAP BondMgr Register
  ICall_BondMgrPasscodeRsp   gapBondMgrPasscodeRsp;//!< GAP BondMgr Passcode Rsp
  ICall_BondMgrFindAddr      gapbondMgrResolveAddr;//!< GAP BondMgr Resolve Addr
  ICall_LinkDBState          linkDBState;          //!< Link DB State
  ICall_LinkDBNumConns       linkDBNumConns;       //!< Link DB Num Conns
  ICall_LinkDBNumActive      linkDBNumActive;      //!< Link DB Num Active
  ICall_LinkDBGetInfo        linkDBGetInfo;        //!< Link DB Find
  ICall_GapPassKeyUpdateParam  gapPassKeyUpdateParams; //!< GAP Passkey Update
  ICall_GattDiscAllPrimaryServ gattDiscAllPrimaryServ; //!< GATT Discover
                                                       //!< Primary Service
  ICall_GattDiscPrimServByUUID gattDiscPrimServByUUID; //!< GATT Disc Primary
                                                       //!< Service by UUID
  ICall_SMRegisterTask         smRegisterTask;     //!< SM register Task for
                                                   //!< Secure Connections
  ICall_SMGetEccKeys           smGetEccKeys;       //!< SM get ECC keys
  ICall_SMGetDHKey             smGetDHKey;         //!< SM get DH Key
  ICall_SMGetScConfirmOob      smGetSCConfirmOob;  //!< SM Secure Connections
                                                   //!< Confirm value for OOB
} ICall_CmdMsg;

/** BLE ICALL_EVENT_EVENT event message common header */
typedef struct _ICall_HciExtEvt_
{
  ICall_Hdr hdr;        //!< message header, event field must be set to
                        //!< ICALL_EVENT_EVENT
  uint16_t eventOpcode; //!< event opcode
} ICall_HciExtEvt;

/** GAP Command status message */
typedef struct _ICall_GapCmdStatus_
{
  ICall_HciExtEvt hdr;  //!< message header, event field must be set to
  uint16_t opCode;      //!< opcode which comprises of subgroup id and
                        //!< command id (or profile id)
  uint8_t cmdId;        //!< command id (applicable only to User Profile subgrp)
  uint8_t len;          //!< length of value field
  uint8_t *pValue;      //!< pointer to value
} ICall_GapCmdStatus;

/** NPI UART buffer message */
typedef struct _ICall_NPIUartMsg_
{
  ICall_HciExtEvt hdr;  //!< message header, event field must be set to
  uint8_t len;          //!< length of value field
  char *pValue;         //!< pointer to value
} ICall_NPIUartMsg;

/**
 * A union for application to be able to access a received BLE stack event
 * message through, in order not to violate strict aliasing rule.
 */
typedef union _ICall_EvtMsg_
{
  ICall_Hdr          hdr;          //!< message header
  ICall_HciExtEvt    hciExtEvt;    //!< HCI EXT CMD event message header
  ICall_GapCmdStatus gapCmdStatus; //!< GAP Command Status event
  ICall_NPIUartMsg   npiUartRxBuf; //!< NPI UART message to app
} ICall_EvtMsg;

/** HCI Generic Command Complete event */
typedef struct ICall_HciCmdCompleteEvtMsg
{
  ICall_Hdr  hdr;                   //!< message header (OSAL event header)
  uint8     *pData;                 //!< pointer to event packet
  uint8      pktType;               //?< HCI packet type
  uint8      evtCode;               //?< HCI event code (Vendor Specific or not)
} ICall_HciCmdCompleteEvtMsg;

/** HCI Read BDADDR Command Complete event */
typedef struct ICall_HciReadBdaddrEvtMsg
{
  uint8  pktType;
  uint8  evtCode;
  uint8  len;
  uint8  numCompletedCmds;
  uint16 opcode;
  uint8  status;
  uint8  bdAddr[B_ADDR_LEN];
} ICall_HciReadBdaddrEvtMsg;

/** HCI Set BDADDR Vendor Specific Complete event */
PACKED_TYPEDEF_STRUCT ICall_HciSetBdaddrEvtMsg
{
  uint8  pktType;
  uint8  evtCode;
  uint8  len;
  uint16 eventOpcode;
  uint8  status;
  uint16 opcode;
} ICall_HciSetBdaddrEvtMsg;

/*********************************************************************
 * FUNCTION APIs
 */

/*********************************************************************
 * @fn      Util_buildRevision
 *
 * @brief   Read the Build Revision used to build the BLE stack.
 *
 * @param   pBuildRev - pointer to variable to copy build revision into
 *
 * @return  SUCCESS: Operation was successfully.
 *          INVALIDPARAMETER: Invalid parameter.
 */
extern bStatus_t Util_buildRevision(ICall_BuildRevision *pBuildRev);

/*********************************************************************
 * @fn          Util_GetTRNG
 *
 * @brief       This routine returns a 32 bit TRNG number.
 *
 * @param       None.
 *
 * @return      A 32 bit TRNG number.
 */
extern uint32_t Util_GetTRNG(void);

/*********************************************************************
 * @fn      BM_free
 *
 * @brief   Implementation of the de-allocator functionality.
 *
 * @param   payload_ptr - pointer to the memory to free.
 *
 * @return  none
 */
extern void BM_free(void *payload_ptr);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ICALLBLEAPIMSG_H */
