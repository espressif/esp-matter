/***************************************************************************//**
 * @file
 * @brief A file for configuring NCP parameters.
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
#include "app/framework/util/af-main.h"

void emberAfPluginNcpConfigurationNcpInitCallback(bool memoryAllocation)
{
  if (memoryAllocation) {
    emberAfSetEzspConfigValue(EZSP_CONFIG_BINDING_TABLE_SIZE,
                              EMBER_BINDING_TABLE_SIZE,
                              "binding table size");
    emberAfSetEzspConfigValue(EZSP_CONFIG_KEY_TABLE_SIZE,
                              EMBER_KEY_TABLE_SIZE,
                              "key table size");
    emberAfSetEzspConfigValue(EZSP_CONFIG_MAX_END_DEVICE_CHILDREN,
                              EMBER_MAX_END_DEVICE_CHILDREN,
                              "max end device children");
    emberAfSetEzspConfigValue(EZSP_CONFIG_APS_UNICAST_MESSAGE_COUNT,
                              EMBER_APS_UNICAST_MESSAGE_COUNT,
                              "aps unicast message count");
    emberAfSetEzspConfigValue(EZSP_CONFIG_BROADCAST_TABLE_SIZE,
                              EMBER_BROADCAST_TABLE_SIZE,
                              "broadcast table size");
    emberAfSetEzspConfigValue(EZSP_CONFIG_NEIGHBOR_TABLE_SIZE,
                              EMBER_NEIGHBOR_TABLE_SIZE,
                              "neighbor table size");
  } else {
    emberAfSetEzspConfigValue(EZSP_CONFIG_END_DEVICE_POLL_TIMEOUT,
                              EMBER_END_DEVICE_POLL_TIMEOUT,
                              "end device poll timeout");
    emberAfSetEzspConfigValue(EZSP_CONFIG_ZLL_GROUP_ADDRESSES,
                              EMBER_ZLL_GROUP_ADDRESSES,
                              "zll group addresses");
    emberAfSetEzspConfigValue(EZSP_CONFIG_ZLL_RSSI_THRESHOLD,
                              EMBER_ZLL_RSSI_THRESHOLD,
                              "zll rssi threshold");
    emberAfSetEzspConfigValue(EZSP_CONFIG_TRANSIENT_KEY_TIMEOUT_S,
                              EMBER_TRANSIENT_KEY_TIMEOUT_S,
                              "transient key timeout");
  }
}
