// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/ws_url.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/string_token.h"

#define WS_PROTOCOL              "ws://"
#define WSS_PROTOCOL             "wss://"
#define MIN_URL_PARSABLE_LENGTH  7

typedef struct WS_URL_TAG
{
    char* url;

    bool is_secure;

    const char* host;
    size_t host_length;

    size_t port;

    const char* path;
    size_t path_length;

    const char* query;
    size_t query_length;
} WS_URL;

static int parse_ws_url(const char* url, WS_URL* ws_url)
{
    int result = 0;
    size_t url_length = strlen(url);

    if (url_length < MIN_URL_PARSABLE_LENGTH)
    {
        LogError("Invalid url (unexpected length)");
        result = MU_FAILURE;
    }
    // Codes_SRS_WS_URL_09_004: [ If url starts with "ws://" (protocol), ws_url->is_secure shall be set to false ]
    if (strncmp(url, WS_PROTOCOL, 5) == 0)
    {
        ws_url->is_secure = false;
    }
    // Codes_SRS_WS_URL_09_005: [ If url starts with "wss://" (protocol), ws_url->is_secure shall be set to true ]
    else if (strncmp(url, WSS_PROTOCOL, 6) == 0)
    {
        ws_url->is_secure = true;
    }
    else
    {
        // Codes_SRS_WS_URL_09_024: [ If protocol cannot be identified in url, the function shall fail and return NULL ]
        LogError("Url protocol prefix not recognized");
        result = MU_FAILURE;
    }

    if (result == 0)
    {
        size_t host_begin;

        const char* port_delimiter = ":";
        const char* path_delimiter = "/";
        const char* query_delimiter = "?";
        
        const char* delimiters1[3];
        const char* delimiters2[1];

        size_t delimiter_count = 3;
        const char** current_delimiters = delimiters1;
        const char* previous_delimiter = NULL;

        bool host_parsed = false;
        bool port_parsed = false;
        bool path_parsed = false;
        bool query_parsed = false;

        STRING_TOKEN_HANDLE token;

        delimiters1[0] = port_delimiter; 
        delimiters1[1] = path_delimiter;
        delimiters1[2] = query_delimiter;
        
        delimiters2[0] = query_delimiter;

        host_begin = (ws_url->is_secure ? 6 : 5);

        token = StringToken_GetFirst(url + host_begin, url_length - host_begin, current_delimiters, delimiter_count);

        if (token == NULL)
        {
            LogError("Failed getting first url token");
            result = MU_FAILURE;
        }
        else
        {
            do
            {
                const char* current_delimiter = (char*)StringToken_GetDelimiter(token);

                if (previous_delimiter == NULL && !host_parsed && !port_parsed && !path_parsed && !query_parsed)
                {
                    // Codes_SRS_WS_URL_09_006: [ The pointer to the token starting right after protocol (in the url string) shall be stored in ws_url->host ]
                    ws_url->host = (char*)StringToken_GetValue(token);
                    // Codes_SRS_WS_URL_09_008: [ The length from ws_url->host up to the first occurrence of either ":" (port_delimiter), "/" (path_delimiter), "?" (query_delimiter) or \0 shall be stored in ws_url->host_length ]
                    ws_url->host_length = StringToken_GetLength(token);

                    // Codes_SRS_WS_URL_09_007: [ If ws_url->host ends up being NULL, the function shall fail and return NULL ]
                    // Codes_SRS_WS_URL_09_009: [ If ws_url->host_length ends up being zero, the function shall fail and return NULL ]
                    if (ws_url->host == NULL || ws_url->host_length == 0)
                    {
                        LogError("Failed parsing websocket url host");
                        result = MU_FAILURE;
                        break;
                    }
                    else
                    {
                        host_parsed = true;
                    }
                }
                // Codes_SRS_WS_URL_09_010: [ If after ws_url->host the port_delimiter occurs (not preceeded by path_delimiter or query_delimiter) the number that follows shall be parsed and stored in ws_url->port ]
                else if (previous_delimiter == port_delimiter && host_parsed && !port_parsed && !path_parsed && !query_parsed)
                {
                    const char* port = StringToken_GetValue(token);
                    size_t port_length = StringToken_GetLength(token);

                    // Codes_SRS_WS_URL_09_011: [ If the port number fails to be parsed, the function shall fail and return NULL ]
                    if (port == NULL || port_length == 0)
                    {
                        LogError("Failed parsing websocket url port");
                        result = MU_FAILURE;
                        break;
                    }
                    else
                    {
                        char port_copy[10];
                        (void)memset(port_copy, 0, sizeof(char) * 10);
                        (void)memcpy(port_copy, port, port_length);

                        ws_url->port = (size_t)atoi(port_copy);

                        port_parsed = true;
                    }
                }
                // Codes_SRS_WS_URL_09_012: [ If after ws_url->host or the port number the path_delimiter occurs (not preceeded by query_delimiter) the following pointer address shall be stored in ws_url->path ]
                else if (previous_delimiter == path_delimiter && host_parsed && !path_parsed && !query_parsed)
                {
                    ws_url->path = (char*)StringToken_GetValue(token);
                    // Codes_SRS_WS_URL_09_014: [ The length from ws_url->path up to the first occurrence of either query_delimiter or \0 shall be stored in ws_url->path_length ]
                    ws_url->path_length = StringToken_GetLength(token);

                    // Codes_SRS_WS_URL_09_013: [ If the path component is present and ws_url->path ends up being NULL, the function shall fail and return NULL ]
                    // Codes_SRS_WS_URL_09_015: [ If the path component is present and ws_url->path_length ends up being zero, the function shall fail and return NULL ]
                    if (ws_url->path == NULL || ws_url->path_length == 0)
                    {
                        LogError("Failed parsing websocket url path");
                        result = MU_FAILURE;
                        break;
                    }
                    else
                    {
                        path_parsed = true;
                    }
                }
                // Codes_SRS_WS_URL_09_016: [ Next if the query_delimiter occurs the following pointer address shall be stored in ws_url->query ]
                else if (previous_delimiter == query_delimiter && current_delimiter == NULL && host_parsed && !query_parsed)
                {
                    ws_url->query = (char*)StringToken_GetValue(token);
                    // Codes_SRS_WS_URL_09_018: [ The length from ws_url->query up to \0 shall be stored in ws_url->query_length ]
                    ws_url->query_length = StringToken_GetLength(token);

                    // Codes_SRS_WS_URL_09_017: [ If the query component is present and ws_url->query ends up being NULL, the function shall fail and return NULL ]
                    // Codes_SRS_WS_URL_09_019: [ If the query component is present and ws_url->query_length ends up being zero, the function shall fail and return NULL ]
                    if (ws_url->query == NULL || ws_url->query_length == 0)
                    {
                        LogError("Failed parsing websocket url query");
                        result = MU_FAILURE;
                        break;
                    }
                    else
                    {
                        query_parsed = true;
                    }
                }
                else
                {
                    LogError("Failed parsing websocket url (format not recognized)");
                    result = MU_FAILURE;
                    break;
                }

                if (current_delimiter == path_delimiter)
                {
                    current_delimiters = delimiters2;
                    delimiter_count = 1;
                }

                previous_delimiter = current_delimiter;
            }
            while (StringToken_GetNext(token, current_delimiters, delimiter_count));

            StringToken_Destroy(token);
        }
    }

    return result;
}

