/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_DMAC_H_
#define _FSL_DMAC_H_

#include "fsl_common.h"

/*!
 * @addtogroup dmac
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief DMAC transfer type */
typedef enum _dmac_channel_transfer_type
{
    kDMAC_ChannelTransferMemoryToMemory = 0x0U, /*!< Transfer type from memory to memory assume that the both source and
                                                   destination address are incremental */
    kDMAC_ChannelTransferPeripheralToMemory,    /*!< Transfer type peripher to memory assume that the source address is
                                                   fixed */
    kDMAC_ChannelTransferMemoryToPeripheral,    /*!< Transfer type from memory to peripheral assume that the destination
                                                   address is fixed */
} dmac_channel_transfer_type_t;

/*! @brief DMAC interrupt source
 * @anchor _dmac_channel_interrupt_flag
 */
enum _dmac_channel_interrupt_flag
{
    kDMAC_ChannelInterruptFlagTransferComplete  = 1U, /*!< Dmac transfer complete interrupt */
    kDMAC_ChannelInterruptBlockTransferComplete = 2U, /*!< Block transfer complete interrupt */
    kDMAC_ChannelInterruptFlagBusError          = 4U, /*!< Bus Error interrupt */
    kDMAC_ChannelInterruptFlagAdressError       = 8U, /*!< Address Error interrupt */
};

/*!@brief dmac channel index */
typedef enum _dmac_channel
{
    kDMAC_Channel0  = 0U,  /*!< channel 0 define, priority level 0, the highest priority */
    kDMAC_Channel1  = 1U,  /*!< channel 1 define, priority level 0, the highest priority */
    kDMAC_Channel2  = 2U,  /*!< channel 2 define, priority level 0, the highest priority */
    kDMAC_Channel3  = 3U,  /*!< channel 3 define, priority level 0, the highest priority */
    kDMAC_Channel4  = 4U,  /*!< channel 4 define, priority level 1 */
    kDMAC_Channel5  = 5U,  /*!< channel 5 define, priority level 1 */
    kDMAC_Channel6  = 6U,  /*!< channel 6 define, priority level 1 */
    kDMAC_Channel7  = 7U,  /*!< channel 7 define, priority level 1 */
    kDMAC_Channel8  = 8U,  /*!< channel 8 define, priority level 2 */
    kDMAC_Channel9  = 9U,  /*!< channel 9 define, priority level 2 */
    kDMAC_Channel10 = 10U, /*!< channel 10 define, priority level 2 */
    kDMAC_Channel11 = 11U, /*!< channel 11 define, priority level 2 */
    kDMAC_Channel12 = 12U, /*!< channel 12 define, priority level 3, the lowest priority */
    kDMAC_Channel13 = 13U, /*!< channel 13 define, priority level 3, the lowest priority */
    kDMAC_Channel14 = 14U, /*!< channel 14 define, priority level 3, the lowest priority */
    kDMAC_Channel15 = 15U, /*!< channel 15 define, priority level 3, the lowest priority */
    kDMAC_Channel16 = 16U, /*!< channel 16 define, priority level 0, the highest priority */
    kDMAC_Channel17 = 17U, /*!< channel 17 define, priority level 0, the highest priority */
    kDMAC_Channel18 = 18U, /*!< channel 18 define, priority level 0, the highest priority */
    kDMAC_Channel19 = 19U, /*!< channel 19 define, priority level 0, the highest priority */
    kDMAC_Channel20 = 20U, /*!< channel 20 define, priority level 1 */
    kDMAC_Channel21 = 21U, /*!< channel 21 define, priority level 1 */
    kDMAC_Channel22 = 22U, /*!< channel 22 define, priority level 1 */
    kDMAC_Channel23 = 23U, /*!< channel 23 define, priority level 1 */
    kDMAC_Channel24 = 24U, /*!< channel 24 define, priority level 2 */
    kDMAC_Channel25 = 25U, /*!< channel 25 define, priority level 2 */
    kDMAC_Channel26 = 26U, /*!< channel 26 define, priority level 2 */
    kDMAC_Channel27 = 27U, /*!< channel 27 define, priority level 2 */
    kDMAC_Channel28 = 28U, /*!< channel 28 define, priority level 3, the lowest priority */
    kDMAC_Channel29 = 29U, /*!< channel 29 define, priority level 3, the lowest priority */
    kDMAC_Channel30 = 30U, /*!< channel 30 define, priority level 3, the lowest priority */
    kDMAC_Channel31 = 31U, /*!< channel 31 define, priority level 3, the lowest priority */
} dmac_channel_t;

