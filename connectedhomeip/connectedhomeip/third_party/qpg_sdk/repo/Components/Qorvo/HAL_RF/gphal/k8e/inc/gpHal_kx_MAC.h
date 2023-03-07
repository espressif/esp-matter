/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
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

#ifndef _GPHAL_KX_MAC_H_
#define _GPHAL_KX_MAC_H_

#include "gpHal_Phy.h"
#include "gpHal_Statistics.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#ifndef GP_HAL_DIVERSITY_CCA_THRESHOLD_VALUE_OVERRULE_FOR_TEST
#define GPHAL_CCA_THRESHOLD_VALUE       (-75)
#else
#define GPHAL_CCA_THRESHOLD_VALUE GP_HAL_DIVERSITY_CCA_THRESHOLD_VALUE_OVERRULE_FOR_TEST
#endif //GP_HAL_DIVERSITY_CCA_THRESHOLD_VALUE_OVERRULE_FOR_TEST

// Maximum amount of 15.4 PBMs that can be queued for TX
#define GPHAL_QTA_MAX_SIZE              7

// PBM options only have 3 bit to store retry count
#define GPHAL_MAX_HW_TX_RETRY_AMOUNT 7

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

//PA settings k7
typedef struct gpHal_PaSettings_s {
    UInt8 PbmSettingAntselInt;
    Int8 internalDbmSetting;
    Bool  pa_low;
    Bool  pa_ultralow;
} gpHal_PaSettings_t;

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

extern gpHal_StatisticsMacCounter_t gpHal_StatisticsMacCounters;
extern UInt8 gpHal_MacState;
extern UInt8 gpHal_awakeCounter;

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

void gpHal_DataRequest_base(UInt8 pbmHandle);
void gpHalMac_CalibrateChannel(gpHal_ChannelSlotIdentifier_t srcId, UInt8 channel);

void gpHal_InitMAC(void);

void gpHalMac_RecalibrateChannels(void);
void gpHalMac_ConfigurePaSettings(gpHal_TxPower_t txPower, UInt8 channel, gpHal_PaSettings_t* pPaSettings);

#ifdef __cplusplus
}
#endif

#endif //_GPHAL_KX_MAC_H_
