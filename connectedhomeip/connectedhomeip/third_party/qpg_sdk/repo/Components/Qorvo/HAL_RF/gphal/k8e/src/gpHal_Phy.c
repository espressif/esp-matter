/*
 * Copyright (c) 2017, Qorvo Inc
 *
 * gpHal_Phy.c
 *   This file contains the implementation of the common phy functions
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

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#include "gpHal.h"
#include "gpHal_DEFS.h"
#include "gpHal_Phy.h"
#include "gpSched.h"

//GP hardware dependent register definitions
#include "gpHal_HW.h"
#include "gpHal_reg.h"

//Debug
#include "hal.h"
#include "gpBsp.h"
#include "gpAssert.h"
#include "gpHal_kx_Rap.h"
#include "gpHal_kx_Phy.h"

#ifdef GP_COMP_TXMONITOR
#include "gpTxMonitor.h"
#endif //GP_COMP_TXMONITOR


#include "gpHal_RadioMgmt.h"
#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef Int16 gpHal_TxPowerx10_t;

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

// Best fit from simulated tx powers
// See: GP_P008_DD_10604_txpower_vs_code.xlsx

// for each dB value in range, we have a corresponding PA setting corresponding to the PA configuration in registers
// ..._PA_POWER_SETTINGS
//     ..000000  00        Desired Power for transmitted packet
//     .0......  00        chooses the low PA power curve when set
//     0.......  00        chooses the low ultra low PA power curve when set

#if defined(GP_BSP_ANTENNATUNECONFIG_10DBM_SINGLE_ENDED) || defined(GP_BSP_ANTENNATUNECONFIG_10DBM_DIFFERENTIAL)
#include "gpHal_PaLutBoost.h"
#else
#include "gpHal_PaLutNominal.h"
#endif

#define HalPhy_TxPowerSettings rap_tx_pa_lut
#define GP_HAL_PA_LOOKUP_OFFSET RAP_TX_PA_LUT_OFFSET

#define GP_HAL_PHY_TX_POWER(val)         ((val) & 0x3F)
#define GP_HAL_PHY_TX_PA_LOW(val)        !!((val) & RAP_TX_PA_LOW)
#define GP_HAL_PHY_TX_PA_ULTRALOW(val)   !!((val) & RAP_TX_PA_ULTRALOW)
#define GP_HAL_PHY_TX_PA_MODE(val)       ((val) & 0xC0)


#define GP_HAL_PHY_DBM_TO_INTERNAL_PA_SETTING(reqPower, correctedReqdBmTimes10, setting) do {                          \
        Int8 i;                                                                                                        \
        i = reqPower - GP_HAL_PA_LOOKUP_OFFSET;                                                                        \
        setting = HalPhy_TxPowerSettings[i]; /* default 0dBm */                                                        \
        /* linearly interpolate Pout value between two points */                                                       \
        if ((i + 1) < number_of_elements(HalPhy_TxPowerSettings))                                                      \
        {                                                                                                              \
            if(GP_HAL_PHY_TX_PA_MODE(HalPhy_TxPowerSettings[i]) != GP_HAL_PHY_TX_PA_MODE(HalPhy_TxPowerSettings[i+1])) \
            {                                                                                                          \
                GP_ASSERT_DEV_INT(i-1>=0); /* this case should not happpen for i == 0 */                               \
                /* assume that here are no two mode changes next to each other */                                      \
                GP_ASSERT_DEV_INT(GP_HAL_PHY_TX_PA_MODE(HalPhy_TxPowerSettings[i]) ==                                  \
                                  GP_HAL_PHY_TX_PA_MODE(HalPhy_TxPowerSettings[i-1]));                                 \
                setting = HalPhy_TxPowerSettings[i] + ((correctedReqdBmTimes10 - 10*(i + GP_HAL_PA_LOOKUP_OFFSET)) *   \
                      (HalPhy_TxPowerSettings[i] - HalPhy_TxPowerSettings[i-1]) + 5)/10;                               \
                      /* + 5 to do the correct rounding */                                                             \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                /* normal case: interpolate between i and i+1 */                                                       \
                setting = HalPhy_TxPowerSettings[i] + ((correctedReqdBmTimes10 - 10*(i + GP_HAL_PA_LOOKUP_OFFSET)) *   \
                      (HalPhy_TxPowerSettings[i+1] - HalPhy_TxPowerSettings[i]) + 5)/10;                               \
                      /* + 5 to do the correct rounding */                                                             \
            }                                                                                                          \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
                /* check that we didn't go beyond the top entry of the table */                                        \
                GP_ASSERT_DEV_INT(i < number_of_elements(HalPhy_TxPowerSettings));                                     \
                setting = HalPhy_TxPowerSettings[i];                                                                   \
        }                                                                                                              \
    } while (false);



