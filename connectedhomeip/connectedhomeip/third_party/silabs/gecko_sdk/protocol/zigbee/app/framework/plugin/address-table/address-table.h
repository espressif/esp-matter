/***************************************************************************/ /**
 * @file
 * @brief This code provides support for managing the address table.
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

#ifndef SILABS_PLUGIN_ADDRESS_TABLE_H
#define SILABS_PLUGIN_ADDRESS_TABLE_H

/**
 * @defgroup address-table Address Table
 * @ingroup component
 * @brief API and Callbacks for the Address Table Component
 *
 * This component supports managing the address table. On the SoC, all
 * actions are directly performed on the stack address table. The host
 * maintains a copy of the address table and ensures that it is always in
 * sync with the NCP's address table. When building the Host, make sure that the
 * NCP can support the address table size in the component configuration.
 * The optimal size varies based on the platform architecture and size of other
 * dynamically-allocated data for this application. If errors occur upon
 * reseting the NCP, the NCP may not have enough room to store a large address
 * table size. If this is not the case, the NCP may not be able to dynamically
 * increase the address table size based on the host's request. To work around
 * this issue, statically allocate the desired address table size on
 * the Zigbee Pro Stack\Pro Leaf Stack components on the NCP.
 *
 */

/**
 * @addtogroup address-table
 * @{
 */

/**
 * @name API
 * @{
 */

/**
 * @brief Add a new entry to the address table.
 *
 * If the entry already exists, no
 * new entry is created and the index of the entry already in the table is
 * returned. If a free entry is found, the entry is added to the table and the
 * corresponding index is returned. If no free entry is available in the address
 * table, the value EMBER_NULL_ADDRESS_TABLE_INDEX is returned.
 */
#define emberAfPluginAddressTableAddEntry(longId) \
  emberAfAddAddressTableEntry((longId), EMBER_UNKNOWN_NODE_ID);

/**
 * @brief Remove an entry from the address table.
 *
 * If the entry exists, the
 * entry is cleared and the EMBER_SUCCESS status is returned. Otherwise, the
 * status EMBER_INVALID_CALL is returned.
 */
EmberStatus emberAfPluginAddressTableRemoveEntry(EmberEUI64 entry);

/**
 * @brief Remove an entry from the address table by index.
 *
 * If the index is valid,
 * the entry is cleared and the EMBER_SUCCESS status is returned. Otherwise, the
 * status EMBER_INVALID_CALL is returned.
 */
#define emberAfPluginAddressTableRemoveEntryByIndex(index) \
  emberAfRemoveAddressTableEntry((index))

/**
 * @brief Search for an EUI64 in the address table.
 *
 * If an entry with a
 * matching EUI64 is found, the function returns the corresponding index.
 * Otherwise, the value EMBER_NULL_ADDRESS_TABLE_INDEX is returned.
 */
#define emberAfPluginAddressTableLookupByEui64(longId) \
  emberAfLookupAddressTableEntryByEui64((longId))
uint8_t emberAfLookupAddressTableEntryByEui64(EmberEUI64 longId);

/**
 * @brief Copy the entry stored at the passed
 * index to the passed variable and return EMBER_SUCCESS if the
 * passed index is valid.
 *
 * Otherwise, the status
 * EMBER_INVALID_CALL is returned.
 */
EmberStatus emberAfPluginAddressTableLookupByIndex(uint8_t index,
                                                   EmberEUI64 entry);

/**
 * @brief Copy the entry stored at the passed
 * index to the passed variable and return EMBER_SUCCESS
 * if the passed index is valid.
 *
 * Otherwise, the status
 * EMBER_INVALID_CALL is returned.
 */
EmberNodeId emberAfPluginAddressTableLookupNodeIdByIndex(uint8_t index);

/**
 * @brief Allow sending a unicast message specifying the EUI64 of the
 * destination node.
 *
 * Note: the specified EUI64 must be present in the address
 * table. Otherwise, the status EMBER_INVALID_CALL is returned.
 */
EmberStatus emberAfSendUnicastToEui64(EmberEUI64 destination,
                                      EmberApsFrame *apsFrame,
                                      uint16_t messageLength,
                                      uint8_t *message);

/**
 * @brief Send the command prepared with emberAfFill.... macro.
 *
 * This function sends a command that was previously prepared
 * using the emberAfFill... macros from the client command API.
 * It will be sent as unicast to the node that corresponds in the address table
 * to the passed EUI64.
 */
EmberStatus emberAfSendCommandUnicastToEui64(EmberEUI64 destination);

/**
 * @brief Copy the sender EUI64, if available, to the address parameter
 * and return EMBER_SUCCESS. Otherwise, return EMBER_INVALID_CALL.
 */
EmberStatus emberAfGetCurrentSenderEui64(EmberEUI64 address);

/** @} */ // end of name APIs
/** @} */ // end of group address-table

#endif // SILABS_PLUGIN_ADDRESS_TABLE_H