void ws_url_destroy(WS_URL_HANDLE url)
{
    // Codes_SRS_WS_URL_09_022: [ If url is NULL, the function shall return without further action ]
    if (url != NULL)
    {
        // Codes_SRS_WS_URL_09_023: [ Otherwise, the memory allocated for url shall released ]
        free(url->url);
        free(url);
    }
}

WS_URL_HANDLE ws_url_create(const char* url)
{
    WS_URL* result;

    // Codes_SRS_WS_URL_09_001: [ If url is NULL the function shall fail and return NULL ]
    if (url == NULL)
    {
        LogError("Invalid argument (url is NULL)");
        result = NULL;
    }
    // Codes_SRS_WS_URL_09_002: [ Memory shall be allocated for an instance of WS_URL (aka ws_url) ]
    else if ((result = malloc(sizeof(WS_URL))) == NULL)
    {
        // Codes_SRS_WS_URL_09_003: [ If ws_url failed to be allocated, the function shall return NULL ]
        LogError("Failed to allocate the websockets url");
    }
    else
    {
        memset(result, 0, sizeof(WS_URL));

        // Codes_SRS_WS_URL_09_024: [ url shall be copied into ws_url->url ]
        if (mallocAndStrcpy_s(&result->url, url) != 0)
        {
            // Codes_SRS_WS_URL_09_025: [ If url fails to be copied, the function shall free ws_url and return NULL ]
            LogError("Failed copying the source url");
            ws_url_destroy(result);
            result = NULL;
        }
        else if (parse_ws_url(result->url, result) != 0)
        {
            // Codes_SRS_WS_URL_09_020: [ If any component cannot be parsed or is out of order, the function shall fail and return NULL ]
            // Codes_SRS_WS_URL_09_021: [ If any failure occurs, all memory allocated by the function shall be released before returning ]
            LogError("Failed parsing the websockets url");
            ws_url_destroy(result);
            result = NULL;
        }
    }

    return result;
}

