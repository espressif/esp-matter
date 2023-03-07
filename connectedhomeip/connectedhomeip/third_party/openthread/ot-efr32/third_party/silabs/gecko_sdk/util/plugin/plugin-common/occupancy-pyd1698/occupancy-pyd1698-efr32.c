// *****************************************************************************
// * occupancy-pyd1698-efr32.c
// *
// * API for interfacing with a PYD-1698 occupancy sensor
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "event_control/event.h"
#include "hal/hal.h"
#include "hal/micro/micro.h"
#include "em_gpio.h"
#include "em_core.h"
#include "em_cmu.h"
#include "em_common.h"
#include "gpiointerrupt.h"
#include "occupancy-pyd1698.h"
#include EMBER_AF_API_GENERIC_INTERRUPT_CONTROL
#include EMBER_AF_API_OCCUPANCY

// ------------------------------------------------------------------------------
// Plugin private macros

// helper macros, for shortening long plugin option macros
// Some of these have conversion addition or subtractions.  The plugin options
// are meant to control the resulting behavior, not the value that needs to be
// written to the corresponding register.  For example, the blind time register
// accepts 0..15, but corresponds to 1..16 half seconds, so the macro for
// DEFAULT_BLIND_TIME_HS subtracts 1 from the user entered plugin value
#define DEFAULT_THRESHOLD       EMBER_AF_PLUGIN_OCCUPANCY_PYD1698_THRESHOLD
#define DEFAULT_BLIND_TIME_HS \
  (EMBER_AF_PLUGIN_OCCUPANCY_PYD1698_BLIND_TIME - 1)
#define DEFAULT_WINDOW_TIME     EMBER_AF_PLUGIN_OCCUPANCY_PYD1698_WINDOW_TIME
#define DEFAULT_PULSE_COUNTER \
  (EMBER_AF_PLUGIN_OCCUPANCY_PYD1698_PULSE_COUNTER - 1)
#define DEFAULT_FILTER          EMBER_AF_PLUGIN_OCCUPANCY_PYD1698_FILTER_SOURCE
#define DEFAULT_MODE            EMBER_AF_PLUGIN_OCCUPANCY_PYD1698_OPERATION_MODE
#define OCCUPANCY_TIMEOUT_MINUTES \
  EMBER_AF_PLUGIN_OCCUPANCY_PYD1698_OCCUPANCY_TIMEOUT
#define OCCUPANCY_CALIBRATION_TIMEOUT_S \
  EMBER_AF_PLUGIN_OCCUPANCY_PYD1698_OCCUPANCY_CALIBRATION_TIMEOUT
#define FIRMWARE_BLIND_TIME_M \
  EMBER_AF_PLUGIN_OCCUPANCY_PYD1698_FIRMWARE_BLIND_TIME
#define SENSOR_WARMUP_TIME_QS   (4 * 5)

// Pin configuration checks.  These should be defined in the board.h file, but
// default definitions are provided here to match the IST-A78 reference design
#ifndef BSP_PYD1698_SERIN_PORT
#error \
  "BSP_PYD1698_SERIN_PORT not defined in hal config.  Select a port by defining that macro to something like gpioPortF"
#endif

#ifndef BSP_PYD1698_SERIN_PIN
#error \
  "BSP_PYD1698_SERIN_PIN not defined in hal config.  Select a port by defining that macro to something like 4"
#define BSP_PYD1698_SERIN_PIN   4
#endif

#ifndef BSP_PYD1698_DLINK_PORT
#error \
  "BSP_PYD1698_DLINK_PORT not defined in hal config.  Select a port by defining that macro to something like gpioPortF"
#endif

#ifndef BSP_PYD1698_DLINK_PIN
#error \
  "BSP_PYD1698_DLINK_PIN not defined in hal config.  Select a port by defining that macro to something like 7"
#endif

#ifndef BSP_PYD1698_INSTALLATION_JP_PORT
#error \
  "BSP_PYD1698_INSTALLATION_JP_PORT not defined in hal config.  Select a port by defining that macro to something like gpioPortB"
#endif

#ifndef BSP_PYD1698_INSTALLATION_JP_PIN
#error \
  "BSP_PYD1698_INSTALLATION_JP_PIN not defined in hal config.  Select a port by defining that macro to something like 11"
#endif

