/*******************************************************************************
* @file  rsi_hal_mcu_interrupt.c
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
#ifdef RSI_WITH_OS
#include <projdefs.h>
#include "FreeRTOSConfig.h"
#include "portmacro.h"
#endif
#include "board.h"
#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "fsl_sdio.h"

extern volatile int sdio_init_done ;

#ifdef WAKEUP_GPIO_INTERRUPT_METHOD
void APP_GPIO_INTA_IRQHandler(void)
{

  rsi_give_wakeup_indication(); 
 
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif    
}

/*===================================================*/
/**
 * @fn           void rsi_give_wakeup_indication(void)
 * @brief        isr to wakeup indication
 * @param[in]    none
 * @param[out]   none
 * @return       none
 * @description  This HAL API should post the semaphore .
 */

void rsi_give_wakeup_indication(void)
{
  if(rsi_hal_get_gpio(RSI_HAL_WAKEUP_INDICATION_PIN))
  {
   rsi_hal_gpio_clear();
   rsi_hal_gpio_mask();
#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap &= ~BIT(4);
#endif
   rsi_semaphore_post_from_isr(&rsi_driver_cb->common_cb->wakeup_gpio_sem);
  }
}
#endif
#if 0 
/*===================================================*/
/**
 * @fn           void rsi_hal_enable_uart_irq(void)
 * @brief        Enables the UART interrupt
 * @param[in]    none  
 * @param[out]   none
 * @return       none
 * @description  This HAL API enables UART interrupts.
 */
void rsi_hal_enable_uart_irq(void)
{
  //! Enable uart interrupt
  EnableIRQ(UART_INTERRUPT_SW_IRQ);
  return;
}
#endif 
/*===================================================*/
/**
 * @fn           void rsi_hal_intr_config(void (* rsi_interrupt_handler)())
 * @brief        Starts and enables the SPI interrupt
 * @param[in]    rsi_interrupt_handler() ,call back function to handle interrupt
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to initialize the register/pins
 *               related to interrupts and enable the interrupts.
 */
void rsi_hal_intr_config(void (* rsi_interrupt_handler)())
{
  //! Configure interrupt pin/register in input mode and register the interrupt handler
  // Pin config is handled in pin_mux.c
  // Interrupt is fixed to PORTE_IRQhandler
  return;

}
#ifdef WAKEUP_GPIO_INTERRUPT_METHOD
/*===================================================*/
/**
 * @fn           void rsi_hal_gpio_mask(void)
 * @brief        Disables the SPI Interrupt
 * @param[in]    none
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to mask/disable interrupts.
 */
void rsi_hal_gpio_mask(void)
{
  //! Mask/Disable the interrupt
  DisableIRQ(WAKEUP_GPIO_SW_IRQ);
  return;

}
/*===================================================*/
/**
 * @fn           void rsi_hal_gpio_unmask(void)
 * @brief        Enables the SPI interrupt
 * @param[in]    none  
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to enable interrupts.
 */
 
void rsi_hal_gpio_unmask(void)
{
  //! Unmask/Enable the interrupt
  EnableIRQ(WAKEUP_GPIO_SW_IRQ);
  return;

}
/*===================================================*/
/**
 * @fn           void rsi_hal_gpio_clear(void)
 * @brief        Clears the pending interrupt
 * @param[in]    none
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to clear the handled interrupts.
 */
void rsi_hal_gpio_clear(void)
{
   //! Clear the interrupt
   GPIO_PinClearInterruptFlag(GPIO,BOARD_INITPINS_PS_IN_PORT, BOARD_INITPINS_PS_IN_PIN,0);
   return;
}
#endif

/*===================================================*/
/** 
 * @fn           void rsi_hal_intr_mask(void)
 * @brief        Disables the SPI Interrupt
 * @param[in]    none
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to mask/disable interrupts.
 */
void rsi_hal_intr_mask(void)
{
  //! Mask/Disable the interrupt      
  USDHC_DisableInterruptStatus(SD_HOST_BASEADDR1, kUSDHC_CardInterruptFlag);
  return;

}


/*===================================================*/
/**
 * @fn           void rsi_hal_intr_unmask(void)
 * @brief        Enables the SPI interrupt
 * @param[in]    none  
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to enable interrupts.
 */
void rsi_hal_intr_unmask(void)
{
  //! Unmask/Enable the interrupt  
  SDMMCHOST_ENABLE_SDIO_INT(SD_HOST_BASEADDR1) ; 
  return;       

}

/*===================================================*/
/**
 * @fn           void rsi_hal_enable_uart_irq(void)
 * @brief        Enables the UART interrupt
 * @param[in]    none  
 * @param[out]   none
 * @return       none
 * @description  This HAL API enables UART interrupts.
 */
void rsi_hal_enable_uart_irq(void)
{
  //! Enable uart interrupt
  EnableIRQ(UART_INTERRUPT_SW_IRQ);
  return;
}

/*===================================================*/
/**
 * @fn           void rsi_hal_intr_clear(void)
 * @brief        Clears the pending interrupt
 * @param[in]    none
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to clear the handled interrupts.
 */
void rsi_hal_intr_clear(void)
{
   //! Clear the interrupt
   return;

 
}


/*===================================================*/
/**
 * @fn          void rsi_hal_intr_pin_status(void)
 * @brief       Checks the SPI interrupt at pin level
 * @param[in]   none  
 * @param[out]  uint8_t, interrupt status 
 * @return      none
 * @description This API is used to check interrupt pin status(pin level whether it is high/low).
 */	
uint8_t rsi_hal_intr_pin_status(void)
{
  volatile uint8_t status = 1;
  if (sdio_init_done)
  {    //! Return interrupt pin  status(high(1) /low (0))
  status = rsi_hal_get_gpio(RSI_HAL_MODULE_INTERRUPT_PIN);
  }
  return status;
}

/*===================================================*/
/**
 * @fn           rsi_reg_flags_t rsi_hal_critical_section_entry(void)
 * @brief        hold interrupt status and disables the SDIO interrupt
 * @param[in]    none  
 * @param[out]   none
 * @return       stored interrupt status
 * @description  This HAL API should contain the code to hold interrupt status and disable interrupts.
 */
uint32_t rsi_hal_critical_section_entry(void)
{
	// hold interrupt status before entering critical section
	
	// disable interrupts	

	// return stored interrupt status
	return 0;
}

/*===================================================*/
/**
 * @fn           void rsi_hal_critical_section_exit(void)
 * @brief        Enables the SDIO interrupt
 * @param[in]    none  
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to enable interrupts.
 */
void rsi_hal_critical_section_exit(void)
{
	// restore interrupts while exiting critical section
}


