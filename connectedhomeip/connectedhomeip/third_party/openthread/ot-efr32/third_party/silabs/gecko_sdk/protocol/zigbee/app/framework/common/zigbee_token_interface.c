/***************************************************************************//**
 * @brief ZigBee Application Framework token management code.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#include PLATFORM_HEADER
#include "hal.h"
#include "stack/include/ember.h"
#include "sl_component_catalog.h"

#if (defined(SL_CATALOG_TOKEN_MANAGER_PRESENT))

#include "sl_token_api.h"
#include "sl_token_manager.h"

// The following interfaces are wrapper on top of platform service token manager
// APIs. The reason for providing an access to token get and set from a host is to
// allow an NCP-Host application to completely backup the token area and restore
// on a new hardware.

#if (defined(USE_NVM3))
// The following implementation is based on NVM3 tokens.
uint8_t emberGetTokenCount(void)
{
  return (TOKEN_COUNT);
}

EmberStatus emberGetTokenInfo(uint8_t index,
                              EmberTokenInfo *tokenInfo)
{
  if (index >= (TOKEN_COUNT)) {
    return EMBER_INDEX_OUT_OF_RANGE;
  }
  tokenInfo->nvm3Key = tokenNvm3Keys[index];
  tokenInfo->isIdx = tokenIsIdx[index];
  tokenInfo->isCnt = tokenIsCnt[index];
  tokenInfo->size = tokenSize[index];
  tokenInfo->arraySize = tokenArraySize[index];

  return EMBER_SUCCESS;
}

EmberStatus emberGetTokenData(uint32_t token,
                              uint32_t index,
                              EmberTokenData *tokenData)
{
  // Look up the token size from the token key because
  // sl_token_get_data needs the size to be passed.
  for (uint8_t i = 0; i < emberGetTokenCount(); i++) {
    if (token == tokenNvm3Keys[i]) {
      tokenData->size = tokenSize[i];
      Ecode_t s = sl_token_get_data(token,
                                    index,
                                    tokenData->data,
                                    tokenData->size);
      return (s == ECODE_OK ? EMBER_SUCCESS : EMBER_ERR_FATAL);
    }
  }
  tokenData->size = 0;
  return EMBER_ERR_FATAL;
}

EmberStatus emberSetTokenData(uint32_t token,
                              uint32_t index,
                              EmberTokenData *tokenData)
{
  Ecode_t s = sl_token_set_data(token,
                                index,
                                tokenData->data,
                                tokenData->size);
  if (s != ECODE_OK) {
    tokenData->size = 0;
  }
  return (s == ECODE_OK ? EMBER_SUCCESS : EMBER_ERR_FATAL);
}

// Strong implementation to restore the EUI64 incase backup/restore
void emberGetRestoredEui64(EmberEUI64 eui64)
{
  uint8_t blank[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  uint8_t restoredEui64[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  EmberTokenData tokenData;
  tokenData.size = 0;
  tokenData.data = (void *)restoredEui64;
  EmberStatus status = emberGetTokenData(NVM3KEY_STACK_RESTORED_EUI64,
                                         0,
                                         &tokenData);
  if (status == EMBER_SUCCESS
      && tokenData.size == sizeof(EmberEUI64)) {
    if (0 == MEMCOMPARE(blank,
                        restoredEui64,
                        sizeof(EmberEUI64))) {
      // There is no restored EUI64, no action
    } else {
      // There is a restored EUI available, so use that.
      MEMCOPY(eui64, restoredEui64, sizeof(EmberEUI64));
    }
  }
}
#endif // USE_NVM3
// If not NVM3, for any other NVM variant such as SimEE 2 it is not supported.
// The APis are stubbed as weak functions, hence application can override.
#endif // SL_CATALOG_TOKEN_MANAGER_PRESENT

// Extended to support the zigbeed that uses host token.
#if defined(SL_CATALOG_ZIGBEE_STACK_UNIX_PRESENT)
#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "platform/service/legacy_host/inc/token.h"
#include <syslog.h>

extern const uint16_t tokenCreators[];
extern const bool tokenIsCnt[];
extern const uint8_t tokenSize[];
extern const uint8_t tokenArraySize[];
extern const void * const tokenDefaults[];

// From the token interface
uint8_t emberGetTokenCount(void)
{
  return (TOKEN_COUNT);
}

EmberStatus emberGetTokenInfo(uint8_t index,
                              EmberTokenInfo *tokenInfo)
{
  if (index >= (TOKEN_COUNT)) {
    return EMBER_INDEX_OUT_OF_RANGE;
  }
  tokenInfo->nvm3Key = tokenCreators[index];
  tokenInfo->isIdx = 1;
  tokenInfo->isCnt = tokenIsCnt[index];
  tokenInfo->size = tokenSize[index];
  tokenInfo->arraySize = tokenArraySize[index];
  return EMBER_SUCCESS;
}

EmberStatus emberGetTokenData(uint32_t token,
                              uint32_t index,
                              EmberTokenData *tokenData)
{
  // Look up the token size from the token key because
  // sl_token_get_data needs the size to be passed.
  for (uint16_t i = 0; i < emberGetTokenCount(); i++) {
    if (token == tokenCreators[i]) {
      tokenData->size = tokenSize[i];
      //syslog(LOG_INFO, "Getting : Creator = %04X Token = %d index = %d tokenData->size = %d",token, i, index, tokenData->size);
      halInternalGetTokenData(tokenData->data, i, index, tokenData->size);
      return EMBER_SUCCESS;
    }
  }
  tokenData->size = 0;
  return EMBER_ERR_FATAL;
}

EmberStatus emberSetTokenData(uint32_t token,
                              uint32_t index,
                              EmberTokenData *tokenData)
{
  for (uint8_t i = 0; i < emberGetTokenCount(); i++) {
    if (token == tokenCreators[i]) {
      //syslog(LOG_INFO, "Setting : Creator = %04X Token = %d index = %d tokenData->size = %d",token, i, index, tokenData->size);
      halInternalSetTokenData(i, index, tokenData->data, tokenData->size);
      return EMBER_SUCCESS;
    }
  }
  return EMBER_ERR_FATAL;
}
#endif
