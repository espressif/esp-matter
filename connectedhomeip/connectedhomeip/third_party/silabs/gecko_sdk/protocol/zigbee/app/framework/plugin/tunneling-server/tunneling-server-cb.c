/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "af.h"

/** @brief Is Protocol Supported
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
WEAK(bool emberAfPluginTunnelingServerIsProtocolSupportedCallback(uint8_t protocolId,
                                                                  uint16_t manufacturerCode))
{
  return false;
}

/** @brief Tunnel Opened
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
WEAK(void emberAfPluginTunnelingServerTunnelOpenedCallback(uint16_t tunnelIndex,
                                                           uint8_t protocolId,
                                                           uint16_t manufacturerCode,
                                                           bool flowControlSupport,
                                                           uint16_t maximumIncomingTransferSize))
{
}

/** @brief Data Received
 *
 * This function is called by the Tunneling server plugin whenever data is
 * received from a client through a tunnel.
 *
 * @param tunnelIndex The identifier of the tunnel through which the data was
 * received. Ver.: always
 * @param data Buffer containing the raw octets of the data. Ver.: always
 * @param dataLen The length in octets of the data. Ver.: always
 */
WEAK(void emberAfPluginTunnelingServerDataReceivedCallback(uint16_t tunnelIndex,
                                                           uint8_t *data,
                                                           uint16_t dataLen))
{
}

/** @brief Data Error
 *
 * This function is called by the Tunneling server plugin whenever a data error
 * occurs on a tunnel. Errors occur if a device attempts to send data on tunnel
 * that is no longer active or if the tunneling does not belong to the device.
 *
 * @param tunnelIndex The identifier of the tunnel on which this data error
 * occurred. Ver.: always
 * @param transferDataStatus The error that occurred. Ver.: always
 */
WEAK(void emberAfPluginTunnelingServerDataErrorCallback(uint16_t tunnelIndex,
                                                        EmberAfTunnelingTransferDataStatus transferDataStatus))
{
}

/** @brief Tunnel Closed
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
WEAK(void emberAfPluginTunnelingServerTunnelClosedCallback(uint16_t tunnelIndex,
                                                           bool clientInitiated))
{
}
