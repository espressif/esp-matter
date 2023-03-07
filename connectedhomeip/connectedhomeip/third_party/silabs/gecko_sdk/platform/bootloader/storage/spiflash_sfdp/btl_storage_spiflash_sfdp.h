/***************************************************************************//**
 * @file
 * @brief Spiflash-backed storage component with JEDEC support for Silicon Labs Bootloader.
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

#ifndef BTL_STORAGE_SPIFLASH_SFDP_H
#define BTL_STORAGE_SPIFLASH_SFDP_H

// Uncomment once JEDEC SFDP is made available
///***************************************************************************//**
// * @addtogroup Components
// * @{
// * @addtogroup Storage
// * @{
// * @page SPI Flash Using JEDEC SFDP Standard
// *   The SPI Flash storage implementation using JEDEC supports all SPI Flash parts
// *   that are JEDEC supported, including the following:
// *   - Spansion S25FL208K (8Mbit)
// *   - Winbond W25X20BV (2Mbit), W25Q80BV (8Mbit)
// *   - Macronix MX25L2006E (2Mbit), MX25L4006E (4Mbit), MX25L8006E (8Mbit),
// *     MX25R8035F (8Mbit low power), MX25L1606E (16Mbit),
// *     MX25U1635E (16Mbit 2Volt), MX25R6435F (64Mbit low power)
// *   - Atmel/Adesto AT25DF041A (4Mbit), AT25DF081A (8Mbit)
// *   - Numonyx/Micron M25P20 (2Mbit), M25P40 (4Mbit), M25P80 (8Mbit),
// *     M25P16 (16Mbit)
// *   - ISSI IS25LQ025B (256Kbit), IS25LQ512B (512Kbit), IS25LQ010B (1Mbit),
// *     IS25LQ020B (2Mbit), IS25LQ040B (4Mbit)
// *
// *   The supported devices will be detected and configured at runtime using the
// *   SFDP standard. The driver queries the parameter table present within a
// *   reserved section of the flash memory to get the device properties. The driver
// *   initializes the flash device in accordance with the values present in the
// *   parameter table.
// *   Including support for SFDP standard in the bootloader will lead to slightly
// *   larger bootloader code sizes
// *
// *   The SPI Flash storage implementation does not support any write protection
// *   functionality.
// ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

// -----------------------------------------------------------------------------
// Typedefs

/// Unique identifiers for supported SPI flash parts
typedef enum {
  UNKNOWN_DEVICE,
  ATMEL_4M_DEVICE,
  ATMEL_8M_DEVICE,
} StorageSpiflashDevice_t;

// -----------------------------------------------------------------------------
// Defines

// -----------------------------------------------------------------------------
// Functions

/** @endcond */

/** @} // addtogroup storage */
/** @} addtogroup Components  */

#endif // BTL_STORAGE_SPIFLASH_SFDP_H
