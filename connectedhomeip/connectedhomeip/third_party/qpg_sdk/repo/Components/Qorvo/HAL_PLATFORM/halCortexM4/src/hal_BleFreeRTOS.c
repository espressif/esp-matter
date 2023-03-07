/*
 * Copyright (c) 2021, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/** @file Implementation of FreeRTOS specific BLE functions */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_HALCORTEXM4
#include "gpHal.h"
#include "gpHal_DEFS.h"
#include "gp_kx.h"
#include "gpAssert.h"
#include "gpLog.h"
#include "gpPd.h"
#include "gpSched.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "hal_BleFreeRTOS.h"
/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define BLE_TASK_NAME           ("BLE Task")
#define BLE_TASK_PRIORITY       (configMAX_PRIORITIES - 1)
// The minimum stack size must be >= 256 (with BleMesh 512)
#define BLE_STACK_SIZE          512
#define BLE_EVENT_QUEUE_LENGTH  2

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definition
 *****************************************************************************/
typedef struct {
    UInt32                  event;
    UInt32                  arg;
} bleEventQueueElement_t;

/*****************************************************************************
 *                    Local Variable
 *****************************************************************************/
#if configSUPPORT_STATIC_ALLOCATION
/* Buffer that the task being created will use as its stack.  Note this is
an array of StackType_t variables.  The size of StackType_t is dependent on
the RTOS port. */
StackType_t xBleStack[ BLE_STACK_SIZE ];
/* Structure that will hold the TCB of the task being created. */
StaticTask_t xBleTaskBuffer;

/* The variable used to hold the scheduler event queue's data structure. */
static StaticQueue_t xBleEventStaticQueue;
/* The array to use as the scheduler event queue's storage area.  This must be at least
uxQueueLength * uxItemSize bytes. */
uint8_t ucBleEventQueueStorageArea[ BLE_EVENT_QUEUE_LENGTH * sizeof(bleEventQueueElement_t) ];
#endif //configSUPPORT_STATIC_ALLOCATION

TaskHandle_t xBleTaskh;

QueueHandle_t xBleISREventQueue;

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
static void vBleTask( void * pvParameters )
{
    (void) pvParameters;

    for(;;)
    {
        bleEventQueueElement_t queueElement = {0};

        // If INCLUDE_vTaskSuspend is set to '1' then specifying the block time as portMAX_DELAY
        // will cause the task to block indefinitely (without a timeout).
        if( xQueueReceive( xBleISREventQueue,
                           &( queueElement ),
                           ( TickType_t ) portMAX_DELAY ) == pdPASS )
        {
            if (queueElement.event == GP_BLE_TASK_EVENT_RCI_CONN_REQ)
            {
                gpHal_cbBleConnectionRequestIndication(queueElement.arg);
                gpSched_NotifySchedTask();
            }
            else if (queueElement.event == GP_BLE_TASK_EVENT_RCI_CONN_RSP)
            {
            }
            else if (queueElement.event == GP_BLE_TASK_EVENT_IPCGPM)
            {
                gpHal_cbBleConnectionRequestConfirm();
            }
        }
    }
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
void hal_BleIsrRciDefer(UInt8 event, UInt8 pbmEntry)
{
    bleEventQueueElement_t queueElement = {
                .event = event,
                .arg = pbmEntry,
            };

    BaseType_t xHigherPriorityTaskWoken = pdFALSE; /* Initialised to pdFALSE. */
    if (xQueueSendFromISR( xBleISREventQueue, &queueElement, &xHigherPriorityTaskWoken ) == pdTRUE)
    {
        GP_LOG_PRINTF("BLE deferred Rci INT", 0);
    }
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void hal_BleIsrIPCGPMDefer(void)
{
    bleEventQueueElement_t queueElement = {
                .event = GP_BLE_TASK_EVENT_IPCGPM,
                .arg = 0,
            };

    BaseType_t xHigherPriorityTaskWoken = pdFALSE; /* Initialised to pdFALSE. */
    if (xQueueSendFromISR( xBleISREventQueue, &queueElement, &xHigherPriorityTaskWoken ) == pdTRUE)
    {
        GP_LOG_PRINTF("BLE deferred IPCGPM INT", 0);
    }
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void hal_BleTaskCreate(void)
{    
#if configSUPPORT_STATIC_ALLOCATION
    xBleISREventQueue = xQueueCreateStatic( BLE_EVENT_QUEUE_LENGTH,
                                            sizeof( bleEventQueueElement_t ),
                                            ucBleEventQueueStorageArea,
                                            &xBleEventStaticQueue );
#else
    xBleISREventQueue = xQueueCreate( BLE_EVENT_QUEUE_LENGTH, sizeof( bleEventQueueElement_t ) );
#endif //configSUPPORT_STATIC_ALLOCATION
    GP_ASSERT_SYSTEM( xBleISREventQueue );

#if configSUPPORT_STATIC_ALLOCATION
    xBleTaskh = xTaskCreateStatic( vBleTask,                /* Function that implements the task. */
                                   BLE_TASK_NAME,           /* Text name for the task. */
                                   BLE_STACK_SIZE,          /* Number of indexes in the xStack array. */
                                   NULL,                    /* Parameter passed into the task. */
                                   BLE_TASK_PRIORITY,       /* Priority at which the task is created. */
                                   xBleStack,               /* Array to use as the task's stack. */
                                   &xBleTaskBuffer );       /* Variable to hold the task's data structure. */
#else
    (void)xTaskCreate(vBleTask,
                      BLE_TASK_NAME,
                      BLE_STACK_SIZE,
                      NULL,
                      BLE_TASK_PRIORITY,
                      &xBleTaskh);
#endif
    GP_ASSERT_SYSTEM( xBleTaskh );
}
