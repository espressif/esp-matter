/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_sdioc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Typedef for interrupt handler. */
typedef void (*sdioc_isr_t)(sdioc_handle_t *handle);
/*! @brief check flag avalibility */
#define IS_SDIOC_FLAG_SET(reg, flag) (((reg) & ((uint32_t)flag)) != 0UL)
/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief SDIOC base pointer array */
static SDIOC_Type *const s_sdiocBase[] = SDIOC_BASE_PTRS;

/*! @brief SDIOC internal handle pointer array */
static sdioc_handle_t *s_sdiocHandle[ARRAY_SIZE(s_sdiocBase)] = {NULL};

/*! @brief SDIOC IRQ name array */
static const IRQn_Type s_sdiocIRQ[] = SDIOC_IRQS;

/* SDIOC ISR for transactional APIs. */
static sdioc_isr_t s_sdiocIsr;
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t SDIOC_GetInstance(SDIOC_Type *base)
{
    uint8_t instance = 0;

    while ((instance < ARRAY_SIZE(s_sdiocBase)) && (s_sdiocBase[instance] != base))
    {
        instance++;
    }

    assert(instance < ARRAY_SIZE(s_sdiocBase));

    return instance;
}

/*!
 * brief Transfer parameters command/data/dma configurations.
 *
 * param base SDIOC base address.
 * param command Transfer command pointer, application should not pass NULL pointer to this parameter.
 * param data Transfer data pointer, application can pass NULL when send command only.
 * param dmaConfig Internal dma configurations, application can pass NULL when dma is not used.
 *
 * retval #kStatus_InvalidArgument Argument is invalid.
 * retval #kStatus_Success Operate successfully.
 */

status_t SDIOC_SetTransferConfig(SDIOC_Type *base,
                                 sdioc_command_t *command,
                                 sdioc_data_t *data,
                                 sdioc_dma_config_t *dmaConfig)
{
    uint32_t xferReg       = 0U;
    uint32_t blockCountReg = 0U;
    status_t error         = kStatus_Success;

    if (command == NULL)
    {
        error = kStatus_InvalidArgument;
    }
    else
    {
        /* command index/type/response type configuration */
        xferReg |= command->commandResponseType | SDIOC_MM4_CMD_XFRMD_CMD_TYPE(command->commandType) |
                   SDIOC_MM4_CMD_XFRMD_CMD_IDX(command->commandIndex);

        /* data configuration */
        if (data != NULL)
        {
            blockCountReg = base->MM4_BLK_CNTL & ~(SDIOC_MM4_BLK_CNTL_XFR_BLKSZ_MASK |
                                                   SDIOC_MM4_BLK_CNTL_DMA_BUFSZ_MASK | SDIOC_MM4_BLK_CNTL_BLK_CNT_MASK);
            xferReg |= SDIOC_MM4_CMD_XFRMD_DXFRDIR(data->dataTxAddr != NULL ? 0U : 1U);

            xferReg |= SDIOC_MM4_CMD_XFRMD_DPSEL_MASK;

            if (data->dataEnableAutoCommand12)
            {
                xferReg |= SDIOC_MM4_CMD_XFRMD_AUTOCMD12_MASK;
            }

            if (dmaConfig != NULL)
            {
                xferReg |= SDIOC_MM4_CMD_XFRMD_DMA_EN_MASK;
                blockCountReg |= SDIOC_MM4_BLK_CNTL_DMA_BUFSZ(dmaConfig->dmaBufferBoundary);
            }

            if (data->dataBlockCount > 1U)
            {
                xferReg |= SDIOC_MM4_CMD_XFRMD_MS_BLKSEL_MASK | SDIOC_MM4_CMD_XFRMD_BLKCNTEN_MASK;
            }

            blockCountReg |=
                SDIOC_MM4_BLK_CNTL_BLK_CNT(data->dataBlockCount) | SDIOC_MM4_BLK_CNTL_XFR_BLKSZ(data->dataBlockSize);

            base->MM4_SYSADDR = data->dataTxAddr != NULL ? (uint32_t)data->dataTxAddr : (uint32_t)data->dataRxAddr;
        }

        base->MM4_BLK_CNTL  = blockCountReg;
        base->MM4_ARG       = command->commandArgument;
        base->MM4_CMD_XFRMD = xferReg;
    }

    return error;
}

