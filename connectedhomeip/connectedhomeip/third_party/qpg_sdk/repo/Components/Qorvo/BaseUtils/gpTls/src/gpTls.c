/*
 * Copyright (c) 2018, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 */

/** @file "gpTls.c"
 *
 *  Implementation of gpTls
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_TLS

/* <CodeGenerator Placeholder> General */
/* </CodeGenerator Placeholder> General */


#include "gpTls.h"

/* <CodeGenerator Placeholder> Includes */
#include "gpAssert.h"
#include "gpLog.h"
#include "gpSched.h"
#include "gpPoolMem.h"

#include "gpHal_SEC.h"

#if defined(GP_TLS_DIVERSITY_USE_MBEDTLS_ALT)
#include "mbedtls/timing.h"

#include "mbedtls/md.h"
#include "mbedtls/md4.h"
#include "mbedtls/md5.h"
#include "mbedtls/ripemd160.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"

#include "mbedtls/arc4.h"
#include "mbedtls/des.h"
#include "mbedtls/aes.h"
#include "mbedtls/aria.h"
#include "mbedtls/blowfish.h"
#include "mbedtls/camellia.h"
#include "mbedtls/chacha20.h"
#include "mbedtls/gcm.h"
#include "mbedtls/ccm.h"
#include "mbedtls/chachapoly.h"
#include "mbedtls/cmac.h"
#include "mbedtls/poly1305.h"

#include "mbedtls/havege.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/hmac_drbg.h"

#include "mbedtls/rsa.h"
#include "mbedtls/dhm.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/ecdh.h"

#include "mbedtls/error.h"
#include "mbedtls/platform.h"
#endif
/* </CodeGenerator Placeholder> Includes */


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/* <CodeGenerator Placeholder> Macro */
#if defined(GP_TLS_DIVERSITY_USER_DEFINED_MBEDTLS_CONFIG)
#endif
/* </CodeGenerator Placeholder> Macro */

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/* <CodeGenerator Placeholder> FunctionalMacro */
/* </CodeGenerator Placeholder> FunctionalMacro */

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/* <CodeGenerator Placeholder> TypeDefinitions */
/* </CodeGenerator Placeholder> TypeDefinitions */

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/* <CodeGenerator Placeholder> StaticData */
/* </CodeGenerator Placeholder> StaticData */

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/* <CodeGenerator Placeholder> StaticFunctionPrototypes */
/* </CodeGenerator Placeholder> StaticFunctionPrototypes */

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/* <CodeGenerator Placeholder> StaticFunctionDefinitions */
#ifdef MBEDTLS_PLATFORM_MEMORY
static void* gpTls_Calloc(size_t num, size_t size)
{
    void *ptr = gpPoolMem_Malloc(GP_COMPONENT_ID, num * size, true);
    GP_ASSERT_SYSTEM(ptr != NULL);
    memset(ptr, 0, (num*size));
    return ptr;
}

static void gpTls_Free (void* pData)
{
    /*mbedtls does not explicitly check if ptr is NULL before freeing it*/
    if(pData != NULL)
    {
        gpPoolMem_Free(pData);
    }
}
#endif /* MBEDTLS_PLATFORM_MEMORY */

/* </CodeGenerator Placeholder> StaticFunctionDefinitions */

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

