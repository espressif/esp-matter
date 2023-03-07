/***************************************************************************//**
 * @file
 * @brief CLI commands for sending various messages.
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

#include "af.h"
#include "app/framework/cli/security-cli.h"
#include "app/util/serial/sl_zigbee_command_interpreter.h"
#include "app/framework/util/af-main.h"
#include "app/framework/util/util.h"
#include "zap-config.h"
#include "zcl-cli.h"

//------------------------------------------------------------------------------
// Globals

// EMBER_AF_MAXIMUM_SEND_PAYLOAD_LENGTH is set in config.h
#define APP_ZCL_BUFFER_SIZE EMBER_AF_MAXIMUM_SEND_PAYLOAD_LENGTH

// The command-interpreter doesn't handle individual arguments longer than
// 255 bytes (since it is uses a 1-byte max-length value
#define APP_ZCL_BUFFER_SIZE_CLI \
  (APP_ZCL_BUFFER_SIZE > 255    \
   ? 255                        \
   : APP_ZCL_BUFFER_SIZE)

uint8_t appZclBuffer[APP_ZCL_BUFFER_SIZE];
uint16_t appZclBufferLen;
bool zclCmdIsBuilt = false;

uint16_t mfgSpecificId = EMBER_AF_NULL_MANUFACTURER_CODE;
uint8_t disableDefaultResponse = 0;
EmberApsFrame globalApsFrame;
static bool useNextSequence = false;
// flag to keep track of the fact that we just sent a read attr for time and
// we should set our time to the result of the read attr response
extern bool emAfSyncingTime;

static bool useMulticastBinding = false;

// The direction of global commands, changeable via the "zcl global direction"
// command.  By default, send from client to server, which is how the CLI always
// functioned in the past.
static uint8_t zcl_global_direction = ZCL_FRAME_CONTROL_CLIENT_TO_SERVER;

// Forward declarations
void sli_zigbee_zcl_global_setup(uint8_t commandId, sl_cli_command_arg_t *arguments);

void emAfApsFrameEndpointSetup(uint8_t srcEndpoint,
                               uint8_t dstEndpoint);

void emAfApsFrameClusterIdSetup(uint16_t clusterId)
{
  // setup the global options and cluster ID
  // send command will setup profile ID and endpoints.
  globalApsFrame.options = EMBER_AF_DEFAULT_APS_OPTIONS;
  globalApsFrame.clusterId = clusterId;
}

// keys print
void keysPrintCommand(sl_cli_command_arg_t *arguments)
{
  (void)arguments;

  sl_zigbee_core_debug_print("%sSECURITY_LEVEL: %2X\n",
                             "EMBER_",
                             EMBER_SECURITY_LEVEL);
  printKeyInfo();
}

void eraseKeyTableEntry(uint8_t index)
{
  EmberStatus status = emberEraseKeyTableEntry(index);

  if (status != EMBER_SUCCESS) {
    sl_zigbee_core_debug_print("%serase key %d: 0x%2X\n",
                               "ERROR: ",
                               index,
                               status);
  }
}

// Key Delete command
void keysDeleteCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t index = sl_cli_get_argument_uint8(arguments, 0);
  eraseKeyTableEntry(index);
}

// keys clear
void keysClearCommand(void)
{
#if EMBER_KEY_TABLE_SIZE
  uint8_t i;
  for (i = 0; i < EMBER_KEY_TABLE_SIZE; i++) {
    eraseKeyTableEntry(i);
  }
#endif
}

void cliBufferPrint(void)
{
  uint8_t cmdIndex = (appZclBuffer[0] & ZCL_MANUFACTURER_SPECIFIC_MASK) ? 4 : 2;
  zclCmdIsBuilt = true;
  sl_zigbee_core_debug_print("Msg: clus 0x%04X, cmd 0x%02X, len %d\n",
                             globalApsFrame.clusterId,
                             appZclBuffer[cmdIndex],
                             appZclBufferLen);
  sl_zigbee_core_debug_print("buffer: ");
  emberAfGuaranteedPrintBuffer(appZclBuffer, appZclBufferLen, true);
  sl_zigbee_core_debug_print("\n");
}

void zclBufferSetup(uint8_t frameType, uint16_t clusterId, uint8_t commandId)
{
  uint8_t index = 0;
  emAfApsFrameClusterIdSetup(clusterId);
  appZclBuffer[index++] = (frameType
                           | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
                           | (mfgSpecificId != EMBER_AF_NULL_MANUFACTURER_CODE
                              ? ZCL_MANUFACTURER_SPECIFIC_MASK
                              : 0)
                           | (disableDefaultResponse
                              ? ZCL_DISABLE_DEFAULT_RESPONSE_MASK
                              : 0));
  if (mfgSpecificId != EMBER_AF_NULL_MANUFACTURER_CODE) {
    appZclBuffer[index++] = (uint8_t)mfgSpecificId;
    appZclBuffer[index++] = (uint8_t)(mfgSpecificId >> 8);
  }
  appZclBuffer[index++] = emberAfNextSequence();
  appZclBuffer[index++] = commandId;
  appZclBufferLen = index;
}

void printTimeCommand(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  emberAfPrintTime(emberAfGetCurrentTime());
}

void emAfCliBsendCommand(sl_cli_command_arg_t *arguments)
{
#if (EMBER_BINDING_TABLE_SIZE > 0)
  uint8_t srcEndpointToUse;
  EmberStatus status;

  // check that cmd is built
  if (zclCmdIsBuilt == false) {
    emberAfCorePrintln("cmd not built");
    return;
  }

  srcEndpointToUse = sl_cli_get_argument_uint8(arguments, 0);

  emberAfCorePrintln("src ep %x, clus %2x",
                     srcEndpointToUse,
                     globalApsFrame.clusterId);

  globalApsFrame.sourceEndpoint = srcEndpointToUse;
  if (useMulticastBinding) {
    emberAfCorePrintln("sending to multicast bind");
    status = emberAfSendMulticastToBindings(&globalApsFrame,
                                            appZclBufferLen,
                                            appZclBuffer);
  } else {
    // find a binding to send on
    status = emberAfSendUnicastToBindings(&globalApsFrame,
                                          appZclBufferLen,
                                          appZclBuffer);
  }
#if (defined(SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT) && (SL_ZIGBEE_DEBUG_ZCL_GROUP_ENABLED == 1) && (SL_ZIGBEE_DEBUG_PRINTS_ZCL_LEGACY_AF_DEBUG_ENABLED == 1))
  emberAfDebugPrintln("T%4x:TX (%p) %ccast 0x%x%p",
                      emberAfGetCurrentTime(),
                      "CLI",
                      'U',
                      status,
                      ((globalApsFrame.options & EMBER_APS_OPTION_ENCRYPTION)
                       ? " w/ link key" : ""));
  emberAfDebugPrint("TX buffer: [");
  emberAfDebugFlush();
  emberAfDebugPrintBuffer(appZclBuffer, appZclBufferLen, true);
  emberAfDebugPrintln("]");
  emberAfDebugFlush();
#else
  UNUSED_VAR(status);
#endif // (defined(SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT) && (SL_ZIGBEE_DEBUG_ZCL_GROUP_ENABLED == 1) && (SL_ZIGBEE_DEBUG_PRINTS_ZCL_LEGACY_AF_DEBUG_ENABLED == 1))
#else
  emberAfCorePrintln("Error: binding table size is 0");
#endif // (EMBER_BINDING_TABLE_SIZE > 0)
}

// ******************************************************
// send <id> <src endpoint> <dst endpoint>
// send_multicast <group id> <src endpoint>
//
// FFFC = all routers
// FFFD = all non-sleepy
// FFFF = all devices, include sleepy
// ******************************************************
void sli_zigbee_cli_send_command(sl_cli_command_arg_t *arguments)
{
  uint16_t destination = sl_cli_get_argument_uint16(arguments, 0);
  uint8_t srcEndpoint = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t dstEndpoint = sl_cli_get_argument_count(arguments) > 2 ? sl_cli_get_argument_uint8(arguments, 2) : EMBER_BROADCAST_ENDPOINT;
  uint8_t position = sl_cli_get_command_count(arguments) - 1;
  uint8_t *commandName = (uint8_t *) sl_cli_get_command_string(arguments, position);
  EmberStatus status;
  uint8_t label;

  // check that cmd is built
  if (zclCmdIsBuilt == false) {
    emberAfCorePrintln("no cmd");
    return;
  }

  emAfApsFrameEndpointSetup(srcEndpoint, dstEndpoint);

  if (emberAfPreCliSendCallback(&globalApsFrame,
                                emberAfGetNodeId(),
                                destination,
                                appZclBuffer,
                                appZclBufferLen)) {
    return;
  }

  if (commandName[4] == '_') {
    label = 'M';
    status = emberAfSendMulticast(destination,
                                  &globalApsFrame,
                                  appZclBufferLen,
                                  appZclBuffer);
  } else if (destination >= EMBER_BROADCAST_ADDRESS) {
    label = 'B';
    status = emberAfSendBroadcast(destination,
                                  &globalApsFrame,
                                  appZclBufferLen,
                                  appZclBuffer);
  } else {
    label = 'U';
    status = emberAfSendUnicast(EMBER_OUTGOING_DIRECT,
                                destination,
                                &globalApsFrame,
                                appZclBufferLen,
                                appZclBuffer);
  }

  if (status != EMBER_SUCCESS) {
    emberAfCorePrintln("Error: CLI Send failed, status: 0x%X", status);
  }
  UNUSED_VAR(label);
  sl_zigbee_core_debug_print("T%04x:TX (%s) %ccast 0x%02x%s\n",
                             emberAfGetCurrentTime(),
                             "CLI",
                             label,
                             status,
                             ((globalApsFrame.options & EMBER_APS_OPTION_ENCRYPTION)
                              ? " w/ link key" : ""));
  sl_zigbee_core_debug_print("TX buffer: [");
  emberAfDebugFlush();
  emberAfDebugPrintBuffer(appZclBuffer, appZclBufferLen, true);
  emberAfDebugPrintln("]");
  emberAfDebugFlush();

  zclCmdIsBuilt = false;
  mfgSpecificId = EMBER_AF_NULL_MANUFACTURER_CODE;
  disableDefaultResponse = 0;
}

// read <endpoint:1> <cluster:2> <attribute:2> <mask:1>
void sli_zigbee_zcl_read_cli_command(sl_cli_command_arg_t *arguments)
{
  EmberAfStatus status;
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  EmberAfClusterId cluster = (EmberAfClusterId)sl_cli_get_argument_uint16(arguments, 1);
  EmberAfAttributeId attribute = (EmberAfAttributeId)sl_cli_get_argument_uint16(arguments, 2);
  bool serverAttribute = (bool)sl_cli_get_argument_uint8(arguments, 3);
  uint8_t data[ATTRIBUTE_LARGEST];
  uint8_t dataType;

  emberAfCorePrint("%p: ep: %d, cl: 0x%2X, attr: 0x%2X",
                   "read",
                   endpoint,
                   cluster,
                   attribute);
  emberAfCorePrintln(", svr: %c", (serverAttribute ? 'y' : 'n'));

  status = emberAfReadAttribute(endpoint,
                                cluster,
                                attribute,
                                (serverAttribute
                                 ? CLUSTER_MASK_SERVER
                                 : CLUSTER_MASK_CLIENT),
                                data,
                                sizeof(data),
                                &dataType);
  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    if (emberAfIsStringAttributeType(dataType)) {
      emberAfCorePrintString(data);
    } else if (emberAfIsLongStringAttributeType(dataType)) {
      emberAfCoreDebugExec(emberAfPrintLongString(EMBER_AF_PRINT_CORE, data));
    } else {
      emberAfCorePrintBuffer(data, emberAfGetDataSize(dataType), true);
    }
    emberAfCorePrintln("");
  } else {
    emberAfCorePrintln("%p: read: 0x%x", "Error", status);
  }
}
// ******************************************************
// write <ep> <cluster> <attrID> <mask> <dataType> <data bytes>
// ******************************************************
void sli_zigbee_zcl_write_cli_command(sl_cli_command_arg_t *arguments)
{
  //uint8_t i;
  EmberAfStatus status;
  // Ensure data[] is at least two bytes to accommodate possibility
  // of zero-length LONG string having two-byte length prefix.
  uint8_t data[ATTRIBUTE_LARGEST + 1];

  uint8_t  endpoint  = sl_cli_get_argument_uint8(arguments, 0);
  uint16_t cluster   = sl_cli_get_argument_uint16(arguments, 1);
  uint16_t attribute = sl_cli_get_argument_uint16(arguments, 2);
  bool serverAttribute = sl_cli_get_argument_uint8(arguments, 3);
  uint8_t  dataType  = sl_cli_get_argument_uint8(arguments, 4);

  emberAfCorePrint("%p: ep: %d, cl: 0x%2X, attr: 0x%2X",
                   "write",
                   endpoint,
                   cluster,
                   attribute);
  emberAfCorePrintln(", svr: %c, dtype: 0x%X",
                     (serverAttribute ? 'y' : 'n'),
                     dataType);

  // If the data type is a string, automatically prepend a length to the data;
  // otherwise, just copy the raw bytes.
  MEMSET(data, 0, ATTRIBUTE_LARGEST);
  if (emberAfIsStringAttributeType(dataType)) {
    // string, 1-byte length prefix
    data[0] = sl_zigbee_copy_hex_arg(arguments,
                                     5,
                                     data + 1,
                                     ATTRIBUTE_LARGEST - 1,
                                     false);
  } else if (emberAfIsLongStringAttributeType(dataType)) {
    // LONG string with 2-byte length prefix.
    // CLI restricts string length to <= 255, so returned length will go
    // into lower prefix byte and upper prefix byte will be zero.
    data[0] = sl_zigbee_copy_hex_arg(arguments,
                                     5,
                                     data + 2,
                                     ATTRIBUTE_LARGEST - 2,
                                     false);
#if (ATTRIBUTE_LARGEST < 1)
#error "ATTRIBUTE_LARGEST should be greater greater than 0"
#endif
    data[1] = 0;
  } else {
    sl_zigbee_copy_hex_arg(arguments,
                           5,
                           data,
                           emberAfGetDataSize(dataType),
                           false);
  }

  status = emberAfWriteAttribute(endpoint,
                                 cluster,
                                 attribute,
                                 (serverAttribute
                                  ? CLUSTER_MASK_SERVER
                                  : CLUSTER_MASK_CLIENT),
                                 data,
                                 dataType);
  emAfPrintStatus("write", status);
  emberAfCorePrintln("");
}

void sli_zigbee_zcl_buffer_add_byte(uint8_t byte)
{
  appZclBuffer[appZclBufferLen] = byte;
  appZclBufferLen += 1;
}

void sli_zigbee_zcl_buffer_add_word(uint16_t word)
{
  sli_zigbee_zcl_buffer_add_byte(LOW_BYTE(word));
  sli_zigbee_zcl_buffer_add_byte(HIGH_BYTE(word));
}

void sli_zigbee_zcl_buffer_add_int32(uint32_t value)
{
  uint8_t i;
  for (i = 0; i < 4; i++) {
    sli_zigbee_zcl_buffer_add_byte(LOW_BYTE(value));
    value = value >> 8;
  }
}

void sli_zigbee_zcl_buffer_add_buffer(const uint8_t *buffer, uint8_t length)
{
  MEMMOVE(appZclBuffer + appZclBufferLen, buffer, length);
  appZclBufferLen += length;
}

// Made non-to remove warnings
void sli_zigbee_zcl_buffer_add_string(const uint8_t *buffer)
{
  sli_zigbee_zcl_buffer_add_buffer(buffer, emberAfStringLength(buffer) + 1);
}

// Writes the length and contents of the string found at argIndex
// into the zcl buffer. Takes into account whether string is non-LONG
// (1-byte length prefix) or LONG (2-byte length prefix).
void sli_zigbee_zcl_buffer_add_length_and_string(sl_cli_command_arg_t *arguments,
                                                 uint8_t argIndex,
                                                 bool isLongStringType)
{
  uint8_t prefixSize = (isLongStringType ? 2 : 1);
  // We protect against copying outside of the appZclBuffer
  // by passing the maximum length of the buffer, or a maximum
  // value of an uint8_t.
  uint16_t maxLength = APP_ZCL_BUFFER_SIZE - (appZclBufferLen + prefixSize);
  if (maxLength > MAX_INT8U_VALUE) {
    maxLength = MAX_INT8U_VALUE;
  }
  uint8_t length = sl_zigbee_copy_string_arg(arguments,
                                             argIndex,
                                             appZclBuffer + appZclBufferLen + prefixSize,
                                             maxLength,
                                             false);
  appZclBuffer[appZclBufferLen] = length;
  if (prefixSize == 2) {
    // CLI max string length is 255, so LONG string upper length byte is zero.
    appZclBuffer[appZclBufferLen + 1] = 0;
  }
  appZclBufferLen += length + prefixSize;
}

/**
 * @brief
 * Given a 32-bit value and a specified number of bits fewer than 32, check that
 * the value does not exceed the range of the specified number of bits.
 * @param val
 * @param bits
 * @return true if value in range else return false
 */
static bool is_zcl_data_type_in_range(uint32_t val, uint8_t bits)
{
  if (bits == 0 || bits > 31) {
    return false;
  }
  // Construct mask of the container's unused higher order bits.
  uint32_t unusedBitsMask = ~((1u << bits) - 1u);
  // Return true if the unused higher order bits are all zero.
  return (unusedBitsMask & val) == 0;
}

/**
 * @brief
 * Given a value then add n number of bytes into the zcl buffer from that value
 * @param val
 * @param noOfBytes
 */
static void add_bytes_to_zcl_buffer(uint32_t val, uint8_t noOfBytes)
{
  for (uint8_t i = 0; i < noOfBytes; i++) {
    uint8_t byteN = (uint8_t)(val >> (8u * i));
    sli_zigbee_zcl_buffer_add_byte(byteN);
  }
}

