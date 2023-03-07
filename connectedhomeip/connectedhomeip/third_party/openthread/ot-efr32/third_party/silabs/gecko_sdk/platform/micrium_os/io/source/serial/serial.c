/***************************************************************************//**
 * @file
 * @brief IO Serial Core
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_IO_SERIAL_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/platform_mgr.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/collections/slist_priv.h>

#include  <io/include/serial.h>
#include  <io/source/serial/serial_priv.h>
#include  <io/source/serial/serial_ctrlr_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_IO

#define  LOG_DFLT_CH                       (IO, SERIAL)

#define  SERIAL_INIT_CFG_DFLT               { \
    .MemSegPtr = DEF_NULL,                    \
    .HandleQty = LIB_MEM_BLK_QTY_UNLIMITED    \
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const SERIAL_INIT_CFG  SerialInitCfgDflt = SERIAL_INIT_CFG_DFLT;
static SERIAL_INIT_CFG SerialInitCfg = SERIAL_INIT_CFG_DFLT;
#else
extern const SERIAL_INIT_CFG SerialInitCfg;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       SERIAL MODULE ROOT STRUCT
 *******************************************************************************************************/

typedef struct serial {
  SLIST_MEMBER *SerialHandleListHeadPtr;                        // Ptr to head of serial handle list.
} SERIAL;

/********************************************************************************************************
 *                                           SERIAL CTRLR HANDLE
 *******************************************************************************************************/

struct serial_handle {
  SLIST_MEMBER   ListNode;                                      // Serial handle list node.

  const CPU_CHAR *Name;                                         // Ptr to serial ctrlr name.
  SERIAL_DRV     Drv;                                           // Serial ctrlr drv struct.

  CPU_INT08U     Mode;                                          // Ctrlr current mode (either SPI, I2C or UART).
  CPU_INT08U     TxClkGenByte;                                  // For ser comm where clk must be gen by tx to recv.

  KAL_SEM_HANDLE RxCmplSemHandle;                               // Handle on rx complete semaphore.
  RTOS_ERR       RxErr;                                         // Receive error.

