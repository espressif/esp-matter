/***************************************************************************//**
 * @file
 * @brief Helper functions for capacitive touch using CSEN
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stdbool.h>
#include "em_cmu.h"
#include "em_csen.h"

#include "app_csen.h"
#include "app_rtcc.h"

static uint32_t pad_capacitance[NUM_OF_PAD] = { 0, 0 };
static uint32_t pad_mins[NUM_OF_PAD] = { 60000, 60000 };
static uint32_t pad_level[NUM_OF_PAD + 2] = PAD_LEVEL_THRS;
static uint8_t currentChannel = 0;
static int8_t max_pad = -1;
static uint32_t maxval = PAD_THRS;

static CSEN_Event_t currentEvent = CSEN_EVENT_DEFAULT;

static const uint8_t pad_channels[NUM_OF_PAD] = { 0, 1 };
static const CSEN_SingleSel_TypeDef sliderInpChannels[] = { csenSingleSelAPORT1YCH13, csenSingleSelAPORT1XCH28 };

/**************************************************************************//**
 * @brief  Setup CSEN, do initial calibration and start continuous scan
 *****************************************************************************/
void setupCSEN(void)
{
  CSEN_Init_TypeDef csenInit = CSEN_INIT_DEFAULT;
  CSEN_InitMode_TypeDef csenMeasureModeInit = CSEN_INITMODE_DEFAULT;
  uint8_t i;

  /* Use LFXO as LF clock source since we are already using it
     for the RTCC */
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);

  CMU_ClockEnable(cmuClock_CSEN_HF, true);
  CMU_ClockEnable(cmuClock_CSEN_LF, true);

  /* Setup timer to do 16 CSEN scans per second
     The input to the CSEN_LF clock is by default LFB/16 */
  csenInit.pcPrescale = csenPCPrescaleDiv16;
  csenInit.pcReload = 7;

  CSEN_Init(CSEN, &csenInit);

  NVIC_ClearPendingIRQ(CSEN_IRQn);
  NVIC_EnableIRQ(CSEN_IRQn);

  // Setup measurement mode to single conversion for calibration
  csenMeasureModeInit.sampleMode = csenSampleModeSingle;
  csenMeasureModeInit.trigSel = csenTrigSelStart;
  csenMeasureModeInit.sarRes = csenSARRes16;
  csenMeasureModeInit.gainSel = csenGainSel8X;                          /* Normal reference current when measuring single pads */

  // Do an initial reading of each pad to establish the zero-level
  for (i = 0; i < NUM_OF_PAD; i++) {
    csenMeasureModeInit.singleSel = sliderInpChannels[i];
    CSEN_InitMode(CSEN, &csenMeasureModeInit);
    CSEN_Enable(CSEN);
    CSEN_Start(CSEN);
    while (CSEN_IsBusy(CSEN) || !(CSEN_IntGet(CSEN) && CSEN_IF_CONV)) ;
    CSEN_IntClear(CSEN, CSEN_IF_CONV);
    // Subtract a margin from the reading to account for read noise
    pad_mins[i] = CSEN_DataGet(CSEN) - APP_CSEN_NOISE_MARGIN;
  }

  // Setup measurement mode to timer-triggered scan
  csenMeasureModeInit.inputMask0 = (1 << 28) | (1 << 13);
  csenMeasureModeInit.sampleMode = csenSampleModeScan;
  csenMeasureModeInit.trigSel = csenTrigSelTimer;

  CSEN_InitMode(CSEN, &csenMeasureModeInit);
  CSEN_Enable(CSEN);

  CSEN_IntEnable(CSEN, CSEN_IEN_CONV);

  // Start continuous scan cycle
  currentChannel = 0;
  CSEN_Start(CSEN);
}

/**************************************************************************//**
 * @brief  Calculate slider position
 *****************************************************************************/
int32_t csenCalcPos(void)
{
  int32_t tmp_pos = -1;

  if (max_pad != -1) {
    tmp_pos = (max_pad) << 6;

    // Avoid moving calculated position to the left when we are close to the right edge
    // and the measured capacitance at the rightmost pad is close to the noise floor
    tmp_pos -= (pad_level[max_pad] << 5) / pad_level[max_pad + 1];

    // Avoid moving calculated position to the right when we are close to the left edge
    // and the measured capacitance at the leftmost pad is close to the noise floor
    tmp_pos += (pad_level[max_pad + 2] << 5) / pad_level[max_pad + 1];
  }

  return tmp_pos;
}

/**************************************************************************//**
 * @brief  Get touch event data
 *****************************************************************************/
CSEN_Event_t csenGetEvent(void)
{
  return currentEvent;
}

/**************************************************************************//**
 * @brief  Check CSEN data after a scan is completed
 *****************************************************************************/
void csenCheckScannedData(void)
{
  uint8_t i;
  uint32_t tmp_max_val = PAD_THRS;

  CSEN_Event_t tmpEvent = currentEvent;

  max_pad = -1;

  for (i = 0; i < NUM_OF_PAD; i++) {
    // Order scan results and check if any pad has exceeded the threshold defining a touch event
    pad_level[i + 1] = ((pad_capacitance[i] - pad_mins[i]) << 16) / (65535 - pad_mins[i]);
    if (pad_level[i + 1] > tmp_max_val) {
      tmp_max_val = pad_level[i + 1];
      max_pad = i;
    } else if (pad_level[i + 1] < PAD_THRS) {
      pad_level[i + 1] = PAD_THRS;
    }
  }

  maxval = tmp_max_val;

  tmpEvent.sliderPos = csenCalcPos();

  if (tmpEvent.eventActive) {
    tmpEvent.eventDuration = millis() - tmpEvent.eventStart;
  } else if (max_pad != -1) {
    tmpEvent.eventActive = true;
    tmpEvent.eventStart = millis();
    tmpEvent.sliderStartPos = tmpEvent.sliderPos;
    tmpEvent.sliderTravel = 0;
  }

  if (max_pad == -1) {
    tmpEvent.eventActive = false;
  }

  if (tmpEvent.eventActive) {
    tmpEvent.touchForce = maxval;
    tmpEvent.sliderPrevPos = tmpEvent.sliderPos;
    tmpEvent.sliderTravel = tmpEvent.sliderPos - tmpEvent.sliderStartPos;
  }

  currentEvent = tmpEvent;
}

/**************************************************************************//**
 * @brief CSEN Interrupt handler
 *****************************************************************************/
void CSEN_IRQHandler(void)
{
  uint8_t pad_number;

  CSEN->IFC = _CSEN_IFC_MASK;

  pad_number = pad_channels[currentChannel];

  pad_capacitance[pad_number] = CSEN->DATA;

  currentChannel++;

  /* If a scan is completed, do some more checking of the data since we have
     time to spare before the next scan is scheduled to start */
  if (currentChannel > 1) {
    currentChannel = 0;
    csenCheckScannedData();
  }
}
