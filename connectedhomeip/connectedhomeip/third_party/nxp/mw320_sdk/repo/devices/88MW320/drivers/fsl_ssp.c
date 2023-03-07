/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_ssp.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Typedef for interrupt handler. */
typedef void (*ssp_isr_t)(ssp_handle_t *handle);
/*! @brief check flag avalibility */
#define IS_SSP_FLAG_SET(reg, flag) (((reg) & ((uint32_t)flag)) != 0UL)
/*! @brief SSAP fifo depth */
#define SSP_FIFO_COUNT(bitWidth) ((bitWidth) == 16U ? 32U : 16U)
/*! @brief SSP transfer state */
/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief SSP base pointer array */
static SSP_Type *const s_sspBase[] = SSP_BASE_PTRS;

/*! @brief SSP internal handle pointer array */
static ssp_handle_t *s_sspHandle[ARRAY_SIZE(s_sspBase)][2];

/*! @brief SSP IRQ name array */
static const IRQn_Type s_sspIRQ[] = SSP_IRQS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/* Clock name array */
static const clock_ip_name_t s_sspClock[] = SSP_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/* SSP send ISR for transactional APIs. */
static ssp_isr_t s_sspSendIsr;
static ssp_isr_t s_sspReceiveIsr;
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief SSP write data fifo.
 *
 *
 * @param base SSP peripheral base address.
 * @param buffer write buffer pointer.
 * @param bitWidth data bitwidth.
 * @param size data size to write.
 */
static void SSP_WriteNonBlocking(SSP_Type *base, uint8_t *buffer, uint32_t bitWidth, uint32_t size);

/*!
 * @brief SSP read data fifo.
 *
 *
 * @param base SSP peripheral base address.
 * @param buffer read buffer pointer.
 * @param bitWidth data bitwidth.
 * @param size data size to read.
 */
static void SSP_ReadNonBlocking(SSP_Type *base, uint8_t *buffer, uint32_t bitWidth, uint32_t size);
/*******************************************************************************
 * Code
 ******************************************************************************/
uint32_t SSP_GetInstance(SSP_Type *base)
{
    uint8_t instance = 0;

    while ((instance < ARRAY_SIZE(s_sspBase)) && (s_sspBase[instance] != base))
    {
        instance++;
    }

    assert(instance < ARRAY_SIZE(s_sspBase));

    return instance;
}

static void SSP_WriteNonBlocking(SSP_Type *base, uint8_t *buffer, uint32_t bitWidth, uint32_t size)
{
    uint32_t i = 0, j = 0U;
    uint32_t bytesPerWord = bitWidth;
    uint32_t data         = 0;
    uint32_t temp         = 0;

    for (i = 0; i < size / bytesPerWord; i++)
    {
        for (j = 0; j < bytesPerWord; j++)
        {
            temp = (uint32_t)(*buffer);
            data |= (temp << (8U * j));
            buffer++;
        }
        SSP_WriteData(base, data);
        data = 0;
    }
}

static void SSP_ReadNonBlocking(SSP_Type *base, uint8_t *buffer, uint32_t bitWidth, uint32_t size)
{
    uint32_t i = 0, j = 0;
    uint32_t bytesPerWord = bitWidth;
    uint32_t data         = 0;

    for (i = 0; i < size / bytesPerWord; i++)
    {
        data = SSP_ReadData(base);
        for (j = 0; j < bytesPerWord; j++)
        {
            *buffer = (uint8_t)(data >> (8U * j)) & 0xFFU;
            buffer++;
        }
    }
}

/*!
 * brief SSP get default configuration.
 *
 * code
 *  config->enableRXHalfCycleDelay     = false;
 *  config->enableMasterClockReturn    = false;
 *  config->enableFifoPackingMode      = false;
 *  config->enableSlaveClockFreeRuning = false;
 *  config->txFifoWaterMark            = SSP_FIFO_COUNT(32) / 2U;
 *  config->rxFifoWaterMark            = SSP_FIFO_COUNT(32) / 2U;
 *  config->mode                       = kSSP_TransmitReceiveMode;
 *  config->format                     = kSSP_TransceiverFormatPSP;
 * endcode
 *
 * param config the pointer of ssp configuration structure.
 */
void SSP_GetDefaultConfig(ssp_config_t *config)
{
    assert(config != NULL);

    (void)memset(config, 0, sizeof(ssp_config_t));

    config->enableRXHalfCycleDelay     = false;
    config->enableMasterClockReturn    = false;
    config->enableFifoPackingMode      = false;
    config->enableSlaveClockFreeRuning = false;
    config->txFifoWaterMark            = SSP_FIFO_COUNT(32) / 2U;
    config->rxFifoWaterMark            = SSP_FIFO_COUNT(32) / 2U;
    config->mode                       = kSSP_TransmitReceiveMode;
    config->format                     = kSSP_TransceiverFormatPSP;
}

