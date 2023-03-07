/*
 * Copyright 2020,2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_I2C_H_
#define _FSL_I2C_H_

#include "fsl_common.h"

/*!
 * @addtogroup i2c_driver
 * @ingroup i2c
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
#define FSL_I2C_DRIVER_VERSION (MAKE_VERSION(2, 0, 2))
/*@}*/

/*! @brief Retry times when checking status flags. */
#ifndef I2C_RETRY_TIMES
#define I2C_RETRY_TIMES 0U /* Default defines to zero, driver keeps checking status flag untill it changes. */
#endif

/*!
 * @brief I2C API status codes, used by bus operation APIs and transactional APIs as return value to indicate the bus's
 * current status as the API's execution result, or used in the callback to indicate transfer results.
 */
enum
{
    kStatus_I2C_Busy = MAKE_STATUS(kStatusGroup_I2C, 0), /*!< I2C bus is busy. */
    kStatus_I2C_Idle = MAKE_STATUS(kStatusGroup_I2C, 1), /*!< I2C Bus is idle. */
    kStatus_I2C_Nak  = MAKE_STATUS(kStatusGroup_I2C, 2), /*!< I2C detected NACK on bus. When in SMBus mode, this means
                                                            the receiver nacks transmitter before PEC byte. */
    kStatus_I2C_ArbitrationLost = MAKE_STATUS(kStatusGroup_I2C, 3), /*!< I2C lost arbitration during addressing. */
    kStatus_I2C_Timeout =
        MAKE_STATUS(kStatusGroup_I2C, 4), /*!< Timeout happens when waiting for status flags to change. */
    kStatus_I2C_Addr_Nak = MAKE_STATUS(kStatusGroup_I2C, 5), /*!< NACK was detected during the address probe. */
    kStatus_I2C_GeneralCall_Nak =
        MAKE_STATUS(kStatusGroup_I2C, 6), /*!< NACK was detected after issuing general call. */
    kStatus_I2C_RxHardwareOverrun = MAKE_STATUS(kStatusGroup_I2C, 7), /*!< I2C rx receiver overrun. */
    kStatus_I2C_TxFifoFlushed     = MAKE_STATUS(kStatusGroup_I2C, 8), /*!< I2C tx FIFO is flushed by hardware. */
};

/*!
 * @brief I2C common hardware status flags
 *
 * These enumerations can be ORed together to form bit masks. The masks can be used together with masks in
 * #_i2c_master_status_flags as parameter by @ref I2C_MasterClearStatusFlags or as return value by
 * @ref I2C_MasterGetStatusFlags. Or together with masks in #_i2c_slave_status_flags as parameter by
 * @ref I2C_SlaveClearStatusFlags or as return value by @ref I2C_SlaveGetStatusFlags
 */
enum _i2c_status_flags
{
    kI2C_RxUnderflowInterruptFlag =
        I2C_IC_RAW_INTR_STAT_RX_UNDER_MASK, /*!< Sets when software tries to read from empty rx FIFO. bit 0 */
    kI2C_RxOverflowInterruptFlag =
        I2C_IC_RAW_INTR_STAT_RX_OVER_MASK, /*!< Sets when new data received and the rx FIFO is already full. bit 1 */
    kI2C_RxDataReadyInterruptFlag =
        I2C_IC_RAW_INTR_STAT_RX_FULL_MASK, /*!< Sets when data in rx FIFO reaches or is above rx watermark. bit 2 */
    kI2C_TxOverflowInterruptFlag = I2C_IC_RAW_INTR_STAT_TX_OVER_MASK, /*!< Sets when software tries to push data to tx
                                                                         FIFO and it's already full. bit 3 */
    kI2C_TxRequestInterruptFlag =
        I2C_IC_RAW_INTR_STAT_TX_EMPTY_MASK, /*!< Sets when data in tx FIFO is at or below tx FIFO watermark. bit 4 */
    kI2C_TxAbortInterruptFlag = I2C_IC_RAW_INTR_STAT_TX_ABRT_MASK, /*!< Sets when any of the below status from
     kI2C_Master7bitAddressNackFlag to kI2C_MasterArbitrationLostFlag is set, results in the tx FIFO being flushed.
     Clear this flag will also clear these status flags automatically. bit 6 */
    kI2C_BusBusyInterruptFlag =
        I2C_IC_RAW_INTR_STAT_ACTIVITY_MASK, /*!< Sets when there is I2C activity on bus. bit 8 */
    kI2C_StopDetectInterruptFlag =
        I2C_IC_RAW_INTR_STAT_STOP_DET_MASK, /*!< Sets when stop condition is detected. bit 9 */
    kI2C_StartDetectInterruptFlag =
        I2C_IC_RAW_INTR_STAT_START_DET_MASK, /*!< Sets when start condition is detected. bit 10 */
    kI2C_ActiveFlag = I2C_IC_STATUS_ACTIVITY_MASK
                      << 25U, /*!< Sets when I2C module is in active transfer state. bit 25 */
    kI2C_TxFifoNotFullFlag  = I2C_IC_STATUS_TFNF_MASK << 25U, /*!< Sets when I2C tx FIFO is not full. bit 26 */
    kI2C_TxFifoEmptyFlag    = I2C_IC_STATUS_TFE_MASK << 25U,  /*!< Sets when I2C txFIFO is completely empty. bit 27 */
    kI2C_RxFifoNotEmptyFlag = I2C_IC_STATUS_RFNE_MASK << 25U, /*!< Sets when I2C rx FIFO is not empty. bit 28 */
    kI2C_RxFifoFullFlag     = I2C_IC_STATUS_RFF_MASK << 25U,  /*!< Sets when I2C rx FIFO is completely full. bit 29 */
};

/*!
 * @brief I2C master hardware status flags
 *
 * These enumerations can be ORed together to form bit masks. The masks can be used as parameter by
 * @ref I2C_MasterClearStatusFlags or as return value by @ref I2C_MasterGetStatusFlags.
 */
enum _i2c_master_status_flags
{
    /* These flags are cleared when kI2C_TxAbortInterruptFlag is cleared */
    kI2C_Master7bitAddressNackFlag = I2C_IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK_MASK
                                     << 12U, /*!< Sets when I2C master is nacked in 7-bit addressing. bit 12 */
    kI2C_Master10bitAddressNack1Flag =
        I2C_IC_TX_ABRT_SOURCE_ABRT_10ADDR1_NOACK_MASK
        << 12U, /*!< Sets when I2C master is nacked of first byte of address in 10-bit addressing. bit 13 */
    kI2C_Master10bitAddressNack2Flag =
        I2C_IC_TX_ABRT_SOURCE_ABRT_10ADDR2_NOACK_MASK
        << 12U, /*!< Sets when I2C master is nacked of second byte of address in 10-bit addressing. bit 14 */
    kI2C_MasterDataNackFlag = I2C_IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK_MASK
                              << 12U, /*!< Sets when I2C master is nacked in data sending. bit 15 */
    kI2C_MasterGeneralCallNackFlag = I2C_IC_TX_ABRT_SOURCE_ABRT_GCALL_NOACK_MASK
                                     << 12U, /*!< Sets when I2C master is nacked after issueing general call. bit 16 */
    kI2C_MasterGeneralCallReadFlag =
        I2C_IC_TX_ABRT_SOURCE_ABRT_GCALL_READ_MASK
        << 12U, /*!< Sets when I2C master uses read command after issueing general call. bit 17 */
    kI2C_MasterHighSpeedCodeAckFlag = I2C_IC_TX_ABRT_SOURCE_ABRT_HS_ACKDET_MASK
                                      << 12U, /*!< Sets when I2C master is in high speed mode and the high speed master
                                                 code was acknowledged. bit 18 */
    kI2C_MasterStartByteAckFlag =
        I2C_IC_TX_ABRT_SOURCE_ABRT_SBYTE_ACKDET_MASK
        << 12U, /*!< Sets when I2C master has sent a START byte and the START byte was acknowledged. bit 19 */
    kI2C_MasterHighSpeedRestartDisabledFlag =
        I2C_IC_TX_ABRT_SOURCE_ABRT_HS_NORSTRT_MASK
        << 12U, /*!< Sets when I2C master tries to transfer data in high speed mode with restart disabled. bit 20 */
    kI2C_MasterStartWithRestartDisabledFlag =
        I2C_IC_TX_ABRT_SOURCE_ABRT_SBYTE_NORSTRT_MASK
        << 12U, /*!< Sets when master tries to send a start with restart disabled. bit 21 */
    kI2C_Master10bitAddressReadRestartDisabledFlag =
        I2C_IC_TX_ABRT_SOURCE_ABRT_10B_RD_NORSTRT_MASK
        << 12U, /*!< Sets when I2C master issues read command in 10-bit addessing with restart disabled. bit 22 */
    kI2C_MasterOperateWhenDisabledFlag =
        I2C_IC_TX_ABRT_SOURCE_ABRT_MASTER_DIS_MASK
        << 12U, /*!< Sets when software wants to initiate a master operation with master mode disabled. bit 23 */
    kI2C_MasterArbitrationLostFlag =
        I2C_IC_TX_ABRT_SOURCE_ARB_LOST_MASK
        << 12U, /*!< Sets when master loses arbitration or kI2C_SlaveArbitrationLostFlag is set. bit 24 */

