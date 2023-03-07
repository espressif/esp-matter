/*
 * Copyright (c) 2012-2018 Texas Instruments Incorporated - http://www.ti.com
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

#if defined(xdc_runtime_Log_DISABLE_ALL) && \
    defined(xdc_runtime_Assert_DISABLE_ALL)
#define NOREGISTRY 1
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

/*
 * This shouldn't be Linux-specific; every environment should #include
 * <unistd.h> for the definition of close().
 *
 * Unfortunately, until NS socket descriptors are _file_ descriptors,
 * we have a conflict between the NS-defined close() (for sockets) and
 * the unistd.h-defined close() (for files).
 *
 * Linux doesn't have this conflict, so it (correctly) uses
 * unistd.h-defined close().  Everyone else uses NS-defined close(),
 * so they should _not_ bring in unistd.h>.
 */
#ifdef __linux__
#include <unistd.h>
#endif

#include <sys/socket.h>
#include <sys/select.h>

#ifdef __linux__
#include <fcntl.h>
#include <sys/eventfd.h>
#endif

#ifdef __NDK__
#include <ti/sysbios/knl/Task.h>
#endif

#include <netinet/in.h>
#include <arpa/inet.h>
#include "urlhandler.h"
#include "logging.h"

#include "httpserver.h"
#include "http.h"

#include <pthread.h>
#include <mqueue.h>

#define TAG_GET         1
#define TAG_POST        2
#define TAG_PUT         3
#define TAG_PATCH       4
#define TAG_DELETE      5
#define TAG_LASTMETHOD  TAG_DELETE
#define TAG_CLEN        6
#define TAG_AUTH        7
#define TAG_HOST        8
#define TAG_DONTCARE    9

 /* end of line - line content in buf */
#define EOLINE  1

/* end of header */
#define EOHEADER 0

/* unknown error */
#define SOCKETERROR -1

#define TIMEOUTUS  (500 * 1000) /* 500 ms */

/* HomeKit requires disconnecting rudely, so set this to 1 */
#define HTTPSRV_DISCONNECTRUDE 1

/* TODO - Needs to be configurable by the user */
#define SERVESENDTHREAD_STACKSIZE (2560)

#define MQ_MAXMSG  3
#define MQ_MSGSIZE 1

#define LOOPBACKADDR    0x7F000001 /* 127.0.0.1 */

#define DEFAULT_TIMEOUT 60
#define DEFAULT_MAXLINELEN 256
#define DEFAULT_MAXURILEN 112
#define DEFAULT_MAXSESSIONS 16

typedef struct Session {
    struct Session * next;
    int s;
    char * line;
    char * uri;
    bool stop;
    int sendRequest;
    HTTPServer_Handle srv;
    URLHandler_Handle urlh[];
} Session;

typedef struct HTTPServer_Object {
    int skt;
#ifdef __linux__
    int cmdfd;                  /* command pipe */
#else
    uint16_t port;
    bool isPortSet;
#endif
    int timeout;
    int maxLineLen;
    int maxURILen;
    int maxSessions;
    int numURLh;
    bool stop;
    bool isSecure;
    pthread_mutex_t sendMutex;
    mqd_t sendMq;
    Session * sessions;
    SlNetSockSecAttrib_t * secAttribs;
    URLHandler_Setup setup[];
} HTTPServer_Object;

typedef struct SendThreadArgs {
    HTTPServer_Handle srv;
    char *mqName;
} SendThreadArgs;

enum SendType {
    EVENT_NOTIFY = 'E',
    HTTP2_PING = 'P',
    STOP_THREAD = 'S'
};

#ifndef NOREGISTRY
Registry_Desc ti_net_http_HTTPServer_desc;
#endif

#define POLLPERIOD 50

static void inline closeSocket(int s)
{
    Log_print1(Diags_ENTRY, "closeSocket> enter (%d)", s);

    close(s);

    Log_print0(Diags_EXIT, "closeSocket> exit");
}