/*!
 * brief SSP module initialization function.
 *
 * Configures the SSP according to the user configuration.
 *
 * param base SSP peripheral base address.
 * param config SSP configuration information.
 */
void SSP_Init(SSP_Type *base, const ssp_config_t *config)
{
    assert(config != NULL);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable SSP clock. */
    CLOCK_EnableClock(s_sspClock[SSP_GetInstance(base)]);
#endif

    /* clear bit error/tx underrun/rx overrun status */
    base->SSP_SSSR |= SSP_SSP_SSSR_TUR_MASK | SSP_SSP_SSITR_TROR_MASK | SSP_SSP_SSSR_BCE_MASK;
    /* SSP configuration */
    base->SSP_SSCR0 = SSP_SSP_SSCR0_FRF(config->format) | SSP_SSP_SSCR0_MCRT(config->enableMasterClockReturn) |
                      SSP_SSP_SSCR0_RHCD(config->enableRXHalfCycleDelay) |
                      SSP_SSP_SSCR0_FPCKE(config->enableFifoPackingMode);

    base->SSP_SSCR1 = SSP_SSP_SSCR1_RFT(config->rxFifoWaterMark - 1UL) |
                      SSP_SSP_SSCR1_TFT(config->txFifoWaterMark - 1UL) | SSP_SSP_SSCR1_RWOT(config->mode) |
                      SSP_SSP_SSCR1_SCFR(config->enableSlaveClockFreeRuning);

    if (config->transceiverConfig != NULL)
    {
        if (config->format == kSSP_TransceiverFormatSPI)
        {
            SSP_SetSPIModeConfig(base, (ssp_spi_mode_config_t *)config->transceiverConfig);
        }
        else
        {
            SSP_SetPSPModeConfig(base, (ssp_psp_mode_config_t *)config->transceiverConfig);
        }
    }
}

/*!
 * brief Deinitializes the SSP.
 *
 * param base SSP peripheral base address.
 */
void SSP_Deinit(SSP_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Disable clock. */
    CLOCK_DisableClock(s_sspClock[SSP_GetInstance(base)]);
#endif
}

/*!
 * brief SSP get default SPI mode configuration.
 *
 * The default SSP SPI mode configurations:
 * code
 *  config->spiClockPolarity  = kSSP_ClockPolarityLow;
 *  config->spiClockPhase     = kSSP_SpiClkPhase0;
 *  config->spiClockDirection = kSSP_ClockDirectionMaster;
 *  config->dataWidth         = kSSP_DataSize8Bit;
 *  config->txdIdleState      = kSSP_TxdIdleStateIsNot3Stated;
 *  config->txdLastPhaseState = kSSP_TxdLastPhase3StatedHalfOfLSBClockCycle;
 * endcode
 *
 * param config the pointer of ssp configuration structure.
 */
void SSP_GetDefaultSPIModeConfig(ssp_spi_mode_config_t *config)
{
    assert(config != NULL);

    config->spiClockPolarity  = kSSP_ClockPolarityLow;
    config->spiClockPhase     = kSSP_SpiClkPhase0;
    config->spiClockDirection = kSSP_ClockDirectionMaster;
    config->dataWidth         = kSSP_DataSize8Bit;
    config->txdIdleState      = kSSP_TxdIdleStateIsNot3Stated;
    config->txdLastPhaseState = kSSP_TxdLastPhase3StatedHalfOfLSBClockCycle;
}

/*!
 * brief SSP set SPI mode configuration.
 *
 * param base SSP peripheral base address.
 * param config the pointer of ssp spi mode configuration structure.
 */
void SSP_SetSPIModeConfig(SSP_Type *base, const ssp_spi_mode_config_t *config)
{
    assert(config != NULL);

    uint32_t sscr = base->SSP_SSCR1;

    sscr &= ~(SSP_SSP_SSCR1_SCLKDIR_MASK | SSP_SSP_SSCR1_SFRMDIR_MASK | SSP_SSP_SSCR1_SPH_MASK |
              SSP_SSP_SSCR1_SPO_MASK | SSP_SSP_SSCR1_TTELP_MASK | SSP_SSP_SSCR1_TTE_MASK);

    sscr |= SSP_SSP_SSCR1_SCLKDIR(config->spiClockDirection) | SSP_SSP_SSCR1_SFRMDIR(config->spiClockDirection) |
            SSP_SSP_SSCR1_SPH(config->spiClockPhase) | SSP_SSP_SSCR1_SPO(config->spiClockPolarity) |
            SSP_SSP_SSCR1_TTELP(config->txdLastPhaseState) | SSP_SSP_SSCR1_TTE(config->txdIdleState);

    base->SSP_SSCR1 = sscr;

    sscr = base->SSP_SSCR0;
    sscr &= ~(SSP_SSP_SSCR0_DSS_MASK | SSP_SSP_SSCR0_EDSS_MASK);
    sscr |= (uint32_t)config->dataWidth;
    base->SSP_SSCR0 = sscr;
}

