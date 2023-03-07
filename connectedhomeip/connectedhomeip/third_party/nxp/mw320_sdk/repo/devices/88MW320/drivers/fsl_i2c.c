/*
 * Copyright 2020,2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "fsl_i2c.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.i2c"
#endif

/*! brief i2c master transfer state, used in I2C driver internal master transfer state machine. */
enum _i2c_master_transfer_states
{
    kMasterIdleState              = 0x0U, /*!< I2C bus idle. */
    kMasterSendCommandState       = 0x1U, /*!< Send command byte phase. */
    kMasterSendDataState          = 0x2U, /*!< Send data transfer phase. */
    kMasterReceiveDataState       = 0x3U, /*!< Receive data transfer phase. */
    kMasterWaitForCompletionState = 0x4U, /*!< Wait for stop signal detected to complete the transfer phase. */
};

/*******************************************************************************
 * Driver Internal Functions Declaration
 ******************************************************************************/
/*!
 * @brief Wait until there is room in tx FIFO and checks bus errors in master mode.
 *
 * @note: The API only checks the error status that occured on bus, error caused by user misuse such as
 * kI2C_RxUnderflowInterruptFlag, kI2C_TxOverflowInterruptFlag or kI2C_MasterOperateWhenDisabledFlag will not be checked
 *
 * @param base The I2C peripheral base address.
 * @retval #kStatus_I2C_RxHardwareOverrun Master rx FIFO overflow
 * @retval #kStatus_I2C_GeneralCall_Nak Master nacked after issueing general call
 * @retval #kStatus_I2C_ArbitrationLost Master lost arbitration when addressing
 * @retval #kStatus_I2C_Nak Master nacked during data transmitting
 * @retval #kStatus_I2C_Addr_Nak Master nacked during slave probing
 */
static status_t I2C_MasterWaitForTxReady(I2C_Type *base);

/*!
 * @brief Wait until there is room in tx FIFO and checks bus errors in slave mode.
 *
 * @note: The API only checks the error status that occured on bus, error caused by user misuse such as
 * kI2C_RxUnderflowInterruptFlag, kI2C_TxOverflowInterruptFlag, kI2C_SlaveReadWhenRequestedDataFlag or
 * kI2C_SlaveDisabledWhileBusy will not be checked
 *
 * @param base The I2C peripheral base address.
 * @retval #kStatus_Success
 * @retval #kStatus_I2C_RxHardwareOverrun Slave rx FIFO overflow
 * @retval #kStatus_I2C_TxFifoFlushed Slave is addressed and is requested data but there are still old data remaining in
 * tx FIFO
 * @retval #kStatus_I2C_ArbitrationLost Data on SDA is not the same as slave transmitted
 * @retval #kStatus_I2C_Nak Slave nacked by master
 */
static status_t I2C_SlaveWaitForTxReady(I2C_Type *base);

/*!
 * @brief Prepares the master transfer software state machine.
 * @param base I2C base pointer
 * @param handle pointer to i2c_master_transfer_handle_t structure which stores the transfer state.
 */
static void I2C_MasterInitTransferStateMachine(I2C_Type *base, i2c_master_handle_t *handle);

/*!
 * @brief Runs master transfer softerware state machine to perform transfer.
 *
 * Master software state machine controls transfer state, perfroms master data transfer according to user's transfer
 * configuration. For example, if user sets the transfer configuration @ref i2c_master_transfer_t to perform master read
 * with command, this function will execute state in such order: kMasterSendCommandState(Send subaddress),
 * kMasterReceiveDataState(Recieve data).
 *
 * @param base I2C base pointer.
 * @param handle pointer to i2c_master_transfer_handle_t structure which stores the transfer state
 * @param isDone input param to get whether transfer is done, true is done
 * @retval kStatus_Success No error found.
 * @retval #kStatus_I2C_ArbitrationLost Transfer error, arbitration lost.
 * @retval #kStatus_I2C_Nak Received Nak error.
 * @retval #kStatus_I2C_Addr_Nak Received Nak during addressing error.
 * @retval #kStatus_I2C_GeneralCall_Nak Master Nacked after issueing general call
 * @retval #kStatus_I2C_Timeout Transfer error, timeout happens when waiting for status flags to change.
 */
static status_t I2C_MasterRunTransferStateMachine(I2C_Type *base, i2c_master_handle_t *handle, bool *isDone);

/*!
 * @brief Master interrupt handler.
 *
 * @param base I2C base pointer.
 * @param i2cHandle pointer to i2c_master_handle_t structure.
 */
static void I2C_MasterTransferHandleIRQ(I2C_Type *base, void *i2cHandle);

/*!
 * @brief Slave interrupt handler.
 *
 * @param base I2C base pointer.
 * @param i2cHandle pointer to i2c_slave_handle_t structure which stores the transfer state
 */
static void I2C_SlaveTransferHandleIRQ(I2C_Type *base, void *i2cHandle);

/*!
 * @brief Shared IRQ handler that can call both master and slave ISRs.
 *
 * The master and slave ISRs are called through function pointers in order to decouple
 * this code from the ISR functions. Without this, the linker would always pull in both
 * ISRs and every function they call, even if only the functional API was used.
 *
 * @param base I2C base pointer.
 * @param instance The I2C peripheral instance number.
 */
static void I2C_CommonIRQHandler(I2C_Type *base, uint32_t instance);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! brief Pointers to i2c bases for each instance. */
static I2C_Type *const s_I2cBases[] = I2C_BASE_PTRS;

/*! brief Pointers to i2c IRQ number for each instance. */
const IRQn_Type s_I2cIrqs[] = I2C_IRQS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! brief Pointers to i2c clocks for each instance. */
static const clock_ip_name_t i2cClocks[] = I2C_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*! brief Pointers to i2c handles for each instance. */
void *s_i2cHandles[ARRAY_SIZE(s_I2cBases)];

/*! brief Pointer to master IRQ handler for each instance. */
i2c_isr_t s_i2cMasterIsr;

/*! brief Pointer to slave IRQ handler for each instance. */
static i2c_isr_t s_i2cSlaveIsr;

/*******************************************************************************
 * Codes
 ******************************************************************************/
static status_t I2C_MasterWaitForTxReady(I2C_Type *base)
{
    status_t result = kStatus_Success;
    uint32_t status;

#if I2C_RETRY_TIMES
    uint32_t waitTimes = I2C_RETRY_TIMES;
#endif
    do
    {
        /* Check for error flags. */
        status = I2C_MasterGetStatusFlags(base);
        result = I2C_MasterCheckAndClearError(base, status);
        if (kStatus_Success != result)
        {
            break;
        }
#if I2C_RETRY_TIMES
        /* Wait until tx FIFO is not full */
    } while (((base->IC_STATUS & I2C_IC_STATUS_TFNF) == 0U) && (0U != --waitTimes));

    if (0U == waitTimes)
    {
        result = kStatus_I2C_Timeout;
    }
#else
        /* Wait until tx FIFO is not full */
    } while ((base->IC_STATUS & I2C_IC_STATUS_TFNF_MASK) == 0U);
#endif

    return result;
}

static status_t I2C_SlaveWaitForTxReady(I2C_Type *base)
{
    status_t result = kStatus_Success;
    uint32_t status;

#if I2C_RETRY_TIMES
    uint32_t waitTimes = I2C_RETRY_TIMES;
#endif
    do
    {
        /* Check for error flags. */
        status = I2C_SlaveGetStatusFlags(base);
        result = I2C_SlaveCheckAndClearError(base, status);
        if (kStatus_Success != result)
        {
            break;
        }
        /* Wait until master is requesting data */
#if I2C_RETRY_TIMES
    } while (((base->IC_RAW_INTR_STAT & I2C_IC_RAW_INTR_STAT_RD_REQ_MASK) == 0U) && (0U != --waitTimes));

    if (0U == waitTimes)
    {
        result = kStatus_I2C_Timeout;
    }
#else
    } while ((base->IC_RAW_INTR_STAT & I2C_IC_RAW_INTR_STAT_RD_REQ_MASK) == 0U);
#endif
    /* Clear read request flag */
    (void)base->IC_CLR_RD_REQ;

    return result;
}

