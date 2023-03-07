/**
 * @file
 *
 * A break-out example of data acquisition operations for a sensor device to a
 * different UserTask than the Main Application Task.
 *
 * All sensor signals are monitored by this task and passed on to the
 * Main Application Task (MAT) via FreeRTOS IPCs mechanisms.
 *
 * ATTENTION: This module may not call any ZAF API function, except for the
 * zpal_pm_stay_awake() function!
 *
 * @copyright 2020 Silicon Laboratories Inc.
 */

#ifndef _SENSOR_PIR_USER_TASK_DATA_ACQUISITION_H__
#define _SENSOR_PIR_USER_TASK_DATA_ACQUISITION_H__

/****************************************************************************
 * INCLUDE FILES
 ***************************************************************************/
#include <ZW_global_definitions.h>
#include <ZW_UserTask.h>
#include <ev_man.h>

/****************************************************************************
 * CONFIGURATIONS*
 ***************************************************************************/

/****************************************************************************
 * DEFINITIONS, TYPEDEF and CONSTANTS
 ***************************************************************************/

/****************************************************************************
 * MACROS*
 ***************************************************************************/

/****************************************************************************
 * EXTERNAL VARIABLES*
 ***************************************************************************/

/****************************************************************************
 * ENUMERATIONS
 ***************************************************************************/

/****************************************************************************
 * API FUNCTIONS
 ***************************************************************************/

/****************************************************************************
 * THREAD FUNCTION*
 ***************************************************************************/
/**
 * @brief This is the task function. It must never return!
 */
NO_RETURN void SensorPIR_DataAcquisitionTask(void* pAppHandles);

#endif // _SENSOR_PIR_USER_TASK_DATA_ACQUISITION_H__