/*!
 * brief SSP get default PSP mode configuration.
 *
 * The default SSP PSP mode configurations are 16bit, 2 slots, i2s protocol.
 * code
 *  config->bitClockDirection = kSSP_ClockDirectionMaster;
 *  config->bitClockMode      = kSPP_BitClockIdleLowDrivenFallingSampleRising;
 *  config->frameClockDirection = kSSP_ClockDirectionMaster;
 *  config->frameClockPolarity  = kSSP_ClockPolarityLow;
 *  config->frameRelativeTiming = kSSP_FrameSyncAssertWithTheLsbOfThePreviousFrame;
 *  config->txdIdleState        = kSSP_TxdIdleStateIsNot3Stated;
 *  config->txdLastPhaseState   = kSSP_TxdLastPhase3StatedOnLSBEndClockEdge;
 *  config->txdEndTransferState = kSSP_TxdEndTransferState0;
 *  config->dataWidth   = kSSP_DataSize16Bit;
 *  config->frameLength = 16U;
 *  config->enableNetWorkMode = true;
 *  config->frameSlotsNumber  = 2U;
 *  config->txActiveSlotMask  = 3U;
 *  config->rxActiveSlotMask  = 3U;
 *  config->dummyStartDelay = 0U;
 *  config->dummyStopDelay  = 0U;
 *  config->startDelay      = 0U;
 *  config->frameDelay      = 0U;
 * endcode
 *
 * param config the pointer of PSP configuration structure.
 */
void SSP_GetDefaultPSPModeConfig(ssp_psp_mode_config_t *config)
{
    assert(config != NULL);

    config->bitClockDirection = kSSP_ClockDirectionMaster;
    config->bitClockMode      = kSPP_BitClockIdleLowDrivenFallingSampleRising;

    config->frameClockDirection = kSSP_ClockDirectionMaster;
    config->frameClockPolarity  = kSSP_ClockPolarityLow;
    config->frameRelativeTiming = kSSP_FrameSyncAssertWithTheLsbOfThePreviousFrame;

    config->txdIdleState        = kSSP_TxdIdleStateIsNot3Stated;
    config->txdLastPhaseState   = kSSP_TxdLastPhase3StatedOnLSBEndClockEdge;
    config->txdEndTransferState = kSSP_TxdEndTransferState0;

    config->dataWidth   = kSSP_DataSize16Bit;
    config->frameLength = 16U;

    config->enableNetWorkMode = true;
    config->frameSlotsNumber  = 2U;
    config->txActiveSlotMask  = 3U;
    config->rxActiveSlotMask  = 3U;
    /* below configuration should be ignored when network mode enabled */
    config->dummyStartDelay = 0U;
    config->dummyStopDelay  = 0U;
    config->startDelay      = 0U;
    config->frameDelay      = 0U;
}

/*!
 * brief SSP set PSP mode configuration.
 *
 * param base SSP peripheral base address.
 * param config the pointer of ssp psp mode configuration structure.
 */
