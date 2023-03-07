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

/** @brief Activate Door Lock
 *
 * This function is provided by the door lock server plugin. It is
 *
 * @param activate True if the lock should move to the locked position, false
 * if it should move to the unlocked position Ver.: always
 */
WEAK(bool emberAfPluginDoorLockServerActivateDoorLockCallback(bool activate))
{
  return true;
}
