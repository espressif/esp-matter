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
#ifdef SL_CATALOG_APP_LOG_PRESENT
#include "app_log.h"
#endif
#include "sl_sleeptimer.h"
#include "os.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/* OS Event Flag ID */
#define PROPRIETARY_FLAG  ((OS_FLAGS)0x01)
// Proprietary Application task
#define PROPRIETARY_APP_TASK_PRIO         6U
// Proprietary Application task stack size
#define PROPRIETARY_APP_TASK_STACK_SIZE   (1024 / sizeof(CPU_STK))
// Redefining DEF_TRUE in order to be able to test the infinite loop
#if defined(__SL_UNIT_TEST)
#if defined(DEF_TRUE)
#undef DEF_TRUE
#endif // DEF_TRUE
extern int num_loops;
#define DEF_TRUE num_loops--
#endif // __SL_UNIT_TEST

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void proprietary_app_task(void *p_arg);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
// OS Event Flag Group
OS_FLAG_GRP  proprietary_event_flags;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
// Proprietary Application task buffer and stack allocation
static CPU_STK proprietary_app_task_stack[PROPRIETARY_APP_TASK_STACK_SIZE];
static OS_TCB  proprietary_app_task_TCB;
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
  RTOS_ERR err;
  // Create the Proprietary Application task
  OSTaskCreate(&proprietary_app_task_TCB,
               "Proprietary App Task",
               proprietary_app_task,
               0U,
               PROPRIETARY_APP_TASK_PRIO,
               &proprietary_app_task_stack[0U],
               (PROPRIETARY_APP_TASK_STACK_SIZE / 10U),
               PROPRIETARY_APP_TASK_STACK_SIZE,
               0U,
               0U,
               0U,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               &err);

  // Initialize the flag group for the proprietary task
  OSFlagCreate(&proprietary_event_flags, "Prop. flags", (OS_FLAGS) 0, &err);
}

/*******************************************************************************
 * The function is used to notify kernel to allow the proprietary task to run.
 *
 * @param None
 * @returns None
 ******************************************************************************/
void app_task_notify(void)
{
  RTOS_ERR err;
  OSFlagPost(&proprietary_event_flags, PROPRIETARY_FLAG, OS_OPT_POST_FLAG_SET, &err);
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
  app_log_info("%s with Micrium\n", app_name);
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
  PP_UNUSED_PARAM(p_arg);
  RTOS_ERR err;
  rail_handle = app_init();

  while (DEF_TRUE) {
    app_process_action(rail_handle);
    OSFlagPend(&proprietary_event_flags,
               PROPRIETARY_FLAG,
               (OS_TICK) 0,
               OS_OPT_PEND_BLOCKING       \
               + OS_OPT_PEND_FLAG_SET_ANY \
               + OS_OPT_PEND_FLAG_CONSUME,
               NULL,
               &err);
  }
}
