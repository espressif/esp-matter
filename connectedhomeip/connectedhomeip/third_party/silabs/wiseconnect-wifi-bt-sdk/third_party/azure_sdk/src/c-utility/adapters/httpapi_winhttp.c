// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <string.h>
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"

#include "windows.h"
#include "winhttp.h"
#include "wincrypt.h"
#include "azure_c_shared_utility/httpapi.h"
#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/x509_schannel.h"
#include "azure_c_shared_utility/shared_util_options.h"

MU_DEFINE_ENUM_STRINGS(HTTPAPI_RESULT, HTTPAPI_RESULT_VALUES)

typedef enum HTTPAPI_STATE_TAG
{
    HTTPAPI_NOT_INITIALIZED,
    HTTPAPI_INITIALIZED
} HTTPAPI_STATE;

typedef struct HTTP_HANDLE_DATA_TAG
{
    /*working set*/
    HINTERNET ConnectionHandle;
    X509_SCHANNEL_HANDLE x509SchannelHandle;
    /*options*/
    unsigned int timeout;
    const char* x509certificate;
    const char* x509privatekey;
    const char* proxy_host;
    const char* proxy_username;
    const char* proxy_password;
    // Certificate to check server certificate chains to, overriding built-in Windows certificate store certificates.
    char* trustedCertificate;
    // Set when a callback fails.
    bool handleClosedOnCallbackError;
} HTTP_HANDLE_DATA;

static HTTPAPI_STATE g_HTTPAPIState = HTTPAPI_NOT_INITIALIZED;

/*There's a global SessionHandle for all the connections*/
static HINTERNET g_SessionHandle;
static size_t nUsersOfHTTPAPI = 0; /*used for reference counting (a weak one)*/

static char* ConcatHttpHeaders(HTTP_HEADERS_HANDLE httpHeadersHandle, size_t toAlloc, size_t headersCount)
{
    char *result = (char*)malloc(toAlloc * sizeof(char) + 1);
    size_t i;
    
    if (result == NULL)
    {
        LogError("unable to malloc");
    }
    else
    {
        result[0] = '\0';
        for (i = 0; i < headersCount; i++)
        {
            char* temp;
            if (HTTPHeaders_GetHeader(httpHeadersHandle, i, &temp) != HTTP_HEADERS_OK)
            {
                LogError("unable to HTTPHeaders_GetHeader");
                break;
            }
            else
            {
                (void)strcat(result, temp);
                (void)strcat(result, "\r\n");
                free(temp);
            }
        }
    
        if (i < headersCount)
        {
            free(result);
            result = NULL;
        }
        else
        {
            /*all is good*/
        }
    }

    return result;
}

/*returns NULL if it failed to construct the headers*/
static HTTPAPI_RESULT ConstructHeadersString(HTTP_HEADERS_HANDLE httpHeadersHandle, wchar_t** httpHeaders)
{
    HTTPAPI_RESULT result;
    size_t headersCount;

    if (HTTPHeaders_GetHeaderCount(httpHeadersHandle, &headersCount) != HTTP_HEADERS_OK)
    {
        result = HTTPAPI_ERROR;
        LogError("HTTPHeaders_GetHeaderCount failed (result = %" PRI_MU_ENUM ").", MU_ENUM_VALUE(HTTPAPI_RESULT, result));
    }
    else
    {
        size_t i;

        /*the total size of all the headers is given by sumof(lengthof(everyheader)+2)*/
        size_t toAlloc = 0;
        for (i = 0; i < headersCount; i++)
        {
            char *temp;
            if (HTTPHeaders_GetHeader(httpHeadersHandle, i, &temp) == HTTP_HEADERS_OK)
            {
                toAlloc += strlen(temp);
                toAlloc += 2;
                free(temp);
            }
            else
            {
                LogError("HTTPHeaders_GetHeader failed");
                break;
            }
        }

        if (i < headersCount)
        {
            result = HTTPAPI_ERROR;
        }
        else
        {
            char *httpHeadersA;
            size_t requiredCharactersForHeaders;

            if ((httpHeadersA = ConcatHttpHeaders(httpHeadersHandle, toAlloc, headersCount)) == NULL)
            {
                result = HTTPAPI_ERROR;
                LogError("Cannot concatenate headers");
            }
            else if ((requiredCharactersForHeaders = MultiByteToWideChar(CP_ACP, 0, httpHeadersA, -1, NULL, 0)) == 0)
            {
                result = HTTPAPI_STRING_PROCESSING_ERROR;
                LogError("MultiByteToWideChar failed, GetLastError=0x%08x (result = %" PRI_MU_ENUM ")", GetLastError(), MU_ENUM_VALUE(HTTPAPI_RESULT, result));
            }
            else if ((*httpHeaders = (wchar_t*)malloc((requiredCharactersForHeaders + 1) * sizeof(wchar_t))) == NULL)
            {
                result = HTTPAPI_ALLOC_FAILED;
                LogError("Cannot allocate memory (result = %" PRI_MU_ENUM ")", MU_ENUM_VALUE(HTTPAPI_RESULT, result));
            }
            else if (MultiByteToWideChar(CP_ACP, 0, httpHeadersA, -1, *httpHeaders, (int)requiredCharactersForHeaders) == 0)
            {
                result = HTTPAPI_STRING_PROCESSING_ERROR;
                LogError("MultiByteToWideChar failed, GetLastError=0x%08x (result = %" PRI_MU_ENUM ")", GetLastError(), MU_ENUM_VALUE(HTTPAPI_RESULT, result));
                free(*httpHeaders);
                *httpHeaders = NULL;
            }
            else
            {
                result = HTTPAPI_OK;
            }

            free(httpHeadersA);
        }
    }

    return result;
}

