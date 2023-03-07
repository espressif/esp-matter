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

#include "config/btl_config.h"

#include "em_device.h"
#include "em_gpio.h"
#include "btl_ezsp_gpio_activation.h"
#include "em_cmu.h"
#include "btl_ezsp_gpio_activation_cfg.h"

// Map GPIO activation polarity settings to GPIO pin states
#define HIGH 0
#define LOW  1

bool ezsp_gpio_enterBootloader(void)
{
  bool pressed;

#if defined(CMU_HFBUSCLKEN0_GPIO)
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);
#endif
#if defined(_CMU_CLKEN0_MASK)
  // Enable GPIO clock
  CMU->CLKEN0_SET = CMU_CLKEN0_GPIO;
#endif

  // Reconfigure as an input with pull(up|down) to read the nWake state
  GPIO_PinModeSet(SL_EZSPSPI_WAKE_INT_PORT,
                  SL_EZSPSPI_WAKE_INT_PIN,
                  gpioModeInputPull,
                  SL_EZSP_GPIO_ACTIVATION_POLARITY);

  pressed = GPIO_PinInGet(SL_EZSPSPI_WAKE_INT_PORT, SL_EZSPSPI_WAKE_INT_PIN)
            != SL_EZSP_GPIO_ACTIVATION_POLARITY;

  // Disable GPIO pin
  GPIO_PinModeSet(SL_EZSPSPI_WAKE_INT_PORT,
                  SL_EZSPSPI_WAKE_INT_PIN,
                  gpioModeDisabled,
                  SL_EZSP_GPIO_ACTIVATION_POLARITY);

#if defined(CMU_HFBUSCLKEN0_GPIO)
  // Disable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, false);
#endif

  return pressed;
}
