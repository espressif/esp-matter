/***************************************************************************//**
 * @file
 * @brief Routines for the Tunneling Server plugin.
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

#include "../../include/af.h"
#include "../../util/common.h"
#include "tunneling-server.h"
#include "app/framework/plugin/address-table/address-table.h"

#ifdef UC_BUILD
#include "zap-cluster-command-parser.h"
#include "tunneling-server-config.h"
#if (EMBER_AF_PLUGIN_TUNNELING_SERVER_TEST_PROTOCOL_SUPPORT == 1)
#define TEST_PROTOCOL_SUPPORT
#endif
#if (EMBER_AF_PLUGIN_TUNNELING_SERVER_CLOSURE_NOTIFICATION_SUPPORT == 1)
#define CLOSURE_NOTIFICATION_SUPPORT
#endif
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_TUNNELING_SERVER_TEST_PROTOCOL_SUPPORT
#define TEST_PROTOCOL_SUPPORT
#endif
#ifdef EMBER_AF_PLUGIN_TUNNELING_SERVER_CLOSURE_NOTIFICATION_SUPPORT
#define CLOSURE_NOTIFICATION_SUPPORT
#endif
#endif // UC_BUILD

#define UNUSED_ENDPOINT_ID 0xFF

// If addressIndex is EMBER_NULL_ADDRESS_TABLE_INDEX and clientEndpoint is
// UNUSED_ENDPOINT_ID, then the entry is unused and available for use by a new
// tunnel.  Occasionally, clientEndpoint will be UNUSED_ENDPOINT_ID, but
// addressIndex will contain a valid index.  This happens after a tunnel is
// removed but before the address table entry has been cleaned up.  There is a
// delay between closure and cleanup to allow the stack to continue using the
// address table entry to send messages to the client.
typedef struct {
/* addressIndex refers to the table entries of both Address Table plugin/stack Addr Table */
  uint8_t   addressIndex;
  uint8_t   clientEndpoint;
  uint8_t   serverEndpoint;
  uint8_t   protocolId;
  uint16_t  manufacturerCode;
  bool flowControlSupport;
  uint32_t  lastActive;
} EmAfTunnelingServerTunnel;

// this tells you both if the test protocol IS SUPPORTED and if
// the current protocol requested IS the test protocol
#ifdef TEST_PROTOCOL_SUPPORT
  #define emAfIsTestProtocol(protocolId, manufacturerCode) \
  ((protocolId) == EMBER_ZCL_TUNNELING_PROTOCOL_ID_TEST    \
   && (manufacturerCode) == ZCL_TUNNELING_CLUSTER_UNUSED_MANUFACTURER_CODE)
  #define emAfTunnelIsTestProtocol(tunnel) \
  (emAfIsTestProtocol((tunnel)->protocolId, (tunnel)->manufacturerCode))
#else
  #define emAfIsTestProtocol(protocolId, manufacturerCode) (false)
  #define emAfTunnelIsTestProtocol(tunnel) (false)
#endif

// global for keeping track of test-harness behavior "busy status"
static bool emberAfPluginTunnelingServerBusyStatus = false;

static EmAfTunnelingServerTunnel tunnels[EMBER_AF_PLUGIN_TUNNELING_SERVER_TUNNEL_LIMIT];

static EmberAfStatus serverFindTunnel(uint16_t tunnelIndex,
                                      uint8_t addressIndex,
                                      uint8_t clientEndpoint,
                                      uint8_t serverEndpoint,
                                      EmAfTunnelingServerTunnel **tunnel);
static void closeInactiveTunnels(uint8_t endpoint);

void emberAfTunnelingClusterServerInitCallback(uint8_t endpoint)
{
  EmberAfStatus status;
  uint16_t closeTunnelTimeout = EMBER_AF_PLUGIN_TUNNELING_SERVER_CLOSE_TUNNEL_TIMEOUT;
  uint8_t i;

  for (i = 0; i < EMBER_AF_PLUGIN_TUNNELING_SERVER_TUNNEL_LIMIT; i++) {
    tunnels[i].addressIndex = EMBER_NULL_ADDRESS_TABLE_INDEX;
    tunnels[i].clientEndpoint = UNUSED_ENDPOINT_ID;
  }

  status = emberAfWriteServerAttribute(endpoint,
                                       ZCL_TUNNELING_CLUSTER_ID,
                                       ZCL_CLOSE_TUNNEL_TIMEOUT_ATTRIBUTE_ID,
                                       (uint8_t *)&closeTunnelTimeout,
                                       ZCL_INT16U_ATTRIBUTE_TYPE);
#if ((defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_TUNNELING_CLUSTER)) || defined(UC_BUILD))
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfTunnelingClusterPrintln("ERR: writing close tunnel timeout 0x%x",
                                   status);
  }
