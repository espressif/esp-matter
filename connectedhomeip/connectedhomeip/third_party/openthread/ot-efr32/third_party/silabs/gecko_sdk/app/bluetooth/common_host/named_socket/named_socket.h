/***************************************************************************//**
 * @file
 * @brief Named socket handler
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

#ifndef NAMED_SOCKET_H
#define NAMED_SOCKET_H

/* Max number of bytes in receive buffer */
#define MAX_PACKET_SIZE 512

/**************************************************************************//**
 * Function to connect to a server domain socket.
 * @param[out]  handle Socket handle
 * @param[in]  fnameServer Name of Server domain socket.
 * @param[in]  fnameClient Name of Client domain socket.
 * @param[in]  encrypted indicates whether the ncp daemon encrypts data going
 *                       through that socket.
 * @return  0 on success, -1 on failure.
 *****************************************************************************/
int connect_domain_socket_server(void *handle, char *fnameServer,
                                 char *fnameClient, int encrypted);

/**************************************************************************//**
 * Function to be called when a BGAPI message is to be sent out to a domain
 *          socket.
 * @param[in]  msg_len Name Number of bytes to send.
 * @param[in]  msg_data Pointer to bytes to send.
 *****************************************************************************/
void af_socket_tx(uint32_t msg_len, uint8_t *msg_data);

/**************************************************************************//**
 * Function to be called when a BGAPI message is to be received from a domain
 *          socket.
 * @param[in]  msg_len Name Number of bytes to received.
 * @param[in]  msg_data Pointer to bytes to receive.
 * @return  The amount of bytes read or -1 on failure.
 *****************************************************************************/
int32_t af_socket_rx(uint32_t msg_len, uint8_t *msg_data);

/**************************************************************************//**
 * Function to determine whether there is new data in a socket.
 * @return  0 if there is no data in queue, any other value indicates data
 *          in queue
 *****************************************************************************/
int32_t af_socket_rx_peek(void);

/**************************************************************************//**
 * Function to turn on encryption.
 *****************************************************************************/
void turn_encryption_on(void);

/**************************************************************************//**
 * Function to turn off encryption.
 *****************************************************************************/
void turn_encryption_off(void);

/**************************************************************************//**
 * Function to read data from the domain socket if there is any.
 * @param[in]  timeout timeout to wait for data to become available,
 *                     if there isn't any.
 *****************************************************************************/
void poll_update(int timeout);

#endif /* NAMED_SOCKET_H */
