/***************************************************************************//**
 * @file
 * @brief Configuration header for bootloader EZSP GPIO Activation
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
#ifndef BTL_EZSP_GPIO_ACTIVATION_CONFIG_H
#define BTL_EZSP_GPIO_ACTIVATION_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Properties of SPI NCP

// <o SL_EZSP_GPIO_ACTIVATION_POLARITY> Active state
// <LOW=> Low
// <HIGH=> High
// <i> Default: LOW
// <i> Enter firmware upgrade mode if GPIO pin has this state
#define SL_EZSP_GPIO_ACTIVATION_POLARITY       LOW

// </h>

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <gpio> SL_EZSPSPI_HOST_INT
// $[GPIO_SL_EZSPSPI_HOST_INT]
#define SL_EZSPSPI_HOST_INT_PORT                 gpioPortA
#define SL_EZSPSPI_HOST_INT_PIN                  4

// [GPIO_SL_EZSPSPI_HOST_INT]$

// <gpio> SL_EZSPSPI_WAKE_INT
// $[GPIO_SL_EZSPSPI_WAKE_INT]
#define SL_EZSPSPI_WAKE_INT_PORT                 gpioPortD
#define SL_EZSPSPI_WAKE_INT_PIN                  10

// [GPIO_SL_EZSPSPI_WAKE_INT]$

// <<< sl:end pin_tool >>>

#endif // BTL_EZSP_GPIO_ACTIVATION_CONFIG_H
