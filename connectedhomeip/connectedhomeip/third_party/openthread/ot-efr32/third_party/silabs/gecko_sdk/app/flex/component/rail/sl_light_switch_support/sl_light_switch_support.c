/***************************************************************************//**
 * @file
 * @brief sl_light_switch_support.c
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
#include "sl_light_switch_support.h"
#include "em_system.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * Set the unique device ID in the transmit data
 *****************************************************************************/
void set_EUI(uint8_t * payload)
{
  uint64_t full_uniq = SYSTEM_GetUnique();
  uint32_t low = full_uniq & 0xFFFFFFFF;
  uint32_t high = (full_uniq >> 32) & 0xFFFFFFFF;
  uint8_t i = 0U;

  if (payload == NULL) {
    return;
  }

  while (i < 4U) {
    payload[i] = low & 0xFFU;
    low >>= 8;
    i++;
  }
  while (i < 8U) {
    payload[i] = high & 0xFFU;
    high >>= 8;
    i++;
  }
}

/******************************************************************************
 * Set the specific bits that indicates the Switch Device for the Light devices
 *****************************************************************************/
void set_role(uint8_t * payload, demo_control_role_t role)
{
  if (payload == NULL) {
    return;
  }

  if (role == DEMO_CONTROL_ROLE_LIGHT) {
    *payload  &= ~DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT;
  } else { // Role == DEMO_CONTROL_ROLE_SWITCH
    *payload  |= DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT;
  }
}

/******************************************************************************
 * Set the actual command for the Light device (e.g. toggle the lights)
 *****************************************************************************/
void set_command_type(uint8_t * payload, demo_control_command_type_t type)
{
  if (payload == NULL) {
    return;
  }

  *payload &= ~DEMO_CONTROL_PAYLOAD_CMD_MASK;
  *payload |= (type << DEMO_CONTROL_PAYLOAD_CMD_MASK_SHIFT) & DEMO_CONTROL_PAYLOAD_CMD_MASK;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
