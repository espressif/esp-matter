/*
 * Copyright 2020,2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_uart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.mw_uart"
#endif

/* UART transfer state. */
enum
{
    kUART_TxIdle,         /*!< TX idle. */
    kUART_TxBusy,         /*!< TX busy. */
    kUART_RxIdle,         /*!< RX idle. */
    kUART_RxBusy,         /*!< RX busy. */
    kUART_RxFramingError, /* Rx framing error */
    kUART_RxParityError   /* Rx parity error */
};

/* Typedef for interrupt handler. */
typedef void (*uart_isr_t)(UART_Type *base, uart_handle_t *handle);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Check whether the RX ring buffer is full.
 *
 * @userData handle UART handle pointer.
 * @retval true  RX ring buffer is full.
 * @retval false RX ring buffer is not full.
 */
static bool UART_TransferIsRxRingBufferFull(UART_Type *base, uart_handle_t *handle);

/*!
 * @brief Write to TX register using non-blocking method.
 *
 * This function writes data to the TX register directly, upper layer must make
 * sure the TX register is empty or TX FIFO has empty room before calling this function.
 *
 * @note This function does not check whether all the data has been sent out to bus,
 * so before disable TX, check kUART_TransmissionCompleteFlag to ensure the TX is
 * finished.
 *
 * @param base UART peripheral base address.
 * @param data Start address of the data to write.
 * @param length Size of the buffer to be sent.
 */
static void UART_WriteNonBlocking(UART_Type *base, const uint8_t *data, size_t length);

/*!
 * @brief Read RX register using non-blocking method.
 *
 * This function reads data from the TX register directly, upper layer must make
 * sure the RX register is full or TX FIFO has data before calling this function.
 *
 * @param base UART peripheral base address.
 * @param data Start address of the buffer to store the received data.
 * @param length Size of the buffer.
 */
static void UART_ReadNonBlocking(UART_Type *base, uint8_t *data, size_t length);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Array of UART peripheral base address. */
static UART_Type *const s_uartBases[] = UART_BASE_PTRS;
/* Array of UART handle. */
static uart_handle_t *s_uartHandle[ARRAY_SIZE(s_uartBases)];
/* Array of UART IRQ number. */
static const IRQn_Type s_uartIRQ[] = UART_IRQS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/* Array of UART clock name. */
static const clock_ip_name_t s_uartClock[] = UART_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/* UART ISR for transactional APIs. */
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
static uart_isr_t s_uartIsr = (uart_isr_t)DefaultISR;
#else
static uart_isr_t s_uartIsr;
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/
static bool UART_TransferIsRxRingBufferFull(UART_Type *base, uart_handle_t *handle)
{
    assert(NULL != handle);

    bool full;

    if (UART_TransferGetRxRingBufferLength(base, handle) == (handle->rxRingBufferSize - 1U))
    {
        full = true;
    }
    else
    {
        full = false;
    }
    return full;
}

static void UART_WriteNonBlocking(UART_Type *base, const uint8_t *data, size_t length)
{
    assert(NULL != data);

    size_t i;

    /* The Non Blocking write data API assume user have ensured there is enough space in
    peripheral to write. */
    for (i = 0U; i < length; i++)
    {
        base->THR = data[i];
    }
}

static void UART_ReadNonBlocking(UART_Type *base, uint8_t *data, size_t length)
{
    assert(NULL != data);

    size_t i;
    uint8_t dataMask = 0xFFU >> (base->LCR ^ 0x3U);

    /* The Non Blocking read data API assume user have ensured there is enough space in
    peripheral to write. */
    for (i = 0; i < length; i++)
    {
        data[i] = (uint8_t)(base->RBR & dataMask);
    }
}

/*!
 * brief Get the UART instance from peripheral base address.
 *
 * param base UART peripheral base address.
 * return UART instance.
 */
uint32_t UART_GetInstance(UART_Type *base)
{
    uint32_t instance;
    /* Find the instance index from base address mappings. */
    for (instance = 0U; instance < ARRAY_SIZE(s_uartBases); instance++)
    {
        if (s_uartBases[instance] == base)
        {
            break;
        }
    }
    assert(instance < ARRAY_SIZE(s_uartBases));
    return instance;
}

/*!
 * brief Initializes an UART instance with the user configuration structure and the peripheral clock frequency.
 *
 * This function configures the UART module with user-defined settings. Call the UART_GetDefaultConfig function
 * to get the configuration structure configured with default settings.
 * The example below shows how to use this API to configure the UART.
 * code
 *  uart_config_t uartConfig;
 *  UART_GetDefaultConfig(&uartConfig);
 *  UART_Init(UART1, &uartConfig, 20000000U);
 * endcode
 *
 * param base UART peripheral base address.
 * param config Pointer to a user-defined configuration structure.
 * param srcClock_Hz UART clock source frequency in Hz.
 * retval kStatus_UART_BaudrateNotSupport Baudrate is not support in current clock source.
 * retval kStatus_Success UART initialize succeed
 */
