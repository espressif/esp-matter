/*
 * Copyright 2022 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <se05x_tlv.h>
#include <se05x_const.h>
#include <se05x_enums.h>
#include "global_platf.h"
#include "sm_apdu.h"


#if defined(NONSECURE_WORLD)
#include "veneer_printf_table.h"
#endif

#if (SSS_HAVE_APPLET_SE051_H && SSS_HAVE_SE05X_VER_07_02)
/* OK */
#else
#error "Only with SE051_H based build"
#endif

#ifndef NEWLINE
#define NEWLINE must be already defined
#endif

#define SE05X_T4T_MAX_BUF_SIZE_RSP 270

#define T4T_APPLET_NAME                          \
    {                                            \
        0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01 \
    }


smStatus_t Se05x_T4T_API_SelectT4TApplet(pSe05xSession_t session_ctx)
{
    unsigned char appletName[] = T4T_APPLET_NAME;
    U8 selectResponseData[256] = {0};
    U16 selectResponseDataLen  = sizeof(selectResponseData);
    U16 ret = 0;

    ret = GP_Select(
        session_ctx->conn_ctx, (const U8*)&appletName, sizeof(appletName), selectResponseData, &selectResponseDataLen);
    return (smStatus_t)ret;
}

smStatus_t Se05x_T4T_API_SelectFile(pSe05xSession_t session_ctx, uint8_t* fileId, size_t fileIDLen)
{
    tlvHeader_t hdr = {{0x00, 0xA4, 0x00, kSE05x_P2_DEFAULT}};
    return DoAPDUTx_s_Case3(session_ctx, &hdr, fileId, fileIDLen);
}

smStatus_t Se05x_T4T_API_ReadBinary(
    pSe05xSession_t session_ctx, uint8_t *output, size_t *outlen)
{
    smStatus_t retStatus                       = SM_NOT_OK;
    tlvHeader_t hdr                            = {{0x00, kSE05x_T4T_INS_READ_BINARY, 0x00, kSE05x_P2_DEFAULT}};
    uint8_t rspbuf[SE05X_T4T_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                           = &rspbuf[0];
    size_t rspbufLen                           = ARRAY_SIZE(rspbuf);

    retStatus = DoAPDUTxRx_s_Case2(session_ctx, &hdr, NULL, 0, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus = SM_NOT_OK;

        if (rspbufLen < 2) {
            goto cleanup;
        }
        retStatus = (smStatus_t)((pRspbuf[rspbufLen - 2] << 8) | pRspbuf[rspbufLen-1]);
        if (retStatus == SM_OK && rspbufLen > 2) {
            memcpy(output, rspbuf, rspbufLen - 2);
            *outlen = rspbufLen - 2;
        }
        else {
            *outlen = 0;
        }
    }
cleanup:
    return retStatus;
}

smStatus_t Se05x_T4T_API_UpdateBinary(pSe05xSession_t session_ctx, uint8_t *data, size_t dataLen)
{
    tlvHeader_t hdr = {{0x00, kSE05x_T4T_INS_UPDATE_BINARY, 0x00, kSE05x_P2_DEFAULT}};
    return DoAPDUTx_s_Case3(session_ctx, &hdr, data, dataLen);
}

smStatus_t Se05x_T4T_API_GetVersion(pSe05xSession_t session_ctx, uint8_t *version, size_t *versionLen)
{
    smStatus_t retStatus                       = SM_NOT_OK;
    int tlvRet                                 = 0;
    tlvHeader_t hdr                            = {{0x00, kSE05x_T4T_GET_DATA, 0x9F, 0x7E}};
    uint8_t rspbuf[SE05X_T4T_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                           = &rspbuf[0];
    size_t rspbufLen                           = ARRAY_SIZE(rspbuf);

    retStatus = DoAPDUTxRx_s_Case2(session_ctx, &hdr, NULL, 0, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus       = SM_NOT_OK;
        size_t rspIndex = 0;
        tlvRet          = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_1, version, versionLen);
        if (0 != tlvRet) {
            goto cleanup;
        }
        if ((rspIndex + 2) == rspbufLen) {
            retStatus = (smStatus_t)((pRspbuf[rspIndex] << 8) | (pRspbuf[rspIndex + 1]));
        }
    }
cleanup:
    return retStatus;
}

smStatus_t Se05x_T4T_API_ManageReadCounter(pSe05xSession_t session_ctx, SE05x_T4T_Read_Ctr_Operation_t readCtrOperation)
{
    tlvHeader_t hdr = {{kSE05x_CLA, kSE05x_T4T_INS_MANAGE_READ_CTR, 0x00, 0x00}};

    switch (readCtrOperation) {
    case kSE05x_T4T_Read_Counter_Reset: {
        hdr.hdr[2] = 0x00;
        hdr.hdr[3] = 0x00;
        break;
    }
    case kSE05x_T4T_Read_Counter_Enable: {
        hdr.hdr[2] = 0x00;
        hdr.hdr[3] = 0xFF;
        break;
    }
    case kSE05x_T4T_Read_Counter_Disable: {
        hdr.hdr[2] = 0xFF;
        hdr.hdr[3] = 0xFF;
        break;
    }
    default: {
        return SM_NOT_OK;
    }
    }

    return DoAPDUTx_s_Case3(session_ctx, &hdr, NULL, 0);
}

smStatus_t Se05x_T4T_API_ReadCounterValue(pSe05xSession_t session_ctx, uint8_t *ctrValue, size_t *ctrValueLen)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr                            = {{0x00, kSE05x_T4T_GET_DATA, 0x9F, 0x3A}};
    uint8_t rspbuf[SE05X_T4T_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                       = &rspbuf[0];
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);

    retStatus = DoAPDUTxRx_s_Case2(session_ctx, &hdr, NULL, 0, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus       = SM_NOT_OK;

        if (rspbufLen != 6) {
            goto cleanup;
        }
        retStatus = (smStatus_t)((pRspbuf[4] << 8) | pRspbuf[5]);
        if(retStatus == SM_OK) {
            memcpy(ctrValue, rspbuf, 4);
            *ctrValueLen = 4;
        }
    }
cleanup:
    return retStatus;
}

