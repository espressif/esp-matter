/***************************************************************************//**
 * @file
 * @brief Radio coexistence utilities
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "coexistence.h"
#include "coexistence-hal.h"

#ifndef COEX_HAL_DISABLED
#define coexReqAndGntIrqShared() \
  (reqCfg.cb == &COEX_GNT_ISR)

typedef struct COEX_ReqArgs {
  /** PTA request arguments*/
  COEX_ReqState_t *reqState;
  COEX_Req_t coexReq;
  COEX_ReqCb_t cb;
} COEX_ReqArgs_t;

static void (*setCoexPowerStateCallbackPtr)(bool powerUp);
static void (*setCoexReqCallbackPtr)(COEX_ReqArgs_t *reqArgs);
static void setCoexReqCallback(COEX_ReqArgs_t *reqArgs);
static const COEX_HalCallbacks_t *coexHalCallbacks;
static COEX_RadioCallback_t coexRadioCallback;
static COEX_RandomDelayCallback_t coexRandomDelayCallback;
static void coexNotifyRadio(void);
__STATIC_INLINE void coexUpdateReqIsr(void);

/** PTA radio hold off GPIO configuration */
static COEX_GpioHandle_t rhoHandle = NULL;

/** PTA grant GPIO configuration */
static COEX_GpioHandle_t gntHandle = NULL;

/** PTA request GPIO configuration */
static COEX_GpioHandle_t reqHandle = NULL;

#if SL_RAIL_UTIL_COEX_OUTPUT_OVERRIDE_GPIO_INPUT
/** PTA external request GPIO configuration */
static COEX_GpioHandle_t externalReqHandle = NULL;
#endif // SL_RAIL_UTIL_COEX_OUTPUT_OVERRIDE_GPIO_INPUT

/** PTA PWM request GPIO configuration */
static COEX_GpioHandle_t pwmReqHandle = NULL;

/** PTA priority GPIO configuration */
static COEX_GpioHandle_t priHandle = NULL;

/** PHY select GPIO configuration */
static COEX_GpioHandle_t phySelectHandle = NULL;

#ifdef SL_RAIL_UTIL_COEX_WIFI_TX_PORT
/** Wifi Tx GPIO configuration */
static COEX_GpioHandle_t wifiTxHandle = NULL;
#endif

typedef struct COEX_Cfg {
  /** PTA request states*/
  COEX_ReqState_t *reqHead;
  volatile COEX_Req_t combinedRequestState;
  volatile bool radioOn : 1;
  volatile bool requestDenied : 1;
  volatile bool updateGrantInProgress : 1;
  volatile COEX_Options_t options;
} COEX_Cfg_t;

static void COEX_REQ_ISR(void);
static void COEX_GNT_ISR(void);
static void COEX_RHO_ISR(void);
static void COEX_PHY_SEL_ISR(void);

static COEX_Cfg_t coexCfg;

static COEX_GpioConfig_t phySelectCfg = {
#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  .index = COEX_GPIO_INDEX_PHY_SELECT,
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  .options = (COEX_GpioOptions_t)(COEX_GPIO_OPTION_INT_ASSERTED),
  .cb = &COEX_PHY_SEL_ISR
};

#ifdef SL_RAIL_UTIL_COEX_WIFI_TX_PORT
static COEX_GpioConfig_t wifiTxCfg = {
#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  .index = COEX_GPIO_INDEX_WIFI_TX,
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  .options = (COEX_GpioOptions_t)(COEX_GPIO_OPTION_INT_ASSERTED
                                  | COEX_GPIO_OPTION_INT_DEASSERTED),
  .cb = &COEX_WIFI_TX_ISR
};
#endif

static COEX_GpioConfig_t rhoCfg = {
#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  .index = COEX_GPIO_INDEX_RHO,
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  .options = (COEX_GpioOptions_t)(COEX_GPIO_OPTION_INT_ASSERTED
                                  | COEX_GPIO_OPTION_INT_DEASSERTED),
  .cb = &COEX_RHO_ISR
};

