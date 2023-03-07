/***************************************************************************//**
 * @file
 * @brief Routines for the HC 11073 Tunnel plugin, which implements the
 *        tunneling of 11073 data over zigbee.
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
#include "11073-tunnel.h"

#ifdef UC_BUILD
#include "zap-cluster-command-parser.h"

bool emberAf11073ProtocolTunnelClusterConnectRequestCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_11073_protocol_tunnel_cluster_connect_request_command_t cmd_data;
  bool connected = false;
  bool preemptible = false;
  EmberAfStatus status;

  if (zcl_decode_11073_protocol_tunnel_cluster_connect_request_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  // Check to see if we are already connected by looking at connected attribute
  status = emberAfReadServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                      CLUSTER_ID_11073_TUNNEL,
                                      ATTRIBUTE_11073_TUNNEL_CONNECTED,
                                      (uint8_t *)&connected,
                                      1);

  // if we are already connected send back connection status ALREADY_CONNECTED
  if (connected) {
    emberAfFillCommand11073ProtocolTunnelClusterConnectStatusNotification(
      EMBER_ZCL_11073_TUNNEL_CONNECTION_STATUS_ALREADY_CONNECTED);
    emberAfSendResponse();
    return true;
  }

  // if not already connected copy attributes
  connected = true;
  status = emberAfWriteServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                       CLUSTER_ID_11073_TUNNEL,
                                       ATTRIBUTE_11073_TUNNEL_CONNECTED,
                                       (uint8_t *)&connected,
                                       ZCL_BOOLEAN_ATTRIBUTE_TYPE);

  preemptible = cmd_data.connectControl
                & EMBER_ZCL_11073_CONNECT_REQUEST_CONNECT_CONTROL_PREEMPTIBLE;
  status = emberAfWriteServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                       CLUSTER_ID_11073_TUNNEL,
                                       ATTRIBUTE_11073_TUNNEL_PREEMPTIBLE,
                                       (uint8_t *)&preemptible,
                                       ZCL_BOOLEAN_ATTRIBUTE_TYPE);

  status = emberAfWriteServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                       CLUSTER_ID_11073_TUNNEL,
                                       ATTRIBUTE_11073_TUNNEL_IDLE_TIMEOUT,
                                       (uint8_t*)&cmd_data.idleTimeout,
                                       ZCL_INT16U_ATTRIBUTE_TYPE);

  status = emberAfWriteServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                       CLUSTER_ID_11073_TUNNEL,
                                       ATTRIBUTE_11073_TUNNEL_MANAGER_TARGET,
                                       (uint8_t*)cmd_data.managerTarget,
                                       ZCL_IEEE_ADDRESS_ATTRIBUTE_TYPE);

  status = emberAfWriteServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                       CLUSTER_ID_11073_TUNNEL,
                                       ATTRIBUTE_11073_TUNNEL_MANAGER_ENDPOINT,
                                       &cmd_data.managerEndpoint,
                                       ZCL_INT8U_ATTRIBUTE_TYPE);

  // if idle timer other than 0xffff, set timer to disconnect, reset timer when
  // rx data

  // Generate conection status connected back to manager
  emberAfFillCommand11073ProtocolTunnelClusterConnectStatusNotification(
    EMBER_ZCL_11073_TUNNEL_CONNECTION_STATUS_CONNECTED);
  emberAfSendResponse();

  return true;
}

bool emberAf11073ProtocolTunnelClusterDisconnectRequestCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_11073_protocol_tunnel_cluster_disconnect_request_command_t cmd_data;
  bool connected = false;
  EmberEUI64 currentManager;
  bool preemptible;
  EmberAfStatus status;

  if (zcl_decode_11073_protocol_tunnel_cluster_disconnect_request_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  // check to see if already connected
  status = emberAfReadServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                      CLUSTER_ID_11073_TUNNEL,
                                      ATTRIBUTE_11073_TUNNEL_CONNECTED,
                                      (uint8_t *)&connected,
                                      1);

  // if not currently connected, generate connection status DISCONNECTED
  if (!connected) {
    emberAfFillCommand11073ProtocolTunnelClusterConnectStatusNotification(
      EMBER_ZCL_11073_TUNNEL_CONNECTION_STATUS_DISCONNECTED);
    emberAfSendResponse();
    return true;
  }

  // if is connected, is ieee address same or is pre-emptible set to true?
  status = emberAfReadServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                      CLUSTER_ID_11073_TUNNEL,
                                      ATTRIBUTE_11073_TUNNEL_PREEMPTIBLE,
                                      (uint8_t *)&preemptible,
                                      1);

  if (!preemptible) {
    status = emberAfReadServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                        CLUSTER_ID_11073_TUNNEL,
                                        ATTRIBUTE_11073_TUNNEL_MANAGER_TARGET,
                                        (uint8_t*)&currentManager,
                                        EUI64_SIZE);
    if (MEMCOMPARE(&currentManager, cmd_data.managerIEEEAddress, EUI64_SIZE) != 0) {
      emberAfFillCommand11073ProtocolTunnelClusterConnectStatusNotification(
        EMBER_ZCL_11073_TUNNEL_CONNECTION_STATUS_NOT_AUTHORIZED);
      emberAfSendResponse();
      return true;
    }
  }

  // Set attribute to disconnected
  connected = false;
  status = emberAfWriteServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                       CLUSTER_ID_11073_TUNNEL,
                                       ATTRIBUTE_11073_TUNNEL_CONNECTED,
                                       (uint8_t *)&connected,
                                       ZCL_BOOLEAN_ATTRIBUTE_TYPE);

  // If it is authorized, then we can disconnect.Within 12 seconds device must send
  // DISCONNECTED notification to the manager device. Connected attribute set to
  // false to manager.
  emberAfFillCommand11073ProtocolTunnelClusterConnectStatusNotification(
    EMBER_ZCL_11073_TUNNEL_CONNECTION_STATUS_DISCONNECTED);
  emberAfSendResponse();
  return true;

  // Send another DISCONNECTED connection event to sender of message. (may be same
  // as manager, may be some other device).

  return false;
}

#else // !UC_BUILD

/** @brief Transfer A P D U
 *
 *
 * @param apdu   Ver.: always
 */
