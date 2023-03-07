/***************************************************************************//**
 * @file
 * @brief See @ref crc for detailed documentation.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#ifndef __CRC_H__
#define __CRC_H__

/***************************************************************************//**
 * @addtogroup legacyhal
 * @{
 ******************************************************************************/

/** @addtogroup crc Cyclic Redundancy Check
 * @brief Functions that provide access to cyclic redundancy code (CRC)
 * calculation. See crc.h for source code.
 *@{
 */

/** @brief Calculates 16-bit cyclic redundancy code (CITT CRC 16).
 *
 * Applies the standard CITT CRC 16 polynomial to a
 * single byte. It should support being called first with an initial
 * value, then repeatedly until all data is processed.
 *
 * @param newByte     The new byte to be run through CRC.
 *
 * @param prevResult  The previous CRC result.
 *
 * @return The new CRC result.
 */
uint16_t halCommonCrc16(uint8_t newByte, uint16_t prevResult);

/** @brief Calculates 32-bit cyclic redundancy code
 *
 * @note On some radios or micros, the CRC
 * for error detection on packet data is calculated in hardware.
 *
 * Applies a CRC32 polynomial to a
 * single byte. It should support being called first with an initial
 * value, then repeatedly until all data is processed.
 *
 * @param newByte       The new byte to be run through CRC.
 *
 * @param prevResult    The previous CRC result.
 *
 * @return The new CRC result.
 */
uint32_t halCommonCrc32(uint8_t newByte, uint32_t prevResult);

// Commonly used initial and expected final CRC32 values
#define INITIAL_CRC             0xFFFFFFFFL
#define CRC32_START             INITIAL_CRC
#define CRC32_END               0xDEBB20E3L  // For CRC32 POLYNOMIAL run LSB-MSB

/** @} (end addtogroup crc) */
/** @} (end addtogroup legacyhal) */

#endif //__CRC_H__
