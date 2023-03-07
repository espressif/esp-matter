/*
 * Copyright (C) 2016-2021, Texas Instruments Incorporated
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

//*****************************************************************************
//
//! \addtogroup file_operations
//! @{
//
//*****************************************************************************

// Standard includes


#include <ifmod/debug_if.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <mqueue.h>

#include <ti/drivers/net/wifi/simplelink.h>
#include "utils_if.h"
#include "debug_if.h"

#undef DEBUG_IF_NAME
#define DEBUG_IF_NAME       "UTILS"
#undef DEBUG_IF_SEVERITY
#define DEBUG_IF_SEVERITY   E_INFO



//*****************************************************************************
// Helper function
//*****************************************************************************
/*!

    \brief     Write file content (from user buffer)

    \param[in] pFilename - file name (full path in device file system)
    \param[in] length - content size
    \param[in] pValue - write buffer
    \param[in] token - write access token

    \return    0 upon success, or negative error code

*/
int FILE_write(char *pFilename, uint16_t length, uint8_t* pValue, uint32_t *pToken, uint32_t flags)
{
    int32_t  lFileHandle;
    int   rc;
    uint32_t ulToken = 0;
    int32_t  OpenFlags = 0;
    int numBytes;

    /* Open the file as bundle !!!*/
    OpenFlags = (SL_FS_CREATE | SL_FS_OVERWRITE);
    OpenFlags |= (SL_FS_CREATE_SECURE | SL_FS_CREATE_NOSIGNATURE);
    OpenFlags |= flags;

    if(!pToken)
    {
        pToken = &ulToken;
        OpenFlags |= (SL_FS_CREATE_PUBLIC_WRITE | SL_FS_CREATE_PUBLIC_READ);
    }

    lFileHandle = sl_FsOpen((uint8_t *)pFilename,  OpenFlags| SL_FS_CREATE_MAX_SIZE( length ), (unsigned long *)pToken);
    if(lFileHandle < 0)
    {
        LOG_TRACE("FILE_write: Error sl_FsOpen %s, Status=%d", pFilename, lFileHandle);
        return (int16_t) lFileHandle;
    }

    rc = (int16_t) sl_FsWrite(lFileHandle, 0, (uint8_t *) pValue, length);
    if (rc < 0)
    {
        LOG_TRACE("FILE_write: Error sl_FsWrite, Status=%d", rc);
        return rc;
    }
    else
    {
        numBytes = rc;
    }

    rc = sl_FsClose(lFileHandle, NULL, NULL, 0);
    if(rc < 0)
    {
        LOG_TRACE("FILE_write: Error sl_FsClose, Status=%d", rc);
        return rc;
    }

    return numBytes;
}

/*!

    \brief     Read file content (into user buffer)

    \param[in] pFilename - file name (full path in device file system)
    \param[in] length - content size
    \param[in] pValue - buffer for the read content
    \param[in] token - read access token

    \return    0 upon success, or negative error code

*/
int FILE_read(int8_t *pFilename, uint16_t length, uint8_t* pValue, uint32_t token)
{
    int32_t lFileHandle;
    int rc;
    int numBytes;

    lFileHandle = (int32_t)sl_FsOpen((uint8_t *)pFilename, SL_FS_READ, (unsigned long *)&token);
    if(lFileHandle < 0)
    {
        if (lFileHandle == SL_ERROR_FS_FILE_NOT_EXISTS)
        {
            LOG_TRACE("FILE_read: file %s, status=SL_ERROR_FS_FILE_NOT_EXISTS", pFilename);
        }
        else
        {
            LOG_TRACE("FILE_read: Error sl_FsOpen %s, Status=%d", pFilename, lFileHandle);
        }
        return (int16_t)lFileHandle;
    }

    rc = (int16_t)sl_FsRead(lFileHandle , 0, (uint8_t *)pValue, length);
    if(rc < 0)
    {
        LOG_TRACE("FILE_read: Error sl_FsRead, Status=%d", rc);
        return rc;
    }
    else
    {
        numBytes = rc;
    }

    rc = sl_FsClose(lFileHandle, NULL, NULL, 0);
    if(rc < 0)
    {
        LOG_TRACE("FILE_read: Error sl_FsClose, Status=%d", rc);
        return rc;
    }

    return numBytes;
}



