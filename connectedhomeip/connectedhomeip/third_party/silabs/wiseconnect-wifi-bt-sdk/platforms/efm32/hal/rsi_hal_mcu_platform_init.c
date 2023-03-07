/*******************************************************************************
* @file rsi_hal_mcu_platform_init.c
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
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_ldma.h"
#include "sl_iostream.h"
#include "sl_iostream_init_instances.h"
#include "sl_iostream_handles.h"
#include "sl_event_handler.h"
#include "sl_rsi_host_api.h"

//! systick interrupt priority
#define SYSTICK_INTR_PRI ((1<<__NVIC_PRIO_BITS)-1)
int platform_initialized = 0;
sl_status_t sl_device_init_dcdc(void);
sl_status_t sl_device_init_hfxo(void);
sl_status_t sl_device_init_clocks(void);
void sl_board_init(void);
void sl_system_init(void);

void app_iostream_usart_init(void)
{
  /* Prevent buffering of output/input.*/
#if !defined(__CROSSWORKS_ARM) && defined(__GNUC__)
  setvbuf(stdout, NULL, _IONBF, 0);   /*Set unbuffered mode for stdout (newlib)*/
  setvbuf(stdin, NULL, _IONBF, 0);   /*Set unbuffered mode for stdin (newlib)*/
#endif

  /* Output on vcom usart instance */
  const char str1[] = "IOstream USART example\r\n\r\n";
  sl_iostream_write(sl_iostream_vcom_handle, str1, strlen(str1));

  /* Setting default stream */
  sl_iostream_set_default(sl_iostream_vcom_handle);
  const char str2[] = "This is output on the default stream\r\n";
  sl_iostream_write(SL_IOSTREAM_STDOUT, str2, strlen(str2));


  /* Using printf */
  /* Writing ASCII art to the VCOM iostream */
  printf("Printf uses the default stream, as long as iostream_retarget_stdio is included.\r\n");
}

/*==============================================*/
/**
 * @fn           void rsi_hal_board_init()
 * @brief        This function Initializes the platform
 * @param[in]    none 
 * @param[out]   none
 * @return       none
 * @section description
 * This function initializes the platform
 *
 */

void rsi_hal_board_init(void)
{
  //! Initializes the platform
if(!platform_initialized)
  {
#ifndef RSI_WITH_OS
   sl_system_init();
#else
   sl_device_init_dcdc();
   sl_device_init_hfxo();
   sl_device_init_clocks();
#endif

  CHIP_Init();
  SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000);
  sl_board_init();
  sl_service_init();

#ifdef RSI_WITH_OS
  // Set Systick Intr priority
  NVIC_SetPriority(SysTick_IRQn, SYSTICK_INTR_PRI);
#endif

//Debug enable
  app_iostream_usart_init();
  platform_initialized = 1;
  }
}


/*==============================================*/
/**
 * @fn           void rsi_switch_to_high_clk_freq()
 * @brief        This function intializes SPI to high clock
 * @param[in]    none 
 * @param[out]   none
 * @return       none
 * @section description
 * This function intializes SPI to high clock
 *
 *
 */

void rsi_switch_to_high_clk_freq(void)
{
  //! Initializes the high clock
}

void rsi_sdio_hal_efm_deinit(void)
{
  sl_rsi_host_deinit_bus();
}



