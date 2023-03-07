/***************************************************************************/ /**
 * @file
 * @brief CPC
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SLI_CPC_CRC_H
#define SLI_CPC_CRC_H

#include "sl_status.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

// -----------------------------------------------------------------------------
// Data Types

// -----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif

// Prototypes

/***************************************************************************//**
 * Computes CRC-16 CCITT on given buffer. Software implementation.
 *
 * @param buffer Pointer to the buffer on which the CRC must be computed.
 * @param buffer_length Length of the buffer, in bytes.
 *
 * @return CRC value.
 ******************************************************************************/
uint16_t sli_cpc_get_crc_sw(const void* buffer, uint16_t buffer_length);

/***************************************************************************//**
 * Computes CRC-16 CCITT on given buffer. Software implementation.
 *
 * @param buffer         Pointer to the buffer on which the CRC must be computed.
 * @param buffer_length  Length of the buffer, in bytes.
 * @param security_tag   Pointer to the buffer containing the security tag.
 * @param security_tag_length  Length of the security tag buffer, in bytes.
 *
 * @return CRC value.
 ******************************************************************************/
uint16_t sli_cpc_get_crc_sw_with_security(const void *buffer, uint16_t buffer_length,
                                          const void *security_tag, uint16_t security_tag_length);

/***************************************************************************//**
 * Validates CRC-16 CCITT on given buffer. Software implementation.
 *
 * @param buffer Pointer to the buffer on which the CRC must be computed.
 * @param buffer_length Length of the buffer, in bytes.
 * @param expected_crc Expected CRC value.
 *
 * @return true if CRC matches. False otherwise.
 ******************************************************************************/
bool sli_cpc_validate_crc_sw(const void* buffer, uint16_t buffer_length, uint16_t expected_crc);

#ifdef __cplusplus
}
#endif

#endif // SLI_CPC_CRC_H