static COEX_GpioConfig_t gntCfg = {
#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  .index = COEX_GPIO_INDEX_GNT,
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  .options = (COEX_GpioOptions_t)(COEX_GPIO_OPTION_DEFAULT_ASSERTED
                                  | COEX_GPIO_OPTION_INT_ASSERTED
                                  | COEX_GPIO_OPTION_INT_DEASSERTED),
  .cb = &COEX_GNT_ISR
};

static COEX_GpioConfig_t reqCfg = {
#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  .index = COEX_GPIO_INDEX_INTERNAL_REQ,
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  .options = (COEX_GpioOptions_t)(COEX_GPIO_OPTION_INT_DEASSERTED
                                  | COEX_GPIO_OPTION_OUTPUT),
  .cb = &COEX_REQ_ISR
};

#ifdef SL_RAIL_UTIL_COEX_OUTPUT_OVERRIDE_GPIO_INPUT
static COEX_GpioConfig_t externalReqCfg = {
  .index = COEX_GPIO_INDEX_REQ,
  .options = (COEX_GpioOptions_t)(COEX_GPIO_OPTION_INT_DEASSERTED
                                  | COEX_GPIO_OPTION_OUTPUT),
  .cb = &COEX_REQ_ISR
};
#endif

static COEX_GpioConfig_t pwmReqCfg = {
  .options = (COEX_GpioOptions_t)(COEX_GPIO_OPTION_OUTPUT
                                  | COEX_GPIO_OPTION_SHARED)
};

static COEX_GpioConfig_t priCfg = {
  .options = COEX_GPIO_OPTION_OUTPUT
};

static bool gntWasAsserted = false;

void COEX_SetHalCallbacks(const COEX_HalCallbacks_t *callbacks)
{
  coexHalCallbacks = callbacks;
}

void COEX_SetRandomDelayCallback(COEX_RandomDelayCallback_t callback)
{
  coexRandomDelayCallback = callback;
}

void COEX_SetRadioCallback(COEX_RadioCallback_t callback)
{
  coexRadioCallback = callback;
}

__STATIC_INLINE bool isReqShared(void)
{
  return (reqCfg.options & COEX_GPIO_OPTION_SHARED) != 0U;
}

__STATIC_INLINE void coexReqRandomBackoff(void)
{
  if (coexRandomDelayCallback != NULL) {
    (*coexRandomDelayCallback)(coexCfg.options
                               & COEX_OPTION_MAX_REQ_BACKOFF_MASK);
  }
}

#ifndef COEX_HAL_GetPriority
#define COEX_HAL_GetPriority() getGpioOut(priHandle, false)
#endif //COEX_HAL_GetPriority

static void setCoexOption(COEX_Options_t option, bool enable)
{
  if (enable) {
    coexCfg.options |= option;
  } else {
    coexCfg.options &= ~option;
  }
}

static void setGpio(COEX_GpioHandle_t gpioHandle, bool enabled)
{
  (*coexHalCallbacks->setGpio)(gpioHandle, enabled);
}

static bool isGpioInSet(COEX_GpioHandle_t gpioHandle, bool defaultValue)
{
  return (*coexHalCallbacks->isGpioInSet)(gpioHandle, defaultValue);
}

static bool getGpioOut(COEX_GpioHandle_t gpioHandle, bool defaultValue)
{
  return (*coexHalCallbacks->isGpioOutSet)(gpioHandle, defaultValue);
}

static void enableGpioInt(COEX_GpioHandle_t gpioHandle, bool *wasAsserted)
{
  (*coexHalCallbacks->enableGpioInt)(gpioHandle, true, wasAsserted);
}

static void disableGpioInt(COEX_GpioHandle_t gpioHandle)
{
  (*coexHalCallbacks->enableGpioInt)(gpioHandle, false, NULL);
}

static void setGpioFlag(COEX_GpioHandle_t gpioHandle)
{
  (*coexHalCallbacks->setGpioFlag)(gpioHandle, true);
}

static void clearGpioFlag(COEX_GpioHandle_t gpioHandle)
{
  (*coexHalCallbacks->setGpioFlag)(gpioHandle, false);
}

