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
// Includes
//*****************************************************************************
// Standard includes

#ifndef UTILS_IF_H
#define UTILS_IF_H

#include <pthread.h>
#include <ti/net/http/httpclient.h>

/* OS Task Creation Flags */
#define OS_TASK_FLAG_DETACHED 0x00000001

/*!

    \brief     Write file content (from user buffer)

    \param[in] pFilename - file name (full path in device file system)
    \param[in] length - content size
    \param[in] pValue - write buffer
    \param[in] token - write access token

    \return    Number of bytes written upon success, or negative error code

*/
int FILE_write(char *pFilename, uint16_t length, uint8_t* pValue, uint32_t *pToken, uint32_t flags);

/*!

    \brief     Read file content (into user buffer)

    \param[in] pFilename - file name (full path in device file system)
    \param[in] length - content size
    \param[in] pValue - buffer for the read content
    \param[in] token - read access token

    \return    Number of bytes written upon success, or negative error code

*/
int FILE_read(int8_t *pFilename, uint16_t length, uint8_t* pValue, uint32_t token);

/*!

    \brief     Create a POSIX execution thread

    \param[in] prio - task priority
    \param[in] stacksize - thread's stack size
    \param[in] fTask - task's main function
    \param[in] arg - task's arguments
    \param[in] flags - bit mask of task flags. Support value:
                        OS_TASK_FLAG_DETACHED

    \return    0 upon success, or negative error code

*/
pthread_t OS_createTask(int32_t prio, size_t stacksize, void* (*fTask)(void*), void *arg, uint32_t flags);

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
                         uint32_t   flags);
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
                             uint32_t flags);
/*!

    \brief     Work Queue handle
*/
typedef void * workQ_t;


/*!

    \brief     User's Work Queue Callback (invoked in WorkQueue thread context)

    \param[in] pData - pointer to a data to be processed

    \return    0 upon success, or negative error code

*/
typedef int32_t (*workQCallback_f)(void *pData);


/*!

    \brief     Create a Work Queue (if same WQ structure is shared between
                modules, only the first call will actually create the context.

    \param[in] prio - WQ task priority
    \param[in] stacksize - thread's stack size
    \param[in] maxMsg - messageQueue size
    \param[in] pName - pointer to wq name

    \return    worqQ_t (WQ handle pointer) in case of success,  or NULL else

*/
workQ_t WQ_create(int32_t prio, size_t stacksize, int32_t maxMsgs, char *pName);


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
int32_t WQ_schedule(workQ_t hWQ, workQCallback_f callback, void *pData, uint16_t dataLen);

#endif // OTA_IF_H
