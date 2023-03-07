/***************************************************************************//**
 * @file
 * @brief TEMPDRV API implementation.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "em_device.h"
#include "em_system.h"
#include "em_emu.h"
#include "string.h"

#include "tempdrv.h"

/***************************************************************************//**
 * @addtogroup tempdrv
 * @{
 ******************************************************************************/

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

typedef struct {
  TEMPDRV_Callback_t callback;    ///< Callback function
  uint16_t temp;                   ///< Limit temperature (EMU value)
} TEMPDRV_CallbackSet_t;

#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_80)
#define TEMPDRV_ERRATA_FIX
#endif

#ifdef TEMPDRV_ERRATA_FIX
#define TEMPDRV_INT_CALLBACK_DEPTH 1
#define TEMPDRV_
#else
#define TEMPDRV_INT_CALLBACK_DEPTH 0
#endif

#define TEMPDRV_CALLBACK_DEPTH (TEMPDRV_INT_CALLBACK_DEPTH + TEMPDRV_CUSTOM_CALLBACK_DEPTH)
#define TEMPDRV_CUSTOM_CALLBACK_INDEX TEMPDRV_INT_CALLBACK_DEPTH

#if defined(_SILICON_LABS_32B_SERIES_1)
// On Series-1 devices the temperature code is inverted. (high value = low temperature)
#define INCREASING_TEMPERATURE_FLAG   EMU_IF_TEMPLOW
#define DECREASING_TEMPERATURE_FLAG   EMU_IF_TEMPHIGH
#else
// On Series-2 devices the temperature code is normal. (high value = high temperature)
#define INCREASING_TEMPERATURE_FLAG   EMU_IF_TEMPHIGH
#define DECREASING_TEMPERATURE_FLAG   EMU_IF_TEMPLOW
#endif

static TEMPDRV_CallbackSet_t tempdrvHighCallbacks[TEMPDRV_CALLBACK_DEPTH];
static TEMPDRV_CallbackSet_t tempdrvLowCallbacks[TEMPDRV_CALLBACK_DEPTH];
static TEMPDRV_CallbackSet_t nullCallback = { NULL, 0 };
static TEMPDRV_CallbackSet_t *highCallback;
static TEMPDRV_CallbackSet_t *lowCallback;

static bool TEMPDRV_InitState = false;
static bool TEMPDRV_EnableState = false;

static uint16_t convertToEmu(int temp);
static void updateInterrupts(void);

#if defined(_DEVINFO_CAL_TEMP_MASK)
// Calibration values to be initialized in TEMPDRV_Init
static int calibrationEMU;
// Fallback calibration values in case DI calibration data not present
static uint8_t fallbackEMU = 0x90;
static uint8_t fallbackTEMP = 25;
#endif

/// @endcond
#if (EMU_CUSTOM_IRQ_HANDLER == false)
/***************************************************************************//**
 * @brief
 *   EMU Interrupt Handler
 *
 * @details
 *   The EMU_IRQHandler provided by TEMPDRV will call @ref TEMPDRV_IRQHandler.
 *   Configure EMU_CUSTOM_IRQ_HANDLER = true if the application wants to
 *   implement its own EMU_IRQHandler. This is typically needed if one of the
 *   non-temperature related EMU interrupt flags are in use.
 ******************************************************************************/
void EMU_IRQHandler(void)
{
  TEMPDRV_IRQHandler();
}
#endif

/*******************************************************************************
 * @brief
 *   TEMPDRV Interrupt Handler
 *
 * @details
 *   This function must be called by the EMU interrupt handler in order to
 *   support the TEMPDRV callback mechanism.
 *
 *   This function handles the @ref EMU_IF_TEMPHIGH, @ref EMU_IF_TEMPLOW
 *   interrupt flags from the EMU internal temperature sensor.
 ******************************************************************************/