#endif // ((defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_TUNNELING_CLUSTER)) || defined(UC_BUILD))
}

void emberAfTunnelingClusterServerTickCallback(uint8_t endpoint)
{
  closeInactiveTunnels(endpoint);
}

void emberAfPluginTunnelingServerStackStatusCallback(EmberStatus status)
{
  uint8_t i;

  if (status == EMBER_NETWORK_DOWN
      && !emberStackIsPerformingRejoin()) {
    emberAfSimpleMeteringClusterPrintln("Re-initializing tunnels due to stack down.");
    for (i = 0; i < EMBER_AF_PLUGIN_TUNNELING_SERVER_TUNNEL_LIMIT; i++) {
      emberAfPluginTunnelingServerCleanup(i);
    }
  }
}

void emberAfTunnelingClusterServerAttributeChangedCallback(uint8_t endpoint,
                                                           EmberAfAttributeId attributeId)
{
  if (attributeId == ZCL_CLOSE_TUNNEL_TIMEOUT_ATTRIBUTE_ID) {
    closeInactiveTunnels(endpoint);
  }
}

//-----------------------
// ZCL commands callbacks

#ifdef UC_BUILD

bool emberAfTunnelingClusterRequestTunnelCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_tunneling_cluster_request_tunnel_command_t cmd_data;
  uint16_t tunnelIndex = ZCL_TUNNELING_CLUSTER_INVALID_TUNNEL_ID;
  EmberAfTunnelingTunnelStatus status = EMBER_ZCL_TUNNELING_TUNNEL_STATUS_NO_MORE_TUNNEL_IDS;

  if (zcl_decode_tunneling_cluster_request_tunnel_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfTunnelingClusterPrintln("RX: RequestTunnel 0x%x, 0x%2x, 0x%x, 0x%2x",
                                 cmd_data.protocolId,
                                 cmd_data.manufacturerCode,
                                 cmd_data.flowControlSupport,
                                 cmd_data.maximumIncomingTransferSize);

  if (!emAfIsTestProtocol(cmd_data.protocolId, cmd_data.manufacturerCode)
      && !emberAfPluginTunnelingServerIsProtocolSupportedCallback(cmd_data.protocolId,
                                                                  cmd_data.manufacturerCode)) {
    status = EMBER_ZCL_TUNNELING_TUNNEL_STATUS_PROTOCOL_NOT_SUPPORTED;
  } else if (cmd_data.flowControlSupport) {
    // TODO: Implement support for flow control.
    status = EMBER_ZCL_TUNNELING_TUNNEL_STATUS_FLOW_CONTROL_NOT_SUPPORTED;
  } else if (emberAfPluginTunnelingServerBusyStatus) {
    status = EMBER_ZCL_TUNNELING_TUNNEL_STATUS_BUSY;
  } else {
    uint8_t i;
    bool tunnelExists = false;
    // ------------------------------------------------------------------------------------------
    // EMZIGBEE-1395, (old EMAPPFWKV2-1350) :
    // GBCS v2.0,Page 98, Section 10.2.2 Requirements for the Tunneling Cluster
    // "when receiving a RequestTunnel command from a client Device which has
    // an active TunnelID registered with the server Device, reset the
    // corresponding inactivity timer for that active TunnelID and send a
    // RequestTunnelResponse containing that activeTunnelID."
    for (i = 0; i < EMBER_AF_PLUGIN_TUNNELING_SERVER_TUNNEL_LIMIT; i++) {
      if (tunnels[i].addressIndex == emberAfGetAddressIndex()
          && tunnels[i].clientEndpoint == cmd->apsFrame->sourceEndpoint
          && tunnels[i].serverEndpoint == cmd->apsFrame->destinationEndpoint
          && tunnels[i].protocolId == cmd_data.protocolId
          && tunnels[i].manufacturerCode == cmd_data.manufacturerCode) {
        // Update Tunnel Index for response
        tunnelIndex = i;
        // If we are here the tunnel is exsisiting already - hence update the
        // tunnel param as per request.
        tunnels[i].flowControlSupport = cmd_data.flowControlSupport;
        // Reset the timer so that it ticks from now.
        tunnels[i].lastActive = emberAfGetCurrentTime();
        status = EMBER_ZCL_TUNNELING_TUNNEL_STATUS_SUCCESS;
        // This will reschedule the tick that will timeout tunnels.
        closeInactiveTunnels(cmd->apsFrame->destinationEndpoint);
        tunnelExists = true;
        break;
      }
    }
    // ------------------------------------------------------------------------------------------
    // The tunnel already exists - so skip following loop of creating one
    if (!tunnelExists) {
      for (i = 0; i < EMBER_AF_PLUGIN_TUNNELING_SERVER_TUNNEL_LIMIT; i++) {
        if (tunnels[i].addressIndex == EMBER_NULL_ADDRESS_TABLE_INDEX
            && tunnels[i].clientEndpoint == UNUSED_ENDPOINT_ID) {
          EmberEUI64 eui64;
          EmberNodeId client = cmd->source;
          status = EMBER_ZCL_TUNNELING_TUNNEL_STATUS_BUSY;
          if (emberLookupEui64ByNodeId(client, eui64) == EMBER_SUCCESS) {
            tunnels[i].addressIndex = emberAfPluginAddressTableAddEntry(eui64);
            if (tunnels[i].addressIndex != EMBER_NULL_ADDRESS_TABLE_INDEX) {
              tunnelIndex = i;
              tunnels[i].clientEndpoint = cmd->apsFrame->sourceEndpoint;
              tunnels[i].serverEndpoint = cmd->apsFrame->destinationEndpoint;
              tunnels[i].protocolId = cmd_data.protocolId;
              tunnels[i].manufacturerCode = cmd_data.manufacturerCode;
              tunnels[i].flowControlSupport = cmd_data.flowControlSupport;
              tunnels[i].lastActive = emberAfGetCurrentTime();
              status = EMBER_ZCL_TUNNELING_TUNNEL_STATUS_SUCCESS;
              // This will reschedule the tick that will timeout tunnels.
              closeInactiveTunnels(cmd->apsFrame->destinationEndpoint);
            } else {
              emberAfTunnelingClusterPrintln("ERR: Could not create address"
                                             " table entry for node 0x%2x",
                                             client);
            }
          } else {
            emberAfTunnelingClusterPrintln("ERR: EUI64 for node 0x%2x"
                                           " is unknown",
                                           client);
          }
          break;
        }
      }
    }
    if (status == EMBER_ZCL_TUNNELING_TUNNEL_STATUS_SUCCESS) {
      emberAfPluginTunnelingServerTunnelOpenedCallback(tunnelIndex,
                                                       cmd_data.protocolId,
                                                       cmd_data.manufacturerCode,
                                                       cmd_data.flowControlSupport,
                                                       cmd_data.maximumIncomingTransferSize);
    }
  }
  emberAfFillCommandTunnelingClusterRequestTunnelResponse(tunnelIndex,
                                                          status,
                                                          EMBER_AF_PLUGIN_TUNNELING_SERVER_MAXIMUM_INCOMING_TRANSFER_SIZE);
  emberAfGetCommandApsFrame()->options |= EMBER_APS_OPTION_SOURCE_EUI64;
  emberAfSendResponse();
  return true;
}

