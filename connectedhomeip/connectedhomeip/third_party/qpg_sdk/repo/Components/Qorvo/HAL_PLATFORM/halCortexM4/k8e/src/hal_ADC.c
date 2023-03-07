/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
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

#define GP_COMPONENT_ID GP_COMPONENT_ID_HALCORTEXM4

#include "hal.h"
#include "gpLog.h"
#include "gpHal_reg.h"
#include "gpSched.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/


#define NOMINAL_K8E_INFOPAGE_OFFSET_GP_ADC_VREF_SINGLE_ENDED 39360
#define NOMINAL_K8E_INFOPAGE_OFFSET_GP_ADC_VREF_DIFFERENTIAL 39322

#define HAL_ADC_NBR_OF_REJECTED_CONVERSIONS 2 /*3rd sample is stable*/
#define HAL_ADC_NBR_OF_REJECTED_SWAP_CONV 2

#define HAL_ADC_VBAT_BUFFER_BYPASS_SCALER_GAIN_ERROR  3

#define HAL_ADC_IS_ANIO_CHANNEL(channel)    ( (channel == GP_WB_ENUM_ADC_CHANNEL_ANIO0) || \
                                              (channel == GP_WB_ENUM_ADC_CHANNEL_ANIO1) || \
                                              (channel == GP_WB_ENUM_ADC_CHANNEL_ANIO2) || \
                                              (channel == GP_WB_ENUM_ADC_CHANNEL_ANIO3) || \
                                              (channel == GP_WB_ENUM_ADC_CHANNEL_VBAT) )

#define HAL_ADC_CHANNEL_ENUM(channel)  ( (channel == hal_AdcChannelANIO0) ? GP_WB_ENUM_ADC_CHANNEL_ANIO0 : \
                                        ((channel == hal_AdcChannelANIO1) ? GP_WB_ENUM_ADC_CHANNEL_ANIO1 : \
                                        ((channel == hal_AdcChannelANIO2) ? GP_WB_ENUM_ADC_CHANNEL_ANIO2 : \
                                        ((channel == hal_AdcChannelANIO3) ? GP_WB_ENUM_ADC_CHANNEL_ANIO3 : \
                                        ((channel == hal_AdcChannelBattery) ? GP_WB_ENUM_ADC_CHANNEL_VBAT : \
                                        ((channel == hal_AdcChannelTemperature) ? GP_WB_ENUM_ADC_CHANNEL_TEMP : 0xFF))))))



/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef struct
{
    UInt8 gainMode;
    Bool differential;
    Bool chopping;
    Int16 offset_adc;
    UInt16 VRef;
} HalAdc_ChannelParams_t;

/*****************************************************************************
 *                    Static Data
 *****************************************************************************/


#define HalADC_FreeBuffer 0xFF

/* Runtime state variables */
static UInt8 bufferA_channel = HalADC_FreeBuffer;
static UInt8 bufferB_channel = HalADC_FreeBuffer;
static UInt8 bufferC_channel = HalADC_FreeBuffer;
static Bool Hal_ADC_Initialized = false;
static halAdc_callback_t HalAdc_OutOfRangeInterruptCallback = NULL;

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/
static UInt32 halAdc_ApplyScalerGain(UInt32 input, UInt8 gainMode, Bool inverse);
static HalAdc_ChannelParams_t halADC_DetermineChannelParameters(UInt8 channel);

static UInt16 halADC_ConvertVoltageToRaw(UQ2_14 voltage, UInt8 channel);
static UQ2_14 halADC_ConvertRawToVoltage(UInt16 raw, UInt8 channel);

static UInt16 halADC_ConvertTemperatureToRaw(Q8_8 temperature);
static Q8_8 halADC_ConvertRawToTemperature(UInt16 raw);

static void halADC_ReleaseChannel(UInt8 channel);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

static UInt32 halAdc_ApplyScalerGain(UInt32 input, UInt8 gainMode, Bool inverse)
{

    /* We compensate for the scaling of the input voltage. */
    switch(gainMode)
    {
        case GP_WB_ENUM_ADC_SCALER_GAIN_X0_25:
            if (inverse)
            {
                input >>= 2;
            }
            else
            {
                input <<= 2;
            }
            break;
        case GP_WB_ENUM_ADC_SCALER_GAIN_X0_33:
            if (inverse)
            {
                input /= 3;
            }
            else
            {
                input *= 3;
            }
            break;
        case GP_WB_ENUM_ADC_SCALER_GAIN_X0_50:
            if (inverse)
            {
                input >>= 1;
            }
            else
            {
                input <<= 1;
            }
            break;
        case GP_WB_ENUM_ADC_SCALER_GAIN_X1_00:
            // no gain -> 1:1
            break;
        case GP_WB_ENUM_ADC_SCALER_GAIN_X2_00:
            if (inverse)
            {
                input <<= 1;
            }
            else
            {
                input >>= 1;
            }
            break;
        case GP_WB_ENUM_ADC_SCALER_GAIN_X1_50:
        case GP_WB_ENUM_ADC_SCALER_GAIN_X0_67:
        case GP_WB_ENUM_ADC_SCALER_GAIN_X9_00:
        default:
            /* no scaler accounting is implemented for these scalers */
            GP_LOG_PRINTF("non-supported gainmode=%d",0, gainMode);
            GP_ASSERT_SYSTEM(false);
            break;
    }

    return input;
}