void TEMPDRV_IRQHandler(void)
{
  uint32_t flags = EMU_IntGetEnabled();
  TEMPDRV_Callback_t activeCallback;

  if (flags & DECREASING_TEMPERATURE_FLAG) {
    if (lowCallback->callback != NULL) {
      activeCallback = lowCallback->callback;
      memset(lowCallback, 0, sizeof(TEMPDRV_CallbackSet_t));
      activeCallback(TEMPDRV_GetTemp(), TEMPDRV_LIMIT_LOW);
    }
    EMU_IntClear(DECREASING_TEMPERATURE_FLAG);
  } else if (flags & INCREASING_TEMPERATURE_FLAG) {
    if (highCallback->callback != NULL) {
      activeCallback = highCallback->callback;
      memset(highCallback, 0, sizeof(TEMPDRV_CallbackSet_t));
      activeCallback(TEMPDRV_GetTemp(), TEMPDRV_LIMIT_HIGH);
    }
    EMU_IntClear(INCREASING_TEMPERATURE_FLAG);
  }

  updateInterrupts();
}

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
#if defined(TEMPDRV_ERRATA_FIX)
/* Errata */
typedef enum ErrataState {
  ERRATA_LOW     = 0, /**< Low temperature <65&deg;C. */
  ERRATA_MID     = 1, /**< Medium temperature >65&deg;C and <80&deg;C. */
  ERRATA_HIGH    = 2  /**< High temperature >80&deg;C. */
} ErrataState_t;

// These are the temperature limits in degrees Celsius
#define ERRATA_MID_LIMIT   65
#define ERRATA_HIGH_LIMIT  80
#define ERRATA_HYSTERESIS   8

static void errataCallback(int8_t temp, TEMPDRV_LimitType_t limit);
static void errataStateUpdate(int8_t temp);

static TEMPDRV_CallbackSet_t errataLowTemp[3];  // Temperature decrease thresholds
static TEMPDRV_CallbackSet_t errataHighTemp[3]; // Temperature increase thresholds

/***************************************************************************//**
 * @brief
 *   Errata Callback
 *
 * @details
 *   This function is called by the TEMPDRV_IRQHandler and will change the
 *   state depending on the current temperature.
 ******************************************************************************/
static void errataCallback(int8_t temp, TEMPDRV_LimitType_t limit)
{
  (void) limit; // unused
  errataStateUpdate(temp);
}

/***************************************************************************//**
 * @brief
 *   Errata State Update
 *
 * @details
 *   Updates registers based on the current temperature
 ******************************************************************************/
static void errataStateUpdate(int8_t temp)
{
  ErrataState_t errataState;
  bool emuLocked = (EMU->LOCK == EMU_LOCK_LOCKKEY_LOCKED);

  // Figure out the current state based on temperature
  if (temp < ERRATA_MID_LIMIT) {
    errataState = ERRATA_LOW;
  } else if (temp < ERRATA_HIGH_LIMIT) {
    errataState = ERRATA_MID;
  } else {
    errataState = ERRATA_HIGH;
  }

  // Activate callback and thresholds for the current state
  tempdrvHighCallbacks[0] = errataHighTemp[errataState];
  tempdrvLowCallbacks[0] = errataLowTemp[errataState];

  if (emuLocked) {
    EMU->LOCK = EMU_LOCK_LOCKKEY_UNLOCK;
  }

  switch (errataState) {
    case ERRATA_LOW:
      EMU_SetBiasMode(emuBiasMode_1KHz);
      break;
    case ERRATA_MID:
      EMU_SetBiasMode(emuBiasMode_4KHz);
      break;
    case ERRATA_HIGH:
      EMU_SetBiasMode(emuBiasMode_Continuous);
      break;
  }

  if (emuLocked) {
    EMU->LOCK = EMU_LOCK_LOCKKEY_LOCK;
  }
}

/***************************************************************************//**
 * @brief
 *   Errata Initialization
 *
 * @details
 *   Initialize the temperature thresholds based on calibration values and
 *   initializes the errata fix based on the current temperature.
 ******************************************************************************/
