// *****************************************************************************
// * gpio-sensor.c
// *
// * Plugin used to interface with a generic gpio sensor
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER

#include "stack/include/ember-types.h"
#include "event_control/event.h"

#include "hal/hal.h"
#include "hal/micro/micro.h"
#include EMBER_AF_API_GENERIC_INTERRUPT_CONTROL
#include EMBER_AF_API_GPIO_SENSOR

// ------------------------------------------------------------------------------
// Plugin private macros

// These values should be defined in the board.h for the project.  If they are
// not present, sample values will be provided based on the EFR32 occupancy
// sensor reference design.
#if defined(CORTEXM3_EFR32) && !defined(GPIO_SENSOR_EM4WUPIN)
#define GPIO_SENSOR_EM4WUPIN 0
#endif

#if !defined(CORTEXM3_EFR32) && !defined(GPIO_SENSOR_IRQ)
#error \
  "No definition for GPIO_SENSOR_IRQ found.  Select an IRQ for this plugin and define it to something like HAL_GIC_IRQ_NUMC"
#endif

#if !defined(GPIO_SENSOR_PIN)
#error \
  "No definition for GPIO_SENSOR_PIN found.  Select an pin for this plugin and define it to something like 6"
#endif

#if !defined(GPIO_SENSOR_PORT)
#error \
  "No definition for GPIO_SENSOR_PORT found.  Select an IRQ for this plugin and define it to something like HAL_GIC_GPIO_PORTC"
#endif

// Shorthand macros used for referencing plugin options
#define SENSOR_ASSERT_DEBOUNCE   EMBER_AF_PLUGIN_GPIO_SENSOR_ASSERT_DEBOUNCE
#define SENSOR_DEASSERT_DEBOUNCE \
  EMBER_AF_PLUGIN_GPIO_SENSOR_DEASSERT_DEBOUNCE
#define SENSOR_IS_ACTIVE_HI      EMBER_AF_PLUGIN_GPIO_SENSOR_SENSOR_POLARITY

// ------------------------------------------------------------------------------
// Plugin private variables

// Events used internal to the plugin
EmberEventControl emberAfPluginGpioSensorInterruptEventControl;
EmberEventControl emberAfPluginGpioSensorDebounceEventControl;

// State variables to track the status of the gpio sensor
static HalGpioSensorState lastSensorStatus = HAL_GPIO_SENSOR_ACTIVE;
static HalGpioSensorState newSensorStatus = HAL_GPIO_SENSOR_NOT_ACTIVE;

// structure used to store irq configuration from GIC plugin
static HalGenericInterruptControlIrqCfg *irqConfig;

// ------------------------------------------------------------------------------
// Forward declaration of functions
static void sensorDeassertedCallback(void);
static void sensorAssertedCallback(void);
static void sensorStateChangeDebounce(HalGpioSensorState status);

void emberAfPluginGpioSensorStateChangedCallback(uint8_t);

// ------------------------------------------------------------------------------
// Plugin consumed callback implementations

// This function will be called on device init.
void emberAfPluginGpioSensorInitCallback(void)
{
  halGpioSensorInitialize();
}

// ------------------------------------------------------------------------------
// Plugin Event Handlers

// gpio sensor state change handler.  This is the handler for the event that is
// activated by the GIC plugin when an interrupt occurs on the gpio sensor.  It
// determines whether the button is asserted or deasserted based on the polarity
// option and GPIO state, and calls the appropriate assert or deassert handler.
void emberAfPluginGpioSensorInterruptEventHandler(void)
{
  uint8_t reedValue;

  emberEventControlSetInactive(emberAfPluginGpioSensorInterruptEventControl);

  reedValue = halGenericInterruptControlIrqReadGpio(irqConfig);

  // If the gpio sensor was set to active high by the plugin properties, call
  // deassert when the value is 0 and assert when the value is 1.
  if (SENSOR_IS_ACTIVE_HI) {
    if (reedValue == 0) {
      sensorDeassertedCallback();
    } else {
      sensorAssertedCallback();
    }
  } else {
    if (reedValue == 0) {
      sensorAssertedCallback();
    } else {
      sensorDeassertedCallback();
    }
  }
}

