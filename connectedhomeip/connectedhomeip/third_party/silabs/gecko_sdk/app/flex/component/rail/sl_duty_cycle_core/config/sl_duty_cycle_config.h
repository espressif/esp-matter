/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef SL_DUTY_CYCLE_CONFIG_H
#define SL_DUTY_CYCLE_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Duty Cycle RX listening time in microseconds

// <o DUTY_CYCLE_ON_TIME> Duty Cycle ON time in microseconds
// <i> Default: 1000
// <i> Define the number of microseconds the radio will be in RX mode.
#define DUTY_CYCLE_ON_TIME      (1000)

// </h> End Duty Cycle RX listening time in microseconds

// <h> Duty Cycle IDLE/Sleep maximum time in microseconds

// <o DUTY_CYCLE_OFF_TIME> Maximum Duty Cycle OFF time in microseconds
// <i> Default: 498000
// <i> Define the maximum number of microseconds the radio will be in IDLE/Sleep mode.
#define DUTY_CYCLE_OFF_TIME      (498000)

// </h> End Duty Cycle IDLE/Sleep maximum time in microseconds

// <h> Duty Cycle LCD and Button enabling

// <q DUTY_CYCLE_USE_LCD_BUTTON> Enable
// <i> Enables LCD and Button.
#define DUTY_CYCLE_USE_LCD_BUTTON      1

// </h> End Duty Cycle LCD and Button enabling

// <h> Duty Cycle EM2 Energy Level enabling

// <q DUTY_CYCLE_ALLOW_EM2> Disable
// <i> Enables EM2 Energy Level, after enabling CLI won't work properly!
#define DUTY_CYCLE_ALLOW_EM2      0

// </h> End Duty Cycle EM2 Energy Level enabling

// <<< end of configuration section >>>

#endif // SL_DUTY_CYCLE_CONFIG_H
