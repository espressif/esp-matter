/******************************************************************************
 @file:       urfc.h

 Group: WCS BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2016-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/**
 *  @addtogroup Micro_BLE_Stack
 *  @{
 *  @defgroup URF Micro RF Settings
 *  @{
 *  @file  urfc.h
 *  @brief This file contains configurable variables for the Micro BLE Stack
 *  radio.
 *
 *  In general, the configuration values vary by device type, device version
 *  and board characteristics. The definitions and values listed in
 *  urfcfg.h and urfcfg.c can be changed or more sets of definitions and
 *  values can be added, depending on which version/type of device and what
 *  characteristics the board has.
 *
 *  @note User configurable variables except the elements of
 *  the power table are only used during the initialization
 *  of the MAC. Changing the values of these variables
 *  except power table elements after this will have no
 *  effect.
 */

#ifndef URFC_H
#define URFC_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */
#include <hal_types.h>
#if !defined(DeviceFamily_CC26X1)
#include <rf_patches/rf_patch_cpe_multi_protocol_rtls.h>
#endif
#include <ti/drivers/rf/RF.h>
#include "rf_hal.h"

/*******************************************************************************
 * MACROS
 */

//
// Tx Power
//
// PG1:              PG2:
// 7..6 | 5..0         15..8   | 7..6 | 5..0
//  GC  |  IB        tempCoeff |  GC  |  IB
//
#if defined(CC26X2) || defined(CC13X2)

#define GEN_TX_POWER_VAL(ib, gc, tc)                                            \
  (uint16)(((ib) & 0x3F) | (((gc) & 0x03) << 6) | (((tc)&0x7F) << 9))

#elif defined(CC13X2P)

// taken from RF_TxPowerTable_DEFAULT_PA_ENTRY in RF.h
#define RF_TxPowerTable_DefaultPAEntry(bias, gain, boost, coefficient)          \
  (uint32)(((bias) << 0) | ((gain) << 6) | ((boost) << 8) | ((coefficient) << 9) | (RF_TxPowerTable_DefaultPA << 31))

// taken from RF_TxPowerTable_HIGH_PA_ENTRY in RF.h
#define RF_TxPowerTable_HighPAEntry(bias, ibboost, boost, coefficient, ldotrim) \
  (uint32)(((bias) << 0) | ((ibboost) << 6) | ((boost) << 8) | ((coefficient) << 9) | ((ldotrim) << 16) | (RF_TxPowerTable_HighPA << 31))

#endif // CC26X2 || CC13X2

/*******************************************************************************
 * CONSTANTS
 */

// Defines only required by Application.
// Note: ICALL_STACK0_ADDR is assigned the entry point of the stack image and
//       is only defined for the Application project
// RF Front End Settings
// Note: The use of these values completely depends on how the PCB is laid out.
//       Please see Device Package and Evaluation Module (EM) Board below.
/** @defgroup URF_Constants Micro RF Constants
 * @{
 */

/**
 * @defgroup URF_RF_Front_Settings Micro BLE Stack RF Front End Settings
 * @{
 * @note The use of these values completely depends on how the PCB is laid out.
 *
 * The device may come in more than one type of packages.
 * For each package, the user may change how the RF Front End (FE) is
 * configured. The possible FE settings are provided as a set of defines.
 * (The user can also set the FE bias, the settings of which are also provided
 * as defines.) The user can change the value of RF_FE_MODE_AND_BIAS to
 * configure the RF FE as desired. However, while setting the FE configuration
 * determines how the device is configured at the package, it is the PCB the
 * device is mounted on (the EM) that determines how those signals are routed.
 * So while the FE is configurable, how signals are used is fixed by the EM.
 * As can be seen, the value of RF_FE_MODE_AND_BIAS is organized by the EM
 * board as defined by EMs produced by Texas Instruments Inc. How the device
 * is mounted, routed, and configured for a user product would of course be
 * user defined, and the value of RF_FE_MODE_AND_BIAS would have to be set
 * accordingly; the user could even dispense with the conditional board
 * compiles entirely. So too with the usage of the Tx Power tables.
 */
#define RF_FE_DIFFERENTIAL               0      //!< Differential
#define RF_FE_SINGLE_ENDED_RFP           1      //!< Single Ended RFP
#define RF_FE_SINGLE_ENDED_RFN           2      //!< Single Ended RFN
#define RF_FE_ANT_DIVERSITY_RFP_FIRST    3      //!< Antenna Diversity RFP First
#define RF_FE_ANT_DIVERSITY_RFN_FIRST    4      //!< Antenna Diveristy RFN First
#define RF_FE_SINGLE_ENDED_RFP_EXT_PINS  5      //!< Single Ended RFP External Pins
#define RF_FE_SINGLE_ENDED_RFN_EXT_PINS  6      //!< Single Ended RFN External Pins
//
#define RF_FE_INT_BIAS                   (0<<3) //!< Internal Bias
#define RF_FE_EXT_BIAS                   (1<<3) //!< External Bias

