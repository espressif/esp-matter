/***************************************************************************//**
 * @file
 * @brief Radio coexistence BLE utilities
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

#include <stdlib.h>
#include <string.h>
#include <em_assert.h>
#include <em_core.h>
#include <em_cmu.h>
#include <rail.h>

#include "coexistence-ble-ll.h"

#include "coexistence-ble.h"
#include "coexistence-hal.h"

#include "rail_ble.h"

struct {
  uint16_t requestWindow;
  uint8_t scheduledPriority;
  bool txAbort : 1;
  bool scheduledRequest : 1;
  bool scheduled : 1;
  bool pwmEnable : 1; //PWM is toggling
  bool scanPwmEnable : 1; // SCANPWM is used if requested by stack
  bool scanPwmActive : 1; // SCANPWM is currently active
  bool scanPwmToggling : 1; //SCANPWM timer is scheduled
  bool scanPwmOn : 1; //SCANPWM signal phase
  bool pwmPriority : 1;
  bool enablePriority : 1;
  bool pullResistor : 1;
  sl_bt_ll_coex_config_t config;
  RAIL_MultiTimer_t timer;
  RAIL_Handle_t handle;
  sl_bt_coex_abort_tx_callback abortTx;
  sl_bt_coex_fast_random_callback fastRandom;
  uint16_t requestWindowCalibration;
  uint8_t requestBackoffMax;
  COEX_ReqState_t reqState;
  COEX_ReqState_t scanPwmState;
#if SL_RAIL_UTIL_COEX_BLE_SIGNAL_IDENTIFIER_ENABLED
  COEX_ReqState_t signalIdentifierReqState;
#endif
} ll_coex;

COEX_Events_t sli_bt_coex_event_filter = ~COEX_EVENT_REQUEST_EVENTS;

static inline bool isCoexEnabled(void)
{
  return COEX_GetOptions() & COEX_OPTION_COEX_ENABLED;
}

/* Update constant pwm state */
static void sl_bt_updatepwm(void)
{
  COEX_Req_t req;
  if (ll_coex.pwmEnable) {
    req = COEX_REQ_PWM | (ll_coex.pwmPriority ? COEX_REQ_HIPRI : 0);
  } else {
    req = COEX_REQ_OFF;
  }
  COEX_SetPwmRequest(req,
                     NULL,
                     ll_coex.config.coex_pwm_dutycycle,
                     ll_coex.config.coex_pwm_period * 2);
}

static void setCoexOptions(COEX_Options_t mask, COEX_Options_t values)
{
  //No bits outside mask must be set
  EFM_ASSERT((~mask & values) == 0);
  //Get existing options, mask out enable bit
  COEX_Options_t options = COEX_GetOptions() & ~mask;
  COEX_SetOptions(options | values);
}

static void scanPwmRequest(bool request);
// Set/clear request and priority signals.
static void setRequest(bool request, uint8_t priority);
// Set/clear request and priority signals
static void coexRequest(bool request, uint8_t priority);
// Update grant signal state
static void coexUpdateGrant(bool abortTx);
// Timer event handler to set delayed COEX request
static void coexHandleTimerEvent(struct RAIL_MultiTimer *tmr,
                                 RAIL_Time_t expectedTimeOfEvent,
                                 void *cbArg);
// Timer event handler for SCANPWM
static void coexHandlePwmTimerEvent(struct RAIL_MultiTimer *tmr,
                                    RAIL_Time_t expectedTimeOfEvent,
                                    void *cbArg);

/**
 * @brief Initialize coex from Link Layer side
 *
 */
void sl_bt_ll_coex_set_context(RAIL_Handle_t handle, sl_bt_coex_abort_tx_callback abortTx, sl_bt_coex_fast_random_callback fastRandom)
{
  memset(&ll_coex, 0, sizeof(ll_coex));

  ll_coex.handle = handle;
  ll_coex.abortTx = abortTx;
  ll_coex.fastRandom = fastRandom;
}

uint16_t sl_bt_ll_coex_fast_random(void)
{
  return ll_coex.fastRandom();
}

