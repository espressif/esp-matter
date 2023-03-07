/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


 #ifndef LLF_ECPKI_ERROR_H
#define LLF_ECPKI_ERROR_H

#include "cc_error.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* base address: LLF_ECPKI_MODULE_ERROR_BASE = 0x00F10800 */

/* The PkaEcdsaVerify related functions errors */
#define  ECWRST_DSA_VERIFY_CALC_SIGN_C_INVALID_ERROR        (LLF_ECPKI_MODULE_ERROR_BASE + 0x01UL)
#define  ECWRST_DSA_VERIFY_CALC_SIGN_D_INVALID_ERROR        (LLF_ECPKI_MODULE_ERROR_BASE + 0x02UL)
#define  ECWRST_DSA_VERIFY_CALC_SIGNATURE_IS_INVALID            (LLF_ECPKI_MODULE_ERROR_BASE + 0x03UL)
#define  ECWRST_DSA_VERIFY_2MUL_FIRST_B2_ERROR              (LLF_ECPKI_MODULE_ERROR_BASE + 0x04UL)
#define  ECWRST_DSA_VERIFY_2MUL_NEXT_B2_ERROR               (LLF_ECPKI_MODULE_ERROR_BASE + 0x05UL)
#define  ECWRST_DSA_VERIFY_2MUL_FACTOR_A_NULL_ERROR             (LLF_ECPKI_MODULE_ERROR_BASE + 0x06UL)
#define  ECWRST_DSA_VERIFY_2MUL_FACTOR_B_NULL_ERROR             (LLF_ECPKI_MODULE_ERROR_BASE + 0x07UL)
/* The CalcSignature function errors */
#define  ECWRST_DSA_SIGN_BAD_EPHEMER_KEY_TRY_AGAIN_ERROR        (LLF_ECPKI_MODULE_ERROR_BASE + 0x10UL)
#define  ECWRST_DSA_SIGN_CALC_CANNOT_CREATE_SIGNATURE       (LLF_ECPKI_MODULE_ERROR_BASE + 0x11UL)

/* The EcWrstDhDeriveSharedSecret function errors */
#define ECWRST_DH_SHARED_VALUE_IS_ON_INFINITY_ERROR     (LLF_ECPKI_MODULE_ERROR_BASE + 0x20UL)

/* The PkaEcWrstScalarMult function errors */
#define ECWRST_SCALAR_MULT_INVALID_SCALAR_VALUE_ERROR           (LLF_ECPKI_MODULE_ERROR_BASE + 0x30UL)
#define ECWRST_SCALAR_MULT_INVALID_MOD_ORDER_SIZE_ERROR         (LLF_ECPKI_MODULE_ERROR_BASE + 0x31UL)

#ifdef __cplusplus
}
#endif

#endif

