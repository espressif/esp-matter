/***************************************************************************//**
 * @file
 * @brief Routines for the Groups Server plugin, the server implementation of
 *        the Groups cluster.
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

#ifndef SILABS_ZIGBEE_GROUPS_SERVER_H
#define SILABS_ZIGBEE_GROUPS_SERVER_H

/**
 * @defgroup groups-server Groups Server
 * @ingroup component cluster
 * @brief API and Callbacks for the Groups Cluster Server Component
 *
 * Silicon Labs implementation of Groups server cluster.
 * This component supports receiving commands to add, retrieve, or modify
 * the APS multicast group membership. Each group requires a binding
 * table entry, so the binding table should be large enough to
 * accommodate groups as well as any other bindings created during normal operation.
 *
 */

/**
 * @addtogroup groups-server
 * @{
 */

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup groups_server_cb Groups Server
 * @ingroup af_callback
 * @brief Callbacks for Groups Server Component
 *
 */

/**
 * @addtogroup groups_server_cb
 * @{
 */

/** @brief Get a group name.
 *
 * This function returns the name of a group with the provided group ID, if
 * it exists.
 *
 * @param endpoint Endpoint Ver.: always
 * @param groupId Group ID Ver.: always
 * @param groupName Group Name Ver.: always
 */
void emberAfPluginGroupsServerGetGroupNameCallback(uint8_t endpoint,
                                                   uint16_t groupId,
                                                   uint8_t *groupName);

/** @brief Set a group name.
 *
 * This function sets the name of a group with the provided group ID.
 *
 * @param endpoint Endpoint Ver.: always
 * @param groupId Group ID Ver.: always
 * @param groupName Group Name Ver.: always
 */
void emberAfPluginGroupsServerSetGroupNameCallback(uint8_t endpoint,
                                                   uint16_t groupId,
                                                   uint8_t *groupName);

/** @brief Return supported group names.
 *
 * This function returns whether or not group names are supported.
 *
 * @param endpoint Endpoint Ver.: always
 */
bool emberAfPluginGroupsServerGroupNamesSupportedCallback(uint8_t endpoint);

/** @brief Clear the group table.
 *
 * This function is called by the framework when the application should clear
 * the group table.
 *
 * @param endpoint The endpoint.  Ver.: always
 */
void emberAfGroupsClusterClearGroupTableCallback(uint8_t endpoint);

/** @} */ // end of groups_server_cb
/** @} */ // end of name Callbacks
/** @} */ // end of groups-server

#endif // SILABS_ZIGBEE_GROUPS_SERVER_H