void sl_bt_coex_set_config(sl_bt_ll_coex_config_t *config)
{
  ll_coex.config = *config; // struct copy
}

//set request and start scanPwm if in use
static void startRequest(bool request, uint8_t priority)
{
  //if either period and dutycycle are 0, do not use scanPwm
  if (ll_coex.scanPwmEnable == false
      || ll_coex.scanPwmActive == false
      || request == false
      || ll_coex.config.coex_pwm_period == 0
      || ll_coex.config.coex_pwm_dutycycle >= 100) {
    coexRequest(request, priority);
    return;
  } else if (ll_coex.config.coex_pwm_dutycycle == 0) {
    //If dutycycle is 0, then always disable
    coexRequest(false, priority);
    return;
  }

  //Get phase
  uint32_t period = ll_coex.config.coex_pwm_period * 1000UL;
  uint32_t phase = RAIL_GetTime() % period;
  uint32_t ontime;
  if (phase * 100 < period * ll_coex.config.coex_pwm_dutycycle) {
    ll_coex.scanPwmOn = true;
    ontime = period * ll_coex.config.coex_pwm_dutycycle / 100 - phase;
  } else {
    ll_coex.scanPwmOn = false;
    ontime = period - phase;
  }

  //Make sure the request line is off
  coexRequest(false, 0);

  ll_coex.scanPwmToggling = true;

  //Toggle scanPwm line
  scanPwmRequest(ll_coex.scanPwmOn);

  RAIL_SetMultiTimer(&ll_coex.timer,
                     ontime,
                     RAIL_TIME_DELAY,
                     &coexHandlePwmTimerEvent,
                     NULL);
}

static void coexHandlePwmTimerEvent(struct RAIL_MultiTimer *tmr,
                                    RAIL_Time_t expectedTimeOfEvent,
                                    void *cbArg)
{
  (void)tmr;
  (void)expectedTimeOfEvent;
  (void)cbArg;
  if (!ll_coex.scanPwmToggling) {
    return;
  }

  startRequest(ll_coex.scheduledRequest, ll_coex.scheduledPriority);
}

void sl_bt_ll_coex_set_request_window(uint16_t requestWindow)
{
  ll_coex.requestWindow = requestWindow;
}

void sl_bt_ll_coex_update_grant(bool abort)
{
  coexUpdateGrant(abort && ll_coex.txAbort);
}

void sl_bt_ll_coex_request_delayed(uint32_t time, bool request, bool scanPwmActive, uint8_t priority)
{
  if (!isCoexEnabled()) {
    return;
  }

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();
  if (ll_coex.scheduled || ll_coex.scanPwmToggling) {
    ll_coex.scheduled = false;
    ll_coex.scanPwmToggling = false;
    RAIL_CancelMultiTimer(&ll_coex.timer);
    scanPwmRequest(false);
  }
  ll_coex.scanPwmActive = scanPwmActive;

  int ret = RAIL_SetMultiTimer(&ll_coex.timer,
                               time - ll_coex.requestWindow,
                               RAIL_TIME_ABSOLUTE,
                               &coexHandleTimerEvent,
                               NULL);
  if (ret) {
    // timer setting failed, request immediately
    coexRequest(request, priority);
  } else {
    // clear request now and wait for timer event to request later
    ll_coex.scheduled = true;
    ll_coex.scheduledRequest = request;
    ll_coex.scheduledPriority = priority;
    coexRequest(false, 0xff);
  }

  CORE_EXIT_ATOMIC();
}

void sl_bt_ll_coex_request(bool request, bool scanPwmActive, uint8_t priority)
{
  if (!isCoexEnabled()) {
    return;
  }

  if (request == true
      && scanPwmActive == false
      && (ll_coex.reqState.coexReq & COEX_REQ_ON)
      && (priority > ll_coex.config.threshold_coex_pri)) {
    //If requesting immediately and already requested, inherit previous request without updating
    //If priority is high then allow increasing
    return;
  }

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();

  if ((ll_coex.scheduled  || ll_coex.scanPwmToggling) && request == false && scanPwmActive == false) {
    ll_coex.scheduled = false;
    ll_coex.scanPwmToggling = false;
    RAIL_CancelMultiTimer(&ll_coex.timer);
  }

  //cache priority for scanPwm
  ll_coex.scheduledPriority = priority;
  ll_coex.scanPwmActive = scanPwmActive;

  startRequest(request, priority);

  //Make sure scanPwm is disabled
  if (!scanPwmActive) {
    scanPwmRequest(false);
  }

  CORE_EXIT_ATOMIC();
}

