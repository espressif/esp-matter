/**
 * @file
 * @brief Power Management Wrapper used by framework. Represents the interface for PM protocol module
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef ZAF_PM_WRAPPER_H_
#define ZAF_PM_WRAPPER_H_

#include <stdint.h>

/**
 * Registers functions that will be called as the last step just before the
 * chip enters deep sleep hibernate.
 *
 * NB: When the function is called the OS tick has been disabled and the FreeRTOS
 *     scheduler is no longer running. OS features like events, queues and timers
 *     are therefore unavailable and must not be called from the callback function.
 *
 *     The callback functions can be used to set pins and write to retention RAM.
 *     Do NOT try to write to the NVM file system.
 *
 * The maximum number of functions that can be registered is given by the macro
 * MAX_POWERDOWN_CALLBACKS
 *
 * @param callback Function to call on power down.
 */
void ZAF_PM_SetPowerDownCallback(void (*callback)(void));

#endif /* ZAF_PM_WRAPPER_H_*/
