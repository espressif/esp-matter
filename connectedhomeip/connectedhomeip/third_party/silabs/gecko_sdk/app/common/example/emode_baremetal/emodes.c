/***************************************************************************//**
 * @file
 * @brief Energy modes functions
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "emodes.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "core_main.h"
#ifdef CRYOTIMER_PRESENT
#include "em_cryotimer.h"
#endif // CRYOTIMER_PRESENT
#ifdef RTCC_PRESENT
#include "em_rtcc.h"
#endif // RTCC_PRESENT
#ifdef RTC_PRESENT
#include "em_rtc.h"
#endif // RTC_PRESENT
#ifdef BURTC_PRESENT
#include "em_burtc.h"
#endif // BURTC_PRESENT
#ifdef SYSRTC_PRESENT
#include "peripheral_sysrtc.h"
#endif // SYSRTC_PRESENT

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static void disable_clocks(void);
static void disable_HF_clocks(void);
static void disable_LF_clocks(void);
static void prime_calc(void); //Calculate Primes
static void em_EM0(energy_mode_t *mode);
static void em_EM1(energy_mode_t *mode);
static void em_EM2(energy_mode_t *mode);
static void em_EM3(energy_mode_t *mode);
#if !defined(_SILICON_LABS_32B_SERIES_0)
static void em_EM4(energy_mode_t *mode);
#endif // !defined(_SILICON_LABS_32B_SERIES_0)
static void em_EM0_Hfxo(void);
static void em_EM1_Hfxo(void);
#if defined(_SILICON_LABS_32B_SERIES_2)
static void em_EM0_Fsrco(void);
static void em_EM0_Hfrco(CMU_HFRCODPLLFreq_TypeDef band);
static void em_EM1_Fsrco(void);
static void em_EM1_Hfrco(CMU_HFRCODPLLFreq_TypeDef band);
static void em_EM3_UlfrcoBURTC(bool powerdown_ram);
static void em_EM4_LfrcoBURTC(void);
static void em_EM4_UlfrcoBURTC(void);
static void em_EM4_none(void);
#elif defined(_SILICON_LABS_32B_SERIES_1)
static void em_EM0_Hfrco(CMU_HFRCOFreq_TypeDef band);
static void em_EM1_Hfrco(CMU_HFRCOFreq_TypeDef band);
static void em_EM4H_LfxoRTCC(void);
static void em_EM4_none(void);
static void em_EM4S(void);
#else // defined(_SILICON_LABS_32B_SERIES_1)
static void em_EM0_Hfrco(CMU_HFRCOBand_TypeDef band);
static void em_EM1_Hfrco(CMU_HFRCOBand_TypeDef band);
#endif
#if defined(RTCC_PRESENT)
static void em_EM2_RTCC(CMU_Select_TypeDef osc, bool powerdown_ram);
#endif // RTCC_PRESENT
#if defined(RTC_PRESENT)
static void em_EM2_LfrcoRTC(bool powerdown_ram);
#elif defined(SYSRTC_PRESENT)
static void em_EM2_LfrcoSYSRTC(bool powerdown_ram);
#if !(defined(ZGM230SB27HGN) && ZGM230SB27HGN == 1)
/* BRD4205B doesn't have a LFXO mounted out of the box
   hence excluding LFXO options for this board. */
static void em_EM2_LfxoSYSRTC(bool powerdown_ram);
#endif // !(defined(ZGM230SB27HGN) && ZGM230SB27HGN == 1)
#endif //SYSRTC_PRESENT
#if defined(CRYOTIMER_PRESENT)
static void em_EM3_UlfrcoCRYO(bool powerdown_ram);
static void em_EM4H_UlfrcoCRYO(void);
#endif // CRYOTIMER_PRESENT

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Enter EM0 with HFXO running.
 *
 * Enter EM0 Active mode with HFXO running and all other peripherals and
 * clocks disabled.
 ******************************************************************************/
#if defined(_SILICON_LABS_32B_SERIES_2)
static void em_EM0_Hfxo(void)
{
  // Make sure clocks are disabled.
  disable_clocks();

  // Set HFXO as system clock.
#if (_SILICON_LABS_32B_SERIES_2_CONFIG > 1)
  CMU_ClockEnable(cmuClock_HFXO, true);
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_HFXO);
  CMU_ClockEnable(cmuClock_HFXO, false);
#else
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_HFXO);
#endif
}
#else // _SILICON_LABS_32B_SERIES_2
static void em_EM0_Hfxo(void)
{
  // Make sure clocks are disabled.
  disable_clocks();

  // Set HFXO for HF clock.
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  // Make sure unwanted oscillators are disabled specifically for EM0 and HFXO.
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);
  CMU_OscillatorEnable(cmuOsc_AUXHFRCO, false, true);
}
#endif

#if defined(_SILICON_LABS_32B_SERIES_2)
/***************************************************************************//**
 * Enter EM0 with FSRCO running.
 *
 * Enter EM0 Active mode with FSRCO running and all other peripherals and
 * clocks disabled.
 ******************************************************************************/
static void em_EM0_Fsrco(void)
{
  // Make sure clocks are disabled.
  disable_clocks();

  // Set FSRCO as system clock.
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_FSRCO);
}
#endif // _SILICON_LABS_32B_SERIES_2

/***************************************************************************//**
 * Enter EM0 with HFRCO running at desired frequency.
 *
 * Enter EM0 Active mode with HFRCO running Between 72 and 1 MHz and all other
 * peripherals and clocks disabled. It takes an input parameter for selecting
 * the desired frequency.
 ******************************************************************************/
#if defined(_SILICON_LABS_32B_SERIES_2)
static void em_EM0_Hfrco(CMU_HFRCODPLLFreq_TypeDef band)
{
  // Make sure clocks are disabled.
  disable_clocks();

  // Set HFRCO as system clock.
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_HFRCODPLL);

  // Set HFRCO frequency.
  CMU_HFRCODPLLBandSet(band);
}
#else // defined(_SILICON_LABS_32B_SERIES_1) || defined(_SILICON_LABS_32B_SERIES_0)
static void em_EM0_Hfrco(
#if defined(_SILICON_LABS_32B_SERIES_1)
  CMU_HFRCOFreq_TypeDef band
#else
  CMU_HFRCOBand_TypeDef band
#endif
  )
{
  // Make sure clocks are disabled.
  disable_clocks();

  // Set HFRCO for HF clock.
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);

  // Set HFRCO frequency.
  CMU_HFRCOBandSet(band);

  // Make sure unwanted oscillators are disabled specifically for EM0 and HFRCO.
  CMU_OscillatorEnable(cmuOsc_HFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);
  CMU_OscillatorEnable(cmuOsc_AUXHFRCO, false, true);
}
#endif

