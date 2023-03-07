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

#ifdef __cplusplus
extern "C" {
#endif

/** @name Radio HoldOff Configuration Definitions
 *
 * The following are used to aid in the abstraction with Radio
 * HoldOff (RHO).  The microcontroller-specific sources use these
 * definitions so they are able to work across a variety of boards
 * which could have different connections.  The names and ports/pins
 * used below are intended to match with a schematic of the system to
 * provide the abstraction.
 *
 * The Radio HoldOff input GPIO is abstracted like BUTTON0/1.
 */
//@{

/**
 * @brief The GPIO configuration register for Radio HoldOff.
 */

/** @brief The following definitions are helpers for managing
 *  Radio HoldOff and should not be modified.
 */
#if     (SL_RAIL_UTIL_COEX_ENABLED && defined(SL_RAIL_UTIL_COEX_RHO_PORT))
// Initial bootup configuration is for Radio HoldOff
  #define halInternalInitRadioHoldOff() halSetRadioHoldOff(true)
#else//!(SL_RAIL_UTIL_COEX_ENABLED && defined(SL_RAIL_UTIL_COEX_RHO_PORT))
// Initial bootup configuration is for default
  #define halInternalInitRadioHoldOff() /* no-op */
#endif//(SL_RAIL_UTIL_COEX_ENABLED && defined(SL_RAIL_UTIL_COEX_RHO_PORT))

//@} //END OF RADIO HOLDOFF CONFIGURATION DEFINITIONS

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

#if     (SL_RAIL_UTIL_COEX_ENABLED && (defined(SL_RAIL_UTIL_COEX_REQ_PORT) || defined(SL_RAIL_UTIL_COEX_GNT_PORT)))
// Initial bootup configuration is to enable PTA
  #define halInternalInitPta() halPtaSetEnable(true)
#else//!(SL_RAIL_UTIL_COEX_ENABLED && (defined(SL_RAIL_UTIL_COEX_REQ_PORT) || defined(SL_RAIL_UTIL_COEX_GNT_PORT)))
// Initial bootup configuration is not to enable PTA
  #define halInternalInitPta() /* no-op */
#endif//(SL_RAIL_UTIL_COEX_ENABLED && (defined(SL_RAIL_UTIL_COEX_REQ_PORT) || defined(SL_RAIL_UTIL_COEX_GNT_PORT)))

//@} //END OF PTA CONFIGURATION DEFINITIONS

// halPta Public API:
//
// halPtaReq_t is a bitmask of features:
//     7     6     5     4     3     2     1     0
// +-----+-----+-----+-----+-----+-----+-----+-----+
// |   0 | no  | no  |wait |wait |force|hipri| req |
// |     | req |grant|grant| req |     |     |     |
// +-----+-----+-----+-----+-----+-----+-----+-----+
//
// This mask is used for both requests and callbacks to
// represent status.

  #define PTA_REQ_OFF         (COEX_REQ_OFF)         // Negate request
  #define PTA_REQ_ON          (COEX_REQ_ON)          // Assert request
  #define PTA_REQ_HIPRI       (COEX_REQ_HIPRI)       // Request is hi-pri
  #define PTA_REQ_FORCE       (COEX_REQ_FORCE)       // Force assertion immediately
  #define PTA_REQCB_REQUESTED (COEX_REQCB_REQUESTED) // Callback when REQUEST asserted
  #define PTA_REQCB_GRANTED   (COEX_REQCB_GRANTED)   // Callback when GRANT asserted
  #define PTA_REQCB_NEGATED   (COEX_REQCB_NEGATED)   // Callback when GRANT negated
  #define PTA_REQCB_OFF       (COEX_REQCB_OFF)       // Callback when REQUEST removed
  #define PTA_REQ_PWM         (COEX_REQ_PWM)         // PWM request

#define halPtaReq_t COEX_Req_t

#define halPtaCb_t COEX_ReqCb_t

//
// HalPtaOptions is a bitmask of features:
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

  #define PTA_OPT_DISABLED             0u          // Disable option
  #define PTA_OPT_RX_RETRY_TIMEOUT_MS (0xffu)      // Rx retry request timeout
  #define PTA_OPT_ACK_HOLDOFF         (1u << 8)    // Enable ack radio holdoff
  #define PTA_OPT_ABORT_TX            (1u << 9)    // Abort mid TX if grant is lost
  #define PTA_OPT_TX_HIPRI            (1u << 10)   // Tx request is hi-pri
  #define PTA_OPT_RX_HIPRI            (1u << 11)   // Rx request is hi-pri
  #define PTA_OPT_RX_RETRY_HIPRI      (1u << 12)   // Rx retry request is hi-pri
  #define PTA_OPT_RX_RETRY_REQ        (1u << 13)   // Request on corrupt packet
  #define PTA_OPT_RHO_ENABLED         (1u << 14)   // Enable RHO
  #define PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT \
  (1u << 15)                                       // Enable toggle on retransmit
  #define PTA_OPT_FORCE_HOLDOFF       (1u << 16)   // Force holdoff by disabling request
  #define PTA_OPT_MAC_HOLDOFF         (1u << 17)   // Hold off transmission in the mac layer
  #define PTA_OPT_REQ_FILTER_PASS     (1u << 18)   // Delay asserting request until address filtering passes
  #define PTA_OPT_HIPRI_FILTER_PASS   (1u << 19)   // Assert request with high priority after address filtering passes
  #define PTA_OPT_CCA_THRESHOLD       (0x7u << 20) // CCA failure counter threshold
  #define PTA_OPT_MAC_RETRY_THRESHOLD (0x3u << 23) // MAC retry counter threshold
  #define PTA_OPT_MAC_FAIL_THRESHOLD  (0x3u << 25) // MAC failure counter threshold
  #define PTA_OPT_LONG_REQ            (1u << 31)   // Hold request across CCA failures

  #define PTA_OPT_SHIFT_CCA_THRESHOLD         (20)
  #define PTA_OPT_SHIFT_MAC_RETRY_THRESHOLD   (23)
  #define PTA_OPT_SHIFT_MAC_FAIL_THRESHOLD (25)
  #define PTA_OPT_SHIFT_RX_RETRY_TIMEOUT_MS (0)
  #define PTA_TX_PRI ((halPtaGetOptions() & PTA_OPT_TX_HIPRI) ? PTA_REQ_HIPRI : PTA_REQ_OFF)
  #define PTA_RX_PRI ((halPtaGetOptions() & PTA_OPT_RX_HIPRI) ? PTA_REQ_HIPRI : PTA_REQ_OFF)
  #define PTA_RX_RETRY_PRI ((halPtaGetOptions() & PTA_OPT_RX_RETRY_HIPRI) ? PTA_REQ_HIPRI : PTA_REQ_OFF)

  #define halPtaGetCcaCounterThreshold() \
  ((halPtaGetOptions() & PTA_OPT_CCA_THRESHOLD) >> PTA_OPT_SHIFT_CCA_THRESHOLD)
  #define halPtaGetMacRetryCounterThreshold() \
  ((halPtaGetOptions() & PTA_OPT_MAC_RETRY_THRESHOLD) >> PTA_OPT_SHIFT_MAC_RETRY_THRESHOLD)
  #define halPtaGetMacFailCounterThreshold() \
  ((halPtaGetOptions() & PTA_OPT_MAC_FAIL_THRESHOLD) >> PTA_OPT_SHIFT_MAC_FAIL_THRESHOLD)

  #define halPtaGetOptionMask(mask, shift)       ((halPtaGetOptions() & mask) >> shift)
  #define halPtaSetOptionMask(value, mask, shift) (halPtaSetOptions((halPtaGetOptions() & (~mask)) | ((((HalPtaOptions)value) << shift) & mask)))

  #define PTA_OPT_MAC_AND_FORCE_HOLDOFFS (PTA_OPT_MAC_HOLDOFF | PTA_OPT_FORCE_HOLDOFF)

typedef uint32_t HalPtaOptions;

// PTA Events that the Stack can report via halPtaStackEvent():
//      STACK EVENT                     Value    supplement passed      returns
//      ------------------------------  -----    ---------------------  -------
#define PTA_STACK_EVENT_TICK            0x00u // MBZ                    SUCCESS
#define PTA_STACK_EVENT_RX_STARTED      0x01u // bool isReceivingFrame  SUCCESS
#define PTA_STACK_EVENT_RX_FILTERED     0x02u // bool isReceivingFrame  SUCCESS
#define PTA_STACK_EVENT_RX_ACCEPTED     0x03u // bool isReceivingFrame  SUCCESS
#define PTA_STACK_EVENT_RX_CORRUPTED    0x04u // bool isReceivingFrame  SUCCESS
#define PTA_STACK_EVENT_RX_ACKING       0x05u // bool isReceivingFrame  SUCCESS
#define PTA_STACK_EVENT_RX_ACK_BLOCKED  0x06u // bool isReceivingFrame  SUCCESS
#define PTA_STACK_EVENT_RX_ACK_ABORTED  0x07u // bool isReceivingFrame  SUCCESS
#define PTA_STACK_EVENT_RX_ACK_SENT     0x08u // bool isReceivingFrame  SUCCESS
#define PTA_STACK_EVENT_RX_ENDED        0x09u // bool isReceivingFrame  SUCCESS
#define PTA_STACK_EVENT_RX_IDLED        0x0Au // MBZ                    SUCCESS
#define PTA_STACK_EVENT_RX_LISTEN       0x0Bu // MBZ                    SUCCESS
#define PTA_STACK_EVENT_TX_PENDED_MAC   0x21u // halPtaCb_t cb          CB_PENDING/HOLDOFF/SUCCESS
#define PTA_STACK_EVENT_TX_PENDED_PHY   0x22u // bool isCcaTransmit     SUCCESS
#define PTA_STACK_EVENT_TX_CCA_SOON     0x23u // MBZ                    SUCCESS
#define PTA_STACK_EVENT_TX_CCA_BUSY     0x24u // bool isNextCcaImminent SUCCESS
#define PTA_STACK_EVENT_TX_STARTED      0x25u // MBZ                    SUCCESS
#define PTA_STACK_EVENT_TX_BLOCKED      0x26u // bool pktRequestedAck   SUCCESS
#define PTA_STACK_EVENT_TX_ABORTED      0x27u // bool pktRequestedAck   SUCCESS
#define PTA_STACK_EVENT_TX_ACK_WAITING  0x28u // MBZ                    SUCCESS
#define PTA_STACK_EVENT_TX_ACK_RECEIVED 0x29u // bool hasFramePending   SUCCESS
#define PTA_STACK_EVENT_TX_ACK_TIMEDOUT 0x2Au // uint8_t macRetries     SUCCESS
#define PTA_STACK_EVENT_TX_ENDED        0x2Bu // MBZ                    SUCCESS
#define PTA_STACK_EVENT_TX_IDLED        0x2Cu // MBZ                    SUCCESS

typedef uint8_t halPtaStackEvent_t;

#define PTA_STACK_STATUS_SUCCESS        0x00u // Normal success
#define PTA_STACK_STATUS_CB_PENDING     0x01u // Callback is pending
#define PTA_STACK_STATUS_HOLDOFF        0x02u // MAC Holdoff option in effect

typedef uint8_t halPtaStackStatus_t;

#include "coexistence/common/coexistence.h"

#define PTA_GPIO_INDEX_RHO        0x00u // Radio holdfoff GPIO index
#define PTA_GPIO_INDEX_REQ        0x01u // Request GPIO index
#define PTA_GPIO_INDEX_GNT        0x02u // Grant GPIO index
#define PTA_GPIO_INDEX_PHY_SELECT 0x03u // PHY Selext index

typedef uint8_t halPtaGpioIndex_t;

#define HalPtaPwmArgs_t COEX_PwmArgs_t

// Release PTA request if long request not enabled or toggle request on macretransmit enabled
  #define  halPtaGetTxReqRelease() \
  (((PTA_OPT_LONG_REQ | PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT) & halPtaGetOptions()) != PTA_OPT_LONG_REQ)
HalPtaOptions halPtaGetOptions(void);

EmberStatus halPtaSetOptions(HalPtaOptions options);

EmberStatus halPtaSetBool(HalPtaOptions option, bool value);

EmberStatus halPtaSetEnable(bool enabled);

bool halPtaIsEnabled(void);

EmberStatus halPtaSetTxRequest(halPtaReq_t ptaReq, halPtaCb_t ptaCb);

EmberStatus halPtaSetRxRequest(halPtaReq_t ptaReq, halPtaCb_t ptaCb);

EmberStatus halPtaSetRequestPwm(halPtaReq_t ptaReq,
                                halPtaCb_t ptaCb,
                                uint8_t dutyCycle,
                                uint8_t periodHalfMs);

const HalPtaPwmArgs_t *halPtaGetRequestPwmArgs(void);

// Which RX request options should be sent on frame detect
halPtaReq_t halPtaFrameDetectReq(void);

// Which RX request options should be sent on filter pass
halPtaReq_t halPtaFilterPassReq(void);

// The Stack reports PTA events via this function
halPtaStackStatus_t halPtaStackEvent(halPtaStackEvent_t ptaStackEvent,
                                     uint32_t supplement);

EmberStatus halPtaSetDirectionalPriorityPulseWidth(uint8_t pulseWidthUs);
uint8_t halPtaGetDirectionalPriorityPulseWidth(void);

#define PTA_PHY_SELECT_TIMEOUT_MAX 255U

// Set PHY select timeout in milliseconds
// case 1. timeoutMs == 0 -> disable COEX optimized PHY
// case 2. 0 < timeoutMs < PTA_PHY_SELECT_TIMEOUT_MAX -> disable COEX optimized PHY
//   if there is no WiFi activity for timeoutMs
// case 3. timeoutMs == PTA_PHY_SELECT_TIMEOUT_MAX -> enable COEX optimize PHY
EmberStatus halPtaSetPhySelectTimeout(uint8_t timeoutMs);
uint8_t halPtaGetPhySelectTimeout(void);

// Get the override input value of a GPIO
bool halPtaGetGpioInputOverride(halPtaGpioIndex_t gpioIndex);

// If SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT is enabled,
// the input value of a PTA is read from a virtual GPIO
// rather than the physical PTA GPIO
EmberStatus halPtaSetGpioInputOverride(halPtaGpioIndex_t gpioIndex, bool enabled);

#ifdef __cplusplus
}
#endif

#endif //__COEXISTENCE_802154_H__

/**@} END micro group
 */
