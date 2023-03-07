/***************************************************************************//**
 * @file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "em_device.h"
#include "em_chip.h"

#include "cslib_hwconfig.h"
#include "cslib_config.h"
#include "cslib.h"
#include "hardware_routines.h"
#include "low_power_config.h"
#include "em_emu.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_csen.h"

uint8_t timerTick;
extern uint8_t CSENtimerTick;
CSEN_SingleSel_TypeDef CSLIB_findAPortForIndex(uint8_t, uint32_t, uint32_t);
void setupCSLIBClock(uint32_t, CSEN_Init_TypeDef*);

// Copied into a variable so noise mitigation algorithm can change TRST
CSEN_InitMode_TypeDef sleep_mode_default = CSEN_SLEEPMODE_DEFAULT;

/**************************************************************************//**
 * Configure sensor for sleep mode
 *
 * Configures sensor peripheral(s) for sleep mode scanning
 *
 *****************************************************************************/
void CSLIB_configureSensorForSleepModeCB(void)
{
  CSEN_Init_TypeDef csen_init = CSEN_INIT_DEFAULT;

  CSEN_InitMode_TypeDef sleep_mode = sleep_mode_default;

  setupCSLIBClock(DEF_SLEEP_MODE_PERIOD, &csen_init);

  // Set converter and scan configuration
  CMU_ClockEnable(cmuClock_CSEN_HF, true);
  CSEN_Init(CSEN, &csen_init);

  CSEN_InitMode(CSEN, &sleep_mode);

  CMU_ClockEnable(cmuClock_LDMA, true);

  CSEN_Enable(CSEN);
}

void InitSleepModeBaseline(void)
{
  uint32_t value;
  CSEN_InitMode_TypeDef sleep_mode = sleep_mode_default;

  // Determine the new baseline based on the current SAR measurements
  sleep_mode.convSel = csenConvSelSAR;
  sleep_mode.accMode = csenAccMode64;
  sleep_mode.sumOnly = 0;
  sleep_mode.trigSel = csenTrigSelStart;

  CSEN_Disable(CSEN);
  CSEN->IFC = CSEN->IF;
  CSEN_InitMode(CSEN, &sleep_mode);

  CSEN_Enable(CSEN);

  value = executeConversion();
  CSEN->DMBASELINE = value;

  CSEN->EMA = value * 8;

  CSEN->IFC = CSEN->IF;

  CSEN_IntEnable(CSEN, CSEN_IEN_CMP);
  NVIC_ClearPendingIRQ(CSEN_IRQn);
  NVIC_EnableIRQ(CSEN_IRQn);
  CSLIB_configureSensorForSleepModeCB();
}

/**************************************************************************//**
 * Configure to sleep mode
 *
 * Re-enable and get system ready for active mode
 *
 *****************************************************************************/
void CSLIB_enterLowPowerStateCB(void)
{
  EMU_EnterEM2(true);
}

/**************************************************************************//**
 * Configure timer for active mode
 *
 * This is a top-level call to configure the timer to active mode, one of the
 * two defined modes of operation in the system.  This instance of the function
 * configures the SmaRTClock to the frequency defined in cslib_config.h.
 *
 *****************************************************************************/
void CSLIB_configureTimerForActiveModeCB(void)
{
  // For CSEN, active mode timing is handled through the CSEN timer
  // and so it is configured in the configureSensor routine
}

/**************************************************************************//**
 * Configure timer for sleep mode
 *
 * This is a top-level call to configure the timer to sleep mode, one of the
 * two defined modes of operation in the system.  This instance of the function
 * configures the SmaRTClock to the frequency defined in cslib_config.h.
 *
 *****************************************************************************/
void CSLIB_configureTimerForSleepModeCB(void)
{
  // For CSEN, sleep mode timing is handled through the CSEN timer
  // and so it is configured in the configureSensor routine
}

/**************************************************************************//**
 * Check timer
 *
 *****************************************************************************/
void CSLIB_checkTimerCB(void)
{
  if (CSENtimerTick == 1) {
    CSENtimerTick = 0;
    timerTick = 1;
  }
}
