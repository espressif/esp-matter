/***************************************************************************//**
 * @file app_init.c
 * @brief Application initialization functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_init.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * The function is used for some basic initialization relates to the
 * application.
 ******************************************************************************/
void app_init(void)
{
  // Enable cycle counter
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  // High drive for SWCLK and SWD, both must on the same GPIO port
  GPIO_SlewrateSet((GPIO_Port_TypeDef)SWCLK_PORT, 6, 6);

  // Initialize GPIO for SWCLK and SWDIO pin
  GPIO_PinModeSet((GPIO_Port_TypeDef)SWCLK_PORT, SWCLK_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet((GPIO_Port_TypeDef)SWDIO_PORT, SWDIO_PIN, gpioModePushPull, 1);

  // Initialize GPIO for RESET pin
  GPIO_PinModeSet((GPIO_Port_TypeDef)RESET_PORT, RESET_PIN, gpioModePushPull, 1);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
