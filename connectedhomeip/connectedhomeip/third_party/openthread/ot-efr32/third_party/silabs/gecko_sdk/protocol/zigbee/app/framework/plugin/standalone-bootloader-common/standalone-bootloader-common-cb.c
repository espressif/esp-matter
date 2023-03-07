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

/** @brief Incoming Message
 *
 * Indicates an Ember Bootload message has been received by the Standalone
 * Bootloader Common plugin.
 *
 * @param longId The sender's EUI64 address Ver.: always
 * @param length The length of the incoming message. Ver.: always
 * @param message The array of bytes for the message. Ver.: always
 */
WEAK(bool emberAfPluginStandaloneBootloaderCommonIncomingMessageCallback(EmberEUI64 longId,
                                                                         uint8_t length,
                                                                         uint8_t*message))
{
  // Return true to indicate the message was handled, false otherwise.
  return false;
}
