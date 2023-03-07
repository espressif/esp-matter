/***************************************************************************//**
 * @file    iot_rtc_hal.c
 * @brief   Silicon Labs implementation of Common I/O RTC API.
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

/* FreeRTOS kernel layer */
#include "FreeRTOS.h"

/* RTC driver layer */
#include "iot_rtc_desc.h"
#include "iot_rtc_drv.h"

/* RTC interface layer */
#include "iot_rtc.h"

/*******************************************************************************
 *                                MACROS
 ******************************************************************************/

#define DATE_TIME_IS_VALID(pxDatetime) (                                \
           pxDatetime->ucMonth  <  12                                && \
           pxDatetime->ucDay    >  0                                 && \
           pxDatetime->ucDay    <= 31                                && \
           pxDatetime->ucWday   <  7                                 && \
           pxDatetime->ucHour   <  24                                && \
           pxDatetime->ucMinute <  60                                && \
           pxDatetime->ucSecond <  60                                   \
        )

#define DATETIME_IS_LESS_THAN(pxDatetime1, pxDatetime2) (               \
           ((pxDatetime1)->usYear   <  (pxDatetime2)->usYear  )         \
           ||                                                           \
           ((pxDatetime1)->usYear   == (pxDatetime2)->usYear         && \
            (pxDatetime1)->ucMonth  <  (pxDatetime2)->ucMonth )         \
           ||                                                           \
           ((pxDatetime1)->usYear   == (pxDatetime2)->usYear         && \
            (pxDatetime1)->ucMonth  == (pxDatetime2)->ucMonth        && \
            (pxDatetime1)->ucDay    <  (pxDatetime2)->ucDay   )         \
           ||                                                           \
           ((pxDatetime1)->usYear   == (pxDatetime2)->usYear         && \
            (pxDatetime1)->ucMonth  == (pxDatetime2)->ucMonth        && \
            (pxDatetime1)->ucDay    == (pxDatetime2)->ucDay          && \
            (pxDatetime1)->ucHour   <  (pxDatetime2)->ucHour  )         \
           ||                                                           \
           ((pxDatetime1)->usYear   == (pxDatetime2)->usYear         && \
            (pxDatetime1)->ucMonth  == (pxDatetime2)->ucMonth        && \
            (pxDatetime1)->ucDay    == (pxDatetime2)->ucDay          && \
            (pxDatetime1)->ucHour   == (pxDatetime2)->ucHour         && \
            (pxDatetime1)->ucMinute <  (pxDatetime2)->ucMinute)         \
           ||                                                           \
           ((pxDatetime1)->usYear   == (pxDatetime2)->usYear         && \
            (pxDatetime1)->ucMonth  == (pxDatetime2)->ucMonth        && \
            (pxDatetime1)->ucDay    == (pxDatetime2)->ucDay          && \
            (pxDatetime1)->ucHour   == (pxDatetime2)->ucHour         && \
            (pxDatetime1)->ucMinute == (pxDatetime2)->ucMinute       && \
            (pxDatetime1)->ucSecond <  (pxDatetime2)->ucSecond)         \
        )

/*******************************************************************************
 *                             iot_rtc_open()
 ******************************************************************************/

/**
 * @brief   iot_rtc_open is used to initialize the RTC timer.
 *          It usually resets the RTC timer, sets up the clock for RTC etc...
 *
 * @param[in]   lRtcInstance   The instance of the RTC timer to initialize.
 *
 * @return
 *   - the handle IotRtcHandle_t on success
 *   - NULL if
 *      - if instance is already open
 *      - invalid instance
 */
