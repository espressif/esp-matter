/***************************************************************************//**
 * @file
 * @brief 802.15.4 Specific Radio Coexistence callbacks.
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

#ifdef PLATFORM_HEADER
#include PLATFORM_HEADER

#include "stack/include/ember.h"
#include "stack/include/ember-types.h"
#include "include/error.h"
#include "hal/hal.h"
#endif //PLATFORM_HEADER

#include "coexistence-hal.h"
#include "coexistence/protocol/ieee802154/coexistence-802154.h"
#include "coexistence/protocol/ieee802154/coulomb-counter-802154.h"

#ifdef RTOS
  #include "rtos/rtos.h"
#endif

#if defined(DEBUG_PTA) || defined(RHO_GPIO) || defined(SL_RAIL_UTIL_COEX_RHO_PORT)
#define COEX_RHO_SUPPORT 1
#endif //defined(DEBUG_PTA) || defined(RHO_GPIO) || defined(SL_RAIL_UTIL_COEX_RHO_PORT)

#if defined(DEBUG_PTA) || defined(PTA_REQ_GPIO) || defined(SL_RAIL_UTIL_COEX_REQ_PORT) \
  || defined(PTA_GNT_GPIO) || defined(SL_RAIL_UTIL_COEX_GNT_PORT) || defined(SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT)
#define COEX_SUPPORT 1
#endif //defined(DEBUG_PTA) || defined(PTA_REQ_GPIO) || defined(SL_RAIL_UTIL_COEX_REQ_PORT)
//|| defined(PTA_GNT_GPIO) || defined(SL_RAIL_UTIL_COEX_GNT_PORT) || defined(SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT)

#if SL_RAIL_UTIL_COEX_ACKHOLDOFF || defined(RHO_GPIO)
  #define DEFAULT_PTA_OPT_ACK_HOLDOFF PTA_OPT_ACK_HOLDOFF
#else //!(SL_RAIL_UTIL_COEX_ACKHOLDOFF || defined(RHO_GPIO))
  #define DEFAULT_PTA_OPT_ACK_HOLDOFF PTA_OPT_DISABLED
#endif //SL_RAIL_UTIL_COEX_ACKHOLDOFF || defined(RHO_GPIO)

#if SL_RAIL_UTIL_COEX_TX_ABORT
  #define DEFAULT_PTA_OPT_ABORT_TX PTA_OPT_ABORT_TX
#else //!SL_RAIL_UTIL_COEX_TX_ABORT
  #define DEFAULT_PTA_OPT_ABORT_TX PTA_OPT_DISABLED
#endif //SL_RAIL_UTIL_COEX_TX_ABORT

#if SL_RAIL_UTIL_COEX_TX_HIPRI
  #define DEFAULT_PTA_OPT_TX_HIPRI PTA_OPT_TX_HIPRI
#else //!SL_RAIL_UTIL_COEX_TX_HIPRI
  #define DEFAULT_PTA_OPT_TX_HIPRI PTA_OPT_DISABLED
#endif //SL_RAIL_UTIL_COEX_TX_HIPRI

#if SL_RAIL_UTIL_COEX_RX_HIPRI
  #define DEFAULT_PTA_OPT_RX_HIPRI PTA_OPT_RX_HIPRI
#else //!SL_RAIL_UTIL_COEX_RX_HIPRI
  #define DEFAULT_PTA_OPT_RX_HIPRI PTA_OPT_DISABLED
#endif //SL_RAIL_UTIL_COEX_RX_HIPRI

#if SL_RAIL_UTIL_COEX_RETRYRX_HIPRI
  #define DEFAULT_PTA_OPT_RX_RETRY_HIPRI PTA_OPT_RX_RETRY_HIPRI
#else //!SL_RAIL_UTIL_COEX_RETRYRX_HIPRI
  #define DEFAULT_PTA_OPT_RX_RETRY_HIPRI PTA_OPT_DISABLED
#endif //SL_RAIL_UTIL_COEX_RETRYRX_HIPRI

#if SL_RAIL_UTIL_COEX_LONG_REQ
  #define DEFAULT_PTA_OPT_LONG_REQ PTA_OPT_LONG_REQ
#else //!SL_RAIL_UTIL_COEX_LONG_REQ
  #define DEFAULT_PTA_OPT_LONG_REQ PTA_OPT_DISABLED
#endif //SL_RAIL_UTIL_COEX_LONG_REQ

#if SL_RAIL_UTIL_COEX_TOGGLE_REQ_ON_MACRETRANSMIT
  #define DEFAULT_PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT
#else //!SL_RAIL_UTIL_COEX_TOGGLE_REQ_ON_MACRETRANSMIT
  #define DEFAULT_PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT PTA_OPT_DISABLED
#endif //SL_RAIL_UTIL_COEX_TOGGLE_REQ_ON_MACRETRANSMIT

#if defined(SL_RAIL_UTIL_COEX_RHO_PORT) || (defined(RADIO_HOLDOFF) && defined(RHO_GPIO))
  #define DEFAULT_PTA_OPT_RHO_ENABLED PTA_OPT_RHO_ENABLED
#else //(!defined(SL_RAIL_UTIL_COEX_RHO_PORT) || (defined(RADIO_HOLDOFF) && defined(RHO_GPIO)))
  #define DEFAULT_PTA_OPT_RHO_ENABLED PTA_OPT_DISABLED
#endif //defined(SL_RAIL_UTIL_COEX_RHO_PORT) || (defined(RADIO_HOLDOFF) && defined(RHO_GPIO))

#if SL_RAIL_UTIL_COEX_RETRYRX_TIMEOUT
  #define DEFAULT_PTA_OPT_RX_RETRY_TIMEOUT_MS SL_RAIL_UTIL_COEX_RETRYRX_TIMEOUT
#else //!SL_RAIL_UTIL_COEX_RETRYRX_TIMEOUT
  #define DEFAULT_PTA_OPT_RX_RETRY_TIMEOUT_MS PTA_OPT_DISABLED
#endif //SL_RAIL_UTIL_COEX_RETRYRX_TIMEOUT

#if SL_RAIL_UTIL_COEX_RETRYRX_ENABLE
  #define DEFAULT_PTA_OPT_RX_RETRY_REQ PTA_OPT_RX_RETRY_REQ
#else //!SL_RAIL_UTIL_COEX_RETRYRX_ENABLE
  #define DEFAULT_PTA_OPT_RX_RETRY_REQ PTA_OPT_DISABLED
#endif //SL_RAIL_UTIL_COEX_RETRYRX_TIMEOUT

#if SL_RAIL_UTIL_COEX_CCA_THRESHOLD
  #define DEFAULT_PTA_OPT_CCA_THRESHOLD \
  ((SL_RAIL_UTIL_COEX_CCA_THRESHOLD << 20) & PTA_OPT_CCA_THRESHOLD)
#else //!SL_RAIL_UTIL_COEX_CCA_THRESHOLD
  #define DEFAULT_PTA_OPT_CCA_THRESHOLD PTA_OPT_DISABLED
#endif  //SL_RAIL_UTIL_COEX_CCA_THRESHOLD

#if SL_RAIL_UTIL_COEX_MAC_FAIL_THRESHOLD
  #define DEFAULT_PTA_OPT_MAC_FAIL_THRESHOLD \
  ((SL_RAIL_UTIL_COEX_MAC_FAIL_THRESHOLD << 25) & PTA_OPT_MAC_FAIL_THRESHOLD)
#else //!SL_RAIL_UTIL_COEX_MAC_FAIL_THRESHOLD
  #define DEFAULT_PTA_OPT_MAC_FAIL_THRESHOLD PTA_OPT_DISABLED
#endif  //SL_RAIL_UTIL_COEX_MAC_FAIL_THRESHOLD

#define DEFAULT_PTA_OPT_FORCE_HOLDOFF PTA_OPT_DISABLED
#define DEFAULT_PTA_OPT_MAC_HOLDOFF   PTA_OPT_DISABLED

#define DEFAULT_PTA_OPTIONS (0U                                            \
                             | DEFAULT_PTA_OPT_RX_RETRY_TIMEOUT_MS         \
                             | DEFAULT_PTA_OPT_ACK_HOLDOFF                 \
                             | DEFAULT_PTA_OPT_ABORT_TX                    \
                             | DEFAULT_PTA_OPT_TX_HIPRI                    \
                             | DEFAULT_PTA_OPT_RX_HIPRI                    \
                             | DEFAULT_PTA_OPT_RX_RETRY_HIPRI              \
                             | DEFAULT_PTA_OPT_RX_RETRY_REQ                \
                             | DEFAULT_PTA_OPT_RHO_ENABLED                 \
                             | DEFAULT_PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT \
                             | DEFAULT_PTA_OPT_FORCE_HOLDOFF               \
                             | DEFAULT_PTA_OPT_MAC_HOLDOFF                 \
                             | DEFAULT_PTA_OPT_CCA_THRESHOLD               \
                             | DEFAULT_PTA_OPT_MAC_FAIL_THRESHOLD          \
                             | DEFAULT_PTA_OPT_LONG_REQ                    \
                             )

#if defined(SL_RAIL_UTIL_COEX_REQ_PORT) || defined(PTA_REQ_GPIO)
  #define PUBLIC_PTA_OPT_RX_RETRY_TIMEOUT_MS \
  PTA_OPT_RX_RETRY_TIMEOUT_MS
  #define PUBLIC_PTA_OPT_RX_RETRY_REQ \
  PTA_OPT_RX_RETRY_REQ
  #define PUBLIC_PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT \
  PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT
  #define PUBLIC_PTA_OPT_FORCE_HOLDOFF PTA_OPT_FORCE_HOLDOFF
  #define PUBLIC_PTA_OPT_MAC_HOLDOFF PTA_OPT_MAC_HOLDOFF
  #define PUBLIC_PTA_OPT_REQ_FILTER_PASS PTA_OPT_REQ_FILTER_PASS
#else //!defined(SL_RAIL_UTIL_COEX_REQ_PORT) && !defined(PTA_REQ_GPIO)
  #define PUBLIC_PTA_OPT_RX_RETRY_TIMEOUT_MS \
  PTA_OPT_DISABLED
  #define PUBLIC_PTA_OPT_RX_RETRY_REQ \
  PTA_OPT_DISABLED
  #define PUBLIC_PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT \
  PTA_OPT_DISABLED
  #define PUBLIC_PTA_OPT_FORCE_HOLDOFF PTA_OPT_DISABLED
  #define PUBLIC_PTA_OPT_MAC_HOLDOFF PTA_OPT_DISABLED
  #define PUBLIC_PTA_OPT_REQ_FILTER_PASS PTA_OPT_DISABLED
#endif

#if defined(SL_RAIL_UTIL_COEX_PRI_PORT) || defined(PTA_PRI_GPIO) || defined(SL_RAIL_UTIL_COEX_DP_PORT)
  #define PUBLIC_PTA_OPT_TX_HIPRI          PTA_OPT_TX_HIPRI
  #define PUBLIC_PTA_OPT_RX_HIPRI          PTA_OPT_RX_HIPRI
  #define PUBLIC_PTA_OPT_RX_RETRY_HIPRI    PTA_OPT_RX_RETRY_HIPRI
  #define PUBLIC_PTA_OPT_HIPRI_FILTER_PASS PTA_OPT_HIPRI_FILTER_PASS
#else //!defined(SL_RAIL_UTIL_COEX_PRI_PORT) && !defined(PTA_PRI_GPIO) && !defined(SL_RAIL_UTIL_COEX_DP_PORT)
  #define PUBLIC_PTA_OPT_TX_HIPRI          PTA_OPT_DISABLED
  #define PUBLIC_PTA_OPT_RX_HIPRI          PTA_OPT_DISABLED
  #define PUBLIC_PTA_OPT_RX_RETRY_HIPRI    PTA_OPT_DISABLED
  #define PUBLIC_PTA_OPT_HIPRI_FILTER_PASS PTA_OPT_DISABLED
#endif

#if defined(SL_RAIL_UTIL_COEX_RHO_PORT) || defined(RHO_GPIO)
  #define PUBLIC_PTA_OPT_RHO_ENABLED PTA_OPT_RHO_ENABLED
#else //!(defined(SL_RAIL_UTIL_COEX_RHO_PORT) || defined(RHO_GPIO))
  #define PUBLIC_PTA_OPT_RHO_ENABLED PTA_OPT_DISABLED
#endif //defined(SL_RAIL_UTIL_COEX_RHO_PORT) || defined(RHO_GPIO)

#define PUBLIC_PTA_OPT_ACK_HOLDOFF PTA_OPT_ACK_HOLDOFF
#define PUBLIC_PTA_OPT_ABORT_TX    PTA_OPT_ABORT_TX

#if SL_RAIL_UTIL_COEX_PRIORITY_ESCALATION_ENABLE
#define PUBLIC_PTA_OPT_CCA_THRESHOLD PTA_OPT_CCA_THRESHOLD
#define PUBLIC_PTA_OPT_MAC_RETRY_THRESHOLD PTA_OPT_MAC_RETRY_THRESHOLD
#define PUBLIC_PTA_OPT_MAC_FAIL_THRESHOLD PTA_OPT_MAC_FAIL_THRESHOLD
#else //!SL_RAIL_UTIL_COEX_PRIORITY_ESCALATION_ENABLE
#define PUBLIC_PTA_OPT_CCA_THRESHOLD PTA_OPT_DISABLED
#define PUBLIC_PTA_OPT_MAC_RETRY_THRESHOLD PTA_OPT_DISABLED
#define PUBLIC_PTA_OPT_MAC_FAIL_THRESHOLD PTA_OPT_DISABLED
#endif //SL_RAIL_UTIL_COEX_PRIORITY_ESCALATION_ENABLE

// Public PTA options can be modified using public PTA APIs
#define PUBLIC_PTA_OPTIONS (0U                                           \
                            | PUBLIC_PTA_OPT_RX_RETRY_TIMEOUT_MS         \
                            | PUBLIC_PTA_OPT_ACK_HOLDOFF                 \
                            | PUBLIC_PTA_OPT_ABORT_TX                    \
                            | PUBLIC_PTA_OPT_TX_HIPRI                    \
                            | PUBLIC_PTA_OPT_RX_HIPRI                    \
                            | PUBLIC_PTA_OPT_RX_RETRY_HIPRI              \
                            | PUBLIC_PTA_OPT_RX_RETRY_REQ                \
                            | PUBLIC_PTA_OPT_RHO_ENABLED                 \
                            | PUBLIC_PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT \
                            | PUBLIC_PTA_OPT_FORCE_HOLDOFF               \
                            | PUBLIC_PTA_OPT_MAC_HOLDOFF                 \
                            | PUBLIC_PTA_OPT_REQ_FILTER_PASS             \
                            | PUBLIC_PTA_OPT_HIPRI_FILTER_PASS           \
                            | PUBLIC_PTA_OPT_CCA_THRESHOLD               \
                            | PUBLIC_PTA_OPT_MAC_RETRY_THRESHOLD         \
                            | PUBLIC_PTA_OPT_MAC_FAIL_THRESHOLD          \
                            )
// Constant PTA options can not be modified using public PTA APIs
#define CONST_PTA_OPTIONS (~(PUBLIC_PTA_OPTIONS))

#ifndef DEFAULT_PTA_OPTIONS
  #define DEFAULT_PTA_OPTIONS PTA_OPT_DISABLED
#endif
#ifndef CONST_PTA_OPTIONS
  #define CONST_PTA_OPTIONS (~PTA_OPT_DISABLED)
#endif

#if !defined(SL_RAIL_UTIL_COEX_PHY_SELECT_PORT)         \
  && (SL_RAIL_UTIL_COEX_DEFAULT_PHY_SELECT_TIMEOUT > 0) \
  && (SL_RAIL_UTIL_COEX_DEFAULT_PHY_SELECT_TIMEOUT < 255)
#error "Select SL_RAIL_UTIL_COEX_PHY_SELECT GPIO before enabling COEX PHY select timeout!"
#endif

extern void emPhyCancelTransmit(void);
void halStackRadioHoldOffPowerDown(void); // fwd ref
void halStackRadioHoldOffPowerUp(void);   // fwd ref

extern void emRadioEnablePta(bool enabled);
extern void emRadioHoldOffIsr(bool active);

#define PHY_IS_RAIL_BASED (PHY_RAIL || PHY_DUALRAIL)
#define USE_MULTITIMER PHY_IS_RAIL_BASED

#if HAL_ANTDIV_RX_RUNTIME_PHY_SELECT
extern bool halAntDivRxPhyChanged(void);
#else //!HAL_ANTDIV_RX_RUNTIME_PHY_SELECT
#define halAntDivRxPhyChanged() (false)
#endif //HAL_ANTDIV_RX_RUNTIME_PHY_SELECT

#if HAL_FEM_RUNTIME_PHY_SELECT
extern bool halFemPhyChanged(void);
#else //!HAL_FEM_RUNTIME_PHY_SELECT
#define halFemPhyChanged() (false)
#endif //HAL_FEM_RUNTIME_PHY_SELECT

#define RUNTIME_PHY_SELECT                 \
  (HAL_ANTDIV_RX_RUNTIME_PHY_SELECT        \
   || SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT \
   || HAL_FEM_RUNTIME_PHY_SELECT)          \

#define COEX_STACK_EVENT_SUPPORT \
  (COEX_SUPPORT                  \
   || COEX_RHO_SUPPORT           \
   || RUNTIME_PHY_SELECT)        \

#if SL_RAIL_UTIL_COEX_PHY_ENABLED
  #ifdef _SILICON_LABS_32B_SERIES_1_CONFIG_1
  #error "COEX PHY is not supported on the selected platform."
  #endif
static uint8_t phySelectTimeoutMs = PTA_PHY_SELECT_TIMEOUT_MAX;
#else //!SL_RAIL_UTIL_COEX_PHY_ENABLED
static uint8_t phySelectTimeoutMs = 0U;
#endif //SL_RAIL_UTIL_COEX_PHY_ENABLED

#if COEX_STACK_EVENT_SUPPORT
static bool coexInitialized = false;
#endif //COEX_STACK_EVENT_SUPPORT

#if RUNTIME_PHY_SELECT

#include "rail_types.h"
// This is grody, but it beats making the PHY need to know about all
// the Coex PHYs.  Borrow from unreleased phy/phy.h:
enum {
  EMBER_RADIO_POWER_MODE_RX_ON,
  EMBER_RADIO_POWER_MODE_OFF
};
typedef uint8_t RadioPowerMode;
#if SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
extern bool halCoexPhySelectedCoex;
#define halCoexPhyChanged() (halCoexNewPhySelectedCoex != halCoexPhySelectedCoex)
#define halCoexUpdateSelectedPhy() (halCoexPhySelectedCoex = halCoexNewPhySelectedCoex)
#else //!SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
#define halCoexPhyChanged() (false)
#define halCoexUpdateSelectedPhy() /*no-op*/
#endif //SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT

extern RAIL_Handle_t emPhyRailHandle;
#if     PHY_DUAL
// Map to native PHY only
#define emPhyGetChannelPageInUse emPhy0_emPhyGetChannelPageInUse
#define emRadioGetIdleMode       emPhy0_emRadioGetIdleMode
#define emRadioSetIdleMode       emPhy0_emRadioSetIdleMode
#endif//PHY_DUAL
extern uint8_t emPhyGetChannelPageInUse(void);
extern RadioPowerMode sl_mac_get_radio_idle_mode(uint8_t mac_index);
extern sl_status_t sl_mac_lower_mac_set_radio_idle_mode(uint8_t mac_index, RadioPowerMode mode);
extern RAIL_Status_t halPluginConfig2p4GHzRadio(RAIL_Handle_t railHandle);

#if     SL_RAIL_UTIL_COEX_PHY_ENABLED
static volatile bool halCoexNewPhySelectedCoex = true;
#else//!SL_RAIL_UTIL_COEX_PHY_ENABLED
static volatile bool halCoexNewPhySelectedCoex = false;
#endif//SL_RAIL_UTIL_COEX_PHY_ENABLED
static uint8_t blockPhySwitch = 0U;
#define BLOCK_SWITCH_RX 0x01u
#define BLOCK_SWITCH_TX 0x02u
#define setBlockPhySwitch(dir, boolval)   \
  do {                                    \
    if (boolval) {                        \
      blockPhySwitch |= (dir);            \
    } else {                              \
      blockPhySwitch &= (uint8_t) ~(dir); \
    }                                     \
  } while (false)