void SSP_SetPSPModeConfig(SSP_Type *base, const ssp_psp_mode_config_t *config)
{
    assert(config != NULL);
    assert(config->frameSlotsNumber != 0U);

    uint32_t sscr = base->SSP_SSCR1, sspsp = base->SSP_SSPSP;

    sscr &=
        ~(SSP_SSP_SSCR1_SCLKDIR_MASK | SSP_SSP_SSCR1_SFRMDIR_MASK | SSP_SSP_SSCR1_TTELP_MASK | SSP_SSP_SSCR1_TTE_MASK);

    sscr |= SSP_SSP_SSCR1_SCLKDIR(config->bitClockDirection) | SSP_SSP_SSCR1_SFRMDIR(config->frameClockDirection) |
            SSP_SSP_SSCR1_TTELP(config->txdLastPhaseState) | SSP_SSP_SSCR1_TTE(config->txdIdleState);

    base->SSP_SSCR1 = sscr;

    sscr = base->SSP_SSCR0;
    sscr &= ~(SSP_SSP_SSCR0_DSS_MASK | SSP_SSP_SSCR0_EDSS_MASK | SSP_SSP_SSCR0_FRDC_MASK | SSP_SSP_SSCR0_MOD_MASK);
    sscr |= (uint32_t)config->dataWidth | SSP_SSP_SSCR0_FRDC(config->frameSlotsNumber - 1UL) |
            SSP_SSP_SSCR0_MOD(config->enableNetWorkMode);
    base->SSP_SSCR0 = sscr;

    sspsp = SSP_SSP_SSPSP_FSRT(config->frameRelativeTiming) | SSP_SSP_SSPSP_SFRMWDTH(config->frameLength) |
            SSP_SSP_SSPSP_ETDS(config->txdEndTransferState) | SSP_SSP_SSPSP_SFRMP(config->frameClockPolarity) |
            SSP_SSP_SSPSP_SCMODE(config->bitClockMode);

    if (config->enableNetWorkMode == false)
    {
        sspsp |= SSP_SSP_SSPSP_EDMYSTOP(config->dummyStopDelay) | SSP_SSP_SSPSP_EDMYSTRT(config->dummyStartDelay) |
                 SSP_SSP_SSPSP_DMYSTOP(config->dummyStopDelay) | SSP_SSP_SSPSP_DMYSTRT(config->dummyStartDelay) |
                 SSP_SSP_SSPSP_SFRMDLY(config->frameDelay) | SSP_SSP_SSPSP_STRTDLY(config->startDelay);
    }
    else
    {
        base->SSP_SSTSA = config->txActiveSlotMask;
        base->SSP_SSRSA = config->rxActiveSlotMask;
    }

    base->SSP_SSPSP = sspsp;
}

/*! brief SSP enable interrupt.
 *
 * param base SSP peripheral base address
 * param interruptMask the Interrupt to be enabled, can a value or OR value of @ref _ssp_interrupt_mask.
 */
void SSP_EnableInterrupt(SSP_Type *base, uint32_t interruptMask)
{
    if ((interruptMask & (uint32_t)kSSP_InterruptTransmitFifoUnderrun) != 0U)
    {
        base->SSP_SSCR0 &= ~SSP_SSP_SSCR0_TIM_MASK;
    }

    if ((interruptMask & (uint32_t)kSSP_InterruptReceiveFifoOverrun) != 0U)
    {
        base->SSP_SSCR0 &= ~SSP_SSP_SSCR0_RIM_MASK;
    }

    if ((interruptMask & (uint32_t)kSSP_InterruptBitCountError) != 0U)
    {
        base->SSP_SSCR1 |= SSP_SSP_SSCR1_EBCEI_MASK;
    }

    if ((interruptMask & (uint32_t)kSSP_InterruptTransmitFifoReady) != 0U)
    {
        base->SSP_SSCR1 |= SSP_SSP_SSCR1_TIE_MASK;
    }

    if ((interruptMask & (uint32_t)kSSP_InterruptReceiveFifoReady) != 0U)
    {
        base->SSP_SSCR1 |= SSP_SSP_SSCR1_RIE_MASK;
    }
}

/*! brief SSP disable interrupt.
 *
 * param base SSP peripheral base address
 * param interruptMask the Interrupt to be disabled, can a value or OR value of @ref _ssp_interrupt_mask.
 */
void SSP_DisableInterrupt(SSP_Type *base, uint32_t interruptMask)
{
    if ((interruptMask & (uint32_t)kSSP_InterruptTransmitFifoUnderrun) != 0U)
    {
        base->SSP_SSCR0 |= SSP_SSP_SSCR0_TIM_MASK;
    }

    if ((interruptMask & (uint32_t)kSSP_InterruptReceiveFifoOverrun) != 0U)
    {
        base->SSP_SSCR0 |= SSP_SSP_SSCR0_RIM_MASK;
    }

    if ((interruptMask & (uint32_t)kSSP_InterruptBitCountError) != 0U)
    {
        base->SSP_SSCR1 &= ~SSP_SSP_SSCR1_EBCEI_MASK;
    }

    if ((interruptMask & (uint32_t)kSSP_InterruptTransmitFifoReady) != 0U)
    {
        base->SSP_SSCR1 &= ~SSP_SSP_SSCR1_TIE_MASK;
    }

    if ((interruptMask & (uint32_t)kSSP_InterruptReceiveFifoReady) != 0U)
    {
        base->SSP_SSCR1 &= ~SSP_SSP_SSCR1_RIE_MASK;
    }
}

/*! brief SSP clear interrupt status.
 *
 * Note: Transmit fifo ready flag and receive fifo flag do not support clear by software.
 *
 * param base SSP peripheral base address
 * param interruptMask the Interrupt to be cleared, can a value or OR'ed value of @ref _ssp_interrupt_mask.
 */
