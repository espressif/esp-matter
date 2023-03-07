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
#include "rail.h"
#include "sl_status.h"
#include "coexistence/protocol/ieee802154_uc/coexistence-802154.h"
#include "rail_ieee802154.h"

#ifdef RTOS
  #include "rtos/rtos.h"
#endif

#ifndef UNUSED_VAR
#define UNUSED_VAR(x) (void)x;
#endif

#if defined(DEBUG_PTA) || defined(RHO_GPIO) || defined(SL_RAIL_UTIL_COEX_RHO_PORT)
#define COEX_RHO_SUPPORT 1
#endif //defined(DEBUG_PTA) || defined(RHO_GPIO) || defined(SL_RAIL_UTIL_COEX_RHO_PORT)

#if defined(DEBUG_PTA) || defined(SL_RAIL_UTIL_COEX_REQ_GPIO) || defined(SL_RAIL_UTIL_COEX_REQ_PORT) \
  || defined(SL_RAIL_UTIL_COEX_GNT_GPIO) || defined(SL_RAIL_UTIL_COEX_GNT_PORT) || defined(SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT)
#define COEX_SUPPORT 1
#endif //defined(DEBUG_PTA) || defined(SL_RAIL_UTIL_COEX_REQ_GPIO) || defined(SL_RAIL_UTIL_COEX_REQ_PORT)
//|| defined(SL_RAIL_UTIL_COEX_GNT_GPIO) || defined(SL_RAIL_UTIL_COEX_GNT_PORT) || defined(SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT)

#if SL_RAIL_UTIL_COEX_ACKHOLDOFF || defined(RHO_GPIO)
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_ACK_HOLDOFF SL_RAIL_UTIL_COEX_OPT_ACK_HOLDOFF
#else //!(SL_RAIL_UTIL_COEX_ACKHOLDOFF || defined(RHO_GPIO))
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_ACK_HOLDOFF SL_RAIL_UTIL_COEX_OPT_DISABLED
#endif //SL_RAIL_UTIL_COEX_ACKHOLDOFF || defined(RHO_GPIO)

#if !defined(SL_RAIL_UTIL_COEX_IEEE802154_TX_ABORT) && defined(SL_RAIL_UTIL_COEX_TX_ABORT)
  #define SL_RAIL_UTIL_COEX_IEEE802154_TX_ABORT SL_RAIL_UTIL_COEX_TX_ABORT
#endif

#if SL_RAIL_UTIL_COEX_IEEE802154_TX_ABORT
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_ABORT_TX SL_RAIL_UTIL_COEX_OPT_ABORT_TX
#else //!SL_RAIL_UTIL_COEX_IEEE802154_TX_ABORT
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_ABORT_TX SL_RAIL_UTIL_COEX_OPT_DISABLED
#endif //SL_RAIL_UTIL_COEX_IEEE802154_TX_ABORT

#if SL_RAIL_UTIL_COEX_TX_HIPRI
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_TX_HIPRI SL_RAIL_UTIL_COEX_OPT_TX_HIPRI
#else //!SL_RAIL_UTIL_COEX_TX_HIPRI
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_TX_HIPRI SL_RAIL_UTIL_COEX_OPT_DISABLED
#endif //SL_RAIL_UTIL_COEX_TX_HIPRI

#if SL_RAIL_UTIL_COEX_RX_HIPRI
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_RX_HIPRI SL_RAIL_UTIL_COEX_OPT_RX_HIPRI
#else //!SL_RAIL_UTIL_COEX_RX_HIPRI
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_RX_HIPRI SL_RAIL_UTIL_COEX_OPT_DISABLED
#endif //SL_RAIL_UTIL_COEX_RX_HIPRI

#if SL_RAIL_UTIL_COEX_RETRYRX_HIPRI
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_RX_RETRY_HIPRI SL_RAIL_UTIL_COEX_OPT_RX_RETRY_HIPRI
#else //!SL_RAIL_UTIL_COEX_RETRYRX_HIPRI
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_RX_RETRY_HIPRI SL_RAIL_UTIL_COEX_OPT_DISABLED
#endif //SL_RAIL_UTIL_COEX_RETRYRX_HIPRI

#if SL_RAIL_UTIL_COEX_LONG_REQ
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_LONG_REQ SL_RAIL_UTIL_COEX_OPT_LONG_REQ
#else //!SL_RAIL_UTIL_COEX_LONG_REQ
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_LONG_REQ SL_RAIL_UTIL_COEX_OPT_DISABLED
#endif //SL_RAIL_UTIL_COEX_LONG_REQ

#if SL_RAIL_UTIL_COEX_TOGGLE_REQ_ON_MACRETRANSMIT
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_TOGGLE_REQ_ON_MACRETRANSMIT SL_RAIL_UTIL_COEX_OPT_TOGGLE_REQ_ON_MACRETRANSMIT
#else //!SL_RAIL_UTIL_COEX_TOGGLE_REQ_ON_MACRETRANSMIT
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_TOGGLE_REQ_ON_MACRETRANSMIT SL_RAIL_UTIL_COEX_OPT_DISABLED
#endif //SL_RAIL_UTIL_COEX_TOGGLE_REQ_ON_MACRETRANSMIT

#if defined(SL_RAIL_UTIL_COEX_RHO_PORT) || (defined(RADIO_HOLDOFF) && defined(RHO_GPIO))
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_RHO_ENABLED SL_RAIL_UTIL_COEX_OPT_RHO_ENABLED
#else //(!defined(SL_RAIL_UTIL_COEX_RHO_PORT) || (defined(RADIO_HOLDOFF) && defined(RHO_GPIO)))
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_RHO_ENABLED SL_RAIL_UTIL_COEX_OPT_DISABLED
#endif //defined(SL_RAIL_UTIL_COEX_RHO_PORT) || (defined(RADIO_HOLDOFF) && defined(RHO_GPIO))

#if SL_RAIL_UTIL_COEX_RETRYRX_TIMEOUT
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_RX_RETRY_TIMEOUT_MS SL_RAIL_UTIL_COEX_RETRYRX_TIMEOUT
#else //!SL_RAIL_UTIL_COEX_RETRYRX_TIMEOUT
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_RX_RETRY_TIMEOUT_MS SL_RAIL_UTIL_COEX_OPT_DISABLED
#endif //SL_RAIL_UTIL_COEX_RETRYRX_TIMEOUT

