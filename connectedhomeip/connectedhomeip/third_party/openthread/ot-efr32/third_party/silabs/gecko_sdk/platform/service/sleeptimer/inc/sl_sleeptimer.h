/***************************************************************************//**
 * @file
 * @brief SLEEPTIMER API definition.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

/***************************************************************************//**
 * @addtogroup sleeptimer Sleep Timer
 * @{
 ******************************************************************************/

#ifndef SL_SLEEPTIMER_H
#define SL_SLEEPTIMER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "sl_sleeptimer_config.h"
#include "em_device.h"
#include "sl_status.h"

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
#define SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG 0x01

#define SLEEPTIMER_ENUM(name) typedef uint8_t name; enum name##_enum
/// @endcond

/// Timestamp, wall clock time in seconds.
typedef uint32_t sl_sleeptimer_timestamp_t;

/// Time zone offset from UTC(second).
typedef int32_t sl_sleeptimer_time_zone_offset_t;

// Forward declaration
typedef struct sl_sleeptimer_timer_handle sl_sleeptimer_timer_handle_t;

/***************************************************************************//**
 * Typedef for the user supplied callback function which is called when
 * a timer expires.
 *
 * @param handle The timer handle.
 *
 * @param data An extra parameter for the user application.
 ******************************************************************************/
typedef void (*sl_sleeptimer_timer_callback_t)(sl_sleeptimer_timer_handle_t *handle, void *data);

/// @brief Timer structure for sleeptimer
struct sl_sleeptimer_timer_handle {
  void *callback_data;                     ///< User data to pass to callback function.
  uint8_t priority;                        ///< Priority of timer.
  uint16_t option_flags;                   ///< Option flags.
  sl_sleeptimer_timer_handle_t *next;      ///< Pointer to next element in list.
  sl_sleeptimer_timer_callback_t callback; ///< Function to call when timer expires.
  uint32_t timeout_periodic;               ///< Periodic timeout.
  uint32_t delta;                          ///< Delay relative to previous element in list.
  uint32_t timeout_expected_tc;            ///< Expected tick count of the next timeout (only used for periodic timer).
};

/// @brief Month enum.
SLEEPTIMER_ENUM(sl_sleeptimer_month_t) {
  MONTH_JANUARY = 0,
  MONTH_FEBRUARY = 1,
  MONTH_MARCH   = 2,
  MONTH_APRIL = 3,
  MONTH_MAY = 4,
  MONTH_JUNE = 5,
  MONTH_JULY = 6,
  MONTH_AUGUST = 7,
  MONTH_SEPTEMBER = 8,
  MONTH_OCTOBER = 9,
  MONTH_NOVEMBER = 10,
  MONTH_DECEMBER = 11,
};

/// @brief Week Day enum.
SLEEPTIMER_ENUM(sl_sleeptimer_weekDay_t) {
  DAY_SUNDAY = 0,
  DAY_MONDAY = 1,
  DAY_TUESDAY = 2,
  DAY_WEDNESDAY = 3,
  DAY_THURSDAY = 4,
  DAY_FRIDAY = 5,
  DAY_SATURDAY = 6,
};

/// @brief Time and Date structure.
typedef  struct  time_date {
  uint8_t sec;                                ///< Second (0-59)
  uint8_t min;                                ///< Minute of month (0-59)
  uint8_t hour;                               ///< Hour (0-23)
  uint8_t month_day;                          ///< Day of month (1-31)
  sl_sleeptimer_month_t month;                ///< Month (0-11)
  uint16_t year;                              ///< Year, based on a 0 Epoch or a 1900 Epoch.
  sl_sleeptimer_weekDay_t day_of_week;        ///< Day of week (0-6)
  uint16_t day_of_year;                       ///< Day of year (1-366)
  sl_sleeptimer_time_zone_offset_t time_zone; ///< Offset, in seconds, from UTC
} sl_sleeptimer_date_t;

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * Initializes the Sleeptimer.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sl_sleeptimer_init(void);

