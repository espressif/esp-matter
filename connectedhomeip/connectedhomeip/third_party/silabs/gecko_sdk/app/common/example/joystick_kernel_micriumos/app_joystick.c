/***************************************************************************//**
 * @file
 * @brief joystick example functions
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stdio.h>
#include <string.h>
#include "sl_joystick.h"
#include "sl_iostream.h"
#include "sl_iostream_init_instances.h"
#include "sl_iostream_handles.h"
#include "os.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#ifndef JOYSTICK_TASK_STACK_SIZE
#define JOYSTICK_TASK_STACK_SIZE      256
#endif

#ifndef JOYSTICK_TASK_PRIO
#define JOYSTICK_TASK_PRIO            20
#endif

/* Duration of delay in milliseconds */
#define DELAY_MS                      100

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static OS_TCB tcb;
static CPU_STK stack[JOYSTICK_TASK_STACK_SIZE];
static sl_joystick_t sl_joystick_handle = JOYSTICK_HANDLE_DEFAULT;

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/
static void app_joystick_task(void *arg);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize example.
 ******************************************************************************/
void app_joystick_init(void)
{
  RTOS_ERR err;

  /* Create Task */
  OSTaskCreate(&tcb,
               "joystick task",
               app_joystick_task,
               DEF_NULL,
               JOYSTICK_TASK_PRIO,
               &stack[0],
               (JOYSTICK_TASK_STACK_SIZE / 10u),
               JOYSTICK_TASK_STACK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
}

/***************************************************************************//**
 * Joystick task.
 *
 * This task reads and prints joystick position via VCOM at intervals specified
 * by DELAY_MS.
 ******************************************************************************/
static void app_joystick_task(void *arg)
{
  sl_joystick_position_t pos;
  sl_status_t status = SL_STATUS_OK;

  (void)&arg;

  /* Output on vcom usart instance */
  const char example_title[] = "Joystick example\r\n\r\n";

  /* Setting default stream */
  sl_iostream_set_default(sl_iostream_vcom_handle);

  printf("%s", example_title);

  /* Initialize the Joystick driver */
  sl_joystick_init(&sl_joystick_handle);

  /* Start Joystick data acquisition */
  sl_joystick_start(&sl_joystick_handle);

  while (1) {
    OS_TICK os_ticks;
    RTOS_ERR err;

    /* Sleep for milliseconds specified in DELAY_MS */
    os_ticks = (OSCfg_TickRate_Hz * DELAY_MS) / 1000;
    OSTimeDly(os_ticks, OS_OPT_TIME_DLY, &err);
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

    status = sl_joystick_get_position(&sl_joystick_handle, &pos);
    if (SL_STATUS_OK != status) {
      return;
    }

    switch (pos) {
      case JOYSTICK_NONE:
        printf("Not Pressed\n");
        break;
      case JOYSTICK_C:
        printf("Center\n");
        break;
      case JOYSTICK_N:
        printf("North\n");
        break;
      case JOYSTICK_E:
        printf("East\n");
        break;
      case JOYSTICK_S:
        printf("South\n");
        break;
      case JOYSTICK_W:
        printf("West\n");
        break;
    }
    ;
  }
}