// gpio sensor Debounce Handler.  This event is used to provide a short,
// parameterized delay between the gpio sensor state initially changing and
// being verified as needing to take action.  In the case of a bounce scenario,
// this event will be cancelled by the debounce state machine before it can
// execute.
void emberAfPluginGpioSensorDebounceEventHandler(void)
{
  emberEventControlSetInactive(emberAfPluginGpioSensorDebounceEventControl);
  lastSensorStatus = newSensorStatus;

  emberAfPluginGpioSensorStateChangedCallback(newSensorStatus);
}

// ------------------------------------------------------------------------------
// Plugin private functions

// Helper function used to define action taken when a not yet debounced change
// in the gpio sensor is detected as having opened the switch.
static void sensorDeassertedCallback(void)
{
  sensorStateChangeDebounce(HAL_GPIO_SENSOR_NOT_ACTIVE);
}

// Helper function used to define action taken when a not yet debounced change
// in the gpio sensor is detected as having closed the switch
static void sensorAssertedCallback(void)
{
  sensorStateChangeDebounce(HAL_GPIO_SENSOR_ACTIVE);
}

// State machine used to debounce the gpio sensor.  This function is called on
// every transition of the gpio sensor's GPIO pin.  A delayed event is used to
// take action on the pin transition.  If the pin changes back to its original
// state before the delayed event can execute, that change is marked as a bounce
// and no further action is taken.
static void sensorStateChangeDebounce(HalGpioSensorState status)
{
  if (status == lastSensorStatus) {
    // we went back to last status before debounce.  don't send the
    // message.
    emberEventControlSetInactive(emberAfPluginGpioSensorDebounceEventControl);
    return;
  }
  if (status == HAL_GPIO_SENSOR_ACTIVE) {
    newSensorStatus = status;
    emberEventControlSetDelayMS(emberAfPluginGpioSensorDebounceEventControl,
                                SENSOR_ASSERT_DEBOUNCE);
    return;
  } else if (status == HAL_GPIO_SENSOR_NOT_ACTIVE) {
    newSensorStatus = status;
    emberEventControlSetDelayMS(emberAfPluginGpioSensorDebounceEventControl,
                                SENSOR_DEASSERT_DEBOUNCE);
    return;
  }
}

// ------------------------------------------------------------------------------
// Plugin public functions

void halGpioSensorInitialize(void)
{
  uint8_t reedValue;

  // Set up the generic interrupt controller to handle changes on the gpio
  // sensor
#if defined(CORTEXM3_EFR32)
  irqConfig = halGenericInterruptControlIrqCfgInitialize(GPIO_SENSOR_PIN,
                                                         GPIO_SENSOR_PORT,
                                                         GPIO_SENSOR_EM4WUPIN);
#else
  irqConfig = halGenericInterruptControlIrqCfgInitialize(GPIO_SENSOR_PIN,
                                                         GPIO_SENSOR_PORT,
                                                         GPIO_SENSOR_IRQ);
#endif
  halGenericInterruptControlIrqEventRegister(irqConfig,
                                             &emberAfPluginGpioSensorInterruptEventControl);
  halGenericInterruptControlIrqEnable(irqConfig);

  // Determine the initial value of the sensor
  reedValue = halGenericInterruptControlIrqReadGpio(irqConfig);
  if (SENSOR_IS_ACTIVE_HI) {
    if (reedValue) {
      newSensorStatus = HAL_GPIO_SENSOR_ACTIVE;
      lastSensorStatus = newSensorStatus;
    } else {
      newSensorStatus = HAL_GPIO_SENSOR_NOT_ACTIVE;
      lastSensorStatus = newSensorStatus;
    }
  } else {
    if (reedValue) {
      newSensorStatus = HAL_GPIO_SENSOR_NOT_ACTIVE;
      lastSensorStatus = newSensorStatus;
    } else {
      newSensorStatus = HAL_GPIO_SENSOR_ACTIVE;
      lastSensorStatus = newSensorStatus;
    }
  }
}

// Get the current state of the sensor
HalGpioSensorState halGpioSensorGetSensorValue(void)
{
  return(newSensorStatus);
}
