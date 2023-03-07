/***************************************************************************//**
 * @file
 * @brief Liquid Crystal Display (LCD) peripheral API
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

#ifndef EM_LCD_H
#define EM_LCD_H

#include "em_device.h"

#if defined(LCD_COUNT) && (LCD_COUNT > 0)
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup lcd
 * @{
 ******************************************************************************/

/*******************************************************************************
 ********************************   DEFINES   **********************************
 ******************************************************************************/

/** Default Clock Prescaler. */
#define LCD_DEFAULT_CLOCK_PRESCALER 64
/** Default LCD Frame Rate Divisor. */
#define LCD_DEFAULT_FRAME_RATE_DIV  4
/** Default LCD Contrast. */
#define LCD_DEFAULT_CONTRAST        15

#if defined(_SILICON_LABS_32B_SERIES_2)
/** Maximum common lines of LCD. */
#define LCD_COM_LINES_MAX  4
/** Maximum segment lines of LCD. */
#define LCD_SEGMENT_LINES_MAX  20
#endif

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

/** MUX setting. */
typedef enum {
  /** Static (segments can be multiplexed with LCD_COM[0]). */
  lcdMuxStatic     = LCD_DISPCTRL_MUX_STATIC,
  /** Duplex / 1/2 Duty cycle (segments can be multiplexed with LCD_COM[0:1]). */
  lcdMuxDuplex     = LCD_DISPCTRL_MUX_DUPLEX,
  /** Triplex / 1/3 Duty cycle (segments can be multiplexed with LCD_COM[0:2]). */
  lcdMuxTriplex    = LCD_DISPCTRL_MUX_TRIPLEX,
  /** Quadruplex / 1/4 Duty cycle (segments can be multiplexed with LCD_COM[0:3]). */
  lcdMuxQuadruplex = LCD_DISPCTRL_MUX_QUADRUPLEX,
#if defined(LCD_DISPCTRL_MUXE_MUXE)
  /** Sextaplex / 1/6 Duty cycle (segments can be multiplexed with LCD_COM[0:5]). */
  lcdMuxSextaplex  = LCD_DISPCTRL_MUXE_MUXE | LCD_DISPCTRL_MUX_DUPLEX,
  /** Octaplex / 1/6 Duty cycle (segments can be multiplexed with LCD_COM[0:5]). */
  lcdMuxOctaplex   = LCD_DISPCTRL_MUXE_MUXE | LCD_DISPCTRL_MUX_QUADRUPLEX
#elif defined(LCD_DISPCTRL_MUX_SEXTAPLEX)
  /** Sextaplex / 1/6 Duty cycle (segments can be multiplexed with LCD_COM[0:5]). */
  lcdMuxSextaplex  = LCD_DISPCTRL_MUX_SEXTAPLEX,
  /** Octaplex / 1/6 Duty cycle (segments can be multiplexed with LCD_COM[0:5]). */
  lcdMuxOctaplex   = LCD_DISPCTRL_MUX_OCTAPLEX,
#endif
} LCD_Mux_TypeDef;

/** Wave type. */
typedef enum {
#if defined(_SILICON_LABS_32B_SERIES_2)
  /** Low power optimized waveform output. */
  lcdWaveLowPower = LCD_DISPCTRL_WAVE_TYPEB,
  /** Regular waveform output */
  lcdWaveNormal   = LCD_DISPCTRL_WAVE_TYPEA
#else
  /** Low power optimized waveform output. */
  lcdWaveLowPower = LCD_DISPCTRL_WAVE_LOWPOWER,
  /** Regular waveform output */
  lcdWaveNormal   = LCD_DISPCTRL_WAVE_NORMAL
#endif
} LCD_Wave_TypeDef;

/** Bias setting. */
typedef enum {
  /** Static (2 levels). */
  lcdBiasStatic    = LCD_DISPCTRL_BIAS_STATIC,
  /** 1/2 Bias (3 levels). */
  lcdBiasOneHalf   = LCD_DISPCTRL_BIAS_ONEHALF,
  /** 1/3 Bias (4 levels). */
  lcdBiasOneThird  = LCD_DISPCTRL_BIAS_ONETHIRD,
#if defined(LCD_DISPCTRL_BIAS_ONEFOURTH)
  /** 1/4 Bias (5 levels). */
  lcdBiasOneFourth = LCD_DISPCTRL_BIAS_ONEFOURTH,
#endif
} LCD_Bias_TypeDef;

#if defined(_SILICON_LABS_32B_SERIES_0)
/** VLCD Voltage Source. */
typedef enum {
  /** VLCD Powered by VDD. */
  lcdVLCDSelVDD       = LCD_DISPCTRL_VLCDSEL_VDD,
  /** VLCD Powered by external VDD / Voltage Boost. */
  lcdVLCDSelVExtBoost = LCD_DISPCTRL_VLCDSEL_VEXTBOOST
} LCD_VLCDSel_TypeDef;
#endif