#if SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
static bool phySelectInitialized = false;
static void phySelectTick(void);
#endif //SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT

static bool checkPhySwitch(void)
{
#if SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
  if (phySelectInitialized) {
    phySelectTick();
  } else if (emPhyRailHandle != NULL) {
    halPtaSetPhySelectTimeout(SL_RAIL_UTIL_COEX_DEFAULT_PHY_SELECT_TIMEOUT);
    phySelectInitialized = true;
  }
#endif //SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
  if ((halAntDivRxPhyChanged() || halCoexPhyChanged() || halFemPhyChanged())
      && (blockPhySwitch == 0U)
      && coexInitialized
      && (emPhyRailHandle != NULL)) {
    //@TODO: Ascertain radio is OFF, RXWARM, or RXSEARCH only.
    halCoexUpdateSelectedPhy();
    if (emPhyGetChannelPageInUse() == 0) { // Using 2.4GHz band
      RadioPowerMode currentMode = sl_mac_get_radio_idle_mode(0U);
      (void) sl_mac_lower_mac_set_radio_idle_mode(0U, EMBER_RADIO_POWER_MODE_OFF);
      (void) halPluginConfig2p4GHzRadio(emPhyRailHandle);
      (void) sl_mac_lower_mac_set_radio_idle_mode(0U, currentMode);
    } else {
      // Defer proper PHY selection to when the 2.4GHz band is next used
    }
    return true;
  }
  return false;
}