// Handles any zcl command where the argument list of the
// command is simply appended to the zcl buffer.  Handles argument types
// mentioned in the switch case below.  String arguments are written with
// a length byte first.
void sli_zigbee_zcl_simple_command(uint8_t frameControl,
                                   uint16_t clusterId,
                                   uint8_t commandId,
                                   sl_cli_command_arg_t *arguments,
                                   uint8_t *argumentTypes)
{
  uint8_t argumentIndex;
  uint8_t count = sl_cli_get_argument_count(arguments);
  uint8_t type;
  uint8_t typeIndex = 0;
  uint8_t *hex_value = 0;
  size_t hex_length = 0;

  zclBufferSetup(frameControl, clusterId, commandId);
  for (argumentIndex = 0; argumentIndex < count; argumentIndex++) {
    type = argumentTypes[typeIndex];
    // For zcl cli array arguments are referenced as optional arguments.
    // Therefore the type of the arguments should remain the same for them.
    if (type != SL_ZCL_CLI_ARG_UINT8OPT
        && type != SL_ZCL_CLI_ARG_UINT16OPT
        && type != SL_ZCL_CLI_ARG_UINT24OPT
        && type != SL_ZCL_CLI_ARG_UINT32OPT
        && type != SL_ZCL_CLI_ARG_UINT40OPT
        && type != SL_ZCL_CLI_ARG_UINT48OPT
        && type != SL_ZCL_CLI_ARG_UINT56OPT
        && type != SL_ZCL_CLI_ARG_UINT64OPT
        && type != SL_ZCL_CLI_ARG_INT8OPT
        && type != SL_ZCL_CLI_ARG_INT16OPT
        && type != SL_ZCL_CLI_ARG_INT24OPT
        && type != SL_ZCL_CLI_ARG_INT32OPT
        && type != SL_ZCL_CLI_ARG_INT40OPT
        && type != SL_ZCL_CLI_ARG_INT48OPT
        && type != SL_ZCL_CLI_ARG_INT56OPT
        && type != SL_ZCL_CLI_ARG_INT64OPT
        && type != SL_ZCL_CLI_ARG_STRINGOPT
        && type != SL_ZCL_CLI_ARG_HEXOPT) {
      typeIndex++;
    }
    switch (type) {
      case SL_ZCL_CLI_ARG_UINT8:
      case SL_ZCL_CLI_ARG_UINT8OPT:
        sli_zigbee_zcl_buffer_add_byte((uint8_t)sl_cli_get_argument_uint8(arguments, argumentIndex));
        break;
      case SL_ZCL_CLI_ARG_UINT16:
      case SL_ZCL_CLI_ARG_UINT16OPT:
        sli_zigbee_zcl_buffer_add_word(sl_cli_get_argument_uint16(arguments, argumentIndex));
        break;
      case SL_ZCL_CLI_ARG_UINT24:
      case SL_ZCL_CLI_ARG_UINT24OPT:
        if (is_zcl_data_type_in_range(sl_cli_get_argument_uint32(arguments, argumentIndex), 24u)) {
          add_bytes_to_zcl_buffer(sl_cli_get_argument_uint32(arguments, argumentIndex), 3);
        } else {
          emberAfAppPrintln("Argument at index: %d is out of range", argumentIndex);
          goto kickout;
        }
        break;
      case SL_ZCL_CLI_ARG_UINT32:
      case SL_ZCL_CLI_ARG_UINT32OPT:
        sli_zigbee_zcl_buffer_add_int32(sl_cli_get_argument_uint32(arguments, argumentIndex));
        break;
      case SL_ZCL_CLI_ARG_INT8:
      case SL_ZCL_CLI_ARG_INT8OPT:
        sli_zigbee_zcl_buffer_add_byte((uint8_t)sl_cli_get_argument_int8(arguments, argumentIndex));
        break;
      case SL_ZCL_CLI_ARG_INT16:
      case SL_ZCL_CLI_ARG_INT16OPT:
        sli_zigbee_zcl_buffer_add_word(sl_cli_get_argument_int16(arguments, argumentIndex));
        break;
      case SL_ZCL_CLI_ARG_INT24:
      case SL_ZCL_CLI_ARG_INT24OPT:
        if (is_zcl_data_type_in_range(sl_cli_get_argument_int32(arguments, argumentIndex), 24u)) {
          add_bytes_to_zcl_buffer(sl_cli_get_argument_int32(arguments, argumentIndex), 3);
        } else {
          emberAfAppPrintln("Argument at index: %d is out of range", argumentIndex);
          goto kickout;
        }
        break;
      case SL_ZCL_CLI_ARG_INT32:
      case SL_ZCL_CLI_ARG_INT32OPT:
        sli_zigbee_zcl_buffer_add_int32(sl_cli_get_argument_int32(arguments, argumentIndex));
        break;
      case SL_ZCL_CLI_ARG_HEX:
      case SL_ZCL_CLI_ARG_HEXOPT:
        hex_value = sl_cli_get_argument_hex(arguments, argumentIndex, &hex_length);
        for (uint8_t i = 0; i < hex_length; i++) {
          appZclBuffer[appZclBufferLen + i] = hex_value[i];
        }
        appZclBufferLen += hex_length;
        break;
      case SL_ZCL_CLI_ARG_STRING:
      case SL_ZCL_CLI_ARG_STRINGOPT:
        sli_zigbee_zcl_buffer_add_length_and_string(arguments, argumentIndex, false);
        break;
      default:
        goto kickout;
    }
  }
  kickout:
  cliBufferPrint();
}

void sli_zigbee_cli_zcl_mfg_code_command(sl_cli_command_arg_t *arguments)
{
  if (zclCmdIsBuilt) {
    emberAfAppPrintln("Command already built.  Cannot set MFG specific code for command.");
    return;
  }
  mfgSpecificId = sl_cli_get_argument_uint16(arguments, 0);
  emberAfAppPrintln("MFG Code Set for next command: 0x%2X", mfgSpecificId);
}

void sli_zigbee_cli_zcl_time_command(sl_cli_command_arg_t *arguments)
{
  emberAfSetTime(sl_cli_get_argument_uint32(arguments, 0));
}

void sli_zigbee_cli_zcl_use_next_sequence_command(sl_cli_command_arg_t *arguments)
{
  useNextSequence = sl_cli_get_argument_uint8(arguments, 0);
  emberAfAppPrintln("Use Next Sequence Set for next command: 0x%X", useNextSequence);
}

void sli_zigbee_cli_zcl_x_default_resp_command(sl_cli_command_arg_t *arguments)
{
  disableDefaultResponse = sl_cli_get_argument_uint8(arguments, 0);
  emberAfAppPrintln("Disable Default Response Set for next command: 0x%X", disableDefaultResponse);
}

// zcl test response on
void sli_zigbee_cli_zcl_test_response_on_command(sl_cli_command_arg_t *arguments)
{
  emberAfSetNoReplyForNextMessage(false);
}

// zcl test response off
void sli_zigbee_cli_zcl_test_response_off_command(sl_cli_command_arg_t *arguments)
{
  emberAfSetNoReplyForNextMessage(true);
}

#if defined(ZCL_USING_IDENTIFY_CLUSTER_SERVER) || defined(ZCL_USING_IDENTIFY_CLUSTER_CLIENT)
static void sli_zigbee_cli_zcl_identify_on_off_command(sl_cli_command_arg_t *arguments, uint16_t timeS)
{
  EmberAfStatus afStatus = emberAfWriteServerAttribute(
    sl_cli_get_argument_uint8(arguments, 0),                       // endpoint
    ZCL_IDENTIFY_CLUSTER_ID,
    ZCL_IDENTIFY_TIME_ATTRIBUTE_ID,
    (uint8_t *)&timeS,
    ZCL_INT16U_ATTRIBUTE_TYPE);
  if (EMBER_ZCL_STATUS_SUCCESS != afStatus) {
    emberAfDebugPrintln("Identify On/Off: failed to write value 0x%x  to cluster "
                        "0x%x attribute ID 0x%x: error 0x%x",
                        timeS,
                        ZCL_IDENTIFY_CLUSTER_ID,
                        ZCL_IDENTIFY_TIME_ATTRIBUTE_ID,
                        afStatus);
  }
}

// zcl identify on <endpoint:1> <time:2>
void sli_zigbee_cli_zcl_identify_on_command(sl_cli_command_arg_t *arguments)
{
  uint16_t timeS = 0;
  timeS = sl_cli_get_argument_uint16(arguments, 1);
  if (timeS == 0) {
    timeS = 60;
  }
  sli_zigbee_cli_zcl_identify_on_off_command(arguments, timeS);
}

// zcl identify off <endpoint:1>
void sli_zigbee_cli_zcl_identify_off_command(sl_cli_command_arg_t *arguments)
{
  sli_zigbee_cli_zcl_identify_on_off_command(arguments, 0);
}
#endif // ZCL_USING_IDENTIFY_CLUSTER_SERVER

#if defined(ZCL_USING_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_SERVER) || defined(ZCL_USING_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_CLIENT)
// zcl drlc lce <eventId:4> <start:4> <duration:2> <event control:1>
void sli_zigbee_cli_zcl_drlc_load_control_event_command(sl_cli_command_arg_t *arguments)
{
  zclBufferSetup(ZCL_CLUSTER_SPECIFIC_COMMAND
                 | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT,
                 ZCL_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_ID,
                 ZCL_LOAD_CONTROL_EVENT_COMMAND_ID);
  sli_zigbee_zcl_buffer_add_int32(sl_cli_get_argument_uint32(arguments, 0));  // event id
  sli_zigbee_zcl_buffer_add_word(0x0fff);          // all device classes
  sli_zigbee_zcl_buffer_add_byte((uint8_t)sl_cli_get_argument_uint8(arguments, 1)); // UEG
  sli_zigbee_zcl_buffer_add_int32(sl_cli_get_argument_uint32(arguments, 2));  // start
  sli_zigbee_zcl_buffer_add_word(sl_cli_get_argument_uint16(arguments, 3)); // duration
  sli_zigbee_zcl_buffer_add_byte(1);               // criticality level, normal
  sli_zigbee_zcl_buffer_add_byte(0);               // 1.1c cooling temp offset
  sli_zigbee_zcl_buffer_add_byte(0);               // 1.1c heating temp offset
  sli_zigbee_zcl_buffer_add_word(0x1a09);          // cool temp set point 23.3c
  sli_zigbee_zcl_buffer_add_word(0x1a09);          // heat temp set point
  sli_zigbee_zcl_buffer_add_byte(0x0a);            // -10% avg load percent
  sli_zigbee_zcl_buffer_add_byte(0);               // duty cycle
  sli_zigbee_zcl_buffer_add_byte((uint8_t)sl_cli_get_argument_uint8(arguments, 4));   // event control
  globalApsFrame.options |= EMBER_APS_OPTION_SOURCE_EUI64;
  cliBufferPrint();
}
#endif

#if defined(ZCL_USING_BACNET_PROTOCOL_TUNNEL_CLUSTER_CLIENT) || defined(ZCL_USING_BACNET_PROTOCOL_TUNNEL_CLUSTER_SERVER)
// zcl bacnet transfer-npdu whois
void sli_zigbee_cli_zcl_bacnet_transfer_whois_command(sl_cli_command_arg_t *arguments)
{
  zclBufferSetup(ZCL_CLUSTER_SPECIFIC_COMMAND
                 | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER,
                 ZCL_BACNET_PROTOCOL_TUNNEL_CLUSTER_ID,
                 ZCL_TRANSFER_NPDU_COMMAND_ID);
  //zclBufferAddByte(0);            // what is this?
  sli_zigbee_zcl_buffer_add_int32(0xFFFF2001);
  sli_zigbee_zcl_buffer_add_int32(0x0810FF00);
  cliBufferPrint();
}
#endif

#if defined(ZCL_USING_TUNNELING_CLUSTER_CLIENT) || defined(ZCL_USING_TUNNELING_CLUSTER_SERVER)
// zcl tunneling random-to-server <tunnel id:2> <length:2>
void sli_zigbee_cli_zcl_tunneling_random_to_server_command(sl_cli_command_arg_t *arguments)
{
  uint16_t length = sl_cli_get_argument_uint16(arguments, 1); // length of random data
  zclBufferSetup(ZCL_CLUSTER_SPECIFIC_COMMAND
                 | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER,
                 ZCL_TUNNELING_CLUSTER_ID,
                 ZCL_TRANSFER_DATA_CLIENT_TO_SERVER_COMMAND_ID);
  sli_zigbee_zcl_buffer_add_word(sl_cli_get_argument_uint16(arguments, 0)); // tunnel id
  appZclBufferLen = (appZclBufferLen + length < APP_ZCL_BUFFER_SIZE
                     ? appZclBufferLen + length
                     : APP_ZCL_BUFFER_SIZE);
  globalApsFrame.options |= EMBER_APS_OPTION_SOURCE_EUI64;
  cliBufferPrint();
}

// zcl tunneling transfer-to-server <tunnel id:2> <data>
void sli_zigbee_cli_zcl_tunneling_transfer_to_server_command(sl_cli_command_arg_t *arguments)
{
  zclBufferSetup(ZCL_CLUSTER_SPECIFIC_COMMAND
                 | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER,
                 ZCL_TUNNELING_CLUSTER_ID,
                 ZCL_TRANSFER_DATA_CLIENT_TO_SERVER_COMMAND_ID);
  sli_zigbee_zcl_buffer_add_word(sl_cli_get_argument_uint16(arguments, 0));              // tunnel id
  appZclBufferLen += sl_zigbee_copy_string_arg(arguments,
                                               1, // data
                                               appZclBuffer + appZclBufferLen,
                                               0xFF, // copy as much data as possible
                                               false); // no padding
  globalApsFrame.options |= EMBER_APS_OPTION_SOURCE_EUI64;
  cliBufferPrint();
}

// zcl tunneling transfer-to-client <tunnel id:2> <data>
void sli_zigbee_cli_zcl_tunneling_transfer_to_client_command(sl_cli_command_arg_t *arguments)
{
  zclBufferSetup(ZCL_CLUSTER_SPECIFIC_COMMAND
                 | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT,
                 ZCL_TUNNELING_CLUSTER_ID,
                 ZCL_TRANSFER_DATA_SERVER_TO_CLIENT_COMMAND_ID);
  sli_zigbee_zcl_buffer_add_word(sl_cli_get_argument_uint16(arguments, 0));              // tunnel id
  appZclBufferLen += sl_zigbee_copy_string_arg(arguments,
                                               1, // data
                                               appZclBuffer + appZclBufferLen,
                                               0xFF, // copy as much data as possible
                                               false); // no padding
  globalApsFrame.options |= EMBER_APS_OPTION_SOURCE_EUI64;
  cliBufferPrint();
}

// zcl tunneling random-to-client <tunnel id:2> <length:2>
void sli_zigbee_cli_zcl_tunneling_random_to_client_command(sl_cli_command_arg_t *arguments)
{
  uint16_t length = sl_cli_get_argument_uint16(arguments, 1); // length of random data
  zclBufferSetup(ZCL_CLUSTER_SPECIFIC_COMMAND
                 | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT,
                 ZCL_TUNNELING_CLUSTER_ID,
                 ZCL_TRANSFER_DATA_SERVER_TO_CLIENT_COMMAND_ID);
  sli_zigbee_zcl_buffer_add_word(sl_cli_get_argument_uint16(arguments, 0)); // tunnel id
  appZclBufferLen = (appZclBufferLen + length < APP_ZCL_BUFFER_SIZE
                     ? appZclBufferLen + length
                     : APP_ZCL_BUFFER_SIZE);
  globalApsFrame.options |= EMBER_APS_OPTION_SOURCE_EUI64;
  cliBufferPrint();
}
#endif

// zcl global disc-com-gen <cluster id:2> <start command id:1> <max command id:1>
void sli_zigbee_cli_zcl_global_command_discovery_generated_command(sl_cli_command_arg_t *arguments)
{
  zclBufferSetup(ZCL_GLOBAL_COMMAND | zcl_global_direction,
                 (EmberAfClusterId)sl_cli_get_argument_uint16(arguments, 0),   // cluster id
                 ZCL_DISCOVER_COMMANDS_GENERATED_COMMAND_ID);
  sli_zigbee_zcl_buffer_add_byte((uint8_t)sl_cli_get_argument_uint8(arguments, 1)); // start command id
  sli_zigbee_zcl_buffer_add_byte((uint8_t)sl_cli_get_argument_uint8(arguments, 2)); // max command ids
  cliBufferPrint();
}

// zcl global disc-com-rec <cluster id:2> <start command id:1> <max command id:1>
void sli_zigbee_cli_zcl_global_command_discovery_received_command(sl_cli_command_arg_t *arguments)
{
  zclBufferSetup(ZCL_GLOBAL_COMMAND | zcl_global_direction,
                 (EmberAfClusterId)sl_cli_get_argument_uint16(arguments, 0),   // cluster id
                 ZCL_DISCOVER_COMMANDS_RECEIVED_COMMAND_ID);
  sli_zigbee_zcl_buffer_add_byte((uint8_t)sl_cli_get_argument_uint8(arguments, 1)); // start command id
  sli_zigbee_zcl_buffer_add_byte((uint8_t)sl_cli_get_argument_uint8(arguments, 2)); // max command ids
  cliBufferPrint();
}

// zcl global direction <direction:1>
void sli_zigbee_cli_zcl_global_direction_command(sl_cli_command_arg_t *arguments)
{
  zcl_global_direction = ((((uint8_t)sl_cli_get_argument_uint8(arguments, 0))
                           == ZCL_DIRECTION_CLIENT_TO_SERVER)
                          ? ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
                          : ZCL_FRAME_CONTROL_SERVER_TO_CLIENT);
}

// zcl global expect-report-from-me <cluster id:2> <attribute id:2> <timeout:2>
void sli_zigbee_cli_zcl_global_expect_report_from_me_command(sl_cli_command_arg_t *arguments)
{
  zclBufferSetup(ZCL_GLOBAL_COMMAND | zcl_global_direction,
                 (EmberAfClusterId)sl_cli_get_argument_uint16(arguments, 0), // cluster id
                 ZCL_CONFIGURE_REPORTING_COMMAND_ID);
  sli_zigbee_zcl_buffer_add_byte(EMBER_ZCL_REPORTING_DIRECTION_RECEIVED);
  sli_zigbee_zcl_buffer_add_word(sl_cli_get_argument_uint16(arguments, 1)); // attribute id
  sli_zigbee_zcl_buffer_add_word(sl_cli_get_argument_uint16(arguments, 2)); // timeout
  cliBufferPrint();
}

// Common zcl global write api for write/nwrite/uwrite
static void zigbee_zcl_global_write_command_common(sl_cli_command_arg_t *arguments, uint8_t type_of_write)
{
  uint8_t type = sl_cli_get_argument_uint8(arguments, 2);
  uint8_t commandId = type_of_write;
  sli_zigbee_zcl_global_setup(commandId, arguments);

  if (emberAfIsThisDataTypeAStringType(type)) {
    uint8_t bytes_reserved_for_string_length = emberAfIsLongStringAttributeType(type) ? 2 : 1;
    size_t hex_array_size;
    sl_cli_get_argument_hex(arguments, 3, &hex_array_size);
    // Checking for ZCL Buffer Overflow by protecting the copy of anything
    // outside appZclBuffer
    uint16_t buffer_length_remaining = APP_ZCL_BUFFER_SIZE - (appZclBufferLen + bytes_reserved_for_string_length);
    if (hex_array_size > buffer_length_remaining) {
      hex_array_size = APP_ZCL_BUFFER_SIZE - (appZclBufferLen + bytes_reserved_for_string_length);
    }
    // Copy the string into the zclBuffer.
    sl_zigbee_copy_hex_arg(arguments,
                           3,
                           appZclBuffer + appZclBufferLen + bytes_reserved_for_string_length,
                           hex_array_size,
                           true);
    // In the zcl buffer, set the length of the string which precedes the string
    *(appZclBuffer + appZclBufferLen) = (uint8_t)hex_array_size;
    if (bytes_reserved_for_string_length == 2) {
      // Making sure the high length byte is written down as well in little endian format.
      *(appZclBuffer + appZclBufferLen + 1) = (uint8_t)(hex_array_size >> 8);
    }
    appZclBufferLen += hex_array_size + bytes_reserved_for_string_length;
  } else {
    uint8_t length = emberAfGetDataSize(type);
    sl_zigbee_copy_hex_arg(arguments,
                           3,
                           appZclBuffer + appZclBufferLen,
                           length,
                           true);    // pad with zeroes
    appZclBufferLen += length;
  }
  cliBufferPrint();
}
// zcl global write <cluster:2> <attrID:2> <type:1> <data>
void sli_zigbee_cli_zcl_global_write_command(sl_cli_command_arg_t *arguments)
{
  zigbee_zcl_global_write_command_common(arguments, ZCL_WRITE_ATTRIBUTES_COMMAND_ID);
}

