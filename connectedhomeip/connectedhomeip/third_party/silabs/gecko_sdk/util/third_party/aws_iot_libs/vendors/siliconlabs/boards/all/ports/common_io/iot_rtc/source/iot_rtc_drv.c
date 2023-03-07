/***************************************************************************//**
 * @file    iot_rtc_drv.c
 * @brief   RTC driver
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

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

/* SDK emlib layer */
#include "em_core.h"
#include "em_device.h"

/* SDK service layer */
#include "sl_sleeptimer.h"

/* RTC driver layer */
#include "iot_rtc_desc.h"
#include "iot_rtc_drv.h"

/*******************************************************************************
 *                             DRIVER CALLBACKS
 ******************************************************************************/

/************************* iot_rtc_drv_cb_midnight() **************************/

static void iot_rtc_drv_cb_midnight(sl_sleeptimer_timer_handle_t *handle,
                                    void *data)
{
  /* local variables */
  IotRtcHandle_t   pxRtcHandle    = NULL;
  uint32_t         ulThisYear     = 0;
  uint8_t          ucIsLeap       = 0;
  uint8_t          ucMonthDays    = 0;

  /* Source: https://en.wikipedia.org/wiki/Gregorian_calendar */
  uint8_t ppucRtcMonthDays[2u][12] = {
    /* Jan  Feb  Mar  Apr  May  Jun  Jul  Aug  Sep  Oct  Nov  Dec */
    { 31u, 28u, 31u, 30u, 31u, 30u, 31u, 31u, 30u, 31u, 30u, 31u },
    { 31u, 29u, 31u, 30u, 31u, 30u, 31u, 31u, 30u, 31u, 30u, 31u }
  };

  /* sleeptimer handle not used here */
  (void) handle;

  /* retrieve RTC descriptor */
  pxRtcHandle = (IotRtcHandle_t) data;

  /* retrieve today's year */
  ulThisYear = pxRtcHandle->usTodayYear + 1900;

  /* Are we in a leap year? */
  ucIsLeap = (((ulThisYear%  4)==0)  && /* divisible by 4 */
              ((ulThisYear%100)!=0)) || /* not divisible by 100 */
             (((ulThisYear%400)==0));   /* divisible by 400 */

  /* how many days in this months? */
  ucMonthDays = ppucRtcMonthDays[ucIsLeap][pxRtcHandle->ucTodayMonth];

  /* Last day of the month? */
  if (pxRtcHandle->ucTodayDay == ucMonthDays) {
    if (pxRtcHandle->ucTodayMonth == 11) {
      /* happy new year! */
      pxRtcHandle->usTodayYear++;
      pxRtcHandle->ucTodayMonth = 0;
      pxRtcHandle->ucTodayDay   = 1;
    } else {
      /* new month */
      pxRtcHandle->ucTodayMonth++;
      pxRtcHandle->ucTodayDay = 1;
    }
  } else {
    /* new day */
    pxRtcHandle->ucTodayDay++;
  }

  /* Increase week day counter */
  pxRtcHandle->ucTodayWday = (pxRtcHandle->ucTodayWday+1) % 7;

  /* store sleep timer tick counter for this spectacular moment */
  pxRtcHandle->ulMidnightTick = sl_sleeptimer_get_tick_count();

  /* wake up again next midnight! */
  sl_sleeptimer_start_timer_ms(
                &(pxRtcHandle->xMidnightTimer),
                24*60*60*1000,
                iot_rtc_drv_cb_midnight,
                (void *) pxRtcHandle,
                0,
                0);

  /* start alarm timer if needed */
  if (pxRtcHandle->ucAlarmIsPending == true) {
    iot_rtc_drv_alarm_start(pxRtcHandle);
  }
}

/************************* iot_rtc_drv_cb_alarm() *****************************/

static void iot_rtc_drv_cb_alarm(sl_sleeptimer_timer_handle_t *handle,
                                  void *data)
{
  /* local variables */
  IotRtcHandle_t    pxRtcHandle       = NULL;
  IotRtcCallback_t  xCallback         = NULL;
  void             *pvContext         = NULL;
  IotRtcDatetime_t  xEmptyDatetime    = {0};

  /* sleeptimer handle not used here */
  (void) handle;

  /* retrieve RTC descriptor */
  pxRtcHandle = (IotRtcHandle_t) data;

  /* retrieve callback info */
  xCallback = pxRtcHandle->pvCallback;
  pvContext = pxRtcHandle->pvContext;

  /* Update alarm information */
  pxRtcHandle->ucAlarmIsPending = false;
  pxRtcHandle->xAlarmDatetime   = xEmptyDatetime;

  /* Update RTC status */
  pxRtcHandle->xStatus = eRtcTimerAlarmTriggered;

  /* Invokate callback function if there is any */
  if (xCallback != NULL) {
    xCallback(eRtcTimerAlarmTriggered, pvContext);
  }
}