/** Contrast Configuration. */
#if defined(_SILICON_LABS_32B_SERIES_0)
typedef enum {
  /** Contrast is adjusted relative to VDD (VLCD). */
  lcdConConfVLCD = LCD_DISPCTRL_CONCONF_VLCD,
  /** Contrast is adjusted relative to Ground. */
  lcdConConfGND  = LCD_DISPCTRL_CONCONF_GND
} LCD_ConConf_TypeDef;
#endif

#if defined(_SILICON_LABS_32B_SERIES_0)
/** Voltage Boost Level - Data sheets document setting for each part number. */
typedef enum {
  lcdVBoostLevel0 = LCD_DISPCTRL_VBLEV_LEVEL0, /**< Voltage boost LEVEL0. */
  lcdVBoostLevel1 = LCD_DISPCTRL_VBLEV_LEVEL1, /**< Voltage boost LEVEL1. */
  lcdVBoostLevel2 = LCD_DISPCTRL_VBLEV_LEVEL2, /**< Voltage boost LEVEL2. */
  lcdVBoostLevel3 = LCD_DISPCTRL_VBLEV_LEVEL3, /**< Voltage boost LEVEL3. */
  lcdVBoostLevel4 = LCD_DISPCTRL_VBLEV_LEVEL4, /**< Voltage boost LEVEL4. */
  lcdVBoostLevel5 = LCD_DISPCTRL_VBLEV_LEVEL5, /**< Voltage boost LEVEL5. */
  lcdVBoostLevel6 = LCD_DISPCTRL_VBLEV_LEVEL6, /**< Voltage boost LEVEL6. */
  lcdVBoostLevel7 = LCD_DISPCTRL_VBLEV_LEVEL7  /**< Voltage boost LEVEL7. */
} LCD_VBoostLevel_TypeDef;
#endif

#if defined(_SILICON_LABS_32B_SERIES_1) || defined(_SILICON_LABS_32B_SERIES_2)
/** Mode of operation. */
typedef enum {
#if defined(_SILICON_LABS_32B_SERIES_1)
  lcdModeNoExtCap = LCD_DISPCTRL_MODE_NOEXTCAP, /**< No external capacitor. */
  lcdModeStepDown = LCD_DISPCTRL_MODE_STEPDOWN, /**< External cap with resistor string. */
  lcdModeCpIntOsc = LCD_DISPCTRL_MODE_CPINTOSC, /**< External cap and internal oscillator. */
#elif defined(_SILICON_LABS_32B_SERIES_2)
  lcdModeStepDown   = LCD_BIASCTRL_MODE_STEPDOWN,   /**< External cap with resistor string. */
  lcdModeChargePump = LCD_BIASCTRL_MODE_CHARGEPUMP, /**< External cap and internal oscillator. */
#endif
} LCD_Mode_Typedef;
#endif

/** Frame Counter Clock Prescaler, FC-CLK = FrameRate (Hz) / this factor. */
typedef enum {
#if defined(_SILICON_LABS_32B_SERIES_2)
  /** Prescale Div 1. */
  lcdFCPrescDiv1 = LCD_BACFG_FCPRESC_DIV1,
  /** Prescale Div 2. */
  lcdFCPrescDiv2 = LCD_BACFG_FCPRESC_DIV2,
  /** Prescale Div 4. */
  lcdFCPrescDiv4 = LCD_BACFG_FCPRESC_DIV4,
  /** Prescale Div 8. */
  lcdFCPrescDiv8 = LCD_BACFG_FCPRESC_DIV8
#else
  /** Prescale Div 1. */
  lcdFCPrescDiv1 = LCD_BACTRL_FCPRESC_DIV1,
  /** Prescale Div 2. */
  lcdFCPrescDiv2 = LCD_BACTRL_FCPRESC_DIV2,
  /** Prescale Div 4. */
  lcdFCPrescDiv4 = LCD_BACTRL_FCPRESC_DIV4,
  /** Prescale Div 8. */
  lcdFCPrescDiv8 = LCD_BACTRL_FCPRESC_DIV8
#endif
} LCD_FCPreScale_TypeDef;

