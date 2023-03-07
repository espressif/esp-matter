/***************************************************************************//**
 * @file
 * @brief UART header file
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef UART_H
#define UART_H

/**************************************************************************//**
 * \defgroup uart UART
 * \brief UART API
 *****************************************************************************/

/**************************************************************************//**
 * \defgroup platform_hw Platform HW
 * \brief Platform HW
 *****************************************************************************/

/**************************************************************************//**
 * @addtogroup platform_hw
 * @{
 *****************************************************************************/

/**************************************************************************//**
 * @addtogroup uart
 * @{
 *****************************************************************************/

/**************************************************************************//**
 * Open the serial port.
 * @param[out]  handle Descriptor handle
 * @param[in]  port Serial port to use.
 * @param[in]  baudRate Baud rate to use.
 * @param[in]  rtsCts Enable/disable hardware flow control.
 * @param[in]  timeout Constant used to calculate the total time-out period fo
 *                     read operations, in milliseconds.
 * @return  -1 on failure; on Win 0 on success,
 *          on Posix serial handler on success
 *****************************************************************************/
int32_t uartOpen(void *handle, int8_t *port, uint32_t baudRate,
                 uint32_t rtsCts, int32_t timeout);

/**************************************************************************//**
 * Flushes accumulated data.
 *
 * @param[in]  handle Descriptor handle
 *****************************************************************************/
void uartFlush(void *handle);

/**************************************************************************//**
 * Close the serial port.
 * @param[in]  handle Descriptor handle
 * @return  0 on success, -1 on failure.
 *****************************************************************************/
int32_t uartClose(void *handle);

/**************************************************************************//**
 * Blocking read data from serial port. The function will block until the
 *          desired amount has been read or an error occurs.
 * @note  In order to use this function the serial port has to be configured
 *        blocking. This can be done by calling uartOpen() with 'timeout = 0'.
 * @param[in]  handle Descriptor handle
 * @param[in]  dataLength The amount of bytes to read.
 * @param[out]  data Buffer used for storing the data.
 * @return  The amount of bytes read or -1 on failure.
 *****************************************************************************/
int32_t uartRx(void *handle, uint32_t dataLength, uint8_t *data);

/**************************************************************************//**
 * Non-blocking read from serial port.
 * @note  A truly non-blocking operation is possible only if uartOpen()
 *        is called with timeout parameter set to 0.
 * @param[in]  handle Descriptor handle
 * @param[in]  dataLength The amount of bytes to read.
 * @param[out]  data Buffer used for storing the data.
 * @return  The amount of bytes read, 0 if configured serial blocking time
 *          interval elapses or -1 on failure.
 *****************************************************************************/
int32_t uartRxNonBlocking(void *handle, uint32_t dataLength, uint8_t *data);

/**************************************************************************//**
 * Return the number of bytes in the input buffer.
 * @param[in]  handle Descriptor handle
 * @return  The number of bytes in the input buffer or -1 on failure.
 *****************************************************************************/
int32_t uartRxPeek(void *handle);

/**************************************************************************//**
 * Write data to serial port. The function will block until
 *          the desired amount has been written or an error occurs.
 * @param[in]  handle Descriptor handle
 * @param[in]  dataLength The amount of bytes to write.
 * @param[in]  data Buffer used for storing the data.
 * @return  The amount of bytes written or -1 on failure.
 *****************************************************************************/
int32_t uartTx(void *handle, uint32_t dataLength, uint8_t *data);

/** @} (end addtogroup uart) */
/** @} (end addtogroup platform_hw) */

#endif /* UART_H */
