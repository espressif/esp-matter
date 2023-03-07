/**
 * @file
 * Platform abstraction for Door Lock Key Pad application
 * 
 * @copyright 2021 Silicon Laboratories Inc.
 */
#ifndef DOORLOCKKEYPAD_HW_H_
#define DOORLOCKKEYPAD_HW_H_

#include <stdbool.h>
#include <stdint.h>

#define BATTERY_LEVEL_REPORTING_DECREMENTS   10  // Round off and report the level in 10% decrements (100%, 90%, 80%, etc)

/**
 * Initialize hardware modules specific to Door Lock Key Pad application.
 */
void DoorLockKeyPad_hw_init(void);

/**
 * Latch status handler. Called when latch status is changed.
 *
 * @param[in] opened Latch status. True if latch is opened, false if closed.
 */
void DoorLockKeyPad_hw_latch_status_handler(bool opened);

/**
 * Bolt status handler. Called when bolt status is changed.
 *
 * @param[in] locked Bolt status. True if bolt is locked, false if unlocked.
 */
void DoorLockKeyPad_hw_bolt_status_handler(bool locked);

/**
 * Get battery level.
 *
 * @return Battery level.
 */
uint8_t DoorLockKeyPad_hw_get_battery_level(void);

#endif /* DOORLOCKKEYPAD_HW_H_ */
