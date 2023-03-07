/*******************************************************************************
* @file  rsi_os_wrapper.c
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

#ifndef RSI_FREE_RTOS
#define RSI_FREE_RTOS
#include <rsi_driver.h>
#ifdef RSI_WITH_OS
/**
 * Include files
 * */
#include <FreeRTOS.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <projdefs.h>
#include <task.h>
#include "rsi_wlan_non_rom.h"
extern rsi_socket_info_non_rom_t *rsi_socket_pool_non_rom;

/** @addtogroup RTOS
* @{
*/
/*==============================================*/
/**
 * @fn          rsi_reg_flags_t rsi_critical_section_entry()
 * @brief       Disable interrupt to enter crtical section.
 * @param[in]   None
 * @return      flags - interrupt status before entering critical section
 *
 *
 */
rsi_reg_flags_t rsi_critical_section_entry()
{
  // hold interrupt status before entering critical section
  // disable interrupts
  rsi_reg_flags_t xflags;
  // hold interrupt status before entering critical section
  // disable interrupts
  xflags = 0;

  rsi_hal_intr_mask();

  taskENTER_CRITICAL();

  // return stored interrupt status
  return (xflags);
}

/*==============================================*/
/**
 * @fn         rsi_critical_section_exit(rsi_reg_flags_t xflags)
 * @brief      Exit critical section by restoring interrupts.
 * @param[in]  xflags - interrupt status to restore interrupt on exit from critical section
 * @return     Void
 *
 *
 *
 */

void rsi_critical_section_exit(rsi_reg_flags_t xflags)
{
  //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(xflags);
  // restore interrupts while exiting critical section
  taskEXIT_CRITICAL();

  rsi_hal_intr_unmask();
}

/*==============================================*/
/**
 * @fn           rsi_error_t rsi_mutex_create(rsi_mutex_handle_t *mutex)
 * @brief        Create and initialize the mutex 
 * @param[in]    mutex - Mutex handle pointer
 * @return       0              - Success \n
 *               Negative Value - Failure 
 *
 *
 */

rsi_error_t rsi_mutex_create(rsi_mutex_handle_t *mutex)
{
  SemaphoreHandle_t *p_mutex = (SemaphoreHandle_t *)mutex;
  if (p_mutex == NULL) {
    return RSI_ERROR_IN_OS_OPERATION;
  }

  *p_mutex = (SemaphoreHandle_t)xSemaphoreCreateMutex();

  if (*p_mutex == NULL) //Note : FreeRTOS porting
  {
    return RSI_ERROR_IN_OS_OPERATION;
  }

  return RSI_ERROR_NONE;
}

/*==============================================*/
/**
 * @fn           rsi_error_t rsi_mutex_lock(volatile rsi_mutex_handle_t *mutex)
 * @brief        Take the mutex 
 * @param[in]    mutex - Mutex handle pointer  
 * @return       0              - Success \n
 *               Negative Value - Failure  
 *
 */
rsi_error_t rsi_mutex_lock(volatile rsi_mutex_handle_t *mutex)
{
  uint32_t timeout_ms = 0;

  SemaphoreHandle_t *p_mutex = (SemaphoreHandle_t *)mutex;
  if (mutex == NULL || *p_mutex == NULL) //Note : FreeRTOS porting
  {
    return RSI_ERROR_INVALID_PARAM;
  }
  if (!timeout_ms) {
    timeout_ms = portMAX_DELAY;
  }
  if (xSemaphoreTake(*p_mutex, timeout_ms) == pdPASS) {
    return RSI_ERROR_NONE;
  }
  return RSI_ERROR_IN_OS_OPERATION;
}

/*==============================================*/
/**
 * @fn           rsi_error_t rsi_mutex_lock_from_isr(volatile rsi_mutex_handle_t *mutex)
 * @brief        Take the mutex from ISR context
 * @param[in]    mutex - Mutex handle pointer  
 * @return       0              - Success \n
 *               Negative Value - Failure  
 *
 */
