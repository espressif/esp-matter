/*******************************************************************************
 * @file  FlashDev.c
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "ccp_flash_prg.h" // FlashOS Structures

struct FlashDevice const FlashDevice __attribute__((section("DevDscr"))) = {
    FLASH_DRV_VERS, // Driver Version, do not modify!
    "RS9117_4MB_FLASH",
    EXTSPI,    // Device Type
    0x8012000, // Device Start Address
    0x3EE000,
    256,   // Programming Page Size
    0,     // Reserved, must be 0
    0xFF,  // Initial Content of Erased Memory
    10000, // Program Page Timeout in mSec
    10000, // Erase Sector Timeout in mSec

    // Specify Size and Address of Sectors
    0x001000, 0x000000, // Sector Size  4kB (1024 Sectors)
    SECTOR_END};
