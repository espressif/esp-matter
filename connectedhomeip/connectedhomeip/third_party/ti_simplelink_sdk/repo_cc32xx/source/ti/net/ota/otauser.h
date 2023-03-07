/*
 * Copyright (c) 2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __OTA_USER_H__
#define __OTA_USER_H__

#ifdef    __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/net/ota/ota.h>

//#define SL_ENABLE_OTA_DEBUG_TRACES /* uncomment to enable internal OTA debug info */
#ifndef SL_ENABLE_OTA_DEBUG_TRACES
#define _SlOtaLibTrace(pargs) 
#else

#ifdef _WIN32

#define _SlOtaLibTrace(pargs)  printf pargs

#elif __MSP432P401R__

extern void LogMessage(const char *pcFormat, ...);
#define _SlOtaLibTrace(pargs)  LogMessage pargs

#else /*CC32xx*/

extern int Report(const char *format, ...);
#define _SlOtaLibTrace(pargs)  Report pargs

#endif

#endif

#define OTA_SERVER_GITHUB      1
#define OTA_SERVER_DROPBOX_V2  2
#define OTA_SERVER_CUSTOM      99

/* The following configuration skips the CDN server handshake,
 * and simply downloads an OTA TAR file from provided URL
 * (of an HTTP server). 
 * Note: For ease of use, a macro definition is provided below for 
 * the file URL (see OTA_FILE_SERVER_URL) which is used by the application 
 * to initialize the OTA library. But, the actual URL is assumed to be dynamic
 * (and temporary). The value should be provided by the applications by 
 * calling OTA_set just before the OTA gets triggered (i.e. before calling OTA_run).
 */
#define OTA_FILE_DOWNLOAD      100

/* USER SHOULD DEFINE HERE WHICH CLOUD TO USE */
/* -------------------------------------------*/
//#define OTA_SERVER_TYPE    OTA_SERVER_GITHUB
//#define OTA_SERVER_TYPE    OTA_SERVER_DROPBOX_V2
#define OTA_SERVER_TYPE      OTA_FILE_DOWNLOAD

#define OTA_VERSION_SUPPORT  (0)

/* OTA server info */
/* --------------- */

#ifdef _WIN32
#define OTA_VENDOR_DIR  "OTA_CC3120"
#elif __MSP432P401R__
#define OTA_VENDOR_DIR  "OTA_CC3120"
#else
#define OTA_VENDOR_DIR  "OTA_CC3220SF"
#endif

#if OTA_SERVER_TYPE == OTA_SERVER_GITHUB

/* Github server info */
#define OTA_SERVER_NAME                 "api.github.com"
#define OTA_SERVER_IP_ADDRESS           0x00000000
#define OTA_SERVER_SECURED              1

/* Github vendor info */
#define OTA_VENDOR_ROOT_DIR             "/repos/<user account>/<user directory>"
//#define OTA_VENDOR_TOKEN              "<User defined Github token>"
#ifndef OTA_VENDOR_TOKEN
#error "Please define your personal cloud account token in OTA_VENDOR_TOKEN above"
#endif

/******************************************************************************
 * An Important Note regarding the GitHub Certificates:
 * GitHub is about to replace the servers' certificates. The change may impact
 * the GitHub OTA server, the GitHub Content server or both. OTA Library Users 
 * should prepare in advance by updating the root CA certificates (for devices
 * in the field).
 * Up to now both servers were verified using "DigCert_High_Assurance_CA.der".
 * Recently GitHub updated their OTA Server to use a certificate that needs to 
 * be verified with "DigiCert_Global_Root_CA.der" (This requires update of
 * OTA_SERVER_ROOT_CA_CERT). 
 * Working with GitHub, TI was able to postpone the change so the library users 
 * can update their devices.
 * The exact date of the change and the content of the change are TBD by GitHub
 * (customers are encouraged to contact GitHub for details).
 *
 * For CC323x customers, TI offer an easier approach. The 
 * "RootCACerts.pem" contains both the old and the new root CA certificates.
 * If used here, this PEM file will enable the connection before and after the
 * change. Unfortunately, this method is not supported by CC3220 devices
 * and thus specific certificate needs to be defined per server.
 * 
 * Having this, it is very important to include a backup method in your code.
 * In case of GitHub failure, the backup can trigger a load from a secondary 
 * server or it can enable the Local OTA method.
 ******************************************************************************/