bool emberAfTunnelingClusterCloseTunnelCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_tunneling_cluster_close_tunnel_command_t cmd_data;
  EmAfTunnelingServerTunnel *tunnel;
  EmberAfStatus status;

  if (zcl_decode_tunneling_cluster_close_tunnel_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfTunnelingClusterPrintln("RX: CloseTunnel 0x%2x", cmd_data.tunnelId);

  status = serverFindTunnel(cmd_data.tunnelId,
                            emberAfGetAddressIndex(),
                            cmd->apsFrame->sourceEndpoint,
                            cmd->apsFrame->destinationEndpoint,
                            &tunnel);
  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    // Mark the entry as unused and schedule a tick to clean up the address
    // table entry.  The delay before cleaning up the address table is to give
    // the stack some time to continue using it for sending the response to the
    // server.
    tunnel->clientEndpoint = UNUSED_ENDPOINT_ID;
    slxu_zigbee_zcl_schedule_server_tick(cmd->apsFrame->destinationEndpoint,
                                         ZCL_TUNNELING_CLUSTER_ID,
                                         MILLISECOND_TICKS_PER_SECOND);
    emberAfPluginTunnelingServerTunnelClosedCallback(cmd_data.tunnelId,
                                                     CLOSE_INITIATED_BY_CLIENT);
  }

  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfTunnelingClusterTransferDataClientToServerCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_tunneling_cluster_transfer_data_client_to_server_command_t cmd_data;
  EmAfTunnelingServerTunnel *tunnel;
  EmberAfStatus status;

  if (zcl_decode_tunneling_cluster_transfer_data_client_to_server_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  uint16_t dataLen = (cmd->bufLen
                      - (cmd->payloadStartIndex
                         + sizeof(cmd_data.tunnelId)));
  EmberAfTunnelingTransferDataStatus tunnelError = EMBER_ZCL_TUNNELING_TRANSFER_DATA_STATUS_DATA_OVERFLOW;

  emberAfTunnelingClusterPrint("RX: TransferData 0x%2x, [", cmd_data.tunnelId);
  emberAfTunnelingClusterPrintBuffer(cmd_data.data, dataLen, false);
  emberAfTunnelingClusterPrintln("]");

  status = serverFindTunnel(cmd_data.tunnelId,
                            emberAfGetAddressIndex(),
                            cmd->apsFrame->sourceEndpoint,
                            cmd->apsFrame->destinationEndpoint,
                            &tunnel);
  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    if (dataLen <= EMBER_AF_PLUGIN_TUNNELING_SERVER_MAXIMUM_INCOMING_TRANSFER_SIZE) {
      tunnel->lastActive = emberAfGetCurrentTime();

      // If this is the test protocol (and the option for test protocol support
      // is enabled), just turn the data around without notifying the
      // application.  Otherwise, everything goes to the application via a
      // callback.
      if (emAfTunnelIsTestProtocol(tunnel)) {
        emberAfPluginTunnelingServerTransferData(cmd_data.tunnelId, cmd_data.data, dataLen);
      } else {
        emberAfPluginTunnelingServerDataReceivedCallback(cmd_data.tunnelId, cmd_data.data, dataLen);
      }
      emberAfSendImmediateDefaultResponse(status);
      return true;
    }
    // else
    //  tunnelError code already set (overflow)
  } else {
    tunnelError = (status == EMBER_ZCL_STATUS_NOT_AUTHORIZED
                   ? EMBER_ZCL_TUNNELING_TRANSFER_DATA_STATUS_WRONG_DEVICE
                   : EMBER_ZCL_TUNNELING_TRANSFER_DATA_STATUS_NO_SUCH_TUNNEL);
  }

  // Error
  emberAfFillCommandTunnelingClusterTransferDataErrorServerToClient(cmd_data.tunnelId,
                                                                    tunnelError);
  emberAfGetCommandApsFrame()->options |= EMBER_APS_OPTION_SOURCE_EUI64;
  emberAfSendResponse();
  return true;
}

