/***************************************************************************//**
 * @file
 * @brief Spiflash-backed storage component for Silicon Labs Bootloader.
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

#ifndef BTL_STORAGE_SPIFLASH_H
#define BTL_STORAGE_SPIFLASH_H

/***************************************************************************//**
 * @addtogroup Storage
 * @{
 * @page storage_spiflash SPI Flash
 *   The SPI Flash storage implementation supports a variety of SPI Flash parts
 *   including the following:
 *   - Spansion S25FL208K (8Mbit)
 *   - Winbond W25X20BV (2Mbit), W25Q80BV (8Mbit)
 *   - Macronix MX25L2006E (2Mbit), MX25L4006E (4Mbit), MX25L8006E (8Mbit),
 *     MX25R8035F (8Mbit low power), MX25L1606E (16Mbit),
 *     MX25U1635E (16Mbit 2Volt), MX25R3235F (32Mbit ultra low power) MX25R6435F (64Mbit low power)
 *   - Atmel/Adesto AT25DF041A (4Mbit), AT25DF081A (8Mbit)
 *   - Numonyx/Micron M25P20 (2Mbit), M25P40 (4Mbit), M25P80 (8Mbit),
 *     M25P16 (16Mbit)
 *   - ISSI IS25LQ025B (256Kbit), IS25LQ512B (512Kbit), IS25LQ010B (1Mbit),
 *     IS25LQ020B (2Mbit), IS25LQ040B (4Mbit)
 *
 *   The subset of supported devices can be configured at compile
 *   time using the configuration defines given in @ref SpiflashConfigs.
 *   Including support for multiple devices requires more Flash space in
 *   the bootloader.
 *
 *   The SPI Flash storage implementation does not support any write protection
 *   functionality.
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

// -----------------------------------------------------------------------------
// Typedefs

/// Unique identifiers for supported SPI flash parts
typedef enum {
  UNKNOWN_DEVICE,
  SPANSION_8M_DEVICE,
  WINBOND_2M_DEVICE,
  WINBOND_8M_DEVICE,
  MACRONIX_2M_DEVICE,
  MACRONIX_4M_DEVICE,
  MACRONIX_8M_DEVICE,
  MACRONIX_8M_LP_DEVICE,
  MACRONIX_16M_DEVICE,
  MACRONIX_16M_2V_DEVICE,
  MACRONIX_32M_LP_DEVICE,
  MACRONIX_64M_LP_DEVICE,
  ATMEL_4M_DEVICE,
  ATMEL_8M_DEVICE,
  // N.B. If add more ATMEL_ devices, update storage_init() accordingly
  ADESTO_4M_DEVICE,
  NUMONYX_2M_DEVICE,
  NUMONYX_4M_DEVICE,
  NUMONYX_8M_DEVICE,
  NUMONYX_16M_DEVICE,
  // N.B. If add more NUMONYX_ devices, update storage_eraseRaw() accordingly
  ISSI_256K_DEVICE,
  ISSI_512K_DEVICE,
  ISSI_1M_DEVICE,
  ISSI_2M_DEVICE,
  ISSI_4M_DEVICE,
} StorageSpiflashDevice_t;

// -----------------------------------------------------------------------------
// Defines

// -----------------------------------------------------------------------------
// Functions

/** @endcond */

/** @} // addtogroup storage */

#endif // BTL_STORAGE_SPIFLASH_H
