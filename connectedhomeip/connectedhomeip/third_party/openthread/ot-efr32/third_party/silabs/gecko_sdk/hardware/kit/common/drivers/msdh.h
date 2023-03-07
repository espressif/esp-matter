/***************************************************************************//**
 * @file
 * @brief Host side implementation of Mass Storage class Device (MSD) interface.
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

#ifndef __MSDH_H
#define __MSDH_H

/***************************************************************************//**
 * @addtogroup kitdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup Msd
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*** MSDH Function prototypes ***/

bool MSDH_Init(uint8_t *usbDeviceInfo, int usbDeviceInfoSize);
bool MSDH_GetSectorCount(uint32_t *sectorCount);
bool MSDH_GetSectorSize(uint16_t *sectorSize);
bool MSDH_GetBlockSize(uint32_t *blockSize);
bool MSDH_ReadSectors(uint32_t lba, uint16_t sectors, void *data);
bool MSDH_WriteSectors(uint32_t lba, uint16_t sectors, const void *data);

#ifdef __cplusplus
}
#endif

/** @} (end group Msd) */
/** @} (end group kitdrv) */

#endif /* __MSDH_H */
