/***************************************************************************//**
 * @file
 * @brief Initiator APIs and defines for the Find and Bind Initiator plugin.
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

#ifndef SILABS_FIND_AND_BIND_INITIATOR_H
#define SILABS_FIND_AND_BIND_INITIATOR_H

/**
 * @defgroup find-and-bind-initiator Find and Bind Initiator
 * @ingroup component
 * @brief API and Callbacks for the Find and Bind Initiator Component
 *
 * This component  provides the functionality to find devices that are
 * performing the find and bind process for a target and then optionally
 * attempts to create a binding to those devices.
 *
 */

/**
 * @addtogroup find-and-bind-initiator
 * @{
 */

// -----------------------------------------------------------------------------
// Constants

#define EMBER_AF_PLUGIN_FIND_AND_BIND_INITIATOR_PLUGIN_NAME "Find and Bind Initiator"

// -----------------------------------------------------------------------------
// API

/**
 * @name API
 * @{
 */

/** @brief Start initiator finding and binding operations.
 *
 * A call to this function will commence the initiator finding and
 * binding operations. Specifically, the initiator will attempt to start
 * searching for potential bindings that can be made with identifying
 * targets.
 *
 * @param endpoint The endpoint on which to begin initiator operations.
 *
 * @returns An ::EmberStatus value describing the success of the
 * commencement of the initiator operations.
 */
EmberStatus emberAfPluginFindAndBindInitiatorStart(uint8_t endpoint);

/** @} */ // end of name API

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup find_bind_initiator_cb Find and Bind Initiator
 * @ingroup af_callback
 * @brief Callbacks for Find and Bind Initiator Component
 *
 */

/**
 * @addtogroup find_bind_initiator_cb
 * @{
 */

/** @brief Bind Target
 *
 * This callback with enable the user to programmatically decide if they want
 * to bind with a potential target. The plugin will try to bind with this
 * target if and only if the function returns true. By default, the callback
 * tells the plugin to try to bind with the target. If the binding type is
 * changed to ::EMBER_MULTICAST_BINDING, a multicast binding will be created.
 *
 * @param nodeId short ID of the potential target Ver.: always
 * @param bindingEntry The binding entry for that target. Ver.: always
 * @param groupName The name of the group if a multicast binding is created.
 * Ver.: always
 */
bool emberAfPluginFindAndBindInitiatorBindTargetCallback(EmberNodeId nodeId,
                                                         EmberBindingTableEntry *bindingEntry,
                                                         uint8_t *groupName);

/** @brief Find and Bind Complete
 *
 * This callback is fired by the initiator when the Find and Bind process is
 * complete.
 *
 * @param status Status code describing the completion of the find and bind
 * process Ver.: always
 */
void emberAfPluginFindAndBindInitiatorCompleteCallback(EmberStatus status);
/** @} */ // end of find_bind_initiator_cb
/** @} */ // end of name Callbacks
/** @} */ // end of find-and-bind-initiator

#endif /* __FIND_AND_BIND_INITIATOR_H__ */
