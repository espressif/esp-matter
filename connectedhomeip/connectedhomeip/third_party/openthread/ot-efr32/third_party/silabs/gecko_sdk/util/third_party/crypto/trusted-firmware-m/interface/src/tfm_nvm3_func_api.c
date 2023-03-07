/***************************************************************************//**
 * @file
 * @brief Silicon Labs TrustZone secure NVM3 service (non-secure side).
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

#if defined(TFM_CONFIG_SL_SECURE_LIBRARY)

#include "psa/error.h"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
  #include "sl_component_catalog.h"
#endif

#include "tfm_ns_interface.h"
#include "sli_tz_s_interface.h"

#include "sli_tz_service_nvm3.h"
#include "nvm3.h"
#include "nvm3_default_config.h"

#else // defined(TFM_CONFIG_SL_SECURE_LIBRARY)

#include "psa/error.h"

#include "tfm_api.h"
#include "tfm_ns_interface.h"
#include "tfm_veneers.h"

#include "nvm3.h"

#endif // defined(TFM_CONFIG_SL_SECURE_LIBRARY)

// The default handles are located in S together with the rest of the NVM3 code. However, the
// NVM3 api requires the handle pointer to be sent in explicitly, which NS does not know. We
// therefore use NULL as a special value, which makes the NVM3 service in TFM use its internal
// default handles.
//
// TODO: Use a non-NULL macro instead?

nvm3_Handle_t placeholder_default_handle = { NULL };
nvm3_Handle_t *nvm3_defaultHandle = &placeholder_default_handle;

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

static nvm3_CacheEntry_t defaultCache[NVM3_DEFAULT_CACHE_SIZE];

nvm3_Init_t nvm3_defaultInitData =
{
  .nvmAdr = (nvm3_HalPtr_t)NVM3_BASE,
  .nvmSize = NVM3_DEFAULT_NVM_SIZE,
  .cachePtr = defaultCache,
  .cacheEntryCount = NVM3_DEFAULT_CACHE_SIZE,
  .maxObjectSize = NVM3_DEFAULT_MAX_OBJECT_SIZE,
  .repackHeadroom = NVM3_DEFAULT_REPACK_HEADROOM,
  // In TrustZone Secure Key Library solutions, the Secure Library owns the flash handle.
  // Setting the flash handle to NULL here indicates that the flash handle on the secure
  // side should be used.
  .halHandle = NULL,
};

nvm3_Init_t *nvm3_defaultInit = &nvm3_defaultInitData;

Ecode_t nvm3_initDefault(void)
{
    Ecode_t nvm3_status;
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_NVM3_INITDEFAULT_SID;

    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id)},
        // Pass the defaultInitData from NS and have S set it up for the default
        // instance in S.
        { nvm3_defaultInit, sizeof(nvm3_defaultInitData)},
    };
    psa_outvec out_vec[] = {
        { &nvm3_status, sizeof(nvm3_status) },
    };

    psa_status_t status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_nvm3,
                                                    (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                                    (uint32_t)out_vec, IOVEC_LEN(out_vec));

    // A PSA error is returned if parameter validation failed. Otherwise the return status
    // will be in the nvm3_status.
    if (status != PSA_SUCCESS) {
      if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
          // The failure occured in the dispath function.
          // For now we return the same error as a failure in the interface function.
          return ECODE_NVM3_ERR_PARAMETER;
      }
      return ECODE_NVM3_ERR_PARAMETER;
    }

    return nvm3_status;
}

Ecode_t nvm3_deinitDefault(void)
{
    Ecode_t nvm3_status;
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_NVM3_DEINITDEFAULT_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id)},
    };
    psa_outvec out_vec[] = {
        { &nvm3_status, sizeof(nvm3_status) },
    };

    psa_status_t status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_nvm3,
                                                    (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                                    (uint32_t)out_vec, IOVEC_LEN(out_vec));

    // A PSA error is returned if parameter validation failed. Otherwise the return status
    // will be in the nvm3_status.
    if (status != PSA_SUCCESS) {
      if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
          // The failure occured in the dispath function.
          // For now we return the same error as a failure in the interface function.
          return ECODE_NVM3_ERR_PARAMETER;
      }
      return ECODE_NVM3_ERR_PARAMETER;
    }

    return nvm3_status;
}

#if defined(SL_CATALOG_TZ_SERVICE_NVM3_PRESENT)
Ecode_t nvm3_readPartialData(nvm3_Handle_t* h, nvm3_ObjectKey_t key, void* value, size_t ofs, size_t len)
{
    Ecode_t nvm3_status;
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_NVM3_READPARTIALDATA_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id) },
        { h,    sizeof(nvm3_Handle_t) },
        { &key, sizeof(key) },
        { &ofs, sizeof(ofs) },
    };
    psa_outvec out_vec[] = {
        { &nvm3_status, sizeof(nvm3_status) },
        { value, len },
    };

    psa_status_t status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_nvm3,
                                                    (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                                    (uint32_t)out_vec, IOVEC_LEN(out_vec));

    // A PSA error is returned if parameter validation failed. Otherwise the return status
    // will be in the nvm3_status.
    if (status != PSA_SUCCESS) {
      if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
          // The failure occured in the dispath function.
          // For now we return the same error as a failure in the interface function.
          return ECODE_NVM3_ERR_PARAMETER;
      }
      return ECODE_NVM3_ERR_PARAMETER;
    }

    return nvm3_status;
}


Ecode_t nvm3_readData(nvm3_Handle_t *h, nvm3_ObjectKey_t key, void *value, size_t len)
{
    Ecode_t nvm3_status;
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_NVM3_READDATA_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id) },
        { h,    sizeof(nvm3_Handle_t) },
        { &key, sizeof(key) },
    };
    psa_outvec out_vec[] = {
        { &nvm3_status, sizeof(nvm3_status) },
        { value, len },
    };

    psa_status_t status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_nvm3,
                                                    (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                                    (uint32_t)out_vec, IOVEC_LEN(out_vec));

    // A PSA error is returned if parameter validation failed. Otherwise the return status
    // will be in the nvm3_status.
    if (status != PSA_SUCCESS) {
      if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
          // The failure occured in the dispath function.
          // For now we return the same error as a failure in the interface function.
          return ECODE_NVM3_ERR_PARAMETER;
      }
      return ECODE_NVM3_ERR_PARAMETER;
    }

    return nvm3_status;
}

Ecode_t nvm3_writeData(nvm3_Handle_t *h, nvm3_ObjectKey_t key, const void *value, size_t len)
{
    Ecode_t nvm3_status;
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_NVM3_WRITEDATA_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id) },
        { h,     sizeof(nvm3_Handle_t) },
        { &key,  sizeof(key) },
        { value, len },
    };
    psa_outvec out_vec[] = {
        { &nvm3_status, sizeof(nvm3_status) },
    };

    psa_status_t status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_nvm3,
                                                    (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                                    (uint32_t)out_vec, IOVEC_LEN(out_vec));

    // A PSA error is returned if parameter validation failed. Otherwise the return status
    // will be in the nvm3_status.
    if (status != PSA_SUCCESS) {
      if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
          // The failure occured in the dispath function.
          // For now we return the same error as a failure in the interface function.
          return ECODE_NVM3_ERR_PARAMETER;
      }
      return ECODE_NVM3_ERR_PARAMETER;
    }

    return nvm3_status;
}

Ecode_t nvm3_deleteObject(nvm3_Handle_t *h, nvm3_ObjectKey_t key)
{
    Ecode_t nvm3_status;
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_NVM3_DELETEOBJECT_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id) },
        { h,     sizeof(nvm3_Handle_t) },
        { &key,  sizeof(key) },
    };
    psa_outvec out_vec[] = {
        { &nvm3_status, sizeof(nvm3_status) },
    };

    psa_status_t status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_nvm3,
                                                    (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                                    (uint32_t)out_vec, IOVEC_LEN(out_vec));

    // A PSA error is returned if parameter validation failed. Otherwise the return status
    // will be in the nvm3_status.
    if (status != PSA_SUCCESS) {
      if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
          // The failure occured in the dispath function.
          // For now we return the same error as a failure in the interface function.
          return ECODE_NVM3_ERR_PARAMETER;
      }
      return ECODE_NVM3_ERR_PARAMETER;
    }

    return nvm3_status;
}

Ecode_t nvm3_open(nvm3_Handle_t *h, const nvm3_Init_t *i)
{
    if (h == nvm3_defaultHandle) {
      if (i != nvm3_defaultInit) {
        // The NVM3 default handle is special in the sense that it is actually owned by
        // the secure library, and the nvm3_defaultHandle here is just used to indicate that
        // the internal default handle should be used. However, we need to use nvm3_initDefault()
        // to indicate to the secure library that the special default handle should be
        // initialized. This can't be done currently if the init struct is anything other than
        // the default init struct.
        return ECODE_NVM3_ERR_PARAMETER;
      }
      return nvm3_initDefault();
    }

    Ecode_t nvm3_status;
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_NVM3_OPEN_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id) },
        { i, sizeof(i) },
    };
    psa_outvec out_vec[] = {
        { &nvm3_status, sizeof(nvm3_status) },
        { h,            sizeof(nvm3_Handle_t) },
    };

    psa_status_t status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_nvm3,
                                                    (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                                    (uint32_t)out_vec, IOVEC_LEN(out_vec));

    // A PSA error is returned if parameter validation failed. Otherwise the return status
    // will be in the nvm3_status.
    if (status != PSA_SUCCESS) {
      if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
          // The failure occured in the dispath function.
          // For now we return the same error as a failure in the interface function.
          return ECODE_NVM3_ERR_PARAMETER;
      }
      return ECODE_NVM3_ERR_PARAMETER;
    }

    return nvm3_status;
}

Ecode_t nvm3_close(nvm3_Handle_t *h)
{
    if (h == nvm3_defaultHandle) {
      return nvm3_deinitDefault();
    }

    Ecode_t nvm3_status;
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_NVM3_CLOSE_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id) },
        { h, sizeof(nvm3_Handle_t) },
    };
    psa_outvec out_vec[] = {
        { &nvm3_status, sizeof(nvm3_status) },
    };

    psa_status_t status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_nvm3,
                                                    (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                                    (uint32_t)out_vec, IOVEC_LEN(out_vec));

    // A PSA error is returned if parameter validation failed. Otherwise the return status
    // will be in the nvm3_status.
    if (status != PSA_SUCCESS) {
      if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
          // The failure occured in the dispath function.
          // For now we return the same error as a failure in the interface function.
          return ECODE_NVM3_ERR_PARAMETER;
      }
      return ECODE_NVM3_ERR_PARAMETER;
    }

    return nvm3_status;
}

Ecode_t nvm3_getObjectInfo(nvm3_Handle_t *h, nvm3_ObjectKey_t key,
                           uint32_t *type, size_t *len)
{
    Ecode_t nvm3_status;
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_NVM3_GETOBJECTINFO_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id) },
        { h,    sizeof(nvm3_Handle_t) },
        { &key, sizeof(key) },
    };
    psa_outvec out_vec[] = {
        { &nvm3_status, sizeof(nvm3_status) },
        { type,         sizeof(type) },
        { len,          sizeof(len) },
    };

    psa_status_t status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_nvm3,
                                                    (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                                    (uint32_t)out_vec, IOVEC_LEN(out_vec));

    // A PSA error is returned if parameter validation failed. Otherwise the return status
    // will be in the nvm3_status.
    if (status != PSA_SUCCESS) {
      if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
          // The failure occured in the dispath function.
          // For now we return the same error as a failure in the interface function.
          return ECODE_NVM3_ERR_PARAMETER;
      }
      return ECODE_NVM3_ERR_PARAMETER;
    }

    return nvm3_status;
}

Ecode_t nvm3_writeCounter(nvm3_Handle_t *h, nvm3_ObjectKey_t key, uint32_t value)
{
    Ecode_t nvm3_status;
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_NVM3_WRITECOUNTER_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id) },
        { h,      sizeof(nvm3_Handle_t) },
        { &key,   sizeof(key) },
        { &value, sizeof(value) },
    };
    psa_outvec out_vec[] = {
        { &nvm3_status, sizeof(nvm3_status) },
    };

    psa_status_t status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_nvm3,
                                                    (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                                    (uint32_t)out_vec, IOVEC_LEN(out_vec));

    // A PSA error is returned if parameter validation failed. Otherwise the return status
    // will be in the nvm3_status.
    if (status != PSA_SUCCESS) {
      if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
          // The failure occured in the dispath function.
          // For now we return the same error as a failure in the interface function.
          return ECODE_NVM3_ERR_PARAMETER;
      }
      return ECODE_NVM3_ERR_PARAMETER;
    }

    return nvm3_status;
}

Ecode_t nvm3_readCounter(nvm3_Handle_t *h, nvm3_ObjectKey_t key, uint32_t *value)
{
    Ecode_t nvm3_status;
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_NVM3_READCOUNTER_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id) },
        { h,      sizeof(nvm3_Handle_t) },
        { &key,   sizeof(key) },
    };
    psa_outvec out_vec[] = {
        { &nvm3_status, sizeof(nvm3_status) },
        { value,        sizeof(value) },
    };

    psa_status_t status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_nvm3,
                                                    (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                                    (uint32_t)out_vec, IOVEC_LEN(out_vec));

    // A PSA error is returned if parameter validation failed. Otherwise the return status
    // will be in the nvm3_status.
    if (status != PSA_SUCCESS) {
      if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
          // The failure occured in the dispath function.
          // For now we return the same error as a failure in the interface function.
          return ECODE_NVM3_ERR_PARAMETER;
      }
      return ECODE_NVM3_ERR_PARAMETER;
    }

    return nvm3_status;
}

Ecode_t nvm3_incrementCounter(nvm3_Handle_t *h, nvm3_ObjectKey_t key, uint32_t *newValue)
{
    Ecode_t nvm3_status;
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_NVM3_INCREMENTCOUNTER_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id) },
        { h,    sizeof(nvm3_Handle_t) },
        { &key, sizeof(key) },
    };
    psa_outvec out_vec[] = {
        { &nvm3_status, sizeof(nvm3_status) },
        // Make a double pointer here to prevent TFM from failing if newValue == NULL,
        // which is valid according to the NVM3 spec.
        { &newValue,    sizeof(newValue) },
    };

    psa_status_t status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_nvm3,
                                                    (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                                    (uint32_t)out_vec, IOVEC_LEN(out_vec));

    // A PSA error is returned if parameter validation failed. Otherwise the return status
    // will be in the nvm3_status.
    if (status != PSA_SUCCESS) {
      if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
          // The failure occured in the dispath function.
          // For now we return the same error as a failure in the interface function.
          return ECODE_NVM3_ERR_PARAMETER;
      }
      return ECODE_NVM3_ERR_PARAMETER;
    }

    return nvm3_status;
}

Ecode_t nvm3_eraseAll(nvm3_Handle_t *h)
{
    Ecode_t nvm3_status;
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_NVM3_ERASEALL_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id) },
        { h,      sizeof(nvm3_Handle_t) },
    };
    psa_outvec out_vec[] = {
        { &nvm3_status, sizeof(nvm3_status) },
    };

    psa_status_t status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_nvm3,
                                                    (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                                    (uint32_t)out_vec, IOVEC_LEN(out_vec));

    // A PSA error is returned if parameter validation failed. Otherwise the return status
    // will be in the nvm3_status.
    if (status != PSA_SUCCESS) {
      if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
          // The failure occured in the dispath function.
          // For now we return the same error as a failure in the interface function.
          return ECODE_NVM3_ERR_PARAMETER;
      }
      return ECODE_NVM3_ERR_PARAMETER;
    }

    return nvm3_status;
}

Ecode_t nvm3_getEraseCount(nvm3_Handle_t *h, uint32_t *eraseCnt)
{
    Ecode_t nvm3_status;
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_NVM3_GETERASECOUNT_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id) },
        { h, sizeof(nvm3_Handle_t) },
    };
    psa_outvec out_vec[] = {
        { &nvm3_status, sizeof(nvm3_status) },
        { eraseCnt,     sizeof(eraseCnt) },
    };

    psa_status_t status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_nvm3,
                                                    (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                                    (uint32_t)out_vec, IOVEC_LEN(out_vec));

    // A PSA error is returned if parameter validation failed. Otherwise the return status
    // will be in the nvm3_status.
    if (status != PSA_SUCCESS) {
      if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
          // The failure occured in the dispath function.
          // For now we return the same error as a failure in the interface function.
          return ECODE_NVM3_ERR_PARAMETER;
      }
      return ECODE_NVM3_ERR_PARAMETER;
    }

    return nvm3_status;
}

void nvm3_setEraseCount(uint32_t eraseCnt)
{
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_NVM3_SETERASECOUNT_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id) },
        { &eraseCnt, sizeof(eraseCnt) },
    };

    psa_status_t status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_nvm3,
                                                    (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                                    (uint32_t)NULL, 0);

    // A PSA error is returned if parameter validation failed. However, we can't really
    // propagate this error to the calling function.
    if (status != PSA_SUCCESS) {
      // Do nothing...
    }
}

Ecode_t nvm3_repack(nvm3_Handle_t *h)
{
    Ecode_t nvm3_status;
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_NVM3_REPACK_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id) },
        { h, sizeof(nvm3_Handle_t) },
    };
    psa_outvec out_vec[] = {
        { &nvm3_status, sizeof(nvm3_status) },
    };

    psa_status_t status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_nvm3,
                                                    (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                                    (uint32_t)out_vec, IOVEC_LEN(out_vec));

    // A PSA error is returned if parameter validation failed. Otherwise the return status
    // will be in the nvm3_status.
    if (status != PSA_SUCCESS) {
      if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
          // The failure occured in the dispath function.
          // For now we return the same error as a failure in the interface function.
          return ECODE_NVM3_ERR_PARAMETER;
      }
      return ECODE_NVM3_ERR_PARAMETER;
    }

    return nvm3_status;
}

bool nvm3_repackNeeded(nvm3_Handle_t *h)
{
    bool ret_val = false;
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_NVM3_REPACKNEEDED_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id) },
        { h, sizeof(nvm3_Handle_t) },
    };
    psa_outvec out_vec[] = {
        { &ret_val, sizeof(ret_val) },
    };

    psa_status_t status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_nvm3,
                                                    (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                                    (uint32_t)out_vec, IOVEC_LEN(out_vec));

    // A PSA error is returned if parameter validation failed. Otherwise the return status
    // will be in the nvm3_status.
    if (status != PSA_SUCCESS) {
      return false;
    }

    return ret_val;
}

Ecode_t nvm3_resize(nvm3_Handle_t *h, nvm3_HalPtr_t newAddr, size_t newSize)
{
    Ecode_t nvm3_status;
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_NVM3_RESIZE_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id) },
        { h, sizeof(nvm3_Handle_t) },
        { &newAddr, sizeof(newAddr) },
        { &newSize, sizeof(newSize) },
    };
    psa_outvec out_vec[] = {
        { &nvm3_status, sizeof(nvm3_status) },
    };

    psa_status_t status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_nvm3,
                                                    (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                                    (uint32_t)out_vec, IOVEC_LEN(out_vec));

    if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
      return ECODE_NVM3_ERR_PARAMETER;
    }

    // A PSA error is returned if parameter validation failed. Otherwise the error will
    // be in the nvm3_status.
    if (status != PSA_SUCCESS) {
      return ECODE_NVM3_ERR_PARAMETER;
    }

    return nvm3_status;
}

size_t nvm3_enumObjects(nvm3_Handle_t *h,
                        nvm3_ObjectKey_t *keyListPtr, size_t keyListSize,
                        nvm3_ObjectKey_t keyMin, nvm3_ObjectKey_t keyMax)
{
    size_t ret_val = 0;
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_NVM3_ENUMOBJECTS_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id) },
        { h,            sizeof(nvm3_Handle_t) },
        { &keyListSize, sizeof(keyListSize) },
        { &keyMin,      sizeof(keyMin) },
        { &keyMax,      sizeof(keyMax) },
    };
    psa_outvec out_vec[] = {
        { &ret_val,    sizeof(ret_val) },
        // Make a double pointer here to prevent TFM from failing if keyListPtr == NULL,
        // which is valid when keyListSize == 0.
        { &keyListPtr, sizeof(keyListPtr) },
    };

    psa_status_t status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_nvm3,
                                                    (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                                    (uint32_t)out_vec, IOVEC_LEN(out_vec));

    // A PSA error is returned if parameter validation failed. Otherwise the error will
    // be in the nvm3_status.
    if (status != PSA_SUCCESS) {
      return 0;
    }

    return ret_val;
}

size_t nvm3_enumDeletedObjects(nvm3_Handle_t *h,
                               nvm3_ObjectKey_t *keyListPtr, size_t keyListSize,
                               nvm3_ObjectKey_t keyMin, nvm3_ObjectKey_t keyMax)
{
    size_t ret_val = 0;
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_NVM3_ENUMDELETEDOBJECTS_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id) },
        { h,            sizeof(nvm3_Handle_t) },
        { &keyListSize, sizeof(keyListSize) },
        { &keyMin,      sizeof(keyMin) },
        { &keyMax,      sizeof(keyMax) },
    };
    psa_outvec out_vec[] = {
        { &ret_val,    sizeof(ret_val) },
        // Make a double pointer here to prevent TFM from failing if keyListPtr == NULL,
        // which is valid when keyListSize == 0.
        { &keyListPtr, sizeof(nvm3_ObjectKey_t *) },
    };

    psa_status_t status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_nvm3,
                                                    (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                                    (uint32_t)out_vec, IOVEC_LEN(out_vec));

    // A PSA error is returned if parameter validation failed. Otherwise the error will
    // be in the nvm3_status.
    if (status != PSA_SUCCESS) {
      return 0;
    }

    return ret_val;
}
#endif // defined(SL_CATALOG_TZ_SERVICE_NVM3_PRESENT)
