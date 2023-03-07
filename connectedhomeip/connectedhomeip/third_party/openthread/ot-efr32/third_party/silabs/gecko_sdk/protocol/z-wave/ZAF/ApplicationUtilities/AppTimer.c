/**
* @file
* @brief Application Timer module
*
* @copyright 2018 Silicon Laboratories Inc.
*/


/* Z-Wave includes */
#include <AppTimer.h>
#include <SwTimerLiaison.h>
#include <SizeOf.h>
#include <ZW_system_startup_api.h>
#include <zpal_retention_register.h>
//#define DEBUGPRINT
#include "DebugPrint.h"

#include <FreeRTOS.h>
#include <task.h>

#include "ZAF_PM_Wrapper.h"

/**
 * Max number of application timers
 */
#define MAX_NUM_APP_TIMERS           12 // Max number of timers total. I.e. the sum of normal timers and persistent timers
#define MAX_NUM_PERSISTENT_APP_TIMERS 6 // Do not increase. We have 8 retention registers allocated for ZAF/App timers. And 2 are used for admin purpose


/**
 * First (zero based) retention register to use for persisting application
 * timers during Deep Sleep. Other retention registers used for Deep Sleep persistent
 * app timers are defined as offsets from this value.
 */
#define FIRST_APP_TIMER_RETENTION_REGISTER        16

/** Retention register to use for persisting the task tick value at power down */
#define TASKTICK_AT_POWERDOWN_RETENTION_REGISTER  (FIRST_APP_TIMER_RETENTION_REGISTER + 0)

/**
 * Retention register to use for persisting the task tick value when the timer
 * values are saved to retention registers
 */
#define TASKTICK_AT_SAVETIMERS_RETENTION_REGISTER (FIRST_APP_TIMER_RETENTION_REGISTER + 1)

/**
 * First retention register to use for persisting the Deep Sleep persistent application
 * timers during Deep Sleep. (actual number of registers used is determined by
 * how many times AppTimerDeepSleepPersistentRegister() is called).
 */
#define TIMER_VALUES_BEGIN_RETENTION_REGISTER     (FIRST_APP_TIMER_RETENTION_REGISTER + 2)

/**
 * On wakeup from Deep Sleep, if the difference between expected timeout of an
 * Deep Sleep persistent application timer and the elapsed time at wake up, is
 * smaller than this value then the timer callback will be activated.
 */
#define APP_TIMER_TRIGGER_DELTA_MS 10

/**
* AppTimer object
*/
typedef struct  SAppTimer
{
  SSwTimerLiaison TimerLiaison;                     /**<  TimerLiaison object for Application task */
  SSwTimer* aTimerPointerArray[MAX_NUM_APP_TIMERS]; /**<  Array for TimerLiaison - for keeping registered timers */
  bool DeepSleepPersistent[MAX_NUM_APP_TIMERS];           /**<  Is timer persistent (persistent timers will be save/reloaded to/from retention registers during Deep Sleep hibernate) */
  void (*pDeepSleepCallback[MAX_NUM_APP_TIMERS])(SSwTimer* pTimer); /**< Holds the SSwTimer callback for Deep Sleep persistent timers. It will be called by AppTimerDeepSleepCallbackWrapper() */
} SAppTimer;


/**
* AppTimer is a singleton, thus containing own object.
*/
static SAppTimer g_AppTimer;

/**
 * Flag used to track if we have registered AppTimerSaveTaskTickAtSleep()
 * to be called at power down
 */
static bool g_PowerDownCallbackRegistered = false;


void AppTimerInit(uint8_t iTaskNotificationBitNumber, void * ReceiverTask)
{
  TimerLiaisonInit(    
                    &g_AppTimer.TimerLiaison,
                    sizeof_array(g_AppTimer.aTimerPointerArray),
                    g_AppTimer.aTimerPointerArray,
                    iTaskNotificationBitNumber,
                    (TaskHandle_t) ReceiverTask
                  );
}


