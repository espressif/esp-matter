/***************************************************************************//**
 * @file
 * @brief This file contains the definitions for the attribute table, its
 * sizes, count, and API.
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

#ifndef ZCL_UTIL_ATTRIBUTE_TABLE_H
#define ZCL_UTIL_ATTRIBUTE_TABLE_H

#include "../include/af.h"

#define ZCL_NULL_ATTRIBUTE_TABLE_INDEX 0xFFFF

// Remote devices writing attributes of local device
EmberAfStatus emberAfWriteAttributeExternal(uint8_t endpoint,
                                            EmberAfClusterId cluster,
                                            EmberAfAttributeId attributeID,
                                            uint8_t mask,
                                            uint16_t manufacturerCode,
                                            uint8_t* dataPtr,
                                            EmberAfAttributeType dataType);

void emberAfRetrieveAttributeAndCraftResponse(uint8_t endpoint,
                                              EmberAfClusterId clusterId,
                                              EmberAfAttributeId attrId,
                                              uint8_t mask,
                                              uint16_t manufacturerCode,
                                              uint16_t readLength);
EmberAfStatus emberAfAppendAttributeReportFields(uint8_t endpoint,
                                                 EmberAfClusterId clusterId,
                                                 EmberAfAttributeId attributeId,
                                                 uint8_t mask,
                                                 uint8_t *buffer,
                                                 uint8_t bufLen,
                                                 uint8_t *bufIndex);
void emberAfPrintAttributeTable(void);

bool emberAfReadSequentialAttributesAddToResponse(uint8_t endpoint,
                                                  EmberAfClusterId clusterId,
                                                  EmberAfAttributeId startAttributeId,
                                                  uint8_t mask,
                                                  uint16_t manufacturerCode,
                                                  uint8_t maxAttributeIds,
                                                  bool includeAccessControl);

EmberAfStatus emAfWriteAttribute(uint8_t endpoint,
                                 EmberAfClusterId cluster,
                                 EmberAfAttributeId attributeID,
                                 uint8_t mask,
                                 uint16_t manufacturerCode,
                                 uint8_t* data,
                                 EmberAfAttributeType dataType,
                                 bool overrideReadOnlyAndDataType,
                                 bool justTest);

EmberAfStatus emAfReadAttribute(uint8_t endpoint,
                                EmberAfClusterId cluster,
                                EmberAfAttributeId attributeID,
                                uint8_t mask,
                                uint16_t manufacturerCode,
                                uint8_t* dataPtr,
                                uint16_t readLength,
                                EmberAfAttributeType* dataType);

uint16_t emAfResolveMfgCodeForDiscoverAttribute(uint8_t endpoint,
                                                EmberAfClusterId clusterId,
                                                EmberAfAttributeId startAttributeId,
                                                uint8_t clientServerMask);
#endif // ZCL_UTIL_ATTRIBUTE_TABLE_H