// Last used Continuous Wave settings
static gpHalPhy_CurrentCWModeSettings_t HalPhy_CurrentCWModeSettings;

static gpHal_AntennaMode_t              HalPhy_AntennaMode;


// Local states for preparing radio for load
static UInt8 gpHalPhy_PlmeRxModeZbSaved;
static UInt8 gpHalPhy_PlmeDutyCyclingSaved;
static UInt8 gpHalPhy_RxAntIntSaved;

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

static void HalPhy_CalibrateFllChannel_Unprotected(gpHal_ChannelSlotIdentifier_t slotId, UInt8 fllChannel)
{
    HAL_DISABLE_GLOBAL_INT();

    rap_fll_set_channel(slotId, fllChannel);

    HAL_ENABLE_GLOBAL_INT();
}


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/


/* Because gpHal_GetDefaultTransmitPower only supports channels [11,26] */
/* CW ON:
    1. set_channel
    2.force_closed_loop(tx = True)
    3.force_antenna(mode='txt', port=0/1)
    4.force_pa_on()
*/
void gpHal_SetContinuousWaveMode( gpHal_ContinuousWaveMode_t mode, UInt8 channel, Int8 txpower, gpHal_AntennaSelection_t antenna)
{
    if (mode != gpHal_CW_Off)
    {
        gpHalRadioMgmtSynch_claimRadio();
    }

    if (mode == gpHal_CW_Off)
    {
        HAL_DISABLE_GLOBAL_INT();
        HalPhy_CurrentCWModeSettings.mode = gpHal_CW_Off;
        rap_tx_cw_disable();
        HAL_ENABLE_GLOBAL_INT();


    }
    else
    {   // Code for switching CW On


        if (antenna == gpHal_AntennaSelection_Auto)
        {
            antenna = GP_HAL_TX_ANTENNA;
        }

        gpHal_PbmTxOptions_t gpHalPhy_TxOptions;
        gpHalPhy_TxOptions.antenna = antenna;
        UInt8  totalCalibrationCorrection = 0;

        if (mode == gpHal_CW_Modulated || mode == gpHal_CW_Unmodulated)
        {
            /* use fll channel */
            gpHalPhy_GetFemTxOptions(txpower, GP_HAL_CONVERT_MACPHY_TO_FLL_CHANNEL(channel), &gpHalPhy_TxOptions, GPHAL_TX_OPTIONS_FOR_TRANSMIT);
        }
        else
        {
            /* use fll channel */
            gpHalPhy_GetFemTxOptions(txpower, GP_HAL_CONVERT_BLEPHY_TO_FLL_CHANNEL(channel), &gpHalPhy_TxOptions, GPHAL_TX_OPTIONS_FOR_TRANSMIT);
        }



        /*Get all pa settings */

        //Calibrate channel
        if (mode == gpHal_CW_Modulated || mode == gpHal_CW_Unmodulated)
        {
            HalPhy_CalibrateFllChannel_Unprotected(GPHAL_ZB_CHANNEL_IDX, channel-11);
        }
        else
        {
            //use static _Unprotected flavor of the file as the protection is handled in gpHal_BleTestDisable/ReEnebleBleMgr() in gpTest
            HalPhy_CalibrateFllChannel_Unprotected(GPHAL_BLE_CHANNEL_IDX, channel+16);
        }


        {
            /* rap CW code assumes that the channel is set, calibrated and no other activity is ongoing */

            /* translate to variables needed by rap functions */
            UInt8 chidx;

            if (mode == gpHal_CW_Modulated || mode == gpHal_CW_Unmodulated)
            {
                chidx = GPHAL_ZB_CHANNEL_IDX;
            }
            else
            {
                chidx = GPHAL_BLE_CHANNEL_IDX;
                if      (mode == gpHal_CW_Ble_UnModulated)
                {
                    GP_WB_WRITE_RIB_TX_PHY_MODE(GP_WB_ENUM_BLE_TRANSMITTER_MODE_BLE);
                }
                else if (mode == gpHal_CW_Ble_Modulated)
                {
                    GP_WB_WRITE_RIB_TX_PHY_MODE(GP_WB_ENUM_BLE_TRANSMITTER_MODE_BLE);
                }
                else if (mode == gpHal_CW_Ble_HDRModulated)
                {
                    GP_WB_WRITE_RIB_TX_PHY_MODE(GP_WB_ENUM_BLE_TRANSMITTER_MODE_BLE_HDR);
                }
            }
            /* In CW-mode the output-power is set using overwrite registers of the chip.
               During overwrite the channel-calibration values that are normaly set py the PA stage
               of the chip are not taken into account. The totalCalibrationCorrection is used to add the channelcalibration
               value manual to the output power. In normal mode the CWCorrection should be zero.
            */

            UInt8 power      = GP_HAL_PHY_TX_POWER(gpHalPhy_TxOptions.tx_power_setting) + totalCalibrationCorrection;
            Bool pa_low      = GP_HAL_PHY_TX_PA_LOW(gpHalPhy_TxOptions.tx_power_setting);
            Bool pa_ultralow = GP_HAL_PHY_TX_PA_ULTRALOW(gpHalPhy_TxOptions.tx_power_setting);
            Bool modulated   = (mode == gpHal_CW_Modulated||mode == gpHal_CW_Ble_Modulated||mode == gpHal_CW_Ble_HDRModulated);
            Bool tx          = true;

            rap_tx_cw_enable(chidx, power, pa_low, pa_ultralow, gpHalPhy_TxOptions.antenna, modulated, tx);

            // Save arguments, so that other modules can know the state of the CW
            HalPhy_CurrentCWModeSettings.mode = mode;
            HalPhy_CurrentCWModeSettings.channel = channel;
            HalPhy_CurrentCWModeSettings.txpower = txpower;
            HalPhy_CurrentCWModeSettings.antenna = antenna;

            /* Wait around 100 us before signalling that the FLL should be stable */
            HAL_WAIT_US(100);
            // Clear any interrupt that may have been raised during starting of the CW
            GP_WB_FLL_CLR_RX_OUT_OF_RANGE_INTERRUPT();
            GP_WB_FLL_CLR_TX_OUT_OF_LOCK_INTERRUPT();
            GP_WB_FLL_CLR_TX_OUT_OF_RANGE_INTERRUPT();

        }
    }
    if (mode == gpHal_CW_Off)
    {
        gpHalRadioMgmtSynch_releaseRadio();
    }
}