status_t UART_Init(UART_Type *base, const uart_config_t *config, uint32_t srcClock_Hz)
{
    assert(NULL != config);
    assert(0U != config->baudRate_Bps);

    /* Reset UART registers */
    base->DLL = 0x2U;
    base->DLH = 0x0U;
    base->IER = 0U;
    base->FCR = 0U;
    base->LCR = 0U;
    base->MCR = 0U;
    base->SCR = 0U;
    base->ISR = 0U;
    base->ABR = 0U;

    if (UART_SetBaudRate(base, config->baudRate_Bps, srcClock_Hz) == kStatus_UART_BaudrateNotSupport)
    {
        return kStatus_UART_BaudrateNotSupport;
    }
    else
    {
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
        /* Enable uart clock */
        CLOCK_EnableClock(s_uartClock[UART_GetInstance(base)]);
#endif
    }
    UART_SetParity(base, config->parityMode);
    UART_EnableStickyParity(base, config->enableStickyParity);
    UART_SetDataBits(base, config->dataBitCount);
    UART_SetStopBits(base, config->stopBitCount);
    UART_SetFifoConfig(base, (uart_fifo_config_t *)&config->fifoConfig);
    UART_SetAutoBaudConfig(base, (uart_autobaud_config_t *)&config->autoBaudConfig);
    UART_SetInfraredConfig(base, (uart_infrared_config_t *)&config->infraredConfig);
    UART_EnableHighSpeed(base, config->enableHighSpeed);
    UART_EnableLoop(base, config->enableLoop);
    UART_EnablCts(base, config->enableTxCTS);
    UART_EnableRts(base, config->enableRxRTS);
    UART_Enable(base, config->enable);

    return kStatus_Success;
}

/*!
 * brief Deinitializes a UART instance.
 *
 * This function waits for transmit to complete, disables TX and RX, and disables the UART clock.
 *
 * param base UART peripheral base address.
 */
void UART_Deinit(UART_Type *base)
{
    /* Wait all data shift out */
    while ((UART_GetStatusFlags(base) & (uint16_t)kUART_TxEmptyInterruptFlag) == 0U)
    {
    }
    /* Disable module. */
    UART_Enable(base, false);
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable uart clock */
    CLOCK_DisableClock(s_uartClock[UART_GetInstance(base)]);
#endif
}

/*!
 * brief Gets the default configuration structure.
 *
 * This function initializes the UART configuration structure to a default value.
 * The example below shows how to use this API to configure the UART.
 * code
 *  uart_config_t uartConfig;
 *  UART_GetDefaultConfig(&uartConfig);
 *  UART_Init(UART1, &uartConfig, 20000000U);
 * endcode
 *
 * param config Pointer to a configuration structure.
 */
void UART_GetDefaultConfig(uart_config_t *config)
{
    assert(NULL != config);
    /* Initializes the configure structure to zero. */
    (void)memset(config, 0, sizeof(*config));

    config->baudRate_Bps                   = 115200U;
    config->parityMode                     = kUART_ParityDisabled;
    config->enableStickyParity             = false;
    config->dataBitCount                   = kUART_Databits8;
    config->stopBitCount                   = kUART_Stopbits1;
    config->fifoConfig.txFifoWatermark     = kUART_TxfifoEmpty;
    config->fifoConfig.rxFifoWatermark     = kUART_RxfifoByte1;
    config->fifoConfig.resetTxFifo         = false;
    config->fifoConfig.resetRxFifo         = false;
    config->fifoConfig.fifoEnable          = true;
    config->autoBaudConfig.autoBaudEnable  = false;
    config->autoBaudConfig.baudProgramType = kUART_BaudrateProgramAuto;
    config->autoBaudConfig.baudCalType     = kUART_BaudrateCalFormula;
    config->infraredConfig.txIrEnable      = false;
    config->infraredConfig.rxIrEnable      = false;
    config->infraredConfig.txIrPolarity    = kUART_IrPositivePulse;
    config->infraredConfig.rxIrPolarity    = kUART_IrPositivePulse;
    config->infraredConfig.irPulseWidth    = kUART_IrPulsewidth3div16;
    config->enableHighSpeed                = true;
    config->enableLoop                     = false;
    config->enableTxCTS                    = false;
    config->enableRxRTS                    = false;
    config->enable                         = false;
}

/*!
 * brief Gets UART hardware status flags.
 *
 * param base UART peripheral base address.
 * return UART status flags, can be a single flag or several flags in #_uart_status_flags combined by OR.
 */
uint16_t UART_GetStatusFlags(UART_Type *base)
{
    uint16_t status = (uint16_t)base->LSR;
    status |= (uint16_t)(base->IIR & 0x18UL) << 8U;
    return status;
}

/*!
 * brief Enables UART interrupts according to the provided mask.
 *
 * This function enables the UART interrupts according to the provided mask. The mask is a logical OR of enumeration
 * members in #_uart_interrupt_enable.
 *
 * param base UART peripheral base address.
 * param u16Interrupts The interrupt source mask, can be a single source or several sources in #_uart_interrupt_enable
 * combined by OR.
 */
void UART_EnableInterrupts(UART_Type *base, uint8_t interrupts)
{
    base->MCR |= UART_MCR_OUT2_MASK;
    if ((interrupts & (uint8_t)kUART_AutoBaudCompleteInterruptEnable) != 0U)
    {
        base->ABR |= UART_ABR_ABLIE_MASK;
    }
    base->IER |= (interrupts & 0x1FUL);
}

/*!
 * brief Disables UART interrupts according to the provided mask.
 *
 * This function disables the UART interrupts according to the provided mask. The mask is a logical OR of enumeration
 * members in #_uart_interrupt_enable.
 *
 * param base UART peripheral base address.
 * param interrupts The interrupt source mask, can be a single source or several sources in #_uart_interrupt_enable
 * combined by OR.
 */