bool emberAfTunnelingClusterTransferDataErrorClientToServerCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_tunneling_cluster_transfer_data_error_client_to_server_command_t cmd_data;
  EmAfTunnelingServerTunnel *tunnel;
  EmberAfStatus status;

  if (zcl_decode_tunneling_cluster_transfer_data_error_client_to_server_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfTunnelingClusterPrintln("RX: TransferDataError 0x%2x, 0x%x",
                                 cmd_data.tunnelId,
                                 cmd_data.transferDataStatus);

  status = serverFindTunnel(cmd_data.tunnelId,
                            emberAfGetAddressIndex(),
                            cmd->apsFrame->sourceEndpoint,
                            cmd->apsFrame->destinationEndpoint,
                            &tunnel);
  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    emberAfPluginTunnelingServerDataErrorCallback(cmd_data.tunnelId,
                                                  (EmberAfTunnelingTransferDataStatus)cmd_data.transferDataStatus);
  }

  emberAfSendImmediateDefaultResponse(status);
  return true;
}

#else // !UC_BUILD

bool emberAfTunnelingClusterRequestTunnelCallback(uint8_t protocolId,
                                                  uint16_t manufacturerCode,
                                                  uint8_t flowControlSupport,
                                                  uint16_t maximumIncomingTransferSize)
{
  uint16_t tunnelIndex = ZCL_TUNNELING_CLUSTER_INVALID_TUNNEL_ID;
  EmberAfTunnelingTunnelStatus status = EMBER_ZCL_TUNNELING_TUNNEL_STATUS_NO_MORE_TUNNEL_IDS;

  emberAfTunnelingClusterPrintln("RX: RequestTunnel 0x%x, 0x%2x, 0x%x, 0x%2x",
                                 protocolId,
                                 manufacturerCode,
                                 flowControlSupport,
                                 maximumIncomingTransferSize);

  if (!emAfIsTestProtocol(protocolId, manufacturerCode)
      && !emberAfPluginTunnelingServerIsProtocolSupportedCallback(protocolId,
                                                                  manufacturerCode)) {
    status = EMBER_ZCL_TUNNELING_TUNNEL_STATUS_PROTOCOL_NOT_SUPPORTED;
  } else if (flowControlSupport) {
    // TODO: Implement support for flow control.
    status = EMBER_ZCL_TUNNELING_TUNNEL_STATUS_FLOW_CONTROL_NOT_SUPPORTED;
  } else if (emberAfPluginTunnelingServerBusyStatus) {
    status = EMBER_ZCL_TUNNELING_TUNNEL_STATUS_BUSY;
  } else {
    uint8_t i;
    bool tunnelExists = false;
    // ------------------------------------------------------------------------------------------
    // EMZIGBEE-1395, (old EMAPPFWKV2-1350) :
    // GBCS v2.0,Page 98, Section 10.2.2 Requirements for the Tunneling Cluster
    // "when receiving a RequestTunnel command from a client Device which has
    // an active TunnelID registered with the server Device, reset the
    // corresponding inactivity timer for that active TunnelID and send a
    // RequestTunnelResponse containing that activeTunnelID."
    for (i = 0; i < EMBER_AF_PLUGIN_TUNNELING_SERVER_TUNNEL_LIMIT; i++) {
      if (tunnels[i].addressIndex == emberAfGetAddressIndex()
          && tunnels[i].clientEndpoint == emberAfCurrentCommand()->apsFrame->sourceEndpoint
          && tunnels[i].serverEndpoint == emberAfCurrentCommand()->apsFrame->destinationEndpoint
          && tunnels[i].protocolId == protocolId
          && tunnels[i].manufacturerCode == manufacturerCode) {
        // Update Tunnel Index for response
        tunnelIndex = i;
        // If we are here the tunnel is exsisiting already - hence update the
        // tunnel param as per request.
        tunnels[i].flowControlSupport = flowControlSupport;
        // Reset the timer so that it ticks from now.
        tunnels[i].lastActive = emberAfGetCurrentTime();
        status = EMBER_ZCL_TUNNELING_TUNNEL_STATUS_SUCCESS;
        // This will reschedule the tick that will timeout tunnels.
        closeInactiveTunnels(emberAfCurrentCommand()->apsFrame->destinationEndpoint);
        tunnelExists = true;
        break;
      }
    }
    // ------------------------------------------------------------------------------------------
    // The tunnel already exists - so skip following loop of creating one
    if (!tunnelExists) {
      for (i = 0; i < EMBER_AF_PLUGIN_TUNNELING_SERVER_TUNNEL_LIMIT; i++) {
        if (tunnels[i].addressIndex == EMBER_NULL_ADDRESS_TABLE_INDEX
            && tunnels[i].clientEndpoint == UNUSED_ENDPOINT_ID) {
          EmberEUI64 eui64;
          EmberNodeId client = emberAfCurrentCommand()->source;
          status = EMBER_ZCL_TUNNELING_TUNNEL_STATUS_BUSY;
          if (emberLookupEui64ByNodeId(client, eui64) == EMBER_SUCCESS) {
            tunnels[i].addressIndex = emberAfPluginAddressTableAddEntry(eui64);
            if (tunnels[i].addressIndex != EMBER_NULL_ADDRESS_TABLE_INDEX) {
              tunnelIndex = i;
              tunnels[i].clientEndpoint = emberAfCurrentCommand()->apsFrame->sourceEndpoint;
              tunnels[i].serverEndpoint = emberAfCurrentCommand()->apsFrame->destinationEndpoint;
              tunnels[i].protocolId = protocolId;
              tunnels[i].manufacturerCode = manufacturerCode;
              tunnels[i].flowControlSupport = flowControlSupport;
              tunnels[i].lastActive = emberAfGetCurrentTime();
              status = EMBER_ZCL_TUNNELING_TUNNEL_STATUS_SUCCESS;
              // This will reschedule the tick that will timeout tunnels.
              closeInactiveTunnels(emberAfCurrentCommand()->apsFrame->destinationEndpoint);
            } else {
              emberAfTunnelingClusterPrintln("ERR: Could not create address"
                                             " table entry for node 0x%2x",
                                             client);
            }
          } else {
            emberAfTunnelingClusterPrintln("ERR: EUI64 for node 0x%2x"
                                           " is unknown",
                                           client);
          }
          break;
        }
      }
    }
    if (status == EMBER_ZCL_TUNNELING_TUNNEL_STATUS_SUCCESS) {
      emberAfPluginTunnelingServerTunnelOpenedCallback(tunnelIndex,
                                                       protocolId,
                                                       manufacturerCode,
                                                       flowControlSupport,
                                                       maximumIncomingTransferSize);
    }
  }
  emberAfFillCommandTunnelingClusterRequestTunnelResponse(tunnelIndex,
                                                          status,
                                                          EMBER_AF_PLUGIN_TUNNELING_SERVER_MAXIMUM_INCOMING_TRANSFER_SIZE);
  emberAfGetCommandApsFrame()->options |= EMBER_APS_OPTION_SOURCE_EUI64;
  emberAfSendResponse();
  return true;
}

