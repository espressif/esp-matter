/***************************************************************************//**
 * @file
 * @brief CANopen CAN Interface Abstraction
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
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/include/rtos_err.h>

#include  <canopen_cfg.h>
#include  <canopen/include/canopen_core.h>
#include  <canopen/source/core/canopen_core_priv.h>
#include  <canopen/source/core/canopen_if_priv.h>
#include  <canopen/source/core/canopen_obj_priv.h>
#include  <canopen/include/canopen_if.h>
#include  <can/include/can_bus.h>
#include  <can/source/can_bus_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (CAN, CANOPEN)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_CANOPEN

#define  CANOPEN_IF_PROPAGATION_TIME_SEGMENT    8u
#define  CANOPEN_IF_PHASE_BUFFER_SEGMENT_1      8u
#define  CANOPEN_IF_PHASE_BUFFER_SEGMENT_2      2u
#define  CANOPEN_IF_SYNCHRONISATION_JUMP_WIDTH  1u

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void CANopen_IfRx(void       *p_data,
                         CPU_INT08U int_nbr);

static void CANopen_IfTmrOvf(void);

static void CANopen_IfFilterSet(CANOPEN_NODE *p_node,
                                RTOS_ERR     *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                            GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                            CANopen_IfInit()
 *
 * @brief    This function initializes and enables the CAN bus interface.
 *
 * @param    p_if      Pointer to the interface structure.
 *
 * @param    p_node    Pointer to the parent node.
 *
 * @param    p_err     Pointer to the variable that will receive one of the following
 *                     error code(s) from this function:
 *                         - RTOS_ERR_NONE
 *                         - RTOS_ERR_NULL_PTR
 *******************************************************************************************************/