static HalAdc_ChannelParams_t halADC_DetermineChannelParameters(UInt8 channel)
{
    HalAdc_ChannelParams_t params;

    params.gainMode = GP_WB_ENUM_ADC_SCALER_GAIN_X9_00; /* unsupported mode to trigger assert */
    params.differential = false;
    params.chopping = false;
    params.offset_adc = 0;

    /* We dynamically check if this channel its slot is in differential mode */
    if (HAL_ADC_IS_ANIO_CHANNEL(channel))
    {
        //Note: NrOfSlotsInUse = 0 means 1 slot is in use!!!
        const UInt8 NrOfSlotsInUse = GP_WB_READ_ADCIF_NBR_OF_SLOTS_IN_CYCLE();
        for(UIntLoop i = 0; i <= NrOfSlotsInUse; i++)
        {
            UInt16 SlotConfig = GP_WB_READ_U16((GP_WB_ADCIF_SLOT_A_CHANNEL_ADDRESS + i*4));

            if (channel == GP_WB_GET_ADCIF_SLOT_A_CHANNEL_FROM_SLOT_A_CONFIG(SlotConfig)) {


                params.gainMode = GP_WB_GET_ADCIF_SLOT_A_SCALER_GAIN_FROM_SLOT_A_CONFIG(SlotConfig);

                if (GP_WB_GET_ADCIF_SLOT_A_DIFFERENTIAL_MODE_FROM_SLOT_A_CONFIG(SlotConfig)) {
                    params.differential = true;
                }

                if (GP_WB_GET_ADCIF_SLOT_A_CHOPPING_ENABLED_FROM_SLOT_A_CONFIG(SlotConfig)) {
                    params.chopping = true;
                }
            }
         }
    }

    if (!params.chopping)
    {
        if (params.differential) {
            params.offset_adc = GP_WB_READ_NVR_GP_ADC_OFFSET_DIFFERENTIAL();
        } else {
            params.offset_adc = GP_WB_READ_NVR_GP_ADC_OFFSET_SINGLE_ENDED();
        }
    }

    if (params.differential) {
        params.VRef = GP_WB_READ_NVR_GP_ADC_VREF_DIFFERENTIAL();
        if (params.VRef == 0)
        {
           params.VRef = NOMINAL_K8E_INFOPAGE_OFFSET_GP_ADC_VREF_DIFFERENTIAL;
        }
    }
    else
    {
        params.VRef = GP_WB_READ_NVR_GP_ADC_VREF_SINGLE_ENDED();
        if (params.VRef == 0)
        {
           params.VRef = NOMINAL_K8E_INFOPAGE_OFFSET_GP_ADC_VREF_SINGLE_ENDED;
        }
    }

    return params;

}



static UInt16 halADC_ConvertVoltageToRaw(UQ2_14 voltage, UInt8 channel)
{
    UInt32 temp = voltage;
    UInt16 raw = 0;

    HalAdc_ChannelParams_t params = halADC_DetermineChannelParameters(channel);

    /* We convert 1/2^14V units into 1/2^24V unit */
    temp <<= 10;

    temp = halAdc_ApplyScalerGain(temp, params.gainMode, true);

    if (params.differential) {
        raw = (temp / params.VRef) + params.offset_adc + (512-HAL_ADC_VBAT_BUFFER_BYPASS_SCALER_GAIN_ERROR);
    } else {
        /* V/2^24 -> V/2^25 */
        temp <<=1;

        temp = temp / params.VRef;

        /* Check if offset would cause an underflow */
        if (((Int16)temp + (Int16)params.offset_adc) < 0)
        {
            // underflow situation, clip to zero
            raw = 0;
        }
        else
        {
            // all good
            raw = temp + params.offset_adc;
        }
    }

    return raw + 1;

}

static UQ2_14 halADC_ConvertRawToVoltage(UInt16 raw, UInt8 channel)
{

    UInt32 voltage = 0;

    HalAdc_ChannelParams_t params = halADC_DetermineChannelParameters(channel);


    if (params.differential) {
        /* raw value is between [0-1023] where 0 ==-VRef and 1023==VRef => we subtract 512 so that 0=>0V and 512=>1/2Vref
         * So the value of 1 raw equals VRef/2^9
         *
         * VRef_single is in units of V/2^15.
         *
         * "HAL_ADC_VBAT_BUFFER_BYPASS_SCALER_GAIN_ERROR" is an error determined by the characterisation team.
         *
         * When we multiply both we get a value in units of V/2^24
         *
         *     we are assuming we won't get negative values, which is expected as this driver only uses differential mode
         *     for measuring TEMP and VBAT, where we expect we will not reach the extreme values (+/-VRef).
         */

        voltage = ((raw-(512-HAL_ADC_VBAT_BUFFER_BYPASS_SCALER_GAIN_ERROR)-params.offset_adc ) * params.VRef);
    } else {

        /* raw value is between [0-1023] where 0 ==0 and 1023==VRef
         * So the value of 1 raw equals VRef/2^10

         * VRef_single is in units of 1/2^25V but already devided by 1023 so actually units of V/2^15.
         *
         * When we multiply both we get a value in units of V/2^25, but we want V/2^24, so shift once more.
         */
        if(raw > params.offset_adc)
        {
            voltage = ((raw - params.offset_adc) * params.VRef); // result in V/2^25
        }

        /* V/2^25 -> V/2^24 */
        voltage >>=1;
    }

    voltage = halAdc_ApplyScalerGain(voltage, params.gainMode, false);

    /* We convert 1/2^24V units into 1/2^14V unit */
    voltage >>= 10;


    if(voltage > 0xFFFF)
    {
        voltage = 0xFFFF;
    }

    // Result (in 1/16384 V, i.e. sixteen bit number with 2 bit before, and 14 bit after the comma)
    return (UQ2_14)voltage;
}

