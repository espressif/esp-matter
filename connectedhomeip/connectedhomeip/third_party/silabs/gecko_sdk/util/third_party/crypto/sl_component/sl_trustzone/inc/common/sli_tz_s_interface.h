/***************************************************************************//**
 * @file
 * @brief Silicon Labs TrustZone Secure Library Interface.
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

#ifndef __SLI_TZ_S_INTERFACE_H__
#define __SLI_TZ_S_INTERFACE_H__

#include "sli_tz_util.h"

#include "psa/client.h"

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Typedefs

typedef uint32_t sli_tz_fn_id;

//------------------------------------------------------------------------------
// Function declarations

SLI_TZ_CMSE_NONSECURE_ENTRY
int32_t sli_tz_s_interface_dispatch_crypto(psa_invec in_vec[],
                                           size_t in_len,
                                           psa_outvec out_vec[],
                                           size_t out_len);

SLI_TZ_CMSE_NONSECURE_ENTRY
int32_t sli_tz_s_interface_dispatch_msc(psa_invec in_vec[],
                                        size_t in_len,
                                        psa_outvec out_vec[],
                                        size_t out_len);

SLI_TZ_CMSE_NONSECURE_ENTRY
int32_t sli_tz_s_interface_dispatch_nvm3(psa_invec in_vec[],
                                         size_t in_len,
                                         psa_outvec out_vec[],
                                         size_t out_len);

SLI_TZ_CMSE_NONSECURE_ENTRY
int32_t sli_tz_s_interface_dispatch_its(psa_invec in_vec[],
                                        size_t in_len,
                                        psa_outvec out_vec[],
                                        size_t out_len);

SLI_TZ_CMSE_NONSECURE_ENTRY
int32_t sli_tz_s_interface_dispatch_se_manager(psa_invec in_vec[],
                                               size_t in_len,
                                               psa_outvec out_vec[],
                                               size_t out_len);

#ifdef __cplusplus
}
#endif

#endif // __SLI_TZ_S_INTERFACE_H__