/***************************************************************************//**
 * Starts a 32 bits timer.
 *
 * @param handle Pointer to handle to timer.
 * @param timeout Timer timeout, in timer ticks.
 * @param callback Callback function that will be called when
 *        initial/periodic timeout expires.
 * @param callback_data Pointer to user data that will be passed to callback.
 * @param priority Priority of callback. Useful in case multiple timer expire
 *        at the same time. 0 = highest priority.
 * @param option_flags Bit array of option flags for the timer.
 *        Valid bit-wise OR of one or more of the following:
 *          - SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sl_sleeptimer_start_timer(sl_sleeptimer_timer_handle_t *handle,
                                      uint32_t timeout,
                                      sl_sleeptimer_timer_callback_t callback,
                                      void *callback_data,
                                      uint8_t priority,
                                      uint16_t option_flags);

/***************************************************************************//**
 * Restarts a 32 bits timer.
 *
 * @param handle Pointer to handle to timer.
 * @param timeout Timer timeout, in timer ticks.
 * @param callback Callback function that will be called when
 *        initial/periodic timeout expires.
 * @param callback_data Pointer to user data that will be passed to callback.
 * @param priority Priority of callback. Useful in case multiple timer expire
 *        at the same time. 0 = highest priority.
 * @param option_flags Bit array of option flags for the timer.
 *        Valid bit-wise OR of one or more of the following:
 *          - SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG
 *        or 0 for not flags.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sl_sleeptimer_restart_timer(sl_sleeptimer_timer_handle_t *handle,
                                        uint32_t timeout,
                                        sl_sleeptimer_timer_callback_t callback,
                                        void *callback_data,
                                        uint8_t priority,
                                        uint16_t option_flags);

/***************************************************************************//**
 * Starts a 32 bits periodic timer.
 *
 * @param handle Pointer to handle to timer.
 * @param timeout Timer periodic timeout, in timer ticks.
 * @param callback Callback function that will be called when
 *        initial/periodic timeout expires.
 * @param callback_data Pointer to user data that will be passed to callback.
 * @param priority Priority of callback. Useful in case multiple timer expire
 *        at the same time. 0 = highest priority.
 * @param option_flags Bit array of option flags for the timer.
 *        Valid bit-wise OR of one or more of the following:
 *          - SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG
 *        or 0 for not flags.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sl_sleeptimer_start_periodic_timer(sl_sleeptimer_timer_handle_t *handle,
                                               uint32_t timeout,
                                               sl_sleeptimer_timer_callback_t callback,
                                               void *callback_data,
                                               uint8_t priority,
                                               uint16_t option_flags);

/***************************************************************************//**
 * Restarts a 32 bits periodic timer.
 *
 * @param handle Pointer to handle to timer.
 * @param timeout Timer periodic timeout, in timer ticks.
 * @param callback Callback function that will be called when
 *        initial/periodic timeout expires.
 * @param callback_data Pointer to user data that will be passed to callback.
 * @param priority Priority of callback. Useful in case multiple timer expire
 *        at the same time. 0 = highest priority.
 * @param option_flags Bit array of option flags for the timer.
 *        Valid bit-wise OR of one or more of the following:
 *          - SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG
 *        or 0 for not flags.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sl_sleeptimer_restart_periodic_timer(sl_sleeptimer_timer_handle_t *handle,
                                                 uint32_t timeout,
                                                 sl_sleeptimer_timer_callback_t callback,
                                                 void *callback_data,
                                                 uint8_t priority,
                                                 uint16_t option_flags);

/***************************************************************************//**
 * Stops a timer.
 *
 * @param handle Pointer to handle to timer.
 *
 * @return
 ******************************************************************************/
sl_status_t sl_sleeptimer_stop_timer(sl_sleeptimer_timer_handle_t *handle);

/***************************************************************************//**
 * Gets the status of a timer.
 *
 * @param handle Pointer to handle to timer.
 * @param running Pointer to the status of the timer.
 *
 * @note A non periodic timer is considered not running during its callback.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sl_sleeptimer_is_timer_running(sl_sleeptimer_timer_handle_t *handle,
                                           bool *running);

/***************************************************************************//**
 * Gets remaining time until timer expires.
 *
 * @param handle Pointer to handle to timer.
 * @param time Time left in timer ticks.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sl_sleeptimer_get_timer_time_remaining(sl_sleeptimer_timer_handle_t *handle,
                                                   uint32_t *time);

/**************************************************************************//**
 * Gets the time remaining until the first timer with the matching set of flags
 * expires.
 *
 * @param option_flags Set of flags to match.
 *
 * @param time_remaining Time left in timer ticks.
 *
 * @return 0 if successful. Error code otherwise.
 *****************************************************************************/
sl_status_t sl_sleeptimer_get_remaining_time_of_first_timer(uint16_t option_flags,
                                                            uint32_t *time_remaining);

/***************************************************************************//**
 * Gets current 32 bits global tick count.
 *
 * @return Current tick count.
 ******************************************************************************/
uint32_t sl_sleeptimer_get_tick_count(void);

/***************************************************************************//**
 * Gets current 64 bits global tick count.
 *
 * @return Current tick count.
 ******************************************************************************/
uint64_t sl_sleeptimer_get_tick_count64(void);

/***************************************************************************//**
 * Get timer frequency.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
uint32_t sl_sleeptimer_get_timer_frequency(void);

#if SL_SLEEPTIMER_WALLCLOCK_CONFIG
/***************************************************************************//**
 * Converts a Unix timestamp into a date.
 *
 * @param time Unix timestamp to convert.
 * @param time_zone Offset from UTC in second.
 * @param date Pointer to converted date.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sl_sleeptimer_convert_time_to_date(sl_sleeptimer_timestamp_t time,
                                               sl_sleeptimer_time_zone_offset_t time_zone,
                                               sl_sleeptimer_date_t *date);

/***************************************************************************//**
 * Converts a date into a Unix timestamp.
 *
 * @param date Pointer to date to convert.
 * @param time Pointer to converted Unix timestamp.
 *
 * @return 0 if successful. Error code otherwise.
 *
 * @note Dates are based on the Unix time representation.
 *       Range of dates supported :
 *          - January 1, 1970, 00:00:00 to January 19, 2038, 03:14:00
 ******************************************************************************/
