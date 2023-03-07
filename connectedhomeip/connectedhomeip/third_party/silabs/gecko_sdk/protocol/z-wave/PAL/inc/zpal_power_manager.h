/**
 * @file
 * Defines a platform abstraction layer for the Z-Wave power manager.
 *
 * @copyright 2021 Silicon Laboratories Inc.
 */

#ifndef ZPAL_POWER_MANAGER_H_
#define ZPAL_POWER_MANAGER_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup zpal
 * @brief
 * Z-Wave Platform Abstraction Layer.
 * @{
 * @addtogroup zpal-power-manager
 * @brief
 * Defines a platform abstraction layer for the Z-Wave power manager.
 *
 * The ZPAL power manager offers an API for registering power locks and for receiving events on power mode 
 * transitions. Power lock forces the chip to stay awake in a given power mode for a given time.
 * An event handler is invoked every time the transition between power modes occurs.
 *
 * Infinite lock example:\n
 * @code{.c}
 * zpal_pm_handle_t handle = zpal_pm_register(ZPAL_PM_TYPE_USE_RADIO);  // Register the power lock that keeps radio active
 * zpal_pm_stay_awake(handle, 0);                                       // Keep the radio active forever
 *                                                                      // Other stuff
 * zpal_pm_cancel(handle);                                              // The chip may go to sleep now
 * @endcode
 * 
 * Lock with timeout example:\n
 * @code{.c}
 * zpal_pm_handle_t handle = zpal_pm_register(ZPAL_PM_TYPE_USE_RADIO);  // Register the power lock that keeps radio active
 * zpal_pm_stay_awake(handle, 2000);                                    // Keep the radio active for 2000 ms from now
 * @endcode
 * 
 * Requirements:
 * - The storage for power locks should be able to hold at least 20 items. Note that this number may change in the future.
 *
 * @{
 */

typedef enum
{
  ZPAL_PM_TYPE_USE_RADIO,   ///< Prevents the system from entering a sleep mode where radio is unavailable.
  ZPAL_PM_TYPE_DEEP_SLEEP,  ///< Prevents the system from going to a sleep mode that requires wake up from reset state.
} zpal_pm_type_t;

typedef enum
{
  ZPAL_PM_MODE_RUNNING,     ///< Processor is active.
  ZPAL_PM_MODE_SLEEP,       ///< Processor is sleeping, but all peripherals are enabled.
  ZPAL_PM_MODE_DEEP_SLEEP,  ///< Most peripherals are disabled, but the radio is in FLiRS mode.
  ZPAL_PM_MODE_SHUTOFF,     ///< Chip is off and will boot from reset. Hence, no transition from this power mode will ever happen.
} zpal_pm_mode_t;

/**
 * @brief The ZPAL implementation must invoke this function whenever a transition happens between one of
 * the power modes defined in @ref zpal_pm_mode_t.
 * This function is implemented by Z-Wave.
 */
extern void zpal_zw_pm_event_handler(zpal_pm_mode_t from, zpal_pm_mode_t to);

/**
 * @brief Power lock handle.
 */
typedef void * zpal_pm_handle_t;


/**
* @brief Registers a power lock of a given type.
* This function must be invoked once to allocate power lock before any other API calls are made on that lock.
* @note Keep in mind that it's not possible to unregister (destroy) locks already created using zpal_pm_register(). 
*
* @param[in] type  Power lock type.
* @return A handle if successfully registered, NULL otherwise.
*/
zpal_pm_handle_t zpal_pm_register(zpal_pm_type_t type);

/**
* @brief Makes the chip stay awake for a given time in a power mode defined by @p handle.
* The power lock must be registered with @ref zpal_pm_register() prior to invoking this function.
* If invoked on an already active power lock, the timeout will be reset.
*
* @param[in] handle               Power lock handle registered by @ref zpal_pm_register().
* @param[in] timeout_milliseconds Timeout value. If zero is given, the power lock
*                                 will stay active until manually cancelled.
*/
void zpal_pm_stay_awake(zpal_pm_handle_t handle, uint32_t timeout_milliseconds);

/**
* @brief Cancels an active power lock.
* If @p handle points to NULL or inactive power lock, nothing happens.
*
* @param[in] handle Power lock handle registered by @ref zpal_pm_register().
*/
void zpal_pm_cancel(zpal_pm_handle_t handle);

/**
* @brief Cancels all active power locks.
* @note It cancels all power locks created during runtime, hence it can be used ONLY as a part of power down routine.
*/
void zpal_pm_cancel_all(void);

/**
 * @} //zpal-power-manager
 * @} //zpal
 */

#ifdef __cplusplus
}
#endif

#endif /* ZPAL_POWER_MANAGER_H_ */