void SSP_ClearInterruptStatus(SSP_Type *base, uint32_t interruptMask)
{
    if ((interruptMask & (uint32_t)kSSP_InterruptTransmitFifoUnderrun) != 0U)
    {
        base->SSP_SSSR = SSP_SSP_SSSR_TUR_MASK;
    }

    if ((interruptMask & (uint32_t)kSSP_InterruptReceiveFifoOverrun) != 0U)
    {
        base->SSP_SSSR = SSP_SSP_SSSR_ROR_MASK;
    }

    if ((interruptMask & (uint32_t)kSSP_InterruptBitCountError) != 0U)
    {
        base->SSP_SSSR = SSP_SSP_SSSR_BCE_MASK;
    }
}

/*! brief SSP get interrupt status.
 *
 * param base SSP peripheral base address
 * param interruptMask the Interrupt to be cleared, can a value or OR'ed value of @ref _ssp_interrupt_mask.
 */
uint32_t SSP_GetInterruptStatus(SSP_Type *base)
{
    uint32_t retval = 0U;

    if ((base->SSP_SSSR & SSP_SSP_SSSR_TUR_MASK) != 0U)
    {
        retval |= (uint32_t)kSSP_InterruptTransmitFifoUnderrun;
    }

    if ((base->SSP_SSSR & SSP_SSP_SSSR_ROR_MASK) != 0U)
    {
        retval |= (uint32_t)kSSP_InterruptReceiveFifoOverrun;
    }

    if ((base->SSP_SSSR & SSP_SSP_SSSR_BCE_MASK) != 0U)
    {
        retval |= (uint32_t)kSSP_InterruptBitCountError;
    }

    if ((base->SSP_SSSR & SSP_SSP_SSSR_TFS_MASK) != 0U)
    {
        retval |= (uint32_t)kSSP_InterruptTransmitFifoReady;
    }

    if ((base->SSP_SSSR & SSP_SSP_SSSR_RFS_MASK) != 0U)
    {
        retval |= (uint32_t)kSSP_InterruptReceiveFifoReady;
    }

    return retval;
}

/*!
 * brief Creates the SSP send handle.
 *
 * param base SSP peripheral base address.
 * param handle SSP handle pointer.
 * param callback Structure pointer to contain all callback functions.
 * param userData Callback function parameter.
 */
void SSP_TransferSendCreateHandle(SSP_Type *base,
                                  ssp_handle_t *handle,
                                  ssp_transfer_callback_t callback,
                                  void *userData)
{
    assert(handle != NULL);
    assert(callback != NULL);

    uint32_t regBitWidth = base->SSP_SSCR0 & (SSP_SSP_SSCR0_DSS_MASK | SSP_SSP_SSCR0_EDSS_MASK);
    /* Zero the handle. */
    (void)memset(handle, 0, sizeof(*handle));

    handle->base = base;

    /* Set the callback. */
    handle->callback         = callback;
    handle->userData         = userData;
    handle->sspFifoWatermark = ((base->SSP_SSCR1 & SSP_SSP_SSCR1_TFT_MASK) >> SSP_SSP_SSCR1_TFT_SHIFT) + 1U;
    handle->bitWidth =
        regBitWidth == (uint32_t)kSSP_DataSize8Bit ? 1U : regBitWidth == (uint32_t)kSSP_DataSize16Bit ? 2U : 4U;

    /* Save the handle in global variables to support the double weak mechanism. */
    s_sspHandle[SSP_GetInstance(base)][0] = handle;

    /* save IRQ handler */
    s_sspSendIsr = SSP_TransferSendHandleIRQ;

    (void)EnableIRQ(s_sspIRQ[SSP_GetInstance(base)]);
}

/*!
 * brief Creates the SSP receive handle.
 *
 * param base SSP peripheral base address.
 * param handle SSP handle pointer.
 * param callback Structure pointer to contain all callback functions.
 * param userData Callback function parameter.
 */
void SSP_TransferReceiveCreateHandle(SSP_Type *base,
                                     ssp_handle_t *handle,
                                     ssp_transfer_callback_t callback,
                                     void *userData)
{
    assert(handle != NULL);
    assert(callback != NULL);

    uint32_t regBitWidth = base->SSP_SSCR0 & (SSP_SSP_SSCR0_DSS_MASK | SSP_SSP_SSCR0_EDSS_MASK);
    /* Zero the handle. */
    (void)memset(handle, 0, sizeof(*handle));

    handle->base = base;

    /* Set the callback. */
    handle->callback         = callback;
    handle->userData         = userData;
    handle->sspFifoWatermark = ((base->SSP_SSCR1 & SSP_SSP_SSCR1_RFT_MASK) >> SSP_SSP_SSCR1_RFT_SHIFT) + 1U;
    handle->bitWidth =
        regBitWidth == (uint32_t)kSSP_DataSize8Bit ? 1U : regBitWidth == (uint32_t)kSSP_DataSize16Bit ? 2U : 4U;

    /* Save the handle in global variables to support the double weak mechanism. */
    s_sspHandle[SSP_GetInstance(base)][1] = handle;

    /* save IRQ handler */
    s_sspReceiveIsr = SSP_TransferReceiveHandleIRQ;

    (void)EnableIRQ(s_sspIRQ[SSP_GetInstance(base)]);
}

