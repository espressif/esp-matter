/**
 * @file
 * See header file for description!
 *
 * @copyright 2020 Silicon Laboratories Inc.
 */

#include <SensorPIR_UserTask_DataAcquisition.h>
#include <zaf_event_helper.h>
#include "events.h"
#include <SizeOf.h>
#include <zpal_power_manager.h>

//#define DEBUGPRINT
#include <DebugPrint.h>

/****************************************************************
 * CONFIGURATIONS OF THIS MODULE
 ***************************************************************/

/****************************************************************
 * DEFINITIONS
 ***************************************************************/

/****************************************************************
 * TYPEDEF and CONSTANTS
 ***************************************************************/

/****************************************************************
 * MACROS
 ***************************************************************/

#define USER_TASK_WAKEUP_PERIOD   30000

/****************************************************************
 * FORWARD DECLARATIONS (none preferred)
 ***************************************************************/

/****************************************************************
 * STATIC VARIABLES
 ***************************************************************/
static zpal_pm_handle_t task_power_lock;
/****************************************************************
 * EXTERNAL VARIABLES (none preferred)
 ***************************************************************/

/****************************************************************
 * STATIC FUNCTIONS
 ***************************************************************/

/**
 * This function does the main operation of this task, which is defined by the User.
 */
NO_RETURN static void executeThread(void)
{
  for (;;)
  {
    zpal_pm_stay_awake(task_power_lock, 0);

    ////////////////////////////////////
    //Do something user specific
    ////////////////////////////////////

    ZAF_EventHelperEventEnqueue(EVENT_APP_USERTASK_DATA_ACQUISITION_FINISHED);  // An event to be send to the main app.
    zpal_pm_cancel(task_power_lock);

    vTaskDelay(pdMS_TO_TICKS(USER_TASK_WAKEUP_PERIOD));
  }
}

/****************************************************************
 * API FUNCTIONS
 ***************************************************************/

/****************************************************************
 * THREAD FUNCTION
 ***************************************************************/

/**
 * A pointer to this function is passed to ZW_UserTask_CreateTask().
 *
 * ATTENTION: This task context shall never call a ZAF API function!
 *
 * The pointer passed to this function is user defined (void-pointer),
 * but here casted to SApplicationHandles-pointer.
 */
NO_RETURN void
SensorPIR_DataAcquisitionTask(void* pUserTaskParam)
{
  const SApplicationHandles* pAppHandles = (SApplicationHandles*)pUserTaskParam;
  UNUSED(pAppHandles);  // Suppose to be NULL anyway in this example app

  DPRINT("\r\nSensorPIR Data Acquisition UserTask started!");

  // Wait for the Main App to initialize all needed modules
  while (ZAF_EventHelperIsInitialized() != true)
  {
    vTaskDelay(pdMS_TO_TICKS(15));  // Value tuned to to the needed wait time!
  }

  task_power_lock = zpal_pm_register(ZPAL_PM_TYPE_DEEP_SLEEP);

  // Generate event that says the Data acquisition UserTask has started!
  if (ZAF_EventHelperEventEnqueue(EVENT_APP_USERTASK_DATA_ACQUISITION_READY))
  {
    DPRINT("\r\nDataAcquisitionTask: Ready event is send to main app!\r\n");
  }

  executeThread();
}
