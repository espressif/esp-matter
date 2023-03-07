 /*
 * Copyright (c) 2017-2018, Qorvo Inc
 *
 * gpHal_Calibration.c
 * Support periodic recalibration of hardware.
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

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL
//#define GP_LOCAL_LOG

#include "hal.h"
#include "gpHal.h"
#include "gpHal_Calibration.h"
#include "gpHal_kx_Ipc.h"
#include "gpAssert.h"
#include "gpLog.h"
#include "gpSched.h"
#include "gpHal_Phy.h"
#include "gpStat.h"

#ifdef GP_DIVERSITY_FREERTOS
#include "timers.h"
#endif
/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

// Interval in which chip will wake up to do a calibration
// In non scheduled case, calibrations will happen when chip wakes
// up each time and when it handles interrupts
// The max temperature slope supported is 1 C change per 8 s. So setting
// the temperature based calibration tasks to run every min in the worst
// case (if not run in an unscheduled manner already) should capture any
// 10C change
#define CALIBRATION_TASK_SCHED_INTERVAL_US 60000000

// Number of temperature measurements to average to get a stable measurement value
#define NOF_TEMP_MEASUREMENTS_TO_AVG 4

// Peripherial timer used by calibration task
#define HAL_CALIBRATION_TIMER HAL_TIMER_2

// rate of calibration timer
#define CALIBRATION_TIMER_RATE_HZ (1000000 / GP_HAL_CALIBRATION_CHECK_INTERVAL_US)
#define CALIBRATION_TIMER_PRESCALER 3
/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef struct {
    gpHal_CalibrationTask_t task;
    gpHal_cbCalibrationHandler_t cbHandler;
    UInt32 nextCalibrationTime;
    Q8_8 lastCalibrationTemperature;
    Bool IsFirstCalibrationAfterWakeup;
} gpHal_CalibrationTaskInfo_t;


/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

// Number of calibration tasks defined.
static UInt8   HalCalibration_NrOfTasks;

// Last measured temperature.
static Q8_8  lastMeasuredTemperature;

// Time of next check for time based events.
static UInt32  HalCalibration_NextCheckTime;

// Time of next check for temperature based  events.
static UInt32  HalCalibrationTempBased_NextCheckTime;

// Calibration task data.
static gpHal_CalibrationTaskInfo_t HalCalibration_TaskInfo[GP_HAL_CALIBRATION_MAX_TASKS];

// Pending calibration on next wakeup
static UInt8 HalCalibration_PendingOnWakeup;

// First calibration after wakeup
static UInt8 HalCalibration_FirstAfterWakeup;

// Variables used for averaging multiple temperature measurements
static UInt8 temperatureMeasurementCounter = 0;
static Int32 temperatureSum = 0;

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

static Bool gpHal_GetCalibrationPendingOnWakeup(UInt8 calTaskId)
{
    return ((HalCalibration_PendingOnWakeup & (1 << calTaskId)) != 0);
}

#ifdef GP_DIVERSITY_FREERTOS
static void gpHal_CalibrationPendingInternal( void *param1, uint32_t param2)
{
    (void)param1;
    (void)param2;
    gpHal_CalibrationHandleTasks();
}

static void gpHal_CalibrationPending(void)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        GP_LOG_PRINTF("FreeRTOS calibration task pending",0);
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTimerPendFunctionCallFromISR(gpHal_CalibrationPendingInternal, NULL, 0, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken )
    }
}

static void gpHal_InitCalibrationTimer(halTimer_timerId_t timerId)
{
    /* Configure Timer to interrupt at the requested rate. */
    halTimer_initTimer(timerId, CALIBRATION_TIMER_PRESCALER, HAL_TIMER_CLKSELINTCLK,
                       (1000000 * (64 >> HAL_GET_MCU_CLOCK_SPEED()) / CALIBRATION_TIMER_RATE_HZ / (1 << CALIBRATION_TIMER_PRESCALER) / 2) - 1UL,
                       gpHal_CalibrationPending,
                       true);
    halTimer_startTimer(timerId);
}
#endif