// zcl global uwrite <cluster:2> <attrID:2> <type:1> <data>
void sli_zigbee_cli_zcl_global_uwrite_command(sl_cli_command_arg_t *arguments)
{
  zigbee_zcl_global_write_command_common(arguments, ZCL_WRITE_ATTRIBUTES_UNDIVIDED_COMMAND_ID);
}

// zcl global nwrite <cluster:2> <attrID:2> <type:1> <data>
void sli_zigbee_cli_zcl_global_nwrite_command(sl_cli_command_arg_t *arguments)
{
  zigbee_zcl_global_write_command_common(arguments, ZCL_WRITE_ATTRIBUTES_NO_RESPONSE_COMMAND_ID);
}

// zcl global discover <cluster> <attrID:2> <max # to report:1>
void sli_zigbee_cli_zcl_global_discover_command(sl_cli_command_arg_t *arguments)
{
  sli_zigbee_zcl_global_setup(ZCL_DISCOVER_ATTRIBUTES_COMMAND_ID, arguments);
  cliBufferPrint();
}

// zcl global read <cluster:2> <attribute:2>
void sli_zigbee_cli_zcl_global_read_command(sl_cli_command_arg_t *arguments)
{
  sli_zigbee_zcl_global_setup(ZCL_READ_ATTRIBUTES_COMMAND_ID, arguments);
  cliBufferPrint();
}

// zcl global report <src endpoint id:1> <cluster id:2> <attribute id:2> <mask:1>
void sli_zigbee_cli_zcl_global_report_command(sl_cli_command_arg_t *arguments)
{
  EmberAfStatus status;
  EmberAfClusterId clusterId = (EmberAfClusterId)sl_cli_get_argument_uint16(arguments, 1);
  EmberAfAttributeId attributeId = (EmberAfAttributeId)sl_cli_get_argument_uint16(arguments, 2);
  uint8_t mask = (uint8_t)sl_cli_get_argument_uint8(arguments, 3);
  EmberAfAttributeType type;
  uint16_t size;
  uint8_t data[ATTRIBUTE_LARGEST];

  status = emberAfReadAttribute((uint8_t)sl_cli_get_argument_uint8(arguments, 0), // endpoint
                                clusterId,
                                attributeId,
                                (mask == 0
                                 ? CLUSTER_MASK_CLIENT
                                 : CLUSTER_MASK_SERVER),
                                data,
                                sizeof(data),
                                &type);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfReportingPrintln("ERR: reading attribute %x", status);
    return;
  }

  zclBufferSetup(ZCL_GLOBAL_COMMAND
                 | (mask == 0
                    ? ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
                    : ZCL_FRAME_CONTROL_SERVER_TO_CLIENT),
                 clusterId,
                 ZCL_REPORT_ATTRIBUTES_COMMAND_ID);
  sli_zigbee_zcl_buffer_add_word(attributeId);
  sli_zigbee_zcl_buffer_add_byte(type);

  size = emberAfAttributeValueSize(type, data, sizeof(data));
  if (size == 0 || size > (APP_ZCL_BUFFER_SIZE - appZclBufferLen)) {
    emberAfReportingPrintln("ERR: attribute size %d too large for buffer", size);
    return;
  }
#if (BIGENDIAN_CPU)
  if (isThisDataTypeSentLittleEndianOTA(type)) {
    emberReverseMemCopy(appZclBuffer + appZclBufferLen, data, size);
  } else {
    MEMMOVE(appZclBuffer + appZclBufferLen, data, size);
  }
#else
  MEMMOVE(appZclBuffer + appZclBufferLen, data, size);
#endif
  appZclBufferLen += size;

  cliBufferPrint();
}

// zcl global report-read <cluster> <attrID:2> <direction:1>
void sli_zigbee_cli_zcl_global_report_read_command(sl_cli_command_arg_t *arguments)
{
  zclBufferSetup(ZCL_GLOBAL_COMMAND | zcl_global_direction,
                 (EmberAfClusterId)sl_cli_get_argument_uint16(arguments, 0), // cluster id
                 ZCL_READ_REPORTING_CONFIGURATION_COMMAND_ID);
  sli_zigbee_zcl_buffer_add_byte(sl_cli_get_argument_uint8(arguments, 2)); // direction
  sli_zigbee_zcl_buffer_add_word(sl_cli_get_argument_uint16(arguments, 1)); // attribute id
  cliBufferPrint();
}

// zcl global send-me-a-report <cluster id:2> <attribute id:2> <type:1> ...
// ... <min interval:2> <max interval:2> <reportable change:1-4>
void sli_zigbee_cli_zcl_global_send_me_a_report_command(sl_cli_command_arg_t *arguments)
{
  uint8_t type = (uint8_t)sl_cli_get_argument_uint8(arguments, 2);
  uint16_t attributeId = sl_cli_get_argument_uint16(arguments, 1);
  zclBufferSetup(ZCL_GLOBAL_COMMAND | zcl_global_direction,
                 (EmberAfClusterId)sl_cli_get_argument_uint16(arguments, 0), // cluster id
                 ZCL_CONFIGURE_REPORTING_COMMAND_ID);
  sli_zigbee_zcl_buffer_add_byte(EMBER_ZCL_REPORTING_DIRECTION_REPORTED);
  sli_zigbee_zcl_buffer_add_word(attributeId);
  sli_zigbee_zcl_buffer_add_byte(type);           // type
  sli_zigbee_zcl_buffer_add_word(sl_cli_get_argument_uint16(arguments, 3));  // minimum reporting interval
  sli_zigbee_zcl_buffer_add_word(sl_cli_get_argument_uint16(arguments, 4));  // maximum reporting interval

  // If the data type is analog, then the reportable change field is the same
  // size as the data type.  Otherwise, it is omitted.
  if (emberAfGetAttributeAnalogOrDiscreteType(type)
      == EMBER_AF_DATA_TYPE_ANALOG) {
    uint8_t dataSize = emberAfGetDataSize(type);
    size_t len = dataSize;
    uint8_t *ptr_string = sl_cli_get_argument_hex(arguments, 5, &len);
    if (len != dataSize) {
      printf("Error: attribute ID 0x%04X reportable change is size %dB, but %dB entered\n",
             attributeId,
             dataSize,
             len);
      return;
    }
    MEMMOVE(appZclBuffer + appZclBufferLen,
            ptr_string,
            len);
    appZclBufferLen += dataSize;
  }
  cliBufferPrint();
}

void sli_zigbee_zcl_global_setup(uint8_t commandId, sl_cli_command_arg_t *arguments)
{
  uint16_t clusterId = (uint16_t)sl_cli_get_argument_uint16(arguments, 0);
  zclBufferSetup(ZCL_GLOBAL_COMMAND | zcl_global_direction,
                 clusterId,
                 commandId);
  sli_zigbee_zcl_buffer_add_word(sl_cli_get_argument_uint16(arguments, 1));  // attr id
  if (commandId != ZCL_READ_ATTRIBUTES_COMMAND_ID) {
    sli_zigbee_zcl_buffer_add_byte((uint8_t)sl_cli_get_argument_uint8(arguments, 2));
  }
}

// for sending raw (user defined) ZCL messages
// raw <cluster> <data bytes>
// note: limited to 0xff bytes in length

void emAfCliRawCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t seqNumIndex;
  uint8_t length;
  emAfApsFrameClusterIdSetup(sl_cli_get_argument_uint16(arguments, 0));
  length = sl_zigbee_copy_hex_arg(arguments, 1, appZclBuffer, APP_ZCL_BUFFER_SIZE_CLI, false);
  seqNumIndex = (appZclBuffer[0] & ZCL_MANUFACTURER_SPECIFIC_MASK) ? 3 : 1;
  if (useNextSequence) {
    appZclBuffer[seqNumIndex] = emberAfNextSequence();
  }
  appZclBufferLen = length;
  cliBufferPrint();
}

void emAfCliSendUsingMulticastBindingCommand(sl_cli_command_arg_t *arguments)
{
  useMulticastBinding = (bool)sl_cli_get_argument_uint8(arguments, 0);
}

// timesync <id> <src endpoint> <dest endpoint>
//
// This sends a read attr for the time of the device specified. It sets a flag
// so when it gets the response it writes the time to its own time attr
void emAfCliTimesyncCommand(sl_cli_command_arg_t *arguments)
{
  // send a read attr for time
  zclBufferSetup(ZCL_GLOBAL_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER,
                 ZCL_TIME_CLUSTER_ID,
                 ZCL_READ_ATTRIBUTES_COMMAND_ID);
  sli_zigbee_zcl_buffer_add_word(ZCL_TIME_ATTRIBUTE_ID);
  emAfSyncingTime = true;
  cliBufferPrint();
  sli_zigbee_cli_send_command(arguments);
}

#else // !UC_BUILD

// common include file
#include "app/framework/util/common.h"

#include "app/framework/util/af-main.h"
#include "app/framework/util/attribute-storage.h"
#include "app/framework/util/service-discovery.h"

#include "app/util/serial/sl_zigbee_command_interpreter.h"
#include "app/framework/cli/security-cli.h"

#include "app/util/common/library.h"

#ifdef EZSP_HOST
// the EM260 host needs to include the config file
  #include "app/framework/util/config.h"
#endif

//------------------------------------------------------------------------------
// Globals

// EMBER_AF_MAXIMUM_SEND_PAYLOAD_LENGTH is set in config.h
#define APP_ZCL_BUFFER_SIZE EMBER_AF_MAXIMUM_SEND_PAYLOAD_LENGTH

// The command-interpreter doesn't handle individual arguments longer than
// 255 bytes (since it is uses a 1-byte max-length value
#define APP_ZCL_BUFFER_SIZE_CLI \
  (APP_ZCL_BUFFER_SIZE > 255    \
   ? 255                        \
   : APP_ZCL_BUFFER_SIZE)

uint8_t appZclBuffer[APP_ZCL_BUFFER_SIZE];
uint16_t appZclBufferLen;
bool zclCmdIsBuilt = false;

uint16_t mfgSpecificId = EMBER_AF_NULL_MANUFACTURER_CODE;
uint8_t disableDefaultResponse = 0;
EmberApsFrame globalApsFrame;

static bool useNextSequence = false;

// a variable containing the number of messages sent from the CLI since the
// last reset

#ifdef EMBER_AF_ENABLE_STATISTICS
uint32_t haZclCliNumPktsSent = 0;
#endif

// flag to keep track of the fact that we just sent a read attr for time and
// we should set our time to the result of the read attr response
extern bool emAfSyncingTime;

// The direction of global commands, changeable via the "zcl global direction"
// command.  By default, send from client to server, which is how the CLI always
// functioned in the past.
static uint8_t zclGlobalDirection = ZCL_FRAME_CONTROL_CLIENT_TO_SERVER;

static bool useMulticastBinding = false;

//------------------------------------------------------------------------------
// Forward declarations

static void bufferAddLengthAndStringFromArgument(uint8_t argIndex,
                                                 bool isLongStringType);

// ******************************************************
// zcl command functions
// ******************************************************

void emAfApsFrameEndpointSetup(uint8_t srcEndpoint,
                               uint8_t dstEndpoint);

// used by zclBufferSetup and emAfCliRawCommand
void emAfApsFrameClusterIdSetup(uint16_t clusterId)
{
  // setup the global options and cluster ID
  // send command will setup profile ID and endpoints.
  globalApsFrame.options = EMBER_AF_DEFAULT_APS_OPTIONS;
  globalApsFrame.clusterId = clusterId;
}

// used by zclCommand and emAfCliRawCommand
void cliBufferPrint(void)
{
  uint8_t cmdIndex = (appZclBuffer[0] & ZCL_MANUFACTURER_SPECIFIC_MASK) ? 4 : 2;
  zclCmdIsBuilt = true;
  emberAfGuaranteedPrintln("Msg: clus 0x%2x, cmd 0x%x, len %d",
                           globalApsFrame.clusterId,
                           appZclBuffer[cmdIndex],
                           appZclBufferLen);
  emberAfGuaranteedPrint("buffer: ", appZclBufferLen);
  emberAfGuaranteedPrintBuffer(appZclBuffer, appZclBufferLen, true);
  emberAfGuaranteedPrintln("");
}

// for sending raw (user defined) ZCL messages
// raw <cluster> <data bytes>
// note: limited to 0xff bytes in length

void emAfCliRawCommand(void)
{
  uint8_t seqNumIndex;
  uint8_t length;
  emAfApsFrameClusterIdSetup((uint16_t)emberUnsignedCommandArgument(0));
  emberCopyStringArgument(1, appZclBuffer, APP_ZCL_BUFFER_SIZE_CLI, false);
  seqNumIndex = (appZclBuffer[0] & ZCL_MANUFACTURER_SPECIFIC_MASK) ? 3 : 1;
  if (useNextSequence) {
    appZclBuffer[seqNumIndex] = emberAfNextSequence();
  }
  emberStringCommandArgument(1, &length);
  appZclBufferLen = length;
  cliBufferPrint();
}

void zclBufferSetup(uint8_t frameType, uint16_t clusterId, uint8_t commandId)
{
  uint8_t index = 0;
  emAfApsFrameClusterIdSetup(clusterId);
  appZclBuffer[index++] = (frameType
                           | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
                           | (mfgSpecificId != EMBER_AF_NULL_MANUFACTURER_CODE
                              ? ZCL_MANUFACTURER_SPECIFIC_MASK
                              : 0)
                           | (disableDefaultResponse
                              ? ZCL_DISABLE_DEFAULT_RESPONSE_MASK
                              : 0));
  if (mfgSpecificId != EMBER_AF_NULL_MANUFACTURER_CODE) {
    appZclBuffer[index++] = (uint8_t)mfgSpecificId;
    appZclBuffer[index++] = (uint8_t)(mfgSpecificId >> 8);
  }
  appZclBuffer[index++] = emberAfNextSequence();
  appZclBuffer[index++] = commandId;
  appZclBufferLen = index;
}

void zclBufferAddByte(uint8_t byte)
{
  appZclBuffer[appZclBufferLen] = byte;
  appZclBufferLen += 1;
}

void zclBufferAddWord(uint16_t word)
{
  zclBufferAddByte(LOW_BYTE(word));
  zclBufferAddByte(HIGH_BYTE(word));
}

void zclBufferAddInt32(uint32_t value)
{
  uint8_t i;
  for (i = 0; i < 4; i++) {
    zclBufferAddByte(LOW_BYTE(value));
    value = value >> 8;
  }
}

void zclBufferAddBuffer(const uint8_t *buffer, uint8_t length)
{
  MEMMOVE(appZclBuffer + appZclBufferLen, buffer, length);
  appZclBufferLen += length;
}

// Made non-to remove warnings
void zclBufferAddString(const uint8_t *buffer)
{
  zclBufferAddBuffer(buffer, emberAfStringLength(buffer) + 1);
}

void zclBufferAddByteFromArgument(uint8_t index)
{
  zclBufferAddByte((uint8_t)emberUnsignedCommandArgument(index));
}

void zclBufferAddInt8sFromArgument(uint8_t index)
{
  zclBufferAddByte((uint8_t)((int8_t)emberSignedCommandArgument(index)));
}

void zclBufferAddInt16sFromArgument(uint8_t index)
{
  zclBufferAddWord((uint16_t)((int16_t)emberSignedCommandArgument(index)));
}

void zclBufferAddInt32sFromArgument(uint8_t index)
{
  zclBufferAddInt32((uint32_t)((int32_t)emberSignedCommandArgument(index)));
}

void zclBufferAddWordFromArgument(uint8_t index)
{
  zclBufferAddWord((uint16_t)emberUnsignedCommandArgument(index));
}

void zclBufferAddInt32FromArgument(uint8_t index)
{
  zclBufferAddInt32(emberUnsignedCommandArgument(index));
}

// Writes the length and contents of the string found at argIndex
// into the zcl buffer. Takes into account whether string is non-LONG
// (1-byte length prefix) or LONG (2-byte length prefix).
static void bufferAddLengthAndStringFromArgument(uint8_t argIndex,
                                                 bool isLongStringType)
{
  uint8_t prefixSize = (isLongStringType ? 2 : 1);
  // We protect against copying outside of the appZclBuffer
  // by passing the maximum length of the buffer, or a maximum
  // value of an uint8_t.
  uint16_t maxLength = APP_ZCL_BUFFER_SIZE - (appZclBufferLen + prefixSize);
  if (maxLength > MAX_INT8U_VALUE) {
    maxLength = MAX_INT8U_VALUE;
  }
  uint8_t length = emberCopyStringArgument(argIndex,
                                           appZclBuffer + appZclBufferLen + prefixSize,
                                           maxLength,
                                           false);
  appZclBuffer[appZclBufferLen] = length;
  if (prefixSize == 2) {
    // CLI max string length is 255, so LONG string upper length byte is zero.
    appZclBuffer[appZclBufferLen + 1] = 0;
  }
  appZclBufferLen += length + prefixSize;
}

// Non-LONG ZCL string type.
void zclBufferAddLengthAndStringFromArgument(uint8_t argIndex)
{
  bufferAddLengthAndStringFromArgument(argIndex, false); // !long
}

// LONG ZCL string type.
void zclBufferAddLongLengthAndStringFromArgument(uint8_t argIndex)
{
  bufferAddLengthAndStringFromArgument(argIndex, true); // long
}

// This function fully formats the zcl buffer given all the necessary data.
// The format string describes how the subsequent arguments are to be
// written into the zcl buffer.  The format characters are the same as
// those used by the command interpreter, namely:
// 'u'   A one-byte unsigned value.
// 'v'   A two-byte unsigned value encoded low byte first.
// 'w'   A four-byte unsigned value encoded from low to high byte.
// 'b'   A sequence of unsigned bytes.  The first supplied value is a
//       pointer to the data and the second value is the number of bytes.

void makeZclBuffer(uint8_t frameControl,
                   uint16_t clusterId,
                   uint8_t commandId,
                   char *format,
                   ...)
{
  uint8_t i = 0;
  va_list argPointer = { 0 };
  zclBufferSetup(frameControl, clusterId, commandId);
  va_start(argPointer, format);

  while (format[i] != 0) {
    char command = format[i];
    switch (command) {
      case 'u':
        zclBufferAddByte(va_arg(argPointer, int));
        break;
      case 'v':
        zclBufferAddWord(va_arg(argPointer, int));
        break;
      case 'w':
        zclBufferAddInt32(va_arg(argPointer, int));
        break;
      case 'b': {
        uint8_t *contents = va_arg(argPointer, uint8_t *);
        uint8_t count = va_arg(argPointer, int);
        zclBufferAddByte(count);
        zclBufferAddBuffer(contents, count);
        break;
      }
      default:
        assert(false);
    }
    i += 1;
  }

  va_end(argPointer);
  cliBufferPrint();
}

