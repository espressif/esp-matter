// Copyright 2016 Silicon Laboratories, Inc.                                *80*

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "stack/include/ember-types.h"
#include "event_control/event.h"
#include "hal/hal.h"

#include EMBER_AF_API_BULB_PWM_DRIVER

// EFR headers
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_timer.h"

// ---------- MACRO DEFINITIONS ----------
#if HAL_BULBPWM_ENABLE == 1

// ---------- HWCONFIG Section -----------
// To support HWCONFIG, we needed to change the names of different macros.  We
// also needed to define certain settings based on which channels are available.
// Note:  in order to simplify the code, we decided to assume white will be on
// timer 0 channel 0, lowtemp will be on timer 0 channel 1, and so forth.
// This doesn't impose a huge constraint, but it greatly simplifies the code.

#define BULB_PWM_USING_TIMER0

#define BULB_PWM_WHITE_TIMER      BSP_BULBPWM_TIMER
#define BULB_PWM_WHITE_CHANNEL    0
#define BULB_PWM_LOWTEMP_TIMER    BSP_BULBPWM_TIMER
#define BULB_PWM_LOWTEMP_CHANNEL  1
#define BULB_PWM_STATUS_TIMER     BSP_BULBPWM_TIMER
#define BULB_PWM_STATUS_CHANNEL   2

#if HAL_BULBPWM_COLOR_ENABLE == 1
  #define BULB_PWM_USING_TIMER1
  #define BULB_PWM_RED_TIMER        BSP_BULBPWM_COLOR_TIMER
  #define BULB_PWM_RED_CHANNEL      0
  #define BULB_PWM_GREEN_TIMER      BSP_BULBPWM_COLOR_TIMER
  #define BULB_PWM_GREEN_CHANNEL    1
  #define BULB_PWM_BLUE_TIMER       BSP_BULBPWM_COLOR_TIMER
  #define BULB_PWM_BLUE_CHANNEL     2
  #define BULB_PWM_AMBER_TIMER      BSP_BULBPWM_COLOR_TIMER
  #define BULB_PWM_AMBER_CHANNEL    3
#endif

// ---------- End of HWCONFIG Section -----------

#define CLOCK_FREQUENCY         38400000

#if defined(EMBER_AF_PLUGIN_PWM_CONTROL_PWM_POLARITY)
#if EMBER_AF_PLUGIN_PWM_CONTROL_PWM_POLARITY == 0
#define PWM_INVERT_OUTPUT       true
#else
#define PWM_INVERT_OUTPUT       false
#endif
#else
#define PWM_INVERT_OUTPUT       false
#endif

#define TIMER_DEFAULT_INIT            \
  {                                   \
    .eventCtrl = timerEventEveryEdge, \
    .edge = timerEdgeBoth,            \
    .prsSel = timerPRSSELCh0,         \
    .cufoa = timerOutputActionNone,   \
    .cofoa = timerOutputActionNone,   \
    .cmoa = timerOutputActionToggle,  \
    .mode = timerCCModePWM,           \
    .filter = false,                  \
    .prsInput = false,                \
    .coist = false,                   \
    .outInvert = PWM_INVERT_OUTPUT,   \
  }

// ---------- GLOBAL VARIABLES ----------
static uint16_t ticksPerPeriod;
static uint16_t pwmFrequency;

// ---------- Predeclared callbacks ----------
uint16_t halBulbPwmDriverFrequencyCallback(void);
void halBulbPwmDriverInitCompleteCallback(void);

// Funciton to initialize the blink code datastructures.
void halBulbPwmDriverBlinkInit(void);

// ----------- function definitions
static void initGpio(void)
{
  // GPIO setup
  CMU_ClockEnable(cmuClock_GPIO, true);

#if HAL_BULBPWM_WHITE_ENABLE == 1
  GPIO_PinModeSet(BSP_BULBPWM_CC0_PORT,
                  BSP_BULBPWM_CC0_PIN,
                  gpioModePushPull,
                  0);
#endif
#if HAL_BULBPWM_LOWTEMP_ENABLE == 1
  GPIO_PinModeSet(BSP_BULBPWM_CC1_PORT,
                  BSP_BULBPWM_CC1_PIN,
                  gpioModePushPull,
                  0);
#endif
#if HAL_BULBPWM_STATUS_ENABLE == 1
  GPIO_PinModeSet(BSP_BULBPWM_CC2_PORT,
                  BSP_BULBPWM_CC2_PIN,
                  gpioModePushPull,
                  0);
#endif
#if HAL_BULBPWM_RED_ENABLE == 1
  GPIO_PinModeSet(BSP_BULBPWM_COLOR_CC0_PORT,
                  BSP_BULBPWM_COLOR_CC0_PIN,
                  gpioModePushPull,
                  0);
#endif
#if HAL_BULBPWM_GREEN_ENABLE == 1
  GPIO_PinModeSet(BSP_BULBPWM_COLOR_CC1_PORT,
                  BSP_BULBPWM_COLOR_CC1_PIN,
                  gpioModePushPull,
                  0);
#endif
#if HAL_BULBPWM_BLUE_ENABLE == 1
  GPIO_PinModeSet(BSP_BULBPWM_COLOR_CC2_PORT,
                  BSP_BULBPWM_COLOR_CC2_PIN,
                  gpioModePushPull,
                  0);
#endif
#if HAL_BULBPWM_AMBER_ENABLE == 1
  GPIO_PinModeSet(BSP_BULBPWM_COLOR_CC3_PORT,
                  BSP_BULBPWM_COLOR_CC3_PIN,
                  gpioModePushPull,
                  0);
#endif
}

