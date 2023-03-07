/***************************************************************************//**
 * @file
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "cslib_hwconfig.h"
#include "cslib_config.h"
#include "low_power_config.h"
#include "cslib.h"
#include "hardware_routines.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_csen.h"
#include "dmadrv.h"

/// Configures LDMA channels to set BASELINE values and read CSEN data
static unsigned int DMAChanA;
static unsigned int DMAChanB;

static volatile bool DMATransferInProgress = false;

/// Set LDMA transfer ISR, checked/cleared by CSLIB timing callback checkTimer()
uint8_t CSENtimerTick = 0;

/// Maps aport settings to bits of CSEN active mode bit array
static const CSEN_SingleSel_TypeDef CSLIB_pinMapping[64] = {
  csenSingleSelAPORT1XCH0,
  csenSingleSelAPORT1YCH1,
  csenSingleSelAPORT1XCH2,
  csenSingleSelAPORT1YCH3,
  csenSingleSelAPORT1XCH4,
  csenSingleSelAPORT1YCH5,
  csenSingleSelAPORT1XCH6,
  csenSingleSelAPORT1YCH7,
  csenSingleSelAPORT1XCH8,
  csenSingleSelAPORT1YCH9,
  csenSingleSelAPORT1XCH10,
  csenSingleSelAPORT1YCH11,
  csenSingleSelAPORT1XCH12,
  csenSingleSelAPORT1YCH13,
  csenSingleSelAPORT1XCH14,
  csenSingleSelAPORT1YCH15,
  csenSingleSelAPORT1XCH16,
  csenSingleSelAPORT1YCH17,
  csenSingleSelAPORT1XCH18,
  csenSingleSelAPORT1YCH19,
  csenSingleSelAPORT1XCH20,
  csenSingleSelAPORT1YCH21,
  csenSingleSelAPORT1XCH22,
  csenSingleSelAPORT1YCH23,
  csenSingleSelAPORT1XCH24,
  csenSingleSelAPORT1YCH25,
  csenSingleSelAPORT1XCH26,
  csenSingleSelAPORT1YCH27,
  csenSingleSelAPORT1XCH28,
  csenSingleSelAPORT1YCH29,
  csenSingleSelAPORT1XCH30,
  csenSingleSelAPORT1YCH31,
  csenSingleSelAPORT3XCH0,
  csenSingleSelAPORT3YCH1,
  csenSingleSelAPORT3XCH2,
  csenSingleSelAPORT3YCH3,
  csenSingleSelAPORT3XCH4,
  csenSingleSelAPORT3YCH5,
  csenSingleSelAPORT3XCH6,
  csenSingleSelAPORT3YCH7,
  csenSingleSelAPORT3XCH8,
  csenSingleSelAPORT3YCH9,
  csenSingleSelAPORT3XCH10,
  csenSingleSelAPORT3YCH11,
  csenSingleSelAPORT3XCH12,
  csenSingleSelAPORT3YCH13,
  csenSingleSelAPORT3XCH14,
  csenSingleSelAPORT3YCH15,
  csenSingleSelAPORT3XCH16,
  csenSingleSelAPORT3YCH17,
  csenSingleSelAPORT3XCH18,
  csenSingleSelAPORT3YCH19,
  csenSingleSelAPORT3XCH20,
  csenSingleSelAPORT3YCH21,
  csenSingleSelAPORT3XCH22,
  csenSingleSelAPORT3YCH23,
  csenSingleSelAPORT3XCH24,
  csenSingleSelAPORT3YCH25,
  csenSingleSelAPORT3XCH26,
  csenSingleSelAPORT3YCH27,
  csenSingleSelAPORT3XCH28,
  csenSingleSelAPORT3YCH29,
  csenSingleSelAPORT3XCH30,
  csenSingleSelAPORT3YCH31
};

// Configures whether sleep mode scan uses DMA transfers or single conversions
// for capacitive sensing output
uint16_t CSLIB_autoScanComplete = 0;
uint16_t CSLIB_autoScan;

// Temporarily saves sensor data before being pushed into CSLIB_node struct
volatile uint32_t autoScanBuffer[DEF_NUM_SENSORS];
// Buffer passed back to CSLIB to copy into CSLIB_node struct
volatile uint32_t CSLIB_autoScanBuffer[DEF_NUM_SENSORS];
volatile uint32_t autoBaselineBuffer[DEF_NUM_SENSORS];

CSEN_InitMode_TypeDef active_mode_default = CSEN_ACTIVEMODE_DEFAULT;
extern CSEN_InitMode_TypeDef sleep_mode_default;

uint8_t indexTRST = 0;             // index to current TRST setting

// Function located in cslib library, need to add access for noise
// mitigation algorithm
void CSLIB_initNoiseCharacterization(void);

static void configureCSENActiveMode(void);

// DMADRV transfer callback
static bool transferCallback(unsigned int channel,
                             unsigned int sequenceNo,
                             void *userParam);

/**************************************************************************//**
 * CSEN pin mapping function
 *
 * This function maps CSLIB_node[] indexes to Aport CSEN_SingleSel_TypeDef
 * values by stepping through the CSEN input masks, counting enabled inputs,
 * and finding the input that corresponds to the CSLIB_node index.  The
 * function then returns the corresponding CSEN_SingleSel_TypeDef value from
 * CSLIB_pinMapping.
 * Note that CSLIB_pinMapping is only valid for default
 * em_csen.h CSEN_InputSel_TypeDef values.
 *
 *****************************************************************************/
