// *****************************************************************************
// * battery-monitor-efr32.c
// *
// * Code to implement a battery monitor.
// *
// * This code will read the battery voltage during a transmission (in theory
// * when we are burning the most current), and update the battery voltage
// * attribute in the power configuration cluster.
// *
// * Copyright 2016 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "stack/include/ember-types.h"
#include "event_control/event.h"
#include "hal/hal.h"

#include EMBER_AF_API_BATTERY_MONITOR
#include EMBER_AF_API_GENERIC_INTERRUPT_CONTROL
#include "em_cmu.h"
#include "em_adc.h"
#include "em_prs.h"

// Shorter macros for plugin options
#define FIFO_SIZE \
  EMBER_AF_PLUGIN_BATTERY_MONITOR_SAMPLE_FIFO_SIZE
#define MS_BETWEEN_BATTERY_CHECK \
  (EMBER_AF_PLUGIN_BATTERY_MONITOR_MONITOR_TIMEOUT_M * 60 * 1000)

#define MAX_INT_MINUS_DELTA              0xe0000000

#define PRS_CH_CTRL_SOURCESEL_RAC_TX \
  (PRS_RAC_TX & _PRS_CH_CTRL_SOURCESEL_MASK)
#define PRS_CH_CTRL_SIGSEL_RAC_TX \
  (PRS_RAC_TX & _PRS_CH_CTRL_SIGSEL_MASK)

// Default settings to be used when configured the PRS to cause an external pin
// to emulate TX_ACTIVE functionality
#define PRS_SOURCE                       PRS_CH_CTRL_SOURCESEL_RAC_TX
#define PRS_SIGNAL                       PRS_CH_CTRL_SIGSEL_RAC_TX
#define PRS_EDGE                         prsEdgeOff
#define PRS_PIN_SHIFT                    (8                                   \
                                          * (BSP_BATTERYMON_TX_ACTIVE_CHANNEL \
                                             % 4))
#define PRS_PIN_MASK                     (0x1F << PRS_PIN_SHIFT)

#if BSP_BATTERYMON_TX_ACTIVE_CHANNEL < 4
#define PRS_ROUTE_LOC                    ROUTELOC0
#elif BSP_BATTERYMON_TX_ACTIVE_CHANNEL < 8
#define PRS_ROUTE_LOC                    ROUTELOC1
#else
#define PRS_ROUTE_LOC                    ROUTELOC2
#endif

// Default settings used to configured the ADC to read the battery voltage
#define ADC_INITSINGLE_BATTERY_VOLTAGE                                \
  {                                                                   \
    adcPRSSELCh0, /* PRS ch0 (if enabled). */                         \
    adcAcqTime16, /* 1 ADC_CLK cycle acquisition time. */             \
    adcRef5VDIFF, /* V internal reference. */                         \
    adcRes12Bit, /* 12 bit resolution. */                             \
    adcPosSelAVDD, /* Select Vdd as posSel */                         \
    adcNegSelVSS, /* Select Vss as negSel */                          \
    false,       /* Single ended input. */                            \
    false,       /* PRS disabled. */                                  \
    false,       /* Right adjust. */                                  \
    false,       /* Deactivate conversion after one scan sequence. */ \
    false,       /* No EM2 DMA wakeup from single FIFO DVL */         \
    false        /* Discard new data on full FIFO. */                 \
  }

#define ADC_REFERENCE_VOLTAGE_MILLIVOLTS 5000
// ------------------------------------------------------------------------------
// Forward Declaration
static uint16_t filterVoltageSample(uint16_t sample);
static uint32_t AdcToMilliV(uint32_t adcVal);

// ------------------------------------------------------------------------------
// Globals

EmberEventControl emberAfPluginBatteryMonitorReadADCEventControl;

// structure used to store irq configuration from GIC plugin
static HalGenericInterruptControlIrqCfg *irqConfig;

// count used to track when the last measurement occurred
// Ticks start at 0.  We use this value to limit how frequently we make
// measurements in an effort to conserve battery power.  By setting this to an
// arbitrary value close to MAX_INT, we are going to make sure we make a
// battery measurement on the first transmission.
static uint32_t lastBatteryMeasureTick = MAX_INT_MINUS_DELTA;

// sample FIFO access variables
static uint8_t samplePtr = 0;
static uint16_t voltageFifo[FIFO_SIZE];
static bool fifoInitialized = false;

// Remember the last reported voltage value from callback, which will be the
// return value if anyone needs to manually poll for data
static uint16_t lastReportedVoltageMilliV;

// ------------------------------------------------------------------------------
// Implementation of public functions

void emberAfPluginBatteryMonitorInitCallback(void)
{
  halBatteryMonitorInitialize();
}