/***************************************************************************//**
 * Enter EM1 with HFXO running.
 *
 * Enter EM1 Sleep mode with HFXO running and all other peripherals and
 * clocks disabled.
 ******************************************************************************/
#if defined(_SILICON_LABS_32B_SERIES_2)
static void em_EM1_Hfxo(void)
{
  // Make sure clocks are disabled.
  disable_clocks();

  // Set HFXO for as system clock.
#if (_SILICON_LABS_32B_SERIES_2_CONFIG > 1)
  CMU_ClockEnable(cmuClock_HFXO, true);
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_HFXO);
  CMU_ClockEnable(cmuClock_HFXO, false);
#else
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_HFXO);
#endif
  // Enter EM1.
  EMU_EnterEM1();
}
#else // defined(_SILICON_LABS_32B_SERIES_1) || defined(_SILICON_LABS_32B_SERIES_0)
static void em_EM1_Hfxo(void)
{
  // Make sure clocks are disabled.
  disable_clocks();

  // Set HFXO for HF clock.
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  // Make sure unwanted oscillators are disabled specifically for EM1 and HFXO.
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);
  CMU_OscillatorEnable(cmuOsc_AUXHFRCO, false, true);

  // Enter EM1.
  EMU_EnterEM1();
}
#endif

#if defined(_SILICON_LABS_32B_SERIES_2)
/***************************************************************************//**
 * Enter EM1 with FSRCO running.
 *
 * Enter EM1 Sleep mode with FSRCO running and all other peripherals and
 * clocks disabled.
 ******************************************************************************/
static void em_EM1_Fsrco(void)
{
  // Make sure clocks are disabled.
  disable_clocks();

  // Set HFXO for as system clock.
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_FSRCO);

  // Enter EM1.
  EMU_EnterEM1();
}
#endif // _SILICON_LABS_32B_SERIES_2

/***************************************************************************//**
 * Enter EM1 with HFRCO running at desired frequency.
 *
 * Enter EM1 Sleep mode with HFRCO running Between 38 and 1 MHz and all other
 * peripherals and clocks disabled. It takes an input parameter for selecting
 * the desired frequency.
 ******************************************************************************/
#if defined(_SILICON_LABS_32B_SERIES_2)
static void em_EM1_Hfrco(CMU_HFRCODPLLFreq_TypeDef band)
{
  // Make sure clocks are disabled.
  disable_clocks();

  // Set HFRCODPLL as system clock.
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_HFRCODPLL);

  // Set HFRCO frequency.
  CMU_HFRCODPLLBandSet(band);

  // Enter EM1.
  EMU_EnterEM1();
}
#else // defined(_SILICON_LABS_32B_SERIES_1) || defined(_SILICON_LABS_32B_SERIES_0)
static void em_EM1_Hfrco(
#if defined(_SILICON_LABS_32B_SERIES_1)
  CMU_HFRCOFreq_TypeDef band
#else
  CMU_HFRCOBand_TypeDef band
#endif
  )
{
  // Make sure clocks are disabled.
  disable_clocks();

  // Set HFRCO for HF clock.
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);

  // Set HFRCO frequency.
  CMU_HFRCOBandSet(band);

  // Make sure unwanted oscillators are disabled specifically for EM1 and HFRCO.
  CMU_OscillatorEnable(cmuOsc_HFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);
  CMU_OscillatorEnable(cmuOsc_AUXHFRCO, false, true);

  // Enter EM1.
  EMU_EnterEM1();
}
#endif

#if defined(RTCC_PRESENT)
/***************************************************************************//**
 * Enter EM2 with RTCC running on a low frequency oscillator.
 *
 * Enter EM2 Deep Sleep mode with RTCC running from 32.768 kHz LFXO or LFRCO.
 * Parameter osc is used to choose the RTCC source clock. If powerDownRam is set
 * to true, Ram is turned down to retain only 16 kB.
 ******************************************************************************/
#if defined(_SILICON_LABS_32B_SERIES_2)
static void em_EM2_RTCC(CMU_Select_TypeDef osc, bool powerdown_ram)
{
  // Make sure clocks are disabled.
  disable_clocks();

  // Route desired oscillator to RTCC clock tree.
  CMU_ClockSelectSet(cmuClock_RTCCCLK, osc);

  // Setup RTC parameters
  RTCC_Init_TypeDef rtccInit = RTCC_INIT_DEFAULT;
  rtccInit.presc = rtccCntPresc_1;

  // Initialize RTCC
  CMU_ClockEnable(cmuClock_RTCC, true);
  RTCC_Reset();
  RTCC_Init(&rtccInit);

  // Power down all RAM blocks except block 0
  if (powerdown_ram) {
    EMU_RamPowerDown(SRAM_BASE, 0);
  }

  // Enter EM2.
  EMU_EnterEM2(false);
}
#else // _SILICON_LABS_32B_SERIES_2
static void em_EM2_RTCC(CMU_Select_TypeDef osc, bool powerdown_ram)
{
  // Make sure clocks are disabled.
  disable_clocks();

  // Route desired oscillator to LFE to run RTCC.
  CMU_ClockSelectSet(cmuClock_LFE, osc);
  CMU_ClockEnable(cmuClock_RTCC, true);

  // Enable clock to the interface with low energy modules.
  CMU_ClockEnable(cmuClock_HFLE, true);

  // Setup RTC parameters
  RTCC_Init_TypeDef rtccInit = RTCC_INIT_DEFAULT;
  rtccInit.presc = rtccCntPresc_256;
  rtccInit.cntWrapOnCCV1 = true;

  // Initialize RTCC. Configure RTCC with prescaler 256.
  RTCC_Init(&rtccInit);

  // Power down all RAM blocks except block 1
  if (powerdown_ram) {
    EMU_RamPowerDown(SRAM_BASE, 0);
  }

  /* Make sure unwanted oscillators are disabled specifically for EM2 and osc.
     Always disable LFRCO first, even if needed later, in order to make it
     possible to enable LFRCO duty cycle mode */
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true); // Disable LFRCO
  if (osc == cmuSelect_LFRCO) {
    CMU_OscillatorEnable(cmuOsc_LFXO, false, true); // Disable LFXO
    BUS_RegBitWrite(&CMU->LFRCOCTRL, _CMU_LFRCOCTRL_ENVREF_SHIFT, 1); // Enable LFRCO duty cycle mode
    CMU_OscillatorEnable(cmuOsc_LFRCO, true, true); // Enable LFRCO
  }

  // Enter EM2.
  EMU_EnterEM2(false);
}
#endif
#endif // RTCC_PRESENT

