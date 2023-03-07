/******************************************************************************

 @file  utc_clock.c

 @brief This file contains the UTC clock definitions for use by
        Application threads.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2011-2022, Texas Instruments Incorporated
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

/*********************************************************************
 * INCLUDES
 */

#include <ti/drivers/dpl/ClockP.h>
#include "comdef.h"
#include "utc_clock.h"

/*********************************************************************
 * MACROS
 */

#define	YearLength(yr)	(IsLeapYear(yr) ? 366 : 365)

/*********************************************************************
 * CONSTANTS
 */

// Update every 1000ms
#define UTC_UPDATE_PERIOD  1000

#define	BEGYEAR	           2000     // UTC started at 00:00:00 January 1, 2000

#define	DAY                86400UL  // 24 hours * 60 minutes * 60 seconds

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// Clock instance used to update UTC clock.
ClockP_Struct UTC_clock;

// Time is the number of seconds since 0 hrs, 0 minutes, 0 seconds, on the
// 1st of January 2000 UTC.
UTCTime UTC_timeSeconds = 0;

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */
static uint8_t UTC_monthLength(uint8_t lpyr, uint8_t mon);

static void UTC_clockUpdate(uint32_t elapsedMSec);

static void UTC_timeUpdateHandler(void);

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/


/*********************************************************************
 * @fn      UTC_init
 *
 * @brief   Initialize the UTC clock module.  Sets up and starts the
 *          clock instance.
 *
 * @param   None.
 *
 * @return  None.
 */
void UTC_init(void)
{
  // Construct a periodic clock with a 1000ms duration and period to start
  // immediately.
  // Util_constructClock(&UTC_clock, UTC_timeUpdateHandler, UTC_UPDATE_PERIOD,
  //                     UTC_UPDATE_PERIOD, true, 0);
  ClockP_Params clockParams;

  // Convert UTC_UPDATE_PERIOD in milliseconds to ticks.
  uint32_t clockTicks = UTC_UPDATE_PERIOD * (1000 / ClockP_getSystemTickPeriod());

  // Setup parameters.
  ClockP_Params_init(&clockParams);

  // Setup argument.
  clockParams.arg = 0;

  // If period is 0, this is a one-shot timer.
  clockParams.period = clockTicks;

  // Starts immediately after construction if true, otherwise wait for a call
  // to start.
  clockParams.startFlag = true;

  // Initialize clock instance.
  ClockP_construct(&UTC_clock, (ClockP_Fxn)&UTC_timeUpdateHandler, clockTicks, &clockParams);
}

/*********************************************************************
 * @fn      UTC_timeUpdateHandler
 *
 * @brief   Expiration callback for UTC clock instance.
 *          Each time this is called the internal counter is updated
 *
 *
 * @param   None.
 *
 * @return  None.
 */
void UTC_timeUpdateHandler(void)
{
  static uint32_t prevClockTicks = 0;
  static uint16_t remUsTicks = 0;
  uint32_t clockTicks, elapsedClockTicks;
  uint32_t elapsedMSec = 0;

  // Get the running count of clock ticks.
  clockTicks = ClockP_getSystemTicks();

  // Check that time has passed.
  if (clockTicks != prevClockTicks)
  {
    // To make sure time has passed and that a negative difference is not
    // calculated, check if the tick count is greater than the previous
    // measurement's.
    if (clockTicks > prevClockTicks)
    {
      // Get the elapsed clock ticks.
      elapsedClockTicks = clockTicks - prevClockTicks;
    }
    // Else tick count rolled over.
    else
    {
      // Get the elapsed clock ticks, accounting for the roll over.
      elapsedClockTicks = (0xFFFFFFFF - prevClockTicks) + clockTicks + 1;
    }

    // Convert to milliseconds.
    elapsedMSec = (elapsedClockTicks * ClockP_getSystemTickPeriod()) / 1000;

    // Find remainder.
    remUsTicks += (elapsedClockTicks * ClockP_getSystemTickPeriod()) % 1000;

    // If the running total of remaining microseconds is greater than or equal
    // to one millisecond.
    if (remUsTicks >= 1000)
    {
      // Add in the extra millisecond.
      // Note: the remainder has an open upper limit of 2 milliseconds.
      elapsedMSec += 1;

      // Adjust the remainder.
      remUsTicks %= 1000;
    }
  }

  // If time has passed
  if (elapsedMSec)
  {
    // Store the tick count for the next iteration through this function.
    prevClockTicks = clockTicks;

    // Update the UTC Clock.
    UTC_clockUpdate(elapsedMSec);
  }
}

/*********************************************************************
 * @fn      UTC_clockUpdate
 *
 * @brief   Updates the UTC Clock time with elapsed milliseconds.
 *
 * @param   elapsedMSec - elapsed milliseconds
 *
 * @return  none
 */
