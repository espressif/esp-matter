/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "sdmmc_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * Variables
 ******************************************************************************/
/*!brief sdmmc dma buffer */
#if defined(SDIO_ENABLED) || defined(SD_ENABLED)
static sd_detect_card_t s_cd;
#endif
static sdmmchost_t s_host;
#ifdef SDIO_ENABLED
static sdio_card_int_t s_sdioInt;
#endif
/*******************************************************************************
 * Code
 ******************************************************************************/
#ifdef SDIO_ENABLED
uint32_t BOARD_SDIO_ClockConfig(void)
{
    uint32_t maxClock     = 50000000U;
    uint32_t currentClock = CLOCK_GetSdioClkFreq();

    if (currentClock > maxClock)
    {
        CLOCK_SetClkDiv(kCLOCK_DivSdio, currentClock / maxClock);
    }

    return CLOCK_GetSdioClkFreq();
}

void BOARD_SDCardDetectInit(sd_cd_t cd, void *userData)
{
    /* install card detect callback */
    s_cd.cdDebounce_ms = BOARD_SDMMC_SD_CARD_DETECT_DEBOUNCE_DELAY_MS;
    s_cd.type          = BOARD_SDMMC_SD_CD_TYPE;
    s_cd.callback      = cd;
    s_cd.userData      = userData;
}

void BOARD_SDIO_Config(void *card, sd_cd_t cd, uint32_t hostIRQPriority, sdio_int_t cardInt)
{
    assert(card);

    ((sdio_card_t *)card)->host                 = &s_host;
    ((sdio_card_t *)card)->host->base           = BOARD_SDMMC_SDIO_HOST_BASEADDR;
    ((sdio_card_t *)card)->host->sourceClock_HZ = BOARD_SDIO_ClockConfig();

    ((sdio_card_t *)card)->usrParam.cd = &s_cd;

    BOARD_SDCardDetectInit(cd, NULL);
    if (cardInt != NULL)
    {
        s_sdioInt.cardInterrupt                 = cardInt;
        ((sdio_card_t *)card)->usrParam.sdioInt = &s_sdioInt;
    }

    NVIC_SetPriority(BOARD_SDMMC_SDIO_HOST_IRQ, hostIRQPriority);
}
#endif