static CSEN_SingleSel_TypeDef CSLIB_findAPortForIndex(uint8_t mux_index, uint32_t Aport0, uint32_t Aport1)
{
  uint32_t cslib_index, index, count;
  count = 0;
  cslib_index = CSLIB_muxValues[mux_index];
  for (index = 0; index < 32; index++) {
    if (Aport0 & (0x0001 << index)) {
      if (count == cslib_index) {
        return CSLIB_pinMapping[index];
      } else {
        count = count + 1;
      }
    }
  }

  for (index = 0; index < 32; index++) {
    if (Aport1 & (0x0001 << index)) {
      if (count == cslib_index) {
        return CSLIB_pinMapping[32 + index];
      } else {
        count = count + 1;
      }
    }
  }
  return csenSingleSelDefault;
}

/**************************************************************************//**
 * scanSensor callback
 *
 * Configures CSEN for a single conversion with no DMA.  In this implementation,
 * scanSensor is not used during operation because the system uses scan+DMA
 * transfers instead.  In this implementation, scanSensor is only called
 * in baseline initialization and so SAR mode is used.
 *
 *****************************************************************************/
uint32_t CSLIB_scanSensorCB(uint8_t index)
{
  uint32_t return_value;
  CSEN_Init_TypeDef csen_init = CSEN_INIT_DEFAULT;
  CSEN_InitMode_TypeDef active_mode_single = active_mode_default;
  active_mode_single.singleSel = CSLIB_findAPortForIndex(index,
                                                         active_mode_single.inputMask0,
                                                         active_mode_single.inputMask1);
  // Disable DMA transfer for single conversion
  active_mode_single.enableDma = false;

  active_mode_single.convSel = csenConvSelSAR;
  active_mode_single.sampleMode = csenSampleModeSingle;

  CMU_ClockEnable(cmuClock_CSEN_HF, true);

  // Clear interrupts
  CSEN_Disable(CSEN);
  CSEN->IFC = CSEN->IF;

  // Initialize the block
  CSEN_Init(CSEN, &csen_init);
  CSEN_InitMode(CSEN, &active_mode_single);

  // Convert one sample
  CSEN_Enable(CSEN);
  return_value = executeConversion();
  CSEN_Disable(CSEN);

  autoBaselineBuffer[(index - 1 + DEF_NUM_SENSORS) % DEF_NUM_SENSORS] = return_value;
  // This is a kinda ugly place to put this, but it makes the most programmatic sense here with the rest of the Baseline code.
  CSEN->DMBASELINE = autoBaselineBuffer[DEF_NUM_SENSORS - 1];

  return return_value;
}

/**************************************************************************//**
 * Execute CSEN conversion callback
 *
 * Called after configuration of CSEN has completed in order to make
 * one conversion and return that value.
 *
 *****************************************************************************/
uint32_t executeConversion(void)
{
  CSEN_Start(CSEN);
  while (!(CSEN->IF & _CSEN_IF_CONV_MASK))
    ;
  return CSEN->DATA;
}

/**************************************************************************//**
 * Pre baseline initialization callback
 *
 * Called before a baseline for a sensor has been initialized.  In the case
 * of the CSEN implementation, no configuration is necessary.
 *
 *****************************************************************************/
void CSLIB_baselineInitEnableCB(void)
{
}

/**************************************************************************//**
 * Post baseline initialization callback
 *
 * Called after a baseline for a sensor has been initialized.
 *
 *****************************************************************************/
void CSLIB_baselineInitDisableCB(void)
{
  configureCSENActiveMode();
}

/*********************************************************
 *****************//**
 * Configure CSEN timer
 *
 * Configures self timer using LFRCO if not already configured.  Derives
 * correct self timer reload and prescale values based on user configuration.
 *
 *****************************************************************************/
