/****************************************************************************
 * components/network/wifi_hosal/port/wifi_hosal_bl602.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "wifi_hosal.h"

#include "bl_pds.h"
#include "bl_efuse.h"

#ifdef BL602_MATTER_SUPPORT
#include <hosal_adc.c>
#endif

/****************************************************************************
 * Definition
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

wifi_hosal_funcs_t g_wifi_hosal_funcs =
{
  .efuse_read_mac = bl_efuse_read_mac,
  .rf_turn_on = bl_pds_rf_turnon,
  .rf_turn_off = bl_pds_rf_turnoff,
  .adc_device_get = hosal_adc_device_get,
  .adc_tsen_value_get =
  #ifdef CONF_ADC_ENABLE_TSEN
    hosal_adc_tsen_value_get,
  #else
    hosal_wifi_ret_zero,
  #endif
  .pm_init = bl_pm_init,
  .pm_event_register = bl_pm_event_register,
  .pm_deinit = bl_pm_deinit,
  .pm_state_run = bl_pm_state_run,
  .pm_capacity_set = bl_pm_capacity_set,
  .pm_post_event = pm_post_event,
  .pm_event_switch = bl_pm_event_switch,
};

#ifdef __cplusplus
}
#endif

