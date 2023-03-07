/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "qspi_nor_flash_ops.h"
#include "fsl_flashc.h"
#include "app.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static uint8_t writeBuff[FLASH_PAGE_SIZE]; /* Write buffer */
static uint8_t readBuff[FLASH_PAGE_SIZE];  /* Read buffer */

/*******************************************************************************
 * Code
 ******************************************************************************/


/* Use QSPI polling way to program serial flash */
void qspi_polling(void)
{
    status_t status       = kStatus_Fail;
    uint32_t i            = 0x00U;
    uint32_t err          = 0x00U;
    uint32_t address      = 0x00U;
    uint32_t sectorNumber = 0x00U;
    uint32_t jedecID      = 0x00U;

    /*Reset program address. */
    address      = FLASH_ERASE_ADDR_OFFSET;
    sectorNumber = FLASH_ERASE_ADDR_OFFSET / FLASH_SECTOR_SIZE;

    FLASH_ResetFastReadQuad(EXAMPLE_QSPI);

    /*Get JEDEC ID. */
    jedecID = FLASH_GetJEDECID(EXAMPLE_QSPI);

    status = FLASH_SetConfig(jedecID);
    if (kStatus_Success != status)
    {
        err++;
    }

    status = FLASH_SectorErase(EXAMPLE_QSPI, sectorNumber);

    if (kStatus_Success == status)
    {
        /* Check erased or not. */
        status = FLASH_IsSectorBlank(EXAMPLE_QSPI, sectorNumber);
        if (kStatus_Success != status)
        {
            err++;
        }
    }
    else
    {
        err++;
    }

    status = FLASH_PageProgram(EXAMPLE_QSPI, kFLASH_ProgramQuad, address, writeBuff, FLASH_PAGE_SIZE);

    /* Program pages in a sector */
    for (i = 0; i < FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE; i++)
    {
        status = FLASH_PageProgram(EXAMPLE_QSPI, kFLASH_ProgramNormal, address, writeBuff, FLASH_PAGE_SIZE);
        if (kStatus_Success != status)
        {
            err++;
        }
        address += FLASH_PAGE_SIZE;
    }

    address -= FLASH_SECTOR_SIZE;

    if (FLASH_PAGE_SIZE != FLASH_Read(EXAMPLE_QSPI, kFLASH_FastRead, address, readBuff, FLASH_PAGE_SIZE))
    {
        err++;
    }

    /* Switch back to FLASHC PAD from QSPI PAD. */
    FLASHC_EnableFLASHCPad(EXAMPLE_FLASHC, kFLASHC_HardwareCmdFastReadQuadIO, jedecID);

    PRINTF("JEDEC ID is 0x%x !\r\n", jedecID);

    for (i = 0; i < FLASH_PAGE_SIZE; i++)
    {
        if (readBuff[i] != writeBuff[i])
        {
            PRINTF("The data in %d is wrong!!\r\n", i);
            PRINTF("The flash value in %d is %d\r\n", i, (address + i), readBuff[i]);
            err++;
        }
    }

    if (err == 0)
    {
        PRINTF("Program through QSPI polling succeed!\r\n");
    }
}

int main(void)
{
    uint32_t i      = 0U;
    status_t status = kStatus_Fail;

    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    PRINTF("QSPI example started!\r\n");

    /* Switch back to QSPI PAD from FLASHC PAD. */
    status = FLASHC_EnableQSPIPad(EXAMPLE_FLASHC);
    if (status != kStatus_Success)
    {
        return -1;
    }

    /* Initialize QSPI. */
    FLASH_Init(EXAMPLE_QSPI);

    /*Initialize data buffer */
    for (i = 0; i < FLASH_PAGE_SIZE; i++)
    {
        writeBuff[i] = i;
        readBuff[i]  = 0x00U;
    }

    /* Use polling to program flash. */
    qspi_polling();

    while (1)
    {
    }
}
