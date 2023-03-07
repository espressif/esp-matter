/********************************************************************************************************
 * @file	ecc_ll.h
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

#ifndef ECC_LL_H_
#define ECC_LL_H_

extern const u8 blt_ecc_dbg_priv_key[32];
extern const u8 blt_ecc_dbg_pub_key[64];

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief		This function is used to register the random number function needed for ECC calculation
* @param		none
* @return		none
*/
void 			blt_ecc_init(void);

/**
* @brief		This function is used to generate an ECDH public-private key pairs
* @param[out]  pub[64]:  output ecdh public key
* @param[out]  priv[64]: output ecdh private key
* @param[in]   use_dbg_key: 0: Non-debug key , others: debug key
* @return		1:  success
*              0: failure
*/
int 			blt_ecc_gen_key_pair(unsigned char pub[64], unsigned char priv[32], bool use_dbg_key);

/**
* @brief		This function is used to calculate DHKEY based on the peer public key and own private key
* @param[in]   peer_pub_key[64]: peer public key
* @param[in]   own_priv_key[32]: own private key
* @param[out]  out_dhkey[32]: dhkey key
* @return		1:  success
*              0: failure
*/
int 			blt_ecc_gen_dhkey(const unsigned char peer_pub[64], const unsigned char own_priv[32], unsigned char out_dhkey[32]);



#ifdef __cplusplus
}
#endif

#endif /* ECC_LL_H_ */