static void configGpio(COEX_GpioHandle_t gpioHandle,
                       COEX_GpioHandle_t *handlePtr,
                       COEX_GpioConfig_t *config)
{
  if (config->cb != NULL) {
    disableGpioInt(*handlePtr);
  }
  *handlePtr = gpioHandle;
  coexHalCallbacks->configGpio(gpioHandle, config);
}

static void coexEventCallback(COEX_Events_t events)
{
  if (coexRadioCallback != NULL) {
    if (COEX_HAL_GetPriority()) {
      events |= COEX_EVENT_PRIORITY_ASSERTED;
    }
    coexRadioCallback(events);
  }
}

#ifdef COEX_PULSE_REQ_ON_RHO_RELEASE_SUPPORT
static void toggleCoexReq(void)
{
  if ( ((coexCfg.options
         & COEX_OPTION_PULSE_REQ_ON_RHO_RELEASE) != 0U) // Pulse request on RHO release selected
       && getGpioOut(reqHandle, false)          // REQUESTing and
       && !isGpioInSet(rhoHandle, false)          // RHO not asserted and
       && !isGpioInSet(gntHandle, true)) {        // GRANT not asserted
    setGpio(reqHandle, false);
    setGpio(reqHandle, true);
  }
}
#else //!COEX_PULSE_REQ_ON_RHO_RELEASE_SUPPORT
static void toggleCoexReq(void)
{
}
#endif //COEX_PULSE_REQ_ON_RHO_RELEASE_SUPPORT

void COEX_UpdateGrant(void)
{
  if (coexCfg.updateGrantInProgress) {
    // Prevent this function from being called recursively
    return;
  } else {
    coexCfg.updateGrantInProgress = true;
  }
  if (getGpioOut(reqHandle, false)) {    // GRANT phase
    bool newGnt = isGpioInSet(gntHandle, false);   // Sample GPIO once, now
    if (newGnt != gntWasAsserted) {
      gntWasAsserted = newGnt;
      coexNotifyRadio();
      // Issue callbacks on GRANT assert or negate
      // These are not one-shot callbacks
      COEX_ReqState_t* reqPtr;
      COEX_Req_t newState = (newGnt ? COEX_REQCB_GRANTED : COEX_REQCB_NEGATED);
      for (reqPtr = coexCfg.reqHead; reqPtr != NULL; reqPtr = reqPtr->next) {
        if ((reqPtr->cb != NULL) && ((reqPtr->coexReq & newState) != 0U)) {
          (*reqPtr->cb)(newState);
        }
      }
      if (!newGnt) {
        coexEventCallback(COEX_EVENT_GRANT_RELEASED);

        // Do we need this to meet GRANT -> REQUEST timing?
        // On GNT deassertion, pulse REQUEST to keep us going.
        // Don't want to revert to REQUEST phase here but stay in GRANT phase.
        // This seems dangerous in that it could allow a peer to assert their
        // REQUEST causing a conflict/race.
        if ((coexCfg.options & COEX_OPTION_PULSE_REQ_ON_GNT_RELEASE) != 0U) {
          setGpio(reqHandle, false);
          setGpio(reqHandle, true);
        }
      }
    }
  } else {                          // REQUEST phase
    if (coexReqAndGntIrqShared() && isReqShared()) {
      // External REQUEST deasserted so we can assert ours
      disableGpioInt(reqHandle);   // This is a one-shot event
      coexReqRandomBackoff();
      coexUpdateReqIsr();
    } else {
      // Ignore GRANT changes unless we are REQUESTing
    }
  }
  coexCfg.updateGrantInProgress = false;
}

// Triggered on both GRANT edges
static void COEX_GNT_ISR(void)
{
  clearGpioFlag(gntHandle);
  COEX_UpdateGrant();
}

// Public API
bool COEX_SetRequest(COEX_ReqState_t *reqState,
                     COEX_Req_t coexReq,
                     COEX_ReqCb_t cb)
{
  if (setCoexReqCallbackPtr == NULL || reqState == NULL) {
    return false;
  }
  COEX_ReqArgs_t reqArgs;
  reqArgs.reqState = reqState;
  reqArgs.coexReq = coexReq;
  reqArgs.cb = cb;
  COEX_HAL_CallAtomic((COEX_AtomicCallback_t)setCoexReqCallbackPtr,
                      &reqArgs);
  return true;
}

