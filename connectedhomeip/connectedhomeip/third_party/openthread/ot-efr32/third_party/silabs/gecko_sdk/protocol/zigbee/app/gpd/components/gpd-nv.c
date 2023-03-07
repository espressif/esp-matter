/***************************************************************************//**
 * @file
 * @brief Routines for non volatile memory interfaces used by the GPD.
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
#include "gpd-components-common.h"
#ifndef UC_BUILD
#include "gpd-callbacks.h"
#else
#include "gpd-cb.h"
#endif

// The NVM array is arranged as follows
//-----------------------------------------------------------------------------
//  0 -- 3  |  4  ---------  19  |  20  |  21  |  22  |  23  |  24-31   |
//    FC    |        KEY         | SecL | SecT |  Ch  | State|  Reserved|
//-----------------------------------------------------------------------------
static uint8_t gpdNvData[EMBER_GPD_NV_DATA_SIZE];

void emberGpdNvInit(void)
{
  emberGpdAfPluginNvInitCallback();
}
// Accessor functions
void emberGpdSaveGpdFrameCounterToShadow(uint32_t frameCounter)
{
  emberGpdUtilityCopy4Bytes(gpdNvData, frameCounter);
}

void emberGpdCopyToShadow(EmberGpd_t * gpd)
{
  emberGpdUtilityCopy4Bytes(gpdNvData, gpd->securityFrameCounter);
  (void) memcpy(&gpdNvData[EMBER_GPD_NV_DATA_GPD_KEY_INDEX], gpd->securityKey, 16);
  gpdNvData[EMBER_GPD_NV_DATA_GPD_SEC_LEVEL_INDEX] = gpd->securityLevel;
  gpdNvData[EMBER_GPD_NV_DATA_GPD_SEC_KEY_TYPE_INDEX] = gpd->securityKeyType;
  gpdNvData[EMBER_GPD_NV_DATA_GPD_CHANNEL_INDEX] = gpd->channel;
  gpdNvData[EMBER_GPD_NV_DATA_GPD_STATE_INDEX] = gpd->gpdState;
}
void emberGpdCopyFromShadow(EmberGpd_t * gpd)
{
  gpd->securityFrameCounter = *((uint32_t *)gpdNvData);
  (void) memcpy(gpd->securityKey, (gpdNvData + EMBER_GPD_NV_DATA_GPD_KEY_INDEX), 16);
  gpd->securityLevel = *(gpdNvData + EMBER_GPD_NV_DATA_GPD_SEC_LEVEL_INDEX);
  gpd->securityKeyType = *(gpdNvData + EMBER_GPD_NV_DATA_GPD_SEC_KEY_TYPE_INDEX);
  gpd->channel = *(gpdNvData + EMBER_GPD_NV_DATA_GPD_CHANNEL_INDEX);
  gpd->gpdState = *(gpdNvData + EMBER_GPD_NV_DATA_GPD_STATE_INDEX);
}
void emberGpdStoreSecDataToNV(EmberGpd_t * gpd)
{
  emberGpdCopyToShadow(gpd);
  emberGpdAfPluginNvSaveAndLoadCallback(gpd,
                                        gpdNvData,
                                        EMBER_GPD_NV_DATA_SIZE,
                                        EMEBER_GPD_AF_CALLBACK_STORE_GPD_TO_NVM);
}

void emberGpdLoadSecDataFromNV(EmberGpd_t * gpd)
{
  emberGpdAfPluginNvSaveAndLoadCallback(gpd,
                                        gpdNvData,
                                        EMBER_GPD_NV_DATA_SIZE,
                                        EMEBER_GPD_AF_CALLBACK_LOAD_GPD_FROM_NVM);
  emberGpdCopyFromShadow(gpd);
}