#if defined(_SILICON_LABS_32B_SERIES_0)
/** Segment selection. */
typedef enum {
  /** Select segment lines 0 to 3. */
  lcdSegment0_3   = (1 << 0),
  /** Select segment lines 4 to 7. */
  lcdSegment4_7   = (1 << 1),
  /** Select segment lines 8 to 11. */
  lcdSegment8_11  = (1 << 2),
  /** Select segment lines 12 to 15. */
  lcdSegment12_15 = (1 << 3),
  /** Select segment lines 16 to 19. */
  lcdSegment16_19 = (1 << 4),
  /** Select segment lines 20 to 23. */
  lcdSegment20_23 = (1 << 5),
#if defined(_LCD_SEGD0L_MASK) && (_LCD_SEGD0L_MASK == 0x00FFFFFFUL)
  /** Select all segment lines. */
  lcdSegmentAll   = (0x003f)
#elif defined(_LCD_SEGD0H_MASK) && (_LCD_SEGD0H_MASK == 0x000000FFUL)
  /** Select segment lines 24 to 27. */
  lcdSegment24_27 = (1 << 6),
  /** Select segment lines 28 to 31. */
  lcdSegment28_31 = (1 << 7),
  /** Select segment lines 32 to 35. */
  lcdSegment32_35 = (1 << 8),
  /** Select segment lines 36 to 39. */
  lcdSegment36_39 = (1 << 9),
  /** Select all segment lines. */
  lcdSegmentAll   = (0x03ff)
#endif
} LCD_SegmentRange_TypeDef;
#endif

/** Update Data Control. */
typedef enum {
  /** Regular update, data transfer done immediately. */
  lcdUpdateCtrlRegular      = LCD_CTRL_UDCTRL_REGULAR,
  /** Data transfer done at Frame Counter event. */
  lcdUpdateCtrlFCEvent      = LCD_CTRL_UDCTRL_FCEVENT,
  /** Data transfer done at Frame Start.  */
  lcdUpdateCtrlFrameStart   = LCD_CTRL_UDCTRL_FRAMESTART,
#if defined(_SILICON_LABS_32B_SERIES_2)
  /** Data transfer done at Display Counter event.  */
  lcdUpdateCtrlDisplayEvent = LCD_CTRL_UDCTRL_DISPLAYEVENT
#endif
} LCD_UpdateCtrl_TypeDef;

#if defined(_SILICON_LABS_32B_SERIES_2)
/** Auto Load address which will start the synchronization from CLK_BUS to CLK_PER. */
typedef enum {
  /** Starts synchronizing registers after a write to BACTRL. */
  lcdLoadAddrNone   = 0,
  /** Starts synchronizing registers after a write to BACTRL. */
  lcdLoadAddrBactrl = LCD_UPDATECTRL_LOADADDR_BACTRLWR,
  /** Starts synchronizing registers after a write to AREGA. */
  lcdLoadAddrAregA  = LCD_UPDATECTRL_LOADADDR_AREGAWR,
  /** Starts synchronizing registers after a write to AREGB. */
  lcdLoadAddrAregB  = LCD_UPDATECTRL_LOADADDR_AREGBWR,
  /** Starts synchronizing registers after a write to SEGD0. */
  lcdLoadAddrSegd0  = LCD_UPDATECTRL_LOADADDR_SEGD0WR,
  /** Starts synchronizing registers after a write to SEGD1. */
  lcdLoadAddrSegd1  = LCD_UPDATECTRL_LOADADDR_SEGD1WR,
  /** Starts synchronizing registers after a write to SEGD2. */
  lcdLoadAddrSegd2  = LCD_UPDATECTRL_LOADADDR_SEGD2WR,
  /** Starts synchronizing registers after a write to SEGD3. */
  lcdLoadAddrSegd3  = LCD_UPDATECTRL_LOADADDR_SEGD3WR
} LCD_LoadAddr_TypeDef;
#endif

/** Animation Shift operation; none, left or right. */
typedef enum {
  /** No shift. */
  lcdAnimShiftNone  = _LCD_BACTRL_AREGASC_NOSHIFT,
  /** Shift segment bits left. */
  lcdAnimShiftLeft  = _LCD_BACTRL_AREGASC_SHIFTLEFT,
  /** Shift segment bits right. */
  lcdAnimShiftRight = _LCD_BACTRL_AREGASC_SHIFTRIGHT
} LCD_AnimShift_TypeDef;

/** Animation Logic Control, how AReg and BReg should be combined. */
typedef enum {
  /** Use bitwise logic AND to mix animation register A (AREGA) and B (AREGB). */
  lcdAnimLogicAnd = LCD_BACTRL_ALOGSEL_AND,
  /** Use bitwise logic OR to mix animation register A (AREGA) and B (AREGB).  */
  lcdAnimLogicOr  = LCD_BACTRL_ALOGSEL_OR
} LCD_AnimLogic_TypeDef;

#if defined(_LCD_BACTRL_ALOC_MASK)
/** Animation Location, set the LCD segments which animation applies to. */
typedef enum {
  /** Animation appears on segments 0 to 7. */
  lcdAnimLocSeg0To7  = LCD_BACTRL_ALOC_SEG0TO7,
  /** Animation appears on segments 8 to 15.  */
  lcdAnimLocSeg8To15 = LCD_BACTRL_ALOC_SEG8TO15
} LCD_AnimLoc_TypeDef;
#endif

