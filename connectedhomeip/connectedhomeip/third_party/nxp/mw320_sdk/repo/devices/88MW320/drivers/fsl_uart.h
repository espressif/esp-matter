/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_UART_H_
#define _FSL_UART_H_

#include "fsl_common.h"

/*!
 * @addtogroup uart_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief UART driver version. */
#define FSL_UART_DRIVER_VERSION (MAKE_VERSION(2, 0, 1))
/*@}*/

/* ============================================= General Control Macro ============================================== */
/*! @brief Retry times for waiting flag. */
#ifndef UART_RETRY_TIMES
#define UART_RETRY_TIMES 0U /* Defining to zero means to keep waiting for the flag until it is asserts/deasserts. */
#endif

/*! @brief Error codes for the UART driver. */
enum
{
    kStatus_UART_TxBusy                  = MAKE_STATUS(kStatusGroup_UART, 0), /*!< TX busy */
    kStatus_UART_RxBusy                  = MAKE_STATUS(kStatusGroup_UART, 1), /*!< RX busy */
    kStatus_UART_TxIdle                  = MAKE_STATUS(kStatusGroup_UART, 2), /*!< UART transmitter is idle. */
    kStatus_UART_RxIdle                  = MAKE_STATUS(kStatusGroup_UART, 3), /*!< UART receiver is idle. */
    kStatus_UART_FlagCannotClearManually = MAKE_STATUS(kStatusGroup_UART, 4), /*!< Some flag can't manually clear */
    kStatus_UART_Error                   = MAKE_STATUS(kStatusGroup_UART, 5), /*!< Error happens on UART. */
    kStatus_UART_RxRingBufferOverrun = MAKE_STATUS(kStatusGroup_UART, 6),  /*!< UART RX software ring buffer overrun. */
    kStatus_UART_RxHardwareOverrun   = MAKE_STATUS(kStatusGroup_UART, 7),  /*!< UART RX receiver overrun. */
    kStatus_UART_FifoError           = MAKE_STATUS(kStatusGroup_UART, 8),  /*!< UART noise error. */
    kStatus_UART_FramingError        = MAKE_STATUS(kStatusGroup_UART, 9),  /*!< UART framing error. */
    kStatus_UART_ParityError         = MAKE_STATUS(kStatusGroup_UART, 10), /*!< UART parity error. */
    kStatus_UART_BaudrateNotSupport =
        MAKE_STATUS(kStatusGroup_UART, 11), /*!< Baudrate is not support in current clock source */
    kStatus_UART_IdleLineDetected = MAKE_STATUS(kStatusGroup_UART, 12), /*!< IDLE flag. */
    kStatus_UART_Timeout          = MAKE_STATUS(kStatusGroup_UART, 13), /*!< UART times out. */
    kStatus_UART_RxFifoError      = MAKE_STATUS(kStatusGroup_UART, 14), /*!< UART Rx FIFO error. */
};

/* ================================== Functional Group Structure/Enumeration List =================================== */
/* ----------------------------------------------- Interrupt Sub-group ---------------------------------------------- */
/*!
 * @brief UART interrupt configuration structure, default settings all disabled.
 *
 * This structure contains the settings for all of the UART interrupt configurations.
 */
enum _uart_interrupt_enable
{
    kUART_RxDataReadyInterruptEnable =
        (UART_IER_RAVIE_MASK), /*!< Receiver data ready interrupt, caused by rx register full when FIFO disabled or rx
                                  data count above water mark when FIFO enabled. bit 0 */
    kUART_TxDataRequestInterruptEnable =
        (UART_IER_TIE_MASK), /*!< Transmitter data request interrupt, caused by tx register empty when FIFO disabled or
                                tx data count below water mark when FIFO enabled. bit 1 */
    kUART_RxStatusInterruptEnable =
        (UART_IER_RLSE_MASK), /*!< Receiver line status interrupt, caused by such status as rx over run, framing/parity
                                 error and break detect. bit 2 */
    kUART_ModemStatusChangeInterruptEnable =
        (UART_IER_MIE_MASK), /*!< Modem status change interrupt, caused by RTS/CTS pin status change. bit 3 */
    kUART_RxTimeoutInterruptEnable = (UART_IER_RTOIE_MASK), /*!< Receiver timeout interrupt. bit 4 */
    kUART_AutoBaudCompleteInterruptEnable =
        (UART_ABR_ABLIE_MASK << 4), /*!< Auto baud completes programing baud rate divisor interrupt. bit 5 */
};

/* ---------------------------------------- Hardware Status Flags Sub-group ----------------------------------------- */
/*!
 * @brief UART status flags.
 *
 * This provides constants for the UART status flags for use in the UART functions.
 */
