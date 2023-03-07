/***************************************************************************//**
 * @file
 * @brief IO - Spi Slave
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
#include  <common/include/lib_mem.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/kal/kal_priv.h>

#include  <io/source/serial/serial_priv.h>
#include  <io/source/serial/spi/spi_priv.h>

#include  <io/include/spi.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_IO

#define  LOG_DFLT_CH                       (IO, SERIAL, SPI)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

const SPI_SLAVE_HANDLE SPI_SlaveHandleNull = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL TABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void SPI_SlaveSelInternal(SPI_SLAVE_HANDLE slave_handle,
                                 CPU_INT32U       timeout_ms,
                                 SPI_OPT          opt,
                                 RTOS_ERR         *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               SPI_SlaveOpen()
 *
 * @brief    Opens/creates a slave on an SPI bus.
 *
 * @param    bus_handle      Handle on SPI bus.
 *
 * @param    p_slave_info    Pointer to slave information structure.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_SEG_OVF
 *
 * @return   SPI slave handle
 *******************************************************************************************************/
SPI_SLAVE_HANDLE SPI_SlaveOpen(SPI_BUS_HANDLE       bus_handle,
                               const SPI_SLAVE_INFO *p_slave_info,
                               RTOS_ERR             *p_err)
{
  CPU_BOOLEAN      valid;
  KAL_LOCK_EXT_CFG lock_cfg;
  SPI_SLAVE_HANDLE slave_handle = SPI_SlaveHandleNull;
  SERIAL_CFG       ser_cfg;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, slave_handle);
  RTOS_ASSERT_DBG(bus_handle != SPI_BusHandleNull, RTOS_ERR_INVALID_HANDLE, slave_handle);
  RTOS_ASSERT_DBG(p_slave_info != DEF_NULL, RTOS_ERR_INVALID_ARG, slave_handle);

  ser_cfg.Baudrate = p_slave_info->SClkFreqMax;
  ser_cfg.FrameSize = p_slave_info->BitsPerFrame;
  ser_cfg.LSB_First = p_slave_info->LSbFirst;
  ser_cfg.Mode = p_slave_info->Mode;
  ser_cfg.ParityMode = SERIAL_PARITY_NONE;
  ser_cfg.StopBit = SERIAL_STOP_BIT_NONE;

  valid = Serial_CfgChk(bus_handle->SerialHandle,
                        &ser_cfg,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (SPI_SlaveHandleNull);
  }

  if (valid != DEF_VALID) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);

    return (SPI_SlaveHandleNull);
  }

  slave_handle = (SPI_SLAVE_HANDLE)Mem_DynPoolBlkGet(&SPI_Ptr->SlaveHandleMemPool,
                                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (SPI_SlaveHandleNull);
  }

  lock_cfg.Opt = KAL_OPT_CREATE_REENTRANT;
  slave_handle->LockHandle = KAL_LockCreate("IO - SPI slave lock",
                                            &lock_cfg,
                                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR err_free;

    Mem_DynPoolBlkFree(&SPI_Ptr->SlaveHandleMemPool,
                       (void *)slave_handle,
                       &err_free);
    if (RTOS_ERR_CODE_GET(err_free) != RTOS_ERR_NONE) {
      LOG_ERR(("Freeing slave handle -> ", RTOS_ERR_LOG_ARG_GET(err_free)));
    }

    return (SPI_SlaveHandleNull);
  }

  slave_handle->SerialCfg = ser_cfg;
  slave_handle->BusHandle = bus_handle;
  slave_handle->IsSlaveSelActive = DEF_NO;
  slave_handle->SlaveID = p_slave_info->SlaveID;
  slave_handle->ActiveLow = p_slave_info->ActiveLow;
  slave_handle->TxDummyByte = p_slave_info->TxDummyByte;

  return (slave_handle);
}

