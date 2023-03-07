/***************************************************************************//**
 * @file
 * @brief CANopen Network Management (Nmt) Service
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

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <canopen_cfg.h>
#include  <canopen/source/core/canopen_core_priv.h>
#include  <canopen/source/core/canopen_if_priv.h>
#include  <canopen/source/core/canopen_tmr_priv.h>
#include  <canopen/source/core/canopen_obj_priv.h>
#include  <canopen/source/nmt/canopen_nmt_priv.h>
#include  <canopen/source/objects/pdo/canopen_tpdo_priv.h>
#include  <canopen/source/objects/pdo/canopen_rpdo_priv.h>
#include  <canopen/source/objects/sdo/canopen_sdo_priv.h>
#include  <canopen/source/objects/sync/canopen_sync_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (CAN, CANOPEN)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_CANOPEN

#define  CANOPEN_NMT_CMD_START              1u
#define  CANOPEN_NMT_CMD_STOP               2u
#define  CANOPEN_NMT_CMD_ENTER_PRE_OP       128u
#define  CANOPEN_NMT_CMD_RESET_NODE         129u
#define  CANOPEN_NMT_CMD_RESET_COMM         130u

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          Allowed objects
 *
 * @note     (1) List of allowed objects based on a given operation state.
 *******************************************************************************************************/

static const CPU_INT08U CANopen_NmtStateObj[] = {
  (0),                                                          ///< Objects in invalid state

  (CANOPEN_BOOT_ALLOWED),                                       ///< Objects in initialization state

  (CANOPEN_SDO_ALLOWED                                          ///< Objects in pre-operational state
   | CANOPEN_SYNC_ALLOWED
   | CANOPEN_TIME_ALLOWED
   | CANOPEN_EMCY_ALLOWED
   | CANOPEN_NMT_ALLOWED),

  (CANOPEN_PDO_ALLOWED                                          ///< Objects in operational state
   | CANOPEN_SDO_ALLOWED
   | CANOPEN_SYNC_ALLOWED
   | CANOPEN_TIME_ALLOWED
   | CANOPEN_EMCY_ALLOWED
   | CANOPEN_NMT_ALLOWED),

  (CANOPEN_NMT_ALLOWED)                                         ///< Objects in stop state
};

/*
 ********************************************************************************************************
 *                                   Encoded NMT state machine state
 *******************************************************************************************************/

static const CPU_INT08U CANopen_NmtStateCode[] = {
  CANOPEN_NMT_STATE_INVALID,                                    ///< Encoding for invalid state
  CANOPEN_NMT_STATE_INIT_BOOT_UP,                               ///< Encoding for initialization state
  CANOPEN_NMT_STATE_PRE_OP,                                     ///< Encoding for pre-operational state
  CANOPEN_NMT_STATE_OP,                                         ///< Encoding for operational state
  CANOPEN_NMT_STATE_STOP                                        ///< Encoding for stopped state
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        CANopen_NmtReset()
 *
 * @brief    Resets the CANopen device with the given type.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    type            The requested NMT reset type:
 *                               - CANOPEN_RESET_NODE
 *                               - CANOPEN_RESET_COMM
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                            code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NULL_PTR
 *******************************************************************************************************/
void CANopen_NmtReset(CANOPEN_NODE_HANDLE node_handle,
                      CANOPEN_NMT_RESET   type,
                      RTOS_ERR            *p_err)
{
  CANOPEN_NMT  *p_nmt;
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CPU_BOOLEAN  send_bootup_msg = DEF_NO;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET(CANOPEN_NODE_HANDLE_IS_VALID(node_handle), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_nmt = &p_node->Nmt;

  if (p_nmt->State != CANOPEN_INIT) {                           // see, if not in initialization state
    CANopen_NmtStateSet(node_handle, CANOPEN_INIT, p_err);      // yes: set state to initialization state
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      CANOPEN_CTR_ERR_INC(p_nmt->NodePtr->CntErrPtr->Nmt.TransitionToInitStateFailCtr);
      return;
    }

    send_bootup_msg = DEF_YES;                                  // Bootup msg is mandatory at the end of INIT state.
  }
  //                                                               -------------------- RESET NODE --------------------
  if (type == CANOPEN_RESET_NODE) {
#if (CANOPEN_OBJ_PARAM_EN == DEF_ENABLED)
    CANopen_NodeParamLoad(node_handle, CANOPEN_RESET_NODE, p_err); // get all parameters within 2000h..9FFFh
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      CANOPEN_CTR_ERR_INC(p_nmt->NodePtr->CntErrPtr->Nmt.ParamLoadFailCtr);
      return;
    }

#endif
  }
  //                                                               --------------- RESET COMMUNICATION ----------------
  if (type <= CANOPEN_RESET_COMM) {
#if (CANOPEN_OBJ_PARAM_EN == DEF_ENABLED)
    //                                                             get all parameters within 1000h..1FFFh
    CANopen_NodeParamLoad(node_handle, CANOPEN_RESET_COMM, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      CANOPEN_CTR_ERR_INC(p_node->CntErrPtr->Nmt.ParamLoadFailCtr);
      return;
    }
#endif

    CANopen_TmrClr(&(p_node->Tmr));                             // clear all CANopen highspeed timers
    CANopen_NmtInit(p_nmt, p_node, p_err);                      // reset heartbeat timer
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      CANOPEN_CTR_ERR_INC(p_node->CntErrPtr->Nmt.InitFailCtr);
      return;
    }
    CANopen_SdoInit(p_node->Sdo, p_node, p_err);                // Initialize CANopen SDO tables
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      CANOPEN_CTR_ERR_INC(p_node->CntErrPtr->Nmt.SdoInitFailCtr);
      return;
    }

    CANopen_IfReset(&(p_node->If), p_err);                      // reset communication interface
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      CANOPEN_CTR_ERR_INC(p_node->CntErrPtr->Nmt.IF_ResetFailCtr);
      return;
    }

