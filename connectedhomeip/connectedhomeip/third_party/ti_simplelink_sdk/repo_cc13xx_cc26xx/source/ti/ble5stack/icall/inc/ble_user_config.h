/******************************************************************************
 @file:       ble_user_config.h

 @brief:    This file contains user configurable variables for the BLE
            Controller and Host. This file is also used for defining the
            type of RF Front End used with the TI device, such as using
            a Differential front end with External Bias, etc. Please see
            below for more detail.

            To change the default values of configurable variables:
              - Include the followings in your application main.c file:
                #ifndef USE_DEFAULT_USER_CFG

                #include "ble_user_config.h"

                // BLE user defined configuration
                bleUserCfg_t user0Cfg = BLE_USER_CFG;

                #endif // USE_DEFAULT_USER_CFG
              - Set the preprocessor symbol MAX_NUM_BLE_CONNS, MAX_NUM_PDU,
                MAX_PDU_SIZE, L2CAP_NUM_PSM or L2CAP_NUM_CO_CHANNELS
                to a desired value in your application project.
              - Include "ble_user_config.h" in your stack OSAL_ICallBle.c
                file.
              - Call setBleUserConfig at the start of stack_main. Actually,
                it is okay to set the variables anywhere in stack_main as
                long as it is BEFORE osal_init_system, but best to set at
                the very start of stack_main.

            Note: User configurable variables are only used during the
                  initialization of the Controller and Host. Changing
                  the values of these variables after this will have no
                  effect.

            Note: To use the default user configurable variables, define
                  the preprocessor symbol USE_DEFAULT_USER_CFG in your
                  application project.

            For example:
              - In your application main.c, include:
                #ifndef USE_DEFAULT_USER_CFG

                #include "bleUserConfig.h"

                // BLE user defined configuration
                bleUserCfg_t user0Cfg = BLE_USER_CFG;
                #endif // USE_DEFAULT_USER_CFG
              - In your application project, set the preprocessor symbol
                MAX_NUM_BLE_CONNS to 1 to change the maximum number of BLE
                connections to 1 from the default value of 3.
              - In your stack OSAL_ICallBle.c file, call setBleUserCfg to
                update the user configuration variables:
                #include "bleUserConfig.h"
                :
                int stack_main(void *arg)
                {
                  setBleUserConfig((bleUserCfg_t *)arg);
                  :
                }

            Default values:
              maxNumConns       : 1
              maxNumPDUs        : 5
              maxPduSize        : 27 (or 69 if Secure Connection enabled)
              maxNumPSM         : 3
              maxNumCoChannels  : 3
              maxWhiteListElems : 16
              maxResolvListElems: 10

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2014-2022, Texas Instruments Incorporated
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
#ifndef BLE_USER_CONFIG_H
#define BLE_USER_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */

#ifdef ICALL_JT
#include "icall_user_config.h"
#include <ti/drivers/rf/RF.h>
#endif /* ICALL_JT */

#include "ble_dispatch.h"
#include "hal_assert.h"

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

#endif // CC26X2 || CC13X2

// taken from RF_TxPowerTable_DEFAULT_PA_ENTRY in RF.h
#define RF_TxPowerTable_DefaultPAEntry(bias, gain, boost, coefficient)          \
  (uint32)(((bias) << 0) | ((gain) << 6) | ((boost) << 8) | ((coefficient) << 9) | (RF_TxPowerTable_DefaultPA << 31))

#if defined(CC13X2P)

// taken from RF_TxPowerTable_HIGH_PA_ENTRY in RF.h
#define RF_TxPowerTable_HighPAEntry(bias, ibboost, boost, coefficient, ldotrim) \
  (uint32)(((bias) << 0) | ((ibboost) << 6) | ((boost) << 8) | ((coefficient) << 9) | ((ldotrim) << 16) | (RF_TxPowerTable_HighPA << 31))

#endif // CC13X2P

/*******************************************************************************
 * CONSTANTS
 */

// Defines only required by Application.
// Note: ICALL_STACK0_ADDR is assigned the entry point of the stack image and
//       is only defined for the Application project
#if defined(ICALL_STACK0_ADDR)

#include <ti_drivers_config.h>