void setupCSLIBClock(uint32_t clock_period, CSEN_Init_TypeDef* csen_init)
{
  uint32_t clock_freq;
  uint32_t exponent, prescaler, reload;

  if (CMU_ClockSelectGet(cmuClock_LFB) == cmuSelect_Disabled) {
    CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);
    CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
  }

  CMU_ClockEnable(cmuClock_CSEN_LF, true);

  clock_freq = CMU_ClockFreqGet(cmuClock_CSEN_LF);

  exponent = 1;
  prescaler = 0;

  // Find the first prescaler with a reload value that is one byte in size,
  // and configure the clock with those values
  do {
    reload = clock_period * clock_freq / 1000 / exponent;
    if (reload < 255) {
      csen_init->pcReload = reload;
      csen_init->pcPrescale = (CSEN_PCPrescale_TypeDef)prescaler;
      return;
    } else {
      exponent = exponent * 2;
      prescaler = prescaler + 1;
    }
  } while ((exponent < 128));

  // If we reach here, no prescale value can yield a slow enough
  // clock frequency, and so set the reload+prescale to max period
  csen_init->pcReload = 0;
  csen_init->pcPrescale = csenPCPrescaleDiv128;
}

/**************************************************************************//**
 * Set up DMA channel for CSEN data transfer
 *
 * Configures DMA channel to read CSEN words during scan.  Channel 0
 * is used to transfer DATA from a conversion result into a buffer,
 * and channel 1 is used to load DM BASELINE with a value before
 * a scan starts.
 *
 *****************************************************************************/
static void setupCSENdataDMA(void)
{
  Ecode_t ecode;

  DMATransferInProgress = true;

  ecode = DMADRV_MemoryPeripheral(DMAChanA,
                                  dmadrvPeripheralSignal_CSEN_BSLN,
                                  (void *)&CSEN->DMBASELINE,
                                  (void *)autoBaselineBuffer,
                                  true,
                                  DEF_NUM_SENSORS,
                                  dmadrvDataSize4,
                                  transferCallback,
                                  (void *)0);
  EFM_ASSERT(ecode == ECODE_EMDRV_DMADRV_OK);

  ecode = DMADRV_PeripheralMemory(DMAChanB,
                                  dmadrvPeripheralSignal_CSEN_DATA,
                                  (void *)autoScanBuffer,
                                  (void *)&CSEN->DATA,
                                  true,
                                  DEF_NUM_SENSORS,
                                  dmadrvDataSize4,
                                  transferCallback,
                                  (void *)0);

  DMATransferInProgress = false;

  EFM_ASSERT(ecode == ECODE_EMDRV_DMADRV_OK);
}

/**************************************************************************//**
 * Configure CSEN and LDMA for active mode
 *
 * Function uses contents of cslib_hwconfig.h to set up active mode scanning.
 *
 *****************************************************************************/
static void configureCSENActiveMode(void)
{
  CSEN_Init_TypeDef csen_init = CSEN_INIT_DEFAULT;
  CSEN_InitMode_TypeDef active_mode = active_mode_default;

  setupCSLIBClock(DEF_ACTIVE_MODE_PERIOD, &csen_init);

  // Set converter and scan configuration
  CMU_ClockEnable(cmuClock_CSEN_HF, true);
  CSEN_Init(CSEN, &csen_init);

  CSEN_InitMode(CSEN, &active_mode);

  CMU_ClockEnable(cmuClock_LDMA, true);

  setupCSENdataDMA();

  // Start a conversion
  CSEN_Enable(CSEN);
}

/**************************************************************************//**
 * Change TRST value based on noise estimation
 *
 * When system-wide noise is above threshold for more than TRST_DELAY LDMA
 * interrupts, switch to a new TRST setting and reset noise value.
 *
 *****************************************************************************/
static void CSLIB_TRSTSwitchIfHighNoise(void)
{
  static uint8_t delayCountTRSTChange = 0;  // number of scans with noise above cutoff

  if ( CSLIB_systemNoiseAverage > TRST_NOISE_THRESHOLD ) {
    // Increment counter
    delayCountTRSTChange++;
    // If counter above change threshold
    if ( delayCountTRSTChange > TRST_DELAY ) {
      // Reset counter
      delayCountTRSTChange = 0;
      // Swap to new TRST setting
      indexTRST++;
      indexTRST = indexTRST % TRST_ARRAY_SIZE;
      sleep_mode_default.resetPhase = CSLIB_TRST[indexTRST];
      active_mode_default.resetPhase = CSLIB_TRST[indexTRST];

      if (TRST_NOISE_EST_RESET) {
        CSLIB_initNoiseCharacterization();
        noise_level = mid;
      }
    }
  } else {
    // Noise is below cutoff, reset counter
    delayCountTRSTChange = 0;
  }
}

