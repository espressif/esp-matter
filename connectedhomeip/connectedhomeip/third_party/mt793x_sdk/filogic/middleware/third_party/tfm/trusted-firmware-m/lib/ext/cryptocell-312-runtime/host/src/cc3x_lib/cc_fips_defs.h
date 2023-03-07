/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef  _CC_FIPS_DEFS_H
#define  _CC_FIPS_DEFS_H

//empty macro since FIPS not supported
#define CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR()
#define CHECK_AND_RETURN_UPON_FIPS_ERROR()
#define CHECK_AND_RETURN_UPON_FIPS_STATE()
#define CHECK_FIPS_SUPPORTED(supported) {supported = false;}
#define FIPS_RSA_VALIDATE(rndContext_ptr,pCcUserPrivKey,pCcUserPubKey,pFipsCtx)  (CC_OK)
#define FIPS_ECC_VALIDATE(pRndContext, pUserPrivKey, pUserPublKey, pFipsCtx)  (CC_OK)
#define CC_FIPS_SET_RND_CONT_ERR()

#endif  // _CC_FIPS_DEFS_H

