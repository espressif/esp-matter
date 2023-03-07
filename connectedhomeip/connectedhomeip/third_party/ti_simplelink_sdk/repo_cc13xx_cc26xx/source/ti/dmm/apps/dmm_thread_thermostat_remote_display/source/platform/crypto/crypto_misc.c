/******************************************************************************

 @file crpto_misc.c 

 @brief mbedTLS debug function patch

 Group: CMCU, LPC
 Target Device: cc13x2_26x2

 ******************************************************************************
 
 Copyright (c) 2017-2020, Texas Instruments Incorporated
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
 
/**
 * when disabling MBEDTLS_SSL_DEBUG_ALL and MBEDTLS_DEBUG_C in
 * openthread/etc/ti/CC26X2R1_LAUNCHXL/ccs/config/mbedtls_config-cc1352 or
 * openthread/etc/ti/CC26X2R1_LAUNCHXL/ccs/config/mbedtls_config-cc2652
 * during linkage, we saw the error
 * `<Linking>`
 
 * undefined                   first referenced
 * symbol                         in file     
 * ---------                   ----------------
 * mbedtls_debug_set_threshold <whole-program> 
 
 * The root cause is that 
 * 1. mbedtls_debug_set_threshold is called in dlts.cpp (openthead)
 * 2. when MBEDTLS_SSL_DEBUG_ALL and MBEDTLS_DEBUG_C are disabled, this function
 *   is not built in mbedTLS lib
 * 3. during the link, you will see this error.
 
 * In order to fix this issue.
 * 1. when MBEDTLS_SSL_DEBUG_ALL and MBEDTLS_DEBUG_C are disabled, don't call this
 *   function. This means we need to modify the openthread code.
 * 2. when MBEDTLS_SSL_DEBUG_ALL and MBEDTLS_DEBUG_C are disabled, in mbedTLS we build
 *   dummy mbedtls_debug_set_threshold. This means we need to modify the mbedTLS
 *   code.
 * 3. in application code, we provide the dummy mbedtls_debug_set_threshold.
 
 * We prefer the option 3.
 
 */ 
//#define MBEDTLS_CONFIG_FILE "mbedtls-config-cc13x2_26x2.h"
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif
 
#if !defined(MBEDTLS_DEBUG_C)

void mbedtls_debug_set_threshold( int threshold )
{
    (void) threshold;
}

#endif
