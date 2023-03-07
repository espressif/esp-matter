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

#include "cslib_hwconfig.h"
#include "cslib_config.h"
#include "low_power_config.h"

#include "cslib.h"
#include "hardware_routines.h"

// EM header files
#include "em_device.h"
#include "caplesense.h"
#include "em_core.h"
#include "em_acmp.h"
#include "em_assert.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_lesense.h"

void configureRelaxOscActiveMode(void);
void capSenseChTrigger(void);

// Callback function for LESENSE interrupts.
static void (*lesenseScanCb)(void);
// Callback function for LESENSE interrupts.
static void (*lesenseChCb)(void);
// Configures whether sleep mode scan uses LESENSE or SENSE algo
uint16_t CSLIB_autoScan;

// Buffer passed back to CSLIB to copy into CSLIB_node struct
volatile uint32_t CSLIB_autoScanBuffer[DEF_NUM_SENSORS];

// Flag used in asychronous scanning to signal foreground that new data is availabe
uint16_t CSLIB_autoScanComplete = 0;

/**************************************************************************//**
 * @brief  Setup the CMU
 *****************************************************************************/
void CAPLESENSE_setupCMU(void)
{
  // Ensure core frequency has been updated
  SystemCoreClockUpdate();

  // Select clock source for HF clock.
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
  // Select clock source for LFA clock.
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
  // Select clock source for LFB clock.
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_Disabled);

  // Enable HF peripheral clock.
  CMU_ClockEnable(cmuClock_HFPER, 1);
  // Enable clock for GPIO.
  CMU_ClockEnable(cmuClock_GPIO, 1);
  // Enable clock for ACMP0.
  CMU_ClockEnable(cmuClock_ACMP0, 1);
  // Enable clock for ACMP1.
  CMU_ClockEnable(cmuClock_ACMP1, 1);
  // Enable CORELE clock.
  CMU_ClockEnable(cmuClock_CORELE, 1);
  // Enable clock for LESENSE.
  CMU_ClockEnable(cmuClock_LESENSE, 1);

  // Enable clock divider for LESENSE.
  CMU_ClockDivSet(cmuClock_LESENSE, cmuClkDiv_1);
}

/**************************************************************************//**
 * @brief  Setup the GPIO
 *****************************************************************************/
void CAPLESENSE_setupGPIO(void)
{
  uint16_t index;

  for (index = 0; index < DEF_NUM_SENSORS; index++) {
    GPIO_DriveModeSet(CSLIB_ports[index], gpioDriveModeStandard);
    GPIO_PinModeSet(CSLIB_ports[index], CSLIB_pins[index], gpioModeDisabled, 0);
  }
}
/**************************************************************************//**
 * @brief  Callback for sensor scan complete.
 *****************************************************************************/
void capSenseScanComplete(void)
{
  ;
}

// The current channel we are sensing
static volatile uint8_t currentChannel;

// Enable or disable vboost
bool vboost = false;

/**************************************************************************//**
 * @brief  A bit vector which represents the channels to iterate through
 * @param LESENSE_CHANNELS Vector of channels.
 *****************************************************************************/
static const bool channelsInUse[LESENSE_CHANNELS] = LESENSE_CAPSENSE_CH_IN_USE;

/**************************************************************************//**
 * @brief  LESENSE callback setup
 * @param  scanCb Scan callback
 * @param  chCb Channel callback
 *****************************************************************************/
void CAPLESENSE_setupCallbacks(void (*scanCb)(void), void (*chCb)(void))
{
  lesenseScanCb = scanCb;
  lesenseChCb   = chCb;
}

/**************************************************************************//**
 * @brief  Callback for sensor channel triggered.
 *****************************************************************************/
void capSenseChTrigger(void)
{
}

/**************************************************************************//**
 * @brief  LESENSE interrupt handler
 *****************************************************************************/
