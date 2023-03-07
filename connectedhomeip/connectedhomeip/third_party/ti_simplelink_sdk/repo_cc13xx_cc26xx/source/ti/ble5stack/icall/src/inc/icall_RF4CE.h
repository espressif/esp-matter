/******************************************************************************

 @file  icall_RF4CE.h

 @brief Indirect function Call dispatcher interface definition

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
#ifndef ICALL_H
#define ICALL_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Bit mask for recommended service id enumeration.
 * These bits can be used to identify a service class.
 */
#define ICALL_SERVICE_CLASS_MASK        0xFFF8

/**
 * Bit mask for recommended service id enumeration.
 * These bits can be used to identify an instance
 * among stack instances of the same service class.
 */
#define ICALL_SERVICE_INSTANCE_MASK     0x0007

/**
 * Service class enumerated value for primitive service.
 * Primitive service includes heap service
 * and messaging service.
 */
#define ICALL_SERVICE_CLASS_PRIMITIVE   0x0008

/* Radio stack services */

/** BLE service class enumerated value */
#define ICALL_SERVICE_CLASS_BLE         0x0010

/** TIMAC service class enumerated value */
#define ICALL_SERVICE_CLASS_TIMAC       0x0018

/** RemoTI RTI interface service class enumerated value*/
#define ICALL_SERVICE_CLASS_REMOTI_RTI  0x0020

/** RemoTI network layer service class enumerated value */
#define ICALL_SERVICE_CLASS_REMOTI_RCN  0x0028

/** ZStack service class enumerated value */
#define ICALL_SERVICE_CLASS_ZSTACK      0x0030

/** NPI service class enumerated value */
#define ICALL_SERVICE_CLASS_NPI         0x0038

/** BLE stack message delivery service */
#define ICALL_SERVICE_CLASS_BLE_MSG     0x0050

/* Services that are visible only for other services
 * but not for applications */

/**
 * Crypto service class.
 * Crypto service is intended for stack software to use.
 */
#define ICALL_SERVICE_CLASS_CRYPTO      0x0080

/**
 * BLE Board dependent service class.
 * This service is intended for BLE stack software's use only.
 */
#define ICALL_SERVICE_CLASS_BLE_BOARD   0x0088

/**
 * TIMAC Board dependent service class.
 * This service is intended for TIMAC stack software's use only.
 */
#define ICALL_SERVICE_CLASS_TIMAC_BOARD 0x0090

/**
 * Radio service class.
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
 * Dummy stack board service class enumeration.
 * Dummy stack board service is used as an example
 * board service for a stack.
 */
#define ICALL_SERVICE_CLASS_DUMMY_BOARD 0x0200

/**
 * Entity ID used to indicate invalid entity
 */
#define ICALL_INVALID_ENTITY_ID            0xffu

/**
 * Timer ID used to indicate invalid ID.
 */
#define ICALL_INVALID_TIMER_ID             NULL

/**
 * @internal
 * Initial value of dest_id field value of @ref ICall_MsgHdr.
 * Note that this value should not be used by regular ICall applications.
 * The value and the message header are meant to be used
 * only by a tightly coupled module.
 */
#define ICALL_UNDEF_DEST_ID                0xffu

/** ICall function return value for successful operation */
#define ICALL_ERRNO_SUCCESS                0

/** ICall function return value for timeout error */
#define ICALL_ERRNO_TIMEOUT                1

/** ICall function return value for no message error */
#define ICALL_ERRNO_NOMSG                  2

/**
 * ICall function return value in case the service
 * corresponding to a passed service id is not registered
 * and hence is unknown.
 */
#define ICALL_ERRNO_INVALID_SERVICE        -1

/**
 * ICall function return value in case the function id
 * is unknown to the registered handler of the service.
 */
#define ICALL_ERRNO_INVALID_FUNCTION       -2

/** ICall function return value for invalid parameter value */
#define ICALL_ERRNO_INVALID_PARAMETER      -3

/** ICall function return value for no resource error */
#define ICALL_ERRNO_NO_RESOURCE            -4

/**
 * ICall function return value in case the calling
 * thread context is not a registered thread,
 * or the entity id passed is not a registered entity.
 */
#define ICALL_ERRNO_UNKNOWN_THREAD         -5

/** ICall function return value for corrupt message error */
#define ICALL_ERRNO_CORRUPT_MSG            -6

/** ICall function return value for counter overflow */
#define ICALL_ERRNO_OVERFLOW               -7

/** ICall function return value for counter underflow */
#define ICALL_ERRNO_UNDERFLOW              -8

/** Message format that requires no change */
#define ICALL_MSG_FORMAT_KEEP              0

/**
 * Message format that requires the first byte
 * to be replaced with the src task ID
 */
#define ICALL_MSG_FORMAT_1ST_CHAR_TASK_ID  1

/**
 * Message format that requires the third byte
 * to be replaced with the src task ID
 */
#define ICALL_MSG_FORMAT_3RD_CHAR_TASK_ID  2

/**
 * Time period that indicates infinite time
 */
#define ICALL_TIMEOUT_FOREVER              0xfffffffful

/**
 * Counting semaphore mode
 */
#define ICALL_SEMAPHORE_MODE_COUNTING      0

/**
 * Binary semaphore mode
 */
#define ICALL_SEMAPHORE_MODE_BINARY        1

/* Primitive service function enumeration */

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