bool emberAfTunnelingClusterCloseTunnelCallback(uint16_t tunnelIndex)
{
  EmAfTunnelingServerTunnel *tunnel;
  EmberAfStatus status;

  emberAfTunnelingClusterPrintln("RX: CloseTunnel 0x%2x", tunnelIndex);

  status = serverFindTunnel(tunnelIndex,
                            emberAfGetAddressIndex(),
                            emberAfCurrentCommand()->apsFrame->sourceEndpoint,
                            emberAfCurrentCommand()->apsFrame->destinationEndpoint,
                            &tunnel);
  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    // Mark the entry as unused and schedule a tick to clean up the address
    // table entry.  The delay before cleaning up the address table is to give
    // the stack some time to continue using it for sending the response to the
    // server.
    tunnel->clientEndpoint = UNUSED_ENDPOINT_ID;
    slxu_zigbee_zcl_schedule_server_tick(emberAfCurrentCommand()->apsFrame->destinationEndpoint,
                                         ZCL_TUNNELING_CLUSTER_ID,
                                         MILLISECOND_TICKS_PER_SECOND);
    emberAfPluginTunnelingServerTunnelClosedCallback(tunnelIndex,
                                                     CLOSE_INITIATED_BY_CLIENT);
  }

  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfTunnelingClusterTransferDataClientToServerCallback(uint16_t tunnelIndex,
                                                               uint8_t* data)
{
  EmAfTunnelingServerTunnel *tunnel;
  EmberAfStatus status;
  uint16_t dataLen = (emberAfCurrentCommand()->bufLen
                      - (emberAfCurrentCommand()->payloadStartIndex
                         + sizeof(tunnelIndex)));
  EmberAfTunnelingTransferDataStatus tunnelError = EMBER_ZCL_TUNNELING_TRANSFER_DATA_STATUS_DATA_OVERFLOW;

  emberAfTunnelingClusterPrint("RX: TransferData 0x%2x, [", tunnelIndex);
  emberAfTunnelingClusterPrintBuffer(data, dataLen, false);
  emberAfTunnelingClusterPrintln("]");

  status = serverFindTunnel(tunnelIndex,
                            emberAfGetAddressIndex(),
                            emberAfCurrentCommand()->apsFrame->sourceEndpoint,
                            emberAfCurrentCommand()->apsFrame->destinationEndpoint,
                            &tunnel);
  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    if (dataLen <= EMBER_AF_PLUGIN_TUNNELING_SERVER_MAXIMUM_INCOMING_TRANSFER_SIZE) {
      tunnel->lastActive = emberAfGetCurrentTime();

      // If this is the test protocol (and the option for test protocol support
      // is enabled), just turn the data around without notifying the
      // application.  Otherwise, everything goes to the application via a
      // callback.
      if (emAfTunnelIsTestProtocol(tunnel)) {
        emberAfPluginTunnelingServerTransferData(tunnelIndex, data, dataLen);
      } else {
        emberAfPluginTunnelingServerDataReceivedCallback(tunnelIndex, data, dataLen);
      }
      emberAfSendImmediateDefaultResponse(status);
      return true;
    }
    // else
    //  tunnelError code already set (overflow)
  } else {
    tunnelError = (status == EMBER_ZCL_STATUS_NOT_AUTHORIZED
                   ? EMBER_ZCL_TUNNELING_TRANSFER_DATA_STATUS_WRONG_DEVICE
                   : EMBER_ZCL_TUNNELING_TRANSFER_DATA_STATUS_NO_SUCH_TUNNEL);
  }

  // Error
  emberAfFillCommandTunnelingClusterTransferDataErrorServerToClient(tunnelIndex,
                                                                    tunnelError);
  emberAfGetCommandApsFrame()->options |= EMBER_APS_OPTION_SOURCE_EUI64;
  emberAfSendResponse();
  return true;
}

