/***************************************************************************//**
 * @file
 * @brief
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

#ifndef SL_RAIL_UTIL_IEEE802154_STACK_EVENT_H
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_H

#include "rail.h"

#ifdef __cplusplus
extern "C" {
#endif

// Events that the Stack can report via sl_rail_util_ieee802154_on_event():
//      STACK EVENT                                         Value    supplement passed      returns
//      -------------------------------------------------   -----    ---------------------  -------
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TICK            0x00u // MBZ                    SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_STARTED      0x01u // bool isReceivingFrame  SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_FILTERED     0x02u // bool isReceivingFrame  SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACCEPTED     0x03u // bool isReceivingFrame  SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_CORRUPTED    0x04u // bool isReceivingFrame  SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACKING       0x05u // bool isReceivingFrame  SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACK_BLOCKED  0x06u // bool isReceivingFrame  SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACK_ABORTED  0x07u // bool isReceivingFrame  SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACK_SENT     0x08u // bool isReceivingFrame  SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ENDED        0x09u // bool isReceivingFrame  SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_IDLED        0x0Au // MBZ                    SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_LISTEN       0x0Bu // MBZ                    SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_PENDED_MAC   0x21u // sl_rail_util_coex_cb_t cb          CB_PENDING/HOLDOFF/SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_PENDED_PHY   0x22u // bool isCcaTransmit     SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_CCA_SOON     0x23u // MBZ                    SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_CCA_BUSY     0x24u // bool isNextCcaImminent SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_STARTED      0x25u // MBZ                    SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_BLOCKED      0x26u // bool pktRequestedAck   SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ABORTED      0x27u // bool pktRequestedAck   SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ACK_WAITING  0x28u // MBZ                    SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ACK_RECEIVED 0x29u // bool hasFramePending   SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ACK_TIMEDOUT 0x2Au // uint8_t macRetries     SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ENDED        0x2Bu // MBZ                    SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_IDLED        0x2Cu // MBZ                    SUCCESS
#define SL_RAIL_UTIL_IEEE802154_STACK_EVENT_SIGNAL_DETECTED 0x2Du // MBZ                    SUCCESS

typedef uint8_t sl_rail_util_ieee802154_stack_event_t;

#define SL_RAIL_UTIL_IEEE802154_STACK_STATUS_SUCCESS        0x00u // Normal success
#define SL_RAIL_UTIL_IEEE802154_STACK_STATUS_CB_PENDING     0x01u // Callback is pending
#define SL_RAIL_UTIL_IEEE802154_STACK_STATUS_HOLDOFF        0x02u // MAC Holdoff option in effect
#define SL_RAIL_UTIL_IEEE802154_STACK_STATUS_UNSUPPORTED    0xFFu // MAC Holdoff option in effect

typedef uint8_t sl_rail_util_ieee802154_stack_status_t;

sl_rail_util_ieee802154_stack_status_t sl_rail_util_ieee802154_on_event(
  sl_rail_util_ieee802154_stack_event_t stack_event,
  uint32_t supplement);

#ifdef __cplusplus
}
#endif

#endif // SL_RAIL_UTIL_IEEE802154_STACK_EVENT_H
