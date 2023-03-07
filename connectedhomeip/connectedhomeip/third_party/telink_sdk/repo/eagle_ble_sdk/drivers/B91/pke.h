/********************************************************************************************************
 * @file	pke.h
 *
 * @brief	This is the header file for B91
 *
 * @author	Driver Group
 * @date	2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#include "reg_include/register_b91.h"




#define GET_WORD_LEN(bitLen)     	((bitLen+31)/32)
#define GET_BYTE_LEN(bitLen)     	((bitLen+7)/8)

#define PKE_BASE             	 	(0X80110000)
#define reg_pke_a_ram(a)		 	((volatile unsigned long *)(PKE_BASE+0x0400+(a)*(0x24)))
#define reg_pke_b_ram(b)		 	((volatile unsigned long *)(PKE_BASE+0x1000+(b)*(0x24)))

/********* oprand length *********/
#define PKE_OPERAND_MAX_WORD_LEN	(0x08)
#define PKE_OPERAND_MAX_BIT_LEN		(0x100)
#define ECC_MAX_WORD_LEN			PKE_OPERAND_MAX_WORD_LEN
#define ECC_MAX_BIT_LEN				PKE_OPERAND_MAX_BIT_LEN

#ifndef NULL
#define NULL  0
#endif

/**
 * eccp curve
 */
typedef struct
{
    unsigned int eccp_p_bitLen;        //bit length of prime p
    unsigned int eccp_n_bitLen;        //bit length of order n
    unsigned int *eccp_p;
    unsigned int *eccp_p_h;
    unsigned int *eccp_p_n1;
    unsigned int *eccp_a;
    unsigned int *eccp_b;
    unsigned int *eccp_Gx;
    unsigned int *eccp_Gy;
    unsigned int *eccp_n;
    unsigned int *eccp_n_h;
    unsigned int *eccp_n_n1;
}eccp_curve_t;

/**
 * mont curve
 */
typedef struct
{
	unsigned int mont_p_bitLen;        //bit length of prime p
	unsigned int *mont_p;
	unsigned int *mont_p_h;
	unsigned int *mont_p_n1;
	unsigned int *mont_a24;
	unsigned int *mont_u;
	unsigned int *mont_v;
	unsigned int *mont_n;
	unsigned int *mont_n_h;
	unsigned int *mont_n_n1;
	unsigned int *mont_h;
}mont_curve_t;

/**
 * edward curve
 */
typedef struct
{
    unsigned int edward_p_bitLen;        //bit length of prime p
    unsigned int *edward_p;
    unsigned int *edward_p_h;
    unsigned int *edward_p_n1;
    unsigned int *edward_d;
    unsigned int *edward_Gx;
    unsigned int *edward_Gy;
    unsigned int *edward_n;
    unsigned int *edward_n_h;
    unsigned int *edward_n_n1;
    unsigned int *edward_h;

}edward_curve_t;

/**
 * pke return code
 */
typedef enum
{
    PKE_SUCCESS = 0,
	PKE_ACTIVE_STOP,
    PKE_MOD_INV_NOT_EXIST,
    PKE_POINT_NOT_ON_CURVE,
    PKE_INVALID_MICRO_CODE,
	PKE_POINTOR_NULL,
	PKE_INVALID_INPUT,
}pke_ret_code_e;

/**
 * pke exe cfg
 */
typedef enum{
	PKE_EXE_CFG_ALL_NON_MONT = 0x15,
	PKE_EXE_CFG_ALL_MONT	 = 0x2A,
}pke_exe_cfg_e;

/**
 * pke micro code
 */
typedef enum{
	PKE_MICROCODE_PDBL = 0x04,
	PKE_MICROCODE_PADD = 0x08,
	PKE_MICROCODE_PVER = 0x0C,
	PKE_MICROCODE_PMUL = 0x10,
	PKE_MICROCODE_MODMUL = 0x18,
	PKE_MICROCODE_MODINV = 0x1C,
	PKE_MICROCODE_MODADD = 0x20,
	PKE_MICROCODE_MODSUB = 0x24,
	PKE_MICROCODE_CAL_PRE_MON = 0x28,
	PKE_MICROCODE_C25519_PMUL = 0x34,
	PKE_MICROCODE_Ed25519_PMUL = 0x38,
	PKE_MICROCODE_Ed25519_PADD = 0x3C,

}pke_microcode_e;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief		This function serves to get pke status.
 * @param[in] 	status	- the interrupt status to be obtained.
 * @return		pke status.
 */
