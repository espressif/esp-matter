/***************************************************************************//**
 * @file
 * @brief CRC16 functionality for Silicon Labs bootloader
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
#ifndef BTL_CRC16_H
#define BTL_CRC16_H

#include <stdint.h>
#include <stddef.h>

/***************************************************************************//**
 * @addtogroup Components
 * @{
 * @addtogroup Security
 * @{
 * @addtogroup CRC16
 * @{
 * @brief CRC16 functionality for the bootloader
 * @details
 ******************************************************************************/

/// CRC16 start value
#define BTL_CRC16_START             0xFFFFU

/***************************************************************************//**
 * Calculate CRC16 on input.
 *
 * @param newByte    Byte to append to CRC16 calculation
 * @param prevResult Previous output from CRC algorithm. @ref BTL_CRC16_START
 *                   when starting a new calculation.
 * @return Result of the CRC16 operation
 ******************************************************************************/
uint16_t btl_crc16(const uint8_t newByte, uint16_t prevResult);

/***************************************************************************//**
 * Calculate CRC16 on input stream.
 *
 * @param buffer     Buffer containing bytes to append to CRC16 calculation
 * @param length     Size of the buffer in bytes
 * @param prevResult Previous output from CRC algorithm. @ref BTL_CRC16_START
 *                   when starting a new calculation.
 * @returns Result of the CRC16 operation
 ******************************************************************************/
uint16_t btl_crc16Stream(const uint8_t *buffer,
                         size_t        length,
                         uint16_t      prevResult);

/** @} addtogroup CRC16 */
/** @} addtogroup Security */
/** @} addtogroup Components */
#endif // BTL_CRC16_H