static int httpExtractTag(char * tag)
{
    if (!strncmp("GET", tag, 3)) {
        return (TAG_GET) ;
    }
    if (!strncmp("PUT", tag, 3)) {
        return (TAG_PUT);
    }
    if (!strncmp("PATCH", tag, 5)) {
        return (TAG_PATCH);
    }
    if (!strncmp("DELETE", tag, 6)) {
        return (TAG_DELETE);
    }
    if (!strncmp("POST", tag, 4)) {
        return (TAG_POST);
    }
    if (!strncmp("Content-Length: ", tag, 16)) {
        return (TAG_CLEN);
    }
    if (!strncmp("Host: ", tag, 6)) {
        return (TAG_HOST);
    }

    return (TAG_DONTCARE);
}

static int handle404(int s, int contentLength)
{
    int len;
    uint8_t buf[32];

    /* dump the request body */
    while (contentLength > 0) {
        len = contentLength > sizeof(buf) ? sizeof(buf) : contentLength;
        len = recv(s, buf, len, 0);
        if (len > 0) {
            contentLength -= len;
        }
        else {
            break;
        }
    }

    HTTPServer_sendErrorResponse(s, HTTP_SC_NOT_FOUND);

    return (contentLength);
}

static int httpRecvLine(int s, char * buf, int bufLen)
{
    ssize_t nbytes;
    int len;

    if ((nbytes = recv(s, buf, 2, 0)) <= 0) {
        if (nbytes < 0) {
            HTTPServer_sendErrorResponse(s, HTTP_SC_BAD_REQUEST);
        }

        return SOCKETERROR;
    }

    if (buf[0] == '\r' && buf[1] == '\n') {
        return EOHEADER;
    }

    len = 2;
    do {
        if ((nbytes = recv(s, &buf[len], 1, 0)) <= 0) {
            if (nbytes < 0) {
                HTTPServer_sendErrorResponse(s, HTTP_SC_BAD_REQUEST);
            }

            return SOCKETERROR;
        }

        if (++len == bufLen) {
            HTTPServer_sendErrorResponse(s, HTTP_SC_REQUEST_ENTITY_TOO_LARGE);

            return SOCKETERROR;
        }
    } while (!(buf[len - 2] == '\r' && buf[len - 1] == '\n'));

    buf[len] = 0;

    return EOLINE;
}

static int transact(HTTPServer_Handle srv, Session * session)
{
    int s = session->s;
    char * line = session->line;
    int lineLen = srv->maxLineLen;
    char * uri = session->uri;
    int uriLen = srv->maxURILen;
    char * uriArgs;
    int method;
    int status;
    int contentLength;
    char * beg = NULL;
    char * end = NULL;
    int i;

    if ((status = httpRecvLine(s, line, lineLen)) <= 0) {
        Log_print1(Diags_ANALYSIS, "transact> httpRecvLine %d", (IArg)status);
        status = 1;
        goto END;
    }

    /*
     *  Parse the request line which should look like:
     *
     *  METHOD /uri HTTP/1.1
     *
     * TODO: handle cases of missing URI and or HTTP/1.1
     * TODO: look for orphan CR or LF in line
     */
    if ((method = httpExtractTag(line)) > TAG_LASTMETHOD) {
        HTTPServer_sendErrorResponse(s, HTTP_SC_BAD_REQUEST);
        status = 1;
        goto END;
    }

    beg = strchr(line, ' ');
    if (beg == NULL) {
        /* missing the URI */
        HTTPServer_sendErrorResponse(s, HTTP_SC_BAD_REQUEST);
        status = 1;
        goto END;
    }

    while (*beg == ' ') {
        beg++;
    }

    end = strchr(beg, ' ');
    if (end == NULL) {
        /* missing HTTP/1.x */
        HTTPServer_sendErrorResponse(s, HTTP_SC_BAD_REQUEST);
        status = 1;
        goto END;
    }

    if (uriLen > (end - beg)) {
        strncpy(uri, beg, end - beg);
        uri[end - beg] = 0;
    }
    else {
        HTTPServer_sendErrorResponse(s, HTTP_SC_REQUEST_ENTITY_TOO_LARGE);
        status = 1;
        goto END;
    }

    /* extract any CGI args from the URI */
    if ((uriArgs = strchr(uri, '?'))) {
        *uriArgs++ = 0;
    }

    /*
     *  Receive and process all the remaining fields in the
     *  request header, looking for the ones we care about.
     *
     *  TODO: enable users to specify required fields and
     *  get the associated data, possibly via callbacks. Some
     *  of the fields should really go to the URLHandlers,
     *  like Content-Type.
     */
    contentLength = 0;
    while ((status = httpRecvLine(s, line, lineLen)) > 0) {
        int nTag;

        nTag = httpExtractTag(line);

        if (nTag == TAG_CLEN) {
            /* 16 == sizeof("Content-Length: ") */
            contentLength = atoi(line + 16);
        }

        for (i = 0; i < srv->numURLh; i++) {
            if (srv->setup[i].scanField) {
                srv->setup[i].scanField(session->urlh[i], method, uri, line);
            }
        }
    }

    if (status < 0) {
        status = 1;
        goto END;
    }

    Log_print4(Diags_ANALYSIS, "start handler> %p %d %s %d", (IArg)srv, method,
               (IArg)uri, contentLength);

    status = URLHandler_ENOTHANDLED;
    for (i = 0; i < srv->numURLh; i++) {
        Log_print1(Diags_ANALYSIS, "urlh: %p", (xdc_IArg)session->urlh[i]);
        status = srv->setup[i].process(session->urlh[i], method, uri, uriArgs,
                                       contentLength, s);
        if (status != URLHandler_ENOTHANDLED) {
            break;
        }
    }

    if (status == URLHandler_EHANDLEDSTOP) {
        srv->stop = true;
    }

    Log_print2(Diags_ANALYSIS, "finish handler> %p %d", (IArg)srv, status);

    if (status == URLHandler_ENOTHANDLED) {
        status = handle404(s, contentLength);
    }
    else if (status == URLHandler_EERRORHANDLED) {
        status = 1;
    }
    else {
        status = 0;
    }

END:
    return (status);
}

