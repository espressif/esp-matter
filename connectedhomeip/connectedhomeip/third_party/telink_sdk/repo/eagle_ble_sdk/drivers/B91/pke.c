/********************************************************************************************************
 * @file	pke.c
 *
 * @brief	This is the source file for B91
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
#include "pke.h"
#include "string.h"

/**
 * @brief       get real bit length of big number a of wordLen words.
 * @param[in]   a			- the buffer a.
 * @param[in]   wordLen		- the length of a.
 * @return      the real bit length of big number a of wordLen words.
 */
unsigned int valid_bits_get(const unsigned int *a, unsigned int wordLen)
{
    unsigned int i = 0;
    unsigned int j = 0;

	if(0 == wordLen)
	{
		return 0;
	}

    for (i = wordLen; i > 0; i--)
    {
        if (a[i - 1])
        {
            break;
        }
    }

	if(0 == i)
	{
		return 0;
	}

    for (j = 32; j > 0; j--)
    {
        if (a[i - 1] & (0x1 << (j - 1)))
        {
            break;
        }
    }

    return ((i - 1) << 5) + j;
}

/**
 * @brief		get real word lenth of big number a of max_words words.
 * @param[in]   a			- the buffer a.
 * @param[in]   max_words	- the length of a.
 * @return		get real word lenth of big number a.
 */
unsigned int valid_words_get(unsigned int *a, unsigned int max_words)
{
    unsigned int i = 0;

    for (i = max_words; i > 0; i--)
    {
        if (a[i - 1])
        {
            return i;
        }
    }

    return 0;
}

/**
 * @brief		compare big integer a and b.
 * @param[in] 	a 			- value.
 * @param[in] 	aWordLen  	- the length of a.
 * @param[in] 	b			- value.
 * @param[in] 	bWordLen	- the length of b.
 * @return		0:a=b,   1:a>b,   -1: a<b.
 */
signed int big_integer_compare(unsigned int *a, unsigned int aWordLen, unsigned int *b, unsigned int bWordLen)
{
	signed int i;

	aWordLen = valid_words_get(a, aWordLen);
	bWordLen = valid_words_get(b, bWordLen);

	if(aWordLen > bWordLen)
		return 1;
	if(aWordLen < bWordLen)
		return -1;

	for(i=(aWordLen-1); i>=0; i--)
	{
		if(a[i] > b[i])
			return 1;
		if(a[i] < b[i])
			return -1;
	}

	return 0;
}

/**
 * @brief		c = a - b.
 * @param[in]   a 		- integer a.
 * @param[in]   b 		- integer b.
 * @param[in]   wordLen	- the length of a and b.
 * @param[out]  c 		- integer c = a - b.
 * @return		none.
 */
void sub_u32(unsigned int *a, unsigned int *b, unsigned int *c, unsigned int wordLen)
{
	unsigned int i, carry, temp;

	carry = 0;
	for(i=0; i<wordLen; i++)
	{
		temp = a[i]-b[i];
		c[i] = temp-carry;
		if(temp > a[i] || c[i] > temp)
		{
			carry = 1;
		}
		else
		{
			carry = 0;
		}
	}
}

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
unsigned int div2n_u32(unsigned int a[], signed int aWordLen, unsigned int n)
{
	signed int i;
	unsigned int j;

	if(!aWordLen)
		return 0;

	if(n < 32)
	{
		for(i=0; i<aWordLen-1; i++)
		{
			a[i] >>= n;
			a[i] |= (a[i+1]<<(32-n));
		}
		a[i] >>= n;

		if(!a[i])
			return i;
		return aWordLen;
	}
	else        //general method
	{
		j = n>>5;
		n &= 31;
		for(i=0; i<aWordLen-(signed int)j-1; i++)
		{
			a[i] = a[i+j]>>n;
			a[i] |= (a[i+j+1]<<(32-n));
		}
		a[i] = a[i+j]>>n;
		memset(a+aWordLen-j, 0, j<<2);

		if(!a[i])
			return i;
		return aWordLen - j;
	}
}