/**
 * Messaging service function id for translating ICall_entityID
 * to locally understandable id.
 */
#define ICALL_MSG_FUNC_GET_LOCAL_MSG_ENTITY_ID            0

/** Error code data type */
typedef int_fast16_t ICall_Errno;

/** Service enumeration data type */
typedef uint_least16_t ICall_ServiceEnum;

/** Message conversion format data type */
typedef uint_least8_t ICall_MSGFormat;

/** Function id data type */
typedef uint_least16_t ICall_FuncID;

/** Semaphore data type */
typedef void *ICall_SyncObject;

typedef void *ICall_Semaphore;

#ifdef ICALL_EVENTS
/** Event data type */
typedef void *ICall_Event;
#endif /* ICALL_EVENTS */

/** Entity id data type */
typedef uint_least16_t ICall_EntityID;

/** Timer id data type */
typedef void *ICall_TimerID;

/**
 * Prototype of a function used to compare a received message for a match
 * @param src   originator of the message as a service enumeration
 * @param dest  destination entity id of the message
 * @param msg   pointer to the message body
 *
 * @return TRUE when the message matches. FALSE, otherwise.
 */
typedef bool (*ICall_MsgMatchFn)(ICall_ServiceEnum src, ICall_EntityID dest,
                                 const void *msg);

/**
 * Prototype of a callback function for timer
 * @param arg   argument passed through ICall_setTimer()
 */
typedef void (*ICall_TimerCback)(void *arg);

/** Common service function arguments */
typedef struct _icall_func_args_hdr_t
{
  /** service id */
  ICall_ServiceEnum service;
  /** function id */
  ICall_FuncID func;
} ICall_FuncArgsHdr;

/** ICall_registerApp() arguments */
typedef struct _icall_register_app_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** field to store the entity id assigned to the application */
  ICall_EntityID    entity;
  /** field to store the synchronous object handle associated
   *  with the calling thread. */
  ICall_SyncObject   msgSyncObj;
} ICall_RegisterAppArgs;

/** ICall_allocMsg() or ICall_malloc() arguments */
typedef struct _icall_alloc_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** size of the message body in bytes */
  size_t size;
  /** pointer to store the start address of the newly allocated memory block */
  void * ptr;
} ICall_AllocArgs;

/** ICall_freeMsg() or ICall_free() arguments */
typedef struct _icall_free_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** pointer to the address of a memory block to free */
  void * ptr;
} ICall_FreeArgs;

/** ICall_sendServiceMsg() and ICall_send() arguments */
typedef struct _icall_send_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** Entity id of the sender of the message */
  ICall_EntityID src;
  /** Destination id */
  union
  {
    /** Entity ID of destination */
    ICall_EntityID entityId;
    /** Service ID of destination */
    ICall_ServiceEnum servId;
  } dest;
  /** Message format */
  ICall_MSGFormat format;
  /** pointer to the message body to send */
  void *msg;
} ICall_SendArgs;

/** ICall_fetchServiceMsg() and ICall_fetchMsg() arguments */
typedef struct _icall_fetch_msg_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** field to store source of the message */
  union
  {
    /** service id */
    ICall_ServiceEnum servId;
    /** entity id */
    ICall_EntityID entityId;
  } src;
  /** field to store the entity id of the destination of the message */
  ICall_EntityID dest;
  /** field to store the starting address of the message body */
  void *msg;
} ICall_FetchMsgArgs;

/** ICall_wait() arguments */
typedef struct _icall_wait_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** timeout period in milliseconds */
  uint_least32_t milliseconds;
} ICall_WaitArgs;

/** ICall_signal() arguments */
typedef struct _icall_signal_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** handle of a synchronous object to signal */
  ICall_SyncObject syncObject;
} ICall_SignalArgs;

/** ICall_signal() arguments */
typedef struct _icall_signal_events_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** handle of a synchronous object to signal */
  ICall_SyncObject syncObject;
  uint32_t          events;
} ICall_SignalEventsArgs;

/* forward reference */
struct _icall_enroll_service_args_t;

/** ICall_entityId2ServiceId() arguments */
typedef struct _icall_entity2service_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** entity id */
  ICall_EntityID entityId;
  /** field to store the service id */
  ICall_ServiceEnum servId;
} ICall_EntityId2ServiceIdArgs;

/** ICall_enableInt() and ICall_disableInt() arguments */
typedef struct _icall_intnum_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** interrupt number */
  int_least32_t intnum;
} ICall_IntNumArgs;

/** ICall_registerISR() arguments */
typedef struct _icall_register_isr_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** interrupt number */
  int_least32_t intnum;
  /** pointer to the interrupt service function */
  void (*isrfunc)(void);
} ICall_RegisterISRArgs;

/** ICall_registerISR_Ext() arguments */
typedef struct _icall_register_isr_args_ext_t
{
  ICall_FuncArgsHdr hdr;               // common arguments
  int_least32_t     intnum;            // interrupt number
  void              (*isrfunc)(void);  // pointer to ISR
  int               intPriority;       // interrupt priority
} ICall_RegisterISRArgs_Ext;

/**
 * ICall_getTicks(), ICall_getTickPeriod() and ICall_getMaxMSecs()
 * arguments
 */
typedef struct _icall_getuint32_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** field to store the retrieved value */
  uint_least32_t value;
} ICall_GetUint32Args;

