/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *   Hardware Abstraction Layer for the SPI master interface.
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

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_HALCORTEXM4

#include "hal.h"
#include "gpHal_reg.h"
#include "gpAssert.h"
#include "gpBsp.h"
#include "gpLog.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define HAL_SPI_MAX_CLK_FREQ  32000000UL

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

static void halSPI_FlushRx(void)
{
    while (GP_WB_READ_SPI_M_UNMASKED_RX_NOT_EMPTY_INTERRUPT())
    {
        GP_WB_READ_SPI_M_RX_DATA_0();
    }
    GP_WB_SPI_M_CLR_RX_OVERRUN_INTERRUPT();
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void hal_InitSPI(UInt32 frequency, UInt8 mode, Bool lsbFirst)
{
    UInt8 clkSetting;
    UInt32 maxFreq;

    // Determine max frequency, if BBPLL is running this equals 32MHz, otherwise the crystal oscilator supports 16MHz
    if (GP_WB_READ_STANDBY_ALWAYS_USE_FAST_CLOCK_AS_SOURCE())
    {
        maxFreq = HAL_SPI_MAX_CLK_FREQ;
    }
    else
    {
        maxFreq = HAL_SPI_MAX_CLK_FREQ / 2;
    }

    //Only factor 2 frequencies available: 32MHz, 16MHz, 8MHz, ... freq will be rounded down to the nearest
    GP_ASSERT_DEV_EXT(frequency <= (maxFreq));
    GP_ASSERT_DEV_EXT(frequency >= (maxFreq >> 7)); //250 kHz (BBPLL) or 125 kHz (Crystal)

    // Set MSPI pin mapping.
    GP_BSP_MSPI_MISO_INIT();
    GP_BSP_MSPI_MOSI_INIT();
    GP_BSP_MSPI_SCLK_INIT();
    // SSN pin not configured here - it is explicitly controlled by software

    // Enable interal pull-up on MISO line.
    hal_gpioModePU(GP_BSP_MSPI_MISO_GPIO, true);

    //Set SPI clk speed
    clkSetting = 0;
    while(frequency < maxFreq)
    {
        frequency *= 2;
        clkSetting++;
    }

    GP_LOG_PRINTF("Setting clk divider to %u with source freq = %lu", 0, clkSetting, maxFreq);

    // BBPLL: 32MHz / 2**sclk_freq
    // Crystal: 16MHz / 2**sclk_freq
    GP_WB_WRITE_SPI_M_SCLK_FREQ(clkSetting);

    //Configure SPI settings
    GP_WB_WRITE_SPI_M_DATA_BITS(8-1);
    GP_WB_WRITE_SPI_M_LSB_FIRST(lsbFirst);
    GP_WB_WRITE_SPI_M_MODE(mode);

    //Flush any pending Rx bytes
    halSPI_FlushRx();
}

void hal_DeInitSPI(void)
{
    GP_BSP_MSPI_MISO_DEINIT();
    GP_BSP_MSPI_MOSI_DEINIT();
    GP_BSP_MSPI_SCLK_DEINIT();
}


UInt8 hal_WriteReadSPI(UInt8 byte)
{
    GP_WB_WRITE_SPI_M_TX_DATA_0(byte);
    // Wait until Rx fifo not empty
    while (!GP_WB_READ_SPI_M_UNMASKED_RX_NOT_EMPTY_INTERRUPT());

    // Return Rx fifo content
    return GP_WB_READ_SPI_M_RX_DATA_0();
}
