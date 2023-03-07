/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_uart_dmac.h"

#include "fsl_device_registers.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_UART           UART0
#define EXAMPLE_DMAC        DMAC
#define UART_DMA_RX_CHANNEL kDMAC_Channel1
#define UART_DMA_RX_SOURCE  kDMAC_PeriphNum_UART0_Rx
#define UART_DMA_TX_CHANNEL kDMAC_Channel0
#define UART_DMA_TX_SOURCE  kDMAC_PeriphNum_UART0_Tx
#define ECHO_BUFFER_LENGTH 8

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* UART user callback */
void UART_UserCallback(UART_Type *base, uart_dmac_handle_t *handle, status_t status, void *userData);

/*******************************************************************************
 * Variables
 ******************************************************************************/

uart_dmac_handle_t g_uartDmaHandle;
dmac_handle_t g_uartTxDmaHandle;
dmac_handle_t g_uartRxDmaHandle;
uint8_t g_tipString[] =
    "Uart DMAC example\r\nBoard receives 8 characters then sends them out\r\nPlease input 8 characters at a "
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
void UART_UserCallback(UART_Type *base, uart_dmac_handle_t *handle, status_t status, void *userData)
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

    /* Initialize the UART. */

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

    /* Init DMAC */
    dmac_config_t dmacConfig;
    DMAC_GetDefaultConfig(&dmacConfig);
    DMAC_Init(EXAMPLE_DMAC, &dmacConfig);

    /* Create DMAC handle for UART tx/rx and attach the UART tx/rx source to channel */
    DMAC_SetPeripheralNumber(EXAMPLE_DMAC, UART_DMA_RX_CHANNEL, UART_DMA_RX_SOURCE);
    DMAC_TransferCreateHandle(EXAMPLE_DMAC, &g_uartRxDmaHandle, UART_DMA_RX_CHANNEL);
    DMAC_SetPeripheralNumber(EXAMPLE_DMAC, UART_DMA_TX_CHANNEL, UART_DMA_TX_SOURCE);
    DMAC_TransferCreateHandle(EXAMPLE_DMAC, &g_uartTxDmaHandle, UART_DMA_TX_CHANNEL);

    /* Create UART DMAC handle. */
    UART_TransferCreateHandleDMAC(DEMO_UART, &g_uartDmaHandle, UART_UserCallback, NULL, &g_uartTxDmaHandle,
                                  &g_uartRxDmaHandle);

    /* Send g_tipString out. */
    xfer.data     = g_tipString;
    xfer.dataSize = sizeof(g_tipString) - 1;
    txOnGoing     = true;
    UART_TransferSendDMAC(DEMO_UART, &g_uartDmaHandle, &xfer);

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
            UART_TransferReceiveDMAC(DEMO_UART, &g_uartDmaHandle, &receiveXfer);
        }

        /* If TX is idle and g_txBuffer is full, start to send data. */
        if ((!txOnGoing) && txBufferFull)
        {
            txOnGoing = true;
            UART_TransferSendDMAC(DEMO_UART, &g_uartDmaHandle, &sendXfer);
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
