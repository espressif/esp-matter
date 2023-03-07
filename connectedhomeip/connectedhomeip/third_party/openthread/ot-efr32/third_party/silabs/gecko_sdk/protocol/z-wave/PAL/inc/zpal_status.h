/**
 * @file
 * Defines status codes returned by zpal functions..
 *
 * @copyright 2021 Silicon Laboratories Inc.
 */

#ifndef ZPAL_STATUS_H_
#define ZPAL_STATUS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup zpal
 * @brief
 * Z-Wave Platform Abstraction Layer.
 * @{
 * @addtogroup zpal-status
 * @brief
 * Defines status codes returned by zpal functions..
 *
 * @{
 */

typedef uint32_t zpal_status_t;

#define ZPAL_STATUS_OK                ((zpal_status_t)0x0000)  ///< No error.
#define ZPAL_STATUS_FAIL              ((zpal_status_t)0x0001)  ///< Generic error.
#define ZPAL_STATUS_BUFFER_FULL       ((zpal_status_t)0x0002)  ///< There is no more room in the buffer.
#define ZPAL_STATUS_BUSY              ((zpal_status_t)0x0003)  ///< Process is busy.
#define ZPAL_STATUS_INVALID_ARGUMENT  ((zpal_status_t)0x0004)  ///< At least one argument is invalid.

/**
 * @} //zpal-status
 * @} //zpal
 */

#ifdef __cplusplus
}
#endif

#endif /* ZPAL_STATUS_H_ */
