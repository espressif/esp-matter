/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/opensslv.h>
#include <openssl/conf.h>
#include <openssl/x509v3.h>

#include "common_crypto_x509.h"
#include "common_crypto_asym.h"
#include "common_rsa_keypair.h"
#include "common_util_files.h"
#include "common_util_log.h"
#include "cc_crypto_boot_defs.h"
#include "cc_crypto_x509_defs.h"

#ifdef CC_SB_SUPPORT_IOT
#include "secdebug_defs.h"
#endif

#define X509_VER_3  2  /* version 3 certificate */
#define ENDLESS_VALIDITY  0xFFFFFFFE
#define MAX_OBJ_ID_LEN  15
#define EXT_PREFIX_LEN  16
#ifdef CC_SB_SUPPORT_IOT
#define MAX_EXT_VAL_LIST (CC_SB_MAX_CONTENT_CERT_BODY_SIZE_IN_BYTES)
#else
#define MAX_EXT_VAL_LIST 80 // additional data
#endif
#define MAX_EXT_INT_VAL_LEN  80

/* multiple each byte by 3 for: each byte has 2 character representaion + ":" */
#define MAX_EXT_VAL_LEN  (MAX_EXT_VAL_LIST *3 + EXT_PREFIX_LEN)
#define CHAR_STR_LEN 6

#define OPEN_SSL_ERROR   0
#define IS_VALID_ENC_FLAG(encFlag) ((0 == (encFlag)) || (1 == (encFlag)) || (0xFF == (encFlag)))
#define IS_VALID_HBK(hbkType) ((CC_SB_HASH_BOOT_KEY_0_128B == (hbkType)) ||\
                   (CC_SB_HASH_BOOT_KEY_1_128B == (hbkType)) ||\
                   (CC_SB_HASH_BOOT_KEY_256B == (hbkType)) ||\
                   (CC_SB_HASH_BOOT_NOT_USED == (hbkType)))

const uint8_t *certType2Str[CC_X509_CERT_TYPE_MAX] = {NULL,
  /*CC_X509_CERT_TYPE_KEY      */      (uint8_t *)CC_X509_CERT_KEY_CERT,
  /*CC_X509_CERT_TYPE_CONTENT  */      (uint8_t *)CC_X509_CERT_CNT_CERT,
  /*CC_X509_CERT_TYPE_ENABLER_DBG */      (uint8_t *)CC_X509_CERT_ENABLER_CERT,
  /*CC_X509_CERT_TYPE_DEVELOPER_DBG */      (uint8_t *)CC_X509_CERT_DEVELOPER_CERT
};


/**
* @brief free X509 certificate
*
* @param[in/out] ppCertBuff          - x.509 certificate
*/
/*********************************************************/
void CC_CommonX509Free(uint8_t **ppCertBuff)
{
    /* validate inputs */
    if ((NULL == ppCertBuff) ||
        (NULL == *ppCertBuff)) {
        UTIL_LOG_ERR("ilegal input\n");
        return;
    }

    UTIL_LOG_INFO("about to X509_free\n");
    /* create the certificate buffer */
    X509_free((X509 *)*ppCertBuff);
    *ppCertBuff = NULL;
    return;
}


