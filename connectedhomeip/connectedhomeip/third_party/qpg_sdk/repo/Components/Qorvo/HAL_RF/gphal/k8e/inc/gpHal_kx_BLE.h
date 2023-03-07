/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _GPHAL_KX_BLE_H_
#define _GPHAL_KX_BLE_H_


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_HAL_BLE_ADV_VALIDATION_START_INDEX       5

#define GP_HAL_BLE_NR_OF_VALIDATION_SETTINGS        (BLE_MAX_VALIDATION_START_IDX + 1)

#define GPHAL_BLE_VALIDATION_THRESHOLD_LOW          6700
#define GPHAL_BLE_VALIDATION_THRESHOLD_MEDIUM       GPHAL_BLE_VALIDATION_THRESHOLD_LOW
#define GPHAL_BLE_VALIDATION_THRESHOLD_HIGH         GPHAL_BLE_VALIDATION_THRESHOLD_LOW

#define GPHAL_BLE_VALIDATION_HDR_THRESHOLD_LOW      GPHAL_BLE_VALIDATION_THRESHOLD_LOW
#define GPHAL_BLE_VALIDATION_HDR_THRESHOLD_MEDIUM   GPHAL_BLE_VALIDATION_THRESHOLD_LOW
#define GPHAL_BLE_VALIDATION_HDR_THRESHOLD_HIGH     GPHAL_BLE_VALIDATION_THRESHOLD_LOW

// Currently, the defaults are the ones that are calculated for the advertising channels.
// They are also used in the beginning of a connection (before the optimal ones are calculated)
#define GPHAL_BLE_VALIDATION_THRESHOLD_DEFAULT      GPHAL_BLE_VALIDATION_THRESHOLD_LOW
#define GPHAL_BLE_VALIDATION_INDEX_DEFAULT          5

#define GPHAL_BLE_PREAMBLE_THRESHOLD_DEFAULT        203

#define GPHAL_BLE_PHY_CHAN_BW                       2
#define GPHAL_BLE_PHY_CHAN_BASE_FREQ                2402
#define GPHAL_BLE_PHY_SPEEDC                        ((UInt32)300000)

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

// Conversion macro's to switch between channel types
#define GP_HAL_CONVERT_BLE_TO_FLL_CHANNEL(bleChannel)   GP_WB_READ_U8(GP_WB_BLE_MGR_CH_CONV_BLE_CHANNEL_0_ADDRESS+(bleChannel))
#define GP_HAL_CONVERT_BLE_TO_PHY_CHANNEL(bleChannel)   (GP_HAL_CONVERT_FLL_TO_BLEPHY_CHANNEL(GP_HAL_CONVERT_BLE_TO_FLL_CHANNEL(bleChannel)))
#define GP_HAL_CONVERT_PHY_TO_BLE_CHANNEL(phyChannel)   gpHal_PhyToBleChannel(phyChannel)

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef struct {
    // Input parameter
    UInt32 accessAddress;
    UInt16 threshHold_low;
    UInt16 threshHold_med;
    UInt16 threshHold_hig;
    UInt8  max_validation_n;
    Bool   isHighDataRate;
} gpHal_BleValidationInputParameters_t;

#define GPHAL_BLE_INIT_VALIDATION_INPUT_NDR(out, accessAddr)  do {\
   out.accessAddress = (accessAddr);\
   out.threshHold_low = GPHAL_BLE_VALIDATION_THRESHOLD_LOW;\
   out.threshHold_med = GPHAL_BLE_VALIDATION_THRESHOLD_MEDIUM;\
   out.threshHold_hig = GPHAL_BLE_VALIDATION_THRESHOLD_HIGH;\
   out.max_validation_n = BLE_MAX_VALIDATION_START_IDX;\
   out.isHighDataRate = false;\
} while(0)

#define GPHAL_BLE_INIT_VALIDATION_INPUT_HDR(out, accessAddr)  do {\
   out.accessAddress = (accessAddr);\
   out.threshHold_low = GPHAL_BLE_VALIDATION_HDR_THRESHOLD_LOW;\
   out.threshHold_med = GPHAL_BLE_VALIDATION_HDR_THRESHOLD_MEDIUM;\
   out.threshHold_hig = GPHAL_BLE_VALIDATION_HDR_THRESHOLD_HIGH;\
   out.max_validation_n = BLE_HDR_MAX_VALIDATION_START_IDX;\
   out.isHighDataRate = true;\
} while(0)

typedef struct {
    // Output parameters / result of validation algorithm
    UInt8 scores[GP_HAL_BLE_NR_OF_VALIDATION_SETTINGS];
    UInt8 numberOfSimilarities[GP_HAL_BLE_NR_OF_VALIDATION_SETTINGS];
    UInt8 firstSimilar[GP_HAL_BLE_NR_OF_VALIDATION_SETTINGS];
    UInt8 similarScore[GP_HAL_BLE_NR_OF_VALIDATION_SETTINGS];
    UInt8 validationStartIndex;
    UInt16 validationThresh;
    Bool fakePreambleFlag;
    UInt8 fakePreambleStartIndex;
    Bool isReliableAccessAddress;
} gpHal_BleValidationParameters_t;

typedef struct {
    // Input parameter
    UInt32 accessAddress;
    // Output parameters / result of validation algorithm
    UInt8 scores[GP_HAL_BLE_NR_OF_VALIDATION_SETTINGS];
    UInt8 numberOfSimilarities[GP_HAL_BLE_NR_OF_VALIDATION_SETTINGS];
    UInt8 firstSimilar[GP_HAL_BLE_NR_OF_VALIDATION_SETTINGS];
    UInt8 similarScore[GP_HAL_BLE_NR_OF_VALIDATION_SETTINGS];
    UInt8 validationStartIndex;
    UInt16 validationThresh;
    Bool fakePreambleFlag;
    UInt8 fakePreambleStartIndex;
} gpHal_BleValidationParameters_test_t;

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Function Prototypes
 *****************************************************************************/

UInt8 gpHal_PhyToBleChannel(UInt8 phyChannel);

// Validation
void gpHal_BleGetValidationParameters(gpHal_BleValidationParameters_t* pValidation, const gpHal_BleValidationInputParameters_t *inParams);


#endif //_GPHAL_KX_BLE_H_