sl_status_t sl_sleeptimer_convert_date_to_time(sl_sleeptimer_date_t *date,
                                               sl_sleeptimer_timestamp_t *time);

/***************************************************************************//**
 * Convert date to string.
 *
 * @param str Output string.
 * @param size Size of the input array.
 * @param format The format specification character.
 * @param date Pointer to date structure.
 *
 * @return 0 if error. Number of character in the output string.
 *
 * @note Refer strftime() from UNIX.
 *       http://man7.org/linux/man-pages/man3/strftime.3.html
 ******************************************************************************/
uint32_t sl_sleeptimer_convert_date_to_str(char *str,
                                           size_t size,
                                           const uint8_t *format,
                                           sl_sleeptimer_date_t *date);

/***************************************************************************//**
 * Sets time zone offset.
 *
 * @param  offset  Time zone offset, in seconds.
 ******************************************************************************/
void sl_sleeptimer_set_tz(sl_sleeptimer_time_zone_offset_t offset);

/***************************************************************************//**
 * Gets time zone offset.
 *
 * @return Time zone offset, in seconds.
 ******************************************************************************/
sl_sleeptimer_time_zone_offset_t sl_sleeptimer_get_tz(void);

/***************************************************************************//**
 * Retrieves current time.
 *
 * @return Current timestamps in Unix format.
 ******************************************************************************/
sl_sleeptimer_timestamp_t sl_sleeptimer_get_time(void);

/***************************************************************************//**
 * Sets current time.
 *
 * @param time Time to set.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sl_sleeptimer_set_time(sl_sleeptimer_timestamp_t time);

/***************************************************************************//**
 * Gets current date.
 *
 * @param date Pointer to a sl_sleeptimer_date_t structure.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sl_sleeptimer_get_datetime(sl_sleeptimer_date_t *date);

/***************************************************************************//**
 * Sets current time, in date format.
 *
 * @param date Pointer to current date.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sl_sleeptimer_set_datetime(sl_sleeptimer_date_t *date);

/***************************************************************************//**
 * Builds a date time structure based on the provided parameters.
 *
 * @param date Pointer to the structure to be populated.
 * @param year Current year. May be provided based on a 0 Epoch or a 1900 Epoch.
 * @param month Months since January. Expected value: 0-11.
 * @param month_day Day of the month. Expected value: 1-31.
 * @param hour Hours since midnight. Expected value: 0-23.
 * @param min Minutes after the hour. Expected value: 0-59.
 * @param sec Seconds after the minute. Expected value: 0-59.
 * @param tzOffset Offset, in seconds, from UTC.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sl_sleeptimer_build_datetime(sl_sleeptimer_date_t *date,
                                         uint16_t year,
                                         sl_sleeptimer_month_t month,
                                         uint8_t month_day,
                                         uint8_t hour,
                                         uint8_t min,
                                         uint8_t sec,
                                         sl_sleeptimer_time_zone_offset_t tzOffset);

/***************************************************************************//**
 * Converts Unix timestamp into NTP timestamp.
 *
 * @param time Unix timestamp.
 * @param ntp_time Pointer to NTP Timestamp.
 *
 * @note Unix timestamp range supported : 0x0 to 0x7C55 817F
 *       ie. January 1, 1970, 00:00:00 to February 07, 2036, 06:28:15
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sl_sleeptimer_convert_unix_time_to_ntp(sl_sleeptimer_timestamp_t time,
                                                   uint32_t *ntp_time);

/***************************************************************************//**
 * Converts NTP timestamp into Unix timestamp.
 *
 * @param ntp_time NTP Timestamp.
 * @param time Pointer to Unix timestamp.
 *
 * @note NTP timestamp range supported : 0x83AA 7E80 to 0xFFFF FFFF
 *       ie. January 1, 1970, 00:00:00 to February 07, 2036, 06:28:15
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sl_sleeptimer_convert_ntp_time_to_unix(uint32_t ntp_time,
                                                   sl_sleeptimer_timestamp_t *time);

/***************************************************************************//**
 * Converts Unix timestamp into Zigbee timestamp.
 *
 * @param time Unix timestamp.
 *
 * @param zigbee_time Pointer to NTP Timestamp.
 *
 * @note Unix timestamp range supported : 0x386D 4380 to 0x7FFF FFFF
 *       ie. January 1, 2000, 00:00:0 to January 19, 2038, 03:14:00
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sl_sleeptimer_convert_unix_time_to_zigbee(sl_sleeptimer_timestamp_t time,
                                                      uint32_t *zigbee_time);

/***************************************************************************//**
 * Converts Zigbee timestamp into Unix timestamp.
 *
 * @param zigbee_time NTP Timestamp.
 * @param time Pointer to Unix timestamp.
 *
 * @note ZIGBEE timestamp range supported : 0x0 to 0x4792 BC7F
 *        ie. January 1, 2000, 00:00:00 to January 19, 2038, 03:14:00
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sl_sleeptimer_convert_zigbee_time_to_unix(uint32_t zigbee_time,
                                                      sl_sleeptimer_timestamp_t *time);

/***************************************************************************//**
 * Calculates offset for time zone after UTC-0.
 *
 * @param hours Number of hours from UTC-0.
 * @param minutes Number of minutes from UTC-0.
 *
 * @return The time zone offset in seconds.
 ******************************************************************************/
