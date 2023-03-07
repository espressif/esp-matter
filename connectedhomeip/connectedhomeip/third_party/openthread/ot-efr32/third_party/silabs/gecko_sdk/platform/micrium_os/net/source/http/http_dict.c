/***************************************************************************//**
 * @file
 * @brief Network - HTTP Dictionary
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_HTTP_SERVER_AVAIL) \
  || defined(RTOS_MODULE_NET_HTTP_CLIENT_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error HTTP Module requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "http_dict_priv.h"
#include  "http_priv.h"

#include  <net/include/http.h>

#include  <common/include/lib_str.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               HTTP METHOD
 *******************************************************************************************************/

const HTTP_DICT HTTP_Dict_ReqMethod[] = {
  { HTTP_METHOD_GET, HTTP_STR_METHOD_GET, (sizeof(HTTP_STR_METHOD_GET)     - 1) },
  { HTTP_METHOD_POST, HTTP_STR_METHOD_POST, (sizeof(HTTP_STR_METHOD_POST)    - 1) },
  { HTTP_METHOD_HEAD, HTTP_STR_METHOD_HEAD, (sizeof(HTTP_STR_METHOD_HEAD)    - 1) },
  { HTTP_METHOD_PUT, HTTP_STR_METHOD_PUT, (sizeof(HTTP_STR_METHOD_PUT)     - 1) },
  { HTTP_METHOD_DELETE, HTTP_STR_METHOD_DELETE, (sizeof(HTTP_STR_METHOD_DELETE)  - 1) },
  { HTTP_METHOD_TRACE, HTTP_STR_METHOD_TRACE, (sizeof(HTTP_STR_METHOD_TRACE)   - 1) },
  { HTTP_METHOD_CONNECT, HTTP_STR_METHOD_CONNECT, (sizeof(HTTP_STR_METHOD_CONNECT) - 1) }
};

CPU_SIZE_T HTTP_Dict_ReqMethodSize = sizeof(HTTP_Dict_ReqMethod);

/********************************************************************************************************
 *                                               HTTP VERSION
 *******************************************************************************************************/

const HTTP_DICT HTTP_Dict_ProtocolVer[] = {
  { HTTP_PROTOCOL_VER_0_9, HTTP_STR_PROTOCOL_VER_0_9, (sizeof(HTTP_STR_PROTOCOL_VER_0_9) - 1) },
  { HTTP_PROTOCOL_VER_1_0, HTTP_STR_PROTOCOL_VER_1_0, (sizeof(HTTP_STR_PROTOCOL_VER_1_0) - 1) },
  { HTTP_PROTOCOL_VER_1_1, HTTP_STR_PROTOCOL_VER_1_1, (sizeof(HTTP_STR_PROTOCOL_VER_1_1) - 1) }
};

CPU_SIZE_T HTTP_Dict_ProtocolVerSize = sizeof(HTTP_Dict_ProtocolVer);

/********************************************************************************************************
 *                                           HTTP STATUS CODE
 *******************************************************************************************************/

