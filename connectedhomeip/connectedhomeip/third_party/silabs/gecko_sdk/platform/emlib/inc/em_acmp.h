/***************************************************************************//**
 * @file
 * @brief Analog Comparator (ACMP) peripheral API
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

#ifndef EM_ACMP_H
#define EM_ACMP_H

#include "em_device.h"
#include "em_gpio.h"

#if defined(ACMP_COUNT) && (ACMP_COUNT > 0)

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup acmp
 * @{
 ******************************************************************************/

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

/** Resistor values used for the internal capacitive sense resistor. See
 *  data sheet for your device for details on each resistor value. */
typedef enum {
#if defined(_ACMP_INPUTCTRL_CSRESSEL_MASK)
  acmpResistor0 = _ACMP_INPUTCTRL_CSRESSEL_RES0,   /**< Resistor value 0 */
  acmpResistor1 = _ACMP_INPUTCTRL_CSRESSEL_RES1,   /**< Resistor value 1 */
  acmpResistor2 = _ACMP_INPUTCTRL_CSRESSEL_RES2,   /**< Resistor value 2 */
  acmpResistor3 = _ACMP_INPUTCTRL_CSRESSEL_RES3,   /**< Resistor value 3 */
  acmpResistor4 = _ACMP_INPUTCTRL_CSRESSEL_RES4,   /**< Resistor value 4 */
  acmpResistor5 = _ACMP_INPUTCTRL_CSRESSEL_RES5,   /**< Resistor value 5 */
  acmpResistor6 = _ACMP_INPUTCTRL_CSRESSEL_RES6,   /**< Resistor value 6 */
#else
  acmpResistor0 = _ACMP_INPUTSEL_CSRESSEL_RES0,   /**< Resistor value 0 */
  acmpResistor1 = _ACMP_INPUTSEL_CSRESSEL_RES1,   /**< Resistor value 1 */
  acmpResistor2 = _ACMP_INPUTSEL_CSRESSEL_RES2,   /**< Resistor value 2 */
  acmpResistor3 = _ACMP_INPUTSEL_CSRESSEL_RES3,   /**< Resistor value 3 */
#if defined(_ACMP_INPUTSEL_CSRESSEL_RES4)
  acmpResistor4 = _ACMP_INPUTSEL_CSRESSEL_RES4,   /**< Resistor value 4 */
  acmpResistor5 = _ACMP_INPUTSEL_CSRESSEL_RES5,   /**< Resistor value 5 */
  acmpResistor6 = _ACMP_INPUTSEL_CSRESSEL_RES6,   /**< Resistor value 6 */
  acmpResistor7 = _ACMP_INPUTSEL_CSRESSEL_RES7,   /**< Resistor value 7 */
#endif
#endif
} ACMP_CapsenseResistor_TypeDef;

/** Hysteresis level. See data sheet for your device for details on each
 *  level. */
typedef enum {
#if defined(_ACMP_CTRL_HYSTSEL_MASK)
  acmpHysteresisLevel0 = _ACMP_CTRL_HYSTSEL_HYST0,       /**< Hysteresis level 0 */
  acmpHysteresisLevel1 = _ACMP_CTRL_HYSTSEL_HYST1,       /**< Hysteresis level 1 */
  acmpHysteresisLevel2 = _ACMP_CTRL_HYSTSEL_HYST2,       /**< Hysteresis level 2 */
  acmpHysteresisLevel3 = _ACMP_CTRL_HYSTSEL_HYST3,       /**< Hysteresis level 3 */
  acmpHysteresisLevel4 = _ACMP_CTRL_HYSTSEL_HYST4,       /**< Hysteresis level 4 */
  acmpHysteresisLevel5 = _ACMP_CTRL_HYSTSEL_HYST5,       /**< Hysteresis level 5 */
  acmpHysteresisLevel6 = _ACMP_CTRL_HYSTSEL_HYST6,       /**< Hysteresis level 6 */
  acmpHysteresisLevel7 = _ACMP_CTRL_HYSTSEL_HYST7        /**< Hysteresis level 7 */
#endif
#if defined(_ACMP_HYSTERESIS0_HYST_MASK)
  acmpHysteresisLevel0 = _ACMP_HYSTERESIS0_HYST_HYST0,   /**< Hysteresis level 0 */
  acmpHysteresisLevel1 = _ACMP_HYSTERESIS0_HYST_HYST1,   /**< Hysteresis level 1 */
  acmpHysteresisLevel2 = _ACMP_HYSTERESIS0_HYST_HYST2,   /**< Hysteresis level 2 */
  acmpHysteresisLevel3 = _ACMP_HYSTERESIS0_HYST_HYST3,   /**< Hysteresis level 3 */
  acmpHysteresisLevel4 = _ACMP_HYSTERESIS0_HYST_HYST4,   /**< Hysteresis level 4 */
  acmpHysteresisLevel5 = _ACMP_HYSTERESIS0_HYST_HYST5,   /**< Hysteresis level 5 */
  acmpHysteresisLevel6 = _ACMP_HYSTERESIS0_HYST_HYST6,   /**< Hysteresis level 6 */
  acmpHysteresisLevel7 = _ACMP_HYSTERESIS0_HYST_HYST7,   /**< Hysteresis level 7 */
  acmpHysteresisLevel8 = _ACMP_HYSTERESIS0_HYST_HYST8,   /**< Hysteresis level 8 */
  acmpHysteresisLevel9 = _ACMP_HYSTERESIS0_HYST_HYST9,   /**< Hysteresis level 9 */
  acmpHysteresisLevel10 = _ACMP_HYSTERESIS0_HYST_HYST10, /**< Hysteresis level 10 */
  acmpHysteresisLevel11 = _ACMP_HYSTERESIS0_HYST_HYST11, /**< Hysteresis level 11 */
  acmpHysteresisLevel12 = _ACMP_HYSTERESIS0_HYST_HYST12, /**< Hysteresis level 12 */
  acmpHysteresisLevel13 = _ACMP_HYSTERESIS0_HYST_HYST13, /**< Hysteresis level 13 */
  acmpHysteresisLevel14 = _ACMP_HYSTERESIS0_HYST_HYST14, /**< Hysteresis level 14 */
  acmpHysteresisLevel15 = _ACMP_HYSTERESIS0_HYST_HYST15, /**< Hysteresis level 15 */
#endif
#if defined(_ACMP_CFG_HYST_MASK)
  acmpHysteresisDisabled = _ACMP_CFG_HYST_DISABLED,   /**< Mode DISABLED for ACMP_CFG */
  acmpHysteresis10Sym = _ACMP_CFG_HYST_SYM10MV,     /**< Mode HYST10SYM for ACMP_CFG */
  acmpHysteresis20Sym = _ACMP_CFG_HYST_SYM20MV,     /**< Mode HYST20SYM for ACMP_CFG */
  acmpHysteresis30Sym = _ACMP_CFG_HYST_SYM30MV,     /**< Mode HYST30SYM for ACMP_CFG */
  acmpHysteresis10Pos = _ACMP_CFG_HYST_POS10MV,     /**< Mode HYST10POS for ACMP_CFG */
  acmpHysteresis20Pos = _ACMP_CFG_HYST_POS20MV,     /**< Mode HYST20POS for ACMP_CFG */
  acmpHysteresis30Pos = _ACMP_CFG_HYST_POS30MV,     /**< Mode HYST30POS for ACMP_CFG */
  acmpHysteresis10Neg = _ACMP_CFG_HYST_NEG10MV,     /**< Mode HYST10NEG for ACMP_CFG */
  acmpHysteresis20Neg = _ACMP_CFG_HYST_NEG20MV,     /**< Mode HYST20NEG for ACMP_CFG */
  acmpHysteresis30Neg = _ACMP_CFG_HYST_NEG30MV,     /**< Mode HYST30NEG for ACMP_CFG */
#endif
} ACMP_HysteresisLevel_TypeDef;

#if defined(_ACMP_CTRL_WARMTIME_MASK)
/** ACMP warmup time. The delay is measured in HFPERCLK / HFPERCCLK cycles and should
 *  be at least 10 us. */
typedef enum {
  /** 4 cycles warmup */
  acmpWarmTime4   = _ACMP_CTRL_WARMTIME_4CYCLES,
  /** 8 cycles warmup */
  acmpWarmTime8   = _ACMP_CTRL_WARMTIME_8CYCLES,
  /** 16 cycles warmup */
  acmpWarmTime16  = _ACMP_CTRL_WARMTIME_16CYCLES,
  /** 32 cycles warmup */
  acmpWarmTime32  = _ACMP_CTRL_WARMTIME_32CYCLES,
  /** 64 cycles warmup */
  acmpWarmTime64  = _ACMP_CTRL_WARMTIME_64CYCLES,
  /** 128 cycles warmup */
  acmpWarmTime128 = _ACMP_CTRL_WARMTIME_128CYCLES,
  /** 256 cycles warmup */
  acmpWarmTime256 = _ACMP_CTRL_WARMTIME_256CYCLES,
  /** 512 cycles warmup */
  acmpWarmTime512 = _ACMP_CTRL_WARMTIME_512CYCLES
} ACMP_WarmTime_TypeDef;
#endif

#if defined(_ACMP_CTRL_INPUTRANGE_MASK) \
  || defined(_ACMP_CFG_INPUTRANGE_MASK)
/**
 * Adjust ACMP performance for a given input voltage range.
 */
typedef enum {
#if defined(_ACMP_CTRL_INPUTRANGE_MASK)
  acmpInputRangeFull = _ACMP_CTRL_INPUTRANGE_FULL,      /**< Input can be from 0 to VDD. */
  acmpInputRangeHigh = _ACMP_CTRL_INPUTRANGE_GTVDDDIV2, /**< Input will always be greater than VDD/2. */
  acmpInputRangeLow  = _ACMP_CTRL_INPUTRANGE_LTVDDDIV2  /**< Input will always be less than VDD/2. */
#elif defined(_ACMP_CFG_INPUTRANGE_MASK)
  acmpInputRangeFull    = _ACMP_CFG_INPUTRANGE_FULL,    /**< Input can be from 0 to VDD. */
  acmpInputRangeReduced = _ACMP_CFG_INPUTRANGE_REDUCED, /**< Input can be from 0 to VDD-0.7 V. */
#endif
} ACMP_InputRange_TypeDef;
#endif

#if defined(_ACMP_CTRL_PWRSEL_MASK)
/**
 * ACMP Power source.
 */
typedef enum {
  acmpPowerSourceAvdd      = _ACMP_CTRL_PWRSEL_AVDD,    /**< Power ACMP using the AVDD supply. */
#if defined(_ACMP_CTRL_PWRSEL_DVDD)
  acmpPowerSourceDvdd      = _ACMP_CTRL_PWRSEL_DVDD,    /**< Power ACMP using the DVDD supply. */
#endif
  acmpPowerSourceIOVdd0    = _ACMP_CTRL_PWRSEL_IOVDD0,  /**< Power ACMP using the IOVDD/IOVDD0 supply. */
  acmpPowerSourceIOVdd1    = _ACMP_CTRL_PWRSEL_IOVDD1,  /**< Power ACMP using the IOVDD1 supply (if the part has two I/O voltages). */
} ACMP_PowerSource_TypeDef;
#endif

