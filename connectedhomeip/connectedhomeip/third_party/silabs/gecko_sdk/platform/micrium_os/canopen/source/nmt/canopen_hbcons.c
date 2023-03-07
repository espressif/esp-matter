/***************************************************************************//**
 * @file
 * @brief CANopen Nmt Error Control Service - Heartbeat Consumer
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
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/kal/kal_priv.h>

#include  <canopen/include/canopen_nmt.h>
#include  <canopen/source/core/canopen_core_priv.h>
#include  <canopen/source/core/canopen_dict_priv.h>
#include  <canopen/source/core/canopen_if_priv.h>
#include  <canopen/source/core/canopen_obj_priv.h>
#include  <canopen/source/core/canopen_tmr_priv.h>
#include  <canopen/source/nmt/canopen_nmt_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (CAN, CANOPEN)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_CANOPEN

#define  CANOPEN_HB_CONS_MISSED_EVENT_CNT_MAX       0xFF
#define  CANOPEN_HB_CONS_TIME_BIT_SHIFT             16u

/********************************************************************************************************
 ********************************************************************************************************
 *                                     LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void CANopen_NmtHbConsMonitor(void *p_arg);

static void CANopen_NmtHbConsWr(CANOPEN_NODE_HANDLE node_handle,
                                CANOPEN_OBJ         *p_obj,
                                void                *p_buf,
                                CPU_INT32U          size,
                                RTOS_ERR            *p_err);

static void CANopen_NmtHbConsRd(CANOPEN_NODE_HANDLE node_handle,
                                CANOPEN_OBJ         *p_obj,
                                void                *p_buf,
                                CPU_INT32U          len,
                                RTOS_ERR            *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                   Object type: heartbeat consumer
 *
 * @note     (1) This object type specializes the general handling of objects for the object directory
 *               entry 0x1016. This entries is designed to provide the heartbeat consumer monitor time.
 *******************************************************************************************************/

const CANOPEN_OBJ_TYPE CANopen_ObjTypeHbCons = {
  .SizeCb = DEF_NULL,
  .CtrlCb = DEF_NULL,
  .RdCb = CANopen_NmtHbConsRd,
  .WrCb = CANopen_NmtHbConsWr
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                    CANopen_NmtHbConsEventsGet()
 *
 * @brief    Gets the number of missed heartbeat events.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    node_id         node ID of monitored node (or 0 for master node).
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NULL_PTR
 *
 * @return   The number of missed heartbeat events for given node ID. @n
 *           < 0, if error detected (e.g. node ID is not monitored).
 *******************************************************************************************************/
CPU_INT16S CANopen_NmtHbConsEventsGet(CANOPEN_NODE_HANDLE node_handle,
                                      CPU_INT08U          node_id,
                                      RTOS_ERR            *p_err)
{
  CANOPEN_NODE   *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CANOPEN_NMT    *p_nmt;
  CPU_INT16S     result = -1;
  CANOPEN_HBCONS *p_hbc;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, -1);

  RTOS_ASSERT_DBG_ERR_SET(CANOPEN_NODE_HANDLE_IS_VALID(node_handle), *p_err, RTOS_ERR_NULL_PTR, -1);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_nmt = &p_node->Nmt;

  p_hbc = p_nmt->HbConsPtr;                                     // get first element in heartbeat chain
  while (p_hbc != DEF_NULL) {                                   // loop through active heartbeat chain
    if (node_id == p_hbc->NodeId) {                             // see, if node ID matches consumer
      result = (CPU_INT16S)p_hbc->MissedEventCnt;               // yes: add event of given node ID
      p_hbc->MissedEventCnt = 0u;                               // clear event counter
    }
    p_hbc = p_hbc->NextPtr;                                     // switch to next heartbeat consumer
  }

  return (result);                                              // return function result
}

/****************************************************************************************************//**
 *                                   CANopen_NmtHbConsLastStateGet()
 *
 * @brief    Returns the last received heartbeat state of a given node.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    node_id         Node ID of monitored node.
 *
 * @param    p_err    Pointer to the variable that will receive one of the following error code(s)
 *                    from this function:
 *                        - RTOS_ERR_NONE
 *                        - RTOS_ERR_NULL_PTR
 *
 * @return   Last detected node state for given node ID:
 *               - CANOPEN_INVALID
 *               - CANOPEN_INIT
 *               - CANOPEN_PREOP
 *               - CANOPEN_OPERATIONAL
 *               - CANOPEN_STOP
 *******************************************************************************************************/
