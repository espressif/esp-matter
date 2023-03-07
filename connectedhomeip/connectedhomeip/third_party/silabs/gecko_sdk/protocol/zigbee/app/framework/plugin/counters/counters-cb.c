/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Counters plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "af.h"
#include "counters.h"

/** @brief Rollover
 *
 * This function is called every time a counter exceeds its threshold.
 *
 * @param type The counter that rolled over Ver.: always
 */
WEAK(void emberAfPluginCountersRolloverCallback(EmberCounterType type))
{
}