/**
* @brief Creates X509 certificate and set its header fields
*
* @param[in/out] ppCertBuff     - x.509 certificate
* @param[in] certType           - certificate type
*/
/*********************************************************/
int32_t CC_CommonX509CreateAndSetHeader(uint8_t **ppCertBuff,
                CCX509CertType_t  certType,CCX509CertHeaderParamsIn_t *pCertHeaderParams)
{
    int32_t rc = 0;
    long endDate = 0;
    uint32_t  serialNum = 0;
    long notBefore = 0; /*X509_CURRENT_TIME*/
    long notAfter = ENDLESS_VALIDITY;
    char* pIssuerName = CC_X509_CERT_ISSUER_NAME;
    const char* pSubjectName = certType2Str[certType];
    X509 *plCert = NULL;
    ASN1_TIME *pDummy = NULL;

    /* validate inputs */
    if ((NULL == ppCertBuff) ||
        (certType >= CC_X509_CERT_TYPE_MAX) ) {
        UTIL_LOG_ERR("ilegal input\n");
        return(-1);
    }

    /* create the certificate buffer */
    plCert = (X509 *)X509_new();
    if (OPEN_SSL_ERROR == plCert) {
        UTIL_LOG_ERR("failed to X509_new\n");
        return 1;
    }

    /* set certificate version to V3 */
    rc = X509_set_version(plCert, X509_VER_3);
    if (OPEN_SSL_ERROR == rc) {
        UTIL_LOG_ERR("failed to X509_set_version\n");
        rc = 1;
        goto END;
    }
    /* set certificate serial number */
    if ( (NULL != pCertHeaderParams) && (pCertHeaderParams->setSerialNum) ) {
        serialNum = pCertHeaderParams->serialNum;
    } else {
        rc = CC_CommonRandBytes(4, (int8_t *)&serialNum);
        if (rc != 0) {
            UTIL_LOG_ERR("failed to set CC_CommonRandBytes\n");
            rc = 1;
            goto END;
        }
    }

    rc = ASN1_INTEGER_set(X509_get_serialNumber(plCert), serialNum);
    if (OPEN_SSL_ERROR == rc) {
        UTIL_LOG_ERR("failed to set X509_get_serialNumber\n");
        rc = 1;
        goto END;
    }
    /* set ecrtificate start time to current time, and end date according to input */
    if ( (NULL != pCertHeaderParams) && (pCertHeaderParams->setNotBefore) ) {
        notBefore = pCertHeaderParams->notBefore;
    }
    pDummy = X509_gmtime_adj(X509_get_notBefore(plCert), (long)notBefore );
    if (NULL == pDummy) {
        UTIL_LOG_ERR("failed set X509_get_notBefore\n");
        rc = 1;
        goto END;
    }
    if ( (NULL != pCertHeaderParams) && (pCertHeaderParams->setNotAfter) ) {
        notAfter = pCertHeaderParams->notAfter;
    }
    pDummy = X509_gmtime_adj(X509_get_notAfter(plCert),(long)notAfter);
    if (NULL == pDummy) {
        UTIL_LOG_ERR("failed set X509_get_notAfter\n");
        rc = 1;
        goto END;
    }

    /* set subject name */
    if ( (NULL != pCertHeaderParams) && (pCertHeaderParams->setSubjectName) ) {
        pSubjectName = pCertHeaderParams->SubjectName;
    }
    rc = X509_NAME_add_entry_by_txt(X509_get_subject_name(plCert),
                        "CN",       /* common name */
                        MBSTRING_ASC,
                        pSubjectName,
                        -1,
                        -1, 0);
    if (OPEN_SSL_ERROR == rc) {
        UTIL_LOG_ERR("failed to set X509_get_subject_name\n");
        rc = 1;
        goto END;
    }

    /* set issuer name */
    if ( (NULL != pCertHeaderParams) && (pCertHeaderParams->setIssuerName) ) {
        pIssuerName = pCertHeaderParams->IssuerName;
    }
    rc = X509_NAME_add_entry_by_txt(X509_get_issuer_name(plCert),
                        "CN",
                        MBSTRING_ASC,
                        pIssuerName,
                        -1,
                        -1, 0);
    if (OPEN_SSL_ERROR == rc) {
        UTIL_LOG_ERR("failed to set X509_get_issuer_name\n");
        rc = 1;
        goto END;
    }



    *ppCertBuff = (uint8_t *)plCert;
    rc = 0;
    UTIL_LOG_INFO("OK\n");

    END:
    if (rc != 0) {
        if (plCert != NULL) {
            X509_free(plCert);
        }
        *ppCertBuff = NULL;
    }
    return rc;
}


