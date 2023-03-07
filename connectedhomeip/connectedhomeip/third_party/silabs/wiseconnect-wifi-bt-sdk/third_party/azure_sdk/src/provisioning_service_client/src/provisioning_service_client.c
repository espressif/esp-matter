// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>

#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/string_tokenizer.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/urlencode.h"
#include "azure_c_shared_utility/connection_string_parser.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/http_proxy_io.h"

#include "azure_uhttp_c/uhttp.h"

#include "prov_service_client/provisioning_service_client.h"
#include "prov_service_client/provisioning_sc_models_serializer.h"
#include "prov_service_client/provisioning_sc_shared_helpers.h"

typedef enum HTTP_CONNECTION_STATE_TAG
{
    HTTP_STATE_DISCONNECTED,
    HTTP_STATE_CONNECTING,
    HTTP_STATE_CONNECTED,
    HTTP_STATE_REQUEST_SENT,
    HTTP_STATE_REQUEST_RECV,
    HTTP_STATE_COMPLETE,
    HTTP_STATE_ERROR
} HTTP_CONNECTION_STATE;

//consider substructure representing SharedAccessSignature?
typedef struct PROVISIONING_SERVICE_CLIENT_TAG
{
    //Connection details
    char* provisioning_service_uri;
    char* key_name;
    char* access_key;

    //Connection data
    HTTP_CONNECTION_STATE http_state;
    char* response;
    HTTP_HEADERS_HANDLE response_headers;

    //Connection options
    TRACING_STATUS tracing;
    HTTP_PROXY_OPTIONS* proxy_options;
    char* certificate;

} PROV_SERVICE_CLIENT;

typedef char*(*VECTOR_SERIALIZE_TO_JSON)(void*);
typedef void*(*VECTOR_DESERIALIZE_FROM_JSON)(char*);
typedef char*(*VECTOR_GET_ID)(void*);
typedef char*(*VECTOR_GET_ETAG)(void*);
typedef void(*VECTOR_DESTROY)(void*);

typedef struct HANDLE_FUNCTION_VECTOR_TAG
{
    VECTOR_SERIALIZE_TO_JSON serializeToJson;
    VECTOR_DESERIALIZE_FROM_JSON deserializeFromJson;
    VECTOR_GET_ID getId;
    VECTOR_GET_ETAG getEtag;
    VECTOR_DESTROY destroy;
} HANDLE_FUNCTION_VECTOR;

static const char* const IOTHUBHOSTNAME =                       "HostName";
static const char* const IOTHUBSHAREDACESSKEYNAME =             "SharedAccessKeyName";
static const char* const IOTHUBSHAREDACESSKEY =                 "SharedAccessKey";
static const char* const PROVISIONING_SERVICE_API_VERSION =     "2018-04-01";
static const char* const ENROLL_GROUP_PROVISION_PATH_FMT =      "/enrollmentGroups/%s";
static const char* const INDV_ENROLL_PROVISION_PATH_FMT =       "/enrollments/%s";
static const char* const REG_STATE_PROVISION_PATH_FMT =         "/registrations/%s";
static const char* const REG_STATE_QUERY_PATH_FMT =             "/registrations/%s/query";
static const char* const INDV_ENROLL_BULK_PATH_FMT =            "/enrollments/";
static const char* const INDV_ENROLL_QUERY_PATH_FMT =           "/enrollments/query";
static const char* const ENROLL_GROUP_QUERY_PATH_FMT =          "/enrollmentGroups/query";
static const char* const API_VERSION_QUERY_PARAM =              "?api-version=%s";
static const char* const HEADER_KEY_AUTHORIZATION =             "Authorization";
static const char* const HEADER_KEY_IF_MATCH =                  "If-Match";
static const char* const HEADER_KEY_USER_AGENT =                "UserAgent";
static const char* const HEADER_KEY_ACCEPT =                    "Accept";
static const char* const HEADER_KEY_CONTENT_TYPE =              "Content-Type";
static const char* const HEADER_KEY_CONTINUATION =              "x-ms-continuation";
static const char* const HEADER_KEY_MAX_ITEM_COUNT =            "x-ms-max-item-count";
static const char* const HEADER_KEY_ITEM_TYPE =                 "x-ms-item-type";
static const char* const HEADER_VALUE_USER_AGENT =              "iothub_dps_prov_client/1.0";
static const char* const HEADER_VALUE_ACCEPT =                  "application/json";
static const char* const HEADER_VALUE_CONTENT_TYPE =            "application/json; charset=utf-8";

#define DEFAULT_HTTPS_PORT          443
#define UID_LENGTH                  37
#define SAS_TOKEN_DEFAULT_LIFETIME  3600
#define EPOCH_TIME_T_VALUE          (time_t)0

