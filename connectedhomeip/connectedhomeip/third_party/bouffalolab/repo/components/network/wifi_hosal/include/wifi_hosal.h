/****************************************************************************
 * components/network/wifi_hosal/include/wifi_hosal.h
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

#ifndef _WIFI_HOSAL_H_
#define _WIFI_HOSAL_H_

#include <sys/_stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "stdint.h"
#include "stddef.h"

#include "hosal_adc.h"
#include "bl_pm.h"

/****************************************************************************
 * Definition
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct wifi_hosal_funcs
{
  int (*efuse_read_mac)(uint8_t mac[6]);
  int (*rf_turn_on)(void *arg);
  int (*rf_turn_off)(void *arg);
  hosal_adc_dev_t* (*adc_device_get)(void);
  int (*adc_tsen_value_get)(hosal_adc_dev_t *adc);
  int (*pm_init) (void);
  int (*pm_event_register) (enum PM_EVEMT event, uint32_t code, uint32_t cap_bit, uint16_t pirority, bl_pm_cb_t ops, void *arg, enum PM_EVENT_ABLE enable);
  int (*pm_deinit) (void);
  int (*pm_state_run) (void);
  int (*pm_capacity_set) (enum PM_LEVEL level);
  int (*pm_post_event) (enum PM_EVEMT event, uint32_t code, uint32_t *retval);
  int (*pm_event_switch) (enum PM_EVEMT event, uint32_t code, enum PM_EVENT_ABLE enable);
};

typedef struct wifi_hosal_funcs wifi_hosal_funcs_t;

/****************************************************************************
 * Private Data
 ****************************************************************************/

extern wifi_hosal_funcs_t g_wifi_hosal_funcs;


/****************************************************************************
 * Private Function
 ****************************************************************************/

static inline int hosal_wifi_ret_zero(void *arg)
{
  return 0;
}

static inline int hosal_wifi_ret_non_zero(void *arg)
{
  return -1;
}

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

int wifi_hosal_efuse_read_mac(uint8_t mac[6]);

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

int wifi_hosal_rf_turn_on(void *arg);

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

int wifi_hosal_rf_turn_off(void *arg);

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
hosal_adc_dev_t* wifi_hosal_adc_device_get(void);


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
int wifi_hosal_adc_tsen_value_get(hosal_adc_dev_t *adc);

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

int wifi_hosal_pm_init(void);
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
int wifi_hosal_pm_event_register(enum PM_EVEMT event, uint32_t code, uint32_t cap_bit, uint16_t pirority, bl_pm_cb_t ops, void *arg, enum PM_EVENT_ABLE enable);
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
int wifi_hosal_pm_deinit(void);
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
int wifi_hosal_pm_state_run(void);
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
int wifi_hosal_pm_capacity_set(enum PM_LEVEL level);
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
int wifi_hosal_pm_post_event(enum PM_EVEMT event, uint32_t code, uint32_t *retval);
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
int wifi_hosal_pm_event_switch(enum PM_EVEMT event, uint32_t code, enum PM_EVENT_ABLE enable);


#ifdef __cplusplus
}
#endif

#endif /* _WIFI_HOSAL_H_ */