#if defined(_ACMP_CTRL_ACCURACY_MASK) \
  || defined(_ACMP_CFG_ACCURACY_MASK)
/**
 * ACMP accuracy mode.
 */
typedef enum {
#if defined(_ACMP_CTRL_ACCURACY_MASK)
  acmpAccuracyLow = _ACMP_CTRL_ACCURACY_LOW,   /**< Low-accuracy mode which consumes less current. */
  acmpAccuracyHigh = _ACMP_CTRL_ACCURACY_HIGH  /**< High-accuracy mode which consumes more current. */
#elif defined(_ACMP_CFG_ACCURACY_MASK)
  acmpAccuracyLow = _ACMP_CFG_ACCURACY_LOW,   /**< Low-accuracy mode which consumes less current. */
  acmpAccuracyHigh = _ACMP_CFG_ACCURACY_HIGH  /**< High-accuracy mode which consumes more current. */
#endif
} ACMP_Accuracy_TypeDef;
#endif

#if defined(_ACMP_INPUTSEL_VASEL_MASK)
/** ACMP input to the VA divider. This enumeration is used to select the input for
 *  the VA Divider. */
typedef enum {
  acmpVAInputVDD         = _ACMP_INPUTSEL_VASEL_VDD,         /**< Select VDD for the VA divider. */
  acmpVAInputAPORT2YCH0  = _ACMP_INPUTSEL_VASEL_APORT2YCH0,  /**< Select APORT2Y CHannel 0 for the VA divider. */
  acmpVAInputAPORT2YCH2  = _ACMP_INPUTSEL_VASEL_APORT2YCH2,  /**< Select APORT2Y CHannel 2 for the VA divider. */
  acmpVAInputAPORT2YCH4  = _ACMP_INPUTSEL_VASEL_APORT2YCH4,  /**< Select APORT2Y CHannel 4 for the VA divider. */
  acmpVAInputAPORT2YCH6  = _ACMP_INPUTSEL_VASEL_APORT2YCH6,  /**< Select APORT2Y CHannel 6 for the VA divider. */
  acmpVAInputAPORT2YCH8  = _ACMP_INPUTSEL_VASEL_APORT2YCH8,  /**< Select APORT2Y CHannel 8 for the VA divider. */
  acmpVAInputAPORT2YCH10 = _ACMP_INPUTSEL_VASEL_APORT2YCH10, /**< Select APORT2Y CHannel 10 for the VA divider. */
  acmpVAInputAPORT2YCH12 = _ACMP_INPUTSEL_VASEL_APORT2YCH12, /**< Select APORT2Y CHannel 12 for the VA divider. */
  acmpVAInputAPORT2YCH14 = _ACMP_INPUTSEL_VASEL_APORT2YCH14, /**< Select APORT2Y CHannel 14 for the VA divider. */
  acmpVAInputAPORT2YCH16 = _ACMP_INPUTSEL_VASEL_APORT2YCH16, /**< Select APORT2Y CHannel 16 for the VA divider. */
  acmpVAInputAPORT2YCH18 = _ACMP_INPUTSEL_VASEL_APORT2YCH18, /**< Select APORT2Y CHannel 18 for the VA divider. */
  acmpVAInputAPORT2YCH20 = _ACMP_INPUTSEL_VASEL_APORT2YCH20, /**< Select APORT2Y CHannel 20 for the VA divider. */
  acmpVAInputAPORT2YCH22 = _ACMP_INPUTSEL_VASEL_APORT2YCH22, /**< Select APORT2Y CHannel 22 for the VA divider. */
  acmpVAInputAPORT2YCH24 = _ACMP_INPUTSEL_VASEL_APORT2YCH24, /**< Select APORT2Y CHannel 24 for the VA divider. */
  acmpVAInputAPORT2YCH26 = _ACMP_INPUTSEL_VASEL_APORT2YCH26, /**< Select APORT2Y CHannel 26 for the VA divider. */
  acmpVAInputAPORT2YCH28 = _ACMP_INPUTSEL_VASEL_APORT2YCH28, /**< Select APORT2Y CHannel 28 for the VA divider. */
  acmpVAInputAPORT2YCH30 = _ACMP_INPUTSEL_VASEL_APORT2YCH30, /**< Select APORT2Y CHannel 30 for the VA divider. */
  acmpVAInputAPORT1XCH0  = _ACMP_INPUTSEL_VASEL_APORT1XCH0,  /**< Select APORT1X CHannel 0 for the VA divider. */
  acmpVAInputAPORT1YCH1  = _ACMP_INPUTSEL_VASEL_APORT1YCH1,  /**< Select APORT1Y CHannel 1 for the VA divider. */
  acmpVAInputAPORT1XCH2  = _ACMP_INPUTSEL_VASEL_APORT1XCH2,  /**< Select APORT1X CHannel 2 for the VA divider. */
  acmpVAInputAPORT1YCH3  = _ACMP_INPUTSEL_VASEL_APORT1YCH3,  /**< Select APORT1Y CHannel 3 for the VA divider. */
  acmpVAInputAPORT1XCH4  = _ACMP_INPUTSEL_VASEL_APORT1XCH4,  /**< Select APORT1X CHannel 4 for the VA divider. */
  acmpVAInputAPORT1YCH5  = _ACMP_INPUTSEL_VASEL_APORT1YCH5,  /**< Select APORT1Y CHannel 5 for the VA divider. */
  acmpVAInputAPORT1XCH6  = _ACMP_INPUTSEL_VASEL_APORT1XCH6,  /**< Select APORT1X CHannel 6 for the VA divider. */
  acmpVAInputAPORT1YCH7  = _ACMP_INPUTSEL_VASEL_APORT1YCH7,  /**< Select APORT1Y CHannel 7 for the VA divider. */
  acmpVAInputAPORT1XCH8  = _ACMP_INPUTSEL_VASEL_APORT1XCH8,  /**< Select APORT1X CHannel 8 for the VA divider. */
  acmpVAInputAPORT1YCH9  = _ACMP_INPUTSEL_VASEL_APORT1YCH9,  /**< Select APORT1Y CHannel 9 for the VA divider. */
  acmpVAInputAPORT1XCH10 = _ACMP_INPUTSEL_VASEL_APORT1XCH10, /**< Select APORT1X CHannel 10 for the VA divider. */
  acmpVAInputAPORT1YCH11 = _ACMP_INPUTSEL_VASEL_APORT1YCH11, /**< Select APORT1Y CHannel 11 for the VA divider. */
  acmpVAInputAPORT1XCH12 = _ACMP_INPUTSEL_VASEL_APORT1XCH12, /**< Select APORT1X CHannel 12 for the VA divider. */
  acmpVAInputAPORT1YCH13 = _ACMP_INPUTSEL_VASEL_APORT1YCH13, /**< Select APORT1Y CHannel 13 for the VA divider. */
  acmpVAInputAPORT1XCH14 = _ACMP_INPUTSEL_VASEL_APORT1XCH14, /**< Select APORT1X CHannel 14 for the VA divider. */
  acmpVAInputAPORT1YCH15 = _ACMP_INPUTSEL_VASEL_APORT1YCH15, /**< Select APORT1Y CHannel 15 for the VA divider. */
  acmpVAInputAPORT1XCH16 = _ACMP_INPUTSEL_VASEL_APORT1XCH16, /**< Select APORT1X CHannel 16 for the VA divider. */
  acmpVAInputAPORT1YCH17 = _ACMP_INPUTSEL_VASEL_APORT1YCH17, /**< Select APORT1Y CHannel 17 for the VA divider. */
  acmpVAInputAPORT1XCH18 = _ACMP_INPUTSEL_VASEL_APORT1XCH18, /**< Select APORT1X CHannel 18 for the VA divider. */
  acmpVAInputAPORT1YCH19 = _ACMP_INPUTSEL_VASEL_APORT1YCH19, /**< Select APORT1Y CHannel 19 for the VA divider. */
  acmpVAInputAPORT1XCH20 = _ACMP_INPUTSEL_VASEL_APORT1XCH20, /**< Select APORT1X CHannel 20 for the VA divider. */
  acmpVAInputAPORT1YCH21 = _ACMP_INPUTSEL_VASEL_APORT1YCH21, /**< Select APORT1Y CHannel 21 for the VA divider. */
  acmpVAInputAPORT1XCH22 = _ACMP_INPUTSEL_VASEL_APORT1XCH22, /**< Select APORT1X CHannel 22 for the VA divider. */
  acmpVAInputAPORT1YCH23 = _ACMP_INPUTSEL_VASEL_APORT1YCH23, /**< Select APORT1Y CHannel 23 for the VA divider. */
  acmpVAInputAPORT1XCH24 = _ACMP_INPUTSEL_VASEL_APORT1XCH24, /**< Select APORT1X CHannel 24 for the VA divider. */
  acmpVAInputAPORT1YCH25 = _ACMP_INPUTSEL_VASEL_APORT1YCH25, /**< Select APORT1Y CHannel 25 for the VA divider. */
  acmpVAInputAPORT1XCH26 = _ACMP_INPUTSEL_VASEL_APORT1XCH26, /**< Select APORT1X CHannel 26 for the VA divider. */
  acmpVAInputAPORT1YCH27 = _ACMP_INPUTSEL_VASEL_APORT1YCH27, /**< Select APORT1Y CHannel 27 for the VA divider. */
  acmpVAInputAPORT1XCH28 = _ACMP_INPUTSEL_VASEL_APORT1XCH28, /**< Select APORT1X CHannel 28 for the VA divider. */
  acmpVAInputAPORT1YCH29 = _ACMP_INPUTSEL_VASEL_APORT1YCH29, /**< Select APORT1Y CHannel 29 for the VA divider. */
  acmpVAInputAPORT1XCH30 = _ACMP_INPUTSEL_VASEL_APORT1XCH30, /**< Select APORT1X CHannel 30 for the VA divider. */
  acmpVAInputAPORT1YCH31 = _ACMP_INPUTSEL_VASEL_APORT1YCH31  /**< Select APORT1Y CHannel 31 for the VA divider. */
} ACMP_VAInput_TypeDef;
#endif

#if defined(_ACMP_INPUTSEL_VBSEL_MASK)
/**
 * ACMP input to the VB divider. This enumeration is used to select the input for
 * the VB divider.
 */
typedef enum {
  acmpVBInput1V25 = _ACMP_INPUTSEL_VBSEL_1V25, /**< Mode 1V25 for ACMP_INPUTSEL */
  acmpVBInput2V5  = _ACMP_INPUTSEL_VBSEL_2V5   /**< Mode 2V5 for ACMP_INPUTSEL */
} ACMP_VBInput_TypeDef;
#endif

#if defined(_ACMP_INPUTSEL_VLPSEL_MASK)
/**
 * ACMP Low-Power Input Selection.
 */
