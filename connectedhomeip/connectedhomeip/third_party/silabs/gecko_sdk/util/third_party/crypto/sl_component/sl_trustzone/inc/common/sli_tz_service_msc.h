/***************************************************************************//**
 * @file
 * @brief Silicon Labs TrustZone secure MSC service.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef __SLI_TZ_MSC_VENEERS_H__
#define __SLI_TZ_MSC_VENEERS_H__

#include <stdint.h>
#include <stdbool.h>

#include "sli_tz_util.h"
#include "psa/crypto_types.h"
#include "psa/client.h"

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Full service (IOVEC usage) function identifiers

/**
 * \brief Define a progressive numerical value for each SID which can be used
 *        when dispatching the requests to the service
 */
enum {
  SLI_TZ_SERVICE_MSC_WRITEWORD_SID = (0u),
  SLI_TZ_SERVICE_MSC_WRITEWORDDMA_SID,
  SLI_TZ_SERVICE_MSC_MAX_SID,
};

//------------------------------------------------------------------------------
// Function declarations

//-------------------------------------
// Simple services (no IOVEC usage)

SLI_TZ_CMSE_NONSECURE_ENTRY
uint32_t sli_tz_msc_get_locked(uint32_t unused);

SLI_TZ_CMSE_NONSECURE_ENTRY
uint32_t sli_tz_msc_set_locked(uint32_t unused);

SLI_TZ_CMSE_NONSECURE_ENTRY
uint32_t sli_tz_msc_set_unlocked(uint32_t unused);

SLI_TZ_CMSE_NONSECURE_ENTRY
uint32_t sli_tz_msc_get_readctrl(uint32_t unused);

SLI_TZ_CMSE_NONSECURE_ENTRY
uint32_t sli_tz_msc_set_readctrl(uint32_t value);

SLI_TZ_CMSE_NONSECURE_ENTRY
uint32_t sli_tz_msc_set_pagelock(uint32_t page_number);

SLI_TZ_CMSE_NONSECURE_ENTRY
uint32_t sli_tz_msc_get_pagelock(uint32_t page_number);

SLI_TZ_CMSE_NONSECURE_ENTRY
uint32_t sli_tz_msc_get_userdata_size(uint32_t unused);

SLI_TZ_CMSE_NONSECURE_ENTRY
uint32_t sli_tz_msc_get_misclockword(uint32_t unused);

SLI_TZ_CMSE_NONSECURE_ENTRY
uint32_t sli_tz_msc_set_misclockword(uint32_t value);

SLI_TZ_CMSE_NONSECURE_ENTRY
uint32_t sli_tz_msc_init(uint32_t unused);

SLI_TZ_CMSE_NONSECURE_ENTRY
uint32_t sli_tz_msc_deinit(uint32_t unused);

SLI_TZ_CMSE_NONSECURE_ENTRY
uint32_t sli_tz_msc_erase_page(uint32_t startAddress);

//-------------------------------------
// Full services (IOVEC usage)

int32_t sli_tz_msc_write_word(psa_invec in_vec[],
                              size_t in_len,
                              psa_outvec out_vec[],
                              size_t out_len);

int32_t sli_tz_msc_write_word_dma(psa_invec in_vec[],
                                  size_t in_len,
                                  psa_outvec out_vec[],
                                  size_t out_len);

#ifdef __cplusplus
}
#endif

#endif // __SLI_TZ_MSC_VENEERS_H__
