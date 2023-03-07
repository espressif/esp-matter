/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define CC_PAL_LOG_CUR_COMPONENT HOST_LOG_MASK_SECURE_BOOT

/************* Include Files ****************/
#include "rsa_pki_pka.h"
#include "rsa_bsv.h"

/************************ Defines ******************************/

/************************ Enums ******************************/

/************************ Typedefs ******************************/

/************************ Global Data ******************************/

#ifdef PKA_DEBUG
  extern uint8_t tempRes[SB_CERT_RSA_KEY_SIZE_IN_WORDS+2*RSA_PKA_BIG_WORD_SIZE_IN_32_BIT_WORDS];
  extern uint32_t g_SramPkaAddr;
#endif

/* ********** Internal private functions ****************** */


/* ************************** public functions ********************************* */


/**
 * @brief The RSA_CalcExponent calculates The following:
 *
 *                   Res = (Base ^ Exp) mod N. ( Exp = 0x10001 )
 *
 *        The calculation is done in a secured way using the PIC.
 *
 * @hwBaseAddress        - Cryptocell base address
 * @Base_ptr[in]         - The pointer to the base buffer.
 * @N_ptr[in]            - The pointer to the modulus buffer.
 * @Np_ptr[in]           - The Np vector buffer..
 *                       - Its size must be 160.
 * @Res_ptr[out]         - The pointer to the buffer that will contain the result.
 *
 * @return CCError_t - On success CC_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
void RSA_CalcExponent( unsigned long hwBaseAddress,
                       uint32_t *Base_ptr,
                       uint32_t *N_ptr,
                       uint32_t *Np_ptr,
                       uint32_t *Res_ptr )
{

   /* Fix value for the Exponent */
   uint32_t Exp = RSA_EXP_VAL;
   uint32_t regsCount = 7; /*5 working + 2 temp registers*/

  /* FUNCTION LOGIC */

   /* .................... initialize local variables ...................... */
   /* ---------------------------------------------------------------------- */

    /* initialize the PKA engine on default mode */
    RSA_PKA_InitPka(SB_CERT_RSA_KEY_SIZE_IN_BITS, regsCount, hwBaseAddress);

    /* copy modulus N into r0 register */
    RSA_HW_PKI_PKA_CopyDataIntoPkaReg( 0/*dstReg*/, 1/*LenID*/,
                   N_ptr/*src_ptr*/, SB_CERT_RSA_KEY_SIZE_IN_WORDS, hwBaseAddress );

    /* copy the NP into r1 register NP */
    RSA_HW_PKI_PKA_CopyDataIntoPkaReg( 1/*dstReg*/, 1/*LenID*/, Np_ptr/*src_ptr*/,
                      RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_WORDS, hwBaseAddress );

    /* copy input data into PKI register: DataIn=>r2 */
    RSA_HW_PKI_PKA_CopyDataIntoPkaReg( 2/*dstReg*/, 1/*LenID*/,
             Base_ptr, SB_CERT_RSA_KEY_SIZE_IN_WORDS, hwBaseAddress );

    /* copy exponent data PKI register: e=>r3 */
    RSA_HW_PKI_PKA_CopyDataIntoPkaReg( 3/*dstReg*/, 1/*LenID*/,
             &Exp, RSA_EXP_SIZE_WORDS, hwBaseAddress );


    /* .. calculate the exponent Res = OpA**OpB mod N;                  ... */
    /* -------------------------------------------------------------------- */
    RSA_HW_PKI_PKA_ModExp( 0/*LenID*/, 2/*OpA*/, 3/*OpB*/, 4/*Res*/, 0/*Tag*/, hwBaseAddress );

    /* copy result into output: r4 =>DataOut */
    RSA_HW_PKI_PKA_CopyDataFromPkaReg( Res_ptr, SB_CERT_RSA_KEY_SIZE_IN_WORDS,
                   4/*srcReg*/, hwBaseAddress );

    /* Finish PKA operations (waiting PKI done and close PKA clocks) */
    RSA_HW_PKI_PKA_FinishPKA( hwBaseAddress );

}/* END OF RSA_CalcExponent */
