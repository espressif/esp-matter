/******************************************************************************
*  Filename:       osc.c
*  Revised:        $Date$
*  Revision:       $Revision$
*
*  Description:    Driver for setting up the system Oscillators
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

#include <stdlib.h>
#include "../inc/hw_types.h"
#include "../inc/hw_ccfg.h"
#include "../inc/hw_fcfg1.h"
#include "aon_batmon.h"
#include "aon_rtc.h"
#include "osc.h"
#include "sys_ctrl.h"
#include "setup_rom.h"

//*****************************************************************************
//
// Handle support for DriverLib in ROM:
// This section will undo prototype renaming made in the header file
//
//*****************************************************************************
#if !defined(DOXYGEN)
    #undef  OSCClockSourceSet
    #define OSCClockSourceSet               NOROM_OSCClockSourceSet
    #undef  OSCClockSourceGet
    #define OSCClockSourceGet               NOROM_OSCClockSourceGet
    #undef  OSCHF_GetStartupTime
    #define OSCHF_GetStartupTime            NOROM_OSCHF_GetStartupTime
    #undef  OSCHF_TurnOnXosc
    #define OSCHF_TurnOnXosc                NOROM_OSCHF_TurnOnXosc
    #undef  OSCHF_AttemptToSwitchToXosc
    #define OSCHF_AttemptToSwitchToXosc     NOROM_OSCHF_AttemptToSwitchToXosc
    #undef  OSCHF_SwitchToRcOscTurnOffXosc
    #define OSCHF_SwitchToRcOscTurnOffXosc  NOROM_OSCHF_SwitchToRcOscTurnOffXosc
    #undef  OSCHF_DebugGetCrystalAmplitude
    #define OSCHF_DebugGetCrystalAmplitude  NOROM_OSCHF_DebugGetCrystalAmplitude
    #undef  OSCHF_DebugGetExpectedAverageCrystalAmplitude
    #define OSCHF_DebugGetExpectedAverageCrystalAmplitude NOROM_OSCHF_DebugGetExpectedAverageCrystalAmplitude
    #undef  OSCHF_DebugGetCrystalStartupTime
    #define OSCHF_DebugGetCrystalStartupTime NOROM_OSCHF_DebugGetCrystalStartupTime
    #undef  OSC_HPOSCInitializeFrequencyOffsetParameters
    #define OSC_HPOSCInitializeFrequencyOffsetParameters NOROM_OSC_HPOSCInitializeFrequencyOffsetParameters
    #undef  OSC_HPOSC_Debug_InitFreqOffsetParams
    #define OSC_HPOSC_Debug_InitFreqOffsetParams NOROM_OSC_HPOSC_Debug_InitFreqOffsetParams
    #undef  OSC_HPOSCInitializeSingleInsertionFreqOffsParams
    #define OSC_HPOSCInitializeSingleInsertionFreqOffsParams NOROM_OSC_HPOSCInitializeSingleInsertionFreqOffsParams
    #undef  OSC_HPOSCRelativeFrequencyOffsetGet
    #define OSC_HPOSCRelativeFrequencyOffsetGet NOROM_OSC_HPOSCRelativeFrequencyOffsetGet
    #undef  OSC_CapArrayAdjustWorkaround_Boot
    #define OSC_CapArrayAdjustWorkaround_Boot NOROM_OSC_CapArrayAdjustWorkaround_Boot
    #undef  OSC_AdjustXoscHfCapArray
    #define OSC_AdjustXoscHfCapArray        NOROM_OSC_AdjustXoscHfCapArray
    #undef  OSC_HPOSCRelativeFrequencyOffsetToRFCoreFormatConvert
    #define OSC_HPOSCRelativeFrequencyOffsetToRFCoreFormatConvert NOROM_OSC_HPOSCRelativeFrequencyOffsetToRFCoreFormatConvert
    #undef  OSC_HPOSCRtcCompensate
    #define OSC_HPOSCRtcCompensate          NOROM_OSC_HPOSCRtcCompensate
#endif

//*****************************************************************************
//
// OSCHF switch time calculator defines and globals
//
//*****************************************************************************

#define RTC_CV_TO_MS(x) ((    1000 * ( x )) >> 16 )
#define RTC_CV_TO_US(x) (( 1000000 * ( x )) >> 16 )

typedef struct {
   uint32_t    previousStartupTimeInUs ;
   uint32_t    timeXoscOff_CV          ;
   uint32_t    timeXoscOn_CV           ;
   uint32_t    timeXoscStable_CV       ;
   int32_t     tempXoscOff             ;
} OscHfGlobals_t;

static OscHfGlobals_t oscHfGlobals;

//*****************************************************************************
//
//  Configure the oscillator input to the a source clock.
//
//*****************************************************************************
void
OSCClockSourceSet(uint32_t ui32SrcClk, uint32_t ui32Osc)
{
    // Check the arguments.
    ASSERT((ui32SrcClk & OSC_SRC_CLK_LF) ||
           (ui32SrcClk & OSC_SRC_CLK_HF));
    ASSERT((ui32Osc == OSC_RCOSC_HF) ||
           (ui32Osc == OSC_RCOSC_LF) ||
           (ui32Osc == OSC_XOSC_HF) ||
           (ui32Osc == OSC_XOSC_LF));

    // Request the high frequency source clock (using 24 MHz XTAL)
    if(ui32SrcClk & OSC_SRC_CLK_HF)
    {
        // Enable the HF XTAL as HF clock source
        DDI16BitfieldWrite(AUX_DDI0_OSC_BASE, DDI_0_OSC_O_CTL0,
                           DDI_0_OSC_CTL0_SCLK_HF_SRC_SEL_M,
                           DDI_0_OSC_CTL0_SCLK_HF_SRC_SEL_S,
                           ui32Osc);
    }

    // Configure the low frequency source clock.
    if(ui32SrcClk & OSC_SRC_CLK_LF)
    {
        // Change the clock source.
        DDI16BitfieldWrite(AUX_DDI0_OSC_BASE, DDI_0_OSC_O_CTL0,
                           DDI_0_OSC_CTL0_SCLK_LF_SRC_SEL_M,
                           DDI_0_OSC_CTL0_SCLK_LF_SRC_SEL_S,
                           ui32Osc);
    }
}

//*****************************************************************************
//
//  Get the source clock settings
//
//*****************************************************************************
uint32_t
OSCClockSourceGet(uint32_t ui32SrcClk)
{
    uint32_t ui32ClockSource;

    // Check the arguments.
    ASSERT((ui32SrcClk & OSC_SRC_CLK_LF) ||
           (ui32SrcClk & OSC_SRC_CLK_HF));

    // Return the source for the selected clock.
    if(ui32SrcClk == OSC_SRC_CLK_LF)
    {
        ui32ClockSource = DDI16BitfieldRead(AUX_DDI0_OSC_BASE, DDI_0_OSC_O_STAT0,
                                            DDI_0_OSC_STAT0_SCLK_LF_SRC_M,
                                            DDI_0_OSC_STAT0_SCLK_LF_SRC_S);
    }
    else
    {
        ui32ClockSource = DDI16BitfieldRead(AUX_DDI0_OSC_BASE, DDI_0_OSC_O_STAT0,
                                            DDI_0_OSC_STAT0_SCLK_HF_SRC_M,
                                            DDI_0_OSC_STAT0_SCLK_HF_SRC_S);
    }
    return ( ui32ClockSource );
}

//*****************************************************************************
//
// Returns maximum startup time (in microseconds) of XOSC_HF
//
//*****************************************************************************
uint32_t
OSCHF_GetStartupTime( uint32_t timeUntilWakeupInMs )
{
   uint32_t deltaTimeSinceXoscOnInMs   ;
   int32_t  deltaTempSinceXoscOn       ;
   uint32_t newStartupTimeInUs         ;

    // Check CCFG to determine if device is configured for TCXO.
    if( ( HWREG( CCFG_BASE + CCFG_O_MODE_CONF ) & CCFG_MODE_CONF_XOSC_FREQ_M ) == CCFG_MODE_CONF_XOSC_FREQ_TCXO )
    {
        // Device configured for TCXO. Report fixed startup time located in CCFG with
        // coversion from number of 100us to number of us.
        newStartupTimeInUs = (( HWREG( CCFG_BASE + CCFG_O_MODE_CONF_1 ) & CCFG_MODE_CONF_1_TCXO_MAX_START_M ) >>
                                                                          CCFG_MODE_CONF_1_TCXO_MAX_START_S ) * 100;
    }
    else
    {
       deltaTimeSinceXoscOnInMs = RTC_CV_TO_MS( AONRTCCurrentCompareValueGet() - oscHfGlobals.timeXoscOn_CV );
       deltaTempSinceXoscOn     = AONBatMonTemperatureGetDegC() - oscHfGlobals.tempXoscOff;

       if ( deltaTempSinceXoscOn < 0 ) {
          deltaTempSinceXoscOn = -deltaTempSinceXoscOn;
       }

       if (  (( timeUntilWakeupInMs + deltaTimeSinceXoscOnInMs )     > 3000 ) ||
             ( deltaTempSinceXoscOn                                  >    5 ) ||
             ( oscHfGlobals.timeXoscStable_CV < oscHfGlobals.timeXoscOn_CV  ) ||
             ( oscHfGlobals.previousStartupTimeInUs                  ==   0 )    )
       {
          newStartupTimeInUs = 2000;
          if (( HWREG( CCFG_BASE + CCFG_O_SIZE_AND_DIS_FLAGS ) & CCFG_SIZE_AND_DIS_FLAGS_DIS_XOSC_OVR_M ) == 0 ) {
             newStartupTimeInUs = (( HWREG( CCFG_BASE + CCFG_O_MODE_CONF_1 ) &
                CCFG_MODE_CONF_1_XOSC_MAX_START_M ) >>
                CCFG_MODE_CONF_1_XOSC_MAX_START_S ) * 125;
                // Note: CCFG startup time is "in units of 100us" adding 25% margin results in *125
          }
       } else {
          newStartupTimeInUs = RTC_CV_TO_US( oscHfGlobals.timeXoscStable_CV - oscHfGlobals.timeXoscOn_CV );
          newStartupTimeInUs += ( newStartupTimeInUs >> 2 ); // Add 25 percent margin
          if ( newStartupTimeInUs < oscHfGlobals.previousStartupTimeInUs ) {
             newStartupTimeInUs = oscHfGlobals.previousStartupTimeInUs;
          }
       }

       if ( newStartupTimeInUs < 200 ) {
          newStartupTimeInUs = 200;
       }
       if ( newStartupTimeInUs > 4000 ) {
          newStartupTimeInUs = 4000;
       }
   }
   return ( newStartupTimeInUs );
}


//*****************************************************************************
//
// Turns on XOSC_HF (but without switching to XOSC_HF)
//
//*****************************************************************************
void
OSCHF_TurnOnXosc( void )
{
#if ( defined( ROM_OSCClockSourceSet ))
   ROM_OSCClockSourceSet( OSC_SRC_CLK_HF, OSC_XOSC_HF );
#else
   OSCClockSourceSet( OSC_SRC_CLK_HF, OSC_XOSC_HF );
#endif
   oscHfGlobals.timeXoscOn_CV  = AONRTCCurrentCompareValueGet();
}


//*****************************************************************************
//
// Switch to XOSC_HF if XOSC_HF is ready.
//
//*****************************************************************************
bool
OSCHF_AttemptToSwitchToXosc( void )
{
   uint32_t startupTimeInUs;
   uint32_t prevLimmit25InUs;

#if ( defined( ROM_OSCClockSourceGet ))
   if ( ROM_OSCClockSourceGet( OSC_SRC_CLK_HF ) == OSC_XOSC_HF )
#else
   if ( OSCClockSourceGet( OSC_SRC_CLK_HF ) == OSC_XOSC_HF )
#endif
   {
      // Already on XOSC - nothing to do
      return ( 1 );
   }
   if ( OSCHfSourceReady()) {
      OSCHfSourceSwitch();

      // Store startup time, but limit to 25 percent reduction each time.
      oscHfGlobals.timeXoscStable_CV  = AONRTCCurrentCompareValueGet();
      startupTimeInUs   = RTC_CV_TO_US( oscHfGlobals.timeXoscStable_CV - oscHfGlobals.timeXoscOn_CV );
      prevLimmit25InUs  = oscHfGlobals.previousStartupTimeInUs;
      prevLimmit25InUs -= ( prevLimmit25InUs >> 2 ); // 25 percent margin
      oscHfGlobals.previousStartupTimeInUs = startupTimeInUs;
      if ( prevLimmit25InUs > startupTimeInUs ) {
         oscHfGlobals.previousStartupTimeInUs = prevLimmit25InUs;
      }
      return ( 1 );
   }
   return ( 0 );
}


//*****************************************************************************
//
// Switch to RCOSC_HF and turn off XOSC_HF
//
//*****************************************************************************
void
OSCHF_SwitchToRcOscTurnOffXosc( void )
{
#if ( defined( ROM_OSCClockSourceSet ))
   ROM_OSCClockSourceSet( OSC_SRC_CLK_HF, OSC_RCOSC_HF );
#else
   OSCClockSourceSet( OSC_SRC_CLK_HF, OSC_RCOSC_HF );
#endif

   // Do the switching if not already running on RCOSC_HF
#if ( defined( ROM_OSCClockSourceGet ))
   if ( ROM_OSCClockSourceGet( OSC_SRC_CLK_HF ) != OSC_RCOSC_HF )
#else
   if ( OSCClockSourceGet( OSC_SRC_CLK_HF ) != OSC_RCOSC_HF )
#endif
   {
      OSCHfSourceSwitch();
   }

   oscHfGlobals.timeXoscOff_CV  = AONRTCCurrentCompareValueGet();
   oscHfGlobals.tempXoscOff     = AONBatMonTemperatureGetDegC();
}

//*****************************************************************************
//
// Internal functions called from one of the two functions below
//
//*****************************************************************************
static void
InternCapArrayAdjustWithBaseline7001F( int32_t capArrayDelta )
{
    int32_t     capArrayIndex       ;
    uint32_t    row                 ;
    uint32_t    col                 ;

    capArrayIndex = 36 + capArrayDelta ; // index = 36 corresponds to row/col 7/001F (which corresponds to 6.1pF)
    if ( capArrayIndex <  0 ) capArrayIndex =  0 ;
    if ( capArrayIndex > 63 ) capArrayIndex = 63 ;
    row = 0xF >> ( 3 - ( capArrayIndex >> 4 ));
    col = 0xFFFF >> ( 15 - ( capArrayIndex & 0xF ));
    HWREG( AUX_DDI0_OSC_BASE + DDI_0_OSC_O_ANABYPASSVAL1 ) = (( row << DDI_0_OSC_ANABYPASSVAL1_XOSC_HF_ROW_Q12_S    ) |
                                                              ( col << DDI_0_OSC_ANABYPASSVAL1_XOSC_HF_COLUMN_Q12_S )   );
}

static uint32_t
SpecialCapArrayWorkaroundEnabledAndNeeded( void )
{
    if ((( HWREG( CCFG_BASE + CCFG_O_SIZE_AND_DIS_FLAGS ) & CCFG_SIZE_AND_DIS_FLAGS_DIS_LINEAR_CAPARRAY_DELTA_WORKAROUND ) == 0      ) &&
        ((( HWREG( FCFG1_BASE + FCFG1_O_CONFIG_OSC_TOP ) >> FCFG1_CONFIG_OSC_TOP_XOSC_HF_COLUMN_Q12_S ) & 0x000FFFF1 ) == 0x000701F0 )    )
    {
        return ( 1 );
    } else {
        return ( 0 );
    }
}

//*****************************************************************************
//
// Workaround function to be called at boot
// Must be called after SetupAfterColdResetWakeupFromShutDownCfg2()
//
//*****************************************************************************
void
OSC_CapArrayAdjustWorkaround_Boot( void )
{
    uint32_t    ccfg_ModeConfReg    ;
    int32_t     ccfg_CapArrayDelta  ;

    if ( SpecialCapArrayWorkaroundEnabledAndNeeded() ) {
        // Workaround for chip settings like 0x701F0 and 0x701FE which get readjusted with baseline 7001F/6.1pF
        ccfg_CapArrayDelta = 0 ;
        ccfg_ModeConfReg = HWREG( CCFG_BASE + CCFG_O_MODE_CONF );
        if (( ccfg_ModeConfReg & CCFG_MODE_CONF_XOSC_CAP_MOD ) == 0 ) {
            // CCFG CapArrayDelta is enabled get sign-extended delta
            ccfg_CapArrayDelta =
                (((int32_t)( ccfg_ModeConfReg << ( 32 - CCFG_MODE_CONF_XOSC_CAPARRAY_DELTA_W - CCFG_MODE_CONF_XOSC_CAPARRAY_DELTA_S )))
                                              >> ( 32 - CCFG_MODE_CONF_XOSC_CAPARRAY_DELTA_W ));
        }
        InternCapArrayAdjustWithBaseline7001F( ccfg_CapArrayDelta );
    }
}

//*****************************************************************************
//
// Adjust the XOSC HF cap array relative to the factory setting
//
//*****************************************************************************
void
OSC_AdjustXoscHfCapArray( int32_t capArrDelta )
{
    if ( SpecialCapArrayWorkaroundEnabledAndNeeded() ) {
        // Workaround for chip settings like 0x701F0 and 0x701FE which get readjusted with baseline 7001F/6.1pF
        InternCapArrayAdjustWithBaseline7001F( capArrDelta );
    } else
    {
        // Read the MODE_CONF register in CCFG
        uint32_t ccfg_ModeConfReg = HWREG( CCFG_BASE + CCFG_O_MODE_CONF );
        // Clear CAP_MODE and the CAPARRAY_DELATA field
        ccfg_ModeConfReg &= ~( CCFG_MODE_CONF_XOSC_CAPARRAY_DELTA_M | CCFG_MODE_CONF_XOSC_CAP_MOD_M );
        // Insert new delta value
        ccfg_ModeConfReg |= ((((uint32_t)capArrDelta) << CCFG_MODE_CONF_XOSC_CAPARRAY_DELTA_S ) & CCFG_MODE_CONF_XOSC_CAPARRAY_DELTA_M );
        // Update the HW register with the new delta value
        DDI32RegWrite(AUX_DDI0_OSC_BASE, DDI_0_OSC_O_ANABYPASSVAL1, SetupGetTrimForAnabypassValue1( ccfg_ModeConfReg ));
    }
}

//*****************************************************************************
//
// Initialize the frequency offset curve fitting parameters
// These are calculated based on the FCFG1:HPOSC_MEAS_x parameters.
//
//*****************************************************************************
#define  D1OFFSET_25C         (-16)
#define  D2OFFSET_85C         (-23)
#define  D3OFFSET_n40C        (5)

#define  HPOSC_COEFF_BITS     (20) // HPOSC p1,p2,p3 coefficient precision
#define  HPOSC_COEFF0_BITS    (16) // HPOSC p0 coefficient precision
#define  HPOSC_D_BITS         (30) // HPOSC internal parameter
#define  HPOSC_COEFF0_SHIFT   (HPOSC_COEFF_BITS - HPOSC_COEFF0_BITS) // HPOSC internal parameter
#define  HPOSC_SHIFT1         (2*HPOSC_COEFF_BITS - HPOSC_D_BITS) // HPOSC internal parameter
#define  HPOSC_DC_BIAS        (100000) // HPOSC internal parameter

int32_t _hposcCoeffs[4] = {0};  // HPOSC polynomial coefficients

typedef struct
{
    int32_t temp[3];
    int32_t dFreq[3];
} hposc_insertions_t;

typedef struct
{
    uint8_t pu0b[4];
    uint8_t pu1b[4];
    uint8_t pu2b[4];
    int64_t pu0c[4];
    int64_t pu1c[4];
    int64_t pu2c[4];
} hposc_param_t;

static void
multiplyColumns(int64_t *v1, int64_t *v2, int64_t *pBuf, uint8_t shift)
{
    pBuf[0] = (v1[1]*v2[2]) >> shift;
    pBuf[1] = (v1[2]*v2[1]) >> shift;
    pBuf[2] = (v1[0]*v2[2]) >> shift;
    pBuf[3] = (v1[2]*v2[0]) >> shift;
    pBuf[4] = (v1[0]*v2[1]) >> shift;
    pBuf[5] = (v1[1]*v2[0]) >> shift;
}

static int64_t
findDenominator(int64_t *col0, int64_t *col1, int64_t *col2)
{
    int64_t tmp, tmpBuf[6];

    multiplyColumns(col1, col2, tmpBuf, HPOSC_SHIFT1); // Keep HPOSC_D_BITS precision

    tmp = (tmpBuf[0]*col0[0] - tmpBuf[1]*col0[0] - tmpBuf[2]*col0[1] +
           tmpBuf[3]*col0[1] + tmpBuf[4]*col0[2] - tmpBuf[5]*col0[2]) >> HPOSC_COEFF_BITS;

    return tmp;
}

static int64_t
findNumerator(int32_t *pInput, int64_t *pBuf)
{
    int64_t tmp;

    tmp = ((int64_t)pInput[0]*pBuf[0]) - ((int64_t)pInput[0]*pBuf[1]) - ((int64_t)pInput[1]*pBuf[2]) +
          ((int64_t)pInput[1]*pBuf[3]) + ((int64_t)pInput[2]*pBuf[4]) - ((int64_t)pInput[2]*pBuf[5]);

    return tmp;
}

static void
findHposcCoefficients(int32_t *pInput, int64_t *col0, int64_t *col1, int64_t *col2, hposc_param_t *pParam)
{
    int64_t d,c0,c1,c2,cn,tmpBuf[6];
    int32_t inputBuf[3];
    uint8_t i;

    if(col1 == NULL) /* 1 insertion */
    {
        inputBuf[0] = pInput[0] - HPOSC_DC_BIAS;
        c0 = (((int64_t)inputBuf[0] << HPOSC_D_BITS) / col0[0]);
        c1 = 0;
        c2 = 0;
    }
    else /* 3 insertion */
    {
        /* Apply DC bias to input data */
        for(i = 0; i < 3; i++)
        {
            inputBuf[i] = pInput[i] - HPOSC_DC_BIAS;
        }

        /* Solve intermediate parameters, d: HPOSC_D_BITS, c: HPOSC_COEFF_BITS*2 bits */
        d = findDenominator(col0, col1, col2);

        multiplyColumns(col1, col2, tmpBuf, 0);
        cn = findNumerator(inputBuf, tmpBuf);
        c0 = cn / d;

        multiplyColumns(col0, col2, tmpBuf, 0);
        cn = -1*findNumerator(inputBuf, tmpBuf);
        c1 = cn / d;

        multiplyColumns(col0, col1, tmpBuf, 0);
        cn = findNumerator(inputBuf, tmpBuf);
        c2 = cn / d;
    }

    /* Compute TCF polynomial coefficients */
    for(i = 0; i < 4; i++)
    {
        cn = (((pParam->pu0c[i]*c0) >> (pParam->pu0b[i] - HPOSC_COEFF_BITS)) +
              ((pParam->pu1c[i]*c1) >> (pParam->pu1b[i] - HPOSC_COEFF_BITS)) +
              ((pParam->pu2c[i]*c2) >> (pParam->pu2b[i] - HPOSC_COEFF_BITS))) >> HPOSC_SHIFT1;

        if(i<3)
        {
            _hposcCoeffs[3-i] = cn;
        }
        else
        {
            _hposcCoeffs[0] = (cn >> HPOSC_COEFF0_SHIFT) + ((int64_t)HPOSC_DC_BIAS << HPOSC_COEFF0_BITS); // p[0] is combined with the DC bias
        }
    }
}

