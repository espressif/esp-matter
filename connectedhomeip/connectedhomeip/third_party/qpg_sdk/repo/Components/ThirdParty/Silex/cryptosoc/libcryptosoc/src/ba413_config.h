/**
 * @file
 * @brief Defines the procedures to make operations with
 *          the BA413 hash function
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#ifndef BA413_CONFIG_H
#define BA413_CONFIG_H

#include <stdint.h>

struct ba413_regs_s {
   volatile uint32_t config;
};


/** @brief Value for ::ba413_regs_s.config, set mode to MD5 */
#define BA413_CONF_MODE_MD5      0x00000001
/** @brief Value for ::ba413_regs_s.config, set mode to SHA1 */
#define BA413_CONF_MODE_SHA1     0x00000002
/** @brief Value for ::ba413_regs_s.config, set mode to SHA224 */
#define BA413_CONF_MODE_SHA224   0x00000004
/** @brief Value for ::ba413_regs_s.config, set mode to SHA256 */
#define BA413_CONF_MODE_SHA256   0x00000008
/** @brief Value for ::ba413_regs_s.config, set mode to SHA384 */
#define BA413_CONF_MODE_SHA384   0X00000010
/** @brief Value for ::ba413_regs_s.config, set mode to SHA512 */
#define BA413_CONF_MODE_SHA512   0x00000020

/** @brief Value for ::ba413_regs_s.config, enable HMAC */
#define BA413_CONF_HMAC    0x00000100
/** @brief Value for ::ba413_regs_s.config, enable padding in hardware  */
#define BA413_CONF_HWPAD   0x00000200
/** @brief Value for ::ba413_regs_s.config, set as final (return digest and not state) */
#define BA413_CONF_FINAL   0x00000400



#endif
