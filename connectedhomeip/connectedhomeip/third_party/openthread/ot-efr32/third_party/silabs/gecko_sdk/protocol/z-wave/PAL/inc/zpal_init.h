/**
 * @file
 * Defines a platform abstraction layer for the Z-Wave initialization
 *
 * @copyright 2021 Silicon Laboratories Inc.
 */

#ifndef ZPAL_INIT_H_
#define ZPAL_INIT_H_

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
 * @addtogroup zpal-init
 * @brief
 * Defines a platform abstraction layer for the Z-Wave initialization
 *
 * How to use the initialization API
 *
 * The ZPAL initialization API is required to validate if application can be run on specific chip.
 *
 * zpal_init_is_valid() must be implemented as Z-Wave always invokes it.
 * zpal_init_invalidate() will be invoked only if zpal_init_is_valid() return false.
 * zpal_system_startup() is an externally defined and implemented by Z-Wave SDK.
 *
 * The following outlines an example of use:
 * 1. Initialize hardware.
 * 2. Invoke zpal_system_startup() at the end of main().
 * 3. During Z-Wave SDK initialization zpal_init_is_valid() will be invoked.
 * 4. If zpal_init_is_valid() returns false, zpal_init_invalidate() will be invoked.
 *
 * Requirements:
 * - The ZPAL initialization requires that zpal_system_startup() is invoked at the end of main() after
 *   hardware initialization is done. (check main() from PAL/templates/main.c)
 * - Function zpal_init_invalidate() never exits. It must setup the hardware so that the chip stops working.
 *
 * @{
 */

/**
* @brief Reset reason.
*/
typedef enum zpal_reset_reason_t
{
  ZPAL_RESET_REASON_PIN                = 0,     ///< Reset triggered by reset pin.
  ZPAL_RESET_REASON_DEEP_SLEEP_WUT     = 1,     ///< Reset triggered by wake up by timer from deep sleep state.
  ZPAL_RESET_REASON_WATCHDOG           = 3,     ///< Reset triggered by watchdog.
  ZPAL_RESET_REASON_DEEP_SLEEP_EXT_INT = 4,     ///< Reset triggered by external interrupt event in deep sleep state.
  ZPAL_RESET_REASON_POWER_ON           = 5,     ///< Reset triggered by power on.
  ZPAL_RESET_REASON_SOFTWARE           = 7,     ///< Reset triggered by software.
  ZPAL_RESET_REASON_BROWNOUT           = 9,     ///< Reset triggered by brownout circuit.
  ZPAL_RESET_REASON_OTHER              = 0xFF,  ///< Reset triggered by none of the above.
} zpal_reset_reason_t;

/**
 * @brief Returns whether generic and specific is a valid combination for current chip.
 *
 * @param[in] generic_type  Generic type of the running application.
 * @param[in] specific_type Specific type of the running application.
 * @return True if combination is valid, false otherwise.
 */
bool zpal_init_is_valid(uint8_t generic_type, uint8_t specific_type);

/**
 * @brief Setup the hardware so that the chip stops working.
 *
 * @note This function never exits.
 */
void zpal_init_invalidate(void);

/**
 * @brief Get reset reason.
 * @return Reset reason.
 */
zpal_reset_reason_t zpal_get_reset_reason(void);

/**
 * @brief System startup, implemented by Z-Wave SDK.
 *
 * @param[in] reset_reason Reset reason.
 *
 * @note This function never exits.
 */
extern void zpal_system_startup(zpal_reset_reason_t reset_reason);

/**
 * @} //zpal-init
 * @} //zpal
 */

#ifdef __cplusplus
}
#endif

#endif /* ZPAL_INIT_H_ */
