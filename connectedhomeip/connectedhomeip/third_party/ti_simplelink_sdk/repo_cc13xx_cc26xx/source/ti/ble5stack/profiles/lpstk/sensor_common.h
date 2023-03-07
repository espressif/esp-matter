/*
 * Copyright (c) 2017-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** ============================================================================
 *  @file       sensor_common.h
 *
 *  Sensor abstraction for using the Launchpad Sensor Tag (LPSTK) sensors
 *
 * # Overview #
 * The common sensors module is intended to provide a simple way to add any available
 * LPSTK sensors to an application. The module is designed so that it can handle
 * any number of the sensors existing. This means that the sensor GATT services will
 * be dynamically added based on which sensors are enabled, thus allowing a GATT client
 * to easily see which sensors are available. It is dependent on the SAIL drivers. Once
 * the common sensors module has been initialized, the handles of any sensors which
 * have successfully been enabled will be available for the application to use with SAIL.
 * The module creates its own task from @ref Sensors_init
 *
 * # Limitations #
 * The sensor controller (used to interface with the accelerometer) and the external
 * flash (used by OAD) share the same SPI. Because there is no arbitration between
 * the two, they can not be used simultaneously. This is accomplished in the sample
 * app (multi_sensor.c) by:
 *  1. Enable OAD (with OAD_open()) before calling @ref Sensors_init
 *  2. Disable sensors (with @ref Sensors_disable) when OAD is started. This would
 *     be desired anyway in order to maximize bandwidth for OAD
 *
 * # Module include #
 * The common sensor header file should be included in an application as follows:
 * @code
 * #include <sensor_common.h>
 * @endcode
 *
 * # Usage #
 * First,
 * @code
 * Sensors_init();
 * @endcode
 *
 * This kicks off the common sensors module. It will create a common
 * sensors module task which will then
 * - Initialize common hardware (GPIO, PWM, I2C, ADC)
 * - Initialize Sensor Profiles which will in turn initialize
 *   GATT services and enable sensors
 * - Populate a mapping of sensor service UUID's to callback functions
 *
 * @Note This module is designed so that it can handle any number
 * of the sensors not existing. Therefore, @ref Sensors_init will return SUCCESS
 * regardless of whether the sensors / services have been enabled successfully.
 * If desired, inspect @ref io for more detailed information on which sensors
 * have been enabled. This also means that the sensor GATT services will be
 * dynamically added based on which sensors are enabled, thus allowing a GATT
 * client to easily see which sensors are available.
 *
 * From this point, the common sensors module will run autonomously without
 * interaction with the application. Many sensors run synchronously via periodic
 * timers. These can all be stopped at once via:
 * @code
 * Sensors_disable();
 * @endcode
 *
 * The client can access the SAIL driver handles that the common sensors module
 * has initialized via the @ref io global variable. These handles can be used to
 * interface with the sensors using the SAIL drivers. The sensor task is created
 * at the lowest priority (1) and the sensors are only used by the common sensors
 * module in the sensor task context. Therefore, it is not necessary to surround
 * the use of these handles with critical sections. Furthermore, the SAIL drivers
 * are re-entrant and the lower-layer drivers (GPIO, PWM, I2C, and ADC) protect
 * any access with critical sections.
 *
 * # Deferring Processing #
 * One major functionality of the common sensors module is to provide a common way
 * for the various sensors, sensor profiles, and sensor services to defer processing
 * from a critical context (SWI, HWI, or BLE Stack) to a safer context (specifically
 * the sensor task). This is accomplished as such:
 *
 * 1. @ref Sensors_defer is called with a target function and, optionally, a pointer
 * to a container of this function's parameters. Consider the example from the
 * button_callback SWI in button_profile.c:
 * @code
 * // Allocate container for button event
 * buttonEventData_t *pData = (buttonEventData_t*)ICall_malloc(sizeof(buttonEventData_t));
 *
 * if (pData != NULL)
 * {
 *  // Fill up Container
 *   pData->buttonEvents = buttonEvents;
 *   pData->buttonHandle = buttonHandle;
 *
 *   // Defer to process in application context
 *   Sensors_defer(profile_readSensor, (uint8_t *)pData);
 * }
 * @endcode
 *
 * 2. @ref Sensors_defer will then enqueue this information as an event to the
 * sensors task and call @ref Sensors_performDeferred when it processes this
 * event in the sensors task context. It will also handle freeing the memory
 * associated with this event. In this example, the button profile will then
 * process profile_readSensor
 *
 * @Note that the client (the button profile in this example) is still
 * responsible for freeing the container memory (i.e. buttonEventData_t *pData
 * allocated above)
 *
 *  ============================================================================
 */

#ifndef SENSOR_COMMON_H
#define SENSOR_COMMON_H

/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "gatt.h"
#include <xdc/std.h>
#include <ti/drivers/ADC.h>
#include <ti/drivers/apps/LED.h>
#include <ti/drivers/apps/Button.h>
#include <ti/common/sail/hdc2010/hdc2010.h>
#include <ti/common/sail/opt3001/opt3001.h>

/*********************************************************************
 * MACROS
 */
// TI Base 128-bit UUID: F000XXXX-0451-4000-B000-000000000000
#define TI_UUID_SIZE        ATT_UUID_SIZE
#define TI_UUID(uuid)       TI_BASE_UUID_128(uuid)

// Utility string macros
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

/**
 * Profile Parameter Identifiers
 *
 * These are not common to every sensor GATT service but are shared by many
 */
typedef enum
{
  SENSOR_DATA  = 0,
  SENSOR_CONF  = 1,
  SENSOR_PERI  = 2
} sensor_char_id_t;

// Data readout periods (range 100 - 2550 ms)
#define SENSOR_MIN_UPDATE_PERIOD        100     // Minimum 100 milliseconds
#define SENSOR_PERIOD_RESOLUTION        10      // Resolution 10 milliseconds
#define SENSOR_DEFAULT_PERIOD           1000    // 1 second

// Common values for turning a sensor on and off + config/status
#define ST_CFG_SENSOR_DISABLE           0x00
#define ST_CFG_SENSOR_ENABLE            0x01
#define ST_CFG_ERROR                    0xFF

/*********************************************************************
 * PROFILE CALLBACKS
 */

/**
 *  Target function to be used as function pointer parameter of
 *  deferrer function @ref Sensors_defer
 */
typedef bStatus_t (*deferredTarget_t)(uint8_t *pParams);

/// Shared callback for all service data
typedef void (*sensorServiceCB_t)(uint16_t connHandle, uint16_t svcUuid,
                                  uint8_t paramID, uint16_t len, uint8_t *pValue);

/// Sensor service callbacks common to all sensors
typedef struct
{
  sensorServiceCB_t pfnChangeCb;    // Called when characteristic value changes
  sensorServiceCB_t pfnCfgChangeCb; // Called when CCC changes
} sensorServiceCBs_t;

/// Callbacks to process data from sensor service callbacks
typedef struct
{
  deferredTarget_t pfnProcessVal;
  deferredTarget_t pfnProcessCfg;
} sensorProcessCBs_t;

/*********************************************************************
 * TYPEDEFS
 */

/// Container for @ref Sensors_performDeferred
typedef struct
{
  deferredTarget_t  pFn;
  uint8_t           *pParams;
} def_params_t;

/// Container to store information from clock expirations
typedef struct
{
  uint16_t          connHandle;
  deferredTarget_t  pFn;
  uint8_t           params[];
} clockEventData_t;

/// Container used for all service data updates
typedef struct
{
  uint16_t connHandle; // connection handle
  uint16_t dataLen; //
  uint8_t  paramID; // Index of the characteristic
  uint8_t  data[];  // Flexible array member,
} char_data_t;


/*********************************************************************
 * Shared Input / Output information
 */

/// LPSTK LED information storage
typedef struct
{
  LED_Handle red;
  LED_Handle blue;
  LED_Handle green;
  bool       ready;
} lpstkLED_t;

/// LPSTK button information storage
typedef struct
{
  Button_Handle zero;
  Button_Handle one;
} lpstkButton_t;

/// LPSTK HDC2010 information storage
typedef struct
{
  HDC2010_Handle  handle;
  bool            temp_enabled;
  bool            hum_enabled;
} lpstkHdc_t;

/**
 * LPSTK I/O information container
 *
 * This stores SAIL driver handles and other I/O initialization information to
 * allow the various I/O to be used across sensors as well as the client of the
 * common sensor service (i.e. the application)
 *
 * The entire structure should be initialized to zero. Individual elements will be
 * set if / as they are initialized by @ref Sensors_init
 */
typedef struct
{
  lpstkLED_t     led;
  lpstkButton_t  button;
  lpstkHdc_t     hdc;
  OPT3001_Handle opt;
  ADC_Handle   adc;
} lpstkIo_t;

/********************************************************************
 * FUNCTIONS
 */

/**
 * Top level sensor initialization
 *
 * @param cb Callback used by this module to defer processing to
 *        the application context
 *
 * @return SUCCESS Task, sensors, and services have been initialized
 * @return FAILURE Task failed to initialize
 */
extern bStatus_t Sensors_init(void);

/**
 * Disable any profiles that explicitly need to be disabled (i.e. their
 * sensors perform readings periodically)
 *
 * This will disable the following profiles:
 *   - Temperature
 *   - Humidity
 *   - Light
 *   - Hall
 *   - Battery
 *   - Accelerometer
 *
 * @return SUCCESS
 */
extern bStatus_t Sensors_disable(void);

/**
 * Registers a sensor profile with the common sensors module
 *
 * This allows the common sensors module to translate the profile's service UUID
 * to the profile's processing function. Therefore, when the service calls the common
 * sensors service value / CCC callback functions, the common sensors module will
 * dispatch the processing to the appropriate profile processing function (after deferring
 * context from the stack to the application)
 */
extern bStatus_t Sensors_registerCbs(uint16_t serviceUUID, sensorProcessCBs_t cbs);

/**
 * Used to defer processing from another context to the multi-sensor
 * task context
 *
 * This will enqueue a SC_DEFERRED_EVT with a target function pointer
 * (pFn) and parameters (pParams). When SC_DEFERRED_EVT is processed
 * in @ref Sensors_taskFxn, this target function will be called from
 * the sensors task context.
 *
 * @param pFn - Pointer to the function that is being deferred
 * @param pParams - parameters to pass into target function
 *
 * @return SUCCESS if SC_DEFERRED_EVT was successfully enqueued
 * @return bleMemAllocError if not able to allocate event
 */
extern bStatus_t Sensors_defer(deferredTarget_t pFn, uint8_t *pParams);

/**
 * Common clock handler used
 *
 * This clock handler is used by all Profile's to defer processing of the
 * clock event from the SWI to application context by passing a deferred
 * target
 *
 * @return deferred_target Target function to be called after deferring context
 */
extern void Sensors_clockHandler(UArg deferred_target);

/***************************************************************************
 * VARIABLES
 */

/// Common sensor processing callbacks to be used by sensor services
extern sensorServiceCBs_t sensorServiceCBs;

/// Input / output information storage
extern lpstkIo_t io;

#endif /* SENSOR_COMMON_H */
