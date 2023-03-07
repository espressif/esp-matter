/***************************************************************************//**
 * @file
 * @brief app_task_init.c
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "sl_component_catalog.h"
#include "app_init.h"
#include "app_task_init.h"
#include "app_process.h"
#ifdef SL_CATALOG_APP_ASSERT_PRESENT
#include "app_assert.h"
#endif
#ifdef SL_CATALOG_APP_LOG_PRESENT
#include "app_log.h"
#endif
#include "sl_sleeptimer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
// Proprietary Application task priority
#define PROPRIETARY_APP_TASK_PRIO         6U
// Proprietary Application task stack size
#define PROPRIETARY_APP_TASK_STACK_SIZE      configMINIMAL_STACK_SIZE
// Redefining DEF_TRUE in order to be able to test the infinite loop
#if defined(__SL_UNIT_TEST)
#if defined(DEF_TRUE)
#undef DEF_TRUE
#endif // DEF_TRUE
extern int num_loops;
#define DEF_TRUE num_loops--
#else
#define DEF_TRUE 1U
#endif // __SL_UNIT_TEST

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void proprietary_app_task(void *p_arg);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/* Stores the handle of the task that will be notified when the
   transmission is complete. */
TaskHandle_t proprietary_task_notify = NULL;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
// Proprietary Application task buffer and stack allocation
static StaticTask_t proprietary_app_task_buffer;
static StackType_t  proprietary_app_task_stack[PROPRIETARY_APP_TASK_STACK_SIZE];
// Proprietary Application task max blocking time
static const TickType_t xMaxBlockTime = pdMS_TO_TICKS(1000);
/// A static handle of a RAIL instance
static RAIL_Handle_t rail_handle;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * The function is used for application initialization.
 *
 * @param None
 * @returns None
 ******************************************************************************/
void app_task_init(void)
{
  TaskHandle_t proprietary_task_handle = NULL;

// Create Proprietary App Task without using any dynamic memory allocation
  proprietary_task_handle = xTaskCreateStatic(proprietary_app_task,
                                              "Proprietary App Task",
                                              PROPRIETARY_APP_TASK_STACK_SIZE,
                                              NULL,
                                              PROPRIETARY_APP_TASK_PRIO,
                                              proprietary_app_task_stack,
                                              &proprietary_app_task_buffer);

  // Since proprietary_app_task_stack and proprietary_app_task_buffer parameters are not NULL,
  // it is impossible for proprietary_task_handle to be null. This check is for
  // rigorous example demonstration.
#ifdef SL_CATALOG_APP_ASSERT_PRESENT
  app_assert(proprietary_task_handle != NULL, "FreeRTOS task creation have failed\n");
#else
  while (proprietary_task_handle == NULL) {
  }
#endif
  //Save task address for notification
  proprietary_task_notify = proprietary_task_handle;
}

/*******************************************************************************
 * The function is used to notify kernel to allow the proprietary task to run.
 *
 * @param None
 * @returns None
 ******************************************************************************/
void app_task_notify(void)
{
  if (proprietary_task_notify != NULL) {
    vTaskNotifyGiveFromISR(proprietary_task_notify, false);
  }
}

/*******************************************************************************
 * Print the sample app name with the OS which is it running on
 *
 * @param app_name: Sample app name to be printed
 * @returns None
 ******************************************************************************/
void print_sample_app_name(const char* app_name)
{
#ifdef SL_CATALOG_APP_LOG_PRESENT
  app_log_info("%s with FreeRTOS\n", app_name);
#else
  (void)app_name;
#endif
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * The function is the task that will be run by the kernel.
 *
 * @param None
 * @returns None
 ******************************************************************************/
static void proprietary_app_task(void *p_arg)
{
  (void)p_arg;
  rail_handle = app_init();

#ifdef SL_CATALOG_APP_ASSERT_PRESENT
  app_assert(rail_handle != NULL, "Failed to get RAIL handle!\n");
#else
  while (rail_handle == NULL) {
  }
#endif

  while (DEF_TRUE) {
    uint32_t ulNotificationValue;
    app_process_action(rail_handle);
    ulNotificationValue = ulTaskNotifyTake(pdFALSE,
                                           xMaxBlockTime);
    if ( ulNotificationValue == 1 ) {
      /* The transmission ended as expected. */
    } else {
      /* The call to ulTaskNotifyTake() timed out. */
    }
  }
}
