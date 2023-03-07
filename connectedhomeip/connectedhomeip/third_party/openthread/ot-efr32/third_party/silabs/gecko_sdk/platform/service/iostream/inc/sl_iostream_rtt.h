/***************************************************************************//**
 * @file
 * @brief IO Stream RTT Component.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_IOSTREAM_RTT_H
#define SL_IOSTREAM_RTT_H

#include "sl_iostream.h"
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup iostream
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup iostream_rtt I/O Stream RTT
 * @brief I/O Stream RTT
 * @details
 * ## Overview
 *
 *   Real Time Transfer (RTT) is a bi-directional communication interface developed
 *   by Segger and used with J-Link module. You need to have the Segger RTT library
 *   in your project to use this stream. It is offered as a third-party module in
 *   the Silicon Labs SDK.
 *
 *   RTT module uses a control block structure located in RAM memory with a specific
 *   ID so that it can be discovered when a connection is established via J-Link.
 *   The control block references a ring buffer for each configured channel. You can
 *   configure the number and size of the ring buffers at compile-time in
 *   SEGGER_RTT_Conf.h configuration file. Please refer to Segger's documentation
 *   for further information on RTT.
 *
 *   Note that you should only use this stream in a development environment. You
 *   should avoid using it in production.
 *
 * ## Initialization
 *
 *   The stream sets itself as the default stream at the end of the initialization
 *   function.You must reconfigure the default interface if you have multiple streams
 *   in your project else the last stream initialized will be set as the system default
 *   stream.
 *
 * ## Power manager integration
 *
 *   Because RTT communication uses the J-link debug interface when going into EM2 or EM3,
 *   the system will actually go into a special Energy Mode to maintain the debug
 *   capabilities and the power consumption will still remain high. Therefore it is unwise
 *   to keep a debug interface with RTT channel open if you want to test your power
 *   consumption.
 *
 * ## Communication channel connection
 *
 *   For connecting to the RTT channel you can use the tools provided by Segger or you
 *   can open a telnet session and connect to the port 19021 using your host IP
 *   address when the debugger is connected using USB and using J-Link debugger IP address
 *   when your debugger is connected over ethernet.
 *
 * @{
 ******************************************************************************/

extern sl_iostream_t *sl_iostream_rtt_handle;                       ///< sl_iostream_rtt_handle
extern sl_iostream_instance_info_t sl_iostream_instance_rtt_info;   ///< sl_iostream_instance_rtt_info

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************//**
 * RTT Stream init.
 *
 * @return  Status result
 ******************************************************************************/
sl_status_t sl_iostream_rtt_init(void);

/** @} (end addtogroup iostream_rtt) */
/** @} (end addtogroup iostream) */

#ifdef __cplusplus
}
#endif

#endif // SL_IOSTREAM_RTT_H