void rsi_mutex_lock_from_isr(volatile rsi_mutex_handle_t *mutex)
{
  BaseType_t xTaskWokenByReceive = pdFALSE;
  SemaphoreHandle_t *xSemaphore  = (SemaphoreHandle_t *)mutex;
  (void)xSemaphoreTakeFromISR(*xSemaphore, &xTaskWokenByReceive);
}
/*==============================================*/
/**
 * @fn           rsi_error_t rsi_mutex_unlock(volatile rsi_mutex_handle_t *mutex)
 * @brief        Give the mutex 
 * @param[in]    mutex - Mutex handle pointer  
 * @return       0              - Success \n 
 *               Negative Value - Failure 
 *
 */
rsi_error_t rsi_mutex_unlock(volatile rsi_mutex_handle_t *mutex)
{
  SemaphoreHandle_t *p_mutex = (SemaphoreHandle_t *)mutex;
  if (mutex == NULL || *p_mutex == NULL) //Note : FreeRTOS porting
  {
    return RSI_ERROR_INVALID_PARAM;
  }
  if (xSemaphoreGive(*p_mutex) == pdPASS) {
    return RSI_ERROR_NONE;
  }
  return RSI_ERROR_IN_OS_OPERATION;
}
/*==============================================*/
/**
 * @fn           rsi_error_t rsi_mutex_unlock_from_isr(volatile rsi_mutex_handle_t *mutex)
 * @brief        Give the mutex from ISR context
 * @param[in]    mutex - Mutex handle pointer  
 * @return       none
 *
 */
void rsi_mutex_unlock_from_isr(volatile rsi_mutex_handle_t *mutex)
{
  BaseType_t xTaskWokenByReceive = pdFALSE;
  SemaphoreHandle_t *xSemaphore  = (SemaphoreHandle_t *)mutex;
  (void)xSemaphoreGiveFromISR(*xSemaphore, &xTaskWokenByReceive);
}
/*==============================================*/
/**
 * @fn           rsi_error_t rsi_mutex_destroy(rsi_mutex_handle_t *mutex)
 * @brief        Destroye the mutex 
 * @param[in]    mutex - Mutex handle pointer  
 * @return       0              - Success \n
 *               Negative Value - Failure 
 *
 */
rsi_error_t rsi_mutex_destroy(rsi_mutex_handle_t *mutex)
{
  SemaphoreHandle_t *p_mutex = (SemaphoreHandle_t *)mutex;
  if (mutex == NULL || *p_mutex == NULL) //Note : FreeRTOS porting
  {
    return RSI_ERROR_INVALID_PARAM;
  }
  vSemaphoreDelete(*p_mutex);

  return RSI_ERROR_NONE;
}

/*==============================================*/
/**
 * @fn           rsi_error_t rsi_semaphore_create(rsi_semaphore_handle_t *semaphore,uint32_t count)
 * @brief        Create and initialize the semaphore instance 
 * @param[in]    semaphore - Semaphore handle pointer  
 * @param[in]    count - Resource count   
 * @return       0              - Success \n 
 *               Negative Value - Failure 
 *
 */
rsi_error_t rsi_semaphore_create(rsi_semaphore_handle_t *semaphore, uint32_t count)
{
  UNUSED_PARAMETER(count); //This statement is added only to resolve compilation warning, value is unchanged
  SemaphoreHandle_t *p_semaphore = NULL;
  p_semaphore                    = (SemaphoreHandle_t *)semaphore;

  if (semaphore == NULL) {
    return RSI_ERROR_IN_OS_OPERATION;
  }
  *p_semaphore = xSemaphoreCreateBinary();

  if (*p_semaphore == NULL) {
    return RSI_ERROR_IN_OS_OPERATION;
  }
  return RSI_ERROR_NONE;
}

/*==============================================*/
/**
 * @fn           rsi_error_t rsi_semaphore_destroy(rsi_semaphore_handle_t *semaphore)
 * @brief        Destroy the semaphore instance
 * @param[in]    semaphore - Semaphore handle pointer  
 * @return       0              - Success \n
 *               Negative Value - Failure 
 */