#if defined(RTC_PRESENT)
/***************************************************************************//**
 * Enter EM2 with RTC running with LFRCO.
 *
 * Enter EM2 Deep Sleep mode with RTC running with LFRCO, prescaled to 1 Hz
 * from 32.768 kHz LFRCO. If powerDownRam is set to true, Ram is turned down to
 * retain only 16 kB.
 ******************************************************************************/
static void em_EM2_LfrcoRTC(bool powerdown_ram)
{
  // Make sure clocks are disabled.
  disable_clocks();

  // Route the LFRCO clock to RTC.
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
  CMU_ClockEnable(cmuClock_RTC, true);

  // Configure RTC to 1Hz.
  CMU_ClockDivSet(cmuClock_RTC, cmuClkDiv_32768);

  // Enable clock to the interface with low energy modules.
  CMU_ClockEnable(cmuClock_CORELE, true);

  // Setup RTC parameters.
  RTC_Init_TypeDef rtcInit = RTC_INIT_DEFAULT;

  // Initialize RTC.
  RTC_Init(&rtcInit);

  // Power down all RAM blocks except block 1
  if (powerdown_ram) {
    EMU_RamPowerDown(SRAM_BASE, 0);
  }

  // Make sure unwanted oscillators are disabled specifically for EM2 and LFRCO.
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);

  // Enter EM2.
  EMU_EnterEM2(false);
}
#elif defined(SYSRTC_PRESENT)
/***************************************************************************//**
 * Enter EM2 with SYSRTC running with LFRCO.
 *
 * Enter EM2 Deep Sleep mode with SYSRTC running with 32.768 kHz LFRCO and all
 * other peripheral and clocks disabled. If powerDownRam is set to true, Ram is turned
 * down to retain only 16 kB.
 ******************************************************************************/
static void em_EM2_LfrcoSYSRTC(bool powerdown_ram)
{
  // Make sure clocks are disabled.
  disable_clocks();

  // Route the LFRCO clock to SYSRTC.
  CMU_ClockSelectSet(cmuClock_SYSRTC, cmuSelect_LFRCO);
  CMU_ClockEnable(cmuClock_SYSRTC, true);

  // Setup RTC parameters.
  sl_sysrtc_config_t sysrtc_config = SYSRTC_CONFIG_DEFAULT;

  // Initialize RTC.
  sl_sysrtc_init(&sysrtc_config);

  // Power down all RAM blocks except block 1
  if (powerdown_ram) {
    EMU_RamPowerDown(SRAM_BASE, 0);
  }

  // Make sure unwanted oscillators are disabled specifically for EM2 and LFRCO.
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);

  // Enter EM2.
  EMU_EnterEM2(false);
}

#if !(defined(ZGM230SB27HGN) && ZGM230SB27HGN == 1)
/***************************************************************************//**
 * Enter EM2 with SYSRTC running with LFXO.
 *
 * Enter EM2 Deep Sleep mode with SYSRTC running with LFXO and all other
 * peripheral and clocks disabled. If powerDownRam is set to true, Ram is turned
 * down to retain only 16 kB.
 ******************************************************************************/
static void em_EM2_LfxoSYSRTC(bool powerdown_ram)
{
  // Make sure clocks are disabled.
  disable_clocks();

  // Route the LFRCO clock to SYSRTC.
  CMU_ClockSelectSet(cmuClock_SYSRTC, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_SYSRTC, true);

  // Setup RTC parameters.
  sl_sysrtc_config_t sysrtc_config = SYSRTC_CONFIG_DEFAULT;

  // Initialize RTC.
  sl_sysrtc_init(&sysrtc_config);

  // Power down all RAM blocks except block 1
  if (powerdown_ram) {
    EMU_RamPowerDown(SRAM_BASE, 0);
  }

  // Make sure unwanted oscillators are disabled specifically for EM2 and LFXO.
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);

  // Enter EM2.
  EMU_EnterEM2(false);
}
#endif //!(defined(ZGM230SB27HGN) && ZGM230SB27HGN == 1)
#endif // RTC_PRESENT

#if defined(_SILICON_LABS_32B_SERIES_2)
/***************************************************************************//**
 * Enter EM3 with BURTC running on ULFRCO.
 *
 * Enter EM3 Stop mode with BURTC running on 1kHz ULFRCO and all other
 * peripheral and clocks disabled. If powerDownRam is set to true, Ram is turned
 * down to retain only 16 kB.
 ******************************************************************************/
static void em_EM3_UlfrcoBURTC(bool powerdown_ram)
{
  // Make sure clocks are disabled.
  disable_clocks();

  // Select ULFRCO as the BURTC clock source.
  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_ULFRCO);

  // Setup BURTC.
  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;
  CMU_ClockEnable(cmuClock_BURTC, true);
  BURTC_Init(&burtcInit);

  // Power down all RAM blocks except block 1
  if (powerdown_ram) {
    EMU_RamPowerDown(SRAM_BASE, 0);
  }

  // Enter EM3.
  EMU_EnterEM3(false);
}
#endif //_SILICON_LABS_32B_SERIES_2

#if defined(CRYOTIMER_PRESENT)
/***************************************************************************//**
 * Enter EM3 with CRYOTIMER running on ULFRCO.
 *
 * Enter EM3 Stop mode with CRYOTIMER running on 1kHz ULFRCO and all other
 * peripheral and clocks disabled. If powerDownRam is set to true, Ram is turned
 * down to retain only 16 kB.
 ******************************************************************************/