/**
 * ICall_pwrIsStableXOSCHF() arguments
 */
typedef struct _icall_getbool_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** field to store the retrieved value */
  bool value;
} ICall_GetBoolArgs;

/** ICall_pwrUpdActivityCounter() arguments */
typedef struct _icall_pwr_upd_activity_counter_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** a boolean flag to indicate either increment or decrement */
  bool incFlag;
} ICall_PwrUpdActivityCounterArgs;

/**
 * Power control bitmap type
 */
typedef uint_fast32_t ICall_PwrBitmap_t;

/**
 * ICall_pwrConfigActivityCounterAction(),
 * ICall_pwrRequire() and ICall_pwrDispense() arguments
 */
typedef struct _icall_pwr_bitmap_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** power configuration bitmap */
  ICall_PwrBitmap_t bitmap;
} ICall_PwrBitmapArgs;

/**
 * ICall_pwrGetTransitionState() arguments
 */
typedef struct _icall_pwr_get_transition_state_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** power transition state*/
  uint_fast8_t state;
} ICall_PwrGetTransitionStateArgs;

/**
 * ICall_pwrGetXOSCStartupTime() arguments
 */
typedef struct _icall_pwr_get_xosc_startup_time_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** field matching timeUntilWakeupInMs argument */
  uint_least32_t timeUntilWakeupInMs;
  /** field to store the retrieved value */
  uint_least32_t value;
} ICall_PwrGetXOSCStartupTimeArgs;

/** ICall_threadServes() argument */
typedef struct _icall_thread_serves_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** Service ID of destination */
  ICall_ServiceEnum servId;
  /** Result (true or false) */
  uint_fast8_t result;
} ICall_ThreadServesArgs;

/** ICall_createTask() argument */
typedef struct _icall_create_task_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** task entry function */
  void (*entryfn)(size_t arg);
  /** task priority as understood by underlying RTOS implementation */
  uint_least8_t priority;
  /** task stack size as understood by underlying RTOS implementation */
  uint_least16_t stacksize;
  /** argument passed to the task entry function */
  size_t arg;
} ICall_CreateTaskArgs;

#ifdef ICALL_EVENTS
/** ICall_createEvent() argument */
typedef struct _icall_create_event_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** created event */
  ICall_Event event;
} ICall_CreateEventArgs;
#endif /* ICALL_EVENTS */
/** ICall_createSemaphore() argument */
typedef struct _icall_create_semaphore_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /**
   * semaphore mode.
   * Either @ref ICALL_SEMAPHORE_MODE_BINARY or
   * @ref ICALL_SEMAPHORE_MODE_COUNTING
   */
  uint_least8_t mode;
  /** semaphore initial count value */
  int initcount;
  /** created semaphore */
  ICall_Semaphore sem;
} ICall_CreateSemaphoreArgs;

#ifdef ICALL_EVENTS
/** ICall_waitSemaphore() argument */
typedef struct _icall_post_semaphore_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** semaphore */
  ICall_Semaphore sem;
} ICall_PostSemaphoreArgs;
/** ICall_psotEvent() argument */
typedef struct _icall_post_event_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** event */
  ICall_Event event;
  /** events */
  uint32_t events;
} ICall_PostEventArgs;
/** ICall_waitEvent() argument */
typedef struct _icall_wait_event_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** event */
  ICall_Event event;
  /** timeout in milliseconds */
  uint_least32_t milliseconds;
} ICall_WaitEventArgs;
#endif /* ICALL_EVENTS */

/** ICall_waitSemaphore() argument */
typedef struct _icall_wait_semaphore_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** semaphore */
  ICall_Semaphore sem;
  /** timeout in milliseconds */
  uint_least32_t milliseconds;
} ICall_WaitSemaphoreArgs;

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

/**
 * Power state transition type of the following values:<br>
 * @ref ICALL_PWR_AWAKE_FROM_STANDBY<br>
 * @ref ICALL_PWR_AWAKE_FROM_STANDBY_LATE<br>
 * @ref ICALL_PWR_ENTER_STANDBY<br>
 * @ref ICALL_PWR_ENTER_SHUTDOWN<br>
 */
typedef uint_fast8_t ICall_PwrTransition;

/* Forward reference */
struct _icall_pwr_notify_data_t;

/**
 * Power state transition notify function type
 * @param pwrTrans  power transition.<br>
 *                  The values are defined as per platform.
 *                  For example, see ICallCC26xxDefs.h.
 * @param obj       pointer data object to identify the client.<br>
 *                  Note that the pointer must not be NULL.
 */
typedef void (*ICall_PwrNotifyFn)(ICall_PwrTransition pwrTrans,
                                  struct _icall_pwr_notify_data_t *obj);

/**
 * Power state transition notify function data object type
 */
typedef struct _icall_pwr_notify_data_t
{
  /**
   * a field for private use. Do not access the field.
   * User data should follow this field.
   */
  ICall_PwrNotifyFn _private;
} ICall_PwrNotifyData;

/** ICall_pwrRegisterNotify() arguments */
typedef struct _icall_pwr_register_notify_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** notify function to handle notification */
  ICall_PwrNotifyFn fn;
  /** data object */
  ICall_PwrNotifyData *obj;
} ICall_PwrRegisterNotifyArgs;

