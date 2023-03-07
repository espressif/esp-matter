/*******************************************************************************
* @file  rsi_os_none.c
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
#include <rsi_driver.h>
#ifndef RSI_WITH_OS
/*
  Include files
 */

/** @addtogroup DRIVER16
* @{
*/
/*==============================================*/
/**
 * @fn          rsi_reg_flags_t rsi_critical_section_entry(void)
 */

rsi_reg_flags_t rsi_critical_section_entry(void)
{
  rsi_reg_flags_t xflags;
  // hold interrupt status before entering critical section
  // disable interrupts
#ifdef RSI_M4_INTERFACE
  xflags = NVIC_GetIRQEnable((IRQn_Type)M4_ISR_IRQ);
  NVIC_DisableIRQ((IRQn_Type)M4_ISR_IRQ);
#else
  xflags = rsi_hal_critical_section_entry();
#endif

  // return stored interrupt status
  return (xflags);
}
/*==============================================*/
/**
 * @fn         void rsi_critical_section_exit(rsi_reg_flags_t xflags)
 *
 */

void rsi_critical_section_exit(rsi_reg_flags_t xflags)
{
  // restore interrupts while exiting critical section
  if (xflags != 0) {
#ifdef RSI_M4_INTERFACE
    // restore interrupts while exiting critical section
    NVIC_EnableIRQ((IRQn_Type)M4_ISR_IRQ);
#else
    rsi_hal_critical_section_exit();
#endif
  }
}
/*==============================================*/
/**
 * @fn        void * rsi_virtual_to_physical_address(void *virtual_address)
 * @brief     Map virtual address to physical address
 * @param[in] virtual_address - pointer to virtual address 
 * @return    void
 */
/// @private
void *rsi_virtual_to_physical_address(void *virtual_address)
{
  return (virtual_address);
}
/*==============================================*/
/**
 * @fn        void *rsi_physical_to_virtual_address(void *physical_address)
 * @brief     Map physical address to virtual address
 * @param[in] physical_address - pointer to physical_address 
 * @return    void 
 *
 */
/// @private
void *rsi_physical_to_virtual_address(void *physical_address)
{
  return (physical_address);
}

/*==============================================*/
/**
 * @fn           rsi_error_t rsi_mutex_create(rsi_mutex_handle_t *mutex)            
 *
 */

rsi_error_t rsi_mutex_create(rsi_mutex_handle_t *mutex)
{
  /* Create dummy mutxe pointer because OS not present */
  RSI_NO_OS_ATOMIC_MUTEX_SET(*mutex, RSI_NO_OS_MUTEX_UNLOCKED);
  return (RSI_ERROR_NONE);
}
/*==============================================*/
/**
 * @fn           rsi_error_t rsi_mutex_lock(volatile rsi_mutex_handle_t *mutex)
 *
 */

rsi_error_t rsi_mutex_lock(volatile rsi_mutex_handle_t *mutex)
{
  while (RSI_NO_OS_ATOMIC_MUTEX_CHECK(*mutex, RSI_NO_OS_MUTEX_LOCKED))
    ;
  RSI_NO_OS_ATOMIC_MUTEX_SET(*mutex, RSI_NO_OS_MUTEX_LOCKED);
  return (RSI_ERROR_NONE);
}
/*==============================================*/
/**
 * @fn           rsi_error_t rsi_mutex_unlock(volatile rsi_mutex_handle_t *mutex)
 */

rsi_error_t rsi_mutex_unlock(volatile rsi_mutex_handle_t *mutex)
{
  RSI_NO_OS_ATOMIC_MUTEX_SET(*mutex, RSI_NO_OS_MUTEX_UNLOCKED);
  return (RSI_ERROR_NONE);
}
/*==============================================*/
/**
 * @fn           rsi_error_t rsi_mutex_destroy(rsi_mutex_handle_t *mutex)
 *
 */

rsi_error_t rsi_mutex_destroy(rsi_mutex_handle_t *mutex)
{
  UNUSED_PARAMETER(mutex); //This statement is added only to resolve compilation warning, value is unchanged
  return (RSI_ERROR_NONE);
}
/*==============================================*/
/**
 * @fn           rsi_error_t rsi_semaphore_create(rsi_semaphore_handle_t *semaphore,uint32_t count)
 *
 */