bool emberAfTunnelingClusterTransferDataErrorClientToServerCallback(uint16_t tunnelIndex,
                                                                    uint8_t transferDataStatus)
{
  EmAfTunnelingServerTunnel *tunnel;
  EmberAfStatus status;

  emberAfTunnelingClusterPrintln("RX: TransferDataError 0x%2x, 0x%x",
                                 tunnelIndex,
                                 transferDataStatus);

  status = serverFindTunnel(tunnelIndex,
                            emberAfGetAddressIndex(),
                            emberAfCurrentCommand()->apsFrame->sourceEndpoint,
                            emberAfCurrentCommand()->apsFrame->destinationEndpoint,
                            &tunnel);
  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    emberAfPluginTunnelingServerDataErrorCallback(tunnelIndex,
                                                  (EmberAfTunnelingTransferDataStatus)transferDataStatus);
  }

  emberAfSendImmediateDefaultResponse(status);
  return true;
}

#endif // UC_BUILD

EmberAfStatus emberAfPluginTunnelingServerTransferData(uint16_t tunnelIndex,
                                                       uint8_t *data,
                                                       uint16_t dataLen)
{
  if (tunnelIndex < EMBER_AF_PLUGIN_TUNNELING_SERVER_TUNNEL_LIMIT
      && tunnels[tunnelIndex].clientEndpoint != UNUSED_ENDPOINT_ID) {
    EmberStatus status;
    emberAfFillCommandTunnelingClusterTransferDataServerToClient(tunnelIndex,
                                                                 data,
                                                                 dataLen);
    emberAfSetCommandEndpoints(tunnels[tunnelIndex].serverEndpoint,
                               tunnels[tunnelIndex].clientEndpoint);
    emberAfGetCommandApsFrame()->options |= EMBER_APS_OPTION_SOURCE_EUI64;
    status = emberAfSendCommandUnicast(EMBER_OUTGOING_VIA_ADDRESS_TABLE,
                                       tunnels[tunnelIndex].addressIndex);

    tunnels[tunnelIndex].lastActive = emberAfGetCurrentTime();
    return (status == EMBER_SUCCESS
            ? EMBER_ZCL_STATUS_SUCCESS
            : EMBER_ZCL_STATUS_FAILURE);
  }
  return EMBER_ZCL_STATUS_NOT_FOUND;
}

