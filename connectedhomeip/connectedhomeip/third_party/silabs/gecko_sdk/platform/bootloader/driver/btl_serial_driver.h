/***************************************************************************//**
 * @file
 * @brief Universal UART driver for the Silicon Labs Bootloader.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#ifndef BTL_DRIVER_UART_H
#define BTL_DRIVER_UART_H
/**
 * @addtogroup Driver
 * @{
 * @addtogroup UART
 * @brief Serial UART Interface Driver
 * @details Flexible UART driver implementation for communication with
 *          external devices.
 * @{
 */
#include "em_device.h"
#include <stdbool.h>
#include <stddef.h>
#include "api/btl_errorcode.h"
#include "em_cmu.h"

/***************************************************************************//**
 * Initialize the configured USART peripheral for UART operation. Also sets up
 *  GPIO settings for TX, RX, and, if configured, flow control.
 ******************************************************************************/
void uart_init(void);

/***************************************************************************//**
 * Disable the configured USART peripheral for UART operation.
 ******************************************************************************/
void uart_deinit(void);

/***************************************************************************//**
 * Write a data buffer to the UART.
 *
 * @param[in] buffer   The data buffer to send
 * @param[in] length   Number of bytes in the buffer to send
 * @param[in] blocking Indicates whether this transfer can be offloaded to LDMA
 *                     and return, or whether to wait on completion before
 *                     returning.
 *
 * @return BOOTLOADER_OK if successful, error code otherwise
 ******************************************************************************/
int32_t uart_sendBuffer(uint8_t* buffer, size_t length, bool blocking);

/***************************************************************************//**
 * Write one byte to the UART in a blocking fashion.
 *
 * @param[in] byte The byte to send
 *
 * @return BOOTLOADER_OK if successful, error code otherwise
 ******************************************************************************/
int32_t uart_sendByte(uint8_t byte);

/***************************************************************************//**
 * Find out whether the UART can accept more data to send.
 *
 * @return true if the UART is not currently transmitting
 ******************************************************************************/
bool    uart_isTxIdle(void);

/***************************************************************************//**
 * Get the number of bytes ready for reading.
 *
 * @return Number of bytes in the receive buffer available for reading with
 *   @ref uart_receiveBuffer
 ******************************************************************************/
size_t  uart_getRxAvailableBytes(void);

/***************************************************************************//**
 * Read from the UART into a data buffer.
 *
 * @param[out] buffer The data buffer to receive into
 * @param[in] requestedLength Number of bytes to read
 * @param[out] receivedLength Number of bytes read
 * @param[in] blocking Indicates whether to wait for requestedLength
 *   bytes to be available and read before returning, or whether to read out
 *   data currently in the buffer and return.
 * @param[in] timeout Number of milliseconds to wait for data in blocking mode
 *
 * @return BOOTLOADER_OK if successful, error code otherwise
 ******************************************************************************/
int32_t uart_receiveBuffer(uint8_t  * buffer,
                           size_t   requestedLength,
                           size_t   * receivedLength,
                           bool     blocking,
                           uint32_t timeout);

/***************************************************************************//**
 * Get one byte from UART in a blocking fashion.
 *
 * @param[out] byte The byte to send
 *
 * @return BOOTLOADER_OK if successful, error code otherwise
 ******************************************************************************/
int32_t uart_receiveByte(uint8_t* byte);

/***************************************************************************//**
 * Get one byte from UART in a blocking fashion.
 *
 * @param[out] byte    The byte to send
 * @param[in]  timeout Maximum timeout before aborting transfer
 *
 * @return BOOTLOADER_OK if successful, error code otherwise
 ******************************************************************************/
int32_t uart_receiveByteTimeout(uint8_t* byte, uint32_t timeout);

/***************************************************************************//**
 * Flush one or both UART buffers.
 *
 * @param[in] flushTx Flush the transmit buffer when true
 * @param[in] flushRx Flush the receive buffer when true
 *
 * @return BOOTLOADER_OK
 ******************************************************************************/
int32_t uart_flush(bool flushTx, bool flushRx);

/**
 * @} // addtogroup Uart
 * @} // addtogroup Driver
 */

#endif // BTL_DRIVER_UART_H