static void errataInit(void)
{
  SYSTEM_ChipRevision_TypeDef rev;
  SYSTEM_ChipRevisionGet(&rev);

  /* Rev A temp errata handling */
  if (rev.major == 0x01) {
    uint16_t limitLow;
    uint16_t limitHigh;

    // Initialize Low temperature state [*, 65]
    limitHigh = convertToEmu(ERRATA_MID_LIMIT);
    errataHighTemp[ERRATA_LOW].temp = limitHigh;
    errataHighTemp[ERRATA_LOW].callback = errataCallback;

    // Initialize Mid temperature state [57, 80]
    limitLow  = convertToEmu(ERRATA_MID_LIMIT - ERRATA_HYSTERESIS);
    limitHigh = convertToEmu(ERRATA_HIGH_LIMIT);
    errataLowTemp[ERRATA_MID].temp = limitLow;
    errataLowTemp[ERRATA_MID].callback = errataCallback;
    errataHighTemp[ERRATA_MID].temp = limitHigh;
    errataHighTemp[ERRATA_MID].callback = errataCallback;

    // Initialize High temperature state [72, *]
    limitLow = convertToEmu(ERRATA_HIGH_LIMIT - ERRATA_HYSTERESIS);
    errataLowTemp[ERRATA_HIGH].temp = limitLow;
    errataLowTemp[ERRATA_HIGH].callback = errataCallback;

    errataStateUpdate(TEMPDRV_GetTemp());
  }
}
#else // TEMPDRV_ERRATA_FIX

#define errataInit()

#endif // TEMPDRV_ERRATA_FIX

/* Internal Functions */
/***************************************************************************//**
 * @brief
 *   Find an empty spot for callback in set
 *
 * @param[in] set
 *   Callback set to search
 *
 * @return
 *   index of empty space if found, -1 if none
 ******************************************************************************/
static int8_t findCallbackSpace(TEMPDRV_CallbackSet_t *set)
{
  uint8_t index;
  for (index = TEMPDRV_CUSTOM_CALLBACK_INDEX; index < TEMPDRV_CALLBACK_DEPTH; index++) {
    if (set[index].callback == NULL) {
      return index;
    }
  }
  // no empty space, return -1
  return -1;
}

/***************************************************************************//**
 * @brief
 *   Attempt to add a callback to a set
 *
 * @param[in] set
 *   Callback set to add callback to
 *
 * @param[in] temp
 *   Temperature to register callback at
 *
 * @param[in] callback
 *   Callback function
 *
 * @return
 *    @li @ref ECODE_EMDRV_TEMPDRV_OK on success.
 *    @li @ref ECODE_EMDRV_TEMPDRV_NO_SPACE if there is no space left for
 *        callbacks.
 ******************************************************************************/
static Ecode_t addCallback(TEMPDRV_CallbackSet_t *set,
                           int8_t temp,
                           TEMPDRV_Callback_t callback)
{
  int8_t index = findCallbackSpace(set);
  if (index < 0) {
    return ECODE_EMDRV_TEMPDRV_NO_SPACE;
  }
  set[index].temp = convertToEmu(temp);
  set[index].callback = callback;
  updateInterrupts();
  return ECODE_EMDRV_TEMPDRV_OK;
}

/***************************************************************************//**
 * @brief
 *   Remove a callback from the set
 *
 * @param[in] set
 *   Callback set to remove callback from
 *
 * @param[in] callback
 *   Callback function
 *
 * @return
 *   @c true on success and @c false if the callback was not found.
 ******************************************************************************/
static bool removeCallback(TEMPDRV_CallbackSet_t *set,
                           TEMPDRV_Callback_t callback)
{
  bool found = false;
  uint8_t index;
  for (index = TEMPDRV_CUSTOM_CALLBACK_INDEX; index < TEMPDRV_CALLBACK_DEPTH; index++) {
    if (set[index].callback == callback) {
      set[index].callback = NULL;
      set[index].temp = 0;
      found = true;
    }
  }
  updateInterrupts();
  return found;
}

/***************************************************************************//**
 * @brief
 *   Check if another callback has registered the same temperature
 *
 * @param[in] set
 *   Callback set to search
 *
 * @param[in] temp
 *   Temperature to match
 *
 * @return
 *    @c true if a duplicate is found, @c false if no duplicates is found.
 ******************************************************************************/
static bool checkForDuplicates(TEMPDRV_CallbackSet_t *set, int8_t temp)
{
  uint8_t index;
  uint16_t emu = convertToEmu(temp);
  for (index = TEMPDRV_CUSTOM_CALLBACK_INDEX; index < TEMPDRV_CALLBACK_DEPTH; index++) {
    // filter out only entries with valid callbacks
    if (set[index].callback != NULL) {
      // if duplicate temperature, return true
      if (set[index].temp == emu) {
        return true;
      }
    }
  }
  // return false if no duplicate temperatures found
  return false;
}