/** ICall_waitMatch() arguments */
typedef struct _icall_wait_match_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** timeout period in milliseconds */
  uint_least32_t milliseconds;
  /** match function */
  ICall_MsgMatchFn matchFn;
  /** service id */
  ICall_ServiceEnum servId;
  /** field to store the entity id of the destination of the message */
  ICall_EntityID dest;
  /** field to store the starting address of the message body */
  void *msg;
} ICall_WaitMatchArgs;

/** ICall_getEntityId() arguments */
typedef struct _icall_get_entity_id_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** client entity id */
  ICall_EntityID entity;
} ICall_GetEntityIdArgs;

/**
 * Call dispatcher function pointer type.
 * @param args    arguments
 * @return error code
 */
typedef ICall_Errno (*ICall_Dispatcher)(ICall_FuncArgsHdr *args);

/**
 * Critical section state data type
 */
typedef uint_least32_t ICall_CSState;

/**
 * Critical section entry function pointer type
 * @return critical section state before entry.
 */
typedef ICall_CSState (*ICall_EnterCS)(void);

/**
 * Critical section exit function pointer type
 * @param critical section state returned from @ref ICall_EnterCS
 *        type function matching to this exit.
 */
typedef void (*ICall_LeaveCS)(ICall_CSState key);

/** Call dispatcher pointer of the current image */
extern ICall_Dispatcher ICall_dispatcher;

/** Enter critical section function pointer of the current image */
extern ICall_EnterCS ICall_enterCriticalSection;

/** Leave critical section function pointer of the current image */
extern ICall_LeaveCS ICall_leaveCriticalSection;

/** Data type of the first argument passed to the entry point
 *  of an image which contains a remote task. */
typedef struct _icall_remote_task_arg_t
{
  ICall_Dispatcher dispatch;
  ICall_EnterCS entercs;
  ICall_LeaveCS leavecs;
} ICall_RemoteTaskArg;

/**
 * Service entry function prototype
 * Note that multiple services must be enrolled from a single
 * function call per downloadable stack image.
 * All enrolled services shall be running from the same thread.
 *
 * @param arg0   argument containing remote dispatch function pointers
 * @param arg1   custom initialization parameter
 */
typedef void (*ICall_RemoteTaskEntry)(const ICall_RemoteTaskArg *arg0,
                                      void *arg1);

/**
 * Call dispatcher service handler function pointer type
 * @param args    arguments
 * @return error code
 */
typedef ICall_Errno (*ICall_ServiceFunc)(ICall_FuncArgsHdr *args);

/** ICall_enrollService() arguments */
typedef struct _icall_enroll_service_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** service id of the enrolling service */
  ICall_ServiceEnum service;
  /** handler function which handles function calls to the service */
  ICall_ServiceFunc fn;
  /** field to store the assigned entity id */
  ICall_EntityID entity;
  /** field to store the synchronous object handle
   *  associated with the calling thread */
  ICall_SyncObject msgSyncObj;
} ICall_EnrollServiceArgs;

/** ICall_setTimer() arguments */
typedef struct _icall_set_timer_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** timeout period */
  uint_least32_t timeout;
  /** timer ID */
  ICall_TimerID timerid;
  /** callback function */
  ICall_TimerCback cback;
  /** pointer to an argument to be passed to the callback function */
  void *arg;
} ICall_SetTimerArgs;

/** ICall_stopTimer() arguments */
typedef struct _icall_stop_timer_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** timer ID */
  ICall_TimerID timerid;
} ICall_StopTimerArgs;

/** ICall_getLocalMsgEntityId() arguments */
typedef struct _icall_get_local_msg_entity_id_args_t
{
  /** Common arguments */
  ICall_FuncArgsHdr hdr;
  /** service id */
  ICall_ServiceEnum service;
  /** entity id */
  ICall_EntityID entity;
  /** local entity ID */
  uint_least8_t localId;
} ICall_GetLocalMsgEntityIdArgs;

/**
 * Special global variable to set the reset reason by the boot code,
 * before ICall is initialized, so that ICall can serve "get reset reason"
 * function.
 */
extern uint_least8_t ICall_resetReason;

/**
 * Initializes the dispatcher implementation.
 * Note that this shall be only called from an image which
 * included dispatcher implementation.
 */
extern void ICall_init(void);

/**
 * Creates remote tasks.
 * One remote task shall be created per external image.
 * Note that this function must be called after calling
 * ICall_init().
 *
 * The external image information must be stored either in a custom
 * ICallAddrs.h header file in an include path when
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
 * module: @ref ICall_imgEntries, @ref ICall_imgTaskPriorities,
 * @ref ICall_imgTaskStackSizes and @ref ICall_numImages.
 */
extern void ICall_createRemoteTasks(void);

/**
 * Registers an application.
 * Note that this function must be called from the thread
 * from which ICall_wait() function will be called.
 *
 * @param entity  pointer to a variable to store entity id assigned
 *                to the application.
 * @param msgsem  pointer to a variable to store the synchronous object handle
 *                associated with the calling thread.
 * @return @ref ICALL_ERRNO_SUCCESS when successful.<br>
 *         @ref ICALL_ERRNO_NO_RESOURCE when ran out of resource.
 */
static ICall_Errno ICall_registerApp(ICall_EntityID *entity,
                                            ICall_SyncObject *msgsyncobj)
{
  ICall_RegisterAppArgs args;
  ICall_Errno errno;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_REGISTER_APP;
  errno = ICall_dispatcher(&args.hdr);
  *entity = args.entity;
  *msgsyncobj = args.msgSyncObj;
  return errno;
}

