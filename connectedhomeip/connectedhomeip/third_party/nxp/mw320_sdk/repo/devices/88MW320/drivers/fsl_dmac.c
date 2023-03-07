/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_dmac.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.mw_dmac"
#endif

/*! brief _dmac_transfer_status DMAC transfer status*/
enum _dmac_transfer_status
{
    kDMAC_TransferStatusIdle, /*!< Channel is idle. */
    kDMAC_TransferStatusBusy, /*!< Channel is busy and can't handle the transfer request. */
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! brief Array to map DMAC instance number to base pointer. */
static DMAC_Type *const s_dmacBases[] = DMAC_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! brief Array to map DMAC instance number to clock name. */
static const clock_ip_name_t s_dmacClockName[] = DMAC_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*! brief Array to map DMAC instance number to IRQ number. */
static const IRQn_Type s_dmacIRQNumber[] = DMAC_IRQS;

/*! brief Pointers to transfer handle for each DMAC channel. */
static dmac_handle_t *s_DMACHandle[FSL_FEATURE_MW_DMAC_MODULE_CHANNEL * FSL_FEATURE_SOC_MW_DMAC_COUNT];

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * brief Get the DMAC instance from peripheral base address.
 *
 * param base DMAC peripheral base address.
 * return DMAC instance.
 */
static uint32_t DMAC_GetInstance(DMAC_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_dmacBases); instance++)
    {
        if (s_dmacBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_dmacBases));

    return instance;
}

/*!
 * brief Get default dmac configuration.
 *
 * note This function will reset all of the configuration structure members to zero firstly,
 *       then apply default configurations to the structure.
 *
 * param config pointer to user's DMAC configure structure, see dmac_config_t for detail.
 */
void DMAC_GetDefaultConfig(dmac_config_t *config)
{
    assert(config != NULL);

    (void)memset(config, 0, sizeof(dmac_config_t));

    /*The value should be less than 1023, default value 12.*/
    config->m2pAckDelayCycle = 12U;
    for (uint32_t i = 0U; i < (uint32_t)FSL_FEATURE_MW_DMAC_MODULE_CHANNEL; i++)
    {
        /*The default value is kDMAC_PeriphNum_None.*/
        config->peripheralNumber[i] = kDMAC_PeriphNum_None;
    }
}

/*!
 * brief DMAC initialization
 *
 * param base DMAC peripheral base address.
 * param Config pointer to user's DMAC configure structure, see dmac_transfer_config_t for detail.
 */
