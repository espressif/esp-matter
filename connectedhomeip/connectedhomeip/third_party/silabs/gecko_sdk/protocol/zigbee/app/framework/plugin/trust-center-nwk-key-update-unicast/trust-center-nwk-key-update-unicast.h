/***************************************************************************//**
 * @file
 * @brief Definitions for the Trust Center Network Key Update Unicast plugin.
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
 * @defgroup trust-center-nwk-key-update-unicast Trust Center Network Key Update Unicast
 * @ingroup component
 * @brief API and Callbacks for the Trust Center Network Key Update Unicast Component
 *
 * This is a Trust Center component that handles updating the
 * Network Key by UNICASTING that key to each device encrypted with their link key.
 * It is intended for use in Smart Energy networks. It will traverse the Trust
 * Center's local key table and send the new NWK key to each non-sleepy device with an
 * authorized link key (a device that has successfully performed key establishment).
 * The new network key will be randomly generated.  After all messages have been sent,
 * it will broadcast a key switch message.
 *
 */

/**
 * @addtogroup trust-center-nwk-key-update-unicast
 * @{
 */

#if defined EMBER_TEST
void zdoDiscoveryCallback(const EmberAfServiceDiscoveryResult* result);
#endif

// Because both the unicast and broadcast plugins for Trust Center NWK Key update
// define this function, we must protect it to eliminate the redudandant
// function declaration.  Unicast and broadcast headers may be included together
// since the code then doesn't need to determine which plugin (unicast or
// broadcast) is being used and thus which header it should inclued.
#if !defined(EM_AF_TC_START_NETWORK_KEY_UPDATE_DECLARATION)
  #define EM_AF_TC_START_NETWORK_KEY_UPDATE_DECLARATION
EmberStatus emberAfTrustCenterStartNetworkKeyUpdate(void);
#endif

/** @} */ // end of trust-center-nwk-key-update-unicast