static void updateReqList(COEX_ReqState_t *reqState)
{
  bool reqFound = false;
  COEX_ReqState_t** current;
  COEX_Req_t combinedRequestState = reqState->coexReq;
  for (current = &coexCfg.reqHead; *current != NULL; current = &((*current)->next)) {
    if (*current == reqState) {
      reqFound = true;
      if (reqState->coexReq == COEX_REQ_OFF) {
        // remove disabled request from the list, it has nothing to combine
        *current = (*current)->next;
        reqState->next = NULL;
        if (*current == NULL) {
          // break out the loop if the tail was deleted
          break;
        }
      }
    }
    combinedRequestState |= (*current)->coexReq;
  }
  if (!reqFound && (reqState->coexReq != COEX_REQ_OFF)) {
    // Insert new non-OFF entry at head of list
    reqState->next = coexCfg.reqHead;
    coexCfg.reqHead = reqState;
  }
  coexCfg.combinedRequestState = combinedRequestState;
}

static void setCoexReqCallback(COEX_ReqArgs_t *reqArgs)
{
  COEX_ReqState_t *reqState = reqArgs->reqState;
  COEX_Req_t coexReq = reqArgs->coexReq;

  if (((coexReq & COEX_REQ_ON) == 0U)
      && ((reqState->coexReq & COEX_REQ_ON) != 0U)
      && (reqState->cb != NULL)
      && ((reqState->coexReq & COEX_REQCB_OFF) != 0U)) {
    (*reqState->cb)(COEX_REQCB_OFF);
  }
  reqState->cb = reqArgs->cb;
  if (reqState->coexReq != coexReq) {
    reqState->coexReq = coexReq;
    updateReqList(reqState);
    coexUpdateReqIsr();
  }
}

static void coexRadioHoldOffPowerDown(void)
{
  if (coexCfg.radioOn) {
    coexCfg.radioOn = false;
    // When sleeping radio, no need to monitor RHO anymore
    disableGpioInt(rhoHandle); //clear RHO top level int enable
  }
}

static void coexRadioHoldOffPowerUp(void)
{
  if (!coexCfg.radioOn) {
    coexCfg.radioOn = true;
    if (rhoHandle != NULL) {
      // When waking radio, set up initial state and resume monitoring
      disableGpioInt(rhoHandle); //ensure RHO interrupt is off
      rhoCfg.cb(); // Manually call ISR to assess current state
      enableGpioInt(rhoHandle, NULL); //enable RHO interrupt
    }
  }
}

void setCoexPowerState(bool powerUp)
{
  if (setCoexPowerStateCallbackPtr != NULL) {
    setCoexPowerStateCallbackPtr(powerUp);
  }
}

static void setCoexPowerStateCallback(bool powerUp)
{
  if (powerUp) {
    coexRadioHoldOffPowerUp();
  } else {
    coexRadioHoldOffPowerDown();
  }
}

static void COEX_RHO_ISR(void)
{
  // Ack interrupt before reading GPIO to avoid potential of missing int
  clearGpioFlag(rhoHandle);
  // Notify Radio land of state change
  toggleCoexReq();
  coexNotifyRadio();
}

static bool (*enableCoexistenceCb)(void) = NULL;

static bool enableCoexistence(void)
{
  bool enabled = ((coexCfg.options & COEX_OPTION_COEX_ENABLED) != 0U);
  COEX_ReqState_t* reqPtr;

  if (enabled) {
    setCoexReqCallbackPtr = &setCoexReqCallback;
  }
  for (reqPtr = coexCfg.reqHead; reqPtr != NULL; reqPtr = reqPtr->next) {
    COEX_SetRequest(reqPtr, COEX_REQ_OFF, NULL);
  }
  if (!enabled) {
    setCoexReqCallbackPtr = NULL;
  }
  (*coexEventCallback)(COEX_EVENT_COEX_CHANGED);
  coexNotifyRadio();
  return true;
}

static bool (*enableRadioHoldOffCb)(void) = NULL;

