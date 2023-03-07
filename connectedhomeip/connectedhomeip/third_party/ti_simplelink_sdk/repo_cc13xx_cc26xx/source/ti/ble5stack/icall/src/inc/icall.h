/******************************************************************************

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2013-2022, Texas Instruments Incorporated
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

/**
 *  @defgroup ICall ICall
 *  @brief This module implements the Indirect Call Layer
 *  @{
 *  @file  icall.h
 *  @brief      ICall layer interface
 */

#ifndef ICALL_H
#define ICALL_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_assert.h"

 /**
 *  @defgroup ICall_Constants ICall Constants
 *  @{
 */

/**
 * @brief Bit mask for recommended service id enumeration.
 *
 * These bits can be used to identify a service class.
 */
#define ICALL_SERVICE_CLASS_MASK        0xFFF8

/**
 * @brief Bit mask for recommended service id enumeration.
 *
 * These bits can be used to identify an instance
 * among stack instances of the same service class.
 */
#define ICALL_SERVICE_INSTANCE_MASK     0x0007

/**
 * @brief Service class enumerated value for primitive service.
 *
 * Primitive service includes heap service
 * and messaging service.
 */
#define ICALL_SERVICE_CLASS_PRIMITIVE   0x0008

/* Radio stack services */

/** @brief BLE service class enumerated value */
#define ICALL_SERVICE_CLASS_BLE         0x0010

/** @brief TIMAC service class enumerated value */
#define ICALL_SERVICE_CLASS_TIMAC       0x0018

/** @brief RemoTI RTI interface service class enumerated value*/
#define ICALL_SERVICE_CLASS_REMOTI_RTI  0x0020

/** @brief RemoTI network layer service class enumerated value */
#define ICALL_SERVICE_CLASS_REMOTI_RCN  0x0028

/** @brief ZStack service class enumerated value */
#define ICALL_SERVICE_CLASS_ZSTACK      0x0030

/** @brief NPI service class enumerated value */
#define ICALL_SERVICE_CLASS_NPI         0x0038

/** @brief BLE stack message delivery service */
#define ICALL_SERVICE_CLASS_BLE_MSG     0x0050

/* Services that are visible only for other services
 * but not for applications */

/**
 * @brief Crypto service class.
 *
 * Crypto service is intended for stack software to use.
 */
#define ICALL_SERVICE_CLASS_CRYPTO      0x0080

/**
 * @brief BLE Board dependent service class.
 *
 * This service is intended for BLE stack software's use only.
 */
#define ICALL_SERVICE_CLASS_BLE_BOARD   0x0088

/**
 * @brief TIMAC Board dependent service class.
 *
 * This service is intended for TIMAC stack software's use only.
 */
#define ICALL_SERVICE_CLASS_TIMAC_BOARD 0x0090

/**
 * @brief Radio service class.
 *
 * Radio service is intended for stack software to use.
 */
#define ICALL_SERVICE_CLASS_RADIO       0x0098

/**
 * Dummy stack service class enumeration.
 * Dummy stack service is used as an example stack port
 * on top of ICall module.
 */
#define ICALL_SERVICE_CLASS_DUMMY       0x0100

/**
 * @brief Dummy stack board service class enumeration.
 *
 * Dummy stack board service is used as an example
 * board service for a stack.
 */
#define ICALL_SERVICE_CLASS_DUMMY_BOARD 0x0200

/**
 * @brief Entity ID used to indicate invalid entity
 */
#define ICALL_INVALID_ENTITY_ID            0xffu

/**
 * @brief Timer ID used to indicate invalid ID.
 */
#define ICALL_INVALID_TIMER_ID             NULL

/// @cond NODOC
/**
 * @internal
 * Initial value of dest_id field value of @ref ICall_MsgHdr.
 * Note that this value should not be used by regular ICall applications.
 * The value and the message header are meant to be used
 * only by a tightly coupled module.
 */
#define ICALL_UNDEF_DEST_ID                0xffu
/// @endcond // NODOC

/** @brief ICall function return value for successful operation */
#define ICALL_ERRNO_SUCCESS                0

/** @brief ICall function return value for timeout error */
#define ICALL_ERRNO_TIMEOUT                1

/** @brief ICall function return value for no message error */
#define ICALL_ERRNO_NOMSG                  2

/**
 * @brief ICall function return value in case the service
 * corresponding to a passed service id is not registered
 * and hence is unknown.
 */
#define ICALL_ERRNO_INVALID_SERVICE        -1

/**
 * @brief ICall function return value in case the function id
 * is unknown to the registered handler of the service.
 */
#define ICALL_ERRNO_INVALID_FUNCTION       -2

/** @brief ICall function return value for invalid parameter value */
#define ICALL_ERRNO_INVALID_PARAMETER      -3

/** @brief ICall function return value for no resource error */
#define ICALL_ERRNO_NO_RESOURCE            -4

/**
 * @brief ICall function return value in case the calling
 * thread context is not a registered thread,
 * or the entity id passed is not a registered entity.
 */
#define ICALL_ERRNO_UNKNOWN_THREAD         -5

/** @brief ICall function return value for corrupt message error */
#define ICALL_ERRNO_CORRUPT_MSG            -6

/** @brief ICall function return value for counter overflow */
#define ICALL_ERRNO_OVERFLOW               -7

/** @brief ICall function return value for counter underflow */
#define ICALL_ERRNO_UNDERFLOW              -8

/** Message format that requires no change */
#define ICALL_MSG_FORMAT_KEEP              0

/**
 *  @brief  Message format that requires the first byte
 * to be replaced with the src task ID
 */
#define ICALL_MSG_FORMAT_1ST_CHAR_TASK_ID  1

/**
 * @brief Message format that requires the third byte
 * to be replaced with the src task ID
 */
#define ICALL_MSG_FORMAT_3RD_CHAR_TASK_ID  2

#ifdef ICALL_LITE
/**
 * @brief Message format indicated a direct API call
 */
#define ICALL_MSG_FORMAT_DIRECT_API_ID     3

/**
 * @brief Message CMD ID to signal DIRECT_API CMD is done
 */
#define ICALL_LITE_DIRECT_API_DONE_CMD_ID  0x42

#endif   /* ICALL_LITE */

/**
 * @brief Time period that indicates infinite time
 */
#define ICALL_TIMEOUT_FOREVER              0xfffffffful

/**
 * @brief Time period that indicates timeout define at build time
 * default is 5s
 */
#ifndef ICALL_TIMEOUT_PREDEFINE
#define ICALL_TIMEOUT_PREDEFINE            5000
#endif   /* ICALL_TIMEOUT_PREDEFINE */

/**
 * @brief Counting semaphore mode
 */
#define ICALL_SEMAPHORE_MODE_COUNTING      0

/**
 * @brief Binary semaphore mode
 */
#define ICALL_SEMAPHORE_MODE_BINARY        1

/* @brief Primitive service function enumeration */

/// @cond NODOC
/** @internal Primitive service enroll function id */
#define ICALL_PRIMITIVE_FUNC_ENROLL                       0

/** @internal Primitive service register application function id */
#define ICALL_PRIMITIVE_FUNC_REGISTER_APP                 1

/** @internal Primitive service message allocation function id */
#define ICALL_PRIMITIVE_FUNC_MSG_ALLOC                    2

/** @internal Primitive service message de-allocation function id */
#define ICALL_PRIMITIVE_FUNC_MSG_FREE                     3

/** @internal Primitive service memory block allocation function id */
#define ICALL_PRIMITIVE_FUNC_MALLOC                       4

/** @internal Primitive service memory block de-allocation function id */
#define ICALL_PRIMITIVE_FUNC_FREE                         5

/** @internal Primitive service message send function id */
#define ICALL_PRIMITIVE_FUNC_SEND_MSG                     6

/** @internal Primitive service message retrieval function id */
#define ICALL_PRIMITIVE_FUNC_FETCH_MSG                    7

/** @internal Primitive service "send to service id" function id */
#define ICALL_PRIMITIVE_FUNC_SEND_SERV_MSG                8

/** @internal Primitive service "retrieve service message" function id */
#define ICALL_PRIMITIVE_FUNC_FETCH_SERV_MSG               9

/** @internal Primitive service "wait" function id */
#define ICALL_PRIMITIVE_FUNC_WAIT                         10

/** @internal Primitive service "semaphore signal" function id */
#define ICALL_PRIMITIVE_FUNC_SIGNAL                       11

/** @internal Primitive service "abort" function id */
#define ICALL_PRIMITIVE_FUNC_ABORT                        12

/** @internal Primitive service "enable single interrupt" function id */
#define ICALL_PRIMITIVE_FUNC_ENABLE_INT                   13

/** @internal Primitive service "disable single interrupt" function id */
#define ICALL_PRIMITIVE_FUNC_DISABLE_INT                  14

/** @internal Primitive service "enable master interrupt" function id */
#define ICALL_PRIMITIVE_FUNC_ENABLE_MINT                  15

/** @internal Primitive service "disable master interrupt" function id */
#define ICALL_PRIMITIVE_FUNC_DISABLE_MINT                 16

/** @internal Primitive service "register ISR" function id */
#define ICALL_PRIMITIVE_FUNC_REGISTER_ISR                 17

/** @internal Primitive service "get ticks" function id */
#define ICALL_PRIMITIVE_FUNC_GET_TICKS                    18

/** @internal Primitive service "schedule wakeup" function id */
#define ICALL_PRIMITIVE_FUNC_SET_TIMER_MSECS              19

/** @internal Primitive service "get tick period" function id */
#define ICALL_PRIMITIVE_FUNC_GET_TICK_PERIOD              20

/** @internal Primitive service "get maximum period" function id */
#define ICALL_PRIMITIVE_FUNC_GET_MAX_MILLISECONDS         21

/** @internal Primitive service "entity id to service id" function id */
#define ICALL_PRIMITIVE_FUNC_ENTITY2SERVICE               22

/** @internal Primitive service "pwr update activity counter" function id */
#define ICALL_PRIMITIVE_FUNC_PWR_UPD_ACTIVITY_COUNTER     23

/** @internal Primitive service "pwr register notify" function id */
#define ICALL_PRIMITIVE_FUNC_PWR_REGISTER_NOTIFY          24

/** @internal Primitive service "wait match" function id */
#define ICALL_PRIMITIVE_FUNC_WAIT_MATCH                   25

/** @internal Primitive service "get entity id" function id */
#define ICALL_PRIMITIVE_FUNC_GET_ENTITY_ID                26

/** @internal Primitive service "set timer" function id */
#define ICALL_PRIMITIVE_FUNC_SET_TIMER                    27

/** @internal Primitive service "stop timer" function id */
#define ICALL_PRIMITIVE_FUNC_STOP_TIMER                   28

/**
 * @internal Primitive service "pwr config activity counter action"
 * function id
 */
#define ICALL_PRIMITIVE_FUNC_PWR_CONFIG_AC_ACTION         29

/** @internal Primitive service "pwr require" function id */
#define ICALL_PRIMITIVE_FUNC_PWR_REQUIRE                  30

/** @internal Primitive service "pwr dispense" function id */
#define ICALL_PRIMITIVE_FUNC_PWR_DISPENSE                 31

/** @internal Primitive service "thread serves" function id */
#define ICALL_PRIMITIVE_FUNC_THREAD_SERVES                32

/** @internal Primitive service "pwr is stable xosc hf" function id */
#define ICALL_PRIMITIVE_FUNC_PWR_IS_STABLE_XOSC_HF        33

/** @internal Primitive service "pwr get transition state" function id */
#define ICALL_PRIMITIVE_FUNC_PWR_GET_TRANSITION_STATE     34

/** @internal Primitive service "create task" function id */
#define ICALL_PRIMITIVE_FUNC_CREATE_TASK                  35

/** @internal Primitive service "create semaphore" function id */
#define ICALL_PRIMITIVE_FUNC_CREATE_SEMAPHORE             36

/** @internal Primitive service "wait semaphore" function id */
#define ICALL_PRIMITIVE_FUNC_WAIT_SEMAPHORE               37

/** @internal Primitive service "switch xosc hf" function id */
#define ICALL_PRIMITIVE_FUNC_SWITCH_XOSC_HF               38

/** @internal Primitive service "get xosc startup time" function id */
#define ICALL_PRIMITIVE_FUNC_PWR_GET_XOSC_STARTUP_TIME    39