enum _uart_status_flags
{
    kUART_RxDataReadyInterruptFlag =
        (UART_LSR_DR_MASK), /*!< Receiver data ready flag, self cleared by reading data out. bit 0 */
    kUART_RxOverrunInterruptFlag = (UART_LSR_OE_MASK),   /*!< Receiver overrun flag, can cause rx status interrupt, slef
                                                            cleared when reading the LSR(line status register). bit 1 */
    kUART_ParityErrorInterruptFlag = (UART_LSR_PE_MASK), /*!< Parity error flag, can cause rx status interrupt, self
                                                            cleared when reading the LSR(line status register). bit 2 */
    kUART_FramingErrorInterruptFLag =
        (UART_LSR_FE_MASK), /*!< Framing error flag, can cause rx status interrupt, self cleared when reading the
                               LSR(line status register). bit 3 */
    kUART_BreakDetectInterruptFlag = (UART_LSR_BI_MASK), /*!< Break detect flag, can cause rx status interrupt, self
                                                            cleared when reading the LSR(line status register). bit 4 */
    kUART_TxDataRequestInterruptFlag =
        (UART_LSR_TDRQ_MASK), /*!< Transmitter data request flag, self cleared by pushing data to tx FIFO until data
                                 count exceed water mark. bit 5 */
    kUART_TxEmptyInterruptFlag =
        (UART_LSR_TEMT_MASK), /*!< Transmitter empty flag, self cleared by pushing data to tx FIFO. bit 6 */
    kUART_FifoErrorInterruptFlag =
        (UART_LSR_FIFOE_MASK), /*!< FIFO error flag, self cleared by empting the rx FIFO. bit 7 */
    kUART_RxTimeoutInterruptFlag = (UART_IIR_TOD_MASK << 8), /*!< Receiver timeout interrupt flag. bit 11 */
    kUART_AutoBaudCompleteInterruptFlag =
        (UART_IIR_ABL_MASK << 8), /*!< Auto baud completes programing baud rate divisor flag. bit 12 */
};

/* --------------------------------------- Peripheral Configuration Sub-group --------------------------------------- */
/*! @brief UART parity mode. */
typedef enum _uart_parity_mode
{
    kUART_ParityDisabled = 0x0U, /*!< Parity disabled, bit setting: LCR[PEN] = 0 */
    kUART_ParityOdd      = 0x1U, /*!< Parity enabled, type odd,  bit setting: LCR[EPS]|[PEN] = 01 */
    kUART_ParityEven     = 0x2U, /*!< Parity enabled, type even, bit setting: LCR[EPS]|[PEN] = 11 */
} uart_parity_mode_t;

/*! @brief UART data bits count. */
typedef enum _uart_data_bits
{
    kUART_Databits5 = 0x0U, /*!< UART data bits length: 5 bits */
    kUART_Databits6,        /*!< UART data bits length: 6 bits */
    kUART_Databits7,        /*!< UART data bits length: 7 bits */
    kUART_Databits8,        /*!< UART data bits length: 8 bits */
} uart_data_bits_t;

/*! @brief UART stop bit count. */
typedef enum _uart_stop_bit_count
{
    kUART_Stopbits1, /*!< UART stop bits length: 1 bits */
    kUART_Stopbits2, /*!< UART stop bits length: 2 bits */
} uart_stop_bit_count_t;

/*!
 * @brief UART TX FIFO watermark level definition
 */
typedef enum _uart_txfifo_watermark
{
    kUART_TxfifoHalfEmpty = 0U, /*!< UART Transmit Interrupt Level: half empty */
    kUART_TxfifoEmpty,          /*!< UART Transmit Interrupt Level: full empty */
} uart_txfifo_watermark_t;

/*!
 * @brief UART RX FIFO watermark level definition
 */
typedef enum _uart_rxfifo_watermark
{
    kUART_RxfifoByte1 = 0U, /*!< 1 byte or more in rx fifo causes interrupt(Not valid in dma mode)  */
    kUART_RxfifoBytes8,     /*!< 8 byte or more in rx fifo causes interrupt */
    kUART_RxfifoBytes16,    /*!< 16 byte or more in rx fifo causes interrupt */
    kUART_RxfifoBytes32,    /*!< 32 byte or more in rx fifo causes interrupt */
} uart_rxfifo_watermark_t;

/*!
 * @brief UART FIFO Configuration Structure definition
 */
typedef struct _uart_fifo_config
{
    uart_txfifo_watermark_t txFifoWatermark; /*!< TX FIFO watermark. FCR[TIL] */
    uart_rxfifo_watermark_t rxFifoWatermark; /*!< RX FIFO watermark. FCR[ITL] */
    bool resetTxFifo;                        /*!< Tx FIFO reset. FCR[RESETTF] */
    bool resetRxFifo;                        /*!< Rx FIFO reset. FCR[RESETRF] */
    bool fifoEnable;                         /*!< FIFO enable. FCR[TRFIFOE] */
} uart_fifo_config_t;

/*!
 * @brief UART Auto Baud Calculation type definition
 */
typedef enum _uart_autobaud_program
{
    kUART_BaudrateProgramManual = 0, /*!< User calculates and programs UART divisor latch register using the value UART
                                        stored in Auto-Baud Count Register. */
    kUART_BaudrateProgramAuto,       /*!< UART programs UART Divisor Latch Register automatically. */
} uart_autobaud_program_t;

/*!
 * @brief UART Auto Baud Calculation type definition
 *
 * When choosing kUART_BaudrateProgramAuto for baud rate program method, this is used to configure the baudrate
 * calculation method of UART.
 */