void LESENSE_IRQHandler(void)
{
  unsigned long count;
  uint16_t CSLIB_node_index;

  // LESENSE scan complete interrupt.
  if (LESENSE_IF_SCANCOMPLETE & LESENSE_IntGetEnabled()) {
    LESENSE_IntClear(LESENSE_IF_SCANCOMPLETE);

    // Flag is cleared upon first read of data buffer LESENSE_ScanResultDataGet[]
    CSLIB_autoScanComplete = 1;
    timerTick = 1;
    // Iterate trough all channels
    CSLIB_node_index = 0;
    for (currentChannel = 0; currentChannel < LESENSE_CHANNELS; currentChannel++) {
      // If this channel is not in use, skip to the next one
      if (!channelsInUse[currentChannel]) {
        continue;
      }

      // Read out value from LESENSE buffer
      count = LESENSE_ScanResultDataGet();

      // Store value in channelValues
      CSLIB_autoScanBuffer[CSLIB_muxValues[CSLIB_node_index]] = count;

      // CSLIB_node_index only increments for enabled channels
      CSLIB_node_index = CSLIB_node_index + 1;
    }

    // Call callback function.
    if (lesenseScanCb != 0x00000000) {
      lesenseScanCb();
    }
  }

  // LESENSE channel interrupt.
  if (CAPLESENSE_CHANNEL_INT & LESENSE_IntGetEnabled()) {
    // Clear flags.
    LESENSE_IntClear(CAPLESENSE_CHANNEL_INT);

    // Call callback function.
    if (lesenseChCb != 0x00000000) {
      lesenseChCb();
    }
  }

  // Notify comms we have new data and it should update
  sendComms = true;
}

/**************************************************************************//**
 * @brief  Setup the ACMP
 *****************************************************************************/
void CAPLESENSE_setupACMP(void)
{
  // ACMP capsense configuration constant table.
  static const ACMP_CapsenseInit_TypeDef initACMP =
  {
    .fullBias                 = false,
    .halfBias                 = false,
    .biasProg                 =                  0x7,
    .warmTime                 = acmpWarmTime512,
    .hysteresisLevel          = acmpHysteresisLevel7,
    .resistor                 = acmpResistor0,
    .lowPowerReferenceEnabled = false,
    .vddLevel                 =                 0x3D,
    .enable                   = false
  };

  // Configure ACMP locations, ACMP output to pin disabled.
  ACMP_GPIOSetup(ACMP0, 0, false, false);
  ACMP_GPIOSetup(ACMP1, 0, false, false);

  // Initialize ACMPs in capacitive sense mode.
  ACMP_CapsenseInit(ACMP0, &initACMP);
  ACMP_CapsenseInit(ACMP1, &initACMP);

  // Don't enable ACMP, LESENSE controls it!
}

// LESENSE channel configuration constant table in sense mode.
static const LESENSE_ChAll_TypeDef initChsSense = LESENSE_CAPSENSE_SCAN_CONF_SENSE;
// LESENSE channel configuration constant table in sleep mode.
static const LESENSE_ChAll_TypeDef initChsSleep = LESENSE_CAPSENSE_SCAN_CONF_SLEEP;
// LESENSE central configuration constant table. */
static const LESENSE_Init_TypeDef  initLESENSE =
{
  .coreCtrl         =
  {
    .scanStart    = lesenseScanStartPeriodic,
    .prsSel       = lesensePRSCh0,
    .scanConfSel  = lesenseScanConfDirMap,
    .invACMP0     = false,
    .invACMP1     = false,
    .dualSample   = false,
    .storeScanRes = false,
    .bufOverWr    = true,
    .bufTrigLevel = lesenseBufTrigHalf,
    .wakeupOnDMA  = lesenseDMAWakeUpDisable,
    .biasMode     = lesenseBiasModeDutyCycle,
    .debugRun     = false
  },

  .timeCtrl         =
  {
    .startDelay     =          0U
  },

  .perCtrl          =
  {
    .dacCh0Data     = lesenseDACIfData,
    .dacCh0ConvMode = lesenseDACConvModeDisable,
    .dacCh0OutMode  = lesenseDACOutModeDisable,
    .dacCh1Data     = lesenseDACIfData,
    .dacCh1ConvMode = lesenseDACConvModeDisable,
    .dacCh1OutMode  = lesenseDACOutModeDisable,
    .dacPresc       =                        0U,
    .dacRef         = lesenseDACRefBandGap,
    .acmp0Mode      = lesenseACMPModeMuxThres,
    .acmp1Mode      = lesenseACMPModeMuxThres,
    .warmupMode     = lesenseWarmupModeNormal
  },

  .decCtrl          =
  {
    .decInput  = lesenseDecInputSensorSt,
    .chkState  = false,
    .intMap    = true,
    .hystPRS0  = false,
    .hystPRS1  = false,
    .hystPRS2  = false,
    .hystIRQ   = false,
    .prsCount  = true,
    .prsChSel0 = lesensePRSCh0,
    .prsChSel1 = lesensePRSCh1,
    .prsChSel2 = lesensePRSCh2,
    .prsChSel3 = lesensePRSCh3
  }
};

