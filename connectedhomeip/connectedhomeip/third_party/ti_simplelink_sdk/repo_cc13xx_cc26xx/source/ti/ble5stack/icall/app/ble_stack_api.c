/******************************************************************************

@file  ble_stack_api.c

@brief This file contains the ble stack wrapper abovr icall

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

/*********************************************************************
 * INCLUDES
 */
#include "ble_stack_api.h"
#include <icall_ble_api.h>
#ifndef NO_TI_BLE_CONFIG
#include "ti_ble_config.h"
#endif
#include <gapbondmgr.h>

/*********************************************************************
 * FUNCTIONS
 */
bStatus_t bleStk_GapAdv_loadLocalByHandle(uint8_t advHandle,GapAdv_dataTypes_t advtype,
                                    uint16_t len, uint8_t *pData);

#ifdef ERPC_SERVER
#include "ble_app_services.h"
static pfnBleStkAdvCB_t  remote_bleApp_GapAdvCb = NULL;
static void local_bleApp_GapAdvCb(uint32 event, GapAdv_data_t *pBuf, uint32_t *arg);
#endif

#ifdef ICALL_NO_APP_EVENTS
/*********************************************************************
 * @fn      bleStack_register
 *
 * @brief   register callback in the ICALL for application events
 *
 * @param   selfEntity - return self entity id from the ICALL
 * @param   appCallback- The callback
 *
 * @return  SUCCESS ot FAILURE.
 */
bleStack_errno_t bleStack_register(uint8_t *selfEntity, appCallback_t appCallback)
{
  ICall_EntityID   localSelfEntity;
  ICall_Errno status;
  // ******************************************************************
  // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ******************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
  status = ICall_registerAppCback(&localSelfEntity, appCallback);

  // applicatios should use the task entity ID
  *selfEntity = localSelfEntity;

  return status;
}
#endif // ICALL_NO_APP_EVENTS

/*********************************************************************
 * @fn      bleStack_createTasks
 *
 * @brief   creaete ICALL tasks
 *
 * @param   None
 *
 * @return  None
 */
void bleStack_createTasks()
{
    /* Initialize ICall module */
    ICall_init();

    /* Start tasks of external images - Priority 5 */
    ICall_createRemoteTasks();
}

/*********************************************************************
 * @fn      bleStack_initGap
 *
 * @brief   Init the GAP
 *
 * @param   role - the role of the application
 * @param   appSelfEntity - self entity id for the ICALL
 * @param   paramUpdateDecision- the param update configuration
 *
 * @return  SUCCESS ot FAILURE.
 */
bStatus_t bleStack_initGap(uint8_t role, ICall_EntityID appSelfEntity, uint16_t paramUpdateDecision)
{
  if (role & (GAP_PROFILE_PERIPHERAL | GAP_PROFILE_CENTRAL))
  {
      // Pass all parameter update requests to the app for it to decide
      GAP_SetParamValue(GAP_PARAM_LINK_UPDATE_DECISION, paramUpdateDecision);
  }

  // Register with GAP for HCI/Host messages. This is needed to receive HCI
  // events. For more information, see the HCI section in the User's Guide:
  // http://software-dl.ti.com/lprf/ble5stack-latest/
  GAP_RegisterForMsgs(appSelfEntity);

  return SUCCESS;
}

/*********************************************************************
 * @fn      bleStack_initGapBondParams
 *
 * @brief   Set all GAP bond manager parameters
 *
 * @param   pGapBondParams - the configuration structure
 *
 * @return  SUCCESS ot FAILURE.
 */