static inline unsigned int pke_get_irq_status(pke_status_e status)
{
    return reg_pke_stat & status;
}

/**
 * @brief		This function serves to clear pke status.
 * @param[in] 	status	- the interrupt status that needs to be cleared.
 * @return		none.
 */
static inline void pke_clr_irq_status(pke_status_e status)
{
	reg_pke_stat = ~status;
}

/**
 * @brief		This function serves to enable pke interrupt function.
 * @param[in] 	mask - the irq mask.
 * @return		none.
 */
static inline void pke_set_irq_mask(pke_conf_e mask)
{
	BM_SET(reg_pke_conf, mask);
}

/**
 * @brief		This function serves to disable PKE interrupt function.
 * @param[in] 	mask - the irq mask.
 * @return		none.
 */
static inline void pke_clr_irq_mask(pke_conf_e mask)
{
	BM_CLR(reg_pke_conf, mask);
}

/**
 * @brief		set operation micro code.
 * @param[in]   addr	- pke micro code.
 * @return		none.
 */
static inline void pke_set_microcode(pke_microcode_e addr)
{
	reg_pke_mc_ptr = addr;
}

/**
 * @brief		set exe config.
 * @param[in]	cfg	- pke exe conf.
 * @return		none.
 */
static inline void pke_set_exe_cfg(pke_exe_cfg_e cfg)
{
	reg_pke_exe_conf = cfg;
}

/**
 * @brief		start pke calculate.
 * @return		none.
 */
static inline void pke_opr_start(void)
{
	BM_SET(reg_pke_ctrl, FLD_PKE_CTRL_START);
}

/**
 * @brief		This is used to indicate the reason when the pke stopped.
 * @return     	0 - normal stop.
 * 				1 - received a termination request(CTRL.STOP is high).
 * 				2 - no valid modulo inverse.
 * 				3 - point is not on the curve(CTRL.CMD:PVER).
 * 				4 - invalid microcode.
 */
static inline unsigned char pke_check_rt_code(void)
{
    return (unsigned char)(reg_pke_rt_code & FLD_PKE_RT_CODE_STOP_LOG);
}

/**
 * @brief		set operand width please make sure 0 < bitLen <= 256.
 * @param[in]	bitLen	- operand width.
 * @return		none.
 */
static inline void pke_set_operand_width(unsigned int bitLen)
{
	BM_CLR(reg_pke_conf, FLD_PKE_CONF_PARTIAL_RADIX);
	BM_SET(reg_pke_conf, GET_WORD_LEN(bitLen)<<16);
	BM_CLR(reg_pke_conf, FLD_PKE_CONF_BASE_RADIX);
	BM_SET(reg_pke_conf, 2<<24);
}

/**
 * @brief		compare big integer a and b.
 * @param[in] 	a 			- value.
 * @param[in] 	aWordLen  	- the length of a.
 * @param[in] 	b			- value.
 * @param[in] 	bWordLen	- the length of b.
 * @return		0:a=b,   1:a>b,   -1: a<b.
 */
signed int big_integer_compare(unsigned int *a, unsigned int aWordLen, unsigned int *b, unsigned int bWordLen);

/**
 * @brief		c = a - b.
 * @param[in]   a 		- integer a.
 * @param[in]   b 		- integer b.
 * @param[in]   wordLen	- the length of a and b.
 * @param[out]  c 		- integer c = a - b.
 * @return		none.
 */
void sub_u32(unsigned int *a, unsigned int *b, unsigned int *c, unsigned int wordLen);

