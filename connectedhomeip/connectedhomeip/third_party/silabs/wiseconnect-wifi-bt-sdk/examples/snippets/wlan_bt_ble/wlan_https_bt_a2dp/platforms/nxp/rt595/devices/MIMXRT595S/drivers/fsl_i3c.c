/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_i3c.h"
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.i3c"
#endif

#define FSL_I3C_ERROR_RATE_MAX (10)
#define FSL_I3C_PPBAUD_HZ (12500000UL)
#define FSL_I3C_PPBAUD_MAX_HZ (12800000UL)
#define FSL_I3C_ODBAUD_HZ (2500000UL)
#define FSL_I3C_ODBAUD_MAX_HZ (2777777UL) /* 360ns cycle */
#define FSL_I3C_PPBAUD_DIV_MAX ((I3C_MCONFIG_PPBAUD_MASK >> I3C_MCONFIG_PPBAUD_SHIFT) + 1)
#define FSL_I3C_ODBAUD_DIV_MAX ((I3C_MCONFIG_ODBAUD_MASK >> I3C_MCONFIG_ODBAUD_SHIFT) + 1)
#define FSL_I3C_I2CBAUD_DIV_MAX (((I3C_MCONFIG_I2CBAUD_MASK >> I3C_MCONFIG_I2CBAUD_SHIFT) + 1) / 2)

/*! @brief Common sets of flags used by the driver. */
enum _i3c_flag_constants
{
    /*! All flags which are cleared by the driver upon starting a transfer. */
    kMasterClearFlags = kI3C_MasterSlaveStartFlag | kI3C_MasterControlDoneFlag | kI3C_MasterCompleteFlag |
                        kI3C_MasterArbitrationWonFlag | kI3C_MasterSlave2MasterFlag,

    /*! IRQ sources enabled by the non-blocking transactional API. */
    kMasterIrqFlags = kI3C_MasterSlaveStartFlag | kI3C_MasterControlDoneFlag | kI3C_MasterCompleteFlag |
                      kI3C_MasterRxReadyFlag | kI3C_MasterTxReadyFlag | kI3C_MasterArbitrationWonFlag |
                      kI3C_MasterErrorFlag | kI3C_MasterSlave2MasterFlag,

    /*! Errors to check for. */
    kMasterErrorFlags = kI3C_MasterErrorNackFlag | kI3C_MasterErrorWriteAbortFlag | kI3C_MasterErrorTermFlag |
                        kI3C_MasterErrorParityFlag | kI3C_MasterErrorCrcFlag | kI3C_MasterErrorReadFlag |
                        kI3C_MasterErrorWriteFlag | kI3C_MasterErrorMsgFlag | kI3C_MasterErrorInvalidReqFlag |
                        kI3C_MasterErrorTimeoutFlag,
};

/*! @brief States for the state machine used by transactional APIs. */
enum _i3c_transfer_states
{
    kIdleState = 0,
    kSendCommandState,
    kTransferDataState,
    kStopState,
    kWaitForCompletionState,
};

/*! @brief Typedef for master interrupt handler. */
typedef void (*i3c_master_isr_t)(I3C_Type *base, i3c_master_handle_t *handle);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* Not static so it can be used from fsl_i3c_dma.c. */
uint32_t I3C_GetInstance(I3C_Type *base);

