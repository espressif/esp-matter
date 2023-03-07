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
#include "FreeRTOS.h"
#include "task.h"
#include "sl_joystick.h"
#include "sl_iostream.h"
#include "sl_iostream_init_instances.h"
#include "sl_iostream_handles.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#ifndef JOYSTICK_TASK_STACK_SIZE
#define JOYSTICK_TASK_STACK_SIZE      256
#endif

#ifndef JOYSTICK_TASK_PRIO
#define JOYSTICK_TASK_PRIO            20
#endif

#ifndef EXAMPLE_USE_STATIC_ALLOCATION
#define EXAMPLE_USE_STATIC_ALLOCATION      1
#endif

/* Duration of delay in milliseconds */
#define DELAY_MS                      100

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

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
  TaskHandle_t xHandle = NULL;

#if (EXAMPLE_USE_STATIC_ALLOCATION == 1)

  static StaticTask_t xTaskBuffer;
  static StackType_t  xStack[JOYSTICK_TASK_STACK_SIZE];

  /* Create Joystick Task without using any dynamic memory allocation */
  xHandle = xTaskCreateStatic(app_joystick_task,
                              "joystick task",
                              JOYSTICK_TASK_STACK_SIZE,
                              ( void * ) NULL,
                              tskIDLE_PRIORITY + 1,
                              xStack,
                              &xTaskBuffer);

  /* Since puxStackBuffer and pxTaskBuffer parameters are not NULL,
     it is impossible for xHandle to be null. This check is for rigorous
     example demonstration */
  EFM_ASSERT(xHandle != NULL);

#else

  BaseType_t xReturned = pdFAIL;

  /* Create Joystick Task using dynamic memory allocation */
  xReturned = xTaskCreate(app_joystick_task,
                          "joystick task",
                          JOYSTICK_TASK_STACK_SIZE,
                          ( void * ) NULL,
                          tskIDLE_PRIORITY + 1,
                          &xHandle);

  /* Unlike task creation using static allocation, dynamic task creation can very likely
     fail due to lack of memory. Checking the return value is relevant */
  EFM_ASSERT(xReturned == pdPASS);

#endif
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

  /* Use the provided calculation macro to convert milliseconds delay specified in
     DELAY_MS milliseconds to OS ticks */
  const TickType_t xDelay = pdMS_TO_TICKS(DELAY_MS);

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
    /* Wait for specified delay */
    vTaskDelay(xDelay);

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
  }
}