/*!
 * brief Send the data using an interrupt and an asynchronous method.
 *
 * This function sends data and returns immediately. It doesn't wait for the transfer to complete or
 * to encounter an error. The application can submit multiple transfer requests without waiting previous
 * transfer request complete, since the SSP driver maintained a software transfer queue, the sizeof the queue
 * is configurable by macro SSP_XFER_QUEUE_SIZE which indicates how many request can be submitted at the same time.
 *
 * note Call API @ref SSP_TransferCreateHandle when calling this API.
 *
 * param handle SSP handle.
 * param transfer transfer pointer.
 * retval #kStatus_InvalidArgument Argument is invalid.
 * retval #kSSP_TransferStatusBusy Busy transferring.
 * retval #kStatus_Success Operate successfully.
 */
status_t SSP_TransferSendNonBlocking(ssp_handle_t *handle, ssp_transfer_t *transfer)
{
    status_t error = kStatus_Success;

    if ((handle == NULL) || (transfer == NULL))
    {
        error = kStatus_InvalidArgument;
    }
    /* Check if the queue is full */
    else
    {
        if (handle->sspQueue[handle->sspQueueUser].dataBuffer != NULL)
        {
            error = kSSP_TransferStatusBusy;
        }
        else
        {
            /* Add into queue */
            handle->sspQueue[handle->sspQueueUser].dataBuffer = transfer->dataBuffer;
            handle->sspQueue[handle->sspQueueUser].dataSize   = transfer->dataSize;
            handle->sspQueueUser                              = (handle->sspQueueUser + 1U) % SSP_XFER_QUEUE_SIZE;

            /* Enable interrupt */
            SSP_EnableInterrupt(handle->base, (uint32_t)kSSP_InterruptTransmitFifoUnderrun |
                                                  (uint32_t)kSSP_InterruptTransmitFifoReady |
                                                  (uint32_t)kSSP_InterruptBitCountError);
            /* Enable ssp port */
            SSP_Enable(handle->base, true);
        }
    }

    return error;
}

/*!
 * brief Receive the data using an interrupt and an asynchronous method.
 *
 * This function sends data and returns immediately. It doesn't wait for the transfer to complete or
 * to encounter an error. The application can submit multiple transfer requests without waiting previous
 * transfer request complete, since the SSP driver maintained a software transfer queue, the sizeof the queue
 * is configurable by macro SSP_XFER_QUEUE_SIZE which indicates how many request can be submitted at the same time.
 *
 * note Call API @ref SSP_TransferCreateHandle when calling this API.
 *
 * param handle SSP handle.
 * param transfer transfer pointer.
 * retval #kStatus_InvalidArgument Argument is invalid.
 * retval #kSSP_TransferStatusBusy Busy transferring.
 * retval #kStatus_Success Operate successfully.
 */
status_t SSP_TransferReceiveNonBlocking(ssp_handle_t *handle, ssp_transfer_t *transfer)
{
    status_t error = kStatus_Success;

    if ((handle == NULL) || (transfer == NULL))
    {
        error = kStatus_InvalidArgument;
    }
    else
    {
        /* Check if the queue is full */
        if (handle->sspQueue[handle->sspQueueUser].dataBuffer != NULL)
        {
            error = kSSP_TransferStatusBusy;
        }
        else
        {
            /* Add into queue */
            handle->sspQueue[handle->sspQueueUser].dataBuffer = transfer->dataBuffer;
            handle->sspQueue[handle->sspQueueUser].dataSize   = transfer->dataSize;
            handle->sspQueueUser                              = (handle->sspQueueUser + 1U) % SSP_XFER_QUEUE_SIZE;

            /* Enable interrupt */
            SSP_EnableInterrupt(handle->base, (uint32_t)kSSP_InterruptBitCountError |
                                                  (uint32_t)kSSP_InterruptReceiveFifoOverrun |
                                                  (uint32_t)kSSP_InterruptReceiveFifoReady);
            /* Enable ssp port */
            SSP_Enable(handle->base, true);
        }
    }

    return error;
}

