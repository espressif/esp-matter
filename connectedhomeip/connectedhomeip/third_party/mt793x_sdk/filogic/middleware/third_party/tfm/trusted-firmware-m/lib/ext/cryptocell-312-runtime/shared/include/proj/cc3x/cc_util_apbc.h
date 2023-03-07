/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _CC_UTIL_APBC_H_
#define _CC_UTIL_APBC_H_

/*!
@file
@defgroup cc_apbc_defs CryptoCell APBC macros
@brief This file contains APBC definitions.
@{
@ingroup cryptocell_api

*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_pal_apbc.h"

/*! Get APBC Access counter. Return number of active APBC accesses operations */
#define CC_APBC_CNTR_GET   CC_PalApbcCntrValue()

/*! Increment APBC access counter. */
#define CC_APBC_ACCESS_INC  CC_PalApbcModeSelect(CC_TRUE)

/*! Decrement APBC access counter. */
#define CC_APBC_ACCESS_DEC  CC_PalApbcModeSelect(CC_FALSE)


#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif /*_CC_UTIL_APBC_H_*/
