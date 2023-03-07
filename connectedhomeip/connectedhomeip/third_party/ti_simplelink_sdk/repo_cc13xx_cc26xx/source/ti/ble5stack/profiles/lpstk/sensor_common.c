/******************************************************************************

 @file  sensor_common.c

 This file contains the implementation of the common sensor functionality

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
#include "stdint.h"
#include "ti_drivers_config.h"
#include "string.h"
#include "util.h"
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/common/cc26xx/uartlog/UartLog.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/utils/List.h>
#include <sail_syscfg_workaround.h>
#include <icall.h>
/* This Header file contains all BLE API and icall structure definition */
#include <icall_ble_api.h>
#include <temp_profile.h>
#include <hum_profile.h>
#include <button_profile.h>
#include <temp_profile.h>
#include <led_profile.h>
#include <hall_profile.h>
#include <accel_profile.h>
#include <light_profile.h>
#include <batt_profile.h>
#include <sensor_common.h>

/*********************************************************************
 * CONSTANTS
 */
// Task configuration
#define SC_TASK_PRIORITY                     1
// TODO: Profile this
#ifndef SC_TASK_STACK_SIZE
  #define SC_TASK_STACK_SIZE                 3000
#endif

// Types of messages that can be sent to the user application task from other
// tasks or interrupts. Note: Messages from BLE Stack are sent differently.
#define SC_DEFERRED_EVT          1  /* Used to switch context to sensor task */

// Internal Events for RTOS application
#define SC_ICALL_EVT                     ICALL_MSG_EVENT_ID  // Event_Id_31
#define SC_MSG_EVT                       Event_Id_30

// Bitwise OR of all RTOS events to pend on
#define SC_ALL_EVENTS                        (SC_ICALL_EVT         | \
                                              SC_MSG_EVT)

/*********************************************************************
 * TYPEDEFS
 */

// Struct for messages sent to the application task
typedef struct
{
    uint8_t event;
    void    *pData;
} scMsg_t;

// List element to map UUID's to sensor service callbacks
typedef struct
{
  List_Elem          elem;
  uint16_t           uuid;
  sensorProcessCBs_t cbs;
} service_cb_map_entry_t;

// TYpe of sensor service callback
typedef enum
{
  SENSOR_CB_VALUE_CHANGE = 0x00,
  SENSOR_CB_CFG_CHANGE   = 0x01
} sensor_cb_type_t;

/*********************************************************************
 * LOCAL VARIABLES
 */

// List mapping services to callbacks
static List_List service_cb_map;

// Shared I2C handle used among sensors
static I2C_Handle i2cHandle;

// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID selfEntity;

// Event globally used to post local events and pend on system and
// local events.
static ICall_SyncHandle syncEvent;

// Queue object used for app messages
static Queue_Struct appMsgQueue;
static Queue_Handle appMsgQueueHandle;

/// Semaphore for task initialization
static Semaphore_Struct semTaskReady;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void sensor_serviceCB(uint16_t connHandle, uint16_t svcUuid, uint8_t paramID,
                             uint16_t len, uint8_t *pValue, sensor_cb_type_t cbType);
static void sensor_serviceCfgChangeCB(uint16_t connHandle, uint16_t svcUuid,
                               uint8_t paramID, uint16_t len, uint8_t *pValue);
static void sensor_serviceValueChangeCB(uint16_t connHandle, uint16_t svcUuid,
                                        uint8_t paramID, uint16_t len,
                                        uint8_t *pValue);
static void Sensors_taskFxn(UArg a0, UArg a1);
static bStatus_t Sensors_enqueueMsg(uint8_t event,  void *pData);
static void Sensors_performDeferred(def_params_t *def_params);

/*********************************************************************
 * GLOBAL VARIABLES
 */

// IO structure to store SAIL handles
lpstkIo_t io = {0};

// Common service callback functions
sensorServiceCBs_t sensorServiceCBs =
{
  .pfnChangeCb = sensor_serviceValueChangeCB,  // Characteristic value change callback handler
  .pfnCfgChangeCb = sensor_serviceCfgChangeCB, // CCC change handler
};

// Task configuration
Task_Struct scTask;
#if defined __TI_COMPILER_VERSION__
  #pragma DATA_ALIGN(scTaskStack, 8)
#else
  #pragma data_alignment=8
#endif
uint8_t scTaskStack[SC_TASK_STACK_SIZE];

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*
 *  ======== Sensors_init ========
 */
bStatus_t Sensors_init(void)
{
    bStatus_t status = FAILURE;

    // Create a binary semaphore, initially blocked, to indicate
    // task initialization to application task
    Semaphore_Params semParams;
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    Semaphore_construct(&semTaskReady, 0, &semParams);

    // Configure task
    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.stack = scTaskStack;
    taskParams.stackSize = SC_TASK_STACK_SIZE;
    taskParams.priority = SC_TASK_PRIORITY;

    // Construct and start task
    Task_construct(&scTask, Sensors_taskFxn, &taskParams, NULL);

    // Wait for task to initialize
    if (Semaphore_pend(Semaphore_handle(&semTaskReady), 2000000 / Clock_tickPeriod))
    {
        status = SUCCESS;
    }
    else
    {
        status = FAILURE;
    }

    return status;
}