typedef enum {
  acmpVLPInputVADIV = _ACMP_INPUTSEL_VLPSEL_VADIV, /**< Mode VADIV for ACMP_INPUTSEL */
  acmpVLPInputVBDIV = _ACMP_INPUTSEL_VLPSEL_VBDIV  /**< Mode VBDIV for ACMP_INPUTSEL */
} ACMP_VLPInput_Typedef;
#endif

#if defined(_ACMP_INPUTCTRL_MASK)
/** ACMP Input Selection. */
typedef enum {
  acmpInputVSS            = _ACMP_INPUTCTRL_POSSEL_VSS,            /**< Select VSS. */
  acmpInputVREFDIVAVDD    = _ACMP_INPUTCTRL_POSSEL_VREFDIVAVDD,    /**< Select Divided AVDD. */
  acmpInputVREFDIVAVDDLP  = _ACMP_INPUTCTRL_POSSEL_VREFDIVAVDDLP,  /**< Select Low-Power Divided AVDD. */
  acmpInputVREFDIV1V25    = _ACMP_INPUTCTRL_POSSEL_VREFDIV1V25,    /**< Select Divided 1V25 reference. */
  acmpInputVREFDIV1V25LP  = _ACMP_INPUTCTRL_POSSEL_VREFDIV1V25LP,  /**< Select Low-power Divided 1V25 reference. */
  acmpInputVREFDIV2V5     = _ACMP_INPUTCTRL_POSSEL_VREFDIV2V5,     /**< Select Divided 2V5 reference. */
  acmpInputVREFDIV2V5LP   = _ACMP_INPUTCTRL_POSSEL_VREFDIV2V5LP,   /**< Select Low-power Divided 2V5 reference. */
  acmpInputVSENSE01DIV4   = _ACMP_INPUTCTRL_POSSEL_VSENSE01DIV4,   /**< Select VSENSE0 divided by 4. */
  acmpInputVSENSE01DIV4LP = _ACMP_INPUTCTRL_POSSEL_VSENSE01DIV4LP, /**< Select Low-power VSENSE0 divided by 4. */
  acmpInputVSENSE11DIV4   = _ACMP_INPUTCTRL_POSSEL_VSENSE11DIV4,   /**< VSENSE1 divided by 4. */
  acmpInputVSENSE11DIV4LP = _ACMP_INPUTCTRL_POSSEL_VSENSE11DIV4LP, /**< Low-power VSENSE1 divided by 4. */
  acmpInputCAPSENSE       = _ACMP_INPUTCTRL_NEGSEL_CAPSENSE,       /**< Select Low-Power Divided AVDD. */
#if defined(_ACMP_INPUTCTRL_POSSEL_VDACOUT0)
  acmpInputVDACOUT0       = _ACMP_INPUTCTRL_POSSEL_VDACOUT0, /**< Select VDAC0 channel 0 output. */
  acmpInputVDACOUT1       = _ACMP_INPUTCTRL_POSSEL_VDACOUT1, /**< Select VDAC0 channel 1 output. */
#endif
  acmpInputPA0            = _ACMP_INPUTCTRL_POSSEL_PA0,  /**< Select Port A Pin0. */
  acmpInputPA1            = _ACMP_INPUTCTRL_POSSEL_PA1,  /**< Select Port A Pin1. */
  acmpInputPA2            = _ACMP_INPUTCTRL_POSSEL_PA2,  /**< Select Port A Pin2. */
  acmpInputPA3            = _ACMP_INPUTCTRL_POSSEL_PA3,  /**< Select Port A Pin3. */
  acmpInputPA4            = _ACMP_INPUTCTRL_POSSEL_PA4,  /**< Select Port A Pin4. */
  acmpInputPA5            = _ACMP_INPUTCTRL_POSSEL_PA5,  /**< Select Port A Pin5. */
  acmpInputPA6            = _ACMP_INPUTCTRL_POSSEL_PA6,  /**< Select Port A Pin6. */
  acmpInputPA7            = _ACMP_INPUTCTRL_POSSEL_PA7,  /**< Select Port A Pin7. */
  acmpInputPA8            = _ACMP_INPUTCTRL_POSSEL_PA8,  /**< Select Port A Pin8. */
  acmpInputPA9            = _ACMP_INPUTCTRL_POSSEL_PA9,  /**< Select Port A Pin9. */
  acmpInputPA10           = _ACMP_INPUTCTRL_POSSEL_PA10, /**< Select Port A Pin10. */
  acmpInputPA11           = _ACMP_INPUTCTRL_POSSEL_PA11, /**< Select Port A Pin11. */
  acmpInputPA12           = _ACMP_INPUTCTRL_POSSEL_PA12, /**< Select Port A Pin12. */
  acmpInputPA13           = _ACMP_INPUTCTRL_POSSEL_PA13, /**< Select Port A Pin13. */
  acmpInputPA14           = _ACMP_INPUTCTRL_POSSEL_PA14, /**< Select Port A Pin14. */
  acmpInputPA15           = _ACMP_INPUTCTRL_POSSEL_PA15, /**< Select Port A Pin15. */
  acmpInputPB0            = _ACMP_INPUTCTRL_POSSEL_PB0,  /**< Select Port B Pin0. */
  acmpInputPB1            = _ACMP_INPUTCTRL_POSSEL_PB1,  /**< Select Port B Pin1. */
  acmpInputPB2            = _ACMP_INPUTCTRL_POSSEL_PB2,  /**< Select Port B Pin2. */
  acmpInputPB3            = _ACMP_INPUTCTRL_POSSEL_PB3,  /**< Select Port B Pin3. */
  acmpInputPB4            = _ACMP_INPUTCTRL_POSSEL_PB4,  /**< Select Port B Pin4. */
  acmpInputPB5            = _ACMP_INPUTCTRL_POSSEL_PB5,  /**< Select Port B Pin5. */
  acmpInputPB6            = _ACMP_INPUTCTRL_POSSEL_PB6,  /**< Select Port B Pin6. */
  acmpInputPB7            = _ACMP_INPUTCTRL_POSSEL_PB7,  /**< Select Port B Pin7. */
  acmpInputPB8            = _ACMP_INPUTCTRL_POSSEL_PB8,  /**< Select Port B Pin8. */
  acmpInputPB9            = _ACMP_INPUTCTRL_POSSEL_PB9,  /**< Select Port B Pin9. */
  acmpInputPB10           = _ACMP_INPUTCTRL_POSSEL_PB10, /**< Select Port B Pin10. */
  acmpInputPB11           = _ACMP_INPUTCTRL_POSSEL_PB11, /**< Select Port B Pin11. */
  acmpInputPB12           = _ACMP_INPUTCTRL_POSSEL_PB12, /**< Select Port B Pin12. */
  acmpInputPB13           = _ACMP_INPUTCTRL_POSSEL_PB13, /**< Select Port B Pin13. */
  acmpInputPB14           = _ACMP_INPUTCTRL_POSSEL_PB14, /**< Select Port B Pin14. */
  acmpInputPB15           = _ACMP_INPUTCTRL_POSSEL_PB15, /**< Select Port B Pin15. */
  acmpInputPC0            = _ACMP_INPUTCTRL_POSSEL_PC0,  /**< Select Port C Pin0. */
  acmpInputPC1            = _ACMP_INPUTCTRL_POSSEL_PC1,  /**< Select Port C Pin1. */
  acmpInputPC2            = _ACMP_INPUTCTRL_POSSEL_PC2,  /**< Select Port C Pin2. */
  acmpInputPC3            = _ACMP_INPUTCTRL_POSSEL_PC3,  /**< Select Port C Pin3. */
  acmpInputPC4            = _ACMP_INPUTCTRL_POSSEL_PC4,  /**< Select Port C Pin4. */
  acmpInputPC5            = _ACMP_INPUTCTRL_POSSEL_PC5,  /**< Select Port C Pin5. */
  acmpInputPC6            = _ACMP_INPUTCTRL_POSSEL_PC6,  /**< Select Port C Pin6. */
  acmpInputPC7            = _ACMP_INPUTCTRL_POSSEL_PC7,  /**< Select Port C Pin7. */
  acmpInputPC8            = _ACMP_INPUTCTRL_POSSEL_PC8,  /**< Select Port C Pin8. */
  acmpInputPC9            = _ACMP_INPUTCTRL_POSSEL_PC9,  /**< Select Port C Pin9. */
  acmpInputPC10           = _ACMP_INPUTCTRL_POSSEL_PC10, /**< Select Port C Pin10. */
  acmpInputPC11           = _ACMP_INPUTCTRL_POSSEL_PC11, /**< Select Port C Pin11. */
  acmpInputPC12           = _ACMP_INPUTCTRL_POSSEL_PC12, /**< Select Port C Pin12. */
  acmpInputPC13           = _ACMP_INPUTCTRL_POSSEL_PC13, /**< Select Port C Pin13. */
  acmpInputPC14           = _ACMP_INPUTCTRL_POSSEL_PC14, /**< Select Port C Pin14. */
  acmpInputPC15           = _ACMP_INPUTCTRL_POSSEL_PC15, /**< Select Port C Pin15. */
  acmpInputPD0            = _ACMP_INPUTCTRL_POSSEL_PD0,  /**< Select Port D Pin0. */
  acmpInputPD1            = _ACMP_INPUTCTRL_POSSEL_PD1,  /**< Select Port D Pin1. */
  acmpInputPD2            = _ACMP_INPUTCTRL_POSSEL_PD2,  /**< Select Port D Pin2. */
  acmpInputPD3            = _ACMP_INPUTCTRL_POSSEL_PD3,  /**< Select Port D Pin3. */
  acmpInputPD4            = _ACMP_INPUTCTRL_POSSEL_PD4,  /**< Select Port D Pin4. */
  acmpInputPD5            = _ACMP_INPUTCTRL_POSSEL_PD5,  /**< Select Port D Pin5. */
  acmpInputPD6            = _ACMP_INPUTCTRL_POSSEL_PD6,  /**< Select Port D Pin6. */
  acmpInputPD7            = _ACMP_INPUTCTRL_POSSEL_PD7,  /**< Select Port D Pin7. */
  acmpInputPD8            = _ACMP_INPUTCTRL_POSSEL_PD8,  /**< Select Port D Pin8. */
  acmpInputPD9            = _ACMP_INPUTCTRL_POSSEL_PD9,  /**< Select Port D Pin9. */
  acmpInputPD10           = _ACMP_INPUTCTRL_POSSEL_PD10, /**< Select Port D Pin10. */
  acmpInputPD11           = _ACMP_INPUTCTRL_POSSEL_PD11, /**< Select Port D Pin11. */
  acmpInputPD12           = _ACMP_INPUTCTRL_POSSEL_PD12, /**< Select Port D Pin12. */
  acmpInputPD13           = _ACMP_INPUTCTRL_POSSEL_PD13, /**< Select Port D Pin13. */
  acmpInputPD14           = _ACMP_INPUTCTRL_POSSEL_PD14, /**< Select Port D Pin14. */
  acmpInputPD15           = _ACMP_INPUTCTRL_POSSEL_PD15, /**< Select Port D Pin15. */
} ACMP_Channel_TypeDef;
#elif defined(_ACMP_INPUTSEL_POSSEL_APORT0XCH0)
/** ACMP Input Selection. */
typedef enum {
  acmpInputAPORT0XCH0  = _ACMP_INPUTSEL_POSSEL_APORT0XCH0,  /**< Select Dedicated APORT0X CHannel 0. */
  acmpInputAPORT0XCH1  = _ACMP_INPUTSEL_POSSEL_APORT0XCH1,  /**< Select Dedicated APORT0X CHannel 1. */
  acmpInputAPORT0XCH2  = _ACMP_INPUTSEL_POSSEL_APORT0XCH2,  /**< Select Dedicated APORT0X CHannel 2. */
  acmpInputAPORT0XCH3  = _ACMP_INPUTSEL_POSSEL_APORT0XCH3,  /**< Select Dedicated APORT0X CHannel 3. */
  acmpInputAPORT0XCH4  = _ACMP_INPUTSEL_POSSEL_APORT0XCH4,  /**< Select Dedicated APORT0X CHannel 4. */
  acmpInputAPORT0XCH5  = _ACMP_INPUTSEL_POSSEL_APORT0XCH5,  /**< Select Dedicated APORT0X CHannel 5. */
  acmpInputAPORT0XCH6  = _ACMP_INPUTSEL_POSSEL_APORT0XCH6,  /**< Select Dedicated APORT0X CHannel 6. */
  acmpInputAPORT0XCH7  = _ACMP_INPUTSEL_POSSEL_APORT0XCH7,  /**< Select Dedicated APORT0X CHannel 7. */
  acmpInputAPORT0XCH8  = _ACMP_INPUTSEL_POSSEL_APORT0XCH8,  /**< Select Dedicated APORT0X CHannel 8. */
  acmpInputAPORT0XCH9  = _ACMP_INPUTSEL_POSSEL_APORT0XCH9,  /**< Select Dedicated APORT0X CHannel 9. */
  acmpInputAPORT0XCH10 = _ACMP_INPUTSEL_POSSEL_APORT0XCH10, /**< Select Dedicated APORT0X CHannel 10. */
  acmpInputAPORT0XCH11 = _ACMP_INPUTSEL_POSSEL_APORT0XCH11, /**< Select Dedicated APORT0X CHannel 11. */
  acmpInputAPORT0XCH12 = _ACMP_INPUTSEL_POSSEL_APORT0XCH12, /**< Select Dedicated APORT0X CHannel 12. */
  acmpInputAPORT0XCH13 = _ACMP_INPUTSEL_POSSEL_APORT0XCH13, /**< Select Dedicated APORT0X CHannel 13. */
  acmpInputAPORT0XCH14 = _ACMP_INPUTSEL_POSSEL_APORT0XCH14, /**< Select Dedicated APORT0X CHannel 14. */
  acmpInputAPORT0XCH15 = _ACMP_INPUTSEL_POSSEL_APORT0XCH15, /**< Select Dedicated APORT0X CHannel 15. */
  acmpInputAPORT0YCH0  = _ACMP_INPUTSEL_POSSEL_APORT0YCH0,  /**< Select Dedicated APORT0Y CHannel 0. */
  acmpInputAPORT0YCH1  = _ACMP_INPUTSEL_POSSEL_APORT0YCH1,  /**< Select Dedicated APORT0Y CHannel 1. */
  acmpInputAPORT0YCH2  = _ACMP_INPUTSEL_POSSEL_APORT0YCH2,  /**< Select Dedicated APORT0Y CHannel 2. */
  acmpInputAPORT0YCH3  = _ACMP_INPUTSEL_POSSEL_APORT0YCH3,  /**< Select Dedicated APORT0Y CHannel 3. */
  acmpInputAPORT0YCH4  = _ACMP_INPUTSEL_POSSEL_APORT0YCH4,  /**< Select Dedicated APORT0Y CHannel 4. */
  acmpInputAPORT0YCH5  = _ACMP_INPUTSEL_POSSEL_APORT0YCH5,  /**< Select Dedicated APORT0Y CHannel 5. */
  acmpInputAPORT0YCH6  = _ACMP_INPUTSEL_POSSEL_APORT0YCH6,  /**< Select Dedicated APORT0Y CHannel 6. */
  acmpInputAPORT0YCH7  = _ACMP_INPUTSEL_POSSEL_APORT0YCH7,  /**< Select Dedicated APORT0Y CHannel 7. */
  acmpInputAPORT0YCH8  = _ACMP_INPUTSEL_POSSEL_APORT0YCH8,  /**< Select Dedicated APORT0Y CHannel 8. */
  acmpInputAPORT0YCH9  = _ACMP_INPUTSEL_POSSEL_APORT0YCH9,  /**< Select Dedicated APORT0Y CHannel 9. */
  acmpInputAPORT0YCH10 = _ACMP_INPUTSEL_POSSEL_APORT0YCH10, /**< Select Dedicated APORT0Y CHannel 10. */
  acmpInputAPORT0YCH11 = _ACMP_INPUTSEL_POSSEL_APORT0YCH11, /**< Select Dedicated APORT0Y CHannel 11. */
  acmpInputAPORT0YCH12 = _ACMP_INPUTSEL_POSSEL_APORT0YCH12, /**< Select Dedicated APORT0Y CHannel 12. */
  acmpInputAPORT0YCH13 = _ACMP_INPUTSEL_POSSEL_APORT0YCH13, /**< Select Dedicated APORT0Y CHannel 13. */
  acmpInputAPORT0YCH14 = _ACMP_INPUTSEL_POSSEL_APORT0YCH14, /**< Select Dedicated APORT0Y CHannel 14. */
  acmpInputAPORT0YCH15 = _ACMP_INPUTSEL_POSSEL_APORT0YCH15, /**< Select Dedicated APORT0Y CHannel 15. */
  acmpInputAPORT1XCH0  = _ACMP_INPUTSEL_POSSEL_APORT1XCH0,  /**< Select Dedicated APORT1X CHannel 0. */
  acmpInputAPORT1YCH1  = _ACMP_INPUTSEL_POSSEL_APORT1YCH1,  /**< Select Dedicated APORT1Y CHannel 1. */
  acmpInputAPORT1XCH2  = _ACMP_INPUTSEL_POSSEL_APORT1XCH2,  /**< Select Dedicated APORT1X CHannel 2. */
  acmpInputAPORT1YCH3  = _ACMP_INPUTSEL_POSSEL_APORT1YCH3,  /**< Select Dedicated APORT1Y CHannel 3. */
  acmpInputAPORT1XCH4  = _ACMP_INPUTSEL_POSSEL_APORT1XCH4,  /**< Select Dedicated APORT1X CHannel 4. */
  acmpInputAPORT1YCH5  = _ACMP_INPUTSEL_POSSEL_APORT1YCH5,  /**< Select Dedicated APORT1Y CHannel 5. */
  acmpInputAPORT1XCH6  = _ACMP_INPUTSEL_POSSEL_APORT1XCH6,  /**< Select Dedicated APORT1X CHannel 6. */
  acmpInputAPORT1YCH7  = _ACMP_INPUTSEL_POSSEL_APORT1YCH7,  /**< Select Dedicated APORT1Y CHannel 7. */
  acmpInputAPORT1XCH8  = _ACMP_INPUTSEL_POSSEL_APORT1XCH8,  /**< Select Dedicated APORT1X CHannel 8. */
  acmpInputAPORT1YCH9  = _ACMP_INPUTSEL_POSSEL_APORT1YCH9,  /**< Select Dedicated APORT1Y CHannel 9. */
  acmpInputAPORT1XCH10 = _ACMP_INPUTSEL_POSSEL_APORT1XCH10, /**< Select Dedicated APORT1X CHannel 10. */
  acmpInputAPORT1YCH11 = _ACMP_INPUTSEL_POSSEL_APORT1YCH11, /**< Select Dedicated APORT1Y CHannel 11. */
  acmpInputAPORT1XCH12 = _ACMP_INPUTSEL_POSSEL_APORT1XCH12, /**< Select Dedicated APORT1X CHannel 12. */
  acmpInputAPORT1YCH13 = _ACMP_INPUTSEL_POSSEL_APORT1YCH13, /**< Select Dedicated APORT1Y CHannel 13. */
  acmpInputAPORT1XCH14 = _ACMP_INPUTSEL_POSSEL_APORT1XCH14, /**< Select Dedicated APORT1X CHannel 14. */
  acmpInputAPORT1YCH15 = _ACMP_INPUTSEL_POSSEL_APORT1YCH15, /**< Select Dedicated APORT1Y CHannel 15. */
  acmpInputAPORT1XCH16 = _ACMP_INPUTSEL_POSSEL_APORT1XCH16, /**< Select Dedicated APORT1X CHannel 16. */
  acmpInputAPORT1YCH17 = _ACMP_INPUTSEL_POSSEL_APORT1YCH17, /**< Select Dedicated APORT1Y CHannel 17. */
  acmpInputAPORT1XCH18 = _ACMP_INPUTSEL_POSSEL_APORT1XCH18, /**< Select Dedicated APORT1X CHannel 18. */
  acmpInputAPORT1YCH19 = _ACMP_INPUTSEL_POSSEL_APORT1YCH19, /**< Select Dedicated APORT1Y CHannel 19. */
  acmpInputAPORT1XCH20 = _ACMP_INPUTSEL_POSSEL_APORT1XCH20, /**< Select Dedicated APORT1X CHannel 20. */
  acmpInputAPORT1YCH21 = _ACMP_INPUTSEL_POSSEL_APORT1YCH21, /**< Select Dedicated APORT1Y CHannel 21. */
  acmpInputAPORT1XCH22 = _ACMP_INPUTSEL_POSSEL_APORT1XCH22, /**< Select Dedicated APORT1X CHannel 22. */
  acmpInputAPORT1YCH23 = _ACMP_INPUTSEL_POSSEL_APORT1YCH23, /**< Select Dedicated APORT1Y CHannel 23. */
  acmpInputAPORT1XCH24 = _ACMP_INPUTSEL_POSSEL_APORT1XCH24, /**< Select Dedicated APORT1X CHannel 24. */
  acmpInputAPORT1YCH25 = _ACMP_INPUTSEL_POSSEL_APORT1YCH25, /**< Select Dedicated APORT1Y CHannel 25. */
  acmpInputAPORT1XCH26 = _ACMP_INPUTSEL_POSSEL_APORT1XCH26, /**< Select Dedicated APORT1X CHannel 26. */
  acmpInputAPORT1YCH27 = _ACMP_INPUTSEL_POSSEL_APORT1YCH27, /**< Select Dedicated APORT1Y CHannel 27. */
  acmpInputAPORT1XCH28 = _ACMP_INPUTSEL_POSSEL_APORT1XCH28, /**< Select Dedicated APORT1X CHannel 28. */
  acmpInputAPORT1YCH29 = _ACMP_INPUTSEL_POSSEL_APORT1YCH29, /**< Select Dedicated APORT1Y CHannel 29. */
  acmpInputAPORT1XCH30 = _ACMP_INPUTSEL_POSSEL_APORT1XCH30, /**< Select Dedicated APORT1X CHannel 30. */
  acmpInputAPORT1YCH31 = _ACMP_INPUTSEL_POSSEL_APORT1YCH31, /**< Select Dedicated APORT1Y CHannel 31. */
  acmpInputAPORT2YCH0  = _ACMP_INPUTSEL_POSSEL_APORT2YCH0,  /**< Select Dedicated APORT2Y CHannel 0. */
  acmpInputAPORT2XCH1  = _ACMP_INPUTSEL_POSSEL_APORT2XCH1,  /**< Select Dedicated APORT2X CHannel 1. */
  acmpInputAPORT2YCH2  = _ACMP_INPUTSEL_POSSEL_APORT2YCH2,  /**< Select Dedicated APORT2Y CHannel 2. */
  acmpInputAPORT2XCH3  = _ACMP_INPUTSEL_POSSEL_APORT2XCH3,  /**< Select Dedicated APORT2X CHannel 3. */
  acmpInputAPORT2YCH4  = _ACMP_INPUTSEL_POSSEL_APORT2YCH4,  /**< Select Dedicated APORT2Y CHannel 4. */
  acmpInputAPORT2XCH5  = _ACMP_INPUTSEL_POSSEL_APORT2XCH5,  /**< Select Dedicated APORT2X CHannel 5. */
  acmpInputAPORT2YCH6  = _ACMP_INPUTSEL_POSSEL_APORT2YCH6,  /**< Select Dedicated APORT2Y CHannel 6. */
  acmpInputAPORT2XCH7  = _ACMP_INPUTSEL_POSSEL_APORT2XCH7,  /**< Select Dedicated APORT2X CHannel 7. */
  acmpInputAPORT2YCH8  = _ACMP_INPUTSEL_POSSEL_APORT2YCH8,  /**< Select Dedicated APORT2Y CHannel 8. */
  acmpInputAPORT2XCH9  = _ACMP_INPUTSEL_POSSEL_APORT2XCH9,  /**< Select Dedicated APORT2X CHannel 9. */
  acmpInputAPORT2YCH10 = _ACMP_INPUTSEL_POSSEL_APORT2YCH10, /**< Select Dedicated APORT2Y CHannel 10. */
  acmpInputAPORT2XCH11 = _ACMP_INPUTSEL_POSSEL_APORT2XCH11, /**< Select Dedicated APORT2X CHannel 11. */
  acmpInputAPORT2YCH12 = _ACMP_INPUTSEL_POSSEL_APORT2YCH12, /**< Select Dedicated APORT2Y CHannel 12. */
  acmpInputAPORT2XCH13 = _ACMP_INPUTSEL_POSSEL_APORT2XCH13, /**< Select Dedicated APORT2X CHannel 13. */
  acmpInputAPORT2YCH14 = _ACMP_INPUTSEL_POSSEL_APORT2YCH14, /**< Select Dedicated APORT2Y CHannel 14. */
  acmpInputAPORT2XCH15 = _ACMP_INPUTSEL_POSSEL_APORT2XCH15, /**< Select Dedicated APORT2X CHannel 15. */
  acmpInputAPORT2YCH16 = _ACMP_INPUTSEL_POSSEL_APORT2YCH16, /**< Select Dedicated APORT2Y CHannel 16. */
  acmpInputAPORT2XCH17 = _ACMP_INPUTSEL_POSSEL_APORT2XCH17, /**< Select Dedicated APORT2X CHannel 17. */
  acmpInputAPORT2YCH18 = _ACMP_INPUTSEL_POSSEL_APORT2YCH18, /**< Select Dedicated APORT2Y CHannel 18. */
  acmpInputAPORT2XCH19 = _ACMP_INPUTSEL_POSSEL_APORT2XCH19, /**< Select Dedicated APORT2X CHannel 19. */
  acmpInputAPORT2YCH20 = _ACMP_INPUTSEL_POSSEL_APORT2YCH20, /**< Select Dedicated APORT2Y CHannel 20. */
  acmpInputAPORT2XCH21 = _ACMP_INPUTSEL_POSSEL_APORT2XCH21, /**< Select Dedicated APORT2X CHannel 21. */
  acmpInputAPORT2YCH22 = _ACMP_INPUTSEL_POSSEL_APORT2YCH22, /**< Select Dedicated APORT2Y CHannel 22. */
  acmpInputAPORT2XCH23 = _ACMP_INPUTSEL_POSSEL_APORT2XCH23, /**< Select Dedicated APORT2X CHannel 23. */
  acmpInputAPORT2YCH24 = _ACMP_INPUTSEL_POSSEL_APORT2YCH24, /**< Select Dedicated APORT2Y CHannel 24. */
  acmpInputAPORT2XCH25 = _ACMP_INPUTSEL_POSSEL_APORT2XCH25, /**< Select Dedicated APORT2X CHannel 25. */
  acmpInputAPORT2YCH26 = _ACMP_INPUTSEL_POSSEL_APORT2YCH26, /**< Select Dedicated APORT2Y CHannel 26. */
  acmpInputAPORT2XCH27 = _ACMP_INPUTSEL_POSSEL_APORT2XCH27, /**< Select Dedicated APORT2X CHannel 27. */
  acmpInputAPORT2YCH28 = _ACMP_INPUTSEL_POSSEL_APORT2YCH28, /**< Select Dedicated APORT2Y CHannel 28. */
  acmpInputAPORT2XCH29 = _ACMP_INPUTSEL_POSSEL_APORT2XCH29, /**< Select Dedicated APORT2X CHannel 29. */
  acmpInputAPORT2YCH30 = _ACMP_INPUTSEL_POSSEL_APORT2YCH30, /**< Select Dedicated APORT2Y CHannel 30. */
  acmpInputAPORT2XCH31 = _ACMP_INPUTSEL_POSSEL_APORT2XCH31, /**< Select Dedicated APORT2X CHannel 31. */
  acmpInputAPORT3XCH0  = _ACMP_INPUTSEL_POSSEL_APORT3XCH0,  /**< Select Dedicated APORT3X CHannel 0. */
  acmpInputAPORT3YCH1  = _ACMP_INPUTSEL_POSSEL_APORT3YCH1,  /**< Select Dedicated APORT3Y CHannel 1. */
  acmpInputAPORT3XCH2  = _ACMP_INPUTSEL_POSSEL_APORT3XCH2,  /**< Select Dedicated APORT3X CHannel 2. */
  acmpInputAPORT3YCH3  = _ACMP_INPUTSEL_POSSEL_APORT3YCH3,  /**< Select Dedicated APORT3Y CHannel 3. */
  acmpInputAPORT3XCH4  = _ACMP_INPUTSEL_POSSEL_APORT3XCH4,  /**< Select Dedicated APORT3X CHannel 4. */
  acmpInputAPORT3YCH5  = _ACMP_INPUTSEL_POSSEL_APORT3YCH5,  /**< Select Dedicated APORT3Y CHannel 5. */
  acmpInputAPORT3XCH6  = _ACMP_INPUTSEL_POSSEL_APORT3XCH6,  /**< Select Dedicated APORT3X CHannel 6. */
  acmpInputAPORT3YCH7  = _ACMP_INPUTSEL_POSSEL_APORT3YCH7,  /**< Select Dedicated APORT3Y CHannel 7. */
  acmpInputAPORT3XCH8  = _ACMP_INPUTSEL_POSSEL_APORT3XCH8,  /**< Select Dedicated APORT3X CHannel 8. */
  acmpInputAPORT3YCH9  = _ACMP_INPUTSEL_POSSEL_APORT3YCH9,  /**< Select Dedicated APORT3Y CHannel 9. */
  acmpInputAPORT3XCH10 = _ACMP_INPUTSEL_POSSEL_APORT3XCH10, /**< Select Dedicated APORT3X CHannel 10. */
  acmpInputAPORT3YCH11 = _ACMP_INPUTSEL_POSSEL_APORT3YCH11, /**< Select Dedicated APORT3Y CHannel 11. */
  acmpInputAPORT3XCH12 = _ACMP_INPUTSEL_POSSEL_APORT3XCH12, /**< Select Dedicated APORT3X CHannel 12. */
  acmpInputAPORT3YCH13 = _ACMP_INPUTSEL_POSSEL_APORT3YCH13, /**< Select Dedicated APORT3Y CHannel 13. */
  acmpInputAPORT3XCH14 = _ACMP_INPUTSEL_POSSEL_APORT3XCH14, /**< Select Dedicated APORT3X CHannel 14. */
  acmpInputAPORT3YCH15 = _ACMP_INPUTSEL_POSSEL_APORT3YCH15, /**< Select Dedicated APORT3Y CHannel 15. */
  acmpInputAPORT3XCH16 = _ACMP_INPUTSEL_POSSEL_APORT3XCH16, /**< Select Dedicated APORT3X CHannel 16. */
  acmpInputAPORT3YCH17 = _ACMP_INPUTSEL_POSSEL_APORT3YCH17, /**< Select Dedicated APORT3Y CHannel 17. */
  acmpInputAPORT3XCH18 = _ACMP_INPUTSEL_POSSEL_APORT3XCH18, /**< Select Dedicated APORT3X CHannel 18. */
  acmpInputAPORT3YCH19 = _ACMP_INPUTSEL_POSSEL_APORT3YCH19, /**< Select Dedicated APORT3Y CHannel 19. */
  acmpInputAPORT3XCH20 = _ACMP_INPUTSEL_POSSEL_APORT3XCH20, /**< Select Dedicated APORT3X CHannel 20. */
  acmpInputAPORT3YCH21 = _ACMP_INPUTSEL_POSSEL_APORT3YCH21, /**< Select Dedicated APORT3Y CHannel 21. */
  acmpInputAPORT3XCH22 = _ACMP_INPUTSEL_POSSEL_APORT3XCH22, /**< Select Dedicated APORT3X CHannel 22. */
  acmpInputAPORT3YCH23 = _ACMP_INPUTSEL_POSSEL_APORT3YCH23, /**< Select Dedicated APORT3Y CHannel 23. */
  acmpInputAPORT3XCH24 = _ACMP_INPUTSEL_POSSEL_APORT3XCH24, /**< Select Dedicated APORT3X CHannel 24. */
  acmpInputAPORT3YCH25 = _ACMP_INPUTSEL_POSSEL_APORT3YCH25, /**< Select Dedicated APORT3Y CHannel 25. */
  acmpInputAPORT3XCH26 = _ACMP_INPUTSEL_POSSEL_APORT3XCH26, /**< Select Dedicated APORT3X CHannel 26. */
  acmpInputAPORT3YCH27 = _ACMP_INPUTSEL_POSSEL_APORT3YCH27, /**< Select Dedicated APORT3Y CHannel 27. */
  acmpInputAPORT3XCH28 = _ACMP_INPUTSEL_POSSEL_APORT3XCH28, /**< Select Dedicated APORT3X CHannel 28. */
  acmpInputAPORT3YCH29 = _ACMP_INPUTSEL_POSSEL_APORT3YCH29, /**< Select Dedicated APORT3Y CHannel 29. */
  acmpInputAPORT3XCH30 = _ACMP_INPUTSEL_POSSEL_APORT3XCH30, /**< Select Dedicated APORT3X CHannel 30. */
  acmpInputAPORT3YCH31 = _ACMP_INPUTSEL_POSSEL_APORT3YCH31, /**< Select Dedicated APORT3Y CHannel 31. */
  acmpInputAPORT4YCH0  = _ACMP_INPUTSEL_POSSEL_APORT4YCH0,  /**< Select Dedicated APORT4Y CHannel 0. */
  acmpInputAPORT4XCH1  = _ACMP_INPUTSEL_POSSEL_APORT4XCH1,  /**< Select Dedicated APORT4X CHannel 1. */
  acmpInputAPORT4YCH2  = _ACMP_INPUTSEL_POSSEL_APORT4YCH2,  /**< Select Dedicated APORT4Y CHannel 2. */
  acmpInputAPORT4XCH3  = _ACMP_INPUTSEL_POSSEL_APORT4XCH3,  /**< Select Dedicated APORT4X CHannel 3. */
  acmpInputAPORT4YCH4  = _ACMP_INPUTSEL_POSSEL_APORT4YCH4,  /**< Select Dedicated APORT4Y CHannel 4. */
  acmpInputAPORT4XCH5  = _ACMP_INPUTSEL_POSSEL_APORT4XCH5,  /**< Select Dedicated APORT4X CHannel 5. */
  acmpInputAPORT4YCH6  = _ACMP_INPUTSEL_POSSEL_APORT4YCH6,  /**< Select Dedicated APORT4Y CHannel 6. */
  acmpInputAPORT4XCH7  = _ACMP_INPUTSEL_POSSEL_APORT4XCH7,  /**< Select Dedicated APORT4X CHannel 7. */
  acmpInputAPORT4YCH8  = _ACMP_INPUTSEL_POSSEL_APORT4YCH8,  /**< Select Dedicated APORT4Y CHannel 8. */
  acmpInputAPORT4XCH9  = _ACMP_INPUTSEL_POSSEL_APORT4XCH9,  /**< Select Dedicated APORT4X CHannel 9. */
  acmpInputAPORT4YCH10 = _ACMP_INPUTSEL_POSSEL_APORT4YCH10, /**< Select Dedicated APORT4Y CHannel 10. */
  acmpInputAPORT4XCH11 = _ACMP_INPUTSEL_POSSEL_APORT4XCH11, /**< Select Dedicated APORT4X CHannel 11. */
  acmpInputAPORT4YCH12 = _ACMP_INPUTSEL_POSSEL_APORT4YCH12, /**< Select Dedicated APORT4Y CHannel 12. */
  acmpInputAPORT4XCH13 = _ACMP_INPUTSEL_POSSEL_APORT4XCH13, /**< Select Dedicated APORT4X CHannel 13. */
  acmpInputAPORT4YCH16 = _ACMP_INPUTSEL_POSSEL_APORT4YCH16, /**< Select Dedicated APORT4Y CHannel 14. */
  acmpInputAPORT4XCH17 = _ACMP_INPUTSEL_POSSEL_APORT4XCH17, /**< Select Dedicated APORT4X CHannel 15. */
  acmpInputAPORT4YCH18 = _ACMP_INPUTSEL_POSSEL_APORT4YCH18, /**< Select Dedicated APORT4Y CHannel 16. */
  acmpInputAPORT4XCH19 = _ACMP_INPUTSEL_POSSEL_APORT4XCH19, /**< Select Dedicated APORT4X CHannel 17. */
  acmpInputAPORT4YCH20 = _ACMP_INPUTSEL_POSSEL_APORT4YCH20, /**< Select Dedicated APORT4Y CHannel 18. */
  acmpInputAPORT4XCH21 = _ACMP_INPUTSEL_POSSEL_APORT4XCH21, /**< Select Dedicated APORT4X CHannel 19. */
  acmpInputAPORT4YCH22 = _ACMP_INPUTSEL_POSSEL_APORT4YCH22, /**< Select Dedicated APORT4Y CHannel 20. */
  acmpInputAPORT4XCH23 = _ACMP_INPUTSEL_POSSEL_APORT4XCH23, /**< Select Dedicated APORT4X CHannel 21. */
  acmpInputAPORT4YCH24 = _ACMP_INPUTSEL_POSSEL_APORT4YCH24, /**< Select Dedicated APORT4Y CHannel 22. */
  acmpInputAPORT4XCH25 = _ACMP_INPUTSEL_POSSEL_APORT4XCH25, /**< Select Dedicated APORT4X CHannel 23. */
  acmpInputAPORT4YCH26 = _ACMP_INPUTSEL_POSSEL_APORT4YCH26, /**< Select Dedicated APORT4Y CHannel 24. */
  acmpInputAPORT4XCH27 = _ACMP_INPUTSEL_POSSEL_APORT4XCH27, /**< Select Dedicated APORT4X CHannel 25. */
  acmpInputAPORT4YCH28 = _ACMP_INPUTSEL_POSSEL_APORT4YCH28, /**< Select Dedicated APORT4Y CHannel 26. */
  acmpInputAPORT4XCH29 = _ACMP_INPUTSEL_POSSEL_APORT4XCH29, /**< Select Dedicated APORT4X CHannel 27. */
  acmpInputAPORT4YCH30 = _ACMP_INPUTSEL_POSSEL_APORT4YCH30, /**< Select Dedicated APORT4Y CHannel 28. */
  acmpInputAPORT4YCH14 = _ACMP_INPUTSEL_POSSEL_APORT4YCH14, /**< Select Dedicated APORT4X CHannel 29. */
  acmpInputAPORT4XCH15 = _ACMP_INPUTSEL_POSSEL_APORT4XCH15, /**< Select Dedicated APORT4Y CHannel 30. */
  acmpInputAPORT4XCH31 = _ACMP_INPUTSEL_POSSEL_APORT4XCH31, /**< Select Dedicated APORT4X CHannel 31. */
#if defined(_ACMP_INPUTSEL_POSSEL_DACOUT0)
  acmpInputDACOUT0   = _ACMP_INPUTSEL_POSSEL_DACOUT0, /**< Select DAC Channel 0 Output. */
#endif
#if defined(_ACMP_INPUTSEL_POSSEL_DACOUT1)
  acmpInputDACOUT1   = _ACMP_INPUTSEL_POSSEL_DACOUT1, /**< Select DAC Channel 1 Output. */
#endif
  acmpInputVLP       = _ACMP_INPUTSEL_POSSEL_VLP,   /**< Select Low-Power Sampled Voltage. */
  acmpInputVBDIV     = _ACMP_INPUTSEL_POSSEL_VBDIV, /**< Select Divided VB Voltage. */
  acmpInputVADIV     = _ACMP_INPUTSEL_POSSEL_VADIV, /**< Select Divided VA Voltage. */
  acmpInputVDD       = _ACMP_INPUTSEL_POSSEL_VDD,   /**< ACMPVDD as selected via PWRSEL. */
  acmpInputVSS       = _ACMP_INPUTSEL_POSSEL_VSS,   /**< Select VSS. */
} ACMP_Channel_TypeDef;
#else
/** ACMP inputs. Note that scaled VDD and bandgap references can only be used
 *  as negative inputs. */
