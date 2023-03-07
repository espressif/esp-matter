/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_SSP_H_
#define _FSL_SSP_H_

#include "fsl_common.h"

/*!
 * @addtogroup ssp_driver
 *  @{
 */

/******************************************************************************
 * Definitions.
 *****************************************************************************/
/*! @name Driver version */
/*@{*/
/*! @brief Driver version 2.0.0. */
#define FSL_SSP_DRIVER_VERSION (MAKE_VERSION(2U, 0U, 0U))
#ifndef SSP_XFER_QUEUE_SIZE
/*! @brief SSP xfer queue size definition */
#define SSP_XFER_QUEUE_SIZE 4U
#endif

/*! @brief transfer status */
enum
{
    kSSP_TransferStatusTxIdle        = MAKE_STATUS(kStatusGroup_SSP, 0), /*!< ssp status idle */
    kSSP_TransferStatusRxIdle        = MAKE_STATUS(kStatusGroup_SSP, 1), /*!< ssp status idle */
    kSSP_TransferStatusBusy          = MAKE_STATUS(kStatusGroup_SSP, 2), /*!< ssp status busy */
    kSSP_TransferStatusBitCountError = MAKE_STATUS(kStatusGroup_SSP, 3), /*!< ssp status bit count error */
    kSSP_TransferStatusTxFifoError   = MAKE_STATUS(kStatusGroup_SSP, 4), /*!< ssp status tx fifo error */
    kSSP_TransferStatusRxFifoError   = MAKE_STATUS(kStatusGroup_SSP, 5), /*!< ssp status rx fifo error */
};

/*! @brief ssp mode */
typedef enum _ssp_transcevier_mode
{
    kSSP_TransmitReceiveMode, /*!< ssp tansmit and receive mode */
    kSSP_ReceiveMode,         /*!< ssp receive without transmit mode */
} ssp_transcevier_mode_t;

/*! @brief ssp interrupt mask
 * @anchor _ssp_interrupt_mask
 */
enum
{
    kSSP_InterruptTransmitFifoUnderrun = 1U,  /*!< transmit fifo underrun */
    kSSP_InterruptReceiveFifoOverrun   = 2U,  /*!< receive fifo underrun */
    kSSP_InterruptBitCountError        = 4U,  /*!< bit count error  */
    kSSP_InterruptTransmitFifoReady    = 8U,  /*!< transmit fifo ready for write */
    kSSP_InterruptReceiveFifoReady     = 16U, /*!< receive fifo ready for read */
};

/*! @brief ssp frame format */
typedef enum _ssp_transceiver_format
{
    kSSP_TransceiverFormatSPI = 0U, /*!< frame format serial Peripheral Interface */
    kSSP_TransceiverFormatPSP = 3U, /*!< frame format programmable serial protocol */
} ssp_transceiver_format_t;

/*! @brief ssp clock direction */
typedef enum _ssp_clock_direction
{
    kSSP_ClockDirectionMaster = 0U, /*!< SSP clock direction master */
    kSSP_ClockDirectionSlave,       /*!< SSP clock direction slave */
} ssp_clock_direction_t;

/*! @brief ssp data size */
typedef enum _ssp_data_width
{
    kSSP_DataSize8Bit  = 0x7U,               /*!< SSP data size 8 bit */
    kSSP_DataSize16Bit = 0xFU,               /*!< SSP data size 16 bit */
    kSSP_DataSize18Bit = 1U | (1U << 20U),   /*!< SSP data size 18 bit */
    kSSP_DataSize32Bit = 0xFU | (1U << 20U), /*!< SSP data size 32 bit */
} ssp_data_width_t;

/*! @brief ssp txd last phase state */
typedef enum _ssp_txd_last_phase_state
{
    kSSP_TxdLastPhase3StatedHalfOfLSBClockCycle =
        0U, /*!< txdx is 3-stated 1/2 clock cycle after the beginning of the LSB */
    kSSP_TxdLastPhase3StatedOnLSBEndClockEdge = 1U, /*!< txdx is 3-stated on the clock edge that ends the LSB */
} ssp_txd_last_phase_state_t;

