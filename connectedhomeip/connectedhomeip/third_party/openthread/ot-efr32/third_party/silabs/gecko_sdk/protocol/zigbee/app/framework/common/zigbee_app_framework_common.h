/***************************************************************************//**
 * @brief Zigbee Application Framework common code.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef ZIGBEE_APP_FRAMEWORK_COMMON_H
#define ZIGBEE_APP_FRAMEWORK_COMMON_H
#include "sl_component_catalog.h"
#include "zigbee_app_framework_event.h"

#define SL_ZIGBEE_INIT_LEVEL_EVENT      0x00
#define SL_ZIGBEE_INIT_LEVEL_LOCAL_DATA 0x01
#define SL_ZIGBEE_INIT_LEVEL_DONE       0x02

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
sl_power_manager_on_isr_exit_t sli_zigbee_sleep_on_isr_exit(void);
#endif //#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)

void sli_zigbee_stack_init_callback(void);
void sli_zigbee_app_framework_init_callback(void);
void sli_zigbee_ncp_init_callback(void);

void sli_zigbee_stack_tick_callback(void);
void sli_zigbee_app_framework_tick_callback(void);
void sli_zigbee_ncp_tick_callback(void);

void sli_zigbee_app_framework_sleep_init(void);
bool sli_zigbee_app_framework_is_ok_to_sleep(void);

void sli_zigbee_common_rtos_init_callback(void);
void sli_zigbee_common_rtos_wakeup_isr_callback(void);

uint32_t sli_zigbee_app_framework_set_pm_requirements_and_get_ms_to_next_wakeup(void);

//------------------------------------------------------------------------------
// Internal debug print stub macros

#ifndef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
#define sl_zigbee_stack_debug_print(...)
#define sl_zigbee_core_debug_print(...)
#define sl_zigbee_app_debug_print(...)
#define sl_zigbee_app_debug_print_buffer(...)
#define sl_zigbee_zcl_debug_print(...)
#define sl_zigbee_legacy_af_debug_print(...)
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT

#endif // ZIGBEE_APP_FRAMEWORK_COMMON_H
