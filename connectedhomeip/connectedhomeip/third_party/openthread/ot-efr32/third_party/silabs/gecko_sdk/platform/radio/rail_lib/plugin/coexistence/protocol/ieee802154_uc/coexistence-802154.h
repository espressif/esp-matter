/***************************************************************************//**
 * @file
 * @brief Packet traffic arbitration interface.
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

/** @addtogroup micro
 * See also coexistence-802154.h for source code.
 *@{
 */

#ifndef __COEXISTENCE_802154_H__
#define __COEXISTENCE_802154_H__

#include "sl_status.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
#include "coexistence-hal.h"
#include "sl_rail_util_coex_config.h"
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

#include "rail_util_ieee802154/sl_rail_util_ieee802154_stack_event.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @name PTA Configuration Definitions
 *
 * The following are used to aid in the abstraction with Packet
 * Traffic Arbitration (PTA).  The microcontroller-specific sources
 * use these definitions so they are able to work across a variety of
 * boards which could have different connections.  The names and
 * ports/pins used below are intended to match with a schematic of the
 * system to provide the abstraction.
 *
 * PTA 2-wire (Request out/shared, Grant in) or 3-wire (Request out/shared,
 * Priority out, and Grant in) configurations are supported.
 */
//@{

#if  defined(SL_RAIL_UTIL_COEX_REQ_PORT) || defined(SL_RAIL_UTIL_COEX_GNT_PORT)
// Initial bootup configuration is to enable PTA
  #define halInternalInitPta() sl_rail_util_coex_set_enable(true)
#else//!(defined(SL_RAIL_UTIL_COEX_REQ_PORT) || defined(SL_RAIL_UTIL_COEX_GNT_PORT))
// Initial bootup configuration is not to enable PTA
  #define halInternalInitPta() /* no-op */
#endif//(defined(SL_RAIL_UTIL_COEX_REQ_PORT) || defined(SL_RAIL_UTIL_COEX_GNT_PORT))

//@} //END OF PTA CONFIGURATION DEFINITIONS

// sl_rail_util_coex Public API:
//
// sl_rail_util_coex_req_t is a bitmask of features:
//     7     6     5     4     3     2     1     0
// +-----+-----+-----+-----+-----+-----+-----+-----+
// |   0 | no  | no  |wait |wait |force|hipri| req |
// |     | req |grant|grant| req |     |     |     |
// +-----+-----+-----+-----+-----+-----+-----+-----+
//
// This mask is used for both requests and callbacks to
// represent status.

  #define SL_RAIL_UTIL_COEX_REQ_OFF         (COEX_REQ_OFF)         // Negate request
  #define SL_RAIL_UTIL_COEX_REQ_ON          (COEX_REQ_ON)          // Assert request
  #define SL_RAIL_UTIL_COEX_REQ_HIPRI       (COEX_REQ_HIPRI)       // Request is hi-pri
  #define SL_RAIL_UTIL_COEX_REQ_FORCE       (COEX_REQ_FORCE)       // Force assertion immediately
  #define SL_RAIL_UTIL_COEX_REQCB_REQUESTED (COEX_REQCB_REQUESTED) // Callback when REQUEST asserted
  #define SL_RAIL_UTIL_COEX_REQCB_GRANTED   (COEX_REQCB_GRANTED)   // Callback when GRANT asserted
  #define SL_RAIL_UTIL_COEX_REQCB_NEGATED   (COEX_REQCB_NEGATED)   // Callback when GRANT negated
  #define SL_RAIL_UTIL_COEX_REQCB_OFF       (COEX_REQCB_OFF)       // Callback when REQUEST removed
  #define SL_RAIL_UTIL_COEX_REQ_PWM         (COEX_REQ_PWM)         // PWM request

#define sl_rail_util_coex_req_t COEX_Req_t

#define sl_rail_util_coex_cb_t COEX_ReqCb_t