/************************* iot_rtc_drv_cb_wakeup() ****************************/

static void iot_rtc_drv_cb_wakeup(sl_sleeptimer_timer_handle_t *handle,
                                   void *data)
{
  /* local variables */
  IotRtcHandle_t    pxRtcHandle       = NULL;
  IotRtcCallback_t  xCallback         = NULL;
  void             *pvContext         = NULL;
  uint32_t          ulEmptyWakeupTime = 0;

  /* sleeptimer handle not used here */
  (void) handle;

  /* retrieve RTC descriptor */
  pxRtcHandle = (IotRtcHandle_t) data;

  /* retrieve callback info */
  xCallback = pxRtcHandle->pvCallback;
  pvContext = pxRtcHandle->pvContext;

  /* Update timer information */
  pxRtcHandle->ucWakeupIsPending = false;
  pxRtcHandle->ulWakeupTime      = ulEmptyWakeupTime;

  /* Update RTC status */
  pxRtcHandle->xStatus = eRtcTimerWakeupTriggered;

  /* Invokate callback function if there is any */
  if (xCallback != NULL) {
    xCallback(eRtcTimerWakeupTriggered, pvContext);
  }
}

/*******************************************************************************
 *                            HARDWARE CONTROL
 ******************************************************************************/

/************************* iot_rtc_drv_hw_enable() ****************************/

int32_t iot_rtc_drv_hw_enable(IotRtcHandle_t pxRtcHandle)
{
  /* local variables */
  sl_sleeptimer_timer_handle_t xEmptySleepTimer = {0};
  sl_status_t                  xStatus          = SL_STATUS_OK;

  /* initialize timers */
  if (xStatus == SL_STATUS_OK) {
    pxRtcHandle->xMidnightTimer = xEmptySleepTimer;
    pxRtcHandle->xAlarmTimer    = xEmptySleepTimer;
    pxRtcHandle->xWakeupTimer   = xEmptySleepTimer;
  }

  /* return status */
  if (xStatus == SL_STATUS_OK) {
    return IOT_RTC_SUCCESS;
  } else {
    return IOT_RTC_SET_FAILED;
  }
}

/************************* iot_rtc_drv_hw_disable() ***************************/

int32_t iot_rtc_drv_hw_disable(IotRtcHandle_t pxRtcHandle)
{
  /* local variables */
  sl_status_t xStatus = SL_STATUS_OK;

  /* stop wake up timer */
  if (xStatus == SL_STATUS_OK) {
    if (pxRtcHandle->ucWakeupIsPending == true) {
      xStatus = sl_sleeptimer_stop_timer(&(pxRtcHandle->xWakeupTimer));
    }
  }

  /* wake up timer stopped? */
  if (xStatus == SL_STATUS_OK) {
    pxRtcHandle->ucWakeupIsPending = false;
  }

  /* stop alarm timer */
  if (xStatus == SL_STATUS_OK) {
    if (pxRtcHandle->ucAlarmIsPending == true) {
      xStatus = sl_sleeptimer_stop_timer(&(pxRtcHandle->xAlarmTimer));
    }
  }

  /* alarm timer stopped? */
  if (xStatus == SL_STATUS_OK) {
    pxRtcHandle->ucAlarmIsPending = false;
  }

  /* stop midnight timer */
  if (xStatus == SL_STATUS_OK) {
    if (pxRtcHandle->xStatus != eRtcTimerStopped) {
      xStatus = sl_sleeptimer_stop_timer(&(pxRtcHandle->xMidnightTimer));
    }
  }

  /* midnight timer stopped? */
  if (xStatus == SL_STATUS_OK) {
    pxRtcHandle->xStatus = eRtcTimerStopped;
  }

  /* return status */
  if (xStatus == SL_STATUS_OK) {
    return IOT_RTC_SUCCESS;
  } else {
    return IOT_RTC_SET_FAILED;
  }
}

/*******************************************************************************
 *                             DATE/TIME CONTROL
 ******************************************************************************/

/************************* iot_rtc_drv_datetime_set() *************************/