static void em_EM3_UlfrcoCRYO(bool powerdown_ram)
{
  // High and low frequency clocks are disabled in EM3.

  // Setup CRYOTIMER parameters
  CRYOTIMER_Init_TypeDef cryotimerInit = CRYOTIMER_INIT_DEFAULT;

  // Initialize CRYOTIMER. Configure CRYOTIMER to run from ULFRCO
  cryotimerInit.osc = cryotimerOscULFRCO;
  cryotimerInit.presc = cryotimerPresc_128;
  cryotimerInit.period = cryotimerPeriod_256;
  CRYOTIMER_Init(&cryotimerInit);

  // Power down all RAM blocks except block 1
  if (powerdown_ram) {
    EMU_RamPowerDown(SRAM_BASE, 0);
  }

  // Make sure unwanted oscillators are disabled specifically for EM3 and ULFRCO.
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);

  // Enter EM3.
  EMU_EnterEM3(false);
}
#endif // CRYOTIMER_PRESENT

#if defined(_SILICON_LABS_32B_SERIES_2)
/***************************************************************************//**
 * Enter EM4 with BURTC running on a LFRCO.
 *
 * Enter EM4 Hibernate Mode with BURTC running on a 32.768 kHz LFRCO. Only 128b
 * RAM is retained in this mode.
 ******************************************************************************/
static void em_EM4_LfrcoBURTC(void)
{
  // Make sure clocks are disabled.
  disable_clocks();

  // Select LFRCO as the BURTC clock source.
  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_LFRCO);

  // Setup BURTC.
  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;
  CMU_ClockEnable(cmuClock_BURTC, true);
  BURTC_Init(&burtcInit);

  // Enter EM4.
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  em4Init.retainLfxo = true;
  em4Init.pinRetentionMode = emuPinRetentionLatch;
  EMU_EM4Init(&em4Init);
  EMU_EnterEM4();
}
#elif defined(_SILICON_LABS_32B_SERIES_1)
/***************************************************************************//**
 * Enter EM4H with RTCC running on a LFXO.
 *
 * Enter EM4H Hibernate Mode with RTCC running on LFXO.
 * Only 128b RAM is retained in this mode.
 ******************************************************************************/
static void em_EM4H_LfxoRTCC(void)
{
  // Make sure clocks are disabled.
  disable_clocks();

  // Route the LFXO clock to RTCC.
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_RTCC, true);

  // Enable clock to the interface with low energy modules.
  CMU_ClockEnable(cmuClock_HFLE, true);

  // Setup RTC parameters.
  RTCC_Init_TypeDef rtccInit = RTCC_INIT_DEFAULT;
  rtccInit.presc = rtccCntPresc_1;
  rtccInit.cntWrapOnCCV1 = true;
  rtccInit.debugRun = true;

  // Initialize RTCC. Configure RTCC with prescaler 1.
  RTCC_Init(&rtccInit);

  // Make sure unwanted oscillators are disabled specifically for EM4H and LFXO.
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);

  // EM4H retains 128 byte RAM through RTCC by default.

  // Enter EM4H.
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  em4Init.em4State = emuEM4Hibernate;
  em4Init.retainLfxo = true;
  em4Init.pinRetentionMode = emuPinRetentionLatch;
  EMU_EM4Init(&em4Init);
  EMU_EnterEM4();
}
#endif

#if defined(_SILICON_LABS_32B_SERIES_2)
/***************************************************************************//**
 * Enter EM4 with BURTC running on a ULFRCO.
 *
 * Enter EM4H Hibernate Mode with BURTC running on a 1kHz ULFRCO. Only 128b RAM
 * is retained in this mode.
 ******************************************************************************/
static void em_EM4_UlfrcoBURTC(void)
{
  // Make sure clocks are disabled.
  disable_clocks();

  // Select ULFRCO as the BURTC clock source.
  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_ULFRCO);

  // Setup BURTC parameters
  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;
  CMU_ClockEnable(cmuClock_BURTC, true);
  BURTC_Init(&burtcInit);

  // Enter EM4H.
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  em4Init.em4State = emuEM4Hibernate;
  em4Init.pinRetentionMode = emuPinRetentionLatch;
  EMU_EM4Init(&em4Init);
  EMU_EnterEM4();
}
#endif // _SILICON_LABS_32B_SERIES_2

#if defined(CRYOTIMER_PRESENT)
/***************************************************************************//**
 * Enter EM4H with CRYOTIMER running on a ULFRCO.
 *
 * Enter EM4H Hibernate Mode with CRYOTIMER running on a 1kHz ULFRCO. Only 128b
 * RAM is retained in this mode.
 ******************************************************************************/
static void em_EM4H_UlfrcoCRYO(void)
{
  // Make sure clocks are disabled.
  disable_clocks();

  // ULFRCO is always running from EM0-EM4H.

  // Setup CRYOTIMER parameters
  CRYOTIMER_Init_TypeDef cryotimerInit = CRYOTIMER_INIT_DEFAULT;
  cryotimerInit.osc = cryotimerOscULFRCO;
  cryotimerInit.presc = cryotimerPresc_128;
  cryotimerInit.period = cryotimerPeriod_256;

  // Initialize CRYOTIMER. Configure CRYOTIMER to run from ULFRCO.
  CRYOTIMER_Init(&cryotimerInit);

  // Make sure unwanted oscillators are disabled specifically for EM4H and ULFRCO.
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);

  // EM4H retains 128 byte RAM through RTCC by default.

  // Enter EM4H.
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  em4Init.em4State = emuEM4Hibernate;
  em4Init.pinRetentionMode = emuPinRetentionLatch;
  EMU_EM4Init(&em4Init);
  EMU_EnterEM4();
}
#endif // CRYOTIMER_PRESENT

#if !defined(_SILICON_LABS_32B_SERIES_0)
/***************************************************************************//**
 * Enter EM4H without RTCC or CRYOTIMER.
 *
 * Enter EM4H Hibernate Mode with no RTCC or CRYOTIMER running. Only 128b RAM is
 * retained in this mode.
 ******************************************************************************/
static void em_EM4_none(void)
{
  // Make sure clocks are disabled.
  disable_clocks();

  // Make sure unwanted oscillators are disabled specifically for EM4.
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);

  // EM4H retains 128 byte RAM through RTCC by default.
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
#if defined(_SILICON_LABS_32B_SERIES_1)
  em4Init.em4State = emuEM4Hibernate;
#endif
  //Gpio pins must be retained to avoid kit power issues - applies for kit v8.
  em4Init.pinRetentionMode = emuPinRetentionLatch;
  EMU_EM4Init(&em4Init);
  // Enter EM4.
  EMU_EnterEM4();
}
#endif // ! _SILICON_LABS_32B_SERIES_0

