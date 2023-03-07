/*
 * Copyright 2020,2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_I2C_DMAC_H_
#define _FSL_I2C_DMAC_H_

#include "fsl_i2c.h"
#include "fsl_dmac.h"

/*!
 * @addtogroup i2c_dmac_driver
 * @ingroup i2c
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief I2C DMAC driver version. */
#define FSL_I2C_DMAC_DRIVER_VERSION (MAKE_VERSION(2, 0, 2))
/*@}*/

/*! @brief Retry times for waiting flag. */
#ifndef I2C_RETRY_TIMES
#define I2C_RETRY_TIMES 0U /* Define to zero means keep waiting until the flag is assert/deassert. */
#endif

/*! @brief Buffer length of re-allocated 16-bit width data. */
#define I2C_TX_LENGTH 8U

/*! @brief I2C master DMAC handle typedef. */
typedef struct _i2c_master_dmac_handle i2c_master_dmac_handle_t;

/*! @brief I2C master DMAC transfer callback typedef. */
typedef void (*i2c_master_dmac_transfer_callback_t)(I2C_Type *base,
                                                    i2c_master_dmac_handle_t *handle,
                                                    status_t status,
                                                    void *userData);

/*! @brief Typedef for master dmac handler. */
typedef void (*i2c_dmac_master_irq_handler_t)(I2C_Type *base, i2c_master_dmac_handle_t *handle);

/*! @brief I2C master DMAC transfer structure. */
struct _i2c_master_dmac_handle
{
    I2C_Type *base;                 /*!< I2C instance base pointer. */
    size_t transferSize;            /*!< Total bytes to be transferred. */
    volatile size_t dataSize;       /*!< Left bytes to be transferred. */
    uint8_t *volatile data;         /*!< Pointer to the user send/receive data buffer. */
    uint16_t txData[I2C_TX_LENGTH]; /*!< The buffer of re-allocated 16-bit width data to be written to tx register. */
    dmac_channel_transfer_config_t dmacTxConfig; /*!< The DMAC transfer configuration to transmit data. */

    uint8_t state;               /*!< I2C master transfer status. */
    dmac_handle_t *dmacTxHandle; /*!< The DMAC handler used for tx. */
    dmac_handle_t *dmacRxHandle; /*!< The DMAC handler used for rx. */
    i2c_master_dmac_transfer_callback_t
        completionCallback; /*!< A callback function called after the DMAC transfer finished. */
    void *userData;         /*!< A callback parameter passed to the callback function. */
};

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus. */

/*!
 * @name I2C Master DMAC Transfer Sub-group
 * @{
 */

/*!
 * @brief Initializes the I2C master transfer in DMAC way.
 *
 * @param base I2C peripheral base address
 * @param handle Pointer to i2c_master_dmac_handle_t structure to store the transfer state.
 * @param callback Pointer to the user callback function
 * @param userData User configurable pointer to any data, function, structure etc that user wish to use in the callback
 * @param dmacTxHandle DMAC handle pointer for tx
 * @param dmacRxHandle DMAC handle pointer for rx
 */
void I2C_MasterTransferCreateHandleDMAC(I2C_Type *base,
                                        i2c_master_dmac_handle_t *handle,
                                        i2c_master_dmac_transfer_callback_t callback,
                                        void *userData,
                                        dmac_handle_t *dmacTxHandle,
                                        dmac_handle_t *dmacRxHandle);

/*!
 * @brief Initiates a master transfer on the I2C bus in DMAC way.
 *
 * @note Transfer in DMAC way is non-blocking which means this API returns immediately after transfer initiates.
 * If user installs a user callback when calling @ref I2C_MasterTransferCreateHandleDMAC before, the callback will be
 * invoked when transfer finishes.
 *
 * @param base I2C base pointer
 * @param handle pointer to i2c_master_transfer_handle_t structure which stores the transfer state.
 * @param xfer Pointer to the transfer configuration structure.
 * @retval kStatus_Success Successfully start the data transmission.
 * @retval #kStatus_I2C_Busy Previous transmission still not finished.
 */
status_t I2C_MasterTransferDMAC(I2C_Type *base, i2c_master_dmac_handle_t *handle, i2c_master_transfer_t *xfer);

/*!
 * @brief Aborts an in-process transfer in DMAC way.
 *
 * @note This API can be called at any time after a transfer of DMAC way initiates and before it finishes to abort
 * the transfer early.
 *
 * @param base I2C base pointer.
 * @param handle Pointer to i2c_master_handle_t structure which stores the transfer state
 */
void I2C_MasterTransferAbortDMAC(I2C_Type *base, i2c_master_dmac_handle_t *handle);

/* @} */
#if defined(__cplusplus)
}
#endif /*_cplusplus. */
/*@}*/
#endif /*_FSL_I2C_DMAC_H_*/
