/***************************************************************************//**
 * @file
 * @brief Bootloading veneer library for Gecko bootloader
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

// -----------------------------------------------------------------------------
// Includes

#include "core/btl_tz_utils.h"
#include "core/btl_bootload.h"
#include "config/btl_config.h"

// -----------------------------------------------------------------------------
// NSC functions

#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
__attribute__((cmse_nonsecure_entry))
bool bootload_nsc_checkSeUpgradeVersion(uint32_t upgradeVersion)
{
  return bootload_checkSeUpgradeVersion(upgradeVersion);
}

__attribute__((cmse_nonsecure_entry))
bool bootload_nsc_commitSeUpgrade(void)
{
  return bootload_commitSeUpgrade(BTL_UPGRADE_LOCATION);
}
#endif

__attribute__((cmse_nonsecure_entry))
bool bootload_nsc_commitBootloaderUpgrade(uint32_t size)
{
  return bootload_commitBootloaderUpgrade(BTL_UPGRADE_LOCATION, size);
}

__attribute__((cmse_nonsecure_entry))
uint32_t bootload_nsc_getBootloaderVersion(void)
{
  return bootload_getBootloaderVersion();
}

__attribute__((cmse_nonsecure_entry))
bool bootload_nsc_getApplicationVersion(uint32_t *version)
{
  return bootload_getApplicationVersion(version);
}
