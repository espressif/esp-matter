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

#define RX_RING_BUFFER_SIZE 20U
#define ECHO_BUFFER_SIZE    8U

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
    "UART RX ring buffer example\r\nSend back received data\r\nEcho every 8 bytes\r\nPlease input 8 characters at a "
    "time\r\n";
uint8_t g_rxRingBuffer[RX_RING_BUFFER_SIZE] = {0}; /* RX ring buffer. */

uint8_t g_rxBuffer[ECHO_BUFFER_SIZE] = {0}; /* Buffer for receive data to echo. */
uint8_t g_txBuffer[ECHO_BUFFER_SIZE] = {0}; /* Buffer for send data to echo. */
volatile bool rxBufferEmpty          = true;
volatile bool txBufferFull           = false;
volatile bool txOnGoing              = false;
volatile bool rxOnGoing              = false;

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
    size_t receivedBytes;
    uint32_t i;

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
    UART_TransferStartRingBuffer(DEMO_UART, &g_uartHandle, g_rxRingBuffer, RX_RING_BUFFER_SIZE);

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
    sendXfer.dataSize    = ECHO_BUFFER_SIZE;
    receiveXfer.data     = g_rxBuffer;
    receiveXfer.dataSize = ECHO_BUFFER_SIZE;

    while (1)
    {
        /* If g_txBuffer is empty and g_rxBuffer is full, copy g_rxBuffer to g_txBuffer. */
        if ((!rxBufferEmpty) && (!txBufferFull))
        {
            memcpy(g_txBuffer, g_rxBuffer, ECHO_BUFFER_SIZE);
            rxBufferEmpty = true;
            txBufferFull  = true;
        }

        /* If RX is idle and g_rxBuffer is empty, start to read data to g_rxBuffer. */
        if ((!rxOnGoing) && rxBufferEmpty)
        {
            rxOnGoing = true;
            UART_TransferReceiveNonBlocking(DEMO_UART, &g_uartHandle, &receiveXfer, &receivedBytes);

            if (ECHO_BUFFER_SIZE == receivedBytes)
            {
                rxBufferEmpty = false;
                rxOnGoing     = false;
            }
        }

        /* If TX is idle and g_txBuffer is full, start to send data. */
        if ((!txOnGoing) && txBufferFull)
        {
            txOnGoing = true;
            UART_TransferSendNonBlocking(DEMO_UART, &g_uartHandle, &sendXfer);
        }

        /* Delay some time, simulate the app is processing other things, input data save to ring buffer. */
        i = 0x10U;
        while (i--)
        {
            __NOP();
        }
    }
}
