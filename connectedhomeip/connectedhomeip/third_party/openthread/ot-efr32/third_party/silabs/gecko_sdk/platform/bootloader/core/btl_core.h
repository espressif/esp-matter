/***************************************************************************//**
 * @file
 * @brief Core functionality for Silicon Labs bootloader.
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

#ifndef BTL_CORE_H
#define BTL_CORE_H

#include <stdint.h>

/***************************************************************************//**
 * @addtogroup Core Bootloader Core
 * @brief Core bootloader functionality.
 * @details Core functionality for the Silicon Labs Bootloader.
 * @{
 ******************************************************************************/

/**
 * Initialize bootloader.
 *
 * @return Error code. @ref BOOTLOADER_OK on success, else error code in
 *         @ref BOOTLOADER_ERROR_INIT_BASE range.
 */
int32_t btl_init(void);

/**
 * Deinitialize bootloader.
 *
 * @return Error code. @ref BOOTLOADER_OK on success, else error code in
 *         @ref BOOTLOADER_ERROR_INIT_BASE range.
 */
int32_t btl_deinit(void);

/** @} addtogroup core */

#endif // BTL_CORE_H