rsi_error_t rsi_semaphore_create(rsi_semaphore_handle_t *semaphore, uint32_t count)
{
  *semaphore = count;
  return (RSI_ERROR_NONE);
}

/*==============================================*/
/**
 * @fn           rsi_error_t rsi_semaphore_destroy(rsi_semaphore_handle_t *semaphore)
 *
 */

rsi_error_t rsi_semaphore_destroy(rsi_semaphore_handle_t *semaphore)
{
  *semaphore = 0;
  return (RSI_ERROR_NONE);
}
/*==============================================*/
/**
 * @fn           rsi_error_t rsi_semaphore_check_and_destroy(rsi_semaphore_handle_t *semaphore)
 * @brief        Check whether semaphore is created or not, It destroys the semaphore instance,if its created, otherwise should return Success 
 * @param[in]    semaphore - Semaphore handle pointer  
 * @return       0              - Success \n
 *               Negative Value - Failure
 *
 */
/// @private
rsi_error_t rsi_semaphore_check_and_destroy(rsi_semaphore_handle_t *semaphore)
{
  if (rsi_semaphore_destroy(semaphore) != RSI_ERROR_NONE) {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  return (RSI_ERROR_NONE);
}

/*==============================================*/
/**
 * @fn          rsi_error_t rsi_semaphore_wait(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms ) 
 *
 */

rsi_error_t rsi_semaphore_wait(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms)
{
  volatile uint32_t *event;
  rsi_timer_instance_t timer_instance;
  rsi_driver_cb_t *rsi_driver_cb = global_cb_p->rsi_driver_cb;
  event                          = (uint32_t *)semaphore;
  if (0UL != *event) {
    RSI_ATOMIC_DECREMENT(*event);
    return (RSI_ERROR_NONE);
  }
  if (0UL == timeout_ms) {
    while (0UL == *event) {
      // if any events pending
      // call rsi schedular
      rsi_scheduler(&rsi_driver_cb->scheduler_cb);
    }
    RSI_ATOMIC_DECREMENT(*event);
    return (RSI_ERROR_NONE);
  }
  rsi_init_timer(&timer_instance, timeout_ms);
  while ((0UL != timeout_ms) && (0UL == *event)) {
    // if any events pending
    // call rsi schedular
    rsi_scheduler(&rsi_driver_cb->scheduler_cb);
    if (rsi_timer_expired(&timer_instance)) {
      break;
    }
  }
  if (0UL == *event || (0UL == timeout_ms)) {
    return (RSI_ERROR_TIMEOUT);
  } else {
    RSI_ATOMIC_DECREMENT(*event);
  }
  return (RSI_ERROR_NONE);
}
/*==============================================*/
/**
 * @fn          rsi_error_t rsi_semaphore_post_from_isr(rsi_semaphore_handle_t *semaphore)
 *
 */

rsi_error_t rsi_semaphore_post_from_isr(rsi_semaphore_handle_t *semaphore)
{
  volatile uint32_t *event;

  event = (uint32_t *)semaphore;
  RSI_ATOMIC_INCREMENT(*event);
  return (RSI_ERROR_NONE);
}
/*==============================================*/
/**
 * @fn          rsi_error_t rsi_semaphore_post(rsi_semaphore_handle_t *semaphore) 
 *
 */

rsi_error_t rsi_semaphore_post(rsi_semaphore_handle_t *semaphore)
{
  volatile uint32_t *event;

  event = (uint32_t *)semaphore;
  RSI_ATOMIC_INCREMENT(*event);
  return (RSI_ERROR_NONE);
}
/*==============================================*/
/**
 * @fn          rsi_error_t rsi_semaphore_reset(rsi_semaphore_handle_t *semaphore) 
 *
 */

rsi_error_t rsi_semaphore_reset(rsi_semaphore_handle_t *semaphore)
{
  volatile uint32_t *event;

  event  = (uint32_t *)semaphore;
  *event = 0;
  return (RSI_ERROR_NONE);
}

#endif
/** @} */
