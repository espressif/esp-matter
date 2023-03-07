/***************************************************************************//**
 * @file
 * @brief MAC Address Filtering - Public APIs
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
 * @defgroup mac-address-filtering MAC Address Filtering
 * @ingroup component soc
 * @brief API and Callbacks for the MAC Address Filtering Component
 *
 * This plugin artificially limits what the stack hears by
 * filtering the packets just above the lower MAC.  The plugin can be
 * configured on the fly via CLI with the short or long address of devices
 * whose packets will be dropped or allowed.  There are two lists, a short
 * address list (2-byte node ID) and a long address list (8-byte IEEE address).
 * Each list may be configured as either a whitelist or blacklist.
 * The default is a blacklist with no addresses configured (accept all packets normally).
 * A separate option allows or drops packets with no address
 * information (e.g., Beacon Requests).  NOTE:  The radio will still generate and
 * process MAC ACKs for packets.  This plugin cannot be used to effect the
 * behavior of MAC ACKs, which are handled by the RAIL library and radio hardware.
 *
 */

/**
 * @addtogroup mac-address-filtering
 * @{
 */

/**
 * @name API
 * @{
 */

/** @brief Print configuration command
 *
 * @param SL_CLI_COMMAND_ARG Ver.: always
 *
 */
void emberAfPluginMacAddressFilteringPrintConfigCommand(SL_CLI_COMMAND_ARG);

/** @brief filter out the no address command
 *
 * @param SL_CLI_COMMAND_ARG Ver.: always
 *
 */
void emberAfPluginMacAddressFilteringFilterNoAddressCommand(SL_CLI_COMMAND_ARG);

/** @brief add the short address command
 *
 * @param SL_CLI_COMMAND_ARG Ver.: always
 *
 */
void emberAfPluginMacAddressFilteringAddShortAddressCommand(SL_CLI_COMMAND_ARG);

/** @brief set the short address list type
 *
 * @param SL_CLI_COMMAND_ARG Ver.: always
 *
 */
void emberAfPluginMacAddressFilteringSetShortAddressListType(SL_CLI_COMMAND_ARG);

/** @brief set the long address list type
 *
 * @param SL_CLI_COMMAND_ARG Ver.: always
 *
 */
void emberAfPluginMacAddressFilteringSetLongAddressListType(SL_CLI_COMMAND_ARG);

/** @brief Add long address
 *
 * @param SL_CLI_COMMAND_ARG Ver.: always
 *
 */
void emberAfPluginMacAddressFilteringAddLongAddressCommand(SL_CLI_COMMAND_ARG);

/** @brief Clear the short address list
 *
 * @param SL_CLI_COMMAND_ARG Ver.: always
 *
 */
void emberAfPluginMacAddressFilteringClearShortAddressList(SL_CLI_COMMAND_ARG);

/** @brief clear the long address list
 *
 * @param SL_CLI_COMMAND_ARG Ver.: always
 *
 */
void emberAfPluginMacAddressFilteringClearLongAddressList(SL_CLI_COMMAND_ARG);

/** @brief Add panID
 *
 * @param SL_CLI_COMMAND_ARG Ver.: always
 *
 */
void emberAfPluginMacAddressFilteringAddPanIdCommand(SL_CLI_COMMAND_ARG);

/** @brief Delete a panID entry
 *
 * @param SL_CLI_COMMAND_ARG Ver.: always
 *
 */
void emberAfPluginMacAddressFilteringPanIdDeleteEntry(SL_CLI_COMMAND_ARG);

/** @brief Clear the panID list
 *
 * @param SL_CLI_COMMAND_ARG Ver.: always
 *
 */
void emberAfPluginMacAddressFilteringClearPanIdList(SL_CLI_COMMAND_ARG);

/** @brief set list type for panID list
 *
 * @param SL_CLI_COMMAND_ARG Ver.: always
 *
 */
void emberAfPluginMacAddressFilteringSetPanIdListType(SL_CLI_COMMAND_ARG);

/** @brief MAC address filtering reset
 *
 * @param SL_CLI_COMMAND_ARG Ver.: always
 *
 */
void emberAfPluginMacAddressFilteringReset(SL_CLI_COMMAND_ARG);

/** @brief Clear all lists for MAC address filtering
 *
 * @param SL_CLI_COMMAND_ARG Ver.: always
 *
 */
void emberAfPluginMacAddressFilteringClearAll(void);

/** @brief Delete an entry from short address list
 *
 * @param SL_CLI_COMMAND_ARG Ver.: always
 *
 */
void emberAfPluginMacAddressFilteringShortAddressDeleteEntry(SL_CLI_COMMAND_ARG);

/** @brief Delete an entry from long address list
 *
 * @param SL_CLI_COMMAND_ARG Ver.: always
 *
 */
void emberAfPluginMacAddressFilteringLongAddressDeleteEntry(SL_CLI_COMMAND_ARG);

/** @brief Get stats for a short address
 *
 * @param index Ver.: always
 * @param matchCountForIndexPtr Ver.: always
 *
 * @return EmberStatus status code
 *
 */
EmberStatus emberAfPluginMacAddressFilteringGetStatsForShortAddress(uint8_t index,
                                                                    uint32_t* matchCountForIndexPtr);
/** @brief Get MAC address filtering stats. Get the allowed, dropped and total packet counts
 *
 * @param shortMode Ver.: always
 * @param allowedPacketCountPtr Ver.: always
 * @param droppedPacketCountPtr Ver.: always
 * @param totalPacketCountPtr Ver.: always
 *
 */
void emberAfPluginMacAddressFilteringGetStats(bool shortMode,
                                              uint32_t* allowedPacketCountPtr,
                                              uint32_t* droppedPacketCountPtr,
                                              uint32_t* totalPacketCountPtr);

/** @} */ // end of name API

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup mac_address_filtering_cb MAC Address Filtering
 * @ingroup af_callback
 * @brief Callbacks for MAC Address Filtering Component
 *
 */

/**
 * @addtogroup mac_address_filtering_cb
 * @{
 */

/** @brief Callback for MAC address filtering initialization
 *
 * @param SLXU_INIT_ARG Ver.: always
 *
 */
void emberAfPluginMacAddressFilteringInitCallback(SLXU_INIT_ARG);

/** @} */ // end of mac_address_filtering_cb
/** @} */ // end of name Callbacks
/** @} */ // end of mac-address-filtering
