/***************************************************************************//**
 * @file
 * @brief 4x4 URA Dual Polarized antenna board pin definitions.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef ANTENNA_ARRAY_BRD4191A_H
#define ANTENNA_ARRAY_BRD4191A_H

// CTRL5 pin (bit 4) selects one of the CHW switches when in DP mode.
#define CHW1 (0 << 4)
#define CHW2 (1 << 4)

// CTRL6 pin (bit 5) selects DP/CP mode.
#define DP_MODE (0 << 5)
#define CP_MODE (1 << 5)

// CTRL7 pin (bit 6) selects AoA/AoD mode.
#define AOA_MODE (0 << 6)
#define AOD_MODE (1 << 6)

// CTRL1..CTRL4 pins (bits 0..3) select one of the 16 outputs
// of both CHW switches simultaneously.
#define ANT_V1   (DP_MODE | CHW1 |  2)
#define ANT_H1   (DP_MODE | CHW1 |  3)
#define ANT_V2   (DP_MODE | CHW1 |  0)
#define ANT_H2   (DP_MODE | CHW1 |  1)
#define ANT_V3   (DP_MODE | CHW1 | 14)
#define ANT_H3   (DP_MODE | CHW1 | 15)
#define ANT_V4   (DP_MODE | CHW1 | 12)
#define ANT_H4   (DP_MODE | CHW1 | 13)
#define ANT_V5   (DP_MODE | CHW1 |  4)
#define ANT_H5   (DP_MODE | CHW1 |  5)
#define ANT_V6   (DP_MODE | CHW1 |  8)
#define ANT_H6   (DP_MODE | CHW2 |  8)
#define ANT_V7   (DP_MODE | CHW2 |  4)
#define ANT_H7   (DP_MODE | CHW2 |  5)
#define ANT_V8   (DP_MODE | CHW1 | 10)
#define ANT_H8   (DP_MODE | CHW1 | 11)
#define ANT_V9   (DP_MODE | CHW1 |  6)
#define ANT_H9   (DP_MODE | CHW1 |  7)
#define ANT_V10  (DP_MODE | CHW2 |  6)
#define ANT_H10  (DP_MODE | CHW2 |  7)
#define ANT_V11  (DP_MODE | CHW2 |  0)
#define ANT_H11  (DP_MODE | CHW2 |  1)
#define ANT_V12  (DP_MODE | CHW2 |  3)
#define ANT_H12  (DP_MODE | CHW2 |  2)
#define ANT_V13  (DP_MODE | CHW1 |  9)
#define ANT_H13  (DP_MODE | CHW2 |  9)
#define ANT_V14  (DP_MODE | CHW2 | 10)
#define ANT_H14  (DP_MODE | CHW2 | 11)
#define ANT_V15  (DP_MODE | CHW2 | 12)
#define ANT_H15  (DP_MODE | CHW2 | 13)
#define ANT_V16  (DP_MODE | CHW2 | 15)
#define ANT_H16  (DP_MODE | CHW2 | 14)

// The following antennas are available in CP mode.
#define ANT_6_CP   (CP_MODE | 8)
#define ANT_13_CP  (CP_MODE | 9)

// Vertically polarized antennas.
#define ANT_VERTICAL  { \
    ANT_V1,             \
    ANT_V2,             \
    ANT_V3,             \
    ANT_V4,             \
    ANT_V5,             \
    ANT_V6,             \
    ANT_V7,             \
    ANT_V8,             \
    ANT_V9,             \
    ANT_V10,            \
    ANT_V11,            \
    ANT_V12,            \
    ANT_V13,            \
    ANT_V14,            \
    ANT_V15,            \
    ANT_V16             \
}

// Horizontally polarized antennas.
#define ANT_HORIZONTAL  { \
    ANT_H1,               \
    ANT_H2,               \
    ANT_H3,               \
    ANT_H4,               \
    ANT_H5,               \
    ANT_H6,               \
    ANT_H7,               \
    ANT_H8,               \
    ANT_H9,               \
    ANT_H10,              \
    ANT_H11,              \
    ANT_H12,              \
    ANT_H13,              \
    ANT_H14,              \
    ANT_H15,              \
    ANT_H16               \
}

#endif // ANTENNA_ARRAY_BRD4191A_H