//
// sl_rail_util_coex_options_t is a bitmask of features:
//       7       6       5       4       3       2       1       0
// +-------+-------+-------+-------+-------+-------+-------+-------+
// |rxretry|rxretry|rxretry|rxretry|rxretry|rxretry|rxretry|rxretry|
// |timeout|timeout|timeout|timeout|timeout|timeout|timeout|timeout|
// +-------+-------+-------+-------+-------+-------+-------+-------+
//
//      15      14      13      12      11      10       9       8
// +-------+-------+-------+-------+-------+-------+-------+-------+
// |toggle |rho    |retry  |rxretry|rx     |tx     |abort  |ack    |
// |req    |on     |req    |hipri  |hipri  |hipri  |tx     |holdoff|
// +-------+-------+-------+-------+-------+-------+-------+-------+
//
//      23      22      21      20      19      18      17      16
// +-------+-------+-------+-------+-------+-------+-------+-------+
// |mac try|cca    |cca    |cca    |filter |req on |mac    |force  |
// |counter|counter|counter|counter|hipri  |filter |holdoff|holdoff|
// +-------+-------+-------+-------+-------+-------+-------+-------+
//
//      31      30      29      28      27      26      25      24
// +-------+-------+-------+-------+-------+-------+-------+-------+
// |long   |   0   |   0   |   0   |   0   |macfail|macfail|mac try|
// |req    |       |       |       |       |counter|counter|counter|
// +-------+-------+-------+-------+-------+-------+-------+-------+
// This mask is used for both requests and callbacks to
// represent status.

  #define SL_RAIL_UTIL_COEX_OPT_DISABLED             0u          // Disable option
  #define SL_RAIL_UTIL_COEX_OPT_RX_RETRY_TIMEOUT_MS (0xffu)      // Rx retry request timeout
  #define SL_RAIL_UTIL_COEX_OPT_ACK_HOLDOFF         (1u << 8)    // Enable ack radio holdoff
  #define SL_RAIL_UTIL_COEX_OPT_ABORT_TX            (1u << 9)    // Abort mid TX if grant is lost
  #define SL_RAIL_UTIL_COEX_OPT_TX_HIPRI            (1u << 10)   // Tx request is hi-pri
  #define SL_RAIL_UTIL_COEX_OPT_RX_HIPRI            (1u << 11)   // Rx request is hi-pri
  #define SL_RAIL_UTIL_COEX_OPT_RX_RETRY_HIPRI      (1u << 12)   // Rx retry request is hi-pri
  #define SL_RAIL_UTIL_COEX_OPT_RX_RETRY_REQ        (1u << 13)   // Request on corrupt packet
  #define SL_RAIL_UTIL_COEX_OPT_RHO_ENABLED         (1u << 14)   // Enable RHO
  #define SL_RAIL_UTIL_COEX_OPT_TOGGLE_REQ_ON_MACRETRANSMIT \
  (1u << 15)                                       // Enable toggle on retransmit
  #define SL_RAIL_UTIL_COEX_OPT_FORCE_HOLDOFF       (1u << 16)   // Force holdoff by disabling request
  #define SL_RAIL_UTIL_COEX_OPT_MAC_HOLDOFF         (1u << 17)   // Hold off transmission in the mac layer
  #define SL_RAIL_UTIL_COEX_OPT_REQ_FILTER_PASS     (1u << 18)   // Delay asserting request until address filtering passes
  #define SL_RAIL_UTIL_COEX_OPT_HIPRI_FILTER_PASS   (1u << 19)   // Assert request with high priority after address filtering passes
  #define SL_RAIL_UTIL_COEX_OPT_CCA_THRESHOLD       (0x7u << 20) // CCA failure counter threshold
  #define SL_RAIL_UTIL_COEX_OPT_MAC_RETRY_THRESHOLD (0x3u << 23) // MAC retry counter threshold
  #define SL_RAIL_UTIL_COEX_OPT_MAC_FAIL_THRESHOLD  (0x3u << 25) // MAC failure counter threshold
  #define SL_RAIL_UTIL_COEX_OPT_LONG_REQ            (1u << 31)   // Hold request across CCA failures

  #define SL_RAIL_UTIL_COEX_OPT_SHIFT_CCA_THRESHOLD         (20)
  #define SL_RAIL_UTIL_COEX_OPT_SHIFT_MAC_RETRY_THRESHOLD   (23)
  #define SL_RAIL_UTIL_COEX_OPT_SHIFT_MAC_FAIL_THRESHOLD (25)
  #define SL_RAIL_UTIL_COEX_OPT_SHIFT_RX_RETRY_TIMEOUT_MS (0)
  #define SL_RAIL_UTIL_COEX_TX_PRI ((sl_rail_util_coex_get_options() & SL_RAIL_UTIL_COEX_OPT_TX_HIPRI) ? SL_RAIL_UTIL_COEX_REQ_HIPRI : SL_RAIL_UTIL_COEX_REQ_OFF)
  #define SL_RAIL_UTIL_COEX_RX_PRI ((sl_rail_util_coex_get_options() & SL_RAIL_UTIL_COEX_OPT_RX_HIPRI) ? SL_RAIL_UTIL_COEX_REQ_HIPRI : SL_RAIL_UTIL_COEX_REQ_OFF)
  #define SL_RAIL_UTIL_COEX_RX_RETRY_PRI ((sl_rail_util_coex_get_options() & SL_RAIL_UTIL_COEX_OPT_RX_RETRY_HIPRI) ? SL_RAIL_UTIL_COEX_REQ_HIPRI : SL_RAIL_UTIL_COEX_REQ_OFF)

  #define sl_rail_util_coex_get_cca_counter_threshold() \
  ((sl_rail_util_coex_get_options() & SL_RAIL_UTIL_COEX_OPT_CCA_THRESHOLD) >> SL_RAIL_UTIL_COEX_OPT_SHIFT_CCA_THRESHOLD)
  #define sl_rail_util_coex_get_mac_retry_counter_threshold() \
  ((sl_rail_util_coex_get_options() & SL_RAIL_UTIL_COEX_OPT_MAC_RETRY_THRESHOLD) >> SL_RAIL_UTIL_COEX_OPT_SHIFT_MAC_RETRY_THRESHOLD)
  #define sl_rail_util_coex_get_mac_fail_counter_threshold() \
  ((sl_rail_util_coex_get_options() & SL_RAIL_UTIL_COEX_OPT_MAC_FAIL_THRESHOLD) >> SL_RAIL_UTIL_COEX_OPT_SHIFT_MAC_FAIL_THRESHOLD)

  #define sl_rail_util_coex_get_option_mask(mask, shift)       ((sl_rail_util_coex_get_options() & mask) >> shift)
  #define sl_rail_util_coex_set_option_mask(value, mask, shift) (sl_rail_util_coex_set_options((sl_rail_util_coex_get_options() & (~mask)) | ((((sl_rail_util_coex_options_t)value) << shift) & mask)))

  #define SL_RAIL_UTIL_COEX_OPT_MAC_AND_FORCE_HOLDOFFS (SL_RAIL_UTIL_COEX_OPT_MAC_HOLDOFF | SL_RAIL_UTIL_COEX_OPT_FORCE_HOLDOFF)

