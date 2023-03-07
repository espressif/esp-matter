/***************************************************************************//**
 * @file
 * @brief Code to manage the mirrors via the CLI.
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
#include "app/framework/util/util.h"
#include "meter-mirror.h"

uint16_t emberAfPluginSimpleMeteringClientRemoveMirrorCallback(EmberEUI64 requestingDeviceIeeeAddress);

//-----------------------------------------------------------------------------
// Globals

#ifdef UC_BUILD
#include "app/util/serial/sl_zigbee_command_interpreter.h"
void emAfPluginMeterMirrorStatusCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t meteringDeviceTypeAttribute = 0xFF;
  uint16_t endpoint;
  uint8_t mirrorEui[EUI64_SIZE];

  emberAfSimpleMeteringClusterPrintln("Known meters");
  // Go over our endpoints and find an unallocated meter mirror
  for (endpoint = EMBER_AF_PLUGIN_METER_MIRROR_ENDPOINT_START;
       endpoint < EMBER_AF_PLUGIN_METER_MIRROR_ENDPOINT_START + EMBER_AF_PLUGIN_METER_MIRROR_MAX_MIRRORS;
       endpoint++) {
    bool validEndpoint;

    // Reset these every run
    MEMSET(mirrorEui, 0xFF, EUI64_SIZE);
    meteringDeviceTypeAttribute = 0xFF;
    validEndpoint = emberAfPluginMeterMirrorGetEui64ByEndpoint(endpoint,
                                                               mirrorEui);

    if (!validEndpoint) {
      continue;
    }
    if (emberAfPluginMeterMirrorIsMirrorUsed(endpoint)) {
      // Retrieve the meter device type, this will tell us what
      // type of meter we are looking at on this endpoint
      emberAfReadServerAttribute(endpoint,
                                 ZCL_SIMPLE_METERING_CLUSTER_ID,
                                 ZCL_METERING_DEVICE_TYPE_ATTRIBUTE_ID,
                                 &meteringDeviceTypeAttribute,
                                 sizeof(meteringDeviceTypeAttribute));
    }

    emberAfSimpleMeteringClusterPrint("ep: %d  ieee:", endpoint);
    emberAfPrintBigEndianEui64(mirrorEui);
    emberAfSimpleMeteringClusterPrintln("  type: 0x%X", meteringDeviceTypeAttribute);
  }
  // Explicitly usea  function call to find the number of mirrors allocated instead
  // of just calculating it.  In case there is ever a discrepancy,
  // this will help track down any bugs.

  emberAfSimpleMeteringClusterPrintln("%d of %d mirrors used.",
                                      emAfPluginMeterMirrorGetMirrorsAllocated(),
                                      EMBER_AF_PLUGIN_METER_MIRROR_MAX_MIRRORS);
}

void emAfPluginMeterMirrorRemoveCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  EmberEUI64 mirrorEui64;
  bool success = emberAfPluginMeterMirrorGetEui64ByEndpoint(endpoint, mirrorEui64);
  if (!success) {
    emberAfSimpleMeteringClusterPrintln("Error: Failed to obtain EUI64 for endpoint %d", endpoint);
    return;
  }
  emberAfPluginSimpleMeteringClientRemoveMirrorCallback(mirrorEui64);
}

#else

void emAfPluginMeterMirrorStatusCommand(void)
{
  uint8_t meteringDeviceTypeAttribute = 0xFF;
  uint16_t endpoint;
  uint8_t mirrorEui[EUI64_SIZE];

  emberAfSimpleMeteringClusterPrintln("Known meters");
  // Go over our endpoints and find an unallocated meter mirror
  for (endpoint = EMBER_AF_PLUGIN_METER_MIRROR_ENDPOINT_START;
       endpoint < EMBER_AF_PLUGIN_METER_MIRROR_ENDPOINT_START + EMBER_AF_PLUGIN_METER_MIRROR_MAX_MIRRORS;
       endpoint++) {
    bool validEndpoint;

    // Reset these every run
    MEMSET(mirrorEui, 0xFF, EUI64_SIZE);
    meteringDeviceTypeAttribute = 0xFF;
    validEndpoint = emberAfPluginMeterMirrorGetEui64ByEndpoint(endpoint,
                                                               mirrorEui);

    if (!validEndpoint) {
      continue;
    }
    if (emberAfPluginMeterMirrorIsMirrorUsed(endpoint)) {
      // Retrieve the meter device type, this will tell us what
      // type of meter we are looking at on this endpoint
      emberAfReadServerAttribute(endpoint,
                                 ZCL_SIMPLE_METERING_CLUSTER_ID,
                                 ZCL_METERING_DEVICE_TYPE_ATTRIBUTE_ID,
                                 &meteringDeviceTypeAttribute,
                                 sizeof(meteringDeviceTypeAttribute));
    }

    emberAfSimpleMeteringClusterPrint("ep: %d  ieee:", endpoint);
    emberAfPrintBigEndianEui64(mirrorEui);
    emberAfSimpleMeteringClusterPrintln("  type: 0x%X", meteringDeviceTypeAttribute);
  }
  // Explicitly usea  function call to find the number of mirrors allocated instead
  // of just calculating it.  In case there is ever a discrepancy,
  // this will help track down any bugs.

  emberAfSimpleMeteringClusterPrintln("%d of %d mirrors used.",
                                      emAfPluginMeterMirrorGetMirrorsAllocated(),
                                      EMBER_AF_PLUGIN_METER_MIRROR_MAX_MIRRORS);
}

void emAfPluginMeterMirrorRemoveCommand(void)
{
  uint8_t endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  EmberEUI64 mirrorEui64;
  bool success = emberAfPluginMeterMirrorGetEui64ByEndpoint(endpoint, mirrorEui64);
  if (!success) {
    emberAfSimpleMeteringClusterPrintln("Error: Failed to obtain EUI64 for endpoint %d", endpoint);
    return;
  }
  emberAfPluginSimpleMeteringClientRemoveMirrorCallback(mirrorEui64);
}
#endif
