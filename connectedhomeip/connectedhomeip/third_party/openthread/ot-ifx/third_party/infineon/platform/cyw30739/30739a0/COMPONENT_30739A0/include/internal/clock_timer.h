/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */
#ifndef CLOCK_TIMER_H
#define CLOCK_TIMER_H

//==================================================================================================
// Types and constants
//==================================================================================================

#ifndef BOOL32
//! Boolean type in its most efficient form, for use in function arguments and return values.
typedef unsigned int BOOL32;
#endif // BOOL32

//! Identifier for a particular branch of the clock tree.
typedef uint8_t clock_id_t;

//! Reserved clock mode to represent an uninitialized state.
#define CLOCK_MODE_UNKNOWN      0

//! Reserved clock mode to represent a disabled state.  If a given branch of the clock tree does
//! not support disabling it, then the code which manages that branch should treat this value as
//! being synonymous with the lowest power consuming mode of the actual options.
#define CLOCK_MODE_DISABLED     1

#define TX_THREAD_TIMER_TICK_10MS           10000

//! Timer call back prototype definition.
struct NOTIFY_TIME;
typedef void (*NotifyTimeCallback)(struct NOTIFY_TIME* which_notify);

/*******************************************Timer functions*********************************************/
//! Structure used to register for (and receive) notification when a specific time arrives.  The
//! target time value is based on the timebase defined by clock_SystemTimeMicroseconds64.
/*******************************************Timer functions*********************************************/
#define OSAPI_TIMER_PERIODIC        1
#define OSAPI_TIMER_WAKEUP          2
#define OSAPI_TIMER_ACTIVE          4
#define OSAPI_TIMER_PRESERVE_CB     8
//! Timer structure
typedef struct OSAPI_TIMER_STRUCT
{
    struct OSAPI_TIMER_STRUCT *  next;  /* Next timer in list */
    // overTimeInUs tells the user how long in micorsecond has passed beyond the targeted time
    void          (*callback)(int32_t registrantData, uint32_t overTimeInUs);   /* timer expiration callback function */
    uint32_t      flags;                /* Flags for timer*/
    uint32_t      interval;             /* Periodical time out inteval, in 1 us unit */
    int32_t       arg;                  /* parameter for expiration function */
    uint64_t      target_time;          /* Target time for timer expire, in us */
} OSAPI_TIMER;


