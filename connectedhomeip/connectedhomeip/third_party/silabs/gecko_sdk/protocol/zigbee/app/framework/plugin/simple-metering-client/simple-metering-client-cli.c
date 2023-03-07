/***************************************************************************//**
 * @file
 * @brief CLI for the Simple Metering Client plugin.
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

#ifdef UC_BUILD
#include "sl_cli.h"

void emAfPluginSimpleMeteringClientCliSchSnapshot(sl_cli_command_arg_t *arguments)
{
  uint8_t payload[13];
  EmberNodeId dstAddr = (EmberNodeId)sl_cli_get_argument_uint16(arguments, 0);
  uint8_t srcEndpoint =  sl_cli_get_argument_uint8(arguments, 1);
  uint8_t dstEndpoint =  sl_cli_get_argument_uint8(arguments, 2);
  uint16_t issuerId = sl_cli_get_argument_uint16(arguments, 3);
  uint8_t commandIndex = sl_cli_get_argument_uint8(arguments, 4);
  uint8_t numberOfCommands = sl_cli_get_argument_uint8(arguments, 5);
  uint8_t snapshotScheduleId = sl_cli_get_argument_uint8(arguments, 6);
  uint32_t startTime = sl_cli_get_argument_uint32(arguments, 7);
  uint32_t snapshotSchedule = sl_cli_get_argument_uint32(arguments, 8);
  uint8_t snapshotType = sl_cli_get_argument_uint8(arguments, 9);
  uint32_t snapshotCause = sl_cli_get_argument_uint32(arguments, 10);

  emberAfSetCommandEndpoints(srcEndpoint, dstEndpoint);

  payload[0] = snapshotScheduleId;
  emberAfCopyInt32u((uint8_t *)payload, 1, startTime);
  emberAfCopyInt24u((uint8_t *)payload, 5, snapshotSchedule);
  payload[8] = snapshotType;
  emberAfCopyInt32u((uint8_t *)payload, 9, snapshotCause);

  emberAfFillCommandSimpleMeteringClusterScheduleSnapshot(issuerId,
                                                          commandIndex,
                                                          numberOfCommands,
                                                          payload,
                                                          13);

  (void)emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, dstAddr);
}

void emAfPluginSimpleMeteringClientCliStartSampling(sl_cli_command_arg_t *arguments)
{
  EmberNodeId dstAddr = (EmberNodeId)sl_cli_get_argument_uint16(arguments, 0);
  uint8_t srcEndpoint =  sl_cli_get_argument_uint8(arguments, 1);
  uint8_t dstEndpoint =  sl_cli_get_argument_uint8(arguments, 2);
  uint16_t issuerId = sl_cli_get_argument_uint16(arguments, 3);
  uint32_t startTime = sl_cli_get_argument_uint32(arguments, 4);
  uint8_t sampleType = sl_cli_get_argument_uint8(arguments, 5);
  uint16_t sampleRequestInterval = sl_cli_get_argument_uint16(arguments, 6);
  uint16_t maxNumberOfSamples = sl_cli_get_argument_uint16(arguments, 7);

  emberAfSetCommandEndpoints(srcEndpoint, dstEndpoint);

  (void) emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND           \
                                    | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER), \
                                   ZCL_SIMPLE_METERING_CLUSTER_ID,         \
                                   ZCL_START_SAMPLING_COMMAND_ID,          \
                                   "wwuvv",                                \
                                   issuerId,                               \
                                   startTime,                              \
                                   sampleType,                             \
                                   sampleRequestInterval,                  \
                                   maxNumberOfSamples);
  // emberAfFillCommandSimpleMeteringClusterStartSampling(issuerId,
  //                                                      startTime,
  //                                                      sampleType,
  //                                                      sampleRequestInterval,
  //                                                      maxNumberOfSamples);
  (void)emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, dstAddr);
}

void emAfPluginSimpleMeteringClientCliGetSampledData(sl_cli_command_arg_t *arguments)
{
  EmberNodeId dstAddr = (EmberNodeId)sl_cli_get_argument_uint16(arguments, 0);
  uint8_t srcEndpoint =  sl_cli_get_argument_uint8(arguments, 1);
  uint8_t dstEndpoint =  sl_cli_get_argument_uint8(arguments, 2);
  uint16_t sampleId = sl_cli_get_argument_uint16(arguments, 3);
  uint32_t startTime = sl_cli_get_argument_uint32(arguments, 4);
  uint8_t sampleType = sl_cli_get_argument_uint8(arguments, 5);
  uint16_t numberOfSamples = sl_cli_get_argument_uint16(arguments, 6);

  emberAfSetCommandEndpoints(srcEndpoint, dstEndpoint);

  (void) emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND \
                                    | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
                                    | ZCL_DISABLE_DEFAULT_RESPONSE_MASK), \
                                   ZCL_SIMPLE_METERING_CLUSTER_ID,        \
                                   ZCL_GET_SAMPLED_DATA_COMMAND_ID,       \
                                   "vwuv",                                \
                                   sampleId,                              \
                                   startTime,                             \
                                   sampleType,                            \
                                   numberOfSamples);
  (void)emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, dstAddr);
}

void emAfPluginSimpleMeteringClientCliLocalChangeSupply(sl_cli_command_arg_t *arguments)
{
  EmberNodeId dstAddr = (EmberNodeId)sl_cli_get_argument_uint16(arguments, 0);
  uint8_t srcEndpoint =  sl_cli_get_argument_uint8(arguments, 1);
  uint8_t dstEndpoint =  sl_cli_get_argument_uint8(arguments, 2);
  uint8_t proposedChange = sl_cli_get_argument_uint8(arguments, 3);

  emberAfSetCommandEndpoints(srcEndpoint, dstEndpoint);

  (void) emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND \
                                    | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
                                    | ZCL_DISABLE_DEFAULT_RESPONSE_MASK), \
                                   ZCL_SIMPLE_METERING_CLUSTER_ID,        \
                                   ZCL_LOCAL_CHANGE_SUPPLY_COMMAND_ID,    \
                                   "u",                                   \
                                   proposedChange);

  (void)emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, dstAddr);
}

#else

#if !defined(EMBER_AF_GENERATE_CLI)

void emAfPluginSimpleMeteringClientCliSchSnapshot(void);
void emAfPluginSimpleMeteringClientCliStartSampling(void);
void emAfPluginSimpleMeteringClientCliGetSampledData(void);
void emAfPluginSimpleMeteringClientCliLocalChangeSupply(void);

EmberCommandEntry emberAfPluginSimpleMeteringClientCommands[] = {
  emberCommandEntryAction("sch-snapshot",
                          emAfPluginSimpleMeteringClientCliSchSnapshot,
                          "vuuwuuuwwuw",
                          "Schedule a snapshot."),
  emberCommandEntryAction("start-sampling",
                          emAfPluginSimpleMeteringClientCliStartSampling,
                          "vuuwwuvv",
                          "Send a start sampling command to a metering server."),
  emberCommandEntryAction("get-sampled-data",
                          emAfPluginSimpleMeteringClientCliGetSampledData,
                          "vuuvwuv",
                          "Send a start sampling command to a metering server."),
  emberCommandEntryAction("local-change-supply",
                          emAfPluginSimpleMeteringClientCliLocalChangeSupply,
                          "vuuu",
                          "Send a start sampling command to a metering server."),
  emberCommandEntryTerminator(),
};

#endif /* !defined(EMBER_AF_GENERATE_CLI) */

