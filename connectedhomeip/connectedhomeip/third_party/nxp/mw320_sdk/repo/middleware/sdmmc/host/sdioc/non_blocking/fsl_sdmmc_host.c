/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_sdmmc_host.h"
#include "fsl_sdmmc_common.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SDMMCHOST_TRANSFER_COMPLETE_TIMEOUT (~0U)
#define SDIOC_MAP_RESPONSE_TYPE(responseType)                                                              \
    ((responseType) == (uint32_t)kCARD_ResponseTypeNone ?                                                  \
         kSDIOC_CommandNoResponse :                                                                        \
         (responseType) == (uint32_t)kCARD_ResponseTypeR1 ?                                                \
         kSDIOC_CommandResponseLen48BitsCheckCrcIndex :                                                    \
         (responseType) == (uint32_t)kCARD_ResponseTypeR1b ?                                               \
         kSDIOC_CommandResponseLen48BitsWithCheckBusyCrcIndex :                                            \
         (responseType) == (uint32_t)kCARD_ResponseTypeR2 ?                                                \
         kSDIOC_CommandResponseLen136BitsCheckCrc :                                                        \
         (responseType) == (uint32_t)kCARD_ResponseTypeR3 ?                                                \
         kSDIOC_CommandResponseLen48Bits :                                                                 \
         (responseType) == (uint32_t)kCARD_ResponseTypeR4 ?                                                \
         kSDIOC_CommandResponseLen48Bits :                                                                 \
         (responseType) == (uint32_t)kCARD_ResponseTypeR5 ?                                                \
         kSDIOC_CommandResponseLen48BitsCheckCrcIndex :                                                    \
         (responseType) == (uint32_t)kCARD_ResponseTypeR5b ?                                               \
         kSDIOC_CommandResponseLen48BitsWithCheckBusyCrcIndex :                                            \
         (responseType) == (uint32_t)kCARD_ResponseTypeR6 ? kSDIOC_CommandResponseLen48BitsCheckCrcIndex : \
                                                            kSDIOC_CommandResponseLen48BitsCheckCrcIndex)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief SDMMCHOST detect card insert status by host controller.
 * @param base host base address.
 * @param userData user can register a application card insert callback through userData.
 */
static void SDMMCHOST_DetectCardInsertByHost(SDIOC_Type *base, void *userData);

/*!
 * @brief SDMMCHOST detect card remove status by host controller.
 * @param base host base address.
 * @param userData user can register a application card insert callback through userData.
 */
static void SDMMCHOST_DetectCardRemoveByHost(SDIOC_Type *base, void *userData);

/*!
 * @brief SDMMCHOST transfer complete callback.
 *
 * @param handle host handle.
 * @param status interrupt status.
 * @param userData user data.
 */
static void SDMMCHOST_TransferCallback(sdioc_handle_t *handle, status_t status, void *userData);

/*!
 * @brief SDMMCHOST error recovery.
 * @param base host base address.
 */
static void SDMMCHOST_ErrorRecovery(SDIOC_Type *base);
/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
static void SDMMCHOST_DetectCardInsertByHost(SDIOC_Type *base, void *userData)
{
    sd_detect_card_t *cd = NULL;

    SDMMC_OSAEventSet(&(((sdmmchost_t *)userData)->hostEvent), SDMMC_OSA_EVENT_CARD_INSERTED);
    SDMMC_OSAEventClear(&(((sdmmchost_t *)userData)->hostEvent), SDMMC_OSA_EVENT_CARD_REMOVED);

    /* disable card insert insterrupt */
    SDIOC_EnableInterruptSignal(base, kSDIOC_InterruptCardInsertedFlag, false);

    if (userData != NULL)
    {
        cd = (sd_detect_card_t *)(((sdmmchost_t *)userData)->cd);
        if (cd != NULL)
        {
            if (cd->callback != NULL)
            {
                cd->callback(true, cd->userData);
            }
        }
    }
}