smStatus_t Se05x_T4T_API_ConfigureAccessCtrl(pSe05xSession_t session_ctx,
    SE05x_T4T_Interface_Const_t seInterface,
    SE05x_T4T_Operation_Const_t operaion,
    SE05x_T4T_Access_Ctrl_t accessCtrl)
{
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_T4T_INS_CONFIGURE_ACCESS_CTRL, operaion, seInterface}};
    uint8_t cmdbuf[2]                          = {0};
    uint8_t *pCmdbuf     = &cmdbuf[0];
    size_t cmdbufLen                           = 0;

    cmdbuf[0] = accessCtrl;
    cmdbufLen++;

    return  DoAPDUTx_s_Case3(session_ctx, &hdr, pCmdbuf, cmdbufLen);
}

smStatus_t Se05x_T4T_API_ReadAccessCtrl(pSe05xSession_t session_ctx,
    SE05x_T4T_Interface_Const_t seInterface,
    SE05x_T4T_Access_Ctrl_t *readOperation,
    SE05x_T4T_Access_Ctrl_t *writeOperation)
{
    smStatus_t retStatus                   = SM_NOT_OK;
    tlvHeader_t hdr                            = {{0x00, kSE05x_T4T_GET_DATA, 0x00, 0x00}};
    uint8_t rspbuf[SE05X_T4T_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                       = &rspbuf[0];
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);

    if (seInterface == kSE05x_T4T_Interface_Contact) {
        hdr.hdr[3] = 0xD7;
    }
    else if (seInterface == kSE05x_T4T_Interface_Contactless) {
        hdr.hdr[3] = 0xD8;
    }
    else {
        return SM_NOT_OK;
    }

    retStatus = DoAPDUTxRx_s_Case2(session_ctx, &hdr, NULL, 0, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus = SM_NOT_OK;

        if ((rspbufLen != 4)) {
            goto cleanup;
        }
        retStatus = (smStatus_t)((pRspbuf[2] << 8) | (pRspbuf[3]));
        if (retStatus == SM_OK) {
            *readOperation = (SE05x_T4T_Access_Ctrl_t)pRspbuf[0];
            *writeOperation = (SE05x_T4T_Access_Ctrl_t)pRspbuf[1];
        }
    }
cleanup:
    return retStatus;
}