/* Return the last state of the CW mode */
gpHalPhy_CurrentCWModeSettings_t *gpHal_GetCurrentContinuousWaveModeSettings(void)
{
    return &HalPhy_CurrentCWModeSettings;
}


void gpHalPhy_GetFemTxOptions(gpHal_TxPower_t txPower, gpHal_FllChannel_t fllChannel, gpHal_PbmTxOptions_t* pTxOptions, Bool forAck)
{
    if (gpHal_DefaultTransmitPower == txPower)
    {
        if (fllChannel < GP_HAL_BLE_FLL_START_INDEX)
        {
            /* ZB case */
        #if defined(GP_COMP_GPHAL_MAC)
            txPower = gpHal_GetDefaultTransmitPower(GP_HAL_CONVERT_FLL_TO_MACPHY_CHANNEL(fllChannel));
        #else
            /* you asked for a ZB channel, but did not enable the GP_COMP_GPHAL_MAC */
            GP_ASSERT_DEV_INT(false);
        #endif
        }
        else
        {
            /* BLE case */
            // for now, BLE default transmit power is set to maximum
            txPower = GPHAL_MAX_TRANSMIT_POWER;
        }
    }
    else if(gpHal_MinTransmitPower == txPower)
    {
        txPower = GPHAL_MIN_TRANSMIT_POWER;
    }
    else if(gpHal_MaxTransmitPower == txPower)
    {
        txPower = GPHAL_MAX_TRANSMIT_POWER;
    }

    txPower = clamp(txPower, GPHAL_MIN_TRANSMIT_POWER, GPHAL_MAX_TRANSMIT_POWER);



    //don't overrule pTxOptions->antenna
    pTxOptions->ext_pa_required = false;

#if defined(GP_HAL_DIVERSITY_EXT_MODE_SIGNALS)
    pTxOptions->phy_mode = gpHalPhy_GetTxMode(txPower);
#else
    pTxOptions->phy_mode = 0;
#endif

    pTxOptions->tx_power_setting = gpHalPhy_GetTxPowerSetting(txPower, forAck);
}