#else//!RUNTIME_PHY_SELECT

#define setBlockPhySwitch(dir, boolval) /*no-op*/
#define checkPhySwitch() (false)

#endif//RUNTIME_PHY_SELECT

#if COEX_STACK_EVENT_SUPPORT
static HalPtaOptions halPtaOptions = DEFAULT_PTA_OPTIONS;
static void configRandomDelay(void);
static void coexEventsCb(COEX_Events_t events);
#if SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
static void phySelectIsr(void);
#endif//SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT

static void eventsCb(COEX_Events_t events)
{
  if ((events & COEX_EVENT_HOLDOFF_CHANGED) != 0U) {
    emRadioHoldOffIsr((COEX_GetOptions() & COEX_OPTION_HOLDOFF_ACTIVE) != 0U);
  }
 #if     SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
  if ((events & COEX_EVENT_PHY_SELECT_CHANGED) != 0U) {
    phySelectIsr();
  }
 #endif//SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
  coexEventsCb(events);
}

static void COEX_802154_Init(void)
{
  if (coexInitialized) {
    return;
  }
  configRandomDelay();
  COEX_SetRadioCallback(&eventsCb);
  COEX_HAL_Init();
  coexInitialized = true;
}

#define MAP_COEX_OPTION(coexOpt, halPtaOpt) \
  (mapCoexOption(&coexOptions, coexOpt, options, halPtaOpt))

static void mapCoexOption(COEX_Options_t * coexOptions, COEX_Options_t coexOpt,
                          HalPtaOptions options, HalPtaOptions halPtaOpt)
{
  if ((options & halPtaOpt) != 0U) {
    *coexOptions |= coexOpt;
  } else {
    *coexOptions &= ~coexOpt;
  }
}

// Public API

EmberStatus halPtaSetBool(HalPtaOptions option, bool value)
{
  HalPtaOptions ptaOptions = halPtaGetOptions();
  ptaOptions = value ? (ptaOptions | option) : (ptaOptions & (~option));
  return halPtaSetOptions(ptaOptions);
}

HalPtaOptions halPtaGetOptions(void)
{
  return halPtaOptions;
}

EmberStatus halPtaSetOptions(HalPtaOptions options)
{
  EmberStatus status = EMBER_ERR_FATAL;
  COEX_802154_Init();
  COEX_Options_t coexOptions = COEX_GetOptions();

#ifndef DEBUG_PTA
  if ((options & CONST_PTA_OPTIONS) != (DEFAULT_PTA_OPTIONS & CONST_PTA_OPTIONS)) {
    //Return error if any options argument is unsupported or constant
    return EMBER_BAD_ARGUMENT;
  }
#endif //!DEBUG_PTA

  if (((~halPtaOptions) & options & PTA_OPT_FORCE_HOLDOFF) != 0U) {
    //Cancel all requests if force holdoff is enabled
    //All future requests will be blocked until force holdoff is disabled
    (void) halPtaSetTxRequest(PTA_REQ_OFF, NULL);
    (void) halPtaSetRxRequest(PTA_REQ_OFF, NULL);
  }
  MAP_COEX_OPTION(COEX_OPTION_TX_ABORT, PTA_OPT_ABORT_TX);
  MAP_COEX_OPTION(COEX_OPTION_RHO_ENABLED, PTA_OPT_RHO_ENABLED);

  if (COEX_SetOptions(coexOptions)) {
    halPtaOptions = options;
    status = EMBER_SUCCESS;
  }
  return status;
}
#else //!COEX_STACK_EVENT_SUPPORT

EmberStatus halPtaSetBool(HalPtaOptions option, bool value)
{
  UNUSED_VAR(option);
  UNUSED_VAR(value);
  return EMBER_ERR_FATAL;
}

HalPtaOptions halPtaGetOptions(void)
{
  return PTA_OPT_DISABLED;
}

EmberStatus halPtaSetOptions(HalPtaOptions options)
{
  UNUSED_VAR(options);
  return EMBER_ERR_FATAL;
}
#endif //COEX_STACK_EVENT_SUPPORT

#ifdef SL_RAIL_UTIL_COEX_DP_ENABLED
EmberStatus halPtaSetDirectionalPriorityPulseWidth(uint8_t pulseWidthUs)
{
  return COEX_HAL_SetDpPulseWidth(pulseWidthUs)
         ? EMBER_SUCCESS : EMBER_ERR_FATAL;
}

uint8_t halPtaGetDirectionalPriorityPulseWidth(void)
{
  return COEX_HAL_GetDpPulseWidth();
}
#else //SL_RAIL_UTIL_COEX_DP_ENABLED
EmberStatus halPtaSetDirectionalPriorityPulseWidth(uint8_t pulseWidthUs)
{
  UNUSED_VAR(pulseWidthUs);
  return EMBER_ERR_FATAL;
}

uint8_t halPtaGetDirectionalPriorityPulseWidth(void)
{
  return 0;
}
#endif //SL_RAIL_UTIL_COEX_DP_ENABLED

#if     COEX_SUPPORT
static void emCoexCounter(COEX_Events_t events);
static COEX_ReqState_t txReq;
static COEX_ReqState_t rxReq;

#if     PHY_IS_RAIL_BASED
#include "rail.h"
static bool cancelTransmit(void)
{
  extern RAIL_Handle_t emPhyRailHandle;
  return RAIL_StopTx(emPhyRailHandle, RAIL_STOP_MODE_ACTIVE) == RAIL_STATUS_NO_ERROR;
}
#else//!PHY_IS_RAIL_BASED
static bool cancelTransmit(void)
{
  emPhyCancelTransmit();
  return true;
}
#endif//PHY_IS_RAIL_BASED

static void coexEventsCb(COEX_Events_t events)
{
  if ((events & COEX_EVENT_COEX_CHANGED) != 0U) {
    emRadioEnablePta((COEX_GetOptions() & COEX_OPTION_COEX_ENABLED) != 0U);
  }
  if ((events & COEX_EVENT_GRANT_RELEASED) != 0U
      && (COEX_GetOptions() & COEX_OPTION_TX_ABORT) != 0U
      && (txReq.coexReq & COEX_REQ_ON) != 0U) {
    if (cancelTransmit()) {
      events |= COEX_EVENT_TX_ABORTED;
    }
  }
  emCoexCounter(events);
}