#if (CANOPEN_EMCY_MAX_ERR_QTY > 0)
    CANopen_EmcyReset(node_handle, p_err);                      // Reset CANopen EMCY tables
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      CANOPEN_CTR_ERR_INC(p_node->CntErrPtr->Nmt.EmcyResetFailCtr);
      return;
    }
#endif
#if (CANOPEN_SYNC_EN == DEF_ENABLED)
    CANopen_SyncInit(&(p_node->Sync), p_node, p_err);           // Initialize SYNC tables
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      CANOPEN_CTR_ERR_INC(p_node->CntErrPtr->Nmt.SyncInitFailCtr);
      return;
    }
#endif
    if (send_bootup_msg == DEF_YES) {                           // Send bootup message only if required.
      CANopen_NmtBootup(p_nmt, p_err);                          // Transmit boot-up message.
      CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE, p_node->CntErrPtr->Nmt.BootUpMsgFailCtr);
    }
  }
}

/****************************************************************************************************//**
 *                                        CANopen_NmtStateSet()
 *
 * @brief    Sets the requested CANopen NMT state machine state.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    state           The requested NMT state:
 *                               - CANOPEN_INIT
 *                               - CANOPEN_PREOP
 *                               - CANOPEN_OPERATIONAL
 *                               - CANOPEN_STOP
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NULL_PTR
 *******************************************************************************************************/
