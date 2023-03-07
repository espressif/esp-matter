/***************************************************************************//**
 * @file
 * @brief Definitions for the ZLL On Off Server plugin.
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

#ifndef SILABS_ZIGBEE_ZLL_ON_OFF_SERVER_H
#define SILABS_ZIGBEE_ZLL_ON_OFF_SERVER_H

/**
 * @defgroup zll-on-off-server ZLL On/Off Server
 * @ingroup component cluster
 * @brief API and Callbacks for the ZLL On/Off Cluster Server Component
 *
 * Silicon Labs implementation of the original Zigbee Light Link profile
 * enhancements to the On/Off server cluster. The component is only intended
 * to assist with debugging, as the implemented callbacks simply parse requests
 * from the Identify client and blink LEDs. Before production, this component
 * should be replaced with code that acts on the requests in an appropriate way.
 * Note that the extensions do not now require Touchlink commissioning
 * to be used, and that the commands may now be required by other device types, such as
 * Lighting and Occupancy.
 *
 */

/**
 * @addtogroup zll-on-off-server
 * @{
 */

/**
 * @name API
 * @{
 */

/** @brief Turn off zll extensions
 *
 * @param cmd pointer to the cluster command struct Ver.: always
 *
 * @return EmberAfStatus return status
 *
 */
EmberAfStatus emberAfPluginZllOnOffServerOffZllExtensions(const EmberAfClusterCommand *cmd);

/** @brief Turn on zll extensions
 *
 * @param cmd pointer to the cluster command struct Ver.: always
 *
 * @return EmberAfStatus return status
 *
 */
EmberAfStatus emberAfPluginZllOnOffServerOnZllExtensions(const EmberAfClusterCommand *cmd);

/** @brief Toggle zll extensions
 *
 * @param cmd pointer to the cluster command struct Ver.: always
 *
 * @return EmberAfStatus return status
 *
 */
EmberAfStatus emberAfPluginZllOnOffServerToggleZllExtensions(const EmberAfClusterCommand *cmd);

/** @brief Level control cluster command for ZLL extentions
 *
 * @param endpoint Ver.: always
 *
 * @return EmberAfStatus return status
 *
 */
EmberAfStatus emberAfPluginZllOnOffServerLevelControlZllExtensions(uint8_t endpoint);

/** @} */ // end of name API

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup zll_on_off_server_cb ZLL On Off Server
 * @ingroup af_callback
 * @brief Callbacks for ZLL On Off Server Component
 *
 */

/**
 * @addtogroup zll_on_off_server_cb
 * @{
 */

/** @brief Off with effect.
 *
 * This callback is called by the ZLL On/Off Server plugin whenever an
 * OffWithEffect command is received. The application should implement the
 * effect and variant requested in the command and return
 * ::EMBER_ZCL_STATUS_SUCCESS if successful or an appropriate error status
 * otherwise.
 *
 * @param endpoint   Ver.: always
 * @param effectId   Ver.: always
 * @param effectVariant   Ver.: always
 *
 * @return EmberAfStatus return status
 */
EmberAfStatus emberAfPluginZllOnOffServerOffWithEffectCallback(uint8_t endpoint,
                                                               uint8_t effectId,
                                                               uint8_t effectVariant);

/** @} */ // end of zll_on_off_server_cb
/** @} */ // end of name Callbacks
/** @} */ // end of zll-on-off-server

#endif // SILABS_ZIGBEE_ZLL_ON_OFF_SERVER_H
