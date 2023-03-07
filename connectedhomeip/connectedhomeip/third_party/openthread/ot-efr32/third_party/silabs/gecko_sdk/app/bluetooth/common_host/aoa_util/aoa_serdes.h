/***************************************************************************//**
 * @file
 * @brief AoA serializer and deserializer functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef AOA_SERDES_H
#define AOA_SERDES_H

#include "sl_status.h"
#include "aoa_types.h"

/***************************************************************************//**
 * Serialize IQ report data structure into string.
 *
 * @param[in] iq_report IQ report data structure.
 * @param[out] str Pointer to the string buffer, should be freed after use.
 ******************************************************************************/
sl_status_t aoa_serialize_iq_report(aoa_iq_report_t *iq_report, char** str);

/***************************************************************************//**
 * Deserialize IQ report data structure from string.
 *
 * @param[in] str String buffer.
 * @param[out] iq_report IQ report data structure.
 ******************************************************************************/
sl_status_t aoa_deserialize_iq_report(char* str, aoa_iq_report_t *iq_report);

/***************************************************************************//**
 * Serialize angle data structure into string.
 *
 * @param[in] angle Angle data structure.
 * @param[out] str Pointer to the string buffer, should be freed after use.
 ******************************************************************************/
sl_status_t aoa_serialize_angle(aoa_angle_t *angle, char** str);

/***************************************************************************//**
 * Deserialize angle data structure from string.
 *
 * @param[in] str String buffer.
 * @param[out] angle Angle data structure.
 ******************************************************************************/
sl_status_t aoa_deserialize_angle(char* str, aoa_angle_t *angle);

/***************************************************************************//**
 * Serialize position data structure into string.
 *
 * @param[in] position Position data structure.
 * @param[out] str Pointer to the string buffer, should be freed after use.
 ******************************************************************************/
sl_status_t aoa_serialize_position(aoa_position_t *position, char** str);

/***************************************************************************//**
 * Deserialize position data structure from string.
 *
 * @param[in] str String buffer.
 * @param[out] position Position data structure.
 ******************************************************************************/
sl_status_t aoa_deserialize_position(char* str, aoa_position_t *position);

#endif // AOA_SERDES_H