#if SL_RAIL_UTIL_COEX_RETRYRX_ENABLE
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_RX_RETRY_REQ SL_RAIL_UTIL_COEX_OPT_RX_RETRY_REQ
#else //!SL_RAIL_UTIL_COEX_RETRYRX_ENABLE
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_RX_RETRY_REQ SL_RAIL_UTIL_COEX_OPT_DISABLED
#endif //SL_RAIL_UTIL_COEX_RETRYRX_TIMEOUT

#if SL_RAIL_UTIL_COEX_PRIORITY_ESCALATION_ENABLE && SL_RAIL_UTIL_COEX_CCA_THRESHOLD
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_CCA_THRESHOLD \
  ((SL_RAIL_UTIL_COEX_CCA_THRESHOLD << 20) & SL_RAIL_UTIL_COEX_OPT_CCA_THRESHOLD)
#else //!SL_RAIL_UTIL_COEX_CCA_THRESHOLD
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_CCA_THRESHOLD SL_RAIL_UTIL_COEX_OPT_DISABLED
#endif  //SL_RAIL_UTIL_COEX_CCA_THRESHOLD

#if SL_RAIL_UTIL_COEX_PRIORITY_ESCALATION_ENABLE && SL_RAIL_UTIL_COEX_MAC_FAIL_THRESHOLD
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_MAC_FAIL_THRESHOLD \
  ((SL_RAIL_UTIL_COEX_MAC_FAIL_THRESHOLD << 25) & SL_RAIL_UTIL_COEX_OPT_MAC_FAIL_THRESHOLD)
#else //!SL_RAIL_UTIL_COEX_MAC_FAIL_THRESHOLD
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPT_MAC_FAIL_THRESHOLD SL_RAIL_UTIL_COEX_OPT_DISABLED
#endif  //SL_RAIL_UTIL_COEX_MAC_FAIL_THRESHOLD

#define DEFAULT_SL_RAIL_UTIL_COEX_OPT_FORCE_HOLDOFF SL_RAIL_UTIL_COEX_OPT_DISABLED
#define DEFAULT_SL_RAIL_UTIL_COEX_OPT_MAC_HOLDOFF   SL_RAIL_UTIL_COEX_OPT_DISABLED

#define DEFAULT_SL_RAIL_UTIL_COEX_OPTIONS (0U                                                          \
                                           | DEFAULT_SL_RAIL_UTIL_COEX_OPT_RX_RETRY_TIMEOUT_MS         \
                                           | DEFAULT_SL_RAIL_UTIL_COEX_OPT_ACK_HOLDOFF                 \
                                           | DEFAULT_SL_RAIL_UTIL_COEX_OPT_ABORT_TX                    \
                                           | DEFAULT_SL_RAIL_UTIL_COEX_OPT_TX_HIPRI                    \
                                           | DEFAULT_SL_RAIL_UTIL_COEX_OPT_RX_HIPRI                    \
                                           | DEFAULT_SL_RAIL_UTIL_COEX_OPT_RX_RETRY_HIPRI              \
                                           | DEFAULT_SL_RAIL_UTIL_COEX_OPT_RX_RETRY_REQ                \
                                           | DEFAULT_SL_RAIL_UTIL_COEX_OPT_RHO_ENABLED                 \
                                           | DEFAULT_SL_RAIL_UTIL_COEX_OPT_TOGGLE_REQ_ON_MACRETRANSMIT \
                                           | DEFAULT_SL_RAIL_UTIL_COEX_OPT_FORCE_HOLDOFF               \
                                           | DEFAULT_SL_RAIL_UTIL_COEX_OPT_MAC_HOLDOFF                 \
                                           | DEFAULT_SL_RAIL_UTIL_COEX_OPT_CCA_THRESHOLD               \
                                           | DEFAULT_SL_RAIL_UTIL_COEX_OPT_MAC_FAIL_THRESHOLD          \
                                           | DEFAULT_SL_RAIL_UTIL_COEX_OPT_LONG_REQ                    \
                                           )

#if defined(SL_RAIL_UTIL_COEX_REQ_PORT) || defined(SL_RAIL_UTIL_COEX_REQ_GPIO)
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_RX_RETRY_TIMEOUT_MS \
  SL_RAIL_UTIL_COEX_OPT_RX_RETRY_TIMEOUT_MS
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_RX_RETRY_REQ \
  SL_RAIL_UTIL_COEX_OPT_RX_RETRY_REQ
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_TOGGLE_REQ_ON_MACRETRANSMIT \
  SL_RAIL_UTIL_COEX_OPT_TOGGLE_REQ_ON_MACRETRANSMIT
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_FORCE_HOLDOFF SL_RAIL_UTIL_COEX_OPT_FORCE_HOLDOFF
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_MAC_HOLDOFF SL_RAIL_UTIL_COEX_OPT_MAC_HOLDOFF
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_REQ_FILTER_PASS SL_RAIL_UTIL_COEX_OPT_REQ_FILTER_PASS
#else //!defined(SL_RAIL_UTIL_COEX_REQ_PORT) && !defined(SL_RAIL_UTIL_COEX_REQ_GPIO)
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_RX_RETRY_TIMEOUT_MS \
  SL_RAIL_UTIL_COEX_OPT_DISABLED
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_RX_RETRY_REQ \
  SL_RAIL_UTIL_COEX_OPT_DISABLED
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_TOGGLE_REQ_ON_MACRETRANSMIT \
  SL_RAIL_UTIL_COEX_OPT_DISABLED
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_FORCE_HOLDOFF SL_RAIL_UTIL_COEX_OPT_DISABLED
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_MAC_HOLDOFF SL_RAIL_UTIL_COEX_OPT_DISABLED
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_REQ_FILTER_PASS SL_RAIL_UTIL_COEX_OPT_DISABLED
#endif

#if defined(SL_RAIL_UTIL_COEX_PRI_PORT) || defined(SL_RAIL_UTIL_COEX_PRI_GPIO) || defined(SL_RAIL_UTIL_COEX_DP_PORT)
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_TX_HIPRI          SL_RAIL_UTIL_COEX_OPT_TX_HIPRI
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_RX_HIPRI          SL_RAIL_UTIL_COEX_OPT_RX_HIPRI
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_RX_RETRY_HIPRI    SL_RAIL_UTIL_COEX_OPT_RX_RETRY_HIPRI
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_HIPRI_FILTER_PASS SL_RAIL_UTIL_COEX_OPT_HIPRI_FILTER_PASS
#else //!defined(SL_RAIL_UTIL_COEX_PRI_PORT) && !defined(SL_RAIL_UTIL_COEX_PRI_GPIO) && !defined(SL_RAIL_UTIL_COEX_DP_PORT)
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_TX_HIPRI          SL_RAIL_UTIL_COEX_OPT_DISABLED
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_RX_HIPRI          SL_RAIL_UTIL_COEX_OPT_DISABLED
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_RX_RETRY_HIPRI    SL_RAIL_UTIL_COEX_OPT_DISABLED
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_HIPRI_FILTER_PASS SL_RAIL_UTIL_COEX_OPT_DISABLED
#endif

