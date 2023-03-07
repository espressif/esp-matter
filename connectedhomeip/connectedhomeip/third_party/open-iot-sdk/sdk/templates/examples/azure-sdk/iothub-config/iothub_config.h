/* Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IOTHUB_CONFIG_H
#define IOTHUB_CONFIG_H

#include "iothub_credentials.h"

/** Azure IoT Hub server TLS certificate - mbedtls specific.
 * It's passed as OPTION_TRUSTED_CERT option to IoT Hub client.
 */
#ifdef IOTHUB_SERVER_TLS_CERTIFICATE
const char *certificates = IOTHUB_SERVER_TLS_CERTIFICATE;
#endif // IOTHUB_SERVER_TLS_CERTIFICATE

#ifdef IOTHUB_DEVICE_CONNECTION_STRING
const char *connectionString = IOTHUB_DEVICE_CONNECTION_STRING;
#endif // IOTHUB_DEVICE_CONNECTION_STRING

#ifdef IOTHUB_DEVICE_X509_CERT
const char *x509certificate = IOTHUB_DEVICE_X509_CERT;
#endif // IOTHUB_DEVICE_X509_CERT

#ifdef IOTHUB_DEVICE_X509_PRIV_KEY
const char *x509privatekey = IOTHUB_DEVICE_X509_PRIV_KEY;
#endif // IOTHUB_DEVICE_X509_PRIV_KEY

#endif /* IOTHUB_CONFIG_H */