/**
 * @brief       get result operand from specified addr.
 * @param[in]   baseaddr	- the address.
 * @param[in]   data		- the buffer data.
 * @param[in]   wordLen		- the length of data.
 * @return      none.
 */
void pke_read_operand(unsigned int *baseaddr, unsigned int *data, unsigned int wordLen)
{
    unsigned int i;

    if(baseaddr != data)
    {
        for (i = 0; i < wordLen; i++)
        {
        	data[i] = baseaddr[i];
        }
	}
}

/**
 * @brief       load operand to specified addr.
 * @param[in]   data		- the buffer data.
 * @param[in]   wordLen		- the length of data.
 * @param[out]  baseaddr	- the address.
 * @return      none.
 */
void pke_load_operand(unsigned int *baseaddr, unsigned int *data, unsigned int wordLen)
{
    unsigned int i;

    if(baseaddr != data)
    {
        for (i = 0; i < wordLen; i++)
        {
        	baseaddr[i] = data[i];
        }
        for (i = wordLen; i < 9; i++)
        {
        	baseaddr[i] = 0x00000000;
        }
	}
}

/**
 * @brief		This function is to complete the calculation of the corresponding function of the PKE module.
 * @param[in]   addr	- micro code.
 * @param[in]	cfg		- pke exe cfg.
 * @return     	0 - normal stop.
 * 				1 - received a termination request(CTRL.STOP is high).
 * 				2 - no valid modulo inverse.
 * 				3 - point is not on the curve(CTRL.CMD:PVER).
 * 				4 - invalid microcode.
 */
unsigned char pke_opr_cal(pke_microcode_e addr, pke_exe_cfg_e cfg)
{
	pke_set_microcode(addr);

	if(0x00 != cfg)
		pke_set_exe_cfg(cfg);

	pke_clr_irq_status(FLD_PKE_STAT_DONE);

	pke_opr_start();

    while(!pke_get_irq_status(FLD_PKE_STAT_DONE)){}   //0(in progress) 1(done))

	return (pke_check_rt_code());
}

/**
 * @brief		load the pre-calculated mont parameters H(R^2 mod modulus) and
 * 				n1( - modulus ^(-1) mod 2^w ).
 * @param[in] 	H 	  	- R^2 mod modulus.
 * @param[in] 	n1 	  	-  modulus ^(-1) mod 2^w, here w is 32 acutally.
 * @param[in] 	wordLen - word length of modulus or H.
 * @return: 	none.
 */
void pke_load_pre_calc_mont(unsigned int *H, unsigned int *n1, unsigned int wordLen)
{
	pke_set_operand_width(wordLen<<5);

	pke_load_operand((unsigned int *)reg_pke_a_ram(3), H, wordLen);
	pke_load_operand((unsigned int *)reg_pke_b_ram(4), n1, 1);
}

