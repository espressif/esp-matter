/***************************************************************************//**
 * @file
 * @brief Wake and sleep functionality configuration
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_WAKE_LOCK_CONFIG_H
#define SL_WAKE_LOCK_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Modes of operation
// <q SL_WAKE_LOCK_ENABLE_LOCAL> Enable local wake-lock (direction: in)
// <i> Default: 1
// <i> Enable the MCU to go to sleep state and be woken up by the remote controller.
// <i> Extra connection is needed from host.
// <i> Also UARTDRV_RESTRICT_ENERGY_MODE_TO_ALLOW_RECEPTION has to be set to 0
// <i> to allow sleep below EM1
#define SL_WAKE_LOCK_ENABLE_LOCAL                 1

// <q SL_WAKE_LOCK_ENABLE_REMOTE> Enable remote wake-lock (direction: out)
// <i> Default: 1
// <i> Enable waking up the remote controller through extra pin.
#define SL_WAKE_LOCK_ENABLE_REMOTE                1
// </h>

// <h>Pin polarity selector
// <q SL_WAKE_LOCK_INPUT_POLARITY_ACTIVE_HIGH> Wake lock input pin polarity active high.
// <i> Default: 0
#define SL_WAKE_LOCK_INPUT_POLARITY_ACTIVE_HIGH   0

// <q SL_WAKE_LOCK_OUTPUT_POLARITY_ACTIVE_HIGH> Wake lock output pin polarity active high.
// <i> Default: 0
#define SL_WAKE_LOCK_OUTPUT_POLARITY_ACTIVE_HIGH  0
// </h>
// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <gpio capability=em2> SL_WAKE_LOCK_INPUT
// $[GPIO_SL_WAKE_LOCK_INPUT]
#define SL_WAKE_LOCK_INPUT_PORT                   gpioPortD
#define SL_WAKE_LOCK_INPUT_PIN                    9
// [GPIO_SL_DEEP_SLEEP]$

// <gpio> SL_WAKE_LOCK_OUTPUT
// $[GPIO_SL_WAKE_LOCK_OUTPUT]
#define SL_WAKE_LOCK_OUTPUT_PORT                  gpioPortD
#define SL_WAKE_LOCK_OUTPUT_PIN                   8
// [GPIO_SL_WAKE_LOCK_OUTPUT]$

// <<< sl:end pin_tool >>>

#endif // SL_WAKE_LOCK_CONFIG_H
