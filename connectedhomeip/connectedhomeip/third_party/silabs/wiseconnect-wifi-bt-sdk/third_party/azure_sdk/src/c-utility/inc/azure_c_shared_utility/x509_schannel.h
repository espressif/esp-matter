// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef X509_SCHANNEL_H
#define X509_SCHANNEL_H

#include "windows.h"

#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct X509_SCHANNEL_HANDLE_DATA_TAG* X509_SCHANNEL_HANDLE;

MOCKABLE_FUNCTION(, X509_SCHANNEL_HANDLE, x509_schannel_create, const char*, x509certificate, const char*, x509privatekey);
MOCKABLE_FUNCTION(, void, x509_schannel_destroy, X509_SCHANNEL_HANDLE, x509_schannel_handle);
MOCKABLE_FUNCTION(, PCCERT_CONTEXT, x509_schannel_get_certificate_context, X509_SCHANNEL_HANDLE, x509_schannel_handle);
MOCKABLE_FUNCTION(, int, x509_verify_certificate_in_chain, const char*, trustedCertificate, PCCERT_CONTEXT, pCertContextToVerify);

#ifdef __cplusplus
}
#endif

#endif