/**
 * Allocates memory block for a message.
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
  errno = ICall_dispatcher(&args.hdr);
  if (errno != ICALL_ERRNO_SUCCESS)
  {
    return NULL;
  }
  return args.ptr;
}

/**
 * Frees the memory block allocated for a message.
 * @param msg   pointer to the start of the message body
 *              which was returned from ICall_allocMsg().
 */
static void ICall_freeMsg(void *msg)
{
  ICall_FreeArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_MSG_FREE;
  args.ptr = msg;
  (void) ICall_dispatcher(&args.hdr);
}

/**
 * Sends a message to a registered server.
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
 * @return @ref ICALL_ERRNO_SUCCESS when successful.<br>
 *         @ref ICALL_ERRNO_INVALID_SERVICE when the 'dest'
 *         is unregistered service.<br>
 *         @ref ICALL_ERRNO_INVALID_PARAMETER when the 'src'
 *         is an out of range entity id or when 'dest' is
 *         is a service that does not receive a message
 *         (such as ICall primitive service).<br>
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
  return ICall_dispatcher(&args.hdr);
}

/**
 * Retrieves a message received at the message queue
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
 *         and a message was retrieved.<br>
 *         @ref ICALL_ERRNO_NOMSG when there is no queued message
 *         at the moment.<br>
 *         @ref ICALL_ERRNO_CORRUPT_MSG when a message queued in
 *         front of the thread's receive queue was not sent by
 *         a server. Note that in this case, the message is
 *         not retrieved but thrown away.<br>
 *         @ref ICALL_ERRNO_UNKNOWN_THREAD when this function is
 *         called from a thread which has not registered
 *         an entity, either through ICall_enrollService()
 *         or through ICall_registerApp().
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
  errno = ICall_dispatcher(&args.hdr);
  *src = args.src.servId;
  *dest = args.dest;
  *msg = args.msg;
  return errno;
}

/**
 * Waits for a signal to the semaphore associated with the calling thread.
 *
 * Note that the semaphore associated with a thread is signaled
 * when a message is queued to the message receive queue of the thread
 * or when ICall_signal() function is called onto the semaphore.
 *
 * @param milliseconds  timeout period in milliseconds.
 * @return @ref ICALL_ERRNO_SUCCESS when the semaphore is signaled.<br>
 *         @ref ICALL_ERRNO_TIMEOUT when designated timeout period
 *         has passed since the call of the function without
 *         the semaphore being signaled.
 */
static ICall_Errno ICall_wait(uint_fast32_t milliseconds)
{
  ICall_WaitArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_WAIT;
  args.milliseconds = milliseconds;
  return ICall_dispatcher(&args.hdr);
}

/**
 * Signals a semaphore.
 * @param msgsem   handle of a synchronous object to signal
 * @return @ref ICALL_ERRNO_SUCCESS
 */
static ICall_Errno ICall_signal(ICall_SyncObject msgsyncobj)
{
  ICall_SignalArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_SIGNAL;
  args.syncObject = msgsyncobj;

  return ICall_dispatcher(&args.hdr);
}

/**
 * Registers a service entity
 * @param service  service id of the enrolling service
 * @param fn       handler function which handles function
 *                 calls to the service.
 * @param entity   pointer to a variable to store the assigned entity id
 * @param msgsem   pointer to a variable to store the synchronous object handle
 *                 associated with the calling thread.
 * @return @ref ICALL_ERRNO_SUCCESS when successful.<br>
 *         @ref ICALL_ERRNO_INVALID_PARAMETER when service id is already
 *         registered by another entity.<br>
 *         @ref ICALL_ERRNO_NO_RESOURCE when maximum number of services
 *         are already registered.
 */
static ICall_Errno
ICall_enrollService(ICall_ServiceEnum service,
                    ICall_ServiceFunc fn,
                    ICall_EntityID *entity,
                    ICall_SyncObject *msgsyncobj)
{
  ICall_EnrollServiceArgs args;
  ICall_Errno errno;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_ENROLL;
  args.service = service;
  args.fn = fn;
  errno = ICall_dispatcher(&args.hdr);
  *entity = args.entity;
  *msgsyncobj = args.msgSyncObj;
  return errno;
}

/**
 * Allocates a memory block.
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
  errno = ICall_dispatcher(&args.hdr);
  if (errno != ICALL_ERRNO_SUCCESS)
  {
    return NULL;
  }
  return args.ptr;
}

/**
 * Frees an allocated memory block.
 * @param msg  pointer to a memory block to free.
 */
static void ICall_free(void *msg)
{
  ICall_FreeArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_FREE;
  args.ptr = msg;
  (void) ICall_dispatcher(&args.hdr);
}

