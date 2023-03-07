/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef  _CC_RNG_PLAT_H
#define  _CC_RNG_PLAT_H

#include "cc_rnd_local.h"
#include "cc_plat.h"

/****************  Defines  ********************/
#define CC_RNG_OTP_SUB_SAMPL_RATIO_BIT_SIZE     7
#define CC_RNG_OTP_SUB_SAMPL_RATIO_MAX_VAL      ((1UL << CC_RNG_OTP_SUB_SAMPL_RATIO_BIT_SIZE) - 1)

/* Default TRNG parameters: used when in OTP set 0 in appropriate bits */
#define CC_RNG_DEFAULT_ROSCS_ALLOWED_FLAG       0xF

/* Default, increment and mininimal values, for Sampling Ratio */

/* On  FE mode */
#define CC_RNG_DEFAULT_SAMPL_RATIO_ON_FE_MODE 1000
#define CC_RNG_SAMPL_RATIO_INCREM_ON_FE_MODE  50
#define CC_RNG_MIN_SAMPL_RATIO_ON_FE_MODE     1000

/* Maximal value of SamplingRatio */
#define CC_RNG_MAX_SAMPL_RATIO_ON_SWEE_MODE     CC_RNG_OTP_SUB_SAMPL_RATIO_MAX_VAL
#define CC_RNG_MAX_SAMPL_RATIO_ON_FE_MODE     (CC_RNG_MIN_SAMPL_RATIO_ON_FE_MODE + \
                        CC_RNG_SAMPL_RATIO_INCREM_ON_FE_MODE * CC_RNG_OTP_SUB_SAMPL_RATIO_MAX_VAL)

/****************************************************************************************/
/**
 *
 * @brief The function retrievess the TRNG parameters, provided by the User trough NVM,
 *        and sets them into structures given by pointers rndState_ptr and trngParams_ptr.
 *
 * @author reuvenl (6/26/2012)
 *
 * @param[out] trngParams_ptr - The pointer to structure, containing parameters
 *                            of HW TRNG.
 *
 * @return CCError_t - no return value
 */
CCError_t RNG_PLAT_SetUserRngParameters(
                        CCRndParams_t  *pTrngParams);


#endif  /* _CC_RNG_PLAT_H */