IotRtcHandle_t iot_rtc_open(int32_t lRtcInstance)
{
  /* local variables */
  int32_t            lStatus           = IOT_RTC_SUCCESS;
  IotRtcDatetime_t   xEmptyDatetime    = {0};
  uint32_t           ulEmptyWakeupTime = 0;
  IotRtcHandle_t     xRtcHandle        = NULL;

  /* retrieve RTC descriptor by instance number */
  xRtcHandle = iot_rtc_desc_get(lRtcInstance);

  /* validate xRtcHandle */
  if (xRtcHandle == NULL) {
    return NULL;
  }

  portENTER_CRITICAL();

  /* selected RTC shouldn't be open */
  if (xRtcHandle->ucIsOpen == pdTRUE) {
    portEXIT_CRITICAL();
    return NULL;
  }

  /* initialize h/w-related data */
  lStatus = iot_rtc_drv_hw_enable(xRtcHandle);

  if (lStatus == IOT_RTC_SUCCESS) {
    /* set xRtcHandle and initialize it */
    /* clear up non H/W-specific parts of the descriptor */
    xRtcHandle->xStatus            = eRtcTimerStopped;
    xRtcHandle->pvCallback         = NULL;
    xRtcHandle->pvContext          = NULL;
    xRtcHandle->ucAlarmIsPending   = pdFALSE;
    xRtcHandle->xAlarmDatetime     = xEmptyDatetime;
    xRtcHandle->ucWakeupIsPending  = pdFALSE;
    xRtcHandle->ulWakeupTime       = ulEmptyWakeupTime;
    /* finally mark RTC as open */
    xRtcHandle->ucIsOpen           = pdTRUE;
  } else {
    xRtcHandle = NULL;
  }

  portEXIT_CRITICAL();

  /* done */
  return xRtcHandle;
}

/*******************************************************************************
 *                           iot_rtc_set_callback()
 ******************************************************************************/

/*!
 * @brief   iot_rtc_set_callback is used to set the callback to be called when alarmTime triggers.
 *          The caller must set the Alarm time using IOCTL to get the callback.
 *
 * @note Single callback is used for both rtc_alarm, and rtc_wakeup features.
 * @note Newly set callback overrides the one previously set
 * @note This callback is per handle. Each instance has its own callback.
 *
 * @param[in]   pxRtcHandle  handle to RTC driver returned in
 *                          iot_rtc_open()
 * @param[in]   xCallback   callback function to be called.
 * @param[in]   pvUserContext   user context to be passed when callback is called.
 *
 */
void iot_rtc_set_callback(IotRtcHandle_t pxRtcHandle,
                          IotRtcCallback_t xCallback,
                          void * pvUserContext)
{
  /* pxRtcHandle can't be null */
  if (pxRtcHandle == NULL) {
    return;
  }

  /* enter critical section */
  portENTER_CRITICAL();

  /* pxRtcHandle must be already open */
  if (pxRtcHandle->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return;
  }

  /* update callback info */
  /* set callback pointers */
  pxRtcHandle->pvCallback = xCallback;
  pxRtcHandle->pvContext = pvUserContext;
  /* exit critical section */
  portEXIT_CRITICAL();

  /* done */
  return;
}

/*******************************************************************************
 *                             iot_rtc_ioctl()
 ******************************************************************************/

/**
 * @brief   iot_rtc_ioctl is used to set RTC configuration and
 *          RTC properties like Wakeup time, alarms etc.
 *          Supported IOCTL requests are defined in iot_RtcIoctlRequest_t
 *
 * @param[in]   pxRtcHandle  handle to RTC driver returned in
 *                          iot_rtc_open()
 * @param[in]   xRequest    configuration request of type IotRtcIoctlRequest_t
 * @param[in,out] pvBuffer  buffer holding RTC set and get values.
 *
 * @return
 *   - IOT_RTC_SUCCESS on success
 *   - IOT_RTC_INVALID_VALUE if
 *      - pxRtcHandle == NULL
 *      - xRequest is invalid
 *      - pvBuffer == NULL (excluding eCancelRtcAlarm, eCancelRtcWakeup)
 *      - if date/time is set in the past for eSetRtcAlarm
 *   - IOT_RTC_SET_FAILED if date/time is invalid for eSetRtcAlarm.
 *   - IOT_RTC_NOT_STARTED on error
 *   - IOT_RTC_FUNCTION_NOT_SUPPORTED if feature not supported
 *      - Only valid for eCancelRtcAlarm, eCancelRtcWakeup
 */
