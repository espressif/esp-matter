/***************************************************************************//**
 * @file
 * @brief Norflash driver
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

#ifndef __NORFLASH_H
#define __NORFLASH_H

#include <stdint.h>
#include <stdbool.h>

/***************************************************************************//**
 * @addtogroup kitdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup NORFlash
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/** @brief NORFlash status enumerator. */
typedef enum {
  NORFLASH_STATUS_OK            = 0,            /**< No errors detected.                        */
  NORFLASH_UNSUPPORTED_DEVICE   = -1,           /**< The flash is not supported by the driver.  */
  NORFLASH_NOT_CFI_DEVICE       = -2,           /**< The flash is not CFI compliant.            */
  NORFLASH_WRITE_FAILURE        = -3,           /**< Flash write/erase failure.                 */
  NORFLASH_WRITE_TIMEOUT        = -4,           /**< Flash write/erase timeout.                 */
  NORFLASH_INVALID_ADDRESS      = -5,           /**< Invalid flash address.                     */
  NORFLASH_MISALIGNED_ADDRESS   = -6,           /**< Misaligned flash word address.             */
} NORFLASH_Status_TypeDef;

/** @brief NORFLASH device information struct. */
typedef struct {
  uint32_t baseAddress;               /**< The device base address in cpu memory map.   */
  uint32_t deviceId;                  /**< The device ID.                               */
  uint32_t deviceSize;                /**< Total device size in bytes.                  */
  uint16_t manufacturerId;            /**< The device manufacturer ID.                  */
  uint32_t sectorCount;               /**< Number of sectors in the device.             */
  uint32_t sectorSize;                /**< Sector size in bytes.                        */
  uint32_t bootSectorCount;           /**< Number of boot sectors in the device.        */
  uint32_t bootSectorSize;            /**< Boot sector size in bytes.                   */
  uint16_t writeBufferSize;           /**< Write buffer size of the device.             */
  uint8_t  deviceArc;                 /**< The device sector architecture.              */
} NORFLASH_Info_TypeDef;

bool                    NORFLASH_AddressValid(uint32_t addr);
NORFLASH_Info_TypeDef*  NORFLASH_DeviceInfo(void);
int                     NORFLASH_EraseDevice(void);
int                     NORFLASH_EraseSector(uint32_t addr);
int                     NORFLASH_Init(void);
int                     NORFLASH_ProgramByte(uint32_t addr, uint8_t data);
int                     NORFLASH_ProgramWord16(uint32_t addr, uint16_t data);
int                     NORFLASH_ProgramWord32(uint32_t addr, uint32_t data);
int                     NORFLASH_Program(uint32_t addr, uint8_t *data, uint32_t count);

#ifdef __cplusplus
}
#endif

/** @} (end group NORFlash) */
/** @} (end group kitdrv) */

#endif /* __NORFLASH_H */
