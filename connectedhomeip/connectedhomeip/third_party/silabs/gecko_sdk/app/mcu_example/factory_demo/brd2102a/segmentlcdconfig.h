/***************************************************************************//**
 * @file
 * @brief Segment LCD Config
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef __SEGMENTLCDCONFIG_H
#define __SEGMENTLCDCONFIG_H

#include "em_lcd.h"

#ifdef __cplusplus
extern "C" {
#endif

// Define the LCD module type
#define LCD_MODULE_CE322_1001

/** Range of symbols available on display */
typedef enum {
  LCD_SYMBOL_GECKO,
  LCD_SYMBOL_EFM32,
  LCD_SYMBOL_COL1,
  LCD_SYMBOL_COL2,
  LCD_SYMBOL_DEGC,
  LCD_SYMBOL_DEGF,
  LCD_SYMBOL_C1,
  LCD_SYMBOL_C2,
  LCD_SYMBOL_C3,
  LCD_SYMBOL_C4,
  LCD_SYMBOL_C5,
  LCD_SYMBOL_C6,
  LCD_SYMBOL_C7,
  LCD_SYMBOL_C8,
  LCD_SYMBOL_C9,
  LCD_SYMBOL_C10,
  LCD_SYMBOL_C11,
  LCD_SYMBOL_C12,
  LCD_SYMBOL_C13,
  LCD_SYMBOL_C14,
  LCD_SYMBOL_C15,
  LCD_SYMBOL_C16,
  LCD_SYMBOL_C17,
  LCD_SYMBOL_C18,
  LCD_SYMBOL_C19,
  LCD_SYMBOL_S2,
  LCD_SYMBOL_S3,
  LCD_SYMBOL_S4,
  LCD_SYMBOL_S5,
  LCD_SYMBOL_S6,
  LCD_SYMBOL_S7,
  LCD_SYMBOL_S8,
  LCD_SYMBOL_S9,
  LCD_SYMBOL_S10,
  LCD_SYMBOL_S11,
  LCD_SYMBOL_S12,
  LCD_SYMBOL_S13,
  LCD_SYMBOL_S14,
} lcdSymbol;