/*!
 * brief SDIOC module initialization function.
 *
 * Configures the SDIOC according to the user configuration.
 *
 * Example:
   code
   sdioc_config_t config;
   config.busVoltage = kSDIOC_BusVoltage_3V3;
   config.dataTimeout = kSDIOC_DataTimeoutValue14;
   SDIOC_Init(SDIOC, &config);
   endcode
 *
 * param base SDIOC peripheral base address.
 * param config SDIOC configuration information.
 * retval #kStatus_Success Operate successfully.
 */
void SDIOC_Init(SDIOC_Type *base, const sdioc_config_t *config)
{
    assert(config != NULL);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable SDIOC clock. */
    CLOCK_EnableClock(kCLOCK_Sdio);
    CLOCK_EnableClock(kCLOCK_SdioAhb);
#endif
    /* reset command and data line */
    SDIOC_ResetCommandLine(base);
    SDIOC_ResetDataLine(base);

    /* enable internal/card clock */
    SDIOC_EnableCardClock(base, true);
    SDIOC_EnableInternalClock(base, true);

    /* set sdio bus voltage */
    SDIOC_SetBusVoltage(base, config->busVoltage);
    /* set data timeout value */
    SDIOC_SetDataTimeout(base, config->dataTimeout);
}

/*!
 * brief Deinitializes the SDIOC.
 *
 * param base SDIOC peripheral base address.
 */
void SDIOC_Deinit(SDIOC_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Disable clock. */
    CLOCK_DisableClock(kCLOCK_Sdio);
    CLOCK_DisableClock(kCLOCK_SdioAhb);
#endif
}

static status_t SDIOC_ReceiveCommandResponse(SDIOC_Type *base, sdioc_command_t *command)
{
    assert(command != NULL);

    uint32_t response0 = SDIOC_ReadCommandResponse(base, kSDIOC_ResponseIndex0);
    uint32_t response1 = SDIOC_ReadCommandResponse(base, kSDIOC_ResponseIndex1);
    uint32_t response2 = SDIOC_ReadCommandResponse(base, kSDIOC_ResponseIndex2);

    if (command->commandResponseType != kSDIOC_CommandNoResponse)
    {
        command->commandResponse[0U] = response0;
        if (command->commandResponseType == kSDIOC_CommandResponseLen136BitsCheckCrc)
        {
            /* R3-R2-R1-R0(lowest 8 bit is invalid bit) has the same format as R2 format in SD specification document
            after removed internal CRC7 and end bit. */
            command->commandResponse[0U] <<= 8U;
            command->commandResponse[1U] = (response1 << 8U) | ((response0 & 0xFF000000U) >> 24U);
            command->commandResponse[2U] = (response2 << 8U) | ((response1 & 0xFF000000U) >> 24U);
            command->commandResponse[3U] =
                (SDIOC_ReadCommandResponse(base, kSDIOC_ResponseIndex3) << 8U) | ((response2 & 0xFF000000U) >> 24U);
        }
    }

    /* check response error flag */
    if ((command->commandResponseErrorFlags != 0U) &&
        ((command->commandResponseType == kSDIOC_CommandResponseLen48Bits) ||
         (command->commandResponseType == kSDIOC_CommandResponseLen48BitsCheckCrcIndex) ||
         (command->commandResponseType == kSDIOC_CommandResponseLen48BitsWithCheckBusyCrcIndex)))
    {
        if ((command->commandResponseErrorFlags & command->commandResponse[0U]) != 0U)
        {
            return kStatus_SDIOC_TransferCommandResponseError;
        }
    }

    return kStatus_Success;
}

