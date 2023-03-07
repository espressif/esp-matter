/**
 * @file
 * Defines a platform abstraction layer for the Z-Wave random number generation.
 *
 * @copyright 2021 Silicon Laboratories Inc.
 */

#ifndef ZPAL_ENTROPY_H_
#define ZPAL_ENTROPY_H_

#include <stddef.h>
#include <stdint.h>
#include "zpal_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup zpal
 * @brief
 * Z-Wave Platform Abstraction Layer.
 * @{
 * @addtogroup zpal-entropy
 * @brief
 * Defines a platform abstraction layer for the Z-Wave random number generation.
 *
 * How to use the entropy API:
 *
 * All the ZPAL entropy APIs are required to be implemented.
 *
 * The following outlines an example of use:
 * 1. Initialize module with zpal_entropy_init().
 * 2. Invoke zpal_get_entropy() or zpal_get_pseudo_random() to get generated data.
 *
 * Requirements:
 * - zpal_get_entropy() generates random data from entropy sources available on platform.
 *   Since collecting entropy from hardware may take some time, Z-Wave SDK shall invoke it
 *   only once during initialization of PRNG.
 * - zpal_get_pseudo_random() is used for non-security related operations such as generating
 *   session IDs or calculating delays. Therefore, the execution speed is highly preferred over
 *   true randomness of the output.
 *
 * @{
 */

/**
 * @brief Initialize entropy module.
 */
void zpal_entropy_init(void);

/**
 * @brief Generate true random array of bytes based on the given length and fill a data buffer.
 * Note that this function may shut down the radio while getting the data.
 *
 * @param[out]  data  A pointer to where the random array of bytes are stored in data buffer.
 *                    It MUST NOT be NULL.
 * @param[in]   len   The requested size of the random array of bytes that shall be stored
 *                    in the data buffer.
 * @return @ref ZPAL_STATUS_OK if the random array of bytes are generated and stored in the data buffer.
 *         @ref ZPAL_STATUS_OK if the random array of bytes are not generated.
 */
zpal_status_t zpal_get_entropy(uint8_t *data, size_t len);

/**
 * @brief Returns a pseudo random byte.
 *
 * @return Pseudo random byte.
 */
uint8_t zpal_get_pseudo_random(void);

/**
 * @} //zpal-entropy
 * @} //zpal
 */

#ifdef __cplusplus
}
#endif

#endif /* ZPAL_ENTROPY_H_ */
