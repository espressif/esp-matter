/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *   Hardware Abstraction Layer for the UART.
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _HAL_SPI_SLAVE_H_
#define _HAL_SPI_SLAVE_H_

/*****************************************************************************
 *                    UART
 *****************************************************************************/

typedef Int16 (* hal_cbSpiSlaveGetTxData_t) (void);
typedef void (* hal_cbSpiSlaveRx_t) (UInt8* data, UInt16 size);
typedef void (* hal_cbSpiSlaveEot_t) (void);
typedef void (* hal_cbSpiSlaveTxDone_t) (UInt32 readIdx);

#ifdef __cplusplus
extern "C" {
#endif

void hal_SpiSlaveInit(void);
void hal_SpiSlaveStart (hal_cbSpiSlaveRx_t cbRx, hal_cbSpiSlaveTxDone_t cbTxDone, UInt8*TxBuf, UInt32 TxBufSize, UInt32 *rxBufferSize);
void hal_SpiSlaveStop(void);

void hal_SpiSlaveDisable(void);
void hal_SpiSlaveEnable(void);
void hal_SpiSlaveTxNewData(UInt32 writeIdx);
void hal_SpiSlaveWaitEndOfTransmission(void);
void hal_SpiSlaveRegisterOneShotEndOfTxCb(hal_cbSpiSlaveEot_t cbEot);

void hal_SpiSlaveComStart(hal_cbSpiSlaveRx_t cbRx, hal_cbSpiSlaveGetTxData_t cbTx);
void hal_SpiSlaveSComStart(hal_cbSpiSlaveRx_t cbRx, hal_cbSpiSlaveGetTxData_t cbTx);
void hal_SpiSlaveComFlush(void);
Bool hal_SpiSlaveTxEnabled(void);
Bool hal_SpiSlaveRxEnabled(void);
void hal_SpiSlaveRxComFlush(void);
void hal_SpiSlave_SetLatencyBytes(UInt32 nbrOfLatencyBytes);

#ifdef __cplusplus
}
#endif

#endif //_HAL_SPI_SLAVE_H_