void CANopen_IfInit(CANOPEN_IF   *p_if,
                    CANOPEN_NODE *p_node,
                    RTOS_ERR     *p_err)
{
  RTOS_ASSERT_DBG_ERR_SET((p_node != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_if != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_if->NodePtr = p_node;                                       // store pointer to parent node

  CanBus_Init(p_err);                                           // Initialise CAN bus interface
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_if->Bus = CanBus_Add(p_node->NamePtr,
                         (void *)p_node,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CanBus_RxSubscribe(p_if->Bus, CANopen_IfRx, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/****************************************************************************************************//**
 *                                             CANopen_IfRd()
 *
 * @brief    This function waits for a CAN frame on the interface without timeout. If a CAN frame
 *           is received, the given p_frm will be filled with the received data.
 *
 * @param    p_if     Pointer to the interface structure
 *
 * @param    p_frm    Pointer to the receive frame buffer
 *
 * @param    p_err    Pointer to the variable that will receive one of the following
 *                    error code(s) from this function:
 *                        - RTOS_ERR_NONE
 *                        - RTOS_ERR_NULL_PTR
 *******************************************************************************************************/
void CANopen_IfRd(CANOPEN_IF     *p_if,
                  CANOPEN_IF_FRM *p_frm,
                  RTOS_ERR       *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_if != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_frm != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_frm->MsgNbr = p_if->IntNbr;

  CanBus_Rd(p_if->Bus,
            &p_frm->MsgNbr,
            &p_frm->MsgId,
            p_frm->Data,
            &p_frm->DLC,
            p_err);
}

/****************************************************************************************************//**
 *                                             CANopen_IfWr()
 *
 * @brief    This function sends the given CAN frame on the interface without delay.
 *
 * @param    p_if     Pointer to the interface structure.
 *
 * @param    p_frm    Pointer to the transmit frame buffer.
 *
 * @param    p_err    Pointer to the variable that will receive one of the following
 *                    error code(s) from this function:
 *                        - RTOS_ERR_NONE
 *                        - RTOS_ERR_NULL_PTR
 *******************************************************************************************************/
void CANopen_IfWr(CANOPEN_IF     *p_if,
                  CANOPEN_IF_FRM *p_frm,
                  RTOS_ERR       *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_if != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_frm != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_frm->MsgNbr = CANOPEN_CAN_ID_TX_FILTER;

  CanBus_Wr(p_if->Bus,
            DEF_NULL,
            p_frm->MsgNbr,
            p_frm->MsgId,
            p_frm->Data,
            p_frm->DLC,
            p_err);
}

/****************************************************************************************************//**
 *                                           CANopen_IfStart()
 *
 * @brief    Start the CAN interface.
 *
 * @param    p_node        Pointer to the CANopen node object.
 *
 * @param    p_err         Pointer to the variable that will receive one of the following error code(s)
 *                         from this function:
 *                             - RTOS_ERR_NONE
 *                             - RTOS_ERR_INVALID_ARG
 *                             - RTOS_ERR_DEV_ALLOC
 *                             - RTOS_ERR_INVALID_HANDLE
 *******************************************************************************************************/
void CANopen_IfStart(CANOPEN_NODE *p_node,
                     RTOS_ERR     *p_err)
{
  CANOPEN_IF *p_if = &p_node->If;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_node != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  CanBus_Start(p_if->Bus, p_node->TmrPeriod, p_node->Baudrate, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CanBus_BitTiming(p_if->Bus,
                   p_node->Baudrate,
                   CANOPEN_IF_PROPAGATION_TIME_SEGMENT,
                   CANOPEN_IF_PHASE_BUFFER_SEGMENT_1,
                   CANOPEN_IF_PHASE_BUFFER_SEGMENT_2,
                   CANOPEN_IF_SYNCHRONISATION_JUMP_WIDTH,
                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CANopen_IfFilterSet(p_node,
                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/****************************************************************************************************//**
 *                                           CANopen_IfReset()
 *
 * @brief    This function resets the CAN interface and flushes all already buffered CAN frames.
 *
 * @param    p_if    Pointer to the interface structure.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NULL_PTR
 *******************************************************************************************************/
void CANopen_IfReset(CANOPEN_IF *p_if,
                     RTOS_ERR   *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_if != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  CanBus_MsgReset(p_if->Bus, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CANopen_IfFilterSet(p_if->NodePtr,
                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/****************************************************************************************************//**
 *                                           CANopen_IfClose()
 *
 * @brief    This function closes the CAN interface.
 *
 * @param    p_if    Pointer to the interface structure.
 *
 * @param    p_err    Pointer to the variable that will receive one of the following
 *                    error code(s) from this function:
 *                        - RTOS_ERR_NONE
 *                        - RTOS_ERR_NULL_PTR
 *******************************************************************************************************/
void CANopen_IfClose(CANOPEN_IF *p_if,
                     RTOS_ERR   *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_if != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  CanBus_MsgReset(p_if->Bus, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CanBus_Stop(p_if->Bus, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/****************************************************************************************************//**
 *                                          CANopen_IfTmrCbReg()
 *
 * @brief    Register a timer callback.
 *
 * @param    p_err    Pointer to the variable that will receive one of the following
 *                    error code(s) from this function:
 *                        - RTOS_ERR_NONE
 *                        - RTOS_ERR_INVALID_HANDLE
 *                        - RTOS_ERR_INVALID_ARG
 *******************************************************************************************************/
void CANopen_IfTmrCbReg(RTOS_ERR *p_err)
{
  CanBus_TmrSubscribe(&CANopen_IfTmrOvf,
                      p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                             CANopen_IfRx()
 *
 * @brief    This function is called for each CAN frame.
 *
 * @param    p_data      Pointer to private data.
 *
 * @param    int_nbr     CAN Bus interrupt number.
 *******************************************************************************************************/
static void CANopen_IfRx(void       *p_data,
                         CPU_INT08U int_nbr)
{
  CANOPEN_EVENT event;

  event.NodePtr = (CANOPEN_NODE *)p_data;
  event.IntNbr = int_nbr;
  event.ErrCodeIx = 0u;
  event.VendorErrCodePtr = DEF_NULL;
  event.EventType = CANOPEN_EVENT_RX;

  CANopen_SvcTaskWakeUp(&event);
}

/****************************************************************************************************//**
 *                                           CANopen_IfTmrOvf()
 *
 * @brief    Timer callback. This functions is called for each timer interrupt.
 *******************************************************************************************************/
static void CANopen_IfTmrOvf(void)
{
  CANOPEN_EVENT event;

  event.NodePtr = DEF_NULL;
  event.IntNbr = 0u;
  event.ErrCodeIx = 0u;
  event.VendorErrCodePtr = DEF_NULL;
  event.EventType = CANOPEN_EVENT_TMR;

  CANopen_SvcTaskWakeUp(&event);
}

/****************************************************************************************************//**
 *                                           CANopen_IfFilterSet()
 *
 * @brief    Set the TX and RX filter for each communication filter.
 *
 * @param    p_node  Pointer to the CANopen node object.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_ARG
 *                       - RTOS_ERR_INVALID_HANDLE
 *******************************************************************************************************/
static void CANopen_IfFilterSet(CANOPEN_NODE *p_node,
                                RTOS_ERR     *p_err)
{
  CAN_BUS_TX_MSG_OBJ msg_obj;
  CAN_BUS_RX_FILTER  filter;
  CANOPEN_IF         *p_if = &p_node->If;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_node != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  msg_obj.EndOfBuffer = DEF_YES;
  msg_obj.MsgNum = CANOPEN_CAN_ID_TX_FILTER;
  msg_obj.RemoteTransfer = DEF_NO;

  CanBus_MsgObjSet(p_if->Bus, msg_obj, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  //                                                               ------------------ NODE SPECIFIC -------------------
  filter.Id = p_if->NodePtr->NodeId;
  filter.Mask = CANOPEN_CAN_ID_NODE_ID_MASK;
  filter.MsgNum = CANOPEN_CAN_ID_RX_FILTER_NODE;
  filter.EndOfBuffer = DEF_NO;

  CanBus_FilterSet(p_if->Bus, filter, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  //                                                               ----------------------- NMT ------------------------
  filter.Id = CANOPEN_COB_ID_NMT;
  filter.Mask = CANOPEN_CAN_ID_MASK;
  filter.MsgNum = CANOPEN_CAN_ID_RX_FILTER_NMT;
  filter.EndOfBuffer = DEF_NO;

  CanBus_FilterSet(p_if->Bus, filter, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  //                                                               ----------------------- SYNC -----------------------
  filter.Id = CANOPEN_COB_ID_SYNC;
  filter.Mask = CANOPEN_CAN_ID_MASK;
  filter.MsgNum = CANOPEN_CAN_ID_RX_FILTER_SYNC;
  filter.EndOfBuffer = DEF_NO;

  CanBus_FilterSet(p_if->Bus, filter, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  //                                                               ---------------------- RPDO#1 ----------------------
  filter.Id = CANOPEN_COB_ID_PDO1_RX_START;
  filter.Mask = CANOPEN_CAN_ID_FNCT_CODE_MASK;
  filter.MsgNum = CANOPEN_CAN_ID_RX_FILTER_PDO1;
  filter.EndOfBuffer = DEF_NO;

  CanBus_FilterSet(p_if->Bus, filter, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  //                                                               ---------------------- RPDO#2 ----------------------
  filter.Id = CANOPEN_COB_ID_PDO2_RX_START;
  filter.Mask = CANOPEN_CAN_ID_FNCT_CODE_MASK;
  filter.MsgNum = CANOPEN_CAN_ID_RX_FILTER_PDO2;
  filter.EndOfBuffer = DEF_NO;

  CanBus_FilterSet(p_if->Bus, filter, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  //                                                               ---------------------- RPDO#3 ----------------------
  filter.Id = CANOPEN_COB_ID_PDO3_RX_START;
  filter.Mask = CANOPEN_CAN_ID_FNCT_CODE_MASK;
  filter.MsgNum = CANOPEN_CAN_ID_RX_FILTER_PDO3;
  filter.EndOfBuffer = DEF_NO;

  CanBus_FilterSet(p_if->Bus, filter, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  //                                                               ---------------------- RPDO#4 ----------------------
  filter.Id = CANOPEN_COB_ID_PDO4_RX_START;
  filter.Mask = CANOPEN_CAN_ID_FNCT_CODE_MASK;
  filter.MsgNum = CANOPEN_CAN_ID_RX_FILTER_PDO4;
  filter.EndOfBuffer = DEF_NO;

  CanBus_FilterSet(p_if->Bus, filter, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  //                                                               ----------------------- HBC ------------------------
  filter.Id = CANOPEN_COB_ID_NMT_ERR_CTRL_START;
  filter.Mask = CANOPEN_CAN_ID_FNCT_CODE_MASK;
  filter.MsgNum = CANOPEN_CAN_ID_RX_FILTER_HBC;
  filter.EndOfBuffer = DEF_YES;

  CanBus_FilterSet(p_if->Bus, filter, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_CANOPEN_AVAIL
