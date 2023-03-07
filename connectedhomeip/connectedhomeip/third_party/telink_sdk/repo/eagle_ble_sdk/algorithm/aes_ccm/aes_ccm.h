/********************************************************************************************************
 * @file	aes_ccm.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	2020.06
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/
#pragma once

#include "stack/ble/ble_format.h"

#define AES_BLOCK_SIZE 16

enum
{
    AES_SUCC = 0,
    AES_NO_BUF,
    AES_FAIL,
};

typedef struct
{
    u32 pkt;
    u8 dir;
    u8 iv[8];
} ble_cyrpt_nonce_t;

typedef struct
{
    u32 enc_pno;
    u32 dec_pno;
    u8 sk[16]; // session key
    ble_cyrpt_nonce_t nonce;
    u8 st;
    u8 enable; // 1: slave enable; 2: master enable
    u8 mic_fail;
} ble_crypt_para_t;

struct CCM_FLAGS_TAG
{
    union
    {
        struct
        {
            u8 L : 3;
            u8 M : 3;
            u8 aData : 1;
            u8 reserved : 1;
        } bf;
        u8 val;
    };
};

typedef struct CCM_FLAGS_TAG ccm_flags_t;

typedef struct
{
    union
    {
        u8 A[AES_BLOCK_SIZE];
        u8 B[AES_BLOCK_SIZE];
    } bf;

    u8 tmpResult[AES_BLOCK_SIZE];
    u8 newAstr[AES_BLOCK_SIZE];
} aes_enc_t;

enum
{
    CRYPT_NONCE_TYPE_ACL = 0,
    CRYPT_NONCE_TYPE_CIS = 1,
    CRYPT_NONCE_TYPE_BIS = 2,
};

typedef union
{
    struct
    {
        u8 enEncFlg : 1;   // enable encryption
        u8 noneType : 2;   // ACL, CIS, BIS
        u8 decMicFail : 1; // Decryption status
        u8 role : 1;       // ll_ccm_enc: Master role must use 1, Slave role must use 0;
                           // ll_ccm_dec: Master role must use 0, Slave role must use 1;
        u8 rsvd : 3;       // Rsvd
    };
    u8 cryptBitsInfo;
} cryptBitsInfo_t;

typedef struct
{
    u64 txPayloadCnt;               // Packet counter for Tx
    u64 rxPayloadCnt;               // Packet counter for Rx
    u8 sk[16];                      // Session key
    ble_cyrpt_nonce_t ccmNonce;     // CCM nonce format
    cryptBitsInfo_t cryptBitsInfo;  // To save Ram
    u16 rsvd;                       // For align
    llPhysChnPdu_t * pllPhysChnPdu; // LL physical channel PDU
} leCryptCtrl_t;


/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
    extern "C" {
#endif

/**
 * @brief encrypt data according to aes algorythm
 * @param[in]  key - encryption key
 * @param[in]  plaintext - input data
 * @param[out] encrypted_data - output data
 * @return none
 */
void aes_ll_encryption(u8 * key, u8 * plaintext, u8 * encrypted_data);

/**
 * @brief   	this function is used to initialize the aes_ccm initial value
 * @param[in]   ltk - encryption key, LTK
 * @param[in]   skdm -
 * @param[in]   skds -
 * @param[in]   ivm -
 * @param[in]   ivs -
 * @param[in]   pd - Reference structure ble_crypt_para_t
 * @return  	none
 */
void aes_ll_ccm_encryption_init(u8 * ltk, u8 * skdm, u8 * skds, u8 * ivm, u8 * ivs, ble_crypt_para_t * pd);

/**
 * @brief   	this function is used to encrypt the aes_ccm value
 * @param[in]   pkt - plaint_text
 * @param[in]   master - ll_ccm_enc: Master role must use 1, Slave role must use 0;
                         ll_ccm_dec: Master role must use 0, Slave role must use 1;
 * @param[in]   pd - Reference structure ble_crypt_para_t
 * @return  	none
 */
void aes_ll_ccm_encryption(u8 * pkt, int master, ble_crypt_para_t * pd);

/**
 * @brief   	this function is used to encrypt the aes_ccm value, version2
 * @param[in]   pd - Reference structure leCryptCtrl_t
 * @return  	none
 */
void aes_ll_ccm_encryption_v2(leCryptCtrl_t * pd);

/**
 * @brief   	this function is used to decrypt the aes_ccm value
 * @param[in]   pkt - plaint_text
 * @param[in]   master - ll_ccm_enc: Master role must use 1, Slave role must use 0;
                         ll_ccm_dec: Master role must use 0, Slave role must use 1;
 * @param[in]   pd - Reference structure ble_crypt_para_t
 * @return  	0: decryption succeeded; 1: decryption failed
 */
int aes_ll_ccm_decryption(u8 * pkt, int master, ble_crypt_para_t * pd);

/**
 * @brief   	this function is used to decrypt the aes_ccm value, version2
 * @param[in]   pd - Reference structure leCryptCtrl_t
 * @return  	0: decryption succeeded; 1: decryption failed
 */
int aes_ll_ccm_decryption_v2(leCryptCtrl_t * pd);

#ifdef __cplusplus
}
#endif