/*!
 * brief Creates the SDIOC handle.
 *
 * param base SDIOC peripheral base address.
 * param handle SDIOC handle pointer.
 * param callback Structure pointer to contain all callback functions.
 * param userData Callback function parameter.
 */
void SDIOC_TransferCreateHandle(SDIOC_Type *base, sdioc_handle_t *handle, sdioc_callback_t *callback, void *userData)
{
    assert(handle != NULL);
    assert(callback != NULL);

    /* Zero the handle. */
    (void)memset(handle, 0, sizeof(*handle));

    handle->base = base;

    /* Set the callback. */
    handle->callback.transferCallback   = callback->transferCallback;
    handle->callback.cardIntCallback    = callback->cardIntCallback;
    handle->callback.cardDetectCallback = callback->cardDetectCallback;
    handle->userData                    = userData;

    /* Save the handle in global variables to support the double weak mechanism. */
    s_sdiocHandle[SDIOC_GetInstance(base)] = handle;

    /* save IRQ handler */
    s_sdiocIsr = SDIOC_TransferHandleIRQ;

    (void)EnableIRQ(s_sdiocIRQ[SDIOC_GetInstance(base)]);
}

/*!
 * brief Transfers the command/data using an interrupt and an asynchronous method.
 *
 * This function sends a command and data and returns immediately. It doesn't wait for the transfer to complete or
 * to encounter an error. The application must not call this API in multiple threads at the same time. Because of that
 * this API doesn't support the re-entry mechanism.
 *
 * This function transfer data through SDIOC internal DMA.
 *
 * note Call API @ref SDIOC_TransferCreateHandle when calling this API.
 *
 * param handle SDIOC handle.
 * param command Transfer command.
 * param data Transfer data.
 * param dmaConfig Internal dma configurations.
 * retval #kStatus_InvalidArgument Argument is invalid.
 * retval #kStatus_SDIOC_BusyTransferring Busy transferring.
 * retval #kStatus_Success Operate successfully.
 */
status_t SDIOC_TransferDMANonBlocking(sdioc_handle_t *handle,
                                      sdioc_command_t *command,
                                      sdioc_data_t *data,
                                      sdioc_dma_config_t *dmaConfig)
{
    status_t error = kStatus_Success;

    if ((handle == NULL) || (command == NULL) || (dmaConfig == NULL))
    {
        error = kStatus_InvalidArgument;
    }
    else
    {
        if ((SDIOC_GetPresentStatus(handle->base) & (uint32_t)kSDIOC_PresentStatusCommandInhibit) != 0U)
        {
            return kStatus_SDIOC_BusyTransferring;
        }

        handle->command = command;

        if (data == NULL)
        {
            SDIOC_ClearInterruptStatus(handle->base, kSDIOC_InterruptCommandFlag);
            SDIOC_EnableInterruptStatus(handle->base, kSDIOC_InterruptCommandFlag, true);
            SDIOC_EnableInterruptSignal(handle->base, kSDIOC_InterruptCommandFlag, true);
        }
        else
        {
            if ((SDIOC_GetPresentStatus(handle->base) & (uint32_t)kSDIOC_PresentStatusDataInhibit) != 0U)
            {
                return kStatus_SDIOC_BusyTransferring;
            }

            SDIOC_ClearInterruptStatus(handle->base, kSDIOC_InterruptCommandAndDataFlag);
            SDIOC_EnableInterruptStatus(handle->base, kSDIOC_InterruptCommandAndDataFlag, true);
            SDIOC_EnableInterruptSignal(handle->base, kSDIOC_InterruptCommandAndDataFlag, true);

            handle->data = data;
        }
    }

    /* transfer configurations */
    if (error == kStatus_Success)
    {
        error = SDIOC_SetTransferConfig(handle->base, command, data, dmaConfig);
    }

    return error;
}

