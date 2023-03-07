/***************************************************************************//**
 * @file
 * @brief Segment LCD Config for the EFM32TG_STK3300 starter kit
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

/** Range of symbols available on display */
typedef enum {
  LCD_SYMBOL_GECKO,
  LCD_SYMBOL_ANT,
  LCD_SYMBOL_PAD0,
  LCD_SYMBOL_PAD1,
  LCD_SYMBOL_EFM32,
  LCD_SYMBOL_MINUS,
  LCD_SYMBOL_COL3,
  LCD_SYMBOL_COL5,
  LCD_SYMBOL_COL10,
  LCD_SYMBOL_DEGC,
  LCD_SYMBOL_DEGF,
  LCD_SYMBOL_DP2,
  LCD_SYMBOL_DP3,
  LCD_SYMBOL_DP4,
  LCD_SYMBOL_DP5,
  LCD_SYMBOL_DP6,
  LCD_SYMBOL_DP10,
} lcdSymbol;

#define LCD_SYMBOL_GECKO_COM  1
#define LCD_SYMBOL_GECKO_SEG  0
#define LCD_SYMBOL_ANT_COM  0
#define LCD_SYMBOL_ANT_SEG  12
#define LCD_SYMBOL_PAD0_COM  3
#define LCD_SYMBOL_PAD0_SEG  19
#define LCD_SYMBOL_PAD1_COM  2
#define LCD_SYMBOL_PAD1_SEG  0
#define LCD_SYMBOL_EFM32_COM  0
#define LCD_SYMBOL_EFM32_SEG  8
#define LCD_SYMBOL_MINUS_COM  3
#define LCD_SYMBOL_MINUS_SEG  0
#define LCD_SYMBOL_COL3_COM  4
#define LCD_SYMBOL_COL3_SEG  0
#define LCD_SYMBOL_COL5_COM  0
#define LCD_SYMBOL_COL5_SEG  10
#define LCD_SYMBOL_COL10_COM  5
#define LCD_SYMBOL_COL10_SEG  19
#define LCD_SYMBOL_DEGC_COM  0
#define LCD_SYMBOL_DEGC_SEG  14
#define LCD_SYMBOL_DEGF_COM  0
#define LCD_SYMBOL_DEGF_SEG  15
#define LCD_SYMBOL_DP2_COM  7
#define LCD_SYMBOL_DP2_SEG  0
#define LCD_SYMBOL_DP3_COM  5
#define LCD_SYMBOL_DP3_SEG  0
#define LCD_SYMBOL_DP4_COM  6
#define LCD_SYMBOL_DP4_SEG  0
#define LCD_SYMBOL_DP5_COM  7
#define LCD_SYMBOL_DP5_SEG  9
#define LCD_SYMBOL_DP6_COM  7
#define LCD_SYMBOL_DP6_SEG  11
#define LCD_SYMBOL_DP10_COM  4
#define LCD_SYMBOL_DP10_SEG  19

#define LCD_STK3300             TRUE
#define LCD_DISPLAY_TYPE        8x20
#define LCD_BOOST_LEVEL         lcdVBoostLevel3

/* LCD Controller Prescaler (divide LFACLK / 64) */
/* LFACLK_LCDpre = 512 Hz */
/* Set FDIV=0, means 512/1 = 512 Hz */
/* With octaplex mode, 512/16 => 32 Hz Frame Rate */
#define LCD_CMU_CLK_PRE         cmuClkDiv_64
#define LCD_CMU_CLK_DIV         cmuClkDiv_1

#define LCD_INIT_DEF  \
  { true,             \
    lcdMuxOctaplex,   \
    lcdBiasOneFourth, \
    lcdWaveLowPower,  \
    lcdVLCDSelVDD,    \
    lcdConConfVLCD }

#define LCD_NUMBER_OFF()                          \
  do {                                            \
    LCD_SegmentSetLow(0, 0x00078000, 0x00000000); \
    LCD_SegmentSetLow(1, 0x00078000, 0x00000000); \
    LCD_SegmentSetLow(2, 0x00078000, 0x00000000); \
    LCD_SegmentSetLow(3, 0x00078000, 0x00000000); \
    LCD_SegmentSetLow(4, 0x00078000, 0x00000000); \
    LCD_SegmentSetLow(5, 0x00078000, 0x00000000); \
    LCD_SegmentSetLow(6, 0x00078000, 0x00000000); \
    LCD_SegmentSetLow(7, 0x00078000, 0x00000000); \
  } while (0)

