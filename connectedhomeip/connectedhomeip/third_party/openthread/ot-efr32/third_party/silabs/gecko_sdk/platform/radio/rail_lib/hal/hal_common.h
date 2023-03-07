/***************************************************************************//**
 * @file
 * @brief Header file for RAIL HAL common functionality
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

#ifndef __RAILTEST_HAL_H__
#define __RAILTEST_HAL_H__

#include <stdint.h>
#include <stdbool.h>

#include "em_core.h"
#include "em_gpio.h" // For debug signal definitions

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Typedefs
 *****************************************************************************/

/**
 * Structure to hold the definition of a debug pin. These are defined for a
 * given board/chip combination and provide some standard GPIOs to use for
 * debugging.
 */
typedef struct debugPin {
  /** Name to use for this pin so the user can select it */
  char *name;
  /** PRS channel to use for outputting on this pin */
  uint8_t prsChannel;
  /** PRS location in the above channel to output on the right GPIO */
  uint8_t prsLocation;
  /** GPIO port that corresponds to this debug pin */
  GPIO_Port_TypeDef gpioPort;
  /** GPIO pin that corresponds to this debug pin */
  uint8_t gpioPin;
} debugPin_t;

/**
 * Structure to hold information about a debug signal that can be output on
 * this chip. This is a combination of the RAIL debug signals and PRS debug
 * signals for this hardware combination.
 */
typedef struct debugSignal {
  /** User friendly name for this debug signal */
  char *name;
  /** Whether to use the PRS for this signal or RAIL */
  bool isPrs;
  /** Union to hold information about the RAIL or PRS signal to output */
  union {
    /** Holds the PRS signal and source for a given debug signal */
    struct prs_t {
      uint8_t signal;
      uint8_t source;
    } prs;
    /** Holds the RAIL debug event number */
    uint16_t debugEventNum;
  } loc;
} debugSignal_t;

/******************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * HAL API to get all the known debug signals for a given chip in a unified
 * structure.
 * @param size A pointer to a integer which will be set to the number of debug
 * signals that are in the returned list.
 * @return A pointer to a list of debug signals available on this chip.
 */
const debugSignal_t* halGetDebugSignals(uint32_t *size);

/**
 * HAL API to get all the known debug pins for a given chip in a unified
 * structure.
 * @param size A pointer to a integer which will be set to the number of debug
 * pins that are in the returned list.
 * @return A pointer to a list of debug pins available on this chip.
 */
const debugPin_t* halGetDebugPins(uint32_t *size);

/**
 * HAL API to enable PRS output on a specific channel of a given debug signal.
 * @param channel The PRS channel to enable.
 * @param loc The output location to use for this PRS channel.
 * @param port The GPIO port to use for this PRS channel.
 * @param pin The GPIO pin to use for this PRS channel.
 * @param source The source to use for this PRS channel's output.
 * @param signal The signal to use for this PRS channel's output.
 */
void halEnablePrs(uint8_t channel,
                  uint8_t loc,
                  GPIO_Port_TypeDef port,
                  uint8_t pin,
                  uint8_t source,
                  uint8_t signal);

/**
 * HAL API to turn off PRS output on a given channel.
 * @param channel The PRS channel to disable.
 */
void halDisablePrs(uint8_t channel);

void halCommonMemMove(void *dest, const void *src, uint16_t bytes);

#define MEMCOPY(d, s, l) halCommonMemMove(d, s, l)

/**
 * @brief Returns the elapsed time between two 32 bit values.
 *   Result may not be valid if the time samples differ by more than 2147483647
 */
#define elapsedTimeInt32u(oldTime, newTime) \
  ((uint32_t) ((uint32_t)(newTime) - (uint32_t)(oldTime)))

#define halResetWatchdog()
#define DISABLE_INTERRUPTS()  CORE_ATOMIC_IRQ_DISABLE()
#define RESTORE_INTERRUPTS()  CORE_ATOMIC_IRQ_ENABLE()

/**
 * @brief A block of code may be made atomic by wrapping it with this
 * macro.  Something which is atomic cannot be interrupted by interrupts.
 */
#define ATOMIC(blah)          CORE_ATOMIC_SECTION(blah)

#ifdef __cplusplus
}
#endif

#endif // __RAILTEST_HAL_H__