void CANopen_NmtStateSet(CANOPEN_NODE_HANDLE node_handle,
                         CANOPEN_NODE_STATE  state,
                         RTOS_ERR            *p_err)
{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CANOPEN_NMT  *p_nmt;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET(CANOPEN_NODE_HANDLE_IS_VALID(node_handle), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_nmt = &p_node->Nmt;

  if (state == CANOPEN_OPERATIONAL) {                           // see, if switching to OPERATIONAL
#if (CANOPEN_TPDO_MAX_QTY > 0)
    CANopen_TPdoInit(p_node->TpdoTbl, p_node, p_err);           // Initialize TPDO tables
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
#endif
#if (CANOPEN_RPDO_MAX_QTY > 0)
    CANopen_RPdoInit(p_node->RpdoTbl, p_node, p_err);           // Initialize RPDO tables
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
#endif
  }
  //                                                               see if state has changed
  if ((p_nmt->State != state)
      && (p_node->EventFnctsPtr != DEF_NULL)
      && (p_node->EventFnctsPtr->StateOnChange != DEF_NULL)) {
    //                                                             call state change callback function
    p_node->EventFnctsPtr->StateOnChange(node_handle, state);
  }

  p_nmt->State = state;                                         // set new NMT state
  p_nmt->CurStateObjAllowed = CANopen_NmtStateObj[state];       // set allowed objects for this state
}

/****************************************************************************************************//**
 *                                        CANopen_NmtStateGet()
 *
 * @brief    Returns the current CANopen NMT state machine state.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @return   NMT state of the parent node.
 *******************************************************************************************************/
CANOPEN_NODE_STATE CANopen_NmtStateGet(CANOPEN_NODE_HANDLE node_handle)
{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;

  RTOS_ASSERT_DBG(CANOPEN_NODE_HANDLE_IS_VALID(node_handle), RTOS_ERR_NULL_PTR,; );

  return (p_node->Nmt.State);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                         INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         CANopen_NmtInit()
 *
 * @brief    Initializes the CANopen NMT environment.
 *
 * @param    p_nmt       Pointer to NMT structure.
 *
 * @param    p_node      Pointer to parent node structure.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
void CANopen_NmtInit(CANOPEN_NMT  *p_nmt,
                     CANOPEN_NODE *p_node,
                     RTOS_ERR     *p_err)
{
  CANOPEN_NODE_HANDLE handle;

  RTOS_ASSERT_DBG_ERR_SET((p_nmt != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_node != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  CANOPEN_NODE_HANDLE_SET(handle, p_node);

  p_nmt->NodePtr = p_node;                                      // set reference to parent node
  CANopen_NmtStateSet(handle, CANOPEN_INIT, p_err);             // set initialization state
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CANopen_NmtHbProdInit(p_nmt, p_err);                          // initialize heartbeat producer
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CANopen_NmtHbConsInit(p_nmt, p_err);                          // initialize heartbeat consumer
}

/****************************************************************************************************//**
 *                                        CANopen_NmtBootup()
 *
 * @brief    Performs the bootup protocol to the configured CAN bus.
 *
 * @param    p_nmt      Pointer to NMT structure.
 *
 * @param    p_err      Error pointer.
 *
 * @note     (1) The bootup protocol is used to signal that a NMT slave has entered the node state
 *               PRE-OPERATIONAL after the state INITIALISING.
 *******************************************************************************************************/
void CANopen_NmtBootup(CANOPEN_NMT *p_nmt,
                       RTOS_ERR    *p_err)
{
  CANOPEN_NODE_HANDLE handle;
  CANOPEN_IF_FRM      frm;                                      // Local: boot-up CAN frame.

  CANOPEN_NODE_HANDLE_SET(handle, p_nmt->NodePtr);

  if (p_nmt->State == CANOPEN_INIT) {                           // see, if device is in INIT state.
    CANopen_NmtStateSet(handle, CANOPEN_PREOP, p_err);          // enter pre-operational state.
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
    //                                                             set bootup CAN identifier.
    CANOPEN_FRM_SET_COB_ID(&frm, CANOPEN_COB_ID_NMT_ERR_CTRL_START + p_nmt->NodePtr->NodeId);
    CANOPEN_FRM_SET_DLC(&frm, 1);                               // set fixed DLC (1 byte).
    CANOPEN_FRM_SET_BYTE(&frm, 0, 0);                           // set required data byte #0 to 0.

    CANopen_IfWr(&p_nmt->NodePtr->If, &frm, p_err);             // send bootup message.
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);               // Boot up msg only send in INIT state.
  }
}

/****************************************************************************************************//**
 *                                        CANopen_NmtCheck()
 *
 * @brief    Checks a received frame to be a NMT message.
 *
 * @param    p_nmt       Pointer to NMT structure.
 *
 * @param    p_frm       Pointer to received CAN frame.
 *
 * @param    p_err       Error pointer.
 *
 * @return    0, if an NMT frame is received.
 *           -1, otherwise.
 *
 * @note     (1) Even if the NMT command processing fails, the error is voluntarily ignored so that
 *               the node frame processing stops decoding the frame payload.
 *******************************************************************************************************/
CPU_INT16S CANopen_NmtCheck(CANOPEN_NMT    *p_nmt,
                            CANOPEN_IF_FRM *p_frm,
                            RTOS_ERR       *p_err)
{
  CANOPEN_NODE_HANDLE handle;
  CPU_INT16S          result = -1;                              // Local: function result

  if (p_frm->MsgId == 0) {                                      // check for NMT message
    result = 0;                                                 // success, regardless if we are addressed

    if ((p_frm->Data[1] == p_nmt->NodePtr->NodeId)              // see, if we are addressed (or broadcast)
        || (p_frm->Data[1] == 0)) {
      CANOPEN_NODE_HANDLE_SET(handle, p_nmt->NodePtr);

      //                                                           Process NMT payload (see Note #1).
      switch (p_frm->Data[0]) {                                 // select command specifier
        case CANOPEN_NMT_CMD_START:                             // Cmd: Start Remote Node
                                                                // If already in OPERATIONAL, no need to re-enter...
          if (p_nmt->State != CANOPEN_OPERATIONAL) {            // ...this state.
            CANopen_NmtStateSet(handle, CANOPEN_OPERATIONAL, p_err);
            CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE, p_nmt->NodePtr->CntErrPtr->Nmt.CmdStartProcessFailCtr);
          }
          break;

        case CANOPEN_NMT_CMD_STOP:                              // Cmd: Stop Remote Node
                                                                // If already in STOP, no need to re-enter...
          if (p_nmt->State != CANOPEN_STOP) {                   // ...this state.
            CANopen_NmtStateSet(handle, CANOPEN_STOP, p_err);
            CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE, p_nmt->NodePtr->CntErrPtr->Nmt.CmdStopProcessFailCtr);
          }
          break;

        case CANOPEN_NMT_CMD_ENTER_PRE_OP:                      // Cmd: Enter Pre-Operational Operation
                                                                // If already in PRE-OPERATIONAL, no need to re-enter...
          if (p_nmt->State != CANOPEN_PREOP) {                  // ...this state.
            CANopen_NmtStateSet(handle, CANOPEN_PREOP, p_err);
            CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE, p_nmt->NodePtr->CntErrPtr->Nmt.CmdEnterPreopProcessFailCtr);
          }
          break;

        case CANOPEN_NMT_CMD_RESET_NODE:                        // Cmd: Reset Node
          CANopen_NmtReset(handle, CANOPEN_RESET_NODE, p_err);
          CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE, p_nmt->NodePtr->CntErrPtr->Nmt.CmdResetNodeProcessFailCtr);
          break;

        case CANOPEN_NMT_CMD_RESET_COMM:                        // Cmd: Reset Communication
          CANopen_NmtReset(handle, CANOPEN_RESET_COMM, p_err);
          CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE, p_nmt->NodePtr->CntErrPtr->Nmt.CmdResetCommProcessFailCtr);
          break;

        default:                                                // Cmd: unknown
          break;
      }
    }
  }

  return (result);
}