static HANDLE_FUNCTION_VECTOR getVector_individualEnrollment()
{
    HANDLE_FUNCTION_VECTOR vector;
    vector.serializeToJson = (VECTOR_SERIALIZE_TO_JSON)individualEnrollment_serializeToJson;
    vector.deserializeFromJson = (VECTOR_DESERIALIZE_FROM_JSON)individualEnrollment_deserializeFromJson;
    vector.getId = (VECTOR_GET_ID)individualEnrollment_getRegistrationId;
    vector.getEtag = (VECTOR_GET_ETAG)individualEnrollment_getEtag;
    vector.destroy = (VECTOR_DESTROY)individualEnrollment_destroy;

    return vector;
}

static HANDLE_FUNCTION_VECTOR getVector_enrollmentGroup()
{
    HANDLE_FUNCTION_VECTOR vector;
    vector.serializeToJson = (VECTOR_SERIALIZE_TO_JSON)enrollmentGroup_serializeToJson;
    vector.deserializeFromJson = (VECTOR_DESERIALIZE_FROM_JSON)enrollmentGroup_deserializeFromJson;
    vector.getId = (VECTOR_GET_ID)enrollmentGroup_getGroupId;
    vector.getEtag = (VECTOR_GET_ETAG)enrollmentGroup_getEtag;
    vector.destroy = (VECTOR_DESTROY)enrollmentGroup_destroy;

    return vector;
}

static HANDLE_FUNCTION_VECTOR getVector_registrationState()
{
    HANDLE_FUNCTION_VECTOR vector;
    vector.serializeToJson = NULL;
    vector.deserializeFromJson = (VECTOR_DESERIALIZE_FROM_JSON)deviceRegistrationState_deserializeFromJson;
    vector.getId = (VECTOR_GET_ID)deviceRegistrationState_getRegistrationId;
    vector.getEtag = (VECTOR_GET_ETAG)deviceRegistrationState_getEtag;
    vector.destroy = (VECTOR_DESTROY)deviceRegistrationState_destroy;

    return vector;
}

static void on_http_connected(void* callback_ctx, HTTP_CALLBACK_REASON connect_result)
{
    if (callback_ctx != NULL)
    {
        PROV_SERVICE_CLIENT* prov_client = (PROV_SERVICE_CLIENT*)callback_ctx;
        if (connect_result == HTTP_CALLBACK_REASON_OK)
        {
            prov_client->http_state = HTTP_STATE_CONNECTED;
        }
        else
        {
            prov_client->http_state = HTTP_STATE_ERROR;
        }
    }
}

static void on_http_error(void* callback_ctx, HTTP_CALLBACK_REASON error_result)
{
    (void)error_result;
    if (callback_ctx != NULL)
    {
        PROV_SERVICE_CLIENT* prov_client = (PROV_SERVICE_CLIENT*)callback_ctx;
        prov_client->http_state = HTTP_STATE_ERROR;
        LogError("Failure encountered in http %d", error_result);
    }
    else
    {
        LogError("Failure encountered in http %d", error_result);
    }
}

static void on_http_reply_recv(void* callback_ctx, HTTP_CALLBACK_REASON request_result, const unsigned char* content, size_t content_len, unsigned int status_code, HTTP_HEADERS_HANDLE responseHeadersHandle)
{
    (void)content_len;
    if (callback_ctx != NULL)
    {
        PROV_SERVICE_CLIENT* prov_client = (PROV_SERVICE_CLIENT*)callback_ctx;
        const char* content_str = (const char*)content;

        //attach headers to prov_client
        if (responseHeadersHandle != NULL)
        {
            if ((prov_client->response_headers = HTTPHeaders_Clone(responseHeadersHandle)) == NULL)
            {
                LogError("Copying response headers failed");
                prov_client->response_headers = NULL;
            }
        }

        //if there is a json response
        if (content != NULL)
        {
            if ((prov_client->response = malloc(content_len + 1)) == NULL)
            {
                LogError("Allocating response failed");
                prov_client->response = NULL;
            }
            else
            {
                memset(prov_client->response, 0, content_len);
                memcpy(prov_client->response, content_str, content_len);
            }
        }

        //update HTTP state
        if (request_result == HTTP_CALLBACK_REASON_OK)
        {
            if (status_code >= 200 && status_code <= 299)
            {
                prov_client->http_state = HTTP_STATE_REQUEST_RECV;
            }
            else
            {
                prov_client->http_state = HTTP_STATE_ERROR;
            }
        }
        else
        {
            prov_client->http_state = HTTP_STATE_ERROR;
        }
    }
    else
    {
        LogError("Invalid callback context");
    }
}

