/***************************************************************************//**
 * @file app_cli.c
 * @brief app_cli.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>
#include "em_chip.h"
#include "sl_component_catalog.h"
#include "sl_cli.h"
#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
#include "sl_zigbee_debug_print.h"
#endif
#include "gpd-components-common.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
#define gpdDebugPrintf sl_zigbee_core_debug_print
#else
#define gpdDebugPrintf(...)
#endif
#define LENGTH_ARG_FOR_GPD_SRC_ID (sizeof(uint8_t) + sizeof(uint32_t))
#define LENGTH_ARG_FOR_GPD_IEEE_ID (sizeof(uint8_t) + sizeof(uint8_t) + EMBER_GPD_EUI64_SIZE)
#define GPD_KEY_SIZE 16
// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
// following function is part of the application implementation.
extern void emberGpdAppSingleEventCommission(void);
/******************************************************************************
 * CLI - info message: Unique ID of the board
 *****************************************************************************/
void emberGpdAfCliResetCommand(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  NVIC_SystemReset();
}

void emberGpdAfCliNodeInfo(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  EmberGpd_t * gpd = emberGpdGetGpd();
  if (gpd->addr.appId == EMBER_GPD_APP_ID_SRC_ID) {
    gpdDebugPrintf("App Id = %d SrcId = 0x%4lx\n", gpd->addr.appId, gpd->addr.id.srcId);
  } else if (gpd->addr.appId == EMBER_GPD_APP_ID_IEEE_ID) {
    uint8_t *ieee = gpd->addr.id.ieee;
    gpdDebugPrintf("App Id = %d Ep = %d IEEE=0x%02X%02X%02X%02X%02X%02X%02X%02X\n",
                   gpd->addr.appId, gpd->addr.gpdEndpoint,
                   ieee[7], ieee[6], ieee[5], ieee[4], ieee[3], ieee[2], ieee[1], ieee[0]);
  }

  gpdDebugPrintf("Auto-Comm=%d\n", gpd->autoCommissioning);
  gpdDebugPrintf("Channel=%d\n", gpd->channel);
  gpdDebugPrintf("GPD State=%d\n", gpd->gpdState);
  gpdDebugPrintf("GPD Bidir=%d\n", gpd->rxAfterTx);
  gpdDebugPrintf("  GPD rxWindow=%d msec\n", gpd->minRxWindow);
  gpdDebugPrintf("  GPD rxOffset=%d msec\n", gpd->rxOffset);
  gpdDebugPrintf("Sec Level=%d\n", gpd->securityLevel);
  gpdDebugPrintf("  Sec KeyType=%d\n", gpd->securityKeyType);
  gpdDebugPrintf("  Sec Key={%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X} \n",
                 gpd->securityKey[0], gpd->securityKey[1], gpd->securityKey[2], gpd->securityKey[3],
                 gpd->securityKey[4], gpd->securityKey[5], gpd->securityKey[6], gpd->securityKey[7],
                 gpd->securityKey[8], gpd->securityKey[9], gpd->securityKey[10], gpd->securityKey[11],
                 gpd->securityKey[12], gpd->securityKey[13], gpd->securityKey[14], gpd->securityKey[15]);
  gpdDebugPrintf("  Sec FC=%ld\n", gpd->securityFrameCounter);
}

void emberGpdAfCliChannelReq(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
}

void emberGpdAfCliCommReq(sl_cli_command_arg_t *arguments)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  uint8_t action = sl_cli_get_argument_uint8(arguments, 0);
  if (action == 1) {
    emberGpdAfPluginCommission(gpd);
  } else if (action == 0) {
    emberGpdAfPluginDeCommission(gpd);
  } else if (action == 0xff) {
    emberGpdAppSingleEventCommission();
  }
}

void emberGpdAfCliNodeSetAddr(sl_cli_command_arg_t *arguments)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  unsigned int argLength = 0;
  uint8_t *str = sl_cli_get_argument_hex(arguments, 0, &argLength);
  // Process only if the length is exactly as needed
  if (argLength == LENGTH_ARG_FOR_GPD_SRC_ID
      || argLength == LENGTH_ARG_FOR_GPD_IEEE_ID) {
    gpd->addr.appId = str[0];
    if (gpd->addr.appId == EMBER_GPD_APP_ID_SRC_ID) {
      gpd->addr.id.srcId = ((uint32_t)str[1] << 24)   \
                           + ((uint32_t)str[2] << 16) \
                           + ((uint32_t)str[3] << 8)  \
                           + str[4];
    } else if (gpd->addr.appId == EMBER_GPD_APP_ID_IEEE_ID) {
      // Endpoint
      gpd->addr.gpdEndpoint = str[1];
      // A simple way to use the command for testing
      memcpy(gpd->addr.id.ieee, &str[2], EMBER_GPD_EUI64_SIZE);
    } else {
      // Error in the input argument
      gpdDebugPrintf("Must be exactly %d or %d bytes hex\n", \
                     LENGTH_ARG_FOR_GPD_SRC_ID, LENGTH_ARG_FOR_GPD_IEEE_ID);
    }
  }
}

void emberGpdAfCliNodeSetAutoComm(sl_cli_command_arg_t *arguments)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  gpd->autoCommissioning = (bool)sl_cli_get_argument_uint8(arguments, 0);
}

void emberGpdAfCliNodeSetBiDirParam(sl_cli_command_arg_t *arguments)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  gpd->rxAfterTx = (bool)sl_cli_get_argument_uint8(arguments, 0);
  if (gpd->rxAfterTx) {
    gpd->rxOffset = sl_cli_get_argument_uint8(arguments, 1);
    gpd->minRxWindow = sl_cli_get_argument_uint8(arguments, 2);
  }
}

void emberGpdAfCliNodeSetChannel(sl_cli_command_arg_t *arguments)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  gpd->channel = sl_cli_get_argument_uint8(arguments, 0);;
}

void emberGpdAfCliNodeSetSecKeyType(sl_cli_command_arg_t *arguments)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  gpd->securityKeyType = sl_cli_get_argument_uint8(arguments, 0);
}

void emberGpdAfCliNodeSetSecKey(sl_cli_command_arg_t *arguments)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  unsigned int argLength;
  uint8_t *key = sl_cli_get_argument_hex(arguments, 0, &argLength);
  if (argLength == GPD_KEY_SIZE) {
    (void) memcpy(gpd->securityKey, key, GPD_KEY_SIZE);
  } else {
    // Error in the input argument
    gpdDebugPrintf("Key must be exactly %d bytes hex\n", GPD_KEY_SIZE);
  }
}

void emberGpdAfCliNodeSetSecFc(sl_cli_command_arg_t *arguments)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  gpd->securityFrameCounter = (uint32_t)sl_cli_get_argument_uint32(arguments, 0);;
}

void emberGpdAfCliNodeSetSecLevel(sl_cli_command_arg_t *arguments)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  gpd->securityLevel = sl_cli_get_argument_uint8(arguments, 0);
}

// Sends a GPDF using the gpd node instance
// <frame type> <command with payload>
void emberGpdAfCliSendGpdf(sl_cli_command_arg_t *arguments)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  uint8_t frameType = sl_cli_get_argument_uint8(arguments, 0);
  unsigned int cmdPayloadLength;
  uint8_t *cmdPayload = sl_cli_get_argument_hex(arguments, 1, &cmdPayloadLength);
  emberAfGpdfSend(frameType, gpd, cmdPayload, cmdPayloadLength, 1);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
