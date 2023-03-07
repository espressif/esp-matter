/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_INT_GENERAL_DEFS_H
#define _CC_INT_GENERAL_DEFS_H

/*!
@file
@brief This file contains internal general definitions of the CryptoCell runtime SW APIs.
@defgroup cc_general_defs CryptoCell general definitions
@{
@ingroup cryptocell_api

*/

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Macros ******************************/

/* check if fatal error bit is set to ON */
#define CC_IS_FATAL_ERR_ON(rc)\
do {\
        uint32_t regVal = 0;\
    regVal = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_AO_LOCK_BITS));\
            rc = CC_REG_FLD_GET(0, HOST_AO_LOCK_BITS, HOST_FATAL_ERR, regVal);\
            rc = (rc == 1)?CC_TRUE:CC_FALSE;\
}while(0)


#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif



