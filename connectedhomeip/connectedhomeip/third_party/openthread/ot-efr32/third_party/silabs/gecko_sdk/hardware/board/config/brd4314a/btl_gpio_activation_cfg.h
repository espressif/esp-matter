/***************************************************************************//**
 * @file
 * @brief Configuration header for bootloader GPIO Activation
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
#ifndef BTL_GPIO_ACTIVATION_CONFIG_H
#define BTL_GPIO_ACTIVATION_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Properties of Bootloader Entry


// <o SL_GPIO_ACTIVATION_POLARITY> Active state
// <LOW=> Low
// <HIGH=> High
// <i> Default: LOW
// <i> Enter firmware upgrade mode if GPIO pin has this state
#define SL_GPIO_ACTIVATION_POLARITY       LOW

// </h>

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>

// <gpio> SL_BTL_BUTTON

// $[GPIO_SL_BTL_BUTTON]
#define SL_BTL_BUTTON_PORT                       gpioPortC
#define SL_BTL_BUTTON_PIN                        7

// [GPIO_SL_BTL_BUTTON]$

// <<< sl:end pin_tool >>>


#endif // BTL_GPIO_ACTIVATION_CONFIG_H