/** @internal Primitive service "register ISR extended" function id */
#define ICALL_PRIMITIVE_FUNC_REGISTER_ISR_EXT             40

#ifdef ICALL_EVENTS
/** @internal Primitive service "post semaphore" function id */
#define ICALL_PRIMITIVE_FUNC_POST_SEMAPHORE               41

/** @internal Primitive service "create event" function id */
#define ICALL_PRIMITIVE_FUNC_CREATE_EVENT                 42

/** @internal Primitive service "wait event" function id */
#define ICALL_PRIMITIVE_FUNC_WAIT_EVENT                   43

/** @internal Primitive service "post event" function id */
#define ICALL_PRIMITIVE_FUNC_POST_EVENT                   44
#endif  /* ICALL_EVENTS */
/// @endcond // NODOC

/**
 * @brief Messaging service function id for translating ICall_entityID
 * to locally understandable id.
 */
#define ICALL_MSG_FUNC_GET_LOCAL_MSG_ENTITY_ID            0

#ifdef ICALL_EVENTS
/**
 * Event reserved to signal for icall message in between application and
 * services.
 *
 * Used when ICALL_EVENTS compile option is enabled only.
 */
#define ICALL_MSG_EVENT_ID                                Event_Id_31
/**
 * Event reserved to signal a wait match period has ended and the caller
 * may continue execution.
 *
 * @note the Event_Id_30 is usually reserved for the
 * the application internal queue processing. */
#define ICALL_WAITMATCH_EVENT_ID                          Event_Id_29
#endif

/** @} End ICall_Constants */

 /**
 *  @defgroup ICall_Structures ICall Data Structures
 *  @{
 */
/** @brief Error code data type */
typedef int_fast16_t ICall_Errno;

/** @brief Service enumeration data type */
typedef uint_least16_t ICall_ServiceEnum;

/** @brief Message conversion format data type */
typedef uint_least8_t ICall_MSGFormat;

/** @brief Function id data type */
typedef uint_least16_t ICall_FuncID;

/** @brief Synchronization object data type */
typedef void *ICall_SyncHandle;

/** @brief Synchronization object data type */
typedef void *ICall_TaskHandle;

/** @brief Semaphore used for ICall **/
typedef void *ICall_Semaphore;

#ifdef ICALL_EVENTS
/** @brief Event data type */
typedef void *ICall_Event;
#endif /* ICALL_EVENTS */

/** @brief Entity id data type */
typedef uint_least8_t ICall_EntityID;

/** @brief Timer id data type */
typedef void *ICall_TimerID;

typedef struct
{
  uint32_t totalSize;
  uint32_t totalFreeSize;
  uint32_t largestFreeSize;

}ICall_heapStats_t;

/**
 * @brief  Prototype of a function used to compare a received message for a match
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
typedef bool (*ICall_MsgMatchFn)(ICall_ServiceEnum src, ICall_EntityID dest,
                                 const void *msg);

/**
 * @brief Prototype of a callback function for timer
 * @param arg   argument passed through @ref ICall_setTimer
 */
typedef void (*ICall_TimerCback)(void *arg);

/** @brief Common service function arguments */
typedef struct _icall_func_args_hdr_t
{
  ICall_ServiceEnum service;   //!< service id
  ICall_FuncID func;           //!< function id
} ICall_FuncArgsHdr;

/** @brief @ref ICall_registerApp arguments */
typedef struct _icall_register_app_args_t
{
  ICall_FuncArgsHdr hdr; //!< common arguments
  ICall_EntityID    entity; //!< field to store the entity id assigned to the application
  ICall_SyncHandle   msgSyncHdl; //!<field to store the synchronous object handle associated with the calling thread
} ICall_RegisterAppArgs;

/** @brief @ref ICall_allocMsg  or @ref ICall_malloc  arguments */
typedef struct _icall_alloc_args_t
{
  ICall_FuncArgsHdr hdr; //!< common arguments
  size_t size;           //!< size of the message body in bytes
  void * ptr;            //!< pointer to store the start address of the newly allocated memory block
} ICall_AllocArgs;

/** @brief @ref ICall_freeMsg  or @ref ICall_free  arguments */
typedef struct _icall_free_args_t
{
  ICall_FuncArgsHdr hdr;    //!< common arguments
  void * ptr;               //!< pointer to the address of a memory block to free
} ICall_FreeArgs;

/** @brief @ref ICall_sendServiceMsg  and @ref ICall_send  arguments */
typedef struct _icall_send_args_t
{
  ICall_FuncArgsHdr hdr;    //!< common arguments
  ICall_EntityID src;       //!< Entity id of the sender of the message Destination id
  union
  {
    ICall_EntityID entityId;    //!< Entity ID of destination
    ICall_ServiceEnum servId;   //!< Service ID of destination
  } dest;     //!< union of entity and service ID's
  ICall_MSGFormat format;       //!< Message format
  void *msg;                    //!< pointer to the message body to send
} ICall_SendArgs;

/** @brief @ref ICall_fetchServiceMsg  and @ref ICall_fetchMsg  arguments */
typedef struct _icall_fetch_msg_args_t
{
  ICall_FuncArgsHdr hdr;      //!< common arguments field to store source of the message
  union
  {
    ICall_ServiceEnum servId; //!< service id
    ICall_EntityID entityId;  //!< entity id
  } src;    //!< union of service and entity id's
  ICall_EntityID dest;        //!< field to store the entity id of the destination of the message
  void *msg;                  //!< field to store the starting address of the message body
} ICall_FetchMsgArgs;

/** @brief @ref ICall_wait  arguments */
typedef struct _icall_wait_args_t
{
  ICall_FuncArgsHdr hdr;         //!< common arguments
  uint_least32_t milliseconds;   //!< timeout period in milliseconds
} ICall_WaitArgs;

/** @brief @ref ICall_signal  arguments */
typedef struct _icall_signal_args_t
{
  ICall_FuncArgsHdr hdr;      //!< common arguments
  ICall_SyncHandle syncHandle;  //!< handle of a synchronous object to signal
} ICall_SignalArgs;

/** @brief @ref ICall_signal  arguments */
typedef struct _icall_signal_events_args_t
{
  ICall_FuncArgsHdr hdr;    //!< common arguments
  ICall_SyncHandle syncHandle;  //!< handle of a synchronous object to signal
  uint32_t          events;   //!< events
} ICall_SignalEventsArgs;

/** @brief forward reference */
struct _icall_enroll_service_args_t;

/** @brief @ref ICall_entityId2ServiceId  arguments */
typedef struct _icall_entity2service_args_t
{
  ICall_FuncArgsHdr hdr;          //!< common arguments
  ICall_EntityID entityId;        //!< entity id
  ICall_ServiceEnum servId;       //!< field to store the service id
} ICall_EntityId2ServiceIdArgs;

/** @brief @ref ICall_enableInt  and @ref ICall_disableInt  arguments */
typedef struct _icall_intnum_args_t
{
  ICall_FuncArgsHdr hdr;      //!< common arguments
  int_least32_t intnum;       //!< interrupt number
} ICall_IntNumArgs;

/** @brief @ref ICall_registerISR  arguments */
typedef struct _icall_register_isr_args_t
{
  ICall_FuncArgsHdr hdr;      //!< common arguments
  int_least32_t intnum;       //!< interrupt number
  void (*isrfunc)(void);      //!< pointer to the interrupt service function
} ICall_RegisterISRArgs;

/** @brief ICall_registerISR_Ext  arguments */
typedef struct _icall_register_isr_args_ext_t
{
  ICall_FuncArgsHdr hdr;               //!< common arguments
  int_least32_t     intnum;            //!< interrupt number
  void              (*isrfunc)(void);  //!< pointer to ISR
  int               intPriority;       //!< interrupt priority
} ICall_RegisterISRArgs_Ext;

/**
 * @brief @ref ICall_getTicks , @ref ICall_getTickPeriod  and @ref ICall_getMaxMSecs
 * arguments
 */
typedef struct _icall_getuint32_args_t
{
  ICall_FuncArgsHdr hdr;      //!< common arguments
  uint_least32_t value;       //!< field to store the retrieved value
} ICall_GetUint32Args;

/**
 * @brief @ref ICall_pwrIsStableXOSCHF arguments
 */
typedef struct _icall_getbool_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** field to store the retrieved value */
  bool value;
} ICall_GetBoolArgs;

/** @brief @ref ICall_pwrUpdActivityCounter  arguments */
typedef struct _icall_pwr_upd_activity_counter_args_t
{
  ICall_FuncArgsHdr hdr;      //!< common arguments
  bool incFlag;               //!< a boolean flag to indicate either increment or decrement
  bool pwrRequired;           //!< the result of current activity counter
} ICall_PwrUpdActivityCounterArgs;

/**
 * @brief Power control bitmap type
 */
typedef uint_fast32_t ICall_PwrBitmap_t;

/**
 * @brief ICall_pwrConfigActivityCounterAction ,
 * @ref ICall_pwrRequire  and @ref ICall_pwrDispense  arguments
 */
typedef struct _icall_pwr_bitmap_args_t
{
  ICall_FuncArgsHdr hdr;      //!< common arguments
  ICall_PwrBitmap_t bitmap;   //!< power configuration bitmap
} ICall_PwrBitmapArgs;

/**
 * @brief @ref ICall_pwrGetTransitionState  arguments
 */
typedef struct _icall_pwr_get_transition_state_args_t
{
  ICall_FuncArgsHdr hdr;      //!< common arguments
  uint_fast8_t state;         //!< power transition state
} ICall_PwrGetTransitionStateArgs;

/**
 * @brief @ref ICall_pwrGetXOSCStartupTime  arguments
 */
typedef struct _icall_pwr_get_xosc_startup_time_args_t
{
  ICall_FuncArgsHdr hdr;      //!< common arguments
  uint_least32_t timeUntilWakeupInMs;   //!< field matching timeUntilWakeupInMs argument
  uint_least32_t value;         //!< field to store the retrieved value
} ICall_PwrGetXOSCStartupTimeArgs;

/** @brief @ref ICall_threadServes  argument */
typedef struct _icall_thread_serves_args_t
{
  ICall_FuncArgsHdr hdr;        //!< common arguments
  ICall_ServiceEnum servId;     //!< Service ID of destination
  uint_fast8_t result;          //!< Result (true or false)
} ICall_ThreadServesArgs;

/** @brief @ref ICall_createTask  argument */
typedef struct _icall_create_task_args_t
{
  ICall_FuncArgsHdr hdr;      //!< common arguments
  void (*entryfn)(size_t arg);   //!< task entry function
  uint_least8_t priority;       //!< task priority as understood by underlying RTOS implementation
  uint_least16_t stacksize;     //!< task priority as understood by underlying RTOS implementation
  size_t arg;                   //!< argument passed to the task entry function
} ICall_CreateTaskArgs;

#ifdef ICALL_RTOS_SEMAPHORE_API
/** @brief @ref ICall_createSemaphore  argument */
typedef struct _icall_create_semaphore_args_t
{
  ICall_FuncArgsHdr hdr;      //!< common arguments
  /**
   * @brief semaphore mode.
   * Either @ref ICALL_SEMAPHORE_MODE_BINARY or
   * @ref ICALL_SEMAPHORE_MODE_COUNTING
   */
  uint_least8_t mode;
  int initcount;      //!< semaphore initial count value
  ICall_Semaphore sem;    //!< created semaphore
} ICall_CreateSemaphoreArgs;

/** @brief @ref ICall_postSemaphore  argument */
typedef struct _icall_post_semaphore_args_t
{
  ICall_FuncArgsHdr hdr;    //!< common arguments
  ICall_Semaphore sem;      //!< semaphore
} ICall_PostSemaphoreArgs;

/** @brief @ref ICall_waitSemaphore  argument */
typedef struct _icall_wait_semaphore_args_t
{
  ICall_FuncArgsHdr hdr;    //!< common arguments
  ICall_Semaphore sem;      //!< semaphore
  uint_least32_t milliseconds;  //!< timeout in milliseconds
} ICall_WaitSemaphoreArgs;
#endif /* ICALL_RTOS_SEMAPHORE_API */

#ifdef ICALL_RTOS_EVENTS_API
/** @brief @ref ICall_createEvent  argument */
typedef struct _icall_create_event_args_t
{
  ICall_FuncArgsHdr hdr;    //!< common arguments
  ICall_Event event;        //!< created event
} ICall_CreateEventArgs;

