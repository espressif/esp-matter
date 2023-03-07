/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define CC_PAL_LOG_CUR_COMPONENT CC_LOG_MASK_CC_API

#include "mbedtls_cc_poly.h"
#include "poly.h"
#include "mbedtls_cc_poly_error.h"


CIMPORT_C CCError_t  mbedtls_poly(
        mbedtls_poly_key         pKey,
        uint8_t                *pDataIn,
        size_t                  dataInSize,
        mbedtls_poly_mac        macRes)

{
    CCError_t rc;

    // Verify inputs
    if (pKey == NULL) {
        return CC_POLY_KEY_INVALID_ERROR;
    }
    if ((macRes == NULL) ||
        ((pDataIn == NULL) ^ (dataInSize == 0)) ||
        (dataInSize > CC_MAX_UINT32_VAL)) {
        return CC_POLY_DATA_INVALID_ERROR;
    }

    // calculate teh MAC using PKA
    rc = PolyMacCalc(pKey, NULL, 0, pDataIn, dataInSize, macRes, false);
    if (rc != CC_OK) {
        return CC_POLY_RESOURCES_ERROR;
    }

    return CC_OK;
}