// This driver requires a substantial amount of bitbanging with very precise
// timing windows.  Unfortunately, the emlib GPIO functions introduce too much
// delay to be effective, and as such register macros with precalculated values
// will be used to control all gpio setting and reading during SERIN and DLINK
// transactions.
#define DLINK_DOUT_REG   (&GPIO->P[BSP_PYD1698_DLINK_PORT].DOUT)
#define DLINK_DIN_REG    (&GPIO->P[BSP_PYD1698_DLINK_PORT].DIN)
#define SERIN_DOUT_REG   (&GPIO->P[BSP_PYD1698_SERIN_PORT].DOUT)
#define SERIN_DIN_REG    (&GPIO->P[BSP_PYD1698_SERIN_PORT].DIN)

#if BSP_PYD1698_DLINK_PIN < 8
#define DLINK_MODE_REG   (&GPIO->P[BSP_PYD1698_DLINK_PORT].MODEL)
#define DLINK_MODE_SHIFT (BSP_PYD1698_DLINK_PIN * 4)
#else
#define DLINK_MODE_REG   (&GPIO->P[BSP_PYD1698_DLINK_PORT].MODEH)
#define DLINK_MODE_SHIFT ((BSP_PYD1698_DLINK_PIN - 8) * 4)
#endif
#define DLINK_MODE_MASK  (0xF << DLINK_MODE_SHIFT)
#define DLINK_MODE_IN    (_GPIO_P_MODEL_MODE0_INPUT << DLINK_MODE_SHIFT)
#define DLINK_MODE_OUT   (_GPIO_P_MODEL_MODE0_PUSHPULL << DLINK_MODE_SHIFT)

#if BSP_PYD1698_SERIN_PIN < 8
#define SERIN_MODE_REG   (&GPIO->P[BSP_PYD1698_SERIN_PORT].MODEL)
#define SERIN_MODE_SHIFT (BSP_PYD1698_SERIN_PIN * 4)
#else
#define SERIN_MODE_REG   (&GPIO->P[BSP_PYD1698_SERIN_PORT].MODEH)
#define SERIN_MODE_SHIFT ((BSP_PYD1698_SERIN_PIN - 8) * 4)
#endif
#define SERIN_MODE_MASK  (0xF << SERIN_MODE_SHIFT)
#define SERIN_MODE_IN    (_GPIO_P_MODEL_MODE0_INPUT << SERIN_MODE_SHIFT)
#define SERIN_MODE_OUT   (_GPIO_P_MODEL_MODE0_PUSHPULL << SERIN_MODE_SHIFT)

// ------------------------------------------------------------------------------
// Plugin events
EmberEventControl emberAfPluginOccupancyPyd1698NotifyEventControl;
EmberEventControl emberAfPluginOccupancyPyd1698OccupancyTimeoutEventControl;
EmberEventControl emberAfPluginOccupancyPyd1698FirmwareBlindTimeoutEventControl;
EmberEventControl emberAfPluginOccupancyPyd1698InitEventControl;

// ------------------------------------------------------------------------------
// Forward declaration of private functions
static uint32_t cfgMsgToData(HalPydCfg_t *cfgMsg);
static void ackPydPin(uint32_t volatile * port, uint8_t pin);
static bool handleJumperChange(bool isInit);
static void clearInterupt(void);
static void interruptIsr(void);

// ------------------------------------------------------------------------------
// Private global variables

// structure used to track configuration data for sensor
static HalPydCfg_t currentPydCfg;

static HalGenericInterruptControlIrqCfg *irqCfg;

static bool initialized = false;

// ------------------------------------------------------------------------------
// Plugin public APIs, callbacks, and event handlers

