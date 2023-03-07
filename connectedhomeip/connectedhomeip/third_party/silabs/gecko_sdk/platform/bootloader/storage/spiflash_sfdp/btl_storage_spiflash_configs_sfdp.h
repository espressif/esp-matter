/***************************************************************************//**
 * @file
 * @brief Spiflash configuration with JEDEC support
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

#ifndef BTL_STORAGE_SPIFLASH_CONFIGS_SFDP
#define BTL_STORAGE_SPIFLASH_CONFIGS_SFDP

#include "config/btl_config.h"

#include "btl_storage_spiflash_sfdp.h"

/***************************************************************************//**
 * @addtogroup Components
 * @{
 * @addtogroup Storage
 * @{
 * @addtogroup SpiflashConfigsSfdp SPI Flash Configurations using SFDP
 * @brief Configuration parameters for SPI flashes Using JEDEC SFDP Standard
 * @details
 * @{
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

// The flash word size in bytes
#define DEVICE_WORD_SIZE                    (1)

// JEDEC Manufacture ID for ATMEL
#define MFG_ID_ATMEL                        (0x1F)

// JEDEC SFDP signature
#define SFDP_SIGNATURE                      (0x53464450)

// JEDEC DEVICE IDs
#define DEVICE_ID_ATMEL_4M                  (0x4401)
#define DEVICE_ID_ATMEL_8M                  (0x4501)

// Protocol commands
#define CMD_READ_SFDP                       (0x5A)
#define CMD_WRITE_ENABLE                    (0x06)
#define CMD_WRITE_DISABLE                   (0x04)
#define CMD_READ_STATUS                     (0x05)
#define CMD_WRITE_STATUS                    (0x01)
#define CMD_READ_DATA                       (0x03)
#define CMD_PAGE_PROG                       (0x02)
#define CMD_ERASE_SECTOR                    (0x20)
#define CMD_ERASE_BLOCK                     (0xD8)
#define CMD_ERASE_CHIP                      (0xC7)
#define CMD_POWER_DOWN                      (0xB9)
#define CMD_POWER_UP                        (0xAB)
#define CMD_JEDEC_ID                        (0x9F)
#define CMD_UNIQUE_ID                       (0x4B)

// Bitmasks for status register fields
#define STATUS_BUSY_MASK                    (0x01)
#define STATUS_WEL_MASK                     (0x02)

// These timings represent the worst case out of all chips supported by this
//  driver.  Some chips may perform faster.
// (in general Winbond is faster than Macronix is faster than Numonyx)

#define TIMING_POWERON_MAX_US               (30000)
#define TIMING_POWERDOWN_MAX_US             (10000)
// (MS units are 1024Hz based)
#define TIMING_ERASE_SECTOR_MAX_MS          (410)
#define TIMING_ERASE_BLOCK_MAX_MS           (3072)

/** @endcond */

#ifdef DOXY_DOC_ONLY
/// Support all Atmel devices
#define BTL_STORAGE_SPIFLASH_ATMEL_DEVICES
#endif

#if defined(BTL_STORAGE_SPIFLASH_ATMEL_DEVICES)
/// Support Atmel AT25DF041A
#define BTL_STORAGE_SPIFLASH_ATMEL_AT25DF041A    // 4MB
/// Support Atmel AT25DF081A
#define BTL_STORAGE_SPIFLASH_ATMEL_AT25DF081A    // 8MB
#endif


/** @} // addtogroup SpiflashConfigsSfdp */
/** @} // addtogroup Storage */
/** @} addtogroup Components  */
#endif // BTL_STORAGE_SPIFLASH_CONFIGS_SFDP