static void deleteSession(HTTPServer_Handle srv, Session * session)
{
    int i;

    Log_print2(Diags_ENTRY, "deleteSession> enter (%p, %p)",
            (IArg)srv, (IArg)session);

    if (session) {
        for (i = 0; i < srv->numURLh; i++) {
            if (session->urlh[i]) {
                srv->setup[i].del(&session->urlh[i]);
            }
        }
        if (session->line) {
            free(session->line);
        }
        if (session->s) {
            closeSocket(session->s);
        }

        free(session);
    }
    Log_print0(Diags_EXIT, "deleteSession> exit");
}

static Session * createSession(HTTPServer_Handle srv, int s)
{
    Session * session;
    int i;
    struct timeval to;

    if ((session = calloc(1, sizeof(Session) +
                          srv->numURLh * sizeof(URLHandler_Handle))) == NULL) {
        return (NULL);
    }

    if ((session->line = malloc(srv->maxLineLen + srv->maxURILen)) == NULL) {
        deleteSession(srv, session);
        return (NULL);
    }

    session->uri = session->line + srv->maxLineLen;
    session->s = s;
    session->stop = false;
    session->next = NULL;
    session->srv = srv;

    for (i = 0; i < srv->numURLh; i++) {
        if (srv->setup[i].create) {
            if ((session->urlh[i] =
                 srv->setup[i].create(srv->setup[i].params,
                                      (URLHandler_Session)session)) == NULL) {

                deleteSession(srv, session);
                return (NULL);
            }
        }
        else {
            session->urlh[i] = NULL;
        }
    }

#if 0 /* TODO - should this be linux-only? or should slnetsock support this? */
#ifndef __SL__
    struct linger  lgr;

    lgr.l_onoff  = 1;
    lgr.l_linger = 5;
    (void)setsockopt(s, SOL_SOCKET, SO_LINGER, &lgr, sizeof(lgr));

    /* Configure our socket timeout to be 10 seconds */
    to.tv_sec  = 10;
    to.tv_usec = 0;
    (void)setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &to, sizeof(to));
#endif
#endif
    to.tv_sec  = srv->timeout;
    to.tv_usec = 0;
    (void)setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof(to));

    return (session);
}