void halBatteryMonitorInitialize(void)
{
  uint32_t flags;
  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
  ADC_InitSingle_TypeDef initAdc = ADC_INITSINGLE_BATTERY_VOLTAGE;

  // Enable ADC clock
  CMU_ClockEnable(cmuClock_ADC0, true);

  // Initialize the ADC peripheral
  ADC_Init(ADC0, &init);

  // Setup ADC for single conversions for reading AVDD with a 5V reference
  ADC_InitSingle(ADC0, &initAdc);

  flags = ADC_IntGet(ADC0);
  ADC_IntClear(ADC0, flags);
  ADC_Start(ADC0, adcStartSingle);

  CMU_ClockEnable(cmuClock_PRS, true);

  // Initialize the PRS system to drive a GPIO high when the preamble is in the
  // air, effectively becoming a TX_ACT pin
  PRS_SourceSignalSet(BSP_BATTERYMON_TX_ACTIVE_CHANNEL,
                      PRS_SOURCE,
                      PRS_SIGNAL,
                      PRS_EDGE);
  // Enable the PRS channel and set the pin routing per the settings in the
  // board configuration header
  BUS_RegMaskedSet(&PRS->ROUTEPEN, (1 << BSP_BATTERYMON_TX_ACTIVE_CHANNEL));
  PRS->PRS_ROUTE_LOC = PRS->PRS_ROUTE_LOC & ~PRS_PIN_MASK;
  PRS->PRS_ROUTE_LOC = PRS->PRS_ROUTE_LOC
                       | (BSP_BATTERYMON_TX_ACTIVE_LOC
                          << PRS_PIN_SHIFT);
  GPIO_PinModeSet(BSP_BATTERYMON_TX_ACTIVE_PORT,
                  BSP_BATTERYMON_TX_ACTIVE_PIN,
                  gpioModePushPull,
                  0);

  // Set up the generic interrupt controller to activate the readADC event when
  // TX_ACTIVE goes hi
  irqConfig = halGenericInterruptControlIrqCfgInitialize(
    BSP_BATTERYMON_TX_ACTIVE_PIN,
    BSP_BATTERYMON_TX_ACTIVE_PORT,
    0);
  halGenericInterruptControlIrqEventRegister(
    irqConfig,
    &emberAfPluginBatteryMonitorReadADCEventControl);
  halGenericInterruptControlIrqEdgeConfig(irqConfig,
                                          HAL_GIC_INT_CFG_LEVEL_POS);

  halGenericInterruptControlIrqEnable(irqConfig);
}

uint16_t halGetBatteryVoltageMilliV(void)
{
  return lastReportedVoltageMilliV;
}

// This event will sample the ADC during a radio transmission and notify any
// interested parties of a new valid battery voltage level via the
// emberAfPluginBatteryMonitorDataReadyCallback
void emberAfPluginBatteryMonitorReadADCEventHandler(void)
{
  uint32_t flags;
  uint32_t vData;
  uint16_t voltageMilliV;
  uint32_t currentMsTick = halCommonGetInt32uMillisecondTick();
  uint32_t timeSinceLastMeasureMS = currentMsTick - lastBatteryMeasureTick;
  ADC_InitSingle_TypeDef initAdc = ADC_INITSINGLE_BATTERY_VOLTAGE;

  emberEventControlSetInactive(emberAfPluginBatteryMonitorReadADCEventControl);

  // In case something else in the system was using the ADC, reconfigure it to
  // properly sample the battery voltage
  ADC_InitSingle(ADC0, &initAdc);

  if (timeSinceLastMeasureMS >= MS_BETWEEN_BATTERY_CHECK) {
    // The most common and shortest (other than the ACK) transmission is the
    // data poll.  It takes 512 uS for a data poll, which is plenty of time for
    // a 16 cycle conversion
    flags = ADC_IntGet(ADC0);
    ADC_IntClear(ADC0, flags);
    ADC_Start(ADC0, adcStartSingle);

    // wait for the ADC to finish sampling
    while ((ADC_IntGet(ADC0) & ADC_IF_SINGLE) != ADC_IF_SINGLE) {
    }
    vData = ADC_DataSingleGet(ADC0);
    voltageMilliV = AdcToMilliV(vData);

    // filter the voltage to prevent spikes from overly influencing data
    voltageMilliV = filterVoltageSample(voltageMilliV);

    emberAfPluginBatteryMonitorDataReadyCallback(voltageMilliV);
    lastReportedVoltageMilliV = voltageMilliV;
    lastBatteryMeasureTick = currentMsTick;
  }
}

// ------------------------------------------------------------------------------
// Implementation of private functions

// Convert a 12 bit analog ADC reading (with a reference of 5 volts) to a value
// in milliVolts
static uint32_t AdcToMilliV(uint32_t adcVal)
{
  uint32_t vMax = 0xFFF; // 12 bit ADC maximum
  uint32_t referenceMilliV = ADC_REFERENCE_VOLTAGE_MILLIVOLTS;
  float milliVPerBit = (float)referenceMilliV / (float)vMax;

  return (uint32_t)(milliVPerBit * adcVal);
}

// Provide smoothing of the voltage readings by reporting an average over the
// last few values
static uint16_t filterVoltageSample(uint16_t sample)
{
  uint32_t voltageSum;
  uint8_t i;

  if (fifoInitialized) {
    voltageFifo[samplePtr++] = sample;

    if (samplePtr >= FIFO_SIZE) {
      samplePtr = 0;
    }
    voltageSum = 0;
    for (i = 0; i < FIFO_SIZE; i++) {
      voltageSum += voltageFifo[i];
    }
    sample = voltageSum / FIFO_SIZE;
  } else {
    for (i = 0; i < FIFO_SIZE; i++) {
      voltageFifo[i] = sample;
    }
    fifoInitialized = true;
  }

  return sample;
}
