/***************************************************************************//**
 * @file
 * @brief Header file for RAIL Flash Data functionality
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

#ifndef __FLASHDATA_PLUGIN_H__
#define __FLASHDATA_PLUGIN_H__

#include <stdint.h>
#include <stdbool.h>

#include "rail_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Return a pointer to a valid byte array in flash.
 *
 * @param[out] data A pointer to a byte array located in flash.
 * @param[out] len A pointer to a length field corresponding to the byte array
 *   in flash. If the len pointer is NULL, no length value is returned.
 * @return The status of the flash read operation.
 *   RAIL_STATUS_INVALID_PARAMETER if an invalid data pointer is provided.
 *   RAIL_STATUS_INVALID_CALL if no valid byte array exists in flash.
 *   RAIL_STATUS_NO_ERROR on successful read of flash.
 */
RAIL_Status_t FD_ReadData(uint8_t **data, uint32_t *len);

/**
 * Write a valid byte array to flash.
 *
 * @param[in] data A pointer to a byte array to be written to flash.
 * @param[in] len The length of the byte array to be written to flash.
 * @return The status of the flash read operation.
 *   RAIL_STATUS_INVALID_PARAMETER if data is a NULL pointer or if the
 *   length of bytes to write is larger than the flash space available.
 *   RAIL_STATUS_INVALID_CALL if no valid byte array exists in flash.
 *   RAIL_STATUS_NO_ERROR on successful write of flash.
 */
RAIL_Status_t FD_WriteData(uint8_t *data, uint32_t len);

/**
 * Erase the entire flash page.
 *
 * @return The status of the flash clear operation.
 *   RAIL_STATUS_INVALID_CALL if no valid byte array exists in flash.
 *   RAIL_STATUS_NO_ERROR on successful erase of flash.
 *
 * @note This results in all bits in the entire flash page being set to 1,
 *   so all words on the page would read as 0xFFFFFFFF.
 */
RAIL_Status_t FD_ClearData(void);

/**
 * Return the array length in bytes of data previously saved in flash.
 *
 * @return The number of bytes previously saved to flash. A value of 0
 *   indicates that no application data was previously saved to flash.
 *
 * Note: Even if a value of 0 is returned, metadata might have previously
 *   been written to flash, but this metadata correlates to no current
 *   application data.
 */
uint32_t FD_GetLength(void);

/**
 * Return the maximum array length in bytes available for use in flash.
 *
 * @return The max byte array length in bytes available for use.
 */
uint32_t FD_GetMaxLength(void);

#ifdef __cplusplus
}
#endif

#endif // __FLASHDATA_PLUGIN_H__