int32_t iot_rtc_drv_datetime_set(IotRtcHandle_t pxRtcHandle,
                                 IotRtcDatetime_t *pxDatetime)
{
  /* local variables */
  uint32_t       ulElapsedSeconds   = 0;
  uint32_t       ulRemainingSeconds = 0;
  uint32_t       ulTimerTick        = 0;
  uint32_t       ulTimerFreq        = 0;
  sl_status_t    xStatus            = SL_STATUS_OK;

  /* stop midnight timer */
  if (xStatus == SL_STATUS_OK) {
    if (pxRtcHandle->xStatus != eRtcTimerStopped) {
      xStatus = sl_sleeptimer_stop_timer(&(pxRtcHandle->xMidnightTimer));
    }
  }

  /* do the math first */
  if (xStatus == SL_STATUS_OK) {
    /* store today's date */
    pxRtcHandle->usTodayYear  = pxDatetime->usYear;
    pxRtcHandle->ucTodayMonth = pxDatetime->ucMonth;
    pxRtcHandle->ucTodayDay   = pxDatetime->ucDay;
    pxRtcHandle->ucTodayWday  = pxDatetime->ucWday;

    /* How many seconds since midnight? */
    ulElapsedSeconds = (pxDatetime->ucHour)*60*60 +
                       (pxDatetime->ucMinute)*60  +
                       (pxDatetime->ucSecond);

    /* How many seconds are remaining? */
    ulRemainingSeconds = 24*60*60 - ulElapsedSeconds;

    /* retrieve sleep timer tick counter and frequency */
    ulTimerTick = sl_sleeptimer_get_tick_count();
    ulTimerFreq = sl_sleeptimer_get_timer_frequency();

    /* what was the tick for last midnight? */
    pxRtcHandle->ulMidnightTick = ulTimerTick-ulElapsedSeconds*ulTimerFreq;
  }

  /* plan for next midnight */
  if (xStatus == SL_STATUS_OK) {
    xStatus = sl_sleeptimer_start_timer_ms(
                &(pxRtcHandle->xMidnightTimer),
                ulRemainingSeconds*1000,
                iot_rtc_drv_cb_midnight,
                (void *) pxRtcHandle,
                0,
                0);
  }

  /* if everything is OK, update the status of RTC */
  if (xStatus == SL_STATUS_OK) {
    pxRtcHandle->xStatus = eRtcTimerRunning;
  }

  /* return status */
  if (xStatus == SL_STATUS_OK) {
    return IOT_RTC_SUCCESS;
  } else {
    return IOT_RTC_SET_FAILED;
  }
}

/************************* iot_rtc_drv_datetime_get() *************************/

int32_t iot_rtc_drv_datetime_get(IotRtcHandle_t pxRtcHandle,
                                 IotRtcDatetime_t * pxDatetime)
{
  /* local variables */
  uint32_t       ulElapsedSeconds   = 0;
  uint32_t       ulTimerTick        = 0;
  uint32_t       ulTimerFreq        = 0;
  sl_status_t    xStatus            = SL_STATUS_OK;

  if (xStatus == SL_STATUS_OK) {
    /* retrieve sleep timer tick counter and frequency */
    ulTimerTick = sl_sleeptimer_get_tick_count();
    ulTimerFreq = sl_sleeptimer_get_timer_frequency();

    /* How many seconds since midnight? */
    ulElapsedSeconds = (ulTimerTick-pxRtcHandle->ulMidnightTick)/ulTimerFreq;

    /* compute HH:MM:SS */
    pxDatetime->ucHour    = (ulElapsedSeconds%(24*60*60))/(60*60);
    pxDatetime->ucMinute  = (ulElapsedSeconds%(60*60   ))/(60);
    pxDatetime->ucSecond  = (ulElapsedSeconds%(60      ));

    /* retrieve YYYY:MM:DD */
    pxDatetime->usYear    = pxRtcHandle->usTodayYear;
    pxDatetime->ucMonth   = pxRtcHandle->ucTodayMonth;
    pxDatetime->ucDay     = pxRtcHandle->ucTodayDay;
    pxDatetime->ucWday    = pxRtcHandle->ucTodayWday;
  }

  /* return status */
  if (xStatus == SL_STATUS_OK) {
    return IOT_RTC_SUCCESS;
  } else {
    return IOT_RTC_GET_FAILED;
  }
}

/*******************************************************************************
 *                           ALARM TIMER CONTROL
 ******************************************************************************/

/************************ iot_rtc_drv_alarm_start() ***************************/

