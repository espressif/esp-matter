/*
 * Copyright 2020,2021 NXP
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
#define DEMO_UART            UART0
#define DEMO_UART_IRQn       UART0_IRQn
#define DEMO_UART_IRQHandler UART0_IRQHandler
/*! @brief Ring buffer size (Unit: Byte). */
#define DEMO_RING_BUFFER_SIZE 16
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

uint8_t g_tipString[] =
    "Uart functional API interrupt example\r\nBoard receives characters then sends them out\r\nPlease input no more "
    "than FIFO depth of data at a time\r\nNow please input:\r\n";

/*******************************************************************************
 * Code
 ******************************************************************************/

void DEMO_UART_IRQHandler(void)
{
    uint8_t data;

    /* If new data arrived. */
    if ((((uint16_t)kUART_RxDataReadyInterruptFlag | (uint16_t)kUART_RxOverrunInterruptFlag) &
         UART_GetStatusFlags(DEMO_UART)) != 0U)
    {
        data = UART_ReadByte(DEMO_UART);
        UART_WriteByte(DEMO_UART, data);
    }
    SDK_ISR_EXIT_BARRIER;
}

/*!
 * @brief Main function
 */
int main(void)
{
    uart_config_t config;

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

    /* Send g_tipString out. */
    UART_WriteBlocking(DEMO_UART, g_tipString, sizeof(g_tipString) / sizeof(g_tipString[0]));

    /* Enable RX interrupt. */
    UART_EnableInterrupts(DEMO_UART,
                          (uint8_t)kUART_RxDataReadyInterruptEnable | (uint8_t)kUART_RxStatusInterruptEnable);
    EnableIRQ(DEMO_UART_IRQn);

    while (1)
    {
    }
}
