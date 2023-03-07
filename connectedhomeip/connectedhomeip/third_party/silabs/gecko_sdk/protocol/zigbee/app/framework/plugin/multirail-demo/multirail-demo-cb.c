/***************************************************************************//**
 * @file
 * @brief
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

#include PLATFORM_HEADER
#include "rail.h"
/** @brief A callback called whenever a secondary instance RAIL event occurs.
 *
 * @param[in] handle A handle for a RAIL instance.
 * @param[in] events A bit mask of RAIL events (full list in rail_types.h)
 */
WEAK(void emberAfPluginMultirailDemoRailEventCallback(RAIL_Handle_t handle,
                                                      RAIL_Events_t events))
{
  (void)handle; // unreferenced parameter
  (void)events; // unreferenced parameter

  // Suggested action:
  // if (events & (...a bitmask made from event(s) from the list...)) {
  //   do_something(handle);
  // }
}