static status_t I3C_MasterWaitForTxReady(I3C_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Array to map I3C instance number to base pointer. */
static I3C_Type *const kI3cBases[] = I3C_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Array to map I3C instance number to clock gate enum. */
static clock_ip_name_t const kI3cClocks[] = I3C_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

#if !(defined(FSL_FEATURE_I3C_HAS_NO_RESET) && FSL_FEATURE_I3C_HAS_NO_RESET)
/*! @brief Pointers to I3C resets for each instance. */
static const reset_ip_name_t kI3cResets[] = I3C_RSTS;
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Returns an instance number given a base address.
 *
 * If an invalid base address is passed, debug builds will assert. Release builds will just return
 * instance number 0.
 *
 * @param base The I3C peripheral base address.
 * @return I3C instance number starting from 0.
 */
uint32_t I3C_GetInstance(I3C_Type *base)
{
    uint32_t instance;
    for (instance = 0; instance < ARRAY_SIZE(kI3cBases); ++instance)
    {
        if (kI3cBases[instance] == base)
        {
            return instance;
        }
    }

    assert(false);
    return 0;
}

/*!
 * @brief Convert provided flags to status code, and clear any errors if present.
 * @param base The I3C peripheral base address.
 * @param status Current status flags value that will be checked.
 * @retval #kStatus_Success
 * @retval #kStatus_I3C_Nak
 * @retval #kStatus_I3C_WriteAbort
 * @retval #kStatus_I3C_Term
 * @retval #kStatus_I3C_ParityError
 * @retval #kStatus_I3C_CrcError
 * @retval #kStatus_I3C_ReadFifoError
 * @retval #kStatus_I3C_WriteFifoError
 * @retval #kStatus_I3C_MsgError
 * @retval #kStatus_I3C_InvalidReq
 * @retval #kStatus_I3C_Timeout
 */
status_t I3C_MasterCheckAndClearError(I3C_Type *base, uint32_t status)
{
    status_t result = kStatus_Success;

    /* Check for error. These errors cause a stop to automatically be sent. We must */
    /* clear the errors before a new transfer can start. */
    status &= kMasterErrorFlags;
    if (status)
    {
        /* Select the correct error code. Ordered by severity, with bus issues first. */
        if (status & kI3C_MasterErrorTimeoutFlag)
        {
            result = kStatus_I3C_Timeout;
        }
        else if (status & kI3C_MasterErrorNackFlag)
        {
            result = kStatus_I3C_Nak;
        }
        else if (status & kI3C_MasterErrorWriteAbortFlag)
        {
            result = kStatus_I3C_WriteAbort;
        }
        else if (status & kI3C_MasterErrorTermFlag)
        {
            result = kStatus_I3C_Term;
        }
        else if (status & kI3C_MasterErrorParityFlag)
        {
            result = kStatus_I3C_ParityError;
        }
        else if (status & kI3C_MasterErrorCrcFlag)
        {
            result = kStatus_I3C_CrcError;
        }
        else if (status & kI3C_MasterErrorMsgFlag)
        {
            result = kStatus_I3C_MsgError;
        }
        else if (status & kI3C_MasterErrorReadFlag)
        {
            result = kStatus_I3C_ReadFifoError;
        }
        else if (status & kI3C_MasterErrorWriteFlag)
        {
            result = kStatus_I3C_WriteFifoError;
        }
        else if (status & kI3C_MasterErrorInvalidReqFlag)
        {
            result = kStatus_I3C_InvalidReq;
        }
        else
        {
            assert(false);
        }

        /* Clear the flags. */
        I3C_MasterClearErrorStatusFlags(base, status);

        /* Reset fifos. These flags clear automatically. */
        base->MDATACTRL |= I3C_MDATACTRL_FLUSHTB_MASK | I3C_MDATACTRL_FLUSHTB_MASK;
    }

    return result;
}

static status_t I3C_MasterWaitForCtrlDone(I3C_Type *base, bool waitIdle)
{
    status_t result = kStatus_Success;
    uint32_t status, errStatus;
#if I3C_WAIT_TIMEOUT
    uint32_t waitTimes = I3C_WAIT_TIMEOUT;
#endif

#if I3C_WAIT_TIMEOUT
    while ((result == kStatus_Success) && (--waitTimes))
#else
    while (result == kStatus_Success)
#endif
    {
        status    = I3C_MasterGetStatusFlags(base);
        errStatus = I3C_MasterGetErrorStatusFlags(base);
        /* Check for error flags. */
        result = I3C_MasterCheckAndClearError(base, errStatus);
        /* Check if the control finishes. */
        if (status & kI3C_MasterControlDoneFlag)
        {
            I3C_MasterClearStatusFlags(base, kI3C_MasterControlDoneFlag);
            if (!waitIdle)
            {
                break;
            }
        }
        /* kI3C_MasterControlDoneFlag only indicates ACK got, need to wait for SDA high. */
        if (waitIdle && I3C_MasterGetState(base) == kI3C_MasterStateIdle)
        {
            break;
        }
    }

#if I3C_WAIT_TIMEOUT
    if (waitTimes == 0)
    {
        return kStatus_I3C_Timeout;
    }
#endif

    return result;
}

static status_t I3C_MasterWaitForTxReady(I3C_Type *base)
{
    uint32_t errStatus;
    status_t result;
    size_t txCount;
    size_t txFifoSize = 2U << ((base->SCAPABILITIES & I3C_SCAPABILITIES_FIFOTX_MASK) >> I3C_SCAPABILITIES_FIFOTX_SHIFT);

#if I3C_WAIT_TIMEOUT
    uint32_t waitTimes = I3C_WAIT_TIMEOUT;
#endif
    do
    {
        /* Get the number of words in the tx fifo and compute empty slots. */
        I3C_MasterGetFifoCounts(base, NULL, &txCount);
        txCount = txFifoSize - txCount;

        /* Check for error flags. */
        errStatus = I3C_MasterGetErrorStatusFlags(base);
        result    = I3C_MasterCheckAndClearError(base, errStatus);
        if (result)
        {
            return result;
        }
#if I3C_WAIT_TIMEOUT
    } while ((!txCount) && (--waitTimes));

    if (waitTimes == 0)
    {
        return kStatus_I3C_Timeout;
    }
#else
    } while (!txCount);
#endif

    return kStatus_Success;
}