  KAL_SEM_HANDLE TxCmplSemHandle;                               // Handle on tx complete semaphore.
  RTOS_ERR       TxErr;                                         // Transmit error.
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL TABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static SERIAL *SerialPtr = DEF_NULL;

const SERIAL_HANDLE Serial_HandleNull = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       Serial_ConfigureMemSeg()
 *
 * @brief    Configures memory segment to use for internal data allocations.
 *
 * @param    p_seg   Pointer to memory segment.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void Serial_ConfigureMemSeg(MEM_SEG *p_seg)
{
  SerialInitCfg.MemSegPtr = p_seg;
}
#endif

/****************************************************************************************************//**
 *                                       Serial_ConfigureHandleQty()
 *
 * @brief    Configure maximum number of serial handles.
 *
 * @param    handle_qty  Maximum number of serial handles.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void Serial_ConfigureHandleQty(CPU_SIZE_T handle_qty)
{
  SerialInitCfg.HandleQty = handle_qty;
}
#endif

/********************************************************************************************************
 *                                       MODULE LOCAL FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               Serial_Init()
 *
 * @brief    Initializes the serial core module.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void Serial_Init(RTOS_ERR *p_err)
{
  CPU_BOOLEAN kal_complete;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (SerialPtr != DEF_NULL) {
    CORE_EXIT_ATOMIC();

    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return;
  }

  SerialPtr = (SERIAL *)1;
  CORE_EXIT_ATOMIC();

  //                                                               Assert that the system has all necessary features
  kal_complete = KAL_FeatureQuery(KAL_FEATURE_SEM_CREATE, KAL_OPT_NONE);
  kal_complete &= KAL_FeatureQuery(KAL_FEATURE_SEM_PEND, KAL_OPT_NONE);
  kal_complete &= KAL_FeatureQuery(KAL_FEATURE_SEM_POST, KAL_OPT_NONE);
  RTOS_ASSERT_CRITICAL(kal_complete, RTOS_ERR_NOT_SUPPORTED,; );

  SerialPtr = (SERIAL *)Mem_SegAlloc("IO - Serial root struct",
                                     SerialInitCfg.MemSegPtr,
                                     sizeof(SERIAL),
                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  SList_Init(&SerialPtr->SerialHandleListHeadPtr);
}

/****************************************************************************************************//**
 *                                               Serial_CtrlrAdd()
 *
 * @brief    Adds a serial controller.
 *
 * @param    name    Name of serial controller.
 *
 * @param    mode    Serial mode that controller should use:
 *                       - SERIAL_CTRLR_MODE_UART
 *                       - SERIAL_CTRLR_MODE_SPI
 *                       - SERIAL_CTRLR_MODE_I2C
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_FOUND
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_OS_OBJ_DEL
 *                       - RTOS_ERR_WOULD_BLOCK
 *                       - RTOS_ERR_IS_OWNER
 *                       - RTOS_ERR_ABORT
 *                       - RTOS_ERR_TIMEOUT
 *
 * @return   Handle to serial controller.
 *******************************************************************************************************/
SERIAL_HANDLE Serial_CtrlrAdd(const CPU_CHAR *name,
                              CPU_INT08U     mode,
                              RTOS_ERR       *p_err)
{
  SERIAL_HANDLE               ser_handle;
  const SERIAL_CTRLR_DRV_INFO *p_ser_drv_info;
  PLATFORM_MGR_ITEM           *p_item;

  p_item = PlatformMgrItemGetByName(name, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (Serial_HandleNull);
  }

  RTOS_ASSERT_DBG(p_item != DEF_NULL, RTOS_ERR_NOT_FOUND, Serial_HandleNull);
  RTOS_ASSERT_DBG(p_item->Type == PLATFORM_MGR_ITEM_TYPE_HW_INFO_IO_SERIAL_CTRLR, RTOS_ERR_INVALID_TYPE, Serial_HandleNull);

  p_ser_drv_info = ((struct _serial_ctrlr_pm_item *)p_item)->DrvInfoPtr;

  RTOS_ASSERT_DBG(DEF_BIT_IS_SET(p_ser_drv_info->HW_Info.SupportedMode, mode), RTOS_ERR_INVALID_TYPE, Serial_HandleNull);

  //                                                               Check if not already open.
  ser_handle = Serial_HandleGetFromName(name);
  if (ser_handle != Serial_HandleNull) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                        // Multi-mode ctrlr already open.
    return (ser_handle);
  }

  ser_handle = (SERIAL_HANDLE)Mem_SegAlloc("IO - Serial handle",
                                           SerialInitCfg.MemSegPtr,
                                           sizeof(struct serial_handle),
                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (Serial_HandleNull);
  }

  ser_handle->RxCmplSemHandle = KAL_SemCreate("IO - Serial Rx Cmpl Sem",
                                              DEF_NULL,
                                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (Serial_HandleNull);
  }

  ser_handle->TxCmplSemHandle = KAL_SemCreate("IO - Serial Tx Cmpl Sem",
                                              DEF_NULL,
                                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (Serial_HandleNull);
  }

  ser_handle->Name = p_item->StrID;
  ser_handle->Mode = SERIAL_CTRLR_MODE_NONE;
  RTOS_ERR_SET(ser_handle->RxErr, RTOS_ERR_NONE);
  RTOS_ERR_SET(ser_handle->TxErr, RTOS_ERR_NONE);

  ser_handle->Drv.DataPtr = DEF_NULL;
  ser_handle->Drv.HW_InfoPtr = &p_ser_drv_info->HW_Info;
  ser_handle->Drv.DrvAPI_Ptr = p_ser_drv_info->DrvAPI_Ptr;
  ser_handle->Drv.BSP_API_Ptr = p_ser_drv_info->BSP_API_Ptr;

  if (ser_handle->Drv.DrvAPI_Ptr->Init != DEF_NULL) {           // Init driver.
    ser_handle->Drv.DrvAPI_Ptr->Init(&ser_handle->Drv,
                                     ser_handle,
                                     SerialInitCfg.MemSegPtr,
                                     p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (Serial_HandleNull);
    }
  }

