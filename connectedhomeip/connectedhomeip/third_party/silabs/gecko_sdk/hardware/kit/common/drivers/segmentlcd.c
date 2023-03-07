/***************************************************************************//**
 * @file
 * @brief Segment LCD Display driver
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"

#include "segmentlcd.h"

/***************************************************************************//**
 * @addtogroup kitdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup SegmentLcd
 * @brief Segment LCD driver
 *
 * @details
 *  This driver implements symbol and string write support for EFM32 kits with
 *  segment LCD.
 * @{
 ******************************************************************************/

#if defined(LCD_MODULE_CE322_1001)
#define NUM_DIGITS              6
#define NUM_VAL_MAX             999999
#define NUM_VAL_MIN             -99999
#define NUM_VAL_MAX_HEX         0xFFFFFF
#define ALPHA_DIGITS            8
#define ALPHA_VAL_MAX           99999999
#define ALPHA_VAL_MIN           -9999999
#else
#define NUM_DIGITS              4
#define NUM_VAL_MAX             9999
#define NUM_VAL_MIN             -999
#define NUM_VAL_MAX_HEX         0xFFFF
#define ALPHA_DIGITS            7
#define ALPHA_VAL_MAX           9999999
#define ALPHA_VAL_MIN           -9999999
#endif

/**************************************************************************//**
 * @brief
 * Defines each text symbol's segment in terms of COM and BIT numbers,
 * in a way that we can enumerate each bit for each text segment in the
 * following bit pattern:
 * @verbatim
 *  -------0------
 *
 * |   \7  |8  /9 |
 * |5   \  |  /   |1
 *
 *  --6---  ---10--
 *
 * |    /  |  \11 |
 * |4  /13 |12 \  |2
 *
 *  -------3------
 * @endverbatim
 * E.g.: First text character bit pattern #3 (above) is
 * Segment 1D for Display
 * Location COM 3, BIT 0
 *****************************************************************************/
typedef struct {
  uint8_t com[14]; /**< LCD COM line (for multiplexing) */
  uint8_t bit[14]; /**< LCD bit number */
} CHAR_TypeDef;

/**************************************************************************//**
 * @brief Defines segment COM and BIT fields numeric display
 *****************************************************************************/
typedef struct {
  uint8_t com[7]; /**< LCD COM line (for multiplexing) */
  uint8_t bit[7]; /**< LCD bit number */
} NUMBER_TypeDef;

/**************************************************************************//**
 * @brief Defines segment COM and BIT fields for Energy Modes on display
 *****************************************************************************/
typedef struct {
  uint8_t com[5]; /**< LCD COM line (for multiplexing) */
  uint8_t bit[5]; /**< LCD bit number */
} EM_TypeDef;

/**************************************************************************//**
 * @brief Defines segment COM and BIT fields for A-wheel (suited for Anim)
 *****************************************************************************/
typedef struct {
  uint8_t com[8]; /**< LCD COM line (for multiplexing) */
  uint8_t bit[8]; /**< LCD bit number */
} ARING_TypeDef;

/**************************************************************************//**
 * @brief Defines segment COM and BIT fields for Battery (suited for Anim)
 *****************************************************************************/
typedef struct {
  uint8_t com[4]; /**< LCD COM line (for multiplexing) */
  uint8_t bit[4]; /**< LCD bit number */
} BATTERY_TypeDef;

/**************************************************************************//**
 * @brief Defines segment COM and BIT fields for array
 *****************************************************************************/
typedef struct {
  uint8_t com[35]; /**< LCD COM line (for multiplexing) */
  uint8_t bit[35]; /**< LCD bit number */
} ARRAY_TypeDef;

/**************************************************************************//**
 * @brief Defines segment COM and BIT fields for top and bottom row blocks.
 * The bit pattern shown above for characters can be split into upper and lower
 * portions for animation purposes. There are separate COM and BIT numbers
 * which together represent a set of stacked blocks which can be shown on two
 * rows in the segmented LCD screen.
 *
 * There are four blocks shown on the top row:
 * @verbatim
 *  -------0------
 *
 * |   \5  |6  /7 |
 * |2   \  |  /   |1
 *
 *  --3---  ---4--
 * @endverbatim
 *
 * There are four blocks shown on the bottom row :
 * @verbatim
 *  --3---  ---4--
 *
 * |    /  |  \5  |
 * |2  /7  |6  \  |0
 *
 *  -------1------
 * @endverbatim
 *
 * The upper row has four blocks which are placed directly above the four
 * blocks on the lower row. Each block can be in one of three states:
 * blank - outer five segments off, inner three segments off
 * outer - outer five segments on,  inner three segments off
 * inner - outer five segments off, inner three segments on
 * full  - outer five segments on,  inner three segments on
 *
 * @verbatim
 * Top row:    0 1 2 3 4 5 6 7
 * Bottom row: 0 1 2 3 4 5 6 7
 * @endverbatim
 *****************************************************************************/
typedef struct {
  uint8_t com[8]; /**< LCD COM line (for multiplexing) */
  uint8_t bit[8]; /**< LCD bit number */
} Block_TypeDef;

/**************************************************************************//**
 * @brief Defines prototype for all segments in display
 *****************************************************************************/
#if defined(_SILICON_LABS_32B_SERIES_0)
typedef struct {
  CHAR_TypeDef    Text[ALPHA_DIGITS];      /**< Text on display */
  NUMBER_TypeDef  Number[NUM_DIGITS];      /**< Numbers on display */
  EM_TypeDef      EMode;                   /**< Display energy mode */
  ARING_TypeDef   ARing;                   /**< Display ring */
  BATTERY_TypeDef Battery;                 /**< Display battery */
  Block_TypeDef   TopBlocks[ALPHA_DIGITS]; /**< Display top blocks */
  Block_TypeDef   BotBlocks[ALPHA_DIGITS]; /**< Display bottom blocks */
} MCU_DISPLAY;
#endif

