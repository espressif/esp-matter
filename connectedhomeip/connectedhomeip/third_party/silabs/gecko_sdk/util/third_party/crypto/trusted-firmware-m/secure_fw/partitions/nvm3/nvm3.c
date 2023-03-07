/***************************************************************************//**
 * @file
 * @brief Silicon Labs TrustZone secure NVM3 service (secure side).
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

// TODO: add IPC mode code

#include <stddef.h>
#include <stdint.h>

#include "psa/error.h"

#include "tfm_nvm3_include.h"

#if defined(TFM_CONFIG_SL_SECURE_LIBRARY)
#include "psa/crypto_types.h"
#include "psa/client.h"
#include "sli_tz_service_nvm3.h"
#else // defined(TFM_CONFIG_SL_SECURE_LIBRARY)
#ifndef TFM_PSA_API
#include "tfm_secure_api.h"
#endif
#endif // defined(TFM_CONFIG_SL_SECURE_LIBRARY)

nvm3_Handle_t  nvm3_defaultHandleData;
nvm3_Handle_t *nvm3_defaultHandle = &nvm3_defaultHandleData;

psa_status_t tfm_nvm3_init(void)
{
  // This init function is required by TFM and not used otherwise.
  return PSA_SUCCESS;
}

psa_status_t tfm_nvm3_init_default(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Copy the provided init data into a new struct, such that the hal handle can be
  // modified without changing the original struct.
  nvm3_Init_t init_data = *((nvm3_Init_t *)in_vec[1].base);
  if (init_data.halHandle != NULL) {
    // The flash handle must explicitly be set to NULL by NS to make it clear that
    // that the Secure-side flash handle will be used instead. We don't want to
    // make the NS side believe it can provide its own flash handle.
    return PSA_ERROR_PROGRAMMER_ERROR;
  }

  // Overwrite NULL pointer with actual flash handle defined in nvm3_hal_flash.c
  extern const nvm3_HalHandle_t nvm3_halFlashHandle;
  init_data.halHandle = &nvm3_halFlashHandle;

  Ecode_t *nvm3_status = out_vec[0].base;

  // We use nvm3_open() instead of nvm3_initDefault() in order to be able to provide
  // the updated default init data.
  *nvm3_status = nvm3_open(nvm3_defaultHandle, &init_data);

  return PSA_SUCCESS;
}

psa_status_t tfm_nvm3_deinit_default(psa_invec in_vec[],
                                     size_t in_len,
                                     psa_outvec out_vec[],
                                     size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  Ecode_t *nvm3_status = out_vec[0].base;

  *nvm3_status = nvm3_close(nvm3_defaultHandle);

  return PSA_SUCCESS;
}

#if defined(SL_CATALOG_TZ_SERVICE_NVM3_PRESENT)
psa_status_t tfm_nvm3_read_partial_data(psa_invec in_vec[],
                                        size_t in_len,
                                        psa_outvec out_vec[],
                                        size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  nvm3_Handle_t *h = (nvm3_Handle_t *)in_vec[1].base;
  nvm3_ObjectKey_t key = *((nvm3_ObjectKey_t *)in_vec[2].base);
  size_t ofs = *((size_t *)in_vec[3].base);

  // Output arguments
  Ecode_t *nvm3_status = out_vec[0].base;
  void *value = out_vec[1].base;
  size_t len = out_vec[1].len;

  if (h->nvmAdr == NULL) {
    h = nvm3_defaultHandle;
  }

  *nvm3_status = nvm3_readPartialData(h, key, value, ofs, len);

  return PSA_SUCCESS;
}

psa_status_t tfm_nvm3_read_data(psa_invec in_vec[],
                                size_t in_len,
                                psa_outvec out_vec[],
                                size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  nvm3_Handle_t *h = (nvm3_Handle_t *)in_vec[1].base;
  nvm3_ObjectKey_t key = *((nvm3_ObjectKey_t *)in_vec[2].base);

  // Output arguments
  Ecode_t *nvm3_status = out_vec[0].base;
  void *value = out_vec[1].base;
  size_t len = out_vec[1].len;

  if (h->nvmAdr == NULL) {
    h = nvm3_defaultHandle;
  }

  *nvm3_status = nvm3_readData(h, key, value, len);

  return PSA_SUCCESS;
}

psa_status_t tfm_nvm3_write_data(psa_invec in_vec[],
                                 size_t in_len,
                                 psa_outvec out_vec[],
                                 size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  nvm3_Handle_t *h = (nvm3_Handle_t *)in_vec[1].base;
  nvm3_ObjectKey_t key = *((nvm3_ObjectKey_t *)in_vec[2].base);
  const void *value = in_vec[3].base;
  size_t len = in_vec[3].len;

  // Output arguments
  Ecode_t *nvm3_status = out_vec[0].base;

  if (h->nvmAdr == NULL) {
    h = nvm3_defaultHandle;
  }

  *nvm3_status = nvm3_writeData(h, key, value, len);

  return PSA_SUCCESS;
}

psa_status_t tfm_nvm3_delete_object(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  nvm3_Handle_t *h = (nvm3_Handle_t *)in_vec[1].base;
  nvm3_ObjectKey_t key = *((nvm3_ObjectKey_t *)in_vec[2].base);

  // Output arguments
  Ecode_t *nvm3_status = out_vec[0].base;

  if (h->nvmAdr == NULL) {
    h = nvm3_defaultHandle;
  }

  *nvm3_status = nvm3_deleteObject(h, key);

  return PSA_SUCCESS;
}

psa_status_t tfm_nvm3_open(psa_invec in_vec[],
                           size_t in_len,
                           psa_outvec out_vec[],
                           size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  nvm3_Init_t *i = (nvm3_Init_t *)in_vec[1].base;

  // Output arguments
  Ecode_t *nvm3_status = out_vec[0].base;
  nvm3_Handle_t *h = (nvm3_Handle_t *)out_vec[1].base;

  if (i->halHandle != NULL) {
    // The flash handle must explicitly be set to NULL by NS to make it clear that
    // that the Secure-side flash handle will be used instead. We don't want to
    // make the NS side believe it can provide its own flash handle.
    return PSA_ERROR_PROGRAMMER_ERROR;
  }

  // Overwrite NULL pointer with actual flash handle defined in nvm3_hal_flash.c
  extern const nvm3_HalHandle_t nvm3_halFlashHandle;
  i->halHandle = &nvm3_halFlashHandle;

  /* check if base is in NS */

  *nvm3_status = nvm3_open(h, i);

  return PSA_SUCCESS;
}

