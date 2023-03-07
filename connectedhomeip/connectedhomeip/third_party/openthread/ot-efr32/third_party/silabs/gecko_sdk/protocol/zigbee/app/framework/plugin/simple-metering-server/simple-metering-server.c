/***************************************************************************//**
 * @file
 * @brief Routines for the Simple Metering Server plugin, which implements the
 *        server side of the Simple Metering cluster.
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
#include "simple-metering-server.h"
#include "simple-metering-test.h"

#ifdef UC_BUILD
#include "zap-cluster-command-parser.h"
#endif

#define MAX_FAST_POLLING_PERIOD 15
#define PROVIDER_ID 0x44556677

#define isValidSamplingDataIndex(index) ((index) < EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_MAX_SAMPLING_SESSIONS)

// Bug: SE1P2-18
// Get Sampled Data Response does not send the maximum number of samples.
// If we want 5 samples(which we interpret as diff's between 2 consecutive sampling data points)
// we need 6 data points.
#define REAL_MAX_SAMPLES_PER_SESSION (EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_MAX_SAMPLES_PER_SESSION + 1)

// Bug:SE1P2-19
uint32_t minIssuerEventId = 0x00000000;

typedef struct {
  uint8_t endpoint;
  uint32_t issuerEventId;
  uint32_t startTime;
  uint16_t sampleId;
  uint8_t  sampleType;
  uint16_t sampleRequestInterval;
  uint16_t maxNumberOfSamples;
  uint8_t  validSamples;
  uint8_t  samples[6][REAL_MAX_SAMPLES_PER_SESSION];
} EmberAfSimpleMeteringClusterSamplingData;

typedef struct SupplyEvent{
  uint8_t srcEndpoint;
  uint8_t destEndpoint;
  uint16_t nodeId;
  uint32_t providerId;
  uint32_t issuerEventId;
  uint32_t implementationDateTime;
  uint8_t proposedSupplyStatus;
} EmberAfSimpleMeteringClusterSupplyEvent;

static uint32_t fastPollEndTimeUtcTable[EMBER_AF_SIMPLE_METERING_CLUSTER_SERVER_ENDPOINT_COUNT];
static EmberAfSimpleMeteringClusterSamplingData samplingData[EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_MAX_SAMPLING_SESSIONS];
static EmberAfSimpleMeteringClusterSupplyEvent changeSupply;

#ifdef UC_BUILD
void emberAfPluginSimpleMeteringServerSamplingEventHandler(SLXU_UC_EVENT);
void emberAfPluginSimpleMeteringServerSupplyEventHandler(SLXU_UC_EVENT);
sl_zigbee_event_t emberAfPluginSimpleMeteringServerSamplingEvent;
sl_zigbee_event_t emberAfPluginSimpleMeteringServerSupplyEvent;

#define samplingEvent (&emberAfPluginSimpleMeteringServerSamplingEvent)
#define supplyEvent (&emberAfPluginSimpleMeteringServerSupplyEvent)
#else // !UC_BUILD
EmberEventControl emberAfPluginSimpleMeteringServerSamplingEventControl;
EmberEventControl emberAfPluginSimpleMeteringServerSupplyEventControl;

#define samplingEvent emberAfPluginSimpleMeteringServerSamplingEventControl
#define supplyEvent emberAfPluginSimpleMeteringServerSupplyEventControl
#endif // UC_BUILD

static uint8_t fastPolling = 0;

void emAfToggleFastPolling(uint8_t enableFastPolling)
{
  fastPolling = enableFastPolling;
}

static void fastPollEndTimeUtcTableInit(void)
{
  uint8_t i;
  for (i = 0; i < EMBER_AF_SIMPLE_METERING_CLUSTER_SERVER_ENDPOINT_COUNT; i++) {
    fastPollEndTimeUtcTable[i] = 0x00000000;
  }
}

static void samplingDataInit(void)
{
  uint8_t i;
  for (i = 0; i < EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_MAX_SAMPLING_SESSIONS; i++) {
    samplingData[i].sampleId = 0xFFFF;
    samplingData[i].validSamples = 0x00;
  }
}

static uint8_t findSamplingSession(uint16_t sampleId)
{
  uint8_t i;
  for (i = 0; i < EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_MAX_SAMPLING_SESSIONS; i++) {
    if (samplingData[i].sampleId == sampleId) {
      return i;
    }
  }
  return 0xFF;
}

static uint8_t findSamplingSessionByEventId(uint32_t issuerEventId)
{
  uint8_t i;
  for (i = 0; i < EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_MAX_SAMPLING_SESSIONS; i++) {
    if (samplingData[i].issuerEventId == issuerEventId) {
      return i;
    }
  }
  return 0xFF;
}

void emberAfSimpleMeteringClusterServerInitCallback(uint8_t endpoint)
{
  slxu_zigbee_event_init(samplingEvent,
                         emberAfPluginSimpleMeteringServerSamplingEventHandler);
  slxu_zigbee_event_init(supplyEvent,
                         emberAfPluginSimpleMeteringServerSupplyEventHandler);
  emAfTestMeterInit(endpoint);
  fastPollEndTimeUtcTableInit();
  samplingDataInit();
  slxu_zigbee_zcl_schedule_server_tick(endpoint,
                                       ZCL_SIMPLE_METERING_CLUSTER_ID,
                                       MILLISECOND_TICKS_PER_SECOND);
}

void emberAfSimpleMeteringClusterServerTickCallback(uint8_t endpoint)
{
  emAfTestMeterTick(endpoint); //run test module
  slxu_zigbee_zcl_schedule_server_tick(endpoint,
                                       ZCL_SIMPLE_METERING_CLUSTER_ID,
                                       MILLISECOND_TICKS_PER_SECOND);
}

void emberAfSimpleMeteringClusterServerDefaultResponseCallback(uint8_t endpoint,
                                                               uint8_t commandId,
                                                               EmberAfStatus status)
{
  if (commandId == ZCL_REMOVE_MIRROR_COMMAND_ID
      && status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfSimpleMeteringClusterPrintln("Mirror remove FAILED status 0x%x", status);
  }
}

uint16_t emberAfPluginSimpleMeteringServerStartSampling(uint16_t requestedSampleId,
                                                        uint32_t issuerEventId,
                                                        uint32_t startSamplingTime,
                                                        uint8_t sampleType,
                                                        uint16_t sampleRequestInterval,
                                                        uint16_t maxNumberOfSamples,
                                                        uint8_t endpoint)
{
  uint32_t delay;
  uint32_t currentTime;
  static uint16_t nextSampleId = 0x0001;
  uint16_t sampleId;
  uint8_t index;

  emberAfSimpleMeteringClusterPrintln("StartSampling: requestedSampleId %u issuerEventId %u startSamplingTime %u sampleType %u sampleRequestInterval %u maxNumberOfSamples %u endpoint %u", requestedSampleId, issuerEventId, startSamplingTime, sampleType, sampleRequestInterval, maxNumberOfSamples, endpoint);

  // Find an unused sampling session table entry
  index = findSamplingSession(0xFFFF);
  // The following conditional tells CSTAT that index is within array bounds;
  // previously was (index == 0xFF). Simpler to make this change rather than
  // suppress the CSTAT check on multiple code lines.
  if (!isValidSamplingDataIndex(index)) {
    emberAfSimpleMeteringClusterPrintln("ERR: No available entries in sampling session table");
    return 0xFFFF;
  }

  // if the caller requested a specific sampleId then check to make sure it's
  // not already in use.
  if (requestedSampleId != 0xFFFF) {
    if (findSamplingSession(requestedSampleId) != 0xFF) {
      emberAfSimpleMeteringClusterPrintln("ERR: requested sampleId already in use");
      return 0xFFFF;
    } else {
      sampleId = requestedSampleId;
    }
  } else {
    sampleId = nextSampleId;
    while (findSamplingSession(sampleId) != 0xFF) {
      sampleId = (sampleId == 0xFFFE) ? 1 : (sampleId + 1);
    }
    nextSampleId = (sampleId == 0xFFFE) ? 1 : (sampleId + 1);
  }

  samplingData[index].sampleId = sampleId;
  samplingData[index].issuerEventId = issuerEventId;
  samplingData[index].startTime = startSamplingTime;
  samplingData[index].sampleType = sampleType;
  samplingData[index].sampleRequestInterval = sampleRequestInterval;

  //This is a hardcoded limit in EmberAfSimpleMeteringClusterSamplingData.
  if (maxNumberOfSamples > REAL_MAX_SAMPLES_PER_SESSION) {
    maxNumberOfSamples = REAL_MAX_SAMPLES_PER_SESSION;
  }
  samplingData[index].maxNumberOfSamples = maxNumberOfSamples;
  samplingData[index].endpoint = endpoint;

  currentTime = emberAfGetCurrentTime();
  if (startSamplingTime < currentTime) {
    startSamplingTime = currentTime;
  }
  delay = startSamplingTime - currentTime;
  slxu_zigbee_event_set_delay_ms(samplingEvent,
                                 (delay * 1000));
  return sampleId;
}

void emberAfPluginSimpleMeteringServerSamplingEventHandler(SLXU_UC_EVENT)
{
  uint8_t i;
  EmberAfAttributeType dataType;
  EmberStatus status;
  slxu_zigbee_event_set_inactive(samplingEvent);

  // Now let's adjust the summation
  for (i = 0; i < EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_MAX_SAMPLING_SESSIONS; i++) {
    if (samplingData[i].startTime <= emberAfGetCurrentTime()
        && samplingData[i].validSamples <= samplingData[i].maxNumberOfSamples
        && samplingData[i].sampleId < 0xFFFF) {
      status = emberAfReadAttribute(samplingData[i].endpoint,
                                    ZCL_SIMPLE_METERING_CLUSTER_ID,
                                    ZCL_CURRENT_SUMMATION_DELIVERED_ATTRIBUTE_ID,
                                    CLUSTER_MASK_SERVER,
                                    samplingData[i].samples[samplingData[i].validSamples],
                                    6,
                                    &dataType);
      if (status == EMBER_SUCCESS) {
        emberAfSimpleMeteringClusterPrintln("Sample %u: 0x%x%x%x%x%x%x",
                                            samplingData[i].validSamples,
                                            samplingData[i].samples[samplingData[i].validSamples][0],
                                            samplingData[i].samples[samplingData[i].validSamples][1],
                                            samplingData[i].samples[samplingData[i].validSamples][2],
                                            samplingData[i].samples[samplingData[i].validSamples][3],
                                            samplingData[i].samples[samplingData[i].validSamples][4],
                                            samplingData[i].samples[samplingData[i].validSamples][5]);
        samplingData[i].validSamples++;
      }
      emberAfSimpleMeteringClusterPrintln("Interval %u", samplingData[i].sampleRequestInterval);
      slxu_zigbee_event_set_delay_ms(samplingEvent,
                                     samplingData[i].sampleRequestInterval * 1000);
    }
  }
}

void emberAfPluginSimpleMeteringServerSupplyEventHandler(SLXU_UC_EVENT)
{
  uint32_t changeTime;
  uint32_t currentTime;
  slxu_zigbee_event_set_inactive(supplyEvent);

  emberAfReadServerAttribute(changeSupply.srcEndpoint,
                             ZCL_SIMPLE_METERING_CLUSTER_ID,
                             ZCL_PROPOSED_CHANGE_SUPPLY_IMPLEMENTATION_TIME_ATTRIBUTE_ID,
                             (uint8_t *)&changeTime,
                             sizeof(changeTime));
  if (changeTime == 0xFFFFFFFF) {
    // Nothing pending
    return;
  }

  currentTime = emberAfGetCurrentTime();
  if (changeTime > currentTime) {
    uint32_t delay = changeTime - currentTime;
    slxu_zigbee_event_set_delay_ms(supplyEvent,
                                   delay * 1000);
  } else {
    // Per SE specification: "The ProposedChangeImplementationTime attribute
    // indicates the time at which a proposed change to the supply is to be
    // implemented. If there is no change of supply pending, this attribute
    // will be set to 0xFFFFFFFF.
    uint32_t implementationDateTime = 0xFFFFFFFF;
    (void) emberAfWriteAttribute(changeSupply.srcEndpoint,
                                 ZCL_SIMPLE_METERING_CLUSTER_ID,
                                 ZCL_PROPOSED_CHANGE_SUPPLY_IMPLEMENTATION_TIME_ATTRIBUTE_ID,
                                 CLUSTER_MASK_SERVER,
                                 (uint8_t *)&implementationDateTime,
                                 ZCL_UTC_TIME_ATTRIBUTE_TYPE);

    emberAfFillCommandSimpleMeteringClusterSupplyStatusResponse(changeSupply.providerId,
                                                                changeSupply.issuerEventId,
                                                                changeSupply.implementationDateTime,
                                                                changeSupply.proposedSupplyStatus);
    emberAfSetCommandEndpoints(changeSupply.srcEndpoint, changeSupply.destEndpoint);
    emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, changeSupply.nodeId);
  }
}

void emberAfPluginSimpleMeteringClusterReadAttributesResponseCallback(EmberAfClusterId clusterId,
                                                                      uint8_t *buffer,
                                                                      uint16_t bufLen)
{
  uint16_t bufIndex = 0;

  if (clusterId != ZCL_SIMPLE_METERING_CLUSTER_ID
      || emberAfCurrentCommand()->direction != ZCL_FRAME_CONTROL_CLIENT_TO_SERVER) {
    return;
  }

  // Each record in the response has a two-byte attribute id and a one-byte
  // status.  If the status is SUCCESS, there will also be a one-byte type and
  // variable-length data.
  while (bufIndex + 3 <= bufLen) {
    EmberAfStatus status;
    EmberAfAttributeId attributeId = (EmberAfAttributeId)emberAfGetInt16u(buffer,
                                                                          bufIndex,
                                                                          bufLen);
    bufIndex += 2;
    status = (EmberAfStatus)emberAfGetInt8u(buffer, bufIndex, bufLen);
    bufIndex++;
    if (status == EMBER_ZCL_STATUS_SUCCESS) {
      uint16_t dataSize;
      uint8_t dataType = emberAfGetInt8u(buffer, bufIndex, bufLen);
      bufIndex++;

      dataSize = emberAfAttributeValueSize(dataType,
                                           buffer + bufIndex,
                                           bufLen - bufIndex);

      if (dataSize != 0) {
        // The Notification Attribute Set is in the range of 0x0000 - 0x00FF
        // and are all 32 bit BitMap types. Each application may decide to
        // handle the notification flags differently so we'll callback to the
        // application for each notification flags attribute.
        if (attributeId < 0x0100) {
          uint32_t bitMap = emberAfGetInt32u(buffer, bufIndex, bufLen);
          emberAfSimpleMeteringClusterPrintln("Attribute value 0x%4x", bitMap);
          emberAfPluginSimpleMeteringServerProcessNotificationFlagsCallback(attributeId, bitMap);
        }
        bufIndex += dataSize;
      } else {
        // dataSize exceeds buffer length, terminate loop
        emberAfSimpleMeteringClusterPrintln("ERR: attr:%2x size %d exceeds buffer size", attributeId, dataSize);
        break;
      }
    }
  }
}

//-----------------------
// ZCL commands callbacks

#ifdef UC_BUILD

bool emberAfSimpleMeteringClusterGetProfileCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_simple_metering_cluster_get_profile_command_t cmd_data;

  if (zcl_decode_simple_metering_cluster_get_profile_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  return emAfTestMeterGetProfiles(cmd_data.intervalChannel, cmd_data.endTime, cmd_data.numberOfPeriods);
}

bool emberAfSimpleMeteringClusterRequestFastPollModeCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_simple_metering_cluster_request_fast_poll_mode_command_t cmd_data;
  uint8_t endpoint;
  uint8_t ep;
  uint8_t appliedUpdateRate;
  EmberStatus status;
  uint8_t fastPollingUpdateAttribute;

  if (zcl_decode_simple_metering_cluster_request_fast_poll_mode_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  if (!fastPolling) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_UNSUP_COMMAND);
    return true;
  }

  endpoint = emberAfCurrentEndpoint();
  ep = emberAfFindClusterServerEndpointIndex(endpoint,
                                             ZCL_SIMPLE_METERING_CLUSTER_ID);

  appliedUpdateRate = cmd_data.fastPollUpdatePeriod;

  if (ep == 0xFF) {
    emberAfSimpleMeteringClusterPrintln("Invalid endpoint %x", emberAfCurrentEndpoint());
    return false;
  }

  status = emberAfReadServerAttribute(endpoint,
                                      ZCL_SIMPLE_METERING_CLUSTER_ID,
                                      ZCL_FAST_POLL_UPDATE_PERIOD_ATTRIBUTE_ID,
                                      (uint8_t *)&fastPollingUpdateAttribute,
                                      sizeof(fastPollingUpdateAttribute));

  if (status == EMBER_SUCCESS) {
    if (cmd_data.fastPollUpdatePeriod < fastPollingUpdateAttribute) {
      appliedUpdateRate = fastPollingUpdateAttribute;
      emberAfSimpleMeteringClusterPrintln("Applying fast Poll rate %x ep %u", appliedUpdateRate, ep);
    }
  } else {
    emberAfSimpleMeteringClusterPrintln("Reading fast Poll Attribute failed. ep %u  status %x", ep, status);
    emberAfFillCommandSimpleMeteringClusterRequestFastPollModeResponse(0,
                                                                       0);
    emberAfSendResponse();
    return true;
  }

  if (emberAfGetCurrentTime() > fastPollEndTimeUtcTable[ep]) {
    cmd_data.duration = cmd_data.duration > MAX_FAST_POLLING_PERIOD ? MAX_FAST_POLLING_PERIOD : cmd_data.duration;
    fastPollEndTimeUtcTable[ep] = emberAfGetCurrentTime() + (cmd_data.duration * 60);
    emberAfSimpleMeteringClusterPrintln("Starting fast polling for %u minutes  End Time 0x%4x,current Time 0x%4x", cmd_data.duration, fastPollEndTimeUtcTable[ep], emberAfGetCurrentTime());
  } else {
    emberAfSimpleMeteringClusterPrintln("Fast polling mode currently active. ep %u fastPollEndTimeUtcTable[%u] 0x%4x current Time 0x%4x ", ep, ep, fastPollEndTimeUtcTable[ep], emberAfGetCurrentTime());
  }
  emberAfFillCommandSimpleMeteringClusterRequestFastPollModeResponse(appliedUpdateRate,
                                                                     fastPollEndTimeUtcTable[ep]);
  emberAfSendResponse();
  return true;
}

bool emberAfSimpleMeteringClusterStartSamplingCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_simple_metering_cluster_start_sampling_command_t cmd_data;
  static uint8_t firstIssuerId = 0;

  if (zcl_decode_simple_metering_cluster_start_sampling_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  // Bug: SE1P2-19
  // Issuer Eventid should not be ignored. We accept *anything* for the first time.
  // After that, we only accept values greater than the max id received so far(which
  // should be the last valid issuerId received).
  if (firstIssuerId == 0) {
    minIssuerEventId = cmd_data.issuerEventId;
    firstIssuerId = 1;
  } else if (cmd_data.issuerEventId <= minIssuerEventId && cmd_data.startSamplingTime != 0xFFFFFFFF) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_FAILURE);
    emberAfSimpleMeteringClusterPrintln("Rejecting StartSamplingCallback issuerEventId %u minIssuerEventId %u", cmd_data.issuerEventId, minIssuerEventId);
    return true;
  } else {
    minIssuerEventId = cmd_data.issuerEventId;
  }

  // Bug: SE1P2-17
  // StartSampling Event ID field is ignored by the metering server
  //Special time reserved to cancel a startSampling Request.
  if (cmd_data.startSamplingTime == 0xFFFFFFFF) {
    uint8_t eventIndex = findSamplingSessionByEventId(cmd_data.issuerEventId);
    //Event not found.
    // The following conditional tells CSTAT that index is within array bounds;
    // previously was (index == 0xFF). Simpler to make this change rather than
    // suppress the CSTAT check on multiple code lines.
    if (!isValidSamplingDataIndex(eventIndex)) {
      emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_NOT_FOUND);
      return true;
    } else {
      samplingData[eventIndex].sampleId = 0xFFFF;
      samplingData[eventIndex].validSamples = 0x00;
      emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
      return true;
    }
  }

  uint16_t sampleId = emberAfPluginSimpleMeteringServerStartSampling(0xFFFF,
                                                                     cmd_data.issuerEventId,
                                                                     cmd_data.startSamplingTime,
                                                                     cmd_data.sampleType,
                                                                     cmd_data.sampleRequestInterval,
                                                                     cmd_data.maxNumberOfSamples,
                                                                     emberAfCurrentEndpoint());
  emberAfFillCommandSimpleMeteringClusterStartSamplingResponse(sampleId);
  emberAfSendResponse();
  return true;
}

bool emberAfSimpleMeteringClusterGetSampledDataCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_simple_metering_cluster_get_sampled_data_command_t cmd_data;
  uint8_t i;

  if (zcl_decode_simple_metering_cluster_get_sampled_data_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfSimpleMeteringClusterPrintln("sampleId %u earliestSampleTime %u sampleType %u numberOfSamples %u",
                                      cmd_data.sampleId, cmd_data.earliestSampleTime, cmd_data.sampleType, cmd_data.numberOfSamples);
  for (i = 0; i < EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_MAX_SAMPLING_SESSIONS; i++) {
    uint8_t j;
    uint32_t diff;
    if (samplingData[i].sampleId == cmd_data.sampleId
        && samplingData[i].sampleId < 0xFFFF
        && samplingData[i].startTime <= cmd_data.earliestSampleTime
        && samplingData[i].sampleType == cmd_data.sampleType) {
      //If we have only one sample data point or lesser, we don't have enough information
      //for even one sample.
      if (samplingData[i].validSamples <= 1) {
        goto kickout;
      }

      //Bug SE1P2-19: Use the max field from the client.

      if (cmd_data.numberOfSamples > samplingData[i].validSamples - 1) {
        cmd_data.numberOfSamples = samplingData[i].validSamples - 1;
      }

      emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                                 | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT),
                                ZCL_SIMPLE_METERING_CLUSTER_ID,
                                ZCL_GET_SAMPLED_DATA_RESPONSE_COMMAND_ID,
                                "vwuvv",
                                samplingData[i].sampleId,
                                samplingData[i].startTime,
                                samplingData[i].sampleType,
                                samplingData[i].sampleRequestInterval,
                                cmd_data.numberOfSamples);

      emberAfSimpleMeteringClusterPrintln("numberOfSamples 0x%2x", cmd_data.numberOfSamples);

      for (j = 0; j < cmd_data.numberOfSamples; j++) {
        uint32_t b = (samplingData[i].samples[j + 1][3] << 24) | (samplingData[i].samples[j + 1][2] << 16) | (samplingData[i].samples[j + 1][1] << 8) | (samplingData[i].samples[j + 1][0] << 0);
        uint32_t a = (samplingData[i].samples[j][3] << 24) | (samplingData[i].samples[j][2] << 16) | (samplingData[i].samples[j][1] << 8) | (samplingData[i].samples[j][0] << 0);

        diff = b - a;
        (void) emberAfPutInt24uInResp(diff);
        emberAfSimpleMeteringClusterPrintln("index %u numberOfSamples %u diff %u", j, cmd_data.numberOfSamples, diff);
      }
      emberAfSendResponse();
      return true;
    }
  }

  kickout:
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_NOT_FOUND);
  return true;
}

bool emberAfSimpleMeteringClusterMirrorReportAttributeResponseCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_simple_metering_cluster_mirror_report_attribute_response_command_t cmd_data;

  if (zcl_decode_simple_metering_cluster_mirror_report_attribute_response_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  if (cmd_data.notificationScheme == 0x01) {
    uint32_t functionalFlags = emberAfGetInt32u(cmd_data.notificationFlags, 0, 4);
    emberAfPluginSimpleMeteringServerProcessNotificationFlagsCallback(ZCL_FUNCTIONAL_NOTIFICATION_FLAGS_ATTRIBUTE_ID, functionalFlags);
  } else if (cmd_data.notificationScheme == 0x02) {
    uint32_t functionalFlags = emberAfGetInt32u(cmd_data.notificationFlags, 0, 20);
    uint32_t notificationFlags2 = emberAfGetInt32u(cmd_data.notificationFlags, 4, 20);
    uint32_t notificationFlags3 = emberAfGetInt32u(cmd_data.notificationFlags, 8, 20);
    uint32_t notificationFlags4 = emberAfGetInt32u(cmd_data.notificationFlags, 12, 20);
    uint32_t notificationFlags5 = emberAfGetInt32u(cmd_data.notificationFlags, 16, 20);
    emberAfSimpleMeteringClusterPrintln("functionalFlags 0x%4x notificationFlags2-5 0x%4x 0x%4x 0x%4x 0x%4x",
                                        functionalFlags, notificationFlags2, notificationFlags3,
                                        notificationFlags4, notificationFlags5);
    emberAfPluginSimpleMeteringServerProcessNotificationFlagsCallback(ZCL_FUNCTIONAL_NOTIFICATION_FLAGS_ATTRIBUTE_ID, functionalFlags);
    emberAfPluginSimpleMeteringServerProcessNotificationFlagsCallback(ZCL_NOTIFICATION_FLAGS_2_ATTRIBUTE_ID, notificationFlags2);
    emberAfPluginSimpleMeteringServerProcessNotificationFlagsCallback(ZCL_NOTIFICATION_FLAGS_3_ATTRIBUTE_ID, notificationFlags3);
    emberAfPluginSimpleMeteringServerProcessNotificationFlagsCallback(ZCL_NOTIFICATION_FLAGS_4_ATTRIBUTE_ID, notificationFlags4);
    emberAfPluginSimpleMeteringServerProcessNotificationFlagsCallback(ZCL_NOTIFICATION_FLAGS_5_ATTRIBUTE_ID, notificationFlags5);
  } else {
    return false;
  }
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfSimpleMeteringClusterResetLoadLimitCounterCallback(EmberAfClusterCommand *cmd)
{
  uint8_t counter = 0;

#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
  sl_zcl_simple_metering_cluster_reset_load_limit_counter_command_t cmd_data;
  if (zcl_decode_simple_metering_cluster_reset_load_limit_counter_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfSimpleMeteringClusterPrintln("Reset Load Counter providerId %u issuerEventId %u", cmd_data.providerId, cmd_data.issuerEventId);
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT

  (void) emberAfWriteAttribute(emberAfCurrentEndpoint(),
                               ZCL_SIMPLE_METERING_CLUSTER_ID,
                               ZCL_LOAD_LIMIT_COUNTER_ATTRIBUTE_ID,
                               CLUSTER_MASK_SERVER,
                               &counter,
                               ZCL_INT8U_ATTRIBUTE_TYPE);
  return true;
}

bool emberAfSimpleMeteringClusterChangeSupplyCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_simple_metering_cluster_change_supply_command_t cmd_data;
  uint32_t delay;

  if (zcl_decode_simple_metering_cluster_change_supply_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfSimpleMeteringClusterPrintln("Change Supply Callback providerId %u issuerEventId %u implementationDateTime %u supplyStatus %u",
                                      cmd_data.providerId, cmd_data.issuerEventId, cmd_data.implementationDateTime, cmd_data.supplyControlBits);
  // TODO: fix this hard-coded check
  if (cmd_data.providerId != PROVIDER_ID) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_NOT_AUTHORIZED);
    return true;
  }
  if (cmd_data.implementationDateTime < emberAfGetCurrentTime() && cmd_data.implementationDateTime != 0) {
    emberAfSimpleMeteringClusterPrintln("implementationDateTime %u", cmd_data.implementationDateTime);
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_INVALID_VALUE);
    return true;
  }
  if (cmd_data.implementationDateTime == 0xFFFFFFFF) {
    emberAfSimpleMeteringClusterPrintln("Canceling change supply");
    // Per SE specification: "The ProposedChangeImplementationTime attribute
    // indicates the time at which a proposed change to the supply is to be
    // implemented. If there is no change of supply pending, this attribute
    // will be set to 0xFFFFFFFF.
    (void) emberAfWriteAttribute(emberAfCurrentEndpoint(),
                                 ZCL_SIMPLE_METERING_CLUSTER_ID,
                                 ZCL_PROPOSED_CHANGE_SUPPLY_IMPLEMENTATION_TIME_ATTRIBUTE_ID,
                                 CLUSTER_MASK_SERVER,
                                 (uint8_t *)&cmd_data.implementationDateTime,
                                 ZCL_UTC_TIME_ATTRIBUTE_TYPE);
    slxu_zigbee_event_set_inactive(supplyEvent);
  }
  if (cmd_data.implementationDateTime == 0x00000000 ) {
    if ((cmd_data.supplyControlBits & 0x01) == 0x01 || cmd_data.proposedSupplyStatus == 2) {
      emberAfFillCommandSimpleMeteringClusterSupplyStatusResponse(cmd_data.providerId,
                                                                  cmd_data.issuerEventId,
                                                                  cmd_data.implementationDateTime,
                                                                  cmd_data.proposedSupplyStatus);
      emberAfSendResponse();
    } else {
      emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    }
  } else {
    (void) emberAfWriteAttribute(emberAfCurrentEndpoint(),
                                 ZCL_SIMPLE_METERING_CLUSTER_ID,
                                 ZCL_PROPOSED_CHANGE_SUPPLY_IMPLEMENTATION_TIME_ATTRIBUTE_ID,
                                 CLUSTER_MASK_SERVER,
                                 (uint8_t *)&cmd_data.implementationDateTime,
                                 ZCL_UTC_TIME_ATTRIBUTE_TYPE);

    (void) emberAfWriteAttribute(emberAfCurrentEndpoint(),
                                 ZCL_SIMPLE_METERING_CLUSTER_ID,
                                 ZCL_PROPOSED_CHANGE_SUPPLY_STATUS_ATTRIBUTE_ID,
                                 CLUSTER_MASK_SERVER,
                                 &cmd_data.proposedSupplyStatus,
                                 ZCL_ENUM8_ATTRIBUTE_TYPE);
    delay = cmd_data.implementationDateTime - emberAfGetCurrentTime();
    changeSupply.srcEndpoint = cmd->apsFrame->destinationEndpoint;
    changeSupply.destEndpoint = cmd->apsFrame->sourceEndpoint;
    changeSupply.nodeId = cmd->source;
    changeSupply.providerId = cmd_data.providerId;
    changeSupply.issuerEventId = cmd_data.issuerEventId;
    changeSupply.implementationDateTime = cmd_data.implementationDateTime;
    changeSupply.proposedSupplyStatus = cmd_data.proposedSupplyStatus;
    if ((cmd_data.supplyControlBits & 0x01) == 0x01) {
      slxu_zigbee_event_set_delay_ms(supplyEvent,
                                     delay * 1000);
    }
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  }

  return true;
}

bool emberAfSimpleMeteringClusterLocalChangeSupplyCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_simple_metering_cluster_local_change_supply_command_t cmd_data;

  if (zcl_decode_simple_metering_cluster_local_change_supply_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  if (cmd_data.proposedSupplyStatus < 1 || cmd_data.proposedSupplyStatus > 2) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_NOT_AUTHORIZED);
  } else {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    emberAfSimpleMeteringClusterPrintln("Setting localSupply Status %u", cmd_data.proposedSupplyStatus);
  }
  return true;
}

bool emberAfSimpleMeteringClusterSetSupplyStatusCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_simple_metering_cluster_set_supply_status_command_t cmd_data;

  if (zcl_decode_simple_metering_cluster_set_supply_status_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  (void) emberAfWriteAttribute(emberAfCurrentEndpoint(),
                               ZCL_SIMPLE_METERING_CLUSTER_ID,
                               ZCL_SUPPLY_TAMPER_STATE_ATTRIBUTE_ID,
                               CLUSTER_MASK_SERVER,
                               &cmd_data.supplyTamperState,
                               ZCL_ENUM8_ATTRIBUTE_TYPE);

  (void) emberAfWriteAttribute(emberAfCurrentEndpoint(),
                               ZCL_SIMPLE_METERING_CLUSTER_ID,
                               ZCL_SUPPLY_DEPLETION_STATE_ATTRIBUTE_ID,
                               CLUSTER_MASK_SERVER,
                               &cmd_data.supplyDepletionState,
                               ZCL_ENUM8_ATTRIBUTE_TYPE);

  (void) emberAfWriteAttribute(emberAfCurrentEndpoint(),
                               ZCL_SIMPLE_METERING_CLUSTER_ID,
                               ZCL_SUPPLY_UNCONTROLLED_FLOW_STATE_ATTRIBUTE_ID,
                               CLUSTER_MASK_SERVER,
                               &cmd_data.supplyUncontrolledFlowState,
                               ZCL_ENUM8_ATTRIBUTE_TYPE);

  (void) emberAfWriteAttribute(emberAfCurrentEndpoint(),
                               ZCL_SIMPLE_METERING_CLUSTER_ID,
                               ZCL_LOAD_LIMIT_SUPPLY_STATE_ATTRIBUTE_ID,
                               CLUSTER_MASK_SERVER,
                               &cmd_data.loadLimitSupplyState,
                               ZCL_ENUM8_ATTRIBUTE_TYPE);
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfSimpleMeteringClusterSetUncontrolledFlowThresholdCallback(EmberAfClusterCommand *cmd)
{
  (void)cmd;

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#else // !UC_BUILD

bool emberAfSimpleMeteringClusterGetProfileCallback(uint8_t intervalChannel,
                                                    uint32_t endTime,
                                                    uint8_t numberOfPeriods)
{
  return emAfTestMeterGetProfiles(intervalChannel, endTime, numberOfPeriods);
}

bool emberAfSimpleMeteringClusterRequestFastPollModeCallback(uint8_t fastPollUpdatePeriod,
                                                             uint8_t duration)
{
  uint8_t endpoint;
  uint8_t ep;
  uint8_t appliedUpdateRate;
  EmberStatus status;
  uint8_t fastPollingUpdateAttribute;

  if (!fastPolling) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_UNSUP_COMMAND);
    return true;
  }
  endpoint = emberAfCurrentEndpoint();
  ep = emberAfFindClusterServerEndpointIndex(endpoint,
                                             ZCL_SIMPLE_METERING_CLUSTER_ID);

  appliedUpdateRate = fastPollUpdatePeriod;

  if (ep == 0xFF) {
    emberAfSimpleMeteringClusterPrintln("Invalid endpoint %x", emberAfCurrentEndpoint());
    return false;
  }

  status = emberAfReadServerAttribute(endpoint,
                                      ZCL_SIMPLE_METERING_CLUSTER_ID,
                                      ZCL_FAST_POLL_UPDATE_PERIOD_ATTRIBUTE_ID,
                                      (uint8_t *)&fastPollingUpdateAttribute,
                                      sizeof(fastPollingUpdateAttribute));

  if (status == EMBER_SUCCESS) {
    if (fastPollUpdatePeriod < fastPollingUpdateAttribute) {
      appliedUpdateRate = fastPollingUpdateAttribute;
      emberAfSimpleMeteringClusterPrintln("Applying fast Poll rate %x ep %u", appliedUpdateRate, ep);
    }
  } else {
    emberAfSimpleMeteringClusterPrintln("Reading fast Poll Attribute failed. ep %u  status %x", ep, status);
    emberAfFillCommandSimpleMeteringClusterRequestFastPollModeResponse(0,
                                                                       0);
    emberAfSendResponse();
    return true;
  }

  if (emberAfGetCurrentTime() > fastPollEndTimeUtcTable[ep]) {
    duration = duration > MAX_FAST_POLLING_PERIOD ? MAX_FAST_POLLING_PERIOD : duration;
    fastPollEndTimeUtcTable[ep] = emberAfGetCurrentTime() + (duration * 60);
    emberAfSimpleMeteringClusterPrintln("Starting fast polling for %u minutes  End Time 0x%4x,current Time 0x%4x", duration, fastPollEndTimeUtcTable[ep], emberAfGetCurrentTime());
  } else {
    emberAfSimpleMeteringClusterPrintln("Fast polling mode currently active. ep %u fastPollEndTimeUtcTable[%u] 0x%4x current Time 0x%4x ", ep, ep, fastPollEndTimeUtcTable[ep], emberAfGetCurrentTime());
  }
  emberAfFillCommandSimpleMeteringClusterRequestFastPollModeResponse(appliedUpdateRate,
                                                                     fastPollEndTimeUtcTable[ep]);
  emberAfSendResponse();
  return true;
}

bool emberAfSimpleMeteringClusterStartSamplingCallback(uint32_t issuerEventId,
                                                       uint32_t startSamplingTime,
                                                       uint8_t sampleType,
                                                       uint16_t sampleRequestInterval,
                                                       uint16_t maxNumberOfSamples)
{
  static uint8_t firstIssuerId = 0;

  // Bug: SE1P2-19
  // Issuer Eventid should not be ignored. We accept *anything* for the first time.
  // After that, we only accept values greater than the max id received so far(which
  // should be the last valid issuerId received).
  if (firstIssuerId == 0) {
    minIssuerEventId = issuerEventId;
    firstIssuerId = 1;
  } else if (issuerEventId <= minIssuerEventId && startSamplingTime != 0xFFFFFFFF) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_FAILURE);
    emberAfSimpleMeteringClusterPrintln("Rejecting StartSamplingCallback issuerEventId %u minIssuerEventId %u", issuerEventId, minIssuerEventId);
    return true;
  } else {
    minIssuerEventId = issuerEventId;
  }

  // Bug: SE1P2-17
  // StartSampling Event ID field is ignored by the metering server
  //Special time reserved to cancel a startSampling Request.
  if (startSamplingTime == 0xFFFFFFFF) {
    uint8_t eventIndex = findSamplingSessionByEventId(issuerEventId);
    //Event not found.
    // The following conditional tells CSTAT that index is within array bounds;
    // previously was (index == 0xFF). Simpler to make this change rather than
    // suppress the CSTAT check on multiple code lines.
    if (!isValidSamplingDataIndex(eventIndex)) {
      emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_NOT_FOUND);
      return true;
    } else {
      samplingData[eventIndex].sampleId = 0xFFFF;
      samplingData[eventIndex].validSamples = 0x00;
      emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
      return true;
    }
  }

  uint16_t sampleId = emberAfPluginSimpleMeteringServerStartSampling(0xFFFF,
                                                                     issuerEventId,
                                                                     startSamplingTime,
                                                                     sampleType,
                                                                     sampleRequestInterval,
                                                                     maxNumberOfSamples,
                                                                     emberAfCurrentEndpoint());
  emberAfFillCommandSimpleMeteringClusterStartSamplingResponse(sampleId);
  emberAfSendResponse();
  return true;
}

bool emberAfSimpleMeteringClusterGetSampledDataCallback(uint16_t sampleId,
                                                        uint32_t earliestSampleTime,
                                                        uint8_t sampleType,
                                                        uint16_t numberOfSamples)
{
  uint8_t i;
  emberAfSimpleMeteringClusterPrintln("sampleId %u earliestSampleTime %u sampleType %u numberOfSamples %u", sampleId, earliestSampleTime, sampleType, numberOfSamples);
  for (i = 0; i < EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_MAX_SAMPLING_SESSIONS; i++) {
    uint8_t j;
    uint32_t diff;
    if (samplingData[i].sampleId == sampleId
        && samplingData[i].sampleId < 0xFFFF
        && samplingData[i].startTime <= earliestSampleTime
        && samplingData[i].sampleType == sampleType) {
      //If we have only one sample data point or lesser, we don't have enough information
      //for even one sample.
      if (samplingData[i].validSamples <= 1) {
        goto kickout;
      }

      //Bug SE1P2-19: Use the max field from the client.

      if (numberOfSamples > samplingData[i].validSamples - 1) {
        numberOfSamples = samplingData[i].validSamples - 1;
      }

      (void) emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND             \
                                        | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT),   \
                                       ZCL_SIMPLE_METERING_CLUSTER_ID,           \
                                       ZCL_GET_SAMPLED_DATA_RESPONSE_COMMAND_ID, \
                                       "vwuvv",                                  \
                                       samplingData[i].sampleId,                 \
                                       samplingData[i].startTime,                \
                                       samplingData[i].sampleType,               \
                                       samplingData[i].sampleRequestInterval,    \
                                       numberOfSamples);

      emberAfSimpleMeteringClusterPrintln("numberOfSamples 0x%2x", numberOfSamples);

      for (j = 0; j < numberOfSamples; j++) {
        uint32_t b = (samplingData[i].samples[j + 1][3] << 24) | (samplingData[i].samples[j + 1][2] << 16) | (samplingData[i].samples[j + 1][1] << 8) | (samplingData[i].samples[j + 1][0] << 0);
        uint32_t a = (samplingData[i].samples[j][3] << 24) | (samplingData[i].samples[j][2] << 16) | (samplingData[i].samples[j][1] << 8) | (samplingData[i].samples[j][0] << 0);

        diff = b - a;
        (void) emberAfPutInt24uInResp(diff);
        emberAfSimpleMeteringClusterPrintln("index %u numberOfSamples %u diff %u", j, numberOfSamples, diff);
      }
      emberAfSendResponse();
      return true;
    }
  }

  kickout:
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_NOT_FOUND);
  return true;
}

bool emberAfSimpleMeteringClusterMirrorReportAttributeResponseCallback(uint8_t notificationScheme,
                                                                       uint8_t* notificationFlags)
{
  if (notificationScheme == 0x01) {
    uint32_t functionalFlags = emberAfGetInt32u(notificationFlags, 0, 4);
    emberAfPluginSimpleMeteringServerProcessNotificationFlagsCallback(ZCL_FUNCTIONAL_NOTIFICATION_FLAGS_ATTRIBUTE_ID, functionalFlags);
  } else if (notificationScheme == 0x02) {
    uint32_t functionalFlags = emberAfGetInt32u(notificationFlags, 0, 20);
    uint32_t notificationFlags2 = emberAfGetInt32u(notificationFlags, 4, 20);
    uint32_t notificationFlags3 = emberAfGetInt32u(notificationFlags, 8, 20);
    uint32_t notificationFlags4 = emberAfGetInt32u(notificationFlags, 12, 20);
    uint32_t notificationFlags5 = emberAfGetInt32u(notificationFlags, 16, 20);
    emberAfSimpleMeteringClusterPrintln("functionalFlags 0x%4x notificationFlags2-5 0x%4x 0x%4x 0x%4x 0x%4x",
                                        functionalFlags, notificationFlags2, notificationFlags3,
                                        notificationFlags4, notificationFlags5);
    emberAfPluginSimpleMeteringServerProcessNotificationFlagsCallback(ZCL_FUNCTIONAL_NOTIFICATION_FLAGS_ATTRIBUTE_ID, functionalFlags);
    emberAfPluginSimpleMeteringServerProcessNotificationFlagsCallback(ZCL_NOTIFICATION_FLAGS_2_ATTRIBUTE_ID, notificationFlags2);
    emberAfPluginSimpleMeteringServerProcessNotificationFlagsCallback(ZCL_NOTIFICATION_FLAGS_3_ATTRIBUTE_ID, notificationFlags3);
    emberAfPluginSimpleMeteringServerProcessNotificationFlagsCallback(ZCL_NOTIFICATION_FLAGS_4_ATTRIBUTE_ID, notificationFlags4);
    emberAfPluginSimpleMeteringServerProcessNotificationFlagsCallback(ZCL_NOTIFICATION_FLAGS_5_ATTRIBUTE_ID, notificationFlags5);
  } else {
    return false;
  }
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfSimpleMeteringClusterResetLoadLimitCounterCallback(uint32_t providerId,
                                                               uint32_t issuerEventId)
{
  uint8_t counter = 0;
  emberAfSimpleMeteringClusterPrintln("Reset Load Counter providerId %u issuerEventId %u", providerId, issuerEventId);
  (void) emberAfWriteAttribute(emberAfCurrentEndpoint(),
                               ZCL_SIMPLE_METERING_CLUSTER_ID,
                               ZCL_LOAD_LIMIT_COUNTER_ATTRIBUTE_ID,
                               CLUSTER_MASK_SERVER,
                               &counter,
                               ZCL_INT8U_ATTRIBUTE_TYPE);
  return true;
}

bool emberAfSimpleMeteringClusterChangeSupplyCallback(uint32_t providerId,
                                                      uint32_t issuerEventId,
                                                      uint32_t requestDateTime,
                                                      uint32_t implementationDateTime,
                                                      uint8_t proposedSupplyStatus,
                                                      uint8_t supplyControlBits)
{
  uint32_t delay;
  EmberAfClusterCommand *cmd;

  emberAfSimpleMeteringClusterPrintln("Change Supply Callback providerId %u issuerEventId %u implementationDateTime %u supplyStatus %u", providerId, issuerEventId, implementationDateTime, supplyControlBits);
  // TODO: fix this hard-coded check
  if (providerId != PROVIDER_ID) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_NOT_AUTHORIZED);
    return true;
  }
  if (implementationDateTime < emberAfGetCurrentTime() && implementationDateTime != 0) {
    emberAfSimpleMeteringClusterPrintln("implementationDateTime %u", implementationDateTime);
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_INVALID_VALUE);
    return true;
  }
  if (implementationDateTime == 0xFFFFFFFF) {
    emberAfSimpleMeteringClusterPrintln("Canceling change supply");
    // Per SE specification: "The ProposedChangeImplementationTime attribute
    // indicates the time at which a proposed change to the supply is to be
    // implemented. If there is no change of supply pending, this attribute
    // will be set to 0xFFFFFFFF.
    (void) emberAfWriteAttribute(emberAfCurrentEndpoint(),
                                 ZCL_SIMPLE_METERING_CLUSTER_ID,
                                 ZCL_PROPOSED_CHANGE_SUPPLY_IMPLEMENTATION_TIME_ATTRIBUTE_ID,
                                 CLUSTER_MASK_SERVER,
                                 (uint8_t *)&implementationDateTime,
                                 ZCL_UTC_TIME_ATTRIBUTE_TYPE);
    slxu_zigbee_event_set_inactive(supplyEvent);
  }
  if (implementationDateTime == 0x00000000 ) {
    if ((supplyControlBits & 0x01) == 0x01 || proposedSupplyStatus == 2) {
      emberAfFillCommandSimpleMeteringClusterSupplyStatusResponse(providerId,
                                                                  issuerEventId,
                                                                  implementationDateTime,
                                                                  proposedSupplyStatus);
      emberAfSendResponse();
    } else {
      emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    }
  } else {
    (void) emberAfWriteAttribute(emberAfCurrentEndpoint(),
                                 ZCL_SIMPLE_METERING_CLUSTER_ID,
                                 ZCL_PROPOSED_CHANGE_SUPPLY_IMPLEMENTATION_TIME_ATTRIBUTE_ID,
                                 CLUSTER_MASK_SERVER,
                                 (uint8_t *)&implementationDateTime,
                                 ZCL_UTC_TIME_ATTRIBUTE_TYPE);

    (void) emberAfWriteAttribute(emberAfCurrentEndpoint(),
                                 ZCL_SIMPLE_METERING_CLUSTER_ID,
                                 ZCL_PROPOSED_CHANGE_SUPPLY_STATUS_ATTRIBUTE_ID,
                                 CLUSTER_MASK_SERVER,
                                 &proposedSupplyStatus,
                                 ZCL_ENUM8_ATTRIBUTE_TYPE);
    delay = implementationDateTime - emberAfGetCurrentTime();
    cmd = emberAfCurrentCommand();
    changeSupply.srcEndpoint = cmd->apsFrame->destinationEndpoint;
    changeSupply.destEndpoint = cmd->apsFrame->sourceEndpoint;
    changeSupply.nodeId = cmd->source;
    changeSupply.providerId = providerId;
    changeSupply.issuerEventId = issuerEventId;
    changeSupply.implementationDateTime = implementationDateTime;
    changeSupply.proposedSupplyStatus = proposedSupplyStatus;
    if ((supplyControlBits & 0x01) == 0x01) {
      slxu_zigbee_event_set_delay_ms(supplyEvent,
                                     delay * 1000);
    }
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  }

  return true;
}

bool emberAfSimpleMeteringClusterLocalChangeSupplyCallback(uint8_t proposedSupplyStatus)
{
  if (proposedSupplyStatus < 1 || proposedSupplyStatus > 2) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_NOT_AUTHORIZED);
  } else {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    emberAfSimpleMeteringClusterPrintln("Setting localSupply Status %u", proposedSupplyStatus);
  }
  return true;
}

bool emberAfSimpleMeteringClusterSetSupplyStatusCallback(uint32_t issuerEventId,
                                                         uint8_t supplyTamperState,
                                                         uint8_t supplyDepletionState,
                                                         uint8_t supplyUncontrolledFlowState,
                                                         uint8_t loadLimitSupplyState)
{
  (void) emberAfWriteAttribute(emberAfCurrentEndpoint(),
                               ZCL_SIMPLE_METERING_CLUSTER_ID,
                               ZCL_SUPPLY_TAMPER_STATE_ATTRIBUTE_ID,
                               CLUSTER_MASK_SERVER,
                               &supplyTamperState,
                               ZCL_ENUM8_ATTRIBUTE_TYPE);

  (void) emberAfWriteAttribute(emberAfCurrentEndpoint(),
                               ZCL_SIMPLE_METERING_CLUSTER_ID,
                               ZCL_SUPPLY_DEPLETION_STATE_ATTRIBUTE_ID,
                               CLUSTER_MASK_SERVER,
                               &supplyDepletionState,
                               ZCL_ENUM8_ATTRIBUTE_TYPE);

  (void) emberAfWriteAttribute(emberAfCurrentEndpoint(),
                               ZCL_SIMPLE_METERING_CLUSTER_ID,
                               ZCL_SUPPLY_UNCONTROLLED_FLOW_STATE_ATTRIBUTE_ID,
                               CLUSTER_MASK_SERVER,
                               &supplyUncontrolledFlowState,
                               ZCL_ENUM8_ATTRIBUTE_TYPE);

  (void) emberAfWriteAttribute(emberAfCurrentEndpoint(),
                               ZCL_SIMPLE_METERING_CLUSTER_ID,
                               ZCL_LOAD_LIMIT_SUPPLY_STATE_ATTRIBUTE_ID,
                               CLUSTER_MASK_SERVER,
                               &loadLimitSupplyState,
                               ZCL_ENUM8_ATTRIBUTE_TYPE);
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfSimpleMeteringClusterSetUncontrolledFlowThresholdCallback(uint32_t providerId,
                                                                      uint32_t issuerEventId,
                                                                      uint16_t uncontrolledFlowThreshold,
                                                                      uint8_t unitOfMeasure,
                                                                      uint16_t multiplier,
                                                                      uint16_t divisor,
                                                                      uint8_t stabilisationPeriod,
                                                                      uint16_t measurementPeriod)
{
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#endif // UC_BUILD

#ifdef UC_BUILD

uint32_t emberAfSimpleMeteringClusterServerCommandParse(sl_service_opcode_t opcode,
                                                        sl_service_function_context_t *context)
{
  (void)opcode;

  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_GET_PROFILE_COMMAND_ID:
      {
        wasHandled = emberAfSimpleMeteringClusterGetProfileCallback(cmd);
        break;
      }
      case ZCL_REQUEST_FAST_POLL_MODE_COMMAND_ID:
      {
        wasHandled = emberAfSimpleMeteringClusterRequestFastPollModeCallback(cmd);
        break;
      }
      case ZCL_START_SAMPLING_COMMAND_ID:
      {
        wasHandled = emberAfSimpleMeteringClusterStartSamplingCallback(cmd);
        break;
      }
      case ZCL_GET_SAMPLED_DATA_COMMAND_ID:
      {
        wasHandled = emberAfSimpleMeteringClusterGetSampledDataCallback(cmd);
        break;
      }
      case ZCL_MIRROR_REPORT_ATTRIBUTE_RESPONSE_COMMAND_ID:
      {
        wasHandled = emberAfSimpleMeteringClusterMirrorReportAttributeResponseCallback(cmd);
        break;
      }
      case ZCL_RESET_LOAD_LIMIT_COUNTER_COMMAND_ID:
      {
        wasHandled = emberAfSimpleMeteringClusterResetLoadLimitCounterCallback(cmd);
        break;
      }
      case ZCL_CHANGE_SUPPLY_COMMAND_ID:
      {
        wasHandled = emberAfSimpleMeteringClusterChangeSupplyCallback(cmd);
        break;
      }
      case ZCL_LOCAL_CHANGE_SUPPLY_COMMAND_ID:
      {
        wasHandled = emberAfSimpleMeteringClusterLocalChangeSupplyCallback(cmd);
        break;
      }
      case ZCL_SET_SUPPLY_STATUS_COMMAND_ID:
      {
        wasHandled = emberAfSimpleMeteringClusterSetSupplyStatusCallback(cmd);
        break;
      }
      case ZCL_SET_UNCONTROLLED_FLOW_THRESHOLD_COMMAND_ID:
      {
        wasHandled = emberAfSimpleMeteringClusterSetUncontrolledFlowThresholdCallback(cmd);
        break;
      }
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD

WEAK(void emAfTestMeterInit(uint8_t endpoint))
{
  (void) endpoint;
}

WEAK(void emAfTestMeterTick(uint8_t endpoint))
{
  (void) endpoint;
}

WEAK(bool emAfTestMeterGetProfiles(uint8_t intervalChannel,
                                   uint32_t endTime,
                                   uint8_t numberOfPeriods))
{
  (void) intervalChannel;
  (void) endTime;
  (void) numberOfPeriods;

  return false;
}