#if defined(_SILICON_LABS_32B_SERIES_1)
typedef struct {
  CHAR_TypeDef    Text[ALPHA_DIGITS];      /**< Text on display */
  NUMBER_TypeDef  Number[NUM_DIGITS];      /**< Numbers on display */
  ARRAY_TypeDef   Array;                   /**< Display array */
  Block_TypeDef   TopBlocks[ALPHA_DIGITS]; /**< Display top blocks */
  Block_TypeDef   BotBlocks[ALPHA_DIGITS]; /**< Display bottom blocks */
} MCU_DISPLAY;
#endif

/**************************************************************************//**
 * @brief Working instance of LCD display
 *****************************************************************************/
static const MCU_DISPLAY EFM_Display = EFM_DISPLAY_DEF;

/**************************************************************************//**
 * @brief Working instance of LCD display
 *****************************************************************************/
static void displayBlock(
  SegmentLCD_BlockMode_TypeDef topMode[SEGMENT_LCD_NUM_BLOCK_COLUMNS],
  SegmentLCD_BlockMode_TypeDef botMode[SEGMENT_LCD_NUM_BLOCK_COLUMNS]);

/**************************************************************************//**
 * @brief
 * Defines higlighted segments for the alphabet, starting from "blank" (SPACE)
 * Uses bit pattern as defined for text segments above.
 * E.g. a capital O, would have bits 0 1 2 3 4 5 => 0x003f defined
 *****************************************************************************/
static const uint16_t Segment_Alphabet[] = {
  0x0000, /* space */
  0x1100, /* ! */
  0x0280, /* " */
  0x0000, /* # */
  0x0000, /* $ */
  0x0602, /* % */
  0x0000, /* & */
  0x0020, /* ' */
  0x0039, /* ( */
  0x000f, /* ) */
  0x0000, /* * */
  0x1540, /* + */
  0x2000, /* , */
  0x0440, /* - */
  0x1000, /* . */
  0x2200, /* / */

  0x003f, /* 0 */
  0x0006, /* 1 */
  0x045b, /* 2 */
  0x044f, /* 3 */
  0x0466, /* 4 */
  0x046d, /* 5 */
  0x047d, /* 6 */
  0x0007, /* 7 */
  0x047f, /* 8 */
  0x046f, /* 9 */

  0x0000, /* : */
  0x0000, /* ; */
  0x0a00, /* < */
  0x0000, /* = */
  0x2080, /* > */
  0x0000, /* ? */
  0xffff, /* @ */

  0x0477, /* A */
  0x0a79, /* B */
  0x0039, /* C */
  0x20b0, /* D */
  0x0079, /* E */
  0x0071, /* F */
  0x047d, /* G */
  0x0476, /* H */
  0x0006, /* I */
  0x000e, /* J */
  0x0a70, /* K */
  0x0038, /* L */
  0x02b6, /* M */
  0x08b6, /* N */
  0x003f, /* O */
  0x0473, /* P */
  0x083f, /* Q */
  0x0c73, /* R */
  0x046d, /* S */
  0x1101, /* T */
  0x003e, /* U */
  0x2230, /* V */
  0x2836, /* W */
  0x2a80, /* X */
  0x046e, /* Y */
  0x2209, /* Z */

  0x0039, /* [ */
  0x0880, /* backslash */
  0x000f, /* ] */
  0x0001, /* ^ */
  0x0008, /* _ */
  0x0100, /* ` */

  0x1058, /* a */
  0x047c, /* b */
  0x0058, /* c */
  0x045e, /* d */
  0x2058, /* e */
  0x0471, /* f */
  0x0c0c, /* g */
  0x0474, /* h */
  0x0004, /* i */
  0x000e, /* j */
  0x0c70, /* k */
  0x0038, /* l */
  0x1454, /* m */
  0x0454, /* n */
  0x045c, /* o */
  0x0473, /* p */
  0x0467, /* q */
  0x0450, /* r */
  0x0c08, /* s */
  0x0078, /* t */
  0x001c, /* u */
  0x2010, /* v */
  0x2814, /* w */
  0x2a80, /* x */
  0x080c, /* y */
  0x2048, /* z */

  0x0000,
};

/**************************************************************************//**
 * @brief
 * Defines higlighted segments for the numeric display
 *****************************************************************************/

static const uint16_t Segment_Numbers[] = {
  0x003f, /* 0 */
  0x0006, /* 1 */
  0x005b, /* 2 */
  0x004f, /* 3 */
  0x0066, /* 4 */
  0x006d, /* 5 */
  0x007d, /* 6 */
  0x0007, /* 7 */
  0x007f, /* 8 */
  0x006f, /* 9 */
  0x0077, /* A */
  0x007c, /* b */
  0x0039, /* C */
  0x005e, /* d */
  0x0079, /* E */
  0x0071, /* F */
  0x0040  /* - */
};

/**************************************************************************//**
 * @brief
 * Defines highlighted segments for the block display
 *****************************************************************************/
static const uint16_t Blocks[] = {
  0x0000, /* blank */
  0x00E0, /* inner */
  0x001F, /* outer */
  0x00FF  /* full */
};

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
/* sign is last element of the table  */
static const uint16_t signIndex = sizeof(Segment_Numbers) / sizeof(uint16_t) - 1;

static const LCD_Init_TypeDef lcdInit = LCD_INIT_DEF;

#if defined(_SILICON_LABS_32B_SERIES_1)
static bool dynamicChgRedistEnabled = true;
#endif

/** @endcond */

/**************************************************************************//**
 * @brief Disable all segments
 *****************************************************************************/
