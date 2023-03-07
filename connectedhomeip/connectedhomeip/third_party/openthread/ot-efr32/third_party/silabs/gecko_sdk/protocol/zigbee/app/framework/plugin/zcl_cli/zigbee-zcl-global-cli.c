/***************************************************************************//**
 * @file
 * @brief APIs and defines for the ZCL Global CLI.
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

extern void sli_zigbee_zcl_simple_command(uint8_t frameControl,
                                          uint16_t clusterId,
                                          uint8_t commandId,
                                          sl_cli_command_arg_t *arguments,
                                          uint8_t *argumentTypes);

#ifdef __cplusplus
extern "C" {
#endif

// Global cli function declarations
extern void sli_zigbee_cli_zcl_global_direction_command(sl_cli_command_arg_t *arguments);
extern void sli_zigbee_cli_zcl_global_command_discovery_generated_command(sl_cli_command_arg_t *arguments);
extern void sli_zigbee_cli_zcl_global_command_discovery_received_command(sl_cli_command_arg_t *arguments);
extern void sli_zigbee_cli_zcl_global_expect_report_from_me_command(sl_cli_command_arg_t *arguments);
extern void sli_zigbee_cli_zcl_global_write_command(sl_cli_command_arg_t *arguments);
extern void sli_zigbee_cli_zcl_global_uwrite_command(sl_cli_command_arg_t *arguments);
extern void sli_zigbee_cli_zcl_global_nwrite_command(sl_cli_command_arg_t *arguments);
extern void sli_zigbee_cli_zcl_global_read_command(sl_cli_command_arg_t *arguments);
extern void sli_zigbee_cli_zcl_global_report_command(sl_cli_command_arg_t *arguments);
extern void sli_zigbee_cli_zcl_global_report_read_command(sl_cli_command_arg_t *arguments);
extern void sli_zigbee_cli_zcl_global_send_me_a_report_command(sl_cli_command_arg_t *arguments);
extern void sli_zigbee_cli_zcl_global_discover_command(sl_cli_command_arg_t *arguments);
extern void sli_zigbee_cli_zcl_mfg_code_command(sl_cli_command_arg_t *arguments);
extern void sli_zigbee_cli_zcl_time_command(sl_cli_command_arg_t *arguments);
extern void sli_zigbee_cli_zcl_use_next_sequence_command(sl_cli_command_arg_t *arguments);
extern void sli_zigbee_cli_zcl_x_default_resp_command(sl_cli_command_arg_t *arguments);
extern void sli_zigbee_cli_zcl_test_response_on_command(sl_cli_command_arg_t *arguments);
extern void sli_zigbee_cli_zcl_test_response_off_command(sl_cli_command_arg_t *arguments);

// ZCL miscellaneous commands
const sl_cli_command_info_t cli_cmd_zcl_mfg_code_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_mfg_code_command,
                 "Sets the two byte manufacturer specific identifier to use for the next command if the command is not already built.",
                 "mfgSpecificId" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_END,
});

const sl_cli_command_info_t cli_cmd_zcl_time_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_time_command,
                 "Cli command to call emberAfSetTime function documented in af.h",
                 "utcTime" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT32,
  SL_CLI_ARG_END,
});

const sl_cli_command_info_t cli_cmd_zcl_use_next_sequence_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_use_next_sequence_command,
                 "Sets the flag to use the incremented sequence number from the framework for the next raw command.",
                 "useNextSequence" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_END,
});

const sl_cli_command_info_t cli_cmd_zcl_x_default_resp_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_x_default_resp_command,
                 "Sets the Disable Default Response Frame Control bit to use for the next command if the command is not already built.",
                 "disableDefaultResponse" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_END,
});

const sl_cli_command_info_t cli_cmd_zcl_test_response_on_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_test_response_on_command,
                 "Sets a flag so that the application framework WILL respond to the next ZCL message that comes in over the air.",
                 SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_END,
});

const sl_cli_command_info_t cli_cmd_zcl_test_response_off_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_test_response_off_command,
                 "Sets a flag so that the application framework WILL NOT respond to the next ZCL message that comes in over the air.",
                 SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_END,
});

// Global Command structs. Names have cli_cmd_zcl_global_[command name]_command signature
static const sl_cli_command_info_t cli_cmd_zcl_global_direction_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_global_direction_command,
                 "Sets the direction for global commands, either client to server or server to client.",
                 "direction" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_END,
});

static const sl_cli_command_info_t cli_cmd_zcl_global_command_discovery_generated_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_global_command_discovery_generated_command,
                 "Create a Discover Commands Generated command with associated values.",
                 "clusterId" SL_CLI_UNIT_SEPARATOR "startCommandId" SL_CLI_UNIT_SEPARATOR "maxCommandId" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_END,
});

static const sl_cli_command_info_t cli_cmd_zcl_global_command_discovery_received_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_global_command_discovery_received_command,
                 "Create a Discover Commands Generated command with associated values.",
                 "clusterId" SL_CLI_UNIT_SEPARATOR "startCommandId" SL_CLI_UNIT_SEPARATOR "maxCommandId" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_END,
});

static const sl_cli_command_info_t cli_cmd_zcl_global_expect_report_from_me_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_global_expect_report_from_me_command,
                 "Create a expect-report-from-me message with associated values.",
                 "clusterId" SL_CLI_UNIT_SEPARATOR "attributeId" SL_CLI_UNIT_SEPARATOR "timeout" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_END,
});

static const sl_cli_command_info_t cli_cmd_zcl_global_write_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_global_write_command,
                 "Creates a global write command message to write to the cluster and attribute specified",
                 "clusterId" SL_CLI_UNIT_SEPARATOR "attributeId" SL_CLI_UNIT_SEPARATOR "type" SL_CLI_UNIT_SEPARATOR "data" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT32,
  SL_CLI_ARG_HEX,
  SL_CLI_ARG_END,
});

static const sl_cli_command_info_t cli_cmd_zcl_global_uwrite_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_global_uwrite_command,
                 "Creates a global write command message to write to the cluster and attribute specified",
                 "clusterId" SL_CLI_UNIT_SEPARATOR "attributeId" SL_CLI_UNIT_SEPARATOR "type" SL_CLI_UNIT_SEPARATOR "data" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT32,
  SL_CLI_ARG_HEX,
  SL_CLI_ARG_END,
});

static const sl_cli_command_info_t cli_cmd_zcl_global_nwrite_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_global_nwrite_command,
                 "Creates a global write command message to write to the cluster and attribute specified",
                 "clusterId" SL_CLI_UNIT_SEPARATOR "attributeId" SL_CLI_UNIT_SEPARATOR "type" SL_CLI_UNIT_SEPARATOR "data" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT32,
  SL_CLI_ARG_HEX,
  SL_CLI_ARG_END,
});

static const sl_cli_command_info_t cli_cmd_zcl_global_read_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_global_read_command,
                 "Creates a global read command message to read from the cluster and attribute specified.",
                 "clusterId" SL_CLI_UNIT_SEPARATOR "attributeId" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_END,
});

static const sl_cli_command_info_t cli_cmd_zcl_global_report_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_global_report_command,
                 "Global reporting command",
                 "endpoint" SL_CLI_UNIT_SEPARATOR "clusterId" SL_CLI_UNIT_SEPARATOR "attributeId" SL_CLI_UNIT_SEPARATOR "mask" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_END,
});

static const sl_cli_command_info_t cli_cmd_zcl_global_report_read_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_global_report_read_command,
                 "Creates a global read reporting command for the associated cluster, attribute and server/client direction.",
                 "clusterId" SL_CLI_UNIT_SEPARATOR "attributeId" SL_CLI_UNIT_SEPARATOR "direction" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_END,
});

static const sl_cli_command_info_t cli_cmd_zcl_global_send_me_a_report_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_global_send_me_a_report_command,
                 "Creates a global send me a report command for the associated values.",
                 "clusterId" SL_CLI_UNIT_SEPARATOR "attributeId" SL_CLI_UNIT_SEPARATOR "dataType" SL_CLI_UNIT_SEPARATOR "minReportTime" SL_CLI_UNIT_SEPARATOR "maxReportTime" SL_CLI_UNIT_SEPARATOR "reportableChange" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_HEX,
  SL_CLI_ARG_END,
});

static const sl_cli_command_info_t cli_cmd_zcl_global_discover_command = \
  SL_CLI_COMMAND(sli_zigbee_cli_zcl_global_discover_command,
                 "Creates a global discover command for the associated values.",
                 "clusterId" SL_CLI_UNIT_SEPARATOR "attributeId" SL_CLI_UNIT_SEPARATOR "maxNumberToReport" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT16,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_END,
});

// Global command table
static const sl_cli_command_entry_t zcl_global_command_table[] = {
  { "direction", &cli_cmd_zcl_global_direction_command, false },
  { "disc-com-gen", &cli_cmd_zcl_global_command_discovery_generated_command, false },
  { "disc-com-rec", &cli_cmd_zcl_global_command_discovery_received_command, false },
  { "expect-report-from-me", &cli_cmd_zcl_global_expect_report_from_me_command, false },
  { "write", &cli_cmd_zcl_global_write_command, false },
  { "uwrite", &cli_cmd_zcl_global_uwrite_command, false },
  { "nwrite", &cli_cmd_zcl_global_nwrite_command, false },
  { "read", &cli_cmd_zcl_global_read_command, false },
  { "report", &cli_cmd_zcl_global_report_command, false },
  { "report-read", &cli_cmd_zcl_global_report_read_command, false },
  { "send-me-a-report", &cli_cmd_zcl_global_send_me_a_report_command, false },
  { "discover", &cli_cmd_zcl_global_discover_command, false },
  { NULL, NULL, false },
};

// Global ZCL commands
const sl_cli_command_info_t cli_cmd_zcl_global_group = \
  SL_CLI_COMMAND_GROUP(zcl_global_command_table, "ZCL global cluster commands");

#endif