static void
findHposcPc(int64_t *pCoeff, uint8_t *pBits, int32_t *pTemp, uint8_t nTemp, int64_t *pOutput)
{
    uint8_t i;
    int32_t t1,t2,t3;

    for(i = 0; i < nTemp; i++)
    {
        t1 = pTemp[i];
        t2 = t1*t1;
        t3 = t2*t1;

        pOutput[i] = (((int64_t)pCoeff[0]*t3)>>(pBits[0]-HPOSC_COEFF_BITS)) + (((int64_t)pCoeff[1]*t2)>>(pBits[1]-HPOSC_COEFF_BITS)) +
                     (((int64_t)pCoeff[2]*t1)>>(pBits[2]-HPOSC_COEFF_BITS)) + (((int64_t)pCoeff[3]   )>>(pBits[3]-HPOSC_COEFF_BITS));
    }
}

static void
readTempAndFreq(uint32_t regAddr, int32_t *pTemp, int32_t *pdFreq, int32_t deltaFreq)
{
    uint32_t insertionData = HWREG(regAddr);

    /* temp_stored = Temperature - 27, offset by -27C */
    *pTemp = (((int32_t)( insertionData << ( 32 - FCFG1_HPOSC_MEAS_1_HPOSC_T1_W - FCFG1_HPOSC_MEAS_1_HPOSC_T1_S )))
                                        >> ( 32 - FCFG1_HPOSC_MEAS_1_HPOSC_T1_W ));

    /* dFreq_stored = round( (Freq/12e6 - 1) * 2^22 ), 12MHz is the ideal frequency */
    *pdFreq = (((int32_t)( insertionData << ( 32 - FCFG1_HPOSC_MEAS_1_HPOSC_D1_W - FCFG1_HPOSC_MEAS_1_HPOSC_D1_S )))
                                         >> ( 32 - FCFG1_HPOSC_MEAS_1_HPOSC_D1_W ));
    *pdFreq = *pdFreq + deltaFreq;
}

