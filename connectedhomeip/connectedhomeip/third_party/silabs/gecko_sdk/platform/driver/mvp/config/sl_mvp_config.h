/***************************************************************************//**
 * @file
 * @brief MVP configuration file.
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

// <<< Use Configuration Wizard in Context Menu >>>

#ifndef SL_MVP_CONFIG_H
#define SL_MVP_CONFIG_H

// <q SL_MVP_ENABLE_DMA> Use DMA to load MVP programs
// <i> Enable or disable DMA for loading MVP programs.
// <i> Default: 0
#define SL_MVP_ENABLE_DMA  0

// <q SL_MVP_DMA_CHANNEL> DMA channel to used when DMA is enabled
// <i> DMA channel to use when DMA is enabled
// <i> Default: 0
#define SL_MVP_DMA_CHANNEL 0

// <o SL_MVP_POWER_MODE> CPU power mode during MVP execution.
// <i> The power mode configuration controls what software should
// <i> do when waiting for an MVP program to finish execution.
//
// <0=> No sleep
// <1=> Enter EM1
// <2=> Yield RTOS thread
//
// <i> When "No sleep" option is used the MCU core will busy-wait for the MVP
// <i> to finish, this is the option which provides the fastest MVP execution
// <i> time. The "No sleep" option can be used in a bare-metal application or
// <i> an application using RTOS.
// <i> When "Enter EM1" option is used the MCU will be put into EM1 whenever
// <i> the driver waits for an MVP program to complete. The "Enter EM1" option
// <i> is not safe to use in an application using RTOS.
// <i> When "Yield RTOS thread" option is used the task waiting for the MVP
// <i> program to complete will yield, allowing other tasks in the system to
// <i> run or potentially let the scheduler put the system into a sleep mode.
// <i> The "Yield RTOS thread" requires that the application is using RTOS.
// <i> Default: 0
#define SL_MVP_POWER_MODE  0

// <q SL_MVP_OPTIMIZE_SPEED> Enable additional speed optimizations for MVP operations
// <i> By enabling this, the MVP will attempt to optimize select operations
// <i> to run faster, at the expence of increased RAM usage.
// <i> Note: This may increase RAM usage greatly.
// <i> Default: 0
#define SL_MVP_OPTIMIZE_SPEED  0

#endif /* SL_MVP_CONFIG_H */

// <<< end of configuration section >>>
