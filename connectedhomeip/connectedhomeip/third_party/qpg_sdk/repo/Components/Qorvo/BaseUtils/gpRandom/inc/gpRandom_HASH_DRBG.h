/*
 * Copyright (c) 2017, Qorvo Inc
 *
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/**
 *@defgroup DEF_RANDOM_HASH_DRBG General Random CTR DRBG Definitions
 * @brief The general Random HASH DRBG (Deterministic Random Bit Generator) Definitions
 *
 * The gpRandom_HASH_DRBG component is intended for generation of the random number sequences as specified in SP 800 90
 *
 *@defgroup DEF_RANDOM_HASH_DRBG General Random Public Functions
 * @brief The general Random CTR DRBG functionality is implemented in these functions
 */


#ifndef _GPRANDOM_HASH_DRBG_H_
#define _GPRANDOM_HASH_DRBG_H_

/** @name gpRandom_Result_t */
//@{
/** @brief The function returned successful. */
#define gpRandom_ResultSuccess             gpHal_ResultSuccess
/** @brief An invalid parameter was given as a parameter to this function. */
#define gpRandom_ResultInvalidParameter    gpHal_ResultInvalidParameter
/** @brief The GP chip is busy. */
#define gpRandom_ResultBusy                gpHal_ResultBusy


#ifdef __cplusplus
extern "C" {
#endif


/**@ingroup DEF_RANDOM_HASH_DRBG
* @brief This function fills a buffer of given size with entropy
*  @param buf: reference to memory location where fresh entropy is to be stored
*  @param size: the number of bytes in the entropy buffer
*/
void gpRandom_FillEntropyBuffer(UInt8* buf, UInt16 size);

 /**@ingroup DEF_RANDOM_HASH_DRBG
 * @brief This function Initializes the Hash DRBG
 *
 * @return
 *          - gpRandom_ResultSuccess
 *          - gpRandom_ResultBusy
 */
gpRandom_Result_t gpRandom_HASH_DRBG_Init(void);

/**@ingroup DEF_RANDOM_HASH_DRBG
 * @brief This function implements the reseed procedure for the HASH DRBG
 * @return
 *          - gpRandom_ResultSuccess
 *          - gpRandom_ResultBusy
 */
gpRandom_Result_t gpRandom_HASH_DRBG_Reseed(void);

/**@ingroup DEF_RANDOM_HASH_DRBG
 *  @brief gpRandom_HASH_DRBG_Generate
 *  @param nmbrRandomBytes number of bytes in buffer
 *  @param pBuffer is reference to memory location where new random number sequence is to be stored
 *  @return
 *          - gpRandom_ResultSuccess
 *          - gpRandom_ResultBusy
 */
gpRandom_Result_t gpRandom_HASH_DRBG_Generate(UInt32 numbytes, UInt8* pData);

#ifdef __cplusplus
}
#endif

#endif // _GPRANDOM_HASH_DRBG_H_