rsi_error_t rsi_semaphore_destroy(rsi_semaphore_handle_t *semaphore)
{
  SemaphoreHandle_t *p_semaphore = NULL;
  p_semaphore                    = (SemaphoreHandle_t *)semaphore;

  if (semaphore == NULL || *p_semaphore == NULL) //Note : FreeRTOS porting
  {
    return RSI_ERROR_INVALID_PARAM;
  }
  vSemaphoreDelete(*p_semaphore);

  return RSI_ERROR_NONE;
}

/*==============================================*/
/**
 * @fn           rsi_error_t rsi_semaphore_check_and_destroy(rsi_semaphore_handle_t *semaphore)
 *
 */

rsi_error_t rsi_semaphore_check_and_destroy(rsi_semaphore_handle_t *semaphore)
{
  SemaphoreHandle_t *p_semaphore = NULL;
  p_semaphore                    = (SemaphoreHandle_t *)semaphore;

  if (semaphore == NULL || *p_semaphore == NULL) //Note : FreeRTOS porting
  {
    return RSI_ERROR_NONE; // returning success since semaphore does not exists
  }
  vSemaphoreDelete(*p_semaphore);

  return RSI_ERROR_NONE;
}

/*==============================================*/
/**
 * @fn          rsi_error_t rsi_semaphore_wait(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms ) 
 * @brief       Wireless library to acquire or wait for semaphore.
 * @param[in]   semaphore - Semaphore handle pointer  
 * @param[in]   time_ms - Maximum time to wait to acquire semaphore. If timeout_ms is 0 then wait
                till acquire semaphore.
 * @return      0              - Success \n 
 *              Negative Value - Failure 
 *
 */

rsi_error_t rsi_semaphore_wait(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms)
{
  SemaphoreHandle_t *p_semaphore = NULL;
  p_semaphore                    = (SemaphoreHandle_t *)semaphore;

  if (semaphore == NULL || *p_semaphore == NULL) //Note : FreeRTOS porting
  {
    return RSI_ERROR_INVALID_PARAM;
  }
  if (!timeout_ms) {
    timeout_ms = portMAX_DELAY;
  }
  if (xSemaphoreTake(*p_semaphore, timeout_ms) == pdPASS) {
    return RSI_ERROR_NONE;
  }
  return RSI_ERROR_IN_OS_OPERATION;
}

/*==============================================*/
/**
 * @fn          rsi_error_t rsi_semaphore_post(rsi_semaphore_handle_t *semaphore) 
 * @brief       Wireless library to release semaphore, which was acquired.
 * @param[in]   semaphore - Semaphore handle pointer  
 * @return      0              - Success \n 
 *              Negative Value - Failure 
 *
 */

rsi_error_t rsi_semaphore_post(rsi_semaphore_handle_t *semaphore)
{
  SemaphoreHandle_t *p_semaphore = NULL;
  p_semaphore                    = (SemaphoreHandle_t *)semaphore;

  if (semaphore == NULL || *p_semaphore == NULL) //Note : FreeRTOS porting
  {
    return RSI_ERROR_INVALID_PARAM;
  }
  if (xSemaphoreGive(*p_semaphore) == pdPASS) {
    return RSI_ERROR_NONE;
  }
  return RSI_ERROR_IN_OS_OPERATION;
}

/*====================================================*/
/**
 * @fn          rsi_error_t rsi_semaphore_post_from_isr(rsi_semaphore_handle_t *semaphore)
 * @brief       Wireless library to release semaphore, which was acquired.
 * @param[in]   semphore - Semaphore handle pointer 
 * @return      0              - Success \n 
 *              Negative Value - Failure 
 *             
 */

rsi_error_t rsi_semaphore_post_from_isr(rsi_semaphore_handle_t *semaphore)
{
  SemaphoreHandle_t *p_semaphore = NULL;
  p_semaphore                    = (SemaphoreHandle_t *)semaphore;

  if (semaphore == NULL || *p_semaphore == NULL) //Note : FreeRTOS porting
  {
    return RSI_ERROR_INVALID_PARAM;
  }

  if (xSemaphoreGiveFromISR(*p_semaphore, NULL) == pdPASS) {
    return RSI_ERROR_NONE;
  }
  return RSI_ERROR_IN_OS_OPERATION;
}