#ifdef __cplusplus
extern "C" {
#endif


//! Create a periodic timer.
//! Interval is in 1 us unit.
extern void osapi_createPeriodicTimer(OSAPI_TIMER *timer,
                  void (*timerCallback)(int32_t arg, uint32_t overTimeInUs),
                  int32_t  arg,
                  uint32_t interval);

//! Create a timer.
void osapi_createTimer(OSAPI_TIMER *timer, void (*timerCallback)(int32_t arg, uint32_t overTimeInUs), int32_t arg);

//! Activates an OSAPI timer.
//! If this timer can be a wakeup source, we should call
//! osapi_setTimerWakeupSource after osapi_createTimer and before
//! osapi_activateTimer. Interval here is in micro seconds.
extern void osapi_activateTimer(OSAPI_TIMER* timer, uint32_t interval);


//! Reports if timer is running/pending or not.
#define osapi_is_timer_running(timer) ((timer)->flags&OSAPI_TIMER_ACTIVE)

//! Marks the timer as being a wake up source.
//! This may be called before the call to osapi_activateTimer() to mark
//! the timer as a wake up source.
extern void osapi_setTimerWakeupSource( OSAPI_TIMER* timer, uint32_t can_be_wakeup_source);

//! Remove the timer from timer list.
//! It will return 0 if successful
//! It will return 1 if timeout callback already called.
//! It will return 2 otherwise.
extern uint32_t osapi_deactivateTimer(OSAPI_TIMER* timer);

//! Query timer for remain time until timeout.
//! This function will query current timer, return microseconds from now until it timeout
extern uint32_t osapi_getTimerRemain(OSAPI_TIMER* timer);

//! See if certain timer is still linked to notifyTimerlist.
//! This will return TRUE, if timer is linked to the list.
extern BOOL32 osapi_timerLinkedToNotifyTimeList(OSAPI_TIMER *timer);

//! Get the expected trigger point (time) of target timer.
//! This will return a 64-bit value time in microsecond.
#define wiced_timer_target_time_get(p_target_timer) (((OSAPI_TIMER *) (p_target_timer))->target_time)

//==================================================================================================
// Functions
//==================================================================================================
void clock_InitTimebase(void);


//! Delay us seconds
//! This function can only be used after system clock switching
void clock_DelayMicroseconds(uint32_t us);


//! System time in us, 64bits will not roll over.
uint64_t clock_SystemTimeMicroseconds64(void);
//! System time in us, 32bits will roll over every 1 hour 11 minutes.
uint32_t clock_SystemTimeMicroseconds32(void);

//! Function to return current 32 bit timestamp, to be called with interrupts locked or from ISR context.
uint32_t clock_SystemTimeMicroseconds32_nolock(void);

//! Update time base after sleep
void clock_UpdateTimebaseAfterSleep(uint64_t microseconds_elapsed_with_timers_off);

//! Enable Timers if stopped.
// if timers are stopped by g_clock_disableTimers=1 and this variable is set back to 0
// timers will be restored here, all active connections will be lost
extern void clock_enableTimersIfStopped(void);

//---------------------------------------------------------------------------------------------------------

typedef enum {          // this enumerated type is also used to index a table of required wait states
    CLOCK_DISABLE,      // for Thick Oxide Ram in slimboot.h.  If you modify this type, update the table
    CLOCK_1MHZ,         // in slimboot.h as well.  20150714 (sm)
    CLOCK_4MHZ,
    CLOCK_6MHZ,
    CLOCK_12MHZ,
    CLOCK_16MHZ,
    CLOCK_24MHZ,
    CLOCK_32MHZ,
    CLOCK_48MHZ,
    CLOCK_96MHZ,
    CLOCK_XTAL,
    CLOCK_UNKNOWN,
} CLOCK_FREQ;

typedef enum {
    CLOCK_REQ_TRANS,                // Set Transport clock to specific clock freqency.
    CLOCK_REQ_FM,                   // Set FM clock  freqency.
    CLOCK_REQ_CPU_SET,              // Set CPU to specific clock frequency
    CLOCK_REQ_CPU_DEFINE_MIN,       // Define the min freq we wlil set the cpu to via needup/release calls.
    CLOCK_REQ_CPU_DEFINE_MAX,       // Define the max freq we wlil set the cpu to via needup/release calls.
    CLOCK_REQ_CPU_NEED_UPTO,        // Update CPU to match new (usually more) clock frequency requirement
    CLOCK_REQ_CPU_RELEASE_FROM,     // release previous clock freqency updating requirement.
    CLOCK_REQ_CPU_LOCK_TO_SPEED,    // lock to the specified cpu - should only be called by one client
    CLOCK_REQ_CPU_UNLOCK_FROM_SPEED,// unlock the locked speed - should only be called by one client
    //CLOCK_REQ_CPU_NEED_AT_LEAST,    // request at least the specified speed.
    //CLOCK_REQ_CPU_NO_LONGER_NEED,   // release the "need at least" request.
    CLOCK_REQ_TRANS_DB_UART_SPI,    // Set Transport clock to specific clock freqency for db_uart/spiffy
} CLOCK_REQ;

//! Request clock.
//! This will make clock request based on req type.
extern uint32_t clock_Request(CLOCK_REQ req, CLOCK_FREQ freq);

// set the pause clock
uint32_t clock_PauseClock_Set(CLOCK_FREQ freq);


//! Determines the reference crystal speed and initializes the timebase.
extern void clock_Init(void);

//! Special version of clock_DelayMicroseconds.  The long name is to deter abuse, as this function
//! bypasses the locking of clock manager state.  This should be used only from inside a clock
//! manager clock state transition pre or post callback.
extern void clock_DelayMicrosecondsWithClockManagerStateLocked(int microseconds);

//! Delays execution for the specified number of clock cycles.  Unless the required delay is
//! determined directly by the CPU clock, use of clock_DelayMicroseconds is probably a better
//! choice.  Please exercise appropriate discretion.
extern void clock_DelayCPUClockCycles(int cycles);

//! Returns whether the timebase has been initialized yet.  If not, none of the timebase-related
//! functions can be called.  In that case, the delay functions can still be called, and they will
//! simply assume worst case scenario to assure the required delay has elapsed before returning,
//! though likely longer.
extern BOOL32 clock_HasTimebaseBeenInitialized(void);

//! Gets the crystal frequency in KHz.  Before the crystal frequency has been established, the
//! function will return 0.  Clients to the clock manager should generally stick to use of
//! clock_freq.  Cases when the crystal frequency needs to be used explicitly should be rare.
//! Please exercise appropriate discretion.
extern uint32_t clock_CrystalFrequencyKHz(void);

//! Determines the current freqency that CPU is running.
extern uint32_t clock_CpuFrequencyKhz(void);

//! Restored the timer saved before SDS
extern void clock_UpdateTimebaseAfterWarmboot(void);
extern void clock_UpdateTimebaseAfterFB(void);
extern uint32_t clock_GetLower32bitsOfSystemTimer(void);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
