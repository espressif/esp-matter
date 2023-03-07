/***************************************************************************//**
 * @file
 * @brief The TZ utilities for Gecko Bootloader
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

#ifndef BTL_TZ_UTILS_H
#define BTL_TZ_UTILS_H

// -----------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>

/***************************************************************************//**
 * @addtogroup Core Bootloader Core
 * @{
 * @addtogroup TrustZone
 * @brief TrustZone utilities
 * @details
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * TrustZone fatal error handler.
 *
 * This function triggers a soft-reset with the reset reason set to
 * BOOTLOADER_RESET_REASON_TZ_FAULT.
 ******************************************************************************/
void bl_fatal_assert_action(void);

/***************************************************************************//**
 * Validate the non-secure pointers.
 *
 * Validate if the address of the memory is actually non-secure as expected.
 *
 * @param p Pointer to the memory.
 * @param s Size of the memory to be validated.
 *
 * @return True if the memory being accessed is from NS
 ******************************************************************************/
bool bl_verify_ns_memory_access(const void *p, size_t s);

/** @} (end addtogroup TrustZone) */
/** @} (end addtogroup Core) */

#endif // BTL_TZ_UTILS_H