    kI2C_MasterActiveFlag = I2C_IC_STATUS_MST_ACTIVITY_MASK
                            << 25U, /*!< Sets when I2C master operation is active. bit 30 */

    /*! These flags are cleared automatically according to module status, or when other status bit is cleared.
        eg. once tx FIFO is not empty kI2C_TxFifoEmptyFlag is cleared automatically.
        eg. status bits from kI2C_Master7bitAddressNackFlag to kI2C_MasterArbitrationLostFlag are cleared when
       kI2C_TxAbortInterruptFlag is cleared. */
    kI2C_MasterAutoClearFlags =
        kI2C_RxDataReadyInterruptFlag | kI2C_TxRequestInterruptFlag | kI2C_ActiveFlag | kI2C_TxFifoNotFullFlag |
        kI2C_TxFifoEmptyFlag | kI2C_RxFifoNotEmptyFlag | kI2C_RxFifoFullFlag | kI2C_MasterActiveFlag |
        kI2C_Master7bitAddressNackFlag | kI2C_Master10bitAddressNack1Flag | kI2C_Master10bitAddressNack2Flag |
        kI2C_MasterDataNackFlag | kI2C_MasterGeneralCallNackFlag | kI2C_MasterGeneralCallReadFlag |
        kI2C_MasterHighSpeedCodeAckFlag | kI2C_MasterStartByteAckFlag | kI2C_MasterHighSpeedRestartDisabledFlag |
        kI2C_MasterStartWithRestartDisabledFlag | kI2C_Master10bitAddressReadRestartDisabledFlag |
        kI2C_MasterOperateWhenDisabledFlag | kI2C_MasterArbitrationLostFlag,

    /*! All flags which are clearable. */
    kI2C_MasterAllClearFlags = kI2C_RxUnderflowInterruptFlag | kI2C_RxOverflowInterruptFlag |
                               kI2C_TxOverflowInterruptFlag | kI2C_TxAbortInterruptFlag | kI2C_BusBusyInterruptFlag |
                               kI2C_StopDetectInterruptFlag | kI2C_StartDetectInterruptFlag,

    /* All error flags */
    kI2C_MasterErrorFlags = kI2C_RxUnderflowInterruptFlag | kI2C_RxOverflowInterruptFlag |
                            kI2C_TxOverflowInterruptFlag | kI2C_TxAbortInterruptFlag | kI2C_Master7bitAddressNackFlag |
                            kI2C_Master10bitAddressNack1Flag | kI2C_Master10bitAddressNack2Flag |
                            kI2C_MasterDataNackFlag | kI2C_MasterGeneralCallNackFlag | kI2C_MasterGeneralCallReadFlag |
                            kI2C_MasterHighSpeedRestartDisabledFlag | kI2C_MasterStartWithRestartDisabledFlag |
                            kI2C_Master10bitAddressReadRestartDisabledFlag | kI2C_MasterOperateWhenDisabledFlag |
                            kI2C_MasterArbitrationLostFlag,
};

/*!
 * @brief I2C slave hardware status flags
 *
 * These enumerations can be ORed together to form bit masks. The masks can be used as parameter by
 * @ref I2C_SlaveClearStatusFlags, or as return value by @ref I2C_SlaveGetStatusFlags.
 */
enum _i2c_slave_status_flags
{
    kI2C_SlaveReadRequestInterruptFlag =
        I2C_IC_RAW_INTR_STAT_RD_REQ_MASK, /*!< Sets when I2C as slave and master tries to read data from it. bit 5 */
    kI2C_SlaveNackInterruptFlag =
        I2C_IC_RAW_INTR_STAT_RX_DONE_MASK, /*!< Sets when I2C as slave transmiter and was nacked by master, indicating
                                              tx complete. bit 7 */
    kI2C_SlaveGeneralCallAckInterruptFlag =
        I2C_IC_RAW_INTR_STAT_GEN_CALL_MASK, /*!< Sets when general call address is received and acknowledged. bit 11 */

    kI2C_SlaveFlushTxFifoFlag =
        I2C_IC_TX_ABRT_SOURCE_ABRT_SLVFLUSH_TXFIFO_MASK, /*!< Sets when I2C slave receives a read command but there are
                                                            still old data remaining in tx FIFO. Cleared when
                                                            kI2C_TxAbortInterruptFlag is cleared. bit 13 */
    kI2C_SlaveArbitrationLostFlag =
        I2C_IC_TX_ABRT_SOURCE_ABRT_SLV_ARBLOST_MASK, /*!< Sets when data on bus is not the same as slave sends. Cleared
                                                        when kI2C_TxAbortInterruptFlag is cleared. bit 14 */
    kI2C_SlaveReadWhenRequestedDataFlag =
        I2C_IC_TX_ABRT_SOURCE_ABRT_SLVRD_INTX_MASK, /*!< Sets when I2C slave is request data from master but use read
                                                       command. Cleared when kI2C_TxAbortInterruptFlag is cleared. bit
                                                       15 */
    kI2C_SlaveDisabledWhileBusy = I2C_IC_ENABLE_STATUS_SLV_DISABLED_WHILE_BUSY_MASK << 16U, /*!< Sets when I2C slave is
     busy while disabled. If slave is addressed when disabled, slave will nack the master even address matched, if slave
     is receiveing slave will nack the transfer immediately. Cleared when module is re-eanbled. bit 16 */
    kI2C_SlaveRxDataLost = I2C_IC_ENABLE_STATUS_SLV_RX_DATA_LOST_MASK
                           << 16U, /*!< Sets when I2C slave is actively
receiving data when disabled, and at least one byte have been received. Cleared when module is re-eanbled. bit 17 */

    kI2C_SlaveActiveFlag = I2C_IC_STATUS_SLV_ACTIVITY_MASK
                           << 25U, /*!< Sets when I2C slave operation is active. bit 31 */

    /*! These flags are cleared automatically according to module status, or when other status bit is cleared.
        eg. once tx FIFO is not empty kI2C_TxFifoEmptyFlag is cleared automatically.
        eg. kI2C_SlaveFlushTxFifoFlag, kI2C_SlaveArbitrationLostFlag and kI2C_SlaveReadWhenRequestedDataFlag are cleared
       when kI2C_TxAbortInterruptFlag is cleared. */
    kI2C_SlaveAutoClearFlags = kI2C_RxDataReadyInterruptFlag | kI2C_TxRequestInterruptFlag | kI2C_SlaveFlushTxFifoFlag |
                               kI2C_SlaveArbitrationLostFlag | kI2C_SlaveReadWhenRequestedDataFlag | kI2C_ActiveFlag |
                               kI2C_TxFifoNotFullFlag | kI2C_TxFifoEmptyFlag | kI2C_RxFifoNotEmptyFlag |
                               kI2C_RxFifoFullFlag | kI2C_SlaveActiveFlag | kI2C_SlaveDisabledWhileBusy |
                               kI2C_SlaveRxDataLost,

    /*! All flags which are clearable. */
    kI2C_SlaveAllClearFlags = kI2C_RxUnderflowInterruptFlag | kI2C_RxOverflowInterruptFlag |
                              kI2C_TxOverflowInterruptFlag | kI2C_SlaveReadRequestInterruptFlag |
                              kI2C_TxAbortInterruptFlag | kI2C_SlaveNackInterruptFlag | kI2C_BusBusyInterruptFlag |
                              kI2C_StopDetectInterruptFlag | kI2C_StartDetectInterruptFlag |
                              kI2C_SlaveGeneralCallAckInterruptFlag,

    /* All error flags */
    kI2C_SlaveErrorFlags = kI2C_RxUnderflowInterruptFlag | kI2C_RxOverflowInterruptFlag | kI2C_TxOverflowInterruptFlag |
                           kI2C_TxAbortInterruptFlag | kI2C_SlaveNackInterruptFlag | kI2C_SlaveFlushTxFifoFlag |
                           kI2C_SlaveArbitrationLostFlag | kI2C_SlaveReadWhenRequestedDataFlag |
                           kI2C_SlaveDisabledWhileBusy | kI2C_SlaveRxDataLost,
};

