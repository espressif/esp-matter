/***************************************************************************//**
 * @file token_legacy.c
 * @brief Legacy Token API support.
 * @version 1.0.0
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_token_manager.h"
#ifdef USE_NVM3
#include "nvm3.h"
#endif // USE_NVM3
#include "ecode.h"
#include "include/error-def.h"

#ifndef ECODE_NVM3_ERR_OPENED_WITH_OTHER_PARAMETERS
  #define ECODE_NVM3_ERR_OPENED_WITH_OTHER_PARAMETERS (ECODE_EMDRV_NVM3_BASE | 0x00000008U)
#endif // ECODE_NVM3_ERR_OPENED_WITH_OTHER_PARAMETERS

#ifndef ECODE_NVM3_ERR_ALIGNMENT_INVALID
  #define ECODE_NVM3_ERR_ALIGNMENT_INVALID            (ECODE_EMDRV_NVM3_BASE | 0x00000001U)
#endif // ECODE_NVM3_ERR_ALIGNMENT_INVALID

#ifndef ECODE_NVM3_ERR_SIZE_TOO_SMALL
  #define ECODE_NVM3_ERR_SIZE_TOO_SMALL               (ECODE_EMDRV_NVM3_BASE | 0x00000002U)
#endif // ECODE_NVM3_ERR_SIZE_TOO_SMALL

#ifndef ECODE_NVM3_ERR_PAGE_SIZE_NOT_SUPPORTED
  #define ECODE_NVM3_ERR_PAGE_SIZE_NOT_SUPPORTED      (ECODE_EMDRV_NVM3_BASE | 0x00000004U)
#endif // ECODE_NVM3_ERR_PAGE_SIZE_NOT_SUPPORTED

#ifndef ECODE_NVM3_OK
  #define ECODE_NVM3_OK                               (ECODE_OK)
#endif // ECODE_NVM3_OK

EmberStatus halStackInitTokens(void)
{
  Ecode_t ecode;
  EmberStatus ret;

  ecode = sl_token_init();

  // Map NVM3 error codes to EmberStatus codes
  switch (ecode) {
    case ECODE_NVM3_ERR_OPENED_WITH_OTHER_PARAMETERS:
      ret = (EmberStatus) EMBER_NVM3_ERR_OPENED_WITH_OTHER_PARAMETERS;
      break;
    case ECODE_NVM3_ERR_ALIGNMENT_INVALID:
      ret = (EmberStatus) EMBER_NVM3_ERR_ALIGNMENT_INVALID;
      break;
    case ECODE_NVM3_ERR_SIZE_TOO_SMALL:
      ret = (EmberStatus) EMBER_NVM3_ERR_SIZE_TOO_SMALL;
      break;
    case ECODE_NVM3_ERR_PAGE_SIZE_NOT_SUPPORTED:
      ret = (EmberStatus) EMBER_NVM3_ERR_PAGE_SIZE_NOT_SUPPORTED;
      break;
    case ECODE_NVM3_OK:
      ret = (EmberStatus) EMBER_SUCCESS;
      break;
    default:
      ret = (EmberStatus) EMBER_NVM3_ERR_TOKEN_INIT;
      break;
  }

  return ret;
}

void halInternalGetTokenData(void *data, uint16_t token, uint8_t index, uint8_t len)
{
  (void) sl_token_get_data(token, index, data, len);
}

void halInternalSetTokenData(uint16_t token, uint8_t index, void *data, uint8_t len)
{
  (void) sl_token_set_data(token, index, data, len);
}

void halInternalIncrementCounterToken(uint8_t token)
{
  (void) sl_token_increment_counter(token);
}

void halInternalGetMfgTokenData(void *data, uint16_t token, uint8_t index, uint32_t len)
{
  (void) sl_token_get_manufacturing_data(token, index, data, len);
}

void halInternalSetMfgTokenData(uint16_t token, void *data, uint32_t len)
{
  (void) sl_token_set_manufacturing_data(token, data, len);
}

//Link the public API to the private internal instance.
#ifndef halCommonGetMfgToken
#define halCommonGetMfgToken(data, token)
halInternalGetMfgTokenData(data, token, 0x7F, token##_SIZE)
#endif

#ifndef halCommonSetMfgTokenData
#define halCommonSetMfgTokenData(token, data, size) \
  halInternalSetMfgTokenData(token, data, size)
#endif