int32_t iot_rtc_ioctl(IotRtcHandle_t pxRtcHandle,
                      IotRtcIoctlRequest_t xRequest,
                      void * const pvBuffer)
{
  /* local variables */
  int32_t           lStatus           = IOT_RTC_SUCCESS;
  IotRtcDatetime_t *pxAlarmDatetime   = NULL;
  uint32_t         *pulWakeupTime     = NULL;
  IotRtcStatus_t   *pxTimerStatus     = NULL;
  IotRtcDatetime_t  xCurrentDatetime  = {0};
  IotRtcDatetime_t  xEmptyDatetime    = {0};
  uint32_t          ulEmptyWakeupTime = 0;

  /* pxRtcHandle can't be null */
  if (pxRtcHandle == NULL) {
    return IOT_RTC_INVALID_VALUE;
  }

  /* pxRtcHandle must be already open */
  if (pxRtcHandle->ucIsOpen == pdFALSE) {
    return IOT_RTC_INVALID_VALUE;
  }

  /* process the request */
  if (lStatus == IOT_RTC_SUCCESS) {
    switch(xRequest) {
      /* ----------- */
      /* start alarm */
      /* ----------- */
      case eSetRtcAlarm:
        /* buffer can't be null */
        if (lStatus == IOT_RTC_SUCCESS) {
          if (pvBuffer == NULL) {
            lStatus = IOT_RTC_INVALID_VALUE;
          }
        }
        /* pvBuffer is IotRtcDatetime_t */
        if (lStatus == IOT_RTC_SUCCESS) {
          pxAlarmDatetime = (IotRtcDatetime_t *) pvBuffer;
        }
        /* enter critical section */
        portENTER_CRITICAL();
        /* RTC must be running */
        if (lStatus == IOT_RTC_SUCCESS) {
          if (pxRtcHandle->xStatus == eRtcTimerStopped) {
            lStatus = IOT_RTC_NOT_STARTED;
          }
        }
        /* pxAlarmDatetime must be valid */
        if (lStatus == IOT_RTC_SUCCESS) {
          if (DATE_TIME_IS_VALID(pxAlarmDatetime) == 0) {
            lStatus = IOT_RTC_SET_FAILED;
          }
        }
        /* retrieve current date time */
        if (lStatus == IOT_RTC_SUCCESS) {
           lStatus = iot_rtc_drv_datetime_get(pxRtcHandle, &xCurrentDatetime);
        }
        /* pxAlarmDatetime can't be in the past */
        if (lStatus == IOT_RTC_SUCCESS) {
          if (DATETIME_IS_LESS_THAN(pxAlarmDatetime, &xCurrentDatetime)) {
            lStatus = IOT_RTC_INVALID_VALUE;
          }
        }
        /* another alarm already started? */
        if (lStatus == IOT_RTC_SUCCESS) {
          if (pxRtcHandle->ucAlarmIsPending == pdTRUE) {
            lStatus = IOT_RTC_NOT_STARTED;
          }
        }
        /* if everything is good, start the alarm */
        if (lStatus == IOT_RTC_SUCCESS) {
          pxRtcHandle->xAlarmDatetime = *pxAlarmDatetime;
          lStatus = iot_rtc_drv_alarm_start(pxRtcHandle);
          if (lStatus != IOT_RTC_SUCCESS) {
            pxRtcHandle->xAlarmDatetime = xEmptyDatetime;
          }
        }
        /* exit critical section */
        portEXIT_CRITICAL();
        /* done */
        break;

      /* -------------- */
      /* get alarm data */
      /* -------------- */
      case eGetRtcAlarm:
        /* buffer can't be null */
        if (lStatus == IOT_RTC_SUCCESS) {
          if (pvBuffer == NULL) {
            lStatus = IOT_RTC_INVALID_VALUE;
          }
        }
        /* pvBuffer is IotRtcDatetime_t */
        if (lStatus == IOT_RTC_SUCCESS) {
          pxAlarmDatetime = (IotRtcDatetime_t *) pvBuffer;
        }
        /* enter critical section */
        portENTER_CRITICAL();
        /* RTC must be running */
        if (lStatus == IOT_RTC_SUCCESS) {
          if (pxRtcHandle->xStatus == eRtcTimerStopped) {
            lStatus = IOT_RTC_NOT_STARTED;
          }
        }
        /* alarm should have already been started */
        if (lStatus == IOT_RTC_SUCCESS) {
          if (pxRtcHandle->ucAlarmIsPending == pdFALSE) {
            lStatus = IOT_RTC_NOT_STARTED;
          }
        }
        /* if everything is good, retrieve alarm info */
        if (lStatus == IOT_RTC_SUCCESS) {
          *pxAlarmDatetime = pxRtcHandle->xAlarmDatetime;
        }
        /* exit critical section */
        portEXIT_CRITICAL();
        /* done */
        break;

      /* ------------ */
      /* cancel alarm */
      /* ------------ */
      case eCancelRtcAlarm:
        /* enter critical section */
        portENTER_CRITICAL();
        /* RTC must be running */
        if (lStatus == IOT_RTC_SUCCESS) {
          if (pxRtcHandle->xStatus == eRtcTimerStopped) {
            lStatus = IOT_RTC_NOT_STARTED;
          }
        }
        /* alarm should have already been started */
        if (lStatus == IOT_RTC_SUCCESS) {
          if (pxRtcHandle->ucAlarmIsPending == pdFALSE) {
            lStatus = IOT_RTC_NOT_STARTED;
          }
        }
        /* if everything is good, cancel the alarm */
        if (lStatus == IOT_RTC_SUCCESS) {
          lStatus = iot_rtc_drv_alarm_stop(pxRtcHandle);
          if (lStatus == IOT_RTC_SUCCESS) {
            pxRtcHandle->xAlarmDatetime = xEmptyDatetime;
          }
        }
        /* exit critical section */
        portEXIT_CRITICAL();
        /* done */
        break;

      /* ------------------ */
      /* start wakeup timer */
      /* ------------------ */
      case eSetRtcWakeupTime:
        /* buffer can't be null */
        if (lStatus == IOT_RTC_SUCCESS) {
          if (pvBuffer == NULL) {
            lStatus = IOT_RTC_INVALID_VALUE;
          }
        }
        /* pvBuffer is uint32_t */
        if (lStatus == IOT_RTC_SUCCESS) {
          pulWakeupTime = (uint32_t *) pvBuffer;
        }
        /* pulWakeupTime must be valid */
        if (lStatus == IOT_RTC_SUCCESS) {
          if (*pulWakeupTime == 0) {
            lStatus = IOT_RTC_INVALID_VALUE;
          }
        }
        /* enter critical section */
        portENTER_CRITICAL();
        /* RTC must be running */
        if (lStatus == IOT_RTC_SUCCESS) {
          if (pxRtcHandle->xStatus == eRtcTimerStopped) {
            lStatus = IOT_RTC_NOT_STARTED;
          }
        }
        /* another timer already started? */
        if (lStatus == IOT_RTC_SUCCESS) {
          if (pxRtcHandle->ucWakeupIsPending == pdTRUE) {
            lStatus = IOT_RTC_NOT_STARTED;
          }
        }
        /* if everything is good, start the wakeup timer */
        if (lStatus == IOT_RTC_SUCCESS) {
          pxRtcHandle->ulWakeupTime = *pulWakeupTime;
          lStatus = iot_rtc_drv_wakeup_start(pxRtcHandle);
          if (lStatus != IOT_RTC_SUCCESS) {
            pxRtcHandle->ulWakeupTime = ulEmptyWakeupTime;
          }
        }
        /* exit critical section */
        portEXIT_CRITICAL();
        /* done */
        break;

      /* --------------------- */
      /* get wakeup timer data */
      /* --------------------- */
      case eGetRtcWakeupTime:
        /* buffer can't be null */
        if (lStatus == IOT_RTC_SUCCESS) {
          if (pvBuffer == NULL) {
            lStatus = IOT_RTC_INVALID_VALUE;
          }
        }
        /* pvBuffer is uint32_t */
        if (lStatus == IOT_RTC_SUCCESS) {
          pulWakeupTime = (uint32_t *) pvBuffer;
        }
        /* enter critical section */
        portENTER_CRITICAL();
        /* RTC must be running */
        if (lStatus == IOT_RTC_SUCCESS) {
          if (pxRtcHandle->xStatus == eRtcTimerStopped) {
            lStatus = IOT_RTC_NOT_STARTED;
          }
        }
        /* timer should have already been started */
        if (lStatus == IOT_RTC_SUCCESS) {
          if (pxRtcHandle->ucWakeupIsPending == pdFALSE) {
            lStatus = IOT_RTC_NOT_STARTED;
          }
        }
        /* if everything is good, retrieve timer info */
        if (lStatus == IOT_RTC_SUCCESS) {
          *pulWakeupTime = pxRtcHandle->ulWakeupTime;
        }
        /* exit critical section */
        portEXIT_CRITICAL();
        /* done */
        break;

      /* ------------------- */
      /* cancel wakeup timer */
      /* ------------------- */
      case eCancelRtcWakeup:
        /* enter critical section */
        portENTER_CRITICAL();
        /* RTC must be running */
        if (lStatus == IOT_RTC_SUCCESS) {
          if (pxRtcHandle->xStatus == eRtcTimerStopped) {
            lStatus = IOT_RTC_NOT_STARTED;
          }
        }
        /* timer should have already been started */
        if (lStatus == IOT_RTC_SUCCESS) {
          if (pxRtcHandle->ucWakeupIsPending == pdFALSE) {
            lStatus = IOT_RTC_NOT_STARTED;
          }
        }
        /* if everything is good, cancel the alarm */
        if (lStatus == IOT_RTC_SUCCESS) {
          lStatus = iot_rtc_drv_wakeup_stop(pxRtcHandle);
          if (lStatus == IOT_RTC_SUCCESS) {
            pxRtcHandle->ulWakeupTime = ulEmptyWakeupTime;
          }
        }
        /* exit critical section */
        portEXIT_CRITICAL();
        /* done */
        break;

      /* -------------- */
      /* get RTC status */
      /* -------------- */
      case eGetRtcStatus:
        /* buffer can't be null */
        if (lStatus == IOT_RTC_SUCCESS) {
          if (pvBuffer == NULL) {
            lStatus = IOT_RTC_INVALID_VALUE;
          }
        }
        /* pvBuffer is IotRtcStatus_t */
        if (lStatus == IOT_RTC_SUCCESS) {
          pxTimerStatus = (IotRtcStatus_t *) pvBuffer;
        }
        /* enter critical section */
        portENTER_CRITICAL();
        /* if everything is good, return the status of RTC */
        if (lStatus == IOT_RTC_SUCCESS) {
          *pxTimerStatus = pxRtcHandle->xStatus;
        }
        /* enter critical section */
        portEXIT_CRITICAL();
        /* done */
        break;

      /* invalid request */
      default:
        /* set error */
        lStatus = IOT_RTC_INVALID_VALUE;
        /* done */
        break;
    }
  }

  /* done */
  return lStatus;
}