static UInt16 halADC_ConvertTemperatureToRaw(Q8_8 temperature)
{

    Int32 temp = temperature;
    UInt16 raw = 0;
    Int16 temperature_offset_calibration = GP_WB_READ_NVR_TEMP_OFFSET();
    UInt16 adc_reference_value_calibration = GP_WB_READ_NVR_GP_ADC_VREF_DIFFERENTIAL();

    //For backwards compatibility
    if (adc_reference_value_calibration == 0)
    {
        adc_reference_value_calibration = NOMINAL_K8E_INFOPAGE_OFFSET_GP_ADC_VREF_DIFFERENTIAL;
    }

    // Clip temperature between -128 and 127 degrees celsius

    if (temp > (HAL_ADC_TEMPERATURE_MAX_DEGREES_CELSIUS << 8))
    {
        temp = HAL_ADC_TEMPERATURE_MAX_DEGREES_CELSIUS << 8;
    }
    else if (temp < (HAL_ADC_TEMPERATURE_MIN_DEGREES_CELSIUS << 8))
    {
        temp = HAL_ADC_TEMPERATURE_MIN_DEGREES_CELSIUS << 8;
    }

    temp -= temperature_offset_calibration;
    temp += 69926;
    temp <<= 8;
    temp /= 571;
    temp <<= 8;
    temp /= adc_reference_value_calibration;
    temp += 512;

    raw = (UInt16)(temp & 0xFFFF);


    return raw;

}

static Q8_8 halADC_ConvertRawToTemperature(UInt16 raw)
{
    Int32 temp;

    UInt16 adc_reference_value_calibration;
    Int16 temperature_offset_calibration;

    adc_reference_value_calibration = GP_WB_READ_NVR_GP_ADC_VREF_DIFFERENTIAL();
    temperature_offset_calibration = GP_WB_READ_NVR_TEMP_OFFSET();

    //For backwards compatibility
    if (adc_reference_value_calibration == 0)
    {
        adc_reference_value_calibration = NOMINAL_K8E_INFOPAGE_OFFSET_GP_ADC_VREF_DIFFERENTIAL;
    }

    temp = raw;
    temp -= 512;
    temp *= adc_reference_value_calibration;
    temp >>= 8;
    temp *= 571;
    temp >>= 8;
    temp -= 69926;
    temp += temperature_offset_calibration;

    // There is a systematic offset on the measured temperature and this offset grows with temperature.

    // By fitting a second degree polynomial on temperature offset measurements at different temperatures,
    // we get the following relation between temperature offset and ADC measured temperature
    // [0.00037, 0.01668, -0.40124]

    // y = 0.00037x^2 + 0.01668x + -0.40124 where x is measured temperature in C, and y is (measured_temperature - actual temperature)

    // To improve temperature accuracy this offset is calculated and compensated here.

    // Note: representing Qx_y simply as Qy below to denote only decimal points.
    // Q8 -> Q7 to accomodate integer values greater than 127 in lower 16 bits
    Int32 temp_q7 = temp >> 1;

    // clip to limit value in 16 bits (9 bit integer, 7 bit decimal)
    // to avoid overflow once we do temp_q7 * temp_q7
    temp_q7 = clamp(temp_q7, -256 << 7, 255 << 7);          // [-256, 255] is max integer value in 9 bits.
    Q8_24 term1 = 24 * ((Int32)(temp_q7 * temp_q7) >> 6);   // Q16 * (Q7 * Q7 / 2^6) => Q24, where 24 == round(0.00037 * 2^16)
    Q8_24 term2 = 2186 * temp_q7;                           // Q17 * Q7 => Q24, where 2186 == round(0.01668 * 2^17)
    Q8_24 term3 = -6731690;                                  // -6731690 == round(-0.40124  * 2^24)
    Q8_24 temperature_offset = (term1 + term2 + term3);

    // reduce the calculated offset from measured temperature
    Q24_8 corr_temp = temp - (temperature_offset >> 16);

    // clamp temperature between minimum and maximum supported values
    corr_temp = clamp(corr_temp, HAL_ADC_TEMPERATURE_MIN_DEGREES_CELSIUS << 8, HAL_ADC_TEMPERATURE_MAX_DEGREES_CELSIUS << 8);
    return (Q8_8)(corr_temp);
}


static void halADC_ReleaseChannel(UInt8 channel)
{

    const UInt8 NrOfSlotsInUse = GP_WB_READ_ADCIF_NBR_OF_SLOTS_IN_CYCLE();
    UInt32 SlotConfigAddr;
    for(UIntLoop i = 0; i <= NrOfSlotsInUse; i++)
    {
        SlotConfigAddr = (GP_WB_ADCIF_SLOT_A_CHANNEL_ADDRESS + i*4);
        if (channel == GP_WB_GET_ADCIF_SLOT_A_CHANNEL_FROM_SLOT_A_CONFIG(GP_WB_READ_U16(SlotConfigAddr)))
        {
            /* Clear Config */
            GP_WB_WRITE_U16(SlotConfigAddr, 0x00);

            /* If not the last one of the running slots,
               fill empty slot with last one from used slots */
            GP_LOG_PRINTF("Remove Slot %i",0,i);
            if (i != NrOfSlotsInUse)
            {
                const UInt32 LastSlotConfigAddr = (GP_WB_ADCIF_SLOT_A_CHANNEL_ADDRESS + NrOfSlotsInUse*4);
                GP_LOG_PRINTF("Copy Config from %"PRIx32" to %"PRIx32,0,LastSlotConfigAddr, SlotConfigAddr);
                GP_WB_WRITE_U16(SlotConfigAddr, GP_WB_READ_U16(LastSlotConfigAddr));
            }
            if (NrOfSlotsInUse > 0) {
                GP_WB_WRITE_ADCIF_NBR_OF_SLOTS_IN_CYCLE(NrOfSlotsInUse-1);
            }
            return;
        }
    }
    /* Unknow slot given */
    GP_ASSERT_DEV_INT(false);
}