void emberAfPluginTunnelingServerCleanup(uint8_t tunnelId)
{
  if (tunnels[tunnelId].clientEndpoint != UNUSED_ENDPOINT_ID) {
    tunnels[tunnelId].clientEndpoint = UNUSED_ENDPOINT_ID;
    emberAfPluginTunnelingServerTunnelClosedCallback(tunnelId, CLOSE_INITIATED_BY_SERVER);
  }
  if (tunnels[tunnelId].addressIndex != EMBER_NULL_ADDRESS_TABLE_INDEX) {
    emberAfPluginAddressTableRemoveEntryByIndex(tunnels[tunnelId].addressIndex);
    tunnels[tunnelId].addressIndex = EMBER_NULL_ADDRESS_TABLE_INDEX;
  }
}

static EmberAfStatus serverFindTunnel(uint16_t tunnelIndex,
                                      uint8_t addressIndex,
                                      uint8_t clientEndpoint,
                                      uint8_t serverEndpoint,
                                      EmAfTunnelingServerTunnel **tunnel)
{
  if (tunnelIndex < EMBER_AF_PLUGIN_TUNNELING_SERVER_TUNNEL_LIMIT
      && tunnels[tunnelIndex].clientEndpoint != UNUSED_ENDPOINT_ID) {
    if (tunnels[tunnelIndex].addressIndex == addressIndex
        && tunnels[tunnelIndex].clientEndpoint == clientEndpoint
        && tunnels[tunnelIndex].serverEndpoint == serverEndpoint) {
      *tunnel = &tunnels[tunnelIndex];
      return EMBER_ZCL_STATUS_SUCCESS;
    } else {
      return EMBER_ZCL_STATUS_NOT_AUTHORIZED;
    }
  }
  return EMBER_ZCL_STATUS_NOT_FOUND;
}

static void closeInactiveTunnels(uint8_t endpoint)
{
  EmberAfStatus status;
  uint32_t currentTime = emberAfGetCurrentTime();
  uint32_t delay = MAX_INT32U_VALUE;
  uint16_t closeTunnelTimeout;
  uint8_t i;

  status = emberAfReadServerAttribute(endpoint,
                                      ZCL_TUNNELING_CLUSTER_ID,
                                      ZCL_CLOSE_TUNNEL_TIMEOUT_ATTRIBUTE_ID,
                                      (uint8_t *)&closeTunnelTimeout,
                                      sizeof(closeTunnelTimeout));
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfTunnelingClusterPrintln("ERR: reading close tunnel timeout 0x%x",
                                   status);
    return;
  }

  // Every time the tick fires, we search the table for inactive tunnels that
  // should be closed and unused entries that still have an address table
  // index.  The unused tunnels have been closed, but the address table entry
  // was not immediately removed so the stack could continue using it.  By this
  // point, we've given the stack a fair shot to use it, so now remove the
  // address table entry.  While looking through the tunnels, the time to next
  // tick is calculated based on how recently the tunnels were used or by the
  // need to clean up newly unused tunnels.
  for (i = 0; i < EMBER_AF_PLUGIN_TUNNELING_SERVER_TUNNEL_LIMIT; i++) {
    if (tunnels[i].serverEndpoint == endpoint) {
      if (tunnels[i].clientEndpoint != UNUSED_ENDPOINT_ID) {
        uint32_t elapsed = currentTime - tunnels[i].lastActive;
        if (closeTunnelTimeout <= elapsed) {
          // If we are closing an inactive tunnel and will not send a closure
          // notification, we can immediately remove the address table entry
          // for the client because it will no longer be used.  Otherwise, we
          // need to schedule a tick to clean up the address table entry so we
          // give the stack a chance to continue using it for sending the
          // notification.
#ifdef CLOSURE_NOTIFICATION_SUPPORT
          emberAfFillCommandTunnelingClusterTunnelClosureNotification(i);
          emberAfSetCommandEndpoints(tunnels[i].serverEndpoint,
                                     tunnels[i].clientEndpoint);
          emberAfGetCommandApsFrame()->options |= EMBER_APS_OPTION_SOURCE_EUI64;
          emberAfSendCommandUnicast(EMBER_OUTGOING_VIA_ADDRESS_TABLE,
                                    tunnels[i].addressIndex);

          delay = 1;
#else
          emberAfPluginAddressTableRemoveEntryByIndex(tunnels[i].addressIndex);
          tunnels[i].addressIndex = EMBER_NULL_ADDRESS_TABLE_INDEX;
#endif
          tunnels[i].clientEndpoint = UNUSED_ENDPOINT_ID;
          emberAfPluginTunnelingServerTunnelClosedCallback(i,
                                                           CLOSE_INITIATED_BY_SERVER);
        } else {
          uint32_t remaining = closeTunnelTimeout - elapsed;
          if (remaining < delay) {
            delay = remaining;
          }
        }
      } else if (tunnels[i].addressIndex != EMBER_NULL_ADDRESS_TABLE_INDEX) {
        emberAfPluginAddressTableRemoveEntryByIndex(tunnels[i].addressIndex);
        tunnels[i].addressIndex = EMBER_NULL_ADDRESS_TABLE_INDEX;
      }
    }
  }

  if (delay != MAX_INT32U_VALUE) {
    slxu_zigbee_zcl_schedule_server_tick(endpoint,
                                         ZCL_TUNNELING_CLUSTER_ID,
                                         delay * MILLISECOND_TICKS_PER_SECOND);
  }
}