// Handles any zcl command where the argument list of the
// command is simply appended to the zcl buffer.  Handles argument types
// 'u', 'v', 'w', 's', 'r', 'q', and 'b'.  String arguments are written with
// a length byte first.
// Because this is constructing the zcl message, we ignore the
// special characters of ! and ? when processing the type string.
void zclSimpleCommand(uint8_t frameControl,
                      uint16_t clusterId,
                      uint8_t commandId)
{
  uint8_t argumentIndex;
  uint8_t typeIndex = 0;
  uint8_t count = emberCommandArgumentCount();
  uint8_t optionalDelimiters = 0;
  uint8_t type;

  zclBufferSetup(frameControl, clusterId, commandId);
  for (argumentIndex = 0; argumentIndex < count; argumentIndex++) {
    type = emberCurrentCommand->argumentTypes[typeIndex];
    // Upper bound to advance typeIndex is the argument count plus number of
    // optional argument delimiters observed thus far.
    if (typeIndex + 1 < (count + optionalDelimiters)
        && emberCurrentCommand->argumentTypes[typeIndex + 1] != '*') {
      typeIndex++;
    }

    switch (type) {
      case 'u':
        zclBufferAddByteFromArgument(argumentIndex);
        break;
      case 'v':
        zclBufferAddWordFromArgument(argumentIndex);
        break;
      case 'w':
        zclBufferAddInt32FromArgument(argumentIndex);
        break;
      case 's':
        zclBufferAddInt8sFromArgument(argumentIndex);
        break;
      case 'r':
        zclBufferAddInt16sFromArgument(argumentIndex);
        break;
      case 'q':
        zclBufferAddInt32sFromArgument(argumentIndex);
        break;
      case 'b':
        zclBufferAddLengthAndStringFromArgument(argumentIndex);
        break;
      case '*':
        break;
      case '!':
        // Delimiter for subsets of optional arguments, does not itself represent an argument.
        argumentIndex--;
        optionalDelimiters++;
        break;
      case '?':
      //lint -fallthrough
      case 0:
      //lint -fallthrough
      default:
        goto kickout;
    }
  }

  kickout:
  cliBufferPrint();
}

void zclGlobalSetup(uint8_t commandId)
{
  uint16_t clusterId = (uint16_t)emberUnsignedCommandArgument(0);
  zclBufferSetup(ZCL_GLOBAL_COMMAND | zclGlobalDirection,
                 clusterId,
                 commandId);
  zclBufferAddWordFromArgument(1);  // attr id
  if (commandId != ZCL_READ_ATTRIBUTES_COMMAND_ID) {
    zclBufferAddByteFromArgument(2);
  }
}

// zcl global direction <direction:1>
void zclGlobalDirectionCommand(void)
{
  zclGlobalDirection = ((((uint8_t)emberUnsignedCommandArgument(0))
                         == ZCL_DIRECTION_CLIENT_TO_SERVER)
                        ? ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
                        : ZCL_FRAME_CONTROL_SERVER_TO_CLIENT);
}

// zcl global read <cluster:2> <attribute:2>
void zclGlobalReadCommand(void)
{
  zclGlobalSetup(ZCL_READ_ATTRIBUTES_COMMAND_ID);
  cliBufferPrint();
}

// zcl global write <cluster:2> <attrID:2> <type:1> <data>
// zcl global uwrite <cluster:2> <attrID:2> <type:1> <data>
// zcl global nwrite <cluster:2> <attrID:2> <type:1> <data>
void zclGlobalWriteCommand(void)
{
  uint8_t type = (uint8_t)emberUnsignedCommandArgument(2);
  uint8_t commandChar = emberCurrentCommand->name[0];
  uint8_t commandId = (commandChar == 'u'
                       ? ZCL_WRITE_ATTRIBUTES_UNDIVIDED_COMMAND_ID
                       : (commandChar == 'n'
                          ? ZCL_WRITE_ATTRIBUTES_NO_RESPONSE_COMMAND_ID
                          : ZCL_WRITE_ATTRIBUTES_COMMAND_ID));
  zclGlobalSetup(commandId);

  if (emberAfIsThisDataTypeAStringType(type)) {
    bufferAddLengthAndStringFromArgument(3, emberAfIsLongStringAttributeType(type));
  } else {
    uint8_t length = emberAfGetDataSize(type);
    emberCopyStringArgument(3,
                            appZclBuffer + appZclBufferLen,
                            length,
                            true);  // pad with zeroes
    appZclBufferLen += length;
  }

  cliBufferPrint();
}

// zcl global discover <cluster> <attrID:2> <max # to report:1>
void zclGlobalDiscoverCommand(void)
{
  zclGlobalSetup(ZCL_DISCOVER_ATTRIBUTES_COMMAND_ID);
  cliBufferPrint();
}

// zcl global report <src endpoint id:1> <cluster id:2> <attribute id:2> <mask:1>
void zclGlobalReportCommand(void)
{
  EmberAfStatus status;
  EmberAfClusterId clusterId = (EmberAfClusterId)emberUnsignedCommandArgument(1);
  EmberAfAttributeId attributeId = (EmberAfAttributeId)emberUnsignedCommandArgument(2);
  uint8_t mask = (uint8_t)emberUnsignedCommandArgument(3);
  EmberAfAttributeType type;
  uint16_t size;
  uint8_t data[ATTRIBUTE_LARGEST];

  status = emberAfReadAttribute((uint8_t)emberUnsignedCommandArgument(0), // endpoint
                                clusterId,
                                attributeId,
                                (mask == 0
                                 ? CLUSTER_MASK_CLIENT
                                 : CLUSTER_MASK_SERVER),
                                data,
                                sizeof(data),
                                &type);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfReportingPrintln("ERR: reading attribute %x", status);
    return;
  }

  zclBufferSetup(ZCL_GLOBAL_COMMAND
                 | (mask == 0
                    ? ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
                    : ZCL_FRAME_CONTROL_SERVER_TO_CLIENT),
                 clusterId,
                 ZCL_REPORT_ATTRIBUTES_COMMAND_ID);
  zclBufferAddWord(attributeId);
  zclBufferAddByte(type);

  size = emberAfAttributeValueSize(type, data, sizeof(data));
  if (size == 0 || size > (APP_ZCL_BUFFER_SIZE - appZclBufferLen)) {
    emberAfReportingPrintln("ERR: attribute size %d too large for buffer", size);
    return;
  }
#if (BIGENDIAN_CPU)
  if (isThisDataTypeSentLittleEndianOTA(type)) {
    emberReverseMemCopy(appZclBuffer + appZclBufferLen, data, size);
  } else {
    MEMMOVE(appZclBuffer + appZclBufferLen, data, size);
  }
#else
  MEMMOVE(appZclBuffer + appZclBufferLen, data, size);
#endif
  appZclBufferLen += size;

  cliBufferPrint();
}

// zcl global report-read <cluster> <attrID:2> <direction:1>
void zclGlobalReportReadCommand(void)
{
  zclBufferSetup(ZCL_GLOBAL_COMMAND | zclGlobalDirection,
                 (EmberAfClusterId)emberUnsignedCommandArgument(0), // cluster id
                 ZCL_READ_REPORTING_CONFIGURATION_COMMAND_ID);
  zclBufferAddByteFromArgument(2); // direction
  zclBufferAddWordFromArgument(1); // attribute id
  cliBufferPrint();
}

// zcl global send-me-a-report <cluster id:2> <attribute id:2> <type:1> ...
// ... <min interval:2> <max interval:2> <reportable change:1-4>
void zclGlobalSendMeAReportCommand(void)
{
  uint8_t type = (uint8_t)emberUnsignedCommandArgument(2);

  zclBufferSetup(ZCL_GLOBAL_COMMAND | zclGlobalDirection,
                 (EmberAfClusterId)emberUnsignedCommandArgument(0), // cluster id
                 ZCL_CONFIGURE_REPORTING_COMMAND_ID);
  zclBufferAddByte(EMBER_ZCL_REPORTING_DIRECTION_REPORTED);
  zclBufferAddWordFromArgument(1);  // attribute id
  zclBufferAddByte(type);           // type
  zclBufferAddWordFromArgument(3);  // minimum reporting interval
  zclBufferAddWordFromArgument(4);  // maximum reporting interval

  // If the data type is analog, then the reportable change field is the same
  // size as the data type.  Otherwise, it is omitted.
  if (emberAfGetAttributeAnalogOrDiscreteType(type)
      == EMBER_AF_DATA_TYPE_ANALOG) {
    uint8_t dataSize = emberAfGetDataSize(type);
    emberCopyStringArgument(5,
                            appZclBuffer + appZclBufferLen,
                            dataSize,
                            false);
    appZclBufferLen += dataSize;
  }
  cliBufferPrint();
}

// zcl global expect-report-from-me <cluster id:2> <attribute id:2> <timeout:2>
void zclGlobalExpectReportFromMeCommand(void)
{
  zclBufferSetup(ZCL_GLOBAL_COMMAND | zclGlobalDirection,
                 (EmberAfClusterId)emberUnsignedCommandArgument(0), // cluster id
                 ZCL_CONFIGURE_REPORTING_COMMAND_ID);
  zclBufferAddByte(EMBER_ZCL_REPORTING_DIRECTION_RECEIVED);
  zclBufferAddWordFromArgument(1); // attribute id
  zclBufferAddWordFromArgument(2); // timeout
  cliBufferPrint();
}

// zcl global disc-com-gen <cluster id:2> <start command id:1> <max command id:1>
// zcl global disc-com-rec <cluster id:2> <start command id:1> <max command id:1>
void zclGlobalCommandDiscoveryCommand(void)
{
  zclBufferSetup(ZCL_GLOBAL_COMMAND | zclGlobalDirection,
                 (EmberAfClusterId)emberUnsignedCommandArgument(0),   // cluster id
                 (emberCurrentCommand->name[9] == 'g'
                  ? ZCL_DISCOVER_COMMANDS_GENERATED_COMMAND_ID
                  : ZCL_DISCOVER_COMMANDS_RECEIVED_COMMAND_ID));
  zclBufferAddByteFromArgument(1);   // start command id
  zclBufferAddByteFromArgument(2); // max command ids
  cliBufferPrint();
}

// zcl test response on
void zclTestResponseOnCommand(void)
{
  emberAfSetNoReplyForNextMessage(false);
}

// zcl test response off
void zclTestResponseOffCommand(void)
{
  emberAfSetNoReplyForNextMessage(true);
}

#ifdef ZCL_USING_BASIC_CLUSTER_CLIENT
// zcl basic rtfd
void zclBasicRtfdCommand(void)
{
  zclSimpleClientCommand(ZCL_BASIC_CLUSTER_ID,
                         ZCL_RESET_TO_FACTORY_DEFAULTS_COMMAND_ID);
}
#endif

void zclMfgCodeCommand(void)
{
  if (zclCmdIsBuilt) {
    emberAfAppPrintln("Command already built.  Cannot set MFG specific code for command.");
    return;
  }
  mfgSpecificId = (uint16_t)emberUnsignedCommandArgument(0);
  emberAfAppPrintln("MFG Code Set for next command: 0x%2X", mfgSpecificId);
}

void zclXDefaultRespCommand(void)
{
  disableDefaultResponse = (uint8_t)emberUnsignedCommandArgument(0);
  emberAfAppPrintln("Disable Default Response Set for next command: 0x%X", disableDefaultResponse);
}

void zclUseNextSequenceCommand(void)
{
  useNextSequence = (uint8_t)emberUnsignedCommandArgument(0);
  emberAfAppPrintln("Use Next Sequence Set for next command: 0x%X", useNextSequence);
}

void zclTimeCommand(void)
{
  emberAfSetTime(emberUnsignedCommandArgument(0));
}

#ifdef ZCL_USING_IDENTIFY_CLUSTER_CLIENT
// zcl identify id <identify time:2>
void zclIdentifyIdCommand(void)
{
  zclSimpleClientCommand(ZCL_IDENTIFY_CLUSTER_ID, ZCL_IDENTIFY_COMMAND_ID);
}

// zcl identify query
void zclIdentifyQueryCommand(void)
{
  zclSimpleClientCommand(ZCL_IDENTIFY_CLUSTER_ID, ZCL_IDENTIFY_QUERY_COMMAND_ID);
}

// zcl identify trigger <effect id:1> <effect variant:1>
void zclIdentifyTriggerEffectCommand(void)
{
  zclSimpleClientCommand(ZCL_IDENTIFY_CLUSTER_ID, ZCL_TRIGGER_EFFECT_COMMAND_ID);
}
#endif // ZCL_USING_IDENTIFY_CLUSTER_CLIENT

#ifdef ZCL_USING_IDENTIFY_CLUSTER_SERVER
// zcl identify on <endpoint:1> <time:2>
// zcl identify off <endpoint:1>
void zclIdentifyOnOffCommand(void)
{
  EmberAfStatus afStatus;
  uint16_t timeS = 0;
  if (emberCurrentCommand->name[1] == 'n') {
    timeS = (uint16_t)emberUnsignedCommandArgument(1);
    if (timeS == 0) {
      timeS = 60;
    }
  }

  afStatus = emberAfWriteServerAttribute(
    (uint8_t)emberUnsignedCommandArgument(0),                       // endpoint
    ZCL_IDENTIFY_CLUSTER_ID,
    ZCL_IDENTIFY_TIME_ATTRIBUTE_ID,
    (uint8_t *)&timeS,
    ZCL_INT16U_ATTRIBUTE_TYPE);
  if (EMBER_ZCL_STATUS_SUCCESS != afStatus) {
    emberAfDebugPrintln("Identify On/Off: failed to write value 0x%x  to cluster "
                        "0x%x attribute ID 0x%x: error 0x%x",
                        timeS,
                        ZCL_IDENTIFY_CLUSTER_ID,
                        ZCL_IDENTIFY_TIME_ATTRIBUTE_ID,
                        afStatus);
  }
}
#else
// TODO: the generated CLI is sucking in these methods regardless of whether or not it should;
// I suspect it has to do with the segmentaton of CLI commands, and it should be fixed, but
// for now I'm stubbing them. - Jeremy
void zclIdentifyOnOffCommand(void)
{
}
#endif // ZCL_USING_IDENTIFY_CLUSTER_SERVER

#ifdef ZCL_USING_GROUPS_CLUSTER_CLIENT
// zcl groups add <group id:2> <name:16>
// zcl groups ad-if-id <group id:2> <name:16>
void zclGroupsAddCommand(void)
{
  uint8_t commandId = (emberCurrentCommand->name[2] == 'd'
                       ? ZCL_ADD_GROUP_COMMAND_ID
                       : ZCL_ADD_GROUP_IF_IDENTIFYING_COMMAND_ID);
  zclSimpleClientCommand(ZCL_GROUPS_CLUSTER_ID, commandId);
}

// zcl groups view <group id:2>
void zclGroupsViewCommand(void)
{
  zclSimpleClientCommand(ZCL_GROUPS_CLUSTER_ID, ZCL_VIEW_GROUP_COMMAND_ID);
}

// zcl groups get <count:1> [<group id:2> * count]
void zclGroupsGetCommand(void)
{
  uint8_t i;
  uint8_t count = (uint8_t)emberUnsignedCommandArgument(0);
  zclBufferSetup(ZCL_CLUSTER_SPECIFIC_COMMAND
                 | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER,
                 ZCL_GROUPS_CLUSTER_ID,
                 ZCL_GET_GROUP_MEMBERSHIP_COMMAND_ID);
  zclBufferAddByte(count); // group count
  for (i = 0; i < count; i++) {
    zclBufferAddWordFromArgument(1 + i);
  }
  cliBufferPrint();
}

// zcl groups remove <group id:2>
void zclGroupsRemoveCommand(void)
{
  zclSimpleClientCommand(ZCL_GROUPS_CLUSTER_ID, ZCL_REMOVE_GROUP_COMMAND_ID);
}

// zcl groups rmall
void zclGroupsRemoveAllCommand(void)
{
  zclSimpleClientCommand(ZCL_GROUPS_CLUSTER_ID,
                         ZCL_REMOVE_ALL_GROUPS_COMMAND_ID);
}
#else
// TODO: the generated CLI is sucking in this method regardless of whether or not it should;
// I suspect it has to do with the segmentaton of CLI commands, and it should be fixed, but
// for now I'm stubbing it. - Jeremy
void zclGroupsGetCommand(void)
{
}
#endif //ZCL_USING_GROUPS_CLUSTER_CLIENT

#ifdef ZCL_USING_SCENES_CLUSTER_CLIENT
#ifdef ZCL_USING_ON_OFF_CLUSTER_CLIENT
bool addScenesOnOffValue;
#endif
#ifdef ZCL_USING_LEVEL_CONTROL_CLUSTER_CLIENT
uint8_t addScenesLevelValue;
#endif

// zcl scenes add <groupId:2> <sceneId:1> <trans time:2> <name> <extensionFieldSets>
// zcl scenes eadd <groupId:2> <sceneId:1> <trans time:2> <name> <extensionFieldSets>
void zclScenesAddCommand(void)
{
  zclBufferSetup(ZCL_CLUSTER_SPECIFIC_COMMAND
                 | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER,
                 ZCL_SCENES_CLUSTER_ID,
                 (emberCurrentCommand->name[0] != 'e'
                  ? ZCL_ADD_SCENE_COMMAND_ID
                  : ZCL_ENHANCED_ADD_SCENE_COMMAND_ID));
  zclBufferAddWordFromArgument(0);
  zclBufferAddByteFromArgument(1);
  zclBufferAddWordFromArgument(2);
  zclBufferAddLengthAndStringFromArgument(3);

#ifdef ZCL_USING_ON_OFF_CLUSTER_CLIENT
  // cluster:2 len:1 value:1 (on/off extension)
  zclBufferAddWord(ZCL_ON_OFF_CLUSTER_ID);
  zclBufferAddByte(1);
  zclBufferAddByte(addScenesOnOffValue);
#endif //ZCL_USING_ON_OFF_CLUSTER_CLIENT

#ifdef ZCL_USING_LEVEL_CONTROL_CLUSTER_CLIENT
  // cluster:2 len:1 value:1 (level control extension)
  zclBufferAddWord(ZCL_LEVEL_CONTROL_CLUSTER_ID);
  zclBufferAddByte(1);
  zclBufferAddByte(addScenesLevelValue);
#endif //ZCL_USING_LEVEL_CONTROL_CLUSTER_CLIENT

  cliBufferPrint();
}

// zcl scenes view <groupId:2> <sceneId:1>
// zcl scenes eview <groupId:2> <sceneId:1>
void zclScenesViewCommand(void)
{
  zclSimpleClientCommand(ZCL_SCENES_CLUSTER_ID,
                         (emberCurrentCommand->name[0] != 'e'
                          ? ZCL_VIEW_SCENE_COMMAND_ID
                          : ZCL_ENHANCED_VIEW_SCENE_COMMAND_ID));
}

// zcl scenes remove <groupId:2> <sceneId:1>
void zclScenesRemoveCommand(void)
{
  zclSimpleClientCommand(ZCL_SCENES_CLUSTER_ID, ZCL_REMOVE_SCENE_COMMAND_ID);
}

// zcl scenes rmall <groupId:2>
void zclScenesRemoveAllCommand(void)
{
  zclSimpleClientCommand(ZCL_SCENES_CLUSTER_ID, ZCL_REMOVE_ALL_SCENES_COMMAND_ID);
}

// zcl scenes store <groupId:2> <sceneId:1>
void zclScenesStoreCommand(void)
{
  zclSimpleClientCommand(ZCL_SCENES_CLUSTER_ID, ZCL_STORE_SCENE_COMMAND_ID);
}

// zcl scenes recall <groupId:2> <sceneId:1>
void zclScenesRecallCommand(void)
{
  zclSimpleClientCommand(ZCL_SCENES_CLUSTER_ID, ZCL_RECALL_SCENE_COMMAND_ID);
}

// zcl scenes get <groupId:2>
void zclScenesGetMembershipCommand(void)
{
  zclSimpleClientCommand(ZCL_SCENES_CLUSTER_ID, ZCL_GET_SCENE_MEMBERSHIP_COMMAND_ID);
}