//*****************************************************************************
// The HPOSC initialization function
// - Must be called before using HPOSC
// - To be used when three temperature frequency offset measurements are available
//*****************************************************************************
void
OSC_HPOSCInitializeFrequencyOffsetParameters( void )
{
    /* Initialize HPOSC internal parameter */
    hposc_insertions_t  hposcMeas;
    int64_t pu0[3],pu1[3],pu2[3] = {0};
    hposc_param_t hposcParm =
    {
        .pu0b = {42, 32, 27, 20},
        .pu0c = {-284,-184,536,-104798},
        .pu1b = {36, 32, 27, 20},
        .pu1c = {-1155,44130,-319090,-3563},
        .pu2b = {36, 32, 27, 20},
        .pu2c = {-3410,261727,-32194,-116627}
    };

    /* Retrieve insertions from FCFG */
    readTempAndFreq(FCFG1_BASE + FCFG1_O_HPOSC_MEAS_1, &hposcMeas.temp[0], &hposcMeas.dFreq[0], D1OFFSET_25C);
    readTempAndFreq(FCFG1_BASE + FCFG1_O_HPOSC_MEAS_2, &hposcMeas.temp[1], &hposcMeas.dFreq[1], D2OFFSET_85C);
    readTempAndFreq(FCFG1_BASE + FCFG1_O_HPOSC_MEAS_3, &hposcMeas.temp[2], &hposcMeas.dFreq[2], D3OFFSET_n40C);

    /* Compute HPOSC polynomial coefficients */
    findHposcPc(hposcParm.pu0c, hposcParm.pu0b, &hposcMeas.temp[0], 3, pu0);
    findHposcPc(hposcParm.pu1c, hposcParm.pu1b, &hposcMeas.temp[0], 3, pu1);
    findHposcPc(hposcParm.pu2c, hposcParm.pu2b, &hposcMeas.temp[0], 3, pu2);
    findHposcCoefficients(&hposcMeas.dFreq[0], pu0, pu1, pu2, &hposcParm);
}

