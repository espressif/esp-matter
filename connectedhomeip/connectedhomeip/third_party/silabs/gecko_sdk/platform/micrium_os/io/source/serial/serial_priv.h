/***************************************************************************//**
 * @file
 * @brief IO Serial Declarations
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

#ifndef  _SERIAL_PRIV_H_
#define  _SERIAL_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <io/source/serial/serial_ctrlr_priv.h>
#include  <io/include/serial.h>

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

extern const SERIAL_HANDLE Serial_HandleNull;

/********************************************************************************************************
 *                                           ISR HANDLER FUNCTION
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   SPI MODULE CONFIGURATION STRUCT
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 * Note(s) : (1) Some SPI driver may provide support for additional capabilities. If the underlying driver
 *               does not provide support for any of these capabilities, these functions will return an
 *               error.
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  __cplusplus
extern "C" {
#endif

void Serial_Init(RTOS_ERR *p_err);

SERIAL_HANDLE Serial_CtrlrAdd(const CPU_CHAR *name,
                              CPU_INT08U     mode,
                              RTOS_ERR       *p_err);

SERIAL_HANDLE Serial_HandleGetFromName(const CPU_CHAR *name);

void Serial_Start(SERIAL_HANDLE handle,
                  CPU_INT08U    mode,
                  RTOS_ERR      *p_err);

void Serial_Stop(SERIAL_HANDLE handle,
                 RTOS_ERR      *p_err);

CPU_BOOLEAN Serial_CfgChk(SERIAL_HANDLE handle,
                          SERIAL_CFG    *p_cfg,
                          RTOS_ERR      *p_err);

void Serial_Cfg(SERIAL_HANDLE handle,
                SERIAL_CFG    *p_cfg,
                RTOS_ERR      *p_err);

void Serial_SlaveSel(SERIAL_HANDLE           handle,
                     const SERIAL_SLAVE_INFO *p_slave_info,
                     RTOS_ERR                *p_err);

void Serial_SlaveDesel(SERIAL_HANDLE           handle,
                       const SERIAL_SLAVE_INFO *p_slave_info,
                       RTOS_ERR                *p_err);

void Serial_TxDummyByteSet(SERIAL_HANDLE handle,
                           CPU_INT08U    tx_dummy_byte);

void Serial_Rx(SERIAL_HANDLE handle,
               CPU_INT08U    *p_buf,
               CPU_INT32U    buf_len,
               CPU_INT32U    timeout_ms,
               RTOS_ERR      *p_err);

void Serial_Tx(SERIAL_HANDLE    handle,
               const CPU_INT08U *p_buf,
               CPU_INT32U       buf_len,
               CPU_INT32U       timeout_ms,
               RTOS_ERR         *p_err);

void Serial_Xfer(SERIAL_HANDLE    handle,
                 CPU_INT08U       *p_rx_buf,
                 const CPU_INT08U *p_tx_buf,
                 CPU_INT32U       buf_len,
                 CPU_INT32U       timeout_ms,
                 RTOS_ERR         *p_err);

void Serial_LoopbackEn(SERIAL_HANDLE handle,
                       CPU_BOOLEAN   en,
                       RTOS_ERR      *p_err);

#ifdef  __cplusplus
}
#endif

#endif // _SPI_H_