#if defined(_SILICON_LABS_32B_SERIES_1)
/***************************************************************************//**
 * Enter EM4S without RTCC or CRYOTIMER.
 *
 * Enter EM4S Shutoff Mode with no RTCC or CRYOTIMER running. No RAM is retained
 * in this mode.
 ******************************************************************************/
static void em_EM4S(void)
{
  // Make sure clocks are disabled.
  disable_clocks();

  // Make sure unwanted oscillators are disabled specifically for EM4S and ULFRCO.
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);

  // No RTCC or RAM retention in EM4S by default.

  // Enter EM4S.
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  em4Init.em4State = emuEM4Shutoff;
  //Gpio pins must be retained to avoid kit power issues - applies for kit v8.
  em4Init.pinRetentionMode = emuPinRetentionLatch;
  EMU_EM4Init(&em4Init);
  EMU_EnterEM4();
}
#endif  // ! _SILICON_LABS_32B_SERIES_0

/***************************************************************************//**
 * Disable all high frequency clocks
 ******************************************************************************/
#if defined(_SILICON_LABS_32B_SERIES_2)
static void disable_HF_clocks(void)
{
  // Disable high frequency peripherals
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
  USART0->EN_CLR = 0x1;
  USART1->EN_CLR = 0x1;
  USART2->EN_CLR = 0x1;
  TIMER0->EN_CLR = 0x1;
  TIMER1->EN_CLR = 0x1;
  TIMER2->EN_CLR = 0x1;
  TIMER3->EN_CLR = 0x1;
#if defined(ACMP0)
  ACMP0->EN_CLR = 0x1;
#endif // (ACMP0)
#if defined(ACMP1)
  ACMP1->EN_CLR = 0x1;
#endif // (ACMP1)
#if defined(IADC0)
  IADC0->EN_CLR = 0x1;
#endif // (IADC0)
#if defined(I2C0)
  I2C0->EN_CLR = 0x1;
#endif // (I2C0)
#if defined(I2C1)
  I2C1->EN_CLR = 0x1;
#endif // (I2C1)
  GPCRC->EN_CLR = 0x1;
#else
  CMU->CLKEN0_SET = CMU_CLKEN0_HFRCO0;
#endif
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_FSRCO);
#if (_SILICON_LABS_32B_SERIES_2_CONFIG <= 2)
  // Check that HFRCODPLL and HFXO are not requested
  while (((HFRCO0->STATUS & _HFRCO_STATUS_ENS_MASK) != 0U)
         || ((HFXO0->STATUS & _HFXO_STATUS_ENS_MASK) != 0U)) {
  }
#endif
#if (_SILICON_LABS_32B_SERIES_2_CONFIG > 1)
#if defined(USART_PRESENT)
  CMU_ClockEnable(cmuClock_USART0, false);
#endif
  CMU_ClockEnable(cmuClock_PRS, false);
  CMU_ClockEnable(cmuClock_GPIO, false);
  CMU_ClockEnable(cmuClock_HFXO, false);
  CMU_ClockEnable(cmuClock_DPLL0, false);
  CMU_ClockEnable(cmuClock_HFRCO0, false);
  CMU_ClockEnable(cmuClock_MSC, false);
  CMU_ClockEnable(cmuClock_DCDC, false);
#endif
}
#elif defined(_SILICON_LABS_32B_SERIES_1)
static void disable_HF_clocks(void)
{
  // Disable High Frequency Peripheral Clocks
  CMU_ClockEnable(cmuClock_HFPER, false);
  CMU_ClockEnable(cmuClock_USART0, false);
  CMU_ClockEnable(cmuClock_USART1, false);
  CMU_ClockEnable(cmuClock_TIMER0, false);
  CMU_ClockEnable(cmuClock_TIMER1, false);
  CMU_ClockEnable(cmuClock_CRYOTIMER, false);
#if defined(ACMP_PRESENT)
  CMU_ClockEnable(cmuClock_ACMP0, false);
#if ACMP_COUNT > 1
  CMU_ClockEnable(cmuClock_ACMP1, false);
#endif
#endif
#if defined(IDAC_PRESENT)
  CMU_ClockEnable(cmuClock_IDAC0, false);
#endif
#if defined(ADC_PRESENT)
  CMU_ClockEnable(cmuClock_ADC0, false);
#endif
  CMU_ClockEnable(cmuClock_I2C0, false);

  // Disable High Frequency Bus Clocks
#if defined(CMU_HFBUSCLKEN0_CRYPTO0)
  CMU_ClockEnable(cmuClock_CRYPTO0, false);
#endif
  CMU_ClockEnable(cmuClock_LDMA, false);
  CMU_ClockEnable(cmuClock_GPCRC, false);
  CMU_ClockEnable(cmuClock_GPIO, false);
  CMU_ClockEnable(cmuClock_HFLE, false);
  CMU_ClockEnable(cmuClock_PRS, false);
}
#else // defined(_SILICON_LABS_32B_SERIES_0)
static void disable_HF_clocks(void)
{
  // Disable High Frequency Peripheral Clocks
  CMU_ClockEnable(cmuClock_HFPER, false);
#if defined(USART_PRESENT)
  CMU_ClockEnable(cmuClock_USART0, false);
#if USART_COUNT > 1
  CMU_ClockEnable(cmuClock_USART1, false);
#endif
#endif
  CMU_ClockEnable(cmuClock_TIMER0, false);
  CMU_ClockEnable(cmuClock_TIMER1, false);
#if defined(ACMP_PRESENT)
  CMU_ClockEnable(cmuClock_ACMP0, false);
#if ACMP_COUNT > 2
  CMU_ClockEnable(cmuClock_ACMP1, false);
#endif
#endif
  CMU_ClockEnable(cmuClock_PRS, false);
#if defined(DAC_PRESENT)
  CMU_ClockEnable(cmuClock_DAC0, false);
#endif
  CMU_ClockEnable(cmuClock_GPIO, false);
  CMU_ClockEnable(cmuClock_VCMP, false);
  CMU_ClockEnable(cmuClock_ADC0, false);
  CMU_ClockEnable(cmuClock_I2C0, false);

  // Disable High Frequency Core/Bus Clocks
  CMU_ClockEnable(cmuClock_AES, false);
  CMU_ClockEnable(cmuClock_DMA, false);
  CMU_ClockEnable(cmuClock_HFLE, false);
}
#endif