const HTTP_DICT HTTP_Dict_StatusCode[] = {
  { HTTP_STATUS_OK, HTTP_STR_STATUS_CODE_OK, (sizeof(HTTP_STR_STATUS_CODE_OK)                              - 1) },
  { HTTP_STATUS_CREATED, HTTP_STR_STATUS_CODE_CREATED, (sizeof(HTTP_STR_STATUS_CODE_CREATED)                         - 1) },
  { HTTP_STATUS_ACCEPTED, HTTP_STR_STATUS_CODE_ACCEPTED, (sizeof(HTTP_STR_STATUS_CODE_ACCEPTED)                        - 1) },
  { HTTP_STATUS_NO_CONTENT, HTTP_STR_STATUS_CODE_NO_CONTENT, (sizeof(HTTP_STR_STATUS_CODE_NO_CONTENT)                      - 1) },
  { HTTP_STATUS_RESET_CONTENT, HTTP_STR_STATUS_CODE_RESET_CONTENT, (sizeof(HTTP_STR_STATUS_CODE_RESET_CONTENT)                   - 1) },
  { HTTP_STATUS_MOVED_PERMANENTLY, HTTP_STR_STATUS_CODE_MOVED_PERMANENTLY, (sizeof(HTTP_STR_STATUS_CODE_MOVED_PERMANENTLY)               - 1) },
  { HTTP_STATUS_FOUND, HTTP_STR_STATUS_CODE_FOUND, (sizeof(HTTP_STR_STATUS_CODE_FOUND)                           - 1) },
  { HTTP_STATUS_SEE_OTHER, HTTP_STR_STATUS_CODE_SEE_OTHER, (sizeof(HTTP_STR_STATUS_CODE_SEE_OTHER)                       - 1) },
  { HTTP_STATUS_NOT_MODIFIED, HTTP_STR_STATUS_CODE_NOT_MODIFIED, (sizeof(HTTP_STR_STATUS_CODE_NOT_MODIFIED)                    - 1) },
  { HTTP_STATUS_USE_PROXY, HTTP_STR_STATUS_CODE_USE_PROXY, (sizeof(HTTP_STR_STATUS_CODE_USE_PROXY)                       - 1) },
  { HTTP_STATUS_TEMPORARY_REDIRECT, HTTP_STR_STATUS_CODE_TEMPORARY_REDIRECT, (sizeof(HTTP_STR_STATUS_CODE_TEMPORARY_REDIRECT)              - 1) },
  { HTTP_STATUS_BAD_REQUEST, HTTP_STR_STATUS_CODE_BAD_REQUEST, (sizeof(HTTP_STR_STATUS_CODE_BAD_REQUEST)                     - 1) },
  { HTTP_STATUS_UNAUTHORIZED, HTTP_STR_STATUS_CODE_UNAUTHORIZED, (sizeof(HTTP_STR_STATUS_CODE_UNAUTHORIZED)                    - 1) },
  { HTTP_STATUS_FORBIDDEN, HTTP_STR_STATUS_CODE_FORBIDDEN, (sizeof(HTTP_STR_STATUS_CODE_FORBIDDEN)                       - 1) },
  { HTTP_STATUS_NOT_FOUND, HTTP_STR_STATUS_CODE_NOT_FOUND, (sizeof(HTTP_STR_STATUS_CODE_NOT_FOUND)                       - 1) },
  { HTTP_STATUS_METHOD_NOT_ALLOWED, HTTP_STR_STATUS_CODE_METHOD_NOT_ALLOWED, (sizeof(HTTP_STR_STATUS_CODE_METHOD_NOT_ALLOWED)              - 1) },
  { HTTP_STATUS_NOT_ACCEPTABLE, HTTP_STR_STATUS_CODE_NOT_ACCEPTABLE, (sizeof(HTTP_STR_STATUS_CODE_NOT_ACCEPTABLE)                  - 1) },
  { HTTP_STATUS_REQUEST_TIMEOUT, HTTP_STR_STATUS_CODE_REQUEST_TIMEOUT, (sizeof(HTTP_STR_STATUS_CODE_REQUEST_TIMEOUT)                 - 1) },
  { HTTP_STATUS_CONFLICT, HTTP_STR_STATUS_CODE_CONFLIT, (sizeof(HTTP_STR_STATUS_CODE_CONFLIT)                         - 1) },
  { HTTP_STATUS_GONE, HTTP_STR_STATUS_CODE_GONE, (sizeof(HTTP_STR_STATUS_CODE_GONE)                            - 1) },
  { HTTP_STATUS_LENGTH_REQUIRED, HTTP_STR_STATUS_CODE_LENGTH_REQUIRED, (sizeof(HTTP_STR_STATUS_CODE_LENGTH_REQUIRED)                 - 1) },
  { HTTP_STATUS_PRECONDITION_FAILED, HTTP_STR_STATUS_CODE_PRECONDITION_FAILED, (sizeof(HTTP_STR_STATUS_CODE_PRECONDITION_FAILED)             - 1) },
  { HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE, HTTP_STR_STATUS_CODE_REQUEST_ENTITY_TOO_LARGE, (sizeof(HTTP_STR_STATUS_CODE_REQUEST_ENTITY_TOO_LARGE)        - 1) },
  { HTTP_STATUS_REQUEST_URI_TOO_LONG, HTTP_STR_STATUS_CODE_REQUEST_URI_TOO_LONG, (sizeof(HTTP_STR_STATUS_CODE_REQUEST_URI_TOO_LONG)            - 1) },
  { HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE, HTTP_STR_STATUS_CODE_UNSUPPORTED_MEDIA_TYPE, (sizeof(HTTP_STR_STATUS_CODE_UNSUPPORTED_MEDIA_TYPE)          - 1) },
  { HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE, HTTP_STR_STATUS_CODE_REQUESTED_RANGE_NOT_SATISFIABLE, (sizeof(HTTP_STR_STATUS_CODE_REQUESTED_RANGE_NOT_SATISFIABLE) - 1) },
  { HTTP_STATUS_EXPECTATION_FAILED, HTTP_STR_STATUS_CODE_EXPECTATION_FAILED, (sizeof(HTTP_STR_STATUS_CODE_EXPECTATION_FAILED)              - 1) },
  { HTTP_STATUS_INTERNAL_SERVER_ERR, HTTP_STR_STATUS_CODE_INTERNAL_SERVER_ERR, (sizeof(HTTP_STR_STATUS_CODE_INTERNAL_SERVER_ERR)             - 1) },
  { HTTP_STATUS_NOT_IMPLEMENTED, HTTP_STR_STATUS_CODE_NOT_IMPLEMENTED, (sizeof(HTTP_STR_STATUS_CODE_NOT_IMPLEMENTED)                 - 1) },
  { HTTP_STATUS_SERVICE_UNAVAILABLE, HTTP_STR_STATUS_CODE_SERVICE_UNAVAILABLE, (sizeof(HTTP_STR_STATUS_CODE_SERVICE_UNAVAILABLE)             - 1) },
  { HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED, HTTP_STR_STATUS_CODE_HTTP_VERSION_NOT_SUPPORTED, (sizeof(HTTP_STR_STATUS_CODE_HTTP_VERSION_NOT_SUPPORTED)      - 1) },
  { HTTP_STATUS_SWITCHING_PROTOCOLS, HTTP_STR_STATUS_CODE_SWITCHING_PROTOCOLS, (sizeof(HTTP_STR_STATUS_CODE_SWITCHING_PROTOCOLS)             - 1) }
};

CPU_SIZE_T HTTP_Dict_StatusCodeSize = sizeof(HTTP_Dict_StatusCode);

/********************************************************************************************************
 *                                           HTTP REASON PHRASE
 *******************************************************************************************************/

const HTTP_DICT HTTP_Dict_ReasonPhrase[] = {
  { HTTP_STATUS_OK, HTTP_STR_REASON_PHRASE_OK, (sizeof(HTTP_STR_REASON_PHRASE_OK)                              - 1) },
  { HTTP_STATUS_CREATED, HTTP_STR_REASON_PHRASE_CREATED, (sizeof(HTTP_STR_REASON_PHRASE_CREATED)                         - 1) },
  { HTTP_STATUS_ACCEPTED, HTTP_STR_REASON_PHRASE_ACCEPTED, (sizeof(HTTP_STR_REASON_PHRASE_ACCEPTED)                        - 1) },
  { HTTP_STATUS_NO_CONTENT, HTTP_STR_REASON_PHRASE_NO_CONTENT, (sizeof(HTTP_STR_REASON_PHRASE_NO_CONTENT)                      - 1) },
  { HTTP_STATUS_RESET_CONTENT, HTTP_STR_REASON_PHRASE_RESET_CONTENT, (sizeof(HTTP_STR_REASON_PHRASE_RESET_CONTENT)                   - 1) },
  { HTTP_STATUS_MOVED_PERMANENTLY, HTTP_STR_REASON_PHRASE_MOVED_PERMANENTLY, (sizeof(HTTP_STR_REASON_PHRASE_MOVED_PERMANENTLY)               - 1) },
  { HTTP_STATUS_FOUND, HTTP_STR_REASON_PHRASE_FOUND, (sizeof(HTTP_STR_REASON_PHRASE_FOUND)                           - 1) },
  { HTTP_STATUS_SEE_OTHER, HTTP_STR_REASON_PHRASE_SEE_OTHER, (sizeof(HTTP_STR_REASON_PHRASE_SEE_OTHER)                       - 1) },
  { HTTP_STATUS_NOT_MODIFIED, HTTP_STR_REASON_PHRASE_NOT_MODIFIED, (sizeof(HTTP_STR_REASON_PHRASE_NOT_MODIFIED)                    - 1) },
  { HTTP_STATUS_USE_PROXY, HTTP_STR_REASON_PHRASE_USE_PROXY, (sizeof(HTTP_STR_REASON_PHRASE_USE_PROXY)                       - 1) },
  { HTTP_STATUS_TEMPORARY_REDIRECT, HTTP_STR_REASON_PHRASE_TEMPORARY_REDIRECT, (sizeof(HTTP_STR_REASON_PHRASE_TEMPORARY_REDIRECT)              - 1) },
  { HTTP_STATUS_BAD_REQUEST, HTTP_STR_REASON_PHRASE_BAD_REQUEST, (sizeof(HTTP_STR_REASON_PHRASE_BAD_REQUEST)                     - 1) },
  { HTTP_STATUS_UNAUTHORIZED, HTTP_STR_REASON_PHRASE_UNAUTHORIZED, (sizeof(HTTP_STR_REASON_PHRASE_UNAUTHORIZED)                    - 1) },
  { HTTP_STATUS_FORBIDDEN, HTTP_STR_REASON_PHRASE_FORBIDDEN, (sizeof(HTTP_STR_REASON_PHRASE_FORBIDDEN)                       - 1) },
  { HTTP_STATUS_NOT_FOUND, HTTP_STR_REASON_PHRASE_NOT_FOUND, (sizeof(HTTP_STR_REASON_PHRASE_NOT_FOUND)                       - 1) },
  { HTTP_STATUS_METHOD_NOT_ALLOWED, HTTP_STR_REASON_PHRASE_METHOD_NOT_ALLOWED, (sizeof(HTTP_STR_REASON_PHRASE_METHOD_NOT_ALLOWED)              - 1) },
  { HTTP_STATUS_NOT_ACCEPTABLE, HTTP_STR_REASON_PHRASE_NOT_ACCEPTABLE, (sizeof(HTTP_STR_REASON_PHRASE_NOT_ACCEPTABLE)                  - 1) },
  { HTTP_STATUS_REQUEST_TIMEOUT, HTTP_STR_REASON_PHRASE_REQUEST_TIMEOUT, (sizeof(HTTP_STR_REASON_PHRASE_REQUEST_TIMEOUT)                 - 1) },
  { HTTP_STATUS_CONFLICT, HTTP_STR_REASON_PHRASE_CONFLICT, (sizeof(HTTP_STR_REASON_PHRASE_CONFLICT)                        - 1) },
  { HTTP_STATUS_GONE, HTTP_STR_REASON_PHRASE_GONE, (sizeof(HTTP_STR_REASON_PHRASE_GONE)                            - 1) },
  { HTTP_STATUS_LENGTH_REQUIRED, HTTP_STR_REASON_PHRASE_LENGTH_REQUIRED, (sizeof(HTTP_STR_REASON_PHRASE_LENGTH_REQUIRED)                 - 1) },
  { HTTP_STATUS_PRECONDITION_FAILED, HTTP_STR_REASON_PHRASE_PRECONDITION_FAILED, (sizeof(HTTP_STR_REASON_PHRASE_PRECONDITION_FAILED)             - 1) },
  { HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE, HTTP_STR_REASON_PHRASE_REQUEST_ENTITY_TOO_LARGE, (sizeof(HTTP_STR_REASON_PHRASE_REQUEST_ENTITY_TOO_LARGE)        - 1) },
  { HTTP_STATUS_REQUEST_URI_TOO_LONG, HTTP_STR_REASON_PHRASE_REQUEST_URI_TOO_LONG, (sizeof(HTTP_STR_REASON_PHRASE_REQUEST_URI_TOO_LONG)            - 1) },
  { HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE, HTTP_STR_REASON_PHRASE_UNSUPPORTED_MEDIA_TYPE, (sizeof(HTTP_STR_REASON_PHRASE_UNSUPPORTED_MEDIA_TYPE)          - 1) },
  { HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE, HTTP_STR_REASON_PHRASE_REQUESTED_RANGE_NOT_SATISFIABLE, (sizeof(HTTP_STR_REASON_PHRASE_REQUESTED_RANGE_NOT_SATISFIABLE) - 1) },
  { HTTP_STATUS_EXPECTATION_FAILED, HTTP_STR_REASON_PHRASE_EXPECTATION_FAILED, (sizeof(HTTP_STR_REASON_PHRASE_EXPECTATION_FAILED)              - 1) },
  { HTTP_STATUS_INTERNAL_SERVER_ERR, HTTP_STR_REASON_PHRASE_INTERNAL_SERVER_ERR, (sizeof(HTTP_STR_REASON_PHRASE_INTERNAL_SERVER_ERR)             - 1) },
  { HTTP_STATUS_NOT_IMPLEMENTED, HTTP_STR_REASON_PHRASE_NOT_IMPLEMENTED, (sizeof(HTTP_STR_REASON_PHRASE_NOT_IMPLEMENTED)                 - 1) },
  { HTTP_STATUS_SERVICE_UNAVAILABLE, HTTP_STR_REASON_PHRASE_SERVICE_UNAVAILABLE, (sizeof(HTTP_STR_REASON_PHRASE_SERVICE_UNAVAILABLE)             - 1) },
  { HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED, HTTP_STR_REASON_PHRASE_HTTP_VERSION_NOT_SUPPORTED, (sizeof(HTTP_STR_REASON_PHRASE_HTTP_VERSION_NOT_SUPPORTED)      - 1) },
  { HTTP_STATUS_SWITCHING_PROTOCOLS, HTTP_STR_REASON_PHRASE_SWITCHING_PROTOCOLS, (sizeof(HTTP_STR_REASON_PHRASE_SWITCHING_PROTOCOLS)             - 1) }
};