void CALLBACK httpapi_WinhttpStatusCallback(
  IN HINTERNET hInternet,
  IN DWORD_PTR dwContext,
  IN DWORD dwInternetStatus,
  IN LPVOID lpvStatusInformation,
  IN DWORD dwStatusInformationLength
)
{
    HTTP_HANDLE_DATA* handleData = (HTTP_HANDLE_DATA*)dwContext;

    (void)dwStatusInformationLength;
    (void)lpvStatusInformation;

    if (dwContext == 0)
    {
        LogError("WinhttpStatusCallback called without context set");
    }
    else if (dwInternetStatus != WINHTTP_CALLBACK_STATUS_SENDING_REQUEST)
    {
        // Silently ignore if there's any statuses we get that we can't handle
        ;
    }
    else if (handleData->trustedCertificate != NULL)
    {
        PCERT_CONTEXT pCertContext = NULL;
        DWORD bufferLength = sizeof(pCertContext);
        bool certificateTrusted;

        if (! WinHttpQueryOption(hInternet, WINHTTP_OPTION_SERVER_CERT_CONTEXT, (void*)&pCertContext, &bufferLength))
        {
            LogErrorWinHTTPWithGetLastErrorAsString("WinHttpQueryOption(WINHTTP_OPTION_SERVER_CERT_CONTEXT) failed");
            certificateTrusted = false;
        }
        else if (x509_verify_certificate_in_chain(handleData->trustedCertificate, pCertContext) != 0)
        {
            LogError("Certificate does not chain up correctly");
            certificateTrusted = false;
        }
        else
        {
            certificateTrusted = true;
        }

        if (certificateTrusted == false)
        {
            LogError("Server certificate is not trusted.  Aborting HTTP request");
            // To signal to caller that the request is to be terminated, the callback closes the handle.
            WinHttpCloseHandle(hInternet);
            // To avoid a double free of this handle (in HTTPAPI_ExecuteRequset cleanup) record we've processed close already.
            handleData->handleClosedOnCallbackError = true;
        }

        if (pCertContext != NULL)
        {
            CertFreeCertificateContext(pCertContext);
        }
    }
}


HTTPAPI_RESULT HTTPAPI_Init(void)
{
    HTTPAPI_RESULT result;

    if (nUsersOfHTTPAPI == 0)
    {
        if ((g_SessionHandle = WinHttpOpen(
            NULL,
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            0)) == NULL)
        {
            LogErrorWinHTTPWithGetLastErrorAsString("WinHttpOpen failed.");
            result = HTTPAPI_INIT_FAILED;
        }
        else
        {
            DWORD supportedProtocols = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1 | // TLS 1.0 is support for back-compat reasons (https://docs.microsoft.com/en-us/azure/iot-fundamentals/iot-security-deployment)
                WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_1 |
                WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2;

            if (!WinHttpSetOption(
                g_SessionHandle,
                WINHTTP_OPTION_SECURE_PROTOCOLS,
                &supportedProtocols,
                sizeof(supportedProtocols)
            ))
            {
                LogErrorWinHTTPWithGetLastErrorAsString("unable to WinHttpSetOption (WINHTTP_OPTION_SECURE_PROTOCOLS)");
                (void)WinHttpCloseHandle(g_SessionHandle);
                g_SessionHandle = NULL;
                result = HTTPAPI_INIT_FAILED;
            }
            else if (WinHttpSetStatusCallback(g_SessionHandle, httpapi_WinhttpStatusCallback, WINHTTP_CALLBACK_FLAG_SEND_REQUEST, 0) == WINHTTP_INVALID_STATUS_CALLBACK)
            {
                LogErrorWinHTTPWithGetLastErrorAsString("WinHttpSetStatusCallback failed.");
                (void)WinHttpCloseHandle(g_SessionHandle);
                g_SessionHandle = NULL;
                result = HTTPAPI_INIT_FAILED;
            }
            else
            {
                nUsersOfHTTPAPI++;
                g_HTTPAPIState = HTTPAPI_INITIALIZED;
                result = HTTPAPI_OK;
            }
        }
    }
    else
    {
        nUsersOfHTTPAPI++;
        result = HTTPAPI_OK;
    }

    return result;
}

void HTTPAPI_Deinit(void)
{
    if (nUsersOfHTTPAPI > 0)
    {
        nUsersOfHTTPAPI--;
        if (nUsersOfHTTPAPI == 0)
        {
            if (g_SessionHandle != NULL)
            {
                (void)WinHttpCloseHandle(g_SessionHandle);
                g_SessionHandle = NULL;
                g_HTTPAPIState = HTTPAPI_NOT_INITIALIZED;
            }
        }
    }


}