static HTTP_HEADERS_HANDLE construct_http_headers(const PROV_SERVICE_CLIENT* prov_client, const char* etag, HTTP_CLIENT_REQUEST_TYPE request)
{
    HTTP_HEADERS_HANDLE result;
    if ((result = HTTPHeaders_Alloc()) == NULL)
    {
        LogError("failure sending request");
    }
    else
    {
        size_t secSinceEpoch = (size_t)(difftime(get_time(NULL), EPOCH_TIME_T_VALUE) + 0);
        size_t expiryTime = secSinceEpoch + SAS_TOKEN_DEFAULT_LIFETIME;

        STRING_HANDLE sas_token = SASToken_CreateString(prov_client->access_key, prov_client->provisioning_service_uri, prov_client->key_name, expiryTime);
        if (sas_token == NULL)
        {
            HTTPHeaders_Free(result);
            result = NULL;
        }
        else
        {
            if ((HTTPHeaders_AddHeaderNameValuePair(result, HEADER_KEY_USER_AGENT, HEADER_VALUE_USER_AGENT) != HTTP_HEADERS_OK) ||
                (HTTPHeaders_AddHeaderNameValuePair(result, HEADER_KEY_ACCEPT, HEADER_VALUE_ACCEPT) != HTTP_HEADERS_OK) ||
                ((request != HTTP_CLIENT_REQUEST_DELETE) && (HTTPHeaders_AddHeaderNameValuePair(result, HEADER_KEY_CONTENT_TYPE, HEADER_VALUE_CONTENT_TYPE) != HTTP_HEADERS_OK)) ||
                (HTTPHeaders_AddHeaderNameValuePair(result, HEADER_KEY_AUTHORIZATION, STRING_c_str(sas_token)) != HTTP_HEADERS_OK) ||
                ((etag != NULL) && (HTTPHeaders_AddHeaderNameValuePair(result, HEADER_KEY_IF_MATCH, etag) != HTTP_HEADERS_OK)))
            {
                LogError("failure adding header value");
                HTTPHeaders_Free(result);
                result = NULL;
            }
            STRING_delete(sas_token);
        }
    }
    return result;
}

static int add_query_headers(HTTP_HEADERS_HANDLE headers, size_t page_size, const char* cont_token)
{
    int result = 0;

    if (cont_token != NULL)
    {
        if (HTTPHeaders_AddHeaderNameValuePair(headers, HEADER_KEY_CONTINUATION, cont_token) != HTTP_HEADERS_OK)
        {
            LogError("Failure adding continuation token header");
            result = MU_FAILURE;
        }
    }

    if (result == 0)
    {
        if (page_size != NO_MAX_PAGE_SIZE)
        {
            char page_size_s[21]; //21 characters covers all 64bit numbers
            sprintf(page_size_s, "%d", (int)page_size);
            if (HTTPHeaders_AddHeaderNameValuePair(headers, HEADER_KEY_MAX_ITEM_COUNT, page_size_s) != HTTP_HEADERS_OK)
            {
                LogError("Failure adding max item count header");
                result = MU_FAILURE;
            }
        }
    }

    return result;
}

static STRING_HANDLE create_registration_path(const char* path_format, const char* id)
{
    STRING_HANDLE registration_path;

    if (id == NULL)
    {
        registration_path = STRING_construct(path_format);
    }
    else
    {
        STRING_HANDLE encoded_id;
        if ((encoded_id = URL_EncodeString(id)) == NULL)
        {
            LogError("Unable to URL encode ID");
            registration_path = NULL;
        }
        else
        {
            registration_path = STRING_construct_sprintf(path_format, STRING_c_str(encoded_id));
            STRING_delete(encoded_id);
        }
    }

    if (registration_path == NULL)
    {
        LogError("Failed constructing base path");
    }
    else if (STRING_sprintf(registration_path, API_VERSION_QUERY_PARAM, PROVISIONING_SERVICE_API_VERSION) != 0)
    {
        LogError("Unable to add query paramters");
        STRING_delete(registration_path);
        registration_path = NULL;
    }

    return registration_path;
}

static int get_response_headers(PROV_SERVICE_CLIENT* prov_client, char** cont_token_ptr, const char** resp_type_ptr)
{
    int result = 0;
    HTTP_HEADERS_HANDLE resp_headers = prov_client->response_headers;
    if (resp_headers == NULL)
    {
        LogError("Unable to retrieve headers");
        result = MU_FAILURE;
    }
    else
    {
        if (cont_token_ptr != NULL)
        {
            const char* cont_token = HTTPHeaders_FindHeaderValue(resp_headers, HEADER_KEY_CONTINUATION);
            if (cont_token != NULL)
            {
                if (mallocAndStrcpy_s(cont_token_ptr, cont_token) != 0)
                {
                    LogError("Failed to copy continuation token");
                    result = MU_FAILURE;
                }
            }
            else
            {
                *cont_token_ptr = NULL;
            }
        }

        if (resp_type_ptr != NULL)
        {
            *resp_type_ptr = HTTPHeaders_FindHeaderValue(resp_headers, HEADER_KEY_ITEM_TYPE);
        }
    }

    return result;
}