void UART_DisableInterrupts(UART_Type *base, uint8_t interrupts)
{
    if ((interrupts & (uint8_t)kUART_AutoBaudCompleteInterruptEnable) != 0U)
    {
        base->ABR &= ~UART_ABR_ABLIE_MASK;
    }
    base->IER &= ~(interrupts & 0x1FUL);
}

/*!
 * brief Gets the enabled UART interrupts.
 *
 * This function gets the enabled UART interrupts. The enabled interrupts are returned as the logical OR value of the
 * enumerators #_uart_interrupt_enable.
 *
 * param base UART peripheral base address.
 * return The interrupt source mask, can be a single source or several sources in #_uart_interrupt_enable combined
 * by OR.
 */
uint8_t UART_GetEnabledInterrupts(UART_Type *base)
{
    uint8_t interrupts = (uint8_t)base->IER & 0x1FU;
    if ((base->ABR & UART_ABR_ABLIE_MASK) != 0U)
    {
        interrupts |= (uint8_t)kUART_AutoBaudCompleteInterruptEnable;
    }
    return interrupts;
}

/*!
 * brief Sets the UART instance baudrate.
 *
 * This API configures the UART module baudrate. This API can be used to update
 * the UART module baudrate after the UART module is initialized by the UART_Init.
 * code
 *  UART_SetBaudRate(UART1, 115200U, 20000000U);
 * endcode
 *
 * param base UART peripheral base address.
 * param baudRate_Bps UART baudrate to be set.
 * param srcClock_Hz UART clock source frequency in HZ.
 * retval kStatus_UART_BaudrateNotSupport Baudrate is not supported in the current clock source.
 * retval kStatus_Success Set baudrate succeeded.
 */
status_t UART_SetBaudRate(UART_Type *base, uint32_t baudRate_Bps, uint32_t srcClock_Hz)
{
    assert((baudRate_Bps != 0U) && (srcClock_Hz != 0U));

    uint16_t div        = (uint16_t)(srcClock_Hz * 10U / baudRate_Bps / 16U + 5U) / 10U;
    uint32_t actualBaud = srcClock_Hz / (uint32_t)div / 16U;
    uint32_t diff       = actualBaud > baudRate_Bps ? actualBaud - baudRate_Bps : baudRate_Bps - actualBaud;
    if (diff > (baudRate_Bps / 100U * 3U))
    {
        return kStatus_UART_BaudrateNotSupport;
    }
    else
    {
        /* Configure the Baud Rate */
        base->LCR |= UART_LCR_DLAB_MASK;
        base->DLH = ((uint32_t)div >> 8U) & 0xFFUL;
        base->DLL = (uint32_t)div & 0xFFUL;
        base->LCR &= ~UART_LCR_DLAB_MASK;
        return kStatus_Success;
    }
}

/*!
 * brief Sets the UART auto baud configuration.
 *
 * This API configures the UART auto baud function, including enable/disable, calculation type, program type.
 *
 * param base UART peripheral base address.
 * param autoBaudConfig pointer to UART auto baud configuration.
 */
void UART_SetAutoBaudConfig(UART_Type *base, uart_autobaud_config_t *autoBaudConfig)
{
    assert(autoBaudConfig != NULL);

    base->ABR = (base->ABR & ~(UART_ABR_ABT_MASK | UART_ABR_ABUP_MASK | UART_ABR_ABE_MASK)) |
                UART_ABR_ABT(autoBaudConfig->baudCalType) | UART_ABR_ABUP(autoBaudConfig->baudProgramType) |
                UART_ABR_ABE(autoBaudConfig->autoBaudEnable);
}

/*!
 * brief Set DL register according to Auto Baud Detect Count value.
 *
 * param base UART peripheral base address.
 */
void UART_SetAutoBaud(UART_Type *base)
{
    uint16_t autoBaudCount = UART_GetAutoBaudCount(base);
    /* Configure the Baud Rate */
    base->LCR |= UART_LCR_DLAB_MASK;
    base->DLH = ((uint32_t)autoBaudCount >> 12U) & 0xFFUL;
    base->DLL = ((uint32_t)autoBaudCount >> 4U) & 0xFFUL;
    base->LCR &= ~UART_LCR_DLAB_MASK;
}

/*!
 * brief Sets the UART auto baud configuration.
 *
 * This API configures the UART auto baud function, including enable/disable, calculation type, program type.
 *
 * param base UART peripheral base address.
 * param infraredConfig pointer to UART infrared configuration.
 */
void UART_SetInfraredConfig(UART_Type *base, uart_infrared_config_t *infraredConfig)
{
    assert(infraredConfig != NULL);

    base->ISR = UART_ISR_RXPL(infraredConfig->rxIrPolarity) | UART_ISR_TXPL(infraredConfig->txIrPolarity) |
                UART_ISR_XMODE(infraredConfig->irPulseWidth) | UART_ISR_RCVEIR(infraredConfig->rxIrEnable) |
                UART_ISR_XMITIR(infraredConfig->txIrEnable);
}

/*!
 * brief Read 9 bit data from the UART peripheral.
 *
 * Before calling this API, make sure parity is enabled, and rx register full or data available in rx FIFO.
 *
 * param base UART peripheral base address.
 * return The received data
 */