Bool hal_StartContinuousADCMeasurementWithParameters(hal_AdcChannel_t channel, Bool maxHold, Bool minHold, Bool outOfRange, UQ2_14 minThreshold, UQ2_14 maxThreshold, Bool anioRange3V6, Bool bypassUnityGainBuffer)
{

    GP_LOG_PRINTF("start channel measurement: %d",0, channel);
    UInt32 adcConfig = 0;
    UInt8 NrOfSlotsInUse = 0;
    UInt8 updateMode;

    GP_ASSERT_DEV_EXT(Hal_ADC_Initialized);

    NOT_USED(GP_WB_READ_ADCIF_FIFO_RESULT());

    channel = HAL_ADC_CHANNEL_ENUM(channel);

    GP_ASSERT_DEV_INT(!(maxHold && minHold));

    if(HalADC_FreeBuffer != bufferA_channel) {
        NrOfSlotsInUse++;
    }
    if(HalADC_FreeBuffer != bufferB_channel) {
        NrOfSlotsInUse++;
    }
    if(HalADC_FreeBuffer != bufferC_channel) {
        NrOfSlotsInUse++;
    }

    GP_WB_WRITE_ADCIF_TRIGGER_MODE(GP_WB_ENUM_ADC_TRIGGER_MODE_NEVER);

    if (NrOfSlotsInUse == 0)
    {
        GP_WB_WRITE_ADCIF_ENABLE(true);
        hal_SleepSetGotoSleepEnable(false);
    }

    /* Fill in channel in ADC configuration */
    GP_WB_SET_ADCIF_SLOT_A_CHANNEL_TO_SLOT_A_CONFIG(adcConfig,channel);

    if (minHold)
    {
        // Always trigger
        GP_WB_WRITE_ADCIF_PRESET_MIN_VALUE(0x3FF);
        GP_WB_WRITE_ADCIF_PRESET_MAX_VALUE(0x3FF);
    }
    else
    {
        // Always trigger
        GP_WB_WRITE_ADCIF_PRESET_MIN_VALUE(0x00);
        GP_WB_WRITE_ADCIF_PRESET_MAX_VALUE(0x00);
    }


    GP_WB_SET_ADCIF_SLOT_A_SCALER_BIAS_CC_PUP_TO_SLOT_A_CONFIG(adcConfig, 1);
    GP_WB_SET_ADCIF_SLOT_A_SMUX_ADC_BUF_N_BYPASS_TO_SLOT_A_CONFIG(adcConfig, bypassUnityGainBuffer);
    GP_WB_SET_ADCIF_SLOT_A_SMUX_ADC_BUF_P_BYPASS_TO_SLOT_A_CONFIG(adcConfig, bypassUnityGainBuffer);

    if (channel == GP_WB_ENUM_ADC_CHANNEL_TEMP)
    {
        GP_WB_WRITE_ADCIF_SMUX_TSENSOR_PUP(1);
        /* Temp. sensor will need Differential mode enabled, gain (max) 9x, chopping (to cancel offset) */
        GP_WB_SET_ADCIF_SLOT_A_SCALER_GAIN_TO_SLOT_A_CONFIG(adcConfig, GP_WB_ENUM_ADC_SCALER_GAIN_X9_00);
        GP_WB_SET_ADCIF_SLOT_A_CHOPPING_ENABLED_TO_SLOT_A_CONFIG(adcConfig, true);
        GP_WB_SET_ADCIF_SLOT_A_DIFFERENTIAL_MODE_TO_SLOT_A_CONFIG(adcConfig, true);

        GP_WB_SET_ADCIF_SLOT_A_SCALER_BIAS_CC_PUP_TO_SLOT_A_CONFIG(adcConfig, 0);
        GP_WB_SET_ADCIF_SLOT_A_SCALER_BIAS_CGM_PUP_TO_SLOT_A_CONFIG(adcConfig, 1);

    }
    else if (channel == GP_WB_ENUM_ADC_CHANNEL_VBAT)
    {
        // Unit Gain Buffer has to be bypassed for battery measurements.
        GP_ASSERT_SYSTEM(bypassUnityGainBuffer);
        GP_WB_SET_ADCIF_SLOT_A_SCALER_GAIN_TO_SLOT_A_CONFIG(adcConfig, GP_WB_ENUM_ADC_SCALER_GAIN_X0_25);
        GP_WB_SET_ADCIF_SLOT_A_CHOPPING_ENABLED_TO_SLOT_A_CONFIG(adcConfig, true);
        GP_WB_SET_ADCIF_SLOT_A_DIFFERENTIAL_MODE_TO_SLOT_A_CONFIG(adcConfig, true);
    }
    else /* ANIO */
    {
        if (anioRange3V6) {
            GP_WB_SET_ADCIF_SLOT_A_SCALER_GAIN_TO_SLOT_A_CONFIG(adcConfig, GP_WB_ENUM_ADC_SCALER_GAIN_X0_33);
        } else {
            GP_WB_SET_ADCIF_SLOT_A_SCALER_GAIN_TO_SLOT_A_CONFIG(adcConfig, GP_WB_ENUM_ADC_SCALER_GAIN_X1_00);
        }
        if(channel == GP_WB_ENUM_ADC_CHANNEL_ANIO0) //24/32 pin
        {
           GP_WB_WRITE_IOB_GPIO_21_INPUT_BUFFER_ENABLE(0);
        }
        else if(channel == GP_WB_ENUM_ADC_CHANNEL_ANIO1) //24/32 pin
        {
           GP_WB_WRITE_IOB_GPIO_22_INPUT_BUFFER_ENABLE(0);
        }
        else if(channel == GP_WB_ENUM_ADC_CHANNEL_ANIO2) //32 pin
        {
           GP_WB_WRITE_IOB_GPIO_17_INPUT_BUFFER_ENABLE(0);
        }
        else if(channel == GP_WB_ENUM_ADC_CHANNEL_ANIO3) //32-pin
        {
           GP_WB_WRITE_IOB_GPIO_18_INPUT_BUFFER_ENABLE(0);
        }
        else
        {
            GP_LOG_PRINTF("[ADC] ADC channel->gpio not defined ", 0);
            GP_ASSERT_DEV_EXT(0);
        }
    }


    /* Already write the configuration to the correct slot
     * such that in a out of range request, the channel parameters
     * can already be read out, to identify the conversion formula
     * for voltages and temperature thresholds, which rely on multiple settings
     * of the ADC channel */
    if(HalADC_FreeBuffer == bufferA_channel)
    {
        GP_WB_SET_ADCIF_SLOT_A_POST_TO_BUFFER_A_TO_SLOT_A_CONFIG(adcConfig,true);
        GP_WB_ADCIF_BUFFER_A_PRESET();
    }
    else if (HalADC_FreeBuffer == bufferB_channel)
    {
        GP_WB_SET_ADCIF_SLOT_A_POST_TO_BUFFER_B_TO_SLOT_A_CONFIG(adcConfig,true);
        GP_WB_ADCIF_BUFFER_B_PRESET();
    }
    else if (HalADC_FreeBuffer == bufferC_channel)
    {
        GP_WB_SET_ADCIF_SLOT_A_POST_TO_BUFFER_C_TO_SLOT_A_CONFIG(adcConfig,true);
        GP_WB_ADCIF_BUFFER_C_PRESET();
    }
    else
    {
        GP_ASSERT_SYSTEM(false);
        return false;
    }


    /* If out of range featured is requested, enable "Post to AWD" to activate interrupts */
    if (outOfRange)
    {
        GP_WB_SET_ADCIF_SLOT_A_POST_TO_AWD_TO_SLOT_A_CONFIG(adcConfig, true);
    }

    switch(NrOfSlotsInUse)
    {
        case 0:
        {
            GP_WB_WRITE_ADCIF_SLOT_A_CONFIG(adcConfig);
            GP_WB_WRITE_ADCIF_NBR_OF_SLOTS_IN_CYCLE(0);
            break;
        }
        case 1:
        {
            GP_WB_WRITE_ADCIF_SLOT_B_CONFIG(adcConfig);
            GP_WB_WRITE_ADCIF_NBR_OF_SLOTS_IN_CYCLE(1);
            break;
        }
        case 2:
        {
            GP_WB_WRITE_ADCIF_SLOT_C_CONFIG(adcConfig);
            GP_WB_WRITE_ADCIF_NBR_OF_SLOTS_IN_CYCLE(2);
            break;
        }
        case 3:
        {
            GP_WB_WRITE_ADCIF_SLOT_D_CONFIG(adcConfig);
            GP_WB_WRITE_ADCIF_NBR_OF_SLOTS_IN_CYCLE(3);
            break;
        }
        default:
        {
            GP_ASSERT_SYSTEM(false);
            return false;
        }
    }


    updateMode = maxHold ? GP_WB_ENUM_ADC_BUFFER_UPDATE_MODE_MAX_HOLD :
             minHold ? GP_WB_ENUM_ADC_BUFFER_UPDATE_MODE_MIN_HOLD :
             GP_WB_ENUM_ADC_BUFFER_UPDATE_MODE_NORMAL;


    if (!outOfRange)
    {
        if (minHold)
        {
            // Always trigger
            GP_WB_WRITE_ADCIF_PRESET_MIN_VALUE(0x3FF);
            GP_WB_WRITE_ADCIF_PRESET_MAX_VALUE(0x3FF);
        }
        else if (maxHold)
        {
            // Always trigger
            GP_WB_WRITE_ADCIF_PRESET_MIN_VALUE(0x00);
            GP_WB_WRITE_ADCIF_PRESET_MAX_VALUE(0x00);
        }
    }
    else
    {

        // Convert request thresholds from voltage/temperature to raw values
        if (channel == GP_WB_ENUM_ADC_CHANNEL_TEMP)
        {
            GP_WB_WRITE_ADCIF_PRESET_MIN_VALUE(halADC_ConvertTemperatureToRaw(minThreshold));
            GP_WB_WRITE_ADCIF_PRESET_MAX_VALUE(halADC_ConvertTemperatureToRaw(maxThreshold));
        }
        else
        {
            GP_WB_WRITE_ADCIF_PRESET_MIN_VALUE(halADC_ConvertVoltageToRaw(minThreshold, channel));
            GP_WB_WRITE_ADCIF_PRESET_MAX_VALUE(halADC_ConvertVoltageToRaw(maxThreshold, channel));
        }

        /* Preset AWD with converted values */
        GP_WB_ADCIF_AWD_PRESET();

        /* Enable ADCIF interrupt to CPU */
        GP_WB_WRITE_INT_CTRL_MASK_INT_ADCIF_INTERRUPT(1);

        /* Enable IRQ line */
        NVIC_EnableIRQ(ADCIF_IRQn);

        /* Enable both AWD interrupts */
        GP_WB_ADCIF_CLR_AWD_ABOVE_MAXIMUM_INTERRUPT();
        GP_WB_ADCIF_CLR_AWD_BELOW_MINIMUM_INTERRUPT();
        GP_WB_WRITE_INT_CTRL_MASK_ADCIF_AWD_ABOVE_MAXIMUM_INTERRUPT(1);
        GP_WB_WRITE_INT_CTRL_MASK_ADCIF_AWD_BELOW_MINIMUM_INTERRUPT(1);

        updateMode = GP_WB_ENUM_ADC_BUFFER_UPDATE_MODE_NORMAL;

    }


    if(HalADC_FreeBuffer == bufferA_channel)
    {
        bufferA_channel = channel;
        GP_WB_WRITE_ADCIF_BUFFER_A_UPDATE_MODE(updateMode);
        GP_WB_ADCIF_BUFFER_A_PRESET();
        GP_WB_ADCIF_CLR_BUFFER_A_UPDATED_INTERRUPT();

    }
    else if (HalADC_FreeBuffer == bufferB_channel)
    {
        bufferB_channel = channel;
        GP_WB_WRITE_ADCIF_BUFFER_B_UPDATE_MODE(updateMode);
        GP_WB_ADCIF_BUFFER_B_PRESET();
        GP_WB_ADCIF_CLR_BUFFER_B_UPDATED_INTERRUPT();

    }
    else if (HalADC_FreeBuffer == bufferC_channel)
    {
        bufferC_channel = channel;
        GP_WB_WRITE_ADCIF_BUFFER_C_UPDATE_MODE(updateMode);
        GP_WB_ADCIF_BUFFER_C_PRESET();
        GP_WB_ADCIF_CLR_BUFFER_C_UPDATED_INTERRUPT();
    }
    else
    {
        GP_ASSERT_SYSTEM(false);
        return false;
    }

    GP_LOG_PRINTF("Add Slot %i: %"PRIx32,0,NrOfSlotsInUse, adcConfig);


    GP_WB_WRITE_ADCIF_TRIGGER_MODE(GP_WB_ENUM_ADC_TRIGGER_MODE_ALWAYS);

    return true;
}


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void hal_InitADC(void)
{
    if(Hal_ADC_Initialized) {
        /* already initialized, do nothing */
        return;
    }

    Hal_ADC_Initialized = true;

    /* 4 Mhz analog clock is default on, this is also the default clock selection */

    GP_LOG_PRINTF("Init ADC",0);

    //Skip samples for stable conversion
    GP_WB_WRITE_ADCIF_NBR_OF_REJECTED_SWAP_CONV(HAL_ADC_NBR_OF_REJECTED_SWAP_CONV);


    /* GP_LOG_PRINTF("ADC_RESIDUAL_OFFSET: %d",0,GP_WB_READ_NVR_1_GP_ADC_RESIDUAL_OFFSET()); */
    /* GP_LOG_PRINTF("ADC_OFFSET: %d",0,GP_WB_READ_NVR_1_GP_ADC_OFFSET()); */
    /* GP_LOG_PRINTF("ADC_VREF_SINGLE_ENDED: %"PRIu16,0,GP_WB_READ_NVR_GP_ADC_VREF_SINGLE_ENDED()); */
    /* GP_LOG_PRINTF("ADC_VREF_DIFFERENTIAL: %"PRIu16,0,GP_WB_READ_NVR_GP_ADC_VREF_DIFFERENTIAL()); */

    GP_WB_WRITE_ADCIF_NBR_OF_REJECTED_CONVERSIONS(HAL_ADC_NBR_OF_REJECTED_CONVERSIONS);

    GP_WB_WRITE_ADCIF_TRIGGER_MODE(GP_WB_ENUM_ADC_TRIGGER_MODE_NEVER);
}