static void Hal_TimeBasedCalibration(void)
{
    UIntLoop i;
    UInt32 currentTime;
    gpHal_CalibrationFlags_t flags;
    Bool recal;

    HAL_TIMER_GET_CURRENT_TIME_1US(currentTime);
    // Loop over calibration tasks.
    for (i = 0; i < HalCalibration_NrOfTasks; i++)
    {
        flags = HalCalibration_TaskInfo[i].task.flags;
        recal = false;

        if(gpHal_GetCalibrationPendingOnWakeup(i) && gpHal_CalibrationGetFirstAfterWakeup())
        {
            // Pending calibration on wakeup for this particular task i
            recal = true;
        }
        else if (((flags & GP_HAL_CALIBRATION_FLAG_PERIODIC) != 0) &&
           !GP_SCHED_TIME_COMPARE_LOWER_US(currentTime, HalCalibration_TaskInfo[i].nextCalibrationTime))
        {
            // Max time between calibrations reached.
            recal = true;
        }
        else if ((flags & GP_HAL_CALIBRATION_FLAG_CALIBRATE_ON_CHIP_WAKEUP) &&
                gpHal_CalibrationGetFirstAfterWakeup())
        {
            // This task needs to be calibrated every time chip wakes up from sleep
            recal = true;
        }

        if (recal)
        {
            // Setup for next calibration.
            HalCalibration_TaskInfo[i].nextCalibrationTime = currentTime + HalCalibration_TaskInfo[i].task.calibrationPeriod;
            // Calibrate.
            GP_LOG_PRINTF("Calibrate %u", 0, i);
            HalCalibration_TaskInfo[i].cbHandler(&HalCalibration_TaskInfo[i].task);
        }
    }

    /* If its first time the calibration sequence is run after waking up, clear the flag set
    by reset handler that indicates that */
    if (gpHal_CalibrationGetFirstAfterWakeup())
    {
        gpHal_CalibrationSetFirstAfterWakeup(false);
    }
}

