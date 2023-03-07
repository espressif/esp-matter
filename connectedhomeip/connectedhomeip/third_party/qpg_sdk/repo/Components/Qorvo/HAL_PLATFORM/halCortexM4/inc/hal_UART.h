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

#ifndef _HAL_UART_H_
#define _HAL_UART_H_

/*****************************************************************************
 *                    UART
 *****************************************************************************/

/** flags */
/* bit 0..1*/
#define HAL_UART_OPT_EVEN_PARITY        GP_WB_ENUM_PARITY_EVEN
#define HAL_UART_OPT_ODD_PARITY         GP_WB_ENUM_PARITY_ODD
#define HAL_UART_OPT_NO_PARITY          GP_WB_ENUM_PARITY_OFF
/* bit 2-3*/
#define HAL_UART_OPT_ONE_STOP_BIT       (1 * BM(2))
#define HAL_UART_OPT_TWO_STOP_BITS      (2 * BM(2))
/* bit 4-7*/
#define HAL_UART_OPT_5_BITS_PER_CHAR    (5 * BM(4))
#define HAL_UART_OPT_6_BITS_PER_CHAR    (6 * BM(4))
#define HAL_UART_OPT_7_BITS_PER_CHAR    (7 * BM(4))
#define HAL_UART_OPT_8_BITS_PER_CHAR    (8 * BM(4))
//#define UART_OPT_9_BITS_PER_CHAR    (9 * BM(4))

typedef Int16 (* hal_cbUartGetTxData_t) (void);
#if defined(HAL_DIVERSITY_UART_RX_BUFFER_CALLBACK)
typedef void (* hal_cbUartRx_t) (UInt8*, UInt16);
#else
typedef void (* hal_cbUartRx_t) (Int16);
#endif
typedef void (* hal_cbUartEot_t) (void);

#ifdef __cplusplus
extern "C" {
#endif

//Calculate symbol period from baudrate
#define HAL_UART_DIVIDER_SYMBOL_PERIOD(baudrate)   (((16000000L / (UInt32)(baudrate)) - 4) / 8)
#define HAL_UART_SYMBOL_PERIOD(baudrate)   (((16000000L+(8*baudrate/2)) / (8*baudrate))-1)
void hal_InitUart(void);
void hal_UartStart (hal_cbUartRx_t cbRx, hal_cbUartGetTxData_t cbGetTxData, UInt16 symbolPeriod, UInt16 flags, UInt8 uart);

void hal_UartDisable(UInt8 uart);
void hal_UartEnable(UInt8 uart);
void hal_UartTxNewData(UInt8 uart);
void hal_UartWaitEndOfTransmission(UInt8 uart);
void hal_UartRegisterOneShotEndOfTxCb(hal_cbUartEot_t cbEot);

void hal_UartComStart( hal_cbUartRx_t cbRx, hal_cbUartGetTxData_t cbTx, UInt8 uart);
void hal_UartComStop(UInt8 uart);
void hal_UartSComStart( hal_cbUartRx_t cbRx, hal_cbUartGetTxData_t cbTx);
void hal_UartComFlush(UInt8 uart);
Bool hal_UartTxEnabled(UInt8 uart);
Bool hal_UartRxEnabled(UInt8 uart);
void hal_UartRxComFlush(UInt8 uart);
void hal_UartSetClockDivider(UInt8 Uart, UInt16 value);
UInt16 hal_UartGetClockDivider(UInt8 Uart);

#define HAVE_HAL_UART_FLUSH
#define HAVE_HAL_UART_RX_FLUSH

/*****************************************************************************
 *                    ucHal UART interface
 *****************************************************************************/

#define HAL_UART_COM_START(cbRx, cbTx) do { \
    hal_UartComStart(cbRx , cbTx, GP_BSP_UART_COM1); \
} while(false)

#define HAL_UART_COM_STOP()  hal_UartComStop(GP_BSP_UART_COM1)

#define HAL_UART_COM2_START(cbRx, cbTx) do { \
    hal_UartComStart(cbRx , cbTx, GP_BSP_UART_COM2); \
} while(false)
#define HAL_UART_COM2_STOP()  hal_UartComStop(GP_BSP_UART_COM2)


#define HAL_UART_COM_POWERDOWN() hal_UartDisable(GP_BSP_UART_COM1)
#define HAL_UART_COM_POWERUP()   hal_UartEnable(GP_BSP_UART_COM1)
#define HAL_UART_COM2_POWERDOWN() hal_UartDisable(GP_BSP_UART_COM2)
#define HAL_UART_COM2_POWERUP() hal_UartEnable(GP_BSP_UART_COM2)

#define HAL_UART_COM_TX_ENABLED()             hal_UartTxEnabled(GP_BSP_UART_COM1)
#define HAL_UART_COM_TX_NEW_DATA()            hal_UartTxNewData(GP_BSP_UART_COM1)
#define HAL_UART_COM_WAIT_END_OF_TX()         hal_UartWaitEndOfTransmission(GP_BSP_UART_COM1)
#define HAL_UART_COM_EXPECT_END_OF_TX(cbEOT)  hal_UartRegisterOneShotEndOfTxCb(cbEOT)

#define HAL_UART_COM2_TX_NEW_DATA()            hal_UartTxNewData(GP_BSP_UART_COM2)


/* SCOM = COM*/
#define HAL_UART_SCOM_START( cbRx , cbTx  ) do{ \
    hal_UartSComStart(cbRx , cbTx); \
}while(false)

#define HAL_UART_SCOM_POWERDOWN()       HAL_UART_COM_POWERDOWN()
#define HAL_UART_SCOM_POWERUP()         HAL_UART_COM_POWERUP()
#define HAL_UART_SCOM_TX_NEW_DATA()     HAL_UART_COM_TX_NEW_DATA()

#ifdef __cplusplus
}
#endif

#endif //_HAL_UART_H_