/*!
 * brief Gets instance number for I2C module.
 *
 * param base I2C base pointer.
 * retval The number of the instance.
 */
uint32_t I2C_GetInstance(I2C_Type *base)
{
    uint32_t instance;
    for (instance = 0U; instance < ARRAY_SIZE(s_I2cBases); ++instance)
    {
        if (s_I2cBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_I2cBases));
    return instance;
}

/*!
 * brief Clears the I2C master status flags.
 *
 * param base I2C base pointer
 * param statusFlags The status flag mask, can be a single flag or several flags in #_i2c_status_flags ORed
 * together.
 */
void I2C_MasterClearStatusFlags(I2C_Type *base, uint32_t statusFlags)
{
    /* These 13 flags from kI2C_Master7bitAddressNackFlag to kI2C_MasterArbitrationLostFlag are cleared when
       kI2C_TxAbortInterruptFlag is cleared, so add the kI2C_TxAbortInterruptFlag if not included in the mask. */
    if ((statusFlags & 0x3FFE0000UL) != 0U)
    {
        statusFlags |= (uint32_t)kI2C_TxAbortInterruptFlag;
    }
    if ((statusFlags & (uint32_t)kI2C_MasterAllClearFlags) != 0U)
    {
        /* Ignore other auto cleared flags. */
        statusFlags &= (uint32_t)kI2C_MasterAllClearFlags;
        /* The software clearable bits are cleared individually by reading their corresponding whole register */
        /* Store the first register base's address */
        uint32_t const volatile *readReg = &base->IC_CLR_RX_UNDER;
        uint8_t regNum;
        /* Bit 0,1,2,3,4,6,8 except 2 and 4 are clearable */
        for (uint8_t i = 0U; i <= 10U; ++i)
        {
            /* If bit i is set, then read the corresponding whole register to clear it */
            if ((statusFlags & (0x1U << i)) != 0U)
            {
                regNum = i;
                if (i == 3U)
                {
                    regNum -= 1U;
                }
                else if (i > 5U)
                {
                    regNum -= 2U;
                }
                else
                {
                    /* Avoid MISRA 15.7 violations */
                }
                /* Read the register */
                *(readReg + regNum);
            }
        }
    }
}

/*!
 * brief Clears the I2C slave status flags.
 *
 * param base I2C base pointer
 * param statusFlags The status flag mask, can be a single flag or several flags in #_i2c_slave_status_flags ORed
 * together
 */
void I2C_SlaveClearStatusFlags(I2C_Type *base, uint32_t statusFlags)
{
    /* These 3 flags are cleared when kI2C_TxAbortInterruptFlag is cleared, so add the kI2C_TxAbortInterruptFlag if not
     * included in the mask. */
    if ((statusFlags & ((uint32_t)kI2C_SlaveFlushTxFifoFlag | (uint32_t)kI2C_SlaveArbitrationLostFlag |
                        (uint32_t)kI2C_SlaveReadWhenRequestedDataFlag)) != 0U)
    {
        statusFlags |= (uint32_t)kI2C_TxAbortInterruptFlag;
    }
    if ((statusFlags & (uint32_t)kI2C_SlaveAllClearFlags) != 0U)
    {
        /* Ignore other auto cleared flags. */
        statusFlags &= (uint32_t)kI2C_SlaveAllClearFlags;
        /* The software clearable bits are cleared individually by reading their corresponding whole register */
        /* Store the first register base's address */
        uint32_t const volatile *readReg = &base->IC_CLR_RX_UNDER;
        uint8_t regNum;
        /* Bit 0-11 except 2 and 4 are clearable */
        for (uint8_t i = 0U; i <= 11U; ++i)
        {
            /* If bit i is set, then read the corresponding whole register to clear it */
            if ((statusFlags & (0x1U << i)) != 0U)
            {
                regNum = i;
                if (i == 3U)
                {
                    regNum -= 1U;
                }
                else if (i >= 5U)
                {
                    regNum -= 2U;
                }
                else
                {
                    /* Avoid MISRA 15.7 violations */
                }
                /* Read the register */
                *(readReg + regNum);
            }
        }
    }
}

/*!
 * brief Sets the I2C master configuration structure to default values.
 *
 * The purpose of this API is to initialize the configuration structure to default value for @ref I2C_MasterInit to use.
 * Use the unchanged structure in @ref I2C_MasterInit or modify the structure before calling @ref I2C_MasterInit.
 * This is an example:
 * code
 *   i2c_master_config_t config;
 *   I2C_MasterGetDefaultConfig(&config);
 *   I2C_MasterInit(I2C0, &config, 12000000U);
 * endcode
 *
 * param masterConfig Pointer to the master configuration structure
 */
void I2C_MasterGetDefaultConfig(i2c_master_config_t *masterConfig)
{
    /* Initializes the configure structure to zero. */
    (void)memset(masterConfig, 0, sizeof(*masterConfig));

    masterConfig->speed                      = kI2C_MasterSpeedStandard;
    masterConfig->addressingMode             = kI2C_AddressMatch7bit;
    masterConfig->highSpeedMasterCodeAddress = 0x1U;
    masterConfig->rxFifoWaterMark            = 0U;
    masterConfig->txFifoWaterMark            = 8U;
    masterConfig->dataHoldTime               = 0x1U;
    masterConfig->dataSetupTime              = 0x64U;
    masterConfig->glitchFilter               = 0x6U;
    masterConfig->glitchFilterHighSpeed      = 0x2U;
    masterConfig->enable                     = false;
}

/*!
 * brief Initializes the I2C peripheral to operate as master.
 *
 * This API initialize the I2C module for master operation.
 *
 * note If FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL is enbaled by user, the init function will not ungate I2C clock source
 * before initialization, to avoid hardfault, user has to manually enable ungate the clock source before calling the API
 *
 * The configuration structure can be filled manully or be set with default values by calling
 * ref I2C_MasterGetDefaultConfig. This is an example.
 * code
 *   I2C_MasterGetDefaultConfig(&config);
 *   I2C_MasterInit(I2C0, &config, 12000000U);
 * endcode
 *
 * param base I2C base pointer
 * param masterConfig Pointer to the master configuration structure
 * param sourceClock_Hz Frequency in Hertz of the I2C functional clock source
 */
