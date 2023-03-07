/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_I3C_H_
#define _FSL_I3C_H_

#include <stddef.h>
#include "fsl_device_registers.h"
#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @addtogroup i3c
 * @{
 */

/*! @name Driver version */
/*@{*/
/*! @brief I3C driver version 2.0.0. */
#define FSL_I3C_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

/*! @brief Timeout times for waiting flag. */
#ifndef I3C_WAIT_TIMEOUT
#define I3C_WAIT_TIMEOUT 0U /* Define to zero means keep waiting until the flag is assert/deassert. */
#endif

/*! @brief I3C status return codes. */
enum _i3c_status
{
    kStatus_I3C_Busy = MAKE_STATUS(kStatusGroup_I3C, 0), /*!< The master is already performing a transfer. */
    kStatus_I3C_Idle = MAKE_STATUS(kStatusGroup_I3C, 1), /*!< The slave driver is idle. */
    kStatus_I3C_Nak  = MAKE_STATUS(kStatusGroup_I3C, 2), /*!< The slave device sent a NAK in response to an address. */
    kStatus_I3C_WriteAbort =
        MAKE_STATUS(kStatusGroup_I3C, 3), /*!< The slave device sent a NAK in response to a write. */
    kStatus_I3C_Term           = MAKE_STATUS(kStatusGroup_I3C, 4), /*!< The master terminates slave read. */
    kStatus_I3C_ParityError    = MAKE_STATUS(kStatusGroup_I3C, 5), /*!< Parity error from DDR read. */
    kStatus_I3C_CrcError       = MAKE_STATUS(kStatusGroup_I3C, 6), /*!< CRC error from DDR read. */
    kStatus_I3C_ReadFifoError  = MAKE_STATUS(kStatusGroup_I3C, 7), /*!< Read from MRDATAB register when FIFO empty. */
    kStatus_I3C_WriteFifoError = MAKE_STATUS(kStatusGroup_I3C, 8), /*!< Write to MWDATAB register when FIFO full. */
    kStatus_I3C_MsgError =
        MAKE_STATUS(kStatusGroup_I3C, 9), /*!< Message SDR/DDR mismatch or read/write message in wrong state */
    kStatus_I3C_InvalidReq = MAKE_STATUS(kStatusGroup_I3C, 10), /*!< Invalid use of request. */
    kStatus_I3C_Timeout    = MAKE_STATUS(kStatusGroup_I3C, 11), /*!< The module has stalled too long in a frame. */
};

/*! @} */

/*!
 * @addtogroup i3c_master_driver
 * @{
 */

/*!
 * @brief I3C master peripheral flags.
 *
 * The following status register flags can be cleared:
 * - #kI3C_MasterSlaveStartFlag
 * - #kI3C_MasterControlDoneFlag
 * - #kI3C_MasterCompleteFlag
 * - #kI3C_MasterArbitrationWonFlag
 * - #kI3C_MasterSlave2MasterFlag
 *
 * All flags except #kI3C_MasterBetweenFlag and #kI3C_MasterNackDetectFlag can be enabled as
 * interrupts.
 *
 * @note These enums are meant to be OR'd together to form a bit mask.
 */
enum _i3c_master_flags
{
    kI3C_MasterBetweenFlag        = I3C_MSTATUS_BETWEEN_MASK,   /*!< Between messages/DAAs flag */
    kI3C_MasterNackDetectFlag     = I3C_MSTATUS_NACKED_MASK,    /*!< NACK detected flag */
    kI3C_MasterSlaveStartFlag     = I3C_MSTATUS_SLVSTART_MASK,  /*!< Slave request start flag */
    kI3C_MasterControlDoneFlag    = I3C_MSTATUS_MCTRLDONE_MASK, /*!< Master request complete flag */
    kI3C_MasterCompleteFlag       = I3C_MSTATUS_COMPLETE_MASK,  /*!< Transfer complete flag */
    kI3C_MasterRxReadyFlag        = I3C_MSTATUS_RXPEND_MASK,    /*!< Rx data ready in Rx buffer flag */
    kI3C_MasterTxReadyFlag        = I3C_MSTATUS_TXNOTFULL_MASK, /*!< Tx buffer ready for Tx data flag */
    kI3C_MasterArbitrationWonFlag = I3C_MSTATUS_IBIWON_MASK,    /*!< Header address won arbitration flag */
    kI3C_MasterErrorFlag          = I3C_MSTATUS_ERRWARN_MASK,   /*!< Error occurred flag */
    kI3C_MasterSlave2MasterFlag   = I3C_MSTATUS_NOWMASTER_MASK, /*!< Switch from slave to master flag */
};

/*!
 * @brief I3C master error flags to indicate the causes.
 *
 * @note These enums are meant to be OR'd together to form a bit mask.
 */
enum _i3c_master_error_flags
{
    kI3C_MasterErrorNackFlag       = I3C_MERRWARN_NACK_MASK,    /*!< Slave NACKed the last address */
    kI3C_MasterErrorWriteAbortFlag = I3C_MERRWARN_WRABT_MASK,   /*!< Slave NACKed the write data */
    kI3C_MasterErrorTermFlag       = I3C_MERRWARN_TERM_MASK,    /*!< Master terminates slave read */
    kI3C_MasterErrorParityFlag     = I3C_MERRWARN_HPAR_MASK,    /*!< Parity error from DDR read */
    kI3C_MasterErrorCrcFlag        = I3C_MERRWARN_HCRC_MASK,    /*!< CRC error from DDR read */
    kI3C_MasterErrorReadFlag       = I3C_MERRWARN_OREAD_MASK,   /*!< Read from MRDATAB register when FIFO empty */
    kI3C_MasterErrorWriteFlag      = I3C_MERRWARN_OWRITE_MASK,  /*!< Write to MWDATAB register when FIFO full */
    kI3C_MasterErrorMsgFlag        = I3C_MERRWARN_MSGERR_MASK,  /*!< Message SDR/DDR mismatch or
            read/write message in wrong state */
    kI3C_MasterErrorInvalidReqFlag = I3C_MERRWARN_INVREQ_MASK,  /*!< Invalid use of request */
    kI3C_MasterErrorTimeoutFlag    = I3C_MERRWARN_TIMEOUT_MASK, /*!< The module has stalled too long in a frame */
};

/*! @brief I3C working master state. */
typedef enum _i3c_master_state
{
    kI3C_MasterStateIdle    = 0U, /*!< Bus stopped. */
    kI3C_MasterStateSlvReq  = 1U, /*!< Bus stopped but slave holding SDA low. */
    kI3C_MasterStateMsgSdr  = 2U, /*!< In SDR Message mode from using MWMSG_SDR. */
    kI3C_MasterStateNormAct = 3U, /*!< In normal active SDR mode. */
    kI3C_MasterStateDdr     = 4U, /*!< In DDR Message mode. */
    kI3C_MasterStateDaa     = 5U, /*!< In ENTDAA mode. */
    kI3C_MasterStateIbiAck  = 6U, /*!< Waiting on IBI ACK/NACK decision. */
    kI3C_MasterStateIbiRcv  = 7U, /*!< receiving IBI. */
} i3c_master_state_t;

/*! @brief I3C master enable configuration. */
typedef enum _i3c_master_enable
{
    kI3C_MasterOff     = 0U, /*!< Master off. */
    kI3C_MasterOn      = 1U, /*!< Master on. */
    kI3C_MasterCapable = 2U  /*!< Master capable. */
} i3c_master_enable_t;

/*! @brief I3C high keeper configuration. */
typedef enum _i3c_master_hkeep
{
    kI3C_MasterHighKeeperNone    = 0U, /*!< Use PUR to hold SCL high. */
    kI3C_MasterHighKeeperWiredIn = 1U, /*!< Use pin_HK controls. */
    kI3C_MasterPassiveSDA        = 2U, /*!< Hi-Z for Bus Free and hold SDA. */
    kI3C_MasterPassiveSDASCL     = 3U  /*!< Hi-Z both for Bus Free, and can Hi-Z SDA for hold. */
} i3c_master_hkeep_t;

/*! @brief Emits the requested operation when doing in pieces vs. by message. */
typedef enum _i3c_bus_request
{
    kI3C_RequestNone          = 0U, /*!< No request. */
    kI3C_RequestEmitStartAddr = 1U, /*!< Request to emit start and address on bus. */
    kI3C_RequestEmitStop      = 2U, /*!< Request to emit stop on bus. */
    kI3C_RequestIbiAckNack    = 3U, /*!< Manual IBI ACK or NACK. */
    kI3C_RequestProcessDAA    = 4U, /*!< Process DAA. */
    kI3C_RequestForceExit     = 6U, /*!< Request to force exit. */
    kI3C_RequestAutoIbi       = 7U, /*!< Hold in stopped state, but Auto-emit START,7E. */
} i3c_bus_request_t;

/*! @brief Bus type with EmitStartAddr. */
typedef enum _i3c_bus_type
{
    kI3C_TypeI3CSdr = 0U, /*!< SDR mode of I3C. */
    kI3C_TypeI2C    = 1U, /*!< Standard i2c protocol. */
    kI3C_TypeI3CDDR = 2U, /*!< HDR-DDR mode of I3C. */
} i3c_bus_type_t;

/*! @brief IBI response. */
typedef enum _i3c_ibi_response
{
    kI3C_IbiRespAck          = 0U, /*!< ACK with no mandatory byte. */
    kI3C_IbiRespNack         = 1U, /*!< NACK. */
    kI3C_IbiRespAckMandatory = 2U, /*!< ACK with mandatory byte. */
    kI3C_IbiRespManual       = 3U, /*!< Reserved. */
} i3c_ibi_response_t;

/*! @brief Direction of master and slave transfers. */
typedef enum _i3c_direction
{
    kI3C_Write = 0U, /*!< Master transmit. */
    kI3C_Read  = 1U  /*!< Master receive. */
} i3c_direction_t;

/*! @brief Watermark of TX int/dma trigger level. */
typedef enum _i3c_tx_trigger_level
{
    kI3C_TxTriggerOnEmpty               = 0U, /*!< Trigger on empty. */
    kI3C_TxTriggerUntilOneQuarterOrLess = 1U, /*!< Trigger on 1/4 full or less. */
    kI3C_TxTriggerUntilOneHalfOrLess    = 2U, /*!< Trigger on 1/2 full or less. */
    kI3C_TxTriggerUntilOneLessThanFull  = 3U, /*!< Trigger on 1 less than full or less. */
} i3c_tx_trigger_level_t;

/*! @brief Watermark of RX int/dma trigger level. */
typedef enum _i3c_rx_trigger_level
{
    kI3C_RxTriggerOnNotEmpty              = 0U, /*!< Trigger on not empty. */
    kI3C_RxTriggerUntilOneQuarterOrMore   = 1U, /*!< Trigger on 1/4 full or more. */
    kI3C_RxTriggerUntilOneHalfOrMore      = 2U, /*!< Trigger on 1/2 full or more. */
    kI3C_RxTriggerUntilThreeQuarterOrMore = 3U, /*!< Trigger on 3/4 full or more. */
} i3c_rx_trigger_level_t;

/*!
 * @brief Structure with settings to initialize the I3C master module.
 *
 * This structure holds configuration settings for the I3C peripheral. To initialize this
 * structure to reasonable defaults, call the I3C_MasterGetDefaultConfig() function and
 * pass a pointer to your configuration structure instance.
 *
 * The configuration structure can be made constant so it resides in flash.
 */
typedef struct _i3c_master_config
{
    i3c_master_enable_t enableMaster; /*!< Enable master mode. */
    bool disableTimeout;              /*!< Whether to disable timeout to prevent the ERRWARN. */
    i3c_master_hkeep_t hKeep;         /*!< High keeper mode setting. */
    bool enableOpenDrainStop;         /*!< Whether to emit open-drain speed STOP. */
    bool enableOpenDrainHigh;         /*!< Enable Open-Drain High to be 1 PPBAUD count for i3c messages, or 1 ODBAUD. */
    uint32_t baudRate_Hz;             /*!< Desired baud rate in Hertz. */
    i3c_bus_type_t busType;           /*!< bus type. */
} i3c_master_config_t;

/* Forward declaration of the transfer descriptor and handle typedefs. */
typedef struct _i3c_master_transfer i3c_master_transfer_t;
typedef struct _i3c_master_handle i3c_master_handle_t;

/*!
 * @brief Master completion callback function pointer type.
 *
 * This callback is used only for the non-blocking master transfer API. Specify the callback you wish to use
 * in the call to I3C_MasterTransferCreateHandle().
 *
 * @param base The I3C peripheral base address.
 * @param completionStatus Either #kStatus_Success or an error code describing how the transfer completed.
 * @param userData Arbitrary pointer-sized value passed from the application.
 */
typedef void (*i3c_master_transfer_callback_t)(I3C_Type *base,
                                               i3c_master_handle_t *handle,
                                               status_t completionStatus,
                                               void *userData);

/*!
 * @brief Transfer option flags.
 *
 * @note These enumerations are intended to be OR'd together to form a bit mask of options for
 * the #_i3c_master_transfer::flags field.
 */
enum _i3c_master_transfer_flags
{
    kI3C_TransferDefaultFlag       = 0x00U, /*!< Transfer starts with a start signal, stops with a stop signal. */
    kI3C_TransferNoStartFlag       = 0x01U, /*!< Don't send a start condition, address, and sub address */
    kI3C_TransferRepeatedStartFlag = 0x02U, /*!< Send a repeated start condition */
    kI3C_TransferNoStopFlag        = 0x04U, /*!< Don't send a stop condition. */
};

/*!
 * @brief Non-blocking transfer descriptor structure.
 *
 * This structure is used to pass transaction parameters to the I3C_MasterTransferNonBlocking() API.
 */
struct _i3c_master_transfer
{
    uint32_t flags; /*!< Bit mask of options for the transfer. See enumeration #_i3c_master_transfer_flags for available
                       options. Set to 0 or #kI3C_TransferDefaultFlag for normal transfers. */
    uint16_t slaveAddress;     /*!< The 7-bit slave address. */
    i3c_direction_t direction; /*!< Either #kI3C_Read or #kI3C_Write. */
    uint32_t subaddress;       /*!< Sub address. Transferred MSB first. */
    size_t subaddressSize;     /*!< Length of sub address to send in bytes. Maximum size is 4 bytes. */
    void *data;                /*!< Pointer to data to transfer. */
    size_t dataSize;           /*!< Number of bytes to transfer. */
};

/*!
 * @brief Driver handle for master non-blocking APIs.
 * @note The contents of this structure are private and subject to change.
 */
struct _i3c_master_handle
{
    uint8_t state;                                     /*!< Transfer state machine current state. */
    uint16_t remainingBytes;                           /*!< Remaining byte count in current state. */
    uint8_t *buf;                                      /*!< Buffer pointer for current state. */
    i3c_master_transfer_t transfer;                    /*!< Copy of the current transfer info. */
    i3c_master_transfer_callback_t completionCallback; /*!< Callback function pointer. */
    void *userData;                                    /*!< Application data passed to callback. */
};

/*! @} */

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @addtogroup i3c_master_driver
 * @{
 */

/*! @name Initialization and deinitialization */
/*@{*/

/*!
 * @brief Provides a default configuration for the I3C master peripheral.
 *
 * This function provides the following default configuration for the I3C master peripheral:
 * @code
 *  masterConfig->enableMaster            = kI3C_MasterOn;
 *  masterConfig->disableTimeout          = false;
 *  masterConfig->hKeep                   = kI3C_MasterHighKeeperNone;
 *  masterConfig->enableOpenDrainStop     = true;
 *  masterConfig->enableOpenDrainHigh     = true;
 *  masterConfig->baudRate_Hz             = 100000U;
 *  masterConfig->busType                 = kI3C_TypeI2C;
 * @endcode
 *
 * After calling this function, you can override any settings in order to customize the configuration,
 * prior to initializing the master driver with I3C_MasterInit().
 *
 * @param[out] masterConfig User provided configuration structure for default values. Refer to #i3c_master_config_t.
 */
void I3C_MasterGetDefaultConfig(i3c_master_config_t *masterConfig);

/*!
 * @brief Initializes the I3C master peripheral.
 *
 * This function enables the peripheral clock and initializes the I3C master peripheral as described by the user
 * provided configuration. A software reset is performed prior to configuration.
 *
 * @param base The I3C peripheral base address.
 * @param masterConfig User provided peripheral configuration. Use I3C_MasterGetDefaultConfig() to get a set of
 * defaults that you can override.
 * @param sourceClock_Hz Frequency in Hertz of the I3C functional clock. Used to calculate the baud rate divisors,
 *      filter widths, and timeout periods.
 */
void I3C_MasterInit(I3C_Type *base, const i3c_master_config_t *masterConfig, uint32_t sourceClock_Hz);

/*!
 * @brief Deinitializes the I3C master peripheral.
 *
 * This function disables the I3C master peripheral and gates the clock. It also performs a software
 * reset to restore the peripheral to reset conditions.
 *
 * @param base The I3C peripheral base address.
 */
void I3C_MasterDeinit(I3C_Type *base);

/* Not static so it can be used from fsl_i3c_edma.c. */
status_t I3C_MasterCheckAndClearError(I3C_Type *base, uint32_t status);

/* Not static so it can be used from fsl_i3c_edma.c. */
status_t I3C_CheckForBusyBus(I3C_Type *base);

/*!
 * @brief Set I3C module master mode.
 *
 * @param base The I3C peripheral base address.
 * @param enable Enable master mode.
 */
static inline void I3C_MasterEnable(I3C_Type *base, i3c_master_enable_t enable)
{
    base->MCONFIG = (base->MCONFIG & ~I3C_MCONFIG_MSTENA_MASK) | I3C_MCONFIG_MSTENA(enable);
}

/*@}*/

/*! @name Status */
/*@{*/

/*!
 * @brief Gets the I3C master status flags.
 *
 * A bit mask with the state of all I3C master status flags is returned. For each flag, the corresponding bit
 * in the return value is set if the flag is asserted.
 *
 * @param base The I3C peripheral base address.
 * @return State of the status flags:
 *         - 1: related status flag is set.
 *         - 0: related status flag is not set.
 * @see _i3c_master_flags
 */
static inline uint32_t I3C_MasterGetStatusFlags(I3C_Type *base)
{
    return base->MSTATUS & ~(I3C_MSTATUS_STATE_MASK | I3C_MSTATUS_IBITYPE_MASK);
}

/*!
 * @brief Clears the I3C master status flag state.
 *
 * The following status register flags can be cleared:
 * - #kI3C_MasterSlaveStartFlag
 * - #kI3C_MasterControlDoneFlag
 * - #kI3C_MasterCompleteFlag
 * - #kI3C_MasterArbitrationWonFlag
 * - #kI3C_MasterSlave2MasterFlag
 *
 * Attempts to clear other flags has no effect.
 *
 * @param base The I3C peripheral base address.
 * @param statusMask A bitmask of status flags that are to be cleared. The mask is composed of
 *  #_i3c_master_flags enumerators OR'd together. You may pass the result of a previous call to
 *  I3C_MasterGetStatusFlags().
 * @see _i3c_master_flags.
 */
static inline void I3C_MasterClearStatusFlags(I3C_Type *base, uint32_t statusMask)
{
    base->MSTATUS = statusMask;
}

/*!
 * @brief Gets the I3C master error status flags.
 *
 * A bit mask with the state of all I3C master error status flags is returned. For each flag, the corresponding bit
 * in the return value is set if the flag is asserted.
 *
 * @param base The I3C peripheral base address.
 * @return State of the error status flags:
 *         - 1: related status flag is set.
 *         - 0: related status flag is not set.
 * @see _i3c_master_error_flags
 */
static inline uint32_t I3C_MasterGetErrorStatusFlags(I3C_Type *base)
{
    return base->MERRWARN;
}

/*!
 * @brief Clears the I3C master error status flag state.
 *
 * @param base The I3C peripheral base address.
 * @param statusMask A bitmask of error status flags that are to be cleared. The mask is composed of
 *  #_i3c_master_error_flags enumerators OR'd together. You may pass the result of a previous call to
 *  I3C_MasterGetStatusFlags().
 * @see _i3c_master_error_flags.
 */
static inline void I3C_MasterClearErrorStatusFlags(I3C_Type *base, uint32_t statusMask)
{
    base->MERRWARN = statusMask;
}

/*!
 * @brief Gets the I3C master state.
 *
 * @param base The I3C peripheral base address.
 * @return I3C master state.
 */
static inline i3c_master_state_t I3C_MasterGetState(I3C_Type *base)
{
    return (i3c_master_state_t)((base->MSTATUS & I3C_MSTATUS_STATE_MASK) >> I3C_MSTATUS_STATE_SHIFT);
}

/*@}*/

/*! @name Interrupts */
/*@{*/

/*!
 * @brief Enables the I3C master interrupt requests.
 *
 * All flags except #kI3C_MasterBetweenFlag and #kI3C_MasterNackDetectFlag can be enabled as
 * interrupts.
 *
 * @param base The I3C peripheral base address.
 * @param interruptMask Bit mask of interrupts to enable. See #_i3c_master_flags for the set
 *      of constants that should be OR'd together to form the bit mask.
 */
static inline void I3C_MasterEnableInterrupts(I3C_Type *base, uint32_t interruptMask)
{
    base->MINTSET |= interruptMask;
}

/*!
 * @brief Disables the I3C master interrupt requests.
 *
 * All flags except #kI3C_MasterBetweenFlag and #kI3C_MasterNackDetectFlag can be enabled as
 * interrupts.
 *
 * @param base The I3C peripheral base address.
 * @param interruptMask Bit mask of interrupts to disable. See #_i3c_master_flags for the set
 *      of constants that should be OR'd together to form the bit mask.
 */
static inline void I3C_MasterDisableInterrupts(I3C_Type *base, uint32_t interruptMask)
{
    base->MINTCLR = interruptMask;
}

/*!
 * @brief Returns the set of currently enabled I3C master interrupt requests.
 *
 * @param base The I3C peripheral base address.
 * @return A bitmask composed of #_i3c_master_flags enumerators OR'd together to indicate the
 *      set of enabled interrupts.
 */
static inline uint32_t I3C_MasterGetEnabledInterrupts(I3C_Type *base)
{
    return base->MINTSET;
}

/*!
 * @brief Returns the set of pending I3C master interrupt requests.
 *
 * @param base The I3C peripheral base address.
 * @return A bitmask composed of #_i3c_master_flags enumerators OR'd together to indicate the
 *      set of pending interrupts.
 */
static inline uint32_t I3C_MasterGetPendingInterrupts(I3C_Type *base)
{
    return base->MINTMASKED;
}

/*@}*/

/*! @name DMA control */
/*@{*/

/*!
 * @brief Enables or disables I3C master DMA requests.
 *
 * @param base The I3C peripheral base address.
 * @param enableTx Enable flag for transmit DMA request. Pass true for enable, false for disable.
 * @param enableRx Enable flag for receive DMA request. Pass true for enable, false for disable.
 * @param width DMA read/write unit in bytes.
 */
static inline void I3C_MasterEnableDMA(I3C_Type *base, bool enableTx, bool enableRx, uint32_t width)
{
    assert(width <= 2);
    base->MDMACTRL =
        I3C_MDMACTRL_DMAFB(enableRx ? 2 : 0) | I3C_MDMACTRL_DMATB(enableTx ? 2 : 0) | I3C_MDMACTRL_DMAWIDTH(width);
}

/*!
 * @brief Gets I3C master transmit data register address for DMA transfer.
 *
 * @param base The I3C peripheral base address.
 * @param width DMA read/write unit in bytes.
 * @return The I3C Master Transmit Data Register address.
 */
static inline uint32_t I3C_MasterGetTxFifoAddress(I3C_Type *base, uint32_t width)
{
    assert(width <= 2);
    return (uint32_t)((width == 2) ? &base->MWDATAH : &base->MWDATAB);
}

/*!
 * @brief Gets I3C master receive data register address for DMA transfer.
 *
 * @param base The I3C peripheral base address.
 * @param width DMA read/write unit in bytes.
 * @return The I3C Master Receive Data Register address.
 */
static inline uint32_t I3C_MasterGetRxFifoAddress(I3C_Type *base, uint32_t width)
{
    assert(width <= 2);
    return (uint32_t)((width == 2) ? &base->MRDATAH : &base->MRDATAB);
}

/*@}*/

/*! @name FIFO control */
/*@{*/

/*!
 * @brief Sets the watermarks for I3C master FIFOs.
 *
 * @param base The I3C peripheral base address.
 * @param txLvl Transmit FIFO watermark level. The #kI3C_MasterTxReadyFlag flag is set whenever
 *      the number of words in the transmit FIFO reaches @a txLvl.
 * @param rxLvl Receive FIFO watermark level. The #kI3C_MasterRxReadyFlag flag is set whenever
 *      the number of words in the receive FIFO reaches @a rxLvl.
 * @param flushTx true if TX FIFO is to be cleared, otherwise TX FIFO remains unchanged.
 * @param flushRx true if RX FIFO is to be cleared, otherwise RX FIFO remains unchanged.
 */
static inline void I3C_MasterSetWatermarks(
    I3C_Type *base, i3c_tx_trigger_level_t txLvl, i3c_rx_trigger_level_t rxLvl, bool flushTx, bool flushRx)
{
    base->MDATACTRL = I3C_MDATACTRL_UNLOCK_MASK | I3C_MDATACTRL_TXTRIG(txLvl) | I3C_MDATACTRL_RXTRIG(rxLvl) |
                      (flushTx ? I3C_MDATACTRL_FLUSHTB_MASK : 0) | (flushRx ? I3C_MDATACTRL_FLUSHFB_MASK : 0);
}

/*!
 * @brief Gets the current number of bytes in the I3C master FIFOs.
 *
 * @param base The I3C peripheral base address.
 * @param[out] txCount Pointer through which the current number of bytes in the transmit FIFO is returned.
 *      Pass NULL if this value is not required.
 * @param[out] rxCount Pointer through which the current number of bytes in the receive FIFO is returned.
 *      Pass NULL if this value is not required.
 */
static inline void I3C_MasterGetFifoCounts(I3C_Type *base, size_t *rxCount, size_t *txCount)
{
    if (txCount)
    {
        *txCount = (base->MDATACTRL & I3C_MDATACTRL_TXCOUNT_MASK) >> I3C_MDATACTRL_TXCOUNT_SHIFT;
    }
    if (rxCount)
    {
        *rxCount = (base->MDATACTRL & I3C_MDATACTRL_RXCOUNT_MASK) >> I3C_MDATACTRL_RXCOUNT_SHIFT;
    }
}

/*@}*/

/*! @name Bus operations */
/*@{*/

/*!
 * @brief Sets the I3C bus frequency for master transactions.
 *
 * The I3C master is automatically disabled and re-enabled as necessary to configure the baud
 * rate. Do not call this function during a transfer, or the transfer is aborted.
 *
 * @param base The I3C peripheral base address.
 * @param baudRate_Hz Requested bus frequency in Hertz.
 * @param sourceClock_Hz I3C functional clock frequency in Hertz.
 */
void I3C_MasterSetBaudRate(I3C_Type *base, uint32_t baudRate_Hz, uint32_t sourceClock_Hz);

/*!
 * @brief Returns whether the bus is idle.
 *
 * Requires the master mode to be enabled.
 *
 * @param base The I3C peripheral base address.
 * @retval true Bus is busy.
 * @retval false Bus is idle.
 */
static inline bool I3C_MasterGetBusIdleState(I3C_Type *base)
{
    return ((base->MSTATUS & I3C_MSTATUS_STATE_MASK) == kI3C_MasterStateIdle);
}

/*!
 * @brief Sends a START signal and slave address on the I2C/I3C bus.
 *
 * This function is used to initiate a new master mode transfer. First, the bus state is checked to ensure
 * that another master is not occupying the bus. Then a START signal is transmitted, followed by the
 * 7-bit address specified in the @a address parameter. Note that this function does not actually wait
 * until the START and address are successfully sent on the bus before returning.
 *
 * @param base The I3C peripheral base address.
 * @param type The bus type to use in this transaction.
 * @param address 7-bit slave device address, in bits [6:0].
 * @param dir Master transfer direction, either #kI3C_Read or #kI3C_Write. This parameter is used to set
 *      the R/w bit (bit 0) in the transmitted slave address.
 * @param rxSize if dir is #kI3C_Read, this assigns bytes to read. Otherwise set to 0.
 * @retval #kStatus_Success START signal and address were successfully enqueued in the transmit FIFO.
 * @retval #kStatus_I3C_Busy Another master is currently utilizing the bus.
 */
status_t I3C_MasterStart(I3C_Type *base, i3c_bus_type_t type, uint8_t address, i3c_direction_t dir, uint32_t rxSize);

/*!
 * @brief Sends a repeated START signal and slave address on the I2C/I3C bus.
 *
 * This function is used to send a Repeated START signal when a transfer is already in progress. Like
 * I3C_MasterStart(), it also sends the specified 7-bit address.
 *
 * @note This function exists primarily to maintain compatible APIs between I3C and I2C drivers,
 *      as well as to better document the intent of code that uses these APIs.
 *
 * @param base The I3C peripheral base address.
 * @param type The bus type to use in this transaction.
 * @param address 7-bit slave device address, in bits [6:0].
 * @param dir Master transfer direction, either #kI3C_Read or #kI3C_Write. This parameter is used to set
 *      the R/w bit (bit 0) in the transmitted slave address.
 * @param rxSize if dir is #kI3C_Read, this assigns bytes to read. Otherwise set to 0.
 * @retval #kStatus_Success Repeated START signal and address were successfully enqueued in the transmit FIFO.
 */
status_t I3C_MasterRepeatedStart(
    I3C_Type *base, i3c_bus_type_t type, uint8_t address, i3c_direction_t dir, uint32_t rxSize);

/*!
 * @brief Performs a polling send transfer on the I2C/I3C bus.
 *
 * Sends up to @a txSize number of bytes to the previously addressed slave device. The slave may
 * reply with a NAK to any byte in order to terminate the transfer early. If this happens, this
 * function returns #kStatus_I3C_Nak.
 *
 * @param base  The I3C peripheral base address.
 * @param txBuff The pointer to the data to be transferred.
 * @param txSize The length in bytes of the data to be transferred.
 * @param flags Bit mask of options for the transfer. See enumeration #_i3c_master_transfer_flags for available options.
 * @retval #kStatus_Success Data was sent successfully.
 * @retval #kStatus_I3C_Busy Another master is currently utilizing the bus.
 * @retval #kStatus_I3C_Nak The slave device sent a NAK in response to a byte.
 * @retval #kStatus_I3C_FifoError FIFO under run or over run.
 * @retval #kStatus_I3C_ArbitrationLost Arbitration lost error.
 * @retval #kStatus_I3C_PinLowTimeout SCL or SDA were held low longer than the timeout.
 */
status_t I3C_MasterSend(I3C_Type *base, const void *txBuff, size_t txSize, uint32_t flags);

/*!
 * @brief Performs a polling receive transfer on the I2C/I3C bus.
 *
 * @param base  The I3C peripheral base address.
 * @param rxBuff The pointer to the data to be transferred.
 * @param rxSize The length in bytes of the data to be transferred.
 * @param flags Bit mask of options for the transfer. See enumeration #_i3c_master_transfer_flags for available options.
 * @retval #kStatus_Success Data was received successfully.
 * @retval #kStatus_I3C_Busy Another master is currently utilizing the bus.
 * @retval #kStatus_I3C_Nak The slave device sent a NAK in response to a byte.
 * @retval #kStatus_I3C_FifoError FIFO under run or overrun.
 * @retval #kStatus_I3C_ArbitrationLost Arbitration lost error.
 * @retval #kStatus_I3C_PinLowTimeout SCL or SDA were held low longer than the timeout.
 */
status_t I3C_MasterReceive(I3C_Type *base, void *rxBuff, size_t rxSize, uint32_t flags);

/*!
 * @brief Sends a STOP signal on the I2C/I3C bus.
 *
 * This function does not return until the STOP signal is seen on the bus, or an error occurs.
 *
 * @param base The I3C peripheral base address.
 * @retval #kStatus_Success The STOP signal was successfully sent on the bus and the transaction terminated.
 * @retval #kStatus_I3C_Busy Another master is currently utilizing the bus.
 * @retval #kStatus_I3C_Nak The slave device sent a NAK in response to a byte.
 * @retval #kStatus_I3C_FifoError FIFO under run or overrun.
 * @retval #kStatus_I3C_ArbitrationLost Arbitration lost error.
 * @retval #kStatus_I3C_PinLowTimeout SCL or SDA were held low longer than the timeout.
 */
status_t I3C_MasterStop(I3C_Type *base);

/*!
 * @brief Performs a master polling transfer on the I2C/I3C bus.
 *
 * @note The API does not return until the transfer succeeds or fails due
 * to error happens during transfer.
 *
 * @param base The I3C peripheral base address.
 * @param type The bus type to use in this transaction.
 * @param transfer Pointer to the transfer structure.
 * @retval #kStatus_Success Data was received successfully.
 * @retval #kStatus_I3C_Busy Another master is currently utilizing the bus.
 * @retval #kStatus_I3C_Nak The slave device sent a NAK in response to a byte.
 * @retval #kStatus_I3C_FifoError FIFO under run or overrun.
 * @retval #kStatus_I3C_ArbitrationLost Arbitration lost error.
 * @retval #kStatus_I3C_PinLowTimeout SCL or SDA were held low longer than the timeout.
 */
status_t I3C_MasterTransferBlocking(I3C_Type *base, i3c_bus_type_t type, i3c_master_transfer_t *transfer);

/*@}*/

/*! @name Non-blocking */
/*@{*/

/*!
 * @brief Creates a new handle for the I3C master non-blocking APIs.
 *
 * The creation of a handle is for use with the non-blocking APIs. Once a handle
 * is created, there is not a corresponding destroy handle. If the user wants to
 * terminate a transfer, the I3C_MasterTransferAbort() API shall be called.
 *
 *
 * @note The function also enables the NVIC IRQ for the input I3C. Need to notice
 * that on some SoCs the I3C IRQ is connected to INTMUX, in this case user needs to
 * enable the associated INTMUX IRQ in application.
 *
 * @param base The I3C peripheral base address.
 * @param[out] handle Pointer to the I3C master driver handle.
 * @param callback User provided pointer to the asynchronous callback function.
 * @param userData User provided pointer to the application callback data.
 */
void I3C_MasterTransferCreateHandle(I3C_Type *base,
                                    i3c_master_handle_t *handle,
                                    i3c_master_transfer_callback_t callback,
                                    void *userData);

/*!
 * @brief Performs a non-blocking transaction on the I2C/I3C bus.
 *
 * @param base The I3C peripheral base address.
 * @param handle Pointer to the I3C master driver handle.
 * @param transfer The pointer to the transfer descriptor.
 * @retval #kStatus_Success The transaction was started successfully.
 * @retval #kStatus_I3C_Busy Either another master is currently utilizing the bus, or a non-blocking
 *      transaction is already in progress.
 */
status_t I3C_MasterTransferNonBlocking(I3C_Type *base, i3c_master_handle_t *handle, i3c_master_transfer_t *transfer);

/*!
 * @brief Returns number of bytes transferred so far.
 * @param base The I3C peripheral base address.
 * @param handle Pointer to the I3C master driver handle.
 * @param[out] count Number of bytes transferred so far by the non-blocking transaction.
 * @retval #kStatus_Success
 * @retval #kStatus_NoTransferInProgress There is not a non-blocking transaction currently in progress.
 */
status_t I3C_MasterTransferGetCount(I3C_Type *base, i3c_master_handle_t *handle, size_t *count);

/*!
 * @brief Terminates a non-blocking I3C master transmission early.
 *
 * @note It is not safe to call this function from an IRQ handler that has a higher priority than the
 *      I3C peripheral's IRQ priority.
 *
 * @param base The I3C peripheral base address.
 * @param handle Pointer to the I3C master driver handle.
 * @retval #kStatus_Success A transaction was successfully aborted.
 * @retval #kStatus_I3C_Idle There is not a non-blocking transaction currently in progress.
 */
void I3C_MasterTransferAbort(I3C_Type *base, i3c_master_handle_t *handle);

/*@}*/

/*! @name IRQ handler */
/*@{*/

/*!
 * @brief Reusable routine to handle master interrupts.
 * @note This function does not need to be called unless you are reimplementing the
 *  nonblocking API's interrupt handler routines to add special functionality.
 * @param base The I3C peripheral base address.
 * @param handle Pointer to the I3C master driver handle.
 */
void I3C_MasterTransferHandleIRQ(I3C_Type *base, i3c_master_handle_t *handle);

/*@}*/

/*! @} */

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_I3C_H_ */
