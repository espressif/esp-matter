/***************************************************************************//**
 * @file sl_token_manager.c
 * @brief TOKEN_MANAGER API implementation.
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

#include <stdint.h>
#include <stdbool.h>
#include "sl_token_api.h"
#include "sl_token_manager.h"
#include "sl_token_manufacturing_api.h"
#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT
#if defined(SL_CATALOG_SIM_EEPROM2_TO_NVM3_UPGRADE_PRESENT)
#include "sl_sim_eeprom2_to_nvm3_upgrade.h"
#endif // SL_CATALOG_SIM_EEPROM2_TO_NVM3_UPGRADE_PRESENT

//prints debug data from the token access functions
#define TOKENDBG(x)
//#include "printf.h"
//int UnityPrintf(const char *format, ...);
//#define TOKENDBG(x) x

static bool tokensActive = false;

#if defined(USE_NVM3)
#include "nvm3.h"

__WEAK void halNvm3Callback(Ecode_t status)
{
  // NVM3 does error handling internally, but this callback is included
  // in case the application requires any custom error handling.
  (void) status;
}

static uint32_t remapTokenToTokenNvm3KeysIndex(uint32_t token)
{
  uint32_t i = 0;

  if (token >= TOKEN_COUNT) {
    for (i = 0; (i < nvm3_countObjects(nvm3_defaultHandle)) && (i < TOKEN_COUNT); i++) {
      if (token == tokenNvm3Keys[i]) {
        TOKENDBG(UnityPrintf("REMAPPING token=%0lx to index %0lx (tokenNvm3Keys[%0lx]=%0lx)\n", token, i, i, tokenNvm3Keys[i]); )
        return i;
      }
    }
  }
  return token;
}

// Global variable used by library code to know number of tokens
#ifdef SIMEE2_TO_NVM3_UPGRADE
uint8_t tokenCount = TOKEN_COUNT;
#endif // SIMEE2_TO_NVM3_UPGRADE

#ifdef SIMEE2_TO_NVM3_UPGRADE
extern uint32_t linker_nvm_begin;

#ifndef NVM3_BASE

#ifndef NVM3_DEFAULT_CACHE_SIZE
#define NVM3_DEFAULT_CACHE_SIZE  100
#endif
#ifndef NVM3_DEFAULT_NVM_SIZE
#define NVM3_DEFAULT_NVM_SIZE  36864
#endif
#ifndef NVM3_DEFAULT_MAX_OBJECT_SIZE
#define NVM3_DEFAULT_MAX_OBJECT_SIZE  NVM3_MAX_OBJECT_SIZE
#endif
#ifndef NVM3_DEFAULT_REPACK_HEADROOM
#define NVM3_DEFAULT_REPACK_HEADROOM  0
#endif

#if defined (__ICCARM__)

#ifndef __NVM3__
#define __NVM3__ "SIMEE"
#endif

__root uint8_t nvm3Storage[NVM3_DEFAULT_NVM_SIZE] @ __NVM3__;
#define NVM3_BASE (nvm3Storage)

#elif defined (__GNUC__)

#ifndef __NVM3__
#define __NVM3__ ".simee"
#endif

__attribute__((used)) uint8_t nvm3Storage[NVM3_DEFAULT_NVM_SIZE] __attribute__ ((section(__NVM3__)));
/* If linker does not provide __nvm3Base symbol, then use nvm3Storage*/
extern char __nvm3Base __attribute__((alias("nvm3Storage")));
#define NVM3_BASE (&__nvm3Base)

#else
#error "Unsupported toolchain"
#endif

#endif //NVM3_BASE

uint8_t       *nvm3Address = (uint8_t *)NVM3_BASE;
nvm3_Handle_t  nvm3_defaultHandleData;
nvm3_Handle_t *nvm3_defaultHandle = &nvm3_defaultHandleData;

static nvm3_CacheEntry_t defaultCache[NVM3_DEFAULT_CACHE_SIZE];

#include "nvm3_hal_flash.h"
nvm3_Init_t    nvm3_defaultInitData =
{
  (nvm3_HalPtr_t)NVM3_BASE,
  NVM3_DEFAULT_NVM_SIZE,
  defaultCache,
  NVM3_DEFAULT_CACHE_SIZE,
  NVM3_DEFAULT_MAX_OBJECT_SIZE,
  NVM3_DEFAULT_REPACK_HEADROOM,
  &nvm3_halFlashHandle,
};

