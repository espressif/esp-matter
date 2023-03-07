/***************************************************************************//**
 * @file
 * @brief EZSP GPIO Activation component for Silicon Labs bootloader.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#ifndef BTL_EZSP_GPIO_ACTIVATION_H
#define BTL_EZSP_GPIO_ACTIVATION_H

/***************************************************************************//**
 * @addtogroup Components
 * @{
 * @addtogroup GpioActivation
 * @{
 * @addtogroup EZSPGPIO EZSP GPIO
 * @brief Enter bootloader based on EZSP GPIO state.
 * @details
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Enter the bootlader if the GPIO pin is active.
 *
 * @return True if the bootloader should be entered
 ******************************************************************************/
bool ezsp_gpio_enterBootloader(void);

/** @} (end addtogroup EZSP GPIO) */
/** @} (end addtogroup GpioActivation) */
/** @} (end addtogroup Components) */

#endif // BTL_EZSP_GPIO_ACTIVATION_H