#if defined(_LCD_DISPCTRL_CHGRDST_MASK)
/** Charge redistribution control. */
typedef enum {
  /** Disable charge redistribution. */
  lcdChargeRedistributionDisable    = LCD_DISPCTRL_CHGRDST_DISABLE,
  /** Use 1 prescaled low frequency clock cycle for charge redistribution. */
  lcdChargeRedistributionEnable     = LCD_DISPCTRL_CHGRDST_ONE,
  /** Use 2 prescaled low frequency clock cycle for charge redistribution. */
  lcdChargeRedistributionTwoCycle   = LCD_DISPCTRL_CHGRDST_TWO,
  /** Use 3 prescaled low frequency clock cycle for charge redistribution. */
  lcdChargeRedistributionThreeCycle = LCD_DISPCTRL_CHGRDST_THREE,
  /** Use 4 prescaled low frequency clock cycle for charge redistribution. */
  lcdChargeRedistributionFourCycle  = LCD_DISPCTRL_CHGRDST_FOUR
} LCD_ChargeRedistribution_TypeDef;
#endif

#if defined(_SILICON_LABS_32B_SERIES_2)
/** DMA mode of operation. */
typedef enum {
  lcdDmaModeDisable      = LCD_BIASCTRL_DMAMODE_DMADISABLE,   /**< No DMA requests are generated. */
  lcdDmaModeFrameCounterEvent   = LCD_BIASCTRL_DMAMODE_DMAFC, /**< DMA request on frame counter event. */
  lcdDmaModeDisplayEvent = LCD_BIASCTRL_DMAMODE_DMADISPLAY    /**< DMA request on display counter event. */
} LCD_DmaMode_Typedef;
#endif

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

/** LCD Animation Configuration. */
typedef struct {
  /** Enable Animation at end of initialization. */
  bool                  enable;
  /** Initial Animation Register A Value. */
  uint32_t              AReg;
  /** Shift operation of Animation Register A. */
  LCD_AnimShift_TypeDef AShift;
  /** Initial Animation Register B Value. */
  uint32_t              BReg;
  /** Shift operation of Animation Register B. */
  LCD_AnimShift_TypeDef BShift;
  /** A and B Logical Operation to use for mixing and outputting resulting segments. */
  LCD_AnimLogic_TypeDef animLogic;
#if defined(_LCD_BACTRL_ALOC_MASK)
  /** Number of first segment to animate. */
  LCD_AnimLoc_TypeDef   startSeg;
#endif
} LCD_AnimInit_TypeDef;

/** LCD Frame Control Initialization. */
typedef struct {
  /** Enable at end. */
  bool                   enable;
  /** Frame Counter top value. */
  uint32_t               top;
  /** Frame Counter clock prescaler. */
  LCD_FCPreScale_TypeDef prescale;
} LCD_FrameCountInit_TypeDef;

/** LCD Controller Initialization structure. */
typedef struct {
  /** Enable controller at end of initialization. */
  bool                enable;
  /** Mux configuration. */
  LCD_Mux_TypeDef     mux;
  /** Bias configuration. */
  LCD_Bias_TypeDef    bias;
  /** Wave configuration. */
  LCD_Wave_TypeDef    wave;
#if defined(_SILICON_LABS_32B_SERIES_0)
  /** VLCD Select. */
  LCD_VLCDSel_TypeDef vlcd;
  /** Contrast Configuration. */
  LCD_ConConf_TypeDef contrast;
#endif
#if defined(_SILICON_LABS_32B_SERIES_1) || defined(_SILICON_LABS_32B_SERIES_2)
  /** Mode of operation. */
  LCD_Mode_Typedef                      mode;
  /** Charge redistribution cycles. */
  LCD_ChargeRedistribution_TypeDef      chargeRedistribution;
  /** Frame rate divider. */
  uint8_t                               frameRateDivider;
  /** Contrast level. */
  int                                   contrastLevel;
#if defined(_SILICON_LABS_32B_SERIES_2)
  /** Clock Prescaler. */
  uint32_t                              clockPrescaler;
#endif
#endif
} LCD_Init_TypeDef;

/** Default configuration for LCD initialization structure, enables 160 segments.  */
#if defined(_SILICON_LABS_32B_SERIES_0)
#define LCD_INIT_DEFAULT \
  {                      \
    true,                \
    lcdMuxQuadruplex,    \
    lcdBiasOneThird,     \
    lcdWaveLowPower,     \
    lcdVLCDSelVDD,       \
    lcdConConfVLCD,      \
  }
#endif

