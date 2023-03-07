/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef  _BYPASS_DRIVER_H
#define  _BYPASS_DRIVER_H

#include "driver_defs.h"

/******************************************************************************
*               FUNCTION PROTOTYPES
******************************************************************************/
/****************************************************************************************************/
/**
 * @brief This function is used to perform the BYPASS operation in one integrated process.
 *
 *
 * @param[in] pInputBuffInfo A structure which represents the data input buffer.
 * @param[in] inputDataAddrType - the memory address input type: SRAM_ADDR or DLLI_ADDR.
 * @param[in] pOutputBuffInfo A structure which represents the data output buffer.
 * @param[in] outputDataAddrType - the memory address input type: SRAM_ADDR or DLLI_ADDR.
 * @param[in] blockSize - number of bytes to copy.
 *
 * @return drvError_t - On success BYPASS_DRV_OK is returned, on failure a value defined in driver_defs.h
 *
 */
drvError_t ProcessBypass(CCBuffInfo_t *pInputBuffInf, dataAddrType_t inputDataAddrType,
                         CCBuffInfo_t *pOutputBuffInfo, dataAddrType_t outputDataAddrType,
                         uint32_t blockSize);

#endif /* _BYPASS_DRIVER_H */

