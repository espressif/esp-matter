/********************************************************************************************************
 * @file	assert.h
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

#ifndef USE_ZEPHYR_HAL_HEADERS

#include "config/user_config.h"			//   for  __DEBUG__

///////////////////////////////////////////////////////////////////////////////////
#if (__DEBUG__)

#define assert(expression)  \
  do{if(!(expression)) __assert (expression, __FILE__, __LINE__)}while(0)

#define __assert(expression, file, lineno)  {printf ("%s:%u: assertion failed!\n", file, lineno);}

#else
#define assert(ignore) ((void) 0)
#endif

////////////////////  To do compiler warning  //////////////////
// http://stackoverflow.com/questions/5966594/how-can-i-use-pragma-message-so-that-the-message-points-to-the-filelineno
// http://gcc.gnu.org/ml/gcc-help/2010-10/msg00196.html
// http://stackoverflow.com/questions/3030099/c-c-pragma-in-define-macro

#define _STRINGIFY(x) #x
#define STRINGIFY(x) _STRINGIFY(x)

#ifdef __GNUC__
#define COMPILE_MESSAGE(x) _Pragma (#x)
#endif

#if (__SHOW_TODO__)
#ifdef __GNUC__
#define TODO(x) COMPILE_MESSAGE(message ("--TODO-- " #x))
#else
#define TODO(x) __pragma(message("--TODO-- "_STRINGIFY(x) " ::function: " __FUNCTION__ "@"STRINGIFY(__LINE__)))
#endif
#else
#define TODO(x)
#endif

#if (__SHOW_WARN__)
#ifdef __GNUC__
#define WARN(x) COMPILE_MESSAGE(message ("--WARN-- " #x))
#else
#define WARN(x) __pragma(message("--WARN-- "_STRINGIFY(x) " ::function: " __FUNCTION__ "@"STRINGIFY(__LINE__)))
#endif
#else
#define WARN(x)
#endif

#if (__SHOW_WARN__)
#ifdef __GNUC__
#define NOTE(x) COMPILE_MESSAGE(message ("--NOTE-- " #x))
#else
#define NOTE(x) __pragma(message("--NOTE-- "_STRINGIFY(x) " ::function: " __FUNCTION__ "@"STRINGIFY(__LINE__)))
#endif
#else
#define NOTE(x)
#endif


#endif /* USE_ZEPHYR_HAL_HEADERS */