// Certain radios may want to override this with their own
WEAK(void emRadioEnablePta(bool enabled))
{
  UNUSED_VAR(enabled);
}

#define coexCounterType(type) ((events & COEX_EVENT_PRIORITY_ASSERTED) != 0U \
                               ? (EMBER_COUNTER_PTA_HI_PRI_ ## type)         \
                               : (EMBER_COUNTER_PTA_LO_PRI_ ## type))

#ifdef EMBER_STACK_IP
  #define coexCounter(type, data) \
  emberCounterHandler(coexCounterType(type), data)
#elif defined(EMBER_STACK_CONNECT)
  #define coexCounter(type, data) //no-op
#else // must be EMBER_STACK_ZIGBEE
extern void emCallCounterHandler(EmberCounterType type, uint8_t data);
  #define coexCounter(type, data) \
  emCallCounterHandler(coexCounterType(type), data)
#endif //EMBER_STACK_IP

static void emCoexCounter(COEX_Events_t events)
{
  if ((events & COEX_EVENT_REQUEST_RELEASED) != 0U) {
    coexCounter(REQUESTED, 1);
  }
  if ((events & COEX_EVENT_REQUEST_DENIED) != 0U) {
    coexCounter(DENIED, 1);
  } else if ((events & COEX_EVENT_TX_ABORTED) != 0U) {
    coexCounter(TX_ABORTED, 1);
  } else {
    // Request not denied
  }
}

// Certain radios may want to override this with their own
WEAK(void emPhyCancelTransmit(void))
{
}

#if SL_RAIL_UTIL_COEX_PRIORITY_ESCALATION_ENABLE || defined(DEBUG_PTA)

static uint8_t ptaCcaFailEscalate = 0U;
static uint8_t ptaMacFailEscalate = 0U;
static bool priorityEscalated = false;

static void escalatePriority(uint8_t * counter, uint8_t threshold)
{
  if (((halPtaGetOptions() & PTA_OPT_TX_HIPRI) == 0U)
      && (threshold > 0U)) {
    ++(*counter);
    if (threshold <= (*counter)) {
      priorityEscalated = true;
    }
  }
}

static void deescalatePriority(void)
{
  if (priorityEscalated) {
    ptaCcaFailEscalate = 0U;
    ptaMacFailEscalate = 0U;
    priorityEscalated = false;
  }
}

#else //!(SL_RAIL_UTIL_COEX_PRIORITY_ESCALATION_ENABLE || defined(DEBUG_PTA))

#define escalatePriority(counter, threshold)  /* no-op */
#define deescalatePriority()                  /* no-op */

#endif //SL_RAIL_UTIL_COEX_PRIORITY_ESCALATION_ENABLE || defined(DEBUG_PTA)

// PTA MAC/PHY Event Handling

#define PTA_RX_TIMEOUT_US 10000u
#define PTA_RX_RETRY_US (1000u * halPtaGetOptionMask(PTA_OPT_RX_RETRY_TIMEOUT_MS, \
                                                     PTA_OPT_SHIFT_RX_RETRY_TIMEOUT_MS))

#if     USE_MULTITIMER

static void setTimer(RAIL_MultiTimer_t * timer,
                     uint32_t time,
                     RAIL_MultiTimerCallback_t cb)
{
  if (!RAIL_IsMultiTimerRunning(timer)) {
    RAIL_SetMultiTimer(timer,
                       time,
                       RAIL_TIME_DELAY,
                       cb,
                       NULL);
  }
}

static void cancelTimer(RAIL_MultiTimer_t * timer)
{
  (void) RAIL_CancelMultiTimer(timer);
}

static RAIL_MultiTimer_t ptaRxTimer;
static RAIL_MultiTimer_t ptaRxRetryTimer;

#if SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
static RAIL_MultiTimer_t ptaPhySelectTimer;
static void ptaPhySelectTimerCb(RAIL_MultiTimer_t * tmr,
                                RAIL_Time_t expectedTimeOfEvent,
                                void *cbArg)
{
  UNUSED_VAR(tmr);
  UNUSED_VAR(expectedTimeOfEvent);
  UNUSED_VAR(cbArg);
  phySelectTick();
}

static void phySelectTick(void)
{
  if (phySelectTimeoutMs != 0U && !RAIL_IsMultiTimerRunning(&ptaPhySelectTimer)) {
    halCoexNewPhySelectedCoex = false;
    COEX_EnablePhySelectIsr(true);
  }
}

#define MICROSECONDS_PER_MILLISECOND (1000U)
static void phySelectIsr(void)
{
  halCoexNewPhySelectedCoex = true;
  if (phySelectInitialized) {
    setTimer(&ptaPhySelectTimer, phySelectTimeoutMs * MICROSECONDS_PER_MILLISECOND, &ptaPhySelectTimerCb);
  }
}
#endif //SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT

static void ptaRxTimerCb(RAIL_MultiTimer_t * tmr,
                         RAIL_Time_t expectedTimeOfEvent,
                         void *cbArg)
{
  UNUSED_VAR(tmr);
  UNUSED_VAR(expectedTimeOfEvent);
  UNUSED_VAR(cbArg);
  cancelTimer(&ptaRxTimer);
  cancelTimer(&ptaRxRetryTimer);
  (void) halPtaSetRxRequest(PTA_REQ_OFF, NULL);
}

// Both timers do the same thing, and cancel each other
#define ptaRxRetryTimerCb ptaRxTimerCb

#define checkTimers() /*no-op*/

#else//!USE_MULTITIMER

static void setTimer(int32_t * timer, uint32_t time, uint16_t * timeout)
{
  *timer = -1;
  *timeout = (uint16_t) (time / 1000u);   // Micro to milliseconds
  *timer = (int32_t) halCommonGetInt16uMillisecondTick();
}

static void cancelTimer(int32_t * timer)
{
  *timer = -1;
}

static int32_t ptaRxTimer = -1;
static uint16_t ptaRxTimerCb = 0;
static int32_t ptaRxRetryTimer = -1;
static uint16_t ptaRxRetryTimerCb = 0;

static bool isPtaTimerExpired(int32_t timerMs, uint16_t timeoutMs)
{
  return (timerMs >= 0)
         && (elapsedTimeInt16u((uint16_t)timerMs,
                               halCommonGetInt16uMillisecondTick()) > timeoutMs);
}

static void checkTimers(void)
{
  if (isPtaTimerExpired(ptaRxTimer, ptaRxTimerCb)
      || isPtaTimerExpired(ptaRxRetryTimer, ptaRxRetryTimerCb)) {
    // Both timers do the same thing, and cancel each other
    cancelTimer(&ptaRxTimer);
    cancelTimer(&ptaRxRetryTimer);
    (void) halPtaSetRxRequest(PTA_REQ_OFF, NULL);
  }
}

#endif//USE_MULTITIMER

