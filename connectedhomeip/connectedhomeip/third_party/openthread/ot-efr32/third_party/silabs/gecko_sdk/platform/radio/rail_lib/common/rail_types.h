/***************************************************************************//**
 * @file
 * @brief This file contains the type definitions for RAIL structures, enums,
 *   and other types.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef __RAIL_TYPES_H__
#define __RAIL_TYPES_H__

// Include standard type headers to help define structures
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/// The RAIL library does not use enumerations because the ARM EABI leaves their
/// size ambiguous, which causes problems if the application is built
/// with different flags than the library. Instead, uint8_t typedefs
/// are used in compiled code for all enumerations. For documentation purposes, this is
/// converted to an actual enumeration since it's much easier to read in Doxygen.
#define RAIL_ENUM(name) enum name
/// This macro is a more generic version of the \ref RAIL_ENUM() macro that
/// allows the size of the type to be overridden instead of forcing the use of
/// a uint8_t. See \ref RAIL_ENUM() for more information.
#define RAIL_ENUM_GENERIC(name, type) enum name
#else
/// Define used for the RAIL library, which sets each enumeration to a uint8_t
/// typedef and creates a named enumeration structure for the enumeration values.
#define RAIL_ENUM(name) typedef uint8_t name; enum name##_enum
#define RAIL_ENUM_GENERIC(name, type) typedef type name; enum name##_enum
// For debugging, use the following define to turn this back into a proper enumeration
// #define RAIL_ENUM(name) typedef enum name##_enum name; enum name##_enum
#endif

/**
 * @addtogroup RAIL_API
 * @{
 */

/******************************************************************************
 * General Structures
 *****************************************************************************/
/**
 * @addtogroup General
 * @{
 */

/**
 * @struct RAIL_Version_t
 * @brief Contains RAIL Library Version Information.
 *   It is filled in by RAIL_GetVersion().
 */
typedef struct RAIL_Version {
  uint32_t hash;      /**< Git hash */
  uint8_t  major;     /**< Major number    */
  uint8_t  minor;     /**< Minor number    */
  uint8_t  rev;       /**< Revision number */
  uint8_t  build;     /**< Build number */
  uint8_t  flags;     /**< Build flags */
  /** Boolean to indicate whether this is a multiprotocol library or not. */
  bool     multiprotocol;
} RAIL_Version_t;

/**
 * @typedef RAIL_Handle_t
 * @brief A generic handle to a particular radio (e.g. RAIL_EFR32_HANDLE),
 *   or a real handle of a RAIL instance, as returned from RAIL_Init().
 *
 * Generic handles should be used for certain RAIL APIs that are called
 * prior to RAIL initialization. However, once RAIL has been initialized,
 * the real handle returned by RAIL_Init() should be used instead.
 */
typedef void *RAIL_Handle_t;

/**
 * @enum RAIL_Status_t
 * @brief A status returned by many RAIL API calls indicating their success or
 *   failure.
 */
