/***************************************************************************//**
 * @file
 * @brief Routines for the Find and Bind Target plugin.
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

#ifdef UC_BUILD
#include "find-and-bind-target-config.h"
#endif

#include "find-and-bind-target.h"

#ifdef EMBER_SCRIPTED_TEST
  #include "../find-and-bind-initiator/find-and-bind-test-configuration.h"
#endif

// -----------------------------------------------------------------------------
// Public API

EmberAfStatus emberAfPluginFindAndBindTargetStart(uint8_t endpoint)
{
  // Write the identify time.
  uint16_t identifyTime = EMBER_AF_PLUGIN_FIND_AND_BIND_TARGET_COMMISSIONING_TIME;
  EmberAfStatus status = EMBER_ZCL_STATUS_UNSUPPORTED_ATTRIBUTE;

  if (emberAfContainsServer(endpoint, ZCL_IDENTIFY_CLUSTER_ID)) {
    status = emberAfWriteServerAttribute(endpoint,
                                         ZCL_IDENTIFY_CLUSTER_ID,
                                         ZCL_IDENTIFY_TIME_ATTRIBUTE_ID,
                                         (uint8_t *)&identifyTime,
                                         ZCL_INT16U_ATTRIBUTE_TYPE);
  }

  emberAfCorePrintln("%p: %p: 0x%X",
                     EMBER_AF_PLUGIN_FIND_AND_BIND_TARGET_PLUGIN_NAME,
                     "Start target",
                     status);

  return status;
}