// Configure the GIC to call the occupancySensedEvent on IRQ from the PYD
// Send an initial configuration message to the occupancy sensor
void halOccupancyInit(void)
{
  // Set up GPIO pins as inputs
  GPIO_PinModeSet(BSP_PYD1698_INSTALLATION_JP_PORT,
                  BSP_PYD1698_INSTALLATION_JP_PIN,
                  gpioModeInput,
                  0);
  GPIO_PinModeSet(BSP_PYD1698_SERIN_PORT,
                  BSP_PYD1698_SERIN_PIN,
                  gpioModePushPull,
                  0);
  GPIO_PinModeSet(BSP_PYD1698_DLINK_PORT,
                  BSP_PYD1698_DLINK_PIN,
                  gpioModeInput,
                  0);
  GPIO_PinOutClear(BSP_PYD1698_SERIN_PORT, BSP_PYD1698_SERIN_PIN);

  currentPydCfg.reserved = PYD_CONFIG_RESERVED_VALUE;
  currentPydCfg.filterSource = DEFAULT_FILTER;
  currentPydCfg.operationMode = DEFAULT_MODE;
  currentPydCfg.windowTime = DEFAULT_WINDOW_TIME;
  currentPydCfg.pulseCounter = DEFAULT_PULSE_COUNTER;
  currentPydCfg.blindTime = DEFAULT_BLIND_TIME_HS;
  currentPydCfg.sensitivity = DEFAULT_THRESHOLD;

  // The sensor tends to provide false positives during the first ~5 seconds of
  // operation, so a short delay should be added between setting up the device
  // and honoring occupancy detect events
  clearInterupt();
  handleJumperChange(true);

  // Set up GIC to interrupt on high level of DATALINK pin
  irqCfg = halGenericInterruptControlIrqCfgInitialize(
    BSP_PYD1698_DLINK_PIN,
    BSP_PYD1698_DLINK_PORT,
    SL_CTZ(BSP_PYD1698_DLINK_EM4WU));
  halGenericInterruptControlIrqIsrAssignFxn(irqCfg, &interruptIsr);
  halGenericInterruptControlIrqEdgeConfig(irqCfg, HAL_GIC_INT_CFG_LEVEL_POS);
  halGenericInterruptControlIrqClear(irqCfg);
  halGenericInterruptControlIrqEnable(irqCfg);

  emberEventControlSetDelayQS(emberAfPluginOccupancyPyd1698InitEventControl,
                              SENSOR_WARMUP_TIME_QS);
}

void emberAfPluginOccupancyPyd1698InitEventHandler(void)
{
  emberEventControlSetInactive(emberAfPluginOccupancyPyd1698InitEventControl);
  initialized = true;
}

// This event will be called by the GIC plugin whenever an interrupt is detected
// from the occupancy sensor.
void emberAfPluginOccupancyPyd1698NotifyEventHandler(void)
{
  emberEventControlSetInactive(
    emberAfPluginOccupancyPyd1698NotifyEventControl);

  if (!initialized) {
    return;
  }

  halOccupancyStateChangedCallback(HAL_OCCUPANCY_STATE_OCCUPIED);

  if (handleJumperChange(false) == true) {
    // Ensure that the device doesn't remain in firmware blind mode when the
    // installation mode jumper is in place
    halGenericInterruptControlIrqEnable(irqCfg);
    emberEventControlSetInactive(
      emberAfPluginOccupancyPyd1698FirmwareBlindTimeoutEventControl);
  } else {
    // If the jumper is not in place, enter the firmware blind mode
    halGenericInterruptControlIrqDisable(irqCfg);
    emberEventControlSetDelayMinutes(
      emberAfPluginOccupancyPyd1698FirmwareBlindTimeoutEventControl,
      FIRMWARE_BLIND_TIME_M);
  }
}

// This event is activated at the end of the firmware blind time.  It must
// determine if an occupancy event occurred during that blind time and act
// accordingly.  The firmware blind is the amount of time (in minutes) during
// which the firmware will ignore interrupts from the occupancy sensor.
// This can be used to generate a large blind window than the 16 seconds
// available through hardware, which allows the MCU to sleep more and thus use
// less battery.  Assign a value of 0 to disable the firmware blind time.
void emberAfPluginOccupancyPyd1698FirmwareBlindTimeoutEventHandler(void)
{
  emberEventControlSetInactive(
    emberAfPluginOccupancyPyd1698FirmwareBlindTimeoutEventControl);

  if (GPIO_PinInGet(BSP_PYD1698_DLINK_PORT,
                    BSP_PYD1698_DLINK_PIN)) {
    // If an occupancy event occurred while the device was sleeping, handle it.
    emberEventControlSetActive(
      emberAfPluginOccupancyPyd1698NotifyEventControl);
    clearInterupt();
  } else {
    // Otherwise, enable interrupts, and the device will wake either the next
    // time the room is occupied or when enough time has passed for the room to
    // be considered unoccupied.
    halGenericInterruptControlIrqEnable(irqCfg);
  }
}