bool emberAf11073ProtocolTunnelClusterTransferAPDUCallback(uint8_t* apdu)
{
  return false;
}

/** @brief Connect Request
 *
 *
 * @param connectControl   Ver.: always
 * @param idleTimeout   Ver.: always
 * @param managerTarget   Ver.: always
 * @param managerEndpoint   Ver.: always
 */
bool emberAf11073ProtocolTunnelClusterConnectRequestCallback(uint8_t connectControl,
                                                             uint16_t idleTimeout,
                                                             uint8_t* managerTarget,
                                                             uint8_t managerEndpoint)
{
  bool connected = false;
  bool preemptible = false;
  EmberAfStatus status;

  // Check to see if we are already connected by looking at connected attribute
  status = emberAfReadServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                      CLUSTER_ID_11073_TUNNEL,
                                      ATTRIBUTE_11073_TUNNEL_CONNECTED,
                                      (uint8_t *)&connected,
                                      1);

  // if we are already connected send back connection status ALREADY_CONNECTED
  if (connected) {
    emberAfFillCommand11073ProtocolTunnelClusterConnectStatusNotification(
      EMBER_ZCL_11073_TUNNEL_CONNECTION_STATUS_ALREADY_CONNECTED);
    emberAfSendResponse();
    return true;
  }

  // if not already connected copy attributes
  connected = true;
  status = emberAfWriteServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                       CLUSTER_ID_11073_TUNNEL,
                                       ATTRIBUTE_11073_TUNNEL_CONNECTED,
                                       (uint8_t *)&connected,
                                       ZCL_BOOLEAN_ATTRIBUTE_TYPE);

  preemptible = connectControl
                & EMBER_ZCL_11073_CONNECT_REQUEST_CONNECT_CONTROL_PREEMPTIBLE;
  status = emberAfWriteServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                       CLUSTER_ID_11073_TUNNEL,
                                       ATTRIBUTE_11073_TUNNEL_PREEMPTIBLE,
                                       (uint8_t *)&preemptible,
                                       ZCL_BOOLEAN_ATTRIBUTE_TYPE);

  status = emberAfWriteServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                       CLUSTER_ID_11073_TUNNEL,
                                       ATTRIBUTE_11073_TUNNEL_IDLE_TIMEOUT,
                                       (uint8_t*)&idleTimeout,
                                       ZCL_INT16U_ATTRIBUTE_TYPE);

  status = emberAfWriteServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                       CLUSTER_ID_11073_TUNNEL,
                                       ATTRIBUTE_11073_TUNNEL_MANAGER_TARGET,
                                       (uint8_t*)managerTarget,
                                       ZCL_IEEE_ADDRESS_ATTRIBUTE_TYPE);

  status = emberAfWriteServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                       CLUSTER_ID_11073_TUNNEL,
                                       ATTRIBUTE_11073_TUNNEL_MANAGER_ENDPOINT,
                                       &managerEndpoint,
                                       ZCL_INT8U_ATTRIBUTE_TYPE);

  // if idle timer other than 0xffff, set timer to disconnect, reset timer when
  // rx data

  // Generate conection status connected back to manager
  emberAfFillCommand11073ProtocolTunnelClusterConnectStatusNotification(
    EMBER_ZCL_11073_TUNNEL_CONNECTION_STATUS_CONNECTED);
  emberAfSendResponse();

  return true;
}

