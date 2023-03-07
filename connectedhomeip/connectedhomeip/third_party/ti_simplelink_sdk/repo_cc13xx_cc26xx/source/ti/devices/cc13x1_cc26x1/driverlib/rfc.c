/******************************************************************************
*  Filename:       rfc.c
*  Revised:        2018-08-08 11:04:37 +0200 (Wed, 08 Aug 2018)
*  Revision:       52334
*
*  Description:    Driver for the RF Core.
*
*  Copyright (c) 2015 - 2017, Texas Instruments Incorporated
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

#include "rfc.h"
#include "rf_mailbox.h"
#include <string.h>

//*****************************************************************************
//
// Handle support for DriverLib in ROM:
// This section will undo prototype renaming made in the header file
//
//*****************************************************************************
#if !defined(DOXYGEN)
    #undef  RFCCpeIntGetAndClear
    #define RFCCpeIntGetAndClear            NOROM_RFCCpeIntGetAndClear
    #undef  RFCDoorbellSendTo
    #define RFCDoorbellSendTo               NOROM_RFCDoorbellSendTo
    #undef  RFCSynthPowerDown
    #define RFCSynthPowerDown               NOROM_RFCSynthPowerDown
    #undef  RFCCpePatchReset
    #define RFCCpePatchReset                NOROM_RFCCpePatchReset
    #undef  RFCOverrideSearch
    #define RFCOverrideSearch               NOROM_RFCOverrideSearch
    #undef  RFCOverrideUpdate
    #define RFCOverrideUpdate               NOROM_RFCOverrideUpdate
    #undef  RFCHwIntGetAndClear
    #define RFCHwIntGetAndClear             NOROM_RFCHwIntGetAndClear
    #undef  RFCAnaDivTxOverride
    #define RFCAnaDivTxOverride             NOROM_RFCAnaDivTxOverride
    #undef  RFCRfTrimRead
    #define RFCRfTrimRead                   NOROM_RFCRfTrimRead
    #undef  RFCRfTrimSet
    #define RFCRfTrimSet                    NOROM_RFCRfTrimSet
#endif


//*****************************************************************************
//
// Get and clear CPE interrupt flags which match the provided bitmask
//
//*****************************************************************************
uint32_t
RFCCpeIntGetAndClear(uint32_t ui32Mask)
{
    // Read the CPE interrupt flags which match the provided bitmask
    uint32_t ui32Ifg = HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFCPEIFG) & ui32Mask;

    // Clear the interrupt flags
    RFCCpeIntClear(ui32Ifg);

    // Return with the interrupt flags
    return (ui32Ifg);
}


//*****************************************************************************
//
// Send a radio operation to the doorbell and wait for an acknowledgement
//
//*****************************************************************************
uint32_t
RFCDoorbellSendTo(uint32_t pOp)
{
    // Wait until the doorbell becomes available
    while(HWREG(RFC_DBELL_BASE + RFC_DBELL_O_CMDR) != 0);
    RFCAckIntClear();

    // Submit the command to the CM0 through the doorbell
    HWREG(RFC_DBELL_BASE + RFC_DBELL_O_CMDR) = pOp;

    // Wait until the CM0 starts to parse the command
    while(!HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFACKIFG));
    RFCAckIntClear();

    // Return with the content of status register
    return(HWREG(RFC_DBELL_BASE + RFC_DBELL_O_CMDSTA));
}


//*****************************************************************************
//
// Turn off the RF synthesizer. The radio will no longer respond to commands!
//
//*****************************************************************************
void
RFCSynthPowerDown(void)
{
    // Definition of reserved words
    const uint32_t RFC_RESERVED0 = 0x40046054;
    const uint32_t RFC_RESERVED1 = 0x40046060;
    const uint32_t RFC_RESERVED2 = 0x40046058;
    const uint32_t RFC_RESERVED3 = 0x40044100;

    // Disable CPE clock, enable FSCA clock.
    HWREG(RFC_PWR_NONBUF_BASE + RFC_PWR_O_PWMCLKEN) = (HWREG(RFC_PWR_NONBUF_BASE + RFC_PWR_O_PWMCLKEN)
                                                    & ~RFC_PWR_PWMCLKEN_CPE_M) | RFC_PWR_PWMCLKEN_FSCA_M | RFC_PWR_PWMCLKEN_RFE_M;

    HWREG(RFC_RESERVED0) = 3;
    HWREG(RFC_RESERVED1) = 0x1030;
    HWREG(RFC_RESERVED2) = 1;
    HWREG(RFC_RESERVED1) = 0x50;
    HWREG(RFC_RESERVED2) = 1;
    HWREG(RFC_RESERVED1) = 0x650;
    HWREG(RFC_RESERVED2) = 1;
    HWREG(RFC_RESERVED1) = 0x10C0;
    HWREG(RFC_RESERVED2) = 1;
    HWREG(RFC_RESERVED3) = 1;
}


//*****************************************************************************
//
// Reset previously patched CPE RAM to a state where it can be patched again
//
//*****************************************************************************
void
RFCCpePatchReset(void)
{
    // Function is not complete
}


//*****************************************************************************
//
// Function to search an override list for the provided pattern within the search depth.
//
//*****************************************************************************
uint8_t
RFCOverrideSearch(const uint32_t *pOverride, const uint32_t pattern, const uint32_t mask, const uint8_t searchDepth)
{
    // Search from start of the override list, to look for first override entry that matches search pattern
    uint8_t override_index;
    for(override_index = 0; (override_index < searchDepth) && (pOverride[override_index] != END_OVERRIDE); override_index++)
    {
        // Compare the value to the given pattern
        if((pOverride[override_index] & mask) == pattern)
        {
            // Return with the index of override in case of match
            return override_index;
        }
    }

    // Return with an invalid index
    return 0xFF;
}

//*****************************************************************************
//
// Function to calculate the proper override run-time for the High Gain PA.
//
//*****************************************************************************
uint32_t
RFCAnaDivTxOverride(uint8_t loDivider, uint8_t frontEndMode)
{
   uint16_t fsOnly;
   uint16_t txSetting;

   switch (loDivider)
   {
        case 0: fsOnly = 0x0502;
                break;
        case 2:
                fsOnly = 0x0102;
                break;
        case 4:
        case 6:
        case 12:
                fsOnly = 0xF101;
                break;
        case 5:
        case 10:
        case 15:
        case 30:
                fsOnly = 0x1101;
                break;
        default:
                // Error, should not occur!
                fsOnly = 0;
                break;
   }

   if (frontEndMode == 255)
   {
        // Special value meaning 20 dBm PA
        txSetting = (fsOnly | 0x00C0) & ~0x0400;
   }
   else if (frontEndMode == 0)
   {
        // Differential
        txSetting = fsOnly | 0x0030;
   }
   else if (frontEndMode & 1)
   {
        // Single ended on RFP
        txSetting = fsOnly | 0x0010;
   }
   else
   {
        // Single ended on RFN
        txSetting = fsOnly | 0x0020;
   }

   return ((((uint32_t) txSetting) << 16) | RFC_FE_OVERRIDE_ADDRESS);
}

//*****************************************************************************
//
// Update the override list based on values stored in FCFG1
//
//*****************************************************************************
uint8_t
RFCOverrideUpdate(rfc_radioOp_t *pOpSetup, uint32_t *pParams)
{
    // Function is left blank for compatibility reasons.
    return 0;
}


//*****************************************************************************
//
// Get and clear HW interrupt flags
//
//*****************************************************************************
uint32_t
RFCHwIntGetAndClear(uint32_t ui32Mask)
{
    // Read the CPE interrupt flags which match the provided bitmask
    uint32_t ui32Ifg = HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFHWIFG) & ui32Mask;

    // Clear the interupt flags
    RFCHwIntClear(ui32Ifg);

    // Return with the interrupt flags
    return (ui32Ifg);
}

//*****************************************************************************
//
// Read RF trim values from FCFG1
//
//*****************************************************************************
void
RFCRfTrimRead(rfc_radioOp_t *pOpSetup, rfTrim_t *pRfTrim)
{
    // Definition of fields in divider entry
    #define DIVIDER_DIVIDER_BM    0x7F
    #define DIVIDER_INV_DIV2_BM   0x80
    // Definition of invalid divider value
    #define FCFG1_DIVIDER_INVALID 0xFF

    // Local variable
    int32_t divider;

    // Based on the type of setup command, decode the divider value
    switch (pOpSetup->commandNo)
    {
      case CMD_RADIO_SETUP:           // Use the loDivider value
                                      divider = ((rfc_CMD_RADIO_SETUP_t *)pOpSetup)->loDivider;
                                      break;
      case CMD_BLE5_RADIO_SETUP:      // Use the loDivider value
                                      divider = ((rfc_CMD_BLE5_RADIO_SETUP_t *)pOpSetup)->loDivider;
                                      break;
      case CMD_PROP_RADIO_DIV_SETUP:  // Use the loDivider value
                                      divider = ((rfc_CMD_PROP_RADIO_DIV_SETUP_t *)pOpSetup)->loDivider;
                                      break;
      default:                        // Use 2.4 GHz by default
                                      divider = 0;
                                      break;
    }

    // Read trim values from FCFG1
    pRfTrim->configIfAdc    = HWREG(FCFG1_BASE + FCFG1_O_CONFIG_IF_ADC);
    pRfTrim->configRfCommon = HWREG(FCFG1_BASE + FCFG1_O_CONFIG_RF_COMMON);

    if ((divider & DIVIDER_DIVIDER_BM) == 0)
    {
        pRfTrim->configRfFrontend = HWREG(FCFG1_BASE + FCFG1_O_CONFIG_CC26_FE);
        if (divider & DIVIDER_INV_DIV2_BM)
        {
            pRfTrim->configSynth = HWREG(FCFG1_BASE + FCFG1_O_CONFIG_SYNTH_DIV2_CC13_2G4);
        }
        else
        {
            pRfTrim->configSynth = HWREG(FCFG1_BASE + FCFG1_O_CONFIG_SYNTH_DIV2_CC26_2G4);
        }
    }
    else
    {
        pRfTrim->configRfFrontend = HWREG(FCFG1_BASE + FCFG1_O_CONFIG_CC13_FE);
        switch (divider)
        {
          case 2:
                  pRfTrim->configSynth = HWREG(FCFG1_BASE + FCFG1_O_CONFIG_SYNTH_DIV2_CC26_1G);
                  break;

          case 2 | DIVIDER_INV_DIV2_BM:
                  pRfTrim->configSynth = HWREG(FCFG1_BASE + FCFG1_O_CONFIG_SYNTH_DIV2_CC13_1G);
                  break;

          case 4:
                  pRfTrim->configSynth = HWREG(FCFG1_BASE + FCFG1_O_CONFIG_SYNTH_DIV4_CC13);
                  break;

          case 4 | DIVIDER_INV_DIV2_BM:
                  pRfTrim->configSynth = HWREG(FCFG1_BASE + FCFG1_O_CONFIG_SYNTH_DIV4_CC26);
                  break;

          case 5:
          case 5 | DIVIDER_INV_DIV2_BM:
                  pRfTrim->configSynth = HWREG(FCFG1_BASE + FCFG1_O_CONFIG_SYNTH_DIV5);
                  break;

          case 6:
                  pRfTrim->configSynth = HWREG(FCFG1_BASE + FCFG1_O_CONFIG_SYNTH_DIV6_CC13);
                  break;

          case 6 | DIVIDER_INV_DIV2_BM:
                  pRfTrim->configSynth = HWREG(FCFG1_BASE + FCFG1_O_CONFIG_SYNTH_DIV6_CC26);
                  break;

          case 10:
          case 10 | DIVIDER_INV_DIV2_BM:
                  pRfTrim->configSynth = HWREG(FCFG1_BASE + FCFG1_O_CONFIG_SYNTH_DIV10);
                  break;

          case 12:
                  pRfTrim->configSynth = HWREG(FCFG1_BASE + FCFG1_O_CONFIG_SYNTH_DIV12_CC13);
                  break;

          case 12 | DIVIDER_INV_DIV2_BM:
                  pRfTrim->configSynth = HWREG(FCFG1_BASE + FCFG1_O_CONFIG_SYNTH_DIV12_CC26);
                  break;

          case 15:
          case 15 | DIVIDER_INV_DIV2_BM:
                  pRfTrim->configSynth = HWREG(FCFG1_BASE + FCFG1_O_CONFIG_SYNTH_DIV15);
                  break;

          case 30:
          case 30 | DIVIDER_INV_DIV2_BM:
                  pRfTrim->configSynth = HWREG(FCFG1_BASE + FCFG1_O_CONFIG_SYNTH_DIV30);
                  break;

          default:
                  // Other values are invalid
                  divider = FCFG1_DIVIDER_INVALID;
                  break;
        }
    }
    pRfTrim->loDivider = divider;
    pRfTrim->noTrim = 0;

    #undef DIVIDER_DIVIDER_BM
    #undef DIVIDER_INV_DIV2_BM
    #undef FCFG1_DIVIDER_INVALID
}


//*****************************************************************************
//
// Write preloaded RF trim values to the CM0
//
//*****************************************************************************
void
RFCRfTrimSet(rfTrim_t* pRfTrim)
{
    memcpy((void*)&HWREG(0x21000280), (void*)pRfTrim, sizeof(rfTrim_t));
}

//*****************************************************************************
//
// Handle support for DriverLib in ROM:
// This section will undo prototype renaming made in the header file
//
//*****************************************************************************
#if !defined(DOXYGEN)
    #undef  RFCCpeIntGetAndClear
    #define RFCCpeIntGetAndClear            NOROM_RFCCpeIntGetAndClear
    #undef  RFCDoorbellSendTo
    #define RFCDoorbellSendTo               NOROM_RFCDoorbellSendTo
    #undef  RFCSynthPowerDown
    #define RFCSynthPowerDown               NOROM_RFCSynthPowerDown
    #undef  RFCCpePatchReset
    #define RFCCpePatchReset                NOROM_RFCCpePatchReset
    #undef  RFCOverrideSearch
    #define RFCOverrideSearch               NOROM_RFCOverrideSearch
    #undef  RFCOverrideUpdate
    #define RFCOverrideUpdate               NOROM_RFCOverrideUpdate
    #undef  RFCHwIntGetAndClear
    #define RFCHwIntGetAndClear             NOROM_RFCHwIntGetAndClear
    #undef  RFCAnaDivTxOverride
    #define RFCAnaDivTxOverride             NOROM_RFCAnaDivTxOverride
    #undef  RFCRfTrimRead
    #define RFCRfTrimRead                   NOROM_RFCRfTrimRead
    #undef  RFCRfTrimSet
    #define RFCRfTrimSet                    NOROM_RFCRfTrimSet
#endif

// See rfc.h for implementation