//#define OTA_SERVER_ROOT_CA_CERT         "DigCert_High_Assurance_CA.der"
//#define OTA_CONTENT_SERVER_ROOT_CA_CERT "DigCert_High_Assurance_CA.der"
#define OTA_SERVER_ROOT_CA_CERT           "RootCACerts.pem"
#define OTA_CONTENT_SERVER_ROOT_CA_CERT   "RootCACerts.pem"
#define OTA_SERVER_AUTH_IGNORE_DATA_TIME_ERROR
#define OTA_SERVER_AUTH_DISABLE_CERT_STORE

#elif OTA_SERVER_TYPE == OTA_SERVER_DROPBOX_V2

/* Dropbox V2 server info */
#define OTA_SERVER_NAME                 "api.dropboxapi.com"
#define OTA_SERVER_IP_ADDRESS           0x00000000
#define OTA_SERVER_SECURED              1

/* Dropbox V2 vendor info */
//#define OTA_VENDOR_TOKEN              "<User defined Dropbox token>"
#ifndef OTA_VENDOR_TOKEN
#error "Please define your personal cloud account token in OTA_VENDOR_TOKEN above"
#endif
/******************************************************************************
 * An Important Note regarding the DropBox Certificates:
 * DropBox is about to replace the servers' certificates. The change may impact
 * the DropBox OTA server, the DropBox Content server or both. OTA Library Users
 * should prepare in advance by updating the root CA certificates (for devices
 * in the field).
 * Up to now both servers were verified using "DigCert_High_Assurance_CA.der".
 * Recently DropBox updated their OTA Content Server to use a certificate that 
 * needs to be verified with "DigiCert_Global_Root_CA.der" (This requires update 
 * of OTA_CONTENT_SERVER_ROOT_CA_CERT).
 * Working with GitHub, TI was able to postpone the change so the library users
 * can update their devices.
 * The change is planned for August 2021. The exact content of the change is TBD 
 * by DropBox (customers are encouraged to contact DropBox for details).
 *
 * For CC323x customers, TI offer an easier approach. The
 * "RootCACerts.pem" contains both the old and the new root CA certificates.
 * If used here, this PEM file will enable the connection before and after the
 * change. Unfortunately, this method is not supported by CC3220 devices
 * and thus specific certificate needs to be defined per server.
 *
 * Having this, it is very important to include a backup method in your code.
 * A fallabck in case of DropBox failure may include triggering a load from 
 * a secondary server or enabling the Local OTA method.
 ******************************************************************************/
//#define OTA_SERVER_ROOT_CA_CERT         "DigCert_High_Assurance_CA.der"
//#define OTA_CONTENT_SERVER_ROOT_CA_CERT "DigCert_High_Assurance_CA.der"
#define OTA_SERVER_ROOT_CA_CERT           "RootCACerts.pem"
#define OTA_CONTENT_SERVER_ROOT_CA_CERT   "RootCACerts.pem"
#define OTA_SERVER_AUTH_IGNORE_DATA_TIME_ERROR
#define OTA_SERVER_AUTH_DISABLE_CERT_STORE

#elif OTA_SERVER_TYPE == OTA_SERVER_CUSTOM

/* add your customer server header here */
#include ""

/* Github server info */
#define OTA_SERVER_NAME                 ""
#define OTA_SERVER_IP_ADDRESS           0x00000000
#define OTA_SERVER_SECURED              1

/* Vendor info */
#define OTA_VENDOR_TOKEN                "User defined token"

#define CdnVendor_SendReqDir           
#define CdnVendor_ParseRespDir          
#define CdnVendor_SendReqFileUrl        
#define CdnVendor_ParseRespFileUrl      

#elif OTA_SERVER_TYPE == OTA_FILE_DOWNLOAD
#define OTA_SERVER_SECURED              1
//#define OTA_CONTENT_SERVER_ROOT_CA_CERT "DigCert_High_Assurance_CA.der"
#define OTA_CONTENT_SERVER_ROOT_CA_CERT   "RootCACerts.pem"

#endif


#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* __OTA_USER_H__ */
