// *****************************************************************************
// * led-blink.c
// *
// * API to allow for basic control of activity LEDs, including on/off
// * functionality and blink pattern creation.
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "stack/include/ember-types.h"
#include "event_control/event.h"
#include "hal/hal.h"
#include "hal/micro/led.h"
#include "hal/micro/micro.h"
#include EMBER_AF_API_LED_BLINK

// ------------------------------------------------------------------------------
// Private plugin macros

// Length restriction for LED pattern
#define MAX_BLINK_PATTERN_LENGTH  20

// ------------------------------------------------------------------------------
// Plugin events
EmberEventControl emberAfPluginLedBlinkLed0EventFunctionEventControl;
EmberEventControl emberAfPluginLedBlinkLed1EventFunctionEventControl;

EmberEventControl * ledEventArray[BSP_LED_COUNT] = {
#if BSP_LED_COUNT >= 1
  &emberAfPluginLedBlinkLed0EventFunctionEventControl,
#endif
#if BSP_LED_COUNT >= 2
  &emberAfPluginLedBlinkLed1EventFunctionEventControl
#endif
};

// ------------------------------------------------------------------------------
// Plugin private types and enums
typedef enum {
  LED_ON = 0x00,
  LED_OFF = 0x01,
  LED_BLINKING_ON = 0x02,
  LED_BLINKING_OFF = 0x03,
  LED_BLINK_PATTERN = 0x04,
} gpioBlinkState;

// ------------------------------------------------------------------------------
// Forward declaration of private plugin functions
static void turnLedOn(uint8_t led);
static void turnLedOff(uint8_t led);
static uint8_t ledLookup(uint8_t led);
static void handleLedEvent(uint8_t ledIndex);

// ------------------------------------------------------------------------------
// Plugin private global variables
#if BSP_LED_COUNT == 2
static gpioBlinkState ledEventState[BSP_LED_COUNT] = { LED_ON, LED_ON };
static uint8_t ledBlinkCount[BSP_LED_COUNT] = { 0x00, 0x00 };
static uint8_t activeLed[BSP_LED_COUNT] = HAL_LED_ENABLE;
#elif BSP_LED_COUNT == 1
static gpioBlinkState ledEventState[BSP_LED_COUNT] = { LED_ON };
static uint8_t ledBlinkCount[BSP_LED_COUNT] = { 0x00 };
static uint8_t activeLed[BSP_LED_COUNT] = HAL_LED_ENABLE;
#endif
static uint16_t ledBlinkTimeMs[BSP_LED_COUNT];
static uint16_t blinkPattern[BSP_LED_COUNT][MAX_BLINK_PATTERN_LENGTH];
static uint8_t blinkPatternLength[BSP_LED_COUNT];
static uint8_t blinkPatternIndex[BSP_LED_COUNT];
static uint8_t ledSequence;

// ------------------------------------------------------------------------------
// Plugin registered callback implementations

// ------------------------------------------------------------------------------
// Plugin led0 event handler
void emberAfPluginLedBlinkLed0EventFunctionEventHandler(void)
{
  handleLedEvent(0);
}

// Plugin led1 event handler
void emberAfPluginLedBlinkLed1EventFunctionEventHandler(void)
{
  handleLedEvent(1);
}

