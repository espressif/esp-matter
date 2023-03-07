#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "FreeRTOSConfigTasks.h"  // Parameters from this task configuration file is used to configure the FreeRTOS!

void enterPowerDown(uint32_t millis);
void exitPowerDown(uint32_t millis);

#define configPRE_SLEEP_PROCESSING enterPowerDown
#define configPOST_SLEEP_PROCESSING exitPowerDown

#define configNUM_SDK_THREAD_LOCAL_STORAGE_POINTERS 2

#define configTICK_RATE_HZ                1000
#define configUSE_PREEMPTION              1
#define configMAX_PRIORITIES              ( TASK_PRIORITY_MAX + 1 )  // Set the number of priority levels. (Not the MAX priority!)
#define configUSE_MUTEXES                 1
#define configSUPPORT_STATIC_ALLOCATION   1
#define configSUPPORT_DYNAMIC_ALLOCATION  1
#define configUSE_TIMERS                  1
#define configTIMER_TASK_PRIORITY         TASK_PRIORITY_FREERTOS_TIMER
#define configUSE_TASK_NOTIFICATIONS      1
#define configUSE_COUNTING_SEMAPHORES     1
#define configUSE_TRACE_FACILITY          1

#define INCLUDE_vTaskPrioritySet          1
#define INCLUDE_uxTaskPriorityGet         1
#define INCLUDE_vTaskDelete               1
#define INCLUDE_vTaskDelay                1
#define INCLUDE_xTimerPendFunctionCall    1
#define INCLUDE_xTaskGetCurrentTaskHandle 1

// Include target specific configuration
#include "FreeRTOSConfigTarget.h"

/*
 * Converts a time in milliseconds to a time in ticks.
 * This macro overrides a macro of the same name defined in projdefs.h that gets
 * uint32_t overflow in case xTimeInMs > 4.2950e6  ~1.2 hours.
 */
#define pdMS_TO_TICKS( xTimeInMs ) \
      ( ( TickType_t ) ( ( ( uint64_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )

#endif /* FREERTOS_CONFIG_H */
