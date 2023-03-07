/***************************************************************************//**
 * @file
 * @brief Definitions for the Trust Center Network Key Update Broadcast plugin.
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
 * @defgroup trust-center-nwk-key-update-broadcast Trust Center Network Key Update Broadcast
 * @ingroup component
 * @brief API and Callbacks for the Trust Center Network Key Update Broadcast Component
 *
 * This is a Trust Center component that handles updating the
 * Network Key by BROADCASTING the key to all devices in the network. This is the
 * method used by HA and networks that are not running Smart Energy.
 * The new network key is randomly generated.
 *
 */

/**
 * @addtogroup trust-center-nwk-key-update-broadcast
 * @{
 */

// Because both the unicast and broadcast plugins for Trust Center NWK Key update
// define this function, we must protect it to eliminate the redundant
// function declaration.  Unicast and broadcast headers may be included together
// since the code then doesn't need to determine which plugin (unicast or
// broadcast) is being used and thus which header it should included.
#if !defined(EM_AF_TC_START_NETWORK_KEY_UPDATE_DECLARATION)
  #define EM_AF_TC_START_NETWORK_KEY_UPDATE_DECLARATION
EmberStatus emberAfTrustCenterStartNetworkKeyUpdate(void);
#endif

/** @} */ // end of trust-center-nwk-key-update-broadcast
