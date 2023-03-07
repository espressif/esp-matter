/***************************************************************************//**
 * @file
 * @brief
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

#ifndef __SL_WISUN_TCP_CLIENT_H__
#define __SL_WISUN_TCP_CLIENT_H__
// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Create a TCP client object.
 * @details Client is connected automatically to the server
 * @param[in] ip_address Server IP address
 * @param[in] port Server port
 *****************************************************************************/
void sl_wisun_tcp_client_create(const char *ip_address, uint16_t port);

/**************************************************************************//**
 * @brief Close TCP client socket.
 * @details
 * @param sockid socket id
 *****************************************************************************/
void sl_wisun_tcp_client_close(const int32_t sockid);

/**************************************************************************//**
 * @brief Write to Server.
 * @details Use the write API with error check
 * @param sockid socket ID
 * @param str string to write
 *****************************************************************************/
void sl_wisun_tcp_client_write(const int32_t sockid, const char *str);

/**************************************************************************//**
 * @brief Read from Server.
 * @details print the available buffer according to the size
 * @param sockid socket ID
 * @param size size of data to read
 *****************************************************************************/
void sl_wisun_tcp_client_read(const int32_t sockid, const uint16_t size);

#endif
