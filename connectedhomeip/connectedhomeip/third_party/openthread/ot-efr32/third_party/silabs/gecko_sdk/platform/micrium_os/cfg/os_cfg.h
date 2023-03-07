/***************************************************************************//**
 * @file
 * @brief Kernel Configuration - Configuration Template File
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _OS_CFG_H_
#define  _OS_CFG_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            MISCELLANEOUS
 *
 * Note(s) : (1) Configure OS_CFG_APP_HOOKS_EN to enable or disable Application-specific Hooks.
 *
 *           (2) Configure OS_CFG_DBG_EN to enable or disable debug helper code and variables.
 *
 *           (3) Configure OS_CFG_TICK_EN to enable or disable support for ticks (Delay functions, pend
 *               with timeouts, etc).
 *
 *           (4) Configure OS_CFG_TS_EN to enable or disable Timestamping capabilities.
 *
 *           (5) Configure OS_CFG_PRIO_MAX to set the maximum number of Task Priorities (see OS_PRIO data
 *               type).
 *
 *           (6) Configure OS_CFG_SCHED_LOCK_TIME_MEAS_EN to enable or disable the Scheduler Lock time
 *               measurement code.
 *
 *           (7) Configure OS_CFG_SCHED_ROUND_ROBIN_EN to enable or disable the Round-Robin Scheduler.
 *
 *           (8) Configure OS_CFG_STK_SIZE_MIN to set the minimum allowable Task Stack size (in CPU_STK
 *               elements).
 ********************************************************************************************************
 *******************************************************************************************************/

// <h>Miscellaneous Configuration

// <q OS_CFG_APP_HOOKS_EN> Enable application hooks
// <i> Enable or disable Application-specific Hooks.
// <i> Default: 0
#define  OS_CFG_APP_HOOKS_EN                                0

// <q OS_CFG_DBG_EN> Add debug helper code and variable
// <i> Enable debug helper code and variables.
// <i> Default: 0
#define  OS_CFG_DBG_EN                                      0

// <q OS_CFG_TICK_EN> Enable ticks support
// <i> Enable or disable support for ticks (Delay functions, pend with timeouts, etc).
// <i> Default: 1
#define  OS_CFG_TICK_EN                                     1

// <q OS_CFG_TS_EN> Add timestamping capabilities
// <i> Default: 0
#define  OS_CFG_TS_EN                                       0

// <o OS_CFG_PRIO_MAX> Maximum number of task priorities
// <i> Default: 64
#define  OS_CFG_PRIO_MAX                                    64u

// <q OS_CFG_SCHED_LOCK_TIME_MEAS_EN> Enable scheduler lock time measurement
// <i> Default: 0
#define  OS_CFG_SCHED_LOCK_TIME_MEAS_EN                     0

// <q OS_CFG_SCHED_ROUND_ROBIN_EN> Enable Round-Robin scheduling
// <i> Default: 0
#define  OS_CFG_SCHED_ROUND_ROBIN_EN                        0

// <o OS_CFG_STK_SIZE_MIN> Minimum allowable task stack size (in CPU_STK elements)
// <i> Default: 64
#define  OS_CFG_STK_SIZE_MIN                                64u

// <o OS_CFG_ERRNO_EN> Add threadsafe errno support
// <i> Default: 0
#define OS_CFG_ERRNO_EN                                     0
// </h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             EVENT FLAGS
 ********************************************************************************************************
 *******************************************************************************************************/

// <h>Event Flags Configuration

// <q OS_CFG_FLAG_EN> Enable event flags
// <i> Enable the event flags synchronization construct.
// <i> Default: 1
#define  OS_CFG_FLAG_EN                                     1

// <q OS_CFG_FLAG_MODE_CLR_EN> Enable Active-low event flags
// <i> Enable the active-low mode of the event flags
// <i> Default: 0
#define  OS_CFG_FLAG_MODE_CLR_EN                            0

// </h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                     MUTUAL EXCLUSION SEMAPHORES
 ********************************************************************************************************
 *******************************************************************************************************/

// <h>Mutual Exclusion Semaphores Configuration

// <q OS_CFG_MUTEX_EN> Enable mutexes
// <i> Enable the Mutual Exclusion (Mutex) synchronization construct.
// <i> Default: 1
#define  OS_CFG_MUTEX_EN                                    1

// </h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            MESSAGE QUEUES
 ********************************************************************************************************
 *******************************************************************************************************/

// <h>Message Queues Configuration

// <q OS_CFG_Q_EN> Enable queues
// <i> Enable the message queue construct.
// <i> Default: 1
#define  OS_CFG_Q_EN                                        1

// </h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                              SEMAPHORES
 ********************************************************************************************************
 *******************************************************************************************************/