  SList_Push(&SerialPtr->SerialHandleListHeadPtr, &ser_handle->ListNode);

  return (ser_handle);
}

/****************************************************************************************************//**
 *                                           Serial_CtrlrReqAlignGet()
 *
 * @brief    Gets required buffer alignment for specified serial controller.
 *
 * @param    name    Serial controller name.
 *
 * @return   Required buffer alignment, in octets.
 *******************************************************************************************************/
CPU_SIZE_T Serial_CtrlrReqAlignGet(const CPU_CHAR *name)
{
  SERIAL_HANDLE ser_handle;
  CPU_SIZE_T    req_align;

  ser_handle = Serial_HandleGetFromName(name);
  if (ser_handle == Serial_HandleNull) {
    return (0u);
  }

  req_align = ser_handle->Drv.DrvAPI_Ptr->ReqAlignGet(&ser_handle->Drv);

  return (req_align);
}

/****************************************************************************************************//**
 *                                       Serial_HandleGetFromName()
 *
 * @brief    Gets serial handle from controller's name.
 *
 * @param    name    Serial controller name.
 *
 * @return   Serial handle,     if exists.
 *           Serial_HandleNull, otherwise.
 *******************************************************************************************************/
SERIAL_HANDLE Serial_HandleGetFromName(const CPU_CHAR *name)
{
  struct serial_handle *p_ser_handle;

  SLIST_FOR_EACH_ENTRY(SerialPtr->SerialHandleListHeadPtr, p_ser_handle, struct serial_handle, ListNode) {
    CPU_INT16S cmp_res;

    cmp_res = Str_Cmp(p_ser_handle->Name, name);
    if (cmp_res == 0u) {
      return ((SERIAL_HANDLE)p_ser_handle);
    }
  }

  return (Serial_HandleNull);
}

/****************************************************************************************************//**
 *                                               Serial_Start()
 *
 * @brief    Starts serial controller.
 *
 * @param    handle  Handle to serial controller.
 *
 * @param    mode    Mode in which serial controller should be started :
 *                       - SERIAL_CTRLR_MODE_UART
 *                       - SERIAL_CTRLR_MODE_SPI
 *                       - SERIAL_CTRLR_MODE_I2C
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_STATE
 *******************************************************************************************************/
void Serial_Start(SERIAL_HANDLE handle,
                  CPU_INT08U    mode,
                  RTOS_ERR      *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (handle->Mode != SERIAL_CTRLR_MODE_NONE) {
    CORE_EXIT_ATOMIC();

    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    return;
  }

  handle->Mode = mode;
  CORE_EXIT_ATOMIC();

  if (handle->Drv.DrvAPI_Ptr->Start != DEF_NULL) {
    handle->Drv.DrvAPI_Ptr->Start(&handle->Drv,                 // Init driver.
                                  mode,
                                  p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      CORE_ENTER_ATOMIC();
      handle->Mode = SERIAL_CTRLR_MODE_NONE;
      CORE_EXIT_ATOMIC();
    }
  }
}

/****************************************************************************************************//**
 *                                               Serial_Stop()
 *
 * @brief    Stops serial controller.
 *
 * @param    handle  Handle to serial controller.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *******************************************************************************************************/
void Serial_Stop(SERIAL_HANDLE handle,
                 RTOS_ERR      *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (handle->Mode == SERIAL_CTRLR_MODE_NONE) {
    CORE_EXIT_ATOMIC();

    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return;
  }

  handle->Mode = SERIAL_CTRLR_MODE_NONE;
  CORE_EXIT_ATOMIC();

  if (handle->Drv.DrvAPI_Ptr->Stop != DEF_NULL) {
    handle->Drv.DrvAPI_Ptr->Stop(&handle->Drv,
                                 p_err);
  }
}

