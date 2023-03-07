
/****************************************************************************
 * components/network/wifi_hosal/port/wifi_hosal_bl808.c
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

/****************************************************************************
 * Definition
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

wifi_hosal_funcs_t g_wifi_hosal_funcs =
{
  .efuse_read_mac = hosal_wifi_ret_zero,
  .rf_turn_on = hosal_wifi_ret_zero,
  .rf_turn_off = hosal_wifi_ret_zero,
  .adc_device_get = hosal_wifi_ret_zero,
  .adc_tsen_value_get = hosal_wifi_ret_zero,
  .pm_init = bl_pm_init,
  .pm_event_register = bl_pm_event_register,
  .pm_deinit = bl_pm_deinit,
  .pm_state_run = bl_pm_state_run,
  .pm_capacity_set = bl_pm_capacity_set,
  .pm_post_event = pm_post_event,
  .pm_event_switch = bl_pm_event_switch,
};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#ifdef __cplusplus
}
#endif