static status_t I3C_MasterWaitForComplete(I3C_Type *base, bool waitIdle)
{
    uint32_t status, errStatus;
    status_t result = kStatus_Success;
#if I3C_WAIT_TIMEOUT
    uint32_t waitTimes = I3C_WAIT_TIMEOUT;
#endif
    do
    {
        status    = I3C_MasterGetStatusFlags(base);
        errStatus = I3C_MasterGetErrorStatusFlags(base);
        result    = I3C_MasterCheckAndClearError(base, errStatus);
#if I3C_WAIT_TIMEOUT
    } while (!(status & kI3C_MasterCompleteFlag) && result == kStatus_Success && --waitTimes);
#else
    } while (!(status & kI3C_MasterCompleteFlag) && result == kStatus_Success);
#endif

    I3C_MasterClearStatusFlags(base, kI3C_MasterCompleteFlag);

#if I3C_WAIT_TIMEOUT
    if (waitTimes == 0)
    {
        return kStatus_I3C_Timeout;
    }
#endif

    if (waitIdle)
    {
#if I3C_WAIT_TIMEOUT
        while (I3C_MasterGetState(base) != kI3C_MasterStateIdle && --waitTimes)
#else
        while (I3C_MasterGetState(base) != kI3C_MasterStateIdle)
#endif
        {
        }
    }

    return result;
}

status_t I3C_MasterEmitStop(I3C_Type *base, bool waitIdle)
{
    status_t result = kStatus_Success;

    /* Return an error if the bus is not in transaction. */
    if (I3C_MasterGetState(base) != kI3C_MasterStateNormAct)
    {
        return kStatus_I3C_InvalidReq;
    }

    /* Send the STOP signal */
    base->MCTRL = (base->MCTRL & ~(I3C_MCTRL_REQUEST_MASK | I3C_MCTRL_DIR_MASK | I3C_MCTRL_RDTERM_MASK)) |
                  I3C_MCTRL_REQUEST(kI3C_RequestEmitStop);

    /* Wait for the stop operation finishes. */
    /* Also check for errors while waiting. */
    result = I3C_MasterWaitForCtrlDone(base, waitIdle);

    return result;
}

/*!
 * @brief Make sure the bus isn't already busy.
 *
 * A busy bus is allowed if we are the one driving it.
 *
 * @param base The I3C peripheral base address.
 * @retval #kStatus_Success
 * @retval #kStatus_I3C_Busy
 */
status_t I3C_CheckForBusyBus(I3C_Type *base)
{
    return I3C_MasterGetBusIdleState(base) ? kStatus_Success : kStatus_I3C_Busy;
}

