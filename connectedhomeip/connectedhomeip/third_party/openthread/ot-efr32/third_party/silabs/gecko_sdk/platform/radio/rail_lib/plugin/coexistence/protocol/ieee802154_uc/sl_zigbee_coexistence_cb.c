/***************************************************************************//**
 * @file
 * @brief Zigbee Specific Radio Coexistence callbacks.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "coexistence-802154.h"
#include "stack/include/ember-types.h"

extern void emCallCounterHandler(EmberCounterType type, uint8_t data);

SL_WEAK void sl_rail_util_coex_counter_on_event(sl_rail_util_coex_event_t event)
{
  EmberCounterType type = EMBER_COUNTER_PTA_LO_PRI_REQUESTED + (EmberCounterType)event;
  emCallCounterHandler(type, 1);
}