//*****************************************************************************
// Degub function to calculate the HPOSC polynomials for experimental data sets.
//*****************************************************************************
void
OSC_HPOSC_Debug_InitFreqOffsetParams( HposcDebugData_t * pDebugData )
{
    /* Initialize HPOSC internal parameter */
    hposc_insertions_t  hposcMeas;
    int64_t pu0[3],pu1[3],pu2[3] = {0};
    hposc_param_t hposcParm =
    {
        .pu0b = {42, 32, 27, 20},
        .pu0c = {-284,-184,536,-104798},
        .pu1b = {36, 32, 27, 20},
        .pu1c = {-1155,44130,-319090,-3563},
        .pu2b = {36, 32, 27, 20},
        .pu2c = {-3410,261727,-32194,-116627}
    };

    /* Retrieve insertions from FCFG */
    readTempAndFreq((uint32_t)&pDebugData->meas_1, &hposcMeas.temp[0], &hposcMeas.dFreq[0], pDebugData->offsetD1);
    readTempAndFreq((uint32_t)&pDebugData->meas_2, &hposcMeas.temp[1], &hposcMeas.dFreq[1], pDebugData->offsetD2);
    readTempAndFreq((uint32_t)&pDebugData->meas_3, &hposcMeas.temp[2], &hposcMeas.dFreq[2], pDebugData->offsetD3);

    /* Compute HPOSC polynomial coefficients */
    findHposcPc(hposcParm.pu0c, hposcParm.pu0b, &hposcMeas.temp[0], 3, pu0);
    findHposcPc(hposcParm.pu1c, hposcParm.pu1b, &hposcMeas.temp[0], 3, pu1);
    findHposcPc(hposcParm.pu2c, hposcParm.pu2b, &hposcMeas.temp[0], 3, pu2);
    findHposcCoefficients(&hposcMeas.dFreq[0], pu0, pu1, pu2, &hposcParm);
}