/** @brief @ref ICall_postEvent  argument */
typedef struct _icall_post_event_args_t
{
  ICall_FuncArgsHdr hdr;    //!< common arguments
  ICall_Event event;        //!< event
  uint32_t events;          //!< events
} ICall_PostEventArgs;

/** @brief @ref ICall_waitEvent  argument */
typedef struct _icall_wait_event_args_t
{
  ICall_FuncArgsHdr hdr;      //!< common arguments
  ICall_Event event;          //!< event
  uint_least32_t milliseconds;  //!< timeout in milliseconds
} ICall_WaitEventArgs;
#endif /* ICALL_RTOS_EVENTS_API */

/// @cond NODOC
/**
 * @internal
 * ICall message header.
 * Note that application must not rely on the content of this data structure.
 * This data structure is used only by tightly coupled module.
 */
typedef struct _icall_msg_hdr_t
{
  void    *next;
  uint8_t  srcentity; /* Source entity across threads */
  uint8_t  dstentity; /* Destination entity across threads */
  uint8_t  format;    /* message transformation request */
  uint16_t len;
  uint8_t  dest_id;
} ICall_MsgHdr;
/// @endcond //NODOC


/**
 * @brief Power state transition type of the following values:
 *
 * - @ref ICALL_PWR_AWAKE_FROM_STANDBY
 * - @ref ICALL_PWR_AWAKE_FROM_STANDBY_LATE
 * - @ref ICALL_PWR_ENTER_STANDBY
 * - @ref ICALL_PWR_ENTER_SHUTDOWN
 */
typedef uint_fast8_t ICall_PwrTransition;

/* @brief Forward reference */
struct _icall_pwr_notify_data_t;

/**
 * Power state transition notify function type
 *
 * @param pwrTrans  power transition.<br>
 *                  The values are defined as per platform.
 *                  For example, see ICallCC26xxDefs.h.
 * @param obj       pointer data object to identify the client.
 *                  Note that the pointer must not be NULL.
 */
typedef void (*ICall_PwrNotifyFn)(ICall_PwrTransition pwrTrans,
                                  struct _icall_pwr_notify_data_t *obj);

/**
 * @brief Power state transition notify function data object type
 */
typedef struct _icall_pwr_notify_data_t
{
  /**
   * @brief a field for private use. Do not access the field.
   * User data should follow this field.
   */
  ICall_PwrNotifyFn _private;
} ICall_PwrNotifyData;

/** @brief @ref ICall_pwrRegisterNotify  arguments */
typedef struct _icall_pwr_register_notify_args_t
{
  ICall_FuncArgsHdr hdr;      //!< common arguments
  ICall_PwrNotifyFn fn;       //!< notify function to handle notification
  ICall_PwrNotifyData *obj;   //!< data object
} ICall_PwrRegisterNotifyArgs;

/** @brief @ref ICall_waitMatch  arguments */
typedef struct _icall_wait_match_args_t
{
  ICall_FuncArgsHdr hdr;        //!< common arguments
  uint_least32_t milliseconds;  //!< timeout period in milliseconds
  ICall_MsgMatchFn matchFn;     //!< match function
  ICall_ServiceEnum servId;     //!< service id
  ICall_EntityID dest;          //!< field to store the entity id of the destination of the message
  void *msg;                    //!< field to store the starting address of the message body
} ICall_WaitMatchArgs;

/** @brief @ref ICall_getEntityId  arguments */
typedef struct _icall_get_entity_id_args_t
{
  ICall_FuncArgsHdr hdr;    //!< common arguments
  ICall_EntityID entity;    //!< client entity id
} ICall_GetEntityIdArgs;

/**
 * @brief Call dispatcher function pointer type.
 *
 * @param args    arguments
 * @return error code
 */
typedef ICall_Errno (*ICall_Dispatcher)(ICall_FuncArgsHdr *args);

/**
 * @brief Critical section state data type
 */
typedef uint_least32_t ICall_CSState;

/**
 * @brief Critical section entry function pointer type
 *
 * @return critical section state before entry.
 */
typedef ICall_CSState (*ICall_EnterCS)(void);

/**
 * @brief Critical section exit function pointer type
 *
 * @param critical section state returned from @ref ICall_EnterCS
 *        type function matching to this exit.
 */
typedef void (*ICall_LeaveCS)(ICall_CSState key);

/** @brief Call dispatcher pointer of the current image */
extern ICall_Dispatcher ICall_dispatcher;

/** @brief Enter critical section function pointer of the current image */
extern ICall_EnterCS ICall_enterCriticalSection;

/** @brief Leave critical section function pointer of the current image */
extern ICall_LeaveCS ICall_leaveCriticalSection;

/** @brief Data type of the first argument passed to the entry point
 *  of an image which contains a remote task. */
typedef struct _icall_remote_task_arg_t
{
  ICall_Dispatcher dispatch;      //!< ICall dispatcher
  ICall_EnterCS entercs;          //!< enter CS
  ICall_LeaveCS leavecs;          //!< leave CS
} ICall_RemoteTaskArg;

/**
 * @brief Service entry function prototype
 *
 * @note Multiple services must be enrolled from a single
 * function call per downloadable stack image.
 * All enrolled services shall be running from the same thread.
 *
 * @param arg0   argument containing remote dispatch function pointers
 * @param arg1   custom initialization parameter
 */
typedef void (*ICall_RemoteTaskEntry)(const ICall_RemoteTaskArg *arg0,
                                      void *arg1);

/**
 * @brief Call dispatcher service handler function pointer type
 *
 * @param args    arguments
 * @return error code
 */
typedef ICall_Errno (*ICall_ServiceFunc)(ICall_FuncArgsHdr *args);

/** @brief @ref ICall_enrollService  arguments */
typedef struct _icall_enroll_service_args_t
{
  ICall_FuncArgsHdr hdr;    //!< common arguments
  ICall_ServiceEnum service;  //!< service id of the enrolling service
  ICall_ServiceFunc fn;       //!< handler function which handles function calls to the service
  ICall_EntityID entity;      //!< field to store the assigned entity id
  ICall_SyncHandle msgSyncHdl;  //!< field to store the synchronous object handle associated with the calling thread
} ICall_EnrollServiceArgs;

/** @brief @ref ICall_setTimer  arguments */
typedef struct _icall_set_timer_args_t
{
  ICall_FuncArgsHdr hdr;      //!< common arguments
  uint_least32_t timeout;     //!< timeout period
  ICall_TimerID timerid;      //!< timer ID
  ICall_TimerCback cback;     //!< callback function
  void *arg;                  //!< pointer to an argument to be passed to the callback function
} ICall_SetTimerArgs;

/** @brief @ref ICall_stopTimer  arguments */
typedef struct _icall_stop_timer_args_t
{
  ICall_FuncArgsHdr hdr;    //!< common arguments
  ICall_TimerID timerid;    //!< timer ID
} ICall_StopTimerArgs;

/** @brief @ref ICall_getLocalMsgEntityId  arguments */
typedef struct _icall_get_local_msg_entity_id_args_t
{
  ICall_FuncArgsHdr hdr;        //!< Common arguments
  ICall_ServiceEnum service;     //!< service id
  ICall_EntityID entity;        //!< entity id
  uint_least8_t localId;        //!< local entity ID
} ICall_GetLocalMsgEntityIdArgs;

/** @brief RemoTI RNP and Remote Icall Messaging Interface Enumeration */
typedef enum
{
   ICALL_RTI_APP_CMD_EVENT       = 0xE0,    //!< Event for app RTI->stack RC App
   ICALL_RTI_STACK_CMD_EVENT     = 0xE1,    //!< Event for stack->app RC App
   ICALL_RTI_APP_ASYNC_CMD_EVENT = 0xE2,    //!< Event for app RTI->stack RTI module
   ICALL_RTI_APP_SYNC_CMD_EVENT  = 0xE3,    //!< Event for stack RTI->app RTI module
   ICALL_RCN_APP_ASYNC_CMD_EVENT = 0xE4,    //!< ASYNC message from RNP app->stack
   ICALL_RCN_APP_SYNC_CMD_EVENT  = 0xE5     //!< SYNC message from RNP app -> stack
}ICall_RemotiCmdEvent_t;

#ifdef ICALL_LITE
typedef uint_least32_t icall_lite_id_t;     //!< ICall Lite ID

/// @brief ICall Lite Command Status
typedef struct _ICall_LiteCmdStatus_
{
  uint_least8_t cmdId;        //!< command id (applicable only to User Profile subgrp)
} ICall_LiteCmdStatus;

PACKED_TYPEDEF_STRUCT
{
  uint_least32_t    *pointerStack;    //!< Stack Pointer
  icall_lite_id_t   directAPI;        //!< Direct API
} icall_directAPIMsg_t;               //!< Direct API Message

PACKED_TYPEDEF_STRUCT
{
  ICall_MsgHdr hdr;                 //!< ICall Message Header
  icall_directAPIMsg_t   msg;       //!< Direct API Message
} icallLiteMsg_t;                   //!< ICall Lite Message

#endif /* ICALL_LITE */

typedef struct
{
  ICall_RemoteTaskEntry startupEntry;
  uint_least32_t        imgTaskPriority;
  uint_least32_t        imgTaskStackSize;
  void                  *ICall_imgInitParam;
} ICall_RemoteTask_t;

/**
 * @brief Special global variable to set the reset reason by the boot code,
 * before ICall is initialized, so that ICall can serve "get reset reason"
 * function.
 */
extern uint_least8_t ICall_resetReason;

/** @} End ICall_Structures */

/**
 * @brief Initializes the dispatcher implementation.
 *
 * @note This shall be only called from an image which
 *        included dispatcher implementation.
 */
extern void ICall_init(void);

/**
 * @brief   Create remote tasks.
 *
 * @par     Note
 * One remote task shall be created per external image. <br>
 * This function must be called after calling
 * @ref ICall_init .
 *
 * The external image information must be stored either in a custom
 * @ref icall_addrs.h header file in an include path when
 * ICALL_FEATURE_SEPARATE_IMGINFO compile flag is not defined,
 * or as a set of external constants.
 *
 * If ICALL_FEATURE_SEPARATE_IMGINFO compile flag is not defined,
 * the following macros must be defined in the ICallAddrs.h file:
 * @ref ICALL_STACK0_ADDR, @ref ICALL_ADDR_MAPS, @ref ICALL_TASK_PRIORITIES
 * and @ref ICALL_TASK_STACK_SIZES.
 *
 * If ICALL_FEATURE_SEPARATE_IMGINFO compile flag is defined,
 * the following constants have to be linked into an image that include ICall
 * module: ICall_imgEntries, ICall_imgTaskPriorities,
 * ICall_imgTaskStackSizes and ICall_numImages.
 */
extern void ICall_createRemoteTasks(void);

/**
 * @brief   Create remote tasks with parameters define at runtime.
 *
 * @par     Note
 * One remote task shall be created per external image. <br>
 * This function must be called after calling
 * @ref ICall_init.
 *
 * The external image information is pass as a parameter
 * @param       remoteTaskTable: table containing the information of all remote task to create
 * @param       nb_elems: number of elements in the table.
 *
 */
extern void ICall_createRemoteTasksAtRuntime(ICall_RemoteTask_t *remoteTaskTable, uint8_t nb_elems);

/**
 * @brief   return the task handle of a task.
 *
 * @param index  index of the remote task
 *               in the table of task. max possible
 *               number of task is ICALL_MAX_NUM_TASKS.
 *               value 0 is for the first create remote
 *               task, (done generaly in main() through
 *               ICall_createRemoteTasks).
 *               Then index is incremeted by 1 for each
 *               new remote task create, or service
 *               register, or application register.
 *               value 1 for the second, etc...
 * @return task_Handle is the handle of the remote task.
 * @return NULL is corresponding index does not contain a task.
 */
extern ICall_TaskHandle ICall_getRemoteTaskHandle(uint8 index);

/**
 * @brief   Search for a service entity entry.
 *
 * @param service  service id
 * @return entity id of the service
 * @return @ref ICALL_INVALID_ENTITY_ID when none found.
 */
extern ICall_EntityID ICall_searchServiceEntity(ICall_ServiceEnum service);

#ifdef ICALL_LITE
 /**
 * @brief       generic variadic function to translate API id to a functional
 *              call in the stack context,
 *
 *              Maximum number of supported parameters is define in the stack
 *              build, can be 4, 8 or 12 parameters (uint32_t).
 *
 * @param       service: service the API belongs to
 * @param       id: Id of the stack API to call.
 * @param       ... : all parameters corresponding to the stack API to call.
 *
 * @return      register r0 will be populated with any return value fill by the Stack API.
 */