bool AppTimerRegister(
                SSwTimer* pTimer,
                bool bAutoReload,
                void(*pCallback)(SSwTimer* pTimer)
              )
{
  ESwTimerLiaisonStatus status = TimerLiaisonRegister(&g_AppTimer.TimerLiaison,
                                                      pTimer,
                                                      bAutoReload,
                                                      pCallback);

  DPRINTF("AppTimerRegister() id=%d\n", pTimer->Id);

  // Do not fail if timer is already registered, just return
  if (status == ESWTIMERLIAISON_STATUS_ALREADY_REGISTRERED)
  {
    return true;
  }

  if (status == ESWTIMERLIAISON_STATUS_SUCCESS)
  {
    g_AppTimer.DeepSleepPersistent[pTimer->Id] = false;
    g_AppTimer.pDeepSleepCallback[pTimer->Id]  = NULL;
    return true;
  }

  return false;
}


/* This function will be called in the correct task context */
void AppTimerDeepSleepCallbackWrapper(SSwTimer* pTimer)
{
  DPRINTF("AppTimerDeepSleepCallbackWrapper timerId=%d\n", pTimer->Id);
  AppTimerDeepSleepPersistentSaveAll();

  ASSERT(g_AppTimer.DeepSleepPersistent[pTimer->Id] && g_AppTimer.pDeepSleepCallback[pTimer->Id]);

  if (g_AppTimer.pDeepSleepCallback[pTimer->Id])
  {
    DPRINTF("Calling g_AppTimer.pDeepSleepCallback[%d] = %p\n", pTimer->Id, g_AppTimer.pDeepSleepCallback[pTimer->Id]);
    (g_AppTimer.pDeepSleepCallback[pTimer->Id])(pTimer);
  }
}


bool AppTimerDeepSleepPersistentRegister(SSwTimer* pTimer,
                                   bool bAutoReload,
                                   void(*pCallback)(SSwTimer* pTimer))
{
  /* We don't support auto reload of Deep Sleep persistent timers (at least it has
   * not been tested - it might actually work now) */
  ASSERT(false == bAutoReload);

  /* Check that we have a retention register available for this new persistent timer */
  uint32_t count = 0;
  for (uint32_t timerId = 0; timerId < MAX_NUM_APP_TIMERS; timerId++)
  {
    if (true == g_AppTimer.DeepSleepPersistent[timerId])
    {
      count++;
    }
  }
  if (count >= MAX_NUM_PERSISTENT_APP_TIMERS)
  {
    /* All timer retention registers are taken */
    DPRINTF("AppTimerDeepSleepPersistentRegister: Max number of registrations exceeded (%d)\n", MAX_NUM_PERSISTENT_APP_TIMERS);
    return false;
  }

  /* We register AppTimerDeepSleepCallbackWrapper() as the call back in order to
   * update the timer status in retention registers when the timer expires.
   * The actual callback is saved to g_AppTimer.pDeepSleepCallback and will be
   * called by AppTimerDeepSleepCallbackWrapper() */
  ESwTimerLiaisonStatus status = TimerLiaisonRegister(&g_AppTimer.TimerLiaison,
                                                      pTimer,
                                                      bAutoReload,
                                                      AppTimerDeepSleepCallbackWrapper);

  DPRINTF("AppTimerDeepSleepPersistentRegister() id=%d pCallback=%p\n", pTimer->Id, pCallback);
  if (status == ESWTIMERLIAISON_STATUS_SUCCESS)
  {
    if (false == g_PowerDownCallbackRegistered)
    {
      /* Capture the task tick value just before going to sleep so we can
       * reload the Deep Sleep persistent application timers after waker-up */
      ZAF_PM_SetPowerDownCallback(AppTimerSaveTaskTickAtSleep);
      g_PowerDownCallbackRegistered = true;
    }

    g_AppTimer.DeepSleepPersistent[pTimer->Id] = true;
    g_AppTimer.pDeepSleepCallback[pTimer->Id]  = pCallback;
    return true;
  }

  return false;
}


ESwTimerStatus AppTimerDeepSleepPersistentStart(SSwTimer* pTimer, uint32_t iTimeout)
{
  DPRINTF("AppTimerDeepSleepPersistentStart() id=%d, timeout=%u\n", pTimer->Id, iTimeout);
  ESwTimerStatus status = TimerStart(pTimer, iTimeout);
  AppTimerDeepSleepPersistentSaveAll();
  return status;
}


ESwTimerStatus AppTimerDeepSleepPersistentRestart(SSwTimer* pTimer)
{
  DPRINTF("AppTimerDeepSleepPersistentRestart() id=%d\n", pTimer->Id);
  ESwTimerStatus status = TimerRestart(pTimer);
  AppTimerDeepSleepPersistentSaveAll();
  return status;
}