UInt8 gpHalPhy_GetTxPowerSetting(gpHal_TxPower_t requested_txPower_dBm, Bool forAck)
{
    UInt8 setting;
    gpHal_TxPower_t    internalPa_requested_txPower_dBm;
    NOT_USED(forAck);
    internalPa_requested_txPower_dBm = requested_txPower_dBm;
    {
        const Int8 minVal = GP_HAL_PA_LOOKUP_OFFSET;
        const Int8 maxVal = GP_HAL_PA_LOOKUP_OFFSET + (Int8)sizeof(HalPhy_TxPowerSettings) - 1 /*taking setting for 0dBm into account*/;
        GP_ASSERT_DEV_INT((internalPa_requested_txPower_dBm >= minVal) && (internalPa_requested_txPower_dBm <= maxVal));

        internalPa_requested_txPower_dBm = clamp(internalPa_requested_txPower_dBm,minVal, maxVal);
    }

    setting = HalPhy_TxPowerSettings[internalPa_requested_txPower_dBm - GP_HAL_PA_LOOKUP_OFFSET];
    return setting;

}

void gpHal_CalibrateFllChannel(gpHal_ChannelSlotIdentifier_t slotId, UInt8 fllChannel)
{
    // claim the radio for management
    gpHalRadioMgmtSynch_claimRadio();

    HalPhy_CalibrateFllChannel_Unprotected(slotId,fllChannel);

    // release the interface
    gpHalRadioMgmtSynch_releaseRadio();
}

void gpHal_CalibrateFllChannelUnprotected(gpHal_ChannelSlotIdentifier_t slotId, UInt8 fllChannel)
{
    HalPhy_CalibrateFllChannel_Unprotected(slotId,fllChannel);
}

void gpHal_PrepareRadioForLoad(void)
{
    GP_ASSERT_DEV_EXT(GP_WB_READ_RADIO_ARB_MGMT_GRANTED() == 1);

    //save currently register configuration
    gpHalPhy_PlmeRxModeZbSaved      = GP_WB_READ_PLME_RX_MODE_ZB();
    gpHalPhy_RxAntIntSaved          = GP_WB_READ_RX_RX_ANTSEL_INT();
    gpHalPhy_PlmeDutyCyclingSaved   = GP_WB_READ_PLME_DUTY_CYCLING_START_TRANSITION();

    //Set RX as antena diversity, so the current load will always be the same, indepent of the current configuration
    GP_WB_WRITE_PLME_RX_MODE_ZB(GP_WB_ENUM_ZB_RECEIVER_MODE_LPL_AD); //Low power listening + antena diversity
    GP_WB_WRITE_RX_RX_ANTSEL_INT(GP_WB_ENUM_ANTSEL_INT_USE_PORT_FROM_DESIGN); //Rx antenna select internal: 0=ant_sel
    GP_WB_WRITE_PLME_DUTY_CYCLING_START_TRANSITION(GP_WB_ENUM_PHY_STATE_TRANSITION_GENERIC_TRANSITION_1); //generic_transition_1
}

void gpHal_RestoreRadioFromLoad(void)
{
    GP_ASSERT_DEV_EXT(GP_WB_READ_RADIO_ARB_MGMT_GRANTED() == 1);
    //restore saved registers
    GP_WB_WRITE_PLME_RX_MODE_ZB(gpHalPhy_PlmeRxModeZbSaved);
    GP_WB_WRITE_RX_RX_ANTSEL_INT(gpHalPhy_RxAntIntSaved);
    GP_WB_WRITE_PLME_DUTY_CYCLING_START_TRANSITION(gpHalPhy_PlmeDutyCyclingSaved);
}