gpTls_Result_t gpTls_CCMEncrypt(UInt16 dataLength, UInt8 auxLength, UInt8 micLength, UInt16 outputDataLength, UInt8* pKey, UInt8* pNonce, UInt8* pData, UInt8* pAux, UInt8* pDataOut, UInt8* pMic)
{
/* <CodeGenerator Placeholder> Implementation_gpTls_CCMEncrypt */
    gpTls_Result_t result = gpTls_ResultBusy;
    result = gpHal_CCMEncrypt_RAM(dataLength, auxLength, micLength, pData, pAux, pMic, pKey, pNonce, pDataOut);
    return result;
/* </CodeGenerator Placeholder> Implementation_gpTls_CCMEncrypt */
}
gpTls_Result_t gpTls_CCMDecrypt(UInt16 dataLength, UInt8 auxLength, UInt8 micLength, UInt16 outputDataLength, UInt8* pKey, UInt8* pNonce, UInt8* pData, UInt8* pAux, UInt8* pDataOut, UInt8* pMic)
{
/* <CodeGenerator Placeholder> Implementation_gpTls_CCMDecrypt */
    gpTls_Result_t result = gpTls_ResultBusy;
    result = gpHal_CCMDecrypt_RAM(dataLength, auxLength, micLength, pData, pAux, pMic, pKey, pNonce, pDataOut);
    return result;
/* </CodeGenerator Placeholder> Implementation_gpTls_CCMDecrypt */
}
gpTls_Result_t gpTls_AESEncrypt(UInt8* pInplaceBuffer, UInt8* pAesKey, gpTls_AESOptions_t AESOptions)
{
/* <CodeGenerator Placeholder> Implementation_gpTls_AESEncrypt */
    gpTls_Result_t result = gpTls_ResultBusy;
    gpEncryption_AESOptions_t gpEnc_AESOptions = {.keylen  = (gpEncryption_AESKeyLen_t)AESOptions.keylen,
                                                  .options = AESOptions.options};
    result = gpHal_AESEncrypt(pInplaceBuffer, pAesKey, gpEnc_AESOptions);
    return result;
/* </CodeGenerator Placeholder> Implementation_gpTls_AESEncrypt */
}
gpTls_Result_t gpTls_AESDecrypt(UInt8* pInplaceBuffer, UInt8* pAesKey, gpTls_AESOptions_t AESOptions)
{
/* <CodeGenerator Placeholder> Implementation_gpTls_AESDecrypt */
    gpTls_Result_t result = gpTls_ResultUnsupported;
    return result;
/* </CodeGenerator Placeholder> Implementation_gpTls_AESDecrypt */
}
gpTls_Result_t gpTls_HMACAuth(gpTls_HashFct_t hashFct, UInt8 keyLength, UInt8 msgLength, UInt8 resultLength, UInt8* pKey, UInt8* pMsg, UInt8* pResult)
{
/* <CodeGenerator Placeholder> Implementation_gpTls_HMACAuth */
    gpTls_Result_t result;
#if defined(GP_TLS_DIVERSITY_USE_MBEDTLS_ALT)
    result = gpTls_ResultUnsupported;
    mbedtls_md_type_t md_type = MBEDTLS_MD_NONE;

    switch(hashFct)
    {
        case  gpTls_MD5:
        {
            md_type = MBEDTLS_MD_MD5;
            break;
        }
        case  gpTls_SHA1:
        {
            md_type = MBEDTLS_MD_SHA1;
            break;
        }
        case  gpTls_SHA224:
        {
            md_type = MBEDTLS_MD_SHA224;
            break;
        }
        case  gpTls_SHA256:
        {
            md_type = MBEDTLS_MD_SHA256;
            break;
        }
        case  gpTls_SHA384:
        {
            md_type = MBEDTLS_MD_SHA384;
            break;
        }
        case  gpTls_SHA512:
        {
            md_type = MBEDTLS_MD_SHA512;
            break;
        }
        default:
        {
            return gpTls_ResultUnsupported;
        }
    }
    result = mbedtls_md_hmac(mbedtls_md_info_from_type(md_type), (const unsigned char*)pKey, keyLength, (const unsigned char*)pMsg, msgLength, pResult);

    if(result == 0)
    {
        result = gpTls_ResultSuccess;
    }
    else
    {
        result = gpTls_ResultInvalidParameter;
    }

#else
    result = gpHal_HMAC((uint8_t)hashFct, keyLength, msgLength, resultLength, pKey, pMsg, pResult);
#endif
    return result;
/* </CodeGenerator Placeholder> Implementation_gpTls_HMACAuth */
}
void gpTls_Init(void)
{
/* <CodeGenerator Placeholder> Implementation_gpTls_Init */

#ifdef MBEDTLS_PLATFORM_MEMORY
    /* Setup calloc/free call in mbedtls to point to
     * gpPoolMem implementation */
    mbedtls_platform_set_calloc_free(gpTls_Calloc, gpTls_Free);
#endif /* MBEDTLS_PLATFORM_MEMORY */
/* </CodeGenerator Placeholder> Implementation_gpTls_Init */
}