/*!
 * @brief I2C interrupt enable/disable source.
 *
 * These enumerations can be ORed together to form bit masks. The masks can be used as parameter by
 * @ref I2C_EnableInterrupts, @ref I2C_DisableInterrupts, or as return value by @ref I2C_GetEnabledInterrupts.
 */
enum _i2c_interrupt_enable
{
    /* Common */
    kI2C_RxUnderflowInterruptEnable = I2C_IC_INTR_MASK_M_RX_UNDER_MASK, /*!< Rx FIFO underflow interrupt. bit 0 */
    kI2C_RxOverflowInterruptEnable  = I2C_IC_INTR_MASK_M_RX_OVER_MASK,  /*!< Rx FIFO overflow interrupt. bit 1 */
    kI2C_RxDataReadyInterruptEnable = I2C_IC_INTR_MASK_M_RX_FULL_MASK,  /*!< Rx data ready interrupt. bit 2 */
    kI2C_TxOverflowInterruptEnable  = I2C_IC_INTR_MASK_M_TX_OVER_MASK,  /*!< Tx FIFO overflow interrupt. bit 3 */
    kI2C_TxRequestInterruptEnable   = I2C_IC_INTR_MASK_M_TX_EMPTY_MASK, /*!< Tx data request interrupt. bit 4 */
    kI2C_SlaveReadRequestInterruptEnable =
        I2C_IC_INTR_MASK_M_RD_REQ_MASK, /*!< I2C slave requested data by master interrupt. bit 5 */
    kI2C_TxAbortInterruptEnable   = I2C_IC_INTR_MASK_M_TX_ABRT_MASK,  /*!< Tx abort interrupt. bit 6 */
    kI2C_SlaveNackInterruptEnable = I2C_IC_INTR_MASK_M_RX_DONE_MASK,  /*!< I2C slave nack interrupt. bit 7 */
    kI2C_BusBusyInterruptEnable   = I2C_IC_INTR_MASK_M_ACTIVITY_MASK, /*!< I2C bus busy interrupt. bit 8 */
    kI2C_StopDetectInterruptEnable =
        I2C_IC_INTR_MASK_M_STOP_DET_MASK, /*!< I2C stop condition detect interrupt. bit 9 */
    kI2C_StartDetectInterruptEnable =
        I2C_IC_INTR_MASK_M_START_DET_MASK, /*!< I2C start condition detect interrupt. bit 10 */
    kI2C_SlaveGeneralCallAckInterruptEnable =
        I2C_IC_INTR_MASK_M_GEN_CALL_MASK, /*!< I2C ganaral call detect and acknowledge interrupt. bit 11 */

    kI2C_SlaveInterruptEnable = kI2C_RxOverflowInterruptEnable | kI2C_RxDataReadyInterruptEnable |
                                kI2C_SlaveReadRequestInterruptEnable | kI2C_TxAbortInterruptEnable |
                                kI2C_SlaveNackInterruptEnable | kI2C_StopDetectInterruptEnable |
                                kI2C_StartDetectInterruptEnable | kI2C_SlaveGeneralCallAckInterruptEnable,

    kI2C_AllInterruptEnable =
        kI2C_RxUnderflowInterruptEnable | kI2C_RxOverflowInterruptEnable | kI2C_RxDataReadyInterruptEnable |
        kI2C_TxOverflowInterruptEnable | kI2C_TxRequestInterruptEnable | kI2C_TxAbortInterruptEnable |
        kI2C_BusBusyInterruptEnable | kI2C_StopDetectInterruptEnable | kI2C_StartDetectInterruptEnable |
        kI2C_SlaveReadRequestInterruptEnable | kI2C_SlaveNackInterruptEnable | kI2C_SlaveGeneralCallAckInterruptEnable,
};

/*! @brief Master speed mode. */
typedef enum _i2c_master_speed_mode
{
    kI2C_MasterSpeedStandard = 0x1U, /*!< Standard mode, 100kbps. */
    kI2C_MasterSpeedFast     = 0X2U, /*!< Fast mode, 400kbps. */
    kI2C_MasterSpeedHigh     = 0X3U, /*!< High speed mode, 3.4Mbps. */
} i2c_master_speed_mode_t;

/*! @brief Addressing mode, normal 7-bit address or 10-bit address. */
typedef enum _i2c_address_mode
{
    kI2C_AddressMatch7bit  = 0x0U, /*!< 7-bit addressing mode. */
    kI2C_AddressMatch10bit = 0X1U, /*!< 10-bit addressing mode. */
} i2c_address_mode_t;

/*! @brief I2C DMA configuration for Transmit and Receive.*/
enum _i2c_dma_enable_flags
{
    kQSPI_DmaRx = 0x1U, /*!< Receive DMA Enable Flag. */
    kQSPI_DmaTx = 0x2U, /*!< Transmit DMA Enable Flag. */
};

/*!
 * @brief I2C master configuration structure.
 *
 * This structure includes all the master operation needed features, user can configure these features one by one
 * manually, or call @ref I2C_MasterGetDefaultConfig to set the structure to default value. Then, call
 * @ref I2C_MasterInit to initialize I2C module. After initialization, the I2C module can only operate as master. To
 * deinitialize I2C, call @ref I2C_MasterDeinit.
 */
typedef struct _i2c_master_config
{
    bool enable; /*!< Enable the I2C peripheral as master during initialization. I2C_IC_CON[IC_SLAVE_DISABLE],
                    I2C_IC_CON[master_mode], I2C_IC_ENABLE[ENABLE] */
    i2c_master_speed_mode_t speed;      /*!< The speed I2C master operates. I2C_IC_CON[SPEED] */
    i2c_address_mode_t addressingMode;  /*!< Addressing mode. I2C_IC_TAR[IC_10BITADDR_MASTER] */
    uint8_t highSpeedMasterCodeAddress; /*!< Master code address in high speed mode, only 0-7 is avaliable.
                                           I2C_IC_HS_MADDR[IC_HS_MAR] */
    uint8_t rxFifoWaterMark; /*!< Rx FIFO watermark that controls the rx interrupt trigger level. I2C_IC_RX_TL[RX_TL] */
    uint8_t txFifoWaterMark; /*!< Tx FIFO watermark that controls the tx interrupt trigger level. I2C_IC_TX_TL[TX_TL] */
    uint16_t dataHoldTime;   /*!< The number of clock source cycles of the data hold time tHD:DAT.
                                I2C_IC_SDA_HOLD[IC_SDA_HOLD] */
    uint16_t dataSetupTime;  /*!< The number of clock source cycles of the data setup time tSU:DAT.
                                I2C_IC_SDA_SETUP[SDA_SETUP] */
    uint8_t glitchFilter;    /*!< The number of clock source cycles of the longest glitch to be filtered in standard and
                                fast mode. I2C_IC_FS_SPKLEN[IC_FS_SPKLEN] */
    uint8_t glitchFilterHighSpeed; /*!< The number of clock source cycles of the longest glitch to be filtered in high
                                      speed mode. I2C_IC_HS_SPKLEN[IC_HS_SPKLEN] */
} i2c_master_config_t;

/*!
 * @brief I2C slave configuration structure.
 *
 * This structure includes all the slave operation needed features, user can configure these features one by one
 * manually, or call @ref I2C_SlaveGetDefaultConfig to set the structure to default value. Then, call
 * @ref I2C_SlaveInit to initialize I2C module. After initialization, the I2C module can only operate as slave. To
 * deinitialize I2C, call @ref I2C_SlaveDeinit.
 */
typedef struct _i2c_slave_config
{
    bool enable; /*!< Enable the I2C peripheral as slave during initialization. I2C_IC_CON[IC_SLAVE_DISABLE],
                    I2C_IC_CON[master_mode], I2C_IC_ENABLE[ENABLE] */
    i2c_address_mode_t addressingMode; /*!< Addressing mode. I2C_IC_CON[IC_10BITADDR_SLAVE] */
    uint16_t slaveAddress;             /*!< Slave address. I2C_IC_SAR[IC_SAR] */
    uint8_t rxFifoWaterMark; /*!< Rx FIFO watermark that controls the rx interrupt trigger level. I2C_IC_RX_TL[RX_TL] */
    uint8_t txFifoWaterMark; /*!< Tx FIFO watermark that controls the tx interrupt trigger level. I2C_IC_TX_TL[TX_TL] */
    uint16_t dataHoldTime;   /*!< The number of clock source cycles of the data hold time tHD:DAT.
                                I2C_IC_SDA_HOLD[IC_SDA_HOLD] */
    uint16_t dataSetupTime;  /*!< The number of clock source cycles of the data setup time tSU:DAT.
                                I2C_IC_SDA_SETUP[SDA_SETUP] */
    bool ackGeneralCall; /*!< True to let slave response general call with Ack. I2C_IC_ACK_GENERAL_CALL[ACK_GEN_CALL] */
    uint8_t glitchFilter; /*!< The number of clock source cycles of the longest glitch to be filtered in standard and
                             fast mode. I2C_IC_FS_SPKLEN[IC_FS_SPKLEN] */
    uint8_t glitchFilterHighSpeed; /*!< The number of clock source cycles of the longest glitch to be filtered in high
                                      speed mode. I2C_IC_HS_SPKLEN[IC_HS_SPKLEN] */
} i2c_slave_config_t;