/**
 * @brief		a = a/(2^n).
 * @param[in]   a			- big integer a.
 * @param[in]   aWordLen	- word length of a.
 * @param[in]   n			- exponent of 2^n.
 * @return		word length of a = a/(2^n).
 * @attention:	1. make sure aWordLen is real word length of a.
 *     			2. a may be 0, then aWordLen is 0, to make sure aWordLen-1 is available, so data
 *        		   type of aWordLen is int32_t, not uint32_t.
 */
unsigned int div2n_u32(unsigned int a[], signed int aWordLen, unsigned int n);

/**
 * @brief		load the pre-calculated mont parameters H(R^2 mod modulus) and
 * 				n1( - modulus ^(-1) mod 2^w ).
 * @param[in] 	H 	  	- R^2 mod modulus.
 * @param[in] 	n1 	  	- modulus ^(-1) mod 2^w, here w is 32 acutally.
 * @param[in] 	wordLen - word length of modulus or H.
 * @return: 	none.
 */
void pke_load_pre_calc_mont(unsigned int *H, unsigned int *n1, unsigned int wordLen);

/**
 * @brief		calc h(R^2 mod modulus) and n1( - modulus ^(-1) mod 2^w ) for modmul, pointMul. etc.
 * 				here w is bit width of word, i,e. 32.
 * @param[in]   modulus - input, modulus.
 * @param[in]   wordLen - input, word length of modulus or H.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_calc_pre_mont(const unsigned int *modulus, unsigned int wordLen);

/**
 * @brief       out = a*b mod modulus.
 * @param[in]   modulus	- modulus.
 * @param[in]   a 		- integer a.
 * @param[in]   b 		- integer b.
 * @param[in]   wordLen	- word length of modulus, a, b.
 * @param[out]  out		- out = a*b mod modulus.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_mod_mul(const unsigned int *modulus, const unsigned int *a, const unsigned int *b,
 							unsigned int *out, unsigned int wordLen);

/**
 * @brief       ainv = a^(-1) mod modulus.
 * @param[in]   modulus		- modulus.
 * @param[in]   a 			- integer a.
 * @param[in]   modWordLen	- word length of modulus, ainv.
 * @param[in]   aWordLen 	- word length of integer a.
 * @param[out]	ainv 		- ainv = a^(-1) mod modulus.
 * @return: 	PKE_SUCCESS(success), other(inverse not exists or error).
 */
 unsigned char pke_mod_inv(const unsigned int *modulus, const unsigned int *a, unsigned int *ainv, unsigned int modWordLen,
 				   unsigned int aWordLen);

/**
 * @brief       out = (a+b) mod modulus.
 * @param[in]   modulus - modulus.
 * @param[in]   a 		- integer a.
 * @param[in]   b 		- integer b.
 * @param[in]   wordLen - word length of modulus, a, b.
 * @param[out]  out 	- out = a+b mod modulus.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_mod_add(const unsigned int *modulus, const unsigned int *a, const unsigned int *b,
				   unsigned int *out, unsigned int wordLen);

/**
 * @brief       out = (a-b) mod modulus.
 * @param[in]   modulus	- input, modulus.
 * @param[in]  	a		- input, integer a.
 * @param[in]   b		- input, integer b.
 * @param[in]   wordLen - input, word length of modulus, a, b.
 * @param[out]  out		- output, out = a-b mod modulus.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_mod_sub(const unsigned int *modulus, const unsigned int *a, const unsigned int *b,
 				   unsigned int *out, unsigned int wordLen);

/**
 * @brief		c = a mod b.
 * @param[in]   a 		 	- integer a.
 * @param[in]   b 		 	- integer b.
 * @param[in]   aWordLen	- word length of a.
 * @param[in]   bWordLen	- word length of b.
 * @param[in]   b_h			- parameter b_h.
 * @param[in]   b_n1		- parameter b_n1.
 * @param[out]  c			- c = a mod b.
 * @return		PKE_SUCCESS(success), other(error).
 */
unsigned char pke_mod(unsigned int *a, unsigned int aWordLen, unsigned int *b, unsigned int *b_h, unsigned int *b_n1,
				unsigned int bWordLen, unsigned int *c);