/*! @brief ssp txd last phase state */
typedef enum _ssp_txd_idle_state
{
    kSSP_TxdIdleStateIsNot3Stated = 0U, /*!< txdx is not 3-stated when not transmitting data */
    kSSP_TxdIdleState3Stated      = 1U, /*!< txdx is 3-stated when not transmitting data */
} ssp_txd_idle_state_t;

/*! @brief ssp txd end transfer state */
typedef enum _ssp_txd_end_transfer_state
{
    kSSP_TxdEndTransferState0 = 0U, /*!< txd end transfer state 0 */
    kSSP_TxdEndTransferState1 = 1U, /*!< txd end transfer state 1 */
} ssp_txd_end_transfer_state_t;

/*! @brief ssp clk polarity */
typedef enum _ssp_clock_polarity
{
    kSSP_ClockPolarityLow  = 0U, /*!< clock polarity low */
    kSSP_ClockPolarityHigh = 1U, /*!< clock polarity high */
} ssp_clock_polarity_t;

/*! @brief ssp bit clock mode */
typedef enum _ssp_bit_clock_mode
{
    kSPP_BitClockIdleLowDrivenFallingSampleRising =
        0U, /*!< data driven on falling edge, sample on rising edge, ide state low */
    kSPP_BitClockIdleLowDrivenRisingSampleFalling =
        1U, /*!< data driven on rising edge, sample on falling edge, ide state low */
    kSPP_BitClockIdleHighDrivenRasingSampleFalling =
        2U, /*!< data driven on rising edge, sample on falling edge, ide state high */
    kSPP_BitClockIdleHighDrivenFallingSampleRising =
        3U, /*!< data driven on falling edge, sample on rising edge, ide state high */
} ssp_bit_clock_mode_t;

/*! @brief ssp frame sync relative timing bit */
typedef enum _ssp_frame_sync_relative_timing
{
    kSSP_FrameSyncAssertAfterEndOfDMTStop            = 0U, /*!< next frame assert after the end of the DMTSTOP timing */
    kSSP_FrameSyncAssertWithTheLsbOfThePreviousFrame = 1U, /*!< next frame assert with the LSB of previous frame */
} ssp_frame_sync_relative_timing_t;

/*! @brief ssp spi clk phase */
typedef enum _ssp_spi_clock_phase
{
    kSSP_SpiClkPhase0 = 0U, /*!< motorola clk inactive until one cycle after the
                              start of frame and active until 1/2 cycle before the end of a frame */

    kSSP_SpiClkPhase1 = 1U, /*!< motorola clk inactive until 1/2 cycle after the
                              start of frame and active until 1 cycle before the end of a frame */
} ssp_spi_clock_phase_t;

/*! @brief ssp motorola spi mode configuration */
typedef struct _ssp_spi_mode_config
{
    ssp_clock_polarity_t spiClockPolarity; /*!< SPI clock polarity */
    ssp_spi_clock_phase_t spiClockPhase;   /*!< SPI clock pahse */
    ssp_clock_direction_t
        spiClockDirection; /*!< SPI clock direction for serial bit clock direction and frame clock direction */

    ssp_data_width_t dataWidth; /*!< data width 8/16/18/32 bits */

    ssp_txd_idle_state_t txdIdleState;            /*!< txd state when idle */
    ssp_txd_last_phase_state_t txdLastPhaseState; /*!< txd last phase state */
} ssp_spi_mode_config_t;

