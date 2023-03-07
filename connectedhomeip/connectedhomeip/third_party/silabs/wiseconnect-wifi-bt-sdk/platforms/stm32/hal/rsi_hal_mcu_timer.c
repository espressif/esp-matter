/*******************************************************************************
* @file  rsi_hal_mcu_timer.c
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
 * @file       rsi_hal_mcu_timer.c
 * @version    0.1
 * @date       15 Aug 2015
 *
 *
 *
 * @brief HAL TIMER: Functions related to HAL timers
 *
 * @section Description
 * This file contains the list of functions for configuring the microcontroller timers.
 * Following are list of API's which need to be defined in this file.
 *
 */


/**
 * Includes
 */

#include "rsi_driver.h"
#include "stm32f4xx_hal.h"
#ifdef RSI_WITH_OS
#include "FreeRTOS.h"
#include "task.h"
#endif


/*===================================================*/
/**
 * @fn           int32_t rsi_timer_start(uint8_t timer_no, uint8_t mode,uint8_t type,uint32_t duration,void (* rsi_timer_expiry_handler)())
 * @brief        Starts and configures timer
 * @param[in]    timer_node, timer node to be configured.
 * @param[in]    mode , mode of the timer
 *               0 - Micro seconds mode
 *               1 - Milli seconds mode
 * @param[in]    type, type of  the timer
 *               0 - single shot type
 *               1 - periodic type
 * @param[in]    duration, timer duration
 * @param[in]    rsi_timer_expiry_handler() ,call back function to handle timer interrupt
 * @param[out]   none
 * @return       0 - success
 *               !0 - Failure
 * @description  This HAL API should contain the code to initialize the timer and start the timer
 *
 */

int32_t rsi_timer_start(uint8_t timer_node, uint8_t mode, uint8_t type, uint32_t duration, void (* rsi_timer_expiry_handler)(void))
{

	UNUSED_PARAMETER(timer_node);//This statement is added only to resolve compilation warning, value is unchanged
	UNUSED_PARAMETER(mode);//This statement is added only to resolve compilation warning, value is unchanged
	UNUSED_PARAMETER(type);//This statement is added only to resolve compilation warning, value is unchanged
	UNUSED_PARAMETER(duration);//This statement is added only to resolve compilation warning, value is unchanged
	UNUSED_PARAMETER(rsi_timer_expiry_handler);//This statement is added only to resolve compilation warning, value is unchanged


  //! Initialise the timer


  //! register the call back


  //! Start timer

  return 0;
}


/*===================================================*/
/**
 * @fn           int32_t rsi_timer_stop(uint8_t timer_no)
 * @brief        Stops timer
 * @param[in]    timer_node, timer node to stop
 * @param[out]   none
 * @return       0 - success
 *               !0 - Failure
 * @description  This HAL API should contain the code to stop the timer
 *
 */

int32_t rsi_timer_stop(uint8_t timer_node)
{
	UNUSED_PARAMETER(timer_node);//This statement is added only to resolve compilation warning, value is unchanged
	//! Stop the timer

	return 0;


}




/*===================================================*/
/**
 * @fn           uint32_t rsi_timer_read(uint8_t timer_node)
 * @brief        read timer
 * @param[in]    timer_node, timer node to read
 * @param[out]   none
 * @return       timer value
 * @description  This HAL API should contain API to  read the timer
 *
 */

uint32_t rsi_timer_read(uint8_t timer_node)
{
	UNUSED_PARAMETER(timer_node);//This statement is added only to resolve compilation warning, value is unchanged
	volatile uint32_t timer_val = 0;

	//! read the timer and return timer value

	return timer_val;


}






/*===================================================*/
/**
 * @fn           void rsi_delay_us(uint32_t delay)
 * @brief        create delay in micro seconds
 * @param[in]    delay_us, timer delay in micro seconds
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to create delay in micro seconds
 *
 */
void rsi_delay_us(uint32_t delay_us)
{

	UNUSED_PARAMETER(delay_us);//This statement is added only to resolve compilation warning, value is unchanged
    //! call the API for delay in micro seconds

	return;

}




/*===================================================*/
/**
 * @fn           void rsi_delay_ms(uint32_t delay)
 * @brief        create delay in micro seconds
 * @param[in]    delay, timer delay in micro seconds
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to create delay in micro seconds
 *
 */
void rsi_delay_ms(uint32_t delay_ms)
{
#ifdef RSI_WITH_OS
  rsi_os_task_delay(delay_ms);
#else
	//! call the API for delay in milli seconds
	uint32_t start;
	if (delay_ms == 0)
		return;
	 start = rsi_hal_gettickcount();
	do {
	} while (rsi_hal_gettickcount() - start < delay_ms);
	return;
#endif
}

#ifdef RSI_WITH_OS
/*===================================================*/
/**
 * @fn           uint32_t HAL_GetTick()
 * @brief        provides a tick value in milliseconds
 * @return       tick value
 * @description  This HAL API should contain the code to read the timer tick count value in milliseconds
 *
 */
 uint32_t HAL_GetTick(void)
{
  return xTaskGetTickCount();
}
#endif

/*===================================================*/
/**
 * @fn           uint32_t rsi_hal_gettickcount()
 * @brief        provides a tick value in milliseconds
 * @return       tick value
 * @description  This HAL API should contain the code to read the timer tick count value in milliseconds
 *
 */

uint32_t rsi_hal_gettickcount()
{
  return HAL_GetTick();
}


#ifdef RSI_WITH_OS
void rsi_os_delay_ms(uint32_t delay_ms)
{

	UNUSED_PARAMETER(delay_ms);//This statement is added only to resolve compilation warning, value is unchanged
//	osDelay(delay_ms);
//volatile unsigned long  i = 0;
//volatile unsigned long  j =  4000 * delay_ms;
//for(i = 0; i < j; i++)
//{
//	__asm("NOP");
//}
}

#endif