// RF Front End Settings
// Note: The use of these values completely depends on how the PCB is laid out.
//       Please see Device Package and Evaluation Module (EM) Board below.
#define RF_FE_DIFFERENTIAL              0
#define RF_FE_SINGLE_ENDED_RFP          1
#define RF_FE_SINGLE_ENDED_RFN          2
#define RF_FE_ANT_DIVERSITY_RFP_FIRST   3
#define RF_FE_ANT_DIVERSITY_RFN_FIRST   4
#define RF_FE_SINGLE_ENDED_RFP_EXT_PINS 5
#define RF_FE_SINGLE_ENDED_RFN_EXT_PINS 6
//
#define RF_FE_INT_BIAS                  (0<<3)
#define RF_FE_EXT_BIAS                  (1<<3)

// This is a common file for the legacy and sysconfig examples,
// the parameters under ifndef SYSCFG are defined in this file for
// the legacy examples and generated using the sysconfig tool for
// the sysconfig examples
#ifndef SYSCFG
// Maximum number of BLE connections. It should be set based on the
// device GAP role. Here're some recommended values:
//      * Central:     3
//      * Peripheral:  1
//      * Observer:    0
//      * Broadcaster: 0
// Note: When the GAP role includes Peripheral and no v4.1 Controller features
//       are configured, MAX_NUM_BLE_CONNS must not be greater than 1
#ifndef MAX_NUM_BLE_CONNS
  #define MAX_NUM_BLE_CONNS             1
#endif

// bitmask of extended stack settings
#ifndef EXTENDED_STACK_SETTINGS
#define EXTENDED_STACK_SETTINGS         0x00
#endif

// bitmask which enables the offset of the overrides for BAW
#define CC2652RB_OVERRIDE_USED          0x02

// Maximum number of BLE HCI PDUs. If the maximum number connections (above)
// is set to 0 then this number should also be set to 0.
#ifndef MAX_NUM_PDU
  #define MAX_NUM_PDU                   5
#endif

// Maximum size in bytes of the BLE HCI PDU. Valid range: 27 to 255
// The maximum ATT_MTU is MAX_PDU_SIZE - 4.
#ifndef MAX_PDU_SIZE
  #define MAX_PDU_SIZE                  69
#endif
#endif //SYSCFG

// Maximum number of L2CAP Protocol/Service Multiplexers (PSM)
#ifndef L2CAP_NUM_PSM
  #define L2CAP_NUM_PSM                 3
#endif

// Maximum number of L2CAP Connection Oriented Channels
#ifndef L2CAP_NUM_CO_CHANNELS
  #define L2CAP_NUM_CO_CHANNELS         8
#endif

#ifndef MAX_NUM_WL_ENTRIES
#define MAX_NUM_WL_ENTRIES             16  // at 8 bytes per WL entry
#endif

#ifndef CFG_MAX_NUM_RL_ENTRIES
#ifdef GAP_BOND_MGR
#define CFG_MAX_NUM_RL_ENTRIES             GAP_BONDINGS_MAX // at 8 bytes per WL entry
#else
#define CFG_MAX_NUM_RL_ENTRIES             10  // at 8 bytes per WL entry
#endif
#endif

// Number of CTE Sampling Buffers
#ifndef MAX_NUM_CTE_BUFS
#define MAX_NUM_CTE_BUFS               1  // one CTE samples buffer of ~2.5KB used for RF auto copy
#endif

// Number of CTE Sampling Buffers
#ifndef MAX_NUM_CTE_BUFS
#define MAX_NUM_CTE_BUFS               1  // one CTE samples buffer of ~2.5KB used for RF auto copy
#endif

// Inactivity timeout in us
#ifndef RF_INACTIVITY_TIMEOUT
  #if defined(DEBUG_SW_TRACE) || defined(USE_FPGA)
    #define RF_INACTIVITY_TIMEOUT       BIOS_WAIT_FOREVER
  #else // !USE_FPGA
    #define RF_INACTIVITY_TIMEOUT       0
  #endif // DEBUG_SW_TRACE | USE_FPGA
#endif

// Powerup time in us
#ifndef RF_POWER_UP_DURATION
  #define RF_POWER_UP_DURATION          0
#endif

// Powerup duration margin in us
#ifndef RF_POWER_UP_DURATION_MARGIN1
  #ifdef CACHE_AS_RAM
    #define RF_POWER_UP_DURATION_MARGIN  550
  #else // !CACHE_AS_RAM
    #define RF_POWER_UP_DURATION_MARGIN  (314)
  #endif // CACHE_AS_RAM
#endif

// RF Driver Error Callback
#ifndef RF_ERR_CB
  #define RF_ERR_CB                      pRfErrCb
#endif

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
// wish to use a single ended RFN RF FE. You would then create your own Board.h
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
#ifndef SYSCFG
#if defined(CC26XX)