/*! @brief ssp psp mode configuration */
typedef struct _ssp_mode_psp_config
{
    ssp_clock_direction_t bitClockDirection; /*!< bit clock generate internally or input externally */
    ssp_bit_clock_mode_t bitClockMode;       /*!< bit clock mode */

    ssp_clock_direction_t frameClockDirection;            /*!< frame clock generate internally or input externally */
    ssp_clock_polarity_t frameClockPolarity;              /*!< frame clock polarity */
    ssp_frame_sync_relative_timing_t frameRelativeTiming; /*!< frame sync relative timing */

    ssp_txd_idle_state_t txdIdleState;                /*!< txd state when idle */
    ssp_txd_last_phase_state_t txdLastPhaseState;     /*!< txd last phase state */
    ssp_txd_end_transfer_state_t txdEndTransferState; /*!< txd end transfer state */

    ssp_data_width_t dataWidth; /*!< data width 8/16/18/32 bits */
    uint8_t frameLength;        /*!< frame length, valid range 1-0x3F SSPSCLKx cycles */

    uint8_t dummyStartDelay; /*!< The MSB of the dummy start delay */
    uint8_t dummyStopDelay;  /*!< The MSB of the dummy stop delay */
    uint8_t startDelay;      /*!< number of 0-7 non-active clocks that defines the duration of idle time */
    uint8_t frameDelay;      /*!< Programmed value specifies the number (0 -127) of active one-half
                                 clocks (SSPSCLKx) asserted from the most-significant bit of TXDx
                                 (output) or RXD (input) being driven to SSPSFRMx. */

    bool enableNetWorkMode;   /*!< network should be enabled when emulating I2S protocol*/
    uint8_t frameSlotsNumber; /*!< the tims slots number per frame, 1-8 can be specified */
    uint8_t
        txActiveSlotMask; /*!< the each bit in the txActiveSlotMask represent a time slot
                          0 is the SSP does not transmit data in this time slot, 1 is the SSP does transmit data in this
                          time slot, so txActiveSlotMask = 3 means slot 0 and 1 is enabled, other slots are disabled */
    uint8_t
        rxActiveSlotMask; /*!< the each bit in the txActiveSlotMask represent a time slot
                          0 is the SSP does not transmit data in this time slot, 1 is the SSP does transmit data in this
                          time slot, so txActiveSlotMask = 3 means slot 0 and 1 is enabled, other slots are disabled */
} ssp_psp_mode_config_t;

/*! @brief ssp configuration */
typedef struct _ssp_config
{
    bool enableRXHalfCycleDelay;     /*!< true is enable rx half cycle delay, false is disable */
    bool enableMasterClockReturn;    /*!< true is enable master clock delay, false is disable */
    bool enableSlaveClockFreeRuning; /*!< true is clock input to SSPSCLKx is only active during data transfer
                                      * false is clock input to SSPSCLKx is continuously runing*/

    bool enableFifoPackingMode; /*!< fifo packing mode */
    uint8_t txFifoWaterMark;    /*!< tx fifo watermark value */
    uint8_t rxFifoWaterMark;    /*!< rx fifo watermark value */

    ssp_transcevier_mode_t mode;     /*!< transceiver work mode */
    ssp_transceiver_format_t format; /*!< transceiver work format */
    void *transceiverConfig;         /*!< transceiver configuration pointer, should be a pointer of
                                        - ssp_psp_mode_config_t, can be used to simulate i2s
                                        - ssp_spi_mode_config_t, can be used to simulate spi */
} ssp_config_t;

/*! @brief ssp transfer content */
typedef struct _ssp_transfer
{
    void *dataBuffer;  /*!< the pointer of data buffer */
    uint32_t dataSize; /*!< the size of data to be transferred */
} ssp_transfer_t;

/*! @brief SSP handle typedef. */
typedef struct _ssp_handle ssp_handle_t;

/*! @brief SSP callback pointer*/
typedef void (*ssp_transfer_callback_t)(ssp_handle_t *handle, status_t transferStatus, void *userData);

/*!
 * @brief SSP handle.
 *
 * Defines the structure to save the SSP state information and callback function.
 * A ssp_handle variable can be used to represent either SSP TX or SSP RX.
 *
 */
struct _ssp_handle
{
    SSP_Type *base; /*!< ssp base address */

    uint32_t bitWidth;         /*!< bit width */
    uint32_t sspFifoWatermark; /*!< ssp fifo WaterMark */

    ssp_transfer_t sspQueue[SSP_XFER_QUEUE_SIZE]; /*!< ssp queue storing queued transfer */
    volatile uint8_t sspQueueUser;                /*!< Index for user to queue transfer */
    volatile uint8_t sspQueueDriver;              /*!< Index for driver to get the transfer data and size */