// <h>Semaphores Configuration

// <q OS_CFG_SEM_EN> Enable semaphores
// <i> Enable the semaphore synchronization construct.
// <i> Default: 1
#define  OS_CFG_SEM_EN                                      1

// </h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MONITORS
 ********************************************************************************************************
 *******************************************************************************************************/

// <h>Monitors Configuration

// <q OS_CFG_MON_EN> Enable monitors
// <i> Enable the monitor (condition variable) synchronization construct.
// <i> Default: 1
#define  OS_CFG_MON_EN                                      1

// </h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           TASK MANAGEMENT
 *
 * Note(s) : (1) Configure OS_CFG_STAT_TASK_EN to enable or disable the Statistics gathering Task.
 *
 *           (2) Configure OS_CFG_STAT_TASK_STK_CHK_EN to enable or disable the stack overflow detection
 *               of the Statistics Task.
 *
 *           (3) Configure OS_CFG_TASK_PROFILE_EN to enable or disable Task profiling instrumentation.
 *
 *           (4) Configure OS_CFG_TASK_Q_EN to enable or disable built-in Task Message Queues.
 *
 *           (5) Configure OS_CFG_TASK_REG_TBL_SIZE to set the number of Task Registers.
 *
 *           (6) Configure OS_CFG_TASK_STK_REDZONE_EN to enable or disable the Redzone Stack Protection.
 *
 *           (7) Configure OS_CFG_TASK_STK_REDZONE_DEPTH to set the depth of the Redzone Stack
 *               Protection.
 ********************************************************************************************************
 *******************************************************************************************************/

// <h>Task Management Configuration

// <q OS_CFG_STAT_TASK_EN> Enable statistics gathering task
// <i> Default: 0
#define  OS_CFG_STAT_TASK_EN                                0

// <q OS_CFG_STAT_TASK_STK_CHK_EN> Enable stack overflow detection of the statistics task
// <i> Default: 1
#define  OS_CFG_STAT_TASK_STK_CHK_EN                        1

// <q OS_CFG_TASK_PROFILE_EN> Enable task profiling instrumentation
// <i> Default: 0
#define  OS_CFG_TASK_PROFILE_EN                             0

// <q OS_CFG_TASK_Q_EN> Enable task message queues
// <i> Default: 0
#define  OS_CFG_TASK_Q_EN                                   0

// <o OS_CFG_TASK_REG_TBL_SIZE> Number of task registers
// <i> Default: 3
#define  OS_CFG_TASK_REG_TBL_SIZE                           3

// <q OS_CFG_TASK_STK_REDZONE_EN> Enable redzone stack protection
// <i> Default: 0
#define  OS_CFG_TASK_STK_REDZONE_EN                         0

// <o OS_CFG_TASK_STK_REDZONE_DEPTH> Depth of the redzone stack protection
// <i> Default: 8
#define  OS_CFG_TASK_STK_REDZONE_DEPTH                      8

// </h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                    TASK LOCAL STORAGE MANAGEMENT
 ********************************************************************************************************
 *******************************************************************************************************/

// <h>Task Local Storage Management Configuration

// <o OS_CFG_TLS_TBL_SIZE> Number of task local storage registers
// <i> Default: 0
#define  OS_CFG_TLS_TBL_SIZE                                0

// </h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           TIMER MANAGEMENT
 ********************************************************************************************************
 *******************************************************************************************************/

// <h>Timer Management Configuration

// <q OS_CFG_TMR_EN> Enable software timers
// <i> Default: 0
#define  OS_CFG_TMR_EN                                      0

// </h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                        CMSIS TIMER MANAGEMENT
 ********************************************************************************************************
 *******************************************************************************************************/

// <h>CMSIS RTOS2 Timer Management Configuration

// <q CMSIS_RTOS2_TIMER_TASK_EN> Enable CMSIS RTOS2 software timers
// <i> Default: 0
#define  CMSIS_RTOS2_TIMER_TASK_EN                          0

// <o CMSIS_RTOS2_TIMER_TASK_STACK_SIZE> Timer task stack size
// <i> Default: 128
#define  CMSIS_RTOS2_TIMER_TASK_STACK_SIZE                  128

// <o CMSIS_RTOS2_TIMER_TASK_PRIO> Timer task priority
// <i> Default: 4
#define  CMSIS_RTOS2_TIMER_TASK_PRIO                        4

// <o CMSIS_RTOS2_TIMER_TASK_QUEUE_SIZE> Timer queue size
// <i> Default: 5
#define  CMSIS_RTOS2_TIMER_TASK_QUEUE_SIZE                  5
// </h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of os_cfg.h module include.

// <<< end of configuration section >>>