typedef enum _uart_autobaud_calculation
{
    kUART_BaudrateCalFormula = 0, /*!< Formula is used to calculate baud rate, any baud rate can be programmed by the
                                     UART but could fail below 28.8 Kbps. */
    kUART_BaudrateCalTable, /*!< Table is used to calculate baud rate, allows any baud rate above 28.8 Kbps. Below 28.8
                               Kbps the only baud rates that can be programmed by the UART are 19200, 14400, 9600, 4800,
                               1200, and 300. */
} uart_autobaud_calculation_t;

/*!
 * @brief UART Auto baud detect and set Configuration Structure definition
 */
typedef struct _uart_autobaud_config
{
    bool autoBaudEnable;                     /*!< Enable or Disable Auto Baud Function. ABR[ABE] */
    uart_autobaud_program_t baudProgramType; /*!< Uart Auto Baud Program type. ABR[ABUP] */
    uart_autobaud_calculation_t baudCalType; /*!< Uart Auto Baud calculation type. ABR[ABT] */
} uart_autobaud_config_t;

/*!
 * @brief UART Infrared mode ncoder/Decoder Polarity type definition
 */
typedef enum _uart_infrared_polarity
{
    kUART_IrPositivePulse = 0, /*!< UART IR Encoder/Decoder takes positive pulse as zero */
    kUART_IrNegativePulse,     /*!< UART IR Encoder/Decoder takes negative pulse as zero */
} uart_infrared_polarity_t;

/*!
 * @brief UART Infrared Transmit Pulse Width type definition
 */
typedef enum _uart_infrared_pulsewidth
{
    kUART_IrPulsewidth3div16 = 0, /*!< UART IR Transmit pulse width is 3/16 of a bit time width */
    kUART_IrPulsewidth1p6us,      /*!< UART IR Transmit pulse width is 1.6us width */
} uart_infrared_pulsewidth_t;

/*!
 * @brief UART Infrared Configuration Structure type definition
 */
typedef struct _uart_infrared_config
{
    bool txIrEnable;                         /*!< Enable or Disable Uart Transmit IR Function. ISR[XMITIR] */
    bool rxIrEnable;                         /*!< Enable or Disable Uart Receive IR Function. ISR[RCVEIR] */
    uart_infrared_polarity_t txIrPolarity;   /*!< Transmit IR Polarity type. ISR[TXPL] */
    uart_infrared_polarity_t rxIrPolarity;   /*!< Receive IR Polarity type. ISR[RXPL] */
    uart_infrared_pulsewidth_t irPulseWidth; /*!< IR Pulse Width type. ISR[XMODE] */
} uart_infrared_config_t;

/* ----------------------------------- Module Initialize/De-initialize Sub-group ------------------------------------ */
/*! @brief UART configuration structure. */
typedef struct _uart_config
{
    uint32_t baudRate_Bps;         /*!< UART baud rate. DLL, DLH  */
    uart_parity_mode_t parityMode; /*!< Parity mode, disabled (default), even, odd. LCR[EPS][PEN] */
    bool enableStickyParity; /*!< Sticky parity, when enabled the parity bit remains unchanged and is opposite to PEN
                                bit. LCR[STKYP] */
    uart_data_bits_t dataBitCount;         /*!< Data bits count, eight (default), seven. LCR[WLS10] */
    uart_stop_bit_count_t stopBitCount;    /*!< Number of stop bits, 1 stop bit (default) or 2 stop bits. LCR[STB]  */
    uart_fifo_config_t fifoConfig;         /*!< FIFO configuration. */
    uart_autobaud_config_t autoBaudConfig; /*!< Auto baud detect and set configuration. */
    uart_infrared_config_t infraredConfig; /*!< Infrared function configuration. */
    bool enableHighSpeed;                  /*!< High speed enable. IER[HSE] = 1 */
    bool enableLoop;                       /*!< Loop mode MCR[LOOP] */
    bool enableTxCTS;                      /*!< TX CTS enable. MCR[AFE] = 1 */
    bool enableRxRTS;                      /*!< RX RTS enable. To enable RTS, CTS must be enabled. MCR[AFE][RTS] = 11 */
    bool enable;                           /*!< Enable UART. IER[UUE] */
} uart_config_t;

/* ================================= Transactional Group Structure/Enumeration List  ================================ */
/* Forward declaration of the handle typedef. */
typedef struct _uart_handle_t uart_handle_t;

/*!
 * @brief UART interrupt transfer callback function definition
 *
 * Defines the interface of user callback function used in UART interrupt transfer using transactional APIs. The
 * callback function shall be defined and declared in application level by user. Before starting UART transmiting or
 * receiving by calling @ref UART_TransferSendNonBlocking or @ref UART_TransferReceiveNonBlocking, call
 * @ref UART_TransferCreateHandle to install the user callback. When the transmiting or receiving ends or any bus error
 * like hardware overrun occurs, user callback will be invoked by driver.
 *
 * @param base UART peripheral base address.
 * @param handle Transfer handle.
 * @param status Transfer status
 * @param userData Pointer to user specified data or structure etc.
 */
typedef void (*uart_transfer_callback_t)(UART_Type *base, uart_handle_t *handle, status_t status, void *userData);