/*==============================================*/
/**
 * @fn          rsi_error_t rsi_semaphore_reset(rsi_semaphore_handle_t *semaphore) 
 * @brief       Used by Wireless Library to reset the semaphore
 * @param[in]   semphore - Semaphore handle pointer  
 * @return      0              - Success \n 
 *              Negative Value - Failure
 *
 */
rsi_error_t rsi_semaphore_reset(rsi_semaphore_handle_t *semaphore)
{
  SemaphoreHandle_t *p_semaphore = NULL;
  p_semaphore                    = (SemaphoreHandle_t *)semaphore;

  if (semaphore == NULL || *p_semaphore == NULL) //!Note : FreeRTOS porting
  {
    return RSI_ERROR_INVALID_PARAM;
  }
  vSemaphoreDelete(*p_semaphore);

  return RSI_ERROR_NONE;
}

/*==============================================*/
/**
 * @fn           rsi_error_t rsi_task_create( rsi_task_function_t task_function,uint8_t *task_name,
                 uint32_t stack_size, void *parameters,
                 uint32_t task_priority,rsi_task_handle_t  *task_handle)
 * @brief        Wireless Library to create platform specific OS task/thread. 
 * @param[in]    task_function - Pointer to function to be executed by created thread. Prototype of the function
 * @param[in]    task_name     - Name of the created task  
 * @param[in]    stack_size    - Stack size given to the created task  
 * @param[in]    parameters    - Pointer to the parameters to be passed to task function
 * @param[in]    task_priority - task priority 
 * @param[in]    task_handle   - task handle/instance created 
 * @return       0              - Success \n
 *               Negative Value - Failure
 *
 */

rsi_error_t rsi_task_create(rsi_task_function_t task_function,
                            uint8_t *task_name,
                            uint32_t stack_size,
                            void *parameters,
                            uint32_t task_priority,
                            rsi_task_handle_t *task_handle)
{
#ifdef SAPIS_BT_STACK_ON_HOST
  task_priority = 7;
#endif
  if (pdPASS
      == xTaskCreate(task_function,
                     (char const *)task_name,
                     stack_size,
                     parameters,
                     task_priority,
                     (TaskHandle_t *)task_handle)) {
    return RSI_ERROR_NONE;
  }

  return RSI_ERROR_IN_OS_OPERATION;
}

/*==============================================*/
/**
 * @fn          void rsi_task_destroy(rsi_task_handle_t *task_handle)
 * @brief       Delete the task created
 * @param[in]   task_handle - Task handle/instance to be deleted
 * @return      void
 *
 */

void rsi_task_destroy(rsi_task_handle_t *task_handle)
{
  vTaskDelete((TaskHandle_t)task_handle);
}

/*==============================================*/
/**
 * @fn          void rsi_os_task_delay(uint32_t timeout_ms)
 * @brief       Induce required delay in milli seconds
 * @param[in]   timeout_ms - Expected delay in milli seconds
 * @return      void 
 */
void rsi_os_task_delay(uint32_t timeout_ms)
{
  vTaskDelay(timeout_ms);
}

/*==============================================*/
/**
 * @fn           void rsi_start_os_scheduler()
 * @brief        Schedule the tasks created
 * @param[in]    None
 * @return       void 
 */
void rsi_start_os_scheduler()
{
  vTaskStartScheduler();
}
/*==============================================*/
/**
 * @fn          void rsi_set_os_errno(int32_t error)
 * @brief       Sets the os error .
 * @param[in]   error - Error
 * @return      void 
 */
void rsi_set_os_errno(int32_t error)
{
  UNUSED_PARAMETER(error); //This statement is added only to resolve compilation warning, value is unchanged
}

/*==============================================*/
/**
 * @fn          rsi_base_type_t rsi_task_notify_wait(uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t *pulNotificationValue, uint32_t timeout)
 * @brief       Allow a task to wait with optional timeout
 * @param[in]   ulBitsToClearOnEntry - bits set here will be cleared in the task’s notification value \n on entry to the function.
 * @param[in]   pulNotificationValue - used to pass out the task notification value and an optional paramter
 * @param[in]   ulBitsToClearOnExit  - bits to clear on exit 
 * @param[in]   timeout              - maximum amount of time the calling task should remain in blocked state
 * @return      0              - Success \n
 *              Negative Value - Failure
 */
