/***************************************************************************//**
 * @file
 * @brief IO - Spi Bus Management Layer
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

#if (defined(RTOS_MODULE_IO_SERIAL_SPI_AVAIL))

#if (!defined(RTOS_MODULE_IO_SERIAL_AVAIL))

#error IO SPI module requires IO Serial module. Make sure it is part of your project and that \
  RTOS_MODULE_IO_SERIAL_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>
#include  <common/include/platform_mgr.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/collections/slist_priv.h>

#include  <io/source/serial/serial_priv.h>
#include  <io/source/serial/serial_ctrlr_priv.h>
#include  <io/source/serial/spi/spi_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_IO

#define  LOG_DFLT_CH                       (IO, SERIAL, SPI)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void SPI_BusLock(SPI_BUS_HANDLE bus_handle,
                        CPU_INT32U     timeout,
                        CPU_BOOLEAN    is_nonblocking,
                        RTOS_ERR       *p_err);

static void SPI_BusUnlock(SPI_BUS_HANDLE bus_handle,
                          RTOS_ERR       *p_err);

static SPI_BUS_STATE SPI_BusStateGet(SPI_BUS_HANDLE bus_handle);

static void SPI_BusStateSet(SPI_BUS_HANDLE bus_handle,
                            SPI_BUS_STATE  state);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

const SPI_BUS_HANDLE SPI_BusHandleNull = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               SPI_BusAdd()
 *
 * @brief    Creates/Adds SPI bus.
 *
 * @param    name    Name to give to the new bus. Must be unique, constant, and in scope for the
 *                   remainder of the program.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
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
 * @return   Handle to SPI bus.
 *******************************************************************************************************/