uint32_t icall_directAPI( uint8_t service, icall_lite_id_t id, ... );

 /**
 * @brief       function to indicate that servicing an direct  API call is done
 *
 * @param       src:     entity id of the sender
 * @param       dest:    entity id of the destination of the message.
 * @param       format:  message format. See ICall_sendServiceMsg().
 * @param       msg:     pointer to the message body.
 *
 * @return @ref ICALL_ERRNO_SUCCESS when successful.<br>
 *         @ref ICALL_ERRNO_INVALID_PARAMETER when either src
 *              or dest is not a valid entity id or when
 *              dest is an entity id of an entity that does
 *              not receive a message
 *              (e.g., ICall primitive service entity).
 */
ICall_Errno ICall_sendServiceComplete(ICall_EntityID src,
                                      ICall_EntityID dest,
                                      ICall_MSGFormat format,
                                      void *msg);
#endif /* ICALL_LITE */

#ifdef ICALL_JT
/**
 * @brief       Registers an application.
 *
 * @par         Note
 *              This function must be called from the thread
 *              from which @ref ICall_wait  function will be called.
 *
 * @param entity  pointer to a variable to store entity id assigned
 *                to the application.
 * @param msgSyncHdl  pointer to a variable to store the synchronous object handle
 *                associated with the calling thread.
 * @return @ref ICALL_ERRNO_SUCCESS when successful.
 * @return @ref ICALL_ERRNO_NO_RESOURCE when ran out of resource.
 */
ICall_Errno ICall_registerApp(ICall_EntityID *entity, ICall_SyncHandle *msgSyncHdl);

/**
 * @brief       Allocates memory block for a message.
 *
 * @param size   size of the message body in bytes.
 * @return pointer to the start of the message body of the newly
 *         allocated memory block, or NULL if the allocation
 *         failed.
 */
void *ICall_allocMsg(size_t size);

/**
 * @brief       Frees the memory block allocated for a message.
 *
 * @param msg   pointer to the start of the message body
 *              which was returned from @ref ICall_allocMsg .
 */
void ICall_freeMsg(void *msg);

/**
 * @brief Sends a message to a registered server.
 *
 * @param src    Entity id of the sender of the message
 * @param dest   Service id
 * @param format Message format:
 *               @ref ICALL_MSG_FORMAT_KEEP,
 *               @ref ICALL_MSG_FORMAT_1ST_CHAR_TASK_ID or
 *               @ref ICALL_MSG_FORMAT_3RD_CHAR_TASK_ID.
 *               Message format indicates whether and which
 *               field of the message must be transformed
 *               into a implementation specific sender
 *               identity for an external image.<br>
 *               When a service message interface is defined,
 *               it may contain a field that is not understood
 *               by the client but only understood by
 *               the system on the server's side.
 *               The format provides an information to the
 *               messaging system on such a server
 *               so that it can generically tag necessary
 *               information to the message.
 * @param msg    pointer to the message body to send.<br>
 *               Note that if message is successfully sent,
 *               the caller should not reference the message any
 *               longer.<br>
 *               However, if the function fails, the caller
 *               still owns the reference to the message.
 *               That is, caller may attempt another send,
 *               or may free the memory block, etc.
 * @return @ref ICALL_ERRNO_SUCCESS when successful
 * @return @ref ICALL_ERRNO_INVALID_SERVICE when the 'dest'
 *         is unregistered service.<br>
 * @return @ref ICALL_ERRNO_INVALID_PARAMETER when the 'src'
 *         is an out of range entity id or when 'dest' is
 *         is a service that does not receive a message
 *         (such as ICall primitive service). <br>
 *         Note that as far as 'src' is within the range,
 *         this function won't notice the 'src' entity id
 *         as invalid.
 */
ICall_Errno
ICall_sendServiceMsg(ICall_EntityID src,
                     ICall_ServiceEnum dest,
                     ICall_MSGFormat format, void *msg);

/**
 * @brief Retrieves a message received at the message queue
 * associated with the calling thread.
 *
 * Note that this function should be used by an application
 * which does not expect any message from non-server entity.
 *
 * @param src    pointer to a variable to store the service id
 *               of the registered server which sent the retrieved
 *               message
 * @param dest   pointer to a variable to store the entity id
 *               of the destination of the message.
 * @param msg    pointer to a pointer variable to store the
 *               starting address of the message body being
 *               retrieved.
 * @return @ref ICALL_ERRNO_SUCCESS when the operation was successful
 *         and a message was retrieved.
 * @return @ref ICALL_ERRNO_NOMSG when there is no queued message
 *         at the moment.
 * @return @ref ICALL_ERRNO_CORRUPT_MSG when a message queued in
 *         front of the thread's receive queue was not sent by
 *         a server. Note that in this case, the message is
 *         not retrieved but thrown away.
 * @return @ref ICALL_ERRNO_UNKNOWN_THREAD when this function is
 *         called from a thread which has not registered
 *         an entity, either through ICall_enrollService
 *         or through ICall_registerApp .
 */
ICall_Errno
ICall_fetchServiceMsg(ICall_ServiceEnum *src,
                      ICall_EntityID *dest,
                      void **msg);

/**
 * @brief Waits for a signal to the semaphore associated with the calling thread.
 *
 * Note that the semaphore associated with a thread is signaled
 * when a message is queued to the message receive queue of the thread
 * or when ICall_signal  function is called onto the semaphore.
 *
 * @param milliseconds  timeout period in milliseconds.
 * @return @ref ICALL_ERRNO_SUCCESS when the semaphore is signaled.
 * @return @ref ICALL_ERRNO_TIMEOUT when designated timeout period
 *         has passed since the call of the function without
 *         the semaphore being signaled.
 */
ICall_Errno ICall_wait(uint_fast32_t milliseconds);

/**
 * @brief Signals a semaphore.
 * @param msgSyncHdl   handle of a synchronous object to signal
 * @return @ref ICALL_ERRNO_SUCCESS
 */
ICall_Errno ICall_signal(ICall_SyncHandle msgSyncHdl);

/**
 * @brief Registers a service entity
 * @param service      service id of the enrolling service
 * @param fn           handler function which handles function
 *                     calls to the service.
 * @param entity       pointer to a variable to store the assigned entity id
 * @param msgSyncHdl   pointer to a variable to store the synchronous object handle
 *                 associated with the calling thread.
 * @return @ref ICALL_ERRNO_SUCCESS when successful.
 * @return @ref ICALL_ERRNO_INVALID_PARAMETER when service id is already
 *         registered by another entity.
 * @return @ref ICALL_ERRNO_NO_RESOURCE when maximum number of services
 *         are already registered.
 */
ICall_Errno
ICall_enrollService(ICall_ServiceEnum service,
                    ICall_ServiceFunc fn,
                    ICall_EntityID *entity,
                    ICall_SyncHandle *msgSyncHdl);

/**
 * @brief Allocates a memory block.
 * @param size   size of the block in bytes.
 * @return address of the allocated memory block or NULL
 *         if allocation fails.
 */
void *ICall_malloc(uint_least16_t size);

/**
 * @brief Frees an allocated memory block.
 * @param msg  pointer to a memory block to free.
 */
void ICall_free(void *msg);

/**
 * Allocates a memory block, but check first if enough memory will be left after the allocation.
 * @param size   size of the block in bytes.
 * @return address of the allocated memory block or NULL
 *         if allocation fails.
 */
void *ICall_mallocLimited(uint_least16_t size);

/**
 * @brief Get Statistic on Heap.
 * @param stats  pointer to a heapStats_t structure.
 */
void ICall_getHeapStats(ICall_heapStats_t *stats);

/**
 * @brief Sends a message to an entity.
 * @param src     entity id of the sender
 * @param dest    entity id of the destination of the message.
 * @param format  message format. See ICall_sendServiceMsg .
 * @param msg     pointer to the message body.
 * @return @ref ICALL_ERRNO_SUCCESS when successful.
 * @return @ref ICALL_ERRNO_INVALID_PARAMETER when either src
 *              or dest is not a valid entity id or when
 *              dest is an entity id of an entity that does
 *              not receive a message
 *              (e.g., ICall primitive service entity).
 */
ICall_Errno ICall_send(ICall_EntityID src,
                              ICall_EntityID dest,
                              ICall_MSGFormat format,
                              void *msg);

/**
 * @brief Retrieves a message, queued to receive queue of the calling thread.
 *
 * @param src   pointer to a variable to store the sender entity id
 *              of the received message.
 * @param dest  pointer to a variable to store the destination entity id
 *              of the received message.
 * @param msg   pointer to a pointer variable to store the starting
 *              address of a received message body.
 * @return @ref ICALL_ERRNO_SUCCESS when a message was successfully
 *         retrieved.
 * @return @ref ICALL_ERRNO_NOMSG when no message was queued to
 *         the receive queue at the moment.
 * @return @ref ICALL_ERRNO_UNKNOWN_THREAD when the calling thread
 *         does not have a received queue associated with it.
 *         This happens when neither ICall_enrollService  nor
 *         ICall_registerApp  was ever called from the calling
 *         thread.
 */
ICall_Errno ICall_fetchMsg(ICall_EntityID *src,
                                         ICall_EntityID *dest,
                                         void **msg);

/**
 * @brief Transforms and entityId into a serviceId.
 * Note that this function is useful in case an application
 * waits for messages from both a server and another application,
 * in which case, the application can only use ICall_fetchMsg ,
 * not ICall_fetchServiceMsg  because the latter will return
 * @ref ICALL_ERRNO_CORRUPT_MSG when a message sent by the other
 * application is about to be fetched.<br>
 * This function, then, is useful to retrieve service id
 * matching the source entity id in case the source entity
 * id is not that of the other application.
 *
 * @param entityId   entity id
 * @param servId     pointer to a variable to store
 *                   the resultant service id
 * @return @ref ICALL_ERRNO_SUCCESS if the transformation was successful.
 * @return @ref ICALL_ERRNO_INVALID_SERVICE if no matching service
 *         is found for the entity id.
 */
ICall_Errno ICall_entityId2ServiceId(ICall_EntityID entityId,
                                                   ICall_ServiceEnum *servId);


#ifdef HEAPMGR_METRICS
/**
 * @brief the function retrieve information about the osal heap.
 * Note :ICall_getHeapMgrGetMetrics will not work when using heapMem and HeapTrack
 *
 * @param   pBlkMax   pointer to a variable to store max cnt of all blocks ever seen at once
 * @param   pBlkCnt   pointer to a variable to store current cnt of all blocks
 * @param   pBlkFree  pointer to a variable to store current cnt of free blocks
 * @param   pMemAlo   pointer to a variable to store current total memory allocated
 * @param   pMemMax   pointer to a variable to store max total memory ever allocated at once
 * @param   pMemUB    pointer to a variable to store the upper bound of memory usage
 *
 */
void ICall_getHeapMgrGetMetrics(uint32_t *pBlkMax,
                                uint32_t *pBlkCnt,
                                uint32_t *pBlkFree,
                                uint32_t *pMemAlo,
                                uint32_t *pMemMax,
                                uint32_t *pMemUB);


#endif

/**
 * @brief Aborts.
 *
 * This is preferred over C runtime abort  function,
 * in an external image since the C runtime abort  is only
 * guaranteed in a root image which contains the C runtime
 * entry function that is executed upon startup.
 *
 * @return @ref ICALL_ERRNO_SUCCESS.
 */
ICall_Errno
ICall_abort(void);

/**
 * @brief Enables interrupt.
 * @param intnum   interrupt number
 * @return @ref ICALL_ERRNO_SUCCESS.
 */
ICall_Errno
ICall_enableInt(int intnum);

/**
 * @brief Disables interrupt
 * @param intnum  interrupt number
 * @return @ref ICALL_ERRNO_SUCCESS
 */
ICall_Errno
ICall_disableInt(int intnum);

/**
 * @brief  Enables master interrupt and context switching.
 * @return @ref ICALL_ERRNO_SUCCESS
 */
ICall_Errno
ICall_enableMInt(void);

/**
 * @brief Disables master interrupt and context switching.
 * @return @ref ICALL_ERRNO_SUCCESS
 */
ICall_Errno
ICall_disableMInt(void);

