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

#ifndef BTL_BOOTLOAD_NS_H
#define BTL_BOOTLOAD_NS_H

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

// -----------------------------------------------------------------------------
// Includes

#include <inttypes.h>
#include <stdbool.h>
#include "em_device.h"

// -----------------------------------------------------------------------------
// Function declarations

#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
// Check that an SE upgrade with a given version number is allowed to be
// installed.
bool bootload_checkSeUpgradeVersion(uint32_t upgradeVersion);
// Perform an SE upgrade.
bool bootload_commitSeUpgrade(void);
#endif

// Perform a bootloader upgrade using the upgrade image present at
// upgradeAddress with length size.
bool bootload_commitBootloaderUpgrade(uint32_t size);
// Get the version of the bootloader.
uint32_t bootload_getBootloaderVersion(void);
// Get the version of the application.
bool bootload_getApplicationVersion(uint32_t *version);

/** @endcond */

#endif // BTL_BOOTLOAD_NS_H
