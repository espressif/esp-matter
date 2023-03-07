// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef PROVISIONING_SC_X509_ATTESTATION_H
#define PROVISIONING_SC_X509_ATTESTATION_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"
#include "azure_macro_utils/macro_utils.h"
#include "parson.h"

#define X509_CERTIFICATE_TYPE_VALUES \
            X509_CERTIFICATE_TYPE_NONE, \
            X509_CERTIFICATE_TYPE_CLIENT, \
            X509_CERTIFICATE_TYPE_SIGNING, \
            X509_CERTIFICATE_TYPE_CA_REFERENCES \
//Note: X509_CERTIFICATE_TYPE_NONE is invalid, indicating error
MU_DEFINE_ENUM_WITHOUT_INVALID(X509_CERTIFICATE_TYPE, X509_CERTIFICATE_TYPE_VALUES);

typedef struct X509_ATTESTATION_TAG* X509_ATTESTATION_HANDLE;
typedef struct X509_CERTIFICATE_WITH_INFO_TAG* X509_CERTIFICATE_HANDLE;

/* X509 Attestation Accessor Functions */
MOCKABLE_FUNCTION(, X509_CERTIFICATE_TYPE, x509Attestation_getCertificateType, X509_ATTESTATION_HANDLE, x509_att);
MOCKABLE_FUNCTION(, X509_CERTIFICATE_HANDLE, x509Attestation_getPrimaryCertificate, X509_ATTESTATION_HANDLE, x509_att);
MOCKABLE_FUNCTION(, X509_CERTIFICATE_HANDLE, x509Attestation_getSecondaryCertificate, X509_ATTESTATION_HANDLE, x509_att);

/* X509 Certificate Accessor Functions */
MOCKABLE_FUNCTION(, const char*, x509Certificate_getSubjectName, X509_CERTIFICATE_HANDLE, x509_cert);
MOCKABLE_FUNCTION(, const char*, x509Certificate_getSha1Thumbprint, X509_CERTIFICATE_HANDLE, x509_cert);
MOCKABLE_FUNCTION(, const char*, x509Certificate_getSha256Thumbprint, X509_CERTIFICATE_HANDLE, x509_cert);
MOCKABLE_FUNCTION(, const char*, x509Certificate_getIssuerName, X509_CERTIFICATE_HANDLE, x509_cert);
MOCKABLE_FUNCTION(, const char*, x509Certificate_getNotBeforeUtc, X509_CERTIFICATE_HANDLE, x509_cert);
MOCKABLE_FUNCTION(, const char*, x509Certificate_getNotAfterUtc, X509_CERTIFICATE_HANDLE, x509_cert);
MOCKABLE_FUNCTION(, const char*, x509Certificate_getSerialNumber, X509_CERTIFICATE_HANDLE, x509_cert);
MOCKABLE_FUNCTION(, int, x509Certificate_getVersion, X509_CERTIFICATE_HANDLE, x509_cert);



/*---INTERNAL USAGE ONLY---*/
MOCKABLE_FUNCTION(, X509_ATTESTATION_HANDLE, x509Attestation_create, X509_CERTIFICATE_TYPE, cert_type, const char*, primary_cert, const char*, secondary_cert)
MOCKABLE_FUNCTION(, void, x509Attestation_destroy, X509_ATTESTATION_HANDLE, x509_att);
MOCKABLE_FUNCTION(, X509_ATTESTATION_HANDLE, x509Attestation_fromJson, JSON_Object*, root_object);
MOCKABLE_FUNCTION(, JSON_Value*, x509Attestation_toJson, const X509_ATTESTATION_HANDLE, x509_att);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PROVISIONING_SC_X509_ATTESTATION_H */