static void Hal_TemperatureBasedCalibration(void)
{
    Q8_8 currentTemperature;
    UIntLoop i;
    gpHal_CalibrationFlags_t flags;
    Bool recal;
    GP_LOG_PRINTF("temp_cal", 0);

#ifdef GP_COMP_HALCORTEXM4
    currentTemperature = halADC_MeasureTemperature();
    if (currentTemperature != GP_HAL_ADC_INVALID_TEMPERATURE)
    {
        if (temperatureMeasurementCounter < NOF_TEMP_MEASUREMENTS_TO_AVG)
        {
            temperatureMeasurementCounter++;
            temperatureSum += currentTemperature;
            return;
        }
        else
        {
            temperatureMeasurementCounter = 0;
            currentTemperature = (Q8_8)(Int32)(temperatureSum / NOF_TEMP_MEASUREMENTS_TO_AVG);
            lastMeasuredTemperature = currentTemperature;
            GP_LOG_PRINTF("CAL: tempcal@%d", 0, Q_PRECISION_DECR8(currentTemperature));
            temperatureSum = 0;
        }
    }
    else
    {
        // temperature reading did not succeed, return
        return;
    }
#else
    currentTemperature = 0;
    lastMeasuredTemperature = 0;
#endif

    // Loop over calibration tasks.
    for (i = 0; i < HalCalibration_NrOfTasks; i++)
    {
        flags = HalCalibration_TaskInfo[i].task.flags;
        recal = false;
        if ((flags & GP_HAL_CALIBRATION_FLAG_TEMPERATURE_SENSITIVE) != 0)
        {
            Q8_8 lastTemperature = HalCalibration_TaskInfo[i].lastCalibrationTemperature;

            UInt16 diffTemperature = ABS(currentTemperature - lastTemperature);
            if (diffTemperature >= HalCalibration_TaskInfo[i].task.temperatureThreshold)
            {
                // Max temperature shift between calibrations reached.
                GP_LOG_PRINTF("diff temp %d", 0, diffTemperature);
                recal = true;
            }
        }

        if (recal)
        {
            // Setup for next calibration.
            HalCalibration_TaskInfo[i].lastCalibrationTemperature = currentTemperature;
            HalCalibration_TaskInfo[i].task.temperature  = currentTemperature;
            // Calibrate.
            GP_LOG_PRINTF("Calibrate %u", 0, i);
            HalCalibration_TaskInfo[i].cbHandler(&HalCalibration_TaskInfo[i].task);
        }
    }
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
void gpHal_SetCalibrationPendingOnWakeup(UInt8 calTaskId)
{
    const UInt8 nofBitsInByte = 8;
    GP_ASSERT_DEV_INT(calTaskId < (sizeof(HalCalibration_PendingOnWakeup) * nofBitsInByte));
    HalCalibration_PendingOnWakeup |= (1 << calTaskId);
}

void gpHal_ClearCalibrationPendingOnWakeup(UInt8 calTaskId)
{
    const UInt8 nofBitsInByte = 8;
    GP_ASSERT_DEV_INT(calTaskId < (sizeof(HalCalibration_PendingOnWakeup) * nofBitsInByte));
    HalCalibration_PendingOnWakeup &= ~(1 << calTaskId);
}

void gpHal_CalibrationSetFirstAfterWakeup(Bool enable)
{
    HalCalibration_FirstAfterWakeup = enable;
}

Bool gpHal_CalibrationGetFirstAfterWakeup()
{
    return HalCalibration_FirstAfterWakeup;
}

void gpHal_InitCalibration(void)
{
    HalCalibration_NrOfTasks = 0;
    HalCalibration_PendingOnWakeup = 0;
    gpHal_CalibrationSetFirstAfterWakeup(false);


    temperatureMeasurementCounter = 0;
    temperatureSum = 0;
}

UInt8 gpHal_CalibrationCreateTask(
        const gpHal_CalibrationTask_t* pTask,
        gpHal_cbCalibrationHandler_t cbHandler)
{
    static Bool isTimersInitialized = false;
    UInt32 currentTime;
    UInt8 taskId;

    GP_LOG_PRINTF("CalibrationCreateTask: %d", 0, HalCalibration_NrOfTasks);

    GP_ASSERT_DEV_INT(pTask != NULL);
    GP_ASSERT_DEV_INT(cbHandler != NULL);
    GP_ASSERT_DEV_INT(((pTask->flags & GP_HAL_CALIBRATION_FLAG_PERIODIC) == 0) || (pTask->calibrationPeriod <= GP_SCHED_EVENT_TIME_MAX));

    if (HalCalibration_NrOfTasks >= GP_HAL_CALIBRATION_MAX_TASKS)
    {
        return GP_HAL_CALIBRATION_INVALID_TASK_HANDLE;
    }

    HAL_TIMER_GET_CURRENT_TIME_1US(currentTime);

    if (HalCalibration_NrOfTasks == 0)
    {
#ifdef GP_COMP_HALCORTEXM4
        lastMeasuredTemperature = halADC_MeasureTemperature();
#else
        lastMeasuredTemperature = 0;
#endif //GP_COMP_HALCORTEXM4
        HalCalibration_NextCheckTime = currentTime + GP_HAL_CALIBRATION_CHECK_INTERVAL_US;
    }
    // Add task to the list
    taskId = HalCalibration_NrOfTasks;
    HalCalibration_TaskInfo[taskId].task = *pTask;
    HalCalibration_TaskInfo[taskId].cbHandler = cbHandler;
    HalCalibration_TaskInfo[taskId].nextCalibrationTime = currentTime + pTask->calibrationPeriod;
    HalCalibration_TaskInfo[taskId].lastCalibrationTemperature = lastMeasuredTemperature;
    HalCalibration_NrOfTasks++;

    if (isTimersInitialized == false)
    {
        isTimersInitialized = true;
#ifdef GP_COMP_HALCORTEXM4
        // Initialize timer
#ifndef GP_DIVERSITY_FREERTOS
        // With FCLK gating, during ARM sleep (WFI) clock is disabled,
        // and Systick will not count. Disabling FCLK gating
        // since we want Systick to wake up ARM during ARM sleep (WFI)
        GP_WB_WRITE_CORTEXM4_DISABLE_FCLK_GATING(1);
        hal_EnableSysTick(0x4E200); //10ms at 32MHz and 5ms at 64 MHz
#else
        // Configure Timer to interrupt at the requested rate.
        gpHal_InitCalibrationTimer(HAL_CALIBRATION_TIMER);
#endif //GP_DIVERSITY_FREERTOS
#endif //GP_COMP_HALCORTEXM4
    }

    return taskId;
}

void gpHal_CalibrationHandleTasks(void)
{
    UInt32 currentTime;
    if (HalCalibration_NrOfTasks == 0)
    {
        // Nothing to do.
        return;
    }

    /* perform any time based calibration
     *  - if time exceeded next calibration check interval OR
     *  - First Calibration routine call after chip wakeup
     */

    HAL_TIMER_GET_CURRENT_TIME_1US(currentTime);
    if (!GP_SCHED_TIME_COMPARE_LOWER_US(currentTime, HalCalibration_NextCheckTime)
            || gpHal_CalibrationGetFirstAfterWakeup())
    {
        // Set time for next check.
        HalCalibration_NextCheckTime = currentTime + GP_HAL_CALIBRATION_CHECK_INTERVAL_US;
        Hal_TimeBasedCalibration();
    }

    /* perform temperature based calibrations */
    HAL_TIMER_GET_CURRENT_TIME_1US(currentTime);
    if (!GP_SCHED_TIME_COMPARE_LOWER_US(currentTime, HalCalibrationTempBased_NextCheckTime))
    {
        // Set time for next check.
        HalCalibrationTempBased_NextCheckTime = currentTime + GP_HAL_CALIBRATION_TEMPERATURE_BASED_CHECK_INTERVAL_US;
        Hal_TemperatureBasedCalibration();
    }
}