void I2C_MasterInit(I2C_Type *base, const i2c_master_config_t *masterConfig, uint32_t sourceClock_Hz)
{
    assert((base != NULL) && (masterConfig != NULL) && (sourceClock_Hz != 0U));
    assert(masterConfig->txFifoWaterMark < I2C_GetTxFifoDepth(base));
    assert(masterConfig->rxFifoWaterMark < I2C_GetRxFifoDepth(base));

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate the clock. */
    CLOCK_EnableClock(i2cClocks[I2C_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
    I2C_MasterSetSpeedMode(base, masterConfig->speed, sourceClock_Hz);
    I2C_MasterSetAddressingMode(base, masterConfig->addressingMode);
    I2C_MasterSetHighSpeedAddress(base, masterConfig->highSpeedMasterCodeAddress);
    I2C_SetRxWaterMark(base, masterConfig->rxFifoWaterMark);
    I2C_SetTxWaterMark(base, masterConfig->txFifoWaterMark);
    I2C_SetSdaHoldTime(base, masterConfig->dataHoldTime);
    I2C_SetSdaSetupTime(base, masterConfig->dataSetupTime);
    I2C_SetGlitchFilter(base, masterConfig->glitchFilter);
    I2C_SetGlitchFilterHighSpeed(base, masterConfig->glitchFilterHighSpeed);
    I2C_EnableMaster(base, true);
    I2C_Enable(base, masterConfig->enable);
}

/*!
 * brief De-initializes the I2C peripheral. Call this API to disable the I2C module.
 *
 * param base I2C base pointer
 */
void I2C_MasterDeinit(I2C_Type *base)
{
    /* Wait until master is not active */
    while ((base->IC_STATUS & I2C_IC_STATUS_MST_ACTIVITY_MASK) != 0U)
    {
    }
    /* Disable I2C module. */
    I2C_Enable(base, false);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Disable I2C clock. */
    CLOCK_DisableClock(i2cClocks[I2C_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Sets the I2C slave configuration structure to default values.
 *
 * The purpose of this API is to initialize the configuration structure for @ref I2C_SlaveInit to use. Use the unchanged
 * initialized structure in @ref I2C_SlaveInit or modify the structure before calling @ref I2C_SlaveInit.
 * This is an example.
 * code
 *   i2c_slave_config_t config;
 *   config.slaveAdress = 0x7FU;
 *   I2C_SlaveGetDefaultConfig(&config);
 * endcode
 *
 * param slaveConfig Pointer to the slave configuration structure
 */
void I2C_SlaveGetDefaultConfig(i2c_slave_config_t *slaveConfig)
{
    assert(slaveConfig != NULL);

    slaveConfig->addressingMode        = kI2C_AddressMatch7bit;
    slaveConfig->rxFifoWaterMark       = 0U;
    slaveConfig->txFifoWaterMark       = 255U;
    slaveConfig->dataHoldTime          = 0x1U;
    slaveConfig->dataSetupTime         = 0x64U;
    slaveConfig->ackGeneralCall        = true;
    slaveConfig->glitchFilter          = 0x6U;
    slaveConfig->glitchFilterHighSpeed = 0x2U;
    slaveConfig->enable                = false;
}

/*!
 * brief Initializes the I2C peripheral to operate as slave.
 *
 * This API initialize the I2C module for slave operation. User can also configure whether to enable the module in the
 * function.
 *
 * note If FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL is enbaled by user, the init function will not ungate I2C clock source
 * before initialization, to avoid hardfault, user has to manually ungate the clock source before calling the API
 *
 * The configuration structure can be filled manully or be set with default values by calling
 * ref I2C_SlaveGetDefaultConfig. This is an example.
 * code
 *    i2c_slave_config_t sConfig;
 *    I2C_SlaveGetDefaultConfig(&sConfig);
 *    sConfig.address = 0x2AU;
 *    I2C_SlaveInit(I2C0, &sConfig);
 * endcode
 *
 * param base I2C base pointer
 * param slaveConfig Pointer to the slave configuration structure
 */
void I2C_SlaveInit(I2C_Type *base, const i2c_slave_config_t *slaveConfig)
{
    assert((base != NULL) && (slaveConfig != NULL));

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate the clock. */
    CLOCK_EnableClock(i2cClocks[I2C_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    I2C_SlaveSetAddressingMode(base, slaveConfig->addressingMode, slaveConfig->slaveAddress);
    I2C_SetRxWaterMark(base, slaveConfig->rxFifoWaterMark);
    I2C_SetTxWaterMark(base, slaveConfig->txFifoWaterMark);
    I2C_SetSdaHoldTime(base, slaveConfig->dataHoldTime);
    I2C_SetSdaSetupTime(base, slaveConfig->dataSetupTime);
    I2C_EnableAckGeneralCall(base, slaveConfig->ackGeneralCall);
    I2C_SetGlitchFilter(base, slaveConfig->glitchFilter);
    I2C_SetGlitchFilterHighSpeed(base, slaveConfig->glitchFilterHighSpeed);
    I2C_EnableMaster(base, false);
    I2C_Enable(base, slaveConfig->enable);
}

/*!
 * brief De-initializes the I2C peripheral. Call this API to disable the I2C module.
 *
 * param base I2C base pointer
 */
void I2C_SlaveDeinit(I2C_Type *base)
{
    /* Wait until slave is not active */
    while ((base->IC_STATUS & I2C_IC_STATUS_SLV_ACTIVITY_MASK) != 0U)
    {
    }
    /* Disable I2C module. */
    I2C_Enable(base, false);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Disable I2C clock. */
    CLOCK_DisableClock(i2cClocks[I2C_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Sets the I2C transaction speed mode.
 *
 * After the I2C module is initialized as master, user can call this function to change the transfer baud rate. There
 * are 3 speed modes to choos from, kI2C_MasterSpeedStandard for 100kbps, kI2C_MasterSpeedFast for 400kbps and
 * kI2C_MasterSpeedHigh for 3.4Mbps.
 *
 * param base I2C base pointer.
 * param speed Master transfer speed mode to be configured
 * param sourceClock_Hz I2C peripheral clock frequency in Hz
 */
void I2C_MasterSetSpeedMode(I2C_Type *base, i2c_master_speed_mode_t speed, uint32_t sourceClock_Hz)
{
    assert((base != NULL) && ((uint8_t)speed != 0U) && (sourceClock_Hz != 0U));

    /* Set speed mode */
    base->IC_CON = (base->IC_CON & ~I2C_IC_CON_SPEED_MASK) | I2C_IC_CON_SPEED((uint8_t)speed);

    uint16_t highCount;
    uint16_t lowCount;
    /* Configure clock count register */
    if (speed == kI2C_MasterSpeedStandard)
    {
        highCount            = (uint16_t)(5U * sourceClock_Hz / 100000U + 5U) / 10U;
        lowCount             = (uint16_t)(5U * sourceClock_Hz / 100000U + 5U) / 10U;
        base->IC_SS_SCL_HCNT = (uint32_t)highCount;
        base->IC_SS_SCL_LCNT = (uint32_t)lowCount;
    }
    else if (speed == kI2C_MasterSpeedFast)
    {
        highCount            = (uint16_t)(sourceClock_Hz / 100000U + 5U) / 10U;
        lowCount             = (uint16_t)(15U * sourceClock_Hz / 1000000U + 5U) / 10U;
        base->IC_FS_SCL_HCNT = (uint32_t)highCount;
        base->IC_FS_SCL_LCNT = (uint32_t)lowCount;
    }
    else
    {
        highCount            = (uint16_t)(4U * sourceClock_Hz / 1000000U + 5U) / 10U;
        lowCount             = (uint16_t)(6U * sourceClock_Hz / 1000000U + 5U) / 10U;
        base->IC_HS_SCL_HCNT = (uint32_t)highCount;
        base->IC_HS_SCL_LCNT = (uint32_t)lowCount;
    }
}

/*!
 * brief Convert provided status flags to status code, and clear any errors if present.
 *
 * note: The API only checks the error status that occured on bus, error caused by user misuse such as
 * kI2C_RxUnderflowInterruptFlag, kI2C_TxOverflowInterruptFlag or kI2C_MasterOperateWhenDisabledFlag will not be checked
 *
 * param base The I2C peripheral base address.
 * param status Status flags to check.
 * retval #kStatus_Success
 * retval #kStatus_I2C_RxHardwareOverrun Master rx FIFO overflow
 * retval #kStatus_I2C_GeneralCall_Nak Master nacked after issueing general call
 * retval #kStatus_I2C_ArbitrationLost Master lost arbitration when addressing
 * retval #kStatus_I2C_Nak Master nacked during data transmitting
 * retval #kStatus_I2C_Addr_Nak Master nacked during slave probing
 */
status_t I2C_MasterCheckAndClearError(I2C_Type *base, uint32_t status)
{
    status_t result = kStatus_Success;
    status &= (uint32_t)kI2C_MasterErrorFlags;
    if (0U != status)
    {
        /* Select the correct error code. Ordered by severity, with bus issues first. */
        if (0U != (status & (uint32_t)kI2C_RxOverflowInterruptFlag))
        {
            /* Empty rx FIFO */
            while ((base->IC_STATUS & I2C_IC_STATUS_RFNE_MASK) != 0U)
            {
                (void)I2C_ReadByte(base);
            }
            result = kStatus_I2C_RxHardwareOverrun;
        }
        if (0U != (status & (uint32_t)kI2C_MasterGeneralCallNackFlag))
        {
            result = kStatus_I2C_GeneralCall_Nak;
        }
        else if (0U != (status & (uint32_t)kI2C_MasterArbitrationLostFlag))
        {
            result = kStatus_I2C_ArbitrationLost;
        }
        else if (0U != (status & (uint32_t)kI2C_MasterDataNackFlag))
        {
            result = kStatus_I2C_Nak;
        }
        else if (0U !=
                 (status & ((uint32_t)kI2C_Master7bitAddressNackFlag | (uint32_t)kI2C_Master10bitAddressNack1Flag |
                            (uint32_t)kI2C_Master10bitAddressNack2Flag)))
        {
            result = kStatus_I2C_Addr_Nak;
        }
        else
        {
            /* Avoid MISRA 15.7 violation */
        }

        /* Clear the flags. */
        I2C_MasterClearStatusFlags(base, status);
    }
    else
    {
        /* Avoid MISRA 15.7 violation */
    }

    return result;
}

/*!
 * brief Sends a piece of data to I2C bus in master mode in blocking way.
 *
 * Call this function when using I2C as master to send certain bytes of data to bus. This function uses the blocking
 * way, which means it does not return until all the data is sent to bus.
 *
 * note The I2C module generates START/STOP/RESTART automatically. When tx FIFO state changes from empty to not empty,
 * start signal is generated; when tx FIFO is empty again, stop signal is generated; when read command follows a write
 * command, restart signal is generated in between. So stop signal will be generated after this API returns, and before
 * calling this API if tx FIFO is empty, start signal will be generated first automatically, if tx FIFO is not empty and
 * the last command in tx FIFO is write, restart signal will be generated first automatically.
 *
 * param base I2C base pointer.
 * param data The pointer to the data to be transmitted.
 * param txSize The length in bytes of the data to be transmitted.
 * retval kStatus_Success Successfully complete the data transmission.
 * retval kStatus_I2C_ArbitrationLost Transfer error, arbitration lost.
 * retval kStatus_I2C_Nak Transfer error, receive NAK during transfer.
 * retval kStatus_I2C_Addr_Nak Transfer error, receive NAK during addressing, only applies when tx FIFO is empty before
 * calling this API.
 * retval #kStatus_I2C_Timeout Transfer error, timeout happens when waiting for status flags to change.
 */
status_t I2C_MasterWriteBlocking(I2C_Type *base, const uint8_t *data, size_t txSize)
{
    assert(NULL != data);

    status_t result = kStatus_Success;
    uint32_t statusFlags;
    uint8_t *buf = (uint8_t *)data;

    /* Send data buffer */
    while (0U != (txSize--))
    {
        /* Wait until there is room in the fifo. This also checks for errors. */
        result = I2C_MasterWaitForTxReady(base);
        if (kStatus_Success != result)
        {
            break;
        }

        I2C_WriteByte(base, *buf++);
    }

    if (result == kStatus_Success)
    {
        statusFlags = I2C_MasterGetStatusFlags(base);
        while ((statusFlags & (uint32_t)kI2C_TxFifoEmptyFlag) != 0U)
        {
            result = I2C_MasterCheckAndClearError(base, statusFlags);
            if (result != kStatus_Success)
            {
                break;
            }
            else
            {
                statusFlags = I2C_MasterGetStatusFlags(base);
            }
        }
    }
    return result;
}

/*!
 * brief Receives a piece of data from I2C bus in master mode in blocking way.
 *
 * Call this function when using I2C as master to receive certain bytes of data from bus. This function uses the
 * blocking way, which means it does not return until all the data is received or is nacked by slave.
 *
 * note When doing read operation, software needs to push dummy data to tx FIFO with read command to generate clock,
 * plus I2C module generates START/STOP/RESTART automatically, meaning when tx FIFO state changes from empty to not
 * empty, start signal is generated, when tx FIFO is empty again, stop signal is generated, when read command follows
 * a write command, restart signal is generated in between. So stop signal will be generated after this API returns,
 * and before calling this API if tx FIFO is empty, start signal will be generated first automatically, if tx FIFO is
 * not empty and the last command in tx FIFO is write, restart signal will be generated first automatically.
 *
 * param base I2C base pointer.
 * param data The pointer to the data to store the received data.
 * param rxSize The length in bytes of the data to be received.
 * retval kStatus_Success Successfully complete the data transmission.
 * retval kStatus_I2C_Timeout Send stop signal failed, timeout.
 */
status_t I2C_MasterReadBlocking(I2C_Type *base, uint8_t *data, size_t rxSize)
{
    status_t result     = kStatus_Success;
    uint8_t txCountEach = 0U;
    uint8_t rxCountEach = 0U;
    uint32_t txSize     = rxSize;

    /* Empty rx FIFO first */
    while ((base->IC_STATUS & I2C_IC_STATUS_RFNE_MASK) != 0U)
    {
        (void)I2C_ReadByte(base);
    }

    while (txSize != 0U)
    {
        /* Calculate how many data can be pushed to tx FIFO */
        txCountEach =
            (uint8_t)MIN(((uint32_t)I2C_GetTxFifoDepth(base) - (uint32_t)I2C_GetTxFifoDataCount(base)), txSize);
        txSize -= txCountEach;
        /* Push dummy to tx FIFO */
        while (txCountEach-- != 0U)
        {
            I2C_MasterWriteDummy(base);
        }
        /* Calculate how many data can be read from rx FIFO */
        rxCountEach = (uint8_t)MIN(I2C_GetRxFifoDataCount(base), rxSize);
        rxSize -= rxCountEach;
        while (rxCountEach-- != 0U)
        {
            *data++ = I2C_ReadByte(base);
        }
    }

    while (rxSize-- != 0U)
    {
#if I2C_RETRY_TIMES
        uint32_t waitTimes = I2C_RETRY_TIMES;
        while (((base->IC_STATUS & I2C_IC_STATUS_RFNE_MASK) == 0U) && (waitTimes-- != 0U))
#else
        while ((base->IC_STATUS & I2C_IC_STATUS_RFNE_MASK) == 0U)
#endif
        {
        }
        *data++ = I2C_ReadByte(base);
    }

    return result;
}

/*!
 * brief Convert provided status flags to status code, and clear any errors if present.
 *
 * note: The API only checks the error status that occured on bus, error caused by user misuse such as
 * kI2C_RxUnderflowInterruptFlag, kI2C_TxOverflowInterruptFlag, kI2C_SlaveReadWhenRequestedDataFlag or
 * kI2C_SlaveDisabledWhileBusy will not be checked
 *
 * param base The I2C peripheral base address.
 * param status Status flags to check.
 * retval #kStatus_Success
 * retval #kStatus_I2C_RxHardwareOverrun Slave rx FIFO overflow
 * retval #kStatus_I2C_TxFifoFlushed Slave is addressed and is requested data but there are still old data remaining in
 * tx FIFO retval #kStatus_I2C_ArbitrationLost Data on SDA is not the same as slave transmitted retval #kStatus_I2C_Nak
 * Slave nacked by master
 */
status_t I2C_SlaveCheckAndClearError(I2C_Type *base, uint32_t status)
{
    status_t result = kStatus_Success;
    status &= (uint32_t)kI2C_SlaveErrorFlags;
    if (0U != status)
    {
        /* Select the correct error code. Ordered by severity, with bus issues first. */
        if (0U != (status & (uint32_t)kI2C_RxOverflowInterruptFlag))
        {
            /* Empty rx FIFO */
            while ((base->IC_STATUS & I2C_IC_STATUS_RFNE_MASK) != 0U)
            {
                (void)I2C_ReadByte(base);
            }
            result = kStatus_I2C_RxHardwareOverrun;
        }
        if (0U != (status & (uint32_t)kI2C_SlaveFlushTxFifoFlag))
        {
            result = kStatus_I2C_TxFifoFlushed;
        }
        else if (0U != (status & (uint32_t)kI2C_SlaveArbitrationLostFlag))
        {
            result = kStatus_I2C_ArbitrationLost;
        }
        else if (0U != (status & (uint32_t)kI2C_SlaveNackInterruptFlag))
        {
            result = kStatus_I2C_Nak;
        }
        else
        {
            /* Avoid MISRA 15.7 violation */
        }

        /* Clear the flags. */
        I2C_SlaveClearStatusFlags(base, status);
    }
    else
    {
        /* Avoid MISRA 15.7 violation */
    }

    return result;
}

/*!
 * brief Sends a piece of data to I2C bus in slave mode in blocking way.
 *
 * Call this funtion to let I2C slave poll register status until it is addressed, then slave sends txSize of data to bus
 * until all the data has been sent to bus or untill it is nacked.
 *
 * param base I2C base pointer.
 * param data The pointer to the data to be transferred.
 * param txSize The length in bytes of the data to be transferred.
 * retval #kStatus_Success
 * retval #kStatus_I2C_TxFifoFlushed Slave is addressed and is requested data but there are still old data remaining in
 * tx FIFO retval #kStatus_I2C_ArbitrationLost Data on SDA is not the same as slave transmitted retval #kStatus_I2C_Nak
 * Slave nacked by master
 */
status_t I2C_SlaveWriteBlocking(I2C_Type *base, const uint8_t *data, size_t txSize)
{
    assert(NULL != data);

    status_t result = kStatus_Success;
    uint8_t *buf    = (uint8_t *)data;

    I2C_SlaveNackNextByte(base, false);
    /* Send data buffer */
    while (0U != (txSize--))
    {
        /* Wait until there is room in the fifo. This also checks for errors. */
        result = I2C_SlaveWaitForTxReady(base);
        if (kStatus_Success != result)
        {
            break;
        }

        I2C_WriteByte(base, *buf++);
    }
    /* For slave data is transmitted one by one after master read request is received, so after the last data is pushed
     * to FIFO, there is no need to check whether it is nacked by master */
    return result;
}

/*!
 * brief Receives a piece of data from I2C bus in slave mode in blocking way.
 *
 * Call this funtion to let I2C slave poll register status until it is addressed, then slave receives rxSize of data
 * until all the data has been received.
 *
 * param base I2C base pointer.
 * param data The pointer to the data to store the received data.
 * param rxSize The length in bytes of the data to be received.
 * retval kStatus_Success Successfully complete data receive.
 * retval kStatus_I2C_Timeout Wait status flag timeout.
 */
status_t I2C_SlaveReadBlocking(I2C_Type *base, uint8_t *data, size_t rxSize)
{
    status_t result     = kStatus_Success;
    uint8_t rxCountEach = 0U;

    /* Empty rx FIFO first */
    while ((base->IC_STATUS & I2C_IC_STATUS_RFNE_MASK) != 0U)
    {
        (void)I2C_ReadByte(base);
    }

#if I2C_RETRY_TIMES
    uint32_t waitTimes = I2C_RETRY_TIMES;
    while ((rxSize != 0U) && (waitTimes-- != 0U))
#else
    while (rxSize != 0U)
#endif
    {
        /* Calculate how many data can be read from rx FIFO */
        rxCountEach = (uint8_t)MIN(I2C_GetRxFifoDataCount(base), rxSize);
        rxSize -= rxCountEach;
        while (rxCountEach-- != 0U)
        {
            *data++ = I2C_ReadByte(base);
        }
    }

    return result;
}

/*!
 * brief Performs a master polling transfer on the I2C bus.
 *
 * note The API does not return until the transfer succeeds or fails due to arbitration lost or receiving a NAK.
 *
 * param base I2C base pointer.
 * param transferConfig Pointer to the transfer configuration structure.
 * retval kStatus_Success Successfully complete the data transmission.
 * retval #kStatus_I2C_Busy Previous transmission still not finished.
 * retval #kStatus_I2C_Timeout Transfer error, timeout happens when waiting for status flags to change.
 * retval #kStatus_I2C_ArbitrationLost Transfer error, arbitration lost.
 * retval #kStatus_I2C_Nak Transfer error, receive NAK during transfer.
 * retval #kStatus_I2C_Addr_Nak Transfer error, receive NAK during slave probing.
 * retval #kStatus_I2C_GeneralCall_Nak Transfer error, receive NAK after issueing general call.
 */
status_t I2C_MasterTransferBlocking(I2C_Type *base, i2c_master_transfer_t *transferConfig)
{
    assert(NULL != transferConfig);
    assert(I2C_IsMaster(base));

    if ((I2C_MasterGetStatusFlags(base) & (uint32_t)kI2C_BusBusyInterruptFlag) != 0U)
    {
        return kStatus_I2C_Busy;
    }

    status_t result = kStatus_Success;
    uint8_t commandBuffer[4U];

    /* Enable module */
    I2C_Enable(base, true);

    if (transferConfig->slaveAddress == 0U)
    {
        I2C_MasterEnableGeneralCall(base, true);
    }
    else
    {
        I2C_MasterEnableGeneralCall(base, false);
        I2C_MasterSetTargetAddress(base, transferConfig->slaveAddress);
    }

    /* Empty rx FIFO first */
    while ((base->IC_STATUS & I2C_IC_STATUS_RFNE_MASK) != 0U)
    {
        (void)I2C_ReadByte(base);
    }

    /* Send subaddress first, start will be issued automatically when tx FIFO is not empty */
    if (transferConfig->subaddressSize != 0U)
    {
        size_t cmdCount;
        for (cmdCount = 1U; cmdCount <= transferConfig->subaddressSize; cmdCount++)
        {
            commandBuffer[cmdCount - 1U] =
                (uint8_t)((transferConfig->subaddress >> (8U * (transferConfig->subaddressSize - cmdCount))) & 0xffU);
        }
        result = I2C_MasterWriteBlocking(base, &commandBuffer[0U], transferConfig->subaddressSize);
    }

    /* Return if error. */
    if (kStatus_Success != result)
    {
        if (result == kStatus_I2C_Nak)
        {
            result = kStatus_I2C_Addr_Nak;
        }
        return result;
    }

    if (transferConfig->direction == kI2C_Write)
    {
        result = I2C_MasterWriteBlocking(base, transferConfig->data, transferConfig->dataSize);
    }
    else
    {
        result = I2C_MasterReadBlocking(base, transferConfig->data, transferConfig->dataSize);
    }

    /* Wait until stop signal is issued automatically. */
    while ((base->IC_RAW_INTR_STAT & I2C_IC_RAW_INTR_STAT_STOP_DET_MASK) == 0U)
    {
    }

    /* Clear all status */
    I2C_MasterClearStatusFlags(base, (uint32_t)kI2C_MasterAllClearFlags);

    return result;
}

/*!
 * brief Initializes the I2C master transfer in interrupt way.
 *
 * This function is responsible for initializig master transfer handle, installing user callback, registering master
 * IRQ handling function and opening global interrupt.
 *
 * param base I2C base pointer
 * param handle pointer to i2c_master_transfer_handle_t structure to store the transfer state
 * param callback pointer to user callback function
 * param userData User configurable pointer to any data, function, structure etc that user wish to use in the callback
 */
void I2C_MasterTransferCreateHandle(I2C_Type *base,
                                    i2c_master_handle_t *handle,
                                    i2c_master_transfer_callback_t callback,
                                    void *userData)
{
    assert(NULL != handle);
    /* Clear out the handle. */
    (void)memset(handle, 0, sizeof(*handle));

    uint32_t instance;
    /* Look up instance number */
    instance = I2C_GetInstance(base);

    /* Save callback and userData. */
    handle->completionCallback = callback;
    handle->userData           = userData;

    /* Save the handle into static handle array */
    s_i2cHandles[instance] = handle;
    /* Save IRQ handler into static ISR function pointer. */
    s_i2cMasterIsr = I2C_MasterTransferHandleIRQ;

    /* Disable internal IRQs and enable global IRQ. */
    I2C_DisableInterrupts(base, (uint16_t)kI2C_AllInterruptEnable);
    (void)EnableIRQ(s_I2cIrqs[instance]);
}

/*!
 * brief Initiates a master transfer on the I2C bus in interrupt way.
 *
 * note Transfer in interrupt way is non-blocking which means this API returns immediately after transfer initiates.
 * User can call @ref I2C_MasterTransferGetCount to get the count of data that master has transmitted/received and check
 * transfer status. If the return status is kStatus_NoTransferInProgress, the transfer is finished. Also if user
 * installs a user callback when calling @ref I2C_MasterTransferCreateHandle before, the callback will be invoked when
 * transfer finishes.
 *
 * param base I2C base pointer
 * param handle pointer to i2c_master_transfer_handle_t structure which stores the transfer state.
 * param transfer Pointer to the transfer configuration structure.
 * retval kStatus_Success Successfully start the data transmission.
 * retval #kStatus_I2C_Busy Previous transmission still not finished.
 */
status_t I2C_MasterTransferNonBlocking(I2C_Type *base, i2c_master_handle_t *handle, i2c_master_transfer_t *transfer)
{
    assert((NULL != handle) && (NULL != transfer));

    /* Return busy if another transaction is in progress. */
    if ((handle->state != (uint8_t)kMasterIdleState) ||
        ((I2C_MasterGetStatusFlags(base) & (uint32_t)kI2C_BusBusyInterruptFlag) != 0U))
    {
        return kStatus_I2C_Busy;
    }

    /* Disable I2C IRQ sources while we configure stuff. */
    I2C_DisableInterrupts(base, (uint16_t)kI2C_AllInterruptEnable);

    /* Save transfer into handle. */
    handle->transfer = *transfer;

    /* Generate commands to send. */
    I2C_MasterInitTransferStateMachine(base, handle);
    /* Clear all flags. */
    I2C_MasterClearStatusFlags(base, (uint32_t)kI2C_MasterAllClearFlags);
    /* Enable I2C internal IRQ sources */
    I2C_EnableInterrupts(base, ((uint16_t)kI2C_TxRequestInterruptEnable | (uint16_t)kI2C_TxAbortInterruptEnable));

    return kStatus_Success;
}

/*!
 * brief Gets the master transfer count and status during a interrupt transfer.
 *
 * param base I2C base pointer.
 * param handle pointer to i2c_master_handle_t structure which stores the transfer state.
 * param count Pointer to number of bytes transferred so far by the non-blocking transaction.
 * retval kStatus_InvalidArgument count is Invalid.
 * retval kStatus_NoTransferInProgress Curent no transfer is in progress.
 * retval kStatus_Success Successfully obtained the count.
 */
status_t I2C_MasterTransferGetCount(I2C_Type *base, i2c_master_handle_t *handle, size_t *count)
{
    assert(NULL != handle);
    status_t result = kStatus_Success;
    if (NULL == count)
    {
        result = kStatus_InvalidArgument;
    }
    /* Catch when there is not an active transfer. */
    else if (handle->state == (uint8_t)kMasterIdleState)
    {
        *count = 0U;
        result = kStatus_NoTransferInProgress;
    }
    else
    {
        /* Cache some fields with IRQs disabled. This ensures all field values */
        /* are synchronized with each other during an ongoing transfer. */
        uint16_t irqs = I2C_GetEnabledInterrupts(base);
        I2C_DisableInterrupts(base, irqs);

        /* Get transfer count based on current transfer state. */
        switch (handle->state)
        {
            case (uint8_t)kMasterIdleState:
            case (uint8_t)kMasterSendCommandState:
                *count = 0;
                break;

            case (uint8_t)kMasterSendDataState:
                *count = handle->transfer.dataSize - handle->transmitSize - I2C_GetTxFifoDataCount(base);
                break;

            case (uint8_t)kMasterReceiveDataState:
                *count = handle->transfer.dataSize - handle->receiveSize;
                break;

            case (uint8_t)kMasterWaitForCompletionState:
                if (handle->transfer.direction == kI2C_Write)
                {
                    *count = handle->transfer.dataSize - I2C_GetTxFifoDataCount(base);
                }
                else
                {
                    *count = handle->transfer.dataSize;
                }
                break;

            default:
                assert(false);
                break;
        }
        I2C_EnableInterrupts(base, irqs);
    }
    return result;
}

/*!
 * brief Aborts an in-process transfer in interrupt way.
 *
 * note This API can be called at any time after a transfer of interrupt way initiates and before it finishes to abort
 * the transfer early.
 *
 * param base I2C base pointer.
 * param handle pointer to i2c_master_handle_t structure which stores the transfer state
 */
void I2C_MasterTransferAbort(I2C_Type *base, i2c_master_handle_t *handle)
{
    if (handle->state != (uint8_t)kMasterIdleState)
    {
        /* Disable internal IRQ enables. */
        I2C_DisableInterrupts(base, (uint16_t)kI2C_AllInterruptEnable);

        /* Disable the module to reset FIFO. */
        I2C_Enable(base, false);

        /* Reset handle. */
        handle->state = (uint8_t)kMasterIdleState;
    }
}

static void I2C_MasterInitTransferStateMachine(I2C_Type *base, i2c_master_handle_t *handle)
{
    /* Enable module */
    I2C_Enable(base, true);

    if (handle->transfer.slaveAddress == 0U)
    {
        I2C_MasterEnableGeneralCall(base, true);
    }
    else
    {
        I2C_MasterEnableGeneralCall(base, false);
        I2C_MasterSetTargetAddress(base, handle->transfer.slaveAddress);
    }

    /* Empty rx FIFO first */
    while ((base->IC_STATUS & I2C_IC_STATUS_RFNE_MASK) != 0U)
    {
        (void)I2C_ReadByte(base);
    }

    handle->transmitSize = handle->transfer.dataSize;
    handle->receiveSize  = handle->transfer.dataSize;

    if (handle->transfer.subaddressSize != 0U)
    {
        handle->state = (uint8_t)kMasterSendCommandState;
    }
    else if (handle->transfer.direction == kI2C_Write)
    {
        handle->state = (uint8_t)kMasterSendDataState;
    }
    else
    {
        handle->state = (uint8_t)kMasterReceiveDataState;
    }
}

static void I2C_MasterTransferHandleIRQ(I2C_Type *base, void *i2cHandle)
{
    assert(i2cHandle != NULL);
    i2c_master_handle_t *handle = (i2c_master_handle_t *)i2cHandle;
    bool isDone                 = false;
    status_t result;

    if (handle->state != (uint8_t)kMasterIdleState)
    {
        result = I2C_MasterRunTransferStateMachine(base, handle, &isDone);

        if ((result != kStatus_Success) || isDone)
        {
            /* Disable internal IRQ enables. */
            I2C_DisableInterrupts(base, (uint16_t)kI2C_AllInterruptEnable);
            /* Clear all status */
            I2C_MasterClearStatusFlags(base, (uint32_t)kI2C_MasterAllClearFlags);
            /* Set handle to idle state. */
            handle->state = (uint8_t)kMasterIdleState;
            /* Handle error */
            if (result != kStatus_Success)
            {
                /* Disable and re-enable the module to reset FIFO. */
                I2C_Enable(base, false);
                I2C_Enable(base, true);
            }

            /* Invoke callback. */
            if (NULL != handle->completionCallback)
            {
                handle->completionCallback(base, handle, result, handle->userData);
            }
        }
    }
}

static status_t I2C_MasterRunTransferStateMachine(I2C_Type *base, i2c_master_handle_t *handle, bool *isDone)
{
    status_t result = kStatus_Success;
    uint32_t status;
    uint8_t txFifoCount = I2C_GetTxFifoDataCount(base);
    uint8_t txCount;
    uint8_t rxCount             = (uint8_t)MIN(I2C_GetRxFifoDataCount(base), handle->receiveSize);
    i2c_master_transfer_t *xfer = &handle->transfer;

    /* Set default isDone return value. */
    *isDone = false;

    /* Check for errors. */
    status = I2C_MasterGetStatusFlags(base);
    /* For the last byte, nack flag is expected. Do not check and clear kI2C_MasterDataNackFlag for the last byte */
    if ((handle->transmitSize == 0U) && (txFifoCount == 0U))
    {
        status &= ~(uint32_t)kI2C_MasterDataNackFlag;
    }
    result = I2C_MasterCheckAndClearError(base, status);
    if (kStatus_Success != result)
    {
        return result;
    }

    /* Execute the state. */
    switch (handle->state)
    {
        case (uint8_t)kMasterSendCommandState:
            txCount = (uint8_t)MIN(((uint32_t)I2C_GetTxFifoDepth(base) - (uint32_t)txFifoCount), xfer->subaddressSize);
            while (txCount-- != 0U)
            {
                xfer->subaddressSize--;
                I2C_WriteByte(base, (uint8_t)((xfer->subaddress >> (8U * xfer->subaddressSize)) & 0xffU));
            }
            if (xfer->subaddressSize == 0U)
            {
                if (0U != xfer->dataSize)
                {
                    if (xfer->direction == kI2C_Read)
                    {
                        handle->state = (uint8_t)kMasterReceiveDataState;
                        /* Enable rx interrupt */
                        I2C_EnableInterrupts(
                            base, ((uint16_t)kI2C_RxOverflowInterruptFlag | (uint16_t)kI2C_RxDataReadyInterruptFlag));
                    }
                    else
                    {
                        handle->state = (uint8_t)kMasterSendDataState;
                    }
                }
                else
                {
                    /* No transfer, so move to stop state. */
                    handle->state = (uint8_t)kMasterWaitForCompletionState;
                }
            }
            break;

        case (uint8_t)kMasterSendDataState:
            txCount = (uint8_t)MIN(((uint32_t)I2C_GetTxFifoDepth(base) - (uint32_t)txFifoCount), handle->transmitSize);
            while (txCount-- != 0U)
            {
                handle->transmitSize--;
                I2C_WriteByte(base, *xfer->data++);
            }
            if (0U == handle->transmitSize)
            {
                handle->state = (uint8_t)kMasterWaitForCompletionState;
            }
            break;

        case (uint8_t)kMasterReceiveDataState:
            txCount = (uint8_t)MIN(((uint32_t)I2C_GetTxFifoDepth(base) - (uint32_t)txFifoCount), handle->transmitSize);
            while (rxCount-- != 0U)
            {
                handle->receiveSize--;
                *(xfer->data)++ = I2C_ReadByte(base);
            }
            if (handle->receiveSize == 0U)
            {
                handle->state = (uint8_t)kMasterWaitForCompletionState;
            }
            while (txCount-- != 0U)
            {
                handle->transmitSize--;
                I2C_MasterWriteDummy(base);
            }
            break;

        case (uint8_t)kMasterWaitForCompletionState:
            /* We stay in this state until the stop is detected. */
            if (0U != (status & (uint32_t)kI2C_StopDetectInterruptFlag))
            {
                *isDone = true;
            }
            break;

        default:
            assert(false);
            break;
    }
    return result;
}

/*!
 * brief Initializes the I2C slave transfer in interrupt way.
 *
 * This function is responsible for initializig slave transfer handle, installing user callback, registering master
 * IRQ handling function and opening global interrupt.
 *
 * param base I2C base pointer
 * param handle pointer to i2c_slave_handle_t structure to store the transfer state
 * param callback pointer to user callback function
 * param userData User configurable pointer to any data, function, structure etc that user wish to use in the callback
 */
void I2C_SlaveTransferCreateHandle(I2C_Type *base,
                                   i2c_slave_handle_t *handle,
                                   i2c_slave_transfer_callback_t callback,
                                   void *userData)
{
    assert(NULL != handle);
    /* Clear out the handle. */
    (void)memset(handle, 0, sizeof(*handle));

    uint32_t instance;
    /* Look up instance number */
    instance = I2C_GetInstance(base);

    /* Save callback and userData. */
    handle->callback = callback;
    handle->userData = userData;

    /* Save the handle into static handle array */
    s_i2cHandles[instance] = handle;
    /* Save IRQ handler into static ISR function pointer. */
    s_i2cSlaveIsr = I2C_SlaveTransferHandleIRQ;

    /* Disable internal IRQs and enable global IRQ. */
    I2C_DisableInterrupts(base, (uint16_t)kI2C_AllInterruptEnable);
    (void)EnableIRQ(s_I2cIrqs[instance]);
}

/*!
 * brief Sets I2C slave ready to process bus events.
 *
 * Call this API to let I2C start monitoring bus events driven by I2C master on bus. When user specified event occurs,
 * callback will be invoked passes event information to the callback.
 *
 * note When #kI2C_SlaveOutofTransmitDataEvent and #kI2C_SlaveReceiveEvent occured, slave callback will
 * always be revoked regardless which events user choose to enable. This means user need not configure them in the
 * eventMask. If user wants to enable all the events, use #kI2C_SlaveAllEvents for convenience.
 *
 * param base The I2C peripheral base address.
 * param handle Pointer to #i2c_slave_transfer_handle_t structure which stores the transfer state.
 * param eventMask Bit mask formed by OR operation of event(s) in #i2c_slave_transfer_event_t enumerators. When these
 *      events occur the callback will be invoked.
 * retval kStatus_I2C_Busy I2C slave has already been started on this handle.
 */
status_t I2C_SlaveTransferNonBlocking(I2C_Type *base, i2c_slave_handle_t *handle, uint8_t eventMask)
{
    assert(NULL != handle);

    status_t result = kStatus_Success;
    /* Return busy if slave is already in transaction, or the bus is occupied with other transfer. */
    if ((handle->isBusy) || ((I2C_SlaveGetStatusFlags(base) & (uint32_t)kI2C_BusBusyInterruptFlag) != 0U))
    {
        return kStatus_I2C_Busy;
    }

    /* Enable module */
    I2C_Enable(base, true);
    /* Empty rx FIFO first */
    while ((base->IC_STATUS & I2C_IC_STATUS_RFNE_MASK) != 0U)
    {
        (void)I2C_ReadByte(base);
    }

    /* Disable I2C IRQ sources while we configure stuff. */
    I2C_DisableInterrupts(base, (uint16_t)kI2C_AllInterruptEnable);
    /* Clear transfer in handle. */
    (void)memset(&handle->transfer, 0, sizeof(handle->transfer));
    /* Record that we're busy. */
    handle->isBusy = true;
    /* Set up event mask. tx and rx are always enabled. */
    handle->eventMask = eventMask | (uint8_t)kI2C_SlaveTransmitEvent | (uint8_t)kI2C_SlaveReceiveEvent;
    /* Disable slave nack the next byte */
    I2C_SlaveNackNextByte(base, false);
    /* Clear all flags. */
    I2C_SlaveClearStatusFlags(base, (uint16_t)kI2C_SlaveAllClearFlags);
    /* Enable I2C internal IRQ sources used in slave transfer. Global IRQ was enabled in I2C_SlaveTransferCreateHandle
     */
    I2C_EnableInterrupts(base, (uint16_t)kI2C_SlaveInterruptEnable);

    return result;
}

/*!
 * brief Gets how many bytes slave have transferred in curent data buffer.
 *
 * param base I2C base pointer.
 * param handle pointer to i2c_slave_transfer_handle_t structure.
 * param count Number of bytes slave have transferred after the last start/repeated start.
 * retval kStatus_InvalidArgument count is Invalid.
 * retval kStatus_NoTransferInProgress Curent no transfer is in progress.
 * retval kStatus_Success Successfully obtained the count.
 */
status_t I2C_SlaveTransferGetCount(I2C_Type *base, i2c_slave_handle_t *handle, size_t *count)
{
    assert(NULL != handle);

    if (NULL == count)
    {
        return kStatus_InvalidArgument;
    }

    /* Catch when there is not an active transfer. */
    if (false == handle->isBusy)
    {
        *count = 0U;
        return kStatus_NoTransferInProgress;
    }

    /* For an active transfer, just return the count from the handle. */
    *count = handle->transferredCount;

    return kStatus_Success;
}

/*!
 * brief Aborts the slave transfer.
 *
 * note This API can be called at any time to stop slave for handling further bus events.
 *
 * param base I2C base pointer.
 * param handle pointer to i2c_slave_transfer_handle_t structure which stores the transfer state.
 */
void I2C_SlaveTransferAbort(I2C_Type *base, i2c_slave_handle_t *handle)
{
    assert(NULL != handle);

    if (handle->isBusy)
    {
        /* Disable I2C IRQ sources. */
        I2C_DisableInterrupts(base, (uint16_t)kI2C_AllInterruptEnable);
        /* Set slave to nack data automatically */
        I2C_SlaveNackNextByte(base, true);

        /* Reset transfer info. */
        (void)memset(&handle->transfer, 0, sizeof(handle->transfer));

        /* We're no longer busy. */
        handle->isBusy = false;
    }
}

static void I2C_SlaveTransferHandleIRQ(I2C_Type *base, void *i2cHandle)
{
    assert(NULL != i2cHandle);

    i2c_slave_handle_t *handle = (i2c_slave_handle_t *)i2cHandle;
    uint32_t statusFlags       = I2C_SlaveGetStatusFlags(base);
    i2c_slave_transfer_t *xfer = &(handle->transfer);
    size_t tmpDataSize;
    uint8_t rxFullSlots;

    /* Check stop flag. */
    if (0U != (statusFlags & (uint32_t)kI2C_StopDetectInterruptFlag))
    {
        /* Read IC_CLR_STOP_DET register to clear the stop detect interrupt flag */
        (void)base->IC_CLR_STOP_DET;

        /* Call slave callback if this is the STOP of the transfer. */
        if (true == handle->isBusy)
        {
            xfer->event            = kI2C_SlaveCompletionEvent;
            xfer->completionStatus = kStatus_Success;
            handle->isBusy         = false;

            if ((0U != (handle->eventMask & (uint8_t)xfer->event)) && (NULL != handle->callback))
            {
                handle->callback(base, xfer, handle->userData);
            }
        }
    }

    /* Check start flag. */
    if (0U != (statusFlags & (uint32_t)kI2C_StartDetectInterruptFlag))
    {
        /* Read IC_CLR_START_DET register to clear the start detect interrupt flag */
        (void)base->IC_CLR_START_DET;
        /* Disable slave nack the next byte */
        I2C_SlaveNackNextByte(base, false);

        handle->isBusy = true;
        xfer->event    = kI2C_SlaveStartDetectEvent;

        if ((0U != (handle->eventMask & (uint8_t)xfer->event)) && (NULL != handle->callback))
        {
            handle->callback(base, xfer, handle->userData);
        }
    }

    /* Check general call ACK flag. */
    if (0U != (statusFlags & (uint32_t)kI2C_SlaveGeneralCallAckInterruptFlag))
    {
        /* Read IC_CLR_GEN_CALL register to clear the general call ack interrupt flag */
        (void)base->IC_CLR_GEN_CALL;
        /* Disable slave nack the next byte */
        I2C_SlaveNackNextByte(base, false);

        handle->isBusy = true;
        xfer->event    = kI2C_SlaveGenaralcallEvent;

        if ((0U != (handle->eventMask & (uint8_t)xfer->event)) && (NULL != handle->callback))
        {
            handle->callback(base, xfer, handle->userData);
        }
    }

    /* Check rx overflow flag. */
    if (0U != (statusFlags & (uint32_t)kI2C_RxOverflowInterruptFlag))
    {
        /* Read IC_CLR_RX_OVER register to clear the rx FIFO overflow interrupt flag */
        (void)base->IC_CLR_RX_OVER;
        /* Set slave to nack the next byte to terminate the transfer */
        I2C_SlaveNackNextByte(base, true);

        xfer->event            = kI2C_SlaveCompletionEvent;
        xfer->completionStatus = kStatus_I2C_RxHardwareOverrun;
        handle->isBusy         = false;

        if ((0U != (handle->eventMask & (uint8_t)xfer->event)) && (NULL != handle->callback))
        {
            handle->callback(base, xfer, handle->userData);
        }
    }

    /* Check NAK */
    if (0U != (statusFlags & (uint32_t)kI2C_SlaveNackInterruptFlag))
    {
        /* Read IC_CLR_RX_DONE register to clear the slave nack interrupt flag */
        (void)base->IC_CLR_RX_DONE;

        if (handle->transfer.dataSize != 0U)
        {
            xfer->completionStatus = kStatus_I2C_Nak;
        }
        else
        {
            xfer->completionStatus = kStatus_Success;
        }
        xfer->event    = kI2C_SlaveCompletionEvent;
        handle->isBusy = false;
        if ((0U != (handle->eventMask & (uint8_t)xfer->event)) && (NULL != handle->callback))
        {
            handle->callback(base, xfer, handle->userData);
        }
    }

    /* Check arbitration, though slave does not really own the bus, there is still such incident as the data on SDA line
       is not the same as slave sends, in such case we call slave loses arbitration */
    if (0U != (statusFlags & (uint32_t)kI2C_SlaveArbitrationLostFlag))
    {
        /* Read IC_CLR_TX_ABRT register to clear the tx abort interrupt flag */
        (void)base->IC_CLR_TX_ABRT;

        xfer->event            = kI2C_SlaveCompletionEvent;
        xfer->completionStatus = kStatus_I2C_ArbitrationLost;
        handle->isBusy         = false;
        if ((0U != (handle->eventMask & (uint8_t)xfer->event)) && (NULL != handle->callback))
        {
            handle->callback(base, xfer, handle->userData);
        }
    }

    /* Receive data */
    if (0U != (statusFlags & (uint32_t)kI2C_RxDataReadyInterruptFlag))
    {
        tmpDataSize = xfer->dataSize;
        /* If we're out of data, invoke callback to get more. */
        if ((NULL == xfer->data) || (0U == tmpDataSize))
        {
            xfer->event = kI2C_SlaveReceiveEvent;
            if (NULL != handle->callback)
            {
                handle->callback(base, xfer, handle->userData);
            }
            /* Clear the transferred count now that we have a new buffer. */
            handle->transferredCount = 0U;
        }

        rxFullSlots = I2C_GetRxFifoDataCount(base);
        while ((rxFullSlots != 0U) && (0U != xfer->dataSize))
        {
            /* Receive data. */
            *handle->transfer.data++ = I2C_ReadByte(base);
            handle->transferredCount++;
            xfer->dataSize--;
            rxFullSlots--;
            if (0U == xfer->dataSize)
            {
                xfer->event            = kI2C_SlaveCompletionEvent;
                xfer->completionStatus = kStatus_Success;
                handle->isBusy         = false;

                /* Proceed receive complete event. */
                if (((handle->eventMask & (uint8_t)xfer->event) != 0U) && (handle->callback != NULL))
                {
                    handle->callback(base, xfer, handle->userData);
                }
            }
        }
    }

    /* Master request data */
    if (0U != (statusFlags & (uint32_t)kI2C_SlaveReadRequestInterruptFlag))
    {
        /* Read IC_CLR_RD_REQ register to clear the read request interrupt flag */
        (void)base->IC_CLR_RD_REQ;

        tmpDataSize = xfer->dataSize;
        /* If we're out of data, invoke callback to get more. */
        if ((NULL == xfer->data) || (0U == tmpDataSize))
        {
            xfer->event = kI2C_SlaveTransmitEvent;
            if (NULL != handle->callback)
            {
                handle->callback(base, xfer, handle->userData);
            }
            /* Clear the transferred count now that we have a new buffer. */
            handle->transferredCount = 0U;
        }

        if (0U != (handle->transfer.dataSize))
        {
            /* Send data. */
            I2C_WriteByte(base, *handle->transfer.data++);
            handle->transfer.dataSize--;
            handle->transferredCount++;
        }
        else
        {
            xfer->event            = kI2C_SlaveCompletionEvent;
            xfer->completionStatus = kStatus_Success;
            handle->isBusy         = false;

            /* Proceed txdone event. */
            if (((handle->eventMask & (uint8_t)xfer->event) != 0U) && (handle->callback != NULL))
            {
                handle->callback(base, xfer, handle->userData);
            }
        }
    }
}

static void I2C_CommonIRQHandler(I2C_Type *base, uint32_t instance)
{
    /* Check for master function. */
    if (I2C_IsMaster(base))
    {
        /* Master mode. */
        s_i2cMasterIsr(base, s_i2cHandles[instance]);
    }

    /* Check for slave function. */
    if (I2C_IsSlave(base))
    {
        /* Slave mode. */
        s_i2cSlaveIsr(base, s_i2cHandles[instance]);
    }
    SDK_ISR_EXIT_BARRIER;
}

#if defined(I2C0)
void I2C0_DriverIRQHandler(void);
/* Implementation of I2C0 handler named in startup code. */
void I2C0_DriverIRQHandler(void)
{
    I2C_CommonIRQHandler(I2C0, 0U);
}
#endif

#if defined(I2C1)
void I2C1_DriverIRQHandler(void);
/* Implementation of I2C1 handler named in startup code. */
void I2C1_DriverIRQHandler(void)
{
    I2C_CommonIRQHandler(I2C1, 1U);
}
#endif
