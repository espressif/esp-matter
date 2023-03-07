/*
 * Copyright (c) 2008-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 * gpHal_ES.h
 *
 *  This file defines all functions for the event scheduler and sleep modes.  These functions can be used to schedule certain actions : an interrupt, TX of a packet, etc.
 *  The diffent sleep and wakeup modes can also be initialized and used with these functions.
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
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _HAL_GP_ES_H_
#define _HAL_GP_ES_H_

/** @file gpHal_ES.h
 *  This file defines all functions for the event scheduler and sleep modes.  These functions can be used to schedule certain actions : an interrupt, TX of a packet, etc.
 *  The different sleep and wakeup modes can also be initialized and used with these functions.
 *
 *  @brief All functions for the event scheduler and sleep modes.
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gp_global.h"
#include "gpHal_reg.h"
#include "gpHal_HW.h"


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

//Execution options
/** @brief Event exection options mask. */
#define GP_ES_EXECUTION_OPTIONS_MASK                      0xF
/** @brief Event exection options : Execute the event even if the trigger time has passed. */
#define GP_ES_EXECUTION_OPTIONS_EXECUTE_IF_TOO_LATE       0x4
/** @brief Event exection options : Prohibit standby as long as event is pending */
#define GP_ES_EXECUTION_OPTIONS_PROHIBIT_STANDBY          0x8
/** @brief Event exection options : Execute the event only if the event was triggered on time. */
#define GP_ES_EXECUTION_OPTIONS_NOT_EXECUTE_IF_TOO_LATE   0x0

//Interrupt options
/** @brief Event interrupt options mask (enables all interrupt options).*/
#define GP_ES_INTERRUPT_OPTIONS_MASK                      0x3F
/** @brief Event interrupt option: generate interrupt on first event execution that is on time.*/
#define GP_ES_INTERRUPT_OPTIONS_ON_FIRST_ON_TIME          0x01 //First bit
/** @brief Event interrupt option: generate interrupt on event execution other then first or last execution that is on time.*/
#define GP_ES_INTERRUPT_OPTIONS_ON_OTHERS_ON_TIME         0x02 //2nd bit
/** @brief Event interrupt option: generate interrupt on last event execution that is on time.*/
#define GP_ES_INTERRUPT_OPTIONS_ON_LAST_ON_TIME           0x04 //3rd bit
/** @brief Event interrupt option: generate interrupt on first event execution that is too late.*/
#define GP_ES_INTERRUPT_OPTIONS_ON_FIRST_TOO_LATE         0x08 //4th bit
/** @brief Event interrupt option: generate interrupt on execution other then first or last execution that is too late.*/
#define GP_ES_INTERRUPT_OPTIONS_ON_OTHERS_TOO_LATE        0x10 //5th bit
/** @brief Event interrupt option: generate interrupt on last event execution that is too late.*/
#define GP_ES_INTERRUPT_OPTIONS_ON_LAST_TOO_LATE          0x20 //6th bit

/** @brief Default value for the calibration of the 32kHz crystal */
#define GPHAL_ES_32KHZ_SLEEP_DEFAULT_CALIB                0x3D090000 // 32,768kHz
/** @brief Default value for the calibration of the 16MHz crystal */
#define GPHAL_ES_16MHZ_SLEEP_DEFAULT_CALIB                0x40000000

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

//Result and state fields in the event control field
/** @brief Get the event result (top 4 bits of the control field). */
#define GP_ES_GET_EVENT_RESULT(control)               (((control)>>4) & 0xF)
/** @brief Get the event state (last 4 bits of the control field). */
#define GP_ES_GET_EVENT_STATE(control)                ( (control)     & 0xF)
/** @brief Set the event result (top 4 bits of the control field). */
#define GP_ES_SET_EVENT_RESULT(control,result)         (control = (control & 0x0F) | (((result)<<4) & 0xF0))
/** @brief Set the event state (last 4 bits of the control field). */
#define GP_ES_SET_EVENT_STATE(control,state)         (control = (control & 0xF0) | ( (state)    & 0x0F))


#define GPHAL_ES_ABSOLUTE_EVENT_ID_INVALID              0xFF

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/