static void coexHandleTimerEvent(struct RAIL_MultiTimer *tmr,
                                 RAIL_Time_t expectedTimeOfEvent,
                                 void *cbArg)
{
  (void)tmr;
  (void)expectedTimeOfEvent;
  (void)cbArg;

  if (!ll_coex.scheduled) {
    return;
  }

  ll_coex.scheduled = false;
  startRequest(ll_coex.scheduledRequest, ll_coex.scheduledPriority);
}

static void coexUpdateGrant(bool abortTx)
{
  bool grant = sl_bt_coex_tx_allowed();
  sli_bt_coex_counter_grant_update(grant);
  RAIL_EnableTxHoldOff(ll_coex.handle, !grant);

  if (abortTx && !grant) {
    EFM_ASSERT(ll_coex.abortTx);
    ll_coex.abortTx();
  }
}

static void coexRequest(bool request, uint8_t priority)
{
  //Priority too low for pta request, make sure existing request is disabled
  if (priority > ll_coex.config.threshold_coex_req && request == true) {
    setRequest(false, priority);
    return;
  }

  setRequest(request, priority);
  if (request) {
    coexUpdateGrant(false);
  }
}

void sl_bt_enable_coex_pull_resistor(bool enable)
{
  ll_coex.pullResistor = enable;
}

static void scanPwmRequest(bool request)
{
#if SL_RAIL_UTIL_COEX_BLE_SIGNAL_IDENTIFIER_ENABLED
  RAIL_BLE_EnableSignalDetection(ll_coex.handle, request);
#else
  COEX_SetRequest(&ll_coex.scanPwmState, (request ? COEX_REQ_PWM : COEX_REQ_OFF) | (ll_coex.pwmPriority ? COEX_REQ_HIPRI : COEX_REQ_OFF), NULL);
#endif
}

/**
 * Set/clear request and priority signals.
 */
static void setRequest(bool request, uint8_t priority)
{
  bool priorityState = ll_coex.enablePriority && (priority <= ll_coex.config.threshold_coex_pri);
  sli_bt_coex_counter_request(request, priorityState);
  if (request) {
    sli_bt_coex_event_filter |= COEX_EVENT_REQUEST_EVENTS;
  } else {
    sli_bt_coex_event_filter &= ~COEX_EVENT_REQUEST_EVENTS;
  }
  if (request) {
    COEX_SetRequest(&ll_coex.reqState, COEX_REQ_ON | (priorityState ? COEX_REQ_HIPRI : 0), NULL);
  } else {
    COEX_SetRequest(&ll_coex.reqState, COEX_REQ_OFF, NULL);
  }
}

/**
 * Get the state of grant signal.
 */
bool sl_bt_coex_tx_allowed(void)
{
  return ((COEX_GetOptions() & COEX_OPTION_HOLDOFF_ACTIVE) == 0U);
}

void sli_bt_coex_radio_callback(COEX_Events_t events)
{
  if (ll_coex.handle == NULL) {
    return;
  }
  if (events & COEX_EVENT_HOLDOFF_CHANGED) {
    coexUpdateGrant(true);
  }
  if (events & COEX_EVENT_REQUEST_DENIED) {
    sli_bt_coex_counter_increment_denied();
  }
}
/**
 * Initialise the coex from application side.
 */
