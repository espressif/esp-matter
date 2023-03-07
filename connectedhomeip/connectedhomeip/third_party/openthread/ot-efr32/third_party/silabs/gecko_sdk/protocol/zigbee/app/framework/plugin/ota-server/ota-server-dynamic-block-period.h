/***************************************************************************//**
 * @file
 * @brief APIs and defines for the dynamic block request feature of the OTA
 *        Server plugin.
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

#ifndef OTA_SERVER_DYNAMIC_BLOCK_PERIOD_H
#define OTA_SERVER_DYNAMIC_BLOCK_PERIOD_H

#include "ota-server.h"

#ifdef DYNAMIC_MIN_BLOCK_PERIOD_SUPPORT
void    emAfOtaServerDynamicBlockPeriodInit();
void    emAfOtaServerDynamicBlockPeriodTick();
uint8_t emAfOtaServerCheckDynamicBlockPeriodDownload(EmberAfImageBlockRequestCallbackStruct *data);
void    emAfOtaServerCompleteDynamicBlockPeriodDownload(EmberNodeId clientId);
bool    emAfOtaServerDynamicBlockPeriodClientUsesSeconds(EmberNodeId clientId);
#else // DYNAMIC_MIN_BLOCK_PERIOD_SUPPORT
// Stubs
  #define emAfOtaServerDynamicBlockPeriodInit()
  #define emAfOtaServerDynamicBlockPeriodTick()
  #define emAfOtaServerCheckDynamicBlockPeriodDownload(arg) \
  EMBER_ZCL_STATUS_SUCCESS
  #define emAfOtaServerCompleteDynamicBlockPeriodDownload(arg)
  #define emAfOtaServerDynamicBlockPeriodClientUsesSeconds(arg) false
#endif // DYNAMIC_MIN_BLOCK_PERIOD_SUPPORT

#endif // OTA_SERVER_DYNAMIC_BLOCK_PERIOD_H
