/***************************************************************************//**
 * @file
 * @brief Helper functions for using LESENSE and I2C sensors
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

#include "app_sensor.h"

#include "em_acmp.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_i2c.h"

#include "sl_i2cspm.h"
#include "sl_i2cspm_instances.h"
#include "sl_si70xx.h"
#include "sl_si72xx.h"

void sensorSetup(void)
{
  sl_si70xx_init(sl_i2cspm_sensor, SI7021_ADDR);

  setupVDAC();

  setupACMP();

  setupLESENSE();
}

/**************************************************************************//**
 * @brief  Helper functions to perform data measurements.
 *****************************************************************************/
void sensorReadHumTemp(uint32_t *RhData, int32_t *TempData)
{
  sl_si70xx_measure_rh_and_temp(sl_i2cspm_sensor, SI7021_ADDR, RhData, TempData);
}

void sensorReadHallEffect(int16_t * HallField)
{
  sl_si72xx_read_magfield_data_and_sleep(sl_i2cspm_sensor,
                                         SI7200_ADDR_0,
                                         SI7210_200MT,
                                         SI72XX_SLEEP_MODE,
                                         HallField);
}

/**************************************************************************//**
 * @brief  Sets up the VDAC for LC sensor exciting
 *****************************************************************************/
void setupVDAC(void)
{
  VDAC_Init_TypeDef dacInit = VDAC_INIT_DEFAULT;

  CMU_ClockEnable(cmuClock_VDAC0, true);

  // Use AVDD as reference
  dacInit.reference = vdacRefAvdd;
  // Use internal oscillator to enable EM2 operation
  dacInit.asyncClockMode = true;

  // Configuration structure for VDAC channel 1
  VDAC_InitChannel_TypeDef vdacCh0Init = VDAC_INITCHANNEL_DEFAULT;
  // Enable channel when initialization is done
  vdacCh0Init.enable = true;

  // Initialize DAC
  VDAC_Init(VDAC0, &dacInit);

  // Initialize DAC channel and output data
  VDAC_InitChannel(VDAC0, &vdacCh0Init, DAC_CHANNEL);
  writeDataDAC(VDAC0, DAC_DATA, DAC_CHANNEL);
}

/**************************************************************************//**
 * @brief  Write DAC conversion value
 *****************************************************************************/
void writeDataDAC(VDAC_TypeDef *dac, unsigned int value, unsigned int ch)
{
  // Write data output value to the correct register.
  if (!ch) {
    // Write data to DAC ch 0
    dac->CH0DATA = value;
  } else {
    // Write data to DAC ch 1
    dac->CH1DATA = value;
  }
}

/**************************************************************************//**
 * @brief  Sets up the ACMP to count LC sensor pulses
 *****************************************************************************/
void setupACMP(void)
{
  // ACMP configuration constant table.
  static const ACMP_Init_TypeDef initACMP =
  {
    .fullBias                 = true,                  // fullBias
    .biasProg                 = 0x1F,                  // biasProg
    .interruptOnFallingEdge   = false,                 // interrupt on rising edge
    .interruptOnRisingEdge    = false,                 // interrupt on falling edge
    .inputRange               = acmpInputRangeFull,    // Full ACMP range
    .accuracy                 = acmpAccuracyHigh,      // Low accuracy, low power consumption
    .powerSource              = acmpPowerSourceAvdd,   // Use AVDD as power source
    .hysteresisLevel_0        = acmpHysteresisLevel0,  // hysteresis level 0
    .hysteresisLevel_1        = acmpHysteresisLevel0,  // hysteresis level 1
    .vlpInput                 = acmpVLPInputVADIV,     // Use VADIV as the VLP input source.
    .inactiveValue            = false,                 // inactive value
    .enable                   = true                   // Enable after init.
  };

  static const ACMP_VAConfig_TypeDef initVa =
  {
    acmpVAInputVDD,                                    // Use VDD as input for VA
    0x0D,                                              // VA divider when ACMP output is 0
    0x0C                                               // VA divider when ACMP output is 1
  };

  CMU_ClockEnable(cmuClock_ACMP0, true);

  // Initialize ACMP
  ACMP_Init(ACMP0, &initACMP);

  // Setup VADIV
  ACMP_VASetup(ACMP0, &initVa);

  // ACMP0 input channels
  ACMP_ChannelSet(ACMP0, acmpInputVADIV, acmpInputAPORT0XCH3);

  // Enable LESENSE control of ACMP
  ACMP_ExternalInputSelect(ACMP0, acmpExternalInputAPORT0X);
}

/**************************************************************************//**
 * @brief  Sets up the LESENSE
 *****************************************************************************/
