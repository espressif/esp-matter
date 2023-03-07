/***************************************************************************//**
 * @file
 * @brief Energy Aware drivers error code definitions.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#ifndef __SILICON_LABS_ECODE_H__
#define __SILICON_LABS_ECODE_H__

#include <stdint.h>

/***************************************************************************//**
 * @addtogroup ecode ECODE - Error Codes
 * @details ECODE is set of error and status codes related to DMA, RTC, SPI,
 *        NVM, USTIMER, UARTDRV, EZRADIO, TEMP, and NVM3 drivers. These error and
 *        status codes are used by the above listed drivers to update the layer
 *        (using the driver) about an error or status.
 *
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief Typedef for API function error code return values.
 *
 * @details
 *        Bit 24-31:  Component, for example emdrv @n
 *        Bit 16-23:  Module, for example @ref uartdrv or @ref spidrv @n
 *        Bit 0-15:   Error code
 ******************************************************************************/
typedef uint32_t Ecode_t;

#define ECODE_EMDRV_BASE  (0xF0000000U)   ///< Base value for all EMDRV errorcodes.

#define ECODE_OK          (0U)            ///< Generic success return value.

#define ECODE_EMDRV_RTCDRV_BASE      (ECODE_EMDRV_BASE | 0x00001000U)   ///< Base value for RTCDRV error codes.
#define ECODE_EMDRV_SPIDRV_BASE      (ECODE_EMDRV_BASE | 0x00002000U)   ///< Base value for SPIDRV error codes.
#define ECODE_EMDRV_NVM_BASE         (ECODE_EMDRV_BASE | 0x00003000U)   ///< Base value for NVM error codes.
#define ECODE_EMDRV_USTIMER_BASE     (ECODE_EMDRV_BASE | 0x00004000U)   ///< Base value for USTIMER error codes.
#define ECODE_EMDRV_UARTDRV_BASE     (ECODE_EMDRV_BASE | 0x00007000U)   ///< Base value for UARTDRV error codes.
#define ECODE_EMDRV_DMADRV_BASE      (ECODE_EMDRV_BASE | 0x00008000U)   ///< Base value for DMADRV error codes.
#define ECODE_EMDRV_EZRADIODRV_BASE  (ECODE_EMDRV_BASE | 0x00009000U)   ///< Base value for EZRADIODRV error codes.
#define ECODE_EMDRV_TEMPDRV_BASE     (ECODE_EMDRV_BASE | 0x0000D000U)   ///< Base value for TEMPDRV error codes.
#define ECODE_EMDRV_NVM3_BASE        (ECODE_EMDRV_BASE | 0x0000E000U)   ///< Base value for NVM3 error codes.

/** @} (end addtogroup ecode) */

#endif // __SILICON_LABS_ECODE_H__
