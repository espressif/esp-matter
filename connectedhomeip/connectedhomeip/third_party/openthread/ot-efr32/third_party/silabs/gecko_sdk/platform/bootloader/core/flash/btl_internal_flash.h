/***************************************************************************//**
 * @file
 * @brief Abstraction of internal flash read and write routines.
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
#ifndef BTL_INTERNAL_FLASH_H
#define BTL_INTERNAL_FLASH_H

#include "core/btl_util.h"
MISRAC_DISABLE
#include "em_device.h"
MISRAC_ENABLE

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/***************************************************************************//**
 * @addtogroup Core Bootloader Core
 * @{
 * @addtogroup Flash
 * @brief Interface to internal flash
 * @details Used for writing application images to the main flash.
 * @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Defines

/// DMA Channel for MSC write
#define SL_GBL_MSC_LDMA_CHANNEL     2

// -----------------------------------------------------------------------------
// Prototypes

/**
 * Erase a flash page.
 *
 * @param[in] address Start address of the flash page to erase.
 * @return True if operation was successful
 */
bool flash_erasePage(uint32_t address);

/**
 * Write buffer to internal flash.
 *
 * @param address   Starting address to write data to. Must be half-word aligned.
 * @param data      Data buffer to write to internal flash
 * @param length    Amount of bytes in the data buffer to write
 * @param ch        DMA channel to use
 * @return True if operation was successful
 */
bool flash_writeBuffer_dma(uint32_t       address,
                           void           *data,
                           size_t         length,
                           int            ch);

/**
 * Write buffer to internal flash.
 *
 * @param address   Starting address to write data to. Must be half-word aligned.
 * @param data      Data buffer to write to internal flash
 * @param length    Amount of bytes in the data buffer to write
 * @return True if operation was successful
 */
bool flash_writeBuffer(uint32_t       address,
                       void           *data,
                       size_t         length);

/** @} addtogroup internal_flash */
/** @} addtogroup core */

#endif // BTL_INTERNAL_FLASH_H
