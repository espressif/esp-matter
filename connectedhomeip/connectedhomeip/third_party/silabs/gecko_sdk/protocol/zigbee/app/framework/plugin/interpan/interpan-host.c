/***************************************************************************//**
 * @file
 * @brief Host-specific code related to the reception and processing of interpan
 * messages.
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
#include "interpan.h"
#include "app/framework/util/af-main.h"

//------------------------------------------------------------------------------

void ezspMacFilterMatchMessageHandler(uint8_t filterIndexMatch,
                                      EmberMacPassthroughType legacyPassthroughType,
                                      uint8_t lastHopLqi,
                                      int8_t lastHopRssi,
                                      uint8_t messageLength,
                                      uint8_t *messageContents)
{
  emAfPluginInterpanProcessMessage(messageLength,
                                   messageContents);
}

EmberStatus emAfPluginInterpanSendRawMessage(uint8_t length, uint8_t* message)
{
  return ezspSendRawMessage(length, message);
}

#ifdef UC_BUILD
void emberAfPluginInterpanNcpInitCallback(bool memoryAllocation)
{
  interpanPluginInit(SL_ZIGBEE_INIT_LEVEL_EVENT);
  interpanPluginInit(SL_ZIGBEE_INIT_LEVEL_LOCAL_DATA);

  EmberMacFilterMatchData filters[] = {
    EMBER_AF_PLUGIN_INTERPAN_FILTER_LIST
  };
  EzspStatus status;
  if (memoryAllocation) {
    status = ezspSetConfigurationValue(EZSP_CONFIG_MAC_FILTER_TABLE_SIZE,
                                       (sizeof(filters)
                                        / sizeof(EmberMacFilterMatchData)) + 1);
    if (status != EZSP_SUCCESS) {
      emberAfAppPrintln("%s%s failed 0x%02X",
                        "Error: ",
                        "Sizing MAC filter table",
                        status);
      return;
    }
  } else {
    interpanPluginSetMacMatchFilterEnable(true);
  }
}

#else // !UC_BUILD

void emberAfPluginInterpanNcpInitCallback(bool memoryAllocation)
{
  interpanPluginInit();

  EmberMacFilterMatchData filters[] = {
    EMBER_AF_PLUGIN_INTERPAN_FILTER_LIST
  };
  EzspStatus status;

  if (memoryAllocation) {
    status = ezspSetConfigurationValue(EZSP_CONFIG_MAC_FILTER_TABLE_SIZE,
                                       (sizeof(filters)
                                        / sizeof(EmberMacFilterMatchData)) + 1);
    if (status != EZSP_SUCCESS) {
      emberAfAppPrintln("%p%p failed 0x%x",
                        "Error: ",
                        "Sizing MAC filter table",
                        status);
      return;
    }
  } else {
    interpanPluginSetMacMatchFilterEnable(true);
  }
}
#endif // UC_BUILD

#ifndef UC_BUILD
void emberAfPluginInterpanInitCallback(SLXU_INIT_ARG)
{
  SLXU_INIT_UNUSED_ARG;
}
#endif // UC_BUILD

void interpanPluginSetMacMatchFilterEnable(bool enable)
{
  EmberMacFilterMatchData filters[] = {
    EMBER_AF_PLUGIN_INTERPAN_FILTER_LIST
  };

  uint8_t value[2 * sizeof(filters) / sizeof(EmberMacFilterMatchData)] = { 0 };

  if (enable) {
    uint8_t i;
    for (i = 0; i < sizeof(value) / 2; i++) {
      value[i * 2]     =  LOW_BYTE(filters[i]);
      value[i * 2 + 1] = HIGH_BYTE(filters[i]);
    }
  }

  EmberStatus status = ezspSetValue(EZSP_VALUE_MAC_FILTER_LIST,
                                    sizeof(value), value);
  if (status != EZSP_SUCCESS) {
    emberAfAppPrintln("ERR: failed %s inter-PAN MAC filter (0x%x)",
                      enable ? "enabling" : "disabling",
                      status);
  }
}

EmberStatus emAfInterpanApsCryptMessage(bool encrypt,
                                        uint8_t* message,
                                        uint8_t* messageLength,
                                        uint8_t apsHeaderEndIndex,
                                        EmberEUI64 remoteEui64)
{
#if defined(ALLOW_APS_ENCRYPTED_MESSAGES)
  #error Not supported by EZSP
#endif

  // Feature not yet supported on EZSP.
  return EMBER_LIBRARY_NOT_PRESENT;
}