void emAfPluginSimpleMeteringClientCliSchSnapshot(void)
{
  uint8_t payload[13];
  EmberNodeId dstAddr = (EmberNodeId)emberUnsignedCommandArgument(0);
  uint8_t srcEndpoint =  (uint8_t)emberUnsignedCommandArgument(1);
  uint8_t dstEndpoint =  (uint8_t)emberUnsignedCommandArgument(2);
  uint32_t issuerId = (uint32_t)emberUnsignedCommandArgument(3);
  uint8_t commandIndex = (uint8_t)emberUnsignedCommandArgument(4);
  uint8_t numberOfCommands = (uint8_t)emberUnsignedCommandArgument(5);
  uint8_t snapshotScheduleId = (uint8_t)emberUnsignedCommandArgument(6);
  uint32_t startTime = (uint32_t)emberUnsignedCommandArgument(7);
  uint32_t snapshotSchedule = (uint32_t)emberUnsignedCommandArgument(8);
  uint8_t snapshotType = (uint8_t)emberUnsignedCommandArgument(9);
  uint32_t snapshotCause = (uint32_t)emberUnsignedCommandArgument(10);

  emberAfSetCommandEndpoints(srcEndpoint, dstEndpoint);

  payload[0] = snapshotScheduleId;
  emberAfCopyInt32u((uint8_t *)payload, 1, startTime);
  emberAfCopyInt24u((uint8_t *)payload, 5, snapshotSchedule);
  payload[8] = snapshotType;
  emberAfCopyInt32u((uint8_t *)payload, 9, snapshotCause);

  emberAfFillCommandSimpleMeteringClusterScheduleSnapshot(issuerId,
                                                          commandIndex,
                                                          numberOfCommands,
                                                          payload,
                                                          13);

  (void)emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, dstAddr);
}

