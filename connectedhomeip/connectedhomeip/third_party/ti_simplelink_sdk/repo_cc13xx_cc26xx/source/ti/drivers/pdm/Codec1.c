/*
 * Copyright (c) 2014-2016 Texas Instruments Incorporated
 *
 * All rights reserved not granted herein.
 *
 * Limited License.
 *
 * Texas Instruments Incorporated grants a world-wide, royalty-free,
 * non-exclusive license under copyrights and patents it now or hereafter
 * owns or controls to make, have made, use, import, offer to sell and sell
 * ("Utilize") this software subject to the terms herein. With respect to the
 * foregoing patent license, such license is granted solely to the extent that
 * any such patent is necessary to Utilize the software alone. The patent
 * license shall not apply to any combinations which include this software,
 * other than combinations with devices manufactured by or for TI
 * ("TI Devices"). No hardware patent is licensed hereunder.
 *
 * Redistributions must preserve existing copyright notices and reproduce
 * this license (including the above copyright notice and the disclaimer and
 * (if applicable) source code license limitations below) in the documentation
 * and/or other materials provided with the distribution
 *
 * Redistribution and use in binary form, without modification, are permitted
 * provided that the following conditions are met:
 *
 * * No reverse engineering, decompilation, or disassembly of this software is
 *   permitted with respect to any software provided in binary form.
 *
 * * any redistribution and use are licensed by TI for use only with TI Devices.
 *
 * * Nothing shall obligate TI to provide you with source code for the software
 *   licensed and provided to you in object code.
 *
 * If software source code is provided to you, modification and redistribution
 * of the source code are permitted provided that the following conditions are
 * met:
 *
 * * any redistribution and use of the source code, including any resulting
 *   derivative works, are licensed by TI for use only with TI Devices.
 *
 * * any redistribution and use of any object code compiled from the source
 *   code and any resulting derivative works, are licensed by TI for use only
 *   with TI Devices.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of its
 * suppliers may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * DISCLAIMER.
 *
 * THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
 * NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/***********************************************************************************
* INCLUDES
*/
#include <stdint.h>
#include <ti/drivers/pdm/Codec1.h>

/**************************************************************************************************
*                                        Global Variables
*/

static int16_t Codec1_pvEnc;
static int16_t Codec1_pvDec;
static int8_t Codec1_siEnc;
static int8_t Codec1_siDec;

static const uint16_t Codec1_stepsizeLut[89] =
{
    7,    8,    9,   10,   11,   12,   13,   14, 16,   17,   19,   21,   23,   25,   28,   31,
    34,   37,   41,   45,   50,   55,   60,   66, 73,   80,   88,   97,  107,  118,  130,  143,
    157,  173,  190,  209,  230,  253,  279,  307, 337,  371,  408,  449,  494,  544,  598,  658,
    724,  796,  876,  963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024,
    3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493,10442,11487,12635,13899,
    15289,16818,18500,20350,22385,24623,27086,29794, 32767
};

static const int8_t Codec1_IndexLut[16] =
{
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8
};

/*
*  ======== Codec1_encodeSingle ========
*/
uint8_t Codec1_encodeSingle(int16_t audSample)
{
    // Difference between samples and previous predicted value.
    // This difference will be encoded
    int16_t difference = audSample - Codec1_pvEnc;
    //Final nibble value
    uint8_t tic1_nibble_4bits = 0;
    //Step size to quantisize the difference,
    int16_t step = Codec1_stepsizeLut[Codec1_siEnc];
    //cumulated difference from samples.
    int16_t cum_diff = step>>3;

    if(difference<0)
    {
        tic1_nibble_4bits = 8;
        difference = -difference;
    }

    if(difference>=step)
    {
        tic1_nibble_4bits |= 4;
        difference -= step;
        cum_diff += step;
    }
    step >>= 1;
    if(difference>=step)
    {
        tic1_nibble_4bits |= 2;
        difference -= step;
        cum_diff += step;
    }
    step >>= 1;
    if(difference>=step)
    {
        tic1_nibble_4bits |= 1;
        cum_diff += step;
    }

    if(tic1_nibble_4bits&8)
    {
        if (Codec1_pvEnc < (-32768+cum_diff))
            (Codec1_pvEnc) = -32768;
        else
            Codec1_pvEnc -= cum_diff;
    }
    else
    {
        if (Codec1_pvEnc > (0x7fff-cum_diff))
            (Codec1_pvEnc) = 0x7fff;
        else
            Codec1_pvEnc += cum_diff;
    }

    Codec1_siEnc += Codec1_IndexLut[tic1_nibble_4bits];

    if(Codec1_siEnc<0)
        Codec1_siEnc = 0;
    else if(Codec1_siEnc>88)
        Codec1_siEnc = 88;

    return tic1_nibble_4bits;
}


/*
*  ======== Codec1_decodeSingle ========
*/
int16_t Codec1_decodeSingle(uint8_t nibble_4bits)
{
    int16_t step = Codec1_stepsizeLut[Codec1_siDec];
    int16_t cum_diff  = step>>3;

    Codec1_siDec += Codec1_IndexLut[nibble_4bits];
    if(Codec1_siDec<0)
        Codec1_siDec = 0;
    else if(Codec1_siDec>88)
        Codec1_siDec = 88;

    if(nibble_4bits&4)
        cum_diff += step;
    if(nibble_4bits&2)
        cum_diff += step>>1;
    if(nibble_4bits&1)
        cum_diff += step>>2;

    if(nibble_4bits&8)
    {
        if (Codec1_pvDec < (-32767+cum_diff))
            (Codec1_pvDec) = -32767;
        else
            Codec1_pvDec -= cum_diff;
    }
    else
    {
        if (Codec1_pvDec > (0x7fff-cum_diff))
            (Codec1_pvDec) = 0x7fff;
        else
            Codec1_pvDec += cum_diff;
    }
    return Codec1_pvDec;
}


/*
*  ======== Codec1_encodeBuff ========
*/
uint8_t Codec1_encodeBuff(uint8_t* dst, int16_t* src, int16_t srcSize, int8_t *si, int16_t *pv)
{
    Codec1_pvEnc = *pv;
    Codec1_siEnc = *si;
    int16_t* end = (src + srcSize);
    int8_t len = 0;

    while(src < end)
    {
        // encode a pcm value from input buffer
        uint8_t nibble = Codec1_encodeSingle( *src++ );

        nibble |= (Codec1_encodeSingle( *src++ ) << 4);

        *dst++ = nibble;

        len++;
    }

    *pv = Codec1_pvEnc;
    *si = Codec1_siEnc;
    return len;
}


/*
*  ======== Codec1_decodeBuff ========
*/
void Codec1_decodeBuff(int16_t* dst, uint8_t* src, unsigned srcSize,  int8_t *si, int16_t *pv)
{
    Codec1_pvDec = *pv;
    Codec1_siDec = *si;

    // calculate pointers to iterate output buffer
    int16_t* out = dst;
    int16_t* end = out+(srcSize>>1);

    while(out<end)
    {
        // get byte from src
        uint8_t nibble = *src;
        *out++ = Codec1_decodeSingle((nibble&0xF));  // decode value and store it
        nibble >>= 4;  // use high nibble of byte
        nibble &= 0xF;  // use high nibble of byte
        *out++ = Codec1_decodeSingle(nibble);  // decode value and store it
        ++src;        // move on a byte for next sample
    }

    *pv = Codec1_pvDec;
    *si = Codec1_siDec;
}
