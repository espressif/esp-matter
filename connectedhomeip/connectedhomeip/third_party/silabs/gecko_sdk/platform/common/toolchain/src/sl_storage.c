/***************************************************************************//**
 * @file
 * @brief Flash storage reservation api
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

#include "sl_storage.h"
#include "sl_storage_config.h"
#include "em_device.h"

// Handle the case where the user wants to use default storage size.
#if SL_BOOTLOADER_STORAGE_USE_CUSTOM_SIZE
  #define _SL_BOOTLOADER_STORAGE_SIZE  SL_BOOTLOADER_STORAGE_SIZE
#else
  // Default storage size is 16 kB less than half the flash size
  #define _SL_BOOTLOADER_STORAGE_SIZE  ((FLASH_SIZE / 2UL) - 0x4000UL)
#endif

#if defined(__GNUC__)
  __attribute__((used)) uint8_t bootloader_storage[_SL_BOOTLOADER_STORAGE_SIZE] __attribute__ ((section(".internal_storage")));
  extern char linker_storage_begin;
  #define SL_BOOTLOADER_STORAGE_ADDR   (&linker_storage_begin)
#elif defined(__ICCARM__)
  __root uint8_t bootloader_storage[_SL_BOOTLOADER_STORAGE_SIZE] @ "INTERNAL_STORAGE";
  #define SL_BOOTLOADER_STORAGE_ADDR   (&bootloader_storage[0])
#endif

sl_memory_region_t sl_storage_get_bootloader_region(void)
{
  sl_memory_region_t region;

  region.addr = SL_BOOTLOADER_STORAGE_ADDR;
  region.size = _SL_BOOTLOADER_STORAGE_SIZE;
  return region;
}