static void initFrequency(void)
{
  TIMER_InitCC_TypeDef timerCCInit = TIMER_DEFAULT_INIT;

  /* Select timer parameters */
  TIMER_Init_TypeDef timerInit = {
    .enable = true,
    .debugRun = true,
    .prescale = timerPrescale1,
    .clkSel = timerClkSelHFPerClk,
    .fallAction = timerInputActionNone,
    .riseAction = timerInputActionNone,
    .mode = timerModeUp,
    .dmaClrAct = false,
    .quadModeX4 = false,
    .oneShot = false,
    .sync = false,
  };

  uint32_t ticksPerPeriod32;

  pwmFrequency = halBulbPwmDriverFrequencyCallback();

  if (pwmFrequency == HAL_BULB_PWM_DRIVER_USE_DEFAULT_FREQUENCY) {
    pwmFrequency = HAL_BULBPWM_FREQUENCY;
  }

  ticksPerPeriod32 = (uint32_t) CLOCK_FREQUENCY;
  ticksPerPeriod32 /= (uint32_t) pwmFrequency;

  ticksPerPeriod = (uint16_t) ticksPerPeriod32;

#ifdef BULB_PWM_USING_TIMER0
  CMU_ClockEnable(cmuClock_TIMER0, true);

#if HAL_BULBPWM_WHITE_ENABLE == 1
  TIMER_InitCC(TIMER0, 0, &timerCCInit);
  TIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;
  TIMER0->ROUTELOC0 |= (BSP_BULBPWM_CC0_LOC << _TIMER_ROUTELOC0_CC0LOC_SHIFT);
  TIMER_CompareBufSet(TIMER0, 0, 0);
#endif

#if HAL_BULBPWM_LOWTEMP_ENABLE == 1
  TIMER_InitCC(TIMER0, 1, &timerCCInit);
  TIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC1PEN;
  TIMER0->ROUTELOC0 |= (BSP_BULBPWM_CC1_LOC << _TIMER_ROUTELOC0_CC1LOC_SHIFT);
  TIMER_CompareBufSet(TIMER0, 1, 0);
#endif

#if HAL_BULBPWM_STATUS_ENABLE == 1
  TIMER_InitCC(TIMER0, 2, &timerCCInit);
  TIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC2PEN;
  TIMER0->ROUTELOC0 |= (BSP_BULBPWM_CC2_LOC << _TIMER_ROUTELOC0_CC2LOC_SHIFT);
  TIMER_CompareBufSet(TIMER0, 2, 0);
#endif

  // Set Top Value
  TIMER_TopSet(TIMER0, ticksPerPeriod);

  // Set compare value starting at 0 - it will be incremented in the interrupt
  // handler
  TIMER_Init(TIMER0, &timerInit);
#endif

#ifdef BULB_PWM_USING_TIMER1
  CMU_ClockEnable(cmuClock_TIMER1, true);

#if HAL_BULBPWM_RED_ENABLE == 1
  TIMER_InitCC(TIMER1, 0, &timerCCInit);
  TIMER1->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;
  TIMER1->ROUTELOC0 |= (BSP_BULBPWM_COLOR_CC0_LOC << _TIMER_ROUTELOC0_CC0LOC_SHIFT);
  TIMER_CompareBufSet(TIMER1, 0, 0);
#endif

#if HAL_BULBPWM_GREEN_ENABLE == 1
  TIMER_InitCC(TIMER1, 1, &timerCCInit);
  TIMER1->ROUTEPEN |= TIMER_ROUTEPEN_CC1PEN;
  TIMER1->ROUTELOC0 |= (BSP_BULBPWM_COLOR_CC1_LOC << _TIMER_ROUTELOC0_CC1LOC_SHIFT);
  TIMER_CompareBufSet(TIMER1, 1, 0);
#endif

#if HAL_BULBPWM_BLUE_ENABLE == 1
  TIMER_InitCC(TIMER1, 2, &timerCCInit);
  TIMER1->ROUTEPEN |= TIMER_ROUTEPEN_CC2PEN;
  TIMER1->ROUTELOC0 |= (BSP_BULBPWM_COLOR_CC2_LOC << _TIMER_ROUTELOC0_CC2LOC_SHIFT);
  TIMER_CompareBufSet(TIMER1, 2, 0);
#endif
#if HAL_BULBPWM_AMBER_ENABLE == 1
  TIMER_InitCC(TIMER1, 3, &timerCCInit);
  TIMER1->ROUTEPEN |= TIMER_ROUTEPEN_CC3PEN;
  TIMER1->ROUTELOC0 |= (BSP_BULBPWM_COLOR_CC3_LOC << _TIMER_ROUTELOC0_CC3LOC_SHIFT);
  TIMER_CompareBufSet(TIMER1, 3, 0);
#endif
  // Set Top Value
  TIMER_TopSet(TIMER1, ticksPerPeriod);

  // Set compare value starting at 0 - it will be incremented in the interrupt
  // handler
  TIMER_Init(TIMER1, &timerInit);
#endif
}

