/*******************************************************************************
* @file rsi_hal_mcu_ioports.c
* @brief
*******************************************************************************
* # License
* <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com
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

/**
 * Includes
 */
#include "rsi_driver.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "sdiodrv.h"

/**
 * Global Variales
 */

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
void rsi_hal_config_gpio(uint8_t gpio_number,uint8_t mode,uint8_t value)
{
  UNUSED_PARAMETER(gpio_number); //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(mode); //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(value); //This statement is added only to resolve compilation warning, value is unchanged
  //! Initialise the gpio pins in input/output mode
  CMU_ClockEnable(cmuClock_GPIO, true);
   //! Initialise the gpio pins in input/output mode
   //!
  GPIO_PinModeSet(gpioPortA,12,gpioModePushPull,0);


   GPIO_PinModeSet(gpioPortA,13,gpioModeInput,0);


   GPIO_PinModeSet((GPIO_Port_TypeDef)SL_GPIO_PORT_B,11,gpioModePushPull,1);//Reset PB 11
   return;


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
void rsi_hal_set_gpio(uint8_t gpio_number)
{
  //! drives a high value on GPIO

   if(gpio_number ==  RSI_HAL_SLEEP_CONFIRM_PIN)
     {

       GPIO_PinModeSet(gpioPortA,12,gpioModePushPull,1);  // mapped to UULP_2

     }

   if(gpio_number ==  RSI_HAL_WAKEUP_INDICATION_PIN)
     {

       GPIO_PinModeSet(gpioPortA,13,gpioModeInput,1); // mapped to UULP_3
     }

  if(gpio_number ==  RSI_HAL_RESET_PIN)
    {
      GPIO_PinModeSet((GPIO_Port_TypeDef)SL_GPIO_PORT_B,11,gpioModePushPull,1);
    }
  return;
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
uint8_t rsi_hal_get_gpio(uint8_t gpio_number)
{
  uint8_t gpio_value = 0;
  //! Get the gpio value

  if(gpio_number == RSI_HAL_SLEEP_CONFIRM_PIN)
    {
      gpio_value = GPIO_PinInGet(gpioPortA,12);
    }

  if(gpio_number == RSI_HAL_WAKEUP_INDICATION_PIN)
    {
      gpio_value =  GPIO_PinInGet(gpioPortA,13);
    }
  if(gpio_number ==  RSI_HAL_MODULE_INTERRUPT_PIN)
      {
        gpio_value =  GPIO_PinInGet((GPIO_Port_TypeDef)SL_GPIO_PORT_E, 10);
    //  gpio_value = GPIO_PinModeGet(SL_GPIO_PORT_E, 10);
      }

  return gpio_value;

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
void rsi_hal_clear_gpio(uint8_t gpio_number)
{

  //! drives a low value on GPIO 
  if(gpio_number ==  RSI_HAL_SLEEP_CONFIRM_PIN)
     {
       GPIO_PinOutClear(gpioPortA,12);
     }

   if(gpio_number ==  RSI_HAL_WAKEUP_INDICATION_PIN)
     {
       GPIO_PinOutClear(gpioPortA,13);
     }

   if(gpio_number ==  RSI_HAL_LP_SLEEP_CONFIRM_PIN)
     {
       GPIO_PinOutClear(gpioPortA,12);
     }
  if(gpio_number ==  RSI_HAL_RESET_PIN)
      {
        GPIO_PinOutClear((GPIO_Port_TypeDef)SL_GPIO_PORT_B,11);
      }

  
  return;
}