CPU_SIZE_T HTTP_Dict_ReasonPhraseSize = sizeof(HTTP_Dict_ReasonPhrase);

/********************************************************************************************************
 *                                           HTTP FILE EXTENSION
 *******************************************************************************************************/

const HTTP_DICT HTTP_Dict_FileExt[] = {
  { HTTP_CONTENT_TYPE_HTML, HTTP_STR_FILE_EXT_HTM, (sizeof(HTTP_STR_FILE_EXT_HTM)      - 1) },
  { HTTP_CONTENT_TYPE_HTML, HTTP_STR_FILE_EXT_HTML, (sizeof(HTTP_STR_FILE_EXT_HTML)     - 1) },
  { HTTP_CONTENT_TYPE_GIF, HTTP_STR_FILE_EXT_GIF, (sizeof(HTTP_STR_FILE_EXT_GIF)      - 1) },
  { HTTP_CONTENT_TYPE_JPEG, HTTP_STR_FILE_EXT_JPEG, (sizeof(HTTP_STR_FILE_EXT_JPEG)     - 1) },
  { HTTP_CONTENT_TYPE_JPEG, HTTP_STR_FILE_EXT_JPG, (sizeof(HTTP_STR_FILE_EXT_JPG)      - 1) },
  { HTTP_CONTENT_TYPE_PNG, HTTP_STR_FILE_EXT_PNG, (sizeof(HTTP_STR_FILE_EXT_PNG)      - 1) },
  { HTTP_CONTENT_TYPE_JS, HTTP_STR_FILE_EXT_JS, (sizeof(HTTP_STR_FILE_EXT_JS)       - 1) },
  { HTTP_CONTENT_TYPE_PLAIN, HTTP_STR_FILE_EXT_TXT, (sizeof(HTTP_STR_FILE_EXT_TXT)      - 1) },
  { HTTP_CONTENT_TYPE_CSS, HTTP_STR_FILE_EXT_CSS, (sizeof(HTTP_STR_FILE_EXT_CSS)      - 1) },
  { HTTP_CONTENT_TYPE_PDF, HTTP_STR_FILE_EXT_PDF, (sizeof(HTTP_STR_FILE_EXT_PDF)      - 1) },
  { HTTP_CONTENT_TYPE_ZIP, HTTP_STR_FILE_EXT_ZIP, (sizeof(HTTP_STR_FILE_EXT_ZIP)      - 1) },
  { HTTP_CONTENT_TYPE_OCTET_STREAM, HTTP_STR_FILE_EXT_ASTERISK, (sizeof(HTTP_STR_FILE_EXT_ASTERISK) - 1) },
  { HTTP_CONTENT_TYPE_OCTET_STREAM, HTTP_STR_FILE_EXT_CLASS, (sizeof(HTTP_STR_FILE_EXT_CLASS)    - 1) }
};

CPU_SIZE_T HTTP_Dict_FileExtSize = sizeof(HTTP_Dict_FileExt);

/********************************************************************************************************
 *                                           HTTP MIME CONTENT TYPE
 *******************************************************************************************************/

const HTTP_DICT HTTP_Dict_ContentType[] = {
  { HTTP_CONTENT_TYPE_HTML, HTTP_STR_CONTENT_TYPE_HTML, (sizeof(HTTP_STR_CONTENT_TYPE_HTML)           - 1) },
  { HTTP_CONTENT_TYPE_GIF, HTTP_STR_CONTENT_TYPE_GIF, (sizeof(HTTP_STR_CONTENT_TYPE_GIF)            - 1) },
  { HTTP_CONTENT_TYPE_JPEG, HTTP_STR_CONTENT_TYPE_JPEG, (sizeof(HTTP_STR_CONTENT_TYPE_JPEG)           - 1) },
  { HTTP_CONTENT_TYPE_PNG, HTTP_STR_CONTENT_TYPE_PNG, (sizeof(HTTP_STR_CONTENT_TYPE_PNG)            - 1) },
  { HTTP_CONTENT_TYPE_JS, HTTP_STR_CONTENT_TYPE_JS, (sizeof(HTTP_STR_CONTENT_TYPE_JS)             - 1) },
  { HTTP_CONTENT_TYPE_PLAIN, HTTP_STR_CONTENT_TYPE_PLAIN, (sizeof(HTTP_STR_CONTENT_TYPE_PLAIN)          - 1) },
  { HTTP_CONTENT_TYPE_CSS, HTTP_STR_CONTENT_TYPE_CSS, (sizeof(HTTP_STR_CONTENT_TYPE_CSS)            - 1) },
  { HTTP_CONTENT_TYPE_OCTET_STREAM, HTTP_STR_CONTENT_TYPE_OCTET_STREAM, (sizeof(HTTP_STR_CONTENT_TYPE_OCTET_STREAM)   - 1) },
  { HTTP_CONTENT_TYPE_PDF, HTTP_STR_CONTENT_TYPE_PDF, (sizeof(HTTP_STR_CONTENT_TYPE_PDF)            - 1) },
  { HTTP_CONTENT_TYPE_ZIP, HTTP_STR_CONTENT_TYPE_ZIP, (sizeof(HTTP_STR_CONTENT_TYPE_ZIP)            - 1) },
  { HTTP_CONTENT_TYPE_JSON, HTTP_STR_CONTENT_TYPE_JSON, (sizeof(HTTP_STR_CONTENT_TYPE_JSON)           - 1) },
  { HTTP_CONTENT_TYPE_APP_FORM, HTTP_STR_CONTENT_TYPE_APP_FORM, (sizeof(HTTP_STR_CONTENT_TYPE_APP_FORM)       - 1) },
  { HTTP_CONTENT_TYPE_MULTIPART_FORM, HTTP_STR_CONTENT_TYPE_MULTIPART_FORM, (sizeof(HTTP_STR_CONTENT_TYPE_MULTIPART_FORM) - 1) }
};

