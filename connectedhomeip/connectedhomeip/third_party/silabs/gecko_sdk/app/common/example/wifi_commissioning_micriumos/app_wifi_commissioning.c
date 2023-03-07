/***************************************************************************//**
 * @file
 * @brief Core Wi-Fi Commissioning application logic.
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
// Define module name for Power Manager debuging feature.
#define CURRENT_MODULE_NAME    "APP_COMMON_EXAMPLE_WIFI_COMMISSIONING"

#include <stdio.h>
#include "os.h"
#include "io.h"
#include "bsp_os.h"
#include "common.h"
#include "em_common.h"
#include "sl_simple_led_instances.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_button_config.h"
#include "app_webpage.h"
#include "app_wifi_events.h"
#include "app_wifi_commissioning.h"
#include "sl_wfx_host.h"

#define START_APP_TASK_PRIO              30u
#define START_APP_TASK_STK_SIZE         600u
/// Start task stack.
static CPU_STK start_app_task_stk[START_APP_TASK_STK_SIZE];
/// Start task TCB.
static OS_TCB  start_app_task_tcb;
static void    start_app_task(void *p_arg);

void sl_button_on_change(const sl_button_t *handle)
{
  if ((handle == &sl_button_btn0)
      && (sl_button_get_state(&sl_button_btn0) == SL_SIMPLE_BUTTON_POLARITY)) {
    sl_led_toggle(&sl_led_led0);
  } else if ((handle == &sl_button_btn1)
             && (sl_button_get_state(&sl_button_btn1) == SL_SIMPLE_BUTTON_POLARITY)) {
    sl_led_toggle(&sl_led_led1);
  }
}

static void start_app_task(void *p_arg)
{
  RTOS_ERR  err;
  PP_UNUSED_PARAM(p_arg); // Prevent compiler warning.

  OSSemPend(&wfx_init_sem, 0, OS_OPT_PEND_BLOCKING, 0, &err);
  // Clear the console and buffer
  printf("Wi-Fi Commissioning Micrium  OS Example\r\n");

  app_wifi_events_start();
  webpage_start();

  // Delete the init thread.
  OSTaskDel(0, &err);
}
/**************************************************************************//**
 * Wi-Fi Commissioning application init.
 *****************************************************************************/
void app_wifi_commissioning_init(void)
{
  RTOS_ERR err;

  OSTaskCreate(&start_app_task_tcb,   // Create the Start Task.
               "Start APP Task",
               start_app_task,
               DEF_NULL,
               START_APP_TASK_PRIO,
               &start_app_task_stk[0],
               (START_APP_TASK_STK_SIZE / 10u),
               START_APP_TASK_STK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
}