/****************************************************************************************************//**
 *                                      CANopen_NmtNodeIdSet()
 *
 * @brief    Sets the requested CANopen Node-ID within the NMT module.
 *
 * @param    p_nmt       Pointer to NMT structure.
 *
 * @param    node_id     The requested NMT node ID.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
void CANopen_NmtNodeIdSet(CANOPEN_NMT *p_nmt,
                          CPU_INT08U  node_id,
                          RTOS_ERR    *p_err)
{
  CANOPEN_NODE_STATE state;                                     // Local: current active NMT state

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_nmt != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_nmt->NodePtr != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );
  RTOS_ASSERT_DBG_ERR_SET((node_id != 0u), *p_err, RTOS_ERR_INVALID_ARG,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  state = p_nmt->State;                                         // get current state
  if (state != CANOPEN_INIT) {                                  // see, if node is not in INIT state
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
  } else {                                                      // otherwise: during INIT state change is ok
    p_nmt->NodePtr->NodeId = node_id;                           // set new node ID
  }
}

/****************************************************************************************************//**
 *                                      CANopen_NmtNodeIdGet()
 *
 * @brief    This function returns the current CANopen Node-ID of the NMT module.
 *
 * @param    p_nmt       Pointer to NMT structure.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Node ID of the parent node.
 *******************************************************************************************************/
CPU_INT08U CANopen_NmtNodeIdGet(CANOPEN_NMT *p_nmt,
                                RTOS_ERR    *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);
  RTOS_ASSERT_DBG_ERR_SET((p_nmt != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);
  RTOS_ASSERT_DBG_ERR_SET((p_nmt->NodePtr != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (p_nmt->NodePtr->NodeId);
}

/****************************************************************************************************//**
 *                                      CANopen_NmtStateDecode()
 *
 * @brief    This function returns the CANopen state to the given heartbeat state encoding.
 *
 * @param    code    Heartbeat state.
 *
 * @return   Decoded NMT state.
 *******************************************************************************************************/
CANOPEN_NODE_STATE CANopen_NmtStateDecode(CPU_INT08U code)
{
  CANOPEN_NODE_STATE result = CANOPEN_INVALID;
  CPU_INT08U         id;

  for (id = 0; id < CANOPEN_STATE_QTY; id++) {
    if (CANopen_NmtStateCode[id] == code) {
      result = (CANOPEN_NODE_STATE)id;
    }
  }
  return (result);
}

/****************************************************************************************************//**
 *                                      CANopen_NmtStateEncode()
 *
 * @brief    This function returns the heartbeat state code for the given CANopen state.
 *
 * @param    state   CANopen state.
 *
 * @return   Encoded NMT state.
 *******************************************************************************************************/
CPU_INT08U CANopen_NmtStateEncode(CANOPEN_NODE_STATE state)
{
  CPU_INT08U result = CANopen_NmtStateCode[CANOPEN_INVALID];

  if (state < CANOPEN_STATE_QTY) {
    result = CANopen_NmtStateCode[state];
  }
  return (result);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_CANOPEN_AVAIL
