// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef SHARED_UTIL_OPTIONS_H
#define SHARED_UTIL_OPTIONS_H

#include "azure_c_shared_utility/const_defines.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct HTTP_PROXY_OPTIONS_TAG
    {
        const char* host_address;
        int port;
        const char* username;
        const char* password;
    } HTTP_PROXY_OPTIONS;

    static STATIC_VAR_UNUSED const char* const OPTION_HTTP_PROXY = "proxy_data";
    static STATIC_VAR_UNUSED const char* const OPTION_HTTP_TIMEOUT = "timeout";

    static STATIC_VAR_UNUSED const char* const OPTION_TRUSTED_CERT = "TrustedCerts";

    // Clients should not use OPTION_OPENSSL_CIPHER_SUITE except for very specialized scenarios.
    // They instead should rely on the underlying client TLS stack and service to negotiate an appropriate cipher.
    static STATIC_VAR_UNUSED const char* const OPTION_OPENSSL_CIPHER_SUITE = "CipherSuite";

    static STATIC_VAR_UNUSED const char* const OPTION_OPENSSL_ENGINE = "Engine";
    static STATIC_VAR_UNUSED const char* const OPTION_OPENSSL_PRIVATE_KEY_TYPE = "x509PrivatekeyType";

    typedef enum OPTION_OPENSSL_KEY_TYPE_TAG
    {
        KEY_TYPE_DEFAULT,
        KEY_TYPE_ENGINE
    } OPTION_OPENSSL_KEY_TYPE;

    static STATIC_VAR_UNUSED const char* const SU_OPTION_X509_CERT = "x509certificate";
    static STATIC_VAR_UNUSED const char* const SU_OPTION_X509_PRIVATE_KEY = "x509privatekey";

    static STATIC_VAR_UNUSED const char* const OPTION_X509_ECC_CERT = "x509EccCertificate";
    static STATIC_VAR_UNUSED const char* const OPTION_X509_ECC_KEY = "x509EccAliasKey";

    static STATIC_VAR_UNUSED const char* const OPTION_CURL_LOW_SPEED_LIMIT = "CURLOPT_LOW_SPEED_LIMIT";
    static STATIC_VAR_UNUSED const char* const OPTION_CURL_LOW_SPEED_TIME = "CURLOPT_LOW_SPEED_TIME";
    static STATIC_VAR_UNUSED const char* const OPTION_CURL_FRESH_CONNECT = "CURLOPT_FRESH_CONNECT";
    static STATIC_VAR_UNUSED const char* const OPTION_CURL_FORBID_REUSE = "CURLOPT_FORBID_REUSE";
    static STATIC_VAR_UNUSED const char* const OPTION_CURL_VERBOSE = "CURLOPT_VERBOSE";

    static STATIC_VAR_UNUSED const char* const OPTION_NET_INT_MAC_ADDRESS = "net_interface_mac_address";

    static STATIC_VAR_UNUSED const char* const OPTION_SET_TLS_RENEGOTIATION = "tls_renegotiation";

    // DEPRECATED: The underlying security library for your platform will use a secure TLS version
    // that in general  should not be overridden with OPTION_TLS_VERSION.
    static STATIC_VAR_UNUSED const char* const OPTION_TLS_VERSION = "tls_version";

    static STATIC_VAR_UNUSED const char* const OPTION_ADDRESS_TYPE = "ADDRESS_TYPE";
    static STATIC_VAR_UNUSED const char* const OPTION_ADDRESS_TYPE_DOMAIN_SOCKET = "DOMAIN_SOCKET";
    static STATIC_VAR_UNUSED const char* const OPTION_ADDRESS_TYPE_IP_SOCKET = "IP_SOCKET";

#ifdef __cplusplus
}
#endif

#endif /* SHARED_UTIL_OPTIONS_H */