/**
 * Sends a message to an entity.
 * @param src     entity id of the sender
 * @param dest    entity id of the destination of the message.
 * @param format  message format. See ICall_sendServiceMsg().
 * @param msg     pointer to the message body.
 * @return @ref ICALL_ERRNO_SUCCESS when successful.<br>
 *         @ref ICALL_ERRNO_INVALID_PARAMETER when either src
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
  return ICall_dispatcher(&args.hdr);
}

/**
 * Retrieves a message, queued to receive queue of the calling thread.
 *
 * @param src   pointer to a variable to store the sender entity id
 *              of the received message.
 * @param dest  pointer to a variable to store the destination entity id
 *              of the received message.
 * @param msg   pointer to a pointer variable to store the starting
 *              address of a received message body.
 * @return @ref ICALL_ERRNO_SUCCESS when a message was successfully
 *         retrieved.<br>
 *         @ref ICALL_ERRNO_NOMSG when no message was queued to
 *         the receive queue at the moment.<br>
 *         @ref ICALL_ERRNO_UNKNOWN_THREAD when the calling thread
 *         does not have a received queue associated with it.
 *         This happens when neither ICall_enrollService() nor
 *         ICall_registerApp() was ever called from the calling
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
  errno = ICall_dispatcher(&args.hdr);
  *src = args.src.entityId;
  *dest = args.dest;
  *msg = args.msg;
  return errno;
}

/**
 * Transforms and entityId into a serviceId.
 * Note that this function is useful in case an application
 * waits for messages from both a server and another application,
 * in which case, the application can only use ICall_fetchMsg(),
 * not ICall_fetchServiceMsg() because the latter will return
 * @ref ICALL_ERRNO_CORRUPT_MSG when a message sent by the other
 * application is about to be fetched.<br>
 * This function, then, is useful to retrieve service id
 * matching the source entity id in case the source entity
 * id is not that of the other application.
 *
 * @param entityId   entity id
 * @param servId     pointer to a variable to store
 *                   the resultant service id
 * @return @ref ICALL_ERRNO_SUCCESS if the transformation was successful.<br>
 *         @ref ICALL_ERRNO_INVALID_SERVICE if no matching service
 *         is found for the entity id.
 */
#ifdef ICALL_FULL_API
static ICall_Errno ICall_entityId2ServiceId(ICall_EntityID entityId,
                                                   ICall_ServiceEnum *servId)
{
  ICall_EntityId2ServiceIdArgs args;
  ICall_Errno errno;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_ENTITY2SERVICE;
  args.entityId = entityId;
  errno = ICall_dispatcher(&args.hdr);
  *servId = args.servId;
  return errno;
}
#endif
/**
 * Aborts.
 *
 * This is preferred over C runtime abort() function,
 * in an external image since the C runtime abort() is only
 * guaranteed in a root image which contains the C runtime
 * entry function that is executed upon startup.
 */
static ICall_Errno
ICall_abort(void)
{
  ICall_FuncArgsHdr args;
  args.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.func = ICALL_PRIMITIVE_FUNC_ABORT;
  return ICall_dispatcher(&args);
}

/**
 * Enables interrupt.
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
  return ICall_dispatcher(&args.hdr);
}

/**
 * Disables interrupt
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
  return ICall_dispatcher(&args.hdr);
}

/**
 * Enables master interrupt and context switching.
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
 * Disables master interrupt and context switching.
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
 * Registers an interrupt service routine
 * @param intnum   interrupt number
 * @param isrfunc  pointer to the interrupt service function
 * @return @ref ICALL_ERRNO_SUCCESS when successful.<br>
 *         @ref ICALL_ERRNO_NO_RESOURCE when the registration
 *         failed due to lack of resources.
 */
#ifdef ICALL_FULL_API
static ICall_Errno
ICall_registerISR(int intnum, void (*isrfunc)(void))
{
  ICall_RegisterISRArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_REGISTER_ISR;
  args.intnum = intnum;
  args.isrfunc = isrfunc;
  return ICall_dispatcher(&args.hdr);
}
#endif
static ICall_Errno
ICall_registerISR_Ext(int intnum, void (*isrfunc)(void), int intPriority )
{
  ICall_RegisterISRArgs_Ext args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func    = ICALL_PRIMITIVE_FUNC_REGISTER_ISR_EXT;
  args.intnum      = intnum;
  args.isrfunc     = isrfunc;
  args.intPriority = intPriority;
  return ICall_dispatcher(&args.hdr);
}

/**
 * Gets the current tick counter value.
 * @return current tick counter value
 */
static uint_fast32_t
ICall_getTicks(void)
{
  ICall_GetUint32Args args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_GET_TICKS;
  (void) ICall_dispatcher(&args.hdr);
  return args.value;
}

/**
 * Gets the tick period.
 * @return tick period in microseconds.
 */
static uint_fast32_t
ICall_getTickPeriod(void)
{
  ICall_GetUint32Args args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_GET_TICK_PERIOD;
  (void) ICall_dispatcher(&args.hdr);
  return args.value;
}

/**
 * Gets the maximum timeout period supported by
 * ICall_setTimerMSecs() function.
 *
 * @return maximum timeout period in milliseconds
 */
static uint_fast32_t
ICall_getMaxMSecs(void)
{
  ICall_GetUint32Args args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_GET_MAX_MILLISECONDS;
  (void) ICall_dispatcher(&args.hdr);
  return args.value;
}