static void pruneSession(HTTPServer_Handle srv)
{
    Session * session;
    Session * prev;
    bool found;

    Log_print1(Diags_ENTRY, "pruneSession> enter (%p)", (IArg)srv);

    do {
        prev = NULL;
        found = false;
        for (session = srv->sessions; session; session = session->next) {
            if (session->stop) {
                if (prev) {
                    prev->next = session->next;
                }
                else {
                    srv->sessions = session->next;
                }
                found = true;
                deleteSession(srv, session);
                break;
            }
            prev = session;
        }
    } while (found);

    Log_print0(Diags_EXIT, "pruneSession> exit");
}

static void removeSession(HTTPServer_Handle srv, Session * session)
{
    session->stop = true;
}

static int numSessions(HTTPServer_Handle srv)
{
    Session * session;
    int num = 0;

    for (session = srv->sessions; session; session = session->next) {
        num++;
    }

    return (num);
}

static Session * addSession(HTTPServer_Handle srv, int s)
{
    Session * newSession;
    Session * session;

    /*
     * If adding this session will make us exceed the max number of supported
     * sessions, remove the first session in the list to make room for this
     * one.
     */
    if (numSessions(srv) >= srv->maxSessions) {
        removeSession(srv, srv->sessions);
    }

    pruneSession(srv);

    if ((newSession = createSession(srv, s))) {
        if (srv->sessions == NULL) {
            srv->sessions = newSession;
        }
        else {
            for (session = srv->sessions; session->next;
                 session = session->next) {
            }
            session->next = newSession;
        }
    }

    Log_print1(Diags_ANALYSIS, "addSession> %d", (IArg)newSession);

    return (newSession);
}

void HTTPServer_init(void)
{
#ifndef NOREGISTRY
    static int regInit = false;

    if (!regInit) {
        Registry_addModule(&ti_net_http_HTTPServer_desc, "ti.net.http.HTTPServer");
        regInit = true;
    }
#endif
}

void HTTPServer_exit(void)
{
    /* TODO: can you remove a Registry entry? */
}

void HTTPServer_Params_init(HTTPServer_Params *params)
{
    params->timeout = DEFAULT_TIMEOUT;
    params->maxLineLen = DEFAULT_MAXLINELEN;
    params->maxURILen = DEFAULT_MAXURILEN;
    params->maxSessions = DEFAULT_MAXSESSIONS;
}

void HTTPServer_enableSecurity(HTTPServer_Handle srv,
                        SlNetSockSecAttrib_t * securityAttributes,
                        bool beginSecurely)
{
    srv->secAttribs = securityAttributes;
    srv->isSecure = beginSecurely;
}

HTTPServer_Handle HTTPServer_create(const URLHandler_Setup * setup, int numURLh,
                              HTTPServer_Params * params)
{
    HTTPServer_Handle srv;

    Log_print3(Diags_ENTRY, "HTTPServer_create> enter (%p), %d, (%p)",
            (IArg)setup, numURLh, (IArg)params);

    if ((srv = malloc(sizeof(HTTPServer_Object) +
                      numURLh * sizeof(URLHandler_Setup)))) {
        int i;

        srv->numURLh = numURLh;
        for (i = 0; i < numURLh; i++) {
            srv->setup[i] = setup[i];
        }

        if (params) {
            srv->timeout = params->timeout;
            srv->maxLineLen = params->maxLineLen;
            srv->maxURILen = params->maxURILen;
            srv->maxSessions = params->maxSessions;
        }
        else {
            srv->timeout = DEFAULT_TIMEOUT;
            srv->maxLineLen = DEFAULT_MAXLINELEN;
            srv->maxURILen = DEFAULT_MAXURILEN;
            srv->maxSessions = DEFAULT_MAXSESSIONS;
        }

        srv->stop = false;
        srv->isSecure = false;
        srv->sessions = NULL;
        srv->sendMq = (mqd_t) -1;
        srv->skt = -1;
        srv->secAttribs = NULL;
#ifdef __linux__
        srv->cmdfd = eventfd(0, 0);
#else
        srv->isPortSet = false;
#endif

#ifdef __NDK__
        fdOpenSession(Task_self());
#endif
    }

    Log_print1(Diags_EXIT, "HTTPServer_create> exit (%p)", (IArg)srv);
    return (srv);
}

