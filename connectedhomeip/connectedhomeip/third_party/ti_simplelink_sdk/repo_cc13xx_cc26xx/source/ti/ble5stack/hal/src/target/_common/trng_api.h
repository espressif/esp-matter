/******************************************************************************

 @file  trng_api.h

 @brief Header for TRNG proxy for stack's interface to the TRNG driver.

 Group: WCS, LPC/BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2015-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#ifndef TRNG_API_H
#define TRNG_API_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */

#include <ti/drivers/TRNG.h>

extern uint32_t *trngDrvTblPtr;

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

// TRNG proxy index for TRNG driver API
#define TRNG_INIT               0
#define TRNG_OPEN               1
#define TRNG_GENERATE_ENTROPY   2
#define TRNG_CLOSE              3

/*
** TRNG API Proxy
*/

#define TRNG_TABLE(index)      (*((uint32_t *)((uint32_t)trngDrvTblPtr + (uint32_t)((index)*4))))

#define TRNG_init              ((void         (*)(void                                      )) TRNG_TABLE(TRNG_INIT))
#define TRNG_open              ((TRNG_Handle  (*)(uint_least8_t index,  TRNG_Params *params )) TRNG_TABLE(TRNG_OPEN))
#define TRNG_generateEntropy   ((int_fast16_t (*)(TRNG_Handle   handle, CryptoKey   *entropy)) TRNG_TABLE(TRNG_GENERATE_ENTROPY))
#define TRNG_close             ((void         (*)(TRNG_Handle   handle                      )) TRNG_TABLE(TRNG_CLOSE))

#ifdef __cplusplus
}
#endif

#endif /* TRNG_API_H */
