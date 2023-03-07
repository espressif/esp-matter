/***************************************************************************//**
 * @file
 * @brief Definitions for the ZLL Scenes Server plugin.
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
 * @defgroup zll-scenes-server ZLL Scenes Server
 * @ingroup component cluster
 * @brief API and Callbacks for the ZLL Scenes Cluster Server Component
 *
 * Silicon Labs implementation of the original Zigbee Light Link profile
 * enhancements to the Scenes server cluster. The component is only intended to
 * help with debugging, as the implemented callbacks simply parse requests
 * from the Identify client and blink LEDs. Before production,
 * this component should be replaced with code that acts on the requests
 * in an appropriate way. Note that the extensions do not now require
 * Touchlink commissioning to be used, and that the commands may now be
 * required by other device types, such as Lighting and Occupancy.
 *
 */

/**
 * @addtogroup zll-scenes-server
 * @{
 */

/**
 * @name API
 * @{
 */

EmberAfStatus emberAfPluginZllScenesServerRecallSceneZllExtensions(uint8_t endpoint);

/** @} */ // end of name API
/** @} */ // end of comp-name