int32_t iot_rtc_drv_alarm_start(IotRtcHandle_t pxRtcHandle)
{
  /* local variables */
  IotRtcDatetime_t *pxAlarmDatetime = NULL;
  uint32_t          ulTimerTick        = 0;
  uint32_t          ulTimerFreq        = 0;
  uint32_t          ulAlarmSeconds     = 0;
  uint32_t          ulAlarmTick        = 0;
  uint32_t          ulTicksToSleep     = 0;
  sl_status_t       xStatus            = SL_STATUS_OK;

  /* start sleeptimer if the alarm is scheduled for today */
  if (xStatus == SL_STATUS_OK) {
    /* mark alarm timer as pending (before any callback is called) */
    pxRtcHandle->ucAlarmIsPending = true;

    /* first we retrieve alarm date/time information */
    pxAlarmDatetime = &(pxRtcHandle->xAlarmDatetime);

    /* check if the alarm is scheduled for today */
    if (pxAlarmDatetime->usYear ==pxRtcHandle->usTodayYear &&
        pxAlarmDatetime->ucMonth==pxRtcHandle->ucTodayMonth &&
        pxAlarmDatetime->ucDay  ==pxRtcHandle->ucTodayDay) {
      /* retrieve sleep timer tick counter and frequency */
      ulTimerTick = sl_sleeptimer_get_tick_count();
      ulTimerFreq = sl_sleeptimer_get_timer_frequency();

      /* when shall the alarm fire? (from midnight) */
      ulAlarmSeconds = pxAlarmDatetime->ucHour*60*60 +
                       pxAlarmDatetime->ucMinute*60  +
                       pxAlarmDatetime->ucSecond;
      ulAlarmTick = pxRtcHandle->ulMidnightTick+ulAlarmSeconds*ulTimerFreq;

      /* how many ticks to sleep for? */
      if (ulAlarmTick > ulTimerTick) {
        ulTicksToSleep = ulAlarmTick - ulTimerTick;
      } else {
        ulTicksToSleep = 0;
      }

      /* start sleeptimer */
      xStatus = sl_sleeptimer_start_timer(
                  &(pxRtcHandle->xAlarmTimer),
                  ulTicksToSleep,
                  iot_rtc_drv_cb_alarm,
                  (void *) pxRtcHandle,
                  0,
                  0);

      /* fall back if an error happened */
      if (xStatus != SL_STATUS_OK) {
        pxRtcHandle->ucAlarmIsPending = false;
      }
    }
  }

  /* return status */
  if (xStatus == SL_STATUS_OK) {
    return IOT_RTC_SUCCESS;
  } else {
    return IOT_RTC_GET_FAILED;
  }
}

/************************ iot_rtc_drv_alarm_stop() ****************************/

int32_t iot_rtc_drv_alarm_stop(IotRtcHandle_t pxRtcHandle)
{
  /* local variables */
  sl_status_t xStatus = SL_STATUS_OK;

  /* stop sleep timer */
  if (xStatus == SL_STATUS_OK) {
    xStatus = sl_sleeptimer_stop_timer(&(pxRtcHandle->xAlarmTimer));
  }

  /* alarm has stopped successfully */
  if (xStatus == SL_STATUS_OK) {
    pxRtcHandle->ucAlarmIsPending = false;
  }

  /* return status */
  if (xStatus == SL_STATUS_OK) {
    return IOT_RTC_SUCCESS;
  } else {
    return IOT_RTC_GET_FAILED;
  }
}

/*******************************************************************************
 *                           WAKEUP TIMER CONTROL
 ******************************************************************************/

/************************ iot_rtc_drv_wakeup_start() **************************/

int32_t iot_rtc_drv_wakeup_start(IotRtcHandle_t pxRtcHandle)
{
  /* local variables */
  sl_status_t xStatus = SL_STATUS_OK;

  /* start a sleeptimer for wakeup period */
  if (xStatus == SL_STATUS_OK) {
    /* mark wakeup timer as pending (before any callback is called) */
    pxRtcHandle->ucWakeupIsPending = true;

    /* start sleep timer */
    xStatus = sl_sleeptimer_start_timer_ms(
                &(pxRtcHandle->xWakeupTimer),
                pxRtcHandle->ulWakeupTime,
                iot_rtc_drv_cb_wakeup,
                (void *) pxRtcHandle,
                0,
                0);

    /* fall back if any error has happened */
    if (xStatus != SL_STATUS_OK) {
      pxRtcHandle->ucWakeupIsPending = false;
    }
  }

  /* return status */
  if (xStatus == SL_STATUS_OK) {
    return IOT_RTC_SUCCESS;
  } else {
    return IOT_RTC_GET_FAILED;
  }
}

/************************** iot_rtc_drv_wakeup_stop() *************************/

int32_t iot_rtc_drv_wakeup_stop(IotRtcHandle_t pxRtcHandle)
{
  /* local variables */
  sl_status_t xStatus           = SL_STATUS_OK;
  uint32_t    ulEmptyWakeupTime = 0;

  /* stop sleep timer */
  if (xStatus == SL_STATUS_OK) {
    xStatus = sl_sleeptimer_stop_timer(&(pxRtcHandle->xWakeupTimer));
  }

  /* wakeup timer has stopped successfully */
  if (xStatus == SL_STATUS_OK) {
    pxRtcHandle->ucWakeupIsPending = false;
    pxRtcHandle->ulWakeupTime      = ulEmptyWakeupTime;
  }

  /* return status */
  if (xStatus == SL_STATUS_OK) {
    return IOT_RTC_SUCCESS;
  } else {
    return IOT_RTC_GET_FAILED;
  }
}