/*! @brief DMAC transfer address increment definition */
typedef enum _dmac_channel_address_increment_type
{
    kDMAC_ChannelAddressFix = 0U,  /*!< Address Fix */
    kDMAC_ChannelAddressIncrement, /*!< Address increment */
} dmac_channel_address_increment_type_t;

/*! @brief DMAC transfer width configuration */
typedef enum _dmac_channel_transfer_width
{
    kDMAC_ChannelTransferWidth8Bits  = 1U, /*!< Source/Destination data transfer width is 1 byte every time */
    kDMAC_ChannelTransferWidth16Bits = 2U, /*!< Source/Destination data transfer width is 2 bytes every time */
    kDMAC_ChannelTransferWidth32Bits = 4U, /*!< Source/Destination data transfer width is 4 bytes every time */
} dmac_channel_transfer_width_t;

/*! @brief DMAC transfer burst length definition */
typedef enum _dmac_channel_burst_length
{
    kDMAC_ChannelBurstLength1DataWidth = 0U, /*!< Maximum burst transaction length 1 data width */
    kDMAC_ChannelBurstLength4DataWidth,      /*!< Maximum burst transaction length 4 data width */
    kDMAC_ChannelBurstLength8DataWidth,      /*!< Maximum burst transaction length 8 data width */
    kDMAC_ChannelBurstLength16DataWidth,     /*!< Maximum burst transaction length 16 data width */
} dmac_channel_burst_length_t;

/*! @brief dmac channel transfer configuration
 *
 * The transfer configuration structure support full feature configuration of the transfer control descriptor.
 *
 */
typedef struct _dmac_channel_transfer_config
{
    void *srcAddr;  /*!< source address */
    void *destAddr; /*!< destination address */

    dmac_channel_address_increment_type_t srcAddrIncrementType;  /*!< Source address increment
                                                           kDMAC_ChannelAddressFix: No change
                                                           kDMAC_ChannelAddressIncrement: Increment */
    dmac_channel_address_increment_type_t destAddrIncrementType; /*!< Destination address increment
                                                           kDMAC_ChannelAddressFix: No change
                                                           kDMAC_ChannelAddressIncrement: Increment */

    dmac_channel_transfer_type_t transferType; /*!< Transfer type and flow control
                                                 kDMAC_ChannelTransferMemoryToMemory: Memory to Memory
                                                 kDMAC_ChannelTransferPeripheralToMemory: Memory to peripheral
                                                 kDMAC_ChannelTransferMemoryToPeripheral: Peripheral to memory */

    dmac_channel_burst_length_t burstLength; /*!< Burst transaction length
                                           kDMAC_ChannelBurstLength1DataWidth: 1 Data Width
                                           kDMAC_ChannelBurstLength4DataWidth: 4 Data Width
                                           kDMAC_ChannelBurstLength8DataWidth: 8 Data Width
                                           kDMAC_ChannelBurstLength16DataWidth: 16 Data Width */

    dmac_channel_transfer_width_t transferWidth; /*!< Transfer width
                                                   kDMAC_ChannelTransferWidth8Bits: 8  bits
                                                   kDMAC_ChannelTransferWidth16Bits: 16  bits
                                                   kDMAC_ChannelTransferWidth32Bits: 32  bits*/

    uint32_t transferTotalBytes; /*!< Transfer totalbytes
                                    0~8191bytes
                                    maximum transfer length (8k-1) bytes*/

    uint32_t interruptMask; /*!< Interrupt Mask */
} dmac_channel_transfer_config_t;

/*! @brief dmac configuration structure */
typedef struct _dmac_config
{
    uint32_t m2pAckDelayCycle; /*!< m2pAckDelayCycle for single write transaction to peripheral. The value should be
                            less than 1023, default value 12. */

    dmac_channel_transfer_config_t
        *channelTransferConfig[FSL_FEATURE_MW_DMAC_MODULE_CHANNEL]; /*!< channel transfer configuration pointer */

    dmac_peripheral_number_t peripheralNumber[FSL_FEATURE_MW_DMAC_MODULE_CHANNEL]; /*!< Peripheral Number
                          Indicates the valid peripheral request number.The default value kDMAC_PeriphNum_None. */
} dmac_config_t;

/*! @brief handler for DMAC */
typedef struct _dmac_handle dmac_handle_t;

