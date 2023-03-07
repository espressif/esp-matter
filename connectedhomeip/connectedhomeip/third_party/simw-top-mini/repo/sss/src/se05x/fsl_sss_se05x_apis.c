/*
 *
 * Copyright 2018-2020 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

/** @file */
#ifdef __cplusplus
extern "C" {
#endif

#include <fsl_sss_se05x_apis.h>
#include <nxLog_sss.h>

#if SSS_HAVE_APPLET_SE05X_IOT
#include <fsl_sss_policy.h>
#include <fsl_sss_se05x_policy.h>
#include <fsl_sss_se05x_scp03.h>
#include <fsl_sss_util_asn1_der.h>
#include <fsl_sss_util_rsa_sign_utils.h>
#include <se05x_const.h>
#include <se05x_ecc_curves.h>
#include <sm_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nxEnsure.h"
#include "nxScp03_Apis.h"
#include "se05x_APDU.h"
#include "se05x_tlv.h"
#include "smCom.h"
#if defined(SMCOM_JRCP_V1_AM)
#include "sm_timer.h"
#endif
#include "se05x_t4t_utils.h"

#if defined(USE_RTOS) && (USE_RTOS == 1)
#define LOCK_TXN(lock)                                   \
    LOG_D("Trying to Acquire Lock");                     \
    if (xSemaphoreTake(lock, portMAX_DELAY) == pdTRUE) { \
        LOG_D("LOCK Acquired");                          \
    }                                                    \
    else {                                               \
        LOG_D("LOCK Acquisition failed");                \
    }
#define UNLOCK_TXN(lock)                  \
    LOG_D("Trying to Released Lock");     \
    if (xSemaphoreGive(lock) == pdTRUE) { \
        LOG_D("LOCK Released");           \
    }                                     \
    else {                                \
        LOG_D("LOCK Releasing failed");   \
    }
#elif (__GNUC__ && !AX_EMBEDDED)
#define LOCK_TXN(lock)                                           \
    LOG_D("Trying to Acquire Lock thread: %ld", pthread_self()); \
    pthread_mutex_lock(&lock);                                   \
    LOG_D("LOCK Acquired by thread: %ld", pthread_self());

#define UNLOCK_TXN(lock)                                             \
    LOG_D("Trying to Released Lock by thread: %ld", pthread_self()); \
    pthread_mutex_unlock(&lock);                                     \
    LOG_D("LOCK Released by thread: %ld", pthread_self());
#endif

#if (__GNUC__ && !AX_EMBEDDED) || (USE_RTOS)
#define USE_LOCK 1
#else
#define USE_LOCK 0
#endif

smStatus_t sss_se05x_create_curve_if_needed(Se05xSession_t *pSession, uint32_t curve_id);
void add_ecc_header(uint8_t *key, size_t *keylen, uint8_t **key_buf, size_t *key_buflen, uint32_t curve_id);

static SE05x_ECSignatureAlgo_t se05x_get_ec_sign_hash_mode(sss_algorithm_t algorithm);

/* Used during testing as well */
void get_ecc_raw_data(uint8_t *key, size_t keylen, uint8_t **key_buf, size_t *key_buflen, uint32_t curve_id);

#if SSSFTR_SE05X_AuthSession
static smStatus_t se05x_CreateVerifyUserIDSession(
    pSe05xSession_t se05xSession, const uint32_t auth_id, SE05x_AuthCtx_ID_t *pin, pSe05xPolicy_t policy);
#endif

#if SSS_HAVE_SCP_SCP03_SSS
#if SSSFTR_SE05X_AuthECKey
static smStatus_t se05x_CreateECKeySession(
    pSe05xSession_t se05xSession, const uint32_t auth_id, SE05x_AuthCtx_ECKey_t *pFScpCtx);
#endif

#if SSSFTR_SE05X_AuthSession
static smStatus_t se05x_CreateVerifyAESKeySession(
    pSe05xSession_t se05xSession, const uint32_t auth_id, NXSCP03_AuthCtx_t *pAppletSCPCtx);
#endif
#endif

static smStatus_t sss_se05x_channel_txnRaw(void *conn_ctx,
    const tlvHeader_t *hdr,
    uint8_t *cmdBuf,
    size_t cmdBufLen,
    uint8_t *rsp,
    size_t *rspLen,
    uint8_t hasle);
#if 0
static SE05x_RSASignatureAlgo_t se05x_get_rsa_sign_mode(
    sss_algorithm_t algorithm);
#endif

#if SSSFTR_SE05X_RSA && SSS_HAVE_RSA
static SE05x_RSAEncryptionAlgo_t se05x_get_rsa_encrypt_mode(sss_algorithm_t algorithm);
static SE05x_RSASignatureAlgo_t se05x_get_rsa_sign_hash_mode(sss_algorithm_t algorithm);
#endif
static SE05x_CipherMode_t se05x_get_cipher_mode(sss_algorithm_t algorithm);
static SE05x_MACAlgo_t se05x_get_mac_algo(sss_algorithm_t algorithm);
#if SSSFTR_SE05X_KEY_SET || SSSFTR_SE05X_KEY_GET
static uint8_t CheckIfKeyIdExists(uint32_t keyId, pSe05xSession_t session_ctx);
#endif
static smStatus_t sss_se05x_channel_txn(void *conn_ctx,
    struct _sss_se05x_tunnel_context *pChannelCtx,
    SE_AuthType_t currAuth,
    const tlvHeader_t *hdr,
    uint8_t *cmdBuf,
    size_t cmdBufLen,
    uint8_t *rsp,
    size_t *rspLen,
    uint8_t hasle);

static smStatus_t sss_se05x_TXn(struct Se05xSession *pSession,
    const tlvHeader_t *hdr,
    uint8_t *cmdBuf,
    size_t cmdBufLen,
    uint8_t *rsp,
    size_t *rspLen,
    uint8_t hasle);

#if SSSFTR_SE05X_AuthECKey || SSSFTR_SE05X_AuthSession
static sss_status_t sss_session_auth_open(sss_se05x_session_t *session,
    sss_type_t subsystem,
    uint32_t auth_id,
    sss_connection_type_t connection_type,
    void *connectionData);
#endif

#if SSSFTR_SE05X_RSA && SSSFTR_SE05X_KEY_SET && SSS_HAVE_RSA
static sss_status_t sss_se05x_key_store_set_rsa_key(sss_se05x_key_store_t *keyStore,
    sss_se05x_object_t *keyObject,
    const uint8_t *key,
    size_t keyLen,
    size_t keyBitLen,
    void *policy_buff,
    size_t policy_buff_len);
#endif

#if SSSFTR_SE05X_ECC || SSSFTR_SE05X_RSA
static sss_status_t se05x_check_input_len(size_t inLen, sss_algorithm_t algorithm);
#endif

#if SSS_HAVE_SE05X_VER_GTE_06_00
static sss_status_t sss_se05x_aead_CCMfinish(sss_se05x_aead_t *context,
    const uint8_t *srcData,
    size_t srcLen,
    uint8_t *destData,
    size_t *destLen,
    uint8_t *tag,
    size_t *tagLen);
#endif

#if SSSFTR_SE05X_ECC && SSSFTR_SE05X_KEY_SET
static smStatus_t sss_se05x_LL_set_ec_key(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    SE05x_MaxAttemps_t maxAttempt,
    uint32_t objectID,
    SE05x_ECCurve_t curveID,
    const uint8_t *privKey,
    size_t privKeyLen,
    const uint8_t *pubKey,
    size_t pubKeyLen,
    const SE05x_INS_t ins_type,
    const SE05x_KeyPart_t key_part,
    SE05x_Result_t obj_exists);

#if SSS_HAVE_SE05X_VER_GTE_06_00
typedef smStatus_t (*fp_Ec_KeyWrite_t)(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    SE05x_MaxAttemps_t maxAttempt,
    uint32_t objectID,
    SE05x_ECCurve_t curveID,
    const uint8_t *privKey,
    size_t privKeyLen,
    const uint8_t *pubKey,
    size_t pubKeyLen,
    const SE05x_INS_t ins_type,
    const SE05x_KeyPart_t key_part,
    uint32_t version);
#endif //SSS_HAVE_SE05X_VER_GTE_06_00
#endif //SSSFTR_SE05X_ECC && SSSFTR_SE05X_KEY_SET

#if SSSFTR_SE05X_KEY_SET
static smStatus_t sss_se05x_LL_set_symm_key(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    SE05x_MaxAttemps_t maxAttempt,
    uint32_t objectID,
    SE05x_KeyID_t kekID,
    const uint8_t *keyValue,
    size_t keyValueLen,
    const SE05x_INS_t ins_type,
    const SE05x_SymmKeyType_t type,
    SE05x_Result_t obj_exists);

#if SSS_HAVE_SE05X_VER_GTE_06_00
typedef smStatus_t (*fp_Symm_KeyWrite_t)(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    SE05x_MaxAttemps_t maxAttempt,
    uint32_t objectID,
    SE05x_KeyID_t kekID,
    const uint8_t *keyValue,
    size_t keyValueLen,
    const SE05x_INS_t ins_type,
    const SE05x_SymmKeyType_t type,
    uint32_t version);
#endif //SSS_HAVE_SE05X_VER_GTE_06_00
#endif //SSSFTR_SE05X_AES && SSSFTR_SE05X_KEY_SET

#if SSSFTR_SE05X_RSA && SSSFTR_SE05X_KEY_SET && SSS_HAVE_RSA
static smStatus_t sss_se05x_LL_set_RSA_key(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    uint32_t objectID,
    uint16_t size,
    const uint8_t *p,
    size_t pLen,
    const uint8_t *q,
    size_t qLen,
    const uint8_t *dp,
    size_t dpLen,
    const uint8_t *dq,
    size_t dqLen,
    const uint8_t *qInv,
    size_t qInvLen,
    const uint8_t *pubExp,
    size_t pubExpLen,
    const uint8_t *priv,
    size_t privLen,
    const uint8_t *pubMod,
    size_t pubModLen,
    const SE05x_INS_t ins_type,
    const SE05x_KeyPart_t key_part,
    const SE05x_RSAKeyFormat_t rsa_format,
    SE05x_Result_t obj_exists);

#if SSS_HAVE_SE05X_VER_GTE_06_00
typedef smStatus_t (*fp_RSA_KeyWrite_t)(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    uint32_t objectID,
    uint16_t size,
    const uint8_t *p,
    size_t pLen,
    const uint8_t *q,
    size_t qLen,
    const uint8_t *dp,
    size_t dpLen,
    const uint8_t *dq,
    size_t dqLen,
    const uint8_t *qInv,
    size_t qInvLen,
    const uint8_t *pubExp,
    size_t pubExpLen,
    const uint8_t *priv,
    size_t privLen,
    const uint8_t *pubMod,
    size_t pubModLen,
    const SE05x_INS_t ins_type,
    const SE05x_KeyPart_t key_part,
    const SE05x_RSAKeyFormat_t rsa_format,
    uint32_t version);
#endif //SSS_HAVE_SE05X_VER_GTE_06_00
#endif //SSSFTR_SE05X_RSA && SSSFTR_SE05X_KEY_SET && SSS_HAVE_RSA

#define SSS_SE05X_RESID_ATTESTATION_KEY 0xF0000012
#if SSS_HAVE_SE05X_VER_GTE_07_02
extern int add_taglength_to_data(
    uint8_t **buf, size_t *bufLen, SE05x_TAG_t tag, const uint8_t *cmd, size_t cmdLen, bool extendedLength);

#if SSS_HAVE_TPM_BN && SSS_HAVE_ECDAA
#define ECDAA_RANDOM_KEY_OBJ_AUTH_ID 0x00000000
#define SSS_SE05X_RESID_ECDAA_RANDOM_KEY_START 0x7DB00000
#define SSS_SE05X_RESID_ECDAA_RANDOM_KEY_NUMBER 0x00010000
#define SSS_SE05X_RESID_ECDAA_RANDOM_KEY_MASK 0xFFFF0000
#define SSS_SE05X_OBJ_ATTR_ORIG_REV_OFFSET 5
#define SSS_SE05X_OBJ_ATTR_TYPE_OFFSET 4

#if SSS_HAVE_SE05X_VER_GTE_07_02
#if SSSFTR_SE05X_ECC
static sss_status_t sss_se05x_generate_ecdaa_random_key(Se05xSession_t *context, uint32_t *random_keyId);
static sss_status_t sss_se05x_get_ecdaa_random_key_id(Se05xSession_t *context, uint32_t *random_keyId);
#endif // SSSFTR_SE05X_ECC
#endif // SSS_HAVE_SE05X_VER_GTE_07_02
#endif // SSS_HAVE_TPM_BN

#endif // SSS_HAVE_SE05X_VER_GTE_07_02

sss_status_t nxECKey_ReadEckaPublicKey(pSe05xSession_t se05xSession,
    sss_key_store_t *pHostKeyStore,
    uint8_t *pSePubEcka,
    size_t *pSePubEckaLen,
    uint32_t eckaObjId);
/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */

/* ************************************************************************** */
/* Functions : sss_se05x_session                                              */
/* ************************************************************************** */

sss_status_t sss_se05x_session_create(sss_se05x_session_t *session,
    sss_type_t subsystem,
    uint32_t application_id,
    sss_connection_type_t connection_type,
    void *connectionData)
{
    AX_UNUSED_ARG(session);
    AX_UNUSED_ARG(subsystem);
    AX_UNUSED_ARG(application_id);
    AX_UNUSED_ARG(connection_type);
    AX_UNUSED_ARG(connectionData);
    sss_status_t retval = kStatus_SSS_Success;
    /* Nothing special to be handled */
    return retval;
}

#define HEX_EXPECTED_APPLET_VERSION \
    (0 | (APPLET_SE050_VER_MAJOR) << (8 * 3) | (APPLET_SE050_VER_MINOR) << (8 * 2) | (APPLET_SE050_VER_DEV) << (8 * 1))

#if defined(APPLET_SE050_VER_DEV_PATCH1)
#define HEX_EXPECTED_APPLET_VERSION_PATCH1                                           \
    (0 | (APPLET_SE050_VER_MAJOR) << (8 * 3) | (APPLET_SE050_VER_MINOR) << (8 * 2) | \
        (APPLET_SE050_VER_DEV_PATCH1) << (8 * 1))
#endif

#define ENABLE_APPLET_VERSION_CHECK 1

sss_status_t sss_se05x_session_open(sss_se05x_session_t *session,
    sss_type_t subsystem,
    uint32_t application_id,
    sss_connection_type_t connection_type,
    void *connectionData)
{
    sss_status_t retval           = kStatus_SSS_InvalidArgument;
    SE05x_Connect_Ctx_t *pAuthCtx = NULL;
    SmCommState_t CommState       = {0};
    smStatus_t status             = SM_NOT_OK;
    int sm_connected              = 0;
    U16 lReturn;
    pSe05xSession_t se05xSession;
#if defined(SMCOM_JRCP_V1_AM)
    int session_open_retry_cnt     = 1;
    int session_open_retry_dly     = 1; //seconds
    int session_open_retry_cnt_max = 50;
    int session_open_retry_dly_max = 10; //seconds
#endif

    ENSURE_OR_RETURN_ON_ERROR(session, kStatus_SSS_Fail);
    se05xSession = &session->s_ctx;

    memset(session, 0, sizeof(*session));

    ENSURE_OR_GO_EXIT(connectionData);
    pAuthCtx = (SE05x_Connect_Ctx_t *)connectionData;

    ENSURE_OR_RETURN_ON_ERROR(se05x_enable_contact_interface() == 0, kStatus_SSS_Fail);

    if (pAuthCtx->connType != kType_SE_Conn_Type_Channel) {
        uint8_t atr[100];
        uint16_t atrLen    = ARRAY_SIZE(atr);
        CommState.connType = pAuthCtx->connType;
        if (1 == pAuthCtx->skip_select_applet) {
            if (pAuthCtx->auth.authType == kSSS_AuthType_None) {
                CommState.select = SELECT_NONE;
            }
            else if (pAuthCtx->auth.authType == kSSS_AuthType_SCP03) {
                CommState.select = SELECT_SSD;
            }
        }
        if (1 == pAuthCtx->sessionResume) {
            CommState.sessionResume = 1;
        }
#if defined(SMCOM_JRCP_V1) || defined(SMCOM_JRCP_V2) || defined(RJCT_VCOM) || defined(SMCOM_PCSC) || \
    defined(SMCOM_RC663_VCOM)
        lReturn = SM_RjctConnect(&(se05xSession->conn_ctx), pAuthCtx->portName, &CommState, atr, &atrLen);

        if (lReturn != SW_OK) {
            LOG_E("SM_RjctConnect Failed. Status %04X", lReturn);
            retval = kStatus_SSS_Fail;
            goto exit;
        }
        if (atrLen != 0) {
            LOG_AU8_I(atr, atrLen);
        }
#elif defined(SMCOM_PN7150)
        lReturn = SM_Connect(&(se05xSession->conn_ctx), &CommState, atr, &atrLen);
        if (lReturn != SW_OK) {
            LOG_E("SM_Connect Failed for NFC Interface. Status %04X", lReturn);
            retval = kStatus_SSS_Fail;
            goto exit;
        }
        if (atrLen != 0) {
            LOG_AU8_I(atr, atrLen);
        }
#else
        /* AX_EMBEDDED Or Native */
        lReturn = SM_I2CConnect(&(se05xSession->conn_ctx), &CommState, atr, &atrLen, pAuthCtx->portName);
        if (lReturn != SW_OK) {
            LOG_E("SM_I2CConnect Failed. Status %04X", lReturn);
            retval = kStatus_SSS_Fail;
            goto exit;
        }
        if (atrLen != 0) {
            LOG_AU8_I(atr, atrLen);
        }
#endif
        sm_connected = 1;

        if (1 == pAuthCtx->skip_select_applet) {
            status = (smStatus_t)lReturn;
            /* Not selecting the applet, so we don't know whether it's old or new */
        }
        else {
#if ENABLE_APPLET_VERSION_CHECK
            if (HEX_EXPECTED_APPLET_VERSION == (0xFFFFFF00 & CommState.appletVersion)) {
                /* Fine */
            }
#if defined(HEX_EXPECTED_APPLET_VERSION_PATCH1)
            else if (HEX_EXPECTED_APPLET_VERSION_PATCH1 == (0xFFFFFF00 & CommState.appletVersion)) {
                /* Fine */
            }
#endif
            else if ((0xFFFFFF00 & CommState.appletVersion) < HEX_EXPECTED_APPLET_VERSION) {
                LOG_E("Mismatch Applet version.");
                LOG_E("Compiled for 0x%X. Got older 0x%X",
                    (HEX_EXPECTED_APPLET_VERSION) >> 8,
                    (CommState.appletVersion) >> 8);
                LOG_E("Aborting!!!");
                SM_Close(se05xSession->conn_ctx, 0);
                retval = kStatus_SSS_Fail;
                goto exit;
            }
            else {
                LOG_I("Newer version of Applet Found");
                LOG_I("Compiled for 0x%X. Got newer 0x%X",
                    (HEX_EXPECTED_APPLET_VERSION) >> 8,
                    (CommState.appletVersion) >> 8);
            }
#else
            LOG_I("Compiled for 0x%X. Connected applet Ver 0x%X",
                (HEX_EXPECTED_APPLET_VERSION) >> 8,
                (CommState.appletVersion) >> 8);
#endif
        }
    }

    if (pAuthCtx->auth.authType == kSSS_AuthType_ECKey) {
        ENSURE_OR_GO_EXIT(pAuthCtx->auth.ctx.eckey.pDyn_ctx);
        if (CommState.appletVersion == 0) {
            /*Get Applet version from previously opened session*/
            uint8_t appletVersion[32]          = {0};
            uint8_t versionIterator            = 0;
            size_t appletVersionLen            = sizeof(appletVersion);
            sss_se05x_session_t *se05x_session = (sss_se05x_session_t *)pAuthCtx->tunnelCtx->session;
            status = Se05x_API_GetVersion(&se05x_session->s_ctx, appletVersion, &appletVersionLen);
            if (status != SM_OK) {
                LOG_E("Unable to retrive applet version");
                retval = kStatus_SSS_Fail;
                goto exit;
            }
            for (versionIterator = 0; versionIterator < 3; versionIterator++) {
                CommState.appletVersion = CommState.appletVersion << 8 | appletVersion[versionIterator];
            }
            CommState.appletVersion = CommState.appletVersion << 8;
        }
        if (CommState.appletVersion >= 0x03050000) {
            pAuthCtx->auth.ctx.eckey.pDyn_ctx->authType = kSSS_AuthType_INT_ECKey_Counter;
        }
        else {
            pAuthCtx->auth.ctx.eckey.pDyn_ctx->authType = kSSS_AuthType_ECKey;
        }
    }

    se05xSession->fp_TXn    = &sss_se05x_TXn;
    se05xSession->fp_RawTXn = &sss_se05x_channel_txn;

    /* Auth type is None */
    if (1 == pAuthCtx->skip_select_applet) {
        /* Not selecting the applet */
    }
    else {
        if ((pAuthCtx->auth.authType == kSSS_AuthType_None) && (connection_type == kSSS_ConnectionType_Plain)) {
            LOG_W("Communication channel is Plain.");
            LOG_W("!!!Not recommended for production use.!!!");
            se05xSession->fp_Transform = &se05x_Transform;
            se05xSession->fp_DeCrypt   = &se05x_DeCrypt;
            se05xSession->authType     = kSSS_AuthType_None;
            status                     = SM_OK;
        }
    }

#if SSS_HAVE_SCP_SCP03_SSS
    /* Auth type is Platform SCP03 */
    if ((pAuthCtx->auth.authType == kSSS_AuthType_SCP03) && (connection_type == kSSS_ConnectionType_Encrypted)) {
        se05xSession->fp_Transform = &se05x_Transform;
        se05xSession->fp_DeCrypt   = &se05x_DeCrypt;
        se05xSession->authType     = kSSS_AuthType_SCP03;
        status                     = SM_NOT_OK;
        retval                     = nxScp03_AuthenticateChannel(se05xSession, &pAuthCtx->auth.ctx.scp03);
        if (retval == kStatus_SSS_Success) {
            /* There is a differnet behaviour of Platform SCP between SE050 and future applet.
             * Here we switch make it clear. */
            if (CommState.appletVersion >= 0x04030000) {
                pAuthCtx->auth.ctx.scp03.pDyn_ctx->authType = (SE_AuthType_t)kSSS_AuthType_AESKey;
            }
            else {
                pAuthCtx->auth.ctx.scp03.pDyn_ctx->authType = (SE_AuthType_t)kSSS_AuthType_SCP03;
            }
            /*Auth type to Platform SCP03 again as channel authentication will modify it
            to auth type None*/
            se05xSession->authType     = kSSS_AuthType_SCP03;
            se05xSession->pdynScp03Ctx = pAuthCtx->auth.ctx.scp03.pDyn_ctx;
            status                     = SM_OK;
            se05xSession->fp_Transform = &se05x_Transform_scp;
        }
        else {
            LOG_E("Could not set SCP03 Secure Channel");
        }
    }
#else
    if (pAuthCtx->auth.authType != kSSS_AuthType_None && pAuthCtx->auth.authType != kSSS_AuthType_ID) {
        LOG_E(
            "Set the SCP to SCP03_SSS in the build configuration and "
            "recompile.!");
    }

#endif

#if SSSFTR_SE05X_AuthECKey || SSSFTR_SE05X_AuthSession
    if (pAuthCtx->connType == kType_SE_Conn_Type_Channel) {
        se05xSession->pChannelCtx = (struct _sss_se05x_tunnel_context *)pAuthCtx->tunnelCtx;
    }

    if ((application_id != 0) &&
        ((connection_type == kSSS_ConnectionType_Password) || (connection_type == kSSS_ConnectionType_Encrypted))) {
#if defined(SMCOM_JRCP_V1_AM)
        {
            // Overwrite session_open_retry_cnt and session_open_retry_dly from env variables
            const char *retry_cnt = NULL;
            const char *retry_dly = NULL;

            retry_cnt = getenv("EX_SSS_SESSION_OPEN_RETRY_CNT");
            if (retry_cnt != NULL) {
                session_open_retry_cnt = atoi(retry_cnt);
                if (session_open_retry_cnt > session_open_retry_cnt_max) {
                    session_open_retry_cnt = session_open_retry_cnt_max;
                }
                LOG_I("Session Open Retry Count ='%d' ", session_open_retry_cnt);
            }

            retry_dly = getenv("EX_SSS_SESSION_OPEN_RETRY_DLY");
            if (retry_dly != NULL) {
                session_open_retry_dly = atoi(retry_dly);
                if (session_open_retry_dly < 1) {
                    session_open_retry_dly = 1;
                }
                if (session_open_retry_dly > session_open_retry_dly_max) {
                    session_open_retry_dly = session_open_retry_dly_max;
                }
                LOG_I("Session Open Retry Delay ='%d' ", session_open_retry_dly);
            }
        }

        do {
            if (session_open_retry_cnt > 0) {
                session_open_retry_cnt--;
            }
            SM_LOCK_CHANNEL();
            retval = sss_session_auth_open(session, subsystem, application_id, connection_type, connectionData);
            SM_UNLOCK_CHANNEL();
            if (retval == kStatus_SSS_Success) {
                break;
            }

            sm_sleep(session_open_retry_dly * 1000);

        } while (session_open_retry_cnt > 0);
#else
        SM_LOCK_CHANNEL();
        retval = sss_session_auth_open(session, subsystem, application_id, connection_type, connectionData);
        SM_UNLOCK_CHANNEL();
#endif

        if (retval == kStatus_SSS_Success) {
            status = SM_OK;
        }
        else {
            status = SM_NOT_OK;
        }
    }
#endif

    if (status == SM_OK) {
        session->subsystem = subsystem;
        retval             = kStatus_SSS_Success;
    }
    else {
        retval = kStatus_SSS_Fail;
    }
exit:
    if (retval != kStatus_SSS_Success) {
        if ((sm_connected) && (pAuthCtx->connType != kType_SE_Conn_Type_Channel)) {
            SM_Close(se05xSession->conn_ctx, 0);
        }

        memset(session, 0x00, sizeof(*session));
    }

    return retval;
}

#if SSSFTR_SE05X_AuthECKey || SSSFTR_SE05X_AuthSession
static sss_status_t sss_session_auth_open(sss_se05x_session_t *session,
    sss_type_t subsystem,
    uint32_t auth_id,
    sss_connection_type_t connect_type,
    void *connectionData)
{
    sss_status_t retval = kStatus_SSS_Fail;
    void *conn_ctx      = session->s_ctx.conn_ctx;
    memset(session, 0, sizeof(*session));
    SE05x_Connect_Ctx_t *pAuthCtx;
    smStatus_t status = SM_NOT_OK;
#if SSSFTR_SE05X_AuthSession
    Se05xPolicy_t se05x_policy;
    uint8_t *ppolicySet;
    uint8_t session_policies_buff[MAX_POLICY_BUFFER_SIZE];
    size_t valid_policy_buff_len = 0;
#endif
    retval                       = kStatus_SSS_Fail;
    pSe05xSession_t se05xSession = &session->s_ctx;

    /* Restore connection context */
    se05xSession->conn_ctx = conn_ctx;

    ENSURE_OR_GO_EXIT(connectionData != NULL);
    pAuthCtx = (SE05x_Connect_Ctx_t *)connectionData;

    if ((pAuthCtx->auth.authType == kSSS_AuthType_ID) && (connect_type != kSSS_ConnectionType_Password)) {
        LOG_D("ERROR: Need both AUTHType=ID and ConnType=Password");
        goto exit;
    }
    if (((pAuthCtx->auth.authType == kSSS_AuthType_AESKey) || (pAuthCtx->auth.authType == kSSS_AuthType_ECKey)) &&
        (connect_type != kSSS_ConnectionType_Encrypted)) {
        LOG_D("ERROR: Need both AUTHType={AESKey||ECKey} and ConnType=Encrypted");
        goto exit;
    }

    se05xSession->fp_TXn    = &sss_se05x_TXn;
    se05xSession->fp_RawTXn = &sss_se05x_channel_txn;
    if (pAuthCtx->connType == kType_SE_Conn_Type_Channel) {
        se05xSession->pChannelCtx = (struct _sss_se05x_tunnel_context *)pAuthCtx->tunnelCtx;
    }

#if SSSFTR_SE05X_AuthSession
    /*Session Policy check*/
    if (pAuthCtx->session_policy) {
        if (kStatus_SSS_Success != sss_se05x_create_session_policy_buffer(
                                       pAuthCtx->session_policy, &session_policies_buff[0], &valid_policy_buff_len)) {
            goto exit;
        }
        ppolicySet = session_policies_buff;
    }
    else {
        ppolicySet = NULL;
    }

    se05x_policy.value     = (uint8_t *)ppolicySet;
    se05x_policy.value_len = valid_policy_buff_len;
#endif

    /* Auth type is Platform UserID */

    if (pAuthCtx->auth.authType == kSSS_AuthType_ID) {
#if SSSFTR_SE05X_AuthSession
        LOG_W("Communication channel is with UserID (But Plain).");
        LOG_W("!!!Not recommended for production use.!!!");
        se05xSession->fp_Transform = &se05x_Transform;
        se05xSession->fp_DeCrypt   = &se05x_DeCrypt;

        status = se05x_CreateVerifyUserIDSession(se05xSession, auth_id, &pAuthCtx->auth.ctx.idobj, &se05x_policy);
#else
        LOG_W("User Id Support compiled out");
        status = SM_NOT_OK;
#endif
    }

#if SSS_HAVE_SCP_SCP03_SSS
    /* Auth type is ECKey */
    else if ((pAuthCtx->auth.authType == kSSS_AuthType_ECKey) && (auth_id != 0)) {
#if SSSFTR_SE05X_AuthECKey && SSSFTR_SE05X_AuthSession
        se05xSession->fp_Transform = &se05x_Transform;
        se05xSession->fp_DeCrypt   = &se05x_DeCrypt;
        status                     = se05x_CreateECKeySession(se05xSession, auth_id, &pAuthCtx->auth.ctx.eckey);
        if (status == SM_OK) {
            se05xSession->fp_Transform = &se05x_Transform_scp;
            if (se05x_policy.value_len > 0) {
                status = Se05x_API_ExchangeSessionData(se05xSession, &se05x_policy);
            }
        }
#else
        LOG_W("ECKey Support compiled out");
        status = SM_NOT_OK;
#endif
    }
    /* Auth type is Applet SCP03 */
    else if ((pAuthCtx->auth.authType == kSSS_AuthType_AESKey) && (auth_id != 0)) {
#if SSSFTR_SE05X_AuthSession
        se05xSession->fp_Transform = &se05x_Transform;
        se05xSession->fp_DeCrypt   = &se05x_DeCrypt;
        status                     = se05x_CreateVerifyAESKeySession(se05xSession, auth_id, &pAuthCtx->auth.ctx.scp03);
        if (status == SM_OK) {
            se05xSession->fp_Transform = &se05x_Transform_scp;
            if (se05x_policy.value_len > 0) {
                status = SM_NOT_OK;
                status = Se05x_API_ExchangeSessionData(se05xSession, &se05x_policy);
            }
        }
#else
        LOG_W("AppletSCP Support compiled out");
        status = SM_NOT_OK;
#endif
    }

#endif

    if (status == SM_OK) {
        session->subsystem = subsystem;
        retval             = kStatus_SSS_Success;
    }
    else {
        memset(session, 0x00, sizeof(*session));
        retval = kStatus_SSS_Fail;
    }
    /* Restore connection context */
    session->s_ctx.conn_ctx = conn_ctx;

exit:
    return retval;
}

#endif

sss_status_t sss_se05x_refresh_session(sss_se05x_session_t *session, void *connectionData)
{
    sss_status_t retval                  = kStatus_SSS_Fail;
    pSe05xSession_t se05xSession         = &session->s_ctx;
    sss_policy_session_u *session_policy = (sss_policy_session_u *)connectionData;
    smStatus_t status                    = SM_NOT_OK;
    size_t valid_policy_buff_len         = 0;
    Se05xPolicy_t se05x_policy           = {0};
    uint8_t *ppolicySet;
    uint8_t session_policies_buff[MAX_POLICY_BUFFER_SIZE];

    if (session_policy) {
        if (kStatus_SSS_Success !=
            sss_se05x_create_session_policy_buffer(session_policy, &session_policies_buff[0], &valid_policy_buff_len)) {
            goto exit;
        }
        ppolicySet             = session_policies_buff;
        se05x_policy.value     = (uint8_t *)ppolicySet;
        se05x_policy.value_len = valid_policy_buff_len;
    }
    else {
        ppolicySet             = NULL;
        se05x_policy.value     = NULL;
        se05x_policy.value_len = 0;
    }

    status = Se05x_API_RefreshSession(se05xSession, &se05x_policy);
    if (status == SM_OK) {
        retval = kStatus_SSS_Success;
    }
exit:
    return retval;
}

sss_status_t sss_se05x_session_prop_get_u32(sss_se05x_session_t *session, uint32_t property, uint32_t *pValue)
{
    AX_UNUSED_ARG(session);
    sss_status_t retval                   = kStatus_SSS_Success;
    sss_session_prop_u32_t prop           = (sss_session_prop_u32_t)property;
    sss_s05x_sesion_prop_u32_t se050xprop = (sss_s05x_sesion_prop_u32_t)property;

    if (pValue == NULL) {
        retval = kStatus_SSS_Fail;
        goto cleanup;
    }

    switch (prop) {
    case kSSS_SessionProp_VerMaj:
        *pValue = PLUGANDTRUST_HOSTLIB_VER_MAJOR;
        break;
    case kSSS_SessionProp_VerMin:
        *pValue = PLUGANDTRUST_HOSTLIB_VER_MINOR;
        break;
    case kSSS_SessionProp_VerDev:
        *pValue = PLUGANDTRUST_HOSTLIB_VER_DEV;
        break;
    case kSSS_SessionProp_UIDLen:
        *pValue = 18;
        break;
    default:
        *pValue = 0;
        retval  = kStatus_SSS_Fail;
    }

    if (retval == kStatus_SSS_Success) {
        goto cleanup;
    }

    switch (se050xprop) {
    case kSSS_SE05x_SessionProp_CertUIDLen: {
        *pValue = 10;
        retval  = kStatus_SSS_Success;
    } break;
    default: {
        *pValue = 0;
        retval  = kStatus_SSS_Fail;
    }
    }

cleanup:
    return retval;
}

sss_status_t sss_se05x_session_prop_get_au8(
    sss_se05x_session_t *session, uint32_t property, uint8_t *pValue, size_t *pValueLen)
{
    sss_status_t retval                   = kStatus_SSS_Fail;
    sss_session_prop_au8_t prop           = (sss_session_prop_au8_t)property;
    sss_s05x_sesion_prop_au8_t se050xprop = (sss_s05x_sesion_prop_au8_t)property;
    smStatus_t sm_status                  = SM_NOT_OK;

    if (pValue == NULL || pValueLen == NULL) {
        goto cleanup;
    }

    switch (prop) {
    case kSSS_SessionProp_UID:
        if (*pValueLen >= 18) {
            sm_status = Se05x_API_ReadObject(&session->s_ctx, kSE05x_AppletResID_UNIQUE_ID, 0, 18, pValue, pValueLen);
        }
        else {
            LOG_D("Buffer too short");
        }
        break;
    default:;
    }

    if (sm_status == SM_OK) {
        goto cleanup;
    }

    switch (se050xprop) {
    case kSSS_SE05x_SessionProp_CertUID:
        if (*pValueLen >= 10) {
            uint8_t uid18[SE050_MODULE_UNIQUE_ID_LEN];
            size_t uid18Len = sizeof(uid18);

            sm_status = Se05x_API_ReadObject(
                &session->s_ctx, kSE05x_AppletResID_UNIQUE_ID, 0, (uint16_t)uid18Len, uid18, &uid18Len);
            if (sm_status == SM_OK) {
                int idx = 0;
#define A71CH_UID_IC_TYPE_OFFSET 2
#define A71CH_UID_IC_FABRICATION_DATA_OFFSET 8
#define A71CH_UID_IC_SERIAL_NR_OFFSET 10
#define A71CH_UID_IC_BATCH_ID_OFFSET 13
                pValue[idx++] = uid18[A71CH_UID_IC_TYPE_OFFSET];
                pValue[idx++] = uid18[A71CH_UID_IC_TYPE_OFFSET + 1];
                pValue[idx++] = uid18[A71CH_UID_IC_FABRICATION_DATA_OFFSET];
                pValue[idx++] = uid18[A71CH_UID_IC_FABRICATION_DATA_OFFSET + 1];
                pValue[idx++] = uid18[A71CH_UID_IC_SERIAL_NR_OFFSET];
                pValue[idx++] = uid18[A71CH_UID_IC_SERIAL_NR_OFFSET + 1];
                pValue[idx++] = uid18[A71CH_UID_IC_SERIAL_NR_OFFSET + 2];
                pValue[idx++] = uid18[A71CH_UID_IC_BATCH_ID_OFFSET];
                pValue[idx++] = uid18[A71CH_UID_IC_BATCH_ID_OFFSET + 1];
                pValue[idx++] = uid18[A71CH_UID_IC_BATCH_ID_OFFSET + 2];
                *pValueLen    = 10;
            }
        }
        break;
    default:
        LOG_E("Invalid property");
    }

cleanup:
    if (sm_status == SM_OK) {
        retval = kStatus_SSS_Success;
    }
    return retval;
}

void sss_se05x_session_close(sss_se05x_session_t *session)
{
    Se05x_API_CloseSession(&session->s_ctx);
    if (session->s_ctx.pChannelCtx == NULL) {
        SM_Close(session->s_ctx.conn_ctx, 0);
    }
    memset(session, 0, sizeof(*session));
}

void sss_se05x_session_delete(sss_se05x_session_t *session)
{
    AX_UNUSED_ARG(session);
}

/* End: se05x_session */

/* ************************************************************************** */
/* Functions : sss_se05x_keyobj                                               */
/* ************************************************************************** */

sss_status_t sss_se05x_key_object_init(sss_se05x_object_t *keyObject, sss_se05x_key_store_t *keyStore)
{
    sss_status_t retval = kStatus_SSS_Success;
    memset(keyObject, 0, sizeof(*keyObject));
    keyObject->keyStore = keyStore;

    return retval;
}

sss_status_t sss_se05x_key_object_allocate_handle(sss_se05x_object_t *keyObject,
    uint32_t keyId,
    sss_key_part_t keyPart,
    sss_cipher_type_t cipherType,
    size_t keyByteLenMax,
    uint32_t options)
{
    AX_UNUSED_ARG(keyByteLenMax);
    sss_status_t retval = kStatus_SSS_Success;
    smStatus_t status;
    SE05x_Result_t exists = kSE05x_Result_NA;
    keyObject->objectType = keyPart;
    keyObject->cipherType = cipherType;
    keyObject->keyId      = keyId;
    if (options == kKeyObject_Mode_Persistent) {
        keyObject->isPersistant = 1;
    }

    status = Se05x_API_CheckObjectExists(&keyObject->keyStore->session->s_ctx, keyId, &exists);
    if (status == SM_OK) {
        if (exists == kSE05x_Result_SUCCESS) {
            LOG_W("Object id 0x%X exists", keyId);
        }
    }
    else {
        LOG_E("Couldn't check if object id 0x%X exists", keyId);
        return kStatus_SSS_Fail;
    }

    return retval;
}

//static sss_status_t sss_se05x_key_object_get_handle_binary(
//    sss_se05x_object_t *keyObject) {
//    sss_status_t retval = kStatus_SSS_Success;
//    keyObject->objectType = kSSS_KeyPart_Default;
//    keyObject->cipherType = kSSS_CipherType_Binary;
//    return retval;
//}
sss_status_t sss_se05x_key_object_get_handle(sss_se05x_object_t *keyObject, uint32_t keyId)
{
    sss_status_t retval = kStatus_SSS_Fail;
#if SSSFTR_SE05X_KEY_GET
    SE05x_SecObjTyp_t retObjectType;
    uint8_t retTransientType;
    SE05x_ECCurve_t retCurveId;
    const SE05x_AttestationType_t attestationType = kSE05x_AttestationType_None;
    smStatus_t apiRetval;

    if (0 == CheckIfKeyIdExists(keyId, &keyObject->keyStore->session->s_ctx)) {
        /* Object does not exist  */
        LOG_D("keyId does not exist");
        LOG_U32_D(keyId);
        return retval;
    }

    keyObject->keyId = keyId;

    apiRetval = Se05x_API_ReadType(
        &keyObject->keyStore->session->s_ctx, keyId, &retObjectType, &retTransientType, attestationType);
    if (apiRetval == SM_OK) {
        keyObject->isPersistant = retTransientType;
#if SSS_HAVE_SE05X_VER_GTE_07_02
        if (retObjectType >= kSE05x_SecObjTyp_EC_KEY_PAIR_NIST_P192 &&
            retObjectType <= kSE05x_SecObjTyp_EC_PUB_KEY_MONT_DH_448)
#else
        if (retObjectType >= kSE05x_SecObjTyp_EC_KEY_PAIR && retObjectType <= kSE05x_SecObjTyp_EC_PUB_KEY)
#endif
        {
            apiRetval = Se05x_API_EC_CurveGetId(&keyObject->keyStore->session->s_ctx, keyId, &retCurveId);
            if (apiRetval == SM_OK) {
                keyObject->curve_id = retCurveId;
                if ((retCurveId == kSE05x_ECCurve_NIST_P256)
#if SSS_HAVE_EC_NIST_192
                    || (retCurveId == kSE05x_ECCurve_NIST_P192)
#endif
#if SSS_HAVE_EC_NIST_224
                    || (retCurveId == kSE05x_ECCurve_NIST_P224)
#endif
#if SSS_HAVE_EC_NIST_521
                    || (retCurveId == kSE05x_ECCurve_NIST_P521)
#endif
                    || (retCurveId == kSE05x_ECCurve_NIST_P384)) {
                    keyObject->cipherType = kSSS_CipherType_EC_NIST_P;
                }
#if SSS_HAVE_EC_BP
                else if ((retCurveId >= kSE05x_ECCurve_Brainpool160) && (retCurveId <= kSE05x_ECCurve_Brainpool512)) {
                    keyObject->cipherType = kSSS_CipherType_EC_BRAINPOOL;
                }
#endif
#if SSS_HAVE_EC_NIST_K
                else if ((retCurveId >= kSE05x_ECCurve_Secp160k1) && (retCurveId <= kSE05x_ECCurve_Secp256k1)) {
                    keyObject->cipherType = kSSS_CipherType_EC_NIST_K;
                }
#endif
#if SSS_HAVE_EC_ED
                else if (retCurveId == kSE05x_ECCurve_RESERVED_ID_ECC_ED_25519) {
                    keyObject->cipherType = kSSS_CipherType_EC_TWISTED_ED;
                }
#endif
#if SSS_HAVE_EC_MONT
                else if (retCurveId == kSE05x_ECCurve_RESERVED_ID_ECC_MONT_DH_25519) {
                    keyObject->cipherType = kSSS_CipherType_EC_MONTGOMERY;
                }
#endif
#if SSS_HAVE_TPM_BN
                else if (retCurveId == kSE05x_ECCurve_TPM_ECC_BN_P256) {
                    keyObject->cipherType = kSSS_CipherType_EC_BARRETO_NAEHRIG;
                }
#endif
#if SSS_HAVE_SE05X_VER_GTE_06_00 && SSS_HAVE_EC_MONT
                else if (retCurveId == kSE05x_ECCurve_RESERVED_ID_ECC_MONT_DH_448) {
                    keyObject->cipherType = kSSS_CipherType_EC_MONTGOMERY;
                }
#endif
                else {
                    keyObject->cipherType = kSSS_CipherType_NONE;
                    return retval;
                }
            }
            else {
                LOG_E("error in Se05x_API_GetECCurveId");
                return retval;
            }
        }
#if SSSFTR_RSA && SSS_HAVE_RSA
        else if (retObjectType == kSE05x_SecObjTyp_RSA_KEY_PAIR_CRT) {
            keyObject->cipherType = kSSS_CipherType_RSA_CRT;
        }
        else if (retObjectType == kSE05x_SecObjTyp_RSA_PRIV_KEY_CRT) {
            keyObject->cipherType = kSSS_CipherType_RSA_CRT;
        }
        else if (retObjectType >= kSE05x_SecObjTyp_RSA_KEY_PAIR && retObjectType <= kSE05x_SecObjTyp_RSA_PUB_KEY) {
            keyObject->cipherType = kSSS_CipherType_RSA;
        }
#endif
        else if (retObjectType == kSE05x_SecObjTyp_AES_KEY) {
            keyObject->cipherType = kSSS_CipherType_AES;
        }
        else if (retObjectType == kSE05x_SecObjTyp_DES_KEY) {
            keyObject->cipherType = kSSS_CipherType_DES;
        }
        else if (retObjectType == kSE05x_SecObjTyp_BINARY_FILE) {
            keyObject->cipherType = kSSS_CipherType_Binary;
        }
        else if (retObjectType == kSE05x_SecObjTyp_UserID) {
            keyObject->cipherType = kSSS_CipherType_UserID;
        }
        else if (retObjectType == kSE05x_SecObjTyp_COUNTER) {
            keyObject->cipherType = kSSS_CipherType_Count;
        }
        else if (retObjectType == kSE05x_SecObjTyp_PCR) {
            keyObject->cipherType = kSSS_CipherType_PCR;
        }
        else if (retObjectType == kSE05x_SecObjTyp_HMAC_KEY) {
            keyObject->cipherType = kSSS_CipherType_HMAC;
        }
        else {
            keyObject->cipherType = kSSS_CipherType_NONE;
        }

        switch (retObjectType) {
        case kSE05x_SecObjTyp_EC_KEY_PAIR:
#if SSS_HAVE_RSA
        case kSE05x_SecObjTyp_RSA_KEY_PAIR:
        case kSE05x_SecObjTyp_RSA_KEY_PAIR_CRT:
#endif
#if SSS_HAVE_SE05X_VER_GTE_07_02
        case kSE05x_SecObjTyp_EC_KEY_PAIR_NIST_P192:
        case kSE05x_SecObjTyp_EC_KEY_PAIR_NIST_P224:
        case kSE05x_SecObjTyp_EC_KEY_PAIR_NIST_P256:
        case kSE05x_SecObjTyp_EC_KEY_PAIR_NIST_P384:
        case kSE05x_SecObjTyp_EC_KEY_PAIR_NIST_P521:
        case kSE05x_SecObjTyp_EC_KEY_PAIR_Brainpool160:
        case kSE05x_SecObjTyp_EC_KEY_PAIR_Brainpool192:
        case kSE05x_SecObjTyp_EC_KEY_PAIR_Brainpool224:
        case kSE05x_SecObjTyp_EC_KEY_PAIR_Brainpool256:
        case kSE05x_SecObjTyp_EC_KEY_PAIR_Brainpool320:
        case kSE05x_SecObjTyp_EC_KEY_PAIR_Brainpool384:
        case kSE05x_SecObjTyp_EC_KEY_PAIR_Brainpool512:
        case kSE05x_SecObjTyp_EC_KEY_PAIR_Secp160k1:
        case kSE05x_SecObjTyp_EC_KEY_PAIR_Secp192k1:
        case kSE05x_SecObjTyp_EC_KEY_PAIR_Secp224k1:
        case kSE05x_SecObjTyp_EC_KEY_PAIR_Secp256k1:
        case kSE05x_SecObjTyp_EC_KEY_PAIR_BN_P256:
        case kSE05x_SecObjTyp_EC_KEY_PAIR_ED25519:
        case kSE05x_SecObjTyp_EC_KEY_PAIR_MONT_DH_25519:
        case kSE05x_SecObjTyp_EC_KEY_PAIR_MONT_DH_448:
#endif
            keyObject->objectType = kSSS_KeyPart_Pair;
            break;

        case kSE05x_SecObjTyp_EC_PUB_KEY:
        case kSE05x_SecObjTyp_RSA_PUB_KEY:
#if SSS_HAVE_SE05X_VER_GTE_07_02
        case kSE05x_SecObjTyp_EC_PUB_KEY_NIST_P192:
        case kSE05x_SecObjTyp_EC_PUB_KEY_NIST_P224:
        case kSE05x_SecObjTyp_EC_PUB_KEY_NIST_P256:
        case kSE05x_SecObjTyp_EC_PUB_KEY_NIST_P384:
        case kSE05x_SecObjTyp_EC_PUB_KEY_NIST_P521:
        case kSE05x_SecObjTyp_EC_PUB_KEY_Brainpool160:
        case kSE05x_SecObjTyp_EC_PUB_KEY_Brainpool192:
        case kSE05x_SecObjTyp_EC_PUB_KEY_Brainpool224:
        case kSE05x_SecObjTyp_EC_PUB_KEY_Brainpool256:
        case kSE05x_SecObjTyp_EC_PUB_KEY_Brainpool320:
        case kSE05x_SecObjTyp_EC_PUB_KEY_Brainpool384:
        case kSE05x_SecObjTyp_EC_PUB_KEY_Brainpool512:
        case kSE05x_SecObjTyp_EC_PUB_KEY_Secp160k1:
        case kSE05x_SecObjTyp_EC_PUB_KEY_Secp192k1:
        case kSE05x_SecObjTyp_EC_PUB_KEY_Secp224k1:
        case kSE05x_SecObjTyp_EC_PUB_KEY_Secp256k1:
        case kSE05x_SecObjTyp_EC_PUB_KEY_BN_P256:
        case kSE05x_SecObjTyp_EC_PUB_KEY_ED25519:
        case kSE05x_SecObjTyp_EC_PUB_KEY_MONT_DH_25519:
        case kSE05x_SecObjTyp_EC_PUB_KEY_MONT_DH_448:
#endif
            keyObject->objectType = kSSS_KeyPart_Public;
            break;

#if SSS_HAVE_SE05X_VER_GTE_07_02
        case kSE05x_SecObjTyp_EC_PRIV_KEY_NIST_P192:
        case kSE05x_SecObjTyp_EC_PRIV_KEY_NIST_P224:
        case kSE05x_SecObjTyp_EC_PRIV_KEY_NIST_P256:
        case kSE05x_SecObjTyp_EC_PRIV_KEY_NIST_P384:
        case kSE05x_SecObjTyp_EC_PRIV_KEY_NIST_P521:
        case kSE05x_SecObjTyp_EC_PRIV_KEY_Brainpool160:
        case kSE05x_SecObjTyp_EC_PRIV_KEY_Brainpool192:
        case kSE05x_SecObjTyp_EC_PRIV_KEY_Brainpool224:
        case kSE05x_SecObjTyp_EC_PRIV_KEY_Brainpool256:
        case kSE05x_SecObjTyp_EC_PRIV_KEY_Brainpool320:
        case kSE05x_SecObjTyp_EC_PRIV_KEY_Brainpool384:
        case kSE05x_SecObjTyp_EC_PRIV_KEY_Brainpool512:
        case kSE05x_SecObjTyp_EC_PRIV_KEY_Secp160k1:
        case kSE05x_SecObjTyp_EC_PRIV_KEY_Secp192k1:
        case kSE05x_SecObjTyp_EC_PRIV_KEY_Secp224k1:
        case kSE05x_SecObjTyp_EC_PRIV_KEY_Secp256k1:
        case kSE05x_SecObjTyp_EC_PRIV_KEY_BN_P256:
        case kSE05x_SecObjTyp_EC_PRIV_KEY_ED25519:
        case kSE05x_SecObjTyp_EC_PRIV_KEY_MONT_DH_25519:
        case kSE05x_SecObjTyp_EC_PRIV_KEY_MONT_DH_448:
            keyObject->objectType = kSSS_KeyPart_Private;
            break;
#endif

        case kSE05x_SecObjTyp_BINARY_FILE:
        case kSE05x_SecObjTyp_PCR:
        case kSE05x_SecObjTyp_AES_KEY:
        case kSE05x_SecObjTyp_DES_KEY:
        case kSE05x_SecObjTyp_HMAC_KEY:
        case kSE05x_SecObjTyp_COUNTER:
        case kSE05x_SecObjTyp_UserID:
            keyObject->objectType = kSSS_KeyPart_Default;
            break;
        default:
            keyObject->objectType = kSSS_KeyPart_NONE;
            break;
        }
    }
    else {
        LOG_W("Error in Se05x_API_ReadType. Further use of object may fail");
        retval = kStatus_SSS_Success;
        return retval;
    }

    retval = kStatus_SSS_Success;
#endif // SSSFTR_SE05X_KEY_GET
    return retval;
}

// LCOV_EXCL_START
sss_status_t sss_se05x_key_object_set_user(sss_se05x_object_t *keyObject, uint32_t user, uint32_t options)
{
    AX_UNUSED_ARG(keyObject);
    AX_UNUSED_ARG(user);
    AX_UNUSED_ARG(options);
    sss_status_t retval = kStatus_SSS_Fail;
    /* Purpose / Policy is set during creation time and hence can not
     * enforced in SE050 later on */
    LOG_W("Not supported in SE05X");
    return retval;
}

sss_status_t sss_se05x_key_object_set_purpose(sss_se05x_object_t *keyObject, sss_mode_t purpose, uint32_t options)
{
    AX_UNUSED_ARG(keyObject);
    AX_UNUSED_ARG(purpose);
    AX_UNUSED_ARG(options);
    sss_status_t retval = kStatus_SSS_Fail;
    /* Purpose / Policy is set during creation time and hence can not
     * enforced in SE050 later on */
    LOG_W("Not supported in SE05X");
    return retval;
}

sss_status_t sss_se05x_key_object_set_access(sss_se05x_object_t *keyObject, uint32_t access, uint32_t options)
{
    AX_UNUSED_ARG(keyObject);
    AX_UNUSED_ARG(access);
    AX_UNUSED_ARG(options);
    sss_status_t retval = kStatus_SSS_Fail;
    LOG_W("Not supported in SE05X");
    return retval;
}

sss_status_t sss_se05x_key_object_set_eccgfp_group(sss_se05x_object_t *keyObject, sss_eccgfp_group_t *group)
{
    AX_UNUSED_ARG(keyObject);
    AX_UNUSED_ARG(group);
    sss_status_t retval = kStatus_SSS_Fail;
    LOG_W("Not supported in SE05X");
    return retval;
}

sss_status_t sss_se05x_key_object_get_user(sss_se05x_object_t *keyObject, uint32_t *user)
{
    AX_UNUSED_ARG(keyObject);
    AX_UNUSED_ARG(user);
    sss_status_t retval = kStatus_SSS_Fail;
    LOG_W("Not supported in SE05X");
    return retval;
}

sss_status_t sss_se05x_key_object_get_purpose(sss_se05x_object_t *keyObject, sss_mode_t *purpose)
{
    AX_UNUSED_ARG(keyObject);
    AX_UNUSED_ARG(purpose);
    sss_status_t retval = kStatus_SSS_Fail;
    LOG_W("Not supported in SE05X");
    return retval;
}

sss_status_t sss_se05x_key_object_get_access(sss_se05x_object_t *keyObject, uint32_t *access)
{
    AX_UNUSED_ARG(keyObject);
    AX_UNUSED_ARG(access);
    sss_status_t retval = kStatus_SSS_Fail;
    LOG_W("Not supported in SE05X");
    return retval;
}
// LCOV_EXCL_STOP

void sss_se05x_key_object_free(sss_se05x_object_t *keyObject)
{
    memset(keyObject, 0, sizeof(*keyObject));
}

/* End: se05x_keyobj */

/* ************************************************************************** */
/* Functions : sss_se05x_keyderive                                            */
/* ************************************************************************** */

sss_status_t sss_se05x_derive_key_context_init(sss_se05x_derive_key_t *context,
    sss_se05x_session_t *session,
    sss_se05x_object_t *keyObject,
    sss_algorithm_t algorithm,
    sss_mode_t mode)
{
    sss_status_t retval = kStatus_SSS_Success;

    context->session   = session;
    context->keyObject = keyObject;
    context->algorithm = algorithm;
    context->mode      = mode;

    return retval;
}

sss_status_t sss_se05x_derive_key_go(sss_se05x_derive_key_t *context,
    const uint8_t *saltData,
    size_t saltLen,
    const uint8_t *info,
    size_t infoLen,
    sss_se05x_object_t *derivedKeyObject,
    uint16_t deriveDataLen,
    uint8_t *hkdfOutput,
    size_t *hkdfOutputLen)
{
    AX_UNUSED_ARG(hkdfOutput);
    AX_UNUSED_ARG(hkdfOutputLen);
    sss_status_t retval                     = kStatus_SSS_Fail;
    smStatus_t status                       = SM_NOT_OK;
    uint8_t hkdfKey[SE05X_MAX_BUF_SIZE_CMD] = {
        0,
    };
    size_t hkdfKeyLen                   = sizeof(hkdfKey);
    sss_object_t *sss_derived_keyObject = (sss_object_t *)derivedKeyObject;
    SE05x_DigestMode_t digestMode;
    ENSURE_OR_GO_EXIT(context);
    ENSURE_OR_GO_EXIT(info);
    ENSURE_OR_GO_EXIT(derivedKeyObject);
    if (saltLen) {
        ENSURE_OR_GO_EXIT(saltData);
    }

    digestMode = se05x_get_sha_algo(context->algorithm);
    ENSURE_OR_GO_EXIT(digestMode != kSE05x_DigestMode_NA);

    status = Se05x_API_HKDF(&context->session->s_ctx,
        context->keyObject->keyId,
        digestMode,
        saltData,
        saltLen,
        info,
        infoLen,
        deriveDataLen,
        hkdfKey,
        &hkdfKeyLen);
    ENSURE_OR_GO_EXIT(status == SM_OK);

    retval = sss_key_store_set_key((sss_key_store_t *)derivedKeyObject->keyStore,
        sss_derived_keyObject,
        hkdfKey,
        hkdfKeyLen,
        hkdfKeyLen * 8,
        NULL,
        0);
    ENSURE_OR_GO_EXIT(retval == kStatus_SSS_Success);

    retval = kStatus_SSS_Success;
exit:

    return retval;
}

sss_status_t sss_se05x_derive_key_one_go(sss_se05x_derive_key_t *context,
    const uint8_t *saltData,
    size_t saltLen,
    const uint8_t *info,
    size_t infoLen,
    sss_se05x_object_t *derivedKeyObject,
    uint16_t deriveDataLen)
{
    sss_status_t retval   = kStatus_SSS_Fail;
    smStatus_t status     = SM_NOT_OK;
    uint8_t hkdfKey[1024] = {
        0,
    };
    size_t hkdfKeyLen                   = sizeof(hkdfKey);
    sss_object_t *sss_derived_keyObject = (sss_object_t *)derivedKeyObject;
    SE05x_DigestMode_t digestMode;
    digestMode            = se05x_get_sha_algo(context->algorithm);
    uint32_t derivedKeyID = (derivedKeyObject == NULL ? 0 : derivedKeyObject->keyId);
    uint8_t *pHkdfKey     = hkdfKey;
    SE05x_HkdfMode_t hkdfMode =
        (context->mode == kMode_SSS_HKDF_ExpandOnly ? kSE05x_HkdfMode_ExpandOnly : kSE05x_HkdfMode_ExtractExpand);

#if SSS_HAVE_SE05X_VER_GTE_06_00
    if (derivedKeyObject != NULL) {
        if (context->keyObject->keyStore->session->subsystem == derivedKeyObject->keyStore->session->subsystem) {
            pHkdfKey = NULL;
        }
    }
#endif

    ENSURE_OR_GO_EXIT(digestMode != kSE05x_DigestMode_NA);

    status = Se05x_API_HKDF_Extended(&context->session->s_ctx,
        context->keyObject->keyId,
        digestMode,
        hkdfMode,
        saltData,
        saltLen,
        0,
        info,
        infoLen,
        derivedKeyID,
        deriveDataLen,
        pHkdfKey,
        &hkdfKeyLen);
    ENSURE_OR_GO_EXIT(status == SM_OK);

    if (pHkdfKey != NULL) {
        if (derivedKeyObject != NULL) {
            retval = sss_key_store_set_key((sss_key_store_t *)derivedKeyObject->keyStore,
                sss_derived_keyObject,
                hkdfKey,
                hkdfKeyLen,
                hkdfKeyLen * 8,
                NULL,
                0);
            ENSURE_OR_GO_EXIT(retval == kStatus_SSS_Success);
        }
    }

    retval = kStatus_SSS_Success;
exit:

    return retval;
}

sss_status_t sss_se05x_derive_key_sobj_one_go(sss_se05x_derive_key_t *context,
    sss_se05x_object_t *saltKeyObject,
    const uint8_t *info,
    size_t infoLen,
    sss_se05x_object_t *derivedKeyObject,
    uint16_t deriveDataLen)
{
    sss_status_t retval   = kStatus_SSS_Fail;
    smStatus_t status     = SM_NOT_OK;
    uint8_t hkdfKey[1024] = {
        0,
    };
    size_t hkdfKeyLen                   = sizeof(hkdfKey);
    sss_object_t *sss_derived_keyObject = (sss_object_t *)derivedKeyObject;
    SE05x_DigestMode_t digestMode;
    digestMode            = se05x_get_sha_algo(context->algorithm);
    uint32_t saltID       = (saltKeyObject == NULL ? 0 : saltKeyObject->keyId);
    uint32_t derivedKeyID = (derivedKeyObject == NULL ? 0 : derivedKeyObject->keyId);
    uint8_t *pHkdfKey     = hkdfKey;
    SE05x_HkdfMode_t hkdfMode =
        (context->mode == kMode_SSS_HKDF_ExpandOnly ? kSE05x_HkdfMode_ExpandOnly : kSE05x_HkdfMode_ExtractExpand);

    if (saltKeyObject != NULL) {
        // Enforce that Salt is stored (securely) in the same keystore as the HMAC key.
        if (context->keyObject->keyStore != saltKeyObject->keyStore) {
            retval = kStatus_SSS_InvalidArgument;
            goto exit;
        }
    }

#if SSS_HAVE_SE05X_VER_GTE_06_00
    if (derivedKeyObject != NULL) {
        if (context->keyObject->keyStore == derivedKeyObject->keyStore) {
            pHkdfKey = NULL;
        }
    }
#endif

    ENSURE_OR_GO_EXIT(digestMode != kSE05x_DigestMode_NA);

    status = Se05x_API_HKDF_Extended(&context->session->s_ctx,
        context->keyObject->keyId,
        digestMode,
        hkdfMode,
        NULL,
        0,
        saltID,
        info,
        infoLen,
        derivedKeyID,
        deriveDataLen,
        pHkdfKey,
        &hkdfKeyLen);
    ENSURE_OR_GO_EXIT(status == SM_OK);

    if (pHkdfKey != NULL) {
        if (derivedKeyObject != NULL) {
            retval = sss_key_store_set_key((sss_key_store_t *)derivedKeyObject->keyStore,
                sss_derived_keyObject,
                hkdfKey,
                hkdfKeyLen,
                hkdfKeyLen * 8,
                NULL,
                0);
            ENSURE_OR_GO_EXIT(retval == kStatus_SSS_Success);
        }
    }

    retval = kStatus_SSS_Success;
exit:

    return retval;
}

sss_status_t sss_se05x_derive_key_dh(
    sss_se05x_derive_key_t *context, sss_se05x_object_t *otherPartyKeyObject, sss_se05x_object_t *derivedKeyObject)
{
    sss_status_t retval = kStatus_SSS_Fail;
    smStatus_t status   = SM_NOT_OK;
    uint8_t pubkey[256] = {0};
    size_t pubkeylen    = sizeof(pubkey);
    uint8_t sharedsecret[256];
    size_t sharedsecretLen  = sizeof(sharedsecret);
    size_t pbKeyBitLen      = 0;
    uint8_t *pPublicKey     = NULL;
    size_t publicKeyLen     = 0;
    uint16_t publicKeyIndex = 0;
#if SSS_HAVE_SE05X_VER_GTE_06_00
    uint8_t invertEndiannes = 0x00;
#endif

    sss_object_t *sss_other_keyObject   = NULL;
    sss_object_t *sss_derived_keyObject = NULL;
    ENSURE_OR_GO_EXIT(context);
    ENSURE_OR_GO_EXIT(otherPartyKeyObject);
    ENSURE_OR_GO_EXIT(derivedKeyObject);
    sss_other_keyObject   = (sss_object_t *)otherPartyKeyObject;
    sss_derived_keyObject = (sss_object_t *)derivedKeyObject;
    retval                = sss_key_store_get_key(
        (sss_key_store_t *)sss_other_keyObject->keyStore, sss_other_keyObject, pubkey, &pubkeylen, &pbKeyBitLen);
    ENSURE_OR_GO_EXIT(retval == kStatus_SSS_Success);

    switch (otherPartyKeyObject->cipherType) {
#if SSS_HAVE_TPM_BN
    case kSSS_CipherType_EC_BARRETO_NAEHRIG:
        /* TODO: Implement asn parser */
        publicKeyLen   = pubkeylen;
        publicKeyIndex = 0;
        break;
#endif
    default: {
        retval = sss_util_pkcs8_asn1_get_ec_public_key_index(
            (const uint8_t *)pubkey, pubkeylen, &publicKeyIndex, &publicKeyLen);
        if (retval != kStatus_SSS_Success) {
            LOG_W("error in sss_util_pkcs8_asn1_get_ec_public_key_index");
            goto exit;
        }
    }
    }

#if SSS_HAVE_EC_MONT
    // Change Endianness Public Key in case of Montgomery Curve
    {
        if (otherPartyKeyObject->cipherType == kSSS_CipherType_EC_MONTGOMERY) {
            for (size_t keyValueIdx = 0; keyValueIdx < (publicKeyLen >> 1); keyValueIdx++) {
                uint8_t swapByte                     = pubkey[publicKeyIndex + keyValueIdx];
                pubkey[publicKeyIndex + keyValueIdx] = pubkey[publicKeyIndex + publicKeyLen - 1 - keyValueIdx];
                pubkey[publicKeyIndex + publicKeyLen - 1 - keyValueIdx] = swapByte;
            }
        }
    }
#endif

    pPublicKey = &pubkey[publicKeyIndex];
#if SSS_HAVE_SE05X_VER_GTE_06_00
#if SSS_HAVE_EC_MONT
    if (otherPartyKeyObject->cipherType == kSSS_CipherType_EC_MONTGOMERY) {
        // In case of Montgomery curves we want to store the
        // shared secret using Little Endian Convention
        invertEndiannes = 0x01;
    }
#endif

    if (context->keyObject->keyStore->session->subsystem == derivedKeyObject->keyStore->session->subsystem) {
        status = Se05x_API_ECDHGenerateSharedSecret_InObject(&context->session->s_ctx,
            context->keyObject->keyId,
            pPublicKey,
            publicKeyLen,
            derivedKeyObject->keyId,
            invertEndiannes);
        if (status != SM_OK) {
            LOG_W("error in Se05x_API_ECDHGenerateSharedSecret_InObject");
            retval = kStatus_SSS_Fail;
            goto exit;
        }
    }
    else {
#endif
        status = Se05x_API_ECGenSharedSecret(&context->session->s_ctx,
            context->keyObject->keyId,
            pPublicKey,
            publicKeyLen,
            sharedsecret,
            &sharedsecretLen);
        if (status != SM_OK) {
            retval = kStatus_SSS_Fail;
            goto exit;
        }

#if SSS_HAVE_EC_MONT
        // Change Endianness Shared Secret in case of Montgomery Curve
        {
            if (otherPartyKeyObject->cipherType == kSSS_CipherType_EC_MONTGOMERY) {
                for (size_t keyValueIdx = 0; keyValueIdx < (publicKeyLen >> 1); keyValueIdx++) {
                    uint8_t swapByte                             = sharedsecret[keyValueIdx];
                    sharedsecret[keyValueIdx]                    = sharedsecret[publicKeyLen - 1 - keyValueIdx];
                    sharedsecret[publicKeyLen - 1 - keyValueIdx] = swapByte;
                }
            }
        }
#endif

        retval = sss_key_store_set_key((sss_key_store_t *)derivedKeyObject->keyStore,
            sss_derived_keyObject,
            sharedsecret,
            sharedsecretLen,
            sharedsecretLen * 8,
            NULL,
            0);
        ENSURE_OR_GO_EXIT(retval == kStatus_SSS_Success);
#if SSS_HAVE_SE05X_VER_GTE_06_00
    }
#endif

    retval = kStatus_SSS_Success;
exit:
    return retval;
}

void sss_se05x_derive_key_context_free(sss_se05x_derive_key_t *context)
{
    AX_UNUSED_ARG(context);
}

/* End: se05x_keyderive */

/* ************************************************************************** */
/* Functions : sss_se05x_keystore                                             */
/* ************************************************************************** */

sss_status_t sss_se05x_key_store_context_init(sss_se05x_key_store_t *keyStore, sss_se05x_session_t *session)
{
    sss_status_t retval = kStatus_SSS_Success;
    memset(keyStore, 0, sizeof(*keyStore));
    keyStore->session = session;
    return retval;
}

sss_status_t sss_se05x_key_store_allocate(sss_se05x_key_store_t *keyStore, uint32_t keyStoreId)
{
    AX_UNUSED_ARG(keyStore);
    AX_UNUSED_ARG(keyStoreId);
    return kStatus_SSS_Success;
}

sss_status_t sss_se05x_key_store_save(sss_se05x_key_store_t *keyStore)
{
    AX_UNUSED_ARG(keyStore);
    return kStatus_SSS_Success;
}

sss_status_t sss_se05x_key_store_load(sss_se05x_key_store_t *keyStore)
{
    AX_UNUSED_ARG(keyStore);
    return kStatus_SSS_Success;
}

#if SSSFTR_SE05X_RSA && SSSFTR_SE05X_KEY_SET && SSS_HAVE_RSA
static sss_status_t sss_se05x_key_store_set_rsa_key(sss_se05x_key_store_t *keyStore,
    sss_se05x_object_t *keyObject,
    const uint8_t *key,
    size_t keyLen,
    size_t keyBitLen,
    void *policy_buff,
    size_t policy_buff_len)
{
    sss_status_t retval = kStatus_SSS_Fail;
    smStatus_t status   = SM_NOT_OK;
    //int ret;
    uint32_t key_type = keyObject->objectType;
    Se05xPolicy_t se05x_policy;
    //SE05x_KeyPart_t key_part;
    uint8_t *rsaN = NULL, *rsaE = NULL, *rsaD = NULL;
    uint8_t *rsaP = NULL, *rsaQ = NULL, *rsaDP = NULL, *rsaDQ = NULL, *rsaQINV = NULL;
    size_t rsaNlen, rsaElen, rsaDlen;
    size_t rsaPlen, rsaQlen, rsaDPlen, rsaDQlen, rsaQINVlen;

    se05x_policy.value     = (uint8_t *)policy_buff;
    se05x_policy.value_len = policy_buff_len;
    SE05x_INS_t transient_type;
    SE05x_RSAKeyFormat_t rsa_format;
    uint8_t IdExists          = 0;
    size_t keyBitLength       = 0;
    SE05x_Result_t obj_exists = kSE05x_Result_NA;

    /* Assign proper instruction type based on keyObject->isPersistant  */
    (keyObject->isPersistant) ? (transient_type = kSE05x_INS_NA) : (transient_type = kSE05x_INS_TRANSIENT);

    if (keyObject->cipherType == kSSS_CipherType_RSA) {
        rsa_format = kSE05x_RSAKeyFormat_RAW;
    }
    else if (keyObject->cipherType == kSSS_CipherType_RSA_CRT) {
        rsa_format = kSE05x_RSAKeyFormat_CRT;
    }
    else {
        retval = kStatus_SSS_Fail;
        goto exit;
    }

#if 0
    if (key_type == kSSS_KeyPart_Public) {
        key_part = SE05x_KeyPart_Public;
    }
    else if (key_type == kSSS_KeyPart_Private) {
        key_part = kSE05x_KeyPart_Private;
    }
    else if (key_type == kSSS_KeyPart_Pair) {
        key_part = kSE05x_KeyPart_Pair;
    }
    else {
        retval = kStatus_SSS_Fail;
        goto exit;
    }

    /* Set the kye parameters */
    status = Se05x_API_WriteRSAKey(&keyStore->session->s_ctx,
        &se05x_policy,
        keyObject->keyId,
        (U16)keyBitLen,
        SE05X_RSA_NO_p,
        SE05X_RSA_NO_q,
        SE05X_RSA_NO_dp,
        SE05X_RSA_NO_dq,
        SE05X_RSA_NO_qInv,
        SE05X_RSA_NO_pubExp,
        SE05X_RSA_NO_priv,
        SE05X_RSA_NO_pubMod,
        transient_type,
        key_part,
        rsa_format);

    if (status != SM_OK) {
        retval = kStatus_SSS_Fail;
        goto exit;
    }
#endif

    if (key_type == kSSS_KeyPart_Public) {
        retval = sss_util_asn1_rsa_parse_public(key, keyLen, &rsaN, &rsaNlen, &rsaE, &rsaElen);
        ENSURE_OR_GO_EXIT(retval == kStatus_SSS_Success);

        IdExists     = CheckIfKeyIdExists(keyObject->keyId, &keyStore->session->s_ctx);
        keyBitLength = (IdExists == 1) ? 0 : keyBitLen;
        obj_exists   = (IdExists == 1) ? kSE05x_Result_SUCCESS : kSE05x_Result_FAILURE;

        /* Set the Public Exponent */
        status = sss_se05x_LL_set_RSA_key(&keyStore->session->s_ctx,
            &se05x_policy,
            keyObject->keyId,
            (U16)keyBitLength,
            SE05X_RSA_NO_p,
            SE05X_RSA_NO_q,
            SE05X_RSA_NO_dp,
            SE05X_RSA_NO_dq,
            SE05X_RSA_NO_qInv,
            rsaE,
            rsaElen,
            SE05X_RSA_NO_priv,
            SE05X_RSA_NO_pubMod,
            transient_type,
            kSE05x_KeyPart_Public,
            rsa_format,
            obj_exists);
        if (status != SM_OK) {
            retval = kStatus_SSS_Fail;
            goto exit;
        }

        /* Set the Modulus */
        status = sss_se05x_LL_set_RSA_key(&keyStore->session->s_ctx,
            NULL,
            keyObject->keyId,
            0,
            SE05X_RSA_NO_p,
            SE05X_RSA_NO_q,
            SE05X_RSA_NO_dp,
            SE05X_RSA_NO_dq,
            SE05X_RSA_NO_qInv,
            SE05X_RSA_NO_pubExp,
            SE05X_RSA_NO_priv,
            rsaN,
            rsaNlen,
            transient_type,
            kSE05x_KeyPart_NA,
            rsa_format,
            obj_exists);

        if (status != SM_OK) {
            retval = kStatus_SSS_Fail;
            goto exit;
        }
    }
    else if (key_type == kSSS_KeyPart_Private) {
        if (keyObject->cipherType == kSSS_CipherType_RSA) {
            retval = sss_util_asn1_rsa_parse_private(key,
                keyLen,
                (sss_cipher_type_t)keyObject->cipherType,
                &rsaN,
                &rsaNlen,
                NULL,
                NULL,
                &rsaD,
                &rsaDlen,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL);
            if (retval != kStatus_SSS_Success) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }
            if ((rsaN == NULL) || (rsaD == NULL)) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }

            IdExists     = CheckIfKeyIdExists(keyObject->keyId, &keyStore->session->s_ctx);
            keyBitLength = (IdExists == 1) ? 0 : keyBitLen;
            obj_exists   = (IdExists == 1) ? kSE05x_Result_SUCCESS : kSE05x_Result_FAILURE;

            // Set D(Private exponent) component
            status = sss_se05x_LL_set_RSA_key(&keyStore->session->s_ctx,
                &se05x_policy,
                keyObject->keyId,
                (U16)keyBitLength,
                SE05X_RSA_NO_p,
                SE05X_RSA_NO_q,
                SE05X_RSA_NO_dp,
                SE05X_RSA_NO_dq,
                SE05X_RSA_NO_qInv,
                SE05X_RSA_NO_pubExp,
                rsaD,
                rsaDlen,
                SE05X_RSA_NO_pubMod,
                transient_type,
                kSE05x_KeyPart_Private,
                rsa_format,
                obj_exists);

            if (status != SM_OK) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }

            // Set N(Modulus) component
            status = sss_se05x_LL_set_RSA_key(&keyStore->session->s_ctx,
                NULL,
                keyObject->keyId,
                0,
                SE05X_RSA_NO_p,
                SE05X_RSA_NO_q,
                SE05X_RSA_NO_dp,
                SE05X_RSA_NO_dq,
                SE05X_RSA_NO_qInv,
                SE05X_RSA_NO_pubExp,
                SE05X_RSA_NO_priv,
                rsaN,
                rsaNlen,
                transient_type,
                kSE05x_KeyPart_NA,
                rsa_format,
                obj_exists);

            if (status != SM_OK) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }
        }
        else if (keyObject->cipherType == kSSS_CipherType_RSA_CRT) {
            retval = sss_util_asn1_rsa_parse_private(key,
                keyLen,
                (sss_cipher_type_t)keyObject->cipherType,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                &rsaP,
                &rsaPlen,
                &rsaQ,
                &rsaQlen,
                &rsaDP,
                &rsaDPlen,
                &rsaDQ,
                &rsaDQlen,
                &rsaQINV,
                &rsaQINVlen);
            if (retval != kStatus_SSS_Success) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }
            if ((rsaP == NULL) || (rsaQ == NULL) || (rsaDP == NULL) || (rsaDQ == NULL) || (rsaQINV == NULL)) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }

            IdExists     = CheckIfKeyIdExists(keyObject->keyId, &keyStore->session->s_ctx);
            keyBitLength = (IdExists == 1) ? 0 : keyBitLen;
            obj_exists   = (IdExists == 1) ? kSE05x_Result_SUCCESS : kSE05x_Result_FAILURE;

            // Set P component
            status = sss_se05x_LL_set_RSA_key(&keyStore->session->s_ctx,
                &se05x_policy,
                keyObject->keyId,
                (U16)keyBitLength,
                rsaP,
                rsaPlen,
                SE05X_RSA_NO_q,
                SE05X_RSA_NO_dp,
                SE05X_RSA_NO_dq,
                SE05X_RSA_NO_qInv,
                SE05X_RSA_NO_pubExp,
                SE05X_RSA_NO_priv,
                SE05X_RSA_NO_pubMod,
                transient_type,
                kSE05x_KeyPart_Private,
                rsa_format,
                obj_exists);

            if (status != SM_OK) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }

            // Set Q component
            status = sss_se05x_LL_set_RSA_key(&keyStore->session->s_ctx,
                NULL,
                keyObject->keyId,
                0,
                SE05X_RSA_NO_p,
                rsaQ,
                rsaQlen,
                SE05X_RSA_NO_dp,
                SE05X_RSA_NO_dq,
                SE05X_RSA_NO_qInv,
                SE05X_RSA_NO_pubExp,
                SE05X_RSA_NO_priv,
                SE05X_RSA_NO_pubMod,
                transient_type,
                kSE05x_KeyPart_NA,
                rsa_format,
                obj_exists);

            if (status != SM_OK) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }

            // Set DP component
            status = sss_se05x_LL_set_RSA_key(&keyStore->session->s_ctx,
                NULL,
                keyObject->keyId,
                0,
                SE05X_RSA_NO_p,
                SE05X_RSA_NO_q,
                rsaDP,
                rsaDPlen,
                SE05X_RSA_NO_dq,
                SE05X_RSA_NO_qInv,
                SE05X_RSA_NO_pubExp,
                SE05X_RSA_NO_priv,
                SE05X_RSA_NO_pubMod,
                transient_type,
                kSE05x_KeyPart_NA,
                rsa_format,
                obj_exists);

            if (status != SM_OK) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }

            // Set DQ component
            status = sss_se05x_LL_set_RSA_key(&keyStore->session->s_ctx,
                NULL,
                keyObject->keyId,
                0,
                SE05X_RSA_NO_p,
                SE05X_RSA_NO_q,
                SE05X_RSA_NO_dp,
                rsaDQ,
                rsaDQlen,
                SE05X_RSA_NO_qInv,
                SE05X_RSA_NO_pubExp,
                SE05X_RSA_NO_priv,
                SE05X_RSA_NO_pubMod,
                transient_type,
                kSE05x_KeyPart_NA,
                rsa_format,
                obj_exists);

            if (status != SM_OK) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }

            // Set INV_Q component
            status = sss_se05x_LL_set_RSA_key(&keyStore->session->s_ctx,
                NULL,
                keyObject->keyId,
                0,
                SE05X_RSA_NO_p,
                SE05X_RSA_NO_q,
                SE05X_RSA_NO_dp,
                SE05X_RSA_NO_dq,
                rsaQINV,
                rsaQINVlen,
                SE05X_RSA_NO_pubExp,
                SE05X_RSA_NO_priv,
                SE05X_RSA_NO_pubMod,
                transient_type,
                kSE05x_KeyPart_NA,
                rsa_format,
                obj_exists);

            if (status != SM_OK) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }
        }
    }
    else if (key_type == kSSS_KeyPart_Pair) {
        if (keyObject->cipherType == kSSS_CipherType_RSA) {
            retval = sss_util_asn1_rsa_parse_private(key,
                keyLen,
                (sss_cipher_type_t)keyObject->cipherType,
                &rsaN,
                &rsaNlen,
                &rsaE,
                &rsaElen,
                &rsaD,
                &rsaDlen,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL);

            ENSURE_OR_GO_EXIT(retval == kStatus_SSS_Success);
            ENSURE_OR_EXIT_WITH_STATUS_ON_ERROR(
                !((rsaD == NULL) || (rsaE == NULL) || (rsaN == NULL)), retval, kStatus_SSS_Fail);

            IdExists     = CheckIfKeyIdExists(keyObject->keyId, &keyStore->session->s_ctx);
            keyBitLength = (IdExists == 1) ? 0 : keyBitLen;
            obj_exists   = (IdExists == 1) ? kSE05x_Result_SUCCESS : kSE05x_Result_FAILURE;

            // Set E(Public exponent) component
            status = sss_se05x_LL_set_RSA_key(&keyStore->session->s_ctx,
                &se05x_policy,
                keyObject->keyId,
                (U16)keyBitLength,
                SE05X_RSA_NO_p,
                SE05X_RSA_NO_q,
                SE05X_RSA_NO_dp,
                SE05X_RSA_NO_dq,
                SE05X_RSA_NO_qInv,
                rsaE,
                rsaElen,
                SE05X_RSA_NO_priv,
                SE05X_RSA_NO_pubMod,
                transient_type,
                kSE05x_KeyPart_Pair,
                rsa_format,
                obj_exists);

            if (status != SM_OK) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }

            // Set D(Private exponent) component
            status = sss_se05x_LL_set_RSA_key(&keyStore->session->s_ctx,
                NULL,
                keyObject->keyId,
                0,
                SE05X_RSA_NO_p,
                SE05X_RSA_NO_q,
                SE05X_RSA_NO_dp,
                SE05X_RSA_NO_dq,
                SE05X_RSA_NO_qInv,
                SE05X_RSA_NO_pubExp,
                rsaD,
                rsaDlen,
                SE05X_RSA_NO_pubMod,
                transient_type,
                kSE05x_KeyPart_NA,
                rsa_format,
                obj_exists);

            if (status != SM_OK) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }

            // Set N(Modulus) component
            status = sss_se05x_LL_set_RSA_key(&keyStore->session->s_ctx,
                NULL,
                keyObject->keyId,
                0,
                SE05X_RSA_NO_p,
                SE05X_RSA_NO_q,
                SE05X_RSA_NO_dp,
                SE05X_RSA_NO_dq,
                SE05X_RSA_NO_qInv,
                SE05X_RSA_NO_pubExp,
                SE05X_RSA_NO_priv,
                rsaN,
                rsaNlen,
                transient_type,
                kSE05x_KeyPart_NA,
                rsa_format,
                obj_exists);

            if (status != SM_OK) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }
        }
        else if (keyObject->cipherType == kSSS_CipherType_RSA_CRT) {
            retval = sss_util_asn1_rsa_parse_private(key,
                keyLen,
                (sss_cipher_type_t)keyObject->cipherType,
                &rsaN,
                &rsaNlen,
                &rsaE,
                &rsaElen,
                NULL,
                NULL,
                &rsaP,
                &rsaPlen,
                &rsaQ,
                &rsaQlen,
                &rsaDP,
                &rsaDPlen,
                &rsaDQ,
                &rsaDQlen,
                &rsaQINV,
                &rsaQINVlen);
            ENSURE_OR_GO_EXIT(retval == kStatus_SSS_Success);

            if ((rsaP == NULL) || (rsaQ == NULL) || (rsaDP == NULL) || (rsaDQ == NULL) || (rsaQINV == NULL) ||
                (rsaE == NULL) || (rsaN == NULL)) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }

            IdExists     = CheckIfKeyIdExists(keyObject->keyId, &keyStore->session->s_ctx);
            keyBitLength = (IdExists == 1) ? 0 : keyBitLen;
            obj_exists   = (IdExists == 1) ? kSE05x_Result_SUCCESS : kSE05x_Result_FAILURE;

            // Set P component
            status = sss_se05x_LL_set_RSA_key(&keyStore->session->s_ctx,
                &se05x_policy,
                keyObject->keyId,
                (U16)keyBitLength,
                rsaP,
                rsaPlen,
                SE05X_RSA_NO_q,
                SE05X_RSA_NO_dp,
                SE05X_RSA_NO_dq,
                SE05X_RSA_NO_qInv,
                SE05X_RSA_NO_pubExp,
                SE05X_RSA_NO_priv,
                SE05X_RSA_NO_pubMod,
                transient_type,
                kSE05x_KeyPart_Pair,
                rsa_format,
                obj_exists);

            if (status != SM_OK) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }

            // Set Q component
            status = sss_se05x_LL_set_RSA_key(&keyStore->session->s_ctx,
                NULL,
                keyObject->keyId,
                0,
                SE05X_RSA_NO_p,
                rsaQ,
                rsaQlen,
                SE05X_RSA_NO_dp,
                SE05X_RSA_NO_dq,
                SE05X_RSA_NO_qInv,
                SE05X_RSA_NO_pubExp,
                SE05X_RSA_NO_priv,
                SE05X_RSA_NO_pubMod,
                transient_type,
                kSE05x_KeyPart_NA,
                rsa_format,
                obj_exists);

            if (status != SM_OK) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }

            // Set DP component
            status = sss_se05x_LL_set_RSA_key(&keyStore->session->s_ctx,
                NULL,
                keyObject->keyId,
                0,
                SE05X_RSA_NO_p,
                SE05X_RSA_NO_q,
                rsaDP,
                rsaDPlen,
                SE05X_RSA_NO_dq,
                SE05X_RSA_NO_qInv,
                SE05X_RSA_NO_pubExp,
                SE05X_RSA_NO_priv,
                SE05X_RSA_NO_pubMod,
                transient_type,
                kSE05x_KeyPart_NA,
                rsa_format,
                obj_exists);

            if (status != SM_OK) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }

            // Set DQ component
            status = sss_se05x_LL_set_RSA_key(&keyStore->session->s_ctx,
                NULL,
                keyObject->keyId,
                0,
                SE05X_RSA_NO_p,
                SE05X_RSA_NO_q,
                SE05X_RSA_NO_dp,
                rsaDQ,
                rsaDQlen,
                SE05X_RSA_NO_qInv,
                SE05X_RSA_NO_pubExp,
                SE05X_RSA_NO_priv,
                SE05X_RSA_NO_pubMod,
                transient_type,
                kSE05x_KeyPart_NA,
                rsa_format,
                obj_exists);

            if (status != SM_OK) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }

            // Set INV_Q component
            status = sss_se05x_LL_set_RSA_key(&keyStore->session->s_ctx,
                NULL,
                keyObject->keyId,
                0,
                SE05X_RSA_NO_p,
                SE05X_RSA_NO_q,
                SE05X_RSA_NO_dp,
                SE05X_RSA_NO_dq,
                rsaQINV,
                rsaQINVlen,
                SE05X_RSA_NO_pubExp,
                SE05X_RSA_NO_priv,
                SE05X_RSA_NO_pubMod,
                transient_type,
                kSE05x_KeyPart_NA,
                rsa_format,
                obj_exists);

            if (status != SM_OK) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }

            // Set E (Public exponent) component
            status = sss_se05x_LL_set_RSA_key(&keyStore->session->s_ctx,
                NULL,
                keyObject->keyId,
                0,
                SE05X_RSA_NO_p,
                SE05X_RSA_NO_q,
                SE05X_RSA_NO_dp,
                SE05X_RSA_NO_dq,
                SE05X_RSA_NO_qInv,
                rsaE,
                rsaElen,
                SE05X_RSA_NO_priv,
                SE05X_RSA_NO_pubMod,
                transient_type,
                kSE05x_KeyPart_NA,
                rsa_format,
                obj_exists);

            if (status != SM_OK) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }

            // Set N (Modulus) component
            status = sss_se05x_LL_set_RSA_key(&keyStore->session->s_ctx,
                NULL,
                keyObject->keyId,
                0,
                SE05X_RSA_NO_p,
                SE05X_RSA_NO_q,
                SE05X_RSA_NO_dp,
                SE05X_RSA_NO_dq,
                SE05X_RSA_NO_qInv,
                SE05X_RSA_NO_pubExp,
                SE05X_RSA_NO_priv,
                rsaN,
                rsaNlen,
                transient_type,
                kSE05x_KeyPart_NA,
                rsa_format,
                obj_exists);

            if (status != SM_OK) {
                retval = kStatus_SSS_Fail;
                goto exit;
            }
        }
    }
exit:
    if (rsaN != NULL) {
        SSS_FREE(rsaN);
    }
    if (rsaE != NULL) {
        SSS_FREE(rsaE);
    }
    if (rsaD != NULL) {
        SSS_FREE(rsaD);
    }
    if (rsaP != NULL) {
        SSS_FREE(rsaP);
    }
    if (rsaQ != NULL) {
        SSS_FREE(rsaQ);
    }
    if (rsaDP != NULL) {
        SSS_FREE(rsaDP);
    }
    if (rsaDQ != NULL) {
        SSS_FREE(rsaDQ);
    }
    if (rsaQINV != NULL) {
        SSS_FREE(rsaQINV);
    }

    return retval;
}
#endif // SSSFTR_SE05X_RSA && SSSFTR_SE05X_KEY_SET && SSS_HAVE_RSA

#if SSSFTR_SE05X_ECC && SSSFTR_SE05X_KEY_SET
static sss_status_t getEccPrivPubKeyLen(uint32_t curve_id, size_t *pubKeyLen, size_t *privKeyLen)
{
    sss_status_t retval = kStatus_SSS_Success;

    if (privKeyLen == NULL || pubKeyLen == NULL) {
        return kStatus_SSS_Fail;
    }

    switch (curve_id) {
#if SSS_HAVE_EC_NIST_K || SSS_HAVE_EC_BP
#if SSS_HAVE_EC_NIST_K
    case kSE05x_ECCurve_Secp160k1:
#endif
#if SSS_HAVE_EC_BP
    case kSE05x_ECCurve_Brainpool160:
#endif
    {
        *privKeyLen = 20;
        *pubKeyLen  = 41;
    } break;
#endif // SSS_HAVE_EC_NIST_K || SSS_HAVE_EC_BP

#if SSS_HAVE_EC_NIST_192 || SSS_HAVE_EC_NIST_K || SSS_HAVE_EC_BP
#if SSS_HAVE_EC_NIST_192
    case kSE05x_ECCurve_NIST_P192:
#endif
#if SSS_HAVE_EC_BP
    case kSE05x_ECCurve_Brainpool192:
#endif
#if SSS_HAVE_EC_NIST_K
    case kSE05x_ECCurve_Secp192k1:
#endif
    {
        *privKeyLen = 24;
        *pubKeyLen  = 49;
    } break;
#endif // SSS_HAVE_EC_NIST_192 || SSS_HAVE_EC_NIST_K || SSS_HAVE_EC_BP

#if SSS_HAVE_EC_NIST_224 | SSS_HAVE_EC_NIST_K || SSS_HAVE_EC_BP
#if SSS_HAVE_EC_NIST_224
    case kSE05x_ECCurve_NIST_P224:
#endif
#if SSS_HAVE_EC_BP
    case kSE05x_ECCurve_Brainpool224:
#endif
#if SSS_HAVE_EC_NIST_K
    case kSE05x_ECCurve_Secp224k1:
#endif
    {
        *privKeyLen = 28;
        *pubKeyLen  = 57;
    } break;
#endif // SSS_HAVE_EC_NIST_224| SSS_HAVE_EC_NIST_K || SSS_HAVE_EC_BP

    case kSE05x_ECCurve_NIST_P256:
#if SSS_HAVE_EC_BP
    case kSE05x_ECCurve_Brainpool256:
#endif
#if SSS_HAVE_EC_NIST_K
    case kSE05x_ECCurve_Secp256k1:
#endif
    {
        *privKeyLen = 32;
        *pubKeyLen  = 65;
    } break;

#if SSS_HAVE_EC_BP
    case kSE05x_ECCurve_Brainpool320: {
        *privKeyLen = 40;
        *pubKeyLen  = 81;
    } break;
#endif
    case kSE05x_ECCurve_NIST_P384:
#if SSS_HAVE_EC_BP
    case kSE05x_ECCurve_Brainpool384:
#endif
    {
        *privKeyLen = 48;
        *pubKeyLen  = 97;
    } break;

#if SSS_HAVE_EC_NIST_521
    case kSE05x_ECCurve_NIST_P521: {
        *privKeyLen = 66;
        *pubKeyLen  = 133;
    } break;
#endif

#if SSS_HAVE_EC_BP
    case kSE05x_ECCurve_Brainpool512: {
        *privKeyLen = 64;
        *pubKeyLen  = 129;
    } break;
#endif

#if SSS_HAVE_EC_MONT || SSS_HAVE_EC_ED
#if SSS_HAVE_EC_MONT
    case kSE05x_ECCurve_ECC_MONT_DH_25519:
#endif
#if SSS_HAVE_EC_ED
    case kSE05x_ECCurve_ECC_ED_25519:
#endif
    {
        *privKeyLen = 32;
        *pubKeyLen  = 32;
    } break;
#endif // SSS_HAVE_EC_MONT || SSS_HAVE_EC_ED

#if SSS_HAVE_SE05X_VER_GTE_06_00 && SSS_HAVE_EC_MONT
    case kSE05x_ECCurve_RESERVED_ID_ECC_MONT_DH_448: {
        *privKeyLen = 56;
        *pubKeyLen  = 56;
    } break;
#endif
    default: {
        *privKeyLen = 0;
        *pubKeyLen  = 0;
        retval      = kStatus_SSS_Fail;
    } break;
    }

    return retval;
}
#endif //SSSFTR_SE05X_ECC && SSSFTR_SE05X_KEY_SET

#if SSSFTR_SE05X_ECC && SSSFTR_SE05X_KEY_SET
/* sss_se05x_create_curve_if_needed for internal to this file and for tests */
smStatus_t sss_se05x_create_curve_if_needed(Se05xSession_t *pSession, uint32_t curve_id)
{
    smStatus_t status = SM_NOT_OK;
    //uint32_t existing_curve_id = 0;
    uint8_t curveList[kSE05x_ECCurve_Total_Weierstrass_Curves] = {
        0,
    };
    size_t curveListLen = sizeof(curveList);
    //int i = 0;

#if SSS_HAVE_EC_ED
    if (curve_id == kSE05x_ECCurve_RESERVED_ID_ECC_ED_25519) {
        /* ECC_ED_25519 is always preset */
        return SM_OK;
    }
#endif

#if SSS_HAVE_EC_MONT
    if (curve_id == kSE05x_ECCurve_RESERVED_ID_ECC_MONT_DH_25519
#if SSS_HAVE_SE05X_VER_GTE_06_00
        || curve_id == kSE05x_ECCurve_RESERVED_ID_ECC_MONT_DH_448
#endif
    ) {
#if SSS_HAVE_SE05X_VER_GTE_06_00
        status = Se05x_API_CreateECCurve(pSession, (SE05x_ECCurve_t)curve_id);
        /* If curve is already created, Se05x_API_CreateECCurve fails. Ignore this error */
        return SM_OK;
#else
        return SM_OK;
        /* ECC_MONT_DH_25519 and ECC_MONT_DH_448 are always present */
#endif
    }
#endif // SSS_HAVE_EC_MONT

    status = Se05x_API_ReadECCurveList(pSession, curveList, &curveListLen);
    if (status == SM_OK) {
        if (curveList[curve_id - 1] == kSE05x_SetIndicator_SET) {
            return SM_OK;
        }
    }
    else {
        return SM_NOT_OK;
    }

    status = SM_NOT_OK;

    switch (curve_id) {
#if SSS_HAVE_EC_NIST_192
    case kSE05x_ECCurve_NIST_P192:
        status = Se05x_API_CreateCurve_prime192v1(pSession, curve_id);
        break;
#endif
#if SSS_HAVE_EC_NIST_224
    case kSE05x_ECCurve_NIST_P224:
        status = Se05x_API_CreateCurve_secp224r1(pSession, curve_id);
        break;
#endif
    case kSE05x_ECCurve_NIST_P256:
        status = Se05x_API_CreateCurve_prime256v1(pSession, curve_id);
        break;
    case kSE05x_ECCurve_NIST_P384:
        status = Se05x_API_CreateCurve_secp384r1(pSession, curve_id);
        break;
#if SSS_HAVE_EC_NIST_521
    case kSE05x_ECCurve_NIST_P521:
        status = Se05x_API_CreateCurve_secp521r1(pSession, curve_id);
        break;
#endif
#if SSS_HAVE_EC_BP
    case kSE05x_ECCurve_Brainpool160:
        status = Se05x_API_CreateCurve_brainpoolP160r1(pSession, curve_id);
        break;
    case kSE05x_ECCurve_Brainpool192:
        status = Se05x_API_CreateCurve_brainpoolP192r1(pSession, curve_id);
        break;
    case kSE05x_ECCurve_Brainpool224:
        status = Se05x_API_CreateCurve_brainpoolP224r1(pSession, curve_id);
        break;
    case kSE05x_ECCurve_Brainpool256:
        status = Se05x_API_CreateCurve_brainpoolP256r1(pSession, curve_id);
        break;
    case kSE05x_ECCurve_Brainpool320:
        status = Se05x_API_CreateCurve_brainpoolP320r1(pSession, curve_id);
        break;
    case kSE05x_ECCurve_Brainpool384:
        status = Se05x_API_CreateCurve_brainpoolP384r1(pSession, curve_id);
        break;
    case kSE05x_ECCurve_Brainpool512:
        status = Se05x_API_CreateCurve_brainpoolP512r1(pSession, curve_id);
        break;
#endif
#if SSS_HAVE_EC_NIST_K
    case kSE05x_ECCurve_Secp160k1:
        status = Se05x_API_CreateCurve_secp160k1(pSession, curve_id);
        break;
    case kSE05x_ECCurve_Secp192k1:
        status = Se05x_API_CreateCurve_secp192k1(pSession, curve_id);
        break;
    case kSE05x_ECCurve_Secp224k1:
        status = Se05x_API_CreateCurve_secp224k1(pSession, curve_id);
        break;
    case kSE05x_ECCurve_Secp256k1:
        status = Se05x_API_CreateCurve_secp256k1(pSession, curve_id);
        break;
#endif
#if SSS_HAVE_TPM_BN
    case kSE05x_ECCurve_TPM_ECC_BN_P256:
        status = Se05x_API_CreateCurve_tpm_bm_p256(pSession, curve_id);
        break;
#endif
    default:
        break;
    }

    ENSURE_OR_GO_EXIT(status != SM_NOT_OK);
    if (status == SM_ERR_CONDITIONS_NOT_SATISFIED) {
        LOG_W("Allowing SM_ERR_CONDITIONS_NOT_SATISFIED for CreateCurve");
    }
exit:
    return status;
}
#endif // SSSFTR_SE05X_ECC && SSSFTR_SE05X_KEY_SET

#if SSSFTR_SE05X_KEY_SET || SSSFTR_SE05X_KEY_GET
static uint8_t CheckIfKeyIdExists(uint32_t keyId, pSe05xSession_t session_ctx)
{
    smStatus_t retStatus    = SM_NOT_OK;
    SE05x_Result_t IdExists = kSE05x_Result_NA;

    retStatus = Se05x_API_CheckObjectExists(session_ctx, keyId, &IdExists);
    if (retStatus == SM_OK) {
        if (IdExists == kSE05x_Result_SUCCESS) {
            LOG_D("Key Id 0x%X exists", keyId);
            return 1;
        }
        else {
            return 0;
        }
    }
    else {
        LOG_E("Error in Se05x_API_CheckObjectExists");
        return 0;
    }
}
#endif

#if SSSFTR_SE05X_ECC && SSSFTR_SE05X_KEY_SET
static sss_status_t sss_se05x_key_store_set_ecc_key(sss_se05x_key_store_t *keyStore,
    sss_se05x_object_t *keyObject,
    const uint8_t *key,
    size_t keyLen,
    size_t keyBitLen,
    void *policy_buff,
    size_t policy_buff_len)
{
    sss_status_t retval     = kStatus_SSS_Fail;
    sss_status_t asn_retval = kStatus_SSS_Fail;
    smStatus_t status       = SM_NOT_OK;
    Se05xPolicy_t se05x_policy;
    SE05x_INS_t transient_type;
    SE05x_ECCurve_t curveId    = keyObject->curve_id;
    SE05x_KeyPart_t key_part   = kSE05x_KeyPart_NA;
    SE05x_Result_t exists      = kSE05x_Result_NA;
    SE05x_ECCurve_t retCurveId = keyObject->curve_id;
    size_t std_pubKey_len      = 0;
    size_t std_privKey_len     = 0;
#if SSS_HAVE_EC_MONT || SSS_HAVE_EC_ED
    uint8_t privKeyReversed[64] = {
        0,
    };
    uint8_t pubKeyReversed[64] = {
        0,
    };
#endif

    /* Assign proper instruction type based on keyObject->isPersistant  */
    (keyObject->isPersistant) ? (transient_type = kSE05x_INS_NA) : (transient_type = kSE05x_INS_TRANSIENT);

    se05x_policy.value     = (uint8_t *)policy_buff;
    se05x_policy.value_len = policy_buff_len;

    if (keyObject->curve_id == 0) {
        keyObject->curve_id =
            (SE05x_ECCurve_t)se05x_sssKeyTypeLenToCurveId((sss_cipher_type_t)keyObject->cipherType, keyBitLen);
    }

    if (keyObject->curve_id == 0) {
        goto exit;
    }

    status = sss_se05x_create_curve_if_needed(&keyObject->keyStore->session->s_ctx, keyObject->curve_id);

    if (status == SM_NOT_OK) {
        goto exit;
    }
    else if (status == SM_ERR_CONDITIONS_NOT_SATISFIED) {
        LOG_W("Allowing SM_ERR_CONDITIONS_NOT_SATISFIED for CreateCurve");
    }
    status = Se05x_API_CheckObjectExists(&keyStore->session->s_ctx, keyObject->keyId, &exists);
    ENSURE_OR_GO_EXIT(status == SM_OK);

    if (exists == kSE05x_Result_SUCCESS) {
        /* Check if object is of same curve id */
        status = Se05x_API_EC_CurveGetId(&keyObject->keyStore->session->s_ctx, keyObject->keyId, &retCurveId);
        ENSURE_OR_GO_EXIT(status == SM_OK);

        if (retCurveId == keyObject->curve_id) {
            curveId = kSE05x_ECCurve_NA;
        }
        else {
            LOG_W("Cannot overwrite object with different curve id");
            goto exit;
        }

        //if (se05x_policy.value_len != 0) {
        //    LOG_W("Policy + Existing Key is not a valid combination");
        //}
    }
    else {
        curveId = keyObject->curve_id;
    }

    if (keyObject->objectType == kSSS_KeyPart_Pair) {
        const uint8_t *pPrivateKey = NULL;
        const uint8_t *pPublicKey  = NULL;
        size_t privateKeyLen       = 0;
        size_t publicKeyLen        = 0;
        uint16_t privateKeyIndex   = 0;
        uint16_t publicKeyIndex    = 0;
        if (exists == kSE05x_Result_FAILURE) {
            key_part = kSE05x_KeyPart_Pair;
        }

        switch (keyObject->curve_id) {
#if SSS_HAVE_TPM_BN
        case kSE05x_ECCurve_TPM_ECC_BN_P256: {
            LOG_I("Key pair should be paased without header");
            /* No header included in ED and BN curve keys */
            privateKeyIndex = 0;
            publicKeyIndex  = 32;
            privateKeyLen   = 32;
            publicKeyLen    = 32;
        } break;
#endif

        default: {
#if SSS_HAVE_EC_MONT || SSS_HAVE_EC_ED
            if ((keyObject->curve_id == kSE05x_ECCurve_ECC_MONT_DH_25519) ||
                (keyObject->curve_id == kSE05x_ECCurve_ECC_MONT_DH_448) ||
                (keyObject->curve_id == kSE05x_ECCurve_ECC_ED_25519)) {
                asn_retval = sss_util_rfc8410_asn1_get_ec_pair_key_index(
                    key, keyLen, &publicKeyIndex, &publicKeyLen, &privateKeyIndex, &privateKeyLen);
                if (asn_retval != kStatus_SSS_Success) {
                    LOG_W("error in sss_util_rfc8410_asn1_get_ec_pair_key_index");
                    goto exit;
                }
            }
            else
#endif // SSS_HAVE_EC_MONT || SSS_HAVE_EC_ED
            {
                asn_retval = sss_util_pkcs8_asn1_get_ec_pair_key_index(
                    key, keyLen, &publicKeyIndex, &publicKeyLen, &privateKeyIndex, &privateKeyLen);
                if (asn_retval != kStatus_SSS_Success) {
                    LOG_W("error in sss_util_pkcs8_asn1_get_ec_pair_key_index");
                    goto exit;
                }
            }

            asn_retval = getEccPrivPubKeyLen((uint32_t)keyObject->curve_id, &std_pubKey_len, &std_privKey_len);
            if (asn_retval != kStatus_SSS_Success) {
                LOG_W("error in getEccPrivPubKeyLen");
                goto exit;
            }

            if (privateKeyLen != std_privKey_len) {
                if (key[privateKeyIndex] == 0) {
                    privateKeyIndex++;
                    privateKeyLen--;
                }
            }
            if (privateKeyLen != std_privKey_len) {
                LOG_W("error in private key length");
                goto exit;
            }

            if (publicKeyLen != std_pubKey_len) {
                if (key[publicKeyIndex] == 0) {
                    publicKeyIndex++;
                    publicKeyLen--;
                }
            }
            if (publicKeyLen != std_pubKey_len) {
                LOG_W("error in public key length");
                goto exit;
            }
        }
        }

        // Conditionally Reverse Endianness
#if SSS_HAVE_EC_MONT || SSS_HAVE_EC_ED
        if ((keyObject->curve_id == kSE05x_ECCurve_ECC_MONT_DH_25519) ||
            (keyObject->curve_id == kSE05x_ECCurve_ECC_MONT_DH_448) ||
            (keyObject->curve_id == kSE05x_ECCurve_ECC_ED_25519)) {
            size_t i        = 0;
            size_t nByteKey = 32; // Corresponds to kSE05x_ECCurve_ECC_MONT_DH_25519

            if (keyObject->curve_id == kSE05x_ECCurve_ECC_MONT_DH_448) {
                nByteKey = 56;
            }

            if (keyObject->curve_id != kSE05x_ECCurve_ECC_ED_25519) {
                while (i < nByteKey) {
                    privKeyReversed[i] = key[privateKeyIndex + privateKeyLen - i - 1];
                    i++;
                }
                pPrivateKey = &privKeyReversed[0];
            }
            else {
                // SE05x expects private key to be in litte endian format
                pPrivateKey = &key[privateKeyIndex];
            }
            i = 0;
            while (i < nByteKey) {
                pubKeyReversed[i] = key[publicKeyIndex + publicKeyLen - i - 1];
                i++;
            }
            pPublicKey = &pubKeyReversed[0];
        }
        else
#endif // SSS_HAVE_EC_MONT || SSS_HAVE_EC_ED
        {
            pPrivateKey = &key[privateKeyIndex];
            pPublicKey  = &key[publicKeyIndex];
        }

#ifdef TMP_ENDIAN_VERBOSE
        {
            printf("Private Key After Reverse:\n");
            for (size_t z = 0; z < privateKeyLen; z++) {
                printf("%02X.", pPrivateKey[z]);
            }
            printf("\n");
        }
#endif

        status = sss_se05x_LL_set_ec_key(&keyStore->session->s_ctx,
            &se05x_policy,
            SE05x_MaxAttemps_UNLIMITED,
            keyObject->keyId,
            curveId,
            pPrivateKey,
            privateKeyLen,
            pPublicKey,
            publicKeyLen,
            transient_type,
            key_part,
            exists);
        ENSURE_OR_GO_EXIT(status == SM_OK);
    }
    else if (keyObject->objectType == kSSS_KeyPart_Public) {
        const uint8_t *pPublicKey = NULL;
        size_t publicKeyLen       = 0;
        uint16_t publicKeyIndex   = 0;
        if (exists == kSE05x_Result_FAILURE) {
            key_part = kSE05x_KeyPart_Public;
        }

        switch (keyObject->curve_id) {
#if SSS_HAVE_TPM_BN
        case kSE05x_ECCurve_TPM_ECC_BN_P256: {
            LOG_I("Public key should be paased without header");
            publicKeyLen = keyLen;
        } break;
#endif
        default: {
            asn_retval = sss_util_pkcs8_asn1_get_ec_public_key_index(key, keyLen, &publicKeyIndex, &publicKeyLen);
            if (asn_retval != kStatus_SSS_Success) {
                LOG_W("error in sss_util_pkcs8_asn1_get_ec_public_key_index");
                goto exit;
            }

            asn_retval = getEccPrivPubKeyLen((uint32_t)keyObject->curve_id, &std_pubKey_len, &std_privKey_len);
            if (asn_retval != kStatus_SSS_Success) {
                LOG_W("error in getEccPrivPubKeyLen");
                goto exit;
            }

            if (publicKeyLen != std_pubKey_len) {
                if (key[publicKeyIndex] == 0) {
                    publicKeyIndex++;
                    publicKeyLen--;
                }
            }
            if (publicKeyLen != std_pubKey_len) {
                LOG_W("error in public key length");
                goto exit;
            }
        }
        }

#ifdef TMP_ENDIAN_VERBOSE
        {
            printf("Pub Key Before Reverse:\n");
            for (size_t z = 0; z < publicKeyLen; z++) {
                printf("%02X.", key[publicKeyIndex + z]);
            }
            printf("\n");
        }
#endif

        // Conditionally Reverse Endianness
#if SSS_HAVE_EC_MONT || SSS_HAVE_EC_ED
        if ((keyObject->curve_id == kSE05x_ECCurve_ECC_MONT_DH_25519) ||
            (keyObject->curve_id == kSE05x_ECCurve_ECC_MONT_DH_448) ||
            (keyObject->curve_id == kSE05x_ECCurve_ECC_ED_25519)) {
            size_t i        = 0;
            size_t nByteKey = 32; // Corresponds to kSE05x_ECCurve_ECC_MONT_DH_25519

            if (keyObject->curve_id == kSE05x_ECCurve_ECC_MONT_DH_448) {
                nByteKey = 56;
            }

            while (i < nByteKey) {
                pubKeyReversed[i] = key[publicKeyIndex + publicKeyLen - i - 1];
                i++;
            }
            pPublicKey = &pubKeyReversed[0];
        }
        else
#endif // SSS_HAVE_EC_MONT || SSS_HAVE_EC_ED
        {
            pPublicKey = &key[publicKeyIndex];
        }

#ifdef TMP_ENDIAN_VERBOSE
        {
            printf("Pub Key After Reverse:\n");
            for (size_t z = 0; z < publicKeyLen; z++) {
                printf("%02X.", pPublicKey[z]);
            }
            printf("\n");
        }
#endif

        status = sss_se05x_LL_set_ec_key(&keyStore->session->s_ctx,
            &se05x_policy,
            SE05x_MaxAttemps_NA,
            keyObject->keyId,
            curveId,
            NULL,
            0,
            pPublicKey,
            publicKeyLen,
            transient_type,
            key_part,
            exists);

        ENSURE_OR_GO_EXIT(status == SM_OK);
    }
    else if (keyObject->objectType == kSSS_KeyPart_Private) {
        const uint8_t *pPrivKey  = NULL;
        size_t privKeyLen        = (uint16_t)keyLen;
        uint16_t privateKeyIndex = 0;
        if (exists == kSE05x_Result_FAILURE) {
            key_part = kSE05x_KeyPart_Private;
        }

        LOG_I("Private key should be passed without header");

        switch (keyObject->curve_id) {
#if SSS_HAVE_TPM_BN
        case kSE05x_ECCurve_TPM_ECC_BN_P256: {
            privateKeyIndex = 0;
        } break;
#endif
#if SSS_HAVE_SE05X_VER_GTE_06_00 && SSS_HAVE_EC_MONT
        case kSE05x_ECCurve_RESERVED_ID_ECC_MONT_DH_448: {
            LOG_W(
                "Private Key injection is not supported for "
                "ECC_MONT_DH_448 curve");
            goto exit;
        }
#endif
        default: {
            asn_retval = getEccPrivPubKeyLen((uint32_t)keyObject->curve_id, &std_pubKey_len, &std_privKey_len);
            if (asn_retval != kStatus_SSS_Success) {
                LOG_W("error in getEccPrivPubKeyLen");
                goto exit;
            }

            if (keyLen != std_privKey_len) {
                if (key[0] == 0) {
                    privKeyLen      = keyLen - 1;
                    privateKeyIndex = 1;
                }
            }
            if (privKeyLen != std_privKey_len) {
                LOG_W("error in private key length");
                goto exit;
            }
        } break;
        }

        pPrivKey = &key[privateKeyIndex];

        status = sss_se05x_LL_set_ec_key(&keyStore->session->s_ctx,
            &se05x_policy,
            SE05x_MaxAttemps_NA,
            keyObject->keyId,
            curveId,
            pPrivKey,
            privKeyLen,
            NULL,
            0,
            transient_type,
            key_part,
            exists);
        ENSURE_OR_GO_EXIT(status == SM_OK);
    }
    else {
        goto exit;
    }

    retval = kStatus_SSS_Success;
exit:
    return retval;
}
#endif // SSSFTR_SE05X_ECC && SSSFTR_SE05X_KEY_SET

#if SSSFTR_SE05X_AES && SSSFTR_SE05X_KEY_SET
static sss_status_t sss_se05x_key_store_set_aes_key(sss_se05x_key_store_t *keyStore,
    sss_se05x_object_t *keyObject,
    const uint8_t *key,
    size_t keyLen,
    size_t keyBitLen,
    void *policy_buff,
    size_t policy_buff_len)
{
    sss_status_t retval = kStatus_SSS_Fail;
    smStatus_t status   = SM_NOT_OK;
    Se05xPolicy_t se05x_policy;
    SE05x_INS_t transient_type;
    SE05x_SymmKeyType_t type = kSE05x_SymmKeyType_NA;
    SE05x_KeyID_t kekID      = SE05x_KeyID_KEK_NONE;
    uint8_t IdExists         = 0;
    SE05x_Result_t objExists = kSE05x_Result_NA;

    /* Assign proper instruction type based on keyObject->isPersistant  */
    (keyObject->isPersistant) ? (transient_type = kSE05x_INS_NA) : (transient_type = kSE05x_INS_TRANSIENT);

    IdExists  = CheckIfKeyIdExists(keyObject->keyId, &keyStore->session->s_ctx);
    objExists = (IdExists == 1) ? kSE05x_Result_SUCCESS : kSE05x_Result_FAILURE;

    se05x_policy.value     = (uint8_t *)policy_buff;
    se05x_policy.value_len = policy_buff_len;

    if (keyBitLen % 8 == 0) {
        if (keyObject->cipherType == kSSS_CipherType_AES) {
            type = kSE05x_SymmKeyType_AES;
        }
        else if (keyObject->cipherType == kSSS_CipherType_CMAC) {
            type = kSE05x_SymmKeyType_CMAC;
        }
        else if (keyObject->cipherType == kSSS_CipherType_HMAC) {
            type = kSE05x_SymmKeyType_HMAC;
        }

        if (keyStore->kekKey != NULL) {
            kekID = keyStore->kekKey->keyId;
        }
        status = sss_se05x_LL_set_symm_key(&keyStore->session->s_ctx,
            &se05x_policy,
            SE05x_MaxAttemps_NA,
            keyObject->keyId,
            kekID,
            key,
            keyLen,
            transient_type,
            type,
            objExists);
        ENSURE_OR_GO_EXIT(status == SM_OK);
    }
    else {
        goto exit;
    }
    retval = kStatus_SSS_Success;
exit:
    return retval;
}
#endif // SSSFTR_SE05X_AES && SSSFTR_SE05X_KEY_SET

#if SSSFTR_SE05X_KEY_SET
static sss_status_t sss_se05x_key_store_set_des_key(sss_se05x_key_store_t *keyStore,
    sss_se05x_object_t *keyObject,
    const uint8_t *key,
    size_t keyLen,
    size_t keyBitLen,
    void *policy_buff,
    size_t policy_buff_len)
{
    AX_UNUSED_ARG(keyBitLen);
    sss_status_t retval = kStatus_SSS_Fail;
    smStatus_t status   = SM_NOT_OK;
    Se05xPolicy_t se05x_policy;
    SE05x_INS_t transient_type;
    SE05x_KeyID_t kekID      = SE05x_KeyID_KEK_NONE;
    uint8_t IdExists         = 0;
    SE05x_Result_t objExists = kSE05x_Result_NA;

    /* Assign proper instruction type based on keyObject->isPersistant  */
    (keyObject->isPersistant) ? (transient_type = kSE05x_INS_NA) : (transient_type = kSE05x_INS_TRANSIENT);
    IdExists = CheckIfKeyIdExists(keyObject->keyId, &keyStore->session->s_ctx);

    objExists              = (IdExists == 1) ? kSE05x_Result_SUCCESS : kSE05x_Result_FAILURE;
    se05x_policy.value     = (uint8_t *)policy_buff;
    se05x_policy.value_len = policy_buff_len;

    if (keyStore->kekKey != NULL) {
        kekID = keyStore->kekKey->keyId;
    }

    status = sss_se05x_LL_set_symm_key(&keyStore->session->s_ctx,
        &se05x_policy,
        SE05x_MaxAttemps_NA,
        keyObject->keyId,
        kekID,
        key,
        keyLen,
        transient_type,
        kSE05x_SymmKeyType_DES,
        objExists);
    ENSURE_OR_GO_EXIT(status == SM_OK);

    retval = kStatus_SSS_Success;
exit:
    return retval;
}
#endif // SSSFTR_SE05X_KEY_SET

#if 0
static sss_status_t sss_se05x_key_store_set_deswrapped_key(
    sss_se05x_key_store_t *keyStore,
    sss_se05x_object_t *keyObject,
    const uint8_t *key,
    size_t keyLen,
    size_t keyBitLen,
    void *policy_buff,
    size_t policy_buff_len)
{
    sss_status_t retval = kStatus_SSS_Fail;
    smStatus_t status = SM_NOT_OK;
    Se05xPolicy_t se05x_policy;

    se05x_policy.value = (uint8_t *)policy_buff;
    se05x_policy.value_len = policy_buff_len;

    if (keyObject->isPersistant) {
        status = Se05x_API_DES_SetNewWrapped_P(&keyStore->session->s_ctx,
            &se05x_policy,
            keyObject->keyId,
            keyObject->kekId,
            (U16)keyBitLen,
            key,
            keyLen);
    }
    else {
        status = Se05x_API_DES_SetNewWrapped_T(&keyStore->session->s_ctx,
            &se05x_policy,
            keyObject->keyId,
            keyObject->kekId,
            (U16)keyBitLen,
            key,
            keyLen);
    }
    ENSURE_OR_GO_EXIT(status == SM_OK);

    retval = kStatus_SSS_Success;
exit:
    return retval;
}

#endif

#if SSSFTR_SE05X_KEY_SET
static sss_status_t sss_se05x_key_store_set_cert(sss_se05x_key_store_t *keyStore,
    sss_se05x_object_t *keyObject,
    const uint8_t *key,
    size_t keyLen,
    size_t keyBitLen,
    void *policy_buff,
    size_t policy_buff_len)
{
    AX_UNUSED_ARG(keyBitLen);
    sss_status_t retval = kStatus_SSS_Fail;
    smStatus_t status   = SM_NOT_OK;
    Se05xPolicy_t se05x_policy;
#if !SSS_HAVE_SE05X_VER_GTE_06_00
    Se05xPolicy_t *ppolicy = &se05x_policy;
#endif
    uint16_t data_rem;
    uint16_t offset   = 0;
    uint16_t fileSize = 0;
    uint8_t IdExists  = 0;
#if SSS_HAVE_SE05X_VER_GTE_06_00
    SE05x_Result_t obj_exists = kSE05x_Result_NA;
#endif

    ENSURE_OR_GO_EXIT(keyLen < 0xFFFFu);

    IdExists = CheckIfKeyIdExists(keyObject->keyId, &keyStore->session->s_ctx);
    fileSize = (IdExists == 1) ? 0 : (uint16_t)keyLen;
    data_rem = (uint16_t)keyLen;

    se05x_policy.value     = (uint8_t *)policy_buff;
    se05x_policy.value_len = policy_buff_len;

    while (data_rem > 0) {
        uint16_t chunk = (data_rem > BINARY_WRITE_MAX_LEN) ? BINARY_WRITE_MAX_LEN : data_rem;
        data_rem       = data_rem - chunk;

#if SSS_HAVE_SE05X_VER_GTE_06_00
        /* Call APIs For SE051 */
        obj_exists = (IdExists == 1) ? kSE05x_Result_SUCCESS : kSE05x_Result_FAILURE;
        if (obj_exists == kSE05x_Result_FAILURE && offset == 0) {
            status = Se05x_API_WriteBinary_Ver(&keyStore->session->s_ctx,
                &se05x_policy,
                keyObject->keyId,
                offset,
                (uint16_t)fileSize,
                (key + offset),
                chunk,
                0);
        }
        else {
            status = Se05x_API_UpdateBinary_Ver(&keyStore->session->s_ctx,
                &se05x_policy,
                keyObject->keyId,
                offset,
                (uint16_t)fileSize,
                (key + offset),
                chunk,
                0);
        }

#else
        /* Call APIs For SE050 */
        status = Se05x_API_WriteBinary(
            &keyStore->session->s_ctx, ppolicy, keyObject->keyId, offset, (uint16_t)fileSize, (key + offset), chunk);
        ppolicy = NULL;
#endif
        ENSURE_OR_GO_EXIT(status == SM_OK);

        fileSize = 0;
        offset   = offset + chunk;
    }
    retval = kStatus_SSS_Success;
exit:
    return retval;
}
#endif // SSSFTR_SE05X_KEY_SET

#if 0
static sss_status_t sss_se05x_key_store_set_pcr(
    sss_se05x_key_store_t *keyStore,
    sss_se05x_object_t *keyObject,
    const uint8_t *key,
    size_t keyLen,
    void *policy_buff,
    size_t policy_buff_len)
{
    sss_status_t retval = kStatus_SSS_Fail;
    smStatus_t status = SM_NOT_OK;
    Se05xPolicy_t se05x_policy;

    se05x_policy.value = (uint8_t *)policy_buff;
    se05x_policy.value_len = policy_buff_len;

    if (keyObject->cipherType == kSSS_CipherType_PCR) {
        status = Se05x_API_WritePCR_WithType(&keyStore->session->s_ctx,
            kSE05x_INS_NA,
            &se05x_policy,
            keyObject->keyId,
            key,
            keyLen,
            NULL,
            0);
    }
    else if (keyObject->cipherType == kSSS_CipherType_Update_PCR) {
        status = Se05x_API_WritePCR_WithType(&keyStore->session->s_ctx,
            kSE05x_INS_NA,
            &se05x_policy,
            keyObject->keyId,
            NULL,
            0,
            key,
            keyLen
        );
    }
    else if (keyObject->cipherType == kSSS_CipherType_Reset_PCR) {
        status = Se05x_API_WritePCR_WithType(&keyStore->session->s_ctx,
            kSE05x_INS_NA,
            &se05x_policy,
            keyObject->keyId,
            NULL,
            0,
            NULL,
            0);
    }
    else
    {
        goto exit;
    }

    ENSURE_OR_GO_EXIT(status == SM_OK);

    retval = kStatus_SSS_Success;
exit:
    return retval;
}
#endif

sss_status_t sss_se05x_key_store_set_key(sss_se05x_key_store_t *keyStore,
    sss_se05x_object_t *keyObject,
    const uint8_t *key,
    size_t keyLen,
    size_t keyBitLen,
    void *options,
    size_t optionsLen)
{
    AX_UNUSED_ARG(optionsLen);
    sss_status_t retval = kStatus_SSS_Fail;

#if SSSFTR_SE05X_KEY_SET

    sss_cipher_type_t cipher_type = kSSS_CipherType_NONE;
    sss_policy_t *policies        = (sss_policy_t *)options;
    uint8_t *ppolicySet;
    size_t valid_policy_buff_len                  = 0;
    uint8_t policies_buff[MAX_POLICY_BUFFER_SIZE] = {
        0,
    };

    ENSURE_OR_GO_EXIT(keyStore);
    ENSURE_OR_GO_EXIT(keyObject);

#if SSS_HAVE_TPM_BN && SSS_HAVE_ECDAA && SSS_HAVE_SE05X_VER_GTE_07_02
    if ((keyObject->keyId & SSS_SE05X_RESID_ECDAA_RANDOM_KEY_MASK) == SSS_SE05X_RESID_ECDAA_RANDOM_KEY_START) {
        LOG_W("Could not set key %X. It's reserved for ECDAA random key.", keyObject->keyId);
        goto exit;
    }
#endif

    if (keyBitLen) {
        ENSURE_OR_GO_EXIT(key);
    }
    cipher_type = (sss_cipher_type_t)keyObject->cipherType;

    if (policies) {
        if (kStatus_SSS_Success !=
            sss_se05x_create_object_policy_buffer(policies, &policies_buff[0], &valid_policy_buff_len)) {
            goto exit;
        }
        ppolicySet = policies_buff;
    }
    else {
        ppolicySet = NULL;
    }

    switch (cipher_type) {
#if SSSFTR_SE05X_RSA && SSS_HAVE_RSA
    case kSSS_CipherType_RSA:
    case kSSS_CipherType_RSA_CRT:
        if (kStatus_SSS_Success !=
            sss_se05x_key_store_set_rsa_key(
                keyStore, keyObject, key, keyLen, keyBitLen, ppolicySet, valid_policy_buff_len)) {
            goto exit;
        }
        break;
#endif
#if SSSFTR_SE05X_ECC
    case kSSS_CipherType_EC_NIST_P:
#if SSS_HAVE_EC_NIST_K
    case kSSS_CipherType_EC_NIST_K:
#endif
#if SSS_HAVE_EC_BP
    case kSSS_CipherType_EC_BRAINPOOL:
#endif
#if SSS_HAVE_EC_MONT
    case kSSS_CipherType_EC_MONTGOMERY:
#endif
#if SSS_HAVE_EC_ED
    case kSSS_CipherType_EC_TWISTED_ED:
#endif
#if SSS_HAVE_TPM_BN
    case kSSS_CipherType_EC_BARRETO_NAEHRIG:
#endif
        if (kStatus_SSS_Success !=
            sss_se05x_key_store_set_ecc_key(
                keyStore, keyObject, key, keyLen, keyBitLen, ppolicySet, valid_policy_buff_len)) {
            goto exit;
        }
        break;
#endif // SSSFTR_SE05X_ECC
    case kSSS_CipherType_AES:
        if ((keyLen != 16 && keyLen != 24 && keyLen != 32 && keyLen != 40)) {
            goto exit;
        }
        /* fall through */
    case kSSS_CipherType_CMAC:
    case kSSS_CipherType_HMAC:
#if SSSFTR_SE05X_AES && SSSFTR_SE05X_KEY_SET
        if (kStatus_SSS_Success !=
            sss_se05x_key_store_set_aes_key(
                keyStore, keyObject, key, keyLen, keyBitLen, ppolicySet, valid_policy_buff_len)) {
            goto exit;
        }
#else
        goto exit;
#endif
        break;
    case kSSS_CipherType_DES:
        if (kStatus_SSS_Success !=
            sss_se05x_key_store_set_des_key(
                keyStore, keyObject, key, keyLen, keyBitLen, ppolicySet, valid_policy_buff_len)) {
            goto exit;
        }
        break;
    case kSSS_CipherType_Binary:
    case kSSS_CipherType_Certificate: {
        if (kStatus_SSS_Success !=
            sss_se05x_key_store_set_cert(
                keyStore, keyObject, key, keyLen, keyBitLen, ppolicySet, valid_policy_buff_len)) {
            goto exit;
        }
    } break;
    default:
        goto exit;
    }
    retval = kStatus_SSS_Success;
exit:
#endif /* SSSFTR_SE05X_KEY_SET */
    return retval;
}

sss_status_t sss_se05x_key_store_generate_key(
    sss_se05x_key_store_t *keyStore, sss_se05x_object_t *keyObject, size_t keyBitLen, void *options)
{
    sss_status_t retval = kStatus_SSS_Fail;

#if SSSFTR_SE05X_KEY_SET
    smStatus_t status      = SM_NOT_OK;
    sss_policy_t *policies = (sss_policy_t *)options;
    uint8_t *ppolicySet;
    size_t valid_policy_buff_len = 0;
    Se05xPolicy_t se05x_policy;
    SE05x_INS_t transient_type;
    uint8_t IdExists = 0;
    uint8_t policies_buff[MAX_POLICY_BUFFER_SIZE];
    ENSURE_OR_GO_EXIT(keyStore);
    ENSURE_OR_GO_EXIT(keyObject);

#if SSS_HAVE_TPM_BN && SSS_HAVE_ECDAA && SSS_HAVE_SE05X_VER_GTE_07_02
    if ((keyObject->keyId & SSS_SE05X_RESID_ECDAA_RANDOM_KEY_MASK) == SSS_SE05X_RESID_ECDAA_RANDOM_KEY_START) {
        LOG_W("Could not generate key %X. It's reserved for ECDAA random key.", keyObject->keyId);
        goto exit;
    }
#endif

    if (policies) {
        if (kStatus_SSS_Success !=
            sss_se05x_create_object_policy_buffer(policies, &policies_buff[0], &valid_policy_buff_len)) {
            goto exit;
        }
        ppolicySet = policies_buff;
    }
    else {
        ppolicySet = NULL;
    }
    se05x_policy.value     = (uint8_t *)ppolicySet;
    se05x_policy.value_len = valid_policy_buff_len;

    /* Assign proper instruction type based on keyObject->isPersistant  */
    (keyObject->isPersistant) ? (transient_type = kSE05x_INS_NA) : (transient_type = kSE05x_INS_TRANSIENT);

    ENSURE_OR_GO_EXIT(keyObject->objectType == kSSS_KeyPart_Pair);

    switch (keyObject->cipherType) {
#if SSSFTR_SE05X_ECC
    case kSSS_CipherType_EC_NIST_P:
#if SSS_HAVE_EC_NIST_K
    case kSSS_CipherType_EC_NIST_K:
#endif
#if SSS_HAVE_EC_BP
    case kSSS_CipherType_EC_BRAINPOOL:
#endif
#if SSS_HAVE_EC_MONT
    case kSSS_CipherType_EC_MONTGOMERY:
#endif
#if SSS_HAVE_TPM_BN
    case kSSS_CipherType_EC_BARRETO_NAEHRIG:
#endif
#if SSS_HAVE_EC_ED
    case kSSS_CipherType_EC_TWISTED_ED:
#endif
    {
        SE05x_ECCurve_t curve_id;
        if (keyObject->curve_id == kSE05x_ECCurve_NA) {
            keyObject->curve_id =
                (SE05x_ECCurve_t)se05x_sssKeyTypeLenToCurveId((sss_cipher_type_t)keyObject->cipherType, keyBitLen);
        }

        if (keyObject->curve_id == kSE05x_ECCurve_NA) {
            goto exit;
        }

        status = sss_se05x_create_curve_if_needed(&keyObject->keyStore->session->s_ctx, keyObject->curve_id);

        IdExists = CheckIfKeyIdExists(keyObject->keyId, &keyStore->session->s_ctx);
        curve_id = (IdExists == 1) ? kSE05x_ECCurve_NA : (SE05x_ECCurve_t)keyObject->curve_id;

        status = Se05x_API_WriteECKey(&keyStore->session->s_ctx,
            &se05x_policy,
            SE05x_MaxAttemps_NA,
            keyObject->keyId,
            curve_id,
            NULL,
            0,
            NULL,
            0,
            transient_type,
            kSE05x_KeyPart_Pair);
        ENSURE_OR_GO_EXIT(status == SM_OK);

#if SSS_HAVE_TPM_BN && SSS_HAVE_ECDAA && SSS_HAVE_SE05X_VER_GTE_07_02
        // Create ECDAA Random key.
        if (keyObject->cipherType == kSSS_CipherType_EC_BARRETO_NAEHRIG) {
            sss_status_t sss_status = kStatus_SSS_Fail;
            uint32_t random_keyId;
            LOG_D("Generate ECDAA Random Key");
            sss_status = sss_se05x_generate_ecdaa_random_key(&keyStore->session->s_ctx, &random_keyId);
            ENSURE_OR_GO_EXIT(sss_status == kStatus_SSS_Success);
        }
#endif
        break;
    }
#endif // < SSSFTR_SE05X_ECC
#if SSSFTR_SE05X_RSA && SSS_HAVE_RSA
    case kSSS_CipherType_RSA:
    case kSSS_CipherType_RSA_CRT: {
        /* Hard Coded Public exponent to be 65537 */
        //uint8_t pubexp[] = {0x01, 0x00, 0x01};
        SE05x_KeyPart_t key_part = kSE05x_KeyPart_Pair;
        SE05x_RSAKeyFormat_t rsa_format;
        size_t keyBitLength = 0;
        if (keyObject->cipherType == kSSS_CipherType_RSA) {
            rsa_format = kSE05x_RSAKeyFormat_RAW;
        }
        else if (keyObject->cipherType == kSSS_CipherType_RSA_CRT) {
            rsa_format = kSE05x_RSAKeyFormat_CRT;
        }
        else {
            retval = kStatus_SSS_Fail;
            goto exit;
        }

        IdExists     = CheckIfKeyIdExists(keyObject->keyId, &keyStore->session->s_ctx);
        keyBitLength = (IdExists == 1) ? 0 : keyBitLen;

        status = Se05x_API_WriteRSAKey(&keyStore->session->s_ctx,
            &se05x_policy,
            keyObject->keyId,
            (uint16_t)keyBitLength,
            SE05X_RSA_NO_p,
            SE05X_RSA_NO_q,
            SE05X_RSA_NO_dp,
            SE05X_RSA_NO_dq,
            SE05X_RSA_NO_qInv,
            SE05X_RSA_NO_pubExp,
            SE05X_RSA_NO_priv,
            SE05X_RSA_NO_pubMod,
            transient_type,
            key_part,
            rsa_format);

        ENSURE_OR_GO_EXIT(status == SM_OK);
        break;
    }
#endif // SSSFTR_SE05X_RSA && SSS_HAVE_RSA
    default: {
        goto exit;
    }
    }

    retval = kStatus_SSS_Success;
exit:
#endif // SSSFTR_SE05X_KEY_SET
    return retval;
}

#define ADD_DER_ECC_NISTP192_HEADER(x) ((x) + der_ecc_nistp192_header_len)
#define REMOVE_DER_ECC_NISTP192_HEADER(x) ((x)-der_ecc_nistp192_header_len)

#define ADD_DER_ECC_NISTP224_HEADER(x) ((x) + der_ecc_nistp224_header_len)
#define REMOVE_DER_ECC_NISTP224_HEADER(x) ((x)-der_ecc_nistp224_header_len)

#define ADD_DER_ECC_NISTP256_HEADER(x) ((x) + der_ecc_nistp256_header_len)
#define REMOVE_DER_ECC_NISTP256_HEADER(x) ((x)-der_ecc_nistp256_header_len)

#define ADD_DER_ECC_NISTP384_HEADER(x) ((x) + der_ecc_nistp384_header_len)
#define REMOVE_DER_ECC_NISTP384_HEADER(x) ((x)-der_ecc_nistp384_header_len)

#define ADD_DER_ECC_NISTP521_HEADER(x) ((x) + der_ecc_nistp521_header_len)
#define REMOVE_DER_ECC_NISTP521_HEADER(x) ((x)-der_ecc_nistp521_header_len)

#define ADD_DER_ECC_160K_HEADER(x) ((x) + der_ecc_160k_header_len)
#define REMOVE_DER_ECC_160K_HEADER(x) ((x)-der_ecc_160k_header_len)

#define ADD_DER_ECC_192K_HEADER(x) ((x) + der_ecc_192k_header_len)
#define REMOVE_DER_ECC_192K_HEADER(x) ((x)-der_ecc_192k_header_len)

#define ADD_DER_ECC_224K_HEADER(x) ((x) + der_ecc_224k_header_len)
#define REMOVE_DER_ECC_224K_HEADER(x) ((x)-der_ecc_224k_header_len)

#define ADD_DER_ECC_256K_HEADER(x) ((x) + der_ecc_256k_header_len)
#define REMOVE_DER_ECC_256K_HEADER(x) ((x)-der_ecc_256k_header_len)

#define ADD_DER_ECC_BP160_HEADER(x) ((x) + der_ecc_bp160_header_len)
#define REMOVE_DER_ECC_BP160_HEADER(x) ((x)-der_ecc_bp160_header_len)

#define ADD_DER_ECC_BP192_HEADER(x) ((x) + der_ecc_bp192_header_len)
#define REMOVE_DER_ECC_BP192_HEADER(x) ((x)-der_ecc_bp192_header_len)

#define ADD_DER_ECC_BP224_HEADER(x) ((x) + der_ecc_bp224_header_len)
#define REMOVE_DER_ECC_BP224_HEADER(x) ((x)-der_ecc_bp224_header_len)

#define ADD_DER_ECC_BP320_HEADER(x) ((x) + der_ecc_bp320_header_len)
#define REMOVE_DER_ECC_BP320_HEADER(x) ((x)-der_ecc_bp320_header_len)

#define ADD_DER_ECC_BP384_HEADER(x) ((x) + der_ecc_bp384_header_len)
#define REMOVE_DER_ECC_BP384_HEADER(x) ((x)-der_ecc_bp384_header_len)

#define ADD_DER_ECC_BP256_HEADER(x) ((x) + der_ecc_bp256_header_len)
#define REMOVE_DER_ECC_BP256_HEADER(x) ((x)-der_ecc_bp256_header_len)

#define ADD_DER_ECC_BP512_HEADER(x) ((x) + der_ecc_bp512_header_len)
#define REMOVE_DER_ECC_BP512_HEADER(x) ((x)-der_ecc_bp512_header_len)

#define ADD_DER_ECC_MONT_DH_448_HEADER(x) ((x) + der_ecc_mont_dh_448_header_len)
#define REMOVE_DER_ECC_MONT_DH_448_HEADER(x) ((x)-der_ecc_mont_dh_448_header_len)
#define ADD_DER_ECC_MONT_DH_25519_HEADER(x) ((x) + der_ecc_mont_dh_25519_header_len)
#define REMOVE_DER_ECC_MONT_DH_25519_HEADER(x) ((x)-der_ecc_mont_dh_25519_header_len)

#define ADD_DER_ECC_TWISTED_ED_25519_HEADER(x) ((x) + der_ecc_twisted_ed_25519_header_len)
#define REMOVE_DER_ECC_TWISTED_ED_25519_HEADER(x) ((x)-der_ecc_twisted_ed_25519_header_len)

#define CONVERT_BYTE(x) ((x) / 8)
#define CONVERT_BIT(x) ((x)*8)

void add_ecc_header(uint8_t *key, size_t *keylen, uint8_t **key_buf, size_t *key_buflen, uint32_t curve_id)
{
    if (key == NULL || key_buf == NULL || key_buflen == NULL) {
        goto exit;
    }
#if SSSFTR_SE05X_KEY_SET
    if (curve_id == kSE05x_ECCurve_NIST_P256) {
        ENSURE_OR_GO_EXIT((*keylen) > der_ecc_nistp256_header_len);
        memcpy(key, gecc_der_header_nist256, der_ecc_nistp256_header_len);
        *key_buf    = ADD_DER_ECC_NISTP256_HEADER(key);
        *key_buflen = (uint16_t)ADD_DER_ECC_NISTP256_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_NIST_P384) {
        ENSURE_OR_GO_EXIT((*keylen) > der_ecc_nistp384_header_len);
        memcpy(key, gecc_der_header_nist384, der_ecc_nistp384_header_len);
        *key_buf    = ADD_DER_ECC_NISTP384_HEADER(key);
        *key_buflen = (uint16_t)ADD_DER_ECC_NISTP384_HEADER(*key_buflen);
    }
#if SSS_HAVE_EC_NIST_192
    else if (curve_id == kSE05x_ECCurve_NIST_P192) {
        ENSURE_OR_GO_EXIT((*keylen) > der_ecc_nistp192_header_len);
        memcpy(key, gecc_der_header_nist192, der_ecc_nistp192_header_len);
        *key_buf    = ADD_DER_ECC_NISTP192_HEADER(key);
        *key_buflen = (uint16_t)ADD_DER_ECC_NISTP192_HEADER(*key_buflen);
    }
#endif
#if SSS_HAVE_EC_NIST_224
    else if (curve_id == kSE05x_ECCurve_NIST_P224) {
        ENSURE_OR_GO_EXIT((*keylen) > der_ecc_nistp224_header_len);
        memcpy(key, gecc_der_header_nist224, der_ecc_nistp224_header_len);
        *key_buf    = ADD_DER_ECC_NISTP224_HEADER(key);
        *key_buflen = (uint16_t)ADD_DER_ECC_NISTP224_HEADER(*key_buflen);
    }
#endif
#if SSS_HAVE_EC_NIST_521
    else if (curve_id == kSE05x_ECCurve_NIST_P521) {
        ENSURE_OR_GO_EXIT((*keylen) > der_ecc_nistp521_header_len);
        memcpy(key, gecc_der_header_nist521, der_ecc_nistp521_header_len);
        *key_buf    = ADD_DER_ECC_NISTP521_HEADER(key);
        *key_buflen = (uint16_t)ADD_DER_ECC_NISTP521_HEADER(*key_buflen);
    }
#endif
#if SSS_HAVE_EC_BP
    else if (curve_id == kSE05x_ECCurve_Brainpool160) {
        ENSURE_OR_GO_EXIT((*keylen) > der_ecc_bp160_header_len);
        memcpy(key, gecc_der_header_bp160, der_ecc_bp160_header_len);
        *key_buf    = ADD_DER_ECC_BP160_HEADER(key);
        *key_buflen = (uint16_t)ADD_DER_ECC_BP160_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_Brainpool192) {
        ENSURE_OR_GO_EXIT((*keylen) > der_ecc_bp192_header_len);
        memcpy(key, gecc_der_header_bp192, der_ecc_bp192_header_len);
        *key_buf    = ADD_DER_ECC_BP192_HEADER(key);
        *key_buflen = (uint16_t)ADD_DER_ECC_BP192_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_Brainpool224) {
        ENSURE_OR_GO_EXIT((*keylen) > der_ecc_bp224_header_len);
        memcpy(key, gecc_der_header_bp224, der_ecc_bp224_header_len);
        *key_buf    = ADD_DER_ECC_BP224_HEADER(key);
        *key_buflen = (uint16_t)ADD_DER_ECC_BP224_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_Brainpool320) {
        ENSURE_OR_GO_EXIT((*keylen) > der_ecc_bp320_header_len);
        memcpy(key, gecc_der_header_bp320, der_ecc_bp320_header_len);
        *key_buf    = ADD_DER_ECC_BP320_HEADER(key);
        *key_buflen = (uint16_t)ADD_DER_ECC_BP320_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_Brainpool384) {
        ENSURE_OR_GO_EXIT((*keylen) > der_ecc_bp384_header_len);
        memcpy(key, gecc_der_header_bp384, der_ecc_bp384_header_len);
        *key_buf    = ADD_DER_ECC_BP384_HEADER(key);
        *key_buflen = (uint16_t)ADD_DER_ECC_BP384_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_Brainpool256) {
        ENSURE_OR_GO_EXIT((*keylen) > der_ecc_bp256_header_len);
        memcpy(key, gecc_der_header_bp256, der_ecc_bp256_header_len);
        *key_buf    = ADD_DER_ECC_BP256_HEADER(key);
        *key_buflen = (uint16_t)ADD_DER_ECC_BP256_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_Brainpool512) {
        ENSURE_OR_GO_EXIT((*keylen) > der_ecc_bp512_header_len);
        memcpy(key, gecc_der_header_bp512, der_ecc_bp512_header_len);
        *key_buf    = ADD_DER_ECC_BP512_HEADER(key);
        *key_buflen = (uint16_t)ADD_DER_ECC_BP512_HEADER(*key_buflen);
    }
#endif
#if SSS_HAVE_EC_NIST_K
    else if (curve_id == kSE05x_ECCurve_Secp256k1) {
        ENSURE_OR_GO_EXIT((*keylen) > der_ecc_256k_header_len);
        memcpy(key, gecc_der_header_256k, der_ecc_256k_header_len);
        *key_buf    = ADD_DER_ECC_256K_HEADER(key);
        *key_buflen = (uint16_t)ADD_DER_ECC_256K_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_Secp160k1) {
        ENSURE_OR_GO_EXIT((*keylen) > der_ecc_160k_header_len);
        memcpy(key, gecc_der_header_160k, der_ecc_160k_header_len);
        *key_buf    = ADD_DER_ECC_160K_HEADER(key);
        *key_buflen = (uint16_t)ADD_DER_ECC_160K_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_Secp192k1) {
        ENSURE_OR_GO_EXIT((*keylen) > der_ecc_192k_header_len);
        memcpy(key, gecc_der_header_192k, der_ecc_192k_header_len);
        *key_buf    = ADD_DER_ECC_192K_HEADER(key);
        *key_buflen = (uint16_t)ADD_DER_ECC_192K_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_Secp224k1) {
        ENSURE_OR_GO_EXIT((*keylen) > der_ecc_224k_header_len);
        memcpy(key, gecc_der_header_224k, der_ecc_224k_header_len);
        *key_buf    = ADD_DER_ECC_224K_HEADER(key);
        *key_buflen = (uint16_t)ADD_DER_ECC_224K_HEADER(*key_buflen);
    }
#endif
#if SSS_HAVE_EC_MONT
#if SSS_HAVE_SE05X_VER_GTE_06_00
    else if (curve_id == kSE05x_ECCurve_ECC_MONT_DH_448) {
        ENSURE_OR_GO_EXIT((*keylen) > der_ecc_mont_dh_448_header_len);
        memcpy(key, gecc_der_header_mont_dh_448, der_ecc_mont_dh_448_header_len);
        *key_buf    = ADD_DER_ECC_MONT_DH_448_HEADER(key);
        *key_buflen = (uint16_t)ADD_DER_ECC_MONT_DH_448_HEADER(*key_buflen);
    }
#endif
    else if (curve_id == kSE05x_ECCurve_ECC_MONT_DH_25519) {
        ENSURE_OR_GO_EXIT((*keylen) > der_ecc_mont_dh_25519_header_len);
        memcpy(key, gecc_der_header_mont_dh_25519, der_ecc_mont_dh_25519_header_len);
        *key_buf    = ADD_DER_ECC_MONT_DH_25519_HEADER(key);
        *key_buflen = (uint16_t)ADD_DER_ECC_MONT_DH_25519_HEADER(*key_buflen);
    }
#endif // SSS_HAVE_EC_MONT
#if SSS_HAVE_EC_ED
    else if (curve_id == kSE05x_ECCurve_ECC_ED_25519) {
        ENSURE_OR_GO_EXIT((*keylen) > der_ecc_twisted_ed_25519_header_len);
        memcpy(key, gecc_der_header_twisted_ed_25519, der_ecc_twisted_ed_25519_header_len);
        *key_buf    = ADD_DER_ECC_TWISTED_ED_25519_HEADER(key);
        *key_buflen = (uint16_t)ADD_DER_ECC_TWISTED_ED_25519_HEADER(*key_buflen);
    }
#endif
    else {
        LOG_W("Returned is not in DER Format");
        *key_buf    = key;
        *key_buflen = 0;
    }
#endif
exit:
    return;
}

void get_ecc_raw_data(uint8_t *key, size_t keylen, uint8_t **key_buf, size_t *key_buflen, uint32_t curve_id)
{
    if (key == NULL || key_buf == NULL || key_buflen == NULL) {
        goto exit;
    }

    if (curve_id == kSE05x_ECCurve_NIST_P256) {
        ENSURE_OR_GO_EXIT(keylen > der_ecc_nistp256_header_len);
        *key_buf    = ADD_DER_ECC_NISTP256_HEADER(key);
        *key_buflen = (uint16_t)REMOVE_DER_ECC_NISTP256_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_NIST_P384) {
        ENSURE_OR_GO_EXIT(keylen > der_ecc_nistp384_header_len);
        *key_buf    = ADD_DER_ECC_NISTP384_HEADER(key);
        *key_buflen = (uint16_t)REMOVE_DER_ECC_NISTP384_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_NIST_P192) {
        ENSURE_OR_GO_EXIT(keylen > der_ecc_nistp192_header_len);
        *key_buf    = ADD_DER_ECC_NISTP192_HEADER(key);
        *key_buflen = (uint16_t)REMOVE_DER_ECC_NISTP192_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_NIST_P224) {
        ENSURE_OR_GO_EXIT(keylen > der_ecc_nistp224_header_len);
        *key_buf    = ADD_DER_ECC_NISTP224_HEADER(key);
        *key_buflen = (uint16_t)REMOVE_DER_ECC_NISTP224_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_NIST_P521) {
        ENSURE_OR_GO_EXIT(keylen > der_ecc_nistp521_header_len);
        *key_buf    = ADD_DER_ECC_NISTP521_HEADER(key);
        *key_buflen = (uint16_t)REMOVE_DER_ECC_NISTP521_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_Brainpool160) {
        ENSURE_OR_GO_EXIT(keylen > der_ecc_bp160_header_len);
        *key_buf    = ADD_DER_ECC_BP160_HEADER(key);
        *key_buflen = (uint16_t)REMOVE_DER_ECC_BP160_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_Brainpool192) {
        ENSURE_OR_GO_EXIT(keylen > der_ecc_bp192_header_len);
        *key_buf    = ADD_DER_ECC_BP192_HEADER(key);
        *key_buflen = (uint16_t)REMOVE_DER_ECC_BP192_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_Brainpool224) {
        ENSURE_OR_GO_EXIT(keylen > der_ecc_bp224_header_len);
        *key_buf    = REMOVE_DER_ECC_BP224_HEADER(key);
        *key_buflen = (uint16_t)REMOVE_DER_ECC_BP224_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_Brainpool320) {
        ENSURE_OR_GO_EXIT(keylen > der_ecc_bp320_header_len);
        *key_buf    = ADD_DER_ECC_BP320_HEADER(key);
        *key_buflen = (uint16_t)REMOVE_DER_ECC_BP320_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_Brainpool384) {
        ENSURE_OR_GO_EXIT(keylen > der_ecc_bp384_header_len);
        *key_buf    = ADD_DER_ECC_BP384_HEADER(key);
        *key_buflen = (uint16_t)REMOVE_DER_ECC_BP384_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_Brainpool256) {
        ENSURE_OR_GO_EXIT(keylen > der_ecc_bp256_header_len);
        *key_buf    = ADD_DER_ECC_BP256_HEADER(key);
        *key_buflen = (uint16_t)REMOVE_DER_ECC_BP256_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_Brainpool512) {
        ENSURE_OR_GO_EXIT(keylen > der_ecc_bp512_header_len);
        *key_buf    = ADD_DER_ECC_BP512_HEADER(key);
        *key_buflen = (uint16_t)REMOVE_DER_ECC_BP512_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_Secp256k1) {
        ENSURE_OR_GO_EXIT(keylen > der_ecc_256k_header_len);
        *key_buf    = ADD_DER_ECC_256K_HEADER(key);
        *key_buflen = (uint16_t)REMOVE_DER_ECC_256K_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_Secp160k1) {
        ENSURE_OR_GO_EXIT(keylen > der_ecc_160k_header_len);
        *key_buf    = ADD_DER_ECC_160K_HEADER(key);
        *key_buflen = (uint16_t)REMOVE_DER_ECC_160K_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_Secp192k1) {
        ENSURE_OR_GO_EXIT(keylen > der_ecc_192k_header_len);
        *key_buf    = ADD_DER_ECC_192K_HEADER(key);
        *key_buflen = (uint16_t)REMOVE_DER_ECC_192K_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_Secp224k1) {
        ENSURE_OR_GO_EXIT(keylen > der_ecc_224k_header_len);
        *key_buf    = ADD_DER_ECC_224K_HEADER(key);
        *key_buflen = (uint16_t)REMOVE_DER_ECC_224K_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_ECC_ED_25519) {
        ENSURE_OR_GO_EXIT(keylen > der_ecc_twisted_ed_25519_header_len);
        *key_buf    = ADD_DER_ECC_TWISTED_ED_25519_HEADER(key);
        *key_buflen = (uint16_t)REMOVE_DER_ECC_TWISTED_ED_25519_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_ECC_MONT_DH_25519) {
        ENSURE_OR_GO_EXIT(keylen > der_ecc_mont_dh_25519_header_len);
        *key_buf    = ADD_DER_ECC_MONT_DH_25519_HEADER(key);
        *key_buflen = (uint16_t)REMOVE_DER_ECC_MONT_DH_25519_HEADER(*key_buflen);
    }
    else if (curve_id == kSE05x_ECCurve_ECC_MONT_DH_448) {
        ENSURE_OR_GO_EXIT(keylen > der_ecc_mont_dh_448_header_len);
        *key_buf    = ADD_DER_ECC_MONT_DH_448_HEADER(key);
        *key_buflen = (uint16_t)REMOVE_DER_ECC_MONT_DH_448_HEADER(*key_buflen);
    }
    else {
        LOG_W("Returned is not in DER Format");
        *key_buf    = key;
        *key_buflen = 0;
    }

exit:
    return;
}

sss_status_t sss_se05x_key_store_get_key(
    sss_se05x_key_store_t *keyStore, sss_se05x_object_t *keyObject, uint8_t *key, size_t *keylen, size_t *pKeyBitLen)
{
    sss_status_t retval           = kStatus_SSS_Fail;
    sss_cipher_type_t cipher_type = kSSS_CipherType_NONE;
    smStatus_t status             = SM_NOT_OK;
    uint16_t size;
    ENSURE_OR_GO_EXIT(keyObject);
    ENSURE_OR_GO_EXIT(key);
    ENSURE_OR_GO_EXIT(keylen);
    ENSURE_OR_GO_EXIT(pKeyBitLen);

    cipher_type = (sss_cipher_type_t)keyObject->cipherType;

    switch (cipher_type) {
    case kSSS_CipherType_EC_NIST_P:
#if SSS_HAVE_EC_NIST_K
    case kSSS_CipherType_EC_NIST_K:
#endif
#if SSS_HAVE_EC_BP
    case kSSS_CipherType_EC_BRAINPOOL:
#endif
#if SSS_HAVE_TPM_BN
    case kSSS_CipherType_EC_BARRETO_NAEHRIG:
#endif
#if SSS_HAVE_EC_MONT
    case kSSS_CipherType_EC_MONTGOMERY:
#endif
#if SSS_HAVE_EC_ED
    case kSSS_CipherType_EC_TWISTED_ED:
#endif
    {
        uint8_t *key_buf  = NULL;
        size_t key_buflen = 0;

        /* Return the Key length including the ECC DER Header */
        add_ecc_header(key, keylen, &key_buf, &key_buflen, keyObject->curve_id);
        (*keylen) = (*keylen) - key_buflen;

        status = Se05x_API_ReadObject(&keyStore->session->s_ctx, keyObject->keyId, 0, 0, key_buf, keylen);
        ENSURE_OR_GO_EXIT(status == SM_OK);

        /* Change Endiannes. */
#if SSS_HAVE_TPM_BN || SSS_HAVE_EC_ED
        if ((keyObject->curve_id == kSE05x_ECCurve_ECC_MONT_DH_25519) ||
            (keyObject->curve_id == kSE05x_ECCurve_ECC_MONT_DH_448) ||
            (keyObject->curve_id == kSE05x_ECCurve_ECC_ED_25519)) {
            for (size_t keyValueIdx = 0; keyValueIdx < (*keylen >> 1); keyValueIdx++) {
                uint8_t swapByte                   = key_buf[keyValueIdx];
                key_buf[keyValueIdx]               = key_buf[*keylen - 1 - keyValueIdx];
                key_buf[*keylen - 1 - keyValueIdx] = swapByte;
            }
        }
#endif

        /* Return the Key length with header length */
        *keylen += key_buflen;

        break;
    }
#if SSSFTR_SE05X_RSA && SSS_HAVE_RSA
    case kSSS_CipherType_RSA:
    case kSSS_CipherType_RSA_CRT: {
        uint8_t modulus[1024] = {0};
        uint8_t exponent[4]   = {0};
        size_t modLen         = sizeof(modulus);
        size_t expLen         = sizeof(exponent);

        status = Se05x_API_ReadRSA(
            &keyStore->session->s_ctx, keyObject->keyId, 0, 0, kSE05x_RSAPubKeyComp_MOD, modulus, &modLen);
        ENSURE_OR_GO_EXIT(status == SM_OK);

        status = Se05x_API_ReadRSA(
            &keyStore->session->s_ctx, keyObject->keyId, 0, 0, kSE05x_RSAPubKeyComp_PUB_EXP, exponent, &expLen);
        ENSURE_OR_GO_EXIT(status == SM_OK);

        if (sss_util_asn1_rsa_get_public(key, keylen, modulus, modLen, exponent, expLen) != kStatus_SSS_Success) {
            goto exit;
        }
    } break;
#endif // SSSFTR_SE05X_RSA && && SSS_HAVE_RSA
    case kSSS_CipherType_AES:
        status = Se05x_API_ReadObject(&keyStore->session->s_ctx, keyObject->keyId, 0, 0, key, keylen);
        ENSURE_OR_GO_EXIT(status == SM_OK);
        break;
    case kSSS_CipherType_Binary:
    case kSSS_CipherType_Certificate: {
        uint16_t rem_data = 0;
        uint16_t offset   = 0;
        size_t max_buffer = 0;
        status            = Se05x_API_ReadSize(&keyStore->session->s_ctx, keyObject->keyId, &size);
        ENSURE_OR_GO_EXIT(status == SM_OK);
        if (*keylen < size) {
            LOG_E("Insufficient buffer ");
            goto exit;
        }

        rem_data = size;
        *keylen  = size;
        while (rem_data > 0) {
            uint16_t chunk = (rem_data > BINARY_WRITE_MAX_LEN) ? BINARY_WRITE_MAX_LEN : rem_data;
            rem_data       = rem_data - chunk;
            max_buffer     = chunk;
            status         = Se05x_API_ReadObject(
                &keyStore->session->s_ctx, keyObject->keyId, offset, chunk, (key + offset), &max_buffer);
            ENSURE_OR_GO_EXIT(status == SM_OK);
            offset = offset + chunk;
        }

    } break;
    case kSSS_CipherType_DES:
        status = Se05x_API_ReadObject(&keyStore->session->s_ctx, keyObject->keyId, 0, 0, key, keylen);
        ENSURE_OR_GO_EXIT(status == SM_OK);
        break;
    case kSSS_CipherType_PCR:
        status = Se05x_API_ReadObject(&keyStore->session->s_ctx, keyObject->keyId, 0, 0, key, keylen);
        ENSURE_OR_GO_EXIT(status == SM_OK);
        break;
    case kSSS_CipherType_Count:
        status = Se05x_API_ReadObject(&keyStore->session->s_ctx, keyObject->keyId, 0, 0, key, keylen);
        ENSURE_OR_GO_EXIT(status == SM_OK);
        break;
    default:
        goto exit;
    }

    retval = kStatus_SSS_Success;
exit:
    return retval;
}

sss_status_t sss_se05x_key_store_get_key_attst(sss_se05x_key_store_t *keyStore,
    sss_se05x_object_t *keyObject,
    uint8_t *key,
    size_t *keylen,
    size_t *pKeyBitLen,
    sss_se05x_object_t *keyObject_attst,
    sss_algorithm_t algorithm_attst,
    uint8_t *random_attst,
    size_t randomLen_attst,
    sss_se05x_attst_data_t *attst_data)
{
    AX_UNUSED_ARG(pKeyBitLen);
    sss_status_t retval           = kStatus_SSS_Fail;
    sss_cipher_type_t cipher_type = (sss_cipher_type_t)keyObject->cipherType;
    smStatus_t status             = SM_NOT_OK;
    uint16_t size;

    uint32_t attestID;
    SE05x_AttestationAlgo_t attestAlgo;

    attestID = keyObject_attst->keyId;

    switch (keyObject_attst->cipherType) {
    case kSSS_CipherType_EC_NIST_P:
#if SSS_HAVE_EC_NIST_K
    case kSSS_CipherType_EC_NIST_K:
#endif
#if SSS_HAVE_EC_BP
    case kSSS_CipherType_EC_BRAINPOOL:
#endif
    {
        SE05x_ECSignatureAlgo_t ecSignAlgo = (SE05x_ECSignatureAlgo_t)se05x_get_ec_sign_hash_mode(algorithm_attst);
        attestAlgo                         = (SE05x_AttestationAlgo_t)ecSignAlgo;
    } break;

#if SSS_HAVE_EC_ED || SSS_HAVE_TPM_BN
    case kSSS_CipherType_EC_TWISTED_ED:
    case kSSS_CipherType_EC_BARRETO_NAEHRIG: {
        LOG_E("Attestation not supported");
        return retval;
    } break;
#endif

#if SSSFTR_SE05X_RSA && SSS_HAVE_RSA
    case kSSS_CipherType_RSA:
    case kSSS_CipherType_RSA_CRT: {
        SE05x_RSASignatureAlgo_t rsaSigningAlgo = se05x_get_rsa_sign_hash_mode(algorithm_attst);
        attestAlgo                              = (SE05x_AttestationAlgo_t)rsaSigningAlgo;
    } break;
#endif
    default:
        goto exit;
    }

    switch (cipher_type) {
    case kSSS_CipherType_EC_NIST_P:
#if SSS_HAVE_EC_NIST_K
    case kSSS_CipherType_EC_NIST_K:
#endif
#if SSS_HAVE_EC_BP
    case kSSS_CipherType_EC_BRAINPOOL:
#endif
#if SSS_HAVE_TPM_BN
    case kSSS_CipherType_EC_BARRETO_NAEHRIG:
#endif
#if SSS_HAVE_EC_MONT
    case kSSS_CipherType_EC_MONTGOMERY:
#endif
#if SSS_HAVE_EC_ED
    case kSSS_CipherType_EC_TWISTED_ED:
#endif
    {
        uint8_t *key_buf  = NULL;
        size_t key_buflen = 0;

        /* Return the Key length including the ECC DER Header */
        add_ecc_header(key, keylen, &key_buf, &key_buflen, keyObject->curve_id);
        (*keylen) = (*keylen) - key_buflen;

        attst_data->data[0].timeStampLen = sizeof(SE05x_TimeStamp_t);
#if SSS_HAVE_SE05X_VER_GTE_07_02
        status = Se05x_API_ReadObject_W_Attst_V2(&keyStore->session->s_ctx,
            keyObject->keyId,
            0,
            0,
            attestID,
            attestAlgo,
            random_attst,
            randomLen_attst,
            key_buf,
            keylen,
            attst_data->data[0].attribute,
            &(attst_data->data[0].attributeLen),
            &(attst_data->data[0].timeStamp),
            attst_data->data[0].chipId,
            &(attst_data->data[0].chipIdLen),
            attst_data->data[0].cmd,
            &(attst_data->data[0].cmdLen),
            attst_data->data[0].objSize,
            &(attst_data->data[0].objSizeLen),
            attst_data->data[0].signature,
            &(attst_data->data[0].signatureLen));

#else
        status = Se05x_API_ReadObject_W_Attst(&keyStore->session->s_ctx,
            keyObject->keyId,
            0,
            0,
            attestID,
            attestAlgo,
            random_attst,
            randomLen_attst,
            key_buf,
            keylen,
            attst_data->data[0].attribute,
            &(attst_data->data[0].attributeLen),
            &(attst_data->data[0].timeStamp),
            attst_data->data[0].outrandom,
            &(attst_data->data[0].outrandomLen),
            attst_data->data[0].chipId,
            &(attst_data->data[0].chipIdLen),
            attst_data->data[0].signature,
            &(attst_data->data[0].signatureLen));
#endif
        ENSURE_OR_GO_EXIT(status == SM_OK);

#if SSS_HAVE_EC_MONT || SSS_HAVE_EC_ED
        /* Change Endiannes. */
        if ((keyObject->curve_id == kSE05x_ECCurve_ECC_MONT_DH_25519) ||
            (keyObject->curve_id == kSE05x_ECCurve_ECC_MONT_DH_448) ||
            (keyObject->curve_id == kSE05x_ECCurve_ECC_ED_25519)) {
            for (size_t keyValueIdx = 0; keyValueIdx < (*keylen >> 1); keyValueIdx++) {
                uint8_t swapByte                   = key_buf[keyValueIdx];
                key_buf[keyValueIdx]               = key_buf[*keylen - 1 - keyValueIdx];
                key_buf[*keylen - 1 - keyValueIdx] = swapByte;
            }
        }
#endif

        attst_data->valid_number = 1;
        /* Return the Key length with header length */
        *keylen += key_buflen;

        break;
    }
#if SSSFTR_SE05X_RSA && SSS_HAVE_RSA
    case kSSS_CipherType_RSA:
    case kSSS_CipherType_RSA_CRT: {
        uint8_t modulus[1024];
        uint8_t exponent[4];
        size_t modLen           = sizeof(modulus);
        size_t expLen           = sizeof(exponent);
        uint16_t key_size_bytes = 0;

        if (attestAlgo == kSE05x_AttestationAlgo_RSA_SHA_512_PKCS1 ||
            attestAlgo == kSE05x_AttestationAlgo_RSA_SHA512_PKCS1_PSS) {
            status = Se05x_API_ReadSize(&keyStore->session->s_ctx, keyObject_attst->keyId, &key_size_bytes);
            if (status != SM_OK) {
                return kStatus_SSS_Fail;
            }

            if ((key_size_bytes * 8) == 512) {
                return kStatus_SSS_Fail;
            }
        }

        attst_data->data[0].timeStampLen = sizeof(SE05x_TimeStamp_t);
#if SSS_HAVE_SE05X_VER_GTE_07_02
        status = Se05x_API_ReadRSA_W_Attst_V2(&keyStore->session->s_ctx,
            keyObject->keyId,
            0,
            0,
            kSE05x_RSAPubKeyComp_MOD,
            attestID,
            attestAlgo,
            random_attst,
            randomLen_attst,
            modulus,
            &modLen,
            attst_data->data[0].attribute,
            &(attst_data->data[0].attributeLen),
            &(attst_data->data[0].timeStamp),
            attst_data->data[0].chipId,
            &(attst_data->data[0].chipIdLen),
            attst_data->data[0].cmd,
            &(attst_data->data[0].cmdLen),
            attst_data->data[0].objSize,
            &(attst_data->data[0].objSizeLen),
            attst_data->data[0].signature,
            &(attst_data->data[0].signatureLen));
#else
        status = Se05x_API_ReadRSA_W_Attst(&keyStore->session->s_ctx,
            keyObject->keyId,
            0,
            0,
            kSE05x_RSAPubKeyComp_MOD,
            attestID,
            attestAlgo,
            random_attst,
            randomLen_attst,
            modulus,
            &modLen,
            attst_data->data[0].attribute,
            &(attst_data->data[0].attributeLen),
            &(attst_data->data[0].timeStamp),
            attst_data->data[0].outrandom,
            &(attst_data->data[0].outrandomLen),
            attst_data->data[0].chipId,
            &(attst_data->data[0].chipIdLen),
            attst_data->data[0].signature,
            &(attst_data->data[0].signatureLen));
#endif
        ENSURE_OR_GO_EXIT(status == SM_OK);

        attst_data->data[1].timeStampLen = sizeof(SE05x_TimeStamp_t);
#if SSS_HAVE_SE05X_VER_GTE_07_02
        status = Se05x_API_ReadRSA_W_Attst_V2(&keyStore->session->s_ctx,
            keyObject->keyId,
            0,
            0,
            kSE05x_RSAPubKeyComp_PUB_EXP,
            attestID,
            attestAlgo,
            random_attst,
            randomLen_attst,
            exponent,
            &expLen,
            attst_data->data[1].attribute,
            &(attst_data->data[1].attributeLen),
            &(attst_data->data[1].timeStamp),
            attst_data->data[1].chipId,
            &(attst_data->data[1].chipIdLen),
            attst_data->data[1].cmd,
            &(attst_data->data[1].cmdLen),
            attst_data->data[1].objSize,
            &(attst_data->data[1].objSizeLen),
            attst_data->data[1].signature,
            &(attst_data->data[1].signatureLen));
#else
        status = Se05x_API_ReadRSA_W_Attst(&keyStore->session->s_ctx,
            keyObject->keyId,
            0,
            0,
            kSE05x_RSAPubKeyComp_PUB_EXP,
            attestID,
            attestAlgo,
            random_attst,
            randomLen_attst,
            exponent,
            &expLen,
            attst_data->data[1].attribute,
            &(attst_data->data[1].attributeLen),
            &(attst_data->data[1].timeStamp),
            attst_data->data[1].outrandom,
            &(attst_data->data[1].outrandomLen),
            attst_data->data[1].chipId,
            &(attst_data->data[1].chipIdLen),
            attst_data->data[1].signature,
            &(attst_data->data[1].signatureLen));
#endif
        attst_data->valid_number = 2;

        ENSURE_OR_GO_EXIT(status == SM_OK);

        if (sss_util_asn1_rsa_get_public(key, keylen, modulus, modLen, exponent, expLen) != kStatus_SSS_Success) {
            goto exit;
        }
    } break;
#endif // SSSFTR_SE05X_RSA && SSS_HAVE_RSA
    case kSSS_CipherType_AES:
        attst_data->data[0].timeStampLen = sizeof(SE05x_TimeStamp_t);
#if SSS_HAVE_SE05X_VER_GTE_07_02
        status = Se05x_API_ReadObject_W_Attst_V2(&keyStore->session->s_ctx,
            keyObject->keyId,
            0,
            0,
            attestID,
            attestAlgo,
            random_attst,
            randomLen_attst,
            key,
            keylen,
            attst_data->data[0].attribute,
            &(attst_data->data[0].attributeLen),
            &(attst_data->data[0].timeStamp),
            attst_data->data[0].chipId,
            &(attst_data->data[0].chipIdLen),
            attst_data->data[0].cmd,
            &(attst_data->data[0].cmdLen),
            attst_data->data[0].objSize,
            &(attst_data->data[0].objSizeLen),
            attst_data->data[0].signature,
            &(attst_data->data[0].signatureLen));
#else
        status = Se05x_API_ReadObject_W_Attst(&keyStore->session->s_ctx,
            keyObject->keyId,
            0,
            0,
            attestID,
            attestAlgo,
            random_attst,
            randomLen_attst,
            key,
            keylen,
            attst_data->data[0].attribute,
            &(attst_data->data[0].attributeLen),
            &(attst_data->data[0].timeStamp),
            attst_data->data[0].outrandom,
            &(attst_data->data[0].outrandomLen),
            attst_data->data[0].chipId,
            &(attst_data->data[0].chipIdLen),
            attst_data->data[0].signature,
            &(attst_data->data[0].signatureLen));
#endif
        attst_data->valid_number = 1;

        ENSURE_OR_GO_EXIT(status == SM_OK);
        break;
    case kSSS_CipherType_Binary:
    case kSSS_CipherType_Certificate: {
        uint16_t rem_data = 0;
        uint16_t offset   = 0;
        size_t dataLen    = 0;
        // size_t signatureLen = 0;
        status = Se05x_API_ReadSize(&keyStore->session->s_ctx, keyObject->keyId, &size);
        ENSURE_OR_GO_EXIT(status == SM_OK);

        if (*keylen < size) {
            LOG_E("Insufficient buffer ");
            goto exit;
        }

        rem_data = size;
        *keylen  = size;
        if (size > BINARY_WRITE_MAX_LEN) {
            LOG_E("Cannot read large binary data with attestation");
            goto exit;
        }
        // while (rem_data > 0) {
        // uint16_t chunk = (rem_data > BINARY_WRITE_MAX_LEN) ?
        //                      BINARY_WRITE_MAX_LEN :
        //                      rem_data;
        // rem_data = rem_data - chunk;
        dataLen = rem_data;

        // signatureLen = attst_data->data[0].signatureLen;
        attst_data->data[0].timeStampLen = sizeof(SE05x_TimeStamp_t);
#if SSS_HAVE_SE05X_VER_GTE_07_02
        status  = Se05x_API_ReadObject_W_Attst_V2(&keyStore->session->s_ctx,
            keyObject->keyId,
            offset,
            rem_data,
            attestID,
            attestAlgo,
            random_attst,
            randomLen_attst,
            (key + 0),
            &dataLen,
            attst_data->data[0].attribute,
            &(attst_data->data[0].attributeLen),
            &(attst_data->data[0].timeStamp),
            attst_data->data[0].chipId,
            &(attst_data->data[0].chipIdLen),
            attst_data->data[0].cmd,
            &(attst_data->data[0].cmdLen),
            attst_data->data[0].objSize,
            &(attst_data->data[0].objSizeLen),
            attst_data->data[0].signature,
            &attst_data->data[0].signatureLen);
        *keylen = dataLen;
#else
        status = Se05x_API_ReadObject_W_Attst(&keyStore->session->s_ctx,
            keyObject->keyId,
            offset,
            rem_data,
            attestID,
            attestAlgo,
            random_attst,
            randomLen_attst,
            (key + 0),
            &dataLen,
            attst_data->data[0].attribute,
            &(attst_data->data[0].attributeLen),
            &(attst_data->data[0].timeStamp),
            attst_data->data[0].outrandom,
            &(attst_data->data[0].outrandomLen),
            attst_data->data[0].chipId,
            &(attst_data->data[0].chipIdLen),
            attst_data->data[0].signature,
            &attst_data->data[0].signatureLen);
#endif

        ENSURE_OR_GO_EXIT(status == SM_OK);

        // offset = offset + chunk;
        // }
    } break;
    case kSSS_CipherType_DES:
        attst_data->data[0].timeStampLen = sizeof(SE05x_TimeStamp_t);
#if SSS_HAVE_SE05X_VER_GTE_07_02
        status = Se05x_API_ReadObject_W_Attst_V2(&keyStore->session->s_ctx,
            keyObject->keyId,
            0,
            0,
            attestID,
            attestAlgo,
            random_attst,
            randomLen_attst,
            key,
            keylen,
            attst_data->data[0].attribute,
            &(attst_data->data[0].attributeLen),
            &(attst_data->data[0].timeStamp),
            attst_data->data[0].chipId,
            &(attst_data->data[0].chipIdLen),
            attst_data->data[0].cmd,
            &(attst_data->data[0].cmdLen),
            attst_data->data[0].objSize,
            &(attst_data->data[0].objSizeLen),
            attst_data->data[0].signature,
            &(attst_data->data[0].signatureLen));
#else
        status = Se05x_API_ReadObject_W_Attst(&keyStore->session->s_ctx,
            keyObject->keyId,
            0,
            0,
            attestID,
            attestAlgo,
            random_attst,
            randomLen_attst,
            key,
            keylen,
            attst_data->data[0].attribute,
            &(attst_data->data[0].attributeLen),
            &(attst_data->data[0].timeStamp),
            attst_data->data[0].outrandom,
            &(attst_data->data[0].outrandomLen),
            attst_data->data[0].chipId,
            &(attst_data->data[0].chipIdLen),
            attst_data->data[0].signature,
            &(attst_data->data[0].signatureLen));

#endif
        attst_data->valid_number = 1;

        ENSURE_OR_GO_EXIT(status == SM_OK);
        break;

    case kSSS_CipherType_PCR:
        attst_data->data[0].timeStampLen = sizeof(SE05x_TimeStamp_t);
#if SSS_HAVE_SE05X_VER_GTE_07_02
        status = Se05x_API_ReadObject_W_Attst_V2(&keyStore->session->s_ctx,
            keyObject->keyId,
            0,
            0,
            attestID,
            attestAlgo,
            random_attst,
            randomLen_attst,
            key,
            keylen,
            attst_data->data[0].attribute,
            &(attst_data->data[0].attributeLen),
            &(attst_data->data[0].timeStamp),
            attst_data->data[0].chipId,
            &(attst_data->data[0].chipIdLen),
            attst_data->data[0].cmd,
            &(attst_data->data[0].cmdLen),
            attst_data->data[0].objSize,
            &(attst_data->data[0].objSizeLen),
            attst_data->data[0].signature,
            &(attst_data->data[0].signatureLen));
#else
        status = Se05x_API_ReadObject_W_Attst(&keyStore->session->s_ctx,
            keyObject->keyId,
            0,
            0,
            attestID,
            attestAlgo,
            random_attst,
            randomLen_attst,
            key,
            keylen,
            attst_data->data[0].attribute,
            &(attst_data->data[0].attributeLen),
            &(attst_data->data[0].timeStamp),
            attst_data->data[0].outrandom,
            &(attst_data->data[0].outrandomLen),
            attst_data->data[0].chipId,
            &(attst_data->data[0].chipIdLen),
            attst_data->data[0].signature,
            &(attst_data->data[0].signatureLen));
#endif
        attst_data->valid_number = 1;

        ENSURE_OR_GO_EXIT(status == SM_OK);
        break;

    case kSSS_CipherType_Count:
        attst_data->data[0].timeStampLen = sizeof(SE05x_TimeStamp_t);
#if SSS_HAVE_SE05X_VER_GTE_07_02
        status = Se05x_API_ReadObject_W_Attst_V2(&keyStore->session->s_ctx,
            keyObject->keyId,
            0,
            0,
            attestID,
            attestAlgo,
            random_attst,
            randomLen_attst,
            key,
            keylen,
            attst_data->data[0].attribute,
            &(attst_data->data[0].attributeLen),
            &(attst_data->data[0].timeStamp),
            attst_data->data[0].chipId,
            &(attst_data->data[0].chipIdLen),
            attst_data->data[0].cmd,
            &(attst_data->data[0].cmdLen),
            attst_data->data[0].objSize,
            &(attst_data->data[0].objSizeLen),
            attst_data->data[0].signature,
            &(attst_data->data[0].signatureLen));
#else
        status = Se05x_API_ReadObject_W_Attst(&keyStore->session->s_ctx,
            keyObject->keyId,
            0,
            0,
            attestID,
            attestAlgo,
            random_attst,
            randomLen_attst,
            key,
            keylen,
            attst_data->data[0].attribute,
            &(attst_data->data[0].attributeLen),
            &(attst_data->data[0].timeStamp),
            attst_data->data[0].outrandom,
            &(attst_data->data[0].outrandomLen),
            attst_data->data[0].chipId,
            &(attst_data->data[0].chipIdLen),
            attst_data->data[0].signature,
            &(attst_data->data[0].signatureLen));
#endif
        attst_data->valid_number = 1;

        ENSURE_OR_GO_EXIT(status == SM_OK);
        break;

    case kSSS_CipherType_HMAC:
    case kSSS_CipherType_CMAC:
    case kSSS_CipherType_UserID: {
        attst_data->data[0].timeStampLen = sizeof(SE05x_TimeStamp_t);
#if SSS_HAVE_SE05X_VER_GTE_07_02
        status = Se05x_API_ReadObject_W_Attst_V2(&keyStore->session->s_ctx,
            keyObject->keyId,
            0,
            0,
            attestID,
            attestAlgo,
            random_attst,
            randomLen_attst,
            key,
            keylen,
            attst_data->data[0].attribute,
            &(attst_data->data[0].attributeLen),
            &(attst_data->data[0].timeStamp),
            attst_data->data[0].chipId,
            &(attst_data->data[0].chipIdLen),
            attst_data->data[0].cmd,
            &(attst_data->data[0].cmdLen),
            attst_data->data[0].objSize,
            &(attst_data->data[0].objSizeLen),
            attst_data->data[0].signature,
            &(attst_data->data[0].signatureLen));
#else
        status = Se05x_API_ReadObject_W_Attst(&keyStore->session->s_ctx,
            keyObject->keyId,
            0,
            0,
            attestID,
            attestAlgo,
            random_attst,
            randomLen_attst,
            key,
            keylen,
            attst_data->data[0].attribute,
            &(attst_data->data[0].attributeLen),
            &(attst_data->data[0].timeStamp),
            attst_data->data[0].outrandom,
            &(attst_data->data[0].outrandomLen),
            attst_data->data[0].chipId,
            &(attst_data->data[0].chipIdLen),
            attst_data->data[0].signature,
            &(attst_data->data[0].signatureLen));
#endif
        attst_data->valid_number = 1;

        ENSURE_OR_GO_EXIT(status == SM_OK);
        break;
    }
    default:
        goto exit;
    }

    retval = kStatus_SSS_Success;
exit:
    return retval;
}

#if 0
/* To be reviewed: Purnank */
sss_status_t sss_se05x_key_store_get_key_fromoffset(sss_se05x_key_store_t *keyStore,
    sss_se05x_object_t *keyObject,
    uint8_t *key,
    size_t *keylen,
    size_t *pKeyBitLen,
    uint16_t offset)
{
    sss_status_t retval = kStatus_SSS_Fail;
    sss_key_type_t key_type = keyObject->objectType;
    smStatus_t status = SM_NOT_OK;

    switch (key_type) {
    case kSSS_KeyType_Certificate:
        status =
            Se05x_API_FIL_BinaryReadFromOffset(&keyStore->session->s_ctx,
                keyObject->keyId,
                (uint16_t)*keylen,
                offset,
                key,
                keylen);
        if (status != SM_OK) {
            goto exit;
        }
        break;
    default:
        goto exit;
    }

    retval = kStatus_SSS_Success;
exit:
    return retval;
}
#endif
sss_status_t sss_se05x_key_store_open_key(sss_se05x_key_store_t *keyStore, sss_se05x_object_t *keyObject)
{
    sss_status_t retval = kStatus_SSS_Fail;

    if (NULL == keyObject) {
        keyStore->kekKey = NULL;
        retval           = kStatus_SSS_Success;
    }
    else if (keyObject->keyStore == keyStore) {
        keyStore->kekKey = keyObject;
        retval           = kStatus_SSS_Success;
    }
    else {
        LOG_W("KeyObject must be of same KeyStore.");
    }

    return retval;
}

sss_status_t sss_se05x_key_store_freeze_key(sss_se05x_key_store_t *keyStore, sss_se05x_object_t *keyObject)
{
    AX_UNUSED_ARG(keyStore);
    AX_UNUSED_ARG(keyObject);
    sss_status_t retval = kStatus_SSS_Fail;
    /* Purpose / Policy is set during creation time and hence can not
     * enforced in SE050 later on */
    return retval;
}

sss_status_t sss_se05x_key_store_erase_key(sss_se05x_key_store_t *keyStore, sss_se05x_object_t *keyObject)
{
    sss_status_t retval = kStatus_SSS_Fail;
    smStatus_t status   = SM_NOT_OK;
    ENSURE_OR_GO_EXIT(keyStore);
    ENSURE_OR_GO_EXIT(keyObject);

    status = Se05x_API_DeleteSecureObject(&keyStore->session->s_ctx, keyObject->keyId);
    if (SM_OK == status) {
        LOG_D("Erased Key id %X", keyObject->keyId);
        retval = kStatus_SSS_Success;
    }
    else {
        LOG_W("Could not delete Key id %X", keyObject->keyId);
    }
exit:
    return retval;
}

void sss_se05x_key_store_context_free(sss_se05x_key_store_t *keyStore)
{
    memset(keyStore, 0, sizeof(*keyStore));
}

sss_status_t sss_se05x_key_store_export_key(
    sss_se05x_key_store_t *keyStore, sss_se05x_object_t *keyObject, uint8_t *key, size_t *keylen)
{
    sss_status_t retval           = kStatus_SSS_Fail;
    sss_cipher_type_t cipher_type = (sss_cipher_type_t)keyObject->cipherType;
    smStatus_t status             = SM_NOT_OK;

    switch (cipher_type) {
    case kSSS_CipherType_EC_NIST_P:
#if SSS_HAVE_EC_NIST_K
    case kSSS_CipherType_EC_NIST_K:
#endif
#if SSS_HAVE_EC_BP
    case kSSS_CipherType_EC_BRAINPOOL:
#endif
#if SSS_HAVE_TPM_BN
    case kSSS_CipherType_EC_BARRETO_NAEHRIG:
#endif
#if SSS_HAVE_EC_MONT
    case kSSS_CipherType_EC_MONTGOMERY:
#endif
#if SSS_HAVE_EC_ED
    case kSSS_CipherType_EC_TWISTED_ED:
#endif
    case kSSS_CipherType_AES:
    case kSSS_CipherType_DES: {
        status =
            Se05x_API_ExportObject(&keyStore->session->s_ctx, keyObject->keyId, kSE05x_RSAKeyComponent_NA, key, keylen);
        if (status != SM_OK) {
            goto exit;
        }

        break;
    }

    default:
        goto exit;
    }

    retval = kStatus_SSS_Success;
exit:
    return retval;
}

sss_status_t sss_se05x_key_store_import_key(
    sss_se05x_key_store_t *keyStore, sss_se05x_object_t *keyObject, uint8_t *key, size_t keylen)
{
    sss_status_t retval           = kStatus_SSS_Fail;
    sss_cipher_type_t cipher_type = (sss_cipher_type_t)keyObject->cipherType;
    smStatus_t status             = SM_NOT_OK;

    switch (cipher_type) {
    case kSSS_CipherType_EC_NIST_P:
#if SSS_HAVE_EC_NIST_K
    case kSSS_CipherType_EC_NIST_K:
#endif
#if SSS_HAVE_EC_BP
    case kSSS_CipherType_EC_BRAINPOOL:
#endif
#if SSS_HAVE_TPM_BN
    case kSSS_CipherType_EC_BARRETO_NAEHRIG:
#endif
#if SSS_HAVE_EC_MONT
    case kSSS_CipherType_EC_MONTGOMERY:
#endif
#if SSS_HAVE_EC_ED
    case kSSS_CipherType_EC_TWISTED_ED:
#endif
    case kSSS_CipherType_AES:
    case kSSS_CipherType_DES: {
        status =
            Se05x_API_ImportObject(&keyStore->session->s_ctx, keyObject->keyId, kSE05x_RSAKeyComponent_NA, key, keylen);
        if (status != SM_OK) {
            goto exit;
        }

        break;
    }

    default:
        goto exit;
    }

    retval = kStatus_SSS_Success;
exit:
    return retval;
}

/* End: se05x_keystore */

/* ************************************************************************** */
/* Functions : sss_se05x_asym                                                 */
/* ************************************************************************** */

sss_status_t sss_se05x_asymmetric_context_init(sss_se05x_asymmetric_t *context,
    sss_se05x_session_t *session,
    sss_se05x_object_t *keyObject,
    sss_algorithm_t algorithm,
    sss_mode_t mode)
{
    sss_status_t retval = kStatus_SSS_Success;

    context->session   = session;
    context->keyObject = keyObject;
    context->algorithm = algorithm;
    context->mode      = mode;

    return retval;
}

sss_status_t sss_se05x_asymmetric_encrypt(
    sss_se05x_asymmetric_t *context, const uint8_t *srcData, size_t srcLen, uint8_t *destData, size_t *destLen)
{
    sss_status_t retval = kStatus_SSS_Fail;
#if SSSFTR_SE05X_RSA && SSS_HAVE_RSA
    smStatus_t status                           = SM_NOT_OK;
    SE05x_RSAEncryptionAlgo_t rsaEncryptionAlgo = se05x_get_rsa_encrypt_mode(context->algorithm);
    status                                      = Se05x_API_RSAEncrypt(
        &context->session->s_ctx, context->keyObject->keyId, rsaEncryptionAlgo, srcData, srcLen, destData, destLen);
    if (status == SM_OK) {
        retval = kStatus_SSS_Success;
    }
#endif
    return retval;
}

sss_status_t sss_se05x_asymmetric_decrypt(
    sss_se05x_asymmetric_t *context, const uint8_t *srcData, size_t srcLen, uint8_t *destData, size_t *destLen)
{
    sss_status_t retval = kStatus_SSS_Fail;
#if SSSFTR_SE05X_RSA && SSS_HAVE_RSA
    smStatus_t status = SM_NOT_OK;

    SE05x_RSAEncryptionAlgo_t rsaEncryptionAlgo = se05x_get_rsa_encrypt_mode(context->algorithm);
    status                                      = Se05x_API_RSADecrypt(
        &context->session->s_ctx, context->keyObject->keyId, rsaEncryptionAlgo, srcData, srcLen, destData, destLen);
    if (status == SM_OK) {
        retval = kStatus_SSS_Success;
    }
#endif
    return retval;
}

sss_status_t sss_se05x_asymmetric_sign_digest(
    sss_se05x_asymmetric_t *context, uint8_t *digest, size_t digestLen, uint8_t *signature, size_t *signatureLen)
{
    sss_status_t retval = kStatus_SSS_Fail;
    smStatus_t status   = SM_NOT_OK;

#if SSSFTR_SE05X_ECC || SSSFTR_SE05X_RSA
    if (kStatus_SSS_Success != se05x_check_input_len(digestLen, context->algorithm)) {
        LOG_E("Algorithm and digest length do not match");
        return kStatus_SSS_Fail;
    }
#endif

    switch (context->keyObject->cipherType) {
#if SSSFTR_SE05X_ECC
    case kSSS_CipherType_EC_NIST_P:
#if SSS_HAVE_EC_NIST_K
    case kSSS_CipherType_EC_NIST_K:
#endif
#if SSS_HAVE_EC_BP
    case kSSS_CipherType_EC_BRAINPOOL:
#endif
    {
        SE05x_ECSignatureAlgo_t ecSignAlgo = se05x_get_ec_sign_hash_mode(context->algorithm);
        status                             = Se05x_API_ECDSASign(&context->session->s_ctx,
            context->keyObject->keyId,
            ecSignAlgo,
            digest,
            digestLen,
            signature,
            signatureLen);
    } break;
#if SSS_HAVE_TPM_BN && SSS_HAVE_ECDAA
#if SSS_HAVE_SE05X_VER_GTE_07_02
    case kSSS_CipherType_EC_BARRETO_NAEHRIG: {
        if (context->algorithm != kAlgorithm_SSS_ECDAA) {
            return kStatus_SSS_Fail;
        }
        uint32_t random_keyId;
        uint8_t raw_signature[64];
        size_t raw_signatureLen               = sizeof(raw_signature);
        SE05x_ECDAASignatureAlgo_t ecSignAlgo = kSE05x_ECDAASignatureAlgo_ECDAA;
        sss_status_t asn_retval               = kStatus_SSS_Fail;

        // Create ECDAA Random key.
        sss_status_t sss_status = kStatus_SSS_Fail;
        LOG_D("(Re)Generate ECDAA Random Key if required");
        sss_status = sss_se05x_generate_ecdaa_random_key(&context->session->s_ctx, &random_keyId);
        if (sss_status != kStatus_SSS_Success) {
            LOG_E("(Re)Generate ECDAA Random Key failed");
            return kStatus_SSS_Fail;
        }

        status = Se05x_API_ECDAASign(&context->session->s_ctx,
            context->keyObject->keyId,
            ecSignAlgo,
            digest,
            digestLen,
            random_keyId,
            raw_signature,
            &raw_signatureLen);
        if (status != SM_OK) {
            LOG_E("SE050 ECDAA Sign failed");
            return kStatus_SSS_Fail;
        }

        asn_retval = sss_util_asn1_ecdaa_get_signature(signature, signatureLen, raw_signature, raw_signatureLen);
        if (asn_retval != kStatus_SSS_Success) {
            LOG_E("SE050 ECDAA Sign failed");
            return kStatus_SSS_Fail;
        }
    } break;
#else // For 06.00 or lower version Applet
    case kSSS_CipherType_EC_BARRETO_NAEHRIG: {
        if (context->algorithm != kAlgorithm_SSS_ECDAA) {
            return kStatus_SSS_Fail;
        }

        uint8_t random[32] = {
            0,
        };
        size_t random_len = sizeof(random);
        uint8_t raw_signature[64];
        size_t raw_signatureLen = sizeof(raw_signature);
        SE05x_ECDAASignatureAlgo_t ecSignAlgo = kSE05x_ECDAASignatureAlgo_ECDAA;
        sss_status_t asn_retval = kStatus_SSS_Fail;

        status = Se05x_API_GetRandom(&context->session->s_ctx, (uint16_t)random_len, random, &random_len);
        if (status != SM_OK) {
            LOG_E("Se05x_API_GetRandom failed");
            return kStatus_SSS_Fail;
        }

        status = Se05x_API_ECDAASign(&context->session->s_ctx,
            context->keyObject->keyId,
            ecSignAlgo,
            digest,
            digestLen,
            random,
            sizeof(random),
            raw_signature,
            &raw_signatureLen);
        if (status != SM_OK) {
            LOG_E("SE050 ECDAA Sign failed");
            return kStatus_SSS_Fail;
        }

        asn_retval = sss_util_asn1_ecdaa_get_signature(signature, signatureLen, raw_signature, raw_signatureLen);
        if (asn_retval != kStatus_SSS_Success) {
            LOG_E("SE050 ECDAA Sign failed");
            return kStatus_SSS_Fail;
        }
    } break;
#endif
#endif // SSS_HAVE_TPM_BN && SSS_HAVE_ECDAA
#if SSS_HAVE_SE05X_VER_GTE_06_00 && SSS_HAVE_EC_MONT
    case kSSS_CipherType_EC_MONTGOMERY: {
        LOG_W(
            "Sign operation is not supported for "
            "kSSS_CipherType_EC_MONTGOMERY curve");
        return kStatus_SSS_Fail;
    } break;
#endif // SSS_HAVE_SE05X_VER_GTE_06_00 && SSS_HAVE_EC_MONT
#endif //SSSFTR_SE05X_ECC
#if SSSFTR_SE05X_RSA && SSS_HAVE_RSA
    case kSSS_CipherType_RSA:
    case kSSS_CipherType_RSA_CRT: {
        if ((context->algorithm <= kAlgorithm_SSS_RSASSA_PKCS1_PSS_MGF1_SHA512) &&
            (context->algorithm >= kAlgorithm_SSS_RSASSA_PKCS1_PSS_MGF1_SHA1)) {
            /* Perform EMSA encoding on input data and and RSA decrypt on emsa data --> RSA sign without hash */
            /* clang-format off */
            uint8_t emsa_data[512] = {0,}; /* MAX - SHA512*/
            size_t emsa_len = sizeof(emsa_data);
            /* clang-format on */

            if (0 != emsa_encode(context, digest, digestLen, emsa_data, &emsa_len)) {
                return kStatus_SSS_Fail;
            }
            status = Se05x_API_RSADecrypt(&context->session->s_ctx,
                context->keyObject->keyId,
                kSE05x_RSAEncryptionAlgo_NO_PAD,
                emsa_data,
                emsa_len,
                signature,
                signatureLen);
        }
        else if ((context->algorithm <= kAlgorithm_SSS_RSASSA_PKCS1_V1_5_SHA512) &&
                 (context->algorithm >= kAlgorithm_SSS_RSASSA_PKCS1_V1_5_SHA1)) {
            /* Perform PKCS1-v15 encoding on input data and and RSA decrypt on PKCS1-v15 data --> RSA sign without hash */
            /* clang-format off */
            uint8_t pkcs1v15_encode_data[512] = {0,}; /* MAX - SHA512*/
            size_t encode_data_len = sizeof(pkcs1v15_encode_data);
            /* clang-format on */

            if (0 != pkcs1_v15_encode(context, digest, digestLen, pkcs1v15_encode_data, &encode_data_len)) {
                return kStatus_SSS_Fail;
            }
            status = Se05x_API_RSADecrypt(&context->session->s_ctx,
                context->keyObject->keyId,
                kSE05x_RSAEncryptionAlgo_NO_PAD,
                pkcs1v15_encode_data,
                encode_data_len,
                signature,
                signatureLen);
        }
        else if (context->algorithm == kAlgorithm_SSS_RSASSA_PKCS1_V1_5_NO_HASH) {
            /* Perform PKCS1-v15 encoding on input data and and RSA decrypt on PKCS1-v15 data --> RSA sign without hash */
            /* clang-format off */
            uint8_t pkcs1v15_encode_data[512] = {0,}; /* MAX - SHA512*/
            size_t encode_data_len = sizeof(pkcs1v15_encode_data);
            /* clang-format on */

            if (0 != pkcs1_v15_encode_no_hash(context, digest, digestLen, pkcs1v15_encode_data, &encode_data_len)) {
                return kStatus_SSS_Fail;
            }
            status = Se05x_API_RSADecrypt(&context->session->s_ctx,
                context->keyObject->keyId,
                kSE05x_RSAEncryptionAlgo_NO_PAD,
                pkcs1v15_encode_data,
                encode_data_len,
                signature,
                signatureLen);
        }
        else if (context->algorithm == kAlgorithm_SSS_RSASSA_NO_PADDING) {
            uint8_t padded_data[512] = {0};
            size_t padded_len        = sizeof(padded_data);

            size_t parsedKeyByteLen      = 0;
            uint16_t u16parsedKeyByteLen = 0;
            status = Se05x_API_ReadSize(&context->session->s_ctx, context->keyObject->keyId, &u16parsedKeyByteLen);
            parsedKeyByteLen = u16parsedKeyByteLen;
            if (status != SM_OK) {
                return kStatus_SSS_Fail;
            }

            if (digestLen <= parsedKeyByteLen && digestLen > 0) {
                memset(padded_data, 0x00, padded_len);
                memcpy(&padded_data[parsedKeyByteLen - digestLen], &digest[0], digestLen);
                padded_len = parsedKeyByteLen;
            }
            else {
                return kStatus_SSS_Fail;
            }
            status = Se05x_API_RSADecrypt(&context->session->s_ctx,
                context->keyObject->keyId,
                kSE05x_RSAEncryptionAlgo_NO_PAD,
                padded_data,
                padded_len,
                signature,
                signatureLen);
        }
        else {
            LOG_E("Selected padding is not supported for RSA Sign in SE050");
            return kStatus_SSS_Fail;
        }
    } break;
#endif // SSSFTR_SE05X_RSA && SSS_HAVE_RSA
    default:
        break;
    }

    if (status == SM_OK) {
        retval = kStatus_SSS_Success;

#if 0  // SSS_HAVE_HOSTCRYPTO_MBEDTLS && SSSFTR_SE05X_ECC
        if (context->keyObject->cipherType >= kSSS_CipherType_EC_NIST_P &&
            context->keyObject->cipherType <
                kSSS_CipherType_EC_BARRETO_NAEHRIG) {
            int ret;
            /* Workaround for ECDSA signiture to omit prefix zeros if asn1
               signiutre tag (integer) length in R and S component is 20 */

            size_t length = 0, bufIndex = 0;
            ret = asn_1_parse_tlv(signature, &length, &bufIndex);
            if (ret != 0) {
                retval = kStatus_SSS_Fail;
                return retval;
            }
            if (signature[bufIndex] == 0x02) /* Check for tag interger */
            {
                LOG_AU8_D(signature, *signatureLen);

                int count = 0;
                uint16_t i = 0;
                /* For R and S component */
                for (i = 0; i < 2; i++) {
                    count = 0;
                    ret = asn_1_parse_tlv(signature, &length, &bufIndex);
                    if (ret != 0) {
                        retval = kStatus_SSS_Fail;
                        return retval;
                    }
                    if (length == 0x20) {
                        size_t j = bufIndex;
                        for (;; j++) {
                            if (signature[j] == 0 && signature[j + 1] > 0x7F) {
                                count++;
                            }
                            else {
                                break;
                            }
                        }
                    }
                    if (count) {
                        uint16_t k = 0;
                        signature[bufIndex - 1] -=
                            count; /* Update the tag length */
                        signature[1] -=
                            count; /* Update the Sequence tag length */

                        for (k = 0; k < (*signatureLen - bufIndex - count);
                             k++) {
                            signature[bufIndex + k] =
                                signature[bufIndex + count + k];
                        }

                        *signatureLen -= count;
                    }
                    bufIndex += length - count;
                }
            }
        }
#endif // SSS_HAVE_HOSTCRYPTO_MBEDTLS && SSSFTR_SE05X_ECC
    }

    return retval;
}

sss_status_t sss_se05x_asymmetric_sign(
    sss_se05x_asymmetric_t *context, uint8_t *srcData, size_t srcLen, uint8_t *destData, size_t *destLen)
{
    sss_status_t retval = kStatus_SSS_Fail;
#if (SSSFTR_SE05X_RSA && SSS_HAVE_RSA) || (SSSFTR_SE05X_ECC && SSS_HAVE_EC_ED)
    smStatus_t status = SM_NOT_OK;
#endif

    switch (context->keyObject->cipherType) {
#if SSSFTR_SE05X_RSA && SSS_HAVE_RSA
    case kSSS_CipherType_RSA:
    case kSSS_CipherType_RSA_CRT: {
        SE05x_RSASignatureAlgo_t rsaSigningAlgo = se05x_get_rsa_sign_hash_mode(context->algorithm);
        uint16_t key_size_bytes                 = 0;

        if (context->algorithm == kAlgorithm_SSS_RSASSA_PKCS1_V1_5_SHA512 ||
            context->algorithm == kAlgorithm_SSS_RSASSA_PKCS1_PSS_MGF1_SHA512) {
            status = Se05x_API_ReadSize(&context->session->s_ctx, context->keyObject->keyId, &key_size_bytes);
            if (status != SM_OK) {
                return kStatus_SSS_Fail;
            }

            if ((key_size_bytes * 8) == 512) {
                return kStatus_SSS_Fail;
            }
        }

        status = Se05x_API_RSASign(
            &context->session->s_ctx, context->keyObject->keyId, rsaSigningAlgo, srcData, srcLen, destData, destLen);
    } break;
#endif // SSSFTR_SE05X_RSA && SSS_HAVE_RSA
#if SSSFTR_SE05X_ECC && SSS_HAVE_EC_ED
    case kSSS_CipherType_EC_TWISTED_ED: {
        if (context->algorithm == kAlgorithm_SSS_SHA512) {
            SE05x_EDSignatureAlgo_t ecSignAlgo = kSE05x_EDSignatureAlgo_ED25519PURE_SHA_512;
            status                             = Se05x_API_EdDSASign(
                &context->session->s_ctx, context->keyObject->keyId, ecSignAlgo, srcData, srcLen, destData, destLen);
        }

#ifdef TMP_ENDIAN_VERBOSE_SIGN
        {
            printf("Signature before Reverse:\n");
            for (size_t z = 0; z < *destLen; z++) {
                printf("%02X.", destData[z]);
            }
            printf("\n");
        }
#endif

        // Revert Endianness
        size_t offset = 0;

        for (size_t keyValueIdx = 0; keyValueIdx < (*destLen >> 2); keyValueIdx++) {
            uint8_t swapByte                                     = destData[keyValueIdx];
            destData[offset + keyValueIdx]                       = destData[offset + (*destLen >> 1) - 1 - keyValueIdx];
            destData[offset + (*destLen >> 1) - 1 - keyValueIdx] = swapByte;
        }

        offset = *destLen >> 1;

        for (size_t keyValueIdx = 0; keyValueIdx < (*destLen >> 2); keyValueIdx++) {
            uint8_t swapByte                                     = destData[offset + keyValueIdx];
            destData[offset + keyValueIdx]                       = destData[offset + (*destLen >> 1) - 1 - keyValueIdx];
            destData[offset + (*destLen >> 1) - 1 - keyValueIdx] = swapByte;
        }

#ifdef TMP_ENDIAN_VERBOSE_SIGN
        {
            printf("Signature after Reverse:\n");
            for (size_t z = 0; z < *destLen; z++) {
                printf("%02X.", destData[z]);
            }
            printf("\n");
        }
#endif

    } break;
#endif // SSSFTR_SE05X_ECC && SSS_HAVE_EC_ED
    default:
        break;
    }

#if (SSSFTR_SE05X_RSA && SSS_HAVE_RSA) || (SSSFTR_SE05X_ECC && SSS_HAVE_EC_ED)
    // status is set only in case of RSA or ED.
    if (status == SM_OK) {
        retval = kStatus_SSS_Success;
    }
#endif

    return retval;
}

sss_status_t sss_se05x_asymmetric_verify_digest(
    sss_se05x_asymmetric_t *context, uint8_t *digest, size_t digestLen, uint8_t *signature, size_t signatureLen)
{
    sss_status_t retval = kStatus_SSS_Fail;
#if SSSFTR_SE05X_ECC || SSSFTR_SE05X_RSA
    smStatus_t status     = SM_NOT_OK;
    SE05x_Result_t result = kSE05x_Result_FAILURE;
#endif // SSSFTR_SE05X_ECC || SSSFTR_SE05X_RSA

#if SSSFTR_SE05X_ECC || SSSFTR_SE05X_RSA
    if (kStatus_SSS_Success != se05x_check_input_len(digestLen, context->algorithm)) {
        LOG_E("Algorithm and digest length do not match");
        return kStatus_SSS_Fail;
    }

    switch (context->keyObject->cipherType) {
#if SSSFTR_SE05X_ECC
    case kSSS_CipherType_EC_NIST_P:
#if SSS_HAVE_EC_NIST_K
    case kSSS_CipherType_EC_NIST_K:
#endif
#if SSS_HAVE_EC_BP
    case kSSS_CipherType_EC_BRAINPOOL:
#endif
    {
        SE05x_ECSignatureAlgo_t ecSignAlgo = se05x_get_ec_sign_hash_mode(context->algorithm);
        status                             = Se05x_API_ECDSAVerify(&context->session->s_ctx,
            context->keyObject->keyId,
            ecSignAlgo,
            digest,
            digestLen,
            signature,
            signatureLen,
            &result);
    } break;
#if SSS_HAVE_TPM_BN
    case kSSS_CipherType_EC_BARRETO_NAEHRIG: {
        retval = kStatus_SSS_Fail;
        LOG_W("Verify not supported for BN Curve");
    } break;
#endif
#if SSS_HAVE_SE05X_VER_GTE_06_00 && SSS_HAVE_EC_MONT
    case kSSS_CipherType_EC_MONTGOMERY: {
        LOG_W(
            "Verify operation is not supported for "
            "kSSS_CipherType_EC_MONTGOMERY curve");
        return kStatus_SSS_Fail;
    } break;
#endif // SSS_HAVE_SE05X_VER_GTE_06_00 && SSS_HAVE_EC_MONT
#endif // SSSFTR_SE05X_ECC
#if SSSFTR_SE05X_RSA && SSS_HAVE_RSA
    case kSSS_CipherType_RSA:
    case kSSS_CipherType_RSA_CRT: {
        if ((context->algorithm <= kAlgorithm_SSS_RSASSA_PKCS1_PSS_MGF1_SHA512) &&
            (context->algorithm >= kAlgorithm_SSS_RSASSA_PKCS1_PSS_MGF1_SHA1)) {
            /* clang-format off */
            uint8_t dec_data[512] = { 0, }; /* MAX - SHA512*/
            size_t dec_len = sizeof(dec_data);
            /* clang-format on */

            status = Se05x_API_RSAEncrypt(&context->session->s_ctx,
                context->keyObject->keyId,
                kSE05x_RSAEncryptionAlgo_NO_PAD,
                signature,
                signatureLen,
                dec_data,
                &dec_len);

            if (0 == emsa_decode_and_compare(context, dec_data, dec_len, digest, digestLen)) {
                result = kSE05x_Result_SUCCESS;
            }
        }
        else if ((context->algorithm <= kAlgorithm_SSS_RSASSA_PKCS1_V1_5_SHA512) &&
                 (context->algorithm >= kAlgorithm_SSS_RSASSA_PKCS1_V1_5_SHA1)) {
            /* clang-format off */
            uint8_t dec_data[512] = { 0, }; /* MAX - SHA512*/
            size_t dec_len = sizeof(dec_data);
            uint8_t pkcs1v15_encode_data[512] = { 0, }; /* MAX - SHA512*/
            size_t encode_data_len = sizeof(pkcs1v15_encode_data);
            /* clang-format on */

            status = Se05x_API_RSAEncrypt(&context->session->s_ctx,
                context->keyObject->keyId,
                kSE05x_RSAEncryptionAlgo_NO_PAD,
                signature,
                signatureLen,
                dec_data,
                &dec_len);

            if (0 != pkcs1_v15_encode(context, digest, digestLen, pkcs1v15_encode_data, &encode_data_len)) {
                return kStatus_SSS_Fail;
            }

            if (memcmp(dec_data, pkcs1v15_encode_data, encode_data_len) == 0) {
                result = kSE05x_Result_SUCCESS;
            }
        }
        else if (context->algorithm == kAlgorithm_SSS_RSASSA_PKCS1_V1_5_NO_HASH) {
            /* clang-format off */
            uint8_t dec_data[512] = { 0, }; /* MAX - SHA512*/
            size_t dec_len = sizeof(dec_data);
            uint8_t pkcs1v15_encode_data[512] = { 0, }; /* MAX - SHA512*/
            size_t encode_data_len = sizeof(pkcs1v15_encode_data);
            /* clang-format on */

            status = Se05x_API_RSAEncrypt(&context->session->s_ctx,
                context->keyObject->keyId,
                kSE05x_RSAEncryptionAlgo_NO_PAD,
                signature,
                signatureLen,
                dec_data,
                &dec_len);

            if (0 != pkcs1_v15_encode_no_hash(context, digest, digestLen, pkcs1v15_encode_data, &encode_data_len)) {
                return kStatus_SSS_Fail;
            }

            if (memcmp(dec_data, pkcs1v15_encode_data, encode_data_len) == 0) {
                result = kSE05x_Result_SUCCESS;
            }
        }
        else if (context->algorithm == kAlgorithm_SSS_RSASSA_NO_PADDING) {
            uint8_t dec_data[512] = {
                0,
            }; /*MAX - RSA4096*/
            size_t dec_len = sizeof(dec_data);

            status = Se05x_API_RSAEncrypt(&context->session->s_ctx,
                context->keyObject->keyId,
                kSE05x_RSAEncryptionAlgo_NO_PAD,
                signature,
                signatureLen,
                dec_data,
                &dec_len);

            uint8_t padded_data[512] = {0};
            size_t padded_len        = sizeof(padded_data);

            size_t parsedKeyByteLen      = 0;
            uint16_t u16parsedKeyByteLen = 0;
            status = Se05x_API_ReadSize(&context->session->s_ctx, context->keyObject->keyId, &u16parsedKeyByteLen);
            parsedKeyByteLen = u16parsedKeyByteLen;
            if (status != SM_OK) {
                return kStatus_SSS_Fail;
            }

            if (digestLen <= parsedKeyByteLen && digestLen > 0) {
                memset(padded_data, 0x00, padded_len);
                memcpy(&padded_data[parsedKeyByteLen - digestLen], &digest[0], digestLen);
                padded_len = parsedKeyByteLen;
            }

            else {
                return kStatus_SSS_Fail;
            }

            if (memcmp(&dec_data[0], &padded_data[0], padded_len) == 0) {
                result = kSE05x_Result_SUCCESS;
            }
        }
        else {
            LOG_E("Selected padding is not supported for RSA Sign in SE050");
            return kStatus_SSS_Fail;
        }

    } break;
#endif // SSSFTR_SE05X_RSA && SSS_HAVE_RSA
    default:
        break;
    }
#endif // SSSFTR_SE05X_ECC || SSSFTR_SE05X_RSA

#if SSSFTR_SE05X_ECC || SSSFTR_SE05X_RSA
    if (status == SM_OK) {
        if (result == kSE05x_Result_SUCCESS) {
            retval = kStatus_SSS_Success;
        }
    }
#endif // SSSFTR_SE05X_ECC || SSSFTR_SE05X_RSA

    return retval;
}

sss_status_t sss_se05x_asymmetric_verify(
    sss_se05x_asymmetric_t *context, uint8_t *srcData, size_t srcLen, uint8_t *signature, size_t signatureLen)
{
    sss_status_t retval = kStatus_SSS_Fail;
#if (SSSFTR_SE05X_RSA && SSS_HAVE_RSA) || (SSSFTR_SE05X_ECC && SSS_HAVE_EC_ED)
    smStatus_t status     = SM_NOT_OK;
    SE05x_Result_t result = kSE05x_Result_FAILURE;
#endif

    switch (context->keyObject->cipherType) {
#if SSSFTR_SE05X_RSA && SSS_HAVE_RSA
    case kSSS_CipherType_RSA:
    case kSSS_CipherType_RSA_CRT: {
        SE05x_RSASignatureAlgo_t rsaSigningAlgo = se05x_get_rsa_sign_hash_mode(context->algorithm);
        uint16_t key_size_bytes                 = 0;

        if (context->algorithm == kAlgorithm_SSS_RSASSA_PKCS1_V1_5_SHA512 ||
            context->algorithm == kAlgorithm_SSS_RSASSA_PKCS1_PSS_MGF1_SHA512) {
            status = Se05x_API_ReadSize(&context->session->s_ctx, context->keyObject->keyId, &key_size_bytes);
            if (status != SM_OK) {
                return kStatus_SSS_Fail;
            }

            if ((key_size_bytes * 8) == 512) {
                return kStatus_SSS_Fail;
            }
        }

        status = Se05x_API_RSAVerify(&context->session->s_ctx,
            context->keyObject->keyId,
            rsaSigningAlgo,
            srcData,
            srcLen,
            signature,
            signatureLen,
            &result);
    } break;
#endif // SSSFTR_SE05X_RSA && SSS_HAVE_RSA
#if SSSFTR_SE05X_ECC && SSS_HAVE_EC_ED
    case kSSS_CipherType_EC_TWISTED_ED: {
#ifdef TMP_ENDIAN_VERBOSE
        {
            printf("Signatire before Reverse:\n");
            for (size_t z = 0; z < signatureLen; z++) {
                printf("%02X.", signature[z]);
            }
            printf("\n");
        }
#endif

        // Revert Endianness
        size_t offset = 0;

        for (size_t keyValueIdx = 0; keyValueIdx < (signatureLen >> 2); keyValueIdx++) {
            uint8_t swapByte                = signature[keyValueIdx];
            signature[offset + keyValueIdx] = signature[offset + (signatureLen >> 1) - 1 - keyValueIdx];
            signature[offset + (signatureLen >> 1) - 1 - keyValueIdx] = swapByte;
        }

        offset = signatureLen >> 1;

        for (size_t keyValueIdx = 0; keyValueIdx < (signatureLen >> 2); keyValueIdx++) {
            uint8_t swapByte                = signature[offset + keyValueIdx];
            signature[offset + keyValueIdx] = signature[offset + (signatureLen >> 1) - 1 - keyValueIdx];
            signature[offset + (signatureLen >> 1) - 1 - keyValueIdx] = swapByte;
        }

#ifdef TMP_ENDIAN_VERBOSE
        {
            printf("Signatire after Reverse:\n");
            for (size_t z = 0; z < signatureLen; z++) {
                printf("%02X.", signature[z]);
            }
            printf("\n");
        }
#endif

        if (context->algorithm == kAlgorithm_SSS_SHA512) {
            SE05x_EDSignatureAlgo_t ecSignAlgo = kSE05x_EDSignatureAlgo_ED25519PURE_SHA_512;
            status                             = Se05x_API_EdDSAVerify(&context->session->s_ctx,
                context->keyObject->keyId,
                ecSignAlgo,
                srcData,
                srcLen,
                signature,
                signatureLen,
                &result);
        }
    } break;
#endif // SSSFTR_SE05X_ECC && SSS_HAVE_EC_ED
    default:
        break;
    }

#if ((SSSFTR_SE05X_RSA && SSS_HAVE_RSA) || (SSSFTR_SE05X_ECC && SSS_HAVE_EC_ED))
    // status is set only in case of RSA or ED.
    if (status == SM_OK) {
        if (result == kSE05x_Result_SUCCESS) {
            retval = kStatus_SSS_Success;
        }
    }
#endif

    return retval;
}

void sss_se05x_asymmetric_context_free(sss_se05x_asymmetric_t *context)
{
    memset(context, 0, sizeof(*context));
}

/* End: se05x_asym */

/* ************************************************************************** */
/* Functions : sss_se05x_symm                                                 */
/* ************************************************************************** */

sss_status_t sss_se05x_symmetric_context_init(sss_se05x_symmetric_t *context,
    sss_se05x_session_t *session,
    sss_se05x_object_t *keyObject,
    sss_algorithm_t algorithm,
    sss_mode_t mode)
{
    sss_status_t retval     = kStatus_SSS_Success;
    context->session        = session;
    context->keyObject      = keyObject;
    context->algorithm      = algorithm;
    context->mode           = mode;
    context->cache_data_len = 0;
    return retval;
}

sss_status_t sss_se05x_cipher_one_go(sss_se05x_symmetric_t *context,
    uint8_t *iv,
    size_t ivLen,
    const uint8_t *srcData,
    uint8_t *destData,
    size_t dataLen)
{
    sss_status_t retval           = kStatus_SSS_Fail;
    smStatus_t status             = SM_NOT_OK;
    SE05x_CipherMode_t cipherMode = se05x_get_cipher_mode(context->algorithm);
    SE05x_Cipher_Oper_OneShot_t OperType =
        (context->mode == kMode_SSS_Encrypt) ? kSE05x_Cipher_Oper_OneShot_Encrypt : kSE05x_Cipher_Oper_OneShot_Decrypt;
    size_t blockLenModulus = dataLen % CIPHER_BLOCK_SIZE;

    ENSURE_OR_GO_EXIT(cipherMode != kSE05x_CipherMode_NA);
    ENSURE_OR_GO_EXIT(
        !((context->algorithm == kAlgorithm_SSS_AES_CTR_INT_IV) && (OperType == kSE05x_Cipher_Oper_OneShot_Decrypt)));

    if ((context->algorithm == kAlgorithm_SSS_AES_CTR) || (context->algorithm == kAlgorithm_SSS_AES_CTR_INT_IV)) {
        ENSURE_OR_GO_EXIT(blockLenModulus == 0);
    }

    status = Se05x_API_CipherOneShot(&context->session->s_ctx,
        context->keyObject->keyId,
        cipherMode,
        srcData,
        dataLen,
        iv,
        ivLen,
        destData,
        &dataLen,
        OperType);

    ENSURE_OR_GO_EXIT(status == SM_OK);

    retval = kStatus_SSS_Success;
exit:
    return retval;
}

sss_status_t sss_se05x_cipher_one_go_v2(sss_se05x_symmetric_t *context,
    uint8_t *iv,
    size_t ivLen,
    const uint8_t *srcData,
    const size_t srcLen,
    uint8_t *destData,
    size_t *pDataLen)
{
    sss_status_t retval           = kStatus_SSS_Fail;
    smStatus_t status             = SM_NOT_OK;
    SE05x_CipherMode_t cipherMode = se05x_get_cipher_mode(context->algorithm);
    SE05x_Cipher_Oper_OneShot_t OperType =
        (context->mode == kMode_SSS_Encrypt) ? kSE05x_Cipher_Oper_OneShot_Encrypt : kSE05x_Cipher_Oper_OneShot_Decrypt;
    size_t blockLenModulus = srcLen % CIPHER_BLOCK_SIZE;

    ENSURE_OR_GO_EXIT(cipherMode != kSE05x_CipherMode_NA);
    ENSURE_OR_GO_EXIT(
        !((context->algorithm == kAlgorithm_SSS_AES_CTR_INT_IV) && (OperType == kSE05x_Cipher_Oper_OneShot_Decrypt)));

    if ((context->algorithm == kAlgorithm_SSS_AES_CTR) || (context->algorithm == kAlgorithm_SSS_AES_CTR_INT_IV)) {
        ENSURE_OR_GO_EXIT(blockLenModulus == 0);
    }

    status = Se05x_API_CipherOneShot(&context->session->s_ctx,
        context->keyObject->keyId,
        cipherMode,
        srcData,
        srcLen,
        iv,
        ivLen,
        destData,
        pDataLen,
        OperType);

    ENSURE_OR_GO_EXIT(status == SM_OK);

    retval = kStatus_SSS_Success;
exit:
    return retval;
}

sss_status_t sss_se05x_cipher_init(sss_se05x_symmetric_t *context, uint8_t *iv, size_t ivLen)
{
    sss_status_t retval = kStatus_SSS_Fail;
    smStatus_t status;
    //size_t retdataLen = 0;
    SE05x_Cipher_Oper_t OperType =
        (context->mode == kMode_SSS_Encrypt) ? kSE05x_Cipher_Oper_Encrypt : kSE05x_Cipher_Oper_Decrypt;
    SE05x_CipherMode_t cipherMode = se05x_get_cipher_mode(context->algorithm);

#if SSSFTR_SE05X_CREATE_DELETE_CRYPTOOBJ
    SE05x_CryptoModeSubType_t subtype;
    uint8_t list[1024] = {
        0,
    };
    size_t listlen = sizeof(list);
    size_t i;
    uint8_t create_crypto_obj = 1;

    ENSURE_OR_GO_EXIT(cipherMode != kSE05x_CipherMode_NA);
    ENSURE_OR_GO_EXIT(
        !((context->algorithm == kAlgorithm_SSS_AES_CTR_INT_IV) && (OperType == kSE05x_Cipher_Oper_Decrypt)))

    subtype.cipher = (cipherMode == kSE05x_CipherMode_AES_CTR_INT_IV) ? kSE05x_CipherMode_AES_CTR : cipherMode;
    switch (context->algorithm) {
    case kAlgorithm_SSS_AES_ECB:
        context->cryptoObjectId = kSE05x_CryptoObject_AES_ECB_NOPAD;
        break;
    case kAlgorithm_SSS_AES_CBC:
        context->cryptoObjectId = kSE05x_CryptoObject_AES_CBC_NOPAD;
        break;
    case kAlgorithm_SSS_AES_CTR:
        context->cryptoObjectId = kSE05x_CryptoObject_AES_CTR;
        break;
    case kAlgorithm_SSS_AES_CTR_INT_IV:
        context->cryptoObjectId = kSE05x_CryptoObject_AES_CTR_INT_IV;
        break;
    case kAlgorithm_SSS_DES_ECB:
    case kAlgorithm_SSS_DES3_ECB:
        context->cryptoObjectId = kSE05x_CryptoObject_DES_ECB_NOPAD;
        break;
    case kAlgorithm_SSS_DES_CBC:
    case kAlgorithm_SSS_DES3_CBC:
        context->cryptoObjectId = kSE05x_CryptoObject_DES_CBC_NOPAD;
        break;
    case kAlgorithm_SSS_DES_CBC_ISO9797_M1:
    case kAlgorithm_SSS_DES3_CBC_ISO9797_M1:
        context->cryptoObjectId = kSE05x_CryptoObject_DES_CBC_ISO9797_M1;
        break;
    case kAlgorithm_SSS_DES_CBC_ISO9797_M2:
    case kAlgorithm_SSS_DES3_CBC_ISO9797_M2:
        context->cryptoObjectId = kSE05x_CryptoObject_DES_CBC_ISO9797_M2;
        break;
    default:
        return kStatus_SSS_Fail;
    }

    status = Se05x_API_ReadCryptoObjectList(&context->session->s_ctx, list, &listlen);
    for (i = 0; i < listlen; i += 4) {
        uint16_t cryptoObjectId = list[i + 1] | (list[i + 0] << 8);
        if (cryptoObjectId == context->cryptoObjectId) {
            create_crypto_obj = 0;
        }
    }

    if (create_crypto_obj) {
        status = Se05x_API_CreateCryptoObject(
            &context->session->s_ctx, context->cryptoObjectId, kSE05x_CryptoContext_CIPHER, subtype);
        if (status != SM_OK) {
            return kStatus_SSS_Fail;
        }
    }
#endif

    /* ivLen = 0 for all ECB operations */
    if (cipherMode == kSE05x_CipherMode_AES_ECB_NOPAD || cipherMode == kSE05x_CipherMode_DES_ECB_NOPAD ||
        cipherMode == kSE05x_CipherMode_DES_ECB_ISO9797_M1 || cipherMode == kSE05x_CipherMode_DES_ECB_ISO9797_M2) {
        ivLen = 0;
    }

    status = Se05x_API_CipherInit(
        &context->session->s_ctx, context->keyObject->keyId, context->cryptoObjectId, iv, ivLen, OperType);
    ENSURE_OR_GO_EXIT(status == SM_OK);

    retval = kStatus_SSS_Success;
exit:
    return retval;
}

sss_status_t sss_se05x_cipher_update(
    sss_se05x_symmetric_t *context, const uint8_t *srcData, size_t srcLen, uint8_t *destData, size_t *destLen)
{
    sss_status_t retval    = kStatus_SSS_Fail;
    smStatus_t status      = SM_NOT_OK;
    size_t inputData_len   = 0;
    size_t src_offset      = 0;
    size_t output_offset   = 0;
    size_t blockoutLen     = 0;
    size_t outBuffSize     = 0;
    size_t cipherBlockSize = CIPHER_BLOCK_SIZE;

    if (context->algorithm == kAlgorithm_SSS_DES_ECB || context->algorithm == kAlgorithm_SSS_DES_CBC ||
        context->algorithm == kAlgorithm_SSS_DES3_ECB || context->algorithm == kAlgorithm_SSS_DES3_CBC) {
        cipherBlockSize = DES_BLOCK_SIZE;
    }

    ENSURE_OR_GO_EXIT(srcData != NULL);
    ENSURE_OR_GO_EXIT(destData != NULL);
    ENSURE_OR_GO_EXIT(destLen != NULL);
    ENSURE_OR_GO_EXIT(srcLen > 0);

    outBuffSize     = *destLen;

    /* Check overflow */
    ENSURE_OR_GO_EXIT((context->cache_data_len + srcLen) >= context->cache_data_len);

    if ((context->cache_data_len + srcLen) < cipherBlockSize) {
        /* Insufficinet data to process . Cache the data */
        memcpy((context->cache_data + context->cache_data_len), srcData, srcLen);
        context->cache_data_len = context->cache_data_len + srcLen;
        *destLen                = 0;
        return kStatus_SSS_Success;
    }
    else {
        if (context->cache_data_len > 0) {
            if (cipherBlockSize - context->cache_data_len > 0) {
                memcpy((context->cache_data + context->cache_data_len),
                    srcData,
                    (cipherBlockSize - context->cache_data_len));
            }

            blockoutLen = outBuffSize;
            status      = Se05x_API_CipherUpdate(&context->session->s_ctx,
                context->cryptoObjectId,
                context->cache_data,
                cipherBlockSize,
                destData,
                &blockoutLen);
            ENSURE_OR_GO_EXIT(status == SM_OK);

            src_offset = cipherBlockSize - context->cache_data_len;
            outBuffSize -= blockoutLen;
            output_offset += blockoutLen;

            context->cache_data_len = 0;
        }

        while (srcLen - src_offset >= cipherBlockSize) {
            size_t rem_srcData = srcLen - src_offset;

            if (rem_srcData > CIPHER_UPDATE_MAX_DATA) {
                inputData_len = CIPHER_UPDATE_MAX_DATA;
            }
            else {
                inputData_len = rem_srcData - (rem_srcData % cipherBlockSize);
            }

            blockoutLen = outBuffSize;
            status      = Se05x_API_CipherUpdate(&context->session->s_ctx,
                context->cryptoObjectId,
                (srcData + src_offset),
                inputData_len,
                (destData + output_offset),
                &blockoutLen);
            ENSURE_OR_GO_EXIT(status == SM_OK);

            src_offset += inputData_len;
            outBuffSize -= blockoutLen;
            output_offset += blockoutLen;
        }

        *destLen = output_offset;

        /* Copy unprocessed data to cache */
        if ((srcLen - src_offset) > 0) {
            memcpy(context->cache_data, (srcData + src_offset), (srcLen - src_offset));
            context->cache_data_len = (srcLen - src_offset);
        }
    }

    retval = kStatus_SSS_Success;
exit:
    if (retval == kStatus_SSS_Fail) {
        if (destLen) {
            *destLen = 0;
        }
    }
    return retval;
}

sss_status_t sss_se05x_cipher_finish(
    sss_se05x_symmetric_t *context, const uint8_t *srcData, size_t srcLen, uint8_t *destData, size_t *destLen)
{
    sss_status_t retval                            = kStatus_SSS_Fail;
    smStatus_t status                              = SM_NOT_OK;
    uint8_t srcdata_updated[2 * CIPHER_BLOCK_SIZE] = {
        0,
    };
    size_t srcdata_updated_len = 0;
    size_t cipherBlockSize     = CIPHER_BLOCK_SIZE;

    if (context->algorithm == kAlgorithm_SSS_DES_ECB || context->algorithm == kAlgorithm_SSS_DES_CBC ||
        context->algorithm == kAlgorithm_SSS_DES3_ECB || context->algorithm == kAlgorithm_SSS_DES3_CBC) {
        cipherBlockSize = DES_BLOCK_SIZE;
    }

    if (srcLen > cipherBlockSize) {
        LOG_E("srcLen cannot be grater than 16 bytes. Call update function ");
        *destLen = 0;
        goto exit;
    }

    if (context->cache_data_len != 0) {
        memcpy(srcdata_updated, context->cache_data, context->cache_data_len);
        srcdata_updated_len     = context->cache_data_len;
        context->cache_data_len = 0;
    }
    if (srcLen != 0) {
        memcpy((srcdata_updated + srcdata_updated_len), srcData, srcLen);
        srcdata_updated_len += srcLen;
    }

    if (context->algorithm == kAlgorithm_SSS_AES_ECB || context->algorithm == kAlgorithm_SSS_AES_CBC) {
        if (srcdata_updated_len > 0) {
            if (srcdata_updated_len % cipherBlockSize != 0) {
                srcdata_updated_len = srcdata_updated_len + (cipherBlockSize - (srcdata_updated_len % cipherBlockSize));
            }
        }
    }

    if (*destLen < srcdata_updated_len) {
        LOG_E("Output buffer not sufficient");
        goto exit;
    }

    status = Se05x_API_CipherFinal(
        &context->session->s_ctx, context->cryptoObjectId, srcdata_updated, srcdata_updated_len, destData, destLen);
    ENSURE_OR_GO_EXIT(status == SM_OK);

    retval = kStatus_SSS_Success;
exit:
    return retval;
}

sss_status_t sss_se05x_cipher_crypt_ctr(sss_se05x_symmetric_t *context,
    const uint8_t *srcData,
    uint8_t *destData,
    size_t size,
    uint8_t *initialCounter,
    uint8_t *lastEncryptedCounter,
    size_t *szLeft)
{
    AX_UNUSED_ARG(lastEncryptedCounter);
    AX_UNUSED_ARG(szLeft);
    sss_status_t retval           = kStatus_SSS_Fail;
    smStatus_t status             = SM_NOT_OK;
    size_t outputDataLen          = size;
    SE05x_CipherMode_t cipherMode = se05x_get_cipher_mode(context->algorithm);
    SE05x_Cipher_Oper_OneShot_t OperType =
        (context->mode == kMode_SSS_Encrypt) ? kSE05x_Cipher_Oper_OneShot_Encrypt : kSE05x_Cipher_Oper_OneShot_Decrypt;
    size_t blockLenModulus = size % CIPHER_BLOCK_SIZE;

    ENSURE_OR_GO_EXIT(cipherMode != kSE05x_CipherMode_NA);
    ENSURE_OR_GO_EXIT(
        !((context->algorithm == kAlgorithm_SSS_AES_CTR_INT_IV) && (OperType == kSE05x_Cipher_Oper_OneShot_Decrypt)));

    if ((context->algorithm == kAlgorithm_SSS_AES_CTR) || (context->algorithm == kAlgorithm_SSS_AES_CTR_INT_IV)) {
        ENSURE_OR_GO_EXIT(blockLenModulus == 0);
    }

    status = Se05x_API_CipherOneShot(&context->session->s_ctx,
        context->keyObject->keyId,
        cipherMode,
        srcData,
        size,
        initialCounter,
        16,
        destData,
        &outputDataLen,
        OperType);

    ENSURE_OR_GO_EXIT(status == SM_OK);

    retval = kStatus_SSS_Success;
exit:
    return retval;
}

void sss_se05x_symmetric_context_free(sss_se05x_symmetric_t *context)
{
#if SSSFTR_SE05X_CREATE_DELETE_CRYPTOOBJ
    smStatus_t status;
    uint8_t list[1024] = {
        0,
    };
    uint8_t object_exists = 0;
    size_t listlen        = sizeof(list);

    if (context->cryptoObjectId != 0) {
        status = Se05x_API_ReadCryptoObjectList(&context->session->s_ctx, list, &listlen);
        for (size_t i = 0; i < listlen; i += 4) {
            uint16_t cryptoObjectId = list[i + 1] | (list[i + 0] << 8);
            if (cryptoObjectId == context->cryptoObjectId) {
                object_exists = 1;
            }
        }

        if (object_exists) {
            status = Se05x_API_DeleteCryptoObject(&context->session->s_ctx, context->cryptoObjectId);
            if (status != SM_OK) {
                LOG_D("Could not delete crypto object 0x04X", context->cryptoObjectId);
                return;
            }
        }
    }
#endif
    memset(context, 0, sizeof(*context));
}

/* End: se05x_symm */

/* ************************************************************************** */
/* Functions : sss_se05x_aead                                                 */
/* ************************************************************************** */

sss_status_t sss_se05x_aead_context_init(sss_se05x_aead_t *context,
    sss_se05x_session_t *session,
    sss_se05x_object_t *keyObject,
    sss_algorithm_t algorithm,
    sss_mode_t mode)
{
    sss_status_t retval = kStatus_SSS_Fail;
    context->session    = session;
    context->keyObject  = keyObject;
    if ((algorithm == kAlgorithm_SSS_AES_CCM) || (algorithm == kAlgorithm_SSS_AES_GCM) ||
        (algorithm == kAlgorithm_SSS_AES_GCM_INT_IV) || (algorithm == kAlgorithm_SSS_AES_CCM_INT_IV)) {
        context->algorithm = algorithm;
    }
    else {
        LOG_E("Improper Algorithm provided!!!");
        goto exit;
    }
    context->mode = mode;
    retval        = kStatus_SSS_Success;
exit:
    return retval;
}

sss_status_t sss_se05x_aead_one_go(sss_se05x_aead_t *context,
    const uint8_t *srcData,
    uint8_t *destData,
    size_t size,
    uint8_t *nonce,
    size_t nonceLen,
    const uint8_t *aad,
    size_t aadLen,
    uint8_t *tag,
    size_t *tagLen)
{
    sss_status_t retval = kStatus_SSS_Fail;
#if SSS_HAVE_SE05X_VER_GTE_06_00
    smStatus_t status  = SM_NOT_OK;
    size_t destDataLen = size;
    SE05x_CipherMode_t cipherMode =
        (context->algorithm == kAlgorithm_SSS_AES_GCM) ? kSE05x_CipherMode_AES_GCM : kSE05x_CipherMode_AES_GCM_INT_IV;
    SE05x_Cipher_Oper_OneShot_t OperType =
        (context->mode == kMode_SSS_Encrypt) ? kSE05x_Cipher_Oper_OneShot_Encrypt : kSE05x_Cipher_Oper_OneShot_Decrypt;

    // Not support decrypt with internal IV.
    ENSURE_OR_GO_EXIT(
        !((context->algorithm == kAlgorithm_SSS_AES_GCM_INT_IV) && (OperType == kSE05x_Cipher_Oper_OneShot_Decrypt)));

    status = Se05x_API_AeadOneShot(&context->session->s_ctx,
        context->keyObject->keyId,
        cipherMode,
        srcData,
        size,
        aad,
        aadLen,
        nonce,
        nonceLen,
        tag,
        tagLen,
        destData,
        &destDataLen,
        OperType);
    ENSURE_OR_GO_EXIT(status == SM_OK);

    retval = kStatus_SSS_Success;
exit:
#endif /* SSS_HAVE_SE05X_VER_GTE_06_00 */
    return retval;
}

sss_status_t sss_se05x_aead_init(
    sss_se05x_aead_t *context, uint8_t *nonce, size_t nonceLen, size_t tagLen, size_t aadLen, size_t payloadLen)
{
    sss_status_t retval = kStatus_SSS_Fail;
#if SSS_HAVE_SE05X_VER_GTE_06_00
    smStatus_t status             = SM_NOT_OK;
    context->cache_data_len       = 0;
    SE05x_CipherMode_t cipherMode = kSE05x_CipherMode_NA;
    SE05x_Cipher_Oper_t OperType =
        (context->mode == kMode_SSS_Encrypt) ? kSE05x_Cipher_Oper_Encrypt : kSE05x_Cipher_Oper_Decrypt;
#if SSSFTR_SE05X_CREATE_DELETE_CRYPTOOBJ
    SE05x_CryptoModeSubType_t subtype;
    uint8_t list[1024] = {
        0,
    };
    size_t listlen = sizeof(list);
    size_t i;
    uint8_t create_crypto_obj = 1;

    // Not support decrypt with internal IV.
    ENSURE_OR_GO_EXIT(
        !((context->algorithm == kAlgorithm_SSS_AES_GCM_INT_IV) && (OperType == kSE05x_Cipher_Oper_Decrypt)));
    ENSURE_OR_GO_EXIT(
        !((context->algorithm == kAlgorithm_SSS_AES_CCM_INT_IV) && (OperType == kSE05x_Cipher_Oper_Decrypt)));

    if (context->algorithm == kAlgorithm_SSS_AES_GCM) {
        context->cryptoObjectId = kSE05x_CryptoObject_AES_GCM;
        subtype.aead            = kSE05x_AeadGCMAlgo;
    }
    else if (context->algorithm == kAlgorithm_SSS_AES_GCM_INT_IV) {
        context->cryptoObjectId = kSE05x_CryptoObject_AES_GCM_INT_IV;
#if SSS_HAVE_SE05X_VER_GTE_07_02
        subtype.aead = kSE05x_AeadGCMAlgo;
#else
        subtype.aead = kSE05x_AeadGCM_IVAlgo;
#endif
    }
    else if (context->algorithm == kAlgorithm_SSS_AES_CCM) {
        context->cryptoObjectId = kSE05x_CryptoObject_AES_CCM;
        subtype.aead            = kSE05x_AeadCCMAlgo;
    }
    else if (context->algorithm == kAlgorithm_SSS_AES_CCM_INT_IV) {
        context->cryptoObjectId = kSE05x_CryptoObject_AES_CCM_INT_IV;
        subtype.aead            = kSE05x_AeadCCMAlgo;
    }
    else {
        goto exit;
    }
    status = Se05x_API_ReadCryptoObjectList(&context->session->s_ctx, list, &listlen);
    for (i = 0; i < listlen; i += 4) {
        uint16_t cryptoObjectId = list[i + 1] | (list[i + 0] << 8);
        if (cryptoObjectId == context->cryptoObjectId) {
            create_crypto_obj = 0;
        }
    }

    if (create_crypto_obj) {
        status = Se05x_API_CreateCryptoObject(
            &context->session->s_ctx, context->cryptoObjectId, kSE05x_CryptoContext_AEAD, subtype);
        if (status != SM_OK) {
            return kStatus_SSS_Fail;
        }
    }

    if (status != SM_OK) {
        LOG_W("CreateCryptoObject Failed");
        return kStatus_SSS_Fail;
    }
#endif
    memset(context->cache_data, 0x00, sizeof(context->cache_data));
    if ((context->algorithm == (kAlgorithm_SSS_AES_GCM)) || (context->algorithm == (kAlgorithm_SSS_AES_GCM_INT_IV))) {
        cipherMode = (context->algorithm == kAlgorithm_SSS_AES_GCM) ? kSE05x_CipherMode_AES_GCM :
                                                                      kSE05x_CipherMode_AES_GCM_INT_IV;
        status = Se05x_API_AeadInit(&context->session->s_ctx,
            context->keyObject->keyId,
            cipherMode,
            context->cryptoObjectId,
            nonce,
            nonceLen,
            OperType);
    }
    else {
        cipherMode = (context->algorithm == kAlgorithm_SSS_AES_CCM) ? kSE05x_CipherMode_AES_CCM :
                                                                      kSE05x_CipherMode_AES_CCM_INT_IV;
        status = Se05x_API_AeadCCMInit(&context->session->s_ctx,
            context->keyObject->keyId,
            cipherMode,
            context->cryptoObjectId,
            nonce,
            nonceLen,
            aadLen,
            payloadLen,
            tagLen,
            OperType);
    }
    ENSURE_OR_GO_EXIT(status == SM_OK);

    retval = kStatus_SSS_Success;
exit:
#endif /* SSS_HAVE_SE05X_VER_GTE_06_00 */
    return retval;
}

sss_status_t sss_se05x_aead_update_aad(sss_se05x_aead_t *context, const uint8_t *aadData, size_t aadDataLen)
{
    sss_status_t retval = kStatus_SSS_Fail;
#if SSS_HAVE_SE05X_VER_GTE_06_00
    smStatus_t status = SM_NOT_OK;
    size_t src_offset = 0;
    if (aadDataLen > AEAD_BLOCK_SIZE) {
        while ((aadDataLen - src_offset) >= AEAD_BLOCK_SIZE) {
            /*For the subsequent blocks which are of block size 16*/
            status = Se05x_API_AeadUpdate_aad(
                &context->session->s_ctx, context->cryptoObjectId, (aadData + src_offset), AEAD_BLOCK_SIZE);
            ENSURE_OR_GO_EXIT(status == SM_OK);
            src_offset += AEAD_BLOCK_SIZE;
        }
        if ((aadDataLen - src_offset) > 0) {
            /*For the subsequent blocks which are yet to process*/
            status = Se05x_API_AeadUpdate_aad(
                &context->session->s_ctx, context->cryptoObjectId, (aadData + src_offset), (aadDataLen - src_offset));
            ENSURE_OR_GO_EXIT(status == SM_OK);
        }
    }
    else {
        status = Se05x_API_AeadUpdate_aad(&context->session->s_ctx, context->cryptoObjectId, aadData, aadDataLen);
        ENSURE_OR_GO_EXIT(status == SM_OK);
    }
    retval = kStatus_SSS_Success;
exit:
#endif /* SSS_HAVE_SE05X_VER_GTE_06_00 */
    return retval;
}

sss_status_t sss_se05x_aead_update(
    sss_se05x_aead_t *context, const uint8_t *srcData, size_t srcLen, uint8_t *destData, size_t *destLen)
{
    sss_status_t retval = kStatus_SSS_Fail;
#if SSS_HAVE_SE05X_VER_GTE_06_00
    smStatus_t status    = SM_NOT_OK;
    size_t inputData_len = 0;
    size_t src_offset    = 0;
    size_t output_offset = 0;
    size_t outBuffSize   = 0;
    size_t blockoutLen   = 0;

    ENSURE_OR_GO_EXIT(srcData != NULL);
    ENSURE_OR_GO_EXIT(destData != NULL);
    ENSURE_OR_GO_EXIT(destLen != NULL);
    ENSURE_OR_GO_EXIT(srcLen > 0);

    outBuffSize   = *destLen;

    /* Check overflow */
    ENSURE_OR_GO_EXIT((context->cache_data_len + srcLen) >= context->cache_data_len);

    if ((context->cache_data_len + srcLen) < AEAD_BLOCK_SIZE) {
        /* Insufficinet data to process . Cache the data */
        memcpy((context->cache_data + context->cache_data_len), srcData, srcLen);
        context->cache_data_len = context->cache_data_len + srcLen;
        *destLen                = 0;
        return kStatus_SSS_Success;
    }
    else {
        if (context->cache_data_len > 0) {
            if (CIPHER_BLOCK_SIZE - context->cache_data_len > 0) {
                memcpy((context->cache_data + context->cache_data_len),
                    srcData,
                    (CIPHER_BLOCK_SIZE - context->cache_data_len));
            }

            blockoutLen = outBuffSize;
            status      = Se05x_API_AeadUpdate(&context->session->s_ctx,
                context->cryptoObjectId,
                context->cache_data,
                CIPHER_BLOCK_SIZE,
                destData,
                &blockoutLen);
            ENSURE_OR_GO_EXIT(status == SM_OK);

            src_offset = CIPHER_BLOCK_SIZE - context->cache_data_len;
            outBuffSize -= blockoutLen;
            output_offset += blockoutLen;

            context->cache_data_len = 0;
        }

        while (srcLen - src_offset >= CIPHER_BLOCK_SIZE) {
            size_t rem_srcData = srcLen - src_offset;

            if (rem_srcData > AEAD_UPDATE_MAX_DATA) {
                inputData_len = AEAD_UPDATE_MAX_DATA;
            }
            else {
                inputData_len = rem_srcData - (rem_srcData % CIPHER_BLOCK_SIZE);
            }

            blockoutLen = outBuffSize;
            status      = Se05x_API_AeadUpdate(&context->session->s_ctx,
                context->cryptoObjectId,
                (srcData + src_offset),
                inputData_len,
                (destData + output_offset),
                &blockoutLen);
            ENSURE_OR_GO_EXIT(status == SM_OK);

            src_offset += inputData_len;
            outBuffSize -= blockoutLen;
            output_offset += blockoutLen;
        }

        *destLen = output_offset;

        /* Copy unprocessed data to cache */
        if ((srcLen - src_offset) > 0) {
            memcpy(context->cache_data, (srcData + src_offset), (srcLen - src_offset));
            context->cache_data_len = (srcLen - src_offset);
        }
    }

    retval = kStatus_SSS_Success;
exit:
    if (retval == kStatus_SSS_Fail) {
        if (destLen) {
            *destLen = 0;
        }
    }
#endif
    return retval;
}

sss_status_t sss_se05x_aead_finish(sss_se05x_aead_t *context,
    const uint8_t *srcData,
    size_t srcLen,
    uint8_t *destData,
    size_t *destLen,
    uint8_t *tag,
    size_t *tagLen)
{
    sss_status_t retval = kStatus_SSS_Fail;
#if SSS_HAVE_SE05X_VER_GTE_06_00
    smStatus_t status = SM_NOT_OK;

    SE05x_Cipher_Oper_t OperType =
        (context->mode == kMode_SSS_Encrypt) ? kSE05x_Cipher_Oper_Encrypt : kSE05x_Cipher_Oper_Decrypt;
    uint8_t srcdata_updated[2 * CIPHER_BLOCK_SIZE] = {
        0,
    };
    size_t srcdata_updated_len = 0;

    if (srcLen > CIPHER_BLOCK_SIZE) {
        LOG_E("srcLen cannot be grater than 16 bytes. Call update function ");
        *destLen = 0;
        goto exit;
    }

    if ((context->algorithm == kAlgorithm_SSS_AES_CCM) || (context->algorithm == kAlgorithm_SSS_AES_CCM_INT_IV)) {
        retval = sss_se05x_aead_CCMfinish(context, srcData, srcLen, destData, destLen, tag, tagLen);
    }
    else {
        if (context->cache_data_len != 0) {
            memcpy(srcdata_updated, context->cache_data, context->cache_data_len);
            srcdata_updated_len = context->cache_data_len;
        }
        if (srcLen != 0) {
            memcpy((srcdata_updated + srcdata_updated_len), srcData, srcLen);
            srcdata_updated_len += srcLen;
        }
        if (srcdata_updated_len > 0) {
            status = Se05x_API_AeadUpdate(&context->session->s_ctx,
                context->cryptoObjectId,
                srcdata_updated,
                srcdata_updated_len,
                destData,
                destLen);
            ENSURE_OR_GO_EXIT(status == SM_OK);
        }
        else {
            /* This condition will occur if all data including cache is alread processed */
            LOG_D("No Data in cache, All data are already processed");
            *destLen = 0;
        }
        status = Se05x_API_AeadFinal(&context->session->s_ctx, context->cryptoObjectId, tag, tagLen, OperType);
        ENSURE_OR_GO_EXIT(status == SM_OK);
        retval = kStatus_SSS_Success;
    }
exit:
#endif /* SSS_HAVE_SE05X_VER_GTE_06_00 */
    return retval;
}

#if SSS_HAVE_SE05X_VER_GTE_06_00
static sss_status_t sss_se05x_aead_CCMfinish(sss_se05x_aead_t *context,
    const uint8_t *srcData,
    size_t srcLen,
    uint8_t *destData,
    size_t *destLen,
    uint8_t *tag,
    size_t *tagLen)
{
    sss_status_t retval   = kStatus_SSS_Fail;
    smStatus_t status     = SM_NOT_OK;
    uint8_t dataprocessed = 1;
    SE05x_Cipher_Oper_t OperType =
        (context->mode == kMode_SSS_Encrypt) ? kSE05x_Cipher_Oper_Encrypt : kSE05x_Cipher_Oper_Decrypt;
    uint8_t srcdata_updated[2 * CIPHER_BLOCK_SIZE] = {
        0,
    };
    size_t srcdata_updated_len = 0;
    size_t outLen              = 0;
    size_t tempoutLen          = 0;
    size_t destBufLen          = *destLen;

    if (context->cache_data_len != 0) {
        memcpy(srcdata_updated, context->cache_data, context->cache_data_len);
        srcdata_updated_len = context->cache_data_len;
    }
    if (srcLen != 0) {
        memcpy((srcdata_updated + srcdata_updated_len), srcData, srcLen);
        srcdata_updated_len += srcLen;
    }
    if (srcdata_updated_len > 0) {
        if (srcdata_updated_len < CIPHER_BLOCK_SIZE) {
            status = Se05x_API_AeadCCMLastUpdate(
                &context->session->s_ctx, context->cryptoObjectId, srcdata_updated, srcdata_updated_len);
            ENSURE_OR_GO_EXIT(status == SM_OK);
            dataprocessed = 0;
        }
        else if (srcdata_updated_len >= CIPHER_BLOCK_SIZE) {
            tempoutLen = destBufLen - outLen;
            status     = Se05x_API_AeadUpdate(&context->session->s_ctx,
                context->cryptoObjectId,
                srcdata_updated,
                CIPHER_BLOCK_SIZE,
                destData,
                &tempoutLen);
            ENSURE_OR_GO_EXIT(status == SM_OK);
            srcdata_updated_len = srcdata_updated_len - CIPHER_BLOCK_SIZE;
            outLen              = outLen + tempoutLen;

            /* Put the remaining data in CCMLastUpdate if present (will always be less than CIPHER_BLOCK_SIZE) */
            if (srcdata_updated_len) {
                status = Se05x_API_AeadCCMLastUpdate(&context->session->s_ctx,
                    context->cryptoObjectId,
                    srcdata_updated + CIPHER_BLOCK_SIZE,
                    srcdata_updated_len);
                ENSURE_OR_GO_EXIT(status == SM_OK);
                dataprocessed = 0;
            }
        }
    }
    else {
        /* This condition will occur if all data including
        cache is already processed just send final*/
        dataprocessed = 1;
    }

    if (dataprocessed == 0) {
        /*All data is updated, lastupdate datalen < 16 o/p
          is expected here */
        tempoutLen = destBufLen - outLen;
        status     = Se05x_API_AeadCCMFinal(
            &context->session->s_ctx, context->cryptoObjectId, (destData + outLen), &tempoutLen, tag, tagLen, OperType);
        outLen = outLen + tempoutLen;
    }
    else {
        /*All data is processed no destination data*/
        status = Se05x_API_AeadFinal(&context->session->s_ctx, context->cryptoObjectId, tag, tagLen, OperType);
    }
    ENSURE_OR_GO_EXIT(status == SM_OK);
    retval   = kStatus_SSS_Success;
    *destLen = outLen;
exit:
    return retval;
}
#endif /* SSS_HAVE_SE05X_VER_GTE_06_00 */

void sss_se05x_aead_context_free(sss_se05x_aead_t *context)
{
#if SSS_HAVE_SE05X_VER_GTE_06_00
#if SSSFTR_SE05X_CREATE_DELETE_CRYPTOOBJ
    smStatus_t status;
    uint8_t list[1024] = {
        0,
    };
    uint8_t object_exists = 0;
    size_t listlen        = sizeof(list);

    if (context->cryptoObjectId != 0) {
        status = Se05x_API_ReadCryptoObjectList(&context->session->s_ctx, list, &listlen);
        for (size_t i = 0; i < listlen; i += 4) {
            uint16_t cryptoObjectId = list[i + 1] | (list[i + 0] << 8);
            if (cryptoObjectId == context->cryptoObjectId) {
                object_exists = 1;
            }
        }
        if (object_exists) {
            status = Se05x_API_DeleteCryptoObject(&context->session->s_ctx, context->cryptoObjectId);
            if (status != SM_OK) {
                LOG_D("Could not delete crypto object 0x04X", context->cryptoObjectId);
                return;
            }
        }
    }
#endif /* SSSFTR_SE05X_CREATE_DELETE_CRYPTOOBJ */
    memset(context, 0, sizeof(*context));
#endif /* SSS_HAVE_SE05X_VER_GTE_06_00 */
}

/* End: se05x_aead */

/* ************************************************************************** */
/* Functions : sss_se05x_mac                                                  */
/* ************************************************************************** */

sss_status_t sss_se05x_mac_context_init(sss_se05x_mac_t *context,
    sss_se05x_session_t *session,
    sss_se05x_object_t *keyObject,
    sss_algorithm_t algorithm,
    sss_mode_t mode)
{
    sss_status_t retval = kStatus_SSS_Success;
    context->session    = session;
    context->keyObject  = keyObject;
    context->algorithm  = algorithm;
    context->mode       = mode;
    return retval;
}

sss_status_t sss_se05x_mac_one_go(
    sss_se05x_mac_t *context, const uint8_t *message, size_t messageLen, uint8_t *mac, size_t *macLen)
{
    sss_status_t retval          = kStatus_SSS_Fail;
    smStatus_t status            = SM_NOT_OK;
    SE05x_MACAlgo_t macOperation = se05x_get_mac_algo(context->algorithm);

    ENSURE_OR_GO_EXIT(message != NULL);
    ENSURE_OR_GO_EXIT(mac != NULL);
    ENSURE_OR_GO_EXIT(macLen != NULL);

    ENSURE_OR_GO_EXIT(macOperation != kSE05x_MACAlgo_NA);

    if (context->mode == kMode_SSS_Mac) {
        status = Se05x_API_MACOneShot_G(
            &context->session->s_ctx, context->keyObject->keyId, macOperation, message, messageLen, mac, macLen);
        ENSURE_OR_GO_EXIT(status == SM_OK);
    }
    else if (context->mode == kMode_SSS_Mac_Validate) {
        SE05x_Result_t result = kSE05x_Result_FAILURE;
        size_t result_size    = sizeof(result);

        status = Se05x_API_MACOneShot_V(&context->session->s_ctx,
            context->keyObject->keyId,
            macOperation,
            message,
            messageLen,
            mac,
            *macLen,
            (uint8_t *)&result,
            &result_size);
        ENSURE_OR_GO_EXIT(status == SM_OK);

        ENSURE_OR_GO_EXIT(result == kSE05x_Result_SUCCESS);
    }
    else {
        LOG_E("Unknown mode");
        goto exit;
    }

    retval = kStatus_SSS_Success;
exit:
    return retval;
}

sss_status_t sss_se05x_mac_validate_one_go(
    sss_se05x_mac_t *context, const uint8_t *message, size_t messageLen, uint8_t *mac, size_t macLen)
{
    sss_status_t retval = kStatus_SSS_Fail;
    smStatus_t status   = SM_NOT_OK;
    SE05x_MACAlgo_t macOperation;
    SE05x_Result_t result = kSE05x_Result_FAILURE;
    size_t result_size    = sizeof(result);

    if (context == NULL) {
        goto exit;
    }

    macOperation = se05x_get_mac_algo(context->algorithm);

    ENSURE_OR_GO_EXIT(macOperation != kSE05x_MACAlgo_NA);

    status = Se05x_API_MACOneShot_V(&context->session->s_ctx,
        context->keyObject->keyId,
        macOperation,
        message,
        messageLen,
        mac,
        macLen,
        (uint8_t *)&result,
        &result_size);

    if (status == SM_OK) {
        if (result == kSE05x_Result_SUCCESS) {
            retval = kStatus_SSS_Success;
        }
    }

exit:
    return retval;
}

sss_status_t sss_se05x_mac_init(sss_se05x_mac_t *context)
{
    sss_status_t retval       = kStatus_SSS_Fail;
    smStatus_t status         = SM_NOT_OK;
    SE05x_Mac_Oper_t operType = kSE05x_Mac_Oper_NA;
#if SSSFTR_SE05X_CREATE_DELETE_CRYPTOOBJ
    SE05x_CryptoModeSubType_t subtype;

    uint8_t list[1024] = {
        0,
    };
    size_t listlen = sizeof(list);
    size_t i;
    uint8_t create_crypto_obj = 1;

    SE05x_CryptoContext_t cryptoContext;

    switch (context->algorithm) {
    case kAlgorithm_SSS_CMAC_AES:
        subtype.mac             = kSE05x_MACAlgo_CMAC_128;
        cryptoContext           = kSE05x_CryptoContext_SIGNATURE;
        context->cryptoObjectId = kSE05x_CryptoObject_CMAC_128;
        break;
#if SSS_HAVE_HASH_1
    case kAlgorithm_SSS_HMAC_SHA1:
        subtype.mac             = kSE05x_MACAlgo_HMAC_SHA1;
        cryptoContext           = kSE05x_CryptoContext_SIGNATURE;
        context->cryptoObjectId = kSE05x_CryptoObject_HMAC_SHA1;
        break;
#endif
    case kAlgorithm_SSS_HMAC_SHA256:
        subtype.mac             = kSE05x_MACAlgo_HMAC_SHA256;
        cryptoContext           = kSE05x_CryptoContext_SIGNATURE;
        context->cryptoObjectId = kSE05x_CryptoObject_HMAC_SHA256;
        break;
    case kAlgorithm_SSS_HMAC_SHA384:
        subtype.mac             = kSE05x_MACAlgo_HMAC_SHA384;
        cryptoContext           = kSE05x_CryptoContext_SIGNATURE;
        context->cryptoObjectId = kSE05x_CryptoObject_HMAC_SHA384;
        break;
#if SSS_HAVE_HASH_512
    case kAlgorithm_SSS_HMAC_SHA512:
        subtype.mac             = kSE05x_MACAlgo_HMAC_SHA512;
        cryptoContext           = kSE05x_CryptoContext_SIGNATURE;
        context->cryptoObjectId = kSE05x_CryptoObject_HMAC_SHA512;
        break;
#endif
    default:
        return kStatus_SSS_Fail;
    }

    status = Se05x_API_ReadCryptoObjectList(&context->session->s_ctx, list, &listlen);
    for (i = 0; i < listlen; i += 4) {
        uint16_t cryptoObjectId = list[i + 1] | (list[i + 0] << 8);
        if (cryptoObjectId == context->cryptoObjectId) {
            create_crypto_obj = 0;
        }
    }

    if (create_crypto_obj) {
        status =
            Se05x_API_CreateCryptoObject(&context->session->s_ctx, context->cryptoObjectId, cryptoContext, subtype);
        if (status != SM_OK) {
            LOG_W("CreateCryptoObject Failed");
            return kStatus_SSS_Fail;
        }
    }
#endif

    if (context->mode == kMode_SSS_Mac) {
        operType = kSE05x_Mac_Oper_Generate;
    }
    else if (context->mode == kMode_SSS_Mac_Validate) {
        operType = kSE05x_Mac_Oper_Validate;
    }
    else {
        LOG_E("Unknown mode");
        goto exit;
    }

    status = Se05x_API_MACInit(&context->session->s_ctx, context->keyObject->keyId, context->cryptoObjectId, operType);
    ENSURE_OR_GO_EXIT(status == SM_OK);

    retval = kStatus_SSS_Success;
exit:
    return retval;
}

sss_status_t sss_se05x_mac_update(sss_se05x_mac_t *context, const uint8_t *message, size_t messageLen)
{
    sss_status_t retval = kStatus_SSS_Fail;
    smStatus_t status   = SM_NOT_OK;

    //SE05x_MACAlgo_t macOperation = se05x_get_mac_algo(context->algorithm);

    status = Se05x_API_MACUpdate(&context->session->s_ctx, message, messageLen, context->cryptoObjectId);
    ENSURE_OR_GO_EXIT(status == SM_OK);

    retval = kStatus_SSS_Success;
exit:
    return retval;
}

sss_status_t sss_se05x_mac_finish(sss_se05x_mac_t *context, uint8_t *mac, size_t *macLen)
{
    sss_status_t retval = kStatus_SSS_Fail;
    smStatus_t status   = SM_NOT_OK;

    //SE05x_MACAlgo_t macOperation = se05x_get_mac_algo(context->algorithm);

    if (context->mode == kMode_SSS_Mac) {
        status = Se05x_API_MACFinal(&context->session->s_ctx, NULL, 0, context->cryptoObjectId, NULL, 0, mac, macLen);
        ENSURE_OR_GO_EXIT(status == SM_OK);
    }
    else if (context->mode == kMode_SSS_Mac_Validate) {
        SE05x_Result_t result = kSE05x_Result_FAILURE;
        size_t result_size    = sizeof(result);

        status = Se05x_API_MACFinal(
            &context->session->s_ctx, NULL, 0, context->cryptoObjectId, mac, *macLen, (uint8_t *)&result, &result_size);
        ENSURE_OR_GO_EXIT(status == SM_OK);

        ENSURE_OR_GO_EXIT(result == kSE05x_Result_SUCCESS);
    }
    else {
        LOG_E("Unknown mode");
        goto exit;
    }

    retval = kStatus_SSS_Success;
exit:
    return retval;
}

void sss_se05x_mac_context_free(sss_se05x_mac_t *context)
{
    if (context->cryptoObjectId != 0) {
        smStatus_t status = Se05x_API_DeleteCryptoObject(&context->session->s_ctx, context->cryptoObjectId);
        if (status != SM_OK) {
            LOG_D("Could not delete crypto object 0x04X", context->cryptoObjectId);
            return;
        }
    }
    memset(context, 0, sizeof(*context));
}

/* End: se05x_mac */

/* ************************************************************************** */
/* Functions : sss_se05x_md                                                   */
/* ************************************************************************** */

sss_status_t sss_se05x_digest_context_init(
    sss_se05x_digest_t *context, sss_se05x_session_t *session, sss_algorithm_t algorithm, sss_mode_t mode)
{
    sss_status_t retval = kStatus_SSS_Success;
    context->session    = session;
    context->algorithm  = algorithm;
    context->mode       = mode;
    return retval;
}

sss_status_t sss_se05x_digest_one_go(
    sss_se05x_digest_t *context, const uint8_t *message, size_t messageLen, uint8_t *digest, size_t *digestLen)
{
    sss_status_t retval = kStatus_SSS_Fail;
    smStatus_t status   = SM_NOT_OK;
    uint8_t sha_type    = se05x_get_sha_algo(context->algorithm);

    ENSURE_OR_GO_EXIT(sha_type != kSE05x_DigestMode_NA);

    status = Se05x_API_SHAOneShot(&context->session->s_ctx, sha_type, message, messageLen, digest, digestLen);
    if (status != SM_OK) {
        *digestLen = 0;
        goto exit;
    }

    retval = kStatus_SSS_Success;
exit:
    return retval;
}

sss_status_t sss_se05x_digest_init(sss_se05x_digest_t *context)
{
    sss_status_t retval = kStatus_SSS_Fail;
    smStatus_t status   = SM_NOT_OK;
#if SSSFTR_SE05X_CREATE_DELETE_CRYPTOOBJ
    SE05x_CryptoModeSubType_t subtype;
    uint8_t list[1024] = {
        0,
    };
    size_t listlen = sizeof(list);
    size_t i;
    uint8_t create_crypto_obj = 1;

    switch (context->algorithm) {
#if SSS_HAVE_HASH_1
    case kAlgorithm_SSS_SHA1:
        subtype.digest          = kSE05x_DigestMode_SHA;
        context->cryptoObjectId = kSE05x_CryptoObject_DIGEST_SHA;
        break;
#endif
#if SSS_HAVE_HASH_224
    case kAlgorithm_SSS_SHA224:
        subtype.digest          = kSE05x_DigestMode_SHA224;
        context->cryptoObjectId = kSE05x_CryptoObject_DIGEST_SHA224;
        break;
#endif
    case kAlgorithm_SSS_SHA256:
        subtype.digest          = kSE05x_DigestMode_SHA256;
        context->cryptoObjectId = kSE05x_CryptoObject_DIGEST_SHA256;
        break;
    case kAlgorithm_SSS_SHA384:
        subtype.digest          = kSE05x_DigestMode_SHA384;
        context->cryptoObjectId = kSE05x_CryptoObject_DIGEST_SHA384;
        break;
#if SSS_HAVE_HASH_512
    case kAlgorithm_SSS_SHA512:
        subtype.digest          = kSE05x_DigestMode_SHA512;
        context->cryptoObjectId = kSE05x_CryptoObject_DIGEST_SHA512;
        break;
#endif
    default:
        return kStatus_SSS_Fail;
    }

    status = Se05x_API_ReadCryptoObjectList(&context->session->s_ctx, list, &listlen);
    for (i = 0; i < listlen; i += 4) {
        uint16_t cryptoObjectId = list[i + 1] | (list[i + 0] << 8);
        if (cryptoObjectId == context->cryptoObjectId) {
            create_crypto_obj = 0;
        }
    }

    if (create_crypto_obj) {
        status = Se05x_API_CreateCryptoObject(
            &context->session->s_ctx, context->cryptoObjectId, kSE05x_CryptoContext_DIGEST, subtype);
        if (status != SM_OK) {
            return kStatus_SSS_Fail;
        }
    }
#endif

    status = Se05x_API_DigestInit(&context->session->s_ctx, context->cryptoObjectId);
    ENSURE_OR_GO_EXIT(status == SM_OK);

    retval = kStatus_SSS_Success;
exit:
    return retval;
}

sss_status_t sss_se05x_digest_update(sss_se05x_digest_t *context, const uint8_t *message, size_t messageLen)
{
    sss_status_t retval = kStatus_SSS_Fail;
    smStatus_t status   = SM_NOT_OK;

    status = Se05x_API_DigestUpdate(&context->session->s_ctx, context->cryptoObjectId, message, messageLen);
    ENSURE_OR_GO_EXIT(status == SM_OK);

    retval = kStatus_SSS_Success;
exit:
    return retval;
}

sss_status_t sss_se05x_digest_finish(sss_se05x_digest_t *context, uint8_t *digest, size_t *digestLen)
{
    sss_status_t retval = kStatus_SSS_Fail;
    smStatus_t status   = SM_NOT_OK;

    status = Se05x_API_DigestFinal(&context->session->s_ctx, context->cryptoObjectId, NULL, 0, digest, digestLen);
    ENSURE_OR_GO_EXIT(status == SM_OK);

    retval = kStatus_SSS_Success;
exit:
    return retval;
}

void sss_se05x_digest_context_free(sss_se05x_digest_t *context)
{
    if (context->cryptoObjectId != 0) {
        smStatus_t status = Se05x_API_DeleteCryptoObject(&context->session->s_ctx, context->cryptoObjectId);
        if (status != SM_OK) {
            LOG_D("Could not delete crypto object 0x04X", context->cryptoObjectId);
            return;
        }
    }
    memset(context, 0, sizeof(*context));
}

/* End: se05x_md */

/* ************************************************************************** */
/* Functions : sss_se05x_rng                                                  */
/* ************************************************************************** */

sss_status_t sss_se05x_rng_context_init(sss_se05x_rng_context_t *context, sss_se05x_session_t *session)
{
    sss_status_t retval = kStatus_SSS_Success;
    context->session    = session;
    return retval;
}

sss_status_t sss_se05x_rng_get_random(sss_se05x_rng_context_t *context, uint8_t *random_data, size_t dataLen)
{
    sss_status_t retval = kStatus_SSS_Fail;
    smStatus_t status   = SM_NOT_OK;
    size_t chunk        = 0;
    size_t offset       = 0;

    while (dataLen > 0) {
        /* TODO - Replace 512 with max rsp buffer size based on with/without SCP */
        if (dataLen > 512) {
            chunk = 512;
        }
        else {
            chunk = dataLen;
        }

        status = Se05x_API_GetRandom(&context->session->s_ctx, (uint16_t)chunk, (random_data + offset), &chunk);
        ENSURE_OR_GO_EXIT(status == SM_OK);

        offset += chunk;
        dataLen -= chunk;
    }

    retval = kStatus_SSS_Success;
exit:
    return retval;
}

sss_status_t sss_se05x_rng_context_free(sss_se05x_rng_context_t *context)
{
    sss_status_t retval = kStatus_SSS_Success;
    memset(context, 0, sizeof(*context));
    return retval;
}
/* End: se05x_rng */

sss_status_t sss_se05x_tunnel_context_init(sss_se05x_tunnel_context_t *context, sss_se05x_session_t *session)
{
    context->se05x_session = session;
    sss_status_t retval    = kStatus_SSS_Success;
#if defined(USE_RTOS) && (USE_RTOS == 1)
    context->channelLock = xSemaphoreCreateMutex();
    if (context->channelLock == NULL) {
        LOG_E("xSemaphoreCreateMutex failed");
        return kStatus_SSS_Fail;
    }
#elif (__GNUC__ && !AX_EMBEDDED)
    if (pthread_mutex_init(&context->channelLock, NULL) != 0) {
        LOG_E("\n mutex init has failed");
        return kStatus_SSS_Fail;
    }
    else {
        LOG_D("Mutex Init successfull");
    }
#endif
    return retval;
}

sss_status_t sss_se05x_tunnel(sss_se05x_tunnel_context_t *context,
    uint8_t *data,
    size_t dataLen,
    sss_se05x_object_t *keyObjects,
    uint32_t keyObjectCount,
    uint32_t tunnelType)
{
    AX_UNUSED_ARG(context);
    AX_UNUSED_ARG(data);
    AX_UNUSED_ARG(dataLen);
    AX_UNUSED_ARG(keyObjects);
    AX_UNUSED_ARG(keyObjectCount);
    AX_UNUSED_ARG(tunnelType);
    sss_status_t retval = kStatus_SSS_Fail;
    return retval;
}

void sss_se05x_tunnel_context_free(sss_se05x_tunnel_context_t *context)
{
#if defined(USE_RTOS) && (USE_RTOS == 1)
    vSemaphoreDelete(context->channelLock);
#elif (__GNUC__ && !AX_EMBEDDED)
    pthread_mutex_destroy(&context->channelLock);
#endif
    memset(context, 0, sizeof(*context));
}

static smStatus_t sss_se05x_TXn(struct Se05xSession *pSession,
    const tlvHeader_t *hdr,
    uint8_t *cmdBuf,
    size_t cmdBufLen,
    uint8_t *rsp,
    size_t *rspLen,
    uint8_t hasle)
{
    smStatus_t ret     = SM_NOT_OK;
    tlvHeader_t outHdr = {
        0,
    };
    uint8_t txBuf[SE05X_MAX_BUF_SIZE_CMD] = {
        0,
    };
    size_t txBufLen = sizeof(txBuf);

    ret = pSession->fp_Transform(pSession, hdr, cmdBuf, cmdBufLen, &outHdr, txBuf, &txBufLen, hasle);
    ENSURE_OR_GO_EXIT(ret == SM_OK);
    ret = pSession->fp_RawTXn(
        pSession->conn_ctx, pSession->pChannelCtx, pSession->authType, &outHdr, txBuf, txBufLen, rsp, rspLen, hasle);

    ret = pSession->fp_DeCrypt(pSession, cmdBufLen, rsp, rspLen, hasle);

    ENSURE_OR_GO_EXIT(ret == SM_OK);
exit:
    return ret;
}

static smStatus_t sss_se05x_channel_txnRaw(void *conn_ctx,
    const tlvHeader_t *hdr,
    uint8_t *cmdBuf,
    size_t cmdBufLen,
    uint8_t *rsp,
    size_t *rspLen,
    uint8_t hasle)
{
    uint8_t txBuf[SE05X_MAX_BUF_SIZE_CMD] = {0};
    size_t i                              = 0;
    memcpy(&txBuf[i], hdr, sizeof(*hdr));
    smStatus_t ret = SM_NOT_OK;
    i += sizeof(*hdr);
    if (cmdBufLen > 0) {
        // The Lc field must be extended in case the length does not fit
        // into a single byte (Note, while the standard would allow to
        // encode 0x100 as 0x00 in the Lc field, nobody who is sane in his mind
        // would actually do that).
        if ((cmdBufLen < 0xFF) && !hasle) {
            txBuf[i++] = (uint8_t)cmdBufLen;
        }
        else {
            txBuf[i++] = 0x00;
            txBuf[i++] = 0xFFu & (cmdBufLen >> 8);
            txBuf[i++] = 0xFFu & (cmdBufLen);
        }
        memcpy(&txBuf[i], cmdBuf, cmdBufLen);
        i += cmdBufLen;
    }
    else {
        if (cmdBufLen == 0) {
            txBuf[i++] = 0x00;
        }
    }

    if (hasle) {
        txBuf[i++] = 0x00;
        txBuf[i++] = 0x00;
    }

    uint32_t U32rspLen = (uint32_t)*rspLen;
    ret                = (smStatus_t)smCom_TransceiveRaw(conn_ctx, txBuf, (U16)i, rsp, &U32rspLen);
    *rspLen            = U32rspLen;
    return ret;
}

static smStatus_t sss_se05x_channel_txn(void *conn_ctx,
    struct _sss_se05x_tunnel_context *pChannelCtx,
    SE_AuthType_t currAuth,
    const tlvHeader_t *hdr,
    uint8_t *cmdBuf,
    size_t cmdBufLen,
    uint8_t *rsp,
    size_t *rspLen,
    uint8_t hasle)
{
    smStatus_t retStatus = SM_NOT_OK;

    if ((pChannelCtx != NULL)) {
#if SSSFTR_SE05X_AuthECKey || SSSFTR_SE05X_AuthSession
        struct Se05xSession *se05xCtx = (struct Se05xSession *)&pChannelCtx->se05x_session->s_ctx;
        if (se05xCtx->authType == kSSS_AuthType_SCP03) {
#if USE_LOCK
            LOCK_TXN(pChannelCtx->channelLock);
#endif
            retStatus = se05xCtx->fp_TXn(se05xCtx, hdr, cmdBuf, cmdBufLen, rsp, rspLen, hasle);

#if USE_LOCK
            UNLOCK_TXN(pChannelCtx->channelLock);
#endif
            ENSURE_OR_GO_EXIT(retStatus == SM_OK);
        }
        else if (se05xCtx->authType == kSSS_AuthType_None) {
#if USE_LOCK
            LOCK_TXN(pChannelCtx->channelLock);
#endif
            retStatus = se05xCtx->fp_TXn(se05xCtx, hdr, cmdBuf, cmdBufLen, rsp, rspLen, hasle);

#if USE_LOCK
            UNLOCK_TXN(pChannelCtx->channelLock);
#endif
            ENSURE_OR_GO_EXIT(retStatus == SM_OK);
        }
        else {
            LOG_E("Invalid auth type");
            goto exit;
        }
#endif
    }
    else {
        if (currAuth == kSSS_AuthType_SCP03) {
            uint32_t u32rspLen = (uint32_t)*rspLen;
            retStatus = (smStatus_t)smCom_TransceiveRaw(conn_ctx, cmdBuf, (uint16_t)cmdBufLen, rsp, &u32rspLen);
            ENSURE_OR_GO_EXIT(retStatus == SM_OK);
            *rspLen = u32rspLen;
        }
        else {
            retStatus = sss_se05x_channel_txnRaw(conn_ctx, hdr, cmdBuf, cmdBufLen, rsp, rspLen, hasle);
            ENSURE_OR_GO_EXIT(retStatus == SM_OK);
        }
    }

exit:
    return retStatus;
}

/* End: se05x_tunnel */

#if SSSFTR_SE05X_ECC && SSSFTR_SE05X_KEY_SET
sss_status_t sss_se05x_key_store_create_curve(Se05xSession_t *pSession, uint32_t curve_id)
{
    sss_status_t retval = kStatus_SSS_Fail;
    smStatus_t status   = SM_NOT_OK;

    status = sss_se05x_create_curve_if_needed(pSession, curve_id);

    if (SM_OK == status) {
        retval = kStatus_SSS_Success;
    }

    return retval;
}
#endif

sss_status_t sss_se05x_set_feature(
    sss_se05x_session_t *session, SE05x_Applet_Feature_t feature, SE05x_Applet_Feature_Disable_t disable_features)
{
    sss_status_t retval                    = kStatus_SSS_Fail;
    smStatus_t status                      = SM_NOT_OK;
    Se05x_AppletFeatures_t applet_features = {kSE05x_AppletConfig_NA, NULL};
    applet_features.extended_features      = NULL;
#if SSS_HAVE_SE05X_VER_GTE_06_00
    SE05x_ExtendedFeatures_t extended = {0};
#endif

    if (session == NULL) {
        goto exit;
    }

#if SSS_HAVE_SE05X_VER_GTE_06_00

    /** Disable feature ECDH B2b8 */
    if (disable_features.EXTCFG_FORBID_ECDH == 1) {
        extended.features[1] |= 0x80; // 8th bit
    }
    /** Disable feature ECDAA B2b7 */
    if (disable_features.EXTCFG_FORBID_ECDAA == 1) {
        extended.features[1] |= 0x40; // 7th bit
    }
    /** Disable feature RSA_LT_2K B6b8 */
    if (disable_features.EXTCFG_FORBID_RSA_LT_2K == 1) {
        extended.features[5] |= 0x80; // 8th bit
    }
    /** Disable feature RSA_SHA1 B6b7 */
    if (disable_features.EXTCFG_FORBID_RSA_SHA1 == 1) {
        extended.features[5] |= 0x40; // 7th bit
    }
    /** Disable feature AES_GCM B8b8 */
    if (disable_features.EXTCFG_FORBID_AES_GCM == 1) {
        extended.features[7] |= 0x80; // 8th bit
    }
    /** Disable feature AES_GCM_EXT_IV B8b7 */
    if (disable_features.EXTCFG_FORBID_AES_GCM_EXT_IV == 1) {
        extended.features[7] |= 0x40; // 7th bit
    }
    /** Disable feature HKDF_EXTRACT B10b7 */
    if (disable_features.EXTCFG_FORBID_HKDF_EXTRACT == 1) {
        extended.features[9] |= 0x40; // 7th bit
    }

    applet_features.extended_features = &extended;
#endif

    if (feature.AppletConfig_ECDAA == 1) {
        applet_features.variant |= kSE05x_AppletConfig_ECDAA;
    }
    else if (feature.AppletConfig_ECDSA_ECDH_ECDHE == 1) {
        applet_features.variant |= kSE05x_AppletConfig_ECDSA_ECDH_ECDHE;
    }
    else if (feature.AppletConfig_EDDSA == 1) {
        applet_features.variant |= kSE05x_AppletConfig_EDDSA;
    }
    else if (feature.AppletConfig_DH_MONT == 1) {
        applet_features.variant |= kSE05x_AppletConfig_DH_MONT;
    }
    else if (feature.AppletConfig_HMAC == 1) {
        applet_features.variant |= kSE05x_AppletConfig_HMAC;
    }
    else if (feature.AppletConfig_RSA_PLAIN == 1) {
        applet_features.variant |= kSE05x_AppletConfig_RSA_PLAIN;
    }
    else if (feature.AppletConfig_RSA_CRT == 1) {
        applet_features.variant |= kSE05x_AppletConfig_RSA_CRT;
    }
    else if (feature.AppletConfig_AES == 1) {
        applet_features.variant |= kSE05x_AppletConfig_AES;
    }
    else if (feature.AppletConfig_DES == 1) {
        applet_features.variant |= kSE05x_AppletConfig_DES;
    }
    else if (feature.AppletConfig_PBKDF == 1) {
        applet_features.variant |= kSE05x_AppletConfig_PBKDF;
    }
    else if (feature.AppletConfig_TLS == 1) {
        applet_features.variant |= kSE05x_AppletConfig_TLS;
    }
    else if (feature.AppletConfig_MIFARE == 1) {
        applet_features.variant |= kSE05x_AppletConfig_MIFARE;
    }
    else if (feature.AppletConfig_I2CM == 1) {
        applet_features.variant |= kSE05x_AppletConfig_I2CM;
    }
    else {
        goto exit;
    }

    status = Se05x_API_SetAppletFeatures(&session->s_ctx, &applet_features);

    if (status == SM_OK) {
        retval = kStatus_SSS_Success;
    }

exit:
    return retval;
}

#if SSSFTR_SE05X_AuthSession
static smStatus_t se05x_CreateVerifyUserIDSession(
    pSe05xSession_t se05xSession, const uint32_t auth_id, SE05x_AuthCtx_ID_t *userId, pSe05xPolicy_t policy)
{
    sss_status_t retval   = kStatus_SSS_Fail;
    SE05x_Result_t exists = kSE05x_Result_FAILURE;
    smStatus_t status     = SM_NOT_OK;
    size_t sessionIdLen   = 8;
    uint8_t keyVal[60];
    size_t keyValLen = sizeof(keyVal);
    size_t keyBitLen = sizeof(keyVal) * 8;

    /* Check if Object exists only if auth_id is non kSE05x_AppletResID_TRANSPORT */
    /* CheckObjectExists returns 6985 SE05x if transport is Locked */
    if (auth_id == kSE05x_AppletResID_TRANSPORT) {
        status = SM_OK;
        LOG_D("Create Session with kSE05x_AppletResID_TRANSPORT");
    }
    else {
        status = Se05x_API_CheckObjectExists(se05xSession, auth_id, &exists);
        if (status == SM_OK && exists == kSE05x_Result_FAILURE) {
            status = SM_NOT_OK;
            LOG_E("UserID is not Provisioned!!!");
        }
    }
    if (status == SM_OK) {
        status = Se05x_API_CreateSession(se05xSession, auth_id, se05xSession->value, &sessionIdLen);
    }
    if (status == SM_OK) {
        status = SM_NOT_OK;
        retval = sss_host_key_store_get_key(userId->pObj->keyStore, userId->pObj, keyVal, &keyValLen, &keyBitLen);

        if (keyValLen < 4) {
            LOG_W("User ID cannot be less than 4 bytes");
            return SM_NOT_OK;
        }

        if (retval == kStatus_SSS_Success) {
            se05xSession->hasSession = 1;
            se05xSession->authType   = kSSS_AuthType_ID;
            se05xSession->auth_id    = auth_id;
            status                   = Se05x_API_VerifySessionUserID(se05xSession, keyVal, keyValLen);
            if (status == SM_OK) {
                if ((policy != NULL) && (policy->value != NULL) && (policy->value_len > 0)) {
                    status = SM_NOT_OK;
                    status = Se05x_API_ExchangeSessionData(se05xSession, policy);
                }
            }
        }
    }
    return status;
}
#endif

#if SSS_HAVE_SCP_SCP03_SSS
#if SSSFTR_SE05X_AuthSession
static smStatus_t se05x_CreateVerifyAESKeySession(
    pSe05xSession_t se05xSession, const uint32_t auth_id, NXSCP03_AuthCtx_t *pAppletSCPCtx)
{
    SE05x_Result_t exists = kSE05x_Result_FAILURE;
    smStatus_t status     = SM_NOT_OK;
    size_t sessionIdLen   = 8;
    sss_status_t retval   = kStatus_SSS_Fail;

    if (auth_id == kSE05x_AppletResID_TRANSPORT) {
        /* SKIP */
        /* If there's a transport lock, Se05x_API_CheckObjectExists would fail. */
        status = SM_OK;
    }
    else {
        status = Se05x_API_CheckObjectExists(se05xSession, auth_id, &exists);
        if (status == SM_OK && exists == kSE05x_Result_FAILURE) {
            status = SM_NOT_OK;
            LOG_E("Applet key is not Provisioned!!!");
        }
    }
    if (status == SM_OK) {
        status = Se05x_API_CreateSession(se05xSession, auth_id, se05xSession->value, &sessionIdLen);
        if (status != SM_OK) {
            se05xSession->hasSession = 0;
        }
        else {
            se05xSession->hasSession = 1;
            se05xSession->authType   = kSSS_AuthType_AESKey;
            se05xSession->auth_id    = auth_id;
            retval                   = nxScp03_AuthenticateChannel(se05xSession, pAppletSCPCtx);
            if (retval == kStatus_SSS_Success) {
                pAppletSCPCtx->pDyn_ctx->authType = kSSS_AuthType_AESKey;
                se05xSession->pdynScp03Ctx        = pAppletSCPCtx->pDyn_ctx;
                status                            = SM_OK;
            }
            else {
                status = SM_NOT_OK;
            }
        }
    }
    return status;
}
#endif

#if SSSFTR_SE05X_AuthECKey

static sss_status_t nxECKey_StoreAttestationPublicKey(
    pSe05xSession_t se05xSession, sss_key_store_t *pHostKeyStore, sss_object_t *pAttestKeyObject)
{
    sss_status_t status     = kStatus_SSS_Fail;
    smStatus_t sm_status    = SM_NOT_OK;
    uint8_t public_key[100] = {0};
    size_t keylen           = sizeof(public_key);

    uint8_t *key_buf  = NULL;
    size_t key_buflen = 0;

    /* Return the Key length including the ECC DER Header */
    add_ecc_header(public_key, &keylen, &key_buf, &key_buflen, kSE05x_ECCurve_NIST_P256);
    keylen = keylen - key_buflen;

    sm_status = Se05x_API_ReadObject(se05xSession, SSS_SE05X_RESID_ATTESTATION_KEY, 0, 0, key_buf, &keylen);
    if (sm_status != SM_OK) {
        status = kStatus_SSS_Fail;
        goto cleanup;
    }
    keylen = keylen + key_buflen;

    status = sss_key_object_init(pAttestKeyObject, pHostKeyStore);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

    status = sss_key_object_allocate_handle(
        pAttestKeyObject, __LINE__, kSSS_KeyPart_Public, kSSS_CipherType_EC_NIST_P, keylen, kKeyObject_Mode_Transient);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

    status = sss_key_store_set_key(pHostKeyStore, pAttestKeyObject, public_key, keylen, 256, NULL, 0);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

cleanup:
    return status;
}

static sss_status_t nxECKey_VerifyAttestation(sss_session_t *pHostSession,
    sss_object_t *pAttestKeyObject,
    uint8_t *pCapdu,
    size_t cApduLen,
    uint8_t *pRsp,
    size_t rspLen,
    uint8_t *pSignature,
    size_t signatureLen)
{
    sss_status_t status              = kStatus_SSS_Fail;
    sss_digest_t digest_ctx          = {0};
    sss_asymmetric_t asymm_ctx       = {0};
    sss_algorithm_t digest_algorithm = kAlgorithm_SSS_SHA256;
    uint8_t digest[32]               = {0};
    size_t digestLen                 = sizeof(digest);
    uint8_t inputData[250]           = {0};
    size_t inputDataLen              = 0;

    status = sss_digest_context_init(&digest_ctx, pHostSession, digest_algorithm, kMode_SSS_Digest);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);
#if SSS_HAVE_SE05X_VER_GTE_07_02
    status = sss_digest_one_go(&digest_ctx, pCapdu, cApduLen, digest, &digestLen);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

    ENSURE_OR_GO_CLEANUP(digestLen <= sizeof(inputData));
    memcpy(inputData, digest, digestLen);
    inputDataLen += digestLen;
#endif // SSS_HAVE_SE05X_VER_GTE_07_02

    ENSURE_OR_GO_CLEANUP((rspLen + inputDataLen) <= sizeof(inputData));
    memcpy(inputData + inputDataLen, pRsp, rspLen);
    inputDataLen += rspLen;

    status = sss_digest_one_go(&digest_ctx, inputData, inputDataLen, digest, &digestLen);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

    status = sss_asymmetric_context_init(
        &asymm_ctx, pHostSession, pAttestKeyObject, kAlgorithm_SSS_SHA256, kMode_SSS_Verify);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

    status = sss_asymmetric_verify_digest(&asymm_ctx, digest, digestLen, pSignature, signatureLen);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

cleanup:
    sss_digest_context_free(&digest_ctx);
    sss_asymmetric_context_free(&asymm_ctx);
    return status;
}

sss_status_t nxECKey_ReadEckaPublicKey(pSe05xSession_t se05xSession,
    sss_key_store_t *pHostKeyStore,
    uint8_t *pSePubEcka,
    size_t *pSePubEckaLen,
    uint32_t eckaObjId)
{
    sss_status_t status = kStatus_SSS_Fail;
    smStatus_t sm_status;
    sss_object_t hostAttestObj         = {0};
    sss_rng_context_t rng_ctx          = {0};
    SE05x_AttestationAlgo_t attestAlgo = kSE05x_AttestationAlgo_EC_SHA_256;
    sss_se05x_attst_data_t att_data    = {.valid_number = 2};
    uint8_t *key                       = pSePubEcka;
    uint8_t random[16]                 = {0};
    size_t randomLen                   = sizeof(random);
    uint8_t rspBuf[250]                = {0};
    uint8_t *pRspBuf                   = &rspBuf[0];
    size_t rspBufLen                   = 0;
#if SSS_HAVE_SE05X_VER_GTE_07_02
    uint8_t objectSize[] = {0x00, 0x20};
    size_t objectSizeLen = sizeof(objectSize);
    int tlvRet           = 0;
#endif // SSS_HAVE_SE05X_VER_GTE_07_02

    status = nxECKey_StoreAttestationPublicKey(se05xSession, pHostKeyStore, &hostAttestObj);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

    status = sss_rng_context_init(&rng_ctx, pHostKeyStore->session);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);
    status = sss_rng_get_random(&rng_ctx, random, randomLen);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);
    status = sss_rng_context_free(&rng_ctx);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

    att_data.data[0].timeStampLen = sizeof(SE05x_TimeStamp_t);
    att_data.data[0].chipIdLen    = sizeof(att_data.data[0].chipId);
    att_data.data[0].attributeLen = sizeof(att_data.data[0].attribute);
    att_data.data[0].signatureLen = sizeof(att_data.data[0].signature);

#if SSS_HAVE_SE05X_VER_GTE_07_02
    att_data.data[0].cmdLen = sizeof(att_data.data[0].cmd);
#else
    att_data.data[0].outrandomLen = sizeof(att_data.data[0].outrandom);
#endif // SSS_HAVE_SE05X_VER_GTE_07_02

    status = kStatus_SSS_Fail;
#if SSS_HAVE_SE05X_VER_GTE_07_02
    sm_status = Se05x_API_ReadObject_W_Attst_V2(se05xSession,
        eckaObjId,
        0,
        0,
        SSS_SE05X_RESID_ATTESTATION_KEY,
        attestAlgo,
        random,
        randomLen,
        key,
        pSePubEckaLen,
        att_data.data[0].attribute,
        &att_data.data[0].attributeLen,
        &att_data.data[0].timeStamp,
        att_data.data[0].chipId,
        &att_data.data[0].chipIdLen,
        att_data.data[0].cmd,
        &att_data.data[0].cmdLen,
        objectSize,
        &objectSizeLen,
        att_data.data[0].signature,
        &att_data.data[0].signatureLen);
#else
    sm_status = Se05x_API_ReadObject_W_Attst(se05xSession,
        eckaObjId,
        0,
        0,
        SSS_SE05X_RESID_ATTESTATION_KEY,
        attestAlgo,
        random,
        randomLen,
        key,
        pSePubEckaLen,
        att_data.data[0].attribute,
        &att_data.data[0].attributeLen,
        &att_data.data[0].timeStamp,
        att_data.data[0].outrandom,
        &att_data.data[0].outrandomLen,
        att_data.data[0].chipId,
        &att_data.data[0].chipIdLen,
        att_data.data[0].signature,
        &att_data.data[0].signatureLen);
#endif // SSS_HAVE_SE05X_VER_GTE_07_02
    ENSURE_OR_GO_CLEANUP(sm_status == SM_OK);

#if SSS_HAVE_SE05X_VER_GTE_07_02
    ENSURE_OR_GO_CLEANUP(
        ((1 + *pSePubEckaLen + 3) + (1 + att_data.data[0].chipIdLen + 3) + (1 + att_data.data[0].attributeLen + 3) +
            (1 + sizeof(objectSize) + 3) + (1 + sizeof(att_data.data[0].timeStamp) + 3)) <= sizeof(rspBuf));
    tlvRet = add_taglength_to_data(&pRspBuf, &rspBufLen, kSE05x_TAG_1, key, *pSePubEckaLen, true);
    ENSURE_OR_GO_CLEANUP(tlvRet == 0);
    tlvRet = add_taglength_to_data(
        &pRspBuf, &rspBufLen, kSE05x_TAG_2, att_data.data[0].chipId, att_data.data[0].chipIdLen, true);
    ENSURE_OR_GO_CLEANUP(tlvRet == 0);
    tlvRet = add_taglength_to_data(
        &pRspBuf, &rspBufLen, kSE05x_TAG_3, att_data.data[0].attribute, att_data.data[0].attributeLen, true);
    ENSURE_OR_GO_CLEANUP(tlvRet == 0);
    tlvRet = add_taglength_to_data(&pRspBuf, &rspBufLen, kSE05x_TAG_4, objectSize, sizeof(objectSize), true);
    ENSURE_OR_GO_CLEANUP(tlvRet == 0);
    tlvRet = add_taglength_to_data(&pRspBuf,
        &rspBufLen,
        kSE05x_TAG_TIMESTAMP,
        att_data.data[0].timeStamp.ts,
        sizeof(att_data.data[0].timeStamp),
        true);
    ENSURE_OR_GO_CLEANUP(tlvRet == 0);
#else
    ENSURE_OR_GO_CLEANUP((*pSePubEckaLen + att_data.data[0].attributeLen + att_data.data[0].timeStampLen +
                             att_data.data[0].outrandomLen + att_data.data[0].chipIdLen) <= sizeof(rspBuf))
    memcpy(pRspBuf, key, *pSePubEckaLen);
    pRspBuf += *pSePubEckaLen;
    memcpy(pRspBuf, att_data.data[0].attribute, att_data.data[0].attributeLen);
    pRspBuf += att_data.data[0].attributeLen;
    memcpy(pRspBuf, att_data.data[0].timeStamp.ts, att_data.data[0].timeStampLen);
    pRspBuf += att_data.data[0].timeStampLen;
    memcpy(pRspBuf, att_data.data[0].outrandom, att_data.data[0].outrandomLen);
    pRspBuf += att_data.data[0].outrandomLen;
    memcpy(pRspBuf, att_data.data[0].chipId, att_data.data[0].chipIdLen);
    pRspBuf += att_data.data[0].chipIdLen;

    rspBufLen = pRspBuf - (&rspBuf[0]);
#endif // SSS_HAVE_SE05X_VER_GTE_07_02

#if SSS_HAVE_SE05X_VER_GTE_07_02
    status = nxECKey_VerifyAttestation(pHostKeyStore->session,
        &hostAttestObj,
        att_data.data[0].cmd,
        att_data.data[0].cmdLen,
        rspBuf,
        rspBufLen,
        att_data.data[0].signature,
        att_data.data[0].signatureLen);
#else
    status = nxECKey_VerifyAttestation(pHostKeyStore->session,
        &hostAttestObj,
        NULL,
        0,
        rspBuf,
        rspBufLen,
        att_data.data[0].signature,
        att_data.data[0].signatureLen);
#endif // SSS_HAVE_SE05X_VER_GTE_07_02
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

    sss_key_store_erase_key(pHostKeyStore, &hostAttestObj);
    sss_key_object_free(&hostAttestObj);

cleanup:
    return status;
}

static smStatus_t se05x_CreateECKeySession(
    pSe05xSession_t se05xSession, const uint32_t auth_id, SE05x_AuthCtx_ECKey_t *pFScpCtx)
{
    sss_status_t retval   = kStatus_SSS_Fail;
    SE05x_Result_t exists = kSE05x_Result_FAILURE;
    smStatus_t status     = SM_NOT_OK;
    size_t sessionIdLen   = 8;

    status = Se05x_API_CheckObjectExists(se05xSession, auth_id, &exists);
    if (status == SM_OK && exists == kSE05x_Result_FAILURE) {
        status = SM_NOT_OK;
        LOG_E("SE ECDSA Public Key is not Provisioned!!!");
    }
    if (status == SM_OK) {
        uint8_t sePubEcka[150] = {0};
        size_t sePubEckaLen    = sizeof(sePubEcka);
        retval                 = nxECKey_ReadEckaPublicKey(se05xSession,
            pFScpCtx->pStatic_ctx->HostEcKeypair.keyStore,
            sePubEcka,
            &sePubEckaLen,
            kSE05x_AppletResID_KP_ECKEY_USER);
        if (retval != kStatus_SSS_Success) {
            status = SM_NOT_OK;
            return status;
        }
        status = Se05x_API_CreateSession(se05xSession, auth_id, se05xSession->value, &sessionIdLen);
        if (status != SM_OK) {
            se05xSession->hasSession = 0;
        }
        else {
            status                   = SM_NOT_OK;
            se05xSession->hasSession = 1;
            retval                   = nxECKey_AuthenticateChannel(se05xSession, pFScpCtx, sePubEcka, &sePubEckaLen);
            if (retval == kStatus_SSS_Success) {
                NXSCP03_DynCtx_t *pDyn_ctx = pFScpCtx->pDyn_ctx;

                pDyn_ctx->authType = se05xSession->authType = kSSS_AuthType_ECKey;
                se05xSession->pdynScp03Ctx                  = pFScpCtx->pDyn_ctx;
                status                                      = SM_OK;
                se05xSession->auth_id                       = auth_id;
            }
        }
    }
    return status;
}
#endif /* SSSFTR_SE05X_AuthECKey */
#endif

#if SSSFTR_SE05X_ECC || SSSFTR_SE05X_RSA
static sss_status_t se05x_check_input_len(size_t inLen, sss_algorithm_t algorithm)
{
    sss_status_t retval = kStatus_SSS_Fail;

    switch (algorithm) {
    case kAlgorithm_SSS_SHA1:
    case kAlgorithm_SSS_ECDSA_SHA1:
#if SSS_HAVE_RSA
    case kAlgorithm_SSS_RSASSA_PKCS1_V1_5_SHA1:
    case kAlgorithm_SSS_RSASSA_PKCS1_PSS_MGF1_SHA1:
#endif
        retval = (inLen == 20) ? kStatus_SSS_Success : kStatus_SSS_Fail;
        break;
    case kAlgorithm_SSS_SHA224:
    case kAlgorithm_SSS_ECDSA_SHA224:
#if SSS_HAVE_RSA
    case kAlgorithm_SSS_RSASSA_PKCS1_V1_5_SHA224:
    case kAlgorithm_SSS_RSASSA_PKCS1_PSS_MGF1_SHA224:
#endif
        retval = (inLen == 28) ? kStatus_SSS_Success : kStatus_SSS_Fail;
        break;
    case kAlgorithm_SSS_SHA256:
#if SSS_HAVE_ECDAA
    case kAlgorithm_SSS_ECDAA:
#endif
    case kAlgorithm_SSS_ECDSA_SHA256:
#if SSS_HAVE_RSA
    case kAlgorithm_SSS_RSASSA_PKCS1_V1_5_SHA256:
    case kAlgorithm_SSS_RSASSA_PKCS1_PSS_MGF1_SHA256:
#endif
        retval = (inLen == 32) ? kStatus_SSS_Success : kStatus_SSS_Fail;
        break;
    case kAlgorithm_SSS_SHA384:
    case kAlgorithm_SSS_ECDSA_SHA384:
#if SSS_HAVE_RSA
    case kAlgorithm_SSS_RSASSA_PKCS1_V1_5_SHA384:
    case kAlgorithm_SSS_RSASSA_PKCS1_PSS_MGF1_SHA384:
#endif
        retval = (inLen == 48) ? kStatus_SSS_Success : kStatus_SSS_Fail;
        break;
    case kAlgorithm_SSS_SHA512:
    case kAlgorithm_SSS_ECDSA_SHA512:
#if SSS_HAVE_RSA
    case kAlgorithm_SSS_RSASSA_PKCS1_V1_5_SHA512:
    case kAlgorithm_SSS_RSASSA_PKCS1_PSS_MGF1_SHA512:
#endif
        retval = (inLen == 64) ? kStatus_SSS_Success : kStatus_SSS_Fail;
        break;
#if SSS_HAVE_RSA
    case kAlgorithm_SSS_RSASSA_PKCS1_V1_5_NO_HASH:
    case kAlgorithm_SSS_RSASSA_NO_PADDING:
        retval = kStatus_SSS_Success;
        break;
#endif
    default:
        LOG_E("Unkown algorithm");
        retval = kStatus_SSS_Fail;
    }
    return retval;
}
#endif

static SE05x_ECSignatureAlgo_t se05x_get_ec_sign_hash_mode(sss_algorithm_t algorithm)
{
    SE05x_ECSignatureAlgo_t mode;
    switch (algorithm) {
    case kAlgorithm_SSS_SHA1:
    case kAlgorithm_SSS_ECDSA_SHA1:
        mode = kSE05x_ECSignatureAlgo_SHA;
        break;
    case kAlgorithm_SSS_SHA224:
    case kAlgorithm_SSS_ECDSA_SHA224:
        mode = kSE05x_ECSignatureAlgo_SHA_224;
        break;
    case kAlgorithm_SSS_SHA256:
    case kAlgorithm_SSS_ECDSA_SHA256:
        mode = kSE05x_ECSignatureAlgo_SHA_256;
        break;
    case kAlgorithm_SSS_SHA384:
    case kAlgorithm_SSS_ECDSA_SHA384:
        mode = kSE05x_ECSignatureAlgo_SHA_384;
        break;
    case kAlgorithm_SSS_SHA512:
    case kAlgorithm_SSS_ECDSA_SHA512:
        mode = kSE05x_ECSignatureAlgo_SHA_512;
        break;
    default:
        mode = kSE05x_ECSignatureAlgo_PLAIN;
        break;
    }
    return mode;
}

#if SSSFTR_SE05X_RSA && SSS_HAVE_RSA
static SE05x_RSAEncryptionAlgo_t se05x_get_rsa_encrypt_mode(sss_algorithm_t algorithm)
{
    SE05x_RSAEncryptionAlgo_t mode;
    switch (algorithm) {
    case kAlgorithm_SSS_RSAES_PKCS1_OAEP_SHA1:
        mode = kSE05x_RSAEncryptionAlgo_PKCS1_OAEP;
        break;
    case kAlgorithm_SSS_RSAES_PKCS1_OAEP_SHA224:
    case kAlgorithm_SSS_RSAES_PKCS1_OAEP_SHA256:
    case kAlgorithm_SSS_RSAES_PKCS1_OAEP_SHA384:
    case kAlgorithm_SSS_RSAES_PKCS1_OAEP_SHA512:
        mode = kSE05x_RSAEncryptionAlgo_NA;
        break;
    case kAlgorithm_SSS_RSAES_PKCS1_V1_5:
        mode = kSE05x_RSAEncryptionAlgo_PKCS1;
        break;
    default:
        mode = kSE05x_RSAEncryptionAlgo_NO_PAD;
        break;
    }
    return mode;
}

static SE05x_RSASignatureAlgo_t se05x_get_rsa_sign_hash_mode(sss_algorithm_t algorithm)
{
    SE05x_RSASignatureAlgo_t mode;
    switch (algorithm) {
    case kAlgorithm_SSS_RSASSA_PKCS1_V1_5_SHA1:
        mode = kSE05x_RSASignatureAlgo_SHA1_PKCS1;
        break;
    case kAlgorithm_SSS_RSASSA_PKCS1_V1_5_SHA224:
        mode = kSE05x_RSASignatureAlgo_SHA_224_PKCS1;
        break;
    case kAlgorithm_SSS_RSASSA_PKCS1_V1_5_SHA256:
        mode = kSE05x_RSASignatureAlgo_SHA_256_PKCS1;
        break;
    case kAlgorithm_SSS_RSASSA_PKCS1_V1_5_SHA384:
        mode = kSE05x_RSASignatureAlgo_SHA_384_PKCS1;
        break;
    case kAlgorithm_SSS_RSASSA_PKCS1_V1_5_SHA512:
        mode = kSE05x_RSASignatureAlgo_SHA_512_PKCS1;
        break;
    case kAlgorithm_SSS_RSASSA_PKCS1_PSS_MGF1_SHA1:
        mode = kSE05x_RSASignatureAlgo_SHA1_PKCS1_PSS;
        break;
    case kAlgorithm_SSS_RSASSA_PKCS1_PSS_MGF1_SHA224:
        mode = kSE05x_RSASignatureAlgo_SHA224_PKCS1_PSS;
        break;
    case kAlgorithm_SSS_RSASSA_PKCS1_PSS_MGF1_SHA256:
        mode = kSE05x_RSASignatureAlgo_SHA256_PKCS1_PSS;
        break;
    case kAlgorithm_SSS_RSASSA_PKCS1_PSS_MGF1_SHA384:
        mode = kSE05x_RSASignatureAlgo_SHA384_PKCS1_PSS;
        break;
    case kAlgorithm_SSS_RSASSA_PKCS1_PSS_MGF1_SHA512:
        mode = kSE05x_RSASignatureAlgo_SHA512_PKCS1_PSS;
        break;
    default:
        mode = kSE05x_RSASignatureAlgo_NA;
    }
    return mode;
}
#endif // SSSFTR_SE05X_RSA

static SE05x_CipherMode_t se05x_get_cipher_mode(sss_algorithm_t algorithm)
{
    SE05x_CipherMode_t mode;
    switch (algorithm) {
    case kAlgorithm_SSS_AES_ECB:
        mode = kSE05x_CipherMode_AES_ECB_NOPAD;
        break;
    case kAlgorithm_SSS_DES_ECB:
    case kAlgorithm_SSS_DES3_ECB:
        mode = kSE05x_CipherMode_DES_ECB_NOPAD;
        break;
    case kAlgorithm_SSS_AES_CBC:
        mode = kSE05x_CipherMode_AES_CBC_NOPAD;
        break;
    case kAlgorithm_SSS_DES_CBC:
    case kAlgorithm_SSS_DES3_CBC:
        mode = kSE05x_CipherMode_DES_CBC_NOPAD;
        break;
    case kAlgorithm_SSS_AES_CTR:
        mode = kSE05x_CipherMode_AES_CTR;
        break;
#if SSS_HAVE_SE05X_VER_GTE_07_02
    case kAlgorithm_SSS_AES_CTR_INT_IV:
        mode = kSE05x_CipherMode_AES_CTR_INT_IV;
        break;
#endif
    case kAlgorithm_SSS_DES_CBC_ISO9797_M1:
    case kAlgorithm_SSS_DES3_CBC_ISO9797_M1:
        mode = kSE05x_CipherMode_DES_CBC_ISO9797_M1;
        break;
    case kAlgorithm_SSS_DES_CBC_ISO9797_M2:
    case kAlgorithm_SSS_DES3_CBC_ISO9797_M2:
        mode = kSE05x_CipherMode_DES_CBC_ISO9797_M2;
        break;
    default:
        mode = kSE05x_CipherMode_NA;
    }
    return mode;
}

SE05x_MACAlgo_t se05x_get_mac_algo(sss_algorithm_t algorithm)
{
    SE05x_MACAlgo_t mode;
    switch (algorithm) {
    case kAlgorithm_SSS_CMAC_AES:
        mode = kSE05x_MACAlgo_CMAC_128;
        break;
#if SSS_HAVE_HASH_1
    case kAlgorithm_SSS_HMAC_SHA1:
        mode = kSE05x_MACAlgo_HMAC_SHA1;
        break;
#endif
    case kAlgorithm_SSS_HMAC_SHA256:
        mode = kSE05x_MACAlgo_HMAC_SHA256;
        break;
    case kAlgorithm_SSS_HMAC_SHA384:
        mode = kSE05x_MACAlgo_HMAC_SHA384;
        break;
#if SSS_HAVE_HASH_512
    case kAlgorithm_SSS_HMAC_SHA512:
        mode = kSE05x_MACAlgo_HMAC_SHA512;
        break;
#endif
    case kAlgorithm_SSS_DES_CMAC8:
        mode = kSE05x_MACAlgo_DES_CMAC8;
        break;
    default:
        mode = kSE05x_MACAlgo_NA;
    }
    return mode;
}

SE05x_DigestMode_t se05x_get_sha_algo(sss_algorithm_t algorithm)
{
    SE05x_DigestMode_t sha_type;

    switch (algorithm) {
#if SSS_HAVE_HASH_1
    case kAlgorithm_SSS_SHA1:
    case kAlgorithm_SSS_HMAC_SHA1:
        sha_type = kSE05x_DigestMode_SHA;
        break;
#endif
#if SSS_HAVE_HASH_224
    case kAlgorithm_SSS_SHA224:
        sha_type = kSE05x_DigestMode_SHA224;
        break;
#endif
    case kAlgorithm_SSS_SHA256:
    case kAlgorithm_SSS_HMAC_SHA256:
        sha_type = kSE05x_DigestMode_SHA256;
        break;
    case kAlgorithm_SSS_SHA384:
    case kAlgorithm_SSS_HMAC_SHA384:
        sha_type = kSE05x_DigestMode_SHA384;
        break;
#if SSS_HAVE_HASH_512
    case kAlgorithm_SSS_SHA512:
    case kAlgorithm_SSS_HMAC_SHA512:
        sha_type = kSE05x_DigestMode_SHA512;
        break;
#endif
    default:
        sha_type = kSE05x_DigestMode_NA;
    }

    return sha_type;
}
////////////////////////////////////////////////////////////////////////
#if SSSFTR_SE05X_ECC && SSSFTR_SE05X_KEY_SET
static smStatus_t sss_se05x_LL_set_ec_key(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    SE05x_MaxAttemps_t maxAttempt,
    uint32_t objectID,
    SE05x_ECCurve_t curveID,
    const uint8_t *privKey,
    size_t privKeyLen,
    const uint8_t *pubKey,
    size_t pubKeyLen,
    const SE05x_INS_t ins_type,
    const SE05x_KeyPart_t key_part,
    SE05x_Result_t obj_exists)
{
    smStatus_t status = SM_NOT_OK;
#if SSS_HAVE_SE05X_VER_GTE_06_00
    fp_Ec_KeyWrite_t fpEcKey_Ver = NULL;
    /* Call APIs For SE051 */
    if (obj_exists == kSE05x_Result_FAILURE) {
        fpEcKey_Ver = &Se05x_API_WriteECKey_Ver;
    }
    else if (obj_exists == kSE05x_Result_SUCCESS) {
        fpEcKey_Ver = &Se05x_API_UpdateECKey_Ver;
    }

    if (fpEcKey_Ver != NULL) {
        status = fpEcKey_Ver(session_ctx,
            policy,
            maxAttempt,
            objectID,
            curveID,
            privKey,
            privKeyLen,
            pubKey,
            pubKeyLen,
            ins_type,
            key_part,
            0);
    }
    else {
        LOG_E("Invalid Object exist status!!!");
    }

#else
    /* Call APIs For SE050 */
    status = Se05x_API_WriteECKey(
        session_ctx, policy, maxAttempt, objectID, curveID, privKey, privKeyLen, pubKey, pubKeyLen, ins_type, key_part);
#endif
    return status;
}
#endif //SSSFTR_SE05X_ECC

#if SSSFTR_SE05X_KEY_SET
static smStatus_t sss_se05x_LL_set_symm_key(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    SE05x_MaxAttemps_t maxAttempt,
    uint32_t objectID,
    SE05x_KeyID_t kekID,
    const uint8_t *keyValue,
    size_t keyValueLen,
    const SE05x_INS_t ins_type,
    const SE05x_SymmKeyType_t type,
    SE05x_Result_t obj_exists)
{
    smStatus_t status = SM_NOT_OK;
#if SSS_HAVE_SE05X_VER_GTE_06_00
    fp_Symm_KeyWrite_t fpSymmKey_Ver = NULL;
    /* Call APIs For SE051 */
    if (obj_exists == kSE05x_Result_FAILURE) {
        fpSymmKey_Ver = &Se05x_API_WriteSymmKey_Ver;
    }
    else if (obj_exists == kSE05x_Result_SUCCESS) {
        fpSymmKey_Ver = &Se05x_API_UpdateSymmKey_Ver;
    }

    if (fpSymmKey_Ver != NULL) {
        status = (*fpSymmKey_Ver)(
            session_ctx, policy, maxAttempt, objectID, kekID, keyValue, keyValueLen, ins_type, type, 0);
    }
    else {
        LOG_E("Invalid Object exist status!!!");
    }
#else
    /* Call APIs For SE050 */
    status =
        Se05x_API_WriteSymmKey(session_ctx, policy, maxAttempt, objectID, kekID, keyValue, keyValueLen, ins_type, type);
#endif
    return status;
}
#endif //SSSFTR_SE05X_AES && SSSFTR_SE05X_KEY_SET

#if SSSFTR_SE05X_RSA && SSSFTR_SE05X_KEY_SET && SSS_HAVE_RSA
static smStatus_t sss_se05x_LL_set_RSA_key(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    uint32_t objectID,
    uint16_t size,
    const uint8_t *p,
    size_t pLen,
    const uint8_t *q,
    size_t qLen,
    const uint8_t *dp,
    size_t dpLen,
    const uint8_t *dq,
    size_t dqLen,
    const uint8_t *qInv,
    size_t qInvLen,
    const uint8_t *pubExp,
    size_t pubExpLen,
    const uint8_t *priv,
    size_t privLen,
    const uint8_t *pubMod,
    size_t pubModLen,
    const SE05x_INS_t ins_type,
    const SE05x_KeyPart_t key_part,
    const SE05x_RSAKeyFormat_t rsa_format,
    SE05x_Result_t obj_exists)
{
    smStatus_t status = SM_NOT_OK;
#if SSS_HAVE_SE05X_VER_GTE_06_00
    fp_RSA_KeyWrite_t fpRSAKey_Ver = NULL;
    /* Call APIs For SE051 */
    if (obj_exists == kSE05x_Result_FAILURE) {
        fpRSAKey_Ver = &Se05x_API_WriteRSAKey_Ver;
    }
    else if (obj_exists == kSE05x_Result_SUCCESS) {
        fpRSAKey_Ver = &Se05x_API_UpdateRSAKey_Ver;
    }

    if (fpRSAKey_Ver != NULL) {
        status = (*fpRSAKey_Ver)(session_ctx,
            policy,
            objectID,
            size,
            p,
            pLen,
            q,
            qLen,
            dp,
            dpLen,
            dq,
            dqLen,
            qInv,
            qInvLen,
            pubExp,
            pubExpLen,
            priv,
            privLen,
            pubMod,
            pubModLen,
            ins_type,
            key_part,
            rsa_format,
            0);
    }
    else {
        LOG_E("Invalid Object exist status!!!");
    }
#else
    /* Call APIs For SE050 */
    status = Se05x_API_WriteRSAKey(session_ctx,
        policy,
        objectID,
        size,
        p,
        pLen,
        q,
        qLen,
        dp,
        dpLen,
        dq,
        dqLen,
        qInv,
        qInvLen,
        pubExp,
        pubExpLen,
        priv,
        privLen,
        pubMod,
        pubModLen,
        ins_type,
        key_part,
        rsa_format);
#endif
    return status;
}
#endif //SSSFTR_SE05X_RSA && SSSFTR_SE05X_KEY_SET

#if SSS_HAVE_TPM_BN && SSS_HAVE_ECDAA && SSS_HAVE_SE05X_VER_GTE_07_02
#if SSSFTR_SE05X_ECC
/* Reserved Random key id is 0x7DB0,0000 -> 0x7DB0,FFFF
*  0x7DB0,0000 is used for None auth session and platform scp03 session
*  Other id = (auth_id % 0x0000,FFFF) + 0x7DB0,0001
*/
static sss_status_t sss_se05x_get_ecdaa_random_key_id(Se05xSession_t *context, uint32_t *random_keyId)
{
    sss_status_t retval = kStatus_SSS_Fail;

    ENSURE_OR_GO_EXIT(context != NULL);
    ENSURE_OR_GO_EXIT(random_keyId != NULL);

    if (context->hasSession == 1) {
        *random_keyId = context->auth_id % (SSS_SE05X_RESID_ECDAA_RANDOM_KEY_NUMBER - 1) +
                        SSS_SE05X_RESID_ECDAA_RANDOM_KEY_START + 1;
    }
    else {
        *random_keyId = SSS_SE05X_RESID_ECDAA_RANDOM_KEY_START;
    }

    retval = kStatus_SSS_Success;
exit:
    return retval;
}

static sss_status_t sss_se05x_generate_ecdaa_random_key(Se05xSession_t *context, uint32_t *random_keyId)
{
    sss_status_t retval = kStatus_SSS_Fail;

    smStatus_t status       = SM_NOT_OK;
    sss_status_t sss_ret    = kStatus_SSS_Fail;
    SE05x_Result_t IdExists = kSE05x_Result_NA;
    uint8_t data[SE05X_MAX_BUF_SIZE_RSP];
    size_t dataLen = SE05X_MAX_BUF_SIZE_RSP;

    /* Policies for key */
    const sss_policy_u key_withPol = {.type = KPolicy_Asym_Key,
        .auth_obj_id                        = ECDAA_RANDOM_KEY_OBJ_AUTH_ID,
        .policy                             = {.asymmkey = {
                       .can_Sign              = 1,
                       .can_Verify            = 0,
                       .can_Encrypt           = 0,
                       .can_Decrypt           = 0,
                       .can_Import_Export     = 0,
                       .forbid_Derived_Output = 0,
                       .can_Gen               = 1,
                       .can_KA                = 1,
                       .can_Attest            = 0,
                   }}};

    /* Common rules */
    const sss_policy_u common = {.type = KPolicy_Common,
        .auth_obj_id                   = ECDAA_RANDOM_KEY_OBJ_AUTH_ID,
        .policy                        = {.common = {
                       .forbid_All  = 0,
                       .can_Read    = 1,
                       .can_Write   = 0,
                       .can_Delete  = 1,
                       .req_Sm      = 0,
                       .req_pcr_val = 0,
                   }}};

    /* create policy set */
    sss_policy_t policy_for_ec_key = {.nPolicies = 2, .policies = {&key_withPol, &common}};

    Se05xPolicy_t se05x_policy;
    uint8_t policies_buff[MAX_POLICY_BUFFER_SIZE];
    size_t valid_policy_buff_len = 0;

    ENSURE_OR_GO_EXIT(context != NULL);
    ENSURE_OR_GO_EXIT(random_keyId != NULL);

    // Construct policy for ECDAA Random key.
    sss_ret = sss_se05x_create_object_policy_buffer(&policy_for_ec_key, &policies_buff[0], &valid_policy_buff_len);
    ENSURE_OR_GO_EXIT(sss_ret == kStatus_SSS_Success);

    se05x_policy.value     = &policies_buff[0];
    se05x_policy.value_len = valid_policy_buff_len;

    // Get Random key id for this session.
    sss_ret = sss_se05x_get_ecdaa_random_key_id(context, random_keyId);
    ENSURE_OR_GO_EXIT(sss_ret == kStatus_SSS_Success);

    // Check if reserved random key already exist. If not, create it.
    IdExists = kSE05x_Result_NA;
    status   = Se05x_API_CheckObjectExists(context, *random_keyId, &IdExists);
    if (status != SM_OK) {
        LOG_W("Fail to check random key 0x%x exists. It may has already been occupied by other session", *random_keyId);
    }
    ENSURE_OR_GO_EXIT(status == SM_OK);

    if (IdExists == kSE05x_Result_FAILURE) {
        // Reserved random key does not exist. Generate ECC keypair.

        uint8_t curveList[kSE05x_ECCurve_Total_Weierstrass_Curves] = {
            0,
        };
        size_t curveListLen = sizeof(curveList);

        status = Se05x_API_ReadECCurveList(context, curveList, &curveListLen);
        ENSURE_OR_GO_EXIT(status == SM_OK);

        if (curveList[kSE05x_ECCurve_TPM_ECC_BN_P256 - 1] == kSE05x_SetIndicator_SET) {
            // Curve already created. Pass
        }
        else {
            status = Se05x_API_CreateCurve_tpm_bm_p256(context, kSE05x_ECCurve_TPM_ECC_BN_P256);

            ENSURE_OR_GO_EXIT(status != SM_NOT_OK);
            if (status == SM_ERR_CONDITIONS_NOT_SATISFIED) {
                LOG_W("Allowing SM_ERR_CONDITIONS_NOT_SATISFIED for CreateCurve");
            }
        }

        LOG_D("Create Key 0x%x as random keypair", *random_keyId);
        status = Se05x_API_WriteECKey(context,
            &se05x_policy,
            SE05x_MaxAttemps_NA,
            *random_keyId,
            kSE05x_ECCurve_TPM_ECC_BN_P256,
            NULL,
            0,
            NULL,
            0,
            kSE05x_INS_TRANSIENT,
            kSE05x_KeyPart_Private);
        ENSURE_OR_GO_EXIT(status == SM_OK);
    }
    else {
        status = Se05x_API_ReadObjectAttributes(context, *random_keyId, data, &dataLen);
        ENSURE_OR_GO_EXIT(status == SM_OK);
        ENSURE_OR_GO_EXIT(dataLen >= 19); //Object identifier+type+auth+AEAD+authid+output+Origin+Version
        ENSURE_OR_GO_EXIT(data[SSS_SE05X_OBJ_ATTR_TYPE_OFFSET] == kSE05x_SecObjTyp_EC_PRIV_KEY_BN_P256)

        // "r" is in uninitialized state after reselect Applet. Regenerate "r" then.
        if (data[dataLen - SSS_SE05X_OBJ_ATTR_ORIG_REV_OFFSET] == kSE05x_Origin_NA) {
            LOG_D("Key 0x%x already exist. Regenerate it", *random_keyId);

            status = Se05x_API_WriteECKey(context,
                NULL,
                SE05x_MaxAttemps_NA,
                *random_keyId,
                kSE05x_ECCurve_NA,
                NULL,
                0,
                NULL,
                0,
                kSE05x_INS_NA,
                kSE05x_KeyPart_NA);
            ENSURE_OR_GO_EXIT(status == SM_OK);
        }
    }

    retval = kStatus_SSS_Success;
exit:
    return retval;
}
#endif // SSSFTR_SE05X_ECC

#endif // #if SSS_HAVE_TPM_BN && SSS_HAVE_ECDAA && SSS_HAVE_SE05X_VER_GTE_07_02 && SSSFTR_SE05X_KEY_SET

#ifdef __cplusplus
}
#endif

#endif /* SSS_HAVE_APPLET_SE05X_IOT */
