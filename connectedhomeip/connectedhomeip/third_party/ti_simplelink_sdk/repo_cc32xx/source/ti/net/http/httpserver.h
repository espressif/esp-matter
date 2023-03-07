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

/*
 * ======== ti/net/http/httpserver.h ========
 */
/**
 *  @file  ti/net/http/httpserver.h
 *
 *  @brief HTTP Server services
 */
/**
 *  @addtogroup ti_net_http_HTTPServer HTTP Server
 *
 *  @brief      The HTTP server provides APIs to instantiate an HTTP
 *              server, and handle requests from HTTP clients.
 *
 *  ## Library Usage ##
 *
 *  To use the HTTPServer APIs, the application should include its header file
 *  as follows:
 *  @code
 *  #include <ti/net/http/httpserver.h>
 *  @endcode
 *
 *  And, add the following HTTP library to the link line:
 *  @code
 *  .../source/ti/net/http/{toolchain}/{isa}/httpserver_{profile}.a
 *  @endcode
 *
 *  ============================================================================
 */

#ifndef ti_net_http_HTTPServer__include
#define ti_net_http_HTTPServer__include

/*! @ingroup ti_net_http_HTTPServer */
/*@{*/

#include <stdint.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <ti/net/slnetsock.h>

#include "urlhandler.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief Internal accept() call failed
 */
#define HTTPServer_EACCEPTFAIL   (-2)

/**
 *  @brief Internal network socket creation failure
 */
#define HTTPServer_ESOCKETFAIL   (-3)

/**
 *  @brief Internal bind() call failed
 */
#define HTTPServer_EBINDFAIL     (-4)

/**
 *  @brief Internal listen() call failed
 */
#define HTTPServer_ELISTENFAIL   (-5)

/**
 *  @brief Internal memory allocation or object creation failure
 */
#define HTTPServer_EMEMFAIL      (-6)

/**
 *  @brief Internal mq creation failure
 */
#define HTTPServer_EMQFAIL       (-7)

/**
 *  @brief Internal mutex-related failure
 */
#define HTTPServer_EMUTEXFAIL    (-8)

/** @cond INTERNAL */
/* internal utility fxn, not for end users */
extern void _HTTPServer_sleepms(uint32_t time);
/** @endcond */

/**
 *  @brief HTTPServer instance create parameters
 */
typedef struct HTTPServer_Params {
    /**
     *  @brief Receive timeout, in seconds
     */
    int timeout;
    /**
     *  @brief Maximum characters in a request header line
     *
     *  @remarks    If a client request includes a header with more
     *              characters than this, the server will send an error
     *              response of HTTP_TOO_BIG.
     */
    int maxLineLen;

    /**
     *  @brief Maximum characters in a URI request
     *
     *  @remarks    If a client requests a URI with more characters than
     *              this, the server will send an error response of
     *              HTTP_TOO_BIG.
     */
    int maxURILen;

    /**
     *  @brief Maximum number of active sessions
     *
     *  @remarks    If a new client arrives and there are currently
     *              @c maxSessions clients connected, the oldest
     *              connection will be dropped to make room for the
     *              new one.
     */
    int maxSessions;
} HTTPServer_Params;

/**
 *  @brief HTTPServer instance object handle
 */
typedef struct HTTPServer_Object * HTTPServer_Handle;

/**
 *  @brief Initialize the HTTPServer module
 *
 *  @remark     This function is used to initialize the HTTPServer module.
 *              Call this function before calling any other HTTPServer
 *              functions.
 *
 *  @remark     This function must be serialized by the caller.
 */
extern void HTTPServer_init(void);

/**
 *  @brief Initialize the instance create params structure
 *
 *  @param[in]  params  params structure to initialize
 */
extern void HTTPServer_Params_init(HTTPServer_Params *params);

