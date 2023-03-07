/**************************************************************************************
* Copyright (c) 2016-2019, Arm Limited (or its affiliates). All rights reserved       *
*                                                                                     *
* This file and the related binary are licensed under the following license:          *
*                                                                                     *
* ARM Object Code and Header Files License, v1.0 Redistribution.                      *
*                                                                                     *
* Redistribution and use of object code, header files, and documentation, without     *
* modification, are permitted provided that the following conditions are met:         *
*                                                                                     *
* 1) Redistributions must reproduce the above copyright notice and the                *
*    following disclaimer in the documentation and/or other materials                 *
*    provided with the distribution.                                                  *
*                                                                                     *
* 2) Unless to the extent explicitly permitted by law, no reverse                     *
*    engineering, decompilation, or disassembly of is permitted.                      *
*                                                                                     *
* 3) Redistribution and use is permitted solely for the purpose of                    *
*    developing or executing applications that are targeted for use                   *
*    on an ARM-based product.                                                         *
*                                                                                     *
* DISCLAIMER. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND                  *
* CONTRIBUTORS "AS IS." ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT             *
* NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, NON-INFRINGEMENT,        *
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE          *
* COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,   *
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED            *
* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR              *
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF              *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING                *
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS                  *
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                        *
**************************************************************************************/

/*!
 @addtogroup cc_rnd
 @{
 */

/*!
 @file
 @brief This file contains the CryptoCell random-number generation (RNG) APIs.

 The random-number generation module implements <em>NIST Special Publication
 800-90A: Recommendation for Random Number Generation Using Deterministic
 Random Bit Generators.</em>
 */


#ifndef _CC_RND_COMMON_H
#define _CC_RND_COMMON_H

#include "cc_error.h"
#include "cc_aes_defs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/************************ Defines ******************************/

/* RND seed and additional input sizes */
/*! The maximal size of the random seed in words. */
#define CC_RND_SEED_MAX_SIZE_WORDS                  12
#ifndef USE_MBEDTLS_CRYPTOCELL
#ifndef CC_RND_ADDITINAL_INPUT_MAX_SIZE_WORDS
/*! The maximal size of the additional input-data in words. */
#define CC_RND_ADDITINAL_INPUT_MAX_SIZE_WORDS   CC_RND_SEED_MAX_SIZE_WORDS
#endif
#endif
/* maximal requested size counter (12 bits active) - maximal count
of generated random 128 bit blocks allowed per one request of
Generate function according NIST 800-90 it is (2^12 - 1) = 0x3FFFF */
/* Max size for one RNG generation (in bits) =
  max_num_of_bits_per_request = 2^19 (FIPS 800-90 Tab.3) */
/*! The maximal size of the generated vector in bits. */
#define CC_RND_MAX_GEN_VECTOR_SIZE_BITS       0x7FFFF
/*! The maximal size of the generated random vector in bytes. */
#define CC_RND_MAX_GEN_VECTOR_SIZE_BYTES    0xFFFF
/*! The maximal size of the generated vector in bytes. */
#define CC_RND_REQUESTED_SIZE_COUNTER  0x3FFFF

/*   Definitions of temp buffer for RND_DMA  */
/*******************************************************************/
/*   Definitions of temp buffer for DMA  */
/*! The size of the temporary buffer in words. */
#define CC_RND_WORK_BUFFER_SIZE_WORDS 136

/*! The definition of the RAM buffer, for internal use in instantiation or
reseeding operations. */
typedef struct
{
    /*! The internal buffer. */
    uint32_t ccRndIntWorkBuff[CC_RND_WORK_BUFFER_SIZE_WORDS];
}CCRndWorkBuff_t;


/* RND source buffer inner (entrpopy) offset       */
/*! The definition of the internal offset in words. */
#define CC_RND_TRNG_SRC_INNER_OFFSET_WORDS    2
/*! The definition of the internal offset in bytes. */
#define CC_RND_TRNG_SRC_INNER_OFFSET_BYTES    (CC_RND_TRNG_SRC_INNER_OFFSET_WORDS*sizeof(uint32_t))


/************************ Enumerators  ****************************/

/*! The definition of the random operation modes. */
typedef  enum
{
    /*! HW entropy estimation: 800-90B or full. */
    CC_RND_FE  = 1,
    /*! Reserved. */
    CC_RND_ModeLast = 0x7FFFFFFF,
} CCRndMode_t;


/************************ Structs  *****************************/