void I3C_MasterGetDefaultConfig(i3c_master_config_t *masterConfig)
{
    masterConfig->enableMaster        = kI3C_MasterOn;
    masterConfig->disableTimeout      = false;
    masterConfig->hKeep               = kI3C_MasterHighKeeperWiredIn;
    masterConfig->enableOpenDrainStop = true;
    masterConfig->enableOpenDrainHigh = true;
    masterConfig->baudRate_Hz         = 400000U;
    masterConfig->busType             = kI3C_TypeI2C;
}

void I3C_MasterInit(I3C_Type *base, const i3c_master_config_t *masterConfig, uint32_t sourceClock_Hz)
{
    uint32_t instance = I3C_GetInstance(base);
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate the clock. */
    CLOCK_EnableClock(kI3cClocks[instance]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

#if !(defined(FSL_FEATURE_I3C_HAS_NO_RESET) && FSL_FEATURE_I3C_HAS_NO_RESET)
    /* Reset the I3C module */
    RESET_PeripheralReset(kI3cResets[instance]);
#endif

    base->MCONFIG = I3C_MCONFIG_MSTENA(masterConfig->enableMaster) | I3C_MCONFIG_DISTO(masterConfig->disableTimeout) |
                    I3C_MCONFIG_HKEEP(masterConfig->hKeep) | I3C_MCONFIG_ODSTOP(masterConfig->enableOpenDrainStop) |
                    I3C_MCONFIG_ODHPP(masterConfig->enableOpenDrainHigh);

    I3C_MasterSetWatermarks(base, kI3C_TxTriggerUntilOneLessThanFull, kI3C_RxTriggerOnNotEmpty, true, true);

    I3C_MasterSetBaudRate(base, masterConfig->baudRate_Hz, sourceClock_Hz);
}

void I3C_MasterDeinit(I3C_Type *base)
{
    uint32_t idx = I3C_GetInstance(base);

#if !(defined(FSL_FEATURE_I3C_HAS_NO_RESET) && FSL_FEATURE_I3C_HAS_NO_RESET)
    /* Reset the I3C module */
    RESET_PeripheralReset(kI3cResets[idx]);
#endif

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Gate clock. */
    CLOCK_DisableClock(kI3cClocks[idx]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

static uint32_t I3C_CalcErrorRatio(uint32_t curFreq, uint32_t desiredFreq)
{
    if (curFreq > desiredFreq)
    {
        return (curFreq - desiredFreq) * 100 / desiredFreq;
    }
    else
    {
        return (desiredFreq - curFreq) * 100 / desiredFreq;
    }
}

void I3C_MasterSetBaudRate(I3C_Type *base, uint32_t baudRate_Hz, uint32_t sourceClock_Hz)
{
    uint32_t div, freq;
    uint32_t ppBaud, odBaud, i2cBaud;
    i3c_master_enable_t mstEna;
    uint32_t errRate0, errRate1;

    /* Disable master mode. */
    mstEna = (i3c_master_enable_t)((base->MCONFIG & I3C_MCONFIG_MSTENA_MASK) >> I3C_MCONFIG_MSTENA_SHIFT);
    I3C_MasterEnable(base, kI3C_MasterOff);

    /* Find out the div to generate target freq */
    freq = sourceClock_Hz / 2;
    /* ppFreq = FCLK / 2 / (PPBAUD + 1)), 0 <= PPBAUD <= 15 */
    /* We need PPBAUD generate 12.5MHz or so. */
    div = freq / FSL_I3C_PPBAUD_HZ;
    div = div == 0 ? 1 : div;
    if (freq / div > FSL_I3C_PPBAUD_MAX_HZ)
    {
        div++;
    }
    assert(div <= FSL_I3C_PPBAUD_DIV_MAX);
    ppBaud = div - 1;
    freq /= div;

    /* odFreq = ppFreq / (ODBAUD + 1), 1 <= ODBAUD <= 255 */
    /* We need ODBAUD generate 2.5MHz or so. */
    div = freq / FSL_I3C_ODBAUD_HZ;
    div = div < 2 ? 2 : div;
    if (freq / div > FSL_I3C_ODBAUD_MAX_HZ)
    {
        div++;
    }
    odBaud = div - 1;
    freq /= div;

    /* i2cFreq = odFreq / (I2CBAUD + 1), 0 <= I2CBAUD <= 7 (I2CBAUD need << 1 in register) */
    div      = freq / baudRate_Hz;
    div      = div == 0 ? 1 : div;
    errRate0 = I3C_CalcErrorRatio(freq / div, baudRate_Hz);
    errRate1 = I3C_CalcErrorRatio(freq * 2 / (div * 2 + 1), baudRate_Hz);
    if (errRate0 < FSL_I3C_ERROR_RATE_MAX || errRate1 < FSL_I3C_ERROR_RATE_MAX)
    {
        /* Use this div */
        i2cBaud = errRate0 < errRate1 ? (div - 1) * 2 : (div - 1) * 2 + 1;
    }
    else
    {
        /* Use div + 1, unless current freq is already lower than desired. */
        i2cBaud = freq / div < baudRate_Hz ? (div - 1) * 2 : div * 2;
    }

    base->MCONFIG = (base->MCONFIG & ~(I3C_MCONFIG_PPBAUD_MASK | I3C_MCONFIG_PPLOW_MASK | I3C_MCONFIG_ODBAUD_MASK |
                                       I3C_MCONFIG_I2CBAUD_MASK)) |
                    I3C_MCONFIG_PPBAUD(ppBaud) | I3C_MCONFIG_ODBAUD(odBaud) | I3C_MCONFIG_I2CBAUD(i2cBaud);

    /* Restore master mode. */
    if (mstEna != kI3C_MasterOff)
    {
        I3C_MasterEnable(base, mstEna);
    }
}

status_t I3C_MasterStart(I3C_Type *base, i3c_bus_type_t type, uint8_t address, i3c_direction_t dir, uint32_t rxSize)
{
    /* Return an error if the bus is already in use not by us. */
    status_t result = I3C_CheckForBusyBus(base);

    if (result)
    {
        return result;
    }

    return I3C_MasterRepeatedStart(base, type, address, dir, rxSize);
}

status_t I3C_MasterRepeatedStart(
    I3C_Type *base, i3c_bus_type_t type, uint8_t address, i3c_direction_t dir, uint32_t rxSize)
{
    assert(rxSize <= (I3C_MCTRL_RDTERM_MASK >> I3C_MCTRL_RDTERM_SHIFT));

    /* Clear all flags. */
    I3C_MasterClearStatusFlags(base, kMasterClearFlags);

    /* Issue start command. */
    base->MCTRL = I3C_MCTRL_TYPE(type) | I3C_MCTRL_REQUEST(kI3C_RequestEmitStartAddr) | I3C_MCTRL_DIR(dir) |
                  I3C_MCTRL_ADDR(address) | I3C_MCTRL_RDTERM(rxSize);

    return kStatus_Success;
}

status_t I3C_MasterStop(I3C_Type *base)
{
    return I3C_MasterEmitStop(base, true);
}

status_t I3C_MasterReceive(I3C_Type *base, void *rxBuff, size_t rxSize, uint32_t flags)
{
    status_t result = kStatus_Success;
    uint32_t status;
    bool completed = false;
    uint8_t *buf;

    assert(rxBuff);

    /* Handle empty read. */
    if (!rxSize)
    {
        return kStatus_Success;
    }

#if I3C_WAIT_TIMEOUT
    uint32_t waitTimes = I3C_WAIT_TIMEOUT;
#endif

    /* Receive data */
    buf = (uint8_t *)rxBuff;
    while (rxSize || !completed)
    {
#if I3C_WAIT_TIMEOUT
        if (--waitTimes == 0)
        {
            return kStatus_I3C_Timeout;
        }
#endif
        /* Check for errors. */
        result = I3C_MasterCheckAndClearError(base, I3C_MasterGetErrorStatusFlags(base));
        if (result)
        {
            return result;
        }

        /* Check complete flag */
        if (!completed)
        {
            status = I3C_MasterGetStatusFlags(base) & kI3C_MasterCompleteFlag;
            if (status)
            {
                completed = true;
                /* Clear complete flag */
                I3C_MasterClearStatusFlags(base, kI3C_MasterCompleteFlag);
                /* Send stop if needed */
                if ((flags & kI3C_TransferNoStopFlag) == 0)
                {
                    result = I3C_MasterEmitStop(base, false);
                    if (result)
                    {
                        return result;
                    }
                }
            }
        }

        /* Check RX data */
        if (rxSize && (base->MDATACTRL & I3C_MDATACTRL_RXCOUNT_MASK))
        {
            *buf++ = base->MRDATAB & I3C_MRDATAB_VALUE_MASK;
            rxSize--;
        }
    }

#if I3C_WAIT_TIMEOUT
    while (I3C_MasterGetState(base) != kI3C_MasterStateIdle && --waitTimes)
#else
    while (I3C_MasterGetState(base) != kI3C_MasterStateIdle)
#endif
    {
    }

    return result;
}

status_t I3C_MasterSend(I3C_Type *base, const void *txBuff, size_t txSize, uint32_t flags)
{
    const uint8_t *buf = (const uint8_t *)((const void *)txBuff);
    status_t result    = kStatus_Success;

    assert(txBuff);

    /* Send data buffer */
    while (txSize--)
    {
        /* Wait until there is room in the fifo. This also checks for errors. */
        result = I3C_MasterWaitForTxReady(base);
        if (result)
        {
            return result;
        }

        /* Write byte into I3C master data register. */
        if (txSize)
        {
            base->MWDATAB = *buf++;
        }
        else
        {
            base->MWDATABE = *buf++;
        }
    }

    result = I3C_MasterWaitForComplete(base, false);
    if (result == kStatus_Success && (flags & kI3C_TransferNoStopFlag) == 0)
    {
        result = I3C_MasterEmitStop(base, true);
    }

    return result;
}

status_t I3C_MasterTransferBlocking(I3C_Type *base, i3c_bus_type_t type, i3c_master_transfer_t *transfer)
{
    status_t result = kStatus_Success;
    i3c_direction_t direction;

    assert(transfer);
    assert(transfer->subaddressSize <= sizeof(transfer->subaddress));

    /* Return an error if the bus is already in use not by us. */
    result = I3C_CheckForBusyBus(base);
    if (result)
    {
        return result;
    }

    /* Clear all flags. */
    I3C_MasterClearStatusFlags(base, kMasterClearFlags);

    direction = transfer->subaddressSize ? kI3C_Write : transfer->direction;
    if (!(transfer->flags & kI3C_TransferNoStartFlag))
    {
        result = I3C_MasterStart(base, type, transfer->slaveAddress, direction,
                                 direction == kI3C_Write ? 0 : transfer->dataSize);
        if (result)
        {
            return result;
        }
    }

    /* Subaddress, MSB first. */
    if (transfer->subaddressSize)
    {
        uint32_t subaddressRemaining = transfer->subaddressSize;
        while (subaddressRemaining--)
        {
            uint8_t subaddressByte = (transfer->subaddress >> (8 * subaddressRemaining)) & 0xff;

            result = I3C_MasterWaitForTxReady(base);
            if (result)
            {
                return result;
            }
            if (!subaddressRemaining && transfer->direction == kI3C_Read)
            {
                base->MWDATABE = subaddressByte;
                result         = I3C_MasterWaitForComplete(base, false);
                if (result)
                {
                    return result;
                }
            }
            else
            {
                base->MWDATAB = subaddressByte;
            }
        }
        /* Need to send repeated start if switching directions to read. */
        if ((transfer->dataSize) && transfer->direction == kI3C_Read)
        {
            result = I3C_MasterRepeatedStart(base, type, transfer->slaveAddress, kI3C_Read, transfer->dataSize);
            if (result)
            {
                return result;
            }
        }
    }

    /* Transmit data. */
    if ((transfer->direction == kI3C_Write) && (transfer->dataSize > 0))
    {
        /* Send Data. */
        result = I3C_MasterSend(base, transfer->data, transfer->dataSize, transfer->flags);
    }
    /* Receive Data. */
    else if ((transfer->direction == kI3C_Read) && (transfer->dataSize > 0))
    {
        result = I3C_MasterReceive(base, transfer->data, transfer->dataSize, transfer->flags);
    }

    return result;
}
