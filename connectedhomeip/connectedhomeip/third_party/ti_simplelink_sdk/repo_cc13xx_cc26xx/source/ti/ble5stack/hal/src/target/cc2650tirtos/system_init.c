/******************************************************************************

 @file  system_init.c

 @brief This file contains generic System initialization code.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated
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

/*******************************************************************************
 * INCLUDES
 */
#include <icall.h>
#include <inc/hw_memmap.h>
#include "hal_types.h"
#include <inc/hw_types.h>
#include <inc/hw_sysctl.h>
#include <driverlib/ioc.h>
#include <inc/hw_ioc.h>
#include <driverlib/vims.h>
#include <driverlib/uart.h>
#include <driverlib/gpio.h>
#include "hapi.h"
#include <driverlib/prcm.h>
//
#include "rfHal.h"

#ifdef DEBUG_SW_TRACE
#define DBG_ENABLE
#include "dbgid_sys_mst.h"
#endif // DEBUG_SW_TRACE

#include <inc/hw_prcm.h>
#include <inc/hw_aon_wuc.h>
#include <inc/hw_aux_wuc.h>
#include "hw_osc_dig.h"
#include "hw_rf24_dbell.h"
#include "hw_rf24_pwr.h"

#include <inc/hw_adi.h>
#include <inc/hw_ddi.h>
#include <inc/hw_adi_2_refsys.h>
#include <inc/hw_adi_3_refsys.h>
#include <inc/hw_adi_4_aux.h>
#include "hw_aon_sysctrl12.h"
#include <inc/hw_factory_cfg.h>
#include <driverlib/ddi.h>
#include <driverlib/adi.h>

#define DELAY(t) {for (volatile uint32 i=0; i<(t); i++){}}
#define DELAY_TIME 0x1000

#define DELAY_CHECK( t, addr, mask, val )                                      \
  for (volatile uint32 i=0; i<(t); i++)                                        \
  {                                                                            \
    if ( (HWREG( (addr) ) & (mask)) == (val) ) break;                          \
  }

/*******************************************************************************
 * EXTERNS
 */

/*******************************************************************************
 * PROTOTYPES
 */
static bool trimDevice(void);
void enableExternalClock( void );
void prepareInternalClock( void );
void System_Setup( void );

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */
// 'Magic' trim value
#define FINAL_TRIM_DONE         0xC5

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * @fn          System Initialization
 *
 * @brief       This routine performs various system initialization.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
__root void System_Init( void )
{
  // setup system
  System_Setup();

  // Set cache mode
  // Disable cache
  VIMSModeSet( VIMS_BASE, VIMS_MODE_DISABLED );

  // Wait for disabling to be complete
  while ( VIMSModeGet( VIMS_BASE ) != VIMS_MODE_DISABLED );

  // enable iCache prefetching
  VIMSConfigure(VIMS_BASE, TRUE, TRUE);

  // Re-enable cache
  VIMSModeSet( VIMS_BASE, VIMS_MODE_ENABLED );

#if defined(HAL_UART) && (HAL_UART==TRUE)
  // UART For Debug Package - Flash EM:
  IOCPinTypeUart(UART0_BASE, IOID_26, IOID_23, IOID_UNUSED, IOID_UNUSED);

  // Enable tracer output on DIO24/ATEST1.
  // Set max drive strength and reduced slew rate.
  // PortID = 46 = RFcore tracer.
  HWREG(IOC_BASE + IOC_O_IOCFG24) = (3 << IOC_IOCFG24_IOSTR_S) | (1 << IOC_IOCFG24_IOSC_S) | 46;

#else // no UART so allow trace on Tx pin - Debug Package only
  // Enable tracer output on DIO23/ATEST0.
  // Set max drive strength and reduced slew rate.
  // PortID = 46 = RFcore tracer.
  HWREG(IOC_BASE + IOC_O_IOCFG23) = (3 << IOC_IOCFG23_IOSTR_S) | (1 << IOC_IOCFG23_IOSC_S) | 46;

#endif // HAL_UART

  return;
}

