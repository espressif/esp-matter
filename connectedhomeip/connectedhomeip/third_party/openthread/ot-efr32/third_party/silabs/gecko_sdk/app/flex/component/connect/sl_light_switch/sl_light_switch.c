/***************************************************************************//**
 * @file
 * @brief sl_light_switch.c
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "sl_light_switch.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// This variable indicates the actual light state
static demo_light_t light_state = DEMO_LIGHT_OFF;
// PAN ID of the device
static uint16_t pan_id;
// Communication channel of the device
static uint16_t channel;
// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * Get the actual state of the light
 *****************************************************************************/
demo_light_t sl_get_light_state(void)
{
  return light_state;
}

/**************************************************************************//**
 * Set the actual state of the light
 *****************************************************************************/
void sl_set_light_state(demo_light_t new_state)
{
  light_state = new_state;
}

/**************************************************************************//**
 * Set the device's PAN ID
 *****************************************************************************/
void sl_set_pan_id(uint16_t new_id)
{
  pan_id = new_id;
}

/**************************************************************************//**
 * Get the device's PAN ID
 *****************************************************************************/
uint16_t sl_get_pan_id(void)
{
  return pan_id;
}

/**************************************************************************//**
 * Set the communication channel
 *****************************************************************************/
void sl_set_channel(uint16_t new_channel)
{
  channel = new_channel;
}

/**************************************************************************//**
 * Get the communication channel
 *****************************************************************************/
uint16_t sl_get_channel(void)
{
  return channel;
}