static void UTC_clockUpdate(uint32_t elapsedMSec)
{
  static uint32_t timeMSec = 0;

  // Add elapsed milliseconds to the saved millisecond portion of time.
  timeMSec += elapsedMSec;

  // Roll up milliseconds to the number of seconds.
  if (timeMSec >= 1000)
  {
    UTC_timeSeconds += timeMSec / 1000;
    timeMSec = timeMSec % 1000;
  }
}

/*********************************************************************
 * @fn      UTC_setClock
 *
 * @brief   Set a new time.  This will only set the seconds portion
 *          of time and doesn't change the factional second counter.
 *
 * @param   newTime - Number of seconds since 0 hrs, 0 minutes,
 *                    0 seconds, on the 1st of January 2000 UTC.
 *
 * @return  none
 */
void UTC_setClock(UTCTime newTime)
{
  UTC_timeSeconds = newTime;
}

/*********************************************************************
 * @fn      UTC_getClock
 *
 * @brief   Gets the current time.  This will only return the seconds
 *          portion of time and doesn't include the factional second
 *          counter.
 *
 * @param   none
 *
 * @return  number of seconds since 0 hrs, 0 minutes, 0 seconds,
 *          on the 1st of January 2000 UTC
 */
UTCTime UTC_getClock(void)
{
  return (UTC_timeSeconds);
}

/*********************************************************************
 * @fn      UTC_convertUTCTime
 *
 * @brief   Converts UTCTime to UTCTimeStruct (from total seconds to exact
 *          date).
 *
 * @param   tm - pointer to breakdown struct.
 *
 * @param   secTime - number of seconds since 0 hrs, 0 minutes,
 *          0 seconds, on the 1st of January 2000 UTC.
 *
 * @return  none
 */
void UTC_convertUTCTime(UTCTimeStruct *tm, UTCTime secTime)
{
  // Calculate the time less than a day - hours, minutes, seconds.
  {
    // The number of seconds that have occured so far stoday.
    uint32_t day = secTime % DAY;

    // Seconds that have passed in the current minute.
    tm->seconds = day % 60UL;
    // Minutes that have passed in the current hour.
    // (seconds per day) / (seconds per minute) = (minutes on an hour boundary)
    tm->minutes = (day % 3600UL) / 60UL;
    // Hours that have passed in the current day.
    tm->hour = day / 3600UL;
  }

  // Fill in the calendar - day, month, year
  {
    uint16_t numDays = secTime / DAY;
    uint8_t monthLen;
    tm->year = BEGYEAR;

    while (numDays >= YearLength(tm->year))
    {
      numDays -= YearLength(tm->year);
      tm->year++;
    }

    // January.
    tm->month = 0;

    monthLen = UTC_monthLength(IsLeapYear(tm->year), tm->month);

    // Determine the number of months which have passed from remaining days.
    while (numDays >= monthLen)
    {
      // Subtract number of days in month from remaining count of days.
      numDays -= monthLen;
      tm->month++;

      // Recalculate month length.
      monthLen = UTC_monthLength(IsLeapYear(tm->year), tm->month);
    }

    // Store the remaining days.
    tm->day = numDays;
  }
}

/*********************************************************************
 * @fn      UTC_monthLength
 *
 * @param   lpyr - 1 for leap year, 0 if not
 *
 * @param   mon - 0 - 11 (jan - dec)
 *
 * @return  number of days in specified month
 */
static uint8_t UTC_monthLength(uint8_t lpyr, uint8_t mon)
{
  uint8_t days = 31;

  if (mon == 1) // feb
  {
    days = (28 + lpyr);
  }
  else
  {
    if (mon > 6) // aug-dec
    {
      mon--;
    }

    if (mon & 1)
    {
      days = 30;
    }
  }

  return (days);
}

/*********************************************************************
 * @fn      UTC_convertUTCSecs
 *
 * @brief   Converts a UTCTimeStruct to UTCTime (from exact date to total
 *          seconds).
 *
 * @param   tm - pointer to provided struct.
 *
 * @return  number of seconds since 00:00:00 on 01/01/2000 (UTC).
 */
UTCTime UTC_convertUTCSecs(UTCTimeStruct *tm)
{
  uint32_t seconds;

  // Seconds for the partial day.
  seconds = (((tm->hour * 60UL) + tm->minutes) * 60UL) + tm->seconds;

  // Account for previous complete days.
  {
    // Start with complete days in current month.
    uint16_t days = tm->day;

    // Next, complete months in current year.
    {
      int8 month = tm->month;
      while (--month >= 0)
      {
        days += UTC_monthLength(IsLeapYear(tm->year), month);
      }
    }

    // Next, complete years before current year.
    {
      uint16_t year = tm->year;
      while (--year >= BEGYEAR)
      {
        days += YearLength(year);
      }
    }

    // Add total seconds before partial day.
    seconds += (days * DAY);
  }

  return (seconds);
}