/*!
 * brief Aborts the current send.
 *
 * note This API can be called any time when an interrupt non-blocking transfer initiates
 * to abort the transfer early.
 *
 * param handle Pointer to the ssp_handle_t structure which stores the transfer state.
 */
void SSP_TransferAbortSend(ssp_handle_t *handle)
{
    assert(handle != NULL);

    SSP_DisableInterrupt(handle->base, (uint32_t)kSSP_InterruptTransmitFifoUnderrun |
                                           (uint32_t)kSSP_InterruptBitCountError |
                                           (uint32_t)kSSP_InterruptTransmitFifoReady);

    /* Clear the queue */
    (void)memset(handle->sspQueue, 0, sizeof(ssp_transfer_t) * SSP_XFER_QUEUE_SIZE);
    handle->sspQueueDriver = 0;
    handle->sspQueueUser   = 0;
}

/*!
 * brief Aborts the current IRQ receive.
 *
 * note This API can be called when an interrupt non-blocking transfer initiates
 * to abort the transfer early.
 *
 * param handle Pointer to the ssp_handle_t structure which stores the transfer state.
 */
void SSP_TransferAbortReceive(ssp_handle_t *handle)
{
    assert(handle != NULL);

    SSP_DisableInterrupt(handle->base, (uint32_t)kSSP_InterruptReceiveFifoOverrun |
                                           (uint32_t)kSSP_InterruptBitCountError |
                                           (uint32_t)kSSP_InterruptReceiveFifoReady);

    /* Clear the queue */
    (void)memset(handle->sspQueue, 0, sizeof(ssp_transfer_t) * SSP_XFER_QUEUE_SIZE);
    handle->sspQueueDriver = 0;
    handle->sspQueueUser   = 0;
}

/*!
 * brief IRQ handler for the SSP transmitter.
 *
 * This function deals with the IRQs on the given host controller.
 *
 * param handle SSP handle.
 */
void SSP_TransferSendHandleIRQ(ssp_handle_t *handle)
{
    assert(handle != NULL);

    SSP_Type *base  = handle->base;
    uint32_t status = base->SSP_SSSR;

    /* clear the interrupt status */
    base->SSP_SSSR |= status;

    /* Handle Error */
    if (IS_SSP_FLAG_SET(status, SSP_SSP_SSSR_BCE_MASK))
    {
        /* Call the callback */
        if (handle->callback != NULL)
        {
            (handle->callback)(handle, kSSP_TransferStatusBitCountError, handle->userData);
        }
    }

    if (IS_SSP_FLAG_SET(status, SSP_SSP_SSSR_TUR_MASK))
    {
        /* Call the callback */
        if (handle->callback != NULL)
        {
            (handle->callback)(handle, kSSP_TransferStatusTxFifoError, handle->userData);
        }
    }

    if (IS_SSP_FLAG_SET(status, SSP_SSP_SSSR_TFS_MASK) && IS_SSP_FLAG_SET(base->SSP_SSCR1, SSP_SSP_SSCR1_TIE_MASK))
    {
        size_t sendSize =
            MIN((handle->sspQueue[handle->sspQueueDriver].dataSize), handle->sspFifoWatermark * handle->bitWidth);
        uint8_t *sendBuffer = handle->sspQueue[handle->sspQueueDriver].dataBuffer;

        SSP_WriteNonBlocking(base, sendBuffer, handle->bitWidth, sendSize);

        /* Update internal counter */
        handle->sspQueue[handle->sspQueueDriver].dataSize -= sendSize;
        handle->sspQueue[handle->sspQueueDriver].dataBuffer = (uint8_t *)((uint32_t)sendBuffer + sendSize);

        /* If finished a block, call the callback function */
        if (handle->sspQueue[handle->sspQueueDriver].dataSize == 0U)
        {
            (void)memset(&handle->sspQueue[handle->sspQueueDriver], 0, sizeof(ssp_transfer_t));
            handle->sspQueueDriver = (handle->sspQueueDriver + 1U) % SSP_XFER_QUEUE_SIZE;
            if (handle->callback != NULL)
            {
                (handle->callback)(handle, kSSP_TransferStatusTxIdle, handle->userData);
            }
        }

        /* If all data finished, just stop the transfer */
        if (handle->sspQueue[handle->sspQueueDriver].dataBuffer == NULL)
        {
            SSP_TransferAbortSend(handle);
        }
    }
}

/*!
 * brief IRQ handler for the SSP receiver.
 *
 * This function deals with the IRQs on the given host controller.
 *
 * param handle SSP handle.
 */