/****************************************************************************************************//**
 *                                               SPI_SlaveClose()
 *
 * @brief    Closes an SPI slave.
 *
 * @param    slave_handle    Handle on SPI slave.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_POOL_FULL
 *                               - RTOS_ERR_OWNERSHIP
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_INVALID_STATE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void SPI_SlaveClose(SPI_SLAVE_HANDLE slave_handle,
                    RTOS_ERR         *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG(slave_handle != SPI_SlaveHandleNull, RTOS_ERR_INVALID_HANDLE,; );

  KAL_LockAcquire(slave_handle->LockHandle,
                  KAL_OPT_NONE,
                  KAL_TIMEOUT_INFINITE,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (slave_handle->IsSlaveSelActive) {                         // Make sure the bus is released before closing
    SPI_BusSlaveDesel(slave_handle->BusHandle,
                      slave_handle->SlaveID,
                      slave_handle->ActiveLow,
                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR err_rel;

      KAL_LockRelease(slave_handle->LockHandle, &err_rel);
      if (RTOS_ERR_CODE_GET(err_rel) != RTOS_ERR_NONE) {
        LOG_ERR(("Releasing lock handle -> ", RTOS_ERR_LOG_ARG_GET(err_rel)));
      }

      return;
    }
  }

  KAL_LockRelease(slave_handle->LockHandle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  KAL_LockDel(slave_handle->LockHandle);

  Mem_DynPoolBlkFree(&SPI_Ptr->SlaveHandleMemPool,
                     (void *)slave_handle,
                     p_err);
}

/****************************************************************************************************//**
 *                                               SPI_SlaveSel()
 *
 * @brief    Enables slave-select for the slave associated with 'handle'
 *           This function will reconfigure the SPI bus using the handle's settings. Each call to
 *           SPI_SlaveSel() must be paired with another call, SPI_SlaveDesel(), to release the
 *           bus.
 *
 * @param    slave_handle    Handle on SPI slave.
 *
 * @param    timeout_ms      Timeout, in milliseconds.
 *
 * @param    opt             Options flags for slave-select. May be one of the following:
 *                               - SPI_OPT_NONE           Block until the bus is available
 *                               - SPI_OPT_NON_BLOCKING   Return immediately if bus is unavailable
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
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
 *
 * @note     (1) Enabling slave-select will lock the handle's SPI bus to the calling task, preventing
 *               any other task from accessing it.
 *
 * @note     (2) Calls to SPI_SlaveSel() cannot be nested.
 *******************************************************************************************************/
