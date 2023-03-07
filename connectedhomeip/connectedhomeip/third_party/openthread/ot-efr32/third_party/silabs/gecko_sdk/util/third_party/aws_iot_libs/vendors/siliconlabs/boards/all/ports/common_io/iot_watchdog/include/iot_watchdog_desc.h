/***************************************************************************//**
 * @file    iot_watchdog_desc.h
 * @brief   Watchdog instance descriptor data structure.
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

#ifndef _IOT_WATCHDOG_DESC_H_
#define _IOT_WATCHDOG_DESC_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "sl_status.h"

#include "em_device.h"
#include "em_wdog.h"
#include "em_cmu.h"

#include "iot_watchdog.h"

/*******************************************************************************
 *                              DESCRIPTOR
 ******************************************************************************/

typedef struct IotWatchdogDescriptor {
  /**************************************/
  /*          WATCHDOG CONFIG           */
  /**************************************/

  int32_t                 lInstNum;       /**< instance number */

  /**************************************/
  /*          WATCHDOG DRIVER           */
  /**************************************/

  WDOG_Init_TypeDef       em_init;        /**< emlib driver's config values */
  WDOG_TypeDef*           em_instance;    /**< emlib driver's watchdog instance */
  uint8_t                 em_instance_no; /**< emlib driver's watchdog number */
  CMU_Clock_TypeDef       cmu_clock;      /**< emlib driver's cmu_clock ID*/
  IRQn_Type               irq_num;        /**< emlib driver's irq number */

  /**************************************/
  /*           WATCHDOG HAL             */
  /**************************************/

  bool                    is_open;        /**< Keep track of the open state */
  IotWatchdogStatus_t     status;         /**< common_io status */
  IotWatchdogCallback_t   callback;       /**< common_io callback */
  void*                   context;        /**< parameter passed to the callback */
  IotWatchdogBiteConfig_t bite_config;    /**< common_io bite config*/
  uint32_t                bark_time_ms;   /**< common_io bark time in milliseconds*/
  uint32_t                bite_time_ms;   /**< common_io bite time in milliseconds */

} IotWatchdogDescriptor_t;

/*******************************************************************************
 *                              PROTOTYPES
 ******************************************************************************/

IotWatchdogDescriptor_t *iot_watchdog_desc_get(int32_t lInstNum);

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_WATCHDOG_DESC_H_ */