uint16_t UART_Read9bits(UART_Type *base)
{
    uint16_t data;
    uint8_t i;
    uint8_t parity = 0U;

    /* Receive data */
    data = (uint16_t)UART_ReadByte(base);

    for (i = 0U; i < 8U; i++)
    {
        parity ^= (((uint8_t)data >> i) & 0x1U);
    }

    /* Check the parity error status bit */
    parity ^= (uint8_t)(base->LSR & UART_LSR_PE_MASK) >> UART_LSR_PE_SHIFT;
    /* Check current parity setting */
    if ((base->LCR & UART_LCR_EPS_MASK) != 0U)
    {
        parity ^= 0x1U;
    }
    /* get bit9 according parity error status */
    data |= (uint16_t)parity << 8U;

    return (data & 0x1FFU);
}

/*!
 * brief Writes TX register using a blocking method.
 *
 * This function polls the TX register, waits TX register to be empty or TX FIFO
 * have room then writes data to the TX buffer.
 *
 * param base UART peripheral base address.
 * param data Start address of the data to write.
 * param length Size of the data to write.
 */
status_t UART_WriteBlocking(UART_Type *base, const uint8_t *data, uint32_t length)
{
    assert(NULL != data);

    const uint8_t *dataAddress = data;

#if UART_RETRY_TIMES
    uint32_t waitTimes;
#endif

    while (0U != length)
    {
#if UART_RETRY_TIMES
        waitTimes = UART_RETRY_TIMES;
        while ((0U == (base->LSR & UART_LSR_TDRQ_MASK)) && (0U != --waitTimes))
#else
        while (0U == (base->LSR & UART_LSR_TDRQ_MASK))
#endif
        {
        }
#if UART_RETRY_TIMES
        if (0U == waitTimes)
        {
            return kStatus_UART_Timeout;
        }
#endif
        base->THR = *(dataAddress);
        dataAddress++;
        length--;
    }
    /* Ensure all the data in the transmit buffer are sent out to bus. */
#if UART_RETRY_TIMES
    waitTimes = UART_RETRY_TIMES;
    while ((0U == (base->LSR & UART_LSR_TEMT_MASK)) && (0U != --waitTimes))
#else
    while (0U == (base->LSR & UART_LSR_TEMT_MASK))
#endif
    {
    }
#if UART_RETRY_TIMES
    if (0U == waitTimes)
    {
        return kStatus_UART_Timeout;
    }
#endif
    return kStatus_Success;
}

/*!
 * brief Reads RX data register using a blocking method.
 *
 * This function polls the RX register, waits RX register to be full or RX FIFO
 * have data, then reads data from the RX register.
 *
 * param base UART peripheral base address.
 * param data Start address of the buffer to store the received data.
 * param length Size of the buffer.
 * retval kStatus_Fail Receiver error occurred while receiving data.
 * retval kStatus_UART_RxHardwareOverrun Receiver overrun occurred while receiving data
 * retval kStatus_UART_NoiseError Noise error occurred while receiving data
 * retval kStatus_UART_FramingErrorFraming error occurred while receiving data
 * retval kStatus_UART_ParityError Parity error occurred while receiving data
 * retval kStatus_Success Successfully received all data.
 */
