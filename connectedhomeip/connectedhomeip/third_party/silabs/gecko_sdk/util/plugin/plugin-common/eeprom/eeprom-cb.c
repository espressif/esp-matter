/***************************************************************************//**
 * @file eeprom-cb.c
 * @brief Weak callback definitions for the eeprom component.
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
#include "hal.h"
#include "eeprom.h"

/** @brief Called whenever the EEPROM changes state
 *
 * @param oldEepromState The old state of the EEPROM
 * @param newEepromState The new state of the EEPROM
 *
 */
WEAK(void emberAfPluginEepromStateChangeCallback(HalEepromState oldEepromState,
                                                 HalEepromState newEepromState))
{
}