/** @name gpHal_EventType_t */
//@{
/** @typedef gpHal_EventType_t
 *  @brief The gpHal_EventType_t type defines the event type.
*/
typedef UInt8 gpHal_EventType_t;
//@}
//
/** @name gpHal_AbsoluteEventId_t */
//@{
/** @typedef gpHal_AbsoluteEventId_t
 *  @brief The gpHal_AbsoluteEventId_t type holds an absolue event index
*/
typedef UInt8 gpHal_AbsoluteEventId_t;
//@}


//Standby modes
/** @name gpHal_SleepMode_t */
//@{
/** @typedef gpHal_SleepMode_t
 *  @brief The gpHal_SleepMode_t type defines the GP chip sleep mode.
*/
typedef UInt8 gpHal_SleepMode_t;
//@}

// The status of a sleep mode (determined by oscillator benchmark measurements, by comparision against 32 MHz clock)
#define gpHal_SleepClockMeasurementStatusNotStarted     0x00
#define gpHal_SleepClockMeasurementStatusPending        0x01
#define gpHal_SleepClockMeasurementStatusNotStable      0x02
#define gpHal_SleepClockMeasurementStatusStable         0x03

typedef UInt8 gpHal_SleepClockMeasurementStatus_t;

//Event states enumeration
/** @name gpHal_EventState_t */
//@{
/** @brief The event is invalid, will not be executed if execution time is reached.*/
#define gpHal_EventStateInvalid                 GPHAL_ENUM_EVENT_STATE_INVALID
/** @brief The event is scheduled, it will be executed if execution time is reached.*/
#define gpHal_EventStateScheduled               GPHAL_ENUM_EVENT_STATE_SCHEDULED
/** @brief The event is scheduled for immediate, it will be executed as soon as possible.*/
#define gpHal_EventStateScheduledForImmediate   GPHAL_ENUM_EVENT_STATE_SCHEDULED_FOR_IMMEDIATE_EXECUTION
/** @brief The event is rescheduled after being triggered before. It will be executed if execution time is reached.*/
#define gpHal_EventStateReScheduled             GPHAL_ENUM_EVENT_STATE_RESCHEDULED
/** @brief The event has been executed.*/
#define gpHal_EventStateDone                    GPHAL_ENUM_EVENT_STATE_DONE
/** @typedef gpHal_EventState_t
 *  @brief The gpHal_EventState_t type defines the Absolute Event state.
*/
typedef UInt8 gpHal_EventState_t;
//@}

/** @name gpHal_EventResult_t */
//@{
/** @brief Event not yet executed. */
#define gpHal_EventResultInvalid    GPHAL_ENUM_EVENT_RESULT_UNKNOWN
/** @brief Event was executed on time. */
#define gpHal_EventResultOnTime     GPHAL_ENUM_EVENT_RESULT_EXECUTED_ON_TIME
/** @brief Event was executed too late and GP_ES_EXECUTION_OPTIONS_EXECUTE_IF_TOO_LATE was set */
#define gpHal_EventResultTooLate    GPHAL_ENUM_EVENT_RESULT_EXECUTED_TOO_LATE
/** @brief Event was executed too late and GP_ES_EXECUTION_OPTIONS_NOT_EXECUTE_IF_TOO_LATE was set */
#define gpHal_EventResultMissed     GPHAL_ENUM_EVENT_RESULT_MISSED_TOO_LATE
/** @typedef gpHal_EventResult_t
 *  @brief The gpHal_EventResult_t type defines the event result.
*/
typedef UInt8 gpHal_EventResult_t;
//@}