static HTTP_CLIENT_HANDLE connect_to_service(PROV_SERVICE_CLIENT* prov_client)
{
    HTTP_CLIENT_HANDLE result;

    // Create uhttp
    TLSIO_CONFIG tls_io_config;
    HTTP_PROXY_IO_CONFIG http_proxy;
    memset(&tls_io_config, 0, sizeof(TLSIO_CONFIG));
    tls_io_config.hostname = prov_client->provisioning_service_uri;
    tls_io_config.port = DEFAULT_HTTPS_PORT;

     // Setup proxy
     if (prov_client->proxy_options != NULL)
     {
         memset(&http_proxy, 0, sizeof(HTTP_PROXY_IO_CONFIG));
         http_proxy.hostname = prov_client->provisioning_service_uri;
         http_proxy.port = DEFAULT_HTTPS_PORT;
         http_proxy.proxy_hostname = prov_client->proxy_options->host_address;
         http_proxy.proxy_port = prov_client->proxy_options->port;
         http_proxy.username = prov_client->proxy_options->username;
         http_proxy.password = prov_client->proxy_options->password;

         tls_io_config.underlying_io_interface = http_proxy_io_get_interface_description();
         tls_io_config.underlying_io_parameters = &http_proxy;
     }

    const IO_INTERFACE_DESCRIPTION* interface_desc = platform_get_default_tlsio();
    if (interface_desc == NULL)
    {
        LogError("platform default tlsio is NULL");
        result = NULL;
    }
    else if ((result = uhttp_client_create(interface_desc, &tls_io_config, on_http_error, prov_client)) == NULL)
    {
        LogError("Failed creating http object");
    }
    else if (prov_client->certificate != NULL && uhttp_client_set_trusted_cert(result, prov_client->certificate) != HTTP_CLIENT_OK)
    {
         LogError("Failed setting trusted cert");
         uhttp_client_destroy(result);
         result = NULL;
     }
    else if (prov_client->tracing == TRACING_STATUS_ON && uhttp_client_set_trace(result, true, true) != HTTP_CLIENT_OK)
    {
        LogError("Failed setting trace");
        uhttp_client_destroy(result);
        result = NULL;
    }
    else if (uhttp_client_open(result, prov_client->provisioning_service_uri, DEFAULT_HTTPS_PORT, on_http_connected, prov_client) != HTTP_CLIENT_OK)
    {
        LogError("Failed opening http url %s", prov_client->provisioning_service_uri);
        uhttp_client_destroy(result);
        result = NULL;
    }
    return result;
}

static int rest_call(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, HTTP_CLIENT_REQUEST_TYPE operation, const char* registration_path, HTTP_HEADERS_HANDLE request_headers, const char* content)
{
    int result;
    size_t content_len;
    HTTP_CLIENT_HANDLE http_client;

    if (content == NULL)
    {
        content_len = 0;
    }
    else
    {
        content_len = strlen(content);
    }

    http_client = connect_to_service(prov_client);
    if (http_client == NULL)
    {
        LogError("Failed connecting to service");
        result = MU_FAILURE;
    }
    else
    {
        result = 0;
        do
        {
            uhttp_client_dowork(http_client);
            if (prov_client->http_state == HTTP_STATE_CONNECTED)
            {
                if (uhttp_client_execute_request(http_client, operation, registration_path, request_headers, (unsigned char*)content, content_len, on_http_reply_recv, prov_client) != HTTP_CLIENT_OK)
                {
                    LogError("Failure executing http request");
                    prov_client->http_state = HTTP_STATE_ERROR;
                    result = MU_FAILURE;
                }
                else
                {
                    prov_client->http_state = HTTP_STATE_REQUEST_SENT;
                }
            }
            else if (prov_client->http_state == HTTP_STATE_REQUEST_RECV)
            {
                prov_client->http_state = HTTP_STATE_COMPLETE;
            }
            else if (prov_client->http_state == HTTP_STATE_ERROR)
            {
                result = MU_FAILURE;
                LogError("HTTP error");
            }
        } while (prov_client->http_state != HTTP_STATE_COMPLETE && prov_client->http_state != HTTP_STATE_ERROR);

        uhttp_client_close(http_client, NULL, NULL);
        uhttp_client_destroy(http_client);
    }

    prov_client->http_state = HTTP_STATE_DISCONNECTED;
    return result;
}

static void clear_response(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client)
{
    free(prov_client->response);
    prov_client->response = NULL;
    HTTPHeaders_Free(prov_client->response_headers);
    prov_client->response_headers = NULL;
}