nvm3_Init_t   *nvm3_defaultInit = &nvm3_defaultInitData;
#endif // SIMEE2_TO_NVM3_UPGRADE

/**************************************************************************//**
 * Initializes the Token Manager.
 *****************************************************************************/
Ecode_t sl_token_init(void)
{
  uint8_t i;
  uint8_t arrayIndex;
  size_t size;
  uint32_t objectType;
  Ecode_t ecode, status;
  nvm3_HalInfo_t halInfo;

  // Upgrade is only supported when using NVM3 in internal flash
#if defined(SL_CATALOG_SIM_EEPROM2_TO_NVM3_UPGRADE_PRESENT)
  #if defined(__GNUC__)
  nvm3Address = (uint8_t *)&linker_nvm_begin;
  #endif // __GNUC__
  if (halSimEeToNvm3Upgrade()) {
    return ECODE_NVM3_ERR_NOT_OPENED;
  }
#endif // SL_CATALOG_SIM_EEPROM2_TO_NVM3_UPGRADE_PRESENT

  ecode = nvm3_open(nvm3_defaultHandle, nvm3_defaultInit);
  TOKENDBG(printf("halStackInitTokens nvm3_open status: 0x%0lx\r\n", ecode); )
  if (ecode != ECODE_NVM3_OK) {
    (void) nvm3_halOpen(nvm3_defaultInit->halHandle, nvm3_defaultInit->nvmAdr, nvm3_defaultInit->nvmSize);
    (void) nvm3_halGetInfo(nvm3_defaultInit->halHandle, &halInfo);
    (void) nvm3_halNvmAccess(nvm3_defaultInit->halHandle, NVM3_HAL_NVM_ACCESS_RDWR);
    for (i = 0; i < nvm3_defaultInit->nvmSize / halInfo.pageSize; i++) {
      ecode = nvm3_halPageErase(nvm3_defaultInit->halHandle,
                                (nvm3_HalPtr_t)((size_t) nvm3_defaultInit->nvmAdr + i * halInfo.pageSize));
      TOKENDBG(printf("nvm3_halPageErase %d status: 0x%0lx\r\n",
                      i,
                      ecode); )
    }
    (void) nvm3_halNvmAccess(nvm3_defaultInit->halHandle, NVM3_HAL_NVM_ACCESS_NONE);
    (void) nvm3_halClose(nvm3_defaultInit->halHandle);

    ecode = nvm3_open(nvm3_defaultHandle, nvm3_defaultInit);
    TOKENDBG(printf("halStackInitTokens second nvm3_open status: 0x%0lx\r\n",
                    ecode); )
  }

  for (i = 0; i < (uint8_t) TOKEN_COUNT; i++) {
    if (ecode == ECODE_NVM3_OK) {
      TOKENDBG(printf("Token[%d]: Key=0x%lx Array size =%d \r\n",
                      i,
                      tokenNvm3Keys[i],
                      tokenArraySize[i]); )

      if (tokenIsCnt[i]) { // Counter token
        status = nvm3_getObjectInfo(nvm3_defaultHandle,
                                    tokenNvm3Keys[i],
                                    &objectType,
                                    &size);

        // Create NVM3 objects for tokens if they don't exist already
        // or if type or size doesn't match
        if ((status == ECODE_NVM3_ERR_KEY_NOT_FOUND)
            || ((status == ECODE_NVM3_OK)
                && (objectType != NVM3_OBJECTTYPE_COUNTER))) {
          TOKENDBG(printf("Creating counter token from default value\r\n"); )

          // If object type is wrong we need to delete it before creating the
          // new one with the correct type.
          if (status == ECODE_NVM3_OK) {
            (void) nvm3_deleteObject(nvm3_defaultHandle, tokenNvm3Keys[i]);
            TOKENDBG(printf("Deleting object of wrong type\r\n"); )
          }

          ecode = nvm3_writeCounter(nvm3_defaultHandle,
                                    tokenNvm3Keys[i],
                                    *(const uint32_t *) tokenDefaults[i]);
          TOKENDBG(printf("halStackInitTokens nvm3_writeCounter value: 0x%0lx status: 0x%0lx\r\n",
                          *(uint32_t *) tokenDefaults[i],
                          ecode); )
        }
      } else { // Basic or indexed token
        // For indexed tokens we need to create every index in the indexed
        // token key domain if it doesn't exist already or if size or type doesn't match
        for (arrayIndex = 0; arrayIndex < tokenArraySize[i]; arrayIndex++) {
          status = nvm3_getObjectInfo(nvm3_defaultHandle,
                                      tokenNvm3Keys[i] + arrayIndex,
                                      &objectType,
                                      &size);

          if ((status == ECODE_NVM3_ERR_KEY_NOT_FOUND)
              || ((status == ECODE_NVM3_OK)
                  && ((objectType != NVM3_OBJECTTYPE_DATA)
                      || (size != tokenSize[i])))) {
            TOKENDBG(printf("Creating basic/index token (index: %x) from default value\r\n", arrayIndex); )

            // If the object type is wrong we need to delete it before creating the
            // new one with the correct type.
            // Wrong size will be corrected by the data write itself
            if (((status == ECODE_NVM3_OK)
                 && ((objectType != NVM3_OBJECTTYPE_DATA)))) {
              (void) nvm3_deleteObject(nvm3_defaultHandle, tokenNvm3Keys[i] + arrayIndex);
              TOKENDBG(printf("Deleting object of wrong type\r\n"); )
            }

            ecode = nvm3_writeData(nvm3_defaultHandle,
                                   tokenNvm3Keys[i] + arrayIndex,
                                   (const void *) tokenDefaults[i],
                                   tokenSize[i]);
            TOKENDBG(printf("halStackInitTokens nvm3_writeData length: %d  status: 0x%0lx\r\n",
                            tokenSize[i],
                            ecode); )
            if (ecode != ECODE_NVM3_OK) {
              break;
            }
          }
        }

        if (tokenIsIdx[i]) {
          // We need to delete objects for any indexes that are no longer used.
          for (arrayIndex = tokenArraySize[i]; arrayIndex < 0x7FU; arrayIndex++) {
            status = nvm3_getObjectInfo(nvm3_defaultHandle,
                                        tokenNvm3Keys[i] + arrayIndex,
                                        &objectType,
                                        &size);

            if (status == ECODE_NVM3_ERR_KEY_NOT_FOUND) {
              // If we don't find an object we assume there are no objects at higher indexes
              // and stop looking.
              break;
            } else {
              ecode = nvm3_deleteObject(nvm3_defaultHandle, tokenNvm3Keys[i] + arrayIndex);
              TOKENDBG(printf("halStackInitTokens Erased unused index %d of token with key 0x%0lx\r\n",
                              arrayIndex,
                              tokenNvm3Keys[i]); )
              if (ecode != ECODE_NVM3_OK) {
                break;
              }
            }
          }
        }
      }
    }
  }

  tokensActive = ecode == ECODE_NVM3_OK;

  // If the NVM3 cache overflows it is too small to index all live and deleted NVM3 objects
  assert(!nvm3_defaultHandle->cache.overflow);

  TOKENDBG(printf("\nnumKeys=%d\n\n", nvm3_countObjects(nvm3_defaultHandle)); )
  return ecode;
}

