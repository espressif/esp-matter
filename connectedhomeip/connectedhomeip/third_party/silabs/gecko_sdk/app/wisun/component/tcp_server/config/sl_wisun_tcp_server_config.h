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
#ifndef SL_WISUN_TCP_SERVER_CONFIG_H
#define SL_WISUN_TCP_SERVER_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Wi-SUN TCP Server configuration
// <o SL_WISUN_TCP_SERVER_PORT> TCP Server port
// <i> Default: 4567
// <i> This is the port number where TCP server is listening
// <1-65536>
#define SL_WISUN_TCP_SERVER_PORT                    4567U

// <o SL_WISUN_TCP_SERVER_BUFF_SIZE> TCP Server receive buffer size [bytes]
// <i> Default: 128
// <1-2048>
#define SL_WISUN_TCP_SERVER_BUFF_SIZE               128U
// </h>

// <<< end of configuration section >>>

#endif // SL_WISUN_TCP_SERVER_CONFIG_H
