/**
 * @file
 * Actuator module implementation
 *
 * @copyright 2020 Silicon Laboratories Inc.
 */

#include "ZAF_Actuator.h"

//#define DEBUGPRINT
#include <DebugPrint.h>
#include <Assert.h>
#include <stdlib.h>
#include <AppTimer.h>

// valueCurrent, valueTarget and singleStepValue are saved as 10 times higher.
// Use these defines as getters/setters
#define CONVERT_TO_INTERNAL(x) (10 * (x))
#define CONVERT_FROM_INTERNAL(x) ((x)/10)

static void ZAF_Actuator_TimerExpired(SSwTimer *timer);
static void initiateTimedChange(s_Actuator *pActuator, uint8_t duration);
static inline void triggerCCCallback(s_Actuator *pAct);
static inline void updateCurrentValue(s_Actuator *pActuator);
static inline void updateLastOnValue(s_Actuator *pActuator);

void ZAF_Actuator_Init(s_Actuator *pActuator,
                          uint8_t minValue,
                          uint8_t maxValue,
                          uint16_t refreshRate,
                          uint8_t durationDefault,
                          zaf_actuator_callback_t cc_callback)
{
  ASSERT(NULL != pActuator);
  DPRINTF("%s min:%u, max:%u, durationDefault:%u, refreshRate:%u\n",
          __func__, minValue, maxValue, durationDefault, refreshRate);

  if (minValue > maxValue) {
    DPRINTF("Minimum %X must be less than maximum: %X\n", minValue, maxValue);
    ASSERT(minValue < maxValue);
  }
  pActuator->min = minValue;
  pActuator->max = maxValue;

  if (!refreshRate) {
    DPRINTF("Refresh rate must be greater than 0: %x\n", refreshRate);
    ASSERT(refreshRate);
  }
  if (20 > refreshRate)
  {
    // Refresh rates lower than 20 causes watchdog reset. Set to 20.
    refreshRate = 20;
    ASSERT(false);
  }
  pActuator->defaultRefreshRate = refreshRate;
  pActuator->refreshRate = pActuator->defaultRefreshRate;
  if (durationDefault == 0xFF)
  {
    DPRINTF("Invalid duration: %x\n", durationDefault);
    ASSERT(durationDefault != 0xFF);
  }
  pActuator->durationDefault = durationDefault;

  AppTimerRegister(&pActuator->timer, true, &ZAF_Actuator_TimerExpired);
  pActuator->timer.ptr = pActuator;
  pActuator->singleStepValue = CONVERT_TO_INTERNAL(1);
  if (NULL != cc_callback) {
    pActuator->cc_cb = cc_callback;
  }
  pActuator->valueCurrent = 0;
  pActuator->valueTarget = 0;
  pActuator->lastOnValue = pActuator->max; // We set lastOnValue to max just to have some initial value turn on at
}

eActuatorState ZAF_Actuator_Set(s_Actuator *pActuator, uint8_t value, uint8_t duration)
{
  ASSERT(NULL != pActuator);
  DPRINTF("%s target value: %X, duration: %X, currentValue=%X\n", __func__, value, duration, pActuator->valueCurrent);

  if (value < pActuator->min || value > pActuator->max) {
    DPRINTF("Invalid value %X. It must be between %X and %X\n", value, pActuator->min, pActuator->max);
    return EACTUATOR_FAILED;
  }
  if (value == ZAF_Actuator_GetCurrentValue(pActuator)) {
    DPRINTF("Already at target value %X, done\n", pActuator->valueCurrent);
    return EACTUATOR_NOT_CHANGING;
  }
  pActuator->valueTarget = CONVERT_TO_INTERNAL(value);

  if ((0 == duration) || (0xFF == duration && 0 == pActuator->durationDefault)) {
    DPRINTF("Instant change from %x to %x\n", CONVERT_FROM_INTERNAL(pActuator->valueCurrent),
                                              CONVERT_FROM_INTERNAL(pActuator->valueTarget));
    pActuator->valueCurrent = pActuator->valueTarget;
    updateLastOnValue(pActuator);
    triggerCCCallback(pActuator);
    return EACTUATOR_NOT_CHANGING;
  } else {
    pActuator->directionUp = pActuator->valueTarget > pActuator->valueCurrent;
    DPRINTF("directionUp = %X\n", pActuator->directionUp);

    initiateTimedChange(pActuator, duration);
    return EACTUATOR_CHANGING;
  }
}