/**
 * @brief       calc h(R^2 mod modulus) and n1( - modulus ^(-1) mod 2^w ) for modmul, pointMul. etc.
 * 				here w is bit width of word, i,e. 32.
 * @param[in]   modulus - input, modulus.
 * @param[in]   wordLen - input, word length of modulus or H.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_calc_pre_mont(const unsigned int *modulus, unsigned int wordLen)
{
	unsigned char ret;

	pke_set_operand_width(wordLen<<5);

    pke_load_operand((unsigned int *)reg_pke_b_ram(3), (unsigned int *)modulus, wordLen);     //B3 modulus

	ret =  pke_opr_cal(PKE_MICROCODE_CAL_PRE_MON, 0x00);

	return ret;
}

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
					  unsigned int *Qx, unsigned int *Qy)
{
	unsigned char ret;
	unsigned int wordLen = (curve->eccp_p_bitLen + 31)>>5;

	pke_set_operand_width(curve->eccp_p_bitLen);

	pke_load_operand((unsigned int *)reg_pke_a_ram(0), Px, wordLen);                         //A0 Px
	pke_load_operand((unsigned int *)reg_pke_a_ram(1), Py, wordLen);                         //A1 Py
	pke_load_operand((unsigned int *)reg_pke_a_ram(5), curve->eccp_a, wordLen);              //A5 a
	pke_load_operand((unsigned int *)reg_pke_b_ram(3), curve->eccp_p, wordLen);              //B3 p

	if((0 != curve->eccp_p_h) && (0 != curve->eccp_p_n1))
	{
		pke_load_operand((unsigned int *)reg_pke_a_ram(3), curve->eccp_p_h, wordLen);        //A3 p_h
		pke_load_operand((unsigned int *)reg_pke_b_ram(4), curve->eccp_p_n1, 1);             //B4 p_n1
	}
	else
	{
		pke_opr_cal(PKE_MICROCODE_CAL_PRE_MON, 0x00);
	}


	ret = pke_opr_cal(PKE_MICROCODE_PDBL, PKE_EXE_CFG_ALL_NON_MONT);
	if(ret)
	{
		return ret;
	}

	pke_read_operand((unsigned int *)reg_pke_a_ram(0), Qx, wordLen);
	if(Qy != 0)
	{
		pke_read_operand((unsigned int *)reg_pke_a_ram(1), Qy, wordLen);
	}

	return ret;
}

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
					  unsigned int *Qx, unsigned int *Qy)
{
	unsigned char ret;
	unsigned int wordLen = (curve->eccp_p_bitLen + 31)>>5;

	pke_set_operand_width(curve->eccp_p_bitLen);

	pke_load_operand((unsigned int *)reg_pke_b_ram(0), P1x, wordLen);                        //B0 P1x
	pke_load_operand((unsigned int *)reg_pke_b_ram(1), P1y, wordLen);                        //B1 P1y
	pke_load_operand((unsigned int *)reg_pke_a_ram(0), P2x, wordLen);                        //A0 P2x
	pke_load_operand((unsigned int *)reg_pke_a_ram(1), P2y, wordLen);                        //A1 P2y
	pke_load_operand((unsigned int *)reg_pke_b_ram(3), curve->eccp_p, wordLen);              //B3 p

	if((0 != curve->eccp_p_h) && (0 != curve->eccp_p_n1))
	{
		pke_load_operand((unsigned int *)reg_pke_a_ram(3), curve->eccp_p_h, wordLen);        //A3 p_h
		pke_load_operand((unsigned int *)reg_pke_b_ram(4), curve->eccp_p_n1, 1);             //B4 p_n1
	}
	else
	{
		pke_opr_cal(PKE_MICROCODE_CAL_PRE_MON, 0x00);
	}

	ret = pke_opr_cal(PKE_MICROCODE_PADD, PKE_EXE_CFG_ALL_NON_MONT);
	if(ret)
	{
		return ret;
	}

	pke_read_operand((unsigned int *)reg_pke_a_ram(0), Qx, wordLen);
	pke_read_operand((unsigned int *)reg_pke_a_ram(1), Qy, wordLen);

	return ret;
}

/**
 * @brief       check whether the input point P is on ECCP curve or not.
 * @param[in]   curve	- ECCP_CURVE struct pointer.
 * @param[in]   Px		- x coordinate of point P.
 * @param[in]   Py		- y coordinate of point P.
 * @return      PKE_SUCCESS(success, on the curve), other(error or not on the curve).
 */
