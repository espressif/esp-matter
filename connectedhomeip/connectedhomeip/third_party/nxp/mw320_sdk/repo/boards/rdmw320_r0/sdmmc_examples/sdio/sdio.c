/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "fsl_sdio.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "sdmmc_config.h"
#include "fsl_device_registers.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/


/* define data buffer size */
#define DATA_BUFFER_SIZE (256U)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Card descriptor */
sdio_card_t g_sdio;

/* define the tuple list */
static const uint32_t g_funcTupleList[2U] = {
    SDIO_TPL_CODE_FUNCID,
    SDIO_TPL_CODE_FUNCE,
};

/* @brief decription about the read/write buffer
 * The address of the read/write buffer should align to the specific DMA data buffer address align value if
 * DMA transfer is used, otherwise the buffer address is not important.
 * At the same time buffer address/size should be aligned to the cache line size if cache is supported.
 */
/*! @brief Data written to the card */
SDK_ALIGN(uint8_t g_dataRead[DATA_BUFFER_SIZE], BOARD_SDMMC_DATA_BUFFER_ALIGN_SIZE);
/*! @brief Data read from the card */
SDK_ALIGN(uint8_t g_dataBlockRead[DATA_BUFFER_SIZE], BOARD_SDMMC_DATA_BUFFER_ALIGN_SIZE);
/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Main function
 */
int main(void)
{
    sdio_card_t *card = &g_sdio;

    BOARD_InitPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    BOARD_SDIO_Config(card, NULL, BOARD_SDMMC_SDIO_HOST_IRQ_PRIORITY, NULL);
    /*
     * Sdio case workaround to cover more wifi module,
     * Some wifi module may not support 1V8 voltage that SDIO3.0 required, but the wifi chip supports SDIO3.0, then the
     * the case will run failed, so the sdio case will specify the high speed timing mode here to avoid SDIO driver
     * perform SDIO3.0 timing probe.
     *
     * Note: If you are sure about the voltage configuration and the wifi module capability, you can comment out below
     * line.
     */
    card->currentTiming = kSD_TimingSDR25HighSpeedMode;

    PRINTF("SDIO card simple example.\r\n");

    /* SD host init function */
    if (SDIO_HostInit(card) != kStatus_Success)
    {
        PRINTF("\r\nSDIO host init fail\r\n");
        return -1;
    }

    PRINTF("\r\nPlease insert a card into board.\r\n");

    if (SDIO_PollingCardInsert(card, kSD_Inserted) == kStatus_Success)
    {
        PRINTF("\r\nCard inserted.\r\n");
        /* reset host once card re-plug in */
        SDIO_HostDoReset(card);
        /* power off card */
        SDIO_SetCardPower(card, false);
        /* power on the card */
        SDIO_SetCardPower(card, true);
    }
    else
    {
        PRINTF("\r\nCard detect fail.\r\n");
        return -1;
    }

    /* Init SDIO card. */
    if (kStatus_Success != SDIO_CardInit(card))
    {
        PRINTF("\r\nSDIO card init failed.\r\n");
        return kStatus_Fail;
    }

    if (card->ioTotalNumber > 0U)
    {
        /* read function capability through GetCardCapability function first */
        if (kStatus_Success != SDIO_GetCardCapability(card, kSDIO_FunctionNum1))
        {
            return kStatus_Fail;
        }

        PRINTF("\r\nRead function CIS, in direct way\r\n");
        if (kStatus_Success != SDIO_ReadCIS(card, kSDIO_FunctionNum1, g_funcTupleList, 2U))
        {
            return kStatus_Fail;
        }

        PRINTF("\r\nRead function CIS, in extended way, non-block mode, non-word aligned size\r\n");

        if (kStatus_Success != SDIO_IO_Read_Extended(card, kSDIO_FunctionNum0, card->ioFBR[0].ioPointerToCIS,
                                                     g_dataRead, 31U, SDIO_EXTEND_CMD_OP_CODE_MASK))
        {
            return kStatus_Fail;
        }
        PRINTF("\r\nRead function CIS, in extended way, block mode, non-word aligned size\r\n");

        /* set block size to a non-word aligned size for test */
        if (kStatus_Success != SDIO_SetBlockSize(card, kSDIO_FunctionNum0, 31U))
        {
            return kStatus_Fail;
        }

        if (kStatus_Success != SDIO_IO_Read_Extended(card, kSDIO_FunctionNum0, card->ioFBR[0].ioPointerToCIS,
                                                     g_dataBlockRead, 1U,
                                                     SDIO_EXTEND_CMD_OP_CODE_MASK | SDIO_EXTEND_CMD_BLOCK_MODE_MASK))
        {
            return kStatus_Fail;
        }

        if (memcmp(g_dataRead, g_dataBlockRead, 31U))
        {
            PRINTF("\r\nThe read content isn't consistent.\r\n");
        }
        else
        {
            PRINTF("\r\nThe read content is consistent.\r\n");
        }
    }

    SDIO_Deinit(card);

    while (true)
    {
    }
}
