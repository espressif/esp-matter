// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef X509_OPENSSL_H
#define X509_OPENSSL_H

#include "openssl/ssl.h"
#include "azure_c_shared_utility/shared_util_options.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umock_c/umock_c_prod.h"

MOCKABLE_FUNCTION(,int, x509_openssl_add_certificates, SSL_CTX*, ssl_ctx, const char*, certificates);
MOCKABLE_FUNCTION(,int, x509_openssl_add_credentials, SSL_CTX*, ssl_ctx, const char*, x509certificate, const char*, x509privatekey, OPTION_OPENSSL_KEY_TYPE, x509privatekeytype, ENGINE*, engine);

#ifdef __cplusplus
}
#endif

#endif