__STATIC_INLINE sl_sleeptimer_time_zone_offset_t sl_sleeptimer_set_tz_ahead_utc(uint8_t hours,
                                                                                uint8_t minutes)
{
  return ((hours * 3600u) + (minutes * 60u));
}

/***************************************************************************//**
 * Calculates offset for time zone before UTC-0.
 *
 * @param hours Number of hours to UTC-0.
 * @param minutes Number of minutes to UTC-0.
 *
 * @return The time zone offset in seconds.
 ******************************************************************************/
__STATIC_INLINE sl_sleeptimer_time_zone_offset_t sl_sleeptimer_set_tz_behind_utc(uint8_t hours,
                                                                                 uint8_t minutes)
{
  return -((hours * 3600u) + (minutes * 60u));
}
#endif

/***************************************************************************//**
 * Active delay.
 *
 * @param time_ms Delay duration in milliseconds.
 ******************************************************************************/
void sl_sleeptimer_delay_millisecond(uint16_t time_ms);

/***************************************************************************//**
 * Converts milliseconds in ticks.
 *
 * @param time_ms Number of milliseconds.
 *
 * @return Corresponding ticks number.
 *
 * @note The result is "rounded" to the superior tick number.
 *       This function is light and cannot fail so it should be privilegied to
 *       perform a millisecond to tick conversion.
 ******************************************************************************/
uint32_t sl_sleeptimer_ms_to_tick(uint16_t time_ms);

/***************************************************************************//**
 * Converts 32-bits milliseconds in ticks.
 *
 * @param time_ms Number of milliseconds.
 * @param tick Pointer to the converted tick number.
 *
 * @return 0 if successful. Error code otherwise.
 *
 * @note  The result is "rounded" to the superior tick number.
 *        If possible the sl_sleeptimer_ms_to_tick() function should be used.
 *
 * @note  This function converts the delay expressed in milliseconds to timer
 *        ticks (represented on 32 bits). This means that the value that can
 *        be passed to the argument 'time_ms' is limited. The maximum
 *        timeout value that can be passed to this function can be retrieved
 *        by calling sl_sleeptimer_get_max_ms32_conversion().
 *        If the value passed to 'time_ms' is too large,
 *        SL_STATUS_INVALID_PARAMETER will be returned.
 ******************************************************************************/
sl_status_t sl_sleeptimer_ms32_to_tick(uint32_t time_ms,
                                       uint32_t *tick);

/***************************************************************************//**
 * Gets the maximum value that can be passed to the functions that have a
 * 32-bits time or timeout argument expressed in milliseconds.
 *
 * @return Maximum time or timeout value in milliseconds.
 ******************************************************************************/
uint32_t sl_sleeptimer_get_max_ms32_conversion(void);

/***************************************************************************//**
 * Converts ticks in milliseconds.
 *
 * @param tick Number of tick.
 *
 * @return Corresponding milliseconds number.
 *
 * @note The result is rounded to the inferior millisecond.
 ******************************************************************************/
uint32_t sl_sleeptimer_tick_to_ms(uint32_t tick);

/***************************************************************************//**
 * Converts 64-bit ticks in milliseconds.
 *
 * @param tick Number of tick.
 * @param ms Pointer to the converted milliseconds number.
 *
 * @return 0 if successful. Error code otherwise.
 *
 * @note The result is rounded to the inferior millisecond.
 ******************************************************************************/
sl_status_t sl_sleeptimer_tick64_to_ms(uint64_t tick,
                                       uint64_t *ms);

/***************************************************************************//**
 * Allow sleep after ISR exit.
 *
 * @return true if sleep is allowed after ISR exit. False otherwise.
 ******************************************************************************/
bool sl_sleeptimer_is_power_manager_early_restore_timer_latest_to_expire(void);

/**************************************************************************//**
 * Starts a 32 bits timer.
 *
 * @param handle Pointer to handle to timer.
 * @param timeout_ms Timer timeout, in milliseconds.
 * @param callback Callback function that will be called when
 *        initial/periodic timeout expires.
 * @param callback_data Pointer to user data that will be passed to callback.
 * @param priority Priority of callback. Useful in case multiple timer expire
 *        at the same time. 0 = highest priority.
 * @param option_flags Bit array of option flags for the timer.
 *        Valid bit-wise OR of one or more of the following:
 *          - SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG
 *        or 0 for not flags.
 *
 * @return 0 if successful. Error code otherwise.
 *
 * @note  This function converts the delay expressed in milliseconds to timer
 *        ticks (represented on 32 bits). This means that the value that can
 *        be passed to the argument 'timeout_ms' is limited. The maximum
 *        timeout value that can be passed to this function can be retrieved
 *        by calling sl_sleeptimer_get_max_ms32_conversion().
 *        If the value passed to 'timeout_ms' is too large,
 *        SL_STATUS_INVALID_PARAMETER will be returned.
 *****************************************************************************/
