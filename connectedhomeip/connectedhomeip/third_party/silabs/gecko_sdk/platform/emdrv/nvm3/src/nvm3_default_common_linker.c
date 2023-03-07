/***************************************************************************//**
 * @file
 * @brief NVM3 definition of the default data structures.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "nvm3.h"
#include "nvm3_hal_flash.h"
#include "nvm3_default_config.h"

#if defined(NVM3_BASE)
/* Manually control the NVM3 address and size */

#elif defined (__ICCARM__)

__root uint8_t nvm3_default_storage[NVM3_DEFAULT_NVM_SIZE] @ "SIMEE";
#define NVM3_BASE (nvm3_default_storage)

#elif defined (__GNUC__)

extern char linker_nvm_begin;

__attribute__((used)) uint8_t nvm3_default_storage[NVM3_DEFAULT_NVM_SIZE] __attribute__ ((section(".simee")));
#define NVM3_BASE (&linker_nvm_begin)

#else
#error "Unsupported toolchain"
#endif

nvm3_Handle_t  nvm3_defaultHandleData;
nvm3_Handle_t *nvm3_defaultHandle = &nvm3_defaultHandleData;

#if (NVM3_DEFAULT_CACHE_SIZE != 0)
static nvm3_CacheEntry_t defaultCache[NVM3_DEFAULT_CACHE_SIZE];
#endif

nvm3_Init_t nvm3_defaultInitData =
{
  (nvm3_HalPtr_t)NVM3_BASE,
  NVM3_DEFAULT_NVM_SIZE,
#if (NVM3_DEFAULT_CACHE_SIZE != 0)
  defaultCache,
#else
  NULL,
#endif
  NVM3_DEFAULT_CACHE_SIZE,
  NVM3_DEFAULT_MAX_OBJECT_SIZE,
  NVM3_DEFAULT_REPACK_HEADROOM,
  &nvm3_halFlashHandle,
};

nvm3_Init_t *nvm3_defaultInit = &nvm3_defaultInitData;

Ecode_t nvm3_initDefault(void)
{
  return nvm3_open(nvm3_defaultHandle, nvm3_defaultInit);
}

Ecode_t nvm3_deinitDefault(void)
{
  return nvm3_close(nvm3_defaultHandle);
}