void sl_bt_init_coex(const sl_bt_coex_init_t *coexInit)
{
  ll_coex.requestWindowCalibration = coexInit->requestWindowCalibration;

  COEX_HAL_Init();
  ll_coex.requestWindow = coexInit->requestWindow + coexInit->requestWindowCalibration;

  //Set default coex parameters
  sl_bt_ll_coex_config_t cfg = SL_BT_COEX_DEFAULT_CONFIG;
  sl_bt_coex_set_config(&cfg);
  sl_bt_set_coex_options(SL_BT_COEX_OPTION_MASK
                         | SL_BT_COEX_OPTION_REQUEST_BACKOFF_MASK,
                         coexInit->options);
  sli_bt_coex_radio_callback(COEX_EVENT_HOLDOFF_CHANGED);
  //Enable signal for early packet reception
  RAIL_ConfigEvents(ll_coex.handle, RAIL_EVENT_RX_SYNC1_DETECT, RAIL_EVENT_RX_SYNC1_DETECT);

#if SL_RAIL_UTIL_COEX_RHO_ENABLED
  setCoexPowerState(true);
#endif //SL_RAIL_UTIL_COEX_RHO_ENABLED
#if SL_RAIL_UTIL_COEX_BLE_SIGNAL_IDENTIFIER_ENABLED
  RAIL_BLE_ConfigSignalIdentifier(ll_coex.handle,
                                  (RAIL_BLE_SignalIdentifierMode_t)SL_RAIL_UTIL_COEX_BLE_SIGNAL_IDENTIFIER_MODE);
#endif
}

RAIL_Events_t ll_radioFilterEvents(RAIL_Handle_t ll_radioHandle, RAIL_Events_t events)
{
  (void)ll_radioHandle;
  if (events & RAIL_EVENT_RX_SYNC1_DETECT) {
    //Request low priority if not already requested
    if (!(ll_coex.reqState.coexReq & COEX_REQ_ON)) {
      setRequest(true, ll_coex.scheduledPriority);
    }
  }
  if (events & (RAIL_EVENT_RX_PACKET_RECEIVED
                | RAIL_EVENT_RX_TIMEOUT
                | RAIL_EVENT_RX_SCHEDULED_RX_END
                | RAIL_EVENT_RSSI_AVERAGE_DONE
                | RAIL_EVENT_RX_PACKET_ABORTED)) {
    RAIL_StateTransitions_t transitions;
    RAIL_GetRxTransitions(ll_coex.handle, &transitions);
    if (transitions.success != RAIL_RF_STATE_TX) {
      setRequest(false, ll_coex.scheduledPriority);
    }
  }
  return events;
}

bool sl_bt_set_coex_options(uint32_t mask, uint32_t options)
{
  if (mask & SL_BT_COEX_OPTION_ENABLE) {
    bool enable = options & SL_BT_COEX_OPTION_ENABLE;
    if (!enable) {
      // disable request and cancel timer
      sl_bt_ll_coex_request(false, false, 0xff);
    }

    setCoexOptions(COEX_OPTION_COEX_ENABLED, enable ? COEX_OPTION_COEX_ENABLED : 0);
  }
  if (mask & SL_BT_COEX_OPTION_TX_ABORT) {
    ll_coex.txAbort = options & SL_BT_COEX_OPTION_TX_ABORT;
  }
  if (mask & SL_BT_COEX_OPTION_PRIORITY_ENABLE) {
    ll_coex.enablePriority = options & SL_BT_COEX_OPTION_PRIORITY_ENABLE;
  }
  if (mask & SL_BT_COEX_OPTION_PWM_PRIORITY) {
    ll_coex.pwmPriority = options & SL_BT_COEX_OPTION_PWM_PRIORITY;
  }
  if (mask & SL_BT_COEX_OPTION_SCANPWM_ENABLE) {
    ll_coex.scanPwmEnable = options & SL_BT_COEX_OPTION_SCANPWM_ENABLE;
  }
  if (mask & SL_BT_COEX_OPTION_PWM_ENABLE) {
    ll_coex.pwmEnable = options & SL_BT_COEX_OPTION_PWM_ENABLE;
    sl_bt_updatepwm();
  }

  if (mask & SL_BT_COEX_OPTION_REQUEST_BACKOFF_MASK) {
    ll_coex.requestBackoffMax = (options & SL_BT_COEX_OPTION_REQUEST_BACKOFF_MASK)
                                >> SL_BT_COEX_OPTION_REQUEST_BACKOFF_SHIFT;
  }

  if (mask & SL_BT_COEX_OPTION_REQUEST_WINDOW_MASK) {
    uint16_t requestWindow = ((options & SL_BT_COEX_OPTION_REQUEST_WINDOW_MASK)
                              >> SL_BT_COEX_OPTION_REQUEST_WINDOW_SHIFT)
                             + ll_coex.requestWindowCalibration;
    sl_bt_ll_coex_set_request_window(requestWindow);
  }

  return true;
}

