/*******************************************************************************
* @file  rsi_hal_mcu_ioports.c
* @brief
*******************************************************************************
* # License
* <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "rsi_driver.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "rsi_board_configuration.h"


/*===========================================================*/
/**
 * @fn            void rsi_hal_config_gpio(uint8_t gpio_number,uint8_t mode,uint8_t value)
 * @brief         Configures gpio pin in output mode,with a value
 * @param[in]     uint8_t gpio_number, gpio pin number to be configured
 * @param[in]     uint8_t mode , input/output mode of the gpio pin to configure
 *                0 - input mode
 *                1 - output mode
 * @param[in]     uint8_t value, default value to be driven if gpio is configured in output mode
 *                0 - low
 *                1 - high
 * @param[out]    none
 * @return        none
 * @description This API is used to configure host gpio pin in output mode.
 */
void rsi_hal_config_gpio(uint8_t gpio_number, uint8_t mode, uint8_t value) {
  UNUSED_PARAMETER(mode); //This statement is added only to resolve compilation warnings, value is unchanged
  UNUSED_PARAMETER(value); //This statement is added only to resolve compilation warnings, value is unchanged
  CMU_ClockEnable(cmuClock_GPIO, true);

  switch(gpio_number)
  {
     case RSI_HAL_SLEEP_CONFIRM_PIN:     GPIO_PinModeSet(SLEEP_CONFIRM_PIN.port, SLEEP_CONFIRM_PIN.pin, gpioModeWiredOrPullDown, 1); break;
#ifndef LOGGING_STATS
     case RSI_HAL_WAKEUP_INDICATION_PIN: GPIO_PinModeSet(WAKE_INDICATOR_PIN.port, WAKE_INDICATOR_PIN.pin, gpioModeWiredOrPullDown, 0); break;
#endif
     case RSI_HAL_RESET_PIN:             GPIO_PinModeSet(RESET_PIN.port, RESET_PIN.pin, gpioModePushPull, 1); break;
     case RSI_HAL_LP_SLEEP_CONFIRM_PIN:  GPIO_PinModeSet(SLEEP_CONFIRM_PIN.port, SLEEP_CONFIRM_PIN.pin, gpioModeWiredOrPullDown, 1);break;
     default: break;
  }
}

/*===========================================================*/
/**
 * @fn            void rsi_hal_set_gpio(uint8_t gpio_number)
 * @brief         Makes/drives the gpio  value high
 * @param[in]     uint8_t gpio_number, gpio pin number
 * @param[out]    none
 * @return        none
 * @description   This API is used to drives or makes the host gpio value high.
 */
void rsi_hal_set_gpio(uint8_t gpio_number) {
  switch(gpio_number)
  {
    case RSI_HAL_SLEEP_CONFIRM_PIN:     GPIO_PinModeSet(SLEEP_CONFIRM_PIN.port, SLEEP_CONFIRM_PIN.pin, gpioModeWiredOrPullDown, 1); break;
#ifndef LOGGING_STATS
   case RSI_HAL_WAKEUP_INDICATION_PIN: GPIO_PinModeSet(WAKE_INDICATOR_PIN.port, WAKE_INDICATOR_PIN.pin, gpioModeInput, 1); break;
#else
   case RSI_HAL_WAKEUP_INDICATION_PIN: GPIO_PinModeSet(LOGGING_WAKE_INDICATOR_PIN.port, LOGGING_WAKE_INDICATOR_PIN.pin, gpioModeInput, 1); break;
#endif
    case RSI_HAL_RESET_PIN:             GPIO_PinModeSet(RESET_PIN.port, RESET_PIN.pin, gpioModeWiredOrPullDown, 1); break;
    case RSI_HAL_LP_SLEEP_CONFIRM_PIN: GPIO_PinModeSet(SLEEP_CONFIRM_PIN.port, SLEEP_CONFIRM_PIN.pin, gpioModeWiredOrPullDown, 1);break;
    default: break;
  }
}

/*===========================================================*/
/**
 * @fn          uint8_t rsi_hal_get_gpio(void)
 * @brief       get the gpio pin value
 * @param[in]   uint8_t gpio_number, gpio pin number
 * @param[out]  none
 * @return      gpio pin value
 * @description This API is used to configure get the gpio pin value.
 */
uint8_t rsi_hal_get_gpio(uint8_t gpio_number) {
  switch(gpio_number)
  {
    case RSI_HAL_SLEEP_CONFIRM_PIN:     return GPIO_PinInGet(SLEEP_CONFIRM_PIN.port, SLEEP_CONFIRM_PIN.pin);
#ifndef LOGGING_STATS
    case RSI_HAL_WAKEUP_INDICATION_PIN: return GPIO_PinInGet(WAKE_INDICATOR_PIN.port, WAKE_INDICATOR_PIN.pin);
#else
    case RSI_HAL_WAKEUP_INDICATION_PIN: return GPIO_PinInGet(LOGGING_WAKE_INDICATOR_PIN.port, LOGGING_WAKE_INDICATOR_PIN.pin);
#endif
    case RSI_HAL_RESET_PIN:             return GPIO_PinInGet(RESET_PIN.port, RESET_PIN.pin);
    case RSI_HAL_MODULE_INTERRUPT_PIN:  return GPIO_PinInGet(INTERRUPT_PIN.port, INTERRUPT_PIN.pin);
    case RSI_HAL_LP_SLEEP_CONFIRM_PIN: return GPIO_PinInGet(SLEEP_CONFIRM_PIN.port, SLEEP_CONFIRM_PIN.pin);
    default: break;
  }

  return 0;
}

/*===========================================================*/
/**
 * @fn            void rsi_hal_set_gpio(uint8_t gpio_number)
 * @brief         Makes/drives the gpio value to low
 * @param[in]     uint8_t gpio_number, gpio pin number
 * @param[out]    none
 * @return        none
 * @description   This API is used to drives or makes the host gpio value low.
 */
void rsi_hal_clear_gpio(uint8_t gpio_number) {
  switch(gpio_number)
  {
    case RSI_HAL_SLEEP_CONFIRM_PIN:
         GPIO_PinOutClear(SLEEP_CONFIRM_PIN.port, SLEEP_CONFIRM_PIN.pin); break;
#ifndef LOGGING_STATS
      //fall through
    case RSI_HAL_WAKEUP_INDICATION_PIN:
      GPIO_PinOutClear(WAKE_INDICATOR_PIN.port, WAKE_INDICATOR_PIN.pin); break;
#else
     //fall through
    case RSI_HAL_WAKEUP_INDICATION_PIN:
      GPIO_PinOutClear(LOGGING_WAKE_INDICATOR_PIN.port, LOGGING_WAKE_INDICATOR_PIN.pin); break;
#endif
    //fall through
    case RSI_HAL_RESET_PIN:              GPIO_PinOutClear(RESET_PIN.port, RESET_PIN.pin); break;
    //fall through
    case RSI_HAL_LP_SLEEP_CONFIRM_PIN:   GPIO_PinOutClear(SLEEP_CONFIRM_PIN.port, SLEEP_CONFIRM_PIN.pin); break;
    //fall through
    default: break;
  }
}