void setupLESENSE(void)
{
  // LESENSE configuration structure
  static const LESENSE_Init_TypeDef initLesense =
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
      .startDelay     = 0
    },

    .perCtrl =
    {
      .dacCh0Data     = lesenseDACIfData,
      .dacCh1Data     = lesenseDACIfData,
      .acmp0Mode      = lesenseACMPModeMux,
      .acmp1Mode      = lesenseACMPModeDisable,
      .warmupMode     = lesenseWarmupModeNormal
    },

    .decCtrl          =
    {
      .decInput  = lesenseDecInputSensorSt,
      .initState = 0,
      .chkState  = false,
      .intMap    = false,
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

  // Channel configuration
  static const LESENSE_ChDesc_TypeDef initLesenseCh3 =
  {
    .enaScanCh     = true,
    .enaPin        = true,
    .enaInt        = true,
    .chPinExMode   = lesenseChPinExLow,
    .chPinIdleMode = lesenseChPinIdleDACC,
    .useAltEx      = false,
    .shiftRes      = false,
    .invRes        = false,
    .storeCntRes   = true,
    .exClk         = lesenseClkHF,
    .sampleClk     = lesenseClkLF,
    .exTime        = 0x07,
    .sampleDelay   = 0x0A,
    .measDelay     = 0x00,
    .acmpThres     = 0x00,
    .sampleMode    = lesenseSampleModeCounter,
    .intMode       = lesenseSetIntPosEdge,
    .cntThres      = 0x0000,
    .compMode      = lesenseCompModeLess,
    .evalMode      = lesenseEvalModeThreshold
  };

  // Configure LC sense excitation/measure pin as push pull
  GPIO_PinModeSet(LCSENSE_CH_PORT, LCSENSE_CH_PIN, gpioModePushPull, 0);

  // Use LFXO as LESENSE clock source since it is already used by the RTCC
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_CORELE, true);
  CMU_ClockEnable(cmuClock_LESENSE, true);

  // Initialize LESENSE interface _with_ RESET
  LESENSE_Init(&initLesense, true);

  // Configure channel 3
  LESENSE_ChannelConfig(&initLesenseCh3, LCSENSE_CH);

  // Set scan frequency
  LESENSE_ScanFreqSet(0, LCSENSE_SCAN_FREQ);

  // Set clock divisor for LF clock
  LESENSE_ClkDivSet(lesenseClkLF, lesenseClkDiv_2);
  // Set clock divisor for HF clock
  LESENSE_ClkDivSet(lesenseClkHF, lesenseClkDiv_1);

  // Enable interrupt in NVIC
  NVIC_EnableIRQ(LESENSE_IRQn);

  // Calibrate LC sensor
  lesenseCalibrateLC(LCSENSE_CH);

  // Start continuous scan
  LESENSE_ScanStart();
}

/***************************************************************************//**
 * @brief
 *   Calibrates an LC sensor on a given channel
 *
 * @details
 *   This function scans a channel until the buffer is full and
 *   uses the last result from the buffer to use as
 *   counter threshold
 *
 * @note
 *   This function assumes that there is no metal near the LC
 *   sensor, that way the number of pulses counted will
 *   be the maximum and can be used as threshold.
 *   If there is metal close the threshold will be lower and
 *   no trigger will occur.
 *   To make sure only the chosen channel is scanned all the
 *   other channels are disabled.
 *
 * @param[in] chIdx
 *   Identifier of the scan channel. Valid range: 0-15.
 *
 ******************************************************************************/
void lesenseCalibrateLC(uint8_t chIdx)
{
  uint8_t i;

  // Enable scan and pin on selected channel
  LESENSE_ChannelEnable(chIdx, true, true);

  // Disable scan and pin on all other channels
  for (i = 0; i < LESENSE_MAX_CHANNELS; i++) {
    if (i != chIdx) {
      LESENSE_ChannelEnable(i, false, false);
    }
  }

  // Start scan
  LESENSE_ScanStart();

  // Waiting for buffer to be full
  while (!(LESENSE->STATUS & LESENSE_STATUS_BUFFULL)) ;

  // Read result and use as counter threshold
  uint32_t calibValue = LESENSE_ScanResultDataBufferGet(BUFFER_INDEX_LAST) - 1;
  LESENSE_ChannelThresSet(chIdx, 0, calibValue);

  // Stop scan
  LESENSE_ScanStop();

  /* Clear result buffer */
  LESENSE_ResultBufferClear();
}

void setupTRNG(void)
{
  uint8_t i = 0, j = 0;

  CMU_ClockEnable(cmuClock_TRNG0, true);

  TRNG0->CONTROL = TRNG_CONTROL_SOFTRESET;
  TRNG0->CONTROL = 0;

  TRNG0->CONTROL = TRNG_CONTROL_CONDBYPASS | TRNG_CONTROL_ENABLE;

  // Read FIFO at least 257 times to ensure randomness
  // of entropy source
  for (i = 0; i < 5; i++) {
    while (TRNG0->FIFOLEVEL < 64) ;
    for (j = 0; j < 64; j++) {
      TRNG0->FIFO;
    }
  }

  // Program random key and enable conditioning function
  TRNG0->KEY0 = littleToBigEndian(TRNG0->FIFO);
  TRNG0->KEY1 = littleToBigEndian(TRNG0->FIFO);
  TRNG0->KEY2 = littleToBigEndian(TRNG0->FIFO);
  TRNG0->KEY3 = littleToBigEndian(TRNG0->FIFO);

  TRNG0->CONTROL = TRNG_CONTROL_ENABLE;
}

void randomNumber(uint32_t *rngResult)
{
  uint8_t i;

  // Read a 128-bit random number from the TRNG FIFO
  for (i = 0; i < 4; i++) {
    rngResult[i] = littleToBigEndian(TRNG0->FIFO);
  }
}

uint32_t littleToBigEndian(uint32_t input)
{
  return ((input >> 24) & 0xFF)
         | ((input << 8) & 0xFF0000)
         | ((input >> 8) & 0xFF00)
         | ((input << 24) & 0xFF000000);
}
