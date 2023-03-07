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

#include "af.h"

/** @brief Request Mirror
 *
 * This function is called by the Simple Metering client plugin whenever a
 * Request Mirror command is received. The application should return the
 * endpoint to which the mirror has been assigned. If no mirror could be
 * assigned, the application should return 0xFFFF.
 *
 * @param requestingDeviceIeeeAddress   Ver.: always
 */
WEAK(uint16_t emberAfPluginSimpleMeteringClientRequestMirrorCallback(EmberEUI64 requestingDeviceIeeeAddress))
{
  return 0xFFFF;
}

/** @brief Remove Mirror
 *
 * This function is called by the Simple Metering client plugin whenever a
 * Remove Mirror command is received. The application should return the
 * endpoint on which the mirror has been removed. If the mirror could not be
 * removed, the application should return 0xFFFF.
 *
 * @param requestingDeviceIeeeAddress   Ver.: always
 */
WEAK(uint16_t emberAfPluginSimpleMeteringClientRemoveMirrorCallback(EmberEUI64 requestingDeviceIeeeAddress))
{
  return 0xFFFF;
}