#if defined(_SILICON_LABS_32B_SERIES_1)
#define LCD_INIT_DEFAULT           \
  {                                \
    true,                          \
    lcdMuxOctaplex,                \
    lcdBiasOneFourth,              \
    lcdWaveLowPower,               \
    lcdModeNoExtCap,               \
    lcdChargeRedistributionEnable, \
    LCD_DEFAULT_FRAME_RATE_DIV,    \
    LCD_DEFAULT_CONTRAST           \
  }
#endif

#if defined(_SILICON_LABS_32B_SERIES_2)
#define LCD_INIT_DEFAULT           \
  {                                \
    true,                          \
    lcdMuxQuadruplex,              \
    lcdBiasOneFourth,              \
    lcdWaveLowPower,               \
    lcdModeStepDown,               \
    lcdChargeRedistributionEnable, \
    LCD_DEFAULT_FRAME_RATE_DIV,    \
    LCD_DEFAULT_CONTRAST,          \
    LCD_DEFAULT_CLOCK_PRESCALER    \
  }
#endif

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

void LCD_Init(const LCD_Init_TypeDef *lcdInit);
#if defined(_SILICON_LABS_32B_SERIES_0)
void LCD_VLCDSelect(LCD_VLCDSel_TypeDef vlcd);
#endif
void LCD_UpdateCtrl(LCD_UpdateCtrl_TypeDef ud);
void LCD_FrameCountInit(const LCD_FrameCountInit_TypeDef *fcInit);
void LCD_AnimInit(const LCD_AnimInit_TypeDef *animInit);

#if defined(_SILICON_LABS_32B_SERIES_0)
void LCD_SegmentRangeEnable(LCD_SegmentRange_TypeDef segment, bool enable);
#endif
#if defined(_SILICON_LABS_32B_SERIES_2)
void LCD_SegmentEnable(uint32_t seg_nbr, bool enable);
void LCD_ComEnable(uint8_t com, bool enable);
void LCD_DmaModeSet(LCD_DmaMode_Typedef mode);
#endif
void LCD_SegmentSet(int com, int bit, bool enable);
void LCD_SegmentSetLow(int com, uint32_t mask, uint32_t bits);
#if defined(_LCD_SEGD0H_MASK)
void LCD_SegmentSetHigh(int com, uint32_t mask, uint32_t bits);
#endif
void LCD_ContrastSet(int level);
void LCD_BiasSet(LCD_Bias_TypeDef bias);
#if defined(_SILICON_LABS_32B_SERIES_0)
void LCD_VBoostSet(LCD_VBoostLevel_TypeDef vboost);
#endif
#if defined(LCD_CTRL_DSC)
void LCD_BiasSegmentSet(int segment, int biasLevel);
void LCD_BiasComSet(int com, int biasLevel);
#endif
#if defined(_SILICON_LABS_32B_SERIES_1) || defined(_SILICON_LABS_32B_SERIES_2)
void LCD_ModeSet(LCD_Mode_Typedef mode);
void LCD_ChargeRedistributionCyclesSet(uint8_t cycles);
#endif

#if defined(_SILICON_LABS_32B_SERIES_2)
/***************************************************************************//**
 * @brief
 *    Wait for load synchronization completion.
 *
 * @note
 *    Doing any writes to HV registers will not go through and will cause a
 *    bus fault.
 ******************************************************************************/
__STATIC_INLINE void LCD_LoadBusyWait(void)
{
  while (LCD->STATUS & _LCD_STATUS_LOADBUSY_MASK) ;
}
#endif

#if defined(_SILICON_LABS_32B_SERIES_2)
/***************************************************************************//**
 * @brief
 *    Wait for the LCD to complete resetting or disabling procedure.
 ******************************************************************************/
__STATIC_INLINE void LCD_ReadyWait(void)
{
  while ((LCD->SWRST & _LCD_SWRST_RESETTING_MASK)
         || (LCD->EN & _LCD_EN_DISABLING_MASK)
         || (LCD->STATUS & _LCD_STATUS_LOADBUSY_MASK)) {
    // Wait for all synchronizations to finish
  }
}
#endif

/***************************************************************************//**
 * @brief
 *   Enable or disable LCD controller.
 *
 * @param[in] enable
 *   If true, enables LCD controller with current configuration. If false,
 *   disables LCD controller. Enable CMU clock for LCD for correct
 *   operation.
 ******************************************************************************/
__STATIC_INLINE void LCD_Enable(bool enable)
{
#if defined(_SILICON_LABS_32B_SERIES_2)
  if (enable) {
    LCD->EN_SET = LCD_EN_EN;
  } else {
    /* Wait for internal synchronization completion. */
    LCD_LoadBusyWait();

    LCD->EN_CLR = LCD_EN_EN;
    while (LCD->EN & _LCD_EN_DISABLING_MASK) {
    }
  }
#else

#if defined(LCD_HAS_SET_CLEAR)
  if (enable) {
    LCD->CTRL_SET = LCD_CTRL_EN;
  } else {
    LCD->CTRL_CLR = LCD_CTRL_EN;
  }
#else
  if (enable) {
    LCD->CTRL |= LCD_CTRL_EN;
  } else {
    LCD->CTRL &= ~LCD_CTRL_EN;
  }
#endif // LCD_HAS_SET_CLEAR

#endif // _SILICON_LABS_32B_SERIES_2
}

