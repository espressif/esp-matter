/***************************************************************************//**
 * @file
 * @brief This is the source for the plugin used to add an IAS Zone cluster server
 * to a project.  This source handles zone enrollment and storing of
 * attributes from a CIE device, and provides an API for different plugins to
 * post updated zone status values.
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

// *****************************************************************************
// * ias-zone-server.c
// *
// *
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

#include "app/framework/include/af.h"
#include "ias-zone-server.h"
#include "hal/hal.h"

#ifdef EMBER_SCRIPTED_TEST
#include "app/framework/plugin/ias-zone-server/ias-zone-server-test.h"
#endif

#ifdef UC_BUILD
#include "ias-zone-server-config.h"
#include "zap-cluster-command-parser.h"
#include "sl_component_catalog.h"
#ifdef SL_CATALOG_ZIGBEE_WWAH_APP_EVENT_RETRY_MANAGER_PRESENT
#include "wwah-app-event-retry-manager-config.h"
#endif
#if (EMBER_AF_PLUGIN_IAS_ZONE_SERVER_ENABLE_QUEUE == 1)
#define ENABLE_QUEUE
#endif
#if (EMBER_AF_PLUGIN_IAS_ZONE_SERVER_UNLIMITED_RETRIES == 1)
#define UNLIMITED_RETRIES
#endif
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_WWAH_APP_EVENT_RETRY_MANAGER
#define SL_CATALOG_ZIGBEE_WWAH_APP_EVENT_RETRY_MANAGER_PRESENT
#endif
#ifdef EMBER_AF_PLUGIN_IAS_ZONE_SERVER_ENABLE_QUEUE
#define ENABLE_QUEUE
#endif
#ifdef EMBER_AF_PLUGIN_IAS_ZONE_SERVER_UNLIMITED_RETRIES
#define UNLIMITED_RETRIES
#endif
#endif // UC_BUILD

#define UNDEFINED_ZONE_ID 0xFF
#define DELAY_TIMER_MS (1 * MILLISECOND_TICKS_PER_SECOND)
#define IAS_ZONE_SERVER_PAYLOAD_COMMAND_IDX  0x02
#define ZCL_FRAME_CONTROL_IDX 0x00

#if defined(ENABLE_QUEUE)
  #if defined(SL_CATALOG_ZIGBEE_WWAH_APP_EVENT_RETRY_MANAGER_PRESENT)
    #define NUM_QUEUE_ENTRIES EMBER_AF_PLUGIN_WWAH_APP_EVENT_RETRY_MANAGER_QUEUE_SIZE
  #else // !WWAH_APP_EVENT_RETRY_MANAGER_PRESENT
    #define NUM_QUEUE_ENTRIES EMBER_AF_PLUGIN_IAS_ZONE_SERVER_QUEUE_DEPTH
  #endif // WWAH_APP_EVENT_RETRY_MANAGER_PRESENT
#else // !ENABLE_QUEUE
  #define NUM_QUEUE_ENTRIES 0
#endif // ENABLE_QUEUE

#define DEFAULT_ENROLLMENT_METHOD  EMBER_ZCL_IAS_ZONE_ENROLLMENT_MODE_REQUEST

typedef struct {
  uint8_t endpoint;
  uint16_t status;
  uint32_t eventTimeMs;
} IasZoneStatusQueueEntry;

typedef struct {
  uint8_t entriesInQueue;
  uint8_t startIdx;
  uint8_t lastIdx;
  IasZoneStatusQueueEntry buffer[NUM_QUEUE_ENTRIES];
} IasZoneStatusQueue;

//-----------------------------------------------------------------------------
// Globals
#ifdef UC_BUILD
sl_zigbee_event_t emberAfPluginIasZoneServerManageQueueEvent;
#define serverManageQueueEventControl (&emberAfPluginIasZoneServerManageQueueEvent)
void emberAfPluginIasZoneServerManageQueueEventHandler(SLXU_UC_EVENT);
#else // !UC_BUILD
EmberEventControl emberAfPluginIasZoneServerManageQueueEventControl;
#define serverManageQueueEventControl emberAfPluginIasZoneServerManageQueueEventControl
#endif // UC_BUILD

static EmberAfIasZoneEnrollmentMode enrollmentMethod;

#if defined(ENABLE_QUEUE)
IasZoneStatusQueue messageQueue;

// Status queue retry parameters
typedef struct {
  IasZoneStatusQueueRetryConfig config;
  uint32_t currentBackoffTimeSec;
  uint8_t currentRetryCount;
} IasZoneStatusQueueRetryParameters;

// Set up status queue retry parameters.
IasZoneStatusQueueRetryParameters queueRetryParams = {
  .config = {
    .firstBackoffTimeSec = EMBER_AF_PLUGIN_IAS_ZONE_SERVER_FIRST_BACKOFF_TIME_SEC,
    .backoffSeqCommonRatio = EMBER_AF_PLUGIN_IAS_ZONE_SERVER_BACKOFF_SEQUENCE_COMMON_RATIO,
    .maxBackoffTimeSec = EMBER_AF_PLUGIN_IAS_ZONE_SERVER_MAX_BACKOFF_TIME_SEC,
#ifdef UNLIMITED_RETRIES
    .unlimitedRetries = true,
#else // !UNLIMITED_RETRIES
    .unlimitedRetries = false,
#endif // UNLIMITED_RETRIES
    .maxRetryAttempts = EMBER_AF_PLUGIN_IAS_ZONE_SERVER_MAX_RETRY_ATTEMPTS
  },
  .currentBackoffTimeSec = EMBER_AF_PLUGIN_IAS_ZONE_SERVER_FIRST_BACKOFF_TIME_SEC,
  .currentRetryCount = 0,
};

static void resetCurrentQueueRetryParams(void)
{
  queueRetryParams.currentRetryCount = 0;
  queueRetryParams.currentBackoffTimeSec = queueRetryParams.config.firstBackoffTimeSec;
}

#endif // ENABLE_QUEUE

//-----------------------------------------------------------------------------
// Forward declarations

static void setZoneId(uint8_t endpoint, uint8_t zoneId);
static bool areZoneServerAttributesTokenized(uint8_t endpoint);
static bool isValidEnrollmentMode(EmberAfIasZoneEnrollmentMode method);
#if defined(ENABLE_QUEUE)
static uint16_t computeElapsedTimeQs(IasZoneStatusQueueEntry *entry);
static void bufferInit(IasZoneStatusQueue *ring);
static int16_t copyToBuffer(IasZoneStatusQueue *ring,
                            const IasZoneStatusQueueEntry *entry);
static int16_t popFromBuffer(IasZoneStatusQueue *ring,
                             IasZoneStatusQueueEntry *entry);
#endif // ENABLE_QUEUE

//-----------------------------------------------------------------------------
// Functions

static EmberStatus sendToClient(uint8_t endpoint)
{
  EmberStatus status;

  // If the device is not a network, there is no one to send to, so do nothing
  if (emberAfNetworkState() != EMBER_JOINED_NETWORK) {
    return EMBER_NETWORK_DOWN;
  }

  // Remote endpoint need not be set, since it will be provided by the call to
  // emberAfSendCommandUnicastToBindings()
  emberAfSetCommandEndpoints(endpoint, 0);

  // A binding table entry is created on Zone Enrollment for each endpoint, so
  // a simple call to SendCommandUnicastToBinding will handle determining the
  // destination endpoint, address, etc for us.
  status = emberAfSendCommandUnicastToBindings();

  if (EMBER_SUCCESS != status) {
    return status;
  }

  return status;
}

static void enrollWithClient(uint8_t endpoint)
{
  EmberStatus status;
  emberAfFillCommandIasZoneClusterZoneEnrollRequest(
    EMBER_AF_PLUGIN_IAS_ZONE_SERVER_ZONE_TYPE,
    EMBER_AF_MANUFACTURER_CODE);
  status = sendToClient(endpoint);
  if (status == EMBER_SUCCESS) {
    emberAfIasZoneClusterPrintln("Sent enroll request to IAS Zone client.");
  } else {
    emberAfIasZoneClusterPrintln("Error sending enroll request: 0x%x\n",
                                 status);
  }
}

EmberAfStatus emberAfIasZoneClusterServerPreAttributeChangedCallback(
  uint8_t endpoint,
  EmberAfAttributeId attributeId,
  EmberAfAttributeType attributeType,
  uint8_t size,
  uint8_t *value)
{
  uint8_t i;
  bool zeroAddress;
  EmberBindingTableEntry bindingEntry;
  EmberBindingTableEntry currentBind;
  EmberEUI64 destEUI;
  uint8_t ieeeAddress[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

  // If this is not a CIE Address write, the CIE address has already been
  // written, or the IAS Zone server is already enrolled, do nothing.
  if (attributeId != ZCL_IAS_CIE_ADDRESS_ATTRIBUTE_ID
      || emberAfCurrentCommand() == NULL) {
    return EMBER_ZCL_STATUS_SUCCESS;
  }

  MEMCOPY(destEUI, value, EUI64_SIZE);

  // Create the binding table entry

  // This code assumes that the endpoint and device that is setting the CIE
  // address is the CIE device itself, and as such the remote endpoint to bind
  // to is the endpoint that generated the attribute change.  This
  // assumption is made based on analysis of the behavior of CIE devices
  // currently existing in the field.
  bindingEntry.type = EMBER_UNICAST_BINDING;
  bindingEntry.local = endpoint;
  bindingEntry.clusterId = ZCL_IAS_ZONE_CLUSTER_ID;
  bindingEntry.remote = emberAfCurrentCommand()->apsFrame->sourceEndpoint;
  MEMCOPY(bindingEntry.identifier, destEUI, EUI64_SIZE);

  // Cycle through the binding table until we find a valid entry that is not
  // being used, then use the created entry to make the bind.
  for (i = 0; i < EMBER_BINDING_TABLE_SIZE; i++) {
    if (emberGetBinding(i, &currentBind) != EMBER_SUCCESS) {
      //break out of the loop to ensure that an error message still prints
      break;
    }
    if (currentBind.type != EMBER_UNUSED_BINDING) {
      // If the binding table entry created based on the response already exists
      // do nothing.
      if ((currentBind.local == bindingEntry.local)
          && (currentBind.clusterId == bindingEntry.clusterId)
          && (currentBind.remote == bindingEntry.remote)
          && (currentBind.type == bindingEntry.type)) {
        break;
      }
      // If this spot in the binding table already exists, move on to the next
      continue;
    } else {
      emberSetBinding(i, &bindingEntry);
      break;
    }
  }

  zeroAddress = true;
  emberAfReadServerAttribute(endpoint,
                             ZCL_IAS_ZONE_CLUSTER_ID,
                             ZCL_IAS_CIE_ADDRESS_ATTRIBUTE_ID,
                             (uint8_t*)ieeeAddress,
                             8);
  for (i = 0; i < 8; i++) {
    if (ieeeAddress[i] != 0) {
      zeroAddress = false;
    }
  }
  emberAfAppPrint("\nzero address: %d\n", zeroAddress);

  if ((zeroAddress == true) && (enrollmentMethod == EMBER_ZCL_IAS_ZONE_ENROLLMENT_MODE_REQUEST)) {
    // Only send the enrollment request if the mode is AUTO-ENROLL-REQUEST.
    // We need to delay to get around a bug where we can't send a command
    // at this point because then the Write Attributes response will not
    // be sent.  But we also delay to give the client time to configure us.
    emberAfIasZoneClusterPrintln("Sending enrollment after %d ms",
                                 DELAY_TIMER_MS);
    slxu_zigbee_zcl_schedule_server_tick_extended(endpoint,
                                                  ZCL_IAS_ZONE_CLUSTER_ID,
                                                  DELAY_TIMER_MS,
                                                  EMBER_AF_SHORT_POLL,
                                                  EMBER_AF_STAY_AWAKE);
  }

  return EMBER_ZCL_STATUS_SUCCESS;
}

EmberAfStatus emberAfPluginIasZoneClusterSetEnrollmentMethod(uint8_t endpoint,
                                                             EmberAfIasZoneEnrollmentMode method)
{
  EmberAfStatus status;

  if (emberAfIasZoneClusterAmIEnrolled(endpoint)) {
    emberAfIasZoneClusterPrintln("Error: Already enrolled");
    status = EMBER_ZCL_STATUS_NOT_AUTHORIZED;
  } else if (!isValidEnrollmentMode(method)) {
    emberAfIasZoneClusterPrintln("Invalid IAS Zone Server Enrollment Mode: %d", method);
    status = EMBER_ZCL_STATUS_INVALID_VALUE;
  } else {
    enrollmentMethod = method;
#ifndef EZSP_HOST
    halCommonSetToken(TOKEN_PLUGIN_IAS_ZONE_SERVER_ENROLLMENT_METHOD, &enrollmentMethod);
#endif
    emberAfIasZoneClusterPrintln("IAS Zone Server Enrollment Mode: %d", method);
    status = EMBER_ZCL_STATUS_SUCCESS;
  }
  return status;
}

static bool isValidEnrollmentMode(EmberAfIasZoneEnrollmentMode method)
{
  return ((method == EMBER_ZCL_IAS_ZONE_ENROLLMENT_MODE_TRIP_TO_PAIR)
          || (method == EMBER_ZCL_IAS_ZONE_ENROLLMENT_MODE_AUTO_ENROLLMENT_RESPONSE)
          || (method == EMBER_ZCL_IAS_ZONE_ENROLLMENT_MODE_REQUEST));
}

bool emberAfIasZoneClusterAmIEnrolled(uint8_t endpoint)
{
  EmberAfIasZoneState zoneState = 0;  // Clear this out completely.
  EmberAfStatus status;
  status = emberAfReadServerAttribute(endpoint,
                                      ZCL_IAS_ZONE_CLUSTER_ID,
                                      ZCL_ZONE_STATE_ATTRIBUTE_ID,
                                      (unsigned char*)&zoneState,
                                      1);  // uint8_t size

  return (status == EMBER_ZCL_STATUS_SUCCESS
          && zoneState == EMBER_ZCL_IAS_ZONE_STATE_ENROLLED);
}

static void updateEnrollState(uint8_t endpoint, bool enrolled)
{
  EmberAfIasZoneState zoneState = (enrolled
                                   ? EMBER_ZCL_IAS_ZONE_STATE_ENROLLED
                                   : EMBER_ZCL_IAS_ZONE_STATE_NOT_ENROLLED);

  emberAfWriteServerAttribute(endpoint,
                              ZCL_IAS_ZONE_CLUSTER_ID,
                              ZCL_ZONE_STATE_ATTRIBUTE_ID,
                              (uint8_t*)&zoneState,
                              ZCL_INT8U_ATTRIBUTE_TYPE);
  emberAfIasZoneClusterPrintln("IAS Zone Server State: %pEnrolled",
                               (enrolled
                                ? ""
                                : "NOT "));
}

//------------------------
// Commands callbacks

#ifdef UC_BUILD

bool emberAfIasZoneClusterZoneEnrollResponseCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_ias_zone_cluster_zone_enroll_response_command_t cmd_data;
  uint8_t endpoint;
  uint8_t epZoneId;
  EmberAfStatus status;

  if (zcl_decode_ias_zone_cluster_zone_enroll_response_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  endpoint = emberAfCurrentEndpoint();
  status = emberAfReadServerAttribute(endpoint,
                                      ZCL_IAS_ZONE_CLUSTER_ID,
                                      ZCL_ZONE_ID_ATTRIBUTE_ID,
                                      &epZoneId,
                                      sizeof(uint8_t));
  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    if (cmd_data.enrollResponseCode == EMBER_ZCL_IAS_ENROLL_RESPONSE_CODE_SUCCESS) {
      updateEnrollState(endpoint, true);
      setZoneId(endpoint, cmd_data.zoneId);
    } else {
      updateEnrollState(endpoint, false);
      setZoneId(endpoint, UNDEFINED_ZONE_ID);
    }

    return true;
  }

  emberAfAppPrintln("ERROR: IAS Zone Server unable to read zone ID attribute");
  return true;
}

#else // !UC_BUILD

bool emberAfIasZoneClusterZoneEnrollResponseCallback(uint8_t enrollResponseCode,
                                                     uint8_t zoneId)
{
  uint8_t endpoint;
  uint8_t epZoneId;
  EmberAfStatus status;

  endpoint = emberAfCurrentEndpoint();
  status = emberAfReadServerAttribute(endpoint,
                                      ZCL_IAS_ZONE_CLUSTER_ID,
                                      ZCL_ZONE_ID_ATTRIBUTE_ID,
                                      &epZoneId,
                                      sizeof(uint8_t));
  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    if (enrollResponseCode == EMBER_ZCL_IAS_ENROLL_RESPONSE_CODE_SUCCESS) {
      updateEnrollState(endpoint, true);
      setZoneId(endpoint, zoneId);
    } else {
      updateEnrollState(endpoint, false);
      setZoneId(endpoint, UNDEFINED_ZONE_ID);
    }

    return true;
  }

  emberAfAppPrintln("ERROR: IAS Zone Server unable to read zone ID attribute");
  return true;
}

#endif // UC_BUILD

static EmberStatus sendZoneUpdate(uint16_t zoneStatus,
                                  uint16_t timeSinceStatusOccurredQs,
                                  uint8_t endpoint)
{
  EmberStatus status;

  if (!emberAfIasZoneClusterAmIEnrolled(endpoint)) {
    return EMBER_INVALID_CALL;
  }
  emberAfFillCommandIasZoneClusterZoneStatusChangeNotification(
    zoneStatus,
    0, // extended status, must be zero per spec
    emberAfPluginIasZoneServerGetZoneId(endpoint),
    timeSinceStatusOccurredQs); // called "delay" in the spec
  status = sendToClient(endpoint);

  return status;
}

#if defined(ENABLE_QUEUE)
static void addNewEntryToQueue(const IasZoneStatusQueueEntry *newEntry)
{
  emberAfIasZoneClusterPrintln("Adding new entry to queue");
  copyToBuffer(&messageQueue, newEntry);
}
#endif // ENABLE_QUEUE

EmberStatus emberAfPluginIasZoneServerUpdateZoneStatus(
  uint8_t endpoint,
  uint16_t newStatus,
  uint16_t timeSinceStatusOccurredQs)
{
#if defined(ENABLE_QUEUE)
  IasZoneStatusQueueEntry newBufferEntry;
  newBufferEntry.endpoint = endpoint;
  newBufferEntry.status = newStatus;
  newBufferEntry.eventTimeMs = halCommonGetInt32uMillisecondTick();
#endif // ENABLE_QUEUE
  EmberStatus sendStatus;

  emberAfWriteServerAttribute(endpoint,
                              ZCL_IAS_ZONE_CLUSTER_ID,
                              ZCL_ZONE_STATUS_ATTRIBUTE_ID,
                              (uint8_t*)&newStatus,
                              ZCL_INT16U_ATTRIBUTE_TYPE);

  if (enrollmentMethod == EMBER_ZCL_IAS_ZONE_ENROLLMENT_MODE_TRIP_TO_PAIR) {
    // If unenrolled, send Zone Enroll Request command.
    if (!emberAfIasZoneClusterAmIEnrolled(endpoint)) {
      slxu_zigbee_zcl_schedule_server_tick(endpoint,
                                           ZCL_IAS_ZONE_CLUSTER_ID,
                                           DELAY_TIMER_MS);
      // Don't send the zone status update since not enrolled.
      return EMBER_SUCCESS;
    }
  }

#if defined(ENABLE_QUEUE)
  // If there are items in the queue waiting to send, this event should not
  // be transmitted, as that could cause the client to receive the events out
  // of order.  Instead, just add the device to the queue
  if (messageQueue.entriesInQueue == 0) {
    sendStatus = sendZoneUpdate(newStatus,
                                timeSinceStatusOccurredQs,
                                endpoint);
  } else {
    // Add a new element to the status queue and depending on the network state
    // either try to resend the first element in the queue immediately or try to
    // restart the parent research pattern.
    addNewEntryToQueue(&newBufferEntry);

    EmberNetworkStatus networkState = emberAfNetworkState();

    if (networkState == EMBER_JOINED_NETWORK_NO_PARENT) {
      emberAfStartMoveCallback();
    } else if (networkState == EMBER_JOINED_NETWORK) {
      resetCurrentQueueRetryParams();
      slxu_zigbee_event_set_active(serverManageQueueEventControl);
    }

    return EMBER_SUCCESS;
  }

#else // !ENABLE_QUEUE
  sendStatus = sendZoneUpdate(newStatus, timeSinceStatusOccurredQs, endpoint);
#endif // ENABLE_QUEUE

  if (sendStatus == EMBER_SUCCESS) {
#if defined(ENABLE_QUEUE)
    // Add a new entry to the zoneUpdate buffer
    addNewEntryToQueue(&newBufferEntry);
#endif // ENABLE_QUEUE
  } else {
    // If we're not on a network and never were, we don't need to do anything.
    // If we used to be on a network and can't talk to our parent, we should
    // try to rejoin the network and add the message to the queue
    if (emberAfNetworkState() == EMBER_JOINED_NETWORK_NO_PARENT) {
      emberAfStartMoveCallback();
#if defined(ENABLE_QUEUE)
      // Add a new entry to the zoneUpdate buffer
      addNewEntryToQueue(&newBufferEntry);
#endif // ENABLE_QUEUE
    }
    emberAfIasZoneClusterPrintln("Failed to send IAS Zone update. Err 0x%x",
                                 sendStatus);
  }
  return sendStatus;
}

void emberAfPluginIasZoneServerManageQueueEventHandler(SLXU_UC_EVENT)
{
#if defined(ENABLE_QUEUE)
  IasZoneStatusQueueEntry *bufferStart;
  uint16_t status;
  uint16_t elapsedTimeQs;
  uint16_t airTimeRemainingMs;

  //If the queue was emptied without our interaction, do nothing
  if (messageQueue.entriesInQueue == 0) {
    slxu_zigbee_event_set_inactive(serverManageQueueEventControl);
    return;
  }

  // Otherwise, pull out the first item and attempt to retransmit it.  The
  // message complete callback will handle removing items from the queue

  // To prevent an activity storm from flooding with retry requests, only
  // re-send a message if it's been at least
  // EMBER_AF_PLUGIN_IAS_ZONE_SERVER_MIN_OTA_TIME_MS since it was sent.
  bufferStart = &(messageQueue.buffer[messageQueue.startIdx]);
  elapsedTimeQs = computeElapsedTimeQs(bufferStart);

  if (elapsedTimeQs < (EMBER_AF_PLUGIN_IAS_ZONE_SERVER_MIN_OTA_TIME_MS
                       / (MILLISECOND_TICKS_PER_SECOND / 4))) {
    airTimeRemainingMs = EMBER_AF_PLUGIN_IAS_ZONE_SERVER_MIN_OTA_TIME_MS
                         - (elapsedTimeQs * MILLISECOND_TICKS_PER_SECOND / 4);
    emberAfIasZoneClusterPrintln(
      "Not enough time passed for a retry, sleeping %d more mS",
      airTimeRemainingMs);
    slxu_zigbee_event_set_delay_ms(serverManageQueueEventControl,
                                   airTimeRemainingMs);
  } else {
    status = bufferStart->status;
    emberAfIasZoneClusterPrintln(
      "Attempting to resend a queued zone status update (status: 0x%02X, "
      "event time (s): %d) with time of %d. Retry count: %d",
      bufferStart->status,
      bufferStart->eventTimeMs / MILLISECOND_TICKS_PER_SECOND,
      elapsedTimeQs,
      queueRetryParams.currentRetryCount);
    sendZoneUpdate(status, elapsedTimeQs, bufferStart->endpoint);
    slxu_zigbee_event_set_inactive(serverManageQueueEventControl);
  }
#else // !ENABLE_QUEUE
  slxu_zigbee_event_set_inactive(serverManageQueueEventControl);
#endif // ENABLE_QUEUE
}

void emberAfIasZoneClusterServerInitCallback(uint8_t endpoint)
{
  slxu_zigbee_event_init(serverManageQueueEventControl,
                         emberAfPluginIasZoneServerManageQueueEventHandler);
  EmberAfIasZoneType zoneType;
  if (!areZoneServerAttributesTokenized(endpoint)) {
    emberAfAppPrint("WARNING: ATTRIBUTES ARE NOT BEING STORED IN FLASH! ");
    emberAfAppPrintln("DEVICE WILL NOT FUNCTION PROPERLY AFTER REBOOTING!!");
  }

#ifndef EZSP_HOST
  halCommonGetToken(&enrollmentMethod, TOKEN_PLUGIN_IAS_ZONE_SERVER_ENROLLMENT_METHOD);
#else
  enrollmentMethod = DEFAULT_ENROLLMENT_METHOD;
#endif
  if (!isValidEnrollmentMode(enrollmentMethod)) {
    // Default Enrollment Method to AUTO-ENROLL-REQUEST.
    enrollmentMethod = DEFAULT_ENROLLMENT_METHOD;
  }

#if defined(ENABLE_QUEUE)
  bufferInit(&messageQueue);
#endif // ENABLE_QUEUE

  zoneType = (EmberAfIasZoneType)EMBER_AF_PLUGIN_IAS_ZONE_SERVER_ZONE_TYPE;
  (void) emberAfWriteAttribute(endpoint,
                               ZCL_IAS_ZONE_CLUSTER_ID,
                               ZCL_ZONE_TYPE_ATTRIBUTE_ID,
                               CLUSTER_MASK_SERVER,
                               (uint8_t*)&zoneType,
                               ZCL_INT16U_ATTRIBUTE_TYPE);

  emberAfPluginIasZoneServerUpdateZoneStatus(endpoint,
                                             0,   // status: All alarms cleared
                                             0);  // time since status occurred
}

void emberAfIasZoneClusterServerTickCallback(uint8_t endpoint)
{
  enrollWithClient(endpoint);
}

uint8_t emberAfPluginIasZoneServerGetZoneId(uint8_t endpoint)
{
  uint8_t zoneId = UNDEFINED_ZONE_ID;
  emberAfReadServerAttribute(endpoint,
                             ZCL_IAS_ZONE_CLUSTER_ID,
                             ZCL_ZONE_ID_ATTRIBUTE_ID,
                             &zoneId,
                             emberAfGetDataSize(ZCL_INT8U_ATTRIBUTE_TYPE));
  return zoneId;
}

//------------------------------------------------------------------------------
//
// This function will verify that all attributes necessary for the IAS zone
// server to properly retain functionality through a power failure are
// tokenized.
//
//------------------------------------------------------------------------------
static bool areZoneServerAttributesTokenized(uint8_t endpoint)
{
  EmberAfAttributeMetadata *metadata;

  metadata = emberAfLocateAttributeMetadata(endpoint,
                                            ZCL_IAS_ZONE_CLUSTER_ID,
                                            ZCL_IAS_CIE_ADDRESS_ATTRIBUTE_ID,
                                            CLUSTER_MASK_SERVER,
                                            EMBER_AF_NULL_MANUFACTURER_CODE);
  if (!emberAfAttributeIsTokenized(metadata)) {
    return false;
  }

  metadata = emberAfLocateAttributeMetadata(endpoint,
                                            ZCL_IAS_ZONE_CLUSTER_ID,
                                            ZCL_ZONE_STATE_ATTRIBUTE_ID,
                                            CLUSTER_MASK_SERVER,
                                            EMBER_AF_NULL_MANUFACTURER_CODE);
  if (!emberAfAttributeIsTokenized(metadata)) {
    return false;
  }

  metadata = emberAfLocateAttributeMetadata(endpoint,
                                            ZCL_IAS_ZONE_CLUSTER_ID,
                                            ZCL_ZONE_TYPE_ATTRIBUTE_ID,
                                            CLUSTER_MASK_SERVER,
                                            EMBER_AF_NULL_MANUFACTURER_CODE);
  if (!emberAfAttributeIsTokenized(metadata)) {
    return false;
  }

  metadata = emberAfLocateAttributeMetadata(endpoint,
                                            ZCL_IAS_ZONE_CLUSTER_ID,
                                            ZCL_ZONE_ID_ATTRIBUTE_ID,
                                            CLUSTER_MASK_SERVER,
                                            EMBER_AF_NULL_MANUFACTURER_CODE);
  if (!emberAfAttributeIsTokenized(metadata)) {
    return false;
  }

  return true;
}

static void setZoneId(uint8_t endpoint, uint8_t zoneId)
{
  emberAfIasZoneClusterPrintln("IAS Zone Server Zone ID: 0x%X", zoneId);
  emberAfWriteServerAttribute(endpoint,
                              ZCL_IAS_ZONE_CLUSTER_ID,
                              ZCL_ZONE_ID_ATTRIBUTE_ID,
                              &zoneId,
                              ZCL_INT8U_ATTRIBUTE_TYPE);
}

static void unenrollSecurityDevice(uint8_t endpoint)
{
  uint8_t ieeeAddress[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  uint16_t zoneType = EMBER_AF_PLUGIN_IAS_ZONE_SERVER_ZONE_TYPE;

  emberAfWriteServerAttribute(endpoint,
                              ZCL_IAS_ZONE_CLUSTER_ID,
                              ZCL_IAS_CIE_ADDRESS_ATTRIBUTE_ID,
                              (uint8_t*)ieeeAddress,
                              ZCL_IEEE_ADDRESS_ATTRIBUTE_TYPE);

  emberAfWriteServerAttribute(endpoint,
                              ZCL_IAS_ZONE_CLUSTER_ID,
                              ZCL_ZONE_TYPE_ATTRIBUTE_ID,
                              (uint8_t*)&zoneType,
                              ZCL_INT16U_ATTRIBUTE_TYPE);

  setZoneId(endpoint, UNDEFINED_ZONE_ID);
  // Restore the enrollment method back to its default value.
  emberAfPluginIasZoneClusterSetEnrollmentMethod(endpoint,
                                                 DEFAULT_ENROLLMENT_METHOD);
  updateEnrollState(endpoint, false); // enrolled?
}

// If you leave the network, unenroll yourself.
void emberAfPluginIasZoneServerStackStatusCallback(EmberStatus status)
{
  uint8_t endpoint;
  uint8_t networkIndex;
  uint8_t i;

  // If the device has left the network, unenroll all endpoints on the device
  // that are servers of the IAS Zone Cluster
  if (status == EMBER_NETWORK_DOWN
      && emberAfNetworkState() == EMBER_NO_NETWORK) {
    for (i = 0; i < emberAfEndpointCount(); i++) {
      endpoint = emberAfEndpointFromIndex(i);
      networkIndex = emberAfNetworkIndexFromEndpointIndex(i);
      if (networkIndex == emberGetCurrentNetwork()
          && emberAfContainsServer(endpoint, ZCL_IAS_ZONE_CLUSTER_ID)) {
        unenrollSecurityDevice(endpoint);
      }
    }
  } else if (status == EMBER_NETWORK_UP) {
#if defined(ENABLE_QUEUE)
    // If we're reconnecting, send any items still in the queue
    emberAfIasZoneClusterPrintln(
      "Rejoined network, retransmiting any queued event");
    slxu_zigbee_event_set_active(serverManageQueueEventControl);
#endif // ENABLE_QUEUE
  }
}

#if defined(ENABLE_QUEUE)
EmberStatus emberAfIasZoneServerConfigStatusQueueRetryParams(
  IasZoneStatusQueueRetryConfig *retryConfig)
{
  if (!(retryConfig->firstBackoffTimeSec)
      || (!retryConfig->backoffSeqCommonRatio)
      || (retryConfig->maxBackoffTimeSec < retryConfig->firstBackoffTimeSec)
      || (retryConfig->maxBackoffTimeSec > IAS_ZONE_STATUS_QUEUE_RETRY_ABS_MAX_BACKOFF_TIME_SEC)
      || (!retryConfig->maxRetryAttempts) ) {
    return EMBER_BAD_ARGUMENT;
  }

  queueRetryParams.config.firstBackoffTimeSec = retryConfig->firstBackoffTimeSec;
  queueRetryParams.config.backoffSeqCommonRatio = retryConfig->backoffSeqCommonRatio;
  queueRetryParams.config.maxBackoffTimeSec = retryConfig->maxBackoffTimeSec;
  queueRetryParams.config.unlimitedRetries = retryConfig->unlimitedRetries;
  queueRetryParams.config.maxRetryAttempts = retryConfig->maxRetryAttempts;

  queueRetryParams.currentBackoffTimeSec = retryConfig->firstBackoffTimeSec;
  queueRetryParams.currentRetryCount = 0;

  return EMBER_SUCCESS;
}

void emberAfIasZoneServerSetStatusQueueRetryParamsToDefault(void)
{
  queueRetryParams.config.firstBackoffTimeSec =
    EMBER_AF_PLUGIN_IAS_ZONE_SERVER_FIRST_BACKOFF_TIME_SEC;
  queueRetryParams.config.backoffSeqCommonRatio =
    EMBER_AF_PLUGIN_IAS_ZONE_SERVER_BACKOFF_SEQUENCE_COMMON_RATIO;
  queueRetryParams.config.maxBackoffTimeSec =
    EMBER_AF_PLUGIN_IAS_ZONE_SERVER_MAX_BACKOFF_TIME_SEC;
#ifdef UNLIMITED_RETRIES
  queueRetryParams.config.unlimitedRetries = true;
#else // !UNLIMITED_RETRIES
  queueRetryParams.config.unlimitedRetries = false;
#endif // UNLIMITED_RETRIES
  queueRetryParams.config.maxRetryAttempts =
    EMBER_AF_PLUGIN_IAS_ZONE_SERVER_MAX_RETRY_ATTEMPTS;

  queueRetryParams.currentBackoffTimeSec =
    EMBER_AF_PLUGIN_IAS_ZONE_SERVER_FIRST_BACKOFF_TIME_SEC;
  queueRetryParams.currentRetryCount = 0;
}

void emberAfIasZoneServerDiscardPendingEventsInStatusQueue(void)
{
  slxu_zigbee_event_set_inactive(serverManageQueueEventControl);
  bufferInit(&messageQueue);
  resetCurrentQueueRetryParams();
}

#if defined(SL_CATALOG_ZIGBEE_WWAH_APP_EVENT_RETRY_MANAGER_PRESENT)
EmberStatus emberAfWwahAppEventRetryManagerConfigBackoffParamsCallback(uint8_t firstBackoffTimeSeconds,
                                                                       uint8_t backoffSeqCommonRatio,
                                                                       uint32_t maxBackoffTimeSeconds,
                                                                       uint8_t maxRedeliveryAttempts)
{
  IasZoneStatusQueueRetryConfig retryConfig = {
    firstBackoffTimeSeconds,
    backoffSeqCommonRatio,
    maxBackoffTimeSeconds,
    (maxRedeliveryAttempts == 0xFF),
    maxRedeliveryAttempts
  };

  // Setting up retry parameters
  return emberAfIasZoneServerConfigStatusQueueRetryParams(&retryConfig);
}

void emberAfWwahAppEventRetryManagerSetBackoffParamsToDefault(void)
{
  emberAfIasZoneServerSetStatusQueueRetryParamsToDefault();
}
#endif // WWAH_APP_EVENT_RETRY_MANAGER_PRESENT

void emberAfPluginIasZoneServerPrintQueue(void)
{
  emberAfIasZoneClusterPrintln("%d/%d entries", messageQueue.entriesInQueue, NUM_QUEUE_ENTRIES);
  for (int i = 0; i < messageQueue.entriesInQueue; i++) {
    emberAfIasZoneClusterPrintln("Entry %d: Endpoint: %d Status: %d EventTimeMs: %d",
                                 i,
                                 messageQueue.buffer[i].endpoint,
                                 messageQueue.buffer[i].status,
                                 messageQueue.buffer[i].eventTimeMs
                                 );
  }
}

void emberAfPluginIasZoneServerPrintQueueConfig(void)
{
  emberAfCorePrintln("First backoff time (sec): %d", queueRetryParams.config.firstBackoffTimeSec);
  emberAfCorePrintln("Backoff sequence common ratio: %d", queueRetryParams.config.backoffSeqCommonRatio);
  emberAfCorePrintln("Max backoff time (sec): %d", queueRetryParams.config.maxBackoffTimeSec);
  emberAfCorePrintln("Max redelivery attempts: %d", queueRetryParams.config.maxRetryAttempts);
}

#endif // ENABLE_QUEUE

// This callback will be generated any time the node receives an ACK or a NAK
// for a message transmitted for the IAS Zone Cluster Server.  Note that this
// will not be called in the case that the message was not delivered to the
// destination when the destination is the only router the node is joined to.
// In that case, the command will never have been sent, as the device will have
// had no router by which to send the command.
void emberAfIasZoneClusterServerMessageSentCallback(
  EmberOutgoingMessageType type,
  int16u indexOrDestination,
  EmberApsFrame *apsFrame,
  int16u msgLen,
  int8u *message,
  EmberStatus status)
{
#if defined(ENABLE_QUEUE)
  uint8_t frameControl;
  uint8_t commandId;

  IasZoneStatusQueueEntry dummyEntry;

  // Verify that this response is for a ZoneStatusChangeNotification command
  // by checking the message length, the command and direction bits of the
  // Frame Control byte, and the command ID
  if (msgLen < IAS_ZONE_SERVER_PAYLOAD_COMMAND_IDX) {
    return;
  }

  frameControl = message[ZCL_FRAME_CONTROL_IDX];
  if (!(frameControl & ZCL_CLUSTER_SPECIFIC_COMMAND)
      || !(frameControl & ZCL_FRAME_CONTROL_SERVER_TO_CLIENT)) {
    return;
  }

  commandId = message[IAS_ZONE_SERVER_PAYLOAD_COMMAND_IDX];
  if (commandId != ZCL_ZONE_STATUS_CHANGE_NOTIFICATION_COMMAND_ID) {
    return;
  }

  // If a change status change notification command is not received by the
  // client, delay the option specified amount of time and try to resend it.
  // The event handler will perform the retransmit per the preset queue retry
  // parameteres, and the original send request will handle populating the buffer.
  // Do not try to retransmit again if the maximum number of retries attempts
  // is reached, this is however discarded if configured for unlimited retries.
  if ((status == EMBER_DELIVERY_FAILED)
      && (queueRetryParams.config.unlimitedRetries
          || (queueRetryParams.currentRetryCount < queueRetryParams.config.maxRetryAttempts))) {
    queueRetryParams.currentRetryCount++;

    emberAfIasZoneClusterPrintln(
      "Status command update failed to send... Retrying in %d seconds...",
      queueRetryParams.currentBackoffTimeSec);

    // Delay according to the current retransmit backoff time.
    slxu_zigbee_event_set_delay_ms(serverManageQueueEventControl,
                                   queueRetryParams.currentBackoffTimeSec * MILLISECOND_TICKS_PER_SECOND);

    // The backoff time needs to be increased if the maximum backoff time is not reached yet.
    if ((queueRetryParams.currentBackoffTimeSec * queueRetryParams.config.backoffSeqCommonRatio)
        <= queueRetryParams.config.maxBackoffTimeSec) {
      queueRetryParams.currentBackoffTimeSec *= queueRetryParams.config.backoffSeqCommonRatio;
    }
  } else {
    // If a command message was sent or max redelivery attempts were reached,
    // remove it from the queue and move on to the next queued message until the queue is empty.
    if (status == EMBER_SUCCESS) {
      emberAfIasZoneClusterPrintln(
        "\nZone update successful, remove entry from queue");
    } else {
      emberAfIasZoneClusterPrintln(
        "\nZone update unsuccessful, max retry attempts reached, remove entry from queue");
    }
    popFromBuffer(&messageQueue, &dummyEntry);

    // Reset queue retry parameters.
    resetCurrentQueueRetryParams();

    if (messageQueue.entriesInQueue) {
      slxu_zigbee_event_set_active(serverManageQueueEventControl);
    }
  }
#endif // ENABLE_QUEUE
}

#if defined(ENABLE_QUEUE)
static void bufferInit(IasZoneStatusQueue *ring)
{
  ring->entriesInQueue = 0;
  ring->startIdx = 0;
  ring->lastIdx = NUM_QUEUE_ENTRIES - 1;
}

// Add the entry to the buffer by copying, returning the index at which it was
// added.  If the buffer is full, return -1, but still copy the entry over the
// last item of the buffer, to ensure that the last item in the buffer is
// always representative of the last known device state.
static int16_t copyToBuffer(IasZoneStatusQueue *ring,
                            const IasZoneStatusQueueEntry *entry)
{
  if (ring->entriesInQueue == NUM_QUEUE_ENTRIES) {
    ring->buffer[ring->lastIdx] = *entry;
    return -1;
  }

  // Increment the last pointer.  If it rolls over the size, circle it back to
  // zero.
  ring->lastIdx++;
  if (ring->lastIdx >= NUM_QUEUE_ENTRIES) {
    ring->lastIdx = 0;
  }

  ring->buffer[ring->lastIdx].endpoint = entry->endpoint;
  ring->buffer[ring->lastIdx].status = entry->status;
  ring->buffer[ring->lastIdx].eventTimeMs = entry->eventTimeMs;

  ring->entriesInQueue++;
  return ring->lastIdx;
}

// Return the idx of the popped entry, or -1 if the buffer was empty.
static int16_t popFromBuffer(IasZoneStatusQueue *ring,
                             IasZoneStatusQueueEntry *entry)
{
  int16_t retVal;

  if (ring->entriesInQueue == 0) {
    return -1;
  }

  // Copy out the first entry, then increment the start pointer.  If it rolls
  // over, circle it back to zero.
  *entry = ring->buffer[ring->startIdx];
  retVal = ring->startIdx;

  ring->startIdx++;
  if (ring->startIdx >= NUM_QUEUE_ENTRIES) {
    ring->startIdx = 0;
  }

  ring->entriesInQueue--;

  return retVal;
}

uint16_t computeElapsedTimeQs(IasZoneStatusQueueEntry *entry)
{
  uint32_t currentTimeMs = halCommonGetInt32uMillisecondTick();
  int64_t deltaTimeMs = currentTimeMs - entry->eventTimeMs;

  if (deltaTimeMs < 0) {
    deltaTimeMs = -deltaTimeMs + (0xFFFFFFFF - currentTimeMs);
  }

  return deltaTimeMs / MILLISECOND_TICKS_PER_QUARTERSECOND;
}
#endif // ENABLE_QUEUE

#ifdef UC_BUILD

uint32_t emberAfIasZoneClusterServerCommandParse(sl_service_opcode_t opcode,
                                                 sl_service_function_context_t *context)
{
  (void)opcode;

  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_ZONE_ENROLL_RESPONSE_COMMAND_ID:
      {
        wasHandled = emberAfIasZoneClusterZoneEnrollResponseCallback(cmd);
        break;
      }
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
