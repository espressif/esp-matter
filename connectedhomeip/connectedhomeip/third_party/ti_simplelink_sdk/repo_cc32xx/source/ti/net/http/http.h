/*
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
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
/**
 *  @file  ti/net/http/http.h
 *
 *  @addtogroup ti_net_http_HTTP HTTP definitions (both client and server)
 *
 *  @brief      HTTP definitions common to both clients and servers
 */

#ifndef ti_net_http_HTTP__include
#define ti_net_http_HTTP__include

/*! @ingroup ti_net_http_HTTP */
/*@{*/

#ifdef __cplusplus
extern "C" {
#endif

 /* HTTP Status Codes */
 enum HTTP_STATUS_CODE {
    HTTP_SC_CONTINUE                          = 100, /*!< Informational */
    HTTP_SC_SWITCHING_PROTOCOLS               = 101, /*!< Informational */

    HTTP_SC_OK                                = 200, /*!< Success */
    HTTP_SC_CREATED                           = 201, /*!< Success */
    HTTP_SC_ACCEPTED                          = 202, /*!< Success */
    HTTP_SC_NON_AUTHORITATIVE_INFORMATION     = 203, /*!< Success */
    HTTP_SC_NO_CONTENT                        = 204, /*!< Success */
    HTTP_SC_RESET_CONTENT                     = 205, /*!< Success */
    HTTP_SC_PARTIAL_CONTENT                   = 206, /*!< Success */
    HTTP_SC_MULTI_STATUS                      = 207, /*!< Success */

    HTTP_SC_MULTIPLE_CHOICES                  = 300, /*!< Redirection */
    HTTP_SC_MOVED_PERMANENTLY                 = 301, /*!< Redirection */
    HTTP_SC_FOUND                             = 302, /*!< Redirection */
    HTTP_SC_SEE_OTHER                         = 303, /*!< Redirection */
    HTTP_SC_NOT_MODIFIED                      = 304, /*!< Redirection */
    HTTP_SC_TEMPORARY_REDIRECT                = 307, /*!< Redirection */

    HTTP_SC_BAD_REQUEST                       = 400, /*!< Client Error */
    HTTP_SC_UNAUTHORIZED                      = 401, /*!< Client Error */
    HTTP_SC_PAYMENT_REQUIRED                  = 402, /*!< Client Error */
    HTTP_SC_FORBIDDEN                         = 403, /*!< Client Error */
    HTTP_SC_NOT_FOUND                         = 404, /*!< Client Error */
    HTTP_SC_METHOD_NOT_ALLOWED                = 405, /*!< Client Error */
    HTTP_SC_NOT_ACCEPTABLE                    = 406, /*!< Client Error */
    HTTP_SC_PROXY_AUTHENTICATION_REQUIRED     = 407, /*!< Client Error */
    HTTP_SC_REQUEST_TIMEOUT                   = 408, /*!< Client Error */
    HTTP_SC_CONFLICT                          = 409, /*!< Client Error */
    HTTP_SC_GONE                              = 410, /*!< Client Error */
    HTTP_SC_LENGTH_REQUIRED                   = 411, /*!< Client Error */
    HTTP_SC_PRECONDITION_FAILED               = 412, /*!< Client Error */
    HTTP_SC_REQUEST_ENTITY_TOO_LARGE          = 413, /*!< Client Error */
    HTTP_SC_REQUEST_URI_TOO_LONG              = 414, /*!< Client Error */
    HTTP_SC_UNSUPPORTED_MEDIA_TYPE            = 415, /*!< Client Error */
    HTTP_SC_REQUESTED_RANGE_NOT_SATISFAIABLE  = 416, /*!< Client Error */
    HTTP_SC_EXPECTATION_FAILED                = 417, /*!< Client Error */
    HTTP_SC_UNPROCESSABLE_ENTITY              = 422, /*!< Client Error */
    HTTP_SC_UPGRADE_REQUIRED                  = 426, /*!< Client Error */
    HTTP_SC_TOO_MANY_REQUESTS                 = 429, /*!< Client Error */

    HTTP_SC_INTERNAL_SERVER_ERROR             = 500, /*!< Server Error */
    HTTP_SC_NOT_IMPLEMENTED                   = 501, /*!< Server Error */
    HTTP_SC_BAD_GATEWAY                       = 502, /*!< Server Error */
    HTTP_SC_SERVICE_UNAVAILABLE               = 503, /*!< Server Error */
    HTTP_SC_GATEWAY_TIMEOUT                   = 504, /*!< Server Error */
    HTTP_SC_HTTP_VERSION_NOT_SUPPORTED        = 505, /*!< Server Error */

    HTTP_SC_STATUS_CODE_END                   = 600
};

/*! @} */
#ifdef __cplusplus
}
#endif

#endif