void bleStack_initGapBondParams(GapBond_params_t *pGapBondParams)
{
    // Set Pairing Mode
    GAPBondMgr_SetParameter(GAPBOND_PAIRING_MODE, sizeof(uint8_t), &pGapBondParams->pairMode);
    // Set MITM Protection
    GAPBondMgr_SetParameter(GAPBOND_MITM_PROTECTION, sizeof(uint8_t), &pGapBondParams->mitm);
    // Set IO Capabilities
    GAPBondMgr_SetParameter(GAPBOND_IO_CAPABILITIES, sizeof(uint8_t), &pGapBondParams->ioCap);
    // Set Bonding
    GAPBondMgr_SetParameter(GAPBOND_BONDING_ENABLED, sizeof(uint8_t), &pGapBondParams->bonding);
    // Set Secure Connection Usage during Pairing
    GAPBondMgr_SetParameter(GAPBOND_SECURE_CONNECTION, sizeof(uint8_t), &pGapBondParams->secureConnection);
    // Set Authenticated Pairing Only mode
    GAPBondMgr_SetParameter(GAPBOND_AUTHEN_PAIRING_ONLY, sizeof(uint8_t), &pGapBondParams->authenPairingOnly);
    // Set Auto Whitelist Sync
    GAPBondMgr_SetParameter(GAPBOND_AUTO_SYNC_WL, sizeof(uint8_t), &pGapBondParams->autoSyncWL);
    // Set ECC Key Regeneration Policy
    GAPBondMgr_SetParameter(GAPBOND_ECCKEY_REGEN_POLICY, sizeof(uint8_t), &pGapBondParams->eccReGenPolicy);
    // Set Key Size used in pairing
    GAPBondMgr_SetParameter(GAPBOND_KEYSIZE, sizeof(uint8_t), &pGapBondParams->KeySize);
    // Set LRU Bond Replacement Scheme
    GAPBondMgr_SetParameter(GAPBOND_LRU_BOND_REPLACEMENT, sizeof(uint8_t), &pGapBondParams->removeLRUBond);
    // Set Key Distribution list for pairing
    GAPBondMgr_SetParameter(GAPBOND_KEY_DIST_LIST, sizeof(uint8_t), &pGapBondParams->KeyDistList);
    // Set Secure Connection Debug Keys
    GAPBondMgr_SetParameter(GAPBOND_SC_HOST_DEBUG, sizeof(uint8_t), &pGapBondParams->eccDebugKeys);
    // Set the Erase bond While in Active Connection Flag
    GAPBondMgr_SetParameter(GAPBOND_ERASE_BOND_IN_CONN, sizeof(uint8_t), &pGapBondParams->eraseBondWhileInConn);
}

/*********************************************************************
 * @fn      bleStack_initGapBond
 *
 * @brief   Init GAP bond manager
 *
 * @param   pGapBondParams - the configuration structure
 * @param   bleApp_bondMgrCBs - the pairing and passcode callbacks
 *
 * @return  SUCCESS ot FAILURE.
 */
bStatus_t bleStack_initGapBond(GapBond_params_t *pGapBondParams, void *bleApp_bondMgrCBs)
{
#if defined ( GAP_BOND_MGR )
    if (pGapBondParams == NULL)
    {
#ifndef NO_TI_BLE_CONFIG
        // Setup the GAP Bond Manager. For more information see the GAP Bond Manager
        // section in the User's Guide
        // Todo: - remove setBondManagerParameters implementation from SysConfig
        // Todo: - set one call _all instaead of many calls to GAPBondMgr_SetParameter
        setBondManagerParameters();
#endif
    }
    else
    {
        bleStack_initGapBondParams(pGapBondParams);
    }

    if (bleApp_bondMgrCBs != NULL)
    {
        // Start Bond Manager and register callback
        VOID GAPBondMgr_Register((gapBondCBs_t *)bleApp_bondMgrCBs);
    }
#endif // GAP_BOND_MGR
    return SUCCESS;
}

/*********************************************************************
 * @fn      bleStack_initGatt
 *
 * @brief   Init the GATT
 *
 * @param   role - the role of the application
 * @param   appSelfEntity - self entity id for the ICALL
 * @param   pAttDeviceName- the device name
 *
 * @return  SUCCESS ot FAILURE.
 */
bStatus_t bleStack_initGatt(uint8_t role, ICall_EntityID appSelfEntity, uint8_t *pAttDeviceName)
{
  // Set the Device Name characteristic in the GAP GATT Service
  // For more information, see the section in the User's Guide:
  // http://software-dl.ti.com/lprf/ble5stack-latest/
  GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, (void *)pAttDeviceName);

  // Initialize GATT attributes
  GGS_AddService(GATT_ALL_SERVICES);           // GAP GATT Service
  GATTServApp_AddService(GATT_ALL_SERVICES);   // GATT Service

  // Register for GATT local events and ATT Responses pending for transmission
  GATT_RegisterForMsgs(appSelfEntity);

  if (role & (GAP_PROFILE_PERIPHERAL | GAP_PROFILE_BROADCASTER))
  // Set default values for Data Length Extension
  // Extended Data Length Feature is already enabled by default
  {
    // Set initial values to maximum
    #define BLEAPP_SUGGESTED_PDU_SIZE 251
    #define BLEAPP_SUGGESTED_TX_TIME 2120 //default is 328us(TX)

    // This API is documented in hci.h
    // See the LE Data Length Extension section in the BLE5-Stack User's Guide for information on using this command:
    // http://software-dl.ti.com/lprf/ble5stack-latest/
    HCI_LE_WriteSuggestedDefaultDataLenCmd(BLEAPP_SUGGESTED_PDU_SIZE, BLEAPP_SUGGESTED_TX_TIME);
  }
  else
  //Set default values for Data Length Extension
  //Extended Data Length Feature is already enabled by default
  //in build_config.opt in stack project.
  {
    //Change initial values of RX/TX PDU and Time, RX is set to max. by default(251 octets, 2120us)
    #define APP_SUGGESTED_RX_PDU_SIZE 251     //default is 251 octets(RX)
    #define APP_SUGGESTED_RX_TIME     17000   //default is 17000us(RX)
    #define APP_SUGGESTED_TX_PDU_SIZE 27      //default is 27 octets(TX)
    #define APP_SUGGESTED_TX_TIME     328     //default is 328us(TX)

    //This API is documented in hci.h
    //See the LE Data Length Extension section in the BLE5-Stack User's Guide for information on using this command:
    //http://software-dl.ti.com/lprf/ble5stack-latest/
    HCI_EXT_SetMaxDataLenCmd(APP_SUGGESTED_TX_PDU_SIZE, APP_SUGGESTED_TX_TIME, APP_SUGGESTED_RX_PDU_SIZE, APP_SUGGESTED_RX_TIME);
  }

  // Initialize GATT Client
  GATT_InitClient("");

  if (role & (GAP_PROFILE_CENTRAL | GAP_PROFILE_OBSERVER))
  {
      // Register to receive incoming ATT Indications/Notifications
      GATT_RegisterForInd(appSelfEntity);
  }

  return SUCCESS;
}