// This event is triggered when no occupancy events have occurred for a long
// enough time that the room should now be considered unoccupied
void emberAfPluginOccupancyPyd1698OccupancyTimeoutEventHandler(void)
{
  emberEventControlSetInactive(
    emberAfPluginOccupancyPyd1698OccupancyTimeoutEventControl);

  halOccupancyStateChangedCallback(HAL_OCCUPANCY_STATE_UNOCCUPIED);
}

// The init callback, which will be called by the framework on init.
void emberAfPluginOccupancyPyd1698InitCallback(void)
{
  halOccupancyInit();
}

HalOccupancySensorType halOccupancyGetSensorType(void)
{
  return HAL_OCCUPANCY_SENSOR_TYPE_PIR;
}

// ------------------------------------------------------------------------------
// Private plugin functions

static void interruptIsr(void)
{
  clearInterupt();
  emberEventControlSetActive(emberAfPluginOccupancyPyd1698NotifyEventControl);
}

// returns true if the jumper is set, false if the jumper is not set.  isInit
// should be true if this is the first time the jumper is being queried.  This
// will activate the occupancyTimeout event, update the static jumper state
// variable, and write the new blind time if this is the first transition to a
// new jumper state.
static bool handleJumperChange(bool isInit)
{
  static bool jumperInserted;

  // on initialization, the jumper will be assumed to be set
  if (isInit) {
    jumperInserted = true;
  }

  // pin will be low when jumper is in place (in installation mode)
  if (GPIO_PinInGet(BSP_PYD1698_INSTALLATION_JP_PORT,
                    BSP_PYD1698_INSTALLATION_JP_PIN)) {
    if ((jumperInserted == true) || (isInit == true)) {
      // if the jumper just transitioned from in place to not in place, reset
      // the blind time to the plugin option specified value, standard operation
      // mode.
      currentPydCfg.blindTime = DEFAULT_BLIND_TIME_HS;
      halOccupancyPyd1698WriteConfiguration(&currentPydCfg);
    }
    jumperInserted = false;
    emberEventControlSetDelayMinutes(
      emberAfPluginOccupancyPyd1698OccupancyTimeoutEventControl,
      OCCUPANCY_TIMEOUT_MINUTES);
  } else {
    if ((jumperInserted == false) || (isInit == true)) {
      // if the jumper just transition from not in place to in place, the user
      // has entered  installation mode, so the blind time should be set to its
      // minimum value
      currentPydCfg.blindTime = 0;
      halOccupancyPyd1698WriteConfiguration(&currentPydCfg);
    }
    jumperInserted = true;
    emberEventControlSetDelayQS(
      emberAfPluginOccupancyPyd1698OccupancyTimeoutEventControl,
      OCCUPANCY_CALIBRATION_TIMEOUT_S * 4);
  }
  return jumperInserted;
}

void halOccupancyPyd1698WriteConfiguration(HalPydCfg_t *cfgMsg)
{
  uint32_t outData;
  volatile uint8_t i;
  uint32_t outHi = *SERIN_DOUT_REG | (1 << BSP_PYD1698_SERIN_PIN);
  uint32_t outLo = *SERIN_DOUT_REG & ~(1 << BSP_PYD1698_SERIN_PIN);

  outData = cfgMsgToData(cfgMsg);

  // The timing windows are very tight in this section.  Adding tens of cycles
  // between GPIO transitions is enough to exceed device tolerances.  As such,
  // all interrupts will be disabled during this time sensitive bit banged
  // transaction
  ATOMIC(
    for (i = 0; i < PYD_MESSAGE_WRITE_LENGTH_BITS; i++) {
    // Drive the SerIn pin hi for ~5 cycles, then low for ~5 cycles, informing
    // the PYD that the next bit is going to be written
    ackPydPin(SERIN_DOUT_REG, BSP_PYD1698_SERIN_PIN);

    // Set hi or low based on the data to be written
    if (outData & (1 << (PYD_MESSAGE_WRITE_LENGTH_BITS - 1 - i))) {
      *SERIN_DOUT_REG = outHi;
    } else {
      *SERIN_DOUT_REG = outLo;
    }

    // Delay long enough for the PYD to read the bit, 72 uS
    halCommonDelayMicroseconds(PYD_MESSAGE_WRITE_BIT_DELAY_US);
  }
    *SERIN_DOUT_REG = outLo;
    )
}