#define LCD_ALPHA_NUMBER_OFF()                    \
  do {                                            \
    LCD_SegmentSetLow(7, 0x000075FE, 0x00000000); \
    LCD_SegmentSetLow(6, 0x00007FFE, 0x00000000); \
    LCD_SegmentSetLow(5, 0x00007FFE, 0x00000000); \
    LCD_SegmentSetLow(4, 0x00007FFE, 0x00000000); \
    LCD_SegmentSetLow(3, 0x00007FFE, 0x00000000); \
    LCD_SegmentSetLow(2, 0x00007FFE, 0x00000000); \
    LCD_SegmentSetLow(1, 0x00007FFE, 0x00000000); \
    LCD_SegmentSetLow(0, 0x00000A00, 0x00000000); \
  } while (0)

#define LCD_ALL_SEGMENTS_OFF()                    \
  do {                                            \
    LCD_SegmentSetLow(0, 0xFFFFFFFF, 0x00000000); \
    LCD_SegmentSetLow(1, 0xFFFFFFFF, 0x00000000); \
    LCD_SegmentSetLow(2, 0xFFFFFFFF, 0x00000000); \
    LCD_SegmentSetLow(3, 0xFFFFFFFF, 0x00000000); \
    LCD_SegmentSetLow(4, 0xFFFFFFFF, 0x00000000); \
    LCD_SegmentSetLow(5, 0xFFFFFFFF, 0x00000000); \
    LCD_SegmentSetLow(6, 0xFFFFFFFF, 0x00000000); \
    LCD_SegmentSetLow(7, 0xFFFFFFFF, 0x00000000); \
  } while (0)

#define LCD_ALL_SEGMENTS_ON()                     \
  do {                                            \
    LCD_SegmentSetLow(0, 0xFFFFFFFF, 0xFFFFFFFF); \
    LCD_SegmentSetLow(1, 0xFFFFFFFF, 0xFFFFFFFF); \
    LCD_SegmentSetLow(2, 0xFFFFFFFF, 0xFFFFFFFF); \
    LCD_SegmentSetLow(3, 0xFFFFFFFF, 0xFFFFFFFF); \
    LCD_SegmentSetLow(4, 0xFFFFFFFF, 0xFFFFFFFF); \
    LCD_SegmentSetLow(5, 0xFFFFFFFF, 0xFFFFFFFF); \
    LCD_SegmentSetLow(6, 0xFFFFFFFF, 0xFFFFFFFF); \
    LCD_SegmentSetLow(7, 0xFFFFFFFF, 0xFFFFFFFF); \
  } while (0)

#define LCD_SEGMENTS_ENABLE()                    \
  do {                                           \
    LCD_SegmentRangeEnable(lcdSegmentAll, true); \
  } while (0)

#define LCD_DISPLAY_ENABLE() \
  do {                       \
    ;                        \
  } while (0)