static bool enableRadioHoldOff(void)
{
  bool enabled = ((coexCfg.options & COEX_OPTION_RHO_ENABLED) != 0U);

  // Configure GPIO as input and if pulling, pull it toward deasserted state
  if (!enabled) {
    disableGpioInt(rhoHandle);
  }
  if (coexCfg.radioOn || !enabled) {
    coexNotifyRadio(); //Notify Radio land of current state
  }
  if (coexCfg.radioOn && enabled) {
    enableGpioInt(rhoHandle, NULL);
  }
  return true;
}

static bool coexHoldOffActive(void)
{
  return ((coexCfg.options & COEX_OPTION_COEX_ENABLED) != 0U)
         && ((!getGpioOut(reqHandle, true)       // not REQUESTing or
              || !isGpioInSet(gntHandle, true) ) );    // REQUEST not GRANTed
}

static bool radioHoldOffActive(void)
{
  return ((coexCfg.options & COEX_OPTION_RHO_ENABLED) != 0U)
         && isGpioInSet(rhoHandle, false);
}

static void coexNotifyRadio(void)
{
  bool coexRho = coexHoldOffActive() || radioHoldOffActive();
  if (!coexRho) {
    coexCfg.requestDenied = false;
  }
  setCoexOption(COEX_OPTION_HOLDOFF_ACTIVE, coexRho);
  coexEventCallback(COEX_EVENT_HOLDOFF_CHANGED);
}

bool COEX_IsEnabled(void)
{
  return (rhoHandle != NULL)
         || (gntHandle != NULL)
         || (reqHandle != NULL);
}

#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
static uint8_t gpioInputOverride = 0U;
static COEX_GpioHandle_t overrideGpioHandles[COEX_GPIO_INDEX_COUNT];

bool COEX_GetGpioInputOverride(COEX_GpioIndex_t gpioIndex)
{
  return gpioInputOverride & (1U << gpioIndex);
}

bool COEX_SetGpioInputOverride(COEX_GpioIndex_t gpioIndex, bool enable)
{
  uint8_t gpioMask = (1U << gpioIndex);

  bool oldValue = ((gpioInputOverride & gpioMask) != 0U);
  if ((gpioIndex == 0U) || (gpioIndex >= COEX_GPIO_INDEX_COUNT)) {
    return false;
  }
  if (oldValue != enable) {
    if (enable) {
      gpioInputOverride |= gpioMask;
    } else {
      gpioInputOverride &= ~gpioMask;
    }
    if (gpioIndex != COEX_GPIO_INDEX_INTERNAL_REQ) {
      setGpio(overrideGpioHandles[gpioIndex], enable);
      setGpioFlag(overrideGpioHandles[gpioIndex]);
    }
  }
  return true;
}
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT

bool COEX_ConfigPhySelect(COEX_GpioHandle_t gpioHandle)
{
#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  overrideGpioHandles[COEX_GPIO_INDEX_PHY_SELECT] = gpioHandle;
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  // Register chip specific PHY select interrupt
  configGpio(gpioHandle, &phySelectHandle, &phySelectCfg);
  COEX_PHY_SEL_ISR();
  return true;
}

#ifdef SL_RAIL_UTIL_COEX_WIFI_TX_PORT
bool COEX_ConfigWifiTx(COEX_GpioHandle_t gpioHandle)
{
#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  overrideGpioHandles[COEX_GPIO_INDEX_WIFI_TX] = gpioHandle;
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  // Register chip specific Wifi Tx interrupt
  configGpio(gpioHandle, &wifiTxHandle, &wifiTxCfg);
  return true;
}
#endif

bool COEX_ConfigRadioHoldOff(COEX_GpioHandle_t gpioHandle)
{
#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  overrideGpioHandles[COEX_GPIO_INDEX_RHO] = gpioHandle;
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  setCoexPowerStateCallbackPtr = &setCoexPowerStateCallback;

  // Register chip specific RHO interrupt
  configGpio(gpioHandle, &rhoHandle, &rhoCfg);
  enableRadioHoldOffCb = &enableRadioHoldOff;
  enableRadioHoldOff();

  return true;
}

