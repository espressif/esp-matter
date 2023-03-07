/******************************************************************************
*  Filename:       rom_bignum.c
*  Revised:        2020-04-03 19:11:17 +0200 (Fri, 03 Apr 2020)
*  Revision:       57290
*
*  Description:    Access functions for arbitrary-precision arithmetic ROM functions
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

#include "rom_bignum.h"
#include "rom.h"

#include <stdint.h>

/******************************************************************************/
/***                          FUNCTION POINTERS                             ***/
/******************************************************************************/

typedef uint8_t (*scalarmul_init_t)(ECC_State *state);
static const scalarmul_init_t scalarmul_init = (scalarmul_init_t)(0x1000530f);

typedef uint32_t* (*invert_t)(ECC_State *state, uint32_t *in1);
static const invert_t invert = (invert_t)(0x10005e9d);

typedef uint32_t (*importmodulus_t)(uint32_t * out, const uint32_t *in1);
static const importmodulus_t importmodulus = (importmodulus_t)(0x10004bf1);

typedef uint32_t (*importoperand_t)(ECC_State *state, uint32_t * out, const uint32_t *in1);
static const importoperand_t importoperand = (importoperand_t)(0x10004c1b);

typedef uint32_t (*exportoperand_t)(ECC_State *state, uint32_t * out, const uint32_t * in1);
static const exportoperand_t exportoperand = (exportoperand_t)(0x10004c67);

typedef void (*setoperand_t)(ECC_State *state, uint32_t * out, uint32_t inValue);
static const setoperand_t setoperand = (setoperand_t)(0x10004cb5);

typedef void (*mset_t)(ECC_State *state, uint32_t * in1, uint32_t size);
static const mset_t mset = (mset_t)(0x10004d0f);

typedef void (*mmult_t)(uint32_t *out, const uint32_t *in1, const uint32_t *in2, uint32_t *mod, uint32_t len);
static const mmult_t mmult = (mmult_t)(0x10007229);

typedef void (*msub_t)(uint32_t *out, const uint32_t *in1, const uint32_t *in2, uint32_t *mod, uint32_t len);
static const msub_t msub = (msub_t)(0x10007875);

typedef void (*madd_t)(uint32_t *out, const uint32_t *in1, const uint32_t *in2, uint32_t *mod, uint32_t len);
static const madd_t madd = (madd_t)(0x1000714d);

typedef void (*mout_t)(uint32_t *out, const uint32_t *in1, uint32_t *mod, uint32_t len);
static const mout_t mout = (mout_t)(0x100077e1);

typedef void (*copy_t)(uint32_t * out, uint32_t *in1, uint32_t size);
static const copy_t copy = (copy_t)(0x10004cd9);

typedef uint8_t (*getbit_t)(uint32_t *in1, uint32_t *in2);
static const getbit_t getbit = (getbit_t)(0x10005a1b);

//*****************************************************************************
// INVERT
//*****************************************************************************
uint32_t* INVERT(ECC_State *state, uint32_t *in1)
{
    return invert(state, in1);
}

//*****************************************************************************
// IMPORTMODULUS
//*****************************************************************************
uint32_t IMPORTMODULUS(uint32_t *out, const uint32_t *in1)
{
    return importmodulus(out, in1);
}

//*****************************************************************************
// IMPORTOPERAND
//*****************************************************************************
uint32_t IMPORTOPERAND(ECC_State *state, uint32_t *out, const uint32_t *in1)
{
    return importoperand(state, out, in1);
}

//*****************************************************************************
// EXPORTOPERAND
//*****************************************************************************
uint32_t EXPORTOPERAND(ECC_State *state, uint32_t *out, const uint32_t *in1)
{
    return exportoperand(state, out, in1);
}

//*****************************************************************************
// SETOPERAND
//*****************************************************************************
void SETOPERAND(ECC_State *state, uint32_t *out, uint32_t inValue)
{
    setoperand(state, out, inValue);
}

//*****************************************************************************
// mSET
//*****************************************************************************
void mSET(ECC_State *state, uint32_t *in1, uint32_t size)
{
    mset(state, in1, size);
}

//*****************************************************************************
// mMULT
//*****************************************************************************
void mMULT(uint32_t *out, const uint32_t *in1, const uint32_t *in2, uint32_t *mod, uint32_t len)
{
    mmult(out, in1, in2, mod, len);
}

//*****************************************************************************
// mSUB
//*****************************************************************************
void mSUB(uint32_t *out, const uint32_t *in1, const uint32_t *in2, uint32_t *mod, uint32_t len)
{
    msub(out, in1, in2, mod, len);
}

//*****************************************************************************
// mADD
//*****************************************************************************
void mADD(uint32_t *out, const uint32_t *in1, const uint32_t *in2, uint32_t *mod, uint32_t len)
{
    madd(out, in1, in2, mod, len);
}

//*****************************************************************************
// mOUT
//*****************************************************************************
void mOUT(uint32_t *out, const uint32_t *in1, uint32_t *mod, uint32_t len)
{
    mout(out, in1, mod, len);
}

//*****************************************************************************
// COPY
//*****************************************************************************
void COPY(uint32_t *out, uint32_t *in1, uint32_t size)
{
    copy(out, in1, size);
}

//*****************************************************************************
// GETBIT
//*****************************************************************************
uint8_t GETBIT(uint32_t *in1, uint32_t *in2)
{
    return getbit(in1, in2);
}

//*****************************************************************************
// SCALARMUL_init
//*****************************************************************************
uint8_t SCALARMUL_init(ECC_State *state)
{
    return scalarmul_init(state);
}
