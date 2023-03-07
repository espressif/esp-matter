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

#ifndef __SL_WISUN_UDP_CLIENT_H__
#define __SL_WISUN_UDP_CLIENT_H__

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
 * @brief Create a UDP client object.
 * @details Client is connected automatically to the server
 *****************************************************************************/
void sl_wisun_udp_client_create(void);

/**************************************************************************//**
 * @brief Close UDP client socket.
 * @details
 * @param sockid socket id
 *****************************************************************************/
void sl_wisun_udp_client_close(const int32_t sockid);

/**************************************************************************//**
 * @brief Write to Server.
 * @details Use the write API with error check
 * @param sockid socket id
 * @param[in] remote_ip_address Server IP address
 * @param[in] remote_port Server port
 * @param[in] str string to write
 *****************************************************************************/
void sl_wisun_udp_client_write(const int32_t sockid, const char *remote_ip_address,
                               const uint16_t remote_port, const char *str);

/**************************************************************************//**
 * @brief Read from Server.
 * @details print the available buffer according to the size
 * @param sockid socket id
 * @param size size of data to read
 *****************************************************************************/
void sl_wisun_udp_client_read(const int32_t sockid, const uint16_t size);

#endif