void SegmentLCD_AllOff(void)
{
  /* Turn on low segments */
  LCD_ALL_SEGMENTS_OFF();

#if defined(_SILICON_LABS_32B_SERIES_1)
  /* Switching charge redistribution OFF (if dynamic change is enabled)*/
  if (dynamicChgRedistEnabled) {
    LCD->DISPCTRL = (LCD->DISPCTRL & ~_LCD_DISPCTRL_CHGRDST_MASK)
                    | lcdChargeRedistributionDisable;
  }
#endif
}

/**************************************************************************//**
 * @brief Enable all segments
 *****************************************************************************/
void SegmentLCD_AllOn(void)
{
  LCD_ALL_SEGMENTS_ON();

#if defined(_SILICON_LABS_32B_SERIES_1)
  /* Switching charge redistribution ON (if dynamic change is enabled) */
  if (dynamicChgRedistEnabled) {
    LCD->DISPCTRL = (LCD->DISPCTRL & ~_LCD_DISPCTRL_CHGRDST_MASK)
                    | lcdChargeRedistributionEnable;
  }
#endif
}

/**************************************************************************//**
 * @brief Turn all segments on alpha characters in display off
 *****************************************************************************/
void SegmentLCD_AlphaNumberOff(void)
{
  LCD_ALPHA_NUMBER_OFF();

#if defined(_SILICON_LABS_32B_SERIES_1)
  /* Evaluating the updated display contents and switching charge
     redistribution ON or OFF accordingly (only if dynamic change is enabled) */
  if (dynamicChgRedistEnabled) {
    SegmentLCD_updateCHGRDST();
  }
#endif
}

/******************************************************************************
* @brief Display blocks on LCD display: blank, fill, outline, outline & fill
* @param topMode array of block modes for the top row with element zero
* representing the left-most column and element six representing the
* right-most column
* @param botMode array of block modes for the bottom row with element zero
* representing the left-most column and element six representing the
* right-most column
******************************************************************************/
void SegmentLCD_Block(
  SegmentLCD_BlockMode_TypeDef topMode[SEGMENT_LCD_NUM_BLOCK_COLUMNS],
  SegmentLCD_BlockMode_TypeDef botMode[SEGMENT_LCD_NUM_BLOCK_COLUMNS])
{
  /* If an update is in progress we must block, or there might be tearing */
  LCD_SyncBusyDelay(0xFFFFFFFF);

  /* Freeze LCD to avoid partial updates */
  LCD_FreezeEnable(true);

  /* Turn all segments off */
  SegmentLCD_AlphaNumberOff();

  displayBlock(topMode, botMode);

  /* Enable update */
  LCD_FreezeEnable(false);
}

/**************************************************************************//**
 * @brief Disables LCD controller
 *****************************************************************************/
void SegmentLCD_Disable(void)
{
  /* Disable LCD */
  LCD_Enable(false);

  /* Make sure CTRL register has been updated */
  LCD_SyncBusyDelay(LCD_SYNCBUSY_CTRL);

  /* Turn off LCD clock */
  CMU_ClockEnable(cmuClock_LCD, false);

#if defined(_SILICON_LABS_32B_SERIES_0)
  /* Turn off voltage boost if enabled */
  CMU->LCDCTRL = 0;
#endif
}

/**************************************************************************//**
 * @brief Segment LCD Initialization routine for EFM32 STK display
 * @param useBoost Set to use voltage boost
 *****************************************************************************/
void SegmentLCD_Init(bool useBoost)
{
  /* Ensure LE modules are accessible */
  CMU_ClockEnable(cmuClock_CORELE, true);

  /* Enable LFRCO as LFACLK in CMU (will also enable oscillator if not enabled) */
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);

  /* LCD Controller Prescaler  */
  CMU_ClockDivSet(cmuClock_LCDpre, LCD_CMU_CLK_PRE);

  /* Frame Rate */
  CMU_LCDClkFDIVSet(LCD_CMU_CLK_DIV);

  /* Enable clock to LCD module */
  CMU_ClockEnable(cmuClock_LCD, true);

  LCD_DISPLAY_ENABLE();

  /* Disable interrupts */
  LCD_IntDisable(0xFFFFFFFF);

  /* Initialize and enable LCD controller */
  LCD_Init(&lcdInit);
#if defined(_SILICON_LABS_32B_SERIES_1)
  if (lcdInit.chargeRedistribution == lcdChargeRedistributionDisable) {
    dynamicChgRedistEnabled = false;
  }
#endif

  /* Enable all display segments */
  LCD_SEGMENTS_ENABLE();

  /* Enable boost if necessary */
  if (useBoost) {
#if defined(_SILICON_LABS_32B_SERIES_0)
    LCD_VBoostSet(LCD_BOOST_LEVEL);
    LCD_VLCDSelect(lcdVLCDSelVExtBoost);
    CMU->LCDCTRL |= CMU_LCDCTRL_VBOOSTEN;
#elif defined(_SILICON_LABS_32B_SERIES_1)
    /* Set charge pump mode and adjust contrast */
    LCD_ModeSet(lcdModeCpIntOsc);
    LCD_ContrastSet(LCD_BOOST_CONTRAST);
#endif
  }

  /* Turn all segments off */
  SegmentLCD_AllOff();

  LCD_SyncBusyDelay(0xFFFFFFFF);
}

/**************************************************************************//**
 * @brief Write a hexadecimal number on lower alphanumeric part of
 *        Segment LCD display
 * @param num Hexadecimal number value to put on display, in range 0
 *        to 0x0FFFFFFF
 *****************************************************************************/
