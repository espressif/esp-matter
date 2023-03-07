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

#include "fsl_device_registers.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* I2C source clock */
#define EXAMPLE_I2C_SLAVE_BASEADDR I2C0

#define I2C_MASTER_SLAVE_ADDR_7BIT 0x7EU
#define I2C_DATA_LENGTH            34U

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

uint8_t g_slave_buff[I2C_DATA_LENGTH];

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Main function
 */
int main(void)
{
    i2c_slave_config_t slaveConfig;
    status_t status = kStatus_Success;
    uint8_t subaddress;

    BOARD_InitPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    PRINTF("\r\nI2C board2board polling example -- Slave transfer.\r\n\r\n");

    /*
     * slaveConfig->addressingMode        = kI2C_AddressMatch7bit;
     * slaveConfig->rxFifoWaterMark       = 0U;
     * slaveConfig->txFifoWaterMark       = 255U;
     * slaveConfig->dataHoldTime          = 0x1U;
     * slaveConfig->dataSetupTime         = 0x64U;
     * slaveConfig->ackGeneralCall        = true;
     * slaveConfig->glitchFilter          = 0x6U;
     * slaveConfig->glitchFilterHighSpeed = 0x2U;
     * slaveConfig->enable                = false;
     */
    I2C_SlaveGetDefaultConfig(&slaveConfig);

    slaveConfig.addressingMode = kI2C_AddressMatch7bit;
    slaveConfig.slaveAddress   = I2C_MASTER_SLAVE_ADDR_7BIT;
    slaveConfig.enable         = true;
    I2C_SlaveInit(EXAMPLE_I2C_SLAVE_BASEADDR, &slaveConfig);

    for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
    {
        g_slave_buff[i] = 0;
    }

    status = I2C_SlaveReadBlocking(EXAMPLE_I2C_SLAVE_BASEADDR, g_slave_buff, I2C_DATA_LENGTH);

    if (status != kStatus_Success)
    {
        PRINTF("\r\nError of I2C slave polling read.\r\n");
    }

    status = I2C_SlaveReadBlocking(EXAMPLE_I2C_SLAVE_BASEADDR, &subaddress, 1);

    if (status != kStatus_Success)
    {
        PRINTF("\r\nError of I2C slave polling read.\r\n");
    }

    status = I2C_SlaveWriteBlocking(EXAMPLE_I2C_SLAVE_BASEADDR, &g_slave_buff[2], g_slave_buff[1]);

    PRINTF("Slave received data :");

    for (uint32_t i = 0U; i < g_slave_buff[1]; i++)
    {
        if (i % 8 == 0)
        {
            PRINTF("\r\n");
        }
        PRINTF("0x%2x  ", g_slave_buff[2 + i]);
    }
    PRINTF("\r\n\r\n");

    if (status != kStatus_Success)
    {
        PRINTF("\r\nError of I2C slave polling write.\r\n");
    }

    PRINTF("\r\nEnd of I2C example .\r\n");

    while (1)
    {
    }
}