CANOPEN_NODE_STATE CANopen_NmtHbConsLastStateGet(CANOPEN_NODE_HANDLE node_handle,
                                                 CPU_INT08U          node_id,
                                                 RTOS_ERR            *p_err)
{
  CANOPEN_NODE       *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CANOPEN_NMT        *p_nmt;
  CANOPEN_NODE_STATE result = CANOPEN_INVALID;                  // Local: function result
  CANOPEN_HBCONS     *p_hbc;                                    // Local: heartbeat consumer

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, CANOPEN_INVALID);

  RTOS_ASSERT_DBG_ERR_SET(CANOPEN_NODE_HANDLE_IS_VALID(node_handle), *p_err, RTOS_ERR_NULL_PTR, CANOPEN_INVALID);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_nmt = &p_node->Nmt;

  p_hbc = p_nmt->HbConsPtr;                                     // get first element in heartbeat chain
  while (p_hbc != DEF_NULL) {                                   // loop through active heartbeat chain
    if (node_id == p_hbc->NodeId) {                             // see, if node ID matches consumer
      result = p_hbc->State;                                    // yes: return received state
    }
    p_hbc = p_hbc->NextPtr;                                     // switch to next heartbeat consumer
  }
  return (result);                                              // return function result
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                         INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        CANopen_NmtHbConsInit()
 *
 * @brief    Initializes the CANopen heartbeat consumer.
 *
 * @param    p_nmt      Pointer to NMT structure.
 *
 * @param    p_err      Error pointer.
 *******************************************************************************************************/