eActuatorState ZAF_Actuator_StartChange(s_Actuator *pActuator,
                                        bool ignoreStartLevel,
                                        bool upDown,
                                        uint8_t startLevel,
                                        uint8_t duration)
{
  ASSERT(NULL != pActuator);
  DPRINTF("%s ignoreStartLevel:%s, %s, startLevel:%X duration:%X, currentValue:%X\n",
          __func__, ignoreStartLevel ? "True":"False", upDown ? "Down":"Up", startLevel, duration, pActuator->valueCurrent);

  pActuator->directionUp = !upDown;
  pActuator->valueTarget = (pActuator->directionUp) ? pActuator->max : pActuator->min;
  pActuator->valueTarget = CONVERT_TO_INTERNAL(pActuator->valueTarget);

  if (!ignoreStartLevel) {
    if (startLevel < pActuator->min || startLevel > pActuator->max) {
      DPRINTF("Invalid start level %X. It must be between %X and %X\n", startLevel, pActuator->min, pActuator->max);
      return EACTUATOR_FAILED;
    }
    pActuator->valueCurrent = CONVERT_TO_INTERNAL(startLevel);
    updateLastOnValue(pActuator);
    // valueTarget must be calculated before triggering CC callback,
    // because caller of Actuator compares current and target value to check is there any change happening at the moment.
    triggerCCCallback(pActuator);
  }
  if (pActuator->valueCurrent == pActuator->valueTarget) {
    DPRINTF("Already at target value %X, done\n", CONVERT_FROM_INTERNAL(pActuator->valueCurrent));
    return EACTUATOR_NOT_CHANGING;
  }

  if ((0 == duration) || (0xFF == duration && 0 == pActuator->durationDefault)) {
    DPRINTF("Instant change from %x to %x\n", CONVERT_FROM_INTERNAL(pActuator->valueCurrent),
                                              CONVERT_FROM_INTERNAL(pActuator->valueTarget));
    pActuator->valueCurrent = pActuator->valueTarget;
    triggerCCCallback(pActuator);
    return EACTUATOR_NOT_CHANGING;
  }
  DPRINTF("Going %s to targetValue=%u\n", (pActuator->directionUp)?"UP":"DOWN",
                                          CONVERT_FROM_INTERNAL(pActuator->valueTarget));
  initiateTimedChange(pActuator, duration);
  return EACTUATOR_CHANGING;
}

eActuatorState ZAF_Actuator_StopChange(s_Actuator *pActuator)
{
  ASSERT(NULL != pActuator);
  if (TimerIsActive(&pActuator->timer)) {
    TimerStop(&pActuator->timer);
    updateLastOnValue(pActuator);
    // From outside of Actuator module, the only way to check if there is ongoing change is to compare target and
    // current value. So update target value to indicate that actuator is in its final state
    pActuator->valueTarget = pActuator->valueCurrent;
    return EACTUATOR_CHANGING; //Change was in progress, inform about that.
  }
  return EACTUATOR_NOT_CHANGING;
}

uint8_t ZAF_Actuator_GetCurrentValue(s_Actuator *pActuator)
{
  ASSERT(NULL != pActuator);
  // current value is internally saved as 10 times bigger, to get more accurate transition.
  return (uint8_t)CONVERT_FROM_INTERNAL(pActuator->valueCurrent);
}

uint8_t ZAF_Actuator_GetTargetValue(s_Actuator *pActuator)
{
  ASSERT(NULL != pActuator);
  return (uint8_t)CONVERT_FROM_INTERNAL(pActuator->valueTarget);
}

uint8_t ZAF_Actuator_GetDurationRemaining(s_Actuator *pActuator)
{
  ASSERT(NULL != pActuator);
  uint32_t duration = 0;
  uint16_t difference = (uint16_t)abs(pActuator->valueTarget - pActuator->valueCurrent);

  if (difference > 0) {
    if (0 == pActuator->singleStepValue) {
      // singleStepValue unknown at the moment. Shouldn't ever happen if change is in progress
      DPRINTF("%s WARNING: step size unknown\n", __func__);
      ASSERT(pActuator->singleStepValue);
    }
    // duration in milliseconds = (numberOfSteps * refreshRate)
    duration = ((difference * pActuator->refreshRate) / pActuator->singleStepValue);
    duration /= 1000; // Convert from MS to seconds
    if (duration > 127) {
      /* According to CC:0000.00.00.11.016 in SDS13781, if duration is up to 127 seconds,
       * unit is second. Above that, convert to minutes */
      duration  = duration / 60 + 0x7F;
    }
    DPRINTF("%s duration Coded value= %#X\n", __func__, duration);
  }
  return (uint8_t)duration;
}

uint32_t getDurationInMs(uint8_t duration)
{
  uint32_t durationMs = 0;
  if (duration > 0 && duration <= 0x7F) {
    DPRINT("Duration in seconds ");
    durationMs = duration * 1000;
  } else if (duration > 0x7F && duration <= 0xFE) {
    DPRINT("Duration in minutes ");
    durationMs = (uint32_t)((duration-0x7F) * 60 * 1000);
  }
  DPRINTF("= %uMS = %u sec (%X)\n", durationMs, durationMs/1000, duration);
  return durationMs;
}