/**************************************************************************//**
 * @brief  Switch to active
 * Assumes that LESENSE scanning has been initiated and configures
 * peripheral for active mode scanning
 *****************************************************************************/
void CAPLESENSE_switchToActive(void)
{
  // Stop LESENSE before configuration.
  LESENSE_ScanStop();

  // Wait until the currently active scan is finished.
  while (LESENSE_STATUS_SCANACTIVE & LESENSE_StatusGet()) ;

  // Clean scan complete interrupt flag.
  LESENSE_IntClear(LESENSE_IEN_SCANCOMPLETE);

  // Clear result buffer.
  LESENSE_ResultBufferClear();

  // Set scan frequency (in Hz).
  (void) LESENSE_ScanFreqSet(0U, (1000U / DEF_ACTIVE_MODE_PERIOD));

  // Set clock divisor for LF clock.
  LESENSE_ClkDivSet(lesenseClkLF, lesenseClkDiv_8);

  // Configure scan channels.
  LESENSE_ChannelAllConfig(&initChsSense);

  // Enable scan complete interrupt.
  LESENSE_IntEnable(LESENSE_IEN_SCANCOMPLETE);

  // Enable LESENSE interrupt in NVIC.
  NVIC_EnableIRQ(LESENSE_IRQn);

  // Start scanning LESENSE channels.
  LESENSE_ScanStart();
}
void CAPLESENSE_switchToSleep(void)
{
  uint8_t     i;
  // Stop LESENSE before configuration.
  LESENSE_ScanStop();

  // Wait until the currently active scan is finished.
  while (LESENSE_STATUS_SCANACTIVE & LESENSE_StatusGet()) ;

  // Clear result buffer.
  LESENSE_ResultBufferClear();

  // Set scan frequency (in Hz).
  (void) LESENSE_ScanFreqSet(0U, (1000U / DEF_SLEEP_MODE_PERIOD));

  // Set clock divisor for LF clock.
  LESENSE_ClkDivSet(lesenseClkLF, lesenseClkDiv_1);

  // Configure scan channels.
  LESENSE_ChannelAllConfig(&initChsSleep);

  for (i = 0; i < DEF_NUM_SENSORS; i++) {
    LESENSE_ChannelThresSet(CSLIB_pins[i], CAPLESENSE_ACMP_VDD_SCALE, 7);
  }
  // Disable scan complete interrupt.
  LESENSE_IntDisable(LESENSE_IEN_SCANCOMPLETE);

  // Enable LESENSE interrupt in NVIC.
  NVIC_EnableIRQ(LESENSE_IRQn);

  // Start scanning LESENSE channels.
  LESENSE_ScanStart();
}
/**************************************************************************//**
 * @brief  Initializes LESENSE peripheral and sets periph to active mode
 * scanning.
 *****************************************************************************/
