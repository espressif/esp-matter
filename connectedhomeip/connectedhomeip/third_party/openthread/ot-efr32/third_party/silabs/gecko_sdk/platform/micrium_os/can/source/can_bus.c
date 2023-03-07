/***************************************************************************//**
 * @file
 * @brief CAN
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

#if (defined(RTOS_MODULE_CAN_BUS_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_cfg.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/collections/slist_priv.h>
#include  <common/include/platform_mgr.h>
#if (RTOS_CFG_LOG_EN == DEF_ENABLED)
#include  <common/include/logging.h>
#endif

#include  <can/include/can_bus.h>
#include  <can/source/can_bus_priv.h>

#include  <em_can.h>
#include  <em_gpio.h>
#include  <em_cmu.h>
#include  <em_core.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                   (CAN)
#define  RTOS_MODULE_CUR                                RTOS_CFG_MODULE_CAN

#define  CAN_BUS_11BITS_ID                             0x7FF
#define  CAN_BUS_29BITS_ID                             0x1FFFFFFF
#define  CAN_BUS_TXRDY_TIMEOUT                         500u
#define  CAN_BUS_MAX_DATA_SIZE                         8u
#define  CAN_BUS_MIN_MSG_NUM                           1u
#define  CAN_BUS_MAX_MSG_NUM                           32u
#define  CAN_BUS_STD_ID_SHIFT                          18u
#define  CAN_BUS_MIN_TIME_QUANTA                       8u // Minimum time quanta on Bosch CAN controller
#define  CAN_BUS_MAX_TIME_QUANTA                       25u // Maximum time quanta on Bosch CAN controller

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

CAN_BUS_DATA *CAN_Ptr = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

const CAN_BUS_HANDLE CAN_BusHandleNull = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                             CanBus_Init()
 *
 * @brief    Initializes the CAN module.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_SEG_OVF
 *
 * @note     (1) This function is NOT thread-safe and should be called ONCE during system initialization,
 *               before multi-threading has begun.
 *
 * @note     (2) This function will not return an error -- any error conditions will trigger a failed
 *               assertion.
 *******************************************************************************************************/
