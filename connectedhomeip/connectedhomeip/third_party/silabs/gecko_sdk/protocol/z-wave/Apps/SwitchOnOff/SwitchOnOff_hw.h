/**
 * @file
 * Platform abstraction for Switch On/Off application
 * 
 * @copyright 2021 Silicon Laboratories Inc.
 */
#ifndef SWITCHONOFF_HW_H_
#define SWITCHONOFF_HW_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * Initialize hardware modules specific to Switch On/Off application.
 */
void SwitchOnOff_hw_init(void);

/**
 * Binary switch handler. Called when value of the switch is changed.
 *
 * @param[in] on Switch state. True if swtich is on, false if off.
 */
void SwitchOnOff_hw_binary_switch_handler(bool on);

#endif /* SWITCHONOFF_HW_H_ */
