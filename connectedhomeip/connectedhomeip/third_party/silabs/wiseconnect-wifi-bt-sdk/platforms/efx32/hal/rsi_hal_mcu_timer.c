/*******************************************************************************
* @file  rsi_hal_mcu_timer.c
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

/**
 * Includes
 */

#include "rsi_driver.h"
#include "em_cmu.h"
#ifdef RSI_WITH_OS
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#if defined(SysTick)
#undef SysTick_Handler
/* FreeRTOS SysTick interrupt handler prototype */
extern void SysTick_Handler     (void);
/* FreeRTOS tick timer interrupt handler prototype */
extern void xPortSysTickHandler (void);
#endif  /* SysTick */

#endif
/* Counts 1ms timeTicks */
volatile uint32_t msTicks = 0;

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
  UNUSED_PARAMETER(timer_node); //This statement is added only to resolve compilation warnings, value is unchanged
  UNUSED_PARAMETER(mode); //This statement is added only to resolve compilation warnings, value is unchanged
  UNUSED_PARAMETER(type); //This statement is added only to resolve compilation warnings, value is unchanged
  UNUSED_PARAMETER(duration); //This statement is added only to resolve compilation warnings, value is unchanged
  UNUSED_PARAMETER(rsi_timer_expiry_handler); //This statement is added only to resolve compilation warnings, value is unchanged
  // Initialise the timer


  // register the call back


  // Start timer

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
  UNUSED_PARAMETER(timer_node); //This statement is added only to resolve compilation warnings, value is unchanged
  // Stop the timer

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
  UNUSED_PARAMETER(timer_node); //This statement is added only to resolve compilation warnings, value is unchanged
  volatile uint32_t timer_val = 0;

  // read the timer and return timer value

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
  UNUSED_PARAMETER(delay_us); //This statement is added only to resolve compilation warnings, value is unchanged
  // call the API for delay in micro seconds

  return;

}


#ifdef RSI_M4_INTERFACE

extern void SysTick_Handler(void);

void SysTick_Handler(void)
{
  _dwTickCount++;
}
uint32_t GetTickCount( void )
{
  return _dwTickCount ;						// gets the tick count from systic ISR
}
#endif

/*===================================================*/
/**
 * @fn           void rsi_delay_ms(uint32_t delay)
 * @brief        create delay in milli seconds
 * @param[in]    delay, timer delay in milli seconds
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to create delay in milli seconds
 */
void rsi_delay_ms(uint32_t delay_ms)
{
#ifndef RSI_WITH_OS
   uint32_t start;
#endif
  if (delay_ms == 0)
    return;

#ifdef RSI_WITH_OS
  vTaskDelay(delay_ms);
#else
  start = rsi_hal_gettickcount();
  do {
  } while (rsi_hal_gettickcount() - start < delay_ms);
#endif
}

/*===================================================*/
/**
 * @fn           uint32_t rsi_hal_gettickcount()
 * @brief        provides a tick value in milliseconds
 * @return       tick value
 * @description  This HAL API should contain the code to read the timer tick count value in milliseconds
 *
 */

#ifndef RSI_HAL_USE_RTOS_SYSTICK
/*
  SysTick handler implementation that also clears overflow flag.
*/
void SysTick_Handler(void)
{
  /* Increment counter necessary in Delay()*/
  msTicks++;
#ifdef RSI_WITH_OS
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
    xPortSysTickHandler();
  }
#endif
}

uint32_t rsi_hal_gettickcount(void)
{
  return msTicks;

#ifdef LINUX_PLATFORM
  // Define your API to get the tick count delay in milli seconds from systic ISR and return the resultant value
  struct rsi_timeval tv1;
  gettimeofday(&tv1, NULL);
  return (tv1.tv_sec * 1000 + tv1.tv_usec/1000);
#endif
}
#else
uint32_t rsi_hal_gettickcount(void)
{
  return xTaskGetTickCount();
}
#endif
