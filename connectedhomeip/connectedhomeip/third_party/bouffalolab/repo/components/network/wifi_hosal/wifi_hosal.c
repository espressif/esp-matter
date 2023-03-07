/****************************************************************************
 * components/network/wifi_hosal/wifi_hosal.c
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
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: wifi_hosal_efuse_read_mac
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

int wifi_hosal_efuse_read_mac(uint8_t mac[6])
{
  return g_wifi_hosal_funcs.efuse_read_mac(mac);
}

/****************************************************************************
 * Name: wifi_hosal_pds_rf_turnon
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

int wifi_hosal_rf_turn_on(void *arg)
{
  return g_wifi_hosal_funcs.rf_turn_on(arg);
}

/****************************************************************************
 * Name: wifi_hosal_pds_rf_turnoff
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

int wifi_hosal_rf_turn_off(void *arg)
{
  return g_wifi_hosal_funcs.rf_turn_off(arg);
}

/****************************************************************************
 * Name: wifi_hosal_adc_device_get
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

hosal_adc_dev_t* wifi_hosal_adc_device_get(void)
{
  return g_wifi_hosal_funcs.adc_device_get();
}


/****************************************************************************
 * Name: wifi_hosal_adc_tsen_value_get
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

int wifi_hosal_adc_tsen_value_get(hosal_adc_dev_t *adc)
{
  return g_wifi_hosal_funcs.adc_tsen_value_get(adc);
}

/****************************************************************************
 * Name: wifi_hosal_adc_tsen_value_get
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/
int wifi_hosal_pm_init(void)
{
  return g_wifi_hosal_funcs.pm_init();
}

/****************************************************************************
 * Name: wifi_hosal_adc_tsen_value_get
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/
int wifi_hosal_pm_event_register(enum PM_EVEMT event, uint32_t code, uint32_t cap_bit, uint16_t pirority, bl_pm_cb_t ops, void *arg, enum PM_EVENT_ABLE enable)
{
  return g_wifi_hosal_funcs.pm_event_register(event, code, cap_bit, pirority, ops, arg, enable);
}

/****************************************************************************
 * Name: wifi_hosal_adc_tsen_value_get
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/
int wifi_hosal_pm_deinit(void)
{
  return g_wifi_hosal_funcs.pm_deinit();
}

/****************************************************************************
 * Name: wifi_hosal_adc_tsen_value_get
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/
int wifi_hosal_pm_state_run(void)
{
  return g_wifi_hosal_funcs.pm_state_run();
}

/****************************************************************************
 * Name: wifi_hosal_adc_tsen_value_get
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/
int wifi_hosal_pm_capacity_set(enum PM_LEVEL level)
{
  return g_wifi_hosal_funcs.pm_capacity_set(level);
}

/****************************************************************************
 * Name: wifi_hosal_adc_tsen_value_get
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/
int wifi_hosal_pm_post_event(enum PM_EVEMT event, uint32_t code, uint32_t *retval)
{
  return g_wifi_hosal_funcs.pm_post_event(event, code, retval);
}

/****************************************************************************
 * Name: wifi_hosal_adc_tsen_value_get
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/
int wifi_hosal_pm_event_switch(enum PM_EVEMT event, uint32_t code, enum PM_EVENT_ABLE enable)
{
  return g_wifi_hosal_funcs.pm_event_switch(event, code, enable);
}




#ifdef __cplusplus
}
#endif