HTTP_HANDLE HTTPAPI_CreateConnection(const char* hostName)
{
    HTTP_HANDLE_DATA* result;
    if (g_HTTPAPIState != HTTPAPI_INITIALIZED)
    {
        LogError("g_HTTPAPIState not HTTPAPI_INITIALIZED");
        result = NULL;
    }
    else
    {
        result = (HTTP_HANDLE_DATA*)malloc(sizeof(HTTP_HANDLE_DATA));
        if (result == NULL)
        {
            LogError("malloc returned NULL.");
        }
        else
        {
            memset(result, 0, sizeof(*result));
            wchar_t* hostNameTemp;
            size_t hostNameTemp_size = MultiByteToWideChar(CP_ACP, 0, hostName, -1, NULL, 0);
            if (hostNameTemp_size == 0)
            {
                LogError("MultiByteToWideChar failed");
                free(result);
                result = NULL;
            }
            else
            {
                hostNameTemp = (wchar_t*)malloc(sizeof(wchar_t) * hostNameTemp_size);
                if (hostNameTemp == NULL)
                {
                    LogError("malloc failed");
                    free(result);
                    result = NULL;
                }
                else
                {
                    if (MultiByteToWideChar(CP_ACP, 0, hostName, -1, hostNameTemp, (int)hostNameTemp_size) == 0)
                    {
                        LogError("MultiByteToWideChar failed");
                        free(result);
                        result = NULL;
                    }
                    else
                    {
                        result->ConnectionHandle = WinHttpConnect(
                            g_SessionHandle,
                            hostNameTemp,
                            INTERNET_DEFAULT_HTTPS_PORT,
                            0);

                        if (result->ConnectionHandle == NULL)
                        {
                            LogErrorWinHTTPWithGetLastErrorAsString("WinHttpConnect returned NULL.");
                            free(result);
                            result = NULL;
                        }
                        else
                        {
                            result->timeout = 60000;
                        }
                    }
                    free(hostNameTemp);
                }
            }
        }
    }

    return (HTTP_HANDLE)result;
}

void HTTPAPI_CloseConnection(HTTP_HANDLE handle)
{
    if (g_HTTPAPIState != HTTPAPI_INITIALIZED)
    {
        LogError("g_HTTPAPIState not HTTPAPI_INITIALIZED");
    }
    else
    {
        HTTP_HANDLE_DATA* handleData = (HTTP_HANDLE_DATA*)handle;

        if (handleData != NULL)
        {
            if (handleData->ConnectionHandle != NULL)
            {
                (void)WinHttpCloseHandle(handleData->ConnectionHandle);
                /*no x509 free because the options are owned by httpapiex.*/
                handleData->ConnectionHandle = NULL;
            }
            if (handleData->proxy_host != NULL)
            {
                free((void*)handleData->proxy_host);
            }
            if (handleData->proxy_username != NULL)
            {
                free((void*)handleData->proxy_username);
            }
            if (handleData->proxy_password != NULL)
            {
                free((void*)handleData->proxy_password);
            }
            x509_schannel_destroy(handleData->x509SchannelHandle);
            free(handleData);
        }
    }
}

static const wchar_t* GetHttpRequestString(HTTPAPI_REQUEST_TYPE requestType)
{
    wchar_t* requestTypeString = NULL;
    
    switch (requestType)
    {
    default:
        break;
    
    case HTTPAPI_REQUEST_GET:
        requestTypeString = L"GET";
        break;
    
    case HTTPAPI_REQUEST_HEAD:
        requestTypeString = L"HEAD";
        break;
    
    case HTTPAPI_REQUEST_POST:
        requestTypeString = L"POST";
        break;
    
    case HTTPAPI_REQUEST_PUT:
        requestTypeString = L"PUT";
        break;
    
    case HTTPAPI_REQUEST_DELETE:
        requestTypeString = L"DELETE";
        break;
    
    case HTTPAPI_REQUEST_PATCH:
        requestTypeString = L"PATCH";
        break;
    }

    return requestTypeString;
}

static HTTPAPI_RESULT InitiateWinhttpRequest(HTTP_HANDLE_DATA* handleData, HTTPAPI_REQUEST_TYPE requestType, const char* relativePath, HINTERNET *requestHandle)
{
    HTTPAPI_RESULT result;
    const wchar_t* requestTypeString;
    size_t requiredCharactersForRelativePath;
    wchar_t* relativePathTemp = NULL;
    
    if ((requestTypeString = GetHttpRequestString(requestType)) == NULL)
    {
        result = HTTPAPI_INVALID_ARG;
        LogError("requestTypeString was NULL (result = %" PRI_MU_ENUM ")", MU_ENUM_VALUE(HTTPAPI_RESULT, result));
    }
    else if ((requiredCharactersForRelativePath = MultiByteToWideChar(CP_ACP, 0, relativePath, -1, NULL, 0)) == 0)
    {
        result = HTTPAPI_STRING_PROCESSING_ERROR;
        LogError("MultiByteToWideChar failed, GetLastError=0x%08x", GetLastError());
    }
    else if ((relativePathTemp = (wchar_t*)malloc((requiredCharactersForRelativePath + 1) * sizeof(wchar_t))) == NULL)
    {
        result = HTTPAPI_ALLOC_FAILED;
        LogError("malloc failed (result = %" PRI_MU_ENUM ")", MU_ENUM_VALUE(HTTPAPI_RESULT, result));
    }
    else if (MultiByteToWideChar(CP_ACP, 0, relativePath, -1, relativePathTemp, (int)requiredCharactersForRelativePath) == 0)
    {
        result = HTTPAPI_STRING_PROCESSING_ERROR;
        LogError("MultiByteToWideChar was 0. (result = %" PRI_MU_ENUM ")", MU_ENUM_VALUE(HTTPAPI_RESULT, result));
    }
    else if ((*requestHandle = WinHttpOpenRequest(
            handleData->ConnectionHandle,
            requestTypeString,
            relativePathTemp,
            NULL,
            WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            WINHTTP_FLAG_SECURE)) == NULL)
    {
        result = HTTPAPI_OPEN_REQUEST_FAILED;
        LogErrorWinHTTPWithGetLastErrorAsString("WinHttpOpenRequest failed (result = %" PRI_MU_ENUM ").", MU_ENUM_VALUE(HTTPAPI_RESULT, result));
    }
    else if ((handleData->x509SchannelHandle != NULL) &&
            !WinHttpSetOption(
                *requestHandle,
                WINHTTP_OPTION_CLIENT_CERT_CONTEXT,
                (void*)x509_schannel_get_certificate_context(handleData->x509SchannelHandle),
                sizeof(CERT_CONTEXT)
    ))
    {
        result = HTTPAPI_SET_X509_FAILURE;
        LogErrorWinHTTPWithGetLastErrorAsString("unable to WinHttpSetOption (WINHTTP_OPTION_CLIENT_CERT_CONTEXT)");
        (void)WinHttpCloseHandle(*requestHandle);
        *requestHandle = NULL;
    }
    else
    {
        result = HTTPAPI_OK;
    }

    free(relativePathTemp);
    return result;
}