/****************************************************************************************************//**
 *                                               Serial_CfgChk()
 *
 * @brief    Ensures serial configurations are supported by controller/driver.
 *
 * @param    handle  Handle to serial controller.
 *
 * @param    p_cfg   Pointer to serial configuration structure.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *
 * @return   Flag indicating if serial configurations are supported.
 *               - DEF_YES, configurations are     supported.
 *               - DEF_NO,  configurations are not supported.
 *******************************************************************************************************/
CPU_BOOLEAN Serial_CfgChk(SERIAL_HANDLE handle,
                          SERIAL_CFG    *p_cfg,
                          RTOS_ERR      *p_err)
{
  CPU_BOOLEAN valid;

  valid = handle->Drv.DrvAPI_Ptr->CfgChk(&handle->Drv,
                                         p_cfg,
                                         p_err);

  return (valid);
}

/****************************************************************************************************//**
 *                                               Serial_Cfg()
 *
 * @brief    Configures serial controller.
 *
 * @param    handle  Handle to serial controller.
 *
 * @param    p_cfg   Pointer to serial configuration structure.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *******************************************************************************************************/
void Serial_Cfg(SERIAL_HANDLE handle,
                SERIAL_CFG    *p_cfg,
                RTOS_ERR      *p_err)
{
  handle->Drv.DrvAPI_Ptr->Cfg(&handle->Drv,
                              p_cfg,
                              p_err);
}

/****************************************************************************************************//**
 *                                               Serial_SlaveSel()
 *
 * @brief    Selects slave.
 *
 * @param    handle          Handle to serial controller.
 *
 * @param    p_slave_info    Pointer to slave information structure.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *******************************************************************************************************/
void Serial_SlaveSel(SERIAL_HANDLE           handle,
                     const SERIAL_SLAVE_INFO *p_slave_info,
                     RTOS_ERR                *p_err)
{
  handle->Drv.DrvAPI_Ptr->SlaveSel(&handle->Drv,
                                   p_slave_info,
                                   p_err);
}

/****************************************************************************************************//**
 *                                           Serial_SlaveDesel()
 *
 * @brief    Deselects slave.
 *
 * @param    handle          Handle to serial controller.
 *
 * @param    p_slave_info    Pointer to slave information structure.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *******************************************************************************************************/
void Serial_SlaveDesel(SERIAL_HANDLE           handle,
                       const SERIAL_SLAVE_INFO *p_slave_info,
                       RTOS_ERR                *p_err)
{
  handle->Drv.DrvAPI_Ptr->SlaveDesel(&handle->Drv,
                                     p_slave_info,
                                     p_err);
}

/****************************************************************************************************//**
 *                                           Serial_TxDummyByteSet()
 *
 * @brief    Sets dummy byte to send in RX only operation (for SPI mode).
 *
 * @param    handle          Handle to serial controller.
 *
 * @param    tx_dummy_byte   Dummy byte to transmit.
 *******************************************************************************************************/
void Serial_TxDummyByteSet(SERIAL_HANDLE handle,
                           CPU_INT08U    tx_dummy_byte)
{
  handle->TxClkGenByte = tx_dummy_byte;
}

