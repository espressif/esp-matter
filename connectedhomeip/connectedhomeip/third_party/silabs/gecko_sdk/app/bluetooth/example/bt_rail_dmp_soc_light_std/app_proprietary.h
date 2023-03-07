/***************************************************************************//**
 * @file
 * @brief Application interface for DMP with standard protocols.
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

#ifndef APP_PROPRIETARY_H
#define APP_PROPRIETARY_H

#include "rail_types.h"

#define TX_PAYLOAD_LENGTH   (16u)
#define RAIL_FIFO_SIZE      (256u)

// Proprietary task event flags
typedef enum {
  PROP_STATUS_SEND                  = 0x00,
  PROP_TIMER_EXPIRED                = 0x01,
  PROP_TOGGLE_MODE                  = 0x02,
  PROP_TOGGLE_RXD                   = 0x03,
  PROP_PROCESS_RXD                  = 0x04
} prop_msg_t;

/**************************************************************************//**
 * Proprietary application init
 *****************************************************************************/
extern void init_prop_app(void);

/**************************************************************************//**
 * Message queue post handler for proprietary application task
 * @param[in] msg Message to put into the queue
 *****************************************************************************/
extern void proprietary_queue_post(prop_msg_t msg);

/**************************************************************************//**
 * RAIL transmit FIFO init for standard protocols
 * @param[in] rail_handle RAIL handle which the tx fifo is to set for
 * @return int16_t size of the allocated tx fifo in bytes
 *****************************************************************************/
extern int16_t app_set_rail_tx_fifo(RAIL_Handle_t rail_handle);

#endif // APP_PROPRIETARY_H