typedef enum {
  /** Channel 0 */
  acmpChannel0    = _ACMP_INPUTSEL_NEGSEL_CH0,
  /** Channel 1 */
  acmpChannel1    = _ACMP_INPUTSEL_NEGSEL_CH1,
  /** Channel 2 */
  acmpChannel2    = _ACMP_INPUTSEL_NEGSEL_CH2,
  /** Channel 3 */
  acmpChannel3    = _ACMP_INPUTSEL_NEGSEL_CH3,
  /** Channel 4 */
  acmpChannel4    = _ACMP_INPUTSEL_NEGSEL_CH4,
  /** Channel 5 */
  acmpChannel5    = _ACMP_INPUTSEL_NEGSEL_CH5,
  /** Channel 6 */
  acmpChannel6    = _ACMP_INPUTSEL_NEGSEL_CH6,
  /** Channel 7 */
  acmpChannel7    = _ACMP_INPUTSEL_NEGSEL_CH7,
  /** 1.25 V internal reference */
  acmpChannel1V25 = _ACMP_INPUTSEL_NEGSEL_1V25,
  /** 2.5 V internal reference */
  acmpChannel2V5  = _ACMP_INPUTSEL_NEGSEL_2V5,
  /** Scaled VDD reference */
  acmpChannelVDD  = _ACMP_INPUTSEL_NEGSEL_VDD,

#if defined(_ACMP_INPUTSEL_NEGSEL_DAC0CH0)
  /** DAC0 channel 0 */
  acmpChannelDAC0Ch0 = _ACMP_INPUTSEL_NEGSEL_DAC0CH0,
#endif

#if defined(_ACMP_INPUTSEL_NEGSEL_DAC0CH1)
  /** DAC0 channel 1 */
  acmpChannelDAC0Ch1 = _ACMP_INPUTSEL_NEGSEL_DAC0CH1,
#endif

#if defined(_ACMP_INPUTSEL_NEGSEL_CAPSENSE)
  /** Capacitive sense mode */
  acmpChannelCapSense = _ACMP_INPUTSEL_NEGSEL_CAPSENSE,
#endif
} ACMP_Channel_TypeDef;
#endif