/****************************************************************************************************//**
 *                                               Serial_Rx()
 *
 * @brief    Receives data from serial bus.
 *
 * @param    handle      Handle to serial controller.
 *
 * @param    p_buf       Pointer to receive buffer.
 *
 * @param    buf_len     Buffer length, in octets.
 *
 * @param    timeout_ms  Timeout, in milliseconds.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void Serial_Rx(SERIAL_HANDLE handle,
               CPU_INT08U    *p_buf,
               CPU_INT32U    buf_len,
               CPU_INT32U    timeout_ms,
               RTOS_ERR      *p_err)
{
  RTOS_ERR err_abort;

  handle->Drv.DrvAPI_Ptr->Rx(&handle->Drv,
                             p_buf,
                             buf_len,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (handle->Mode == SERIAL_CTRLR_MODE_SPI) {
    handle->Drv.DrvAPI_Ptr->ByteTx(&handle->Drv,
                                   handle->TxClkGenByte,
                                   buf_len,
                                   p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto end_err;
    }

    KAL_SemPend(handle->TxCmplSemHandle,
                KAL_OPT_NONE,
                timeout_ms,
                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto end_err;
    }
  }

  KAL_SemPend(handle->RxCmplSemHandle,
              KAL_OPT_NONE,
              timeout_ms,
              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    RTOS_ERR_COPY(*p_err, handle->RxErr);
  } else {
    goto end_err;
  }

  return;

end_err:
  handle->Drv.DrvAPI_Ptr->Abort(&handle->Drv,
                                SERIAL_ABORT_DIR_RX | ((handle->Mode == SERIAL_CTRLR_MODE_SPI) ? SERIAL_ABORT_DIR_TX : 0u),
                                &err_abort);
  if (RTOS_ERR_CODE_GET(err_abort) != RTOS_ERR_NONE) {
    LOG_ERR(("Aborting serial transfer -> ", RTOS_ERR_LOG_ARG_GET(err_abort)));
  }

  KAL_SemSet(handle->RxCmplSemHandle,
             0u,
             &err_abort);
  if (RTOS_ERR_CODE_GET(err_abort) != RTOS_ERR_NONE) {
    LOG_ERR(("Reset Rx sem cnt -> ", RTOS_ERR_LOG_ARG_GET(err_abort)));
  }

  KAL_SemSet(handle->TxCmplSemHandle,
             0u,
             &err_abort);
  if (RTOS_ERR_CODE_GET(err_abort) != RTOS_ERR_NONE) {
    LOG_ERR(("Reset Tx sem cnt -> ", RTOS_ERR_LOG_ARG_GET(err_abort)));
  }
}

/****************************************************************************************************//**
 *                                               Serial_Tx()
 *
 * @brief    Transmits data to serial bus.
 *
 * @param    handle      Handle to serial controller.
 *
 * @param    p_buf       Pointer to transmit buffer.
 *
 * @param    buf_len     Buffer length, in octets.
 *
 * @param    timeout_ms  Timeout, in milliseconds.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void Serial_Tx(SERIAL_HANDLE    handle,
               const CPU_INT08U *p_buf,
               CPU_INT32U       buf_len,
               CPU_INT32U       timeout_ms,
               RTOS_ERR         *p_err)
{
  RTOS_ERR err_abort;

  handle->Drv.DrvAPI_Ptr->Tx(&handle->Drv,
                             p_buf,
                             buf_len,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  KAL_SemPend(handle->TxCmplSemHandle,
              KAL_OPT_NONE,
              timeout_ms,
              p_err);
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_NONE:
      RTOS_ERR_COPY(*p_err, handle->TxErr);
      break;

    case RTOS_ERR_TIMEOUT:
      handle->Drv.DrvAPI_Ptr->Abort(&handle->Drv,
                                    SERIAL_ABORT_DIR_TX,
                                    &err_abort);
      if (RTOS_ERR_CODE_GET(err_abort) != RTOS_ERR_NONE) {
        LOG_ERR(("Aborting serial transfer -> ", RTOS_ERR_LOG_ARG_GET(err_abort)));
      }

    default:
      break;
  }
}

/****************************************************************************************************//**
 *                                               Serial_Xfer()
 *
 * @brief    Receives/transmits data from/to serial bus.
 *
 * @param    handle      Handle to serial controller.
 *
 * @param    p_rx_buf    Pointer to receive buffer.
 *
 * @param    p_tx_buf    Pointer to transmit buffer.
 *
 * @param    buf_len     Length of buffer, in octets.
 *
 * @param    timeout_ms  Timeout, in milliseconds.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void Serial_Xfer(SERIAL_HANDLE    handle,
                 CPU_INT08U       *p_rx_buf,
                 const CPU_INT08U *p_tx_buf,
                 CPU_INT32U       buf_len,
                 CPU_INT32U       timeout_ms,
                 RTOS_ERR         *p_err)
{
  RTOS_ERR err_abort;

  handle->Drv.DrvAPI_Ptr->Rx(&handle->Drv,
                             p_rx_buf,
                             buf_len,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  handle->Drv.DrvAPI_Ptr->Tx(&handle->Drv,
                             p_tx_buf,
                             buf_len,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto xfer_err;
  }

  KAL_SemPend(handle->TxCmplSemHandle,
              KAL_OPT_NONE,
              timeout_ms,
              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto xfer_err;
  }

  KAL_SemPend(handle->RxCmplSemHandle,
              KAL_OPT_NONE,
              timeout_ms,
              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto xfer_err;
  }

  RTOS_ERR_COPY(*p_err, (RTOS_ERR_CODE_GET(handle->RxErr) != RTOS_ERR_NONE) ? handle->RxErr : handle->TxErr);

  return;

xfer_err:
  handle->Drv.DrvAPI_Ptr->Abort(&handle->Drv,
                                (SERIAL_ABORT_DIR_RX | SERIAL_ABORT_DIR_TX),
                                &err_abort);
  if (RTOS_ERR_CODE_GET(err_abort) != RTOS_ERR_NONE) {
    LOG_ERR(("Aborting serial transfer -> ", RTOS_ERR_LOG_ARG_GET(err_abort)));
  }

  KAL_SemSet(handle->RxCmplSemHandle,
             0u,
             &err_abort);
  if (RTOS_ERR_CODE_GET(err_abort) != RTOS_ERR_NONE) {
    LOG_ERR(("Reset Rx sem cnt -> ", RTOS_ERR_LOG_ARG_GET(err_abort)));
  }

  KAL_SemSet(handle->TxCmplSemHandle,
             0u,
             &err_abort);
  if (RTOS_ERR_CODE_GET(err_abort) != RTOS_ERR_NONE) {
    LOG_ERR(("Reset Tx sem cnt -> ", RTOS_ERR_LOG_ARG_GET(err_abort)));
  }
}

/****************************************************************************************************//**
 *                                           Serial_LoopbackEn()
 *
 * @brief    Enables/disables loopback mode (if possible) on serial controller.
 *
 * @param    handle  Handle to serial controller.
 *
 * @param    en      Flag inidicating if loopback mode should be enabled or disabled.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_SUPPORTED
 *******************************************************************************************************/
