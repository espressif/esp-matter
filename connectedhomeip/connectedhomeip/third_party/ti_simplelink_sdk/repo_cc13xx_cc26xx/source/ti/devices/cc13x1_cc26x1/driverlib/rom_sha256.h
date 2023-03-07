/******************************************************************************
*  Filename:       rom_sha256.h
*  Revised:        2016-09-19 10:36:17 +0200 (Mon, 19 Sep 2016)
*  Revision:       47179
*
*  Description:    Defines and prototypes for the NIST-P256 curve constants to
*                  be used with the ECC SW implementation.
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

#ifndef __ROM_SHA256_H__
#define __ROM_SHA256_H__

#include <stdint.h>
#include "rom.h"


/******************************************************************************/
/***                              DEFINES                                   ***/
/******************************************************************************/

/*! \brief Length in bytes of NISTP256 curve parameters excluding the prepended
 *  length word.
 */
#define ECC_NISTP256_PARAM_LENGTH_BYTES 32

/*! \brief Number of bytes for the length word prepended before all parameters
 *  passed into the ECC functions.
 */
#define ECC_LENGTH_OFFSET_BYTES 4

/*! \brief Length in bytes of NISTP256 curve parameters including the prepended
 *  length word.
 */
#define ECC_NISTP256_PARAM_LENGTH_WITH_OFFSET_BYTES (ECC_NISTP256_PARAM_LENGTH_BYTES + ECC_LENGTH_OFFSET_BYTES)


/******************************************************************************/
/***                               STATUS                                   ***/
/******************************************************************************/

#define SHA256_SUCCESS 0x77
#define SHA256_ERROR 0x88


//*****************************************************************************
/*!
 * \brief Initialize a SHA-256 operation
 *
 * \param workzone Working memory that holds intermediate results
 *
 * \return Status
 */
//*****************************************************************************
extern uint8_t SHA256_init(SHA256_Workzone *workzone);

//*****************************************************************************
/*!
 * \brief Add a message segment to the hash
 *
 * \param [in,out] workzone     Working memory that holds intermediate results
 * \param [in]     inputBuffer  Segment of an input message to process
 * \param [in]     bufLength    Number of bytes to process
 *
 * \return Status
 */
//*****************************************************************************
extern uint8_t SHA256_process(SHA256_Workzone *workzone,
                              uint8_t *inputBuffer,
                              uint32_t bufLength);

//*****************************************************************************
/*!
 * \brief Finalize the hash and write back the digest
 *
 * \param [in,out] workzone     Working memory that holds intermediate results
 * \param [out]    digest       Resultant digest of the hash
 *
 * \return Status
 */
//*****************************************************************************
extern uint8_t SHA256_final(SHA256_Workzone *workzone, uint8_t *digest);

//*****************************************************************************
/*!
 * \brief Compute the hash of a message in one go
 *
 * \param [in,out] workzone     Working memory that holds intermediate results
 * \param [out]    digest       Resultant digest of the hash
 * \param [in]     inputBuffer  Segment of an input message to process
 * \param [in]     bufLength    Number of bytes to process
 *
 * \return Status
 */
//*****************************************************************************
extern uint8_t SHA256_full(SHA256_Workzone *workzone,
                           uint8_t *digest,
                           uint8_t *inputBuffer,
                           uint32_t bufLength);

#endif // __ROM_SHA256_H__