/*!

    \brief     Create a POSIX execution thread

    \param[in] prio - task priority
    \param[in] stacksize - thread's stack size
    \param[in] fTask - task's main function
    \param[in] arg - task's arguments

    \return    0 upon success, or negative error code

*/
pthread_t OS_createTask(int32_t prio, size_t stacksize, void* (*fTask)(void*), void *arg, uint32_t flags)
{
    int32_t status = 0;
    pthread_attr_t attr;
    struct sched_param priParam;
    pthread_t thread;

    /* Start the SimpleLink Host */
    pthread_attr_init(&attr);
    priParam.sched_priority = prio;
    status = pthread_attr_setschedparam(&attr, &priParam);
    status |= pthread_attr_setstacksize(&attr, stacksize);

    if(flags & OS_TASK_FLAG_DETACHED)
    {
        status |= pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    }
    if(status)
    {
        LOG_DEBUG("Task attributes error (%d)", status);
        while (1)
            ;
    }
    status = pthread_create(&thread, &attr, fTask, arg);
    if(status)
    {
        LOG_DEBUG("Task create failed (%d)", status);
        while (1)
            ;
    }
    return thread;
}

typedef struct
{
    pthread_t           thread;
    mqd_t               msgQ;
    char                *pName;
} wq_t;


typedef struct
{
    workQCallback_f     callback;
    void                *pData;
} wqMsg_t;

static void *WorkQTask(void *hWQ)
{
    wq_t *pWQ = (wq_t*)hWQ;
    wqMsg_t wqMsg;

    while(1)
    {
        mq_receive(pWQ->msgQ, (char*)&wqMsg, sizeof(wqMsg_t), NULL);

        LOG_DEBUG("WorkQTask[%s]:: callback=%p", pWQ->pName, wqMsg.callback);
        if(wqMsg.callback)
        {
            int ret = (wqMsg.callback)(wqMsg.pData);
            if (ret < 0) {
                LOG_DEBUG("WorkQTask[%s]:: Callback error (%d)", pWQ->pName, ret);
            }
        }
        else
            break;
    }
    LOG_ERROR("WorkQTask[%s]:: Exit", pWQ->pName);
    return NULL;
}

/*!

    \brief     Create a Work Queue (if same WQ structure is shared between
                modules, only the first call will actually create the context.

    \param[in] prio - WQ task priority
    \param[in] stacksize - thread's stack size
    \param[in] maxMsg - messageQueue size

    \return    worqQ_t (WQ handle pointer) in case of success,  or NULL else

*/
workQ_t WQ_create(int32_t prio, size_t stacksize, int32_t maxMsgs, char *pName)
{
    static uint8_t wq_instance = 0;
    wq_t *pWQ = (wq_t*)malloc(sizeof(wq_t));
    workQ_t ret = (workQ_t)NULL;

    if(pWQ)
    {
        mq_attr attr;
        if(pName)
        {
            pWQ->pName = pName;
        }
        else
        {
            pWQ->pName = "WQ_0";
            pWQ->pName[3] = '0' + (wq_instance);
        }
        wq_instance++;

        attr.mq_maxmsg = maxMsgs;
        attr.mq_msgsize = sizeof(wqMsg_t);
        pWQ->msgQ = mq_open(pWQ->pName, O_CREAT, 0, &attr);
        if(((int)pWQ->msgQ) <= 0) {
            LOG_DEBUG("WorkQTask[%s]:: msgQ creation failed (%d)", pWQ->pName, (int)pWQ->msgQ);
            free(pWQ);
        }
        else {
            pWQ->thread = OS_createTask(prio, stacksize, WorkQTask, pWQ, PTHREAD_CREATE_DETACHED);
            ret = (workQ_t)pWQ;
        }
    }

    return ret;
}


/*!

    \brief     Schedule a work (sends a message for specific module ID).

    \param[in] pWQ - pointer to a WorkQ context
    \param[in] callback - workQ callback
    \param[in] pData - pointer to data buffer to process
    \param[in] dataLen - data length (in case the buffer needs to be allocated and copied),
                         0 if the data buffer is handled by the user (in
                         such case only the pointer will be used).

                         Note 1: Set dataLen only if the data buffer will not be valid in the
                                 WQ thread context (e.g. allocated on stack, etc)
                         Note 2: The Callback will need to free the memory in case the data len was set

    \return    module id (>=0) upon success, or negative error code

*/
int32_t WQ_schedule(workQ_t hWQ, workQCallback_f callback, void *pData, uint16_t dataLen)
{
    int32_t ret = SL_ERROR_BSD_EINVAL;
    wq_t *pWQ = (wq_t*)hWQ;
    if(pWQ && callback) {
        wqMsg_t wqMsg;
        wqMsg.callback = callback;
        if(dataLen)
        {
            uint8_t *pCopiedData = (uint8_t *)malloc(dataLen);
            if(pCopiedData) {
                memcpy(pCopiedData, pData, dataLen);
                wqMsg.pData = pCopiedData;
            }
            else {
                LOG_ERROR("WQ_Schedule:: No room for new message");
                return SL_ERROR_BSD_ENOMEM;
            }
        }
        else {
            wqMsg.pData = pData;
        }
        ret = mq_send(pWQ->msgQ, (char*)&wqMsg, sizeof(wqMsg_t), 0);
        if(0 > ret) {
            LOG_ERROR("WQ_Schedule:: No room in the MsgQ");
        }
        else {
            ret = 0;
        }
    }
    return ret;
}