void SPI_SlaveSel(SPI_SLAVE_HANDLE slave_handle,
                  CPU_INT32U       timeout_ms,
                  SPI_OPT          opt,
                  RTOS_ERR         *p_err)
{
  RTOS_ERR err_rel;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG(slave_handle != SPI_SlaveHandleNull, RTOS_ERR_INVALID_ARG,; );

  KAL_LockAcquire(slave_handle->LockHandle,
                  KAL_OPT_NONE,
                  timeout_ms,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (slave_handle->IsSlaveSelActive) {
    goto end_rel;
  }

  SPI_SlaveSelInternal(slave_handle,
                       timeout_ms,
                       opt,
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  CORE_ENTER_ATOMIC();
  slave_handle->IsSlaveSelActive = DEF_TRUE;
  CORE_EXIT_ATOMIC();

  return;

end_rel:
  KAL_LockRelease(slave_handle->LockHandle, &err_rel);
  if (RTOS_ERR_CODE_GET(err_rel) != RTOS_ERR_NONE) {
    LOG_ERR(("Releasing lock handle -> ", RTOS_ERR_LOG_ARG_GET(err_rel)));
  }
}

/****************************************************************************************************//**
 *                                               SPI_SlaveDesel()
 *
 * @brief    Disables slave-select for the slave associated with 'slave_handle'.
 *
 * @param    slave_handle    Handle on an SPI slave.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OWNERSHIP
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_INVALID_STATE
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void SPI_SlaveDesel(SPI_SLAVE_HANDLE slave_handle,
                    RTOS_ERR         *p_err)
{
  RTOS_ERR err_rel;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG(slave_handle != SPI_SlaveHandleNull, RTOS_ERR_INVALID_ARG,; );

  CORE_ENTER_ATOMIC();
  if (!slave_handle->IsSlaveSelActive) {
    CORE_EXIT_ATOMIC();
    return;
  }
  CORE_EXIT_ATOMIC();

  SPI_BusSlaveDesel(slave_handle->BusHandle,
                    slave_handle->SlaveID,
                    slave_handle->ActiveLow,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  slave_handle->IsSlaveSelActive = DEF_FALSE;

  KAL_LockRelease(slave_handle->LockHandle, &err_rel);
  if (RTOS_ERR_CODE_GET(err_rel) != RTOS_ERR_NONE) {
    LOG_ERR(("Releasing lock handle -> ", RTOS_ERR_LOG_ARG_GET(err_rel)));
  }
}

/****************************************************************************************************//**
 *                                               SPI_SlaveRx()
 *
 * @brief    Receives data from SPI slave.
 *
 * @param    slave_handle    Handle on an SPI slave.
 *
 * @param    p_buf           Pointer to receive buffer.
 *
 * @param    buf_len         Buffer length, in octets,
 *
 * @param    timeout_ms      Timeout, in milliseconds.
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
void SPI_SlaveRx(SPI_SLAVE_HANDLE slave_handle,
                 CPU_INT08U       *p_buf,
                 CPU_INT32U       buf_len,
                 CPU_INT32U       timeout_ms,
                 RTOS_ERR         *p_err)
{
  RTOS_ERR err_rel;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG(slave_handle != SPI_SlaveHandleNull, RTOS_ERR_INVALID_HANDLE,; );
  RTOS_ASSERT_DBG(p_buf != DEF_NULL, RTOS_ERR_INVALID_ARG,; );
  RTOS_ASSERT_DBG(buf_len > 0u, RTOS_ERR_INVALID_ARG,; );

  KAL_LockAcquire(slave_handle->LockHandle,
                  KAL_OPT_NONE,
                  timeout_ms,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (!slave_handle->IsSlaveSelActive) {
    SPI_SlaveSelInternal(slave_handle,
                         timeout_ms,
                         SPI_OPT_NONE,
                         p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto end_rel;
    }
  }

  SPI_BusRx(slave_handle->BusHandle,
            p_buf,
            buf_len,
            timeout_ms,
            p_err);

  if (!slave_handle->IsSlaveSelActive) {
    RTOS_ERR err_desel;

    SPI_BusSlaveDesel(slave_handle->BusHandle,
                      slave_handle->SlaveID,
                      slave_handle->ActiveLow,
                      &err_desel);
    if (RTOS_ERR_CODE_GET(err_desel) != RTOS_ERR_NONE) {
      LOG_ERR(("De-selecting slave -> ", RTOS_ERR_LOG_ARG_GET(err_desel)));
    }
  }

end_rel:
  KAL_LockRelease(slave_handle->LockHandle, &err_rel);
  if (RTOS_ERR_CODE_GET(err_rel) != RTOS_ERR_NONE) {
    LOG_ERR(("Releasing lock handle -> ", RTOS_ERR_LOG_ARG_GET(err_rel)));
  }
}

/****************************************************************************************************//**
 *                                               SPI_SlaveTx()
 *
 * @brief    Transmits data to SPI slave.
 *
 * @param    slave_handle    Handle on an SPI slave.
 *
 * @param    p_buf           Pointer to transmit buffer.
 *
 * @param    buf_len         Buffer length, in octets,
 *
 * @param    timeout_ms      Timeout, in milliseconds.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
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
void SPI_SlaveTx(SPI_SLAVE_HANDLE slave_handle,
                 CPU_INT08U       *p_buf,
                 CPU_INT32U       buf_len,
                 CPU_INT32U       timeout_ms,
                 RTOS_ERR         *p_err)
{
  RTOS_ERR err_rel;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG(slave_handle != SPI_SlaveHandleNull, RTOS_ERR_INVALID_HANDLE,; );
  RTOS_ASSERT_DBG(p_buf != DEF_NULL, RTOS_ERR_INVALID_ARG,; );
  RTOS_ASSERT_DBG(buf_len > 0u, RTOS_ERR_INVALID_ARG,; );

  KAL_LockAcquire(slave_handle->LockHandle,
                  KAL_OPT_NONE,
                  timeout_ms,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (!slave_handle->IsSlaveSelActive) {
    SPI_SlaveSelInternal(slave_handle,
                         timeout_ms,
                         SPI_OPT_NONE,
                         p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto end_rel;
    }
  }

  SPI_BusTx(slave_handle->BusHandle,
            p_buf,
            buf_len,
            timeout_ms,
            p_err);

  if (!slave_handle->IsSlaveSelActive) {
    RTOS_ERR err_desel;

    SPI_BusSlaveDesel(slave_handle->BusHandle,
                      slave_handle->SlaveID,
                      slave_handle->ActiveLow,
                      &err_desel);
    if (RTOS_ERR_CODE_GET(err_desel) != RTOS_ERR_NONE) {
      LOG_ERR(("De-selecting slave -> ", RTOS_ERR_LOG_ARG_GET(err_desel)));
    }
  }

end_rel:
  KAL_LockRelease(slave_handle->LockHandle, &err_rel);
  if (RTOS_ERR_CODE_GET(err_rel) != RTOS_ERR_NONE) {
    LOG_ERR(("Releasing lock handle -> ", RTOS_ERR_LOG_ARG_GET(err_rel)));
  }
}

/****************************************************************************************************//**
 *                                               SPI_SlaveXfer()
 *
 * @brief    Transmits and receives data to/from SPI slave.
 *
 * @param    slave_handle    Handle on an SPI slave.
 *
 * @param    p_buf_rx        Pointer to receive buffer.
 *
 * @param    p_buf_tx        Pointer to transmit buffer.
 *
 * @param    buf_len         Buffer length, in octets,
 *
 * @param    timeout_ms      Timeout, in milliseconds.
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
void SPI_SlaveXfer(SPI_SLAVE_HANDLE slave_handle,
                   CPU_INT08U       *p_buf_rx,
                   CPU_INT08U       *p_buf_tx,
                   CPU_INT32U       buf_len,
                   CPU_INT32U       timeout_ms,
                   RTOS_ERR         *p_err)
{
  RTOS_ERR err_rel;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG(slave_handle != SPI_SlaveHandleNull, RTOS_ERR_INVALID_HANDLE,; );
  RTOS_ASSERT_DBG((p_buf_rx != DEF_NULL) && (p_buf_tx != DEF_NULL), RTOS_ERR_INVALID_ARG,; );
  RTOS_ASSERT_DBG(buf_len > 0u, RTOS_ERR_INVALID_ARG,; );

  KAL_LockAcquire(slave_handle->LockHandle,
                  KAL_OPT_NONE,
                  timeout_ms,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (!slave_handle->IsSlaveSelActive) {
    SPI_SlaveSelInternal(slave_handle,
                         timeout_ms,
                         SPI_OPT_NONE,
                         p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto end_rel;
    }
  }

  SPI_BusXfer(slave_handle->BusHandle,
              p_buf_rx,
              p_buf_tx,
              buf_len,
              timeout_ms,
              p_err);

  if (!slave_handle->IsSlaveSelActive) {
    RTOS_ERR err_desel;

    SPI_BusSlaveDesel(slave_handle->BusHandle,
                      slave_handle->SlaveID,
                      slave_handle->ActiveLow,
                      &err_desel);
    if (RTOS_ERR_CODE_GET(err_desel) != RTOS_ERR_NONE) {
      LOG_ERR(("De-selecting slave -> ", RTOS_ERR_LOG_ARG_GET(err_desel)));
    }
  }

end_rel:
  KAL_LockRelease(slave_handle->LockHandle, &err_rel);
  if (RTOS_ERR_CODE_GET(err_rel) != RTOS_ERR_NONE) {
    LOG_ERR(("Releasing lock handle -> ", RTOS_ERR_LOG_ARG_GET(err_rel)));
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           SPI_SlaveSelInternal()
 *
 * @brief    Enables slave-select for the slave associated with 'handle'
 *
 *           This function will reconfigure the SPI bus using the handle's settings. Each call to
 *           SPI_SlaveSel() must be paired with another call, SPI_SlaveDesel(), to release the
 *           bus.
 *
 * @param    slave_handle    Handle on SPI slave.
 *
 * @param    timeout_ms      Timeout, in milliseconds.
 *
 * @param    opt             Options flags for slave-select. May be one of the following:
 *                           SPI_OPT_NONE           Block until the bus is available
 *                           SPI_OPT_NON_BLOCKING   Return immediately if bus is unavailable
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
static void SPI_SlaveSelInternal(SPI_SLAVE_HANDLE slave_handle,
                                 CPU_INT32U       timeout_ms,
                                 SPI_OPT          opt,
                                 RTOS_ERR         *p_err)
{
  CPU_BOOLEAN is_nonblocking;

  is_nonblocking = (opt & SPI_OPT_NON_BLOCKING);

  SPI_BusSlaveSel(slave_handle->BusHandle,
                  slave_handle->SlaveID,
                  slave_handle->ActiveLow,
                  &slave_handle->SerialCfg,
                  slave_handle->TxDummyByte,
                  timeout_ms,
                  is_nonblocking,
                  p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_IO_SERIAL_SPI_AVAIL))
