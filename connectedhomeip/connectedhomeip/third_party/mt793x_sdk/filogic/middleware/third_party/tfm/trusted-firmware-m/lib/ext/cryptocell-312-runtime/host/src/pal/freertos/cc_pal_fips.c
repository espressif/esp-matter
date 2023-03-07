/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/************* Include Files ****************/
#include "cc_pal_types.h"
#include "cc_pal_fips.h"
#include "cc_pal_mem.h"

CCFipsStateData_t   gStateData = { CC_FIPS_STATE_CRYPTO_APPROVED, CC_TEE_FIPS_ERROR_OK, CC_FIPS_TRACE_NONE };


CCError_t CC_PalFipsWaitForReeStatus(void)
{
    FipsSetReeStatus(CC_TEE_FIPS_REE_STATUS_OK);
    return CC_OK;
}

CCError_t CC_PalFipsStopWaitingRee(void)
{
    return CC_OK;
}

CCError_t CC_PalFipsGetState(CCFipsState_t *pFipsState)
{
    *pFipsState = gStateData.state;

    return CC_OK;
}


CCError_t CC_PalFipsGetError(CCFipsError_t *pFipsError)
{
    *pFipsError = gStateData.error;

    return CC_OK;
}


CCError_t CC_PalFipsGetTrace(CCFipsTrace_t *pFipsTrace)
{
    *pFipsTrace = gStateData.trace;

    return CC_OK;
}

CCError_t CC_PalFipsSetState(CCFipsState_t fipsState)
{
    gStateData.state = fipsState;

    return CC_OK;
}

CCError_t CC_PalFipsSetError(CCFipsError_t fipsError)
{
    gStateData.error = fipsError;

    return CC_OK;
}

CCError_t CC_PalFipsSetTrace(CCFipsTrace_t fipsTrace)
{
    gStateData.trace = (CCFipsTrace_t)(gStateData.trace | fipsTrace);

    return CC_OK;
}