CPU_SIZE_T HTTP_Dict_ContentTypeSize = sizeof(HTTP_Dict_ContentType);

/********************************************************************************************************
 *                                           HTTP HEADER FIELD
 *******************************************************************************************************/

const HTTP_DICT HTTP_Dict_HdrField[] = {
  { HTTP_HDR_FIELD_CONTENT_TYPE, HTTP_STR_HDR_FIELD_CONTENT_TYPE, (sizeof(HTTP_STR_HDR_FIELD_CONTENT_TYPE)        - 1) },
  { HTTP_HDR_FIELD_CONTENT_LEN, HTTP_STR_HDR_FIELD_CONTENT_LEN, (sizeof(HTTP_STR_HDR_FIELD_CONTENT_LEN)         - 1) },
  { HTTP_HDR_FIELD_CONTENT_DISPOSITION, HTTP_STR_HDR_FIELD_CONTENT_DISPOSITION, (sizeof(HTTP_STR_HDR_FIELD_CONTENT_DISPOSITION) - 1) },
  { HTTP_HDR_FIELD_HOST, HTTP_STR_HDR_FIELD_HOST, (sizeof(HTTP_STR_HDR_FIELD_HOST)                - 1) },
  { HTTP_HDR_FIELD_LOCATION, HTTP_STR_HDR_FIELD_LOCATION, (sizeof(HTTP_STR_HDR_FIELD_LOCATION)            - 1) },
  { HTTP_HDR_FIELD_CONN, HTTP_STR_HDR_FIELD_CONN, (sizeof(HTTP_STR_HDR_FIELD_CONN)                - 1) },
  { HTTP_HDR_FIELD_TRANSFER_ENCODING, HTTP_STR_HDR_FIELD_TRANSFER_ENCODING, (sizeof(HTTP_STR_HDR_FIELD_TRANSFER_ENCODING)   - 1) },
  { HTTP_HDR_FIELD_ACCEPT, HTTP_STR_HDR_FIELD_ACCEPT, (sizeof(HTTP_STR_HDR_FIELD_ACCEPT)              - 1) },
  { HTTP_HDR_FIELD_ACCEPT_CHARSET, HTTP_STR_HDR_FIELD_ACCEPT_CHARSET, (sizeof(HTTP_STR_HDR_FIELD_ACCEPT_CHARSET)      - 1) },
  { HTTP_HDR_FIELD_ACCEPT_ENCODING, HTTP_STR_HDR_FIELD_ACCEPT_ENCODING, (sizeof(HTTP_STR_HDR_FIELD_ACCEPT_ENCODING)     - 1) },
  { HTTP_HDR_FIELD_ACCEPT_LANGUAGE, HTTP_STR_HDR_FIELD_ACCEPT_LANGUAGE, (sizeof(HTTP_STR_HDR_FIELD_ACCEPT_LANGUAGE)     - 1) },
  { HTTP_HDR_FIELD_ACCEPT_RANGES, HTTP_STR_HDR_FIELD_ACCEPT_RANGES, (sizeof(HTTP_STR_HDR_FIELD_ACCEPT_RANGES)       - 1) },
  { HTTP_HDR_FIELD_AGE, HTTP_STR_HDR_FIELD_AGE, (sizeof(HTTP_STR_HDR_FIELD_AGE)                 - 1) },
  { HTTP_HDR_FIELD_ALLOW, HTTP_STR_HDR_FIELD_ALLOW, (sizeof(HTTP_STR_HDR_FIELD_ALLOW)               - 1) },
  { HTTP_HDR_FIELD_AUTHORIZATION, HTTP_STR_HDR_FIELD_AUTHORIZATION, (sizeof(HTTP_STR_HDR_FIELD_AUTHORIZATION)       - 1) },
  { HTTP_HDR_FIELD_CONTENT_ENCODING, HTTP_STR_HDR_FIELD_CONTENT_ENCODING, (sizeof(HTTP_STR_HDR_FIELD_CONTENT_ENCODING)    - 1) },
  { HTTP_HDR_FIELD_CONTENT_LANGUAGE, HTTP_STR_HDR_FIELD_CONTENT_LANGUAGE, (sizeof(HTTP_STR_HDR_FIELD_CONTENT_LANGUAGE)    - 1) },
  { HTTP_HDR_FIELD_CONTENT_LOCATION, HTTP_STR_HDR_FIELD_CONTENT_LOCATION, (sizeof(HTTP_STR_HDR_FIELD_CONTENT_LOCATION)    - 1) },
  { HTTP_HDR_FIELD_CONTENT_MD5, HTTP_STR_HDR_FIELD_CONTENT_MD5, (sizeof(HTTP_STR_HDR_FIELD_CONTENT_MD5)         - 1) },
  { HTTP_HDR_FIELD_CONTENT_RANGE, HTTP_STR_HDR_FIELD_CONTENT_RANGE, (sizeof(HTTP_STR_HDR_FIELD_CONTENT_RANGE)       - 1) },
  { HTTP_HDR_FIELD_COOKIE, HTTP_STR_HDR_FIELD_COOKIE, (sizeof(HTTP_STR_HDR_FIELD_COOKIE)              - 1) },
  { HTTP_HDR_FIELD_COOKIE2, HTTP_STR_HDR_FIELD_COOKIE2, (sizeof(HTTP_STR_HDR_FIELD_COOKIE2)             - 1) },
  { HTTP_HDR_FIELD_DATE, HTTP_STR_HDR_FIELD_DATE, (sizeof(HTTP_STR_HDR_FIELD_DATE)                - 1) },
  { HTTP_HDR_FIELD_ETAG, HTTP_STR_HDR_FIELD_ETAG, (sizeof(HTTP_STR_HDR_FIELD_ETAG)                - 1) },
  { HTTP_HDR_FIELD_EXPECT, HTTP_STR_HDR_FIELD_EXPECT, (sizeof(HTTP_STR_HDR_FIELD_EXPECT)              - 1) },
  { HTTP_HDR_FIELD_EXPIRES, HTTP_STR_HDR_FIELD_EXPIRES, (sizeof(HTTP_STR_HDR_FIELD_EXPIRES)             - 1) },
  { HTTP_HDR_FIELD_FROM, HTTP_STR_HDR_FIELD_FROM, (sizeof(HTTP_STR_HDR_FIELD_FROM)                - 1) },
  { HTTP_HDR_FIELD_IF_MODIFIED_SINCE, HTTP_STR_HDR_FIELD_IF_MODIFIED_SINCE, (sizeof(HTTP_STR_HDR_FIELD_IF_MODIFIED_SINCE)   - 1) },
  { HTTP_HDR_FIELD_IF_MATCH, HTTP_STR_HDR_FIELD_IF_MATCH, (sizeof(HTTP_STR_HDR_FIELD_IF_MATCH)            - 1) },
  { HTTP_HDR_FIELD_IF_NONE_MATCH, HTTP_STR_HDR_FIELD_IF_NONE_MATCH, (sizeof(HTTP_STR_HDR_FIELD_IF_NONE_MATCH)       - 1) },
  { HTTP_HDR_FIELD_IF_RANGE, HTTP_STR_HDR_FIELD_IF_RANGE, (sizeof(HTTP_STR_HDR_FIELD_IF_RANGE)            - 1) },
  { HTTP_HDR_FIELD_IF_UNMODIFIED_SINCE, HTTP_STR_HDR_FIELD_IF_UNMODIFIED_SINCE, (sizeof(HTTP_STR_HDR_FIELD_IF_UNMODIFIED_SINCE) - 1) },
  { HTTP_HDR_FIELD_LAST_MODIFIED, HTTP_STR_HDR_FIELD_LAST_MODIFIED, (sizeof(HTTP_STR_HDR_FIELD_LAST_MODIFIED)       - 1) },
  { HTTP_HDR_FIELD_RANGE, HTTP_STR_HDR_FIELD_RANGE, (sizeof(HTTP_STR_HDR_FIELD_RANGE)               - 1) },
  { HTTP_HDR_FIELD_REFERER, HTTP_STR_HDR_FIELD_REFERER, (sizeof(HTTP_STR_HDR_FIELD_REFERER)             - 1) },
  { HTTP_HDR_FIELD_RETRY_AFTER, HTTP_STR_HDR_FIELD_RETRY_AFTER, (sizeof(HTTP_STR_HDR_FIELD_RETRY_AFTER)         - 1) },
  { HTTP_HDR_FIELD_SERVER, HTTP_STR_HDR_FIELD_SERVER, (sizeof(HTTP_STR_HDR_FIELD_SERVER)              - 1) },
  { HTTP_HDR_FIELD_SET_COOKIE, HTTP_STR_HDR_FIELD_SET_COOKIE, (sizeof(HTTP_STR_HDR_FIELD_SET_COOKIE)          - 1) },
  { HTTP_HDR_FIELD_SET_COOKIE2, HTTP_STR_HDR_FIELD_SET_COOKIE2, (sizeof(HTTP_STR_HDR_FIELD_SET_COOKIE2)         - 1) },
  { HTTP_HDR_FIELD_TE, HTTP_STR_HDR_FIELD_TE, (sizeof(HTTP_STR_HDR_FIELD_TE)                  - 1) },
  { HTTP_HDR_FIELD_TRAILER, HTTP_STR_HDR_FIELD_TRAILER, (sizeof(HTTP_STR_HDR_FIELD_TRAILER)             - 1) },
  { HTTP_HDR_FIELD_UPGRADE, HTTP_STR_HDR_FIELD_UPGRADE, (sizeof(HTTP_STR_HDR_FIELD_UPGRADE)             - 1) },
  { HTTP_HDR_FIELD_USER_AGENT, HTTP_STR_HDR_FIELD_USER_AGENT, (sizeof(HTTP_STR_HDR_FIELD_USER_AGENT)          - 1) },
  { HTTP_HDR_FIELD_VARY, HTTP_STR_HDR_FIELD_VARY, (sizeof(HTTP_STR_HDR_FIELD_VARY)                - 1) },
  { HTTP_HDR_FIELD_VIA, HTTP_STR_HDR_FIELD_VIA, (sizeof(HTTP_STR_HDR_FIELD_VIA)                 - 1) },
  { HTTP_HDR_FIELD_WARNING, HTTP_STR_HDR_FIELD_WARNING, (sizeof(HTTP_STR_HDR_FIELD_WARNING)             - 1) },
  { HTTP_HDR_FIELD_WWW_AUTHENTICATE, HTTP_STR_HDR_FIELD_WWW_AUTHENTICATE, (sizeof(HTTP_STR_HDR_FIELD_WWW_AUTHENTICATE)    - 1) },
  { HTTP_HDR_FIELD_WEBSOCKET_KEY, HTTP_STR_HDR_FIELD_WEBSOCKET_KEY, (sizeof(HTTP_STR_HDR_FIELD_WEBSOCKET_KEY)       - 1) },
  { HTTP_HDR_FIELD_WEBSOCKET_ACCEPT, HTTP_STR_HDR_FIELD_WEBSOCKET_ACCEPT, (sizeof(HTTP_STR_HDR_FIELD_WEBSOCKET_ACCEPT)    - 1) },
  { HTTP_HDR_FIELD_WEBSOCKET_VERSION, HTTP_STR_HDR_FIELD_WEBSOCKET_VERSION, (sizeof(HTTP_STR_HDR_FIELD_WEBSOCKET_VERSION)   - 1) },
  { HTTP_HDR_FIELD_WEBSOCKET_PROTOCOL, HTTP_STR_HDR_FIELD_WEBSOCKET_PROTOCOL, (sizeof(HTTP_STR_HDR_FIELD_WEBSOCKET_PROTOCOL)  - 1) },
  { HTTP_HDR_FIELD_WEBSOCKET_EXTENSIONS, HTTP_STR_HDR_FIELD_WEBSOCKET_EXTENSIONS, (sizeof(HTTP_STR_HDR_FIELD_WEBSOCKET_EXTENSIONS) - 1) }
};

