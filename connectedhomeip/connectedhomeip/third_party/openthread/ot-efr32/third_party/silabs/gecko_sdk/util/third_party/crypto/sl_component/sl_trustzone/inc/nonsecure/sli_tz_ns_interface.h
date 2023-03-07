/***************************************************************************//**
 * @file
 * @brief Silicon Labs TrustZone non-secure interface dispatch functionality.
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

#ifndef __SLI_TZ_NS_INTERFACE_H__
#define __SLI_TZ_NS_INTERFACE_H__

#include <stdint.h>
#include "psa/crypto_types.h"
#include "psa/client.h"

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Typedefs

typedef int32_t (*sli_tz_veneer_fn) (psa_invec *in_vec, size_t in_len,
                                     psa_outvec *out_vec, size_t out_len);

typedef uint32_t (*sli_tz_simple_veneer_fn) (uint32_t arg);

//------------------------------------------------------------------------------
// Function declarations

uint32_t sli_tz_ns_interface_init(void);

int32_t sli_tz_ns_interface_dispatch(sli_tz_veneer_fn fn,
                                     uint32_t arg0, uint32_t arg1,
                                     uint32_t arg2, uint32_t arg3);

uint32_t sli_tz_ns_interface_dispatch_simple(sli_tz_simple_veneer_fn fn,
                                             uint32_t arg);

#ifdef __cplusplus
}
#endif

#endif // __SLI_TZ_NS_INTERFACE_H__