/***************************************************************************//**
 * Read the data stored in the specified data or manufacturing token.
 ******************************************************************************/
Ecode_t sl_token_get_data(uint32_t token,
                          uint32_t index,
                          void *data,
                          uint32_t length)
{
  uint8_t offset;
  Ecode_t ecode = ECODE_NVM3_OK;

  TOKENDBG(printf("Get token[%0lx]: Key=0x%0lx Index= 0x%0lx  length= 0x%0lx\r\n",
                  token,
                  tokenNvm3Keys[token],
                  index,
                  length); )

  token = remapTokenToTokenNvm3KeysIndex(token);

  if ( token < TOKEN_COUNT) {
    if (tokensActive) {
      if (tokenIsCnt[token]) {
        ecode = nvm3_readCounter(nvm3_defaultHandle,
                                 tokenNvm3Keys[token],
                                 data);
        TOKENDBG(printf("halInternalGetTokenData nvm3_readCounter value: 0x%0lx status: 0x%0lx\r\n",
                        *(uint32_t *) data,
                        ecode); )
      } else { // Basic/indexed token
        offset = index == 0x7FU ? 0U : index;
        ecode = nvm3_readData(nvm3_defaultHandle,
                              tokenNvm3Keys[token] + offset,
                              data,
                              length);
        TOKENDBG(printf("halInternalGetTokenData nvm3_readData length: %0lx status: 0x%0lx\r\n",
                        length,
                        ecode); )
      }
    } else {
      TOKENDBG(printf("halInternalGetTokenData supressed\r\n"); )
    }
  } else if (token >= 256U) {
    #ifdef EMBER_TEST
    assert(false);
    #else //EMBER_TEST
    halInternalGetMfgTokenData(data, token, index, length);
    #endif // EMBER_TEST
  } else {
    assert(false);
  }

  if (ecode != ECODE_NVM3_OK) {
    halNvm3Callback(ecode);
  }

  return ecode;
}