ESwTimerStatus AppTimerDeepSleepPersistentStop(SSwTimer* pTimer)
{
  DPRINTF("AppTimerDeepSleepPersistentStop() id=%d\n", pTimer->Id);
  ESwTimerStatus status = TimerStop(pTimer);
  AppTimerDeepSleepPersistentSaveAll();
  return status;
}


void AppTimerNotificationHandler(void)
{
  TimerLiaisonNotificationHandler(&g_AppTimer.TimerLiaison);
}


void AppTimerSetReceiverTask(void * ReceiverTask)
{
  TimerLiaisonSetReceiverTask(&g_AppTimer.TimerLiaison,
                              (TaskHandle_t) ReceiverTask);
}


uint32_t AppTimerGetFirstRetentionRegister(void)
{
  return FIRST_APP_TIMER_RETENTION_REGISTER;
}


uint32_t AppTimerGetLastRetentionRegister(void)
{
  uint32_t count = 0;
  for (uint32_t timerId = 0; timerId < MAX_NUM_APP_TIMERS; timerId++)
  {
    if (true == g_AppTimer.DeepSleepPersistent[timerId])
    {
      count++;
    }
  }
  return TIMER_VALUES_BEGIN_RETENTION_REGISTER + count - 1;
}


void AppTimerDeepSleepPersistentResetStorage(void)
{
  uint32_t first       = AppTimerGetFirstRetentionRegister();
  uint32_t last        = AppTimerGetLastRetentionRegister();

  DPRINTF("\nResetDeepSleepPersistentAppTimerStorage first=%u, last=%ux\n", first, last);

  ASSERT(first < last);
  for (uint32_t reg = first; reg <= last; reg++)
  {
    const zpal_status_t writeStatus = zpal_retention_register_write(reg, 0);
    ASSERT(ZPAL_STATUS_OK == writeStatus);
  }
}

/*
 * How the Deep Sleep persistent application timers are saved to RTCC retention registers
 *
 * For example the following list of APPLICATION TIMERS (contained in struct
 * g_AppTimer) is assumed (ordered by timer id)
 *
 * 0 (DeepSleepPersistent = false)
 * 1 (DeepSleepPersistent = false)
 * 2 (DeepSleepPersistent = true) MsUntilTimeout=30000
 * 3 (DeepSleepPersistent = true) MsUntilTimeout=0xFFFFFFFF (not active when saved)
 * 4 (DeepSleepPersistent = false)
 * 5 (DeepSleepPersistent = true) MsUntilTimeout=20000
 * 6 (DeepSleepPersistent = false)
 * 7 (DeepSleepPersistent = false)
 *
 * - DeepSleepPersistent is the flag in g_AppTimer ("true" implies that the
 *   timer should be saved, and it also implies that the timer exist)
 *
 * - MsUntilTimeout is the calculated number of milliseconds remaining
 *   before the timer times out. If equal to 0xFFFFFFFF then the timer is
 *   not active.
 *
 * The RETENTION REGISTERS will only contain the timer values for the Deep Sleep
 * persistent timers plus the task tick values when the timer values were
 * saved and when the device is going to sleep in Deep Sleep:
 *
 * 0 TaskTick at power-down
 * 1 TaskTick at save timers
 * 2 timerValue_ms=30000
 * 3 timerValue_ms=0xFFFFFFFF
 * 4 timerValue_ms=20000
 *
 * When the device wakes up from Deep Sleep the values in the retention registers
 * together with the time spent in Deep Sleep hibernate are used to determine if a
 * timer has expired or what value should be used to start it again to have
 * it time out at the right moment relative to its original start time.
 */