static void SDMMCHOST_DetectCardRemoveByHost(SDIOC_Type *base, void *userData)
{
    sd_detect_card_t *cd = NULL;

    SDMMC_OSAEventSet(&(((sdmmchost_t *)userData)->hostEvent), SDMMC_OSA_EVENT_CARD_REMOVED);
    SDMMC_OSAEventClear(&(((sdmmchost_t *)userData)->hostEvent), SDMMC_OSA_EVENT_CARD_INSERTED);

    SDIOC_EnableInterruptSignal(base, kSDIOC_InterruptCardRemovedFlag, false);

    if (userData != NULL)
    {
        cd = (sd_detect_card_t *)(((sdmmchost_t *)userData)->cd);
        if (cd != NULL)
        {
            if (cd->callback != NULL)
            {
                cd->callback(false, cd->userData);
            }
        }
    }
}

static void SDMMCHOST_CardDetectCallback(sdioc_handle_t *handle, bool isCardInserted, void *userData)
{
    if (isCardInserted)
    {
        SDMMCHOST_DetectCardInsertByHost(handle->base, userData);
    }
    else
    {
        SDMMCHOST_DetectCardRemoveByHost(handle->base, userData);
    }
}

static void SDMMCHOST_CardInterrupt(sdioc_handle_t *handle, void *userData)
{
    sdio_card_int_t *cardInt = NULL;

    /* application callback */
    if (userData != NULL)
    {
        cardInt = ((sdmmchost_t *)userData)->cardInt;
        if ((cardInt != NULL) && (cardInt->cardInterrupt != NULL))
        {
            cardInt->cardInterrupt(cardInt->userData);
        }
    }
}

status_t SDMMCHOST_CardIntInit(sdmmchost_t *host, void *sdioInt)
{
    host->cardInt                         = sdioInt;
    host->handle.callback.cardIntCallback = SDMMCHOST_CardInterrupt;
    SDMMCHOST_EnableCardInt(host, true);

    return kStatus_Success;
}

status_t SDMMCHOST_CardDetectInit(sdmmchost_t *host, void *cd)
{
    SDIOC_Type *base       = host->base;
    sd_detect_card_t *sdCD = (sd_detect_card_t *)cd;
    if (cd == NULL)
    {
        return kStatus_Fail;
    }

    host->cd = cd;

    /* enable card detect interrupt */
    SDIOC_EnableInterruptStatus(base, kSDIOC_InterruptCardRemovedFlag, true);
    SDIOC_ClearInterruptStatus(base, kSDIOC_InterruptCardInsertedFlag | kSDIOC_InterruptCardRemovedFlag);

    if (SDMMCHOST_CardDetectStatus(host) == kSD_Inserted)
    {
        SDMMC_OSAEventSet(&(host->hostEvent), SDMMC_OSA_EVENT_CARD_INSERTED);
        /* notify application about the card insertion status */
        if (sdCD->callback)
        {
            sdCD->callback(true, sdCD->userData);
        }
    }
    else
    {
        SDMMC_OSAEventSet(&(host->hostEvent), SDMMC_OSA_EVENT_CARD_REMOVED);
        SDIOC_EnableInterruptStatus(base, kSDIOC_InterruptCardInsertedFlag, true);
        SDIOC_EnableInterruptSignal(base, kSDIOC_InterruptCardInsertedFlag, true);
    }

    return kStatus_Success;
}

uint32_t SDMMCHOST_CardDetectStatus(sdmmchost_t *host)
{
    uint32_t insertStatus = kSD_Removed;

    if ((SDIOC_GetPresentStatus(host->base) & kSDIOC_PresentStatusCardInserted) != 0U)
    {
        insertStatus = kSD_Inserted;
    }

    return insertStatus;
}