/*********************************************************************
 * @fn      bleStk_initAdvSet
 *
 * @brief   Initialize and starts advertise set (legacy or extended)
 *
 * @param advCallback     - callback for advertising progress states
 * @param advHandle       - return the created advertising handle
 * @param advParams       - pointer to structure of adversing parameters
 * @param advData         - pointer to array containing the advertise data
 * @param advDataLen      - length (in bytes) of advData
 * @param scanRespData    - pointer to array containing the scan response data
 * @param scanRespDataLen - length (in bytes) of scanRespDataLen
 *
 * @return SUCCESS upon successful initialization,
 *         else, relevant error code upon failure
 */
bStatus_t bleStk_initAdvSet(pfnBleStkAdvCB_t advCallback, uint8_t *advHandle,
                              GapAdv_eventMaskFlags_t eventMask, GapAdv_params_t *advParams,
                              uint16_t advDataLen ,uint8_t advData[],
                              uint16_t scanRespDataLen, uint8_t scanRespData[])
{
  bStatus_t status;

#ifdef ERPC_SERVER
  // keep the remote eRPC app callback
  remote_bleApp_GapAdvCb = advCallback;

  // Create Advertisement set and assign handle
  status = GapAdv_create((pfnGapCB_t)local_bleApp_GapAdvCb, advParams, advHandle);
#else
  status = GapAdv_create((pfnGapCB_t)advCallback, advParams, advHandle);
#endif
  if (status != SUCCESS)
  {
    return status;
  }

  // Load advertising data for set that is statically allocated by the app
#ifdef ERPC_SERVER
  status = bleStk_GapAdv_loadLocalByHandle(*advHandle, GAP_ADV_DATA_TYPE_ADV, advDataLen, advData);
#else
  status = GapAdv_loadByHandle(*advHandle, GAP_ADV_DATA_TYPE_ADV, advDataLen, advData);
#endif
  if (status != SUCCESS)
  {
    return status;
  }

  // Load scan response data for set that is statically allocated by the app
  if (scanRespData != NULL)
  {
#ifdef ERPC_SERVER
    status = bleStk_GapAdv_loadLocalByHandle(*advHandle, GAP_ADV_DATA_TYPE_SCAN_RSP, scanRespDataLen, scanRespData);
#else
    status = GapAdv_loadByHandle(*advHandle, GAP_ADV_DATA_TYPE_SCAN_RSP, scanRespDataLen, scanRespData);
#endif
    if (status != SUCCESS)
    {
      return status;
    }
  }

  // Set event mask for set
  status = GapAdv_setEventMask(*advHandle, eventMask);
  if (status != SUCCESS)
  {
    return status;
  }

  // Enable advertising for set
  status = GapAdv_enable(*advHandle, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);

  return status;
}

#ifdef ERPC_SERVER
//!< Advertising set payload pointer, first set [0..Max] is for ADV, second set [Max+1..2xMax] is for SCAN_RSP
uint8_t  *localAdvPayload[AE_DEFAULT_NUM_ADV_SETS*2];
/*********************************************************************
 * @fn      bleStk_GapAdv_loadLocalByHandle
 *
 * @brief   copy the advertising data into local buffers before loading the ADV data
 *
 * @param advHandle - the ADV set handle
 * @param advtype   - ADV type
 * @param len       - ADV data len
 * @param pData     - ADV data buffer
 *
 * @return  SUCCESS of FAILURE.
 */
