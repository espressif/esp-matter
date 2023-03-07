/******************************************************************************
*  Filename:       rom_sha256.c
*  Revised:        2020-04-03 19:11:17 +0200 (Fri, 03 Apr 2020)
*  Revision:       57290
*
*  Description:    Constant definitions for ECC_NISTP256 curve
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

#include "rom_sha256.h"

#include <stdbool.h>
#include <string.h>

//*****************************************************************************
// SHA256_init
//*****************************************************************************
uint8_t SHA256_init(SHA256_Workzone *workzone)
{
    return HapiSHA256Init(workzone);
}

//*****************************************************************************
// SHA256_process
//*****************************************************************************
uint8_t SHA256_process(SHA256_Workzone *workzone,
                       uint8_t *inputBuffer,
                       uint32_t bufLength)
{
    return HapiSHA256Process(workzone,
                             inputBuffer,
                             bufLength);
}

//*****************************************************************************
// SHA256_final
//*****************************************************************************
uint8_t SHA256_final(SHA256_Workzone *workzone, uint8_t *digest)
{
    return HapiSHA256Final(workzone, digest);
}

//*****************************************************************************
// SHA256_full
//*****************************************************************************
uint8_t SHA256_full(SHA256_Workzone *workzone,
                    uint8_t *digest,
                    uint8_t *inputBuffer,
                    uint32_t bufLength)
{
    return HapiSHA256Full(workzone,
                          digest,
                          inputBuffer,
                          bufLength);
}
