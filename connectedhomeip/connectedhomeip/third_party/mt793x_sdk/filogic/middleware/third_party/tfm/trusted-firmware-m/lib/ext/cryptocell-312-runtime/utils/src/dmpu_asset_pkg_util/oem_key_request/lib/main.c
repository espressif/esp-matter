/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <openssl/objects.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/bn.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include "common_util_log.h"
#include "common_crypto_asym.h"
#include "common_crypto_sym.h"
#include "common_util_files.h"
#include "cc_production_asset.h"
#include "common_sb_ops.h"

#define Nullptr (void *)0

/**
 * @brief The SBU_RSA_Sign generates RSA signature and returns it.
 *
 * The function follows the steps:
 * 1. Read RSA private key structure
 * 2. Call function according to PKCS version to create RSA signature
 *
 * @param[in] pkcsVersion - the version used (according to global definitions of available versions)
 * @param[in] DataIn_ptr - the data to sign on
 * @param[in] DataInSize - the data size
 * @param[in] PemEncryptedFileName_ptr - the private key file
 * @param[in] pwdFileName - file name of the password
 * @param[out] Signature_ptr - the RSA signature
 *
 */
 /*********************************************************/
SBUEXPORT_C int SBU_RSA_Sign(int pkcsVersion,
                             char* DataIn_ptr,
                             unsigned int DataInSize,
                             char* PemEncryptedFileName_ptr,
                             char* pwdFileName,
                             char* Signature_ptr)
{
    RSA  *pRsaPrivKey = NULL;
    unsigned char *pwd = NULL;
    int  ret_code;

    OpenSSL_add_all_algorithms ();

    /* parse the passphrase for a given file */
    if( strlen(pwdFileName) ) {
        if(CC_CommonGetPassphrase(pwdFileName, &pwd) != 0){
            printf("Failed to retrieve pwd\n");
            if (pwd != NULL)
                free(pwd);
            return (-1);
        }
    }
    else {
        pwd = Nullptr;
    }

    if (CC_CommonGetKeyPair (&pRsaPrivKey, PemEncryptedFileName_ptr, pwd) < 0)
    {
        printf ("\nCC_CommonGetKeyPair Cannot read RSA private key\n");
        return (-1);
    }

        ret_code = Sign_v21(pRsaPrivKey, DataIn_ptr, DataInSize, Signature_ptr);

END:
    EVP_cleanup();
    return (ret_code);
}

