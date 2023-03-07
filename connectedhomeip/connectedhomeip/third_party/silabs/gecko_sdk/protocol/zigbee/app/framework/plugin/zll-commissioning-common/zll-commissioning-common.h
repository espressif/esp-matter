/***************************************************************************//**
 * @file
 * @brief Definitions for the ZLL Commissioning Common plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef __ZLL_COMMISIONING_COMMON_H__
#define __ZLL_COMMISIONING_COMMON_H__

#ifdef UC_BUILD
#include "zll-commissioning-common-config.h"
#if (EMBER_AF_PLUGIN_ZLL_COMMISSIONING_COMMON_SCAN_SECONDARY_CHANNELS == 1)
#define SCAN_SECONDARY_CHANNELS
#endif
#if (EMBER_AF_PLUGIN_ZLL_COMMISSIONING_COMMON_ZIGBEE3_SUPPORT == 1)
#define ZIGBEE3_SUPPORT
#endif
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_COMMON_SCAN_SECONDARY_CHANNELS
#define SCAN_SECONDARY_CHANNELS
#endif
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_COMMON_ZIGBEE3_SUPPORT
#define ZIGBEE3_SUPPORT
#endif
#endif // UC_BUILD

/**
 * @defgroup zll-commissioning-common ZLL Commissioning Common
 * @ingroup component
 * @brief API and Callbacks for the ZLL Commissioning Common Component
 *
 * Common code for the ZigBee 3.0 touchlink commissioning mechanism.
 * This component is required for both server and client side implementations.
 *
 */

/**
 * @addtogroup zll-commissioning-common
 * @{
 */

/**
 * @name API
 * @{
 */

/** @brief Generate a random network key and initializes the security state of
 * the device.
 *
 * This function is a convenience wrapper for ::emberZllSetInitialSecurityState,
 * which must be called before starting or joining a network. The plugin
 * initializes the security state for the initiator during touch linking. The
 * target must initialize its own security state prior to forming a network
 * either by using this function or by calling ::emberZllSetInitialSecurityState
 * directly.
 *
 * @return An ::EmberStatus value that indicates the success or failure of the
 * command.
 */
EmberStatus emberAfZllSetInitialSecurityState(void);

/** @brief Indicate whether a touch link procedure is currently in progress.
 *
 * @return True if a touch link is in progress or false otherwise.
 */
bool emberAfZllTouchLinkInProgress(void);

/** @brief Reset the local device to a factory new state.
 *
 * This function causes the device to leave the network and clear its
 * network parameters, resets its attributes to their default values, and clears
 * the group and scene tables.
 */
void emberAfZllResetToFactoryNew(void);

/** @brief Disable touchlink processing.
 *
 * This function will cause the device to refuse network start/join
 * requests if it receives them and will not allow touchlinking.
 * Note that this will have the effect of overriding the
 * emberAfZllNoTouchlinkForNFN function.
 */
EmberStatus emberAfZllDisable(void);

/** @brief Enable touchlink processing.
 *
 * This function will cause the device to accept network start/join
 * requests if it receives them and will not allow touchlinking.
 * Note that this will have the effect of overriding the
 * emberAfZllNoTouchlinkForNFN function.
 */
EmberStatus emberAfZllEnable(void);

/** @} */ // end of name API

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup zll_commissioning_common_cb Zll Commissioning Common
 * @ingroup af_callback
 * @brief Callbacks for Zll Commissioning Common Component
 *
 */

/**
 * @addtogroup zll_commissioning_common_cb
 * @{
 */

/** @brief Initial security state.
 *
 * This function is called by the ZLL Commissioning Common plugin to determine the
 * initial security state to be used by the device. The application must
 * populate the ::EmberZllInitialSecurityState structure with a configuration
 * appropriate for the network being formed, joined, or started. After the
 * device forms, joins, or starts a network, the same security configuration
 * will remain in place until the device leaves the network.
 *
 * @param securityState The security configuration to be populated by the
 * application and ultimately set in the stack. Ver.: always
 */
void emberAfPluginZllCommissioningCommonInitialSecurityStateCallback(EmberZllInitialSecurityState *securityState);

/** @brief Touch link complete.
 *
 * This function is called by the ZLL Commissioning Common plugin when touch linking
 * completes.
 *
 * @param networkInfo The ZigBee and ZLL-specific information about the network
 * and target. Ver.: always
 * @param deviceInformationRecordCount The number of sub-device information
 * records for the target. Ver.: always
 * @param deviceInformationRecordList The list of sub-device information
 * records for the target. Ver.: always
 */
void emberAfPluginZllCommissioningCommonTouchLinkCompleteCallback(const EmberZllNetwork *networkInfo,
                                                                  uint8_t deviceInformationRecordCount,
                                                                  const EmberZllDeviceInfoRecord *deviceInformationRecordList);

/** @brief Reset to factory new.
 *
 * This function is called by the ZLL Commissioning Common plugin when a request to
 * reset to factory new is received. The plugin will leave the network, reset
 * attributes managed by the framework to their default values, and clear the
 * group and scene tables. The application should perform any other necessary
 * reset-related operations in this callback, including resetting any
 * externally-stored attributes.
 *
 */
