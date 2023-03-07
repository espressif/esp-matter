/***************************************************************************//**
 * @file
 * @brief Standalone bootloader HAL functions
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

#include PLATFORM_HEADER
#include "hal/hal.h"

#include "stack/include/ember-types.h"
#include "stack/include/error.h"
#if defined(EMBER_STACK_CONNECT)
#include "stack/include/stack-info.h"
#endif
#include "bootloader-common.h"
#include "bootloader-interface.h"
#include "bootloader-interface-standalone.h"

#include "api/btl_interface.h"

extern uint8_t emGetPhyRadioChannel(void);
extern int8_t emGetPhyRadioPower(void);

#if !defined _SILICON_LABS_32B_SERIES_1_CONFIG_1

#define NO_BAT

static bool bootloaderIsCommonBootloader(void)
{
  return true;
}

#else

static bool bootloaderIsCommonBootloader(void)
{
  if (halBootloaderAddressTable->baseTable.type == BOOTLOADER_ADDRESS_TABLE_TYPE) {
    return false;
  } else {
    return true;
  }
}

#endif

EmberStatus halLaunchStandaloneBootloader(uint8_t mode)
{
  if (bootloaderIsCommonBootloader()) {
    if (!(mainBootloaderTable->capabilities & BOOTLOADER_CAPABILITY_COMMUNICATION)) {
      return EMBER_ERR_FATAL;
    }
  } else {
#ifndef NO_BAT
    if (BOOTLOADER_BASE_TYPE(halBootloaderAddressTable->bootloaderType)
        != BL_TYPE_STANDALONE) {
      return EMBER_ERR_FATAL;
    }
#else
    return EMBER_ERR_FATAL;
#endif
  }
  if ((mode == STANDALONE_BOOTLOADER_NORMAL_MODE)
      || (mode == STANDALONE_BOOTLOADER_RECOVERY_MODE)) {
    // should never return
    // standard bootloader reset
    halInternalSysReset(RESET_BOOTLOADER_BOOTLOAD);
  }

  return EMBER_ERR_FATAL;
}

uint16_t halGetStandaloneBootloaderVersion(void)
{
  if (bootloaderIsCommonBootloader()) {
    // assumes major and minor versions won't exceed 4 bits width and
    // customer version version won't exceed 8 bits width, even though
    // these are defined as 8 bits and 16 bits, respectively, in btl_config.h
    uint8_t verMajor = (mainBootloaderTable->header.version >> 24) & 0x0F; // low nibble of major version (top byte)
    uint8_t verMinor = (mainBootloaderTable->header.version >> 16) & 0x0F; // low nibble of minor version (2nd highest byte)
    uint8_t verCustomer = mainBootloaderTable->header.version & 0xFF; // low byte of customer version (low word)
    return (verMajor << 12 | verMinor << 8 | verCustomer);
  } else {
#ifndef NO_BAT
    if (BOOTLOADER_BASE_TYPE(halBootloaderAddressTable->bootloaderType)
        == BL_TYPE_STANDALONE) {
      return halGetBootloaderVersion();
    }
#endif
    return BOOTLOADER_INVALID_VERSION;
  }
}
