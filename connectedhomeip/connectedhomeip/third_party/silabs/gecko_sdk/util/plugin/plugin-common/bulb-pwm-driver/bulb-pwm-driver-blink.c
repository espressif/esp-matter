// Copyright 2016 Silicon Laboratories, Inc.                                *80*

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "stack/include/ember-types.h"
#include "event_control/event.h"
#include "hal/hal.h"

#include EMBER_AF_API_BULB_PWM_DRIVER

// **********************************************
// LED Output Blinking State
//
// API for blinking light value for user feedback.

#define pwmBlinkEventControl  emberAfPluginBulbPwmDriverBlinkEventControl

EmberEventControl emberAfPluginBulbPwmDriverBlinkEventControl;
EmberEventControl emberAfPluginBulbPwmDriverStatusEventControl;

enum {
  LED_ON            = 0x00,
  LED_OFF           = 0x01,
  LED_BLINKING_ON   = 0x02,
  LED_BLINKING_OFF  = 0x03,
  LED_BLINK_PATTERN = 0x04,
};

enum {
  BLINK_LED = 0x00,
  BLINK_STATUS = 0x01
};

#define BLINK_CHANNELS             2

#ifdef EMBER_AF_PLUGIN_BULB_PWM_DRIVER_BLINK_PATTERN_MAX_LENGTH
#define BLINK_PATTERN_MAX_LENGTH \
  EMBER_AF_PLUGIN_BULB_PWM_DRIVER_BLINK_PATTERN_MAX_LENGTH
#else
#define BLINK_PATTERN_MAX_LENGTH   20
#endif
#define SECONDS_TO_MILLISECONDS    1000

typedef struct {
  uint8_t state;
  uint8_t count;
  uint16_t blinkTime;

  uint16_t pattern[BLINK_PATTERN_MAX_LENGTH];
  uint8_t patternLength;
  uint8_t patternIndex;

  void (*turnOn)(void);
  void (*turnOff)(void);
  void (*start)(void);
  void (*stop)(void);

  EmberEventControl *eventControl;
} BlinkState;

BlinkState blinkState[BLINK_CHANNELS];

static void turnStatusLedOn(void)
{
  halBulbPwmDriverStatusLedOn( );
}

static void turnStatusLedOff(void)
{
  halBulbPwmDriverStatusLedOff( );
}

static void nullFunction(void)
{
}

static void turnLedOn(void)
{
  halBulbPwmDriverBlinkOnCallback( );
}

static void turnLedOff(void)
{
  halBulbPwmDriverBlinkOffCallback( );
}

static void ledBlinkStop(void)
{
  halBulbPwmDriverBlinkStopCallback( );
}

static void ledBlinkStart(void)
{
  // Indicate that the light is starting a blink pattern.  The application
  // code should therefore not attempt to drive the LEDs.
  halBulbPwmDriverBlinkStartCallback( );
}

static void ledOn(uint8_t time, BlinkState *p)
{
  p->turnOn();
  p->state = LED_ON;

  if (time > 0) {
    emberEventControlSetDelayMS(*(p->eventControl),
                                ((uint32_t) time) * SECONDS_TO_MILLISECONDS);
  } else {
    emberEventControlSetInactive(*(p->eventControl));
  }
}

static void ledOff(uint8_t time, BlinkState *p)
{
  p->turnOff();
  p->state = LED_OFF;

  if (time > 0) {
    emberEventControlSetDelayMS(*(p->eventControl),
                                ((uint32_t) time) * SECONDS_TO_MILLISECONDS);
  } else {
    emberEventControlSetInactive(*(p->eventControl));
  }
}

static void ledBlink(uint8_t count, uint16_t blinkTime, BlinkState *p)
{
  p->blinkTime = blinkTime;

  p->turnOff();
  p->state = LED_BLINKING_OFF;
  emberEventControlSetDelayMS(*(p->eventControl),
                              p->blinkTime);
  p->count = count;
}

// Implements a function to blink an arbitrary pattern on the bulb output.  The
// function receives a count, which is the number of times to cycle through the
// pattern, a length of the pattern, and a short array of 16 bit integer values
// which are interpreted as the blink pattern in milliseconds.  The first value
// is a number of milliseconds for the light to be on, the second is a number
// of milliseconds for the light to be off, and so on until length has been
// reached.  For example, if we wished to create an SOS pattern, we would
// program the following array:
// pattern[20] = {500, 100, 500, 100, 500, 100, 100, 100, 100, 100, 100, 100,
//                500, 100, 500, 100, 500, 100};
// Where the light would be on in a sequences of 500 and 100 mS intervals, and
// the light would be off for 100 mS in between the on intervals.
static void blinkPattern(uint8_t    count,
                         uint8_t    length,
                         uint16_t   *pattern,
                         BlinkState *p)
{
  uint8_t i;

  if (length < 2) {
    return;
  }

  p->turnOn();

  p->state = LED_BLINK_PATTERN;

  if (length > BLINK_PATTERN_MAX_LENGTH) {
    length = BLINK_PATTERN_MAX_LENGTH;
  }

  p->patternLength = length;
  p->count = count;

  for (i = 0; i < p->patternLength; i++) {
    p->pattern[i] = pattern[i];
  }

  emberEventControlSetDelayMS(*(p->eventControl),
                              p->pattern[0]);

  p->patternIndex = 1;
}