void CAPLESENSE_initLESENSE(bool sleep)
{
  (void) sleep;
  uint8_t     i;
  static bool init = true;

  // Array for storing the calibration values.
  static uint16_t capsenseCalibrateVals[4];
  // Indicates that sleep mode scanning with LESENSE should be used by library
  CSLIB_autoScan = 1;

  if (init) {
    // Initialize LESENSE interface with RESET.
    LESENSE_Init(&initLESENSE, true);

    // Stop LESENSE before configuration.
    LESENSE_ScanStop();

    // Wait until the currently active scan is finished.
    while (LESENSE_STATUS_SCANACTIVE & LESENSE_StatusGet()) ;

    // Clear result buffer.
    LESENSE_ResultBufferClear();

    // Set scan frequency (in Hz).
    (void) LESENSE_ScanFreqSet(0U, 4U);

    // Set clock divisor for LF clock.
    LESENSE_ClkDivSet(lesenseClkLF, lesenseClkDiv_1);

    // Configure scan channels.
    LESENSE_ChannelAllConfig(&initChsSleep);

    for (i = 0; i < DEF_NUM_SENSORS; i++) {
      LESENSE_ChannelThresSet(CSLIB_pins[i], CAPLESENSE_ACMP_VDD_SCALE, 7);
    }
    // Disable scan complete interrupt.
    LESENSE_IntDisable(LESENSE_IEN_SCANCOMPLETE);

    // Enable LESENSE interrupt in NVIC.
    NVIC_EnableIRQ(LESENSE_IRQn);

    // Start scanning LESENSE channels.
    LESENSE_ScanStart();

    // Run it only once.

    // Assuming that the pads are not touched at first, we can use the result as
    // the threshold value to calibrate the capacitive sensing in LESENSE.
    init = false;

    // Waiting for buffer to be full.
    while (!(LESENSE->STATUS & LESENSE_STATUS_BUFHALFFULL)) ;

    // Read out steady state values from LESENSE for calibration.
    for (i = 0U; i < DEF_NUM_SENSORS; i++) {
      capsenseCalibrateVals[i] = LESENSE_ScanResultDataBufferGet(i) - CAPLESENSE_SENSITIVITY_OFFS;
    }

    for (i = 0; i < DEF_NUM_SENSORS; i++) {
      LESENSE_ChannelThresSet(CSLIB_pins[i], CAPLESENSE_ACMP_VDD_SCALE, capsenseCalibrateVals[i]);
    }
  }
}

/**************************************************************************//**
 * @brief Send the capacative sense system to sleep mode.
 *****************************************************************************/
void CAPLESENSE_Sleep(void)
{
  // Go to EM2 and wait for the measurement to complete.
  EMU_EnterEM2(true);
}

/**************************************************************************//**
 * Execute one CS0 conversion
 *
 * Assumes that the performance characteristics of the sensor have already
 * been configured.  Enables the sensor, starts a scan, blocks until
 * the scan is complete.
 *
 *****************************************************************************/
uint16_t executeConversion(void)
{
  // system will wake either after a scan completes in active mode,
  // or when there is a threshold crossing in sleep mode
  CAPLESENSE_Sleep();
  return 0;
}

/**************************************************************************//**
 * Pre baseline initialization callback
 *
 * Called before a baseline for a sensor has been initialized.
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
}

/**************************************************************************//**
 * Ready CS0 for active mode, unbound sensor scanning
 *
 * This is a top-level call to configure the sensor to its operational state
 * during active mode.
 *
 *****************************************************************************/
void CSLIB_configureSensorForActiveModeCB(void)
{
  configureRelaxOscActiveMode();
}

/**************************************************************************//**
 * Configure CS0 block for active scanning
 *
 * Configure capsense input block for unbound, sensor-by-sensor scanning.
 *
 *****************************************************************************/
void configureRelaxOscActiveMode(void)
{
  CORE_DECLARE_IRQ_STATE;

  // Disable interrupts
  CORE_ENTER_ATOMIC();

  // Setup CMU.
  CAPLESENSE_setupCMU();
  // Setup GPIO.
  CAPLESENSE_setupGPIO();
  // Setup ACMP.
  CAPLESENSE_setupACMP();
  // Setup LESENSE.
  // CAPLESENSE_setupLESENSE(true);
  CAPLESENSE_initLESENSE(false);
  CAPLESENSE_switchToActive();

  // Initialization done, enable interrupts globally. */
  CORE_EXIT_ATOMIC();

  // Setup capSense callbacks.
  CAPLESENSE_setupCallbacks(&capSenseScanComplete, &capSenseChTrigger);
}

/**************************************************************************//**
 * Find highest valid gain setting
 *
 * @returns CS0MD1 gain setting that is valid
 *
 * Executed before entering sleep mode.  used to find highest gain setting
 * possible for all bound CS0 inputs without saturating sensor and gives
 * margin for sleep touch qualification.
 *
 *****************************************************************************/
uint8_t determine_highest_gain(void)
{
  // Stub function, not needed by EFM32 but called by library
  return 0;
}

uint32_t CSLIB_scanSensorCB(uint8_t index)
{
  (void) index;
  // stub callback function, not used in LESENSE
  return 0;
}