void SegmentLCD_LowerHex(uint32_t num)
{
  int       i;
  char      str[ALPHA_DIGITS + 1];
  uint32_t  nibble;

#if defined(_SILICON_LABS_32B_SERIES_0)
  SegmentLCD_Symbol(LCD_SYMBOL_MINUS, 0);
#endif

  for (i = (ALPHA_DIGITS - 1); i >= 0; i--) {
    nibble = num & 0xF;

    if ( nibble < 10 ) {
      str[i] = nibble + '0';
    } else if ( nibble == 11 ) {
      str[i] = 'b';
    } else if ( nibble == 13 ) {
      str[i] = 'd';
    } else {
      str[i] = (nibble - 10) + 'A';
    }

    num >>= 4;
  }

  SegmentLCD_Write(str);
}

/**************************************************************************//**
 * @brief Write number on lower alphanumeric part of Segment LCD display
 * @param num Numeric value to put on display, in range -9999999 to +9999999
 *****************************************************************************/
void SegmentLCD_LowerNumber(int num)
{
  int i;
  char str[ALPHA_DIGITS + 1];
  bool neg = false;
  bool val = true;

  memset(str, 0, sizeof(str));
#if defined(_SILICON_LABS_32B_SERIES_0)
  SegmentLCD_Symbol(LCD_SYMBOL_MINUS, false);
#endif

  if ((num > ALPHA_VAL_MAX) || (num < ALPHA_VAL_MIN)) {
    SegmentLCD_Write("Ovrflow");
    return;
  }

  if (num < 0) {
    num = -num;
    neg = true;
  }

  for (i = (ALPHA_DIGITS - 1); i >= 0; i--) {
    if (val) {
      str[i] = (num % 10) + '0';
    } else {
      str[i] = neg ? '-' : ' ';
      neg = false;
    }
    num /= 10;
    val = (num != 0);
  }
#if defined(_SILICON_LABS_32B_SERIES_0)
  if (neg) {
    SegmentLCD_Symbol(LCD_SYMBOL_MINUS, true);
  }
#endif

  SegmentLCD_Write(str);
}

/**************************************************************************//**
 * @brief Write number on numeric part on Segment LCD display
 * @param value Numeric value to put on display, in range -999 to +9999
 *****************************************************************************/
void SegmentLCD_Number(int value)
{
  int      i, com, bit, digit, div, neg;
  uint16_t bitpattern;
  uint16_t num;

  /* Parameter consistancy check */
  if (value >= NUM_VAL_MAX) {
    value = NUM_VAL_MAX;
  }
  if (value < NUM_VAL_MIN) {
    value = NUM_VAL_MIN;
  }
  if (value < 0) {
    value = abs(value);
    neg   = 1;
  } else {
    neg = 0;
  }

  /* If an update is in progress we must block, or there might be tearing */
  LCD_SyncBusyDelay(0xFFFFFFFF);

  /* Freeze updates to avoid partial refresh of display */
  LCD_FreezeEnable(true);

  /* Turn off all number LCD segments */
  SegmentLCD_NumberOff();

  /* Extract useful digits */
  div = 1;
  for (digit = 0; digit < NUM_DIGITS; digit++) {
    num = (value / div) % 10;
    if ((neg == 1) && (digit == (NUM_DIGITS - 1))) {
      num = signIndex;
    }
    /* Get number layout of display */
    bitpattern = Segment_Numbers[num];
    for (i = 0; i < 7; i++) {
      bit = EFM_Display.Number[digit].bit[i];
      com = EFM_Display.Number[digit].com[i];
      if (bitpattern & (1 << i)) {
        LCD_SegmentSet(com, bit, true);
      }
    }
    div = div * 10;
  }
  /* Sync LCD registers to LE domain */
  LCD_FreezeEnable(false);

#if defined(_SILICON_LABS_32B_SERIES_1)
  /* Evaluating the updated display contents and switching charge
     redistribution ON or OFF accordingly (only if dynamic change is enabled) */
  if (dynamicChgRedistEnabled) {
    SegmentLCD_updateCHGRDST();
  }
#endif
}

/**************************************************************************//**
 * @brief Turn all segments on numeric digits in display off
 *****************************************************************************/
void SegmentLCD_NumberOff(void)
{
  /* Turn off all number segments */
  LCD_NUMBER_OFF();

#if defined(_SILICON_LABS_32B_SERIES_1)
  /* Evaluating the updated display contents and switching charge
     redistribution ON or OFF accordingly (only if dynamic change is enabled) */
  if (dynamicChgRedistEnabled) {
    SegmentLCD_updateCHGRDST();
  }
#endif
}

/**************************************************************************//**
 * @brief Light up or shut off various symbols on Segment LCD
 * @param s Which symbol to turn on or off
 * @param on Zero is off, non-zero is on
 *****************************************************************************/