//*****************************************************************************
// Special HPOSC initialization function
// - Used when a single temperature offset measurement is available
// - To get a better crystal performance (SW TCXO)
//*****************************************************************************
void
OSC_HPOSCInitializeSingleInsertionFreqOffsParams( uint32_t measFieldAddress )
{
    /* Initialize HPOSC internal parameter */
    hposc_insertions_t  hposcMeas;
    int64_t pu0;
    hposc_param_t hposcParm =
    {
        /* Coefficients for SW-TCXO */
        .pu0b = {44, 44, 27, 20},
        .pu0c = {8183, -2546, -210, -104866}
    };

    /* Retrieve insertions from FCFG */
    readTempAndFreq( measFieldAddress, &hposcMeas.temp[0], &hposcMeas.dFreq[0], 0);

    /* Compute HPOSC polynomial coefficients */
    findHposcPc(hposcParm.pu0c, hposcParm.pu0b, &hposcMeas.temp[0], 1, &pu0);
    findHposcCoefficients(&hposcMeas.dFreq[0], &pu0, NULL, NULL, &hposcParm);
}

//*****************************************************************************
//
// Calculate the temperature dependent relative frequency offset of HPOSC
//
//*****************************************************************************
int32_t
OSC_HPOSCRelativeFrequencyOffsetGet( int32_t tempDegC )
{
    // Estimate HPOSC frequency offset, using temperature and curve fitting parameters

    // Now we can find the HPOSC freq offset, given as a signed variable d, expressed by:
    //
    //    F_HPOSC = F_nom * (1 + d/(2^22))    , where: F_HPOSC = HPOSC frequency
    //                                                 F_nom = nominal clock source frequency (e.g. 48.000 MHz)
    //                                                 d = describes relative freq offset

    // We can estimate the d variable, using temperature compensation parameters:
    //
    //    d = P[3]*(t - T0)^3 + P[2]*(t - T0)^2 + P[1]*(t - T0) + P[0], where: P0,P1,P2,P3 are curve fitting parameters
    //                                                                  t = current temperature (from temp sensor) in deg C
    //                                                                  T0 = 27 deg C (fixed temperature constant)

    int32_t d,t1,t2,t3;

    t1 = tempDegC - 27;
    t2 = t1 * t1;
    t3 = t2 * t1;

    d =  ((((int64_t)_hposcCoeffs[3]*t3 + (int64_t)_hposcCoeffs[2]*t2 + (int64_t)_hposcCoeffs[1]*t1) >> HPOSC_COEFF0_SHIFT) +
            (int64_t)_hposcCoeffs[0]) >> HPOSC_COEFF0_BITS;

    return ( d );
}

