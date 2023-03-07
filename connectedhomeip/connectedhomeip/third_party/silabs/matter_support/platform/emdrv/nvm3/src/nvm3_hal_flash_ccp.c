/*****************************************************************************
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

#include "ccp_flash_intf.h"
#include "nvm3.h"
#include "nvm3_hal_flash.h"
#include <stdbool.h>
#include <string.h>

/*******************************************************************************
 * @addtogroup nvm3
 * @{
 ******************************************************************************/

/*******************************************************************************
 * @addtogroup nvm3hal
 * @{
 ******************************************************************************/

/******************************************************************************
 ******************************    MACROS    **********************************
 *****************************************************************************/

#define CHECK_DATA 0 ///< Macro defining if data should be checked

/* Enable delay for flash (Qspi) operations.*/
#define CCP_FLASH_DELAY 100000

/******************************************************************************
 ***************************   LOCAL VARIABLES   ******************************
 *****************************************************************************/

/******************************************************************************
 ***************************   LOCAL FUNCTIONS   ******************************
 *****************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

// Check if the page is erased.
static bool isErased(void *adr, size_t len) {
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

static Ecode_t nvm3_halFlashOpen(nvm3_HalPtr_t nvmAdr, size_t flashSize) {
  (void)nvmAdr;
  (void)flashSize;
  Ecode_t halSta = ECODE_NVM3_ERR_NOT_OPENED;
  bool ret = ECODE_QSPI_ERROR;

#if CCP_FLASH_DELAY
  /* Delay is added for flash (Qspi) operations.
   * It is added to resolve target connection lost (mcu reset) issue in SiWx917
   * SOC Device.
   */
  for (int i = 0; i < CCP_FLASH_DELAY; i++)
    __asm__("nop;");
#endif /* CCP_FLASH_DELAY */

  /* CCP flash Initilize */
  ret = Init(0, 0, 0);
  if (!ret)
    halSta = ECODE_NVM3_OK;

  return halSta;
}

static void nvm3_halFlashClose(void) {
  /* Need to implement  deinitilize API for CCP flash */
}

static Ecode_t nvm3_halFlashGetInfo(nvm3_HalInfo_t *halInfo) {
  /* Hardcode with EFR32 */
  halInfo->deviceFamilyPartNumber = 19;
  halInfo->writeSize = 1;
  halInfo->memoryMapped = 1;
  halInfo->pageSize = 1024;
  halInfo->systemUnique = 0;
  return ECODE_NVM3_OK;
}

static void nvm3_halFlashAccess(nvm3_HalNvmAccessCode_t access) {
  (void)access;
}

static Ecode_t nvm3_halFlashReadWords(nvm3_HalPtr_t nvmAdr, void *dst,
                                      size_t wordCnt) {
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

static Ecode_t nvm3_halFlashWriteWords(nvm3_HalPtr_t nvmAdr, void const *src,
                                       size_t wordCnt) {
  const uint32_t *pSrc = src;
  uint32_t *pDst = (uint32_t *)nvmAdr;
  Ecode_t halSta = ECODE_NVM3_ERR_WRITE_FAILED;
  bool ret = ECODE_QSPI_ERROR;
  size_t byteCnt;

  byteCnt = wordCnt * sizeof(uint32_t);

  /* CCP flash Write */
  ret = ProgramPage(pDst, byteCnt, (char *)pSrc);
  if (!ret)
    halSta = ECODE_NVM3_OK;

#if CHECK_DATA
  if (halSta == ECODE_NVM3_OK) {
    if (memcmp(pDst, pSrc, byteCnt) != 0) {
      halSta = ECODE_NVM3_ERR_WRITE_FAILED;
    }
  }
#endif

  return halSta;
}

static Ecode_t nvm3_halFlashPageErase(nvm3_HalPtr_t nvmAdr) {
  Ecode_t halSta = ECODE_NVM3_ERR_ERASE_FAILED;
  bool ret = ECODE_QSPI_ERROR;

  /* CCP flash Erase */
  ret = EraseSector((uint32_t *)nvmAdr);
  if (!ret)
    halSta = ECODE_NVM3_OK;

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
    .open = nvm3_halFlashOpen,             ///< Set the open function
    .close = nvm3_halFlashClose,           ///< Set the close function
    .getInfo = nvm3_halFlashGetInfo,       ///< Set the get-info function
    .access = nvm3_halFlashAccess,         ///< Set the access function
    .pageErase = nvm3_halFlashPageErase,   ///< Set the page-erase function
    .readWords = nvm3_halFlashReadWords,   ///< Set the read-words function
    .writeWords = nvm3_halFlashWriteWords, ///< Set the write-words function
};

/** @} (end addtogroup nvm3hal) */
/** @} (end addtogroup nvm3) */