CPU_SIZE_T HTTP_Dict_HdrFieldSize = sizeof(HTTP_Dict_HdrField);

/********************************************************************************************************
 *                                       HTTP CONTENT DISPOSITION VALUE
 *******************************************************************************************************/

const HTTP_DICT HTTP_DictContentDispositionVal[] = {
  { HTTP_CONTENT_DISPOSITION_FORM_DATA, HTTP_STR_CONTENT_DISPOSITION_FORM_DATA, (sizeof(HTTP_STR_CONTENT_DISPOSITION_FORM_DATA)  - 1) }
};

CPU_SIZE_T HTTP_Dict_ContentDispositionValSize = sizeof(HTTP_DictContentDispositionVal);

/********************************************************************************************************
 *                                           HTTP CONNECTION VALUE
 *******************************************************************************************************/

const HTTP_DICT HTTP_Dict_HdrFieldConnVal[] = {
  { HTTP_HDR_FIELD_CONN_CLOSE, HTTP_STR_HDR_FIELD_CONN_CLOSE, (sizeof(HTTP_STR_HDR_FIELD_CONN_CLOSE)      - 1) },
  { HTTP_HDR_FIELD_CONN_PERSISTENT, HTTP_STR_HDR_FIELD_CONN_KEEP_ALIVE, (sizeof(HTTP_STR_HDR_FIELD_CONN_KEEP_ALIVE) - 1) },
  { HTTP_HDR_FIELD_CONN_UPGRADE, HTTP_STR_HDR_FIELD_CONN_UPGRADE, (sizeof(HTTP_STR_HDR_FIELD_CONN_UPGRADE)    - 1) }
};