void SegmentLCD_Symbol(lcdSymbol s, int on)
{
  int com = 0;
  int bit = 0;

  switch (s) {
    case LCD_SYMBOL_GECKO:
      com = LCD_SYMBOL_GECKO_COM;
      bit = LCD_SYMBOL_GECKO_SEG;
      break;
#ifdef LCD_SYMBOL_ANT_SEG
    case LCD_SYMBOL_ANT:
      com = LCD_SYMBOL_ANT_COM;
      bit = LCD_SYMBOL_ANT_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_PAD0_SEG
    case LCD_SYMBOL_PAD0:
      com = LCD_SYMBOL_PAD0_COM;
      bit = LCD_SYMBOL_PAD0_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_PAD1_SEG
    case LCD_SYMBOL_PAD1:
      com = LCD_SYMBOL_PAD1_COM;
      bit = LCD_SYMBOL_PAD1_SEG;
      break;
#endif
    case LCD_SYMBOL_EFM32:
      com = LCD_SYMBOL_EFM32_COM;
      bit = LCD_SYMBOL_EFM32_SEG;
      break;
#ifdef LCD_SYMBOL_MINUS_SEG
    case LCD_SYMBOL_MINUS:
      com = LCD_SYMBOL_MINUS_COM;
      bit = LCD_SYMBOL_MINUS_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_C1_SEG
    case LCD_SYMBOL_C1:
      com = LCD_SYMBOL_C1_COM;
      bit = LCD_SYMBOL_C1_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_C2_SEG
    case LCD_SYMBOL_C2:
      com = LCD_SYMBOL_C2_COM;
      bit = LCD_SYMBOL_C2_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_C3_SEG
    case LCD_SYMBOL_C3:
      com = LCD_SYMBOL_C3_COM;
      bit = LCD_SYMBOL_C3_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_C4_SEG
    case LCD_SYMBOL_C4:
      com = LCD_SYMBOL_C4_COM;
      bit = LCD_SYMBOL_C4_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_C5_SEG
    case LCD_SYMBOL_C5:
      com = LCD_SYMBOL_C5_COM;
      bit = LCD_SYMBOL_C5_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_C6_SEG
    case LCD_SYMBOL_C6:
      com = LCD_SYMBOL_C6_COM;
      bit = LCD_SYMBOL_C6_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_C7_SEG
    case LCD_SYMBOL_C7:
      com = LCD_SYMBOL_C7_COM;
      bit = LCD_SYMBOL_C7_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_C8_SEG
    case LCD_SYMBOL_C8:
      com = LCD_SYMBOL_C8_COM;
      bit = LCD_SYMBOL_C8_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_C9_SEG
    case LCD_SYMBOL_C9:
      com = LCD_SYMBOL_C9_COM;
      bit = LCD_SYMBOL_C9_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_C10_SEG
    case LCD_SYMBOL_C10:
      com = LCD_SYMBOL_C10_COM;
      bit = LCD_SYMBOL_C10_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_C11_SEG
    case LCD_SYMBOL_C11:
      com = LCD_SYMBOL_C11_COM;
      bit = LCD_SYMBOL_C11_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_C12_SEG
    case LCD_SYMBOL_C12:
      com = LCD_SYMBOL_C12_COM;
      bit = LCD_SYMBOL_C12_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_C13_SEG
    case LCD_SYMBOL_C13:
      com = LCD_SYMBOL_C13_COM;
      bit = LCD_SYMBOL_C13_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_C14_SEG
    case LCD_SYMBOL_C14:
      com = LCD_SYMBOL_C14_COM;
      bit = LCD_SYMBOL_C14_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_C15_SEG
    case LCD_SYMBOL_C15:
      com = LCD_SYMBOL_C15_COM;
      bit = LCD_SYMBOL_C15_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_C16_SEG
    case LCD_SYMBOL_C16:
      com = LCD_SYMBOL_C16_COM;
      bit = LCD_SYMBOL_C16_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_C17_SEG
    case LCD_SYMBOL_C17:
      com = LCD_SYMBOL_C17_COM;
      bit = LCD_SYMBOL_C17_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_C18_SEG
    case LCD_SYMBOL_C18:
      com = LCD_SYMBOL_C18_COM;
      bit = LCD_SYMBOL_C18_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_C19_SEG
    case LCD_SYMBOL_C19:
      com = LCD_SYMBOL_C19_COM;
      bit = LCD_SYMBOL_C19_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_COL1_SEG
    case LCD_SYMBOL_COL1:
      com = LCD_SYMBOL_COL1_COM;
      bit = LCD_SYMBOL_COL1_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_COL2_SEG
    case LCD_SYMBOL_COL2:
      com = LCD_SYMBOL_COL2_COM;
      bit = LCD_SYMBOL_COL2_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_COL3_SEG
    case LCD_SYMBOL_COL3:
      com = LCD_SYMBOL_COL3_COM;
      bit = LCD_SYMBOL_COL3_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_COL5_SEG
    case LCD_SYMBOL_COL5:
      com = LCD_SYMBOL_COL5_COM;
      bit = LCD_SYMBOL_COL5_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_COL10_SEG
    case LCD_SYMBOL_COL10:
      com = LCD_SYMBOL_COL10_COM;
      bit = LCD_SYMBOL_COL10_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_DEGC_SEG
    case LCD_SYMBOL_DEGC:
      com = LCD_SYMBOL_DEGC_COM;
      bit = LCD_SYMBOL_DEGC_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_DEGF_SEG
    case LCD_SYMBOL_DEGF:
      com = LCD_SYMBOL_DEGF_COM;
      bit = LCD_SYMBOL_DEGF_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_DP2_SEG
    case LCD_SYMBOL_DP2:
      com = LCD_SYMBOL_DP2_COM;
      bit = LCD_SYMBOL_DP2_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_DP3_SEG
    case LCD_SYMBOL_DP3:
      com = LCD_SYMBOL_DP3_COM;
      bit = LCD_SYMBOL_DP3_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_DP4_SEG
    case LCD_SYMBOL_DP4:
      com = LCD_SYMBOL_DP4_COM;
      bit = LCD_SYMBOL_DP4_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_DP5_SEG
    case LCD_SYMBOL_DP5:
      com = LCD_SYMBOL_DP5_COM;
      bit = LCD_SYMBOL_DP5_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_DP6
    case LCD_SYMBOL_DP6:
      com = LCD_SYMBOL_DP6_COM;
      bit = LCD_SYMBOL_DP6_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_DP10
    case LCD_SYMBOL_DP10:
      com = LCD_SYMBOL_DP10_COM;
      bit = LCD_SYMBOL_DP10_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_AM_SEG
    case LCD_SYMBOL_AM:
      com = LCD_SYMBOL_AM_COM;
      bit = LCD_SYMBOL_AM_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_PM_SEG
    case LCD_SYMBOL_PM:
      com = LCD_SYMBOL_PM_COM;
      bit = LCD_SYMBOL_PM_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_MICROAMP_SEG
    case LCD_SYMBOL_MICROAMP:
      com = LCD_SYMBOL_MICROAMP_COM;
      bit = LCD_SYMBOL_MICROAMP_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_MILLIAMP_SEG
    case LCD_SYMBOL_MILLIAMP:
      com = LCD_SYMBOL_MILLIAMP_COM;
      bit = LCD_SYMBOL_MILLIAMP_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_S2_SEG
    case LCD_SYMBOL_S2:
      com = LCD_SYMBOL_S2_COM;
      bit = LCD_SYMBOL_S2_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_S3_SEG
    case LCD_SYMBOL_S3:
      com = LCD_SYMBOL_S3_COM;
      bit = LCD_SYMBOL_S3_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_S4_SEG
    case LCD_SYMBOL_S4:
      com = LCD_SYMBOL_S4_COM;
      bit = LCD_SYMBOL_S4_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_S5_SEG
    case LCD_SYMBOL_S5:
      com = LCD_SYMBOL_S5_COM;
      bit = LCD_SYMBOL_S5_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_S6_SEG
    case LCD_SYMBOL_S6:
      com = LCD_SYMBOL_S6_COM;
      bit = LCD_SYMBOL_S6_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_S7_SEG
    case LCD_SYMBOL_S7:
      com = LCD_SYMBOL_S7_COM;
      bit = LCD_SYMBOL_S7_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_S8_SEG
    case LCD_SYMBOL_S8:
      com = LCD_SYMBOL_S8_COM;
      bit = LCD_SYMBOL_S8_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_S9_SEG
    case LCD_SYMBOL_S9:
      com = LCD_SYMBOL_S9_COM;
      bit = LCD_SYMBOL_S9_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_S10_SEG
    case LCD_SYMBOL_S10:
      com = LCD_SYMBOL_S10_COM;
      bit = LCD_SYMBOL_S10_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_S11_SEG
    case LCD_SYMBOL_S11:
      com = LCD_SYMBOL_S11_COM;
      bit = LCD_SYMBOL_S11_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_S12_SEG
    case LCD_SYMBOL_S12:
      com = LCD_SYMBOL_S12_COM;
      bit = LCD_SYMBOL_S12_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_S13_SEG
    case LCD_SYMBOL_S13:
      com = LCD_SYMBOL_S13_COM;
      bit = LCD_SYMBOL_S13_SEG;
      break;
#endif
#ifdef LCD_SYMBOL_S14_SEG
    case LCD_SYMBOL_S14:
      com = LCD_SYMBOL_S14_COM;
      bit = LCD_SYMBOL_S14_SEG;
      break;
#endif
    default:
      break;
  }
  if (on) {
    LCD_SegmentSet(com, bit, true);
  } else {
    LCD_SegmentSet(com, bit, false);
  }

#if defined(_SILICON_LABS_32B_SERIES_1)
  /* Evaluating the updated display contents and switching charge
     redistribution ON or OFF accordingly (only if dynamic change is enabled) */
  if (dynamicChgRedistEnabled) {
    SegmentLCD_updateCHGRDST();
  }
#endif
}

