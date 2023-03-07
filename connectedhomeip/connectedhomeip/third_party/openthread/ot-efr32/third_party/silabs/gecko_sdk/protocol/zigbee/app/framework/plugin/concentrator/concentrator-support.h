/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Concentrator plugin.
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
 * @defgroup concentrator Concentrator Support
 * @ingroup component
 * @brief API and Callbacks for the Concentrator Support Component
 *
 * Code that handles periodically broadcasting a many-to-one-route-request
 * (MTORR) so that the device will act as a concentrator (sink) in the network.
 *  - Note: NCP/SOC source route table size can be now defined as part
 * of the source routing library.
 *
 */

/**
 * @addtogroup concentrator
 * @{
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS
extern uint8_t emAfRouteErrorCount;
extern uint8_t emAfDeliveryFailureCount;
#endif

extern EmberEventControl emberAfPluginConcentratorUpdateEventControl;

#define LOW_RAM_CONCENTRATOR  EMBER_LOW_RAM_CONCENTRATOR
#define HIGH_RAM_CONCENTRATOR EMBER_HIGH_RAM_CONCENTRATOR

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define emAfConcentratorStartDiscovery emberAfPluginConcentratorQueueDiscovery
void emAfConcentratorStopDiscovery(void);
#endif

/**
 * @name API
 * @{
 */

/** @brief Concentrator source route discovery
 *
 * @return uint32_t
 *
 */
uint32_t emberAfPluginConcentratorQueueDiscovery(void);

/** @brief Concentrator stop source route discover
 *
 *
 */
void emberAfPluginConcentratorStopDiscovery(void);

/** @} */ // end of API

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// These values are defined by appbuilder.
// TODO: these can be removed (and the enum below should be update accordingly
// once we drop AppBuilder support)
#define NONE (0x00)
#define FULL (0x01)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberAfPluginConcentratorRouterBehavior
#else
enum
#endif //DOXYGEN_SHOULD_SKIP_THIS
{
  EMBER_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_NONE = NONE,
  EMBER_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_FULL = FULL,
  EMBER_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_MAX = EMBER_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_FULL,
};
typedef uint8_t EmberAfPluginConcentratorRouterBehavior;

extern EmberAfPluginConcentratorRouterBehavior emAfPluginConcentratorRouterBehaviors[];
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define emAfPluginConcentratorRouterBehavior emAfPluginConcentratorRouterBehaviors[emberGetCurrentNetwork()]
#endif
#define emberAfPluginConcentratorGetRouterBehavior() \
  (emAfPluginConcentratorRouterBehavior)
#define emberAfPluginConcentratorSetRouterBehavior(behavior) \
  do { emAfPluginConcentratorRouterBehavior = behavior; } while (0);

/** @} */ // end of concentrator
