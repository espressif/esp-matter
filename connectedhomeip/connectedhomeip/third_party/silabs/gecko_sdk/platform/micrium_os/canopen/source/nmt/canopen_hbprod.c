/***************************************************************************//**
 * @file
 * @brief CANopen Nmt Error Control Service - Heartbeat Producer
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
#include  <common/include/lib_def.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <canopen/source/core/canopen_core_priv.h>
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

/********************************************************************************************************
 ********************************************************************************************************
 *                                     LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void CANopen_NmtHbProdSend(void *p_arg);

static void CANopen_NmtHbProdWr(CANOPEN_NODE_HANDLE node_handle,
                                CANOPEN_OBJ         *p_obj,
                                void                *p_buf,
                                CPU_INT32U          size,
                                RTOS_ERR            *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                   Object type: heartbeat producer
 *
 * @note     (1) This object type specializes the general handling of objects for the object directory
 *               entry 0x1017. This entries is designed to provide the heartbeat producer cycle time.
 *******************************************************************************************************/

const CANOPEN_OBJ_TYPE CANopen_ObjTypeHbProd = {
  .SizeCb = DEF_NULL,
  .CtrlCb = DEF_NULL,
  .RdCb = DEF_NULL,
  .WrCb = CANopen_NmtHbProdWr
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                          INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        CANopen_NmtHbProdInit()
 *
 * @brief    Initializes the CANopen heartbeat producer.
 *
 * @param    p_nmt     Pointer to NMT structure.
 *
 * @param    p_err     Error pointer.
 *******************************************************************************************************/
void CANopen_NmtHbProdInit(CANOPEN_NMT *p_nmt,
                           RTOS_ERR    *p_err)
{
  CANOPEN_NODE_HANDLE handle;
  CANOPEN_NODE        *p_node;
  CPU_INT16U          cyc_time_ms;
  CPU_INT32U          cyc_time_us;

  RTOS_ASSERT_DBG_ERR_SET((p_nmt != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_node = p_nmt->NodePtr;
  CANOPEN_NODE_HANDLE_SET(handle, p_node);

  CANopen_DictWordRd(handle,                                    // read heartbeat producer time
                     CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_PROD_HB_TIME, 0),
                     &cyc_time_ms,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_nmt->TmrId >= 0) {                                      // see, if an heartbeat timer is running
    CANopen_TmrDel(&p_node->Tmr,                                // delete heartbeat timer
                   p_nmt->TmrId,
                   p_err);
    RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  if (cyc_time_ms > 0) {                                        // if cycle time is valid
    cyc_time_us = cyc_time_ms * DEF_TIME_NBR_uS_PER_mS;

    if (cyc_time_us >= p_node->TmrPeriod) {
      cyc_time_us = cyc_time_us / p_node->TmrPeriod;
    } else {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      return;
    }

    p_nmt->TmrId = CANopen_TmrCreate(&p_node->Tmr,              // create heartbeat timer
                                     cyc_time_us,
                                     cyc_time_us,
                                     CANopen_NmtHbProdSend,
                                     p_nmt,
                                     p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  } else {                                                      // otherwise: heartbeat is disabled
    p_nmt->TmrId = CANOPEN_TMR_INVALID_ID;                      // set heartbeat timer to invalid value
  }
}

/****************************************************************************************************//**
 *                                        CANopen_NmtHbProdSend()
 *
 * @brief    Timer-driven callback that generates the heartbeat message and sends it to the configured
 *           CAN bus.
 *
 * @param    p_arg    Reference to NMT structure.
 *******************************************************************************************************/
static void CANopen_NmtHbProdSend(void *p_arg)
{
  CANOPEN_IF_FRM frm;                                           // Local: heartbeat CAN frame
  CANOPEN_NMT    *p_nmt;                                        // LocaL: reference to NMT strucutre
  CPU_INT08U     state;                                         // Local: NMT state in heartbeat encoding
  RTOS_ERR       local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_nmt = (CANOPEN_NMT *)p_arg;                                 // get reference to the NMT structure
                                                                // see, if NMT messages are not allowed
  if ((p_nmt->CurStateObjAllowed & CANOPEN_NMT_ALLOWED) == 0) {
    return;                                                     // exit function without heartbeat message
  }

  state = CANopen_NmtStateEncode(p_nmt->State);                 // encode current NMT state
                                                                // set heartbeat CAN identifier
  CANOPEN_FRM_SET_COB_ID(&frm, CANOPEN_COB_ID_NMT_ERR_CTRL_START + p_nmt->NodePtr->NodeId);
  CANOPEN_FRM_SET_DLC(&frm, 1);                                 // set fixed DLC (1 byte)
  CANOPEN_FRM_SET_BYTE(&frm, state, 0);                         // set data byte #0 to NMT state encoding

  CANopen_IfWr(&p_nmt->NodePtr->If, &frm, &local_err);          // send heartbeat message
                                                                // possible error registered in node
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        CANopen_NmtHbProdWr()
 *
 * @brief    Writes heartbeat producer time.
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
static void CANopen_NmtHbProdWr(CANOPEN_NODE_HANDLE node_handle,
                                CANOPEN_OBJ         *p_obj,
                                void                *p_buf,
                                CPU_INT32U          size,
                                RTOS_ERR            *p_err)
{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CPU_INT16U   cyc_time_ms;
  CPU_INT32U   cyc_time_us;

  PP_UNUSED_PARAM(size);

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((CANOPEN_OBJ_GET_IX(p_obj->Key) == CANOPEN_COMM_PROFILE_OBJ_IX_PROD_HB_TIME),
                          *p_err, RTOS_ERR_CANOPEN_INVALID_ENTRY,; );

  cyc_time_ms = (CPU_INT16U)(*(CPU_INT32U *)p_buf);             // get cycle time from written value

  if (p_node->Nmt.TmrId >= 0) {                                 // if timer active
    CANopen_TmrDel(&p_node->Tmr,                                // shutdown heartbeat timer
                   p_node->Nmt.TmrId,
                   p_err);
    RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    p_node->Nmt.TmrId = CANOPEN_TMR_INVALID_ID;                 // clear timer identifier
  }

  if (cyc_time_ms > 0u) {                                       // see, if new cycle time is valid
    cyc_time_us = cyc_time_ms * DEF_TIME_NBR_uS_PER_mS;

    if (cyc_time_us >= p_node->TmrPeriod) {
      cyc_time_us = cyc_time_us / p_node->TmrPeriod;
    } else {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      return;
    }

    p_node->Nmt.TmrId = CANopen_TmrCreate(&p_node->Tmr,         // create heartbeat timer
                                          cyc_time_us,
                                          cyc_time_us,
                                          CANopen_NmtHbProdSend,
                                          &p_node->Nmt,
                                          p_err);
    RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  } else {                                                      // otherwise: heartbeat is disabled
    p_node->Nmt.TmrId = CANOPEN_TMR_INVALID_ID;                 // set heartbeat timer to invalid value
  }

  CANopen_ObjDirectWr(p_obj,                                    // write value to object data element
                      (void *)&cyc_time_ms,
                      CANOPEN_OBJ_WORD,
                      p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_CANOPEN_AVAIL