#define LCD_SYMBOL_GECKO_COM    3
#define LCD_SYMBOL_GECKO_SEG    32
#define LCD_SYMBOL_EFM32_COM    0
#define LCD_SYMBOL_EFM32_SEG    10
#define LCD_SYMBOL_COL1_COM     2
#define LCD_SYMBOL_COL1_SEG     9
#define LCD_SYMBOL_COL2_COM     2
#define LCD_SYMBOL_COL2_SEG     4
#define LCD_SYMBOL_DEGC_COM     1
#define LCD_SYMBOL_DEGC_SEG     33
#define LCD_SYMBOL_DEGF_COM     3
#define LCD_SYMBOL_DEGF_SEG     33
#define LCD_SYMBOL_C1_COM       0
#define LCD_SYMBOL_C1_SEG       0
#define LCD_SYMBOL_C2_COM       0
#define LCD_SYMBOL_C2_SEG       1
#define LCD_SYMBOL_C3_COM       0
#define LCD_SYMBOL_C3_SEG       2
#define LCD_SYMBOL_C4_COM       0
#define LCD_SYMBOL_C4_SEG       3
#define LCD_SYMBOL_C5_COM       0
#define LCD_SYMBOL_C5_SEG       4
#define LCD_SYMBOL_C6_COM       0
#define LCD_SYMBOL_C6_SEG       5
#define LCD_SYMBOL_C7_COM       0
#define LCD_SYMBOL_C7_SEG       6
#define LCD_SYMBOL_C8_COM       0
#define LCD_SYMBOL_C8_SEG       7
#define LCD_SYMBOL_C9_COM       3
#define LCD_SYMBOL_C9_SEG       17
#define LCD_SYMBOL_C10_COM      3
#define LCD_SYMBOL_C10_SEG      24
#define LCD_SYMBOL_C11_COM      3
#define LCD_SYMBOL_C11_SEG      25
#define LCD_SYMBOL_C12_COM      3
#define LCD_SYMBOL_C12_SEG      26
#define LCD_SYMBOL_C13_COM      3
#define LCD_SYMBOL_C13_SEG      27
#define LCD_SYMBOL_C14_COM      3
#define LCD_SYMBOL_C14_SEG      30
#define LCD_SYMBOL_C15_COM      3
#define LCD_SYMBOL_C15_SEG      31
#define LCD_SYMBOL_C16_COM      1
#define LCD_SYMBOL_C16_SEG      9
#define LCD_SYMBOL_C17_COM      1
#define LCD_SYMBOL_C17_SEG      1
#define LCD_SYMBOL_C18_COM      1
#define LCD_SYMBOL_C18_SEG      4
#define LCD_SYMBOL_C19_COM      1
#define LCD_SYMBOL_C19_SEG      31
#define LCD_SYMBOL_S2_COM       3
#define LCD_SYMBOL_S2_SEG       0
#define LCD_SYMBOL_S3_COM       3
#define LCD_SYMBOL_S3_SEG       1
#define LCD_SYMBOL_S4_COM       3
#define LCD_SYMBOL_S4_SEG       2
#define LCD_SYMBOL_S5_COM       3
#define LCD_SYMBOL_S5_SEG       3
#define LCD_SYMBOL_S6_COM       3
#define LCD_SYMBOL_S6_SEG       4
#define LCD_SYMBOL_S7_COM       3
#define LCD_SYMBOL_S7_SEG       5
#define LCD_SYMBOL_S8_COM       3
#define LCD_SYMBOL_S8_SEG       6
#define LCD_SYMBOL_S9_COM       3
#define LCD_SYMBOL_S9_SEG       7
#define LCD_SYMBOL_S10_COM      3
#define LCD_SYMBOL_S10_SEG      32
#define LCD_SYMBOL_S11_COM      0
#define LCD_SYMBOL_S11_SEG      33
#define LCD_SYMBOL_S12_COM      2
#define LCD_SYMBOL_S12_SEG      33
#define LCD_SYMBOL_S13_COM      1
#define LCD_SYMBOL_S13_SEG      33
#define LCD_SYMBOL_S14_COM      3
#define LCD_SYMBOL_S14_SEG      33

// LCD controller frequency
// LFACLK / 8 / 8 = 512
// With oktaplex mode, 512 / 16 = 32 Hz Frame Rate
#define LCD_CMU_CLK_PRE         cmuClkDiv_8
#define LCD_CMU_CLK_DIV         cmuClkDiv_8

#define LCD_BOOST_CONTRAST      0x1D

#define LCD_INIT_DEF                \
  {                                 \
    true,                           \
    lcdMuxOctaplex,                 \
    lcdBiasOneFourth,               \
    lcdWaveLowPower,                \
    lcdModeNoExtCap,                \
    lcdChargeRedistributionDisable, \
    LCD_DEFAULT_FRAME_RATE_DIV,     \
    LCD_DEFAULT_CONTRAST            \
  }

#define LCD_NUMBER_OFF()                           \
  do {                                             \
    LCD_SegmentSetLow(0, 0xcf020a00, 0x00000000);  \
    LCD_SegmentSetLow(1, 0x4f020ced, 0x00000000);  \
    LCD_SegmentSetLow(2, 0xcf020cef, 0x00000000);  \
    LCD_SegmentSetHigh(0, 0x00000001, 0x00000000); \
    LCD_SegmentSetHigh(1, 0x00000001, 0x00000000); \
    LCD_SegmentSetHigh(2, 0x00000001, 0x00000000); \
  } while (0)

