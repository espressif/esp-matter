/***************************************************************************//**
 * @file
 * @brief IO - Internal Spi Api
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

#ifndef  _SPI_PRIV_H_
#define  _SPI_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/collections/slist_priv.h>
#include  <io/source/serial/serial_ctrlr_priv.h>

#include  <io/include/serial.h>
#include  <io/include/spi.h>
#include  <io/include/spi_slave.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 -------------- SPI MODULE ROOT STRUCT --------------
typedef struct spi {
  MEM_DYN_POOL SlaveHandleMemPool;                              // Pool of slave handle.
  SLIST_MEMBER *BusHandleListHeadPtr;                           // Ptr to head of bus handle list.
} SPI;

//                                                                 ------------------ SPI BUS STATE -------------------
typedef enum spi_bus_state {
  SPI_BUS_STATE_FAULT = -1,                                     // Fault.
  SPI_BUS_STATE_STOPPED = 0,                                    // Bus stopped.
  SPI_BUS_STATE_IDLE = 1,                                       // Bus/driver has been started
  SPI_BUS_STATE_ACTIVE = 2                                      // Slave select has been enabled & bus is rdy for comm
} SPI_BUS_STATE;

/****************************************************************************************************
 *                                           SPI BUS HANDLE STRUCT
 ***************************************************************************************************/

struct spi_bus_handle {
  SLIST_MEMBER    ListNode;                                     // Bus handle list node.
  SERIAL_HANDLE   SerialHandle;                                 // Handle on serial controller.

  const CPU_CHAR  *NamePtr;                                     // Pointer to platform manager item name.
  SPI_BUS_STATE   State;                                        // Current SPI bus state.
  KAL_LOCK_HANDLE Lock;                                         // Handle on SPI bus lock.

  SERIAL_CFG      SerCfgCur;                                    // Current serial controller cfgs.
};

/********************************************************************************************************
 *                                           SPI SLAVE HANDLE
 *******************************************************************************************************/

struct spi_slave_handle {
  CPU_INT16U      SlaveID;                                      // Numeral ID of slave.
  CPU_BOOLEAN     ActiveLow;                                    // Flag that indicates if slave is active low or high.

  KAL_LOCK_HANDLE LockHandle;                                   // Handle on slave lock.
  CPU_BOOLEAN     IsSlaveSelActive;                             // Flag indicating if slave currently active.
  SPI_BUS_HANDLE  BusHandle;                                    // Handle on SPI bus.
  SERIAL_CFG      SerialCfg;                                    // Slave's serial cfgs.
  CPU_INT08U      TxDummyByte;                                  // Byte to send on RX only operations.
};

/********************************************************************************************************
 *                                           GLOBAL VARIABLES
 *******************************************************************************************************/

extern SPI *SPI_Ptr;

extern SPI_INIT_CFG SPI_InitCfg;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void SPI_Init(RTOS_ERR *p_err);

void SPI_BusSlaveSel(SPI_BUS_HANDLE bus_handle,
                     CPU_INT16U     slave_id,
                     CPU_BOOLEAN    active_low,
                     SERIAL_CFG     *p_ser_cfg,
                     CPU_INT08U     tx_dummy_byte,
                     CPU_INT32U     timeout_ms,
                     CPU_BOOLEAN    is_nonblocking,
                     RTOS_ERR       *p_err);

void SPI_BusSlaveDesel(SPI_BUS_HANDLE bus_handle,
                       CPU_INT16U     slave_id,
                       CPU_BOOLEAN    active_low,
                       RTOS_ERR       *p_err);

void SPI_BusXfer(SPI_BUS_HANDLE   bus_handle,
                 CPU_INT08U       *p_buf_rx,
                 const CPU_INT08U *p_buf_tx,
                 CPU_INT32U       buf_len,
                 CPU_INT32U       timeout_ms,
                 RTOS_ERR         *p_err);

void SPI_BusRx(SPI_BUS_HANDLE bus_handle,
               CPU_INT08U     *p_buf,
               CPU_INT32U     buf_len,
               CPU_INT32U     timeout_ms,
               RTOS_ERR       *p_err);

void SPI_BusTx(SPI_BUS_HANDLE   bus_handle,
               const CPU_INT08U *p_buf,
               CPU_INT32U       buf_len,
               CPU_INT32U       timeout_ms,
               RTOS_ERR         *p_err);
#endif
