/******************************************************************************
*  Filename:       setup.c
*  Revised:        $Date$
*  Revision:       $Revision$
*
*  Description:    Setup file for CC13xx/CC26xx devices.
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


// Hardware headers
#include "../inc/hw_types.h"
#include "../inc/hw_memmap.h"
#include "../inc/hw_adi.h"
#include "../inc/hw_adi_2_refsys.h"
#include "../inc/hw_adi_3_refsys.h"
#include "../inc/hw_adi_4_aux.h"
// Temporarily adding these defines as they are missing in hw_adi_4_aux.h
#define ADI_4_AUX_O_LPMBIAS                                         0x0000000E
#define ADI_4_AUX_LPMBIAS_LPM_TRIM_IOUT_M                           0x0000003F
#define ADI_4_AUX_LPMBIAS_LPM_TRIM_IOUT_S                                    0
#define ADI_4_AUX_COMP_LPM_BIAS_WIDTH_TRIM_M                        0x00000038
#define ADI_4_AUX_COMP_LPM_BIAS_WIDTH_TRIM_S                                 3
#include "../inc/hw_aon_ioc.h"
#include "../inc/hw_aon_pmctl.h"
#include "../inc/hw_aon_rtc.h"
#include "../inc/hw_ddi_0_osc.h"
#include "../inc/hw_ddi.h"
#include "../inc/hw_ccfg.h"
#include "../inc/hw_fcfg1.h"
#include "../inc/hw_flash.h"
#include "../inc/hw_prcm.h"
#include "../inc/hw_vims.h"
#include "../inc/hw_aux_sce.h"
// Driverlib headers
#include "aon_rtc.h"
#include "interrupt.h"
#include "aux_sysif.h"
#include "chipinfo.h"
#include "osc.h"
#include "setup.h"
#include "setup_rom.h"

//*****************************************************************************
//
// Handle support for DriverLib in ROM:
// This section will undo prototype renaming made in the header file
//
//*****************************************************************************
#if !defined(DOXYGEN)
    #undef  SetupTrimDevice
    #define SetupTrimDevice                 NOROM_SetupTrimDevice
#endif



//*****************************************************************************
//
// Defined CPU delay macro with microseconds as input
// Quick check shows: (To be further investigated)
// At 48 MHz RCOSC and VIMS.CONTROL.PREFETCH = 0, there is 5 cycles
// At 48 MHz RCOSC and VIMS.CONTROL.PREFETCH = 1, there is 4 cycles
// At 24 MHz RCOSC and VIMS.CONTROL.PREFETCH = 0, there is 3 cycles
//
//*****************************************************************************
#define CPU_DELAY_MICRO_SECONDS( x ) \
   CPUdelay(((uint32_t)((( x ) * 48.0 ) / 5.0 )) - 1 )


//*****************************************************************************
//
// Function declarations
//
//*****************************************************************************
static void     TrimAfterColdReset( void );
static void     TrimAfterColdResetWakeupFromShutDown( uint32_t ui32Fcfg1Revision );
static void     TrimAfterColdResetWakeupFromShutDownWakeupFromPowerDown( void );

//*****************************************************************************
//
// Perform the necessary trim of the device which is not done in boot code
//
// This function should only execute coming from ROM boot. The current
// implementation does not take soft reset into account. However, it does no
// damage to execute it again. It only consumes time.
//
//*****************************************************************************
void
SetupTrimDevice(void)
{
    uint32_t ui32Fcfg1Revision;
    uint32_t ui32AonSysResetctl;

    // Get layout revision of the factory configuration area
    // (Handle undefined revision as revision = 0)
    ui32Fcfg1Revision = HWREG(FCFG1_BASE + FCFG1_O_FCFG1_REVISION);
    if ( ui32Fcfg1Revision == 0xFFFFFFFF ) {
        ui32Fcfg1Revision = 0;
    }

    // This driverlib version and setup file is for the CC13x4, CC26x4 chips.
    // Halt if violated
    ThisLibraryIsFor_CC13x4_CC26x4_HaltIfViolated();

    // Select correct CACHE mode and set correct CACHE configuration
#if ( CCFG_BASE == CCFG_BASE_DEFAULT )
    SetupSetCacheModeAccordingToCcfgSetting();
#else
    NOROM_SetupSetCacheModeAccordingToCcfgSetting();
#endif

    // 1. Check for powerdown
    // 2. Check for shutdown
    // 3. Assume cold reset if none of the above.
    //
    // It is always assumed that the application will freeze the latches in
    // AON_IOC when going to powerdown in order to retain the values on the IOs.
    //
    // NB. If this bit is not cleared before proceeding to powerdown, the IOs
    //     will all default to the reset configuration when restarting.
    if( ! ( HWREG( AON_IOC_BASE + AON_IOC_O_IOCLATCH ) & AON_IOC_IOCLATCH_EN ))
    {
        // NB. This should be calling a ROM implementation of required trim and
        // compensation
        // e.g. TrimAfterColdResetWakeupFromShutDownWakeupFromPowerDown()
        TrimAfterColdResetWakeupFromShutDownWakeupFromPowerDown();
    }
    // Check for shutdown
    //
    // When device is going to shutdown the hardware will automatically clear
    // the SLEEPDIS bit in the SLEEP register in the AON_PMCTL module.
    // It is left for the application to assert this bit when waking back up,
    // but not before the desired IO configuration has been re-established.
    else if( ! ( HWREG( AON_PMCTL_BASE + AON_PMCTL_O_SLEEPCTL ) & AON_PMCTL_SLEEPCTL_IO_PAD_SLEEP_DIS ))
    {
        // NB. This should be calling a ROM implementation of required trim and
        // compensation
        // e.g. TrimAfterColdResetWakeupFromShutDown()    -->
        //      TrimAfterColdResetWakeupFromShutDownWakeupFromPowerDown();
        TrimAfterColdResetWakeupFromShutDown(ui32Fcfg1Revision);
        TrimAfterColdResetWakeupFromShutDownWakeupFromPowerDown();
    }
    else
    {
        // Consider adding a check for soft reset to allow debugging to skip
        // this section!!!
        //
        // NB. This should be calling a ROM implementation of required trim and
        // compensation
        // e.g. TrimAfterColdReset()   -->
        //      TrimAfterColdResetWakeupFromShutDown()    -->
        //      TrimAfterColdResetWakeupFromShutDownWakeupFromPowerDown()
        TrimAfterColdReset();
        TrimAfterColdResetWakeupFromShutDown(ui32Fcfg1Revision);
        TrimAfterColdResetWakeupFromShutDownWakeupFromPowerDown();

    }

    // Set VIMS power domain control.
    // PDCTL1VIMS = 0 ==> VIMS power domain is only powered when CPU power domain is powered
    HWREG( PRCM_BASE + PRCM_O_PDCTL1VIMS ) = 0;

    // And finally at the end of the flash boot process:
    // SET BOOT_DET bits in AON_PMCTL to 3 if already found to be 1
    // Note: The BOOT_DET_x_CLR/SET bits must be manually cleared
    if ((( HWREG( AON_PMCTL_BASE + AON_PMCTL_O_RESETCTL ) &
        ( AON_PMCTL_RESETCTL_BOOT_DET_1_M | AON_PMCTL_RESETCTL_BOOT_DET_0_M )) >>
        AON_PMCTL_RESETCTL_BOOT_DET_0_S ) == 1 )
    {
        ui32AonSysResetctl = ( HWREG( AON_PMCTL_BASE + AON_PMCTL_O_RESETCTL ) &
        ~( AON_PMCTL_RESETCTL_BOOT_DET_1_CLR_M | AON_PMCTL_RESETCTL_BOOT_DET_0_CLR_M |
           AON_PMCTL_RESETCTL_BOOT_DET_1_SET_M | AON_PMCTL_RESETCTL_BOOT_DET_0_SET_M | AON_PMCTL_RESETCTL_MCU_WARM_RESET_M ));
        HWREG( AON_PMCTL_BASE + AON_PMCTL_O_RESETCTL ) = ui32AonSysResetctl | AON_PMCTL_RESETCTL_BOOT_DET_1_SET_M;
        HWREG( AON_PMCTL_BASE + AON_PMCTL_O_RESETCTL ) = ui32AonSysResetctl;
    }

    // Reset the RTC
    AONRTCReset();
    // Configure the combined event
    IntPendClear(INT_AON_RTC_COMB);
    AONRTCCombinedEventConfig(AON_RTC_CH0 | AON_RTC_CH1 | AON_RTC_CH2);
    // Start the RTC
    AONRTCEnable();

    // Make sure there are no ongoing VIMS mode change when leaving SetupTrimDevice()
    // (There should typically be no wait time here, but need to be sure)
    while ( HWREG( VIMS_BASE + VIMS_O_STAT ) & VIMS_STAT_MODE_CHANGING ) {
        // Do nothing - wait for an eventual ongoing mode change to complete.
    }

    // Configure the NONSECWRn registers to allow manipulation of
    // ADI_3_REFSYS_DCDCCTL5_IPEAK and ADI_3_REFSYS_DCDCCTL5_DITHER from the
    // non-secure side. This is required by the radio for proper operation.
    // The ADI_3_REFSYS_O_DCDCCTL5 register byte-offset is divided by two
    // since the the ADDR field is encoded as a half-word index

    // Dither disabled, IPEAK = 0
    HWREG( ADI3_BASE + ADI_O_NONSECWR0) = (( 0x00 << ADI_NONSECWR0_DATA_S) & ADI_NONSECWR0_DATA_M ) |
                                          ((( ( ADI_3_REFSYS_O_DCDCCTL5) ) << ADI_NONSECWR0_ADDR_S ) & ADI_NONSECWR0_ADDR_M) |
                                          (( 0x0F << ADI_NONSECWR0_WR_MASK_S) & ADI_NONSECWR0_WR_MASK_M);

    // Dither enabled, IPEAK = 0
    HWREG( ADI3_BASE + ADI_O_NONSECWR1) = (( 0x08 << ADI_NONSECWR1_DATA_S) & ADI_NONSECWR1_DATA_M ) |
                                          ((( ( ADI_3_REFSYS_O_DCDCCTL5) ) << ADI_NONSECWR1_ADDR_S ) & ADI_NONSECWR1_ADDR_M) |
                                          (( 0x0F << ADI_NONSECWR1_WR_MASK_S) & ADI_NONSECWR1_WR_MASK_M);

    // Dither disabled, IPEAK = 3
    HWREG( ADI3_BASE + ADI_O_NONSECWR2) = (( 0x03 << ADI_NONSECWR2_DATA_S) & ADI_NONSECWR2_DATA_M ) |
                                          ((( ( ADI_3_REFSYS_O_DCDCCTL5)  ) << ADI_NONSECWR2_ADDR_S ) & ADI_NONSECWR2_ADDR_M) |
                                          (( 0x0F << ADI_NONSECWR2_WR_MASK_S) & ADI_NONSECWR2_WR_MASK_M);

    // Dither disabled, IPEAK = 7
    HWREG( ADI3_BASE + ADI_O_NONSECWR3) = (( 0x07 << ADI_NONSECWR3_DATA_S) & ADI_NONSECWR3_DATA_M ) |
                                          ((( ( ADI_3_REFSYS_O_DCDCCTL5) ) << ADI_NONSECWR3_ADDR_S ) & ADI_NONSECWR3_ADDR_M) |
                                          (( 0x0F << ADI_NONSECWR3_WR_MASK_S) & ADI_NONSECWR3_WR_MASK_M);

    // Configure Sensor Controller access to TDC clock control
    // Allow read and write ACLK_TDC_SRC_SEL and ACLK_REF_SRC_SEL fields in
    // DDI_0_OSC_O_CTL0 register.
    // The ADDR field is encoded as a half-word index. So we need to divide the
    // regular byte-offset by two.
    // Use NONSECDDIACC3 because if ADDR is 0 for multiple NONSECDDIACCn,
    // only the highest NONSECDIACCn takes effect. 0 is the reset value for
    // ADDR and DDI_0_OSC_O_CTL0 has an ADDR offset of 0. So to access this
    // Register, we should always aim to use the highest NONSECDDIACCn.
    HWREG( AUX_SCE_BASE + AUX_SCE_O_NONSECDDIACC3) = AUX_SCE_NONSECDDIACC3_RD_EN |
                                                     (( ( DDI_0_OSC_O_CTL0 / 2 ) << AUX_SCE_NONSECDDIACC3_ADDR_S ) & AUX_SCE_NONSECDDIACC3_ADDR_M ) |
                                                     (( DDI_0_OSC_CTL0_ACLK_TDC_SRC_SEL_M | DDI_0_OSC_CTL0_ACLK_REF_SRC_SEL_M ) & AUX_SCE_NONSECDDIACC3_WR_MASK_M );

    // Configure Sensor Controller access to COMPB 32 kHz clock enable
    // We want to read and write DDI_0_OSC_ATESTCTL_SCLK_LF_AUX_EN.
    // This is a 16-bit write, so we need to add to the address offset since
    // DDI_0_OSC_ATESTCTL_SCLK_LF_AUX_EN is in the upper half of the register.
    // The ADDR field is encoded as a half-word index. So we need to divide the
    // regular byte-offset by two.
    // The mask needs to be shifted down because the original register field
    // mask is provided as a 32-bit mask.
    HWREG( AUX_SCE_BASE + AUX_SCE_O_NONSECDDIACC1) = AUX_SCE_NONSECDDIACC1_RD_EN |
                                                     ((( ( DDI_0_OSC_O_ATESTCTL + 2 ) / 2 ) << AUX_SCE_NONSECDDIACC1_ADDR_S ) & AUX_SCE_NONSECDDIACC1_ADDR_M) |
                                                     (( DDI_0_OSC_ATESTCTL_SCLK_LF_AUX_EN_M >> 16 ) & AUX_SCE_NONSECDDIACC1_WR_MASK_M);

    // Enable output of RTC clock for Radio Timer Synchronization
    HWREG(AON_RTC_BASE + AON_RTC_O_CTL) |= AON_RTC_CTL_RTC_UPD_EN_M;
}

//*****************************************************************************
//
//! \brief Trims to be applied when coming from POWER_DOWN (also called when
//! coming from SHUTDOWN and PIN_RESET).
//!
//! \return None
//
//*****************************************************************************
static void
TrimAfterColdResetWakeupFromShutDownWakeupFromPowerDown( void )
{
    // Currently no specific trim for Powerdown
}

//*****************************************************************************
//
//! \brief Trims to be applied when coming from SHUTDOWN (also called when
//! coming from PIN_RESET).
//!
//! \param ui32Fcfg1Revision
//!
//! \return None
//
//*****************************************************************************
static void
TrimAfterColdResetWakeupFromShutDown(uint32_t ui32Fcfg1Revision)
{
    uint32_t   ccfg_ModeConfReg  ;

    // Check in CCFG for alternative DCDC setting
    if (( HWREG( CCFG_BASE + CCFG_O_SIZE_AND_DIS_FLAGS ) & CCFG_SIZE_AND_DIS_FLAGS_DIS_ALT_DCDC_SETTING ) == 0 ) {
        // ADI_3_REFSYS:DCDCCTL5[3]  (=DITHER_EN) = CCFG_MODE_CONF_1[19]   (=ALT_DCDC_DITHER_EN)
        // ADI_3_REFSYS:DCDCCTL5[2:0](=IPEAK    ) = CCFG_MODE_CONF_1[18:16](=ALT_DCDC_IPEAK    )
        // Using a single 4-bit masked write since layout is equal for both source and destination
        HWREGB( ADI3_BASE + ADI_O_MASK4B + ( ADI_3_REFSYS_O_DCDCCTL5 * 2 )) = ( 0xF0 |
            ( HWREG( CCFG_BASE + CCFG_O_MODE_CONF_1 ) >> CCFG_MODE_CONF_1_ALT_DCDC_IPEAK_S ));
    }

    // Force DCDC to use RCOSC before starting up XOSC.
    // Clock loss detector does not use XOSC until SCLK_HF actually switches
    // and thus DCDC is not protected from clock loss on XOSC in that time frame.
    // The force must be released when the switch to XOSC has happened. This is done
    // in OSCHfSourceSwitch().
    HWREG(AUX_DDI0_OSC_BASE + DDI_O_MASK16B + (DDI_0_OSC_O_CTL0 << 1) + 4) = DDI_0_OSC_CTL0_CLK_DCDC_SRC_SEL_M | (DDI_0_OSC_CTL0_CLK_DCDC_SRC_SEL_M >> 16);
    // Dummy read to ensure that the write has propagated
    HWREGH(AUX_DDI0_OSC_BASE + DDI_0_OSC_O_CTL0);

    // read the MODE_CONF register in CCFG
    ccfg_ModeConfReg = HWREG( CCFG_BASE + CCFG_O_MODE_CONF );

    // First part of trim done after cold reset and wakeup from shutdown:
    // -Adjust the VDDR_TRIM_SLEEP value.
    // -Configure DCDC.
    SetupAfterColdResetWakeupFromShutDownCfg1( ccfg_ModeConfReg );

    // Addition to the CC1352 boost mode for HWREV >= 2.0
    // The combination VDDR_EXT_LOAD=0 and VDDS_BOD_LEVEL=1 is defined to select boost mode
    if ((( ccfg_ModeConfReg & CCFG_MODE_CONF_VDDR_EXT_LOAD  ) == 0 ) &&
        (( ccfg_ModeConfReg & CCFG_MODE_CONF_VDDS_BOD_LEVEL ) != 0 )    )
    {
        HWREGB( ADI3_BASE + ADI_3_REFSYS_O_DCDCCTL3 ) = ADI_3_REFSYS_DCDCCTL3_VDDR_BOOST_COMP_BOOST ;
    }

    // Second part of trim done after cold reset and wakeup from shutdown:
    // -Configure XOSC.
#if ( CCFG_BASE == CCFG_BASE_DEFAULT )
    SetupAfterColdResetWakeupFromShutDownCfg2( ui32Fcfg1Revision, ccfg_ModeConfReg );
#else
    NOROM_SetupAfterColdResetWakeupFromShutDownCfg2( ui32Fcfg1Revision, ccfg_ModeConfReg );
#endif

    {
        uint32_t  trimReg        ;
        uint32_t  ui32TrimValue  ;

        //--- Propagate the LPM_BIAS trim ---
        trimReg = HWREG( FCFG1_BASE + FCFG1_O_DAC_BIAS_CNF );
        ui32TrimValue = (( trimReg & FCFG1_DAC_BIAS_CNF_LPM_TRIM_IOUT_M ) >>
                                     FCFG1_DAC_BIAS_CNF_LPM_TRIM_IOUT_S ) ;
        HWREGB( AUX_ADI4_BASE + ADI_4_AUX_O_LPMBIAS ) = (( ui32TrimValue << ADI_4_AUX_LPMBIAS_LPM_TRIM_IOUT_S ) &
                                                                            ADI_4_AUX_LPMBIAS_LPM_TRIM_IOUT_M ) ;
        // Set LPM_BIAS_BACKUP_EN according to FCFG1 configuration
        if ( trimReg & FCFG1_DAC_BIAS_CNF_LPM_BIAS_BACKUP_EN ) {
            HWREGB( ADI3_BASE + ADI_O_SET + ADI_3_REFSYS_O_AUX_DEBUG ) = ADI_3_REFSYS_AUX_DEBUG_LPM_BIAS_BACKUP_EN;
        } else {
            HWREGB( ADI3_BASE + ADI_O_CLR + ADI_3_REFSYS_O_AUX_DEBUG ) = ADI_3_REFSYS_AUX_DEBUG_LPM_BIAS_BACKUP_EN;
        }
        // Set LPM_BIAS_WIDTH_TRIM according to FCFG1 configuration
        {
            uint32_t widthTrim = (( trimReg & FCFG1_DAC_BIAS_CNF_LPM_BIAS_WIDTH_TRIM_M ) >> FCFG1_DAC_BIAS_CNF_LPM_BIAS_WIDTH_TRIM_S );
            HWREGH( AUX_ADI4_BASE + ADI_O_MASK8B + ( ADI_4_AUX_O_COMP * 2 )) = // Set LPM_BIAS_WIDTH_TRIM = 3
                (( ADI_4_AUX_COMP_LPM_BIAS_WIDTH_TRIM_M << 8         ) |       // Set mask (bits to be written) in [15:8]
                 ( widthTrim << ADI_4_AUX_COMP_LPM_BIAS_WIDTH_TRIM_S )   );    // Set value (in correct bit pos) in [7:0]
        }
    }

    // Third part of trim done after cold reset and wakeup from shutdown:
    // -Configure HPOSC.
    // -Setup the LF clock.
#if ( CCFG_BASE == CCFG_BASE_DEFAULT )
    SetupAfterColdResetWakeupFromShutDownCfg3( ccfg_ModeConfReg );
#else
    NOROM_SetupAfterColdResetWakeupFromShutDownCfg3( ccfg_ModeConfReg );
#endif

    // Set AUX into power down active mode
    AUXSYSIFOpModeChange( AUX_SYSIF_OPMODE_TARGET_PDA );

    // Disable EFUSE clock
    // TBD - Not sure how this is done on Thor1M at moment
}


//*****************************************************************************
//
//! \brief Trims to be applied when coming from PIN_RESET.
//!
//! \return None
//
//*****************************************************************************
static void
TrimAfterColdReset( void )
{
    // Currently no specific trim for Cold Reset
}