/**************************************************************************//**
 * LDMA interrupt handler
 *
 * The LDMA is used to signal that a CSEN scan has completed and
 * and that the LDMA has copied all scan results.  The ISR sets up the
 * LDMA for the next transfer and sets flags for timing and sample processing.
 *
 *****************************************************************************/
static bool transferCallback(unsigned int channel,
                             unsigned int sequenceNo,
                             void *userParam)
{
  (void)channel;
  (void)sequenceNo;
  (void)userParam;
  uint32_t index;
  bool channelADone;
  bool channelBDone;

  if ( (CSEN->CTRL & CSEN_CTRL_CONVSEL_DM) == CSEN_CTRL_CONVSEL_DM) {
    /* Setup DMA for next transfer, common for single and scan mode */
    if (!DMATransferInProgress) {
      EFM_ASSERT(DMADRV_TransferDone(DMAChanA, &channelADone) == ECODE_EMDRV_DMADRV_OK);
      EFM_ASSERT(DMADRV_TransferDone(DMAChanB, &channelBDone) == ECODE_EMDRV_DMADRV_OK);
      if (channelADone && channelBDone) {
        for ( index = 0; index < DEF_NUM_SENSORS; index++ ) {
          // Only use the updated point if it is greater than  (library baseline - DELTA_CUTOFF)
          // This step must be done here because the sensor baseline must not be polluted with bad points.
          if ( autoScanBuffer[CSLIB_muxValues[index]] > (CSLIB_node[index].currentBaseline - DELTA_CUTOFF)) {
            CSLIB_autoScanBuffer[index] = autoScanBuffer[CSLIB_muxValues[index]];
            autoBaselineBuffer[(index - 1 + DEF_NUM_SENSORS) % DEF_NUM_SENSORS] = autoScanBuffer[index];
          }
        }
        CSEN->DMBASELINE = autoBaselineBuffer[DEF_NUM_SENSORS - 1];
        setupCSENdataDMA();
        CSLIB_autoScanComplete = 1;
        CSENtimerTick = 1;
      }
    }
  } else {
    /* Setup DMA for next transfer, common for single and scan mode */
    for ( index = 0; index < DEF_NUM_SENSORS; index++ ) {
      CSLIB_autoScanBuffer[index] = autoScanBuffer[CSLIB_muxValues[index]];
      autoBaselineBuffer[(index - 1 + DEF_NUM_SENSORS) % DEF_NUM_SENSORS] = autoScanBuffer[index];
    }
    setupCSENdataDMA();
    CSLIB_autoScanComplete = 1;
    CSENtimerTick = 1;
  }

  CSLIB_TRSTSwitchIfHighNoise();
  // Notify comms we have new data and it should update
  sendComms = true;
  return true;
}

/**************************************************************************//**
 * CSEN interrupt service routine
 *
 * This ISR is only entered when exiting EM2.  ISR should clear
 * and disable CSEN interrupts before exiting.
 * Note that this function could be used to signal application
 * layer firmware that a candidate touch event has happened.
 *
 *****************************************************************************/
void CSEN_IRQHandler(void)
{
  CSEN->IFC = CSEN->IF;                // Clear interrupt
  NVIC_DisableIRQ(CSEN_IRQn);
  NVIC_ClearPendingIRQ(CSEN_IRQn);

  // Sensor 0 was used for sleep mode scanning, need to reload its
  // HW baseline.
  CSLIB_scanSensorCB(0);
}

/**************************************************************************//**
 * Callback for active mode scan configuration
 *
 * This is a top-level call to configure the sensor to its operational state
 * during active mode.
 *
 *****************************************************************************/
void CSLIB_configureSensorForActiveModeCB(void)
{
  Ecode_t ecode;

  ecode = DMADRV_Init();
  if (ecode == ECODE_EMDRV_DMADRV_OK) {
    ecode = DMADRV_AllocateChannel(&DMAChanA, (void *)0);
    EFM_ASSERT(ecode == ECODE_EMDRV_DMADRV_OK);

    ecode = DMADRV_AllocateChannel(&DMAChanB, (void *)0);
    EFM_ASSERT(ecode == ECODE_EMDRV_DMADRV_OK);
  }
  configureCSENActiveMode();           // Initialize CSEN and LDMA
  CSLIB_autoScan = 1;                  // Signal to CSLIB to use auto scan state machine
}
