/***************************************************************************//**
 * @file    iot_rtc_drv.h
 * @brief   RTC driver header file
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

#ifndef _IOT_RTC_DRV_H_
#define _IOT_RTC_DRV_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "sl_status.h"

#include "iot_rtc.h"

/*******************************************************************************
 *                         MIDDLE-LAYER PROTOTYPES
 ******************************************************************************/

/* RTC enable/disable */
int32_t iot_rtc_drv_hw_enable(IotRtcHandle_t pxRtcHandle);
int32_t iot_rtc_drv_hw_disable(IotRtcHandle_t pxRtcHandle);

/* RTC date/time set/get */
int32_t iot_rtc_drv_datetime_set(IotRtcHandle_t pxRtcHandle,
                                 IotRtcDatetime_t *pxDatetime);
int32_t iot_rtc_drv_datetime_get(IotRtcHandle_t pxRtcHandle,
                                 IotRtcDatetime_t *pxDatetime);

/* RTC alarm timer start/stop */
int32_t iot_rtc_drv_alarm_start(IotRtcHandle_t pxRtcHandle);
int32_t iot_rtc_drv_alarm_stop(IotRtcHandle_t pxRtcHandle);

/* RTC wakeup timer start/stop */
int32_t iot_rtc_drv_wakeup_start(IotRtcHandle_t pxRtcHandle);
int32_t iot_rtc_drv_wakeup_stop(IotRtcHandle_t pxRtcHandle);

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_RTC_DRV_H_ */
