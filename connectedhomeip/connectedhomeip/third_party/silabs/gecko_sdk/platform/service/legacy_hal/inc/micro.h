/***************************************************************************//**
 * @file
 * @brief Full HAL functions common across all microcontroller-specific files.
 * See @ref micro for documentation.
 *
 * Some functions in this file return an ::EmberStatus value.
 * See error-def.h for definitions of all ::EmberStatus return values.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

/***************************************************************************//**
 * @addtogroup legacyhal
 * @{
 ******************************************************************************/

/** @addtogroup micro Microcontroller functions
 * @brief Microcontroller functions
 *
 * Many of the supplied example applications use these microcontroller functions.
 * See hal/micro/micro.h for source code.
 *
 * @note The term SFD refers to the Start Frame Delimiter.
 *@{
 */

#ifndef __MICRO_H__
#define __MICRO_H__

#include "micro-types.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// Make sure that a proper plat/micro combination was selected if we aren't
// building for a host processor
#if ((!defined(EZSP_HOST)) && (!defined(UNIX_HOST)) && (!defined(TOKEN_MANAGER_TEST)) && !defined(EMBER_TEST))

#ifndef PLAT
  #error no platform defined, or unsupported
#endif
#ifndef MICRO
  #error no micro defined, or unsupported
#endif
#ifndef PHY
  #error no phy defined, or unsupported
#endif

#endif // ((! defined(EZSP_HOST)) && (! defined(UNIX_HOST)))

#endif // DOXYGEN_SHOULD_SKIP_THIS

/** @brief Gets information about what caused the microcontroller to reset.
 *
 * @return A code identifying the cause of the reset.
 */
uint8_t halGetResetInfo(void);

/** @brief Calls ::halGetResetInfo() and supplies a string describing it.
 *
 * @appusage Useful for diagnostic printing of text just after program
 * initialization.
 *
 * @return A pointer to a program space string.
 */
const char * halGetResetString(void);

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "micro-common.h"

#if defined(CORTEXM3_EFM32_MICRO)
  #include "efm32_micro.h"
#elif defined(TOKEN_MANAGER_TEST) || defined(EMBER_TEST)
// pass
#elif ((defined(EZSP_HOST) || defined(UNIX_HOST)))
  #include "micro_host.h"
#else
  #error no platform or micro defined
#endif

#endif // DOXYGEN_SHOULD_SKIP_THIS

/** @} (end addtogroup micro) */
/** @} (end addtogroup legacyhal) */

#endif //__MICRO_H__