UQ2_14 halADC_ConvertToFixedPointValue(UInt16 raw, UInt8 channel)
{

    if(channel == GP_WB_ENUM_ADC_CHANNEL_TEMP)
    {

        return halADC_ConvertRawToTemperature(raw);
    }

    return halADC_ConvertRawToVoltage(raw, channel);
}


UQ2_14 hal_GetContinuousADCMeasurement(hal_AdcChannel_t channel)
{

    Bool timedOut = false;
    UInt16 raw=0;
    GP_ASSERT_DEV_EXT(Hal_ADC_Initialized);
    channel = HAL_ADC_CHANNEL_ENUM(channel);

    if (channel == bufferA_channel)
    {
        if(GP_WB_READ_ADCIF_BUFFER_A_UPDATE_MODE() == GP_WB_ENUM_ADC_BUFFER_UPDATE_MODE_NORMAL)
        {
            GP_DO_WHILE_TIMEOUT(!GP_WB_READ_ADCIF_UNMASKED_BUFFER_A_UPDATED_INTERRUPT(),100,&timedOut);
        }

        if(!timedOut)
        {
            GP_WB_ADCIF_CLR_BUFFER_A_UPDATED_INTERRUPT();
            raw = GP_WB_READ_ADCIF_BUFFER_A_RESULT();
        }
    }
    else if (channel == bufferB_channel)
    {

        if(GP_WB_READ_ADCIF_BUFFER_B_UPDATE_MODE() == GP_WB_ENUM_ADC_BUFFER_UPDATE_MODE_NORMAL)
        {
            GP_DO_WHILE_TIMEOUT(!GP_WB_READ_ADCIF_UNMASKED_BUFFER_B_UPDATED_INTERRUPT(),100,&timedOut);
        }

        if(!timedOut)
        {
            GP_WB_ADCIF_CLR_BUFFER_B_UPDATED_INTERRUPT();
            raw = GP_WB_READ_ADCIF_BUFFER_B_RESULT();
        }
    }
    else if (channel == bufferC_channel)
    {

        if(GP_WB_READ_ADCIF_BUFFER_C_UPDATE_MODE() == GP_WB_ENUM_ADC_BUFFER_UPDATE_MODE_NORMAL)
        {
            GP_DO_WHILE_TIMEOUT(!GP_WB_READ_ADCIF_UNMASKED_BUFFER_C_UPDATED_INTERRUPT(),100,&timedOut);
        }

        if(!timedOut)
        {
            GP_WB_ADCIF_CLR_BUFFER_C_UPDATED_INTERRUPT();
            raw = GP_WB_READ_ADCIF_BUFFER_C_RESULT();
        }
    }
    else
    {
        GP_ASSERT_SYSTEM(false);
        return 0;
    }

    if (!timedOut)
    {
        return halADC_ConvertToFixedPointValue(raw, channel);
    }
    else
    {
        /* Timeout or invalid slot */
        NOT_USED(raw);
        GP_ASSERT_DEV_EXT(false);
        return 0;
    }

}

