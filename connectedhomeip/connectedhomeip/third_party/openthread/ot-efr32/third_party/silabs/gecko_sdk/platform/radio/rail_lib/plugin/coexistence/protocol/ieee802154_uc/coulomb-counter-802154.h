/***************************************************************************//**
 * @file
 * @brief Coulomb Counter (CC) interface.
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

#ifndef COULOMB_COUNTER_802154_H
#define COULOMB_COUNTER_802154_H

#include "sl_rail_util_ieee802154_stack_event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Set state for Coulomb Counter, Idle/Awake/Tx/Rx.
 *
 * @param[in] stack_event stack event.
 * @param[in] supplement additional stack event information .
 *
 * This function maps stack events to
 * corresponding Idle/Awake/Tx/Rx Coulomb Counter events .
 *
 */
void sl_rail_util_coulomb_counter_on_event(sl_rail_util_ieee802154_stack_event_t stack_event, uint32_t supplement);

/**
 * Callback that this plug-in uses to pass up the radio state.
 * @param ccState The current radio state
 */
void coexCoulombCounterEventCallback(uint16_t ccState);

#ifdef __cplusplus
}
#endif

#endif //COULOMB_COUNTER_802154_H

/**@} END micro group
 */
