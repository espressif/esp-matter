/***************************************************************************//**
 * @file
 * @brief Silicon Labs TrustZone secure NVM3 service.
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

#ifndef __SLI_TZ_NVM3_VENEERS_H__
#define __SLI_TZ_NVM3_VENEERS_H__

#include "tfm_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(SL_COMPONENT_CATALOG_PRESENT)
  #include "sl_component_catalog.h"
#endif

//------------------------------------------------------------------------------
// Service function identifiers

/**
 * \brief Define a progressive numerical value for each SID which can be used
 *        when dispatching the requests to the service
 */
enum {
  SLI_TZ_SERVICE_NVM3_INITDEFAULT_SID = (0u),
  SLI_TZ_SERVICE_NVM3_DEINITDEFAULT_SID,
#if defined(SL_CATALOG_TZ_SERVICE_NVM3_PRESENT)
  SLI_TZ_SERVICE_NVM3_READPARTIALDATA_SID,
  SLI_TZ_SERVICE_NVM3_READDATA_SID,
  SLI_TZ_SERVICE_NVM3_WRITEDATA_SID,
  SLI_TZ_SERVICE_NVM3_DELETEOBJECT_SID,
  SLI_TZ_SERVICE_NVM3_OPEN_SID,
  SLI_TZ_SERVICE_NVM3_CLOSE_SID,
  SLI_TZ_SERVICE_NVM3_GETOBJECTINFO_SID,
  SLI_TZ_SERVICE_NVM3_WRITECOUNTER_SID,
  SLI_TZ_SERVICE_NVM3_READCOUNTER_SID,
  SLI_TZ_SERVICE_NVM3_INCREMENTCOUNTER_SID,
  SLI_TZ_SERVICE_NVM3_ERASEALL_SID,
  SLI_TZ_SERVICE_NVM3_GETERASECOUNT_SID,
  SLI_TZ_SERVICE_NVM3_SETERASECOUNT_SID,
  SLI_TZ_SERVICE_NVM3_REPACK_SID,
  SLI_TZ_SERVICE_NVM3_REPACKNEEDED_SID,
  SLI_TZ_SERVICE_NVM3_RESIZE_SID,
  SLI_TZ_SERVICE_NVM3_ENUMOBJECTS_SID,
  SLI_TZ_SERVICE_NVM3_ENUMDELETEDOBJECTS_SID,
#endif // defined(SL_CATALOG_TZ_SERVICE_NVM3_PRESENT)
  SLI_TZ_SERVICE_NVM3_MAX_SID,
};

//------------------------------------------------------------------------------
// Function declarations

psa_status_t tfm_nvm3_init_default(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t tfm_nvm3_deinit_default(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
#if defined(SL_CATALOG_TZ_SERVICE_NVM3_PRESENT)
psa_status_t tfm_nvm3_read_partial_data(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t tfm_nvm3_read_data(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t tfm_nvm3_write_data(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t tfm_nvm3_delete_object(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t tfm_nvm3_open(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t tfm_nvm3_close(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t tfm_nvm3_get_object_info(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t tfm_nvm3_write_counter(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t tfm_nvm3_read_counter(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t tfm_nvm3_increment_counter(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t tfm_nvm3_erase_all(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t tfm_nvm3_get_erase_count(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t tfm_nvm3_set_erase_count(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t tfm_nvm3_repack(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t tfm_nvm3_repack_needed(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t tfm_nvm3_resize(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t tfm_nvm3_enum_objects(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t tfm_nvm3_enum_deleted_objects(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
#endif // defined(SL_CATALOG_TZ_SERVICE_NVM3_PRESENT)

#ifdef __cplusplus
}
#endif

#endif /* __SLI_TZ_NVM3_VENEERS_H__ */
