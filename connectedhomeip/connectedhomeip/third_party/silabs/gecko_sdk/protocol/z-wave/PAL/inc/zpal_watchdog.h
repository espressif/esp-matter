/**
 * @file
 * Defines a platform abstraction layer for the Z-Wave watchdog.
 *
 * @copyright 2021 Silicon Laboratories Inc.
 */

#ifndef ZPAL_WATCHDOG_H_
#define ZPAL_WATCHDOG_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup zpal
 * @brief
 * Z-Wave Platform Abstraction Layer.
 * @{
 * @addtogroup zpal-watchdog
 * @brief
 * Defines a platform abstraction layer for the Z-Wave watchdog.
 *
 * How to use the watchdog API
 *
 * The following outlines an example of use:
 * 1. Enable the watchdog functionality with zpal_enable_watchdog().
 * 2. Periodically invoke zpal_feed_watchdog() to restart watchdog.
 * 3. If watchdog needs to be disabled, check current state with zpal_is_watchdog_enabled(),
 *    disable it by invoking zpal_enable_watchdog(), then restore previous state.
 *
 * Note:
 * - Recommended watchdog timeout period is 1 second.
 *
 * @{
 */

/**
 * @brief Returns whether the watchdog is enabled.
 *
 * @return True if watchdog is enabled, false if not.
 */
bool zpal_is_watchdog_enabled(void);

/**
 * @brief Enables or disables the watchdog based on the input.
 *
 * @param[in] enable true enables the watchdog and false disables it.
 */
void zpal_enable_watchdog(bool enable);

/**
 * @brief Feeds the watchdog.
 */
void zpal_feed_watchdog(void);

/**
 * @} //zpal-watchdog
 * @} //zpal
 */

#ifdef __cplusplus
}
#endif

#endif /* ZPAL_WATCHDOG_H_ */