/**
 * @brief Registers an interrupt service routine
 * @param intnum   interrupt number
 * @param isrfunc  pointer to the interrupt service function
 * @return @ref ICALL_ERRNO_SUCCESS when successful.
 * @return @ref ICALL_ERRNO_NO_RESOURCE when the registration
 *         failed due to lack of resources.
 */
ICall_Errno
ICall_registerISR(int intnum, void (*isrfunc)(void));

/// @brief ICall Register ISR
ICall_Errno
ICall_registerISR_Ext(int intnum, void (*isrfunc)(void), int intPriority );

/**
 * @brief Gets the current tick counter value.
 * @return current tick counter value
 */
uint_fast32_t
ICall_getTicks(void);

/**
 * @brief Gets the tick period.
 * @return tick period in microseconds.
 */
uint_fast32_t
ICall_getTickPeriod(void);
/**
 * @brief Gets the maximum timeout period supported by
 * ICall_setTimerMSecs  function.
 *
 * @return maximum timeout period in milliseconds
 */
uint_fast32_t
ICall_getMaxMSecs(void);

/**
 * @brief Set up or restart a timer.
 * Note that the timer setup initially by this function may be restarted
 * using ICall_setTimer  function as well.
 *
 * @param msecs  timeout period in milliseconds after which callback function
 *               shall be called.
 * @param cback  callback function pointer
 * @param arg    argument to pass to the callback function
 * @param id     pointer to the timer ID.
 *               If a new timer must be set up, the value of the timer ID
 *               must be set to @ref ICALL_INVALID_TIMER_ID when making this
 *               call and when the function returns successfully, the variable
 *               will be assigned a new timer ID value.
 *               The value other than @ref ICALL_INVALID_TIMER_ID shall be
 *               regarded as a request to restart the earlier set timer.
 * @return @ref ICALL_ERRNO_SUCCESS when successful
 * @return @ref ICALL_ERRNO_INVALID_PARAMETER when msecs is greater than
 *              maximum value supported
 * @return @ref ICALL_ERRNO_NO_RESOURCE when ran out of resource.
 *         Check ICall heap size and OS heap size if this happens.
 *
 * @ref ICall_getMaxMSecs
 */
ICall_Errno
ICall_setTimerMSecs(uint_fast32_t msecs,
                    ICall_TimerCback cback,
                    void *arg,
                    ICall_TimerID *id);

/**
 * @brief Set up or restart a timer.
 * Note that the timer setup initially by this function may be restarted
 * using ICall_setTimerMSecs  function as well.
 *
 * @param ticks  timeout period in ticks after which the callback function
 *               shall be called.
 * @param cback  callback function pointer
 * @param arg    argument to pass to the callback function
 * @param id     pointer to the timer ID.
 *               If a new timer must be set up, the value of the timer ID
 *               must be set to @ref ICALL_INVALID_TIMER_ID when making this
 *               call and when the function returns successfully, the variable
 *               will be assigned a new timer ID value.
 *               The value other than @ref ICALL_INVALID_TIMER_ID shall be
 *               regarded as a request to restart the earlier set timer.
 * @return @ref ICALL_ERRNO_SUCCESS when successful
 * @return @ref ICALL_ERRNO_NO_RESOURCE when ran out of resource.
 *         Check ICall heap size and OS heap size if this happens.
 *
 * @ref ICall_getTickPeriod
 */
ICall_Errno
ICall_setTimer(uint32_t ticks,
               ICall_TimerCback cback,
               void *arg,
               ICall_TimerID *id);

/**
 * @brief Stops a timer.
 *
 * @param id    timer ID.
 */
void
ICall_stopTimer(ICall_TimerID id);
/**
 * @brief Increments or decrements power activity counter.
 *
 * When power activity counter is greater than zero,
 * the device shall stay in the active power state.
 * The caller has to make sure that it decrements the counter
 * as many times as it has incremented the counter
 * when there is no activity that requires the active power state.
 * It is recommended that each client increments the counter by just one,
 * but it is not mandated to be so.
 *
 * @param incFlag   TRUE to indicate incrementing the counter.<br>
 *                  FALSE to indicate decrementing the counter.
 * @return TRUE if power is required.
 * @return FALSE if power is not required.
 */
bool
ICall_pwrUpdActivityCounter(bool incFlag);

/**
 * @brief Configures power constraint/dependency set/release actions upon
 * activity counter change.
 *
 * When activity counter value drops to zero, all constraints and
 * dependencies configured by this function shall be released.<br>
 * When activity counter value increments to one, all constraints
 * and dependencies configured by this function shall be set.
 *
 * @param bitmap  a bitmap of constraint or dependency flags.<br>
 *                The flag definition is specific to each platform.
 *                For instance, see ICallCC26xxDefs.h.
 *
 * @return @ref ICALL_ERRNO_SUCCESS when successful
 * @return @ref ICALL_ERRNO_INVALID_PARAMETER when an invalid
 *              flag in the bitmap is detected.
 */
ICall_Errno
ICall_pwrConfigACAction(ICall_PwrBitmap_t bitmap);
/**
 * @brief Sets power constraints and dependencies.
 *
 * @param bitmap  a bitmap of constraint or dependency flags.<br>
 *                The flag definition is specific to each platform.
 *                For instance, see ICallCC26xxDefs.h.
 *
 * @return @ref ICALL_ERRNO_SUCCESS when successful
 * @return @ref ICALL_ERRNO_INVALID_PARAMETER when an invalid
 *              flag in the bitmap is detected.
 */
ICall_Errno
ICall_pwrRequire(ICall_PwrBitmap_t bitmap);

/**
 * @brief Releases power constraints and dependencies.
 *
 * @param bitmap  a bitmap of constraint or dependency flags.<br>
 *                The flag definition is specific to each platform.
 *                For instance, see ICallCC26xxDefs.h.
 *
 * @return @ref ICALL_ERRNO_SUCCESS when successful
 * @return @ref ICALL_ERRNO_INVALID_PARAMETER when an invalid
 *              flag in the bitmap is detected.
 */
ICall_Errno
ICall_pwrDispense(ICall_PwrBitmap_t bitmap);

/**
 * @brief Checks whether HF XOSC is stable.
 * This function must be called after HF XOSC is turned on
 * (through power dependency).
 *
 * @return TRUE when HF XOSC is stable.
 * @return FALSE when HF XOSC is not stable.
 */
bool
ICall_pwrIsStableXOSCHF(void);

/**
 * @brief Switch clock source to HF XOSC.
 * This function must be called after HF XOSC is stable.
 *
 * @return @ref ICALL_ERRNO_SUCCESS
 */
ICall_Errno
ICall_pwrSwitchXOSCHF(void);
/**
 * Gets the estimated crystal oscillator startup time.
 *
 * @param timeUntilWakeupInMs time until wakeup in milliseconds
 *
 * @return estimated crystal oscillator startup time
 */
uint32_t
ICall_pwrGetXOSCStartupTime(uint_fast32_t timeUntilWakeupInMs);

/**
 * @brief Registers a power state transition notify function.
 *
 * The registered notify function shall be called when the power state
 * changes.
 *
 * @param fn  notify function pointer
 * @param obj pointer to data object to be passed to notify function.<br>
 *            This pointer must not be NULL and can point to an aggregate type
 *            containing the @ref ICall_PwrNotifyData.
 *            The object must not be released.
 * @return @ref ICALL_ERRNO_SUCCESS when successful
 * @return @ref ICALL_ERRNO_NO_RESOURCE when registration failed
 */
ICall_Errno
ICall_pwrRegisterNotify(ICall_PwrNotifyFn fn, ICall_PwrNotifyData *obj);
/**
 * @brief Retrieves power transition state.
 *
 * @return Implementation specific transition state when successful
 * @return Zero when the function is not implemented.
 */
uint_fast8_t
ICall_pwrGetTransitionState(void);

/**
 * @brief Creates a new RTOS task.
 *
 * @param entryfn   task entry function.
 * @param priority  task priority as understood by the underlying RTOS
 * @param stacksize stack size as understood by the underlying RTOS
 * @param arg       argument to pass to the task entry function
 *
 * @return @ref ICALL_ERRNO_SUCCESS when successful
 * @return @ref ICALL_ERRNO_NO_RESOURCE when creation failed
 */
ICall_Errno
ICall_createTask(void (*entryfn)(size_t arg), uint_fast8_t priority,
                 uint_fast16_t stacksize, size_t arg);

#ifdef ICALL_RTOS_SEMAPHORE_API
/**
 * @brief Creates a semaphore.
 *
 * @param mode  Semaphore mode<br>
 *              @ref ICALL_SEMAPHORE_MODE_COUNTING, or<br>
 *              @ref ICALL_SEMAPHORE_MODE_BINARY
 * @param initcount initial count value
 * @return created semaphore when successful
 * @return NULL when creation failed
 */
ICall_Semaphore
ICall_createSemaphore(uint_fast8_t mode, int initcount);
#endif

#ifdef ICALL_RTOS_SEMAPHORE_API
/**
 * @brief Post on a semaphore
 *
 * @param sem semaphore.
 *
 * @return @ref ICALL_ERRNO_SUCCESS when the operation was successful
 */
ICall_Errno
ICall_postSemaphore(ICall_Semaphore sem);
#endif /* ICALL_RTOS_SEMAPHORE_API */

#ifdef ICALL_RTOS_EVENTS_API
/**
 * @brief Creates a event.
 *
 * @return created event when successful<br>
 *         NULL when creation failed<br>
 */
ICall_Event
ICall_createEvent(void);
/**
 * @brief Post on an event
 *
 * @param event event.
 *
 * @return @ref ICALL_ERRNO_SUCCESS when the operation was successful
 */
ICall_Errno
ICall_postEvent(ICall_Event event, uint32_t events);
/**
 * @brief Waits on a event for ICALL_MSG_EVENT_ID
 *
 * @param event           event.
 * @param milliseconds  timeout in milliseconds
 *                      or @ref ICALL_TIMEOUT_FOREVER to wait forever
 *
 * @return @ref ICALL_ERRNO_SUCCESS when the operation was successful
 *         and event was received.
 * @return @ref ICALL_ERRNO_TIMEOUT when designated timeout period
 *         has passed since the call of the function without
 *         the event being signaled.
 */
ICall_Errno
ICall_waitEvent(ICall_Event event, uint_fast32_t milliseconds);
#endif /* ICALL_RTOS_EVENTS_API */

#ifdef ICALL_RTOS_SEMAPHORE_API
/**
 * @brief Waits on a semaphore
 *
 * @param sem           semaphore.
 * @param milliseconds  timeout in milliseconds
 *                      or @ref ICALL_TIMEOUT_FOREVER to wait forever
 *
 * @return @ref ICALL_ERRNO_SUCCESS when the operation was successful
 *         and semaphore was acquired.
 * @return @ref ICALL_ERRNO_TIMEOUT when designated timeout period
 *         has passed since the call of the function without
 *         the semaphore being signaled.
 */
ICall_Errno
ICall_waitSemaphore(ICall_Semaphore sem, uint_fast32_t milliseconds);
#endif /* ICALL_RTOS_SEMAPHORE_API */

/**
 * @brief Waits for and retrieves a message received at the message queue
 * associated with the calling thread, which matches a certain condition.
 *
 * @param milliseconds  timeout period in milliseconds.
 * @param matchFn  pointer to a function that would return TRUE when
 *                 the message matches its condition.
 * @param src    pointer to a variable to store the service id
 *               of the registered server which sent the retrieved
 *               message, or NULL if not interested in storing service id.
 * @param dest   pointer to a variable to store the entity id
 *               of the destination of the message,
 *               of NULL if not interested in storing the destination entity id.
 * @param msg    pointer to a pointer variable to store the
 *               starting address of the message body being
 *               retrieved. The pointer must not be NULL.
 * @return @ref ICALL_ERRNO_SUCCESS when the operation was successful
 *         and a message was retrieved.
 * @return @ref ICALL_ERRNO_TIMEOUT when designated timeout period
 *         has passed since the call of the function without
 *         the semaphore being signaled.
 * @return @ref ICALL_ERRNO_UNKNOWN_THREAD when this function is
 *         called from a thread which has not registered
 *         an entity, either through ICall_enrollService
 *         or through ICall_registerApp .
 */
ICall_Errno
ICall_waitMatch(uint_least32_t milliseconds,
                ICall_MsgMatchFn matchFn,
                ICall_ServiceEnum *src,
                ICall_EntityID *dest,
                void **msg);

