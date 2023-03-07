/***************************************************************************//**
 * @file
 * @brief Definitions for the Tunneling Server plugin.
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
#include "tunneling-server-config.h"
#endif // UC_BUILD

/**
 * @defgroup tunneling-server Tunneling Server
 * @ingroup component cluster
 * @brief API and Callbacks for the Tunneling Cluster Server Component
 *
 * Silicon Labs implementation of Tunneling server cluster.  This component
 * requires extending to integrate the software that processes the tunneled data.
 * Note if the maximum transfer size requires fragmentation, manually
 * include the Fragmentation plugin and configure it to support the tunnel data size.
 * Additionally, the component uses the address table to communicate with clients,
 * so you must manually configure the address table size so that it accommodates
 * active tunnels managed by this component as well as any other entries created
 * during normal operation.  The Tunneling cluster test specification suggests
 * that implementations support the test protocol (protocol id 199, manufacturer
 * code 0xFFFF). If you do not support it, you need some other means of proving
 * two way communications works and is verifiable. If test protocol support is
 * enabled, the component automatically handles requests for the test protocol
 * and these messages will not fall through to the application.
 *
 */

/**
 * @addtogroup tunneling-server
 * @{
 */

#define ZCL_TUNNELING_CLUSTER_INVALID_TUNNEL_ID        0xFFFF
#define ZCL_TUNNELING_CLUSTER_UNUSED_MANUFACTURER_CODE 0xFFFF
#define CLOSE_INITIATED_BY_CLIENT true
#define CLOSE_INITIATED_BY_SERVER false

/**
 * @name API
 * @{
 */

/**
 * @brief Transfer data to a client through a tunneling cluster tunnel.
 *
 * This function transfers data to a client through a tunnel. The
 * tunneling server plugin will send data to the endpoint on the node that
 * opened the given tunnel.
 *
 * @param tunnelIndex The identifier of the tunnel through which to send data.
 * @param data The buffer containing the raw octets of data.
 * @param dataLen The length in octets of data.
 * @return ::EMBER_ZCL_STATUS_SUCCESS Indicates if data was sent,
 * ::EMBER_ZCL_STATUS_FAILURE if an error occurred, or
 * ::EMBER_ZCL_STATUS_NOT_FOUND if the tunnel does not exist.
 */
EmberAfStatus emberAfPluginTunnelingServerTransferData(uint16_t tunnelIndex,
                                                       uint8_t *data,
                                                       uint16_t dataLen);

/**
 * @brief Toggle a "server busy" status for running as a test harness.
 *
 * This function sets the server into a busy state, where it
 * will respond to all request tunnel commands with a busy status.  NOTE:
 * existing tunnels will continue to operate normally.
 */
void emberAfPluginTunnelingServerToggleBusyCommand(void);

/**
 * @brief Clean up a tunneling cluster tunnel.
 *
 * This function cleans up all states associated with a tunnel.
 * The tunneling server plugin will not send the close notification command.
 *
 * @param tunnelId The identifier of the tunnel to clean up.
 */
void emberAfPluginTunnelingServerCleanup(uint8_t tunnelId);
/** @} */ // end of name API

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup tunneling_server_cb Tunneling Server
 * @ingroup af_callback
 * @brief Callbacks for Tunneling Server Component
 *
 */

/**
 * @addtogroup tunneling_server_cb
 * @{
 */

/** @brief Notify whether a protocol is supported.
 *
 * This function is called by the Tunneling server plugin whenever a Request
 * Tunnel command is received. The application should return true if the
 * protocol is supported and false otherwise.
 *
 * @param protocolId The identifier of the metering communication protocol for
 * which the tunnel is requested. Ver.: always
 * @param manufacturerCode The manufacturer code for manufacturer-defined
 * protocols or 0xFFFF in unused. Ver.: always
 */
bool emberAfPluginTunnelingServerIsProtocolSupportedCallback(uint8_t protocolId,
                                                             uint16_t manufacturerCode);

/** @brief Tunnel opened.
 *
 * This function is called by the Tunneling server plugin whenever a tunnel is
 * opened. Clients may open tunnels by sending a Request Tunnel command.
 *
 * @param tunnelIndex The index to the tunnel table entry that has been opened.
 * Ver.: always
 * @param protocolId The identifier of the metering communication protocol for
 * the tunnel. Ver.: always
 * @param manufacturerCode The manufacturer code for manufacturer-defined
 * protocols or 0xFFFF in unused. Ver.: always
 * @param flowControlSupport true is flow control support is requested or false
 * if it is not. Ver.: always
 * @param maximumIncomingTransferSize The maximum incoming transfer size of the
 * client. Ver.: always
 */
void emberAfPluginTunnelingServerTunnelOpenedCallback(uint16_t tunnelIndex,
                                                      uint8_t protocolId,
                                                      uint16_t manufacturerCode,
                                                      bool flowControlSupport,
                                                      uint16_t maximumIncomingTransferSize);

/** @brief Data received.
 *
 * This function is called by the Tunneling server plugin whenever data is
 * received from a client through a tunnel.
 *
 * @param tunnelIndex The identifier of the tunnel through which the data was
 * received. Ver.: always
 * @param data Buffer containing the raw octets of the data. Ver.: always
 * @param dataLen The length in octets of the data. Ver.: always
 */
void emberAfPluginTunnelingServerDataReceivedCallback(uint16_t tunnelIndex,
                                                      uint8_t *data,
                                                      uint16_t dataLen);
/** @brief Data error.
 *
 * This function is called by the Tunneling server plugin whenever a data error
 * occurs on a tunnel. Errors occur if a device attempts to send data on tunnel
 * that is no longer active or if the tunneling does not belong to the device.
 *
 * @param tunnelIndex The identifier of the tunnel on which this data error
 * occurred. Ver.: always
 * @param transferDataStatus The error that occurred. Ver.: always
 */
void emberAfPluginTunnelingServerDataErrorCallback(uint16_t tunnelIndex,
                                                   EmberAfTunnelingTransferDataStatus transferDataStatus);

/** @brief Tunnel closed.
 *
 * This function is called by the Tunneling server plugin whenever a tunnel is
 * closed. Clients may close tunnels by sending a Close Tunnel command. The
 * server can preemptively close inactive tunnels after a timeout.
 *
 * @param tunnelIndex The identifier of the tunnel that has been closed.
 * Ver.: always
 * @param clientInitiated true if the client initiated the closing of the
 * tunnel or false if the server closed the tunnel due to inactivity.
 * Ver.: always
 */
void emberAfPluginTunnelingServerTunnelClosedCallback(uint16_t tunnelIndex,
                                                      bool clientInitiated);

/** @} */ // end of tunneling_server_cb
/** @} */ // end of name Callbacks
/** @} */ // end of tunneling-server

void emAfPluginTunnelingServerPrint(void);