static void handleLedEvent(uint8_t ledIndex)
{
  // Verify that this event never tries to interact with an LED that has no
  // allocated array entries

  if (ledIndex < BSP_LED_COUNT) {
    switch (ledEventState[ledIndex]) {
      // The API to turn the light on can be used to either change the LED state to
      // a permanent on state, or to flash the LED on temporarily after which time
      // the LED will automatically turn off. This is achieved by specifying a
      // timeout for the on or off state. If you specify 0 for the timeout, the API
      // will permanently turn the LED on. If you specify a non-zero timeout, the
      // LED is immediately turned on, and the event is scheduled in the future
      // based on the timeout. Hence, if we hit this event and the current LED state
      // is ON, we need to turn it off.
      // The same is true for the LED_OFF state, only in the timeout event we need
      // to turn the LED on.
      case LED_ON:
        // was on.  this must be time to turn it off.
        turnLedOff(activeLed[ledIndex]);
        emberEventControlSetInactive(*(ledEventArray[ledIndex]));
        break;

      case LED_OFF:
        // was on.  this must be time to turn it off.
        turnLedOn(activeLed[ledIndex]);
        emberEventControlSetInactive(*(ledEventArray[ledIndex]));
        break;

      case LED_BLINKING_ON:
        turnLedOff(activeLed[ledIndex]);
        if (ledBlinkCount[ledIndex] > 0) {
          if (ledBlinkCount[ledIndex] != 255) { // blink forever if count is 255
            ledBlinkCount[ledIndex]--;
          }
          if (ledBlinkCount[ledIndex] > 0) {
            ledEventState[ledIndex] = LED_BLINKING_OFF;
            emberEventControlSetDelayMS(*(ledEventArray[ledIndex]),
                                        ledBlinkTimeMs[ledIndex]);
          } else {
            ledEventState[ledIndex] = LED_OFF;
            emberEventControlSetInactive(*(ledEventArray[ledIndex]));
          }
        } else {
          ledEventState[ledIndex] = LED_BLINKING_OFF;
          emberEventControlSetDelayMS(*(ledEventArray[ledIndex]),
                                      ledBlinkTimeMs[ledIndex]);
        }
        break;
      case LED_BLINKING_OFF:
        turnLedOn(activeLed[ledIndex]);
        ledEventState[ledIndex] = LED_BLINKING_ON;
        emberEventControlSetDelayMS(*(ledEventArray[ledIndex]),
                                    ledBlinkTimeMs[ledIndex]);
        break;
      case LED_BLINK_PATTERN:
        if (ledBlinkCount[ledIndex] == 0) {
          turnLedOff(activeLed[ledIndex]);

          ledEventState[ledIndex] = LED_OFF;
          emberEventControlSetInactive(*(ledEventArray[ledIndex]));

          break;
        }

        if (blinkPatternIndex[ledIndex] % 2 == 1) {
          turnLedOff(activeLed[ledIndex]);
        } else {
          turnLedOn(activeLed[ledIndex]);
        }

        emberEventControlSetDelayMS(*(ledEventArray[ledIndex]),
                                    blinkPattern[ledIndex][blinkPatternIndex[
                                                             ledIndex]]);

        blinkPatternIndex[ledIndex]++;

        if (blinkPatternIndex[ledIndex] >= blinkPatternLength[ledIndex]) {
          blinkPatternIndex[ledIndex] = 0;
          if (ledBlinkCount[ledIndex] != 255) { // blink forever if count is 255
            ledBlinkCount[ledIndex]--;
          }
        }
        break;
      default:
        break;
    }
  } else {
    assert(false);
  }
}

// ------------------------------------------------------------------------------
// Plugin public API function implementations

void halLedBlinkLedOn(uint8_t timeMs)
{
  halMultiLedBlinkLedOn(timeMs, activeLed[0]);
}

void halLedBlinkLedOff(uint8_t timeMs)
{
  halMultiLedBlinkLedOff(timeMs, activeLed[0]);
}

void halLedBlinkBlink(uint8_t count, uint16_t blinkTimeMs)
{
  halMultiLedBlinkBlink(count, blinkTimeMs, activeLed[0]);
}

void halLedBlinkPattern(uint8_t count, uint8_t length, uint16_t *pattern)
{
  halMultiLedBlinkPattern(count, length, pattern, activeLed[0]);
}

void halLedBlinkSetActivityLed(uint8_t led)
{
  activeLed[0] = (uint8_t)led;
}

void halMultiLedBlinkLedOn(uint8_t timeMs, uint8_t led)
{
  uint8_t ledIndex;

  ledIndex = ledLookup(led);
  turnLedOn(activeLed[ledIndex]);
  ledEventState[ledIndex] = LED_ON;

  if (timeMs > 0) {
    emberEventControlSetDelayQS(*(ledEventArray[ledIndex]),
                                ((uint16_t) timeMs) * 4);
  } else {
    emberEventControlSetInactive(*(ledEventArray[ledIndex]));
  }
}

