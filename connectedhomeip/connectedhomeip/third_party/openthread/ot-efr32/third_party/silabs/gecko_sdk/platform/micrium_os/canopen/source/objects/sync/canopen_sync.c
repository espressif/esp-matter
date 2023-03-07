/***************************************************************************//**
 * @file
 * @brief CANopen Synchronization (Sync) Object Service
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_CANOPEN_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <canopen_cfg.h>

#include  <cpu/include/cpu.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/include/rtos_err.h>

#if (CANOPEN_SYNC_EN == DEF_ENABLED)
#include  <canopen/include/canopen_dict.h>
#include  <canopen/source/core/canopen_if_priv.h>
#include  <canopen/source/core/canopen_core_priv.h>
#include  <canopen/source/objects/pdo/canopen_tpdo_priv.h>
#include  <canopen/source/objects/pdo/canopen_rpdo_priv.h>
#include  <canopen/source/objects/sync/canopen_sync_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (CAN, CANOPEN)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_CANOPEN

/********************************************************************************************************
 ********************************************************************************************************
 *                                        INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          CANopen_SyncInit()
 *
 * @brief    Read and cache the synchronization object.
 *
 * @param    p_sync  Pointer to SYNC object.
 *
 * @param    p_node  Pointer to parent node.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void CANopen_SyncInit(CANOPEN_SYNC *p_sync,
                      CANOPEN_NODE *p_node,
                      RTOS_ERR     *p_err)
{
  CANOPEN_NODE_HANDLE handle;
#if ((CANOPEN_TPDO_MAX_QTY > 0) || (CANOPEN_RPDO_MAX_QTY > 0))
  CPU_INT08U i;
#endif

  RTOS_ASSERT_DBG_ERR_SET((p_sync != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_node != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  CANOPEN_NODE_HANDLE_SET(handle, p_node);

  p_sync->NodePtr = p_node;                                     // link to parent node
#if (CANOPEN_TPDO_MAX_QTY > 0)
  for (i = 0; i < CANOPEN_TPDO_MAX_QTY; i++) {                  // loop through TPDO table
    p_sync->TPdoSyncCntTbl[i] = 0;                              // clear transmission time
    p_sync->TPdoTblPtr[i] = DEF_NULL;                           // invalidate pointer to TPDO
    p_sync->TPdoTransmissionTypeTbl[i] = 0;                     // clear number of SYNC till sending
  }
#endif
#if (CANOPEN_RPDO_MAX_QTY > 0)
  for (i = 0; i < CANOPEN_RPDO_MAX_QTY; i++) {                  // clear through RPDO table
    p_sync->RPdoTblPtr[i] = DEF_NULL;                           // invalidate pointer to RPDO
  }
#endif
  CANopen_DictLongRd(handle,                                    // get sync identifier in object directory
                     CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_COB_ID_SYNC_MSG, 0),
                     &p_sync->CobId,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_sync->CobId = 0;                                          // clear sync id
  }
}

/****************************************************************************************************//**
 *                                         CANopen_SyncHandler()
 *
 * @brief    Handle the synchronous PDOs as defined in the object dictionary, when a SYNC message is
 *           received
 *
 * @param    p_sync   Pointer to SYNC object.
 *******************************************************************************************************/