static int prov_sc_create_or_update_record(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, void** handle_ptr, HANDLE_FUNCTION_VECTOR vector, const char* path_format)
{
    int result = 0;
    void* handle;

    if (prov_client == NULL)
    {
        LogError("Invalid Provisioning Client Handle");
        result = MU_FAILURE;
    }
    else if ((handle_ptr == NULL) || ((handle = *handle_ptr) == NULL))
    {
        LogError("Invalid handle");
        result = MU_FAILURE;
    }
    else
    {
        char* content;
        if ((content = vector.serializeToJson(handle)) == NULL)
        {
            LogError("Failure serializing enrollment");
            result = MU_FAILURE;
        }
        else
        {
            STRING_HANDLE registration_path = NULL;
            const char* id = NULL;
            if ((id = vector.getId(handle)) == NULL)
            {
                LogError("Given model does not have a valid ID");
                result = MU_FAILURE;
            }
            else if ((registration_path = create_registration_path(path_format, id)) == NULL)
            {
                LogError("Failed to construct a registration path");
                result = MU_FAILURE;
            }
            else
            {
                HTTP_HEADERS_HANDLE request_headers;
                if ((request_headers = construct_http_headers(prov_client, vector.getEtag(handle), HTTP_CLIENT_REQUEST_PUT)) == NULL)
                {
                    LogError("Failure constructing headers");
                    result = MU_FAILURE;
                }
                else
                {
                    result = rest_call(prov_client, HTTP_CLIENT_REQUEST_PUT, STRING_c_str(registration_path), request_headers, content);

                    if (result == 0)
                    {
                        INDIVIDUAL_ENROLLMENT_HANDLE new_handle;
                        if ((new_handle = vector.deserializeFromJson(prov_client->response)) == NULL)
                        {
                            LogError("Failure constructing new enrollment structure from json response");
                            result = MU_FAILURE;
                        }

                        //Free the user submitted enrollment, and replace the pointer reference to a new enrollment from the provisioning service
                        vector.destroy(handle);
                        *handle_ptr = new_handle;
                    }
                    else
                    {
                        LogError("Rest call failed");
                    }
                    clear_response(prov_client);
                }
                HTTPHeaders_Free(request_headers);
            }
            STRING_delete(registration_path);
        }
        free(content);
    }

    return result;
}

static int prov_sc_delete_record_by_param(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, const char* id, const char* etag, const char* path_format)
{
    int result = 0;

    if (prov_client == NULL)
    {
        LogError("Invalid Provisioning Client Handle");
        result = MU_FAILURE;
    }
    else if (id == NULL)
    {
        LogError("Invalid Id");
        result = MU_FAILURE;
    }
    else
    {
        STRING_HANDLE registration_path = create_registration_path(path_format, id);
        if (registration_path == NULL)
        {
            LogError("Failed to construct a registration path");
            result = MU_FAILURE;
        }
        else
        {
            HTTP_HEADERS_HANDLE request_headers;
            if ((request_headers = construct_http_headers(prov_client, etag, HTTP_CLIENT_REQUEST_DELETE)) == NULL)
            {
                LogError("Failure constructing http headers");
                result = MU_FAILURE;
            }
            else
            {
                result = rest_call(prov_client, HTTP_CLIENT_REQUEST_DELETE, STRING_c_str(registration_path), request_headers, NULL);
                clear_response(prov_client);
            }
            HTTPHeaders_Free(request_headers);
        }
        STRING_delete(registration_path);
    }

    return result;
}

static int prov_sc_get_record(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, const char* id, void** handle_ptr, HANDLE_FUNCTION_VECTOR vector, const char* path_format)
{
    int result = 0;

    if (prov_client == NULL)
    {
        LogError("Invalid Provisioning Client Handle");
        result = MU_FAILURE;
    }
    else if (id == NULL)
    {
        LogError("Invalid id");
        result = MU_FAILURE;
    }
    else if (handle_ptr == NULL)
    {
        LogError("Invalid handle");
        result = MU_FAILURE;
    }
    else
    {
        STRING_HANDLE registration_path = create_registration_path(path_format, id);
        if (registration_path == NULL)
        {
            LogError("Failed to construct a registration path");
            result = MU_FAILURE;
        }
        else
        {
            HTTP_HEADERS_HANDLE request_headers;
            if ((request_headers = construct_http_headers(prov_client, NULL, HTTP_CLIENT_REQUEST_GET)) == NULL)
            {
                LogError("Failure constructing http headers");
                result = MU_FAILURE;
            }
            else
            {
                result = rest_call(prov_client, HTTP_CLIENT_REQUEST_GET, STRING_c_str(registration_path), request_headers, NULL);

                if (result == 0)
                {
                    void* handle;
                    if ((handle = vector.deserializeFromJson(prov_client->response)) == NULL)
                    {
                        LogError("Failure constructing new enrollment structure from json response");
                        result = MU_FAILURE;
                    }
                    *handle_ptr = handle;
                }
                clear_response(prov_client);
            }
            HTTPHeaders_Free(request_headers);
        }
        STRING_delete(registration_path);
    }

    return result;
}