#if USE_MULTITIMER && SL_RAIL_UTIL_COEX_PWM_DEFAULT_ENABLED
static bool pwmRequestInitialized = false;
static void initPwmRequest(void)
{
  extern RAIL_Handle_t emPhyRailHandle;
  if (!pwmRequestInitialized && emPhyRailHandle != NULL) {
    halPtaSetRequestPwm(COEX_REQ_PWM
                        | (SL_RAIL_UTIL_COEX_PWM_PRIORITY << COEX_REQ_HIPRI_SHIFT),
                        NULL,
                        SL_RAIL_UTIL_COEX_PWM_REQ_DUTYCYCLE,
                        SL_RAIL_UTIL_COEX_PWM_REQ_PERIOD);
    pwmRequestInitialized = true;
  }
}
#else //!(USE_MULTITIMER && SL_RAIL_UTIL_COEX_PWM_DEFAULT_ENABLED)
#define initPwmRequest() //no-op
#endif // USE_MULTITIMER && SL_RAIL_UTIL_COEX_PWM_DEFAULT_ENABLED

halPtaStackStatus_t halPtaStackEvent(halPtaStackEvent_t ptaStackEvent,
                                     uint32_t supplement)
{
  halPtaStackStatus_t status = PTA_STACK_STATUS_SUCCESS;
  bool isReceivingFrame = false;

  switch (ptaStackEvent) {
    case PTA_STACK_EVENT_TICK:
      if (halPtaIsEnabled()) {
        initPwmRequest();
        checkTimers();
      }
      (void) checkPhySwitch();
      break;

    // RX events:
    case PTA_STACK_EVENT_RX_STARTED:
      if (halPtaIsEnabled()) {
        isReceivingFrame = (bool) supplement;
        if (isReceivingFrame) {
          (void) halPtaSetRxRequest(halPtaFrameDetectReq(), NULL);
          cancelTimer(&ptaRxRetryTimer);
          setTimer(&ptaRxTimer, PTA_RX_TIMEOUT_US, &ptaRxTimerCb);
        }
      }
      setBlockPhySwitch(BLOCK_SWITCH_RX, true);
      break;

    case PTA_STACK_EVENT_RX_ACCEPTED:
      if (halPtaIsEnabled()) {
        isReceivingFrame = (bool) supplement;
        if (isReceivingFrame) {
          halPtaReq_t filterPass = halPtaFilterPassReq();
          if (filterPass != halPtaFrameDetectReq()) {
            (void) halPtaSetRxRequest(filterPass, NULL);
          }
        }
      }
      setBlockPhySwitch(BLOCK_SWITCH_RX, true);
      break;

    case PTA_STACK_EVENT_RX_ACKING:
      // Defer Rx PTA cancellation to RX_ACK_* events
      setBlockPhySwitch(BLOCK_SWITCH_RX, true);
      break;

    case PTA_STACK_EVENT_RX_CORRUPTED:
    case PTA_STACK_EVENT_RX_ACK_BLOCKED:
    case PTA_STACK_EVENT_RX_ACK_ABORTED:
      if (halPtaIsEnabled()) {
        if ((halPtaGetOptions() & PTA_OPT_RX_RETRY_REQ) != 0U) {
          cancelTimer(&ptaRxTimer);
          // Assert request and start rx retry timer.
          (void) halPtaSetRxRequest(PTA_REQ_ON | PTA_RX_RETRY_PRI, NULL);
          setTimer(&ptaRxRetryTimer, PTA_RX_RETRY_US, &ptaRxRetryTimerCb);
          break;
        }
      }
    // FALLTHROUGH

    case PTA_STACK_EVENT_RX_FILTERED:
    case PTA_STACK_EVENT_RX_ENDED:
    case PTA_STACK_EVENT_RX_ACK_SENT:
      isReceivingFrame = (bool) supplement;
    // FALLTHROUGH

    case PTA_STACK_EVENT_RX_IDLED:
      if (halPtaIsEnabled()) {
        cancelTimer(&ptaRxTimer);
        cancelTimer(&ptaRxRetryTimer);
        if (!isReceivingFrame) {
          (void) halPtaSetRxRequest(PTA_REQ_OFF, NULL);
        }
      }
      setBlockPhySwitch(BLOCK_SWITCH_RX, isReceivingFrame);
      break;

    // TX events:
    case PTA_STACK_EVENT_TX_PENDED_MAC:
      if (halPtaIsEnabled()) {
        halPtaCb_t cb = (halPtaCb_t) supplement;
        HalPtaOptions ptaOptions = halPtaGetOptions();
        if ((ptaOptions & PTA_OPT_MAC_AND_FORCE_HOLDOFFS) == PTA_OPT_MAC_AND_FORCE_HOLDOFFS) {
          // Tell caller NOT to transmit
          status = PTA_STACK_STATUS_HOLDOFF;
        } else if (((ptaOptions & PTA_OPT_MAC_AND_FORCE_HOLDOFFS) == PTA_OPT_MAC_HOLDOFF)
                   && (cb != NULL)) {
          // Assert Tx REQUEST  and wait for GRANT
          halPtaSetTxRequest(PTA_REQ_ON | PTA_TX_PRI | PTA_REQCB_GRANTED, cb);
          status = PTA_STACK_STATUS_CB_PENDING;
        } else {
          // No callback - proceed with a normal transmit
        }
      }
      break;

    case PTA_STACK_EVENT_TX_PENDED_PHY:
      if (halPtaIsEnabled()) {
        bool isCcaTransmit = (bool) supplement;
        if (isCcaTransmit && ((halPtaGetOptions() & PTA_OPT_LONG_REQ) == 0U)) {
          // Defer Tx req to CCA_SOON event
        } else {
          halPtaSetTxRequest(PTA_REQ_ON | PTA_TX_PRI, NULL);
        }
      }
      setBlockPhySwitch(BLOCK_SWITCH_TX, true);
      break;

    case PTA_STACK_EVENT_TX_CCA_SOON:
      if (halPtaIsEnabled()) {
        (void) halPtaSetTxRequest(PTA_REQ_ON | PTA_TX_PRI, NULL);
      }
      break;

    case PTA_STACK_EVENT_TX_CCA_BUSY:
      if (halPtaIsEnabled()) {
        bool isNextCcaImminent = (bool) supplement;
        if (!isNextCcaImminent) {
          (void) halPtaSetTxRequest(PTA_REQ_OFF, NULL);
        }
      }
      break;

    case PTA_STACK_EVENT_TX_STARTED:
    case PTA_STACK_EVENT_TX_ACK_WAITING:
      // Currently these events are advisory and leave TX REQ asserted
      break;

    case PTA_STACK_EVENT_TX_ACK_RECEIVED:
      if (halPtaIsEnabled()) {
        bool hasFramePending = (bool) supplement;
        (void) hasFramePending;
        (void) halPtaSetTxRequest(PTA_REQ_OFF, NULL);
        deescalatePriority();
      }
      setBlockPhySwitch(BLOCK_SWITCH_TX, false);
      break;

    case PTA_STACK_EVENT_TX_ACK_TIMEDOUT:
      if (halPtaIsEnabled()) {
        uint8_t macRetries = (uint8_t) supplement;
        if ((macRetries == 0) || halPtaGetTxReqRelease()) {
          (void) halPtaSetTxRequest(PTA_REQ_OFF, NULL);
          if (macRetries == 0) {
            escalatePriority(&ptaMacFailEscalate,
                             halPtaGetMacFailCounterThreshold());
          }
        } else {
          // Hold Tx REQ for a MAC retransmit
        }
      }
      setBlockPhySwitch(BLOCK_SWITCH_TX, false);
      break;

    case PTA_STACK_EVENT_TX_BLOCKED:
    case PTA_STACK_EVENT_TX_ABORTED:
      if (halPtaIsEnabled()) {
        (void) halPtaSetTxRequest(PTA_REQ_OFF, NULL);
        bool pktRequestedAck = (bool) supplement;
        if (pktRequestedAck) {
          if (ptaStackEvent == PTA_STACK_EVENT_TX_BLOCKED) {
            escalatePriority(&ptaCcaFailEscalate,
                             halPtaGetCcaCounterThreshold());
          }
          escalatePriority(&ptaMacFailEscalate,
                           halPtaGetMacFailCounterThreshold());
        }
      }
      setBlockPhySwitch(BLOCK_SWITCH_TX, false);
      break;

    case PTA_STACK_EVENT_TX_ENDED:
    case PTA_STACK_EVENT_TX_IDLED:
      if (halPtaIsEnabled()) {
        (void) halPtaSetTxRequest(PTA_REQ_OFF, NULL);
      }
      setBlockPhySwitch(BLOCK_SWITCH_TX, false);
      break;

    default:
      break;
  }
  #if EMBER_AF_PLUGIN_COULOMB_COUNTER
  // Set the state w.r.t Coulomb Counter
  halCoulombCounterEvent(ptaStackEvent);
  #endif
  return status;
}