#define EFM_DISPLAY_DEF {                                         \
    .Text        = {                                              \
      { /* 1 */                                                   \
        .com[0] = 1, .com[1] = 1, .com[2] = 5, .com[3] = 7,       \
        .bit[0] = 1, .bit[1] = 2, .bit[2] = 2, .bit[3] = 2,       \
        .com[4] = 7, .com[5] = 3, .com[6] = 4, .com[7] = 2,       \
        .bit[4] = 1, .bit[5] = 1, .bit[6] = 1, .bit[7] = 1,       \
        .com[8] = 3, .com[9] = 2, .com[10] = 4, .com[11] = 6,     \
        .bit[8] = 2, .bit[9] = 2, .bit[10] = 2, .bit[11] = 2,     \
        .com[12] = 5, .com[13] = 6,                               \
        .bit[12] = 1, .bit[13] = 1                                \
      },                                                          \
      { /* 2 */                                                   \
        .com[0] = 1, .com[1] = 1, .com[2] = 5, .com[3] = 7,       \
        .bit[0] = 3, .bit[1] = 4, .bit[2] = 4, .bit[3] = 4,       \
        .com[4] = 7, .com[5] = 3, .com[6] = 4, .com[7] = 2,       \
        .bit[4] = 3, .bit[5] = 3, .bit[6] = 3, .bit[7] = 3,       \
        .com[8] = 3, .com[9] = 2, .com[10] = 4, .com[11] = 6,     \
        .bit[8] = 4, .bit[9] = 4, .bit[10] = 4, .bit[11] = 4,     \
        .com[12] = 5, .com[13] = 6,                               \
        .bit[12] = 3, .bit[13] = 3                                \
      },                                                          \
      { /* 3 */                                                   \
        .com[0] = 1, .com[1] = 1, .com[2] = 5, .com[3] = 7,       \
        .bit[0] = 5, .bit[1] = 6, .bit[2] = 6, .bit[3] = 6,       \
        .com[4] = 7, .com[5] = 3, .com[6] = 4, .com[7] = 2,       \
        .bit[4] = 5, .bit[5] = 5, .bit[6] = 5, .bit[7] = 5,       \
        .com[8] = 3, .com[9] = 2, .com[10] = 4, .com[11] = 6,     \
        .bit[8] = 6, .bit[9] = 6, .bit[10] = 6, .bit[11] = 6,     \
        .com[12] = 5, .com[13] = 6,                               \
        .bit[12] = 5, .bit[13] = 5                                \
      },                                                          \
      { /* 4 */                                                   \
        .com[0] = 1, .com[1] = 1, .com[2] = 5, .com[3] = 7,       \
        .bit[0] = 7, .bit[1] = 8, .bit[2] = 8, .bit[3] = 8,       \
        .com[4] = 7, .com[5] = 3, .com[6] = 4, .com[7] = 2,       \
        .bit[4] = 7, .bit[5] = 7, .bit[6] = 7, .bit[7] = 7,       \
        .com[8] = 3, .com[9] = 2, .com[10] = 4, .com[11] = 6,     \
        .bit[8] = 8, .bit[9] = 8, .bit[10] = 8, .bit[11] = 8,     \
        .com[12] = 5, .com[13] = 6,                               \
        .bit[12] = 7, .bit[13] = 7                                \
      },                                                          \
      { /* 5 */                                                   \
        .com[0] = 0, .com[1] = 1, .com[2] = 5, .com[3] = 7,       \
        .bit[0] = 9, .bit[1] = 10, .bit[2] = 10, .bit[3] = 10,    \
        .com[4] = 6, .com[5] = 2, .com[6] = 3, .com[7] = 1,       \
        .bit[4] = 9, .bit[5] = 9, .bit[6] = 9, .bit[7] = 9,       \
        .com[8] = 3, .com[9] = 2, .com[10] = 4, .com[11] = 6,     \
        .bit[8] = 10, .bit[9] = 10, .bit[10] = 10, .bit[11] = 10, \
        .com[12] = 4, .com[13] = 5,                               \
        .bit[12] = 9, .bit[13] = 9                                \
      },                                                          \
      { /* 6 */                                                   \
        .com[0] = 0, .com[1] = 1, .com[2] = 5, .com[3] = 7,       \
        .bit[0] = 11, .bit[1] = 12, .bit[2] = 12, .bit[3] = 12,   \
        .com[4] = 6, .com[5] = 2, .com[6] = 3, .com[7] = 1,       \
        .bit[4] = 11, .bit[5] = 11, .bit[6] = 11, .bit[7] = 11,   \
        .com[8] = 3, .com[9] = 2, .com[10] = 4, .com[11] = 6,     \
        .bit[8] = 12, .bit[9] = 12, .bit[10] = 12, .bit[11] = 12, \
        .com[12] = 4, .com[13] = 5,                               \
        .bit[12] = 11, .bit[13] = 11                              \
      },                                                          \
      { /* 7 */                                                   \
        .com[0] = 1, .com[1] = 1, .com[2] = 5, .com[3] = 7,       \
        .bit[0] = 13, .bit[1] = 14, .bit[2] = 14, .bit[3] = 14,   \
        .com[4] = 7, .com[5] = 3, .com[6] = 4, .com[7] = 2,       \
        .bit[4] = 13, .bit[5] = 13, .bit[6] = 13, .bit[7] = 13,   \
        .com[8] = 3, .com[9] = 2, .com[10] = 4, .com[11] = 6,     \
        .bit[8] = 14, .bit[9] = 14, .bit[10] = 14, .bit[11] = 14, \
        .com[12] = 5, .com[13] = 6,                               \
        .bit[12] = 13, .bit[13] = 13                              \
      },                                                          \
    },                                                            \
    .Number      = {                                              \
      {                                                           \
        .com[0] = 7, .com[1] = 5, .com[2] = 2, .com[3] = 1,       \
        .bit[0] = 15, .bit[1] = 15, .bit[2] = 15, .bit[3] = 15,   \
        .com[4] = 3, .com[5] = 6, .com[6] = 4,                    \
        .bit[4] = 15, .bit[5] = 15, .bit[6] = 15,                 \
      },                                                          \
      {                                                           \
        .com[0] = 7, .com[1] = 5, .com[2] = 2, .com[3] = 1,       \
        .bit[0] = 16, .bit[1] = 16, .bit[2] = 16, .bit[3] = 16,   \
        .com[4] = 3, .com[5] = 6, .com[6] = 4,                    \
        .bit[4] = 16, .bit[5] = 16, .bit[6] = 16,                 \
      },                                                          \
      {                                                           \
        .com[0] = 7, .com[1] = 5, .com[2] = 2, .com[3] = 1,       \
        .bit[0] = 17, .bit[1] = 17, .bit[2] = 17, .bit[3] = 17,   \
        .com[4] = 3, .com[5] = 6, .com[6] = 4,                    \
        .bit[4] = 17, .bit[5] = 17, .bit[6] = 17,                 \
      },                                                          \
      {                                                           \
        .com[0] = 7, .com[1] = 5, .com[2] = 2, .com[3] = 1,       \
        .bit[0] = 18, .bit[1] = 18, .bit[2] = 18, .bit[3] = 18,   \
        .com[4] = 3, .com[5] = 6, .com[6] = 4,                    \
        .bit[4] = 18, .bit[5] = 18, .bit[6] = 18,                 \
      },                                                          \
    },                                                            \
    .EMode       = {                                              \
      .com[0] = 0, .bit[0] = 19,                                  \
      .com[1] = 1, .bit[1] = 19,                                  \
      .com[2] = 7, .bit[2] = 19,                                  \
      .com[3] = 2, .bit[3] = 19,                                  \
      .com[4] = 6, .bit[4] = 19,                                  \
    },                                                            \
    .ARing       = {                                              \
      .com[0] = 0, .bit[0] = 7,                                   \
      .com[1] = 0, .bit[1] = 6,                                   \
      .com[2] = 0, .bit[2] = 5,                                   \
      .com[3] = 0, .bit[3] = 4,                                   \
      .com[4] = 0, .bit[4] = 3,                                   \
      .com[5] = 0, .bit[5] = 2,                                   \
      .com[6] = 0, .bit[6] = 1,                                   \
      .com[7] = 0, .bit[7] = 0,                                   \
    },                                                            \
    .Battery     = {                                              \
      .com[0] = 0, .bit[0] = 13,                                  \
      .com[1] = 0, .bit[1] = 17,                                  \
      .com[2] = 0, .bit[2] = 16,                                  \
      .com[3] = 0, .bit[3] = 18,                                  \
    },                                                            \
    .TopBlocks   = {                                              \
      { /* 1 */                                                   \
        .com[0] = 1, .com[1] = 1, .com[2] = 3, .com[3] = 4,       \
        .bit[0] = 1, .bit[1] = 2, .bit[2] = 1, .bit[3] = 1,       \
        .com[4] = 4, .com[5] = 2, .com[6] = 3, .com[7] = 2,       \
        .bit[4] = 2, .bit[5] = 1, .bit[6] = 2, .bit[7] = 2        \
      },                                                          \
      { /* 2 */                                                   \
        .com[0] = 1, .com[1] = 1, .com[2] = 3, .com[3] = 4,       \
        .bit[0] = 3, .bit[1] = 4, .bit[2] = 3, .bit[3] = 3,       \
        .com[4] = 4, .com[5] = 2, .com[6] = 3, .com[7] = 2,       \
        .bit[4] = 4, .bit[5] = 3, .bit[6] = 4, .bit[7] = 4        \
      },                                                          \
      { /* 3 */                                                   \
        .com[0] = 1, .com[1] = 1, .com[2] = 3, .com[3] = 4,       \
        .bit[0] = 5, .bit[1] = 6, .bit[2] = 5, .bit[3] = 5,       \
        .com[4] = 4, .com[5] = 2, .com[6] = 3, .com[7] = 2,       \
        .bit[4] = 6, .bit[5] = 5, .bit[6] = 6, .bit[7] = 6        \
      },                                                          \
      { /* 4 */                                                   \
        .com[0] = 1, .com[1] = 1, .com[2] = 3, .com[3] = 4,       \
        .bit[0] = 7, .bit[1] = 8, .bit[2] = 7, .bit[3] = 7,       \
        .com[4] = 4, .com[5] = 2, .com[6] = 3, .com[7] = 2,       \
        .bit[4] = 8, .bit[5] = 7, .bit[6] = 8, .bit[7] = 8        \
      },                                                          \
      { /* 5 */                                                   \
        .com[0] = 0, .com[1] = 1, .com[2] = 2, .com[3] = 3,       \
        .bit[0] = 9, .bit[1] = 10, .bit[2] = 9, .bit[3] = 9,      \
        .com[4] = 4, .com[5] = 1, .com[6] = 3, .com[7] = 2,       \
        .bit[4] = 10, .bit[5] = 9, .bit[6] = 10, .bit[7] = 10     \
      },                                                          \
      { /* 6 */                                                   \
        .com[0] = 0, .com[1] = 1, .com[2] = 2, .com[3] = 3,       \
        .bit[0] = 11, .bit[1] = 12, .bit[2] = 11, .bit[3] = 11,   \
        .com[4] = 4, .com[5] = 1, .com[6] = 3, .com[7] = 2,       \
        .bit[4] = 12, .bit[5] = 11, .bit[6] = 12, .bit[7] = 12    \
      },                                                          \
      { /* 7 */                                                   \
        .com[0] = 1, .com[1] = 1, .com[2] = 3, .com[3] = 4,       \
        .bit[0] = 13, .bit[1] = 14, .bit[2] = 13, .bit[3] = 13,   \
        .com[4] = 4, .com[5] = 2, .com[6] = 3, .com[7] = 2,       \
        .bit[4] = 14, .bit[5] = 13, .bit[6] = 14, .bit[7] = 14    \
      },                                                          \
    },                                                            \
    .BotBlocks   = {                                              \
      { /* 1 */                                                   \
        .com[0] = 5, .com[1] = 7, .com[2] = 7, .com[3] = 4,       \
        .bit[0] = 2, .bit[1] = 2, .bit[2] = 1, .bit[3] = 1,       \
        .com[4] = 4, .com[5] = 6, .com[6] = 5, .com[7] = 6,       \
        .bit[4] = 2, .bit[5] = 2, .bit[6] = 1, .bit[7] = 1        \
      },                                                          \
      { /* 2 */                                                   \
        .com[0] = 5, .com[1] = 7, .com[2] = 7, .com[3] = 4,       \
        .bit[0] = 4, .bit[1] = 4, .bit[2] = 3, .bit[3] = 3,       \
        .com[4] = 4, .com[5] = 6, .com[6] = 5, .com[7] = 6,       \
        .bit[4] = 4, .bit[5] = 4, .bit[6] = 3, .bit[7] = 3        \
      },                                                          \
      { /* 3 */                                                   \
        .com[0] = 5, .com[1] = 7, .com[2] = 7, .com[3] = 4,       \
        .bit[0] = 6, .bit[1] = 6, .bit[2] = 5, .bit[3] = 5,       \
        .com[4] = 4, .com[5] = 6, .com[6] = 5, .com[7] = 6,       \
        .bit[4] = 6, .bit[5] = 6, .bit[6] = 5, .bit[7] = 5        \
      },                                                          \
      { /* 4 */                                                   \
        .com[0] = 5, .com[1] = 7, .com[2] = 7, .com[3] = 4,       \
        .bit[0] = 8, .bit[1] = 8, .bit[2] = 7, .bit[3] = 7,       \
        .com[4] = 4, .com[5] = 6, .com[6] = 5, .com[7] = 6,       \
        .bit[4] = 8, .bit[5] = 8, .bit[6] = 7, .bit[7] = 7        \
      },                                                          \
      { /* 5 */                                                   \
        .com[0] = 5, .com[1] = 7, .com[2] = 6, .com[3] = 3,       \
        .bit[0] = 10, .bit[1] = 10, .bit[2] = 9, .bit[3] = 9,     \
        .com[4] = 4, .com[5] = 6, .com[6] = 4, .com[7] = 5,       \
        .bit[4] = 10, .bit[5] = 10, .bit[6] = 9, .bit[7] = 9      \
      },                                                          \
      { /* 6 */                                                   \
        .com[0] = 5, .com[1] = 7, .com[2] = 6, .com[3] = 3,       \
        .bit[0] = 12, .bit[1] = 12, .bit[2] = 11, .bit[3] = 11,   \
        .com[4] = 4, .com[5] = 6, .com[6] = 4, .com[7] = 5,       \
        .bit[4] = 12, .bit[5] = 12, .bit[6] = 11, .bit[7] = 11    \
      },                                                          \
      { /* 7 */                                                   \
        .com[0] = 5, .com[1] = 7, .com[2] = 7, .com[3] = 4,       \
        .bit[0] = 14, .bit[1] = 14, .bit[2] = 13, .bit[3] = 13,   \
        .com[4] = 4, .com[5] = 6, .com[6] = 5, .com[7] = 6,       \
        .bit[4] = 14, .bit[5] = 14, .bit[6] = 13, .bit[7] = 13    \
      },                                                          \
    }                                                             \
}

#ifdef __cplusplus
}
#endif

#endif