void AppTimerDeepSleepPersistentSaveAll(void)
{
  uint32_t reg = TIMER_VALUES_BEGIN_RETENTION_REGISTER;
  zpal_status_t writeStatus = ZPAL_STATUS_FAIL;

  uint32_t taskTickCount = xTaskGetTickCount();
  writeStatus = zpal_retention_register_write(TASKTICK_AT_SAVETIMERS_RETENTION_REGISTER, taskTickCount);
  ASSERT(ZPAL_STATUS_OK == writeStatus);

  DPRINTF("AppTimerDeepSleepPersistentSaveAll tick: %u\n", taskTickCount);

  for (uint32_t timerId = 0; timerId < MAX_NUM_APP_TIMERS; timerId++)
  {
    if (true == g_AppTimer.DeepSleepPersistent[timerId])
    {
      SSwTimer *pTimer        = g_AppTimer.aTimerPointerArray[timerId];
      uint32_t  timerValue_ms = UINT32_MAX;

      TimerGetMsUntilTimeout(pTimer, taskTickCount, &timerValue_ms);

      DPRINTF("Saving value for timer %d: %u (0x%x) ms\n", timerId, timerValue_ms, timerValue_ms);

      writeStatus = zpal_retention_register_write(reg, timerValue_ms);
      ASSERT(ZPAL_STATUS_OK == writeStatus);
      reg++;
    }
  }
}


void AppTimerSaveTaskTickAtSleep(void)
{
  uint32_t taskTickCount = xTaskGetTickCount();

  /* Called while the scheduler is disabled just before being forced into
   * Deep Sleep hibernate. If printing to serial line we need to delay the function
   * return to allow for the serial buffer content to be flushed */
#ifdef DEBUGPRINT
  DPRINTF("Saving task tick: %u\n", taskTickCount);
  for(int i=0; i < 2000; i++) __asm__("nop"); // Allow the serial line to flush before sleeping
#endif

  const zpal_status_t writeStatus = zpal_retention_register_write(TASKTICK_AT_POWERDOWN_RETENTION_REGISTER, taskTickCount);
  ASSERT(ZPAL_STATUS_OK == writeStatus);
}