void CANopen_NmtHbConsInit(CANOPEN_NMT *p_nmt,
                           RTOS_ERR    *p_err)
{
  CPU_INT08U     num;                                           // Local: number of heartbeat consumer
  CANOPEN_OBJ    *obj;
  CANOPEN_HBCONS *hbc;
  CANOPEN_NODE   *node;
  RTOS_ERR       local_err;

  RTOS_ASSERT_DBG_ERR_SET((p_nmt != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_nmt->HbConsPtr = DEF_NULL;                                  // reset used heartbeat consumer chain

  node = p_nmt->NodePtr;
  obj = CANopen_DictFind(&node->Dict,
                         CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_CONS_HB_TIME, 0),
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (!CANOPEN_OBJ_IS_RD_ONLY(obj->Key)) {
    KAL_LockAcquire(node->ObjLock,
                    KAL_OPT_PEND_NONE,
                    node->LockTimeoutMs,
                    p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR_CONTEXT_REFRESH(*p_err);
      return;
    }
  }

  CANopen_ObjValRd(node, obj, &num, CANOPEN_OBJ_BYTE, 0, p_err); // read number of heartbeat consumers

  if (!CANOPEN_OBJ_IS_RD_ONLY(obj->Key)) {
    KAL_LockRelease(node->ObjLock,
                    &local_err);
    RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
  //                                                               see, if value is allowed
  if ((RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)
      || (num < 1)) {
    return;
  }

  while (num > 0) {
    obj = CANopen_DictFind(&node->Dict,
                           CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_CONS_HB_TIME, num),
                           p_err);
    if (obj == DEF_NULL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;
    }
    hbc = (CANOPEN_HBCONS *)obj->Data;
    if (hbc == DEF_NULL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_CANOPEN_INVALID_ENTRY);     // if entry exists, the value must be valid
      break;
    }

    if (!CANOPEN_OBJ_IS_RD_ONLY(obj->Key)) {
      KAL_LockAcquire(node->ObjLock,
                      KAL_OPT_PEND_NONE,
                      KAL_TIMEOUT_INFINITE,
                      p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        RTOS_ERR_CONTEXT_REFRESH(*p_err);
        return;
      }
    }

    CANopen_NmtHbConsActivate(p_nmt,                            // try to activate heartbeat consumer
                              hbc,
                              hbc->TimeMs,
                              hbc->NodeId,
                              p_err);

    if (!CANOPEN_OBJ_IS_RD_ONLY(obj->Key)) {
      KAL_LockRelease(node->ObjLock,
                      &local_err);
      RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }

    num--;
  }
}

/****************************************************************************************************//**
 *                                      CANopen_NmtHbConsActivate()
 *
 * @brief    Activates a single heartbeat consumer.
 *
 * @param    p_nmt    Pointer to NMT structure.
 *
 * @param    p_hbc    Pointer to heartbeat consumer structure.
 *
 * @param    time     Heartbeat time, in ms.
 *
 * @param    node_id  Node ID in which to activate the heartbeat consumer.
 *
 * @param    p_err    Error pointer.
 *******************************************************************************************************/
void CANopen_NmtHbConsActivate(CANOPEN_NMT    *p_nmt,
                               CANOPEN_HBCONS *p_hbc,
                               CPU_INT16U     time,
                               CPU_INT08U     node_id,
                               RTOS_ERR       *p_err)
{
  CANOPEN_HBCONS *act;
  CANOPEN_HBCONS *prev;
  CANOPEN_HBCONS *found = DEF_NULL;

  prev = DEF_NULL;
  act = p_nmt->HbConsPtr;                                       // get root of heartbeat consumer chain
  while (act != DEF_NULL) {                                     // loop through all active consumers
    if (act->NodeId == node_id) {                               // see, if node ID is already active
      found = act;                                              // already in active list
      break;
    }
    prev = act;                                                 // store previous active consumer
    act = act->NextPtr;                                         // switch to next consumer
  }

  if (found != DEF_NULL) {                                      // see, if consumer is in active chain
    if (time > 0u) {                                            // see, if we want to enable/change time
      RTOS_ERR_SET(*p_err, RTOS_ERR_CANOPEN_PARAM_INCOMP);      // not allowed to change during active
    } else {                                                    // otherwise: we want disable consumer
      if (p_hbc->TmrId >= 0) {                                  // see, if monitor is already active
                                                                // abort monitor
        CANopen_TmrDel(&p_nmt->NodePtr->Tmr,
                       p_hbc->TmrId,
                       p_err);
        RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }
      p_hbc->TimeMs = time;
      p_hbc->NodeId = node_id;
      p_hbc->TmrId = CANOPEN_TMR_INVALID_ID;                    // set monitor timer to invalid value
      p_hbc->MissedEventCnt = 0u;                               // reset event counter
      p_hbc->State = CANOPEN_INVALID;                           // reset state to 'invalid state'
      if (prev == DEF_NULL) {                                   // see, if consumer is first in chain
        p_nmt->HbConsPtr = p_hbc->NextPtr;                      // yes: remove first consumer in chain
      } else {                                                  // otherwise: consumer is within list
        prev->NextPtr = p_hbc->NextPtr;                         // remove consumer within chain
      }
      p_hbc->NextPtr = DEF_NULL;                                // clear next pointer in disabled consumer
    }
  } else {                                                      // otherwise: consumer not in active chain
    p_hbc->TimeMs = time;
    p_hbc->NodeId = node_id;
    p_hbc->TmrId = CANOPEN_TMR_INVALID_ID;                      // set monitor timer to invalid value
    p_hbc->MissedEventCnt = 0u;                                 // reset event counter
    p_hbc->State = CANOPEN_INVALID;                             // reset state to 'invalid state'

    if (time > 0u) {                                            // see, if consumer is enabled
      p_hbc->NextPtr = p_nmt->HbConsPtr;                        // add in consumer chain
      p_nmt->HbConsPtr = p_hbc;
    } else {                                                    // otherwise: time changed while disabled
      p_hbc->NextPtr = DEF_NULL;                                // clear next pointer in disabled consumer
    }
  }
}

/****************************************************************************************************//**
 *                                       CANopen_NmtHbConsCheck()
 *
 * @brief    Checks a received CAN frame against all heartbeat consumers and increments the receive
 *           counter in the specific consumer monitor.
 *
 * @param    p_nmt   Pointer to NMT structure.
 *
 * @param    p_frm   Pointer to CAN frame structure.
 *
 * @param    p_err   Error pointer.
 *
 * @return   Consumer node ID of matching/consumed heartbeat message.
 *
 * @note     (1) Node ID return value of 0 for heartbeat consuming is allowed, because the CANopen master
 *               heartbeat may come with this node ID.
 *******************************************************************************************************/
CPU_INT16S CANopen_NmtHbConsCheck(CANOPEN_NMT    *p_nmt,
                                  CANOPEN_IF_FRM *p_frm,
                                  RTOS_ERR       *p_err)
{
  CPU_INT16S         result = -1;                               // Local: function result
  CPU_INT32U         cob_id;                                    // Local: CAN identifier
  CPU_INT08U         node_id;                                   // Local: heartbeat producer node-ID
  CANOPEN_NODE_STATE state;                                     // Local: received state
  CANOPEN_HBCONS     *p_hbc;                                    // Local: active heartbeat consumer chain
  CPU_INT32U         cyc_time_us;

  cob_id = p_frm->MsgId;                                        // get COBID of the CAN frame
  p_hbc = p_nmt->HbConsPtr;                                     // get first heartbeat consumer element
  if (p_hbc == DEF_NULL) {                                      // see, if no heartbeat consumer active
    return (result);                                            // yes: finished
  }
  //                                                               see, if CAN frame is a heartbeat frame
  if ((cob_id >= CANOPEN_COB_ID_NMT_ERR_CTRL_START)
      && (cob_id <= CANOPEN_COB_ID_NMT_ERR_CTRL_START + CANOPEN_COB_ID_NMT_ERR_CTRL_RANGE)) {
    //                                                             yes: calculate node-ID out of COBID
    node_id = (CPU_INT08U)(cob_id - CANOPEN_COB_ID_NMT_ERR_CTRL_START);
  } else {                                                      // otherwise: no heartbeat message
    return (result);                                            // finished.
  }
  while (p_hbc != DEF_NULL) {                                   // loop through all active consumers
    if (p_hbc->NodeId != node_id) {                             // see, if node-ID is not for this consumer
      p_hbc = p_hbc->NextPtr;                                   // yes: switch to next consumer
    } else {                                                    // otherwise: message is for this consumer
      if (p_hbc->TmrId >= 0) {                                  // see, if consumer is waiting
        CANopen_TmrDel(&p_nmt->NodePtr->Tmr,
                       p_hbc->TmrId,
                       p_err);
        RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }

      cyc_time_us = p_hbc->TimeMs * DEF_TIME_NBR_uS_PER_mS;

      if (cyc_time_us >= p_nmt->NodePtr->TmrPeriod) {
        cyc_time_us = cyc_time_us / p_nmt->NodePtr->TmrPeriod;
      } else {
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
        return (-1);
      }
      //                                                           start the consumer monitor timeout
      p_hbc->TmrId = CANopen_TmrCreate(&p_nmt->NodePtr->Tmr,
                                       cyc_time_us,
                                       0,
                                       CANopen_NmtHbConsMonitor,
                                       p_nmt->NodePtr,
                                       p_err);
      RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

      state = CANopen_NmtStateDecode(p_frm->Data[0]);           // decode received state

      if ((p_hbc->State != state)
          && (p_nmt->NodePtr->EventFnctsPtr != DEF_NULL)
          && (p_nmt->NodePtr->EventFnctsPtr->HbcOnChange != DEF_NULL)) {
        CANOPEN_NODE_HANDLE handle;

        CANOPEN_NODE_HANDLE_SET(handle, p_nmt->NodePtr);

        p_nmt->NodePtr->EventFnctsPtr->HbcOnChange(handle,
                                                   p_hbc->NodeId,
                                                   state);
      }

      p_hbc->State = state;                                     // set received state
      result = (CPU_INT16S)p_hbc->NodeId;                       // 'consume' given CAN frame
      break;
    }
  }
  return (result);                                              // return function result
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                      CANopen_NmtHbConsMonitor()
 *
 * @brief    Timer-driven callback function which checks that at least one received heartbeat
 *           is detected for this heartbeat consumer.
 *
 * @param    p_arg    Pointer to heartbeat consumer structure.
 *******************************************************************************************************/
static void CANopen_NmtHbConsMonitor(void *p_arg)
{
  CANOPEN_NODE   *node;                                         // Local: reference to parent node
  CANOPEN_HBCONS *hbc;                                          // Local: reference to consumer structure
  CPU_INT32U     cyc_time_us;
  RTOS_ERR       local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  node = (CANOPEN_NODE *)p_arg;                                 // get parent node for this consumer
  hbc = node->Nmt.HbConsPtr;                                    // get reference to the consumer structure

  cyc_time_us = hbc->TimeMs * DEF_TIME_NBR_uS_PER_mS;

  if (cyc_time_us >= node->TmrPeriod) {
    cyc_time_us = cyc_time_us / node->TmrPeriod;
  } else {
    return;
  }
  //                                                               start next consumer monitor timeout
  hbc->TmrId = CANopen_TmrCreate(&node->Tmr,
                                 cyc_time_us,
                                 0,
                                 CANopen_NmtHbConsMonitor,
                                 hbc,
                                 &local_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  //                                                               increment event counter up to 0xFF
  if (hbc->MissedEventCnt < CANOPEN_HB_CONS_MISSED_EVENT_CNT_MAX) {
    hbc->MissedEventCnt++;
  }

  if ((node->EventFnctsPtr != DEF_NULL)
      && (node->EventFnctsPtr->HbcOnEvent != DEF_NULL)) {
    CANOPEN_NODE_HANDLE handle;

    CANOPEN_NODE_HANDLE_SET(handle, node);
    //                                                             call heartbeat consumer event callback
    node->EventFnctsPtr->HbcOnEvent(handle,
                                    hbc->NodeId);
  }
}

/****************************************************************************************************//**
 *                                        CANopen_NmtHbConsWr()
 *
 * @brief    Writes the heartbeat consumer configuration.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_obj           Pointer to accessed object entry.
 *
 * @param    p_buf           Pointer to the value to write.
 *
 * @param    size            Size of the value to write.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void CANopen_NmtHbConsWr(CANOPEN_NODE_HANDLE node_handle,
                                CANOPEN_OBJ         *p_obj,
                                void                *p_buf,
                                CPU_INT32U          size,
                                RTOS_ERR            *p_err)
{
  CANOPEN_HBCONS *p_hbc;
  CANOPEN_NODE   *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CPU_INT32U     val = 0u;                                      // Local: encoded heartbeat consumer value
  CPU_INT16U     time;                                          // Local: time to write
  CPU_INT08U     node_id;                                       // Local: nodeid to monitor

  p_hbc = (CANOPEN_HBCONS *)p_obj->Data;
  if (p_hbc == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }
  if (size != CANOPEN_OBJ_LONG) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }
  val = *((CPU_INT32U *)p_buf);
  time = (CPU_INT16U)val;
  node_id = (CPU_INT08U)(val >> CANOPEN_HB_CONS_TIME_BIT_SHIFT);

  if (!CANOPEN_OBJ_IS_RD_ONLY(p_obj->Key)) {
    KAL_LockAcquire(p_node->ObjLock,
                    KAL_OPT_PEND_NONE,
                    p_node->LockTimeoutMs,
                    p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR_CONTEXT_REFRESH(*p_err);
      return;
    }
  }

  CANopen_NmtHbConsActivate(&p_node->Nmt,                       // try to activate heartbeat consumer
                            p_hbc,
                            time,
                            node_id,
                            p_err);
  CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE,
                           p_node->CntErrPtr->Nmt.HbConsActFailCtr);

  if (!CANOPEN_OBJ_IS_RD_ONLY(p_obj->Key)) {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    KAL_LockRelease(p_node->ObjLock,
                    &local_err);
    RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
}

/****************************************************************************************************//**
 *                                        CANopen_NmtHbConsRd()
 *
 * @brief    Reads the heartbeat consumer configuration.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_obj           Pointer to accessed object entry.
 *
 * @param    p_buf           Pointer to receive buffer.
 *
 * @param    len             Length of the value to read.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void CANopen_NmtHbConsRd(CANOPEN_NODE_HANDLE node_handle,
                                CANOPEN_OBJ         *p_obj,
                                void                *p_buf,
                                CPU_INT32U          len,
                                RTOS_ERR            *p_err)
{
  CANOPEN_HBCONS *p_hbc;                                        // Local: ptr to the HB consumer structure
  CPU_INT32U     value;                                         // Local: encoded value for this entry
  CPU_INT08U     *src;                                          // Local: pointer within RAM domain
  CPU_INT08U     *dst;                                          // Local: pointer to buffer
  CPU_INT32U     num;                                           // Local: remaining bytes in domain

  PP_UNUSED_PARAM(node_handle);
  PP_UNUSED_PARAM(p_err);

  p_hbc = (CANOPEN_HBCONS *)(p_obj->Data);                      // get HB consumer structure
  value = (CPU_INT32U)(p_hbc->TimeMs);                          // set HB consumer time in value
                                                                // add HB consumer node
  value |= ((CPU_INT32U)(p_hbc->NodeId)) << CANOPEN_HB_CONS_TIME_BIT_SHIFT;
  num = CANOPEN_OBJ_LONG;                                       // set size of object entry
  src = (CPU_INT08U *)&value;                                   // set encoded value as source
  dst = (CPU_INT08U *)p_buf;                                    // get buffer pointer
  while ((len > 0u) && (num > 0u)) {                            // loop through remaining domain
    *dst = *src;                                                // copy domain into buffer location
    src++;                                                      // switch to next domain byte
    dst++;                                                      // switch to next buffer location
    len--;                                                      // decrement remaining space in buffer
    num--;                                                      // update remaining bytes of value
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_CANOPEN_AVAIL