rsi_base_type_t rsi_task_notify_wait(uint32_t ulBitsToClearOnEntry,
                                     uint32_t ulBitsToClearOnExit,
                                     uint32_t *pulNotificationValue,
                                     uint32_t timeout)
{
  rsi_base_type_t xResult;
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(timeout);

  /* Wait to be notified of an interrupt. */
  xResult = xTaskNotifyWait(ulBitsToClearOnEntry, /* Don't clear bits on entry. */
                            ulBitsToClearOnExit,  /* Clear all bits on exit. */
                            pulNotificationValue, /* Stores the notified value. */
                            xMaxBlockTime);

  return xResult;
}

/*==============================================*/
/**
 * @fn          rsi_base_type_t rsi_task_notify(rsi_task_handle_t xTaskToNotify, uint32_t ulValue)
 * @brief       Notify to a task
 * @param[in]   xTaskToNotify - The handle of the task to which the notification is being sent
 * @param[in]   ulValue       - How ulValue is used is dependent on the eNotifyAction value
 * @return      0  - Success \n
 *              Non-Zero Value - Failure
 */
rsi_base_type_t rsi_task_notify(rsi_task_handle_t xTaskToNotify, uint32_t ulValue)
{
  // eAction - an enumerated type that specifies how to update the receiving task’s notification value
  eNotifyAction eAction = eSetBits;
  if (xTaskToNotify != NULL) {
    return xTaskNotify(xTaskToNotify, ulValue, (eNotifyAction)eAction);
  } else {
    return -1;
  }
}

/*==============================================*/
/**
 * @fn          rsi_base_type_t rsi_task_notify_from_isr(rsi_task_handle_t xTaskToNotify, uint32_t ulValue, rsi_base_type_t *pxHigherPriorityTaskWoken )
 * @brief       Notification directly to a task and should be used in ISR only
 * @param[in]	xTaskToNotify			- The handle of the task to which the notification is being sent
 * @param[in]	ulValue       			- How ulValue is used is dependent on the eNotifyAction value
 * @param[in]	pxHigherPriorityTaskWoken	- Sets,if sending the notification caused a task to unblock, and the unblocked task has a priority higher than the currently running task.
 * @return      0  - Success \n
 *              Non-Zero Value - Failure
 */
rsi_base_type_t rsi_task_notify_from_isr(rsi_task_handle_t xTaskToNotify,
                                         uint32_t ulValue,
                                         rsi_base_type_t *pxHigherPriorityTaskWoken)
{
  // eAction - an enumerated type that specifies how to update the receiving task’s notification value
  eNotifyAction eAction = eSetBits;

  if (xTaskToNotify != NULL) {
    return xTaskNotifyFromISR(xTaskToNotify, ulValue, (eNotifyAction)eAction, pxHigherPriorityTaskWoken);
  } else {
    return -1;
  }
}

/*==============================================*/
/**
 * @fn          uint32_t rsi_os_task_notify_take( BaseType_t xClearCountOnExit, TickType_t xTicksToWait )
 * @brief       Allow a task in wait in blocked state for its notification value >0
 * @param[in]   xClearCountOnExit - Based on this value calling task notification value will be decremented/zero
 * @param[in]   xTicksToWait      - maximum amount of time calling task should remain in blocked state
 * @return      0  - Success \n 
 *              Non-Zero Value - Failure
 */
uint32_t rsi_os_task_notify_take(BaseType_t xClearCountOnExit, TickType_t xTicksToWait)
{
  return ulTaskNotifyTake(xClearCountOnExit, xTicksToWait);
}

/*==============================================*/
/**
 * @fn          BaseType_t rsi_os_task_notify_give(rsi_task_handle_t xTaskToNotify)
 * @brief       Notify to a task
 * @param[in]   xTaskToNotify - Task handle to notify
 * @return      Task notification value \n
 * 		-1 - Failure
 */