/*! @brief Master transfer direction. */
typedef enum _i2c_direction
{
    kI2C_Write = 0U, /*!< Master transmit. */
    kI2C_Read  = 1U  /*!< Master receive. */
} i2c_direction_t;

/*!
 * @brief I2C master transfer configuration structure.
 *
 * This structure definition includes all the user configurable features, that are used to control single I2C transfer
 * of master mode, in polling way or in interrupt way.
 */
typedef struct _i2c_master_transfer
{
    uint16_t slaveAddress;     /*!< 7-bit slave address, set to 0 to issue general call. */
    i2c_direction_t direction; /*!< Transfer direction, #kI2C_Write or #kI2C_Read. */
    uint32_t subaddress;       /*!< Pointer to command code. */
    size_t subaddressSize;     /*!< Size of the command code, max value 4. */
    uint8_t *volatile data;    /*!< Pointer to the send/receive data buffer. */
    volatile size_t dataSize;  /*!< Transfer size. */
} i2c_master_transfer_t;

/*! @brief Forward declaration of the I2C master transfer handle structure. */
typedef struct _i2c_master_handle i2c_master_handle_t;

/*!
 * @brief I2C master transfer callback function definition
 *
 * Defines the interface of user callback function used in master interrupt transfer. The callback function shall be
 * defined and declared in application level by user. Before starting master transfer by calling
 * @ref I2C_MasterTransferNonBlocking, call @ref I2C_MasterTransferCreateHandle to install the user callback. When
 * master transfer ends successfully or failed due to any event like arbitration lost or nacked by slave, user callback
 * will be invoked by driver. And then user can decide what to do next in the callback according to its third parameter
 * completionStatus that indicates how the transfer ends.
 *
 * @param base The I2C peripheral base address.
 * @param handle I2C transfer handle, which contains the information of base pointer, completionStatus and user data.
 * @param completionStatus Either kStatus_Success or an error code describing how the transfer completed.
 * @param userData Pointer to user configurable structure.
 */
typedef void (*i2c_master_transfer_callback_t)(I2C_Type *base,
                                               i2c_master_handle_t *handle,
                                               status_t completionStatus,
                                               void *userData);
/*!
 * @brief I2C master transfer handle
 *
 * @note If user wants to use the transactional API to transfer data in interrupt way in master mode, one I2C instance
 * should and can only be allocated one master handle.
 * @note The handle is maintained by I2C driver internally, which means the transfer state is retained and user shall
 * not modify its state #state in application level. If user only wish to use transactional APIs without understanding
 * its machanism, it is not necessary to understand these members.
 */
struct _i2c_master_handle
{
    uint8_t state;                                     /*!< A transfer state maintained during transfer. */
    i2c_master_transfer_t transfer;                    /*!< I2C master transfer structure. */
    size_t transmitSize;                               /*!< Total bytes to be transferred. */
    size_t receiveSize;                                /*!< Total bytes to be received. */
    i2c_master_transfer_callback_t completionCallback; /*!< Callback function invoked when the transfer is finished. */
    void *userData; /*!< User configurable pointer to any data, function, structure etc that user wish to use in the
                        callback */
};

/*!
 * @brief Set of slave transfer events.
 *
 * This enumeration lists all the protocol level events that may happen during slave transfer. They can be used for two
 * related purposes:
 *  1. User can select certain events and combined them by OR operation to form a mask, and use the mask to configure
 * slave transfer configuration stucture i2c_slave_transfer_t::u8EventMask. If any of these selected events happens,
 * driver will alert user by invoking callback.
 *  2. When slave callback is invoked, user has to know which specific event occured. Callback uses slave transfer
 * configuration structure i2c_slave_transfer_t as 2nd parameter, its member i2c_slave_transfer_t::eEvent shows which
 * event just happened.
 */
typedef enum _i2c_slave_transfer_event
{
    kI2C_SlaveStartDetectEvent = 0x01U, /*!< A start/repeated start was detected. */
    kI2C_SlaveGenaralcallEvent = 0x02U, /*!< Received the general call address after a start or repeated start. */
    kI2C_SlaveTransmitEvent    = 0x04U, /*!< Slave runs out of data to transmit, request a new data buffer. */
    kI2C_SlaveReceiveEvent = 0x08U, /*!< Slave runs out of space to store received data, request a new data buffer. */
    kI2C_SlaveCompletionEvent =
        0x10U, /*!< Slave detects a stop signal, or slave is nacked by master during master-receive,
                or slave has finished transmit/receive previously configured amount of data. */
    /*! A bit mask of all available events. */
    kI2C_SlaveAllEvents = kI2C_SlaveStartDetectEvent | kI2C_SlaveGenaralcallEvent | kI2C_SlaveTransmitEvent |
                          kI2C_SlaveReceiveEvent | kI2C_SlaveCompletionEvent,
} i2c_slave_transfer_event_t;

/*!
 * @brief I2C slave transfer configuration structure.
 *
 * Covers slave transfer data buffer pointer, data size and the events user want driver to alert.
 * @note Unlike master who controls the transfer flow, slave has to monitor any bus event and change its configuration
 * accordingly. So this slave transfer configuration structure is also used as second parameter of callback, for user
 * to change the transfer configuration in the callback. The read-only member eEvent shows which event occured that
 * causes the callback being invoked.
 */
typedef struct _i2c_slave_transfer
{
    uint8_t *volatile data;           /*!< Pointer to the buffer of data to send, or to store received data. */
    volatile uint16_t dataSize;       /*!< Transfer size. */
    i2c_slave_transfer_event_t event; /*!< The event that caused the callback being invoked. Read-only. */
    status_t completionStatus;        /*!< Success or error code describing how the transfer completed. Only applies for
                                         #kI2C_SlaveCompletionEvent. */
} i2c_slave_transfer_t;
/*!
 * @brief I2C slave transfer callback function definition
 *
 * Defines the interface of slave user callback function. The callback function shall be defined and declared in
 * application level by user. Before calling @ref I2C_SlaveTransferNonBlocking to let I2C slave ready to process bus
 * events, call @ref I2C_SlaveTransferCreateHandle first to install the user callback to slave handle. When I2C slave
 * meets user selected events, callback will be invoked and user can decide the following steps in the callback.
 * All the events that can trigger callback are listed in #i2c_slave_transfer_event_t.
 *
 * @param base Base address for the I2C instance on which the event occurred.
 * @param transfer Pointer to transfer descriptor containing values passed to and/or from the callback.
 * @param userData Pointer to user configurable structure.
 */
typedef void (*i2c_slave_transfer_callback_t)(I2C_Type *base, i2c_slave_transfer_t *transfer, void *userData);

/*!
 * @brief I2C slave transfer handle
 *
 * @note If user wants to use the transactional API to transfer data in slave mode, one I2C instance should and can only
 * be allocated one handle.
 * @note The handle is maintained by I2C driver internally, which means the transfer state is retained and user shall
 * not modify its state #isBusy in application level. If user only wish to use transactional APIs without understanding
 * its machanism, it is not necessary to understand these members.
 */
typedef struct _i2c_slave_handle
{
    i2c_slave_transfer_t transfer; /*!< I2C slave transfer structure. */
    size_t transferredCount;       /*!< The number of bytes actually transferred for curent data buffer. */
    bool isBusy;                   /*!< Whether transfer is busy. */
    uint8_t eventMask; /*!< Mask of the events. When these enents occur during transfer driver will alert user using
                            callback. */
    i2c_slave_transfer_callback_t callback; /*!< Callback function invoked at the transfer event. */
    status_t completionStatus; /*!< I2C slave transfer complete status, indicating how the transfer ends, such as
    #kStatus_I2C_Nak indicates the slave was nacked by master before all the data was sent. This parameter is only
    useful when eEvent is #kI2C_SlaveCompletionEvent. */
    void *userData; /*!< User configurable pointer to any data, function, structure etc that user wish to use in the
                        callback. */
} i2c_slave_handle_t;
/* --------------------------------------------- Slave Transfer Sub-group ------------------------------------------- */

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! brief Typedef for interrupt handler. */
typedef void (*i2c_isr_t)(I2C_Type *base, void *i2cHandle);