#define LCD_ALPHA_NUMBER_OFF()                     \
  do {                                             \
    LCD_SegmentSetLow(4, 0xcf0fffff, 0x00000000);  \
    LCD_SegmentSetLow(5, 0xcf0fffff, 0x00000000);  \
    LCD_SegmentSetLow(6, 0xcf0fffff, 0x00000000);  \
    LCD_SegmentSetLow(7, 0xcf0fffff, 0x00000000);  \
    LCD_SegmentSetHigh(4, 0x00000003, 0x00000000); \
    LCD_SegmentSetHigh(5, 0x00000003, 0x00000000); \
    LCD_SegmentSetHigh(6, 0x00000003, 0x00000000); \
    LCD_SegmentSetHigh(7, 0x00000003, 0x00000000); \
  } while (0)

#define LCD_ALL_SEGMENTS_OFF()                     \
  do {                                             \
    LCD_SegmentSetLow(0, 0xCF0FFFFF, 0x00000000);  \
    LCD_SegmentSetLow(1, 0xCF0FFFFF, 0x00000000);  \
    LCD_SegmentSetLow(2, 0xCF0FFFFF, 0x00000000);  \
    LCD_SegmentSetLow(3, 0xCF0FFFFF, 0x00000000);  \
    LCD_SegmentSetLow(4, 0xCF0FFFFF, 0x00000000);  \
    LCD_SegmentSetLow(5, 0xCF0FFFFF, 0x00000000);  \
    LCD_SegmentSetLow(6, 0xCF0FFFFF, 0x00000000);  \
    LCD_SegmentSetLow(7, 0xCF0FFFFF, 0x00000000);  \
    LCD_SegmentSetHigh(0, 0x00000003, 0x00000000); \
    LCD_SegmentSetHigh(1, 0x00000003, 0x00000000); \
    LCD_SegmentSetHigh(2, 0x00000003, 0x00000000); \
    LCD_SegmentSetHigh(3, 0x00000003, 0x00000000); \
    LCD_SegmentSetHigh(4, 0x00000003, 0x00000000); \
    LCD_SegmentSetHigh(5, 0x00000003, 0x00000000); \
    LCD_SegmentSetHigh(6, 0x00000003, 0x00000000); \
    LCD_SegmentSetHigh(7, 0x00000003, 0x00000000); \
  } while (0)

#define LCD_ALL_SEGMENTS_ON()                      \
  do {                                             \
    LCD_SegmentSetLow(0, 0xCF0FFFFF, 0xFFFFFFFF);  \
    LCD_SegmentSetLow(1, 0xCF0FFFFF, 0xFFFFFFFF);  \
    LCD_SegmentSetLow(2, 0xCF0FFFFF, 0xFFFFFFFF);  \
    LCD_SegmentSetLow(3, 0xCF0FFFFF, 0xFFFFFFFF);  \
    LCD_SegmentSetLow(4, 0xCF0FFFFF, 0xFFFFFFFF);  \
    LCD_SegmentSetLow(5, 0xCF0FFFFF, 0xFFFFFFFF);  \
    LCD_SegmentSetLow(6, 0xCF0FFFFF, 0xFFFFFFFF);  \
    LCD_SegmentSetLow(7, 0xCF0FFFFF, 0xFFFFFFFF);  \
    LCD_SegmentSetHigh(0, 0x00000003, 0xFFFFFFFF); \
    LCD_SegmentSetHigh(1, 0x00000003, 0xFFFFFFFF); \
    LCD_SegmentSetHigh(2, 0x00000003, 0xFFFFFFFF); \
    LCD_SegmentSetHigh(3, 0x00000003, 0xFFFFFFFF); \
    LCD_SegmentSetHigh(4, 0x00000003, 0xFFFFFFFF); \
    LCD_SegmentSetHigh(5, 0x00000003, 0xFFFFFFFF); \
    LCD_SegmentSetHigh(6, 0x00000003, 0xFFFFFFFF); \
    LCD_SegmentSetHigh(7, 0x00000003, 0xFFFFFFFF); \
  } while (0)