/***************************************************************************//**
 * Disable all low frequency clocks
 ******************************************************************************/
#if defined(_SILICON_LABS_32B_SERIES_2)
static void disable_LF_clocks(void)
{
  // Disable low frequency peripherals
#if defined(RTCC_PRESENT)
  RTCC->EN_CLR = 0x1;
#endif
#if  defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
  WDOG0->EN_CLR = 0x1;
  WDOG1->EN_CLR = 0x1;
  LETIMER0->EN_CLR = 0x1;
  BURTC->EN_CLR = 0x1;
#else
  CMU_ClockEnable(cmuClock_LFRCO, true);
  CMU->CLKEN0_SET = CMU_CLKEN0_LFRCO;
  CMU_ClockEnable(cmuClock_LFXO, true);
  CMU->CLKEN0_SET = CMU_CLKEN0_LFXO;
#endif

  // Check that all low frequency oscillators are stopped
  while ((LFRCO->STATUS != 0U) && (LFXO->STATUS != 0U)) {
  }
#if (_SILICON_LABS_32B_SERIES_2_CONFIG > 1)
#if defined(RTCC_PRESENT)
  CMU_ClockEnable(cmuClock_RTCC, false);
#endif
  CMU_ClockEnable(cmuClock_LFRCO, false);
  CMU_ClockEnable(cmuClock_LFXO, false);
#endif
}
#elif defined(_SILICON_LABS_32B_SERIES_1)
static void disable_LF_clocks(void)
{
  // Enable LFXO for Low Frequency Clock Disables
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

  // Disable Low Frequency A Peripheral Clocks
  // Note: LFA clock must be sourced before modifying peripheral clock enables
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_LETIMER0, false);
  CMU_ClockEnable(cmuClock_PCNT0, false);
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_Disabled);

  // Disable Low Frequency B Peripheral Clocks
  // Note: LFB clock must be sourced before modifying peripheral clock enables
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_LEUART0, false);
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_Disabled);

  // Disable Low Frequency E Peripheral Clocks
  // Note: LFE clock must be sourced before modifying peripheral clock enables
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_RTCC, false);
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_Disabled);

  // Disable Low Frequency Oscillator
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
}
#else // defined(_SILICON_LABS_32B_SERIES_0)
static void disable_LF_clocks(void)
{
  // Enable LFXO for Low Frequency Clock Disables
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

  // Disable Low Frequency A Peripheral Clocks
  // Note: LFA clock must be sourced before modifying peripheral clock enables
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_RTC, false);
#if defined(LETIMER_PRESENT)
  CMU_ClockEnable(cmuClock_LETIMER0, false);
#endif
#if defined(LCD_PRESENT)
  CMU_ClockEnable(cmuClock_LCD, false);
#endif
  CMU_ClockEnable(cmuClock_PCNT0, false);
#if defined(LESENSE_PRESENT)
  CMU_ClockEnable(cmuClock_LESENSE, false);
#endif
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_Disabled);

  // Disable Low Frequency B Peripheral Clocks
  // Note: LFB clock must be sourced before modifying peripheral clock enables
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_LEUART0, false);
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_Disabled);

  // Disable Low Frequency Oscillator
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
}
#endif

/***************************************************************************//**
 * Disable all clocks to achieve lowest current consumption numbers.
 ******************************************************************************/
static void disable_clocks(void)
{
  // Disable High Frequency Clocks
  disable_HF_clocks();

  // Disable Low Frequency Clocks
  disable_LF_clocks();
}

/***************************************************************************//**
 * Calculate primes.
 ******************************************************************************/
static void prime_calc(void)
{
  uint32_t i, d, n;
  uint32_t primes[64];

  // Find prime numbers forever.
  while (1) {
    primes[0] = 1;
    for (i = 1; i < 64; ) {
      for (n = primes[i - 1] + 1;; n++) {
        for (d = 2; d <= n; d++) {
          if (n == d) {
            primes[i] = n;
            goto nexti;
          }
          if (n % d == 0) {
            break;
          }
        }
      }
      nexti:
      i++;
    }
  }
}

/***************************************************************************//**
 * Helper function to call related function which enters EM0 Active Mode.
 ******************************************************************************/
static void em_EM0(energy_mode_t *mode)
{
  switch ((em01_oscillator_enum_t)mode->osc) {
    case HFXO_:
      em_EM0_Hfxo();
      break;
#if defined(_SILICON_LABS_32B_SERIES_2)
    case FSRCO_20MHZ:
      em_EM0_Fsrco();
      break;
    case HFRCO_80MHZ:
      em_EM0_Hfrco(cmuHFRCODPLLFreq_80M0Hz);
      break;
    case HFRCO_38MHZ:
      em_EM0_Hfrco(cmuHFRCODPLLFreq_38M0Hz);
      break;
    case HFRCO_26MHZ:
      em_EM0_Hfrco(cmuHFRCODPLLFreq_26M0Hz);
      break;
    case HFRCO_1MHZ:
      em_EM0_Hfrco(cmuHFRCODPLLFreq_1M0Hz);
      break;
#elif defined(_SILICON_LABS_32B_SERIES_1)
#if defined(_DEVINFO_HFRCOCAL16_MASK)
    case HFRCO_72MHZ:
      em_EM0_Hfrco(cmuHFRCOFreq_72M0Hz);
      break;
#endif
#if defined(_DEVINFO_HFRCOCAL15_MASK)
    case HFRCO_64MHZ:
      em_EM0_Hfrco(cmuHFRCOFreq_64M0Hz);
      break;
#endif
#if defined(_DEVINFO_HFRCOCAL14_MASK)
    case HFRCO_56MHZ:
      em_EM0_Hfrco(cmuHFRCOFreq_56M0Hz);
      break;
#endif
#if defined(_DEVINFO_HFRCOCAL13_MASK)
    case HFRCO_48MHZ:
      em_EM0_Hfrco(cmuHFRCOFreq_48M0Hz);
      break;
#endif
    case HFRCO_38MHZ:
      em_EM0_Hfrco(cmuHFRCOFreq_38M0Hz);
      break;
    case HFRCO_26MHZ:
      em_EM0_Hfrco(cmuHFRCOFreq_26M0Hz);
      break;
    case HFRCO_19MHZ:
      em_EM0_Hfrco(cmuHFRCOFreq_19M0Hz);
      break;
    case HFRCO_4MHZ:
      em_EM0_Hfrco(cmuHFRCOFreq_4M0Hz);
      break;
    case HFRCO_1MHZ:
      em_EM0_Hfrco(cmuHFRCOFreq_1M0Hz);
      break;
#else
#if defined(CMU_HFRCOCTRL_BAND_28MHZ)
    case HFRCO_28MHZ:
      em_EM0_Hfrco(cmuHFRCOBand_28MHz);
      break;
#endif
    case HFRCO_21MHZ:
      em_EM0_Hfrco(cmuHFRCOBand_21MHz);
      break;
    case HFRCO_14MHZ:
      em_EM0_Hfrco(cmuHFRCOBand_14MHz);
      break;
    case HFRCO_11MHZ:
      em_EM0_Hfrco(cmuHFRCOBand_11MHz);
      break;
    case HFRCO_7MHZ:
      em_EM0_Hfrco(cmuHFRCOBand_7MHz);
      break;
    case HFRCO_1MHZ:
      em_EM0_Hfrco(cmuHFRCOBand_1MHz);
      break;
#endif
    default:
      EFM_ASSERT(false);
      break;
  }
  switch (mode->op) {
    case WHILE:
      while (1) {
      }
    // no break
    case PRIME:
      prime_calc();
      break;
    case COREMARK:
#if !defined(_SILICON_LABS_32B_SERIES_2)
      CMU_ClockEnable(cmuClock_HFPER, true); // enable timer for CoreMark
#endif
      while (1) {
        CoreMark_Main();
      }
    // no break
    default:
      EFM_ASSERT(false);
      break;
  }
}