    ssp_transfer_callback_t callback; /*!< Callback function. */
    void *userData;                   /*!< Parameter for transfer complete callback. */
};

/*************************************************************************************************
 * API
 ************************************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name SSP Initialization and deinitialization
 * @{
 */

/*!
 * @brief SSP get default configuration.
 *
 * @code
 *  config->enableRXHalfCycleDelay     = false;
 *  config->enableMasterClockReturn    = false;
 *  config->enableFifoPackingMode      = false;
 *  config->enableSlaveClockFreeRuning = false;
 *  config->txFifoWaterMark            = SSP_FIFO_COUNT(32) / 2U;
 *  config->rxFifoWaterMark            = SSP_FIFO_COUNT(32) / 2U;
 *  config->mode                       = kSSP_TransmitReceiveMode;
 *  config->format                     = kSSP_TransceiverFormatPSP;
 * @endcode
 *
 * @param config the pointer of ssp configuration structure.
 */
void SSP_GetDefaultConfig(ssp_config_t *config);

/*!
 * @brief SSP module initialization function.
 *
 * Configures the SSP according to the user configuration.
 *
 * @param base SSP peripheral base address.
 * @param config SSP configuration information.
 */
void SSP_Init(SSP_Type *base, const ssp_config_t *config);

/*!
 * @brief Deinitializes the SSP.
 *
 * @param base SSP peripheral base address.
 */
void SSP_Deinit(SSP_Type *base);

/*!
 * @brief Get SSP instance index.
 *
 * Used for SSP DMA driver only.
 *
 * @param base SSP peripheral base address.
 */
uint32_t SSP_GetInstance(SSP_Type *base);

/* @} */

/*!
 * @name SSP bus functional interface
 * @{
 */

/*!
 * @brief SSP get default SPI mode configuration.
 *
 * The default SSP SPI mode configurations:
 * @code
 *  config->spiClockPolarity  = kSSP_ClockPolarityLow;
 *  config->spiClockPhase     = kSSP_SpiClkPhase0;
 *  config->spiClockDirection = kSSP_ClockDirectionMaster;
 *  config->dataWidth         = kSSP_DataSize8Bit;
 *  config->txdIdleState      = kSSP_TxdIdleStateIsNot3Stated;
 *  config->txdLastPhaseState = kSSP_TxdLastPhase3StatedHalfOfLSBClockCycle;
 * @endcode
 *
 * @param config the pointer of ssp configuration structure.
 */
void SSP_GetDefaultSPIModeConfig(ssp_spi_mode_config_t *config);

/*!
 * @brief SSP set SPI mode configuration.
 *
 * @param base SSP peripheral base address.
 * @param config the pointer of ssp spi mode configuration structure.
 */
void SSP_SetSPIModeConfig(SSP_Type *base, const ssp_spi_mode_config_t *config);

/*!
 * @brief SSP get default PSP mode configuration.
 *
 * The default SSP PSP mode configurations are 16bit, 2 slots, i2s protocol.
 * @code
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
 * @endcode
 *
 * @param config the pointer of PSP configuration structure.
 */
void SSP_GetDefaultPSPModeConfig(ssp_psp_mode_config_t *config);

/*!
 * @brief SSP set PSP mode configuration.
 *
 * @param base SSP peripheral base address.
 * @param config the pointer of ssp psp mode configuration structure.
 */
void SSP_SetPSPModeConfig(SSP_Type *base, const ssp_psp_mode_config_t *config);
/*!
 * @brief SSP enable/disable loop back mode.
 *
 * The tx serial shifter is internally connected to input of RX serial shifter, the loopback mode used for test purpose
 * only.
 *
 * @param base SSP peripheral base address.
 * @param enable true is enable the loop back mode, false is disable the loop back mode.
 */
static inline void SSP_EnableLoopBackMode(SSP_Type *base, bool enable)
{
    if (enable)
    {
        base->SSP_SSCR1 |= SSP_SSP_SSCR1_LBM_MASK;
    }
    else
    {
        base->SSP_SSCR1 &= ~SSP_SSP_SSCR1_LBM_MASK;
    }
}