__STATIC_INLINE sl_status_t sl_sleeptimer_start_timer_ms(sl_sleeptimer_timer_handle_t *handle,
                                                         uint32_t timeout_ms,
                                                         sl_sleeptimer_timer_callback_t callback,
                                                         void *callback_data,
                                                         uint8_t priority,
                                                         uint16_t option_flags)
{
  sl_status_t status;
  uint32_t timeout_tick;

  status = sl_sleeptimer_ms32_to_tick(timeout_ms, &timeout_tick);
  if (status != SL_STATUS_OK) {
    return status;
  }

  return sl_sleeptimer_start_timer(handle, timeout_tick, callback, callback_data, priority, option_flags);
}

/**************************************************************************//**
 * Restarts a 32 bits timer.
 *
 * @param handle Pointer to handle to timer.
 * @param timeout_ms Timer timeout, in milliseconds.
 * @param callback Callback function that will be called when
 *        initial/periodic timeout expires.
 * @param callback_data Pointer to user data that will be passed to callback.
 * @param priority Priority of callback. Useful in case multiple timer expire
 *        at the same time. 0 = highest priority.
 * @param option_flags Bit array of option flags for the timer.
 *        Valid bit-wise OR of one or more of the following:
 *          - SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG
 *        or 0 for not flags.
 *
 * @return 0 if successful. Error code otherwise.
 *
 * @note  This function converts the delay expressed in milliseconds to timer
 *        ticks (represented on 32 bits). This means that the value that can
 *        be passed to the argument 'timeout_ms' is limited. The maximum
 *        timeout value that can be passed to this function can be retrieved
 *        by calling sl_sleeptimer_get_max_ms32_conversion().
 *        If the value passed to 'timeout_ms' is too large,
 *        SL_STATUS_INVALID_PARAMETER will be returned.
 *****************************************************************************/
__STATIC_INLINE sl_status_t sl_sleeptimer_restart_timer_ms(sl_sleeptimer_timer_handle_t *handle,
                                                           uint32_t timeout_ms,
                                                           sl_sleeptimer_timer_callback_t callback,
                                                           void *callback_data,
                                                           uint8_t priority,
                                                           uint16_t option_flags)
{
  sl_status_t status;
  uint32_t timeout_tick;

  status = sl_sleeptimer_ms32_to_tick(timeout_ms, &timeout_tick);
  if (status != SL_STATUS_OK) {
    return status;
  }

  return sl_sleeptimer_restart_timer(handle, timeout_tick, callback, callback_data, priority, option_flags);
}

/***************************************************************************//**
 * Starts a 32 bits periodic timer.
 *
 * @param handle Pointer to handle to timer.
 * @param timeout_ms Timer periodic timeout, in milliseconds.
 * @param callback Callback function that will be called when
 *        initial/periodic timeout expires.
 * @param callback_data Pointer to user data that will be passed to callback.
 * @param priority Priority of callback. Useful in case multiple timer expire
 *        at the same time. 0 = highest priority.
 * @param option_flags Bit array of option flags for the timer.
 *        Valid bit-wise OR of one or more of the following:
 *          - SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG
 *        or 0 for not flags.
 *
 * @return 0 if successful. Error code otherwise.
 *
 * @note  This function converts the delay expressed in milliseconds to timer
 *        ticks (represented on 32 bits). This means that the value that can
 *        be passed to the argument 'timeout_ms' is limited. The maximum
 *        timeout value that can be passed to this function can be retrieved
 *        by calling sl_sleeptimer_get_max_ms32_conversion().
 *        If the value passed to 'timeout_ms' is too large,
 *        SL_STATUS_INVALID_PARAMETER will be returned.
 ******************************************************************************/
__STATIC_INLINE sl_status_t sl_sleeptimer_start_periodic_timer_ms(sl_sleeptimer_timer_handle_t *handle,
                                                                  uint32_t timeout_ms,
                                                                  sl_sleeptimer_timer_callback_t callback,
                                                                  void *callback_data,
                                                                  uint8_t priority,
                                                                  uint16_t option_flags)
{
  sl_status_t status;
  uint32_t timeout_tick;

  status = sl_sleeptimer_ms32_to_tick(timeout_ms, &timeout_tick);
  if (status != SL_STATUS_OK) {
    return status;
  }

  return sl_sleeptimer_start_periodic_timer(handle, timeout_tick, callback, callback_data, priority, option_flags);
}