void HTTPServer_delete(HTTPServer_Handle * srv)
{
    Session * session;
    Session * nextSession;

#ifdef __NDK__
    fdCloseSession(Task_self());
#endif

    if (srv && *srv) {
#ifdef __linux__
        close((*srv)->cmdfd);
#endif

        for (session = (*srv)->sessions; session; session = nextSession) {
            nextSession = session->next;
            deleteSession(*srv, session);
        }

        free(*srv);
        *srv = NULL;
    }
}

static void *serveSendThread(void *p)
{
    SendThreadArgs *args = p;
    HTTPServer_Handle srv = args->srv;
    char *mqName = args->mqName;
    Session *session;
    mqd_t recvMq;
    char data;

    Log_print1(Diags_ENTRY, "serveSendThread> enter (%p)", (IArg)p);

    recvMq = mq_open(mqName, O_RDONLY);
    if (recvMq == (mqd_t) -1) {
        Log_error1("serveSendThread> couldn't open recv MQ handle, errno %d",
                errno);
        return (NULL);
    }

    while (1)  {
        if (mq_receive(recvMq, (char *)&data, sizeof(data), NULL) == -1) {
            Log_error1("serveSendThread> failed to recv on MQ, errno %d",
                    errno);
            goto stopThread;
        }

        switch (data) {
            case EVENT_NOTIFY:
                Log_print0(Diags_ANALYSIS,
                        "serveSendThread> processing event notifications");

                if (pthread_mutex_lock(&(srv->sendMutex)) != 0) {
                    Log_error0("serveSendThread> failed to acquire lock");
                    goto stopThread;
                }

                for (session = srv->sessions; session;
                        session = session->next) {
                    if (session->sendRequest) {
/*
                        int i;
                        for (i = 0; i < srv->numURLh; i++) {
                            if (srv->setup[i].send) {
                                srv->setup[i].send(session->urlh[i],
                                        session->s);
                            }
                        }
*/
                        session->sendRequest--;
                    }
                }

                if (pthread_mutex_unlock(&(srv->sendMutex)) != 0) {
                    Log_error0("serveSendThread> failed to release lock");
                    goto stopThread;
                }

                break;

            case STOP_THREAD:
                Log_print0(Diags_ANALYSIS, "serveSendThread> stop thread");
                goto stopThread;

            default:
                break;
        }
    }

stopThread:
    mq_close(recvMq);

    return (NULL);
}

int HTTPServer_serveSelect(HTTPServer_Handle srv, const struct sockaddr * addr,
                        int len, int backlog)
{
    int sc;
    Session * session;
    Session * newSession;
    int status;
    fd_set fds;
    int maxs;
    bool prune = false;
    char mqName[16] = {0};
    struct mq_attr mqAttrs;
    pthread_t thread;
    pthread_attr_t attr;
    SendThreadArgs args;
    char data;

    Log_print4(Diags_ENTRY, "HTTPServer_serveSelect> enter (%p), (%p), %d, %d",
            (IArg)srv, (IArg)addr, len, backlog);

#ifdef __linux__
    snprintf(mqName, sizeof(mqName), "/hk%x", (unsigned int)getpid());
#else
    snprintf(mqName, sizeof(mqName), "/hk%x", (unsigned int)srv);
#endif

    if (pthread_mutex_init(&(srv->sendMutex), NULL) != 0) {
        Log_error0("serveSelect> failed to create mutex");

        return (HTTPServer_EMEMFAIL);
    }

    /* Open the reply message queue */
    mqAttrs.mq_flags = 0;
    mqAttrs.mq_curmsgs = 0;
    mqAttrs.mq_maxmsg = MQ_MAXMSG;
    mqAttrs.mq_msgsize = MQ_MSGSIZE;

#ifdef __linux__
    /* Check if older undeleted MQ exists (can happen in Linux) */
    mq_unlink(mqName);
#endif
    srv->sendMq = mq_open(mqName, O_CREAT | O_WRONLY, (mode_t) 0644, &mqAttrs);
    if (srv->sendMq == (mqd_t) -1) {
        Log_error1("serveSelect> couldn't open send MQ, errno %d", errno);
        status = HTTPServer_EMQFAIL;
        goto selectFail;
    }

    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, SERVESENDTHREAD_STACKSIZE);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    args.srv = srv;
    args.mqName = mqName;
    if (pthread_create(&thread, &attr, serveSendThread, (void *)&args) != 0) {
        Log_error0("serveSelect> failed to create serveSend thread");
        pthread_attr_destroy(&attr);
        status = HTTPServer_EMEMFAIL;
        goto selectFail;
    }
    pthread_attr_destroy(&attr);

    if ((srv->skt = socket(addr->sa_family, SOCK_STREAM, 0)) == -1) {
        status = HTTPServer_ESOCKETFAIL;
        goto selectFail;
    }