/* The internal state of DRBG mechanism based on AES CTR and CBC-MAC
   algorithms. It is set as global data defined by the following
   structure  */
/*!

  @brief The structure for the RND state.
  This includes internal data that must be saved by the user between boots.
 */
typedef  struct
{
#ifndef USE_MBEDTLS_CRYPTOCELL
    /* Seed buffer, consists from concatenated Key||V: max size 12 words */
    /*! The random-seed buffer. */
    uint32_t  Seed[CC_RND_SEED_MAX_SIZE_WORDS];
    /* Previous value for continuous test */
    /*! The previous random data, used for continuous test. */
    uint32_t  PreviousRandValue[CC_AES_CRYPTO_BLOCK_SIZE_IN_WORDS];
    /* AdditionalInput buffer max size = seed max size words + 4w for padding*/
    /*! The previous additional-input buffer. */
    uint32_t  PreviousAdditionalInput[CC_RND_ADDITINAL_INPUT_MAX_SIZE_WORDS+3];
    /*! The additional-input buffer. */
    uint32_t  AdditionalInput[CC_RND_ADDITINAL_INPUT_MAX_SIZE_WORDS+4];
    /*! The size of the additional input in words. */
    uint32_t  AddInputSizeWords;
    /*! The size of the entropy source in words. */
    uint32_t  EntropySourceSizeWords;
    /*! The Reseed counter (32-bit active). Indicates the number of requests
    for entropy since instantiation or reseeding. */
    uint32_t  ReseedCounter;
    /*! The key size in words, according to security strength: 128 bits:
    4 words. 256 bits: 8 words. */
    uint32_t KeySizeWords;
    /* State flag (see definition of StateFlag above), containing bit-fields, defining:
    - b'0: instantiation steps:   0 - not done, 1 - done;
    - 2b'9,8: working or testing mode: 0 - working, 1 - KAT DRBG test, 2 -
      KAT TRNG test;
    b'16: flag defining is Previous random valid or not:
            0 - not valid, 1 - valid */
    /*! The state flag used internally in the code. */
    uint32_t StateFlag;
    /* validation tag */
    /*! The validation tag used internally in the code. */
    uint32_t ValidTag;
    /*! The size of the RND source entropy in bits. */
    uint32_t  EntropySizeBits;

#endif
    /*! The TRNG process state used internally in the code. */
    uint32_t TrngProcesState;

} CCRndState_t;


/*! The RND vector-generation function pointer. */
typedef int (*CCRndGenerateVectWorkFunc_t)(        \
        /*! A pointer to the RND-state context. */
        void              *rndState_ptr, \
        /*! A pointer to the output buffer. */
        unsigned char     *out_ptr, \
        /*! The size of the output in bytes. */
        size_t            outSizeBytes
        );


/*! The definition of the RND context that includes the CryptoCell
    RND state structure, and a function pointer for the RND-generation
    function. */
typedef  struct
{
        /*! A pointer to the internal state of the RND.
        Note: This pointer should be allocated in a physical and contiguous
        memory, that is accessible to the CryptoCell DMA. This pointer should
        be allocated and assigned before calling CC_LibInit(). */
       void *   rndState;
       /*! A pointer to the entropy context. Note: This pointer should be
       allocated and assigned before calling CC_LibInit(). */
       void *   entropyCtx;
       /*! A pointer to the user-given function for generation a random
       vector. */
       CCRndGenerateVectWorkFunc_t rndGenerateVectFunc;
} CCRndContext_t;





/*****************************************************************************/
/**********************        Public Functions      *************************/
/*****************************************************************************/


/****************************************************************************************/
/*!
 @brief This function sets the RND vector-generation function into the RND
 context.

 It is called as part of Arm CryptoCell library initialization, to
 set the RND vector generation function into the primary RND context.

 @note It must be called before any other API that requires the RND context as
 a parameter.

 @return \c CC_OK on success.
 @return A non-zero value from cc_rnd_error.h on failure.
 */
CCError_t CC_RndSetGenerateVectorFunc(
            /*! [in/out] A pointer to the RND context buffer that is allocated
            by the user, which is used to maintain the RND state, as well as
            pointers to the functions used for random vector generation. */
            CCRndContext_t *rndContext_ptr,
            /*! [in] A pointer to the \c CC_RndGenerateVector random
            vector-generation function. */
            CCRndGenerateVectWorkFunc_t rndGenerateVectFunc
);




#ifdef __cplusplus
}
#endif

/*!
 @}
 */
#endif /* #ifndef _CC_RND_COMMON_H */