#if defined(_SILICON_LABS_32B_SERIES_2)
/***************************************************************************//**
 * @brief
 *   Reset the LCD.
 ******************************************************************************/
__STATIC_INLINE void LCD_Reset(void)
{
  LCD->SWRST_SET = LCD_SWRST_SWRST;

  /* Wait for reset to complete. */
  while ((LCD->SWRST & _LCD_SWRST_RESETTING_MASK)) {
  }
}
#endif

/***************************************************************************//**
 * @brief
 *   Enable or disable LCD Animation feature.
 *
 * @param[in] enable
 *   Boolean true enables animation, false disables animation.
 ******************************************************************************/
__STATIC_INLINE void LCD_AnimEnable(bool enable)
{
#if defined(LCD_HAS_SET_CLEAR)
  if (enable) {
    LCD->BACTRL_SET = LCD_BACTRL_AEN;
  } else {
    LCD->BACTRL_CLR = LCD_BACTRL_AEN;
  }
#else
  if (enable) {
    LCD->BACTRL |= LCD_BACTRL_AEN;
  } else {
    LCD->BACTRL &= ~LCD_BACTRL_AEN;
  }
#endif
}

/***************************************************************************//**
 * @brief
 *   Enable or disable the LCD blink.
 *
 * @param[in] enable
 *   Boolean true enables blink, false disables blink.
 ******************************************************************************/
__STATIC_INLINE void LCD_BlinkEnable(bool enable)
{
#if defined(LCD_HAS_SET_CLEAR)
  if (enable) {
    LCD->BACTRL_SET = LCD_BACTRL_BLINKEN;
  } else {
    LCD->BACTRL_CLR = LCD_BACTRL_BLINKEN;
  }
#else
  if (enable) {
    LCD->BACTRL |= LCD_BACTRL_BLINKEN;
  } else {
    LCD->BACTRL &= ~LCD_BACTRL_BLINKEN;
  }
#endif
}

/***************************************************************************//**
 * @brief
 *   Disable all segments while keeping segment state.
 *
 * @param[in] enable
 *   Boolean true clears all segments, boolean false restores all segment lines.
 ******************************************************************************/
__STATIC_INLINE void LCD_BlankEnable(bool enable)
{
#if defined(LCD_HAS_SET_CLEAR)
  if (enable) {
    LCD->BACTRL_SET = LCD_BACTRL_BLANK;
  } else {
    LCD->BACTRL_CLR = LCD_BACTRL_BLANK;
  }
#else
  if (enable) {
    LCD->BACTRL |= LCD_BACTRL_BLANK;
  } else {
    LCD->BACTRL &= ~LCD_BACTRL_BLANK;
  }
#endif
}

/***************************************************************************//**
 * @brief
 *   Enable or disable LCD Frame counter.
 *
 * @param[in] enable
 *   Boolean true enables frame counter, false disables frame counter.
 ******************************************************************************/
__STATIC_INLINE void LCD_FrameCountEnable(bool enable)
{
#if defined(_SILICON_LABS_32B_SERIES_2)
  /* Ensure no internal sync is in progress. */
  LCD_LoadBusyWait();
#endif

#if defined(LCD_HAS_SET_CLEAR)
  if (enable) {
    LCD->BACTRL_SET = LCD_BACTRL_FCEN;
  } else {
    LCD->BACTRL_CLR = LCD_BACTRL_FCEN;
  }
#else
  if (enable) {
    LCD->BACTRL |= LCD_BACTRL_FCEN;
  } else {
    LCD->BACTRL &= ~LCD_BACTRL_FCEN;
  }
#endif
}

#if defined(_SILICON_LABS_32B_SERIES_2)
/***************************************************************************//**
 * @brief
 *   Enable or disable the LCD Display counter.
 *
 * @param[in] enable
 *   Boolean true enables display counter, false disables display counter.
 ******************************************************************************/
__STATIC_INLINE void LCD_DisplayCountEnable(bool enable)
{
  /* Ensure no internal sync is in progress. */
  LCD_LoadBusyWait();

  if (enable) {
    LCD->BACTRL_SET = LCD_BACTRL_DISPLAYCNTEN;
  } else {
    LCD->BACTRL_CLR = LCD_BACTRL_DISPLAYCNTEN;
  }
}
#endif

/***************************************************************************//**
 * @brief
 *   Return the current animation state.
 *
 * @return
 *   Animation state, in range 0-15.
 ******************************************************************************/
__STATIC_INLINE int LCD_AnimState(void)
{
  return (int)(LCD->STATUS & _LCD_STATUS_ASTATE_MASK) >> _LCD_STATUS_ASTATE_SHIFT;
}

/***************************************************************************//**
 * @brief
 *   Return the current blink state.
 *
 * @return
 *   Return value is 1 if segments are enabled, 0 if disabled.
 ******************************************************************************/
__STATIC_INLINE int LCD_BlinkState(void)
{
  return (int)(LCD->STATUS & _LCD_STATUS_BLINK_MASK) >> _LCD_STATUS_BLINK_SHIFT;
}

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
/***************************************************************************//**
 * @brief
 *   When set, LCD registers will not be updated until cleared.
 *
 * @param[in] enable
 *   When enable is true, update is stopped; when false, all registers are
 *   updated.
 ******************************************************************************/
__STATIC_INLINE void LCD_FreezeEnable(bool enable)
{
  if (enable) {
    LCD->FREEZE = LCD_FREEZE_REGFREEZE_FREEZE;
  } else {
    LCD->FREEZE = LCD_FREEZE_REGFREEZE_UPDATE;
  }
}
#endif

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
/***************************************************************************//**
 * @brief
 *   Return SYNCBUSY bits, indicating which registers have pending updates.
 *
 * @return
 *   Bit fields for LCD registers that have pending updates.
 ******************************************************************************/
__STATIC_INLINE uint32_t LCD_SyncBusyGet(void)
{
  return LCD->SYNCBUSY;
}
#endif

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
/***************************************************************************//**
 * @brief
 *   Poll LCD SYNCBUSY flags until the flag has been cleared.
 *
 * @param[in] flags
 *   Bit fields for LCD registers that will be updated before we continue.
 ******************************************************************************/
__STATIC_INLINE void LCD_SyncBusyDelay(uint32_t flags)
{
  while (LCD->SYNCBUSY & flags) ;
}
#endif

#if defined(_SILICON_LABS_32B_SERIES_2)
/***************************************************************************//**
 * @brief
 *   Start the synchronization process.
 *
 * @param[in] autoload
 *   Flag indicating if the synchronization is started manually with CMD.LOAD
 *   (false) or if the synchronization is managed automatically by Auto Load
 *   (true).
 *
 * @param[in] load_addr
 *   Address which will start the synchronization from CLK_BUS to CLK_PER
 *   when Auto Load is selected. This argument has no effect if 'autoload' is
 *   false.
 ******************************************************************************/
__STATIC_INLINE void LCD_SyncStart(bool autoload, LCD_LoadAddr_TypeDef load_addr)
{
  /* Ensure no synchronization in progress. */
  LCD_LoadBusyWait();

  if (autoload) {
    LCD_Enable(false); /* Ensure LCD disabled before writing WSTATIC fields. */
    LCD_ReadyWait();
    LCD->UPDATECTRL_CLR = _LCD_UPDATECTRL_LOADADDR_MASK;
    LCD->UPDATECTRL |= load_addr;
    LCD->UPDATECTRL_SET = LCD_UPDATECTRL_AUTOLOAD;
    LCD_Enable(true);
  } else {
    /* Start synchronization from HV registers to CLK_PER domain. */
    LCD->CMD = LCD_CMD_LOAD;
  }
}
#endif

#if defined(_SILICON_LABS_32B_SERIES_2)
/***************************************************************************//**
 * @brief
 *   Stop the synchronization process.
 *
 * @param[in] autoload
 *   Flag indicating if the synchronization is stopped manually with CMD.CLEAR
 *   (false) or if the synchronization managed by Auto Load is disabled (true).
 ******************************************************************************/
__STATIC_INLINE void LCD_SyncStop(bool autoload)
{
  if (autoload) {
    /* Autoload cannot be disabled if synchronization in progress. */
    LCD_LoadBusyWait();

    LCD->UPDATECTRL_CLR = LCD_UPDATECTRL_AUTOLOAD;
    LCD->UPDATECTRL_CLR = _LCD_UPDATECTRL_LOADADDR_MASK;
  } else {
    LCD->CMD = LCD_CMD_CLEAR;
  }
}
#endif

/***************************************************************************//**
 * @brief
 *    Get pending LCD interrupt flags.
 *
 * @return
 *   Pending LCD interrupt sources. Returns a set of interrupt flags OR-ed
 *   together for multiple interrupt sources in the LCD module (LCD_IFS_nnn).
 ******************************************************************************/
__STATIC_INLINE uint32_t LCD_IntGet(void)
{
  return LCD->IF;
}