#if defined(SL_RAIL_UTIL_COEX_RHO_PORT) || defined(RHO_GPIO)
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_RHO_ENABLED SL_RAIL_UTIL_COEX_OPT_RHO_ENABLED
#else //!(defined(SL_RAIL_UTIL_COEX_RHO_PORT) || defined(RHO_GPIO))
  #define PUBLIC_SL_RAIL_UTIL_COEX_OPT_RHO_ENABLED SL_RAIL_UTIL_COEX_OPT_DISABLED
#endif //defined(SL_RAIL_UTIL_COEX_RHO_PORT) || defined(RHO_GPIO)

#define PUBLIC_SL_RAIL_UTIL_COEX_OPT_ACK_HOLDOFF SL_RAIL_UTIL_COEX_OPT_ACK_HOLDOFF
#define PUBLIC_SL_RAIL_UTIL_COEX_OPT_ABORT_TX    SL_RAIL_UTIL_COEX_OPT_ABORT_TX

#if SL_RAIL_UTIL_COEX_PRIORITY_ESCALATION_ENABLE
#define PUBLIC_SL_RAIL_UTIL_COEX_OPT_CCA_THRESHOLD SL_RAIL_UTIL_COEX_OPT_CCA_THRESHOLD
#define PUBLIC_SL_RAIL_UTIL_COEX_OPT_MAC_RETRY_THRESHOLD SL_RAIL_UTIL_COEX_OPT_MAC_RETRY_THRESHOLD
#define PUBLIC_SL_RAIL_UTIL_COEX_OPT_MAC_FAIL_THRESHOLD SL_RAIL_UTIL_COEX_OPT_MAC_FAIL_THRESHOLD
#else //!SL_RAIL_UTIL_COEX_PRIORITY_ESCALATION_ENABLE
#define PUBLIC_SL_RAIL_UTIL_COEX_OPT_CCA_THRESHOLD SL_RAIL_UTIL_COEX_OPT_DISABLED
#define PUBLIC_SL_RAIL_UTIL_COEX_OPT_MAC_RETRY_THRESHOLD SL_RAIL_UTIL_COEX_OPT_DISABLED
#define PUBLIC_SL_RAIL_UTIL_COEX_OPT_MAC_FAIL_THRESHOLD SL_RAIL_UTIL_COEX_OPT_DISABLED
#endif //SL_RAIL_UTIL_COEX_PRIORITY_ESCALATION_ENABLE

// Public PTA options can be modified using public PTA APIs
#define PUBLIC_SL_RAIL_UTIL_COEX_OPTIONS (0U                                                         \
                                          | PUBLIC_SL_RAIL_UTIL_COEX_OPT_RX_RETRY_TIMEOUT_MS         \
                                          | PUBLIC_SL_RAIL_UTIL_COEX_OPT_ACK_HOLDOFF                 \
                                          | PUBLIC_SL_RAIL_UTIL_COEX_OPT_ABORT_TX                    \
                                          | PUBLIC_SL_RAIL_UTIL_COEX_OPT_TX_HIPRI                    \
                                          | PUBLIC_SL_RAIL_UTIL_COEX_OPT_RX_HIPRI                    \
                                          | PUBLIC_SL_RAIL_UTIL_COEX_OPT_RX_RETRY_HIPRI              \
                                          | PUBLIC_SL_RAIL_UTIL_COEX_OPT_RX_RETRY_REQ                \
                                          | PUBLIC_SL_RAIL_UTIL_COEX_OPT_RHO_ENABLED                 \
                                          | PUBLIC_SL_RAIL_UTIL_COEX_OPT_TOGGLE_REQ_ON_MACRETRANSMIT \
                                          | PUBLIC_SL_RAIL_UTIL_COEX_OPT_FORCE_HOLDOFF               \
                                          | PUBLIC_SL_RAIL_UTIL_COEX_OPT_MAC_HOLDOFF                 \
                                          | PUBLIC_SL_RAIL_UTIL_COEX_OPT_REQ_FILTER_PASS             \
                                          | PUBLIC_SL_RAIL_UTIL_COEX_OPT_HIPRI_FILTER_PASS           \
                                          | PUBLIC_SL_RAIL_UTIL_COEX_OPT_CCA_THRESHOLD               \
                                          | PUBLIC_SL_RAIL_UTIL_COEX_OPT_MAC_RETRY_THRESHOLD         \
                                          | PUBLIC_SL_RAIL_UTIL_COEX_OPT_MAC_FAIL_THRESHOLD          \
                                          )
// Constant PTA options can not be modified using public PTA APIs
#define CONST_SL_RAIL_UTIL_COEX_OPTIONS (~(PUBLIC_SL_RAIL_UTIL_COEX_OPTIONS))

#ifndef DEFAULT_SL_RAIL_UTIL_COEX_OPTIONS
  #define DEFAULT_SL_RAIL_UTIL_COEX_OPTIONS SL_RAIL_UTIL_COEX_OPT_DISABLED
#endif
#ifndef CONST_SL_RAIL_UTIL_COEX_OPTIONS
  #define CONST_SL_RAIL_UTIL_COEX_OPTIONS (~SL_RAIL_UTIL_COEX_OPT_DISABLED)
#endif

#if !defined(SL_RAIL_UTIL_COEX_PHY_SELECT_PORT)         \
  && (SL_RAIL_UTIL_COEX_DEFAULT_PHY_SELECT_TIMEOUT > 0) \
  && (SL_RAIL_UTIL_COEX_DEFAULT_PHY_SELECT_TIMEOUT < SL_RAIL_UTIL_COEX_PHY_SELECT_TIMEOUT_MAX)
#error "Select SL_RAIL_UTIL_COEX_PHY_SELECT GPIO before enabling COEX PHY select timeout!"
#endif

