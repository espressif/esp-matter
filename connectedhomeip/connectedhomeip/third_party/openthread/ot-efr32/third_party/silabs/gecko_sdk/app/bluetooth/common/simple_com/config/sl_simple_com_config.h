/***************************************************************************//**
 * @file
 * @brief Simple Communication Interface (UART) Configuration
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

#ifndef SL_SIMPLE_COM_CONFIG_H
#define SL_SIMPLE_COM_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <o SL_SIMPLE_COM_RX_BUF_SIZE> Receive buffer size (bytes) <260-1024>
// <i> Default: 260
// <i> Define the size of the receive buffer in bytes.
#define SL_SIMPLE_COM_RX_BUF_SIZE        (260)

// <o SL_SIMPLE_COM_TX_BUF_SIZE> Transmit buffer size (bytes) <260-4096>
// <i> Default: 260
// <i> Define the size of the transmit buffer in bytes.
#define SL_SIMPLE_COM_TX_BUF_SIZE        (260)

// <<< end of configuration section >>>

#endif // SL_SIMPLE_COM_CONFIG_H