bool COEX_ConfigPriority(COEX_GpioHandle_t gpioHandle)
{
  if ((coexCfg.options & COEX_OPTION_PRI_SHARED) != 0U) {
    priCfg.options |= COEX_GPIO_OPTION_SHARED;
  } else {
    priCfg.options &= ~COEX_GPIO_OPTION_SHARED;
  }
  configGpio(gpioHandle, &priHandle, &priCfg);
  return true;
}

bool COEX_ConfigPwmRequest(COEX_GpioHandle_t gpioHandle)
{
  configGpio(gpioHandle, &pwmReqHandle, &pwmReqCfg);
  return true;
}

bool COEX_ConfigGrant(COEX_GpioHandle_t gpioHandle)
{
#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  overrideGpioHandles[COEX_GPIO_INDEX_GNT] = gpioHandle;
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  configGpio(gpioHandle, &gntHandle, &gntCfg);
  return true;
}

#if SL_RAIL_UTIL_COEX_OUTPUT_OVERRIDE_GPIO_INPUT
bool COEX_ConfigExternalRequest(COEX_GpioHandle_t gpioHandle)
{
  overrideGpioHandles[COEX_GPIO_INDEX_REQ] = gpioHandle;
  configGpio(gpioHandle, &externalReqHandle, &externalReqCfg);
  return true;
}
#endif //SL_RAIL_UTIL_COEX_OUTPUT_OVERRIDE_GPIO_INPUT

bool COEX_ConfigRequest(COEX_GpioHandle_t gpioHandle)
{
#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  overrideGpioHandles[COEX_GPIO_INDEX_INTERNAL_REQ] = gpioHandle;
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  if ((coexCfg.options & COEX_OPTION_REQ_SHARED) != 0U) {
    reqCfg.options |= COEX_GPIO_OPTION_SHARED;
  } else {
    reqCfg.options &= ~COEX_GPIO_OPTION_SHARED;
  }
  configGpio(gpioHandle, &reqHandle, &reqCfg);

  enableCoexistenceCb = &enableCoexistence;
  enableCoexistence();

  return true;
}

bool COEX_SetOptions(COEX_Options_t options)
{
  COEX_Options_t changedOptions = (COEX_Options_t)(coexCfg.options ^ options);

  coexCfg.options = options;
  if ((changedOptions & COEX_OPTION_RHO_ENABLED) != 0U) {
    if (enableRadioHoldOffCb == NULL) {
      return false;
    }
    enableRadioHoldOffCb();
  }
  if ((changedOptions & COEX_OPTION_COEX_ENABLED) != 0U) {
    if (enableCoexistenceCb == NULL) {
      return false;
    }
    enableCoexistenceCb();
  }
  return true;
}

COEX_Options_t COEX_GetOptions(void)
{
  return coexCfg.options;
}

static void COEX_PHY_SEL_ISR(void)
{
  disableGpioInt(phySelectHandle);
  coexEventCallback(COEX_EVENT_PHY_SELECT_CHANGED);
}

static void enablePhySelectIsr(void *args)
{
  (void)args;
  enableGpioInt(phySelectHandle, NULL);
  if (isGpioInSet(phySelectHandle, false)) {
    COEX_PHY_SEL_ISR();
  }
}

void COEX_EnablePhySelectIsr(bool enable)
{
  if (enable) {
    clearGpioFlag(phySelectHandle);
    COEX_HAL_CallAtomic(enablePhySelectIsr, NULL);
  } else {
    disableGpioInt(phySelectHandle);
  }
}

#if COEX_HAL_FAST_REQUEST
#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
#define COEX_ReadRequest() isGpioInSet(reqHandle, false)
#else //!SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
#define COEX_ReadRequest() COEX_HAL_ReadRequest()
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT

__STATIC_INLINE void COEX_SetPriorityAndRequest(bool request, bool priority)
{
  // We are purposely defining every combination of request
  // and priority. This reduces the delay between setting
  // the 3 coex GPIOs.  If all 3 coex registers are on the
  // same port, all GPIOs can be set simultaneously.
  if (request) {
    if (priority) {
#ifdef COEX_HAL_CONFIG_DP_PRS
      //Setup PRS priority config before enabling request
      COEX_HAL_SetPriority();
#endif //COEX_HAL_CONFIG_DP_PRS
      COEX_HAL_SetPwmRequest();
      COEX_HAL_SetRequest();
#ifndef COEX_HAL_CONFIG_DP_PRS
      COEX_HAL_SetPriority();
#endif //COEX_HAL_CONFIG_DP_PRS
    } else {
      COEX_HAL_SetPwmRequest();
      COEX_HAL_SetRequest();
      COEX_HAL_ClearPriority();
    }
  } else {
    if (priority) {
      COEX_HAL_SetPwmRequest();
      COEX_HAL_ClearRequest();
      COEX_HAL_SetPriority();
    } else {
      COEX_HAL_SetPwmRequest();
      COEX_HAL_ClearRequest();
      COEX_HAL_ClearPriority();
    }
  }
}
__STATIC_INLINE void COEX_ClearRequestAndPriority(void)
{
  COEX_HAL_ClearPriority();
  COEX_HAL_ClearRequest();
  COEX_HAL_ClearPwmRequest();
}
#else //!COEX_HAL_FAST_REQUEST
#define COEX_ReadRequest() isGpioInSet(reqHandle, false)
__STATIC_INLINE void COEX_SetPriorityAndRequest(bool request, bool priority)
{
  setGpio(pwmReqHandle, true);
  setGpio(reqHandle, request);
  setGpio(priHandle, priority);
}
__STATIC_INLINE void COEX_ClearRequestAndPriority(void)
{
  setGpio(priHandle, false);
  setGpio(reqHandle, false);
  setGpio(pwmReqHandle, false);
}
#endif //!COEX_HAL_FAST_REQUEST

#if defined(__ICCARM__) /* IAR */
#pragma optimize=speed
#elif __clang__
__attribute__((optnone))
#elif defined(__GNUC__)
__attribute__((optimize("-Ofast")))
#else
// Don't know how to optimize unsupported compiler
#endif
// This IRQ is triggered on the negate REQUEST edge,
// needed only when REQUEST signal is shared,
// and not piggybacking GNT and REQ on same IRQ.
static void COEX_REQ_ISR(void)
{
  // External REQUEST deasserted so we can assert ours
  //clearGpioFlag(reqHandle) // Covered within disableGpioInt(reqHandle)
  disableGpioInt(reqHandle); // This is a one-shot event
  coexReqRandomBackoff();
  coexUpdateReqIsr();
}

#if defined(__ICCARM__) /* IAR */
#pragma optimize=speed
#elif __clang__
__attribute__((optnone))
#elif defined(__GNUC__)
__attribute__((optimize("-Ofast")))
#else
// Don't know how to optimize unsupported compiler
#endif

#if SL_RAIL_UTIL_COEX_DP_ENABLED
__STATIC_INLINE void pulseDirectionalPriority(void)
{
  COEX_Req_t combinedReqState = coexCfg.combinedRequestState; // Local non-volatile flavor avoids warnings
  bool highPriority = ((combinedReqState & COEX_REQ_HIPRI) != 0U);
  bool setRequest = ((combinedReqState & COEX_REQ_ON) != 0U);

  // DP priority possible
  // Not in Direction PRIORITY bypass?
  if ((COEX_HAL_GetDpPulseWidth() != 0)
      // coexUpdateReqIsr() triggered by REQUEST changing, not PWM changing?
      && (setRequest != getGpioOut(reqHandle, false))
      // PRIORITY changing requiring a new priority pulse?
      && (highPriority && !COEX_HAL_GetPriority())) {
    COEX_ClearRequestAndPriority(); // then deaasert all PTA signals
  }
}
#else
#define pulseDirectionalPriority() //no-op
#endif