void hal_ClearContinuousADCMeasurement(hal_AdcChannel_t channel)
{
    GP_ASSERT_DEV_EXT(Hal_ADC_Initialized);
    channel = HAL_ADC_CHANNEL_ENUM(channel);

    if(channel == bufferA_channel)
    {
        GP_WB_ADCIF_CLR_BUFFER_A_UPDATED_INTERRUPT();
        GP_WB_ADCIF_BUFFER_A_PRESET();
    }
    else if (channel == bufferB_channel)
    {
        GP_WB_ADCIF_CLR_BUFFER_B_UPDATED_INTERRUPT();
        GP_WB_ADCIF_BUFFER_B_PRESET();
    }
    else if (channel == bufferC_channel)
    {
        GP_WB_ADCIF_CLR_BUFFER_C_UPDATED_INTERRUPT();
        GP_WB_ADCIF_BUFFER_C_PRESET();
    }
}

void halAdc_ToggleOutOfRangeInterrupts(Bool enable)
{
    HAL_DISABLE_GLOBAL_INT();

    if(enable)
    {
        GP_WB_WRITE_INT_CTRL_MASK_ADCIF_AWD_BELOW_MINIMUM_INTERRUPT(1);
        GP_WB_WRITE_INT_CTRL_MASK_ADCIF_AWD_ABOVE_MAXIMUM_INTERRUPT(1);
    }
    else
    {
        GP_WB_WRITE_INT_CTRL_MASK_ADCIF_AWD_BELOW_MINIMUM_INTERRUPT(0);
        GP_WB_WRITE_INT_CTRL_MASK_ADCIF_AWD_ABOVE_MAXIMUM_INTERRUPT(0);
    }

    HAL_ENABLE_GLOBAL_INT();

}