void SSP_TransferReceiveHandleIRQ(ssp_handle_t *handle)
{
    assert(handle != NULL);

    SSP_Type *base  = handle->base;
    uint32_t status = base->SSP_SSSR;

    /* clear the interrupt status */
    base->SSP_SSSR |= status;

    /* Handle Error */
    if (IS_SSP_FLAG_SET(status, SSP_SSP_SSSR_BCE_MASK))
    {
        /* Call the callback */
        if (handle->callback != NULL)
        {
            (handle->callback)(handle, kSSP_TransferStatusBitCountError, handle->userData);
        }
    }

    if (IS_SSP_FLAG_SET(status, SSP_SSP_SSSR_ROR_MASK))
    {
        /* Call the callback */
        if (handle->callback != NULL)
        {
            (handle->callback)(handle, kSSP_TransferStatusRxFifoError, handle->userData);
        }
    }

    if (IS_SSP_FLAG_SET(status, SSP_SSP_SSSR_RFS_MASK) && IS_SSP_FLAG_SET(base->SSP_SSCR1, SSP_SSP_SSCR1_RIE_MASK))
    {
        size_t receiveSize =
            MIN((handle->sspQueue[handle->sspQueueDriver].dataSize), handle->sspFifoWatermark * handle->bitWidth);
        uint8_t *receiveBuffer = handle->sspQueue[handle->sspQueueDriver].dataBuffer;

        SSP_ReadNonBlocking(base, receiveBuffer, handle->bitWidth, receiveSize);

        /* Update internal counter */
        handle->sspQueue[handle->sspQueueDriver].dataSize -= receiveSize;
        handle->sspQueue[handle->sspQueueDriver].dataBuffer = (uint8_t *)((uint32_t)receiveBuffer + receiveSize);

        /* If finished a block, call the callback function */
        if (handle->sspQueue[handle->sspQueueDriver].dataSize == 0U)
        {
            (void)memset(&handle->sspQueue[handle->sspQueueDriver], 0, sizeof(ssp_transfer_t));
            handle->sspQueueDriver = (handle->sspQueueDriver + 1U) % SSP_XFER_QUEUE_SIZE;
            if (handle->callback != NULL)
            {
                (handle->callback)(handle, kSSP_TransferStatusRxIdle, handle->userData);
            }
        }

        /* If all data finished, just stop the transfer */
        if (handle->sspQueue[handle->sspQueueDriver].dataBuffer == NULL)
        {
            SSP_TransferAbortReceive(handle);
        }
    }
}

#ifdef SSP0
void SSP0_DriverIRQHandler(void);
void SSP0_DriverIRQHandler(void)
{
    if (IS_SSP_FLAG_SET(SSP0->SSP_SSSR, SSP_SSP_SSSR_TUR_MASK | SSP_SSP_SSSR_TFS_MASK))
    {
        s_sspSendIsr(s_sspHandle[0U][0U]);
    }

    if (IS_SSP_FLAG_SET(SSP0->SSP_SSSR, SSP_SSP_SSSR_ROR_MASK | SSP_SSP_SSSR_RFS_MASK))
    {
        s_sspReceiveIsr(s_sspHandle[0U][1U]);
    }
    SDK_ISR_EXIT_BARRIER;
}
#endif

#ifdef SSP1
void SSP1_DriverIRQHandler(void);
void SSP1_DriverIRQHandler(void)
{
    if (IS_SSP_FLAG_SET(SSP1->SSP_SSSR, SSP_SSP_SSSR_TUR_MASK | SSP_SSP_SSSR_TFS_MASK))
    {
        s_sspSendIsr(s_sspHandle[1U][0U]);
    }

    if (IS_SSP_FLAG_SET(SSP1->SSP_SSSR, SSP_SSP_SSSR_ROR_MASK | SSP_SSP_SSSR_RFS_MASK))
    {
        s_sspReceiveIsr(s_sspHandle[1U][1U]);
    }
    SDK_ISR_EXIT_BARRIER;
}
#endif

#ifdef SSP2
void SSP2_DriverIRQHandler(void);
void SSP2_DriverIRQHandler(void)
{
    if (IS_SSP_FLAG_SET(SSP2->SSP_SSSR, SSP_SSP_SSSR_TUR_MASK | SSP_SSP_SSSR_TFS_MASK))
    {
        s_sspSendIsr(s_sspHandle[2U][0U]);
    }

    if (IS_SSP_FLAG_SET(SSP2->SSP_SSSR, SSP_SSP_SSSR_ROR_MASK | SSP_SSP_SSSR_RFS_MASK))
    {
        s_sspReceiveIsr(s_sspHandle[2U][1U]);
    }
    SDK_ISR_EXIT_BARRIER;
}
#endif