#if defined(CC26X2) || defined(CC2652PSIP_LP)

  #define RF_FE_MODE_AND_BIAS           (RF_FE_DIFFERENTIAL |                 \
                                         RF_FE_INT_BIAS)
#else // unknown device package

  #error "***BLE USER CONFIG BUILD ERROR*** Unknown package type!"

#endif // <board>

#elif defined(CC13XX)

#if defined(CC13X2)

#define RF_FE_MODE_AND_BIAS           (RF_FE_DIFFERENTIAL |     \
                                       RF_FE_EXT_BIAS)
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
#endif //SYSCFG

#ifndef PM_STARTUP_MARGIN
  #define PM_STARTUP_MARGIN             300
#endif

#ifdef ICALL_JT
#define BLE_USER_CFG                    { &bleStackConfig,         \
                                          &driverTable,            \
                                          &boardConfig,            \
                                          &bleAppServiceInfoTable }

#else /* !(ICALL_JT) */

#ifdef EXT_HAL_ASSERT
  #define ASSERT_CBACK                  &halAssertCback
#else // !EXT_HAL_ASSERT
  #define ASSERT_CBACK                  &appAssertCback
#endif // EXT_HAL_ASSERT

#define BLE_USER_CFG                    { MAX_NUM_BLE_CONNS,    \
                                          MAX_NUM_PDU,          \
                                          MAX_PDU_SIZE,         \
                                          RF_FE_MODE_AND_BIAS,  \
                                          pOverridesCommon,     \
                                          pOverrides1Mbps,      \
#if defined(BLE_V50_FEATURES) && (BLE_V50_FEATURES & (PHY_2MBPS_CFG | PHY_LR_CFG))
                                          pOverrides2Mbps,      \
                                          pOverridesCoded,      \
#endif // PHY_2MBPS_CFG | PHY_LR_CFG
                                          &appTxPwrTbl,         \
                                          rfDriverTable,        \
                                          eccDriverTable,       \
                                          cryptoDriverTable,    \
                                          trngDriverTable,      \
                                          ASSERT_CBACK,         \
                                          L2CAP_NUM_PSM,        \
                                          L2CAP_NUM_CO_CHANNELS,\
                                          &pfnBMAlloc,          \
                                          &pfnBMFree,           \
                                          PM_STARTUP_MARGIN,    \
#if defined(CC13X2P)                                            
                                          &appTxPwrBackoffTbl,
                                          pOverridesTx20, \
                                          pOverridesTxStd,\
#endif // CC13X2P
                                          NULL }
#endif /* ICALL_JT */

// Make sure there's enough heap needed for BLE connection Tx buffers, which
// is based on MAX_PDU_SIZE and MAX_NUM_PDU configured by the application.
// The heap memory needed for BLE connection Tx buffers should not be more
// that 1/3 of the total ICall heap size (HEAPMGR_SIZE).
//
//  Notes: Over the Air (OTA) PDU Size = 27, and LL Header Size = 14
//         If HEAPMGR_SIZE = 0 then auto-size heap is being used
//
#if (MAX_NUM_BLE_CONNS > 0) && !defined(NO_HEAPSIZE_VALIDATE) && (HEAPMGR_SIZE != 0)
  #if  (((((MAX_PDU_SIZE / 27) + 1) * MAX_NUM_PDU) * (27 + 14)) > (HEAPMGR_SIZE / 3))
    #warning Not enough heap for configured MAX_NUM_PDU and MAX_PDU_SIZE! Adjust HEAPMGR_SIZE.
  #endif
#endif

// Check that the number of CTE buffers have not exceeded 16
// which is the maximum CTE count allowed by the Spec
// This means the controller will allocate a maximum of 40KB (16 * ~2.5KB)
#if (MAX_NUM_CTE_BUFS > 16)
  #warning Invalid MAX_NUM_CTE_BUFS - Should be less than or equal to 16.
#endif

#endif // !(CTRL_CONFIG | HOST_CONFIG)

