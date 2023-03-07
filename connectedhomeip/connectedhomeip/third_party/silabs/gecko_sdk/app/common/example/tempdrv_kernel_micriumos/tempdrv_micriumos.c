/***************************************************************************//**
 * @file
 * @brief tempdrv micriumos examples functions
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
#include <string.h>
#include <stdio.h>
#include "tempdrv_micriumos.h"
#include "tempdrv.h"
#include "sl_simple_led.h"
#include "sl_simple_led_instances.h"
#include "os.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

// MicriumOS task configuration
#define TEMPDRV_TASK_STACK_SIZE      256
#define TEMPDRV_TASK_PRIO            20

// LED_INSTANCE_LO will turn on when the temperature drops
// below configured threshold
#define LED_INSTANCE_LO             sl_led_led0

// LED_INSTANCE_HI will turn on when the temperature exceeds
// configured threshold
#define LED_INSTANCE_HI             sl_led_led1

// The limits are set relative to the initial temperature
#define TEMPERATURE_BAND_C          10

// Hysteresis for registering callback fired when temperature returns to normal
#define HYSTERESIS_C                2

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static OS_TCB tcb;
static CPU_STK stack[TEMPDRV_TASK_STACK_SIZE];
static uint8_t start_temperature;
static uint8_t high_limit;
static uint8_t low_limit;

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

// Application task
static void tempdrv_task(void *arg);

/***************************************************************************//**
 * Temperature lower limit callback
 *
 * This function is called when the measured temperature drops below
 * the configured threshold
 ******************************************************************************/
static void tempCallback_lower(int8_t temp, TEMPDRV_LimitType_t limit)
{
  (void)(temp);

  // When a callback fires, it is un-registered and has to be
  // re-registered.
  if (limit == TEMPDRV_LIMIT_LOW) {
    // temperature has decreased below lower threshold
    sl_led_turn_on(&LED_INSTANCE_LO);

    // register callback for return to normal temperature
    // with hysteresis
    TEMPDRV_RegisterCallback(low_limit + HYSTERESIS_C,
                             TEMPDRV_LIMIT_HIGH,
                             tempCallback_lower);
  } else {
    // temperature has returned to normal
    sl_led_turn_off(&LED_INSTANCE_LO);

    // re-register callback for low temperature
    TEMPDRV_RegisterCallback(low_limit,
                             TEMPDRV_LIMIT_LOW,
                             tempCallback_lower);
  }
}

/***************************************************************************//**
 * Temperature upper limit callback
 *
 * This function is called when the measured temperature exceeds
 * the configured threshold
 ******************************************************************************/
static void tempCallback_upper(int8_t temp, TEMPDRV_LimitType_t limit)
{
  (void)(temp);

  // When a callback fires, it is un-registered and has to be
  // re-registered.
  if (limit == TEMPDRV_LIMIT_HIGH) {
    // temperature has increased above upper threshold
    sl_led_turn_on(&LED_INSTANCE_HI);

    // register callback for return to normal temperature
    // with hysteresis
    TEMPDRV_RegisterCallback(high_limit - HYSTERESIS_C,
                             TEMPDRV_LIMIT_LOW,
                             tempCallback_upper);
  } else {
    // temperature has returned to normal
    sl_led_turn_off(&LED_INSTANCE_HI);

    // re-register callback for high temperature
    TEMPDRV_RegisterCallback(high_limit,
                             TEMPDRV_LIMIT_HIGH,
                             tempCallback_upper);
  }
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/*******************************************************************************
 * Initialize tempdrv example.
 ******************************************************************************/
void tempdrv_init(void)
{
  RTOS_ERR err;

  TEMPDRV_Init();

  // printf is configured to redirect to vcom in project file
  printf("\r\nWelcome to the tempdrv sample application\r\n\r\n");

  // Initialise LEDS to off
  sl_led_turn_off(&LED_INSTANCE_LO);
  sl_led_turn_off(&LED_INSTANCE_HI);

  //Create tempdrv Task
  OSTaskCreate(&tcb,
               "tempdrv task",
               tempdrv_task,
               DEF_NULL,
               TEMPDRV_TASK_PRIO,
               &stack[0],
               (TEMPDRV_TASK_STACK_SIZE / 10u),
               TEMPDRV_TASK_STACK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
}

/***************************************************************************//**
 * Tempdrv task.
 ******************************************************************************/
static void tempdrv_task(void *arg)
{
  (void)&arg;
  Ecode_t ecode;
  RTOS_ERR err;

  // Get current temperature
  start_temperature = TEMPDRV_GetTemp();

  // Output on vcom
  printf("Initial temperature = %u C\r\n", start_temperature);

  // Set temperature limits based on initial temperature. Registering
  // a callback that would fire immediately is not supported.
  low_limit = start_temperature - (TEMPERATURE_BAND_C / 2);
  high_limit = start_temperature + (TEMPERATURE_BAND_C / 2);

  // Register callbacks with calculated thresholds.
  ecode = TEMPDRV_RegisterCallback(low_limit,
                                   TEMPDRV_LIMIT_LOW,
                                   tempCallback_lower);
  EFM_ASSERT(ecode == ECODE_EMDRV_TEMPDRV_OK);

  ecode = TEMPDRV_RegisterCallback(high_limit,
                                   TEMPDRV_LIMIT_HIGH,
                                   tempCallback_upper);
  EFM_ASSERT(ecode == ECODE_EMDRV_TEMPDRV_OK);

  while (1) {
    // Periodically output temperature to VCOM
    OSTimeDlyHMSM(0, 0, 10, 0, OS_OPT_TIME_DLY, &err);
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
    printf("Current temperature = %u C\r\n", TEMPDRV_GetTemp());
  }
}