#if defined(GP_DIVERSITY_JUMPTABLES)
Int8 gpHalPhy_GetMinTransmitPower(void)
{
    return GPHAL_MIN_TRANSMIT_POWER;
}

Int8 gpHalPhy_GetMaxTransmitPower(void)
{
    return GPHAL_MAX_TRANSMIT_POWER;
}
#endif //defined(GP_DIVERSITY_JUMPTABLES)

//-------------------------------------------------------------------------------------------------------
// External PA
//-------------------------------------------------------------------------------------------------------



void gpHal_InitPhy(void)
{
#ifdef GP_HAL_DIVERSITY_EXT_MODE_SIGNALS
    /* Define what to do with mode signals when tx and rx off */
    GP_WB_WRITE_TRX_EXT_MODE_CTRL_FOR_TRX_OFF(GP_BSP_PHY_MODE(GP_BSP_PHY_MODE_TRX_OFF_SIGNALS));
    GP_WB_WRITE_TRX_EXT_MODE_CTRL_FOR_RX_ON_ATT_CTRL_LOW(GP_BSP_PHY_MODE(GP_BSP_PHY_MODE_RX_DEFAULT_SIGNALS));
    GP_WB_WRITE_TRX_EXT_MODE_CTRL_FOR_RX_ON_ATT_CTRL_HIGH(GP_BSP_PHY_MODE(GP_BSP_PHY_MODE_RX_DEFAULT_SIGNALS));

#endif

    GP_WB_WRITE_RX_RX_ANTSEL_INT(GP_BSP_RX_ANTSEL_INT_DEFAULT);
    GP_WB_WRITE_TX_TX_ANTSEL_INT(GP_BSP_TX_ANTSEL_INT_DEFAULT);

    // Set Last used CW Mode settings to defaults
    HalPhy_CurrentCWModeSettings.mode = gpHal_CW_Off;
    HalPhy_CurrentCWModeSettings.channel = 0;
    HalPhy_CurrentCWModeSettings.txpower = gpHal_DefaultTransmitPower;
    HalPhy_CurrentCWModeSettings.antenna = gpHal_AntennaSelection_Unknown;

    //Enable lower level FLL isr masks
    GP_WB_WRITE_INT_CTRL_MASK_PHY_FLL_RX_OUT_OF_RANGE_INTERRUPT(1);
    GP_WB_WRITE_INT_CTRL_MASK_PHY_FLL_TX_OUT_OF_RANGE_INTERRUPT(1);
    GP_WB_WRITE_INT_CTRL_MASK_PHY_FLL_TX_OUT_OF_LOCK_INTERRUPT(1);



    gpHalRadioMgmt_init();
}

#if defined(GP_HAL_DIVERSITY_EXT_MODE_SIGNALS)
UInt8 gpHalPhy_GetTxMode(gpHal_TxPower_t txPower)
{
    return ( gpHalPhy_IsPaRequired(txPower) ? GP_BSP_PHY_MODE(GP_BSP_PHY_MODE_TX_PA_ENABLED_SIGNALS) : GP_BSP_PHY_MODE(GP_BSP_PHY_MODE_TX_DEFAULT_SIGNALS) );
}
#endif //GP_HAL_DIVERSITY_EXT_MODE_SIGNALS


void gpHal_SetRxLnaAttDuringTimeoutForRssiBasedAgcMode(Bool enable)
{
    GP_WB_WRITE_RX_LNA_AGC_ENABLE_ATT_DURING_TIMEOUT(enable);
}

UInt8 gpHal_GetPaPowerTableIndex(UInt8 externalOutputPower, UInt8 externalPAGain)
{
    return externalOutputPower - GP_HAL_PA_LOOKUP_OFFSET - externalPAGain;
}

gpHal_AntennaSelection_t gpHal_GetTxAntenna(void)
{
    /* make it clear this is obsolete on the current platform */
    GP_ASSERT_SYSTEM(false);
    return gpHal_AntennaSelection_Auto; /* to satisfy the compiler */
}

