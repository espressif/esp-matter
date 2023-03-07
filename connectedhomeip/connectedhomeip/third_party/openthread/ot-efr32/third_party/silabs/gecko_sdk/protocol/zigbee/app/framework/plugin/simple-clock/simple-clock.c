/***************************************************************************//**
 * @file
 * @brief Routines for the Simple Clock plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "app/framework/include/af.h"
#include "simple-clock.h"

// The variable "timeS" represents seconds since the ZigBee epoch, which was 0
// hours, 0 minutes, 0 seconds, on the 1st of January, 2000 UTC.  The variable
// "tickMs" is the millsecond tick at which that time was set.  The variable
// "remainderMs" is used to track sub-second chunks of time when converting
// from ticks to seconds.
static uint32_t timeS = 0;
static uint32_t tickMs = 0;
static uint16_t remainderMs = 0;

EmberAfPluginSimpleClockTimeSyncStatus syncStatus = EMBER_AF_SIMPLE_CLOCK_NEVER_UTC_SYNC;

// This event is used to periodically force an update to the internal time in
// order to avoid potential rollover problems with the system ticks.  A call to
// GetCurrentTime or SetTime will reschedule the event.
#ifdef UC_BUILD
sl_zigbee_event_t emberAfPluginSimpleClockUpdateEvent;
#define updateEvent (&emberAfPluginSimpleClockUpdateEvent)
void emberAfPluginSimpleClockUpdateEventHandler(SLXU_UC_EVENT);
#else
EmberEventControl emberAfPluginSimpleClockUpdateEventControl;
#define updateEvent emberAfPluginSimpleClockUpdateEventControl
#endif

#ifdef UC_BUILD

void emberAfPluginSimpleClockInitCallback(uint8_t init_level)
{
  switch (init_level) {
    case SL_ZIGBEE_INIT_LEVEL_EVENT:
    {
      slxu_zigbee_event_init(updateEvent,
                             emberAfPluginSimpleClockUpdateEventHandler);
      break;
    }

    case SL_ZIGBEE_INIT_LEVEL_DONE:
    {
      emberAfSetTimeCallback(0);
      break;
    }
  }
}

#else // !UC_BUILD

void emberAfPluginSimpleClockInitCallback(void)
{
  emberAfSetTimeCallback(0);
}

#endif // UC_BUILD

uint32_t emberAfGetCurrentTimeCallback(void)
{
  // Using system ticks, calculate how many seconds have elapsed since we last
  // got the time.  That amount plus the old time is our new time.  Remember
  // the tick time right now too so we can do the same calculations again when
  // we are next asked for the time.  Also, keep track of the sub-second chunks
  // of time during the conversion from ticks to seconds so the clock does not
  // drift due to rounding.
  uint32_t elapsedMs, lastTickMs = tickMs;
  tickMs = halCommonGetInt32uMillisecondTick();
  elapsedMs = elapsedTimeInt32u(lastTickMs, tickMs);
  timeS += elapsedMs / MILLISECOND_TICKS_PER_SECOND;
  remainderMs += elapsedMs % MILLISECOND_TICKS_PER_SECOND;
  if (MILLISECOND_TICKS_PER_SECOND <= remainderMs) {
    timeS++;
    remainderMs -= MILLISECOND_TICKS_PER_SECOND;
  }

  // Schedule an event to recalculate time to help avoid rollover problems.
  slxu_zigbee_event_set_delay_ms(updateEvent,
                                 MILLISECOND_TICKS_PER_DAY);
  return timeS;
}

uint32_t emberAfGetCurrentTimeSecondsWithMsPrecision(uint16_t* millisecondsRemainderReturn)
{
  uint32_t value = emberAfGetCurrentTimeCallback();
  *millisecondsRemainderReturn = remainderMs;
  return value;
}

void emberAfSetTimeCallback(uint32_t utcTime)
{
  tickMs = halCommonGetInt32uMillisecondTick();
  timeS = utcTime;
  remainderMs = 0;

  // Immediately get the new time in order to reschedule the event.
  emberAfGetCurrentTimeCallback();
}

void emberAfPluginSimpleClockSetUtcSyncedTime(uint32_t utcTime)
{
  emberAfSetTimeCallback(utcTime);
  syncStatus = EMBER_AF_SIMPLE_CLOCK_UTC_SYNCED;
}

EmberAfPluginSimpleClockTimeSyncStatus emberAfPluginSimpleClockGetTimeSyncStatus(void)
{
  return syncStatus;
}

void emberAfPluginSimpleClockUpdateEventHandler(SLXU_UC_EVENT)
{
  // Get the time, which will reschedule the event.

  // TODO: If time has not been synced for a while, set the status to
  // EMBER_AF_SIMPLE_CLOCK_STALE_UTC_SYNC

  emberAfGetCurrentTimeCallback();
}
