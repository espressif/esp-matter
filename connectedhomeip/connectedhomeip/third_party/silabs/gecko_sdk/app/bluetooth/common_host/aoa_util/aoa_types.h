/***************************************************************************//**
 * @file
 * @brief AoA type definitions.
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

#ifndef AOA_TYPES_H
#define AOA_TYPES_H

#include <stdint.h>

#define AOA_ID_MAX_SIZE    64

typedef char aoa_id_t[AOA_ID_MAX_SIZE];

typedef struct aoa_iq_report_s {
  uint8_t channel;
  int8_t rssi;
  uint16_t event_counter;
  uint8_t length;
  int8_t *samples;
} aoa_iq_report_t;

typedef struct aoa_angle_s {
  float azimuth;
  float azimuth_stdev;
  float elevation;
  float elevation_stdev;
  float distance;
  float distance_stdev;
  int32_t sequence;
} aoa_angle_t;

typedef struct aoa_position_s {
  float x;
  float x_stdev;
  float y;
  float y_stdev;
  float z;
  float z_stdev;
  int32_t sequence;
} aoa_position_t;

#endif // AOA_TYPES_H