RAIL_Events_t sl_bt_ll_coex_get_events(void)
{
#if SL_RAIL_UTIL_COEX_BLE_SIGNAL_IDENTIFIER_ENABLED
  RAIL_Events_t events = (RAIL_EVENT_RX_SYNC1_DETECT
                          | RAIL_EVENT_RX_SYNC2_DETECT
                          | RAIL_EVENT_SIGNAL_DETECTED);
  return events;
#else
  return RAIL_EVENTS_NONE;
#endif
}

#if SL_RAIL_UTIL_COEX_BLE_SIGNAL_IDENTIFIER_ENABLED
static RAIL_MultiTimer_t channelSwitchTimer;
#define RAIL_UTIL_COEX_BLE_CHANNEL_SWITCH_TIME 30U
extern void ll_scanHopToNextChannel(uint32_t minTimeToHop);

static void channelSwitchTimerCb(RAIL_MultiTimer_t *tmr,
                                 RAIL_Time_t expectedTimeOfEvent,
                                 void *cbArg)
{
  (void)tmr;
  (void)expectedTimeOfEvent;
  (void)cbArg;
  COEX_SetRequest(&ll_coex.signalIdentifierReqState, COEX_REQ_ON, NULL);
  ll_scanHopToNextChannel(SL_RAIL_UTIL_COEX_BLE_MIN_TIME_FOR_HOPPING);
}
#endif

void sl_bt_ll_coex_handle_events(RAIL_Events_t events)
{
  if (!isCoexEnabled()) {
    return;
  }
  switch (events) {
    case RAIL_EVENT_RX_PACKET_RECEIVED:
      sl_bt_ll_coex_update_grant(false);
    #if SL_RAIL_UTIL_COEX_BLE_SIGNAL_IDENTIFIER_ENABLED
      COEX_SetRequest(&ll_coex.signalIdentifierReqState, COEX_REQ_OFF, NULL);
    #endif
      break;
    case RAIL_EVENT_RX_SYNC1_DETECT:
    case RAIL_EVENT_RX_SYNC2_DETECT:
    #if SL_RAIL_UTIL_COEX_BLE_SIGNAL_IDENTIFIER_ENABLED
      RAIL_CancelMultiTimer(&channelSwitchTimer);
      COEX_SetRequest(&ll_coex.signalIdentifierReqState, COEX_REQ_ON, NULL);
    #endif
      break;
    case RAIL_EVENT_SIGNAL_DETECTED:
    #if SL_RAIL_UTIL_COEX_BLE_SIGNAL_IDENTIFIER_ENABLED
      RAIL_SetMultiTimer(&channelSwitchTimer, RAIL_UTIL_COEX_BLE_CHANNEL_SWITCH_TIME, RAIL_TIME_DELAY, &channelSwitchTimerCb, NULL);
    #endif
      break;
    case RAIL_EVENT_RX_TIMEOUT:
    case RAIL_EVENT_RX_SCHEDULED_RX_END:
    case RAIL_EVENT_RX_PACKET_ABORTED:
    #if SL_RAIL_UTIL_COEX_BLE_SIGNAL_IDENTIFIER_ENABLED
      COEX_SetRequest(&ll_coex.signalIdentifierReqState, COEX_REQ_OFF, NULL);
    #endif
      break;
    default:
      break;
  }
}

SL_WEAK void sli_bt_coex_counter_increment_denied(void)
{
}

SL_WEAK void sli_bt_coex_counter_request(bool request, bool priority)
{
  (void)request;
  (void)priority;
}

SL_WEAK void sli_bt_coex_counter_grant_update(bool state)
{
  (void)state;
}
