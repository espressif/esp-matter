/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_UART_DMAC_H_
#define _FSL_UART_DMAC_H_

#include "fsl_uart.h"
#include "fsl_dmac.h"

/*!
 * @addtogroup uart_dmac_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief UART DMAC driver version. */
#define FSL_UART_DMAC_DRIVER_VERSION (MAKE_VERSION(2, 0, 1))
/*@}*/

/* Forward declaration of the handle typedef. */
typedef struct _uart_dmac_handle uart_dmac_handle_t;

/*! @brief UART transfer callback function. */
typedef void (*uart_dmac_transfer_callback_t)(UART_Type *base,
                                              uart_dmac_handle_t *handle,
                                              status_t status,
                                              void *userData);

/*!
 * @brief UART DMAC handle
 */
struct _uart_dmac_handle
{
    UART_Type *base; /*!< UART peripheral base address. */

    uart_dmac_transfer_callback_t callback; /*!< Callback function. */
    void *userData;                         /*!< UART callback function parameter.*/

    dmac_handle_t *txDmaHandle; /*!< The DMAC TX channel used. */
    dmac_handle_t *rxDmaHandle; /*!< The DMAC RX channel used. */

    volatile uint8_t txState; /*!< TX transfer state. */
    volatile uint8_t rxState; /*!< RX transfer state */
};

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name DMAC transactional APIs
 * @{
 */

/*!
 * @brief Initializes the UART handle which is used in transactional functions and sets the callback.
 *
 * @param base UART peripheral base address.
 * @param handle Pointer to the uart_dmac_handle_t structure.
 * @param callback UART callback, NULL means no callback.
 * @param userData User callback function data.
 * @param rxDmaHandle User requested DMAC handle for the RX DMAC transfer.
 * @param txDmaHandle User requested DMAC handle for the TX DMAC transfer.
 */
void UART_TransferCreateHandleDMAC(UART_Type *base,
                                   uart_dmac_handle_t *handle,
                                   uart_dmac_transfer_callback_t callback,
                                   void *userData,
                                   dmac_handle_t *txDmaHandle,
                                   dmac_handle_t *rxDmaHandle);

/*!
 * @brief Sends data using DMAC.
 *
 * This function sends data using DMAC. This is non-blocking function, which returns
 * right away. When all data is sent, the send callback function is called.
 *
 * @param base UART peripheral base address.
 * @param handle UART handle pointer.
 * @param xfer UART DMAC transfer structure. See #uart_transfer_t.
 * @retval kStatus_Success if succeeded; otherwise failed.
 * @retval kStatus_UART_TxBusy Previous transfer ongoing.
 * @retval kStatus_InvalidArgument Invalid argument.
 */
status_t UART_TransferSendDMAC(UART_Type *base, uart_dmac_handle_t *handle, uart_transfer_t *xfer);

/*!
 * @brief Receives data using DMAC.
 *
 * This function receives data using DMAC. This is non-blocking function, which returns
 * right away. When all data is received, the receive callback function is called.
 *
 * @param base UART peripheral base address.
 * @param handle Pointer to the uart_dmac_handle_t structure.
 * @param xfer UART DMAC transfer structure. See #uart_transfer_t.
 * @retval kStatus_Success if succeeded; otherwise failed.
 * @retval kStatus_UART_RxBusy Previous transfer on going.
 * @retval kStatus_InvalidArgument Invalid argument.
 */
status_t UART_TransferReceiveDMAC(UART_Type *base, uart_dmac_handle_t *handle, uart_transfer_t *xfer);

/*!
 * @brief Aborts the send data using DMAC.
 *
 * This function aborts the sent data using DMAC.
 *
 * @note This function only aborts the tx DMAC transfer not disable the DMA function, since disable DMA function will
 * disable tx and rx together. If user wants to abort both tx/rx DMAC transfer, use UART_EnableDMA(base, false).
 *
 * @param base UART peripheral base address.
 * @param handle Pointer to uart_dmac_handle_t structure.
 */
void UART_TransferAbortSendDMAC(UART_Type *base, uart_dmac_handle_t *handle);

/*!
 * @brief Aborts the received data using DMAC.
 *
 * This function abort receive data which using DMAC.
 *
 * @note This function only aborts the rx DMAC transfer not disable the DMA function, since disable DMA function will
 * disable tx and rx together. If user wants to abort both tx/rx DMAC transfer, use UART_EnableDMA(base, false).
 *
 * @param base UART peripheral base address.
 * @param handle Pointer to uart_dmac_handle_t structure.
 */
void UART_TransferAbortReceiveDMAC(UART_Type *base, uart_dmac_handle_t *handle);

/*@}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_UART_DMAC_H_ */