/***************************************************************************//**
 * Helper function to call related function which enters EM1 Sleep Mode.
 ******************************************************************************/
static void em_EM1(energy_mode_t *mode)
{
  switch ((em01_oscillator_enum_t)mode->osc) {
    case HFXO_:
      em_EM1_Hfxo();
      break;
#if defined(_SILICON_LABS_32B_SERIES_2)
    case FSRCO_20MHZ:
      em_EM1_Fsrco();
      break;
    case HFRCO_80MHZ:
      em_EM1_Hfrco(cmuHFRCODPLLFreq_80M0Hz);
      break;
    case HFRCO_38MHZ:
      em_EM1_Hfrco(cmuHFRCODPLLFreq_38M0Hz);
      break;
    case HFRCO_26MHZ:
      em_EM1_Hfrco(cmuHFRCODPLLFreq_26M0Hz);
      break;
    case HFRCO_1MHZ:
      em_EM1_Hfrco(cmuHFRCODPLLFreq_1M0Hz);
      break;
#elif defined(_SILICON_LABS_32B_SERIES_0)
#if defined(CMU_HFRCOCTRL_BAND_28MHZ)
    case HFRCO_28MHZ:
      em_EM1_Hfrco(cmuHFRCOBand_28MHz);
      break;
#endif
    case HFRCO_21MHZ:
      em_EM1_Hfrco(cmuHFRCOBand_21MHz);
      break;
    case HFRCO_14MHZ:
      em_EM1_Hfrco(cmuHFRCOBand_14MHz);
      break;
    case HFRCO_11MHZ:
      em_EM1_Hfrco(cmuHFRCOBand_11MHz);
      break;
    case HFRCO_7MHZ:
      em_EM1_Hfrco(cmuHFRCOBand_7MHz);
      break;
    case HFRCO_1MHZ:
      em_EM1_Hfrco(cmuHFRCOBand_1MHz);
      break;
#else
#if defined(_DEVINFO_HFRCOCAL16_MASK)
    case HFRCO_72MHZ:
      em_EM1_Hfrco(cmuHFRCOFreq_72M0Hz);
      break;
#endif
#if defined(_DEVINFO_HFRCOCAL15_MASK)
    case HFRCO_64MHZ:
      em_EM1_Hfrco(cmuHFRCOFreq_64M0Hz);
      break;
#endif
#if defined(_DEVINFO_HFRCOCAL14_MASK)
    case HFRCO_56MHZ:
      em_EM1_Hfrco(cmuHFRCOFreq_56M0Hz);
      break;
#endif
#if defined(_DEVINFO_HFRCOCAL13_MASK)
    case HFRCO_48MHZ:
      em_EM1_Hfrco(cmuHFRCOFreq_48M0Hz);
      break;
#endif
    case HFRCO_38MHZ:
      em_EM1_Hfrco(cmuHFRCOFreq_38M0Hz);
      break;
    case HFRCO_26MHZ:
      em_EM1_Hfrco(cmuHFRCOFreq_26M0Hz);
      break;
    case HFRCO_19MHZ:
      em_EM1_Hfrco(cmuHFRCOFreq_19M0Hz);
      break;
    case HFRCO_4MHZ:
      em_EM1_Hfrco(cmuHFRCOFreq_4M0Hz);
      break;
    case HFRCO_1MHZ:
      em_EM1_Hfrco(cmuHFRCOFreq_1M0Hz);
      break;
#endif
    default:
      EFM_ASSERT(false);
      break;
  }
}

/***************************************************************************//**
 * Helper function to call related function which enters EM2 Deep Sleep Mode.
 ******************************************************************************/