/***************************************************************************//**
 * Writes the data to the specified token.
 ******************************************************************************/
Ecode_t sl_token_set_data(uint32_t token,
                          uint32_t index,
                          void *data,
                          uint32_t length)
{
  uint8_t offset;
  Ecode_t ecode = ECODE_NVM3_OK;

  TOKENDBG(printf("Set token[%0lx]: tokenNvm3Keys[token]=0x%0lx Index= 0x%0lx  length= 0x%0lx\r\n",
                  token,
                  tokenNvm3Keys[token],
                  index,
                  length); )

  token = remapTokenToTokenNvm3KeysIndex(token);

  if ( token < TOKEN_COUNT) {
    if (tokensActive) {
      if (tokenIsCnt[token]) {
        ecode = nvm3_writeCounter(nvm3_defaultHandle,
                                  tokenNvm3Keys[token],
                                  *(uint32_t *) data);
        TOKENDBG(printf("halInternalSetTokenData nvm3_writeCounter value: 0x%0lx status: 0x%0lx\r\n",
                        *(uint32_t *) data,
                        ecode); )
      } else { //Basic/indexed token
        offset = index == 0x7FU ? 0U : index;
        ecode = nvm3_writeData(nvm3_defaultHandle,
                               tokenNvm3Keys[token] + offset,
                               data,
                               length);
        TOKENDBG(printf("halInternalSetTokenData nvm3_writeData length: %0lx status: 0x%0lx\r\n",
                        length,
                        ecode); )
      }
    } else {
      TOKENDBG(printf("halInternalSetTokenData supressed\r\n"); )
    }
  } else if (token >= 256U) {
    #ifdef EMBER_TEST
    assert(false);
    #else //EMBER_TEST
    halInternalSetMfgTokenData(token, data, length);
    #endif //EMBER_TEST
  } else {
    assert(false);
  }
  if (ecode != ECODE_NVM3_OK) {
    halNvm3Callback(ecode);
  }

  return ecode;
}

/***************************************************************************//**
 * Increments the value of a counter token.  This call does not support
 * manufacturing tokens.
 ******************************************************************************/
Ecode_t sl_token_increment_counter(uint32_t token)
{
  Ecode_t ecode = ECODE_NVM3_OK;

  token = remapTokenToTokenNvm3KeysIndex(token);

  if (tokensActive) {
    TOKENDBG(printf("Increment token[%0lx]: tokenNvm3Keys[token]=0x%0lx\r\n",
                    token,
                    tokenNvm3Keys[token]); )
    ecode = nvm3_incrementCounter(nvm3_defaultHandle, tokenNvm3Keys[token], NULL);
    TOKENDBG(printf("halInternalIncrementCounterToken nvm3_incrementCounter status: 0x%0lx\r\n",
                    ecode); )
  } else {
    TOKENDBG(printf("halInternalIncrementCounterToken supressed\r\n"); )
  }
  if (ecode != ECODE_NVM3_OK) {
    halNvm3Callback(ecode);
  }

  return ecode;
}

#else // defined(USE_NVM3)

#include "include/error-def.h"
#include "sl_sim_eeprom.h"

#ifndef ECODE_NVM3_ERR_NOT_OPENED
  #define ECODE_NVM3_ERR_NOT_OPENED (ECODE_EMDRV_NVM3_BASE | 0x00000007U)
#endif