//Absolute event descriptor
/** @struct gpHal_AbsoluteEventDescriptor
 *  @brief The gpHal_AbsoluteEventDescriptor structure specifying the parameters of an Absolute Event.
 *  @typedef gpHal_AbsoluteEventDescriptor_t
 *  @brief The gpHal_AbsoluteEventDescriptor_t type definition based on the structure gpHal_AbsoluteEventDescriptor.
*/
typedef struct gpHal_AbsoluteEventDescriptor
{
/** This field contains the absolute time (absolute to the symbol counter of the GP chip) at which the event should be executed. */
    UInt32 exTime;
/** This field contains the period between different periodic executions of this event. This value will only be used by the GP chip when recAmount > 0. */
    UInt32 recPeriod;                 //Recurrence Period
/** This field contains the amount of recurrences of the event. If set to 0 = 1 execution, set to 1 = 2 executions. Value 0xFFFF indicates an endless recurrent event. */
    UInt16 recAmount;                 //Recurrence Amount
/** This field contains Custom Data that can be associated with the event. This data can be read when using the gpHal_MonitorAbsoluteEvent() function.*/
    UInt16 customData;                //custom uP data
/** This field contains the bitfield specifying the execution of the event (see GP_ES_EXECUTION_OPTIONS_MASK).*/
    UInt8  executionOptions;
/** This field contains the bitfield specifying the interrupts given by the event (see GP_ES_INTERRUPT_OPTIONS_MASK).*/
    UInt8  interruptOptions;          //interrupt on_first/others/last_on_time
/** This field contains the Event state (see enum gpHal_EventState_t) and the Event result
  * (see enum gpHal_EventResult_t).  The macro GP_ES_SET_EVENT_STATE() (resp. GP_ES_SET_EVENT_RESULT())
  * should be used in order to extract the information and the macro GP_ES_SET_EVENT_STATE() (resp. GP_ES_SET_EVENT_RESULT()) in order to initialize this field.
  *
  * Preferably the result field is written to INVALID at schedule time.  After the event was triggered,
  * this field will return information about the execution status. */
    UInt8  control;      //Control
/** This field contains the Event type (see enum gpHal_EventType_t). This specifies what action needs to be performed on execution of the event. */
    UInt8  type;         //Type
} gpHal_AbsoluteEventDescriptor_t;

/** @struct gpHal_ExternalEventDescriptor
 *  @brief The gpHal_ExternalEventDescriptor structure specifying the External Event.
 *  @typedef gpHal_ExternalEventDescriptor_t
 *  @brief The gpHal_ExternalEventDescriptor_t type definition based on the structure gpHal_ExternalEventDescriptor.
*/
typedef struct gpHal_ExternalEventDescriptor
{
/** This field contains the Event type (see enum gpHal_EventType_t). This specifies what action needs to be performed on execution of the event. */
    gpHal_EventType_t type;
} gpHal_ExternalEventDescriptor_t;



/*****************************************************************************
 *                    Public function prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
//ES Enable/Disable callbacks

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpHal_CodeJumpTableFlash_Defs_ES.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */

/**
 * @brief This function enables the interrupt line of the External Event interrupt.
 *
 * This function enables the interrupt line of the External Event interrupt by setting the interrupt mask of the External Event interrupt.
 *
 * @param enable Enables the interrupt source if true.
*/
#define gpHal_EnableExternalEventCallbackInterrupt(enable)     GP_HAL_ENABLE_EXTERNAL_EVENT_INTERRUPT(enable)

/**
 * @brief This function enables the interrupt line of an Absolute Event interrupt.
 *
 * This function enables the interrupt line of an Absolute Event interrupt by setting the interrupt mask of the Absolute Event with index eventNbr.
 *
 * @param eventNbr The index of the Absolute Event (1..16).
 * @param enable   Enables the interrupt source if true.
*/
GP_API void gpHal_EnableAbsoluteEventCallbackInterrupt(UInt8 eventNbr, Bool enable);

//ES register callbacks
/**
 * @brief Registers the callback for an Absolute Event.
 *
 * This function registers the callback for an Absolute Event. The callback will be executed when the Absolute Event is triggered.
 * The Absolute Event with the correct index needs to be enabled.
 *
 * @param callback The pointer to the callback function.
 * @param eventNbr The index of the Absolute Event (1..16).
*/
GP_API void gpHal_RegisterAbsoluteEventCallback(gpHal_AbsoluteEventCallback_t callback, UInt8 eventNbr);