psa_status_t tfm_nvm3_close(psa_invec in_vec[],
                            size_t in_len,
                            psa_outvec out_vec[],
                            size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  nvm3_Handle_t *h = (nvm3_Handle_t *)in_vec[1].base;

  // Output arguments
  Ecode_t *nvm3_status = out_vec[0].base;

  if (h->nvmAdr == NULL) {
    h = nvm3_defaultHandle;
  }

  *nvm3_status = nvm3_close(h);

  return PSA_SUCCESS;
}

psa_status_t tfm_nvm3_get_object_info(psa_invec in_vec[],
                                      size_t in_len,
                                      psa_outvec out_vec[],
                                      size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  nvm3_Handle_t *h = (nvm3_Handle_t *)in_vec[1].base;
  nvm3_ObjectKey_t key = *((nvm3_ObjectKey_t *)in_vec[2].base);

  // Output arguments
  Ecode_t *nvm3_status = out_vec[0].base;
  uint32_t *type = (uint32_t *)out_vec[1].base;
  size_t *len = (size_t *)out_vec[2].base;

  if (h->nvmAdr == NULL) {
    h = nvm3_defaultHandle;
  }

  *nvm3_status = nvm3_getObjectInfo(h, key, type, len);

  return PSA_SUCCESS;
}

psa_status_t tfm_nvm3_write_counter(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  nvm3_Handle_t *h = (nvm3_Handle_t *)in_vec[1].base;
  nvm3_ObjectKey_t key = *((nvm3_ObjectKey_t *)in_vec[2].base);
  uint32_t value = *((uint32_t *)in_vec[3].base);

  // Output arguments
  Ecode_t *nvm3_status = out_vec[0].base;

  if (h->nvmAdr == NULL) {
    h = nvm3_defaultHandle;
  }

  *nvm3_status = nvm3_writeCounter(h, key, value);

  return PSA_SUCCESS;
}

psa_status_t tfm_nvm3_read_counter(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  nvm3_Handle_t *h = (nvm3_Handle_t *)in_vec[1].base;
  nvm3_ObjectKey_t key = *((nvm3_ObjectKey_t *)in_vec[2].base);

  // Output arguments
  Ecode_t *nvm3_status = out_vec[0].base;
  uint32_t *value = (uint32_t *)out_vec[1].base;

  if (h->nvmAdr == NULL) {
    h = nvm3_defaultHandle;
  }

  *nvm3_status = nvm3_readCounter(h, key, value);

  return PSA_SUCCESS;
}

psa_status_t tfm_nvm3_increment_counter(psa_invec in_vec[],
                                        size_t in_len,
                                        psa_outvec out_vec[],
                                        size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  nvm3_Handle_t *h = (nvm3_Handle_t *)in_vec[1].base;
  nvm3_ObjectKey_t key = *((nvm3_ObjectKey_t *)in_vec[2].base);

  // Output arguments
  Ecode_t *nvm3_status = out_vec[0].base;
  uint32_t **new_value = (uint32_t **)out_vec[1].base;

  if (h->nvmAdr == NULL) {
    h = nvm3_defaultHandle;
  }

  *nvm3_status = nvm3_incrementCounter(h, key, *new_value);

  return PSA_SUCCESS;
}