/**
 *  @brief Attach security params to the created, but not yet initialized server
 *
 *  @param[in]  srv  Pointer to the server that will take on the attributes.
 *  @param[in]  securityAttributes A list of security objects as detailed in
 *                                 slnetsock.h.
 *  @param      beginSecurely Whether to activate security right away or not.
 *                            This is typically set to true.
 *
 *  @remark     The securityAttributes passed to this function must remain
 *              in memory for the duration of the program. They cannot be
 *              safely freed before the server has been deleted.
 *  @remark     Call this function before HTTPServer_serveSelect.
 *              beginSecurely is typically set to true as there is currently
 *              no way to activate security after this function has been called
 *              and returns.
 */
extern void HTTPServer_enableSecurity(HTTPServer_Handle srv,
        SlNetSockSecAttrib_t * securityAttributes, bool beginSecurely);

/**
 *  @brief Create an HTTPServer instance
 *
 *  @param[in]  urlh    Array of URLHandler setup descriptors
 *  @param      numURLh Number of elements in the @c urlh array
 *  @param[in]  params  Optional parameters to specify characteristics - use
 *                      NULL for defaults
 *
 *  @httpserver_init_precondition
 *
 *  @retval HTTPServer instance handle
 *  @retval NULL if unable to create the instance
 *
 *  @sa HTTPServer_Params_init()
 *  @sa HTTPServer_delete()
 */
extern HTTPServer_Handle HTTPServer_create(const URLHandler_Setup * urlh,
        int numURLh, HTTPServer_Params * params);

/**
 *  @brief Delete an HTTPServer instance
 *
 *  @param[in,out] srv    Pointer containing a handle to the instance to
 *                        delete.
 *
 *  @httpserver_init_precondition
 *
 *  @remarks    Upon successful return, the handle pointed to by @c handlePtr
 *              will be invalid.
 */
extern void HTTPServer_delete(HTTPServer_Handle * srv);

/** @cond INTERNAL */
/**
 *  @brief Process the HTTP transactions for a client socket
 *
 *  @param srv  server instance returned from HTTPServer_create
 *  @param s    TCP/IP socket connected to a client (returned from accept())
 *
 */
extern int HTTPServer_processClient(HTTPServer_Handle srv, int s);
/** @endcond */

/**
 *  @brief Send a simple, complete response to a client
 *
 *  This function is typically called by a URL Handler, in response to
 *  handling a URL request.
 *
 *  @param[in]  s       TCP/IP socket connected to a client
 *  @param[in]  status  Status Code associated with the response
 *  @param[in]  type    Content-type of the response
 *  @param[in]  len     Number of bytes in the response buffer @c buf
 *  @param[in]  buf     Response buffer
 *
 *  @httpserver_init_precondition
 *
 *  @remarks    This function sends 2 response headers, "Content-Length" and
 *              "Content-Type".  To send custom response headers, use
 *              HTTPServer_sendResponse().
 *
 *  @sa HTTPServer_sendErrorResponse()
 *  @sa HTTPServer_sendResponseChunked()
 */
extern void HTTPServer_sendSimpleResponse(int s, int status,
        const char * type, size_t len, const void * buf);

/**
 *  @brief Send a complete response to a client
 *
 *  This function is typically called by a URL Handler, in response to
 *  handling a URL request.
 *
 *  @param[in]  s       TCP/IP socket connected to a client
 *  @param[in]  status  Status Code associated with the response
 *  @param[in]  headers Optional response headers
 *  @param[in]  numHeaders  number of elements in @c headers
 *  @param[in]  len     Number of bytes in the response buffer @c buf
 *  @param[in]  buf     Response buffer
 *
 *  @httpserver_init_precondition
 *
 *  @remarks    This function sends the user supplied @c headers as
 *              response headers, followed by a generated
 *              "Content-Length" header (with a value of @c len).  If
 *              you only need to send "Content-Length" and
 *              "Content-Type" headers, consider using
 *              HTTPServer_sendSimpleResponse().
 *
 *  @sa HTTPServer_sendErrorResponse()
 *  @sa HTTPServer_sendResponseChunked()
 */