/**
 * @brief Retrieves an entity ID of (arbitrary) one of the entities registered
 * from the calling thread.
 *
 * Note that, if multiple entities were registered from the same thread,
 * this function shall arbitrarily pick one of the entities.
 *
 * @return A valid entity ID or @ref ICALL_INVALID_ENTITY_ID
 *         when no entity was registered from the calling thread.
 */
ICall_EntityID
ICall_getEntityId(void);
/**
 * @brief Checks whether the calling thread provides the designated service.
 *
 * @param  service    Service enumeration value for the service.
 *
 * @return Non-zero if the current thread provides the designated service.
 * @return Zero, otherwise.
 */
uint_fast8_t
ICall_threadServes(ICall_ServiceEnum service);

/**
 * @brief Retrieves an internal ID understood only by a service entity,
 * corresponding to an ICall entity ID.
 *
 * This function is used when a message body includes a field indicating a
 * software entity and the destination of the message has its own internal
 * message routing mechanism with its own entity enumeration system.
 *
 * @param  service    Service enumeration value for the service.
 *                    Currently @ref ICALL_SERVICE_CLASS_BLE_MSG is supported.
 * @param  entity     ICall entity ID.
 *
 * @return Stack specific 8 bit ID or 0xFF when failed.
 */
uint_fast8_t
ICall_getLocalMsgEntityId(ICall_ServiceEnum service, ICall_EntityID entity);

#else /* ICALL_JT */

/**
 * @brief Registers an application.
 * Note that this function must be called from the thread
 * from which ICall_wait  function will be called.
 *
 * @param entity  pointer to a variable to store entity id assigned
 *                to the application.
 * @param msgSyncHdl  pointer to a variable to store the synchronous object handle
 *                associated with the calling thread.
 * @return @ref ICALL_ERRNO_SUCCESS when successful.
 * @return  @ref ICALL_ERRNO_NO_RESOURCE when ran out of resource.
 */
static ICall_Errno ICall_registerApp(ICall_EntityID *entity,
                                            ICall_SyncHandle *msgSyncHdl)
{
  ICall_RegisterAppArgs args;
  ICall_Errno errno;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_REGISTER_APP;
  errno = ICall_dispatcher((ICall_FuncArgsHdr *)&args);
  *entity = args.entity;
  *msgSyncHdl = args.msgSyncHdl;
  return errno;
}

/**
 * @brief Allocates memory block for a message.
 * @param size   size of the message body in bytes.
 * @return pointer to the start of the message body of the newly
 *         allocated memory block, or NULL if the allocation
 *         failed.
 */
static void *ICall_allocMsg(size_t size)
{
  ICall_AllocArgs args;
  ICall_Errno errno;

  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_MSG_ALLOC;
  args.size = size;
  errno = ICall_dispatcher((ICall_FuncArgsHdr *)&args);
  if (errno != ICALL_ERRNO_SUCCESS)
  {
    return NULL;
  }
  return args.ptr;
}

/**
 * @brief Frees the memory block allocated for a message.
 * @param msg   pointer to the start of the message body
 *              which was returned from ICall_allocMsg .
 */
static void ICall_freeMsg(void *msg)
{
  ICall_FreeArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_MSG_FREE;
  args.ptr = msg;
  (void) ICall_dispatcher((ICall_FuncArgsHdr *)&args);
}

/**
 * @brief Sends a message to a registered server.
 *
 * @param src    Entity id of the sender of the message
 * @param dest   Service id
 * @param format Message format:
 *               @ref ICALL_MSG_FORMAT_KEEP,
 *               @ref ICALL_MSG_FORMAT_1ST_CHAR_TASK_ID or
 *               @ref ICALL_MSG_FORMAT_3RD_CHAR_TASK_ID.
 *               Message format indicates whether and which
 *               field of the message must be transformed
 *               into a implementation specific sender
 *               identity for an external image.<br>
 *               When a service message interface is defined,
 *               it may contain a field that is not understood
 *               by the client but only understood by
 *               the system on the server's side.
 *               The format provides an information to the
 *               messaging system on such a server
 *               so that it can generically tag necessary
 *               information to the message.
 * @param msg    pointer to the message body to send.<br>
 *               Note that if message is successfully sent,
 *               the caller should not reference the message any
 *               longer.<br>
 *               However, if the function fails, the caller
 *               still owns the reference to the message.
 *               That is, caller may attempt another send,
 *               or may free the memory block, etc.
 * @return @ref ICALL_ERRNO_SUCCESS when successful.
 * @return @ref ICALL_ERRNO_INVALID_SERVICE when the 'dest'
 *         is unregistered service.<br>
 * @return @ref ICALL_ERRNO_INVALID_PARAMETER when the 'src'
 *         is an out of range entity id or when 'dest' is
 *         is a service that does not receive a message
 *         (such as ICall primitive service).
 *         Note that as far as 'src' is within the range,
 *         this function won't notice the 'src' entity id
 *         as invalid.
 */
static ICall_Errno
ICall_sendServiceMsg(ICall_EntityID src,
                     ICall_ServiceEnum dest,
                     ICall_MSGFormat format, void *msg)
{
  ICall_SendArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_SEND_SERV_MSG;
  args.src = src;
  args.dest.servId = dest;
  args.format = format;
  args.msg = msg;
  return ICall_dispatcher((ICall_FuncArgsHdr *)&args);
}

/**
 * @brief Retrieves a message received at the message queue
 * associated with the calling thread.
 *
 * Note that this function should be used by an application
 * which does not expect any message from non-server entity.
 *
 * @param src    pointer to a variable to store the service id
 *               of the registered server which sent the retrieved
 *               message
 * @param dest   pointer to a variable to store the entity id
 *               of the destination of the message.
 * @param msg    pointer to a pointer variable to store the
 *               starting address of the message body being
 *               retrieved.
 * @return @ref ICALL_ERRNO_SUCCESS when the operation was successful
 *         and a message was retrieved.
 * @return @ref ICALL_ERRNO_NOMSG when there is no queued message
 *         at the moment.
 * @return @ref ICALL_ERRNO_CORRUPT_MSG when a message queued in
 *         front of the thread's receive queue was not sent by
 *         a server. Note that in this case, the message is
 *         not retrieved but thrown away.
 * @return @ref ICALL_ERRNO_UNKNOWN_THREAD when this function is
 *         called from a thread which has not registered
 *         an entity, either through ICall_enrollService
 *         or through ICall_registerApp .
 */
static ICall_Errno
ICall_fetchServiceMsg(ICall_ServiceEnum *src,
                      ICall_EntityID *dest,
                      void **msg)
{
  ICall_FetchMsgArgs args;
  ICall_Errno errno;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_FETCH_SERV_MSG;
  errno = ICall_dispatcher((ICall_FuncArgsHdr *)&args);
  *src = args.src.servId;
  *dest = args.dest;
  *msg = args.msg;
  return errno;
}

/**
 * @brief Waits for a signal to the semaphore associated with the calling thread.
 *
 * Note that the semaphore associated with a thread is signaled
 * when a message is queued to the message receive queue of the thread
 * or when ICall_signal  function is called onto the semaphore.
 *
 * @param milliseconds  timeout period in milliseconds.
 * @return @ref ICALL_ERRNO_SUCCESS when the semaphore is signaled.
 * @return @ref ICALL_ERRNO_TIMEOUT when designated timeout period
 *         has passed since the call of the function without
 *         the semaphore being signaled.
 */
static ICall_Errno ICall_wait(uint_fast32_t milliseconds)
{
  ICall_WaitArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_WAIT;
  args.milliseconds = milliseconds;
  return ICall_dispatcher((ICall_FuncArgsHdr *)&args);
}

/**
 * @brief Signals a semaphore.
 * @param msgSyncHdl   handle of a synchronous object to signal
 * @return @ref ICALL_ERRNO_SUCCESS
 */
static ICall_Errno ICall_signal(ICall_SyncHandle msgSyncHdl)
{
  ICall_SignalArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_SIGNAL;
  args.syncHandle = msgSyncHdl;

  return ICall_dispatcher((ICall_FuncArgsHdr *)&args);
}

/**
 * @brief Registers a service entity
 *
 * @param service  service id of the enrolling service
 * @param fn       handler function which handles function
 *                 calls to the service.
 * @param entity   pointer to a variable to store the assigned entity id
 * @param msgSyncHdl   pointer to a variable to store the synchronous object handle
 *                 associated with the calling thread.
 * @return @ref ICALL_ERRNO_SUCCESS when successful.
 * @return @ref ICALL_ERRNO_INVALID_PARAMETER when service id is already
 *         registered by another entity.
 * @return @ref ICALL_ERRNO_NO_RESOURCE when maximum number of services
 *         are already registered.
 */
static ICall_Errno
ICall_enrollService(ICall_ServiceEnum service,
                    ICall_ServiceFunc fn,
                    ICall_EntityID *entity,
                    ICall_SyncHandle *msgSyncHdl)
{
  ICall_EnrollServiceArgs args;
  ICall_Errno errno;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_ENROLL;
  args.service = service;
  args.fn = fn;
  errno = ICall_dispatcher((ICall_FuncArgsHdr *)&args);
  *entity = args.entity;
  *msgSyncHdl = args.msgSyncHdl;
  return errno;
}

/**
 * @brief Allocates a memory block.
 *
 * @param size   size of the block in bytes.
 * @return address of the allocated memory block or NULL
 *         if allocation fails.
 */
static void *ICall_malloc(uint_least16_t size)
{
  ICall_AllocArgs args;
  ICall_Errno errno;

  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_MALLOC;
  args.size = size;
  errno = ICall_dispatcher((ICall_FuncArgsHdr *)&args);
  if (errno != ICALL_ERRNO_SUCCESS)
  {
    return NULL;
  }
  return args.ptr;
}

/**
 * @brief Frees an allocated memory block.
 *
 * @param msg  pointer to a memory block to free.
 */
static void ICall_free(void *msg)
{
  ICall_FreeArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_FREE;
  args.ptr = msg;
  (void) ICall_dispatcher((ICall_FuncArgsHdr *)&args);
}

/**
 * @brief Sends a message to an entity.
 *
 * @param src     entity id of the sender
 * @param dest    entity id of the destination of the message.
 * @param format  message format. See ICall_sendServiceMsg .
 * @param msg     pointer to the message body.
 * @return @ref ICALL_ERRNO_SUCCESS when successful.
 * @return @ref ICALL_ERRNO_INVALID_PARAMETER when either src
 *              or dest is not a valid entity id or when
 *              dest is an entity id of an entity that does
 *              not receive a message
 *              (e.g., ICall primitive service entity).
 */
static ICall_Errno ICall_send(ICall_EntityID src,
                                     ICall_EntityID dest,
                                     ICall_MSGFormat format,
                                     void *msg)
{
  ICall_SendArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_SEND_MSG;
  args.src = src;
  args.dest.entityId = dest;
  args.format = format;
  args.msg = msg;
  return ICall_dispatcher((ICall_FuncArgsHdr *)&args);
}

/**
 * @brief Retrieves a message, queued to receive queue of the calling thread.
 *
 * @param src   pointer to a variable to store the sender entity id
 *              of the received message.
 * @param dest  pointer to a variable to store the destination entity id
 *              of the received message.
 * @param msg   pointer to a pointer variable to store the starting
 *              address of a received message body.
 * @return @ref ICALL_ERRNO_SUCCESS when a message was successfully
 *         retrieved.
 * @return @ref ICALL_ERRNO_NOMSG when no message was queued to
 *         the receive queue at the moment.
 * @return @ref ICALL_ERRNO_UNKNOWN_THREAD when the calling thread
 *         does not have a received queue associated with it.
 *         This happens when neither ICall_enrollService  nor
 *         ICall_registerApp  was ever called from the calling
 *         thread.
 */
static ICall_Errno ICall_fetchMsg(ICall_EntityID *src,
                                         ICall_EntityID *dest,
                                         void **msg)
{
  ICall_FetchMsgArgs args;
  ICall_Errno errno;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_FETCH_MSG;
  errno = ICall_dispatcher((ICall_FuncArgsHdr *)&args);
  *src = args.src.entityId;
  *dest = args.dest;
  *msg = args.msg;
  return errno;
}