/**
* @brief Add ASN.1 critical integer extension to X.509V3 certificate
*
* @param[in/out] pCertBuff          - x.509 certificate
* @param[in] certType                 - certificate type
* @param[in] extType              - extension type
* @param[in] val              - Extension value
*/
/*********************************************************/
int32_t CC_CommonX509AddIntegerExtension(uint8_t *pCertBuff,
                       CCX509CertType_t certType,
                       CCX509ExtType_t  extType,
                       int32_t val)

{
    int32_t rc = 0;
    int32_t nid = 0;
    X509 *plCert = (X509 *)pCertBuff;
    X509_EXTENSION *ext = NULL;
    uint8_t objId[MAX_OBJ_ID_LEN];
    uint8_t extValue[MAX_EXT_INT_VAL_LEN];
    int32_t writtenBytes = 0;

    /* validate inputs */
    if (NULL == pCertBuff) {
        UTIL_LOG_ERR("Illegal parameters \n");
        return (-1);
    }

    /* create new object */
    snprintf(objId, MAX_OBJ_ID_LEN, "2.20.%d.%d",certType,extType);
    nid = OBJ_create(objId, "MyAlias", "My Test Alias Extension");
    if (OPEN_SSL_ERROR == nid) {
        UTIL_LOG_ERR("failed to OBJ_create\n");
        ERR_print_errors_fp(stderr);
        rc = 1;
        goto END;
    }
    rc = X509V3_EXT_add_alias(nid, NID_netscape_comment);
    if (OPEN_SSL_ERROR == nid) {
        UTIL_LOG_ERR("failed to X509V3_EXT_add_alias\n");
        ERR_print_errors_fp(stderr);
        rc = 1;
        goto END;
    }
    /* create the extension value */
    writtenBytes = snprintf(extValue, MAX_EXT_INT_VAL_LEN, "critical,ASN1:INTEGER:0x%X",val);
    /* build the extension */
    ext = X509V3_EXT_conf_nid(NULL, NULL, nid, extValue);
    if (OPEN_SSL_ERROR == ext) {
        UTIL_LOG_ERR("failed to X509V3_EXT_conf_nid\n");
        ERR_print_errors_fp(stderr);
        rc = 1;
        goto END;
    }

    /* Add the extension to the certificate */
    rc = X509_add_ext(plCert, ext, -1);
    if (OPEN_SSL_ERROR == rc) {
        UTIL_LOG_ERR("failed to X509_add_ext\n");
        ERR_print_errors_fp(stderr);
        rc = 1;
        goto END;
    }
    rc = 0;
    UTIL_LOG_INFO("OK\n");

    END:
    X509_EXTENSION_free(ext);
    return rc;
}


/**
* @brief Add critical DER extension to X.509V3 certificate
*
* @param[in/out] pCertBuff          - x.509 certificate
* @param[in] certType                 - certificate tyoes
* @param[in] extType              - extension type
* @param[in] pVal                 - Extension data
* @param[in] valLen               - extension data length
*/
/*********************************************************/
int32_t CC_CommonX509AddStringExtension(uint8_t *pCertBuff,
                      CCX509CertType_t  certType,
                      CCX509ExtType_t  extType,
                      uint8_t *pVal,
                      uint32_t valLen)