/*! @brief Define Callback function for DMAC
 *
 * @param handle DMAC handle pointer, users shall not touch the values inside.
 * @param userData The callback user parameter pointer. Users can use this parameter to involve things users need to
 *                 change in DMAC Callback function.
 * @param status The mask of channel status . Users need to use the _dmac_channel_interrupt_status type.
 *               If the current loaded transfer done. In normal mode it means if all transfer done. In
 *               scatter gather mode, this parameter shows is the current transfer block in DMAC register is done.
 */
typedef void (*dmac_transfer_callback_t)(dmac_handle_t *handle, void *userData, uint32_t status);

/*! @brief DMAC transfer handle structure */
struct _dmac_handle
{
    volatile uint32_t state;           /*!< channel transfer state */
    dmac_transfer_callback_t callback; /*!< callback function */
    void *userData;                    /*!< Callback function parameter. */
    DMAC_Type *base;                   /*!< DMAC peripheral base address. */
    dmac_channel_t channel;            /*!< DMAC channel number. */
};

/*******************************************************************************
 * APIs
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @name DMAC Initialization and De-initialization interfaces
 * @{
 */

/*!
 * @brief Get default dmac configuration.
 *
 * @note This function will reset all of the configuration structure members to zero firstly,
 *       then apply default configurations to the structure.
 *
 * @param config pointer to user's DMAC configure structure, see @ref dmac_config_t for detail.
 */
void DMAC_GetDefaultConfig(dmac_config_t *config);

/*!
 * @brief DMAC initialization
 *
 * @param base DMAC peripheral base address.
 * @param config pointer to user's DMAC config structure, see @ref dmac_config_t for detail.
 */
void DMAC_Init(DMAC_Type *base, dmac_config_t *config);

/*!
 * @brief DMAC De-initialization
 *
 * @param base DMAC peripheral base address.
 */
void DMAC_Deinit(DMAC_Type *base);

/*! @} */

/*!
 * @name DMAC channel functional interfaces
 * @{
 */

/*!
 * @brief Get channel default transfer configuration.
 *
 * @param transfer pointer to user's DMAC channel configure structure, see @ref dmac_channel_transfer_config_t for
 * detail.
 * @param srcAddr source address, must be byte address.
 * @param destAddr destination address,  must be byte address.
 * @param transferWidth the number of bits are transferred in each read/write.
 * @param burstLength burst transaction length.
 * @param transferTotalBytes transfer totalbytes 0~8191bytes maximum transfer length (8k-1) bytes.
 * @param transferType DMAC channel transfer type.
 */
void DMAC_GetChannelDefaultTransferConfig(dmac_channel_transfer_config_t *transfer,
                                          void *srcAddr,
                                          void *destAddr,
                                          dmac_channel_transfer_width_t transferWidth,
                                          dmac_channel_burst_length_t burstLength,
                                          uint32_t transferTotalBytes,
                                          dmac_channel_transfer_type_t transferType);

/*!
 * @brief DMAC set channel transfer configurations
 *
 * @param base DMAC peripheral base address.
 * @param channel DMAC channel number.
 * @param transfer pointer to user's DMAC channel configure structure,see @ref dmac_channel_transfer_config_t for
 * detail.
 */
void DMAC_SetChannelTransferConfig(DMAC_Type *base, dmac_channel_t channel, dmac_channel_transfer_config_t *transfer);

/*!
 * @brief start the dmac channel
 *
 * @param base DMAC peripheral base address.
 * @param channel DMAC channel number.
 */
static inline void DMAC_StartChannel(DMAC_Type *base, dmac_channel_t channel)
{
    base->CHANNEL[channel].CHL_EN |= DMAC_CHL_EN_CHL_EN(1U);
}

/*!
 * @brief stop the dmac channel
 *
 * @param base DMAC peripheral base address.
 * @param channel DMAC channel number.
 */
static inline void DMAC_StopChannel(DMAC_Type *base, dmac_channel_t channel)
{
    base->CHANNEL[channel].CHL_STOP |= DMAC_CHL_STOP_CHL_STOP(1U);
}

/*!
 * @brief DMAC ack delay cycle for single transfer in M2P transfer type.Only called when the transfer type is M2P.
 *
 * @param base DMAC peripheral base address.
 * @param m2pAckDelayCycle for single write transaction to peripheral. The value should be less than 1023, default
 * value 12.
 */
void DMAC_SetAckDelayCycle(DMAC_Type *base, uint32_t m2pAckDelayCycle);

