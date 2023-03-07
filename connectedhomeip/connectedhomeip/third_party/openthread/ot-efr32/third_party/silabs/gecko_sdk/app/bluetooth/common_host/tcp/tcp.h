/***************************************************************************//**
 * @file
 * @brief TCP header file
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef TCP_H
#define TCP_H

#include <stdint.h>

#define DEFAULT_BUFLEN 512

/**************************************************************************//**
 * Open a TCP communication through socket.
 * @param[out]  handle Socket handle
 * @param[in]  ip IPv4 address.
 * @param[in]  port Port to use.
 * @return  0 on success, -1 on failure.
 *****************************************************************************/
int32_t tcp_open(void *handle, char *ip, char *port);

/**************************************************************************//**
 * Send data to device through TCP. The function will block until
 *          the desired amount has been written or an error occurs.
 * @param[in]  handle Socket handle
 * @param[in]  data_length The amount of bytes to write.
 * @param[in]  data Buffer used for storing the data.
 * @return  The amount of bytes written or -1 on failure.
 *****************************************************************************/
int32_t tcp_tx(void *handle, uint32_t data_length, uint8_t *data);

/**************************************************************************//**
 * Blocking read data from device through TCP. The function will block until
 *          the desired amount has been read or an error occurs.
 * @param[in]  handle Socket handle
 * @param[in]  data_length The amount of bytes to read.
 * @param[out]  data Buffer used for storing the data.
 * @return  The amount of bytes read or -1 on failure.
 *****************************************************************************/
int32_t tcp_rx(void *handle, uint32_t data_length, uint8_t *data);

/**************************************************************************//**
 * Return the number of bytes in the input buffer.
 * @param[in]  handle Socket handle
 * @return  The number of bytes in the input buffer or -1 on failure.
 *****************************************************************************/
int32_t tcp_rx_peek(void *handle);

/**************************************************************************//**
 * Close the TCP connection.
 * @param[in]  handle Socket handle
 * @return  0 on success, -1 on failure.
 *****************************************************************************/
int32_t tcp_close(void *handle);

#endif