// Must be called with interrupts disabled
__STATIC_INLINE void coexUpdateReqIsr(void)
{
  COEX_Req_t combinedReqState = coexCfg.combinedRequestState; // Local non-volatile flavor avoids warnings
  bool myReq = ((combinedReqState & (COEX_REQ_ON | COEX_REQ_PWM)) != 0U); // I need to REQUEST
  bool force = ((combinedReqState & COEX_REQ_FORCE) != 0U); // (ignoring others)
  bool exReq; // external requestor?

  if (!isReqShared() || getGpioOut(reqHandle, false)) {  // in GRANT phase
    exReq = false;                // ignore external requestors
  } else {                        // in REQUEST phase
    clearGpioFlag(reqHandle);  // Before sampling REQUEST, avoids race
    exReq = COEX_ReadRequest();
  }
  if (myReq) {                    // want to assert REQUEST
    if (force || !exReq) {        // can assert REQUEST
      if (!getGpioOut(reqHandle, false)) {
        // Assume request denied until request is granted
        coexCfg.requestDenied = true;
      }
      enableGpioInt(gntHandle, &gntWasAsserted);
      bool highPriority = ((combinedReqState & COEX_REQ_HIPRI) != 0U);
      // If COEX_REQ_ON is not set this must be a PWM request
      // If request is not shared map PWM requests to the standard request
      bool setRequest = ((combinedReqState & COEX_REQ_ON) != 0U) || !isReqShared();
      pulseDirectionalPriority();
      COEX_SetPriorityAndRequest(setRequest, highPriority);

      // Issue callbacks on REQUEST assertion
      // These are one-shot callbacks
      COEX_ReqState_t* reqPtr;
      for (reqPtr = coexCfg.reqHead; reqPtr != NULL; reqPtr = reqPtr->next) {
        if ((reqPtr->cb != NULL) && ((reqPtr->coexReq & COEX_REQCB_REQUESTED) != 0U)) {
          reqPtr->coexReq &= ~COEX_REQCB_REQUESTED;
          (*reqPtr->cb)(COEX_REQCB_REQUESTED);
        }
      }
      setGpioFlag(gntHandle); // Manually force GRANT check if missed/no edge
    } else {                      // must wait for REQUEST
      enableGpioInt(reqHandle, NULL);
    }
  } else {                        // negate REQUEST
    if (getGpioOut(reqHandle, false) ) {
      COEX_Events_t coexEvents = COEX_EVENT_REQUEST_RELEASED;
      if (coexCfg.requestDenied) {
        coexCfg.requestDenied = false;
        coexEvents |= COEX_EVENT_REQUEST_DENIED;
      }
      coexEventCallback(coexEvents);
    }
    COEX_ClearRequestAndPriority();
    disableGpioInt(gntHandle);
    if (isReqShared()) {
      disableGpioInt(reqHandle);
    }
  }
  coexNotifyRadio(); // Reassess (assert) RHO
}

void COEX_InitHalConfigOptions(void)
{
  COEX_Options_t options = COEX_OPTION_NONE;

  #ifdef SL_RAIL_UTIL_COEX_RHO_PORT
  options |= COEX_OPTION_RHO_ENABLED;
  #endif //SL_RAIL_UTIL_COEX_RHO_PORT
  #if defined(SL_RAIL_UTIL_COEX_REQ_PORT) || defined(SL_RAIL_UTIL_COEX_GNT_PORT)
  options |= COEX_OPTION_COEX_ENABLED;
  #endif //defined(SL_RAIL_UTIL_COEX_REQ_PORT) || defined(SL_RAIL_UTIL_COEX_GNT_PORT)
  #if SL_RAIL_UTIL_COEX_REQ_BACKOFF
  options |= (COEX_OPTION_MAX_REQ_BACKOFF_MASK
              & SL_RAIL_UTIL_COEX_REQ_BACKOFF);
  #endif //SL_RAIL_UTIL_COEX_REQ_BACKOFF
  #if SL_RAIL_UTIL_COEX_REQ_SHARED
  options |= COEX_OPTION_REQ_SHARED;
  #endif //SL_RAIL_UTIL_COEX_REQ_SHARED
  #if SL_RAIL_UTIL_COEX_PRI_SHARED && !SL_RAIL_UTIL_COEX_DP_ENABLED
  options |= COEX_OPTION_PRI_SHARED;
  #endif //SL_RAIL_UTIL_COEX_PRI_SHARED && !SL_RAIL_UTIL_COEX_DP_ENABLED
  #if SL_RAIL_UTIL_COEX_DP_ENABLED
  COEX_HAL_ConfigDp(SL_RAIL_UTIL_COEX_DP_PULSE_WIDTH_US);
  #endif
  COEX_SetOptions(options);
}
#endif //COEX_HAL_DISABLED
