/******************************************************************************
*  Filename:       rom_bignum.h
*  Revised:        2016-09-19 10:36:17 +0200 (Mon, 19 Sep 2016)
*  Revision:       47179
*
*  Description:    Prototypes for arbitrary-precision arithmetic ROM functions
*                  for use by third party ECC library. These are not meant to
*                  be called directly by drivers, stacks, or application code.
*
*  Copyright (c) 2015 - 2021, Texas Instruments Incorporated
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*  3) Neither the name of the ORGANIZATION nor the names of its contributors may
*     be used to endorse or promote products derived from this software without
*     specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
*  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

#ifndef __ROM_BIGNUM_H__
#define __ROM_BIGNUM_H__

#include <stdint.h>
#include "rom.h"

/** @defgroup ecclib_lowlevel Low-Level functions from ECC library in ROM.
 *  @{
 */

//*****************************************************************************
//!
//!  @brief   Perform a modular inversion of the element at address in in1
//!
//!  @param state   pointer to ECC state object
//!  @param in1     pointer to value to invert
//!
//!  @return pointer to inverted value (same as in1)
//!
//*****************************************************************************
uint32_t* INVERT(ECC_State *state, uint32_t *in1);

//*****************************************************************************
//!
//!  @brief Imports the integer in LV format found at address in1 into address out in modulus
//!         format [0, 0, LSW, ..., MSW]. Returns the wordsize of the imported modulus.
//!
//!  @param out     points where the modulus is to be written
//!  @param in1     pointer to integer in LV format [L, LSW, ..., MSW]
//!
//!  @return wordsize of the imported modulus
//!
//*****************************************************************************
uint32_t IMPORTMODULUS(uint32_t * out, const uint32_t *in1);

//*****************************************************************************
//!
//!  @brief Imports the integer in LV format found at address in1 into address out in operand
//!         format [LSW, ..., MSW, 0, ..., 0], making sure that the obtained array has LEN+1
//!         words, thus truncating or right-padding with zeroes as needed. Returns the wordsize
//!         of the imported integer.
//!
//!  @param state   pointer to ECC state object
//!  @param out     points where the operand is to be written
//!  @param in1     pointer to integer in LV format [L, LSW, ..., MSW]
//!
//!  @return wordsize of the imported integer
//!
//*****************************************************************************
uint32_t IMPORTOPERAND(ECC_State *state, uint32_t *out, const uint32_t *in1);

//*****************************************************************************
//!
//!  @brief Exports the integer found at address in1 to address out in LV
//!         format [LEN, LSW, ..., MSW]. Returns LEN.
//!
//!  @param  state   pointer to ECC state object
//!  @param  in1     pointer to integer
//!  @param  out     points where the LV format integer is to be written
//!
//!  @return LEN (wordsize length) from ECC state object
//!
//*****************************************************************************
uint32_t EXPORTOPERAND(ECC_State *state, uint32_t *out, const uint32_t *in1);

//*****************************************************************************
//!
//!  @brief Writes the 32-bit integer in aux at address out in operand format
//!
//!  @param state     pointer to ECC state object
//!  @param out       points where the integer is to be written
//!  @param inValue   word to copy
//!
//*****************************************************************************
void SETOPERAND (ECC_State *state, uint32_t *out, uint32_t inValue);

//*****************************************************************************
//!
//!  @brief Initializes the modulus at address in1 for later modular operations. Completes the
//!         modulus with the modulus-specific constant alpha = -1/m mod 2^64.
//!
//!  @param state   pointer to ECC state object
//!  @param in1     pointer to modulus in format [0, 0, LSW(m), ..., MSW(m)]
//!  @param size    indicates the wordsize of the modulus
//!
//!  @note In simulation mode, only al0 = alpha mod 2^32 is computed. The routine uses the
//!        Newton-based inductive formula alpha = alpha * (m[0] * alpha + 2) mod 2^32
//!        repeated 6 times where alpha is previously set to 1.
//*****************************************************************************
void mSET(ECC_State *state, uint32_t *in1, uint32_t size);

//*****************************************************************************
//!
//!  @brief  Montgomery modular multiplication out = in1 * in2 * K mod MOD where K is the
//!      Montgomery factor K = 2^-(32*LEN) mod MOD.
//!
//!  @param out     points anywhere in memory except in1 or in2
//!  @param in1     pointer to multiplicator in operand format
//!  @param in2     pointer to multiplicand in operand format
//!  @param mod     pointer to modulus
//!  @param len     word length of operands
//!
//*****************************************************************************
void mMULT(uint32_t *out, const uint32_t *in1, const uint32_t *in2, uint32_t *mod, uint32_t len);

//*****************************************************************************
//!
//!  @brief  Computes out = in1 - in2 mod MOD
//!
//!  @param out     pointer to result to be written in operand format
//!  @param in1     pointer to first operand in operand format
//!  @param in2     pointer to second operand in operand format
//!  @param mod     pointer to modulus
//!  @param len     word length of operands
//!
//!  @pre    A modulus must have been initialized by the mSET routine.
//!
//*****************************************************************************
void mSUB(uint32_t *out, const uint32_t *in1, const uint32_t *in2, uint32_t *mod, uint32_t len);

//*****************************************************************************
//!
//!  @brief  Computes out = in1 + in2 mod MOD
//!
//!  @param out     pointer to result to be written in operand format
//!  @param in1     pointer to first operand in operand format
//!  @param in2     pointer to second operand in operand format
//!  @param mod     pointer to modulus
//!  @param len     word length of operands
//!
//!  @pre    A modulus must have been initialized by the mSET routine.
//!
//*****************************************************************************
void mADD(uint32_t *out, const uint32_t *in1, const uint32_t *in2, uint32_t *mod, uint32_t len);

//*****************************************************************************
//!
//!  @brief  Computes the scrict modular remainder out = in1 mod MOD < MOD.
//!
//!  @param out     pointer to result to be written in operand format
//!  @param in1     pointer to integer in operand format
//!  @param mod     pointer to modulus
//!  @param len     word length of operands
//!
//!  @pre    A modulus must have been initialized by the mSET routine.
//!
//*****************************************************************************
void mOUT(uint32_t *out, const uint32_t *in1, uint32_t *mod, uint32_t len);

//*****************************************************************************
//!
//!  @brief Copies aux words from address in1 to address out.
//!
//!  @param out     points anywhere in memory
//!  @param in1     points anywhere in memory
//!  @param size    number of words to be copied
//!
//*****************************************************************************
void COPY(uint32_t *out, uint32_t *in1, uint32_t size);

//*****************************************************************************
//!
//!  @brief  Extract the bit of given index
//!
//!  @param in1     pointer to input
//!  @param in2     pointer to index of bit
//!
//!  @return The bit of the given index
//!
//*****************************************************************************
uint8_t GETBIT(uint32_t *in1, uint32_t *in2);

//*****************************************************************************
//!
//!  @brief  Initialization for scalar multiplication
//!
//!  @param state   pointer to ECC state object
//!
//!  @retval STATUS_SCALAR_MUL_OK
//!  @retval STATUS_MODULUS_EVEN
//!  @retval STATUS_MODULUS_MSW_IS_ZERO
//!  @retval STATUS_MODULUS_LENGTH_ZERO
//!  @retval STATUS_MODULUS_LARGER_THAN_255_WORDS
//!
//*****************************************************************************
uint8_t SCALARMUL_init(ECC_State *state);

/** @} */ /* end of ecclib_lowlevel */

#endif // __ROM_BIGNUM_H__