/*******************************************************************************
 *                            iot_rtc_set_datetime()
 ******************************************************************************/

/**
 * @brief   iot_rtc_set_date_time is used to set the current time as a reference in RTC timer counter.
 *
 * @param[in]   pxRtcHandle  handle to RTC driver returned in
 *                          iot_rtc_open()
 * @param[in]   pxDatetime  pointer to IotRtcDatetime_t structure to set the date&time
 *                          to be set in RTC counter.
 *
 * @return
 *   - IOT_RTC_SUCCESS on success
 *   - IOT_RTC_INVALID_VALUE if pxRtcHandle == NULL or pxDatetime == NULL
 *   - IOT_RTC_SET_FAILED on error.
 */
int32_t iot_rtc_set_datetime(IotRtcHandle_t pxRtcHandle,
                             const IotRtcDatetime_t * pxDatetime)
{
  int32_t lStatus;

  /* pxRtcHandle can't be null */
  if (pxRtcHandle == NULL) {
    return IOT_RTC_INVALID_VALUE;
  }

  /* pxDatetime can't be null */
  if (pxDatetime == NULL) {
    return IOT_RTC_INVALID_VALUE;
  }

  /* pxDatetime must be valid */
  if (DATE_TIME_IS_VALID(pxDatetime) == 0) {
    return IOT_RTC_SET_FAILED;
  }

  portENTER_CRITICAL();

  /* pxRtcHandle must be already open */
  if (pxRtcHandle->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return IOT_RTC_INVALID_VALUE;
  }

  /* set datetime on the RTC */
  lStatus = iot_rtc_drv_datetime_set(pxRtcHandle,
                                     (IotRtcDatetime_t *) pxDatetime);

  portEXIT_CRITICAL();

  /* done */
  return lStatus;
}