void CANopen_SyncHandler(CANOPEN_SYNC *p_sync)
{
#if ((CANOPEN_TPDO_MAX_QTY > 0) || (CANOPEN_RPDO_MAX_QTY > 0))
  CPU_INT08U ix;
  RTOS_ERR   local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
#endif

#if (CANOPEN_TPDO_MAX_QTY > 0)
  for (ix = 0; ix < CANOPEN_TPDO_MAX_QTY; ix++) {               // Loop through transmit PDO table
    if (p_sync->TPdoTblPtr[ix] != DEF_NULL) {                   // See, if TPDO is synchronous
                                                                // See, if transmission type is 0
      if (p_sync->TPdoTransmissionTypeTbl[ix] == CANOPEN_TPDO_COMM_TYPE_SYNC_ACYCLIC) {
        //                                                         Send TPDO only if an app event has occurred.
        if ((p_sync->TPdoTblPtr[ix]->Flags & CANOPEN_TPDO_FLG___E) != 0) {
          p_sync->TPdoTblPtr[ix]->Flags &= ~CANOPEN_TPDO_FLG___E;
          CANopen_TPdoWr(p_sync->TPdoTblPtr[ix], &local_err);
          CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE, p_sync->NodePtr->CntErrPtr->Sync.TpdoTxFailCtr);
        }
      } else {                                                  // See if sync time fits.
        if (p_sync->TPdoSyncCntTbl[ix] == p_sync->TPdoTransmissionTypeTbl[ix]) {
          //                                                       Send TPDO at matching SYNC
          CANopen_TPdoWr(p_sync->TPdoTblPtr[ix], &local_err);
          CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE, p_sync->NodePtr->CntErrPtr->Sync.TpdoTxFailCtr);
          p_sync->TPdoSyncCntTbl[ix] = 0;                       // Reset SYNC time counter.
        }
      }
    }
  }
#endif

#if (CANOPEN_RPDO_MAX_QTY > 0)
  for (ix = 0; ix < CANOPEN_RPDO_MAX_QTY; ix++) {               // loop through receive PDO table
                                                                // see, if RPDO is synchronous
    if ((p_sync->RPdoTblPtr[ix] != DEF_NULL)
        && ((p_sync->RPdoTblPtr[ix]->Flag &  CANOPEN_RPDO_FLG_R_) != 0)) {
      CANopen_RPdoWr(p_sync->RPdoTblPtr[ix],
                     &p_sync->RPdoFrmTbl[ix],
                     &local_err);
      p_sync->RPdoTblPtr[ix]->Flag &= ~CANOPEN_RPDO_FLG_R_;
      CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE, p_sync->NodePtr->CntErrPtr->Sync.RpdoObjDictionaryWrFailCtr);
    }
  }
#endif
}

/****************************************************************************************************//**
 *                                           CANopen_SyncAdd()
 *
 * @brief    Add a PDO to the synchronous PDO tables.
 *
 * @param    p_sync      Pointer to SYNC object.
 *
 * @param    nbr         PDO number.
 *
 * @param    msg_type    CAN message type.
 *
 * @param    tx_type     PDO transmission type.
 *******************************************************************************************************/
void CANopen_SyncAdd(CANOPEN_SYNC *p_sync,
                     CPU_INT16U   nbr,
                     CPU_INT08U   msg_type,
                     CPU_INT08U   tx_type)
{
#if (CANOPEN_TPDO_MAX_QTY > 0)
  //                                                               ----------------- SYNCHRONOUS TPDO -----------------
  if (msg_type == CANOPEN_SYNC_FLG_TX) {
    if (p_sync->TPdoTblPtr[nbr] == DEF_NULL) {                  // see, if PDO is not configured
                                                                // link TPDO to synchronous object
      p_sync->TPdoTblPtr[nbr] = &p_sync->NodePtr->TpdoTbl[nbr];
    }
    p_sync->TPdoTransmissionTypeTbl[nbr] = tx_type;             // set number of SYNC till send
    p_sync->TPdoSyncCntTbl[nbr] = 0;                            // reset SYNC counter
  }
#else
  PP_UNUSED_PARAM(tx_type);
#endif

#if (CANOPEN_RPDO_MAX_QTY > 0)
  //                                                               ----------------- SYNCHRONOUS RPDO -----------------
  if (msg_type == CANOPEN_SYNC_FLG_RX) {
    if (p_sync->RPdoTblPtr[nbr] == DEF_NULL) {                  // see, if PDO is not configured
                                                                // link RPDO to synchronous object
      p_sync->RPdoTblPtr[nbr] = &p_sync->NodePtr->RpdoTbl[nbr];
    }
  }
#endif
}

/****************************************************************************************************//**
 *                                         CANopen_SyncRemove()
 *
 * @brief    Remove a SYNC PDO from the SYNC PDO table.
 *
 * @param    p_sync      Pointer to SYNC object.
 *
 * @param    nbr         PDO number.
 *
 * @param    msg_type    CAN message type.
 *******************************************************************************************************/