RAIL_ENUM(RAIL_Status_t) {
  RAIL_STATUS_NO_ERROR, /**< RAIL function reports no error. */
  RAIL_STATUS_INVALID_PARAMETER, /**< Call to RAIL function threw an error
                                      because of an invalid parameter. */
  RAIL_STATUS_INVALID_STATE, /**< Call to RAIL function threw an error
                                  because it was called during an invalid
                                  radio state. */
  RAIL_STATUS_INVALID_CALL, /**< RAIL function is called in an invalid order. */
  RAIL_STATUS_SUSPENDED, /**< RAIL function did not finish in the allotted
                              time. */
  RAIL_STATUS_SCHED_ERROR, /**< RAIL function could not be scheduled
                                by the Radio scheduler. Only issued when
                                using a Multiprotocol application. */
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_STATUS_NO_ERROR          ((RAIL_Status_t) RAIL_STATUS_NO_ERROR)
#define RAIL_STATUS_INVALID_PARAMETER ((RAIL_Status_t) RAIL_STATUS_INVALID_PARAMETER)
#define RAIL_STATUS_INVALID_STATE     ((RAIL_Status_t) RAIL_STATUS_INVALID_STATE)
#define RAIL_STATUS_INVALID_CALL      ((RAIL_Status_t) RAIL_STATUS_INVALID_CALL)
#define RAIL_STATUS_SUSPENDED         ((RAIL_Status_t) RAIL_STATUS_SUSPENDED)
#define RAIL_STATUS_SCHED_ERROR       ((RAIL_Status_t) RAIL_STATUS_SCHED_ERROR)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * A pointer to init complete callback function
 *
 * @param[in] railHandle The initialized RAIL instance handle.
 *
 */
typedef void (*RAIL_InitCompleteCallbackPtr_t)(RAIL_Handle_t railHandle);

/** A value to signal that RAIL should not use DMA. */
#define RAIL_DMA_INVALID (0xFFU)

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * A linked list structure for RAIL state buffers which \ref RAIL_Init()
 * utilizes for managing internal RAIL state.
 */
typedef struct RAIL_StateBufferEntry {
  struct RAIL_StateBufferEntry *next; /**< pointer to next buffer in linked list */
  uint32_t bufferBytes;               /**< size of the buffer */
  uint64_t *buffer;                   /**< pointer to the buffer in RAM */
} RAIL_StateBufferEntry_t;

#endif//DOXYGEN_SHOULD_SKIP_THIS

/** @} */ // end of group General

/******************************************************************************
 * System Timing Structures
 *****************************************************************************/
/**
 * @addtogroup System_Timing
 * @{
 */

/**
 * @typedef RAIL_Time_t
 * @brief Time in microseconds
 */
typedef uint32_t RAIL_Time_t;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @typedef RAIL_TimerTick_t
 * @brief Internal RAIL hardware timer tick that drives the RAIL timebase.
 *
 * @note \ref RAIL_TimerTick_t does not use the full 32-bit range since we also
 *   account for fractional error drift on timebase overflow. This counts up
 *   to ~17 minutes before wrapping.
 *
 * @note \ref RAIL_TimerTicksToUs() can be used to convert the delta between
 *   two \ref RAIL_TimerTick_t values to microseconds.
 */
typedef uint32_t RAIL_TimerTick_t;
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * A pointer to the callback called when the RAIL timer expires.
 *
 * @param[in] cbArg The argument passed to the callback.
 */
typedef void (*RAIL_TimerCallback_t)(RAIL_Handle_t cbArg);

/**
 * @enum RAIL_TimeMode_t
 * @brief Specify a time offset in RAIL APIs.
 *
 * Different APIs use the same constants and may provide more specifics about
 * how they're used but the general use for each is described below.
 */
RAIL_ENUM(RAIL_TimeMode_t) {
  /**
   * The time specified is an exact time in the RAIL timebase. The given
   * event should happen at exactly that time. If this time is already in the
   * past, an error is returned. Because the RAIL timebase wraps at 32
   * bits, there is no real 'past'. Instead, any event greater than
   * 3/4 of the way into the future is considered to be in the past.
   */
  RAIL_TIME_ABSOLUTE,
  /**
   * The time specified is relative to the current time. The event should occur
   * that many ticks in the future. Delays are only guaranteed at least as long
   * as the value specified. An overhead may occur between the time when the
   * API is called and when the delay starts. As a result, using this for
   * operations that must happen at an exact given time is not recommended.
   * For that, you must use \ref RAIL_TIME_ABSOLUTE delays.
   *
   * Note that, if you specify a delay 0, that event is triggered as soon as
   * possible. This is different than specifying an absolute time of now which
   * would return an error unless it was possible.
   */
  RAIL_TIME_DELAY,
  /**
   * The specified time is invalid and should be ignored. For some APIs this
   * can also indicate that any previously stored delay should be invalidated
   * and disabled.
   */
  RAIL_TIME_DISABLED,
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_TIME_ABSOLUTE ((RAIL_TimeMode_t) RAIL_TIME_ABSOLUTE)
#define RAIL_TIME_DELAY    ((RAIL_TimeMode_t) RAIL_TIME_DELAY)
#define RAIL_TIME_DISABLED ((RAIL_TimeMode_t) RAIL_TIME_DISABLED)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/// Forward declaration of RAIL_MultiTimer
struct RAIL_MultiTimer;

/**
 * @typedef RAIL_MultiTimerCallback_t
 * @brief Callback fired when timer expires.
 *
 * @param[in] tmr A pointer to an expired timer.
 * @param[in] expectedTimeOfEvent An absolute time event fired.
 * @param[in] cbArg A user-supplied callback argument.
 */
typedef void (*RAIL_MultiTimerCallback_t)(struct RAIL_MultiTimer *tmr,
                                          RAIL_Time_t expectedTimeOfEvent,
                                          void *cbArg);

/**
 * @struct RAIL_MultiTimer_t
 * @brief RAIL timer state structure
 *
 * This structure is filled out and maintained internally only.
 * The user/application should not alter any elements of this structure.
 */
typedef struct RAIL_MultiTimer {
  RAIL_Time_t       absOffset;        /**< Absolute time before the next event. */
  RAIL_Time_t       relPeriodic;      /**< Relative, periodic time between events; 0 = timer is oneshot. */
  RAIL_MultiTimerCallback_t callback; /**< A user callback. */
  void                *cbArg;          /**< A user callback argument. */
  struct RAIL_MultiTimer   *next;          /**< A pointer to the next soft timer structure. */
  uint8_t             priority;       /**< A priority of the callback; 0 = highest priority; 255 = lowest. */
  bool                isRunning;      /**< Indicates the timer is currently running. */
  bool                doCallback;     /**< Indicates the callback needs to run. */
} RAIL_MultiTimer_t;

/**
 * @enum RAIL_PacketTimePosition_t
 * @brief The available packet timestamp position choices
 */
RAIL_ENUM(RAIL_PacketTimePosition_t) {
  /**
   * Indicate that a timestamp is not to be or was not provided.
   * It is useful if the application doesn't care about packet timestamps
   * and doesn't want RAIL to spend time calculating one.
   */
  RAIL_PACKET_TIME_INVALID = 0,
  /**
   * Request the choice most expedient for RAIL to calculate,
   * which may depend on the radio and/or its configuration.
   * The actual choice would always be reflected in the timePosition
   * field of \ref RAIL_RxPacketDetails_t or \ref RAIL_TxPacketDetails_t
   * returned and would never be one of the _USED_TOTAL values.
   */
  RAIL_PACKET_TIME_DEFAULT = 1,
  /**
   * Request the timestamp corresponding to the first preamble bit
   * sent or received.
   * Indicate that timestamp did not require using totalPacketBytes.
   */
  RAIL_PACKET_TIME_AT_PREAMBLE_START = 2,
  /**
   * Request the timestamp corresponding to the first preamble bit
   * sent or received.
   * Indicate that timestamp did require using totalPacketBytes.
   */
  RAIL_PACKET_TIME_AT_PREAMBLE_START_USED_TOTAL = 3,
  /**
   * Request the timestamp corresponding to right after its last
   * SYNC word bit has been sent or received.
   * Indicate that timestamp did not require using totalPacketBytes.
   */
  RAIL_PACKET_TIME_AT_SYNC_END = 4,
  /**
   * Request the timestamp corresponding to right after its last
   * SYNC word bit has been sent or received.
   * Indicate that timestamp did require using totalPacketBytes.
   */
  RAIL_PACKET_TIME_AT_SYNC_END_USED_TOTAL = 5,
  /**
   * Request the timestamp corresponding to right after its last
   * bit has been sent or received.
   * Indicate that timestamp did not require using totalPacketBytes.
   */
  RAIL_PACKET_TIME_AT_PACKET_END = 6,
  /**
   * Request the timestamp corresponding to right after its last
   * bit has been sent or received.
   * Indicate that timestamp did require using totalPacketBytes.
   */
  RAIL_PACKET_TIME_AT_PACKET_END_USED_TOTAL = 7,
  RAIL_PACKET_TIME_COUNT /**< A count of the choices in this enumeration. */
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_PACKET_TIME_INVALID                      ((RAIL_PacketTimePosition_t) RAIL_PACKET_TIME_INVALID)
#define RAIL_PACKET_TIME_DEFAULT                      ((RAIL_PacketTimePosition_t) RAIL_PACKET_TIME_DEFAULT)
#define RAIL_PACKET_TIME_AT_PREAMBLE_START            ((RAIL_PacketTimePosition_t) RAIL_PACKET_TIME_AT_PREAMBLE_START)
#define RAIL_PACKET_TIME_AT_PREAMBLE_START_USED_TOTAL ((RAIL_PacketTimePosition_t) RAIL_PACKET_TIME_AT_PREAMBLE_START_USED_TOTAL)
#define RAIL_PACKET_TIME_AT_SYNC_END                  ((RAIL_PacketTimePosition_t) RAIL_PACKET_TIME_AT_SYNC_END)
#define RAIL_PACKET_TIME_AT_SYNC_END_USED_TOTAL       ((RAIL_PacketTimePosition_t) RAIL_PACKET_TIME_AT_SYNC_END_USED_TOTAL)
#define RAIL_PACKET_TIME_AT_PACKET_END                ((RAIL_PacketTimePosition_t) RAIL_PACKET_TIME_AT_PACKET_END)
#define RAIL_PACKET_TIME_AT_PACKET_END_USED_TOTAL     ((RAIL_PacketTimePosition_t) RAIL_PACKET_TIME_AT_PACKET_END_USED_TOTAL)
#define RAIL_PACKET_TIME_COUNT                        ((RAIL_PacketTimePosition_t) RAIL_PACKET_TIME_COUNT)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * @struct RAIL_PacketTimeStamp_t
 * @brief Information for calculating and representing a packet timestamp.
 */
typedef struct RAIL_PacketTimeStamp {
  /**
   * Timestamp of the packet in the RAIL timebase.
   */
  RAIL_Time_t packetTime;
  /**
   * A value specifying the total length in bytes of the packet
   * used when calculating the packetTime requested by the timePosition
   * field. This should account for all bytes sent over the air after
   * the Preamble and Sync word(s) including CRC bytes.
   */
  uint16_t totalPacketBytes;
  /**
   * A RAIL_PacketTimePosition_t value specifying the packet position
   * to return in the packetTime field.
   * If this is \ref RAIL_PACKET_TIME_DEFAULT, this field will be
   * updated with the actual position corresponding to the packetTime
   * value filled in by a call using this structure.
   */
  RAIL_PacketTimePosition_t timePosition;
} RAIL_PacketTimeStamp_t;

/** @} */ // end of group System_Timing

/**
 * @addtogroup Sleep
 * @{
 */

/**
 * @enum RAIL_SleepConfig_t
 * @brief The configuration
 */
RAIL_ENUM(RAIL_SleepConfig_t) {
  RAIL_SLEEP_CONFIG_TIMERSYNC_DISABLED, /**< Disable timer sync before and after sleep. */
  RAIL_SLEEP_CONFIG_TIMERSYNC_ENABLED, /**< Enable timer sync before and after sleep. */
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_SLEEP_CONFIG_TIMERSYNC_DISABLED ((RAIL_SleepConfig_t) RAIL_SLEEP_CONFIG_TIMERSYNC_DISABLED)
#define RAIL_SLEEP_CONFIG_TIMERSYNC_ENABLED  ((RAIL_SleepConfig_t) RAIL_SLEEP_CONFIG_TIMERSYNC_ENABLED)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * @struct RAIL_TimerSyncConfig_t
 * @brief Channel values used to perform timer sync before and after sleep.
 *
 * The default value of this structure is provided in the
 * \ref RAIL_TIMER_SYNC_DEFAULT macro.
 */
typedef struct RAIL_TimerSyncConfig {
  /**
   * PRS Channel used for timer sync operations.
   */
  uint8_t prsChannel;
  /**
   * RTCC Channel used for timer sync operations
   */
  uint8_t rtccChannel;
  /**
   * Whether to sync the timer before and after sleeping
   */
  RAIL_SleepConfig_t sleep;
} RAIL_TimerSyncConfig_t;

/** @} */ // end of group Sleep

/******************************************************************************
 * Multiprotocol Structures
 *****************************************************************************/
/**
 * @addtogroup Multiprotocol
 * @{
 */

/**
 * @struct RAIL_SchedulerInfo_t
 * @brief A structure to hold information used by the scheduler.
 *
 * For multiprotocol versions of RAIL, this can be used to control how a receive
 * or transmit operation is run. It's not necessary in single-protocol applications.
 */
typedef struct RAIL_SchedulerInfo {
  /**
   * The scheduler priority to use for this operation. This priority is used to
   * preempt a long running lower-priority task to ensure higher-priority
   * operations complete in time. A lower numerical value represents a higher
   * logical priority meaning 0 is the highest priority and 255 is the lowest.
   */
  uint8_t priority;
  /**
   * The amount of time in us that this operation can slip by into the future
   * and still be run. This time is relative to the start time which may be
   * the current time for relative transmits. If the scheduler can't start the
   * operation by this time, it will be considered a failure.
   */
  RAIL_Time_t slipTime;
  /**
   * The transaction time in us for this operation. Since transaction times may
   * not be known exactly, use a minimum or an expected
   * guess for this time. The scheduler will use the value entered here to look
   * for overlaps between low-priority and high-priority tasks and attempt to
   * find a schedule where all tasks get to run.
   */
  RAIL_Time_t transactionTime;
} RAIL_SchedulerInfo_t;

/** Radio Scheduler Status mask*/
#define RAIL_SCHEDULER_STATUS_MASK       0x0FU
/** Radio Scheduler Status shift*/
#define RAIL_SCHEDULER_STATUS_SHIFT      0

/** Radio Scheduler Task mask*/
#define RAIL_SCHEDULER_TASK_MASK         0xF0U
/** Radio Scheduler Task shift*/
#define RAIL_SCHEDULER_TASK_SHIFT        4
/**
 * @enum RAIL_SchedulerStatus_t
 * @brief Multiprotocol scheduler status returned by RAIL_GetSchedulerStatus().
 *
 * \ref Multiprotocol scheduler status is a combination of the upper 4 bits which
 * constitute the type of scheduler task and the lower 4 bits which constitute
 * the type of scheduler error.
 */
RAIL_ENUM(RAIL_SchedulerStatus_t) {
  /** Lower 4 bits of uint8_t capture the different Radio Scheduler errors */
  /** Multiprotocol scheduler reports no error. */
  RAIL_SCHEDULER_STATUS_NO_ERROR = (0U << RAIL_SCHEDULER_STATUS_SHIFT),
  /**
   * The scheduler is disabled or the requested scheduler operation is
   * unsupported.
   */
  RAIL_SCHEDULER_STATUS_UNSUPPORTED = (1U << RAIL_SCHEDULER_STATUS_SHIFT),
  /**
   * The scheduled task was started but was interrupted by a higher-priority
   * event before it could be completed.
   */
  RAIL_SCHEDULER_STATUS_EVENT_INTERRUPTED = (2U << RAIL_SCHEDULER_STATUS_SHIFT),
  /**
   * Scheduled task could not be scheduled given its priority and the other
   * tasks running on the system.
   */
  RAIL_SCHEDULER_STATUS_SCHEDULE_FAIL = (3U << RAIL_SCHEDULER_STATUS_SHIFT),
  /**
   * Calling the RAIL API associated with the Radio scheduler task returned
   * an error code. See \ref RAIL_GetSchedulerStatus or \ref RAIL_GetSchedulerStatusAlt
   * for more information about \ref RAIL_Status_t status.
   */
  RAIL_SCHEDULER_STATUS_TASK_FAIL = (4U << RAIL_SCHEDULER_STATUS_SHIFT),
  /**
   * An internal error occurred in scheduler data structures, which should
   * not happen and indicates a problem.
   */
  RAIL_SCHEDULER_STATUS_INTERNAL_ERROR = (5U << RAIL_SCHEDULER_STATUS_SHIFT),

  /** Upper 4 bits of uint8_t capture the different Radio Scheduler tasks */
  RAIL_SCHEDULER_TASK_EMPTY = (0U << RAIL_SCHEDULER_TASK_SHIFT),
  /** Radio scheduler calls \ref RAIL_ScheduleRx(). */
  RAIL_SCHEDULER_TASK_SCHEDULED_RX = (1U << RAIL_SCHEDULER_TASK_SHIFT),
  /** Radio scheduler calls \ref RAIL_StartScheduledTx(). */
  RAIL_SCHEDULER_TASK_SCHEDULED_TX = (2U << RAIL_SCHEDULER_TASK_SHIFT),
  /** Radio scheduler calls \ref RAIL_StartTx(). */
  RAIL_SCHEDULER_TASK_SINGLE_TX = (3U << RAIL_SCHEDULER_TASK_SHIFT),
  /** Radio scheduler calls \ref RAIL_StartCcaCsmaTx(). */
  RAIL_SCHEDULER_TASK_SINGLE_CCA_CSMA_TX = (4U << RAIL_SCHEDULER_TASK_SHIFT),
  /** Radio scheduler calls \ref RAIL_StartCcaLbtTx(). */
  RAIL_SCHEDULER_TASK_SINGLE_CCA_LBT_TX = (5U << RAIL_SCHEDULER_TASK_SHIFT),
  /** Radio scheduler calls \ref RAIL_StartScheduledCcaCsmaTx(). */
  RAIL_SCHEDULER_TASK_SCHEDULED_CCA_CSMA_TX = (6U << RAIL_SCHEDULER_TASK_SHIFT),
  /** Radio scheduler calls \ref RAIL_StartScheduledCcaLbtTx(). */
  RAIL_SCHEDULER_TASK_SCHEDULED_CCA_LBT_TX = (7U << RAIL_SCHEDULER_TASK_SHIFT),
  /** Radio scheduler calls \ref RAIL_StartTxStream(). */
  RAIL_SCHEDULER_TASK_TX_STREAM = (8U << RAIL_SCHEDULER_TASK_SHIFT),
  /** Radio scheduler calls \ref RAIL_StartAverageRssi(). */
  RAIL_SCHEDULER_TASK_AVERAGE_RSSI = (9U << RAIL_SCHEDULER_TASK_SHIFT),

  /** \ref RAIL_StartScheduledTx() returned error status. */
  RAIL_SCHEDULER_STATUS_SCHEDULED_TX_FAIL = (RAIL_SCHEDULER_TASK_SCHEDULED_TX
                                             | RAIL_SCHEDULER_STATUS_TASK_FAIL),
  /** \ref RAIL_StartTx() returned error status. */
  RAIL_SCHEDULER_STATUS_SINGLE_TX_FAIL = (RAIL_SCHEDULER_TASK_SINGLE_TX
                                          | RAIL_SCHEDULER_STATUS_TASK_FAIL),
  /** \ref RAIL_StartCcaCsmaTx() returned error status. */
  RAIL_SCHEDULER_STATUS_CCA_CSMA_TX_FAIL = (RAIL_SCHEDULER_TASK_SINGLE_CCA_CSMA_TX
                                            | RAIL_SCHEDULER_STATUS_TASK_FAIL),
  /** \ref RAIL_StartCcaLbtTx() returned error status. */
  RAIL_SCHEDULER_STATUS_CCA_LBT_TX_FAIL = (RAIL_SCHEDULER_TASK_SINGLE_CCA_LBT_TX
                                           | RAIL_SCHEDULER_STATUS_TASK_FAIL),
  /** \ref RAIL_ScheduleRx() returned error status. */
  RAIL_SCHEDULER_STATUS_SCHEDULED_RX_FAIL = (RAIL_SCHEDULER_TASK_SCHEDULED_RX
                                             | RAIL_SCHEDULER_STATUS_TASK_FAIL),
  /** \ref RAIL_StartTxStream() returned error status. */
  RAIL_SCHEDULER_STATUS_TX_STREAM_FAIL = (RAIL_SCHEDULER_TASK_TX_STREAM
                                          | RAIL_SCHEDULER_STATUS_TASK_FAIL),
  /** \ref RAIL_StartAverageRssi() returned error status. */
  RAIL_SCHEDULER_STATUS_AVERAGE_RSSI_FAIL = (RAIL_SCHEDULER_TASK_AVERAGE_RSSI
                                             | RAIL_SCHEDULER_STATUS_TASK_FAIL),

  /** Multiprotocol scheduled receive function internal error. */
  RAIL_SCHEDULER_SCHEDULED_RX_INTERNAL_ERROR = (RAIL_SCHEDULER_TASK_SCHEDULED_RX
                                                | RAIL_SCHEDULER_STATUS_INTERNAL_ERROR),
  /** Multiprotocol scheduled receive scheduling error. */
  RAIL_SCHEDULER_SCHEDULED_RX_SCHEDULING_ERROR = (RAIL_SCHEDULER_TASK_SCHEDULED_RX
                                                  | RAIL_SCHEDULER_STATUS_SCHEDULE_FAIL),
  /** \ref RAIL_ScheduleRx() operation interrupted */
  RAIL_SCHEDULER_SCHEDULED_RX_INTERRUPTED = (RAIL_SCHEDULER_TASK_SCHEDULED_RX
                                             | RAIL_SCHEDULER_STATUS_EVENT_INTERRUPTED),

  /** Multiprotocol scheduled TX internal error. */
  RAIL_SCHEDULER_SCHEDULED_TX_INTERNAL_ERROR = (RAIL_SCHEDULER_TASK_SCHEDULED_TX
                                                | RAIL_SCHEDULER_STATUS_INTERNAL_ERROR),
  /** Multiprotocol scheduled TX scheduling error. */
  RAIL_SCHEDULER_SCHEDULED_TX_SCHEDULING_ERROR = (RAIL_SCHEDULER_TASK_SCHEDULED_TX
                                                  | RAIL_SCHEDULER_STATUS_SCHEDULE_FAIL),
  /** \ref RAIL_StartScheduledTx() operation interrupted */
  RAIL_SCHEDULER_SCHEDULED_TX_INTERRUPTED = (RAIL_SCHEDULER_TASK_SCHEDULED_TX
                                             | RAIL_SCHEDULER_STATUS_EVENT_INTERRUPTED),

  /** Multiprotocol instantaneous TX internal error. */
  RAIL_SCHEDULER_SINGLE_TX_INTERNAL_ERROR = (RAIL_SCHEDULER_TASK_SINGLE_TX
                                             | RAIL_SCHEDULER_STATUS_INTERNAL_ERROR),
  /** Multiprotocol instantaneous TX scheduling error. */
  RAIL_SCHEDULER_SINGLE_TX_SCHEDULING_ERROR = (RAIL_SCHEDULER_TASK_SINGLE_TX
                                               | RAIL_SCHEDULER_STATUS_SCHEDULE_FAIL),
  /** \ref RAIL_StartTx() operation interrupted */
  RAIL_SCHEDULER_SINGLE_TX_INTERRUPTED = (RAIL_SCHEDULER_TASK_SINGLE_TX
                                          | RAIL_SCHEDULER_STATUS_EVENT_INTERRUPTED),

  /** Multiprotocol single CSMA transmit function internal error. */
  RAIL_SCHEDULER_SINGLE_CCA_CSMA_TX_INTERNAL_ERROR = (RAIL_SCHEDULER_TASK_SINGLE_CCA_CSMA_TX
                                                      | RAIL_SCHEDULER_STATUS_INTERNAL_ERROR),
  /** Multiprotocol single CSMA transmit scheduling error. */
  RAIL_SCHEDULER_SINGLE_CCA_CSMA_TX_SCHEDULING_ERROR = (RAIL_SCHEDULER_TASK_SINGLE_CCA_CSMA_TX
                                                        | RAIL_SCHEDULER_STATUS_SCHEDULE_FAIL),
  /** \ref RAIL_StartCcaCsmaTx() operation interrupted */
  RAIL_SCHEDULER_SINGLE_CCA_CSMA_TX_INTERRUPTED = (RAIL_SCHEDULER_TASK_SINGLE_CCA_CSMA_TX
                                                   | RAIL_SCHEDULER_STATUS_EVENT_INTERRUPTED),

  /** Multiprotocol single LBT transmit function internal error. */
  RAIL_SCHEDULER_SINGLE_CCA_LBT_TX_INTERNAL_ERROR = (RAIL_SCHEDULER_TASK_SINGLE_CCA_LBT_TX
                                                     | RAIL_SCHEDULER_STATUS_INTERNAL_ERROR),
  /** Multiprotocol single LBT transmit scheduling error. */
  RAIL_SCHEDULER_SINGLE_CCA_LBT_TX_SCHEDULING_ERROR = (RAIL_SCHEDULER_TASK_SINGLE_CCA_LBT_TX
                                                       | RAIL_SCHEDULER_STATUS_SCHEDULE_FAIL),
  /** \ref RAIL_StartCcaLbtTx() operation interrupted */
  RAIL_SCHEDULER_SINGLE_CCA_LBT_TX_INTERRUPTED = (RAIL_SCHEDULER_TASK_SINGLE_CCA_LBT_TX
                                                  | RAIL_SCHEDULER_STATUS_EVENT_INTERRUPTED),

  /** Multiprotocol scheduled CSMA transmit function internal error. */
  RAIL_SCHEDULER_SCHEDULED_CCA_CSMA_TX_INTERNAL_ERROR = (RAIL_SCHEDULER_TASK_SCHEDULED_CCA_CSMA_TX
                                                         | RAIL_SCHEDULER_STATUS_INTERNAL_ERROR),
  /** \ref RAIL_StartScheduledCcaCsmaTx() returned error status. */
  RAIL_SCHEDULER_SCHEDULED_CCA_CSMA_TX_FAIL = (RAIL_SCHEDULER_TASK_SCHEDULED_CCA_CSMA_TX
                                               | RAIL_SCHEDULER_STATUS_TASK_FAIL),
  /** Multiprotocol scheduled CSMA transmit scheduling error. */
  RAIL_SCHEDULER_SCHEDULED_CCA_CSMA_TX_SCHEDULING_ERROR = (RAIL_SCHEDULER_TASK_SCHEDULED_CCA_CSMA_TX
                                                           | RAIL_SCHEDULER_STATUS_SCHEDULE_FAIL),
  /** \ref RAIL_StartScheduledCcaCsmaTx() operation interrupted */
  RAIL_SCHEDULER_SCHEDULED_CCA_CSMA_TX_INTERRUPTED = (RAIL_SCHEDULER_TASK_SCHEDULED_CCA_CSMA_TX
                                                      | RAIL_SCHEDULER_STATUS_EVENT_INTERRUPTED),

  /** Multiprotocol scheduled LBT transmit function internal error. */
  RAIL_SCHEDULER_SCHEDULED_CCA_LBT_TX_INTERNAL_ERROR = (RAIL_SCHEDULER_TASK_SCHEDULED_CCA_LBT_TX
                                                        | RAIL_SCHEDULER_STATUS_INTERNAL_ERROR),
  /** \ref RAIL_StartScheduledCcaLbtTx() returned error status. */
  RAIL_SCHEDULER_SCHEDULED_CCA_LBT_TX_FAIL = (RAIL_SCHEDULER_TASK_SCHEDULED_CCA_LBT_TX
                                              | RAIL_SCHEDULER_STATUS_TASK_FAIL),
  /** Multiprotocol scheduled LBT transmit scheduling error. */
  RAIL_SCHEDULER_SCHEDULED_CCA_LBT_TX_SCHEDULING_ERROR = (RAIL_SCHEDULER_TASK_SCHEDULED_CCA_LBT_TX
                                                          | RAIL_SCHEDULER_STATUS_SCHEDULE_FAIL),
  /** \ref RAIL_StartScheduledCcaLbtTx() operation interrupted */
  RAIL_SCHEDULER_SCHEDULED_CCA_LBT_TX_INTERRUPTED = (RAIL_SCHEDULER_TASK_SCHEDULED_CCA_LBT_TX
                                                     | RAIL_SCHEDULER_STATUS_EVENT_INTERRUPTED),

  /** Multiprotocol stream transmit function internal error. */
  RAIL_SCHEDULER_TX_STREAM_INTERNAL_ERROR = (RAIL_SCHEDULER_TASK_TX_STREAM
                                             | RAIL_SCHEDULER_STATUS_INTERNAL_ERROR),
  /** Multiprotocol stream transmit scheduling error. */
  RAIL_SCHEDULER_TX_STREAM_SCHEDULING_ERROR = (RAIL_SCHEDULER_TASK_TX_STREAM
                                               | RAIL_SCHEDULER_STATUS_SCHEDULE_FAIL),
  /** \ref RAIL_StartTxStream() operation interrupted */
  RAIL_SCHEDULER_TX_STREAM_INTERRUPTED = (RAIL_SCHEDULER_TASK_TX_STREAM
                                          | RAIL_SCHEDULER_STATUS_EVENT_INTERRUPTED),

  /** Multiprotocol RSSI averaging function internal error. */
  RAIL_SCHEDULER_AVERAGE_RSSI_INTERNAL_ERROR = (RAIL_SCHEDULER_TASK_AVERAGE_RSSI
                                                | RAIL_SCHEDULER_STATUS_INTERNAL_ERROR),
  /** Multiprotocol RSSI average scheduling error. */
  RAIL_SCHEDULER_AVERAGE_RSSI_SCHEDULING_ERROR = (RAIL_SCHEDULER_TASK_AVERAGE_RSSI
                                                  | RAIL_SCHEDULER_STATUS_SCHEDULE_FAIL),
  /** \ref RAIL_StartAverageRssi() operation interrupted */
  RAIL_SCHEDULER_AVERAGE_RSSI_INTERRUPTED = (RAIL_SCHEDULER_TASK_AVERAGE_RSSI
                                             | RAIL_SCHEDULER_STATUS_EVENT_INTERRUPTED),
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_SCHEDULER_STATUS_NO_ERROR                               ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_STATUS_NO_ERROR)
#define RAIL_SCHEDULER_STATUS_UNSUPPORTED                            ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_STATUS_UNSUPPORTED)
#define RAIL_SCHEDULER_STATUS_EVENT_INTERRUPTED                      ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_STATUS_EVENT_INTERRUPTED)
#define RAIL_SCHEDULER_STATUS_SCHEDULE_FAIL                          ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_STATUS_SCHEDULE_FAIL)
#define RAIL_SCHEDULER_STATUS_SCHEDULED_TX_FAIL                      ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_STATUS_SCHEDULED_TX_FAIL)
#define RAIL_SCHEDULER_STATUS_SINGLE_TX_FAIL                         ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_STATUS_SINGLE_TX_FAIL)
#define RAIL_SCHEDULER_STATUS_CCA_CSMA_TX_FAIL                       ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_STATUS_CCA_CSMA_TX_FAIL)
#define RAIL_SCHEDULER_STATUS_CCA_LBT_TX_FAIL                        ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_STATUS_CCA_LBT_TX_FAIL)
#define RAIL_SCHEDULER_STATUS_SCHEDULED_RX_FAIL                      ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_STATUS_SCHEDULED_RX_FAIL)
#define RAIL_SCHEDULER_STATUS_TX_STREAM_FAIL                         ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_STATUS_TX_STREAM_FAIL)
#define RAIL_SCHEDULER_STATUS_AVERAGE_RSSI_FAIL                      ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_STATUS_AVERAGE_RSSI_FAIL)
#define RAIL_SCHEDULER_STATUS_INTERNAL_ERROR                         ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_STATUS_INTERNAL_ERROR)

#define RAIL_SCHEDULER_TASK_EMPTY                                    ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_TASK_EMPTY)
#define RAIL_SCHEDULER_TASK_SCHEDULED_RX                             ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_TASK_SCHEDULED_RX)
#define RAIL_SCHEDULER_TASK_SCHEDULED_TX                             ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_SCHEDULED_TX)
#define RAIL_SCHEDULER_TASK_SINGLE_TX                                ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_TASK_SINGLE_TX)
#define RAIL_SCHEDULER_TASK_SINGLE_CCA_CSMA_TX                       ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_TASK_SINGLE_CCA_CSMA_TX)
#define RAIL_SCHEDULER_TASK_SINGLE_CCA_LBT_TX                        ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_TASK_SINGLE_CCA_LBT_TX)
#define RAIL_SCHEDULER_TASK_SCHEDULED_CCA_CSMA_TX                    ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_TASK_SCHEDULED_CCA_CSMA_TX)
#define RAIL_SCHEDULER_TASK_SCHEDULED_CCA_LBT_TX                     ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_TASK_SCHEDULED_CCA_LBT_TX)
#define RAIL_SCHEDULER_TASK_TX_STREAM                                ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_TASK_TX_STREAM)
#define RAIL_SCHEDULER_TASK_AVERAGE_RSSI                             ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_TASK_AVERAGE_RSSI)

#define RAIL_SCHEDULER_SCHEDULED_RX_INTERNAL_ERROR                   ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_SCHEDULED_RX_INTERNAL_ERROR)
#define RAIL_SCHEDULER_SCHEDULED_RX_SCHEDULING_ERROR                 ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_SCHEDULED_RX_SCHEDULING_ERROR)
#define RAIL_SCHEDULER_SCHEDULED_RX_INTERRUPTED                      ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_SCHEDULED_RX_INTERRUPTED)
#define RAIL_SCHEDULER_SCHEDULED_TX_INTERNAL_ERROR                   ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_SCHEDULED_TX_INTERNAL_ERROR)
#define RAIL_SCHEDULER_SCHEDULED_TX_SCHEDULING_ERROR                 ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_SCHEDULED_TX_SCHEDULING_ERROR)
#define RAIL_SCHEDULER_SCHEDULED_TX_INTERRUPTED                      ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_SCHEDULED_TX_INTERRUPTED)
#define RAIL_SCHEDULER_SINGLE_TX_INTERNAL_ERROR                      ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_SINGLE_TX_INTERNAL_ERROR)
#define RAIL_SCHEDULER_SINGLE_TX_SCHEDULING_ERROR                    ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_SINGLE_TX_SCHEDULING_ERROR)
#define RAIL_SCHEDULER_SINGLE_TX_INTERRUPTED                         ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_SINGLE_TX_INTERRUPTED)
#define RAIL_SCHEDULER_CCA_CSMA_TX_INTERNAL_ERROR                    ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_CCA_CSMA_TX_INTERNAL_ERROR)
#define RAIL_SCHEDULER_CCA_CSMA_TX_SCHEDULING_ERROR                  ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_CCA_CSMA_TX_SCHEDULING_ERROR)
#define RAIL_SCHEDULER_CCA_CSMA_TX_INTERRUPTED                       ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_CCA_CSMA_TX_INTERRUPTED)
#define RAIL_SCHEDULER_CCA_LBT_TX_INTERNAL_ERROR                     ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_CCA_LBT_TX_INTERNAL_ERROR)
#define RAIL_SCHEDULER_CCA_LBT_TX_SCHEDULING_ERROR                   ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_CCA_LBT_TX_SCHEDULING_ERROR)
#define RAIL_SCHEDULER_CCA_LBT_TX_INTERRUPTED                        ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_CCA_LBT_TX_INTERRUPTED)
#define RAIL_SCHEDULER_SCHEDULED_CCA_CSMA_TX_INTERNAL_ERROR          ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_SCHEDULED_CCA_CSMA_TX_INTERNAL_ERROR)
#define RAIL_SCHEDULER_SCHEDULED_CCA_CSMA_TX_FAIL                    ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_SCHEDULED_CCA_CSMA_TX_FAIL)
#define RAIL_SCHEDULER_SCHEDULED_CCA_CSMA_TX_SCHEDULING_ERROR        ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_SCHEDULED_CCA_CSMA_TX_SCHEDULING_ERROR)
#define RAIL_SCHEDULER_SCHEDULED_CCA_CSMA_TX_INTERRUPTED             ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_SCHEDULED_CCA_CSMA_TX_INTERRUPTED)
#define RAIL_SCHEDULER_SCHEDULED_CCA_LBT_TX_INTERNAL_ERROR           ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_SCHEDULED_CCA_LBT_TX_INTERNAL_ERROR)
#define RAIL_SCHEDULER_SCHEDULED_CCA_LBT_TX_FAIL                     ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_SCHEDULED_CCA_LBT_TX_FAIL)
#define RAIL_SCHEDULER_SCHEDULED_CCA_LBT_TX_SCHEDULING_ERROR         ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_SCHEDULED_CCA_LBT_TX_SCHEDULING_ERROR)
#define RAIL_SCHEDULER_SCHEDULED_CCA_LBT_TX_INTERRUPTED              ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_SCHEDULED_CCA_LBT_TX_INTERRUPTED)
#define RAIL_SCHEDULER_TX_STREAM_INTERNAL_ERROR                      ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_TX_STREAM_INTERNAL_ERROR)
#define RAIL_SCHEDULER_TX_STREAM_SCHEDULING_ERROR                    ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_TX_STREAM_SCHEDULING_ERROR)
#define RAIL_SCHEDULER_TX_STREAM_INTERRUPTED                         ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_TX_STREAM_INTERRUPTED)
#define RAIL_SCHEDULER_AVERAGE_RSSI_INTERNAL_ERROR                   ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_AVERAGE_RSSI_INTERNAL_ERROR)
#define RAIL_SCHEDULER_AVERAGE_RSSI_SCHEDULING_ERROR                 ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_AVERAGE_RSSI_SCHEDULING_ERROR)
#define RAIL_SCHEDULER_AVERAGE_RSSI_INTERRUPTED                      ((RAIL_SchedulerStatus_t) RAIL_SCHEDULER_AVERAGE_RSSI_INTERRUPTED)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * @enum RAIL_TaskType_t
 * @brief Multiprotocol radio operation task types, used with
 *        RAIL_SetTaskPriority.
 */
RAIL_ENUM(RAIL_TaskType_t) {
  /** Indicate a task started using RAIL_StartRx */
  RAIL_TASK_TYPE_START_RX,
  /** Indicate a task started functions other than RAIL_StartRx */
  RAIL_TASK_TYPE_OTHER,
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_TASK_TYPE_START_RX ((RAIL_TaskType_t) RAIL_TASK_TYPE_START_RX)
#define RAIL_TASK_TYPE_OTHER    ((RAIL_TaskType_t) RAIL_TASK_TYPE_OTHER)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/** @} */ // end of group Multiprotocol

/******************************************************************************
 * Event Structures
 *****************************************************************************/
/**
 * @addtogroup Events
 * @{
 */

/**
 * @enum RAIL_Events_t
 * @brief RAIL events passed to the event callback. More than one event may be
 *   indicated due to interrupt latency.
 */
RAIL_ENUM_GENERIC(RAIL_Events_t, uint64_t) {
  // RX Event Bit Shifts

  /** Shift position of \ref RAIL_EVENT_RSSI_AVERAGE_DONE bit */
  RAIL_EVENT_RSSI_AVERAGE_DONE_SHIFT = 0,
  /** Shift position of \ref RAIL_EVENT_RX_ACK_TIMEOUT bit */
  RAIL_EVENT_RX_ACK_TIMEOUT_SHIFT,
  /** Shift position of \ref RAIL_EVENT_RX_FIFO_ALMOST_FULL bit */
  RAIL_EVENT_RX_FIFO_ALMOST_FULL_SHIFT,
  /** Shift position of \ref RAIL_EVENT_RX_PACKET_RECEIVED bit */
  RAIL_EVENT_RX_PACKET_RECEIVED_SHIFT,
  /** Shift position of \ref RAIL_EVENT_RX_PREAMBLE_LOST bit */
  RAIL_EVENT_RX_PREAMBLE_LOST_SHIFT,
  /** Shift position of \ref RAIL_EVENT_RX_PREAMBLE_DETECT bit */
  RAIL_EVENT_RX_PREAMBLE_DETECT_SHIFT,
  /** Shift position of \ref RAIL_EVENT_RX_SYNC1_DETECT bit */
  RAIL_EVENT_RX_SYNC1_DETECT_SHIFT,
  /** Shift position of \ref RAIL_EVENT_RX_SYNC2_DETECT bit */
  RAIL_EVENT_RX_SYNC2_DETECT_SHIFT,
  /** Shift position of \ref RAIL_EVENT_RX_FRAME_ERROR bit */
  RAIL_EVENT_RX_FRAME_ERROR_SHIFT,
  /** Shift position of \ref RAIL_EVENT_RX_FIFO_FULL bit */
  RAIL_EVENT_RX_FIFO_FULL_SHIFT,
  /** Shift position of \ref RAIL_EVENT_RX_FIFO_OVERFLOW bit */
  RAIL_EVENT_RX_FIFO_OVERFLOW_SHIFT,
  /** Shift position of \ref RAIL_EVENT_RX_ADDRESS_FILTERED bit */
  RAIL_EVENT_RX_ADDRESS_FILTERED_SHIFT,
  /** Shift position of \ref RAIL_EVENT_RX_TIMEOUT bit */
  RAIL_EVENT_RX_TIMEOUT_SHIFT,
  /** Shift position of \ref RAIL_EVENT_SCHEDULED_RX_STARTED bit */
  RAIL_EVENT_SCHEDULED_RX_STARTED_SHIFT,
  /** Shift position of \ref RAIL_EVENT_SCHEDULED_TX_STARTED bit */
  RAIL_EVENT_SCHEDULED_TX_STARTED_SHIFT = RAIL_EVENT_SCHEDULED_RX_STARTED_SHIFT,
  /** Shift position of \ref  RAIL_EVENT_RX_SCHEDULED_RX_END bit */
  RAIL_EVENT_RX_SCHEDULED_RX_END_SHIFT,
  /** Shift position of \ref  RAIL_EVENT_RX_SCHEDULED_RX_MISSED bit */
  RAIL_EVENT_RX_SCHEDULED_RX_MISSED_SHIFT,
  /** Shift position of \ref RAIL_EVENT_RX_PACKET_ABORTED bit */
  RAIL_EVENT_RX_PACKET_ABORTED_SHIFT,
  /** Shift position of \ref RAIL_EVENT_RX_FILTER_PASSED bit */
  RAIL_EVENT_RX_FILTER_PASSED_SHIFT,
  /** Shift position of \ref RAIL_EVENT_RX_TIMING_LOST bit */
  RAIL_EVENT_RX_TIMING_LOST_SHIFT,
  /** Shift position of \ref RAIL_EVENT_RX_TIMING_DETECT bit */
  RAIL_EVENT_RX_TIMING_DETECT_SHIFT,
  /** Shift position of \ref RAIL_EVENT_RX_CHANNEL_HOPPING_COMPLETE bit */
  RAIL_EVENT_RX_CHANNEL_HOPPING_COMPLETE_SHIFT,
  /** Shift position of \ref RAIL_EVENT_RX_DUTY_CYCLE_RX_END bit */
  RAIL_EVENT_RX_DUTY_CYCLE_RX_END_SHIFT = RAIL_EVENT_RX_CHANNEL_HOPPING_COMPLETE_SHIFT,
  /** Shift position of \ref RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND bit */
  RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND_SHIFT,
  /** Shift position of \ref RAIL_EVENT_ZWAVE_LR_ACK_REQUEST_COMMAND_SHIFT bit */
  RAIL_EVENT_ZWAVE_LR_ACK_REQUEST_COMMAND_SHIFT = RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND_SHIFT,
  // TX Event Bit Shifts

  /** Shift position of \ref RAIL_EVENT_MFM_TX_BUFFER_DONE bit */
  RAIL_EVENT_MFM_TX_BUFFER_DONE_SHIFT = RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND_SHIFT,
  /** Shift position of \ref RAIL_EVENT_ZWAVE_BEAM bit */
  RAIL_EVENT_ZWAVE_BEAM_SHIFT,

  /** Shift position of \ref RAIL_EVENT_TX_FIFO_ALMOST_EMPTY bit */
  RAIL_EVENT_TX_FIFO_ALMOST_EMPTY_SHIFT,
  /** Shift position of \ref RAIL_EVENT_TX_PACKET_SENT bit */
  RAIL_EVENT_TX_PACKET_SENT_SHIFT,
  /** Shift position of \ref RAIL_EVENT_TXACK_PACKET_SENT bit */
  RAIL_EVENT_TXACK_PACKET_SENT_SHIFT,
  /** Shift position of \ref RAIL_EVENT_TX_ABORTED bit */
  RAIL_EVENT_TX_ABORTED_SHIFT,
  /** Shift position of \ref RAIL_EVENT_TXACK_ABORTED bit */
  RAIL_EVENT_TXACK_ABORTED_SHIFT,
  /** Shift position of \ref RAIL_EVENT_TX_BLOCKED bit */
  RAIL_EVENT_TX_BLOCKED_SHIFT,
  /** Shift position of \ref RAIL_EVENT_TXACK_BLOCKED bit */
  RAIL_EVENT_TXACK_BLOCKED_SHIFT,
  /** Shift position of \ref RAIL_EVENT_TX_UNDERFLOW bit */
  RAIL_EVENT_TX_UNDERFLOW_SHIFT,
  /** Shift position of \ref RAIL_EVENT_TXACK_UNDERFLOW bit */
  RAIL_EVENT_TXACK_UNDERFLOW_SHIFT,
  /** Shift position of \ref RAIL_EVENT_TX_CHANNEL_CLEAR bit */
  RAIL_EVENT_TX_CHANNEL_CLEAR_SHIFT,
  /** Shift position of \ref RAIL_EVENT_TX_CHANNEL_BUSY bit */
  RAIL_EVENT_TX_CHANNEL_BUSY_SHIFT,
  /** Shift position of \ref RAIL_EVENT_TX_CCA_RETRY bit */
  RAIL_EVENT_TX_CCA_RETRY_SHIFT,
  /** Shift position of \ref RAIL_EVENT_TX_START_CCA bit */
  RAIL_EVENT_TX_START_CCA_SHIFT,
  /** Shift position of \ref RAIL_EVENT_TX_STARTED bit */
  RAIL_EVENT_TX_STARTED_SHIFT,
  /** Shift position of \ref  RAIL_EVENT_TX_SCHEDULED_TX_MISSED bit */
  RAIL_EVENT_TX_SCHEDULED_TX_MISSED_SHIFT,

  // Scheduler Event Bit Shifts

  /** Shift position of \ref RAIL_EVENT_CONFIG_UNSCHEDULED bit */
  RAIL_EVENT_CONFIG_UNSCHEDULED_SHIFT,
  /** Shift position of \ref RAIL_EVENT_CONFIG_SCHEDULED bit */
  RAIL_EVENT_CONFIG_SCHEDULED_SHIFT,
  /** Shift position of \ref RAIL_EVENT_SCHEDULER_STATUS bit */
  RAIL_EVENT_SCHEDULER_STATUS_SHIFT,

  // Other Event Bit Shifts

  /** Shift position of \ref RAIL_EVENT_CAL_NEEDED bit */
  RAIL_EVENT_CAL_NEEDED_SHIFT,
  /** Shift position of \ref RAIL_EVENT_RF_SENSED bit */
  RAIL_EVENT_RF_SENSED_SHIFT,
  /** Shift position of \ref RAIL_EVENT_PA_PROTECTION bit */
  RAIL_EVENT_PA_PROTECTION_SHIFT,
  /** Shift position of \ref RAIL_EVENT_SIGNAL_DETECTED bit */
  RAIL_EVENT_SIGNAL_DETECTED_SHIFT,
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  /** Shift position of \ref RAIL_EVENT_IEEE802154_MODESWITCH_START bit */
  RAIL_EVENT_IEEE802154_MODESWITCH_START_SHIFT,
  /** Shift position of \ref RAIL_EVENT_IEEE802154_MODESWITCH_END bit */
  RAIL_EVENT_IEEE802154_MODESWITCH_END_SHIFT,
#endif//DOXYGEN_SHOULD_SKIP_THIS
  /** Shift position of \ref RAIL_EVENT_DETECT_RSSI_THRESHOLD bit */
  RAIL_EVENT_DETECT_RSSI_THRESHOLD_SHIFT,
};

// RAIL_Event_t bitmasks

/** A value representing no events */
#define RAIL_EVENTS_NONE 0ULL

/**
 * Occurs when the hardware-averaged RSSI is done in response to
 * RAIL_StartAverageRssi() to indicate that the hardware has completed
 * averaging.
 *
 * Call RAIL_GetAverageRssi() to get the result.
 */
#define RAIL_EVENT_RSSI_AVERAGE_DONE (1ULL << RAIL_EVENT_RSSI_AVERAGE_DONE_SHIFT)

/**
 * Occurs when the ACK timeout expires while waiting to receive the
 * sync word of an expected ACK. If the timeout occurs within packet
 * reception, this event won't be signaled until after packet
 * completion has determined the packet wasn't the expected ACK.
 * See \ref RAIL_RxPacketDetails_t::isAck for the definition of an
 * expected ACK.
 *
 * This event only occurs after calling RAIL_ConfigAutoAck() and after
 * transmitting a packet with \ref RAIL_TX_OPTION_WAIT_FOR_ACK set.
 */
#define RAIL_EVENT_RX_ACK_TIMEOUT (1ULL << RAIL_EVENT_RX_ACK_TIMEOUT_SHIFT)

/**
 * Occurs when the number of bytes in the receive FIFO exceeds the configured
 * threshold value.
 *
 * Call RAIL_GetRxFifoBytesAvailable() to get the number of
 * bytes available. When using this event, the threshold should be set via
 * RAIL_SetRxFifoThreshold().
 */
#define RAIL_EVENT_RX_FIFO_ALMOST_FULL (1ULL << RAIL_EVENT_RX_FIFO_ALMOST_FULL_SHIFT)

/**
 * Occurs whenever a packet is received with \ref RAIL_RX_PACKET_READY_SUCCESS
 * or \ref RAIL_RX_PACKET_READY_CRC_ERROR.
 *
 * Call RAIL_GetRxPacketInfo() to get
 * basic information about the packet along with a handle to this packet for
 * subsequent use with RAIL_PeekRxPacket(), RAIL_GetRxPacketDetails(),
 * RAIL_HoldRxPacket(), and RAIL_ReleaseRxPacket() as needed.
 */
#define RAIL_EVENT_RX_PACKET_RECEIVED (1ULL << RAIL_EVENT_RX_PACKET_RECEIVED_SHIFT)

/**
 * Occurs when the radio has lost a preamble.
 *
 * This event can occur multiple
 * times while searching for a packet and is generally used for diagnostic
 * purposes. It can only occur after a
 * \ref RAIL_EVENT_RX_PREAMBLE_DETECT event has already occurred.
 */
#define RAIL_EVENT_RX_PREAMBLE_LOST (1ULL << RAIL_EVENT_RX_PREAMBLE_LOST_SHIFT)

/**
 * Occurs when the radio has detected a preamble.
 *
 * This event can occur multiple
 * times while searching for a packet and is generally used for diagnostic
 * purposes. It can only occur after a \ref RAIL_EVENT_RX_TIMING_DETECT
 * event has already occurred.
 */
#define RAIL_EVENT_RX_PREAMBLE_DETECT (1ULL << RAIL_EVENT_RX_PREAMBLE_DETECT_SHIFT)

/**
 * Occurs when the first sync word is detected.
 *
 * After this event occurs, one of
 * the events in the \ref RAIL_EVENTS_RX_COMPLETION mask will occur.
 */
#define RAIL_EVENT_RX_SYNC1_DETECT (1ULL << RAIL_EVENT_RX_SYNC1_DETECT_SHIFT)

/**
 * Occurs when the second sync word is detected.
 *
 * After this event occurs, one of
 * the events in the \ref RAIL_EVENTS_RX_COMPLETION mask will occur.
 */
#define RAIL_EVENT_RX_SYNC2_DETECT (1ULL << RAIL_EVENT_RX_SYNC2_DETECT_SHIFT)

/**
 * Occurs when a receive is aborted with \ref RAIL_RX_PACKET_ABORT_CRC_ERROR
 * which only happens after any filtering has passed.
 *
 * For EFR32 parts, this event includes CRC errors, block decoding errors,
 * and illegal frame length -- when detected after filtering. (When such
 * errors are detected during filtering, they're signaled as \ref
 * RAIL_EVENT_RX_PACKET_ABORTED instead.)
 *
 * If \ref RAIL_RX_OPTION_IGNORE_CRC_ERRORS is set, this event will not
 * occur for CRC errors, but could still occur for the other errors.
 */
#define RAIL_EVENT_RX_FRAME_ERROR (1ULL << RAIL_EVENT_RX_FRAME_ERROR_SHIFT)

/**
 * When using \ref RAIL_RxDataSource_t::RX_PACKET_DATA this event
 * occurs coincident to a receive packet completion event in which the
 * receive FIFO or any supplemental packet metadata FIFO (see \ref
 * Data_Management) are full and further packet reception is jeopardized.
 *
 * It signals that an overflow is imminent (and may already have occurred)
 * telling the application it should release the oldest packet(s) as soon
 * as possible. This event may may be posted multiple times with subsequent
 * receive completion events if the FIFO(s) remain full, and should also
 * occur coincident with \ref RAIL_EVENT_RX_FIFO_OVERFLOW.
 *
 * When not using \ref RAIL_RxDataSource_t::RX_PACKET_DATA this event
 * is not tied to packet completion and will occur coincident with
 * \ref RAIL_EVENT_RX_FIFO_OVERFLOW when the receive FIFO has filled and
 * overflowed. The application should consume receive FIFO data via
 * \ref RAIL_ReadRxFifo() as soon as possible to minimize lost raw data.
 */
#define RAIL_EVENT_RX_FIFO_FULL (1ULL << RAIL_EVENT_RX_FIFO_FULL_SHIFT)

/**
 * When using \ref RAIL_RxDataSource_t::RX_PACKET_DATA this event
 * occurs when a receive is aborted with \ref RAIL_RX_PACKET_ABORT_OVERFLOW
 * due to overflowing the receive FIFO or any supplemental packet metadata
 * FIFO (see \ref Data_Management).
 *
 * The radio suspends receiving packets until this event is posted and
 * the receive FIFO(s) have been fully processed (drained and released
 * or reset). It is not guaranteed that a \ref RAIL_EVENT_RX_FIFO_FULL
 * will precede this event, but both events should be coincident.
 *
 * When not using \ref RAIL_RxDataSource_t::RX_PACKET_DATA this event
 * is not tied to packet completion and will occur coincident with
 * \ref RAIL_EVENT_RX_FIFO_FULL when the receive FIFO has filled and
 * overflowed. The application should consume receive FIFO data via
 * \ref RAIL_ReadRxFifo() as soon as possible to minimize lost raw data.
 */
#define RAIL_EVENT_RX_FIFO_OVERFLOW (1ULL << RAIL_EVENT_RX_FIFO_OVERFLOW_SHIFT)

/**
 * Occurs when a receive is aborted with \ref RAIL_RX_PACKET_ABORT_FILTERED
 * because its address does not match the filtering settings.
 *
 * This event can only occur after calling RAIL_EnableAddressFilter().
 */
#define RAIL_EVENT_RX_ADDRESS_FILTERED (1ULL << RAIL_EVENT_RX_ADDRESS_FILTERED_SHIFT)

/**
 * Occurs when an RX event times out.
 *
 * This event can only occur if the
 * RAIL_StateTiming_t::rxSearchTimeout passed to RAIL_SetStateTiming() is
 * not zero.
 */
#define RAIL_EVENT_RX_TIMEOUT (1ULL << RAIL_EVENT_RX_TIMEOUT_SHIFT)

/**
 * Occurs when a scheduled RX begins turning on the transmitter.
 * This event has the same numerical value as RAIL_EVENT_SCHEDULED_TX_STARTED
 * because one cannot schedule both RX and TX simultaneously.
 */
#define RAIL_EVENT_SCHEDULED_RX_STARTED (1ULL << RAIL_EVENT_SCHEDULED_RX_STARTED_SHIFT)

/**
 * Occurs when a scheduled TX begins turning on the transmitter.
 * This event has the same numerical value as RAIL_EVENT_SCHEDULED_RX_STARTED
 * because one cannot schedule both RX and TX simultaneously.
 */
#define RAIL_EVENT_SCHEDULED_TX_STARTED (1ULL << RAIL_EVENT_SCHEDULED_TX_STARTED_SHIFT)

/**
 * Occurs when the scheduled RX window ends.
 *
 * This event only occurs in response
 * to a scheduled receive timeout after calling RAIL_ScheduleRx(). If
 * RAIL_ScheduleRxConfig_t::rxTransitionEndSchedule was passed as false,
 * this event will occur unless the receive is aborted (due to a call to
 * RAIL_Idle() or a scheduler preemption, for instance). If
 * RAIL_ScheduleRxConfig_t::rxTransitionEndSchedule was passed as true,
 * any of the \ref RAIL_EVENTS_RX_COMPLETION events occurring will also cause
 * this event not to occur, since the scheduled receive will end with the
 * transition at the end of the packet. However, if the application has not
 * enabled the specific \ref RAIL_EVENTS_RX_COMPLETION event which implicitly
 * ended the scheduled receive, this event will be posted instead.
 */
#define RAIL_EVENT_RX_SCHEDULED_RX_END (1ULL << RAIL_EVENT_RX_SCHEDULED_RX_END_SHIFT)

/**
 * Occurs when start of a scheduled receive is missed
 *
 * This can occur if the radio is put to sleep and not woken up with enough time
 * to configure the scheduled receive event.
 */
#define RAIL_EVENT_RX_SCHEDULED_RX_MISSED (1ULL << RAIL_EVENT_RX_SCHEDULED_RX_MISSED_SHIFT)

/**
 * Occurs when a receive is aborted during filtering with
 * \ref RAIL_RX_PACKET_ABORT_FORMAT or after filtering with
 * \ref RAIL_RX_PACKET_ABORT_ABORTED for reasons other than address
 * filtering mismatch (which triggers \ref RAIL_EVENT_RX_ADDRESS_FILTERED
 * instead).
 *
 * For EFR32 parts, this event includes CRC errors, block decoding errors,
 * illegal frame length, and other RAIL built-in protocol-specific packet
 * content errors -- when detected during filtering. (When such errors
 * are detected after filtering, they're signaled as \ref
 * RAIL_EVENT_RX_FRAME_ERROR instead.) It also includes application or
 * multiprotocol scheduler aborting a receive after filtering has passed.
 */
#define RAIL_EVENT_RX_PACKET_ABORTED (1ULL << RAIL_EVENT_RX_PACKET_ABORTED_SHIFT)

/**
 * Occurs when the packet has passed any configured address and frame
 * filtering options.
 *
 * This event will only occur between the start of the
 * packet, indicated by \ref RAIL_EVENT_RX_SYNC1_DETECT or
 * \ref RAIL_EVENT_RX_SYNC2_DETECT and one of the events in the
 * \ref RAIL_EVENTS_RX_COMPLETION mask. It will always occur before or
 * concurrently with \ref RAIL_EVENT_RX_PACKET_RECEIVED. If IEEE 802.15.4 frame
 * and address filtering are enabled, this event will occur immediately after
 * destination address filtering.
 */
#define RAIL_EVENT_RX_FILTER_PASSED (1ULL << RAIL_EVENT_RX_FILTER_PASSED_SHIFT)

/**
 * Occurs when the modem timing is lost.
 *
 * This event can occur multiple times
 * while searching for a packet and is generally used for diagnostic purposes.
 * It can only occur after a \ref RAIL_EVENT_RX_TIMING_DETECT event has
 * already occurred.
 */
#define RAIL_EVENT_RX_TIMING_LOST (1ULL << RAIL_EVENT_RX_TIMING_LOST_SHIFT)

/**
 * Occurs when the modem timing is detected.
 *
 * This event can occur multiple times
 * while searching for a packet and is generally used for diagnostic purposes.
 */
#define RAIL_EVENT_RX_TIMING_DETECT (1ULL << RAIL_EVENT_RX_TIMING_DETECT_SHIFT)

/**
 * Occurs when RX Channel Hopping is enabled and channel hopping finishes
 * receiving on the last channel in its sequence.
 *
 * The intent behind this event
 * is to allow the user to keep the radio on for as short a time as possible.
 * That is, once the channel sequence is complete, the application will receive
 * this event and can trigger a sleep/idle until it is necessary to cycle
 * through the channels again. If this event is left on indefinitely and not
 * handled it will likely be a fairly noisy event, as it continues to fire
 * each time the hopping algorithm cycles through the channel sequence.
 */
#define RAIL_EVENT_RX_CHANNEL_HOPPING_COMPLETE (1ULL << RAIL_EVENT_RX_CHANNEL_HOPPING_COMPLETE_SHIFT)

/**
 * Occurs during RX duty cycle mode when the radio finishes its time in
 * receive mode.
 *
 * The application can then trigger a sleep/idle until it
 * needs to listen again.
 */
#define RAIL_EVENT_RX_DUTY_CYCLE_RX_END (1ULL << RAIL_EVENT_RX_DUTY_CYCLE_RX_END_SHIFT)

/**
 * Indicate a Data Request is received when using IEEE 802.15.4
 * functionality.
 *
 * It occurs when the command byte of an incoming ACK-requesting MAC Control
 * frame is for a data request. This callback is called before
 * the packet is fully received to allow the node to have more time to decide
 * whether to indicate a frame is pending in the outgoing ACK. This event only
 * occurs if the RAIL IEEE 802.15.4 functionality is enabled, but will never
 * occur if promiscuous mode is enabled via
 * RAIL_IEEE802154_SetPromiscuousMode().
 *
 * Call RAIL_IEEE802154_GetAddress() to get the source address of the packet.
 */
#define RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND (1ULL << RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND_SHIFT)

/**
 * Indicate a Z-Wave Beam Request relevant to the node was received.
 *
 * This event only occurs if the RAIL Z-Wave functionality is enabled
 * and its \ref RAIL_ZWAVE_OPTION_DETECT_BEAM_FRAMES is enabled.
 * This event is used in lieu of \ref RAIL_EVENT_RX_PACKET_RECEIVED,
 * which is reserved for Z-Wave packets other than Beams.
 *
 * Call RAIL_ZWAVE_GetBeamNodeId() to get the NodeId to which the Beam was
 * targeted, which would be either the broadcast id 0xFF or the node's own
 * single-cast id.
 *
 * @note All Z-Wave Beam requests are generally discarded, triggering
 *   \ref RAIL_EVENT_RX_PACKET_ABORTED.
 */
#define RAIL_EVENT_ZWAVE_BEAM (1ULL << RAIL_EVENT_ZWAVE_BEAM_SHIFT)

/**
 * Indicate a MFM buffer has completely transmitted.
 *
 * This event only occurs if the RAIL MFM functionality is enabled
 * and a MFM buffer has completely transmitted.
 *
 * Following this event, the application can update the MFM buffer
 * that has transmitted to be used for the next transmission.
 */
#define RAIL_EVENT_MFM_TX_BUFFER_DONE (1ULL << RAIL_EVENT_MFM_TX_BUFFER_DONE_SHIFT)

/**
 * Indicate a request for populating Z-Wave LR ACK packet.
 * This event only occurs if the RAIL Z-Wave functionality is enabled.
 *
 * Following this event, the application must call \ref RAIL_ZWAVE_SetLrAckData()
 * to populate noise floor, TX power and receive RSSI fields of the Z-Wave
 * Long Range ACK packet.
 */
#define RAIL_EVENT_ZWAVE_LR_ACK_REQUEST_COMMAND (1ULL << RAIL_EVENT_ZWAVE_LR_ACK_REQUEST_COMMAND_SHIFT)

/**
 * The mask representing all events that determine the end of a received
 * packet.
 *
 * After a \ref RAIL_EVENT_RX_SYNC1_DETECT or a
 * \ref RAIL_EVENT_RX_SYNC2_DETECT,
 * exactly one of the following events will occur. When one of these events
 * occurs, a state transition will take place based on the parameter passed to
 * RAIL_SetRxTransitions(). The RAIL_StateTransitions_t::success transition
 * will be followed only if the \ref RAIL_EVENT_RX_PACKET_RECEIVED event occurs.
 * Any of the other events will trigger the RAIL_StateTransitions_t::error
 * transition.
 */
#define RAIL_EVENTS_RX_COMPLETION (RAIL_EVENT_RX_PACKET_RECEIVED    \
                                   | RAIL_EVENT_RX_PACKET_ABORTED   \
                                   | RAIL_EVENT_RX_FRAME_ERROR      \
                                   | RAIL_EVENT_RX_FIFO_OVERFLOW    \
                                   | RAIL_EVENT_RX_ADDRESS_FILTERED \
                                   | RAIL_EVENT_RX_SCHEDULED_RX_MISSED)

// TX Event Bitmasks

/**
 * Occurs when the number of bytes in the transmit FIFO falls below the
 * configured threshold value.
 *
 * This event does not occur on initialization or after resetting the transmit
 * FIFO with RAIL_ResetFifo().
 *
 * Call RAIL_GetTxFifoSpaceAvailable() to get the
 * number of bytes available in the transmit FIFO at the time of the callback
 * dispatch. When using this event, the threshold should be set via
 * RAIL_SetTxFifoThreshold().
 */
#define RAIL_EVENT_TX_FIFO_ALMOST_EMPTY (1ULL << RAIL_EVENT_TX_FIFO_ALMOST_EMPTY_SHIFT)

/**
 * Occurs after a packet has been transmitted.
 *
 * Call RAIL_GetTxPacketDetails()
 * to get information about the packet that was transmitted.
 * @note RAIL_GetTxPacketDetails() is only valid to call during the time frame
 *   of the RAIL_Config_t::eventsCallback.
 */
#define RAIL_EVENT_TX_PACKET_SENT (1ULL << RAIL_EVENT_TX_PACKET_SENT_SHIFT)

/**
 * Occurs after an ACK packet has been transmitted.
 *
 * Call RAIL_GetTxPacketDetails()
 * to get information about the packet that was transmitted. This event can only occur
 * after calling RAIL_ConfigAutoAck().
 * @note RAIL_GetTxPacketDetails() is only valid to call during the time frame
 *   of the RAIL_Config_t::eventsCallback.
 */
#define RAIL_EVENT_TXACK_PACKET_SENT (1ULL << RAIL_EVENT_TXACK_PACKET_SENT_SHIFT)

/**
 * Occurs when a transmit is aborted by the user.
 *
 * This can happen due to calling RAIL_Idle() or due to a scheduler
 * preemption.
 *
 * @note The Transmit FIFO is left in an indeterminate state and should be
 *    reset prior to reuse for sending a new packet. Contrast this
 *    with \ref RAIL_EVENT_TX_BLOCKED.
 */
#define RAIL_EVENT_TX_ABORTED (1ULL << RAIL_EVENT_TX_ABORTED_SHIFT)

/**
 * Occurs when an ACK transmit is aborted by the user.
 *
 * This event can only
 * occur after calling RAIL_ConfigAutoAck(), which can happen due to calling
 * RAIL_Idle() or due to a scheduler preemption.
 */
#define RAIL_EVENT_TXACK_ABORTED (1ULL << RAIL_EVENT_TXACK_ABORTED_SHIFT)

/**
 * Occurs when a transmit is blocked from occurring because
 * RAIL_EnableTxHoldOff() was called.
 *
 * @note Since the transmit never started, the Transmit FIFO remains intact
 *   after this event -- no packet data was consumed from it. Contrast this
 *   with \ref RAIL_EVENT_TX_ABORTED.
 */
#define RAIL_EVENT_TX_BLOCKED (1ULL << RAIL_EVENT_TX_BLOCKED_SHIFT)

/**
 * Occurs when an ACK transmit is blocked from occurring because
 * RAIL_EnableTxHoldOff() was called.
 *
 * This event can only occur after calling RAIL_ConfigAutoAck().
 */
#define RAIL_EVENT_TXACK_BLOCKED (1ULL << RAIL_EVENT_TXACK_BLOCKED_SHIFT)

/**
 * Occurs when the transmit buffer underflows.
 *
 * This can happen due to the
 * transmitted packet specifying an unintended length based on the current
 * radio configuration or due to RAIL_WriteTxFifo() calls not keeping up with
 * the transmit rate if the entire packet isn't loaded at once.
 *
 * @note The Transmit FIFO is left in an indeterminate state and should be
 *    reset prior to reuse for sending a new packet. Contrast this
 *    with \ref RAIL_EVENT_TX_BLOCKED.
 */
#define RAIL_EVENT_TX_UNDERFLOW (1ULL << RAIL_EVENT_TX_UNDERFLOW_SHIFT)

/**
 * Occurs when the ACK transmit buffer underflows.
 *
 * This can happen due to the
 * transmitted packet specifying an unintended length based on the current
 * radio configuration or due to RAIL_WriteAutoAckFifo() not being called at
 * all before an ACK transmit.
 *
 * This event can only occur after calling RAIL_ConfigAutoAck().
 */
#define RAIL_EVENT_TXACK_UNDERFLOW (1ULL << RAIL_EVENT_TXACK_UNDERFLOW_SHIFT)

/**
 * Occurs when Carrier Sense Multiple Access (CSMA) or Listen Before Talk (LBT)
 * succeeds.
 *
 * This event can only happen after calling RAIL_StartCcaCsmaTx() or
 * RAIL_StartCcaLbtTx().
 */
#define RAIL_EVENT_TX_CHANNEL_CLEAR (1ULL << RAIL_EVENT_TX_CHANNEL_CLEAR_SHIFT)

/**
 * Occurs when Carrier Sense Multiple Access (CSMA) or Listen Before Talk (LBT)
 * fails.
 *
 * This event can only happen after calling RAIL_StartCcaCsmaTx() or
 * RAIL_StartCcaLbtTx().
 *
 * @note Since the transmit never started, the Transmit FIFO remains intact
 *   after this event -- no packet data was consumed from it.
 */
#define RAIL_EVENT_TX_CHANNEL_BUSY (1ULL << RAIL_EVENT_TX_CHANNEL_BUSY_SHIFT)

/**
 * Occurs during CSMA or LBT when an individual Clear Channel Assessment (CCA)
 * check fails, but there are more tries needed before the overall operation
 * completes.
 *
 * This event can occur multiple times based on the configuration
 * of the ongoing CSMA or LBT transmission. It can only happen after
 * calling RAIL_StartCcaCsmaTx() or RAIL_StartCcaLbtTx().
 */
#define RAIL_EVENT_TX_CCA_RETRY (1ULL << RAIL_EVENT_TX_CCA_RETRY_SHIFT)

/**
 * Occurs when the receiver is activated to perform a Clear Channel Assessment
 * (CCA) check.
 *
 * This event generally precedes the actual start of a CCA check by roughly
 * the \ref RAIL_StateTiming_t::idleToRx time (subject to
 * \ref RAIL_MINIMUM_TRANSITION_US).  It can
 * occur multiple times based on the configuration of the ongoing CSMA or LBT
 * transmission. It can only happen after calling RAIL_StartCcaCsmaTx()
 * or RAIL_StartCcaLbtTx().
 */
#define RAIL_EVENT_TX_START_CCA (1ULL << RAIL_EVENT_TX_START_CCA_SHIFT)

/**
 * Occurs when the radio starts transmitting a normal packet on the air.
 *
 * A start-of-transmit timestamp is captured for this event. It can be
 * retrieved by calling \ref RAIL_GetTxTimePreambleStart() passing \ref
 * RAIL_TX_STARTED_BYTES for its totalPacketBytes parameter.
 *
 * @note This event does not apply to ACK transmits. Currently there
 *   is no equivalent event or timestamp captured for the start of an
 *   ACK transmit.
 */
#define RAIL_EVENT_TX_STARTED (1ULL << RAIL_EVENT_TX_STARTED_SHIFT)

/**
 * A value to pass as \ref RAIL_GetTxTimePreambleStart() totalPacketBytes
 * parameter to retrieve the \ref RAIL_EVENT_TX_STARTED timestamp.
 */
#define RAIL_TX_STARTED_BYTES 0U

/**
 * Occurs when the start of a scheduled transmit is missed
 *
 * This can occur if the radio is put to sleep and not woken up with enough time
 * to configure the scheduled transmit event.
 *
 * @note Since the transmit never started, the Transmit FIFO remains intact
 *   after this event -- no packet data was consumed from it.
 */
#define RAIL_EVENT_TX_SCHEDULED_TX_MISSED (1ULL << RAIL_EVENT_TX_SCHEDULED_TX_MISSED_SHIFT)

/**
 * A mask representing all events that determine the end of a transmitted
 * packet. After a \ref RAIL_STATUS_NO_ERROR return value
 * from one of the transmit functions, exactly one of the following
 * events will occur. When one of these events occurs, a state transition
 * takes place based on the parameter passed to RAIL_SetTxTransitions().
 * The RAIL_StateTransitions_t::success transition will be followed only
 * if the \ref RAIL_EVENT_TX_PACKET_SENT event occurs. Any of the other
 * events will trigger the RAIL_StateTransitions_t::error transition.
 */
#define RAIL_EVENTS_TX_COMPLETION (RAIL_EVENT_TX_PACKET_SENT    \
                                   | RAIL_EVENT_TX_ABORTED      \
                                   | RAIL_EVENT_TX_BLOCKED      \
                                   | RAIL_EVENT_TX_UNDERFLOW    \
                                   | RAIL_EVENT_TX_CHANNEL_BUSY \
                                   | RAIL_EVENT_TX_SCHEDULED_TX_MISSED)

/**
 * A mask representing all events that determine the end of a transmitted
 * ACK packet. After an ACK-requesting receive, exactly one of the
 * following events will occur. When one of these events occurs, a state
 * transition takes place based on the RAIL_AutoAckConfig_t::rxTransitions
 * passed to RAIL_ConfigAutoAck(). The receive transitions are used because the
 * transmitted ACK packet is considered a part of the ACK-requesting received
 * packet. The RAIL_StateTransitions_t::success transition will be followed
 * only if the \ref RAIL_EVENT_TXACK_PACKET_SENT event occurs. Any of the other
 * events will trigger the RAIL_StateTransitions_t::error transition.
 */
#define RAIL_EVENTS_TXACK_COMPLETION (RAIL_EVENT_TXACK_PACKET_SENT \
                                      | RAIL_EVENT_TXACK_ABORTED   \
                                      | RAIL_EVENT_TXACK_BLOCKED   \
                                      | RAIL_EVENT_TXACK_UNDERFLOW)

// Scheduler Event Bitmasks

/**
 * Occurs when the scheduler switches away from this configuration.
 *
 * This event will occur in dynamic multiprotocol scenarios each
 * time a protocol is shutting down. When it does occur, it will be
 * the only event passed to RAIL_Config_t::eventsCallback. Therefore,
 * to optimize protocol switch time, this event should be handled
 * among the first in that callback, and then the application can return
 * immediately.
 *
 * @note: To minimize protocol switch time, Silicon Labs recommends this event
 *        event being turned off unless it is used.
 */
#define RAIL_EVENT_CONFIG_UNSCHEDULED (1ULL << RAIL_EVENT_CONFIG_UNSCHEDULED_SHIFT)

/**
 * Occurs when the scheduler switches to this configuration.
 *
 * This event will occur in dynamic multiprotocol scenarios each time
 * a protocol is starting up. When it does occur, it will
 * be the only event passed to RAIL_Config_t::eventsCallback. Therefore, in
 * order to optimize protocol switch time, this event should be handled among
 * the first in that callback, and then the application can return immediately.
 *
 * @note: To minimize protocol switch time, Silicon Labs recommends this event
 *        event being turned off unless it is used.
 */
#define RAIL_EVENT_CONFIG_SCHEDULED (1ULL << RAIL_EVENT_CONFIG_SCHEDULED_SHIFT)

/**
 * Occurs when the scheduler has a status to report.
 *
 * The exact status can be found with RAIL_GetSchedulerStatus().
 * See \ref RAIL_SchedulerStatus_t for more details. When this event
 * does occur, it will be the only event passed to RAIL_Config_t::eventsCallback.
 * Therefore, to optimize protocol switch time, this event should
 * be handled among the first in that callback, and then the application
 * can return immediately.
 *
 * @note RAIL_GetSchedulerStatus() is only valid to call during the time frame
 *   of the RAIL_Config_t::eventsCallback.
 *
 * @note: To minimize protocol switch time, Silicon Labs recommends this event
 *        event being turned off unless it is used.
 */
#define RAIL_EVENT_SCHEDULER_STATUS (1ULL << RAIL_EVENT_SCHEDULER_STATUS_SHIFT)

// Other Event Bitmasks

/**
 * Occurs when the application needs to run a calibration, as
 * determined by the RAIL library.
 *
 * The application determines the opportune time to call RAIL_Calibrate().
 */
#define RAIL_EVENT_CAL_NEEDED (1ULL << RAIL_EVENT_CAL_NEEDED_SHIFT)

/**
 * Occurs when RF energy is sensed from the radio. This event can be used as
 * an alternative to the callback passed as \ref RAIL_RfSense_CallbackPtr_t.
 *
 * Alternatively, the application can poll using \ref RAIL_IsRfSensed().
 *
 * @note This event will not occur when waking up from EM4. Prefer
 *   \ref RAIL_IsRfSensed() when waking from EM4.
 */
#define RAIL_EVENT_RF_SENSED (1ULL << RAIL_EVENT_RF_SENSED_SHIFT)

/**
 * Occurs when PA protection circuit kicks in.
 */
#define RAIL_EVENT_PA_PROTECTION (1ULL << RAIL_EVENT_PA_PROTECTION_SHIFT)

/**
 * Occurs after enabling the signal detection using \ref RAIL_BLE_EnableSignalDetection
 * or \ref RAIL_IEEE802154_EnableSignalDetection when a signal is detected.
 * This is only used on platforms that support signal identifier, where
 * \ref RAIL_BLE_SUPPORTS_SIGNAL_IDENTIFIER or
 * \ref RAIL_IEEE802154_SUPPORTS_SIGNAL_IDENTIFIER is true.
 */
#define RAIL_EVENT_SIGNAL_DETECTED (1ULL << RAIL_EVENT_SIGNAL_DETECTED_SHIFT)

#ifndef DOXYGEN_SHOULD_SKIP_THIS
/**
 * Occurs at the beginning of Wi-SUN's mode switch process, i.e. after the switch to
 * the new channel has been performed. Applies to RX node that receives the mode switch packet.
 */
#define RAIL_EVENT_IEEE802154_MODESWITCH_START (1ULL << RAIL_EVENT_IEEE802154_MODESWITCH_START_SHIFT)

/**
 * Occurs at the end of the mode switch process, i.e. after the second switch back
 * to the base channel has been performed. Applies to RX node that receives the mode switch packet.
 */
#define RAIL_EVENT_IEEE802154_MODESWITCH_END (1ULL << RAIL_EVENT_IEEE802154_MODESWITCH_END_SHIFT)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * Occurs when the sampled RSSI is above the threshold set by
 * \ref RAIL_SetRssiDetectThreshold().
 */
#define RAIL_EVENT_DETECT_RSSI_THRESHOLD (1ULL << RAIL_EVENT_DETECT_RSSI_THRESHOLD_SHIFT)

/** A value representing all possible events */
#define RAIL_EVENTS_ALL 0xFFFFFFFFFFFFFFFFULL

/** @} */ // end of group Events

/**
 * @addtogroup PA Power Amplifier (PA)
 * @ingroup Transmit
 * @{
 */

/**
 * The transmit power in deci-dBm units (e.g., 4.5 dBm -> 45 deci-dBm). These
 * values are used by the conversion functions to convert a \ref
 * RAIL_TxPowerLevel_t to deci-dBm for the application consumption. On EFR32,
 * they can range from \ref RAIL_TX_POWER_MIN to \ref RAIL_TX_POWER_MAX.
 */
typedef int16_t RAIL_TxPower_t;

/** The maximum valid value for a \ref RAIL_TxPower_t. */
#define RAIL_TX_POWER_MAX ((RAIL_TxPower_t)0x7FFF)
/** The minimum valid value for a \ref RAIL_TxPower_t. */
#define RAIL_TX_POWER_MIN ((RAIL_TxPower_t)0x8000)

/** The maximum power in deci-dBm the curve supports */
#define RAIL_TX_POWER_CURVE_DEFAULT_MAX ((RAIL_TxPower_t)200)
/** The increment step in deci-dBm for calculating power level*/
#define RAIL_TX_POWER_CURVE_DEFAULT_INCREMENT ((RAIL_TxPower_t)40)

/// mV are used for all TX power voltage values.
/// TX power voltages take and return voltages multiplied by this factor.
#define RAIL_TX_POWER_VOLTAGE_SCALING_FACTOR 1000

/// deci-dBm are used for all TX power dBm values.
/// All dBm inputs to TX power functions take dBm power times this factor.
#define RAIL_TX_POWER_DBM_SCALING_FACTOR 10

/** @} */ // PA Power Amplifier (PA)

/******************************************************************************
 * Radio Configuration Structures
 *****************************************************************************/
/**
 * @addtogroup Radio_Configuration
 * @{
 */

/**
 * @struct RAIL_FrameType_t
 * @brief Configures if there is a frame type in your frame and the lengths of
 * each frame. The number of bits set in the mask determines the number of
 * elements in frameLen. A maximum of 8 different frame types may be specified.
 */
typedef struct RAIL_FrameType {
  /**
   * A pointer to an array of frame lengths for each frame type. The length of this
   * array should be equal to the number of frame types. The array that
   * frameLen points to should not change location or be modified.
   */
  uint16_t *frameLen;
  /**
   * Zero-indexed location of the byte containing the frame type field.
   */
  uint8_t offset;
  /**
   * A bitmask of the frame type field, which determines a number of frames expected
   * based on the number of bits set. No more than 3 bits can be set in the mask and
   * they must be contiguous ones. For example, if the highest three bits of the byte
   * specified by offset constitute the frame type, then mask should be 0xE0,
   * which has 3 bits set, indicating 8 possible frame types.
   */
  uint8_t mask;
  /**
   * A bitmask that marks if each frame is valid or should be filtered. Frame type
   * 0 corresponds to the lowest bit in isValid. If the frame is filtered, a
   * RAIL_EVENT_RX_PACKET_ABORTED will be raised.
   */
  uint8_t isValid;
  /**
   * A bitmask that marks if each frame should have the address filter applied.
   * Frame type 0 corresponds to the least significant bit in addressFilter.
   */
  uint8_t addressFilter;
} RAIL_FrameType_t;

/**
 * @def RAIL_SETFIXEDLENGTH_INVALID
 * @brief An invalid return value when calling RAIL_SetFixedLength().
 *
 * An invalid return value when calling RAIL_SetFixedLength() while the radio is
 * not in fixed-length mode.
 */
#define RAIL_SETFIXEDLENGTH_INVALID (0xFFFFU)

/**
 * @struct RAIL_ChannelConfigEntryAttr_t
 * @brief A channel configuration entry attribute structure. Items listed
 *  are designed to be altered and updated during run-time.
 */
typedef struct RAIL_ChannelConfigEntryAttr RAIL_ChannelConfigEntryAttr_t;

/**
 * @enum RAIL_ChannelConfigEntryType_t
 * @brief Define if the channel support using concurrent PHY during channel
 * hopping. RAIL_RX_CHANNEL_HOPPING_MODE_CONC and RAIL_RX_CHANNEL_HOPPING_MODE_VT
 * can only be used if the channel supports it.
 */
RAIL_ENUM(RAIL_ChannelConfigEntryType_t) {
  RAIL_CH_TYPE_NORMAL, /**< Not a concurrent PHY. */
  RAIL_CH_TYPE_CONC_BASE, /**< Base concurrent PHY. */
  RAIL_CH_TYPE_CONC_VIRTUAL, /**< Virtual concurrent PHY. */
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_CH_TYPE_NORMAL       ((RAIL_ChannelConfigEntryType_t) RAIL_CH_TYPE_NORMAL)
#define RAIL_CH_TYPE_CONC_BASE    ((RAIL_ChannelConfigEntryType_t) RAIL_CH_TYPE_CONC_BASE)
#define RAIL_CH_TYPE_CONC_VIRTUAL ((RAIL_ChannelConfigEntryType_t) RAIL_CH_TYPE_CONC_VIRTUAL)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * @def RADIO_CONFIG_ENABLE_CONC_PHY
 * @brief Indicates this version of RAIL supports concurrent PHY information in
 * radio configurator output. Needed for backwards compatibility.
 */
#define RADIO_CONFIG_ENABLE_CONC_PHY 1

/**
 * @def RADIO_CONFIG_ENABLE_STACK_INFO
 * @brief Indicates this version of RAIL supports stack info feature in
 * radio configurator output. Needed for backwards compatibility.
 */
#define RADIO_CONFIG_ENABLE_STACK_INFO

/**
 * @struct RAIL_ChannelConfigEntry_t
 * @brief A channel configuration entry structure, which defines a channel range
 *   and parameters across which a corresponding radio configuration is valid.
 *
 * operating frequency = baseFrequency
 *   + channelSpacing * (channel - physicalChannelOffset);
 */
typedef struct RAIL_ChannelConfigEntry {
  const uint32_t *phyConfigDeltaAdd; /**< The minimum radio configuration to apply to the base
                                          configuration for this channel set. */
  uint32_t baseFrequency; /**< A base frequency in Hz of this channel set. */
  uint32_t channelSpacing; /**< A channel spacing in Hz of this channel set. */
  uint16_t physicalChannelOffset; /**< The offset to subtract from the logical
                                       channel to align them with the zero
                                       based physical channels which are
                                       relative to baseFrequency.
                                       (i.e., By default ch 0 = base freq, but
                                       if offset = 11, ch 11 = base freq.) */
  uint16_t channelNumberStart; /**< The first valid RAIL channel number for this
                                    channel set. */
  uint16_t channelNumberEnd; /**< The last valid RAIL channel number for this
                                  channel set. */
  RAIL_TxPower_t maxPower; /**< The maximum power allowed in this channel set. */
  RAIL_ChannelConfigEntryAttr_t *attr; /**< A pointer to a structure containing
                                            attributes specific to this
                                            channel set. */
  RAIL_ChannelConfigEntryType_t entryType; /**< Indicates channel config type. */
  uint8_t reserved[3]; /**< to align to 32-bit boundary. */
  const uint8_t *stackInfo; /**< Array containing information according to the protocolId value,
                                 first byte of this array.
                                 The first 2 fields are common to all protocols and accessible by RAIL,
                                 others are ignored by RAIL and only used by the application.
                                 Common fields are listed in RAIL_StackInfoCommon_t. */
} RAIL_ChannelConfigEntry_t;

/// @struct RAIL_ChannelConfig_t
/// @brief A channel configuration structure, which defines the channel meaning
///   when a channel number is passed into a RAIL function, e.g., RAIL_StartTx()
///   and RAIL_StartRx().
///
/// A RAIL_ChannelConfig_t structure defines the channel scheme that an
/// application uses when registered in RAIL_ConfigChannels().
///
/// These are a few examples of different channel configurations:
/// @code{.c}
/// // 21 channels starting at 2.45 GHz with channel spacing of 1 MHz
/// // ... generated by Simplicity Studio (i.e., rail_config.c) ...
/// const uint32_t generated[] = { ... };
/// RAIL_ChannelConfigEntryAttr_t generated_entryAttr = { ... };
/// const RAIL_ChannelConfigEntry_t generated_channels[] = {
///   {
///     .phyConfigDeltaAdd = NULL, // Add this to default configuration for this entry
///     .baseFrequency = 2450000000,
///     .channelSpacing = 1000000,
///     .physicalChannelOffset = 0,
///     .channelNumberStart = 0,
///     .channelNumberEnd = 20,
///     .maxPower = RAIL_TX_POWER_MAX,
///     .attr = &generated_entryAttr
///   },
/// };
/// const RAIL_ChannelConfig_t generated_channelConfig = {
///   .phyConfigBase = generated, // Default radio configuration for all entries
///   .phyConfigDeltaSubtract = NULL, // Subtract this to restore the default configuration
///   .configs = generated_channels,
///   .length = 1 // There are this many channel configuration entries
/// };
/// const RAIL_ChannelConfig_t *channelConfigs[] = {
///   &generated_channelConfig,
///   NULL
/// };
/// // ... in main code ...
/// // Associate a specific channel configuration with a particular RAIL instance.
/// RAIL_ConfigChannels(railHandle, channelConfigs[0]);
///
/// // 4 nonlinear channels
/// // ... in rail_config.c ...
/// const uint32_t generated[] = { ... };
/// RAIL_ChannelConfigEntryAttr_t generated_entryAttr = { ... };
/// const RAIL_ChannelConfigEntry_t generated_channels[] = {
///   {
///     .phyConfigDeltaAdd = NULL, // Add this to default configuration for this entry
///     .baseFrequency = 910123456,
///     .channelSpacing = 0,
///     .physicalChannelOffset = 0,
///     .channelNumberStart = 0,
///     .channelNumberEnd = 0,
///     .maxPower = RAIL_TX_POWER_MAX,
///     .attr = &generated_entryAttr
///   },
///   {
///     .phyConfigDeltaAdd = NULL,
///     .baseFrequency = 911654789,
///     .channelSpacing = 0,
///     .physicalChannelOffset = 0, // Since ch spacing = 0, offset can be 0
///     .channelNumberStart = 1,
///     .channelNumberEnd = 1,
///     .maxPower = RAIL_TX_POWER_MAX,
///     .attr = &generated_entryAttr
///   },
///   {
///     .phyConfigDeltaAdd = NULL,
///     .baseFrequency = 912321456,
///     .channelSpacing = 100000,
///     .physicalChannelOffset = 2, // Since ch spacing != 0, offset = 2
///     .channelNumberStart = 2, // ch 2 = baseFrequency
///     .channelNumberEnd = 2,
///     .maxPower = RAIL_TX_POWER_MAX,
///     .attr = &generated_entryAttr
///   },
///   {
///     .phyConfigDeltaAdd = NULL,
///     .baseFrequency = 913147852,
///     .channelSpacing = 0,
///     .physicalChannelOffset = 0,
///     .channelNumberStart = 3,
///     .channelNumberEnd = 3,
///     .maxPower = RAIL_TX_POWER_MAX,
///     .attr = &generated_entryAttr
///   },
/// };
/// const RAIL_ChannelConfig_t generated_channelConfig = {
///   .phyConfigBase = generated, // Default radio configuration for all entries
///   .phyConfigDeltaSubtract = NULL, // Subtract this to restore the default configuration
///   .configs = generated_channels,
///   .length = 4 // There are this many channel configuration entries
/// };
/// const RAIL_ChannelConfig_t *channelConfigs[] = {
///   &generated_channelConfig,
///   NULL
/// };
/// // ... in main code ...
/// // Associate a specific channel configuration with a particular RAIL instance.
/// RAIL_ConfigChannels(railHandle, channelConfigs[0]);
///
/// // Multiple radio configurations
/// // ... in rail_config.c ...
/// const uint32_t generated0[] = { ... };
/// RAIL_ChannelConfigEntryAttr_t generated0_entryAttr = { ... };
/// const RAIL_ChannelConfigEntry_t generated0_channels[] = {
///   {
///     .phyConfigDeltaAdd = NULL, // Add this to the default configuration for this entry
///     .baseFrequency = 2450000000,
///     .channelSpacing = 1000000,
///     .physicalChannelOffset = 0,
///     .channelNumberStart = 0,
///     .channelNumberEnd = 20,
///     .maxPower = RAIL_TX_POWER_MAX,
///     .attr = &generated0_entryAttr
///   },
/// };
/// const RAIL_ChannelConfig_t generated0_channelConfig = {
///   .phyConfigBase = generated0, // Default radio configuration for all entries
///   .phyConfigDeltaSubtract = NULL, // Subtract this to restore default configuration
///   .configs = generated0_channels,
///   .length = 1 // There are this many channel configuration entries
/// };
/// const uint32_t generated1[] = { ... };
/// RAIL_ChannelConfigEntryAttr_t generated1_entryAttr = { ... };
/// const RAIL_ChannelConfigEntry_t generated1_channels[] = {
///   {
///     .phyConfigDeltaAdd = NULL,
///     .baseFrequency = 2450000000,
///     .channelSpacing = 1000000,
///     .physicalChannelOffset = 0,
///     .channelNumberStart = 0,
///     .channelNumberEnd = 20,
///     .maxPower = -100, // Use this entry when TX power <= -10dBm
///     .attr = &generated1_entryAttr
///   },
///   {
///     .phyConfigDeltaAdd = NULL,
///     .baseFrequency = 2450000000,
///     .channelSpacing = 1000000,
///     .physicalChannelOffset = 0,
///     .channelNumberStart = 0,
///     .channelNumberEnd = 20,
///     .maxPower = 15, // Use this entry when TX power > -10dBm
///                     // and TX power <= 1.5dBm
///     .attr = &generated1_entryAttr
///   },
///   {
///     .phyConfigDeltaAdd = NULL,
///     .baseFrequency = 2450000000,
///     .channelSpacing = 1000000,
///     .physicalChannelOffset = 0,
///     .channelNumberStart = 0,
///     .channelNumberEnd = 20,
///     .maxPower = RAIL_TX_POWER_MAX, // Use this entry when TX power > 1.5dBm
///     .attr = &generated1_entryAttr
///   },
/// };
/// const RAIL_ChannelConfig_t generated1_channelConfig = {
///   .phyConfigBase = generated1,
///   .phyConfigDeltaSubtract = NULL,
///   .configs = generated1_channels,
///   .length = 3
/// };
/// const uint32_t generated2[] = { ... };
/// RAIL_ChannelConfigEntryAttr_t generated2_entryAttr = { ... };
/// const RAIL_ChannelConfigEntry_t generated2_channels[] = {
///   {
///     .phyConfigDeltaAdd = NULL,
///     .baseFrequency = 2450000000,
///     .channelSpacing = 1000000,
///     .physicalChannelOffset = 0,
///     .channelNumberStart = 0,
///     .channelNumberEnd = 20,
///     .maxPower = RAIL_TX_POWER_MAX,
///     .attr = &generated2_entryAttr
///   },
/// };
/// const RAIL_ChannelConfig_t generated2_channelConfig = {
///   .phyConfigBase = generated2,
///   .phyConfigDeltaSubtract = NULL,
///   .configs = generated2_channels,
///   .length = 1
/// };
/// const RAIL_ChannelConfig_t *channelConfigs[] = {
///   &generated0_channelConfig,
///   &generated1_channelConfig,
///   &generated2_channelConfig,
///   NULL
/// };
/// // ... in main code ...
/// // Create a unique RAIL handle for each unique channel configuration.
/// railHandle0 = RAIL_Init(&railCfg0, &RAILCb_RfReady0);
/// railHandle1 = RAIL_Init(&railCfg1, &RAILCb_RfReady1);
/// railHandle2 = RAIL_Init(&railCfg2, &RAILCb_RfReady2);
/// // Associate each channel configuration with its corresponding RAIL handle.
/// RAIL_ConfigChannels(railHandle0, channelConfigs[0]);
/// RAIL_ConfigChannels(railHandle1, channelConfigs[1]);
/// RAIL_ConfigChannels(railHandle2, channelConfigs[2]);
/// // Use a RAIL handle and channel to access the desired channel configuration entry.
/// RAIL_SetTxPowerDbm(railHandle1, 100); // set 10.0 dBm TX power
/// RAIL_StartRx(railHandle1, 0, &schedInfo); // RX using generated1_channels[2]
/// RAIL_SetTxPowerDbm(railHandle1, 0); // set 0 dBm TX power
/// RAIL_StartRx(railHandle1, 0, &schedInfo); // RX using generated1_channels[1]
/// RAIL_StartRx(railHandle2, 0, &schedInfo); // RX using generated2_channels[0]
/// @endcode

typedef struct RAIL_ChannelConfig {
  const uint32_t *phyConfigBase; /**< Base radio configuration for the corresponding
                                      channel configuration entries. */
  const uint32_t *phyConfigDeltaSubtract; /**< Minimum radio configuration to restore
                                               channel entries back to base
                                               configuration. */
  const RAIL_ChannelConfigEntry_t *configs; /**< Pointer to an array of
                                                 RAIL_ChannelConfigEntry_t
                                                 entries. */
  uint32_t length; /**< Number of RAIL_ChannelConfigEntry_t entries. */
  uint32_t signature; /**< Signature for this structure. Only used on modules. */
} RAIL_ChannelConfig_t;

/**
 * @struct RAIL_ChannelMetadata_t
 * @brief Container for individual channel metadata.
 */
typedef struct RAIL_ChannelMetadata {
  uint16_t channel; /**< Channel number */
  uint16_t reserved; /**< Word alignment */
  uint32_t frequency; /**< Channel frequency, in Hz */
} RAIL_ChannelMetadata_t;

/**
 * @struct RAIL_StackInfoCommon_t
 * @brief StackInfo fields common to all protocols.
 */
typedef struct RAIL_StackInfoCommon {
  uint8_t protocolId; /**< Same as RAIL_PtiProtocol_t */
  uint8_t phyId;  /**< PHY Id depending on the protocol_id value */
} RAIL_StackInfoCommon_t;

/**
 * @typedef RAIL_RadioConfigChangedCallback_t
 * @brief A pointer to a function called whenever a radio configuration change occurs.
 *
 * @param[in] railHandle A handle for RAIL instance.
 * @param[in] entry The radio configuration entry being changed to.
 */
typedef void (*RAIL_RadioConfigChangedCallback_t)(RAIL_Handle_t railHandle,
                                                  const RAIL_ChannelConfigEntry_t *entry);

/** @} */ // end of group Radio_Configuration

/******************************************************************************
 * Packet Trace Interface (PTI) Structures
 *****************************************************************************/
/**
 * @addtogroup PTI
 * @{
 */

/**
 * @enum RAIL_PtiProtocol_t
 * @brief The protocol that RAIL outputs via the Packet Trace Interface (PTI).
 */
RAIL_ENUM(RAIL_PtiProtocol_t) {
  RAIL_PTI_PROTOCOL_CUSTOM = 0, /**< PTI output for a custom protocol. */
  RAIL_PTI_PROTOCOL_THREAD = 2, /**< PTI output for the Thread protocol. */
  RAIL_PTI_PROTOCOL_BLE = 3, /**< PTI output for the Bluetooth Smart protocol. */
  RAIL_PTI_PROTOCOL_CONNECT = 4, /**< PTI output for the Connect protocol. */
  RAIL_PTI_PROTOCOL_ZIGBEE = 5, /**< PTI output for the Zigbee protocol. */
  RAIL_PTI_PROTOCOL_ZWAVE = 6, /**< PTI output for the Z-Wave protocol. */
  RAIL_PTI_PROTOCOL_WISUN = 7, /**< PTI output for the Wi-SUN protocol. */
  RAIL_PTI_PROTOCOL_802154 = 8, /**< PTI output for a custom protocol using a built-in 802.15.4 radio config. */
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_PTI_PROTOCOL_CUSTOM  ((RAIL_PtiProtocol_t) RAIL_PTI_PROTOCOL_CUSTOM)
#define RAIL_PTI_PROTOCOL_THREAD  ((RAIL_PtiProtocol_t) RAIL_PTI_PROTOCOL_THREAD)
#define RAIL_PTI_PROTOCOL_BLE     ((RAIL_PtiProtocol_t) RAIL_PTI_PROTOCOL_BLE)
#define RAIL_PTI_PROTOCOL_CONNECT ((RAIL_PtiProtocol_t) RAIL_PTI_PROTOCOL_CONNECT)
#define RAIL_PTI_PROTOCOL_ZIGBEE  ((RAIL_PtiProtocol_t) RAIL_PTI_PROTOCOL_ZIGBEE)
#define RAIL_PTI_PROTOCOL_ZWAVE   ((RAIL_PtiProtocol_t) RAIL_PTI_PROTOCOL_ZWAVE)
#define RAIL_PTI_PROTOCOL_802154  ((RAIL_PtiProtocol_t) RAIL_PTI_PROTOCOL_802154)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/** @} */ // end of group PTI

/******************************************************************************
 * Data Management Structures
 *****************************************************************************/
/**
 * @addtogroup Data_Management
 * @{
 */

/**
 * @enum RAIL_TxDataSource_t
 * @brief Transmit data sources supported by RAIL.
 */
RAIL_ENUM(RAIL_TxDataSource_t) {
  TX_PACKET_DATA, /**< Uses the frame hardware to packetize data. */
  /** Uses the multi-level frequency modulation data.
   * @note This is only supported on devices where \ref RAIL_SUPPORTS_MFM
   *   or \ref RAIL_SupportsMfm() are true.
   * @note This feature cannot be used with built-in protocols (802.15.4, BLE,
   *   Z-Wave).
   */
  TX_MFM_DATA,
  /** A count of the choices in this enumeration. */
  RAIL_TX_DATA_SOURCE_COUNT // Must be last
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define TX_PACKET_DATA            ((RAIL_TxDataSource_t) TX_PACKET_DATA)
#define TX_MFM_DATA               ((RAIL_TxDataSource_t) TX_MFM_DATA)
#define RAIL_TX_DATA_SOURCE_COUNT ((RAIL_TxDataSource_t) RAIL_TX_DATA_SOURCE_COUNT)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * @enum RAIL_RxDataSource_t
 * @brief Receive data sources supported by RAIL.
 *
 * @note Data sources other than \ref RX_PACKET_DATA require use of
 *   \ref RAIL_DataMethod_t::FIFO_MODE.
 */
RAIL_ENUM(RAIL_RxDataSource_t) {
  RX_PACKET_DATA, /**< Uses the frame hardware to packetize data. */
  RX_DEMOD_DATA, /**< Gets 8-bit data output from the demodulator. */
  RX_IQDATA_FILTLSB, /**< Gets lower 16 bits of I/Q data provided to the
                          demodulator. */
  RX_IQDATA_FILTMSB, /**< Gets highest 16 bits of I/Q data provided to the
                         demodulator. */
  RX_DIRECT_MODE_DATA, /**< Gets RX direct mode data output from the demodulator.
                            Only supported if
                            \ref RAIL_SUPPORTS_RX_DIRECT_MODE_DATA_TO_FIFO
                            is true. */
  /** A count of the choices in this enumeration. */
  RAIL_RX_DATA_SOURCE_COUNT // Must be last
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RX_PACKET_DATA            ((RAIL_RxDataSource_t) RX_PACKET_DATA)
#define RX_DEMOD_DATA             ((RAIL_RxDataSource_t) RX_DEMOD_DATA)
#define RX_IQDATA_FILTLSB         ((RAIL_RxDataSource_t) RX_IQDATA_FILTLSB)
#define RX_IQDATA_FILTMSB         ((RAIL_RxDataSource_t) RX_IQDATA_FILTMSB)
#define RX_DIRECT_MODE_DATA       ((RAIL_RxDataSource_t) RX_DIRECT_MODE_DATA)
#define RAIL_RX_DATA_SOURCE_COUNT ((RAIL_RxDataSource_t) RAIL_RX_DATA_SOURCE_COUNT)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * @enum RAIL_DataMethod_t
 * @brief Methods for the application to provide and retrieve data from RAIL.
 *
 * For Transmit the distinction between \ref RAIL_DataMethod_t::PACKET_MODE
 * and \ref RAIL_DataMethod_t::FIFO_MODE has become more cosmetic than
 * functional, as the RAIL_WriteTxFifo() and RAIL_SetTxFifoThreshold() APIs
 * and related \ref RAIL_EVENT_TX_FIFO_ALMOST_EMPTY event can be used in
 * either mode. For Receive the distinction is functionally important because
 * in \ref RAIL_DataMethod_t::PACKET_MODE rollback occurs automatically for
 * unsuccessfully-received packets (\ref RAIL_RxPacketStatus_t ABORT statuses),
 * flushing their data. In \ref RAIL_DataMethod_t::FIFO_MODE rollback is
 * prevented, leaving the data from unsuccessfully-received packets in the
 * receive FIFO for the application to deal with. This allows for packets
 * larger than the receive FIFO size where automatic rollback would corrupt
 * the receive FIFO.
 */
RAIL_ENUM(RAIL_DataMethod_t) {
  PACKET_MODE, /**< Packet-based data method. */
  FIFO_MODE, /**< FIFO-based data method. */
  /** A count of the choices in this enumeration. */
  RAIL_DATA_METHOD_COUNT // Must be last
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define PACKET_MODE            ((RAIL_DataMethod_t) PACKET_MODE)
#define FIFO_MODE              ((RAIL_DataMethod_t) FIFO_MODE)
#define RAIL_DATA_METHOD_COUNT ((RAIL_DataMethod_t) RAIL_DATA_METHOD_COUNT)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * @def RAIL_FIFO_THRESHOLD_DISABLED
 * @brief A FIFO threshold value that disables the threshold.
 */
#define RAIL_FIFO_THRESHOLD_DISABLED 0xFFFFU

/**
 * @struct RAIL_DataConfig_t
 * @brief RAIL data configuration structure
 *
 * Select the transmit/receive data sources and the
 * method the application uses to provide/retrieve data from RAIL.
 */
typedef struct {
  RAIL_TxDataSource_t txSource; /**< Source of TX Data. */
  RAIL_RxDataSource_t rxSource; /**< Source of RX Data. */
  RAIL_DataMethod_t txMethod; /**< Method of providing transmit data. */
  RAIL_DataMethod_t rxMethod; /**< Method of retrieving receive data. */
} RAIL_DataConfig_t;

/** @} */ // end of group Data Management

/******************************************************************************
 * State Transitions
 *****************************************************************************/
/**
 * @addtogroup State_Transitions
 * @{
 */

/**
 * @enum RAIL_RadioState_t
 * @brief The state of the radio.
 */
RAIL_ENUM(RAIL_RadioState_t) {
  RAIL_RF_STATE_INACTIVE = 0u,       /**< Radio is inactive. */
  RAIL_RF_STATE_ACTIVE = (1u << 0),  /**< Radio is either idle or,
                                          in combination with the RX and TX states,
                                          receiving or transmitting a frame.*/
  RAIL_RF_STATE_RX = (1u << 1),      /**< Radio is in receive. */
  RAIL_RF_STATE_TX = (1u << 2),      /**< Radio is in transmit. */
  RAIL_RF_STATE_IDLE = (RAIL_RF_STATE_ACTIVE),  /**< Radio is idle. */
  /** Radio is actively receiving a frame. */
  RAIL_RF_STATE_RX_ACTIVE = (RAIL_RF_STATE_RX | RAIL_RF_STATE_ACTIVE),
  /** Radio is actively transmitting a frame. */
  RAIL_RF_STATE_TX_ACTIVE = (RAIL_RF_STATE_TX | RAIL_RF_STATE_ACTIVE)
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_RF_STATE_INACTIVE  ((RAIL_RadioState_t) RAIL_RF_STATE_INACTIVE)
#define RAIL_RF_STATE_ACTIVE    ((RAIL_RadioState_t) RAIL_RF_STATE_ACTIVE)
#define RAIL_RF_STATE_RX        ((RAIL_RadioState_t) RAIL_RF_STATE_RX)
#define RAIL_RF_STATE_TX        ((RAIL_RadioState_t) RAIL_RF_STATE_TX)
#define RAIL_RF_STATE_IDLE      ((RAIL_RadioState_t) RAIL_RF_STATE_IDLE)
#define RAIL_RF_STATE_RX_ACTIVE ((RAIL_RadioState_t) RAIL_RF_STATE_RX_ACTIVE)
#define RAIL_RF_STATE_TX_ACTIVE ((RAIL_RadioState_t) RAIL_RF_STATE_TX_ACTIVE)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * @struct RAIL_StateTransitions_t
 * @brief Used to specify radio states to transition to on success or failure.
 */
typedef struct RAIL_StateTransitions {
  /**
   * Indicate the state the radio should return to after a successful action.
   */
  RAIL_RadioState_t success;
  /**
   * Indicate the state the radio should return to after an error.
   */
  RAIL_RadioState_t error;
} RAIL_StateTransitions_t;

/**
 * @enum RAIL_RadioStateDetail_t
 * @brief The detailed state of the radio.
 *
 * The three radio state bits \ref RAIL_RF_STATE_DETAIL_IDLE_STATE, \ref
 * RAIL_RF_STATE_DETAIL_RX_STATE, and \ref RAIL_RF_STATE_DETAIL_TX_STATE
 * comprise a set of mutually exclusive core radio states. Only one (or none)
 * of these bits can be set at a time. Otherwise, the value is invalid.
 *
 * The precise meaning of each of these three core bits, when set, depends on
 * the value of the two bits \ref RAIL_RF_STATE_DETAIL_TRANSITION and \ref
 * RAIL_RF_STATE_DETAIL_ACTIVE. When \ref RAIL_RF_STATE_DETAIL_TRANSITION is
 * set, the radio is transitioning into the core radio state corresponding
 * to the set state bit. When it is clear, the radio is already in the core
 * radio state that corresponds to the set state bit. When \ref
 * RAIL_RF_STATE_DETAIL_ACTIVE is set, the radio is actively transmitting or
 * receiving. When it is clear, the radio is not actively transmitting or receiving.
 * This bit will always be clear when \ref RAIL_RF_STATE_DETAIL_IDLE_STATE is
 * set, and will always be set when \ref RAIL_RF_STATE_DETAIL_TX_STATE is set.
 * Otherwise, the value is invalid.
 *
 * The bit \ref RAIL_RF_STATE_DETAIL_NO_FRAMES is set if the radio is currently
 * operating with frame detection disabled, and clear otherwise. The bit \ref
 * RAIL_RF_STATE_DETAIL_LBT_SHIFT is set if an LBT/CSMA operation
 * (e.g., performing CCA) is currently ongoing, and clear otherwise.
 */
RAIL_ENUM(RAIL_RadioStateDetail_t) {
  /** Shift position of \ref RAIL_RF_STATE_DETAIL_IDLE_STATE bit */
  RAIL_RF_STATE_DETAIL_IDLE_STATE_SHIFT = 0u,
  /** Shift position of \ref RAIL_RF_STATE_DETAIL_RX_STATE bit */
  RAIL_RF_STATE_DETAIL_RX_STATE_SHIFT,
  /** Shift position of \ref RAIL_RF_STATE_DETAIL_TX_STATE bit */
  RAIL_RF_STATE_DETAIL_TX_STATE_SHIFT,
  /** Shift position of \ref RAIL_RF_STATE_DETAIL_TRANSITION bit */
  RAIL_RF_STATE_DETAIL_TRANSITION_SHIFT,
  /** Shift position of \ref RAIL_RF_STATE_DETAIL_ACTIVE bit */
  RAIL_RF_STATE_DETAIL_ACTIVE_SHIFT,
  /** Shift position of \ref RAIL_RF_STATE_DETAIL_NO_FRAMES bit */
  RAIL_RF_STATE_DETAIL_NO_FRAMES_SHIFT,
  /** Shift position of \ref RAIL_RF_STATE_DETAIL_LBT bit */
  RAIL_RF_STATE_DETAIL_LBT_SHIFT,
};

/** Radio is inactive. */
#define RAIL_RF_STATE_DETAIL_INACTIVE (0U)
/** Radio is in or headed to the idle state. */
#define RAIL_RF_STATE_DETAIL_IDLE_STATE (1U << RAIL_RF_STATE_DETAIL_IDLE_STATE_SHIFT)
/** Radio is in or headed to the receive state. */
#define RAIL_RF_STATE_DETAIL_RX_STATE (1U << RAIL_RF_STATE_DETAIL_RX_STATE_SHIFT)
/** Radio is in or headed to the transmit state. */
#define RAIL_RF_STATE_DETAIL_TX_STATE (1U << RAIL_RF_STATE_DETAIL_TX_STATE_SHIFT)
/** Radio is headed to the idle, receive, or transmit state. */
#define RAIL_RF_STATE_DETAIL_TRANSITION (1U << RAIL_RF_STATE_DETAIL_TRANSITION_SHIFT)
/** Radio is actively transmitting or receiving. */
#define RAIL_RF_STATE_DETAIL_ACTIVE (1U << RAIL_RF_STATE_DETAIL_ACTIVE_SHIFT)
/** Radio has frame detect disabled. */
#define RAIL_RF_STATE_DETAIL_NO_FRAMES (1U << RAIL_RF_STATE_DETAIL_NO_FRAMES_SHIFT)
/** LBT/CSMA operation is currently ongoing. */
#define RAIL_RF_STATE_DETAIL_LBT (1U << RAIL_RF_STATE_DETAIL_LBT_SHIFT)
/** Mask for core radio state bits. */
#define RAIL_RF_STATE_DETAIL_CORE_STATE_MASK (RAIL_RF_STATE_DETAIL_IDLE_STATE \
                                              | RAIL_RF_STATE_DETAIL_RX_STATE \
                                              | RAIL_RF_STATE_DETAIL_TX_STATE)

/**
 * @enum RAIL_IdleMode_t
 * @brief An enumeration for the different types of supported idle modes. These
 *   vary how quickly and destructively they put the radio into idle.
 */
RAIL_ENUM(RAIL_IdleMode_t) {
  /**
   * Idle the radio by turning off receive and canceling any future scheduled
   * receive or transmit operations. It does not abort a receive or
   * transmit in progress.
   */
  RAIL_IDLE,
  /**
   * Idle the radio by turning off receive and any scheduled events. It
   * also aborts any receive, transmit, or scheduled events in progress.
   */
  RAIL_IDLE_ABORT,
  /**
   * Force the radio into a shutdown mode by stopping whatever state is in
   * progress. This is a more destructive shutdown than \ref RAIL_IDLE or
   * \ref RAIL_IDLE_ABORT and can be useful in certain situations when directed
   * by the support team or for debugging. Note that this method may corrupt
   * receive and transmit buffers so it requires a more thorough cleanup
   * and any held packets will be lost.
   */
  RAIL_IDLE_FORCE_SHUTDOWN,
  /**
   * Similar to the \ref RAIL_IDLE_FORCE_SHUTDOWN command, however, it will also
   * clear any pending RAIL events related to receive and transmit.
   */
  RAIL_IDLE_FORCE_SHUTDOWN_CLEAR_FLAGS,
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_IDLE                            ((RAIL_IdleMode_t) RAIL_IDLE)
#define RAIL_IDLE_ABORT                      ((RAIL_IdleMode_t) RAIL_IDLE_ABORT)
#define RAIL_IDLE_FORCE_SHUTDOWN             ((RAIL_IdleMode_t) RAIL_IDLE_FORCE_SHUTDOWN)
#define RAIL_IDLE_FORCE_SHUTDOWN_CLEAR_FLAGS ((RAIL_IdleMode_t) RAIL_IDLE_FORCE_SHUTDOWN_CLEAR_FLAGS)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/** @} */ // end of group State_Transitions

/******************************************************************************
 * TX Channel Hopping
 *****************************************************************************/
/**
 * @addtogroup Tx_Channel_Hopping TX Channel Hopping
 * @{
 */

/**
 * @struct RAIL_TxChannelHoppingConfigEntry_t
 * @brief Structure that represents one of the channels that is part of a
 *   \ref RAIL_TxChannelHoppingConfig_t sequence of channels used in
 *   channel hopping.
 */
typedef struct RAIL_TxChannelHoppingConfigEntry {
  /**
   * The channel number to be used for this entry in the channel hopping
   * sequence. If this is an invalid channel for the current PHY, the
   * call to \ref RAIL_SetNextTxRepeat() will fail.
   */
  uint16_t channel;
  /**
   * Pad bytes reserved for future use and currently ignored.
   */
  uint8_t reserved[2];
  /**
   * Idle time in microseconds to wait before transmitting on the channel
   * indicated by this entry.
   */
  uint32_t delay;
} RAIL_TxChannelHoppingConfigEntry_t;

/**
 * @struct RAIL_TxChannelHoppingConfig_t
 * @brief Wrapper struct that will contain the sequence of
 *   \ref RAIL_TxChannelHoppingConfigEntry_t that represents the channel
 *   sequence to use during TX Channel Hopping.
 */
typedef struct RAIL_TxChannelHoppingConfig {
  /**
   * Pointer to contiguous global read-write memory that will be used
   * by RAIL to store channel hopping information throughout its operation.
   * It need not be initialized and applications should never write
   * data anywhere in this buffer.
   *
   * @note the size of this buffer must be at least as large as
   * 3 + 30 * numberOfChannels, plus the sum of the sizes of the
   * radioConfigDeltaAdd's of the required channels, plus the size of the
   * radioConfigDeltaSubtract. In the case that one channel
   * appears two or more times in your channel sequence
   * (e.g., 1, 2, 3, 2), you must account for the radio configuration
   * size that number of times (i.e., need to count channel 2's
   * radio configuration size twice for the given example). The overall
   * 3 words and 30 words per channel needed in this buffer are
   * for internal use to the library.
   */
  uint32_t *buffer;
  /**
   * This parameter must be set to the length of the buffer array. This way,
   * during configuration, the software can confirm it's writing within the
   * range of the buffer. The configuration API will return an error
   * if bufferLength is insufficient.
   */
  uint16_t bufferLength;
  /**
   * The number of channels in the channel hopping sequence, which is the
   * number of elements in the array that entries points to.
   */
  uint8_t numberOfChannels;
  /**
   * Pad byte reserved for future use and currently ignored.
   */
  uint8_t reserved;
  /**
   * A pointer to the first element of an array of \ref
   * RAIL_TxChannelHoppingConfigEntry_t that represents the channels
   * used during channel hopping. The length of this array must be
   * numberOfChannels.
   */
  RAIL_TxChannelHoppingConfigEntry_t *entries;
} RAIL_TxChannelHoppingConfig_t;

/** @} */ // end of group Tx_Channel_Hopping

/******************************************************************************
 * TX/RX Configuration Structures
 *****************************************************************************/
/**
 * @addtogroup Transmit
 * @{
 */

/**
 * @enum RAIL_StopMode_t
 * @brief Stop radio operation options bit mask
 */
RAIL_ENUM(RAIL_StopMode_t) {
  /** Shift position of \ref RAIL_STOP_MODE_ACTIVE bit */
  RAIL_STOP_MODE_ACTIVE_SHIFT = 0,
  /** Shift position of \ref RAIL_STOP_MODE_PENDING bit */
  RAIL_STOP_MODE_PENDING_SHIFT = 1
};

/** Do not stop any radio operations */
#define RAIL_STOP_MODES_NONE   (0U)
/** Stop active radio operations only */
#define RAIL_STOP_MODE_ACTIVE (1U << RAIL_STOP_MODE_ACTIVE_SHIFT)
/** Stop pending radio operations */
#define RAIL_STOP_MODE_PENDING (1U << RAIL_STOP_MODE_PENDING_SHIFT)
/** Stop all radio operations */
#define RAIL_STOP_MODES_ALL (0xFFU)

/**
 * @enum RAIL_TxOptions_t
 * @brief Transmit options, in reality a bitmask.
 */
RAIL_ENUM_GENERIC(RAIL_TxOptions_t, uint32_t) {
  /** Shift position of \ref RAIL_TX_OPTION_WAIT_FOR_ACK bit */
  RAIL_TX_OPTION_WAIT_FOR_ACK_SHIFT = 0,
  /** Shift position of \ref RAIL_TX_OPTION_REMOVE_CRC bit */
  RAIL_TX_OPTION_REMOVE_CRC_SHIFT,
  /** Shift position of \ref RAIL_TX_OPTION_SYNC_WORD_ID bit */
  RAIL_TX_OPTION_SYNC_WORD_ID_SHIFT,
  /** Shift position of \ref RAIL_TX_OPTION_ANTENNA0 bit */
  RAIL_TX_OPTION_ANTENNA0_SHIFT,
  /** Shift position of \ref RAIL_TX_OPTION_ANTENNA1 bit */
  RAIL_TX_OPTION_ANTENNA1_SHIFT,
  /** Shift position of \ref RAIL_TX_OPTION_ALT_PREAMBLE_LEN bit */
  RAIL_TX_OPTION_ALT_PREAMBLE_LEN_SHIFT,
  /** Shift position of \ref RAIL_TX_OPTION_CCA_PEAK_RSSI bit */
  RAIL_TX_OPTION_CCA_PEAK_RSSI_SHIFT,
  /** Shift position of \ref RAIL_TX_OPTION_CCA_ONLY bit */
  RAIL_TX_OPTION_CCA_ONLY_SHIFT,
  /** Shift position of \ref RAIL_TX_OPTION_RESEND bit */
  RAIL_TX_OPTION_RESEND_SHIFT,
  /** A count of the choices in this enumeration. */
  RAIL_TX_OPTIONS_COUNT // Must be last
};

/** A value representing no options enabled. */
#define RAIL_TX_OPTIONS_NONE 0UL
/** All options disabled by default. This is the fastest TX option to apply. */
#define RAIL_TX_OPTIONS_DEFAULT RAIL_TX_OPTIONS_NONE
/**
 * An option to configure whether or not the TXing node will listen for an ACK.
 * If this is false, the isAck flag in RAIL_RxPacketDetails_t of a received
 * packet will always be false.
 */
#define RAIL_TX_OPTION_WAIT_FOR_ACK (1UL << RAIL_TX_OPTION_WAIT_FOR_ACK_SHIFT)
/**
 * An option to remove CRC bytes from TX packets. To receive packets when the
 * sender has this option set true, set \ref RAIL_RX_OPTION_IGNORE_CRC_ERRORS
 * on the receive side.
 */
#define RAIL_TX_OPTION_REMOVE_CRC (1UL << RAIL_TX_OPTION_REMOVE_CRC_SHIFT)
/**
 * An option to select which sync word to send (0 or 1). This does not set the
 * actual sync words, it just picks which of the two will be sent with the
 * outgoing packet. Setting to 0 will transmit on SYNC1. Setting to 1 will
 * transmit on SYNC2.
 *
 * @note There are a few special radio configurations (e.g. BLE Viterbi) that do
 * not support transmitting different sync words.
 */
#define RAIL_TX_OPTION_SYNC_WORD_ID (1UL << RAIL_TX_OPTION_SYNC_WORD_ID_SHIFT)
/**
 * An option to select antenna 0 for transmission. If the antenna selection
 * option is not set or if both antenna options are set, then the transmit
 * will occur on either antenna depending on the last receive or transmit
 * selection. This option is only valid on platforms that support
 * \ref Antenna_Control and have been configured via RAIL_ConfigAntenna().
 *
 * @note These TX antenna options do not control the antenna used for
 *   \ref Auto_Ack transmissions, which always occur on the same antenna
 *   used to receive the packet being acknowledged.
 */
#define RAIL_TX_OPTION_ANTENNA0 (1UL << RAIL_TX_OPTION_ANTENNA0_SHIFT)
/**
 * An option to select antenna 1 for transmission. If the antenna selection
 * option is not set or if both antenna options are set, then the transmit
 * will occur on either antenna depending on the last receive or transmit
 * selection. This option is only valid on platforms that support
 * \ref Antenna_Control and have been configured via RAIL_ConfigAntenna().
 *
 * @note These TX antenna options do not control the antenna used for
 *   \ref Auto_Ack transmissions, which always occur on the same antenna
 *   used to receive the packet being acknowledged.
 */
#define RAIL_TX_OPTION_ANTENNA1 (1UL << RAIL_TX_OPTION_ANTENNA1_SHIFT)
/**
 * An option to dynamically set an alternate preamble length for the
 * transmission. If this option is not set, the pre-configured
 * channel preamble length will be used.
 */
#define RAIL_TX_OPTION_ALT_PREAMBLE_LEN (1UL << RAIL_TX_OPTION_ALT_PREAMBLE_LEN_SHIFT)
/**
 * An option to use peak rather than average RSSI energy detected during
 * CSMA's RAIL_CsmaConfig_t::ccaDuration or LBT's
 * RAIL_LbtConfig_t::lbtDuration to determine whether the channel is clear
 * or busy.
 * @note This option does nothing on platforms like EFR32XG1 that lack
 * support for capturing peak RSSI energy.
 */
#define RAIL_TX_OPTION_CCA_PEAK_RSSI (1UL << RAIL_TX_OPTION_CCA_PEAK_RSSI_SHIFT)
/**
 * An option to only perform the CCA (CSMA/LBT) operation but *not*
 * automatically transmit if the channel is clear.
 *
 * Application can then use the \ref RAIL_EVENT_TX_CHANNEL_CLEAR to
 * initiate transmit manually, e.g., giving it the opportunity to adjust
 * outgoing packet data before the packet goes out.
 *
 * @note Configured state transitions to Rx or Idle are suspended during
 * this CSMA/LBT operation. If packet reception occurs, the radio will
 * return to the state it was in just prior to the CSMA/LBT operation
 * when that reception (including any AutoACK response) is complete.
 */
#define RAIL_TX_OPTION_CCA_ONLY (1UL << RAIL_TX_OPTION_CCA_ONLY_SHIFT)

/**
 * An option to resend packet at the beginning of the Transmit FIFO.
 *
 * The packet to be resent must have been previously provided by
 * \ref RAIL_SetTxFifo() or \ref RAIL_WriteTxFifo() passing true for
 * the latter's reset parameter. It works by setting the
 * transmit FIFO's read offset to the beginning of the FIFO while
 * leaving its write offset intact. For this to work,
 * \ref RAIL_DataConfig_t::txMethod must be RAIL_DataMethod_t::PACKET_MODE
 * (i.e., the packet can't exceed the Transmit FIFO's size), otherwise
 * undefined behavior will result.
 *
 * This option can also be used with \ref RAIL_SetNextTxRepeat() to cause
 * the repeated packet(s) to all be the same as the first.
 */
#define RAIL_TX_OPTION_RESEND (1UL << RAIL_TX_OPTION_RESEND_SHIFT)

/** A value representing all possible options. */
#define RAIL_TX_OPTIONS_ALL 0xFFFFFFFFUL

/**
 * @struct RAIL_TxPacketDetails_t
 * @brief Detailed information requested about the packet that was just,
 *   or is currently being, transmitted.
 */
typedef struct RAIL_TxPacketDetails {
  /**
   * The timestamp of the transmitted packet in the RAIL timebase,
   * filled in by RAIL_GetTxPacketDetails().
   */
  RAIL_PacketTimeStamp_t timeSent;
  /**
   * Indicate whether the transmitted packet was an automatic ACK. In a generic
   * sense, an automatic ACK is defined as a packet sent in response to a
   * received ACK-requesting frame when auto-ACK is enabled. In a protocol
   * specific sense this definition may be more or less restrictive to match the
   * specification and you should refer to that protocol's documentation.
   */
  bool isAck;
} RAIL_TxPacketDetails_t;

/**
 * @enum RAIL_ScheduledTxDuringRx_t
 * @brief Enumerates the possible outcomes of what will occur if a
 *   scheduled TX ends up firing during RX. Because RX and TX can't
 *   happen at the same time, it is up to the user how the TX should be
 *   handled. This enumeration is passed into RAIL_StartScheduledTx()
 *   as part of \ref RAIL_ScheduleTxConfig_t.
 */
RAIL_ENUM(RAIL_ScheduledTxDuringRx_t) {
  /**
   * The scheduled TX will be postponed until RX completes and then sent.
   */
  RAIL_SCHEDULED_TX_DURING_RX_POSTPONE_TX,
  /**
   * The scheduled TX will be aborted and a
   * \ref RAIL_EVENT_TX_BLOCKED event will fire.
   */
  RAIL_SCHEDULED_TX_DURING_RX_ABORT_TX,
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_SCHEDULED_TX_DURING_RX_POSTPONE_TX ((RAIL_ScheduledTxDuringRx_t) RAIL_SCHEDULED_TX_DURING_RX_POSTPONE_TX)
#define RAIL_SCHEDULED_TX_DURING_RX_ABORT_TX    ((RAIL_ScheduledTxDuringRx_t) RAIL_SCHEDULED_TX_DURING_RX_ABORT_TX)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * @struct RAIL_ScheduleTxConfig_t
 * @brief A configuration structure for a scheduled transmit.
 */
typedef struct RAIL_ScheduleTxConfig {
  /**
   * The time when to transmit this packet. The exact interpretation of
   * this value depends on the mode specified below.
   */
  RAIL_Time_t when;
  /**
   * The type of delay. See the \ref RAIL_TimeMode_t documentation for
   * more information. Be sure to use \ref RAIL_TIME_ABSOLUTE delays for
   * time-critical protocols.
   */
  RAIL_TimeMode_t mode;
  /**
   * Indicate which action to take with a scheduled TX if it occurs during RX.
   * See \ref RAIL_ScheduledTxDuringRx_t structure for more information on
   * potential options.
   */
  RAIL_ScheduledTxDuringRx_t txDuringRx;
} RAIL_ScheduleTxConfig_t;

/**
 * @def RAIL_MAX_LBT_TRIES
 * @brief The maximum number of LBT/CSMA retries supported.
 */
#define RAIL_MAX_LBT_TRIES      (15U)

/**
 * @def RAIL_MAX_CSMA_EXPONENT
 * @brief The maximum power-of-2 exponent for CSMA backoffs.
 */
#define RAIL_MAX_CSMA_EXPONENT  (8U)

///
/// @struct RAIL_CsmaConfig_t
/// @brief A configuration structure for the CSMA transmit algorithm.
///
/// One of RAIL's schemes for polite spectrum access is an implementation of
/// a Carrier-Sense Multiple Access (CSMA) algorithm based on IEEE 802.15.4
/// (unslotted).
/// \n In pseudo-code it works like this, showing relevant event notifications:
/// @code{.c}
/// // Return true to transmit packet, false to not transmit packet.
/// bool performCsma(const RAIL_CsmaConfig_t *csmaConfig)
/// {
///   bool isFixedBackoff = ((csmaConfig->csmaMinBoExp == 0)
///                          && (csmaConfig->csmaMaxBoExp == 0));
///   int backoffExp = csmaConfig->csmaMinBoExp; // Initial backoff exponent
///   int backoffMultiplier = 1; // Assume fixed backoff
///   int try;
///
///   // Special-case tries == 0 to transmit immediately without backoff+CCA
///   if (csmaConfig->csmaTries == 0) {
///     return true;
///   }
///
///   // Start overall timeout if specified:
///   if (csmaConfig->csmaTimeout > 0) {
///     StartAbortTimer(csmaConfig->csmaTimeout, RAIL_EVENT_TX_CHANNEL_BUSY);
///     // If timeout occurs, abort and signal the indicated event.
///   }
///
///   for (try = 0; try < csmaConfig->csmaTries; try++) {
///     if (try > 0) {
///       signalEvent(RAIL_EVENT_TX_CCA_RETRY);
///     }
///     // Determine the backoff multipler for this try:
///     if (isFixedBackoff) {
///       // backoffMultiplier already set to 1 for fixed backoff
///     } else {
///       // Start with the backoff exponent for this try:
///       if (try > 0) {
///         backoffExp++;
///         if (backoffExp > csmaConfig->csmaMaxBoExp) {
///           backoffExp = csmaConfig->csmaMaxBoExp;
///         }
///       }
///       // Pick random multiplier between 0 and 2^backoffExp - 1 inclusive:
///       backoffMultiplier = pickRandomInteger(0, (1 << backoffExp) - 1);
///     }
///     // Perform the backoff:
///     delayMicroseconds(backoffMultiplier * csmaConfig->ccaBackoff);
///     // Perform the Clear-Channel Assessment (CCA):
///     // Channel is considered busy if radio is actively receiving or
///     // transmitting, or the average energy detected across duration
///     // is above the threshold.
///     signalEvent(RAIL_EVENT_TX_START_CCA);
///     if (performCca(csmaConfig->ccaDuration, csmaConfig->ccaThreshold)) {
///       // CCA (and CSMA) success: Transmit after RX-to-TX turnaround
///       StopAbortTimer();
///       signalEvent(RAIL_EVENT_TX_CHANNEL_CLEAR);
///       return true;
///     } else {
///       // CCA failed: loop to try again, or exit if out of tries
///     }
///   }
///   // Overall CSMA failure: Don't transmit
///   StopAbortTimer();
///   signalEvent(RAIL_EVENT_TX_CHANNEL_BUSY);
///   return false;
/// }
/// @endcode
///
typedef struct RAIL_CsmaConfig {
  /**
   * The minimum (starting) exponent for CSMA random backoff (2^exp - 1).
   * It can range from 0 to \ref RAIL_MAX_CSMA_EXPONENT.
   *
   * @warning On EFR32, due to a hardware limitation, this can only be 0
   *   if \ref csmaMaxBoExp is also 0 specifying a non-random fixed backoff.
   *   \ref RAIL_STATUS_INVALID_PARAMETER will result otherwise.
   *   If you really want CSMA's first iteration to have no backoff prior to
   *   CCA, with subsequent iterations having random backoff as the exponent
   *   is increased, you must do a fixed backoff of 0 operation first
   *   (\ref csmaMinBoExp = 0, \ref csmaMaxBoExp = 0, \ref ccaBackoff = 0,
   *   \ref csmaTries = 1), and if that fails (\ref RAIL_EVENT_TX_CHANNEL_BUSY),
   *   follow up with a random backoff operation starting at \ref csmaMinBoExp
   *   = 1 for the remaining iterations.
   */
  uint8_t  csmaMinBoExp;
  /**
   * The maximum exponent for CSMA random backoff (2^exp - 1).
   * It can range from 0 to \ref RAIL_MAX_CSMA_EXPONENT and must be greater
   * than or equal to \ref csmaMinBoExp.
   * \n If both exponents are 0, a non-random fixed backoff of \ref ccaBackoff
   * duration results.
   */
  uint8_t  csmaMaxBoExp;
  /**
   * The number of backoff-then-CCA iterations that can fail before reporting
   * \ref RAIL_EVENT_TX_CHANNEL_BUSY. Typically ranges from 1 to \ref
   * RAIL_MAX_LBT_TRIES; higher values are disallowed. A value 0 always
   * transmits immediately without performing CSMA, similar to calling
   * RAIL_StartTx().
   */
  uint8_t  csmaTries;
  /**
   * The CCA RSSI threshold, in dBm, above which the channel is
   * considered 'busy'.
   */
  int8_t   ccaThreshold;
  /**
   * The backoff unit period in RAIL's microsecond time base. It is
   * multiplied by the random backoff exponential controlled by \ref
   * csmaMinBoExp and \ref csmaMaxBoExp to determine the overall backoff
   * period. For random backoffs, any value above 511 microseconds will
   * be truncated. For fixed backoffs it can go up to 65535 microseconds.
   */
  uint16_t ccaBackoff;
  /**
   * The minimum desired CCA check duration in microseconds.
   *
   * @note Depending on the radio configuration, due to hardware constraints,
   *   the actual duration may be longer. Also, if the requested duration
   *   is too large for the radio to accommodate, RAIL_StartCcaCsmaTx()
   *   will fail returning \ref RAIL_STATUS_INVALID_PARAMETER.
   */
  uint16_t ccaDuration;
  /**
   * An overall timeout, in RAIL's microsecond time base, for the operation.
   * If the transmission doesn't start before this timeout expires, the
   * transmission will fail with \ref RAIL_EVENT_TX_CHANNEL_BUSY.
   * A value 0 means no timeout is imposed.
   */
  RAIL_Time_t csmaTimeout;
} RAIL_CsmaConfig_t;

/**
 * @def RAIL_CSMA_CONFIG_802_15_4_2003_2p4_GHz_OQPSK_CSMA
 * @brief RAIL_CsmaConfig_t initializer configuring CSMA per IEEE 802.15.4-2003
 *   on 2.4 GHz OSPSK, commonly used by ZigBee.
 */
#define RAIL_CSMA_CONFIG_802_15_4_2003_2p4_GHz_OQPSK_CSMA {                    \
    /* CSMA per 802.15.4-2003 on 2.4 GHz OSPSK, commonly used by ZigBee     */ \
    /* csmaMinBoExp */ 3,   /* 2^3-1 for 0..7 backoffs on 1st try           */ \
    /* csmaMaxBoExp */ 5,   /* 2^5-1 for 0..31 backoffs on 3rd+ tries       */ \
    /* csmaTries    */ 5,   /* 5 tries overall (4 re-tries)                 */ \
    /* ccaThreshold */ -75, /* 10 dB above sensitivity                      */ \
    /* ccaBackoff   */ 320, /* 20 symbols at 16 us/symbol                   */ \
    /* ccaDuration  */ 128, /* 8 symbols at 16 us/symbol                    */ \
    /* csmaTimeout  */ 0,   /* No timeout                                   */ \
}

/**
 * @def RAIL_CSMA_CONFIG_SINGLE_CCA
 * @brief RAIL_CsmaConfig_t initializer configuring a single CCA prior to TX.
 *   It can be used to as a basis for implementing other channel access schemes
 *   with custom backoff delays. Users can override ccaBackoff with a fixed
 *   delay on each use.
 */
#define RAIL_CSMA_CONFIG_SINGLE_CCA {                                          \
    /* Perform a single CCA after 'fixed' delay                             */ \
    /* csmaMinBoExp */ 0,   /* Used for fixed backoff                       */ \
    /* csmaMaxBoExp */ 0,   /* Used for fixed backoff                       */ \
    /* csmaTries    */ 1,   /* Single try                                   */ \
    /* ccaThreshold */ -75, /* Override if not desired choice               */ \
    /* ccaBackoff   */ 0,   /* No backoff (override with fixed value)       */ \
    /* ccaDuration  */ 128, /* Override if not desired length               */ \
    /* csmaTimeout  */ 0,   /* no timeout                                   */ \
}

///
/// @struct RAIL_LbtConfig_t
/// @brief A configuration structure for the LBT transmit algorithm.
///
/// One of RAIL's schemes for polite spectrum access is an implementation of
/// a Listen-Before-Talk (LBT) algorithm, loosely based on ETSI 300 220-1.
/// \n Currently, however, it is constrained by the EFR32's CSMA-oriented hardware
/// so is turned into an equivalent \ref RAIL_CsmaConfig_t configuration and
/// passed to the CSMA engine:
/// @code{.c}
/// if (lbtMaxBoRand == lbtMinBoRand) {
///   // Fixed backoff
///   csmaMinBoExp = csmaMaxBoExp = 0;
///   if (lbtMinBoRand == 0) {
///     ccaBackoff = lbtBackoff;
///   } else {
///     ccaBackoff = lbtMinBoRand * lbtBackoff;
///   }
///   ccaDuration = lbtDuration;
/// } else {
///   // Random backoff: map to random range 0 .. (lbtMaxBoRand - lbtMinBoRand)
///   csmaMinBoExp = csmaMaxBoExp = ceiling(log2(lbtMaxBoRand - lbtMinBoRand));
///   ccaBackoff = round((lbtBackoff * (lbtMaxBoRand - lbtMinBoRand))
///                      / (1 << csmaMinBoExp));
///   ccaDuration = lbtDuration + (lbtMinBoRand * lbtBackoff);
/// }
/// csmaTries    = lbtTries;
/// ccaThreshold = lbtThreshold;
/// csmaTimeout  = lbtTimeout;
/// @endcode
///
typedef struct RAIL_LbtConfig {
  /**
   * The minimum backoff random multiplier.
   */
  uint8_t  lbtMinBoRand;
  /**
   * The maximum backoff random multiplier.
   * It must be greater than or equal to \ref lbtMinBoRand.
   * \n If both backoff multipliers are identical, a non-random fixed backoff
   * of \ref lbtBackoff times the multiplier (minimum 1) duration results.
   */
  uint8_t  lbtMaxBoRand;
  /**
   * The number of LBT iterations that can fail before reporting
   * \ref RAIL_EVENT_TX_CHANNEL_BUSY. Typically ranges from 1 to \ref
   * RAIL_MAX_LBT_TRIES; higher values are disallowed. A value 0 always
   * transmits immediately without performing LBT, similar to calling
   * RAIL_StartTx().
   */
  uint8_t  lbtTries;
  /**
   * The LBT RSSI threshold, in dBm, above which the channel is
   * considered 'busy'.
   */
  int8_t   lbtThreshold;
  /**
   * The backoff unit period, in RAIL's microsecond time base. It is
   * multiplied by the random backoff multiplier controlled by \ref
   * lbtMinBoRand and \ref lbtMaxBoRand to determine the overall backoff
   * period. For random backoffs, any value above 511 microseconds will
   * be truncated. For fixed backoffs, it can go up to 65535 microseconds.
   */
  uint16_t lbtBackoff;
  /**
   * The minimum desired LBT check duration in microseconds.
   *
   * @note Depending on the radio configuration, due to hardware constraints,
   *   the actual duration may be longer. Also, if the requested duration
   *   is too large for the radio to accommodate, RAIL_StartCcaLbtTx()
   *   will fail returning \ref RAIL_STATUS_INVALID_PARAMETER.
   */
  uint16_t lbtDuration;
  /**
   * An overall timeout, in RAIL's microsecond time base, for the operation.
   * If the transmission doesn't start before this timeout expires, the
   * transmission will fail with \ref RAIL_EVENT_TX_CHANNEL_BUSY.
   * This is important for limiting LBT due to LBT's unbounded requirement
   * that if the channel is busy, the next try must wait for the channel to
   * clear. A value 0 means no timeout is imposed.
   */
  RAIL_Time_t lbtTimeout;
} RAIL_LbtConfig_t;

/**
 * @def RAIL_LBT_CONFIG_ETSI_EN_300_220_1_V2_4_1
 * @brief RAIL_LbtConfig_t initializer configuring LBT per ETSI 300 220-1
 *   V2.4.1 for a typical Sub-GHz band. To be practical, users should override
 *   lbtTries and/or lbtTimeout so channel access failure will be reported in a
 *   reasonable time frame rather than the unbounded time frame ETSI defined.
 */
#define RAIL_LBT_CONFIG_ETSI_EN_300_220_1_V2_4_1 {                                \
    /* LBT per ETSI 300 220-1 V2.4.1                                        */    \
    /* LBT time = random backoff of 0-5 ms in .5 ms increments plus 5 ms fixed */ \
    /* lbtMinBoRand */ 0,    /*                                             */    \
    /* lbtMaxBoRand */ 10,   /*                                             */    \
    /* lbtTries     */ RAIL_MAX_LBT_TRIES, /* the maximum supported         */    \
    /* lbtThreshold */ -87,  /*                                             */    \
    /* lbtBackoff   */ 500,  /* 0.5 ms                                      */    \
    /* lbtDuration  */ 5000, /* 5 ms                                        */    \
    /* lbtTimeout   */ 0,    /* No timeout (recommend user override)        */    \
}

/**
 * @struct RAIL_SyncWordConfig_t
 * @brief RAIL sync words and length configuration.
 *
 */
typedef struct RAIL_SyncWordConfig {
  /** Sync word length in bits, between 2 and 32, inclusive.*/
  uint8_t syncWordBits;
  /** Sync Word1*/
  uint32_t syncWord1;
  /** Sync Word2*/
  uint32_t syncWord2;
} RAIL_SyncWordConfig_t;

/** @} */ // end of group Transmit

/**
 * @addtogroup Receive
 * @{
 */

/**
 * @addtogroup Address_Filtering
 * @{
 */

/// A default address filtering match table for configurations that use only one
/// address field. The truth table for address matching is shown below.
///
/// |                | No Match | Address 0 | Address 1 | Address 2 | Address 3 |
/// |----------------|----------|-----------|-----------|-----------|-----------|
/// | __No Match__   |    0     |     1     |     1     |     1     |     1     |
/// | __Address 0__  |    1     |     1     |     1     |     1     |     1     |
/// | __Address 1__  |    1     |     1     |     1     |     1     |     1     |
/// | __Address 2__  |    1     |     1     |     1     |     1     |     1     |
/// | __Address 3__  |    1     |     1     |     1     |     1     |     1     |
///
#define ADDRCONFIG_MATCH_TABLE_SINGLE_FIELD (0x1FFFFFE)
/// A default address filtering match table for configurations that use two
/// address fields and want to match the same index in each. The truth
/// table for address matching is shown below.
///
/// |                | No Match | Address 0 | Address 1 | Address 2 | Address 3 |
/// |----------------|----------|-----------|-----------|-----------|-----------|
/// | __No Match__   |    0     |    0      |    0      |    0      |    0      |
/// | __Address 0__  |    0     |    1      |    0      |    0      |    0      |
/// | __Address 1__  |    0     |    0      |    1      |    0      |    0      |
/// | __Address 2__  |    0     |    0      |    0      |    1      |    0      |
/// | __Address 3__  |    0     |    0      |    0      |    0      |    1      |
#define ADDRCONFIG_MATCH_TABLE_DOUBLE_FIELD (0x1041040)

/// The maximum number of address fields that can be used by the address
/// filtering logic.
#define ADDRCONFIG_MAX_ADDRESS_FIELDS (2)

/**
 * @struct RAIL_AddrConfig_t
 * @brief A structure to configure the address filtering functionality in RAIL.
 */
typedef struct RAIL_AddrConfig {
  /**
   * A list of the start offsets for each field.
   *
   * These offsets are specified relative to the previous field's end.
   * For the first field, it is relative to either the beginning of the packet
   * or the end of the frame type byte if frame type decoding is enabled. If a
   * field is unused, it's offset should be set to 0.
   */
  uint8_t offsets[ADDRCONFIG_MAX_ADDRESS_FIELDS];

  /**
   * A list of the address field sizes.
   *
   * These sizes are specified in bytes from 0 to 8. If you choose a
   * size of 0, this field is effectively disabled.
   */
  uint8_t sizes[ADDRCONFIG_MAX_ADDRESS_FIELDS];

  /**
   * The truth table to determine how the two fields combine to create a match.
   *
   * For detailed information about how this truth table is formed, see the
   * detailed description of \ref Address_Filtering.
   *
   * For simple predefined configurations use the following defines.
   *  - ADDRCONFIG_MATCH_TABLE_SINGLE_FIELD
   *    - For filtering that only uses a single address field.
   *  - ADDRCONFIG_MATCH_TABLE_DOUBLE_FIELD for two field filtering where you
   *    - For filtering that uses two address fields in a configurations where
   *      you want the following logic `((Field0, Index0) && (Field1, Index0))
   *      || ((Field0, Index1) && (Field1, Index1)) || ...`
   */
  uint32_t matchTable;
} RAIL_AddrConfig_t;

/**
 * @brief A bitmask representation of which 4 filters passed for each
 *   \ref ADDRCONFIG_MAX_ADDRESS_FIELDS when filtering has completed
 *   successfully.
 *
 * It's layout is:
 * |  Bit 7 |  Bit 6 |  Bit 5 |  Bit 4 |  Bit 3 |  Bit 2 |  Bit 1 |  Bit 0 |
 * |--------+--------+--------+--------+--------+--------+--------+--------|
 * |   Second Address Field Nibble     |     First Address Field Nibble    |
 * | Addr 3 | Addr 2 | Addr 1 | Addr 0 | Addr 3 | Addr 2 | Addr 1 | Addr 0 |
 * |  match |  match |  match |  match |  match |  match |  match |  match |
 * |--------+--------+--------+--------+--------+--------+--------+--------|
 *
 * @note This information is valid in \ref RAIL_IEEE802154_Address_t on all
 *   platforms, but is only valid in \ref RAIL_RxPacketInfo_t on platforms
 *   where \ref RAIL_SUPPORTS_ADDR_FILTER_MASK is true.
 */
typedef uint8_t RAIL_AddrFilterMask_t;

/** @} */ // end of group Address_Filtering

/**
 * @enum RAIL_RxOptions_t
 * @brief Receive options, in reality a bitmask.
 */
RAIL_ENUM_GENERIC(RAIL_RxOptions_t, uint32_t) {
  /** Shift position of \ref RAIL_RX_OPTION_STORE_CRC bit. */
  RAIL_RX_OPTION_STORE_CRC_SHIFT = 0,
  /** Shift position of \ref RAIL_RX_OPTION_IGNORE_CRC_ERRORS bit. */
  RAIL_RX_OPTION_IGNORE_CRC_ERRORS_SHIFT,
  /** Shift position of \ref RAIL_RX_OPTION_ENABLE_DUALSYNC bit. */
  RAIL_RX_OPTION_ENABLE_DUALSYNC_SHIFT,
  /** Shift position of \ref RAIL_RX_OPTION_TRACK_ABORTED_FRAMES bit. */
  RAIL_RX_OPTION_TRACK_ABORTED_FRAMES_SHIFT,
  /** Shift position of \ref RAIL_RX_OPTION_REMOVE_APPENDED_INFO bit. */
  RAIL_RX_OPTION_REMOVE_APPENDED_INFO_SHIFT,
  /** Shift position of \ref RAIL_RX_OPTION_ANTENNA0 bit. */
  RAIL_RX_OPTION_ANTENNA0_SHIFT,
  /** Shift position of \ref RAIL_RX_OPTION_ANTENNA1 bit. */
  RAIL_RX_OPTION_ANTENNA1_SHIFT,
  /** Shift position of \ref RAIL_RX_OPTION_DISABLE_FRAME_DETECTION bit. */
  RAIL_RX_OPTION_DISABLE_FRAME_DETECTION_SHIFT,
  #ifndef DOXYGEN_SHOULD_SKIP_THIS
  /** Shift position of \ref RAIL_RX_OPTION_SKIP_DC_CAL bit. */
  RAIL_RX_OPTION_SKIP_DC_CAL_SHIFT,
  /** Shift position of \ref RAIL_RX_OPTION_SKIP_SYNTH_CAL bit. */
  RAIL_RX_OPTION_SKIP_SYNTH_CAL_SHIFT,
  #endif //DOXYGEN_SHOULD_SKIP_THIS
};

/** A value representing no options enabled. */
#define RAIL_RX_OPTIONS_NONE 0
/** All options are disabled by default. */
#define RAIL_RX_OPTIONS_DEFAULT RAIL_RX_OPTIONS_NONE

/**
 * An option to configure whether the CRC portion of the packet is included in
 * the packet payload exposed to the app on packet reception.
 * Defaults to false.
 */
#define RAIL_RX_OPTION_STORE_CRC (1UL << RAIL_RX_OPTION_STORE_CRC_SHIFT)
/**
 * An option to configure whether CRC errors will be ignored.
 * If this is set, RX will still be successful, even if
 * the CRC does not pass the check. Defaults to false.
 *
 * @note An expected ACK that fails CRC with this option set
 *   will still be considered the expected ACK, terminating
 *   the RAIL_AutoAckConfig_t::ackTimeout period.
 */
#define RAIL_RX_OPTION_IGNORE_CRC_ERRORS (1UL << RAIL_RX_OPTION_IGNORE_CRC_ERRORS_SHIFT)

/**
 * An option to control which sync words will be accepted. Setting it to
 * 0 (default) will cause the receiver to listen for SYNC1 only. Setting it to
 * 1 causes the receiver to listen for either SYNC1 or SYNC2. RX appended info
 * will contain which sync word was detected. Note, this only affects which
 * sync word(s) are received, but not what each of the sync words actually are.
 * This feature may not be available on some combinations of chips, PHYs, and
 * protocols. Use the compile time symbol RAIL_SUPPORTS_DUAL_SYNC_WORDS or
 * the runtime call RAIL_SupportsDualSyncWords() to check whether the
 * platform supports this feature. Also, DUALSYNC may be incompatible
 * with certain radio configurations. In these cases, setting this bit will
 * be ignored. See the data sheet or support team for more details.
 */
#define RAIL_RX_OPTION_ENABLE_DUALSYNC (1UL << RAIL_RX_OPTION_ENABLE_DUALSYNC_SHIFT)

/**
 * An option to configure whether frames which are aborted during reception
 * should continue to be tracked. Setting this option allows viewing Packet
 * Trace information for frames which get discarded. Defaults to false.
 */
#define RAIL_RX_OPTION_TRACK_ABORTED_FRAMES (1UL << RAIL_RX_OPTION_TRACK_ABORTED_FRAMES_SHIFT)

/**
 * An option to suppress capturing the appended information after
 * received frames. Defaults to false. When suppressed, certain
 * \ref RAIL_RxPacketDetails_t details will not be available for received
 * packets whose \ref RAIL_RxPacketStatus_t is among the RAIL_RX_PACKET_READY_
 * set.
 *
 * @warning This option should be changed only when the radio is idle
 *   and the receive FIFO is empty or has been reset,
 *   otherwise \ref RAIL_GetRxPacketInfo() and \ref RAIL_GetRxPacketDetails()
 *   may think appended info is packet data or vice-versa.
 */
#define RAIL_RX_OPTION_REMOVE_APPENDED_INFO (1UL << RAIL_RX_OPTION_REMOVE_APPENDED_INFO_SHIFT)

/**
 * An option to select the use of antenna 0 during receive (including
 * \ref Auto_Ack receive). If no antenna option is selected, the packet
 * will be received on the last antenna used for receive or transmit.
 * Defaults to false. This option is only valid on platforms that support
 * \ref Antenna_Control and have been configured via RAIL_ConfigAntenna().
 */
#define RAIL_RX_OPTION_ANTENNA0 (1UL << RAIL_RX_OPTION_ANTENNA0_SHIFT)

/**
 * An option to select the use of antenna 1 during receive (including
 * \ref Auto_Ack receive). If no antenna option is selected, the packet
 * will be received on the last antenna used for receive or transmit.
 * Defaults to false. This option is only valid on platforms that support
 * \ref Antenna_Control and have been configured via RAIL_ConfigAntenna().
 */
#define RAIL_RX_OPTION_ANTENNA1 (1UL << RAIL_RX_OPTION_ANTENNA1_SHIFT)

/**
 * An option combination to automatically choose an antenna during receive
 * (including \ref Auto_Ack receive). If both antenna 0 and antenna 1
 * options are set, the radio will dynamically switch between antennas
 * during packet detection and choose the best one for completing the
 * reception. This option is only valid when the antenna diversity
 * field is properly configured via Simplicity Studio.
 * This option is only valid on platforms that support
 * \ref Antenna_Control and have been configured via RAIL_ConfigAntenna().
 */
#define RAIL_RX_OPTION_ANTENNA_AUTO (RAIL_RX_OPTION_ANTENNA0 | RAIL_RX_OPTION_ANTENNA1)

/**
 * An option to disable frame detection. This can be useful for doing energy
 * detection without risking packet reception. Enabling this will abort any
 * frame currently being received in addition to preventing further frames
 * from being received. Defaults to false.
 */
#define RAIL_RX_OPTION_DISABLE_FRAME_DETECTION (1UL << RAIL_RX_OPTION_DISABLE_FRAME_DETECTION_SHIFT)

#ifndef DOXYGEN_SHOULD_SKIP_THIS
/**
 * An option to skip DC calibration when transitioning from RX to RX. This can be
 * useful for reducing the state transition time, but risks impacting
 * receive capability. Enabling this bypasses DC calibration (like
 * \ref RAIL_RX_CHANNEL_HOPPING_OPTION_SKIP_DC_CAL)
 * Defaults to false.
 */
#define RAIL_RX_OPTION_SKIP_DC_CAL (1UL << RAIL_RX_OPTION_SKIP_DC_CAL_SHIFT)

/**
 * An option to skip synth calibration when transitioning from RX to RX. This can
 * be useful for reducing the state transition time, but risks impacting receive
 * capability. Enabling this bypasses synth calibration (like
 * \ref RAIL_RX_CHANNEL_HOPPING_OPTION_SKIP_SYNTH_CAL)
 * Defaults to false.
 */
#define RAIL_RX_OPTION_SKIP_SYNTH_CAL (1U << RAIL_RX_OPTION_SKIP_SYNTH_CAL_SHIFT)
#endif //DOXYGEN_SHOULD_SKIP_THIS

/** A value representing all possible options. */
#define RAIL_RX_OPTIONS_ALL 0xFFFFFFFFUL

/** The value returned by RAIL for an invalid RSSI, in dBm. */
#define RAIL_RSSI_INVALID_DBM     (-128)
/** The value returned by RAIL for an invalid RSSI: in quarter dBm. */
#define RAIL_RSSI_INVALID         ((int16_t)(RAIL_RSSI_INVALID_DBM * 4))
/** The lowest RSSI value returned by RAIL: in quarter dBm. */
#define RAIL_RSSI_LOWEST          ((int16_t)(RAIL_RSSI_INVALID + 1))

/** Maximum absolute value for RSSI offset */
#define RAIL_RSSI_OFFSET_MAX      35

/** A sentinel value to indicate waiting for a valid RSSI without a timeout. */
#define RAIL_GET_RSSI_WAIT_WITHOUT_TIMEOUT ((RAIL_Time_t)0xFFFFFFFFU)
/** A sentinel value to indicate no waiting for a valid RSSI. */
#define RAIL_GET_RSSI_NO_WAIT ((RAIL_Time_t)0U)

/**
 * @struct RAIL_ScheduleRxConfig_t
 * @brief Configures the scheduled RX algorithm.
 *
 * Defines the start and end times of the receive window created
 * for a scheduled receive. If either start or end times are disabled, they
 * will be ignored.
 */
typedef struct RAIL_ScheduleRxConfig {
  /**
   * The time to start receive. See startMode for more information about the
   * types of start times that you can specify.
   */
  RAIL_Time_t start;
  /**
   * How to interpret the time value specified in the start parameter. See the
   * \ref RAIL_TimeMode_t documentation for more information. Use
   * \ref RAIL_TIME_ABSOLUTE for absolute times, \ref RAIL_TIME_DELAY for times
   * relative to the current time and \ref RAIL_TIME_DISABLED to ignore the
   * start time.
   */
  RAIL_TimeMode_t startMode;
  /**
   * The time to end receive. See endMode for more information about the types
   * of end times you can specify.
   */
  RAIL_Time_t end;
  /**
   * How to interpret the time value specified in the end parameter. See the
   * \ref RAIL_TimeMode_t documentation for more information. Note that, in
   * this API, if you specify a \ref RAIL_TIME_DELAY, it is relative to the
   * start time if given and relative to now if none is specified. Also, using
   * \ref RAIL_TIME_DISABLED means that this window will not end unless you
   * explicitly call \ref RAIL_Idle() or add an end event through a future
   * update to this configuration.
   */
  RAIL_TimeMode_t endMode;
  /**
   * While in scheduled RX, you can still control the radio state via
   * state transitions. This option configures whether a transition
   * to RX goes back to scheduled RX or to the normal RX state. Once in the
   * normal RX state, you will effectively end the scheduled RX window and can
   * continue to receive indefinitely depending on the state transitions. Set
   * to 1 to transition to normal RX and 0 to stay in the scheduled RX.
   *
   * This setting also influences the posting of
   * \ref RAIL_EVENT_RX_SCHEDULED_RX_END when the scheduled Rx window is
   * implicitly ended by a packet receive (any of the
   * \ref RAIL_EVENTS_RX_COMPLETION events). See that event for details.
   *
   * @note An Rx transition to Idle state will always terminate the
   * scheduled Rx window, regardless of this setting. This can be used
   * to ensure Scheduled RX terminates on the first packet received
   * (or first successful packet if the RX error transition is to Rx
   * while the Rx success transition is to Idle).
   */
  uint8_t rxTransitionEndSchedule;
  /**
   * This setting tells RAIL what to do with a packet being received
   * when the window end event occurs. If set to 0, such a packet
   * will be allowed to complete. Any other setting will cause that
   * packet to be aborted. In either situation, any posting of
   * \ref RAIL_EVENT_RX_SCHEDULED_RX_END is deferred briefly to when
   * the packet's corresponding \ref RAIL_EVENTS_RX_COMPLETION occurs.
   */
  uint8_t hardWindowEnd;
} RAIL_ScheduleRxConfig_t;

/**
 * @enum RAIL_RxPacketStatus_t
 * @brief The packet status code associated with a packet received or
 *   currently being received.
 *
 * @note RECEIVING implies some packet data may be available, but
 *   is untrustworthy (not CRC-verified) and might disappear if the packet
 *   is rolled back on error. No packet details are yet available.
 * @note In RX \ref RAIL_DataMethod_t::FIFO_MODE, ABORT statuses imply some
 *   packet data may be available, but it's incomplete and not trustworthy.
 */
RAIL_ENUM(RAIL_RxPacketStatus_t) {
  /**
   * The radio is idle or searching for a packet.
   */
  RAIL_RX_PACKET_NONE = 0,
  /**
   * The packet was aborted during filtering because of illegal frame length,
   * CRC or block decoding errors, other RAIL built-in protocol-specific
   * packet content errors, or by the application or multiprotocol scheduler
   * idling the radio with \ref RAIL_IDLE_ABORT or higher.
   *
   * Corresponding \ref RAIL_EVENT_RX_PACKET_ABORTED is triggered.
   */
  RAIL_RX_PACKET_ABORT_FORMAT,
  /**
   * The packet failed address filtering.
   *
   * Corresponding \ref RAIL_EVENT_RX_ADDRESS_FILTERED is triggered.
   */
  RAIL_RX_PACKET_ABORT_FILTERED,
  /**
   * The packet passed any filtering but was aborted by the application
   * or multiprotocol scheduler idling the radio with \ref RAIL_IDLE_ABORT
   * or higher.
   *
   * Corresponding \ref RAIL_EVENT_RX_PACKET_ABORTED is triggered.
   */
  RAIL_RX_PACKET_ABORT_ABORTED,
  /**
   * The packet overflowed the receive buffer.
   *
   * Corresponding \ref RAIL_EVENT_RX_FIFO_OVERFLOW is triggered.
   */
  RAIL_RX_PACKET_ABORT_OVERFLOW,
  /**
   * The packet passed any filtering but subsequently failed CRC check(s)
   * block decoding, or illegal frame length, and was aborted.
   *
   * Corresponding \ref RAIL_EVENT_RX_FRAME_ERROR is triggered.
   */
  RAIL_RX_PACKET_ABORT_CRC_ERROR,
  /**
   * The packet passed any filtering but subsequently failed CRC check(s)
   * with \ref RAIL_RX_OPTION_IGNORE_CRC_ERRORS in effect. Can also occur
   * when the packet prematurely ended successfully during filtering,
   * and either the \ref RAIL_EVENT_RX_PACKET_ABORTED or
   * \ref RAIL_EVENT_RX_ADDRESS_FILTERED events had been enabled
   * requesting notification of such packets.
   *
   * Corresponding \ref RAIL_EVENT_RX_PACKET_RECEIVED is triggered.
   */
  RAIL_RX_PACKET_READY_CRC_ERROR,
  /**
   * The packet was successfully received, passing CRC check(s).
   *
   * Corresponding \ref RAIL_EVENT_RX_PACKET_RECEIVED is triggered.
   */
  RAIL_RX_PACKET_READY_SUCCESS,
  /**
   * A packet is being received and is not yet complete.
   */
  RAIL_RX_PACKET_RECEIVING,
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_RX_PACKET_NONE            ((RAIL_RxPacketStatus_t) RAIL_RX_PACKET_NONE)
#define RAIL_RX_PACKET_ABORT_FORMAT    ((RAIL_RxPacketStatus_t) RAIL_RX_PACKET_ABORT_FORMAT)
#define RAIL_RX_PACKET_ABORT_FILTERED  ((RAIL_RxPacketStatus_t) RAIL_RX_PACKET_ABORT_FILTERED)
#define RAIL_RX_PACKET_ABORT_ABORTED   ((RAIL_RxPacketStatus_t) RAIL_RX_PACKET_ABORT_ABORTED)
#define RAIL_RX_PACKET_ABORT_OVERFLOW  ((RAIL_RxPacketStatus_t) RAIL_RX_PACKET_ABORT_OVERFLOW)
#define RAIL_RX_PACKET_ABORT_CRC_ERROR ((RAIL_RxPacketStatus_t) RAIL_RX_PACKET_ABORT_CRC_ERROR)
#define RAIL_RX_PACKET_READY_CRC_ERROR ((RAIL_RxPacketStatus_t) RAIL_RX_PACKET_READY_CRC_ERROR)
#define RAIL_RX_PACKET_READY_SUCCESS   ((RAIL_RxPacketStatus_t) RAIL_RX_PACKET_READY_SUCCESS)
#define RAIL_RX_PACKET_RECEIVING       ((RAIL_RxPacketStatus_t) RAIL_RX_PACKET_RECEIVING)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * @typedef RAIL_RxPacketHandle_t
 * @brief A handle used to reference a packet during reception processing.
 *   There are several sentinel handle values that pertain to certain
 *   circumstances: \ref RAIL_RX_PACKET_HANDLE_INVALID, \ref
 *   RAIL_RX_PACKET_HANDLE_OLDEST, \ref RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE
 *   and \ref RAIL_RX_PACKET_HANDLE_NEWEST.
 */
typedef const void *RAIL_RxPacketHandle_t;

/** An invalid RX packet handle value. */
#define RAIL_RX_PACKET_HANDLE_INVALID  (NULL)

/** A special RX packet handle to refer to the oldest unreleased packet.
 * This includes the newest unread packet which is possibly incomplete or not
 * yet started.
 * This handle is used implicitly by \ref RAIL_ReadRxFifo().
 */
#define RAIL_RX_PACKET_HANDLE_OLDEST   ((RAIL_RxPacketHandle_t) 1)

/** A special RX packet handle to refer to the oldest unreleased
 *  complete packet. This never includes incomplete or unstarted packets.
 *  (Using \ref RAIL_RX_PACKET_HANDLE_OLDEST is inappropriate for this
 *  purpose because it can refer to an unstarted, incomplete, or
 *  unheld packet which are inappropriate to be consumed by the application.)
 */
#define RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE   ((RAIL_RxPacketHandle_t) 2)

/** A special RX packet handle to refer to the newest unreleased packet
 *  when in callback context. For a callback involving a completed
 *  receive event, this refers to the packet just completed. For
 *  other callback events, this refers to the next packet to be
 *  completed, which might be in-progress or might not have even
 *  started yet.
 */
#define RAIL_RX_PACKET_HANDLE_NEWEST   ((RAIL_RxPacketHandle_t) 3)

/**
 * @struct RAIL_RxPacketInfo_t
 * @brief Basic information about a packet being received or already
 *   completed and awaiting processing, including memory pointers to
 *   its data in the circular receive FIFO buffer. This packet information
 *   refers to remaining packet data that has not already been consumed
 *   by RAIL_ReadRxFifo().
 * @note Because the receive FIFO buffer is circular, a packet might start
 *   near the end of the buffer and wrap around to the beginning of
 *   the buffer to finish, hence the distinction between the first
 *   and last portions. Packets that fit without wrapping only have
 *   a first portion (firstPortionBytes == packetBytes and lastPortionData
 *   will be NULL).
 */
typedef struct RAIL_RxPacketInfo {
  RAIL_RxPacketStatus_t packetStatus; /**< The packet status of this packet. */
  uint16_t packetBytes;               /**< The number of packet data bytes
                                           available to read in this packet. */
  uint16_t firstPortionBytes;         /**< The number of bytes in the first portion. */
  uint8_t *firstPortionData;          /**< The pointer to the first portion of packet
                                           data containing firstPortionBytes
                                           number of bytes. */
  uint8_t *lastPortionData;           /**< The pointer to the last portion of a
                                           packet, if any; NULL otherwise. The
                                           number of bytes in this portion is
                                           packetBytes - firstPortionBytes. */
  RAIL_AddrFilterMask_t filterMask;   /**< A bitmask representing which address
                                           filter(s) this packet has passed.
                                           Will be 0 when not filtering or if
                                           packet info is retrieved before
                                           filtering has completed. It's
                                           undefined on platforms lacking \ref
                                           RAIL_SUPPORTS_ADDR_FILTER_MASK */
} RAIL_RxPacketInfo_t;

/**
 * @struct RAIL_RxPacketDetails_t
 * @brief Received packet details obtained via RAIL_GetRxPacketDetails()
 *   or RAIL_GetRxPacketDetailsAlt().
 *
 * @note Certain details are always available, while others are only available
 *   if the \ref RAIL_RxOptions_t \ref RAIL_RX_OPTION_REMOVE_APPENDED_INFO
 *   option is not in effect and the received packet's
 *   \ref RAIL_RxPacketStatus_t is among the RAIL_RX_PACKET_READY_ set.
 *   Each detail's description indicates its availability.
 *
 */
typedef struct RAIL_RxPacketDetails {
  /**
   * The timestamp of the received packet in the RAIL timebase.
   *
   * When not available it will be \ref RAIL_PACKET_TIME_INVALID.
   */
  RAIL_PacketTimeStamp_t timeReceived;
  /**
   * Indicates whether the CRC passed or failed for the received packet.
   * It is true for \ref RAIL_RX_PACKET_READY_SUCCESS packets and false
   * for all others.
   *
   * It is always available.
   */
  bool crcPassed;
  /**
   * Indicate whether the received packet was the expected ACK.
   * It is true for the expected ACK and false otherwise.
   *
   * It is always available.
   *
   * An expected ACK is defined as a protocol-correct ACK packet
   * successfully-received (\ref RAIL_RX_PACKET_READY_SUCCESS or
   * \ref RAIL_RX_PACKET_READY_CRC_ERROR) and whose sync word was
   * detected within the
   * RAIL_AutoAckConfig_t::ackTimeout period following a transmit
   * which specified \ref RAIL_TX_OPTION_WAIT_FOR_ACK, requested
   * an ACK, and auto-ACK is enabled. When true, the ackTimeout
   * period was terminated so no \ref RAIL_EVENT_RX_ACK_TIMEOUT
   * will be subsequently posted for the transmit.
   *
   * A "protocol-correct ACK" applies to the 802.15.4 or Z-Wave
   * protocols for which RAIL can discern the frame type and match
   * the ACK's sequence number with that of the transmitted frame.
   * For other protocols, the first packet successfully-received
   * whose sync word was detected within the ackTimeout period is
   * considered the expected ACK; upper layers are responsible for
   * confirming this.
   */
  bool isAck;
  /**
   * RSSI of the received packet in integer dBm. It is latched when the sync
   * word is detected for the packet.
   *
   * When not available it will be \ref RAIL_RSSI_INVALID_DBM.
   */
  int8_t rssi;
  /**
   * The link quality indicator of the received packet. A zero would
   * indicate a very low quality packet while a 255 would indicate a very
   * high quality packet.
   *
   * When not available it will be 0.
   */
  uint8_t lqi;
  /**
   * For radios and PHY configurations that support multiple sync words, this
   * number is the ID of the sync word that was used for this packet.
   *
   * It is always available.
   */
  uint8_t syncWordId;
  /**
   * In configurations where the radio has the option of receiving a given
   * packet in multiple ways, indicates which of the sub-PHY options
   * was used to receive the packet. Most radio configurations do not have
   * this ability and the subPhyId is set to 0.
   *
   * Currently, this field is used by the BLE Coded PHY, the BLE Simulscan PHY
   * and the SUN OFDM PHYs.
   * In BLE cases, a value of 0 marks a 500 kbps packet, a value of 1 marks a 125
   * kbps packet, and a value of 2 marks a 1 Mbps packet.
   * Also, see \ref RAIL_BLE_ConfigPhyCoded and \ref RAIL_BLE_ConfigPhySimulscan.
   *
   * In SUN OFDM cases, the value corresponds to the numerical value of the
   * Modulation and Coding Scheme (MCS) level of the last received packet.
   * The packet bitrate depends on the MCS value, as well as the OFDM option.
   * Packets bitrates for SUN OFDM PHYs can be found in 802.15.4-2020 specification,
   * chapter 20.3, table 20-10.
   * Ex: Packet bitrate for OFDM option 1 MCS0 is 100kb/s and 2400kb/s for MCS6.
   *
   * It is always available.
   */
  uint8_t subPhyId;
  /**
   * For \ref Antenna_Control configurations where the device has multiple
   * antennas, this indicates which antenna received the packet. When there
   * is only one antenna, this will be set to the default of 0.
   *
   * It is always available.
   */
  uint8_t antennaId;
  /**
   * When channel hopping is enabled, this field will contain the index
   * of the channel in \ref RAIL_RxChannelHoppingConfig_t::entries on which
   * this packet was received, or a sentinel value. On EFR32XG1 parts,
   * on which channel hopping is not supported, this value is still part
   * of the structure, but will be a meaningless value.
   *
   * It is always available.
   */
  uint8_t channelHoppingChannelIndex;
  /**
   * The channel on which the packet was received.
   *
   * It is always available.
   *
   * @note It is best to fully process (empty or clear) the receive FIFO
   *   before changing channel configurations (\ref RAIL_ConfigChannels()
   *   or a built-in configuration) as unprocessed packets' channel
   *   could reflect the wrong configuration. On EFR32xG1 only this
   *   advice also applies when changing channels for receive or transmit
   *   where unprocessed packets' channel could reflect the new channel.
   */
  uint16_t channel;
} RAIL_RxPacketDetails_t;

/** @} */ // end of group Receive

/**
 * @addtogroup Auto_Ack
 * @{
 */
/**
 * @struct RAIL_AutoAckConfig_t
 * @brief Enable/disable the auto-ACK algorithm, based on "enable".
 *
 * The structure provides a default state (the "success" of tx/rxTransitions
 * when ACKing is enabled) for the radio to return to after an ACK
 * operation occurs (transmitting or attempting to receive an ACK), or normal
 * state transitions to return to in the case ACKing is
 * disabled. Regardless whether the ACK operation was successful, the
 * radio returns to the specified success state.
 *
 * ackTimeout specifies how long to stay in receive and wait for an ACK
 * to start (sync detected) before issuing a RAIL_EVENT_RX_ACK_TIMEOUT
 * event and return to the default state.
 */
typedef struct RAIL_AutoAckConfig {
  /**
   * Indicate whether auto-ACKing should be enabled or disabled.
   */
  bool enable;
  /**
   * Define the RX ACK timeout duration in microseconds up to 65535
   * microseconds maximum. Only applied when auto-ACKing is enabled.
   * The ACK timeout timer starts at the completion of a \ref
   * RAIL_TX_OPTION_WAIT_FOR_ACK transmit and expires only while waiting
   * for a packet (prior to SYNC detect), triggering \ref
   * RAIL_EVENT_RX_ACK_TIMEOUT. During packet reception that event is
   * held off until packet completion and suppressed entirely if the
   * received packet is the expected ACK.
   */
  uint16_t ackTimeout;
  /**
   * State transitions to do after receiving a packet. When auto-ACKing is
   * enabled, the "error" transition is always ignored and the radio will
   * return to the "success" state after any ACKing sequence
   * (\ref RAIL_RF_STATE_RX or \ref RAIL_RF_STATE_IDLE).
   * See \ref RAIL_ConfigAutoAck for more details on this.
   */
  RAIL_StateTransitions_t rxTransitions;
  /**
   * State transitions to do after transmitting a packet. When auto-ACKing is
   * enabled, the "error" transition is always ignored and the radio will
   * return to the "success" state after any ACKing sequence
   * (\ref RAIL_RF_STATE_RX or \ref RAIL_RF_STATE_IDLE).
   * See \ref RAIL_ConfigAutoAck for more details on this.
   */
  RAIL_StateTransitions_t txTransitions;
} RAIL_AutoAckConfig_t;

/// Acknowledgment packets cannot be longer than 64 bytes.
#define RAIL_AUTOACK_MAX_LENGTH (64U)

/** @} */ // end of group Auto_Ack

/******************************************************************************
 * External_Thermistor Structures
 *****************************************************************************/
/**
 * @addtogroup External_Thermistor
 * @{
 */

/// A sentinel value to indicate an invalid thermistor measurement value.
#define RAIL_INVALID_THERMISTOR_VALUE (0xFFFFFFFF)

/**
 * @struct RAIL_HFXOThermistorConfig_t
 * @brief Configure the port and pin of the thermistor.
 * @note This configuration is OPN dependent.
 */
typedef struct RAIL_HFXOThermistorConfig {
  /**
   * The GPIO port to access.
   */
  uint8_t port;
  /**
   * The GPIO pin to set.
   */
  uint8_t pin;
} RAIL_HFXOThermistorConfig_t;
/** @} */ // end of group External_Thermistor

/******************************************************************************
 * Calibration Structures
 *****************************************************************************/
/**
 * @addtogroup Calibration
 * @{
 */

/**
 * @typedef RAIL_CalMask_t
 * @brief A calibration mask type
 *
 * This type is a bitmask of different RAIL calibration values. The exact
 * meaning of these bits depends on what a particular chip supports.
 */
typedef uint32_t RAIL_CalMask_t;

/** @} */ // end of group Calibration

/******************************************************************************
 * LQI Structures
 *****************************************************************************/
/**
 * @addtogroup Receive
 * @{
 */

/**
 * @typedef RAIL_ConvertLqiCallback_t
 * @brief A pointer to a function called before LQI is copied into the
 *   \ref RAIL_RxPacketDetails_t structure.
 *
 * @param[in] lqi The LQI value obtained by hardware and being readied for
 *   application consumption. This LQI value is in integral units ranging from
 *   0 to 255.
 * @param[in] rssi The RSSI value corresponding to the packet from which the
 *   hardware LQI value was obtained. This RSSI value is in integral dBm units.
 * @return uint8_t The converted LQI value that will be loaded into the
 *   \ref RAIL_RxPacketDetails_t structure in preparation for application
 *   consumption. This value should likewise be in integral units ranging from
 *   0 to 255.
 */
typedef uint8_t (*RAIL_ConvertLqiCallback_t)(uint8_t lqi,
                                             int8_t rssi);

/** @} */ // end of group Receive

/******************************************************************************
 * RF Sense Structures
 *****************************************************************************/
/**
 * @addtogroup Rf_Sense
 * @{
 */

/**
 * A pointer to an RF Sense callback function.
 *
 * Consider using the event \ref RAIL_EVENT_RF_SENSED as an alternative.
 */
typedef void (*RAIL_RfSense_CallbackPtr_t)(void);

/**
 * RF Sense low sensitivity offset.
 */
#define RAIL_RFSENSE_LOW_SENSITIVITY_OFFSET   (0x20U)

/**
 * @enum RAIL_RfSenseBand_t
 * @brief An enumeration for specifying the RF Sense frequency band.
 */
RAIL_ENUM(RAIL_RfSenseBand_t) {
  RAIL_RFSENSE_OFF,    /**< RF Sense is disabled. */
  RAIL_RFSENSE_2_4GHZ, /**< RF Sense is in 2.4 G band. */
  RAIL_RFSENSE_SUBGHZ, /**< RF Sense is in subgig band. */
  RAIL_RFSENSE_ANY,    /**< RF Sense is in both bands. */
  RAIL_RFSENSE_MAX,    // Must be last before sensitivity options.
  RAIL_RFSENSE_2_4GHZ_LOW_SENSITIVITY = RAIL_RFSENSE_LOW_SENSITIVITY_OFFSET + RAIL_RFSENSE_2_4GHZ,  /**< RF Sense is in low sensitivity 2.4 G band */
  RAIL_RFSENSE_SUBGHZ_LOW_SENSITIVITY = RAIL_RFSENSE_LOW_SENSITIVITY_OFFSET + RAIL_RFSENSE_SUBGHZ,  /**< RF Sense is in low sensitivity subgig band */
  RAIL_RFENSE_ANY_LOW_SENSITIVITY = RAIL_RFSENSE_LOW_SENSITIVITY_OFFSET + RAIL_RFSENSE_ANY          /**< RF Sense is in low sensitivity for both bands. */
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_RFSENSE_OFF                    ((RAIL_RfSenseBand_t) RAIL_RFSENSE_OFF)
#define RAIL_RFSENSE_2_4GHZ                 ((RAIL_RfSenseBand_t) RAIL_RFSENSE_2_4GHZ)
#define RAIL_RFSENSE_SUBGHZ                 ((RAIL_RfSenseBand_t) RAIL_RFSENSE_SUBGHZ)
#define RAIL_RFSENSE_ANY                    ((RAIL_RfSenseBand_t) RAIL_RFSENSE_ANY)
#define RAIL_RFSENSE_MAX                    ((RAIL_RfSenseBand_t) RAIL_RFSENSE_MAX)
#define RAIL_RFSENSE_2_4GHZ_LOW_SENSITIVITY ((RAIL_RfSenseBand_t) RAIL_RFSENSE_2_4GHZ_LOW_SENSITIVITY)
#define RAIL_RFSENSE_SUBGHZ_LOW_SENSITIVITY ((RAIL_RfSenseBand_t) RAIL_RFSENSE_SUBGHZ_LOW_SENSITIVITY)
#define RAIL_RFENSE_ANY_LOW_SENSITIVITY     ((RAIL_RfSenseBand_t) RAIL_RFENSE_ANY_LOW_SENSITIVITY)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * Use the MODEM default sync word.
 */
#define RAIL_RFSENSE_USE_HW_SYNCWORD    (0U)

/**
 * @struct RAIL_RfSenseSelectiveOokConfig_t
 * @brief Structure to configure RFSENSE Selective(OOK) mode.
 */
typedef struct RAIL_RfSenseSelectiveOokConfig {
  /**
   * The frequency band(s) on which to sense the
   * RF energy. To stop RF Sense, specify \ref RAIL_RFSENSE_OFF.
   */
  RAIL_RfSenseBand_t band;
  /**
   * Syncword Length in bytes, 1-4 bytes.
   * @note When \ref syncWord is set to use \ref RAIL_RFSENSE_USE_HW_SYNCWORD,
   * the \ref syncWordNumBytes value will be ignored since we rely on the
   * HW default settings for sync word.
   */
  uint8_t syncWordNumBytes;
  /**
   * Sync Word Value.
   * To use HW default sync word, set to \ref RAIL_RFSENSE_USE_HW_SYNCWORD.
   */
  uint32_t syncWord;
  /**
   * The callback function, called when RF is sensed.
   */
  RAIL_RfSense_CallbackPtr_t cb;
} RAIL_RfSenseSelectiveOokConfig_t;

/** @} */ // end of group Rf_Sense

/******************************************************************************
 * RX Channel Hopping
 *****************************************************************************/
/**
 * @addtogroup Rx_Channel_Hopping RX Channel Hopping
 * @{
 */

/**
 * @enum RAIL_RxChannelHoppingMode_t
 * @brief Modes by which RAIL can determine when to proceed to the next
 * channel during channel hopping
 */
RAIL_ENUM(RAIL_RxChannelHoppingMode_t) {
  /**
   * Switch to the next channel each time the radio enters RX.
   */
  RAIL_RX_CHANNEL_HOPPING_MODE_MANUAL,
  /**
   * Switch to the next channel after a certain amount of time passes.
   * The time should be specified in microseconds in \ref
   * RAIL_RxChannelHoppingConfigEntry_t::parameter, and must be less
   * than \ref RAIL_RX_CHANNEL_HOPPING_MAX_SENSE_TIME_US.
   */
  RAIL_RX_CHANNEL_HOPPING_MODE_TIMEOUT,
  /**
   * Listen in receive RX for at least a specified timeout. If,
   * by the end of the timeout, the radio has packet timing,
   * remain in the current channel until the radio loses it. The
   * timeout should be specified in microseconds in \ref
   * RAIL_RxChannelHoppingConfigEntry_t::parameter, and must be less
   * than \ref RAIL_RX_CHANNEL_HOPPING_MAX_SENSE_TIME_US.
   */
  RAIL_RX_CHANNEL_HOPPING_MODE_TIMING_SENSE,
  /**
   * Listen in receive RX for at least a specified timeout. If,
   * by the end of the timeout, the radio has a packet preamble,
   * remain in the current channel until the radio loses it. The
   * timeout should be specified in microseconds in \ref
   * RAIL_RxChannelHoppingConfigEntry_t::parameter, and must be less
   * than \ref RAIL_RX_CHANNEL_HOPPING_MAX_SENSE_TIME_US.
   */
  RAIL_RX_CHANNEL_HOPPING_MODE_PREAMBLE_SENSE,
  /**
   * Placeholder for a reserved hopping mode that is not supported.
   */
  RAIL_RX_CHANNEL_HOPPING_MODE_RESERVED1,
  /**
   * A mode that combines modes TIMING_SENSE, PREAMBLE_SENSE, and
   * TIMEOUT (sync detect) all running in parallel. Refer to \ref
   * RAIL_RxChannelHoppingConfigMultiMode_t for further details.
   * A pointer to that structure, allocated in global read-write
   * memory and initialized to the desired configuration values, is
   * specified as the \ref RAIL_RxChannelHoppingConfigEntry_t::parameter
   * or \ref RAIL_RxDutyCycleConfig_t::parameter, cast appropriately:
   * @code{.c}
   *   .parameter = (uint32_t)(void *)&hoppingConfigMultiMode,
   * @endcode
   *
   * @note RAIL will overwrite the contents of the \ref
   * RAIL_RxChannelHoppingConfigMultiMode_t during operation so it
   * must be reinitialized with the desired configuration prior to
   * every call to \ref RAIL_ConfigRxChannelHopping() or
   * \ref RAIL_ConfigRxDutyCycle().
   *
   * @note Multiple \ref RAIL_RxChannelHoppingConfigEntry_t entries may
   * share the same \ref RAIL_RxChannelHoppingConfigMultiMode_t if their
   * settings are identical, otherwise a separate \ref
   * RAIL_RxChannelHoppingConfigMultiMode_t is needed for each
   * \ref RAIL_RxChannelHoppingConfigEntry_t that uses this mode.
   */
  RAIL_RX_CHANNEL_HOPPING_MODE_MULTI_SENSE,
  /**
   * Switch to the next channel based on the demodulation settings in the PHY
   * config. This mode is PHY and chip dependent. The
   * \ref RAIL_RxChannelHoppingConfigEntry_t::parameter is ignored, and should
   * be set to 0 for future compatibility.
   */
  RAIL_RX_CHANNEL_HOPPING_MODE_SQ,
  /**
   * Marks that the channel is concurrent with another channel, and otherwise
   * behaves identically to \ref RAIL_RX_CHANNEL_HOPPING_MODE_SQ.
   */
  RAIL_RX_CHANNEL_HOPPING_MODE_CONC,
  /**
   * Indicates that this channel is a virtual channel that is concurrently
   * detected with the channel indicated by the
   * \ref RAIL_RxChannelHoppingConfigEntry_t::parameter. Otherwise behaves
   * identically to \ref RAIL_RX_CHANNEL_HOPPING_MODE_SQ.
   */
  RAIL_RX_CHANNEL_HOPPING_MODE_VT,
  /**
   * This is the transmit channel used for auto-ACK if the regular channel,
   * specified in RAIL_RxChannelHoppingConfigEntry::parameter, is
   * optimized for RX which may degrade some TX performance
   */
  RAIL_RX_CHANNEL_HOPPING_MODE_TX,
  /**
   * A count of the basic choices in this enumeration.
   */
  RAIL_RX_CHANNEL_HOPPING_MODES_COUNT, // Must be last before _WITH_OPTIONS twins

  /**
   * The start of equivalent modes requiring non-default \ref
   * RAIL_RxDutyCycleConfig_t::options, needed for backwards-compatibility
   * with earlier \ref RAIL_RxDutyCycleConfig_t format. Non-default options
   * are supported with \ref RAIL_RxChannelHoppingConfigEntry_t in all modes.
   */
  RAIL_RX_CHANNEL_HOPPING_MODES_WITH_OPTIONS_BASE = 0x80,
  /** Variant of \ref RAIL_RX_CHANNEL_HOPPING_MODE_MANUAL with options. */
  RAIL_RX_CHANNEL_HOPPING_MODE_MANUAL_WITH_OPTIONS
    = RAIL_RX_CHANNEL_HOPPING_MODES_WITH_OPTIONS_BASE,
  /** Variant of \ref RAIL_RX_CHANNEL_HOPPING_MODE_TIMEOUT with options. */
  RAIL_RX_CHANNEL_HOPPING_MODE_TIMEOUT_WITH_OPTIONS,
  /** Variant of \ref RAIL_RX_CHANNEL_HOPPING_MODE_TIMING_SENSE with options. */
  RAIL_RX_CHANNEL_HOPPING_MODE_TIMING_SENSE_WITH_OPTIONS,
  /** Variant of \ref RAIL_RX_CHANNEL_HOPPING_MODE_PREAMBLE_SENSE with options. */
  RAIL_RX_CHANNEL_HOPPING_MODE_PREAMBLE_SENSE_WITH_OPTIONS,
  /** Variant of \ref RAIL_RX_CHANNEL_HOPPING_MODE_RESERVED1 with options. */
  RAIL_RX_CHANNEL_HOPPING_MODE_RESERVED1_WITH_OPTIONS,
  /** Variant of \ref RAIL_RX_CHANNEL_HOPPING_MODE_MULTI_SENSE with options. */
  RAIL_RX_CHANNEL_HOPPING_MODE_MULTI_SENSE_WITH_OPTIONS,
  /** Variant of \ref RAIL_RX_CHANNEL_HOPPING_MODE_SQ with options. */
  RAIL_RX_CHANNEL_HOPPING_MODE_SQ_WITH_OPTIONS,
  /** Variant of \ref RAIL_RX_CHANNEL_HOPPING_MODE_CONC with options. */
  RAIL_RX_CHANNEL_HOPPING_MODE_CONC_WITH_OPTIONS,
  /** Variant of \ref RAIL_RX_CHANNEL_HOPPING_MODE_VT with options. */
  RAIL_RX_CHANNEL_HOPPING_MODE_VT_WITH_OPTIONS,
  /** Variant of \ref RAIL_RX_CHANNEL_HOPPING_MODE_TX with options. */
  RAIL_RX_CHANNEL_HOPPING_MODE_TX_WITH_OPTIONS,
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_RX_CHANNEL_HOPPING_MODE_MANUAL                      ((RAIL_RxChannelHoppingMode_t) RAIL_RX_CHANNEL_HOPPING_MODE_MANUAL)
#define RAIL_RX_CHANNEL_HOPPING_MODE_TIMEOUT                     ((RAIL_RxChannelHoppingMode_t) RAIL_RX_CHANNEL_HOPPING_MODE_TIMEOUT)
#define RAIL_RX_CHANNEL_HOPPING_MODE_TIMING_SENSE                ((RAIL_RxChannelHoppingMode_t) RAIL_RX_CHANNEL_HOPPING_MODE_TIMING_SENSE)
#define RAIL_RX_CHANNEL_HOPPING_MODE_PREAMBLE_SENSE              ((RAIL_RxChannelHoppingMode_t) RAIL_RX_CHANNEL_HOPPING_MODE_PREAMBLE_SENSE)
#define RAIL_RX_CHANNEL_HOPPING_MODE_RESERVED1                   ((RAIL_RxChannelHoppingMode_t) RAIL_RX_CHANNEL_HOPPING_MODE_RESERVED1)
#define RAIL_RX_CHANNEL_HOPPING_MODE_MULTI_SENSE                 ((RAIL_RxChannelHoppingMode_t) RAIL_RX_CHANNEL_HOPPING_MODE_MULTI_SENSE)
#define RAIL_RX_CHANNEL_HOPPING_MODE_SQ                          ((RAIL_RxChannelHoppingMode_t) RAIL_RX_CHANNEL_HOPPING_MODE_SQ)
#define RAIL_RX_CHANNEL_HOPPING_MODE_CONC                        ((RAIL_RxChannelHoppingMode_t) RAIL_RX_CHANNEL_HOPPING_MODE_CONC)
#define RAIL_RX_CHANNEL_HOPPING_MODE_VT                          ((RAIL_RxChannelHoppingMode_t) RAIL_RX_CHANNEL_HOPPING_MODE_VT)
#define RAIL_RX_CHANNEL_HOPPING_MODE_TX                          ((RAIL_RxChannelHoppingMode_t) RAIL_RX_CHANNEL_HOPPING_MODE_TX)
#define RAIL_RX_CHANNEL_HOPPING_MODES_WITH_OPTIONS_BASE          ((RAIL_RxChannelHoppingMode_t) RAIL_RX_CHANNEL_HOPPING_MODES_WITH_OPTIONS_BASE)
#define RAIL_RX_CHANNEL_HOPPING_MODE_MANUAL_WITH_OPTIONS         ((RAIL_RxChannelHoppingMode_t) RAIL_RX_CHANNEL_HOPPING_MODE_MANUAL_WITH_OPTIONS)
#define RAIL_RX_CHANNEL_HOPPING_MODE_TIMEOUT_WITH_OPTIONS        ((RAIL_RxChannelHoppingMode_t) RAIL_RX_CHANNEL_HOPPING_MODE_TIMEOUT_WITH_OPTIONS)
#define RAIL_RX_CHANNEL_HOPPING_MODE_TIMING_SENSE_WITH_OPTIONS   ((RAIL_RxChannelHoppingMode_t) RAIL_RX_CHANNEL_HOPPING_MODE_TIMING_SENSE_WITH_OPTIONS)
#define RAIL_RX_CHANNEL_HOPPING_MODE_PREAMBLE_SENSE_WITH_OPTIONS ((RAIL_RxChannelHoppingMode_t) RAIL_RX_CHANNEL_HOPPING_MODE_PREAMBLE_SENSE_WITH_OPTIONS)
#define RAIL_RX_CHANNEL_HOPPING_MODE_RESERVED1_WITH_OPTIONS      ((RAIL_RxChannelHoppingMode_t) RAIL_RX_CHANNEL_HOPPING_MODE_RESERVED1_WITH_OPTIONS)
#define RAIL_RX_CHANNEL_HOPPING_MODE_MULTI_SENSE_WITH_OPTIONS    ((RAIL_RxChannelHoppingMode_t) RAIL_RX_CHANNEL_HOPPING_MODE_MULTI_SENSE_WITH_OPTIONS)
#define RAIL_RX_CHANNEL_HOPPING_MODE_SQ_WITH_OPTIONS             ((RAIL_RxChannelHoppingMode_t) RAIL_RX_CHANNEL_HOPPING_MODE_SQ_WITH_OPTIONS)
#define RAIL_RX_CHANNEL_HOPPING_MODE_CONC_WITH_OPTIONS           ((RAIL_RxChannelHoppingMode_t) RAIL_RX_CHANNEL_HOPPING_MODE_CONC_WITH_OPTIONS)
#define RAIL_RX_CHANNEL_HOPPING_MODE_VT_WITH_OPTIONS             ((RAIL_RxChannelHoppingMode_t) RAIL_RX_CHANNEL_HOPPING_MODE_VT_WITH_OPTIONS)
#define RAIL_RX_CHANNEL_HOPPING_MODE_TX_WITH_OPTIONS             ((RAIL_RxChannelHoppingMode_t) RAIL_RX_CHANNEL_HOPPING_MODE_TX_WITH_OPTIONS)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * The maximum sense time supported for those \ref RAIL_RxChannelHoppingMode_t
 * modes whose parameter(s) specify a sensing time.
 */
#define RAIL_RX_CHANNEL_HOPPING_MAX_SENSE_TIME_US 0x08000000UL

/**
 * @enum RAIL_RxChannelHoppingDelayMode_t
 * @brief Deprecated enum. Set only to RAIL_RX_CHANNEL_DELAY_MODE_STATIC
 */
RAIL_ENUM(RAIL_RxChannelHoppingDelayMode_t) {
  /**
   * Always delay for exactly the amount of time specified
   * in the delay parameter, regardless of how other channel
   * hopping channels were extended via preamble sense or other means.
   */
  RAIL_RX_CHANNEL_HOPPING_DELAY_MODE_STATIC,
};

/**
 * @typedef RAIL_RxChannelHoppingParameter_t
 * @brief Rx channel hopping on-channel time
 */
typedef uint32_t RAIL_RxChannelHoppingParameter_t;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_RX_CHANNEL_HOPPING_DELAY_MODE_STATIC ((RAIL_RxChannelHoppingDelayMode_t) RAIL_RX_CHANNEL_HOPPING_DELAY_MODE_STATIC)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * @enum RAIL_RxChannelHoppingOptions_t
 * @brief Options that can customize channel hopping behavior
 * on a per-hop basis.
 */
RAIL_ENUM(RAIL_RxChannelHoppingOptions_t) {
  /** Shift position of \ref RAIL_RX_CHANNEL_HOPPING_OPTION_SKIP_SYNTH_CAL bit */
  RAIL_RX_CHANNEL_HOPPING_OPTION_SKIP_SYNTH_CAL_SHIFT,
  /** Shift position of \ref RAIL_RX_CHANNEL_HOPPING_OPTION_SKIP_DC_CAL bit */
  RAIL_RX_CHANNEL_HOPPING_OPTION_SKIP_DC_CAL_SHIFT,
  /** Shift position of \ref RAIL_RX_CHANNEL_HOPPING_OPTION_RSSI_THRESHOLD bit */
  RAIL_RX_CHANNEL_HOPPING_OPTION_RSSI_THRESHOLD_SHIFT,
  /** Stop hopping on this hop. */
  RAIL_RX_CHANNEL_HOPPING_OPTION_STOP_SHIFT,
  /** A count of the choices in this enumeration. */
  RAIL_RX_CHANNEL_HOPPING_OPTIONS_COUNT // Must be last
};

/** A value representing no options enabled. */
#define RAIL_RX_CHANNEL_HOPPING_OPTIONS_NONE 0U
/**
 * All options disabled by default.
 * Channel hopping will behave as described by other
 * parameters as it did in RAIL 2.7 and earlier.
 */
#define RAIL_RX_CHANNEL_HOPPING_OPTIONS_DEFAULT RAIL_RX_CHANNEL_HOPPING_OPTIONS_NONE
/**
 * Deprecated backwards-compatible synonym of \ref
 * RAIL_RX_CHANNEL_HOPPING_OPTIONS_DEFAULT.
 */
#define RAIL_RX_CHANNEL_HOPPING_OPTION_DEFAULT RAIL_RX_CHANNEL_HOPPING_OPTIONS_DEFAULT
/**
 * An option to skip synth calibration while *hopping into* the channel
 * specified in the current entry.
 */
#define RAIL_RX_CHANNEL_HOPPING_OPTION_SKIP_SYNTH_CAL (1U << RAIL_RX_CHANNEL_HOPPING_OPTION_SKIP_SYNTH_CAL_SHIFT)
/**
 * An option to skip DC calibration while *hopping into* the channel
 * specified in the current entry.
 */
#define RAIL_RX_CHANNEL_HOPPING_OPTION_SKIP_DC_CAL (1U << RAIL_RX_CHANNEL_HOPPING_OPTION_SKIP_DC_CAL_SHIFT)
/**
 * An option to check RSSI after *hopping into* the channel specified
 * in the current entry and hop if that RSSI is below the threshold
 * specified in \ref RAIL_RxChannelHoppingConfigEntry_t::rssiThresholdDbm.
 * This check runs in parallel with the \ref RAIL_RxChannelHoppingMode_t
 * specified and may cause a hop sooner than that mode otherwise would.
 */
#define RAIL_RX_CHANNEL_HOPPING_OPTION_RSSI_THRESHOLD (1U << RAIL_RX_CHANNEL_HOPPING_OPTION_RSSI_THRESHOLD_SHIFT)
/**
 * An option to stop the hopping sequence at this entry in the hop
 * table.
 */
#define RAIL_RX_CHANNEL_HOPPING_OPTION_STOP (1U << RAIL_RX_CHANNEL_HOPPING_OPTION_STOP_SHIFT)

/// @struct RAIL_RxChannelHoppingConfigMultiMode_t
/// @brief Structure that parameterizes \ref
///   RAIL_RX_CHANNEL_HOPPING_MODE_MULTI_SENSE.
///
/// Every \ref RAIL_RxChannelHoppingConfigEntry_t or
/// \ref RAIL_RxDutyCycleConfig_t that uses \ref
/// RAIL_RX_CHANNEL_HOPPING_MODE_MULTI_SENSE must allocate one of these
/// structures in global read-write memory to provide the settings
/// for this mode and for RAIL to use during hopping or duty cycling.
/// A pointer to this structure, cast appropriately, is what is passed
/// in the corresponding \ref RAIL_RxChannelHoppingConfigEntry_t::parameter
/// or \ref RAIL_RxDutyCycleConfig_t::parameter.
///
/// The contents of this structure must be initialized prior to each
/// \ref RAIL_ConfigRxChannelHopping() or \ref RAIL_ConfigRxDutyCycle()
/// call and must not be touched thereafter until the next such call.
/// RAIL may change these contents during configuration or operation.
///
/// This mode of operation functions algorithmically like this pseudocode:
/// @code{.c}
/// extern bool channelHopping; // true if channel hopping, false if duty cycling
/// extern RAIL_RxChannelHoppingConfigEntry_t *hopConfigEntry; // current channel
///
/// static RAIL_RxChannelHoppingConfigMultiMode_t *multiParams;
/// static RAIL_Time_t rxStartTime;
/// static bool preambleSensed;
///
/// static void hopOrSuspendRx(uint32_t delay)
/// {
///   disableDemodEvents();
///   disableTimerEvents();
///   stopTimer();
///   if (channelHopping) {
///     hopToNextChannel(delay, &hopConfigEntry); // updates hopConfigEntry
///   } else {
///     suspendRx(delay);
///   }
///   onStartRx(); // resume receive after delay (on new channel if hopping)
/// }
///
/// void onStartRx(void) // called upon entry to receive
/// {
///   rxStartTime = RAIL_GetTime();
///   multiParams = (RAIL_RxChannelHoppingConfigMultiMode_t *)
///                 (void *)hopConfigEntry->parameter;
///   startTimer(rxStartTime + multiParams->timingSense);
///   preambleSensed = false;
///   enableTimerEvents(); // timer will trigger onTimerEvent() handler
///   enableDemodEvents(); // demod will trigger onDemodEvent() handler
/// }
///
/// void onTimerEvent(void) // called when timer expires
/// {
///   hopOrSuspendRx(hopConfigEntry->delay);
/// }
///
/// void onDemodEvent(void) // called when demodulator state changes
/// {
///   if (DEMOD_TIMING_SENSED) {
///     stopTimer();
///     startTimer(rxStartTime + multiParams->syncDetect);
///   }
///   if (DEMOD_TIMING_LOST) {
///     stopTimer();
///     uint32_t newTimeout = RAIL_GetTime() + multiParams->timingReSense;
///     uint32_t limitTimeout;
///     if (preambleSensed) {
///       limitTimeout = rxStartTime + multiParams->syncDetect;
///     } else {
///       limitTimeout = rxStartTime + multiParams->preambleSense;
///     }
///     if (newTimeout > limitTimeout) {
///       newTimeout = limitTimeout;
///     }
///     if (newTimeout > RAIL_GetTime()) {
///       startTimer(newTimeout);
///     } else {
///       hopOrSuspendRx(hopConfigEntry->delay);
///     }
///   }
///   if (DEMOD_PREAMBLE_SENSED) {
///     preambleSensed = true;
///   }
///   if (DEMOD_PREAMBLE_LOST) {
///     preambleSensed = false;
///   }
///   if (DEMOD_SYNC_DETECTED) {
///     disableDemodEvents();
///     disableTimerEvents();
///     stopTimer();
///     receivePacket(); // stay on channel to receive frame
///     hopOrSuspendRx(0); // continue RX per state transitions with no delay
///   }
/// }
/// @endcode

typedef struct RAIL_RxChannelHoppingConfigMultiMode {
  /**
   * Switch to the next channel if sync is not detected before
   * this time, in microseconds, measured from entry to Rx.
   * This must be greater than preambleSense and less than
   * \ref RAIL_RX_CHANNEL_HOPPING_MAX_SENSE_TIME_US.
   */
  uint32_t syncDetect;
  /**
   * Switch to the next channel if timing was sensed but then
   * lost after this time, in microseconds, measured from entry
   * to Rx -- unless preamble had been sensed in which case any
   * switching is deferred to timingReSense and, if timing is
   * regained, to syncDetect.  This must be greater than timingSense
   * and less than \ref RAIL_RX_CHANNEL_HOPPING_MAX_SENSE_TIME_US.
   */
  uint32_t preambleSense;
  /**
   * Switch to the next channel if timing is not sensed before
   * this time, in microseconds, measured from entry to Rx. This
   * must be greater than 2 and less than
   * \ref RAIL_RX_CHANNEL_HOPPING_MAX_SENSE_TIME_US.
   */
  uint32_t timingSense;
  /**
   * Switch to the next channel if timing was sensed but then
   * lost and not regained before this time, in microseconds,
   * measured from when timing was lost. This must be less than
   * \ref RAIL_RX_CHANNEL_HOPPING_MAX_SENSE_TIME_US.
   */
  uint32_t timingReSense;
  /**
   * Set this to 0. This field, along with the others, may be
   * used internally by RAIL during configuration or operation.
   */
  uint32_t status;
} RAIL_RxChannelHoppingConfigMultiMode_t;

/**
 * @struct RAIL_RxChannelHoppingConfigEntry_t
 * @brief Structure that represents one of the channels that is part of a
 *   \ref RAIL_RxChannelHoppingConfig_t sequence of channels used in
 *   channel hopping.
 */
typedef struct RAIL_RxChannelHoppingConfigEntry {
  /**
   * The channel number to be used for this entry in the channel hopping
   * sequence. If this is an invalid channel for the current PHY, the
   * call to \ref RAIL_ConfigRxChannelHopping() will fail.
   */
  uint16_t channel;
  /** The mode by which RAIL determines when to hop to the next channel. */
  RAIL_RxChannelHoppingMode_t mode;
  // Unnamed 'uint8_t reserved1[1]' pad byte field here.
  /**
   * Depending on the 'mode' parameter that was specified, this member
   * is used to parameterize that mode. See the comments on each value of
   * \ref RAIL_RxChannelHoppingMode_t to learn what to specify here.
   */
  RAIL_RxChannelHoppingParameter_t parameter;
  /**
   * Idle time in microseconds to wait before hopping into the
   * channel indicated by this entry.
   */
  uint32_t delay;
  /** Deprecated field. Set to RAIL_RX_CHANNEL_HOPPING_DELAY_MODE_STATIC. */
  RAIL_RxChannelHoppingDelayMode_t delayMode;
  /**
   * Bitmask of various options that can be applied to the current
   * channel hop.
   */
  RAIL_RxChannelHoppingOptions_t options;
  /**
   * The RSSI threshold (in dBm) below which a hop will occur in
   * any mode when \ref RAIL_RX_CHANNEL_HOPPING_OPTION_RSSI_THRESHOLD is
   * specified.
   */
  int8_t rssiThresholdDbm;
  /**
   * Pad bytes reserved for future use and currently ignored.
   */
  uint8_t reserved2[1];
} RAIL_RxChannelHoppingConfigEntry_t;

/**
 * @struct RAIL_RxChannelHoppingConfig_t
 * @brief Wrapper struct that will contain the sequence of
 *   \ref RAIL_RxChannelHoppingConfigEntry_t that represents the channel
 *   sequence to use during RX Channel Hopping.
 */
typedef struct RAIL_RxChannelHoppingConfig {
  /**
   * Pointer to contiguous global read-write memory that will be used
   * by RAIL to store channel hopping information throughout its operation.
   * It need not be initialized and applications should never write
   * data anywhere in this buffer.
   *
   * @note the size of this buffer must be at least as large as
   * 3 + 30 * numberOfChannels, plus the sum of the sizes of the
   * radioConfigDeltaAdd's of the required channels, plus the size of the
   * radioConfigDeltaSubtract. In the case that one channel
   * appears two or more times in your channel sequence
   * (e.g., 1, 2, 1, 3), you must account for the radio configuration
   * size that number of times (i.e., need to count channel 1's
   * radio configuration size twice for the given example). The overall
   * 3 words and 30 words per channel needed in this buffer are
   * for internal use to the library.
   */
  uint32_t *buffer;
  /**
   * This parameter must be set to the length of the buffer array, in 32 bit
   * words. This way, during configuration, the software can confirm it's
   * writing within the range of the buffer. The configuration API will return
   * an error if bufferLength is insufficient.
   */
  uint16_t bufferLength;
  /**
   * The number of channels in the channel hopping sequence, which is the
   * number of elements in the array that entries points to.
   */
  uint8_t numberOfChannels;
  /**
   * A pointer to the first element of an array of \ref
   * RAIL_RxChannelHoppingConfigEntry_t that represents the channels
   * used during channel hopping. The length of this array must be
   * numberOfChannels.
   */
  RAIL_RxChannelHoppingConfigEntry_t *entries;
} RAIL_RxChannelHoppingConfig_t;

/**
 * @struct RAIL_RxDutyCycleConfig_t
 * @brief Structure to configure duty cycled receive mode.
 */
typedef struct RAIL_RxDutyCycleConfig {
  /** The mode by which RAIL determines when to exit RX. */
  RAIL_RxChannelHoppingMode_t mode;
  /**
   * Depending on the 'mode' parameter that was specified, this member
   * is used to parameterize that mode. See the comments on each value of
   * \ref RAIL_RxChannelHoppingMode_t to learn what to specify here.
   */
  // Unnamed 'uint8_t reserved[3]' pad byte field here.
  RAIL_RxChannelHoppingParameter_t parameter;
  /**
   * Idle time in microseconds to wait before re-entering RX.
   */
  uint32_t delay;
  /**
   * Indicate how the timing specified in 'delay' should be applied.
   */
  RAIL_RxChannelHoppingDelayMode_t delayMode;
  /**
   * Bitmask of various options that can be applied to the current
   * duty cycle operation when the mode is >= \ref
   * RAIL_RX_CHANNEL_HOPPING_MODE_MANUAL_WITH_OPTIONS (ignored otherwise).
   */
  RAIL_RxChannelHoppingOptions_t options;
  /**
   * The RSSI threshold (in dBm) below which Rx will end in
   * any mode when \ref RAIL_RX_CHANNEL_HOPPING_OPTION_RSSI_THRESHOLD
   * is specified.
   */
  int8_t rssiThresholdDbm;
  /**
   * Pad bytes reserved for future use and currently ignored.
   */
  uint8_t reserved2[1];
} RAIL_RxDutyCycleConfig_t;

/// A sentinel value to flag an invalid channel hopping index.
#define RAIL_CHANNEL_HOPPING_INVALID_INDEX (0xFEU)

/** @} */ // end of group Rx_Channel_Hopping

/******************************************************************************
 * Diagnostic Structures
 *****************************************************************************/
/**
 * @addtogroup Diagnostic
 * @{
 */

/**
 * @enum RAIL_StreamMode_t
 * @brief Possible stream output modes.
 */
RAIL_ENUM(RAIL_StreamMode_t) {
  RAIL_STREAM_CARRIER_WAVE = 0, /**< An unmodulated carrier wave. */
  RAIL_STREAM_PN9_STREAM = 1,   /**< PN9 byte sequence. */
  RAIL_STREAM_10_STREAM = 2, /**< 101010 sequence. */
  RAIL_STREAM_CARRIER_WAVE_PHASENOISE = 3, /**< An unmodulated carrier wave with no change to PLL BW. For series-2, same as RAIL_STREAM_CARRIER_WAVE */
  RAIL_STREAM_RAMP_STREAM = 4, /**< ramp sequence starting at a different offset for consecutive packets. Only available for some modulations. Fall back to RAIL_STREAM_PN9_STREAM if not available. */
  RAIL_STREAM_CARRIER_WAVE_SHIFTED = 5, /**< An unmodulated carrier wave not centered on DC but shifted roughly by channel_bandwidth/6 allowing an easy check of the residual DC. Only available for OFDM PA. Fall back to RAIL_STREAM_CARRIER_WAVE_PHASENOISE if not available. */
  RAIL_STREAM_MODES_COUNT   /**< A count of the choices in this enumeration. Must be last. */
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_STREAM_CARRIER_WAVE ((RAIL_StreamMode_t) RAIL_STREAM_CARRIER_WAVE)
#define RAIL_STREAM_PN9_STREAM   ((RAIL_StreamMode_t) RAIL_STREAM_PN9_STREAM)
#define RAIL_STREAM_10_STREAM   ((RAIL_StreamMode_t) RAIL_STREAM_10_STREAM)
#define RAIL_STREAM_CARRIER_WAVE_PHASENOISE ((RAIL_StreamMode_t) RAIL_STREAM_CARRIER_WAVE_PHASENOISE)
#define RAIL_STREAM_RAMP_STREAM   ((RAIL_StreamMode_t) RAIL_STREAM_RAMP_STREAM)
#define RAIL_STREAM_CARRIER_WAVE_SHIFTED ((RAIL_StreamMode_t) RAIL_STREAM_CARRIER_WAVE_SHIFTED)

#define RAIL_STREAM_MODES_COUNT   ((RAIL_StreamMode_t) RAIL_STREAM_MODES_COUNT)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * @def RAIL_VERIFY_DURATION_MAX
 * @brief This radio state verification duration indicates to RAIL that
 *   all memory contents should be verified by RAIL before returning to the
 *   application.
 */
#define RAIL_VERIFY_DURATION_MAX 0xFFFFFFFFUL

/**
 * A pointer to a verification callback function. This will be called by the
 * radio state verification feature built into RAIL when a verified memory
 * value is different from its reference value.
 *
 * @param[in] address The address of the data in question.
 * @param[in] expectedValue The expected value of the data in question.
 * @param[in] actualValue The actual value of the data in question.
 * @return bool True indicates a data value difference is acceptable. False
 *   indicates a data value difference in unacceptable.
 *
 * @note This callback will be issued when an address' value is different from
 *   its reference value and either of the following conditions are met:
 *   1) The default radio configuration provided by the radio configurator is used
 *      for verification purposes (i.e., a custom radio configuration is not supplied
 *      as an input to \ref RAIL_ConfigVerification()), and the radio
 *      configurator has flagged the address under question as being verifiable.
 *   2) A custom radio configuration is provided to the verification API (i.e.,
 *      a custom radio configuration is supplied as an input to \ref
 *      RAIL_ConfigVerification()). When providing a custom radio configuration for
 *      verification purposes, all addresses in that configuration will be verified,
 *      regardless of whether or not the addresses are flagged as verifiable.
 */
typedef bool (*RAIL_VerifyCallbackPtr_t)(uint32_t address,
                                         uint32_t expectedValue,
                                         uint32_t actualValue);

/**
 * @struct RAIL_VerifyConfig_t
 * @brief The configuration array provided to RAIL for use by the radio state
 *   verification feature. This structure will be populated with appropriate
 *   values by calling \ref RAIL_ConfigVerification(). The application should
 *   not set or alter any of these structure elements.
 */
typedef struct RAIL_VerifyConfig {
  /** Internal verification tracking information. */
  RAIL_Handle_t correspondingHandle;
  /** Internal verification tracking information. */
  uint32_t nextIndexToVerify;
  /** Internal verification tracking information. */
  const uint32_t *override;
  /** Internal verification tracking information. */
  RAIL_VerifyCallbackPtr_t cb;
} RAIL_VerifyConfig_t;

/** @} */ // end of group Diagnostic

/******************************************************************************
 * Energy Friendly Front End Module (EFF)
 *****************************************************************************/
/**
 * @addtogroup EFF
 * @{
 */

/**
 * @enum RAIL_EffDevice_t
 * @brief EFF part numbers.
 *
 * The part number of the attached EFF device is passed to
 * \ref RAIL_ConfigEff() as \ref RAIL_EffConfig_t.device.
 * The \ref rail_util_eff configures and controls
 * the EFF based on the capabilities of the attached EFF.
 */
RAIL_ENUM(RAIL_EffDevice_t) {
  RAIL_EFF_DEVICE_NONE = 0, /**< No EFF device attached. */
  RAIL_EFF_DEVICE_EFF01Z11, /**< EFF01Z11. LNA only, TX is bypass mode. */
  RAIL_EFF_DEVICE_EFF01A12, /**< EFF01A12. */
  RAIL_EFF_DEVICE_COUNT,    /**< A count of the choices in this enumeration. */
};
/**
 * @def RAIL_EFF_SUPPORTS_TRANSMIT(x)
 * @brief A macro that checks for EFFxx devices that support high power transmit
 */
#define RAIL_EFF_SUPPORTS_TRANSMIT(x) ( ((x) == RAIL_EFF_DEVICE_EFF01A12) \
                                        )
/**
 * @def RAIL_EFF_SUPPORTS_RECEIVE(x)
 * @brief A macro that checks for EFFxx devices that support receive
 */
#define RAIL_EFF_SUPPORTS_RECEIVE(x) ( ((x) == RAIL_EFF_DEVICE_EFF01A12)    \
                                       || ((x) == RAIL_EFF_DEVICE_EFF01Z11) \
                                       )

/**
 * @enum RAIL_EffLnaMode_t
 * @brief EFF LNA Modes.
 *
 * The enabled EFF LNA modes are passed to\ref RAIL_ConfigEff() as the
 * \ref RAIL_EffConfig_t.enabledLnaModes.
 * The \ref rail_util_eff dynamically transitions between enabled LNA modes to
 * maximize receive performance.
 */
RAIL_ENUM(RAIL_EffLnaMode_t) {
  RAIL_EFF_LNA_MODE_RURAL   = (0x01U << 0), /**< Rural LNA Mode. */
  RAIL_EFF_LNA_MODE_URBAN   = (0x01U << 1), /**< Urban LNA Mode. */
  RAIL_EFF_LNA_MODE_BYPASS  = (0x01U << 2), /**< Bypass LNA Mode. */
  RAIL_EFF_LNA_MODE_COUNT   = (0x01U << 3), /**< A count of the choices in this enumeration. */
};

/**
 * @enum RAIL_ClpcEnable_t
 * @brief EFF Closed Loop Power Control (CLPC) Enable states.
 *
 * The EFF CLPC Enable state is passed to \ref RAIL_ConfigEff() as the
 * \ref RAIL_EffConfig_t.clpcEnable.
 * The \ref rail_util_eff uses advanced power controls to tune EFF output to match
 * Surface Acoustic Wave (SAW) filter losses and antenna performance.
 */
RAIL_ENUM(RAIL_ClpcEnable_t) {
  RAIL_EFF_CLPC_DISABLED                  = 0,  /**<CLPC actions are completely disabled. EFF will output but CLPC will not change modes, take power measurements or tune power output. Temperature measurements are taken.*/
  RAIL_EFF_CLPC_MODE_CHANGE               = 1,  /**<CLPC actions are completely disabled. EFF will output, change modes, and take measurements, but not tune power output. */
  RAIL_EFF_CLPC_POWER_SLOW                = 2,  /**<CLPC actions allows Slow Loop. EFF will output, change modes, and take measurements, and tune power output based on slow loop. */
  RAIL_EFF_CLPC_POWER_FAST                = 3,  /**<CLPC actions allows Fast Loop. EFF will output, change modes, and take measurements, and tune power output based on fast loop. */
  RAIL_EFF_CLPC_POWER_BOTH                = 4,  /**<CLPC actions are completely enabled. EFF will output, change modes, take measurements, and tune power output based on slow and fast loops. Default state. */
  RAIL_EFF_CLPC_POWER_SLOW_BELOW_MAX      = 5,  /**<CLPC actions allows Slow Loop. EFF will output, change modes, and take measurements, but not tune power output because requested output power is less than max. Internal only state. */
  RAIL_EFF_CLPC_POWER_FAST_BELOW_MAX      = 6,  /**<CLPC actions allows Fast Loop. EFF will output, change modes, and take measurements,  but not tune power output because requested output power is less than max. Internal only state. */
  RAIL_EFF_CLPC_POWER_BOTH_BELOW_MAX      = 7,  /**<CLPC actions are completely enabled. EFF will output, change modes, and take measurements, but not tune power output because requested output power is less than max. Internal only state. */
  RAIL_EFF_CLPC_COUNT                     = 8   /**< A count of the choices in this enumeration. Must be last */
};

/**
 * @struct RAIL_FemProtectionConfig_t
 *
 * @brief Temperature protection configuration for the attached FEM.
 */
typedef struct RAIL_FemProtectionConfig {
  RAIL_TxPower_t PMaxContinuousTx;  /**< Power limit at FEM output, in deci-dBm */
  uint8_t txDutyCycle;              /**< TX duty cycle limit, in percentage */
  uint32_t reserved[2];             /**< Reserved. Values ignored. */
} RAIL_FemProtectionConfig_t;

/**
 * @struct RAIL_EffConfig_t
 *
 * @brief Configuration data for the attached EFF device.
 *
 * A structure of type \ref RAIL_EffConfig_t is passed to \ref RAIL_ConfigEff().
 */
typedef struct RAIL_EffConfig {
  RAIL_EffDevice_t device;            /**< EFF Device Type */
  uint8_t ctrl0Port;                  /**< CTRL0 output GPIO port */
  uint8_t ctrl0Pin;                   /**< CTRL0 output GPIO pin */
  uint8_t ctrl1Port;                  /**< CTRL1 output GPIO port */
  uint8_t ctrl1Pin;                   /**< CTRL1 output GPIO pin */
  uint8_t ctrl2Port;                  /**< CTRL2 output GPIO port */
  uint8_t ctrl2Pin;                   /**< CTRL2 output GPIO pin */
  uint8_t ctrl3Port;                  /**< CTRL3 output GPIO port */
  uint8_t ctrl3Pin;                   /**< CTRL3 output GPIO pin */
  uint8_t testPort;                   /**< TEST output GPIO port */
  uint8_t testPin;                    /**< TEST output GPIO pin */
  uint8_t sensePort;                  /**< SENSE input GPIO port */
  uint8_t sensePin;                   /**< SENSE input GPIO pin */
  RAIL_EffLnaMode_t enabledLnaModes;  /**< LNA modes enable bitmask **/
  uint16_t ruralUrbanMv;              /**< Trip point from rural to urban mode, in millivolts */
  uint16_t urbanBypassMv;             /**< Trip point from urban to bypass mode, in millivolts */
  uint16_t lnaReserved;               /**< Reserved for future use */
  uint32_t urbanDwellTimeMs;          /**< Time to stay in urban mode before transitioning to rural mode, in milliseconds */
  uint32_t bypassDwellTimeMs;         /**< Time to stay in bypass mode before transitioning to urban or rural mode, in milliseconds */
  uint16_t clpcSlowLoopTarget;        /**< Target for Closed Loop Power Control (CLPC) slow loop, in milliwatts. */
  uint16_t clpcSlowLoopSlope;         /**< Slope for CLPC slow loop, in delta-GAINDIG/delta-milliwatts. */
  uint16_t clpcFastLoopTarget;        /**< Target for CLPC fast loop, in millivolts. */
  uint16_t clpcFastLoopSlope;         /**< Slope for CLPC fast loop, in delta-GAINDIG/delta-millivolts. */
  uint8_t  clpcReserved;              /**< Reserved for future use */
  RAIL_ClpcEnable_t clpcEnable;       /**< Select CLPC mode */
  uint8_t maxTxContinuousPowerDbm;    /**< Maximum continuous power (in dBm) */
  uint8_t maxTxDutyCycle;             /**< Maximum transmit duty cycle (as a
                                           percentage) */
  uint16_t effTempThreshold;          /**< Temperature of EFF above which transmit
                                           is not allowed, in degrees Kelvin */
  uint16_t internalTempThreshold;     /**< Chip's internal temperature above which transmit
                                           is not allowed, in degrees Kelvin */
} RAIL_EffConfig_t;

/** @} */ // end of group EFF

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/******************************************************************************
 * Debug Structures
 *****************************************************************************/
/**
 * @addtogroup Debug
 * @{
 */

/**
 * @def RAIL_DEBUG_MODE_FREQ_OVERRIDE
 * @brief A bitmask to enable the frequency override debug mode to
 *   manually tune to a specified frequency. Note that this should only be used
 *   for testing and is not as tuned as frequencies from the calculator.
 */
#define RAIL_DEBUG_MODE_FREQ_OVERRIDE  0x00000001UL

/**
 * @def RAIL_DEBUG_MODE_VALID_MASK
 * @brief Any debug mode bits outside of this mask are invalid and ignored.
 */
#define RAIL_DEBUG_MODE_VALID_MASK     (~(RAIL_DEBUG_MODE_FREQ_OVERRIDE))

/** @} */ // end of group Debug

#endif//DOXYGEN_SHOULD_SKIP_THIS

/** @} */ // end of RAIL_API

#ifdef __cplusplus
}
#endif

// Include appropriate chip-specific types and APIs *after* common types, and
// *before* types that require chip-specific abstractions.
#include "rail_chip_specific.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RAIL_API
 * @{
 */

/**
 * @addtogroup State_Transitions
 * @{
 */

/**
 * @def RAIL_TRANSITION_TIME_KEEP
 * @brief A value to use in \ref RAIL_StateTiming_t fields when
 *   calling \ref RAIL_SetStateTiming() to keep that timing
 *   parameter at it current setting.
 */
#define RAIL_TRANSITION_TIME_KEEP ((RAIL_TransitionTime_t) -1)

/**
 * @struct RAIL_StateTiming_t
 * @brief A timing configuration structure for the RAIL State Machine.
 *
 * Configure the timings of the radio state transitions for common situations.
 * All of the listed timings are in microseconds. Transitions from an active
 * radio state to idle are not configurable, and will always happen as fast
 * as possible.
 * No timing value can exceed \ref RAIL_MAXIMUM_TRANSITION_US.
 * Use \ref RAIL_TRANSITION_TIME_KEEP to keep an existing setting.
 *
 * For idleToRx, idleToTx, rxToTx, txToRx, and txToTx a value of 0 for the
 * transition time means that the specified transition should happen as fast
 * as possible, even if the timing cannot be as consistent. Otherwise, the
 * timing value cannot be below \ref RAIL_MINIMUM_TRANSITION_US.
 *
 * For idleToTx, rxToTx, and txToTx setting a longer \ref
 * RAIL_TxPowerConfig_t::rampTime may result in a larger minimum value.
 *
 * For rxSearchTimeout and txToRxSearchTimeout, there is no minimum value. A
 * value of 0 disables the feature, functioning as an infinite timeout.
 */
typedef struct RAIL_StateTiming {
  RAIL_TransitionTime_t idleToRx; /**< Transition time from IDLE to RX. */
  RAIL_TransitionTime_t txToRx; /**< Transition time from TX to RX. */
  RAIL_TransitionTime_t idleToTx; /**< Transition time from IDLE to RX. */
  RAIL_TransitionTime_t rxToTx; /**< Transition time from RX packet to TX. */
  RAIL_TransitionTime_t rxSearchTimeout; /**< Length of time the radio will search for a
                                            packet when coming from idle or RX. */
  RAIL_TransitionTime_t txToRxSearchTimeout; /**< Length of time the radio will search for a
                                                packet when coming from TX. */
  RAIL_TransitionTime_t txToTx; /**< Transition time from TX packet to TX. */
} RAIL_StateTiming_t;

/** @} */ // end of group State_Transitions

/**
 * @addtogroup Transmit
 * @{
 */

/**
 * @enum RAIL_TxRepeatOptions_t
 * @brief Transmit repeat options, in reality a bitmask.
 */
RAIL_ENUM_GENERIC(RAIL_TxRepeatOptions_t, uint16_t) {
  /** Shift position of \ref RAIL_TX_REPEAT_OPTION_HOP bit */
  RAIL_TX_REPEAT_OPTION_HOP_SHIFT = 0,
};

/** A value representing no repeat options enabled. */
#define RAIL_TX_REPEAT_OPTIONS_NONE 0U
/** All repeat options disabled by default. */
#define RAIL_TX_REPEAT_OPTIONS_DEFAULT RAIL_TX_REPEAT_OPTIONS_NONE
/**
 * An option to configure whether or not to channel-hop before each
 * repeated transmit.
 */
#define RAIL_TX_REPEAT_OPTION_HOP (1U << RAIL_TX_REPEAT_OPTION_HOP_SHIFT)

/// @struct RAIL_TxRepeatConfig_t
/// @brief A configuration structure for repeated transmits
///
/// @note The PA will always be ramped down and up in between transmits so
/// there will always be some minimum delay between transmits depending on the
/// ramp time configuration.
typedef struct RAIL_TxRepeatConfig {
  /**
   * The number of repeated transmits to run. A total of (iterations + 1)
   * transmits will go on-air in the absence of errors.
   */
  uint16_t iterations;
  /**
   * Repeat option(s) to apply.
   */
  RAIL_TxRepeatOptions_t repeatOptions;
  /**
   * Per-repeat delay or hopping configuration, depending on repeatOptions.
   */
  union {
    /**
     * When \ref RAIL_TX_REPEAT_OPTION_HOP is not set, specifies
     * the delay time between each repeated transmit. Specify \ref
     * RAIL_TRANSITION_TIME_KEEP to use the current \ref
     * RAIL_StateTiming_t::txToTx transition time setting.
     */
    RAIL_TransitionTime_t delay;
    /**
     * When \ref RAIL_TX_REPEAT_OPTION_HOP is set, this specifies
     * the channel hopping configuration to use when hopping between
     * repeated transmits. Per-hop delays are configured within each
     * \ref RAIL_TxChannelHoppingConfigEntry_t::delay rather than
     * this union's delay field.
     */
    RAIL_TxChannelHoppingConfig_t channelHopping;
  } delayOrHop;
} RAIL_TxRepeatConfig_t;

/// RAIL_TxRepeatConfig_t::iterations initializer configuring infinite
/// repeated transmissions.
#define RAIL_TX_REPEAT_INFINITE_ITERATIONS (0xFFFFU)

/** @} */ // end of group Transmit

/** @} */ // end of RAIL_API

#ifdef __cplusplus
}
#endif

#endif  // __RAIL_TYPES_H__
