/***************************************************************************//**
 * @file    iot_rtc_desc.h
 * @brief   RTC instance descriptor data structure.
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
 *                              SAFE GUARD
 ******************************************************************************/

#ifndef _IOT_RTC_DESC_H_
#define _IOT_RTC_DESC_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "sl_status.h"

#include "em_device.h"

#include "sl_sleeptimer.h"

#include "iot_rtc.h"

/*******************************************************************************
 *                              DESCRIPTOR
 ******************************************************************************/

typedef struct IotRtcDescriptor {
  /**************************************/
  /*            RTC CONFIG              */
  /**************************************/

  /* instance number */
  int32_t                       lInstNum;

  /**************************************/
  /*            RTC DRIVER              */
  /**************************************/

  /* sleeptimers */
  sl_sleeptimer_timer_handle_t  xMidnightTimer;
  sl_sleeptimer_timer_handle_t  xAlarmTimer;
  sl_sleeptimer_timer_handle_t  xWakeupTimer;

  /* sleep timer needs to remember todays date */
  uint16_t                      usTodayYear;
  uint8_t                       ucTodayMonth;
  uint8_t                       ucTodayDay;
  uint8_t                       ucTodayWday;

  /* the tick when the next day will start */
  uint32_t                      ulMidnightTick;

  /**************************************/
  /*             RTC HAL                */
  /**************************************/

  /* open flag */
  uint8_t                       ucIsOpen;

  /* RTC status */
  IotRtcStatus_t                xStatus;

  /* callback info */
  void                         *pvCallback;
  void                         *pvContext;

  /* alarm information */
  uint8_t                       ucAlarmIsPending;
  IotRtcDatetime_t              xAlarmDatetime;

  /* wakeup information */
  uint8_t                       ucWakeupIsPending;
  uint32_t                      ulWakeupTime;

} IotRtcDescriptor_t;

/*******************************************************************************
 *                              PROTOTYPES
 ******************************************************************************/

IotRtcDescriptor_t *iot_rtc_desc_get(int32_t lInstNum);

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_RTC_DESC_H_ */
