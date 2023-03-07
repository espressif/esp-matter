/***************************************************************************//**
 * @file
 * @brief CRC32 functionality for Silicon Labs bootloader
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#ifndef BTL_CRC32_H
#define BTL_CRC32_H

#include <stdint.h>
#include <stddef.h>

/***************************************************************************//**
 * @addtogroup Components
 * @{
 * @addtogroup Security
 * @{
 * @addtogroup CRC32
 * @{
 * @brief CRC32 functionality for the bootloader
 * @details
 ******************************************************************************/

/// CRC32 start value
#define BTL_CRC32_START             0xFFFFFFFFUL
/// CRC32 end value
#define BTL_CRC32_END               0xDEBB20E3UL

/***************************************************************************//**
 * Calculate CRC32 on input buffer.
 *
 * @param buffer     Buffer containing bytes to append to CRC32 calculation
 * @param length     Size of the buffer in bytes
 * @param prevResult Previous output from the CRC algorithm. Polynomial if
 *                   starting a new calculation
 * @returns Result of the CRC32 operation
 ******************************************************************************/
uint32_t btl_crc32Stream(const uint8_t *buffer,
                         size_t        length,
                         uint32_t      prevResult);

/** @} addtogroup CRC32 */
/** @} addtogroup Security */
/** @} addtogroup Components */

#endif // BTL_CRC32_H
