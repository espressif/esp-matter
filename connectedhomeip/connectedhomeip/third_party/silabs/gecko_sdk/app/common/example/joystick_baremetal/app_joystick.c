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
#include "sl_sleeptimer.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/* Duration of delay in milliseconds */
#define DELAY_MS    100

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

sl_sleeptimer_timer_handle_t timer;
bool delay_timeout = false;
static sl_joystick_t sl_joystick_handle = JOYSTICK_HANDLE_DEFAULT;

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static void on_timeout(sl_sleeptimer_timer_handle_t *handle,
                       void *data);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize example.
 ******************************************************************************/
void app_joystick_init(void)
{
  /* Output on vcom usart instance */
  const char example_title[] = "Joystick example\r\n\r\n";

  /* Setting default stream */
  sl_iostream_set_default(sl_iostream_vcom_handle);

  printf("%s", example_title);

  /* Create timer for waking up the system periodically as per DELAY_MS */
  sl_sleeptimer_start_periodic_timer_ms(&timer,
                                        DELAY_MS,
                                        on_timeout, NULL,
                                        0,
                                        SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);

  /* Initialize the Joystick driver */
  sl_joystick_init(&sl_joystick_handle);

  /* Start Joystick data acquisition */
  sl_joystick_start(&sl_joystick_handle);
}

/***************************************************************************//**
 * Example ticking function.
 *
 * This function is called in the main while loop. It reads and prints joystick
 * position via VCOM at regular intervals specified by DELAY_MS.
 ******************************************************************************/
void app_joystick_process_action(void)
{
  sl_joystick_position_t pos;
  sl_status_t status = SL_STATUS_OK;

  /* Check if delay timer timed out */
  if (delay_timeout == true) {
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
    delay_timeout = false;
  }
}

/***************************************************************************//**
 * Sleeptimer timeout callback.
 ******************************************************************************/
static void on_timeout(sl_sleeptimer_timer_handle_t *handle,
                       void *data)
{
  (void)&handle;
  (void)&data;
  delay_timeout = true;
}