static void randomDelayCallback(uint16_t randomDelayMaskUs)
{
  halCommonDelayMicroseconds(halCommonGetRandom() & randomDelayMaskUs);
}
static void configRandomDelay(void)
{
  COEX_SetRandomDelayCallback(&randomDelayCallback);
}

static EmberStatus halInternalPtaSetRequest(COEX_ReqState_t * coexReqState,
                                            COEX_Req_t coexReq,
                                            COEX_ReqCb_t coexCb)
{
  // if force holdoff is enabled, don't set request
  return (((halPtaOptions & PTA_OPT_FORCE_HOLDOFF) == 0U)
          && COEX_SetRequest(coexReqState, coexReq, coexCb))
         ? EMBER_SUCCESS : EMBER_ERR_FATAL;
}

halPtaReq_t halPtaFrameDetectReq(void)
{
  HalPtaOptions options = halPtaGetOptions();
  halPtaReq_t syncDet = PTA_REQ_OFF;
  if (halPtaIsEnabled() && (options & PTA_OPT_REQ_FILTER_PASS) == 0) {
    syncDet |= PTA_REQ_ON;
    if ((options & PTA_OPT_RX_HIPRI) != 0U) {
      syncDet |= PTA_REQ_HIPRI;
    }
  }
  return syncDet;
}

halPtaReq_t halPtaFilterPassReq(void)
{
  HalPtaOptions options = halPtaGetOptions();
  halPtaReq_t filterPass = PTA_REQ_OFF;
  if (halPtaIsEnabled()) {
    filterPass |= PTA_REQ_ON;
    if (options & (PTA_OPT_RX_HIPRI | PTA_OPT_HIPRI_FILTER_PASS)) {
      filterPass |= PTA_REQ_HIPRI;
    }
  }
  return filterPass;
}

EmberStatus halPtaSetTxRequest(COEX_Req_t coexReq, COEX_ReqCb_t coexCb)
{
#if SL_RAIL_UTIL_COEX_PRIORITY_ESCALATION_ENABLE || defined(DEBUG_PTA)
  if (((coexReq & COEX_REQ_ON) != 0U) && priorityEscalated) {
    coexReq |= COEX_REQ_HIPRI;
  }
#endif // SL_RAIL_UTIL_COEX_PRIORITY_ESCALATION_ENABLE || defined(DEBUG_PTA)
  return halInternalPtaSetRequest(&txReq, coexReq, coexCb);
}

EmberStatus halPtaSetRxRequest(COEX_Req_t coexReq, COEX_ReqCb_t coexCb)
{
  return halInternalPtaSetRequest(&rxReq, coexReq, coexCb);
}

EmberStatus halPtaSetEnable(bool enabled)
{
  COEX_Options_t coexOptions = COEX_GetOptions();

  if (enabled) {
    coexOptions |= COEX_OPTION_COEX_ENABLED;
  } else {
    coexOptions &= ~COEX_OPTION_COEX_ENABLED;
  }
  return COEX_SetOptions(coexOptions)
         ? EMBER_SUCCESS : EMBER_ERR_FATAL;
}

bool halPtaIsEnabled(void)
{
  return (COEX_GetOptions() & COEX_OPTION_COEX_ENABLED) != 0U;
}

#else // !COEX_REQ_SUPPORT
#if COEX_STACK_EVENT_SUPPORT
static void configRandomDelay(void)
{
}
static void coexEventsCb(COEX_Events_t events)
{
  UNUSED_VAR(events);
}
#endif //!COEX_RHO_SUPPORT

halPtaReq_t halPtaFrameDetectReq(void)
{
  return PTA_REQ_OFF;
}

halPtaReq_t halPtaFilterPassReq(void)
{
  return PTA_REQ_OFF;
}

EmberStatus halPtaSetTxRequest(COEX_Req_t coexReq, COEX_ReqCb_t coexCb)
{
  UNUSED_VAR(coexReq);
  UNUSED_VAR(coexCb);
  return EMBER_ERR_FATAL;
}

EmberStatus halPtaSetRxRequest(COEX_Req_t coexReq, COEX_ReqCb_t coexCb)
{
  UNUSED_VAR(coexReq);
  UNUSED_VAR(coexCb);
  return EMBER_ERR_FATAL;
}

EmberStatus halPtaSetEnable(bool enabled)
{
  UNUSED_VAR(enabled);
  return EMBER_ERR_FATAL;
}

bool halPtaIsEnabled(void)
{
  return false;
}

