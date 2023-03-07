/***************************************************************************//**
 * @file
 * @brief APIs and defines for the ZCL Custom Cluster CLI. This file defines zcl
 * cli commands for certain cluster commands which need customized handling.
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

#include "sl_component_catalog.h"

#ifdef SL_CATALOG_ZIGBEE_ZCL_CLI_PRESENT
#include "sl_cli.h"
#include "zap-config.h"

#if defined(ZCL_USING_IDENTIFY_CLUSTER_SERVER) || defined(ZCL_USING_IDENTIFY_CLUSTER_CLIENT)
extern void sli_zigbee_cli_zcl_identify_on_command(sl_cli_command_arg_t *arguments);
extern void sli_zigbee_cli_zcl_identify_off_command(sl_cli_command_arg_t *arguments);
#endif

#if defined(ZCL_USING_BACNET_PROTOCOL_TUNNEL_CLUSTER_CLIENT) || defined(ZCL_USING_BACNET_PROTOCOL_TUNNEL_CLUSTER_SERVER)
extern void sli_zigbee_cli_zcl_bacnet_transfer_whois_command(sl_cli_command_arg_t *arguments);
#endif

#if defined(ZCL_USING_TUNNELING_CLUSTER_CLIENT) || defined(ZCL_USING_TUNNELING_CLUSTER_SERVER)
extern void sli_zigbee_cli_zcl_tunneling_random_to_server_command(sl_cli_command_arg_t *arguments);
extern void sli_zigbee_cli_zcl_tunneling_transfer_to_server_command(sl_cli_command_arg_t *arguments);
extern void sli_zigbee_cli_zcl_tunneling_random_to_client_command(sl_cli_command_arg_t *arguments);
extern void sli_zigbee_cli_zcl_tunneling_transfer_to_client_command(sl_cli_command_arg_t *arguments);
#endif

// ZCL cluster cli commands with no corresponding command defined in the ZCL spec.
// The following commands are added for backwards compatibility.
#if defined(ZCL_USING_IDENTIFY_CLUSTER_SERVER) || defined(ZCL_USING_IDENTIFY_CLUSTER_CLIENT)
const sl_cli_command_info_t cli_cmd_zcl_identify_on_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_identify_on_command,
                 "Writes the IdentifyTime attribute.",
                 "endpoint" SL_CLI_UNIT_SEPARATOR "time" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_END,
});

const sl_cli_command_info_t cli_cmd_zcl_identify_off_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_identify_off_command,
                 "Writes the IdentifyTime attribute.",
                 "endpoint" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_END,
});
#endif

#if defined(ZCL_USING_BACNET_PROTOCOL_TUNNEL_CLUSTER_CLIENT) || defined(ZCL_USING_BACNET_PROTOCOL_TUNNEL_CLUSTER_SERVER)
const sl_cli_command_info_t cli_cmd_zcl_bacnet_transfer_whois_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_bacnet_transfer_whois_command,
                 "Send a sample whois command.",
                 SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_END,
});
#endif

#if defined(ZCL_USING_TUNNELING_CLUSTER_CLIENT) || defined(ZCL_USING_TUNNELING_CLUSTER_SERVER)
const sl_cli_command_info_t cli_cmd_zcl_tunneling_random_to_server_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_tunneling_random_to_server_command,
                 "Send a random amount of data through a tunnel to the server (used for testing).",
                 "tunnel-id" SL_CLI_UNIT_SEPARATOR "length" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_END,
});

const sl_cli_command_info_t cli_cmd_zcl_tunneling_transfer_to_server_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_tunneling_transfer_to_server_command,
                 "Transfer data through a tunnel to the server.",
                 "tunnel-id" SL_CLI_UNIT_SEPARATOR "data" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_HEX,
  SL_CLI_ARG_END,
});

const sl_cli_command_info_t cli_cmd_zcl_tunneling_random_to_client_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_tunneling_random_to_client_command,
                 "Send a random amount of data through a tunnel to the client (used for testing)",
                 "tunnel-id" SL_CLI_UNIT_SEPARATOR "length" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_END,
});

const sl_cli_command_info_t cli_cmd_zcl_tunneling_transfer_to_client_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_tunneling_transfer_to_client_command,
                 "Transfer data through a tunnel to the client (used for testing)",
                 "tunnel-id" SL_CLI_UNIT_SEPARATOR "data" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_HEX,
  SL_CLI_ARG_END,
});
#endif

#endif
