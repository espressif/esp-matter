/******************************************************************************
*  Filename:       aux_dac.c
*  Revised:        2021-01-25 12:18:33 +0200 (mon., 25 jan 2021)
*  Revision:       58172
*
*  Description:    Driver for the AUX Digital-to-Analog Converter interface.
*
*  Copyright (c) 2015 - 2021, Texas Instruments Incorporated
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*  3) Neither the name of the ORGANIZATION nor the names of its contributors may
*     be used to endorse or promote products derived from this software without
*     specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
*  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/
#include "aux_dac.h"


//*****************************************************************************
//
// Sets the DAC's voltage reference
//
//*****************************************************************************
void
AUXDACSetVref(uint8_t refSource)
{
    HWREGB(AUX_ADI4_BASE + ADI_4_AUX_O_MUX2) = refSource;

    if(refSource == AUXDAC_VREF_SEL_ADCREF)
    {
        // Enable the ADC reference module.
        HWREGB(AUX_ADI4_BASE + ADI_4_AUX_O_ADCREF0) |= ADI_4_AUX_ADCREF0_REF_ON_IDLE | ADI_4_AUX_ADCREF0_EN;
    }
}

//*****************************************************************************
//
// Enables precharge
//
//*****************************************************************************
void
AUXDACEnablePreCharge(void)
{
    HWREG(AUX_ANAIF_BASE + AUX_ANAIF_O_DACCTL) |= AUX_ANAIF_DACCTL_DAC_PRECHARGE_EN;
}

//*****************************************************************************
//
// Disables precharge
//
//*****************************************************************************
void
AUXDACDisablePreCharge(void)
{
    HWREG(AUX_ANAIF_BASE + AUX_ANAIF_O_DACCTL) &= ~(0x1 << AUX_ANAIF_DACCTL_DAC_PRECHARGE_EN_S);
}

//*****************************************************************************
//
// Returns maximum DAC voltage in uV
//
//*****************************************************************************
uint32_t
AUXDACCalcMax(void)
{
    uint32_t prechargeStatus;
    uint32_t refSource;
    uint32_t dacMax;

    // Depending on the selected DAC voltage reference, obtain calibration value C2 in millivolts.
    refSource = ((HWREG(AUX_ADI4_BASE + ADI_4_AUX_O_MUX2) & ADI_4_AUX_MUX2_DAC_VREF_SEL_M) >> ADI_4_AUX_MUX2_DAC_VREF_SEL_S);

    switch (refSource)
    {
        // Nothing connected. Input is floating.
        case AUXDAC_VREF_SEL_NC:

            dacMax = 0;
            break;

        // Core Voltage Decoupling pin as the DAC's voltage reference source.
        case AUXDAC_VREF_SEL_DCOUPL:

            prechargeStatus = ((HWREG( AUX_ANAIF_BASE + AUX_ANAIF_O_DACCTL) & AUX_ANAIF_DACCTL_DAC_PRECHARGE_EN_M) >> AUX_ANAIF_DACCTL_DAC_PRECHARGE_EN_S);
            dacMax = (prechargeStatus == 0) ? ((HWREG(FCFG1_BASE + FCFG1_O_DAC_CAL0) & FCFG1_DAC_CAL0_SOC_DAC_VOUT_CAL_DECOUPLE_C2_M) >> FCFG1_DAC_CAL0_SOC_DAC_VOUT_CAL_DECOUPLE_C2_S) \
                                            : ((HWREG(FCFG1_BASE + FCFG1_O_DAC_CAL1) & FCFG1_DAC_CAL1_SOC_DAC_VOUT_CAL_PRECH_C2_M) >> FCFG1_DAC_CAL1_SOC_DAC_VOUT_CAL_PRECH_C2_S);
            break;

        // ADC reference voltage as the DAC's voltage reference source.
        case AUXDAC_VREF_SEL_ADCREF:

            dacMax = ((HWREG(FCFG1_BASE + FCFG1_O_DAC_CAL2) & FCFG1_DAC_CAL2_SOC_DAC_VOUT_CAL_ADCREF_C2_M) >> FCFG1_DAC_CAL2_SOC_DAC_VOUT_CAL_ADCREF_C2_S);
            break;

        // Main supply voltage VDDS as the DAC's voltage reference source.
        case AUXDAC_VREF_SEL_VDDS:

            // Note: The calibration value is measured at a VDDS of 3.0V. If VDDS is selected as reference,
            // measure VDDS and scale the calibration value.
            dacMax = ((HWREG(FCFG1_BASE + FCFG1_O_DAC_CAL3) & FCFG1_DAC_CAL3_SOC_DAC_VOUT_CAL_VDDS_C2_M) >> FCFG1_DAC_CAL3_SOC_DAC_VOUT_CAL_VDDS_C2_S);
            break;

        // Nothing connected. Input is floating.
        default:

            dacMax = 0;
    }

    return dacMax;
}

//*****************************************************************************
//
// Returns minimum DAC voltage in uV
//
//*****************************************************************************
uint32_t
AUXDACCalcMin(void)
{
    uint32_t refSource;
    uint32_t prechargeStatus;
    uint32_t dacMin;

    // Depending on the selected DAC voltage reference, obtain calibration value C1 in millivolts.
    refSource = ((HWREG(AUX_ADI4_BASE + ADI_4_AUX_O_MUX2) & ADI_4_AUX_MUX2_DAC_VREF_SEL_M) >> ADI_4_AUX_MUX2_DAC_VREF_SEL_S);

    switch (refSource)
    {
        // Nothing connected. Input is floating.
        case AUXDAC_VREF_SEL_NC:
            dacMin = 0;
            break;

        // Core Voltage Decoupling pin as the DAC's voltage reference source.
        case AUXDAC_VREF_SEL_DCOUPL:

            prechargeStatus = ((HWREG(AUX_ANAIF_BASE + AUX_ANAIF_O_DACCTL) & AUX_ANAIF_DACCTL_DAC_PRECHARGE_EN_M) >> AUX_ANAIF_DACCTL_DAC_PRECHARGE_EN_S);
            dacMin = (prechargeStatus == 0) ? ((HWREG(FCFG1_BASE + FCFG1_O_DAC_CAL0) & FCFG1_DAC_CAL0_SOC_DAC_VOUT_CAL_DECOUPLE_C1_M) >> FCFG1_DAC_CAL0_SOC_DAC_VOUT_CAL_DECOUPLE_C1_S) \
                                            : ((HWREG(FCFG1_BASE + FCFG1_O_DAC_CAL1) & FCFG1_DAC_CAL1_SOC_DAC_VOUT_CAL_PRECH_C1_M) >> FCFG1_DAC_CAL1_SOC_DAC_VOUT_CAL_PRECH_C1_S);
            break;

        // ADC reference voltage as the DAC's voltage reference source.
        case AUXDAC_VREF_SEL_ADCREF:
            dacMin = ((HWREG( FCFG1_BASE + FCFG1_O_DAC_CAL2) & FCFG1_DAC_CAL2_SOC_DAC_VOUT_CAL_ADCREF_C1_M) >> FCFG1_DAC_CAL2_SOC_DAC_VOUT_CAL_ADCREF_C1_S);
            break;

        // Main supply voltage VDDS as the DAC's voltage reference source.
        case AUXDAC_VREF_SEL_VDDS:
            dacMin = ((HWREG( FCFG1_BASE + FCFG1_O_DAC_CAL3) & FCFG1_DAC_CAL3_SOC_DAC_VOUT_CAL_VDDS_C1_M) >> FCFG1_DAC_CAL3_SOC_DAC_VOUT_CAL_VDDS_C1_S);
            break;

        // Nothing connected. Input is floating.
        default:

            dacMin = 0;
    }

    return dacMin;
}

//*****************************************************************************
//
// Returns 8-bit DAC code from requested voltage in (uV)
//
//*****************************************************************************
uint32_t
AUXDACCalcCode(uint32_t uVoltOut, uint32_t uVoltOutMin, uint32_t uVoltOutMax)
{
    uint32_t dacCode = 0;

    if(uVoltOut != 0)
    {
        // Rounding (by adding 0.5 before truncating)
        dacCode = (((254 * (uVoltOut - uVoltOutMin) * 2) / (uVoltOutMax - uVoltOutMin) + 1) / 2) + 1;
    }
    return (uint32_t)dacCode;
}

//*****************************************************************************
//
// Sets the DAC sample clock
//
//*****************************************************************************
void
AUXDACSetSampleClock(uint8_t dacClkDiv)
{

    // Set the peripheral operational rate to the AUX bus rate of 24 MHz.
    HWREGB(AUX_SYSIF_BASE + AUX_SYSIF_O_PEROPRATE) |= AUX_SYSIF_PEROPRATE_ANAIF_DAC_OP_RATE_BUS_RATE;

    // Set sample clock frequency considering a clock base frequency of 24 MHz and the given clock divider.
    HWREGB(AUX_ANAIF_BASE + AUX_ANAIF_O_DACSMPLCFG0) = dacClkDiv;
}

//*****************************************************************************
//
// Sets the DAC's code
//
//*****************************************************************************
void
AUXDACSetCode(uint8_t dacCode)
{
    // Update the DAC's output value.
    HWREGB(AUX_ANAIF_BASE + AUX_ANAIF_O_DACVALUE) = dacCode;
}

//*****************************************************************************
//
// Enables the DAC
//
//*****************************************************************************
void
AUXDACEnable(uint8_t dacPinId)
{
    // Set the operational mode to active
    HWREG(AUX_SYSIF_BASE + AUX_SYSIF_O_OPMODEREQ) = AUX_SYSIF_OPMODEREQ_REQ_A;

    // Disable the low power bias control
    HWREGB(AUX_ANAIF_BASE + AUX_ANAIF_O_LPMBIASCTL) = 0x0;

    // Enable the DAC sample clock
    HWREGB(AUX_ANAIF_BASE + AUX_ANAIF_O_DACSMPLCTL) = AUX_ANAIF_DACSMPLCTL_EN;

    // Enable DAC and buffer and set COMPA_IN
    HWREGB(AUX_ANAIF_BASE + AUX_ANAIF_O_DACCTL) |= AUX_ANAIF_DACCTL_DAC_EN | AUX_ANAIF_DACCTL_DAC_BUFFER_EN | AUX_ANAIF_DACCTL_DAC_VOUT_SEL_COMPA_IN;

    // Select COMPA output (0 will disconnect from DIOs)
    HWREGB(AUX_ADI4_BASE + ADI_4_AUX_O_MUX1) = dacPinId;
}

//*****************************************************************************
//
// Disables the DAC
//
//*****************************************************************************
void
AUXDACDisable(void)
{
    // Disable the DAC sample clock
    HWREGB(AUX_ANAIF_BASE + AUX_ANAIF_O_DACSMPLCTL) = 0x0;

    // Disable DAC, the DAC's buffer, and disconnect internally.
    HWREGB(AUX_ANAIF_BASE + AUX_ANAIF_O_DACCTL) &= ~(0x1 << AUX_ANAIF_DACCTL_DAC_EN_S) &      \
                                                   ~(0x1 << AUX_ANAIF_DACCTL_DAC_BUFFER_EN_S) &   \
                                                   ~(AUX_ANAIF_DACCTL_DAC_VOUT_SEL_M << AUX_ANAIF_DACCTL_DAC_VOUT_SEL_S);

    // Disconnect COMPA_IN from DIOs
    HWREGB(AUX_ADI4_BASE + ADI_4_AUX_O_MUX1) = 0x0;

    // Set operational mode to Power-Down
    HWREG(AUX_SYSIF_BASE + AUX_SYSIF_O_OPMODEREQ) = AUX_SYSIF_OPMODEREQ_REQ_PDA;

    // Set the peripheral operational rate to the SCE rate
    HWREGB(AUX_SYSIF_BASE + AUX_SYSIF_O_PEROPRATE) = AUX_SYSIF_PEROPRATE_ANAIF_DAC_OP_RATE_SCE_RATE;
}