/*!
 * @brief SSP write data fifo.
 *
 * When a data sample size of less than 32-bits is selected, or 16 bits for packed mode, software
 * should right-justify the data that is written to the SSP Data Register for automatic insertion into the
 * TXFIFO. The transmit logic left-justifies the data and ignores any unused bits, cannot perform a write in
 * packed mode of less than 32 bits wide.
 *
 * @param base SSP peripheral base address.
 * @param data data to write.
 */
static inline void SSP_WriteData(SSP_Type *base, uint32_t data)
{
    base->SSP_SSDR = data;
}

/*!
 * @brief SSP read data fifo.
 *
 * Received data of less than 32 bits is right-justified automatically in the RXFIFO
 *
 * @param base SSP peripheral base address.
 * @return data value read from fifo.
 */
static inline uint32_t SSP_ReadData(SSP_Type *base)
{
    return base->SSP_SSDR;
}

/*!
 * @brief SSP enable/disable.
 *
 * @param base SSP peripheral base address.
 * @param enable true is enable, false is disable.
 */
static inline void SSP_Enable(SSP_Type *base, bool enable)
{
    if (enable)
    {
        base->SSP_SSCR0 |= SSP_SSP_SSCR0_SSE_MASK;
    }
    else
    {
        base->SSP_SSCR0 &= ~SSP_SSP_SSCR0_SSE_MASK;
    }
}

/* @} */

/*!
 * @name SSP interrupt functional interface
 * @{
 */
/*! @brief SSP enable interrupt.
 *
 * @param base SSP peripheral base address
 * @param interruptMask the Interrupt to be enabled, can a value or OR value of @ref _ssp_interrupt_mask.
 */
void SSP_EnableInterrupt(SSP_Type *base, uint32_t interruptMask);

/*! @brief SSP disable interrupt.
 *
 * @param base SSP peripheral base address
 * @param interruptMask the Interrupt to be disable, can a value or OR value of @ref _ssp_interrupt_mask.
 */
void SSP_DisableInterrupt(SSP_Type *base, uint32_t interruptMask);

/*! @brief SSP clear interrupt status.
 *
 * Note: Transmit fifo ready flag and receive fifo flag do not support clear by software.
 *
 * @param base SSP peripheral base address
 * @param interruptMask the Interrupt to be cleared, can a value or OR'ed value of @ref _ssp_interrupt_mask.
 */
void SSP_ClearInterruptStatus(SSP_Type *base, uint32_t interruptMask);

/*! brief SSP get interrupt status.
 *
 * @param base SSP peripheral base address
 * @return interrupt status flag, a value or OR'ed value of @ref _ssp_interrupt_mask.
 */
uint32_t SSP_GetInterruptStatus(SSP_Type *base);

/* @} */

/*!
 * @name SSP dma functional interface
 * @{
 */
/*!
 * @brief SSP enable/disable tx DMA requst.
 *
 * @param base SSP peripheral base address.
 * @param enable true is enable TX dma request, false is disable
 */
static inline void SSP_EnableSendDMARequest(SSP_Type *base, bool enable)
{
    if (enable)
    {
        base->SSP_SSCR1 |= SSP_SSP_SSCR1_TSRE_MASK;
    }
    else
    {
        base->SSP_SSCR1 &= ~SSP_SSP_SSCR1_TSRE_MASK;
    }
}

/*!
 * @brief SSP enable/disable rx DMA requst.
 *
 * @param base SSP peripheral base address.
 * @param enable true is enable RX dma request, false is disable
 */
static inline void SSP_EnableReceiveDMARequest(SSP_Type *base, bool enable)
{
    if (enable)
    {
        base->SSP_SSCR1 |= SSP_SSP_SSCR1_RSRE_MASK;
    }
    else
    {
        base->SSP_SSCR1 &= ~SSP_SSP_SSCR1_RSRE_MASK;
    }
}

/*!
 * @brief SSP get data fifo address.
 *
 * @param base SSP peripheral base address.
 * @return the ssp data fifo register address.
 */
static inline uint32_t SSP_GetFifoAddress(SSP_Type *base)
{
    return (uint32_t)&base->SSP_SSDR;
}
/* @} */