status_t SDMMCHOST_PollingCardDetectStatus(sdmmchost_t *host, uint32_t waitCardStatus, uint32_t timeout)
{
    assert(host != NULL);
    assert(host->cd != NULL);

    sd_detect_card_t *cd = host->cd;
    uint32_t event       = 0U;

    SDMMC_OSAEventGet(&(host->hostEvent), SDMMC_OSA_EVENT_CARD_INSERTED | SDMMC_OSA_EVENT_CARD_REMOVED, &event);
    if ((((event & SDMMC_OSA_EVENT_CARD_INSERTED) == SDMMC_OSA_EVENT_CARD_INSERTED) &&
         (waitCardStatus == kSD_Inserted)) ||
        (((event & SDMMC_OSA_EVENT_CARD_REMOVED) == SDMMC_OSA_EVENT_CARD_REMOVED) && (waitCardStatus == kSD_Removed)))
    {
        return kStatus_Success;
    }

    /* Wait card inserted. */
    do
    {
        if (SDMMC_OSAEventWait(&(host->hostEvent), SDMMC_OSA_EVENT_CARD_INSERTED | SDMMC_OSA_EVENT_CARD_REMOVED,
                               timeout, &event) != kStatus_Success)
        {
            return kStatus_Fail;
        }
        else
        {
            if ((waitCardStatus == kSD_Inserted) &&
                ((event & SDMMC_OSA_EVENT_CARD_INSERTED) == SDMMC_OSA_EVENT_CARD_INSERTED))
            {
                SDMMC_OSADelay(cd->cdDebounce_ms);
                SDMMC_OSAEventGet(&(host->hostEvent), SDMMC_OSA_EVENT_CARD_INSERTED, &event);
                if ((event & SDMMC_OSA_EVENT_CARD_INSERTED) == SDMMC_OSA_EVENT_CARD_INSERTED)
                {
                    break;
                }
            }

            if (((event & SDMMC_OSA_EVENT_CARD_REMOVED) == SDMMC_OSA_EVENT_CARD_REMOVED) &&
                (waitCardStatus == kSD_Removed))
            {
                break;
            }
        }
    } while (true);

    return kStatus_Success;
}

static void SDMMCHOST_TransferCallback(sdioc_handle_t *handle, status_t status, void *userData)
{
    uint32_t eventStatus = 0U;

    if (status == kStatus_SDIOC_TransferDataFail)
    {
        eventStatus = SDMMC_OSA_EVENT_TRANSFER_DATA_FAIL;
    }
    else if (status == kStatus_SDIOC_TransferDataSuccess)
    {
        eventStatus = SDMMC_OSA_EVENT_TRANSFER_DATA_SUCCESS;
    }
    else if ((status == kStatus_SDIOC_TransferCommandFail) || (status == kStatus_SDIOC_TransferCommandResponseError))
    {
        eventStatus = SDMMC_OSA_EVENT_TRANSFER_CMD_FAIL;
    }
    else
    {
        eventStatus = SDMMC_OSA_EVENT_TRANSFER_CMD_SUCCESS;
    }

    SDMMC_OSAEventSet(&(((sdmmchost_t *)userData)->hostEvent), eventStatus);
}

status_t SDMMCHOST_TransferFunction(sdmmchost_t *host, sdmmchost_transfer_t *content)
{
    status_t error               = kStatus_Success;
    uint32_t event               = 0U;
    sdioc_dma_config_t dmaConfig = {.dmaBufferBoundary = kSDIOC_DMABufferBoundary512K};

    content->command->responseType = (uint32_t)SDIOC_MAP_RESPONSE_TYPE(content->command->responseType);

    /* clear redundant transfer event flag */
    SDMMC_OSAEventClear(&(host->hostEvent), SDMMC_OSA_EVENT_TRANSFER_CMD_SUCCESS | SDMMC_OSA_EVENT_TRANSFER_CMD_FAIL |
                                                SDMMC_OSA_EVENT_TRANSFER_DATA_SUCCESS |
                                                SDMMC_OSA_EVENT_TRANSFER_DATA_FAIL);

    error = SDIOC_TransferDMANonBlocking(&host->handle, (sdioc_command_t *)content->command,
                                         (sdioc_data_t *)content->data, &dmaConfig);

    if (error == kStatus_Success)
    {
        /* wait command event */
        if ((kStatus_Fail ==
             SDMMC_OSAEventWait(&(host->hostEvent),
                                SDMMC_OSA_EVENT_TRANSFER_CMD_SUCCESS | SDMMC_OSA_EVENT_TRANSFER_CMD_FAIL |
                                    SDMMC_OSA_EVENT_TRANSFER_DATA_SUCCESS | SDMMC_OSA_EVENT_TRANSFER_DATA_FAIL,
                                SDMMCHOST_TRANSFER_COMPLETE_TIMEOUT, &event)) ||
            ((event & SDMMC_OSA_EVENT_TRANSFER_CMD_FAIL) != 0U))
        {
            error = kStatus_Fail;
        }
        else
        {
            if (content->data != NULL)
            {
                if ((event & SDMMC_OSA_EVENT_TRANSFER_DATA_SUCCESS) == 0U)
                {
                    if (((event & SDMMC_OSA_EVENT_TRANSFER_DATA_FAIL) != 0U) ||
                        (kStatus_Fail == SDMMC_OSAEventWait(
                                             &(host->hostEvent),
                                             SDMMC_OSA_EVENT_TRANSFER_DATA_SUCCESS | SDMMC_OSA_EVENT_TRANSFER_DATA_FAIL,
                                             SDMMCHOST_TRANSFER_COMPLETE_TIMEOUT, &event) ||
                         ((event & SDMMC_OSA_EVENT_TRANSFER_DATA_FAIL) != 0U)))
                    {
                        error = kStatus_Fail;
                    }
                }
            }
        }
    }
    else
    {
        error = kStatus_Fail;
        /* host error recovery */
        SDMMCHOST_ErrorRecovery(host->base);
    }

    return error;
}