/*!
 * @brief DMAC set peripheral number.
 *
 * @param base DMAC peripheral base address.
 * @param channel DMAC channel number.
 * @param peripheralNumber Indicates the valid peripheral request number, default value kDMAC_PeriphNum_None,
 * see dmac_peripheral_number_t for detail.
 */
void DMAC_SetPeripheralNumber(DMAC_Type *base, dmac_channel_t channel, dmac_peripheral_number_t peripheralNumber);

/*! @} */

/*!
 * @name DMAC channel interrupt Interfaces
 * @{
 */
/*!
 * @brief enables the dmac channel interrupts
 *
 * @param base DMAC peripheral base address.
 * @param channel DMAC channel number.
 * @param interruptsMask the mask is a logical OR of enumerator members see @ref _dmac_channel_interrupt_flag.
 */
void DMAC_EnableChannelInterrupts(DMAC_Type *base, dmac_channel_t channel, uint32_t interruptsMask);

/*!
 * @brief disables the dmac channel interrupts
 *
 * @param base DMAC peripheral base address.
 * @param channel DMAC channel number.
 * @param interruptsMask the mask is a logical OR of enumerator members see @ref _dmac_channel_interrupt_flag.
 */
void DMAC_DisableChannelInterrupts(DMAC_Type *base, dmac_channel_t channel, uint32_t interruptsMask);

/*! @} */

/*!
 * @name DMAC channel Status Interfaces
 * @{
 */

/*!
 * @brief Gets the DMAC channel status.
 *
 * @param base DMAC peripheral base address.
 * @param channel DMAC channel number.
 * @retval The mask of channel status. Users need to use the @ref
 *         _dmac_channel_interrupt_flag type to decode the return variables.
 */
uint32_t DMAC_GetChannelInterruptStatus(DMAC_Type *base, dmac_channel_t channel);

/*!
 * @brief Clears the DMAC channel status.
 *
 * @param base DMAC peripheral base address.
 * @param channel DMAC channel number.
 * @param status The mask of channel status to be cleared. Users need to use
 *             the defined @ref _dmac_channel_interrupt_flag type.
 */
void DMAC_ClearChannelInterruptStatus(DMAC_Type *base, dmac_channel_t channel, uint32_t status);

/*! @} */

/*!
 * @name DMAC Transactional Interfaces
 * @{
 */

/*!
 * @brief Creates the DMAC channel handle.
 *
 * This function is called if using the transactional API for DMAC. This function
 * initializes the internal state of the DMAC handle.
 *
 * @param base DMAC peripheral base address.
 * @param handle DMAC handle pointer. The DMAC handle stores Callback function and
 *               parameters.
 * @param channel DMAC channel number.
 *
 */
void DMAC_TransferCreateHandle(DMAC_Type *base, dmac_handle_t *handle, dmac_channel_t channel);

/*!
 * @brief Sets the DMAC Callback function.
 *
 * This callback is called in the DMAC IRQ handler. Use the callback to do something
 * after the current transfer complete.
 *
 * @param handle DMAC handle pointer.
 * @param callback DMAC callback function pointer.
 * @param userData Parameter for callback function. If it is not needed, just set to NULL.
 */
void DMAC_TransferSetCallback(dmac_handle_t *handle, dmac_transfer_callback_t callback, void *userData);

/*!
 * @brief Submits the DMAC transfer configurations.
 *
 * @param handle DMAC handle pointer
 * @param transfer pointer to user's DMAC channel configure structure,see @ref dmac_channel_transfer_config_t for
 * detail.
 *
 * @retval #kStatus_Success It means submit transfer request succeed
 * @retval #kStatus_Fail It means submit transfer request failed
 */
status_t DMAC_TransferSubmit(dmac_handle_t *handle, dmac_channel_transfer_config_t *transfer);

/*!
 * @brief DMAC starts transfer.
 *
 * This function enables the channel request. Users can call this function after submitting the transfer request
 * or before submitting the transfer request.
 *
 * @param handle DMAC handle pointer.
 */
void DMAC_TransferStart(dmac_handle_t *handle);

/*!
 * @brief DMAC stops transfer.
 *
 * This function disables the channel request to pause the transfer. Users can call DMAC_StartTransfer()
 * again to resume the transfer.
 *
 * @param handle DMAC handle pointer.
 */
void DMAC_TransferStop(dmac_handle_t *handle);

/*!
 * @brief DMAC IRQ handler for the transfer completion.
 *
 * @param base DMAC peripheral base address.
 */
void DMAC_TransferHandleIRQ(DMAC_Type *base);

/*! @} */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/*! @} */

#endif /*_FSL_DMAC_H_*/