/***************************************************************************//**
 * @brief
 *   Convert a temperature in &deg;C to an EMU sensor value
 *
 * @param[in] temp
 *   Temperature to convert in degrees Celsius
 *
 * @return
 *   EMU temperature sensor value that represents the given temperature
 ******************************************************************************/
static uint16_t convertToEmu(int temp)
{
#if defined(_SILICON_LABS_32B_SERIES_2)
  return (uint16_t)((temp + 273) & 0x1FF); // Convert from Celsius to Kelvin (9 bits)
#else
  int32_t res = (int32_t) calibrationEMU -  ((temp * 5) >> 3);
  // Cap conversion results at uint8_t bounds
  if (res > 255) {
    res = 255;
  } else if (res < 0) {
    res = 0;
  }
  return (uint8_t) res;
#endif
}

/***************************************************************************//**
 * @brief
 *   Turn off and clear EMU temperature related interrupts
 ******************************************************************************/
static void disableInterrupts(void)
{
  EMU_IntClear(EMU_IF_TEMPLOW | EMU_IF_TEMPHIGH);
  EMU_IntDisable(EMU_IF_TEMPLOW | EMU_IF_TEMPHIGH);
}

/***************************************************************************//**
 * @brief
 *   Update interrupts based on active callbacks
 *
 * @details
 *   There can only be two temperature thresholds active in the EMU sensor at a
 *   single point in time. One threshold for increasing temperature and one
 *   threshold for decreasing temperature. The responsibility of this function
 *   is to always select which temperature thresholds that should be activated
 *   in the EMU sensor at any given time.
 ******************************************************************************/
static void updateInterrupts(void)
{
  int index;

  // Find lowest temperature active high callback
  for (index = 0; index < TEMPDRV_CALLBACK_DEPTH; index++) {
    // filter out only entries with valid callbacks
    if (tempdrvHighCallbacks[index].callback != NULL) {
      if (highCallback->callback == NULL) {
        highCallback = &tempdrvHighCallbacks[index];
      } else {
        if (tempdrvHighCallbacks[index].temp > highCallback->temp) {
          highCallback = &tempdrvHighCallbacks[index];
        }
      }
    }
  }

  // Find highest temperature active low callback
  for (index = 0; index < TEMPDRV_CALLBACK_DEPTH; index++) {
    // filter out only entries with valid callbacks
    if (tempdrvLowCallbacks[index].callback != NULL) {
      if (lowCallback->callback == NULL) {
        lowCallback = &tempdrvLowCallbacks[index];
      } else {
        if (tempdrvLowCallbacks[index].temp < lowCallback->temp) {
          lowCallback = &tempdrvLowCallbacks[index];
        }
      }
    }
  }

#if defined(_SILICON_LABS_32B_SERIES_1)
  // On Series-1 devices the temperature code is inverted (high value = low temperature)
  if (highCallback->callback != NULL) {
    EMU->TEMPLIMITS = (EMU->TEMPLIMITS & ~_EMU_TEMPLIMITS_TEMPLOW_MASK)
                      | ((highCallback->temp << _EMU_TEMPLIMITS_TEMPLOW_SHIFT) & _EMU_TEMPLIMITS_TEMPLOW_MASK);
    EMU_IntEnable(EMU_IEN_TEMPLOW);
  } else {
    EMU_IntDisable(EMU_IEN_TEMPLOW);
  }
  if (lowCallback->callback != NULL) {
    EMU->TEMPLIMITS = (EMU->TEMPLIMITS & ~_EMU_TEMPLIMITS_TEMPHIGH_MASK)
                      | ((lowCallback->temp << _EMU_TEMPLIMITS_TEMPHIGH_SHIFT) & _EMU_TEMPLIMITS_TEMPHIGH_MASK);
    EMU_IntEnable(EMU_IEN_TEMPHIGH);
  } else {
    EMU_IntDisable(EMU_IEN_TEMPHIGH);
  }
#else
  // On Series-2 devices the temperature code is normal (high value = high temperature)
  if (highCallback->callback != NULL) {
    EMU->TEMPLIMITS = (EMU->TEMPLIMITS & ~_EMU_TEMPLIMITS_TEMPHIGH_MASK)
                      | ((highCallback->temp << _EMU_TEMPLIMITS_TEMPHIGH_SHIFT) & _EMU_TEMPLIMITS_TEMPHIGH_MASK);
    EMU_IntEnable(EMU_IEN_TEMPHIGH);
  } else {
    EMU_IntDisable(EMU_IEN_TEMPHIGH);
  }
  if (lowCallback->callback != NULL) {
    EMU->TEMPLIMITS = (EMU->TEMPLIMITS & ~_EMU_TEMPLIMITS_TEMPLOW_MASK)
                      | ((lowCallback->temp << _EMU_TEMPLIMITS_TEMPLOW_SHIFT) & _EMU_TEMPLIMITS_TEMPLOW_MASK);
    EMU_IntEnable(EMU_IEN_TEMPLOW);
  } else {
    EMU_IntDisable(EMU_IEN_TEMPLOW);
  }
#endif
}