/***************************************************************************//**
 * Restarts a 32 bits periodic timer.
 *
 * @param handle Pointer to handle to timer.
 * @param timeout_ms Timer periodic timeout, in milliseconds.
 * @param callback Callback function that will be called when
 *        initial/periodic timeout expires.
 * @param callback_data Pointer to user data that will be passed to callback.
 * @param priority Priority of callback. Useful in case multiple timer expire
 *        at the same time. 0 = highest priority.
 * @param option_flags Bit array of option flags for the timer.
 *        Valid bit-wise OR of one or more of the following:
 *          - SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG
 *        or 0 for not flags.
 *
 * @return 0 if successful. Error code otherwise.
 *
 * @note  This function converts the delay expressed in milliseconds to timer
 *        ticks (represented on 32 bits). This means that the value that can
 *        be passed to the argument 'timeout_ms' is limited. The maximum
 *        timeout value that can be passed to this function can be retrieved
 *        by calling sl_sleeptimer_get_max_ms32_conversion().
 *        If the value passed to 'timeout_ms' is too large,
 *        SL_STATUS_INVALID_PARAMETER will be returned.
 ******************************************************************************/
__STATIC_INLINE sl_status_t sl_sleeptimer_restart_periodic_timer_ms(sl_sleeptimer_timer_handle_t *handle,
                                                                    uint32_t timeout_ms,
                                                                    sl_sleeptimer_timer_callback_t callback,
                                                                    void *callback_data,
                                                                    uint8_t priority,
                                                                    uint16_t option_flags)
{
  sl_status_t status;
  uint32_t timeout_tick;

  status = sl_sleeptimer_ms32_to_tick(timeout_ms, &timeout_tick);
  if (status != SL_STATUS_OK) {
    return status;
  }

  return sl_sleeptimer_restart_periodic_timer(handle, timeout_tick, callback, callback_data, priority, option_flags);
}

#ifdef __cplusplus
}
#endif

/** @} (end addtogroup sleeptimer) */

