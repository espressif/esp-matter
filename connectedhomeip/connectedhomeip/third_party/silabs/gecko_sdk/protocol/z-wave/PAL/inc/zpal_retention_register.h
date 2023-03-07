/**
 * @file
 * Defines a platform abstraction layer for the Z-Wave retention register.
 *
 * @copyright 2021 Silicon Laboratories Inc.
 */

#ifndef ZPAL_RETENTION_REGISTER_H_
#define ZPAL_RETENTION_REGISTER_H_

#include <stdint.h>
#include "zpal_status.h"

#define ZPAL_RETENTION_REGISTER_SIZE 32

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup zpal
 * @brief
 * Z-Wave Platform Abstraction Layer.
 * @{
 * @addtogroup zpal-retention-register
 * @brief
 * Defines a platform abstraction layer for the Z-Wave retention register.
 *
 * How to use the retention register API
 *
 * The retention register should support at least 32 x 32 bit registers which can be retained in deep sleep.
 *
 * The following outlines an example of use:
 * 1. Store value in retention register using zpal_retention_register_write().
 * 2. Read value from retention register using zpal_retention_register_read().
 *
 * Note:
 * - Retention registers with index 0-15 are reserved for Z-Wave protocol code.
 * - Retention registers are frequently written, so it is not recommended to use flash memory as storage.
 *
 * @{
 */

#define ZPAL_RETENTION_REGISTER_SMARTSTART            0
#define ZPAL_RETENTION_REGISTER_RESPONSEROUTE_1       1
#define ZPAL_RETENTION_REGISTER_RESPONSEROUTE_2       2
#define ZPAL_RETENTION_REGISTER_RESPONSEROUTE_3       3
#define ZPAL_RETENTION_REGISTER_RESPONSEROUTE_4       4
#define ZPAL_RETENTION_REGISTER_DEEP_SLEEP_TICK       5
#define ZPAL_RETENTION_REGISTER_TXPOWER_RSSI_LR       6

/**
 * @brief Reads a 32-bit value from the specified retention register.
 *
 * @param[in]   index Retention register number (zero-based).
 * @param[out]  data  Pointer to a 32-bit variable where the value can be stored.
 * @return @ref ZPAL_STATUS_OK on success, @ref ZPAL_STATUS_INVALID_ARGUMENT on
 * invalid @p index or @p data and @ref ZPAL_STATUS_FAIL otherwise.
 *
 * @note Retention Register should support at least 32 objects.
 */
zpal_status_t zpal_retention_register_read(uint32_t index, uint32_t *data);

/**
 * @brief Write a 32-bit value to the specified retention register.
 *
 * @param[in] index Retention register number (zero-based).
 * @param[in] value 32-bit value to save in retention register.
 * @return @ref ZPAL_STATUS_OK on success, @ref ZPAL_STATUS_INVALID_ARGUMENT on
 * invalid @p index and @ref ZPAL_STATUS_FAIL otherwise.
 *
 * @note Retention Register should support at least 32 objects.
 */
zpal_status_t zpal_retention_register_write(uint32_t index, uint32_t value);

/**
 * @} //zpal-retention-register
 * @} //zpal
 */

#ifdef __cplusplus
}
#endif

#endif /* ZPAL_RETENTION_REGISTER_H_ */