void AppTimerDeepSleepPersistentLoadAll(EResetReason_t resetReason)
{
  uint32_t tickValueAtPowerDown  = 0;
  uint32_t tickValueAtSaveTimers = 0;
  zpal_status_t readStatus       = ZPAL_STATUS_FAIL;
  uint8_t  timerId               = 0;
  uint8_t  valIdx                = 0;
  uint32_t savedTimerValue       = 0;
  uint32_t elapsedMsFromSaveTimerValuesToSleep = 0;
  uint32_t elapsedMsFromTimerValueSave         = 0;
  uint32_t durationDiffMs = 0;
  bool deepSleepTimersStarted   = false;

  /* Do nothing if we did not wake up from Deep Sleep */
  if (ERESETREASON_DEEP_SLEEP_EXT_INT != resetReason && ERESETREASON_DEEP_SLEEP_WUT != resetReason)
  {
    return;
  }

  if (true == IsWakeupCausedByRtccTimeout())
  {
    DPRINT("\nRTCC wakeup!\n");
  }

  /* Read the task tick values saved before sleeping in Deep Sleep */
  readStatus = zpal_retention_register_read(TASKTICK_AT_POWERDOWN_RETENTION_REGISTER, &tickValueAtPowerDown);
  ASSERT(ZPAL_STATUS_OK == readStatus);
  DPRINTF("Loaded tickValueAtPowerDown: %u\n", tickValueAtPowerDown);

  readStatus = zpal_retention_register_read(TASKTICK_AT_SAVETIMERS_RETENTION_REGISTER, &tickValueAtSaveTimers);
  ASSERT(ZPAL_STATUS_OK == readStatus);
  DPRINTF("Loaded tickValueAtSaveTimers: %u\n", tickValueAtSaveTimers);

  if ((0 == tickValueAtPowerDown) || (0 == tickValueAtSaveTimers))
  {
    /* Retention registers are still at initial value - nothing to process now */
    return;
  }

  /* How many ms before power-down were the timer values saved to
   * retention registers? (NB: one tick = one millisecond) */
  if (tickValueAtPowerDown >= tickValueAtSaveTimers)
  {
    elapsedMsFromSaveTimerValuesToSleep = tickValueAtPowerDown - tickValueAtSaveTimers;
  }
  else
  {
    /* The 32-bit task tick has wrapped around
     * (VERY unlikely for a sleeping node) */
    elapsedMsFromSaveTimerValuesToSleep = (UINT32_MAX - tickValueAtSaveTimers) + tickValueAtPowerDown;
  }

  /* How many ms since the timer values were saved, including the
   * time spent sleeping */
  elapsedMsFromTimerValueSave = elapsedMsFromSaveTimerValuesToSleep +
                                GetCompletedSleepDurationMs();

  DPRINTF("elapsedMsFromTimerValueSave=%u\n", elapsedMsFromTimerValueSave);

  /* Read saved timer values from retention registers into array
   * while looking for smallest value larger than savedBeforePowerdownMs */
  for (timerId = 0; timerId < MAX_NUM_APP_TIMERS; timerId++)
  {
    if (true == g_AppTimer.DeepSleepPersistent[timerId])
    {
      readStatus = zpal_retention_register_read(TIMER_VALUES_BEGIN_RETENTION_REGISTER + valIdx, &savedTimerValue);
      ASSERT(ZPAL_STATUS_OK == readStatus);
      if (ZPAL_STATUS_OK == readStatus)
      {
        SSwTimer *pTimer = g_AppTimer.aTimerPointerArray[timerId];

        DPRINTF("Loaded value for timer %d: %u ms\n", timerId, savedTimerValue);

        /* We don't expect any timer to expire before power down without being restarted */
        ASSERT(savedTimerValue > elapsedMsFromSaveTimerValuesToSleep);

        /* How close is the timer to its expire time? */
        if (savedTimerValue > elapsedMsFromTimerValueSave)
        {
          durationDiffMs = savedTimerValue - elapsedMsFromTimerValueSave;
        }
        else
        {
          //Time is past expiration. Make sure the callback is called.
          durationDiffMs = 0;
        }

        DPRINTF("durationDiffMs=%u\n", durationDiffMs);

        /* If the timer timeout is within APP_TIMER_TRIGGER_DELTA_MS milliseconds
         * of the wakeup time - no matter if the wakeup was caused by an RTCC
         * timeout or e.g. pin interrupt - we simply activate its callback now.
         * (A 10 ms difference means the timeout is so close to the wake-up event
         * that the timer could run out before the task tick and scheduler is
         * started. In any case, for fast repeating timers (if any), we would
         * never get here anyway since we only enter Deep Sleep hibernate if we are
         * expected to sleep for at least 4000 ms (see enterPowerDown())
         */
        if (durationDiffMs < APP_TIMER_TRIGGER_DELTA_MS)
        {
          DPRINTF("Timer %d has expired. Activating callback.\n", timerId);

          /* Activate the callback for the expired timer (for Deep Sleep persistent timer
           * this will call the wrapper AppTimerDeepSleepCallcackWrapper that will call
           * AppTimerDeepSleepPersistentSaveAll and the actual callback)
           */
          TimerLiaisonExpiredTimerCallback(pTimer);
        }
        else
        {
          /* Reduce all saved timer values (for active timers that have not yet expired) by
           * number of milliseconds elapsed since it was last saved. Then start the timer
           * with this new value.
           */
          if ((UINT32_MAX != savedTimerValue) && (elapsedMsFromTimerValueSave < savedTimerValue))
          {
            uint32_t newTimerValue = savedTimerValue - elapsedMsFromTimerValueSave;
            DPRINTF("Setting timer %d to %u ms\n", timerId, newTimerValue);
            /* We call TimerStart() here instead of AppTimerDeepSleepPersistentStart()
             * to avoid AppTimerDeepSleepPersistentSaveAll() being called multiple
             * times. Instead we call AppTimerDeepSleepPersistentSaveAll() once for
             * all (if needed) outside the loop */
            ESwTimerStatus timerStatus = TimerStart(pTimer, newTimerValue);
            ASSERT(ESWTIMER_STATUS_SUCCESS == timerStatus);
            deepSleepTimersStarted = true;
          }
        }
      }
      valIdx++; // Only increment when we have processed an Deep Sleep persistent timer
    }
  }

  if (true == deepSleepTimersStarted)
  {
    /* One or more Deep Sleep persistent timers were started. Update the retention
     * registers.
     */
    AppTimerDeepSleepPersistentSaveAll();
  }
}

void AppTimerStopAll(void)
{
  /*Stops all timers */
  for (uint32_t i = 0; i < g_AppTimer.TimerLiaison.iTimerCount; i ++)
  {
    if (g_AppTimer.DeepSleepPersistent[i])
    {
      AppTimerDeepSleepPersistentStop(g_AppTimer.aTimerPointerArray[i]);
    }
    else
    {
      TimerStop(g_AppTimer.aTimerPointerArray[i]);
    }
  }
}
