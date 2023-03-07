/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*  Standard C Included Files */
#include <string.h>
/*  SDK Included Files */
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_i2c.h"
#include "fsl_i2c_dmac.h"

#include "fsl_device_registers.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* I2C source clock */
#define I2C_MASTER_CLK_FREQ         CLOCK_GetI2cClkFreq()
#define EXAMPLE_I2C_MASTER_BASEADDR I2C0
#define EXAMPLE_DMAC                DMAC
#define I2C_DMA_TX_CHANNEL          kDMAC_Channel0
#define I2C_DMA_RX_CHANNEL          kDMAC_Channel1
#define I2C_DMA_TX_SOURCE           kDMAC_PeriphNum_I2C0_Tx
#define I2C_DMA_RX_SOURCE           kDMAC_PeriphNum_I2C0_Rx

#define I2C_MASTER_SLAVE_ADDR_7BIT 0x7EU
#define I2C_BAUDRATE               100000U
#define I2C_DATA_LENGTH            33U

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

uint8_t g_master_txBuff[I2C_DATA_LENGTH];
uint8_t g_master_rxBuff[I2C_DATA_LENGTH];
i2c_master_dmac_handle_t g_m_dmac_handle;
dmac_handle_t dmacTxHandle;
dmac_handle_t dmacRxHandle;
volatile bool g_MasterCompletionFlag = false;

/*******************************************************************************
 * Code
 ******************************************************************************/

static void i2c_master_callback(I2C_Type *base, i2c_master_dmac_handle_t *handle, status_t status, void *userData)
{
    /* Signal transfer success when received success status. */
    if (status == kStatus_Success)
    {
        g_MasterCompletionFlag = true;
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
    i2c_master_config_t masterConfig;
    i2c_master_transfer_t masterXfer;

    BOARD_InitPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    PRINTF("\r\nI2C board2board DMAC example -- Master transfer.\r\n");

    /* Set up i2c master to send data to slave*/
    /* First data in txBuff is data length of the transmiting data. */
    g_master_txBuff[0] = I2C_DATA_LENGTH - 1U;
    for (uint32_t i = 1U; i < I2C_DATA_LENGTH; i++)
    {
        g_master_txBuff[i] = i - 1;
    }

    PRINTF("Master will send data :");
    for (uint32_t i = 0U; i < I2C_DATA_LENGTH - 1U; i++)
    {
        if (i % 8 == 0)
        {
            PRINTF("\r\n");
        }
        PRINTF("0x%2x  ", g_master_txBuff[i + 1]);
    }
    PRINTF("\r\n\r\n");

    /* Init I2C master */
    /*
     * masterConfig->speed                      = kI2C_MasterSpeedStandard;
     * masterConfig->addressingMode             = kI2C_AddressMatch7bit;
     * masterConfig->highSpeedMasterCodeAddress = 0x1U;
     * masterConfig->rxFifoWaterMark            = 0U;
     * masterConfig->txFifoWaterMark            = 8U;
     * masterConfig->dataHoldTime               = 0x1U;
     * masterConfig->dataSetupTime              = 0x64U;
     * masterConfig->glitchFilter               = 0x6U;
     * masterConfig->glitchFilterHighSpeed      = 0x2U;
     * masterConfig->enable                     = false;
     */
    I2C_MasterGetDefaultConfig(&masterConfig);
    masterConfig.speed = kI2C_MasterSpeedStandard;
    I2C_MasterInit(EXAMPLE_I2C_MASTER_BASEADDR, &masterConfig, I2C_MASTER_CLK_FREQ);

    /* Init DMAC */
    /*Init DMAC for example*/
    dmac_config_t dmacConfig;
    DMAC_GetDefaultConfig(&dmacConfig);
    DMAC_Init(EXAMPLE_DMAC, &dmacConfig);

    /* Set transfer configuration */
    /* subAddress = 0x01, data = g_master_txBuff - write to slave.
      start + slaveaddress(w) + subAddress + length of data buffer + data buffer + stop*/
    memset(&masterXfer, 0, sizeof(masterXfer));
    uint8_t deviceAddress     = 0x01U;
    masterXfer.slaveAddress   = I2C_MASTER_SLAVE_ADDR_7BIT;
    masterXfer.direction      = kI2C_Write;
    masterXfer.subaddress     = (uint32_t)deviceAddress;
    masterXfer.subaddressSize = 1;
    masterXfer.data           = g_master_txBuff;
    masterXfer.dataSize       = I2C_DATA_LENGTH;

    /* Create DMAC handle for I2C tx/rx and attach the I2C tx/rx source to channel */
    DMAC_SetPeripheralNumber(EXAMPLE_DMAC, I2C_DMA_RX_CHANNEL, I2C_DMA_RX_SOURCE);
    DMAC_TransferCreateHandle(EXAMPLE_DMAC, &dmacRxHandle, I2C_DMA_RX_CHANNEL);
    DMAC_SetPeripheralNumber(EXAMPLE_DMAC, I2C_DMA_TX_CHANNEL, I2C_DMA_TX_SOURCE);
    DMAC_TransferCreateHandle(EXAMPLE_DMAC, &dmacTxHandle, I2C_DMA_TX_CHANNEL);

    /* Initiate transfer */
    I2C_MasterTransferCreateHandleDMAC(EXAMPLE_I2C_MASTER_BASEADDR, &g_m_dmac_handle, i2c_master_callback, NULL,
                                       &dmacTxHandle, &dmacRxHandle);
    I2C_MasterTransferDMAC(EXAMPLE_I2C_MASTER_BASEADDR, &g_m_dmac_handle, &masterXfer);

    /*  Wait for transfer completed. */
    while (!g_MasterCompletionFlag)
    {
    }
    g_MasterCompletionFlag = false;

    PRINTF("Receive sent data from slave :");

    /* subAddress = 0x01, data = g_master_rxBuff - read from slave.
      start + slaveaddress(w) + subAddress + repeated start + slaveaddress(r) + rx data buffer + stop */
    masterXfer.slaveAddress   = I2C_MASTER_SLAVE_ADDR_7BIT;
    masterXfer.direction      = kI2C_Read;
    masterXfer.subaddress     = (uint32_t)deviceAddress;
    masterXfer.subaddressSize = 1;
    masterXfer.data           = g_master_rxBuff;
    masterXfer.dataSize       = I2C_DATA_LENGTH - 1;

    I2C_MasterTransferDMAC(EXAMPLE_I2C_MASTER_BASEADDR, &g_m_dmac_handle, &masterXfer);

    /*  Reset master completion flag to false. */
    g_MasterCompletionFlag = false;

    /*  Wait for transfer completed. */
    while (!g_MasterCompletionFlag)
    {
    }
    g_MasterCompletionFlag = false;

    for (uint32_t i = 0U; i < I2C_DATA_LENGTH - 1; i++)
    {
        if (i % 8 == 0)
        {
            PRINTF("\r\n");
        }
        PRINTF("0x%2x  ", g_master_rxBuff[i]);
    }
    PRINTF("\r\n\r\n");

    /* Transfer completed. Check the data.*/
    for (uint32_t i = 0U; i < I2C_DATA_LENGTH - 1; i++)
    {
        if (g_master_rxBuff[i] != g_master_txBuff[i + 1])
        {
            PRINTF("\r\nError occurred in the transfer ! \r\n");
            break;
        }
    }

    PRINTF("\r\nEnd of I2C example .\r\n");
    while (1)
    {
    }
}