{
    int32_t rc = 0;
    int32_t nid;
    X509 *plCert = (X509 *)pCertBuff;
    X509_EXTENSION *ext = NULL;
    uint8_t objId[MAX_OBJ_ID_LEN];
    uint8_t extValue[MAX_EXT_VAL_LEN];
    int32_t writtenBytes = 0;
    int32_t pValIdx = 0;

    /* validate inputs */
    if ((NULL == pCertBuff) ||
        (NULL == pVal)||
        (valLen > MAX_EXT_VAL_LIST)) {
        UTIL_LOG_ERR("Illegal parameters \n");
        return (-1);
    }
    /* create new object */
    snprintf(objId, MAX_OBJ_ID_LEN, "2.20.%d.%d",certType,extType);
    nid = OBJ_create(objId, "MyAlias", "My Test Alias Extension");
    if (OPEN_SSL_ERROR == nid) {
        UTIL_LOG_ERR("failed to OBJ_create\n");
        ERR_print_errors_fp(stderr);
        rc = 1;
        goto END;
    }

    pValIdx = 0;
    writtenBytes = snprintf(extValue, MAX_EXT_VAL_LEN, "critical,DER: %02X",pVal[pValIdx++]);
    UTIL_LOG_INFO("writtenBytes %d, extValue %s\n",writtenBytes, extValue);
    while (pValIdx < valLen) {
        writtenBytes += snprintf((extValue+writtenBytes), CHAR_STR_LEN, ":%02X", pVal[pValIdx++]);
        UTIL_LOG_INFO("writtenBytes %d, extValue %s\n",writtenBytes, extValue);
    }

    rc = X509V3_EXT_add_alias(nid, NID_netscape_comment);  // if NID is unknown openssl ignores it. meaning it is not added to cert.
    if (OPEN_SSL_ERROR == rc) {
        UTIL_LOG_ERR("failed to X509V3_EXT_add_alias\n");
        ERR_print_errors_fp(stderr);
        rc = 1;
        goto END;
    }

    ext = X509V3_EXT_conf_nid(NULL, NULL, nid, extValue);
    if (OPEN_SSL_ERROR == ext) {
        UTIL_LOG_ERR("failed to X509V3_EXT_conf_nid\n");
        ERR_print_errors_fp(stderr);
        rc = 1;
        goto END;
    }

    /* Add the extension to the certificate */
    rc = X509_add_ext(plCert, ext, -1);
    if (OPEN_SSL_ERROR == rc) {
        UTIL_LOG_ERR("failed to X509_add_ext\n");
        ERR_print_errors_fp(stderr);
        rc = 1;
        goto END;
    }
    rc = 0;
    UTIL_LOG_INFO("OK\n");

    END:
        if (ext != NULL) {
                X509_EXTENSION_free(ext);
        }
    return rc;
}