extern RAIL_Handle_t emPhyRailHandle;

void halStackRadioHoldOffPowerDown(void); // fwd ref
void halStackRadioHoldOffPowerUp(void);   // fwd ref

extern void emRadioEnablePta(bool enabled);
extern void emRadioHoldOffIsr(bool active);

#define COEX_STACK_EVENT_SUPPORT \
  (COEX_SUPPORT                  \
   || COEX_RHO_SUPPORT)          \

#if SL_RAIL_UTIL_COEX_PHY_ENABLED
  #ifdef _SILICON_LABS_32B_SERIES_1_CONFIG_1
  #error "COEX PHY is not supported on the selected platform."
  #endif
static uint8_t phySelectTimeoutMs = SL_RAIL_UTIL_COEX_PHY_SELECT_TIMEOUT_MAX;
#else //!SL_RAIL_UTIL_COEX_PHY_ENABLED
static uint8_t phySelectTimeoutMs = 0U;
#endif //SL_RAIL_UTIL_COEX_PHY_ENABLED

#include "rail_types.h"

COEX_Events_t sli_rail_util_ieee802154_coex_event_filter = ~COEX_EVENT_REQUEST_EVENTS;

#if SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
#if SL_RAIL_UTIL_COEX_PHY_ENABLED
static volatile bool coexNewPhySelectedCoex = true;
#else //!SL_RAIL_UTIL_COEX_PHY_ENABLED
static volatile bool coexNewPhySelectedCoex = false;
#endif //SL_RAIL_UTIL_COEX_PHY_ENABLED
#elif SL_RAIL_UTIL_COEX_PHY_ENABLED
#define coexNewPhySelectedCoex (true)
#else //!SL_RAIL_UTIL_COEX_PHY_ENABLED
#define coexNewPhySelectedCoex (false)
#endif //SL_RAIL_UTIL_COEX_PHY_ENABLED

bool sl_rail_util_coex_is_coex_phy_selected(void)
{
  return coexNewPhySelectedCoex;
}

#if SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
static bool phySelectInitialized = false;
static void phySelectTick(void);
#endif //SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT

#if COEX_STACK_EVENT_SUPPORT
static sl_rail_util_coex_options_t sl_rail_util_coex_options = DEFAULT_SL_RAIL_UTIL_COEX_OPTIONS;
static void coexEventsCb(COEX_Events_t events);
#if SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
static void phySelectIsr(void);
#endif//SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT

void sli_rail_util_ieee802154_coex_on_event(COEX_Events_t events)
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

#define MAP_COEX_OPTION(coexOpt, sl_rail_util_coex_opt) \
  (mapCoexOption(&coexOptions, coexOpt, options, sl_rail_util_coex_opt))

static void mapCoexOption(COEX_Options_t * coexOptions, COEX_Options_t coexOpt,
                          sl_rail_util_coex_options_t options, sl_rail_util_coex_options_t sl_rail_util_coex_opt)
{
  if ((options & sl_rail_util_coex_opt) != 0U) {
    *coexOptions |= coexOpt;
  } else {
    *coexOptions &= ~coexOpt;
  }
}

// Public API

sl_status_t sl_rail_util_coex_set_bool(sl_rail_util_coex_options_t option, bool value)
{
  sl_rail_util_coex_options_t ptaOptions = sl_rail_util_coex_get_options();
  ptaOptions = value ? (ptaOptions | option) : (ptaOptions & (~option));
  return sl_rail_util_coex_set_options(ptaOptions);
}

sl_rail_util_coex_options_t sl_rail_util_coex_get_options(void)
{
  return sl_rail_util_coex_options;
}

sl_status_t sl_rail_util_coex_set_options(sl_rail_util_coex_options_t options)
{
  sl_status_t status = SL_STATUS_NOT_SUPPORTED;
  COEX_HAL_Init();
  COEX_Options_t coexOptions = COEX_GetOptions();

#ifndef DEBUG_PTA
  if ((options & CONST_SL_RAIL_UTIL_COEX_OPTIONS) != (DEFAULT_SL_RAIL_UTIL_COEX_OPTIONS & CONST_SL_RAIL_UTIL_COEX_OPTIONS)) {
    //Return error if any options argument is unsupported or constant
    return SL_STATUS_INVALID_PARAMETER;
  }
#endif //!DEBUG_PTA

  if (((~sl_rail_util_coex_options) & options & SL_RAIL_UTIL_COEX_OPT_FORCE_HOLDOFF) != 0U) {
    //Cancel all requests if force holdoff is enabled
    //All future requests will be blocked until force holdoff is disabled
    (void) sl_rail_util_coex_set_tx_request(SL_RAIL_UTIL_COEX_REQ_OFF, NULL);
    (void) sl_rail_util_coex_set_rx_request(SL_RAIL_UTIL_COEX_REQ_OFF, NULL);
  }
  MAP_COEX_OPTION(COEX_OPTION_RHO_ENABLED, SL_RAIL_UTIL_COEX_OPT_RHO_ENABLED);

  if (COEX_SetOptions(coexOptions)) {
    sl_rail_util_coex_options = options;
    status = SL_STATUS_OK;
  }
  return status;
}
#else //!COEX_STACK_EVENT_SUPPORT

sl_status_t sl_rail_util_coex_set_bool(sl_rail_util_coex_options_t option, bool value)
{
  UNUSED_VAR(option);
  UNUSED_VAR(value);
  return SL_STATUS_NOT_SUPPORTED;
}

sl_rail_util_coex_options_t sl_rail_util_coex_get_options(void)
{
  return SL_RAIL_UTIL_COEX_OPT_DISABLED;
}

sl_status_t sl_rail_util_coex_set_options(sl_rail_util_coex_options_t options)
{
  UNUSED_VAR(options);
  return SL_STATUS_NOT_SUPPORTED;
}
#endif //COEX_STACK_EVENT_SUPPORT

#ifdef SL_RAIL_UTIL_COEX_DP_ENABLED
sl_status_t sl_rail_util_coex_set_directional_priority_pulse_width(uint8_t pulseWidthUs)
{
  return COEX_HAL_SetDpPulseWidth(pulseWidthUs)
         ? SL_STATUS_OK : SL_STATUS_NOT_SUPPORTED;
}

