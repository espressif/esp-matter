/***************************************************************************//**
 * @file
 * @brief Definitions for the ZLL Level Control Server plugin.
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
 * @defgroup zll-level-control-server ZLL Level Control Server
 * @ingroup component cluster
 * @brief API and Callbacks for the ZLL Level Control Cluster Server Component
 *
 * Silicon Labs implementation of the original Zigbee Light Link profile
 * enhancements to the Level Control server cluster.  The component is only
 * intended to assist with debugging, as the implemented callbacks simply
 * parse requests from the Identify client and blink LEDs.
 * Before production, this component should be replaced with code
 * that acts on the requests in an appropriate way. Note that the
 * extensions do not now require Touchlink commissioning to be used,
 * and that the commands may now be required by other device types,
 * such as Lighting and Occupancy.
 *
 */

/**
 * @addtogroup zll-level-control-server
 * @{
 */

#define EMBER_AF_PLUGIN_ZLL_LEVEL_CONTROL_SERVER_MINIMUM_LEVEL 0x01
#define EMBER_AF_PLUGIN_ZLL_LEVEL_CONTROL_SERVER_MAXIMUM_LEVEL 0xFE

/**
 * @name API
 * @{
 */

EmberAfStatus emberAfPluginZllLevelControlServerMoveToLevelWithOnOffZllExtensions(const EmberAfClusterCommand *cmd);
bool emberAfPluginZllLevelControlServerIgnoreMoveToLevelMoveStepStop(uint8_t endpoint, uint8_t commandId);

/** @} */ // end of name API
/** @} */ // end of zll-level-control-server