/*
 *  ======== Sensors_defer ========
 */
bStatus_t Sensors_defer(deferredTarget_t pFn, uint8_t *pParams)
{
    bStatus_t status = SUCCESS;

    // Allocate container for deferred function and params
    def_params_t *pData = ICall_malloc(sizeof(def_params_t));

    if (pData == NULL)
    {
      status = bleMemAllocError;
    }
    else
    {
        pData->pFn = pFn;
        pData->pParams = pParams;

        if(Sensors_enqueueMsg(SC_DEFERRED_EVT, pData) != SUCCESS)
        {
          if (pData != NULL)
          {
            ICall_free(pData);
          }
          status = bleMemAllocError;
        }
    }

    return(status);
}

/*
 *  ======== Sensors_disable ========
 */
bStatus_t Sensors_disable(void)
{
  // These sensors need to be explicitly disabled as they perform periodic
  // functionality
  TempProfile_disable();
  HumProfile_disable();
  LightProfile_disable();
  /** Disabled for simplelink_cc13x2_26x2_sdk_3_30_00 release **/
  //HallProfile_disable();
  BatteryProfile_disable();
  AccelProfile_disable();

  return SUCCESS;
}

/*
 *  ======== Sensors_registerCbs ========
 */
bStatus_t Sensors_registerCbs(uint16_t serviceUUID, sensorProcessCBs_t cbs)
{
  bStatus_t status = SUCCESS;

  service_cb_map_entry_t *entry = ICall_malloc(sizeof(service_cb_map_entry_t));

  if (entry == NULL)
  {
    status = bleMemAllocError;
  }
  else
  {
    entry->cbs = cbs;
    entry->uuid = serviceUUID;
    List_put(&service_cb_map, (List_Elem *)entry);
  }

  return status;
}

/*********************************************************************
 * PRIVATE FUNCTIONS
 */