/*! @brief Pointers to i2c IRQ number for each instance. */
extern const IRQn_Type s_I2cIrqs[];

/*! @brief Pointers to i2c handles for each instance. */
extern void *s_i2cHandles[];

/*! @brief Pointer to master IRQ handler for each instance. */
extern i2c_isr_t s_i2cMasterIsr;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus. */

/*!
 * @name Hardware Status Flags Sub-group
 * @{
 */
/*!
 * @brief Gets the I2C master hardware status flags.
 *
 * @param base I2C base pointer
 * @return the mask of status flags, can be a single flag or several flags in #_i2c_master_status_flags ORed together
 */
static inline uint32_t I2C_MasterGetStatusFlags(I2C_Type *base)
{
    return ((base->IC_RAW_INTR_STAT & 0x75FUL) | ((base->IC_TX_ABRT_SOURCE & 0x1FFFUL) << 12U) |
            ((base->IC_STATUS & 0x3FUL) << 25U));
}

/*!
 * @brief Clears the I2C master status flags.
 *
 * @param base I2C base pointer
 * @param statusFlags The status flag mask, can be a single flag or several flags in #_i2c_status_flags ORed
 * together.
 */
void I2C_MasterClearStatusFlags(I2C_Type *base, uint32_t statusFlags);

/*!
 * @brief Gets the I2C slave hardware status flags.
 *
 * @param base I2C base pointer
 * @return the mask of status flags, can be a single flag or several flags in #_i2c_status_flags ORed together
 */
static inline uint32_t I2C_SlaveGetStatusFlags(I2C_Type *base)
{
    return ((base->IC_RAW_INTR_STAT & 0xFFFUL) | (base->IC_TX_ABRT_SOURCE & 0xE000UL) |
            ((base->IC_ENABLE_STATUS & 0x6UL) << 16U) | ((base->IC_STATUS & 0x5FUL) << 25U));
}

/*!
 * @brief Clears the I2C slave status flags.
 *
 * @param base I2C base pointer
 * @param statusFlags The status flag mask, can be a single flag or several flags in #_i2c_slave_status_flags ORed
 * together
 */
void I2C_SlaveClearStatusFlags(I2C_Type *base, uint32_t statusFlags);
/* @} */

/*!
 * @name Interrupt Sub-group
 * @{
 */
/*!
 * @brief Enables I2C interrupt source.
 *
 * @param base I2C base pointer
 * @param interrupts The interrupt source mask, can be a single source or several sources in #_i2c_interrupt_enable
 * ORed together
 */
static inline void I2C_EnableInterrupts(I2C_Type *base, uint16_t interrupts)
{
    base->IC_INTR_MASK |= (uint32_t)interrupts;
}

/*!
 * @brief Disables I2C interrupt source.
 *
 * @param base I2C base pointer
 * @param interrupts The interrupt source mask, can be a single source or several sources in #_i2c_interrupt_enable
 * ORed together
 */
static inline void I2C_DisableInterrupts(I2C_Type *base, uint16_t interrupts)
{
    base->IC_INTR_MASK &= ~(uint32_t)interrupts;
}
/*!
 * @brief Get all the enabled interrupt sources.
 *
 * @param base I2C base pointer
 * @return The interrupt source mask, can be a single source or several sources in #_i2c_interrupt_enable ORed
 * together
 */
static inline uint16_t I2C_GetEnabledInterrupts(I2C_Type *base)
{
    return (uint16_t)(base->IC_INTR_MASK & 0xFFFUL);
}
/* @} */

/*!
 * @name Module Init/Deinit Sub-group
 * @{
 */
/*!
 * @brief Sets the I2C master configuration structure to default values.
 *
 * The purpose of this API is to initialize the configuration structure to default value for @ref I2C_MasterInit to use.
 * Use the unchanged structure in @ref I2C_MasterInit or modify the structure before calling @ref I2C_MasterInit.
 * This is an example:
 * @code
 *   i2c_master_config_t config;
 *   I2C_MasterGetDefaultConfig(&config);
 *   I2C_MasterInit(I2C0, &config, 12000000U);
 * @endcode
 *
 * @param masterConfig Pointer to the master configuration structure
 */
void I2C_MasterGetDefaultConfig(i2c_master_config_t *masterConfig);

/*!
 * @brief Initializes the I2C peripheral to operate as master.
 *
 * This API initialize the I2C module for master operation.
 *
 * @note If FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL is enbaled by user, the init function will not ungate I2C clock source
 * before initialization, to avoid hardfault, user has to manually enable ungate the clock source before calling the API
 *
 * The configuration structure can be filled manully or be set with default values by calling
 * @ref I2C_MasterGetDefaultConfig. This is an example.
 * @code
 *   I2C_MasterGetDefaultConfig(&config);
 *   I2C_MasterInit(I2C0, &config, 12000000U);
 * @endcode
 *
 * @param base I2C base pointer
 * @param masterConfig Pointer to the master configuration structure
 * @param sourceClock_Hz Frequency in Hertz of the I2C functional clock source
 */
void I2C_MasterInit(I2C_Type *base, const i2c_master_config_t *masterConfig, uint32_t sourceClock_Hz);

/*!
 * @brief De-initializes the I2C peripheral. Call this API to disable the I2C module.
 *
 * @param base I2C base pointer
 */
void I2C_MasterDeinit(I2C_Type *base);

/*!
 * @brief Sets the I2C slave configuration structure to default values.
 *
 * The purpose of this API is to initialize the configuration structure for @ref I2C_SlaveInit to use. Use the unchanged
 * initialized structure in @ref I2C_SlaveInit or modify the structure before calling @ref I2C_SlaveInit.
 * This is an example.
 * @code
 *   i2c_slave_config_t config;
 *   config.slaveAdress = 0x7FU;
 *   I2C_SlaveGetDefaultConfig(&config);
 * @endcode
 *
 * @param slaveConfig Pointer to the slave configuration structure
 */
void I2C_SlaveGetDefaultConfig(i2c_slave_config_t *slaveConfig);

/*!
 * @brief Initializes the I2C peripheral to operate as slave.
 *
 * This API initialize the I2C module for slave operation. User can also configure whether to enable the module in the
 * function.
 *
 * @note If FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL is enbaled by user, the init function will not ungate I2C clock source
 * before initialization, to avoid hardfault, user has to manually ungate the clock source before calling the API
 *
 * The configuration structure can be filled manully or be set with default values by calling
 * @ref I2C_SlaveGetDefaultConfig. This is an example.
 * @code
 *    i2c_slave_config_t sConfig;
 *    I2C_SlaveGetDefaultConfig(&sConfig);
 *    sConfig.address = 0x2AU;
 *    I2C_SlaveInit(I2C0, &sConfig);
 * @endcode
 *
 * @param base I2C base pointer
 * @param slaveConfig Pointer to the slave configuration structure
 */
void I2C_SlaveInit(I2C_Type *base, const i2c_slave_config_t *slaveConfig);

/*!
 * @brief De-initializes the I2C peripheral. Call this API to disable the I2C module.
 *
 * @param base I2C base pointer
 */
void I2C_SlaveDeinit(I2C_Type *base);
/* @} */

/*!
 * @name Common Peripheral Configuration Sub-group
 * @{
 */
/*!
 * @brief Sets I2C rx FIFO watermark.
 *
 * @param base I2C base pointer
 * @param rxWaterMark rx watermark level
 */
static inline void I2C_SetRxWaterMark(I2C_Type *base, uint8_t rxWaterMark)
{
    base->IC_RX_TL = (uint32_t)rxWaterMark;
}

/*!
 * @brief Sets I2C tx FIFO watermark.
 *
 * @param base I2C base pointer
 * @param txWaterMark tx watermark level
 */
static inline void I2C_SetTxWaterMark(I2C_Type *base, uint8_t txWaterMark)
{
    base->IC_TX_TL = (uint32_t)txWaterMark;
}

/*!
 * @brief Enables or disables the I2C module.
 *
 * @param base I2C base pointer
 * @param enable Pass true to enable and false to disable the module
 */
static inline void I2C_Enable(I2C_Type *base, bool enable)
{
    if (enable)
    {
        base->IC_ENABLE |= (uint32_t)I2C_IC_ENABLE_ENABLE_MASK;
    }
    else
    {
        base->IC_ENABLE &= ~(uint32_t)I2C_IC_ENABLE_ENABLE_MASK;
    }
}

/*!
 * @brief Enables the I2C module as master or slave.
 *
 * @param base I2C base pointer
 * @param enable Pass true to enable as master and false to enable as slave
 */