void DMAC_Init(DMAC_Type *base, dmac_config_t *config)
{
    assert(config != NULL);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate DMAC peripheral clock */
    CLOCK_EnableClock(s_dmacClockName[DMAC_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    /* clear all the enabled request, status to make sure DMAC status is in normal condition */
    base->STATUS_TFRINT     = 0xFFFFFFFFU;
    base->STATUS_BLOCKINT   = 0xFFFFFFFFU;
    base->STATUS_BUSERRINT  = 0xFFFFFFFFU;
    base->STATUS_ADDRERRINT = 0xFFFFFFFFU;

    /* Configure DMAC peripheral according to the configuration structure. */
    DMAC_SetAckDelayCycle(base, config->m2pAckDelayCycle);

    /* channel transfer configuration */
    for (uint32_t i = 0U; i < (uint32_t)FSL_FEATURE_MW_DMAC_MODULE_CHANNEL; i++)
    {
        if (config->channelTransferConfig[i] != NULL)
        {
            DMAC_SetChannelTransferConfig(base, (dmac_channel_t)i, config->channelTransferConfig[i]);
        }
        if (config->peripheralNumber[i] != kDMAC_PeriphNum_None)
        {
            DMAC_SetPeripheralNumber(base, (dmac_channel_t)i, config->peripheralNumber[i]);
        }
    }
}

/*!
 * brief DMAC De-initialization
 *
 * param base DMAC peripheral base address.
 */
void DMAC_Deinit(DMAC_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate DMAC peripheral clock */
    CLOCK_DisableClock(s_dmacClockName[DMAC_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Get channel default transfer configuration.
 *
 * param transfer pointer to user's DMAC channel configure structure, see dmac_channel_transfer_config_t for detail.
 * param srcAddr source address, must be byte address.
 * param destAddr destination address,  must be byte address.
 * param transferWidth the number of bits are transferred in each read/write.
 * param burstLength burst transaction length.
 * param transferTotalBytes transfer totalbytes 0~8191bytes maximum transfer length (8k-1) bytes.
 * param transferType DMAC channel transfer type.
 */
void DMAC_GetChannelDefaultTransferConfig(dmac_channel_transfer_config_t *transfer,
                                          void *srcAddr,
                                          void *destAddr,
                                          dmac_channel_transfer_width_t transferWidth,
                                          dmac_channel_burst_length_t burstLength,
                                          uint32_t transferTotalBytes,
                                          dmac_channel_transfer_type_t transferType)
{
    assert(transfer != NULL);
    assert(((transferTotalBytes % (uint32_t)transferWidth) == 0U));
    assert(((uint32_t)(uint32_t *)srcAddr & ((uint32_t)transferWidth - 1U)) == 0U);
    assert(((uint32_t)(uint32_t *)destAddr & ((uint32_t)transferWidth - 1U)) == 0U);

    (void)memset(transfer, 0, sizeof(dmac_channel_transfer_config_t));

    transfer->srcAddr            = srcAddr;
    transfer->destAddr           = destAddr;
    transfer->transferWidth      = transferWidth;
    transfer->burstLength        = burstLength;
    transfer->transferTotalBytes = transferTotalBytes;
    transfer->transferType       = transferType;

    /* default advance channel transfer configurations */
    if (transferType == kDMAC_ChannelTransferMemoryToMemory)
    {
        transfer->srcAddrIncrementType  = kDMAC_ChannelAddressIncrement;
        transfer->destAddrIncrementType = kDMAC_ChannelAddressIncrement;
    }
    else if (transferType == kDMAC_ChannelTransferPeripheralToMemory)
    {
        transfer->srcAddrIncrementType  = kDMAC_ChannelAddressFix;
        transfer->destAddrIncrementType = kDMAC_ChannelAddressIncrement;
    }
    else
    {
        transfer->srcAddrIncrementType  = kDMAC_ChannelAddressIncrement;
        transfer->destAddrIncrementType = kDMAC_ChannelAddressFix;
    }
}

/*!
 * brief DMAC set channel transfer configurations
 *
 * param base DMAC peripheral base address.
 * param channel DMAC channel number.
 * param transfer pointer to user's DMAC channel configure structure, see dmac_channel_transfer_config_t for detail.
 */
void DMAC_SetChannelTransferConfig(DMAC_Type *base, dmac_channel_t channel, dmac_channel_transfer_config_t *transfer)
{
    assert(transfer != NULL);
    assert(((transfer->transferTotalBytes % (uint32_t)transfer->transferWidth) == 0U));
    assert(((uint32_t)(uint32_t *)transfer->srcAddr & ((uint32_t)transfer->transferWidth - 1U)) == 0U);
    assert(((uint32_t)(uint32_t *)transfer->destAddr & ((uint32_t)transfer->transferWidth - 1U)) == 0U);

    uint32_t ctrlA = base->CHANNEL[channel].CTRLA;

    base->CHANNEL[channel].SADR = (uint32_t)(uint32_t *)transfer->srcAddr;

    base->CHANNEL[channel].TADR = (uint32_t)(uint32_t *)transfer->destAddr;

    ctrlA &= ~DMAC_CTRLA_WIDTH_MASK;
    ctrlA |= DMAC_CTRLA_WIDTH((transfer->transferWidth == kDMAC_ChannelTransferWidth32Bits) ?
                                  ((uint32_t)transfer->transferWidth - 1U) :
                                  (uint32_t)transfer->transferWidth);

    ctrlA &= ~DMAC_CTRLA_TRAN_SIZE_MASK;
    ctrlA |= DMAC_CTRLA_TRAN_SIZE(transfer->burstLength);

    ctrlA &= ~DMAC_CTRLA_LEN_MASK;
    ctrlA |= DMAC_CTRLA_LEN(transfer->transferTotalBytes);

    ctrlA &= ~DMAC_CTRLA_TRAN_TYPE_MASK;
    ctrlA |= DMAC_CTRLA_TRAN_TYPE(transfer->transferType);

    ctrlA &= ~DMAC_CTRLA_INCSRCADDR_MASK;
    ctrlA |= DMAC_CTRLA_INCSRCADDR(transfer->srcAddrIncrementType);

    ctrlA &= ~DMAC_CTRLA_INCTRGADDR_MASK;
    ctrlA |= DMAC_CTRLA_INCTRGADDR(transfer->destAddrIncrementType);

    base->CHANNEL[channel].CTRLA = ctrlA;

    DMAC_EnableChannelInterrupts(base, channel, transfer->interruptMask);
}

/*!
 * brief DMAC ack delay cycle for single transfer in M2P transfer type.Only called when the transfer type is M2P.
 *
 * param base DMAC peripheral base address.
 * param m2pAckDelayCycle for single write transaction to peripheral. The value should be less than 1023, default
 * value 12.
 */
void DMAC_SetAckDelayCycle(DMAC_Type *base, uint32_t m2pAckDelayCycle)
{
    /* Config DMAC ACK Delay Cycle for single transfer */
    base->ACK_DELAY = DMAC_ACK_DELAY_ACK_DELAY_NUM(m2pAckDelayCycle);
}

/*!
 * brief DMAC set peripheral number.
 *
 * param base DMAC peripheral base address.
 * param channel DMAC channel number.
 * param peripheralNumber Indicates the valid peripheral request number, default value kDMAC_PeriphNum_None,
 * see @ref dmac_peripheral_number_t for detail.
 */
void DMAC_SetPeripheralNumber(DMAC_Type *base, dmac_channel_t channel, dmac_peripheral_number_t peripheralNumber)
{
    base->CHANNEL[channel].CTRLB =
        (base->CHANNEL[channel].CTRLB & (~DMAC_CTRLB_PERNUM_MASK)) | DMAC_CTRLB_PERNUM(peripheralNumber);
}

/*!
 * brief enables the dmac channel interrupts
 *
 * param base DMAC peripheral base address.
 * param channel DMAC channel number.
 * param interruptsMask the mask is a logical OR of enumerator members see _dmac_channel_interrupt_flag.
 */
void DMAC_EnableChannelInterrupts(DMAC_Type *base, dmac_channel_t channel, uint32_t interruptsMask)
{
    if (0U != (interruptsMask & (uint32_t)kDMAC_ChannelInterruptFlagBusError))
    {
        base->MASK_BUSERRINT |= (1UL << channel);
    }
    else
    {
        base->MASK_BUSERRINT &= ~(1UL << channel);
    }

    if (0U != (interruptsMask & (uint32_t)kDMAC_ChannelInterruptFlagAdressError))
    {
        base->MASK_ADDRERRINT |= (1UL << channel);
    }
    else
    {
        base->MASK_ADDRERRINT &= ~(1UL << channel);
    }

    if (0U != (interruptsMask & (uint32_t)kDMAC_ChannelInterruptFlagTransferComplete))
    {
        base->MASK_TFRINT |= (1UL << channel);
    }
    else
    {
        base->MASK_TFRINT &= ~(1UL << channel);
    }

    if (0U != (interruptsMask & (uint32_t)kDMAC_ChannelInterruptBlockTransferComplete))
    {
        base->MASK_BLOCKINT |= (1UL << channel);
    }
    else
    {
        base->MASK_BLOCKINT &= ~(1UL << channel);
    }
}

/*!
 * brief disables the dmac channel interrupts
 *
 * param base DMAC peripheral base address.
 * param channel DMAC channel number.
 * param interruptsMask the mask is a logical OR of enumerator members see _dmac_channel_interrupt_flag.
 */
void DMAC_DisableChannelInterrupts(DMAC_Type *base, dmac_channel_t channel, uint32_t interruptsMask)
{
    if (interruptsMask == (uint32_t)kDMAC_ChannelInterruptFlagBusError)
    {
        base->MASK_BUSERRINT |= (1UL << channel);
    }

    if (interruptsMask == (uint32_t)kDMAC_ChannelInterruptFlagAdressError)
    {
        base->MASK_ADDRERRINT |= (1UL << channel);
    }

    if (interruptsMask == (uint32_t)kDMAC_ChannelInterruptFlagTransferComplete)
    {
        base->MASK_TFRINT |= (1UL << channel);
    }

    if (interruptsMask == (uint32_t)kDMAC_ChannelInterruptBlockTransferComplete)
    {
        base->MASK_BLOCKINT |= (1UL << channel);
    }
}

/*!
 * brief Gets the DMAC channel status.
 *
 * param base DMAC peripheral base address.
 * param channel DMAC channel number.
 * retval The mask of channel status. Users need to use the
 *         _dmac_channel_interrupt_flag type to decode the return variables.
 */
uint32_t DMAC_GetChannelInterruptStatus(DMAC_Type *base, dmac_channel_t channel)
{
    uint32_t retval = 0U;

    /* Get ChannelTransferComplete bit flag */
    if (((base->STATUS_TFRINT >> channel) & 0x1U) != 0U)
    {
        retval |= (uint32_t)kDMAC_ChannelInterruptFlagTransferComplete;
    }

    /* Get ChannelBlockTransferComplete bit flag */
    if (((base->STATUS_BLOCKINT >> channel) & 0x1U) != 0U)
    {
        retval |= (uint32_t)kDMAC_ChannelInterruptBlockTransferComplete;
    }

    /* Get ChannelBusError bit flag */
    if (((base->STATUS_BUSERRINT >> channel) & 0x1U) != 0U)
    {
        retval |= (uint32_t)kDMAC_ChannelInterruptFlagBusError;
    }

    /* Get ChannelAddressError bit flag */
    if (((base->STATUS_ADDRERRINT >> channel) & 0x1U) != 0U)
    {
        retval |= (uint32_t)kDMAC_ChannelInterruptFlagAdressError;
    }

    return retval;
}

/*!
 * brief Clears the DMAC channel status
 *
 * param base DMAC peripheral base address.
 * param channel DMAC channel number.
 * param status The mask of channel status to be cleared. Users need to use
 *             the defined _dmac_channel_interrupt_flag type.
 */
void DMAC_ClearChannelInterruptStatus(DMAC_Type *base, dmac_channel_t channel, uint32_t status)
{
    /* Clear ChannelTransferComplete bit flag */
    if (0U != (status & (uint32_t)kDMAC_ChannelInterruptFlagTransferComplete))
    {
        base->STATUS_TFRINT = (1UL << channel);
    }
    /* Clear ChannelBlockTransferComplete bit flag */
    if (0U != (status & (uint32_t)kDMAC_ChannelInterruptBlockTransferComplete))
    {
        base->STATUS_BLOCKINT = (1UL << channel);
    }
    /* Clear ChannelBusError bit flag */
    if (0U != (status & (uint32_t)kDMAC_ChannelInterruptFlagBusError))
    {
        base->STATUS_BUSERRINT = (1UL << channel);
    }
    /* Clear ChannelAddressError bit flag */
    if (0U != (status & (uint32_t)kDMAC_ChannelInterruptFlagAdressError))
    {
        base->STATUS_ADDRERRINT = (1UL << channel);
    }
}

/*!
 * brief Creates the DMAC channel handle.
 *
 * This function is called if using the transactional API for DMAC. This function
 * initializes the internal state of the DMAC handle.
 *
 * param base DMAC peripheral base address.
 * param handle DMAC handle pointer. The DMAC handle stores Callback function and
 *               parameters.
 * param channel DMAC channel number.
 *
 */
void DMAC_TransferCreateHandle(DMAC_Type *base, dmac_handle_t *handle, dmac_channel_t channel)
{
    assert(handle != NULL);

    uint32_t dmacInstance = 0U;

    /* Zero the handle */
    (void)memset(handle, 0, sizeof(dmac_handle_t));

    handle->base    = base;
    handle->channel = channel;

    /* Get the DMAC instance number */
    handle->state         = (uint32_t)kDMAC_TransferStatusIdle;
    dmacInstance          = DMAC_GetInstance(base);
    s_DMACHandle[channel] = handle;

    /* Enable interrupt */
    (void)EnableIRQ(s_dmacIRQNumber[dmacInstance]);
}

/*!
 * brief Sets the DMAC Callback function.
 *
 * This callback is called in the DMAC IRQ handler. Use the callback to do something
 * after the current transfer complete.
 *
 * param handle DMAC handle pointer.
 * param callback DMAC callback function pointer.
 * param userData Parameter for callback function. If it is not needed, just set to NULL.
 */
void DMAC_TransferSetCallback(dmac_handle_t *handle, dmac_transfer_callback_t callback, void *userData)
{
    assert(handle != NULL);

    handle->callback = callback;
    handle->userData = userData;
}

/*!
 * @brief Submits the DMAC transfer configurations.
 *
 * @param handle DMAC handle pointer
 * @param transfer pointer to user's DMAC channel configure structure, see _dmac_channel_interrupt_flag for detail
 *
 * @retval #kStatus_Success It means submit transfer request succeed
 * @retval #kStatus_Fail It means submit transfer request failed
 */
status_t DMAC_TransferSubmit(dmac_handle_t *handle, dmac_channel_transfer_config_t *transfer)
{
    assert(handle != NULL);
    assert(transfer != NULL);

    if (handle->state == (uint32_t)kDMAC_TransferStatusBusy)
    {
        return kStatus_Fail;
    }
    DMAC_SetChannelTransferConfig(handle->base, handle->channel, transfer);
    handle->state = kDMAC_TransferStatusBusy;

    return kStatus_Success;
}

/*!
 * brief DMAC starts transfer.
 *
 * This function enables the channel request. Users can call this function after submitting the transfer request
 * or before submitting the transfer request.
 *
 * param handle DMAC handle pointer.
 */
void DMAC_TransferStart(dmac_handle_t *handle)
{
    assert(handle != NULL);

    DMAC_StartChannel(handle->base, handle->channel);
}

/*!
 * brief DMAC stops transfer.
 *
 * This function disables the channel request to pause the transfer. Users can call DMAC_StartTransfer()
 * again to resume the transfer.
 *
 * param handle DMAC handle pointer.
 */
void DMAC_TransferStop(dmac_handle_t *handle)
{
    assert(handle != NULL);

    DMAC_StopChannel(handle->base, handle->channel);
    handle->state = (uint32_t)kDMAC_TransferStatusIdle;
}

/*!
 * brief DMAC IRQ handler for the transfer completion.
 *
 * param base DMAC peripheral base address.
 */
void DMAC_TransferHandleIRQ(DMAC_Type *base)
{
    dmac_handle_t *handle;
    uint32_t startChannel = 0U;
    uint32_t intStatusTfr, intStatusBlock, intStatusBusErr, intStatusAddrErr;

    /* Get current unmasked interrupt status */
    intStatusTfr     = base->STATUS_TFRINT;
    intStatusBlock   = base->STATUS_BLOCKINT;
    intStatusBusErr  = base->STATUS_BUSERRINT;
    intStatusAddrErr = base->STATUS_ADDRERRINT;

    /* Clear the unmasked generated interrupts */
    base->STATUS_TFRINT |= intStatusTfr;
    base->STATUS_BLOCKINT |= intStatusBlock;
    base->STATUS_BUSERRINT |= intStatusBusErr;
    base->STATUS_ADDRERRINT |= intStatusAddrErr;

    /* Find channels that have completed transfer */
    for (uint32_t i = 0U; i < (uint32_t)FSL_FEATURE_MW_DMAC_MODULE_CHANNEL; i++)
    {
        handle = s_DMACHandle[i + startChannel];
        /* Handle is not present */
        if (NULL == handle)
        {
            continue;
        }

        if (((intStatusTfr & (0x01UL << i)) != 0U) && ((base->MASK_TFRINT & (0x01UL << i)) != 0U))
        {
            if (NULL != handle->callback)
            {
                handle->state = (uint32_t)kDMAC_TransferStatusIdle;
                (handle->callback)(handle, handle->userData, kDMAC_ChannelInterruptFlagTransferComplete);
            }
        }

        if (((intStatusBlock & (0x01UL << i)) != 0U) && ((base->MASK_BLOCKINT & (0x01UL << i)) != 0U))
        {
            if (NULL != handle->callback)
            {
                (handle->callback)(handle, handle->userData, kDMAC_ChannelInterruptBlockTransferComplete);
            }
        }

        if ((intStatusBusErr & (0x01UL << i)) != 0U)
        {
            if (NULL != handle->callback)
            {
                (handle->callback)(handle, handle->userData, kDMAC_ChannelInterruptFlagBusError);
            }
        }

        if ((intStatusAddrErr & (0x01UL << i)) != 0U)
        {
            if (NULL != handle->callback)
            {
                (handle->callback)(handle, handle->userData, kDMAC_ChannelInterruptFlagAdressError);
            }
        }
    }
}
#if defined DMAC
void DMAC_DriverIRQHandler(void);
void DMAC_DriverIRQHandler(void)
{
    DMAC_TransferHandleIRQ(DMAC);

    SDK_ISR_EXIT_BARRIER;
}
#endif