static int prov_sc_run_bulk_operation(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, PROVISIONING_BULK_OPERATION* bulk_op, PROVISIONING_BULK_OPERATION_RESULT** bulk_res_ptr , const char* path_format)
{
    int result = 0;

    if (prov_client == NULL)
    {
        LogError("Invalid Provisioning Client Handle");
        result = MU_FAILURE;
    }
    else if (bulk_op == NULL)
    {
        LogError("Invalid Bulk Op");
        result = MU_FAILURE;
    }
    else if (bulk_op->version != PROVISIONING_BULK_OPERATION_VERSION_1)
    {
        LogError("Invalid Bulk Op Version #");
        result = MU_FAILURE;
    }
    else if (bulk_res_ptr == NULL)
    {
        LogError("Invalid Bulk Op Result pointer");
        result = MU_FAILURE;
    }
    else
    {
        char* content;
        if ((content = bulkOperation_serializeToJson(bulk_op)) == NULL)
        {
            LogError("Failure serializing bulk operation");
            result = MU_FAILURE;
        }
        else
        {
            STRING_HANDLE registration_path = create_registration_path(path_format, NULL);
            if (registration_path == NULL)
            {
                LogError("Failed to construct a registration path");
                result = MU_FAILURE;
            }
            else
            {
                HTTP_HEADERS_HANDLE request_headers;
                if ((request_headers = construct_http_headers(prov_client, NULL, HTTP_CLIENT_REQUEST_POST)) == NULL)
                {
                    LogError("Failure constructing http headers");
                    result = MU_FAILURE;
                }
                else
                {
                    result = rest_call(prov_client, HTTP_CLIENT_REQUEST_POST, STRING_c_str(registration_path), request_headers, content);

                    if (result == 0)
                    {
                        if ((*bulk_res_ptr = bulkOperationResult_deserializeFromJson(prov_client->response)) == NULL)
                        {
                            LogError("Failure deserializing bulk operation result");
                            result = MU_FAILURE;
                        }
                    }
                    else
                    {
                        LogError("Rest call failed");
                    }
                    clear_response(prov_client);
                }
                HTTPHeaders_Free(request_headers);
            }
            STRING_delete(registration_path);
        }
        free(content);
    }

    return result;
}

static int prov_sc_query_records(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, PROVISIONING_QUERY_SPECIFICATION* query_spec, char** cont_token_ptr, PROVISIONING_QUERY_RESPONSE** query_res_ptr, const char* path_format)
{
    int result = 0;

    if (prov_client == NULL)
    {
        LogError("Invalid Provisioning Client Handle");
        result = MU_FAILURE;
    }
    else if (query_spec == NULL || query_spec->version != PROVISIONING_QUERY_SPECIFICATION_VERSION_1)
    {
        LogError("Invalid Query details");
        result = MU_FAILURE;
    }
    else if (cont_token_ptr == NULL)
    {
        LogError("Invalid Continuation Token pointer");
        result = MU_FAILURE;
    }
    else if (query_res_ptr == NULL)
    {
        LogError("Invalid Query Response pointer");
        result = MU_FAILURE;
    }
    else
    {
        char* content = NULL;

        //do not serialize the query specification if there is no query_string (i.e. DRS query)
        if ((query_spec->query_string != NULL) && ((content = querySpecification_serializeToJson(query_spec)) == NULL))
        {
            LogError("Failure serializing query specification");
            result = MU_FAILURE;
        }
        else
        {
            STRING_HANDLE registration_path = create_registration_path(path_format, query_spec->registration_id);
            if (registration_path == NULL)
            {
                LogError("Failed to construct a registration path");
                result = MU_FAILURE;
            }
            else
            {
                HTTP_HEADERS_HANDLE request_headers;
                if ((request_headers = construct_http_headers(prov_client, NULL, HTTP_CLIENT_REQUEST_POST)) == NULL)
                {
                    LogError("Failure constructing http headers");
                    result = MU_FAILURE;
                }
                else if ((add_query_headers(request_headers, query_spec->page_size, *cont_token_ptr)) != 0)
                {
                    LogError("Failure adding query headers");
                    result = MU_FAILURE;
                }
                else
                {
                    result = rest_call(prov_client, HTTP_CLIENT_REQUEST_POST, STRING_c_str(registration_path), request_headers, content);

                    if (result == 0)
                    {
                        const char* resp_type = NULL;
                        char* new_cont_token = NULL;
                        PROVISIONING_QUERY_TYPE type;

                        if (get_response_headers(prov_client, &new_cont_token, &resp_type) != 0)
                        {
                            LogError("Failure reading response headers");
                            result = MU_FAILURE;
                        }
                        else if ((type = queryType_stringToEnum(resp_type)) == QUERY_TYPE_INVALID)
                        {
                            LogError("Failure to parse response type");
                            result = MU_FAILURE;
                        }
                        else if ((*query_res_ptr = queryResponse_deserializeFromJson(prov_client->response, type)) == NULL)
                        {
                            LogError("Failure deserializing query response");
                            result = MU_FAILURE;
                        }
                        free(*cont_token_ptr);
                        *cont_token_ptr = new_cont_token;
                    }
                    else
                    {
                        LogError("Rest call failed");
                    }
                    clear_response(prov_client);
                }
                HTTPHeaders_Free(request_headers);
            }
            STRING_delete(registration_path);
        }
        free(content);
    }

    return result;
}

//Exposed functions below