SPI_BUS_HANDLE SPI_BusAdd(const CPU_CHAR *name,
                          RTOS_ERR       *p_err)
{
  SERIAL_HANDLE  serial_handle;
  SPI_BUS_HANDLE spi_bus_handle;

  spi_bus_handle = (SPI_BUS_HANDLE)Mem_SegAlloc("IO - SPI bus handle",
                                                SPI_InitCfg.MemSegPtr,
                                                sizeof(struct spi_bus_handle),
                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  serial_handle = Serial_CtrlrAdd(name,
                                  SERIAL_CTRLR_MODE_SPI,
                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  spi_bus_handle->SerialHandle = serial_handle;
  spi_bus_handle->State = SPI_BUS_STATE_STOPPED;
  spi_bus_handle->NamePtr = name;

  SPI_BusStateSet(spi_bus_handle, SPI_BUS_STATE_STOPPED);

  spi_bus_handle->Lock = KAL_LockCreate("IO - Bus lock",
                                        DEF_NULL,
                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  SList_Push(&SPI_Ptr->BusHandleListHeadPtr, &spi_bus_handle->ListNode);

  return (spi_bus_handle);
}

/****************************************************************************************************//**
 *                                       SPI_BusHandleGetFromName()
 *
 * @brief    Gets SPI bus handle from serial controller name.
 *
 * @param    name    Serial controller name.
 *
 * @return   Bas handle,         if exists.
 *           SPI_BusHandleNull,  otherwise.
 *******************************************************************************************************/
SPI_BUS_HANDLE SPI_BusHandleGetFromName(const CPU_CHAR *name)
{
  SPI_BUS_HANDLE bus_handle;
  SERIAL_HANDLE  ser_handle;

  ser_handle = Serial_HandleGetFromName(name);
  if (ser_handle == Serial_HandleNull) {
    return (SPI_BusHandleNull);
  }

  SLIST_FOR_EACH_ENTRY(SPI_Ptr->BusHandleListHeadPtr, bus_handle, struct spi_bus_handle, ListNode) {
    if (bus_handle->SerialHandle == ser_handle) {
      return (bus_handle);
    }
  }

  return (SPI_BusHandleNull);
}

/****************************************************************************************************//**
 *                                               SPI_BusStart()
 *
 * @brief    Starts given SPI bus/driver.
 *
 * @param    bus_handle  Handle to SPI bus.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_INVALID_STATE
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void SPI_BusStart(SPI_BUS_HANDLE bus_handle,
                  RTOS_ERR       *p_err)
{
  RTOS_ERR      err_unlock;
  SPI_BUS_STATE bus_state;

  RTOS_ASSERT_DBG(bus_handle != DEF_NULL, RTOS_ERR_INVALID_HANDLE,; );

  //                                                               Lock bus.
  SPI_BusLock(bus_handle,
              0u,
              DEF_NO,
              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  bus_state = SPI_BusStateGet(bus_handle);
  switch (bus_state) {
    case SPI_BUS_STATE_STOPPED:                                 // Bus is stopped. Start it up.
      Serial_Start(bus_handle->SerialHandle,
                   SERIAL_CTRLR_MODE_SPI,
                   p_err);
      break;

    case SPI_BUS_STATE_IDLE:                                    // Bus is already started. Return error.
    case SPI_BUS_STATE_ACTIVE:
    case SPI_BUS_STATE_FAULT:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      return;
  }

  //                                                               Na slave currently selected, no current ser cfg.
  bus_handle->SerCfgCur.Baudrate = 0u;
  bus_handle->SerCfgCur.FrameSize = 0u;
  bus_handle->SerCfgCur.LSB_First = DEF_NO;
  bus_handle->SerCfgCur.Mode = DEF_BIT_NONE;
  bus_handle->SerCfgCur.ParityMode = SERIAL_PARITY_NONE;
  bus_handle->SerCfgCur.StopBit = SERIAL_STOP_BIT_NONE;

  SPI_BusStateSet(bus_handle, SPI_BUS_STATE_IDLE);

  SPI_BusUnlock(bus_handle, &err_unlock);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err_unlock) == RTOS_ERR_NONE, RTOS_ERR_FAIL,; );
}

/****************************************************************************************************//**
 *                                               SPI_BusStop()
 *
 * @brief    Stops given SPI bus/driver.
 *
 * @param    bus_handle  Handle to SPI bus.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_INVALID_STATE
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void SPI_BusStop(SPI_BUS_HANDLE bus_handle,
                 RTOS_ERR       *p_err)
{
  RTOS_ERR      err_unlock;
  SPI_BUS_STATE bus_state;

  RTOS_ASSERT_DBG(bus_handle != DEF_NULL, RTOS_ERR_INVALID_HANDLE,; );

  //                                                               Lock bus.
  SPI_BusLock(bus_handle,
              0u,
              DEF_NO,
              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  bus_state = SPI_BusStateGet(bus_handle);
  switch (bus_state) {
    case SPI_BUS_STATE_FAULT:
    case SPI_BUS_STATE_IDLE:
      Serial_Stop(bus_handle->SerialHandle, p_err);

      SPI_BusStateSet(bus_handle, SPI_BUS_STATE_STOPPED);
      break;

    case SPI_BUS_STATE_STOPPED:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    case SPI_BUS_STATE_ACTIVE:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      break;
  }

  SPI_BusUnlock(bus_handle, &err_unlock);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err_unlock) == RTOS_ERR_NONE, RTOS_ERR_FAIL,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               SPI_BusSlaveSel()
 *
 * @brief    Enables slave-select and configures SPI bus for given slave. This will lock the bus,
 *           preventing any other thread/slave from accessing it.
 *
 * @param    bus_handle      Handle to SPI bus.
 *
 * @param    slave_id        Slave identification number.
 *
 * @param    active_low      Flag that indicates if slave is active low or high.
 *
 * @param    p_ser_cfg       Pointer to structure containing serial configurations.
 *
 * @param    tx_dummy_byte   Dummy byte to transmit in RX only operations.
 *
 * @param    timeout         Timeout, in milliseconds.
 *
 * @param    is_nonblocking  Flag indicating if current call is non-blocking.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_INVALID_STATE
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void SPI_BusSlaveSel(SPI_BUS_HANDLE bus_handle,
                     CPU_INT16U     slave_id,
                     CPU_BOOLEAN    active_low,
                     SERIAL_CFG     *p_ser_cfg,
                     CPU_INT08U     tx_dummy_byte,
                     CPU_INT32U     timeout,
                     CPU_BOOLEAN    is_nonblocking,
                     RTOS_ERR       *p_err)
{
  SPI_BUS_STATE     bus_state;
  SERIAL_SLAVE_INFO ser_slave_info;

  SPI_BusLock(bus_handle,
              timeout,
              is_nonblocking,
              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  bus_state = SPI_BusStateGet(bus_handle);
  switch (bus_state) {
    case SPI_BUS_STATE_STOPPED:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);                  // Bus has not been started. Return error.
      break;

    case SPI_BUS_STATE_IDLE:
      //                                                           Re-configure the SPI bus
      if ((bus_handle->SerCfgCur.Baudrate != p_ser_cfg->Baudrate)
          || (bus_handle->SerCfgCur.FrameSize != p_ser_cfg->FrameSize)
          || (bus_handle->SerCfgCur.LSB_First != p_ser_cfg->LSB_First)
          || (bus_handle->SerCfgCur.Mode != p_ser_cfg->Mode)) {
        Serial_Cfg(bus_handle->SerialHandle,
                   p_ser_cfg,
                   p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          break;
        }

        bus_handle->SerCfgCur = *p_ser_cfg;
      }

      Serial_TxDummyByteSet(bus_handle->SerialHandle,
                            tx_dummy_byte);

      //                                                           Call driver slave-select function
      ser_slave_info.Addr = slave_id;
      ser_slave_info.ActiveLow = active_low;
      Serial_SlaveSel(bus_handle->SerialHandle,
                      &ser_slave_info,
                      p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        break;
      }

      SPI_BusStateSet(bus_handle, SPI_BUS_STATE_ACTIVE);        // Config/slave-sel success. Update bus state.
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      break;
  }

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR err_unlock;

    SPI_BusUnlock(bus_handle, &err_unlock);                     // Config/slave-sel failed.  Return error.
    if (RTOS_ERR_CODE_GET(err_unlock) != RTOS_ERR_NONE) {
      LOG_ERR(("Unlocking bus -> ", RTOS_ERR_LOG_ARG_GET(err_unlock)));
    }
  }
}

/****************************************************************************************************//**
 *                                           SPI_BusSlaveDesel()
 *
 * @brief    Disables slave-select on the SPI bus.
 *
 * @param    bus_handle  Handle to SPI bus.
 *
 * @param    slave_id    Slave identification number.
 *
 * @param    active_low  Flag that indiciates of slave is active low or high.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_NOT_INIT
 *                           - RTOS_ERR_OWNERSHIP
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_INVALID_STATE
 *******************************************************************************************************/
void SPI_BusSlaveDesel(SPI_BUS_HANDLE bus_handle,
                       CPU_INT16U     slave_id,
                       CPU_BOOLEAN    active_low,
                       RTOS_ERR       *p_err)
{
  SPI_BUS_STATE     bus_state;
  SERIAL_SLAVE_INFO ser_slave_info;

  bus_state = SPI_BusStateGet(bus_handle);                      // See note (1)
  switch (bus_state) {
    case SPI_BUS_STATE_STOPPED:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
      break;

    case SPI_BUS_STATE_ACTIVE:
      ser_slave_info.Addr = slave_id;
      ser_slave_info.ActiveLow = active_low;
      Serial_SlaveDesel(bus_handle->SerialHandle,
                        &ser_slave_info,
                        p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        break;
      }

      SPI_BusStateSet(bus_handle, SPI_BUS_STATE_IDLE);          // Put the bus back in the IDLE state

      SPI_BusUnlock(bus_handle, p_err);
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      break;
  }
}

/****************************************************************************************************//**
 *                                               SPI_BusRx()
 *
 * @brief    Recevies data from SPI bus.
 *
 * @param    bus_handle  Handle to SPI bus.
 *
 * @param    p_buf       Pointer to receive buffer.
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
 *
 * @note     (1) This function assumes the SPI bus has been locked by the caller.
 *******************************************************************************************************/
void SPI_BusRx(SPI_BUS_HANDLE bus_handle,
               CPU_INT08U     *p_buf,
               CPU_INT32U     buf_len,
               CPU_INT32U     timeout_ms,
               RTOS_ERR       *p_err)
{
  SPI_BUS_STATE bus_state;

  bus_state = SPI_BusStateGet(bus_handle);
  if (bus_state != SPI_BUS_STATE_ACTIVE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    return;
  }

  Serial_Rx(bus_handle->SerialHandle,
            p_buf,
            buf_len,
            timeout_ms,
            p_err);
}

/****************************************************************************************************//**
 *                                               SPI_BusTx()
 *
 * @brief    Transmits data to SPI bus.
 *
 * @param    bus_handle  Handle to SPI bus.
 *
 * @param    p_buf       Pointer to transmit buffer.
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
 *
 * @note     (1) This function assumes the SPI bus has been locked by the caller.
 *******************************************************************************************************/
void SPI_BusTx(SPI_BUS_HANDLE   bus_handle,
               const CPU_INT08U *p_buf,
               CPU_INT32U       buf_len,
               CPU_INT32U       timeout_ms,
               RTOS_ERR         *p_err)
{
  SPI_BUS_STATE bus_state;

  bus_state = SPI_BusStateGet(bus_handle);
  if (bus_state != SPI_BUS_STATE_ACTIVE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    return;
  }

  Serial_Tx(bus_handle->SerialHandle,
            p_buf,
            buf_len,
            timeout_ms,
            p_err);
}

/****************************************************************************************************//**
 *                                               SPI_BusXfer()
 *
 * @brief    Receives and transmits data from/to SPI bus.
 *
 * @param    bus_handle  Handle to SPI bus.
 *
 * @param    p_buf_rx    Pointer to receive buffer.
 *
 * @param    p_buf_tx    Pointer to transmit buffer.
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
void SPI_BusXfer(SPI_BUS_HANDLE   bus_handle,
                 CPU_INT08U       *p_buf_rx,
                 const CPU_INT08U *p_buf_tx,
                 CPU_INT32U       buf_len,
                 CPU_INT32U       timeout_ms,
                 RTOS_ERR         *p_err)
{
  SPI_BUS_STATE bus_state;

  bus_state = SPI_BusStateGet(bus_handle);
  if (bus_state != SPI_BUS_STATE_ACTIVE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    return;
  }

  Serial_Xfer(bus_handle->SerialHandle,
              p_buf_rx,
              p_buf_tx,
              buf_len,
              timeout_ms,
              p_err);
}

/****************************************************************************************************//**
 *                                           SPI_BusLoopBackEn()
 *
 * @brief    Enables loopback on SPI bus via IO Ctrl.
 *
 * @param    bus_handle  Handle to SPI bus.
 *
 * @param    en          Flag indicating if loopback mode should be enabled or disabled.
 *                       DEF_ENABLED
 *                       DEF_DISABLED
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *******************************************************************************************************/
void SPI_BusLoopBackEn(SPI_BUS_HANDLE bus_handle,
                       CPU_BOOLEAN    en,
                       RTOS_ERR       *p_err)
{
  Serial_LoopbackEn(bus_handle->SerialHandle,
                    en,
                    p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               SPI_BusLock()
 *
 * @brief    Locks SPI bus.
 *
 * @param    bus_handle      Handle to SPI bus.
 *
 * @param    timeout         Timeout, in milliseconds.
 *
 * @param    is_nonblocking  Flag indicating if call should be blocking or not.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
static void SPI_BusLock(SPI_BUS_HANDLE bus_handle,
                        CPU_INT32U     timeout,
                        CPU_BOOLEAN    is_nonblocking,
                        RTOS_ERR       *p_err)
{
  KAL_OPT opt;

  opt = DEF_BIT_NONE;

  if (is_nonblocking) {
    opt |= KAL_OPT_PEND_NON_BLOCKING;
  }

  KAL_LockAcquire(bus_handle->Lock, opt, timeout, p_err);
}

/****************************************************************************************************//**
 *                                               SPI_BusUnlock()
 *
 * @brief    Unlocks SPI bus.
 *
 * @param    bus_handle  Handle to SPI bus.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_OWNERSHIP
 *                           - RTOS_ERR_IS_OWNER
 *******************************************************************************************************/
static void SPI_BusUnlock(SPI_BUS_HANDLE bus_handle,
                          RTOS_ERR       *p_err)
{
  KAL_LockRelease(bus_handle->Lock, p_err);
}

/****************************************************************************************************//**
 *                                               SPI_BusStateGet()
 *
 * @brief    Gets current SPI bus state..
 *
 * @param    bus_handle  Handle to SPI bus.
 *
 * @return   Current SPI bus state.
 *******************************************************************************************************/
static SPI_BUS_STATE SPI_BusStateGet(SPI_BUS_HANDLE bus_handle)
{
  SPI_BUS_STATE state;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  state = bus_handle->State;
  CORE_EXIT_ATOMIC();

  return (state);
}

/****************************************************************************************************//**
 *                                               SPI_BusStateSet()
 *
 * @brief    Sets current SPI bus state.
 *
 * @param    bus_handle  Handle to SPI bus.
 *
 * @param    state       SPI bus state to set.
 *******************************************************************************************************/
static void SPI_BusStateSet(SPI_BUS_HANDLE bus_handle,
                            SPI_BUS_STATE  state)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  bus_handle->State = state;
  CORE_EXIT_ATOMIC();
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_IO_SERIAL_SPI_AVAIL))
