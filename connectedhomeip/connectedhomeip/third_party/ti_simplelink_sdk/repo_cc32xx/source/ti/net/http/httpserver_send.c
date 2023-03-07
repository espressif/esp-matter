/*
 * Copyright (c) 2012-2019 Texas Instruments Incorporated - http://www.ti.com
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

/*
 * ======== httpserver_send.c ========
 * Send functions used by HTTP server and applications
 *
 */

#include <stdio.h>
#include <string.h>

#include "httpserver.h"
#include "http.h"
#include "logging.h"

static const char *CONTENT_TYPE_PLAIN   = "text/plain";
static const char *CONTENT_TYPE         = "Content-Type:";
static const char *CONTENT_LENGTH       = "Content-Length:";
static const char *HTTP_VER             = "HTTP/1.1";

/*
 *  Size of a status and/or content type response header. Worst case is max of:
 *
 *  HTTP/1.1 NNN Explanation\r\n
 *  Content-type: some-mime-type\r\n
 *
 *  Currently, "Explanation" is max 22 char, so 37 for status line
 *  "Content-type: " and some-mime-type, assume 100 or less char.
 */
#define MAXRESPONSESIZE  100

/*
 *  Max size of the "Failed: NNN STATUS TEXT" message sent in
 *  HTTPServer_sendErrorResponse, i.e.
 *
 *  "Failed: 401 Authorization Required"
 */
#define MAXFAILUREMSGSIZE 35

typedef struct StatusMap {
    int  status;
    const char * message;
} StatusMap;

static StatusMap httpStatusMap[] =
{
    {HTTP_SC_OK, "OK"},
    {HTTP_SC_NO_CONTENT, "No Content"},
    {HTTP_SC_BAD_REQUEST, "Bad Request"},
    {HTTP_SC_UNAUTHORIZED, "Authorization Required"},
    {HTTP_SC_NOT_FOUND, "Not Found"},
    {HTTP_SC_NOT_IMPLEMENTED, "Not Implemented"},
    {HTTP_SC_METHOD_NOT_ALLOWED, "Not Allowed"},
    {0, "Unknown"}
};

int (*HTTPServer_errorResponseHook)(int s, int StatusCode) = 0;

static const char * getStatusMessage(int status)
{
    int  i;

    for (i = 0; ((httpStatusMap[i].status) &&
            (httpStatusMap[i].status != status)); i++);

    return (httpStatusMap[i].message);
}

/*
 * Send the status line, including \r\n.
 */
static void httpSendStatusLine(int s, int status)
{
    char buf[MAXRESPONSESIZE];
    const char * msg;
    int len;

    Log_print1(Diags_ANALYSIS, "sendStatusLine> %d", (IArg)status);

    if (status < 0 || status > 999) {
        status = 999;
    }

    msg = getStatusMessage(status);

    len = snprintf(buf, sizeof(buf), "%s %3d %s\r\n", HTTP_VER,
            status, msg);

    if (len == sizeof(buf)) {
        Log_print1(Diags_WARNING,
                "sendStatusLine> Buffer size too small: %d", sizeof(buf));
    }

    send(s, buf, len, 0);
}

/*
 * Write out the entity length tag.
 */
static void httpSendEntityLength(int s, int32_t entityLen)
{
    char buf[32]; /* sizeof("Content-Length: ") + 11 + 4 + 1]; */
    size_t size = sizeof(buf);
    int len;

    len = snprintf(buf, size, "%s%d\r\n", CONTENT_LENGTH, entityLen);
    send(s, buf, len, 0);
}

/*
 * Write out the content type header.
 */
static void httpSendContentType(int s, const char *type)
{
     char buf[MAXRESPONSESIZE];
     size_t size = sizeof(buf);
     int len;

     len = snprintf(buf, size, "%s%s\r\n", CONTENT_TYPE, type);

     if (len == sizeof(buf)) {
         Log_print1(Diags_WARNING,
                 "sendContentType> Buffer size too small: %d", sizeof(buf));
     }

    send(s, buf, len, 0);
}

void HTTPServer_sendErrorResponse(int s, int status)
{
    /* send a default response if there is no user callback */
    if (!HTTPServer_errorResponseHook ||
            !HTTPServer_errorResponseHook(s, status)) {
        int len;
        char buf[MAXFAILUREMSGSIZE];

        len = snprintf(buf, sizeof(buf), "Failed: %d %s", status,
                        getStatusMessage(status));
        //Log_print2(Diags_ANALYSIS, "len = %d: %s\n", len, (xdc_IArg)buf);
        HTTPServer_sendSimpleResponse(s, status, CONTENT_TYPE_PLAIN, len, buf);
    }
}

void HTTPServer_sendSimpleResponse(int s, int status, const char * type,
                        size_t len, const void * buf)
{
    httpSendStatusLine(s, status);

    if (type) {
        httpSendContentType(s, type);
    }

    if (status != HTTP_SC_NO_CONTENT) {
        httpSendEntityLength(s, len);
    }

    /* done with status/headers, send 2nd \r\n */
    send(s, "\r\n", 2, 0);

    if (len > 0 && buf) {
        send(s, buf, len, 0);
    }
}

void HTTPServer_sendResponse(int s, int status, const char * headers[],
        int numHeaders, size_t len, const void * buf)
{
    int i;

    httpSendStatusLine(s, status);

    for (i = 0; i < numHeaders; i++) {
        send(s, headers[i], strlen(headers[i]), 0);
        send(s, "\r\n", 2, 0);
    }

    if (status != HTTP_SC_NO_CONTENT) {
        httpSendEntityLength(s, len);
    }

    /* done with status/headers, send 2nd \r\n */
    send(s, "\r\n", 2, 0);

    if (len > 0 && buf) {
        send(s, buf, len, 0);
    }
}

void HTTPServer_sendResponseChunked(int s, int status,
                                 const char * type)
{
    const char field[] = "Transfer-Encoding: chunked\r\n\r\n";

    httpSendStatusLine(s, status);

    if (type) {
        httpSendContentType(s, type);
    }

    send(s, field, sizeof(field) - 1, 0);
}

void HTTPServer_sendChunk(int s, const void * buf, size_t len)
{
    char sizeBuf[12];
    int sizeLen;

    sizeLen = snprintf(sizeBuf, sizeof(sizeBuf), "%x\r\n", len);
    send(s, sizeBuf, sizeLen, 0);
    if (len > 0) {
        send(s, buf, len, 0);
    }
    send(s, "\r\n", 2, 0);
}
