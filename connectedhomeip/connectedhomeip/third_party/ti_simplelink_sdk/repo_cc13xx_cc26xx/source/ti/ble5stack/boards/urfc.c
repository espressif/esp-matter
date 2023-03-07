/******************************************************************************

 @file  urfc.c

 @brief User configurable variables for the Micro BLE Stack Radio.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated

 All rights reserved not granted herein.
 Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free,
 non-exclusive license under copyrights and patents it now or hereafter
 owns or controls to make, have made, use, import, offer to sell and sell
 ("Utilize") this software subject to the terms herein. With respect to the
 foregoing patent license, such license is granted solely to the extent that
 any such patent is necessary to Utilize the software alone. The patent
 license shall not apply to any combinations which include this software,
 other than combinations with devices manufactured by or for TI ("TI
 Devices"). No hardware patent is licensed hereunder.

 Redistributions must preserve existing copyright notices and reproduce
 this license (including the above copyright notice and the disclaimer and
 (if applicable) source code license limitations below) in the documentation
 and/or other materials provided with the distribution.

 Redistribution and use in binary form, without modification, are permitted
 provided that the following conditions are met:

   * No reverse engineering, decompilation, or disassembly of this software
     is permitted with respect to any software provided in binary form.
   * Any redistribution and use are licensed by TI for use only with TI Devices.
   * Nothing shall obligate TI to provide you with source code for the software
     licensed and provided to you in object code.

 If software source code is provided to you, modification and redistribution
 of the source code are permitted provided that the following conditions are
 met:

   * Any redistribution and use of the source code, including any resulting
     derivative works, are licensed by TI for use only with TI Devices.
   * Any redistribution and use of any object code compiled from the source
     code and any resulting derivative works, are licensed by TI for use
     only with TI Devices.

 Neither the name of Texas Instruments Incorporated nor the names of its
 suppliers may be used to endorse or promote products derived from this
 software without specific prior written permission.

 DISCLAIMER.

 THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/******************************************************************************
 * INCLUDES
 */

#include "urfc.h"
#include "hal_defs.h"
#include <ti_drivers_config.h>

/******************************************************************************
 * MACROS
 */

/******************************************************************************
 * CONSTANTS
 */

// Tx Power
#define UB_NUM_TX_POWER_VALUE (sizeof(ubTxPowerVal) / sizeof(ubTxPowerVal_t))

// Default Tx Power Index
#if defined(CC13X2P)
#define DEFAULT_TX_POWER               9
#else // !CC13X2
#define DEFAULT_TX_POWER               7
#endif // CC13X2

/******************************************************************************
 * TYPEDEFS
 */

/******************************************************************************
 * LOCAL VARIABLES
 */

/******************************************************************************
 * GLOBAL VARIABLES
 */

/* RF patch function pointers */
const RF_Mode ubRfMode =
{
  .rfMode      = RF_MODE_BLE,
#if defined(CC26XX_R2) || defined(CC26X2)
  .cpePatchFxn = rf_patch_cpe_multi_protocol_rtls,
  .mcePatchFxn = 0,
  .rfePatchFxn = 0,
#else /* !CC26XX_R2 */
  .cpePatchFxn = &rf_patch_cpe_ble,
  .mcePatchFxn = 0,
  .rfePatchFxn = &rf_patch_rfe_ble,
#endif /* CC26XX_R2 */
};

// RF Override Registers
// Note: Used with CMD_RADIO_SETUP; called at boot time and after wake.
// Note: Must be in RAM as these overrides may need to be modified at runtime
//       based on temperature compensation, although it is possible this may
//       be automated in CM0 in PG2.0.
#if defined(CC26XX)
  // CC26xx Normal Package with Flash Settings for 48 MHz device
  #if defined(CC26X2)
  regOverride_t pOverridesCommon[] = {
    0x00158000, // S2RCFG: Capture S2R from FrontEnd, on event (CM0 will arm)
    0x000E51D0, // After FRAC
    ((CTE_CONFIG << 16) | 0x8BB3), // Enable CTE capture
    ((CTE_OFFSET << 24) | ((CTE_SAMPLING_CONFIG_1MBPS | (CTE_SAMPLING_CONFIG_1MBPS << 4)) << 16) | 0x0BC3), // Sampling rate, offset
    0xC0080341, // Pointer to antenna switching table in next entry
    (uint32_t) NULL, // Pointer to antenna switching table
    (uint32_t) NULL,
     END_OVERRIDE };

   regOverride_t pOverrides1Mbps[] = {
     END_OVERRIDE };

   #if defined(BLE_V50_FEATURES) && (BLE_V50_FEATURES & (PHY_2MBPS_CFG | PHY_LR_CFG))
   regOverride_t pOverrides2Mbps[] = {
     END_OVERRIDE };

   regOverride_t pOverridesCoded[] = {
     END_OVERRIDE };
   #endif // PHY_2MBPS_CFG | PHY_LR_CFG

  #else // unknown device package
    #error "***BLE USER CONFIG BUILD ERROR*** Unknown package type!"
  #endif // <board>