void eventHandler(BlinkState *p)
{
  emberEventControlSetInactive(*(p->eventControl));
  switch (p->state) {
    case LED_ON:
      p->turnOff();
      p->stop();
      break;

    case LED_OFF:
      p->turnOn();
      p->stop();
      break;

    case LED_BLINKING_ON:
      p->turnOff();
      if (p->count == 0) {
        p->state = LED_OFF;
        p->stop();

        break;
      }

      if (p->count != HAL_BULB_PWM_DRIVER_BLINK_FOREVER) {
        p->count--;
      }
      if (p->count > 0) {
        p->state = LED_BLINKING_OFF;
        emberEventControlSetDelayMS(*(p->eventControl),
                                    p->blinkTime);
      } else {
        p->state = LED_OFF;
        p->stop();
      }

      break;
    case LED_BLINKING_OFF:
      p->turnOn();
      p->state = LED_BLINKING_ON;
      emberEventControlSetDelayMS(*(p->eventControl),
                                  p->blinkTime);
      break;
    case LED_BLINK_PATTERN:
      if (p->count == 0) {
        p->turnOff();

        p->state = LED_OFF;
        p->stop();

        break;
      }

      if (p->patternIndex % 2 == 1) {
        p->turnOff();
      } else {
        p->turnOn();
      }

      emberEventControlSetDelayMS(*(p->eventControl),
                                  p->pattern[p->patternIndex]);

      p->patternIndex++;

      if (p->patternIndex >= p->patternLength) {
        p->patternIndex = 0;

        if (p->count != HAL_BULB_PWM_DRIVER_BLINK_FOREVER) {
          p->count--;
        }
      }

    default:
      break;
  }
}

void halBulbPwmDriverBlinkInit(void)
{
  blinkState[BLINK_LED].turnOn = turnLedOn;
  blinkState[BLINK_LED].turnOff = turnLedOff;
  blinkState[BLINK_LED].start = ledBlinkStart;
  blinkState[BLINK_LED].stop = ledBlinkStop;
  blinkState[BLINK_LED].eventControl =
    &(emberAfPluginBulbPwmDriverBlinkEventControl);

  blinkState[BLINK_STATUS].turnOn = turnStatusLedOn;
  blinkState[BLINK_STATUS].turnOff = turnStatusLedOff;
  blinkState[BLINK_STATUS].start = nullFunction;
  blinkState[BLINK_STATUS].stop = nullFunction;
  blinkState[BLINK_STATUS].eventControl =
    &(emberAfPluginBulbPwmDriverStatusEventControl);
}

// ******** APIs and Event Functions *************
void emberAfPluginBulbPwmDriverBlinkEventHandler(void)
{
  eventHandler(&(blinkState[BLINK_LED]));
}

void emberAfPluginBulbPwmDriverStatusEventHandler(void)
{
  eventHandler(&(blinkState[BLINK_STATUS]));
}

void halBulbPwmDriverLedBlinkPattern(uint8_t  count,
                                     uint8_t  length,
                                     uint16_t *pattern)
{
  blinkPattern(count, length, pattern, &(blinkState[BLINK_LED]));
}

void halBulbPwmDriverStatusBlinkPattern(uint8_t  count,
                                        uint8_t  length,
                                        uint16_t *pattern)
{
  blinkPattern(count, length, pattern, &(blinkState[BLINK_STATUS]));
}

void halBulbPwmDriverLedBlink(uint8_t count, uint16_t blinkTime)
{
  ledBlink(count, blinkTime, &(blinkState[BLINK_LED]));
}

void halBulbPwmDriverStatusBlink(uint8_t count, uint16_t blinkTime)
{
  ledBlink(count, blinkTime, &(blinkState[BLINK_STATUS]));
}

void halBulbPwmDriverLedOff(uint8_t time)
{
  ledOff(time, &(blinkState[BLINK_LED]));
}

void halBulbPwmDriverStatusOff(uint8_t time)
{
  ledOff(time, &(blinkState[BLINK_STATUS]));
}

void halBulbPwmDriverLedOn(uint8_t time)
{
  ledOn(time, &(blinkState[BLINK_LED]));
}

void halBulbPwmDriverStatusOn(uint8_t time)
{
  ledOn(time, &(blinkState[BLINK_STATUS]));
}