/* *INDENT-OFF* */
/* THE REST OF THE FILE IS DOCUMENTATION ONLY! */
/// @addtogroup sleeptimer Sleep Timer
/// @{
///
///   @details
///   Sleep Timer can be used for creating timers which are tightly integrated with power management.
///   The Power Manager requires precision timing to have all clocks ready on time, so that wakeup
///   happens a little bit earlier to prepare the system to be ready at the right time.
///   Sleep Timer uses one Hardware Timer and creates multiple software timer instances.
///
///   The sleeptimer.c and sleeptimer.h source files for the SLEEPTIMER device driver library are in the
///   service/sleeptimer folder.
///
///   @li @ref sleeptimer_intro
///   @li @ref sleeptimer_functionalities_overview
///   @li @ref sleeptimer_getting_started
///   @li @ref sleeptimer_conf
///   @li @ref sleeptimer_api
///   @li @ref sleeptimer_example
///
///   @n @section sleeptimer_intro Introduction
///
///   The Sleeptimer driver provides software timers, delays, timekeeping and date functionalities using a low-frequency real-time clock peripheral.
///
///   All Silicon Labs microcontrollers equipped with the RTC or RTCC peripheral are currently supported. Only one instance of this driver can be initialized by the application.
///
///   @n @section sleeptimer_functionalities_overview Functionalities overview
///
///   @n @subsection software_timers Software Timers
///
///   This functionality allows the user to create periodic and one shot timers. A user callback can be associated with a timer and is called when the timer expires.
///
///   Timer structures must be allocated by the user. The function is called from within an interrupt handler with interrupts enabled.
///
///   @n @subsection timekeeping Timekeeping
///
///   A 64-bits tick counter is accessible through the @li uint64_t sl_sleeptimer_get_tick_count64(void) API. It keeps the tick count since the initialization of the driver
///
///   The `SL_SLEEPTIMER_WALLCLOCK_CONFIG` configuration enables a UNIX timestamp (seconds count since January 1, 1970, 00:00:00).
///
///   This timestamp can be retrieved/modified using the following API:
///
///   @li sl_sleeptimer_timestamp_t sl_sleeptimer_get_time(void);
///   @li sl_status_t sl_sleeptimer_set_time(sl_sleeptimer_timestamp_t time);
///
///   Convenience conversion functions are provided to convert UNIX timestamp to/from NTP and Zigbee cluster format :
///
///   @li sl_status_t sl_sleeptimer_convert_unix_time_to_ntp(sl_sleeptimer_timestamp_t time, uint32_t *ntp_time);
///   @li sl_status_t sl_sleeptimer_convert_ntp_time_to_unix(uint32_t ntp_time, sl_sleeptimer_timestamp_t *time);
///   @li sl_status_t sl_sleeptimer_convert_unix_time_to_zigbee(sl_sleeptimer_timestamp_t time, uint32_t *zigbee_time);
///   @li sl_status_t sl_sleeptimer_convert_zigbee_time_to_unix(uint32_t zigbee_time, sl_sleeptimer_timestamp_t *time);
///
///   @n @subsection date Date
///
///   The previously described internal timestamp can also be retrieved/modified in a date format sl_sleeptimer_date_t.
///
///   @n <b>API :</b> @n
///
///   @li sl_status_t sl_sleeptimer_get_datetime(sl_sleeptimer_date_t *date);
///   @li sl_status_t sl_sleeptimer_set_datetime(sl_sleeptimer_date_t *date);
///
///   @n @subsection frequency_setup Frequency Setup and Tick Count
///
///   This driver works with a configurable time unit called tick.
///
///   The frequency of the ticks is based on the clock source and the internal frequency divider.
///
///   One of the following clock sources must be enabled before initializing the sleeptimer:
///
///   @li LFXO: external crystal oscillator. Typically running at 32.768 kHz.
///   @li LFRCO: internal oscillator running at 32.768 kHz
///   @li ULFRCO: Ultra low-frequency oscillator running at 1.000 kHz
///
///   The frequency divider is selected with the `SL_SLEEPTIMER_FREQ_DIVIDER` configuration. Its value must be a power of two within the range of 1 to 32. The number of ticks per second (sleeptimer frequency) is dictated by the following formula:
///
///   Tick (seconds) = 1 / (clock_frequency / frequency_divider)
///
///   The highest resolution for a tick is 30.5 us. It is achieved with a 32.768 kHz clock and a divider of 1.
///
///   @n @section sleeptimer_getting_started Getting Started
///
///   @n @subsection  clock_selection Clock Selection
///
///   The sleeptimer relies on the hardware timer to operate. The hardware timer peripheral must be properly clocked from the application. Selecting the appropriate timer is crucial for design considerations. Each timer can potentially be used as a sleeptimer and is also available to the user. However, note that if a timer is used by the sleeptimer, it can't be used by the application and vice versa.
///
///   @n @subsection  Clock Selection in a Project without Micrium OS
///
///   When RTC, RTCC, or BURTC is selected, the clock source for the peripheral must be configured and enabled in the application before initializing the sleeptimer module or any communication stacks. Most of the time, it consists in enabling the desired oscillators and setting up the clock source for the peripheral, like in the following example:
///
///   @code{.c}
///   CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFRCO);
///   CMU_ClockEnable(cmuClock_RTCC, true);
///   @endcode
///
///   @n @subsection  clock_branch_select Clock Branch Select
///
///   | Clock  | Enum                    | Description                       | Frequency |
///   |--------|-------------------------|-----------------------------------|-----------|
///   | LFXO   | <b>cmuSelect_LFXO</b>   | Low-frequency crystal oscillator  |32.768 Khz |
///   | LFRCO  | <b>cmuSelect_LFRCO</b>  | Low-frequency RC oscillator       |32.768 Khz |
///   | ULFRCO | <b>cmuSelect_ULFRCO</b> | Ultra low-frequency RC oscillator |1 Khz      |
///
///   @n @subsection  timer_clock_enable Timer Clock Enable
///
///   | Module             | Enum                  | Description                                        |
///   |--------------------|-----------------------|----------------------------------------------------|
///   | RTCC               | <b>cmuClock_RTCC</b>  | Real-time counter and calendar clock (LF E branch) |
///   | RTC                | <b>cmuClock_RTC</b>   | Real time counter clock (LF A branch)              |
///   | BURTC              | <b>cmuClock_BURTC</b> | BURTC clock (EM4 Group A branch)                   |
///
///   When the Radio internal RTC (PRORTC) is selected, it is not necessary to configure the clock source for the peripheral. However, it is important to enable the desired oscillator before initializing the sleeptimer module or any communication stacks. The best oscillator available (LFXO being the first choice) will be used by the sleeptimer at initalization. The following example shows how the desired oscilator should be enabled:
///
///   @code{.c}
///   CMU_OscillatorEnable(cmuSelect_LFXO, true, true);
///   @endcode
///
///   @n @subsection  clock_micrium_os Clock Selection in a Project with Micrium OS
///
///   When Micrium OS is used, a BSP (all instances) is provided that sets up some parts of the clock tree. The sleeptimer clock source will be enabled by this bsp. However, the desired oscillator remains configurable from the file <b>bsp_cfg.h</b>.
///
///   The configuration `BSP_LF_CLK_SEL` determines which oscillator will be used by the sleeptimer's hardware timer peripheral. It can take the following values:
///
///   | Config                   | Description                       | Frequency |
///   |--------------------------|-----------------------------------|-----------|
///   | <b>BSP_LF_CLK_LFXO</b>   | Low-frequency crystal oscillator  |32.768 Khz |
///   | <b>BSP_LF_CLK_LFRCO</b>  | Low-frequency RC oscillator       |32.768 Khz |
///   | <b>BSP_LF_CLK_ULFRCO</b> | Ultra low-frequency RC oscillator |1 Khz      |
///
///   @n @section sleeptimer_conf Configuration Options
///
///   `SL_SLEEPTIMER_PERIPHERAL` can be set to one of the following values:
///
///   | Config                            | Description                                                                                          |
///   | --------------------------------- |------------------------------------------------------------------------------------------------------|
///   | `SL_SLEEPTIMER_PERIPHERAL_DEFAULT`| Selects either RTC or RTCC, depending of what is available on the platform.                          |
///   | `SL_SLEEPTIMER_PERIPHERAL_RTCC`   | Selects RTCC                                                                                         |
///   | `SL_SLEEPTIMER_PERIPHERAL_RTC`    | Selects RTC                                                                                          |
///   | `SL_SLEEPTIMER_PERIPHERAL_PRORTC` | Selects Internal radio RTC. Available only on EFR32XG13, EFR32XG14, EFR32XG21 and EFR32XG22 families.|
///   | `SL_SLEEPTIMER_PERIPHERAL_BURTC`  | Selects BURTC. Not available on Series 0 devices.                                                    |
///
///   `SL_SLEEPTIMER_WALLCLOCK_CONFIG` must be set to 1 to enable timestamp and date functionnalities.
///
///   `SL_SLEEPTIMER_FREQ_DIVIDER` must be a power of 2 within the range 1 to 32. When `SL_SLEEPTIMER_PERIPHERAL` is set to `SL_SLEEPTIMER_PERIPHERAL_PRORTC`, `SL_SLEEPTIMER_FREQ_DIVIDER` must be set to 1.
///
///   `SL_SLEEPTIMER_PRORTC_HAL_OWNS_IRQ_HANDLER` is only meaningful when `SL_SLEEPTIMER_PERIPHERAL` is set to `SL_SLEEPTIMER_PERIPHERAL_PRORTC`. Set to 1 if no communication stack is used in your project. Otherwise, must be set to 0.
///
///   @n @section sleeptimer_api The API
///
///   This section contains brief descriptions of the API functions. For
///   more information about input and output parameters and return values,
///   click on the hyperlinked function names. Most functions return an error
///   code, `SL_STATUS_OK` is returned on success,
///   see sl_status.h for other error codes.
///
///   The application code must include the @em sl_sleeptimer.h header file.
///
///   All API functions can be called from within interrupt handlers.
///
///   @ref sl_sleeptimer_init() @n
///    These functions initialize the sleeptimer driver. Typically,
///    @htmlonly sl_sleeptimer_init() @endhtmlonly is called once in the startup code.
///
///   @ref sl_sleeptimer_start_timer() @n
///    Start a one shot 32 bits timer. When a timer expires, a user-supplied callback function
///    is called. A pointer to this function is passed to
///    @htmlonly sl_sleeptimer_start_timer()@endhtmlonly. See @ref callback for
///    details of the callback prototype.
///
///   @ref sl_sleeptimer_restart_timer() @n
///    Restart a one shot 32 bits timer. When a timer expires, a user-supplied callback function
///    is called. A pointer to this function is passed to
///    @htmlonly sl_sleeptimer_start_timer()@endhtmlonly. See @ref callback for
///    details of the callback prototype.
///
///   @ref sl_sleeptimer_start_periodic_timer() @n
///    Start a periodic 32 bits timer. When a timer expires, a user-supplied callback function
///    is called. A pointer to this function is passed to
///    @htmlonly sl_sleeptimer_start_timer()@endhtmlonly. See @ref callback for
///    details of the callback prototype.
///
///   @ref sl_sleeptimer_restart_periodic_timer() @n
///    Restart a periodic 32 bits timer. When a timer expires, a user-supplied callback function
///    is called. A pointer to this function is passed to
///    @htmlonly sl_sleeptimer_start_timer()@endhtmlonly. See @ref callback for
///    details of the callback prototype.
///
///   @ref sl_sleeptimer_stop_timer() @n
///    Stop a timer.
///
///   @ref sl_sleeptimer_get_timer_time_remaining() @n
///    Get the time remaining before the timer expires.
///
///   @ref sl_sleeptimer_delay_millisecond() @n
///    Delay for the given number of milliseconds. This is an "active wait" delay function.
///
///   @ref sl_sleeptimer_is_timer_running() @n
///    Check if a timer is running.
///
///   @ref sl_sleeptimer_get_time(), @ref sl_sleeptimer_set_time() @n
///    Get or set wallclock time.
///
///   @ref sl_sleeptimer_ms_to_tick(), @ref sl_sleeptimer_ms32_to_tick(),
///   @ref sl_sleeptimer_tick_to_ms(), @ref sl_sleeptimer_tick64_to_ms() @n
///    Convert between milliseconds and RTC/RTCC
///    counter ticks.
///
///   @n @anchor callback <b>The timer expiry callback function:</b> @n
///   The callback function, prototyped as @ref sl_sleeptimer_timer_callback_t(), is called from
///   within the RTC peripheral interrupt handler on timer expiration.
///   @htmlonly sl_sleeptimer_timer_callback_t(sl_sleeptimer_timer_handle_t *handle, void *data)@endhtmlonly
///
///   @n @section sleeptimer_example Example
///   @code{.c}
///#include "sl_sleeptimer.h"
///
///void my_timer_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
///{
///  //Code executed when the timer expire.
///}
///
///int start_timer(void)
///{
///  sl_status_t status;
///  sl_sleeptimer_timer_handle_t my_timer;
///  uint32_t timer_timeout = 300;
///
///  // We assume the sleeptimer is initialized properly
///
///  status = sl_sleeptimer_start_timer(&my_timer,
///                                     timer_timeout,
///                                     my_timer_callback,
///                                     (void *)NULL,
///                                     0,
///                                     0);
///  if(status != SL_STATUS_OK) {
///    return -1;
///  }
///  return 1;
///}
///   @endcode
///
/// @} (end addtogroup sleeptimer)

#endif // SL_SLEEPTIMER_H