void halBulbPwmDriverInitialize(void)
{
  initGpio();

  initFrequency();

  halBulbPwmDriverBlinkInit();

  halBulbPwmDriverInitCompleteCallback();
}

uint16_t halBulbPwmDriverTicksPerPeriod(void)
{
  return ticksPerPeriod;
}

uint16_t halBulbPwmDriverTicksPerMicrosecond(void)
{
  return (uint16_t) (CLOCK_FREQUENCY / 1000000);
}

void halBulbPwmDriverSetPwmLevel(uint16_t value, uint8_t pwm)
{
  switch (pwm) {
#if HAL_BULBPWM_WHITE_ENABLE == 1
    case HAL_BULBPWM_WHITE_ID:
      TIMER_CompareBufSet(BULB_PWM_WHITE_TIMER, BULB_PWM_WHITE_CHANNEL, value);
      break;
#endif
#if HAL_BULBPWM_LOWTEMP_ENABLE == 1
    case HAL_BULBPWM_LOWTEMP_ID:
      TIMER_CompareBufSet(BULB_PWM_LOWTEMP_TIMER, BULB_PWM_LOWTEMP_CHANNEL,
                          value);
      break;
#endif
#if HAL_BULBPWM_STATUS_ENABLE == 1
    case HAL_BULBPWM_STATUS_ID:
      TIMER_CompareBufSet(BULB_PWM_STATUS_TIMER, BULB_PWM_STATUS_CHANNEL, value);
      break;
#endif
#if HAL_BULBPWM_RED_ENABLE == 1
    case HAL_BULBPWM_RED_ID:
      TIMER_CompareBufSet(BULB_PWM_RED_TIMER, BULB_PWM_RED_CHANNEL, value);
      break;
#endif
#if HAL_BULBPWM_GREEN_ENABLE == 1
    case HAL_BULBPWM_GREEN_ID:
      TIMER_CompareBufSet(BULB_PWM_GREEN_TIMER, BULB_PWM_GREEN_CHANNEL, value);
      break;
#endif
#if HAL_BULBPWM_BLUE_ENABLE == 1
    case HAL_BULBPWM_BLUE_ID:
      TIMER_CompareBufSet(BULB_PWM_BLUE_TIMER, BULB_PWM_BLUE_CHANNEL, value);
      break;
#endif
#if HAL_BULBPWM_AMBER_ENABLE == 1
    case HAL_BULBPWM_AMBER_ID:
      TIMER_CompareBufSet(BULB_PWM_AMBER_TIMER, BULB_PWM_AMBER_CHANNEL, value);
      break;
#endif
    default:
      assert(0);
      break;
  }
}

// Required for AFv2 support
void emberAfPluginBulbPwmDriverInitCallback()
{
  halBulbPwmDriverInitialize();
}

void halBulbPwmDriverStatusLedOn(void)
{
#if HAL_BULBPWM_STATUS_ENABLE == 1
  halBulbPwmDriverSetPwmLevel(halBulbPwmDriverTicksPerPeriod(),
                              HAL_BULBPWM_STATUS_ID);
#endif
}

void halBulbPwmDriverStatusLedOff(void)
{
#if HAL_BULBPWM_STATUS_ENABLE == 1
  halBulbPwmDriverSetPwmLevel(0, HAL_BULBPWM_STATUS_ID);
#endif
}

#else // HAL_BULBPWM_ENABLE
  #error "Must HAL_BULBPWM_ENABLE and set it to 1 to use bulb pwm plugin."
#endif
