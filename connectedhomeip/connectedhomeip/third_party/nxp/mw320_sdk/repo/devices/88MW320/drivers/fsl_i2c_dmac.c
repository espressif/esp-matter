/*
 * Copyright 2020,2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_i2c_dmac.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.i2c_dmac"
#endif

/*! brief i2c master dmac transfer state, used in I2C dmac driver internal transfer state. */
enum _i2c_master_dmac_transfer_states
{
    kMasterIdleState = 0x0U, /*!< I2C bus idle. */
    kMasterBusy      = 0x1U, /*!< i2C bus busy. */
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief DMAC callback for I2C master DMAC transfer.
 *
 * @param handle DMAC handle
 * @param userData user param passed to the callback function
 * @param status Transfer completion status
 */
static void I2C_MasterTransferCallbackDMA(dmac_handle_t *handle, void *userData, uint32_t status);

/*!
 * @brief Master interrupt handler.
 *
 * @param base I2C base pointer.
 * @param i2cHandle pointer to i2c_master_handle_t structure.
 */
static void I2C_MasterDmacTransferHandleIRQ(I2C_Type *base, void *i2cHandle);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! brief The dummy data used to send to generate clock when performing read operatipn. */
static uint32_t g_TxDummy = 0x100UL;

/*******************************************************************************
 * Codes
 ******************************************************************************/
static void I2C_MasterTransferCallbackDMA(dmac_handle_t *handle, void *userData, uint32_t status)
{
    i2c_master_dmac_handle_t *i2cDmacHandle = (i2c_master_dmac_handle_t *)userData;

    if (i2cDmacHandle->dataSize == 0U)
    {
        /* Disable tx and rx dma function. */
        I2C_DisableDMA(i2cDmacHandle->base, (uint8_t)kQSPI_DmaTx | (uint8_t)kQSPI_DmaRx);
        /* Disable tx abort interrupt and enable stop detect interrupt. */
        I2C_DisableInterrupts(i2cDmacHandle->base, (uint16_t)kI2C_TxAbortInterruptEnable);
        I2C_EnableInterrupts(i2cDmacHandle->base, (uint16_t)kI2C_StopDetectInterruptEnable);
    }
    else
    {
        if (i2cDmacHandle->dataSize > I2C_TX_LENGTH)
        {
            i2cDmacHandle->dmacTxConfig.transferTotalBytes = I2C_TX_LENGTH * 2U;
        }
        else
        {
            i2cDmacHandle->dmacTxConfig.transferTotalBytes = i2cDmacHandle->dataSize * 2U;
        }

        for (uint32_t i = 0U; i < i2cDmacHandle->dmacTxConfig.transferTotalBytes / 2U; i++)
        {
            i2cDmacHandle->txData[i] =
                (uint16_t)i2cDmacHandle->data[i2cDmacHandle->transferSize - i2cDmacHandle->dataSize + i];
        }
        i2cDmacHandle->dataSize -= i2cDmacHandle->dmacTxConfig.transferTotalBytes / 2U;

        (void)DMAC_TransferSubmit(i2cDmacHandle->dmacTxHandle, &i2cDmacHandle->dmacTxConfig);
        DMAC_TransferStart(i2cDmacHandle->dmacTxHandle);
    }
}

/*!
 * brief Initializes the I2C master transfer in DMAC way.
 *
 * param base I2C peripheral base address
 * param handle Pointer to i2c_master_dmac_handle_t structure to store the transfer state.
 * param callback Pointer to the user callback function
 * param userData User configurable pointer to any data, function, structure etc that user wish to use in the callback
 * param dmacTxHandle DMAC handle pointer for tx
 * param dmacRxHandle DMAC handle pointer for rx
 */
void I2C_MasterTransferCreateHandleDMAC(I2C_Type *base,
                                        i2c_master_dmac_handle_t *handle,
                                        i2c_master_dmac_transfer_callback_t callback,
                                        void *userData,
                                        dmac_handle_t *dmacTxHandle,
                                        dmac_handle_t *dmacRxHandle)
{
    assert(handle != NULL);
    assert(dmacTxHandle != NULL);
    assert(dmacRxHandle != NULL);

    uint32_t instance = I2C_GetInstance(base);

    /* Zero handle. */
    (void)memset(handle, 0, sizeof(*handle));

    /* Set the user callback and userData. */
    handle->completionCallback = callback;
    handle->userData           = userData;

    /* Set the DMAC handle and I2C base. */
    handle->dmacTxHandle = dmacTxHandle;
    handle->dmacRxHandle = dmacRxHandle;
    handle->base         = base;

    /* Set the state to idle. */
    handle->state = (uint8_t)kMasterIdleState;

    /* Why using IRQ handler in DMAC transfer is that once master is nacked by slave or loses arbitration the transfer
       should abort, however DMAC still keeps writing/reading which will push new data to tx FIFO and initiate new
       transfer automatically, so we need to open interrupt to monitor event that can causes transfer abort. */

    /* Save the handle into static handle array */
    s_i2cHandles[instance] = handle;
    /* Save IRQ handler into static ISR function pointer. */
    s_i2cMasterIsr = I2C_MasterDmacTransferHandleIRQ;
    /* Disable internal IRQs and enable global IRQ. */
    I2C_DisableInterrupts(base, (uint16_t)kI2C_AllInterruptEnable);
    (void)EnableIRQ(s_I2cIrqs[instance]);
}

/*!
 * brief Initiates a master transfer on the I2C bus in DMAC way.
 *
 * note Transfer in DMAC way is non-blocking which means this API returns immediately after transfer initiates.
 * If user installs a user callback when calling @ref I2C_MasterTransferCreateHandleDMAC before, the callback will be
 * invoked when transfer finishes.
 *
 * param base I2C base pointer
 * param handle pointer to i2c_master_transfer_handle_t structure which stores the transfer state.
 * param xfer Pointer to the transfer configuration structure.
 * retval kStatus_Success Successfully start the data transmission.
 * retval #kStatus_I2C_Busy Previous transmission still not finished.
 */
status_t I2C_MasterTransferDMAC(I2C_Type *base, i2c_master_dmac_handle_t *handle, i2c_master_transfer_t *xfer)
{
    assert(handle != NULL);
    assert(xfer != NULL);

    /* Return busy if another transaction is in progress. */
    if ((handle->state != (uint8_t)kMasterIdleState) ||
        ((I2C_MasterGetStatusFlags(base) & (uint32_t)kI2C_BusBusyInterruptFlag) != 0U))
    {
        return kStatus_I2C_Busy;
    }

    /* Set the state to busy. */
    handle->state = (uint8_t)kMasterBusy;

    /* Enable module */
    I2C_Enable(base, true);

    /* Set target slave address */
    if (xfer->slaveAddress == 0U)
    {
        I2C_MasterEnableGeneralCall(base, true);
    }
    else
    {
        I2C_MasterEnableGeneralCall(base, false);
        I2C_MasterSetTargetAddress(base, xfer->slaveAddress);
    }

    /* Empty rx FIFO first */
    while ((base->IC_STATUS & I2C_IC_STATUS_RFNE_MASK) != 0U)
    {
        (void)I2C_ReadByte(base);
    }

    /* Clear all flags. */
    I2C_MasterClearStatusFlags(base, (uint32_t)kI2C_MasterAllClearFlags);
    /* Enable I2C internal IRQ sources */
    I2C_EnableInterrupts(base, (uint16_t)kI2C_TxAbortInterruptEnable);

    if (xfer->direction == kI2C_Write)
    {
        DMAC_GetChannelDefaultTransferConfig(&handle->dmacTxConfig, (void *)handle->txData,
                                             (uint32_t *)I2C_GetDataRegAddr(base), kDMAC_ChannelTransferWidth16Bits,
                                             kDMAC_ChannelBurstLength1DataWidth, xfer->dataSize * 2U,
                                             kDMAC_ChannelTransferMemoryToPeripheral);
        handle->data         = xfer->data;
        handle->transferSize = xfer->dataSize;
        if (xfer->dataSize > I2C_TX_LENGTH)
        {
            handle->dataSize                        = xfer->dataSize - I2C_TX_LENGTH;
            handle->dmacTxConfig.transferTotalBytes = I2C_TX_LENGTH * 2U;
        }
        else
        {
            handle->dataSize                        = 0U;
            handle->dmacTxConfig.transferTotalBytes = xfer->dataSize * 2U;
        }

        for (uint32_t i = 0U; i < handle->dmacTxConfig.transferTotalBytes / 2U; i++)
        {
            handle->txData[i] = (uint16_t)xfer->data[i];
        }
        handle->dmacTxConfig.interruptMask = (uint32_t)kDMAC_ChannelInterruptFlagTransferComplete;
        DMAC_TransferSetCallback(handle->dmacTxHandle, I2C_MasterTransferCallbackDMA, handle);
        (void)DMAC_TransferSubmit(handle->dmacTxHandle, &handle->dmacTxConfig);
    }
    else
    {
        handle->dataSize = 0U;
        dmac_channel_transfer_config_t dmacConfig;

        DMAC_GetChannelDefaultTransferConfig(&dmacConfig, (uint32_t *)I2C_GetDataRegAddr(base), (void *)xfer->data,
                                             kDMAC_ChannelTransferWidth8Bits, kDMAC_ChannelBurstLength1DataWidth,
                                             xfer->dataSize, kDMAC_ChannelTransferPeripheralToMemory);
        dmacConfig.interruptMask = (uint32_t)kDMAC_ChannelInterruptFlagTransferComplete;
        DMAC_TransferSetCallback(handle->dmacRxHandle, I2C_MasterTransferCallbackDMA, handle);
        (void)DMAC_TransferSubmit(handle->dmacRxHandle, &dmacConfig);

        DMAC_GetChannelDefaultTransferConfig(&dmacConfig, &g_TxDummy, (uint32_t *)I2C_GetDataRegAddr(base),
                                             kDMAC_ChannelTransferWidth32Bits, kDMAC_ChannelBurstLength1DataWidth,
                                             xfer->dataSize * sizeof(g_TxDummy),
                                             kDMAC_ChannelTransferMemoryToPeripheral);
        dmacConfig.srcAddrIncrementType = kDMAC_ChannelAddressFix;
        (void)DMAC_TransferSubmit(handle->dmacTxHandle, &dmacConfig);
    }

    while (xfer->subaddressSize != 0U)
    {
        xfer->subaddressSize--;
        I2C_WriteByte(base, (uint8_t)((xfer->subaddress >> (8U * xfer->subaddressSize)) & 0xffU));
    }

    if (xfer->direction == kI2C_Write)
    {
        DMAC_TransferStart(handle->dmacTxHandle);
        I2C_EnableDMA(base, (uint8_t)kQSPI_DmaTx);
    }
    else
    {
        DMAC_TransferStart(handle->dmacRxHandle);
        DMAC_TransferStart(handle->dmacTxHandle);
        I2C_EnableDMA(base, (uint8_t)kQSPI_DmaTx | (uint8_t)kQSPI_DmaRx);
    }

    return kStatus_Success;
}

/*!
 * brief Aborts a master DMAC non-blocking transfer early.
 *
 * param base I2C peripheral base address
 * param handle A pointer to the i2c_master_dmac_handle_t structure.
 */
void I2C_MasterTransferAbortDMAC(I2C_Type *base, i2c_master_dmac_handle_t *handle)
{
    DMAC_TransferStop(handle->dmacTxHandle);
    DMAC_TransferStop(handle->dmacRxHandle);

    /* Disable dmac transfer. */
    I2C_DisableDMA(base, (uint8_t)kQSPI_DmaTx | (uint8_t)kQSPI_DmaRx);
    /* Reset the state to idle. */
    handle->state = (uint8_t)kMasterIdleState;
}

static void I2C_MasterDmacTransferHandleIRQ(I2C_Type *base, void *i2cHandle)
{
    assert(i2cHandle != NULL);
    status_t result                  = kStatus_Success;
    i2c_master_dmac_handle_t *handle = (i2c_master_dmac_handle_t *)i2cHandle;
    uint32_t status                  = I2C_MasterGetStatusFlags(base);

    I2C_MasterTransferAbortDMAC(base, handle);
    /* Disable I2C tx abort interrupt. */
    I2C_DisableInterrupts(base, (uint16_t)kI2C_TxAbortInterruptEnable);

    if ((status & (uint32_t)kI2C_StopDetectInterruptFlag) != 0U)
    {
        I2C_DisableInterrupts(base, (uint16_t)kI2C_StopDetectInterruptEnable);
    }

    if ((status & (uint32_t)kI2C_Master7bitAddressNackFlag) != 0U)
    {
        result = kStatus_I2C_Addr_Nak;
    }

    if ((status & (uint32_t)kI2C_MasterDataNackFlag) != 0U)
    {
        result = kStatus_I2C_Nak;
    }

    if ((status & (uint32_t)kI2C_MasterGeneralCallNackFlag) != 0U)
    {
        result = kStatus_I2C_GeneralCall_Nak;
    }

    if ((status & (uint32_t)kI2C_MasterArbitrationLostFlag) != 0U)
    {
        result = kStatus_I2C_ArbitrationLost;
    }

    if (result != kStatus_Success)
    {
        /* Stop detect occurs slightly later(one clock cycle) than error status, wait for stop detect to set. */
        while ((I2C_MasterGetStatusFlags(base) & (uint32_t)kI2C_StopDetectInterruptFlag) == 0U)
        {
        }
    }

    /* Clear all status flags. */
    I2C_MasterClearStatusFlags(base, (uint32_t)kI2C_MasterAllClearFlags);
    /* Reset the state to idle. */
    handle->state = (uint8_t)kMasterIdleState;

    if (handle->completionCallback != NULL)
    {
        handle->completionCallback(base, handle, result, handle->userData);
    }
}