halPtaStackStatus_t halPtaStackEvent(halPtaStackEvent_t ptaStackEvent,
                                     uint32_t supplement)
{
  UNUSED_VAR(supplement);
  UNUSED_VAR(ptaStackEvent);
 #if RUNTIME_PHY_SELECT
  bool isReceivingFrame = false;
  switch (ptaStackEvent) {
    case PTA_STACK_EVENT_TICK:
      (void) checkPhySwitch();
      break;

    // RX events:
    case PTA_STACK_EVENT_RX_STARTED:
    case PTA_STACK_EVENT_RX_ACCEPTED:
    case PTA_STACK_EVENT_RX_ACKING:
      setBlockPhySwitch(BLOCK_SWITCH_RX, true);
      break;
    case PTA_STACK_EVENT_RX_CORRUPTED:
    case PTA_STACK_EVENT_RX_ACK_BLOCKED:
    case PTA_STACK_EVENT_RX_ACK_ABORTED:
    case PTA_STACK_EVENT_RX_FILTERED:
    case PTA_STACK_EVENT_RX_ENDED:
    case PTA_STACK_EVENT_RX_ACK_SENT:
      isReceivingFrame = (bool) supplement;
    // FALLTHROUGH
    case PTA_STACK_EVENT_RX_IDLED:
      setBlockPhySwitch(BLOCK_SWITCH_RX, isReceivingFrame);
      break;

    // TX events:
    case PTA_STACK_EVENT_TX_PENDED_MAC:
      break;
    case PTA_STACK_EVENT_TX_PENDED_PHY:
      setBlockPhySwitch(BLOCK_SWITCH_TX, true);
      break;
    case PTA_STACK_EVENT_TX_CCA_SOON:
    case PTA_STACK_EVENT_TX_CCA_BUSY:
    case PTA_STACK_EVENT_TX_STARTED:
    case PTA_STACK_EVENT_TX_ACK_WAITING:
      break;
    case PTA_STACK_EVENT_TX_ACK_RECEIVED:
    case PTA_STACK_EVENT_TX_ACK_TIMEDOUT:
    case PTA_STACK_EVENT_TX_BLOCKED:
    case PTA_STACK_EVENT_TX_ABORTED:
    case PTA_STACK_EVENT_TX_ENDED:
    case PTA_STACK_EVENT_TX_IDLED:
      setBlockPhySwitch(BLOCK_SWITCH_TX, false);
      break;
    default:
      break;
  }
 #endif//RUNTIME_PHY_SELECT

 #ifdef EMBER_AF_PLUGIN_COULOMB_COUNTER
  // Set the state w.r.t Coulomb Counter
  halCoulombCounterEvent(ptaStackEvent);
 #endif//EMBER_AF_PLUGIN_COULOMB_COUNTER

  return PTA_STACK_STATUS_SUCCESS;
}
#endif//COEX_SUPPORT

#if     USE_MULTITIMER && COEX_SUPPORT
const HalPtaPwmArgs_t *halPtaGetRequestPwmArgs(void)
{
  return (const HalPtaPwmArgs_t *)COEX_GetPwmRequest();
}

EmberStatus halPtaSetRequestPwm(halPtaReq_t ptaReq,
                                halPtaCb_t ptaCb,
                                uint8_t dutyCycle,
                                uint8_t periodHalfMs)
{
  return COEX_SetPwmRequest(ptaReq,
                            ptaCb,
                            dutyCycle,
                            periodHalfMs)
         ? EMBER_SUCCESS : EMBER_BAD_ARGUMENT;
}

#else//!(USE_MULTITIMER && COEX_SUPPORT)

EmberStatus halPtaSetRequestPwm(halPtaReq_t ptaReq,
                                halPtaCb_t ptaCb,
                                uint8_t dutyCycle,
                                uint8_t periodHalfMs)
{
  UNUSED_VAR(ptaReq);
  UNUSED_VAR(ptaCb);
  UNUSED_VAR(dutyCycle);
  UNUSED_VAR(periodHalfMs);
  return EMBER_ERR_FATAL;
}

const HalPtaPwmArgs_t *halPtaGetRequestPwmArgs(void)
{
  return NULL;
}

#endif//(USE_MULTITIMER && COEX_SUPPORT)

#ifdef COEX_HAL_SMALL_RHO
// RHO implementation is defined in coexistence-hal.c
#elif COEX_RHO_SUPPORT

bool halGetRadioHoldOff(void)
{
  return (COEX_GetOptions() & COEX_OPTION_RHO_ENABLED) != 0U;
}

EmberStatus halSetRadioHoldOff(bool enabled)
{
  return halPtaSetBool(PTA_OPT_RHO_ENABLED, enabled);
}

void halStackRadioHoldOffPowerDown(void)
{
  setCoexPowerState(false);
}

void halStackRadioHoldOffPowerUp(void)
{
  setCoexPowerState(true);
}

#else//!COEX_RHO_SUPPORT

// Stub RHO implementation

bool halGetRadioHoldOff(void)
{
  return false;
}

EmberStatus halSetRadioHoldOff(bool enabled)
{
  UNUSED_VAR(enabled);
  return EMBER_ERR_FATAL;
}

void halStackRadioHoldOffPowerDown(void)
{
}

void halStackRadioHoldOffPowerUp(void)
{
}

#endif//COEX_RHO_SUPPORT

EmberStatus halPtaSetPhySelectTimeout(uint8_t timeoutMs)
{
#if SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
  if (phySelectTimeoutMs != timeoutMs) {
    phySelectTimeoutMs = timeoutMs;
    if (timeoutMs == 0U) {
      RAIL_CancelMultiTimer(&ptaPhySelectTimer);
      halCoexNewPhySelectedCoex = false;
    } else if (timeoutMs == PTA_PHY_SELECT_TIMEOUT_MAX) {
      RAIL_CancelMultiTimer(&ptaPhySelectTimer);
      halCoexNewPhySelectedCoex = true;
    } else {
#ifdef SL_RAIL_UTIL_COEX_PHY_SELECT_PORT
      COEX_EnablePhySelectIsr(true);
      return EMBER_SUCCESS;
#else //!SL_RAIL_UTIL_COEX_PHY_SELECT_PORT
      return EMBER_BAD_ARGUMENT;
#endif //SL_RAIL_UTIL_COEX_PHY_SELECT_PORT
    }
#ifdef SL_RAIL_UTIL_COEX_PHY_SELECT_PORT
    COEX_EnablePhySelectIsr(false);
#endif //SL_RAIL_UTIL_COEX_PHY_SELECT_PORT
  }
  return EMBER_SUCCESS;
#else //SL_RAIL_UTIL_COEX_PHY_ENABLED
  return (timeoutMs == phySelectTimeoutMs) ? EMBER_SUCCESS : EMBER_BAD_ARGUMENT;
#endif //SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
}

uint8_t halPtaGetPhySelectTimeout(void)
{
  return phySelectTimeoutMs;
}

#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
bool halPtaGetGpioInputOverride(halPtaGpioIndex_t gpioIndex)
{
  return COEX_GetGpioInputOverride((COEX_GpioIndex_t)(gpioIndex + 1));
}

EmberStatus halPtaSetGpioInputOverride(halPtaGpioIndex_t gpioIndex,
                                       bool enabled)
{
  return COEX_SetGpioInputOverride((COEX_GpioIndex_t)(gpioIndex + 1), enabled)
         ? EMBER_SUCCESS : EMBER_BAD_ARGUMENT;
}
#else //!SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
bool halPtaGetGpioInputOverride(halPtaGpioIndex_t gpioIndex)
{
  UNUSED_VAR(gpioIndex);
  return false;
}

EmberStatus halPtaSetGpioInputOverride(halPtaGpioIndex_t gpioIndex,
                                       bool enabled)
{
  UNUSED_VAR(gpioIndex);
  UNUSED_VAR(enabled);
  return EMBER_ERR_FATAL;
}
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