psa_status_t tfm_nvm3_erase_all(psa_invec in_vec[],
                                size_t in_len,
                                psa_outvec out_vec[],
                                size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  nvm3_Handle_t *h = (nvm3_Handle_t *)in_vec[1].base;

  // Output arguments
  Ecode_t *nvm3_status = out_vec[0].base;

  if (h->nvmAdr == NULL) {
    h = nvm3_defaultHandle;
  }

  *nvm3_status = nvm3_eraseAll(h);

  return PSA_SUCCESS;
}

psa_status_t tfm_nvm3_get_erase_count(psa_invec in_vec[],
                                      size_t in_len,
                                      psa_outvec out_vec[],
                                      size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  nvm3_Handle_t *h = (nvm3_Handle_t *)in_vec[1].base;

  // Output arguments
  Ecode_t *nvm3_status = out_vec[0].base;
  uint32_t *erase_cnt = (uint32_t *)out_vec[1].base;

  if (h->nvmAdr == NULL) {
    h = nvm3_defaultHandle;
  }

  *nvm3_status = nvm3_getEraseCount(h, erase_cnt);

  return PSA_SUCCESS;
}

psa_status_t tfm_nvm3_set_erase_count(psa_invec in_vec[],
                                      size_t in_len,
                                      psa_outvec out_vec[],
                                      size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  uint32_t erase_cnt = *((uint32_t *)in_vec[1].base);

  // Output arguments
  // ... None ...

  nvm3_setEraseCount(erase_cnt);

  return PSA_SUCCESS;
}


/* Ecode_t nvm3_repack(nvm3_Handle_t *h); */


psa_status_t tfm_nvm3_repack(psa_invec in_vec[],
                             size_t in_len,
                             psa_outvec out_vec[],
                             size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  nvm3_Handle_t *h = (nvm3_Handle_t *)in_vec[1].base;

  // Output arguments
  Ecode_t *nvm3_status = out_vec[0].base;

  if (h->nvmAdr == NULL) {
    h = nvm3_defaultHandle;
  }

  *nvm3_status = nvm3_repack(h);

  return PSA_SUCCESS;
}

psa_status_t tfm_nvm3_repack_needed(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  nvm3_Handle_t *h = (nvm3_Handle_t *)in_vec[1].base;

  // Output arguments
  bool *nvm3_status = out_vec[0].base;

  if (h->nvmAdr == NULL) {
    h = nvm3_defaultHandle;
  }

  *nvm3_status = nvm3_repackNeeded(h);

  return PSA_SUCCESS;
}

psa_status_t tfm_nvm3_resize(psa_invec in_vec[],
                             size_t in_len,
                             psa_outvec out_vec[],
                             size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  nvm3_Handle_t *h = (nvm3_Handle_t *)in_vec[1].base;
  nvm3_HalPtr_t new_addr = *((nvm3_HalPtr_t *)in_vec[2].base);
  size_t new_size = *((size_t *)in_vec[3].base);

  // Output arguments
  Ecode_t *nvm3_status = out_vec[0].base;

  if (h->nvmAdr == NULL) {
    h = nvm3_defaultHandle;
  }

  *nvm3_status = nvm3_resize(h, new_addr, new_size);

  return PSA_SUCCESS;
}

psa_status_t tfm_nvm3_enum_objects(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  nvm3_Handle_t *h = (nvm3_Handle_t *)in_vec[1].base;
  size_t key_list_size = *((size_t *)in_vec[2].base);
  nvm3_ObjectKey_t key_min = *((nvm3_ObjectKey_t *)in_vec[3].base);
  nvm3_ObjectKey_t key_max = *((nvm3_ObjectKey_t *)in_vec[4].base);

  // Output arguments
  Ecode_t *nvm3_status = out_vec[0].base;
  nvm3_ObjectKey_t **key_list_ptr = (nvm3_ObjectKey_t **)out_vec[1].base;

  if (h->nvmAdr == NULL) {
    h = nvm3_defaultHandle;
  }

  *nvm3_status = nvm3_enumObjects(h, *key_list_ptr, key_list_size, key_min, key_max);

  return PSA_SUCCESS;
}

psa_status_t tfm_nvm3_enum_deleted_objects(psa_invec in_vec[],
                                           size_t in_len,
                                           psa_outvec out_vec[],
                                           size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  nvm3_Handle_t *h = (nvm3_Handle_t *)in_vec[1].base;
  size_t key_list_size = *((size_t *)in_vec[2].base);
  nvm3_ObjectKey_t key_min = *((nvm3_ObjectKey_t *)in_vec[3].base);
  nvm3_ObjectKey_t key_max = *((nvm3_ObjectKey_t *)in_vec[4].base);


  // Output arguments
  Ecode_t *nvm3_status = out_vec[0].base;
  nvm3_ObjectKey_t **key_list_ptr = (nvm3_ObjectKey_t **)out_vec[1].base;

  if (h->nvmAdr == NULL) {
    h = nvm3_defaultHandle;
  }

  *nvm3_status = nvm3_enumDeletedObjects(h, *key_list_ptr, key_list_size, key_min, key_max);

  return PSA_SUCCESS;
}
#endif // defined(SL_CATALOG_TZ_SERVICE_NVM3_PRESENT)