// zcl scenes copy <mode:1> <group id from:2> <scenes id from:1> <group id to:2> <scene id:1>
void zclScenesCopyCommand(void)
{
  zclSimpleClientCommand(ZCL_SCENES_CLUSTER_ID, ZCL_COPY_SCENE_COMMAND_ID);
}

// zcl scenes set [on|off] <level:1>
void zclScenesSetCommand(void)
{
#ifdef ZCL_USING_ON_OFF_CLUSTER_CLIENT
  addScenesOnOffValue = (emberCurrentCommand->name[1] == 'n');
#endif
#ifdef ZCL_USING_LEVEL_CONTROL_CLUSTER_CLIENT
  addScenesLevelValue = (uint8_t)emberUnsignedCommandArgument(0);
#endif
}
#endif //ZCL_USING_SCENES_CLUSTER_CLIENT

#ifdef ZCL_USING_ON_OFF_CLUSTER_CLIENT
// zcl on-off [ off | on | toggle ]
void zclOnOffCommand(void)
{
  uint8_t secondChar = emberCurrentCommand->name[1];
  uint8_t commandId = (secondChar == 'n'
                       ? ZCL_ON_COMMAND_ID
                       : (secondChar == 'f'
                          ? ZCL_OFF_COMMAND_ID
                          : ZCL_TOGGLE_COMMAND_ID));
  zclSimpleClientCommand(ZCL_ON_OFF_CLUSTER_ID, commandId);
}

// zcl on-off offeffect <effect id:1> <effect variant:1>
void zclOnOffOffWithEffectCommand(void)
{
  zclSimpleClientCommand(ZCL_ON_OFF_CLUSTER_ID, ZCL_OFF_WITH_EFFECT_COMMAND_ID);
}

// zcl on-off onrecall
void zclOnOffOnWithRecallGlobalSceneCommand(void)
{
  zclSimpleClientCommand(ZCL_ON_OFF_CLUSTER_ID,
                         ZCL_ON_WITH_RECALL_GLOBAL_SCENE_COMMAND_ID);
}

// zcl on-off ontimedoff <on off control:1> <on time:2> <off wait time:2>
void zclOnOffOnWithTimedOffCommand(void)
{
  zclSimpleClientCommand(ZCL_ON_OFF_CLUSTER_ID,
                         ZCL_ON_WITH_TIMED_OFF_COMMAND_ID);
}

#endif //ZCL_USING_ON_OFF_CLUSTER_CLIENT

#ifdef ZCL_USING_LEVEL_CONTROL_CLUSTER_CLIENT
// zcl level-control mv-to-level [level:1] [trans time:2]
// zcl level-control o-mv-to-level [level:1] [trans time:2]
void zclLevelControlMoveToLevelCommand(void)
{
  uint8_t commandId = (emberCurrentCommand->name[0] == 'o'
                       ? ZCL_MOVE_TO_LEVEL_WITH_ON_OFF_COMMAND_ID
                       : ZCL_MOVE_TO_LEVEL_COMMAND_ID);
  zclSimpleClientCommand(ZCL_LEVEL_CONTROL_CLUSTER_ID, commandId);
}

// zcl level-control move [mode:1] [rate:1]
// zcl level-control o-move [mode:1] [rate:1]
void zclLevelControlMoveCommand(void)
{
  uint8_t commandId = (emberCurrentCommand->name[0] == 'o'
                       ? ZCL_MOVE_WITH_ON_OFF_COMMAND_ID
                       : ZCL_MOVE_COMMAND_ID);
  zclSimpleClientCommand(ZCL_LEVEL_CONTROL_CLUSTER_ID, commandId);
}

// zcl level-control step [step:1] [step size:1] [trans time:2]
// zcl level-control o-step [step:1] [step size:1] [trans time:2]
void zclLevelControlStepCommand(void)
{
  uint8_t commandId = (emberCurrentCommand->name[0] == 'o'
                       ? ZCL_STEP_WITH_ON_OFF_COMMAND_ID
                       : ZCL_STEP_COMMAND_ID);
  zclSimpleClientCommand(ZCL_LEVEL_CONTROL_CLUSTER_ID, commandId);
}

// zcl level-control stop
// zcl level-control ostop
void zclLevelControlStopCommand(void)
{
  uint8_t commandId = (emberCurrentCommand->name[0] == 'o'
                       ? ZCL_STOP_WITH_ON_OFF_COMMAND_ID
                       : ZCL_STOP_COMMAND_ID);
  zclSimpleClientCommand(ZCL_LEVEL_CONTROL_CLUSTER_ID, commandId);
}
#endif //ZCL_USING_LEVEL_CONTROL_CLUSTER_CLIENT

#ifdef ZCL_USING_THERMOSTAT_CLUSTER_CLIENT
// zcl tstat set <mode:1 int> <amount:1 int>
void zclThermostatSetCommand(void)
{
  zclSimpleClientCommand(ZCL_THERMOSTAT_CLUSTER_ID,
                         ZCL_SETPOINT_RAISE_LOWER_COMMAND_ID);
}
#endif // ZCL_USING_THERMOSTAT_CLUSTER_CLIENT

#ifdef ZCL_USING_IAS_ZONE_CLUSTER_SERVER
// zcl ias-zone enroll <zone type: 2 int> <manuf code: 2 int>
void zclIasZoneEnrollCommand(void)
{
  zclSimpleServerCommand(ZCL_IAS_ZONE_CLUSTER_ID,
                         ZCL_ZONE_ENROLL_REQUEST_COMMAND_ID);
  globalApsFrame.options |= EMBER_APS_OPTION_SOURCE_EUI64;
}

// zcl ias-zone sc <zone status: 2 int> <ext status: 1 int> <zone id: 1 int> <delay: 2 int>
void zclIasZoneStatusChangeCommand(void)
{
  zclSimpleServerCommand(ZCL_IAS_ZONE_CLUSTER_ID,
                         ZCL_ZONE_STATUS_CHANGE_NOTIFICATION_COMMAND_ID);
  globalApsFrame.options |= EMBER_APS_OPTION_SOURCE_EUI64;
}
#endif // ZCL_USING_IAS_ZONE_CLUSTER_SERVER

#ifdef ZCL_USING_IAS_ACE_CLUSTER_CLIENT
// zcl ias-ace a <int:1>
void zclIasAceArmCommand(void)
{
  zclSimpleClientCommand(ZCL_IAS_ACE_CLUSTER_ID, ZCL_ARM_COMMAND_ID);
}

// zcl ias-ace b <zones as hex string>
void zclIasAceBypassCommand(void)
{
  zclSimpleClientCommand(ZCL_IAS_ACE_CLUSTER_ID, ZCL_BYPASS_COMMAND_ID);
}

// zcl ias-ace e
void zclIasAceEmergencyCommand(void)
{
  zclSimpleClientCommand(ZCL_IAS_ACE_CLUSTER_ID, ZCL_EMERGENCY_COMMAND_ID);
}

// zcl ias-ace f
void zclIasAceFireCommand(void)
{
  zclSimpleClientCommand(ZCL_IAS_ACE_CLUSTER_ID, ZCL_FIRE_COMMAND_ID);
}

// zcl ias-ace p
void zclIasAcePanicCommand(void)
{
  zclSimpleClientCommand(ZCL_IAS_ACE_CLUSTER_ID, ZCL_PANIC_COMMAND_ID);
}

// zcl ias-ace getzm
void zclIasAceGetZoneMapCommand(void)
{
  zclSimpleClientCommand(ZCL_IAS_ACE_CLUSTER_ID,
                         ZCL_GET_ZONE_ID_MAP_COMMAND_ID);
}

// zcl ias-ace getzi <zone id:1>
void zclIasAceGetZoneInfoCommand(void)
{
  zclSimpleClientCommand(ZCL_IAS_ACE_CLUSTER_ID,
                         ZCL_GET_ZONE_INFORMATION_COMMAND_ID);
}
#endif // ZCL_USING_IAS_ACE_CLUSTER_CLIENT

#ifdef ZCL_USING_COLOR_CONTROL_CLUSTER_CLIENT
// zcl color-control movetohue <hue:1> <direction:1> <transition time:2>
// zcl color-control emovetohue <enhanced hue:2> <direction:1> <transition time:2>
void zclColorControlMoveToHueCommand(void)
{
  zclSimpleClientCommand(ZCL_COLOR_CONTROL_CLUSTER_ID,
                         (emberCurrentCommand->name[0] != 'e'
                          ? ZCL_MOVE_TO_HUE_COMMAND_ID
                          : ZCL_ENHANCED_MOVE_TO_HUE_COMMAND_ID));
}

// zcl color-control movehue <move mode:1> <rate:1>
// zcl color-control emovehue <move mode:1> <rate:2>
void zclColorControlMoveHueCommand(void)
{
  zclSimpleClientCommand(ZCL_COLOR_CONTROL_CLUSTER_ID,
                         (emberCurrentCommand->name[0] != 'e'
                          ? ZCL_MOVE_HUE_COMMAND_ID
                          : ZCL_ENHANCED_MOVE_HUE_COMMAND_ID));
}

// zcl color-control stephue <step mode:1> <step size:1> <transition time:1>
// zcl color-control estephue <step mode:1> <step size:2> <transition time:2>
void zclColorControlStepHueCommand(void)
{
  zclSimpleClientCommand(ZCL_COLOR_CONTROL_CLUSTER_ID,
                         (emberCurrentCommand->name[0] != 'e'
                          ? ZCL_STEP_HUE_COMMAND_ID
                          : ZCL_ENHANCED_STEP_HUE_COMMAND_ID));
}

// zcl color-control movetosat <saturation:1> <transition time:2>
void zclColorControlMoveToSatCommand(void)
{
  zclSimpleClientCommand(ZCL_COLOR_CONTROL_CLUSTER_ID,
                         ZCL_MOVE_TO_SATURATION_COMMAND_ID);
}

// zcl color-control movesat <move mode:1> <rate:1>
void zclColorControlMoveSatCommand(void)
{
  zclSimpleClientCommand(ZCL_COLOR_CONTROL_CLUSTER_ID,
                         ZCL_MOVE_SATURATION_COMMAND_ID);
}

// zcl color-control stepsat <step mode:1> <step size:1> <transition time:1>
void zclColorControlStepSatCommand(void)
{
  zclSimpleClientCommand(ZCL_COLOR_CONTROL_CLUSTER_ID,
                         ZCL_STEP_SATURATION_COMMAND_ID);
}

// zcl color-control movetohueandsat <hue:1> <saturation:1> <transition time:2>
// zcl color-control emovetohueandsat <enhanced hue:2> <saturation:1> <transition time:2>
void zclColorControlMoveToHueAndSatCommand(void)
{
  zclSimpleClientCommand(ZCL_COLOR_CONTROL_CLUSTER_ID,
                         (emberCurrentCommand->name[0] != 'e'
                          ? ZCL_MOVE_TO_HUE_AND_SATURATION_COMMAND_ID
                          : ZCL_ENHANCED_MOVE_TO_HUE_AND_SATURATION_COMMAND_ID));
}

// zcl color-control movetocolor <color x:2> <color y:2> <transition time:2>
void zclColorControlMoveToColorCommand(void)
{
  zclSimpleClientCommand(ZCL_COLOR_CONTROL_CLUSTER_ID,
                         ZCL_MOVE_TO_COLOR_COMMAND_ID);
}

// zcl color-control movecolor <rate x:2> <rate y:2>
// zcl color-control movecolortemp <move mode:1> <rate:2> <color temperature min:2> <color temperature max:2>
void zclColorControlMoveColorCommand(void)
{
  zclSimpleClientCommand(ZCL_COLOR_CONTROL_CLUSTER_ID,
                         (emberCurrentCommand->argumentTypes[0] == 'r'
                          ? ZCL_MOVE_COLOR_COMMAND_ID
                          : ZCL_MOVE_COLOR_TEMPERATURE_COMMAND_ID));
}

// zcl color-control stepcolor <step x:2> <step y:2> <transition time:2>
// zcl color-control stepcolortemp <step mode:1> <step size:2> <transition time:2> <color temperature min:2> <color temperature max:2>
void zclColorControlStepColorCommand(void)
{
  zclSimpleClientCommand(ZCL_COLOR_CONTROL_CLUSTER_ID,
                         (emberCurrentCommand->argumentTypes[0] == 'r'
                          ? ZCL_STEP_COLOR_COMMAND_ID
                          : ZCL_STEP_COLOR_TEMPERATURE_COMMAND_ID));
}

// zcl color-control movetocolortemp <color temperature:2> <transition time:2>
void zclColorControlMoveToColorTemperatureCommand(void)
{
  zclSimpleClientCommand(ZCL_COLOR_CONTROL_CLUSTER_ID,
                         ZCL_MOVE_TO_COLOR_TEMPERATURE_COMMAND_ID);
}

// zcl color-control loop <update flags:1> <action:1> <direction:1> <time:2> <start hue:2>
void zclColorControlColorLoopSetCommand(void)
{
  zclSimpleClientCommand(ZCL_COLOR_CONTROL_CLUSTER_ID,
                         ZCL_COLOR_LOOP_SET_COMMAND_ID);
}

// zcl color-control stopmovestep
void zclColorControlStopMoveStepCommand(void)
{
  zclSimpleClientCommand(ZCL_COLOR_CONTROL_CLUSTER_ID,
                         ZCL_STOP_MOVE_STEP_COMMAND_ID);
}
#endif // ZCL_USING_COLOR_CONTROL_CLUSTER_CLIENT

#ifdef ZCL_USING_POLL_CONTROL_CLUSTER_CLIENT
void zclPollControlFastPollStopCommand(void)
{
  zclSimpleClientCommand(ZCL_POLL_CONTROL_CLUSTER_ID,
                         ZCL_FAST_POLL_STOP_COMMAND_ID);
}

// zcl poll-control long <long poll interval:4>
// zcl poll-control short <short poll interval:2>
void zclPollControlSetPollIntervalCommand(void)
{
  zclSimpleClientCommand(ZCL_POLL_CONTROL_CLUSTER_ID,
                         (emberCurrentCommand->name[0] == 'l'
                          ? ZCL_SET_LONG_POLL_INTERVAL_COMMAND_ID
                          : ZCL_SET_SHORT_POLL_INTERVAL_COMMAND_ID));
}
#endif

#ifdef ZCL_USING_POWER_PROFILE_CLUSTER_CLIENT
//zcl power-profile profile <profile id:1>
void zclPowerProfileRequestCommand(void)
{
  zclSimpleClientCommand(ZCL_POWER_PROFILE_CLUSTER_ID,
                         ZCL_POWER_PROFILE_REQUEST_COMMAND_ID);
}

//zcl power-profile state
void zclPowerProfileStateRequestCommand(void)
{
  zclSimpleClientCommand(ZCL_POWER_PROFILE_CLUSTER_ID,
                         ZCL_POWER_PROFILE_STATE_REQUEST_COMMAND_ID);
}

//zcl power-profile energy-phases-schedule <profile id:1> <num scheduled phases:1> [<energy phase ID:1> <scheduled time:2> * num scheduled phases]
void zclPowerProfileEnergyPhasesScheduleNotificationCommand(void)
{
  uint8_t i;
  uint8_t numScheduledPhases = (uint8_t)emberUnsignedCommandArgument(1);

  //Check if an even number of args and minimum number of args were supplied
  if (emberCommandArgumentCount() & 1 || emberCommandArgumentCount() < 2) {
    emberAfGuaranteedPrintln("Wrong number of args");
    emberAfGuaranteedPrintln("Usage:");
    emberAfGuaranteedPrint("<profile id:1> <num scheduled phases:1> ");
    emberAfGuaranteedPrint("[<energy phase ID:1> <scheduled time:2> * ");
    emberAfGuaranteedPrintln("num scheduled phases]");
  } else {
    zclBufferSetup(ZCL_CLUSTER_SPECIFIC_COMMAND
                   | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER,
                   ZCL_POWER_PROFILE_CLUSTER_ID,
                   ZCL_ENERGY_PHASES_SCHEDULE_NOTIFICATION_COMMAND_ID);

    zclBufferAddByteFromArgument(0);   // profile id
    zclBufferAddByte(numScheduledPhases); // number scheduled phase count

    //Add energy phase ID and scheduled time pairs
    for (i = 2; (i + 1) < emberCommandArgumentCount(); i += 2) {
      zclBufferAddByteFromArgument(i);
      zclBufferAddWordFromArgument(i + 1);
    }
    cliBufferPrint();
  }
}

//zcl power-profile schedule-constraints <profile id:1>
void zclPowerProfileScheduleConstraintsRequestCommand(void)
{
  zclSimpleClientCommand(ZCL_POWER_PROFILE_CLUSTER_ID,
                         ZCL_POWER_PROFILE_SCHEDULE_CONSTRAINTS_REQUEST_COMMAND_ID);
}

//zcl power-profile energy-phases-schedule-states <profile id:1>
void zclPowerProfileEnergyPhasesScheduleStateRequestCommand(void)
{
  zclSimpleClientCommand(ZCL_POWER_PROFILE_CLUSTER_ID,
                         ZCL_ENERGY_PHASES_SCHEDULE_STATE_REQUEST_COMMAND_ID);
}
#endif //ZCL_USING_POWER_PROFILE_CLUSTER_CLIENT

#ifndef EMBER_AF_GENERATE_CLI
#ifdef ZCL_USING_WINDOW_COVERING_CLUSTER_CLIENT
static void zclWindowCoveringUpCommand(void)
{
  zclSimpleClientCommand(ZCL_WINDOW_COVERING_CLUSTER_ID,
                         ZCL_WINDOW_COVERING_UP_OPEN_COMMAND_ID);
}

static void zclWindowCoveringDownCommand(void)
{
  zclSimpleClientCommand(ZCL_WINDOW_COVERING_CLUSTER_ID,
                         ZCL_WINDOW_COVERING_DOWN_CLOSE_COMMAND_ID);
}

static void zclWindowCoveringStopCommand(void)
{
  zclSimpleClientCommand(ZCL_WINDOW_COVERING_CLUSTER_ID,
                         ZCL_WINDOW_COVERING_STOP_COMMAND_ID);
}

static void zclWindowCoveringGoToLiftValueCommand(void)
{
  zclSimpleClientCommand(ZCL_WINDOW_COVERING_CLUSTER_ID,
                         ZCL_WINDOW_COVERING_GO_TO_LIFT_VALUE_COMMAND_ID);
}

static void zclWindowCoveringGoToLiftPercentageCommand(void)
{
  zclSimpleClientCommand(ZCL_WINDOW_COVERING_CLUSTER_ID,
                         ZCL_WINDOW_COVERING_GO_TO_LIFT_PERCENTAGE_COMMAND_ID);
}

static void zclWindowCoveringGoToTiltValueCommand(void)
{
  zclSimpleClientCommand(ZCL_WINDOW_COVERING_CLUSTER_ID,
                         ZCL_WINDOW_COVERING_GO_TO_TILT_VALUE_COMMAND_ID);
}

static void zclWindowCoveringGoToTiltPercentageCommand(void)
{
  zclSimpleClientCommand(ZCL_WINDOW_COVERING_CLUSTER_ID,
                         ZCL_WINDOW_COVERING_GO_TO_TILT_PERCENTAGE_COMMAND_ID);
}
#endif //ZCL_USING_WINDOW_COVERING_CLUSTER_CLIENT
#endif //EMBER_AF_GENERATE_CLI