/**
* @brief Add subject public key to the X509 certificate
*   and sign the certificate
*
* @param[in/out] pCertBuff      - x.509 certificate
* @param[in] pKeyPairFileName   - key pair file name in PEM format
* @param[in] pKeyPairPwd    - passphrase of key pair
*/
/*********************************************************/
int32_t CC_CommonX509SetKeyAndSign(uint8_t *pCertBuff,
                uint8_t *pKeyPairFileName,
                uint8_t *pKeyPairPwd)
{
    int32_t rc = 0;
    X509 *plCert = (X509 *)pCertBuff;
    RSA  *pRsaKeyPair = NULL;
    uint8_t *pwd = NULL;
    EVP_PKEY *pKey     = NULL;
    EVP_MD_CTX  mdCtx;
    EVP_PKEY_CTX *pKeyCtx = NULL;

    /* validate inputs */
    if ((NULL == pCertBuff) ||
        (NULL == pKeyPairFileName)) {
        UTIL_LOG_ERR("ilegal input\n");
        return(-1);
    }

    /* get certificate Subject's RSA public and private key from key pair file */
    /* parse the passphrase for a given file */
    if (pKeyPairPwd != NULL) {
        rc = CC_CommonGetPassphrase(pKeyPairPwd, &pwd);
        if (rc != CC_COMMON_OK) {
            UTIL_LOG_ERR("Failed to retrieve pwd\n");
            goto END;
        }
    }
    pRsaKeyPair = RSA_new();
    if (NULL == pRsaKeyPair) {
        UTIL_LOG_ERR("Failed RSA_new\n");
        goto END;
    }
    rc = CC_CommonGetKeyPair(&pRsaKeyPair, pKeyPairFileName, pwd);
    if (rc != CC_COMMON_OK) {
        UTIL_LOG_ERR("CC_CommonGetKeyPair Cannot read RSA private key\n");
        rc = 1;
        goto END;
    }
    /* allocate an empty EVP_PKEY structure which
    is used by OpenSSL to store private keys.*/
    pKey = EVP_PKEY_new();
    if (NULL == pKey) {
        UTIL_LOG_ERR("failed to EVP_PKEY_new\n");
        rc = 1;
        goto END;
    }
    /*set the referenced key to RSA key*/
    rc = EVP_PKEY_assign_RSA(pKey, pRsaKeyPair);
    if (OPEN_SSL_ERROR == rc) {
        UTIL_LOG_ERR("failed to EVP_PKEY_assign_RSA\n");
        rc = 1;
        goto END;
    }


    EVP_MD_CTX_init(&mdCtx);
    rc = EVP_DigestSignInit(&mdCtx, &pKeyCtx, EVP_sha256(), NULL, pKey);
    if (OPEN_SSL_ERROR == rc) {
        UTIL_LOG_ERR("failed to EVP_DigestSignInit\n");
        rc = 1;
        goto END;
    }
    rc = EVP_PKEY_CTX_set_rsa_padding(pKeyCtx, RSA_PKCS1_PSS_PADDING);
    if (rc <= OPEN_SSL_ERROR) {
        UTIL_LOG_ERR("failed to EVP_PKEY_CTX_set_rsa_padding\n");
        rc = 1;
        goto END;
    }
    rc = EVP_PKEY_CTX_set_rsa_pss_saltlen(pKeyCtx, RSA_SALT_LEN);
    if (rc <= OPEN_SSL_ERROR) {
        UTIL_LOG_ERR("failed to EVP_PKEY_CTX_set_rsa_pss_saltlen\n");
        rc = 1;
        goto END;
    }
    rc = EVP_PKEY_CTX_set_rsa_mgf1_md(pKeyCtx, EVP_sha256());
    if (rc <= OPEN_SSL_ERROR) {
        UTIL_LOG_ERR("failed to EVP_PKEY_CTX_set_rsa_mgf1_md\n");
        rc = 1;
        goto END;
    }
    UTIL_LOG_INFO("about to X509_set_pubkey\n");
    /*set the key into certificate*/
    rc = X509_set_pubkey(plCert,pKey);
    if (OPEN_SSL_ERROR == rc) {
        UTIL_LOG_ERR("failed to X509_set_pubkey\n");
        ERR_print_errors_fp(stderr);
        rc = 1;
        goto END;
    }

    UTIL_LOG_INFO("about to X509_sign_ctx\n");
    rc = X509_sign_ctx(plCert, &mdCtx);
    if (OPEN_SSL_ERROR == rc) {
        UTIL_LOG_ERR("failed to X509_sign\n");
        rc = 1;
        goto END;
    }
    rc = 0;
    UTIL_LOG_INFO("OK\n");

END:
    EVP_MD_CTX_cleanup(&mdCtx);
    if (pRsaKeyPair != NULL) {
        RSA_free(pRsaKeyPair); // free pKey as well
    }
    if (pwd != NULL) {
        free(pwd);
    }
    return rc;
}


/**
* @brief convert the x.509 certificate to DER format
*
* @param[in/out] ppCertBuff      - x.509 certificate
* @param[out] pOutCertSize  - certificate size in DER format
*/
/*********************************************************/
int32_t CC_CommonX509ToDer(uint8_t **ppCertBuff,
                uint32_t *pOutCertSize)
{
    unsigned char *outBuff = NULL;
    uint8_t *pPemCertData = NULL;

    /* validate inputs */
    if ((NULL == ppCertBuff) ||
        (NULL == pOutCertSize)) {
        UTIL_LOG_ERR("ilegal input\n");
        return(-1);
    }


    *pOutCertSize = i2d_X509((X509 *)*ppCertBuff, &outBuff);
    CC_CommonX509Free(ppCertBuff);
    *ppCertBuff = outBuff;

    UTIL_LOG_INFO("OK\n");

    return 0;
}


#ifdef CC_SB_SUPPORT_IOT