BaseType_t rsi_os_task_notify_give(rsi_task_handle_t xTaskToNotify)
{
  if (xTaskToNotify != NULL) {
    return xTaskNotifyGive(xTaskToNotify);
  } else {
    return -1;
  }
}

#ifdef RSI_WLAN_ENABLE

/*==============================================*/
/**
 * @fn          int32_t rsi_get_error(int32_t sockID)
 * @brief       Return wlan status
 * @param[in]   sockID - Socket Id
 * @return      wlan status
 */
/// @private
int32_t rsi_get_error(int32_t sockID)
{
  if (rsi_socket_pool_non_rom[sockID].socket_status == 0xFF87
      || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB38) {
    return RSI_ERROR_ECONNREFUSED;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == 0x00D2) {
    return RSI_ERROR_ECONNABORTED;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == 0xFF6C
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xFF6A
             || rsi_socket_pool_non_rom[sockID].socket_status == RSI_ERROR_RESPONSE_TIMEOUT
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xBBD6) {
    return RSI_ERROR_ETIMEDOUT;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == 0x0021
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xFF82
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xFF74
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB27
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xFF80
             || rsi_socket_pool_non_rom[sockID].socket_status == 0x00D1) {
    return RSI_ERROR_EACCES;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == 0x00D3
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xFFFE) {
    return RSI_ERROR_EPERM;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == 0xFF7E
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xBBED
             || rsi_socket_pool_non_rom[sockID].socket_status == 0x003E
             || rsi_socket_pool_non_rom[sockID].socket_status == 0x0030
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB3E) {
    return RSI_ERROR_EMSGSIZE;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == 0xFF86) {
    return RSI_ERROR_EBADF;
  } else if ((rsi_socket_pool_non_rom[sockID].socket_status == 0x0015)
             || (rsi_socket_pool_non_rom[sockID].socket_status == 0x0025)) {
    return RSI_ERROR_EAGAIN;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == 0xBB22
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB42) {
    return RSI_ERROR_EISCONN;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == 0xFFFF
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB50) {
    return RSI_ERROR_ENOTCONN;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == 0xBB33
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB23
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB45
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB46
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB50) {
    return RSI_ERROR_EADDRINUSE;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == 0xBB36) {
    return RSI_ERROR_ENOTSOCK;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == RSI_ERROR_PKT_ALLOCATION_FAILURE) {
    return RSI_ERROR_ENOMEM;
  } else
    return RSI_SOCK_ERROR;
}

#endif

/*==============================================*/
/**
 * @fn          void *rsi_malloc (uint32_t size)
 * @brief       Allocate memory from the buffer which is maintained by freertos	
 * @param[in]   size - required bytes in size
 * @return      void
 */

void *rsi_malloc(uint32_t size)
{
  if (size == 0) {
    return NULL;
  } else {
    return pvPortMalloc(size);
  }
}

/*==============================================*/
/**
 * @fn          void rsi_free (void *ptr)
 * @brief       Free the memory pointed by 'ptr'
 * @param[in]   ptr - starting address of the memory to be freed
 * @return      void
 */
void rsi_free(void *ptr)
{
  if (ptr == NULL) {
    // do nothing
  } else {
    vPortFree(ptr);
  }
}
/*==============================================*/
/**
 * @fn          void rsi_vport_enter_critical(void)
 * @brief       Enter into critical section 
 * @param[in]   void 
 * @return      void 
 */
void rsi_vport_enter_critical(void)
{
  vPortEnterCritical();
}

/*==============================================*/
/**
 * @fn         void rsi_vport_exit_critical(void)
 * @brief       Enter into exit section 
 * @param[in]   void 
 * @return      void 
 */

void rsi_vport_exit_critical(void)
{
  vPortExitCritical();
}

/*==============================================*/
/**
 * @fn          void rsi_task_suspend(rsi_task_handle_t *task_handle)
 * @brief       Exit into critical section 
 * @param[in]   task_handle - Task handle to be suspended
 * @return      void 
 */
void rsi_task_suspend(rsi_task_handle_t *task_handle)
{
  vTaskSuspend((TaskHandle_t)task_handle);
}
#endif
#endif
/** @} */