uint8_t ZAF_Actuator_GetLastOnValue(s_Actuator *pActuator)
{
  ASSERT(NULL != pActuator);
  return pActuator->lastOnValue;
}

uint8_t ZAF_Actuator_GetMax(s_Actuator *pActuator)
{
  ASSERT(NULL != pActuator);
  return pActuator->max;
}

/**
 * Internal function for Actuator module. Triggered when timer expires.
 * Updated values of Actuator object and informs CC about latest state
 *
 * @param timer Timer defined in Actuator
 */
static void ZAF_Actuator_TimerExpired(SSwTimer *timer)
{
  s_Actuator *pActuator = timer->ptr;

  /* Target value has been reached */
  if(pActuator->valueCurrent == pActuator->valueTarget)
  {
    TimerStop(timer);
  }
  else
  {
    updateCurrentValue(pActuator);
    triggerCCCallback(pActuator);
  }
}

static void initiateTimedChange(s_Actuator *pActuator, uint8_t duration)
{
  if (0xFF == duration) {
    duration = pActuator->durationDefault;
    DPRINTF("Using factory default duration  = %u sec\n", duration);
  }
  uint32_t numberOfSteps = (duration *1000) / pActuator->defaultRefreshRate;
  uint16_t maxNumberOfSteps = (uint16_t)abs(pActuator->valueTarget - pActuator->valueCurrent);
  maxNumberOfSteps = CONVERT_FROM_INTERNAL(maxNumberOfSteps);
  if (numberOfSteps > maxNumberOfSteps) {
    DPRINTF("Adjust numberOfSteps from %u to %u\n", numberOfSteps, maxNumberOfSteps);
    numberOfSteps = maxNumberOfSteps;
    uint32_t durationMs = getDurationInMs(duration);
    pActuator->refreshRate = durationMs / numberOfSteps; // Find max possible value of refreshRate
    if (20 > pActuator->refreshRate) {
      // Refresh rates lower than 20 causes watchdog reset. Set to 20.
      pActuator->refreshRate = 20;
    }
    DPRINTF("Using RefreshRate %u = %u/%u\n", pActuator->refreshRate, durationMs, numberOfSteps);
    pActuator->singleStepValue = CONVERT_TO_INTERNAL(1);
  } else {
    pActuator->singleStepValue = (uint16_t)((uint32_t)abs(pActuator->valueTarget - pActuator->valueCurrent)/ numberOfSteps);
    pActuator->refreshRate = pActuator->defaultRefreshRate;
    DPRINTF("Using default RefreshRate=%u and numberOfSteps=%u\n", pActuator->refreshRate, numberOfSteps);
  }
  DPRINTF("current=%X, target=%X, STEPVALUE=%X, directionUp = %X \n",
          pActuator->valueCurrent, pActuator->valueTarget, pActuator->singleStepValue, pActuator->directionUp);

  if (!TimerIsActive(&pActuator->timer)) {
    TimerStart(&pActuator->timer, pActuator->refreshRate);
  } else {
    DPRINT("Timer already active, restarting\n");
    TimerRestart(&pActuator->timer);
  }
}

static inline void triggerCCCallback(s_Actuator *pActuator)
{
  if (NULL != pActuator->cc_cb) {
    zaf_actuator_callback_t cb = pActuator->cc_cb;
    //DPRINT("Triggering CC callback\n");
    cb(pActuator);
  }
}

/**
 * Calculate new current value
 * @param pActuator pointer to Actuator object whose current value should be updated
 */
static inline void updateCurrentValue(s_Actuator *pActuator)
{
  /* difference is less then size of next step */
  if (abs(pActuator->valueTarget - pActuator->valueCurrent) < pActuator->singleStepValue) {
    pActuator->valueCurrent = pActuator->valueTarget;
  }  else {
    pActuator->valueCurrent = (pActuator->directionUp) ? (pActuator->valueCurrent + pActuator->singleStepValue)
                                                       : (pActuator->valueCurrent - pActuator->singleStepValue);
  }
  if(pActuator->valueCurrent == pActuator->valueTarget)
  {
    // We have reached the target value. Update the "Last On" value.
    updateLastOnValue(pActuator);
  }

}

static inline void updateLastOnValue(s_Actuator *pActuator)
{
  // Set "Last On" only if the value doesn't translate to "off". (I.e. if greater than pActuator->min).
  if (pActuator->valueCurrent > pActuator->min)  {
    pActuator->lastOnValue = ZAF_Actuator_GetCurrentValue(pActuator);
  }
}