#ifdef ZCL_USING_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_SERVER
// zcl drlc lce <eventId:4> <start:4> <duration:2> <event control:1>
void zclDrlcLoadControlEventCommand(void)
{
  zclBufferSetup(ZCL_CLUSTER_SPECIFIC_COMMAND
                 | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT,
                 ZCL_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_ID,
                 ZCL_LOAD_CONTROL_EVENT_COMMAND_ID);
  zclBufferAddInt32FromArgument(0);  // event id
  zclBufferAddWord(0x0fff);          // all device classes
  zclBufferAddByteFromArgument(1);   // UEG
  zclBufferAddInt32FromArgument(2);  // start
  zclBufferAddWordFromArgument(3);   // duration
  zclBufferAddByte(1);               // criticality level, normal
  zclBufferAddByte(0);               // 1.1c cooling temp offset
  zclBufferAddByte(0);               // 1.1c heating temp offset
  zclBufferAddWord(0x1a09);          // cool temp set point 23.3c
  zclBufferAddWord(0x1a09);          // heat temp set point
  zclBufferAddByte(0x0a);            // -10% avg load percent
  zclBufferAddByte(0);               // duty cycle
  zclBufferAddByteFromArgument(4);   // event control
  globalApsFrame.options |= EMBER_APS_OPTION_SOURCE_EUI64;
  cliBufferPrint();
}

// zcl drlc cl <eventId:4> <device class:2> <ueg: 1> <cancel control: 1> <start time:4>
void zclDrlcCancelCommand(void)
{
  zclSimpleServerCommand(ZCL_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_ID,
                         ZCL_CANCEL_LOAD_CONTROL_EVENT_COMMAND_ID);
  globalApsFrame.options |= EMBER_APS_OPTION_SOURCE_EUI64;
}

// zcl drlc ca
void zclDrlcCancelAllCommand(void)
{
  zclBufferSetup(ZCL_CLUSTER_SPECIFIC_COMMAND
                 | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT,
                 ZCL_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_ID,
                 ZCL_CANCEL_ALL_LOAD_CONTROL_EVENTS_COMMAND_ID);
  zclBufferAddByte(0);  // bogus time
  globalApsFrame.options |= EMBER_APS_OPTION_SOURCE_EUI64;
  cliBufferPrint();
}

#else
// TODO: the generated CLI is sucking in this method regardless of whether or not it should;
// I suspect it has to do with the segmentaton of CLI commands, and it should be fixed, but
// for now I'm stubbing it. - Jeremy
void zclDrlcLoadControlEventCommand(void)
{
}

#endif //ZCL_USING_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_SERVER

#ifdef ZCL_USING_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_CLIENT
// zcl drlc gse <start time:4> <number of events:1>
void zclDrlcGetScheduledEventsCommand(void)
{
  zclSimpleClientCommand(ZCL_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_ID,
                         ZCL_GET_SCHEDULED_EVENTS_COMMAND_ID);
}

#endif //ZCL_USING_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_CLIENT

#ifdef ZCL_USING_SIMPLE_METERING_CLUSTER_CLIENT
// zcl sm  gp <type:1> <time:4> <intervals:1>
void zclSimpleMeteringGetProfileCommand(void)
{
  zclSimpleClientCommand(ZCL_SIMPLE_METERING_CLUSTER_ID,
                         ZCL_GET_PROFILE_COMMAND_ID);
}

// zcl sm fp <update period:1> <duration:1>
void zclSimpleMeteringFastPollCommand(void)
{
  zclSimpleClientCommand(ZCL_SIMPLE_METERING_CLUSTER_ID,
                         ZCL_REQUEST_FAST_POLL_MODE_COMMAND_ID);
}
#endif //ZCL_USING_SIMPLE_METERING_CLUSTER_CLIENT

#ifdef ZCL_USING_SIMPLE_METERING_CLUSTER_SERVER
// zcl sm rm
void zclSimpleMeteringRequestMirrorCommand(void)
{
  zclSimpleServerCommand(ZCL_SIMPLE_METERING_CLUSTER_ID,
                         ZCL_REQUEST_MIRROR_COMMAND_ID);
}

// zcl sm dm
void zclSimpleMeteringDeleteMirrorCommand(void)
{
  zclSimpleServerCommand(ZCL_SIMPLE_METERING_CLUSTER_ID,
                         ZCL_MIRROR_REMOVED_COMMAND_ID);
}
#endif //ZCL_USING_SIMPLE_METERING_CLUSTER_SERVER

#ifdef ZCL_USING_PRICE_CLUSTER_CLIENT
// zcl price current
void zclPriceGetCurrentCommand(void)
{
  zclBufferSetup(ZCL_CLUSTER_SPECIFIC_COMMAND
                 | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER,
                 ZCL_PRICE_CLUSTER_ID,
                 ZCL_GET_CURRENT_PRICE_COMMAND_ID);
  zclBufferAddByte(0);  // command options
  cliBufferPrint();
}

// zcl price scheduled <startTime:4> <num-events:1>
void zclPriceGetScheduleCommand(void)
{
  zclSimpleClientCommand(ZCL_PRICE_CLUSTER_ID,
                         ZCL_GET_SCHEDULED_PRICES_COMMAND_ID);
}
#endif //ZCL_USING_PRICE_CLUSTER_CLIENT

#ifdef ZCL_USING_MESSAGING_CLUSTER_CLIENT
// zcl message get
void zclMessageGetCommand(void)
{
  zclSimpleClientCommand(ZCL_MESSAGING_CLUSTER_ID,
                         ZCL_GET_LAST_MESSAGE_COMMAND_ID);
}
#endif //ZCL_USING_MESSAGING_CLUSTER_CLIENT

#ifdef ZCL_USING_GENERIC_TUNNEL_CLUSTER_CLIENT
// zcl tunnel match <address bytes>
void zclTunnelMatchCommand(void)
{
  zclSimpleClientCommand(ZCL_GENERIC_TUNNEL_CLUSTER_ID,
                         ZCL_MATCH_PROTOCOL_ADDRESS_COMMAND_ID);
}
#endif //ZCL_USING_GENERIC_TUNNEL_CLUSTER_CLIENT

#ifdef ZCL_USING_GENERIC_TUNNEL_CLUSTER_SERVER
// zcl tunnel advertise <address bytes>
void zclTunnelAdvertiseCommand(void)
{
  zclSimpleServerCommand(ZCL_GENERIC_TUNNEL_CLUSTER_ID,
                         ZCL_ADVERTISE_PROTOCOL_ADDRESS_COMMAND_ID);
}

// zcl tunnel response
void zclTunnelResponseCommand(void)
{
  zclSimpleServerCommand(ZCL_GENERIC_TUNNEL_CLUSTER_ID,
                         ZCL_MATCH_PROTOCOL_ADDRESS_RESPONSE_COMMAND_ID);
}
#endif //ZCL_USING_GENERIC_TUNNEL_CLUSTER_SERVER

#ifdef ZCL_USING_BACNET_PROTOCOL_TUNNEL_CLUSTER_CLIENT
// zcl bacnet transfer-npdu fixed <length:1> <bytes>
void zclBacnetTransferFixedCommand(void)
{
  // not done yet
}

// zcl bacnet transfer-npdu random <length:1>
void zclBacnetTransferRandomCommand(void)
{
  uint8_t i;
  uint8_t length = (uint8_t)emberUnsignedCommandArgument(0);
  zclBufferSetup(ZCL_CLUSTER_SPECIFIC_COMMAND
                 | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER,
                 ZCL_BACNET_PROTOCOL_TUNNEL_CLUSTER_ID,
                 ZCL_TRANSFER_NPDU_COMMAND_ID);
  zclBufferAddByte(length);
  for (i = 0; i < length; i++) {
    zclBufferAddByte(LOW_BYTE(emberGetPseudoRandomNumber()));
  }
  cliBufferPrint();
}

// zcl bacnet transfer-npdu whois
void zclBacnetTransferWhoisCommand(void)
{
  zclBufferSetup(ZCL_CLUSTER_SPECIFIC_COMMAND
                 | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER,
                 ZCL_BACNET_PROTOCOL_TUNNEL_CLUSTER_ID,
                 ZCL_TRANSFER_NPDU_COMMAND_ID);
  //zclBufferAddByte(0);            // what is this?
  zclBufferAddInt32(0xFFFF2001);
  zclBufferAddInt32(0x0810FF00);
  cliBufferPrint();
}
#endif //ZCL_USING_BACNET_PROTOCOL_TUNNEL_CLUSTER_CLIENT

#ifdef ZCL_USING_DOOR_LOCK_CLUSTER_CLIENT
// zcl lock [ lock | unlock ]
void zclDoorLockCommand(void)
{
  zclSimpleClientCommand(ZCL_DOOR_LOCK_CLUSTER_ID,
                         (emberCurrentCommand->name[0] == 'l'
                          ? ZCL_LOCK_DOOR_COMMAND_ID
                          : ZCL_UNLOCK_DOOR_COMMAND_ID));
}
#endif //ZCL_USING_DOOR_LOCK_CLUSTER_CLIENT

#ifdef ZCL_USING_TUNNELING_CLUSTER_CLIENT
// zcl tunneling request <protocol id:1> <manufacturer code:2> <flow control:1> <maxIncomingTxSize:2>
void zclTunnelingRequestCommand(void)
{
  zclSimpleClientCommand(ZCL_TUNNELING_CLUSTER_ID,
                         ZCL_REQUEST_TUNNEL_COMMAND_ID);
  globalApsFrame.options |= EMBER_APS_OPTION_SOURCE_EUI64;
}

// zcl tunneling close <tunnel id:2>
void zclTunnelingCloseCommand(void)
{
  zclSimpleClientCommand(ZCL_TUNNELING_CLUSTER_ID,
                         ZCL_CLOSE_TUNNEL_COMMAND_ID);
  globalApsFrame.options |= EMBER_APS_OPTION_SOURCE_EUI64;
}

// zcl tunneling transfer-to-server <tunnel id:2> <data>
void zclTunnelingTransferToServerCommand(void)
{
  zclBufferSetup(ZCL_CLUSTER_SPECIFIC_COMMAND
                 | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER,
                 ZCL_TUNNELING_CLUSTER_ID,
                 ZCL_TRANSFER_DATA_CLIENT_TO_SERVER_COMMAND_ID);
  zclBufferAddWordFromArgument(0);              // tunnel id
  appZclBufferLen += emberCopyStringArgument(1, // data
                                             appZclBuffer + appZclBufferLen,
                                             0xFF,   // copy as much data as possible
                                             false); // no padding
  globalApsFrame.options |= EMBER_APS_OPTION_SOURCE_EUI64;
  cliBufferPrint();
}

// zcl tunneling random-to-server <tunnel id:2> <length:2>
void zclTunnelingRandomToServerCommand(void)
{
  uint16_t length = (uint16_t)emberUnsignedCommandArgument(1); // length of random data
  zclBufferSetup(ZCL_CLUSTER_SPECIFIC_COMMAND
                 | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER,
                 ZCL_TUNNELING_CLUSTER_ID,
                 ZCL_TRANSFER_DATA_CLIENT_TO_SERVER_COMMAND_ID);
  zclBufferAddWordFromArgument(0); // tunnel id
  appZclBufferLen = (appZclBufferLen + length < APP_ZCL_BUFFER_SIZE
                     ? appZclBufferLen + length
                     : APP_ZCL_BUFFER_SIZE);
  globalApsFrame.options |= EMBER_APS_OPTION_SOURCE_EUI64;
  cliBufferPrint();
}
#endif //ZCL_USING_TUNNELING_CLUSTER_CLIENT

#ifdef ZCL_USING_TUNNELING_CLUSTER_SERVER
// zcl tunneling transfer-to-client <tunnel id:2> <data>
void zclTunnelingTransferToClientCommand(void)
{
  zclBufferSetup(ZCL_CLUSTER_SPECIFIC_COMMAND
                 | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT,
                 ZCL_TUNNELING_CLUSTER_ID,
                 ZCL_TRANSFER_DATA_SERVER_TO_CLIENT_COMMAND_ID);
  zclBufferAddWordFromArgument(0);              // tunnel id
  appZclBufferLen += emberCopyStringArgument(1, // data
                                             appZclBuffer + appZclBufferLen,
                                             0xFF,   // copy as much data as possible
                                             false); // no padding
  globalApsFrame.options |= EMBER_APS_OPTION_SOURCE_EUI64;
  cliBufferPrint();
}

// zcl tunneling random-to-client <tunnel id:2> <length:2>
void zclTunnelingRandomToClientCommand(void)
{
  uint16_t length = (uint16_t)emberUnsignedCommandArgument(1); // length of random data
  zclBufferSetup(ZCL_CLUSTER_SPECIFIC_COMMAND
                 | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT,
                 ZCL_TUNNELING_CLUSTER_ID,
                 ZCL_TRANSFER_DATA_SERVER_TO_CLIENT_COMMAND_ID);
  zclBufferAddWordFromArgument(0); // tunnel id
  appZclBufferLen = (appZclBufferLen + length < APP_ZCL_BUFFER_SIZE
                     ? appZclBufferLen + length
                     : APP_ZCL_BUFFER_SIZE);
  globalApsFrame.options |= EMBER_APS_OPTION_SOURCE_EUI64;
  cliBufferPrint();
}
#endif //ZCL_USING_TUNNELING_CLUSTER_SERVER

// ******************************************************
// send <id> <src endpoint> <dst endpoint>
// send_multicast <group id> <src endpoint>
//
// FFFC = all routers
// FFFD = all non-sleepy
// FFFF = all devices, include sleepy
// ******************************************************
void emAfCliSendCommand(void)
{
  uint16_t destination = (uint16_t)emberUnsignedCommandArgument(0);
  uint8_t srcEndpoint = (uint8_t)emberUnsignedCommandArgument(1);
  uint8_t dstEndpoint = (uint8_t)emberUnsignedCommandArgument(2);
  uint8_t *commandName = (uint8_t *)emberCurrentCommand->name;
  EmberStatus status;
  uint8_t label;

  // check that cmd is built
  if (zclCmdIsBuilt == false) {
    emberAfCorePrintln("no cmd");
    return;
  }

  emAfApsFrameEndpointSetup(srcEndpoint, dstEndpoint);

  if (emberAfPreCliSendCallback(&globalApsFrame,
                                emberAfGetNodeId(),
                                destination,
                                appZclBuffer,
                                appZclBufferLen)) {
    return;
  }

  if (commandName[4] == '_') {
    label = 'M';
    status = emberAfSendMulticast(destination,
                                  &globalApsFrame,
                                  appZclBufferLen,
                                  appZclBuffer);
  } else if (destination >= EMBER_BROADCAST_ADDRESS) {
    label = 'B';
    status = emberAfSendBroadcast(destination,
                                  &globalApsFrame,
                                  appZclBufferLen,
                                  appZclBuffer);
  } else {
    label = 'U';
    status = emberAfSendUnicast(EMBER_OUTGOING_DIRECT,
                                destination,
                                &globalApsFrame,
                                appZclBufferLen,
                                appZclBuffer);
  }

  if (status != EMBER_SUCCESS) {
    emberAfCorePrintln("Error: CLI Send failed, status: 0x%X", status);
  }
  UNUSED_VAR(label);
  emberAfDebugPrintln("T%4x:TX (%p) %ccast 0x%x%p",
                      emberAfGetCurrentTime(),
                      "CLI",
                      label,
                      status,
                      ((globalApsFrame.options & EMBER_APS_OPTION_ENCRYPTION)
                       ? " w/ link key" : ""));
  emberAfDebugPrint("TX buffer: [");
  emberAfDebugFlush();
  emberAfDebugPrintBuffer(appZclBuffer, appZclBufferLen, true);
  emberAfDebugPrintln("]");
  emberAfDebugFlush();

  zclCmdIsBuilt = false;
  mfgSpecificId = EMBER_AF_NULL_MANUFACTURER_CODE;
  disableDefaultResponse = 0;
  useNextSequence = false;
}

// **********************************************************************
// send-using-multicast-binding <useMulticastBinding> - when sending
//            using a binding, specify whether to use a multicast binding
//
// **********************************************************************
void emAfCliSendUsingMulticastBindingCommand(void)
{
  useMulticastBinding = (bool)emberUnsignedCommandArgument(0);
}

// **********************************************************************
// bsend <src endpoint> - send using a binding based on the clusterId in
//            the globalApsFrame and the srcEndpoint specified (if the
//            src endpoint is zero it only sends based on the clusterId)
//
// **********************************************************************
void emAfCliBsendCommand(void)
{
  uint8_t srcEndpointToUse, i;
  EmberStatus status;
  EmberBindingTableEntry candidate;

  // check that cmd is built
  if (zclCmdIsBuilt == false) {
    emberAfCorePrintln("cmd not built");
    return;
  }

  srcEndpointToUse = (uint8_t)emberUnsignedCommandArgument(0);

  emberAfCorePrintln("src ep %x, clus %2x",
                     srcEndpointToUse,
                     globalApsFrame.clusterId);

  if (useMulticastBinding) {
    emberAfCorePrintln("sending to multicast bind");

    globalApsFrame.sourceEndpoint = srcEndpointToUse;

    status = emberAfSendMulticastToBindings(&globalApsFrame,
                                            appZclBufferLen,
                                            appZclBuffer);

    emberAfDebugPrintln("T%4x:TX (%p) %ccast 0x%x%p",
                        emberAfGetCurrentTime(),
                        "CLI",
                        'U',
                        status,
                        ((globalApsFrame.options & EMBER_APS_OPTION_ENCRYPTION)
                         ? " w/ link key" : ""));
    emberAfDebugPrint("TX buffer: [");
    emberAfDebugFlush();
    emberAfDebugPrintBuffer(appZclBuffer, appZclBufferLen, true);
    emberAfDebugPrintln("]");
    emberAfDebugFlush();
  } else {
    // find a binding to send on
    for (i = 0; i < EMBER_BINDING_TABLE_SIZE; i++) {
      status = emberGetBinding(i, &candidate);

      // if we can read the binding, it is unicast, the endpoint is the
      // one we want (or we have no preference) and the cluster matches
      // then use that binding to send the message
      if (status == EMBER_SUCCESS
          && candidate.type == EMBER_UNICAST_BINDING
          && (srcEndpointToUse == 0
              || candidate.local == srcEndpointToUse)
          && candidate.clusterId == globalApsFrame.clusterId) {
        emberAfCorePrintln("sending to bind %x", i);

        status = emberAfSendUnicast(EMBER_OUTGOING_VIA_BINDING,
                                    i,
                                    &globalApsFrame,
                                    appZclBufferLen,
                                    appZclBuffer);

        emberAfDebugPrintln("T%4x:TX (%p) %ccast 0x%x%p",
                            emberAfGetCurrentTime(),
                            "CLI",
                            'U',
                            status,
                            ((globalApsFrame.options & EMBER_APS_OPTION_ENCRYPTION)
                             ? " w/ link key" : ""));
        emberAfDebugPrint("TX buffer: [");
        emberAfDebugFlush();
        emberAfDebugPrintBuffer(appZclBuffer, appZclBufferLen, true);
        emberAfDebugPrintln("]");
        emberAfDebugFlush();
      }
    }
  }
}

// timesync <id> <src endpoint> <dest endpoint>
//
// This sends a read attr for the time of the device specified. It sets a flag
// so when it gets the response it writes the time to its own time attr
void emAfCliTimesyncCommand(void)
{
  // send a read attr for time
  zclBufferSetup(ZCL_GLOBAL_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER,
                 ZCL_TIME_CLUSTER_ID,
                 ZCL_READ_ATTRIBUTES_COMMAND_ID);
  zclBufferAddWord(ZCL_TIME_ATTRIBUTE_ID);
  emAfSyncingTime = true;
  cliBufferPrint();
  emAfCliSendCommand();
}