/***************************************************************************//**
 * @brief
 *   Prepare calibration values for temperature conversion.
 ******************************************************************************/
static void calibration(void)
{
#if defined(_DEVINFO_CAL_TEMP_MASK)
  uint32_t DItemp, DIemu;

  // Retrieve calibration data from DI page
  DItemp = ((DEVINFO->CAL & _DEVINFO_CAL_TEMP_MASK) >> _DEVINFO_CAL_TEMP_SHIFT);
  DIemu = ((DEVINFO->EMUTEMP & _DEVINFO_EMUTEMP_EMUTEMPROOM_MASK) >> _DEVINFO_EMUTEMP_EMUTEMPROOM_SHIFT);

  if ((DItemp == (_DEVINFO_CAL_TEMP_MASK >> _DEVINFO_CAL_TEMP_SHIFT))
      || (DIemu == (_DEVINFO_EMUTEMP_EMUTEMPROOM_MASK >> _DEVINFO_EMUTEMP_EMUTEMPROOM_SHIFT))) {
    // Missing DI page calibration data, substitute fixed values
    DItemp = fallbackTEMP;
    DIemu = fallbackEMU;
  }

  // calculate conversion offsets. Based on assumed slope of 5/8
  calibrationEMU = (DIemu) + ((5 * (DItemp)) / 8);
#endif
}
/// @endcond
/* Official API */
/***************************************************************************//**
 * @brief
 *   Initialize the TEMP driver.
 *
 * @details
 *   This will clear all the registered callbacks and enable the EMU IRQ in the
 *   NVIC. Calling this function will also enable the EMU_E201 errata fix for
 *   first generation Pearl, Jade and EFR32 devices.
 *
 * @return
 *   @ref ECODE_EMDRV_TEMPDRV_OK on success.
 ******************************************************************************/