static HTTPAPI_RESULT SendHttpRequest(HTTP_HANDLE_DATA* handleData, HINTERNET requestHandle, const unsigned char* content, size_t contentLength, const wchar_t* httpHeaders)
{
    HTTPAPI_RESULT result;

    if (WinHttpSetTimeouts(requestHandle,
        0,                      /*_In_  int dwResolveTimeout - The initial value is zero, meaning no time-out (infinite). */
        60000,                  /*_In_  int dwConnectTimeout, -  The initial value is 60,000 (60 seconds).*/
        handleData->timeout,    /*_In_  int dwSendTimeout, -  The initial value is 30,000 (30 seconds).*/
        handleData->timeout     /* int dwReceiveTimeout The initial value is 30,000 (30 seconds).*/
    ) == FALSE)
    {
        result = HTTPAPI_SET_TIMEOUTS_FAILED;
        LogErrorWinHTTPWithGetLastErrorAsString("WinHttpOpenRequest failed (result = %" PRI_MU_ENUM ")", MU_ENUM_VALUE(HTTPAPI_RESULT, result));
    }
    else
    {
        DWORD dwSecurityFlags;

        if (handleData->trustedCertificate != NULL)
        {
            // If caller has specified trusted certificates, then we'll instruct Winhttp to ignore certain classes
            // of invalid certificate errors (since the trusted cert won't chain into the Windows cert store).
            // We will validate certificate manually during Winhttp callbacks.
            dwSecurityFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA;
        }
        else
        {
            dwSecurityFlags = 0;
        }

        if (!WinHttpSetOption(
            requestHandle,
            WINHTTP_OPTION_SECURITY_FLAGS,
            &dwSecurityFlags,
            sizeof(dwSecurityFlags)))
        {
            result = HTTPAPI_SET_OPTION_FAILED;
            LogErrorWinHTTPWithGetLastErrorAsString("WinHttpSetOption failed (result = %" PRI_MU_ENUM ").", MU_ENUM_VALUE(HTTPAPI_RESULT, result));
        }
        else if (!WinHttpSendRequest(
                requestHandle,
                httpHeaders,
                (DWORD)-1L, /*An unsigned long integer value that contains the length, in characters, of the additional headers. If this parameter is -1L ... */
                (void*)content,
                (DWORD)contentLength,
                (DWORD)contentLength,
                (DWORD_PTR)handleData))
        {
            result = HTTPAPI_SEND_REQUEST_FAILED;
            LogErrorWinHTTPWithGetLastErrorAsString("WinHttpSendRequest: (result = %" PRI_MU_ENUM ").", MU_ENUM_VALUE(HTTPAPI_RESULT, result));
        }
        else
        {
            result = HTTPAPI_OK;
        }
    }
    
    return result;    
}
    
static HTTPAPI_RESULT SetProxyIfNecessary(HTTP_HANDLE_DATA* handleData, HINTERNET requestHandle)
{
    HTTPAPI_RESULT result;

    // Set proxy host if needed
    if (handleData->proxy_host != NULL)
    {
        WINHTTP_PROXY_INFO winhttp_proxy;
        wchar_t wproxy[MAX_HOSTNAME_LEN];
        winhttp_proxy.dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
        if (mbstowcs_s(NULL, wproxy, MAX_HOSTNAME_LEN, handleData->proxy_host, MAX_HOSTNAME_LEN-1) != 0)
        {
            LogError("Error during proxy host conversion");
            result = HTTPAPI_ERROR;
        }
        else
        {
            winhttp_proxy.lpszProxy = wproxy;
            winhttp_proxy.lpszProxyBypass = NULL;
            if (WinHttpSetOption(requestHandle,
                WINHTTP_OPTION_PROXY,
                &winhttp_proxy,
                (DWORD)sizeof(WINHTTP_PROXY_INFO)) != TRUE)
            {
                LogError("failure setting proxy address (%i)", GetLastError());
                result = HTTPAPI_ERROR;
            }
            else
            {
                //Set username and password if needed
                if (handleData->proxy_username != NULL && handleData->proxy_password != NULL)
                {
                    wchar_t wusername[MAX_USERNAME_LEN];
                    if (mbstowcs_s(NULL, wusername, MAX_USERNAME_LEN, handleData->proxy_username, MAX_USERNAME_LEN-1) != 0)
                    {
                        LogError("Error during proxy username conversion");
                        result = HTTPAPI_ERROR;
                    }
                    else
                    {
                        wchar_t wpassword[MAX_PASSWORD_LEN];
                        if (mbstowcs_s(NULL, wpassword, MAX_PASSWORD_LEN, handleData->proxy_password, MAX_PASSWORD_LEN-1) != 0)
                        {
                            LogError("Error during proxy password conversion");
                            result = HTTPAPI_ERROR;
                        }
                        else
                        {
                            if (WinHttpSetCredentials(requestHandle,
                                WINHTTP_AUTH_TARGET_PROXY,
                                WINHTTP_AUTH_SCHEME_BASIC,
                                wusername,
                                wpassword,
                                NULL) != TRUE)
                            {
                                LogErrorWinHTTPWithGetLastErrorAsString("Failure setting proxy credentials");
                                result = HTTPAPI_ERROR;
                            }
                            else
                            {
                                result = HTTPAPI_OK;
                            }
                        }
                    }
                }
                else
                {
                    result = HTTPAPI_OK;
                }
            }
        }
    }
    else
    {
        result = HTTPAPI_OK;
    }

    return result;
}