/**
* @brief build package for the certificate
*
* @param[in] ppCertBuff          - the x509 certificate  in PEM format
* @param[in] certSize       - certificate size
* @param[in] certType           - certificate type
* @param[in] encFlag        - indicates whether images were encrypted
* @param[in] hbkType        - hbk type to use by target, in the verification
* @param[in] pAddData       - additional data to add to package
* @param[in] addDataSize        - length of additional data
* @param[in] outPkgFile     - package file name to write the package to
*/
/*********************************************************/
int32_t CC_CommonX509BuildCertPkg(uint8_t **ppCertBuff,
                uint32_t certSize,
                uint8_t *pAddData,
                uint32_t addDataSize,
                uint8_t *outPkgFile)
{
    int32_t rc = 0;
    FILE *fp = NULL;
    uint8_t *pCertPkg = NULL;
    uint32_t pkgBytesSize = 0;

    UTIL_LOG_INFO("started\n");
    /* check inputs */
    if ((NULL == outPkgFile) ||
        (NULL == ppCertBuff) || (NULL == *ppCertBuff) ||
        (0 == certSize) ||
        (certSize >= CC_X509_MAX_CERT_SIZE) ||
        ((pAddData != NULL) && (0 == addDataSize)) ||
        ((NULL == pAddData) && (addDataSize != 0))) {
        UTIL_LOG_ERR("illegal input\n");
        rc = (-1);
        goto END;
    }

    /* calcultae package size */
    pkgBytesSize = (certSize + addDataSize + 1); /* Adding 1 for "\0"*/

    UTIL_LOG_INFO("openning certificate pkg file for writing\n");
    fp = fopen(outPkgFile, "w");
    if (NULL == fp) {
        UTIL_LOG_ERR("failed to open %s\n", outPkgFile);
        rc = (-1);
        goto END;
    }

    UTIL_LOG_INFO("about to allocate memory for pkg:size %d, certSize %d, addDataSize %d\n",pkgBytesSize, certSize, addDataSize);

    /* create the package buffer */
    pCertPkg = (uint8_t *) malloc(pkgBytesSize);
    if (pCertPkg == NULL){
        UTIL_LOG_ERR("failed to allocate pkg\n");
        rc = (-1);
        goto END;
    }
    /* copy additional data to package */
    if (pAddData != NULL) {
        memcpy(&pCertPkg[0], pAddData, addDataSize);
    }
    /* copy certificate PEM  to package */
    memcpy(&pCertPkg[addDataSize], *ppCertBuff, certSize);


    /* write out the package in binary format  */
    UTIL_LOG_INFO("writing pkg to file\n");
    rc = CC_CommonUtilCopyBuffToBinFile(outPkgFile, pCertPkg, pkgBytesSize);
    if (rc != 0) {
        UTIL_LOG_ERR("failed to CC_CommonUtilCopyBuffToBinFile\n");
        rc = 1;
        goto END;
    }
    rc = 0;
    UTIL_LOG_INFO("OK\n");

END:
    if (fp != NULL){
        fclose(fp);
    }
    if (pCertPkg != NULL) {
        free(pCertPkg);
    }
    return rc;
}

