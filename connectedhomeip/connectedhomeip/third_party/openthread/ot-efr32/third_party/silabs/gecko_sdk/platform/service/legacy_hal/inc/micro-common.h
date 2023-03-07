/***************************************************************************//**
 * @file
 * @brief Minimal Hal functions common across all microcontroller-specific files.
 * See @ref micro for documentation.
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

/** @addtogroup micro
 *@{
 */

#ifndef __MICRO_COMMON_H__
#define __MICRO_COMMON_H__

/** @brief Initializes microcontroller-specific peripherals.
 */
void halInit(void);

/** @brief Restarts the microcontroller and therefore everything else.
 */
void halReboot(void);

/** @brief The value that must be passed as the single parameter to
 *  ::halInternalDisableWatchDog() in order to successfully disable the watchdog
 *  timer.
 */
#define MICRO_DISABLE_WATCH_DOG_KEY 0xA5U

/** @brief Enables the watchdog timer.
 */
void halInternalEnableWatchDog(void);

/** @brief Disables the watchdog timer.
 *
 * @note To prevent the watchdog from being disabled accidentally,
 * a magic key must be provided.
 *
 * @param magicKey  A value (::MICRO_DISABLE_WATCH_DOG_KEY) that enables the function.
 */
void halInternalDisableWatchDog(uint8_t magicKey);

/** @brief Determines whether the watchdog has been enabled or disabled.
 *
 * @return A bool value indicating if the watchdog is current enabled.
 */
bool halInternalWatchDogEnabled(void);

void halResetWatchdog(void);

/**
 * @brief Change the CTUNE value. Involves switching to HFRCO and turning off
 * the HFXO temporarily.
 *
 * @param tune   Value to set CTUNE to.
 *
 */
void halInternalSetCtune(uint16_t tune);

/**
 * @brief Get the CTUNE value.
 *
 * @return Current CTUNE value.
 *
 */
uint16_t halInternalGetCtune(void);

/** @} (end addtogroup micro) */
/** @} (end addtogroup legacyhal) */

typedef uint8_t SleepModes;

#endif //__MICRO_COMMON_H__