/**
 * @brief Transforms and entityId into a serviceId.
 *
 * Note that this function is useful in case an application
 * waits for messages from both a server and another application,
 * in which case, the application can only use ICall_fetchMsg ,
 * not ICall_fetchServiceMsg  because the latter will return
 * @ref ICALL_ERRNO_CORRUPT_MSG when a message sent by the other
 * application is about to be fetched.<br>
 * This function, then, is useful to retrieve service id
 * matching the source entity id in case the source entity
 * id is not that of the other application.
 *
 * @param entityId   entity id
 * @param servId     pointer to a variable to store
 *                   the resultant service id
 * @return @ref ICALL_ERRNO_SUCCESS if the transformation was successful.
 * @return @ref ICALL_ERRNO_INVALID_SERVICE if no matching service
 *         is found for the entity id.
 */
static ICall_Errno ICall_entityId2ServiceId(ICall_EntityID entityId,
                                                   ICall_ServiceEnum *servId)
{
  ICall_EntityId2ServiceIdArgs args;
  ICall_Errno errno;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_ENTITY2SERVICE;
  args.entityId = entityId;
  errno = ICall_dispatcher((ICall_FuncArgsHdr *)&args);
  *servId = args.servId;
  return errno;
}

/**
 * @brief Aborts.
 *
 * This is preferred over C runtime abort  function,
 * in an external image since the C runtime abort  is only
 * guaranteed in a root image which contains the C runtime
 * entry function that is executed upon startup.
 */
static ICall_Errno
ICall_abort(void)
{
  HAL_ASSERT( HAL_ASSERT_CAUSE_ICALL_ABORT );

  ICall_FuncArgsHdr args;
  args.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.func = ICALL_PRIMITIVE_FUNC_ABORT;
  return ICall_dispatcher(&args);
}

/**
 * @brief Enables interrupt.
 * @param intnum   interrupt number
 * @return @ref ICALL_ERRNO_SUCCESS.
 */
static ICall_Errno
ICall_enableInt(int intnum)
{
  ICall_IntNumArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_ENABLE_INT;
  args.intnum = intnum;
  return ICall_dispatcher((ICall_FuncArgsHdr *)&args);
}

/**
 * @brief Disables interrupt
 * @param intnum  interrupt number
 * @return @ref ICALL_ERRNO_SUCCESS
 */
static ICall_Errno
ICall_disableInt(int intnum)
{
  ICall_IntNumArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_DISABLE_INT;
  args.intnum = intnum;
  return ICall_dispatcher((ICall_FuncArgsHdr *)&args);
}

/**
 * @brief Enables master interrupt and context switching.
 * @return @ref ICALL_ERRNO_SUCCESS
 */
static ICall_Errno
ICall_enableMInt(void)
{
  ICall_FuncArgsHdr args;
  args.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.func = ICALL_PRIMITIVE_FUNC_ENABLE_MINT;
  return ICall_dispatcher(&args);
}

/**
 * @brief Disables master interrupt and context switching.
 * @return @ref ICALL_ERRNO_SUCCESS
 */
static ICall_Errno
ICall_disableMInt(void)
{
  ICall_FuncArgsHdr args;
  args.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.func = ICALL_PRIMITIVE_FUNC_DISABLE_MINT;
  return ICall_dispatcher(&args);
}

/**
 * @brief Registers an interrupt service routine
 * @param intnum   interrupt number
 * @param isrfunc  pointer to the interrupt service function
 * @return @ref ICALL_ERRNO_SUCCESS when successful.
 * @return @ref ICALL_ERRNO_NO_RESOURCE when the registration
 *         failed due to lack of resources.
 */
static ICall_Errno
ICall_registerISR(int intnum, void (*isrfunc)(void))
{
  ICall_RegisterISRArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_REGISTER_ISR;
  args.intnum = intnum;
  args.isrfunc = isrfunc;
  return ICall_dispatcher((ICall_FuncArgsHdr *)&args);
}

static ICall_Errno
ICall_registerISR_Ext(int intnum, void (*isrfunc)(void), int intPriority )
{
  ICall_RegisterISRArgs_Ext args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func    = ICALL_PRIMITIVE_FUNC_REGISTER_ISR_EXT;
  args.intnum      = intnum;
  args.isrfunc     = isrfunc;
  args.intPriority = intPriority;
  return ICall_dispatcher((ICall_FuncArgsHdr *)&args);
}

/**
 * @brief Gets the current tick counter value.
 * @return current tick counter value
 */
static uint_fast32_t
ICall_getTicks(void)
{
  ICall_GetUint32Args args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_GET_TICKS;
  (void) ICall_dispatcher((ICall_FuncArgsHdr *)&args);
  return args.value;
}

/**
 * @brief Gets the tick period.
 * @return tick period in microseconds.
 */
static uint_fast32_t
ICall_getTickPeriod(void)
{
  ICall_GetUint32Args args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_GET_TICK_PERIOD;
  (void) ICall_dispatcher((ICall_FuncArgsHdr *)&args);
  return args.value;
}

/**
 * @brief Gets the maximum timeout period supported by
 * ICall_setTimerMSecs  function.
 *
 * @return maximum timeout period in milliseconds
 */
static uint_fast32_t
ICall_getMaxMSecs(void)
{
  ICall_GetUint32Args args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_GET_MAX_MILLISECONDS;
  (void) ICall_dispatcher((ICall_FuncArgsHdr *)&args);
  return args.value;
}

/**
 * @brief Set up or restart a timer.
 * Note that the timer setup initially by this function may be restarted
 * using ICall_setTimer  function as well.
 *
 * @param msecs  timeout period in milliseconds after which callback function
 *               shall be called.
 * @param cback  callback function pointer
 * @param arg    argument to pass to the callback function
 * @param id     pointer to the timer ID.
 *               If a new timer must be set up, the value of the timer ID
 *               must be set to @ref ICALL_INVALID_TIMER_ID when making this
 *               call and when the function returns successfully, the variable
 *               will be assigned a new timer ID value.
 *               The value other than @ref ICALL_INVALID_TIMER_ID shall be
 *               regarded as a request to restart the earlier set timer.
 * @return @ref ICALL_ERRNO_SUCCESS when successful
 * @return @ref ICALL_ERRNO_INVALID_PARAMETER when msecs is greater than
 *              maximum value supported
 * @return @ref ICALL_ERRNO_NO_RESOURCE when ran out of resource.
 *         Check ICall heap size and OS heap size if this happens.
 *
 * @ref ICall_getMaxMSecs
 */
static ICall_Errno
ICall_setTimerMSecs(uint_fast32_t msecs,
                    ICall_TimerCback cback,
                    void *arg,
                    ICall_TimerID *id)
{
  ICall_SetTimerArgs args;
  ICall_Errno errno;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_SET_TIMER_MSECS;
  args.timeout = msecs;
  args.cback = cback;
  args.arg = arg;
  args.timerid = *id;
  errno = ICall_dispatcher((ICall_FuncArgsHdr *)&args);
  *id = args.timerid;

  return errno;
}

/**
 * @brief Set up or restart a timer.
 *
 * Note that the timer setup initially by this function may be restarted
 * using ICall_setTimerMSecs  function as well.
 *
 * @param ticks  timeout period in ticks after which the callback function
 *               shall be called.
 * @param cback  callback function pointer
 * @param arg    argument to pass to the callback function
 * @param id     pointer to the timer ID.
 *               If a new timer must be set up, the value of the timer ID
 *               must be set to @ref ICALL_INVALID_TIMER_ID when making this
 *               call and when the function returns successfully, the variable
 *               will be assigned a new timer ID value.
 *               The value other than @ref ICALL_INVALID_TIMER_ID shall be
 *               regarded as a request to restart the earlier set timer.
 * @return @ref ICALL_ERRNO_SUCCESS when successful
 * @return @ref ICALL_ERRNO_NO_RESOURCE when ran out of resource.
 *         Check ICall heap size and OS heap size if this happens.
 *
 * @ref ICall_getTickPeriod
 */
static ICall_Errno
ICall_setTimer(uint_fast32_t ticks,
               ICall_TimerCback cback,
               void *arg,
               ICall_TimerID *id)
{
  ICall_SetTimerArgs args;
  ICall_Errno errno;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_SET_TIMER;
  args.timeout = ticks;
  args.cback = cback;
  args.arg = arg;
  args.timerid = *id;
  errno = ICall_dispatcher((ICall_FuncArgsHdr *)&args);
  *id = args.timerid;

  return errno;
}

/**
 * @brief Stops a timer.
 *
 * @param id    timer ID.
 */
static void
ICall_stopTimer(ICall_TimerID id)
{
  ICall_StopTimerArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_STOP_TIMER;
  args.timerid = id;
  (void) ICall_dispatcher((ICall_FuncArgsHdr *)&args);
}

/**
 * @brief Increments or decrements power activity counter.
 *
 * When power activity counter is greater than zero,
 * the device shall stay in the active power state.
 * The caller has to make sure that it decrements the counter
 * as many times as it has incremented the counter
 * when there is no activity that requires the active power state.
 * It is recommended that each client increments the counter by just one,
 * but it is not mandated to be so.
 *
 * @param incFlag   TRUE to indicate incrementing the counter.<br>
 *                  FALSE to indicate decrementing the counter.
 * @return TRUE if power is required.
 * @return FALSE if power is not required.
 */
static bool
ICall_pwrUpdActivityCounter(bool incFlag)
{
  ICall_PwrUpdActivityCounterArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_PWR_UPD_ACTIVITY_COUNTER;
  args.incFlag = incFlag;
  (void)ICall_dispatcher((ICall_FuncArgsHdr *)&args);
  return (args.pwrRequired);
}

/**
 * @brief Configures power constraint/dependency set/release actions upon
 * activity counter change.
 *
 * When activity counter value drops to zero, all constraints and
 * dependencies configured by this function shall be released.<br>
 * When activity counter value increments to one, all constraints
 * and dependencies configured by this function shall be set.
 *
 * @param bitmap  a bitmap of constraint or dependency flags.<br>
 *                The flag definition is specific to each platform.
 *                For instance, see ICallCC26xxDefs.h.
 *
 * @return @ref ICALL_ERRNO_SUCCESS when successful
 * @return @ref ICALL_ERRNO_INVALID_PARAMETER when an invalid
 *              flag in the bitmap is detected.
 */
static ICall_Errno
ICall_pwrConfigACAction(ICall_PwrBitmap_t bitmap)
{
  ICall_PwrBitmapArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_PWR_CONFIG_AC_ACTION;
  args.bitmap = bitmap;
  return ICall_dispatcher((ICall_FuncArgsHdr *)&args);
}

/**
 * @brief Sets power constraints and dependencies.
 *
 * @param bitmap  a bitmap of constraint or dependency flags.<br>
 *                The flag definition is specific to each platform.
 *                For instance, see ICallCC26xxDefs.h.
 *
 * @return @ref ICALL_ERRNO_SUCCESS when successful
 * @return @ref ICALL_ERRNO_INVALID_PARAMETER when an invalid
 *              flag in the bitmap is detected.
 */
static ICall_Errno
ICall_pwrRequire(ICall_PwrBitmap_t bitmap)
{
  ICall_PwrBitmapArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_PWR_REQUIRE;
  args.bitmap = bitmap;
  return ICall_dispatcher((ICall_FuncArgsHdr *)&args);
}

/**
 * @brief Releases power constraints and dependencies.
 *
 * @param bitmap  a bitmap of constraint or dependency flags.<br>
 *                The flag definition is specific to each platform.
 *                For instance, see ICallCC26xxDefs.h.
 *
 * @return @ref ICALL_ERRNO_SUCCESS when successful
 * @return @ref ICALL_ERRNO_INVALID_PARAMETER when an invalid
 *              flag in the bitmap is detected.
 */
static ICall_Errno
ICall_pwrDispense(ICall_PwrBitmap_t bitmap)
{
  ICall_PwrBitmapArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_PWR_DISPENSE;
  args.bitmap = bitmap;
  return ICall_dispatcher((ICall_FuncArgsHdr *)&args);
}

/**
 * @brief Checks whether HF XOSC is stable.
 * This function must be called after HF XOSC is turned on
 * (through power dependency).
 *
 * @return TRUE when HF XOSC is stable.
 * @return FALSE when HF XOSC is not stable.
 */
static bool
ICall_pwrIsStableXOSCHF(void)
{
  ICall_GetBoolArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_PWR_IS_STABLE_XOSC_HF;
  (void) ICall_dispatcher((ICall_FuncArgsHdr *)&args);
  return args.value;
}