//*****************************************************************************
//
// Converts the relative frequency offset of HPOSC to the RF Core parameter format.
//
//*****************************************************************************
int16_t
OSC_HPOSCRelativeFrequencyOffsetToRFCoreFormatConvert( int32_t HPOSC_RelFreqOffset )
{
   // The input argument, hereby referred to simply as "d", describes the frequency offset
   // of the HPOSC relative to the nominal frequency in this way:
   //
   //    F_HPOSC = F_nom * (1 + d/(2^22))
   //
   // But for use by the radio, to compensate the frequency error, we need to find the
   // frequency offset "rfcFreqOffset" defined in the following format:
   //
   //    F_nom = F_HPOSC * (1 + rfCoreFreqOffset/(2^22))
   //
   // To derive "rfCoreFreqOffset" from "d" we combine the two above equations and get:
   //
   //    (1 + rfCoreFreqOffset/(2^22)) = (1 + d/(2^22))^-1
   //
   // Which can be rewritten into:
   //
   //    rfCoreFreqOffset = -d*(2^22) / ((2^22) + d)
   //
   //               = -d * [ 1 / (1 + d/(2^22)) ]
   //
   // To avoid doing a 64-bit division due to the (1 + d/(2^22))^-1 expression,
   // we can use Taylor series (Maclaurin series) to approximate it:
   //
   //       1 / (1 - x) ~= 1 + x + x^2 + x^3 + x^4 + ... etc      (Maclaurin series)
   //
   // In our case, we have x = - d/(2^22), and we only include up to the first
   // order term of the series, as the second order term ((d^2)/(2^44)) is very small:
   //
   //       freqError ~= -d + d^2/(2^22)   (+ small approximation error)
   //
   // The approximation error is negligible for our use.

   int32_t rfCoreFreqOffset = -HPOSC_RelFreqOffset + (( HPOSC_RelFreqOffset * HPOSC_RelFreqOffset ) >> 22 );

   return ( rfCoreFreqOffset );
}

