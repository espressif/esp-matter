/***************************************************************************//**
 * @file
 * @brief Board support package API for functions on MCU plugin boards.
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

#include <stdbool.h>
#include "bsp.h"
#include "em_gpio.h"
#include "em_cmu.h"

/***************************************************************************//**
 * @addtogroup BSP
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup BSP_DK API for DKs
 * @{
 ******************************************************************************/

/**************************************************************************//**
 * @brief Disable MCU plugin board peripherals.
 * @return @ref BSP_STATUS_OK.
 *****************************************************************************/
int BSP_McuBoard_DeInit(void)
{
#ifdef BSP_MCUBOARD_USB
  /* Disable GPIO port pin mode. */
  GPIO_PinModeSet(BSP_USB_STATUSLED_PORT, BSP_USB_STATUSLED_PIN, gpioModeDisabled, 0);
  GPIO_PinModeSet(BSP_USB_OCFLAG_PORT, BSP_USB_OCFLAG_PIN, gpioModeDisabled, 0);
  GPIO_PinModeSet(BSP_USB_VBUSEN_PORT, BSP_USB_VBUSEN_PIN, gpioModeDisabled, 0);
#endif

  return BSP_STATUS_OK;
}

/**************************************************************************//**
 * @brief Enable MCU plugin board peripherals.
 * @return @ref BSP_STATUS_OK.
 *****************************************************************************/
int BSP_McuBoard_Init(void)
{
#ifdef BSP_MCUBOARD_USB
  /* Make sure that the CMU clock to the GPIO peripheral is enabled  */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* USB status LED - configure PE1 as push pull */
  GPIO_PinModeSet(BSP_USB_STATUSLED_PORT, BSP_USB_STATUSLED_PIN, gpioModePushPull, 0);

  /* USB PHY overcurrent status input */
  GPIO_PinModeSet(BSP_USB_OCFLAG_PORT, BSP_USB_OCFLAG_PIN, gpioModeInput, 0);

  /* USB VBUS switch - configure PF5 as push pull - Default OFF */
  GPIO_PinModeSet(BSP_USB_VBUSEN_PORT, BSP_USB_VBUSEN_PIN, gpioModePushPull, 0);
#endif

  return BSP_STATUS_OK;
}

/**************************************************************************//**
 * @brief Set state of MCU plugin board USB status LED.
 * @param[in] enable Set to true to turn on LED, false to turn off.
 * @return @ref BSP_STATUS_OK on plugin boards with USB capability,
 *         @ref BSP_STATUS_NOT_IMPLEMENTED otherwise.
 *****************************************************************************/
int BSP_McuBoard_UsbStatusLedEnable(bool enable)
{
#ifdef BSP_MCUBOARD_USB
  if ( enable ) {
    GPIO_PinOutSet(BSP_USB_STATUSLED_PORT, BSP_USB_STATUSLED_PIN);
  } else {
    GPIO_PinOutClear(BSP_USB_STATUSLED_PORT, BSP_USB_STATUSLED_PIN);
  }

  return BSP_STATUS_OK;
#else

  (void)enable;
  return BSP_STATUS_NOT_IMPLEMENTED;
#endif
}

/**************************************************************************//**
 * @brief Get state MCU plugin board VBUS overcurrent flag.
 * @return True if overcurrent situation exist, false otherwise.
 *****************************************************************************/
bool BSP_McuBoard_UsbVbusOcFlagGet(void)
{
#ifdef BSP_MCUBOARD_USB
  bool flag;

  if ( !GPIO_PinInGet(BSP_USB_OCFLAG_PORT, BSP_USB_OCFLAG_PIN) ) {
    flag = true;
  } else {
    flag = false;
  }

  return flag;
#else

  return false;
#endif
}

/**************************************************************************//**
 * @brief Enable MCU plugin board VBUS power switch.
 * @param[in] enable Set to true to turn on VBUS power, false to turn off.
 * @return @ref BSP_STATUS_OK on plugin boards with USB capability,
 *         @ref BSP_STATUS_NOT_IMPLEMENTED otherwise.
 *****************************************************************************/
int BSP_McuBoard_UsbVbusPowerEnable(bool enable)
{
#ifdef BSP_MCUBOARD_USB
  GPIO_PinModeSet(BSP_USB_VBUSEN_PORT, BSP_USB_VBUSEN_PIN, gpioModePushPull, enable);

  return BSP_STATUS_OK;
#else

  (void)enable;
  return BSP_STATUS_NOT_IMPLEMENTED;
#endif
}

/** @} (end group BSP_DK) */
/** @} (end group BSP) */