// Tx Power
// CC254x (dBm):  -23, -6, 0, 1
// CC26xx (dBm):  -21, -18, -15, -12, -9, -6, -3, 0..5
// CC13x2P (dBm): -10, -5, 0..6, 9..20
#define TX_POWER_20_DBM                  20    //!< 20 dBm
#define TX_POWER_19_DBM                  19    //!< 19 dBm
#define TX_POWER_18_DBM                  18    //!< 18 dBm
#define TX_POWER_17_DBM                  17    //!< 17 dBm
#define TX_POWER_16_DBM                  16    //!< 16 dBm
#define TX_POWER_15_DBM                  15    //!< 15 dBm
#define TX_POWER_14_DBM                  14    //!< 14 dBm
#define TX_POWER_13_DBM                  13    //!< 13 dBm
#define TX_POWER_12_DBM                  12    //!< 12 dBm
#define TX_POWER_11_DBM                  11    //!< 11 dBm
#define TX_POWER_10_DBM                  10    //!< 10 dBm
#define TX_POWER_9_DBM                   9     //!< 9 dBm
#define TX_POWER_6_DBM                   6     //!< 6 dBm
/** @} End URF_RF_Front_Settings */

/**
 * @defgroup URF_TX_Power Micro BLE Stack TX Power Options
 * @{
 */
#define TX_POWER_5_DBM                   5    //!< 5 dBm
#define TX_POWER_4_DBM                   4    //!< 4 dBm
#define TX_POWER_3_DBM                   3    //!< 3 dBm
#define TX_POWER_2_DBM                   2    //!< 2 dBm
#define TX_POWER_1_DBM                   1    //!< 1 dBm
#define TX_POWER_0_DBM                   0    //!< 0 dBm
#define TX_POWER_MINUS_3_DBM             -3   //!< -3 dBm
#define TX_POWER_MINUS_5_DBM             -5   //!< -5 dBm
#define TX_POWER_MINUS_6_DBM             -6   //!< -6 dBm
#define TX_POWER_MINUS_9_DBM             -9   //!< -9 dBm
#define TX_POWER_MINUS_10_DBM            -10  //!< -10 dBm
#define TX_POWER_MINUS_12_DBM            -12  //!< -12 dBm
#define TX_POWER_MINUS_15_DBM            -15  //!< -15 dBm
#define TX_POWER_MINUS_18_DBM            -18  //!< -18 dBm
#define TX_POWER_MINUS_21_DBM            -21  //!< -21 dBm
/** @} End URF_TX_Power */
//
// Device Package and Evaluation Module (EM) Board
//
// The CC26xx device comes in three types of packages: 7x7, 5x5, 4x4.
// For each package, the user may change how the RF Front End (FE) is
// configured. The possible FE settings are provided as a set of defines.
// (The user can also set the FE bias, the settings of which are also provided
// as defines.) The user can change the value of RF_FE_MODE_AND_BIAS to
// configure the RF FE as desired. However, while setting the FE configuration
// determines how the device is configured at the package, it is the PCB the
// device is mounted on (the EM) that determines how those signals are routed.
// So while the FE is configurable, how signals are used is fixed by the EM.
// As can be seen, the value of RF_FE_MODE_AND_BIAS is organized by the EM
// board as defined by EMs produced by Texas Instruments Inc. How the device
// is mounted, routed, and configured for a user product would of course be
// user defined, and the value of RF_FE_MODE_AND_BIAS would have to be set
// accordingly; the user could even dispense with the conditional board
// compiles entirely. So too with the usage of the Tx Power tables. As can be
// seen in bleUserConfig.c, there are two tables, one for packages using a
// differential FE, and one for single-end. This too has been organized by TI
// defined EMs and would have to be set appropriately by the user.
//
// For example:
// Let's say you decided to build several boards using the CC26xx 4x4 package.
// For one board, you plan to use a differential RF FE, while on the other you
// wish to use a single ended RFN RF FE. You would then create your own board.h
// (located by your preprocessor Include path name) that contains one of two
// defines that you create (say MY_CC26xx_4ID and MY_CC26xx_4XS). Then you can
// define your own choice of RF FE conditionally based on these defines
// (completely replacing those shown below), as follows:
//
//#if defined(MY_CC26xx_4ID)
//
//  #define RF_FE_MODE_AND_BIAS           (RF_FE_DIFFERENTIAL |               \
//                                          RF_FE_INT_BIAS)
//#elif defined(MY_CC26xx_4XS)
//
//  #define RF_FE_MODE_AND_BIAS           (RF_FE_SINGLE_ENDED_RFN |           \
//                                          RF_FE_EXT_BIAS)
//#else // unknown device package
// :
//
// In this way, you can define your own board I/O settings, and configure your
// RF FE based on your own board defines.
//
// For additional details and examples, please see the Software Developer's
// Guide.
//