/**************************************************************************//**
 * @brief Write hexadecimal number on numeric part on Segment LCD display
 * @param value Numeric value to put on display
 *****************************************************************************/
void SegmentLCD_UnsignedHex(uint32_t value)
{
  int      num, i, com, bit, digit;
  uint16_t bitpattern;

  /* Parameter consistancy check */
  if (value >= NUM_VAL_MAX_HEX) {
    value = NUM_VAL_MAX_HEX;
  }

  /* If an update is in progress we must block, or there might be tearing */
  LCD_SyncBusyDelay(0xFFFFFFFF);

  /* Freeze updates to avoid partial refresh of display */
  LCD_FreezeEnable(true);

  /* Turn off all number LCD segments */
  SegmentLCD_NumberOff();

  for (digit = 0; digit < NUM_DIGITS; digit++) {
    num        = (value >> (4 * digit)) & 0x0f;
    bitpattern = Segment_Numbers[num];
    for (i = 0; i < 7; i++) {
      bit = EFM_Display.Number[digit].bit[i];
      com = EFM_Display.Number[digit].com[i];
      if (bitpattern & (1 << i)) {
        LCD_SegmentSet(com, bit, true);
      }
    }
  }

  /* Sync LCD registers to LE domain */
  LCD_FreezeEnable(false);

#if defined(_SILICON_LABS_32B_SERIES_1)
  /* Evaluating the updated display contents and switching charge
     redistribution ON or OFF accordingly (only if dynamic change is enabled) */
  if (dynamicChgRedistEnabled) {
    SegmentLCD_updateCHGRDST();
  }
#endif
}

/**************************************************************************//**
 * @brief Write text on LCD display
 * @param string Text string to show on display
 *****************************************************************************/
void SegmentLCD_Write(const char *string)
{
  int      data, length, index;
  uint16_t bitfield;
  uint32_t com, bit;
  int      i;

  length = strlen(string);
  index  = 0;

  /* If an update is in progress we must block, or there might be tearing */
  LCD_SyncBusyDelay(0xFFFFFFFF);

  /* Freeze LCD to avoid partial updates */
  LCD_FreezeEnable(true);

  /* Turn all segments off */
  SegmentLCD_AlphaNumberOff();

  /* Fill out all characters on display */
  for (index = 0; index < ALPHA_DIGITS; index++) {
    if (index < length) {
      data = (int) *string;
    } else {       /* Padding with space */
      data = 0x20; /* SPACE */
    }
    /* Defined letters currently starts at "SPACE" - ASCII 0x20; */
    data = data - 0x20;
    /* Get font for this letter */
    bitfield = Segment_Alphabet[data];

    for (i = 0; i < 14; i++) {
      bit = EFM_Display.Text[index].bit[i];
      com = EFM_Display.Text[index].com[i];

      if (bitfield & (1 << i)) {
        /* Turn on segment */
        LCD_SegmentSet(com, bit, true);
      }
    }
    string++;
  }
  /* Enable update */
  LCD_FreezeEnable(false);

#if defined(_SILICON_LABS_32B_SERIES_1)
  /* Evaluating the updated display contents and switching charge
     redistribution ON or OFF accordingly (only if dynamic change is enabled) */
  if (dynamicChgRedistEnabled) {
    SegmentLCD_updateCHGRDST();
  }
#endif
}