static void SDMMCHOST_ErrorRecovery(SDIOC_Type *base)
{
    uint32_t status = 0U;
    /* get host present status */
    status = SDIOC_GetPresentStatus(base);
    /* check command inhibit status flag */
    if ((status & (uint32_t)kSDIOC_PresentStatusCommandInhibit) != 0U)
    {
        /* reset command line */
        SDIOC_ResetCommandLine(base);
    }
    /* check data inhibit status flag */
    if ((status & (uint32_t)kSDIOC_PresentStatusDataInhibit) != 0U)
    {
        /* reset data line */
        SDIOC_ResetDataLine(base);
    }
}

void SDMMCHOST_SetCardPower(sdmmchost_t *host, bool enable)
{
    SDIOC_EnableBusPower(host->base, enable);
}

status_t SDMMCHOST_Init(sdmmchost_t *host)
{
    if ((host == NULL) || (host->base == NULL))
    {
        return kStatus_InvalidArgument;
    }

    sdioc_handle_t *sdiocHandle = &(host->handle);
    sdioc_config_t config       = {.busVoltage = kSDIOC_BusVoltage_3V3, .dataTimeout = kSDIOC_DataTimeoutValue14};
    sdioc_callback_t callback   = {
        .transferCallback   = SDMMCHOST_TransferCallback,
        .cardIntCallback    = SDMMCHOST_CardInterrupt,
        .cardDetectCallback = SDMMCHOST_CardDetectCallback,
    };

    /* sdmmc osa init */
    SDMMC_OSAInit();

    /* Initializes SDIOC. */
    host->endianMode = kSDMMCHOST_EndianModeLittle;

    SDIOC_Init(host->base, &config);

    /* Create handle for SDIOC driver */
    SDIOC_TransferCreateHandle(host->base, sdiocHandle, &callback, host);

    /* Create transfer event. */
    if (kStatus_Success != SDMMC_OSAEventCreate(&(host->hostEvent)))
    {
        return kStatus_Fail;
    }

    return kStatus_Success;
}

uint32_t SDMMCHOST_SetCardClock(sdmmchost_t *host, uint32_t targetClock)
{
    if (targetClock > 50000000U)
    {
        return 0U;
    }

    if (targetClock <= 25000000U)
    {
        SDIOC_SetBusSpeed(host->base, kSDIOC_BusSpeedNormal);
    }
    else
    {
        SDIOC_SetBusSpeed(host->base, kSDIOC_BusSpeedHigh);
    }

    return targetClock;
}

void SDMMCHOST_Reset(sdmmchost_t *host)
{
    /* reserved for future */
}

void SDMMCHOST_SetCardBusWidth(sdmmchost_t *host, uint32_t dataBusWidth)
{
    SDIOC_SetDataBusWidth(host->base, dataBusWidth == kSDMMC_BusWdith1Bit ? kSDIOC_BusWidth1Bit : kSDIOC_BusWidth4Bit);
}

void SDMMCHOST_Deinit(sdmmchost_t *host)
{
    SDIOC_Deinit(host->base);
    SDMMC_OSAEventDestroy(&(host->hostEvent));
}