void Serial_LoopbackEn(SERIAL_HANDLE handle,
                       CPU_BOOLEAN   en,
                       RTOS_ERR      *p_err)
{
  handle->Drv.DrvAPI_Ptr->IO_Ctrl(&handle->Drv,
                                  SERIAL_IOCTRL_CMD_LOOPBACK,
                                  &en,
                                  p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           Serial_EventRxCmpl()
 *
 * @brief    Callback called when data receive completed by driver.
 *
 * @param    handle  Handle to serial controller.
 *
 * @param    err     Error code.
 *******************************************************************************************************/
void Serial_EventRxCmpl(SERIAL_HANDLE handle,
                        RTOS_ERR      err)
{
  RTOS_ERR err_local;

  RTOS_ERR_COPY(handle->RxErr, err);

  KAL_SemPost(handle->RxCmplSemHandle,
              KAL_OPT_NONE,
              &err_local);
  if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
    LOG_ERR(("Posting RX cmpl monitor -> ", RTOS_ERR_LOG_ARG_GET(err_local)));
  }
}

/****************************************************************************************************//**
 *                                           Serial_EventTxCmpl()
 *
 * @brief    Callback called when data transmit completed by driver.
 *
 * @param    handle  Handle to serial controller.
 *
 * @param    err     Error code.
 *******************************************************************************************************/
void Serial_EventTxCmpl(SERIAL_HANDLE handle,
                        RTOS_ERR      err)
{
  RTOS_ERR err_local;

  RTOS_ERR_COPY(handle->TxErr, err);

  KAL_SemPost(handle->TxCmplSemHandle,
              KAL_OPT_NONE,
              &err_local);
  if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
    LOG_ERR(("Posting TX cmpl monitor -> ", RTOS_ERR_LOG_ARG_GET(err_local)));
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_IO_SERIAL_AVAIL))