/**************************************************************************//**
 * @brief Counting the number of active segments in a single common line
 * @param value in registers LCD_SEGDnL and LCD_SEGDnH  (n = common line number)
 *****************************************************************************/
unsigned int SegmentLCD_segCnt(unsigned int segdl, unsigned int segdh)
{
  /* Number of ones in a nibble */
  const unsigned int oneCnt[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };

  unsigned int cnt = 0;

  /* Accumulating number of ones in each nibble */
  cnt += oneCnt[(segdl >>  0) & 0x0000000f];
  cnt += oneCnt[(segdl >>  4) & 0x0000000f];
  cnt += oneCnt[(segdl >>  8) & 0x0000000f];
  cnt += oneCnt[(segdl >> 12) & 0x0000000f];
  cnt += oneCnt[(segdl >> 16) & 0x0000000f];
  cnt += oneCnt[(segdl >> 20) & 0x0000000f];
  cnt += oneCnt[(segdl >> 24) & 0x0000000f];
  cnt += oneCnt[(segdl >> 28) & 0x0000000f];
  cnt += oneCnt[(segdh >>  0) & 0x0000000f];
  cnt += oneCnt[(segdh >>  4) & 0x0000000f];

  return cnt;
}

/**************************************************************************//**
 * @brief Enabling/disabling dynamic charge redistribution
 * @param true ==> enable, false ==> disable
 *****************************************************************************/
#if defined(_SILICON_LABS_32B_SERIES_1)
void SegmentLCD_chargeRedistributionEnable(bool enable)
{
  if ( enable ) {
    LCD->DISPCTRL = (LCD->DISPCTRL & ~_LCD_DISPCTRL_CHGRDST_MASK)
                    | lcdChargeRedistributionEnable;
    dynamicChgRedistEnabled = true;
  } else {
    LCD->DISPCTRL = (LCD->DISPCTRL & ~_LCD_DISPCTRL_CHGRDST_MASK)
                    | lcdChargeRedistributionDisable;
    dynamicChgRedistEnabled = false;
  }
}
#endif

/**************************************************************************//**
 * @brief
 * Dynamically switching charge redistribution ON/OFF based on display contents
 * @param None
 *****************************************************************************/
#if defined(_SILICON_LABS_32B_SERIES_1)
void SegmentLCD_updateCHGRDST(void)
{
  const unsigned int COM_0_THRESHOLD_HI = 20;
  const unsigned int COM_0_THRESHOLD_LO = 14;
  const unsigned int COM_1_6_THRESHOLD_HI = 11;
  const unsigned int COM_1_6_THRESHOLD_LO = 9;
  const unsigned int COM_7_THRESHOLD_HI = 28;
  const unsigned int COM_7_THRESHOLD_LO = 24;
  const unsigned int SEG_TOTAL_THRESHOLD = 40;
  unsigned int segdl, segCnt;
  unsigned int segCntAcc = 0;
  bool thresholdHiReached = false;
  bool thresholdLoReached = false;
  bool switchChgRedistON = false;

  segdl = LCD->SEGD0L;
  segCnt = SegmentLCD_segCnt(segdl, LCD->SEGD0H);
  if (segCnt >= COM_0_THRESHOLD_HI) {
    thresholdHiReached = true;
  }
  if (segCnt >= COM_0_THRESHOLD_LO) {
    thresholdLoReached = true;
  }
  segCntAcc += segCnt;

  segdl = LCD->SEGD1L;
  segCnt = SegmentLCD_segCnt(segdl, LCD->SEGD1H);
  if (segCnt >= COM_1_6_THRESHOLD_HI) {
    thresholdHiReached = true;
  }
  if (segCnt >= COM_1_6_THRESHOLD_LO) {
    thresholdLoReached = true;
  }
  segCntAcc += segCnt;

  segdl = LCD->SEGD2L;
  segCnt = SegmentLCD_segCnt(segdl, LCD->SEGD2H);
  if (segCnt >= COM_1_6_THRESHOLD_HI) {
    thresholdHiReached = true;
  }
  if (segCnt >= COM_1_6_THRESHOLD_LO) {
    thresholdLoReached = true;
  }
  segCntAcc += segCnt;

  segdl = LCD->SEGD3L;
  segCnt = SegmentLCD_segCnt(segdl, LCD->SEGD3H);
  if (segCnt >= COM_1_6_THRESHOLD_HI) {
    thresholdHiReached = true;
  }
  if (segCnt >= COM_1_6_THRESHOLD_LO) {
    thresholdLoReached = true;
  }
  segCntAcc += segCnt;

  segdl = LCD->SEGD4L;
  segCnt = SegmentLCD_segCnt(segdl, LCD->SEGD4H);
  if (segCnt >= COM_1_6_THRESHOLD_HI) {
    thresholdHiReached = true;
  }
  if (segCnt >= COM_1_6_THRESHOLD_LO) {
    thresholdLoReached = true;
  }
  segCntAcc += segCnt;

  segdl = LCD->SEGD5L;
  segCnt = SegmentLCD_segCnt(segdl, LCD->SEGD5H);
  if (segCnt >= COM_1_6_THRESHOLD_HI) {
    thresholdHiReached = true;
  }
  if (segCnt >= COM_1_6_THRESHOLD_LO) {
    thresholdLoReached = true;
  }
  segCntAcc += segCnt;

  segdl = LCD->SEGD6L;
  segCnt = SegmentLCD_segCnt(segdl, LCD->SEGD6H);
  if (segCnt >= COM_1_6_THRESHOLD_HI) {
    thresholdHiReached = true;
  }
  if (segCnt >= COM_1_6_THRESHOLD_LO) {
    thresholdLoReached = true;
  }
  segCntAcc += segCnt;

  segdl = LCD->SEGD7L;
  segCnt = SegmentLCD_segCnt(segdl, LCD->SEGD7H);
  if (segCnt >= COM_7_THRESHOLD_HI) {
    thresholdHiReached = true;
  }
  if (segCnt >= COM_7_THRESHOLD_LO) {
    thresholdLoReached = true;
  }
  segCntAcc += segCnt;

  /* Switch charge redistribution ON if condition 1 or 2 (or both) below is met
     1. at least one of the common lines has reached its high threshold
        of active segments
     2. at least one of the common lines has reached its low threshold
        of active segments and there are 40 or more active segments in
        the display (i.e. all common lines combined) */
  switchChgRedistON =
    (thresholdHiReached)
    || (thresholdLoReached && (segCntAcc >= SEG_TOTAL_THRESHOLD));

  if (switchChgRedistON) {
    /* Switching charge redistribution ON */
    LCD->DISPCTRL = (LCD->DISPCTRL & ~_LCD_DISPCTRL_CHGRDST_MASK)
                    | lcdChargeRedistributionEnable;
  } else {
    /* Switching charge redistribution OFF */
    LCD->DISPCTRL = (LCD->DISPCTRL & ~_LCD_DISPCTRL_CHGRDST_MASK)
                    | lcdChargeRedistributionDisable;
  }
}
#endif

