/***************************************************************************//**
 * @file
 * @brief Non-secure CRC16 functions.
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

#include "security/btl_crc16.h"

// -----------------------------------------------------------------------------
// NSC functions

extern uint16_t btl_nsc_crc16(const uint8_t newByte, uint16_t prevResult);
extern uint16_t btl_nsc_crc16Stream(const uint8_t *buffer,
                                    size_t        length,
                                    uint16_t      prevResult);

// -----------------------------------------------------------------------------
// NS functions

uint16_t btl_crc16(const uint8_t newByte, uint16_t prevResult)
{
  return btl_nsc_crc16(newByte, prevResult);
}

uint16_t btl_crc16Stream(const uint8_t *buffer,
                         size_t        length,
                         uint16_t      prevResult)
{
  return btl_nsc_crc16Stream(buffer, length, prevResult);
}