/*!
 * @brief UART transfer handle
 *
 * @note If user wants to use the transactional API to transfer data in interrupt way, one UART instance should and can
 * only be allocated one handle.
 * @note The handle is maintained by UART driver internally, which means the transfer state is retained and user shall
 * not modify its state #txState or #rxState in application level. If user only wish to use transactional APIs
 * without understanding its machanism, it is not necessary to understand these members.
 */
struct _uart_handle_t
{
    uint8_t dataMask;
    uint8_t *volatile txData;   /*!< Address of remaining data to send. */
    volatile size_t txDataSize; /*!< Size of the remaining data to send. */
    size_t txDataSizeAll;       /*!< Size of the data to send out. */
    uint8_t *volatile rxData;   /*!< Address of remaining data to receive. */
    volatile size_t rxDataSize; /*!< Size of the remaining data to receive. */
    size_t rxDataSizeAll;       /*!< Size of the data to receive. */

    uint8_t *rxRingBuffer;              /*!< Start address of the receiver ring buffer. */
    size_t rxRingBufferSize;            /*!< Size of the ring buffer. */
    volatile uint16_t rxRingBufferHead; /*!< Index for the driver to store received data into ring buffer. */
    volatile uint16_t rxRingBufferTail; /*!< Index for the user to get data from the ring buffer. */

    uart_transfer_callback_t callback; /*!< Callback function. */
    void *userData;                    /*!< uart callback function parameter.*/

    volatile uint8_t txState; /*!< TX transfer state. */
    volatile uint8_t rxState; /*!< RX transfer state. */
};

/*! @brief UART transfer structure. */
typedef struct _uart_transfer
{
    uint8_t *data;     /*!< The buffer pointer of data to be transferred.*/
    uint32_t dataSize; /*!< The byte count to be transferred. */
} uart_transfer_t;
/* ============================== End of Transactional Group Structure/Enumeration List  ============================ */

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/*!
 * @brief Get the UART instance from peripheral base address.
 *
 * @param base UART peripheral base address.
 * @return UART instance.
 */
uint32_t UART_GetInstance(UART_Type *base);

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Initializes an UART instance with the user configuration structure and the peripheral clock frequency.
 *
 * This function configures the UART module with user-defined settings. Call the UART_GetDefaultConfig function
 * to get the configuration structure configured with default settings.
 * The example below shows how to use this API to configure the UART.
 * @code
 *  uart_config_t uartConfig;
 *  UART_GetDefaultConfig(&uartConfig);
 *  UART_Init(UART1, &uartConfig, 20000000U);
 * @endcode
 *
 * @param base UART peripheral base address.
 * @param config Pointer to a user-defined configuration structure.
 * @param srcClock_Hz UART clock source frequency in Hz.
 * @retval kStatus_UART_BaudrateNotSupport Baudrate is not support in current clock source.
 * @retval kStatus_Success UART initialize succeed
 */
status_t UART_Init(UART_Type *base, const uart_config_t *config, uint32_t srcClock_Hz);

/*!
 * @brief Deinitializes a UART instance.
 *
 * This function waits for transmit to complete, disables TX and RX, and disables the UART clock.
 *
 * @param base UART peripheral base address.
 */
void UART_Deinit(UART_Type *base);

/*!
 * @brief Gets the default configuration structure.
 *
 * This function initializes the UART configuration structure to a default value.
 * The example below shows how to use this API to configure the UART.
 * @code
 *  uart_config_t uartConfig;
 *  UART_GetDefaultConfig(&uartConfig);
 *  UART_Init(UART1, &uartConfig, 20000000U);
 * @endcode
 *
 * @param config Pointer to a configuration structure.
 */
void UART_GetDefaultConfig(uart_config_t *config);
/* @} */

/*!
 * @name Hardware Status Flags
 * @{
 */
/*!
 * @brief Gets UART hardware status flags.
 *
 * @param base UART peripheral base address.
 * @return UART status flags, can be a single flag or several flags in #_uart_status_flags combined by OR.
 */
uint16_t UART_GetStatusFlags(UART_Type *base);
/* @} */

/*!
 * @name Interrupt
 * @{
 */
/*!
 * @brief Enables UART interrupts according to the provided mask.
 *
 * This function enables the UART interrupts according to the provided mask. The mask is a logical OR of enumeration
 * members in #_uart_interrupt_enable.
 *
 * @param base UART peripheral base address.
 * @param interrupts The interrupt source mask, can be a single source or several sources in #_uart_interrupt_enable
 * combined by OR.
 */
void UART_EnableInterrupts(UART_Type *base, uint8_t interrupts);

/*!
 * @brief Disables UART interrupts according to the provided mask.
 *
 * This function disables the UART interrupts according to the provided mask. The mask is a logical OR of enumeration
 * members in #_uart_interrupt_enable.
 *
 * @param base UART peripheral base address.
 * @param interrupts The interrupt source mask, can be a single source or several sources in #_uart_interrupt_enable
 * combined by OR.
 */
void UART_DisableInterrupts(UART_Type *base, uint8_t interrupts);