// interpan group <groupId:2> <destPAN:2> <destProfileID:2>
// interpan short <shortId:2> <destPAN:2> <destProfileID:2>
void interpanCommand(void)
{
  EmberStatus status;
  EmberAfInterpanHeader header;
  uint16_t shortOrGroupId;

  if (zclCmdIsBuilt == false) {
    emberAfCorePrintln("no cmd");
    return;
  }

  MEMSET(&header, 0, sizeof(EmberAfInterpanHeader));
  shortOrGroupId = (uint16_t)emberUnsignedCommandArgument(0);
  header.panId = (uint16_t)emberUnsignedCommandArgument(1);
  header.profileId = (uint16_t)emberUnsignedCommandArgument(2);
  header.clusterId = globalApsFrame.clusterId;

  if (emberCurrentCommand->name[0] == 'g') {
    header.groupId = shortOrGroupId;
    emberAfDebugPrintln("interpan %p %2x", "group", header.groupId);
  } else {
    header.shortAddress = shortOrGroupId;
    emberAfDebugPrintln("interpan %p %2x", "short", shortOrGroupId);
  }

  status = emberAfInterpanSendMessageCallback(&header,
                                              appZclBufferLen,
                                              appZclBuffer);
  if (status != EMBER_SUCCESS) {
    emberAfDebugPrintln("%psend failed: 0x%X",
                        "ERR: Inter-PAN ",
                        status);
  }
}

// interpan long  <longId:8>  <destPAN:2> <destProfileID:2> <options:2>
//    Options: Bit(0) = encrypt.  Can only encrypt with this CLI command
//    since long address must be present.
void interpanLongCommand(void)
{
  EmberStatus status;
  EmberAfInterpanHeader header;

  if (zclCmdIsBuilt == false) {
    emberAfCorePrintln("no cmd");
    return;
  }

  MEMSET(&header, 0, sizeof(EmberAfInterpanHeader));
  header.panId = (uint16_t)emberUnsignedCommandArgument(1);
  header.profileId = (uint16_t)emberUnsignedCommandArgument(2);
  header.options = ((uint16_t)emberUnsignedCommandArgument(3)
                    | EMBER_AF_INTERPAN_OPTION_MAC_HAS_LONG_ADDRESS);
  header.clusterId = globalApsFrame.clusterId;

  emberCopyBigEndianEui64Argument(0, header.longAddress);

  status = emberAfInterpanSendMessageCallback(&header,
                                              appZclBufferLen,
                                              appZclBuffer);
  if (status != EMBER_SUCCESS) {
    emberAfDebugPrintln("%psend failed: 0x%X",
                        "ERR: Inter-PAN ",
                        status);
  }
}

void printTimeCommand(void)
{
  emberAfPrintTime(emberAfGetCurrentTime());
}

// read <endpoint:1> <cluster:2> <attribute:2> <mask:1>
void emAfCliReadCommand(void)
{
  EmberAfStatus status;
  uint8_t endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  EmberAfClusterId cluster = (EmberAfClusterId)emberUnsignedCommandArgument(1);
  EmberAfAttributeId attribute = (EmberAfAttributeId)emberUnsignedCommandArgument(2);
  bool serverAttribute = (bool)emberUnsignedCommandArgument(3);
  uint8_t data[ATTRIBUTE_LARGEST];
  uint8_t dataType;

  emberAfCorePrint("%p: ep: %d, cl: 0x%2X, attr: 0x%2X",
                   "read",
                   endpoint,
                   cluster,
                   attribute);
  emberAfCorePrintln(", svr: %c", (serverAttribute ? 'y' : 'n'));

  status = emberAfReadAttribute(endpoint,
                                cluster,
                                attribute,
                                (serverAttribute
                                 ? CLUSTER_MASK_SERVER
                                 : CLUSTER_MASK_CLIENT),
                                data,
                                sizeof(data),
                                &dataType);
  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    if (emberAfIsStringAttributeType(dataType)) {
      emberAfCorePrintString(data);
    } else if (emberAfIsLongStringAttributeType(dataType)) {
      emberAfCoreDebugExec(emberAfPrintLongString(EMBER_AF_PRINT_CORE, data));
    } else {
      emberAfCorePrintBuffer(data, emberAfGetDataSize(dataType), true);
    }
    emberAfCorePrintln("");
  } else {
    emberAfCorePrintln("%p: read: 0x%x", "Error", status);
  }
}

// ******************************************************
// write <ep> <cluster> <attrID> <mask> <dataType> <data bytes>
// ******************************************************
void emAfCliWriteCommand(void)
{
  //uint8_t i;
  EmberAfStatus status;
  // Ensure data[] is at least two bytes to accommodate possibility
  // of zero-length LONG string having two-byte length prefix.
  uint8_t data[ATTRIBUTE_LARGEST + 1];

  uint8_t  endpoint  = (uint8_t)emberUnsignedCommandArgument(0);
  uint16_t cluster   = (uint16_t)emberUnsignedCommandArgument(1);
  uint16_t attribute = (uint16_t)emberUnsignedCommandArgument(2);
  bool serverAttribute = (bool)emberUnsignedCommandArgument(3);
  uint8_t  dataType  = (uint8_t)emberUnsignedCommandArgument(4);

  emberAfCorePrint("%p: ep: %d, cl: 0x%2X, attr: 0x%2X",
                   "write",
                   endpoint,
                   cluster,
                   attribute);
  emberAfCorePrintln(", svr: %c, dtype: 0x%X",
                     (serverAttribute ? 'y' : 'n'),
                     dataType);

  // If the data type is a string, automatically prepend a length to the data;
  // otherwise, just copy the raw bytes.
  MEMSET(data, 0, ATTRIBUTE_LARGEST);
  if (emberAfIsStringAttributeType(dataType)) {
    // string, 1-byte length prefix.
    data[0] = emberCopyStringArgument(5,
                                      data + 1,
                                      ATTRIBUTE_LARGEST - 1,
                                      false);
  } else if (emberAfIsLongStringAttributeType(dataType)) {
    // LONG string with 2-byte length prefix.
    // CLI restricts string length to <= 255, so returned length will go
    // into lower prefix byte and upper prefix byte will be zero.
    data[0] = emberCopyStringArgument(5,
                                      data + 2,
                                      ATTRIBUTE_LARGEST - 2,
                                      false);
    data[1] = 0;
  } else {
    emberCopyStringArgument(5,
                            data,
                            emberAfGetDataSize(dataType),
                            false);
  }

  status = emberAfWriteAttribute(endpoint,
                                 cluster,
                                 attribute,
                                 (serverAttribute
                                  ? CLUSTER_MASK_SERVER
                                  : CLUSTER_MASK_CLIENT),
                                 data,
                                 dataType);
  emAfPrintStatus("write", status);
  emberAfCorePrintln("");
}

void eraseKeyTableEntry(uint8_t index)
{
  EmberStatus status = emberEraseKeyTableEntry(index);

  if (status != EMBER_SUCCESS) {
    emberAfCorePrintln("%perase key %d: 0x%x",
                       "ERROR: ",
                       index,
                       status);
  }
}

// keys clear
void keysClearCommand(void)
{
#if EMBER_KEY_TABLE_SIZE
  uint8_t i;
  for (i = 0; i < EMBER_KEY_TABLE_SIZE; i++) {
    eraseKeyTableEntry(i);
  }
#endif
}

// keys print
void keysPrintCommand(void)
{
  emberAfCorePrintln("%pSECURITY_LEVEL: %x",
                     "EMBER_",
                     EMBER_SECURITY_LEVEL);
  printKeyInfo();
}

// keys delete <index:1>
void keysDeleteCommand(void)
{
  uint8_t index = (uint8_t)emberUnsignedCommandArgument(0);
  eraseKeyTableEntry(index);
}

#ifdef CERTIFICATION_TESTING
void attributeReadCausingFragmentedResponseCommand(void)
{
  zclBufferSetup(ZCL_GLOBAL_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER,
                 0x0000,
                 ZCL_READ_ATTRIBUTES_COMMAND_ID);
  zclBufferAddWord(0x0000);
  zclBufferAddWord(0x1000);
  zclBufferAddWord(0x1001);
  zclBufferAddWord(0x1002);
  zclBufferAddWord(0x1003);
  zclBufferAddWord(0x1004);
  zclBufferAddWord(0x1005);
  zclBufferAddWord(0x1006);
  zclBufferAddWord(0x1007);
  zclBufferAddWord(0x1008);
  zclBufferAddWord(0x1009);
  zclBufferAddWord(0x100A);
  zclBufferAddWord(0x100B);
  zclBufferAddWord(0x100C);
  zclBufferAddWord(0x100D);
  zclBufferAddWord(0x100E);
  zclBufferAddWord(0x100F);
  zclBufferAddWord(0x1010);
  zclBufferAddWord(0x1011);
  zclBufferAddWord(0x1012);
  zclBufferAddWord(0x1013);
  zclBufferAddWord(0x1014);
  zclBufferAddWord(0x1015);
  zclBufferAddWord(0x1016);
  zclBufferAddWord(0x1017);
  zclBufferAddWord(0x1018);
  zclBufferAddWord(0x1019);
  zclBufferAddWord(0x101A);
  zclBufferAddWord(0x2001);
  zclBufferAddWord(0x2002);
  zclBufferAddWord(0x2003);
  zclBufferAddWord(0x2004);
  zclBufferAddWord(0x2005);
  zclBufferAddWord(0x2006);
  zclBufferAddWord(0x2007);
  zclBufferAddWord(0x2008);
  zclBufferAddWord(0x2009);
  cliBufferPrint();
}
#endif //CERTIFICATION_TESTING

//******************************************************************
// zcl command table entries
//******************************************************************

#ifndef EMBER_AF_GENERATE_CLI

/**
 * @addtogroup cli
 * @{
 */
/**
 * @brief Commands used to send global commands in the zcl
 *
 *        <b>zcl global read &lt;cluster&gt; &lt;attribute id&gt; </b>
 *        - Creates a global read command message to read from
 *              the cluster and attribute specified.
 *          - cluster - uint16_t. The cluster id to read from.
 *          - attribute id - uint16_t. The attribute id to read from
 *
 *        <b>zcl global write &lt;cluster&gt; &lt;attribute id&gt; &lt;data type&gt; &lt;data&gt;</b>
 *        - Creates a global write  command message to write to
 *              the cluster and attribute specified.
 *          - cluster - uint16_t. The cluster id to read from.
 *          - attribute id - uint16_t. The attribute id to read from
 *          - data type - uint8_t. The two byte zigbee type value
 *          - data - byte array. The data to write
 *
 *        <b>zcl global uwrite &lt;cluster&gt; &lt;attribute id&gt; &lt;data type&gt; &lt;data&gt;</b>
 *        - Creates a global write command message to write to the
 *              cluster and attribute specified.
 *          - cluster - uint16_t. The cluster id to read from.
 *          - attribute id - uint16_t. The attribute id to read from
 *          - data type - uint8_t. The two byte zigbee type value
 *          - data - byte array. The data to write
 *
 *        <b>zcl global nwrite &lt;cluster&gt; &lt;attribute id&gt; &lt;data type&gt; &lt;data&gt;</b>
 *        - Creates a global write command message to write to the
 *              cluster and attribute specified.
 *          - cluster - uint16_t. The cluster id to read from.
 *          - attribute id - uint16_t. The attribute id to read from
 *          - data type - uint8_t. The two byte zigbee type value
 *          - data - byte array. The data to write
 *
 *        <b>zcl global discover &lt;cluster&gt; &lt;attribute id&gt; &lt;max&gt;</b>
 *        - Creates discovery message for devices with the associated
 *              cluster and attribute
 *          - cluster - uint16_t. The cluster id to read from.
 *          - attribute id - uint16_t. The attribute id to read from
 *          - max - uint8_t. Max number of discovery responses.
 *
 *        <b>zcl global report-read &lt;cluster&gt; &lt;attribute id&gt; &lt;direction&gt;</b>
 *        - Creates a global read reporting command for the associated
 *              cluster, attribute and server/client direction.
 *          - cluster - uint16_t. The cluster id to read from.
 *          - attribute id - uint16_t. The attribute id to read from
 *          - direction - uint8_t. 0 for client-to-server, 1 for server-to-client.
 *
 *        <b>zcl global send-me-a-report &lt;cluster&gt; &lt;attribute id&gt; &lt;data type&gt;
 *           &lt;min report time&gt; &lt;max report time&gt; &lt;reportable change&gt;</b>
 *        - Creates a global send me a report command for the associated values.
 *          - cluster - uint16_t. The cluster id for requested report.
 *          - attribute id - uint16_t. The attribute for requested report.
 *          - data type - uint8_t. The two byte zigbee type value for the requested report
 *          - min report time - uint16_t. Minimum number of seconds between reports.
 *          - max report time - uint16_t. Maximum number of seconds between reports.
 *          - reportable change - byte array. Amount of change to trigger a report.
 *
 *        <b>zcl global expect-report-from-me &lt;cluster&gt &lt;attribute id&gt; &lt;timeout&gt;</b>
 *        - Create a expect-report-from-me message with associated values.
 *          - cluster - uint16_t. The cluster id for requested report.
 *          - attribute id - uint16_t. The attribute for requested report.
 *          - timeout - uint16_t. The maximum amount of time between reports.
 *
 *        <b>zcl global direction &lt;direction&gt;</b>
 *        - Sets the direction for global commands, either client to server
 *              or server to client.
 *          - direction - uint8_t. The direction for global commands.
 *            client-to-server == 0, server-to-client == 1.
 *
 *        <b>zcl global disc-com-gen &lt;start command id&gt; &lt;max command ids&gt;</b>
 *        - Sends a global Discover Commands Generated command.
 *          - start command id - uint8_t. The command id to start discovery at.
 *          - max command ids - uint8_t. The maximum number of command ids to return.
 *
 *        <b>zcl global disc-com-rec &lt;start command id&gt; &lt;max command ids&gt;</b>
 *        - Sends a global Discover Commands Received command.
 *          - start command id - uint8_t. The command id to start discovery at.
 *          - max command ids - uint8_t. The maximum number of command ids to return.
 */
#define EMBER_AF_DOXYGEN_CLI__ZCL_GLOBAL_COMMANDS
/** @} END addtogroup */
EmberCommandEntry zclGlobalCommands[] = {
  { "read", zclGlobalReadCommand, "vv" },
  { "write", zclGlobalWriteCommand, "vvub" },
  { "uwrite", zclGlobalWriteCommand, "vvub" },
  { "nwrite", zclGlobalWriteCommand, "vvub" },
  { "discover", zclGlobalDiscoverCommand, "vvu" },
  { "report-read", zclGlobalReportReadCommand, "vvu" },
  { "send-me-a-report", zclGlobalSendMeAReportCommand, "vvuvvb" },
  { "expect-report-from-me", zclGlobalExpectReportFromMeCommand, "vvv" },
  { "report", zclGlobalReportCommand, "uvvu" },
  { "direction", zclGlobalDirectionCommand, "u" },
  { "disc-com-gen", zclGlobalCommandDiscoveryCommand, "vuu" },
  { "disc-com-rec", zclGlobalCommandDiscoveryCommand, "vuu" },
  { NULL }
};

EmberCommandEntry zclTestResponseCommands[] = {
  { "on", zclTestResponseOnCommand, "" },
  { "off", zclTestResponseOffCommand, "" },
  { NULL }
};

EmberCommandEntry zclTestCommands[] = {
  { "response", NULL, (const char *)zclTestResponseCommands },
  { NULL }
};

#ifdef ZCL_USING_BASIC_CLUSTER_CLIENT
EmberCommandEntry zclBasicCommands[] = {
  { "rtfd", zclBasicRtfdCommand, "" },
  { NULL }
};
#endif

#if defined(ZCL_USING_IDENTIFY_CLUSTER_CLIENT) || defined(ZCL_USING_IDENTIFY_CLUSTER_SERVER)
EmberCommandEntry zclIdentifyCommands[] = {
  #ifdef ZCL_USING_IDENTIFY_CLUSTER_CLIENT
  { "id", zclIdentifyIdCommand, "v" },
  { "query", zclIdentifyQueryCommand, "" },
  { "trigger", zclIdentifyTriggerEffectCommand, "uu" },
  #endif
  #ifdef ZCL_USING_IDENTIFY_CLUSTER_SERVER
  { "on", zclIdentifyOnOffCommand, "uv" },
  { "off", zclIdentifyOnOffCommand, "u" },
  #endif
  { NULL }
};
#endif

#ifdef ZCL_USING_GROUPS_CLUSTER_CLIENT
EmberCommandEntry zclGroupsCommands[] = {
  { "add", zclGroupsAddCommand, "vb" },
  { "ad-if-id", zclGroupsAddCommand, "vb" },
  { "view", zclGroupsViewCommand, "v" },
  { "get", zclGroupsGetCommand, "uv*" },
  { "remove", zclGroupsRemoveCommand, "v" },
  { "rmall", zclGroupsRemoveAllCommand, "" },
  { NULL }
};
#endif

#ifdef ZCL_USING_SCENES_CLUSTER_CLIENT
EmberCommandEntry zclScenesSetCommands[] = {
  { "on", zclScenesSetCommand, "u" },
  { "off", zclScenesSetCommand, "u" },
  { NULL }
};

EmberCommandEntry zclScenesCommands[] = {
  { "add", zclScenesAddCommand, "vuvb" },
  { "view", zclScenesViewCommand, "vu" },
  { "remove", zclScenesRemoveCommand, "vu" },
  { "rmall", zclScenesRemoveAllCommand, "v" },
  { "store", zclScenesStoreCommand, "vu" },
  { "recall", zclScenesRecallCommand, "vu" },
  { "get", zclScenesGetMembershipCommand, "v" },
  { "eadd", zclScenesAddCommand, "vuvb" },
  { "eview", zclScenesViewCommand, "vu" },
  { "copy", zclScenesCopyCommand, "uvuvu" },
  { "set", NULL, (const char *)zclScenesSetCommands },
  { NULL }
};
#endif

#ifdef ZCL_USING_ON_OFF_CLUSTER_CLIENT
EmberCommandEntry zclOnOffCommands[] = {
  { "on", zclOnOffCommand, "" },
  { "off", zclOnOffCommand, "" },
  { "toggle", zclOnOffCommand, "" },
  { "offeffect", zclOnOffOffWithEffectCommand, "uu" },
  { "onrecall", zclOnOffOnWithRecallGlobalSceneCommand, "" },
  { "ontimedoff", zclOnOffOnWithTimedOffCommand, "uvv" },
  { NULL }
};
#endif

#ifdef ZCL_USING_LEVEL_CONTROL_CLUSTER_CLIENT
EmberCommandEntry zclLevelControlCommands[] = {
  { "mv-to-level", zclLevelControlMoveToLevelCommand, "uv" },
  { "move", zclLevelControlMoveCommand, "uu" },
  { "step", zclLevelControlStepCommand, "uuv" },
  { "stop", zclLevelControlStopCommand, "" },
  { "o-mv-to-level", zclLevelControlMoveToLevelCommand, "uv" },
  { "o-move", zclLevelControlMoveCommand, "uu" },
  { "o-step", zclLevelControlStepCommand, "uuv" },
  { "o-stop", zclLevelControlStopCommand, "" },
  { NULL }
};
#endif