#if defined(_ACMP_EXTIFCTRL_MASK)
/**
 * ACMP external input select. This type is used to select which APORT is
 * used by an external module, such as LESENSE, when it's taking control over
 * the ACMP input.
 */
typedef enum {
  acmpExternalInputAPORT0X  = _ACMP_EXTIFCTRL_APORTSEL_APORT0X,  /**< Select APORT0X as an external input. */
  acmpExternalInputAPORT0Y  = _ACMP_EXTIFCTRL_APORTSEL_APORT0Y,  /**< Select APORT0Y as an external input. */
  acmpExternalInputAPORT1X  = _ACMP_EXTIFCTRL_APORTSEL_APORT1X,  /**< Select APORT1X as an external input. */
  acmpExternalInputAPORT1Y  = _ACMP_EXTIFCTRL_APORTSEL_APORT1Y,  /**< Select APORT1Y as an external input. */
  acmpExternalInputAPORT1XY = _ACMP_EXTIFCTRL_APORTSEL_APORT1XY, /**< Select APORT1XY as an external input. */
  acmpExternalInputAPORT2X  = _ACMP_EXTIFCTRL_APORTSEL_APORT2X,  /**< Select APORT2X as an external input. */
  acmpExternalInputAPORT2Y  = _ACMP_EXTIFCTRL_APORTSEL_APORT2Y,  /**< Select APORT2Y as an external input. */
  acmpExternalInputAPORT2YX = _ACMP_EXTIFCTRL_APORTSEL_APORT2YX, /**< Select APORT2YX as an external input. */
  acmpExternalInputAPORT3X  = _ACMP_EXTIFCTRL_APORTSEL_APORT3X,  /**< Select APORT3X as an external input. */
  acmpExternalInputAPORT3Y  = _ACMP_EXTIFCTRL_APORTSEL_APORT3Y,  /**< Select APORT3Y as an external input. */
  acmpExternalInputAPORT3XY = _ACMP_EXTIFCTRL_APORTSEL_APORT3XY, /**< Select APORT3XY as an external input. */
  acmpExternalInputAPORT4X  = _ACMP_EXTIFCTRL_APORTSEL_APORT4X,  /**< Select APORT4X as an external input. */
  acmpExternalInputAPORT4Y  = _ACMP_EXTIFCTRL_APORTSEL_APORT4Y,  /**< Select APORT4Y as an external input. */
  acmpExternalInputAPORT4YX = _ACMP_EXTIFCTRL_APORTSEL_APORT4YX, /**< Select APORT4YX as an external input. */
} ACMP_ExternalInput_Typedef;
#endif