/*!
 * @brief Gets the enabled UART interrupts.
 *
 * This function gets the enabled UART interrupts. The enabled interrupts are returned as the logical OR value of the
 * enumerators #_uart_interrupt_enable.
 *
 * @param base UART peripheral base address.
 * @return The interrupt source mask, can be a single source or several sources in #_uart_interrupt_enable combined
 * by OR.
 */
uint8_t UART_GetEnabledInterrupts(UART_Type *base);
/* @} */

/*!
 * @name General Peripheral Configuration
 * @{
 */
/*!
 * @brief Enables or disables the UART module.
 *
 * @param base UART peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void UART_Enable(UART_Type *base, bool enable)
{
    if (enable)
    {
        base->IER |= UART_IER_UUE_MASK;
    }
    else
    {
        base->IER &= ~UART_IER_UUE_MASK;
    }
}

/*!
 * @brief Enables or disables the UART high speed mode.
 *
 * @param base UART peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void UART_EnableHighSpeed(UART_Type *base, bool enable)
{
    if (enable)
    {
        base->IER |= UART_IER_HSE_MASK;
    }
    else
    {
        base->IER &= ~UART_IER_HSE_MASK;
    }
}

/*!
 * @brief Enables or disables the UART dma request.
 *
 * @param base UART peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void UART_EnableDMA(UART_Type *base, bool enable)
{
    if (enable)
    {
        base->IER |= UART_IER_DMAE_MASK;
    }
    else
    {
        base->IER &= ~UART_IER_DMAE_MASK;
    }
}

/*!
 * @brief Set the UART FIFO configuration
 *
 * @note The FIFO configuration register FCR is write only, so we have no way of getting current FIFO configuration.
 * In this case any modification of FCR must contain the full FIFO configuration, including tx/rx watermark, whether to
 * reset tx/rx FIFO, and tx/rx FIFO enable.
 *
 * @param base UART peripheral base address
 * @param fifoConfig Pointer to FIFO configuration structure
 */
static inline void UART_SetFifoConfig(UART_Type *base, uart_fifo_config_t *fifoConfig)
{
    base->FCR = UART_FCR_ITL(fifoConfig->rxFifoWatermark) | UART_FCR_TIL(fifoConfig->txFifoWatermark) |
                UART_FCR_RESETRF(fifoConfig->resetRxFifo) | UART_FCR_RESETTF(fifoConfig->resetTxFifo) |
                UART_FCR_TRFIFOE(fifoConfig->fifoEnable);
}

/*!
 * @brief Checks the UART FIFO enable status.
 *
 * @param base UART peripheral base address.
 * @return true is FIFO is enabled.
 */
static inline bool UART_IsFifoEnabled(UART_Type *base)
{
    return ((base->IIR & UART_IIR_FIFOES10_MASK) != 0U);
}

/*!
 * @brief Enables or disables the UART CTS input.
 *
 * @note The RTS output can not be enabled without enabling the CTS input, which means disabling CTS will
 * disables RTS as well.
 *
 * @param base UART peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void UART_EnablCts(UART_Type *base, bool enable)
{
    if (enable)
    {
        base->MCR |= UART_MCR_AFE_MASK;
    }
    else
    {
        base->MCR &= ~UART_MCR_AFE_MASK;
    }
}

/*!
 * @brief Enables or disables the UART RTS output.
 *
 * @param base UART peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void UART_EnableRts(UART_Type *base, bool enable)
{
    if (enable)
    {
        base->MCR |= UART_MCR_RTS_MASK;
    }
    else
    {
        base->MCR &= ~UART_MCR_RTS_MASK;
    }
}

/*!
 * @brief Checks the CTS pin status.
 *
 * @param base UART peripheral base address.
 * @return true is CTS pin is asserted meaning free to send data.
 */
static inline bool UART_IsCtsAsserted(UART_Type *base)
{
    return ((base->MSR & UART_MSR_CTS_MASK) == 0U);
}

/*!
 * @brief Enables or disables the UART loop mode.
 *
 * @param base UART peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void UART_EnableLoop(UART_Type *base, bool enable)
{
    if (enable)
    {
        base->MCR |= UART_MCR_LOOP_MASK;
    }
    else
    {
        base->MCR &= ~UART_MCR_LOOP_MASK;
    }
}

/*!
 * @brief Set the UART parity type
 *
 * @param base UART peripheral base address.
 * @param parity parity type
 */
static inline void UART_SetParity(UART_Type *base, uart_parity_mode_t parity)
{
    if (parity == kUART_ParityDisabled)
    {
        base->LCR &= ~UART_LCR_PEN_MASK;
    }
    else
    {
        base->LCR |= UART_LCR_PEN_MASK;
        if (parity == kUART_ParityOdd)
        {
            base->LCR &= ~UART_LCR_EPS_MASK;
        }
        else
        {
            base->LCR |= UART_LCR_EPS_MASK;
        }
    }
}

/*!
 * @brief Enables or disables sticky parity
 *
 * @param base UART peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void UART_EnableStickyParity(UART_Type *base, bool enable)
{
    if (enable)
    {
        base->LCR |= UART_LCR_STKYP_MASK;
    }
    else
    {
        base->LCR &= ~UART_LCR_STKYP_MASK;
    }
}

/*!
 * @brief Set the UART data bits
 *
 * @param base UART peripheral base address.
 * @param dataBits bits to set
 */