Ecode_t sl_token_init(void)
{
  tokensActive = true;
  if (halInternalSimEeInit() != EMBER_SUCCESS) {
    TOKENDBG(emberSerialPrintf(SER232, "halInternalSimEeInit Attempt 1 fail\r\n"); )
    if (halInternalSimEeInit() != EMBER_SUCCESS) {
      TOKENDBG(emberSerialPrintf(SER232, "halInternalSimEeInit Attempt 2 fail\r\n"); )
      tokensActive = false;
      return EMBER_SIM_EEPROM_INIT_2_FAILED;
    }
  }
  TOKENDBG(emberSerialPrintf(SER232, "halInternalSimEeInit() == EMBER_SUCCESS\n"); )

 #if !defined(BOOTLOADER) && !defined(EMBER_TEST)
  {
    tokTypeStackNvdataVersion tokStack;
    halCommonGetToken(&tokStack, TOKEN_STACK_NVDATA_VERSION);

    if (CURRENT_STACK_TOKEN_VERSION != tokStack) {
 #if defined(DEBUG)
      //Debug images with a bad stack token version should attempt to
      //fix the SimEE before continuing on.
      TOKENDBG(emberSerialPrintf(SER232, "Stack Version mismatch, reloading\r\n"); )
      halInternalSimEeRepair(true);
      if (halInternalSimEeInit() != EMBER_SUCCESS) {
        TOKENDBG(emberSerialPrintf(SER232, "halInternalSimEeInit Attempt 3 fail\r\n"); )
        tokensActive = false;
        return EMBER_SIM_EEPROM_INIT_3_FAILED;
      }
 #else //  node release image
      TOKENDBG(emberSerialPrintf(SER232, "EEPROM_x_VERSION_MISMATCH (%d)\r\n",
                                 EMBER_EEPROM_STACK_VERSION_MISMATCH); )
      tokensActive = false;
 #if defined(PLATFORMTEST) || defined(LEVEL_ONE_TEST)
      tokensActive = true;      //Keep tokens active for test code.
 #endif //defined(PLATFORMTEST)
      return EMBER_EEPROM_STACK_VERSION_MISMATCH;
 #endif
    }
  }
 #endif //!defined(BOOTLOADER) && !defined(EMBER_TEST)

  return ECODE_OK;
}

Ecode_t sl_token_get_data(uint32_t token,
                          uint32_t index,
                          void *data,
                          uint32_t length)
{
  if (token < TOKEN_COUNT) {
    //the token ID is within the SimEEPROM's range, route to the SimEEPROM
    if (tokensActive) {
      halInternalSimEeGetData(data, token, index, length);
    } else {
      TOKENDBG(emberSerialPrintf(SER232, "getIdxToken supressed.\r\n"); )
      // Tokens not being active is fatal for this function.
      return ECODE_NVM3_ERR_NOT_OPENED;
    }
  } else if (token >= 256U) {
    #ifdef EMBER_TEST
    assert(false);
    #else //EMBER_TEST
    halInternalGetMfgTokenData(data, token, index, length);
    #endif //EMBER_TEST
  } else {
    assert(false);
  }

  return ECODE_OK;
}

bool simEeSetDataActiveSemaphore = false;
Ecode_t sl_token_set_data(uint32_t token,
                          uint32_t index,
                          void *data,
                          uint32_t length)
{
  if (token < TOKEN_COUNT) {
    //the token ID is within the SimEEPROM's range, route to the SimEEPROM
    if (tokensActive) {
      //You cannot interrupt SetData with another SetData!
      assert(!simEeSetDataActiveSemaphore);
      simEeSetDataActiveSemaphore = true;
      halInternalSimEeSetData(token, data, index, length);
      simEeSetDataActiveSemaphore = false;
    } else {
      TOKENDBG(emberSerialPrintf(SER232, "setIdxToken supressed.\r\n"); )
      // Tokens not being active is fatal for this function.
      return ECODE_NVM3_ERR_NOT_OPENED;
    }
  } else if (token >= 256) {
    //Something is making a set token call on a manufacturing token,
    //on efm32 we permit this if the token is otherwise unprogrammed
    halInternalSetMfgTokenData(token, data, length);
  } else {
    assert(false);
  }

  return ECODE_OK;
}

Ecode_t sl_token_increment_counter(uint32_t token)
{
  if (tokensActive) {
    halInternalSimEeIncrementCounter(token);
  } else {
    TOKENDBG(emberSerialPrintf(SER232, "IncrementCounter supressed.\r\n"); )
    // Tokens not being active is fatal for this function.
    return ECODE_NVM3_ERR_NOT_OPENED;
  }

  return ECODE_OK;
}

#endif // defined(USE_NVM3)