static inline void I2C_EnableMaster(I2C_Type *base, bool enable)
{
    if (enable)
    {
        base->IC_CON |= ((uint32_t)I2C_IC_CON_MASTER_MODE_MASK | (uint32_t)I2C_IC_CON_IC_SLAVE_DISABLE_MASK);
    }
    else
    {
        base->IC_CON &= ~((uint32_t)I2C_IC_CON_MASTER_MODE_MASK | (uint32_t)I2C_IC_CON_IC_SLAVE_DISABLE_MASK);
    }
}

/*!
 * @brief Gets the data count in tx FIFO.
 *
 * @param base I2C base pointer
 * @return The data count in tx FIFO
 */
static inline uint8_t I2C_GetTxFifoDataCount(I2C_Type *base)
{
    return (uint8_t)base->IC_TXFLR;
}

/*!
 * @brief Gets the data count in rx FIFO.
 *
 * @param base I2C base pointer
 * @return The data count in rx FIFO
 */
static inline uint8_t I2C_GetRxFifoDataCount(I2C_Type *base)
{
    return (uint8_t)base->IC_RXFLR;
}

/*!
 * @brief Gets tx FIFO depth.
 *
 * @param base I2C base pointer
 * @return The tx FIFO depth
 */
static inline uint8_t I2C_GetTxFifoDepth(I2C_Type *base)
{
    return (uint8_t)((base->IC_COMP_PARAM_1 & I2C_IC_COMP_PARAM_1_TX_BUFFER_DEPTH_MASK) >>
                     I2C_IC_COMP_PARAM_1_TX_BUFFER_DEPTH_SHIFT);
}

/*!
 * @brief Gets rx FIFO depth.
 *
 * @param base I2C base pointer
 * @return The rx FIFO depth
 */
static inline uint8_t I2C_GetRxFifoDepth(I2C_Type *base)
{
    return (uint8_t)((base->IC_COMP_PARAM_1 & I2C_IC_COMP_PARAM_1_RX_BUFFER_DEPTH_MASK) >>
                     I2C_IC_COMP_PARAM_1_RX_BUFFER_DEPTH_SHIFT);
}

/*!
 * @brief Sets I2C SDA hold time.
 *
 * @param base I2C base pointer
 * @param sdaHoldTime Number of clock source cycles of SDA hold time
 */
static inline void I2C_SetSdaHoldTime(I2C_Type *base, uint16_t sdaHoldTime)
{
    base->IC_SDA_HOLD = (uint32_t)sdaHoldTime;
}

/*!
 * @brief Sets I2C SDA setup time.
 *
 * @param base I2C base pointer
 * @param sdaSetupTime Number of clock source cycles of SDA setup time
 */
static inline void I2C_SetSdaSetupTime(I2C_Type *base, uint16_t sdaSetupTime)
{
    base->IC_SDA_SETUP = (uint32_t)sdaSetupTime;
}

/*!
 * @brief Enables I2C module to response ack to general call.
 *
 * @param base I2C base pointer
 * @param enable Pass true to enable and false to disable
 */
static inline void I2C_EnableAckGeneralCall(I2C_Type *base, bool enable)
{
    base->IC_ACK_GENERAL_CALL = (uint32_t)enable;
}

/*!
 * @brief Returns whether the I2C module is enabled.
 *
 * @param base I2C base pointer
 * @return True for enabled, false for disabled.
 */
static inline bool I2C_IsEnabled(I2C_Type *base)
{
    return (0U != (base->IC_ENABLE_STATUS & (uint32_t)I2C_IC_ENABLE_STATUS_IC_EN_MASK));
}

/*!
 * @brief Sets I2C glitch filter width of normal and fast mode.
 *
 * @param base I2C base pointer
 * @param glitchFilter Number of clock source cycles of max glitch time
 */
static inline void I2C_SetGlitchFilter(I2C_Type *base, uint8_t glitchFilter)
{
    base->IC_FS_SPKLEN = (uint32_t)glitchFilter;
}

/*!
 * @brief Sets I2C glitch filter width of high speed mode.
 *
 * @param base I2C base pointer
 * @param glitchFilter Number of clock source cycles of max glitch time
 */
static inline void I2C_SetGlitchFilterHighSpeed(I2C_Type *base, uint8_t glitchFilter)
{
    base->IC_HS_SPKLEN = (uint32_t)glitchFilter;
}
/* @} */

/*!
 * @name Master Peripheral Configuration Sub-group
 * @{
 */
/*!
 * @brief Sets I2C master high speed mode address.
 *
 * @param base I2C base pointer
 * @param address High speed address, only low 3 bits are avaliable
 */
static inline void I2C_MasterSetHighSpeedAddress(I2C_Type *base, uint8_t address)
{
    base->IC_HS_MADDR = (uint32_t)(address & 0x7UL);
}

/*!
 * @brief Returns whether the I2C module is in master mode.
 *
 * @note Although I2C can not be master and slave at the same time, when I2C is not in master mode does not mean it's in
 * slave mode, the master and slave function may both be disabled.
 *
 * @param base I2C base pointer
 * @return True for master mode enabled, false for disabled.
 */
static inline bool I2C_IsMaster(I2C_Type *base)
{
    return ((base->IC_CON & ((uint32_t)I2C_IC_CON_MASTER_MODE_MASK | (uint32_t)I2C_IC_CON_IC_SLAVE_DISABLE_MASK)) ==
            ((uint32_t)I2C_IC_CON_MASTER_MODE_MASK | (uint32_t)I2C_IC_CON_IC_SLAVE_DISABLE_MASK));
}

/*!
 * @brief Sets I2C master addressing mode.
 *
 * @param base I2C base pointer
 * @param addressingMode The I2C addressing mode master operates
 */
static inline void I2C_MasterSetAddressingMode(I2C_Type *base, i2c_address_mode_t addressingMode)
{
    if (addressingMode == kI2C_AddressMatch7bit)
    {
        base->IC_TAR &= ~(uint32_t)I2C_IC_TAR_IC_10BITADDR_MASTER_MASK;
    }
    else
    {
        base->IC_TAR |= (uint32_t)I2C_IC_TAR_IC_10BITADDR_MASTER_MASK;
    }
}

/*!
 * @brief Sets target address for I2C master transaction.
 *
 * @param base I2C base pointer
 * @param address The I2C addressing mode master operates
 */
static inline void I2C_MasterSetTargetAddress(I2C_Type *base, uint16_t address)
{
    base->IC_TAR = (base->IC_TAR & ~(uint32_t)I2C_IC_TAR_IC_TAR_MASK) | I2C_IC_TAR_IC_TAR(address);
}

/*!
 * @brief Choose whether to perform general call command.
 *
 * @param base I2C base pointer
 * @param enable True to issue general call, false to issue common start byte according to target address
 */
static inline void I2C_MasterEnableGeneralCall(I2C_Type *base, bool enable)
{
    if (enable)
    {
        base->IC_TAR = (base->IC_TAR & ~(uint32_t)I2C_IC_TAR_GC_OR_START_MASK) | (uint32_t)I2C_IC_TAR_SPECIAL_MASK;
    }
    else
    {
        base->IC_TAR &= ~((uint32_t)I2C_IC_TAR_SPECIAL_MASK | (uint32_t)I2C_IC_TAR_GC_OR_START_MASK);
    }
}

/*!
 * @brief Sets the I2C transaction speed mode.
 *
 * After the I2C module is initialized as master, user can call this function to change the transfer baud rate. There
 * are 3 speed modes to choos from, kI2C_MasterSpeedStandard for 100kbps, kI2C_MasterSpeedFast for 400kbps and
 * kI2C_MasterSpeedHigh for 3.4Mbps.
 *
 * @param base I2C base pointer.
 * @param speed Master transfer speed mode to be configured
 * @param sourceClock_Hz I2C peripheral clock frequency in Hz
 */
void I2C_MasterSetSpeedMode(I2C_Type *base, i2c_master_speed_mode_t speed, uint32_t sourceClock_Hz);
/* @} */

/*!
 * @name Slave Peripheral Configuration Sub-group
 * @{
 */
/*!
 * @brief Returns whether the I2C module is in slave mode.
 *
 * @note Although I2C can not be master and slave at the same time, when I2C is not in slave mode does not mean it's in
 * master mode, the master and slave function may both be disabled.
 *
 * @param base I2C base pointer
 * @return True for slave mode enabled, false for disabled.
 */
static inline bool I2C_IsSlave(I2C_Type *base)
{
    return ((base->IC_CON & ((uint32_t)I2C_IC_CON_MASTER_MODE_MASK | (uint32_t)I2C_IC_CON_IC_SLAVE_DISABLE_MASK)) ==
            0U);
}

/*!
 * @brief Sets I2C slave addressing mode and address.
 *
 * @param base I2C base pointer
 * @param addressingMode The I2C addressing mode slave resonses to
 * @param address The I2C address, for 7-bit only low 7 bits are valid and for 10-bit only low 10 bits are valid
 */