// RF Front End Mode and Bias Configuration

#if defined(CC26XX)
  #if defined(CC26X2)
    #define RF_FE_MODE_AND_BIAS           (RF_FE_DIFFERENTIAL |               \
                                           RF_FE_INT_BIAS)
  #else // unknown device package
    #error "***BLE USER CONFIG BUILD ERROR*** Unknown package type!"
  #endif // <board>
#elif defined(CC13XX)
  #if defined(CC13X2
    #define RF_FE_MODE_AND_BIAS           (RF_FE_DIFFERENTIAL |     \
                                           RF_FE_INT_BIAS)
  #elif defined(CC13X2P)
    #define RF_FE_MODE_AND_BIAS           (RF_FE_DIFFERENTIAL |     \
                                           RF_FE_EXT_BIAS)
  #elif defined(MY_CC26x2_BOARD)
    #define RF_FE_MODE_AND_BIAS           (RF_FE_SINGLE_ENDED_RFP | \
                                           RF_FE_EXT_BIAS)
  #else // unknown board package
    #error "***BLE USER CONFIG BUILD ERROR*** Unknown board type!"
  #endif // <board>
#else // unknown platform
  #error "ERROR: Unknown platform!"
#endif // <board>
/** @} End URF_Constants */


// CTE Config parameters
#define  CTE_CONNECTION_ENABLE                         (0)
#define  CTE_GENERIC_RX_ENABLE                         (1)
#define  CTE_SCANNER_ENABLE                            (2)
#define  CTE_USE_MCE_RAM                               (3)
#define  CTE_USE_RFE_RAM                               (4)
#define  CTE_USE_DUAL_RAM                              (5)
#define  CTE_FLOW_CONTROL                              (6)
#define  CTE_AUTO_COPY                                 (7)

// Supported CTE for generic Rx and allow using RFE Ram for sampling
#define CTE_CONFIG  (BV(CTE_CONNECTION_ENABLE) | \
                     BV(CTE_GENERIC_RX_ENABLE) | \
                     BV(CTE_SCANNER_ENABLE)    | \
                     BV(CTE_USE_MCE_RAM)       | \
                     BV(CTE_USE_RFE_RAM)       | \
                     BV(CTE_USE_DUAL_RAM)      | \
                     BV(CTE_FLOW_CONTROL)      | \
                     BV(CTE_AUTO_COPY))

// 4 us before start of sampling
#define CTE_OFFSET                                     (4)
#define CTE_REFERENCE_PERIOD                           (8)

// sampling rate on 1 Mbps and 2 Mbps packets
#define CTE_SAMPLING_CONFIG_1MHZ                       (1)
#define CTE_SAMPLING_CONFIG_2MHZ                       (2)
#define CTE_SAMPLING_CONFIG_3MHZ                       (3)
#define CTE_SAMPLING_CONFIG_4MHZ                       (4)

// set the CTE sampling rate 4Mhz (4 samples in 1us)
#define CTE_SAMPLING_CONFIG_1MBPS     (CTE_SAMPLING_CONFIG_4MHZ)
#define CTE_SAMPLING_CONFIG_2MBPS     (CTE_SAMPLING_CONFIG_4MHZ)

/*******************************************************************************
 * TYPEDEFS
 */

/** @defgroup URF_Structures Micro RF Structures
 * @{
 */

PACKED_TYPEDEF_CONST_STRUCT
{
  int8   dBm;         //!< dBm
  uint16 txPowerVal;  //!< Tx Power Value
} ubTxPowerVal_t;     //!< Tx Power Value

PACKED_TYPEDEF_CONST_STRUCT
{
  ubTxPowerVal_t* pTxPowerVals;   //!< pointer to Tx Power Values
  uint8           numTxPowerVal;  //!< Number of Tx Power Values
} ubTxPowerTable_t;               //!< Tx Power Table

/** @} End URF_Structures */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/** @brief RF Core patch */
extern const RF_Mode          ubRfMode;

/** @brief Overrides for CMD_RADIO_SETUP */
extern regOverride_t         *ubRfRegOverride;

/** @brief TX Power table */
extern const ubTxPowerTable_t ubTxPowerTable;

/** @brief RF frontend mode bias */
extern const uint8_t          ubFeModeBias;

#ifdef __cplusplus
}
#endif

#endif /* BLE_USER_CONFIG_H */

/** @} End URF */

/** @} */ // end of Micro_BLE_Stack