#define LCD_SEGMENTS_ENABLE() \
  do {                        \
    LCD->SEGEN = 0xcf0fffff;  \
    LCD->SEGEN2 = 0x00000003; \
  } while (0)

#define LCD_DISPLAY_ENABLE() \
  do {                       \
    ;                        \
  } while (0)

#define EFM_DISPLAY_DEF {                                         \
    .Text        = {                                              \
      { /* 7 */                                                   \
        .com[0] = 4, .com[1] = 5, .com[2] = 7, .com[3] = 7,       \
        .bit[0] = 19, .bit[1] = 17, .bit[2] = 17, .bit[3] = 19,   \
        .com[4] = 7, .com[5] = 4, .com[6] = 5, .com[7] = 5,       \
        .bit[4] = 8, .bit[5] = 8, .bit[6] = 8, .bit[7] = 19,      \
        .com[8] = 4, .com[9] = 5, .com[10] = 6, .com[11] = 7,     \
        .bit[8] = 18, .bit[9] = 18, .bit[10] = 18, .bit[11] = 18, \
        .com[12] = 6, .com[13] = 6,                               \
        .bit[12] = 19, .bit[13] = 8                               \
      },                                                          \
      { /* 8 */                                                   \
        .com[0] = 4, .com[1] = 4, .com[2] = 7, .com[3] = 7,       \
        .bit[0] = 15, .bit[1] = 14, .bit[2] = 14, .bit[3] = 15,   \
        .com[4] = 6, .com[5] = 4, .com[6] = 6, .com[7] = 5,       \
        .bit[4] = 17, .bit[5] = 17, .bit[6] = 16, .bit[7] = 16,   \
        .com[8] = 4, .com[9] = 5, .com[10] = 5, .com[11] = 6,     \
        .bit[8] = 16, .bit[9] = 15, .bit[10] = 14, .bit[11] = 14, \
        .com[12] = 6, .com[13] = 7,                               \
        .bit[12] = 15, .bit[13] = 16                              \
      },                                                          \
      { /* 9 */                                                   \
        .com[0] = 4, .com[1] = 5, .com[2] = 7, .com[3] = 7,       \
        .bit[0] = 13, .bit[1] = 25, .bit[2] = 25, .bit[3] = 13,   \
        .com[4] = 7, .com[5] = 4, .com[6] = 5, .com[7] = 5,       \
        .bit[4] = 24, .bit[5] = 24, .bit[6] = 24, .bit[7] = 13,   \
        .com[8] = 4, .com[9] = 5, .com[10] = 6, .com[11] = 7,     \
        .bit[8] = 12, .bit[9] = 12, .bit[10] = 12, .bit[11] = 12, \
        .com[12] = 6, .com[13] = 6,                               \
        .bit[12] = 13, .bit[13] = 24                              \
      },                                                          \
      { /* 10 */                                                  \
        .com[0] = 4, .com[1] = 4, .com[2] = 7, .com[3] = 7,       \
        .bit[0] = 10, .bit[1] = 9, .bit[2] = 9, .bit[3] = 10,     \
        .com[4] = 6, .com[5] = 4, .com[6] = 6, .com[7] = 5,       \
        .bit[4] = 25, .bit[5] = 25, .bit[6] = 11, .bit[7] = 11,   \
        .com[8] = 4, .com[9] = 5, .com[10] = 5, .com[11] = 6,     \
        .bit[8] = 11, .bit[9] = 10, .bit[10] = 9, .bit[11] = 9,   \
        .com[12] = 6, .com[13] = 7,                               \
        .bit[12] = 10, .bit[13] = 11                              \
      },                                                          \
      { /* 11 */                                                  \
        .com[0] = 4, .com[1] = 5, .com[2] = 7, .com[3] = 7,       \
        .bit[0] = 0, .bit[1] = 27, .bit[2] = 27, .bit[3] = 0,     \
        .com[4] = 7, .com[5] = 4, .com[6] = 5, .com[7] = 5,       \
        .bit[4] = 26, .bit[5] = 26, .bit[6] = 26, .bit[7] = 0,    \
        .com[8] = 4, .com[9] = 5, .com[10] = 6, .com[11] = 7,     \
        .bit[8] = 1, .bit[9] = 1, .bit[10] = 1, .bit[11] = 1,     \
        .com[12] = 6, .com[13] = 6,                               \
        .bit[12] = 0, .bit[13] = 26                               \
      },                                                          \
      { /* 12 */                                                  \
        .com[0] = 4, .com[1] = 4, .com[2] = 7, .com[3] = 7,       \
        .bit[0] = 3, .bit[1] = 4, .bit[2] = 4, .bit[3] = 3,       \
        .com[4] = 6, .com[5] = 4, .com[6] = 6, .com[7] = 5,       \
        .bit[4] = 27, .bit[5] = 27, .bit[6] = 2, .bit[7] = 2,     \
        .com[8] = 4, .com[9] = 5, .com[10] = 5, .com[11] = 6,     \
        .bit[8] = 2, .bit[9] = 3, .bit[10] = 4, .bit[11] = 4,     \
        .com[12] = 6, .com[13] = 7,                               \
        .bit[12] = 3, .bit[13] = 2                                \
      },                                                          \
      { /* 13 */                                                  \
        .com[0] = 4, .com[1] = 5, .com[2] = 7, .com[3] = 7,       \
        .bit[0] = 5, .bit[1] = 31, .bit[2] = 31, .bit[3] = 5,     \
        .com[4] = 7, .com[5] = 4, .com[6] = 5, .com[7] = 5,       \
        .bit[4] = 30, .bit[5] = 30, .bit[6] = 30, .bit[7] = 5,    \
        .com[8] = 4, .com[9] = 5, .com[10] = 6, .com[11] = 7,     \
        .bit[8] = 6, .bit[9] = 6, .bit[10] = 6, .bit[11] = 6,     \
        .com[12] = 6, .com[13] = 6,                               \
        .bit[12] = 5, .bit[13] = 30                               \
      },                                                          \
      { /* 14 */                                                  \
        .com[0] = 4, .com[1] = 4, .com[2] = 7, .com[3] = 7,       \
        .bit[0] = 32, .bit[1] = 33, .bit[2] = 33, .bit[3] = 32,   \
        .com[4] = 6, .com[5] = 4, .com[6] = 6, .com[7] = 5,       \
        .bit[4] = 31, .bit[5] = 31, .bit[6] = 7, .bit[7] = 7,     \
        .com[8] = 4, .com[9] = 5, .com[10] = 5, .com[11] = 6,     \
        .bit[8] = 7, .bit[9] = 32, .bit[10] = 33, .bit[11] = 33,  \
        .com[12] = 6, .com[13] = 7,                               \
        .bit[12] = 32, .bit[13] = 7                               \
      },                                                          \
    },                                                            \
    .Number      = {                                              \
      { /* 6 */                                                   \
        .com[0] = 2, .com[1] = 0, .com[2] = 2, .com[3] = 1,       \
        .bit[0] = 7, .bit[1] = 32, .bit[2] = 32, .bit[3] = 32,    \
        .com[4] = 2, .com[5] = 0, .com[6] = 1,                    \
        .bit[4] = 31, .bit[5] = 31, .bit[6] = 7,                  \
      },                                                          \
      { /* 5 */                                                   \
        .com[0] = 2, .com[1] = 1, .com[2] = 2, .com[3] = 1,       \
        .bit[0] = 5, .bit[1] = 5, .bit[2] = 6, .bit[3] = 6,       \
        .com[4] = 1, .com[5] = 0, .com[6] = 2,                    \
        .bit[4] = 30, .bit[5] = 30, .bit[6] = 30,                 \
      },                                                          \
      { /* 4 */                                                   \
        .com[0] = 2, .com[1] = 1, .com[2] = 2, .com[3] = 1,       \
        .bit[0] = 2, .bit[1] = 2, .bit[2] = 3, .bit[3] = 3,       \
        .com[4] = 1, .com[5] = 0, .com[6] = 2,                    \
        .bit[4] = 27, .bit[5] = 27, .bit[6] = 27,                 \
      },                                                          \
      { /* 3 */                                                   \
        .com[0] = 0, .com[1] = 2, .com[2] = 2, .com[3] = 1,       \
        .bit[0] = 26, .bit[1] = 0, .bit[2] = 1, .bit[3] = 0,      \
        .com[4] = 1, .com[5] = 0, .com[6] = 2,                    \
        .bit[4] = 26, .bit[5] = 9, .bit[6] = 26,                  \
      },                                                          \
      { /* 2 */                                                   \
        .com[0] = 0, .com[1] = 1, .com[2] = 2, .com[3] = 1,       \
        .bit[0] = 11, .bit[1] = 11, .bit[2] = 10, .bit[3] = 10,   \
        .com[4] = 1, .com[5] = 0, .com[6] = 2,                    \
        .bit[4] = 25, .bit[5] = 25, .bit[6] = 25,                 \
      },                                                          \
      { /* 1 */                                                   \
        .com[0] = 2, .com[1] = 0, .com[2] = 2, .com[3] = 1,       \
        .bit[0] = 11, .bit[1] = 24, .bit[2] = 24, .bit[3] = 24,   \
        .com[4] = 1, .com[5] = 0, .com[6] = 2,                    \
        .bit[4] = 17, .bit[5] = 17, .bit[6] = 17,                 \
      }                                                           \
    },                                                            \
    .Array = {                                                    \
      .com[0] = 0, .com[1] = 2, .com[2] = 1, .com[3] = 3,         \
      .bit[0] = 8, .bit[1] = 8, .bit[2] = 8, .bit[3] = 8,         \
      .com[4] = 3, .com[5] = 3, .com[6] = 3, .com[7] = 3,         \
      .bit[4] = 9, .bit[5] = 10, .bit[6] = 11, .bit[7] = 19,      \
      .com[8] = 3, .com[9] = 3, .com[10] = 3, .com[11] = 3,       \
      .bit[8] = 18, .bit[9] = 16, .bit[10] = 15, .bit[11] = 14,   \
      .com[12] = 3, .com[13] = 3, .com[14] = 0, .com[15] = 0,     \
      .bit[12] = 13, .bit[13] = 12, .bit[14] = 19, .bit[15] = 18, \
      .com[16] = 0, .com[17] = 0, .com[18] = 0, .com[19] = 0,     \
      .bit[16] = 16, .bit[17] = 15, .bit[18] = 14, .bit[19] = 13, \
      .com[20] = 0, .com[21] = 2, .com[22] = 2, .com[23] = 2,     \
      .bit[20] = 12, .bit[21] = 19, .bit[22] = 18, .bit[23] = 16, \
      .com[24] = 2, .com[25] = 2, .com[26] = 2, .com[27] = 2,     \
      .bit[24] = 15, .bit[25] = 14, .bit[26] = 13, .bit[27] = 12, \
      .com[28] = 1, .com[29] = 1, .com[30] = 1, .com[31] = 1,     \
      .bit[28] = 19, .bit[29] = 18, .bit[30] = 16, .bit[31] = 15, \
      .com[32] = 1, .com[33] = 1, .com[34] = 1,                   \
      .bit[32] = 14, .bit[33] = 13, .bit[34] = 12,                \
    }                                                             \
}

#ifdef __cplusplus
}
#endif

#endif