static HTTPAPI_RESULT ReceiveResponseAndStatusCode(HINTERNET requestHandle, unsigned int* statusCode)
{
    HTTPAPI_RESULT result;

    if (!WinHttpReceiveResponse(
        requestHandle,
        0))
    {
        result = HTTPAPI_RECEIVE_RESPONSE_FAILED;
        LogErrorWinHTTPWithGetLastErrorAsString("WinHttpReceiveResponse: (result = %" PRI_MU_ENUM ").", MU_ENUM_VALUE(HTTPAPI_RESULT, result));
    }
    else if (statusCode != NULL)
    {
        DWORD dwStatusCode = 0;
        DWORD dwBufferLength = sizeof(DWORD);
    
        if (!WinHttpQueryHeaders(
            requestHandle,
            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
            WINHTTP_HEADER_NAME_BY_INDEX,
            &dwStatusCode,
            &dwBufferLength,
            WINHTTP_NO_HEADER_INDEX))
        {
            result = HTTPAPI_QUERY_HEADERS_FAILED;
            LogErrorWinHTTPWithGetLastErrorAsString("WinHttpQueryHeaders failed (result = %" PRI_MU_ENUM ")", MU_ENUM_VALUE(HTTPAPI_RESULT, result));
        }
        else
        {
            *statusCode = dwStatusCode;
            result = HTTPAPI_OK;
        }
    }
    else
    {
        result = HTTPAPI_OK;
    }

    return result;
}

static HTTPAPI_RESULT ReceiveResponseContent(HINTERNET requestHandle, BUFFER_HANDLE responseContent)
{
    HTTPAPI_RESULT result;
    DWORD responseBytesAvailable;

    while (true)
    {
        /*from MSDN: If no data is available and the end of the file has not been reached, one of two things happens. If the session is synchronous, the request waits until data becomes available.*/
        if (!WinHttpQueryDataAvailable(requestHandle, &responseBytesAvailable))
        {
            result = HTTPAPI_QUERY_DATA_AVAILABLE_FAILED;
            LogErrorWinHTTPWithGetLastErrorAsString("WinHttpQueryDataAvailable failed (result = %" PRI_MU_ENUM ").", MU_ENUM_VALUE(HTTPAPI_RESULT, result));
            break;
        }
        else if (responseBytesAvailable == 0)
        {
            /*end of the stream, go out*/
            result = HTTPAPI_OK;
            break;
        }
        else if (BUFFER_enlarge(responseContent, responseBytesAvailable) != 0)
        {
            result = HTTPAPI_ALLOC_FAILED;
            LogError("(result = %" PRI_MU_ENUM ")", MU_ENUM_VALUE(HTTPAPI_RESULT, result));
            break;
        }
        else
        {
            /*Add the read bytes to the response buffer*/
            size_t bufferSize;
            const unsigned char* bufferContent;

            if (BUFFER_content(responseContent, &bufferContent) != 0)
            {
                result = HTTPAPI_ERROR;
                LogError("(result = %" PRI_MU_ENUM ")", MU_ENUM_VALUE(HTTPAPI_RESULT, result));
                break;
            }
            else if (BUFFER_size(responseContent, &bufferSize) != 0)
            {
                result = HTTPAPI_ERROR;
                LogError("(result = %" PRI_MU_ENUM ")", MU_ENUM_VALUE(HTTPAPI_RESULT, result));
                break;
            }
            else
            {
                DWORD bytesReceived;
                if (!WinHttpReadData(requestHandle, (LPVOID)(bufferContent + bufferSize - responseBytesAvailable), responseBytesAvailable, &bytesReceived))
                {
                    result = HTTPAPI_READ_DATA_FAILED;
                    LogErrorWinHTTPWithGetLastErrorAsString("WinHttpReadData failed (result = %" PRI_MU_ENUM ")", MU_ENUM_VALUE(HTTPAPI_RESULT, result));
                    break;
                }
                /*if for some reason bytesReceived is zero If you are using WinHttpReadData synchronously, and the return value is TRUE and the number of bytes read is zero, the transfer has been completed and there are no more bytes to read on the handle.*/
                else if (bytesReceived == 0)
                {
                    /*end of everything, but this looks like an error still, or a non-conformance between WinHttpQueryDataAvailable and WinHttpReadData*/
                    result = HTTPAPI_READ_DATA_FAILED;
                    LogError("bytesReceived was unexpectedly zero (result = %" PRI_MU_ENUM ")", MU_ENUM_VALUE(HTTPAPI_RESULT, result));
                    break;
                }
                else
                {
                    /*all is fine, keep going*/
                }
            }
        }
    } 

    return result;
}


