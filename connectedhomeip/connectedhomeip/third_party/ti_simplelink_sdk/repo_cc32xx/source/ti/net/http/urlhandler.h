/*
 * Copyright (c) 2014-2018, Texas Instruments Incorporated
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
 * ======== ti/net/http/urlhandler.h ========
 */
/**
 *  @file  ti/net/http/urlhandler.h
 *
 *  @brief URL Handler interface
 */
/**
 *  @addtogroup ti_net_http_HTTPServer HTTP Server
 *
 */

#ifndef ti_net_http__URLHandler__include
#define ti_net_http__URLHandler__include

/*! @ingroup ti_net_http_HTTPServer */
/*@{*/

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define URLHandler_GET 1
#define URLHandler_POST 2
#define URLHandler_PUT 3
#define URLHandler_PATCH 4
#define URLHandler_DELETE 5

#define URLHandler_ENOTHANDLED 0
#define URLHandler_EHANDLED 1
#define URLHandler_EERRORHANDLED 2
#define URLHandler_EHANDLEDSTOP 3

/**
 *  @brief A placeholder used to refer to a user-defined type.
 *
 *  This object handle is the type returned from the
 *  URLHandler_CreateFxn and passed to all other URLHandler_*
 *  functions.
 *
 */
typedef struct URLHandler_Object * URLHandler_Handle;
typedef struct URLHandler_State * URLHandler_Session;

/**
 *  @brief Create a user-defined URLHandler_Object
 *
 *  This function (if used) should be used to instantiate a
 *  user-defined #URLHandler_Handle object that contains whatever
 *  information is deemed necessary to pass to other URLHandler_*
 *  functions while the server is running. The handle of the user-defined
 *  type should be cast as a #URLHandler_Handle before returning it from this
 *  function. The handle is later passed to other URLHandler_* functions in
 *  order to access session-specific data. The handle should be recast as
 *  the user-defined type before accessing it in other URLHandler_* functions.
 *
 *  @param[in]  params  Optional parameters to specify characteristics
 *  @param[in]  session Handler to current client session
 *
 *  @remarks    The @c session argument is not yet used, but may be in the
 *              future
 *
 *  @retval     URLHandler instance handle
 *
 *  @sa URLHandler_ProcessFxn()
 *  @sa URLHandler_ScanFieldFxn()
 *  @sa URLHandler_DeleteFxn()
 */
typedef URLHandler_Handle (*URLHandler_CreateFxn)(void * params,
        URLHandler_Session session);

/**
 *  @brief Process an HTTP request
 *
 *  This function is called after an HTTP
 *  request is received and the request headers have been processed,
 *  possibly by calling the URLHandler_ScanFieldFxn(). For every request
 *  received by the server this method is called once per URLHandler until
 *  a URLHandler handles the request.
 *
 *  The body of a request can be retrieved by calling recv() with the
 *  supplied socket @c s argument. The length of the request body is
 *  available in the @c contentLength parameter. Given the HTTP method,
 *  URL, and request body, this function can determine how to proceed.
 *  i.e. if a GET request is received, this is where the resource identified
 *  by the URL should be fetched; If a POST request is made, this is where
 *  the data passed within @c urlArgs or the request body can be parsed and
 *  acted upon (i.e. stored within the server); etc.
 *
 *  After processing a request, this function should construct a response.
 *  This involves selecting a status code (see the @c HTTP_STATUS_CODE enum
 *  in ti/net/http/http.h), creating any desired headers, and creating the
 *  response body.
 *
 *  Once the components of the response have been created, they can be sent
 *  to the client via the recommended HTTPServer_send* methods. Note that
 *  users can directly send a response using send(), however the user is then
 *  responsible for creating well-formed headers and responses.
 *
 *  The @c u argument contains the #URLHandler_Handle that was returned from
 *  URLHandler_CreateFxn(), or NULL if the handler didn't provide one.
 *
 *  The return value of this function determines how the server proceeds:
 *
 *  - If URLHandler_ENOTHANDLED is returned and there are more URLHandlers
 *    whose process methods have not been called, the server will proceed to
 *    call said process methods, one by one, until the request is handled or
 *    until there are no more URLHandler->process methods available.
 *  - If URLHandler_ENOTHANDLED is returned and no URLHandlers are able to
 *    handle the request, a 404 response will be sent to the client.
 *  - If URLHandler_EHANDLEDSTOP is returned, the server will remove all open
 *    sessions, close its listening socket, and shut down.
 *  - If URLHandler_EERRORHANDLED is returned, the current session (client
 *    connection) is closed and the server continues.
 *
 *  @param[in]  u               Handle to the URL Handler containing relevant
 *                              data
 *  @param[in]  method          HTTP method of the request being parsed
 *  @param[in]  url             URI of the current request
 *  @param[in]  urlArgs         The query string, if present
 *  @param[in]  contentLength   Content-Length (body length) header value, if
 *                              present
 *  @param[in]  s               TCP/IP socket connected to a client
 *
 *  @retval     int             Return status
 *
 *  @sa HTTPServer_sendSimpleResponse()
 *  @sa URLHandler_ScanFieldFxn()
 */