void halMultiLedBlinkLedOff(uint8_t timeMs, uint8_t led)
{
  uint8_t ledIndex;

  ledIndex = ledLookup(led);
  turnLedOff(activeLed[ledIndex]);
  ledEventState[ledIndex] = LED_OFF;

  if (timeMs > 0) {
    emberEventControlSetDelayQS(*(ledEventArray[ledIndex]),
                                ((uint16_t) timeMs) * 4);
  } else {
    emberEventControlSetInactive(*(ledEventArray[ledIndex]));
  }
}

void halMultiLedBlinkBlink(uint8_t  count,
                           uint16_t blinkTimeMs,
                           uint8_t  led)
{
  uint8_t ledIndex;

  ledIndex = ledLookup(led);
  ledBlinkTimeMs[ledIndex] = blinkTimeMs;
  turnLedOff(activeLed[ledIndex]);
  ledEventState[ledIndex] = LED_BLINKING_OFF;
  emberEventControlSetDelayMS(*(ledEventArray[ledIndex]),
                              ledBlinkTimeMs[ledIndex]);
  ledBlinkCount[ledIndex] = count;
}

void halMultiLedBlinkPattern(uint8_t  count,
                             uint8_t  length,
                             uint16_t *pattern,
                             uint8_t  led)
{
  uint8_t i, ledIndex;

  ledIndex = ledLookup(led);

  if (length < 2) {
    return;
  }

  turnLedOn(activeLed[ledIndex]);

  ledEventState[ledIndex] = LED_BLINK_PATTERN;

  if (length > MAX_BLINK_PATTERN_LENGTH) {
    length = MAX_BLINK_PATTERN_LENGTH;
  }

  blinkPatternLength[ledIndex] = length;
  ledBlinkCount[ledIndex] = count;

  for (i = 0; i < blinkPatternLength[ledIndex]; i++) {
    blinkPattern[ledIndex][i] = pattern[i];
  }

  emberEventControlSetDelayMS(*(ledEventArray[ledIndex]),
                              blinkPattern[ledIndex][0]);

  blinkPatternIndex[ledIndex] = 1;
}

void halMultiLedBlinkSetActivityLeds(uint8_t led)
{
  uint8_t i;

  for (i = 0; i < ledSequence; i++) {
    if (activeLed[i] == led) {
      return;
    }
  }
  activeLed[ledSequence] = (uint8_t)led;
  ledSequence++;

  if (ledSequence == BSP_LED_COUNT) {
    ledSequence = 0;
  }
}

// ------------------------------------------------------------------------------
// Plugin private function implementations

// *****************************************************************************
// function to set the GPIO and maintain the state during sleep.
// Port is 0 for port a, 1 for port b, and 2 for port c.
void halLedBlinkSleepySetGpio(uint8_t port, uint8_t pin)
{
  GPIO_PinOutSet((GPIO_Port_TypeDef)port, pin);
}

// *****************************************************************************
// function to clear the GPIO and maintain the state during sleep.
// Port is 0 for port a, 1 for port b, and 2 for port c.
void halLedBlinkSleepyClearGpio(uint8_t port, uint8_t pin)
{
  GPIO_PinOutClear((GPIO_Port_TypeDef)port, pin);
}

// *****************************************************************************
// Helper function to lookup which led to be acted on
static uint8_t ledLookup(uint8_t led)
{
  uint8_t i, ledIndex = 0;

  for (i = 0; i < BSP_LED_COUNT; i++) {
    if (led == activeLed[i]) {
      ledIndex = i;
      break;
    }
  }
  return ledIndex;
}

// *****************************************************************************
// Drive the LED for a GPIO high and update sleepy state
static void turnLedOn(uint8_t led)
{
#ifdef LED_ACTIVE_HIGH
  halSetLed((HalBoardLed)led);
#else
  halClearLed((HalBoardLed)led);
#endif // LED_ACTIVE_HIGH
}

// *****************************************************************************
// Drive the LED for a GPIO low and update sleepy state
static void turnLedOff(uint8_t led)
{
#ifdef LED_ACTIVE_HIGH
  halClearLed((HalBoardLed)led);
#else
  halSetLed((HalBoardLed)led);
#endif // LED_ACTIVE_HIGH
}
