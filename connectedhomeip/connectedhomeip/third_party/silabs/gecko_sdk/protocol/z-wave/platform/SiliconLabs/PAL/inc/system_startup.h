/**
 * @file
 * Export of data collected during system startup
 * @copyright 2022 Silicon Laboratories Inc.
 */

#ifndef __SYSTEM_STARTUP_H__
#define __SYSTEM_STARTUP_H__

/**
 * @brief Get the wake-up pins activated that led to a wake-up
 * 
 * @return uint32_t GPIO bitmask
 */
uint32_t getWakeUpFlags(void);

#endif /* __SYSTEM_STARTUP_H__ */
