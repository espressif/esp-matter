/***************************************************************************//**
 * @file
 * @brief Routines for the Reporting plugin, which sends asynchronous reports
 *        when a ZCL attribute's value has changed.
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

#include "app/framework/include/af.h"
#include "app/framework/util/common.h"
#include "app/framework/util/attribute-storage.h"
#include "reporting.h"

#ifdef ATTRIBUTE_LARGEST
#define READ_DATA_SIZE ATTRIBUTE_LARGEST
#else
#define READ_DATA_SIZE 8 // max size if attributes aren't present
#endif

static uint16_t reportTableActiveLength = 0;

static void conditionallySendReport(uint8_t endpoint, EmberAfClusterId clusterId);
static void scheduleTick(void);
static void removeConfiguration(uint16_t index);
static void removeConfigurationAndScheduleTick(uint16_t index);
static EmberAfStatus configureReceivedAttribute(const EmberAfClusterCommand *cmd,
                                                EmberAfAttributeId attributeId,
                                                uint8_t mask,
                                                uint16_t timeout);
static void putReportableChangeInResp(const EmberAfPluginReportingEntry *entry,
                                      EmberAfAttributeType dataType);
static void retrySendReport(EmberOutgoingMessageType type,
                            uint16_t indexOrDestination,
                            EmberApsFrame *apsFrame,
                            uint16_t msgLen,
                            uint8_t *message,
                            EmberStatus status);
static uint32_t computeStringHash(uint8_t *data, uint8_t length);
static EmberStatus readAttributeAndGetLastValue(const EmberAfPluginReportingEntry* const entry,
                                                uint8_t entryIndex,
                                                EmberAfAttributeType* pDataType,
                                                uint16_t* pDataSize,
                                                uint8_t* pReadData,
                                                uint16_t readDataSize,
                                                bool reportChange);
static void markReportTableChange(uint8_t *dataRef,
                                  uint8_t dataSize,
                                  EmberAfAttributeType dataType,
                                  const EmberAfPluginReportingEntry* const entry,
                                  uint8_t entryIndex);
#ifdef UC_BUILD
sl_zigbee_event_t emberAfPluginReportingTickEvent;
#define tickEvent (&emberAfPluginReportingTickEvent)
void emberAfPluginReportingTickEventHandler(SLXU_UC_EVENT);
#else
EmberEventControl emberAfPluginReportingTickEventControl;
#define tickEvent emberAfPluginReportingTickEventControl
#endif // UC_BUILD

EmAfPluginReportVolatileData emAfPluginReportVolatileData[REPORT_TABLE_SIZE];

static void retrySendReport(EmberOutgoingMessageType type,
                            uint16_t indexOrDestination,
                            EmberApsFrame *apsFrame,
                            uint16_t msgLen,
                            uint8_t *message,
                            EmberStatus status)
{
  // Retry once, and do so by unicasting without a pointer to this callback
  if (status != EMBER_SUCCESS) {
    emberAfSendUnicast(type, indexOrDestination, apsFrame, msgLen, message);
  }
}

// Implementation based on public domain Fowler/Noll/Vo FNV-1a hash function:
// http://isthe.com/chongo/tech/comp/fnv/
// https://tools.ietf.org/html/draft-eastlake-fnv-14
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
//
// Used to save and compare hashes of CHAR and OCTET string values in order to detect
// reportable changes. The strings themselves are longer than the storage size.
#define FNV1_OFFSET_BASIS (2166136261)
#define FNV1_PRIME (16777619)
static uint32_t computeStringHash(uint8_t *data, uint8_t length)
{
  // FNV-1a, 32-bit hash
  uint32_t hash = FNV1_OFFSET_BASIS;
  for (int i = 0; i < length; ++i) {
    hash ^= data[i];
    hash *= FNV1_PRIME; // Or, hash += (hash<<1) + (hash<<4) + (hash<<7) + (hash<<8) + (hash<<24);
  }
  return hash;
}

#define ifValidIndex(expr) do {      \
    if (index < REPORT_TABLE_SIZE) { \
      expr;                          \
    }                                \
} while (0)

#ifdef EZSP_HOST
static EmberAfPluginReportingEntry table[REPORT_TABLE_SIZE];
void emAfPluginReportingGetEntry(uint16_t index, EmberAfPluginReportingEntry *result)
{
  ifValidIndex(MEMMOVE(result, &table[index], sizeof(EmberAfPluginReportingEntry)));
}
void emAfPluginReportingSetEntry(uint16_t index, EmberAfPluginReportingEntry *value)
{
  ifValidIndex(MEMMOVE(&table[index], value, sizeof(EmberAfPluginReportingEntry)));
}
#elif (ENABLE_EXPANDED_TABLE)
#define reportingTableKey(index) (NVM3KEY_REPORTING_TABLE_EXPANDED + (index))
void emAfPluginReportingGetEntry(uint16_t index, EmberAfPluginReportingEntry *result)
{
  ifValidIndex(nvm3_readData(nvm3_defaultHandle, reportingTableKey(index), result, sizeof(EmberAfPluginReportingEntry)));
}
void emAfPluginReportingSetEntry(uint16_t index, EmberAfPluginReportingEntry *value)
{
  ifValidIndex(nvm3_writeData(nvm3_defaultHandle, reportingTableKey(index), value, sizeof(EmberAfPluginReportingEntry)));
}
#else
void emAfPluginReportingGetEntry(uint16_t index, EmberAfPluginReportingEntry *result)
{
  ifValidIndex(halCommonGetIndexedToken(result, TOKEN_REPORT_TABLE, index));
}
void emAfPluginReportingSetEntry(uint16_t index, EmberAfPluginReportingEntry *value)
{
  ifValidIndex(halCommonSetIndexedToken(TOKEN_REPORT_TABLE, index, value));
}
#endif

// TODO: renamed for naming consistency purposes
#ifdef UC_BUILD
void emAfPluginReportingStackStatusCallback(EmberStatus status)
#else
void emberAfPluginReportingStackStatusCallback(EmberStatus status)
#endif
{
  if (status == EMBER_NETWORK_UP) {
    // Load default reporting configurations
    emberAfPluginReportingLoadReportingConfigDefaults();

    scheduleTick();
  }
}

#ifdef UC_BUILD

void emberAfPluginReportingInitCallback(uint8_t init_level)
{
  switch (init_level) {
    case SL_ZIGBEE_INIT_LEVEL_EVENT:
    {
      slxu_zigbee_event_init(tickEvent,
                             emberAfPluginReportingTickEventHandler);
      break;
    }

    case SL_ZIGBEE_INIT_LEVEL_DONE:
    {
      // Deferring this to the DONE phase as it relies on endpoint
      // enable/disable state having first been initialized. That
      // state is referenced directly here, and in tick scheduler.
      //
      // On device initialization, any attributes that have been set up to report
      // should generate an attribute report.
      uint16_t i;
      for (i = 0; i < REPORT_TABLE_SIZE; i++) {
        EmberAfPluginReportingEntry entry;
        entry.endpoint = EMBER_AF_PLUGIN_REPORTING_UNUSED_ENDPOINT_ID;
        emAfPluginReportingGetEntry(i, &entry);
        if (entry.endpoint == EMBER_AF_PLUGIN_REPORTING_UNUSED_ENDPOINT_ID) {
          // we structure the table so that all "active" entries fall within
          // a single range.  When we encounter an unused entry, assume
          // we've reached the end of active entries and break the loop
          break;
        }
        if (emberAfEndpointIsEnabled(entry.endpoint)
            && entry.direction == EMBER_ZCL_REPORTING_DIRECTION_REPORTED) {
          emAfPluginReportVolatileData[i].reportableChange = true;
        }
      }
      reportTableActiveLength = i;
      scheduleTick();
      break;
    }

    default:
      // MISRA requires default case.
      break;
  }
}

#else // !UC_BUILD

void emberAfPluginReportingInitCallback(void)
{
  // On device initialization, any attributes that have been set up to report
  // should generate an attribute report.
  uint16_t i;

  for (i = 0; i < REPORT_TABLE_SIZE; i++) {
    EmberAfPluginReportingEntry entry;
    entry.endpoint = EMBER_AF_PLUGIN_REPORTING_UNUSED_ENDPOINT_ID;
    emAfPluginReportingGetEntry(i, &entry);
    if (entry.endpoint == EMBER_AF_PLUGIN_REPORTING_UNUSED_ENDPOINT_ID) {
      // we structure the table so that all "active" entries fall within
      // a single range.  When we encounter an unused entry, assume
      // we've reached the end of active entries and break the loop
      break;
    }
    if (emberAfEndpointIsEnabled(entry.endpoint)
        && entry.direction == EMBER_ZCL_REPORTING_DIRECTION_REPORTED) {
      emAfPluginReportVolatileData[i].reportableChange = true;
    }
  }

  reportTableActiveLength = i;
  scheduleTick();
}

#endif // UC_BUILD

uint16_t emAfPluginReportingNumEntries(void)
{
  return reportTableActiveLength;
}

void emberAfPluginReportingTickEventHandler(SLXU_UC_EVENT)
{
  EmberApsFrame *apsFrame = NULL;
  EmberAfStatus status;
  EmberAfAttributeType dataType;
  uint16_t manufacturerCode = 0;
  uint8_t readData[READ_DATA_SIZE];
  uint16_t dataSize;
  bool clientToServer = false;
  EmberBindingTableEntry bindingEntry;
  uint8_t reportSize = 0, currentPayloadMaxLength = 0, smallestPayloadMaxLength = 0;
  uint16_t i;

  for (i = 0; i < reportTableActiveLength; i++) {
    EmberAfPluginReportingEntry entry;
    uint32_t elapsedMs;
    emAfPluginReportingGetEntry(i, &entry);
    // We will only send reports for active reported attributes and only if a
    // reportable change has occurred and the minimum interval has elapsed or
    // if the maximum interval is set and has elapsed.
    elapsedMs = elapsedTimeInt32u(emAfPluginReportVolatileData[i].lastReportTimeMs,
                                  halCommonGetInt32uMillisecondTick());
    if (!emberAfEndpointIsEnabled(entry.endpoint)
        || entry.direction != EMBER_ZCL_REPORTING_DIRECTION_REPORTED
        || (elapsedMs
            < entry.data.reported.minInterval * MILLISECOND_TICKS_PER_SECOND)
        || (!emAfPluginReportVolatileData[i].reportableChange
            && (entry.data.reported.maxInterval == 0
                || (elapsedMs
                    < (entry.data.reported.maxInterval
                       * MILLISECOND_TICKS_PER_SECOND))))) {
      continue;
    }
    status = readAttributeAndGetLastValue(&entry, i, &dataType, &dataSize, readData, READ_DATA_SIZE, false);
    if (status != EMBER_ZCL_STATUS_SUCCESS) {
      goto skipAttribute;
    }

    reportSize = sizeof(entry.attributeId) + sizeof(dataType) + dataSize;

    // If we have already started a report for a different attribute or
    // destination, or if the current entry is too big for current report, send it and create a new one.
    if (apsFrame != NULL
        && (!(entry.endpoint == apsFrame->sourceEndpoint
              && entry.clusterId == apsFrame->clusterId
              && emberAfClusterIsClient(&entry) == clientToServer
              && entry.manufacturerCode == manufacturerCode)
            || (appResponseLength + reportSize > smallestPayloadMaxLength))) {
      if (appResponseLength + reportSize > smallestPayloadMaxLength) {
        emberAfReportingPrintln("Reporting Entry Full - creating new report");
      }
      conditionallySendReport(apsFrame->sourceEndpoint, apsFrame->clusterId);
      apsFrame = NULL;
    }

    // If we haven't made the message header, make it.
    if (apsFrame == NULL) {
      apsFrame = emberAfGetCommandApsFrame();
      clientToServer = emberAfClusterIsClient(&entry);
      // The manufacturer-specfic version of the fill API only creates a
      // manufacturer-specfic command if the manufacturer code is set.  For
      // non-manufacturer-specfic reports, the manufacturer code is unset, so
      // we can get away with using this API for both cases.
      emberAfFillExternalManufacturerSpecificBuffer((clientToServer
                                                     ? (ZCL_GLOBAL_COMMAND
                                                        | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
                                                        | EMBER_AF_DEFAULT_RESPONSE_POLICY_REQUESTS)
                                                     : (ZCL_GLOBAL_COMMAND
                                                        | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT
                                                        | EMBER_AF_DEFAULT_RESPONSE_POLICY_REQUESTS)),
                                                    entry.clusterId,
                                                    entry.manufacturerCode,
                                                    ZCL_REPORT_ATTRIBUTES_COMMAND_ID,
                                                    "");
      apsFrame->sourceEndpoint = entry.endpoint;
      apsFrame->options = EMBER_AF_DEFAULT_APS_OPTIONS;
      manufacturerCode = entry.manufacturerCode;

      // EMAPPFWKV2-1327: Reporting plugin does not account for reporting too many attributes
      //                  in the same ZCL:ReportAttributes message

      // find smallest maximum payload that the destination can receive for this cluster and source endpoint
      uint8_t index;
      smallestPayloadMaxLength = MAX_INT8U_VALUE;
      for (index = 0; index < EMBER_BINDING_TABLE_SIZE; index++) {
        status = (EmberAfStatus)emberGetBinding(index, &bindingEntry);
        if (status == (EmberAfStatus)EMBER_SUCCESS && bindingEntry.local == entry.endpoint && bindingEntry.clusterId == entry.clusterId) {
          currentPayloadMaxLength = emberAfMaximumApsPayloadLength(bindingEntry.type, bindingEntry.networkIndex, apsFrame);
          if (currentPayloadMaxLength < smallestPayloadMaxLength) {
            smallestPayloadMaxLength = currentPayloadMaxLength;
          }
        }
      }
    }

    // Payload is [attribute id:2] [type:1] [data:N].
    (void) emberAfPutInt16uInResp(entry.attributeId);
    (void) emberAfPutInt8uInResp(dataType);

#if (BIGENDIAN_CPU)
    if (isThisDataTypeSentLittleEndianOTA(dataType)) {
      uint8_t i;
      for (i = 0; i < dataSize; i++) {
        (void) emberAfPutInt8uInResp(readData[dataSize - i - 1]);
      }
    } else {
      emberAfPutBlockInResp(readData, dataSize);
    }
#else
    emberAfPutBlockInResp(readData, dataSize);
#endif

    // Normally will arrive here at the conclusion of attribute processing.
    // Update the state used to decide if an attribute value is ready to
    // be reported. The shortest and longest intervals between reports for
    // this attribute will be governed by the minInterval and maxInterval
    // settings in the attribute's report configuration.
    //
    // Alternatively...
    //
    skipAttribute:
    //
    // ...may arrive here via goto label if attribute processing detected
    // a condition that prevents the attribute value from being reported.
    // In that case, this state still must be updated; otherwise the tick
    // scheduler, executed at the end of this handler, will see the attribute
    // as still being ready to be reported, and will schedule the handler
    // to execute again IMMEDIATELY. If the problematic attribute condition
    // persists, the handler will effectively try to execute continuously.
    emAfPluginReportVolatileData[i].reportableChange = false;
    emAfPluginReportVolatileData[i].lastReportTimeMs = halCommonGetInt32uMillisecondTick();
  }

  if (apsFrame != NULL) {
    conditionallySendReport(apsFrame->sourceEndpoint, apsFrame->clusterId);
  }
  scheduleTick();
}

static void conditionallySendReport(uint8_t endpoint, EmberAfClusterId clusterId)
{
  EmberStatus status;
  if ((emberAfIsDeviceEnabled(endpoint)
       || clusterId == ZCL_IDENTIFY_CLUSTER_ID)
      // No transmissions from the s2s shades device is desired. The S2S
      // Shades sets up bindings at startup. The device then tries to send
      // messages over those bindings - something we don't want. The below code
      // stops all reports if the device is a s2s device
      && ((emberAfNetworkState() != EMBER_JOINED_NETWORK_S2S_INITIATOR)
          && (emberAfNetworkState() != EMBER_JOINED_NETWORK_S2S_TARGET))) {
    status = emberAfSendCommandUnicastToBindingsWithCallback((EmberAfMessageSentFunction)(&retrySendReport));

    // If the callback table is full, attempt to send the message with no
    // callback.  Note that this could lead to a message failing to transmit
    // with no notification to the user for any number of reasons (ex: hitting
    // the message queue limit), but is better than not sending the message at
    // all because the system hits its callback queue limit.
    if (status == EMBER_TABLE_FULL) {
      emberAfSendCommandUnicastToBindings();
    }

#ifdef ENABLE_GROUP_BOUND_REPORTS
    emberAfSendCommandMulticastToBindings();
#endif // ENABLE_GROUP_BOUND_REPORTS
  }
}

bool emberAfConfigureReportingCommandCallback(const EmberAfClusterCommand *cmd)
{
  EmberStatus sendStatus;
  uint16_t bufIndex = cmd->payloadStartIndex;
  uint8_t frameControl, mask;
  bool failures = false;

  emberAfReportingPrint("%p: ", "CFG_RPT");
  emberAfReportingDebugExec(emberAfDecodeAndPrintClusterWithMfgCode(cmd->apsFrame->clusterId, cmd->mfgCode));
  emberAfReportingPrintln("");
  emberAfReportingFlush();

  if (cmd->direction == ZCL_DIRECTION_CLIENT_TO_SERVER) {
    frameControl = (ZCL_GLOBAL_COMMAND
                    | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT
                    | EMBER_AF_DEFAULT_RESPONSE_POLICY_REQUESTS);
    mask = CLUSTER_MASK_SERVER;
  } else {
    frameControl = (ZCL_GLOBAL_COMMAND
                    | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
                    | EMBER_AF_DEFAULT_RESPONSE_POLICY_REQUESTS);
    mask = CLUSTER_MASK_CLIENT;
  }

  // The manufacturer-specfic version of the fill API only creates a
  // manufacturer-specfic command if the manufacturer code is set.  For non-
  // manufacturer-specfic reports, the manufacturer code is unset, so we can
  // get away with using this API for both cases.
  emberAfFillExternalManufacturerSpecificBuffer(frameControl,
                                                cmd->apsFrame->clusterId,
                                                cmd->mfgCode,
                                                ZCL_CONFIGURE_REPORTING_RESPONSE_COMMAND_ID,
                                                "");

  // Each record in the command has at least a one-byte direction and a two-
  // byte attribute id.  Additional fields are present depending on the value
  // of the direction field.
  while (bufIndex + 3 < cmd->bufLen) {
    EmberAfAttributeId attributeId;
    EmberAfReportingDirection direction;
    EmberAfStatus status;

    direction = (EmberAfReportingDirection)emberAfGetInt8u(cmd->buffer,
                                                           bufIndex,
                                                           cmd->bufLen);
    bufIndex++;
    attributeId = (EmberAfAttributeId)emberAfGetInt16u(cmd->buffer,
                                                       bufIndex,
                                                       cmd->bufLen);
    bufIndex += 2;

    emberAfReportingPrintln(" - direction:%x, attr:%2x", direction, attributeId);

    switch (direction) {
      case EMBER_ZCL_REPORTING_DIRECTION_REPORTED:
      {
        EmberAfAttributeMetadata* metadata;
        EmberAfAttributeType dataType;
        uint16_t minInterval, maxInterval;
        uint32_t reportableChange = 0;
        EmberAfPluginReportingEntry newEntry;

        dataType = (EmberAfAttributeType)emberAfGetInt8u(cmd->buffer,
                                                         bufIndex,
                                                         cmd->bufLen);
        bufIndex++;
        minInterval = emberAfGetInt16u(cmd->buffer, bufIndex, cmd->bufLen);
        bufIndex += 2;
        maxInterval = emberAfGetInt16u(cmd->buffer, bufIndex, cmd->bufLen);
        bufIndex += 2;

        emberAfReportingPrintln("   type:%x, min:%2x, max:%2x",
                                dataType,
                                minInterval,
                                maxInterval);
        emberAfReportingFlush();

        if (emberAfGetAttributeAnalogOrDiscreteType(dataType)
            == EMBER_AF_DATA_TYPE_ANALOG) {
          uint8_t dataSize = emberAfGetDataSize(dataType);
          reportableChange = emberAfGetInt(cmd->buffer,
                                           bufIndex,
                                           cmd->bufLen,
                                           dataSize);

          emberAfReportingPrint("   change:");
          emberAfReportingPrintBuffer(cmd->buffer + bufIndex, dataSize, false);
          emberAfReportingPrintln("");

          bufIndex += dataSize;
        }

        // emberAfPluginReportingConfigureReportedAttribute handles non-
        // existent attributes, but does not verify the attribute data type, so
        // we need to check it here.
        metadata = emberAfLocateAttributeMetadata(cmd->apsFrame->destinationEndpoint,
                                                  cmd->apsFrame->clusterId,
                                                  attributeId,
                                                  mask,
                                                  cmd->mfgCode);
        if (metadata != NULL && metadata->attributeType != dataType) {
          status = EMBER_ZCL_STATUS_INVALID_DATA_TYPE;
        } else if (ZCL_ARRAY_ATTRIBUTE_TYPE == dataType
                   || ZCL_STRUCT_ATTRIBUTE_TYPE == dataType
                   || ZCL_SET_ATTRIBUTE_TYPE == dataType
                   || ZCL_BAG_ATTRIBUTE_TYPE == dataType) {
          // The attribute type is array, structure, set or bag
          status = EMBER_ZCL_STATUS_UNREPORTABLE_ATTRIBUTE;
        } else {
          // Add a reporting entry for a reported attribute.  The reports will
          // be sent from us to the source of the Configure Reporting command.
          newEntry.endpoint = cmd->apsFrame->destinationEndpoint;
          newEntry.clusterId = cmd->apsFrame->clusterId;
          newEntry.attributeId = attributeId;
          newEntry.mask = mask;
          newEntry.manufacturerCode = cmd->mfgCode;
          newEntry.data.reported.minInterval = minInterval;
          newEntry.data.reported.maxInterval = maxInterval;
          newEntry.data.reported.reportableChange = reportableChange;
          status = emberAfPluginReportingConfigureReportedAttribute(&newEntry);
        }
        break;
      }
      case EMBER_ZCL_REPORTING_DIRECTION_RECEIVED:
      {
        uint16_t timeout = emberAfGetInt16u(cmd->buffer, bufIndex, cmd->bufLen);
        bufIndex += 2;

        emberAfReportingPrintln("   timeout:%2x", timeout);

        // Add a reporting entry from a received attribute.  The reports
        // will be sent to us from the source of the Configure Reporting
        // command.
        status = configureReceivedAttribute(cmd,
                                            attributeId,
                                            mask,
                                            timeout);
        break;
      }
      default:
        // This will abort the processing (see below).
        status = EMBER_ZCL_STATUS_INVALID_FIELD;
        break;
    }

    // If a report cannot be configured, the status, direction, and
    // attribute are added to the response.  If the failure was due to an
    // invalid field, we have to abort after this record because we don't
    // know how to interpret the rest of the data in the request.
    if (status != EMBER_ZCL_STATUS_SUCCESS) {
      (void) emberAfPutInt8uInResp(status);
      (void) emberAfPutInt8uInResp(direction);
      (void) emberAfPutInt16uInResp(attributeId);
      failures = true;
      if (status == EMBER_ZCL_STATUS_INVALID_FIELD) {
        break;
      }
    }
  }

  // We just respond with SUCCESS if we made it through without failures.
  if (!failures) {
    (void) emberAfPutInt8uInResp(EMBER_ZCL_STATUS_SUCCESS);
  }

  sendStatus = emberAfSendResponse();
  if (EMBER_SUCCESS != sendStatus) {
    emberAfReportingPrintln("Reporting: failed to send %s response: 0x%x",
                            "configure_reporting",
                            sendStatus);
  }
  return true;
}

bool emberAfReadReportingConfigurationCommandCallback(const EmberAfClusterCommand *cmd)
{
  EmberStatus sendStatus;
  uint16_t bufIndex = cmd->payloadStartIndex;
  uint8_t frameControl, mask;

  emberAfReportingPrint("%p: ", "READ_RPT_CFG");
  emberAfReportingDebugExec(emberAfDecodeAndPrintClusterWithMfgCode(cmd->apsFrame->clusterId, cmd->mfgCode));
  emberAfReportingPrintln("");
  emberAfReportingFlush();

  if (cmd->direction == ZCL_DIRECTION_CLIENT_TO_SERVER) {
    frameControl = (ZCL_GLOBAL_COMMAND
                    | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT
                    | EMBER_AF_DEFAULT_RESPONSE_POLICY_REQUESTS);
    mask = CLUSTER_MASK_SERVER;
  } else {
    frameControl = (ZCL_GLOBAL_COMMAND
                    | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
                    | EMBER_AF_DEFAULT_RESPONSE_POLICY_REQUESTS);
    mask = CLUSTER_MASK_CLIENT;
  }

  // The manufacturer-specfic version of the fill API only creates a
  // manufacturer-specfic command if the manufacturer code is set.  For non-
  // manufacturer-specfic reports, the manufacturer code is unset, so we can
  // get away with using this API for both cases.
  emberAfFillExternalManufacturerSpecificBuffer(frameControl,
                                                cmd->apsFrame->clusterId,
                                                cmd->mfgCode,
                                                ZCL_READ_REPORTING_CONFIGURATION_RESPONSE_COMMAND_ID,
                                                "");

  // Each record in the command has a one-byte direction and a two-byte
  // attribute id.
  while (bufIndex + 3 <= cmd->bufLen) {
    EmberAfAttributeId attributeId;
    EmberAfAttributeMetadata *metadata = NULL;
    EmberAfPluginReportingEntry entry;
    EmberAfReportingDirection direction;
    uint8_t i;
    bool found = false;

    direction = (EmberAfReportingDirection)emberAfGetInt8u(cmd->buffer,
                                                           bufIndex,
                                                           cmd->bufLen);
    bufIndex++;
    attributeId = (EmberAfAttributeId)emberAfGetInt16u(cmd->buffer,
                                                       bufIndex,
                                                       cmd->bufLen);
    bufIndex += 2;

    switch (direction) {
      case EMBER_ZCL_REPORTING_DIRECTION_REPORTED:
      case EMBER_ZCL_REPORTING_DIRECTION_RECEIVED:
        metadata = emberAfLocateAttributeMetadata(cmd->apsFrame->destinationEndpoint,
                                                  cmd->apsFrame->clusterId,
                                                  attributeId,
                                                  mask,
                                                  cmd->mfgCode);
        if (metadata == NULL) {
          (void) emberAfPutInt8uInResp(EMBER_ZCL_STATUS_UNSUPPORTED_ATTRIBUTE);
          (void) emberAfPutInt8uInResp(direction);
          (void) emberAfPutInt16uInResp(attributeId);
          continue;
        }
        break;
      default:
        (void) emberAfPutInt8uInResp(EMBER_ZCL_STATUS_INVALID_FIELD);
        (void) emberAfPutInt8uInResp(direction);
        (void) emberAfPutInt16uInResp(attributeId);
        continue;
    }

    // 075123r03 seems to suggest that SUCCESS is returned even if reporting
    // isn't configured for the requested attribute.  The individual fields
    // of the response for this attribute get populated with defaults.
    // CCB 1854 removes the ambiguity and requires NOT_FOUND to be returned in
    // the status field and all fields except direction and attribute identifier
    // to be omitted if there is no report configuration found.
    for (i = 0; i < reportTableActiveLength; i++) {
      emAfPluginReportingGetEntry(i, &entry);
      if (entry.direction == direction
          && entry.endpoint == cmd->apsFrame->destinationEndpoint
          && entry.clusterId == cmd->apsFrame->clusterId
          && entry.attributeId == attributeId
          && entry.mask == mask
          && entry.manufacturerCode == cmd->mfgCode
          && (entry.direction == EMBER_ZCL_REPORTING_DIRECTION_REPORTED
              || (entry.data.received.source == cmd->source
                  && entry.data.received.endpoint == cmd->apsFrame->sourceEndpoint))) {
        found = true;
        break;
      }
    }
    // Attribute supported, reportable, no report configuration was found.
    if (found == false) {
      (void) emberAfPutInt8uInResp(EMBER_ZCL_STATUS_NOT_FOUND);
      (void) emberAfPutInt8uInResp(direction);
      (void) emberAfPutInt16uInResp(attributeId);
      continue;
    }
    // Attribute supported, reportable, report configuration was found.
    (void) emberAfPutInt8uInResp(EMBER_ZCL_STATUS_SUCCESS);
    (void) emberAfPutInt8uInResp(direction);
    (void) emberAfPutInt16uInResp(attributeId);
    switch (direction) {
      case EMBER_ZCL_REPORTING_DIRECTION_REPORTED:
        if (metadata != NULL) {
          (void) emberAfPutInt8uInResp(metadata->attributeType);
          (void) emberAfPutInt16uInResp(entry.data.reported.minInterval);
          (void) emberAfPutInt16uInResp(entry.data.reported.maxInterval);
          if (emberAfGetAttributeAnalogOrDiscreteType(metadata->attributeType)
              == EMBER_AF_DATA_TYPE_ANALOG) {
            putReportableChangeInResp(&entry, metadata->attributeType);
          }
        }
        break;
      case EMBER_ZCL_REPORTING_DIRECTION_RECEIVED:
        (void) emberAfPutInt16uInResp(entry.data.received.timeout);
        break;
    }
  }

  sendStatus = emberAfSendResponse();
  if (EMBER_SUCCESS != sendStatus) {
    emberAfReportingPrintln("Reporting: failed to send %s response: 0x%x",
                            "read_reporting_configuration",
                            sendStatus);
  }
  return true;
}

EmberStatus emberAfClearReportTableCallback(void)
{
  // this operation is more efficient if we loop over the table backwards
  while (reportTableActiveLength > 0) {
    removeConfiguration(reportTableActiveLength - 1);
  }
  slxu_zigbee_event_set_inactive(tickEvent);
  return EMBER_SUCCESS;
}

EmberStatus emAfPluginReportingRemoveEntry(uint16_t index)
{
  EmberStatus status = EMBER_INDEX_OUT_OF_RANGE;
  if (index < reportTableActiveLength) {
    removeConfigurationAndScheduleTick(index);
    status = EMBER_SUCCESS;
  }
  return status;
}

// This function will check all entries in report table and update
// lastReportValue field with current value of attributes
void emAfPluginReportingGetLastValueAll(void)
{
  uint16_t i;
  for (i = 0; i < reportTableActiveLength; i++) {
    EmberAfPluginReportingEntry entry;
    emAfPluginReportingGetEntry(i, &entry);
    if (emberAfEndpointIsEnabled(entry.endpoint)
        && entry.direction == EMBER_ZCL_REPORTING_DIRECTION_REPORTED) {
      readAttributeAndGetLastValue(&entry, i, NULL, NULL, NULL, 0, true);
    }
  }
  scheduleTick();
}

static void markReportTableChange(uint8_t *dataRef,
                                  uint8_t dataSize,
                                  EmberAfAttributeType dataType,
                                  const EmberAfPluginReportingEntry* const entry,
                                  uint8_t entryIndex)
{
  // If we are reporting this particular attribute, we only care whether
  // the new value meets the reportable change criteria.  If it does, we
  // mark the entry as ready to report and reschedule the tick.  Whether
  // the tick will be scheduled for immediate or delayed execution depends
  // on the minimum reporting interval.  This is handled in the scheduler.
  EmberAfDifferenceType difference
    = emberAfGetDifference(dataRef,
                           emAfPluginReportVolatileData[entryIndex].lastReportValue,
                           dataSize);
  uint8_t analogOrDiscrete = emberAfGetAttributeAnalogOrDiscreteType(dataType);
  if ((analogOrDiscrete == EMBER_AF_DATA_TYPE_DISCRETE && difference != 0)
      || (analogOrDiscrete == EMBER_AF_DATA_TYPE_ANALOG
          && entry->data.reported.reportableChange <= difference)) {
    emAfPluginReportVolatileData[entryIndex].reportableChange = true;
    scheduleTick();
  }
}

// This function will check specified entry in report table and update
// lastReportValue field with current value of attribute
static EmberStatus readAttributeAndGetLastValue(const EmberAfPluginReportingEntry* const entry,
                                                uint8_t entryIndex,
                                                EmberAfAttributeType* pDataType,
                                                uint16_t* pDataSize,
                                                uint8_t* pReadData,
                                                uint16_t readDataSize,
                                                bool reportChange)
{
  uint8_t readData[READ_DATA_SIZE];
  uint16_t dataSize;
  EmberAfAttributeType dataType;

  EmberStatus status = emAfReadAttribute(entry->endpoint,
                                         entry->clusterId,
                                         entry->attributeId,
                                         entry->mask,
                                         entry->manufacturerCode,
                                         readData,
                                         READ_DATA_SIZE,
                                         &dataType);

  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfReportingPrintln("ERR: reading cluster 0x%2x attribute 0x%2x: 0x%x",
                            entry->clusterId,
                            entry->attributeId,
                            status);
    return status;
  }

  if (emberAfIsLongStringAttributeType(dataType)) {
    // LONG string types are rarely used and even more rarely (never?)
    // reported; ignore and leave ensuing handling of other types unchanged.
    emberAfReportingPrintln("ERR: reporting of LONG string attribute type not supported: cluster 0x%2x attribute 0x%2x",
                            entry->clusterId,
                            entry->attributeId);
    return EMBER_ZCL_STATUS_INVALID_DATA_TYPE;
  }

  // find size of current report
  dataSize = emberAfAttributeValueSize(dataType, readData, READ_DATA_SIZE);
  if (dataSize == 0  || (pReadData != NULL && dataSize > readDataSize)) {
    return EMBER_ZCL_STATUS_FAILURE;   // defensive; read attribute above should have failed
  }

  // Store the current attribute value for comparison with future values
  // to detect reportable changes. Use the actual attribute value for
  // data types that are small enough to efficiently store; for string
  // types, substitute a 32-bit hash of the string value.
  uint32_t stringHash = 0;
  uint8_t *copyData = readData;
  uint8_t copySize = dataSize;
  if (dataType == ZCL_OCTET_STRING_ATTRIBUTE_TYPE || dataType == ZCL_CHAR_STRING_ATTRIBUTE_TYPE) {
    // dataSize was set above to count the string's length byte, in addition to string length.
    // Compute hash on string value only.
    stringHash = computeStringHash(readData + 1, dataSize - 1);
    copyData = (uint8_t *)&stringHash;
    copySize = sizeof(stringHash);
  }

  // If need to report attribute change, calculate the difference and schedule to send reportAttributes
  if (reportChange) {
    markReportTableChange(readData, copySize, dataType, entry, entryIndex);
    // The lastReportValue will be updated later in emberAfPluginReportingTickEventHandler
    return EMBER_ZCL_STATUS_SUCCESS;
  }

  if (copySize <= sizeof(emAfPluginReportVolatileData[entryIndex].lastReportValue)) {
    emAfPluginReportVolatileData[entryIndex].lastReportValue = 0;
#if (BIGENDIAN_CPU)
    MEMMOVE(((uint8_t *)&emAfPluginReportVolatileData[entryIndex].lastReportValue
             + sizeof(emAfPluginReportVolatileData[entryIndex].lastReportValue)
             - copySize),
            copyData,
            copySize);
#else
    MEMMOVE(&emAfPluginReportVolatileData[entryIndex].lastReportValue, copyData, copySize);
#endif
  }

  if (pDataType != NULL) {
    *pDataType = dataType;
  }
  if (pDataSize != NULL) {
    *pDataSize = dataSize;
  }
  if (pReadData != NULL) {
    memcpy(pReadData, readData, dataSize);
  }

  return EMBER_ZCL_STATUS_SUCCESS;
}

void emberAfReportingAttributeChangeCallback(uint8_t endpoint,
                                             EmberAfClusterId clusterId,
                                             EmberAfAttributeId attributeId,
                                             uint8_t mask,
                                             uint16_t manufacturerCode,
                                             EmberAfAttributeType type,
                                             uint8_t *data)
{
  uint16_t i;
  for (i = 0; i < reportTableActiveLength; i++) {
    EmberAfPluginReportingEntry entry;
    emAfPluginReportingGetEntry(i, &entry);
    if (entry.direction == EMBER_ZCL_REPORTING_DIRECTION_REPORTED
        && entry.endpoint == endpoint
        && entry.clusterId == clusterId
        && entry.attributeId == attributeId
        && entry.mask == mask
        && entry.manufacturerCode == manufacturerCode) {
      // For CHAR and OCTET strings, the string value may be too long to fit into the
      // lastReportValue field (EmberAfDifferenceType), so instead we save the string's
      // hash, and detect changes in string value based on unequal hash.
      uint32_t stringHash = 0;
      uint8_t dataSize  = emberAfGetDataSize(type);
      uint8_t *dataRef = data;
      if (type == ZCL_OCTET_STRING_ATTRIBUTE_TYPE || type == ZCL_CHAR_STRING_ATTRIBUTE_TYPE) {
        stringHash = computeStringHash(data + 1, emberAfStringLength(data));
        dataRef = (uint8_t *)&stringHash;
        dataSize = sizeof(stringHash);
      }
      markReportTableChange(dataRef, dataSize, type, &entry, i);
      break;
    }
  }
}

bool emAfPluginReportingDoEntriesMatch(const EmberAfPluginReportingEntry* const entry1,
                                       const EmberAfPluginReportingEntry* const entry2)
{
  // Verify that the reporting parameters of both entries match.
  // If the entries are for EMBER_ZCL_REPORTING_DIRECTION_REPORTED, the
  // reporting configurations do not need to match.  If the direction is
  // EMBER_ZCL_REPORTING_DIRECTION_RECEIVED, then the source and destination
  // endpoints need to match.
  if ((entry1->endpoint == entry2->endpoint)
      && (entry1->clusterId == entry2->clusterId)
      && (entry1->attributeId == entry2->attributeId)
      && (entry1->mask == entry2->mask)
      && (entry1->manufacturerCode == entry2->manufacturerCode)
      && (entry1->direction == entry2->direction)
      && ((entry1->direction == EMBER_ZCL_REPORTING_DIRECTION_REPORTED)
          || ((entry1->data.received.source == entry2->data.received.source)
              && (entry1->data.received.endpoint
                  == entry2->data.received.endpoint)))) {
    return true;
  }
  return false;
}

uint16_t emAfPluginReportingAddEntry(EmberAfPluginReportingEntry* newEntry)
{
  uint16_t i;
  EmberAfPluginReportingEntry oldEntry;

  // If an entry already exists, or exists but with different parameters,
  // overwrite it with the new entry to prevent pollution of the report table
  for (i = 0; i < reportTableActiveLength; i++) {
    emAfPluginReportingGetEntry(i, &oldEntry);
    if (emAfPluginReportingDoEntriesMatch(&oldEntry, newEntry)) {
      emAfPluginReportingSetEntry(i, newEntry);
      return i;
    }
  }

  // if no entry is found, add it to an unused index
  return emAfPluginReportingAppendEntry(newEntry);
}

uint16_t emAfPluginReportingAppendEntry(EmberAfPluginReportingEntry* newEntry)
{
  uint16_t index = NULL_INDEX;
  if (reportTableActiveLength < REPORT_TABLE_SIZE) {
    index = reportTableActiveLength;
    emAfPluginReportingSetEntry(index, newEntry);
    reportTableActiveLength++;
  }
  return index;
}

static void scheduleTick(void)
{
  uint32_t delayMs = MAX_INT32U_VALUE;
  uint16_t i;
  for (i = 0; i < reportTableActiveLength; i++) {
    EmberAfPluginReportingEntry entry;
    emAfPluginReportingGetEntry(i, &entry);
    if (emberAfEndpointIsEnabled(entry.endpoint)
        && entry.direction == EMBER_ZCL_REPORTING_DIRECTION_REPORTED) {
      uint32_t minIntervalMs = (entry.data.reported.minInterval
                                * MILLISECOND_TICKS_PER_SECOND);
      uint32_t maxIntervalMs = (entry.data.reported.maxInterval
                                * MILLISECOND_TICKS_PER_SECOND);
      uint32_t elapsedMs = elapsedTimeInt32u(emAfPluginReportVolatileData[i].lastReportTimeMs,
                                             halCommonGetInt32uMillisecondTick());
      uint32_t remainingMs = MAX_INT32U_VALUE;
      if (emAfPluginReportVolatileData[i].reportableChange) {
        remainingMs = (minIntervalMs < elapsedMs
                       ? 0
                       : minIntervalMs - elapsedMs);
      } else if (maxIntervalMs) {
        remainingMs = (maxIntervalMs < elapsedMs
                       ? 0
                       : maxIntervalMs - elapsedMs);
      }
      if (remainingMs < delayMs) {
        delayMs = remainingMs;
      }
    }
  }
  if (delayMs != MAX_INT32U_VALUE) {
    emberAfDebugPrintln("sched report event for: 0x%4x", delayMs);
    slxu_zigbee_event_set_delay_ms(tickEvent, delayMs);
  } else {
    emberAfDebugPrintln("deactivate report event");
    slxu_zigbee_event_set_inactive(tickEvent);
  }
}

static void removeConfiguration(uint16_t index)
{
  if (index < reportTableActiveLength) {
    EmberAfPluginReportingEntry entry;
    emAfPluginReportingGetEntry(index, &entry);
    entry.endpoint = EMBER_AF_PLUGIN_REPORTING_UNUSED_ENDPOINT_ID;
    reportTableActiveLength--;
    if (reportTableActiveLength == index) {
      // we are removing the last entry from the list
      emAfPluginReportingSetEntry(index, &entry);
    } else {
      // we are removing an entry from the middle of the list
      // must perform a swap in order to maintain list structure
      EmberAfPluginReportingEntry swap;
      EmAfPluginReportVolatileData ramSwap;
      // swap with the last entry
      emAfPluginReportingGetEntry(reportTableActiveLength, &swap);
      ramSwap = emAfPluginReportVolatileData[reportTableActiveLength];
      emAfPluginReportingSetEntry(index, &swap);
      emAfPluginReportVolatileData[index] = ramSwap;
      emAfPluginReportingSetEntry(reportTableActiveLength, &entry);
      // TODO add a callback that fires when indices change to inform anyone who might be watching a specific index
    }

    emberAfPluginReportingConfiguredCallback(&entry);
  }
}

static void removeConfigurationAndScheduleTick(uint16_t index)
{
  removeConfiguration(index);
  scheduleTick();
}

EmberAfStatus emberAfPluginReportingConfigureReportedAttribute(const EmberAfPluginReportingEntry* newEntry)
{
  EmberAfAttributeMetadata *metadata;
  EmberAfPluginReportingEntry entry;
  EmberAfStatus status;
  uint16_t i, index = reportTableActiveLength;
  bool initialize = true;

  // Verify that we support the attribute and that the data type matches.
  metadata = emberAfLocateAttributeMetadata(newEntry->endpoint,
                                            newEntry->clusterId,
                                            newEntry->attributeId,
                                            newEntry->mask,
                                            newEntry->manufacturerCode);
  if (metadata == NULL) {
    return EMBER_ZCL_STATUS_UNSUPPORTED_ATTRIBUTE;
  }

  // Verify the minimum and maximum intervals make sense.
  if (newEntry->data.reported.maxInterval != 0
      && (newEntry->data.reported.maxInterval
          < newEntry->data.reported.minInterval)) {
    return EMBER_ZCL_STATUS_INVALID_VALUE;
  }

  // Check the table for an entry that matches this request and also watch for
  // empty slots along the way.  If a report exists, it will be overwritten
  // with the new configuration.  Otherwise, a new entry will be created and
  // initialized.
  for (i = 0; i < reportTableActiveLength; i++) {
    emAfPluginReportingGetEntry(i, &entry);
    if (entry.direction == EMBER_ZCL_REPORTING_DIRECTION_REPORTED
        && entry.endpoint == newEntry->endpoint
        && entry.clusterId == newEntry->clusterId
        && entry.attributeId == newEntry->attributeId
        && entry.mask == newEntry->mask
        && entry.manufacturerCode == newEntry->manufacturerCode) {
      initialize = false;
      index = i;
      break;
    }
  }

  // If the maximum reporting interval is 0xFFFF, the device shall not issue
  // reports for the attribute and the configuration information for that
  // attribute need not be maintained.
  if (newEntry->data.reported.maxInterval == 0xFFFF) {
    if (!initialize) {
      removeConfigurationAndScheduleTick(index);
    }
    return EMBER_ZCL_STATUS_SUCCESS;
  }

  // ZCL v6 Section 2.5.7.1.6 Maximum Reporting Interval Field
  // If this value is set to 0x0000 and the minimum reporting interval field
  // equals 0xffff, then the device SHALL revert back to its default reporting
  // configuration. The reportable change field, if present, SHALL be set to
  // zero.
  // Verify special condition to reset the reporting configuration to defaults
  // if the minimum == 0xFFFF and maximum == 0x0000
  bool reset = false;
  if ((newEntry->data.reported.maxInterval == 0x0000)
      && (newEntry->data.reported.minInterval == 0xFFFF)) {
    // Get the configuration from the default configuration table for this
    MEMMOVE(&entry, newEntry, sizeof(EmberAfPluginReportingEntry));
    if (emberAfPluginReportingGetReportingConfigDefaults(&entry)) {
      // Then it must be initialise with the default config - explicity
      initialize = true;
      reset = true;
    }
  }

  if (initialize) {
    if (index >= REPORT_TABLE_SIZE) {
      return EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
    }
    entry.direction = EMBER_ZCL_REPORTING_DIRECTION_REPORTED;
    entry.endpoint = newEntry->endpoint;
    entry.clusterId = newEntry->clusterId;
    entry.attributeId = newEntry->attributeId;
    entry.mask = newEntry->mask;
    entry.manufacturerCode = newEntry->manufacturerCode;
  }

  // For new or updated entries, set the intervals and reportable change.
  // Updated entries will retain all other settings configured previously.
  if (!reset) {
    entry.data.reported.minInterval = newEntry->data.reported.minInterval;
    entry.data.reported.maxInterval = newEntry->data.reported.maxInterval;
    entry.data.reported.reportableChange = newEntry->data.reported.reportableChange;
  }
  // Give the application a chance to review the configuration that we have
  // been building up.  If the application rejects it, we just do not save the
  // record.  If we were supposed to add a new configuration, it will not be
  // created.  If we were supposed to update an existing configuration, we will
  // keep the old one and just discard any changes.  So, in either case, life
  // continues unchanged if the application rejects the configuration.
  status = emberAfPluginReportingConfiguredCallback(&entry);
  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    if (index == reportTableActiveLength) {
      reportTableActiveLength++;
    }
    // Always update the lastReportTimeMs and lastReportValue when the entry is updated or newly added
    emAfPluginReportVolatileData[index].lastReportTimeMs = halCommonGetInt32uMillisecondTick();
    if (readAttributeAndGetLastValue(&entry, index, NULL, NULL, NULL, 0, false) != EMBER_ZCL_STATUS_SUCCESS) {
      emAfPluginReportVolatileData[index].lastReportValue = 0;
    }
    emAfPluginReportingSetEntry(index, &entry);
    scheduleTick();
  }
  return status;
}

static EmberAfStatus configureReceivedAttribute(const EmberAfClusterCommand *cmd,
                                                EmberAfAttributeId attributeId,
                                                uint8_t mask,
                                                uint16_t timeout)
{
  EmberAfPluginReportingEntry entry;
  EmberAfStatus status;
  uint16_t i, index = reportTableActiveLength;
  bool initialize = true;

  // Check the table for an entry that matches this request and also watch for
  // empty slots along the way.  If a report exists, it will be overwritten
  // with the new configuration.  Otherwise, a new entry will be created and
  // initialized.
  for (i = 0; i < reportTableActiveLength; i++) {
    emAfPluginReportingGetEntry(i, &entry);
    if (entry.direction == EMBER_ZCL_REPORTING_DIRECTION_RECEIVED
        && entry.endpoint == cmd->apsFrame->destinationEndpoint
        && entry.clusterId == cmd->apsFrame->clusterId
        && entry.attributeId == attributeId
        && entry.mask == mask
        && entry.manufacturerCode == cmd->mfgCode
        && entry.data.received.source == cmd->source
        && entry.data.received.endpoint == cmd->apsFrame->sourceEndpoint) {
      initialize = false;
      index = i;
      break;
    }
  }

  if (initialize) {
    if (index >= REPORT_TABLE_SIZE) {
      return EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
    }
    entry.direction = EMBER_ZCL_REPORTING_DIRECTION_RECEIVED;
    entry.endpoint = cmd->apsFrame->destinationEndpoint;
    entry.clusterId = cmd->apsFrame->clusterId;
    entry.attributeId = attributeId;
    entry.mask = mask;
    entry.manufacturerCode = cmd->mfgCode;
    entry.data.received.source = cmd->source;
    entry.data.received.endpoint = cmd->apsFrame->sourceEndpoint;
  }

  // For new or updated entries, set the timeout.  Updated entries will retain
  // all other settings configured previously.
  entry.data.received.timeout = timeout;

  // Give the application a chance to review the configuration that we have
  // been building up.  If the application rejects it, we just do not save the
  // record.  If we were supposed to add a new configuration, it will not be
  // created.  If we were supposed to update an existing configuration, we will
  // keep the old one and just discard any changes.  So, in either case, life
  // continues unchanged if the application rejects the configuration.  If the
  // application accepts the change, the tick does not have to be rescheduled
  // here because we don't do anything with received reports.
  status = emberAfPluginReportingConfiguredCallback(&entry);
  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    if (index == reportTableActiveLength) {
      reportTableActiveLength++;
    }

    emAfPluginReportingSetEntry(index, &entry);
  }
  return status;
}

static void putReportableChangeInResp(const EmberAfPluginReportingEntry *entry,
                                      EmberAfAttributeType dataType)
{
  uint8_t bytes = emberAfGetDataSize(dataType);
  if (entry == NULL) { // default, 0xFF...UL or 0x80...L
    for (; bytes > 0; bytes--) {
      uint8_t b = 0xFF;
      if (emberAfIsTypeSigned(dataType)) {
        b = (bytes == 1 ? 0x80 : 0x00);
      }
      (void) emberAfPutInt8uInResp(b);
    }
  } else { // reportable change value
    uint32_t value = entry->data.reported.reportableChange;
    for (; bytes > 0; bytes--) {
      uint8_t b = BYTE_0(value);
      (void) emberAfPutInt8uInResp(b);
      value >>= 8;
    }
  }
}

// Conditionally add reporting entry.
static bool reportEntryDoesNotExist(const EmberAfPluginReportingEntry* newEntry)
{
  uint16_t i;
  EmberAfPluginReportingEntry entry;

  for (i = 0; i < reportTableActiveLength; i++) {
    emAfPluginReportingGetEntry(i, &entry);
    if (emAfPluginReportingDoEntriesMatch(&entry, newEntry)) {
      return false;
    }
  }

  return true;
}

uint16_t emAfPluginReportingConditionallyAddReportingEntry(EmberAfPluginReportingEntry* newEntry)
{
  if (reportEntryDoesNotExist(newEntry)) {
    return emAfPluginReportingAppendEntry(newEntry);
  }
  return 0;
}

//------------------------------------------------------------------------------
// Weak callback definitions.

WEAK(EmberAfStatus emberAfPluginReportingConfiguredCallback(const EmberAfPluginReportingEntry *entry))
{
  return EMBER_ZCL_STATUS_SUCCESS;
}

WEAK(bool emberAfPluginReportingGetDefaultReportingConfigCallback(EmberAfPluginReportingEntry *entry))
{
  // Change the values as appropriate for the application.
  entry->data.reported.minInterval = 1;
  entry->data.reported.maxInterval = 0xFFFE;
  entry->data.reported.reportableChange = 1;
  entry->direction = EMBER_ZCL_REPORTING_DIRECTION_REPORTED;
  return true;
}