typedef uint32_t sl_rail_util_coex_options_t;

#include "coexistence/common/coexistence.h"

#define SL_RAIL_UTIL_COEX_GPIO_INDEX_RHO        0x00u // Radio holdfoff GPIO index
#define SL_RAIL_UTIL_COEX_GPIO_INDEX_REQ        0x01u // Request GPIO index
#define SL_RAIL_UTIL_COEX_GPIO_INDEX_GNT        0x02u // Grant GPIO index
#define SL_RAIL_UTIL_COEX_GPIO_INDEX_PHY_SELECT 0x03u // PHY Selext index

typedef uint8_t sl_rail_util_coex_gpio_index_t;

/**
 * @enum sl_rail_util_coex_event_t
 *
 * Events triggered by coexistence component
 *
 * These events are passed to sl_rail_util_coex_counter_on_event.
 */
typedef enum {
  /** Low priority request initiated */
  SL_RAIL_UTIL_COEX_EVENT_LO_PRI_REQUESTED,
  /** High priority request initiated */
  SL_RAIL_UTIL_COEX_EVENT_HI_PRI_REQUESTED,
  /** Low priority request denied */
  SL_RAIL_UTIL_COEX_EVENT_LO_PRI_DENIED,
  /** High priority request denied */
  SL_RAIL_UTIL_COEX_EVENT_HI_PRI_DENIED,
  /** Low priority transmission aborted mid packet */
  SL_RAIL_UTIL_COEX_EVENT_LO_PRI_TX_ABORTED,
  /** High priority transmission aborted mid packet */
  SL_RAIL_UTIL_COEX_EVENT_HI_PRI_TX_ABORTED,
  /** Number of coexistence events */
  SL_RAIL_UTIL_COEX_EVENT_COUNT,
} sl_rail_util_coex_event_t;

void sl_rail_util_coex_counter_on_event(sl_rail_util_coex_event_t event);

#define sl_rail_util_coex_pwm_args_t COEX_PwmArgs_t

