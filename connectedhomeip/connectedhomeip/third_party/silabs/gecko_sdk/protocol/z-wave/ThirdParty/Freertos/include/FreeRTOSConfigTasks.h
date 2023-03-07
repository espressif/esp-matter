/**
 * @file
 * This file globally defines all task related configuration options.
 * The aim is to create an overview of existing task governing parameters.
 *
 * @copyright 2020 Silicon Laboratories Inc.
 */

#ifndef _FREERTOS_CONFIG_TASK_H_
#define _FREERTOS_CONFIG_TASK_H_

/****************************************************************************
 * INCLUDE FILES
 ***************************************************************************/

/****************************************************************************
 * CONFIGURATIONS
 ***************************************************************************/

/************************************************************************
 * FREERTOS THREAD/TASK CONFIGURATIONS
 ***********************************************************************/

// The maximum priority that can be set! (Not the number of priority levels in existence)
#define TASK_PRIORITY_MAX                       ( 55 )
#define TASK_STACK_SIZE_MINIMUM                 ( ( unsigned short ) 160 )  // The minimum allowed stack size. Do not reduce!

/******************************************
 * PLATFORM SPECIFIC TASKS
 *****************************************/

// TASK NAMES (MAX 8 characters. See FreeRTOSConfig.h)
#define TASK_NAME_Z_WAVE_STACK                  "Z-Wave "  ///< The name of the stack-task!
#define TASK_NAME_FREERTOS_TIMER                "TIMER  "  ///< The name of the FreeRTOS Timer-task!

// TASK PRIORITIES
#define TASK_PRIORITY_FREERTOS_TIMER            ( TASK_PRIORITY_MAX - 0 )  ///< Highest to enable in-time callback invocation.
#define TASK_PRIORITY_Z_WAVE_STACK              ( TASK_PRIORITY_MAX - 10 )  ///< High, due to time critical protocol activity.

// TASK STACK SIZES
/*
 * The bellow stack size for protocol necessary for initializing security,
 * The mainloop(unsigned int work[64], const unsigned char e[32]) in smul.c consume about 3256 bytes of stack
 * when this implementation changes then we can reduce the protocol stack usage further
 */

#ifdef ZWAVE_PSA_SECURE_VAULT
#define TASK_STACK_SIZE_Z_WAVE_STACK            ( 5632 / sizeof(StackType_t) )  // 5632 bytes (security library requires this)
#else
#define TASK_STACK_SIZE_Z_WAVE_STACK            ( 4608 / sizeof(StackType_t) )  // 4608 bytes (security library requires this)
#endif
#define TASK_STACK_SIZE_FREERTOS_TIMER          TASK_STACK_SIZE_MINIMUM

/******************************************
 * USER-TASK CONFIGURATIONS
 *****************************************/

// TASK NAMES (MAX 15 characters. See FreeRTOSConfig.h)
#define TASK_NAME_MAIN_USER_APP                 "APP"  // Suggest changing to "MainApp " if it doesn't break anything?!

// USER-TASK PRIORITIES (These priorities are reserved for ZW_UserTask.h created tasks!)
#define TASK_PRIORITY_HIGHEST                   ( TASK_PRIORITY_MAX - 20 )  ///< High priority UserTask (Good for interrupt handling)
#define TASK_PRIORITY_NORMAL                    ( TASK_PRIORITY_MAX - 30 )  ///< Good for applications. (Used by the main User-Task!)
#define TASK_PRIORITY_BACKGROUND                ( TASK_PRIORITY_MAX - 40 )  ///< Must be 1 priority higher than idle task!
/* FreeRTOS IDLE task has priority 0 (It gets complicated if the IDLE tasks shares its priority with another task.) */

// USER-TASK STACK SIZES
#define TASK_STACK_SIZE_MAIN_USER_APP           2048  // 2048 bytes (The main user-task!)
#define TASK_STACK_SIZE_MAX                     2048  // 2048 bytes (Tasks created by ZW_UserTask.h)
#define TASK_STACK_SIZE_MIN                     TASK_STACK_SIZE_MINIMUM         //  512 bytes (Tasks created by ZW_UserTask.h)

/****************************************************************************
 * DEFINITIONS, TYPEDEF and CONSTANTS
 ***************************************************************************/

/****************************************************************************
 * ENUMERATIONS
 ***************************************************************************/

/****************************************************************************
 * API FUNCTIONS
 ***************************************************************************/

#endif // _FREERTOS_CONFIG_TASK_H_