#elif defined(USE_FPGA)
  regOverride_t pOverridesCommon[] = {
  #if defined(CC26X2)
    // CC2642, as update by Helge on 12/12/17: Common Overrides for BLE5
    0x000151D0,
    0x00041110,
    0x00000083,
    0x00800403,
    0x80000303,
    0x02980243,
    0x01080263,
    0x08E90AA3,
    0x00068BA3,
    0x0E490C83,
    0x00005100, // Update matched filter for wired input
    0x721C5104, // Update matched filter for wired input
    0x00725108, // Update matched filter for wired input
    0x48f450d4, // Update matched filter gain for wired input
    END_OVERRIDE };
  #endif // CC26X2

  regOverride_t pOverrides1Mbps[] = {
    0x02405320,
    0x010302A3,
    END_OVERRIDE };

  #if defined(BLE_V50_FEATURES) && (BLE_V50_FEATURES & (PHY_2MBPS_CFG | PHY_LR_CFG))
  regOverride_t pOverrides2Mbps[] = {
    0x02405320,
    0x00B502A3,
    END_OVERRIDE };

  regOverride_t pOverridesCoded[] = {
    0x01013487,
    0x02405320,
    0x069802A3,
    END_OVERRIDE };
  #endif // PHY_2MBPS_CFG | PHY_LR_CFG

  #if defined(CC13X2P)
  // high gain PA overrides
  regOverride_t pOverridesTx20[] = {
    TX20_POWER_OVERRIDE(0x1234),
    0x01C20703, // Function of loDivider, frontEnd, and PA (High)
    END_OVERRIDE };

  // default PA overrides
  regOverride_t pOverridesTxStd[] = {
    TX_STD_POWER_OVERRIDE(0x1234),
    0x05320703, // Function loDivider, frontEnd, and PA (Default)
    END_OVERRIDE };
  #endif //CC13X2P


#elif defined(CC13XX)
  #if defined(CC13X2)
  regOverride_t pOverridesCommon[] = {
    END_OVERRIDE };

  regOverride_t pOverrides1Mbps[] = {
    END_OVERRIDE };

  #if defined(BLE_V50_FEATURES) && (BLE_V50_FEATURES & (PHY_2MBPS_CFG | PHY_LR_CFG))
  regOverride_t pOverrides2Mbps[] = {
    END_OVERRIDE };

  regOverride_t pOverridesCoded[] = {
    END_OVERRIDE };
  #endif // PHY_2MBPS_CFG | PHY_LR_CFG

  #elif defined(CC13X2P)
  regOverride_t pOverridesCommon[] = {
    // List of hardware and configuration registers to override during common initialization of any Bluetooth 5 PHY format
    // Bluetooth 5: Reconfigure pilot tone length for high output power PA
    HW_REG_OVERRIDE(0x6024,0x4C20),
    // Bluetooth 5: Compensate for modified pilot tone length
    (uint32_t)0x01500263,
    END_OVERRIDE };

  regOverride_t pOverrides1Mbps[] = {
    // List of hardware and configuration registers to override when selecting Bluetooth 5, LE 1M PHY
    // Bluetooth 5: Reconfigure pilot tone length for high output power PA
    HW_REG_OVERRIDE(0x5320,0x05A0),
    // Bluetooth 5: Compensate for modified pilot tone length
    (uint32_t)0x017B02A3,
    END_OVERRIDE };

  #if defined(BLE_V50_FEATURES) && (BLE_V50_FEATURES & (PHY_2MBPS_CFG | PHY_LR_CFG))
  regOverride_t pOverrides2Mbps[] = {
    // List of hardware and configuration registers to override when selecting Bluetooth 5, LE 2M PHY
    // Bluetooth 5: Reconfigure pilot tone length for high output power PA
    HW_REG_OVERRIDE(0x5320,0x05A0),
    // Bluetooth 5: Compensate for modified pilot tone length
    (uint32_t)0x011902A3,
    END_OVERRIDE };

  regOverride_t pOverridesCoded[] = {
    // List of hardware and configuration registers to override when selecting Bluetooth 5, LE Coded PHY
    // Bluetooth 5: Reconfigure pilot tone length for high output power PA
    HW_REG_OVERRIDE(0x5320,0x05A0),
    // Bluetooth 5: Compensate for modified pilot tone length
    (uint32_t)0x07D102A3,
    END_OVERRIDE };
  #endif // PHY_2MBPS_CFG | PHY_LR_CFG

  // high gain PA overrides
  regOverride_t pOverridesTx20[] = {
    // The TX Power element should always be the first in the list
    TX20_POWER_OVERRIDE(0x003F5BB8),
    // The ANADIV radio parameter based on the LO divider (0) and front-end (0) settings
    (uint32_t)0x01C20703,
    END_OVERRIDE };

  // default PA overrides
  regOverride_t pOverridesTxStd[] = {
    // The TX Power element should always be the first in the list
    TX_STD_POWER_OVERRIDE(0x941E),
    // The ANADIV radio parameter based on the LO divider (0) and front-end (0) settings
    (uint32_t)0x05320703,
    END_OVERRIDE };

  #else // unknown board package
    #error "***BLE USER CONFIG BUILD ERROR*** Unknown board type!"
  #endif // <board>