/*******************************************************************************
 ******************************   STRUCTS   ************************************
 ******************************************************************************/

/** Capsense initialization structure. */
typedef struct {
#if defined(_ACMP_CTRL_FULLBIAS_MASK)
  /** Full-bias current. See the ACMP chapter about bias and response time in
   *  the reference manual for details. */
  bool                          fullBias;
#endif

#if defined(_ACMP_CTRL_HALFBIAS_MASK)
  /** Half-bias current. See the ACMP chapter about bias and response time in
   *  the reference manual for details. */
  bool                          halfBias;
#endif

  /** Bias current. See the ACMP chapter about bias and response time in the
   *  reference manual for details. */
  uint32_t                      biasProg;

#if defined(_ACMP_CTRL_WARMTIME_MASK)
  /** Warmup time, which is measured in HFPERCLK / HFPERCCLK cycles and should be
   *  about 10 us in wall clock time. */
  ACMP_WarmTime_TypeDef         warmTime;
#endif

#if defined(_ACMP_CTRL_HYSTSEL_MASK) \
  ||  defined(_ACMP_CFG_HYST_MASK)
  /** Hysteresis level. */
  ACMP_HysteresisLevel_TypeDef  hysteresisLevel;
#else
  /** Hysteresis level when ACMP output is 0. */
  ACMP_HysteresisLevel_TypeDef  hysteresisLevel_0;

  /** Hysteresis level when ACMP output is 1. */
  ACMP_HysteresisLevel_TypeDef  hysteresisLevel_1;
#endif

  /** A resistor used in the capacitive sensing circuit. For values see
   *  the device data sheet. */
  ACMP_CapsenseResistor_TypeDef resistor;

#if defined(_ACMP_INPUTSEL_LPREF_MASK)
  /** Low-power reference enabled. This setting, if enabled, reduces the
   *  power used by VDD and bandgap references. */
  bool                          lowPowerReferenceEnabled;
#endif

#if defined(_ACMP_INPUTCTRL_VREFDIV_MASK)
  /** VDD division factor. VREFOUT = VREFIN * (VREFDIV / 63).
   *  Valid values are in the 0-63 range. */
  uint32_t                      vrefDiv;
#elif defined(_ACMP_INPUTSEL_VDDLEVEL_MASK)
  /** VDD reference value. VDD_SCALED = (VDD * VDDLEVEL) / 63.
   *  Valid values are in the 0-63 range. */
  uint32_t                      vddLevel;
#else
  /**
   * This value configures the upper voltage threshold of the capsense
   * oscillation rail.
   *
   * The voltage threshold is calculated as follows:
   *   VDD * (vddLevelHigh + 1) / 64
   */
  uint32_t                      vddLevelHigh;

  /**
   * This value configures the lower voltage threshold of the capsense
   * oscillation rail.
   *
   * The voltage threshold is calculated as follows:
   *   VDD * (vddLevelLow + 1) / 64
   */
  uint32_t                      vddLevelLow;
#endif

  /** If true, ACMP is enabled after configuration. */
  bool                          enable;
} ACMP_CapsenseInit_TypeDef;