void prov_sc_destroy(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client)
{
    if (prov_client != NULL)
    {
        free(prov_client->provisioning_service_uri);
        free(prov_client->key_name);
        free(prov_client->access_key);
        free(prov_client->response);
        HTTPHeaders_Free(prov_client->response_headers);
        free(prov_client->certificate);
        free(prov_client);
    }
}

PROVISIONING_SERVICE_CLIENT_HANDLE prov_sc_create_from_connection_string(const char* conn_string)
{
    PROV_SERVICE_CLIENT* result;

    if (conn_string == NULL)
    {
        LogError("Input parameter is NULL: conn_string");
        result = NULL;
    }
    else
    {
        STRING_HANDLE cs_string;
        if ((cs_string = STRING_construct(conn_string)) == NULL)
        {
            LogError("STRING_construct failed");
            result = NULL;
        }
        else
        {
            MAP_HANDLE connection_string_values_map;
            if ((connection_string_values_map = connectionstringparser_parse(cs_string)) == NULL)
            {
                LogError("Tokenizing conn_string failed");
                result = NULL;
            }
            else
            {
                const char* hostname = NULL;
                const char* key_name = NULL;
                const char* key = NULL;

                if ((hostname = Map_GetValueFromKey(connection_string_values_map, IOTHUBHOSTNAME)) == NULL)
                {
                    LogError("Couldn't find %s in conn_string", IOTHUBHOSTNAME);
                    result = NULL;
                }
                else if ((key_name = Map_GetValueFromKey(connection_string_values_map, IOTHUBSHAREDACESSKEYNAME)) == NULL)
                {
                    LogError("Couldn't find %s in conn_string", IOTHUBSHAREDACESSKEYNAME);
                    result = NULL;
                }
                else if ((key = Map_GetValueFromKey(connection_string_values_map, IOTHUBSHAREDACESSKEY)) == NULL)
                {
                    LogError("Couldn't find %s in conn_string", IOTHUBSHAREDACESSKEY);
                    result = NULL;
                }
                if (hostname == NULL || key_name == NULL || key == NULL)
                {
                    LogError("invalid parameter hostname: %p, key_name: %p, key: %p", hostname, key_name, key);
                    result = NULL;
                }
                else if ((result = malloc(sizeof(PROV_SERVICE_CLIENT))) == NULL)
                {
                    LogError("Allocation of provisioning service client failed");
                    result = NULL;
                }
                else
                {
                    memset(result, 0, sizeof(PROV_SERVICE_CLIENT));
                    if (mallocAndStrcpy_s(&result->provisioning_service_uri, hostname) != 0)
                    {
                        LogError("Failure allocating of provisioning service uri");
                        prov_sc_destroy(result);
                        result = NULL;
                    }
                    else if (mallocAndStrcpy_s(&result->key_name, key_name) != 0)
                    {
                        LogError("Failure allocating of keyname");
                        prov_sc_destroy(result);
                        result = NULL;
                    }
                    else if (mallocAndStrcpy_s(&result->access_key, key) != 0)
                    {
                        LogError("Failure allocating of access key");
                        prov_sc_destroy(result);
                        result = NULL;
                    }
                    else
                    {
                        result->tracing = TRACING_STATUS_OFF;
                    }
                }
                Map_Destroy(connection_string_values_map);
            }
        }
        STRING_delete(cs_string);
    }
    return result;
}

void prov_sc_set_trace(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, TRACING_STATUS status)
{
    if (prov_client != NULL)
    {
        prov_client->tracing = status;
    }
}

int prov_sc_set_certificate(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, const char* certificate)
{
    int result = 0;

    if (prov_client == NULL)
    {
        LogError("Invalid prov_client");
        result = MU_FAILURE;
    }
    else if (certificate == NULL)
    {
        free(prov_client->certificate);
        prov_client->certificate = NULL;
    }
    else if (mallocAndStrcpy_overwrite(&prov_client->certificate, (char*)certificate) != 0)
    {
        LogError("Failed allocating memory for certificate");
        result = MU_FAILURE;
    }

    return result;
}

int prov_sc_set_proxy(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, HTTP_PROXY_OPTIONS* proxy_options)
{
    int result = 0;

    if (prov_client == NULL)
    {
        LogError("Invalid prov_client");
        result = MU_FAILURE;
    }
    else if (proxy_options == NULL)
    {
        LogError("Invalid proxy options");
        result = MU_FAILURE;
    }
    else
    {
        if (proxy_options->host_address == NULL)
        {
            LogError("Null host address in proxy options");
            result = MU_FAILURE;
        }
        else if (((proxy_options->username == NULL) || (proxy_options->password == NULL))
            && (proxy_options->username != proxy_options->password))
        {
            LogError("Only one of username and password for proxy settings was NULL");
            result = MU_FAILURE;
        }
        else
        {
            prov_client->proxy_options = proxy_options;
        }
    }

    return result;
}

