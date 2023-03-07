/***************************************************************************//**
 * @file
 * @brief Header file for RAIL Flash Data Configurations
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef __FLASHDATACONFIG_PLUGIN_H__
#define __FLASHDATACONFIG_PLUGIN_H__

#include "stdint.h"

#include "em_device.h"

/**
 * The page size of flash for the current hardware. For example, the value
 * FLASH_PAGE_SIZE can be used from em_device.h.
 */
#ifndef FLASH_DATA_PAGE_SIZE
#define FLASH_DATA_PAGE_SIZE FLASH_PAGE_SIZE
#endif

/**
 * A randomized 32-bit (word-sized) value stored at the beginning of flash
 * intended for flash usage purposes. Whether or not this value exists in flash
 * indicates if a particular flash location was previously used to save data.
 */
#ifndef FLASH_DATA_PREFIX
#define FLASH_DATA_PREFIX 0xD505DA05UL
#endif

/**
 * The default byte array to be written into flash.
 *
 * Note: In order for an application to use the define FLASH_DATA_BYTE_ARRAY,
 * the application must not use the define FLASH_DATA_BASE_ADDR.
 * Setting FLASH_DATA_BYTE_ARRAY to "" by default still causes a '\0'
 * character to be written to flash, so FD_GetLength() will return 1.
 */
#ifndef FLASH_DATA_BYTE_ARRAY
#define FLASH_DATA_BYTE_ARRAY ""
#endif

#endif // __FLASHDATACONFIG_PLUGIN_H__
