/***************************************************************************//**
 * @file
 * @brief ZLL test harness functions for the Test Harness Z3 plugin.
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

//
// test-harness-z3-zll.c
//
// Wednesday, December 9, 2015
//
// ZigBee 3.0 touchlink test harness functionality
//

// -----------------------------------------------------------------------------
// Internal constants

#define EM_AF_PLUGIN_TEST_HARNESS_Z3_ZLL_SERVER_TO_CLIENT_FRAME_CONTROL \
  (ZCL_CLUSTER_SPECIFIC_COMMAND                                         \
   | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT                                 \
   | ZCL_DISABLE_DEFAULT_RESPONSE_MASK)
#define EM_AF_PLUGIN_TEST_HARNESS_Z3_ZLL_CLIENT_TO_SERVER_FRAME_CONTROL \
  (ZCL_CLUSTER_SPECIFIC_COMMAND                                         \
   | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER                                 \
   | ZCL_DISABLE_DEFAULT_RESPONSE_MASK)

// -----------------------------------------------------------------------------
// Framework-internal callbacks

void emAfPluginTestHarnessZ3ZllStackStatusCallback(EmberStatus status);

void emAfPluginTestHarnessZ3ZllNetworkFoundCallback(const EmberZllNetwork *networkInfo);
void emAfPluginTestHarnessZ3ZllScanCompleteCallback(EmberStatus status);
EmberPacketAction emAfPluginTestHarnessZ3ZllCommandCallback(uint8_t *command,
                                                            EmberEUI64 sourceEui64);
EmberPacketAction emAfPluginTestHarnessZ3ZllModifyInterpanCommand(uint8_t *commandData,
                                                                  uint8_t macHeaderLength);