int prov_sc_create_or_update_individual_enrollment(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, INDIVIDUAL_ENROLLMENT_HANDLE* enrollment_ptr)
{
    return prov_sc_create_or_update_record(prov_client,(void**)enrollment_ptr, getVector_individualEnrollment(), INDV_ENROLL_PROVISION_PATH_FMT);
}

int prov_sc_delete_individual_enrollment(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, INDIVIDUAL_ENROLLMENT_HANDLE enrollment)
{
    return prov_sc_delete_record_by_param(prov_client, individualEnrollment_getRegistrationId(enrollment), individualEnrollment_getEtag(enrollment), INDV_ENROLL_PROVISION_PATH_FMT);
}

int prov_sc_delete_individual_enrollment_by_param(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, const char* reg_id, const char* etag)
{
    return prov_sc_delete_record_by_param(prov_client, reg_id, etag, INDV_ENROLL_PROVISION_PATH_FMT);
}

int prov_sc_get_individual_enrollment(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, const char* reg_id, INDIVIDUAL_ENROLLMENT_HANDLE* enrollment_ptr)
{
    return prov_sc_get_record(prov_client, reg_id, (void**)enrollment_ptr, getVector_individualEnrollment(), INDV_ENROLL_PROVISION_PATH_FMT);
}

int prov_sc_query_individual_enrollment(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, PROVISIONING_QUERY_SPECIFICATION* query_spec, char** cont_token_ptr, PROVISIONING_QUERY_RESPONSE** query_resp_ptr)
{
    return prov_sc_query_records(prov_client, query_spec, cont_token_ptr, query_resp_ptr, INDV_ENROLL_QUERY_PATH_FMT);
}

int prov_sc_run_individual_enrollment_bulk_operation(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, PROVISIONING_BULK_OPERATION* bulk_op, PROVISIONING_BULK_OPERATION_RESULT** bulk_res_ptr)
{
    return prov_sc_run_bulk_operation(prov_client, bulk_op, bulk_res_ptr, INDV_ENROLL_BULK_PATH_FMT);
}

int prov_sc_delete_device_registration_state(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, DEVICE_REGISTRATION_STATE_HANDLE reg_state)
{
    return prov_sc_delete_record_by_param(prov_client, deviceRegistrationState_getRegistrationId(reg_state), deviceRegistrationState_getEtag(reg_state), REG_STATE_PROVISION_PATH_FMT);
}

int prov_sc_delete_device_registration_state_by_param(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, const char* reg_id, const char* etag)
{
    return prov_sc_delete_record_by_param(prov_client, reg_id, etag, REG_STATE_PROVISION_PATH_FMT);
}

int prov_sc_get_device_registration_state(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, const char* reg_id, DEVICE_REGISTRATION_STATE_HANDLE* reg_state_ptr)
{
    return prov_sc_get_record(prov_client, reg_id, (void**)reg_state_ptr, getVector_registrationState(), REG_STATE_PROVISION_PATH_FMT);
}

int prov_sc_query_device_registration_state(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, PROVISIONING_QUERY_SPECIFICATION* query_spec, char** cont_token_ptr, PROVISIONING_QUERY_RESPONSE** query_resp_ptr)
{
    return prov_sc_query_records(prov_client, query_spec, cont_token_ptr, query_resp_ptr, REG_STATE_QUERY_PATH_FMT);
}

int prov_sc_create_or_update_enrollment_group(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, ENROLLMENT_GROUP_HANDLE* enrollment_ptr)
{
    return prov_sc_create_or_update_record(prov_client, (void**)enrollment_ptr, getVector_enrollmentGroup(), ENROLL_GROUP_PROVISION_PATH_FMT);
}

int prov_sc_delete_enrollment_group(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, ENROLLMENT_GROUP_HANDLE enrollment)
{
    return prov_sc_delete_record_by_param(prov_client, enrollmentGroup_getGroupId(enrollment), enrollmentGroup_getEtag(enrollment), ENROLL_GROUP_PROVISION_PATH_FMT);
}

int prov_sc_delete_enrollment_group_by_param(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, const char* group_id, const char* etag)
{
    return prov_sc_delete_record_by_param(prov_client, group_id, etag, ENROLL_GROUP_PROVISION_PATH_FMT);
}

int prov_sc_get_enrollment_group(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, const char* group_id, ENROLLMENT_GROUP_HANDLE* enrollment_ptr)
{
    return prov_sc_get_record(prov_client, group_id, (void**)enrollment_ptr, getVector_enrollmentGroup(), ENROLL_GROUP_PROVISION_PATH_FMT);
}

int prov_sc_query_enrollment_group(PROVISIONING_SERVICE_CLIENT_HANDLE prov_client, PROVISIONING_QUERY_SPECIFICATION* query_spec, char** cont_token_ptr, PROVISIONING_QUERY_RESPONSE** query_resp_ptr)
{
    return prov_sc_query_records(prov_client, query_spec, cont_token_ptr, query_resp_ptr, ENROLL_GROUP_QUERY_PATH_FMT);
}