/**
 * @brief Registers the callback for a External Event.
 *
 * This function registers the callback for a External Event. It returns the callback that was
 * registered earlier or NULL if none was registered. Multiple External Event handlers can be
 * threaded by calling previously registered handler from the new handler.
 *
 * The callback will be executed when the External Event is triggered.
 * The External Event interrupt must be enabled.
 *
 * @param callback The pointer to the callback function.
*/
GP_API gpHal_ExternalEventCallback_t gpHal_RegisterExternalEventCallback(gpHal_ExternalEventCallback_t callback);

// ES functions

/** @brief Resets the timebase of the GP chip */
GP_API void gpHal_ResetTime(void);

/**
 * @brief Gets the time of the GP chip.
 *
 * This function returns the current time of the GP chip in us.
 *
 * @param pTime Pointer to the variable where the time will be stored.
*/
GP_API void gpHal_GetTime(UInt32* pTime);

/**
 * @brief Calibrates the timer of the GP chip.
 *
 * This function calibrates the GP chip time base by applying a correction of the current time and an adjustment of the timer slope.
 *
 * @param phaseAdjustment The phase adjustment to be applied to the GP chip timer.
 * @param frequency       The desired frequency of the timer/slope of the counter.
*/
GP_API void gpHal_ApplyCalibration( Int32 phaseAdjustment, UInt32 frequency);

/**
 * @brief Schedules an Absolute Event in the GP chip.
 *
 * This function uploads and activates an event in the GP chip Event Scheduler.  To facilitate a callback on the execution of the event
 * one must register the callback using gpHal_RegisterAbsoluteEventCallback() and enable the interrupt using gpHal_EnableAbsoluteEventCallbackInterrupt().
 *
 * @param pAbsoluteEventDescriptor Pointer to the AbsoluteEventDescriptor_t structure containing the Event options.
 * @param eventNbr                 The index of the Absolute Event (1..16).
*/
GP_API void gpHal_ScheduleAbsoluteEvent(gpHal_AbsoluteEventDescriptor_t* pAbsoluteEventDescriptor, gpHal_AbsoluteEventId_t eventNbr);

/**
 * @brief Allocates an available absolute event id
 *
*/
GP_API gpHal_AbsoluteEventId_t gpHal_GetAbsoluteEvent(void);
/**
 * @brief Frees an allocated absolute event id
 *
*/
GP_API void gpHal_FreeAbsoluteEvent(gpHal_AbsoluteEventId_t EventId);

/**
 * @brief Refreshes an Absolute Event in the GP chip.
 *
 * This function refreshes an already prepared Absolute Event. The event descriptor gpHal_AbsoluteEventDescriptor_t needs to be written as part of the preparation.
 * Writing the gpHal_AbsoluteEventDescriptor_t can be done with GP_ES_WRITE_EVENT_DESCRIPTOR().
 *
 * @param eventNbr                 The index of the Absolute Event (1..16).
 * @param absTime                  The absolute execution time of the event (in us)
 * @param control                  The control field of the event descriptor (see gpHal_AbsoluteEventDescriptor_t).
 *
*/
GP_API void gpHal_RefreshAbsoluteEvent(gpHal_AbsoluteEventId_t eventNbr, UInt32 absTime, UInt8 control);

/**
 * @brief Unschedules an Absolute Event.
 *
 * This function disables the Absolute Event in the GP chip and returns the current EventState.
 * Possible Event States are enumerated in the enumeration gpHal_EventState.
 *
 * @param eventNbr The index of the Absolute Event (1..16).
*/
GP_API gpHal_EventState_t gpHal_UnscheduleAbsoluteEvent(gpHal_AbsoluteEventId_t eventNbr);

/**
 * @brief Returns all information about a registered event.
 *
 * This function returns the AbsoluteEventDescriptor_t structure of an Absolute Event.
 *
 * This function needs to be used carefully, because it temporarilly disables the event and the event could be missed.
 *
 * Possible results are:
 *          - gpHal_ResultSuccess
 *          - gpHal_ResultInvalidHandle      (no Absolute Event registered at given index)
 *
 * @param eventNbr       The index of the Absolute Event (1..16).
 * @param pAbsoluteEvent The pointer where the AbsoluteEventDescriptor_t structure is returned.
*/
GP_API gpHal_Result_t gpHal_MonitorAbsoluteEvent(UInt8 eventNbr, gpHal_AbsoluteEventDescriptor_t* pAbsoluteEvent);

