/**
 * @file
 * Platform abstraction for Key fob application
 * 
 * @copyright 2022 Silicon Laboratories Inc.
 */
#ifndef KEYFOB_HW_H_
#define KEYFOB_HW_H_

#include <stdint.h>
#include <ZW_basis_api.h>

#define BATTERY_LEVEL_REPORTING_DECREMENTS   10  // Round off and report the level in 10% decrements (100%, 90%, 80%, etc)

void KeyFob_hw_init(EResetReason_t reset_reason);
uint8_t KeyFob_hw_get_battery_level(void);
void KeyFob_hw_deep_sleep_wakeup_handler(void);
void KeyFob_basic_on_Led_handler(bool ledOn);
void KeyFob_basic_off_Led_handler(bool ledOn);
void KeyFob_group_add_Led_handler(bool ledOn);
void KeyFob_group_remove_Led_handler(bool ledOn);
void KeyFob_node_add_remove_Led_handler(bool ledOn);
void KeyFob_network_learnmode_led_handler(bool ledOn);
#endif /* KEYFOB_HW_H_ */
