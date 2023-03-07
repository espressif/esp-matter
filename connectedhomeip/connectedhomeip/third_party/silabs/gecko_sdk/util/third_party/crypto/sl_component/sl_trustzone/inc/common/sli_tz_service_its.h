/***************************************************************************//**
 * @file
 * @brief Silicon Labs TrustZone secure ITS service.
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

#ifndef __SL_ITS_VENEERS_H__
#define __SL_ITS_VENEERS_H__

#include "tfm_api.h"

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Service function identifiers

/**
 * \brief Define a progressive numerical value for each SID which can be used
 *        when dispatching the requests to the service
 */
enum {
  SLI_TZ_SERVICE_ITS_SET_SID = (0u),
  SLI_TZ_SERVICE_ITS_GET_SID,
  SLI_TZ_SERVICE_ITS_GETINFO_SID,
  SLI_TZ_SERVICE_ITS_REMOVE_SID,
  SLI_TZ_SERVICE_ITS_ENCRYPTED_SID,
  SLI_TZ_SERVICE_ITS_MAX_SID,
};

//------------------------------------------------------------------------------
// Function declarations

psa_status_t tfm_its_set_req(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t tfm_its_get_req(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t tfm_its_get_info_req(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t tfm_its_remove_req(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_psa_its_encrypted_req(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);

#ifdef __cplusplus
}
#endif

#endif /* __SL_ITS_VENEERS_H__ */
