/******************************************************************************

 @file  ccm.h

 @brief Describe the purpose and contents of the file.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2006-2022, Texas Instruments Incorporated
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

#ifndef     _CCM_H_
#define     _CCM_H_

uint8_t SSP_CCM_Auth_Sw (uint8_t, uint8_t *, uint8_t *, uint16_t, uint8_t *, uint16_t, uint8_t *, uint8_t *, uint8_t);
uint8_t SSP_CCM_Encrypt_Sw (uint8_t, uint8_t *, uint8_t *, uint16_t, uint8_t *, uint8_t *, uint8_t);
uint8_t SSP_CCM_Decrypt_Sw (uint8_t, uint8_t *, uint8_t *, uint16_t, uint8_t *, uint8_t *, uint8_t);
uint8_t SSP_CCM_InvAuth_Sw (uint8_t, uint8_t *, uint8_t *, uint16_t, uint8_t *, uint16_t, uint8_t *, uint8_t *, uint8_t);

uint8_t SSP_CTR_Decrypt_Sw (uint8_t* pCipherTxt, uint16_t cipherTxtLen, uint8_t *AesKey, uint8_t* Nonce, uint8_t* IV);
uint8_t SSP_CTR_Encrypt_Sw (uint8_t *M, uint16_t len_m, uint8_t *AesKey, uint8_t *Nonce, uint8_t* IV);

uint8_t SSP_CCM_Auth_Encrypt_Sw (uint8_t, uint8_t, uint8_t *, uint8_t *, uint16_t, uint8_t *, uint16_t, uint8_t *, uint8_t *, uint8_t);
uint8_t SSP_CCM_InvAuth_Decrypt_Sw (uint8_t, uint8_t, uint8_t *, uint8_t *, uint16_t, uint8_t *, uint16_t, uint8_t *, uint8_t *, uint8_t);

#endif  // _CCM_H_

