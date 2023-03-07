/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "cc_pal_mem.h"
#include "cc_pal_types.h"
#include "cc_hal_plat.h"
#include "cc_common_math.h"
#include "cc_ecpki_error.h"
#include "cc_ec_mont_api.h"

#include "pka_hw_defs.h"
#include "pka.h"
#include "pki.h"
#include "pka_error.h"
#include "ec_mont.h"
#include "pka_ec_mont_glob_regs_def.h"
#include "cc_int_general_defs.h"

/* global data definitions */
extern CC_PalMutex CCAsymCryptoMutex;


/*********************************************************************/
/*!
@brief The function performs EC Montgomery (Curve25519) scalar multiplication:
       resPoint = scalar * point.

 @return CCError_t
*/
CCError_t EcMontScalarmult(
                                uint32_t *resPoint,       /* [out] pointer to result point (compressed,
                                                                   the size = ec modulus size) */
                                uint32_t *scalar,         /* [in] pointer to the scalar, the size = ec order size) */
                                uint32_t *inPoint,        /* [in] pointer to the input point (compressed,
                                                                       the size = ec modulus size)  */
                                const CCEcMontDomain_t *pEcDomain /* [in] pointer to EC domain (curve). */)
{
        CCError_t err = CC_OK;
        uint32_t pkaRegsUsed = EC_MONT_PKA_REGS_USED;
        uint32_t scalarSizeBits;

        /* get the hardware semaphore  */
        err = CC_PalMutexLock(&CCAsymCryptoMutex, CC_INFINITE);
        if (err != CC_SUCCESS) {
            CC_PalAbort("Fail to acquire mutex\n");
        }

        /* verify that the device is not in fatal error state before activating the PKA engine */
        CC_IS_FATAL_ERR_ON(err);
        if (err == CC_TRUE) {
                /* release the hardware semaphore */
                if (CC_PalMutexUnlock(&CCAsymCryptoMutex) != CC_SUCCESS) {
                        CC_PalAbort("Fail to release mutex\n");
                }
                return PKA_FATAL_ERR_STATE_ERROR;
        }

        /* increase CC counter at the beginning of each operation */
        err = CC_IS_WAKE;
        if (err != CC_SUCCESS) {
            CC_PalAbort("Fail to increase PM counter\n");
        }

        /* init PKA, mapping and sizes tables */
        err = PkaInitPka(pEcDomain->ecModSizeInBits, 0,
                          &pkaRegsUsed/*regs.count*/);

        if (err != CC_SUCCESS) {
                goto End;
        }

        scalarSizeBits = CC_CommonGetWordsCounterEffectiveSizeInBits(
                                            scalar, pEcDomain->ecOrdSizeInWords);

        /* call EC scalar multiplication (with ladder) function */
        err = EcMontPkaScalMultWithLadder(
                        resPoint,
                        scalar,
                        scalarSizeBits,
                        inPoint,
                        pEcDomain);

End:

        PkaFinishAndMutexUnlock(pkaRegsUsed);
        return err;

}