/** A default configuration for capacitive sense mode initialization. */
#if defined(_ACMP_CFG_MASK)

// PM5507: provide default configuration that is functional
/** Analog comparator CFG with initial bias value */
#define PM5507_ACMP_CFG_BIAS_DEFAULT  0x00000004UL
/** Analog comparator reset value */
#define PM5507_ACMP_CFG_RESETVALUE    0x00000004UL

/** Capacitive sense mode configuration default values. */
#define ACMP_CAPSENSE_INIT_DEFAULT                                                \
  {                                                                               \
    PM5507_ACMP_CFG_BIAS_DEFAULT, /* Using biasProg default value. */             \
    acmpHysteresisDisabled,       /* Disable hysteresis. */                       \
    acmpResistor5,                /* Use internal resistor value 5. */            \
    0x3F,                         /* Set VREFDIV to maximum to disable divide. */ \
    true                          /* Enable after init. */                        \
  }
#elif defined(_ACMP_HYSTERESIS0_HYST_MASK)
#define ACMP_CAPSENSE_INIT_DEFAULT                                            \
  {                                                                           \
    false,              /* Don't use fullBias to lower power consumption. */  \
    0x20,               /* Using biasProg value of 0x20 (32). */              \
    acmpHysteresisLevel8, /* Use hysteresis level 8 when ACMP output is 0. */ \
    acmpHysteresisLevel8, /* Use hysteresis level 8 when ACMP output is 1. */ \
    acmpResistor5,      /* Use internal resistor value 5. */                  \
    0x30,               /* VDD level high. */                                 \
    0x10,               /* VDD level low. */                                  \
    true                /* Enable after initialization. */                    \
  }
#elif defined(_ACMP_CTRL_WARMTIME_MASK)
#define ACMP_CAPSENSE_INIT_DEFAULT                      \
  {                                                     \
    false,            /* fullBias */                    \
    false,            /* halfBias */                    \
    0x7,              /* biasProg */                    \
    acmpWarmTime512,  /* 512 cycle warmup to be safe */ \
    acmpHysteresisLevel5,                               \
    acmpResistor3,                                      \
    false,            /* low power reference */         \
    0x3D,             /* VDD level */                   \
    true              /* Enable after init. */          \
  }
#else
#define ACMP_CAPSENSE_INIT_DEFAULT              \
  {                                             \
    false,            /* fullBias */            \
    false,            /* halfBias */            \
    0x7,              /* biasProg */            \
    acmpHysteresisLevel5,                       \
    acmpResistor3,                              \
    false,            /* low power reference */ \
    0x3D,             /* VDD level */           \
    true              /* Enable after init. */  \
  }
#endif

/** ACMP initialization structure. */
typedef struct {
#if defined(_ACMP_CTRL_FULLBIAS_MASK)
  /** Full-bias current. See the ACMP chapter about bias and response time in
   *  the reference manual for details. */
  bool                         fullBias;
#endif

#if defined(_ACMP_CTRL_HALFBIAS_MASK)
  /** Half-bias current. See the ACMP chapter about bias and response time in
   *  the reference manual for details. */
  bool                         halfBias;
#endif

  /** Bias current. See the ACMP chapter about bias and response time in the
   *  reference manual for details. Valid values are in the range 0-7. */
  uint32_t                     biasProg;

#if defined(_ACMP_CTRL_IFALL_SHIFT)
  /** Enable setting the interrupt flag on the falling edge. */
  bool                         interruptOnFallingEdge;
#endif
#if defined(_ACMP_CTRL_IRISE_SHIFT)
  /** Enable setting the interrupt flag on the rising edge. */
  bool                         interruptOnRisingEdge;
#endif

#if defined(_ACMP_CTRL_INPUTRANGE_MASK) \
  || defined(_ACMP_CFG_INPUTRANGE_MASK)
  /** Input range. Adjust this setting to optimize the performance for a
   *  given input voltage range.  */
  ACMP_InputRange_TypeDef      inputRange;
#endif

#if defined(_ACMP_CTRL_ACCURACY_MASK) \
  || defined(_ACMP_CFG_ACCURACY_MASK)
  /** ACMP accuracy mode. Select the accuracy mode that matches the
   *  required current usage and accuracy requirement. Low accuracy
   *  consumes less current while high accuracy consumes more current. */
  ACMP_Accuracy_TypeDef        accuracy;
#endif

#if defined(_ACMP_CTRL_PWRSEL_MASK)
  /** Select the power source for the ACMP. */
  ACMP_PowerSource_TypeDef     powerSource;
#endif

#if defined(_ACMP_CTRL_WARMTIME_MASK)
  /** Warmup time, which is measured in HFPERCLK / HFPERCCLK cycles and should be
   *  about 10 us in wall clock time. */
  ACMP_WarmTime_TypeDef        warmTime;
#endif

#if defined(_ACMP_CTRL_HYSTSEL_MASK) \
  ||  defined(_ACMP_CFG_HYST_MASK)
  /** Hysteresis level. */
  ACMP_HysteresisLevel_TypeDef hysteresisLevel;
#else
  /** Hysteresis when ACMP output is 0. */
  ACMP_HysteresisLevel_TypeDef hysteresisLevel_0;

  /** Hysteresis when ACMP output is 1. */
  ACMP_HysteresisLevel_TypeDef hysteresisLevel_1;
#endif

#if defined(_ACMP_INPUTSEL_VLPSEL_MASK)
  /** VLP Input source. Select between using VADIV or VBDIV as the VLP
   *  source. */
  ACMP_VLPInput_Typedef        vlpInput;
#endif

  /** Inactive value emitted by ACMP during warmup. */
  bool                         inactiveValue;

#if defined(_ACMP_INPUTSEL_LPREF_MASK)
  /** Low power reference enabled. This setting, if enabled, reduces the
   *  power used by the VDD and bandgap references. */
  bool                         lowPowerReferenceEnabled;
#endif

#if defined(_ACMP_INPUTCTRL_VREFDIV_MASK)
  /** VDD division factor. VREFOUT = VREFIN * (VREFDIV / 63).
   *  Valid values are in the 0-63 range. */
  uint32_t                     vrefDiv;
#elif defined(_ACMP_INPUTSEL_VDDLEVEL_MASK)
  /** VDD reference value. VDD_SCALED = VDD * VDDLEVEL * 50 mV/3.8 V.
   *  Valid values are in the 0-63 range. */
  uint32_t                     vddLevel;
#endif

  /** If true, ACMP is enabled after configuration. */
  bool                         enable;
} ACMP_Init_TypeDef;

/** Default configuration for ACMP regular initialization. */
#if defined(_ACMP_CFG_MASK)
#define ACMP_INIT_DEFAULT                                                         \
  {                                                                               \
    PM5507_ACMP_CFG_BIAS_DEFAULT, /* Using biasProg default value. */             \
    acmpInputRangeFull,           /* Input range from 0 to Vdd. */                \
    acmpAccuracyLow,              /* Low accuracy, less current usage. */         \
    acmpHysteresisDisabled,       /* Disable hysteresis. */                       \
    false,                        /* Output 0 when ACMP is inactive. */           \
    0x3F,                         /* Set VREFDIV to maximum to disable divide. */ \
    true                          /* Enable after init. */                        \
  }