static inline void UART_SetDataBits(UART_Type *base, uart_data_bits_t dataBits)
{
    base->LCR |= (base->LCR & (~UART_LCR_WLS10_MASK)) | UART_LCR_WLS10((uint8_t)dataBits);
}

/*!
 * @brief Set the UART stop bits
 *
 * @param base UART peripheral base address.
 * @param stopBits stop bits to set
 */
static inline void UART_SetStopBits(UART_Type *base, uart_stop_bit_count_t stopBits)
{
    base->LCR |= (base->LCR & (~UART_LCR_STB_MASK)) | UART_LCR_STB((uint8_t)stopBits);
}

/*!
 * @brief Get current UART receive FIFO level.
 *
 * @param base Select the UART port, should be kUART1_Id, kUART2_Id.
 * @return The number of data entries in the receive FIFO
 */
static inline uint8_t UART_GetRxFifoLevel(UART_Type *base)
{
    return (uint8_t)base->RFOR;
}

/*!
 * @brief Sets the UART instance baudrate.
 *
 * This API configures the UART module baudrate. This API can be used to update
 * the UART module baudrate after the UART module is initialized by the UART_Init.
 * @code
 *  UART_SetBaudRate(UART1, 115200U, 20000000U);
 * @endcode
 *
 * @param base UART peripheral base address.
 * @param baudRate_Bps UART baudrate to be set.
 * @param srcClock_Hz UART clock source frequency in HZ.
 * @retval kStatus_UART_BaudrateNotSupport Baudrate is not supported in the current clock source.
 * @retval kStatus_Success Set baudrate succeeded.
 */
status_t UART_SetBaudRate(UART_Type *base, uint32_t baudRate_Bps, uint32_t srcClock_Hz);

/*!
 * @brief Sets the UART auto baud configuration.
 *
 * This API configures the UART auto baud function, including enable/disable, calculation type, program type.
 *
 * @param base UART peripheral base address.
 * @param autoBaudConfig pointer to UART auto baud configuration.
 */
void UART_SetAutoBaudConfig(UART_Type *base, uart_autobaud_config_t *autoBaudConfig);

/*!
 * @brief Get UART auto baud count number.
 *
 * @param base UART peripheral base address
 * @return none
 */
static inline uint16_t UART_GetAutoBaudCount(UART_Type *base)
{
    return (uint16_t)base->ACR;
}

/*!
 * @brief Set DL register according to Auto Baud Detect Count value.
 *
 * @param base UART peripheral base address.
 */
void UART_SetAutoBaud(UART_Type *base);

/*!
 * @brief Sets the UART auto baud configuration.
 *
 * This API configures the UART auto baud function, including enable/disable, calculation type, program type.
 *
 * @param base UART peripheral base address.
 * @param infraredConfig pointer to UART infrared configuration.
 */
void UART_SetInfraredConfig(UART_Type *base, uart_infrared_config_t *infraredConfig);
/* @} */

/*!
 * @name Bus Operation
 * @{
 */
/*!
 * @brief Gets the UART receive data register byte address.
 *
 * This function returns the UART receive data register address, which is mainly used by DMA/eDMA.
 *
 * @param base UART peripheral base address.
 * @return UART data register byte addresses which are used both by the transmitter and the receiver.
 */
static inline uint32_t UART_GetRxDataRegisterAddress(UART_Type *base)
{
    return (uint32_t)((uint8_t *)&(base->RBR));
}

/*!
 * @brief Gets the UART transmit data register byte address.
 *
 * This function returns the UART transmit data register address, which is mainly used by DMA/eDMA.
 *
 * @param base UART peripheral base address.
 * @return UART data register byte addresses which are used both by the transmitter and the receiver.
 */
static inline uint32_t UART_GetTxDataRegisterAddress(UART_Type *base)
{
    return (uint32_t)((uint8_t *)&(base->THR));
}

/*!
 * @brief Writes to the TX register.
 *
 * This function writes data to the TX register directly. The upper layer must ensure
 * that the TX register is empty or TX FIFO has room before calling this function.
 *
 * @param base UART peripheral base address.
 * @param data The byte to write.
 */
static inline void UART_WriteByte(UART_Type *base, uint8_t data)
{
    base->THR = data;
}

/*!
 * @brief Reads the RX register directly.
 *
 * This function reads data from the RX register directly. The upper layer must
 * ensure that the RX register is full or that the TX FIFO has data before calling this function.
 *
 * @param base UART peripheral base address.
 * @return The byte read from UART data register.
 */
static inline uint8_t UART_ReadByte(UART_Type *base)
{
    return (uint8_t)(base->RBR);
}

/*!
 * @brief Send 9 bit data to the UART peripheral.
 *
 * Before calling this API, make sure parity and sticky parity are enabled, and tx register empty or room in tx FIFO.
 *
 * @param base UART peripheral base address.
 * @param data The data to be sent
 */