void halOccupancyPyd1698Read(HalPydInMsg_t *readMsg)
{
  uint8_t i;
  volatile int64u data = 0;
  uint32_t outCfg;
  uint32_t inCfg;
  uint32_t bitHiVal = *DLINK_DOUT_REG | (1 << BSP_PYD1698_DLINK_PIN);
  uint32_t bitLoVal = *DLINK_DOUT_REG & ~(1 << BSP_PYD1698_DLINK_PIN);

  outCfg = *DLINK_MODE_REG & ~DLINK_MODE_MASK;
  inCfg = outCfg | DLINK_MODE_IN;
  outCfg |= DLINK_MODE_OUT;

  halGenericInterruptControlIrqDisable(irqCfg);
  // To initiate a read, first reconfigure the interrupt/ser_in pin as an output
  // Then, drive it high for 110-150 uS.
  // Then, switch to an input, wait for the input to be driven by the PYD, and
  //   read the input bit
  // Finally, switch back to an output and drive the signal high for another
  //   110-150 uS, signalling to the PYD that the bit was received and it can
  //   transmit another.

  // Set the DLINK bit of the GPIO set register, to guarantee output will be
  // driven high once pin is enabled as output
  GPIO_PinOutSet(BSP_PYD1698_DLINK_PORT, BSP_PYD1698_DLINK_PIN);

  ATOMIC(

    // Set DLINK as output
    GPIO_PinModeSet(BSP_PYD1698_DLINK_PORT, BSP_PYD1698_DLINK_PIN,
                    gpioModePushPull, 0);
    GPIO_PinOutSet(BSP_PYD1698_DLINK_PORT, BSP_PYD1698_DLINK_PIN);

    // Hold hi for 110-150 uS.  Obtained from datasheet.
    halCommonDelayMicroseconds(110);

    // The timing windows are very tight in this section.  Adding tens of cycles
    // between GPIO transitions is enough to exceed device tolerances.  As such,
    // all interrupts will be disabled during this time sensitive bit banged
    // transaction
    for (i = 0; i < PYD_MESSAGE_READ_LENGTH_BITS; i++) {
    // Drive the output high for ~5 cycles, then low for ~5 cycles, triggering
    // the next bit of data from the PYD
    ackPydPin(DLINK_DOUT_REG, BSP_PYD1698_DLINK_PIN);

    // Set the pin to input
    *DLINK_MODE_REG = inCfg;

    // we need to wait long enough for the PYD to drive the signal low.  16 uS
    // should do, derived from experimentation.
    halCommonDelayMicroseconds(16);

    // read in the data
    if (*DLINK_DIN_REG & (1 << BSP_PYD1698_DLINK_PIN)) {
      data |= 1 << (PYD_MESSAGE_READ_LENGTH_BITS - 1 - i);
    }

    *DLINK_DOUT_REG = bitLoVal;
    *DLINK_MODE_REG = outCfg;
    *DLINK_DOUT_REG = bitLoVal;
  }
    )

  // convert the read message to configuration structure format
  readMsg->config->reserved = (data & PYD_CONFIG_RESERVED_MASK)
                              >> PYD_CONFIG_RESERVED_BIT;
  readMsg->config->filterSource = (data & PYD_CONFIG_FILTER_SRC_MASK)
                                  >> PYD_CONFIG_FILTER_SRC_BIT;
  readMsg->config->operationMode = (data & PYD_CONFIG_OPERATION_MODE_MASK)
                                   >> PYD_CONFIG_OPERATION_MODE_BIT;
  readMsg->config->windowTime = (data & PYD_CONFIG_WINDOW_TIME_MASK)
                                >> PYD_CONFIG_WINDOW_TIME_BIT;
  readMsg->config->pulseCounter = (data & PYD_CONFIG_PULSE_COUNTER_MASK)
                                  >> PYD_CONFIG_PULSE_COUNTER_BIT;
  readMsg->config->blindTime = (data & PYD_CONFIG_BLIND_TIME_MASK)
                               >> PYD_CONFIG_BLIND_TIME_BIT;
  readMsg->config->sensitivity = (data & PYD_CONFIG_SENSITIVITY_MASK)
                                 >> PYD_CONFIG_SENSITIVITY_BIT;
  readMsg->AdcVoltage = ((data & PYD_ADC_VOLTAGE_MASK) >> PYD_ADC_VOLTAGE_BIT);

  // return the GPIO to its previous configuration
  GPIO_PinModeSet(BSP_PYD1698_DLINK_PORT,
                  BSP_PYD1698_DLINK_PIN,
                  gpioModeInput,
                  0);

  halGenericInterruptControlIrqClear(irqCfg);
  halGenericInterruptControlIrqEnable(irqCfg);
  return;
}