/******************************************************************************
* @brief Display blocks on LCD display: blank, fill, outline, outline & fill
* @param topMode array of block modes for the top row with element zero
* representing the left-most column and element six representing the
* right-most column
* @param botMode array of block modes for the bottom row with element zero
* representing the left-most column and element six representing the
* right-most column
******************************************************************************/
static void displayBlock(
  SegmentLCD_BlockMode_TypeDef topMode[SEGMENT_LCD_NUM_BLOCK_COLUMNS],
  SegmentLCD_BlockMode_TypeDef botMode[SEGMENT_LCD_NUM_BLOCK_COLUMNS])
{
  int index;
  int mode;
  uint16_t bitfield;
  uint32_t com, bit;
  int      i;

  /* Fill out all blocks in the top row of the display */
  for (index = 0; index < SEGMENT_LCD_NUM_BLOCK_COLUMNS; index++) {
    mode = topMode[index];

    /* Get segment bitmap for this block position */
    bitfield = Blocks[mode];

    for (i = 0; i < 8; i++) {
      if (bitfield & (1 << i)) {
        bit = EFM_Display.TopBlocks[index].bit[i];
        com = EFM_Display.TopBlocks[index].com[i];

        // Turn on segment
        LCD_SegmentSet(com, bit, true);
      }
    }
  }
  /* Fill out all blocks in the bottom row of the display */
  for (index = 0; index < SEGMENT_LCD_NUM_BLOCK_COLUMNS; index++) {
    mode = botMode[index];

    /* Get segment bitmap for this block position */
    bitfield = Blocks[mode];

    for (i = 0; i < 8; i++) {
      if (bitfield & (1 << i)) {
        bit = EFM_Display.BotBlocks[index].bit[i];
        com = EFM_Display.BotBlocks[index].com[i];

        // Turn on segment
        LCD_SegmentSet(com, bit, true);
      }
    }
  }
}

#if defined(LCD_MODULE_CE322_1001)
/**************************************************************************//**
 * @brief Turn on or off individual pixels in the array
 * @param element "pixel number", range 0 - 34
 * @param on false is off, true is on
 *****************************************************************************/
void SegmentLCD_Array(int element, bool on)
{
  uint32_t com, bit;

  com = EFM_Display.Array.com[element];
  bit = EFM_Display.Array.bit[element];
  LCD_SegmentSet(com, bit, on);
}

#else

/**************************************************************************//**
 * @brief Light up or shut off Ring of Indicators
 * @param anum "Segment number" on "Ring", range 0 - 7
 * @param on Zero is off, non-zero is on
 *****************************************************************************/
void SegmentLCD_ARing(int anum, int on)
{
  uint32_t com, bit;

  com = EFM_Display.ARing.com[anum];
  bit = EFM_Display.ARing.bit[anum];

  if (on) {
    LCD_SegmentSet(com, bit, true);
  } else {
    LCD_SegmentSet(com, bit, false);
  }
}

/**************************************************************************//**
 * @brief Light up or shut off Battery Indicator
 * @param batteryLevel Battery Level, 0 to 4 (0 turns all off)
 *****************************************************************************/
void SegmentLCD_Battery(int batteryLevel)
{
  uint32_t com, bit;
  int      i, on;

  for (i = 0; i < 4; i++) {
    if (i < batteryLevel) {
      on = 1;
    } else {
      on = 0;
    }
    com = EFM_Display.Battery.com[i];
    bit = EFM_Display.Battery.bit[i];

    if (on) {
      LCD_SegmentSet(com, bit, true);
    } else {
      LCD_SegmentSet(com, bit, false);
    }
  }
}

/**************************************************************************//**
 * @brief Light up or shut off Energy Mode indicator
 * @param em Energy Mode numer 0 to 4
 * @param on Zero is off, non-zero is on
 *****************************************************************************/
void SegmentLCD_EnergyMode(int em, int on)
{
  uint32_t com, bit;

  com = EFM_Display.EMode.com[em];
  bit = EFM_Display.EMode.bit[em];

  if (on) {
    LCD_SegmentSet(com, bit, true);
  } else {
    LCD_SegmentSet(com, bit, false);
  }
}
#endif

/** @} (end group SegmentLcd) */
/** @} (end group kitdrv) */