// Release PTA request if long request not enabled or toggle request on macretransmit enabled
  #define  sl_rail_util_coex_get_tx_req_release() \
  (((SL_RAIL_UTIL_COEX_OPT_LONG_REQ | SL_RAIL_UTIL_COEX_OPT_TOGGLE_REQ_ON_MACRETRANSMIT) & sl_rail_util_coex_get_options()) != SL_RAIL_UTIL_COEX_OPT_LONG_REQ)
sl_rail_util_coex_options_t sl_rail_util_coex_get_options(void);

// Initialize coexistence
void sl_rail_util_coex_init(void);

sl_status_t sl_rail_util_coex_set_options(sl_rail_util_coex_options_t options);

sl_status_t sl_rail_util_coex_set_bool(sl_rail_util_coex_options_t option, bool value);

sl_status_t sl_rail_util_coex_set_enable(bool enabled);

bool sl_rail_util_coex_is_enabled(void);

sl_status_t sl_rail_util_coex_set_tx_request(sl_rail_util_coex_req_t ptaReq, sl_rail_util_coex_cb_t ptaCb);

sl_status_t sl_rail_util_coex_set_rx_request(sl_rail_util_coex_req_t ptaReq, sl_rail_util_coex_cb_t ptaCb);

sl_status_t sl_rail_util_coex_set_request_pwm(sl_rail_util_coex_req_t ptaReq,
                                              sl_rail_util_coex_cb_t ptaCb,
                                              uint8_t dutyCycle,
                                              uint8_t periodHalfMs);

const sl_rail_util_coex_pwm_args_t *sl_rail_util_coex_get_request_pwm_args(void);

// Which RX request options should be sent on frame detect
sl_rail_util_coex_req_t sl_rail_util_coex_frame_detect_req(void);

// Which RX request options should be sent on filter pass
sl_rail_util_coex_req_t sl_rail_util_coex_filter_pass_req(void);

// The Stack reports PTA events via this function
sl_rail_util_ieee802154_stack_status_t sl_rail_util_coex_on_event(
  sl_rail_util_ieee802154_stack_event_t stack_event,
  uint32_t supplement);

sl_status_t sl_rail_util_coex_set_directional_priority_pulse_width(uint8_t pulseWidthUs);
uint8_t sl_rail_util_coex_get_directional_priority_pulse_width(void);

#define SL_RAIL_UTIL_COEX_PHY_SELECT_TIMEOUT_MAX 255U

// Set PHY select timeout in milliseconds
// case 1. timeoutMs == 0 -> disable COEX optimized PHY
// case 2. 0 < timeoutMs < SL_RAIL_UTIL_COEX_PHY_SELECT_TIMEOUT_MAX -> disable COEX optimized PHY
//   if there is no WiFi activity for timeoutMs
// case 3. timeoutMs == SL_RAIL_UTIL_COEX_PHY_SELECT_TIMEOUT_MAX -> enable COEX optimize PHY
sl_status_t sl_rail_util_coex_set_phy_select_timeout(uint8_t timeoutMs);
uint8_t sl_rail_util_coex_get_phy_select_timeout(void);
// Return whether or not COEX optimized PHY is selected
bool sl_rail_util_coex_is_coex_phy_selected(void);

// Get the override input value of a GPIO
bool sl_rail_util_coex_get_gpio_input_override(sl_rail_util_coex_gpio_index_t gpioIndex);

// If SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT is enabled,
// the input value of a PTA is read from a virtual GPIO
// rather than the physical PTA GPIO
sl_status_t sl_rail_util_coex_set_gpio_input_override(sl_rail_util_coex_gpio_index_t gpioIndex, bool enabled);

// Return whether or not Radio Hold-Off is enabled
bool sl_rail_util_coex_get_radio_holdoff(void);

// Enable/disable Radio Hold-Off
sl_status_t sl_rail_util_coex_set_radio_holdoff(bool enabled);

// Tells whether 15.4 coex signal identifier is enabled
bool sl_rail_util_is_coex_signal_identifier_enabled(void);

// ieee802.15.4 coexistence event callback
void sli_rail_util_ieee802154_coex_on_event(COEX_Events_t events);

// Tells whether 15.4 coex signal identifier is enabled
bool sl_rail_util_is_coex_signal_identifier_enabled(void);

// ieee802.15.4 coexistence event filter
extern COEX_Events_t sli_rail_util_ieee802154_coex_event_filter;

#ifdef __cplusplus
}
#endif

#endif //__COEXISTENCE_802154_H__

/**@} END micro group
 */