static inline void I2C_SlaveSetAddressingMode(I2C_Type *base, i2c_address_mode_t addressingMode, uint16_t address)
{
    if (addressingMode == kI2C_AddressMatch7bit)
    {
        base->IC_CON &= ~(uint32_t)I2C_IC_CON_IC_10BITADDR_SLAVE_MASK;
    }
    else
    {
        base->IC_CON |= (uint32_t)I2C_IC_CON_IC_10BITADDR_SLAVE_MASK;
    }
    base->IC_SAR = (uint32_t)address;
}
/* @} */

/*!
 * @name Common Bus Operation Sub-Group
 * @{
 */
/*!
 * @brief Reads one byte from data register directly.
 *
 * @param base I2C base pointer
 * @return The data read from data register.
 */
static inline uint8_t I2C_ReadByte(I2C_Type *base)
{
    return (uint8_t)(base->IC_DATA_CMD & 0xFFUL);
}

/*!
 * @brief Writes one byte to the data register directly.
 *
 * @note If using I2C as master to perform read operation, user needs to write dummy to data register to generate clock,
 * do not use this API to write dummy since this API will issue write command simultaneously, use @ref
 * I2C_MasterWriteDummy.
 *
 * @param base I2C base pointer
 * @param data The byte to write.
 */
static inline void I2C_WriteByte(I2C_Type *base, uint8_t data)
{
    base->IC_DATA_CMD = (((uint32_t)data) & 0xFFUL);
}
/* @} */

/*!
 * @name DMA Configuration Sub-Group
 * @{
 */
/*!
 * @brief Disables the I2C DMA requests.
 *
 * @param base I2C base pointer
 * @param mask Enable mask formed by enums in #_i2c_dma_enable_flags
 */
static inline void I2C_EnableDMA(I2C_Type *base, uint8_t mask)
{
    base->IC_DMA_CR |= (uint32_t)mask;
}

/*!
 * @brief Enables the I2C DMA requests.
 *
 * @param base I2C base pointer
 * @param mask Disable mask formed by enums in #_i2c_dma_enable_flags
 */
static inline void I2C_DisableDMA(I2C_Type *base, uint8_t mask)
{
    base->IC_DMA_CR &= ~(uint32_t)mask;
}

/*!
 * @brief Sets I2C tx DMA trigger level.
 *
 * @param base I2C base pointer
 * @param txLevel tx DMA trigger level
 */
static inline void I2C_SetDmaTxWaterMark(I2C_Type *base, uint8_t txLevel)
{
    base->IC_DMA_TDLR = (uint32_t)txLevel;
}

/*!
 * @brief Sets I2C rx DMA trigger level.
 *
 * @param base I2C base pointer
 * @param rxLevel rx DMA trigger level
 */
static inline void I2C_SetDmaRxWaterMark(I2C_Type *base, uint8_t rxLevel)
{
    base->IC_DMA_RDLR = (uint32_t)rxLevel;
}

/*!
 * @brief Gets I2C read/write register address.
 *
 * @param base I2C base pointer
 * @return The I2C Data Register address.
 */
static inline uint32_t I2C_GetDataRegAddr(I2C_Type *base)
{
    return (uint32_t)((void *)(&(base->IC_DATA_CMD)));
}
/* @} */

/*!
 * @name Master Bus Operation Sub-Group
 * @{
 */
/*!
 * @brief Writes dummy data data register and issue read command to generate clock.
 *
 * @note If using I2C as master to perform read operation, user needs to write dummy to data register to generate clock,
 * use this API not @ref I2C_WriteByte to write dummy, since I2C_WriteByte issue write command simultaneously.
 *
 * @param base I2C base pointer
 */
static inline void I2C_MasterWriteDummy(I2C_Type *base)
{
    base->IC_DATA_CMD = 0x100UL;
}

/*!
 * @brief Convert provided status flags to status code, and clear any errors if present.
 *
 * @note: The API only checks the error status that occured on bus, error caused by user misuse such as
 * kI2C_RxUnderflowInterruptFlag, kI2C_TxOverflowInterruptFlag or kI2C_MasterOperateWhenDisabledFlag will not be checked
 *
 * @param base The I2C peripheral base address.
 * @param status Status flags to check.
 * @retval #kStatus_Success
 * @retval #kStatus_I2C_RxHardwareOverrun Master rx FIFO overflow
 * @retval #kStatus_I2C_GeneralCall_Nak Master nacked after issueing general call
 * @retval #kStatus_I2C_ArbitrationLost Master lost arbitration when addressing
 * @retval #kStatus_I2C_Nak Master nacked during data transmitting
 * @retval #kStatus_I2C_Addr_Nak Master nacked during slave probing
 */
status_t I2C_MasterCheckAndClearError(I2C_Type *base, uint32_t status);

/*!
 * @brief Sends a piece of data to I2C bus in master mode in blocking way.
 *
 * Call this function when using I2C as master to send certain bytes of data to bus. This function uses the blocking
 * way, which means it does not return until all the data is sent to bus.
 *
 * @note The I2C module generates START/STOP/RESTART automatically. When tx FIFO state changes from empty to not empty,
 * start signal is generated; when tx FIFO is empty again, stop signal is generated; when read command follows a write
 * command, restart signal is generated in between. So stop signal will be generated after this API returns, and before
 * calling this API if tx FIFO is empty, start signal will be generated first automatically, if tx FIFO is not empty and
 * the last command in tx FIFO is write, restart signal will be generated first automatically.
 *
 * @param base I2C base pointer.
 * @param data The pointer to the data to be transmitted.
 * @param txSize The length in bytes of the data to be transmitted.
 * @retval kStatus_Success Successfully complete the data transmission.
 * @retval kStatus_I2C_ArbitrationLost Transfer error, arbitration lost.
 * @retval kStatus_I2C_Nak Transfer error, receive NAK during transfer.
 * @retval kStatus_I2C_Addr_Nak Transfer error, receive NAK during addressing, only applies when tx FIFO is empty before
 * calling this API.
 * @retval #kStatus_I2C_Timeout Transfer error, timeout happens when waiting for status flags to change.
 */
status_t I2C_MasterWriteBlocking(I2C_Type *base, const uint8_t *data, size_t txSize);

/*!
 * @brief Receives a piece of data from I2C bus in master mode in blocking way.
 *
 * Call this function when using I2C as master to receive certain bytes of data from bus. This function uses the
 * blocking way, which means it does not return until all the data is received or is nacked by slave.
 *
 * @note When doing read operation, software needs to push dummy data to tx FIFO with read command to generate clock,
 * plus I2C module generates START/STOP/RESTART automatically, meaning when tx FIFO state changes from empty to not
 * empty, start signal is generated, when tx FIFO is empty again, stop signal is generated, when read command follows
 * a write command, restart signal is generated in between. So stop signal will be generated after this API returns,
 * and before calling this API if tx FIFO is empty, start signal will be generated first automatically, if tx FIFO is
 * not empty and the last command in tx FIFO is write, restart signal will be generated first automatically.
 *
 * @param base I2C base pointer.
 * @param data The pointer to the data to store the received data.
 * @param rxSize The length in bytes of the data to be received.
 * @retval kStatus_Success Successfully complete the data transmission.
 * @retval kStatus_I2C_Timeout Send stop signal failed, timeout.
 */
status_t I2C_MasterReadBlocking(I2C_Type *base, uint8_t *data, size_t rxSize);
/* @} */

/*!
 * @name Slave Bus Operation Sub-Group
 * @{
 */
/*!
 * @brief Enables slave to send nack after receiving next byte of data
 *
 * @param base I2C base pointer
 * @param enable Pass true to enable nack and false to disable
 */
static inline void I2C_SlaveNackNextByte(I2C_Type *base, bool enable)
{
    if (enable)
    {
        base->IC_SLV_DATA_NACK_ONLY |= I2C_IC_SLV_DATA_NACK_ONLY_NACK_MASK;
    }
    else
    {
        base->IC_SLV_DATA_NACK_ONLY &= ~I2C_IC_SLV_DATA_NACK_ONLY_NACK_MASK;
    }
}

/*!
 * @brief Convert provided status flags to status code, and clear any errors if present.
 *
 * @note: The API only checks the error status that occured on bus, error caused by user misuse such as
 * kI2C_RxUnderflowInterruptFlag, kI2C_TxOverflowInterruptFlag, kI2C_SlaveReadWhenRequestedDataFlag or
 * kI2C_SlaveDisabledWhileBusy will not be checked
 *
 * @param base The I2C peripheral base address.
 * @param status Status flags to check.
 * @retval #kStatus_Success
 * @retval #kStatus_I2C_RxHardwareOverrun Slave rx FIFO overflow
 * @retval #kStatus_I2C_TxFifoFlushed Slave is addressed and is requested data but there are still old data remaining in
 * tx FIFO
 * @retval #kStatus_I2C_ArbitrationLost Data on SDA is not the same as slave transmitted
 * @retval #kStatus_I2C_Nak Slave nacked by master
 */
