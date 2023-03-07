/***************************************************************************//**
 * @file    iot_timer_desc.h
 * @brief   Timer instance descriptor data structure.
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

#ifndef _IOT_TIMER_DESC_H_
#define _IOT_TIMER_DESC_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "sl_status.h"

#include "em_cmu.h"
#include "em_device.h"

#include "sl_sleeptimer.h"

/*******************************************************************************
 *                              DESCRIPTOR
 ******************************************************************************/

typedef struct IotTimerDescriptor {
  /**************************************/
  /*            TIMER CONFIG            */
  /**************************************/

  int32_t lInstNum;

  uint32_t default_priority;

  /**************************************/
  /*            TIMER DRIVER            */
  /**************************************/

  sl_sleeptimer_timer_handle_t sleeptimer_handle;

  uint64_t total_elapsed_ticks;
  uint64_t timeout_in_us;
  uint64_t timeout_in_ticks;

  /**************************************/
  /*             TIMER HAL              */
  /**************************************/

  bool is_open;

  void *callback;
  void *context;

  bool is_running;
  bool is_expired;
  bool is_canceled;
} IotTimerDescriptor_t;

/*******************************************************************************
 *                              PROTOTYPES
 ******************************************************************************/

IotTimerDescriptor_t *iot_timer_desc_get(int32_t lInstNum);

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_TIMER_DESC_H_ */