/*******************************************************************************
 *                            iot_rtc_get_datetime()
 ******************************************************************************/

/**
 * @brief   iot_rtc_get_datetime is used to get the current time from the RTC counter.
 *          The time must be set first as a reference to get the time.
 *
 * @param[in]   pxRtcHandle  handle to RTC driver returned in
 *                          iot_rtc_open()
 * @param[in]   pxDatetime  pointer to IotRtcDatetime_t structure to get the date&time
 *                          from RTC counter.
 *
 * @return
 *   - IOT_RTC_SUCCESS on success
 *   - IOT_RTC_INVALID_VALUE if pxRtcHandle == NULL or pxDatetime == NULL
 *   - IOT_RTC_NOT_STARTED on error
 */
int32_t iot_rtc_get_datetime(IotRtcHandle_t pxRtcHandle,
                             IotRtcDatetime_t *pxDatetime)
{
  /* local variables */
  int32_t lStatus = IOT_RTC_SUCCESS;

  /* pxRtcHandle can't be null */
  if (pxRtcHandle == NULL) {
    return IOT_RTC_INVALID_VALUE;
  }

  /* pxDatetime can't be null */
  if (pxDatetime == NULL) {
    return IOT_RTC_INVALID_VALUE;
  }

  /* enter critical section */
  portENTER_CRITICAL();

  /* pxRtcHandle must be already open */
  if (pxRtcHandle->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return IOT_RTC_INVALID_VALUE;
  }

  /* pxRtcHandle RTC must be running */
  if (pxRtcHandle->xStatus == eRtcTimerStopped) {
    portEXIT_CRITICAL();
    return IOT_RTC_NOT_STARTED;
  }

  /* get datetime from the RTC */
  lStatus = iot_rtc_drv_datetime_get(pxRtcHandle, pxDatetime);

  /* exit critical section */
  portEXIT_CRITICAL();

  /* done */
  return lStatus;
}