Ecode_t TEMPDRV_Init(void)
{
  // Flag up
  TEMPDRV_InitState = true;

  // reset stack state by erasing callbacks
  memset(tempdrvHighCallbacks, 0, sizeof (TEMPDRV_CallbackSet_t) * TEMPDRV_CALLBACK_DEPTH);
  memset(tempdrvLowCallbacks, 0, sizeof (TEMPDRV_CallbackSet_t) * TEMPDRV_CALLBACK_DEPTH);
  highCallback = &nullCallback;
  lowCallback = &nullCallback;

  calibration();
  errataInit();
  disableInterrupts();
  NVIC_ClearPendingIRQ(EMU_IRQn);
  NVIC_EnableIRQ(EMU_IRQn);
  updateInterrupts();

  return ECODE_EMDRV_TEMPDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    De-initialize the TEMP driver.
 *
 * @details
 *   This will clear all the registered callbacks and disable the EMU IRQ in
 *   the NVIC.
 *
 * @return
 *    @ref ECODE_EMDRV_TEMPDRV_OK on success.
 ******************************************************************************/
Ecode_t TEMPDRV_DeInit(void)
{
  TEMPDRV_InitState = false;
  NVIC_DisableIRQ(EMU_IRQn);
  NVIC_ClearPendingIRQ(EMU_IRQn);
  disableInterrupts();
  memset(tempdrvHighCallbacks, 0, sizeof (TEMPDRV_CallbackSet_t) * TEMPDRV_CALLBACK_DEPTH);
  memset(tempdrvLowCallbacks, 0, sizeof (TEMPDRV_CallbackSet_t) * TEMPDRV_CALLBACK_DEPTH);
  return ECODE_EMDRV_TEMPDRV_OK;
}

/***************************************************************************//**
 * @brief
 *   Enable or disable the TEMP driver.
 *
 * @param[in] enable
 *   true to enable the TEMP driver, false to disable the TEMP driver.
 *
 * @return
 *   @ref ECODE_EMDRV_TEMPDRV_OK on success.
 ******************************************************************************/
Ecode_t TEMPDRV_Enable(bool enable)
{
  if (TEMPDRV_EnableState != enable) {
    TEMPDRV_EnableState = enable;
    if (enable) {
      updateInterrupts();
    } else {
      disableInterrupts();
    }
  }
  return ECODE_EMDRV_TEMPDRV_OK;
}

/***************************************************************************//**
 * @brief
 *   Get the number of active callbacks for a limit.
 *
 * @param[in] limit
 *   Limit type, refer to @ref TEMPDRV_LimitType.
 *
 * @return
 *   Number of active callbacks
 ******************************************************************************/
uint8_t TEMPDRV_GetActiveCallbacks(TEMPDRV_LimitType_t limit)
{
  TEMPDRV_CallbackSet_t *set;

  if (limit == TEMPDRV_LIMIT_HIGH) {
    // Define callback set
    set = tempdrvHighCallbacks;
  } else if (limit == TEMPDRV_LIMIT_LOW) {
    // Define callback set
    set = tempdrvLowCallbacks;
  } else {
    // Invalid limit
    return 0;
  }
  uint8_t index, count = 0;
  for (index = TEMPDRV_CUSTOM_CALLBACK_INDEX; index < TEMPDRV_CALLBACK_DEPTH; index++) {
    // filter out only entries with valid callbacks
    if (set[index].callback != NULL) {
      count++;
    }
  }
  return count;
}

/***************************************************************************//**
 * @brief
 *    Get the current temperature.
 *
 * @return
 *    Current temperature in degrees Celsius.
 ******************************************************************************/
int8_t TEMPDRV_GetTemp(void)
{
  float tempCelsius;
  int8_t result;

  tempCelsius = EMU_TemperatureGet();

  if (tempCelsius < INT8_MIN) {
    tempCelsius = INT8_MIN;
  } else if (tempCelsius > INT8_MAX) {
    tempCelsius = INT8_MAX;
  }

  // adding extra 0.5 before truncating to simulate rounding behavior
  result = (((int8_t) (tempCelsius + 0.5 - INT8_MIN)) + INT8_MIN);

  return result;
}

/***************************************************************************//**
 * @brief
 *   Register a callback in the TEMP driver
 *
 * @details
 *   This function is used for registering an application callback when the
 *   temperature changes. Note that when calling this function an application
 *   must specify the direction of the temperature change, use
 *   @ref TEMPDRV_LIMIT_LOW to receive a callback when the temperature drops
 *   below the specified temp and use @ref TEMPDRV_LIMIT_HIGH to receive a
 *   callback when the temperature increases above the specified temp.
 *
 * @note
 *   The user registered callback will be cleared once it's called. This means
 *   that the callback functions are not persistent, and have to be managed by
 *   the application. This feature can be used to implement a user controlled
 *   hysteresis. So for instance to register a callback at 50&deg;C with a
 *   5&deg;C hysteresis you can first register a callback at 50&deg;C or above
 *   using this function, and when the callback fires you can use this function
 *   again to register a callback when the temperature decreases to 45&deg;C or
 *   below. Each time a callback fires you only need to call the
 *   @ref TEMPDRV_RegisterCallback() function, there is no need to call
 *   @ref TEMPDRV_UnregisterCallback().
 *
 * @details
 *   It's important to know the current temperature before calling this
 *   function. Attempting to register a callback that would fire immediately
 *   is not supported and will result in a return value of
 *   @ref ECODE_EMDRV_TEMPDRV_BAD_LIMIT. Examples of an illegal scenario would
 *   be to register a callback for a temperature that is higher than the current
 *   temperature and with a limit set to @ref TEMPDRV_LIMIT_LOW.
 *
 * @param[in] temp
 *   Temperature to trigger on given in number of &deg;C.
 *
 * @param[in] limit
 *   Limit type, refer to @ref TEMPDRV_LimitType. Using @ref TEMPDRV_LIMIT_LOW
 *   will register a callback when the EMU temperature reaches \p temp &deg;C
 *   or lower, and using @ref TEMPDRV_LIMIT_HIGH will register a callback when
 *   the EMU temperature reaches \p temp &deg;C or higher.
 *
 * @param[in] callback
 *   User defined function to call when temperature threshold is reached or passed.
 *
 * @return
 *   @li @ref ECODE_EMDRV_TEMPDRV_OK on success.
 *
 *   @li @ref ECODE_EMDRV_TEMPDRV_PARAM_ERROR if the callback is NULL.
 *
 *   @li @ref ECODE_EMDRV_TEMPDRV_NO_INIT if the user has forgot to call @ref
 *     TEMPDRV_Init() before attempting to register a callback.
 *
 *   @li @ref ECODE_EMDRV_TEMPDRV_BAD_LIMIT is returned if \p temp is below the
 *     current temperature and \p limit is @ref TEMPDRV_LIMIT_LOW. It is also
 *     returned if \p temp is above the current temperature and \p limit is
 *     @ref TEMPDRV_LIMIT_HIGH.
 *
 *   @li @ref ECODE_EMDRV_TEMPDRV_DUP_TEMP is returned if a duplicate callback
 *     is detected. A duplicate callback is if you attempt to register a new
 *     callback with the same \p temp and the same \p limit as some
 *     already registered callback.
 ******************************************************************************/
Ecode_t TEMPDRV_RegisterCallback(int8_t temp,
                                 TEMPDRV_LimitType_t limit,
                                 TEMPDRV_Callback_t callback)
{
  TEMPDRV_CallbackSet_t *set;
  if (TEMPDRV_InitState == false) {
    return ECODE_EMDRV_TEMPDRV_NO_INIT;
  }
  // cannot register null callback
  if (callback == NULL) {
    return ECODE_EMDRV_TEMPDRV_PARAM_ERROR;
  }
  if (limit == TEMPDRV_LIMIT_HIGH) {
    // current temperature is already higher than requested temperature
    if (TEMPDRV_GetTemp() > temp) {
      return ECODE_EMDRV_TEMPDRV_BAD_LIMIT;
    }
    // Define callback set
    set = tempdrvHighCallbacks;
  } else if (limit == TEMPDRV_LIMIT_LOW) {
    // current temperature is already lower than requested temperature
    if (TEMPDRV_GetTemp() < temp) {
      return ECODE_EMDRV_TEMPDRV_BAD_LIMIT;
    }
    // Define callback set
    set = tempdrvLowCallbacks;
  } else {
    // Invalid limit
    return ECODE_EMDRV_TEMPDRV_PARAM_ERROR;
  }

  // Cannot register duplicate temperature callback
  if (checkForDuplicates(set, temp) == true) {
    return ECODE_EMDRV_TEMPDRV_DUP_TEMP;
  }

  return addCallback(set, temp, callback);
}

/***************************************************************************//**
 * @brief
 *   Unregister a callback in the TEMP driver.
 *
 * @param[in] callback
 *   Callback to unregister.
 *
 * @return
 *   @li @ref ECODE_EMDRV_TEMPDRV_OK on success.
 *
 *   @li @ref ECODE_EMDRV_TEMPDRV_PARAM_ERROR if the callback is NULL.
 *
 *   @li @ref ECODE_EMDRV_TEMPDRV_NO_CALLBACK if the callback was not found.
 ******************************************************************************/
Ecode_t TEMPDRV_UnregisterCallback(TEMPDRV_Callback_t callback)
{
  // cannot register null callback
  if (callback == NULL) {
    return ECODE_EMDRV_TEMPDRV_PARAM_ERROR;
  }
  if (removeCallback(tempdrvHighCallbacks, callback) == false
      && removeCallback(tempdrvLowCallbacks, callback) == false) {
    return ECODE_EMDRV_TEMPDRV_NO_CALLBACK;
  }
  return ECODE_EMDRV_TEMPDRV_OK;
}

/* *INDENT-OFF* */
// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @brief Temperature Sensor Driver
/// @{
///
///   @details
///
///   The source files for the TEMP driver library resides in the
///   emdrv/tempdrv folder, and consists of tempdrv.c and tempdrv.h.
///
///   @li @ref tempdrv_intro
///   @li @ref tempdrv_conf
///   @li @ref tempdrv_api
///
///   @n @section tempdrv_intro Introduction
///
///   TEMPDRV gives the user a nice interface to the EMU internal temperature sensor
///   which is present on the EFR32 and some EFM32 devices. The TEMPDRV supports
///   application specific callbacks at given temperature thresholds. The EMU
///   internal temperature sensor is running in EM0-EM4H and is capable of waking
///   up the core on temperature change. The EMU temperature sensor is running
///   continuously and measurements are taken every 250 ms.
///
///   @note The TEMPDRV is using the EMU peripheral and not the ADC peripheral.
///   The ADC contains another internal temperature sensor which is not touched
///   by the TEMPDRV.
///
///   The TEMPDRV also provides an important errata fix for the EFR32 first
///   generation devices when operating at high temperature environments (above
///   50&deg;C). The errata document for the EFR32 describes the errata which is
///   called "EMU_E201 - High Temperature Operation". To implement the errata fix
///   in a user application it is enough to include the TEMPDRV and call
///   @ref TEMPDRV_Init() at the start of the program. This will activate the errata
///   fix code which is modifying registers based on changes in the EMU temperature.
///
///   @n @section tempdrv_conf Configuration Options
///
///   Some properties of the TEMPDRV driver are compile-time configurable. These
///   properties are set in a file named tempdrv_config.h. A template for this
///   file, containing default values, resides in the emdrv/tempdrv/config folder.
///   To configure TEMPDRV for your application, provide your own configuration file.
///   These are the available configuration parameters with default values defined.
///   @code{.c}
///
///  // Callback table depth (for high and low callbacks each)
///  #define TEMPDRV_CALLBACK_DEPTH 5
///
///  // Allow temperature sensor to wake the device up from EM4
///  #define TEMPDRV_EM4WAKEUP false
///
///  // Allow TEMPDRV to define the EMU_IRQ_Handler. Enable if EMU_IRQ_Handler is
///  // defined elsewhere.
///  #define EMU_CUSTOM_IRQ_HANDLER false
///   @endcode
///
///   Callback table depth determines the number of concurrent callbacks that can be
///   registered at a single time. The depth applies to each limit, so depth of 5
///   allows up to 5 high and 5 low callbacks to be registered.
///   There are no run-time configuration options for TEMPDRV.
///
///   @n @section tempdrv_api The API
///
///   This section contain brief descriptions of the functions in the API. You will
///   find detailed information on input and output parameters and return values by
///   clicking on the function names. Most functions return an error
///   code, @ref ECODE_EMDRV_TEMPDRV_OK is returned on success,
///   see ecode.h and tempdrv.h for other error codes.
///
///   Your application code must include one header file: @em tempdrv.h.
///
///   @ref TEMPDRV_Init(), @ref TEMPDRV_DeInit() @n
///    These functions initializes or deinitializes the TEMPDRV driver. This will
///    erase any registered callbacks and disabled all interrupts. Typically
///    @htmlonly TEMPDRV_Init() @endhtmlonly is called once in your startup code.
///
///   @ref TEMPDRV_Enable() @n
///    Enable or disable the temperature driver without losing any registered
///    callbacks.
///
///   @ref TEMPDRV_GetTemp() @n
///    Get the current temperature in degrees Celsius. This measurement is based on
///    a conversion from the EMU temperature sensor and calibration data that is
///    stored in the DI page.
///
///   @ref TEMPDRV_RegisterCallback(), @ref TEMPDRV_UnregisterCallback() @n
///    Callbacks can be registered for rising or falling thresholds and will called
///    as soon as the temperature matches the specified threshold. Multiple
///    callbacks at the same temperature are not permitted, nor are mismatches
///    between temperature and limit (e. g temperature is lower than current but
///    the limit is set to high). Additionally, unregistering a callback will remove
///    all entries of matching callbacks.
///
///   @n @section tempdrv_example Example
///   @code{.c}
///#include "tempdrv.h"
///
///boolean flag = false;
///
///void callback(int8_t temp, TEMPDRV_LimitType_t limit)
///{
///  flag = true;
///}
///
///int main(void)
///{
///  TEMPDRV_Init();
///
///  // Register a callback at 10 degrees above current temperature
///  TEMPDRV_RegisterCallback(TEMPDRV_GetTemp()+10, TEMPDRV_LIMIT_HIGH, callback);
///
///  while (flag==false) {};
///}
///   @endcode
///
/// @}  *******************************************************

/** @} (end addtogroup tempdrv) */