int ws_url_is_secure(WS_URL_HANDLE url, bool* is_secure)
{
    int result;

    // Codes_SRS_WS_URL_09_026: [ If url is NULL, the function shall return a non-zero value (failure) ]
    if (url == NULL || is_secure == NULL)
    {
        LogError("Invalid argument (url=%p, is_secure=%p)", url, is_secure);
        result = MU_FAILURE;
    }
    else
    {
        // Codes_SRS_WS_URL_09_027: [ Otherwize the function shall set is_secure as url->is_secure ]
        *is_secure = url->is_secure;

        // Codes_SRS_WS_URL_09_028: [ If no errors occur function shall return zero (success) ]
        result = 0;
    }

    return result;
}

int ws_url_get_host(WS_URL_HANDLE url, const char** host, size_t* length)
{
    int result;

    // Codes_SRS_WS_URL_09_029: [ If url or host or length are NULL, the function shall return a non-zero value (failure) ]
    if (url == NULL || host == NULL || length == NULL)
    {
        LogError("Invalid argument (url=%p, host=%p, length=%p)", url, host, length);
        result = MU_FAILURE;
    }
    else
    {
        // Codes_SRS_WS_URL_09_030: [ Otherwize the function shall set host to url->host and length to url->host_length ]
        *host = url->host;
        *length = url->host_length;

        // Codes_SRS_WS_URL_09_031: [ If no errors occur function shall return zero (success) ]
        result = 0;
    }

    return result;
}

int ws_url_get_port(WS_URL_HANDLE url, size_t* port)
{
    int result;

    // Codes_SRS_WS_URL_09_038: [ If url or port are NULL, the function shall return a non-zero value (failure) ]
    if (url == NULL || port == NULL)
    {
        LogError("Invalid argument (url=%p, port=%p)", url, port);
        result = MU_FAILURE;
    }
    else
    {
        // Codes_SRS_WS_URL_09_039: [ Otherwize the function shall set port as url->port ]
        *port = url->port;

        // Codes_SRS_WS_URL_09_040: [ If no errors occur function shall return zero (success) ]
        result = 0;
    }

    return result;
}

int ws_url_get_path(WS_URL_HANDLE url, const char** path, size_t* length)
{
    int result;

    // Codes_SRS_WS_URL_09_032: [ If url or path or length are NULL, the function shall return a non-zero value (failure) ]
    if (url == NULL || path == NULL || length == NULL)
    {
        LogError("Invalid argument (url=%p, path=%p, length=%p)", url, path, length);
        result = MU_FAILURE;
    }
    else
    {
        // Codes_SRS_WS_URL_09_033: [ Otherwize the function shall set path to url->path and length to url->path_length ]
        *path = url->path;
        *length = url->path_length;

        // Codes_SRS_WS_URL_09_034: [ If no errors occur function shall return zero (success) ]
        result = 0;
    }

    return result;
}

int ws_url_get_query(WS_URL_HANDLE url, const char** query, size_t* length)
{
    int result;

    // Codes_SRS_WS_URL_09_035: [ If url or query or length are NULL, the function shall return a non-zero value (failure) ]
    if (url == NULL || query == NULL || length == NULL)
    {
        LogError("Invalid argument (url=%p, query=%p, length=%p)", url, query, length);
        result = MU_FAILURE;
    }
    else
    {
        // Codes_SRS_WS_URL_09_036: [ Otherwize the function shall set query to url->query and length to url->query_length ]
        *query = url->query;
        *length = url->query_length;

        // Codes_SRS_WS_URL_09_037: [ If no errors occur function shall return zero (success) ]
        result = 0;
    }

    return result;
}