void CANopen_SyncRemove(CANOPEN_SYNC *p_sync,
                        CPU_INT16U   nbr,
                        CPU_INT08U   msg_type)
{
#if (CANOPEN_TPDO_MAX_QTY > 0)
  if (msg_type == CANOPEN_SYNC_FLG_TX) {                        // ----------------- SYNCHRONOUS TPDO -----------------
    p_sync->TPdoTblPtr[nbr] = DEF_NULL;                         // indicate PDO is not synchronous
    p_sync->TPdoTransmissionTypeTbl[nbr] = 0;                   // reset number of SYNC till transmission
    p_sync->TPdoSyncCntTbl[nbr] = 0;                            // reset SYNC counter
  }
#endif

#if (CANOPEN_RPDO_MAX_QTY > 0)
  if (msg_type == CANOPEN_SYNC_FLG_RX) {                        // ----------------- SYNCHRONOUS RPDO -----------------
    p_sync->RPdoTblPtr[nbr] = DEF_NULL;                         // clear pointer to received frame
  }
#endif
}

#if (CANOPEN_RPDO_MAX_QTY > 0)
/****************************************************************************************************//**
 *                                           CANopen_SyncRx()
 *
 * @brief    Handle a received synchronous RPDO.
 *
 * @param    p_sync  Pointer to SYNC object.
 *
 * @param    p_frm   CAN Frame, received from CAN bus.
 *******************************************************************************************************/
void CANopen_SyncRx(CANOPEN_SYNC   *p_sync,
                    CANOPEN_IF_FRM *p_frm)
{
  CPU_INT16U ix;
  CPU_INT16U data_ix;

  for (ix = 0; ix < CANOPEN_RPDO_MAX_QTY; ix++) {                    // loop through the RPDO table
    if (p_sync->RPdoTblPtr[ix]->MsgId == p_frm->MsgId) {             // see, if message matches RPDO
      for (data_ix = 0; data_ix < p_frm->DLC; data_ix++) {           // yes: cpy payload of CAN message in cache
        p_sync->RPdoFrmTbl[ix].Data[data_ix] = p_frm->Data[data_ix]; // copy data byte
      }
      p_sync->RPdoFrmTbl[ix].DLC = p_frm->DLC;                       // copy DLC of message
      p_sync->RPdoTblPtr[ix]->Flag |= CANOPEN_RPDO_FLG_R_;

      break;
    }
  }
}
#endif

#if (CANOPEN_TPDO_MAX_QTY > 0)
/****************************************************************************************************//**
 *                                         CANopen_SyncUpdate()
 *
 * @brief    Check the given frame to be a SYNC message.
 *
 * @param    p_sync  Pointer to SYNC object.
 *
 * @param    p_frm   CAN Frame, received from CAN bus.
 *
 * @return   DEF_YES, CAN message frame is a SYNC message.
 *           DEF_NO,  CAN message frame is not a SYNC message.
 *******************************************************************************************************/
CPU_BOOLEAN CANopen_SyncUpdate(CANOPEN_SYNC   *p_sync,
                               CANOPEN_IF_FRM *p_frm)
{
  CPU_INT08U  ix;
  CPU_BOOLEAN can_msg_is_sync = DEF_NO;

  if (p_frm->MsgId == p_sync->CobId) {                          // see, if SYNC message is received
    for (ix = 0; ix < CANOPEN_TPDO_MAX_QTY; ix++) {             // loop through synchronous transmit PDOs
      if (p_sync->TPdoTblPtr[ix] != DEF_NULL) {                 // see, if TPDO is synchronous
        p_sync->TPdoSyncCntTbl[ix]++;                           // increment SYNC counter for this TPDO
      }
    }
    can_msg_is_sync = DEF_YES;                                  // indicate SNYC message detected/consumed
  }

  return (can_msg_is_sync);                                     // return function result
}
#endif

#endif // CANOPEN_SYNC_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_CANOPEN_AVAIL