static HTTPAPI_RESULT ReceiveResponseHeaders(HINTERNET requestHandle, HTTP_HEADERS_HANDLE responseHeadersHandle)
{
    HTTPAPI_RESULT result;
    wchar_t* responseHeadersTemp = NULL;
    DWORD responseHeadersTempLength = 0;

    // Don't explicictly check return code here - since this should fail (to determine length)
    // and checking return code means an explicit GetLastError() comparison.  Instead
    // rely on subsequent WinHttpQueryHeaders() to fail.
    WinHttpQueryHeaders(
        requestHandle,
        WINHTTP_QUERY_RAW_HEADERS_CRLF,
        WINHTTP_HEADER_NAME_BY_INDEX,
        WINHTTP_NO_OUTPUT_BUFFER,
        &responseHeadersTempLength,
        WINHTTP_NO_HEADER_INDEX);

    if ((responseHeadersTemp = (wchar_t*)malloc(responseHeadersTempLength + 2)) == NULL)
    {
        result = HTTPAPI_ALLOC_FAILED;
        LogError("malloc failed: (result = %" PRI_MU_ENUM ")", MU_ENUM_VALUE(HTTPAPI_RESULT, result));
    }
    else if (! WinHttpQueryHeaders(
            requestHandle,
            WINHTTP_QUERY_RAW_HEADERS_CRLF,
            WINHTTP_HEADER_NAME_BY_INDEX,
            responseHeadersTemp,
            &responseHeadersTempLength,
            WINHTTP_NO_HEADER_INDEX))
    {
        result = HTTPAPI_QUERY_HEADERS_FAILED;
        LogErrorWinHTTPWithGetLastErrorAsString("WinHttpQueryHeaders failed (result = %" PRI_MU_ENUM ")", MU_ENUM_VALUE(HTTPAPI_RESULT, result));
    }
    else    
    {
        wchar_t *next_token = NULL;
        wchar_t* token = wcstok_s(responseHeadersTemp, L"\r\n", &next_token);
        char* tokenTemp = NULL;     

        result = HTTPAPI_OK;

        while ((token != NULL) &&
            (token[0] != L'\0'))
        {
            size_t tokenTemp_size;

            tokenTemp_size = WideCharToMultiByte(CP_ACP, 0, token, -1, NULL, 0, NULL, NULL);
            if (tokenTemp_size == 0)
            {
                result = HTTPAPI_STRING_PROCESSING_ERROR;
                LogError("WideCharToMultiByte failed");
                break;
            }
            else if ((tokenTemp = (char*)malloc(sizeof(char) * tokenTemp_size)) == NULL)
            {
                result = HTTPAPI_ALLOC_FAILED;
                LogError("malloc failed");
                break;
            }
            else if (WideCharToMultiByte(CP_ACP, 0, token, -1, tokenTemp, (int)tokenTemp_size, NULL, NULL) == 0)
            {
                result = HTTPAPI_STRING_PROCESSING_ERROR;
                LogError("WideCharToMultiByte failed");
                break;
            }
            else
            {
                /*breaking the token in 2 parts: everything before the first ":" and everything after the first ":"*/
                /*if there is no such character, then skip it*/
                /*if there is a : then replace is by a '\0' and so it breaks the original string in name and value*/
                char* whereIsColon = strchr(tokenTemp, ':');
                if (whereIsColon != NULL)
                {
                    *whereIsColon = '\0';
                    if (HTTPHeaders_AddHeaderNameValuePair(responseHeadersHandle, tokenTemp, whereIsColon + 1) != HTTP_HEADERS_OK)
                    {
                        LogError("HTTPHeaders_AddHeaderNameValuePair failed");
                        result = HTTPAPI_HTTP_HEADERS_FAILED;
                        break;
                    }
                }
            }
            free(tokenTemp);
            tokenTemp = NULL;

            token = wcstok_s(NULL, L"\r\n", &next_token);
        }

        free(tokenTemp);
        tokenTemp = NULL;
    }

    free(responseHeadersTemp);
    return result;
}


HTTPAPI_RESULT HTTPAPI_ExecuteRequest(HTTP_HANDLE handle, HTTPAPI_REQUEST_TYPE requestType, const char* relativePath,
    HTTP_HEADERS_HANDLE httpHeadersHandle, const unsigned char* content,
    size_t contentLength, unsigned int* statusCode,
    HTTP_HEADERS_HANDLE responseHeadersHandle, BUFFER_HANDLE responseContent)
{
    HTTPAPI_RESULT result;

    if (g_HTTPAPIState != HTTPAPI_INITIALIZED)
    {
        LogError("g_HTTPAPIState not HTTPAPI_INITIALIZED");
        result = HTTPAPI_NOT_INIT;
    }
    else if ((handle == NULL) ||
            (relativePath == NULL) ||
            (httpHeadersHandle == NULL))
    {
        result = HTTPAPI_INVALID_ARG;
        LogError("NULL parameter detected (result = %" PRI_MU_ENUM ")", MU_ENUM_VALUE(HTTPAPI_RESULT, result));
    }
    else
    {
        HTTP_HANDLE_DATA* handleData = (HTTP_HANDLE_DATA*)handle;
        wchar_t* httpHeaders = NULL;
        HINTERNET requestHandle = NULL;

        if ((result = InitiateWinhttpRequest(handleData, requestType, relativePath, &requestHandle)) != HTTPAPI_OK)
        {
            LogError("Cannot create Winhttp request handle");
        }
        else if ((result = SetProxyIfNecessary(handleData, requestHandle)) != HTTPAPI_OK)
        {
            LogError("unable to set proxy");
        }
        else if ((result = ConstructHeadersString(httpHeadersHandle, &httpHeaders)) != HTTPAPI_OK)
        {
            LogError("Cannot construct http headers");
        }
        else if ((result = SendHttpRequest(handleData, requestHandle, content, contentLength, httpHeaders)) != HTTPAPI_OK)
        {
            LogError("Cannot set options / send http request");
        }
        else if ((result = ReceiveResponseAndStatusCode(requestHandle, statusCode)) != HTTPAPI_OK)
        {
            LogError("failed receiving response and/or headeders");
        }
        else if ((responseContent != NULL) && ((result = ReceiveResponseContent(requestHandle, responseContent)) != HTTPAPI_OK))
        {
            LogError("failed to receive response content");
        }
        else if ((responseHeadersHandle != NULL) && ((result = ReceiveResponseHeaders(requestHandle, responseHeadersHandle)) != HTTPAPI_OK))
        {
            LogError("Unable to retrieve http response headers");
        }
        else
        {
            result = HTTPAPI_OK;
        }

        if ((requestHandle != NULL) && (handleData->handleClosedOnCallbackError == false))
        {
            (void)WinHttpCloseHandle(requestHandle);
        }
        handleData->handleClosedOnCallbackError = false;

        free(httpHeaders);
    }

    return result;
}

