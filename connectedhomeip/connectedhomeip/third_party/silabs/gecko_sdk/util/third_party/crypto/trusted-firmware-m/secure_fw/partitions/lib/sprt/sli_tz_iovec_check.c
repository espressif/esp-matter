/***************************************************************************//**
 * @file
 * @brief Silicon Labs TrustZone Secure Library IOVEC validation.
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

#include "sli_tz_iovec_check.h"

#include <stdint.h>
#include <stdbool.h>
#include <arm_cmse.h>

//------------------------------------------------------------------------------
// Static functions

/***************************************************************************//**
 * @brief
 *   Check if an object of given size is fully encapsulated in a region of
 *   memory that is read- and writeable from the non-secure domain.
 ******************************************************************************/
static inline bool object_lives_in_ns(const void *object, size_t object_size)
{
  return cmse_check_address_range((void *)object,
                                  object_size,
                                  (CMSE_NONSECURE | CMSE_MPU_READWRITE));
}

//------------------------------------------------------------------------------
// Global functions

uint32_t sli_tz_iovecs_live_in_ns(psa_invec in_vec[],
                                  size_t in_len,
                                  psa_outvec out_vec[],
                                  size_t out_len,
                                  sli_tz_iovec_params_t *iovec_copy)
{
  // We can only handle a limited amount of iovecs per secure function call.
  if ((in_len > PSA_MAX_IOVEC)
      || (out_len > PSA_MAX_IOVEC)) {
    return SLI_TZ_IOVEC_ERROR;
  }

  // Check whether the caller partition has at write access to the iovec
  // structures themselves.
  if (in_len > 0) {
    if ((in_vec == NULL)
        || (!object_lives_in_ns(in_vec, sizeof(psa_invec)*in_len))) {
      return SLI_TZ_IOVEC_ERROR;
    }
  } else {
    if (in_vec != NULL) {
      return SLI_TZ_IOVEC_ERROR;
    }
  }
  if (out_len > 0) {
    if ((out_vec == NULL)
        || (!object_lives_in_ns(out_vec, sizeof(psa_outvec)*out_len))) {
      return SLI_TZ_IOVEC_ERROR;
    }
  } else {
    if (out_vec != NULL) {
      return SLI_TZ_IOVEC_ERROR;
    }
  }

  // Copy the iovec objects passed by NS caller.
  for (size_t i = 0; i < in_len; ++i) {
    iovec_copy->in_vec[i].base = in_vec[i].base;
    iovec_copy->in_vec[i].len = in_vec[i].len;
  }
  for (size_t i = 0; i < out_len; ++i) {
    iovec_copy->out_vec[i].base = out_vec[i].base;
    iovec_copy->out_vec[i].len = out_vec[i].len;
  }

  // Check whether the caller has access to the data inside the iovecs.
  for (size_t i = 0; i < in_len; ++i) {
    if (iovec_copy->in_vec[i].len > 0) {
      if ((iovec_copy->in_vec[i].base == NULL)
          || (!object_lives_in_ns(iovec_copy->in_vec[i].base,
                                  iovec_copy->in_vec[i].len))) {
        return SLI_TZ_IOVEC_ERROR;
      }
    }
  }
  for (size_t i = 0; i < out_len; ++i) {
    if (iovec_copy->out_vec[i].len > 0) {
      if ((iovec_copy->out_vec[i].base == NULL)
          || (!object_lives_in_ns(iovec_copy->out_vec[i].base,
                                  iovec_copy->out_vec[i].len))) {
        return SLI_TZ_IOVEC_ERROR;
      }
    }
  }

  return SLI_TZ_IOVEC_OK;
}
