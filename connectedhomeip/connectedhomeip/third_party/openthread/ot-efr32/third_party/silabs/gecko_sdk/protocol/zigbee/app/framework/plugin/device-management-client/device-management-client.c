/***************************************************************************//**
 * @file
 * @brief Routines for the Device Management Client plugin, which is based on
 *        the SE 1.2 cluster.
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
#include "app/framework/plugin/device-management-server/device-management-common.h"
#include "device-management-client.h"

#ifdef UC_BUILD
#include "sl_component_catalog.h"
#include "zap-cluster-command-parser.h"
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER
#define SL_CATALOG_ZIGBEE_SIMPLE_METERING_SERVER_PRESENT
#endif
#endif // UC_BUILD

static EmberAfDeviceManagementInfo pmInfo;
static EmberAfDeviceManagementPassword servicePassword;
static EmberAfDeviceManagementPassword consumerPassword;

static uint8_t numberOfAttributeSets = 9;
#define isValidAttributeTableIndex(index) ((index) < (numberOfAttributeSets))
static EmberAfDeviceManagementAttributeTable attributeTable[9] = {
  {
    0x00,
    {
      { 0x00, 0x01 }, //min inclusive, max not included
      { 0x10, 0x11 },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
    },
  },
  {
    0x01,
    {
      { 0x00, 0x0E },
      { 0xB0, 0xE8 },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
    },
  },
  {
    0x02,
    {
      { 0x00, 0x2A },
      { 0x30, 0x38 },
      { 0x50, 0x52 },
      { 0x60, 0x66 },
      { 0x70, 0xA0 },
      { 0xB0, 0xB9 },
      { 0xC0, 0xFA },
    }
  },
  {
    0x03,
    {
      { 0x00, 0x01 },
      { 0xC0, 0xC8 },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
    }
  },
  {
    0x04,
    {
      { 0x00, 0x06 },
      { 0x20, 0x34 },
      { 0x41, 0x44 },
      { 0xC0, 0xF0 },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
    }
  },
  {
    0x05,
    {
      { 0x00, 0x03 },
      { 0xC0, 0xD4 },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
    }
  },
  {
    0x06,
    {
      { 0x00, 0x05 },
      { 0x10, 0x16 },
      { 0xC0, 0xD8 },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
    }
  },
  {
    0x07,
    {
      { 0x00, 0x03 },
      { 0xC0, 0xD5 },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
    }
  },
  {
    0x08,
    {
      { 0x00, 0x06 },
      { 0xC0, 0xCF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
      { 0xFF, 0xFF },
    }
  }
};

static void emberAfDeviceManagementClusterPrintPendingStatus(EmberAfDeviceManagementChangePendingFlags mask)
{
  if (pmInfo.pendingUpdates & mask) {
    emberAfDeviceManagementClusterPrintln("  Status: pending");
  } else {
    emberAfDeviceManagementClusterPrintln("  Status: not pending");
  }
}

static void sendDeviceManagementClusterReportWildCardAttribute(uint8_t attributeSet,
                                                               uint8_t endpoint)
{
  uint8_t attrSet, j, k;
  uint8_t eventConfiguration;
  EmberAfStatus status;
  bool atLeastOneEvent = false;
  uint8_t numberOfAttrSetsToReport = 1; // allow one iteratoion

  if (attributeSet == 0xFF) {
    numberOfAttrSetsToReport = numberOfAttributeSets;
  }

  (void) emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                                    | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER),
                                   ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                   ZCL_REPORT_EVENT_CONFIGURATION_COMMAND_ID,
                                   "uu",
                                   0,
                                   1);
// The maximum number of intervals possible in attributeRange.
  for (attrSet = 0; attrSet < numberOfAttrSetsToReport; attrSet++) {
    for (j = 0; j < 7; j++) {
      //min and max for each interval.
      uint8_t actualSet = (!isValidAttributeTableIndex(attributeSet)) ? attrSet : attributeSet;
      for (k = attributeTable[actualSet].attributeRange[j].startIndex; k < attributeTable[actualSet].attributeRange[j].endIndex; k++) {
        uint8_t topByte = (attributeSet == 0xFF) ? attributeTable[attrSet].attributeSetId : attributeSet;
        uint16_t attributeId = ((topByte << 8) | k);
        if (emberAfLocateAttributeMetadata(endpoint,
                                           ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                           attributeId,
                                           CLUSTER_MASK_CLIENT,
                                           EMBER_AF_NULL_MANUFACTURER_CODE)) {
          status = emberAfReadClientAttribute(endpoint,
                                              ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                              attributeId,
                                              (uint8_t *)&eventConfiguration,
                                              sizeof(eventConfiguration));

          if (status == EMBER_ZCL_STATUS_SUCCESS) {
            //emberAfDeviceManagementClusterPrintln("Attributed Id 0x%2x",attributeId);
            atLeastOneEvent = true;
            (void) emberAfPutInt16uInResp(attributeId);
            (void) emberAfPutInt8uInResp(eventConfiguration);
          }
        }
      }
    }
  }

  if (atLeastOneEvent) {
    emberAfSendResponse();
  } else {
    status = EMBER_ZCL_STATUS_NOT_FOUND;
    emberAfDeviceManagementClusterPrintln("sending default response");
    emberAfSendImmediateDefaultResponse(status);
  }
}

static void writeDeviceManagementClusterWildCardAttribute(uint8_t attributeSet,
                                                          uint8_t endpoint,
                                                          uint8_t attributeConfiguration)
{
  uint8_t attrRange, attrId;

  // CSTAT needs assurance that attributeSet won't exceed array bounds.
  if (!isValidAttributeTableIndex(attributeSet)) {
    return;
  }

  for (attrRange = 0; attrRange < 7; attrRange++) {
    for (attrId = attributeTable[attributeSet].attributeRange[attrRange].startIndex; attrId < attributeTable[attributeSet].attributeRange[attrRange].endIndex; attrId++) {
      uint16_t attributeId = ((attributeSet << 8) | attrId);
      if (emberAfLocateAttributeMetadata(endpoint,
                                         ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                         attributeId,
                                         CLUSTER_MASK_CLIENT,
                                         EMBER_AF_NULL_MANUFACTURER_CODE)) {
        (void)emberAfWriteAttribute(endpoint,
                                    ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                    attributeId,
                                    CLUSTER_MASK_CLIENT,
                                    (uint8_t*)&attributeConfiguration,
                                    ZCL_BITMAP8_ATTRIBUTE_TYPE);
      }
    }
  }
}

static void writeDeviceManagementClusterByLogTypeAttribute(uint8_t logType,
                                                           uint8_t endpoint,
                                                           uint8_t attributeConfiguration)
{
  uint8_t i, j, k;
  uint8_t eventConfiguration;
  EmberAfStatus status;

  for (i = 0; i < numberOfAttributeSets; i++) {
    for (j = 0; j < 7; j++) {
      for (k = attributeTable[i].attributeRange[j].startIndex; k < attributeTable[i].attributeRange[j].endIndex; k++) {
        uint8_t topByte = attributeTable[i].attributeSetId;
        uint16_t attributeId = ((topByte << 8) | k);
        if (emberAfLocateAttributeMetadata(endpoint,
                                           ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                           attributeId,
                                           CLUSTER_MASK_CLIENT,
                                           EMBER_AF_NULL_MANUFACTURER_CODE)) {
          status = emberAfReadClientAttribute(endpoint,
                                              ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                              attributeId,
                                              (uint8_t *)&eventConfiguration,
                                              sizeof(eventConfiguration));
          if (status == EMBER_ZCL_STATUS_SUCCESS) {
            if ((eventConfiguration & 0x03) == logType) {
              EmberAfStatus status  = emberAfWriteAttribute(endpoint,
                                                            ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                                            attributeId,
                                                            CLUSTER_MASK_CLIENT,
                                                            (uint8_t*)&attributeConfiguration,
                                                            ZCL_BITMAP8_ATTRIBUTE_TYPE);
              if (status == EMBER_ZCL_STATUS_SUCCESS) {
                emberAfDeviceManagementClusterPrintln("Writing %x to 0x%2x", attributeConfiguration, attributeId);
              }
            }
          }
        }
      }
    }
  }
}

static void writeDeviceManagementClusterByMatchingAttribute(uint8_t currentConfiguration,
                                                            uint8_t endpoint,
                                                            uint8_t attributeConfiguration)
{
  uint8_t i, j, k;
  uint8_t eventConfiguration;
  EmberAfStatus status;

  for (i = 0; i < numberOfAttributeSets; i++) {
    for (j = 0; j < 7; j++) {
      for (k = attributeTable[i].attributeRange[j].startIndex; k < attributeTable[i].attributeRange[j].endIndex; k++) {
        uint8_t topByte = attributeTable[i].attributeSetId;
        uint16_t attributeId = ((topByte << 8) | k);
        if (emberAfLocateAttributeMetadata(endpoint,
                                           ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                           attributeId,
                                           CLUSTER_MASK_CLIENT,
                                           EMBER_AF_NULL_MANUFACTURER_CODE)) {
          status = emberAfReadClientAttribute(endpoint,
                                              ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                              attributeId,
                                              (uint8_t *)&eventConfiguration,
                                              sizeof(eventConfiguration));
          if (status == EMBER_ZCL_STATUS_SUCCESS) {
            if (eventConfiguration == currentConfiguration) {
              EmberAfStatus status  = emberAfWriteAttribute(endpoint,
                                                            ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                                            attributeId,
                                                            CLUSTER_MASK_CLIENT,
                                                            (uint8_t*)&attributeConfiguration,
                                                            ZCL_BITMAP8_ATTRIBUTE_TYPE);
              if (status == EMBER_ZCL_STATUS_SUCCESS) {
                emberAfDeviceManagementClusterPrintln("Writing %x to 0x%2x", attributeConfiguration, attributeId);
              }
            }
          }
        }
      }
    }
  }
}

static void writeDeviceManagementAttribute(uint16_t attributeId,
                                           uint8_t attributeConfiguration)
{
  uint8_t endpoint = emberAfCurrentEndpoint();
  EmberAfStatus status  = emberAfWriteAttribute(endpoint,
                                                ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                                attributeId,
                                                CLUSTER_MASK_CLIENT,
                                                (uint8_t*)&attributeConfiguration,
                                                ZCL_BITMAP8_ATTRIBUTE_TYPE);
  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    emberAfDeviceManagementClusterPrintln("Wrote %u to %u", attributeConfiguration, attributeId);
  } else {
    emberAfDeviceManagementClusterPrintln("couldn't write %u to %u", attributeConfiguration, attributeId);
  }
}

void emberAfDeviceManagementClusterClientInitCallback(uint8_t endpoint)
{
  pmInfo.providerId = 0;
  pmInfo.issuerEventId = 0;
  pmInfo.tariffType = (EmberAfTariffType) 0xFF;
  pmInfo.pendingUpdates = 0;
  attributeTable[0].attributeSetId = 0x01;
  // MEMSET(&servicePassword, 0x00, sizeof(servicePassword));
  // MEMSET(&consumerPassword, 0x00, sizeof(consumerPassword));
}

void emberAfDeviceManagementClusterClientTickCallback(uint8_t endpoint)
{
  uint32_t currentTime = emberAfGetCurrentTime();
  uint32_t nextTick = currentTime;

  // Action pending changes
  if (pmInfo.pendingUpdates & EMBER_AF_DEVICE_MANAGEMENT_CHANGE_OF_TENANCY_PENDING_MASK) {
    if (currentTime >= pmInfo.tenancy.implementationDateTime) {
      emberAfDeviceManagementClusterPrintln("DEVICE MANAGEMENT CLIENT: Enacting change of tenancy at time 0x%4x\n",
                                            currentTime);
      emberAfPluginDeviceManagementClientEnactChangeOfTenancyCallback(endpoint,
                                                                      &(pmInfo.tenancy));
      pmInfo.pendingUpdates &= ~(EMBER_AF_DEVICE_MANAGEMENT_CHANGE_OF_TENANCY_PENDING_MASK);
    } else if (pmInfo.tenancy.implementationDateTime - currentTime < nextTick) {
      nextTick = pmInfo.tenancy.implementationDateTime - currentTime;
    }
  }

  if (pmInfo.pendingUpdates & EMBER_AF_DEVICE_MANAGEMENT_CHANGE_OF_SUPPLIER_PENDING_MASK) {
    if (currentTime >= pmInfo.supplier.implementationDateTime) {
      emberAfDeviceManagementClusterPrintln("DEVICE MANAGEMENT CLIENT: Enacting change of supplier at time 0x%4x\n",
                                            currentTime);
      emberAfPluginDeviceManagementClientEnactChangeOfSupplierCallback(endpoint,
                                                                       &(pmInfo.supplier));
      pmInfo.pendingUpdates &= ~(EMBER_AF_DEVICE_MANAGEMENT_CHANGE_OF_SUPPLIER_PENDING_MASK);
    } else if (pmInfo.supplier.implementationDateTime - currentTime < nextTick) {
      nextTick = pmInfo.supplier.implementationDateTime - currentTime;
    }
  }

  if (pmInfo.pendingUpdates & EMBER_AF_DEVICE_MANAGEMENT_UPDATE_SITE_ID_PENDING_MASK) {
    if (currentTime >= pmInfo.siteId.implementationDateTime) {
      EmberAfStatus status = emberAfWriteAttribute(endpoint,
                                                   ZCL_SIMPLE_METERING_CLUSTER_ID,
                                                   ZCL_SITE_ID_ATTRIBUTE_ID,
                                                   CLUSTER_MASK_SERVER,
                                                   (uint8_t*)&(pmInfo.siteId.siteId),
                                                   ZCL_OCTET_STRING_ATTRIBUTE_TYPE);
      if (status == EMBER_ZCL_STATUS_SUCCESS) {
        pmInfo.pendingUpdates &= ~(EMBER_AF_DEVICE_MANAGEMENT_UPDATE_SITE_ID_PENDING_MASK);
      }
      emberAfDeviceManagementClusterPrintln("DEVICE MANAGEMENT CLIENT: Enacting site id update at time 0x%4x: %d\n",
                                            currentTime,
                                            status);
    } else if (pmInfo.siteId.implementationDateTime - currentTime < nextTick) {
      nextTick = pmInfo.siteId.implementationDateTime - currentTime;
    }
  }

  if (pmInfo.pendingUpdates & EMBER_AF_DEVICE_MANAGEMENT_UPDATE_CIN_PENDING_MASK) {
    if (currentTime >= pmInfo.cin.implementationDateTime) {
      EmberAfStatus status = emberAfWriteAttribute(endpoint,
                                                   ZCL_SIMPLE_METERING_CLUSTER_ID,
                                                   ZCL_CUSTOMER_ID_NUMBER_ATTRIBUTE_ID,
                                                   CLUSTER_MASK_SERVER,
                                                   (uint8_t*)&(pmInfo.cin.cin),
                                                   ZCL_OCTET_STRING_ATTRIBUTE_TYPE);
      if (status == EMBER_ZCL_STATUS_SUCCESS) {
        pmInfo.pendingUpdates &= ~(EMBER_AF_DEVICE_MANAGEMENT_UPDATE_CIN_PENDING_MASK);
      }
      emberAfDeviceManagementClusterPrintln("DEVICE MANAGEMENT CLIENT: Enacting customer id number update at time 0x%4x: %d\n",
                                            currentTime,
                                            status);
    } else if (pmInfo.cin.implementationDateTime - currentTime < nextTick) {
      nextTick = pmInfo.cin.implementationDateTime - currentTime;
    }
  }

  if (pmInfo.pendingUpdates & EMBER_AF_DEVICE_MANAGEMENT_UPDATE_SERVICE_PASSWORD_PENDING_MASK) {
    emberAfDeviceManagementClusterPrintln("DEVICE MANAGEMENT CLIENT: Service password current time 0x%4x: implementationTime 0x%4x\n",
                                          currentTime,
                                          servicePassword.implementationDateTime);
    if (currentTime >= servicePassword.implementationDateTime) {
      pmInfo.servicePassword.implementationDateTime = servicePassword.implementationDateTime;
      pmInfo.servicePassword.durationInMinutes = servicePassword.durationInMinutes;
      pmInfo.servicePassword.passwordType = servicePassword.passwordType;
      MEMCOPY(&pmInfo.servicePassword.password, &servicePassword.password, emberAfStringLength(servicePassword.password) + 1);
      pmInfo.pendingUpdates &= (~EMBER_AF_DEVICE_MANAGEMENT_UPDATE_SERVICE_PASSWORD_PENDING_MASK);
    } else if (servicePassword.implementationDateTime - currentTime < nextTick) {
      nextTick = servicePassword.implementationDateTime - currentTime;
    }
  }

  if (pmInfo.pendingUpdates & EMBER_AF_DEVICE_MANAGEMENT_UPDATE_CONSUMER_PASSWORD_PENDING_MASK) {
    if (currentTime >= consumerPassword.implementationDateTime) {
      pmInfo.consumerPassword.implementationDateTime = consumerPassword.implementationDateTime;
      pmInfo.consumerPassword.durationInMinutes = consumerPassword.durationInMinutes;
      pmInfo.consumerPassword.passwordType = consumerPassword.passwordType;
      MEMCOPY(&pmInfo.consumerPassword.password, &consumerPassword.password, emberAfStringLength(consumerPassword.password) + 1);
      pmInfo.pendingUpdates &= (~EMBER_AF_DEVICE_MANAGEMENT_UPDATE_CONSUMER_PASSWORD_PENDING_MASK);
    } else if (consumerPassword.implementationDateTime - currentTime < nextTick) {
      nextTick = consumerPassword.implementationDateTime - currentTime;
    }
  }

  // Reschedule the next tick, if necessary
  if (pmInfo.pendingUpdates != 0) {
    slxu_zigbee_zcl_schedule_client_tick(endpoint,
                                         ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                         nextTick * MILLISECOND_TICKS_PER_SECOND);
  }
}

#ifdef UC_BUILD

bool emberAfDeviceManagementClusterPublishChangeOfTenancyCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_device_management_cluster_publish_change_of_tenancy_command_t cmd_data;
  uint32_t currentTime = emberAfGetCurrentTime();
  uint8_t endpoint = emberAfCurrentEndpoint();
  EmberAfStatus status;

  if (zcl_decode_device_management_cluster_publish_change_of_tenancy_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfDeviceManagementClusterPrintln("RX: PublishChangeOfTenancy: 0x%4X, 0x%4X, 0x%X, 0x%4X, 0x%4X",
                                        cmd_data.providerId,
                                        cmd_data.issuerEventId,
                                        cmd_data.tariffType,
                                        cmd_data.implementationDateTime,
                                        cmd_data.proposedTenancyChangeControl);

  if (pmInfo.providerId == 0) {
    pmInfo.providerId = cmd_data.providerId;
  }

  if (cmd_data.issuerEventId > pmInfo.issuerEventId) {
    pmInfo.issuerEventId = cmd_data.issuerEventId;
  }

  pmInfo.tariffType = (EmberAfTariffType)cmd_data.tariffType;

  pmInfo.tenancy.implementationDateTime = cmd_data.implementationDateTime;
  pmInfo.tenancy.tenancy = cmd_data.proposedTenancyChangeControl;

  // Even if we aren't to immediately action the change of tenancy,
  // we still set these attributes accordingly
  status = emberAfWriteAttribute(endpoint,
                                 ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                 ZCL_CHANGE_OF_TENANCY_UPDATE_DATE_TIME_ATTRIBUTE_ID,
                                 CLUSTER_MASK_CLIENT,
                                 (uint8_t*)&cmd_data.implementationDateTime,
                                 ZCL_UTC_TIME_ATTRIBUTE_TYPE);

  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto kickout;
  }

  status = emberAfWriteAttribute(endpoint,
                                 ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                 ZCL_PROPOSED_TENANCY_CHANGE_CONTROL_ATTRIBUTE_ID,
                                 CLUSTER_MASK_CLIENT,
                                 (uint8_t*)&cmd_data.proposedTenancyChangeControl,
                                 ZCL_INT32U_ATTRIBUTE_TYPE);

  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto kickout;
  }

  // If the time has passed since the change of tenancy was to be implemented, take action
  if (currentTime >= cmd_data.implementationDateTime) {
    emberAfPluginDeviceManagementClientEnactChangeOfTenancyCallback(endpoint,
                                                                    &(pmInfo.tenancy));
    pmInfo.pendingUpdates &= ~EMBER_AF_DEVICE_MANAGEMENT_CHANGE_OF_TENANCY_PENDING_MASK;
  } else {
    // Otherwise, wait until the time of implementation
    slxu_zigbee_zcl_schedule_client_tick(endpoint,
                                         ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                         (cmd_data.implementationDateTime - currentTime)
                                         * MILLISECOND_TICKS_PER_SECOND);
    pmInfo.pendingUpdates |= EMBER_AF_DEVICE_MANAGEMENT_CHANGE_OF_TENANCY_PENDING_MASK;
  }

  kickout:
  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfDeviceManagementClusterPublishChangeOfSupplierCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_device_management_cluster_publish_change_of_supplier_command_t cmd_data;
  EmberAfStatus status = EMBER_ZCL_STATUS_SUCCESS;
  uint32_t currentTime = emberAfGetCurrentTime();
  uint8_t endpoint = emberAfCurrentEndpoint();

  if (zcl_decode_device_management_cluster_publish_change_of_supplier_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfDeviceManagementClusterPrintln("RX: PublishChangeOfSupplier: 0x%4X, 0x%4X, 0x%X, 0x%4X, 0x%4X, 0x%4X, ",
                                        cmd_data.currentProviderId,
                                        cmd_data.issuerEventId,
                                        cmd_data.tariffType,
                                        cmd_data.proposedProviderId,
                                        cmd_data.providerChangeImplementationTime,
                                        cmd_data.providerChangeControl);
  emberAfDeviceManagementClusterPrintString(cmd_data.proposedProviderName);
  emberAfDeviceManagementClusterPrintln(", ");
  emberAfDeviceManagementClusterPrintString(cmd_data.proposedProviderContactDetails);
  emberAfDeviceManagementClusterPrintln("");

  if (cmd_data.proposedProviderName == NULL) {
    status = EMBER_ZCL_STATUS_INVALID_VALUE;
    goto kickout;
  }

  if (pmInfo.providerId == 0) {
    pmInfo.providerId = cmd_data.currentProviderId;
  }

  if (pmInfo.issuerEventId == 0) {
    pmInfo.issuerEventId = cmd_data.issuerEventId;
  }

  pmInfo.tariffType = (EmberAfTariffType)cmd_data.tariffType;

  pmInfo.supplier.proposedProviderId = cmd_data.proposedProviderId;
  pmInfo.supplier.implementationDateTime = cmd_data.providerChangeImplementationTime;
  pmInfo.supplier.providerChangeControl = cmd_data.providerChangeControl;
  emberAfCopyString(pmInfo.supplier.proposedProviderName,
                    cmd_data.proposedProviderName,
                    EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_PROPOSED_PROVIDER_NAME_LENGTH);
  emberAfCopyString(pmInfo.supplier.proposedProviderContactDetails,
                    cmd_data.proposedProviderContactDetails,
                    EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_PROPOSED_PROVIDER_CONTACT_DETAILS_LENGTH);

  // If the time has passed since the change of supplier was to be implemented, take action
  if (currentTime >= cmd_data.providerChangeImplementationTime) {
    emberAfPluginDeviceManagementClientEnactChangeOfSupplierCallback(endpoint,
                                                                     &(pmInfo.supplier));
    pmInfo.pendingUpdates &= ~EMBER_AF_DEVICE_MANAGEMENT_CHANGE_OF_SUPPLIER_PENDING_MASK;
  } else {
    // Otherwise, wait until the time of implementation
    slxu_zigbee_zcl_schedule_client_tick(endpoint,
                                         ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                         (cmd_data.providerChangeImplementationTime - currentTime)
                                         * MILLISECOND_TICKS_PER_SECOND);
    pmInfo.pendingUpdates |= EMBER_AF_DEVICE_MANAGEMENT_CHANGE_OF_SUPPLIER_PENDING_MASK;
  }

  kickout:
  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfDeviceManagementClusterRequestNewPasswordResponseCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_device_management_cluster_request_new_password_response_command_t cmd_data;
  uint32_t currentTime = emberAfGetCurrentTime();
  uint8_t endpoint = emberAfCurrentEndpoint();
  EmberAfDeviceManagementPassword *pass;
  EmberAfStatus status = EMBER_ZCL_STATUS_SUCCESS;

  if (zcl_decode_device_management_cluster_request_new_password_response_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  if (cmd_data.password == NULL) {
    status = EMBER_ZCL_STATUS_INVALID_VALUE;
    goto kickout;
  }

  if (cmd_data.issuerEventId > pmInfo.issuerEventId) {
    pmInfo.issuerEventId = cmd_data.issuerEventId;
  }

  if (cmd_data.implementationDateTime < currentTime && cmd_data.implementationDateTime != 0x00000000) {
    goto kickout;
  }

  if (cmd_data.passwordType & SERVICE_PASSWORD) {
    if (currentTime == cmd_data.implementationDateTime || cmd_data.implementationDateTime == 0x00000000) {
      pass = &(pmInfo.servicePassword);
    } else {
      pass = &(servicePassword);
      pmInfo.pendingUpdates |= EMBER_AF_DEVICE_MANAGEMENT_UPDATE_SERVICE_PASSWORD_PENDING_MASK;
      slxu_zigbee_zcl_schedule_client_tick(endpoint,
                                           ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                           (cmd_data.implementationDateTime - currentTime)
                                           * MILLISECOND_TICKS_PER_SECOND);
    }
  } else if (cmd_data.passwordType & CONSUMER_PASSWORD) {
    if (currentTime == cmd_data.implementationDateTime || cmd_data.implementationDateTime == (0x00000000)) {
      pass = &(pmInfo.consumerPassword);
    } else {
      pass = &(consumerPassword);
      pmInfo.pendingUpdates |= EMBER_AF_DEVICE_MANAGEMENT_UPDATE_CONSUMER_PASSWORD_PENDING_MASK;
      slxu_zigbee_zcl_schedule_client_tick(endpoint,
                                           ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                           (cmd_data.implementationDateTime - currentTime)
                                           * MILLISECOND_TICKS_PER_SECOND);
    }
  } else {
    status = EMBER_ZCL_STATUS_INVALID_VALUE;
    goto kickout;
  }

  pass->implementationDateTime = cmd_data.implementationDateTime;
  pass->durationInMinutes = cmd_data.durationInMinutes;
  pass->passwordType = cmd_data.passwordType;
  emberAfCopyString(pass->password,
                    cmd_data.password,
                    EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_PASSWORD_LENGTH);

  kickout:
  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfDeviceManagementClusterUpdateSiteIdCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_device_management_cluster_update_site_id_command_t cmd_data;
  uint32_t currentTime = emberAfGetCurrentTime();
  uint8_t endpoint = emberAfCurrentEndpoint();
  EmberAfStatus status = EMBER_ZCL_STATUS_FAILURE;

  if (zcl_decode_device_management_cluster_update_site_id_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  if (pmInfo.providerId != cmd_data.providerId) {
    status = EMBER_ZCL_STATUS_NOT_AUTHORIZED;
    goto kickout;
  }

  emberAfCopyString(pmInfo.siteId.siteId,
                    cmd_data.siteId,
                    EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_SITE_ID_LENGTH);

  // If siteIdTime is 0xFFFFFFFF, cancel any pending change and replace it with
  // the incoming site ID
  // cancelling a scheduled update
  if (cmd_data.siteIdTime == 0xFFFFFFFF) {
    if ((pmInfo.pendingUpdates & EMBER_AF_DEVICE_MANAGEMENT_UPDATE_SITE_ID_PENDING_MASK)
        && (pmInfo.providerId == cmd_data.providerId)
        && (pmInfo.siteId.issuerEventId == cmd_data.issuerEventId)) {
      pmInfo.pendingUpdates &= ~EMBER_AF_DEVICE_MANAGEMENT_UPDATE_SITE_ID_PENDING_MASK;
      MEMSET(&(pmInfo.siteId), 0, sizeof(EmberAfDeviceManagementSiteId));
      status = EMBER_ZCL_STATUS_SUCCESS;
    } else {
      emberAfDeviceManagementClusterPrintln("Unable to cancel scheduled siteId update.");
      emberAfDeviceManagementClusterPrintln("Provider ID: 0x%4x", pmInfo.providerId);
      emberAfDeviceManagementClusterPrintln("Issuer Event ID: 0x%4x", pmInfo.issuerEventId);
      emberAfDeviceManagementClusterPrintPendingStatus(EMBER_AF_DEVICE_MANAGEMENT_UPDATE_SITE_ID_PENDING_MASK);
      status = EMBER_ZCL_STATUS_FAILURE;
      goto kickout;
    }
  } else { // schedule siteid update or immediate update
    if (cmd_data.issuerEventId > pmInfo.issuerEventId) {
      pmInfo.issuerEventId = cmd_data.issuerEventId;
    } else {
      status = EMBER_ZCL_STATUS_FAILURE;
      goto kickout;
    }

    // implementation time:
    //   0x00000000 - execute cmd immediately
    //   0xFFFFFFFF - cancel update (case covered by previous code blok)
    //   otherwise  - schedule for new pending action
    if (cmd_data.siteIdTime == 0x00000000) {
      // Are we meter or ihd?
      // Meter - update attr and reply with error if not sucessful
      // Ihd - no update attr needed. reply success.
      status = emberAfWriteAttribute(endpoint,
                                     ZCL_SIMPLE_METERING_CLUSTER_ID,
                                     ZCL_SITE_ID_ATTRIBUTE_ID,
                                     CLUSTER_MASK_SERVER,
                                     (uint8_t*)cmd_data.siteId,
                                     ZCL_OCTET_STRING_ATTRIBUTE_TYPE);
      if (emberAfContainsServer(emberAfCurrentEndpoint(), ZCL_SIMPLE_METERING_CLUSTER_ID)
          && (status != EMBER_ZCL_STATUS_SUCCESS)) {
        emberAfDeviceManagementClusterPrintln("Unable to write siteId attr in Metering cluster: 0x%d", status);
        status = EMBER_ZCL_STATUS_FAILURE;
      } else {
        status = EMBER_ZCL_STATUS_SUCCESS;
      }
      pmInfo.pendingUpdates &= ~EMBER_AF_DEVICE_MANAGEMENT_UPDATE_SITE_ID_PENDING_MASK;
      MEMSET(&(pmInfo.siteId), 0, sizeof(EmberAfDeviceManagementSiteId));
    } else {
      slxu_zigbee_zcl_schedule_client_tick(endpoint,
                                           ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                           (cmd_data.siteIdTime - currentTime)
                                           * MILLISECOND_TICKS_PER_SECOND);
      pmInfo.pendingUpdates |= EMBER_AF_DEVICE_MANAGEMENT_UPDATE_SITE_ID_PENDING_MASK;
      emberAfCopyString(pmInfo.siteId.siteId,
                        cmd_data.siteId,
                        EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_SITE_ID_LENGTH);
      pmInfo.siteId.implementationDateTime = cmd_data.siteIdTime;
      pmInfo.siteId.issuerEventId = cmd_data.issuerEventId;
      status = EMBER_ZCL_STATUS_SUCCESS;
    }
  }

  kickout:
  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfDeviceManagementClusterUpdateCINCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_device_management_cluster_update_cin_command_t cmd_data;
  uint32_t currentTime = emberAfGetCurrentTime();
  uint8_t endpoint = emberAfCurrentEndpoint();
  EmberAfStatus status = EMBER_ZCL_STATUS_FAILURE;

  if (zcl_decode_device_management_cluster_update_cin_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  if (pmInfo.providerId != cmd_data.providerId) {
    status = EMBER_ZCL_STATUS_NOT_AUTHORIZED;
    goto kickout;
  }

  // cancelling a scheduled update
  if (cmd_data.implementationTime == 0xFFFFFFFF) {
    if ((pmInfo.pendingUpdates & EMBER_AF_DEVICE_MANAGEMENT_UPDATE_CIN_PENDING_MASK)
        && (pmInfo.providerId == cmd_data.providerId)
        && (pmInfo.cin.issuerEventId == cmd_data.issuerEventId)) {
      pmInfo.pendingUpdates &= ~EMBER_AF_DEVICE_MANAGEMENT_UPDATE_CIN_PENDING_MASK;
      MEMSET(&(pmInfo.cin), 0, sizeof(EmberAfDeviceManagementCIN));
      status = EMBER_ZCL_STATUS_SUCCESS;
    } else {
      emberAfDeviceManagementClusterPrintln("Unable to cancel scheduled CIN update.");
      emberAfDeviceManagementClusterPrintln("Provider ID: 0x%4x", pmInfo.providerId);
      emberAfDeviceManagementClusterPrintln("Issuer Event ID: 0x%4x", pmInfo.issuerEventId);
      emberAfDeviceManagementClusterPrintPendingStatus(EMBER_AF_DEVICE_MANAGEMENT_UPDATE_CIN_PENDING_MASK);
      status = EMBER_ZCL_STATUS_FAILURE;
      goto kickout;
    }
  } else { // schedule CIN update or immediate update
    if (cmd_data.issuerEventId > pmInfo.issuerEventId) {
      pmInfo.issuerEventId = cmd_data.issuerEventId;
    } else {
      status = EMBER_ZCL_STATUS_FAILURE;
      goto kickout;
    }

    // implementation time:
    //   0x00000000 - execute cmd immediately
    //   0xFFFFFFFF - cancel update (case covered by previous code blok)
    //   otherwise  - schedule for new pending action
    if (cmd_data.implementationTime == 0x00000000) {
#ifdef SL_CATALOG_ZIGBEE_SIMPLE_METERING_SERVER_PRESENT
      // only try to update attr if we actually implement cluster / ESI
      status = emberAfWriteAttribute(endpoint,
                                     ZCL_SIMPLE_METERING_CLUSTER_ID,
                                     ZCL_CUSTOMER_ID_NUMBER_ATTRIBUTE_ID,
                                     CLUSTER_MASK_SERVER,
                                     (uint8_t*)cmd_data.customerIdNumber,
                                     ZCL_OCTET_STRING_ATTRIBUTE_TYPE);
      if (status != EMBER_ZCL_STATUS_SUCCESS) {
        emberAfDeviceManagementClusterPrintln("Unable to write CIN attr in Metering cluster: 0x%d", status);
        status = EMBER_ZCL_STATUS_FAILURE;
      } else {
#endif
      status = EMBER_ZCL_STATUS_SUCCESS;
#ifdef SL_CATALOG_ZIGBEE_SIMPLE_METERING_SERVER_PRESENT
    }
#endif
      pmInfo.pendingUpdates &= ~EMBER_AF_DEVICE_MANAGEMENT_UPDATE_CIN_PENDING_MASK;
    } else {
      slxu_zigbee_zcl_schedule_client_tick(endpoint,
                                           ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                           (cmd_data.implementationTime - currentTime)
                                           * MILLISECOND_TICKS_PER_SECOND);
      pmInfo.pendingUpdates |= EMBER_AF_DEVICE_MANAGEMENT_UPDATE_CIN_PENDING_MASK;
      status = EMBER_ZCL_STATUS_SUCCESS;
    }

    emberAfCopyString(pmInfo.cin.cin, cmd_data.customerIdNumber, EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_CIN_LENGTH);
    pmInfo.cin.implementationDateTime = cmd_data.implementationTime;
    pmInfo.cin.issuerEventId = cmd_data.issuerEventId;
  }

  kickout:
  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfDeviceManagementClusterGetEventConfigurationCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_device_management_cluster_get_event_configuration_command_t cmd_data;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t eventConfiguration;
  EmberAfStatus status;

  if (zcl_decode_device_management_cluster_get_event_configuration_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  if ((cmd_data.eventId & 0xFF) == 0xFF) {
    //emberAfDeviceManagementClusterPrintln("Wildcard profile Id requested %u", cmd_data.eventId & 0xFF00);
    uint8_t attributeSet = (cmd_data.eventId & 0xFF00) >> 8;
    //emberAfDeviceManagementClusterPrintln("attribute set %u",attributeSet);
    if ((attributeSet < 1 || attributeSet > 8) && attributeSet != 0xFF) {
      status = EMBER_ZCL_STATUS_NOT_FOUND;
      emberAfSendImmediateDefaultResponse(status);
      return true;
    } else {
      sendDeviceManagementClusterReportWildCardAttribute(attributeSet,
                                                         endpoint);
      return true;
    }
  } else {
    emberAfDeviceManagementClusterPrintln("Get Event callback %u", cmd_data.eventId);
    status = emberAfReadClientAttribute(endpoint,
                                        ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                        cmd_data.eventId,
                                        (uint8_t *)&eventConfiguration,
                                        sizeof(eventConfiguration));
    emberAfDeviceManagementClusterPrintln("Get Event status %u eventConfiguration %u", status, eventConfiguration);

    if (status == EMBER_ZCL_STATUS_SUCCESS) {
      (void) emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                                        | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER),
                                       ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                       ZCL_REPORT_EVENT_CONFIGURATION_COMMAND_ID,
                                       "uu",
                                       0,
                                       1);
      (void) emberAfPutInt16uInResp(cmd_data.eventId);
      (void) emberAfPutInt8uInResp(eventConfiguration);
      emberAfSendResponse();
      return true;
    } else {
      status = EMBER_ZCL_STATUS_NOT_FOUND;
      emberAfSendImmediateDefaultResponse(status);
      return true;
    }
  }
}

bool emberAfDeviceManagementClusterSetEventConfigurationCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_device_management_cluster_set_event_configuration_command_t cmd_data;
  uint8_t payloadIndex = 0;

  if (zcl_decode_device_management_cluster_set_event_configuration_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  switch (cmd_data.configurationControl) {
    case EMBER_ZCL_EVENT_CONFIGURATION_CONTROL_APPLY_BY_LIST: {
      uint8_t numberOfEvents = cmd_data.eventConfigurationPayload[0];
      emberAfDeviceManagementClusterPrintln("Number of Events %u", numberOfEvents);
      payloadIndex = 1;
      while (payloadIndex < numberOfEvents * 2) {
        uint16_t attributeId = emberAfGetInt16u(cmd_data.eventConfigurationPayload, payloadIndex, numberOfEvents * 2 + 1);
        payloadIndex += 2;
        emberAfDeviceManagementClusterPrintln("AttributeId 0x%2x", attributeId);
        writeDeviceManagementAttribute(attributeId, cmd_data.eventConfiguration);
      }
      break;
    }
    case EMBER_ZCL_EVENT_CONFIGURATION_CONTROL_APPLY_BY_EVENT_GROUP: {
      uint16_t eventGroupId = emberAfGetInt16u(cmd_data.eventConfigurationPayload, 0, 2);
      uint8_t attributeSet = (eventGroupId & 0xFF00) >> 8;
      writeDeviceManagementClusterWildCardAttribute(attributeSet,
                                                    emberAfCurrentEndpoint(),
                                                    cmd_data.eventConfiguration);
      break;
    }
    case EMBER_ZCL_EVENT_CONFIGURATION_CONTROL_APPLY_BY_LOG_TYPE: {
      uint8_t logType = cmd_data.eventConfigurationPayload[0];
      writeDeviceManagementClusterByLogTypeAttribute(logType,
                                                     emberAfCurrentEndpoint(),
                                                     cmd_data.eventConfiguration);
      break;
    }
    case EMBER_ZCL_EVENT_CONFIGURATION_CONTROL_APPLY_BY_CONFIGURATION_MATCH: {
      uint8_t currentConfiguration = cmd_data.eventConfigurationPayload[0];
      writeDeviceManagementClusterByMatchingAttribute(currentConfiguration,
                                                      emberAfCurrentEndpoint(),
                                                      cmd_data.eventConfiguration);
      break;
    }
    default:
      emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_INVALID_VALUE);
  }
  return true;
}

#else // !UC_BUILD

bool emberAfDeviceManagementClusterPublishChangeOfTenancyCallback(uint32_t supplierId,
                                                                  uint32_t issuerEventId,
                                                                  uint8_t tariffType,
                                                                  uint32_t implementationDateTime,
                                                                  uint32_t proposedTenancyChangeControl)
{
  uint32_t currentTime = emberAfGetCurrentTime();
  uint8_t endpoint = emberAfCurrentEndpoint();
  EmberAfStatus status;

  emberAfDeviceManagementClusterPrintln("RX: PublishChangeOfTenancy: 0x%4X, 0x%4X, 0x%X, 0x%4X, 0x%4X",
                                        supplierId,
                                        issuerEventId,
                                        tariffType,
                                        implementationDateTime,
                                        proposedTenancyChangeControl);

  if (pmInfo.providerId == 0) {
    pmInfo.providerId = supplierId;
  }

  if (issuerEventId > pmInfo.issuerEventId) {
    pmInfo.issuerEventId = issuerEventId;
  }

  pmInfo.tariffType = (EmberAfTariffType)tariffType;

  pmInfo.tenancy.implementationDateTime = implementationDateTime;
  pmInfo.tenancy.tenancy = proposedTenancyChangeControl;

  // Even if we aren't to immediately action the change of tenancy,
  // we still set these attributes accordingly
  status = emberAfWriteAttribute(endpoint,
                                 ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                 ZCL_CHANGE_OF_TENANCY_UPDATE_DATE_TIME_ATTRIBUTE_ID,
                                 CLUSTER_MASK_CLIENT,
                                 (uint8_t*)&implementationDateTime,
                                 ZCL_UTC_TIME_ATTRIBUTE_TYPE);

  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto kickout;
  }

  status = emberAfWriteAttribute(endpoint,
                                 ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                 ZCL_PROPOSED_TENANCY_CHANGE_CONTROL_ATTRIBUTE_ID,
                                 CLUSTER_MASK_CLIENT,
                                 (uint8_t*)&proposedTenancyChangeControl,
                                 ZCL_INT32U_ATTRIBUTE_TYPE);

  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto kickout;
  }

  // If the time has passed since the change of tenancy was to be implemented, take action
  if (currentTime >= implementationDateTime) {
    emberAfPluginDeviceManagementClientEnactChangeOfTenancyCallback(endpoint,
                                                                    &(pmInfo.tenancy));
    pmInfo.pendingUpdates &= ~EMBER_AF_DEVICE_MANAGEMENT_CHANGE_OF_TENANCY_PENDING_MASK;
  } else {
    // Otherwise, wait until the time of implementation
    slxu_zigbee_zcl_schedule_client_tick(endpoint,
                                         ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                         (implementationDateTime - currentTime)
                                         * MILLISECOND_TICKS_PER_SECOND);
    pmInfo.pendingUpdates |= EMBER_AF_DEVICE_MANAGEMENT_CHANGE_OF_TENANCY_PENDING_MASK;
  }

  kickout:
  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfDeviceManagementClusterPublishChangeOfSupplierCallback(uint32_t currentProviderId,
                                                                   uint32_t issuerEventId,
                                                                   uint8_t  tariffType,
                                                                   uint32_t proposedProviderId,
                                                                   uint32_t providerChangeImplementationTime,
                                                                   uint32_t providerChangeControl,
                                                                   uint8_t* proposedProviderName,
                                                                   uint8_t* proposedProviderContactDetails)
{
  EmberAfStatus status = EMBER_ZCL_STATUS_SUCCESS;
  uint32_t currentTime = emberAfGetCurrentTime();
  uint8_t endpoint = emberAfCurrentEndpoint();

  emberAfDeviceManagementClusterPrintln("RX: PublishChangeOfSupplier: 0x%4X, 0x%4X, 0x%X, 0x%4X, 0x%4X, 0x%4X, ",
                                        currentProviderId,
                                        issuerEventId,
                                        tariffType,
                                        proposedProviderId,
                                        providerChangeImplementationTime,
                                        providerChangeControl);
  emberAfDeviceManagementClusterPrintString(proposedProviderName);
  emberAfDeviceManagementClusterPrintln(", ");
  emberAfDeviceManagementClusterPrintString(proposedProviderContactDetails);
  emberAfDeviceManagementClusterPrintln("");

  if (proposedProviderName == NULL) {
    status = EMBER_ZCL_STATUS_INVALID_VALUE;
    goto kickout;
  }

  if (pmInfo.providerId == 0) {
    pmInfo.providerId = currentProviderId;
  }

  if (pmInfo.issuerEventId == 0) {
    pmInfo.issuerEventId = issuerEventId;
  }

  pmInfo.tariffType = (EmberAfTariffType) tariffType;

  pmInfo.supplier.proposedProviderId = proposedProviderId;
  pmInfo.supplier.implementationDateTime = providerChangeImplementationTime;
  pmInfo.supplier.providerChangeControl = providerChangeControl;
  emberAfCopyString(pmInfo.supplier.proposedProviderName,
                    proposedProviderName,
                    EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_PROPOSED_PROVIDER_NAME_LENGTH);
  emberAfCopyString(pmInfo.supplier.proposedProviderContactDetails,
                    proposedProviderContactDetails,
                    EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_PROPOSED_PROVIDER_CONTACT_DETAILS_LENGTH);

  // If the time has passed since the change of supplier was to be implemented, take action
  if (currentTime >= providerChangeImplementationTime) {
    emberAfPluginDeviceManagementClientEnactChangeOfSupplierCallback(endpoint,
                                                                     &(pmInfo.supplier));
    pmInfo.pendingUpdates &= ~EMBER_AF_DEVICE_MANAGEMENT_CHANGE_OF_SUPPLIER_PENDING_MASK;
  } else {
    // Otherwise, wait until the time of implementation
    slxu_zigbee_zcl_schedule_client_tick(endpoint,
                                         ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                         (providerChangeImplementationTime - currentTime)
                                         * MILLISECOND_TICKS_PER_SECOND);
    pmInfo.pendingUpdates |= EMBER_AF_DEVICE_MANAGEMENT_CHANGE_OF_SUPPLIER_PENDING_MASK;
  }

  kickout:
  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfDeviceManagementClusterRequestNewPasswordResponseCallback(uint32_t issuerEventId,
                                                                      uint32_t implementationDateTime,
                                                                      uint16_t durationInMinutes,
                                                                      uint8_t passwordType,
                                                                      uint8_t* password)
{
  uint32_t currentTime = emberAfGetCurrentTime();
  uint8_t endpoint = emberAfCurrentEndpoint();
  EmberAfDeviceManagementPassword *pass;
  EmberAfStatus status = EMBER_ZCL_STATUS_SUCCESS;

  if (password == NULL) {
    status = EMBER_ZCL_STATUS_INVALID_VALUE;
    goto kickout;
  }

  if (issuerEventId > pmInfo.issuerEventId) {
    pmInfo.issuerEventId = issuerEventId;
  }

  if (implementationDateTime < currentTime && implementationDateTime != 0x00000000) {
    goto kickout;
  }

  if (passwordType & SERVICE_PASSWORD) {
    if (currentTime == implementationDateTime || implementationDateTime == 0x00000000) {
      pass = &(pmInfo.servicePassword);
    } else {
      pass = &(servicePassword);
      pmInfo.pendingUpdates |= EMBER_AF_DEVICE_MANAGEMENT_UPDATE_SERVICE_PASSWORD_PENDING_MASK;
      slxu_zigbee_zcl_schedule_client_tick(endpoint,
                                           ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                           (implementationDateTime - currentTime)
                                           * MILLISECOND_TICKS_PER_SECOND);
    }
  } else if (passwordType & CONSUMER_PASSWORD) {
    if (currentTime == implementationDateTime || implementationDateTime == (0x00000000)) {
      pass = &(pmInfo.consumerPassword);
    } else {
      pass = &(consumerPassword);
      pmInfo.pendingUpdates |= EMBER_AF_DEVICE_MANAGEMENT_UPDATE_CONSUMER_PASSWORD_PENDING_MASK;
      slxu_zigbee_zcl_schedule_client_tick(endpoint,
                                           ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                           (implementationDateTime - currentTime)
                                           * MILLISECOND_TICKS_PER_SECOND);
    }
  } else {
    status = EMBER_ZCL_STATUS_INVALID_VALUE;
    goto kickout;
  }

  pass->implementationDateTime = implementationDateTime;
  pass->durationInMinutes = durationInMinutes;
  pass->passwordType = passwordType;
  emberAfCopyString(pass->password,
                    password,
                    EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_PASSWORD_LENGTH);

  kickout:
  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfDeviceManagementClusterUpdateSiteIdCallback(uint32_t issuerEventId,
                                                        uint32_t implementationDateTime,
                                                        uint32_t providerId,
                                                        uint8_t* siteId)
{
  uint32_t currentTime = emberAfGetCurrentTime();
  uint8_t endpoint = emberAfCurrentEndpoint();
  EmberAfStatus status = EMBER_ZCL_STATUS_FAILURE;

  if (pmInfo.providerId != providerId) {
    status = EMBER_ZCL_STATUS_NOT_AUTHORIZED;
    goto kickout;
  }

  emberAfCopyString(pmInfo.siteId.siteId,
                    siteId,
                    EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_SITE_ID_LENGTH);

  // If implementationDateTime is 0xFFFFFFFF, cancel any pending change and replace it with
  // the incoming site ID
  // cancelling a scheduled update
  if (implementationDateTime == 0xFFFFFFFF) {
    if ((pmInfo.pendingUpdates & EMBER_AF_DEVICE_MANAGEMENT_UPDATE_SITE_ID_PENDING_MASK)
        && (pmInfo.providerId == providerId)
        && (pmInfo.siteId.issuerEventId == issuerEventId)) {
      pmInfo.pendingUpdates &= ~EMBER_AF_DEVICE_MANAGEMENT_UPDATE_SITE_ID_PENDING_MASK;
      MEMSET(&(pmInfo.siteId), 0, sizeof(EmberAfDeviceManagementSiteId));
      status = EMBER_ZCL_STATUS_SUCCESS;
    } else {
      emberAfDeviceManagementClusterPrintln("Unable to cancel scheduled siteId update.");
      emberAfDeviceManagementClusterPrintln("Provider ID: 0x%4x", pmInfo.providerId);
      emberAfDeviceManagementClusterPrintln("Issuer Event ID: 0x%4x", pmInfo.issuerEventId);
      emberAfDeviceManagementClusterPrintPendingStatus(EMBER_AF_DEVICE_MANAGEMENT_UPDATE_SITE_ID_PENDING_MASK);
      status = EMBER_ZCL_STATUS_FAILURE;
      goto kickout;
    }
  } else { // schedule siteid update or immediate update
    if (issuerEventId > pmInfo.issuerEventId) {
      pmInfo.issuerEventId = issuerEventId;
    } else {
      status = EMBER_ZCL_STATUS_FAILURE;
      goto kickout;
    }

    // implementation time:
    //   0x00000000 - execute cmd immediately
    //   0xFFFFFFFF - cancel update (case covered by previous code blok)
    //   otherwise  - schedule for new pending action
    if (implementationDateTime == 0x00000000) {
      // Are we meter or ihd?
      // Meter - update attr and reply with error if not sucessful
      // Ihd - no update attr needed. reply success.
      status = emberAfWriteAttribute(endpoint,
                                     ZCL_SIMPLE_METERING_CLUSTER_ID,
                                     ZCL_SITE_ID_ATTRIBUTE_ID,
                                     CLUSTER_MASK_SERVER,
                                     (uint8_t*)siteId,
                                     ZCL_OCTET_STRING_ATTRIBUTE_TYPE);
      if (emberAfContainsServer(emberAfCurrentEndpoint(), ZCL_SIMPLE_METERING_CLUSTER_ID)
          && (status != EMBER_ZCL_STATUS_SUCCESS)) {
        emberAfDeviceManagementClusterPrintln("Unable to write siteId attr in Metering cluster: 0x%d", status);
        status = EMBER_ZCL_STATUS_FAILURE;
      } else {
        status = EMBER_ZCL_STATUS_SUCCESS;
      }
      pmInfo.pendingUpdates &= ~EMBER_AF_DEVICE_MANAGEMENT_UPDATE_SITE_ID_PENDING_MASK;
      MEMSET(&(pmInfo.siteId), 0, sizeof(EmberAfDeviceManagementSiteId));
    } else {
      slxu_zigbee_zcl_schedule_client_tick(endpoint,
                                           ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                           (implementationDateTime - currentTime)
                                           * MILLISECOND_TICKS_PER_SECOND);
      pmInfo.pendingUpdates |= EMBER_AF_DEVICE_MANAGEMENT_UPDATE_SITE_ID_PENDING_MASK;
      emberAfCopyString(pmInfo.siteId.siteId,
                        siteId,
                        EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_SITE_ID_LENGTH);
      pmInfo.siteId.implementationDateTime = implementationDateTime;
      pmInfo.siteId.issuerEventId = issuerEventId;
      status = EMBER_ZCL_STATUS_SUCCESS;
    }
  }

  kickout:
  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfDeviceManagementClusterSetSupplyStatusCallback(uint32_t issuerEventId,
                                                           uint8_t supplyTamperState,
                                                           uint8_t supplyDepletionState,
                                                           uint8_t supplyUncontrolledFlowState,
                                                           uint8_t loadLimitSupplyState)
{
  EmberAfDeviceManagementSupplyStatusFlags *flags = &(pmInfo.supplyStatusFlags);
  uint8_t endpoint = emberAfCurrentEndpoint();

  if (pmInfo.issuerEventId == 0) {
    pmInfo.issuerEventId = issuerEventId;
  }

  flags->supplyTamperState = supplyTamperState;
  flags->supplyDepletionState = supplyDepletionState;
  flags->supplyUncontrolledFlowState = supplyUncontrolledFlowState;
  flags->loadLimitSupplyState = loadLimitSupplyState;

  emberAfPluginDeviceManagementClientSetSupplyStatusCallback(endpoint,
                                                             flags);

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfDeviceManagementClusterPublishUncontrolledFlowThresholdCallback(uint32_t providerId,
                                                                            uint32_t issuerEventId,
                                                                            uint16_t uncontrolledFlowThreshold,
                                                                            uint8_t unitOfMeasure,
                                                                            uint16_t multiplier,
                                                                            uint16_t divisor,
                                                                            uint8_t stabilisationPeriod,
                                                                            uint16_t measurementPeriod)
{
  EmberAfDeviceManagementUncontrolledFlowThreshold *threshold = &(pmInfo.threshold);
  uint8_t endpoint = emberAfCurrentEndpoint();

  if (pmInfo.providerId == 0) {
    pmInfo.providerId = providerId;
  }

  if (pmInfo.issuerEventId == 0) {
    pmInfo.issuerEventId = issuerEventId;
  }

  threshold->uncontrolledFlowThreshold = uncontrolledFlowThreshold;
  threshold->unitOfMeasure = unitOfMeasure;
  threshold->multiplier = multiplier;
  threshold->divisor = divisor;
  threshold->stabilisationPeriod = stabilisationPeriod;
  threshold->measurementPeriod = measurementPeriod;

  emberAfPluginDeviceManagementClientEnactUpdateUncontrolledFlowThresholdCallback(endpoint,
                                                                                  threshold);

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfDeviceManagementClusterUpdateCINCallback(uint32_t issuerEventId,
                                                     uint32_t implementationDateTime,
                                                     uint32_t providerId,
                                                     uint8_t* customerIdNumber)
{
  uint32_t currentTime = emberAfGetCurrentTime();
  uint8_t endpoint = emberAfCurrentEndpoint();
  EmberAfStatus status = EMBER_ZCL_STATUS_FAILURE;

  if (pmInfo.providerId != providerId) {
    status = EMBER_ZCL_STATUS_NOT_AUTHORIZED;
    goto kickout;
  }

  // cancelling a scheduled update
  if (implementationDateTime == 0xFFFFFFFF) {
    if ((pmInfo.pendingUpdates & EMBER_AF_DEVICE_MANAGEMENT_UPDATE_CIN_PENDING_MASK)
        && (pmInfo.providerId == providerId)
        && (pmInfo.cin.issuerEventId == issuerEventId)) {
      pmInfo.pendingUpdates &= ~EMBER_AF_DEVICE_MANAGEMENT_UPDATE_CIN_PENDING_MASK;
      MEMSET(&(pmInfo.cin), 0, sizeof(EmberAfDeviceManagementCIN));
      status = EMBER_ZCL_STATUS_SUCCESS;
    } else {
      emberAfDeviceManagementClusterPrintln("Unable to cancel scheduled CIN update.");
      emberAfDeviceManagementClusterPrintln("Provider ID: 0x%4x", pmInfo.providerId);
      emberAfDeviceManagementClusterPrintln("Issuer Event ID: 0x%4x", pmInfo.issuerEventId);
      emberAfDeviceManagementClusterPrintPendingStatus(EMBER_AF_DEVICE_MANAGEMENT_UPDATE_CIN_PENDING_MASK);
      status = EMBER_ZCL_STATUS_FAILURE;
      goto kickout;
    }
  } else { // schedule CIN update or immediate update
    if (issuerEventId > pmInfo.issuerEventId) {
      pmInfo.issuerEventId = issuerEventId;
    } else {
      status = EMBER_ZCL_STATUS_FAILURE;
      goto kickout;
    }

    // implementation time:
    //   0x00000000 - execute cmd immediately
    //   0xFFFFFFFF - cancel update (case covered by previous code blok)
    //   otherwise  - schedule for new pending action
    if (implementationDateTime == 0x00000000) {
#ifdef SL_CATALOG_ZIGBEE_SIMPLE_METERING_SERVER_PRESENT
      // only try to update attr if we actually implement cluster / ESI
      status = emberAfWriteAttribute(endpoint,
                                     ZCL_SIMPLE_METERING_CLUSTER_ID,
                                     ZCL_CUSTOMER_ID_NUMBER_ATTRIBUTE_ID,
                                     CLUSTER_MASK_SERVER,
                                     (uint8_t*)customerIdNumber,
                                     ZCL_OCTET_STRING_ATTRIBUTE_TYPE);
      if (status != EMBER_ZCL_STATUS_SUCCESS) {
        emberAfDeviceManagementClusterPrintln("Unable to write CIN attr in Metering cluster: 0x%d", status);
        status = EMBER_ZCL_STATUS_FAILURE;
      } else {
#endif
      status = EMBER_ZCL_STATUS_SUCCESS;
#ifdef SL_CATALOG_ZIGBEE_SIMPLE_METERING_SERVER_PRESENT
    }
#endif
      pmInfo.pendingUpdates &= ~EMBER_AF_DEVICE_MANAGEMENT_UPDATE_CIN_PENDING_MASK;
    } else {
      slxu_zigbee_zcl_schedule_client_tick(endpoint,
                                           ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                           (implementationDateTime - currentTime)
                                           * MILLISECOND_TICKS_PER_SECOND);
      pmInfo.pendingUpdates |= EMBER_AF_DEVICE_MANAGEMENT_UPDATE_CIN_PENDING_MASK;
      status = EMBER_ZCL_STATUS_SUCCESS;
    }

    emberAfCopyString(pmInfo.cin.cin, customerIdNumber, EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_CIN_LENGTH);
    pmInfo.cin.implementationDateTime = implementationDateTime;
    pmInfo.cin.issuerEventId = issuerEventId;
  }

  kickout:
  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfDeviceManagementClusterGetEventConfigurationCallback(uint16_t eventId)
{
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t eventConfiguration;
  EmberAfStatus status;
  if ((eventId & 0xFF) == 0xFF) {
    //emberAfDeviceManagementClusterPrintln("Wildcard profile Id requested %u",eventId & 0xFF00);
    uint8_t attributeSet = (eventId & 0xFF00) >> 8;
    //emberAfDeviceManagementClusterPrintln("attribute set %u",attributeSet);
    if ((attributeSet < 1 || attributeSet > 8) && attributeSet != 0xFF) {
      status = EMBER_ZCL_STATUS_NOT_FOUND;
      emberAfSendImmediateDefaultResponse(status);
      return true;
    } else {
      sendDeviceManagementClusterReportWildCardAttribute(attributeSet,
                                                         endpoint);
      return true;
    }
  } else {
    emberAfDeviceManagementClusterPrintln("Get Event callback %u", eventId);
    status = emberAfReadClientAttribute(endpoint,
                                        ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                        eventId,
                                        (uint8_t *)&eventConfiguration,
                                        sizeof(eventConfiguration));
    emberAfDeviceManagementClusterPrintln("Get Event status %u eventConfiguration %u", status, eventConfiguration);

    if (status == EMBER_ZCL_STATUS_SUCCESS) {
      (void) emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                                        | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER),
                                       ZCL_DEVICE_MANAGEMENT_CLUSTER_ID,
                                       ZCL_REPORT_EVENT_CONFIGURATION_COMMAND_ID,
                                       "uu",
                                       0,
                                       1);
      (void) emberAfPutInt16uInResp(eventId);
      (void) emberAfPutInt8uInResp(eventConfiguration);
      emberAfSendResponse();
      return true;
    } else {
      status = EMBER_ZCL_STATUS_NOT_FOUND;
      emberAfSendImmediateDefaultResponse(status);
      return true;
    }
  }
}

bool emberAfDeviceManagementClusterSetEventConfigurationCallback(uint32_t issuerEventId,
                                                                 uint32_t startDateTime,
                                                                 uint8_t eventConfiguration,
                                                                 uint8_t configurationControl,
                                                                 uint8_t* eventConfigurationPayload)
{
  uint8_t payloadIndex = 0;
  switch (configurationControl) {
    case EMBER_ZCL_EVENT_CONFIGURATION_CONTROL_APPLY_BY_LIST: {
      uint8_t numberOfEvents = eventConfigurationPayload[0];
      emberAfDeviceManagementClusterPrintln("Number of Events %u", numberOfEvents);
      payloadIndex = 1;
      while (payloadIndex < numberOfEvents * 2) {
        uint16_t attributeId = emberAfGetInt16u(eventConfigurationPayload, payloadIndex, numberOfEvents * 2 + 1);
        payloadIndex += 2;
        emberAfDeviceManagementClusterPrintln("AttributeId 0x%2x", attributeId);
        writeDeviceManagementAttribute(attributeId, eventConfiguration);
      }
      break;
    }
    case EMBER_ZCL_EVENT_CONFIGURATION_CONTROL_APPLY_BY_EVENT_GROUP: {
      uint16_t eventGroupId = emberAfGetInt16u(eventConfigurationPayload, 0, 2);
      uint8_t attributeSet = (eventGroupId & 0xFF00) >> 8;
      writeDeviceManagementClusterWildCardAttribute(attributeSet,
                                                    emberAfCurrentEndpoint(),
                                                    eventConfiguration);
      break;
    }
    case EMBER_ZCL_EVENT_CONFIGURATION_CONTROL_APPLY_BY_LOG_TYPE: {
      uint8_t logType = eventConfigurationPayload[0];
      writeDeviceManagementClusterByLogTypeAttribute(logType,
                                                     emberAfCurrentEndpoint(),
                                                     eventConfiguration);
      break;
    }
    case EMBER_ZCL_EVENT_CONFIGURATION_CONTROL_APPLY_BY_CONFIGURATION_MATCH: {
      uint8_t currentConfiguration = eventConfigurationPayload[0];
      writeDeviceManagementClusterByMatchingAttribute(currentConfiguration,
                                                      emberAfCurrentEndpoint(),
                                                      eventConfiguration);
      break;
    }
    default:
      emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_INVALID_VALUE);
  }
  return true;
}

#endif // UC_BUILD

void emberAfDeviceManagementClientPrint(void)

{
  emberAfDeviceManagementClusterPrintln("== Device Management Information ==\n");

  emberAfDeviceManagementClusterPrintln("  == Tenancy ==");
  emberAfDeviceManagementClusterPrintln("  Implementation Date / Time: 0x%4x", pmInfo.tenancy.implementationDateTime);
  emberAfDeviceManagementClusterPrintln("  Tenancy: 0x%4x", pmInfo.tenancy.tenancy);
  emberAfDeviceManagementClusterPrintPendingStatus(EMBER_AF_DEVICE_MANAGEMENT_CHANGE_OF_TENANCY_PENDING_MASK);
  emberAfDeviceManagementClusterPrintln("");

  emberAfDeviceManagementClusterPrintln("  == Supplier ==");
  emberAfDeviceManagementClusterPrint("  Provider name: ");
  emberAfDeviceManagementClusterPrintString(pmInfo.supplier.proposedProviderName);
  emberAfDeviceManagementClusterPrintln("\n  Proposed Provider Id: 0x%4x", pmInfo.supplier.proposedProviderId);
  emberAfDeviceManagementClusterPrintln("  Implementation Date / Time: 0x%4x", pmInfo.supplier.implementationDateTime);
  emberAfDeviceManagementClusterPrintln("  Provider Change Control: 0x%4x", pmInfo.supplier.providerChangeControl);
  emberAfDeviceManagementClusterPrintPendingStatus(EMBER_AF_DEVICE_MANAGEMENT_CHANGE_OF_TENANCY_PENDING_MASK);
  emberAfDeviceManagementClusterPrintln("");

  emberAfDeviceManagementClusterPrintln("  == Site ID ==");
  emberAfDeviceManagementClusterPrint("  Site ID: ");
  emberAfDeviceManagementClusterPrintString(pmInfo.siteId.siteId);
  emberAfDeviceManagementClusterPrintln("\n  Site Id Implementation Date / Time: 0x%4x", pmInfo.siteId.implementationDateTime);
  emberAfDeviceManagementClusterPrintPendingStatus(EMBER_AF_DEVICE_MANAGEMENT_UPDATE_SITE_ID_PENDING_MASK);
  emberAfDeviceManagementClusterPrintln("");

  emberAfDeviceManagementClusterPrintln("  == Customer ID Number ==");
  emberAfDeviceManagementClusterPrint("  Customer ID Number: ");
  emberAfDeviceManagementClusterPrintString(pmInfo.cin.cin);
  emberAfDeviceManagementClusterPrintln("\n  Customer ID Number Implementation Date / Time: 0x%4x\n", pmInfo.cin.implementationDateTime);
  emberAfDeviceManagementClusterPrintPendingStatus(EMBER_AF_DEVICE_MANAGEMENT_UPDATE_CIN_PENDING_MASK);

  emberAfDeviceManagementClusterPrintln("  == Passwords ==");
  emberAfDeviceManagementClusterPrintln("   = Service Password =");
  emberAfDeviceManagementClusterPrint("   Password: ");
  emberAfDeviceManagementClusterPrintString(pmInfo.servicePassword.password);
  emberAfDeviceManagementClusterPrintln("\n   Implementation Date / Time: 0x%4x", pmInfo.servicePassword.implementationDateTime);
  emberAfDeviceManagementClusterPrintln("   Duration In Minutes: 0x%2x", pmInfo.servicePassword.durationInMinutes);
  emberAfDeviceManagementClusterPrintln("   Password Type: 0x%x\n", pmInfo.servicePassword.passwordType);

  emberAfDeviceManagementClusterPrintln("   = Consumer Password =");
  emberAfDeviceManagementClusterPrint("   Password: ");
  emberAfDeviceManagementClusterPrintString(pmInfo.consumerPassword.password);
  emberAfDeviceManagementClusterPrintln("\n   Implementation Date / Time: 0x%4x", pmInfo.consumerPassword.implementationDateTime);
  emberAfDeviceManagementClusterPrintln("   Duration In Minutes: 0x%2x", pmInfo.consumerPassword.durationInMinutes);
  emberAfDeviceManagementClusterPrintln("   Password Type: 0x%x\n", pmInfo.consumerPassword.passwordType);

  emberAfDeviceManagementClusterPrintln("  == Issuer Event ID ==");
  emberAfDeviceManagementClusterPrintln("  Issuer Event ID: 0x%4x\n", pmInfo.issuerEventId);

  emberAfDeviceManagementClusterPrintln("  == Provider ID ==");
  emberAfDeviceManagementClusterPrintln("  Provider Id: 0x%4x", pmInfo.providerId);

  emberAfDeviceManagementClusterPrintln("== End of Device Management Information ==");
}

#ifdef UC_BUILD

uint32_t emberAfDeviceManagementClusterClientCommandParse(sl_service_opcode_t opcode,
                                                          sl_service_function_context_t *context)
{
  (void)opcode;

  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_PUBLISH_CHANGE_OF_TENANCY_COMMAND_ID:
      {
        wasHandled = emberAfDeviceManagementClusterPublishChangeOfTenancyCallback(cmd);
        break;
      }
      case ZCL_PUBLISH_CHANGE_OF_SUPPLIER_COMMAND_ID:
      {
        wasHandled = emberAfDeviceManagementClusterPublishChangeOfSupplierCallback(cmd);
        break;
      }
      case ZCL_REQUEST_NEW_PASSWORD_RESPONSE_COMMAND_ID:
      {
        wasHandled = emberAfDeviceManagementClusterRequestNewPasswordResponseCallback(cmd);
        break;
      }
      case ZCL_UPDATE_SITE_ID_COMMAND_ID:
      {
        wasHandled = emberAfDeviceManagementClusterUpdateSiteIdCallback(cmd);
        break;
      }
      case ZCL_SET_EVENT_CONFIGURATION_COMMAND_ID:
      {
        wasHandled = emberAfDeviceManagementClusterSetEventConfigurationCallback(cmd);
        break;
      }
      case ZCL_GET_EVENT_CONFIGURATION_COMMAND_ID:
      {
        wasHandled = emberAfDeviceManagementClusterGetEventConfigurationCallback(cmd);
        break;
      }
      case ZCL_UPDATE_CIN_COMMAND_ID:
      {
        wasHandled = emberAfDeviceManagementClusterUpdateCINCallback(cmd);
        break;
      }
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