#ifdef __linux__
    int opt = 1;
    setsockopt(srv->skt, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#else
    srv->port = ((struct sockaddr_in *)addr)->sin_port;
    srv->isPortSet = true;
#endif

    if ((srv->secAttribs != NULL) && (srv->isSecure)) {
        status = SlNetSock_startSec(srv->skt, srv->secAttribs,
                                    SLNETSOCK_SEC_BIND_CONTEXT_ONLY |
                                    SLNETSOCK_SEC_IS_SERVER);
        if (status < 0) {
            goto selectFail;
        }
    }

    if ((status = bind(srv->skt, addr, len)) == -1) {
        status = HTTPServer_EBINDFAIL;
        goto selectFail;
    }

    if ((status = listen(srv->skt, backlog)) == -1) {
        status = HTTPServer_ELISTENFAIL;
        goto selectFail;
    }

    status = 0;
    while (1) {
        int result;

        FD_ZERO(&fds);
        FD_SET(srv->skt, &fds);
        maxs = srv->skt;
        prune = false;

#ifdef __linux__
        /* add command pipe file descriptor */
        FD_SET(srv->cmdfd, &fds);
        maxs = (srv->cmdfd > maxs ? srv->cmdfd : maxs);
#endif

        for (session = srv->sessions; session; session = session->next) {
            if (session->stop) {
                prune = true;
            }
            else {
                FD_SET(session->s, &fds);
                if (session->s > maxs) {
                    maxs = session->s;
                }
            }
        }

        /* TODO: add timeout support to close dormant clients */
        result = select(maxs + 1, &fds, NULL, NULL, NULL);

#ifdef __linux__
        /* check command pipe for data */
        if (FD_ISSET(srv->cmdfd, &fds)) {
            uint64_t event;
            read(srv->cmdfd, &event, sizeof(event)); /* just clear for now */
        }
#endif

        if (srv->stop || result < 0) {
            for (session = srv->sessions; session; session = session->next) {
                removeSession(srv, session);
            }
            pruneSession(srv);
            status = srv->stop ? 0 : -1;
            srv->stop = false;
            break;
        }

        if (FD_ISSET(srv->skt, &fds)) {
            sc = accept(srv->skt, NULL, NULL);
            if (sc == -1) {
                if (errno == ENFILE) {
                    Log_error0("Too many connections open, no sockets are "
                            "available to receive new connection.");
                }
                else {
                    status = HTTPServer_EACCEPTFAIL;
                    break;
                }
            }
            else {
                newSession = addSession(srv, sc);

                if (newSession) {
                    if ((srv->secAttribs != NULL) && (srv->isSecure)) {
                        /* Start the tls session between server and new client*/
                        status = SlNetSock_startSec(sc, srv->secAttribs,
                                    SLNETSOCK_SEC_START_SECURITY_SESSION_ONLY);
                        if (status < 0) {
                            removeSession(srv, newSession);
                            pruneSession(srv);
                        }
                    }
                } else {
                    closeSocket(sc);
                }
            }
        }

        if (pthread_mutex_lock(&(srv->sendMutex)) != 0) {
            status = HTTPServer_EMUTEXFAIL;
            break;
        }

        for (session = srv->sessions; session; session = session->next) {
            if (FD_ISSET(session->s, &fds)) {
                if (transact(srv, session)) {
                    Log_print1(Diags_ANALYSIS, "closing %p", session->s);
                    removeSession(srv, session);
                    prune = true;
                }
            }
        }

        if (pthread_mutex_unlock(&(srv->sendMutex)) != 0) {
            status = HTTPServer_EMUTEXFAIL;
            break;
        }

        if (srv->stop) {
            for (session = srv->sessions; session; session = session->next) {
                removeSession(srv, session);
            }
            pruneSession(srv);
            status = 0;
            srv->stop = false;
            break;
        }

        /* Check if any sessions has stopped if we haven't already */
        if (!prune) {
            for (session = srv->sessions; session; session = session->next) {
                if (session->stop) {
                    prune = true;
                    break;
                }
            }
        }

        if (prune) {
            pruneSession(srv);
        }
    }

selectFail:
    pthread_mutex_destroy(&(srv->sendMutex));

    if (status != HTTPServer_EMQFAIL) {
#ifndef __linux__
        srv->isPortSet = false;
#endif
        /* stop serveSendThread */
        data = STOP_THREAD;
        mq_send(srv->sendMq, &data, sizeof(data), 0);
        pthread_join(thread, NULL);

        /* clean up */
        mq_close(srv->sendMq);
        mq_unlink(mqName);

        if (srv->skt != -1) {
            closeSocket(srv->skt);
            srv->skt = -1;
        }
    }

    Log_print1(Diags_EXIT, "HTTPServer_serveSelect> exit (%d)", status);
    return (status);
}

