/***************************************************************************//**
 * @file
 * @brief Sample Certificates And Private Keys For SSL/TLS Ports
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/****************************************************************************************************//**
 * @note     (1) The way to format PEM Certificates & Keys and their lenghts may vary from one port to
 *               another. This file serves the purpose of abstracting this.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                             INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#if 0 // Define one
#define  POLARSSL
#define  CUSTOM
#define  VALICORE
#define  GEOTRUST
#endif

#define  POLARSSL

#ifdef POLARSSL
#define  SSL_CA_CERT          SSL_CA_Cert_PolarSSL
#define  SSL_SERVER_CERT      SSL_ServerCert_PolarSSL
#define  SSL_SERVER_KEY       SSL_ServerKey_PolarSSL
#define  SSL_CLIENT_CERT      SSL_ClientCert_PolarSSL
#define  SSL_CLIENT_KEY       SSL_ClientKey_PolarSSL
#endif

#ifdef CUSTOM
#define  SSL_CA_CERT          SSL_CA_Cert_Symantec
#define  SSL_SERVER_CERT      SSL_ServerCert_Custom
#define  SSL_SERVER_KEY       SSL_ServerKey_Custom
#define  SSL_CLIENT_CERT      SSL_ClientCert_Custom
#define  SSL_CLIENT_KEY       SSL_ClientKey_Custom
#endif

extern const CPU_CHAR SSL_CA_Cert_PolarSSL[];
extern const CPU_CHAR SSL_ServerCert_PolarSSL[];
extern const CPU_CHAR SSL_ServerKey_PolarSSL[];
extern const CPU_CHAR SSL_ClientCert_PolarSSL[];
extern const CPU_CHAR SSL_ClientKey_PolarSSL[];

extern const CPU_CHAR SSL_CA_Cert_Symantec[];                   // From Amazon
extern const CPU_CHAR SSL_ServerCert_Custom[];
extern const CPU_CHAR SSL_ServerKey_Custom[];
extern const CPU_CHAR SSL_ClientCert_Custom[];
extern const CPU_CHAR SSL_ClientKey_Custom[];

extern const CPU_CHAR SSL_CA_Cert_GeoTrust[];
extern const CPU_CHAR SSL_CA_Cert_GlobalSign[];
extern const CPU_CHAR SSL_CA_Cert_DST[];
extern const CPU_CHAR SSL_CA_Cert_GoDaddy[];
extern const CPU_CHAR SSL_CA_Cert_Thawte[];
extern const CPU_CHAR SSL_CA_Cert_Equifax[];

extern const CPU_INT32U SSL_CA_Cert_Len;
extern const CPU_INT32U SSL_ServerCertLen;
extern const CPU_INT32U SSL_ServerKeyLen;
extern const CPU_INT32U SSL_ClientCertLen;
extern const CPU_INT32U SSL_ClientKeyLen;
