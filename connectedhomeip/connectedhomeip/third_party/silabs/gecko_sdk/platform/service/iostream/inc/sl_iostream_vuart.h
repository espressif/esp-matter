/***************************************************************************//**
 * @file
 * @brief IO Stream VUART Component.
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
#ifndef SL_IOSTREAM_VUART_H
#define SL_IOSTREAM_VUART_H

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
 * @addtogroup iostream_vuart I/O Stream VUART
 * @brief IO Stream VUART
 * @details
 * ## Overview
 *
 *   Virtual UART (VUART) is a special interface where RTT is used as input channel
 *   and the Serial Wire Output(SWO) is used as output channel.
 *
 *   On the receive side, a RTT buffer with a dedicated name is allocated for
 *   the reception.
 *
 *   IOStream-VUART uses SWO-stimulus 8 as output channel. Note that the transmit
 *   channel also implements a small protocol, so the data is encapsulated in a
 *   frame which must be decoded on the host side.
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
 *   Because RTT communication uses the J-link debug interface, when going into EM2
 *   or EM3, the system will actually go into a special EM2 where the high-Frequency
 *   clocks are still running and some CPU core functionalities are still powered to
 *   maintain the debug features.Therefore it is unwise to keep a debug interface with
 *   RTT channel open if you want to test your power consumption.
 *
 * ## Communication channel connection
 *
 *   For connecting to the vuart console you need to use the WSTK and you must be
 *   connected using the ethernet link. Then you can open a telnet session on port
 *   4900 using the WSTK IP address. Note that the WSTK firmware decodes the
 *   received frame and it outputs only the payload into vuart console.
 *
 * @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Global Variables

extern sl_iostream_t *sl_iostream_vuart_handle;
extern sl_iostream_instance_info_t sl_iostream_instance_vuart_info;

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************//**
 * Initialize VUART stream component.
 *
 * @return  Status Code:
 *            - SL_STATUS_OK
 *            - SL_STATUS_FAIL
 ******************************************************************************/
sl_status_t sl_iostream_vuart_init(void);

/** @} (end addtogroup iostream_vuart) */
/** @} (end addtogroup iostream) */

#ifdef __cplusplus
}
#endif

#endif /* SL_IOSTREAM_VUART_H */