void adcif_handler_impl(void)
{
    HAL_DISABLE_GLOBAL_INT();

    /* Only expecting above/below threshold interrupts */
    GP_ASSERT_SYSTEM(GP_WB_READ_INT_CTRL_MASKED_ADCIF_AWD_ABOVE_MAXIMUM_INTERRUPT() || GP_WB_READ_INT_CTRL_MASKED_ADCIF_AWD_BELOW_MINIMUM_INTERRUPT());

    if(GP_WB_READ_INT_CTRL_MASKED_ADCIF_AWD_ABOVE_MAXIMUM_INTERRUPT())
    {

        HalAdc_OutOfRangeInterruptCallback(hal_AdcAboveMaximum);

        GP_WB_WRITE_INT_CTRL_MASK_ADCIF_AWD_ABOVE_MAXIMUM_INTERRUPT(0);
        GP_WB_ADCIF_CLR_AWD_ABOVE_MAXIMUM_INTERRUPT();
    }

    if(GP_WB_READ_INT_CTRL_MASKED_ADCIF_AWD_BELOW_MINIMUM_INTERRUPT())
    {
        HalAdc_OutOfRangeInterruptCallback(hal_AdcBelowMinimum);

        GP_WB_WRITE_INT_CTRL_MASK_ADCIF_AWD_BELOW_MINIMUM_INTERRUPT(0);
        GP_WB_ADCIF_CLR_AWD_BELOW_MINIMUM_INTERRUPT();
    }

    HAL_ENABLE_GLOBAL_INT();

}


void hal_StopContinuousADCMeasurementWithOutOfRangeInterrupt(hal_AdcChannel_t channel)
{
    HalAdc_OutOfRangeInterruptCallback = NULL;

    /* Disable ADCIF interrupt to CPU */
    GP_WB_WRITE_INT_CTRL_MASK_INT_ADCIF_INTERRUPT(0);

    /* Disable IRQ line */
    NVIC_DisableIRQ(ADCIF_IRQn);

    /* Disable both AWD interrupts */
    GP_WB_WRITE_INT_CTRL_MASK_ADCIF_AWD_ABOVE_MAXIMUM_INTERRUPT(0);
    GP_WB_WRITE_INT_CTRL_MASK_ADCIF_AWD_BELOW_MINIMUM_INTERRUPT(0);
    GP_WB_ADCIF_CLR_AWD_ABOVE_MAXIMUM_INTERRUPT();
    GP_WB_ADCIF_CLR_AWD_BELOW_MINIMUM_INTERRUPT();

    hal_StopContinuousADCMeasurement(channel);
}

Bool hal_StartContinuousADCMeasurementWithOutOfRangeInterrupt(hal_AdcChannel_t channel, UQ2_14 minThreshold, UQ2_14 maxThreshold, Bool anioRange3V6, halAdc_callback_t cb )
{
   /* Check whether callback has already been set */
   if (HalAdc_OutOfRangeInterruptCallback != NULL)
   {
       return false;
   }

   HalAdc_OutOfRangeInterruptCallback = cb;

   // Legacy use cases: vbat measurement is only use case that need the bypass
   Bool bypassUnityGainBuffer = (channel == GP_WB_ENUM_ADC_CHANNEL_VBAT) ? true : false;

   return hal_StartContinuousADCMeasurementWithParameters(channel, false, false, true, minThreshold, maxThreshold, anioRange3V6, bypassUnityGainBuffer);
}