/**
 * @brief Schedules an immediate event trigger in the GP chip.
 *
 * This function uploads and activates the Relative Event in the GP chip Event Scheduler immediately.
 *
 * @param type  Type of event to execute without delay
*/
GP_API void gpHal_ScheduleImmediateEvent(gpHal_EventType_t type);

/**
 * @brief Schedules the External Event in the GP chip
 *
 * This function uploads and activates the External Event in the GP chip Event Scheduler.  To facilitate a callback on the execution of the event
 * the callback must be registered using gpHal_RegisterExternalEventCallback() and the interrupt enabled using gpHal_EnableExternalEventCallbackInterrupt().
 *
 * @param pExternalEventDescriptor Pointer to the gpHal_ExternalEventDescriptor_t structure containing the Event options.
*/
GP_API void gpHal_ScheduleExternalEvent(gpHal_ExternalEventDescriptor_t* pExternalEventDescriptor);

/**
 * @brief Unschedules the External Event.
 *
 * This function disables the External Event in the GP chip.
 *
 *  Possible results are:
 *          - gpHal_ResultSuccess
*/
GP_API gpHal_Result_t gpHal_UnscheduleExternalEvent(void);

/**
 * @brief Returns all information about the External Event.
 *
 * This funcion returns the gpHal_ExternalEventDescriptor_t structure of the External Event.
 * The contents of the structure are only valid in case the function returns gpHal_ResultSuccess.
 *
 *  Possible results are:
 *          - gpHal_ResultSuccess            (valid External Event found)
 *          - gpHal_ResultInvalidRequest     (no valid External Event present)
 *
 * @param pExternalEventDescriptor The pointer where the gpHal_ExternalEventDescriptor_t structure is returned.
*/
GP_API gpHal_Result_t gpHal_MonitorExternalEvent(gpHal_ExternalEventDescriptor_t* pExternalEventDescriptor);

// Sleep functions
/**
 * @brief Sets the sleep mode of the GP chip.
 *
 * This function sets the sleep mode of the GP chip. As enumerated under the enumeration gpHal_SleepMode the GP chip can be put into 4 different sleep modes.
 * The desired setting can be made using this function.
 *
 * @param mode The sleepmode enumerated in gpHal_SleepMode.
 * @return gpHal_ResultSuccess only if the operation was successful
*/
GP_API gpHal_Result_t gpHal_SetSleepMode(gpHal_SleepMode_t mode);

/**
 * @brief Gets the sleep mode of the GP chip.
 *
 * This function returns which sleep mode is currently set. The return mode is returned as enumerated under the enumeration gpHal_SleepMode.
*/
GP_API gpHal_SleepMode_t gpHal_GetSleepMode(void);
/**
 * @brief Gets the actual (measured) deviation of a sleep clock with respect to the 32 MHz clock
 *
 * This function returns whether the measurements for the requested sleep mode have been performed and what the
 * measured frequency is.
*/
GP_API gpHal_SleepClockMeasurementStatus_t gpHal_GetMeasuredSleepClockFrequency(gpHal_SleepMode_t mode, UInt32* frequencymHz);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

/**
 * @brief Returns the average sleep clock accuracy of the currently selected sleep clock
*/
GP_API UInt16 gpHal_GetSleepClockAccuracy(void);

/**
 * @brief Get the next average sleep clock accuracy for the given sleep mode.
 * @param nextSleepMode The sleepmode enumerated in gpHal_SleepMode.
 * @return Returns the next average sleep clock accuracy in ppm.
*/
GP_API UInt16 gpHal_GetAverageSleepClockAccuracy(gpHal_SleepMode_t nextSleepMode);

/**
 * @brief Returns the worst-case sleep clock accuracy of the currently selected sleep clock
*/
GP_API UInt16 gpHal_GetWorstSleepClockAccuracy(void);

#ifdef __cplusplus
}
#endif

#if   defined(GP_DIVERSITY_GPHAL_K8E) 
# include "gpHal_kx_ES.h"
#endif

#endif  /* _HAL_GP_ES_H_ */

