/**
 * @file
 * Platform abstraction for Power Strip application
 * 
 * @copyright 2021 Silicon Laboratories Inc.
 */
#ifndef POWERSTRIP_HW_H_
#define POWERSTRIP_HW_H_

#include <stdbool.h>
#include <CC_MultilevelSwitch_Support.h>

/**
 * Initialize hardware modules specific to Power Strip application.
 */
void PowerStrip_hw_init(void);

/**
 * Binary switch handler. Called when value of the switch is changed.
 *
 * @param[in] on Switch state. True if swtich is on, false if off.
 */
void PowerStrip_hw_binary_switch_handler(bool on);

/**
 * Multilevel switch handler. Called when value of the switch is changed.
 *
 * @param[in] p_switch Pointer to corresponding @ref cc_multilevel_switch_t object.
 */
void PowerStrip_hw_multilevel_switch_handler(cc_multilevel_switch_t * p_switch);

#endif /* POWERSTRIP_HW_H_ */