/**
 * @brief       ECCP curve point del point, Q=2P.
 * @param[in]   curve	- ECCP_CURVE struct pointer.
 * @param[in]   Px 		- x coordinate of point P.
 * @param[in]   Py 		- y coordinate of point P.
 * @param[out]  Qx 		- x coordinate of point Q=2P.
 * @param[out]  Qy 		- y coordinate of point Q=2P.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_eccp_point_del(eccp_curve_t *curve, unsigned int *Px, unsigned int *Py,
					  unsigned int *Qx, unsigned int *Qy);

/**
 * @brief       check whether the input point P is on ECCP curve or not.
 * @param[in]   curve	- ECCP_CURVE struct pointer.
 * @param[in]   Px		- x coordinate of point P.
 * @param[in]   Py		- y coordinate of point P.
 * @return      PKE_SUCCESS(success, on the curve), other(error or not on the curve).
 */
unsigned char pke_eccp_point_verify(eccp_curve_t *curve, unsigned int *Px, unsigned int *Py);

/**
 * @brief       ECCP curve point mul(random point), Q=[k]P.
 * @param[in]   curve	- ECCP_CURVE struct pointer.
 * @param[in]   k	 	- scalar.
 * @param[in]   Px 		- x coordinate of point P.
 * @param[in]   Py 		- y coordinate of point P.
 * @param[out]  Qx 		- x coordinate of point Q=[k]P.
 * @param[out]  Qy 		- y coordinate of point Q=[k]P.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_eccp_point_mul(eccp_curve_t *curve, unsigned int *k, unsigned int *Px, unsigned int *Py,
					  unsigned int *Qx, unsigned int *Qy);

/**
 * @brief       ECCP curve point add, Q=P1+P2.
 * @param[in]   curve	- eccp curve struct pointer.
 * @param[in]   P1x 	- x coordinate of point P1.
 * @param[in]   P1y 	- y coordinate of point P1.
 * @param[in]   P2x 	- x coordinate of point P2.
 * @param[in]   P2y 	- y coordinate of point P2.
 * @param[out]  Qx 		- x coordinate of point Q=P1+P2.
 * @param[out]  Qy 		- y coordinate of point Q=P1+P2.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_eccp_point_add(eccp_curve_t *curve, unsigned int *P1x, unsigned int *P1y, unsigned int *P2x, unsigned int *P2y,
					  unsigned int *Qx, unsigned int *Qy);

/**
 * @brief       c25519 point mul(random point), Q=[k]P.
 * @param[in]   curve	- c25519 curve struct pointer.
 * @param[in]  	k		- scalar.
 * @param[in]   Pu		- u coordinate of point P.
 * @param[out]  Qu		- u coordinate of point Q=[k]P.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_x25519_point_mul(mont_curve_t *curve, unsigned int *k, unsigned int *Pu, unsigned int *Qu);

/**
 * @brief       edwards25519 curve point mul(random point), Q=[k]P.
 * @param[in]   curve	- edwards25519 curve struct pointer.
 * @param[in]  	k		- scalar.
 * @param[in]   Px 		- x coordinate of point P.
 * @param[in]   Py 		- y coordinate of point P.
 * @param[out]  Qx 		- x coordinate of point Q=[k]P.
 * @param[out]  Qy 		- y coordinate of point Q=[k]P.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_ed25519_point_mul(edward_curve_t *curve, unsigned int *k, unsigned int *Px, unsigned int *Py,
					  unsigned int *Qx, unsigned int *Qy);

/**
 * @brief       edwards25519 point add, Q=P1+P2.
 * @param[in]   curve	- edwards25519 curve struct pointer.
 * @param[in]   P1x 	- x coordinate of point P1.
 * @param[in]   P1y 	- y coordinate of point P1.
 * @param[in]   P2x 	- x coordinate of point P2.
 * @param[in]   P2y 	- y coordinate of point P2.
 * @param[out]  Qx 		- x coordinate of point Q=P1+P2.
 * @param[out]  Qy 		- y coordinate of point Q=P1+P2.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_ed25519_point_add(edward_curve_t *curve, unsigned int *P1x, unsigned int *P1y, unsigned int *P2x, unsigned int *P2y,
					  unsigned int *Qx, unsigned int *Qy);

#ifdef __cplusplus
}
#endif


