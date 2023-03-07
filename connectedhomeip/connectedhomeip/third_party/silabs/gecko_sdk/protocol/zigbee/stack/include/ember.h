/***************************************************************************//**
 * @file
 * @brief The master include file for the EmberZNet API.
 *
 *  See @ref ember for documentation.
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

/**
 * @addtogroup ember EmberZNet Stack API Reference
 * This documentation describes the application programming interface (API)
 * for the EmberZNet stack.
 * The file ember.h is the master include file for the EmberZNet API modules.
 */

#ifndef SILABS_EMBER_H
#define SILABS_EMBER_H

#if (!defined(EZSP_HOST) && !defined(EMBER_TEST))
#include "em_device.h"  // To get _SILICON_LABS macros
#endif  // !EZSP_HOST && !EMBER_TEST

#ifndef EMBER_TEST
#define EVENT_NAME(x) ""
#else
#define EVENT_NAME(x) x
#endif

// We define EMBER_MULTI_NETWORK_STRIPPED before including header files
// so that the EMBER_MULTI_NETWORK_STRIPPED can be well aware.
#if defined(SMART_ENERGY_STACK_TOKENS) && !defined(EMBER_MULTI_NETWORK_STRIPPED)
#define EMBER_MULTI_NETWORK_STRIPPED
#endif

// EMBER_MULTI_NETWORK_STRIPPED is an optional #define
// that is used to conditionally compile out multi-network
// related source code. The following flash-space constrained chips
// have EMBER_MULTI_NETWORK_STRIPPED by default.
// It is possible for one to reenable the multi-network code by defining
// EMBER_MULTI_NETWORK_REENABLE when building libraries. There are no known
// consumers of these type of libraries right now
#ifndef EMBER_MULTI_NETWORK_STRIPPED
#if defined(CORTEXM3_EM351) || defined(CORTEXM3_EM357) || defined(_SILICON_LABS_32B_SERIES_1_CONFIG_1) || defined(_SILICON_LABS_32B_SERIES_1_CONFIG_4)
#ifndef EMBER_MULTI_NETWORK_REENABLE
#define EMBER_MULTI_NETWORK_STRIPPED
#endif
#endif
#endif // EMBER_MULTI_NETWORK_STRIPPED

#include "ember-types.h"
#include "byte-utilities.h"
#include "stack-info.h"
#include "network-formation.h"
#include "buffer_manager/buffer-management.h"
#include "buffer_manager/buffer-queue.h"
#include "buffer_manager/legacy-packet-buffer.h"
#include "message.h"
#include "child.h"
#include "security.h"
#include "aes-mmo.h"
#include "binding-table.h"
#include "bootload.h"
#include "zigbee-device-stack.h"
#ifndef UC_BUILD
#include "event_control/event.h"
#endif
#include "event_queue/event-queue.h"
#include "ember-debug.h"
#include "library.h"
#include "zll-api.h"
#include "multi-phy.h"
#include "ember-duty-cycle.h"
#include "mac-layer.h"
#include "ember-random-api.h"
#include "ember-alternate-mac.h"
#include "raw-message.h"
#include "multi-network.h"

/** @name PHY Information
 * Bit masks for TOKEN_MFG_RADIO_BANDS_SUPPORTED.
 */
//@{
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define RADIO_BANDS_SUPPORTED_868   BIT(0)
#define RADIO_BANDS_SUPPORTED_915   BIT(1)
#define RADIO_BANDS_SUPPORTED_433   BIT(2)
#endif // DOXYGEN_SHOULD_SKIP_THIS

/** @brief 2.4 GHz band */
#define RADIO_BANDS_SUPPORTED_2400  BIT(3)

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define RADIO_BANDS_SUPPORTED_408   BIT(4)
#endif // DOXYGEN_SHOULD_SKIP_THIS

//@} //END PHY INFO

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/** @name Stack Build Configuration Settings */
//@{
#include "config/config.h"
//@} //END STACK BUILD CONFIG SETTINGS

#ifdef DEBUG_ASSERT
extern bool enableFailure;
extern uint8_t rateOfFailure;
extern uint8_t failuresInARow;
static uint8_t bufferFailure;
bool generateFailure(void);
void dumpFailure(void);
#endif

#endif //DOXYGEN_SHOULD_SKIP_THIS

/**
 * <!-- HIDDEN
 * @page 2p5_to_3p0
 * <hr>
 * Many functions have been moved from ember.h to the following files:
 * - binding-table.h
 * - bootload.h
 * - child.h
 * - network-formation.h
 * - stack-info.h
 * - trust-center.h
 *
 * Changes include:
 * <ul>
 * <li> <b>New items</b>
 * <li> <b>Changed items</b>
 *   - emberJoinNetwork()
 *   .
 * <li> <b>Removed items</b>
 *   - emberCloseConnection()
 *   - emberConnectionStatus()
 *   - emberConnectionStatusHandler()
 *   - emberCreateAggregationRoutes()
 *   - emberGetBindingDestinationNodeId()
 *   - emberGetCachedDescription()
 *   - emberIncomingRawMessageHandler()
 *   - emberIncomingSpdoMessageHandler()
 *   - emberMaximumTransportPayloadLength()
 *   - emberMobileNodeHasMoved()
 *   - emberOpenConnection()
 *   - emberSendDatagram() - now use ::emberSendUnicast().
 *   - emberSendDiscoveryInformationToParent()
 *   - emberSendLimitedMulticast()
 *   - emberSendSequenced()
 *   - emberSendSpdoDatagramToParent()
 *   - emberSetBindingDestinationNodeId()
 *   - emberSetEncryptionKey()
 *   - emberSpdoUnicastSent()
 *   - emberUnicastSent()
 *   .
 * </ul>
 * HIDDEN -->
 */

#endif // SILABS_EMBER_H