int HTTPServer_processClient(HTTPServer_Handle srv, int sock)
{
    Session * session;
    int status = 0;

    Log_print2(Diags_ENTRY, "_processClient> enter (srv:%p, sock:%d)",
           (IArg)srv, (IArg)sock);

    session = createSession(srv, sock);
    if (session == NULL) {
        Log_error0("Failed to create session");
        status = -1;
        goto exit;
    }

    Log_print1(Diags_ANALYSIS, "processClient> start %p", (IArg)srv);

    while (transact(srv, session) == 0) {
    }

    Log_print1(Diags_ANALYSIS, "finish processClient> %p", (IArg)srv);

    deleteSession(srv, session);

exit:
    Log_print0(Diags_EXIT, "_processClient> exit");

    return (status);
}

bool HTTPServer_stop(HTTPServer_Handle srv, uint32_t timeout)
{
    srv->stop = true;
#ifndef __linux__
    int skt = -1;
    struct sockaddr_in inaddr;
    char buf[] = "stop";
#endif

    /* Wake up select */
#ifdef __linux__
    {
    uint64_t event = 1;
    write(srv->cmdfd, &event, sizeof(event));
    }
#else
    if (!srv->isPortSet) {
        goto stop_error;
    }
    inaddr.sin_family = AF_INET;
    inaddr.sin_port = srv->port;
    inaddr.sin_addr.s_addr = htonl(LOOPBACKADDR);
    if ((skt = socket(inaddr.sin_family, SOCK_STREAM, 0)) == -1) {
        goto stop_error;
    }

    if (connect(skt, (struct sockaddr *)&inaddr, sizeof(inaddr)) == -1) {
        goto stop_error;
    }

    send(skt, buf, sizeof(buf), 0);
#endif

    while (srv->stop && timeout > POLLPERIOD) {
        _HTTPServer_sleepms(POLLPERIOD);
        timeout -= POLLPERIOD;
    }

#ifndef __linux__
stop_error:
    if (skt != -1) {
        closeSocket(skt);
    }
#endif

    return (!srv->stop);
}

void HTTPServer_requestSend(URLHandler_Session urls)
{
    Session * session = (Session *)urls;
    char data = EVENT_NOTIFY;

    session->sendRequest++;

    if (session->srv->sendMq != (mqd_t) -1) {
        if (mq_send(session->srv->sendMq, &data, sizeof(data), 0) == -1) {
            Log_error0("HTTPServer_requestSend> serveSendThread signal failed");
        }
        else {
            Log_print0(Diags_ANALYSIS,
                    "HTTPServer_requestSend> signaled serveSendThread");
        }
    }
}

void HTTPServer_stopSession(URLHandler_Session urls)
{
    Log_print1(Diags_ENTRY, "HTTPServer_stopSession> enter (%p)", (IArg)urls);

    Session * session = (Session *)urls;
    session->stop = true;

    Log_print0(Diags_EXIT, "HTTPServer_stopSession> exit");
}

bool HTTPServer_isSessionSecure(URLHandler_Session sess)
{
    Session * session = (Session *)sess;

    return session->srv->isSecure;
}