/*******************************************************************************
 *                              iot_rtc_close()
 ******************************************************************************/

/**
 * @brief   iot_rtc_close is used to de-Initialize RTC Timer.
 *          it resets the RTC timer and may stop the timer.
 *
 * @param[in]   pxRtcHandle  handle to RTC interface.
 *
 * @return
 *   - IOT_RTC_SUCCESS on success
 *   - IOT_RTC_INVALID_VALUE if
 *      - pxRtcHandle == NULL
 *      - not in open state (already closed).
 */
int32_t iot_rtc_close(IotRtcHandle_t pxRtcHandle)
{
  /* local variables */
  int32_t            lStatus           = IOT_RTC_SUCCESS;
  IotRtcDatetime_t   xEmptyDatetime    = {0};
  uint32_t           ulEmptyWakeupTime = 0;

  /* pxRtcHandle can't be null */
  if (pxRtcHandle == NULL) {
    return IOT_RTC_INVALID_VALUE;
  }

  /* enter critical section */
  portENTER_CRITICAL();

  /* pxRtcHandle must be already open */
  if (pxRtcHandle->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return IOT_RTC_INVALID_VALUE;
  }

  /* cancel alarm if pending */
  if (lStatus == IOT_RTC_SUCCESS) {
    if (pxRtcHandle->ucAlarmIsPending == pdTRUE) {
      lStatus = iot_rtc_drv_alarm_stop(pxRtcHandle);
    }
  }

  /* cancel wakeup timer if pending */
  if (lStatus == IOT_RTC_SUCCESS) {
    if (pxRtcHandle->ucWakeupIsPending == pdTRUE) {
      lStatus = iot_rtc_drv_wakeup_stop(pxRtcHandle);
    }
  }

  /* de-initialize h/w-related data */
  if (lStatus == IOT_RTC_SUCCESS) {
    lStatus = iot_rtc_drv_hw_disable(pxRtcHandle);
  }

  if (lStatus == IOT_RTC_SUCCESS) {
    /* clear up the descriptor */
    pxRtcHandle->xStatus            = eRtcTimerStopped;
    pxRtcHandle->pvCallback         = NULL;
    pxRtcHandle->pvContext          = NULL;
    pxRtcHandle->ucAlarmIsPending   = pdFALSE;
    pxRtcHandle->xAlarmDatetime     = xEmptyDatetime;
    pxRtcHandle->ucWakeupIsPending  = pdFALSE;
    pxRtcHandle->ulWakeupTime       = ulEmptyWakeupTime;

    /* mark as closed */
    pxRtcHandle->ucIsOpen = pdFALSE;
  }

  /* exit critical section */
  portEXIT_CRITICAL();

  /* done */
  return lStatus;
}