gpHal_AntennaSelection_t gpHal_GetRxAntenna(void)
{
    gpHal_AntennaSelection_t antennaSelection=gpHal_AntennaSelection_Unknown;

    if (HalPhy_AntennaMode == gpHal_AntennaModeDiversity)
    {
        antennaSelection = gpHal_AntennaSelection_Auto;
    }
    else if (HalPhy_AntennaMode == gpHal_AntennaModeAntenna0)
    {
        antennaSelection = gpHal_AntennaSelection_Ant0;
    }
    else if (HalPhy_AntennaMode == gpHal_AntennaModeAntenna1)
    {
        antennaSelection = gpHal_AntennaSelection_Ant1;
    }

    return antennaSelection;
}

gpHal_AntennaSelection_t gpHal_GetBleAntenna(void)
{
    /* same antenna as ZB, but can't have antenna diversity */
    return GP_HAL_ANTSEL_INT_TO_ANT(HalPhy_AntennaMode);
}

#ifdef GP_COMP_GPHAL_MAC
extern gpHal_RxModeConfig_t    gpHal_ConfiguredReceiverMode;
#endif //GP_COMP_GPHAL_MAC

void gpHal_SetRxAntenna(gpHal_AntennaSelection_t rxAntenna)
{
#ifdef GP_HAL_DIVERSITY_SINGLE_ANTENNA
    rxAntenna = GP_HAL_DIVERSITY_SINGLE_ANTENNA;
#endif
    if (rxAntenna == gpHal_AntennaSelection_Auto)
    {
        HalPhy_AntennaMode = gpHal_AntennaModeDiversity;
    }
    else if (rxAntenna == gpHal_AntennaSelection_Ant0)
    {
        HalPhy_AntennaMode = gpHal_AntennaModeAntenna0;
    }
    else if (rxAntenna == gpHal_AntennaSelection_Ant1)
    {
        HalPhy_AntennaMode = gpHal_AntennaModeAntenna1;
    }
#ifdef GP_COMP_GPHAL_MAC
    GP_WB_WRITE_RX_ANT_USE_ZB( GP_HAL_ANTSEL_INT_TO_ANT(HalPhy_AntennaMode) );
    if(gpHal_ConfiguredReceiverMode.rxMode != 0)
    {
        /* skip this if the gpHal_Mac is not initialised yet */
        gpHal_ConfiguredReceiverMode.antennaMode = HalPhy_AntennaMode;
        gpHal_SetRxModeConfig(&gpHal_ConfiguredReceiverMode);
    }
#endif //GP_COMP_GPHAL_MAC
}



STATIC_FUNC void gpHal_ResetFirFilter(void)
{
    /* reset values as documented in the addressmap.txt */
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF0(0);
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF1(0);
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF2(0);
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF3(0);
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF4(0);
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF5(0);
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF6(0);
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF7(0);
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF8(0);
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF9(0);
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF10(0);
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF11(0);
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF12(0);
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF13(0x283);
}

STATIC_FUNC void gpHal_SetFilterFir25(void)
{
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF6(2);
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF7(6);
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF8(14);
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF9(28);
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF10(50);
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF11(74);
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF12(95);
    GP_WB_WRITE_FLL_MSK_FILTER_COEFF13(104);
}

void gpHal_SetRadioFirFilter(gpHal_FirFilter_t firFilter)
{
    switch(firFilter)
    {
        case gpHal_FirFilter_None:
        {
            gpHal_ResetFirFilter();
            break;
        }
        case gpHal_FirFilter_FIR25:
        {
            gpHal_SetFilterFir25();
            break;
        }
        default:
        {
            GP_ASSERT_DEV_INT(false);
            break;
        }
    }
}

/*****************************************************************************
 *            Obsolote or for backward compatibility
 *****************************************************************************/

void gpHal_SetAntenna(gpHal_AntennaSelection_t rxAntenna)
{
    /* keep this for backward compatibility */
    gpHal_SetRxAntenna(rxAntenna);
}

Bool gpHal_GetRxAntennaDiversity(void)
{
    /* keep this for backward compatibility */
    return (gpHal_GetRxAntenna() == gpHal_AntennaSelection_Auto);
}
