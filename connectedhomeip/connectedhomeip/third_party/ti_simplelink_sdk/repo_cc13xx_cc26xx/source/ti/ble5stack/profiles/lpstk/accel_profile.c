/******************************************************************************

 @file  accel_profile.c

 This file contains the profile implementation for the accelerometer sensor
 and accelerometer service.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include <ti_drivers_config.h>
#include <string.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <scif.h>
#include <util.h>
#include <icall.h>
/* This Header file contains all BLE API and icall structure definition */
#include <icall_ble_api.h>
#include <sensor_common.h>
#include <accel_service.h>
#include <accel_profile.h>
#include <ti/common/cc26xx/uartlog/UartLog.h>

#include <ti/sysbios/knl/Task.h>

/*********************************************************************
 * LOCAL VARIABLES
 */

/// Semaphore for sensor control READY signal
static Semaphore_Struct semScReady;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static bStatus_t profile_readSensor(uint8_t *params);
static bStatus_t profile_processValueChange(uint8_t *pParam);

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Service callback function implementation
static sensorProcessCBs_t processCBs =
{
  .pfnProcessVal = profile_processValueChange, // Characteristic value change callback handler
  .pfnProcessCfg = NULL, // No CCCD change handler implemented
};

// SCIF driver callback: Task control interface ready (non-blocking task control operation completed)
void scCtrlReadyCallback(void);

// SCIF driver callback: Sensor Controller task code has generated an alert interrupt
void scTaskAlertCallback(void);

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*
 *  ======== AccelProfile_init ========
 */
bStatus_t AccelProfile_init(void)
{
  bStatus_t status = SUCCESS;

  // Create a binary semaphore, initially blocked.
  Semaphore_Params semParams;
  Semaphore_Params_init(&semParams);
  semParams.mode = Semaphore_Mode_BINARY;
  Semaphore_construct(&semScReady, 0, &semParams);

  // Initialize the SCIF operating system abstraction layer
  scifOsalInit();
  scifOsalRegisterCtrlReadyCallback(scCtrlReadyCallback);
  scifOsalRegisterTaskAlertCallback(scTaskAlertCallback);
  // Initialize the SCIF driver
  scifInit(&scifDriverSetup);

  // Wait for sensor controller to initialize
  if (!(Semaphore_pend(Semaphore_handle(&semScReady), 500000 / Clock_tickPeriod)))
  {
    Log_error0("Sensor controller failed to initialize");
    return FAILURE;
  }

  // Register with common sensors module
  status = Sensors_registerCbs(ACCEL_SERVICE_UUID, processCBs);

  if (status == SUCCESS)
  {
    // Add service
    status = Accel_AddService();
  }

  if (status == SUCCESS)
  {
    // Register callbacks with service
    status = Accel_RegisterProfileCBs(&sensorServiceCBs);
  }

  return status;
}

/*
 *  ======== AccelProfile_enable ========
 */
bStatus_t AccelProfile_enable(void)
{
  bStatus_t status = SUCCESS;

  // Only enable if not already enabled
  if (!(scifGetActiveTaskIds() & (1 << SCIF_SPI_ACCELEROMETER_TASK_ID)))
  {
    // Start the "SPI Accelerometer" Sensor Controller task
    scifStartTasksNbl(1 << SCIF_SPI_ACCELEROMETER_TASK_ID);

    // Wait for sensor controller
    if (!(Semaphore_pend(Semaphore_handle(&semScReady), 500000 / Clock_tickPeriod)))
    {
      status = FAILURE;
    }
  }

  if (status == SUCCESS)
  {
      Log_info0("Sensor Controller Accelerometer enabled");
  }
  else
  {
     Log_error0("Sensor controller failed to enable");
  }

  return status;
}

/*
 *  ======== AccelProfile_disable ========
 */
bStatus_t AccelProfile_disable(void)
{
  bStatus_t status = SUCCESS;

  // Only disable if not already enabled
  if (scifGetActiveTaskIds() & (1 << SCIF_SPI_ACCELEROMETER_TASK_ID))
  {
    // Start the "SPI Accelerometer" Sensor Controller task
    scifStopTasksNbl(1 << SCIF_SPI_ACCELEROMETER_TASK_ID);

    // Wait for sensor controller
    if (!(Semaphore_pend(Semaphore_handle(&semScReady), 500000 / Clock_tickPeriod)))
    {
      status = FAILURE;
    }
  }

  if (status == SUCCESS)
  {
      Log_info0("Sensor Controller Accelerometer disabled");
  }
  else
  {
     Log_error0("Sensor controller failed to disable");
  }

  return status;
}

/*********************************************************************
* Private functions
*/

/**
 * Callback in sensor controller task context notifying of available data
 */
void scTaskAlertCallback(void)
{
  // Clear the ALERT interrupt source
  scifClearAlertIntSource();

  // Read sensor in sensor task
  Sensors_defer(profile_readSensor, NULL);
}

/**
 * Callback in sensor controller task to set ready flag
 */
void scCtrlReadyCallback(void)
{
  // Indicate to sensors task that sensor controller is ready
  Semaphore_post(Semaphore_handle(&semScReady));
}

/**
 * Read accelerometer data from sensor controller and send over the air
 *
 * @param params not used
 *
 * @return Always SUCCESS
 */
static bStatus_t profile_readSensor(uint8_t *params)
{
  // Read accelerometer data
  Log_info1("X: %d", scifTaskData.spiAccelerometer.output.x);
  Log_info1("Y: %d", scifTaskData.spiAccelerometer.output.y);
  Log_info1("Z: %d", scifTaskData.spiAccelerometer.output.z);

  // Set profile value (and notify if notifications are enabled)
  Accel_SetParameter(ACCEL_X_ID, ACCEL_X_LENGTH, (void *)&(scifTaskData.spiAccelerometer.output.x));
  Accel_SetParameter(ACCEL_Y_ID, ACCEL_Y_LENGTH, (void *)&(scifTaskData.spiAccelerometer.output.y));
  Accel_SetParameter(ACCEL_Z_ID, ACCEL_Z_LENGTH, (void *)&(scifTaskData.spiAccelerometer.output.z));

  // Acknowledge the ALERT event
  scifAckAlertEvents();

  return SUCCESS;
}

/**
 * Handle the changing of a service's characteristic value
 *
 * @param characteristic data: should be cast to char_data_t
 *
 * @return Always SUCCESS
 */
static bStatus_t profile_processValueChange(uint8_t *pParam)
{
  char_data_t *pCharData = (char_data_t *) pParam;

  switch (pCharData->paramID)
  {
    case ACCEL_ENABLER_ID:
    {
      if (*(pCharData->data) == ST_CFG_SENSOR_DISABLE)
      {
        AccelProfile_disable();
      }
      else if (*(pCharData->data) == ST_CFG_SENSOR_ENABLE)
      {
        AccelProfile_enable();
      }
    }
    break;
  }

  return SUCCESS;
}