//*****************************************************************************
//
//! Perform the necessary trim of the device which is not done in boot code
//
//*****************************************************************************
void
resetHook(void)
{
  System_Init();
}
/*******************************************************************************
 * @fn          System Setup
 *
 * @brief       This routine performs what is required to setup the system
 *              such that software can execute from Flash properly.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void System_Setup()
{
  rfOpImmedCmd_ReadWriteFwParam_t writeFwParCmd;
  rfOpImmedCmd_UpdateRadioSetup_t updateRadioSetupCmd;

  uint32_t tmpOverrides[9];

  bool bIrefOverride;

  bIrefOverride = trimDevice();

  // Turn on power to AUX
  HWREG(AON_WUC_BASE + AON_WUC_O_AUXEVENT) = AON_WUC_AUXEVENT_FORCEON;
  while(!(HWREG(AON_WUC_BASE + AON_WUC_O_PWRSTAT) & AON_WUC_PWRSTAT_AUXPWR)) {};

  // Turn on clocks to the DDI interface
  HWREG(AUX_WUC_BASE + AUX_WUC_O_MODCLKEN) = AUX_WUC_MODCLKEN_OSCCTRL;

  // Turn on peripheral, serial and RF core power domain
  uint32_t powerMask = ( PRCM_SWPWR_CTRL_PERIPH |
                         PRCM_SWPWR_CTRL_SERIAL |
                         PRCM_SWPWR_CTRL_RFCORE );

  // Turn on peripheral, serial and RF core power domain
  HWREG( PRCM_BASE + PRCM_O_SWPWR_CTRL ) = powerMask;



  if (bIrefOverride) {
    // Set correct synth config value so that ADI1 can be written
    writeFwParCmd.cmdNum = CMD_WRITE_FW_PARAM;
    writeFwParCmd.address = 0x106E;
    writeFwParCmd.value = 0x0001;

    // Make update setup command that will write the ADI
    updateRadioSetupCmd.cmdNum = CMD_UPDATE_RADIO_SETUP;
    updateRadioSetupCmd.reserved = 0;
    updateRadioSetupCmd.pRegOverride = (regOverride_t *) tmpOverrides;

    // Make override that writes ADI bits
    tmpOverrides[0] = 0x52220002;
    tmpOverrides[1] = 0x58220002;
    tmpOverrides[2] = 0x70220002;
    tmpOverrides[3] = 0x76220002;
    tmpOverrides[4] = 0x40220002;
    tmpOverrides[5] = 0x46440002;
    tmpOverrides[6] = 0x4C220002;
    tmpOverrides[7] = 0xC022188A;
    tmpOverrides[8] = 0xFFFFFFFF;
  }

  // Wait until all power domains are powered up
  while (( HWREG( PRCM_BASE + PRCM_O_SWPWR_STATUS ) & powerMask ) != powerMask ) { }

  // Enable clocks for all radio internal modules.
  // Use Non-Buff access for safety and check for sanity
  HWREG(RF24_PWR_NONBUF_BASE + RF24_PWR_O_PWMCLKENABLE) = 0x7FF;
  // Do not read back to check, for two reasons:
  // 1. CPE will update the PWMCLKENABLE register right after boot
  // 2. The PWMCLKENABLE register always reads back what is written

  if (bIrefOverride) {
    HWREG(RF24_DBELL_NONBUF_BASE + RF24_DBELL_O_CMDR) = (uint32_t) &writeFwParCmd;
    // Wait for command to be processed
    while (HWREG(RF24_DBELL_BASE + RF24_DBELL_O_CMDSTA) == 0);

    // Send command to RF core to always enable bias for analog modules. This must be done while running on RCOSC.
    // This helps preventing the problem where XOSC clock is suddenly changed
    HWREG(RF24_DBELL_NONBUF_BASE + RF24_DBELL_O_CMDR) = (uint32_t) &updateRadioSetupCmd;
    // Wait for command to be processed
    while (HWREG(RF24_DBELL_BASE + RF24_DBELL_O_CMDSTA) == 0);
  }

  // enable clock
#ifdef EXTERNAL_CLOCK
  enableExternalClock();
#else
  prepareInternalClock();
#endif

  // Turn on the UDMA, TRNG and CRYPTO clock in CM3 Run, Sleep and DeepSleep mode
  HWREG(PRCM_BASE + PRCM_O_SEC_DMA_RCG) = PRCM_SEC_DMA_RCG_DMA | PRCM_SEC_DMA_RCG_TRNG | PRCM_SEC_DMA_RCG_CRYPTO;
  HWREG(PRCM_BASE + PRCM_O_SEC_DMA_SCG) = PRCM_SEC_DMA_SCG_DMA | PRCM_SEC_DMA_SCG_TRNG | PRCM_SEC_DMA_SCG_CRYPTO;
  HWREG(PRCM_BASE + PRCM_O_SEC_DMA_DSCG) = PRCM_SEC_DMA_DSCG_DMA | PRCM_SEC_DMA_DSCG_TRNG | PRCM_SEC_DMA_DSCG_CRYPTO;

  // Turn on the GPIO clock in CM3 Run, Sleep and DeepSleep mode
  HWREG(PRCM_BASE + PRCM_O_GPIO_RCG) = PRCM_GPIO_RCG_GPIO;
  HWREG(PRCM_BASE + PRCM_O_GPIO_SCG) = PRCM_GPIO_SCG_GPIO;
  HWREG(PRCM_BASE + PRCM_O_GPIO_DSCG) = PRCM_GPIO_DSCG_GPIO;

  // Turn on the GPT clocks in CM3 Run, Sleep and DeepSleep mode
  HWREG(PRCM_BASE + PRCM_O_GPTM_RCG) = PRCM_GPTM_RCG_GPTM_M;
  HWREG(PRCM_BASE + PRCM_O_GPTM_SCG) = PRCM_GPTM_SCG_GPTM_M;
  HWREG(PRCM_BASE + PRCM_O_GPTM_DSCG) = PRCM_GPTM_DSCG_GPTM_M;

  // Turn on the I2C clock in CM3 Run, Sleep and DeepSleep mode
  HWREG(PRCM_BASE + PRCM_O_I2C_RCG) = PRCM_I2C_RCG_I2C;
  HWREG(PRCM_BASE + PRCM_O_I2C_SCG) = PRCM_I2C_SCG_I2C;
  HWREG(PRCM_BASE + PRCM_O_I2C_DSCG) = PRCM_I2C_DSCG_I2C;

  // Turn on the UART clock in CM3 Run, Sleep and DeepSleep mode
  HWREG(PRCM_BASE + PRCM_O_UART_RCG) = PRCM_UART_RCG_UART;
  HWREG(PRCM_BASE + PRCM_O_UART_SCG) = PRCM_UART_SCG_UART;
  HWREG(PRCM_BASE + PRCM_O_UART_DSCG) = PRCM_UART_DSCG_UART;

  // Turn on the SSI clock in CM3 Run, Sleep and DeepSleep mode
  HWREG(PRCM_BASE + PRCM_O_SSI_RCG) = PRCM_SSI_RCG_SSI_M;
  HWREG(PRCM_BASE + PRCM_O_SSI_SCG) = PRCM_SSI_SCG_SSI_M;
  HWREG(PRCM_BASE + PRCM_O_SSI_DSCG) = PRCM_SSI_DSCG_SSI_M;

  // Turn on the I2S clock in CM3 Run, Sleep and DeepSleep mode
  HWREG(PRCM_BASE + PRCM_O_I2S_RCG) = PRCM_I2S_RCG_I2S;
  HWREG(PRCM_BASE + PRCM_O_I2S_SCG) = PRCM_I2S_SCG_I2S;
  HWREG(PRCM_BASE + PRCM_O_I2S_DSCG) = PRCM_I2S_DSCG_I2S;

  // Load the clock settings and check the clocks are updated
  HWREG(PRCM_BASE + PRCM_O_CLKCTRL) = PRCM_CLKCTRL_LOAD;
  while(!(HWREG(PRCM_BASE + PRCM_O_CLKCTRL) & PRCM_CLKCTRL_LOAD_STATUS));

  // CM0 Bad Pointer Workaround (PG1.0 Only)
  // Moved to cm0_patch.h
  // *((uint32 *)0x21000028) = 0x00000000;

  // NOTE: This could be done later to save some boot time
  // Wait for XOSC to be ready
  while ((((HWREG(AUX_OSCDDI_BASE + OSC_DIG_O_STATUS0 ))) & 1 ) == 0);
  // Switch safely to RCOSC
  HapiHFSourceSafeSwitch();

  return;
}

#ifdef EXTERNAL_CLOCK
void enableExternalClock( void )
{
 // Use external Clock: PC
  HWREG(AUX_OSCDDI_BASE + OSC_DIG_O_AMPCOMP_TH2)           =  0x68880000; // Sets AMPCOMP settings for switching to XOSCHF
  HWREG(AUX_OSCDDI_BASE + OSC_DIG_O_AMPCOMP_TH1)           =  0x886876A4; // Sets AMPCOMP settings for switching to XOSCHF
  HWREG(AUX_OSCDDI_BASE + OSC_DIG_O_ANABYPASS_VALUE1)      =  0x24003FFF; // sets CAP SETTINGS
  HWREG(AUX_OSCDDI_BASE + OSC_DIG_O_ANABYPASS_VALUE2)      =  0x780F0FFF; // Sets XOSCHF IBIAS THERM
  HWREG(AUX_OSCDDI_BASE + OSC_DIG_O_XOSCLF_RCOSCLF_CTRL)   =  0x000000D8; // Sets default RCOSC_LF trim
  HWREG(AUX_OSCDDI_BASE + OSC_DIG_O_ADC_DBLR_NANOAMP_CTRL) =  0x1F000000; // Sets default NANOAMP_BIAS settings
  HWREG(AUX_OSCDDI_BASE + OSC_DIG_O_AMPCOMP_CTRL)          =  0x3000000; // Sets AMPCOMP settings for switching to XOSCHF
  HWREG(AUX_OSCDDI_BASE + OSC_DIG_O_XOSCHF_CTRL)           =  0x00000040; // Sets XOSC in bypass
  HWREG(AUX_OSCDDI_BASE + OSC_DIG_O_CTRL0)                 =  0x00010001; // Sets XOSC to clock source and allow switching
  // HWREG(AUX_OSCDDI_BASE + OSC_DIG_O_CTRL0) =  0x00010005; // Sets XOSC to LF clock source and allow RCOSC_HF to be off
  // end use external clock from PC

  return;
}
#endif


void prepareInternalClock( void )
{
  uint32 status0;

  // Prepare for switching to XOSC HF clock
  status0 = HWREG(AUX_OSCDDI_BASE + OSC_DIG_O_STATUS0);
  if (status0 & OSC_DIG_STATUS0_SCLK_HF_SRC_M)
  {
    // Currently running on XOSC. Switch back to RCOSC to be able to switch settings
    HWREG(AUX_OSCDDI_BASE + OSC_DIG_O_CTRL0 ) =
      (1 << OSC_DIG_CTRL0_XTAL_IS_24M_S) |      // 24 MHz xtal
      (3 << OSC_DIG_CTRL0_SCLK_LF_SRC_SEL_S) |  // Use 32 kHz XOSC for LF clk
      (0 << OSC_DIG_CTRL0_SCLK_MF_SRC_SEL_S) |  // Use RCOSC for MF clk
      (0 << OSC_DIG_CTRL0_SCLK_HF_SRC_SEL_S);   // Use RCOSC as clock


    while (((status0 = (HWREG(AUX_OSCDDI_BASE + OSC_DIG_O_STATUS0 ))) & 1 ) == 0);

    // Switch safely to RCOSC
    HapiHFSourceSafeSwitch();
   }

  // Enable xosc_hf_clk
  HWREG( AUX_OSCDDI_BASE + OSC_DIG_O_CTRL0 ) =
      (1 << OSC_DIG_CTRL0_XTAL_IS_24M_S) |      // 24 MHz xtal
      (3 << OSC_DIG_CTRL0_SCLK_LF_SRC_SEL_S) |  // Use 32 kHz XOSC for LF clk
      //(0 << OSC_DIG_CTRL0_SCLK_LF_SRC_SEL_S) |  // 1: HF XOSC divided to 32kHz, 0: RC OSC divided to 32kHz
      (1 << OSC_DIG_CTRL0_SCLK_MF_SRC_SEL_S) |  // Use XOSC for MF clk
      (1 << OSC_DIG_CTRL0_SCLK_HF_SRC_SEL_S);   // Use XOSC as clock

  return;
}


//*****************************************************************************
//
//! Perform the necessary trim of the device which is not done in boot code
//! Returns true if Iref override was performed; false if skipped because it
//! was previously done
//
//*****************************************************************************
static bool trimDevice(void)
{
    uint32_t ui32Trim;
    uint32_t ui32Reg;
    uint32_t ui32ResetEvents;


    //
    // Check if trimming is required
    //
    if(HWREGB(ADI3_BASE + ADI_3_REFSYS_O_ADI3_SPARE3) != FINAL_TRIM_DONE)
    {
        //
        // Force AUX on and enable clocks
        //
        // No need to save the current status of the power/clock registers.
        // At this point both AUX and AON should have been reset to 0x0.
        //
        HWREG(AON_WUC_BASE + AON_WUC_O_AUXEVENT) = AON_WUC_AUXEVENT_FORCEON;
        HWREG(AUX_WUC_BASE + AUX_WUC_O_MODCLKEN) = AUX_WUC_MODCLKEN_OSCCTRL;

        // --- DCDC 1A trim ---
        // Set VDDR to 1.7V
        //
        // -- Disable reset sources while trimming
        ui32ResetEvents = HWREG(AON_SYSCTRL12_BASE + AON_SYSCTRL12_O_RESET);
        HWREG(AON_SYSCTRL12_BASE + AON_SYSCTRL12_O_RESET) = 0x00;
        // Patric Cruise: Make this staircase this because  applying large step in
        // VDDR can cause system reset
        // TBD : The final value is device dependent!!!
        ADI8SetValBit(ADI3_BASE, ADI_3_REFSYS_O_DCDC_CTRL_1,
                      ADI_3_REFSYS_DCDC_CTRL_1_VDDR_TRIM_3P3V_M,0x01);
        ADI8SetValBit(ADI3_BASE, ADI_3_REFSYS_O_DCDC_CTRL_1,
                      ADI_3_REFSYS_DCDC_CTRL_1_VDDR_TRIM_3P3V_M,0x03);
        ADI8SetValBit(ADI3_BASE, ADI_3_REFSYS_O_DCDC_CTRL_1,
                      ADI_3_REFSYS_DCDC_CTRL_1_VDDR_TRIM_3P3V_M,0x05);
        // ----- Default trim sequence end, re-enable reset sources -----
        HWREG(AON_SYSCTRL12_BASE + AON_SYSCTRL12_O_RESET) = ui32ResetEvents;

        //
        // Following sequence is required for using XTAL, if not included
        // devices crashes when trying to switch to XTAL.
        //
        ui32Reg = HWREG(FCFG1_BASE + FACTORY_CFG_O_CONFIG_OSC_TOP);

        // Ryan Smith: If this register is zero the boot process should be halted!
        // TBD - is this a likely scenario???
        if(ui32Reg == 0)
        {
            while(1);
        }
        // Sets CAP SETTINGS
        DDI32RegWrite(AUX_OSCDDI_BASE, OSC_DIG_O_ANABYPASS_VALUE1, 0x000F0FFF);
        /*
        ui32Trim = (ui32Reg & FACTORY_CFG_CONFIG_OSC_TOP_XOSC_HF_CAP_TRIM_M) >>
            FACTORY_CFG_CONFIG_OSC_TOP_XOSC_HF_CAP_TRIM_S;
        DDI16BitfieldWrite(AUX_OSCDDI_BASE, OSC_DIG_O_ANABYPASS_VALUE1,
                           OSC_DIG_ANABYPASS_VALUE1_XOSC_HF_COLUMN_Q12_M,
                           OSC_DIG_ANABYPASS_VALUE1_XOSC_HF_COLUMN_Q12_S, ui32Trim);
        */
        // Sets default RCOSC_LF trim
        ui32Trim = (ui32Reg & FACTORY_CFG_CONFIG_OSC_TOP_RCOSCLF_CTUNE_TRIM_M) >>
            FACTORY_CFG_CONFIG_OSC_TOP_RCOSCLF_CTUNE_TRIM_S;

        DDI16BitfieldWrite(AUX_OSCDDI_BASE, OSC_DIG_O_XOSCLF_RCOSCLF_CTRL,
                           OSC_DIG_XOSCLF_RCOSCLF_CTRL_RCOSCLF_CTUNE_TRIM_M,
                           OSC_DIG_XOSCLF_RCOSCLF_CTRL_RCOSCLF_CTUNE_TRIM_S,
                           ui32Trim);
        // Sets XOSCHF IBIAS THERM
        DDI32RegWrite(AUX_OSCDDI_BASE, OSC_DIG_O_ANABYPASS_VALUE2, 0x000003FF);
        // Sets AMPCOMP settings for switching to XOSCHF
        DDI32RegWrite(AUX_OSCDDI_BASE, OSC_DIG_O_AMPCOMP_TH2, 0x68880000);
        // Sets AMPCOMP settings for switching to XOSCHF
        DDI32RegWrite(AUX_OSCDDI_BASE, OSC_DIG_O_AMPCOMP_TH1, 0x886876A4);
        // Sets AMPCOMP settings for switching to XOSCHF
        DDI32RegWrite(AUX_OSCDDI_BASE, OSC_DIG_O_AMPCOMP_CTRL, 0x00713F27);
        // TBD : What does this do???
        DDI32RegWrite(AUX_OSCDDI_BASE, OSC_DIG_O_RADC_EXTERNAL_CFG, 0x403F4000);

        // Disable clock for OSC_DIG and release power on AUX
        HWREG(AUX_WUC_BASE + AUX_WUC_O_MODCLKEN) = 0x0;
        HWREG(AON_WUC_BASE + AON_WUC_O_AUXEVENT) = 0x0;

        // Make sure to flag that trim has been performed
        HWREGB(ADI3_BASE + ADI_3_REFSYS_O_ADI3_SPARE3) = FINAL_TRIM_DONE;

    }

    //
    // Check if IREFs must be enabled to avoid kickback...
    // Should only be necessary after POR.
    //
    if(!(HWREGB(ADI3_BASE + ADI_3_REFSYS_O_DCDC_CTRL_3) &
         ADI_3_REFSYS_DCDC_CTRL_3_EN_DMY_3P3V))
    {
        //
        // Force AUX on and enable clocks
        //
        // No need to save the current status of the power/clock registers.
        // At this point both AUX and AON should have been reset to 0x0.
        //
        HWREG(AON_WUC_BASE + AON_WUC_O_AUXEVENT) = AON_WUC_AUXEVENT_FORCEON;
        HWREG(AUX_WUC_BASE + AUX_WUC_O_MODCLKEN) = AUX_WUC_MODCLKEN_OSCCTRL |
                                                   AUX_WUC_MODCLKEN_AUX_ADI;

        //
        // Enable IREF in the DCDC - needed to avoid kickback which will potentially
        // trick the frequency doubler in to locking to a lower harmonic.
        //
        HWREGB(ADI3_BASE + ADI_O_SET + ADI_3_REFSYS_O_DCDC_CTRL_3) =
            ADI_3_REFSYS_DCDC_CTRL_3_EN_DMY_3P3V;

        //
        // Enable IREF in the SOC LDO - needed to avoid kickback which will
        // potentially trick the frequency doubler in to locking to a lower
        // harmonic.
        //
        HWREGB(ADI2_BASE + ADI_O_SET + ADI_2_REFSYS_O_SOCLDO_1_0) =
            ADI_2_REFSYS_SOCLDO_1_0_EN_IPTAT;

        //
        // Enable IREF in the XOSCHF - needed to avoid kickback which will
        // potentially trick the frequency doubler in to locking to a lower
        // harmonic.
        //
        HWREGH(AUX_OSCDDI_BASE + DDI_O_SET + OSC_DIG_O_XOSCHF_CTRL) =
            OSC_DIG_XOSCHF_CTRL_XOSCHF_HPBUFF_IREF_OVERRIDE |
                OSC_DIG_XOSCHF_CTRL_XOSCHF_PEAKDET_IPTAT_OVERRIDE;

        //
        // Enable IREF in the ADC - needed to avoid kickback which will potentially
        // trick the frequency doubler in to locking to a lower harmonic.
        //
        HWREGB(AUX_ADI_BASE + ADI_O_SET + ADI_4_AUX_O_CTL2_2_ADC_REF) =
            ADI_4_AUX_CTL2_2_ADC_REF_REF_IVREF_EN |
                ADI_4_AUX_CTL2_2_ADC_REF_REF_IREF_EN;
        HWREGB(AUX_ADI_BASE + ADI_O_SET + ADI_4_AUX_O_CTL2_0_ADC) =
            ADI_4_AUX_CTL2_0_ADC_IREF_EN;

        //
        // Disable clock for OSC_DIG and release power on AUX
        //
        HWREG(AUX_WUC_BASE + AUX_WUC_O_MODCLKEN) = 0x0;
        HWREG(AON_WUC_BASE + AON_WUC_O_AUXEVENT) = 0x0;

        return true;
    }

    return false;
}

/*******************************************************************************
 */