HTTPAPI_RESULT HTTPAPI_SetOption(HTTP_HANDLE handle, const char* optionName, const void* value)
{
    HTTPAPI_RESULT result;
    if (
        (handle == NULL) ||
        (optionName == NULL) ||
        (value == NULL)
        )
    {
        result = HTTPAPI_INVALID_ARG;
        LogError("invalid parameter (NULL) passed to HTTPAPI_SetOption");
    }
    else
    {
        HTTP_HANDLE_DATA* httpHandleData = (HTTP_HANDLE_DATA*)handle;
        if (strcmp(OPTION_HTTP_TIMEOUT, optionName) == 0)
        {
            long timeout = (long)(*(unsigned int*)value);
            httpHandleData->timeout = timeout;
            result = HTTPAPI_OK;
        }
        else if (strcmp(SU_OPTION_X509_CERT, optionName) == 0 || strcmp(OPTION_X509_ECC_CERT, optionName) == 0)
        {
            httpHandleData->x509certificate = (const char*)value;
            if (httpHandleData->x509privatekey != NULL)
            {
                httpHandleData->x509SchannelHandle = x509_schannel_create(httpHandleData->x509certificate, httpHandleData->x509privatekey);
                if (httpHandleData->x509SchannelHandle == NULL)
                {
                    LogError("unable to x509_schannel_create");
                    result = HTTPAPI_ERROR;
                }
                else
                {
                    result = HTTPAPI_OK;
                }
            }
            else
            {
                /*if certificate comes 1st and private key is not set yet, then return OK and wait for the private key to be set*/
                result = HTTPAPI_OK;
            }
        }
        else if (strcmp(SU_OPTION_X509_PRIVATE_KEY, optionName) == 0 || strcmp(OPTION_X509_ECC_KEY, optionName) == 0)
        {
            httpHandleData->x509privatekey = (const char*)value;
            if (httpHandleData->x509certificate != NULL)
            {
                httpHandleData->x509SchannelHandle = x509_schannel_create(httpHandleData->x509certificate, httpHandleData->x509privatekey);
                if (httpHandleData->x509SchannelHandle == NULL)
                {
                    LogError("unable to x509_schannel_create");
                    result = HTTPAPI_ERROR;
                }
                else
                {
                    result = HTTPAPI_OK;
                }
            }
            else
            {
                /*if privatekey comes 1st and certificate is not set yet, then return OK and wait for the certificate to be set*/
                result = HTTPAPI_OK;
            }
        }
        else if (strcmp(OPTION_TRUSTED_CERT, optionName) == 0)
        {
            if (value == NULL)
            {
                LogError("Invalid paramater: OPTION_TRUSTED_CERT value=NULL"); 
                result = MU_FAILURE;
            }
            else
            {
                if (httpHandleData->trustedCertificate != NULL)
                {   
                    free(httpHandleData->trustedCertificate);
                    httpHandleData->trustedCertificate = NULL;
                }

                if (mallocAndStrcpy_s((char**)&httpHandleData->trustedCertificate, (const char*)value) != 0)
                {
                    LogError("unable to mallocAndStrcpy_s option trusted certificate");
                    result = MU_FAILURE;
                }
                else
                {
                    result = HTTPAPI_OK;
                }
            }
        }
        else if (strcmp(OPTION_HTTP_PROXY, optionName) == 0)
        {
            char proxyAddressAndPort[MAX_HOSTNAME_LEN];
            HTTP_PROXY_OPTIONS* proxy_data = (HTTP_PROXY_OPTIONS*)value;
            if (proxy_data->host_address == NULL || proxy_data->port <= 0)
            {
                LogError("invalid proxy_data values ( host_address = %p, port = %d)", proxy_data->host_address, proxy_data->port);
                result = HTTPAPI_ERROR;
            }
            else
            {
                if (sprintf_s(proxyAddressAndPort, MAX_HOSTNAME_LEN, "%s:%d", proxy_data->host_address, proxy_data->port) <= 0)
                {
                    LogError("failure constructing proxy address");
                    result = HTTPAPI_ERROR;
                }
                else
                {
                    if(httpHandleData->proxy_host != NULL)
                    {
                        free((void*)httpHandleData->proxy_host);
                        httpHandleData->proxy_host = NULL;
                    }
                    if(mallocAndStrcpy_s((char**)&(httpHandleData->proxy_host), (const char*)proxyAddressAndPort) != 0)
                    {
                        LogError("failure allocate proxy host");
                        result = HTTPAPI_ERROR;
                    }
                    else
                    {
                        if (proxy_data->username != NULL && proxy_data->password != NULL)
                        {
                            if(httpHandleData->proxy_username != NULL)
                            {
                                free((void*)httpHandleData->proxy_username);
                                httpHandleData->proxy_username = NULL;
                            }
                            if(mallocAndStrcpy_s((char**)&(httpHandleData->proxy_username), (const char*)proxy_data->username) != 0)
                            {
                                LogError("failure allocate proxy username");
                                free((void*)httpHandleData->proxy_host);
                                httpHandleData->proxy_host = NULL;
                                result = HTTPAPI_ERROR;
                            }
                            else
                            {
                                if(httpHandleData->proxy_password != NULL)
                                {
                                  free((void*)httpHandleData->proxy_password);
                                  httpHandleData->proxy_password = NULL;
                                }
                                if(mallocAndStrcpy_s((char**)&(httpHandleData->proxy_password), (const char*)proxy_data->password) != 0)
                                {
                                    LogError("failure allocate proxy password");
                                    free((void*)httpHandleData->proxy_host);
                                    httpHandleData->proxy_host = NULL;
                                    free((void*)httpHandleData->proxy_username);
                                    httpHandleData->proxy_username = NULL;
                                    result = HTTPAPI_ERROR;
                                }
                                else
                                {
                                  result = HTTPAPI_OK;
                                }
                            }
                        }
                        else
                        {
                            result = HTTPAPI_OK;
                        }
                    }
                }
            }
        }
        else
        {
            result = HTTPAPI_INVALID_ARG;
            LogError("unknown option %s", optionName);
        }
    }
    return result;
}