static void Sensors_taskFxn(UArg a0, UArg a1)
{
  Log_info0("Sensor task started");

  // ******************************************************************
  // NO STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ******************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
  ICall_registerApp(&selfEntity, &syncEvent);

  // Initialize queue for application messages.
  // Note: Used to transfer control to application thread from e.g. interrupts.
  Queue_construct(&appMsgQueue, NULL);
  appMsgQueueHandle = Queue_handle(&appMsgQueue);

  // Clear service callback list (in case sensors_init was already called)
  List_clearList(&service_cb_map);

  // Initialize commonly used drivers
  GPIO_init();
  I2C_init();
  ADC_init();

  // Open shared I2C
  I2C_Params i2cParams;
  I2C_Params_init(&i2cParams);
  i2cParams.bitRate = I2C_100kHz;
  i2cHandle = I2C_open(I2C_SENSORS, &i2cParams);
  if (i2cHandle == NULL)
  {
    Log_error0("I2C handle failed to open");
  }
  else
  {
    Log_info0("I2C handle opened");

    // Try to initialize profiles / sensors that rely on I2C
    TempProfile_init(&i2cHandle);
    HumProfile_init(&i2cHandle);
    LightProfile_init(&i2cHandle);
  }

  // Try to initialize remaining profiles / sensors
  LEDProfile_init();
  ButtonProfile_init();
  BatteryProfile_init();
  AccelProfile_init();
  /** Disabled for simplelink_cc13x2_26x2_sdk_3_30_00 release **/
  //HallProfile_init();

  // Indicate to application task that initialization has completed
  Semaphore_post(Semaphore_handle(&semTaskReady));

  // Task main loop
  for(;;)
  {
      uint32_t events;

      // Waits for an event to be posted associated with the calling thread.
      // Note that an event associated with a thread is posted when a
      // message is queued to the message receive queue of the thread
      events = Event_pend(syncEvent, Event_Id_NONE, SC_ALL_EVENTS,
                          ICALL_TIMEOUT_FOREVER);

      if(events)
      {
          ICall_EntityID dest;
          ICall_ServiceEnum src;
          ICall_HciExtEvt *pMsg = NULL;

          // Fetch any available messages that might have been sent from the stack
          if(ICall_fetchServiceMsg(&src, &dest, (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
          {
            // We don't care about these but free in case we receive any
            ICall_freeMsg(pMsg);
          }

          // Process messages sent from another task or another context.
          while(!Queue_empty(appMsgQueueHandle))
          {
              scMsg_t *pMsg = (scMsg_t *)Util_dequeueMsg(appMsgQueueHandle);
              if(pMsg)
              {
                  if (pMsg->event == SC_DEFERRED_EVT)
                  {
                    Sensors_performDeferred((def_params_t *)(pMsg->pData));
                  }

                  // Free the received message.
                  ICall_free(pMsg);
              }
          }
      }
  }
}

/*
 *  ======== Sensors_clockHandler ========
 */
void Sensors_clockHandler(UArg deferred_target)
{
   // Call application to switch context
   Sensors_defer((deferredTarget_t)deferred_target, NULL);
}


/*********************************************************************
* PRIVATE FUNCTIONS
*/

/**
 * Characteristic value change callback used by all LPSTK sensor services
 *
 * @param connHandle connection handle
 * @param svcUUID UUID of service
 * @param paramID identifier of service characteristic
 * @param len length of data
 * @param pointer to data
 */
static void sensor_serviceValueChangeCB(uint16_t connHandle, uint16_t svcUuid,
                                         uint8_t paramID, uint16_t len, uint8_t *pValue)
{
  sensor_serviceCB(connHandle, svcUuid, paramID, len, pValue, SENSOR_CB_VALUE_CHANGE);
}

/**
 * Client Characteristic Configuration callback used by all LPSTK sensor services
 *
 * @param connHandle connection handle
 * @param svcUUID UUID of service
 * @param paramID identifier of service characteristic
 * @param len length of data
 * @param pointer to data
 */
static void sensor_serviceCfgChangeCB(uint16_t connHandle, uint16_t svcUuid,
                                      uint8_t paramID, uint16_t len, uint8_t *pValue)
{
  sensor_serviceCB(connHandle, svcUuid, paramID, len, pValue, SENSOR_CB_CFG_CHANGE);
}

/**
 * Sensor service callbacks used by both CCC and Value change callbacks
 *
 * Proxies information from service client characteristic / value change callback
 * functions into containers, finds service's processing functions, and calls
 * @ref Sensors_defer to defer this processing to the sensors context
 *
 * @param connHandle connection handle
 * @param svcUUID UUID of service
 * @param paramID identifier of service characteristic
 * @param len length of data
 * @param pointer to data
 * @param cbType whether this is a CCC or value change callback
 */
static void sensor_serviceCB(uint16_t connHandle, uint16_t svcUuid, uint8_t paramID,
                             uint16_t len, uint8_t *pValue, sensor_cb_type_t cbType)
{
  // Allocate container for params
  char_data_t *pData = (char_data_t*)ICall_malloc(sizeof(char_data_t));

  if (pData != NULL)
  {
    memcpy(pData->data, pValue, len);
    pData->dataLen = len;
    pData->connHandle = connHandle;
    pData->paramID = paramID;

    // Find callback from service UUID
    deferredTarget_t cb = NULL;
    List_Elem *temp = NULL;
    for (temp = List_head(&service_cb_map); temp != NULL; temp = List_next(temp))
    {
      if (((service_cb_map_entry_t *)temp)->uuid == svcUuid)
      {
        if (cbType == SENSOR_CB_VALUE_CHANGE)
        {
          cb = ((service_cb_map_entry_t *)temp)->cbs.pfnProcessVal;
        }
        else if (cbType == SENSOR_CB_CFG_CHANGE)
        {
          cb = ((service_cb_map_entry_t *)temp)->cbs.pfnProcessCfg;
        }
        break;
      }
    }

    if (cb == NULL)
    {
      if (pData != NULL)
      {
          ICall_free(pData);
      }
    }
    else
    {
      Sensors_defer(cb, (uint8_t *)pData);
    }
  }
}

/**
 * Used to call deferred target with deferred params from sensors context
 *
 * @Note this also takes care of freeing the deferred params container
 *
 * @param def_params deferred target function and optional deferred params
 */
static void Sensors_performDeferred(def_params_t *def_params)
{
  // Perform deferred functionality
  ((def_params)->pFn)(def_params->pParams);

  // Free data container
  if (def_params->pParams != NULL)
  {
      ICall_free(def_params->pParams);
  }
}

/**
 * Utility function that sends the event and data to the application.
 *         Handled in the task loop.
 *
 * @param  event    Event type
 * @param  pData    Pointer to message data
 *
 * @return SUCCESS message was allocated and queued,
 * @return FAILURE enqueueing failed
 * @return bleMemAllocError message couldn't be allocated
 */
static bStatus_t Sensors_enqueueMsg(uint8_t event, void *pData)
{
    uint8_t success;
    scMsg_t *pMsg = ICall_malloc(sizeof(scMsg_t));

    if(pMsg)
    {
        pMsg->event = event;
        pMsg->pData = pData;

        success = Util_enqueueMsg(appMsgQueueHandle, syncEvent, (uint8_t *)pMsg);
        return (success) ? SUCCESS : FAILURE;
    }

    return(bleMemAllocError);
}