void halOccupancyPyd1698GetCurrentConfiguration(HalPydCfg_t *config)
{
  config->reserved = currentPydCfg.reserved;
  config->filterSource = currentPydCfg.filterSource;
  config->operationMode = currentPydCfg.operationMode;
  config->windowTime = currentPydCfg.windowTime;
  config->pulseCounter = currentPydCfg.pulseCounter;
  config->blindTime = currentPydCfg.blindTime;
  config->sensitivity = currentPydCfg.sensitivity;
}

static uint32_t cfgMsgToData(HalPydCfg_t *cfgMsg)
{
  uint32_t retVal;

  retVal = 0;

  cfgMsg->reserved = PYD_CONFIG_RESERVED_VALUE;

  retVal |= (cfgMsg->reserved << PYD_CONFIG_RESERVED_BIT)
            & PYD_CONFIG_RESERVED_MASK;
  retVal |= (cfgMsg->filterSource << PYD_CONFIG_FILTER_SRC_BIT)
            & PYD_CONFIG_FILTER_SRC_MASK;
  retVal |= (cfgMsg->operationMode << PYD_CONFIG_OPERATION_MODE_BIT)
            & PYD_CONFIG_OPERATION_MODE_MASK;
  retVal |= (cfgMsg->windowTime << PYD_CONFIG_WINDOW_TIME_BIT)
            & PYD_CONFIG_WINDOW_TIME_MASK;
  retVal |= (cfgMsg->pulseCounter << PYD_CONFIG_PULSE_COUNTER_BIT)
            & PYD_CONFIG_PULSE_COUNTER_MASK;
  retVal |= (cfgMsg->blindTime << PYD_CONFIG_BLIND_TIME_BIT)
            & PYD_CONFIG_BLIND_TIME_MASK;
  retVal |= (cfgMsg->sensitivity << PYD_CONFIG_SENSITIVITY_BIT)
            & PYD_CONFIG_SENSITIVITY_MASK;

  return retVal;
}

static void clearInterupt(void)
{
  // drive the DLink pin low to reset interrupt
  GPIO_PinOutClear(BSP_PYD1698_DLINK_PORT, BSP_PYD1698_DLINK_PIN);

  // Set DLINK as output
  GPIO_PinModeSet(BSP_PYD1698_DLINK_PORT,
                  BSP_PYD1698_DLINK_PIN,
                  gpioModePushPull,
                  0);

  ATOMIC(
    // Wait two cycles of the PYD's clock using NO OP instructions
    asm ("NOP");
    asm ("NOP");
    asm ("NOP");
    asm ("NOP");
    asm ("NOP");
    asm ("NOP");
    asm ("NOP");
    )
  // Revert the config register to what it was before we modified it
  GPIO_PinModeSet(BSP_PYD1698_DLINK_PORT,
                  BSP_PYD1698_DLINK_PIN,
                  gpioModeInput,
                  0);
}

__STATIC_INLINE void ackPydPin(uint32_t volatile * port, uint8_t pin)
{
  uint16_t bitHiVal = *port | (1 << pin);
  uint16_t bitLoVal = *port & ~(1 << pin);

  *port = bitLoVal;
  asm ("NOP");
  asm ("NOP");
  asm ("NOP");
  asm ("NOP");
  asm ("NOP");
  asm ("NOP");
  asm ("NOP");
  asm ("NOP");
  *port = bitHiVal;
  asm ("NOP");
  asm ("NOP");
  asm ("NOP");
  asm ("NOP");
  asm ("NOP");
  asm ("NOP");
  asm ("NOP");
  asm ("NOP");
}