void emberAfPluginTunnelingServerToggleBusyCommand(void)
{
  emberAfTunnelingClusterPrintln("");
  if (emberAfPluginTunnelingServerBusyStatus) {
    emberAfPluginTunnelingServerBusyStatus = false;
    emberAfTunnelingClusterPrintln("  NOTE: current status is NOT BUSY (tunneling works)");
  } else {
    emberAfPluginTunnelingServerBusyStatus = true;
    emberAfTunnelingClusterPrintln("  NOTE: current status is BUSY (tunneling won't work)");
  }
  emberAfTunnelingClusterPrintln("");
  emberAfTunnelingClusterFlush();
}

void emAfPluginTunnelingServerCliPrint(void)
{
  uint32_t currentTime = emberAfGetCurrentTime();
  uint8_t i;
  emberAfTunnelingClusterPrintln("");
  emberAfTunnelingClusterPrintln("#   client              cep  sep  tid    pid  mfg    age");
  emberAfTunnelingClusterFlush();
  for (i = 0; i < EMBER_AF_PLUGIN_TUNNELING_SERVER_TUNNEL_LIMIT; i++) {
    emberAfTunnelingClusterPrint("%x: ", i);
    if (tunnels[i].clientEndpoint != UNUSED_ENDPOINT_ID) {
      EmberEUI64 eui64;
      emberAfPluginAddressTableLookupByIndex(tunnels[i].addressIndex, eui64);
      emberAfTunnelingClusterDebugExec(emberAfPrintBigEndianEui64(eui64));
      emberAfTunnelingClusterPrint(" 0x%x 0x%x 0x%2x",
                                   tunnels[i].clientEndpoint,
                                   tunnels[i].serverEndpoint,
                                   i);
      emberAfTunnelingClusterFlush();
      emberAfTunnelingClusterPrint(" 0x%x 0x%2x 0x%4x",
                                   tunnels[i].protocolId,
                                   tunnels[i].manufacturerCode,
                                   currentTime - tunnels[i].lastActive);
      emberAfTunnelingClusterFlush();
    }
    emberAfTunnelingClusterPrintln("");
  }
}

#ifdef UC_BUILD

uint32_t emberAfTunnelingClusterServerCommandParse(sl_service_opcode_t opcode,
                                                   sl_service_function_context_t *context)
{
  (void)opcode;

  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_REQUEST_TUNNEL_COMMAND_ID:
      {
        wasHandled = emberAfTunnelingClusterRequestTunnelCallback(cmd);
        break;
      }
      case ZCL_CLOSE_TUNNEL_COMMAND_ID:
      {
        wasHandled = emberAfTunnelingClusterCloseTunnelCallback(cmd);
        break;
      }
      case ZCL_TRANSFER_DATA_CLIENT_TO_SERVER_COMMAND_ID:
      {
        wasHandled = emberAfTunnelingClusterTransferDataClientToServerCallback(cmd);
        break;
      }
      case ZCL_TRANSFER_DATA_ERROR_CLIENT_TO_SERVER_COMMAND_ID:
      {
        wasHandled = emberAfTunnelingClusterTransferDataErrorClientToServerCallback(cmd);
        break;
      }
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
