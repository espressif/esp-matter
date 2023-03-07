/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_uart.h"

#include "fsl_device_registers.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_UART UART0

#define ECHO_BUFFER_LENGTH 8

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* UART user callback */
void UART_UserCallback(UART_Type *base, uart_handle_t *handle, status_t status, void *userData);

/*******************************************************************************
 * Variables
 ******************************************************************************/
uart_handle_t g_uartHandle;

uint8_t g_tipString[] =
    "Uart interrupt example\r\nBoard receives 8 characters then sends them out\r\nPlease input 8 characters at a "
    "time\r\nNow please input:\r\n";

uint8_t g_txBuffer[ECHO_BUFFER_LENGTH] = {0};
uint8_t g_rxBuffer[ECHO_BUFFER_LENGTH] = {0};
volatile bool rxBufferEmpty            = true;
volatile bool txBufferFull             = false;
volatile bool txOnGoing                = false;
volatile bool rxOnGoing                = false;

/*******************************************************************************
 * Code
 ******************************************************************************/
/* UART user callback */
void UART_UserCallback(UART_Type *base, uart_handle_t *handle, status_t status, void *userData)
{
    userData = userData;

    if (kStatus_UART_TxIdle == status)
    {
        txBufferFull = false;
        txOnGoing    = false;
    }

    if (kStatus_UART_RxIdle == status)
    {
        rxBufferEmpty = false;
        rxOnGoing     = false;
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
    uart_config_t config;
    uart_transfer_t xfer;
    uart_transfer_t sendXfer;
    uart_transfer_t receiveXfer;

    BOARD_InitPins();
    BOARD_InitBootClocks();

    /*
     * config.baudRate_Bps                   = 115200U;
     * config.parityMode                     = kUART_ParityDisabled;
     * config.enableStickyParity             = false;
     * config.dataBitCount                   = kUART_Databits8;
     * config.stopBitCount                   = kUART_Stopbits1;
     * config.fifoConfig.txFifoWatermark     = kUART_TxfifoEmpty;
     * config.fifoConfig.rxFifoWatermark     = kUART_RxfifoByte1;
     * config.fifoConfig.resetTxFifo         = false;
     * config.fifoConfig.resetRxFifo         = false;
     * config.fifoConfig.fifoEnable          = true;
     * config.autoBaudConfig.autoBaudEnable  = false;
     * config.autoBaudConfig.baudProgramType = kUART_BaudrateProgramAuto;
     * config.autoBaudConfig.baudCalType     = kUART_BaudrateCalTable;
     * config.infraredConfig.txIrEnable      = false;
     * config.infraredConfig.rxIrEnable      = false;
     * config.infraredConfig.txIrPolarity    = kUART_IrPositivePulse;
     * config.infraredConfig.rxIrPolarity    = kUART_IrPositivePulse;
     * config.infraredConfig.irPulseWidth    = kUART_IrPulsewidth3div16;
     * config.enableHighSpeed                = true;
     * config.enableLoop                     = false;
     * config.enableTxCTS                    = false;
     * config.enableRxRTS                    = false;
     * config.enable                         = true;
     */
    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
    config.enable       = true;

    UART_Init(DEMO_UART, &config, BOARD_DEBUG_UART_CLK_FREQ);
    UART_TransferCreateHandle(DEMO_UART, &g_uartHandle, UART_UserCallback, NULL);

    /* Send g_tipString out. */
    xfer.data     = g_tipString;
    xfer.dataSize = sizeof(g_tipString) - 1;
    txOnGoing     = true;
    UART_TransferSendNonBlocking(DEMO_UART, &g_uartHandle, &xfer);

    /* Wait send finished */
    while (txOnGoing)
    {
    }

    /* Start to echo. */
    sendXfer.data        = g_txBuffer;
    sendXfer.dataSize    = ECHO_BUFFER_LENGTH;
    receiveXfer.data     = g_rxBuffer;
    receiveXfer.dataSize = ECHO_BUFFER_LENGTH;

    while (1)
    {
        /* If RX is idle and g_rxBuffer is empty, start to read data to g_rxBuffer. */
        if ((!rxOnGoing) && rxBufferEmpty)
        {
            rxOnGoing = true;
            UART_TransferReceiveNonBlocking(DEMO_UART, &g_uartHandle, &receiveXfer, NULL);
        }

        /* If TX is idle and g_txBuffer is full, start to send data. */
        if ((!txOnGoing) && txBufferFull)
        {
            txOnGoing = true;
            UART_TransferSendNonBlocking(DEMO_UART, &g_uartHandle, &sendXfer);
        }

        /* If g_txBuffer is empty and g_rxBuffer is full, copy g_rxBuffer to g_txBuffer. */
        if ((!rxBufferEmpty) && (!txBufferFull))
        {
            memcpy(g_txBuffer, g_rxBuffer, ECHO_BUFFER_LENGTH);
            rxBufferEmpty = true;
            txBufferFull  = true;
        }
    }
}
