/***************************************************************************//**
 * @file
 * @brief Definitions for the Tunneling Client plugin.
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
#include "tunneling-client-config.h"
#endif // UC_BUILD

/**
 * @defgroup tunneling-client Tunneling Client
 * @ingroup component cluster
 * @brief API and Callbacks for the Tunneling Cluster Client Component
 *
 * Silicon Labs implementation of Tunneling client cluster.  This component requires
 * extending to integrate the software that processes the tunneled data.
 * Note if the maximum transfer size requires fragmentation,
 * manually include the Fragmentation component and configure it to support
 * the tunnel data size.  Additionally, the component uses the address table
 * to communicate with the server, so you must manually configure the address
 * table size so that it accommodates active tunnels managed by this component
 * as well as any other entries created during normal operation.
 *
 */

/**
 * @addtogroup tunneling-client
 * @{
 */

#define EMBER_AF_PLUGIN_TUNNELING_CLIENT_NULL_INDEX 0xFF

/**
 * @name API
 * @{
 */

/**
 * @brief Request a tunneling cluster tunnel with a server.
 *
 * This function requests a tunnel with a server. The tunneling
 * client plugin will look up the long address of the server (using discovery,
 * if necessary), establish a link key with the server, and create an address
 * table entry for the server before sending the request. All future
 * communication using the tunnel will be sent using the address table entry.
 * The plugin will call ::emberAfPluginTunnelingClientTunnelOpenedCallback with
 * the status of the request.
 *
 * @param server The network address of the server to which the request will be
 * sent.
 * @param clientEndpoint The local endpoint from which the request will be
 * sent.
 * @param serverEndpoint The remote endpoint to which the request will be sent.
 * @param protocolId The protocol ID of the requested tunnel.
 * @param manufacturerCode The manufacturer code of the requested tunnel.
 * @param flowControlSupport true if flow control support is requested or false
 * if not.  Note: flow control is not currently supported by the Tunneling
 * client or server plugins.
 * @return ::EMBER_AF_PLUGIN_TUNNELING_CLIENT_SUCCESS if the request is in
 * process or another ::EmberAfPluginTunnelingClientStatus otherwise.
 */
EmberAfPluginTunnelingClientStatus emberAfPluginTunnelingClientRequestTunnel(EmberNodeId server,
                                                                             uint8_t clientEndpoint,
                                                                             uint8_t serverEndpoint,
                                                                             uint8_t protocolId,
                                                                             uint16_t manufacturerCode,
                                                                             bool flowControlSupport);

/**
 * @brief Transfer data to a server through a tunneling cluster tunnel.
 *
 * This function transfers data to a server through a tunnel. The
 * tunneling client plugin will send data to the endpoint on the node that
 * is managing the given tunnel.
 *
 * @param tunnelIndex The index of the tunnel through which to send data.
 * @param data The buffer containing the raw octets of data.
 * @param dataLen The length in octets of the data.
 * @return ::EMBER_ZCL_STATUS_SUCCESS Indicates if data was sent,
 * ::EMBER_ZCL_STATUS_FAILURE if an error occurred, or
 * ::EMBER_ZCL_STATUS_NOT_FOUND if the tunnel does not exist.
 */
EmberAfStatus emberAfPluginTunnelingClientTransferData(uint8_t tunnelIndex,
                                                       uint8_t *data,
                                                       uint16_t dataLen);

/**
 * @brief Close a tunneling cluster tunnel.
 *
 * This function closes a tunnel. The tunneling client plugin
 * will send the close command to the endpoint on the node that is managing the
 * given tunnel.
 *
 * @param tunnelIndex The index of the tunnel to close.
 * @return ::EMBER_ZCL_STATUS_SUCCESS Indicates if the close request was sent,
 * ::EMBER_ZCL_STATUS_FAILURE if an error occurred, or
 * ::EMBER_ZCL_STATUS_NOT_FOUND if the tunnel does not exist.
 */