//*****************************************************************************
//
// Compensate the RTC increment based on the relative frequency offset of HPOSC
//
//*****************************************************************************
void
OSC_HPOSCRtcCompensate( int32_t relFreqOffset )
{
    uint32_t rtcSubSecInc;
    uint32_t lfClkFrequency;
    uint32_t hfFreq;
    int64_t  calcFactor;

    // Calculate SCLK_HF frequency, defined as:
    // hfFreq = 48000000 * (1 + relFreqOffset/(2^22))
    if( relFreqOffset >= 0 )
    {
        calcFactor = ( ( 48000000 * (int64_t)relFreqOffset ) + 0x200000 ) / 0x400000;
    }
    else
    {
        calcFactor = ( ( 48000000 * (int64_t)relFreqOffset ) - 0x200000 ) / 0x400000;
    }
    hfFreq = 48000000 + calcFactor;

    // Calculate SCLK_LF frequency, defined as SCLK_LF_FREQ = SCLK_HF_FREQ / 1536
    lfClkFrequency = ( hfFreq + 768 ) / 1536;

    // Calculate SUBSECINC, defined as: SUBSECINC = 2^38 / SCLK_LF_FREQ
    rtcSubSecInc = 0x4000000000 / lfClkFrequency;

    /* Update SUBSECINC value */
    SetupSetAonRtcSubSecInc(rtcSubSecInc);
}