status_t UART_ReadBlocking(UART_Type *base, uint8_t *data, uint32_t length)
{
    assert(NULL != data);

    status_t status = kStatus_Success;
    uint16_t statusFlag;
    uint8_t dataMask = 0xFFU >> (base->LCR ^ 0x3U);

#if UART_RETRY_TIMES
    uint32_t waitTimes;
#endif

    while (0U != (length--))
    {
        statusFlag = UART_GetStatusFlags(base);
#if UART_RETRY_TIMES
        waitTimes = UART_RETRY_TIMES;
        while (((statusFlag & (uint16_t)kUART_RxDataReadyInterruptFlag) == 0U) && (0U != --waitTimes))
        {
            if (0U == waitTimes)
            {
                status = kStatus_UART_Timeout;
                break;
            }
#else
        while ((statusFlag & (uint16_t)kUART_RxDataReadyInterruptFlag) == 0U)
        {
#endif
            if (0U != (statusFlag & (uint16_t)kUART_RxOverrunInterruptFlag))
            {
                status = kStatus_UART_RxHardwareOverrun;
                /* Other error flags(FE, NF, and PF) are prevented from setting once OR is set, no need to check other
                 * error flags*/
                break;
            }

            if (0U != (statusFlag & (uint16_t)kUART_ParityErrorInterruptFlag))
            {
                status = kStatus_UART_ParityError;
            }

            if (0U != (statusFlag & (uint16_t)kUART_FramingErrorInterruptFLag))
            {
                status = kStatus_UART_FramingError;
            }

            if (0U != (statusFlag & (uint16_t)kUART_FifoErrorInterruptFlag))
            {
                status = kStatus_UART_FifoError;
            }
            if (kStatus_Success != status)
            {
                break;
            }
            statusFlag = UART_GetStatusFlags(base);
        }

        if (kStatus_Success == status)
        {
            *(data++) = (uint8_t)(base->RBR & dataMask);
        }
        else
        {
            break;
        }
    }
    return status;
}

/*!
 * brief Initializes the UART handle.
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
 * param base UART peripheral base address.
 * param handle UART handle pointer.
 * param callback Callback function.
 * param userData User data.
 */
void UART_TransferCreateHandle(UART_Type *base,
                               uart_handle_t *handle,
                               uart_transfer_callback_t callback,
                               void *userData)
{
    assert(NULL != handle);

    uint32_t instance;

    /* Zero the handle. */
    (void)memset(handle, 0, sizeof(uart_handle_t));

    /* Set data mask */
    handle->dataMask = 0xFFU >> (base->LCR ^ 0x3U);

    /* Set the TX/RX state. */
    handle->rxState = (uint8_t)kUART_RxIdle;
    handle->txState = (uint8_t)kUART_TxIdle;

    /* Set the callback and user data. */
    handle->callback = callback;
    handle->userData = userData;

    /* Get instance from peripheral base address. */
    instance = UART_GetInstance(base);

    /* Save the handle in global variables to support the double weak mechanism. */
    s_uartHandle[instance] = handle;
    s_uartIsr              = UART_TransferHandleIRQ;

    /* Enable interrupt in NVIC. */
    (void)EnableIRQ(s_uartIRQ[instance]);
}

/*!
 * brief Sets up the RX ring buffer.
 *
 * This function sets up the RX ring buffer to a specific UART handle.
 *
 * When the RX ring buffer is used, data received is stored into the ring buffer even when
 * the user doesn't call the UART_TransferReceiveNonBlocking() API. If there is already data received
 * in the ring buffer, the user can get the received data from the ring buffer directly.
 *
 * note When using RX ring buffer, one byte is reserved for internal use. In other
 * words, if @p ringBufferSize is 32, then only 31 bytes are used for saving data.
 *
 * param base UART peripheral base address.
 * param handle UART handle pointer.
 * param ringBuffer Start address of ring buffer for background receiving. Pass NULL to disable the ring buffer.
 * param ringBufferSize size of the ring buffer.
 */
void UART_TransferStartRingBuffer(UART_Type *base, uart_handle_t *handle, uint8_t *ringBuffer, size_t ringBufferSize)
{
    assert(NULL != handle);
    assert(NULL != ringBuffer);

    /* Setup the ring buffer address */
    handle->rxRingBuffer     = ringBuffer;
    handle->rxRingBufferSize = ringBufferSize;
    handle->rxRingBufferHead = 0U;
    handle->rxRingBufferTail = 0U;

    /* Enable the interrupt to accept the data when user need the ring buffer. */
    UART_EnableInterrupts(base, (uint16_t)kUART_RxDataReadyInterruptEnable | (uint16_t)kUART_RxStatusInterruptEnable);
}

/*!
 * brief Aborts the background transfer and uninstalls the ring buffer.
 *
 * This function aborts the background transfer and uninstalls the ring buffer.
 *
 * param base UART peripheral base address.
 * param handle UART handle pointer.
 */
void UART_TransferStopRingBuffer(UART_Type *base, uart_handle_t *handle)
{
    assert(NULL != handle);

    if (handle->rxState == (uint8_t)kUART_RxIdle)
    {
        UART_DisableInterrupts(base,
                               (uint16_t)kUART_RxDataReadyInterruptEnable | (uint16_t)kUART_RxStatusInterruptEnable);
    }

    handle->rxRingBuffer     = NULL;
    handle->rxRingBufferSize = 0U;
    handle->rxRingBufferHead = 0U;
    handle->rxRingBufferTail = 0U;
}

/*!
 * brief Get the length of received data in RX ring buffer.
 *
 * param base UART peripheral base address.
 * param handle UART handle pointer.
 * return Length of received data in RX ring buffer.
 */
size_t UART_TransferGetRxRingBufferLength(UART_Type *base, uart_handle_t *handle)
{
    assert(NULL != handle);

    size_t size;
    size_t tmpRxRingBufferSize   = handle->rxRingBufferSize;
    uint16_t tmpRxRingBufferTail = handle->rxRingBufferTail;
    uint16_t tmpRxRingBufferHead = handle->rxRingBufferHead;

    if (tmpRxRingBufferTail > tmpRxRingBufferHead)
    {
        size = ((size_t)tmpRxRingBufferHead + tmpRxRingBufferSize - (size_t)tmpRxRingBufferTail);
    }
    else
    {
        size = ((size_t)tmpRxRingBufferHead - (size_t)tmpRxRingBufferTail);
    }

    return size;
}

/*!
 * brief Transmits a buffer of data using the interrupt method.
 *
 * This function send data using an interrupt method. This is a non-blocking function, which
 * returns directly without waiting for all data written to the transmitter register. When
 * all data is written to the TX register in the ISR, the UART driver calls the callback
 * function and passes the @ref kStatus_UART_TxIdle as status parameter.
 *
 * note The kStatus_UART_TxIdle is passed to the upper layer when all data are written
 * to the TX register. However, there is no check to ensure that all the data sent out. Before disabling the TX,
 * check the kUART_TransmissionCompleteFlag to ensure that the transmit is finished.
 *
 * param base UART peripheral base address.
 * param handle UART handle pointer.
 * param xfer UART transfer structure, see #uart_transfer_t.
 * @retval kStatus_Success Successfully start the data transmission.
 * @retval kStatus_UART_TxBusy Previous transmission still not finished, data not all written to the TX register.
 * @retval kStatus_InvalidArgument Invalid argument.
 */
status_t UART_TransferSendNonBlocking(UART_Type *base, uart_handle_t *handle, uart_transfer_t *xfer)
{
    assert(NULL != handle);
    assert(NULL != xfer);
    assert(NULL != xfer->data);
    assert(0U != xfer->dataSize);

    status_t status;

    /* Return error if current TX busy. */
    if ((uint8_t)kUART_TxBusy == handle->txState)
    {
        status = kStatus_UART_TxBusy;
    }
    else
    {
        handle->txData        = xfer->data;
        handle->txDataSize    = xfer->dataSize;
        handle->txDataSizeAll = xfer->dataSize;
        handle->txState       = (uint8_t)kUART_TxBusy;

        /* Enable transmitter interrupt. */
        UART_EnableInterrupts(base, (uint8_t)kUART_TxDataRequestInterruptEnable);

        status = kStatus_Success;
    }

    return status;
}

/*!
 * brief Gets the number of bytes that have been sent out to bus.
 *
 * This function gets the number of bytes that have been sent out to bus by an interrupt method.
 *
 * param base UART peripheral base address.
 * param handle UART handle pointer.
 * param count Send bytes count.
 * @retval kStatus_NoTransferInProgress No send in progress.
 * @retval kStatus_InvalidArgument Parameter is invalid.
 * @retval kStatus_Success Get successfully through the parameter \p count;
 */
status_t UART_TransferGetSendCount(UART_Type *base, uart_handle_t *handle, uint32_t *count)
{
    assert(NULL != handle);
    assert(NULL != count);

    status_t status      = kStatus_Success;
    size_t tmptxDataSize = handle->txDataSize;

    if ((uint8_t)kUART_TxIdle == handle->txState)
    {
        status = kStatus_NoTransferInProgress;
    }
    else
    {
        *count = handle->txDataSizeAll - tmptxDataSize;
    }
    return status;
}

/*!
 * brief Aborts the interrupt-driven data transmit.
 *
 * This function aborts the interrupt driven data sending. The user can get the remainBtyes to find out
 * how many bytes are not sent out.
 *
 * param base UART peripheral base address.
 * param handle UART handle pointer.
 */
void UART_TransferAbortSend(UART_Type *base, uart_handle_t *handle)
{
    assert(NULL != handle);

    UART_DisableInterrupts(base, (uint8_t)kUART_TxDataRequestInterruptEnable);

    handle->txDataSize = 0;
    handle->txState    = (uint8_t)kUART_TxIdle;
}

/*!
 * brief Receives a buffer of data using the interrupt method.
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
 * param base UART peripheral base address.
 * param handle UART handle pointer.
 * param xfer UART transfer structure, see uart_transfer_t.
 * param receivedBytes Bytes received from the ring buffer directly.
 * @retval kStatus_Success Successfully queue the transfer into the transmit queue.
 * @retval kStatus_UART_RxBusy Previous receive request is not finished.
 * @retval kStatus_InvalidArgument Invalid argument.
 */
status_t UART_TransferReceiveNonBlocking(UART_Type *base,
                                         uart_handle_t *handle,
                                         uart_transfer_t *xfer,
                                         size_t *receivedBytes)
{
    assert(NULL != handle);
    assert(NULL != xfer);
    assert(NULL != xfer->data);
    assert(0U != xfer->dataSize);

    uint32_t i;
    status_t status;
    /* How many bytes to copy from ring buffer to user memory. */
    size_t bytesToCopy = 0U;
    /* How many bytes to receive. */
    size_t bytesToReceive;
    /* How many bytes currently have received. */
    size_t bytesCurrentReceived;

    /* How to get data:
       1. If RX ring buffer is not enabled, then save xfer->data and xfer->dataSize
          to uart handle, enable interrupt to store received data to xfer->data. When
          all data received, trigger callback.
       2. If RX ring buffer is enabled and not empty, get data from ring buffer first.
          If there are enough data in ring buffer, copy them to xfer->data and return.
          If there are not enough data in ring buffer, copy all of them to xfer->data,
          save the xfer->data remained empty space to uart handle, receive data
          to this empty space and trigger callback when finished. */

    if ((uint8_t)kUART_RxBusy == handle->rxState)
    {
        status = kStatus_UART_RxBusy;
    }
    else
    {
        bytesToReceive       = xfer->dataSize;
        bytesCurrentReceived = 0;

        /* If RX ring buffer is used. */
        if (NULL != handle->rxRingBuffer)
        {
            /* Disable UART RX IRQ, protect ring buffer. */
            UART_DisableInterrupts(base, (uint16_t)kUART_RxDataReadyInterruptEnable);

            /* How many bytes in RX ring buffer currently. */
            bytesToCopy = UART_TransferGetRxRingBufferLength(base, handle);

            if (0U != bytesToCopy)
            {
                bytesToCopy = MIN(bytesToReceive, bytesToCopy);

                bytesToReceive -= bytesToCopy;

                /* Copy data from ring buffer to user memory. */
                for (i = 0U; i < bytesToCopy; i++)
                {
                    xfer->data[bytesCurrentReceived] = handle->rxRingBuffer[handle->rxRingBufferTail];
                    bytesCurrentReceived++;

                    /* Wrap to 0. Not use modulo (%) because it might be large and slow. */
                    if (((uint32_t)handle->rxRingBufferTail + 1U) == handle->rxRingBufferSize)
                    {
                        handle->rxRingBufferTail = 0U;
                    }
                    else
                    {
                        handle->rxRingBufferTail++;
                    }
                }
            }

            /* If ring buffer does not have enough data, still need to read more data. */
            if (0U != bytesToReceive)
            {
                /* No data in ring buffer, save the request to UART handle. */
                handle->rxData        = &xfer->data[bytesCurrentReceived];
                handle->rxDataSize    = bytesToReceive;
                handle->rxDataSizeAll = bytesToReceive;
                handle->rxState       = (uint8_t)kUART_RxBusy;
            }
            /* Enable UART RX IRQ if previously enabled. */
            UART_EnableInterrupts(base, (uint16_t)kUART_RxDataReadyInterruptEnable);

            /* Call user callback since all data are received. */
            if (0U == bytesToReceive)
            {
                if (NULL != handle->callback)
                {
                    handle->callback(base, handle, kStatus_UART_RxIdle, handle->userData);
                }
            }
        }
        /* Ring buffer not used. */
        else
        {
            handle->rxData        = xfer->data;
            handle->rxDataSize    = bytesToReceive;
            handle->rxDataSizeAll = bytesToReceive;
            handle->rxState       = (uint8_t)kUART_RxBusy;

            /* Enable RX interrupt. */
            UART_EnableInterrupts(base,
                                  (uint16_t)kUART_RxDataReadyInterruptEnable | (uint16_t)kUART_RxStatusInterruptEnable);
        }

        /* Return the how many bytes have read. */
        if (NULL != receivedBytes)
        {
            *receivedBytes = bytesCurrentReceived;
        }

        status = kStatus_Success;
    }

    return status;
}

/*!
 * brief Gets the number of bytes that have been received.
 *
 * This function gets the number of bytes that have been received.
 *
 * param base UART peripheral base address.
 * param handle UART handle pointer.
 * param count Receive bytes count.
 * @retval kStatus_NoTransferInProgress No receive in progress.
 * @retval kStatus_InvalidArgument Parameter is invalid.
 * @retval kStatus_Success Get successfully through the parameter \p count;
 */
status_t UART_TransferGetReceiveCount(UART_Type *base, uart_handle_t *handle, uint32_t *count)
{
    assert(NULL != handle);
    assert(NULL != count);

    status_t status      = kStatus_Success;
    size_t tmprxDataSize = handle->rxDataSize;

    if ((uint8_t)kUART_RxIdle == handle->rxState)
    {
        status = kStatus_NoTransferInProgress;
    }
    else
    {
        *count = handle->rxDataSizeAll - tmprxDataSize;
    }

    return status;
}

/*!
 * brief Aborts the interrupt-driven data receiving.
 *
 * This function aborts the interrupt-driven data receiving. The user can get the remainBytes to find out
 * how many bytes not received yet.
 *
 * param base UART peripheral base address.
 * param handle UART handle pointer.
 */
void UART_TransferAbortReceive(UART_Type *base, uart_handle_t *handle)
{
    assert(NULL != handle);

    /* Only abort the receive to handle->rxData, the RX ring buffer is still working. */
    if (NULL == handle->rxRingBuffer)
    {
        /* Disable RX interrupt. */
        UART_DisableInterrupts(base,
                               (uint16_t)kUART_RxDataReadyInterruptEnable | (uint16_t)kUART_RxStatusInterruptEnable);
    }

    handle->rxDataSize = 0U;
    handle->rxState    = (uint8_t)kUART_RxIdle;
}

/*!
 * brief UART IRQ handle function.
 *
 * This function handles the UART transmit and receive IRQ request.
 *
 * param base UART peripheral base address.
 * param handle UART handle pointer.
 */
void UART_TransferHandleIRQ(UART_Type *base, uart_handle_t *handle)
{
    assert(NULL != handle);

    uint8_t count;
    uint8_t tempCount;
    uint16_t status           = UART_GetStatusFlags(base);
    uint8_t enabledInterrupts = UART_GetEnabledInterrupts(base);
    uint16_t tpmRxRingBufferHead;
    uint32_t tpmData;

    /* If RX framing error */
    if (((uint16_t)kUART_FramingErrorInterruptFLag & status) != 0U)
    {
        handle->rxState    = (uint8_t)kUART_RxFramingError;
        handle->rxDataSize = 0U;
        /* Trigger callback. */
        if (handle->callback != NULL)
        {
            handle->callback(base, handle, kStatus_UART_FramingError, handle->userData);
        }
    }

    /* If RX parity error */
    if (((uint16_t)kUART_ParityErrorInterruptFlag & status) != 0U)
    {
        handle->rxState    = (uint8_t)kUART_RxParityError;
        handle->rxDataSize = 0U;
        /* Trigger callback. */
        if (handle->callback != NULL)
        {
            handle->callback(base, handle, kStatus_UART_ParityError, handle->userData);
        }
    }

    /* If RX overrun. */
    if (((uint16_t)kUART_RxOverrunInterruptFlag & status) != 0U)
    {
        /* Trigger callback. */
        if (handle->callback != NULL)
        {
            handle->callback(base, handle, kStatus_UART_RxHardwareOverrun, handle->userData);
        }
    }

    /* If RX FIFO error. */
    if (((uint16_t)kUART_FifoErrorInterruptFlag & status) != 0U)
    {
        /* Trigger callback. */
        if (handle->callback != NULL)
        {
            handle->callback(base, handle, kStatus_UART_RxHardwareOverrun, handle->userData);
        }
    }

    /* Receive data register full */
    if ((0U != ((uint16_t)kUART_RxDataReadyInterruptFlag & status)) &&
        (0U != ((uint8_t)kUART_RxDataReadyInterruptEnable & enabledInterrupts)))
    {
        /* Get the size that can be stored into buffer for this interrupt. */
        if (UART_IsFifoEnabled(base))
        {
            count = UART_GetRxFifoLevel(base);
        }
        else
        {
            count = 1;
        }

        /* If handle->rxDataSize is not 0, save data to handle->rxData first. */
        while ((0U != handle->rxDataSize) && (0U != count))
        {
            tempCount = (uint8_t)MIN(handle->rxDataSize, count);

            /* Using non block API to read the data from the registers. */
            UART_ReadNonBlocking(base, handle->rxData, tempCount);
            handle->rxData = &handle->rxData[tempCount];
            handle->rxDataSize -= tempCount;
            count -= tempCount;

            /* If all the data required for upper layer is ready, trigger callback. */
            if (0U == handle->rxDataSize)
            {
                handle->rxState = (uint8_t)kUART_RxIdle;

                if (NULL != handle->callback)
                {
                    handle->callback(base, handle, kStatus_UART_RxIdle, handle->userData);
                }
            }
        }

        /* If use RX ring buffer, receive data to ring buffer. */
        if (NULL != handle->rxRingBuffer)
        {
            while (0U != count--)
            {
                /* If RX ring buffer is full, trigger callback to notify ring buffer overrun. */
                if (UART_TransferIsRxRingBufferFull(base, handle))
                {
                    if (NULL != handle->callback)
                    {
                        handle->callback(base, handle, kStatus_UART_RxRingBufferOverrun, handle->userData);
                    }
                }

                /* If ring buffer is still full after callback function, the oldest data is over write. */
                if (UART_TransferIsRxRingBufferFull(base, handle))
                {
                    /* Increase handle->rxRingBufferTail to make room for new data. */
                    if (((uint32_t)handle->rxRingBufferTail + 1U) == handle->rxRingBufferSize)
                    {
                        handle->rxRingBufferTail = 0U;
                    }
                    else
                    {
                        handle->rxRingBufferTail++;
                    }
                }

                /* Read data. */
                tpmRxRingBufferHead                       = handle->rxRingBufferHead;
                tpmData                                   = UART_ReadByte(base);
                handle->rxRingBuffer[tpmRxRingBufferHead] = (uint8_t)(tpmData & handle->dataMask);

                /* Increase handle->rxRingBufferHead. */
                if (((uint32_t)handle->rxRingBufferHead + 1U) == handle->rxRingBufferSize)
                {
                    handle->rxRingBufferHead = 0U;
                }
                else
                {
                    handle->rxRingBufferHead++;
                }
            }
        }
        /* If no receive requst pending, stop RX interrupt. */
        else if (0U == handle->rxDataSize)
        {
            UART_DisableInterrupts(
                base, (uint16_t)kUART_RxDataReadyInterruptEnable | (uint16_t)kUART_RxStatusInterruptEnable);
        }
        else
        {
            /*  Avoid MISRA 15.7 */
        }
    }

    /* Send data register request and the interrupt is enabled. */
    if ((0U != ((uint16_t)kUART_TxDataRequestInterruptFlag & status)) &&
        (0U != ((uint8_t)kUART_TxDataRequestInterruptEnable & enabledInterrupts)))
    {
        /* Get the bytes that available at this moment. */
        if (UART_IsFifoEnabled(base))
        {
            /* Since we do not have access to the tx data count on FIFO, and FCR is write only so
               we do not have the access to the tx FIFO water mark setting either, so safely assume
               half of the FIFO is available */
            count = 32U;
        }
        else
        {
            count = 1U;
        }

        while ((0U != handle->txDataSize) && (0U != count))
        {
            tempCount = (uint8_t)MIN(handle->txDataSize, count);

            /* Using non block API to write the data to the registers. */
            UART_WriteNonBlocking(base, handle->txData, tempCount);
            handle->txData = &handle->txData[tempCount];
            handle->txDataSize -= tempCount;
            count -= tempCount;
        }
    }

    /* Send data register empty and the interrupt is enabled. */
    if ((0U != ((uint16_t)kUART_TxEmptyInterruptFlag & status)) &&
        (0U != ((uint8_t)kUART_TxDataRequestInterruptEnable & enabledInterrupts)))
    {
        /* If all the data are written to data register, notify user with the callback, then TX finished. */
        if (0U == handle->txDataSize)
        {
            /* Set txState to idle only when all data has been sent out to bus. */
            handle->txState = (uint8_t)kUART_TxIdle;
            /* Disable transmission complete interrupt. */
            UART_DisableInterrupts(base, (uint8_t)kUART_TxDataRequestInterruptEnable);

            /* Trigger callback. */
            if (NULL != handle->callback)
            {
                handle->callback(base, handle, kStatus_UART_TxIdle, handle->userData);
            }
        }
    }
}

/*!
 * brief UART Error IRQ handle function.
 *
 * This function handles the UART error IRQ request.
 *
 * param base UART peripheral base address.
 * param handle UART handle pointer.
 */
void UART_TransferHandleErrorIRQ(UART_Type *base, uart_handle_t *handle)
{
    /* To be implemented by User. */
}

#if defined(UART0)
void UART0_DriverIRQHandler(void);
void UART0_DriverIRQHandler(void)
{
    s_uartIsr(UART0, s_uartHandle[0]);
    SDK_ISR_EXIT_BARRIER;
}
#endif

#if defined(UART1)
void UART1_DriverIRQHandler(void);
void UART1_DriverIRQHandler(void)
{
    s_uartIsr(UART1, s_uartHandle[1]);
    SDK_ISR_EXIT_BARRIER;
}
#endif

#if defined(UART2)
void UART2_DriverIRQHandler(void);
void UART2_DriverIRQHandler(void)
{
    s_uartIsr(UART2, s_uartHandle[2]);
    SDK_ISR_EXIT_BARRIER;
}
#endif
