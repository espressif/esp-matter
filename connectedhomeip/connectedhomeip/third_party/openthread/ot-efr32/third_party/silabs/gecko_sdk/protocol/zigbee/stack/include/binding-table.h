/***************************************************************************//**
 * @file
 * @brief See @ref binding_table for documentation.
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

#ifndef SILABS_BINDING_TABLE_H
#define SILABS_BINDING_TABLE_H

/**
 * @addtogroup binding_table
 * @brief EmberZNet binding table API.
 *
 * See binding-table.h for source code.
 * @{
 */

/** @brief Set an entry in the binding table by copying the structure
 * pointed to by \c value into the binding table.
 * @note You do not need to reserve memory for \c value.
 *
 * @param index  The index of a binding table entry.
 *
 * @param value  A pointer to a structure.
 *
 * @return An ::EmberStatus value that indicates the success
 * or failure of the command.
 */
EmberStatus emberSetBinding(uint8_t index, EmberBindingTableEntry *value);

/** @brief Copy a binding table entry to the structure that the
 * \c result points to.
 *
 * @param index  The index of a binding table entry.
 *
 * @param result A pointer to the location to which to copy the binding
 * table entry.
 *
 * @return An ::EmberStatus value that indicates the success
 * or failure of the command.
 */
EmberStatus emberGetBinding(uint8_t index, EmberBindingTableEntry *result);

/** @brief Delete a binding table entry.
 *
 * @param index  The index of a binding table entry.
 *
 * @return An ::EmberStatus value that indicates the success
 * or failure of the command.
 */
EmberStatus emberDeleteBinding(uint8_t index);

/** @brief Indicate whether any messages are currently being sent using
 * this binding table entry.
 *
 * Note that this function does not indicate whether a binding is clear.
 * To determine whether a binding is clear, check the
 * ::EmberBindingTableEntry structure that defines the binding.
 * The type field should have the value ::EMBER_UNUSED_BINDING.
 *
 * @param index  The index of a binding table entry.
 *
 * @return True if the binding table entry is active, false otherwise.
 */
bool emberBindingIsActive(uint8_t index);

/** @brief Return the node ID for the binding's destination if the ID
 * is known.
 *
 * If a message is sent using the binding and the destination's
 * ID is not known, the stack will discover the ID by broadcasting a ZDO
 * address request. The application can avoid the need for this discovery
 * by calling ::emberNoteSendersBinding() whenever a message arrives from
 * the binding's destination, or by calling ::emberSetBindingRemoteNodeId()
 * when it knows the correct ID via some other means, such as having saved
 * it in nonvolatile memory.
 *
 * The destination's node ID is forgotten when the binding is changed,
 * when the local node reboots
 * or, much more rarely, when the destination node changes its ID in response
 * to an ID conflict.
 *
 * @param index  The index of a binding table entry.
 *
 * @return The short ID of the destination node or ::EMBER_NULL_NODE_ID
 *   if no destination is known.
 */
EmberNodeId emberGetBindingRemoteNodeId(uint8_t index);

/** @brief Set the node ID for the binding's destination.
 * See ::emberGetBindingRemoteNodeId() for a description.
 *
 * @param index  The index of a binding table entry.
 *
 * @param id     The ID of the binding's destination.
 */
void emberSetBindingRemoteNodeId(uint8_t index, EmberNodeId id);

/** @brief Delete all binding table entries.
 *
 * @return An ::EmberStatus value that indicates the success
 * or failure of the command.
 */
EmberStatus emberClearBindingTable(void);

/** @brief A callback invoked when a remote node requests that
 * a binding be added to the local binding table (via the ZigBee
 * Device Object at endpoint 0).
 *
 * The application is free to add the
 * binding to the binding table, ignore the request, or take some other
 * action.  It is recommended that nonvolatile bindings be used for
 * remote provisioning applications.
 *
 * The binding's type defaults to ::EMBER_UNICAST_BINDING. The application
 * should set the type as appropriate for the binding's local endpoint and
 * cluster ID.
 *
 * If the application includes ::emberRemoteSetBindingHandler(),
 * it must define EMBER_APPLICATION_HAS_REMOTE_BINDING_HANDLER in its
 * CONFIGURATION_HEADER and also include ::emberRemoteDeleteBindingHandler().
 *
 * @param entry   A pointer to a new binding table entry.
 *
 * @return ::EMBER_ZDP_SUCCESS if the binding was added to the table
 * ::EMBER_ZDP_NOT_AUTHORIZED if permission was denied
 * ::EMBER_ZDP_NOT_PERMITTED if the binding is active
 * ::EMBER_ZDP_TABLE_FULL if there is no more space in the binding table
 */