/** @brief Disconnect Request
 *
 *
 * @param managerIEEEAddress   Ver.: always
 */
bool emberAf11073ProtocolTunnelClusterDisconnectRequestCallback(uint8_t* managerIEEEAddress)
{
  bool connected = false;
  EmberEUI64 currentManager;
  bool preemptible;
  EmberAfStatus status;

  // check to see if already connected
  status = emberAfReadServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                      CLUSTER_ID_11073_TUNNEL,
                                      ATTRIBUTE_11073_TUNNEL_CONNECTED,
                                      (uint8_t *)&connected,
                                      1);

  // if not currently connected, generate connection status DISCONNECTED
  if (!connected) {
    emberAfFillCommand11073ProtocolTunnelClusterConnectStatusNotification(
      EMBER_ZCL_11073_TUNNEL_CONNECTION_STATUS_DISCONNECTED);
    emberAfSendResponse();
    return true;
  }

  // if is connected, is ieee address same or is pre-emptible set to true?
  status = emberAfReadServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                      CLUSTER_ID_11073_TUNNEL,
                                      ATTRIBUTE_11073_TUNNEL_PREEMPTIBLE,
                                      (uint8_t *)&preemptible,
                                      1);

  if (!preemptible) {
    status = emberAfReadServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                        CLUSTER_ID_11073_TUNNEL,
                                        ATTRIBUTE_11073_TUNNEL_MANAGER_TARGET,
                                        (uint8_t*)&currentManager,
                                        EUI64_SIZE);
    if (MEMCOMPARE(&currentManager, managerIEEEAddress, EUI64_SIZE) != 0) {
      emberAfFillCommand11073ProtocolTunnelClusterConnectStatusNotification(
        EMBER_ZCL_11073_TUNNEL_CONNECTION_STATUS_NOT_AUTHORIZED);
      emberAfSendResponse();
      return true;
    }
  }

  // Set attribute to disconnected
  connected = false;
  status = emberAfWriteServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                       CLUSTER_ID_11073_TUNNEL,
                                       ATTRIBUTE_11073_TUNNEL_CONNECTED,
                                       (uint8_t *)&connected,
                                       ZCL_BOOLEAN_ATTRIBUTE_TYPE);

  // If it is authorized, then we can disconnect.Within 12 seconds device must send
  // DISCONNECTED notification to the manager device. Connected attribute set to
  // false to manager.
  emberAfFillCommand11073ProtocolTunnelClusterConnectStatusNotification(
    EMBER_ZCL_11073_TUNNEL_CONNECTION_STATUS_DISCONNECTED);
  emberAfSendResponse();
  return true;

  // Send another DISCONNECTED connection event to sender of message. (may be same
  // as manager, may be some other device).

  return false;
}

/** @brief Connect Status Notification
 *
 *
 * @param connectStatus   Ver.: always
 */
bool emberAf11073ProtocolTunnelClusterConnectStatusNotificationCallback(uint8_t connectStatus)
{
  return false;
}

#endif // UC_BUILD

#ifdef UC_BUILD

uint32_t emberAf11073ProtocolTunnelClusterServerCommandParse(sl_service_opcode_t opcode,
                                                             sl_service_function_context_t *context)
{
  (void)opcode;

  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_CONNECT_REQUEST_COMMAND_ID:
      {
        wasHandled = emberAf11073ProtocolTunnelClusterConnectRequestCallback(cmd);
        break;
      }
      case ZCL_DISCONNECT_REQUEST_COMMAND_ID:
      {
        wasHandled = emberAf11073ProtocolTunnelClusterDisconnectRequestCallback(cmd);
        break;
      }
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
