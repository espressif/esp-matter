/***************************************************************************//**
 * @file
 * @brief APIs and defines for the MN Price Passthrough plugin, which handles
 *        demonstrates client and server Price cluster passthrough on a multi-
 *        network device.
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

#ifdef UC_BUILD
#include "sl_component_catalog.h"
#ifdef SL_CATALOG_ZIGBEE_PRICE_SERVER_PRESENT
#endif
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_PRICE_SERVER
#define SL_CATALOG_ZIGBEE_PRICE_SERVER_PRESENT
#endif
#endif // UC_BUILD

/**
 * @defgroup mn-price-passthrough  MN Price Passthrough Client/Server
 * @ingroup component cluster
 * @brief API and Callbacks for the MN Price Passthrough Cluster Client/Server Component
 *
 * This component demonstrates the capabilities of the multi-network support.
 * The component acts as a Price cluster client on one network and a
 * Price cluster server on another network.  As a client, it
 * periodically polls a remote server for pricing information and then
 * stores the data locally.  As a server, it makes that pricing information
 * available to clients, thereby proxying the data between networks.
 *
 */

/**
 * @addtogroup mn-price-passthrough
 * @{
 */

void emAfPluginMnPricePassthroughStartPollAndForward(void);
void emAfPluginMnPricePassthroughStopPollAndForward(void);
void emAfPluginMnPricePassthroughRoutingSetup(EmberNodeId fwdId,
                                              uint8_t fwdEndpoint,
                                              uint8_t esiEndpoint);
void emAfPluginMnPricePassthroughPrintCurrentPrice(void);