EmberZdoStatus emberRemoteSetBindingHandler(EmberBindingTableEntry *entry);

/** @brief Invoked when a remote node requests that
 * a binding be removed from the local binding table (via the ZigBee
 * Device Object at endpoint 0).
 *
 * The application is free to remove the
 * binding from the binding table, ignore the request, or take some other
 * action.
 *
 * If the application includes ::emberRemoteDeleteBindingHandler(),
 * it must define EMBER_APPLICATION_HAS_REMOTE_BINDING_HANDLER in its
 * CONFIGURATION_HEADER and also include ::emberRemoteSetBindingHandler().
 *
 * @param index  The index of the binding entry to be removed.
 *
 * @return ::EMBER_ZDP_SUCCESS if the binding was removed from the table
 * ::EMBER_ZDP_NOT_AUTHORIZED if permission was denied
 * ::EMBER_ZDP_NOT_PERMITTED if the binding is active
 * ::EMBER_ZDP_NO_ENTRY if the binding doesn't exist
 */
EmberZdoStatus emberRemoteDeleteBindingHandler(uint8_t index);

/** @brief Return a binding index that matches the current incoming
 * message, if known.
 *
 * A binding matches the incoming message if:
 *   - The binding's source endpoint is the same as the message's destination
 *     endpoint.
 *   - The binding's destination endpoint is the same as the message's source
 *     endpoint.
 *   - The source of the message has been previously identified as the
 *     the binding's remote node by a successful address discovery or
 *     by the application via a call to either
 *     ::emberSetReplyBinding()  or
 *     ::emberNoteSendersBinding().
 *
 * @note This function can be called only from within
 *  ::emberIncomingMessageHandler().
 *
 * @return The index of a binding that matches the current incoming message
 *   or <code>0xFF</code> if there is no matching binding.
 */
uint8_t emberGetBindingIndex(void);

/** @brief Create a binding table entry for the sender of a message,
 * which can be used to send messages to that sender.
 *
 * This function is identical to ::emberSetBinding() except that
 * calling it tells the stack that this binding corresponds to the sender of
 * the current message. The stack uses this information to associate the
 * sender's routing info with the binding table entry.
 * @note This function may only be called from within
 * ::emberIncomingMessageHandler().
 *
 * @param index  The index of the binding to set.
 *
 * @param entry  A pointer to data for the binding.
 *
 * @return An ::EmberStatus value that indicates the success
 * or failure of the command.
 */
EmberStatus emberSetReplyBinding(uint8_t index, EmberBindingTableEntry *entry);

/** @brief Update the routing information associated with a binding
 * table entry for the sender of a message.
 *
 * This function should be used in place of ::emberSetReplyBinding() when a
 * message arrives from a remote endpoint for which a binding already exists.
 *
 * @param index  The index of the binding to update.
 *
 * @return An ::EmberStatus value that indicates the success
 * or failure of the command.
 */
EmberStatus emberNoteSendersBinding(uint8_t index);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// This is defined in config/ember-configuration.c.
extern uint8_t emberBindingTableSize;
#endif

/** @} END addtogroup */

/**
 * <!-- HIDDEN
 * @page 2p5_to_3p0
 * <hr>
 * The file binding-table.h is new and is described in @ref binding_table.
 * <ul>
 * <li> <b>New items</b>
 * <ul>
 * <li> emberGetBindingRemoteNodeId()
 * <li> emberSetBindingRemoteNodeId()
 * </ul>
 * <li> <b>Changed items</b>
 * <ul>
 * </ul>
 * <li> <b>Items moved from ember.h</b>
 *  - emberBindingIsActive()
 *  - emberClearBindingTable()
 *  - emberDeleteBinding()
 *  - emberGetBinding()
 *  - emberGetBindingIndex()
 *  - emberNoteSendersBinding()
 *  - emberRemoteDeleteBindingHandler()
 *  - emberRemoteSetBindingHandler()
 *  - emberSetBinding()
 *  - emberSetReplyBinding()
 *  .
 * </ul>
 * HIDDEN -->
 */

#endif // SILABS_BINDING_TABLE_H