//*****************************************************************************
//
// Get crystal amplitude (assuming crystal is running).
//
//*****************************************************************************
uint32_t
OSCHF_DebugGetCrystalAmplitude( void )
{
   uint32_t oscCfgRegCopy  ;
   uint32_t startTime      ;
   uint32_t deltaTime      ;
   uint32_t ampValue       ;

   // The specified method is as follows:
   // 1. Set minimum interval between oscillator amplitude calibrations.
   //    (Done by setting PER_M=0 and PER_E=1)
   // 2. Wait approximately 4 milliseconds in order to measure over a
   //    moderately large number of calibrations.
   // 3. Read out the crystal amplitude value from the peek detector.
   // 4. Restore original oscillator amplitude calibrations interval.
   // 5. Return crystal amplitude value converted to millivolt.
   oscCfgRegCopy = HWREG( AON_PMCTL_BASE + AON_PMCTL_O_OSCCFG );
   HWREG( AON_PMCTL_BASE + AON_PMCTL_O_OSCCFG ) = ( 1 << AON_PMCTL_OSCCFG_PER_E_S );
   startTime = AONRTCCurrentCompareValueGet();
   do {
      deltaTime = AONRTCCurrentCompareValueGet() - startTime;
   } while ( deltaTime < ((uint32_t)( 0.004 * FACTOR_SEC_TO_COMP_VAL_FORMAT )));
   ampValue = ( HWREG( AUX_DDI0_OSC_BASE + DDI_0_OSC_O_STAT1 ) &
      DDI_0_OSC_STAT1_HPM_UPDATE_AMP_M ) >>
      DDI_0_OSC_STAT1_HPM_UPDATE_AMP_S ;
   HWREG( AON_PMCTL_BASE + AON_PMCTL_O_OSCCFG ) = oscCfgRegCopy;

   return ( ampValue * 15 );
}

//*****************************************************************************
//
// Get the expected average crystal amplitude.
//
//*****************************************************************************
uint32_t
OSCHF_DebugGetExpectedAverageCrystalAmplitude( void )
{
   uint32_t ampCompTh1    ;
   uint32_t highThreshold ;
   uint32_t lowThreshold  ;

   ampCompTh1 = HWREG( AUX_DDI0_OSC_BASE + DDI_0_OSC_O_AMPCOMPTH1 );
   highThreshold = ( ampCompTh1 & DDI_0_OSC_AMPCOMPTH1_HPMRAMP3_HTH_M ) >>
                                  DDI_0_OSC_AMPCOMPTH1_HPMRAMP3_HTH_S ;
   lowThreshold  = ( ampCompTh1 & DDI_0_OSC_AMPCOMPTH1_HPMRAMP3_LTH_M ) >>
                                  DDI_0_OSC_AMPCOMPTH1_HPMRAMP3_LTH_S ;

   return ((( highThreshold + lowThreshold ) * 15 ) >> 1 );
}

//*****************************************************************************
//
// Measure the crystal startup time - in number of LF clock edges
//
//*****************************************************************************
uint32_t OSCHF_DebugGetCrystalStartupTime( void )
{
   uint32_t lfEdgesFound = 0 ;

   // Start operation in sync with the LF clock
   HWREG( AON_RTC_BASE + AON_RTC_O_SYNCLF );

   OSCHF_TurnOnXosc();
   while ( ! OSCHF_AttemptToSwitchToXosc() ) {
      HWREG( AON_RTC_BASE + AON_RTC_O_SYNCLF );
      lfEdgesFound ++ ;
   }
   OSCHF_SwitchToRcOscTurnOffXosc();

   return ( lfEdgesFound );
}