bStatus_t bleStk_GapAdv_loadLocalByHandle(uint8_t advHandle,GapAdv_dataTypes_t advtype,
                                    uint16_t len, uint8_t *pData)
{
  bStatus_t status;
  uint8_t index;

  // set the index of the saved payload, first set [0..Max] is for ADV, second set [Max+1..2xMax] is for SCAN_RSP
  // advHandle will be offset in the selected set
  if (advtype == GAP_ADV_DATA_TYPE_SCAN_RSP)
  {
    index = AE_DEFAULT_NUM_ADV_SETS;    // scan data is the last (20) adv set in the buffer
  }
  else if (advtype == GAP_ADV_DATA_TYPE_ADV)
  {
    index = 0;
  }
  else
  {
    return(INVALIDPARAMETER);
  }

  if (advHandle >= AE_DEFAULT_NUM_ADV_SETS )
  {
    // Unknow Handler
    return(bleGAPNotFound);
  }

  // offset = advHandle+index is now the offset to advPayload[offset] to save the ADV payload

  // Free the handle if already exists
  if (localAdvPayload[advHandle+index])
  {
    status = GapAdv_prepareLoadByHandle(advHandle, GAP_ADV_FREE_OPTION_ADV_DATA);
  }

  if (len)
  {
    if(!(localAdvPayload[advHandle+index] = (uint8_t*) bleStack_malloc(len)))
    {
      return(bleNoResources);
    }

    // Copy the ADV data into local DB
    memcpy(localAdvPayload[advHandle+index], pData, len);
  }
  else
  {
    // emptying Adv Data...
    localAdvPayload[advHandle+index] = NULL;
  }

  // Configure the local copied ADV data into the BLE stack
  status |= GapAdv_loadByHandle(advHandle, advtype,
                               len, localAdvPayload[advHandle+index]);
  return(status);
}

uint8_t *shim_GapAdvCb(uint32 event, GapAdv_data_t *pBuf, uint32_t *arg)
{
  uint8_t *pMsg = malloc(sizeof(uint32) + sizeof(GapAdv_data_t) + sizeof(uint32_t));
  if (pMsg)
  {
    uint32_t int_val;

    // copy event
    int_val = (uint32_t)event;
    memcpy(pMsg, (void *)&int_val, sizeof(uint32_t));

    if (pBuf)
    {
      // copy the buffer
      memcpy(pMsg + sizeof(uint32_t), pBuf, sizeof(GapAdv_data_t));
    }
    else
    {
      // fill with zero
      memset(pMsg + sizeof(uint32_t), 0, sizeof(GapAdv_data_t));
    }

    // copy arg
    int_val = *arg;
    memcpy(pMsg + sizeof(uint32_t) + sizeof(GapAdv_data_t), (void *)&int_val, sizeof(uint32_t));
  }

  return pMsg;
}

static void appContext_GapAdvCb(uint8_t *pMsg)
{
  if (remote_bleApp_GapAdvCb)
  {
    uint32_t event = *(uint32_t *)pMsg;
    GapAdv_data_t *pBuf = (GapAdv_data_t *)(pMsg+sizeof(uint32_t));
    uint32_t *arg = (uint32_t *)(pMsg+sizeof(uint32_t)+sizeof(GapAdv_data_t));

    remote_bleApp_GapAdvCb(event, pBuf, arg);
  }
}

static void local_bleApp_GapAdvCb(uint32 event, GapAdv_data_t *pBuf, uint32_t *arg)
{
  uint8_t *pMsg = shim_GapAdvCb(event, pBuf, arg);
  if (pMsg)
  {
    bleSrv_callOnAppContext((callbackFxn_t)appContext_GapAdvCb, pMsg);
  }

  bleStack_free(pBuf);
}

#endif //ERPC_SERVER

/*********************************************************************
 * @fn      bleStk_getDevAddr
 *
 * @brief   Get the address of this device
 *
 * @param   wantIA - TRUE  for Identity Address
 *                   FALSE for Resolvable Private Address (if GAP device
 *                   address mode is @ref ADDRMODE_RP_WITH_PUBLIC_ID or
 *                   @ref ADDRMODE_RP_WITH_RANDOM_ID)
 * @param   pAddr - the returned address
 *
 * @return  None.
 */
void bleStk_getDevAddr(uint8_t wantIA, uint8_t *pAddr)
{
  uint8_t* pDevAddr = NULL;

  pDevAddr = GAP_GetDevAddress(wantIA);
  memcpy(pAddr, pDevAddr, B_ADDR_LEN);
}
