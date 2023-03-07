/***************************************************************************//**
 * @file
 * @brief CANopen Process Data Objects (Pdo) Service - Transmit Pdo
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

#if (CANOPEN_TPDO_MAX_QTY > 0)
#include  <cpu/include/cpu.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/include/rtos_err.h>

#include  <canopen/source/core/canopen_core_priv.h>
#include  <canopen/source/objects/pdo/canopen_tpdo_priv.h>
#include  <canopen/source/objects/sync/canopen_sync_priv.h>
#include  <canopen/source/core/canopen_obj_priv.h>
#include  <canopen/source/core/canopen_if_priv.h>
#include  <canopen/source/core/canopen_dict_priv.h>
#include  <canopen/source/core/canopen_tmr_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (CAN, CANOPEN)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_CANOPEN

#define  CANOPEN_TPDO_MAX_DATA_PAYLOAD      8u

/********************************************************************************************************
 *                                       TPDO MAPPING PARAMETER
 *******************************************************************************************************/

#define  CANOPEN_TPDO_MAPPING_OBJ_LEN_MSK   0xFF

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void CANopen_TPdoAsyncCtrl(CANOPEN_NODE_HANDLE node_handle,
                                  CANOPEN_OBJ         *p_obj,
                                  CPU_INT16U          fnct,
                                  CPU_INT32U          param,
                                  RTOS_ERR            *p_err);

static void CANopen_TPdoSyncCtrl(CANOPEN_NODE_HANDLE node_handle,
                                 CANOPEN_OBJ         *p_obj,
                                 CPU_INT16U          fnct,
                                 CPU_INT32U          param,
                                 RTOS_ERR            *p_err);

static void CANopen_TPdoEventWr(CANOPEN_NODE_HANDLE node_handle,
                                CANOPEN_OBJ         *p_obj,
                                void                *p_buf,
                                CPU_INT32U          size,
                                RTOS_ERR            *p_err);

static void CANopen_TPdoMapAdd(CANOPEN_TPDO_LINK *p_map,
                               CANOPEN_OBJ       *p_obj,
                               CPU_INT16U        nbr);

static void CANopen_TPdoInvalidWr(CANOPEN_NODE_HANDLE node_handle,
                                  CANOPEN_OBJ         *p_obj,
                                  void                *p_buf,
                                  CPU_INT32U          size,
                                  RTOS_ERR            *p_err);