void CanBus_Init(RTOS_ERR *p_err)
{
  CPU_BOOLEAN  kal_complete;
  CAN_BUS_DATA *p_can;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  //                                                               Assert that the system has all necessary features.
  kal_complete = KAL_FeatureQuery(KAL_FEATURE_SEM_CREATE, KAL_OPT_NONE);
  kal_complete &= KAL_FeatureQuery(KAL_FEATURE_SEM_PEND, KAL_OPT_NONE);
  kal_complete &= KAL_FeatureQuery(KAL_FEATURE_SEM_POST, KAL_OPT_NONE);
  RTOS_ASSERT_CRITICAL(kal_complete, RTOS_ERR_NOT_SUPPORTED,; );

  p_can = (CAN_BUS_DATA *)Mem_SegAlloc("CAN - CAN root struct",
                                       DEF_NULL,
                                       sizeof(CAN_BUS_DATA),
                                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  Mem_Clr((void *)p_can,
          sizeof(CAN_BUS_DATA));

  SList_Init(&p_can->BusHandleListHeadPtr);

  CORE_ENTER_ATOMIC();
  CAN_Ptr = p_can;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                              CanBus_Add()
 *
 * @brief    Adds a CAN bus.
 *
 * @param    name      Name of CAN bus controller.
 *
 * @param    p_data    Pointer to a data structure.
 *
 * @param    p_err     Pointer to the variable that will receive one of the following error code(s) from this
 *                     function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_FOUND
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_OS_TASK_WAITING
 *******************************************************************************************************/
CAN_BUS_HANDLE CanBus_Add(const CPU_CHAR *name,
                          void           *p_data,
                          RTOS_ERR       *p_err)
{
  CAN_BUS_HANDLE           bus_handle;
  struct can_ctrlr_pm_item *p_can_item;
  CAN_CTRLR_DRV_INFO       *p_info;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, CAN_BusHandleNull);

  p_can_item = (struct can_ctrlr_pm_item *)PlatformMgrItemGetByName(name, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return (CAN_BusHandleNull);
  }

  RTOS_ASSERT_DBG(p_can_item != DEF_NULL, RTOS_ERR_NOT_FOUND, CAN_BusHandleNull);
  RTOS_ASSERT_DBG(p_can_item->Item.Type == PLATFORM_MGR_ITEM_TYPE_HW_INFO_CAN, RTOS_ERR_INVALID_TYPE, CAN_BusHandleNull);

  bus_handle = (CAN_BUS_HANDLE)Mem_SegAlloc("CAN - CAN Bus handle",
                                            DEF_NULL,
                                            sizeof(struct can_bus_handle),
                                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return (CAN_BusHandleNull);
  }

  p_info = (CAN_CTRLR_DRV_INFO *)p_can_item->BusDrvInfoPtr;

  bus_handle->NamePtr = p_can_item->Item.StrID;
  bus_handle->RegBaseAddr = p_info->HW_Info.BaseAddr;
  bus_handle->BusBspApiPtr = p_info->BSP_API_Ptr;
  bus_handle->IF_Rx = p_info->HW_Info.IF_Rx;
  bus_handle->IF_Tx = p_info->HW_Info.IF_Tx;
  bus_handle->OnTx = DEF_NULL;
  bus_handle->OnRx = DEF_NULL;
  bus_handle->BusDrvDataPtr = p_data;
  bus_handle->TxRdySemHandle = KAL_SemCreate("CAN - TXRdySem",
                                             DEF_NULL,
                                             p_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  CORE_ENTER_ATOMIC();
  SList_Push(&CAN_Ptr->BusHandleListHeadPtr,
             &bus_handle->ListNode);
  CORE_EXIT_ATOMIC();

  return (bus_handle);
}

/****************************************************************************************************//**
 *                                             CanBus_Start()
 *
 * @brief    Starts CAN bus controller.
 *
 * @param    bus_handle    Handle to CAN bus.
 *
 * @param    tmr_period    Timer base time in microsecond. Used by high-level protocol.
 *
 * @param    baudrate      Baud rate at which to start the CAN bus.
 *
 * @param    p_err         Pointer to the variable that will receive one of the following error code(s)
 *                         from this function:
 *                             - RTOS_ERR_NONE
 *                             - RTOS_ERR_DEV_ALLOC
 *                             - RTOS_ERR_INVALID_HANDLE
 *                             - RTOS_ERR_INVALID_ARG
 *******************************************************************************************************/
void CanBus_Start(CAN_BUS_HANDLE bus_handle,
                  CPU_INT32U     tmr_period,
                  CPU_INT32U     baudrate,
                  RTOS_ERR       *p_err)
{
  CAN_TypeDef      *p_can;
  CAN_Init_TypeDef can_init = CAN_INIT_DEFAULT;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((bus_handle != CAN_BusHandleNull), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  p_can = (CAN_TypeDef *)bus_handle->RegBaseAddr;

  if ((bus_handle->BusBspApiPtr != DEF_NULL)
      && (bus_handle->BusBspApiPtr->Open != DEF_NULL)) {
    bus_handle->BusBspApiPtr->Open();
  }

  if ((bus_handle->BusBspApiPtr != DEF_NULL)
      && (bus_handle->BusBspApiPtr->TmrCfg != DEF_NULL)) {
    bus_handle->BusBspApiPtr->TmrCfg(tmr_period);
  }

  can_init.bitrate = baudrate;

  CAN_Init(p_can, &can_init);

  KAL_SemSet(bus_handle->TxRdySemHandle,                        // Set semaphore count to 1u. Make TX Ready.
             1u,
             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  CanBus_ModeSet(bus_handle,
                 CAN_BUS_MODE_NORMAL,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if ((bus_handle->BusBspApiPtr != DEF_NULL)
      && (bus_handle->BusBspApiPtr->IntCtrl != DEF_NULL)) {
    bus_handle->BusBspApiPtr->IntCtrl(bus_handle);
  }
}

/****************************************************************************************************//**
 *                                          CanBus_BitTiming()
 *
 * @brief    Set the bitrate and its parameters.
 *
 * @param    bus_handle                    Handle to CAN bus.
 *
 * @param    bitrate                       A wanted bitrate on the CAN bus.
 *
 * @param    propagation_time_segment      A value for the Propagation Time Segment.
 *
 * @param    phase_buffer_segment1         A value for the Phase Buffer Segment 1.
 *
 * @param    phase_buffer_segment2         A value for the Phase Buffer Segment 2.
 *
 * @param    synchronisation_jump_width    A value for the Synchronization Jump Width.
 *
 * @param    p_err         Pointer to the variable that will receive one of the following error code(s)
 *                         from this function:
 *                             - RTOS_ERR_NONE
 *                             - RTOS_ERR_INVALID_ARG
 *
 * @note     (1)    Multiple parameters need to be properly configured.
 *                  See the reference manual for a detailed description.
 *                  Careful : the BRP (Baud Rate Prescaler) is calculated by:
 *                  'brp = freq / (period * bitrate);'. freq is the frequency of the CAN
 *                  device, period the time of transmission of a bit. The result is an 32-bits value.
 *                  Hence it's truncated, causing an approximation error. This error is non
 *                  negligible when the period is high, the bitrate is high, and frequency is low.
 *                  See http://www.bittiming.can-wiki.info/ to help with these configurations.
 *******************************************************************************************************/
void CanBus_BitTiming(CAN_BUS_HANDLE bus_handle,
                      CPU_INT32U     bitrate,
                      CPU_INT16U     propagation_time_segment,
                      CPU_INT16U     phase_buffer_segment1,
                      CPU_INT16U     phase_buffer_segment2,
                      CPU_INT16U     synchronisation_jump_width,
                      RTOS_ERR       *p_err)
{
  CAN_TypeDef *p_can;
  CPU_INT32U  ratio;
  CPU_INT32U  sum;
  CPU_INT32U  brp;
  CPU_INT32U  period;
  CPU_INT32U  freq;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((bus_handle != CAN_BusHandleNull), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  p_can = (CAN_TypeDef *)bus_handle->RegBaseAddr;

  sum = phase_buffer_segment1 + propagation_time_segment;

  period = 1 + sum + phase_buffer_segment2;
  freq = CAN_GetClockFrequency(p_can);

  brp = freq / (period * bitrate);

  if (brp == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

  ratio = (freq / bitrate) / (brp - 1) - 1;

  if ((ratio < CAN_BUS_MIN_TIME_QUANTA)
      || (ratio > CAN_BUS_MAX_TIME_QUANTA)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

  CAN_SetBitTiming(p_can,
                   bitrate,
                   propagation_time_segment,
                   phase_buffer_segment1,
                   phase_buffer_segment2,
                   synchronisation_jump_width);
}

/****************************************************************************************************//**
 *                                        CanBus_TmrSubscribe()
 *
 * @brief    Start hardware timer.
 *
 * @param    tmr_fnct    Pointer to a timer overflow callback.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_INVALID_ARG
 *******************************************************************************************************/
void CanBus_TmrSubscribe(CAN_BUS_ON_TMR_OVF tmr_fnct,
                         RTOS_ERR           *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((tmr_fnct != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  CORE_ENTER_ATOMIC();
  CAN_Ptr->TmrOvfEvent = tmr_fnct;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                       CanBus_HandleGetFromName()
 *
 * @brief    Gets CAN bus handle from its name.
 *
 * @param    name    Name of CAN bus controller.
 *
 * @return   Handle to CAN bus.
 *******************************************************************************************************/
CAN_BUS_HANDLE CanBus_HandleGetFromName(const CPU_CHAR *name)
{
  struct can_bus_handle *p_bus_handle;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  SLIST_FOR_EACH_ENTRY(CAN_Ptr->BusHandleListHeadPtr, p_bus_handle, struct can_bus_handle, ListNode) {
    CPU_INT16S cmp_res;

    CORE_EXIT_ATOMIC();

    cmp_res = Str_Cmp(p_bus_handle->NamePtr, name);
    if (cmp_res == 0u) {
      return ((CAN_BUS_HANDLE)p_bus_handle);
    }

    CORE_ENTER_ATOMIC();
  }
  CORE_EXIT_ATOMIC();

  return (CAN_BusHandleNull);
}

/****************************************************************************************************//**
 *                                             CanBus_Stop()
 *
 * @brief    Stop CAN bus controller.
 *
 * @param    bus_handle      Handle to CAN bus.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_DEV_ALLOC
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void CanBus_Stop(CAN_BUS_HANDLE bus_handle,
                 RTOS_ERR       *p_err)
{
  CAN_TypeDef *p_can;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((bus_handle != CAN_BusHandleNull), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  p_can = (CAN_TypeDef *)bus_handle->RegBaseAddr;

  KAL_SemPend(bus_handle->TxRdySemHandle,
              KAL_OPT_PEND_BLOCKING,
              CAN_BUS_TXRDY_TIMEOUT,
              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  if ((bus_handle->BusBspApiPtr != DEF_NULL)
      && (bus_handle->BusBspApiPtr->Close != DEF_NULL)) {
    bus_handle->BusBspApiPtr->Close();
  }

  CAN_Reset(p_can);
}

/****************************************************************************************************//**
 *                                           CanBus_FilterSet()
 *
 * @brief    Set CAN bus message filter.
 *
 * @param    bus_handle      Handle to CAN bus.
 *
 * @param    filter          Filter structure.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_ARG
 *******************************************************************************************************/
void CanBus_FilterSet(CAN_BUS_HANDLE    bus_handle,
                      CAN_BUS_RX_FILTER filter,
                      RTOS_ERR          *p_err)
{
  CAN_MessageObject_TypeDef message;
  CAN_TypeDef               *p_can;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((bus_handle != CAN_BusHandleNull), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  p_can = (CAN_TypeDef *)bus_handle->RegBaseAddr;

  if ((filter.Id > CAN_BUS_29BITS_ID)
      || (filter.Mask > CAN_BUS_29BITS_ID)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

  if (filter.Id <= CAN_BUS_11BITS_ID) {
    message.extended = DEF_FALSE;
  } else {
    message.extended = DEF_TRUE;
  }

  if (filter.Mask <= CAN_BUS_11BITS_ID) {
    message.extendedMask = DEF_FALSE;
  } else {
    message.extendedMask = DEF_TRUE;
  }

  message.id = filter.Id;
  message.mask = filter.Mask;
  message.msgNum = filter.MsgNum;
  message.directionMask = DEF_FALSE;

  CAN_ConfigureMessageObject(p_can,
                             bus_handle->IF_Rx,
                             message.msgNum,
                             DEF_TRUE,
                             DEF_FALSE,
                             DEF_FALSE,
                             filter.EndOfBuffer,
                             DEF_TRUE);

  CAN_SetIdAndFilter(p_can,
                     bus_handle->IF_Rx,
                     DEF_TRUE,
                     &message,
                     DEF_TRUE);
}

/****************************************************************************************************//**
 *                                          CanBus_RxSubscribe()
 *
 * @brief    Subscribe to data receive events.
 *
 * @param    bus_handle    Handle to CAN bus.
 *
 * @param    rx_fnct       Pointer to RX callback.
 *
 * @param    p_err         Pointer to the variable that will receive one of the following error code(s)
 *                         from this function:
 *                             - RTOS_ERR_NONE
 *                             - RTOS_ERR_INVALID_HANDLE
 *                             - RTOS_ERR_INVALID_ARG
 *
 * @note     (1)    This callback will be called from the CAN Bus ISR.
 *******************************************************************************************************/
void CanBus_RxSubscribe(CAN_BUS_HANDLE bus_handle,
                        CAN_BUS_ON_RX  rx_fnct,
                        RTOS_ERR       *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((bus_handle != CAN_BusHandleNull), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  RTOS_ASSERT_DBG_ERR_SET((rx_fnct != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  bus_handle->OnRx = rx_fnct;
}

/****************************************************************************************************//**
 *                                           CanBus_MsgObjSet()
 *
 * @brief    Set CAN Bus message objects.
 *
 * @param    bus_handle    Handle to CAN Bus.
 *
 * @param    msg_obj       CAN Bus message object structure.
 *
 * @param    p_err         Pointer to the variable that will receive one of the following error code(s)
 *                         from this function:
 *                             - RTOS_ERR_NONE
 *                             - RTOS_ERR_INVALID_HANDLE
 *******************************************************************************************************/
void CanBus_MsgObjSet(CAN_BUS_HANDLE     bus_handle,
                      CAN_BUS_TX_MSG_OBJ msg_obj,
                      RTOS_ERR           *p_err)
{
  CAN_TypeDef *p_can;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((bus_handle != CAN_BusHandleNull), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  p_can = (CAN_TypeDef *)bus_handle->RegBaseAddr;

  CAN_ConfigureMessageObject(p_can,
                             bus_handle->IF_Tx,
                             msg_obj.MsgNum,
                             true,
                             true,
                             msg_obj.RemoteTransfer,
                             msg_obj.EndOfBuffer,
                             true);
}

/****************************************************************************************************//**
 *                                              CanBus_Rd()
 *
 * @brief    Receive data from CAN Bus.
 *
 * @param    bus_handle      Handle to CAN bus.
 *
 * @param    p_msg_nbr       Pointer to the variable that will receive the message number.
 *
 * @param    p_can_id        Pointer to the variable that will receive the CAN message ID.
 *
 * @param    p_buf           Pointer to data buffer.
 *
 * @param    p_size          Pointer to the variable that will receive the data length code (DLC).
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_NULL_PTR
 *******************************************************************************************************/
void CanBus_Rd(CAN_BUS_HANDLE bus_handle,
               CPU_INT08U     *p_msg_nbr,
               CPU_INT32U     *p_can_id,
               CPU_INT08U     *p_buf,
               CPU_INT08U     *p_size,
               RTOS_ERR       *p_err)
{
  CAN_TypeDef               *p_can;
  CAN_MessageObject_TypeDef message;
  CPU_INT08U                if_nbr;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((bus_handle != CAN_BusHandleNull), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  RTOS_ASSERT_DBG_ERR_SET((p_size != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_can_id != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_msg_nbr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_can = (CAN_TypeDef *)bus_handle->RegBaseAddr;

  if_nbr = bus_handle->IF_Rx;

  message.msgNum = *p_msg_nbr;

  //                                                               Read message from CAN Bus.
  CAN_ReadMessage(p_can, if_nbr, &message);

  *p_size = message.dlc;

  if (DEF_BIT_IS_SET(p_can->MIR[if_nbr].ARB, _CAN_MIR_ARB_XTD_MASK)) {
    //                                                             Extended identifier.
    *p_can_id = DEF_BIT_FIELD_RD(p_can->MIR[if_nbr].ARB, _CAN_MIR_ARB_ID_MASK);
  } else {                                                      // Standard identifier.
    *p_can_id = DEF_BIT_FIELD_RD(p_can->MIR[if_nbr].ARB, _CAN_MIR_ARB_ID_MASK) >> CAN_BUS_STD_ID_SHIFT;
  }

  Mem_Copy(p_buf, message.data, *p_size);                       // Copy data from message buffer to frame pointer.

  *p_msg_nbr = message.msgNum;
}

/****************************************************************************************************//**
 *                                           CanBus_MsgReset()
 *
 * @brief    Reset message objects in both mode (Rx and Tx).
 *
 * @param    bus_handle    Handle to CAN bus.
 *
 * @param    p_err         Pointer to the variable that will receive one of the following error code(s)
 *                         from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *******************************************************************************************************/
void CanBus_MsgReset(CAN_BUS_HANDLE bus_handle,
                     RTOS_ERR       *p_err)
{
  CAN_TypeDef *p_can;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((bus_handle != CAN_BusHandleNull), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  p_can = (CAN_TypeDef *)bus_handle->RegBaseAddr;

  CAN_ResetMessages(p_can, bus_handle->IF_Tx);
  CAN_ResetMessages(p_can, bus_handle->IF_Rx);
}

/****************************************************************************************************//**
 *                                              CanBus_Wr()
 *
 * @brief    Send a CAN frame on the CAN Bus.
 *
 * @param    bus_handle    Handle to CAN bus.
 *
 * @param    tx_fnct       Pointer to TX callback function.
 *
 * @param    msg_nbr       Message number of the frame [1..32].
 *
 * @param    can_id        CAN identification.
 *
 * @param    p_buf         Pointer to data buffer.
 *
 * @param    size          Number of byte to send.
 *
 * @param    p_err         Pointer to the variable that will receive one of the following error code(s)
 *                         from this function:
 *                             - RTOS_ERR_NONE
 *                             - RTOS_ERR_INVALID_ARG
 *                             - RTOS_ERR_INVALID_HANDLE
 *                             - RTOS_ERR_NULL_PTR
 *
 *@note    (1)    tx_fnct will be called on transmission completion from the CAN Bus ISR.
 *******************************************************************************************************/
void CanBus_Wr(CAN_BUS_HANDLE bus_handle,
               CAN_BUS_ON_TX  tx_fnct,
               CPU_INT08U     msg_nbr,
               CPU_INT32U     can_id,
               CPU_INT08U     *p_buf,
               CPU_INT16U     size,
               RTOS_ERR       *p_err)
{
  CAN_TypeDef               *p_can;
  CAN_MessageObject_TypeDef message;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((bus_handle != CAN_BusHandleNull), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((size <= CAN_BUS_MAX_DATA_SIZE), *p_err, RTOS_ERR_INVALID_ARG,; );

  RTOS_ASSERT_DBG_ERR_SET((can_id < CAN_BUS_29BITS_ID), *p_err, RTOS_ERR_INVALID_ARG,; );

  p_can = (CAN_TypeDef *)bus_handle->RegBaseAddr;

  if ((msg_nbr > CAN_BUS_MAX_MSG_NUM)
      || (msg_nbr < CAN_BUS_MIN_MSG_NUM)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

  bus_handle->OnTx = tx_fnct;

  message.dlc = size;
  message.id = can_id;
  message.msgNum = msg_nbr;

  Mem_Copy(&message.data,
           p_buf,
           size);

  if (message.id <= CAN_BUS_11BITS_ID) {
    message.extended = false;
  } else {
    message.extended = true;
  }

  KAL_SemPend(bus_handle->TxRdySemHandle,
              KAL_OPT_PEND_BLOCKING,
              CAN_BUS_TXRDY_TIMEOUT,
              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  CAN_SendMessage(p_can,
                  bus_handle->IF_Tx,
                  &message,
                  true);
}

/****************************************************************************************************//**
 *                                            CanBus_SetMode()
 *
 * @brief    Set CAN Bus operational mode.
 *
 * @param    bus_handle    Handle to CAN bus.
 *
 * @param    mode          Operational mode.
 *
 * @param    p_err         Pointer to the variable that will receive one of the following error code(s)
 *                         from this function:
 *                             - RTOS_ERR_NONE
 *                             - RTOS_ERR_INVALID_ARG
 *******************************************************************************************************/
void CanBus_ModeSet(CAN_BUS_HANDLE bus_handle,
                    CAN_BUS_MODE   mode,
                    RTOS_ERR       *p_err)
{
  CAN_TypeDef      *p_can;
  CAN_Mode_TypeDef can_mode = canModeNormal;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_CRITICAL((bus_handle != CAN_BusHandleNull), RTOS_ERR_INVALID_HANDLE,; );

  p_can = (CAN_TypeDef *)bus_handle->RegBaseAddr;

  switch (mode) {
    case CAN_BUS_MODE_NORMAL:
      can_mode = canModeNormal;
      break;

    case CAN_BUS_MODE_BASIC:
      can_mode = canModeBasic;
      break;

    case CAN_BUS_MODE_LOOPBACK:
      can_mode = canModeLoopBack;
      break;

    case CAN_BUS_MODE_SILENTLOOPBACK:
      can_mode = canModeSilentLoopBack;
      break;

    case CAN_BUS_MODE_SILENT:
      can_mode = canModeSilent;
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      break;
  }

  CAN_SetMode(p_can, can_mode);
}

/****************************************************************************************************//**
 *                                         CanBus_SetBitTiming()
 *
 * @brief    Set CAN Bus bit timing.
 *
 * @param    bus_handle                  Handle to CAN bus.
 *
 * @param    bitrate                     Wanted bitrate on the CAN Bus. Value range from 10000 to 1000000.
 *
 * @param    propagation_time_segment    Value for the Propagation Time Segment. Value range from 1 to 8.
 *
 * @param    phase_buffer_segment1       Value for the Phase Buffer Segment 1. Value range from 1 to 8.
 *
 * @param    phase_buffer_segment2       Value for the Phase Buffer Segment 2. Value range from 1 to 8.
 *
 * @param    sync_jump_width             Value for the Synchronization Jump Width. value range from 1 to 3.
 *
 * @param    p_err                       Pointer to the variable that will receive one of the following
 *                                       error code(s) from this function:
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_INVALID_ARG
 *
 * @note     (1)    According to the CAN specification, the bit time is divided into four segments.
 *                  The Synchronization Segment, the Propagation Time Segment, the Phase Buffer Segment 1,
 *                  and the Phase Buffer Segment 2. Each segment consists of a specific,
 *                  programmable number of time quanta.
 *                  See 'BOSCH CAN Specification, version 2.0, section 8' for more information.
 *******************************************************************************************************/
void CanBus_BitTimingSet(CAN_BUS_HANDLE bus_handle,
                         CPU_INT32U     bitrate,
                         CPU_INT16U     propagation_time_segment,
                         CPU_INT16U     phase_buffer_segment1,
                         CPU_INT16U     phase_buffer_segment2,
                         CPU_INT16U     sync_jump_width,
                         RTOS_ERR       *p_err)
{
  CAN_TypeDef *p_can;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_CRITICAL((bus_handle != CAN_BusHandleNull), RTOS_ERR_INVALID_HANDLE,; );

  p_can = (CAN_TypeDef *)bus_handle->RegBaseAddr;

  CAN_SetBitTiming(p_can,
                   bitrate,
                   propagation_time_segment,
                   phase_buffer_segment1,
                   phase_buffer_segment2,
                   sync_jump_width);
}

/****************************************************************************************************//**
 *                                          CanBus_ISRHandler()
 *
 * @brief    CAN bus ISR handler.
 *
 * @param    bus_handle    Handle to CAN bus.
 *******************************************************************************************************/
void CanBus_ISRHandler(CAN_BUS_HANDLE bus_handle)
{
  CPU_INT32U  status = 0u;
  CPU_INT32U  message_int = 0u;
  CAN_TypeDef *p_can;
  RTOS_ERR    err_local;

  RTOS_ASSERT_CRITICAL((bus_handle != CAN_BusHandleNull), RTOS_ERR_INVALID_HANDLE,; );

  p_can = (CAN_TypeDef *)bus_handle->RegBaseAddr;

  status = CAN_StatusGet(p_can);
  message_int = CAN_MessageIntGetEnabled(p_can);

  if (DEF_BIT_IS_SET(status, _CAN_STATUS_TXOK_MASK)) {
    if ((bus_handle->OnTx != DEF_NULL)) {
      bus_handle->OnTx(bus_handle);
    }

    KAL_SemPost(bus_handle->TxRdySemHandle,
                KAL_OPT_POST_NONE,
                &err_local);
    RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err_local) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  if (DEF_BIT_IS_SET(status, _CAN_STATUS_RXOK_MASK)
      && ((p_can->INTID     & _CAN_INTID_INTID_MASK) > 0u)      // Check if ID corresponds to a valid filter.
      && (bus_handle->OnRx != DEF_NULL)) {
    bus_handle->OnRx(bus_handle->BusDrvDataPtr,
                     (CPU_INT08U)p_can->INTID);
  }

  CAN_StatusClear(p_can, status);
  CAN_MessageIntClear(p_can, message_int);
}

/****************************************************************************************************//**
 *                                          CanBus_TimerISRHandler()
 *
 * @brief    CAN bus timer ISR handler.
 *******************************************************************************************************/
void CanBus_TmrISRHandler(void)
{
  if (CAN_Ptr->TmrOvfEvent != DEF_NULL) {
    CAN_Ptr->TmrOvfEvent();
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_CAN_BUS_AVAIL