static inline void UART_Write9bits(UART_Type *base, uint16_t data)
{
    if (((data >> 8U) & 0x1U) != 0U)
    {
        base->LCR &= ~UART_LCR_EPS_MASK;
    }
    else
    {
        base->LCR |= UART_LCR_EPS_MASK;
    }

    /* Send data */
    UART_WriteByte(base, (uint8_t)data);
}

/*!
 * @brief Read 9 bit data from the UART peripheral.
 *
 * Before calling this API, make sure parity is enabled, and rx register full or data available in rx FIFO.
 *
 * @param base UART peripheral base address.
 * @return The received data
 */
uint16_t UART_Read9bits(UART_Type *base);

/*!
 * @brief Writes TX register using a blocking method.
 *
 * This function polls the TX register, waits TX register to be empty or TX FIFO
 * have room then writes data to the TX buffer.
 *
 * @param base UART peripheral base address.
 * @param data Start address of the data to write.
 * @param length Size of the data to write.
 */
status_t UART_WriteBlocking(UART_Type *base, const uint8_t *data, uint32_t length);

/*!
 * @brief Reads RX data register using a blocking method.
 *
 * This function polls the RX register, waits RX register to be full or RX FIFO
 * have data, then reads data from the RX register.
 *
 * @param base UART peripheral base address.
 * @param data Start address of the buffer to store the received data.
 * @param length Size of the buffer.
 * @retval kStatus_Fail Receiver error occurred while receiving data.
 * @retval kStatus_UART_RxHardwareOverrun Receiver overrun occurred while receiving data
 * @retval kStatus_UART_NoiseError Noise error occurred while receiving data
 * @retval kStatus_UART_FramingErrorFraming error occurred while receiving data
 * @retval kStatus_UART_ParityError Parity error occurred while receiving data
 * @retval kStatus_Success Successfully received all data.
 */
status_t UART_ReadBlocking(UART_Type *base, uint8_t *data, uint32_t length);

/*!
 * @brief Enables or disables break condition.
 *
 * When break condition is enabled tx pin is pulled low by UART, when disabled the pin is high.
 *
 * @param base UART peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void UART_EnableBreakCondition(UART_Type *base, bool enable)
{
    if (enable)
    {
        base->LCR |= UART_LCR_SB_MASK;
    }
    else
    {
        base->LCR &= ~UART_LCR_SB_MASK;
    }
}
/* @} */

/*!
 * @name Transactional
 * @{
 */
/*!
 * @brief Initializes the UART handle.
 *
 * This function initializes the UART handle, which can be used for other UART
 * transactional APIs. Usually, for a specified UART instance,
 * call this API once to get the initialized handle.
 *
 * The UART driver supports the "background" receiving, which means that user can set up
 * an RX ring buffer optionally. Data received is stored into the ring buffer even when the
 * user doesn't call the UART_TransferReceiveNonBlocking() API. If there is already data received
 * in the ring buffer, the user can get the received data from the ring buffer directly.
 * The ring buffer is disabled if passing NULL as @p ringBuffer.
 *
 * @param base UART peripheral base address.
 * @param handle UART handle pointer.
 * @param callback Callback function.
 * @param userData User data.
 */
void UART_TransferCreateHandle(UART_Type *base,
                               uart_handle_t *handle,
                               uart_transfer_callback_t callback,
                               void *userData);

/*!
 * @brief Sets up the RX ring buffer.
 *
 * This function sets up the RX ring buffer to a specific UART handle.
 *
 * When the RX ring buffer is used, data received is stored into the ring buffer even when
 * the user doesn't call the UART_TransferReceiveNonBlocking() API. If there is already data received
 * in the ring buffer, the user can get the received data from the ring buffer directly.
 *
 * @note When using RX ring buffer, one byte is reserved for internal use. In other
 * words, if @p ringBufferSize is 32, then only 31 bytes are used for saving data.
 *
 * @param base UART peripheral base address.
 * @param handle UART handle pointer.
 * @param ringBuffer Start address of ring buffer for background receiving. Pass NULL to disable the ring buffer.
 * @param ringBufferSize size of the ring buffer.
 */
void UART_TransferStartRingBuffer(UART_Type *base, uart_handle_t *handle, uint8_t *ringBuffer, size_t ringBufferSize);

/*!
 * @brief Aborts the background transfer and uninstalls the ring buffer.
 *
 * This function aborts the background transfer and uninstalls the ring buffer.
 *
 * @param base UART peripheral base address.
 * @param handle UART handle pointer.
 */
void UART_TransferStopRingBuffer(UART_Type *base, uart_handle_t *handle);

/*!
 * @brief Get the length of received data in RX ring buffer.
 *
 * @param base UART peripheral base address.
 * @param handle UART handle pointer.
 * @return Length of received data in RX ring buffer.
 */
size_t UART_TransferGetRxRingBufferLength(UART_Type *base, uart_handle_t *handle);