#elif defined(_ACMP_HYSTERESIS0_HYST_MASK)
#define ACMP_INIT_DEFAULT                                                   \
  {                                                                         \
    false,                    /* fullBias */                                \
    0x7,                      /* biasProg */                                \
    false,                    /* No interrupt on falling edge. */           \
    false,                    /* No interrupt on rising edge. */            \
    acmpInputRangeFull,       /* Input range from 0 to VDD. */              \
    acmpAccuracyLow,          /* Low accuracy, less current usage. */       \
    acmpPowerSourceAvdd,      /* Use the AVDD supply. */                    \
    acmpHysteresisLevel5,     /* Use hysteresis level 5 when output is 0 */ \
    acmpHysteresisLevel5,     /* Use hysteresis level 5 when output is 1 */ \
    acmpVLPInputVADIV,        /* Use VADIV as the VLP input source. */      \
    false,                    /* Output 0 when ACMP is inactive. */         \
    true                      /* Enable after init. */                      \
  }
#else
#define ACMP_INIT_DEFAULT                                                   \
  {                                                                         \
    false,            /* fullBias */                                        \
    false,            /* halfBias */                                        \
    0x7,              /* biasProg */                                        \
    false,            /* No interrupt on falling edge. */                   \
    false,            /* No interrupt on rising edge. */                    \
    acmpWarmTime512,  /* 512 cycle warmup to be safe */                     \
    acmpHysteresisLevel5,                                                   \
    false,            /* Disabled emitting inactive value during warmup. */ \
    false,            /* low power reference */                             \
    0x3D,             /* VDD level */                                       \
    true              /* Enable after init. */                              \
  }
#endif

#if defined(_ACMP_INPUTSEL_VASEL_MASK)
/** VA Configuration structure. This structure is used to configure the
 *  VA voltage input source and its dividers. */
typedef struct {
  ACMP_VAInput_TypeDef input; /**< VA voltage input source */

  /**
   * A divider for VA voltage input source when ACMP output is 0. This value is
   * used to divide the VA voltage input source by a specific value. The valid
   * range is between 0 and 63.
   *
   *  VA divided = VA input * (div0 + 1) / 64
   */
  uint32_t             div0;

  /**
   * A divider for VA voltage input source when ACMP output is 1. This value is
   * used to divide the VA voltage input source by a specific value. The valid
   * range is between 0 and 63.
   *
   *  VA divided = VA input * (div1 + 1) / 64
   */
  uint32_t             div1;
} ACMP_VAConfig_TypeDef;

/** VA default configuration. */
#define ACMP_VACONFIG_DEFAULT                                               \
  {                                                                         \
    acmpVAInputVDD, /* Use VDD as VA voltage input source. */               \
    63,           /* No division of the VA source when ACMP output is 0. */ \
    63,           /* No division of the VA source when ACMP output is 1. */ \
  }
#endif

#if defined(_ACMP_INPUTSEL_VBSEL_MASK)
/** VB Configuration structure. This structure is used to configure the
 *  VB voltage input source and its dividers. */
typedef struct {
  ACMP_VBInput_TypeDef input; /**< VB Voltage input source */

  /**
   * A divider for VB voltage input source when ACMP output is 0. This value is
   * used to divide the VB voltage input source by a specific value. The valid
   * range is between 0 and 63.
   *
   *  VB divided = VB input * (div0 + 1) / 64
   */
  uint32_t             div0;

  /**
   * A divider for VB voltage input source when ACMP output is 1. This value is
   * used to divide the VB voltage input source by a specific value. The valid
   * range is between 0 and 63.
   *
   *  VB divided = VB input * (div1 + 1) / 64
   */
  uint32_t             div1;
} ACMP_VBConfig_TypeDef;

/** VB default configuration. */
#define ACMP_VBCONFIG_DEFAULT                                                \
  {                                                                          \
    acmpVBInput1V25, /* Use 1.25 V as VB voltage input source. */            \
    63,            /* No division of the VB source when ACMP output is 0. */ \
    63,            /* No division of the VB source when ACMP output is 1. */ \
  }
#endif

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

void ACMP_CapsenseInit(ACMP_TypeDef *acmp, const ACMP_CapsenseInit_TypeDef *init);
void ACMP_CapsenseChannelSet(ACMP_TypeDef *acmp, ACMP_Channel_TypeDef channel);
void ACMP_ChannelSet(ACMP_TypeDef *acmp, ACMP_Channel_TypeDef negSel, ACMP_Channel_TypeDef posSel);
void ACMP_Disable(ACMP_TypeDef *acmp);
void ACMP_Enable(ACMP_TypeDef *acmp);
#if defined(_ACMP_EXTIFCTRL_MASK)
void ACMP_ExternalInputSelect(ACMP_TypeDef *acmp, ACMP_ExternalInput_Typedef aport);
#endif
#if defined(_GPIO_ACMP_ROUTEEN_MASK)
void ACMP_GPIOSetup(ACMP_TypeDef *acmp, GPIO_Port_TypeDef port, unsigned int pin, bool enable, bool invert);
#else
void ACMP_GPIOSetup(ACMP_TypeDef *acmp, uint32_t location, bool enable, bool invert);
#endif
void ACMP_Init(ACMP_TypeDef *acmp, const ACMP_Init_TypeDef *init);
void ACMP_Reset(ACMP_TypeDef *acmp);
#if defined(_ACMP_INPUTSEL_VASEL_MASK)
void ACMP_VASetup(ACMP_TypeDef *acmp, const ACMP_VAConfig_TypeDef *vaconfig);
#endif
#if defined(_ACMP_INPUTSEL_VBSEL_MASK)
void ACMP_VBSetup(ACMP_TypeDef *acmp, const ACMP_VBConfig_TypeDef *vbconfig);
#endif

/***************************************************************************//**
 * @brief
 *   Clear one or more pending ACMP interrupts.
 *
 * @param[in] acmp
 *   A pointer to the ACMP peripheral register block.
 *
 * @param[in] flags
 *   Pending ACMP interrupt source to clear. Use a bitwise logic OR combination
 *   of valid interrupt flags for the ACMP module. The flags can be, for instance,
 *   ACMP_IFC_EDGE or ACMP_IFC_WARMUP.
 ******************************************************************************/
__STATIC_INLINE void ACMP_IntClear(ACMP_TypeDef *acmp, uint32_t flags)
{
#if defined(ACMP_HAS_SET_CLEAR)
  acmp->IF_CLR = flags;
#else
  acmp->IFC = flags;
#endif
}

/***************************************************************************//**
 * @brief
 *   Disable one or more ACMP interrupts.
 *
 * @param[in] acmp
 *   A pointer to the ACMP peripheral register block.
 *
 * @param[in] flags
 *   ACMP interrupt sources to disable. Use a bitwise logic OR combination of
 *   valid interrupt flags for the ACMP module. The flags can be, for instance,
 *   ACMP_IEN_EDGE or ACMP_IEN_WARMUP.
 ******************************************************************************/
__STATIC_INLINE void ACMP_IntDisable(ACMP_TypeDef *acmp, uint32_t flags)
{
  BUS_RegMaskedClear(&(acmp->IEN), flags);
}

/***************************************************************************//**
 * @brief
 *   Enable one or more ACMP interrupts.
 *
 * @note
 *   Depending on the use, a pending interrupt may already be set prior to
 *   enabling the interrupt. Consider using ACMP_IntClear() prior to enabling
 *   if a pending interrupt should be ignored.
 *
 * @param[in] acmp
 *   A pointer to the ACMP peripheral register block.
 *
 * @param[in] flags
 *   ACMP interrupt sources to enable. Use a bitwise logic OR combination of
 *   valid interrupt flags for the ACMP module. The flags can be, for instance,
 *   ACMP_IEN_EDGE or ACMP_IEN_WARMUP.
 ******************************************************************************/
__STATIC_INLINE void ACMP_IntEnable(ACMP_TypeDef *acmp, uint32_t flags)
{
#if defined(ACMP_HAS_SET_CLEAR)
  acmp->IEN_SET = flags;
#else
  acmp->IEN |= flags;
#endif
}

/***************************************************************************//**
 * @brief
 *   Get pending ACMP interrupt flags.
 *
 * @note
 *   This function does not clear event bits.
 *
 * @param[in] acmp
 *   A pointer to the ACMP peripheral register block.
 *
 * @return
 *   Pending ACMP interrupt sources. A bitwise logic OR combination of valid
 *   interrupt flags for the ACMP module. The pending interrupt sources can be,
 *   for instance, ACMP_IF_EDGE or ACMP_IF_WARMUP.
 ******************************************************************************/
__STATIC_INLINE uint32_t ACMP_IntGet(ACMP_TypeDef *acmp)
{
  return acmp->IF;
}

/***************************************************************************//**
 * @brief
 *   Get enabled and pending ACMP interrupt flags.
 *   Useful for handling more interrupt sources in the same interrupt handler.
 *
 * @param[in] acmp
 *   A pointer to the ACMP peripheral register block.
 *
 * @note
 *   This function does not clear interrupt flags.
 *
 * @return
 *   Pending and enabled ACMP interrupt sources.
 *   The return value is the bitwise AND combination of
 *   - the OR combination of enabled interrupt sources in ACMPx_IEN_nnn
 *     register (ACMPx_IEN_nnn) and
 *   - the OR combination of valid interrupt flags of the ACMP module
 *     (ACMPx_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE uint32_t ACMP_IntGetEnabled(ACMP_TypeDef *acmp)
{
  uint32_t tmp;

  /* Store ACMPx->IEN in a temporary variable to define the explicit order
   * of volatile accesses. */
  tmp = acmp->IEN;

  /* Bitwise AND of pending and enabled interrupts. */
  return acmp->IF & tmp;
}

/***************************************************************************//**
 * @brief
 *   Set one or more pending ACMP interrupts from software.
 *
 * @param[in] acmp
 *   A pointer to the ACMP peripheral register block.
 *
 * @param[in] flags
 *   ACMP interrupt sources to set as pending. Use a bitwise logic OR
 *   combination of valid interrupt flags for the ACMP module. The flags can be,
 *   for instance, ACMP_IFS_EDGE or ACMP_IFS_WARMUP.
 ******************************************************************************/
__STATIC_INLINE void ACMP_IntSet(ACMP_TypeDef *acmp, uint32_t flags)
{
#if defined(ACMP_HAS_SET_CLEAR)
  acmp->IF_SET = flags;
#else
  acmp->IFS = flags;
#endif
}

#if defined(_ACMP_INPUTCTRL_MASK)
/***************************************************************************//**
 * @brief
 *   Convert GPIO port/pin to ACMP input selection.
 *
 * @param[in] port
 *   GPIO port
 *
 * @param[in] pin
 *   GPIO pin
 *
 * @return
 *   ACMP input selection
 ******************************************************************************/
__STATIC_INLINE ACMP_Channel_TypeDef ACMP_PortPinToInput(GPIO_Port_TypeDef port, uint8_t pin)
{
  uint32_t input = (((uint32_t) port + (_ACMP_INPUTCTRL_POSSEL_PA0 >> 4)) << 4) | pin;

  return (ACMP_Channel_TypeDef) input;
}
#endif

/** @} (end addtogroup acmp) */

#ifdef __cplusplus
}
#endif

#endif /* defined(ACMP_COUNT) && (ACMP_COUNT > 0) */
#endif /* EM_ACMP_H */