/**
 * @brief Switch clock source to HF XOSC.
 * This function must be called after HF XOSC is stable.
 *
 * @return @ref ICALL_ERRNO_SUCCESS
 */
static ICall_Errno
ICall_pwrSwitchXOSCHF(void)
{
  ICall_FuncArgsHdr args;
  args.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.func = ICALL_PRIMITIVE_FUNC_SWITCH_XOSC_HF;
  return ICall_dispatcher(&args);
}

/**
 * @brief Gets the estimated crystal oscillator startup time.
 *
 * @return estimated crystal oscillator startup time
 */
static uint32_t
ICall_pwrGetXOSCStartupTime(uint_fast32_t timeUntilWakeupInMs)
{
  ICall_PwrGetXOSCStartupTimeArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_PWR_GET_XOSC_STARTUP_TIME;
  args.timeUntilWakeupInMs = timeUntilWakeupInMs;
  (void) ICall_dispatcher((ICall_FuncArgsHdr *)&args);
  return args.value;
}

/**
 * @brief Registers a power state transition notify function.
 *
 * The registered notify function shall be called when the power state
 * changes.
 *
 * @param fn  notify function pointer
 * @param obj pointer to data object to be passed to notify function.<br>
 *            This pointer must not be NULL and can point to an aggregate type
 *            containing the @ref ICall_PwrNotifyData.
 *            The object must not be released.
 * @return @ref ICALL_ERRNO_SUCCESS when successful
 * @return @ref ICALL_ERRNO_NO_RESOURCE when registration failed
 */
static ICall_Errno
ICall_pwrRegisterNotify(ICall_PwrNotifyFn fn, ICall_PwrNotifyData *obj)
{
  ICall_PwrRegisterNotifyArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_PWR_REGISTER_NOTIFY;
  args.fn = fn;
  args.obj = obj;
  return ICall_dispatcher((ICall_FuncArgsHdr *)&args);
}

/**
 * @brief Retrieves power transition state.
 *
 * @return Implementation specific transition state when successful
 * @return Zero when the function is not implemented.
 */
static uint_fast8_t
ICall_pwrGetTransitionState(void)
{
  ICall_PwrGetTransitionStateArgs args;
  ICall_Errno errno;

  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_PWR_GET_TRANSITION_STATE;
  errno = ICall_dispatcher((ICall_FuncArgsHdr *)&args);

  if (errno == ICALL_ERRNO_SUCCESS)
  {
    return args.state;
  }
  return 0;
}

/**
 * @brief Creates a new RTOS task.
 *
 * @param entryfn   task entry function.
 * @param priority  task priority as understood by the underlying RTOS
 * @param stacksize stack size as understood by the underlying RTOS
 * @param arg       argument to pass to the task entry function
 *
 * @return @ref ICALL_ERRNO_SUCCESS when successful
 * @return  @ref ICALL_ERRNO_NO_RESOURCE when creation failed
 */
static ICall_Errno
ICall_createTask(void (*entryfn)(size_t arg), uint_fast8_t priority,
                 uint_fast16_t stacksize, size_t arg)
{
  ICall_CreateTaskArgs args;

  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_CREATE_TASK;
  args.entryfn = entryfn;
  args.priority = priority;
  args.stacksize = stacksize;
  args.arg = arg;
  return ICall_dispatcher((ICall_FuncArgsHdr *)&args);
}

#ifdef ICALL_RTOS_SEMAPHORE_API
/**
 * @brief Creates a semaphore.
 *
 * @param mode  Semaphore mode<br>
 *              @ref ICALL_SEMAPHORE_MODE_COUNTING, or<br>
 *              @ref ICALL_SEMAPHORE_MODE_BINARY
 * @param initcount initial count value
 * @return created semaphore when successful
 * @return NULL when creation failed
 */
static ICall_Semaphore
ICall_createSemaphore(uint_fast8_t mode, int initcount)
{
  ICall_CreateSemaphoreArgs args;
  ICall_Errno errno;

  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_CREATE_SEMAPHORE;
  args.mode = mode;
  args.initcount = initcount;
  errno = ICall_dispatcher((ICall_FuncArgsHdr *)&args);
  if (errno == ICALL_ERRNO_SUCCESS)
  {
    return args.sem;
  }
  return NULL;
}
#endif

#ifdef ICALL_RTOS_SEMAPHORE_API
/**
 * @brief Post on a semaphore
 *
 * @param sem  semaphore.
 *
 * @return @ref ICALL_ERRNO_SUCCESS when the operation was successful
 */
static ICall_Errno
ICall_postSemaphore(ICall_Semaphore sem)
{
  ICall_PostSemaphoreArgs args;

  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_POST_SEMAPHORE;
  args.sem = sem;
  return ICall_dispatcher((ICall_FuncArgsHdr *)&args);
}
#endif /* ICALL_RTOS_SEMAPHORE_API */

#ifdef ICALL_RTOS_EVENTS_API
/**
 * @brief Creates a event.
 *
 * @return created event when successful
 * @return NULL when creation failed
 */
static ICall_Event
ICall_createEvent(void)
{
  ICall_CreateEventArgs args;
  ICall_Errno errno;

  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_CREATE_EVENT;
  errno = ICall_dispatcher((ICall_FuncArgsHdr *)&args);
  if (errno == ICALL_ERRNO_SUCCESS)
  {
    return args.event;
  }
  return NULL;
}

/**
 * @brief Post on an event
 *
 * @param event  event.
 *
 * @return @ref ICALL_ERRNO_SUCCESS when the operation was successful
 */
static ICall_Errno
ICall_postEvent(ICall_Event event, uint32_t events)
{
  ICall_PostEventArgs args;

  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_POST_EVENT;
  args.event = event;
  args.events = events;
  return ICall_dispatcher((ICall_FuncArgsHdr *)&args);
}
/**
 * @brief Waits on a event for ICALL_MSG_EVENT_ID
 *
 * @param event           event.
 * @param milliseconds  timeout in milliseconds
 *                      or @ref ICALL_TIMEOUT_FOREVER to wait forever
 *
 * @return @ref ICALL_ERRNO_SUCCESS when the operation was successful
 *         and event was received.
 * @return @ref ICALL_ERRNO_TIMEOUT when designated timeout period
 *         has passed since the call of the function without
 *         the event being signaled.
 */
static ICall_Errno
ICall_waitEvent(ICall_Event event, uint_fast32_t milliseconds)
{
  ICall_WaitEventArgs args;

  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_WAIT_EVENT;
  args.event = event;
  args.milliseconds = milliseconds;
  return ICall_dispatcher((ICall_FuncArgsHdr *)&args);
}
#endif /* ICALL_RTOS_EVENTS_API */

#ifdef ICALL_RTOS_SEMAPHORE_API
/**
 * @brief Waits on a semaphore
 *
 * @param sem           semaphore.
 * @param milliseconds  timeout in milliseconds
 *                      or @ref ICALL_TIMEOUT_FOREVER to wait forever
 *
 * @return @ref ICALL_ERRNO_SUCCESS when the operation was successful
 *         and semaphore was acquired.
 * @return @ref ICALL_ERRNO_TIMEOUT when designated timeout period
 *         has passed since the call of the function without
 *         the semaphore being signaled.
 */
static ICall_Errno
ICall_waitSemaphore(ICall_Semaphore sem, uint_fast32_t milliseconds)
{
  ICall_WaitSemaphoreArgs args;

  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_WAIT_SEMAPHORE;
  args.sem = sem;
  args.milliseconds = milliseconds;
  return ICall_dispatcher((ICall_FuncArgsHdr *)&args);
}
#endif /* ICALL_RTOS_SEMAPHORE_API */

/**
 * @brief Waits for and retrieves a message received at the message queue
 * associated with the calling thread, which matches a certain condition.
 *
 * @param milliseconds  timeout period in milliseconds.
 * @param matchFn  pointer to a function that would return TRUE when
 *                 the message matches its condition.
 * @param src    pointer to a variable to store the service id
 *               of the registered server which sent the retrieved
 *               message, or NULL if not interested in storing service id.
 * @param dest   pointer to a variable to store the entity id
 *               of the destination of the message,
 *               of NULL if not interested in storing the destination entity id.
 * @param msg    pointer to a pointer variable to store the
 *               starting address of the message body being
 *               retrieved. The pointer must not be NULL.
 * @return @ref ICALL_ERRNO_SUCCESS when the operation was successful
 *         and a message was retrieved.
 * @return @ref ICALL_ERRNO_TIMEOUT when designated timeout period
 *         has passed since the call of the function without
 *         the semaphore being signaled.
 * @return @ref ICALL_ERRNO_UNKNOWN_THREAD when this function is
 *         called from a thread which has not registered
 *         an entity, either through ICall_enrollService
 *         or through ICall_registerApp .
 */
static ICall_Errno
ICall_waitMatch(uint_least32_t milliseconds,
                ICall_MsgMatchFn matchFn,
                ICall_ServiceEnum *src,
                ICall_EntityID *dest,
                void **msg)
{
  ICall_WaitMatchArgs args;
  ICall_Errno errno;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_WAIT_MATCH;
  args.milliseconds = milliseconds;
  args.matchFn = matchFn;
  errno = ICall_dispatcher((ICall_FuncArgsHdr *)&args);
  if (src != NULL)
  {
    *src = args.servId;
  }
  if (dest != NULL)
  {
    *dest = args.dest;
  }
  *msg = args.msg;
  return errno;
}

/**
 * @brief Retrieves an entity ID of (arbitrary) one of the entities registered
 * from the calling thread.
 *
 * @note If multiple entities were registered from the same thread,
 * this function shall arbitrarily pick one of the entities.
 *
 * @return A valid entity ID or @ref ICALL_INVALID_ENTITY_ID
 *         when no entity was registered from the calling thread.
 */
static ICall_EntityID
ICall_getEntityId(void)
{
  ICall_GetEntityIdArgs args;
  ICall_Errno errno;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_GET_ENTITY_ID;
  errno = ICall_dispatcher((ICall_FuncArgsHdr *)&args);
  if (errno == ICALL_ERRNO_SUCCESS)
  {
    return args.entity;
  }
  return ICALL_INVALID_ENTITY_ID;
}

/**
 * @brief Checks whether the calling thread provides the designated service.
 *
 * @param  service    Service enumeration value for the service.
 *
 * @return Non-zero if the current thread provides the designated service.
 *         Zero, otherwise.
 */
static uint_fast8_t
ICall_threadServes(ICall_ServiceEnum service)
{
  ICall_ThreadServesArgs args;
  ICall_Errno errno;

  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_THREAD_SERVES;
  args.servId = service;
  errno = ICall_dispatcher((ICall_FuncArgsHdr *)&args);
  if (errno == ICALL_ERRNO_SUCCESS)
  {
    return args.result;
  }
  return 0;
}

/**
 * @brief Retrieves an internal ID understood only by a service entity,
 * corresponding to an ICall entity ID.
 *
 * This function is used when a message body includes a field indicating a
 * software entity and the destination of the message has its own internal
 * message routing mechanism with its own entity enumeration system.
 *
 * @param  service    Service enumeration value for the service.
 *                    Currently @ref ICALL_SERVICE_CLASS_BLE_MSG is supported.
 * @param  entity     ICall entity ID.
 *
 * @return Stack specific 8 bit ID or 0xFF when failed.
 */
static uint_fast8_t
ICall_getLocalMsgEntityId(ICall_ServiceEnum service, ICall_EntityID entity)
{
  ICall_GetLocalMsgEntityIdArgs args;
  ICall_Errno errno;
  args.hdr.service = service;
  args.hdr.func = ICALL_MSG_FUNC_GET_LOCAL_MSG_ENTITY_ID;
  args.entity = entity;
  errno = ICall_dispatcher((ICall_FuncArgsHdr *)&args);
  if (errno == ICALL_ERRNO_SUCCESS)
  {
    return args.localId;
  }
  return 0xFF;
}


#endif /* ICALL_JT */

#ifdef ICALL_NO_APP_EVENTS
typedef uint8_t (*appCallback_t)(uint8_t event, uint8_t *msg);
ICall_Errno ICall_registerAppCback(uint8_t *selfEntity, appCallback_t appCallback);
#endif

#ifdef __cplusplus
}
#endif

#endif /* ICALL_H */

/** @} End ICall */