CPU_SIZE_T HTTP_Dict_HdrFieldConnValSize = sizeof(HTTP_Dict_HdrFieldConnVal);

/********************************************************************************************************
 *                                   HTTP TRANSFER ENCODING VALUE
 *******************************************************************************************************/

const HTTP_DICT HTTP_Dict_HdrFieldTransferTypeVal[] = {
  { HTTP_HDR_FIELD_TRANSFER_TYPE_CHUNCKED, HTTP_STR_HDR_FIELD_CHUNKED, (sizeof(HTTP_STR_HDR_FIELD_CHUNKED)   - 1) }
};

CPU_SIZE_T HTTP_Dict_HdrFieldTransferTypeValSize = sizeof(HTTP_Dict_HdrFieldTransferTypeVal);

/********************************************************************************************************
 *                                   HTTP FORM MULTIPART CONTENT FIELD
 *******************************************************************************************************/

const HTTP_DICT HTTP_Dict_MultipartField[] = {
  { HTTP_MULTIPART_FIELD_NAME, HTTP_STR_MULTIPART_FIELD_NAME, (sizeof(HTTP_STR_MULTIPART_FIELD_NAME)      - 1) },
  { HTTP_MULTIPART_FIELD_FILE_NAME, HTTP_STR_MULTIPART_FIELD_FILE_NAME, (sizeof(HTTP_STR_MULTIPART_FIELD_FILE_NAME) - 1) }
};

CPU_SIZE_T HTTP_Dict_MultipartFieldSize = sizeof(HTTP_Dict_MultipartField);

/********************************************************************************************************
 *                                           HTTP UPGRADE VALUE
 *******************************************************************************************************/

const HTTP_DICT HTTP_Dict_HdrFieldUpgradeVal[] = {
  { HTTP_HDR_FIELD_UPGRADE_WEBSOCKET, HTTP_STR_HDR_FIELD_UPGRADE_WEBSOCKET, (sizeof(HTTP_STR_HDR_FIELD_UPGRADE_WEBSOCKET) - 1) },
};

CPU_SIZE_T HTTP_Dict_HdrFieldUpgradeValSize = sizeof(HTTP_Dict_HdrFieldUpgradeVal);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         HTTP_Dict_KeyGet()
 *
 * @brief    Get the HTTP dictionary key associated with a string entry by comparing given string with
 *           dictionary string entries.
 *
 * @param    p_dict_tbl  Pointer to the HTTP dictionary.
 *
 * @param    dict_size   Size of the given HTTP dictionary in octet.
 *
 * @param    p_str_cmp   Pointer to the string use for comparison.
 *
 * @param    str_len     String's length.
 *
 * @return   Key,                if string found within dictionary.
 *           Invalid key number, otherwise.
 *
 *           Various.
 *******************************************************************************************************/
CPU_INT32U HTTP_Dict_KeyGet(const HTTP_DICT *p_dict_tbl,
                            CPU_INT32U      dict_size,
                            const CPU_CHAR  *p_str_cmp,
                            CPU_BOOLEAN     case_sensitive,
                            CPU_INT32U      str_len)
{
  CPU_INT32U nbr_entry;
  CPU_INT32U ix;
  CPU_INT32U len;
  CPU_INT16S cmp;
  HTTP_DICT  *p_srch;

  nbr_entry = dict_size / sizeof(HTTP_DICT);
  p_srch = (HTTP_DICT *)p_dict_tbl;
  for (ix = 0; ix < nbr_entry; ix++) {
    len = DEF_MIN(str_len, p_srch->StrLen);
    if (case_sensitive == DEF_YES) {
      cmp = Str_Cmp_N(p_str_cmp, p_srch->StrPtr, len);
      if (cmp == 0) {
        return (p_srch->Key);
      }
    } else {
      cmp = Str_CmpIgnoreCase_N(p_str_cmp, p_srch->StrPtr, len);
      if (cmp == 0) {
        return (p_srch->Key);
      }
    }

    p_srch++;
  }

  return (HTTP_DICT_KEY_INVALID);
}