static void SDIOC_TransferHandleCardDetect(sdioc_handle_t *handle, uint32_t interruptFlags)
{
    if (handle->callback.cardDetectCallback != NULL)
    {
        handle->callback.cardDetectCallback(
            handle, (interruptFlags & (uint32_t)kSDIOC_InterruptCardInsertedFlag) != 0U ? true : false,
            handle->userData);
    }
}

static void SDIOC_TransferHandleCommand(sdioc_handle_t *handle, uint32_t interruptFlags)
{
    status_t transferStatus = kStatus_SDIOC_TransferCommandSuccess;

    if (handle->command != NULL)
    {
        if ((interruptFlags & kSDIOC_InterruptCommandErrorFlag) != 0U)
        {
            transferStatus = kStatus_SDIOC_TransferCommandFail;
        }
        else
        {
            if (kStatus_Success != SDIOC_ReceiveCommandResponse(handle->base, handle->command))
            {
                transferStatus = kStatus_SDIOC_TransferCommandResponseError;
            }
        }
        if (handle->callback.transferCallback != NULL)
        {
            handle->callback.transferCallback(handle, transferStatus, handle->userData);
        }

        handle->command = NULL;
    }
    /* disable command interrupt signal */
    SDIOC_EnableInterruptSignal(handle->base, kSDIOC_InterruptCommandFlag, false);
}

static void SDIOC_TransferHandleData(sdioc_handle_t *handle, uint32_t interruptFlags)
{
    if (handle->data != NULL)
    {
        if (handle->callback.transferCallback != NULL)
        {
            handle->callback.transferCallback(handle,
                                              (interruptFlags & (uint32_t)kSDIOC_InterruptDataErrorFlag) != 0U ?
                                                  kStatus_SDIOC_TransferDataFail :
                                                  kStatus_SDIOC_TransferDataSuccess,
                                              handle->userData);
        }

        handle->data = NULL;
    }

    /* disable data interrupt signal */
    SDIOC_EnableInterruptSignal(handle->base, kSDIOC_InterruptDataFlag, false);
}

static void SDIOC_TransferHandleCardInterrupt(sdioc_handle_t *handle)
{
    if (handle->callback.cardIntCallback != NULL)
    {
        handle->callback.cardIntCallback(handle, handle->userData);
    }
}

/*!
 * brief IRQ handler for the SDIOC.
 *
 * This function deals with the IRQs on the given host controller.
 *
 * param handle SDIOC handle.
 */
void SDIOC_TransferHandleIRQ(sdioc_handle_t *handle)
{
    assert(handle != NULL);

    uint32_t interruptFlags = SDIOC_GetEnabledInterruptStatus(handle->base);
    SDIOC_ClearInterruptStatus(handle->base, interruptFlags);

    if (IS_SDIOC_FLAG_SET(interruptFlags, kSDIOC_InterruptCardDetectFlag))
    {
        SDIOC_TransferHandleCardDetect(handle, interruptFlags);
    }
    if (IS_SDIOC_FLAG_SET(interruptFlags, kSDIOC_InterruptCommandFlag))
    {
        SDIOC_TransferHandleCommand(handle, interruptFlags);
    }
    if (IS_SDIOC_FLAG_SET(interruptFlags, kSDIOC_InterruptDataFlag))
    {
        SDIOC_TransferHandleData(handle, interruptFlags);
    }
    if (IS_SDIOC_FLAG_SET(interruptFlags, kSDIOC_InterruptCardInterruptFlag))
    {
        SDIOC_TransferHandleCardInterrupt(handle);
    }
}

#ifdef SDIOC
void SDIO_DriverIRQHandler(void)
{
    s_sdiocIsr(s_sdiocHandle[0U]);
    SDK_ISR_EXIT_BARRIER;
}
#endif