// CTE Config parameters
#define  CTE_CONNECTION_ENABLE        (0)
#define  CTE_GENERIC_RX_ENABLE        (1)
#define  CTE_SCANNER_ENABLE           (2)
#define  CTE_USE_MCE_RAM              (3)
#define  CTE_USE_RFE_RAM              (4)
#define  CTE_USE_DUAL_RAM             (5)
#define  CTE_FLOW_CONTROL             (6)
#define  CTE_AUTO_COPY                (7)
// Enable Connection CTE and using which Rams for sampling with flow control and auto copy
#define  CTE_CONFIG (BV(CTE_CONNECTION_ENABLE) | \
                     BV(CTE_GENERIC_RX_ENABLE) | \
                     BV(CTE_SCANNER_ENABLE)    | \
                     BV(CTE_USE_MCE_RAM)       | \
                     BV(CTE_USE_RFE_RAM)       | \
                     BV(CTE_USE_DUAL_RAM)      | \
                     BV(CTE_FLOW_CONTROL)      | \
                     BV(CTE_AUTO_COPY))
// 4 us before start of sampling
#define CTE_OFFSET                    (4)
#define CTE_REFERENCE_PERIOD          (8)
// sampling rate on 1 Mbps and 2 Mbps packets
#define CTE_SAMPLING_CONFIG_1MHZ      (1)
#define CTE_SAMPLING_CONFIG_2MHZ      (2)
#define CTE_SAMPLING_CONFIG_3MHZ      (3)
#define CTE_SAMPLING_CONFIG_4MHZ      (4)
// sampling control flags
#define CTE_SAMPLING_CONTROL_DEFAULT_FILTERING      (0x00)      // skip antenna switching samples
#define CTE_SAMPLING_CONTROL_RF_RAW_NO_FILTERING    (0x01)      // copy i/q samples as is from RF RAM
#define CTE_SAMPLING_CONTROL_DEFAULT                CTE_SAMPLING_CONTROL_DEFAULT_FILTERING
// set the CTE sampling rate 4Mhz (4 samples in 1us)
#define CTE_SAMPLING_CONFIG_1MBPS     (CTE_SAMPLING_CONFIG_4MHZ)
#define CTE_SAMPLING_CONFIG_2MBPS     (CTE_SAMPLING_CONFIG_4MHZ)

/*******************************************************************************
 * TYPEDEFS
 */
typedef void (*pfnFastStateUpdate_t)(uint32_t stackType, uint32_t stackState);

#ifdef ICALL_JT

typedef struct
{
  uint32_t       powerUpDurationMargin;
  uint32_t       inactivityTimeout;
  uint32_t       powerUpDuration;
  RF_Callback   *pErrCb;
} userConfigRfDriverParams_t;

typedef struct
{
  uint8_t                     maxNumConns;
  uint8_t                     maxNumPDUs;
  uint8_t                     maxPduSize;
  uint8_t                     maxNumPSM;
  uint8_t                     maxNumCoChannels;
  uint8_t                     maxWhiteListElems;
  uint8_t                     maxResolvListElems;
  pfnBMAlloc_t                *pfnBMAlloc;
  pfnBMFree_t                 *pfnBMFree;
  userConfigRfDriverParams_t  rfDriverParams;
  ECCParams_CurveParams       *eccParams;
  pfnFastStateUpdate_t        fastStateUpdateCb;
  uint32_t                    bleStackType;
  uint32_t                    extStackSettings; // | reserved | use CC2652RB | MasterGuard |
                                                // |   31..2  |       1      |      0      |
  uint8                       maxNumCteBuffers;
} stackSpecific_t;

#else /* !(ICALL_JT) */

PACKED_TYPEDEF_CONST_STRUCT
{
  int8   Pout;
#if defined(CC13X2P)
  uint32 txPwrVal;
#else // !CC13X2P
  uint16 txPwrVal;
#endif // CC13X2P
} txPwrVal_t;

PACKED_TYPEDEF_CONST_STRUCT
{
  txPwrVal_t *txPwrValsPtr;
  uint8       numTxPwrVals;
  int8        defaultTxPwrVal;
} txPwrTbl_t;

#if defined(CC13X2P)
PACKED_TYPEDEF_CONST_STRUCT
{
  int8 txPwrBackoff1MPhy;
  int8 txPwrBackoff2MPhy;
  int8 txPwrBackoffCoded;
} txPwrBackoffVal_t;

PACKED_TYPEDEF_CONST_STRUCT
{
  txPwrBackoffVal_t *txPwrBackoffValsPtr;
} txPwrBackoffTbl_t;

#endif // CC13X2P

typedef const uint32 rfDrvTblPtr_t;

typedef const uint32 eccDrvTblPtr_t;

typedef const uint32 cryptoDrvTblPtr_t;

typedef const uint32 rtosApiPtr_t;

typedef const uint32 trngDrvTblPtr_t;