unsigned char pke_eccp_point_verify(eccp_curve_t *curve, unsigned int *Px, unsigned int *Py)
{
	signed int ret;
	unsigned int wordLen = (curve->eccp_p_bitLen + 31)>>5;

	pke_set_operand_width(curve->eccp_p_bitLen);

	pke_load_operand((unsigned int *)reg_pke_b_ram(0), Px, wordLen);                         //B0 Px
	pke_load_operand((unsigned int *)reg_pke_b_ram(1), Py, wordLen);                         //B1 Py
	pke_load_operand((unsigned int *)reg_pke_a_ram(5), curve->eccp_a, wordLen);              //A5 a
	pke_load_operand((unsigned int *)reg_pke_a_ram(4), curve->eccp_b, wordLen);              //A4 b
	pke_load_operand((unsigned int *)reg_pke_b_ram(3), curve->eccp_p, wordLen);              //B3 p

	if((0 != curve->eccp_p_h) && (0 != curve->eccp_p_n1))
	{
		pke_load_operand((unsigned int *)reg_pke_a_ram(3), curve->eccp_p_h, wordLen);        //A3 p_h
		pke_load_operand((unsigned int *)reg_pke_b_ram(4), curve->eccp_p_n1, 1);             //B4 p_n1
	}
	else
	{
		pke_opr_cal(PKE_MICROCODE_CAL_PRE_MON, 0x00);
	}

	ret = pke_opr_cal(PKE_MICROCODE_PVER, PKE_EXE_CFG_ALL_NON_MONT);
	if(ret)
	{
		return ret;
	}

	return PKE_SUCCESS;
}

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
					  unsigned int *Qx, unsigned int *Qy)
{
	unsigned char ret;
	unsigned int wordLen = (curve->eccp_p_bitLen + 31)>>5;

	pke_set_operand_width(curve->eccp_p_bitLen);

	pke_load_operand((unsigned int *)reg_pke_b_ram(0), Px, wordLen);                         //B0 Px
	pke_load_operand((unsigned int *)reg_pke_b_ram(1), Py, wordLen);                         //B1 Py
	pke_load_operand((unsigned int *)reg_pke_a_ram(5), curve->eccp_a, wordLen);              //A5 a
	pke_load_operand((unsigned int *)reg_pke_a_ram(4), k, wordLen);                          //A4 k
	pke_load_operand((unsigned int *)reg_pke_b_ram(3), curve->eccp_p, wordLen);              //B3 p

	if((0 != curve->eccp_p_h) && (0 != curve->eccp_p_n1))
	{
		pke_load_operand((unsigned int *)reg_pke_a_ram(3), curve->eccp_p_h, wordLen);        //A3 p_h
		pke_load_operand((unsigned int *)reg_pke_b_ram(4), curve->eccp_p_n1, 1);             //B4 p_n1
	}
	else
	{
		pke_opr_cal(PKE_MICROCODE_CAL_PRE_MON, 0x00);
	}


	ret = pke_opr_cal(PKE_MICROCODE_PMUL, PKE_EXE_CFG_ALL_NON_MONT);
	if(ret)
	{
		return ret;
	}

	pke_read_operand((unsigned int *)reg_pke_a_ram(0), Qx, wordLen);
	if(Qy != 0)
	{
		pke_read_operand((unsigned int *)reg_pke_a_ram(1), Qy, wordLen);
	}

	return ret;
}

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
 							unsigned int *out, unsigned int wordLen)
 {
 	unsigned char ret;

 	pke_set_operand_width(wordLen<<5);

 	pke_load_operand((unsigned int *)(reg_pke_b_ram(3)), (unsigned int *)modulus, wordLen);		//B3 modulus
 	pke_load_operand((unsigned int *)(reg_pke_a_ram(0)), (unsigned int *)a, wordLen);			//A0 a
 	pke_load_operand((unsigned int *)(reg_pke_b_ram(0)), (unsigned int *)b, wordLen);			//B0 b

 	ret = pke_opr_cal(PKE_MICROCODE_MODMUL, PKE_EXE_CFG_ALL_NON_MONT);
 	if(ret)
 	{
 		return ret;
 	}

	pke_read_operand((unsigned int *)(reg_pke_a_ram(0)), out, wordLen);                      	//A0 result

	return PKE_SUCCESS;
}

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
 				   unsigned int aWordLen)
 {
 	unsigned char ret;

 	pke_set_operand_width(modWordLen<<5);


 	pke_load_operand((unsigned int *)(reg_pke_b_ram(3)), (unsigned int *)modulus, modWordLen);	//B3 modulus
 	pke_load_operand((unsigned int *)(reg_pke_b_ram(0)), (unsigned int *)a, aWordLen);			//B0 a

 	ret = pke_opr_cal(PKE_MICROCODE_MODINV, 0x00);
 	if(ret)
 	{
 		return ret;
	}

	pke_read_operand((unsigned int *)(reg_pke_a_ram(0)), (unsigned int *)ainv, modWordLen);    	//A0 ainv

	return PKE_SUCCESS;
}

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
				   unsigned int *out, unsigned int wordLen)
{
	unsigned char ret;

	pke_set_operand_width(wordLen<<5);

	pke_load_operand((unsigned int *)(reg_pke_b_ram(3)), (unsigned int *)modulus, wordLen);     //B3 modulus
	pke_load_operand((unsigned int *)(reg_pke_a_ram(0)), (unsigned int *)a, wordLen);           //A0 a
	pke_load_operand((unsigned int *)(reg_pke_b_ram(0)), (unsigned int *)b, wordLen);           //B0 b

	ret =  pke_opr_cal(PKE_MICROCODE_MODADD, 0x00);
	if(ret)
	{
		return ret;
	}

	pke_read_operand((unsigned int *)(reg_pke_a_ram(0)), out, wordLen);                     //A0 result

	return PKE_SUCCESS;
}

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
 				   unsigned int *out, unsigned int wordLen)
{
 	unsigned char ret;

 	pke_set_operand_width(wordLen<<5);

 	pke_load_operand((unsigned int *)(reg_pke_b_ram(3)), (unsigned int *)modulus, wordLen);      //B3 modulus
 	pke_load_operand((unsigned int *)(reg_pke_a_ram(0)), (unsigned int *)a, wordLen);            //A0 a
 	pke_load_operand((unsigned int *)(reg_pke_b_ram(0)), (unsigned int *)b, wordLen);            //B0 b

 	ret =  pke_opr_cal(PKE_MICROCODE_MODSUB, 0x00);
 	if(ret)
 	{
 		return ret;
 	}

	pke_read_operand((unsigned int *)(reg_pke_a_ram(0)), out, wordLen);                     	//A0 result

	return PKE_SUCCESS;
}

