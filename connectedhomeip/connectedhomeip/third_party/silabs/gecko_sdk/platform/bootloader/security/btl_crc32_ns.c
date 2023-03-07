/***************************************************************************//**
 * @file
 * @brief Non-secure CRC32 functions.
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

#include "security/btl_crc32.h"

// -----------------------------------------------------------------------------
// NSC functions

extern uint32_t btl_nsc_crc32Stream(const uint8_t *buffer,
                                    size_t        length,
                                    uint32_t      prevResult);

// -----------------------------------------------------------------------------
// NS functions

uint32_t btl_crc32Stream(const uint8_t *buffer,
                         size_t        length,
                         uint32_t      prevResult)
{
  return btl_nsc_crc32Stream(buffer, length, prevResult);
}