Bool hal_StartContinuousADCMeasurement(hal_AdcChannel_t channel, Bool maxHold, Bool minHold, Bool anioRange3V6)
{
    // Legacy use cases: vbat measurement is only use case that need the bypass
    Bool bypassUnityGainBuffer = (channel == GP_WB_ENUM_ADC_CHANNEL_VBAT) ? true : false;

    // legacy
    return hal_StartContinuousADCMeasurementWithParameters(channel,maxHold,minHold,false, 0,0, anioRange3V6, bypassUnityGainBuffer);
}

void hal_StopContinuousADCMeasurement(hal_AdcChannel_t channel)
{

    Bool timedOut;
    UInt16 NrOfSlotsInUse = 0;
    GP_ASSERT_DEV_EXT(Hal_ADC_Initialized);

    channel = HAL_ADC_CHANNEL_ENUM(channel);

    GP_WB_WRITE_ADCIF_TRIGGER_MODE(GP_WB_ENUM_ADC_TRIGGER_MODE_NEVER);

    if(channel == bufferA_channel)
    {
        bufferA_channel = HalADC_FreeBuffer;
    }
    else if (channel == bufferB_channel)
    {
        bufferB_channel = HalADC_FreeBuffer;
    }
    else if (channel == bufferC_channel)
    {
        bufferC_channel = HalADC_FreeBuffer;
    }

    if(HalADC_FreeBuffer != bufferA_channel) {
        NrOfSlotsInUse++;
    }
    if(HalADC_FreeBuffer != bufferB_channel) {
        NrOfSlotsInUse++;
    }
    if(HalADC_FreeBuffer != bufferC_channel) {
        NrOfSlotsInUse++;
    }

    //Restoring overrides for specific channels
    if (channel == GP_WB_ENUM_ADC_CHANNEL_TEMP)
    {
        GP_WB_WRITE_ADCIF_SMUX_TSENSOR_PUP(0);
    }
    else if(channel == GP_WB_ENUM_ADC_CHANNEL_VBAT)
    {
        // nothing to do.
    }
    else if(channel == GP_WB_ENUM_ADC_CHANNEL_ANIO0) //24/32 pin
    {
        GP_WB_WRITE_IOB_GPIO_21_INPUT_BUFFER_ENABLE(1);
    }
    else if(channel == GP_WB_ENUM_ADC_CHANNEL_ANIO1) //24/32 pin
    {
        GP_WB_WRITE_IOB_GPIO_22_INPUT_BUFFER_ENABLE(1);
    }
    else if(channel == GP_WB_ENUM_ADC_CHANNEL_ANIO2) //32 pin
    {
        GP_WB_WRITE_IOB_GPIO_17_INPUT_BUFFER_ENABLE(1);
    }
    else if(channel == GP_WB_ENUM_ADC_CHANNEL_ANIO3) //32 pin
    {
        GP_WB_WRITE_IOB_GPIO_18_INPUT_BUFFER_ENABLE(1);
    }
    else
    {
        GP_LOG_PRINTF("[ADC] ADC channel->gpio not defined ", 0);
        GP_ASSERT_DEV_EXT(0);
    }

    halADC_ReleaseChannel(channel);

    if (0 != NrOfSlotsInUse)
    {
        GP_WB_WRITE_ADCIF_TRIGGER_MODE(GP_WB_ENUM_ADC_TRIGGER_MODE_ALWAYS);
    }
    else
    {
        GP_DO_WHILE_TIMEOUT(GP_WB_READ_ADCIF_CONVERSION_CYCLE_BUSY(),50, &timedOut);
        NOT_USED(timedOut);
        GP_WB_WRITE_ADCIF_ENABLE(false);
        hal_SleepSetGotoSleepEnable(true);
    }
}

UInt8 hal_GetBufferUsedBy(hal_AdcChannel_t channel)
{
    channel = HAL_ADC_CHANNEL_ENUM(channel);

    if(channel == bufferA_channel)
    {
        return 0;
    }
    else if (channel == bufferB_channel)
    {
        return 1;
    }
    else if (channel == bufferC_channel)
    {
        return 2;
    }
    return 0xFF;
}

Bool hal_MeasureADC_8(hal_AdcChannel_t channel, UInt8 measurementConfig, UQ2_6 *pValue)
{
    UQ2_14 v;
    GP_ASSERT_DEV_EXT(Hal_ADC_Initialized);
    GP_ASSERT_DEV_EXT(pValue);

    if (!hal_MeasureADC_16(channel, measurementConfig,&v))
    {
        return false;
    }
    (*pValue) = UQ_PRECISION_DECR8(v);
    return true;
}


Bool hal_MeasureADC_16(hal_AdcChannel_t channel, UInt8 measurementConfig, UQ2_14* pValue)
{
    NOT_USED(measurementConfig);
    GP_ASSERT_DEV_EXT(Hal_ADC_Initialized);
    GP_ASSERT_DEV_EXT(pValue);

    if (!hal_StartContinuousADCMeasurement(channel, HAL_DISABLE_HOLD_MAX, HAL_DISABLE_HOLD_MIN, HAL_ENABLE_3V6))
    {
        return false;
    }
    (*pValue) = hal_GetContinuousADCMeasurement(channel);

    hal_StopContinuousADCMeasurement(channel);
    return true;
}


/* Measure chip temperature. */
Q8_8 halADC_MeasureTemperature(void)
{
    Q8_8 measuredTemp;

    UInt16 temp;
    if (!hal_MeasureADC_16(GP_WB_ENUM_ADC_CHANNEL_TEMP, 0, &temp))
    {
        temp = (UInt16)GP_HAL_ADC_INVALID_TEMPERATURE;
    }
    measuredTemp = (Q8_8)temp;
    GP_LOG_PRINTF("Calibration_Temp 0x%x", 0, measuredTemp);
    return measuredTemp;
}