void emberAfPluginZllCommissioningCommonResetToFactoryNewCallback(void);

/** @} */ // end of zll_commissioning_common_cb
/** @} */ // end of name Callbacks

// Global data for all ZLL commissioning plugins.
extern EmberZllNetwork emAfZllNetwork;
extern uint16_t emAfZllFlags;
extern uint8_t emAfInitiatorRejoinRetryCount;

#ifdef UC_BUILD
#include "sl_component_catalog.h"
#endif

#if (defined(SL_CATALOG_ZIGBEE_ZLL_COMMISSIONING_CLIENT_PRESENT) || defined(EMBER_AF_PLUGIN_ZLL_COMMISSIONING_CLIENT))
// Sub-device info (mainly for client, but server needs to initialize the count).
extern EmberZllDeviceInfoRecord emAfZllSubDevices[];
extern uint8_t emAfZllSubDeviceCount;

// The identify duration may be updated by CLI command.
extern uint16_t emAfZllIdentifyDurationSec;
#endif

// State bits for client and server.
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum StateEnum
#else
enum
#endif
{
  INITIAL                     = 0x0000,
  SCAN_FOR_TOUCH_LINK         = 0x0001,
  SCAN_FOR_DEVICE_INFORMATION = 0x0002,
  SCAN_FOR_IDENTIFY           = 0x0004,
  SCAN_FOR_RESET              = 0x0008,
  TARGET_NETWORK_FOUND        = 0x0010,
  ABORTING_TOUCH_LINK         = 0x0020,
  SCAN_COMPLETE               = 0x0040,
  TOUCH_LINK_TARGET           = 0x0080,
  FORMING_NETWORK             = 0x0100,
  RESETTING_TO_FACTORY_NEW    = 0x0200,
};

#define touchLinkInProgress()      (emAfZllFlags                     \
                                    & (SCAN_FOR_TOUCH_LINK           \
                                       | SCAN_FOR_DEVICE_INFORMATION \
                                       | SCAN_FOR_IDENTIFY           \
                                       | SCAN_FOR_RESET              \
                                       | TOUCH_LINK_TARGET))
#define scanForTouchLink()         (emAfZllFlags & SCAN_FOR_TOUCH_LINK)
#define scanForDeviceInformation() (emAfZllFlags & SCAN_FOR_DEVICE_INFORMATION)
#define scanForIdentify()          (emAfZllFlags & SCAN_FOR_IDENTIFY)
#define scanForReset()             (emAfZllFlags & SCAN_FOR_RESET)
#define targetNetworkFound()       (emAfZllFlags & TARGET_NETWORK_FOUND)
#define abortingTouchLink()        (emAfZllFlags & ABORTING_TOUCH_LINK)
#define scanComplete()             (emAfZllFlags & SCAN_COMPLETE)
#define touchLinkTarget()          (emAfZllFlags & TOUCH_LINK_TARGET)
#define formingNetwork()           (emAfZllFlags & FORMING_NETWORK)
#define resettingToFactoryNew()    (emAfZllFlags & RESETTING_TO_FACTORY_NEW)

// The bits for cluster-specific command (0) and disable default response (4)
// are always set.  The direction bit (3) is only set for server-to-client
// commands (i.e., DeviceInformationResponse).  Some Philips devices still use
// the old frame format and set the frame control to zero.
#define ZLL_FRAME_CONTROL_LEGACY           0x00
#define ZLL_FRAME_CONTROL_CLIENT_TO_SERVER 0x11
#define ZLL_FRAME_CONTROL_SERVER_TO_CLIENT 0x19

#define ZLL_HEADER_FRAME_CONTROL_OFFSET   0 // one byte
#define ZLL_HEADER_SEQUENCE_NUMBER_OFFSET 1 // one byte
#define ZLL_HEADER_COMMAND_ID_OFFSET      2 // one byte
#define ZLL_HEADER_TRANSACTION_ID_OFFSET  3 // four bytes
#define ZLL_HEADER_OVERHEAD               7

// Uncomment the next line for extra debugs!
// #define PLUGIN_DEBUG
#if defined(PLUGIN_DEBUG)
  #define debugPrintln(...) emberAfCorePrintln(__VA_ARGS__)
  #define debugPrint(...)   emberAfCorePrint(__VA_ARGS__)
  #define debugExec(x) do { x; } while (0)
#else
  #define debugPrintln(...)
  #define debugPrint(...)
  #define debugExec(x)
#endif

// For legacy code
#define emberAfPluginZllCommissioningInitialSecurityStateCallback \
  emberAfPluginZllCommissioningCommonInitialSecurityStateCallback
#define emberAfPluginZllCommissioningTouchLinkCompleteCallback \
  emberAfPluginZllCommissioningCommonTouchLinkCompleteCallback
#define emberAfPluginZllCommissioningResetToFactoryNewCallback \
  emberAfPluginZllCommissioningCommonResetToFactoryNewCallback

/** @} */ // end of zll-commissioning-common

#endif // __ZLL_COMMISIONING_COMMON_H__
