/***************************************************************************//**
 * @file
 * @brief Socket component configuration
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
#ifndef SOCKET_CONFIG_H
#define SOCKET_CONFIG_H

/**************************************************************************//**
 * @defgroup SL_WISUN_SOCKET_CONFIG Configuration
 * @ingroup SL_WISUN_SOCKET
 * @{
 *****************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Socket configuration

// <o SOCKET_HND_MAX_SOCKET_NUMBER> Max number of sockets
// <i> Default: 10
// <2-10>
#define SOCKET_HND_MAX_SOCKET_NUMBER                                   10U

// <o SOCKET_BUFFER_SIZE> Size of the socket fifo buffer. It is allocated per socket [bytes]
// <i> Default: 128
// <16-2048>
#define SOCKET_BUFFER_SIZE                                             128U
// </h>

// <<< end of configuration section >>>

/** @}*/

#endif // SOCKET_CONFIG_H