/***************************************************************************//**
 * @brief
 *   Get enabled and pending LCD interrupt flags.
 *
 * @details
 *   Useful for handling more interrupt sources in the same interrupt handler.
 *
 * @note
 *   Event bits are not cleared by the use of this function.
 *
 * @return
 *   Pending and enabled LCD interrupt sources.
 *   Return value is the bitwise AND combination of
 *   - the OR combination of enabled interrupt sources in LCD_IEN_nnn
 *   register (LCD_IEN_nnn) and
 *   - the bitwise OR combination of valid interrupt flags of LCD module
 *   (LCD_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE uint32_t LCD_IntGetEnabled(void)
{
  uint32_t ien;

  /* Store the LCD->IEN in temporary variable to define the explicit order
   * of volatile accesses. */
  ien = LCD->IEN;

  /* Bitwise AND of pending and enabled interrupts */
  return LCD->IF & ien;
}

/***************************************************************************//**
 * @brief
 *    Set one or more pending LCD interrupts from SW.
 *
 * @param[in] flags
 *   LCD interrupt sources to set to pending. Use a set of interrupt flags
 *   OR-ed together to set multiple interrupt sources for the LCD module
 *   (LCD_IFS_nnn).
 ******************************************************************************/
__STATIC_INLINE void LCD_IntSet(uint32_t flags)
{
#if defined(_SILICON_LABS_32B_SERIES_2)
#if defined(LCD_HAS_SET_CLEAR)
  LCD->IF_SET = flags;
#else
  LCD->IF |= flags;
#endif // LCD_HAS_SET_CLEAR
#else
  LCD->IFS = flags;
#endif
}

/***************************************************************************//**
 * @brief
 *    Enable LCD interrupts.
 *
 * @param[in] flags
 *   LCD interrupt sources to enable. Use a set of interrupt flags OR-ed
 *   together to set multiple interrupt sources for LCD module
 *   (LCD_IFS_nnn).
 ******************************************************************************/
__STATIC_INLINE void LCD_IntEnable(uint32_t flags)
{
#if defined(LCD_HAS_SET_CLEAR)
  LCD->IEN_SET = flags;
#else
  LCD->IEN |= flags;
#endif
}

/***************************************************************************//**
 * @brief
 *    Disable LCD interrupts.
 *
 * @param[in] flags
 *   LCD interrupt sources to disable. Use a set of interrupt flags OR-ed
 *   together to disable multiple interrupt sources for LCD module
 *   (LCD_IFS_nnn).
 ******************************************************************************/
__STATIC_INLINE void LCD_IntDisable(uint32_t flags)
{
#if defined(LCD_HAS_SET_CLEAR)
  LCD->IEN_CLR = flags;
#else
  LCD->IEN &= ~flags;
#endif
}

/***************************************************************************//**
 * @brief
 *   Clear one or more interrupt flags.
 *
 * @param[in] flags
 *   LCD interrupt sources to clear. Use a set of interrupt flags OR-ed
 *   together to clear multiple interrupt sources for LCD module
 *   (LCD_IFS_nnn).
 ******************************************************************************/
__STATIC_INLINE void LCD_IntClear(uint32_t flags)
{
#if defined(_SILICON_LABS_32B_SERIES_2)
#if defined(LCD_HAS_SET_CLEAR)
  LCD->IF_CLR = flags;
#else
  LCD->IF &= ~flags;
#endif // LCD_HAS_SET_CLEAR
#else
  LCD->IFC = flags;
#endif
}

#if defined(LCD_CTRL_DSC)
/***************************************************************************//**
 * @brief
 *   Enable or disable LCD Direct Segment Control.
 *
 * @param[in] enable
 *   If true, enables LCD controller Direct Segment Control
 *   Segment and COM line bias levels need to be set explicitly with
 *   LCD_BiasSegmentSet() and LCD_BiasComSet() function calls respectively.
 ******************************************************************************/
__STATIC_INLINE void LCD_DSCEnable(bool enable)
{
#if defined(_SILICON_LABS_32B_SERIES_2)
  LCD_Enable(false); /* Ensure LCD disabled before writing WSTATIC fields. */
  LCD_ReadyWait();
#endif

#if defined(LCD_HAS_SET_CLEAR)
  if (enable) {
    LCD->CTRL_SET = LCD_CTRL_DSC;
  } else {
    LCD->CTRL_CLR = LCD_CTRL_DSC;
  }
#else
  if (enable) {
    LCD->CTRL |= LCD_CTRL_DSC;
  } else {
    LCD->CTRL &= ~LCD_CTRL_DSC;
  }
#endif

#if defined(_SILICON_LABS_32B_SERIES_2)
  LCD_Enable(true);
#endif
}

#endif

/** @} (end addtogroup lcd) */

#ifdef __cplusplus
}
#endif

#endif /* defined(LCD_COUNT) && (LCD_COUNT > 0) */

#endif /* EM_LCD_H */