#ifdef ZCL_USING_THERMOSTAT_CLUSTER_CLIENT
EmberCommandEntry zclThermostatCommands[] = {
  { "set", zclThermostatSetCommand, "uu" },
  { NULL }
};
#endif

#ifdef ZCL_USING_IAS_ZONE_CLUSTER_SERVER
EmberCommandEntry zclIasZoneCommands[] = {
  { "enroll", zclIasZoneEnrollCommand, "vv" },
  { "sc", zclIasZoneStatusChangeCommand, "vuuv" },
  { NULL }
};
#endif

#ifdef ZCL_USING_IAS_ACE_CLUSTER_CLIENT
EmberCommandEntry zclIasAceCommands[] = {
  { "a", zclIasAceArmCommand, "ubu" },
  { "b", zclIasAceBypassCommand, "b" },
  { "e", zclIasAceEmergencyCommand, "" },
  { "f", zclIasAceFireCommand, "" },
  { "p", zclIasAcePanicCommand, "" },
  { "getzm", zclIasAceGetZoneMapCommand, "" },
  { "getzi", zclIasAceGetZoneInfoCommand, "u" },
  { NULL }
};
#endif

#ifdef ZCL_USING_COLOR_CONTROL_CLUSTER_CLIENT
EmberCommandEntry zclColorControlCommands[] = {
  { "movetohue", zclColorControlMoveToHueCommand, "uuv" },
  { "movehue", zclColorControlMoveHueCommand, "uu" },
  { "stephue", zclColorControlStepHueCommand, "uuu" },
  { "movetosat", zclColorControlMoveToSatCommand, "uv" },
  { "movesat", zclColorControlMoveSatCommand, "uu" },
  { "stepsat", zclColorControlStepSatCommand, "uuu" },
  { "movetohueandsat", zclColorControlMoveToHueAndSatCommand, "uuv" },
  { "movetocolor", zclColorControlMoveToColorCommand, "vvv" },
  { "movecolor", zclColorControlMoveColorCommand, "vv" },
  { "stepcolor", zclColorControlStepColorCommand, "vvv" },
  { "movetocolortemp", zclColorControlMoveToColorTemperatureCommand, "vv" },
  { "emovetohue", zclColorControlMoveToHueCommand, "vuv" },
  { "emovehue", zclColorControlMoveHueCommand, "uv" },
  { "estephue", zclColorControlStepHueCommand, "uvv" },
  { "emovetohueandsat", zclColorControlMoveToHueAndSatCommand, "vuv" },
  { "loop", zclColorControlColorLoopSetCommand, "uuuvv" },
  { "stopmovestep", zclColorControlStopMoveStepCommand, "" },
  { "movecolortemp", zclColorControlMoveColorCommand, "uvvv" },
  { "stepcolortemp", zclColorControlStepColorCommand, "uvvvv" },
  { NULL }
};
#endif

#ifdef ZCL_USING_POLL_CONTROL_CLUSTER_CLIENT
EmberCommandEntry zclPollControlCommands[] = {
  emberCommandEntryAction("stop", zclPollControlFastPollStopCommand, "", "Fast Poll Stop"),
  emberCommandEntryAction("long", zclPollControlSetPollIntervalCommand, "w", "Set Long Poll Interval"),
  emberCommandEntryAction("short", zclPollControlSetPollIntervalCommand, "v", "Set Short Poll Interval"),
  emberCommandEntryTerminator(),
};
#endif

#ifdef ZCL_USING_POWER_PROFILE_CLUSTER_CLIENT
EmberCommandEntry zclPowerProfileCommands[] = {
  emberCommandEntryAction("profile", zclPowerProfileRequestCommand,
                          "u", "Request Profile"),
  emberCommandEntryAction("state", zclPowerProfileStateRequestCommand,
                          "", "Request State"),
  emberCommandEntryAction("energy-phases-schedule",
                          zclPowerProfileEnergyPhasesScheduleNotificationCommand,
                          "?", "Notify Energy Phase Schedule"),
  emberCommandEntryAction("schedule-constraints",
                          zclPowerProfileScheduleConstraintsRequestCommand,
                          "u", "Request Schedule Constraints"),
  emberCommandEntryAction("energy-phases-schedule-states",
                          zclPowerProfileEnergyPhasesScheduleStateRequestCommand,
                          "u", "Request Energy Phases Schedule States"),
  emberCommandEntryTerminator(),
};
#endif //ZCL_USING_POWER_PROFILE_CLUSTER_CLIENT

#ifdef ZCL_USING_WINDOW_COVERING_CLUSTER_CLIENT
EmberCommandEntry zclWindowCoveringCommands[] = {
  emberCommandEntryAction("up", zclWindowCoveringUpCommand,
                          "", "Up"),
  emberCommandEntryAction("down", zclWindowCoveringDownCommand,
                          "", "Down"),
  emberCommandEntryAction("stop",
                          zclWindowCoveringStopCommand,
                          "", "Stop"),
  emberCommandEntryAction("go-to-lift-value",
                          zclWindowCoveringGoToLiftValueCommand,
                          "v", "Go To Lift Value"),
  emberCommandEntryAction("go-to-lift-percent",
                          zclWindowCoveringGoToLiftPercentageCommand,
                          "u", "Go To Lift Percentage"),
  emberCommandEntryAction("go-to-tilt-value",
                          zclWindowCoveringGoToTiltValueCommand,
                          "v", "Go To Tilt Value"),
  emberCommandEntryAction("go-to-tilt-percentage",
                          zclWindowCoveringGoToTiltPercentageCommand,
                          "u", "Go To Tilt Percentage"),
  emberCommandEntryTerminator(),
};
#endif //ZCL_USING_WINDOW_COVERING_CLUSTER_CLIENT

#if defined(ZCL_USING_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_CLIENT) || defined(ZCL_USING_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_SERVER)

EmberCommandEntry zclDrlcCommands[] = {
  #ifdef ZCL_USING_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_SERVER

  { "lce", zclDrlcLoadControlEventCommand, "wwvu" },
  { "cl", zclDrlcCancelCommand, "wvuuw" },
  { "ca", zclDrlcCancelAllCommand, "" },
  #endif
  #ifdef ZCL_USING_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_CLIENT
  { "gse", zclDrlcGetScheduledEventsCommand, "wu" },
  #endif
  { NULL }
};
#endif //ZCL_USING_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_CLIENT || SERVER

#if defined(ZCL_USING_SIMPLE_METERING_CLUSTER_CLIENT) || defined(ZCL_USING_SIMPLE_METERING_CLUSTER_SERVER)
EmberCommandEntry zclSimpleMeteringCommands[] = {
#ifdef ZCL_USING_SIMPLE_METERING_CLUSTER_CLIENT
  { "gp", zclSimpleMeteringGetProfileCommand, "uwu" },
  { "fp", zclSimpleMeteringFastPollCommand, "uu" },
#endif //ZCL_USING_SIMPLE_METERING_CLUSTER_CLIENT
#ifdef ZCL_USING_SIMPLE_METERING_CLUSTER_SERVER
  { "rm", zclSimpleMeteringRequestMirrorCommand, "" },
  { "dm", zclSimpleMeteringDeleteMirrorCommand, "" },
#endif //ZCL_USING_SIMPLE_METERING_CLUSTER_SERVER
  { NULL }
};
#endif //ZCL_USING_SIMPLE_METERING_CLUSTER_CLIENT || ZCL_USING_SIMPLE_METERING_CLUSTER_SERVER

#ifdef ZCL_USING_PRICE_CLUSTER_CLIENT
EmberCommandEntry zclPriceClientCommands[] = {
  { "cu", zclPriceGetCurrentCommand, "" },
  { "sc", zclPriceGetScheduleCommand, "wu" },
  { NULL }
};
#endif //ZCL_USING_PRICE_CLUSTER_CLIENT

EmberCommandEntry zclMessageCommands[] = {
#ifdef ZCL_USING_MESSAGING_CLUSTER_CLIENT
  { "get", zclMessageGetCommand, "" },
#endif //ZCL_USING_MESSAGING_CLUSTER_CLIENT
  { NULL }
};

#if defined(ZCL_USING_GENERIC_TUNNEL_CLUSTER_CLIENT) || defined(ZCL_USING_GENERIC_TUNNEL_CLUSTER_SERVER)
EmberCommandEntry zclTunnelCommands[] = {
#ifdef ZCL_USING_GENERIC_TUNNEL_CLUSTER_CLIENT
  { "match", zclTunnelMatchCommand, "b" },
#endif //ZCL_USING_GENERIC_TUNNEL_CLUSTER_CLIENT
#ifdef ZCL_USING_GENERIC_TUNNEL_CLUSTER_SERVER
  { "advertise", zclTunnelAdvertiseCommand, "b" },
  { "response", zclTunnelResponseCommand, "" },
#endif //ZCL_USING_GENERIC_TUNNEL_CLUSTER_SERVER
  { NULL }
};
#endif //ZCL_USING_GENERIC_TUNNEL_CLUSTER_CLIENT || ZCL_USING_GENERIC_TUNNEL_CLUSTER_SERVER

#ifdef ZCL_USING_BACNET_PROTOCOL_TUNNEL_CLUSTER_CLIENT
EmberCommandEntry zclBacnetTransferCommands[] = {
  { "fixed", zclBacnetTransferFixedCommand, "ub" },
  { "random", zclBacnetTransferRandomCommand, "u" },
  { "whois", zclBacnetTransferWhoisCommand, "" }, //removed length byte from whois
  { NULL },
};

EmberCommandEntry zclBacnetCommands[] = {
  { "transfer-npdu", NULL, (const char *)zclBacnetTransferCommands },
  { NULL }
};
#endif

#ifdef ZCL_USING_DOOR_LOCK_CLUSTER_CLIENT
EmberCommandEntry zclDoorLockCommands[] = {
  { "lock", zclDoorLockCommand, "" },
  { "unlock", zclDoorLockCommand, "" },
  { NULL }
};
#endif //ZCL_USING_DOOR_LOCK_CLUSTER_CLIENT

#if defined(ZCL_USING_TUNNELING_CLUSTER_CLIENT) || defined(ZCL_USING_TUNNELING_CLUSTER_SERVER)
EmberCommandEntry zclTunnelingCommands[] = {
#ifdef ZCL_USING_TUNNELING_CLUSTER_CLIENT
  { "request", zclTunnelingRequestCommand, "uvuv" },
  { "close", zclTunnelingCloseCommand, "v" },
  { "transfer-to-server", zclTunnelingTransferToServerCommand, "vb" },
  { "random-to-server", zclTunnelingRandomToServerCommand, "vv" },
#endif //ZCL_USING_TUNNELING_CLUSTER_CLIENT
#ifdef ZCL_USING_TUNNELING_CLUSTER_SERVER
  { "transfer-to-client", zclTunnelingTransferToClientCommand, "vb" },
  { "random-to-client", zclTunnelingRandomToClientCommand, "vv" },
#endif //ZCL_USING_TUNNELING_CLUSTER_SERVER
  { NULL }
};
#endif //defined(ZCL_USING_TUNNELING_CLUSTER_CLIENT) || defined(ZCL_USING_TUNNELING_CLUSTER_SERVER)

EmberCommandEntry zclCommands[] = {
//#define CERTIFICATION_TESTING
#ifdef CERTIFICATION_TESTING
  { "attr-read-frag-resp", attributeReadCausingFragmentedResponseCommand, "" },
#endif //CERTIFICATION_TESTING

  { "mfg-code", zclMfgCodeCommand, "v" },
  { "x-default-resp", zclXDefaultRespCommand, "u" },

  { "global", NULL, (const char *)zclGlobalCommands },
  { "test", NULL, (const char *)zclTestCommands },
  { "time", zclTimeCommand, "w" },
  #ifdef ZCL_USING_BASIC_CLUSTER_CLIENT
  { "basic", NULL, (const char *)zclBasicCommands },
  #endif

  #if defined(ZCL_USING_IDENTIFY_CLUSTER_CLIENT) || defined(ZCL_USING_IDENTIFY_CLUSTER_SERVER)
  { "identify", NULL, (const char *)zclIdentifyCommands },
  #endif //ZCL_USING_IDENTIFY_CLUSTER_CLIENT || SERVER
  #ifdef ZCL_USING_GROUPS_CLUSTER_CLIENT
  { "groups", NULL, (const char *)zclGroupsCommands },
  #endif
  #ifdef ZCL_USING_SCENES_CLUSTER_CLIENT
  { "scenes", NULL, (const char *)zclScenesCommands },
  #endif
  #ifdef ZCL_USING_ON_OFF_CLUSTER_CLIENT
  { "on-off", NULL, (const char *)zclOnOffCommands },
  #endif
  #ifdef ZCL_USING_LEVEL_CONTROL_CLUSTER_CLIENT
  { "level-control", NULL, (const char *)zclLevelControlCommands },
  #endif
  #ifdef ZCL_USING_THERMOSTAT_CLUSTER_CLIENT
  { "tstat", NULL, (const char *)zclThermostatCommands },
  #endif
  #ifdef ZCL_USING_IAS_ZONE_CLUSTER_SERVER
  { "ias-zone", NULL, (const char *)zclIasZoneCommands },
  #endif
  #ifdef ZCL_USING_IAS_ACE_CLUSTER_CLIENT
  { "ias-ace", NULL, (const char *)zclIasAceCommands },
  #endif
  #ifdef ZCL_USING_POLL_CONTROL_CLUSTER_CLIENT
  emberCommandEntrySubMenu("poll-control", zclPollControlCommands, "Poll Control commands"),
  #endif
  #ifdef ZCL_USING_POWER_PROFILE_CLUSTER_CLIENT
  emberCommandEntrySubMenu("power-profile", zclPowerProfileCommands, "Power Profile commands"),
  #endif
  #ifdef ZCL_USING_WINDOW_COVERING_CLUSTER_CLIENT
  emberCommandEntrySubMenu("window-covering", zclWindowCoveringCommands, "Window Covering commands"),
  #endif
  #ifdef ZCL_USING_COLOR_CONTROL_CLUSTER_CLIENT
  { "color-control", NULL, (const char *)zclColorControlCommands },
  #endif
#if defined(ZCL_USING_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_CLIENT) || defined(ZCL_USING_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_SERVER)
  { "drlc", NULL, (const char *)zclDrlcCommands },
#endif //ZCL_USING_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_CLIENT || SERVER
#if defined(ZCL_USING_SIMPLE_METERING_CLUSTER_CLIENT) || defined(ZCL_USING_SIMPLE_METERING_CLUSTER_SERVER)
  { "sm", NULL, (const char *)zclSimpleMeteringCommands },
#endif //ZCL_USING_SIMPLE_METERING_CLUSTER_CLIENT || ZCL_USING_SIMPLE_METERING_CLUSTER_SERVER
  #ifdef ZCL_USING_PRICE_CLUSTER_CLIENT
  { "pr", NULL, (const char *)zclPriceClientCommands },
  #endif
  #if defined(ZCL_USING_MESSAGING_CLUSTER_CLIENT)
  { "message", NULL, (const char *)zclMessageCommands },
  #endif
#if defined(ZCL_USING_GENERIC_TUNNEL_CLUSTER_CLIENT) || defined(ZCL_USING_GENERIC_TUNNEL_CLUSTER_SERVER)
  { "tunnel", NULL, (const char *)zclTunnelCommands },
#endif //ZCL_USING_GENERIC_TUNNEL_CLUSTER_CLIENT || ZCL_USING_GENERIC_TUNNEL_CLUSTER_SERVER
  #ifdef ZCL_USING_BACNET_PROTOCOL_TUNNEL_CLUSTER_CLIENT
  { "bacnet", NULL, (const char *)zclBacnetCommands },
  #endif

  #ifdef ZCL_USING_DOOR_LOCK_CLUSTER_CLIENT
  { "lock", NULL, (const char *)zclDoorLockCommands },
  #endif //ZCL_USING_DOOR_LOCK_CLUSTER_CLIENT

#if defined(ZCL_USING_TUNNELING_CLUSTER_CLIENT) || defined(ZCL_USING_TUNNELING_CLUSTER_SERVER)
  { "tunneling", NULL, (const char *)zclTunnelingCommands },
#endif //defined(ZCL_USING_TUNNELING_CLUSTER_CLIENT) || defined(ZCL_USING_TUNNELING_CLUSTER_SERVER)

  { NULL }
};

// ******************************************************
// keys command functions
// ******************************************************

EmberCommandEntry keysCommands[] = {
  emberCommandEntryAction("clear", keysClearCommand, "", "Clear all link keys"),
  emberCommandEntryAction("print", keysPrintCommand, "", "Print the NWK and link key table"),
  emberCommandEntryAction("delete", keysDeleteCommand, "u", "Delete the specified link key index"),
  emberCommandEntryTerminator(),
};

// ******************************************************
// Print commands.
//
// print attr
// print time
// ******************************************************

EmberCommandEntry printCommands[] = {
  emberCommandEntryAction("time", printTimeCommand, "", "Print current time attribute"),
  emberCommandEntryAction("attr", emberAfPrintAttributeTable, "", "Print the attribute table"),
  emberCommandEntryTerminator(),
};

// ******************************************************
// interpan commands
//
// mac is     : [FC 2; seq 1; dest PAN 2; dest addr 2|8; src pan 2; src addr 8]
// stub nwk is: [Frame Control 2]
// stub APS is: [FC 1; groupId 0|2; clusterId 2; appProfileId 2]
//
// MAC FC = bits 0,1,2: frame type (Data) 1
//          bit 3: security enabled = false (0)
//          bit 4: frame pending  = false (0)
//          bit 5: ack required = true (1)
//          bit 6: intra PAN = false (0)
//          bit 7,8,9: reserved (0)
//          bit 10,11: dest addr mode = group (1), short (2), long(3)
//          bit 12,13: reserved (0)
//          bit 14,15: src addr mode = long(3)
//   for GROUP_DEST FC is: 0xC421
//   for SHORT_DEST FC is: 0xC821
//   for LONG_DEST FC is : 0xCC21
//
// Stub NWK FC: bits 0,1: frame type 3 (0b11)
//              bits 2-5: protocol version 2 (0b0010)
//              bits 6-15: reserved (0)
//   this is always 0x000B
//
// stub APS FC: bits 0,1: 3 (0b11)
//              bits 2,3: delivery mode unicast(0), bcast(2), group(3)
//              bit 4: reserved (0)
//              bit 5: security: none (0)
//              bit 6: ack request: NO ack (0)
//              bit 7: ext header present: No (0)
//   this is 03 for unicast, 0B for bcast, 0F for group
//
// ******************************************************

EmberCommandEntry interpanCommands[] = {
  { "group", interpanCommand, "vvv" },
  { "short", interpanCommand, "vvv" },
  { "long", interpanLongCommand, "bvvv" },
  { NULL }
};

#endif // EMBER_AF_GENERATE_CLI

#endif // !UC_BUILD

// Common Functions for UC_BUILD and !UC_BUILD
void emAfApsFrameEndpointSetup(uint8_t srcEndpoint,
                               uint8_t dstEndpoint)
{
  globalApsFrame.sourceEndpoint = (srcEndpoint == 0
                                   ? emberAfPrimaryEndpointForCurrentNetworkIndex()
                                   : srcEndpoint);
  globalApsFrame.destinationEndpoint = dstEndpoint;
}