#else
/**
* @brief build package for the certificate
*
* @param[in] ppCertBuff          - the x509 certificate  in PEM format
* @param[in] certSize       - certificate size
* @param[in] certType           - certificate type
* @param[in] encFlag        - indicates whether images were encrypted
* @param[in] hbkType        - hbk type to use by target, in the verification
* @param[in] pAddData       - additional data to add to package
* @param[in] addDataSize        - length of additional data
* @param[in] outPkgFile     - package file name to write the package to
*/
/*********************************************************/
int32_t CC_CommonX509BuildCertPkg(uint8_t **ppCertBuff,
                uint32_t certSize,
                CCX509CertType_t certType,
                uint8_t encFlag,
                uint8_t hbkType,
                uint8_t *pAddData,
                uint32_t addDataSize,
                uint8_t *outPkgFile)
{
    int32_t rc = 0;
    FILE *fp = NULL;
    uint8_t *pCertPkg = NULL;
    uint32_t nextBlockOffset = 0;
    uint32_t pkgBytesSize = 0;

    UTIL_LOG_INFO("started\n");
    /* check inputs */
    if ((NULL == outPkgFile) ||
        (NULL == ppCertBuff) || (NULL == *ppCertBuff) ||
        (0 == certSize) ||
        (certSize >= CC_X509_MAX_CERT_SIZE) ||
        ((certType <= CC_X509_CERT_TYPE_MIN) || (certType >= CC_X509_CERT_TYPE_MAX)) ||
        ((pAddData != NULL) && (0 == addDataSize)) ||
        ((NULL == pAddData) && (addDataSize != 0)) ||
        (!IS_VALID_ENC_FLAG(encFlag & 0xFF)) ||
        (!IS_VALID_HBK(hbkType & 0xFF))) {
        UTIL_LOG_ERR("illegal input\n");
        rc = (-1);
        goto END;
    }

    /* calcultae package size */
    pkgBytesSize = (sizeof(CCX509PkgHeader_t) + addDataSize);

    UTIL_LOG_INFO("openning certificate pkg file for writing\n");
    fp = fopen(outPkgFile, "w");
    if (NULL == fp) {
        UTIL_LOG_ERR("failed to open %s\n", outPkgFile);
        rc = (-1);
        goto END;
    }

    pkgBytesSize += certSize + 1; /* Adding 1 for "\0"*/
    UTIL_LOG_INFO("about to allocate memory for pkg:size %d, certSize %d, addDataSize %d\n",pkgBytesSize, certSize, addDataSize);

    /* create the package buffer */
    pCertPkg = (uint8_t *) malloc(pkgBytesSize);
    if (pCertPkg == NULL){
        UTIL_LOG_ERR("failed to allocate pkg\n");
        rc = (-1);
        goto END;
    }
    nextBlockOffset = sizeof(CCX509PkgHeader_t);
    /* copy additional data to package */
    if (pAddData != NULL) {
        memcpy(&pCertPkg[nextBlockOffset], pAddData, addDataSize);
        nextBlockOffset += addDataSize;
    }
      /* copy certificate PEM  to package */
    memcpy(&pCertPkg[nextBlockOffset], *ppCertBuff, certSize);

    /* setting pkg header */
    UTIL_LOG_INFO("setting pkg header\n");
    ((CCX509PkgHeader_t *)pCertPkg)->pkgToken = CC_X509_CERT_PKG_TOKEN;
    ((CCX509PkgHeader_t *)pCertPkg)->pkgVer = CC_X509_CERT_PKG_VERSION;
    ((CCX509PkgHeader_t *)pCertPkg)->pkgFlags.pkgFlagsWord = 0;
    ((CCX509PkgHeader_t *)pCertPkg)->pkgFlags.pkgFlagsBits.certType = certType & 0xFF;
    ((CCX509PkgHeader_t *)pCertPkg)->pkgFlags.pkgFlagsBits.imageEnc = encFlag & 0xFF;
    ((CCX509PkgHeader_t *)pCertPkg)->pkgFlags.pkgFlagsBits.hbkType = hbkType & 0xFF;
    ((CCX509PkgHeader_t *)pCertPkg)->certInfo.certInfoWord = 0;
    ((CCX509PkgHeader_t *)pCertPkg)->certInfo.certInfoBits.certOffset = (sizeof(CCX509PkgHeader_t)+addDataSize) & 0xFFFF;
    ((CCX509PkgHeader_t *)pCertPkg)->certInfo.certInfoBits.certSize = certSize;



    /* write out the package in binary format  */
    UTIL_LOG_INFO("writing pkg to file\n");
    rc = CC_CommonUtilCopyBuffToBinFile(outPkgFile, pCertPkg, pkgBytesSize);
    if (rc != 0) {
        UTIL_LOG_ERR("failed to CC_CommonUtilCopyBuffToBinFile\n");
        rc = 1;
        goto END;
    }
    rc = 0;
    UTIL_LOG_INFO("OK\n");

END:
    if (fp != NULL){
        fclose(fp);
    }
    if (pCertPkg != NULL) {
        free(pCertPkg);
    }
    return rc;
}
#endif