status_t I2C_SlaveCheckAndClearError(I2C_Type *base, uint32_t status);

/*!
 * @brief Sends a piece of data to I2C bus in slave mode in blocking way.
 *
 * Call this funtion to let I2C slave poll register status until it is addressed, then slave sends txSize of data to bus
 * until all the data has been sent to bus or untill it is nacked.
 *
 * @param base I2C base pointer.
 * @param data The pointer to the data to be transferred.
 * @param txSize The length in bytes of the data to be transferred.
 * @retval #kStatus_Success
 * @retval #kStatus_I2C_TxFifoFlushed Slave is addressed and is requested data but there are still old data remaining in
 * tx FIFO
 * @retval #kStatus_I2C_ArbitrationLost Data on SDA is not the same as slave transmitted
 * @retval #kStatus_I2C_Nak Slave nacked by master
 */
status_t I2C_SlaveWriteBlocking(I2C_Type *base, const uint8_t *data, size_t txSize);

/*!
 * @brief Receives a piece of data from I2C bus in slave mode in blocking way.
 *
 * Call this funtion to let I2C slave poll register status until it is addressed, then slave receives rxSize of data
 * until all the data has been received.
 *
 * @param base I2C base pointer.
 * @param data The pointer to the data to store the received data.
 * @param rxSize The length in bytes of the data to be received.
 * @retval kStatus_Success Successfully complete data receive.
 * @retval kStatus_I2C_Timeout Wait status flag timeout.
 */
status_t I2C_SlaveReadBlocking(I2C_Type *base, uint8_t *data, size_t rxSize);
/* @} */

/*!
 * @name Master Transfer Sub-group
 * @{
 */
/*!
 * @brief Performs a master polling transfer on the I2C bus.
 *
 * @note The API does not return until the transfer succeeds or fails due to arbitration lost or receiving a NAK.
 *
 * @param base I2C base pointer.
 * @param transferConfig Pointer to the transfer configuration structure.
 * @retval kStatus_Success Successfully complete the data transmission.
 * @retval #kStatus_I2C_Busy Previous transmission still not finished.
 * @retval #kStatus_I2C_Timeout Transfer error, timeout happens when waiting for status flags to change.
 * @retval #kStatus_I2C_ArbitrationLost Transfer error, arbitration lost.
 * @retval #kStatus_I2C_Nak Transfer error, receive NAK during transfer.
 * @retval #kStatus_I2C_Addr_Nak Transfer error, receive NAK during slave probing.
 * @retval #kStatus_I2C_GeneralCall_Nak Transfer error, receive NAK after issueing general call.
 */
status_t I2C_MasterTransferBlocking(I2C_Type *base, i2c_master_transfer_t *transferConfig);

/*!
 * @brief Initializes the I2C master transfer in interrupt way.
 *
 * This function is responsible for initializig master transfer handle, installing user callback, registering master
 * IRQ handling function and opening global interrupt.
 *
 * @param base I2C base pointer.
 * @param handle pointer to i2c_master_transfer_handle_t structure to store the transfer state.
 * @param callback pointer to user callback function.
 * @param userData User configurable pointer to any data, function, structure etc that user wish to use in the callback
 */
void I2C_MasterTransferCreateHandle(I2C_Type *base,
                                    i2c_master_handle_t *handle,
                                    i2c_master_transfer_callback_t callback,
                                    void *userData);

/*!
 * @brief Initiates a master transfer on the I2C bus in interrupt way.
 *
 * @note Transfer in interrupt way is non-blocking which means this API returns immediately after transfer initiates.
 * User can call @ref I2C_MasterTransferGetCount to get the count of data that master has transmitted/received and check
 * transfer status. If the return status is kStatus_NoTransferInProgress, the transfer is finished. Also if user
 * installs a user callback when calling @ref I2C_MasterTransferCreateHandle before, the callback will be invoked when
 * transfer finishes.
 *
 * @param base I2C base pointer
 * @param handle pointer to i2c_master_transfer_handle_t structure which stores the transfer state.
 * @param transfer Pointer to the transfer configuration structure.
 * @retval kStatus_Success Successfully start the data transmission.
 * @retval #kStatus_I2C_Busy Previous transmission still not finished.
 */
status_t I2C_MasterTransferNonBlocking(I2C_Type *base, i2c_master_handle_t *handle, i2c_master_transfer_t *transfer);

/*!
 * @brief Gets the master transfer count and status during a interrupt transfer.
 *
 * @param base I2C base pointer.
 * @param handle pointer to i2c_master_transfer_handle_t structure which stores the transfer state.
 * @param count Pointer to number of bytes transferred so far by the non-blocking transaction.
 * @retval kStatus_InvalidArgument count is Invalid.
 * @retval kStatus_NoTransferInProgress Curent no transfer is in progress.
 * @retval kStatus_Success Successfully obtained the count.
 */
status_t I2C_MasterTransferGetCount(I2C_Type *base, i2c_master_handle_t *handle, size_t *count);

/*!
 * @brief Aborts an in-process transfer in interrupt way.
 *
 * @note This API can be called at any time after a transfer of interrupt way initiates and before it finishes to abort
 * the transfer early.
 *
 * @param base I2C base pointer.
 * @param handle pointer to i2c_master_handle_t structure which stores the transfer state
 */
void I2C_MasterTransferAbort(I2C_Type *base, i2c_master_handle_t *handle);
/* @} */

/*!
 * @name Slave Transfer Sub-group
 * @{
 */
/*!
 * @brief Initializes the I2C slave transfer in interrupt way.
 *
 * This function is responsible for initializig slave transfer handle, installing user callback, registering master
 * IRQ handling function and opening global interrupt.
 *
 * @param base I2C base pointer
 * @param handle pointer to i2c_slave_handle_t structure to store the transfer state
 * @param callback pointer to user callback function
 * @param userData User configurable pointer to any data, function, structure etc that user wish to use in the callback
 */
void I2C_SlaveTransferCreateHandle(I2C_Type *base,
                                   i2c_slave_handle_t *handle,
                                   i2c_slave_transfer_callback_t callback,
                                   void *userData);

/*!
 * @brief Sets I2C slave ready to process bus events.
 *
 * Call this API to let I2C start monitoring bus events driven by I2C master on bus. When user specified event occurs,
 * callback will be invoked passes event information to the callback.
 *
 * @note When #kI2C_SlaveTransmitEvent and #kI2C_SlaveReceiveEvent occured, slave callback will
 * always be revoked regardless which events user choose to enable. This means user need not configure them in the
 * eventMask. If user wants to enable all the events, use #kI2C_SlaveAllEvents for convenience.
 *
 * @param base The I2C peripheral base address.
 * @param handle Pointer to #i2c_slave_handle_t structure which stores the transfer state.
 * @param eventMask Bit mask formed by OR operation of event(s) in #i2c_slave_transfer_event_t enumerators. When these
 *      events occur the callback will be invoked.
 * @retval kStatus_I2C_Busy I2C slave has already been started on this handle.
 */
status_t I2C_SlaveTransferNonBlocking(I2C_Type *base, i2c_slave_handle_t *handle, uint8_t eventMask);

/*!
 * @brief Gets how many bytes slave have transferred in curent data buffer.
 *
 * @param base I2C base pointer.
 * @param handle pointer to i2c_slave_handle_t structure.
 * @param count Number of bytes slave have transferred after the last start/repeated start.
 * @retval kStatus_InvalidArgument count is Invalid.
 * @retval kStatus_NoTransferInProgress Curent no transfer is in progress.
 * @retval kStatus_Success Successfully obtained the count.
 */
status_t I2C_SlaveTransferGetCount(I2C_Type *base, i2c_slave_handle_t *handle, size_t *count);

/*!
 * @brief Aborts the slave transfer.
 *
 * @note This API can be called at any time to stop slave for handling further bus events.
 *
 * @param base I2C base pointer.
 * @param handle pointer to i2c_slave_handle_t structure which stores the transfer state.
 */
void I2C_SlaveTransferAbort(I2C_Type *base, i2c_slave_handle_t *handle);
/* @} */

/*!
 * @brief Gets instance number for I2C module.
 *
 * @param base I2C base pointer.
 * @retval The number of the instance.
 */
uint32_t I2C_GetInstance(I2C_Type *base);

#if defined(__cplusplus)
}
#endif /*_cplusplus. */
/*@}*/

#endif /* _FSL_I2C_H_*/
