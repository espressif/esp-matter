/***************************************************************************//**
 * @file
 * @brief Non-secure bootloading functions.
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

#include "core/btl_bootload_ns.h"

// -----------------------------------------------------------------------------
// NSC functions

extern bool bootload_nsc_checkSeUpgradeVersion(uint32_t upgradeVersion);
extern bool bootload_nsc_commitSeUpgrade(void);
extern bool bootload_nsc_commitBootloaderUpgrade(uint32_t size);
extern uint32_t bootload_nsc_getBootloaderVersion(void);
extern bool bootload_nsc_getApplicationVersion(uint32_t *version);

// -----------------------------------------------------------------------------
// NS functions

#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
bool bootload_checkSeUpgradeVersion(uint32_t upgradeVersion)
{
  return bootload_nsc_checkSeUpgradeVersion(upgradeVersion);
}

bool bootload_commitSeUpgrade(void)
{
  return bootload_nsc_commitSeUpgrade();
}
#endif // SEMAILBOX_PRESENT || CRYPTOACC_PRESENT

bool bootload_commitBootloaderUpgrade(uint32_t size)
{
  return bootload_nsc_commitBootloaderUpgrade(size);
}

uint32_t bootload_getBootloaderVersion(void)
{
  return bootload_nsc_getBootloaderVersion();
}

bool bootload_getApplicationVersion(uint32_t *version)
{
  return bootload_nsc_getApplicationVersion(version);
}