static void em_EM2(energy_mode_t *mode)
{
  switch ((em2_oscillator_enum_t)mode->osc) {
#if defined(RTCC_PRESENT)
    case EM2_LFXO_RTCC:
      // Full RAM
      em_EM2_RTCC(cmuSelect_LFXO, false);   // disable RAM powerdown
      break;
    case EM2_LFXO_RTCC_RAM_POWERDOWN:
      // 16kB RAM
      em_EM2_RTCC(cmuSelect_LFXO, true);    // enable RAM powerdown
      break;
    case EM2_LFRCO_RTCC:
      // Full RAM
      em_EM2_RTCC(cmuSelect_LFRCO, false);  // disable RAM powerdown
      break;
    case EM2_LFRCO_RTCC_RAM_POWERDOWN:
      // 16kB RAM
      em_EM2_RTCC(cmuSelect_LFRCO, true);   // enable RAM powerdown
      break;
#endif
#if defined(RTC_PRESENT)
    case EM2_LFRCO_RTC:
      // Full RAM
      em_EM2_LfrcoRTC(false);               // disable RAM powerdown
      break;
    case EM2_LFRCO_RTC_RAM_POWERDOWN:
      // 16kB RAM
      em_EM2_LfrcoRTC(true);                // enable RAM powerdown
      break;
#elif defined(SYSRTC_PRESENT)
    case EM2_LFRCO_SYSRTC:
      em_EM2_LfrcoSYSRTC(false);            // disable RAM powerdown
      break;
    case EM2_LFRCO_SYSRTC_RAM_POWERDOWN:
      em_EM2_LfrcoSYSRTC(true);             // enable RAM powerdown
      break;
#if !(defined(ZGM230SB27HGN) && ZGM230SB27HGN == 1)
    case EM2_LFXO_SYSRTC:
      em_EM2_LfxoSYSRTC(false);             // disable RAM powerdown
      break;
    case EM2_LFXO_SYSRTC_RAM_POWERDOWN:
      em_EM2_LfxoSYSRTC(true);              // enable RAM powerdown
      break;
#endif
#endif
    default:
      EFM_ASSERT(false);
      break;
  }
}

/***************************************************************************//**
 * Helper function to call related function which enters EM3 Stop Mode.
 ******************************************************************************/
static void em_EM3(energy_mode_t *mode)
{
  switch ((em3_oscillator_enum_t)mode->osc) {
    case EM3_ULFRCO:
      // High and low frequency clocks are disabled in EM3
      // All unwanted oscillators are disabled in EM3
#if defined(_SILICON_LABS_32B_SERIES_2)
      disable_clocks();
#endif
      // Enter EM3
      EMU_EnterEM3(false);
      break;
    case EM3_ULFRCO_RAM_POWERDOWN:
      // High and low frequency clocks are disabled in EM3
      // All unwanted oscillators are disabled in EM3
#if defined(_SILICON_LABS_32B_SERIES_2)
      disable_clocks();
#endif
      EMU_RamPowerDown(SRAM_BASE, 0); // Power down all RAM blocks except block 1
      // Enter EM3
      EMU_EnterEM3(false);
      break;
#if defined(_SILICON_LABS_32B_SERIES_2)
    case EM3_ULFRCO_BURTC:
      em_EM3_UlfrcoBURTC(false);            // disable RAM powerdown
      break;
    case EM3_ULFRCO_BURTC_RAM_POWERDOWN:
      em_EM3_UlfrcoBURTC(true);             // enable RAM powerdown
      break;
#endif
#if defined(CRYOTIMER_PRESENT)
    case EM3_ULFRCO_CRYO:
      // Full RAM
      em_EM3_UlfrcoCRYO(false);             // disable RAM powerdown
      break;
    case EM3_ULFRCO_CRYO_RAM_POWERDOWN:
      em_EM3_UlfrcoCRYO(true);              // enable RAM powerdown
      break;
#endif
    default:
      EFM_ASSERT(false);
      break;
  }
}

#if !defined(_SILICON_LABS_32B_SERIES_0)
/***************************************************************************//**
 * Helper function to call related function which enters EM4H Hibernate Mode.
 ******************************************************************************/
static void em_EM4(energy_mode_t *mode)
{
  switch ((em4h_oscillator_enum_t)mode->osc) {
    case NONE:
      // 128b RAM
      em_EM4_none();
      break;
#if defined(_SILICON_LABS_32B_SERIES_2)
    case EM4_LFRCO_BURTC:
      em_EM4_LfrcoBURTC();
      break;
    case EM4_ULFRCO_BURTC:
      em_EM4_UlfrcoBURTC();
      break;
#else
    case EM4H_LFXO_RTCC:
      // 128b RAM
      em_EM4H_LfxoRTCC();
      break;
#if defined(CRYOTIMER_PRESENT)
    case EM4H_ULFRCO_CRYO:
      // 128b RAM
      em_EM4H_UlfrcoCRYO();
      break;
#endif
#endif
    default:
      EFM_ASSERT(false);
      break;
  }
}
#endif // ! _SILICON_LABS_32B_SERIES_0

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize Emodes.
 ******************************************************************************/
void em_init(void)
{
#if !defined(_SILICON_LABS_32B_SERIES_0)
#if defined(EMU_VSCALE_EM01_PRESENT)
  /* Use default settings for energy modes */
  /* Enable voltage downscaling in EM modes. */
  EMU_EM01Init_TypeDef em01Init = EMU_EM01INIT_DEFAULT;
  em01Init.vScaleEM01LowPowerVoltageEnable = true;
  EMU_EM01Init(&em01Init);
#endif
#if defined(EMU_VSCALE_PRESENT)
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;
  EMU_EM23Init(&em23Init);
#endif
#if defined(_EMU_CTRL_EM4HVSCALE_MASK)
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  em4Init.vScaleEM4HVoltage = emuVScaleEM4H_LowPower;
  EMU_EM4Init(&em4Init);
#endif
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_MASK)
  /* Disable peripheral retention. */
  EMU_PeripheralRetention(emuPeripheralRetention_ALL, false);
#endif
}

/***************************************************************************//**
 * Function to start the Emode test.
 ******************************************************************************/
void start_emode_test(energy_mode_t *mode)
{
#if defined(DCDC_PRESENT)
  if (!mode->dcdc) {
    EMU_DCDCModeSet(emuDcdcMode_Bypass);
  }
#if defined(_EMU_DCDCLPEM01CFG_MASK)
  else if (mode->em == 1) {
    EMU_DCDCModeSet(emuDcdcMode_LowPower);
  }
#endif
#endif
  switch (mode->em) {
    case EM0:
      em_EM0(mode);
      break;
    case EM1:
      em_EM1(mode);
      break;
    case EM2:
      em_EM2(mode);
      break;
    case EM3:
      em_EM3(mode);
      break;
#if !defined(_SILICON_LABS_32B_SERIES_1)
    case EM4:
#if defined(_SILICON_LABS_32B_SERIES_0)
      // Enter EM4.
      EMU_EnterEM4();
#else
      em_EM4(mode);
#endif
      break;
#else
    case EM4H:
      em_EM4(mode);
      break;
    case EM4S:
      // No RAM
      em_EM4S();
      break;
#endif
    default:
      EFM_ASSERT(false);
      break;
  }
}
