/*******************************************************************************
* @file  rsi_hal_mcu_ioports.c
* @brief 
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



/**
 * Includes
 */
#include "rsi_driver.h"
/**
 * Global Variables
 */
#include "pin_mux.h"
#include "board.h"

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

	// all configuration is in pin_mux.c
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
  //! module reset pin
  if(gpio_number == RSI_HAL_RESET_PIN)
  { 
    GPIO_PinWrite(GPIO, BOARD_INITPINS_RESET_PORT,BOARD_INITPINS_RESET_PIN, 1);  
  }
  else if( gpio_number == RSI_HAL_SLEEP_CONFIRM_PIN)
  {
    GPIO_PinWrite(GPIO, BOARD_INITPINS_PS_OUT_PORT,BOARD_INITPINS_PS_OUT_PIN, 1);
  }

  //! Setting power save input GPIO to 1
  else if( gpio_number == RSI_HAL_WAKEUP_INDICATION_PIN)
  {
    GPIO_PinWrite(GPIO, BOARD_INITPINS_PS_IN_PORT,BOARD_INITPINS_PS_IN_PIN, 1);
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
  if(gpio_number == RSI_HAL_RESET_PIN)
  {
    gpio_value = GPIO_PinRead(GPIO,BOARD_INITPINS_RESET_PORT, BOARD_INITPINS_RESET_PIN);  
  }

  //! SPI interrupt pin
  if(gpio_number == RSI_HAL_MODULE_INTERRUPT_PIN)
  {
       /* Reads the SDIO interrupt pin value */
   gpio_value = GPIO_PinRead(GPIO,SD1_D1_PORT,SD1_D1_PIN); 
  }
  //! Get PS sleep gpio value
  else if( gpio_number == RSI_HAL_SLEEP_CONFIRM_PIN)
  {
    gpio_value = GPIO_PinRead(GPIO,BOARD_INITPINS_PS_OUT_PORT, BOARD_INITPINS_PS_OUT_PIN);
  }
  //! Get PS wakeup gpio value
  else if ( gpio_number == RSI_HAL_WAKEUP_INDICATION_PIN )
  {
    gpio_value = GPIO_PinRead(GPIO, BOARD_INITPINS_PS_IN_PORT, BOARD_INITPINS_PS_IN_PIN);
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
  if(gpio_number == RSI_HAL_RESET_PIN)
  { 
    GPIO_PinWrite(GPIO, BOARD_INITPINS_RESET_PORT,BOARD_INITPINS_RESET_PIN, 0);  
  }

  //! Setting power save output GPIO to 1
  else if( gpio_number == RSI_HAL_SLEEP_CONFIRM_PIN)
  {
    GPIO_PinWrite(GPIO, BOARD_INITPINS_PS_OUT_PORT,BOARD_INITPINS_PS_OUT_PIN, 0);
  }

  //! Setting power save input GPIO to 1
  else if( gpio_number == RSI_HAL_WAKEUP_INDICATION_PIN)
  {
    GPIO_PinWrite(GPIO, BOARD_INITPINS_PS_IN_PORT,BOARD_INITPINS_PS_IN_PIN, 0);
  }
  return;
}