HTTPAPI_RESULT HTTPAPI_CloneOption(const char* optionName, const void* value, const void** savedValue)
{
    HTTPAPI_RESULT result;
    if (
        (optionName == NULL) ||
        (value == NULL) ||
        (savedValue == NULL)
        )
    {
        result = HTTPAPI_INVALID_ARG;
        LogError("invalid argument(NULL) passed to HTTPAPI_CloneOption");
    }
    else
    {
        if (strcmp(OPTION_HTTP_TIMEOUT, optionName) == 0)
        {
            /*by convention value is pointing to an unsigned int */
            unsigned int* temp = (unsigned int*)malloc(sizeof(unsigned int)); /*shall be freed by HTTPAPIEX*/
            if (temp == NULL)
            {
                result = HTTPAPI_ERROR;
                LogError("malloc failed (result = %" PRI_MU_ENUM ")", MU_ENUM_VALUE(HTTPAPI_RESULT, result));
            }
            else
            {
                *temp = *(const unsigned int*)value;
                *savedValue = temp;
                result = HTTPAPI_OK;
            }
        }
        else if (strcmp(SU_OPTION_X509_CERT, optionName) == 0 || strcmp(OPTION_X509_ECC_CERT, optionName) == 0)
        {
            /*this is getting the x509 certificate. In this case, value is a pointer to a const char* that contains the certificate as a null terminated string*/
            if (mallocAndStrcpy_s((char**)savedValue, (const char*)value) != 0)
            {
                LogError("unable to clone the x509 certificate content");
                result = HTTPAPI_ERROR;
            }
            else
            {
                /*return OK when the certificate has been cloned successfully*/
                result = HTTPAPI_OK;
            }
        }
        else if (strcmp(SU_OPTION_X509_PRIVATE_KEY, optionName) == 0 || strcmp(OPTION_X509_ECC_KEY, optionName) == 0)
        {
            /*this is getting the x509 private key. In this case, value is a pointer to a const char* that contains the private key as a null terminated string*/
            if (mallocAndStrcpy_s((char**)savedValue, (const char*)value) != 0)
            {
                LogError("unable to clone the x509 private key content");
                result = HTTPAPI_ERROR;
            }
            else
            {
                /*return OK when the private key has been cloned successfully*/
                result = HTTPAPI_OK;
            }
        }
        else if (strcmp(OPTION_TRUSTED_CERT, optionName) == 0)
        {
            /*this is getting the trusted certificate */
            if (mallocAndStrcpy_s((char**)savedValue, (const char*)value) != 0)
            {
                LogError("unable to clone the trusted certificate content");
                result = HTTPAPI_ERROR;
            }
            else
            {
                /*return OK when the certificate has been cloned successfully*/
                result = HTTPAPI_OK;
            }
        }
        else if (strcmp(OPTION_HTTP_PROXY, optionName) == 0)
        {
            *savedValue = malloc(sizeof(HTTP_PROXY_OPTIONS));
            if (*savedValue == NULL)
            {
                LogError("failure in malloc");
                result = HTTPAPI_ERROR;
            }
            else
            {
                HTTP_PROXY_OPTIONS *savedOption = (HTTP_PROXY_OPTIONS*) *savedValue;
                HTTP_PROXY_OPTIONS *option = (HTTP_PROXY_OPTIONS*) value;
                memset((void*)*savedValue, 0, sizeof(HTTP_PROXY_OPTIONS));
                if (mallocAndStrcpy_s((char**)&(savedOption->host_address),
                                      (const char*)option->host_address) != 0)
                {
                  LogError("unable to clone the proxy host adress content");
                  free((void*)*savedValue);
                  *savedValue = NULL;
                  result = HTTPAPI_ERROR;
                }
                else
                {
                    savedOption->port = option->port;
                    if (option->username != NULL && mallocAndStrcpy_s((char**)&(savedOption->username),
                                                                      (const char*)option->username) != 0)
                    {
                      LogError("unable to clone the proxy username content");
                      free((void*)savedOption->host_address);
                      savedOption->host_address = NULL;
                      free((void*)*savedValue);
                      *savedValue = NULL;
                      result = HTTPAPI_ERROR;
                    }
                    else
                    {
                        if (option->password != NULL && mallocAndStrcpy_s((char**)&(savedOption->password),
                                                                          (const char*)option->password) != 0)
                        {
                          LogError("unable to clone the proxy password content");
                          free((void*)savedOption->host_address);
                          savedOption->host_address = NULL;
                          free((void*)savedOption->username);
                          savedOption->username = NULL;
                          free((void*)*savedValue);
                          *savedValue = NULL;
                          result = HTTPAPI_ERROR;
                        }
                        else
                        {
                            result = HTTPAPI_OK;
                        }
                    }
                }
            }
        }
        else
        {
            result = HTTPAPI_INVALID_ARG;
            LogError("unknown option %s", optionName);
        }
    }
    return result;
}