static void CANopen_TPdoInvalidRd(CANOPEN_NODE_HANDLE node_handle,
                                  CANOPEN_OBJ         *p_obj,
                                  void                *p_buf,
                                  CPU_INT32U          size,
                                  RTOS_ERR            *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/*
 ********************************************************************************************************
 *                                    Object type TPDO asynchronous
 *******************************************************************************************************/

const CANOPEN_OBJ_TYPE CANopen_ObjTypeTpdoAsync = {
  .SizeCb = DEF_NULL,
  .CtrlCb = CANopen_TPdoAsyncCtrl,
  .RdCb = DEF_NULL,
  .WrCb = DEF_NULL
};

/*
 ********************************************************************************************************
 *                                    Object type TPDO synchronous
 *******************************************************************************************************/

const CANOPEN_OBJ_TYPE CANopen_ObjTypeTpdoSync = {
  .SizeCb = DEF_NULL,
  .CtrlCb = CANopen_TPdoSyncCtrl,
  .RdCb = DEF_NULL,
  .WrCb = DEF_NULL
};

/*
 ********************************************************************************************************
 *                                        Object type TPDO event
 *******************************************************************************************************/

const CANOPEN_OBJ_TYPE CANopen_ObjTypeTpdoEvent = {
  .SizeCb = DEF_NULL,
  .CtrlCb = DEF_NULL,
  .RdCb = DEF_NULL,
  .WrCb = CANopen_TPdoEventWr
};

/*
 ********************************************************************************************************
 *                                  Object type TPDO invalid Sub-index
 *******************************************************************************************************/

const CANOPEN_OBJ_TYPE CANopen_ObjTypeTpdoInvalid = {
  .SizeCb = DEF_NULL,
  .CtrlCb = DEF_NULL,
  .RdCb = CANopen_TPdoInvalidRd,
  .WrCb = CANopen_TPdoInvalidWr
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                         INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          CANopen_TPdoInit()
 *
 * @brief    Checks the object dictionary and creates the PDO related configuration data from the object
 *           dictionary settings.
 *
 * @param    p_pdo   Pointer to start of TPDO array.
 *
 * @param    p_node  Pointer to parent node object.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void CANopen_TPdoInit(CANOPEN_TPDO *p_pdo,
                      CANOPEN_NODE *p_node,
                      RTOS_ERR     *p_err)
{
  CANOPEN_NODE_HANDLE handle;
  CPU_INT16U          nbr;                                      // Local: number of PDO
  CPU_INT08U          on;                                       // Local: number of mapped objects in PDO
  CPU_INT08U          tnum;                                     // Local: number of PDO Com Entries

  RTOS_ASSERT_DBG_ERR_SET((p_pdo != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_node != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  CANOPEN_NODE_HANDLE_SET(handle, p_node);

  CANopen_TPdoMapClr(p_node->TpdoMappingTbl);                   // initialize signal link table
  for (nbr = 0; nbr < CANOPEN_TPDO_MAX_QTY; nbr++) {            // loop through all possible supported TPDO
    p_pdo[nbr].NodePtr = p_node;                                // link CANopen node informations
    p_pdo[nbr].AsyncEventTmrId = CANOPEN_TMR_INVALID_ID;        // indicate no active event timer
    p_pdo[nbr].AsyncInhibitTmrId = CANOPEN_TMR_INVALID_ID;      // indicate no active inhibit timer
    p_pdo[nbr].MsgId = CANOPEN_TPDO_COMM_COB_ID_VALID;          // indicate no configured message
    p_pdo[nbr].MapObjQty = 0;                                   // clear number of mapped TPDO objects
    for (on = 0; on < CANOPEN_TPDO_MAX_MAP_QTY; on++) {         // loop through all object mappings
      p_pdo[nbr].MapPtrTbl[on] = DEF_NULL;                      // clear object mapping
    }
    CANopen_DictByteRd(handle,                                  // check TPDO communication settings
                       CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_TPDO_COMM_PARAM_START, 0),
                       &tnum,
                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {           // TPDO Communication settings are existing
      CANopen_TPdoReset(p_pdo, nbr, p_err);                     // scan TPDO communication and mapping
    }
  }
}

/****************************************************************************************************//**
 *                                           CANopen_TPdoClr()
 *
 * @brief    Sets the PDO related configuration data to a known state (inactive).
 *
 * @param    p_pdo   Pointer to start of TPDO array.
 *
 * @param    p_node  Pointer to parent node object.
 *******************************************************************************************************/
void CANopen_TPdoClr(CANOPEN_TPDO *p_pdo,
                     CANOPEN_NODE *p_node)
{
  CPU_INT16U nbr;                                               // Local: number of PDO
  CPU_INT08U on;                                                // Local: number of mapped objects in PDO

  RTOS_ASSERT_DBG((p_pdo != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG((p_node != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  CANopen_TPdoMapClr(p_node->TpdoMappingTbl);                   // initialize signal link table
  for (nbr = 0; nbr < CANOPEN_TPDO_MAX_QTY; nbr++) {            // loop through all possible supported TPDO
    p_pdo[nbr].NodePtr = p_node;                                // link CANopen node informations
    p_pdo[nbr].AsyncEventTmrId = CANOPEN_TMR_INVALID_ID;        // indicate no active event timer
    p_pdo[nbr].AsyncInhibitTmrId = CANOPEN_TMR_INVALID_ID;      // indicate no active inhibit timer
    p_pdo[nbr].MsgId = CANOPEN_TPDO_COMM_COB_ID_VALID;          // indicate no configured message
    p_pdo[nbr].MapObjQty = 0;                                   // clear number of mapped TPDO objects
    for (on = 0; on < CANOPEN_TPDO_MAX_MAP_QTY; on++) {         // loop through all object mappings
      p_pdo[nbr].MapPtrTbl[on] = DEF_NULL;                      // clear object mapping
    }
  }
}

/****************************************************************************************************//**
 *                                          CANopen_TPdoReset()
 *
 * @brief    Scans the object dictionary for the TPDO communication and mapping parameter. The function
 *           will fill the TPDO info structure with the corresponding informations.
 *
 * @param    p_pdo   Pointer to start of TPDO array.
 *
 * @param    nbr     TPDO Number (0..511).
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) The following list shows the considered communication parameter entries:
 *               -# 0x1800+[nbr] : 0x01 = TPDO CAN identifier
 *               -# 0x1800+[nbr] : 0x02 = Transmission type
 *               -# 0x1800+[nbr] : 0x03 = Inhibit time
 *               -# 0x1800+[nbr] : 0x05 = Event timer (for transmission type 254 and 255)
 *
 * @note     (2) The object entry 0x1800+[nbr] : 0x04 is reserved and should be not existing.
 *               This is not checked with this function.
 *******************************************************************************************************/
void CANopen_TPdoReset(CANOPEN_TPDO *p_pdo,
                       CPU_INT16U   nbr,
                       RTOS_ERR     *p_err)
{
  CANOPEN_NODE_HANDLE handle;
  CANOPEN_TPDO        *p_wrk_tpdo;                              // Local: Pointer to working TPDO
                                                                // Local: COBID from profile
  CPU_INT32U id = CANOPEN_TPDO_COMM_COB_ID_VALID;
  CPU_INT16U inhibit = 0;                                       // Local: inhibit time from profile [100us]
  CPU_INT16U tmr = 0;                                           // Local: event timer from profile [1ms]
  CPU_INT08U type = 0;                                          // Local: transmission type from profile
  CPU_INT32U inhibit_time_us = 0;
  CPU_INT32U event_tmr_us = 0;

  p_wrk_tpdo = &p_pdo[nbr];                                     // set pointer to working pdo
  if (p_wrk_tpdo->AsyncEventTmrId >= 0) {                       // see, if TPDO event timer is activated
    CANopen_TmrDel(&p_wrk_tpdo->NodePtr->Tmr, p_wrk_tpdo->AsyncEventTmrId, p_err);
    RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    p_wrk_tpdo->AsyncEventTmrId = CANOPEN_TMR_INVALID_ID;       // indicate no active event timer
  }
  if (p_wrk_tpdo->AsyncInhibitTmrId >= 0) {                     // see, if TPDO inhibit timer is activated
    CANopen_TmrDel(&p_wrk_tpdo->NodePtr->Tmr, p_wrk_tpdo->AsyncInhibitTmrId, p_err);
    RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    p_wrk_tpdo->AsyncInhibitTmrId = CANOPEN_TMR_INVALID_ID;     // indicate no active event timer
#if (CANOPEN_SYNC_EN == DEF_ENABLED)
  }
  if ((p_wrk_tpdo->Flags & CANOPEN_TPDO_FLG_S__) != 0) {        // see, if TPDO is synchronized
    CANopen_SyncRemove(&p_pdo->NodePtr->Sync,                   // remove TPDO from SYNC management table
                       nbr,
                       CANOPEN_SYNC_FLG_TX);
#endif
  }
  p_wrk_tpdo->Flags = 0;                                        // clear all TPDO flags

  CANOPEN_NODE_HANDLE_SET(handle, p_wrk_tpdo->NodePtr);

  //                                                               ------------ C O M M U N I C A T I O N -------------
  CANopen_DictByteRd(handle,                                    // get TPDO transmission type
                     CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_TPDO_COMM_PARAM_START + nbr, 2),
                     &type,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CANopen_DictWordRd(handle,                                    // get TPDO inhibit time
                     CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_TPDO_COMM_PARAM_START + nbr, 3),
                     &inhibit,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                        // reset error: inhibit time is optional
  }

  if (inhibit > 0) {                                            // see, if inhibit time is enabled
                                                                // Inhibit time [100us]
    inhibit_time_us = inhibit * CANOPEN_TPDO_COMM_INHIBIT_TIME_UNIT_uS;

    if (inhibit_time_us >= p_pdo->NodePtr->TmrPeriod) {
      inhibit_time_us = inhibit_time_us / p_pdo->NodePtr->TmrPeriod;
    } else {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      return;
    }
  }

  p_pdo[nbr].AsyncInhibitTime = inhibit_time_us;                // convert and set TPDO inhibit[ticks]

  //                                                               see, if transmission type = asynchronous
  if ((type == CANOPEN_TPDO_COMM_TYPE_ASYNC_EVENT_VENDOR_SPECIFIC)
      || (type == CANOPEN_TPDO_COMM_TYPE_ASYNC_EVENT_DEV_APP_SPECIFIC)) {
    CANopen_DictWordRd(handle,                                  // get TPDO event timer cycle
                       CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_TPDO_COMM_PARAM_START + nbr, 5),
                       &tmr,
                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                      // reset error: event time is optional
    }

#if (CANOPEN_SYNC_EN == DEF_DISABLED)
  } else {                                                      // otherwise: synchronous transmission type
                                                                // set error code
    RTOS_ERR_SET(*p_err, RTOS_ERR_CANOPEN_xPDO_COM_INVALID_CFG);
    return;                                                     // return error indication
#endif
  }

  CANopen_DictLongRd(handle,                                    // get TPDO identifier
                     CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_TPDO_COMM_PARAM_START + nbr, 1),
                     &id,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if ((id & CANOPEN_TPDO_COMM_COB_ID_RTR) == 0) {               // see, if PDO is configured with RTR
    RTOS_ERR_SET(*p_err, RTOS_ERR_CANOPEN_xPDO_COM_INVALID_CFG);
    return;
  }

  if ((id & CANOPEN_TPDO_COMM_COB_ID_FRM) != 0) {               // see, if PDO is configured with ext. ID
    RTOS_ERR_SET(*p_err, RTOS_ERR_CANOPEN_xPDO_COM_INVALID_CFG);
    return;
  }

  if ((id & CANOPEN_TPDO_COMM_COB_ID_VALID) == 0) {
    //                                                             set identifier in message
    p_pdo[nbr].MsgId = (id & CANOPEN_TPDO_COMM_COB_ID_CAN_ID_29_BITS_MSK);
  } else {
    p_pdo[nbr].MsgId = CANOPEN_TPDO_COMM_COB_ID_VALID;          // set identifier in message
  }
  //                                                               ------------------ M A P P I N G -------------------

  CANopen_TPdoMapGet(p_pdo, nbr, p_err);                        // get TPDO mapping from object dictionary
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_pdo[nbr].MsgId != CANOPEN_TPDO_COMM_COB_ID_VALID) {     // see, if message configuration is valid
#if (CANOPEN_SYNC_EN == DEF_ENABLED)
    //                                                             see, if transmission type is 0..240
    if (type <= CANOPEN_TPDO_COMM_TYPE_SYNC_CYCLIC_MAX_VAL) {
      p_pdo[nbr].Flags |= CANOPEN_TPDO_FLG_S__;                 // set message type to synchronous
      CANopen_SyncAdd(&p_pdo->NodePtr->Sync,                    // add TPDO to SYNC management table
                      nbr,
                      CANOPEN_SYNC_FLG_TX,
                      type);
    }
#endif
  }

  if (tmr > 0) {
    event_tmr_us = tmr * DEF_TIME_NBR_uS_PER_mS;
    if (event_tmr_us >= p_pdo->NodePtr->TmrPeriod) {
      event_tmr_us = event_tmr_us / p_pdo->NodePtr->TmrPeriod;
    } else {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      return;
    }
  }

  p_pdo[nbr].AsyncEventTmrTime = event_tmr_us;                  // set TPDO event timer.

  if (p_pdo[nbr].AsyncEventTmrTime > 0) {                       // see, if event timer is enabled
                                                                // create cyclic timer for event timer
    p_pdo[nbr].AsyncEventTmrId = CANopen_TmrCreate(&p_pdo->NodePtr->Tmr,
                                                   p_pdo[nbr].AsyncEventTmrTime + nbr,
                                                   0,
                                                   CANopen_TPdoTmrEvent,
                                                   &p_pdo[nbr],
                                                   p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      CANOPEN_CTR_ERR_INC(p_pdo->NodePtr->CntErrPtr->Pdo.PdoTmrCreateFailCtr);
      return;
    }
  }
}

/****************************************************************************************************//**
 *                                         CANopen_TPdoMapGet()
 *
 * @brief    Gets the PDO mapping data out of the object dictionary and puts the pre-calculated values in
 *           the CAN message configuration.
 *
 * @param    p_pdo   Pointer to start of TPDO array.
 *
 * @param    nbr     TPDO Number (0..511).
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) The following list shows the considered mapping profile entries:
 *               -# 0x1A00+[nbr] : 0x00 = Number of mapped signals (0..8)
 *               -# 0x1A00+[nbr] : 0x01..0x08 = Mapped signal
 *******************************************************************************************************/
void CANopen_TPdoMapGet(CANOPEN_TPDO *p_pdo,
                        CPU_INT16U   nbr,
                        RTOS_ERR     *p_err)
{
  CANOPEN_NODE_HANDLE handle;
  CANOPEN_OBJ         *p_obj;                                   // Local: pointer to object entry
  CPU_INT32U          mapping;                                  // Local: PDO mapping information
  CPU_INT16U          idx;                                      // Local: object entry index
  CPU_INT16U          map_obj_ix;                               // Local: Loop counter for mapped objects
  CPU_INT08U          map_obj_qty;                              // Local: number of PDO mappings
  CPU_INT08U          dlc;                                      // Local: resulting data length code

  //                                                               set index to TPDO mapping profile
  idx = CANOPEN_COMM_PROFILE_OBJ_IX_TPDO_MAPPING_PARAM_START + nbr;

  CANOPEN_NODE_HANDLE_SET(handle, p_pdo[nbr].NodePtr);

  //                                                               get number of mapped signals
  CANopen_DictByteRd(handle, CANOPEN_DEV(idx, 0), &map_obj_qty, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  dlc = 0;                                                       // clear number of bytes in message

  for (map_obj_ix = 0; map_obj_ix < map_obj_qty; map_obj_ix++) { // Loop through all mapped signals
    CANopen_DictLongRd(handle,                                   // get mapping information of this PDO
                       CANOPEN_DEV(idx, (1 + map_obj_ix)),
                       &mapping,
                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
    //                                                             add number of bits, converted to bytes
    dlc += (CPU_INT08U)(mapping & CANOPEN_TPDO_MAPPING_OBJ_LEN_MSK) >> 3;
    if (dlc > CANOPEN_TPDO_MAX_DATA_PAYLOAD) {                  // more than 8 byte not possible in a PDO
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_TYPE);
      return;
    }
    //                                                             get mapping information of this PDO
    p_obj = CANopen_DictFind(&p_pdo->NodePtr->Dict, mapping, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {           // see, if object entry is not found
      return;
    }

    p_pdo[nbr].MapPtrTbl[map_obj_ix] = p_obj;                   // add object pointer to TPDO p_map
                                                                // add signal to TPDO number into link p_map
    CANopen_TPdoMapAdd(p_pdo->NodePtr->TpdoMappingTbl, p_obj, nbr);
  }

  p_pdo[nbr].MapObjQty = map_obj_qty;                           // set number of TPDO object references
}

/****************************************************************************************************//**
 *                                        CANopen_TPdoTmrEvent()
 *
 * @brief    Triggers a PDO with the event timer object (see sub-index 5 of communication parameters).
 *
 * @param    p_arg   Pointer to TPDO element.
 *******************************************************************************************************/
void CANopen_TPdoTmrEvent(void *p_arg)
{
  CANOPEN_TPDO *p_pdo;                                          // Local: pointer to TPDO object
  RTOS_ERR     local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_pdo = (CANOPEN_TPDO *)p_arg;                                // get pointer to TPDO

  CANopen_TPdoWr(p_pdo, &local_err);                            // transmit TPDO
  CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE,
                           p_pdo->NodePtr->CntErrPtr->Pdo.TPdoTxFailCtr);
}

/****************************************************************************************************//**
 *                                       CANopen_TPdoEndInhibit()
 *
 * @brief    Releases the transmission stop after the PDO inhibit time.
 *
 * @param    p_arg   Pointer to TPDO element
 *******************************************************************************************************/
void CANopen_TPdoEndInhibit(void *p_arg)
{
  CANOPEN_TPDO *p_pdo;                                          // Local: pointer to TPDO object
  RTOS_ERR     local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_pdo = (CANOPEN_TPDO *)p_arg;                                // get pointer to TPDO
  p_pdo->Flags &= ~CANOPEN_TPDO_FLG__I_;                        // clear inhibit time indication
  p_pdo->AsyncInhibitTmrId = CANOPEN_TMR_INVALID_ID;            // clear inhibit timer id
  if ((p_pdo->Flags & CANOPEN_TPDO_FLG___E) != 0) {             // see, if event occurs within inhibit time
    p_pdo->Flags &= ~CANOPEN_TPDO_FLG___E;                      // clear event indication

    CANopen_TPdoWr(p_pdo, &local_err);                          // yes: transmit TPDO
    CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE,
                             p_pdo->NodePtr->CntErrPtr->Pdo.TPdoTxFailCtr);
  }
}

/****************************************************************************************************//**
 *                                           CANopen_TPdoWr()
 *
 * @brief    Transmits a TPDO.
 *
 * @param    p_pdo   Pointer to TPDO element.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void CANopen_TPdoWr(CANOPEN_TPDO *p_pdo,
                    RTOS_ERR     *p_err)
{
  CANOPEN_IF_FRM frm;                                           // Local: memory for one CAN frame
  CPU_INT32U     sz;                                            // Local: size of linked object
  CPU_INT32U     data;                                          // Local: data buffer for TPDO massage
  CPU_INT08U     nbr;                                           // Local: number of linked object

  //                                                               see, if PDO communication is not allowed
  if ((p_pdo->NodePtr->Nmt.CurStateObjAllowed & CANOPEN_PDO_ALLOWED) == 0) {
    return;                                                     // abort transmission of PDO
  }
  if ((p_pdo->Flags & CANOPEN_TPDO_FLG__I_) != 0) {             // see, if inhibit time is active
    p_pdo->Flags |= CANOPEN_TPDO_FLG___E;                       // remember transmission request
    return;                                                     // abort transmission of PDO
  }
  if (p_pdo->MsgId == CANOPEN_TPDO_COMM_COB_ID_VALID) {         // see, if PDO identifier is valid
    return;
  }

  if (p_pdo->AsyncInhibitTime > 0) {                            // see, if inhibit time is enabled
    p_pdo->AsyncInhibitTmrId = CANOPEN_TMR_INVALID_ID;          // indicate deactivated inhibit timer

    p_pdo->Flags |= CANOPEN_TPDO_FLG__I_;                       // yes: stop further transmission of PDO
                                                                // create one-shot timer for end of inhibit
    p_pdo->AsyncInhibitTmrId = CANopen_TmrCreate(&p_pdo->NodePtr->Tmr,
                                                 p_pdo->AsyncInhibitTime,
                                                 0,
                                                 CANopen_TPdoEndInhibit,
                                                 (void *)p_pdo,
                                                 p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      CANOPEN_CTR_ERR_INC(p_pdo->NodePtr->CntErrPtr->Pdo.PdoTmrCreateFailCtr);
      return;
    }
  }
  if (p_pdo->AsyncEventTmrTime > 0) {
    p_pdo->AsyncEventTmrId = CANOPEN_TMR_INVALID_ID;            // indicate deactivated event timer
                                                                // create one-shot timer for event timer
    p_pdo->AsyncEventTmrId = CANopen_TmrCreate(&p_pdo->NodePtr->Tmr,
                                               p_pdo->AsyncEventTmrTime,
                                               0,
                                               CANopen_TPdoTmrEvent,
                                               (void *)p_pdo,
                                               p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      CANOPEN_CTR_ERR_INC(p_pdo->NodePtr->CntErrPtr->Pdo.PdoTmrCreateFailCtr);
      return;
    }
  }

  frm.MsgId = p_pdo->MsgId;                                     // set message identifier
  frm.DLC = 0;                                                  // clear DLC of TPDO message

  for (nbr = 0; nbr < p_pdo->MapObjQty; nbr++) {                // loop through all PDO objects
                                                                // get size of PDO object
    sz = CANopen_ObjSizeGet(p_pdo->MapPtrTbl[nbr], 0L, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
    if (sz <= (CPU_INT32U)(8 - frm.DLC)) {                      // see, if size is possible for TPDOs
      if (!CANOPEN_OBJ_IS_RD_ONLY(p_pdo->MapPtrTbl[nbr]->Key)) {
        KAL_LockAcquire(p_pdo->NodePtr->ObjLock,
                        KAL_OPT_PEND_NONE,
                        p_pdo->NodePtr->LockTimeoutMs,
                        p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          RTOS_ERR_CONTEXT_REFRESH(*p_err);
          return;
        }
      }
      CANopen_ObjValRd(p_pdo->NodePtr,
                       p_pdo->MapPtrTbl[nbr],                   // read object value
                       &data,
                       CANOPEN_OBJ_LONG,
                       p_pdo->NodePtr->NodeId,
                       p_err);
      if (!CANOPEN_OBJ_IS_RD_ONLY(p_pdo->MapPtrTbl[nbr]->Key)) {
        RTOS_ERR local_err;

        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

        KAL_LockRelease(p_pdo->NodePtr->ObjLock,
                        &local_err);
        RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }

      if (sz == CANOPEN_OBJ_BYTE) {                             // see, if object holds a byte value
        CANOPEN_FRM_SET_BYTE(&frm, data, frm.DLC);              // copy byte value to can frame
      } else if (sz == CANOPEN_OBJ_WORD) {                      // see, if object holds a word value
        CANOPEN_FRM_SET_WORD(&frm, data, frm.DLC);              // copy word value to can frame
      } else if (sz == CANOPEN_OBJ_LONG) {                      // see, if object holds a long value
        CANOPEN_FRM_SET_LONG(&frm, data, frm.DLC);              // copy long value to can frame
      }
      frm.DLC += (CPU_INT08U)sz;                                // add size to DLC of message
    }
  }

  if ((p_pdo->NodePtr->EventFnctsPtr != DEF_NULL)
      && (p_pdo->NodePtr->EventFnctsPtr->TpdoOnTx != DEF_NULL)) {
    CANOPEN_NODE_HANDLE handle;

    CANOPEN_NODE_HANDLE_SET(handle, p_pdo->NodePtr);

    //                                                             Application callback function
    p_pdo->NodePtr->EventFnctsPtr->TpdoOnTx(handle, &frm);
  }

  CANopen_IfWr(&p_pdo->NodePtr->If, &frm, p_err);               // Sent message via can bus

  CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE,
                           p_pdo->NodePtr->CntErrPtr->Pdo.TPdoTxFailCtr);
}

/****************************************************************************************************//**
 *                                         CANopen_TPdoMapClr()
 *
 * @brief    Initializes the signal to TPDO link mapping table.
 *
 * @param    p_map   Pointer to start of link mapping table.
 *******************************************************************************************************/
void CANopen_TPdoMapClr(CANOPEN_TPDO_LINK *p_map)
{
  CPU_INT16U id;                                                // Local: loop counter through link p_map

  for (id = 0; id < CANOPEN_TPDO_MAX_MAP_QTY; id++) {           // loop through all link p_map entries
    p_map[id].ObjPtr = DEF_NULL;                                // clear signal identifier
    p_map[id].Nbr = 0xFFFF;                                     // clear linked TPDO number
  }
}

/****************************************************************************************************//**
 *                                        CANopen_TPdoMapNbrDel()
 *
 * @brief    Deletes all entries, which contains the given TPDO number.
 *
 * @param    p_map   Pointer to start of link mapping table.
 *
 * @param    nbr     Linked TPDO number.
 *******************************************************************************************************/
void CANopen_TPdoMapNbrDel(CANOPEN_TPDO_LINK *p_map,
                           CPU_INT16U        nbr)
{
  CPU_INT16U id;                                                // Local: loop counter through link p_map

  for (id = 0; id < CANOPEN_TPDO_MAX_MAP_QTY; id++) {           // loop through all link p_map entries
    if (p_map[id].Nbr == nbr) {                                 // see, if TPDO number matches given value
      p_map[id].ObjPtr = DEF_NULL;                              // clear signal identifier
      p_map[id].Nbr = 0xFFFF;                                   // clear linked TPDO number
    }
  }
}

/****************************************************************************************************//**
 *                                        CANopen_TPdoMapSigDel()
 *
 * @brief    Deletes all entries, which contains the given signal identifier.
 *
 * @param    p_map   Pointer to start of link mapping table.
 *
 * @param    p_obj   Pointer to object entry.
 *******************************************************************************************************/
void CANopen_TPdoMapSigDel(CANOPEN_TPDO_LINK *p_map,
                           CANOPEN_OBJ       *p_obj)
{
  CPU_INT16U id;                                                // Local: loop counter through link p_map

  for (id = 0; id < CANOPEN_TPDO_MAX_MAP_QTY; id++) {           // loop through all link p_map entries
    if (p_map[id].ObjPtr == p_obj) {                            // see, if signal id matches given value
      p_map[id].ObjPtr = DEF_NULL;                              // clear signal identifier
      p_map[id].Nbr = 0xFFFF;                                   // clear linked TPDO number
    }
  }
}

/****************************************************************************************************//**
 *                                         CANopen_TPdoObjTrig()
 *
 * @brief    Allows the application to trigger a TPDO event for the given object entry. The event will be
 *           distributed to all TPDOs, which hold an active mapping entry to this object entry.
 *
 * @param    p_pdo   Pointer to start of TPDO array.
 *
 * @param    p_obj   Pointer to object entry.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void CANopen_TPdoObjTrig(CANOPEN_TPDO *p_pdo,
                         CANOPEN_OBJ  *p_obj,
                         RTOS_ERR     *p_err)
{
  CPU_INT32U n;                                                 // Local: loop counter through link p_map
  CPU_INT16U nbr;                                               // Local: number of TPDO

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_pdo != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  if (CANOPEN_OBJ_IS_PDOMAP(p_obj->Key) != 0) {                 // see, if the object is PDO mappable
    for (n = 0; n < CANOPEN_TPDO_MAX_MAP_QTY; n++) {            // loop through mapping link p_map
                                                                // see, if mapping object is addressed
      if (p_pdo->NodePtr->TpdoMappingTbl[n].ObjPtr == p_obj) {
        nbr = p_pdo->NodePtr->TpdoMappingTbl[n].Nbr;            // get PDO number of linked object
        CANopen_TPdoPdoTrig(p_pdo, nbr, p_err);                 // trigger this PDO via PDO number
      }
    }
  } else {                                                      // otherwise: object is not PDO mappable
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
  }
}

/****************************************************************************************************//**
 *                                         CANopen_TPdoPdoTrig()
 *
 * @brief    Allows the application to trigger a TPDO event for the given TPDO number.
 *
 * @param    p_pdo   Pointer to start of TPDO array.
 *
 * @param    nbr     TPDO Number (0..511).
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void CANopen_TPdoPdoTrig(CANOPEN_TPDO *p_pdo,
                         CPU_INT16U   nbr,
                         RTOS_ERR     *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_pdo != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  if (nbr < CANOPEN_TPDO_MAX_QTY) {                             // see, if given TPDO number is valid
    CANopen_TPdoWr(&p_pdo[nbr], p_err);                         // transmit TPDO immediately
  } else {                                                      // otherwise: TPDO number is invalid
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        CANopen_TPdoAsyncCtrl()
 *
 * @brief    Triggers all mapped TPDOs on change of the corresponding object entry value.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_obj           Pointer to object entry.
 *
 * @param    fnct            Control function code.
 *
 * @param    param           Control function parameter.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void CANopen_TPdoAsyncCtrl(CANOPEN_NODE_HANDLE node_handle,
                                  CANOPEN_OBJ         *p_obj,
                                  CPU_INT16U          fnct,
                                  CPU_INT32U          param,
                                  RTOS_ERR            *p_err)

{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;

  PP_UNUSED_PARAM(param);

  if (fnct == CANOPEN_TPDO_ASYNC) {                             // see, if correct function code is given
    CANopen_TPdoObjTrig(p_node->TpdoTbl, p_obj, p_err);         // trigger TPDO via object reference
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
  }
}

/****************************************************************************************************//**
 *                                        CANopen_TPdoSyncCtrl()
 *
 * @brief    Set event flag for all mapped TPDOs on change of the corresponding object entry value.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_obj           Pointer to object entry.
 *
 * @param    fnct            Control function code.
 *
 * @param    param           Control function parameter.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void CANopen_TPdoSyncCtrl(CANOPEN_NODE_HANDLE node_handle,
                                 CANOPEN_OBJ         *p_obj,
                                 CPU_INT16U          fnct,
                                 CPU_INT32U          param,
                                 RTOS_ERR            *p_err)
{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CPU_INT32U   tpdo_map_ix;                                     // Local: loop counter through link p_map
  CPU_INT16U   tpdo_ix;                                         // Local: number of TPDO

  PP_UNUSED_PARAM(param);                                       // unused; prevent compiler warning

  if (fnct == CANOPEN_TPDO_SYNC) {                              // see, if correct function code is given
    if (CANOPEN_OBJ_IS_PDOMAP(p_obj->Key) != 0) {               // see, if the object is PDO mappable
      for (tpdo_ix = 0; tpdo_ix < CANOPEN_TPDO_MAX_QTY; tpdo_ix++) {
        //                                                         loop through mapping link map
        for (tpdo_map_ix = 0; tpdo_map_ix < CANOPEN_TPDO_MAX_MAP_QTY; tpdo_map_ix++) {
          //                                                       see, if mapping object is addressed
          if (p_node->TpdoTbl[tpdo_ix].NodePtr->TpdoMappingTbl[tpdo_map_ix].ObjPtr == p_obj) {
            //                                                     see, if sync is active
            if ((p_node->TpdoTbl[tpdo_ix].Flags & CANOPEN_TPDO_FLG_S__) != 0) {
              //                                                   remember transmission request
              p_node->TpdoTbl[tpdo_ix].Flags |= CANOPEN_TPDO_FLG___E;
              return;
            }
          }
        }
      }
    } else {                                                    // otherwise: object is not PDO mappable
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    }
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
  }
}

/****************************************************************************************************//**
 *                                         CANopen_TPdoEventWr()
 *
 * @brief    Performs all necessary activities during a write access of the TPDO event timer communication
 *           object entry.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_obj           Pointer to object entry.
 *
 * @param    p_buf           Pointer to data buffer.
 *
 * @param    size            Size of given data in buffer.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void CANopen_TPdoEventWr(CANOPEN_NODE_HANDLE node_handle,
                                CANOPEN_OBJ         *p_obj,
                                void                *p_buf,
                                CPU_INT32U          size,
                                RTOS_ERR            *p_err)
{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CANOPEN_NMT  *nmt;                                            // Local: pointer to NMT management
  CANOPEN_TPDO *p_pdo;                                          // Local: pointer to TPDO data
  CPU_INT32U   cobid = 0;                                       // Local: CAN Identifier
  CPU_INT16U   cycTime;                                         // Local: new cycle timer
  CPU_INT16U   nbr;                                             // Local: number of TPDO
  CPU_INT32U   cyc_time_us = 0;
  RTOS_ERR     local_err;

  CANopen_ObjDirectWr(p_obj,                                    // write value to object data element
                      (void *)p_buf,
                      size,
                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  nmt = &p_node->Nmt;                                           // get pointer to NMT management
  nbr = CANOPEN_OBJ_GET_IX(p_obj->Key);                         // get addressed TPDO index
  nbr &= CANOPEN_TPDO_COMM_PARAM_IX_RANGE;                      // extract TPDO number out of index
  p_pdo = &p_node->TpdoTbl[nbr];                                // get addressed TPDO data structure
  if (nmt->State != CANOPEN_OPERATIONAL) {                      // see, if not in OPERATIONAL state
    return;                                                     // finish with success; no special handling
  }

  cycTime = (CPU_INT16U)(*(CPU_INT32U *)p_buf);                 // get new cycle time for event timer
  if (cycTime > 0) {                                            // see, if event timer is enabled
    cyc_time_us = cycTime * DEF_TIME_NBR_uS_PER_mS;

    if (cyc_time_us >= p_node->TmrPeriod) {
      cyc_time_us = cyc_time_us / p_node->TmrPeriod;
    } else {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      return;
    }
  }

  p_pdo->AsyncEventTmrTime = cyc_time_us;                       // set TPDO event timer.
  if (p_pdo->AsyncEventTmrId >= 0) {                            // see, if a timer is active
    CANopen_TmrDel(&p_pdo->NodePtr->Tmr,                        // shutdown PDO timer
                   p_pdo->AsyncEventTmrId,
                   p_err);
    RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
  if (p_pdo->AsyncInhibitTime > 0) {                            // see, if a timer is active
    CANopen_TmrDel(&p_pdo->NodePtr->Tmr,                        // shutdown PDO timer
                   p_pdo->AsyncInhibitTmrId,
                   p_err);
    RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  CANopen_DictLongRd(node_handle,                               // get TPDO identifier
                     CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_TPDO_COMM_PARAM_START + nbr, 1),
                     &cobid,
                     &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    CANOPEN_CTR_ERR_INC(p_pdo->NodePtr->CntErrPtr->Pdo.RdIdFailCtr);
    return;
  }

  if (((cobid & CANOPEN_TPDO_COMM_COB_ID_VALID) == 0)           // see, if TPDO is enabled
      && (p_pdo->AsyncEventTmrTime > 0)                         // and event timer is enabled
      && (nmt->State == CANOPEN_OPERATIONAL)) {                 // and node NMT mode is OPERATIONAL
                                                                // Create TPDO timer
    p_pdo->AsyncEventTmrId = CANopen_TmrCreate(&p_pdo->NodePtr->Tmr,
                                               p_pdo->AsyncEventTmrTime,
                                               0,
                                               CANopen_TPdoTmrEvent,
                                               (void *)p_pdo,
                                               &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      CANOPEN_CTR_ERR_INC(p_pdo->NodePtr->CntErrPtr->Pdo.PdoTmrCreateFailCtr);
      return;
    }
  }
}

/****************************************************************************************************//**
 *                                         CANopen_TPdoMapAdd()
 *
 * @brief    Adds signal to the given TPDO number into the link mapping table.
 *
 * @param    p_map   Pointer to start of link mapping table.
 *
 * @param    p_obj   Pointer to object entry.
 *
 * @param    nbr     Linked TPDO number.
 *******************************************************************************************************/
static void CANopen_TPdoMapAdd(CANOPEN_TPDO_LINK *p_map,
                               CANOPEN_OBJ       *p_obj,
                               CPU_INT16U        nbr)
{
  CPU_INT16U id;                                                // Local: loop counter through link p_map

  for (id = 0; id < CANOPEN_TPDO_MAX_MAP_QTY; id++) {           // loop through all link p_map entries
    if (p_map[id].ObjPtr == DEF_NULL) {                         // see, if link p_map entry is unused
      p_map[id].ObjPtr = p_obj;                                 // yes: set object pointer
      p_map[id].Nbr = nbr;                                      // yes: linked TPDO number
      break;                                                    // finished; exit function
    }
  }
}

/****************************************************************************************************//**
 *                                         CANopen_TPdoInvalidWr()
 *
 * @brief    Set a specific error code, if user tries to write to sub-index 0x04 of TPDO communication
 *           parameter. This sub-index is reserved.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_obj           Pointer to object entry.
 *
 * @param    p_buf           Pointer to data buffer.
 *
 * @param    size            Size of given data in buffer.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void CANopen_TPdoInvalidWr(CANOPEN_NODE_HANDLE node_handle,
                                  CANOPEN_OBJ         *p_obj,
                                  void                *p_buf,
                                  CPU_INT32U          size,
                                  RTOS_ERR            *p_err)
{
  CPU_INT32U sud_ix;

  PP_UNUSED_PARAM(node_handle);
  PP_UNUSED_PARAM(size);

  RTOS_ASSERT_DBG_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  sud_ix = CANOPEN_OBJ_GET_SUBIX(p_obj->Key);

  if (sud_ix == CANOPEN_TPDO_COMM_PARAM_SUB_IX_4_RESERVED) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_CANOPEN_SDO_ERR_SUB_IX);
  }
}

/****************************************************************************************************//**
 *                                         CANopen_TPdoInvalidRd()
 *
 * @brief    Set a specific error code, if user tries to read from sub-index 0x04 of TPDO communication
 *           parameter. This sub-index is reserved.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_obj           Pointer to object entry.
 *
 * @param    p_buf           Pointer to data buffer.
 *
 * @param    size            Size of given data in buffer.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void CANopen_TPdoInvalidRd(CANOPEN_NODE_HANDLE node_handle,
                                  CANOPEN_OBJ         *p_obj,
                                  void                *p_buf,
                                  CPU_INT32U          size,
                                  RTOS_ERR            *p_err)
{
  CPU_INT32U sud_ix;

  PP_UNUSED_PARAM(node_handle);
  PP_UNUSED_PARAM(size);

  RTOS_ASSERT_DBG_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  sud_ix = CANOPEN_OBJ_GET_SUBIX(p_obj->Key);

  if (sud_ix == CANOPEN_TPDO_COMM_PARAM_SUB_IX_4_RESERVED) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_CANOPEN_SDO_ERR_SUB_IX);
  }
}

#endif // CANOPEN_TPDO_MAX_QTY

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_CANOPEN_AVAIL
