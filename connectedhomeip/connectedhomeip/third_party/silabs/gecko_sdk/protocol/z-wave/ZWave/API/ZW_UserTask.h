/**
 * @file
 * A helper API for Task creation for the user domain and also a wrapper module
 * for the task creation functions of FreeRTOS.
 *
 * This module is used to control and limit the use of task creation by the user
 * so to accommodate easy backward compatibility in the future.
 *
 * This module is limited to when the RTOS Scheduler is not running!
 *
 * @copyright 2020 Silicon Laboratories Inc.
 */

#ifndef _USER_TASK_API_H_
#define _USER_TASK_API_H_

/****************************************************************************
 * INCLUDE FILES
 ***************************************************************************/

#include <ZW_global_definitions.h>
#include <ZW_typedefs.h>
#include <ZW_application_transport_interface.h>

/****************************************************************************
 * CONFIGURATIONS*
 ***************************************************************************/

/****************************************************************************
 * DEFINITIONS, TYPEDEF and CONSTANTS
 ***************************************************************************/

/*
 * @brief A buffer structure to allocate the needed memory for a task in user space.
 *
 * The stack size is not fixed, but limited to between TASK_STACK_SIZE_MAX
 * and TASK_STACK_SIZE_MIN. The actual size chosen must be set into
 * stackBufferLength!
 * The buffers must be allocated elsewhere, since this structure does not
 * allocate any space, but holds the pointers to the buffers!
 */
typedef struct {
  StaticTask_t*  taskBuffer;         ///< Memory needed by the Task for internal states. Pointer to buffer!
  uint8_t*       stackBuffer;        ///< Pointer to a statically allocated stack-buffer-array for the task. Pointer to buffer!
  uint16_t       stackBufferLength;  ///< This is measured in bytes and is preferably a multiple of 4, or it will be rounded down!
} ZW_UserTask_Buffer_t;

typedef enum {
  USERTASK_PRIORITY_BACKGROUND = 0, ///< Right above idle priority. Good for HMI and worker threads!
  USERTASK_PRIORITY_NORMAL,         ///< The default 'APP'-task priority!
  USERTASK_PRIORITY_HIGHEST,        ///< High priority UserTask. Good for handling interrupts requests.
} ZW_UserTask_Priority_t;

typedef struct {
  TaskFunction_t         pTaskFunc;      ///< The function of the task! (Invalid parameter if NULL)
  char*                  pTaskName;      ///< The name of the task. (Max characters = configMAX_TASK_NAME_LEN). NULL allowed!
  void*                  pUserTaskParam; ///< Any parameter variable of own choice that is casted as (void*). Can be NULL!
  ZW_UserTask_Priority_t priority;       ///< The desired priority of the task.
  ZW_UserTask_Buffer_t*  taskBuffer;     ///< The buffer collection for the task. @see ZW_UserTask_TaskBuffer_t for more info.
} ZW_UserTask_t;

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
 * STATIC CONTROLLING FUNCTIONS OF DUT
 ***************************************************************************/

/****************************************************************************
 * API FUNCTIONS
 ***************************************************************************/

/**
 * @brief Initializes this module.
 * Used by the low-level implementation, and not by user.
 *
 * Return Returns ReturnCode_t. See ReturnCode_t for more info.
 */
ReturnCode_t ZW_UserTask_Init(void);

/**
 * @brief Used to create user application level tasks.
 *
 * @param[in] task The task definition as input. See ZW_UserTask_t for more info.
 * @param[out] xHandle Used to return the task-handle! (Can be NULL!)
 * Return Returns ReturnCode_t. See ReturnCode_t for more info.
 */
ReturnCode_t ZW_UserTask_CreateTask(ZW_UserTask_t* task, TaskHandle_t* xHandle);

/**
 * @brief Used to create the main application task at highest UserTask priority!
 *
 * This function further registers Queue Notification Bit Numbers so that an association is created between
 * the bit number and the event handle to invoke for that event, when it occurs.
 *
 * This function should only be called ones, or 'false' is returned!
 *
 * @attention This task is the only UserTask that should use the ZAF API functions.
 * Accessing the ZAF API functions from any other ZW_UserTask.h generated tasks
 * will lead to undefined behavior!
 *
 * @param appTaskFunc Application task that will be triggered by FreeRTOS. (function pointer)
 * @param iZwRxQueueTaskNotificationBitNumber The bit number set by Protocol when a message has been put on the ZW Rx queue.
 *        (Receiving EAPPLICATIONEVENT_ZWRX events)
 * @param iZwCommandStatusQueueTaskNotificationBitNumber The bit number Set by Protocol when a message has been put on the ZW command status queue.
 *        (Receiving EAPPLICATIONEVENT_ZWCOMMANDSTATUS events)
 * @param pProtocolConfig Pointer to a statically allocated Protocol Config structure. (@see SProtocolConfig_t)
 * @return Returns true if the main APP task was created successfully! ('false' if not!)
 */
bool ZW_UserTask_ApplicationRegisterTask(  VOID_CALLBACKFUNC(appTaskFunc)(SApplicationHandles*),
                                           uint8_t iZwRxQueueTaskNotificationBitNumber,
                                           uint8_t iZwCommandStatusQueueTaskNotificationBitNumber,
                                           const SProtocolConfig_t * pProtocolConfig);

/****************************************************************************
 * THREAD FUNCTION*
 ***************************************************************************/

#endif // _USER_TASK_API_H_