void emAfPluginSimpleMeteringClientCliStartSampling(void)
{
  EmberNodeId dstAddr = (EmberNodeId)emberUnsignedCommandArgument(0);
  uint8_t srcEndpoint =  (uint8_t)emberUnsignedCommandArgument(1);
  uint8_t dstEndpoint =  (uint8_t)emberUnsignedCommandArgument(2);
  uint32_t issuerId = (uint32_t)emberUnsignedCommandArgument(3);
  uint32_t startTime = (uint32_t)emberUnsignedCommandArgument(4);
  uint8_t sampleType = (uint8_t)emberUnsignedCommandArgument(5);
  uint16_t sampleRequestInterval = (uint16_t)emberUnsignedCommandArgument(6);
  uint16_t maxNumberOfSamples = (uint16_t)emberUnsignedCommandArgument(7);

  emberAfSetCommandEndpoints(srcEndpoint, dstEndpoint);

  (void) emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND           \
                                    | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER), \
                                   ZCL_SIMPLE_METERING_CLUSTER_ID,         \
                                   ZCL_START_SAMPLING_COMMAND_ID,          \
                                   "wwuvv",                                \
                                   issuerId,                               \
                                   startTime,                              \
                                   sampleType,                             \
                                   sampleRequestInterval,                  \
                                   maxNumberOfSamples);
  // emberAfFillCommandSimpleMeteringClusterStartSampling(issuerId,
  //                                                      startTime,
  //                                                      sampleType,
  //                                                      sampleRequestInterval,
  //                                                      maxNumberOfSamples);
  (void)emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, dstAddr);
}

void emAfPluginSimpleMeteringClientCliGetSampledData(void)
{
  EmberNodeId dstAddr = (EmberNodeId)emberUnsignedCommandArgument(0);
  uint8_t srcEndpoint =  (uint8_t)emberUnsignedCommandArgument(1);
  uint8_t dstEndpoint =  (uint8_t)emberUnsignedCommandArgument(2);
  uint16_t sampleId = (uint16_t)emberUnsignedCommandArgument(3);
  uint32_t startTime = (uint32_t)emberUnsignedCommandArgument(4);
  uint8_t sampleType = (uint8_t)emberUnsignedCommandArgument(5);
  uint16_t numberOfSamples = (uint16_t)emberUnsignedCommandArgument(6);

  emberAfSetCommandEndpoints(srcEndpoint, dstEndpoint);

  (void) emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND \
                                    | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
                                    | ZCL_DISABLE_DEFAULT_RESPONSE_MASK), \
                                   ZCL_SIMPLE_METERING_CLUSTER_ID,        \
                                   ZCL_GET_SAMPLED_DATA_COMMAND_ID,       \
                                   "vwuv",                                \
                                   sampleId,                              \
                                   startTime,                             \
                                   sampleType,                            \
                                   numberOfSamples);
  (void)emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, dstAddr);
}

void emAfPluginSimpleMeteringClientCliLocalChangeSupply(void)
{
  EmberNodeId dstAddr = (EmberNodeId)emberUnsignedCommandArgument(0);
  uint8_t srcEndpoint =  (uint8_t)emberUnsignedCommandArgument(1);
  uint8_t dstEndpoint =  (uint8_t)emberUnsignedCommandArgument(2);
  uint8_t proposedChange = (uint8_t)emberUnsignedCommandArgument(3);

  emberAfSetCommandEndpoints(srcEndpoint, dstEndpoint);

  (void) emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND \
                                    | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
                                    | ZCL_DISABLE_DEFAULT_RESPONSE_MASK), \
                                   ZCL_SIMPLE_METERING_CLUSTER_ID,        \
                                   ZCL_LOCAL_CHANGE_SUPPLY_COMMAND_ID,    \
                                   "u",                                   \
                                   proposedChange);

  (void)emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, dstAddr);
}

#endif