uint8_t sl_rail_util_coex_get_directional_priority_pulse_width(void)
{
  return COEX_HAL_GetDpPulseWidth();
}
#else //SL_RAIL_UTIL_COEX_DP_ENABLED
sl_status_t sl_rail_util_coex_set_directional_priority_pulse_width(uint8_t pulseWidthUs)
{
  UNUSED_VAR(pulseWidthUs);
  return SL_STATUS_NOT_SUPPORTED;
}

uint8_t sl_rail_util_coex_get_directional_priority_pulse_width(void)
{
  return 0;
}
#endif //SL_RAIL_UTIL_COEX_DP_ENABLED

#if     COEX_SUPPORT
static void emCoexCounter(COEX_Events_t events);
static COEX_ReqState_t txReq;
static COEX_ReqState_t rxReq;

static bool cancelTransmit(void)
{
  return RAIL_StopTx(emPhyRailHandle, RAIL_STOP_MODE_ACTIVE) == RAIL_STATUS_NO_ERROR;
}

static void coexEventsCb(COEX_Events_t events)
{
  if ((events & COEX_EVENT_COEX_CHANGED) != 0U) {
    emRadioEnablePta((COEX_GetOptions() & COEX_OPTION_COEX_ENABLED) != 0U);
  }
  if ((events & COEX_EVENT_GRANT_RELEASED) != 0U
      && (sl_rail_util_coex_get_options() & SL_RAIL_UTIL_COEX_OPT_ABORT_TX) != 0U
      && (txReq.coexReq & COEX_REQ_ON) != 0U) {
    if (cancelTransmit()) {
      events |= COEX_EVENT_TX_ABORTED;
    }
  }
  emCoexCounter(events);
}

// Certain radios may want to override this with their own
SL_WEAK void emRadioEnablePta(bool enabled)
{
  UNUSED_VAR(enabled);
}

SL_WEAK void sl_rail_util_coex_counter_on_event(sl_rail_util_coex_event_t event)
{
  (void)event;
}

