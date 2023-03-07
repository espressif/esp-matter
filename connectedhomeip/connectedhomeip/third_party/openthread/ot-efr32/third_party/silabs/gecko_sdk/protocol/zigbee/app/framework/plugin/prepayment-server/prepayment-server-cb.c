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

/** @brief Consumer Top Up
 *
 * This function is called when a Consumer Top Up command is received. This
 * callback must evaluate the command data and determine if it is a valid Top
 * Up command.
 *
 * @param originatingDevice   Ver.: always
 * @param topUpCode   Ver.: always
 */
WEAK(bool emberAfPluginPrepaymentServerConsumerTopUpCallback(uint8_t originatingDevice,
                                                             uint8_t *topUpCode))
{
  // Return true if the top up command is valid, false if not.
  return true;
}