/*!

    \brief     Setup HTTP Connection using Server Root CA and backup

    \param[in/out] httpClientHandle - http client handle pointer
                            (if NULL, the handle will be created)
    \param[in] pHostName - Host to connect to
    \param[in] pPrivateKey - Client private key (if needed), or NULL
    \param[in] pClientCert - Client certifcate (if needed), or NULL
    \param[in] pRootCa1 - Server's primary root CA (path to cert file in file system)
    \param[in] pRootCa1 - Server's backup root CA (path to cert file in file system)
                          or NULL, if the backup is not needed
    \param[in] flags - TBD (0)

    \return    0 upon success, or negative error code

*/
int HTTP_setupConnection(HTTPClient_Handle *httpClientHandle,
                         const char *pHostName,
                         const char *pPrivateKey, const char *pClientCert,  /* Client key pair */
                         const char *pRootCa1, const char *pRootCa2,        /* Root CA + alternative */
                         uint32_t   flags)
{
    int16_t ret = 0;
    // Create httpClient Handle
    if(*httpClientHandle == NULL)
    {
        *httpClientHandle = HTTPClient_create(&ret,0);
        if(ret < 0) {
            LOG_ERROR("creation of http client handle failed: %d", ret);
        }
    }
    if(*httpClientHandle)
    {
        HTTPClient_extSecParams secParams;
        secParams.clientCert = pClientCert;
        secParams.privateKey = pPrivateKey;
        secParams.rootCa     = pRootCa1;
        LOG_DEBUG("Connecting to %s with rootCa %s", pHostName, pRootCa1);
        // Connect to HTTP server
        ret = HTTPClient_connect(*httpClientHandle, pHostName, &secParams,0);
        if(ret == SL_ERROR_BSD_ESEC_ASN_NO_SIGNER_E && pRootCa2)
        {
            secParams.rootCa     = pRootCa2;
            LOG_DEBUG("Connecting to %s with rootCa %s", pHostName, pRootCa2);
            // Connect to HTTP server
            ret = HTTPClient_connect(*httpClientHandle, pHostName, &secParams,0);

        }
        if(ret < 0) {
            LOG_ERROR("connect failed: %d", ret);
        }
    }
    if(ret < 0)
    {
        HTTPClient_destroy(*httpClientHandle);
        *httpClientHandle = NULL;
    }
    return ret;
}

/*!

    \brief     Prepare HTTP request headers

    \param[in] httpClientHandle - http client handle
    \param[in] pUserAgent - pointer to User Agent string
                            (or NULL if not header field is needed)
    \param[in] pAppToken - pointer to APP Token string
                            (or NULL if not header field is needed)
    \param[in] pContentType - pointer to Content Type Agent string
                            (or NULL if not header field is needed)
    \param[in] flags - HTTPClient_HFIELD_PERSISTENT / HTTPClient_HFIELD_NOT_PERSISTENT

    \return    0 upon success, or negative error code

*/
int HTTP_setHeaders(HTTPClient_Handle httpClientHandle,
                             char *pUserAgent,
                             char *pAppToken, char *pContentType,
                             uint32_t flags)
{
    int16_t ret = 0;

    if(httpClientHandle)
    {
        if(pUserAgent)
        {
            // Set headers for HTTP request
            ret = HTTPClient_setHeader(httpClientHandle,
                                    HTTPClient_HFIELD_REQ_USER_AGENT,
                                    pUserAgent, strlen(pUserAgent)+1,
                                    flags);
            if(ret < 0) {
                LOG_ERROR("setting User-Agent header failed: %d", ret);
            }
        }
        if(ret == 0 && pAppToken)
        {
            // Authorization header is required for polling private user folder.
            ret = HTTPClient_setHeader(httpClientHandle,
                                       HTTPClient_HFIELD_REQ_AUTHORIZATION,
                                       pAppToken, strlen(pAppToken)+1,
                                       flags);
            if(ret < 0) {
                LOG_ERROR("setting Authorization header failed: %d", ret);
            }
        }
        if(ret == 0 && pContentType)
        {
            // Content-Type header is required for signaling the kind of response expected.
            ret = HTTPClient_setHeader(httpClientHandle,
                                       HTTPClient_HFIELD_REQ_CONTENT_TYPE,
                                       pContentType, strlen(pContentType)+1,
                                       flags);
            if(ret < 0) {
                LOG_ERROR("setting Content-Type header failed: %d", ret);
            }
        }
        if(ret < 0) {
            HTTPClient_destroy(httpClientHandle);
        }
    }
    else
    {
        ret = HTTPClient_ENULLPOINTER;
    }
    return(ret);
}