#define coexCounterType(type) ((events & COEX_EVENT_PRIORITY_ASSERTED) != 0U \
                               ? (SL_RAIL_UTIL_COEX_EVENT_HI_PRI_ ## type)   \
                               : (SL_RAIL_UTIL_COEX_EVENT_LO_PRI_ ## type))

#define coexCounter(type) sl_rail_util_coex_counter_on_event(coexCounterType(type))

static void emCoexCounter(COEX_Events_t events)
{
  if ((events & COEX_EVENT_REQUEST_RELEASED) != 0U) {
    coexCounter(REQUESTED);
  }
  if ((events & COEX_EVENT_REQUEST_DENIED) != 0U) {
    coexCounter(DENIED);
  } else if ((events & COEX_EVENT_TX_ABORTED) != 0U) {
    coexCounter(TX_ABORTED);
  } else {
    // Request not denied
  }
}

#if SL_RAIL_UTIL_COEX_PRIORITY_ESCALATION_ENABLE || defined(DEBUG_PTA)

static uint8_t ptaCcaFailEscalate = 0U;
static uint8_t ptaMacFailEscalate = 0U;
static bool priorityEscalated = false;

static void escalatePriority(uint8_t * counter, uint8_t threshold)
{
  if (((sl_rail_util_coex_get_options() & SL_RAIL_UTIL_COEX_OPT_TX_HIPRI) == 0U)
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

#define SL_RAIL_UTIL_COEX_RX_TIMEOUT_US 10000u
#define SL_RAIL_UTIL_COEX_RX_RETRY_US (1000u * sl_rail_util_coex_get_option_mask(SL_RAIL_UTIL_COEX_OPT_RX_RETRY_TIMEOUT_MS, \
                                                                                 SL_RAIL_UTIL_COEX_OPT_SHIFT_RX_RETRY_TIMEOUT_MS))
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
  if (phySelectTimeoutMs != SL_RAIL_UTIL_COEX_PHY_SELECT_TIMEOUT_MAX && phySelectTimeoutMs != 0U && !RAIL_IsMultiTimerRunning(&ptaPhySelectTimer)) {
    coexNewPhySelectedCoex = false;
    COEX_EnablePhySelectIsr(true);
  }
}

#define MICROSECONDS_PER_MILLISECOND (1000U)
static void phySelectIsr(void)
{
  coexNewPhySelectedCoex = true;
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
  (void) sl_rail_util_coex_set_rx_request(SL_RAIL_UTIL_COEX_REQ_OFF, NULL);
}

// Both timers do the same thing, and cancel each other
#define ptaRxRetryTimerCb ptaRxTimerCb

static bool stackEventTickInitialized = false;

static void stackEventTickInit(void)
{
  if (!stackEventTickInitialized && emPhyRailHandle != NULL) {
#if SL_RAIL_UTIL_COEX_PWM_REQ_ENABLED && SL_RAIL_UTIL_COEX_PWM_DEFAULT_ENABLED
    sl_rail_util_coex_set_request_pwm(COEX_REQ_PWM
                                      | (SL_RAIL_UTIL_COEX_PWM_PRIORITY << COEX_REQ_HIPRI_SHIFT),
                                      NULL,
                                      SL_RAIL_UTIL_COEX_PWM_REQ_DUTYCYCLE,
                                      SL_RAIL_UTIL_COEX_PWM_REQ_PERIOD);
#endif
#if SL_RAIL_UTIL_COEX_IEEE802154_SIGNAL_IDENTIFIER_ENABLED
    RAIL_IEEE802154_ConfigSignalIdentifier(emPhyRailHandle, RAIL_IEEE802154_SIGNAL_IDENTIFIER_MODE_154);
#endif
    stackEventTickInitialized = true;
  }
}

sl_rail_util_ieee802154_stack_status_t sl_rail_util_coex_on_event(
  sl_rail_util_ieee802154_stack_event_t stack_event,
  uint32_t supplement)
{
  bool isReceivingFrame = false;
  sl_status_t status = SL_STATUS_OK;

  switch (stack_event) {
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TICK:
      if (sl_rail_util_coex_is_enabled()) {
        stackEventTickInit();
      }
#if SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
      if (phySelectInitialized) {
        phySelectTick();
      } else if (emPhyRailHandle != NULL) {
        sl_rail_util_coex_set_phy_select_timeout(SL_RAIL_UTIL_COEX_DEFAULT_PHY_SELECT_TIMEOUT);
        phySelectInitialized = true;
      }
#endif //SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
      break;

    // RX events:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_LISTEN:
      setCoexPowerState(true);
      break;
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_STARTED:
      if (sl_rail_util_coex_is_enabled()) {
        isReceivingFrame = (bool) supplement;
        if (isReceivingFrame) {
          (void) sl_rail_util_coex_set_rx_request(sl_rail_util_coex_frame_detect_req(), NULL);
          cancelTimer(&ptaRxRetryTimer);
          setTimer(&ptaRxTimer, SL_RAIL_UTIL_COEX_RX_TIMEOUT_US, &ptaRxTimerCb);
        }
      }
      break;

    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACCEPTED:
      if (sl_rail_util_coex_is_enabled()) {
        isReceivingFrame = (bool) supplement;
        if (isReceivingFrame) {
          sl_rail_util_coex_req_t filterPass = sl_rail_util_coex_filter_pass_req();
          if (filterPass != sl_rail_util_coex_frame_detect_req()) {
            (void) sl_rail_util_coex_set_rx_request(filterPass, NULL);
          }
        }
      }
      break;

    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_CORRUPTED:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACK_BLOCKED:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACK_ABORTED:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_SIGNAL_DETECTED:
      if (sl_rail_util_coex_is_enabled()) {
        if ((sl_rail_util_coex_get_options() & SL_RAIL_UTIL_COEX_OPT_RX_RETRY_REQ) != 0U) {
          cancelTimer(&ptaRxTimer);
          // Assert request and start rx retry timer.
          (void) sl_rail_util_coex_set_rx_request(SL_RAIL_UTIL_COEX_REQ_ON | SL_RAIL_UTIL_COEX_RX_RETRY_PRI, NULL);
          setTimer(&ptaRxRetryTimer, SL_RAIL_UTIL_COEX_RX_RETRY_US, &ptaRxRetryTimerCb);
          break;
        }
      }
    // FALLTHROUGH

    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_FILTERED:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ENDED:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACK_SENT:
      isReceivingFrame = (bool) supplement;
    // FALLTHROUGH

    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_IDLED:
      if (sl_rail_util_coex_is_enabled()) {
        cancelTimer(&ptaRxTimer);
        cancelTimer(&ptaRxRetryTimer);
        if (!isReceivingFrame) {
          (void) sl_rail_util_coex_set_rx_request(SL_RAIL_UTIL_COEX_REQ_OFF, NULL);
        }
      }
      break;

    // TX events:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_PENDED_MAC:
      if (sl_rail_util_coex_is_enabled()) {
        sl_rail_util_coex_cb_t cb = (sl_rail_util_coex_cb_t) supplement;
        sl_rail_util_coex_options_t ptaOptions = sl_rail_util_coex_get_options();
        if ((ptaOptions & SL_RAIL_UTIL_COEX_OPT_MAC_AND_FORCE_HOLDOFFS) == SL_RAIL_UTIL_COEX_OPT_MAC_AND_FORCE_HOLDOFFS) {
          // Tell caller NOT to transmit
          status = SL_RAIL_UTIL_IEEE802154_STACK_STATUS_HOLDOFF;
        } else if (((ptaOptions & SL_RAIL_UTIL_COEX_OPT_MAC_AND_FORCE_HOLDOFFS) == SL_RAIL_UTIL_COEX_OPT_MAC_HOLDOFF)
                   && (cb != NULL)) {
          // Assert Tx REQUEST  and wait for GRANT
          sl_rail_util_coex_set_tx_request(SL_RAIL_UTIL_COEX_REQ_ON | SL_RAIL_UTIL_COEX_TX_PRI | SL_RAIL_UTIL_COEX_REQCB_GRANTED, cb);
          status = SL_RAIL_UTIL_IEEE802154_STACK_STATUS_CB_PENDING;
        } else {
          // No callback - proceed with a normal transmit
        }
      }
      break;

    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_PENDED_PHY:
      if (sl_rail_util_coex_is_enabled()) {
        bool isCcaTransmit = (bool) supplement;
        if (isCcaTransmit && ((sl_rail_util_coex_get_options() & SL_RAIL_UTIL_COEX_OPT_LONG_REQ) == 0U)) {
          // Defer Tx req to CCA_SOON event
        } else {
          sl_rail_util_coex_set_tx_request(SL_RAIL_UTIL_COEX_REQ_ON | SL_RAIL_UTIL_COEX_TX_PRI, NULL);
        }
      }
      setCoexPowerState(true);
      break;

    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_CCA_SOON:
      if (sl_rail_util_coex_is_enabled()) {
        (void) sl_rail_util_coex_set_tx_request(SL_RAIL_UTIL_COEX_REQ_ON | SL_RAIL_UTIL_COEX_TX_PRI, NULL);
      }
      break;

    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_CCA_BUSY:
      if (sl_rail_util_coex_is_enabled()) {
        bool isNextCcaImminent = (bool) supplement;
        if (!isNextCcaImminent) {
          (void) sl_rail_util_coex_set_tx_request(SL_RAIL_UTIL_COEX_REQ_OFF, NULL);
        }
      }
      break;

    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_STARTED:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ACK_WAITING:
      // Currently these events are advisory and leave TX REQ asserted
      break;

    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ACK_RECEIVED:
      if (sl_rail_util_coex_is_enabled()) {
        bool hasFramePending = (bool) supplement;
        (void) hasFramePending;
        (void) sl_rail_util_coex_set_tx_request(SL_RAIL_UTIL_COEX_REQ_OFF, NULL);
        deescalatePriority();
      }
      break;

    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ACK_TIMEDOUT:
      if (sl_rail_util_coex_is_enabled()) {
        uint8_t macRetries = (uint8_t) supplement;
        if ((macRetries == 0) || sl_rail_util_coex_get_tx_req_release()) {
          (void) sl_rail_util_coex_set_tx_request(SL_RAIL_UTIL_COEX_REQ_OFF, NULL);
          if (macRetries == 0) {
            escalatePriority(&ptaMacFailEscalate,
                             sl_rail_util_coex_get_mac_fail_counter_threshold());
          }
        } else {
          // Hold Tx REQ for a MAC retransmit
        }
      }
      break;

    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_BLOCKED:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ABORTED:
      if (sl_rail_util_coex_is_enabled()) {
        (void) sl_rail_util_coex_set_tx_request(SL_RAIL_UTIL_COEX_REQ_OFF, NULL);
        bool pktRequestedAck = (bool) supplement;
        if (pktRequestedAck) {
          if (stack_event == SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_BLOCKED) {
            escalatePriority(&ptaCcaFailEscalate,
                             sl_rail_util_coex_get_cca_counter_threshold());
          }
          escalatePriority(&ptaMacFailEscalate,
                           sl_rail_util_coex_get_mac_fail_counter_threshold());
        }
      }
      break;

    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_IDLED:
      setCoexPowerState(false);
    // FALLTHROUGH
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ENDED:
      if (sl_rail_util_coex_is_enabled()) {
        (void) sl_rail_util_coex_set_tx_request(SL_RAIL_UTIL_COEX_REQ_OFF, NULL);
      }

    default:
      break;
  }
  return status;
}

static sl_status_t internalPtaSetRequest(COEX_ReqState_t * coexReqState,
                                         COEX_Req_t coexReq,
                                         COEX_ReqCb_t coexCb)
{
  if (txReq.coexReq == COEX_REQ_OFF && rxReq.coexReq == COEX_REQ_OFF) {
    sli_rail_util_ieee802154_coex_event_filter &= ~COEX_EVENT_REQUEST_EVENTS;
  } else {
    sli_rail_util_ieee802154_coex_event_filter |= COEX_EVENT_REQUEST_EVENTS;
  }
  // if force holdoff is enabled, don't set request
  return (((sl_rail_util_coex_options & SL_RAIL_UTIL_COEX_OPT_FORCE_HOLDOFF) == 0U)
          && COEX_SetRequest(coexReqState, coexReq, coexCb))
         ? SL_STATUS_OK : SL_STATUS_NOT_SUPPORTED;
}

sl_rail_util_coex_req_t sl_rail_util_coex_frame_detect_req(void)
{
  sl_rail_util_coex_options_t options = sl_rail_util_coex_get_options();
  sl_rail_util_coex_req_t syncDet = SL_RAIL_UTIL_COEX_REQ_OFF;
  if (sl_rail_util_coex_is_enabled() && (options & SL_RAIL_UTIL_COEX_OPT_REQ_FILTER_PASS) == 0) {
    syncDet |= SL_RAIL_UTIL_COEX_REQ_ON;
    if ((options & SL_RAIL_UTIL_COEX_OPT_RX_HIPRI) != 0U) {
      syncDet |= SL_RAIL_UTIL_COEX_REQ_HIPRI;
    }
  }
  return syncDet;
}

sl_rail_util_coex_req_t sl_rail_util_coex_filter_pass_req(void)
{
  sl_rail_util_coex_options_t options = sl_rail_util_coex_get_options();
  sl_rail_util_coex_req_t filterPass = SL_RAIL_UTIL_COEX_REQ_OFF;
  if (sl_rail_util_coex_is_enabled()) {
    filterPass |= SL_RAIL_UTIL_COEX_REQ_ON;
    if (options & (SL_RAIL_UTIL_COEX_OPT_RX_HIPRI | SL_RAIL_UTIL_COEX_OPT_HIPRI_FILTER_PASS)) {
      filterPass |= SL_RAIL_UTIL_COEX_REQ_HIPRI;
    }
  }
  return filterPass;
}

sl_status_t sl_rail_util_coex_set_tx_request(COEX_Req_t coexReq, COEX_ReqCb_t coexCb)
{
#if SL_RAIL_UTIL_COEX_PRIORITY_ESCALATION_ENABLE || defined(DEBUG_PTA)
  if (((coexReq & COEX_REQ_ON) != 0U) && priorityEscalated) {
    coexReq |= COEX_REQ_HIPRI;
  }
#endif // SL_RAIL_UTIL_COEX_PRIORITY_ESCALATION_ENABLE || defined(DEBUG_PTA)
  return internalPtaSetRequest(&txReq, coexReq, coexCb);
}

sl_status_t sl_rail_util_coex_set_rx_request(COEX_Req_t coexReq, COEX_ReqCb_t coexCb)
{
#if SL_RAIL_UTIL_COEX_IEEE802154_SIGNAL_IDENTIFIER_ENABLED
  if (emPhyRailHandle != NULL) {
    // Restart signal detection when request deasserts and vice-versa.
    bool enable = ((coexReq & SL_RAIL_UTIL_COEX_REQ_ON) == SL_RAIL_UTIL_COEX_REQ_ON) ? false : true;
    (void) RAIL_IEEE802154_EnableSignalDetection(emPhyRailHandle, enable);
  }
#endif
  return internalPtaSetRequest(&rxReq, coexReq, coexCb);
}

sl_status_t sl_rail_util_coex_set_enable(bool enabled)
{
  COEX_Options_t coexOptions = COEX_GetOptions();

  if (enabled) {
    coexOptions |= COEX_OPTION_COEX_ENABLED;
  } else {
    coexOptions &= ~COEX_OPTION_COEX_ENABLED;
  }
  return COEX_SetOptions(coexOptions)
         ? SL_STATUS_OK : SL_STATUS_NOT_SUPPORTED;
}

bool sl_rail_util_coex_is_enabled(void)
{
  return (COEX_GetOptions() & COEX_OPTION_COEX_ENABLED) != 0U;
}

#else // !COEX_REQ_SUPPORT
#if COEX_STACK_EVENT_SUPPORT
static void coexEventsCb(COEX_Events_t events)
{
  UNUSED_VAR(events);
}
#endif //!COEX_RHO_SUPPORT

sl_rail_util_coex_req_t sl_rail_util_coex_frame_detect_req(void)
{
  return SL_RAIL_UTIL_COEX_REQ_OFF;
}

sl_rail_util_coex_req_t sl_rail_util_coex_filter_pass_req(void)
{
  return SL_RAIL_UTIL_COEX_REQ_OFF;
}

sl_status_t sl_rail_util_coex_set_tx_request(COEX_Req_t coexReq, COEX_ReqCb_t coexCb)
{
  UNUSED_VAR(coexReq);
  UNUSED_VAR(coexCb);
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_rail_util_coex_set_rx_request(COEX_Req_t coexReq, COEX_ReqCb_t coexCb)
{
  UNUSED_VAR(coexReq);
  UNUSED_VAR(coexCb);
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_rail_util_coex_set_enable(bool enabled)
{
  UNUSED_VAR(enabled);
  return SL_STATUS_NOT_SUPPORTED;
}

bool sl_rail_util_coex_is_enabled(void)
{
  return false;
}

sl_rail_util_ieee802154_stack_status_t sl_rail_util_coex_on_event(
  sl_rail_util_ieee802154_stack_event_t stack_event,
  uint32_t supplement)
{
  UNUSED_VAR(stack_event);
  UNUSED_VAR(supplement);
  return SL_STATUS_NOT_SUPPORTED;
}
#endif//COEX_SUPPORT

#if COEX_SUPPORT
const sl_rail_util_coex_pwm_args_t *sl_rail_util_coex_get_request_pwm_args(void)
{
  return (const sl_rail_util_coex_pwm_args_t *)COEX_GetPwmRequest();
}

sl_status_t sl_rail_util_coex_set_request_pwm(sl_rail_util_coex_req_t ptaReq,
                                              sl_rail_util_coex_cb_t ptaCb,
                                              uint8_t dutyCycle,
                                              uint8_t periodHalfMs)
{
  return COEX_SetPwmRequest(ptaReq,
                            ptaCb,
                            dutyCycle,
                            periodHalfMs)
         ? SL_STATUS_OK : SL_STATUS_NOT_SUPPORTED;
}

#else//!COEX_SUPPORT

sl_status_t sl_rail_util_coex_set_request_pwm(sl_rail_util_coex_req_t ptaReq,
                                              sl_rail_util_coex_cb_t ptaCb,
                                              uint8_t dutyCycle,
                                              uint8_t periodHalfMs)
{
  UNUSED_VAR(ptaReq);
  UNUSED_VAR(ptaCb);
  UNUSED_VAR(dutyCycle);
  UNUSED_VAR(periodHalfMs);
  return SL_STATUS_NOT_SUPPORTED;
}

const sl_rail_util_coex_pwm_args_t *sl_rail_util_coex_get_request_pwm_args(void)
{
  return NULL;
}

void sli_rail_util_ieee802154_coex_on_event(COEX_Events_t events)
{
  (void)events;
}

#endif//COEX_SUPPORT

#ifdef COEX_HAL_SMALL_RHO
// RHO implementation is defined in coexistence-hal.c
#elif COEX_RHO_SUPPORT

bool sl_rail_util_coex_get_radio_holdoff(void)
{
  return (COEX_GetOptions() & COEX_OPTION_RHO_ENABLED) != 0U;
}

sl_status_t sl_rail_util_coex_set_radio_holdoff(bool enabled)
{
  return sl_rail_util_coex_set_bool(SL_RAIL_UTIL_COEX_OPT_RHO_ENABLED, enabled);
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

bool sl_rail_util_coex_get_radio_holdoff(void)
{
  return false;
}

sl_status_t sl_rail_util_coex_set_radio_holdoff(bool enabled)
{
  UNUSED_VAR(enabled);
  return SL_STATUS_NOT_SUPPORTED;
}

void halStackRadioHoldOffPowerDown(void)
{
}

void halStackRadioHoldOffPowerUp(void)
{
}

#endif//COEX_RHO_SUPPORT

sl_status_t sl_rail_util_coex_set_phy_select_timeout(uint8_t timeoutMs)
{
#if SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
  if (phySelectTimeoutMs != timeoutMs) {
    phySelectTimeoutMs = timeoutMs;
    if (timeoutMs == 0U) {
      RAIL_CancelMultiTimer(&ptaPhySelectTimer);
      coexNewPhySelectedCoex = false;
    } else if (timeoutMs == SL_RAIL_UTIL_COEX_PHY_SELECT_TIMEOUT_MAX) {
      RAIL_CancelMultiTimer(&ptaPhySelectTimer);
      coexNewPhySelectedCoex = true;
    } else {
#ifdef SL_RAIL_UTIL_COEX_PHY_SELECT_PORT
      COEX_EnablePhySelectIsr(true);
      return SL_STATUS_OK;
#else //!SL_RAIL_UTIL_COEX_PHY_SELECT_PORT
      return SL_STATUS_NOT_SUPPORTED;
#endif //SL_RAIL_UTIL_COEX_PHY_SELECT_PORT
    }
#ifdef SL_RAIL_UTIL_COEX_PHY_SELECT_PORT
    COEX_EnablePhySelectIsr(false);
#endif //SL_RAIL_UTIL_COEX_PHY_SELECT_PORT
  }
  return SL_STATUS_OK;
#else //SL_RAIL_UTIL_COEX_PHY_ENABLED
  return (timeoutMs == phySelectTimeoutMs) ? SL_STATUS_OK : SL_STATUS_NOT_SUPPORTED;
#endif //SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
}

uint8_t sl_rail_util_coex_get_phy_select_timeout(void)
{
  return phySelectTimeoutMs;
}

#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
bool sl_rail_util_coex_get_gpio_input_override(sl_rail_util_coex_gpio_index_t gpioIndex)
{
  return COEX_GetGpioInputOverride((COEX_GpioIndex_t)(gpioIndex + 1));
}

sl_status_t sl_rail_util_coex_set_gpio_input_override(sl_rail_util_coex_gpio_index_t gpioIndex,
                                                      bool enabled)
{
  return COEX_SetGpioInputOverride((COEX_GpioIndex_t)(gpioIndex + 1), enabled)
         ? SL_STATUS_OK : SL_STATUS_NOT_SUPPORTED;
}
#else //!SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
bool sl_rail_util_coex_get_gpio_input_override(sl_rail_util_coex_gpio_index_t gpioIndex)
{
  UNUSED_VAR(gpioIndex);
  return false;
}

sl_status_t sl_rail_util_coex_set_gpio_input_override(sl_rail_util_coex_gpio_index_t gpioIndex,
                                                      bool enabled)
{
  UNUSED_VAR(gpioIndex);
  UNUSED_VAR(enabled);
  return SL_STATUS_NOT_SUPPORTED;
}
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT

void sl_rail_util_coex_init(void)
{
  sl_rail_util_coex_set_options(DEFAULT_SL_RAIL_UTIL_COEX_OPTIONS);
#if defined(SL_RAIL_UTIL_COEX_REQ_PORT) || defined(SL_RAIL_UTIL_COEX_GNT_PORT)
  sl_rail_util_coex_set_enable(true);
#endif //defined(SL_RAIL_UTIL_COEX_REQ_PORT) || defined(SL_RAIL_UTIL_COEX_GNT_PORT)
}

bool sl_rail_util_is_coex_signal_identifier_enabled(void)
{
#if SL_RAIL_UTIL_COEX_IEEE802154_SIGNAL_IDENTIFIER_ENABLED
  return true;
#else
  return false;
#endif
}