/**
 * Set up or restart a timer.
 * Note that the timer setup initially by this function may be restarted
 * using ICall_setTimer() function as well.
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
 * @return @ref ICALL_ERRNO_SUCCESS when successful;<br>
 *         @ref ICALL_ERRNO_INVALID_PARAMETER when msecs is greater than
 *              maximum value supported;<br>
 *         @ref ICALL_ERRNO_NO_RESOURCE when ran out of resource.
 *         Check ICall heap size and OS heap size if this happens.
 *
 * @see ICall_getMaxMSecs()
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
  errno = ICall_dispatcher(&args.hdr);
  *id = args.timerid;

  return errno;
}

/**
 * Set up or restart a timer.
 * Note that the timer setup initially by this function may be restarted
 * using ICall_setTimerMSecs() function as well.
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
 * @return @ref ICALL_ERRNO_SUCCESS when successful;<br>
 *         @ref ICALL_ERRNO_NO_RESOURCE when ran out of resource.
 *         Check ICall heap size and OS heap size if this happens.
 *
 * @see ICall_getTickPeriod()
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
  errno = ICall_dispatcher(&args.hdr);
  *id = args.timerid;

  return errno;
}

/**
 * Stops a timer.
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
  (void) ICall_dispatcher(&args.hdr);
}

/**
 * Increments or decrements power activity counter.
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
 * @return @ref ICALL_ERRNO_SUCCESS when successful<br>
 *         @ref ICALL_ERRNO_OVERFLOW when the counter overflowed<br>
 *         @ref ICALL_ERRNO_UNDERFLOW when the counter underflowed.
 */
static ICall_Errno
ICall_pwrUpdActivityCounter(bool incFlag)
{
  ICall_PwrUpdActivityCounterArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_PWR_UPD_ACTIVITY_COUNTER;
  args.incFlag = incFlag;
  return ICall_dispatcher(&args.hdr);
}

/**
 * Configures power constraint/dependency set/release actions upon
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
 * @return @ref ICALL_ERRNO_SUCCESS when successful<br>
 *         @ref ICALL_ERRNO_INVALID_PARAMETER when an invalid
 *              flag in the bitmap is detected.<br>
 */
#ifdef ICALL_FULL_API
static ICall_Errno
ICall_pwrConfigACAction(ICall_PwrBitmap_t bitmap)
{
  ICall_PwrBitmapArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_PWR_CONFIG_AC_ACTION;
  args.bitmap = bitmap;
  return ICall_dispatcher(&args.hdr);
}
#endif
/**
 * Sets power constraints and dependencies.
 *
 * @param bitmap  a bitmap of constraint or dependency flags.<br>
 *                The flag definition is specific to each platform.
 *                For instance, see ICallCC26xxDefs.h.
 *
 * @return @ref ICALL_ERRNO_SUCCESS when successful<br>
 *         @ref ICALL_ERRNO_INVALID_PARAMETER when an invalid
 *              flag in the bitmap is detected.<br>
 */
static ICall_Errno
ICall_pwrRequire(ICall_PwrBitmap_t bitmap)
{
  ICall_PwrBitmapArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_PWR_REQUIRE;
  args.bitmap = bitmap;
  return ICall_dispatcher(&args.hdr);
}

/**
 * Releases power constraints and dependencies.
 *
 * @param bitmap  a bitmap of constraint or dependency flags.<br>
 *                The flag definition is specific to each platform.
 *                For instance, see ICallCC26xxDefs.h.
 *
 * @return @ref ICALL_ERRNO_SUCCESS when successful<br>
 *         @ref ICALL_ERRNO_INVALID_PARAMETER when an invalid
 *              flag in the bitmap is detected.<br>
 */
static ICall_Errno
ICall_pwrDispense(ICall_PwrBitmap_t bitmap)
{
  ICall_PwrBitmapArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_PWR_DISPENSE;
  args.bitmap = bitmap;
  return ICall_dispatcher(&args.hdr);
}

/**
 * Checks whether HF XOSC is stable.
 * This function must be called after HF XOSC is turned on
 * (through power dependency).
 *
 * @return TRUE when HF XOSC is stable.<br>
 *         FALSE when HF XOSC is not stable.<br>
 */
static bool
ICall_pwrIsStableXOSCHF(void)
{
  ICall_GetBoolArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_PWR_IS_STABLE_XOSC_HF;
  (void) ICall_dispatcher(&args.hdr);
  return args.value;
}

/**
 * Switch clock source to HF XOSC.
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
 * Gets the estimated crystal oscillator startup time.
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
  (void) ICall_dispatcher(&args.hdr);
  return args.value;
}

/**
 * Registers a power state transition notify function.
 *
 * The registered notify function shall be called when the power state
 * changes.
 *
 * @param fn  notify function pointer
 * @param obj pointer to data object to be passed to notify function.<br>
 *            This pointer must not be NULL and can point to an aggregate type
 *            containing the @ref ICall_PwrNotifyData.
 *            The object must not be released.
 * @return @ref ICALL_ERRNO_SUCCESS when successful<br>
 *         @ref ICALL_ERRNO_NO_RESOURCE when registration failed<br>
 */
static ICall_Errno
ICall_pwrRegisterNotify(ICall_PwrNotifyFn fn, ICall_PwrNotifyData *obj)
{
  ICall_PwrRegisterNotifyArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_PWR_REGISTER_NOTIFY;
  args.fn = fn;
  args.obj = obj;
  return ICall_dispatcher(&args.hdr);
}

/**
 * Retrieves power transition state.
 *
 * @return Implementation specific transition state when successful<br>
 *         Zero when the function is not implemented.<br>
 */
static uint_fast8_t
ICall_pwrGetTransitionState(void)
{
  ICall_PwrGetTransitionStateArgs args;
  ICall_Errno errno;

  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_PWR_GET_TRANSITION_STATE;
  errno = ICall_dispatcher(&args.hdr);

  if (errno == ICALL_ERRNO_SUCCESS)
  {
    return args.state;
  }
  return 0;
}