EmberAfStatus emberAfPluginTunnelingClientCloseTunnel(uint8_t tunnelIndex);

/**
 * @brief Clean up a tunneling cluster tunnel.
 *
 * This function cleans up all states associated with a tunnel.
 * The tunneling client plugin will not send the close command.
 *
 * @param tunnelIndex The index of the tunnel to clean up.
 */
void emberAfPluginTunnelingClientCleanup(uint8_t tunnelIndex);
/** @} */ // end of name API

/** @name
 * Callbacks
 * @{
 */

/**
 * @defgroup tunneling_client_cb Tunneling Client
 * @ingroup af_callback
 * @brief Callbacks for Tunneling Client Component
 *
 */

/**
 * @addtogroup tunneling_client_cb
 * @{
 */

/** @brief Tunnel opened.
 *
 * This function is called by the Tunneling client plugin whenever a tunnel is
 * opened. Clients may open tunnels by sending a Request Tunnel command.
 *
 * @param tunnelIndex The index of the tunnel that has been opened.
 * Ver.: always
 * @param tunnelStatus The status of the request. Ver.: always
 * @param maximumIncomingTransferSize The maximum incoming transfer size of the
 * server. Ver.: always
 */
void emberAfPluginTunnelingClientTunnelOpenedCallback(uint8_t tunnelIndex,
                                                      EmberAfPluginTunnelingClientStatus tunnelStatus,
                                                      uint16_t maximumIncomingTransferSize);
/** @brief Data received.
 *
 * This function is called by the Tunneling client plugin whenever data is
 * received from a server through a tunnel.
 *
 * @param tunnelIndex The index of the tunnel through which the data was
 * received. Ver.: always
 * @param data Buffer containing the raw octets of the data. Ver.: always
 * @param dataLen The length in octets of the data. Ver.: always
 */
void emberAfPluginTunnelingClientDataReceivedCallback(uint8_t tunnelIndex,
                                                      uint8_t *data,
                                                      uint16_t dataLen);
/** @brief Data error.
 *
 * This function is called by the Tunneling client plugin whenever a data error
 * occurs on a tunnel. Errors occur if a device attempts to send data on tunnel
 * that is no longer active or if the tunneling does not belong to the device.
 *
 * @param tunnelIndex The index of the tunnel on which this data error
 * occurred. Ver.: always
 * @param transferDataStatus The error that occurred. Ver.: always
 */
void emberAfPluginTunnelingClientDataErrorCallback(uint8_t tunnelIndex,
                                                   EmberAfTunnelingTransferDataStatus transferDataStatus);
/** @brief Tunnel closed.
 *
 * This function is called by the Tunneling client plugin whenever a server
 * sends a notification that it preemptively closed an inactive tunnel. Servers
 * are not required to notify clients of tunnel closures, so applications
 * cannot rely on this callback being called for all tunnels.
 *
 * @param tunnelIndex The index of the tunnel that has been closed.
 * Ver.: always
 */
void emberAfPluginTunnelingClientTunnelClosedCallback(uint8_t tunnelIndex);
/** @brief Transfer data failure.
 *
 * This function is called when a TransferData command fails to be sent to the
 * intended destination.
 *
 * @param indexOfDestination The index or destination address of the
 * TransferData command. Ver.: always
 * @param apsFrame The ::EmberApsFrame of the command. Ver.: always
 * @param msgLen The length of the payload sent in the command. Ver.: always
 * @param message The payload that was sent in the command. Ver.: always
 * @param status The non-success status code from the transmission of the
 * command. Ver.: always
 */
void emberAfPluginTunnelingClientTransferDataFailureCallback(uint16_t indexOfDestination,
                                                             EmberApsFrame*apsFrame,
                                                             uint16_t msgLen,
                                                             uint8_t*message,
                                                             EmberStatus status);
/** @} */ // end of tunneling_client_cb
/** @} */ // end of Callbacks
/** @} */ // end of tunneling-client

void emAfPluginTunnelingClientCliPrint(void);