extern void HTTPServer_sendResponse(int s, int status,
        const char * headers[], int numHeaders, size_t len, const void * buf);

/**
 *  @brief Send an error response to a client
 *
 *  This function is typically called by a URL Handler, in response to
 *  handling a URL request.
 *
 *  @param[in]  s       TCP/IP socket connected to a client
 *  @param[in]  status  Status Code associated with the response
 *
 *  @httpserver_init_precondition
 *
 *  @sa HTTPServer_sendResponse()
 */
extern void HTTPServer_sendErrorResponse(int s, int status);

/**
 *  @brief Begin the process of sending a chunked response to a client
 *
 *  This function is typically called by a URL Handler, in response to
 *  handling a URL request.
 *
 *  @param[in]  s       TCP/IP socket connected to a client
 *  @param[in]  status  Status Code associated with the response
 *  @param[in]  type    Content-type of the response
 *
 *  @httpserver_init_precondition
 *
 *  @remarks    HTTPServer_sendResponseChunked() starts the process of
 *              sending a chunked reply.  It sends a line with the
 *              status field (e.g. "Status: 200 OK"), followed by the
 *              "Transfer-Encoding: chunked\r\n\r\n" field.  The rest
 *              of the response can then be sent using
 *              HTTPServer_sendChunk().
 *
 *  @sa HTTPServer_sendChunk()
 */
extern void HTTPServer_sendResponseChunked(int s, int status,
        const char * type);

/**
 *  @brief      Continue and complete the process of sending a chunked
 *              response to a client
 *
 *  This function is typically called by a URL Handler, in response to
 *  handling a URL request.
 *
 *  @param[in]  s       TCP/IP socket connected to a client
 *  @param[in]  buf     Response buffer
 *  @param[in]  len     Number of bytes in the response buffer @c buf
 *
 *  @httpserver_init_precondition
 *
 *  @remarks    HTTPServer_sendChunked() sends the requisite
 *              ASCII-encoded hex size of the data being sent (@c
 *              len), followed by the data in @c buf.
 *
 *  @remarks    To indicate the end of a chunked reply, call
 *              HTTPServer_sendChunk() with @c len set to zero.
 *
 *  @sa HTTPServer_sendChunkedResponse()
 */
extern void HTTPServer_sendChunk(int s, const void * buf, size_t len);

/**
 *  @brief Begin the HTTP Server's main processing loop.
 *
 *  @param[in]  srv     Handle to the server
 *  @param[in]  addr    Address information for server startup
 *  @param[in]  len     Length of the address information structure
 *  @param[in]  backlog Maximum number of pending connections to server
 *
 *  @retval     0       Server received stop command
 *  @retval     -1      Server shutdown unexpectedly
 *  @retval     <-1     See HTTPServer_E error codes
 *
 */
extern int HTTPServer_serveSelect(HTTPServer_Handle srv,
        const struct sockaddr *addr, int len, int backlog);

/**
 *  @brief Stop a currently running server
 *
 *  In order to stop the server using this function, this must be called
 *  from an outside thread.
 *
 *  @param[in]  srv     Handle to the server being stopped
 *  @param      timeout The time in which the server is expected to halt
 *
 */
extern bool HTTPServer_stop(HTTPServer_Handle srv, uint32_t timeout);

/** @cond INTERNAL */
/* internal services not quite ready to expose just yet */
extern int (*HTTPServer_errorResponseHook)(int s, int status);

extern void HTTPServer_requestSend(URLHandler_Session urls);

extern void HTTPServer_stopSession(URLHandler_Session urls);
/** @endcond */

/**
 *  @brief Obtain the session's security status.
 *
 *  @param      sess    A handle containing session state.
 *
 *  @sa HTTPServer_enableSecurity()
 */
extern bool HTTPServer_isSessionSecure(URLHandler_Session sess);

/*! @} */
#ifdef __cplusplus
}
#endif

#endif