/*!
 * @brief Transmits a buffer of data using the interrupt method.
 *
 * This function send data using an interrupt method. This is a non-blocking function, which
 * returns directly without waiting for all data written to the transmitter register. When
 * all data is written to the TX register in the ISR, the UART driver calls the callback
 * function and passes the @ref kStatus_UART_TxIdle as status parameter.
 *
 * @note The kStatus_UART_TxIdle is passed to the upper layer when all data are written
 * to the TX register. However, there is no check to ensure that all the data sent out. Before disabling the TX,
 * check the kUART_TransmissionCompleteFlag to ensure that the transmit is finished.
 *
 * @param base UART peripheral base address.
 * @param handle UART handle pointer.
 * @param xfer UART transfer structure, see #uart_transfer_t.
 * @retval kStatus_Success Successfully start the data transmission.
 * @retval kStatus_UART_TxBusy Previous transmission still not finished, data not all written to the TX register.
 * @retval kStatus_InvalidArgument Invalid argument.
 */
status_t UART_TransferSendNonBlocking(UART_Type *base, uart_handle_t *handle, uart_transfer_t *xfer);

/*!
 * @brief Gets the number of bytes that have been sent out to bus.
 *
 * This function gets the number of bytes that have been sent out to bus by an interrupt method.
 *
 * @param base UART peripheral base address.
 * @param handle UART handle pointer.
 * @param count Send bytes count.
 * @retval kStatus_NoTransferInProgress No send in progress.
 * @retval kStatus_InvalidArgument Parameter is invalid.
 * @retval kStatus_Success Get successfully through the parameter \p count;
 */
status_t UART_TransferGetSendCount(UART_Type *base, uart_handle_t *handle, uint32_t *count);

/*!
 * @brief Aborts the interrupt-driven data transmit.
 *
 * This function aborts the interrupt driven data sending. The user can get the remainBtyes to find out
 * how many bytes are not sent out.
 *
 * @param base UART peripheral base address.
 * @param handle UART handle pointer.
 */
void UART_TransferAbortSend(UART_Type *base, uart_handle_t *handle);

/*!
 * @brief Receives a buffer of data using the interrupt method.
 *
 * This function receives data using an interrupt method. This is a non-blocking function
 * which returns without waiting to ensure that all data are received.
 * If the RX ring buffer is used and not empty, the data in the ring buffer is copied and
 * the parameter @p receivedBytes shows how many bytes are copied from the ring buffer.
 * After copying, if the data in the ring buffer is not enough for read, the receive
 * request is saved by the UART driver. When the new data arrives, the receive request
 * is serviced first. When all data is received, the UART driver notifies the upper layer
 * through a callback function and passes a status parameter kStatus_UART_RxIdle.
 * For example, the upper layer needs 10 bytes but there are only 5 bytes in ring buffer.
 * The 5 bytes are copied to xfer->data, which returns with the
 * parameter @p receivedBytes set to 5. For the remaining 5 bytes, the newly arrived data is
 * saved from xfer->data[5]. When 5 bytes are received, the UART driver notifies the upper layer.
 * If the RX ring buffer is not enabled, this function enables the RX and RX interrupt
 * to receive data to xfer->data. When all data is received, the upper layer is notified.
 *
 * @param base UART peripheral base address.
 * @param handle UART handle pointer.
 * @param xfer UART transfer structure, see uart_transfer_t.
 * @param receivedBytes Bytes received from the ring buffer directly.
 * @retval kStatus_Success Successfully queue the transfer into the transmit queue.
 * @retval kStatus_UART_RxBusy Previous receive request is not finished.
 * @retval kStatus_InvalidArgument Invalid argument.
 */
status_t UART_TransferReceiveNonBlocking(UART_Type *base,
                                         uart_handle_t *handle,
                                         uart_transfer_t *xfer,
                                         size_t *receivedBytes);

/*!
 * @brief Gets the number of bytes that have been received.
 *
 * This function gets the number of bytes that have been received.
 *
 * @param base UART peripheral base address.
 * @param handle UART handle pointer.
 * @param count Receive bytes count.
 * @retval kStatus_NoTransferInProgress No receive in progress.
 * @retval kStatus_InvalidArgument Parameter is invalid.
 * @retval kStatus_Success Get successfully through the parameter \p count;
 */
status_t UART_TransferGetReceiveCount(UART_Type *base, uart_handle_t *handle, uint32_t *count);

/*!
 * @brief Aborts the interrupt-driven data receiving.
 *
 * This function aborts the interrupt-driven data receiving. The user can get the remainBytes to find out
 * how many bytes not received yet.
 *
 * @param base UART peripheral base address.
 * @param handle UART handle pointer.
 */
void UART_TransferAbortReceive(UART_Type *base, uart_handle_t *handle);

/*!
 * @brief UART IRQ handle function.
 *
 * This function handles the UART transmit and receive IRQ request.
 *
 * @param base UART peripheral base address.
 * @param handle UART handle pointer.
 */
void UART_TransferHandleIRQ(UART_Type *base, uart_handle_t *handle);

/*!
 * @brief UART Error IRQ handle function.
 *
 * This function handles the UART error IRQ request.
 *
 * @param base UART peripheral base address.
 * @param handle UART handle pointer.
 */
void UART_TransferHandleErrorIRQ(UART_Type *base, uart_handle_t *handle);
/* @} */

#if defined(__cplusplus)
}
#endif

/*! @} End of group uart_driver*/

#endif /* _FSL_UART_H_ */