/**
 * @brief       c25519 point mul(random point), Q=[k]P.
 * @param[in]   curve	- c25519 curve struct pointer.
 * @param[in]  	k		- scalar.
 * @param[in]   Pu		- u coordinate of point P.
 * @param[out]  Qu		- u coordinate of point Q=[k]P.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_x25519_point_mul(mont_curve_t *curve, unsigned int *k, unsigned int *Pu, unsigned int *Qu)
{
	unsigned char ret;
	unsigned int wordLen = (curve->mont_p_bitLen + 31)>>5;

	pke_set_operand_width(curve->mont_p_bitLen);

	pke_load_operand((unsigned int *)reg_pke_a_ram(0), Pu, wordLen);                         //A0 Pu
	pke_load_operand((unsigned int *)reg_pke_b_ram(0), curve->mont_a24, wordLen);            //B0 a24
	pke_load_operand((unsigned int *)reg_pke_a_ram(4), k, wordLen);                          //A4 k
	pke_load_operand((unsigned int *)reg_pke_b_ram(3), curve->mont_p, wordLen);              //B3 p

	if((NULL != curve->mont_p_h) && (NULL != curve->mont_p_n1))
	{
		pke_load_operand((unsigned int *)reg_pke_a_ram(3), curve->mont_p_h, wordLen);        //A3 p_h
		pke_load_operand((unsigned int *)reg_pke_b_ram(4), curve->mont_p_n1, 1);             //B4 p_n1
	}
	else
	{
		pke_opr_cal(PKE_MICROCODE_CAL_PRE_MON, 0x00);
	}


	ret = pke_opr_cal(PKE_MICROCODE_C25519_PMUL, PKE_EXE_CFG_ALL_NON_MONT);
	if(ret)
	{
		return ret;
	}

	pke_read_operand((unsigned int *)reg_pke_a_ram(1), Qu, wordLen);

	return ret;
}

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
					  unsigned int *Qx, unsigned int *Qy)
{
	unsigned char ret;
	unsigned int wordLen = (curve->edward_p_bitLen + 31)>>5;

	pke_set_operand_width(curve->edward_p_bitLen);

	pke_load_operand((unsigned int *)reg_pke_a_ram(1), Px, wordLen);                        //A1 Px
	pke_load_operand((unsigned int *)reg_pke_a_ram(2), Py, wordLen);                        //A2 Py
	pke_load_operand((unsigned int *)reg_pke_b_ram(0), curve->edward_d, wordLen);           //B0 d
	pke_load_operand((unsigned int *)reg_pke_a_ram(0), k, wordLen);                         //A0 k
	pke_load_operand((unsigned int *)reg_pke_b_ram(3), curve->edward_p, wordLen);           //B3 p

	if((0 != curve->edward_p_h) && (0 != curve->edward_p_n1))
	{
		pke_load_operand((unsigned int *)reg_pke_a_ram(3), curve->edward_p_h, wordLen);      //A3 p_h
		pke_load_operand((unsigned int *)reg_pke_b_ram(4), curve->edward_p_n1, 1);           //B4 p_n1
	}
	else
	{
		pke_opr_cal(PKE_MICROCODE_CAL_PRE_MON, 0x00);
	}


	ret = pke_opr_cal(PKE_MICROCODE_Ed25519_PMUL, PKE_EXE_CFG_ALL_NON_MONT);
	if(ret)
	{
		return ret;
	}

	pke_read_operand((unsigned int *)reg_pke_a_ram(1), Qx, wordLen);
	if(Qy != 0)
	{
		pke_read_operand((unsigned int *)reg_pke_a_ram(2), Qy, wordLen);
	}

	return ret;
}

/**
 * @brief       edwards25519 point add, Q=P1+P2.
 * @param[in]   curve	- edwards25519 curve struct pointer.
 * @param[in]   P1x 	- x coordinate of point P1.
 * @param[in]   P1y 	- y coordinate of point P1.
 * @param[in]   P2x 	- x coordinate of point P2.
 * @param[in]   P2y 	- y coordinate of point P2.
 * @param[out]  Qx 		- x coordinate of point Qx=P1x+P2x.
 * @param[out]  Qy 		- y coordinate of point Qy=P1y+P2y.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_ed25519_point_add(edward_curve_t *curve, unsigned int *P1x, unsigned int *P1y, unsigned int *P2x, unsigned int *P2y,
					  unsigned int *Qx, unsigned int *Qy)
{
	unsigned char ret;
	unsigned int wordLen = (curve->edward_p_bitLen + 31)>>5;

	pke_set_operand_width(curve->edward_p_bitLen);

	pke_load_operand((unsigned int *)reg_pke_a_ram(1), P1x, wordLen);                       //A1 P1x
	pke_load_operand((unsigned int *)reg_pke_a_ram(2), P1y, wordLen);                       //A2 P1y
	pke_load_operand((unsigned int *)reg_pke_b_ram(1), P2x, wordLen);                       //B1 P2x
	pke_load_operand((unsigned int *)reg_pke_b_ram(2), P2y, wordLen);                       //B2 P2y
	pke_load_operand((unsigned int *)reg_pke_b_ram(0), curve->edward_d, wordLen);           //B0 d
	pke_load_operand((unsigned int *)reg_pke_b_ram(3), curve->edward_p, wordLen);           //B3 p

	if((0 != curve->edward_p_h) && (0 != curve->edward_p_n1))
	{
		pke_load_operand((unsigned int *)reg_pke_a_ram(3), curve->edward_p_h, wordLen);      //A3 p_h
		pke_load_operand((unsigned int *)reg_pke_b_ram(4), curve->edward_p_n1, 1);           //B4 p_n1
	}
	else
	{
		pke_opr_cal(PKE_MICROCODE_CAL_PRE_MON, 0x00);
	}

	ret = pke_opr_cal(PKE_MICROCODE_Ed25519_PADD, PKE_EXE_CFG_ALL_NON_MONT);
	if(ret)
	{
		return ret;
	}

	pke_read_operand((unsigned int *)reg_pke_a_ram(1), Qx, wordLen);
	pke_read_operand((unsigned int *)reg_pke_a_ram(2), Qy, wordLen);

	return ret;
}

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
				unsigned int bWordLen, unsigned int *c)
{
	signed int ret;
	unsigned int bitLen, tmpLen;
	unsigned int *a_high, *a_low, *p;

	ret = big_integer_compare(a, aWordLen, b, bWordLen);
	if(ret < 0)
	{
		aWordLen = valid_words_get(a, aWordLen);
		memcpy(c, a, aWordLen<<2);
		memset(c+aWordLen, 0, (bWordLen-aWordLen)<<2);

		return PKE_SUCCESS;
	}
	else if(0 == ret)
	{
		memset(c, 0, bWordLen<<2);

		return PKE_SUCCESS;
	}

	bitLen = valid_bits_get(b, bWordLen) & 0x1F;
	pke_set_operand_width(bWordLen<<5);
	p = (unsigned int *)reg_pke_a_ram(1);

	//get a_high mod b
	a_high = c;
	if(bitLen)
	{
		tmpLen = aWordLen-bWordLen+1;
		memcpy(p, a+bWordLen-1, tmpLen<<2);
		div2n_u32(p, tmpLen, bitLen);
		if(tmpLen < bWordLen)
		{
			memset(p+tmpLen, 0, (bWordLen-tmpLen)<<2);
		}

		if(big_integer_compare(p, bWordLen, b, bWordLen) >= 0)
		{
			sub_u32(p, b, a_high, bWordLen);
		}
		else
		{
			memcpy(a_high, p, bWordLen<<2);
		}
	}
	else
	{
		tmpLen = aWordLen - bWordLen;
		if(big_integer_compare(a+bWordLen, tmpLen, b, bWordLen) > 0)
		{
			sub_u32(a+bWordLen, b, a_high, bWordLen);
		}
		else
		{
			memcpy(a_high, a+bWordLen, tmpLen<<2);
			memset(a_high+tmpLen, 0, (bWordLen-tmpLen)<<2);
		}
	}

	if(0 == b_h || 0 == b_n1)
	{
		ret = pke_calc_pre_mont(b, bWordLen);
		if(PKE_SUCCESS != ret)
		{
			return ret;
		}
	}
	else
	{
		pke_load_pre_calc_mont(b_h, b_n1, bWordLen);
	}

	//get 1000...000 mod b
	memset(p, 0, bWordLen<<2);
	if(bitLen)
	{
		p[bWordLen-1] = 1<<(bitLen);
	}
	sub_u32(p, b, (unsigned int *)reg_pke_b_ram(1), bWordLen);


	//get a_high * 1000..000 mod b
	pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);
	pke_calc_pre_mont(b, bWordLen);
	ret = pke_mod_mul(b, (unsigned int *)reg_pke_b_ram(1), a_high, (unsigned int *)reg_pke_b_ram(1), bWordLen);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	//get a_low mod b
	if(bitLen)
	{
		a_low = c;
		memcpy(p, a, bWordLen<<2);
		p[bWordLen-1] &= ((1<<(bitLen))-1);
		if(big_integer_compare(p, bWordLen, b, bWordLen) >= 0)
		{
			sub_u32(p, b, a_low, bWordLen);
		}
		else
		{
			memcpy(a_low, p, bWordLen<<2);
		}
	}
	else
	{
		if(big_integer_compare(a, bWordLen, b, bWordLen) >= 0)
		{
			a_low = c;
			sub_u32(a, b, a_low, bWordLen);
		}
		else
		{
			a_low = a;
		}
	}

	return pke_mod_add(b, a_low, (unsigned int *)reg_pke_b_ram(1), c, bWordLen);
}





