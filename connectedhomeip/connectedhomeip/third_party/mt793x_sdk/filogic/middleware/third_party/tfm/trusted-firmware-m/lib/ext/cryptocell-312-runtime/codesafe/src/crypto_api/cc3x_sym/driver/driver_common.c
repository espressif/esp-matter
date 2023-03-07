/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "driver_defs.h"
#include "cc_pal_buff_attr.h"
#include "cc_pal_abort.h"
#include "cc_error.h"

/******************************************************************************
*       PUBLIC FUNCTIONS
******************************************************************************/


drvError_t SetDataBuffersInfo(const uint8_t *pDataIn, size_t dataInSize, CCBuffInfo_t *pInputBuffInfo,
                              const uint8_t *pDataOut, size_t dataOutSize, CCBuffInfo_t *pOutputBuffInfo)
{
    drvError_t drvRet = CC_OK;
    uint8_t  buffNs = 0;

    drvRet = CC_PalDataBufferAttrGet(pDataIn, dataInSize, INPUT_DATA_BUFFER, &buffNs);
    if (drvRet != CC_OK){
        CC_PAL_LOG_ERR("input buffer memory is illegal\n");
        return CC_FATAL_ERROR;
    }
    pInputBuffInfo->dataBuffAddr = (uint32_t)pDataIn;
    pInputBuffInfo->dataBuffNs = buffNs;

    if (pOutputBuffInfo != NULL) {
        if (pDataOut != NULL) {
            drvRet = CC_PalDataBufferAttrGet(pDataOut, dataOutSize, OUTPUT_DATA_BUFFER, &buffNs);
            if (drvRet != CC_OK){
                CC_PAL_LOG_ERR("output buffer memory is illegal\n");
                return CC_FATAL_ERROR;
            }
        }
        pOutputBuffInfo->dataBuffAddr = (uint32_t)pDataOut;
        pOutputBuffInfo->dataBuffNs = buffNs;
    }

    return drvRet;
}



