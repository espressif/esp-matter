/***************************************************************************//**
 * @file
 * @brief Coulomb Counter Specific callbacks.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/** @addtogroup micro
 *@{
 */
#ifndef SIMULATION_DEVICE
#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#endif // SIMULATION_DEVICE

#include <stdint.h>
#include <stddef.h>
#include "coexistence-hal.h"
#include "coexistence/protocol/ieee802154/coexistence-802154.h"
#include "coexistence/protocol/ieee802154/coulomb-counter-802154.h"

#ifdef EMBER_AF_PLUGIN_COULOMB_COUNTER

#include "coulomb-counter.h"

/**
 * Set state for Coulomb Counter, Idle/Awake/Tx/Rx.
 *
 * @param[in] ptaStackEvent PTA stack event.
 *
 * This function maps PTA events to
 * corresponding Idle/Awake/Tx/Rx Coulomb Counter events .
 * Runs in ISR context
 *
 */
void halCoulombCounterEvent(halPtaStackEvent_t ptaStackEvent)
{
  static uint16_t ccState = 0U;
  static bool normalTxActive = false;

  switch (ptaStackEvent) {
    // RX listening start event(s):
    case PTA_STACK_EVENT_RX_LISTEN:
      ccState |= EMBER_AF_PLUGIN_COULOMB_COUNTER_RADIO_MODE_RX;
      break;

    // RX end event(s):
    case PTA_STACK_EVENT_RX_IDLED:
      ccState &= ~EMBER_AF_PLUGIN_COULOMB_COUNTER_RADIO_MODE_RX;
      break;

    // TX start event(s):
    case PTA_STACK_EVENT_TX_PENDED_PHY:
      normalTxActive = true;
    // Fall through - To enter TX state...

    // ACK TX start event(s):
    case PTA_STACK_EVENT_RX_ACKING:
      // RX is implicitly turned on when TX is on
      // since that's the state post-TX returns to;
      // it gets turned off only when RX_IDLED.
      ccState |= (EMBER_AF_PLUGIN_COULOMB_COUNTER_RADIO_MODE_TX
                  | EMBER_AF_PLUGIN_COULOMB_COUNTER_RADIO_MODE_RX);
      break;

    // TX end event(s):
    case PTA_STACK_EVENT_TX_BLOCKED:
    case PTA_STACK_EVENT_TX_ABORTED:
    case PTA_STACK_EVENT_TX_ENDED:
    case PTA_STACK_EVENT_TX_ACK_WAITING:
    case PTA_STACK_EVENT_TX_IDLED:
      normalTxActive = false;
    // Fall through - To exit TX state (back to RX)...

    // ACK TX start event(s):
    case PTA_STACK_EVENT_RX_ACK_BLOCKED:
    case PTA_STACK_EVENT_RX_ACK_ABORTED:
    case PTA_STACK_EVENT_RX_ACK_SENT:
      if (!normalTxActive) {
        ccState &= ~EMBER_AF_PLUGIN_COULOMB_COUNTER_RADIO_MODE_TX;
      }
      break;

    // Events that don't matter:
    default:
      return;
      break;
  }
  coexCoulombCounterEventCallback(ccState);
}

#endif //EMBER_AF_PLUGIN_COULOMB_COUNTER

/**@} END micro group
 */