typedef struct
{
  uint8_t              maxNumConns;             // Max number of BLE connections
  uint8_t              maxNumPDUs;              // Max number of BLE PDUs
  uint8_t              maxPduSize;              // Max size of the BLE PDU.
  uint8_t              rfFeModeBias;            // RF Front End Mode and Bias (based on package)
  regOverride_t        *rfRegTbl;               // RF Common Override Register Table
  regOverride_t        *rfRegTbl1M;             // RF 1M Override Register Table
#if defined(BLE_V50_FEATURES) && (BLE_V50_FEATURES & (PHY_2MBPS_CFG | PHY_LR_CFG))
  regOverride_t        *rfRegTbl2M;             // RF 2M Override Register Table
  regOverride_t        *rfRegTblCoded;          // RF Coded Override Register Table
#endif // PHY_2MBPS_CFG | PHY_LR_CFG
  txPwrTbl_t           *txPwrTbl;               // Tx Power Table
  rfDrvTblPtr_t        *rfDrvTbl;               // RF Driver API Table
  eccDrvTblPtr_t       *eccDrvTbl;              // ECC Driver API Table
  cryptoDrvTblPtr_t    *cryptoDrvTbl;           // Crypto Driver API Table
  trngDrvTblPtr_t      *trngDrvTbl;             // TRNG Driver API Table
  void                 *nvintfFpsPtr;           // nvintf NV Driver Struct
  assertCback_t        *assertCback;            // Assert Callback
  uint8_t              maxNumPSM;               // Max number of L2CAP Protocol/Service Multiplexers
  uint8_t              maxNumCoChannels;        // Max number of L2CAP Connection Oriented Channels
  pfnBMAlloc_t         *pfnBMAlloc;             // BM allocator function pointer
  pfnBMFree_t          *pfnBMFree;              // BM de-allocator function pointer
  uint32_t             startupMarginUsecs;      // power management MARGIN
  pfnFastStateUpdate_t fastStateUpdateCb;       // Fast state update callback
  uint32_t             bleStackType;            // BLE Stack Type
#if defined(CC13X2P)
  txPwrBackOffTbl_t    *txPwrBackoffTbl;        // Tx power backoff table
  regOverride_t        *RegOverrideTxStdTbl;    // Default PA
  regOverride_t        *RegOverrideTx20Tbl;     // High power PA
#endif // CC13X2P

} bleUserCfg_t;
#endif /* ICALL_JT */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

extern regOverride_t pOverridesCommon[];
extern regOverride_t pOverrides1Mbps[];
#if defined(BLE_V50_FEATURES) && (BLE_V50_FEATURES & (PHY_2MBPS_CFG | PHY_LR_CFG))
extern regOverride_t pOverrides2Mbps[];
extern regOverride_t pOverridesCoded[];
#endif // PHY_2MBPS_CFG | PHY_LR_CFG

#if defined(CC13X2P)
extern txPwrBackoffTbl_t appTxPwrBackoffTbl;
extern regOverride_t pOverridesTx20[];
extern regOverride_t pOverridesTxStd[];
#endif

extern txPwrTbl_t    appTxPwrTbl;
extern rfDrvTblPtr_t rfDriverTable[];

extern eccDrvTblPtr_t eccDriverTable[];

extern cryptoDrvTblPtr_t  cryptoDriverTable[];
extern trngDrvTblPtr_t trngDriverTable[];

extern pfnBMAlloc_t pfnBMAlloc;
extern pfnBMFree_t  pfnBMFree;

extern uint16_t bleUserCfg_maxPduSize;
extern uint16_t llUserConfig_maxPduSize;

#ifdef ICALL_JT

extern RF_Callback pRfErrCb;

extern const drvTblPtr_t           driverTable;
extern const stackSpecific_t       bleStackConfig;
extern const boardConfig_t         boardConfig;
extern applicationService_t        bleAppServiceInfoTable;

#endif /* ICALL_JT */

/*********************************************************************
 * FUNCTIONS
 */
#ifdef ICALL_JT
extern void setBleUserConfig(icall_userCfg_t *userCfg);
#else /* !(ICALL_JT) */
extern void setBleUserConfig(bleUserCfg_t *userCfg);
#endif /* ICALL_JT */
extern void RegisterAssertCback(assertCback_t appAssertHandler);
extern void DefaultAssertCback(uint8 assertCause, uint8 assertSubcause);
extern assertCback_t appAssertCback; // only App's ble_user_config.c
extern assertCback_t halAssertCback; // only Stack's ble_user_config.c

#ifdef __cplusplus
}
#endif

#endif /* BLE_USER_CONFIG_H */