typedef int (*URLHandler_ProcessFxn)(URLHandler_Handle u, int method,
        const char * url, const char * urlArgs, int contentLength, int s);

/**
 *  @brief Scan for specific request headers
 *
 *  This function can be used to process headers of incoming requests.
 *  Every URL Handler with this function defined will have each header
 *  passed into this function, one by one. It is called when a request
 *  is received by the server, before the request is sent to the
 *  URLHandler_ProcessFxn. The *Content-Length* header is parsed
 *  automatically by the server (however, it will still be passed into
 *  this function, should the developer want to examine it).
 *
 *  @param[in]  u       Handle to the URL Handler containing relevant data
 *  @param[in]  method  HTTP method of the request being parsed
 *  @param[in]  url     URI of the current request
 *  @param[in]  field   Specific request line containing the header
 *
 *  @sa URLHandler_ProcessFxn()
 */
typedef void (*URLHandler_ScanFieldFxn)(URLHandler_Handle u, int method,
        const char * url, const char * field);

/**
 *  @brief Delete a URLHandler
 *
 *  This function is called when the session associated with the
 *  handler parameter is closed. Any memory allocated for the URLHandler
 *  (i.e. in the _create function) should be cleaned up here.
 *
 *  @param[in]  u   The Handler for deletion
 *
 *  @sa URLHandler_CreateFxn()
 */
typedef void (*URLHandler_DeleteFxn)(URLHandler_Handle * u);

/** @cond INTERNAL
 *  @brief Send an event notification from the server
 *
 *  This function can be used to alert clients of events and send
 *  out status updates from the server, possibly based on state data
 *  contained within the given URL Handler. This function is triggered
 *  by HTTPServer_requestSend
 *
 *  @param[in]  u   A handle to the URL Handler containing relevant data
 *  @param[in]  s   TCP/IP socket connected to a client
 *
 */
typedef void (*URLHandler_SendFxn)(URLHandler_Handle u, int s);
/** @endcond */

/**
 *  @brief Structure containing URL Handler components
 *
 *  This structure contains parameters needed for setup of a user-
 *  defined URLHandler object and pointers to associated user-defined
 *  URLHandler_* functions. The created HTTP Server maintains a table
 *  of this data structure for each unique URL Handler.
 *
 */
typedef struct URLHandler_Setup {
    /**
     *  @brief Parameters needed for URL Handler instantiation
     */
    void * params;

    /**
     *  @brief URL Handler Create function
     */
    URLHandler_CreateFxn create;

    /**
     *  @brief URL Handler Delete function
     */
    URLHandler_DeleteFxn del;

    /**
     *  @brief URL Handler Process function
     */
    URLHandler_ProcessFxn process;

    /**
     *  @brief Scan field function
     */
    URLHandler_ScanFieldFxn scanField;

    /**
     *  @brief This field is reserved - set to NULL
     */
    void * reserved1;
} URLHandler_Setup;

/*! @} */
#ifdef __cplusplus
}
#endif

#endif
