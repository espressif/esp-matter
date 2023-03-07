/***************************************************************************//**
 * @file
 * @brief Top level application functions
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
#include "tflite_micro_hello_world.h"
#include "main_functions.h"
#include "sl_sleeptimer.h"
#include "sl_pwm_instances.h"
#include "sl_pwm.h"

/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/
void tflite_micro_hello_world_init(void)
{
  sl_pwm_start(&sl_pwm_led0);
  setup();
}

/***************************************************************************//**
 * Ticking function.
 ******************************************************************************/
void tflite_micro_hello_world_process_action(void)
{
  // Delay between model inferences to simplify visible output
  sl_sleeptimer_delay_millisecond(100);
  loop();
}