/****************************************************************************************************//**
 *                                          HTTP_Dict_EntryGet()
 *
 * @brief    Get an HTTP dictionary entry object from a dictionary key entry.
 *
 * @param    p_dict_tbl  Pointer to the HTTP dictionary.
 *
 * @param    dict_size   Size of the given HTTP dictionary in octet.
 *
 * @param    key         Dictionary key entry.
 *
 * @return   Pointer to the dictionary entry object.
 *
 *           Various.
 *******************************************************************************************************/
HTTP_DICT *HTTP_Dict_EntryGet(const HTTP_DICT *p_dict_tbl,
                              CPU_INT32U      dict_size,
                              CPU_INT32U      key)
{
  HTTP_DICT  *p_entry;
  CPU_INT32U nbr_entry;
  CPU_INT32U ix;

  nbr_entry = dict_size / sizeof(HTTP_DICT);
  p_entry = (HTTP_DICT *)p_dict_tbl;
  for (ix = 0; ix < nbr_entry; ix++) {                          // Srch until last entry is reached.
    if (p_entry->Key == key) {                                  // If keys match ...
      return (p_entry);                                         // ... the first entry is found.
    }

    p_entry++;                                                  // Move to next entry.
  }

  return (DEF_NULL);                                            // No entry found.
}

/****************************************************************************************************//**
 *                                         HTTP_Dict_ValCopy()
 *
 * @brief    Copy dictionary entry string to destination string buffer, up to a maximum number of characters.
 *
 * @param    p_dict      Pointer to the HTTP dictionary.
 *
 * @param    dict_size   Size of the given HTTP dictionary in octet.
 *
 * @param    key         Dictionary key entry.
 *
 * @param    p_buf       Pointer to destination string buffer where string will be copied.
 *
 * @param    buf_len     Maximum number of characters to copy.
 *
 * @return   Pointer to the end of the value copied, if value successfully copied.
 *           Pointer to NULL,                        otherwise.
 *
 *           Various.
 *******************************************************************************************************/
CPU_CHAR *HTTP_Dict_ValCopy(const HTTP_DICT *p_dict_tbl,
                            CPU_INT32U      dict_size,
                            CPU_INT32U      key,
                            CPU_CHAR        *p_buf,
                            CPU_SIZE_T      buf_len)
{
  const HTTP_DICT *p_entry;
  CPU_CHAR        *p_str_rtn;

  p_entry = &p_dict_tbl[key];
  if (p_entry->Key != key) {                                    // If entry key doesn't match dictionary key ...
    p_entry = HTTP_Dict_EntryGet(p_dict_tbl,                    // ... get first entry that match the    key.
                                 dict_size,
                                 key);
    if (p_entry == DEF_NULL) {
      return (DEF_NULL);
    }
  }

  if ((p_entry->StrLen == 0u)                                   // Validate entry value.
      || (p_entry->StrPtr == DEF_NULL)) {
    return (DEF_NULL);
  }

  if (p_entry->StrLen > buf_len) {                              // Validate value len and buf len.
    return (DEF_NULL);
  }

  (void)Str_Copy_N(p_buf, p_entry->StrPtr, p_entry->StrLen);    // Copy string to the buffer.

  p_str_rtn = p_buf + p_entry->StrLen;                          // Set ptr to return.

  return (p_str_rtn);
}

/****************************************************************************************************//**
 *                                       HTTP_Dict_StrKeySrch()
 *
 * @brief    (1) Search string for first occurrence of a specific dictionary key, up to a maximum number
 *               of characters:
 *               - (a) Validate pointers
 *               - (b) Set dictionary entry
 *               - (c) Search for string
 *
 * @param    p_dict_tbl  Pointer to the HTTP dictionary.
 *
 * @param    dict_size   Size of the given HTTP dictionary in octet.
 *
 * @param    key         Dictionary key entry.
 *
 * @param    p_str       Pointer to string to found in entry.
 *
 * @param    str_len     Maximum number of characters to search.
 *
 * @return   Pointer to first occurrence of search string key, if any.
 *           Pointer to NULL,                                  otherwise.
 *
 *           Various.
 *******************************************************************************************************/
CPU_CHAR *HTTP_Dict_StrKeySrch(const HTTP_DICT *p_dict_tbl,
                               CPU_INT32U      dict_size,
                               CPU_INT32U      key,
                               const CPU_CHAR  *p_str,
                               CPU_SIZE_T      str_len)
{
  const HTTP_DICT *p_entry;
  CPU_CHAR        *p_found;

  //                                                               ------------------ VALIDATE PTRS -------------------
  if ((p_str == DEF_NULL)
      || (p_dict_tbl == DEF_NULL)) {
    return (DEF_NULL);
  }

  //                                                               --------------- SET DICTIONARY ENTRY ---------------
  p_entry = &p_dict_tbl[key];
  if (p_entry->Key != key) {                                    // If entry key doesn't match dictionary key ...
    p_entry = HTTP_Dict_EntryGet(p_dict_tbl,                    // ... get first entry that match the    key.
                                 dict_size,
                                 key);
    if (p_entry == DEF_NULL) {
      return (DEF_NULL);
    }
  }

  //                                                               ----------------- SRCH FOR STRING ------------------
  p_found = Str_Str_N(p_str, p_entry->StrPtr, str_len);

  return (p_found);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_SERVER_AVAIL || RTOS_MODULE_NET_HTTP_CLIENT_AVAIL