/*!
 * @name SSP transactional interface
 * @{
 */
/*!
 * @brief Creates the SSP send handle.
 *
 * @param base SSP peripheral base address.
 * @param handle SSP handle pointer.
 * @param callback Structure pointer to contain all callback functions.
 * @param userData Callback function parameter.
 */
void SSP_TransferSendCreateHandle(SSP_Type *base,
                                  ssp_handle_t *handle,
                                  ssp_transfer_callback_t callback,
                                  void *userData);

/*!
 * @brief Creates the SSP receive handle.
 *
 * @param base SSP peripheral base address.
 * @param handle SSP handle pointer.
 * @param callback Structure pointer to contain all callback functions.
 * @param userData Callback function parameter.
 */
void SSP_TransferReceiveCreateHandle(SSP_Type *base,
                                     ssp_handle_t *handle,
                                     ssp_transfer_callback_t callback,
                                     void *userData);

/*!
 * @brief Send the data using an interrupt and an asynchronous method.
 *
 * This function sends data and returns immediately. It doesn't wait for the transfer to complete or
 * to encounter an error. The application can submit multiple transfer requests without waiting previous
 * transfer request complete, since the SSP driver maintained a software transfer queue, the sizeof the queue
 * is configurable by macro SSP_XFER_QUEUE_SIZE which indicates how many request can be submitted at the same time.
 *
 * @note Call API @ref SSP_TransferSendCreateHandle when calling this API.
 *
 * @param handle SSP handle.
 * @param transfer transfer pointer.
 * @retval #kStatus_InvalidArgument Argument is invalid.
 * @retval #kSSP_TransferStatusBusy Busy transferring.
 * @retval #kStatus_Success Operate successfully.
 */
status_t SSP_TransferSendNonBlocking(ssp_handle_t *handle, ssp_transfer_t *transfer);

/*!
 * @brief Receive the data using an interrupt and an asynchronous method.
 *
 * This function sends data and returns immediately. It doesn't wait for the transfer to complete or
 * to encounter an error. The application can submit multiple transfer requests without waiting previous
 * transfer request complete, since the SSP driver maintained a software transfer queue, the sizeof the queue
 * is configurable by macro SSP_XFER_QUEUE_SIZE which indicates how many request can be submitted at the same time.
 *
 * @note Call API @ref SSP_TransferReceiveCreateHandle when calling this API.
 *
 * @param handle SSP handle.
 * @param transfer transfer pointer.
 * @retval #kStatus_InvalidArgument Argument is invalid.
 * @retval #kSSP_TransferStatusBusy Busy transferring.
 * @retval #kStatus_Success Operate successfully.
 */
status_t SSP_TransferReceiveNonBlocking(ssp_handle_t *handle, ssp_transfer_t *transfer);

/*!
 * @brief Aborts the current send.
 *
 * @note This API can be called any time when an interrupt non-blocking transfer initiates
 * to abort the transfer early.
 *
 * @param handle Pointer to the ssp_handle_t structure which stores the transfer state.
 */
void SSP_TransferAbortSend(ssp_handle_t *handle);

/*!
 * @brief Aborts the current IRQ receive.
 *
 * @note This API can be called when an interrupt non-blocking transfer initiates
 * to abort the transfer early.
 *
 * @param handle Pointer to the ssp_handle_t structure which stores the transfer state.
 */
void SSP_TransferAbortReceive(ssp_handle_t *handle);

/*!
 * @brief IRQ handler for the SSP transmitter.
 *
 * This function deals with the IRQs on the given host controller.
 *
 * @param handle SSP handle.
 */
void SSP_TransferSendHandleIRQ(ssp_handle_t *handle);

/*!
 * @brief IRQ handler for the SSP receiver.
 *
 * This function deals with the IRQs on the given host controller.
 *
 * @param handle SSP handle.
 */
void SSP_TransferReceiveHandleIRQ(ssp_handle_t *handle);

/* @} */
#if defined(__cplusplus)
}
#endif
/*! @} */

#endif /* _FSL_SSP_H_ */