/**
 * Creates a new RTOS task.
 *
 * @param entryfn   task entry function.
 * @param priority  task priority as understood by the underlying RTOS
 * @param stacksize stack size as understood by the underlying RTOS
 * @param arg       argument to pass to the task entry function
 *
 * @return @ref ICALL_ERRNO_SUCCESS when successful<br>
 *         @ref ICALL_ERRNO_NO_RESOURCE when creation failed<br>
 */
#ifdef ICALL_FULL_API
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
  return ICall_dispatcher(&args.hdr);
}
#endif

#ifdef ICALL_RTOS_SEMAPHORE_API
/**
 * Creates a semaphore.
 *
 * @param mode  Semaphore mode<br>
 *              @ref ICALL_SEMAPHORE_MODE_COUNTING, or<br>
 *              @ref ICALL_SEMAPHORE_MODE_BINARY
 * @param initcount initial count value
 * @return created semaphore when successful<br>
 *         NULL when creation failed<br>
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
  errno = ICall_dispatcher(&args.hdr);
  if (errno == ICALL_ERRNO_SUCCESS)
  {
    return args.sem;
  }
  return NULL;
}
#endif

#ifdef ICALL_RTOS_SEMAPHORE_API
/**
 * Post on a semaphore
 *
 * @param sem           semaphore.
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
  return ICall_dispatcher(&args.hdr);
}
#endif /* ICALL_RTOS_SEMAPHORE_API */

#ifdef ICALL_RTOS_EVENTS_API
/**
 * Creates a event.
 *
 * @return created event when successful<br>
 *         NULL when creation failed<br>
 */
static ICall_Event
ICall_createEvent(void)
{
  ICall_CreateEventArgs args;
  ICall_Errno errno;

  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_CREATE_EVENT;
  errno = ICall_dispatcher(&args.hdr);
  if (errno == ICALL_ERRNO_SUCCESS)
  {
    return args.event;
  }
  return NULL;
}

/**
 * Post on an event
 *
 * @param event           event.
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
  return ICall_dispatcher(&args.hdr);
}
/**
 * Waits on a event for event 31
 *
 * @param event           event.
 * @param milliseconds  timeout in milliseconds
 *                      or @ref ICALL_TIMEOUT_FOREVER to wait forever
 *
 * @return @ref ICALL_ERRNO_SUCCESS when the operation was successful
 *         and event was received.<br>
 *         @ref ICALL_ERRNO_TIMEOUT when designated timeout period
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
  return ICall_dispatcher(&args.hdr);
}
#endif /* ICALL_RTOS_EVENTS_API */

#ifdef ICALL_RTOS_SEMAPHORE_API
/**
 * Waits on a semaphore
 *
 * @param sem           semaphore.
 * @param milliseconds  timeout in milliseconds
 *                      or @ref ICALL_TIMEOUT_FOREVER to wait forever
 *
 * @return @ref ICALL_ERRNO_SUCCESS when the operation was successful
 *         and semaphore was acquired.<br>
 *         @ref ICALL_ERRNO_TIMEOUT when designated timeout period
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
  return ICall_dispatcher(&args.hdr);
}
#endif /* ICALL_RTOS_SEMAPHORE_API */

/**
 * Waits for and retrieves a message received at the message queue
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
 *         and a message was retrieved.<br>
 *         @ref ICALL_ERRNO_TIMEOUT when designated timeout period
 *         has passed since the call of the function without
 *         the semaphore being signaled.
 *         @ref ICALL_ERRNO_UNKNOWN_THREAD when this function is
 *         called from a thread which has not registered
 *         an entity, either through ICall_enrollService()
 *         or through ICall_registerApp().
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
  errno = ICall_dispatcher(&args.hdr);
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
 * Retrieves an entity ID of (arbitrary) one of the entities registered
 * from the calling thread.
 *
 * Note that, if multiple entities were registered from the same thread,
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
  errno = ICall_dispatcher(&args.hdr);
  if (errno == ICALL_ERRNO_SUCCESS)
  {
    return args.entity;
  }
  return ICALL_INVALID_ENTITY_ID;
}

/**
 * Checks whether the calling thread provides the designated service.
 *
 * @param  service    Service enumeration value for the service.
 *
 * @return Non-zero if the current thread provides the designated service.
 *         Zero, otherwise.
 */
#ifdef ICALL_FULL_API
static uint_fast8_t
ICall_threadServes(ICall_ServiceEnum service)
{
  ICall_ThreadServesArgs args;
  ICall_Errno errno;

  args.hdr.service = ICALL_SERVICE_CLASS_PRIMITIVE;
  args.hdr.func = ICALL_PRIMITIVE_FUNC_THREAD_SERVES;
  args.servId = service;
  errno = ICall_dispatcher(&args.hdr);
  if (errno == ICALL_ERRNO_SUCCESS)
  {
    return args.result;
  }
  return 0;
}
#endif

/**
 * Retrieves an internal ID understood only by a service entity,
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
  errno = ICall_dispatcher(&args.hdr);
  if (errno == ICALL_ERRNO_SUCCESS)
  {
    return args.localId;
  }
  return 0xFF;
}


#ifdef __cplusplus
}
#endif

#endif /* ICALL_H */
