/***************************************************************************//**
 * @file
 * @brief Non-Volatile Memory Wear-Leveling driver HAL implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdbool.h>
#include <string.h>
#include "nvm3.h"
#include "nvm3_hal_flash.h"
#include "em_system.h"
#include "em_msc.h"

/***************************************************************************//**
 * @addtogroup nvm3
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup nvm3hal
 * @{
 ******************************************************************************/

/******************************************************************************
 ******************************    MACROS    **********************************
 *****************************************************************************/

#define CHECK_DATA  1           ///< Macro defining if data should be checked

/******************************************************************************
 ***************************   LOCAL VARIABLES   ******************************
 *****************************************************************************/

/******************************************************************************
 ***************************   LOCAL FUNCTIONS   ******************************
 *****************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *   Convert return type.
 *
 * @details
 *   This function converts between the return type of the emlib and the
 *   NVM3 API.
 *
 * @param[in] result
 *   Operation result.
 *
 * @return
 *   Returns remapped status code.
 ******************************************************************************/
static Ecode_t convertMscStatusToNvm3Status(MSC_Status_TypeDef result)
{
  Ecode_t ret;

  switch (result) {
    case mscReturnOk:
      ret = ECODE_NVM3_OK;
      break;
    case mscReturnInvalidAddr:
      ret = ECODE_NVM3_ERR_INT_ADDR_INVALID;
      break;
    default:
      ret = ECODE_NVM3_ERR_INT_EMULATOR;
      break;
  }

  return ret;
}

// Check if the page is erased.
static bool isErased(void *adr, size_t len)
{
  size_t i;
  size_t cnt;
  uint32_t *dat = adr;

  cnt = len / sizeof(uint32_t);
  for (i = 0U; i < cnt; i++) {
    if (*dat != 0xFFFFFFFFUL) {
      return false;
    }
    dat++;
  }

  return true;
}

/** @endcond */

static Ecode_t nvm3_halFlashOpen(nvm3_HalPtr_t nvmAdr, size_t flashSize)
{
  (void)nvmAdr;
  (void)flashSize;
  MSC_Init();

  return ECODE_NVM3_OK;
}

static void nvm3_halFlashClose(void)
{
  MSC_Deinit();
}

static Ecode_t nvm3_halFlashGetInfo(nvm3_HalInfo_t *halInfo)
{
  SYSTEM_ChipRevision_TypeDef chipRev;

  SYSTEM_ChipRevisionGet(&chipRev);
#if defined(_SYSCFG_CHIPREV_PARTNUMBER_MASK)
  halInfo->deviceFamilyPartNumber = chipRev.partNumber;
#else
  halInfo->deviceFamilyPartNumber = chipRev.family;
#endif
  halInfo->memoryMapped = 1;
#if defined(_SILICON_LABS_32B_SERIES_2)
  halInfo->writeSize = NVM3_HAL_WRITE_SIZE_32;
#else
  halInfo->writeSize = NVM3_HAL_WRITE_SIZE_16;
#endif
  halInfo->pageSize = SYSTEM_GetFlashPageSize();

  return ECODE_NVM3_OK;
}

static void nvm3_halFlashAccess(nvm3_HalNvmAccessCode_t access)
{
  (void)access;
}

static Ecode_t nvm3_halFlashReadWords(nvm3_HalPtr_t nvmAdr, void *dst, size_t wordCnt)
{
  uint32_t *pSrc = (uint32_t *)nvmAdr;
  uint32_t *pDst = dst;

  if ((((size_t)pSrc % 4) == 0) && (((size_t)pDst % 4) == 0)) {
    while (wordCnt > 0U) {
      *pDst++ = *pSrc++;
      wordCnt--;
    }
  } else {
    (void)memcpy(dst, nvmAdr, wordCnt * sizeof(uint32_t));
  }

  return ECODE_NVM3_OK;
}

static Ecode_t nvm3_halFlashWriteWords(nvm3_HalPtr_t nvmAdr, void const *src, size_t wordCnt)
{
  const uint32_t *pSrc = src;
  uint32_t *pDst = (uint32_t *)nvmAdr;
  MSC_Status_TypeDef mscSta;
  Ecode_t halSta;
  size_t byteCnt;

  byteCnt = wordCnt * sizeof(uint32_t);
  mscSta = MSC_WriteWord(pDst, pSrc, byteCnt);
  halSta = convertMscStatusToNvm3Status(mscSta);

#if CHECK_DATA
  if (halSta == ECODE_NVM3_OK) {
    if (memcmp(pDst, pSrc, byteCnt) != 0) {
      halSta = ECODE_NVM3_ERR_WRITE_FAILED;
    }
  }
#endif

  return halSta;
}

static Ecode_t nvm3_halFlashPageErase(nvm3_HalPtr_t nvmAdr)
{
  MSC_Status_TypeDef mscSta;
  Ecode_t halSta;

  mscSta = MSC_ErasePage((uint32_t *)nvmAdr);
  halSta = convertMscStatusToNvm3Status(mscSta);

#if CHECK_DATA
  if (halSta == ECODE_NVM3_OK) {
    if (!isErased(nvmAdr, SYSTEM_GetFlashPageSize())) {
      halSta = ECODE_NVM3_ERR_ERASE_FAILED;
    }
  }
#endif

  return halSta;
}

/*******************************************************************************
 ***************************   GLOBAL VARIABLES   ******************************
 ******************************************************************************/

const nvm3_HalHandle_t nvm3_halFlashHandle = {
  .open = nvm3_halFlashOpen,                    ///< Set the open function
  .close = nvm3_halFlashClose,                  ///< Set the close function
  .getInfo = nvm3_halFlashGetInfo,              ///< Set the get-info function
  .access = nvm3_halFlashAccess,                ///< Set the access function
  .pageErase = nvm3_halFlashPageErase,          ///< Set the page-erase function
  .readWords = nvm3_halFlashReadWords,          ///< Set the read-words function
  .writeWords = nvm3_halFlashWriteWords,        ///< Set the write-words function
};

/** @} (end addtogroup nvm3hal) */
/** @} (end addtogroup nvm3) */