#else // unknown platform
  #error "ERROR: Unknown platform!"
#endif // <board>

//
// Tx Power Table Used Depends on Device Package
//
const ubTxPowerVal_t ubTxPowerVal[] = {
#if defined(USE_FPGA)
  // Differential Output (same as CC2650EM_7ID for now)
    { TX_POWER_MINUS_21_DBM, 0x06C7 },
    { TX_POWER_MINUS_18_DBM, 0x06C9 },
    { TX_POWER_MINUS_15_DBM, 0x0C88 },
    { TX_POWER_MINUS_12_DBM, 0x108A },
    { TX_POWER_MINUS_9_DBM,  0x0A8D },
    { TX_POWER_MINUS_6_DBM,  0x204D },
    { TX_POWER_MINUS_3_DBM,  0x2851 },
    { TX_POWER_0_DBM,        0x3459 },
    { TX_POWER_1_DBM,        0x385C },
    { TX_POWER_2_DBM,        0x440D },
    { TX_POWER_3_DBM,        0x5411 },
    { TX_POWER_4_DBM,        0x6C16 },
    { TX_POWER_5_DBM,        0x941E }
#elif defined(CC26XX)
  #if defined(CC26X2)
  // Differential Output
    { TX_POWER_MINUS_21_DBM, 0x06C7 },
    { TX_POWER_MINUS_18_DBM, 0x06C9 },
    { TX_POWER_MINUS_15_DBM, 0x0C88 },
    { TX_POWER_MINUS_12_DBM, 0x108A },
    { TX_POWER_MINUS_9_DBM,  0x0A8D },
    { TX_POWER_MINUS_6_DBM,  0x204D },
    { TX_POWER_MINUS_3_DBM,  0x2851 },
    { TX_POWER_0_DBM,        0x3459 },
    { TX_POWER_1_DBM,        0x385C },
    { TX_POWER_2_DBM,        0x440D },
    { TX_POWER_3_DBM,        0x5411 },
    { TX_POWER_4_DBM,        0x6C16 },
    { TX_POWER_5_DBM,        0x941E }
  #else // unknown board package
    #error "***BLE USER CONFIG BUILD ERROR*** Unknown CC26x2 board type!"
  #endif // <board>
#elif defined(CC13XX)
  #if defined(CC13X2)
  // Tx Power Values (Pout, Tx Power)
      { TX_POWER_MINUS_21_DBM, 0x06C7 },
      { TX_POWER_MINUS_18_DBM, 0x06C9 },
      { TX_POWER_MINUS_15_DBM, 0x0C88 },
      { TX_POWER_MINUS_12_DBM, 0x108A },
      { TX_POWER_MINUS_9_DBM,  0x0A8D },
      { TX_POWER_MINUS_6_DBM,  0x204D },
      { TX_POWER_MINUS_3_DBM,  0x2851 },
      { TX_POWER_0_DBM,        0x3459 },
      { TX_POWER_1_DBM,        0x385C },
      { TX_POWER_2_DBM,        0x440D },
      { TX_POWER_3_DBM,        0x5411 },
      { TX_POWER_4_DBM,        0x6C16 },
      { TX_POWER_5_DBM,        0x941E }
 #elif defined(CC13X2P)
 // Tx Power Values (Pout, Tx Power)
     {TX_POWER_MINUS_21_DBM, RF_TxPowerTable_DefaultPAEntry( 7, 3, 0,  3) },    // 0x000006C7
     {TX_POWER_MINUS_18_DBM, RF_TxPowerTable_DefaultPAEntry( 9, 3, 0,  3) },    // 0x000006C9
     {TX_POWER_MINUS_15_DBM, RF_TxPowerTable_DefaultPAEntry( 8, 2, 0,  6) },    // 0x00000C88
     {TX_POWER_MINUS_12_DBM, RF_TxPowerTable_DefaultPAEntry(10, 2, 0,  8) },    // 0x0000108A
     {TX_POWER_MINUS_10_DBM, RF_TxPowerTable_DefaultPAEntry(12, 2, 0, 11) },    // 0x0000168C
     {TX_POWER_MINUS_9_DBM,  RF_TxPowerTable_DefaultPAEntry(13, 2, 0,  5) },    // 0x00000A8D
     {TX_POWER_MINUS_6_DBM,  RF_TxPowerTable_DefaultPAEntry(13, 1, 0,  6) },    // 0x0000204D
     {TX_POWER_MINUS_5_DBM,  RF_TxPowerTable_DefaultPAEntry(14, 1, 0, 17) },    // 0x0000224E
     {TX_POWER_MINUS_3_DBM,  RF_TxPowerTable_DefaultPAEntry(17, 1, 0, 20) },    // 0x00002851
     {TX_POWER_0_DBM,        RF_TxPowerTable_DefaultPAEntry(25, 1, 0, 26) },    // 0x00003459
     {TX_POWER_1_DBM,        RF_TxPowerTable_DefaultPAEntry(28, 1, 0, 28) },    // 0x0000385C
     {TX_POWER_2_DBM,        RF_TxPowerTable_DefaultPAEntry(13, 0, 0, 34) },    // 0x0000440D
     {TX_POWER_3_DBM,        RF_TxPowerTable_DefaultPAEntry(17, 0, 0, 42) },    // 0x00005411
     {TX_POWER_4_DBM,        RF_TxPowerTable_DefaultPAEntry(22, 0, 0, 54) },    // 0x00006C16
     {TX_POWER_5_DBM,        RF_TxPowerTable_DefaultPAEntry(30, 0, 0, 74) },    // 0x0000941E
     {TX_POWER_6_DBM,        RF_TxPowerTable_HighPAEntry(46, 0, 1, 26,  7) },   // 0x8007352E
     {TX_POWER_9_DBM,        RF_TxPowerTable_HighPAEntry(40, 0, 1, 39, 41) },   // 0x80294F28
     {TX_POWER_10_DBM,       RF_TxPowerTable_HighPAEntry(23, 2, 1, 65,  5) },   // 0x80058397
     {TX_POWER_11_DBM,       RF_TxPowerTable_HighPAEntry(24, 2, 1, 29,  7) },   // 0x80073B98
     {TX_POWER_12_DBM,       RF_TxPowerTable_HighPAEntry(19, 2, 1, 16, 25) },   // 0x80192193
     {TX_POWER_13_DBM,       RF_TxPowerTable_HighPAEntry(27, 2, 1, 19, 13) },   // 0x800D279B
     {TX_POWER_14_DBM,       RF_TxPowerTable_HighPAEntry(24, 2, 1, 19, 27) },   // 0x801B2798
     {TX_POWER_15_DBM,       RF_TxPowerTable_HighPAEntry(23, 2, 1, 20, 39) },   // 0x80272997
     {TX_POWER_16_DBM,       RF_TxPowerTable_HighPAEntry(34, 2, 1, 26, 23) },   // 0x801735A2
     {TX_POWER_17_DBM,       RF_TxPowerTable_HighPAEntry(38, 2, 1, 33, 25) },   // 0x801943A6
     {TX_POWER_18_DBM,       RF_TxPowerTable_HighPAEntry(30, 2, 1, 37, 53) },   // 0x80354B9E
     {TX_POWER_19_DBM,       RF_TxPowerTable_HighPAEntry(36, 2, 1, 57, 59) },   // 0x803B73A4
     {TX_POWER_20_DBM,       RF_TxPowerTable_HighPAEntry(56, 2, 1, 45, 63) } }; // 0x803F5BB8
  #else // unknown board package
    #error "***BLE USER CONFIG BUILD ERROR*** Unknown CC135x board type!"
  #endif // <board>
#else // unknown platform
  #error "ERROR: Unknown platform!"
#endif // <board>
 };

/* RF frontend mode bias */
const uint8_t ubFeModeBias = RF_FE_MODE_AND_BIAS;

/* Overrides for CMD_RADIO_SETUP */
regOverride_t *ubRfRegOverride = pOverridesCommon;

/* Tx Power Table */
const ubTxPowerTable_t ubTxPowerTable = { ubTxPowerVal, UB_NUM_TX_POWER_VALUE };

/*******************************************************************************
 */
