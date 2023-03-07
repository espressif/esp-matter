/********************************************************************************************************
 * @file	static_assert.h
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

// static assertion. evaluate at compile time. It is very useful like,  STATIC_ASSERT(sizeof(a) == 5);

// #define STATIC_ASSERT(expr)   			{ char static_assertion[(expr) ? 1 : -1]; ((void) static_assertion); }	// (void) array;  to remove compiler unused variable warning

// more complicated version canbe used anywhere in the source
#define STATIC_ASSERT_M(COND,MSG) typedef char static_assertion_##MSG[(!!(COND))*2-1] 
// token pasting madness: 
#define STATIC_ASSERT3(X,L) 	STATIC_ASSERT_M(X,static_assertion_at_line_##L) 
#define STATIC_ASSERT2(X,L) 	STATIC_ASSERT3(X,L) 

#define STATIC_ASSERT(X)    	STATIC_ASSERT2(X,__LINE__) 

#define STATIC_ASSERT_POW2(expr)		STATIC_ASSERT(!((expr) & ((expr)-1)))					//  assert  expr  is  2**N
#define STATIC_ASSERT_EVEN(expr)		STATIC_ASSERT(!((expr) & 1))
#define STATIC_ASSERT_ODD(expr)			STATIC_ASSERT(((expr) & 1))
#define STATIC_ASSERT_INT_DIV(a, b) 	STATIC_ASSERT((a) / (b) * (b) == (a))

